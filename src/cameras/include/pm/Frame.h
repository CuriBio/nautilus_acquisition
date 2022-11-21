#ifndef PM_FRAME_H
#define PM_FRAME_H
#include <shared_mutex>

#include <interfaces/FrameInterface.h>
#include <Allocator.h>
#include <PMemCopy.h>

static FrameInfo sEmptyFrameInfo = FrameInfo();

namespace pm {
    class Frame {
        private:
            mutable std::shared_mutex m_mutex{};
            void* m_data{nullptr};
            void* m_dataSrc{nullptr};

            //TODO parameterize allocator alignment
            Allocator<1> m_allocator{};

            size_t m_frameBytes{0};
            bool m_deepCopy{false};
            PMemCopy m_PMemCopy{4};

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

        private:
            void setInfo(const FrameInfo& info);
            void setData(void* data);
            bool copyData();
    };
};
static_assert(FrameConcept<pm::Frame>);

#endif //PM_FRAME_H
