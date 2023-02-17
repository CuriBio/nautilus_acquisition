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
 * @file  NIDAQmx_wrapper.h
 *
 * Definition of the NIDAQmx_wrapper class.
 *********************************************************************/
#ifndef _NIDAQMX_WRAPPER_H
#define _NIDAQMX_WRAPPER_H

typedef void* TaskHandle;

#define DAQmx_Val_Volts 10348       // Volts
#define DAQmx_Val_GroupByChannel 0  // Group by Channel
#define DAQmx_Val_ChanForAllLines 1 // One Channel For All Lines


/*
* NIDAQmx wrapper library.
*/
class NIDAQmx {
    private:
        std::map<std::string, TaskHandle> m_tasks;

    public:
        /*
         * NIDAQmx constructor.
         */
        NIDAQmx();

        /*
         * NIDAQmx deconstructor.
         */
        ~NIDAQmx();

        /*
         * Create NIDAQmx task.
         *
         * @param taskName
         *
         * @return true if successful, false otherwise.
         */
        bool CreateTask(std::string& taskName);

        /*
         * Clear task.
         *
         * @param taskName
         * @return true if successful, false otherwise.
         */
        bool ClearTask(std::string& taskName);

        /*
         * Start task.
         *
         * @param taskName.
         *
         * @return true if successful, false otherwise.
         */
        bool StartTask(std::string& taskName);

        /*
         * Stop task.
         *
         * @param taskName
         *
         * @return true if successful, false otherwise.
         */
        bool StopTask(std::string& taskName);

        /*
         * Wait for task to stop.
         *
         * @param taskName
         * @param waitTimeS Time to wait for task to stop in seconds.
         *
         * @return true if successful, false otherwise.
         */
        bool WaitForTask(std::string& taskName, double waitTimeS);

        /*
         * Create analog output voltage channel.
         *
         * @param taskName
         * @param physicalChan
         * @param minVal
         * @param maxVal
         * @param units
         *
         * @return true if successful, false otherwise.
         */
        bool CreateAnalogOutpuVoltageChan(std::string& taskName, const char physicalChan[], double minVal, double maxVal, int32_t units);

        /*
         * Create digital output channel.
         *
         * @param taskName
         * @param lines Digit lines to create.
         * @param lineGrouping
         *
         * @return true if successful, false otherwise.
         */
        bool CreateDigitalOutputChan(std::string& taskName, const char lines[], int32_t lineGrouping);

        /*
         * Write digital lines to channel.
         *
         * @param taskName
         * @param numSampsPerChan
         * @param autoStart
         * @param timeout
         * @param dataLayout
         * @param writeArray
         * @param sampsPerChanWritten
         *
         * @return true if successful, false otherwise.
         */
        bool WriteDigitalLines(std::string& taskName, int32_t numSampsPerChan, unsigned long autoStart, double timeout, double dataLayout, uint8_t writeArray[], int32_t* sampsPerChanWritten);

        /*
         * Write analog out channel data.
         *
         * @param taskName
         * @param numSampsPerChan
         * @param autoStart
         * @param timeout
         * @param dataLayout
         * @param writeArray
         * @param sampsPerChanWritten
         *
         * @return true if successful, false otherwise.
         */
        bool WriteAnalogF64(std::string& taskName, int32_t numSampsPerChan, unsigned long autoStart, double timeout, double dataLayout, const double writeArray[], int32_t* sampsPerChanWritten);


        std::vector<std::string> GetListOfDevices();
    private:
        /*
         * Get latest error string.
         *
         * @return Error string.
         */
        std::string GetExtendedErrorInfo();
};
#endif //_NIDAQMX_WRAPPER_H
