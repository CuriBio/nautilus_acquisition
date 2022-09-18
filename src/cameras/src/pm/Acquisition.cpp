#include <chrono>
#include <spdlog/spdlog.h>

#include <FrameInterface.h>

#include <pm/Frame.h>
#include <pm/Acquisition.h>
#include <PMemCopy.h>

/* THREADS */

template<FrameConcept F>
void PV_DECL pm::Acquisition<F>::EofCallback(FRAME_INFO* frameInfo, void* ctx) {
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

template<FrameConcept F>
void pm::Acquisition<F>::checkLostFrame(uint32_t frameN, uint32_t& lastFrame) {
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

template<FrameConcept F>
bool pm::Acquisition<F>::ProcessNewFrame(std::shared_ptr<F> frame) {
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

template<FrameConcept F>
void pm::Acquisition<F>::acquireThread() {
    m_lastFrameInCallback = 0;
    m_lastFrameInProcessing = 0;

    {
        std::unique_lock<std::mutex> lock(m_acquireLock);
        //TODO add conidtion variable to signal when acquireThread is running
        /* if (m_running) { */
        /*   spdlog::warn("Acquisition already running"); */
        /*   return; */
        /* } */

        if (!m_camera->StartExp((void*)&pm::Acquisition<F>::EofCallback, this)) {
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
            nFrames++;

        } //release lock

        if (!ProcessNewFrame(frame)) {
            //TODO handle error
        }

        //m_unusedFramePool->EnsurePoolSize(3);
    } while (true || nFrames < m_camera->ctx->curExp->frameCount);

    spdlog::info("Acquisition finished");
    m_camera->StopExp();
}


template<FrameConcept F>
void pm::Acquisition<F>::frameWriterThread() {
    spdlog::info("Starting frame writer thread");

    size_t frameIndex = 0;
    std::shared_ptr<F> frame{nullptr};

    do {
        {
            std::unique_lock<std::mutex> lock(m_frameWriterLock);
            //m_frameWriterCond.wait(lock);
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

        //TODO write frame
        frameIndex += 1;

        //Release frame
        m_unusedFramePool->Release(frame);

    //TODO isAcqModeLive && m_frameWriterAbortFlag
    } while ((true || frameIndex < m_camera->ctx->curExp->frameCount)); // && !m_diskThreadAbortFlag);
}


template<FrameConcept F>
pm::Acquisition<F>::Acquisition(std::shared_ptr<pm::Camera<F>> camera) : m_camera(camera), m_running(false) {
    spdlog::info("calling Acquisition");
    //TODO preallocate unused frame pool size based on acquisition size
    m_unusedFramePool = std::make_shared<ObjPool<F, uns32, bool>>(20, camera->ctx->frameBytes, true);
}

template<FrameConcept F>
pm::Acquisition<F>::~Acquisition() {
    spdlog::info("calling ~Acquisition");
}


template<FrameConcept F>
bool pm::Acquisition<F>::Start() {
    spdlog::info("Starting acquisition: frameCount {}", m_camera->ctx->curExp->frameCount);
    m_frameWriterThread = new std::thread(&Acquisition<F>::frameWriterThread, this);
    m_acquireThread = new std::thread(&Acquisition<F>::acquireThread, this);

    if (m_acquireThread) {
        m_running = true;
        return true;
    }

    m_running = false;
    return false;
}


template<FrameConcept F>
bool pm::Acquisition<F>::Abort() {
    return true;
}


template<FrameConcept F>
void pm::Acquisition<F>::WaitForStop() {
    m_acquireThread->join();
    return;
}


template<FrameConcept F>
bool pm::Acquisition<F>::IsRunning() {
    return true;
}

//Avoid link errors
template class pm::Acquisition<pm::Frame>;
