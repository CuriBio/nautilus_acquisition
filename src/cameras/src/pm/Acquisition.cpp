/*
 * MIT License
 *
 * Copyright (c) 2022 Curi Bio
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/*********************************************************************
 * @file  Acquisition.cpp
 * 
 * @brief Implementation of the Acquisition class.
 *********************************************************************/
#include <cassert>
#include <chrono>
#include <thread>
#include <string>
#include <sstream>
#include <algorithm>
#include <fmt/core.h>
#include <spdlog/spdlog.h>

#include <interfaces/FrameInterface.h>
#include <interfaces/ColorConfigInterface.h>

#include <pm/Frame.h>
#include <pm/Acquisition.h>
#include <pm/ColorConfig.h>
#include <PMemCopy.h>
#include <TiffFile.h>
#include <RawFile.h>
#include <ThreadPool.h>

#ifdef _WIN64
#include <windows.h>
/*
* @brief Gets total system memory available.
* @return Total system memory in bytes.
*/
unsigned long long getTotalSystemMemory()
{
    MEMORYSTATUSEX status;
    status.dwLength = sizeof(status);
    GlobalMemoryStatusEx(&status);
    return status.ullTotalPhys;
}
#endif


/* THREADS */
template<FrameConcept F, ColorConfigConcept C>
void PV_DECL pm::Acquisition<F, C>::EofCallback(FRAME_INFO* frameInfo, void* ctx) noexcept {
    Acquisition* cls = static_cast<Acquisition*>(ctx);

    if (!frameInfo) {
        spdlog::error("Invalid Frame");
        return;
    } 

    F* frame = cls->m_unusedFramePool->Acquire();
    if (!cls->m_camera->GetLatestFrame(frame)) {
      //TODO handle error
      spdlog::error("GetLatestFrame failed");
      return;
    }

    //Check for skipped frames
    const uint32_t cbFrameNr = frameInfo->FrameNr;
    if (cls->m_lastFrameInCallback == 0) {// first frame in capture
        cls->m_lastFrameInCallback = cbFrameNr;
    } else {
        cls->checkLostFrame(cbFrameNr, cls->m_lastFrameInCallback, 0);
    }

    if (!frame) {
        //TODO handle error
        spdlog::error("Could not acquire unused frame from frame pool");
        return;
    }

    auto state = cls->GetState();
    //TODO if m_frameWriterQueue.size is valid
    {
        std::unique_lock<std::mutex> lock(cls->m_frameProcessingQueueLock);
        if (cbFrameNr % 1000 == 0) {
            spdlog::info("Current Frame ({}), framePoolSize: {}, writerQueue size: {}", cbFrameNr, cls->m_unusedFramePool->Size(), cls->m_frameProcessingQueue.size());
        }
        cls->m_frameProcessingQueue.push(frame);

    }
    cls->m_frameProcessingCond.notify_one();

    if (state == AcquisitionState::AcqCapture || state == AcquisitionState::AcqCaptureLiveScan) {
        ++cls->m_capturedFrames;
    }

    return;
}


template<FrameConcept F, ColorConfigConcept C>
void pm::Acquisition<F, C>::checkLostFrame(uint32_t frameN, uint32_t &lastFrame, uint8_t i) noexcept {
    std::unique_lock<std::mutex> lock(m_lock);
    if (frameN > lastFrame + 1) {
        //TODO keep frame stats for lost frames
        spdlog::warn("({}) Current Frame ({}), Last Frame ({}), framePoolSize: {}, writerQueue size: {}", i, frameN, lastFrame, m_unusedFramePool->Size(), m_frameProcessingQueue.size());
    }
    lastFrame = frameN;
}

template<FrameConcept F, ColorConfigConcept C>
void pm::Acquisition<F, C>::writeFrame(F* frame) noexcept {
    //TODO support different storage types
    switch (m_camera->ctx->curExp->storageType) {
        case StorageType::Tiff:
        case StorageType::BigTiff: //TODO Actually handle this
        {
            std::string file = fmt::format("{}{:04}.tiff", m_camera->ctx->curExp->filePrefix, m_frameIndex);
            std::string path = (m_camera->ctx->curExp->acquisitionDir / "data" / file).string();

            TiffFile tiff(
                m_camera->ctx->curExp->region,
                m_camera->ctx->info.imageFormat,
                m_camera->ctx->info.spdTable[m_camera->ctx->curExp->spdTableIdx].bitDepth,
                (m_camera->ctx->curExp->storageType == StorageType::TiffStack) ? m_camera->ctx->curExp->frameCount : 1
            );

            tiff.Open(path);
            tiff.WriteFrame<F>(frame);
            m_unusedFramePool->Release(frame);
            tiff.Close();
        }
        break;
        case StorageType::Raw:
        {
            std::string file = fmt::format("{}{:04}.raw", m_camera->ctx->curExp->filePrefix, m_frameIndex);
            std::filesystem::path rawpath = (m_camera->ctx->curExp->acquisitionDir / "data" / file);

            uint32_t width = (m_camera->ctx->curExp->region.s2 - m_camera->ctx->curExp->region.s1 + 1) / m_camera->ctx->curExp->region.sbin;
            uint32_t height = (m_camera->ctx->curExp->region.p2 - m_camera->ctx->curExp->region.p1 + 1) / m_camera->ctx->curExp->region.pbin;

            RawFile<4> raw(
                rawpath,
                16, //bitdepth
                width,
                height
            );

            raw.Write(frame->GetData(), 0);
            m_unusedFramePool->Release(frame);
            raw.Close();
        }
        break;
    }
    frame = nullptr;

    return;
}

template<FrameConcept F, ColorConfigConcept C>
void pm::Acquisition<F, C>::frameProcessingThread() noexcept {
    m_frameIndex = 0;
    F* frame{nullptr};
    m_running = true;
    bool captured = false;

    if (!m_camera->StartExp((void*)&pm::Acquisition<F, C>::EofCallback, this)) {
        //TODO handle error
        spdlog::error("frameProcessingThread StartExp failed");
        m_running = false;
        return;
    }

    m_lastFrameInProcessing = 0;
    m_frameProcessingReadyCond.notify_all();

    do {
        {
            std::unique_lock<std::mutex> lock(m_frameProcessingLock);
            if (m_frameProcessingQueue.empty()) {
                const bool timedOut = !m_frameProcessingCond.wait_for(
                            lock,
                            std::chrono::milliseconds(200),
                            [this]() { return !m_frameProcessingQueue.empty(); }
                            );

                if (timedOut) {
                    //TODO handle timeout
                    continue;
                }
            }
        } //release lock

        {
            std::unique_lock<std::mutex> lock(m_frameProcessingQueueLock);
            frame = m_frameProcessingQueue.front();
            m_frameProcessingQueue.pop();
        }

        const uint32_t frameNr = frame->GetInfo()->frameNr;
        if (m_lastFrameInProcessing == 0) {
            spdlog::info("Syncing frame number {}, {}", m_lastFrameInProcessing, frameNr);
            m_lastFrameInProcessing = frameNr;
        } else if (frameNr <= m_lastFrameInProcessing) { //sync frame number
            //TODO log stats on dropped frame
            spdlog::error("Frame number out of order: {}, last frame number was {}, ignoring", frameNr, m_lastFrameInProcessing);

            // Drop frame for invalid frame number
            m_lastFrameInProcessing = frameNr;
        }

        // Check to make sure we didn't skip a frame
        checkLostFrame(frameNr, m_lastFrameInProcessing, 1);

        //copy frame
        if (!frame->CopyData()) {
            spdlog::info("Failed to copy frame data");
            return;
        }
        {
            std::unique_lock<std::mutex> lock(m_lock);
            m_latestFrame = frame;
        }

        switch (m_state) {
            case AcquisitionState::AcqStopped: {
                spdlog::info("Shutting down acquisition");
                m_diskThreadAbortFlag = true;
                continue;
            }
            case AcquisitionState::AcqIdle:
            case AcquisitionState::AcqLiveScan: { //live view only, release frame
                m_unusedFramePool->Release(frame);
                frame = nullptr;
                continue;
            }
            case AcquisitionState::AcqCaptureLiveScan:
            case AcquisitionState::AcqCapture: {
                if (m_frameIndex >= m_camera->ctx->curExp->frameCount) {
                    spdlog::info("m_frameIndex > frameCount");
                    m_unusedFramePool->Release(frame);
                    frame = nullptr;

                    if (m_state == AcquisitionState::AcqCaptureLiveScan) {
                        m_state = AcquisitionState::AcqLiveScan;
                    } else {
                        m_state = AcquisitionState::AcqIdle;
                    }

                    spdlog::info("Notify acquisition finished");
                    std::unique_lock<std::mutex> lock(m_acquisitionFinishedLock);
                    m_acquisitionFinishedCond.notify_all();
                    m_hasNotified = true;
                } else if (m_frameIndex < m_camera->ctx->curExp->frameCount) {
                    if (m_processFn) {
                        FrameCtx frameCtx {
                            .width = (m_camera->ctx->curExp->region.s2 - m_camera->ctx->curExp->region.s1 + 1) / m_camera->ctx->curExp->region.sbin,
                            .height = (m_camera->ctx->curExp->region.p2 - m_camera->ctx->curExp->region.p1 + 1) / m_camera->ctx->curExp->region.pbin,
                            .index = m_frameIndex,
                            .bitDepth = 16, //TODO support other bitdepths
                            .path = (m_camera->ctx->curExp->acquisitionDir / "data" / fmt::format("{}{:04}.raw", m_camera->ctx->curExp->filePrefix, m_frameIndex)),
                        };

                        m_processFn(&frameCtx, frame);
                    }

                    m_unusedFramePool->Release(frame);
                    ++m_frameIndex;
                    frame = nullptr;

                    if (m_progress) { m_progress(1); }
                    captured = true;
                }
            }
        }

    } while (!m_diskThreadAbortFlag);

    spdlog::info("Acquisition finished. flag: {}, frameIndex: {}, frameCount: {}", m_diskThreadAbortFlag, m_frameIndex, m_camera->ctx->curExp->frameCount);
    m_camera->StopExp();

    while (!m_frameProcessingQueue.empty()) {
        {
            std::unique_lock<std::mutex> lock(m_frameProcessingQueueLock);
            frame = m_frameProcessingQueue.front();
            m_frameProcessingQueue.pop();
        }

        if (captured) {
            //copy frame
            if (!frame->CopyData()) {
                spdlog::info("Failed to copy frame data");
                return;
            }

            if (m_frameIndex < m_camera->ctx->curExp->frameCount) {
                if (m_processFn) {
                    FrameCtx frameCtx {
                        .width = (m_camera->ctx->curExp->region.s2 - m_camera->ctx->curExp->region.s1 + 1) / m_camera->ctx->curExp->region.sbin,
                        .height = (m_camera->ctx->curExp->region.p2 - m_camera->ctx->curExp->region.p1 + 1) / m_camera->ctx->curExp->region.pbin,
                        .index = m_frameIndex,
                        .bitDepth = 16, //TODO support other bitdepths
                        .path = (m_camera->ctx->curExp->acquisitionDir / "data" / fmt::format("{}{:04}.raw", m_camera->ctx->curExp->filePrefix, m_frameIndex)),
                    };

                    m_processFn(&frameCtx, frame);
                }

                m_unusedFramePool->Release(frame);
                ++m_frameIndex;

                if (m_progress) { m_progress(1); }
            }
        } else {
            m_unusedFramePool->Release(frame);
        }
    }

    m_processFn = nullptr;
    m_progress = nullptr;
    m_lastFrameInProcessing = 0;
}

template<FrameConcept F, ColorConfigConcept C>
pm::Acquisition<F, C>::Acquisition(std::shared_ptr<pm::Camera<F>> camera) : m_camera(camera), m_running(false) {
    m_pCopy = std::make_shared<PMemCopy>();
    m_parTask = std::make_shared<ParTask>(3);

    //TODO preallocate unused frame pool size based on acquisition size
    const uint64_t bufferCount = m_camera->ctx->curExp->bufferCount;
    const uint64_t frameCount = m_camera->ctx->curExp->frameCount;
    const uint32_t frameBytes = m_camera->ctx->frameBytes;

#ifdef _WIN64
    spdlog::info("Total system memory: {}", getTotalSystemMemory());
    m_framesMax = uint64_t((0.9 * getTotalSystemMemory())  / uint64_t(frameBytes));
#else
    m_framesMax = (0xFFFFFFFF - 1) / frameBytes;
#endif

    //TODO parameterize min value
    spdlog::info("Initializing frame pool with {} objects of size {}", std::min<uint64_t>(1000, frameCount), frameBytes);
    m_unusedFramePool = std::make_unique<FramePool<F>>(std::min<uint64_t>(1000, m_framesMax), frameBytes, true, m_parTask);

    spdlog::info("Get speed table {}", m_camera->ctx->curExp->spdTableIdx);
    uint16_t spdTblIdx = m_camera->ctx->curExp->spdTableIdx;
    m_spdTable = m_camera->ctx->info.spdTable[spdTblIdx];

    m_latestFrame = new Frame(frameBytes, true, m_parTask);
}


template<FrameConcept F, ColorConfigConcept C>
pm::Acquisition<F, C>::~Acquisition() {
    //TODO cleanup
}


template<FrameConcept F, ColorConfigConcept C>
void pm::Acquisition<F, C>::StartAcquisition(std::function<void(size_t)> progressCB, std::function<void(FrameCtx*, F*)> process) {
    std::unique_lock<std::mutex> lock(m_lock);

    m_progress = progressCB;
    m_processFn = process;

    m_hasNotified = false;
    m_frameIndex = 0;
    m_capturedFrames = 0;

    m_unusedFramePool->EnsurePoolSize(std::min<uint64_t>(m_camera->ctx->curExp->frameCount, m_framesMax));
    startProcessingThread();
}


template<FrameConcept F, ColorConfigConcept C>
void pm::Acquisition<F, C>::StartLiveView() {
    std::unique_lock<std::mutex> lock(m_lock);
    spdlog::info("Starting processing thread for live view");
    startProcessingThread();
}


template<FrameConcept F, ColorConfigConcept C>
void pm::Acquisition<F, C>::startProcessingThread() noexcept {
    switch (m_state) {
        case AcquisitionState::AcqIdle:
        case AcquisitionState::AcqStopped:
            m_state = AcquisitionState::AcqCapture;
            break;
        case AcquisitionState::AcqLiveScan:
            m_state = AcquisitionState::AcqCaptureLiveScan;
            break;
        default:
            break;
    }

    if (!m_frameProcessingThread) {
        std::unique_lock<std::mutex> lock(m_frameProcessingReadyLock);
        spdlog::info("Starting frame processing thread: frameCount {}", m_camera->ctx->curExp->frameCount);
        m_frameProcessingThread = new std::thread(&Acquisition<F, C>::frameProcessingThread, this);
        m_frameProcessingReadyCond.wait(lock);
    } else {
        spdlog::info("Starting capture");
        if (!m_camera->StartExp((void*)&pm::Acquisition<F, C>::EofCallback, this)) {
            //TODO handle error
            spdlog::error("StartExp failed");
        }
    }
}

template<FrameConcept F, ColorConfigConcept C>
void pm::Acquisition<F, C>::StopCapture() {
    switch (m_state) {
        case AcquisitionState::AcqCapture: {
            StopAll();
            return;
        }
        case AcquisitionState::AcqCaptureLiveScan: {
            m_state = AcquisitionState::AcqLiveScan;
            return;
        }
        case AcquisitionState::AcqLiveScan: {
            return;
        }
        default: {
            return;
        }
    }
}

template<FrameConcept F, ColorConfigConcept C>
void pm::Acquisition<F, C>::StopLiveView() {
    switch (m_state) {
        case AcquisitionState::AcqCapture: {
            return;
        }
        case AcquisitionState::AcqCaptureLiveScan: {
            m_state = AcquisitionState::AcqCapture;
            return;
        }
        case AcquisitionState::AcqLiveScan: {
            StopAll();
            return;
        }
        default: {
            return;
        }
    }

}

template<FrameConcept F, ColorConfigConcept C>
void pm::Acquisition<F, C>::StopAll() {
    m_diskThreadAbortFlag = true;
    m_state = AcquisitionState::AcqStopped;
    m_frameProcessingCond.notify_one();
}

template<FrameConcept F, ColorConfigConcept C>
void pm::Acquisition<F, C>::WaitForStop() {
    std::unique_lock<std::mutex> lock(m_stopLock);
    if (m_frameProcessingThread && m_running) {
        m_frameProcessingCond.notify_one();
        m_frameProcessingThread->join();

        m_frameProcessingThread = nullptr;
        m_running = false;
    } else {
        spdlog::error("m_frameProcessingThread not running");
        
    }

    m_camera->StopExp();

    m_capturedFrames = 0;
    m_state = AcquisitionState::AcqStopped;
    m_lastFrameInProcessing = m_lastFrameInCallback;

    m_diskThreadAbortFlag = false;
    m_frameProcessingThread = nullptr;
    m_running = false;

    return;
}

template<FrameConcept F, ColorConfigConcept C>
void pm::Acquisition<F, C>::WaitForAcquisition() {
    std::unique_lock<std::mutex> lock(m_acquisitionFinishedLock);
    m_acquisitionFinishedCond.wait(lock);
}

template<FrameConcept F, ColorConfigConcept C>
F* pm::Acquisition<F,C>::GetLatestFrame() {
    std::unique_lock<std::mutex> lock(m_lock);
    return m_latestFrame;
}

template<FrameConcept F, ColorConfigConcept C>
void pm::Acquisition<F,C>::SetLatestFrame(F* frame) {
    std::unique_lock<std::mutex> lock(m_lock);
    m_latestFrame = frame;
}

template<FrameConcept F, ColorConfigConcept C>
bool pm::Acquisition<F, C>::IsRunning() {
    return m_running;
}

template<FrameConcept F, ColorConfigConcept C>
AcquisitionState pm::Acquisition<F, C>::GetState() {
    return m_state;
}

template<FrameConcept F, ColorConfigConcept C>
void pm::Acquisition<F,C>::LoadTestData(std::string testImgPath) {
    uint32_t width, height;
    m_testImgPath = testImgPath;
    std::filesystem::path p = testImgPath.c_str();
    m_fakeData = TiffFile::LoadTIFF(p.string().c_str(), width, height);
}

//Avoid link errors
template class pm::Acquisition<pm::Frame, pm::ColorConfig<ph_color_context>>;
