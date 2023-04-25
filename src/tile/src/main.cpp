#include <string>
#include <sstream>
#include <vector>
#include <tuple>
#include <ranges>

#include <cxxopts.hpp>
#include <spdlog/spdlog.h>
#include <toml.hpp>
#include <tsl/ordered_map.h>

#include <PostProcess.h>
#include <VideoEncoder.h>
#include <RawFile.h>

#ifndef _WIN32
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#else
#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
#endif


int main(int argc, char* argv[]) {
    cxxopts::Options options("Nautilus AutoTile", "CuriBio");
    options.add_options()
      ("o,outdir", "Output directory", cxxopts::value<std::string>())
      ("i,indir", "Input directory", cxxopts::value<std::string>())
      ("f,frames", "Number of input frames per capture", cxxopts::value<uint16_t>())
      ("r,rows", "Number of input rows for tiled image", cxxopts::value<uint16_t>()->default_value("2"))
      ("c,cols", "Number of input columns for tiled image", cxxopts::value<uint16_t>()->default_value("3"))
      ("w,width", "Width of each input image", cxxopts::value<uint32_t>())
      ("h,height", "Height of each input image", cxxopts::value<uint32_t>())
      ("vflip", "Vertical flip", cxxopts::value<bool>()->default_value("false"))
      ("hflip", "Horizontal flip", cxxopts::value<bool>()->default_value("false"))
      ("autocb", "Auto contrast/brightness", cxxopts::value<bool>()->default_value("false"))
      ("codec", "Codec to use for video output", cxxopts::value<std::string>())
      ("fps", "Frames per second for video encoding", cxxopts::value<int>()->default_value("20"))
      ("help", "Usage")
      ;

    auto userargs = options.parse(argc, argv);


    if (userargs.count("help")) {
        std::cout << options.help() << std::endl;
        exit(0);
    }

    std::filesystem::path indir{};
    if (userargs.count("indir")) {
        indir = userargs["indir"].as<std::string>();
        spdlog::info("Using input directory {}", indir.string());
    } else {
        spdlog::info("Missing input directory");
        std::cout << options.help() << std::endl;
        exit(0);
    }
    std::filesystem::path outdir{};

    if (userargs.count("outdir")) {
        outdir = userargs["outdir"].as<std::string>();
    }
    spdlog::info("Using output directory {}", outdir.string());

    uint16_t rows = userargs["rows"].as<uint16_t>();
    uint16_t cols = userargs["cols"].as<uint16_t>();

    uint32_t width = userargs["width"].as<uint32_t>();
    uint32_t height = userargs["height"].as<uint32_t>();

    uint16_t frames = userargs["frames"].as<uint16_t>();
    int fps = userargs["fps"].as<int>();

    std::string codec = userargs["codec"].as<std::string>();

    bool vflip = userargs["vflip"].as<bool>();
    bool hflip = userargs["hflip"].as<bool>();
    bool autocb = userargs["autocb"].as<bool>();


    std::shared_ptr<VideoEncoder> w = std::make_shared<VideoEncoder>((outdir / "default.avi"), codec, fps, static_cast<size_t>(cols*width), static_cast<size_t>(rows*height));
    w->Initialize();

    std::shared_ptr<RawFile> afw = std::make_shared<RawFile>((outdir / "defaul.raw"), 16, static_cast<uint16_t>(cols*width), static_cast<uint16_t>(rows*height), frames);

    PostProcess::AutoTile(
                    indir,
                    "default_",
                    frames,
                    rows,
                    cols,
                    width,
                    height,
                    vflip,
                    hflip,
                    autocb,
                    [&](size_t n) {},
                    afw,
                    w
                );
    afw->Close();
    w->close();

    return 0;
}
