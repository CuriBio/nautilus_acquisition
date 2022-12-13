#include <stdint.h>
#include <map>

#include <spdlog/spdlog.h>
#include <NIDAQmx.h>
#include "NIDAQmx_wrapper.h"


NIDAQmx::NIDAQmx() {
}

NIDAQmx::~NIDAQmx() {
    for (const auto & [taskName, taskHandle] : m_tasks) {
        if (DAQmxStopTask(taskHandle) < 0) {
            spdlog::error("Stop task {} failed, ({})", taskName, GetExtendedErrorInfo());
            continue;
        }
        if (DAQmxClearTask(taskHandle) < 0) {
            spdlog::error("Clear task {} failed, ({})", taskName, GetExtendedErrorInfo());
            continue;
        }

    }
}

bool NIDAQmx::CreateTask(std::string& taskName) {
    if (!m_tasks.contains(taskName)) {
        TaskHandle handle = (TaskHandle)malloc(sizeof(void*));
        spdlog::info("Creating task {} (taskHandle {})", taskName, fmt::ptr(handle));

        if (!handle) {
            spdlog::error("Failed to allocate TaskHandle for {}", taskName);
            return false;
        }

        if (DAQmxCreateTask(taskName.c_str(), &handle) < 0) {
            free(handle);
            spdlog::error("Clear task {} (handle {}) failed, ({})", taskName, fmt::ptr(handle), GetExtendedErrorInfo());
            return false;
        }

        m_tasks[taskName] = handle;
        return true;
    } else {
        spdlog::error("Task {} already exists", taskName);
        return false;
    }
}

bool NIDAQmx::ClearTask(std::string& taskName) {
    if(m_tasks.contains(taskName)) {
        if (DAQmxClearTask(m_tasks[taskName]) < 0) {
            spdlog::error("Failed to clear task {}, ({})", taskName, GetExtendedErrorInfo());
            return false;
        }
        return true;
    } else {
        spdlog::info("Task {} does not exist", taskName);
        return false;
    }
}

bool NIDAQmx::StartTask(std::string& taskName) {
    if(m_tasks.contains(taskName)) {
        if (DAQmxStartTask(m_tasks[taskName]) < 0) {
            spdlog::error("Failed to start task {}, ({})", taskName, GetExtendedErrorInfo());
            return false;
        }
        return true;
    } else {
        spdlog::error("Task {} does not exist", taskName);
        return false;
    }
}

bool NIDAQmx::StopTask(std::string& taskName) {
    if(m_tasks.contains(taskName)) {
        if (DAQmxStopTask(m_tasks[taskName]) < 0) {
            spdlog::error("Failed to stop task {}, ({})", taskName, GetExtendedErrorInfo());
            return false;
        }
        return true;
    } else {
        spdlog::error("Task {} does not exist", taskName);
        return false;
    }
}

bool NIDAQmx::WaitForTask(std::string& taskName, double waitTimeS) {
    if(m_tasks.contains(taskName)) {
        if (DAQmxWaitUntilTaskDone(m_tasks[taskName], static_cast<float64>(waitTimeS)) < 0) {
            spdlog::error("Failed to wait for task {}, ({})", taskName, GetExtendedErrorInfo());
            return false;
        }
        return true;
    } else {
        spdlog::error("Task {} does not exist", taskName);
        return false;
    }
}

bool NIDAQmx::CreateAnalogOutpuVoltageChan(std::string& taskName, const char physicalChan[], double minVal, double maxVal, int32_t units) {
    if(m_tasks.contains(taskName)) {
        float64 minv = static_cast<float64>(minVal);
        float64 maxv = static_cast<float64>(maxVal);
        spdlog::info("CreateAnalogOutpuVoltageChan: taskHandle {}", fmt::ptr(m_tasks[taskName]));
        if (DAQmxCreateAOVoltageChan(m_tasks[taskName], physicalChan, "", minv, maxv, DAQmx_Val_Volts, NULL) < 0) {
            spdlog::error("Failed to create Analog Output voltage channel for task {} (handle {}), ({})", taskName, m_tasks[taskName], GetExtendedErrorInfo());
            return false;
        }
        return true;
    } else {
        spdlog::error("Task {} does not exist", taskName);
        return false;
    }
}

bool NIDAQmx::CreateDigitalOutputChan(std::string& taskName, const char lines[], int32_t lineGrouping) {
    if(m_tasks.contains(taskName)) {
        if (DAQmxCreateDOChan(m_tasks[taskName], lines, "",  static_cast<int32>(lineGrouping)) < 0) {
            spdlog::error("Failed to create digital output channel for task {} (handle {}), ({})", taskName, m_tasks[taskName], GetExtendedErrorInfo());
            return false;
        }
        return true;
    } else {
        spdlog::error("Task {} does not exist", taskName);
        return false;
    }
}

bool NIDAQmx::WriteAnalogF64(
    std::string& taskName,
    int32_t numSampsPerChan,
    unsigned long autoStart,
    double timeout,
    double dataLayout,
    const double writeArray[],
    int32_t* sampsPerChanWritten)
{
    if(m_tasks.contains(taskName)) {
        int32 _nSamps = static_cast<int32>(numSampsPerChan);
        bool32 _autoStart = static_cast<bool32>(autoStart);
        float64 _timeout = static_cast<float64>(timeout);
        bool32 _dataLayout = static_cast<bool32>(dataLayout);
        const float64* _writeArray = static_cast<const float64*>(writeArray);
        int32* _sampsWritten = reinterpret_cast<int32*>(sampsPerChanWritten);

        if (DAQmxWriteAnalogF64(m_tasks[taskName], _nSamps, _autoStart, _timeout, _dataLayout, _writeArray, _sampsWritten, NULL) < 0) {
            spdlog::error("Failed to write analog64 value for task {}, ({})", taskName, GetExtendedErrorInfo());
            return false;
        }
        return true;
    } else {
        spdlog::error("Task {} does not exist", taskName);
        return false;
    }
}

bool NIDAQmx::WriteDigitalLines(
    std::string& taskName,
    int32_t numSampsPerChan,
    unsigned long autoStart,
    double timeout,
    double dataLayout,
    uint8_t writeArray[],
    int32_t* sampsPerChanWritten)
{
    if(m_tasks.contains(taskName)) {
        int32 _nSamps = static_cast<int32>(numSampsPerChan);
        bool32 _autoStart = static_cast<bool32>(autoStart);
        float64 _timeout = static_cast<float64>(timeout);
        bool32 _dataLayout = static_cast<bool32>(dataLayout);
        //uInt8 _writeArray[] = static_cast<uInt8[]>(writeArray);
        int32* _sampsWritten = reinterpret_cast<int32*>(sampsPerChanWritten);

        if (DAQmxWriteDigitalLines(m_tasks[taskName], _nSamps, _autoStart, _timeout, _dataLayout, writeArray, _sampsWritten, NULL) < 0) {
            spdlog::error("Failed to write lines value for task {}, ({})", taskName, GetExtendedErrorInfo());
            return false;
        }
        return true;
    } else {
        spdlog::error("Task {} does not exist", taskName);
        return false;
    }
}

std::string NIDAQmx::GetExtendedErrorInfo() {
    char buf[2048];
    DAQmxGetExtendedErrorInfo(buf, 2048);
    return std::string(buf);
}

