#include <Rois.h>

namespace Rois {
    std::vector<std::tuple<size_t, size_t>> roiOffsets2(RoiCfg* roi, size_t frameWidth, size_t frameHeight) {
        std::vector<std::tuple<size_t, size_t>> roi_offsets = {};

        uint32_t roiX = roi->width; // / roi->scale;
        uint32_t roiY = roi->height;// / roi->scale;
        int32_t well_width_px = (roi->well_spacing / roi->scale);

        int32_t top_x = frameWidth / 2 - 0.5*(roi->cols - 1) * well_width_px;
        int32_t top_y = frameHeight / 2 - 0.5*(roi->rows - 1) * well_width_px;

        for (size_t r = 0; r < roi->rows; r++) {
            int32_t y = top_y + r * well_width_px - roiY/2;

            for (size_t c = 0; c < roi->cols; c++) {
                int32_t x = top_x + c * well_width_px - roiX/2;
                roi_offsets.push_back(std::make_tuple(x, y));
            }
        }

        return roi_offsets;
    }

    std::vector<std::tuple<uint32_t, uint32_t>> roiOffsets(RoiCfg* roi, size_t frameWidth, size_t frameHeight) {
        std::vector<std::tuple<uint32_t, uint32_t>> roi_offsets = {};

        double mid_x = frameWidth / 2.0;
        double mid_y = frameHeight / 2.0;
        double well_spacing_scaled = roi->well_spacing / (roi->xy_pixel_size * roi->scale);

        double top_x = mid_x - 0.5 * (roi->cols - 1) * well_spacing_scaled - (roi->width / (2 * roi->scale)); // - well_spacing_scaled / 2;
        double top_y = mid_y - 0.5 * (roi->rows - 1) * well_spacing_scaled - (roi->height / (2 * roi->scale)); // - well_spacing_scaled / 2;

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
}
