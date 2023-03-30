#include <string>
#include <sstream>
#include <vector>
#include <tuple>
#include <ranges>

#include <cxxopts.hpp>
#include <spdlog/spdlog.h>
#include <toml.hpp>

#include <PostProcess.h>


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

    PostProcess::AutoTile(indir, outdir, frames, rows, cols, width, height, false, true);

    return 0;
}
