#include <chrono>
#include <spdlog/spdlog.h>

#include <interfaces/FrameInterface.h>
#include <interfaces/ColorConfigInterface.h>

#include <pm/Frame.h>
#include <pm/Acquisition.h>
#include <pm/ColorConfig.h>
#include <PMemCopy.h>
#include <TiffFile.h>

/* THREADS */

template<FrameConcept F, ColorConfigConcept C>
void PV_DECL pm::Acquisition<F, C>::EofCallback(FRAME_INFO* frameInfo, void* ctx) {
    Acquisition* cls = static_cast<Acquisition*>(ctx);

    if (!frameInfo) {
        //cls->Abort();
        spdlog::error("Invalid Frame");
        return;
    } 

    //Check for skipped frames
    const uint32_t cbFrameNr = frameInfo->FrameNr;
    cls->checkLostFrame(cbFrameNr, cls->m_lastFrameInCallback);

    std::shared_ptr<F> frame = cls->m_unusedFramePool->Acquire();

    if (!frame) {
        //TODO handle error
        spdlog::error("Could not acquire unused frame from frame pool");
        return;
    }

    if (!cls->m_camera->GetLatestFrame(frame)) {
      //TODO handle error
      spdlog::error("GetLatestFrame failed");
      return;
    }

    //queue frame for processing
    {
        //TODO
        //if m_acquireFrameQueue.size is valid
        std::lock_guard<std::mutex> lock(cls->m_acquireLock);
        cls->m_acquireFrameQueue.push(frame);
        //else
        //check for lost frames
    } // release lock

    cls->m_acquireFrameCond.notify_one();

    return;
}

template<FrameConcept F, ColorConfigConcept C>
void pm::Acquisition<F, C>::checkLostFrame(uint32_t frameN, uint32_t& lastFrame) {
    if (frameN > lastFrame + 1) {
        //m_toBeProcessedFramesStats.ReportFrameLost(frameNr - m_lastFrameInCallback - 1);

        // Log all the frame numbers we missed
        for (uint32_t nr = lastFrame + 1; nr < frameN; nr++) {
            spdlog::info("Lost frame # {}", nr);
            //m_uncaughtFrames.AddItem(nr);
        }
    }
    lastFrame = frameN;
}

template<FrameConcept F, ColorConfigConcept C>
bool pm::Acquisition<F, C>::ProcessNewFrame(std::shared_ptr<F> frame) {
    const uint32_t frameNr = frame->GetInfo()->frameNr;

    if (frameNr <= m_lastFrameInProcessing) {
        //TODO log stats on dropped frame

        spdlog::error("Frame number out of order: {}, last frame number was {}, ignoring", frameNr, m_lastFrameInProcessing);

        // Drop frame for invalid frame number
        // Number out of order, cannot add it to m_unsavedFrames stats
        return true;
    }

    // Check to make sure we didn't skip a frame
    checkLostFrame(frameNr, m_lastFrameInProcessing);

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

    return true;
}

template<FrameConcept F, ColorConfigConcept C>
void pm::Acquisition<F, C>::acquireThread() {
    m_lastFrameInCallback = 0;
    m_lastFrameInProcessing = 0;

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

    auto nFrames = 0;

    do {
        std::shared_ptr<F> frame{nullptr};
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
            m_acquireFrameQueue.pop();

            if (m_capture) {
                nFrames++;
            }

        } //release lock

        if (!m_capture) {
            std::unique_lock<std::mutex> lock(m_lock);
            m_latestFrame = frame;
        }

        if (m_capture && !ProcessNewFrame(frame)) {
            //TODO handle error
        }

        m_unusedFramePool->EnsurePoolSize(3);
    } while (!m_acquireThreadAbortFlag && nFrames < m_camera->ctx->curExp->frameCount);

    if (!m_capture) { //if running live view only need to tell frameWriterThread to stop
        m_diskThreadAbortFlag = true;
    }

    spdlog::info("Acquisition finished");
    //m_camera->StopExp();
}


template<FrameConcept F, ColorConfigConcept C>
void pm::Acquisition<F, C>::frameWriterThread() {
    size_t frameIndex = 0;
    size_t startFrame = 0;

    std::shared_ptr<F> frame{nullptr};
    TiffFile<F>* file = new TiffFile<F>(
        m_camera->ctx->curExp->region,
        m_camera->ctx->curExp->imgFormat,
        m_camera->ctx->info.spdTable[m_camera->ctx->curExp->spdTableIdx].bitDepth,
        m_camera->ctx->curExp->frameCount
    );

    std::string fileName = m_camera->ctx->curExp->fileName;
    spdlog::info("Capture mode is enabled: {}", m_capture ? "true" : "false");

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

            frame = m_frameWriterQueue.front();
            m_frameWriterQueue.pop();
        } //release lock

        //TODO support different storage types
        if (frameIndex == 0) {
            fileName += ".tiff";
            file->Open(fileName);

            //Get start frame number
            //startFrame = frame->GetInfo()->frameNr;
        }

        file->WriteFrame(frame);
        frameIndex++; //= frame->GetInfo()->frameNr;

        //Release frame
        m_unusedFramePool->Release(frame);
        spdlog::info("frameIndex: {}, startFrame: {}, startFrame + frameCount: {}",
            frameIndex,
            startFrame,
            startFrame + m_camera->ctx->curExp->frameCount
        );

    //TODO isAcqModeLive && m_frameWriterAbortFlag
    //} while (!m_diskThreadAbortFlag && (frameIndex < (startFrame + m_camera->ctx->curExp->frameCount - 1)));
    } while (!m_diskThreadAbortFlag && (frameIndex < m_camera->ctx->curExp->frameCount));

    spdlog::info("Frame writer finished");

    file->Close();
    delete file;
    file = nullptr;
}


template<FrameConcept F, ColorConfigConcept C>
pm::Acquisition<F, C>::Acquisition(std::shared_ptr<pm::Camera<F>> camera) : m_camera(camera), m_running(false) {
    //TODO preallocate unused frame pool size based on acquisition size
    const uint64_t bufferCount = camera->ctx->curExp->bufferCount;
    const uint64_t frameCount = camera->ctx->curExp->frameCount;
    const uint64_t frameBytes = camera->ctx->frameBytes;
    const uint64_t framesMax = (frameBytes == 0) ? 0 : ((100 << 20) / frameBytes); //number of frames for 100MB buffer
    const uint64_t recommendedFrameCount = std::min<uint64_t>(10 + std::min<uint64_t>(frameCount, framesMax), bufferCount);

    spdlog::info("Initializing frame pool with {} objects of size {}", recommendedFrameCount, camera->ctx->frameBytes);
    m_unusedFramePool = std::make_shared<ObjPool<F, uns32, bool>>(recommendedFrameCount, camera->ctx->frameBytes, true);

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
        m_capture = saveToDisk;

        if (!m_frameWriterThread) {
            spdlog::info("Starting frame writer thread");
            m_frameWriterThread = new std::thread(&Acquisition<F, C>::frameWriterThread, this);
        }

        if (!m_acquireThread) {
            spdlog::info("Starting acquisition: frameCount {}", m_camera->ctx->curExp->frameCount);
            m_acquireThread = new std::thread(&Acquisition<F, C>::acquireThread, this);
        }

        if (m_acquireThread && m_frameWriterThread) {
            m_running = true;
            return true;
        }
    } else if (m_running && !m_capture && saveToDisk) {
        //live view is running but not capturing so enable capture
        m_capture = true;
        return true;
    } else if (m_running && m_capture) {
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

    WaitForStop();
    m_camera->StopExp();

    return true;
}


template<FrameConcept F, ColorConfigConcept C>
void pm::Acquisition<F, C>::WaitForStop() {
    std::unique_lock<std::mutex> lock(m_stopLock);
    if (m_acquireThread && m_frameWriterThread && m_running) {
        m_acquireThread->join();
        m_frameWriterThread->join();

        m_acquireThread = nullptr;
        m_frameWriterThread = nullptr;
        m_running = false;
    }

    return;
}

template<FrameConcept F, ColorConfigConcept C>
std::shared_ptr<F> pm::Acquisition<F,C>::GetLatestFrame() {
    std::unique_lock<std::mutex> lock(m_lock);
    return m_latestFrame;
}


template<FrameConcept F, ColorConfigConcept C>
bool pm::Acquisition<F, C>::IsRunning() {
    return m_running;
}

//Avoid link errors
template class pm::Acquisition<pm::Frame, pm::ColorConfig<ph_color_context>>;
