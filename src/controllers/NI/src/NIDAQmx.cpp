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
 * @file  NIDAQmx.cpp
 *
 * @brief Implementation of the NIDAQmx wrapper class.
 *********************************************************************/
#include <stdint.h>
#include <map>

#include <spdlog/spdlog.h>
#include <NIDAQmx.h>
#include "NIDAQmx_wrapper.h"


/*
* @breif
*
*
*
* @param
*/
NIDAQmx::NIDAQmx() {
}


/*
* @breif
*
*
*
* @param
*/
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


/*
* @breif
*
*
*
* @param
*/
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


/*
* @breif
*
*
*
* @param
*/
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


/*
* @breif
*
*
*
* @param
*/
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


/*
* @breif
*
*
*
* @param
*/
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


/*
* @breif
*
*
*
* @param
*/
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


/*
* @breif
*
*
*
* @param
*/
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


/*
* @breif
*
*
*
* @param
*/
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


/*
* @breif
*
*
*
* @param
*/
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


/*
* @breif
*
*
*
* @param
*/
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


/*
* Get the list of all ni device names as a vector of string
*/
std::vector<std::string> NIDAQmx::GetListOfDevices(){
    constexpr size_t bufferSize = 1000;
    char devicenamesbuffer[bufferSize] = {};

    if(DAQmxFailed(DAQmxGetSysDevNames(devicenamesbuffer, bufferSize))){
        spdlog::error("NI device search failed");
        std::vector<std::string> empty;
        return empty;
    }else{
        std::string tempstring(devicenamesbuffer);
        std::vector<std::string> devicenamesbuffer;
        std::stringstream ss(tempstring);
        while(ss.good()){
            std::string nidevicename;
            getline(ss,nidevicename,',');
            if(nidevicename == ""){
                break;
            }
            devicenamesbuffer.push_back(nidevicename);
        }
        spdlog::info("NI search complete");
        return devicenamesbuffer;
    }
}


/*
* @breif
*
*
*
* @param
*/
std::string NIDAQmx::GetExtendedErrorInfo() {
    char buf[2048];
    DAQmxGetExtendedErrorInfo(buf, 2048);
    return std::string(buf);
}