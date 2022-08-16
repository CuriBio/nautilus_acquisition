#ifndef PM_FRAME_H
#define PM_FRAME_H
#include <shared_mutex>

#include <Allocator.h>
#include <FrameInterface.h>

static FrameInfo sEmptyFrameInfo = FrameInfo();

namespace pm {
    class Frame {
        private:
            std::recursive_mutex m_recursive_mutex{};
            void* m_data{nullptr};
            void* m_dataSrc{nullptr};

            //TODO parameterize allocator alignment
            Allocator<4096> m_allocator{};

            size_t m_frameBytes{0};
            bool m_deepCopy{false};

            FrameInfo* m_info{&sEmptyFrameInfo};

        public:
            Frame(size_t frameBytes, bool deepCopy);
            ~Frame();

            void SetData(void* data);
            void* GetData();
            bool CopyData();
            bool Copy(const Frame& from, bool deepCopy);

            FrameInfo* GetInfo() const;
            void SetInfo(const FrameInfo& info);
    };
};
static_assert(FrameConcept<pm::Frame>);

#endif //PM_FRAME_H
