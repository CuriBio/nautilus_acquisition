#ifndef PMCAMERA_H
#define PMCAMERA_H
#include <cstdint>
#include <thread>
#include <mutex>
#include <vector>
#include <map>

#include <pvcam/master.h>
#include <pvcam/pvcam.h>

#include <CameraInterface.h>
#include <FrameInterface.h>

#include <pm/Config.h>
#include <pm/Settings.h>


namespace pm {
    //pvcam callback type
    using CbEx3Fn = void (PV_DECL *)(FRAME_INFO* frameInfo, void* context);

    struct NVP {
        int32 value{ 0 };
        std::string name{};
    };

    rs_bool pl_get_param_exists(int16 hcam, uns32 paramID);
    rs_bool pl_get_param_if_exists(int16 hcam, uns32 paramID, int16 paramAttr, void* paramValue);
    rs_bool pl_set_param_if_exists(int16 hcam, uns32 paramID, void* paramValue);
    bool pl_read_enum(int16 hcam, std::vector<NVP>* pNvpc, uns32 paramID);

    struct SpdtabGain {
        // In PVCAM, gain indexes are 1-based.
        int32 index{ 1 };
        // Not all cameras support named gains. If not supported, this
        // string stays empty.
        std::string name{};
        // The bit-depth may be different for each gain, therefore it is stored
        // within this structure. For example, the Prime BSI camera has gains
        // with different bit-depths under the same speed.
        int16 bitDepth{ 0 };
    };

    // This structure holds description of a speed entry.
    struct SpdtabSpeed {
        // In PVCAM, speed indexes are 0-based.
        int32 index{ 0 };
        // Pixel time can be used to calculate the overall readout rate. This is less
        // relevant with sCMOS sensors, but the pix time is still reported to provide
        // an approximate readout rate of a particular speed.
        uns16 pixTimeNs{ 1 };
        // List of gains under this particular speed.
        std::vector<SpdtabGain> gains{};
    };

    // This structure holds description of a port entry.
    struct SpdtabPort {
        int32 value{ 0 };
        std::string name{};
        std::vector<SpdtabSpeed> speeds{};
    };

    struct CameraInfo {
        std::string name{""};
        std::string driver{""};
        std::string firmware{""};
        std::string chip{""};

        // Camera sensor serial size (sensor width)
        uns16 sensorResX{0};
        // Camera sensor parallel size (sensor height)
        uns16 sensorResY{0};
    };

    template<FrameConcept F>
    struct CameraCtx {
        //Camera details
        CameraInfo info{};
        // Camera handle
        int16 hcam{-1};
        // is camera imaging
        bool imaging{false};
        //current capture settings
        std::unique_ptr<ExpSettings> curExp{nullptr};

        //speed table
        std::vector<SpdtabPort> spdtable;

        // Frame info structure used to store data, for example, in EOF callback handlers
        FRAME_INFO* curFrameInfo{nullptr};

        // The address of latest frame stored, for example, in EOF callback handlers
        void* eofFrame{nullptr};

        // Sensor region and binning factors to be used for the acquisition,
        // initialized to full sensor size with 1x1 binning upon opening the camera.
        rgn_type region { 0 };

        // Image format reported after acq. setup, value from PL_IMAGE_FORMATS
        int32 imageFormat{PL_IMAGE_FORMAT_MONO16};

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
        std::vector<std::shared_ptr<F>> frames;
        mutable std::map<uint32_t, size_t> framesMap{};

        std::mutex lock{};
    };


    template<FrameConcept F>
        class Camera {
            private:
                std::string m_version;
                int16 m_camCount{0};
            public:
                std::shared_ptr<CameraCtx<F>> ctx{nullptr};
            public:
                Camera();
                ~Camera();

                bool Open(int8_t cameraId);
                bool Close();
                bool SetupExp(const ExpSettings& settings);
                bool StartExp(void* eofCallback, void* callbackCtx);
                bool StopExp();

                bool GetLatestFrame(std::shared_ptr<F>);
                uint32_t GetFrameExpTime(uint32_t frameNr);
                std::string GetError() const;
            private:
                static void eofHandler(FRAME_INFO* frameInfo, void* ctx);
                static void rmCamHandler(FRAME_INFO* frameInfo, void* ctx);

                bool initSpeedTable();
                bool setExp(const ExpSettings& settings);
                bool getLatestFrameIndex(size_t& index);
                void updateFrameIndexMap(uint32_t oldFrameNr, size_t index) const;
        };
}

#endif //PMCAMERA_H
