#ifndef POST_PROCESS_H
#define POST_PROCESS_H
#include <filesystem>
#include <string>
#include <vector>
#include <numeric>

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
    
    /** @brief Downsample images with user-defined bin factor */
    void Downsample(
        int fr,
        uint16_t *frameData,
        std::shared_ptr<RawFile<6>> r,
        uint32_t width,
        uint32_t height,
        uint8_t binFactor)
    {   
        uint32_t binnedWidth = width / binFactor;
        uint32_t binnedHeight = height / binFactor;
        uint16_t *binnedFrameData = new uint16_t[binnedWidth * binnedHeight];

        for (uint32_t i = 0; i < binnedHeight; i++) {
            for (uint32_t j = 0; j < binnedWidth; j++) {

                int currentRow = i * binFactor * width;
                int firstIdx = currentRow + j * binFactor;

                std::vector<uint16_t> pixelsToAvg;

                for (uint8_t k = 0; k < binFactor; k++) {

                    uint32_t start = firstIdx + (width * k);
                    uint32_t end = start + binFactor;
                    
                    for (uint32_t l = start; l < end; l++) {
                        pixelsToAvg.push_back(frameData[l]);
                    }
                }

                uint16_t average = std::reduce(pixelsToAvg.begin(), pixelsToAvg.end()) / pixelsToAvg.size();
                binnedFrameData[(i*binnedWidth) + j] = average;
            }
        }

        r->Write(binnedFrameData, fr); 
    }

    /** @brief Autotile images from indir to single tiff stack in outdir */
    void AutoTile(
        std::filesystem::path indir,
        std::string prefix,
        uint32_t frames,
        uint32_t rows,
        uint32_t cols,
        std::vector<uint8_t>& tileMap,
        std::vector<bool>& tileEnabled,
        uint32_t width,
        uint32_t height,
        bool vflip,
        bool hflip,
        bool autoConBright,
        std::function<void(size_t n)> progressCB,
        std::shared_ptr<RawFile<6>> r,
        std::shared_ptr<RawFile<6>> r2,
        StorageType storageType,
        uint8_t binFactor)
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
                    auto curr = col+row*cols;
                    if (!tileEnabled[curr]) {
                        continue;
                    }

                    auto tileIdx = tileMap[curr];
                    size_t blockStartIdx = blockStart(cols, row, col, width, height);
                    switch (storageType) {
                        case StorageType::Tiff:
                            {
                                std::string f = fmt::format("{}_{}_{:#04}.tiff", prefix, tileIdx+1, fr);
                                p.AddTask(CopyTask, (indir / f).string(), frameData+blockStartIdx, width, height, cols, vflip, hflip);
                            }
                            break;
                        case StorageType::Raw:
                            {
                                std::string f = fmt::format("{}_{}_{:#04}.raw", prefix, tileIdx+1, fr);
                                p.AddTask(CopyRawTask, (indir / f).string(), frameData+blockStartIdx, width, height, cols, vflip, hflip);
                            }
                            break;
                    };
                }
            }
            p.WaitForAll();
            r->Write(frameData, fr);
            
            if (r2 != nullptr) {
                spdlog::info("Downsampling frame {}", fr);
                // pass in complete pixels, rows/columns do not matter at this point
                Downsample(fr, frameData, r2, cols*width, rows*height, binFactor);
            }

            progressCB(1);
        }
    }

};


#endif //POST_PROCESS_H
