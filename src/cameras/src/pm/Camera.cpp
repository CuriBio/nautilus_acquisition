#include <cstdint>
#include <vector>
#include <ranges>
#include <string>
#include <thread>
#include <mutex>

#include <spdlog/spdlog.h>
#include <pvcam/master.h>
#include <pvcam/pvcam.h>

#include <interfaces/FrameInterface.h>

#include <pm/Frame.h>
#include <pm/Camera.h>



/**
 * PVCAM HELPER FUNCTIONS
 */
rs_bool pm::pl_get_param_exists(int16 hcam, uns32 paramID) {
    rs_bool available = false;
    if(PV_OK != pl_get_param(hcam, paramID, ATTR_AVAIL, (void*)&available)) { return false; }
    return available;
}

rs_bool pm::pl_get_param_if_exists(int16 hcam, uns32 paramID, int16 paramAttr, void* paramValue) {
    if (PV_OK != pm::pl_get_param_exists(hcam, paramID)) {
        return false;
    }
    return pl_get_param(hcam, paramID, paramAttr, (void*)paramValue);
}

bool pm::pl_read_enum(int16 hcam, std::vector<NVP>* pNvpc, uns32 paramID) {
    if (!pNvpc) { return false; }

    uns32 count;
    if(PV_OK != pm::pl_get_param_if_exists(hcam, paramID, ATTR_COUNT, (void*)&count)) {
        return false;
    }

    std::vector<NVP> nvpc;
    for (uns32 i = 0; i < count; ++i) {
        // Retrieve the enum string length
        uns32 strLength;
        if (PV_OK != pl_enum_str_length(hcam, paramID, i, &strLength)) { return false; }

        // Allocate the destination string
        char* name = new char[strLength];
        if (!name) { return false; }

        // Get the string and value
        int32 value;
        if (PV_OK != pl_get_enum_param(hcam, paramID, i, &value, name, strLength)) { return false; }

        NVP nvp { .value = value, .name = name, };

        nvpc.push_back(nvp);
        delete [] name;
    }
    pNvpc->swap(nvpc);

    return !pNvpc->empty();
}


rs_bool pm::pl_set_param_if_exists(int16 hcam, uns32 paramID, void* paramValue) {
    if(pm::pl_get_param_exists(hcam, paramID)) {
        return pl_set_param(hcam, paramID, paramValue);
    } else {
        spdlog::info("Param {} does not exist", paramID);
        return false;
    }
}

template<FrameConcept F>
pm::Camera<F>::Camera() {
    if (PV_OK != pl_pvcam_init()) {
        spdlog::error("Failure initializing PVCAM");
    }

    uns16 version;
    if (PV_OK != pl_pvcam_get_ver(&version)) {
        spdlog::error("Failed get PVCAM version");
    }

    if (PV_OK != pl_cam_get_total(&m_camCount)) {
        spdlog::error("Failure getting camera count");
    }

    m_version = fmt::format("{}.{}.{}",
            (version >> 8) & 0xFF,
            (version >> 4) & 0x0F,
            (version >> 0) & 0x0F);

    spdlog::info("Using PCVAM version {}", m_version);
}


template<FrameConcept F>
pm::Camera<F>::~Camera() {
    if (ctx) {
        if(ctx->curFrameInfo) {
            (void)pl_release_frame_info_struct(ctx->curFrameInfo);
            ctx->curFrameInfo = nullptr;
        }
        if(ctx->curExp) {
            ctx->curExp = nullptr;
        }
        //TODO cleanup frames buffer
    }

    if (PV_OK != pl_pvcam_uninit()) {
        spdlog::error("Failed to uninit pvcam library");
    }
}


template<FrameConcept F>
bool pm::Camera<F>::Open(int8_t cameraId) {
    if (cameraId > m_camCount) {
        spdlog::error("cameraId out of range");
        return false;
    }

    //open camera
    char camName[CAM_NAME_LEN];
    if (PV_OK != pl_cam_get_name(cameraId, &camName[0])) {
        spdlog::error("Failed to get name for camera ({})", GetError());
        return false;
    }

    ctx = std::make_unique<CameraCtx<F>>();
    ctx->info.name = camName;

    if (!ctx) {
        spdlog::warn("No camera found for id {}", cameraId);
        return false;
    }

    //lock mutex
    std::lock_guard<std::mutex> lock(ctx->lock);

    if (PV_OK != pl_create_frame_info_struct(&ctx->curFrameInfo)) {
        spdlog::error("Failed creating frame info structure, ({})", GetError());
        return false;
    }

    if (PV_OK != pl_cam_open((char*)ctx->info.name.c_str(), &ctx->hcam, OPEN_EXCLUSIVE)) {
        spdlog::error("Failed opening camera {}, ({})", ctx->info.name.c_str(), GetError());
        ctx->hcam = -1;

        (void)pl_release_frame_info_struct(ctx->curFrameInfo);
        ctx->curFrameInfo = nullptr;
        return false;
    }

    // Device Driver version
    uns16 ddVersion;
    if (PV_OK != pm::pl_get_param_if_exists(ctx->hcam, PARAM_DD_VERSION, ATTR_CURRENT, (void*)&ddVersion)) {
        spdlog::error("Failed getting device driver version, ({})", GetError());
        return false;
    }

    ctx->info.driver = fmt::format("{}.{}.{}",
            (ddVersion >> 8) & 0xFF,
            (ddVersion >> 4) & 0x0F,
            (ddVersion >> 0) & 0x0F);
    spdlog::info("Found driver version {}", ctx->info.driver);

    // Read the camera sensor parallel size (sensor height/number of rows)
    if (!pm::pl_get_param_if_exists(ctx->hcam, PARAM_SER_SIZE, ATTR_CURRENT, (void*)&ctx->info.sensorResX)) {
        spdlog::error("Could not read CCD X-resolution for camera {}, ({})", ctx->info.name, GetError());
        return false;
    }

    if (PV_OK != pl_get_param(ctx->hcam, PARAM_PAR_SIZE, ATTR_CURRENT, (void*)&ctx->info.sensorResY)) {
        spdlog::error("Could not read CCD Y-resolution for camera {}, ({})", ctx->info.name, GetError());
        return false;
    }

    spdlog::info("Sensor size: {}x{} px", ctx->info.sensorResX, ctx->info.sensorResY);

    // Reset PP features to their default values.
    (void)pl_pp_reset(ctx->hcam);

    //get speed table
    if (!initSpeedTable()) {
        spdlog::error("Failed to init speed table for camera {}", ctx->info.name);
    }

    /* if (PV_OK != pl_set_param(ctx->hcam, PARAM_READOUT_PORT, (void*)&ctx->spdtable[0].value)) { */
    /*     spdlog::error("Readout port could not be set"); */
    /*     return false; */
    /* } */

    /* if (PV_OK != pl_set_param(ctx->hcam, PARAM_SPDTAB_INDEX, (void*)&ctx->spdtable[0].speeds[0].index)) { */
    /*     spdlog::error("Readout speed index could not be set"); */
    /*     return false; */
    /* } */

    /* if (PV_OK != pl_set_param(ctx->hcam, PARAM_GAIN_INDEX, (void*)&ctx->spdtable[0].speeds[0].gains[0].index)) { */
    /*     spdlog::info("Gain index could not be set"); */
    /*     return false; */
    /* } */

    // Set the number of sensor clear cycles to 2 (default).
    // This is mostly relevant to CCD cameras only and it has
    // no effect with CLEAR_NEVER or CLEAR_AUTO clearing modes 
    // typically used with sCMOS cameras.
    uns16 clearCycles = 2;
    if(!pm::pl_set_param_if_exists(ctx->hcam, PARAM_CLEAR_CYCLES, (void*)&clearCycles)) {
        spdlog::error("Failed to set PARAM_CLEAR_CYCLES, ({})", GetError());
        return false;
    }
    spdlog::info("Set CLEAR_CYCLES: {}", clearCycles);

    // Find out if the sensor is a frame transfer or other (typically interline)
    // type. This process is relevant for CCD cameras only.
    if(PV_OK != pm::pl_get_param_if_exists(ctx->hcam, PARAM_FRAME_CAPABLE, ATTR_CURRENT, (void*)&ctx->isFrameTransfer)) {
        spdlog::warn("Failed to get PARAM_FRAME_CAPABLE value");
    }
    spdlog::info("Camera supports Frame Transfer capability: {}", ctx->isFrameTransfer ? "true" : "false");

    int32 pMode = (ctx->isFrameTransfer)? PMODE_FT : PMODE_NORMAL;
    if(!pm::pl_set_param_if_exists(ctx->hcam, PARAM_PMODE, (void*)&pMode)) {
        spdlog::warn("Unable to set PMODE for camera {}", ctx->info.name);
    }
    spdlog::info("Set PMODE: {}", (ctx->isFrameTransfer) ? "PMODE_FT" : "PMODE_NORMAL");

    ctx->isSmartStreaming = pm::pl_get_param_exists(ctx->hcam, PARAM_SMART_STREAM_MODE);
    spdlog::info("Smart Streaming availability: {}", ctx->isSmartStreaming);

    //get exposure modes
    std::vector<NVP> exposureModes;
    if(!pm::pl_read_enum(ctx->hcam, &exposureModes, PARAM_EXPOSE_OUT_MODE)) {
        spdlog::error("Failed to read exposure modes");
    } else {
        spdlog::info("Exposure modes:");
        for (auto n : exposureModes) {
            spdlog::info("  {}: {}", n.name, n.value);
        }
    }


    //get trigger modes
    std::vector<NVP> triggerModes;
    if(!pm::pl_read_enum(ctx->hcam, &triggerModes, PARAM_EXPOSURE_MODE)) {
        spdlog::error("Failed to read trigger modes");
    } else {
        spdlog::info("Trigger modes:");
        for (auto n : triggerModes) {
            spdlog::info("  {}: {}", n.name, n.value);
        }
    }

    //get clear modes
    std::vector<NVP> clearModes;
    if(!pm::pl_read_enum(ctx->hcam, &clearModes, PARAM_CLEAR_MODE)) {
        spdlog::error("Failed to read clear modes");
    } else {
        spdlog::info("Clear modes:");
        for (auto n : clearModes) {
            spdlog::info("  {}: {}", n.name, n.value);
        }
    }

    //get color modes
    std::vector<NVP> colorModes;
    if(!pm::pl_read_enum(ctx->hcam, &colorModes, PARAM_COLOR_MODE)) {
        spdlog::error("Failed to read color modes");
    } else {
        spdlog::info("Clear modes:");
        for (auto n : colorModes) {
            spdlog::info("  {}: {}", n.name, n.value);
        }
    }

    std::vector<NVP> imageFormats;
    if(!pm::pl_read_enum(ctx->hcam, &imageFormats, PARAM_IMAGE_FORMAT)) {
        spdlog::error("Failed to read color modes");
    } else {
        spdlog::info("Image formats:");
        for (auto n : imageFormats) {
            spdlog::info("  {}: {}", n.name, n.value);
        }
    }

    spdlog::info("Registering PL_CALLBACK_CAM_REMOVED handler");
    if (PV_OK != pl_cam_register_callback_ex3(
                ctx->hcam,
                PL_CALLBACK_CAM_REMOVED, (void*)&pm::Camera<F>::rmCamHandler,
                &ctx
                )) {
        spdlog::error("Unable to register camera removal callback, ({})", GetError());
        return false;
    }

    return true;
}


template<FrameConcept F>
bool pm::Camera<F>::Close() {
    if (!ctx) {
        spdlog::error("Camera not found");
        return false;
    }

    //lock mutex
    std::lock_guard<std::mutex> lock(ctx->lock);

    if (PV_OK != pl_cam_deregister_callback(ctx->hcam, PL_CALLBACK_CAM_REMOVED)) {
        spdlog::error("Failed to unregister camera removal callback for camera {}", ctx->info.name);
    } 

    if (PV_OK != pl_cam_close(ctx->hcam)) {
        spdlog::error("Failed to close camera {}", ctx->info.name);
    }

    if (PV_OK != pl_release_frame_info_struct(ctx->curFrameInfo)) {
        spdlog::error("Failure releasing frame info structure for camera {}", ctx->info.name);
    }

    //TODO Delete buffer
    if (ctx->curFrameInfo) {
        delete ctx->curFrameInfo;
        ctx->curFrameInfo = nullptr;
    }

    ctx->hcam = -1;
    ctx->curFrameInfo = nullptr;
    return true;
}


template<FrameConcept F>
CameraInfo& pm::Camera<F>::GetInfo() {
    return ctx->info;
}


template<FrameConcept F>
bool pm::Camera<F>::StopExp() {
    if (ctx->imaging) {
        std::lock_guard<std::mutex> lock(ctx->lock); //lock mutex

        if(ctx->imaging) {
            if (PV_OK != pl_exp_abort(ctx->hcam, CCS_HALT)) {
                spdlog::error("Failed to abort acquisition, error ignored ({})", GetError());
            }

            if (PV_OK != pl_exp_finish_seq(ctx->hcam, ctx->buffer.get(), 0)) {
                spdlog::error("Failed to finish sequence, error ignored ({})", GetError());
            }

            ctx->imaging = false;

            // Do not deregister callbacks before pl_exp_abort, abort could freeze then
            if (PV_OK != pl_cam_deregister_callback(ctx->hcam, PL_CALLBACK_EOF)) {
                spdlog::error("Failed to deregister EOF callback, error ignored ({})", GetError());
            }
        }
    }

    return true;
}


template<FrameConcept F>
bool pm::Camera<F>::SetupExp(const ExpSettings& settings) {
    if (!ctx) {
        spdlog::error("No camera found");
        return false;
    }

    //lock mutex
    std::lock_guard<std::mutex> lock(ctx->lock);

    //return if already imaging
    if (ctx->imaging) {
        spdlog::error("Camera:SetupExp, Imaging already running for camera {}", ctx->info.name);
        return false;
    }

    //TODO clear out old settings if they exist
    if (ctx->curExp) {
        spdlog::info("Deleting Exp settings for camera {}", ctx->info.name);
    }

    return setExp(settings);
}


template<FrameConcept F>
std::string pm::Camera<F>::GetError() const {
    std::string message;
    char errMsg[ERROR_MSG_LEN] = "\0";

    const int16 code = pl_error_code();
    if (PV_OK != pl_error_message(code, errMsg)) {
        message = std::string("Unable to get error message for error code ")
            + std::to_string(code);
    }
    else {
        message = errMsg;
    }

    return message;
}


template<FrameConcept F>
bool pm::Camera<F>::StartExp(void* eofCallback, void* callbackCtx) {
    if (!ctx) {
        spdlog::error("No camera");
        return false;
    }

    std::lock_guard<std::mutex> lock(ctx->lock); //lock mutex
    if (ctx->imaging) { 
        spdlog::error("Camera::StartExp, Imaging already running for camera {}", ctx->info.name);
        return false;
    }

    //clear out old settings if they exist
    if (!ctx->curExp) {
        spdlog::error("Exp settings not found for camera {}", ctx->info.name);
        return false;
    }

    if (PV_OK != pl_cam_register_callback_ex3(ctx->hcam, PL_CALLBACK_EOF, (void*)eofCallback, callbackCtx)) {
        spdlog::error("Failed to register EOF callback");
        return false;
    }

    switch(ctx->curExp->acqMode) {
        case AcqMode::LiveCircBuffer:
            spdlog::info("Starting Continuous Acquisition, cam: {}, bufferCount: {}, frameBytes: {}, Exposure Mode: {}, Trigger Mode: {}",
                ctx->hcam,
                ctx->curExp->bufferCount,
                ctx->frameBytes,
                ctx->curExp->expModeOut,
                ctx->curExp->trigMode
            );

            if(PV_OK != pl_exp_start_cont(ctx->hcam, ctx->buffer.get(),
                        ctx->curExp->bufferCount * ctx->frameBytes)) {
                //TODO: clean up
                spdlog::error("Failed to start continuous acquisition, {}", GetError());
                return false;
            }
            break;
        case AcqMode::SnapCircBuffer:
        case AcqMode::SnapSequence:
        case AcqMode::SnapTimeLapse:
        case AcqMode::LiveTimeLapse:
            //TODO Implement
            return false;
    }

    spdlog::info("Acquisition started on camera {}", ctx->info.name);
    ctx->imaging = true;

    return true;
}

template<FrameConcept F>
bool pm::Camera<F>::GetLatestFrame(F* frame) {
    //TODO implement
    size_t index;
    if (!getLatestFrameIndex(index)) {
        return false;
    }

    //TODO invalidate frame
    //frame.Invalidate();
    return frame->Copy(*ctx->frames[index], false);
}

template<FrameConcept F>
uint32_t pm::Camera<F>::GetFrameExpTime(uint32_t frameNr) {
    //TODO implement all these
    /* if (m_settings.GetTrigMode() == VARIABLE_TIMED_MODE) */
    /* { */
    /*     const auto& vtmExposures = m_settings.GetVtmExposures(); */
    /*     // frameNr is 1-based, not 0-based */
    /*     const uint32_t vtmExpIndex = (frameNr - 1) % vtmExposures.size(); */
    /*     const uint16_t vtmExpTime = vtmExposures.at(vtmExpIndex); */
    /*     return static_cast<uint32_t>(vtmExpTime); */
    /* } */
    /* else if (!m_smartExposures.empty()) */
    /* { */
    /*     const uint32_t ssExpIndex = (frameNr - 1) % m_smartExposures.size(); */
    /*     return m_smartExposures.at(ssExpIndex); */
    /* } */

    return ctx->curExp->expTimeMS;
}

/**
 * Camera private methods
 */

template<FrameConcept F>
bool pm::Camera<F>::initSpeedTable() {
    std::vector<NVP> ports;

    if(!ctx) {
        spdlog::error("No camera");
        return false;
    }

    if (!pm::pl_read_enum(ctx->hcam, &ports, PARAM_READOUT_PORT)) { return false; }
    if (!pm::pl_get_param_exists(ctx->hcam, PARAM_SPDTAB_INDEX)) { return false; }
    if (!pm::pl_get_param_exists(ctx->hcam, PARAM_PIX_TIME)) { return false; }
    if (!pm::pl_get_param_exists(ctx->hcam, PARAM_GAIN_INDEX)) { return false; }
    if (!pm::pl_get_param_exists(ctx->hcam, PARAM_BIT_DEPTH)) { return false; }
    if (!pm::pl_get_param_exists(ctx->hcam, PARAM_GAIN_NAME)) { return false; }

    // Iterate through available ports and their speeds
    for (size_t pi = 0; pi < ports.size(); pi++) {
        // Set readout port
        if (PV_OK != pl_set_param(ctx->hcam, PARAM_READOUT_PORT, (void*)&ports[pi].value)) {
            return false;
        }

        // Get number of available speeds for this port
        uns32 speedCount;
        if (PV_OK != pl_get_param(ctx->hcam, PARAM_SPDTAB_INDEX, ATTR_COUNT, (void*)&speedCount)) {
            return false;
        }

        // Iterate through all the speeds
        for (int16 si = 0; si < (int16)speedCount; si++) {
            // Set camera to new speed index
            if (PV_OK != pl_set_param(ctx->hcam, PARAM_SPDTAB_INDEX, (void*)&si)) {
                return false;
            }

            // Get pixel time (readout time of one pixel in nanoseconds) for the
            // current port/speed pair. This can be used to calculate readout
            // frequency of the port/speed pair.
            uns16 pixTime;
            if (PV_OK != pl_get_param(ctx->hcam, PARAM_PIX_TIME, ATTR_CURRENT, (void*)&pixTime)) {
                return false;
            }

            uns32 gainCount;
            if (PV_OK != pl_get_param(ctx->hcam, PARAM_GAIN_INDEX, ATTR_COUNT, (void*)&gainCount)) {
                return false;
            }


            // Iterate through all the gains, notice it starts at value 1!
            for (int16 gi = 1; gi <= (int16)gainCount; gi++) {
                // Set camera to new gain index
                if (PV_OK != pl_set_param(ctx->hcam, PARAM_GAIN_INDEX, (void*)&gi)) {
                    return false;
                }

                // Get bit depth for the current gain
                int16 bitDepth;
                if (PV_OK != pl_get_param(ctx->hcam, PARAM_BIT_DEPTH, ATTR_CURRENT, (void*)&bitDepth)) {
                    return false;
                }

                char gainName[MAX_GAIN_NAME_LEN];
                if (PV_OK != pl_get_param(ctx->hcam, PARAM_GAIN_NAME, ATTR_CURRENT, (void*)gainName)) {
                    return false;
                }

                SpdTable tbl {
                    .gainIndex=gi,
                    .bitDepth=bitDepth,
                    .gainName=gainName,
                    .spdIndex=si,
                    .pixTimeNs=pixTime,
                    .spdTabPort=ports[pi].value,
                    .portName=ports[pi].name,
                };
                ctx->info.spdTable.push_back(tbl);

            }
        }
    }

    return true;
}


template<FrameConcept F>
bool pm::Camera<F>::setExp(const ExpSettings& settings) {
    if (!ctx) {
        spdlog::error("No camera");
        return false;
    }
    if (ctx->imaging) {
        spdlog::warn("Camera is already capturing");
        return false;
    }

    //copy capture settings
    ctx->curExp.reset(new ExpSettings{
            .acqMode = settings.acqMode,
            .filePath = settings.filePath,
            .filePrefix = settings.filePrefix,
            .region = settings.region,
            .imgFormat = settings.imgFormat,
            .spdTableIdx = settings.spdTableIdx,
            .expTimeMS = settings.expTimeMS,
            .trigMode = settings.trigMode,
            .expModeOut = settings.expModeOut,
            .frameCount = settings.frameCount,
            .bufferCount = settings.bufferCount,
            .colorWbScaleRed = settings.colorWbScaleRed,
            .colorWbScaleGreen = settings.colorWbScaleGreen,
            .colorWbScaleBlue = settings.colorWbScaleBlue
        });

    auto stIdx = settings.spdTableIdx;
    if (PV_OK != pl_set_param(ctx->hcam, PARAM_READOUT_PORT, (void*)&ctx->info.spdTable[stIdx].spdTabPort)) {
        spdlog::error("Readout port could not be set, ({})", GetError());
        return false;
    }
    spdlog::info("Readout port set to {}", ctx->info.spdTable[stIdx].portName);

    if (PV_OK != pl_set_param(ctx->hcam, PARAM_SPDTAB_INDEX, (void*)&ctx->info.spdTable[stIdx].spdIndex)) {
        spdlog::error("Readout speed index could not be set, ({})", GetError());
        return false;
    }
    spdlog::info("Readout speed index set to {}", ctx->info.spdTable[stIdx].spdIndex);

    if (PV_OK != pl_set_param(ctx->hcam, PARAM_GAIN_INDEX, (void*)&ctx->info.spdTable[stIdx].gainIndex)) {
        spdlog::error("Gain index could not be set, ({})", GetError());
        return false;
    }
    spdlog::info("Gain index set to {}", ctx->info.spdTable[stIdx].gainIndex);
    spdlog::info("speed table: running at {} MHz", 1000 / (float)ctx->info.spdTable[stIdx].pixTimeNs);

    if (PV_OK != pl_get_param_if_exists(ctx->hcam, PARAM_IMAGE_FORMAT, ATTR_CURRENT, (void*)&ctx->curExp->imgFormat)) {
        spdlog::error("Failed to get IMAGE_FORMAT, ({})", GetError());
    }


    uns32 exposure = (settings.trigMode == VARIABLE_TIMED_MODE) ? 1 : settings.expTimeMS;
    rgn_type rgn = {
        .s1 = settings.region.s1,
        .s2 = settings.region.s2,
        .sbin = settings.region.sbin,
        .p1 = settings.region.p1,
        .p2 = settings.region.p2,
        .pbin = settings.region.pbin
    };

    switch(settings.acqMode) {
        case AcqMode::SnapCircBuffer:
        case AcqMode::LiveCircBuffer:
            //TODO allow multiple regions
            if (PV_OK != pl_exp_setup_cont(
                        //TODO Support regions
                        ctx->hcam, 1/*rgn_total*/, &rgn, settings.expModeOut | settings.trigMode, exposure, &ctx->frameBytes, CIRC_OVERWRITE)) {
                spdlog::error("Failed to setup continuous acquisition, ({})", GetError());
                return false;
            }
            spdlog::info("frameBytes: {}", ctx->frameBytes);
            break;
        default:
            spdlog::error("Acquisition mode not supported/implemented");
            return false;
    }

    if (ctx->bufferBytes) { //need to deallocate old buffers first
        ctx->frames.clear();
        ctx->buffer.reset();

        ctx->bufferBytes = 0;
        ctx->buffer = nullptr;
    }

    //PVCAM, at least the virtual cam, will only allow up to 4GB buffer
    //so allocate as much as allowed here
    uint32_t maxBuffers = uint32_t((0xFFFFFFFF >> 1) / ctx->frameBytes);
    spdlog::info("MaxBuffers {}", maxBuffers);
    ctx->curExp->bufferCount = (ctx->curExp->bufferCount == 0) ? maxBuffers : ctx->curExp->bufferCount;

    //allocate buffer, example code mentions error with PCIe data, to fix it adds 16
    //to the buffer size, so going to do that here
    ctx->bufferBytes = ctx->curExp->bufferCount * ctx->frameBytes + 16;
    spdlog::info("Allocating bufferBytes ({}) for bufferCount ({}) with frameBytes ({})", ctx->bufferBytes, ctx->curExp->bufferCount, ctx->frameBytes);

    //TODO use 4k alignment, might parameterize later
    const size_t sizeAligned = (ctx->bufferBytes + (0x1000 - 1)) & ~(0x1000 - 1);
    ctx->buffer = std::make_unique<uns8[]>(sizeAligned);
    spdlog::info("Allocated buffer[{}] for {} frames", ctx->bufferBytes, ctx->curExp->bufferCount);

    //setup frames over buffer
    ctx->frames.reserve(ctx->curExp->bufferCount);
    for(uint32_t i = 0; i < ctx->curExp->bufferCount; i++) {
        F* frame = new F(ctx->frameBytes, false, nullptr);

        void* data = ctx->buffer.get() + i * ctx->frameBytes;
        frame->SetData(data);
        if(!frame->CopyData()) {
            spdlog::error("Allocating frames failed");
            return false;
        }
        ctx->frames.push_back(frame);
    }
    ctx->frames.shrink_to_fit();
    spdlog::info("Allocated frames ready");

    return true;
}

template<FrameConcept F>
void pm::Camera<F>::updateFrameIndexMap(uint32_t oldFrameNr, size_t index) const {
    ctx->framesMap.erase(oldFrameNr);

    if (index >= ctx->frames.size()) {
        spdlog::info("updateFrameIndex index out of range");
        return;
    }

    const uint32_t frameNr = ctx->frames.at(index)->GetInfo()->frameNr;
    ctx->framesMap[frameNr] = index;
}

template<FrameConcept F>
bool pm::Camera<F>::getLatestFrameIndex(size_t& index) {
    // Set to an error state before PVCAM tries to reset pointer to valid frame location
    void* data = nullptr;

    // Get the latest frame
    if (PV_OK != pl_exp_get_latest_frame_ex(ctx->hcam, &data, ctx->curFrameInfo)) {
        spdlog::error("Failed to get latest frame from PVCAM {}, {}", ctx->info.name, GetError());
        return false;
    }

    if (!data) {
        spdlog::error("Invalid frame pointer");
        return false;
    }

    //TODO handle differnt modes
    // Fix the frame number which is always 1 in time lapse mode

    //TODO Check for valid acquisition configuration
    const size_t frameBytes = ctx->frameBytes;
    if (frameBytes == 0) {
        spdlog::error("Invalid acquisition configuration");
        return false;
    }

    const size_t offset = static_cast<uint8_t*>(data) - ctx->buffer.get();
    const size_t idx = offset / frameBytes;
    if (idx * frameBytes != offset) {
        spdlog::error("Invalid frame offset");
        return false;
    }

    if (ctx->frames[idx]->GetData() != data) {
        spdlog::error("Frame data address does not match");
        return false;
    }
    index = idx;

    //TODO invalidate frame
    /* m_frames[index]->Invalidate(); // Does some cleanup */
    /* m_frames[index]->OverrideValidity(true); */

    const uint32_t oldFrameNr = ctx->frames[index]->GetInfo()->frameNr;

    const FrameInfo fi {
        .frameNr = (uint32_t)ctx->curFrameInfo->FrameNr,
        .timestampBOF = (uint64_t)ctx->curFrameInfo->TimeStampBOF,
        .timestampEOF = (uint64_t)ctx->curFrameInfo->TimeStamp,
        .expTime = GetFrameExpTime((uint32_t)ctx->curFrameInfo->FrameNr),
        .colorWbScaleRed = ctx->curExp->colorWbScaleRed,
        .colorWbScaleGreen = ctx->curExp->colorWbScaleGreen,
        .colorWbScaleBlue = ctx->curExp->colorWbScaleBlue
    };
    ctx->frames[index]->SetInfo(fi);
    updateFrameIndexMap(oldFrameNr, index);

    return true;
}


/**
 * Camera callback methods
 */

template<FrameConcept F>
void pm::Camera<F>::rmCamHandler(FRAME_INFO* frameInfo, void* ctx) {
    spdlog::info("RM Handler called");
    return;
}

//Avoid link errors
template class pm::Camera<pm::Frame>;
