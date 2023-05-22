#ifndef POST_PROCESS_H
#define POST_PROCESS_H
#include <filesystem>
#include <string>

#include <spdlog/spdlog.h>

#include <TiffFile.h>
#include <ThreadPool.h>
#include <RawFile.h>
#include <VideoEncoder.h>
#include <TaskFrameStats.h>
#include <TaskFrameLut16.h>
#include <TaskApplyLut16.h>

#ifdef _WIN64
#include <windows.h>
#endif

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
                std::reverse(buf+idx, buf+idx+width);
            }
        }
        t.Close();
    }

    void CopyRawTask(std::string inf, uint16_t* buf, uint32_t width, uint32_t height, size_t cols, bool vflip, bool hflip) {
        size_t size = width*height;
        uint16_t* data = nullptr;

        //MMAP file
#ifdef _WIN64
        HANDLE file = CreateFileA(inf.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
        if(file == INVALID_HANDLE_VALUE) {
            spdlog::error("Could not open file {}", inf);
            return;
        };

        HANDLE mapping  = CreateFileMapping(file, 0, PAGE_READONLY, 0, 0, 0);

        if(mapping == 0) {
            spdlog::error("Could not mmap file {}", inf);
            return;
        }

        data = (uint16_t*) MapViewOfFile(mapping, FILE_MAP_READ, 0, 0, 0);
#endif

        for (uint32_t i = 0; i < height; i++) {
            size_t idx = width*cols*i;

            if (vflip) {
                std::memcpy(buf+idx, data+((height - i - 1) * width), 2*width);
            } else {
                std::memcpy(buf+idx, data+(i*width), 2*width);
            }

            if (hflip) {
                std::reverse(buf+idx, buf+idx+width);
            }
        }

#ifdef _WIN64
        UnmapViewOfFile(data);
        CloseHandle(mapping);
        CloseHandle(file);
#endif
    }


    /** @brief Autotile images from indir to single tiff stack in outdir */
    void AutoTile(
        std::filesystem::path indir,
        std::string prefix,
        uint16_t frames,
        uint32_t rows,
        uint32_t cols,
        std::vector<uint8_t>& tileMap,
        uint32_t width,
        uint32_t height,
        bool vflip,
        bool hflip,
        bool autoConBright,
        std::function<void(size_t n)> progressCB,
        std::shared_ptr<RawFile<6>> r,
        StorageType storageType)
        //std::shared_ptr<VideoEncoder> v)
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
            std::vector<std::string> pendingDelete = {};

            //read each image, 1-based index for file names
            for(uint32_t row = 0; row < rows; row++) {
                for(uint32_t col = 0; col < cols; col++) {

                    size_t idx = blockStart(cols, row, col, width, height);
                    switch (storageType) {
                        case StorageType::Tiff:
                            {
                                std::string f = fmt::format("{}_{}_{:#04}.tiff", prefix, tileMap[col+row*cols]+1, fr);
                                pendingDelete.push_back((indir / f).string());
                                p.AddTask(CopyTask, (indir / f).string(), frameData+idx, width, height, cols, vflip, hflip);
                            }
                            break;
                        case StorageType::Raw:
                            {
                                std::string f = fmt::format("{}_{}_{:#04}.raw", prefix, tileMap[col+row*cols]+1, fr);
                                pendingDelete.push_back((indir / f).string());
                                p.AddTask(CopyRawTask, (indir / f).string(), frameData+idx, width, height, cols, vflip, hflip);
                            }
                            break;
                    };
                }
            }
            p.WaitForAll();

            // delete files in thread after copy task is finished
            std::thread deleteFilesT([&pendingDelete]() {
                for (auto &f : pendingDelete) { std::remove(f.c_str()); }
            });

            /* if (autoConBright && v) { */
            /*     uint32_t min{0}, max{0}, hmax{0}; */

            /*     uint32_t* hist = new uint32_t[(1<<16)-1]; */
            /*     memset((void*)hist, 0, sizeof(uint32_t)*((1<<16)-1)); */

            /*     uint16_t* img16 = new uint16_t[rows*height*cols*width]; */
            /*     memset((void*)img16, 0, sizeof(uint16_t)*rows*height*cols*width); */

            /*     TaskFrameStats frameStats(rows*cols); */
            /*     TaskFrameLut16 frameLut; */
            /*     TaskApplyLut16 applyLut; */

            /*     frameStats.Setup(frameData, hist, cols*width, rows*height); */
            /*     for (uint8_t i = 0; i < rows*cols; i++) { */
            /*         p.AddTask([&](uint8_t n) { frameStats.Run(rows*cols, n); }, i); */
            /*     } */
            /*     p.WaitForAll(); */
            /*     frameStats.Results(min, max, hmax); */

            /*     frameLut.Setup(min, max); */
            /*     for (uint8_t i = 0; i < rows*cols; i++) { */
            /*         p.AddTask([&](uint8_t n) { frameLut.Run(rows*cols, n); }, i); */
            /*     } */
            /*     p.WaitForAll(); */
            /*     uint16_t* lut16 = frameLut.Results(); */

            /*     applyLut.Setup(frameData, img16, lut16, rows*cols*width*height); */
            /*     for (uint8_t i = 0; i < rows*cols; i++) { */
            /*         p.AddTask([&](uint8_t n) { applyLut.Run(rows*cols, n); }, i); */
            /*     } */
            /*     p.WaitForAll(); */
            /*     v->writeFrame(img16, fr+1); */
            /* } else if (v) { */
            /*     v->writeFrame(frameData, fr+1); */
            /* } */

            r->Write(frameData, fr);
            progressCB(fr);

            //wait for delete to finish
            deleteFilesT.join();
        }
    }
};

#endif //POST_PROCESS_H
