#ifndef POST_PROCESS_H
#define POST_PROCESS_H
#include <filesystem>
#include <string>

#include <spdlog/spdlog.h>

#include <TiffFile.h>
#include <ThreadPool.h>
#include <TaskFrameStats.h>
#include <TaskFrameLut16.h>
#include <TaskApplyLut16.h>

namespace PostProcess {
    /** @brief Copies rows from tiff file into output buffer */
    void CopyTask(std::string inf, uint16_t* buf, uint32_t width, uint32_t height, size_t cols, bool vflip, bool hflip) {
        TiffFile t(inf);

        for (uint32_t i = 0; i < height; i++) {
            size_t idx = width*cols*i;

            if (vflip) {
                t.Read16(buf+idx, (height - i - 1));
            } else {
                t.Read16(buf+idx, i);
            }

            if (hflip) {
                for (uint32_t j = 0; j < width / 2; j++) {
                    std::swap(buf[j], buf[width-j-1]);
                }
            }
        }
        t.Close();
    }


    /** @brief Autotile images from indir to single tiff stack in outdir */
    void AutoTile(
        std::filesystem::path indir,
        std::filesystem::path outdir,
        uint16_t frames,
        uint32_t rows,
        uint32_t cols,
        uint32_t width,
        uint32_t height,
        bool vflip,
        bool hflip,
        bool autoConBright,
        std::function<void(void*, size_t)> writer)
    {
        ThreadPool p(static_cast<concurrency_t>(rows*cols));


        auto blockStart = [](uint8_t cols, uint8_t rowIdx, uint8_t colIdx, size_t width, size_t height) {
            return static_cast<size_t>((rowIdx * height) * (width * cols) + (colIdx * width));
        };

        spdlog::info("Tiling images from {} with rows: {}, cols: {}, frames: {}, width: {}, height: {}, vflip: {}, hflip: {}, thread count: {}", indir.string(), rows, cols, frames, width, height, vflip, hflip, p.ThreadCount());

        uint16_t *frameData = new uint16_t[rows * cols * width * height];

        for (auto fr = 0; fr < frames; fr++) {
            //reset each frame
            memset((void*)frameData, 0, sizeof(uint16_t) * rows * cols * width * height);

            //read each image, 1-based index for file names
            for(uint32_t row = 0; row < rows; row++) {
                for(uint32_t col = 0; col < cols; col++) {
                    std::string f = fmt::format("default__{}_{:#04}.tiff", (col+1) + row * (rows + 1), fr);
                    size_t idx = blockStart(cols, row, col, width, height);
                    p.AddTask(CopyTask, (indir / f).string(), frameData+idx, width, height, cols, vflip, hflip);
                }
            }
            p.WaitForAll();

            if (autoConBright) {
                uint32_t min{0}, max{0}, hmax{0};

                uint32_t* hist = new uint32_t[(1<<16)-1];
                memset((void*)hist, 0, sizeof(uint32_t)*((1<<16)-1));

                uint8_t* img8 = new uint8_t[rows*height*cols*width];
                memset((void*)img8, 0, sizeof(uint8_t)*rows*height*cols*width);

                TaskFrameStats frameStats(rows*cols);
                TaskFrameLut16 frameLut;
                TaskApplyLut16 applyLut;

                frameStats.Setup(frameData, hist, cols*width, rows*height);
                for (uint8_t i = 0; i < rows*cols; i++) {
                    p.AddTask([&](uint8_t n) { frameStats.Run(rows*cols, n); }, i);
                }
                p.WaitForAll();
                frameStats.Results(min, max, hmax);

                frameLut.Setup(min, max);
                for (uint8_t i = 0; i < rows*cols; i++) {
                    p.AddTask([&](uint8_t n) { frameLut.Run(rows*cols, n); }, i);
                }
                p.WaitForAll();
                uint8_t* lut8 = frameLut.Results();

                applyLut.Setup(frameData, img8, lut8, rows*cols*width*height);
                for (uint8_t i = 0; i < rows*cols; i++) {
                    p.AddTask([&](uint8_t n) { applyLut.Run(rows*cols, n); }, i);
                }
                p.WaitForAll();

                writer(img8, fr);
            } else {
                writer(frameData, fr);
            }
        }
    }
};

#endif //POST_PROCESS_H
