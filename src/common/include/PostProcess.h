#ifndef POST_PROCESS_H
#define POST_PROCESS_H
#include <filesystem>
#include <string>

#include <spdlog/spdlog.h>

#include <TiffFile.h>
#include <ThreadPool.h>

namespace PostProcess {
    /** @brief Copies rows from tiff file into output buffer */
    void CopyTask(std::string inf, uint16_t* buf, size_t width, size_t height, size_t cols, bool vflip, bool hflip) {
        TiffFile t(inf);

        for (auto i = 0; i < height; i++) {
            size_t idx = width*cols*i;

            if (vflip) {
                t.Read16(buf+idx, (height - i - 1));
            } else {
                t.Read16(buf+idx, i);
            }

            if (hflip) {
                for (auto j = 0; j < width / 2; j++) {
                    std::swap(buf[j], buf[width-j-1]);
                }
            }
        }
        t.Close();
    }


    /** @brief Autotile images from indir to single tiff stack in outdir */
    void AutoTile(std::filesystem::path indir, std::filesystem::path outdir, size_t frames, size_t rows, size_t cols, size_t width, size_t height, bool vflip, bool hflip) {
        ThreadPool p(rows*cols);

        auto blockStart = [](uint8_t cols, uint8_t rowIdx, uint8_t colIdx, size_t width, size_t height) {
            return static_cast<size_t>((rowIdx * height) * (width * cols) + (colIdx * width));
        };

        spdlog::info("Tiling images from {} with rows: {}, cols: {}, frames: {}, width: {}, height: {}, vflip: {}, hflip: {}, thread count: {}", indir.string(), rows, cols, frames, width, height, vflip, hflip, p.ThreadCount());

        uint16_t *frameData = new uint16_t[rows * cols * width * height];
        TiffFile outTiff(outdir.string(), cols*width, rows*height, 16, frames, true);

        for (auto fr = 0; fr < frames; fr++) {
            //reset each frame
            memset((void*)frameData, 0, sizeof(uint16_t) * rows * cols * width * height);

            //read each image, 1-based index for file names
            for(auto row = 0; row < rows; row++) {
                for(auto col = 0; col < cols; col++) {
                    std::string f = fmt::format("default__{}_{:#04}.tiff", (col+1) + row * (rows + 1), fr);
                    size_t idx = blockStart(cols, row, col, width, height);
                    p.AddTask(CopyTask, (indir / f).string(), frameData+idx, width, height, cols, vflip, hflip);
                }
            }
            p.WaitForAll();
            outTiff.Write16(frameData);
        }

        outTiff.Close();
    }
};

#endif //POST_PROCESS_H
