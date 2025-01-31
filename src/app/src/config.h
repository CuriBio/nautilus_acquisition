#ifndef __NAUTILAI_CONFIG_H
#define __NAUTILAI_CONFIG_H
#include <stdlib.h>
#include <filesystem>
#include <sstream>
#include <string>
#include <vector>
#include <tsl/ordered_map.h>

#include <toml.hpp>
#include <cxxopts.hpp>
#include <interfaces/CameraInterface.h>
#include <interfaces/AcquisitionInterface.h>


std::filesystem::path enableLongPath(std::filesystem::path path);

class Config {
    public:
        std::filesystem::path path;

        //nautilai options
        std::string prefix;
        bool noAutoConBright;
        std::filesystem::path userProfile;
        std::filesystem::path backgroundRecordingDir;
        std::filesystem::path plateFormat;
        std::filesystem::path extAnalysis;
        std::filesystem::path ffmpegDir;
        double xyPixelSize;
        std::filesystem::path machineVarsFilePath;
        toml::value machineVars;

        //acquisition options
        double fps;
        double duration;
        double ledIntensity;
        StorageType storageType;
        std::string storageTypeName;
        bool autoTile;
        bool encodeVideo;
        uint8_t rows;
        uint8_t cols;
        uint32_t bufferCount;
        uint32_t frameCount;
        double expTimeMs;
        std::vector<uint8_t> tileMap;
        bool enableDownsampleRawFiles;
        bool keepOriginalRaw;
        uint8_t binFactor;
        std::vector<std::string> storedPlateIds;
        std::string plateId;
        RecordingType recordingType;
        bool useBackgroundSubtraction;


        //acquisition.region options
        Region rgn;

        //acquisition.live_view options
        bool enableLiveViewDuringAcquisition;
        bool displayRoisDuringLiveView;
        bool vflip;
        bool hflip;

        //postprocess.video options
        tsl::ordered_map<std::string, uint16_t> videoQualityOptions;
        std::string selectedVideoQualityOption;

        //device.photometrics options
        int16_t triggerMode;
        std::string triggerModeName;
        int16_t exposureMode;
        std::string exposureModeName;
        uint16_t spdtable;

        //device.kinetix.line_read_times options
        std::vector<double> lineTimes;

        //device.nidaqmx
        std::string niDev;
        std::string trigDev;
        double maxVoltage;
        uint16_t shutterDelayMs;


        //device.tango
        std::string stageComPort;
        std::vector<int> stageStepSizes;

        //stage
        double dxCal;
        double dyCal;
        double theta;
        double scalingFactor;

        //disk
        std::string disk_name;
        std::string hd_serial_num;

        //debug
        std::string testImgPath;
        bool ignoreErrors;
        bool asyncInit;

        //other
        std::string version;
        std::string configFile;
        std::string configError;
        std::stringstream s;

        //updates
        bool updateAvailable;

    public:
        Config(std::filesystem::path cfg, std::filesystem::path profile, cxxopts::ParseResult userargs);
        ~Config() { };
        void Dump();
};

#endif //__NAUTILAI_CONFIG_H
