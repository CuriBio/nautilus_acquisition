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
      ("f,frames", "Number of input frames per capture", cxxopts::value<size_t>())
      ("r,rows", "Number of input rows for tiled image", cxxopts::value<uint8_t>()->default_value("2"))
      ("c,cols", "Number of input columns for tiled image", cxxopts::value<uint8_t>()->default_value("3"))
      ("w,width", "Width of each input image", cxxopts::value<size_t>())
      ("h,height", "Height of each input image", cxxopts::value<size_t>())
      ("codec", "Codec to use for video output", cxxopts::value<std::string>())
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

    uint8_t rows = userargs["rows"].as<uint8_t>();
    uint8_t cols = userargs["cols"].as<uint8_t>();
    size_t frames = userargs["frames"].as<size_t>(); 
    size_t width = userargs["width"].as<size_t>();
    size_t height = userargs["height"].as<size_t>();

    std::string codec = userargs["codec"].as<std::string>();

    std::shared_ptr<VideoEncoder> w = std::make_shared<VideoEncoder>((outdir / "default.avi"), codec, frames, cols*width, rows*height);
    w->Initialize();

    std::shared_ptr<RawFile> afw = std::make_shared<RawFile>((outdir / "defaul.raw"), 16, cols*width, rows*height, frames);

    PostProcess::AutoTile(
                    indir,
                    frames,
                    rows,
                    cols,
                    width,
                    height,
                    false,
                    true,
                    false,
                    [&](size_t n) {},
                    afw,
                    w
                );
    afw->Close();
    w->close();

    return 0;
}
