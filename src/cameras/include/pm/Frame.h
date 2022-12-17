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
 * @file  Frame.h
 * 
 * @brief Definition of the Frame class.
 *********************************************************************/
#ifndef PM_FRAME_H
#define PM_FRAME_H
#include <shared_mutex>

#include <interfaces/FrameInterface.h>
#include <Allocator.h>
#include <PMemCopy.h>
#include <ParTask.h>

static FrameInfo sEmptyFrameInfo = FrameInfo();

namespace pm {

   /*
    * @breif
    *
    *
    *
    * @param
    */
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
           /*
            * @brief Frame constructor.
            *
            *  Constructs new image frame object.
            *
            * @param frameBytes The size of the frame.
            * @param deepCopy Flag to control deep or shallow copy.
            * @param pTask Pointer to parallel task executor.
            */
            Frame(size_t frameBytes, bool deepCopy, std::shared_ptr<ParTask> pTask);

           /*
            * @brief Frame destructor.
            */
            ~Frame();

           /*
            * @brief Set frame data pointer.
            *
            * Sets the internal data pointer for this frame.
            *
            * @param data Pointer to data.
            */
            void SetData(void* data);

           /*
            * @brief Gets pointer to internal frame data.
            */
            void* GetData();

            /*
             * @brief Copy internal frame data.
             *
             *  Copies internal frame data source to frame object.
             *  Frame takes ownership of copied data.
             *
             * @return true if successful, false otherwise.
             */
            bool CopyData();

            /*
             * @brief Copy data from one frame to this frame.
             *
             *  Does deep or shallow copy of from frame data to this frame.
             *
             * @param from The frame to copy data from.
             * @param deepCopy Flag to control deep or shallow copy.
             *
             * @return true if successful, false otherwise.
             */
            bool Copy(const Frame& from, bool deepCopy);

            /*
             * @brief Get this frame info.
             *
             *  Gets pointer to internal frame info.
             *
             * @return Pointer to internal frame info.
             */
            FrameInfo* GetInfo() const;

            /*
             * @brief Set frame info.
             *
             *  Sets internal frame info.
             *
             * @param info The frame info to set in this frame.
             */
            void SetInfo(const FrameInfo& info);

        private:
            /*
             * @brief Internal set info method.
             *
             * @param info The frame info to set in this frame.
             */
            void setInfo(const FrameInfo& info);

            /*
             * @brief Sets internal frame data pointer.
             *
             * @param data The data to set in internal frame.
             */
            void setData(void* data);

            /*
             * @brief Copies internal data in frame.
             *
             * @return true if successful, false otherwise.
             */
            bool copyData();
    };
};
static_assert(FrameConcept<pm::Frame>);

#endif //PM_FRAME_H
