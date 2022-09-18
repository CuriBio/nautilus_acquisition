#include <cstdint>
#include <chrono>

#include <spdlog/spdlog.h>
#include <pvcam/master.h>
#include <pvcam/pvcam.h>

#include <pm/Frame.h>


pm::Frame::Frame(size_t frameBytes, bool deepCopy) :
    m_frameBytes(frameBytes),
    m_deepCopy(deepCopy) {
    if (deepCopy && frameBytes > 0) { //allocate data if using deepcopy
        m_data = m_allocator.Allocate(frameBytes);
    }
}

pm::Frame::~Frame() {
    if(m_deepCopy) {
        m_allocator.Free(m_data);
        m_data = nullptr;
    }
}

void pm::Frame::SetData(void* data) {
    std::lock_guard<std::shared_mutex> lock(m_mutex);
    setData(data);
}

void* pm::Frame::GetData() {
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    return m_data;
}

bool pm::Frame::CopyData() {
    std::lock_guard<std::shared_mutex> lock(m_mutex);
    if (m_deepCopy) {
        return copyData();
    } else {
        m_data = m_dataSrc;
    }
    return true;
}

FrameInfo* pm::Frame::GetInfo() const {
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    return m_info;
}

void pm::Frame::SetInfo(const FrameInfo& info) {
    std::lock_guard<std::shared_mutex> lock(m_mutex);
    return setInfo(info);
}

bool pm::Frame::Copy(const Frame& from, bool deepCopy) {
    std::unique_lock<std::shared_mutex> wrLock(m_mutex, std::defer_lock);
    std::shared_lock<std::shared_mutex> rdLock(from.m_mutex, std::defer_lock);
    std::lock(wrLock, rdLock);

    //TODO check configuration matches
    setData(from.m_data);

    if (deepCopy) {
        if(!copyData()) {
            spdlog::error("Failed to deep copy data");
            return false;
        }
        setInfo(*from.GetInfo());
        //TODO set trajectories
    } else {
        //TODO handle shallow copy? Doing this for now for testing
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
    /* auto start = std::chrono::high_resolution_clock::now(); */
    m_PMemCopy.Copy(m_data, m_dataSrc, m_frameBytes);
    //std::memcpy(m_data, m_dataSrc, m_frameBytes);
    /* auto finish = std::chrono::high_resolution_clock::now(); */
    /* auto timens = std::chrono::duration_cast<std::chrono::nanoseconds>(finish-start).count(); */
    /* spdlog::info("PMemCopy runtime: {}", timens); */
    return true;
}
