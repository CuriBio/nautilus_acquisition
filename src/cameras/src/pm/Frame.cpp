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
#include <cstdint>
#include <chrono>

#include <spdlog/spdlog.h>
#include <pvcam/master.h>
#include <pvcam/pvcam.h>

#include <pm/Frame.h>


pm::Frame::Frame(size_t frameBytes, bool deepCopy, std::shared_ptr<ParTask> pTask) :
    m_frameBytes(frameBytes), m_deepCopy(deepCopy), m_pTask(pTask) {
    if (deepCopy && frameBytes > 0) { //allocate data if using deepcopy
        m_data = m_allocator.Allocate(frameBytes);
    }
    m_info = new FrameInfo(); 
    m_PMemCopy = std::make_shared<PMemCopy>();
}

pm::Frame::~Frame() {
    if(m_deepCopy) {
        m_allocator.Free(m_data);
        m_data = nullptr;
    }
}

void pm::Frame::SetData(void* data) {
    std::unique_lock<std::mutex> lock(m_mutex);
    setData(data);
}

void* pm::Frame::GetData() {
    std::unique_lock<std::mutex> lock(m_mutex);
    return m_data;
}

bool pm::Frame::CopyData() {
    std::unique_lock<std::mutex> lock(m_mutex);
    if (m_deepCopy) {
        return copyData();
    } else {
        m_data = m_dataSrc;
    }
    return true;
}

FrameInfo* pm::Frame::GetInfo() const {
    std::unique_lock<std::mutex> lock(m_mutex);
    return m_info;
}

void pm::Frame::SetInfo(const FrameInfo& info) {
    std::unique_lock<std::mutex> lock(m_mutex);
    return setInfo(info);
}

bool pm::Frame::Copy(const Frame& from, bool deepCopy) {
    assert(&from != this);
    /* std::unique_lock<std::shared_mutex> wrLock(m_mutex, std::defer_lock); */
    /* std::shared_lock<std::shared_mutex> rdLock(from.m_mutex, std::defer_lock); */
    /* std::lock(wrLock, rdLock); */
    std::unique_lock<std::mutex> lock(m_mutex);

    setData(from.m_dataSrc);

    if (deepCopy) {
        if(m_dataSrc && m_data && !copyData()) {
            spdlog::error("Failed to deep copy data");
            return false;
        }
        setInfo(*from.GetInfo());
        //TODO set trajectories
    } else {
        //TODO check configuration matches
        setInfo(*from.GetInfo());
    }
    return true;
}

void pm::Frame::setData(void* data) {
    m_dataSrc = data;
}

void pm::Frame::setInfo(const FrameInfo& info) {
    *m_info = info;
}

bool pm::Frame::copyData() {
    m_PMemCopy->Copy(m_data, m_dataSrc, m_frameBytes);
    m_pTask->Start<PMemCopy>(m_PMemCopy);
    return true;
}
