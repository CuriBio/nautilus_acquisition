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
#ifndef PM_FRAME_H
#define PM_FRAME_H
#include <shared_mutex>

#include <interfaces/FrameInterface.h>
#include <Allocator.h>
#include <PMemCopy.h>
#include <ParTask.h>

static FrameInfo sEmptyFrameInfo = FrameInfo();

namespace pm {
    class Frame {
        private:
            mutable std::mutex m_mutex{};
            void* m_data{nullptr};
            void* m_dataSrc{nullptr};

            //TODO parameterize allocator alignment
            Allocator<1> m_allocator{};

            size_t m_frameBytes{0};
            bool m_deepCopy{false};

            std::shared_ptr<PMemCopy> m_PMemCopy;
            std::shared_ptr<ParTask> m_pTask;

            FrameInfo* m_info{&sEmptyFrameInfo};

        public:
            Frame(size_t frameBytes, bool deepCopy, std::shared_ptr<ParTask> pTask);
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
