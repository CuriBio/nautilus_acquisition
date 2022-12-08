#include <stdio.h>

#include <NIDAQmx.h>
#include <spdlog/spdlog.h>

int32 __CFUNC DAQmxCreateAOVoltageChan(TaskHandle taskHandle, const char physicalChannel[], const char nameToAssignToChannel[], float64 minVal, float64 maxVal, int32 units, const char customScaleName[]) {
    spdlog::info("DAQmxCreateAOVoltageChan - chan: {}, name: {}, minVal: {}, maxVal: {}, units: {}, customScaleName: {}",
        physicalChannel,
        nameToAssignToChannel,
        minVal,
        maxVal,
        units,
        customScaleName
    );
}

int32 __CFUNC DAQmxCreateTask(const char taskName[], TaskHandle *taskHandle) {
    spdlog::info("DAQmxCreateTask, task {}", taskName);
}

int32 __CFUNC DAQmxStartTask(TaskHandle taskHandle) {
    spdlog::info("DAQmxStartTask");
}

int32 __CFUNC DAQmxStopTask(TaskHandle taskHandle) {
    spdlog::info("DAQmxStopTask");
}

int32 __CFUNC DAQmxClearTask(TaskHandle taskHandle) {
    spdlog::info("DAQmxClearTask");
}

int32 __CFUNC DAQmxWriteAnalogF64(TaskHandle taskHandle, int32 numSampsPerChan, bool32 autoStart, float64 timeout, bool32 dataLayout, const float64 writeArray[], int32 *sampsPerChanWritten, bool32 *reserved) {
    spdlog::info("DAQmxWriteAnalogF64 - numSampsPerChan: {}, autoStart: {}, timeout: {}, dataLayout: {}, sampsPerChanWritten: {}",
        numSampsPerChan,
        autoStart,
        timeout,
        dataLayout,
        *sampsPerChanWritten
    );
}

int32 __CFUNC DAQmxGetExtendedErrorInfo(char errorString[], uInt32 bufferSize) {
    spdlog::info("DAQmxGetExtendedErrorInfo - error: {}, bufferSize: {}", errorString, bufferSize);
}

int32 __CFUNC DAQmxWaitUntilTaskDone(TaskHandle taskHandle, float64 timeToWait) {
    spdlog::info("DAQmxWaitUntilTaskDone");
}

int32 __CFUNC DAQmxWriteDigitalLines (TaskHandle taskHandle, int32 numSampsPerChan, bool32 autoStart, float64 timeout, bool32 dataLayout, const uInt8 writeArray[], int32 *sampsPerChanWritten, bool32 *reserved) {
    spdlog::info("DAQmxWriteDigitalLines");
}

int32 __CFUNC DAQmxCreateDOChan(TaskHandle taskHandle, const char lines[], const char nameToAssignToLines[], int32 lineGrouping) {
    spdlog::info("DAQmxCreateDOChan");
}
