#include <FFmpegFilter.h>
#include <fmt/format.h>

namespace FFmpegFilter {
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
    std::string getCropFilter(FFmpegFilterCfg* cfg) {
        double mid_x = cfg->frameWidth / 2.0;
        double mid_y = cfg->frameHeight / 2.0;
        double well_spacing_scaled = cfg->well_spacing / (cfg->xy_pixel_size * cfg->scale);
        double h_offset_scaled = cfg->h_offset / cfg->scale;
        double v_offset_scaled = cfg->v_offset / cfg->scale;

        uint32_t x_offset = std::round(mid_x - 0.5 * cfg->num_wells_h * well_spacing_scaled + h_offset_scaled);
        uint32_t y_offset = std::round(mid_y - 0.5 * cfg->num_wells_v * well_spacing_scaled + v_offset_scaled);

        uint32_t x_crop_size = std::round(cfg->num_wells_h * well_spacing_scaled);
        uint32_t y_crop_size = std::round(cfg->num_wells_v * well_spacing_scaled);

        std::string split_cmd = fmt::format("split={}", cfg->rows * cfg->cols);
        std::string crop_cmd = "";
        std::string hstack_cmd = "";
        std::string vstack_cmd = "";

        for (uint32_t r = 0; r < cfg->rows; r++) {
            uint32_t y = y_offset + (r * cfg->frameHeight);

            for (uint32_t c = 0; c < cfg->cols; c++) {
                uint32_t x = x_offset + (c * cfg->frameWidth);

                std::string inLabel = fmt::format("L_{}_{}", r, c);
                std::string outLabel = inLabel + "_c";  // for "cropped"

                split_cmd += fmt::format("[{}]", inLabel);
                crop_cmd += fmt::format("[{}] crop={}:{}:{}:{} [{}];", inLabel, x_crop_size, y_crop_size, x, y, outLabel);
                hstack_cmd += fmt::format("[{}]", outLabel);
            }

            std::string rLabel = fmt::format("R{}", r);
            hstack_cmd += fmt::format(" hstack=inputs={} [{}];", cfg->cols, rLabel);
            vstack_cmd += fmt::format("[{}]", rLabel);
        }

        split_cmd += ";";
        vstack_cmd += " vstack";

        return split_cmd + crop_cmd + hstack_cmd + vstack_cmd;
    }
}
