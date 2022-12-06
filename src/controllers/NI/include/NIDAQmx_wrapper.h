#ifndef _NIDAQMX_WRAPPER_H
#define _NIDAQMX_WRAPPER_H

typedef void* TaskHandle;

#define DAQmx_Val_Volts 10348       // Volts
#define DAQmx_Val_GroupByChannel 0  // Group by Channel
#define DAQmx_Val_ChanForAllLines 1 // One Channel For All Lines

class NIDAQmx {
    private:
        std::map<std::string, TaskHandle> m_tasks;

    public:
        NIDAQmx();
        ~NIDAQmx();

        bool CreateTask(std::string& taskName);
        bool ClearTask(std::string& taskName);

        bool StartTask(std::string& taskName);
        bool StopTask(std::string& taskName);
        bool WaitForTask(std::string& taskName, double waitTimeS);

        bool CreateAnalogOutpuVoltageChan(std::string& taskName, const char physicalChan[], double minVal, double maxVal, int32_t units);
        bool CreateDigitalOutputChan(std::string& taskName, const char lines[], int32_t lineGrouping);

        bool WriteDigitalLines(std::string& taskName, int32_t numSampsPerChan, unsigned long autoStart, double timeout, double dataLayout, uint8_t writeArray[], int32_t* sampsPerChanWritten);
        bool WriteAnalogF64(std::string& taskName, int32_t numSampsPerChan, unsigned long autoStart, double timeout, double dataLayout, const double writeArray[], int32_t* sampsPerChanWritten);

    private:
        std::string GetExtendedErrorInfo();
};
#endif //_NIDAQMX_WRAPPER_H
