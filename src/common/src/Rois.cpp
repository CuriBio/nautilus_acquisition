#include <Rois.h>
#include <fmt/format.h>

namespace Rois {
    std::vector<std::tuple<uint32_t, uint32_t>> roiOffsets(RoiCfg* roi, size_t frameWidth, size_t frameHeight) {
        std::vector<std::tuple<uint32_t, uint32_t>> roi_offsets = {};

        double mid_x = frameWidth / 2.0;
        double mid_y = frameHeight / 2.0;
        double well_spacing_scaled = roi->well_spacing / (roi->xy_pixel_size * roi->scale);
        double h_offset_scaled = roi->h_offset / roi->scale;
        double v_offset_scaled = roi->v_offset / roi->scale;

        double top_x = mid_x - 0.5 * (roi->cols - 1) * well_spacing_scaled - (roi->width / (2 * roi->scale)) + h_offset_scaled;
        double top_y = mid_y - 0.5 * (roi->rows - 1) * well_spacing_scaled - (roi->height / (2 * roi->scale)) + v_offset_scaled;

        for (size_t r = 0; r < roi->rows; r++) {
            double y = top_y + (r * well_spacing_scaled);
            for (size_t c = 0; c < roi->cols; c++) {
                double x = top_x + (c * well_spacing_scaled);
                roi_offsets.push_back(std::make_tuple(uint32_t(x), uint32_t(y)));
            }
        }

        return roi_offsets;
    }

    std::string wellName(uint32_t row, uint32_t col) {
        row += 1;
        std::string name =  (row > 0) ? "" : "A";

        while (row > 0) {
            char r = (row - 1) % 26;
            name = static_cast<char>('A' + r) + name;
            row = (row - r) / 26;
        }

        return name + std::to_string(col+1);
    }

    uint32_t roiToOffset(uint32_t x, uint32_t y, uint32_t width) {
        return y * width + x;
    }

    /*
     * Generate a string containing a "crop" filter that can be passed to FFmpeg so it will crop out overlapping regions
     *
     * Sample cfg = {
     *   well_spacing = 9000;
     *   xy_pixel_size = 41.67;
     *   scale = 2;
     *   num_wells_v = 4;
     *   num_wells_h = 4;
     *   frameWidth = 512;
     *   frameHeight = 512;
     *   rows = 2;
     *   cols = 3;
     *   v_offset = 0;
     *   h_offset = 0;
     * }
     *
     * Sample output for sample cfg (well_spacing_scaled = 108, so the 4x4 region preserved per FOV is 432x432):
     *
     * split=6[L_0_0][L_0_1][L_0_2][L_1_0][L_1_1][L_1_2];
     * [L_0_0] crop=432:432:40:40 [L_0_0_c];
     * [L_0_1] crop=432:432:552:40 [L_0_1_c];
     * [L_0_2] crop=432:432:1064:40 [L_0_2_c];
     * [L_1_0] crop=432:432:40:552 [L_1_0_c];
     * [L_1_1] crop=432:432:552:552 [L_1_1_c];
     * [L_1_2] crop=432:432:1064:552 [L_1_2_c];
     * [L_0_0_c][L_0_1_c][L_0_2_c] hstack=inputs=3 [R0];
     * [L_1_0_c][L_1_1_c][L_1_2_c] hstack=inputs=3 [R1];
     * [R0][R1] vstack"
     *
     * See https://www.ffmpeg.org/ffmpeg-filters.html for more details
     */
    std::string getFFmpegCropFilter(RoiCfg* roi, size_t frameWidth, size_t frameHeight) {
        double mid_x = frameWidth / 2.0;
        double mid_y = frameHeight / 2.0;
        double well_spacing_scaled = roi->well_spacing / (roi->xy_pixel_size * roi->scale);
        double h_offset_scaled = roi->h_offset / roi->scale;
        double v_offset_scaled = roi->v_offset / roi->scale;

        uint32_t x_offset = std::round(mid_x - 0.5 * roi->cols * well_spacing_scaled + h_offset_scaled);
        uint32_t y_offset = std::round(mid_y - 0.5 * roi->rows * well_spacing_scaled + v_offset_scaled);

        uint32_t x_crop_size = std::round(roi->cols * well_spacing_scaled);
        uint32_t y_crop_size = std::round(roi->rows * well_spacing_scaled);

        std::string split_cmd = fmt::format("split={}", roi->fovRows * roi->fovCols);
        std::string crop_cmd = "";
        std::string hstack_cmd = "";
        std::string vstack_cmd = "";

        for (uint32_t r = 0; r < roi->fovRows; r++) {
            uint32_t y = y_offset + (r * frameHeight);

            for (uint32_t c = 0; c < roi->fovCols; c++) {
                uint32_t x = x_offset + (c * frameWidth);

                std::string inLabel = fmt::format("L_{}_{}", r, c);
                std::string outLabel = inLabel + "_c";  // for "cropped"

                split_cmd += fmt::format("[{}]", inLabel);
                crop_cmd += fmt::format("[{}] crop={}:{}:{}:{} [{}];", inLabel, x_crop_size, y_crop_size, x, y, outLabel);
                hstack_cmd += fmt::format("[{}]", outLabel);
            }

            std::string rLabel = fmt::format("R{}", r);
            hstack_cmd += fmt::format(" hstack=inputs={} [{}];", roi->fovCols, rLabel);
            vstack_cmd += fmt::format("[{}]", rLabel);
        }

        split_cmd += ";";
        vstack_cmd += " vstack";

        return split_cmd + crop_cmd + hstack_cmd + vstack_cmd;
    }
}
