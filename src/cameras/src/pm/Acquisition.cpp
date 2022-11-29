#include <cassert>
#include <chrono>
#include <thread>
#include <string>
#include <sstream>
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
    std::unique_lock<std::mutex> cblock(cls->m_cbLock);

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
    cls->checkLostFrame(cbFrameNr, cls->m_lastFrameInCallback, 0);


    if (!frame) {
        //TODO handle error
        spdlog::error("Could not acquire unused frame from frame pool");
        return;
    }


    //queue frame for processing
    {
        //TODO
        //if m_acquireFrameQueue.size is valid
        //std::lock_guard<std::mutex> lock(cls->m_acquireLock);
        //cls->m_acquireFrameQueue.push(frame);
        std::unique_lock<std::mutex> lock(cls->m_frameWriterLock);
        std::unique_lock<std::mutex> qlock(cls->m_acquireLock);
        cls->m_frameWriterQueue.push(frame);
        //else
        //check for lost frames
    } // release lock

    //notify acquisition thread
    //cls->m_acquireFrameCond.notify_one();
    cls->m_frameWriterCond.notify_one();

    return;
}

template<FrameConcept F, ColorConfigConcept C>
void pm::Acquisition<F, C>::checkLostFrame(uint32_t frameN, uint32_t &lastFrame, uint8_t i) {
    std::unique_lock<std::mutex> lock(m_lock);
    if (frameN > lastFrame + 1) {
        //m_toBeProcessedFramesStats.ReportFrameLost(frameNr - m_lastFrameInCallback - 1);

        // Log all the frame numbers we missed
        for (uint32_t nr = lastFrame + 1; nr < frameN; nr++) {
            spdlog::info("({}) Current Frame ({}), Lost frame # {}", i, frameN, nr);
            //m_uncaughtFrames.AddItem(nr);
        }
    }
    lastFrame = frameN;
}


template<FrameConcept F, ColorConfigConcept C>
bool pm::Acquisition<F, C>::ProcessNewFrame(F* frame) {
    assert(frame != nullptr && frame->GetData() != nullptr);

    const uint32_t frameNr = frame->GetInfo()->frameNr;
    if (frameNr <= m_lastFrameInProcessing) {
        //TODO log stats on dropped frame

        spdlog::error("Frame number out of order: {}, last frame number was {}, ignoring", frameNr, m_lastFrameInProcessing);

        // Drop frame for invalid frame number
        // Number out of order, cannot add it to m_unsavedFrames stats
        m_lastFrameInProcessing = frameNr;
        return true;
    }

    // Check to make sure we didn't skip a frame
    checkLostFrame(frameNr, m_lastFrameInProcessing, 1);

    if (m_state == AcquisitionState::AcqCapture) {
        if (!frame->CopyData()) {
            return false;
        }
        {
            std::unique_lock<std::mutex> lock(m_frameWriterLock);
            //TODO check queue capacity
            m_frameWriterQueue.push(frame);
            //TODO handle not enough RAM
        }

        // Notify disk waiter about new queued frame
        m_frameWriterCond.notify_one();
    }

    return true;
}


template<FrameConcept F, ColorConfigConcept C>
void pm::Acquisition<F, C>::acquireThread() {
    {
        std::unique_lock<std::mutex> lock(m_acquireLock);
        //TODO add conidtion variable to signal when acquireThread is running
        /* if (m_running) { */
        /*   spdlog::warn("Acquisition already running"); */
        /*   return; */
        /* } */

        if (!m_camera->StartExp((void*)&pm::Acquisition<F, C>::EofCallback, this)) {
            //TODO handle error
            spdlog::error("StartExp failed");
        }
        m_running = true;
    } //release lock

    uint32_t nFrames = 0;

    do {
        F* frame{nullptr};
        {
            std::unique_lock<std::mutex> lock(m_acquireLock);
            const bool timedOut = !m_acquireFrameCond.wait_for(
                        lock,
                        std::chrono::milliseconds(5000),
                        [this]() { return !m_acquireFrameQueue.empty(); }
                        );

            if (timedOut) {
                //TODO handle timeout
                spdlog::info("m_acquireFrameCond timeout");
                continue;
            }

            frame = m_acquireFrameQueue.front();
            assert(frame != nullptr);
            m_acquireFrameQueue.pop();

            if (m_state == AcquisitionState::AcqCapture) {
                //std::unique_lock<std::mutex> lock(m_lock);
                nFrames++;
            }

        } //release lock

        if (m_state == AcquisitionState::AcqLiveScan) {
            std::unique_lock<std::mutex> lock(m_lock);
            //m_latestFrame = frame;
        }

        if (!ProcessNewFrame(frame)) {
            //TODO handle error
        }

        m_unusedFramePool->EnsurePoolSize(3);
        spdlog::info("{} < {}", nFrames, m_camera->ctx->curExp->frameCount);
    } while (m_state == AcquisitionState::AcqLiveScan || (!m_acquireThreadAbortFlag && nFrames < m_camera->ctx->curExp->frameCount));

    spdlog::info("Acquisition finished");
    m_camera->StopExp();
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
        m_camera->ctx->curExp->imgFormat,
        m_camera->ctx->info.spdTable[m_camera->ctx->curExp->spdTableIdx].bitDepth,
        (m_camera->ctx->curExp->storageType == StorageType::TiffStack) ? m_camera->ctx->curExp->frameCount : 1
    );

    spdlog::info("Capture mode is enabled: {}", (m_state == AcquisitionState::AcqCapture) ? "true" : "false");

    {
        std::unique_lock<std::mutex> lock(m_acquireLock);
        //TODO add conidtion variable to signal when acquireThread is running
        /* if (m_running) { */
        /*   spdlog::warn("Acquisition already running"); */
        /*   return; */
        /* } */

        if (!m_camera->StartExp((void*)&pm::Acquisition<F, C>::EofCallback, this)) {
            //TODO handle error
            spdlog::error("StartExp failed");
        }
        m_running = true;
    } //release lock

    do {
        {
            std::unique_lock<std::mutex> lock(m_frameWriterLock);
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

            std::unique_lock<std::mutex> qlock(m_acquireLock);
            while (!m_frameWriterQueue.empty()) {
                frame = m_frameWriterQueue.front();
                assert(frame != nullptr && file != nullptr);
                const uint32_t frameNr = frame->GetInfo()->frameNr;
                m_frameWriterQueue.pop();
            }
        } //release lock

        /* const uint32_t frameNr = frame->GetInfo()->frameNr; */
        /* spdlog::info("processing frame {} - {}", fmt::ptr(frame), frameNr); */
        /* if (frameNr <= m_lastFrameInProcessing) { */
        /*     //TODO log stats on dropped frame */

        /*     spdlog::error("Frame number out of order: {}, last frame number was {}, ignoring", frameNr, m_lastFrameInProcessing); */

        /*     // Drop frame for invalid frame number */
        /*     // Number out of order, cannot add it to m_unsavedFrames stats */
        /*     m_lastFrameInProcessing = frameNr; */
        /* } */

        // Check to make sure we didn't skip a frame
        //checkLostFrame(frameNr, m_lastFrameInProcessing, 1);

        if (m_state == AcquisitionState::AcqCapture) {
            //copy frame
            if (!frame->CopyData()) {
                spdlog::info("Failed to copy frame data");
                continue;
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
            m_unusedFramePool->Release(frame);
            frame = nullptr;
            frameIndex++;
        }
    } while (m_state == AcquisitionState::AcqLiveScan || (!m_diskThreadAbortFlag && (frameIndex < m_camera->ctx->curExp->frameCount)));

    spdlog::info("Acquisition finished");
    m_camera->StopExp();

    if (file) {
        file->Close();
        delete file;
        file = nullptr;
    }
}


template<FrameConcept F, ColorConfigConcept C>
pm::Acquisition<F, C>::Acquisition(std::shared_ptr<pm::Camera<F>> camera) : m_camera(camera), m_running(false) {
    m_pCopy = std::make_shared<PMemCopy>(4);
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
    //const uint64_t recommendedFrameCount = std::min<uint64_t>(10 + std::min<uint64_t>(frameCount, framesMax), bufferCount);

    spdlog::info("Initializing frame pool with {} objects of size {}", framesMax, frameBytes);
    m_unusedFramePool = std::make_unique<FramePool<F>>(framesMax, frameBytes, true, m_pCopy);

    spdlog::info("Get speed table {}", camera->ctx->curExp->spdTableIdx);
    uint16_t spdTblIdx = camera->ctx->curExp->spdTableIdx;
    m_spdTable = camera->ctx->info.spdTable[spdTblIdx];
}


template<FrameConcept F, ColorConfigConcept C>
pm::Acquisition<F, C>::~Acquisition() {
    //TODO cleanup
}


template<FrameConcept F, ColorConfigConcept C>
bool pm::Acquisition<F, C>::Start(bool saveToDisk, double tiffFillValue, const C* tiffColorCtx) {
    //TODO implement colorCtx support

    if (!m_running) {
        std::unique_lock<std::mutex> lock(m_lock);
        m_state = (saveToDisk) ? AcquisitionState::AcqCapture : AcquisitionState::AcqLiveScan;

        if (!m_frameWriterThread) {
            spdlog::info("Starting frame writer thread: frameCount {}", m_camera->ctx->curExp->frameCount);
            m_frameWriterThread = new std::thread(&Acquisition<F, C>::frameWriterThread, this);
        }

        /* if (!m_acquireThread) { */
        /*     spdlog::info("Starting acquisition: frameCount {}", m_camera->ctx->curExp->frameCount); */
        /*     m_acquireThread = new std::thread(&Acquisition<F, C>::acquireThread, this); */
        /* } */

        if (/*m_acquireThread &&*/ m_frameWriterThread) {
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
bool pm::Acquisition<F, C>::Abort() {
    m_acquireThreadAbortFlag = true;
    m_diskThreadAbortFlag = true;
    m_state = AcquisitionState::AcqStopped;

    /* WaitForStop(); */
    /* m_camera->StopExp(); */

    return true;
}


template<FrameConcept F, ColorConfigConcept C>
void pm::Acquisition<F, C>::WaitForStop() {
    std::unique_lock<std::mutex> lock(m_stopLock);
    if (/*m_acquireThread &&*/ m_frameWriterThread && m_running) {
        //m_acquireThread->join();
        m_frameWriterThread->join();

        //m_acquireThread = nullptr;
        m_frameWriterThread = nullptr;
        m_running = false;
    }

    return;
}

template<FrameConcept F, ColorConfigConcept C>
F* pm::Acquisition<F,C>::GetLatestFrame() {
    std::unique_lock<std::mutex> lock(m_lock);
    return m_latestFrame;
}


template<FrameConcept F, ColorConfigConcept C>
bool pm::Acquisition<F, C>::IsRunning() {
    return m_running;
}

template<FrameConcept F, ColorConfigConcept C>
AcquisitionState pm::Acquisition<F, C>::GetState() {
    return m_state;
}

//Avoid link errors
template class pm::Acquisition<pm::Frame, pm::ColorConfig<ph_color_context>>;
