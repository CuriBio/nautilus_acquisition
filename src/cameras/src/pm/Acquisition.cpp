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
 * @file  Acquistion.cpp
 * 
 * @brief Implementation of the Acquistion class.
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
void PV_DECL pm::Acquisition<F, C>::EofCallback(FRAME_INFO* frameInfo, void* ctx) {
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
        std::unique_lock<std::mutex> lock(cls->m_frameWriterLock);
        cls->m_frameWriterQueue.push(frame);
    }

    if (state == AcquisitionState::AcqCapture) {
        //queue frame for processing
        uint32_t frameCount = cls->m_camera->ctx->curExp->frameCount;
        cls->m_capturedFrames++;

        if (cls->m_capturedFrames >= frameCount) {
            //Got all requested frames
            cls->m_camera->StopExp();
        }
        //notify thread
    }

    cls->m_frameWriterCond.notify_one();

    return;
}


template<FrameConcept F, ColorConfigConcept C>
void pm::Acquisition<F, C>::checkLostFrame(uint32_t frameN, uint32_t &lastFrame, uint8_t i) {
    std::unique_lock<std::mutex> lock(m_lock);
    if (frameN > lastFrame + 1) {
        //TODO keep frame stats for lost frames
        // Log all the frame numbers we missed
        for (uint32_t nr = lastFrame + 1; nr < frameN; nr++) {
            spdlog::info("({}) Current Frame ({}), Lost frame # {}", i, frameN, nr);
        }
    }
    lastFrame = frameN;
}


template<FrameConcept F, ColorConfigConcept C>
void pm::Acquisition<F, C>::frameWriterThread() {
    size_t frameIndex = 0;
    size_t startFrame = 0;

    F* frame{nullptr};

    std::string fileName = m_camera->ctx->curExp->filePrefix;
    std::filesystem::path filePath = m_camera->ctx->curExp->filePath;

    TiffFile<F>* file = new TiffFile<F>(
        m_camera->ctx->curExp->region,
        m_camera->ctx->info.imageFormat,
        m_camera->ctx->info.spdTable[m_camera->ctx->curExp->spdTableIdx].bitDepth,
        (m_camera->ctx->curExp->storageType == StorageType::TiffStack) ? m_camera->ctx->curExp->frameCount : 1
    );

    spdlog::info("Capture mode is enabled: {}", (m_state == AcquisitionState::AcqCapture) ? "true" : "false");
    if (!m_camera->StartExp((void*)&pm::Acquisition<F, C>::EofCallback, this)) {
        //TODO handle error
        spdlog::error("StartExp failed");
    }

    if (m_fakeData) {
        spdlog::warn("Using test image data from {}", m_testImgPath);
    }
    m_running = true;

    do {
        {
            std::unique_lock<std::mutex> lock(m_frameWriterLock);
            if (m_capturedFrames < m_camera->ctx->curExp->frameCount) {
                const bool timedOut = !m_frameWriterCond.wait_for(
                            lock,
                            std::chrono::milliseconds(5000),
                            [this]() { return !m_frameWriterQueue.empty(); }
                            );

                if (timedOut) {
                    //TODO handle timeout
                    spdlog::info("m_frameWriterCond timeout");
                    continue;
                }
            }

            frame = m_frameWriterQueue.front();
            assert(frame != nullptr && file != nullptr);
            m_frameWriterQueue.pop();
        } //release lock

        const uint32_t frameNr = frame->GetInfo()->frameNr;
        if (m_lastFrameInProcessing == 0) {
            m_lastFrameInProcessing = frameNr;
        } else if (frameNr <= m_lastFrameInProcessing) {
            //TODO log stats on dropped frame
            spdlog::error("Frame number out of order: {}, last frame number was {}, ignoring", frameNr, m_lastFrameInProcessing);

            // Drop frame for invalid frame number
            m_lastFrameInProcessing = frameNr;
        }

        // Check to make sure we didn't skip a frame
        checkLostFrame(frameNr, m_lastFrameInProcessing, 1);

        switch (m_state) {
            case AcquisitionState::AcqCapture:
                {
                    if (m_fakeData) {
                        frame->SetData((void*)m_fakeData);
                    }

                    //copy frame
                    if (!frame->CopyData()) {
                        spdlog::info("Failed to copy frame data");
                        continue;
                    }
                    {
                        std::unique_lock<std::mutex> lock(m_lock);
                        m_latestFrame = frame;
                    }

                    // make subdirectory to write to
                    if (!std::filesystem::exists(filePath)) {
                        std::filesystem::create_directories(filePath);
                        spdlog::info("Acquisition being written under directory: {}", filePath.string());
                    }

                    //TODO support different storage types
                    switch (m_camera->ctx->curExp->storageType) {
                        case StorageType::Tiff:
                        case StorageType::BigTiff: //TODO Actually handle this
                        case StorageType::Prd: //TODO Actually handle this
                        {
                            std::stringstream ss;
                            ss << std::setfill('0') << std::setw(3) << frameIndex;

                            if (file->IsOpen()) { file->Close(); }
                            file->Open((filePath / (fileName + ss.str() + ".tiff")).string());
                        }
                            break;
                        case StorageType::TiffStack:
                            if (frameIndex == 0) {
                                file->Open((filePath / (fileName + ".tiff")).string());
                            }
                            break;
                    }
                    file->WriteFrame(frame);
                    frameIndex++;
                }
                break;
            case AcquisitionState::AcqLiveScan:
                {
                    if (m_fakeData) {
                        frame->SetData((void*)m_fakeData);
                    }

                    if (!frame->CopyData()) {
                        spdlog::info("Failed to copy frame data");
                        continue;
                    }
                    {
                        std::unique_lock<std::mutex> lock(m_lock);
                        m_latestFrame = frame;
                    }
                }
                break;
            case AcquisitionState::AcqStopped:
                spdlog::info("Shutting down acquisition");
                break;
            default:
                spdlog::error("Error: AcquisitionState");
                break;
        }

        m_unusedFramePool->Release(frame);
        frame = nullptr;
    } while (m_state == AcquisitionState::AcqLiveScan || (!m_diskThreadAbortFlag && (frameIndex < m_camera->ctx->curExp->frameCount)));

    spdlog::info("Acquisition finished. flag: {}, frameIndex: {}, frameCount: {}", m_diskThreadAbortFlag, frameIndex, m_camera->ctx->curExp->frameCount);
    m_camera->StopExp();

    if (file) {
        file->Close();
        delete file;
        file = nullptr;
    }
}


template<FrameConcept F, ColorConfigConcept C>
pm::Acquisition<F, C>::Acquisition(std::shared_ptr<pm::Camera<F>> camera) : m_camera(camera), m_running(false) {
    m_pCopy = std::make_shared<PMemCopy>();
    m_parTask = std::make_shared<ParTask>(8);

    //TODO preallocate unused frame pool size based on acquisition size
    const uint64_t bufferCount = camera->ctx->curExp->bufferCount;
    const uint64_t frameCount = camera->ctx->curExp->frameCount;
    const uint32_t frameBytes = camera->ctx->frameBytes;

#ifdef _WIN64
    spdlog::info("Total system memory: {}", getTotalSystemMemory());
    uint64_t framesMax = uint64_t((0.9 * getTotalSystemMemory())  / uint64_t(frameBytes));
#else
    uint64_t framesMax = (0xFFFFFFFF - 1) / frameBytes;
#endif
    //TODO parameterize min value
    framesMax = std::min<uint64_t>(1000, framesMax); //Use 1000 frames max for now so startup isn't so slow
    //const uint64_t recommendedFrameCount = std::min<uint64_t>(10 + std::min<uint64_t>(frameCount, framesMax), bufferCount);

    spdlog::info("Initializing frame pool with {} objects of size {}", framesMax, frameBytes);
    m_unusedFramePool = std::make_unique<FramePool<F>>(framesMax, frameBytes, true, m_parTask);

    spdlog::info("Get speed table {}", camera->ctx->curExp->spdTableIdx);
    uint16_t spdTblIdx = camera->ctx->curExp->spdTableIdx;
    m_spdTable = camera->ctx->info.spdTable[spdTblIdx];

    m_latestFrame = new Frame(frameBytes, true, m_parTask);
}


template<FrameConcept F, ColorConfigConcept C>
pm::Acquisition<F, C>::~Acquisition() {
    //TODO cleanup
}


template<FrameConcept F, ColorConfigConcept C>
bool pm::Acquisition<F, C>::Start(bool saveToDisk, double tiffFillValue, const C* tiffColorCtx) {
    //TODO implement colorCtx support
    std::unique_lock<std::mutex> lock(m_lock);

    if (!m_running) {
        m_state = (saveToDisk) ? AcquisitionState::AcqCapture : AcquisitionState::AcqLiveScan;

        if (!m_frameWriterThread) {
            spdlog::info("Starting frame writer thread: frameCount {}", m_camera->ctx->curExp->frameCount);
            m_frameWriterThread = new std::thread(&Acquisition<F, C>::frameWriterThread, this);
        }

        if (m_frameWriterThread) {
            m_running = true;
            return true;
        }
    } else if (m_running && m_state == AcquisitionState::AcqLiveScan && saveToDisk) {
        //live view is running but not capturing so enable capture
        spdlog::info("Switching from LiveScan to Capture");
        m_state = AcquisitionState::AcqCapture;
        return true;
    } else if (m_running && m_state == AcquisitionState::AcqCapture) {
        spdlog::error("Acquisition with capture is already running");
        return false;
    }

    m_running = false;
    return false;
}


template<FrameConcept F, ColorConfigConcept C>
bool pm::Acquisition<F, C>::Stop() {
    m_diskThreadAbortFlag = true;
    m_state = AcquisitionState::AcqStopped;

    return true;
}


template<FrameConcept F, ColorConfigConcept C>
void pm::Acquisition<F, C>::WaitForStop() {
    std::unique_lock<std::mutex> lock(m_stopLock);
    if (m_frameWriterThread && m_running) {
        m_frameWriterThread->join();

        m_frameWriterThread = nullptr;
        m_running = false;
    }

    m_capturedFrames = 0;
    m_state = AcquisitionState::AcqStopped;
    m_lastFrameInProcessing = 0;
    m_lastFrameInCallback = 0;
    m_diskThreadAbortFlag = false;

    return;
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
    m_fakeData = TiffFile<F>::LoadTIFF(p.string().c_str(), width, height);
}

//Avoid link errors
template class pm::Acquisition<pm::Frame, pm::ColorConfig<ph_color_context>>;
