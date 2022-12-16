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
 * @file  Camera.h
 * 
 * @brief Definition of the Camera class.
 *********************************************************************/
#ifndef PMCAMERA_H
#define PMCAMERA_H
#include <cstdint>
#include <thread>
#include <mutex>
#include <vector>
#include <map>

#include <pvcam/master.h>
#include <pvcam/pvcam.h>

#include <interfaces/CameraInterface.h>
#include <interfaces/FrameInterface.h>
#include <BitmapFormat.h>


namespace pm {
    //pvcam callback type
    using CbEx3Fn = void (PV_DECL *)(FRAME_INFO* frameInfo, void* context);

    struct NVP {
        int32_t value{ 0 };
        std::string name{};
    };

    rs_bool pl_get_param_exists(int16_t hcam, uint32_t paramID);
    rs_bool pl_get_param_if_exists(int16_t hcam, uint32_t paramID, int16_t paramAttr, void* paramValue);
    rs_bool pl_set_param_if_exists(int16_t hcam, uint32_t paramID, void* paramValue);
    bool pl_read_enum(int16_t hcam, std::vector<NVP>* pNvpc, uint32_t paramID);

    template<FrameConcept F>
    struct CameraCtx {
        //Camera details
        CameraInfo info{};
        // Camera handle
        int16_t hcam{-1};
        // is camera imaging
        bool imaging{false};
        //current capture settings
        std::unique_ptr<ExpSettings> curExp{nullptr};

        // Frame info structure used to store data, for example, in EOF callback handlers
        FRAME_INFO* curFrameInfo{nullptr};

        // The address of latest frame stored, for example, in EOF callback handlers
        void* eofFrame{nullptr};

        /* // Sensor region and binning factors to be used for the acquisition, */
        /* // initialized to full sensor size with 1x1 binning upon opening the camera. */
        /* rgn_type region { 0 }; */

        /* // Image format reported after acq. setup, value from PL_IMAGE_FORMATS */
        /* int32 imageFormat{PL_IMAGE_FORMAT_MONO16}; */
        ImageFormat imgFormat {ImageFormat::Mono16};

        //current bitdepth
        uint16_t bitDepth;

        // Sensor type (if not Frame Transfer CCD then camera is Interline CCD or sCMOS).
        // Not relevant for sCMOS sensors.
        bool isFrameTransfer{false};

        // Flag marking the camera as Smart Streaming capable
        bool isSmartStreaming{false};

        // Event used for communication between acq. loop and EOF callback routine
        //Event eofEvent{};

        uns32 frameBytes{0};
        uint64_t bufferBytes{0};

        std::unique_ptr<uns8[]> buffer{ nullptr };
        std::vector<F*> frames;
        mutable std::map<uint32_t, size_t> framesMap{};

        std::mutex lock{};
    };


    template<FrameConcept F>
        class Camera {
            private:
                std::string m_version;
                int16_t m_camCount{0};
            public:
                std::shared_ptr<CameraCtx<F>> ctx{nullptr};
            public:
                Camera();
                ~Camera();

                bool Open(int8_t cameraId);
                bool Close();
                CameraInfo& GetInfo();
                bool SetupExp(const ExpSettings& settings);
                bool StartExp(void* eofCallback, void* callbackCtx);
                bool StopExp();

                bool GetLatestFrame(F* frame);
                uint32_t GetFrameExpTime(uint32_t frameNr);
                std::string GetError() const;
            private:
                static void rmCamHandler(FRAME_INFO* frameInfo, void* ctx);

                bool initSpeedTable();
                bool setExp(const ExpSettings& settings);
                bool getLatestFrameIndex(size_t& index);
                void updateFrameIndexMap(uint32_t oldFrameNr, size_t index) const;
        };
}

#endif //PMCAMERA_H
