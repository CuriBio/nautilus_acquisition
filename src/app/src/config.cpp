#include <spdlog/spdlog.h>
#include <toml.hpp>
#include <tsl/ordered_map.h>

#include "config.h"
#include <pm/Camera.h>

Config::Config(std::filesystem::path cfg, cxxopts::ParseResult userargs) {
    toml::value config;
    try {
        config = toml::parse<toml::preserve_comments, tsl::ordered_map>(cfg.string());
    } catch(const std::exception& e) {
        spdlog::error("Failed to parse config file \"{}\"", e.what());
    }
    configFile = cfg.string();

    machineVarsFilePath = toml::find_or<std::string>(config, "nautilai", "machine_vars_file_path", std::string(""));

    try {
        machineVars = toml::parse<toml::preserve_comments, tsl::ordered_map>(machineVarsFilePath.string());
    } catch(const std::exception& e) {
        spdlog::error("Failed to parse machine vars file \"{}\"", e.what());
    }

    try {
        //nautilus table options
        path = toml::find<std::string>(config, "nautilai", "outdir");
        if (userargs.count("outdir")) { path = userargs["outdir"].as<std::string>(); }

        prefix = toml::find<std::string>(config, "nautilai", "prefix");
        if (userargs.count("prefix")) { prefix = userargs["prefix"].as<std::string>(); }

        noAutoConBright = !toml::find<bool>(config, "nautilai", "auto_contrast_brightness");
        if (userargs.count("no_autocb")) { noAutoConBright = true; }

        plateFormat = toml::find<std::string>(config, "nautilai", "plate_format");
        if (userargs.count("plateFormat")) { plateFormat = userargs["plate_format"].as<std::string>(); }

        extAnalysis = toml::find<std::string>(config, "nautilai", "ext_analysis");
        if (userargs.count("ext_analysis")) { extAnalysis = userargs["ext_analysis"].as<std::string>(); }

        ffmpegDir = toml::find<std::string>(config, "nautilai", "ffmpeg_dir");
        if (userargs.count("ffmpeg_dir")) { ffmpegDir = userargs["ffmpeg_dir"].as<std::string>(); }

        xyPixelSize = toml::find<double>(machineVars, "nautilai", "xy_pixel_size");

        //acquisition table options
        fps = toml::find<double>(config, "acquisition", "fps");
        if (userargs.count("fps")) { fps = userargs["fps"].as<double>(); }


        duration = toml::find<double>(config, "acquisition", "duration");
        if (userargs.count("duration")) { duration = userargs["duration"].as<double>(); }


        ledIntensity = toml::find<double>(config, "acquisition", "led_intensity");
        if (userargs.count("led")) { ledIntensity = userargs["led"].as<double>(); }


        int storage_type = toml::find<int>(config, "acquisition", "storage_type");
        if (userargs.count("storage_type")) { storage_type = userargs["storage_type"].as<int>(); }


        switch (storage_type) {
            case 0:
                storageType = StorageType::Tiff;
                storageTypeName = "tiff";
                break;
            case 1:
                storageType = StorageType::BigTiff;
                storageTypeName = "big tiff";
                break;
            case 2:
                spdlog::info("Storage type: {}", "raw");
                storageType = StorageType::Raw;
                storageTypeName = "raw";
                break;
            default:
                spdlog::error("Invalid storage type");
                if (!ignoreErrors) { exit(0); }
        }

        autoTile = toml::find<bool>(config, "acquisition", "auto_tile");
        encodeVideo = toml::find<bool>(config, "acquisition", "encode_video");
        rows = toml::find<uint8_t>(config, "acquisition", "rows");
        cols = toml::find<uint8_t>(config, "acquisition", "cols");
        tileMap = toml::find<std::vector<uint8_t>>(config, "acquisition", "tile_map");
        bufferCount = toml::find<uint32_t>(config, "acquisition", "buffers");
        if (userargs.count("buffers")) { bufferCount = userargs["buffers"].as<uint32_t>(); }

        frameCount = static_cast<uint32_t>(duration * fps);
        expTimeMs = 1000 * (1.0 / fps);
        enableDownsampleRawFiles = false;
        binFactor = 2;
        keepOriginalRaw = false;

        //acquisition.region
        uint16_t s1 = toml::find<uint16_t>(config, "acquisition", "region", "s1");
        uint16_t s2 = toml::find<uint16_t>(config, "acquisition", "region", "s2", 2399);
        uint16_t sbin = toml::find<uint16_t>(machineVars, "acquisition", "region", "sbin");
        uint16_t p1 = toml::find<uint16_t>(config, "acquisition", "region", "p1");
        uint16_t p2 = toml::find<uint16_t>(config, "acquisition", "region", "p2");
        uint16_t pbin = toml::find<uint16_t>(machineVars, "acquisition", "region", "pbin");

        rgn = Region {
            .s1 = s1, .s2 = s2, .sbin = sbin,
            .p1 = p1, .p2 = p2, .pbin = pbin
        };

        //acquisition.live_view
        enableLiveViewDuringAcquisition = toml::find<bool>(config, "acquisition", "live_view", "enable_live_view_during_acquisition");
        vflip = toml::find<bool>(machineVars, "acquisition", "live_view", "vflip");
        hflip = toml::find<bool>(machineVars, "acquisition", "live_view", "hflip");

        //device.photometrics options
        triggerMode = toml::find<int16_t>(config, "device", "photometrics", "trigger_mode");

        if (userargs.count("trigger_mode")) { triggerMode = userargs["trigger_mode"].as<int16_t>(); }

        switch (triggerMode) {
            case EXT_TRIG_INTERNAL:
                triggerModeName = "EXT_TRIG_INTERNAL";
                break;
            case EXT_TRIG_TRIG_FIRST:
                triggerModeName = "EXT_TRIG_TRIG_FIRST";
                break;
            case EXT_TRIG_EDGE_RISING:
                triggerModeName = "EXT_TRIG_EDGE_RISING";
                break;
            case EXT_TRIG_LEVEL:
                triggerModeName = "EXT_TRIG_LEVEL";
                break;
            case EXT_TRIG_SOFTWARE_FIRST:
                triggerModeName = "EXT_TRIG_SOFTWARE_FIRST";
                break;
            case EXT_TRIG_SOFTWARE_EDGE:
                triggerModeName = "EXT_TRIG_SOFTWARE_EDGE";
                break;
            case EXT_TRIG_LEVEL_OVERLAP:
                triggerModeName = "EXT_TRIG_LEVEL_OVERLAP";
                break;
            case EXT_TRIG_LEVEL_PULSED:
                triggerModeName = "EXT_TRIG_LEVEL_PULSED";
                break;
            default:
                triggerModeName = "INVALID";
                if (!ignoreErrors) { exit(0); }
        }


        exposureMode = toml::find<int16_t>(config, "device", "photometrics", "exposure_mode");
        if (userargs.count("exposure_mode")) { exposureMode = userargs["exposure_mode"].as<int16_t>(); }

        switch (exposureMode) {
            case EXPOSE_OUT_FIRST_ROW:
                exposureModeName = "EXPOSE_OUT_FIRST_ROW";
                break;
            case EXPOSE_OUT_ALL_ROWS:
                exposureModeName = "EXPOSE_OUT_ALL_ROWS";
                break;
            case EXPOSE_OUT_ANY_ROW:
                exposureModeName = "EXPOSE_OUT_ANY_ROW";
                break;
            case EXPOSE_OUT_ROLLING_SHUTTER:
                exposureModeName = "EXPOSE_OUT_ROLLING_SHUTTER";
                break;
            case EXPOSE_OUT_LINE_TRIGGER:
                exposureModeName = "EXPOSE_OUT_LINE_TRIGGER";
                break;
            case EXPOSE_OUT_GLOBAL_SHUTTER:
                exposureModeName = "EXPOSE_OUT_GLOBAL_SHUTTER";
                break;
            case MAX_EXPOSE_OUT_MODE:
                exposureModeName = "MAX_EXPOSE_OUT_MODE";
                break;
            default:
                exposureModeName = "INVALID";
                if (!ignoreErrors) { exit(0); }
                break;
        }

        spdtable = toml::find<uint16_t>(config, "device", "photometrics", "speed_table_index");
        if (userargs.count("spdtable")) { spdtable = userargs["spdtable"].as<uint16_t>(); }


        //device.kinetix.line_read_times options
        lineTimes = {
            toml::find<double>(config, "device", "kinetix", "line_read_times", "sensitivity"),
            toml::find<double>(config, "device", "kinetix", "line_read_times", "speed"),
            toml::find<double>(config, "device", "kinetix", "line_read_times", "dynamic_range"),
            toml::find<double>(config, "device", "kinetix", "line_read_times", "sub_electron")
        };

        //device.nidaqmx options
        niDev = toml::find<std::string>(config, "device", "nidaqmx", "device");
        if (userargs.count("ni_dev")) { niDev = userargs["ni_dev"].as<std::string>(); }

        trigDev = toml::find<std::string>(config, "device", "nidaqmx", "device_2");
        if (userargs.count("trig_dev")) { trigDev = userargs["trig_dev"].as<std::string>(); }

        maxVoltage = toml::find<double>(machineVars, "device", "nidaqmx", "max_voltage");
        if (userargs.count("max_voltage")) { maxVoltage = userargs["max_voltage"].as<double>(); }

        shutterDelayMs = toml::find<uint16_t>(machineVars, "device", "nidaqmx", "max_voltage");
        if (userargs.count("shutter_delay_ms")) { shutterDelayMs = userargs["shutter_delay_ms"].as<uint16_t>(); }

        //device.tango
        stageComPort = toml::find<std::string>(machineVars, "device", "tango", "com");
        if (userargs.count("stage_com_port")) { stageComPort = userargs["stage_com_port"].as<std::string>(); }

        stageStepSizes = {
            toml::find<int>(config, "device", "tango", "step_small"),
            toml::find<int>(config, "device", "tango", "step_medium"),
            toml::find<int>(config, "device", "tango", "step_large")
        };

        //stage.locations
        stageLocations = {};
        for (auto& v : toml::find<std::vector<toml::table>>(config, "stage", "location")) {
            auto x = static_cast<double>(v.at("x").as_floating());
            auto y = static_cast<double>(v.at("y").as_floating());
            stageLocations.push_back(std::pair(x,y));
        }

    } catch(const std::out_of_range& e) {
        spdlog::error("Missing required config values \"{}\"", e.what());
    }

    //debug
    testImgPath = "";
    if (userargs.count("test_img")) { testImgPath = userargs["test_img"].as<std::string>(); }
    ignoreErrors = toml::find_or<bool>(config, "debug", "ignore_errors", false);

    asyncInit = toml::find_or<bool>(config, "debug", "async_init", true);
    
}

void Config::Dump() {
    //nautilai options
    spdlog::info("nautilai.outdir: {}", path.string());
    spdlog::info("nautilai.prefix: {}", prefix);
    spdlog::info("nautilai.auto_contrast_brightness: {}", !noAutoConBright);
    spdlog::info("nautilai.plate_format {}", plateFormat.string());
    spdlog::info("nautilai.ext_analysis {}", extAnalysis.string());
    spdlog::info("nautilai.ffmpeg_dir {}", ffmpegDir.string());
    spdlog::info("nautilai.xy_pixel_size: {}", xyPixelSize);
    spdlog::info("nautilai.machine_vars_file_path: {}", machineVarsFilePath.string());

    //acquisition options
    spdlog::info("acquisition.fps: {}", fps);
    spdlog::info("acquisition.duration: {}", duration);
    spdlog::info("acquisition.led_intensity: {}", ledIntensity);
    spdlog::info("acquisition.buffers: {}", bufferCount);
    spdlog::info("acquisition.frameCount: {}", frameCount);
    spdlog::info("acquisition.expTimeMs: {}", expTimeMs);
    spdlog::info("acquisition.tile_map: [{}]", fmt::join(tileMap, ", "));

    spdlog::info("acquisition.storage_type: {} ({})", storageType, storageTypeName);
    spdlog::info("acquisition.auto_tile: {}", autoTile);
    spdlog::info("acquisition.encode_video: {}", encodeVideo);
    spdlog::info("acquisition.rows: {}", rows);
    spdlog::info("acquisition.cols: {}", cols);

    //acquisition.region
    spdlog::info("acquisition.region.s1: {}", rgn.s1);
    spdlog::info("acquisition.region.s2: {}", rgn.s2);
    spdlog::info("acquisition.region.sbin: {}", rgn.sbin);
    spdlog::info("acquisition.region.p1: {}", rgn.p1);
    spdlog::info("acquisition.region.p2: {}", rgn.p2);
    spdlog::info("acquisition.region.pbin: {}", rgn.pbin);


    //acquisition.live_view
    spdlog::info("acquisition.live_view.vflip: {}", vflip);
    spdlog::info("acquisition.live_view.hflip: {}", hflip);

    //device.photometrics
    spdlog::info("device.photometrics.trigger_mode  {} ({})", triggerMode, triggerModeName);
    spdlog::info("device.photometrics.exposure_mode  {} ({})", exposureMode, exposureModeName);
    spdlog::info("device.photometrics.speed_table_index: {}", spdtable);

    //device.kinetix.line_read_times
    spdlog::info("device.kinetix.line_read_times.sensitivity: {}", lineTimes[0]); 
    spdlog::info("device.kinetix.line_read_times.speed: {}", lineTimes[1]); 
    spdlog::info("device.kinetix.line_read_times.dynamic_range: {}", lineTimes[2]);
    spdlog::info("device.kinetix.line_read_times.sub_electron: {}", lineTimes[3]); 

    //device.nidaqmx
    spdlog::info("device.nidaqmx.device: {}", niDev);
    spdlog::info("device.nidaqmx.device_2: {}", trigDev);
    spdlog::info("device.nidaqmx.max_voltage: {}", maxVoltage);
    spdlog::info("device.nidaqmx.shutter_delay_ms: {}", shutterDelayMs);

    //device.tango
    spdlog::info("device.tango.com_port: {}", stageComPort);
    spdlog::info("device.tango.step_small: {}", stageStepSizes[0]);
    spdlog::info("device.tango.step_medium: {}", stageStepSizes[1]);
    spdlog::info("device.tango.step_large: {}", stageStepSizes[2]);

    //stage.locations
    spdlog::info("stage.locations length: {}", stageLocations.size());

    //debug
    spdlog::info("debug.ignore_errors: {}", ignoreErrors);
    spdlog::info("debug.async_init: {}", asyncInit);
}
