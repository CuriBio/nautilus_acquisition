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
 * @file  Acquisition.h
 * 
 * @brief Definition of the Acquisition class.
 *********************************************************************/
#ifndef PM_ACQUISITION_H
#define PM_ACQUISITION_H
#include <chrono>
#include <condition_variable>
#include <filesystem>
#include <functional>
#include <memory>
#include <semaphore>
#include <thread>

#include <pvcam/master.h>
#include <pvcam/pvcam.h>
#include <pvcam/pvcam_helper_color.h>

#include <interfaces/FrameInterface.h>
#include <interfaces/ColorConfigInterface.h>
#include <FramePool.h>
#include <TiffFile.h>
#include <PMemCopy.h>

#include <pm/Camera.h>
#include <pm/Frame.h>

namespace pm {

    /*
     * @breif Acquisition controller class.
     *
     * Acquisition class for starting/stopping image acquisition.
     *
     * @tparam F FrameConcept type
     * @tparam C ColorConfigConcept type
     */
    template<FrameConcept F, ColorConfigConcept C>
        class Acquisition {
            private:
                std::shared_ptr<pm::Camera<F>> m_camera;
                std::mutex m_lock;
                std::mutex m_stopLock;
                bool m_hasNotified{false};

                bool m_running{ false };
                bool m_diskThreadAbortFlag{ false };
                bool m_acquireThreadAbortFlag{ false };
                AcquisitionState m_state { AcquisitionState::AcqStopped };

                std::mutex m_frameProcessingQueueLock;
                std::mutex m_frameProcessingLock;
                std::condition_variable m_frameProcessingCond;
                std::queue<F*> m_frameProcessingQueue;

                std::mutex m_frameProcessingReadyLock;
                std::condition_variable m_frameProcessingReadyCond;
                std::thread* m_frameProcessingThread{ nullptr };

                std::mutex m_acquisitionFinishedLock;
                std::condition_variable m_acquisitionFinishedCond;

                std::atomic<size_t> m_frameIndex{0};
                uint64_t m_framesMax{0};
                std::unique_ptr<FramePool<F>> m_unusedFramePool{nullptr};

                uint32_t m_lastFrameInCallback{0};
                uint32_t m_lastFrameInProcessing{0};
                F* m_latestFrame{nullptr};

                //TODO color context support
                //typename TiffFile<F>::ProcHelper m_tiffHelper{};
                SpdTable m_spdTable{};
                std::shared_ptr<PMemCopy> m_pCopy;
                std::shared_ptr<ParTask> m_parTask;
                //std::shared_ptr<PMemCopy> m_pCopy;

                uint16_t* m_fakeData{nullptr};
                std::string m_testImgPath{};

                std::function<void(size_t n)> m_progress;
                std::function<void(FrameCtx* ctx, F* frame)> m_processFn;

            public:

                /*
                 * @brief Acquisition constructor.
                 *
                 * Constructs a new acquisition class.
                 *
                 * @param c Pointer to camera instance to use for acquisition.
                 */
                Acquisition(std::shared_ptr<pm::Camera<F>> c);

                /*
                 * @brief Acquisition destructor.
                 */
                ~Acquisition();

                /*
                 * @brief Starts acquisition.
                 *
                 * Starts this acquisition with configured params.
                 *
                 * @param tiffFillValue Value to use as fill value if enabled.
                 * @param tiffColorCtx Color context, nullptr if not needed.
                 *
                 * @return true if successful, false otherwise.
                 */
                void StartAcquisition(std::function<void(size_t)> progressCB, std::function<void(FrameCtx*, F*)> process);

                /*
                 * @brief Starts live view.
                 */
                void StartLiveView();

                /*
                 * @brief Stops this acquisition.
                 */
                void StopAll();

                /*
                 * @brief Stops this acquisition, leave live view running.
                 */
                void StopCapture();

                /*
                 * @brief Stops liveview, leaves capture running..
                 */
                void StopLiveView();

                /*
                 * @brief Waits for acquisition to stop.
                 *
                 * Blocks until acquisition is finished.
                 */
                void WaitForStop();

                /*
                 * @brief Waits for acquisition to notifiy is finished;
                 *
                 * Blocks until acquisition is finished.
                 */
                void WaitForAcquisition();

                /*
                 * @brief Checks if acquisition is running.
                 *
                 * @return true if running, false otherwise.
                 */
                bool IsRunning();

                /*
                 * @brief Sets the latest frame for live view.
                 *
                 * Sets latest frame acquired by camera for use in live view.
                 *
                 * @param F Pointer to frame concept.
                 */
                void SetLatestFrame(F* frame);

                /*
                 * @brief Get latest frame
                 *
                 * Gets latest frame acquired by camera.
                 *
                 * @return Pointer to latest frame captured.
                 */
                F* GetLatestFrame();

                /*
                 * @brief Get current acquisition state.
                 *
                 * @return The current acquisition state.
                 */
                AcquisitionState GetState();

                /*
                 * @brief Loads test data for debugging.
                 *
                 * Takes path to tiff file, the data for each captured frame is replaced with
                 * the raw pixel data from the test image.
                 *
                 * @param testImgPath The path to a tiff file.
                 */
                void LoadTestData(std::string testImgPath);

            public:
                std::atomic<size_t> m_capturedFrames{0};

            private:
                /*
                 * @brief The eof camera callback.
                 *
                 * Called by PVCAM for each frame captured. This callback must not block.
                 *
                 * @param frameInfo The captured frame info struct.
                 * @param ctx The acquisition context.
                 */
                static void PV_DECL EofCallback(FRAME_INFO* frameInfo, void* ctx) noexcept;

                /*
                 * @brief Thread for processing incoming frames.
                 */
                void frameProcessingThread() noexcept;

                /*
                 * @brief Start processing thread
                 */
                void startProcessingThread() noexcept;

                /*
                 * @brief Write frame
                */
                void writeFrame(F* frame) noexcept;

                /*
                 * @brief Checks for lost frames.
                 *
                 * Helper function to check if any frames have been lost.
                 *
                 * @param frameN The current frame number.
                 * @param lastFrame The last captured frame number.
                 * @param i Frame index value.
                 */
                void checkLostFrame(uint32_t frameN, uint32_t& lastFrame, uint8_t i) noexcept;
        };
}

#endif //PM_ACQUISITION_H
