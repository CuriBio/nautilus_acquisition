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
    /*
     * @brief PVCAM callback type.
     */
    using CbEx3Fn = void (PV_DECL *)(FRAME_INFO* frameInfo, void* context);


    /*
     * @brief NPV struct.
     *
     * Struct used for reading PVCAM enum values.
     */
    struct NVP {
        int32_t value{ 0 };
        std::string name{};
    };


   /*
    * @brief PVCAM get param exists helper function.
    *
    * Helper function to check if a param exists for connected camera.
    *
    * @param hcam Camera handle.
    * @param paramID The id of the param.
    */
    rs_bool pl_get_param_exists(int16_t hcam, uint32_t paramID);


   /*
    * @brief PVCAM get param helper function.
    *
    * Helper function to get a parameter if it exists for connected camera.
    *
    * @param hcam Camera handle.
    * @param paramID The ID of the parameter.
    * @param paramValue The output param value.
    *
    * @return true if successful, false otherwise.
    */
    rs_bool pl_get_param_if_exists(int16_t hcam, uint32_t paramID, int16_t paramAttr, void* paramValue);


   /*
    * @brief PVCAM set param helper function.
    *
    * Helper function to set PVCAM value if the parameter exists for connected camera.
    *
    * @param hcam Camera handle.
    * @param paramID The parameter ID.
    * @param paramValue The value to set.
    *
    * @return true if successful, false otherwise.
    */
    rs_bool pl_set_param_if_exists(int16_t hcam, uint32_t paramID, void* paramValue);

   /*
    * @brief PVCAM read enum helper function
    *
    * Helper function of reading enumeration from PVCAM.
    *
    * @param hcam Handle to active camera.
    * @param pNvpc Pointer to vector of NVP structs, holds enum values.
    * @param paramID The id of the enum parameter to read.
    *
    * @return true if successful, false otherwise.
    */
    bool pl_read_enum(int16_t hcam, std::vector<NVP>* pNvpc, uint32_t paramID);


   /*
    * @brief Camera context struct
    *
    * Struct used for context of each connected camera.
    *
    * @tparam F FrameConcept type
    */
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


    /*
     * @brief Camera controller class.
     *
     * Camera class interfacing with camera hardware.
     *
     * @tparam F FrameConcept type
     */
    template<FrameConcept F>
        class Camera {
            private:
                std::string m_version;
                int16_t m_camCount{0};
            public:
                std::shared_ptr<CameraCtx<F>> ctx{nullptr};
            public:

                /*
                 * @brief Camera constructor.
                 */
                Camera();

                /*
                 * @brief Camera destructor.
                 */
                ~Camera();

                /*
                 * @brief Opens a camera.
                 *
                 * Opens a connected camera for the given camera id.
                 *
                 * @param cameraId The camera id.
                 *
                 * @return true if successful, false otherwise.
                 */
                bool Open(int8_t cameraId);

                /*
                 * @brief Close the camera.
                 *
                 * Closes the currently opened camera.
                 *
                 * @return true if successful, false otherwise.
                 */
                bool Close();

                /*
                 * @brief Get connected camera info.
                 *
                 * Gets camera info for connected camera.
                 *
                 * @return CameraInfo structure reference for this camera.
                 */
                CameraInfo& GetInfo();

                /*
                 * @brief Update camera exposure.
                 *
                 * Updates the connected camera's exposure settings.
                 *
                 * @param settings Exposure settings structure.
                 *
                 * @return true if successful, false otherwise.
                 */
                bool UpdateExp(const ExpSettings& settings);

                /*
                 * @brief Set camera exposure.
                 *
                 * Sets the connected camera's initial exposure settings.
                 *
                 * @param settings Exposure settings structure.
                 *
                 * @return true if successful, false otherwise.
                 */
                bool SetupExp(ExpSettings& settings);

                /*
                 * @brief Start exposure.
                 *
                 * Starts exposure for this camera using provided eof callback and context.
                 *
                 * @param eofCallback Callback function PVCAM will call for each new frame.
                 * @param callbackCtx Context passed to each eofCallback call.
                 *
                 * @return true if successful, false otherwise.
                 */
                bool StartExp(void* eofCallback, void* callbackCtx);

                /*
                 * @brief Stops running exposure.
                 *
                 * Stops running exposure for this camera.
                 *
                 * @return true if successful, false otherwise.
                 */
                bool StopExp();

                /*
                 * @brief Get latest captured frame.
                 *
                 * Gets latest frame captured for running exposure.
                 *
                 * @param F Pointer to class that implements the FrameConcept.
                 *
                 * @return true if successful, false otherwise.
                 */
                bool GetLatestFrame(F* frame);

                /*
                 * @brief Get frame exposure time.
                 *
                 * Gets the exposure time for a given frame number
                 *
                 * @param frameNr The frame to get exp time for.
                 *
                 * @return The exposure time for given frame.
                 */
                uint32_t GetFrameExpTime(uint32_t frameNr);

                /*
                 * @brief Get last error.
                 *
                 * Gets the error string for last error in PVCAM.
                 *
                 * @return The error string.
                 */
                std::string GetError() const;
            private:
                /*
                 * @brief Removed camera handler.
                 *
                 * Callback handler for when the camera is removed.
                 *
                 * @param frameInfo Frame info pointer???
                 * @param ctx Callback camera context.
                 */
                static void rmCamHandler(FRAME_INFO* frameInfo, void* ctx);

                /*
                 * @brief Initializes speed table.
                 *
                 * Initalize internal speed table for this camera.
                 *
                 * @return true if successful, false otherwise.
                 */
                bool initSpeedTable();

                /*
                 * @brief Set up exposure settings.
                 *
                 * Set up exposure values for this camera.
                 *
                 * @return true if successful, false otherwise.
                 */
                bool setExp();

                /*
                 * @brief Update exposure settings.
                 *
                 * Update exposure values for this camera.
                 *
                 * @return true if successful, false otherwise.
                 */
                bool updateExp(const ExpSettings& settings);

                /*
                 * @brief Get latest frame index.
                 *
                 * Gets the index value of the latest frame.
                 *
                 * @param index The latest index.
                 *
                 * @return true if successful, false otherwise.
                 */
                bool getLatestFrameIndex(size_t& index);

                /*
                 * @brief Updates frame index map.
                 *
                 * Used internally, updates the map pointer to latest frame index.
                 *
                 * @param oldFrameNr The old frame number in the index map.
                 * @param index The index value.
                 */
                void updateFrameIndexMap(uint32_t oldFrameNr, size_t index) const;
        };
}

#endif //PMCAMERA_H
