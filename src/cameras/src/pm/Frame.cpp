#include <cstdint>

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
    std::lock_guard<std::recursive_mutex> lock(m_recursive_mutex);
    m_dataSrc = data;
}

void* pm::Frame::GetData() {
    //std::recursive_lock<std::recursive_mutex> lock(m_recursive_mutex);
    return m_data;
}

bool pm::Frame::CopyData() {
    std::lock_guard<std::recursive_mutex> lock(m_recursive_mutex);
    if (m_deepCopy) {
        if (m_data && m_dataSrc) {
            //TODO parallel copy
            /* spdlog::info("Copying {} bytes", m_frameBytes); */
            std::memcpy(m_data, m_dataSrc, m_frameBytes);
        } else {
            return false;
        }
    } else {
        m_data = m_dataSrc;
    }
    return true;
}

FrameInfo* pm::Frame::GetInfo() const {
    return m_info;
}

void pm::Frame::SetInfo(const FrameInfo& info) {
    std::lock_guard<std::recursive_mutex> lock(m_recursive_mutex);
    *m_info = info;
}

bool pm::Frame::Copy(const Frame& from, bool deepCopy) {
    std::lock_guard<std::recursive_mutex> lock(m_recursive_mutex);

    //TODO check configuration matches
    SetData(from.m_data);

    if (deepCopy) {
        if(!CopyData()) {
            spdlog::error("Failed to deep copy data");
            return false;
        }
        SetInfo(*from.GetInfo());
        //TODO set trajectories
    } else {
        //TODO handle shallow copy? Doing this for now for testing
        SetInfo(*from.GetInfo());
    }

    return true;

}
