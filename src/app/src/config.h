#ifndef __NAUTILUS_CONFIG_H
#define __NAUTILUS_CONFIG_H
#include <filesystem>
#include <fmt/chrono.h>
#include <iostream>
#include <stdlib.h>
#include <vector>

#include <cxxopts.hpp>
#include <toml.hpp>
#include <tsl/ordered_map.h>

#include <interfaces/CameraInterface.h>

struct Config {
    std::string version;
    std::string path;
    std::string prefix;
    std::string niDev;
    std::string testImgPath;
    double fps;
    double duration;
    double expTimeMs;
    uint16_t spdtable;
    double ledIntensity;
    uint32_t bufferCount;
    uint32_t frameCount;
    StorageType storageType;
    int16_t triggerMode;
    int16_t exposureMode;
    double maxVoltage;
    bool noAutoConBright;
    bool autoTile;
    bool encodeVideo;
    bool vflip;
    bool hflip;
    uint8_t rows;
    uint8_t cols;
    double xyPixelSize;
    Region& rgn;
    std::string stageComPort;
    std::vector<int> stageStepSizes;
    std::string configFile;
    toml::basic_value<toml::preserve_comments, tsl::ordered_map>& config;
    std::vector<double> lineTimes;
};
#endif //__NAUTILUS_CONFIG_H
