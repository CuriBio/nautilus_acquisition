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

//#include <immintrin.h>

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
        std::unique_lock<std::mutex> lock(cls->m_frameQueueLock);
        if (cbFrameNr % 1000 == 0) {
            spdlog::info("Current Frame ({}), framePoolSize: {}, writerQueue size: {}", cbFrameNr, cls->m_unusedFramePool->Size(), cls->m_frameWriterQueue.size());
        }
        cls->m_frameWriterQueue.push(frame);

    }
    cls->m_frameWriterCond.notify_one();

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
        // Log all the frame numbers we missed
        spdlog::warn("({}) Current Frame ({}), Last Frame ({}), framePoolSize: {}, writerQueue size: {}", i, frameN, lastFrame, m_unusedFramePool->Size(), m_frameWriterQueue.size());
        // for (uint32_t nr = lastFrame + 1; nr < frameN; nr++) {
        //     spdlog::warn("({}) Current Frame ({}), Lost frame # {}", i, frameN, nr);
        // }
    }
    lastFrame = frameN;
}

// inline static double roiAverage(uint16_t* data, size_t row_width) {
//     uint32_t total = 0;
//
//     __m256i vecSum = _mm256_setzero_si256();
//
//     for (size_t n = 0; n < 64; n+=4) {
//         for (size_t i = 0; i < 4; i++) {
//             size_t offset = (n+i)*row_width;
//             __m256i vec0 = _mm256_load_si256((const __m256i*)(data+offset); 
//             vecSum = _mm256_add_epi16(vecSum, vec0);
//
//             __m256i vec1 = _mm256_load_si256((const __m256i*)(data+offset+16);
//             vecSum = _mm256_add_epi16(vecSum, vec1);
//
//             __m256i vec2 = _mm256_load_si256((const __m256i*)(data+offset+32);
//             vecSum = _mm256_add_epi16(vecSum, vec2);
//
//             __m256i vec3 = _mm256_load_si256((const __m256i*)(data+offset+48);
//             vecSum = _mm256_add_epi16(vecSum, vec3);
//         }
//
//         __m128i hi128 = _mm256_extracti128_si256(vecSum, 1);
//         __m128i lo128 = _mm256_castsi256_si128(vecSum);
//
//         __m256i hi32_256 = _mm256_cvtepi16_epi32(hi128);
//         __m256i lo32_256 = _mm256_cvtepi16_epi32(lo128);
//         __m256i sum256 = _mm256_add_epi32(lo32_256, hi32_256);
//
//         __m128i sum128 = _mm_add_epi32(
//             _mm256_castsi256_si128(sum256),
//             _mm256_extracti128_si256(sum256, 1));
//
//         __m128i hi64 = _mm_unpackhi_epi64(sum128, sum128);
//         __m128i sum64 = _mm_add_epi32(hi64, sum128);
//         __m128i hi32 = _mm_shuffle_epi32(sum64, _MM_SHUFFLE(2, 3, 0, 1));
//         __m128i sum32 = _mm_add_epi32(sum64, hi32);
//
//         total += _mm_cvtsi128_si32(sum32);
//     }
//     return total / 4096.0;
// } 

template<FrameConcept F, ColorConfigConcept C>
void pm::Acquisition<F, C>::processFrame(F* frame) noexcept {
    for (auto const& [idx, r] : m_rois | std::views::enumerate) {
        uint16_t *d16 = (uint16_t*)frame->GetData();
        uint64_t sum = 0;

        for (size_t j = 0; j < 64; j++) {
            for (size_t i = 0; i < 64; i++) {
                sum += d16[r + i + j*512];
            }
        }
        // __m256i *d = (__m256i*)&d16[r];
        // m_avgs[idx] = 0;
        //
        // __m256i sumpxs = _mm256_setzero_si256();
        // for (size_t i = 0; i < 64; i++) {
        //     __m256i *d = (__m256i*)&d16[r+i*512];
        //     sumpxs = _mm256_add_epi16(d[0], d[1]);
        //     sumpxs = _mm256_add_epi16(sumpxs, d[2]);
        //     sumpxs = _mm256_add_epi16(sumpxs, d[3]);
        // }
        // uint16_t* avgvs = (uint16_t*)&sumpxs;
        // for (size_t i = 0; i < 16; i++) {
        //     m_avgs[idx] += avgvs[i];
        // }
        spdlog::info("idx: {} - avg: {}", idx, sum/4096.0);
    }

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
                height,
                m_camera->ctx->curExp->frameCount
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
void pm::Acquisition<F, C>::frameWriterThread() noexcept {
    m_frameIndex = 0;
    F* frame{nullptr};
    m_running = true;
    bool captured = false;

    if (!m_camera->StartExp((void*)&pm::Acquisition<F, C>::EofCallback, this)) {
        //TODO handle error
        spdlog::error("frameWriterThread StartExp failed");
        m_running = false;
        return;
    }

    if (m_fakeData) {
        spdlog::warn("Using test image data from {}", m_testImgPath);
    }

    m_lastFrameInProcessing = 0;
    m_frameWriterReadyCond.notify_all();

    do {
        {
            std::unique_lock<std::mutex> lock(m_frameWriterLock);
            if (m_frameWriterQueue.empty()) {
                const bool timedOut = !m_frameWriterCond.wait_for(
                            lock,
                            std::chrono::milliseconds(200),
                            [this]() { return !m_frameWriterQueue.empty(); }
                            );

                if (timedOut) {
                    //TODO handle timeout
                    continue;
                }
            }

            if (m_diskThreadAbortFlag) {
                spdlog::info("m_diskThreadAbortFlag: {}", m_diskThreadAbortFlag);
                continue;
            }

        } //release lock

        {
            std::unique_lock<std::mutex> lock(m_frameQueueLock);
            frame = m_frameWriterQueue.front();
            m_frameWriterQueue.pop();
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

        if (m_fakeData) {
            frame->SetData((void*)m_fakeData);
        }

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
                    writeFrame(frame);
                    processFrame(frame);

                    ++m_frameIndex;
                    if (m_progress) { m_progress(1); }
                    captured = true;
                }
            }
        }

    } while (!m_diskThreadAbortFlag);

    spdlog::info("Acquisition finished. flag: {}, frameIndex: {}, frameCount: {}", m_diskThreadAbortFlag, m_frameIndex, m_camera->ctx->curExp->frameCount);
    m_camera->StopExp();

    while (!m_frameWriterQueue.empty() && captured) {
        {
            std::unique_lock<std::mutex> lock(m_frameQueueLock);
            frame = m_frameWriterQueue.front();
            m_frameWriterQueue.pop();
        }

        //copy frame
        if (!frame->CopyData()) {
            spdlog::info("Failed to copy frame data");
            return;
        }

        if (m_frameIndex < m_camera->ctx->curExp->frameCount) {
            writeFrame(frame);
            ++m_frameIndex;
        }
        m_unusedFramePool->Release(frame);
    }

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
    //m_framesMax = std::min<uint64_t>(std::max<uint64_t>(500, frameCount), m_framesMax);
    //const uint64_t recommendedFrameCount = std::min<uint64_t>(10 + std::min<uint64_t>(frameCount, framesMax), bufferCount);

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
void pm::Acquisition<F, C>::StartAcquisition(std::function<void(size_t)> progressCB, double tiffFillValue, const C* tiffColorCtx) {
    //TODO implement colorCtx support
    std::unique_lock<std::mutex> lock(m_lock);

    m_progress = progressCB;
    m_hasNotified = false;
    m_frameIndex = 0;
    m_capturedFrames = 0;

    m_unusedFramePool->EnsurePoolSize(std::min<uint64_t>(m_camera->ctx->curExp->frameCount, m_framesMax));

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


    if (!m_frameWriterThread) {
        std::unique_lock<std::mutex> lock(m_frameWriterReadyLock);
        spdlog::info("Starting frame writer thread: frameCount {}", m_camera->ctx->curExp->frameCount);
        m_frameWriterThread = new std::thread(&Acquisition<F, C>::frameWriterThread, this);
        m_frameWriterReadyCond.wait(lock);
    } else {
        spdlog::info("Starting capture");
        if (!m_camera->StartExp((void*)&pm::Acquisition<F, C>::EofCallback, this)) {
            //TODO handle error
            spdlog::error("StartExp failed");
        }
    }

    //TODO move this out of acquistion, used by liveview as well for ROI calculations
    m_rois.clear();
    int32_t well_width_px = (4500 / 80);
    int32_t top_x = 512 / 2 - 0.5*(4 - 1) * well_width_px;
    int32_t top_y = 512 / 2 - 0.5*(4 - 1) * well_width_px;

    for (size_t r = 0; r < 4; r++) {
        int32_t y = top_y + r*well_width_px - 64/2;
	    for (size_t c = 0; c < 4; c++) {
	        int32_t x = top_x + c*well_width_px - 64/2;
            m_rois.push_back(x + y*512);
	    }
    }
    memset(m_avgs, 0, 256*2);

    return;
}


template<FrameConcept F, ColorConfigConcept C>
void pm::Acquisition<F, C>::StartLiveView() {
    std::unique_lock<std::mutex> lock(m_lock);

    switch (m_state) {
        case AcquisitionState::AcqIdle:
        case AcquisitionState::AcqStopped: 
            m_state = AcquisitionState::AcqLiveScan;
            break;
        case AcquisitionState::AcqCapture:
            m_state = AcquisitionState::AcqCaptureLiveScan;
            break;
        default:
            return;
    }

    if (!m_frameWriterThread) {
        std::unique_lock<std::mutex> lock(m_frameWriterReadyLock);
        spdlog::info("Starting frame writer thread for live view");
        m_frameWriterThread = new std::thread(&Acquisition<F, C>::frameWriterThread, this);
        m_frameWriterReadyCond.wait(lock);
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
    //m_camera->StopExp();
    m_diskThreadAbortFlag = true;
    m_state = AcquisitionState::AcqStopped;
    m_frameWriterCond.notify_one();
}


template<FrameConcept F, ColorConfigConcept C>
void pm::Acquisition<F, C>::WaitForStop() {
    std::unique_lock<std::mutex> lock(m_stopLock);
    if (m_frameWriterThread && m_running) {
        m_frameWriterCond.notify_one();
        m_frameWriterThread->join();

        m_frameWriterThread = nullptr;
        m_running = false;
    } else {
        spdlog::error("m_frameWriterThread not running");
        
    }

    m_camera->StopExp();

    m_capturedFrames = 0;
    m_state = AcquisitionState::AcqStopped;
    m_lastFrameInProcessing = m_lastFrameInCallback;
    //m_lastFrameInCallback = 0;
    m_diskThreadAbortFlag = false;
    m_frameWriterThread = nullptr;
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
