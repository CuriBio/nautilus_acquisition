#ifndef PM_ACQUISITION_H
#define PM_ACQUISITION_H
#include <chrono>
#include <condition_variable>
#include <semaphore>
#include <memory>
#include <thread>

#include <pvcam/master.h>
#include <pvcam/pvcam.h>
#include <pvcam/pvcam_helper_color.h>

#include <interfaces/FrameInterface.h>
#include <interfaces/ColorConfigInterface.h>
#include <ObjectPool.h>
#include <TiffFile.h>

#include <pm/Camera.h>
#include <pm/Frame.h>

namespace pm {
    template<FrameConcept F, ColorConfigConcept C>
        class Acquisition {
            private:
                std::shared_ptr<pm::Camera<F>> m_camera;
                bool m_running{ false };

                std::mutex m_acquireLock;
                std::condition_variable m_acquireFrameCond;
                std::queue<std::shared_ptr<F>> m_acquireFrameQueue;

                std::mutex m_frameWriterLock;
                std::condition_variable m_frameWriterCond;
                std::queue<std::shared_ptr<F>> m_frameWriterQueue;

                std::thread* m_acquireThread{ nullptr };
                std::thread* m_frameWriterThread{ nullptr };

                /* std::mutex m_writerLock; */
                /* std::thread* m_writerThread{ nullptr }; */

                //TODO should be initialized with size after
                //the number of frames needed is known
                std::shared_ptr<ObjPool<F, uns32, bool>> m_unusedFramePool{nullptr};

                uint32_t m_lastFrameInCallback{0};
                uint32_t m_lastFrameInProcessing{0};

                typename TiffFile<F>::ProcHelper m_tiffHelper{};
                SpdTable m_spdTable{};

            public:
                Acquisition(std::shared_ptr<pm::Camera<F>> c);
                ~Acquisition();

                bool Start(double tiffFillValue = 0.0, const C* tiffColorCtx = nullptr);
                bool Abort();
                void WaitForStop();
                bool IsRunning();

                bool ProcessNewFrame(std::shared_ptr<F> frame);
            private:
                static void PV_DECL EofCallback(FRAME_INFO* frameInfo, void* Acquisition_pointer);

                void acquireThread();
                void frameWriterThread();
                void checkLostFrame(uint32_t frameN, uint32_t& lastFrame);
        };
}

#endif //PM_ACQUISITION_H
