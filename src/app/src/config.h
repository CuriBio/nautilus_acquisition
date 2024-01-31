#ifndef __NAUTILAI_CONFIG_H
#define __NAUTILAI_CONFIG_H
#include <stdlib.h>
#include <filesystem>
#include <string>
#include <vector>

#include <toml.hpp>
#include <cxxopts.hpp>
#include <interfaces/CameraInterface.h>

class Config {
    public:
        //nautilai options
        std::string prefix;
        bool noAutoConBright;
        std::filesystem::path path;
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

        //acquisition.region options
        Region rgn;

        //acquisition.live_view options
        bool enableLiveViewDuringAcquisition;
        bool vflip;
        bool hflip;

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

        //debug
        std::string testImgPath;
        bool ignoreErrors;
        bool asyncInit;

        //stage
        std::vector<std::pair<double, double>> stageLocations;

        //other
        std::string version;
        std::string configFile;

    public:
        Config(std::filesystem::path cfg, cxxopts::ParseResult userargs);
        ~Config() { };
        void Dump();
};

#endif //__NAUTILAI_CONFIG_H
