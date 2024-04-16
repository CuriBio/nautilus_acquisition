
/*
 * MIT License
 *
 * Copyright (c) 2024 Curi Bio
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/*********************************************************************
 * @file  Rois.h
 *********************************************************************/
#ifndef _ROIS_H
#define _ROIS_H

struct RoiCfg {
    double well_spacing;
    double xy_pixel_size;
    double scale;

    size_t rows;
    size_t cols;

    size_t width;
    size_t height;
};

std::vector<size_t> roiOffsets(RoiCfg* roi, size_t frameWidth, size_t frameHeight) {
    std::vector<size_t> roi_offsets = {};
    size_t roiX = roi->width / roi->scale;
    size_t roiY = roi->height / roi->scale;

    int32_t well_width_px = (roi->well_spacing / (roi->xy_pixel_size * roi->scale));
    int32_t top_x = frameWidth / 2 - 0.5*(roi->cols - 1) * well_width_px;
    int32_t top_y = frameHeight / 2 - 0.5*(roi->rows - 1) * well_width_px;

    for (size_t r = 0; r < roi->rows; r++) {
        int32_t y = top_y + r * well_width_px - roiY/2;
        for (size_t c = 0; c < roi->cols; c++) {
            int32_t x = top_x + c * well_width_px - roiX/2;
            roi_offsets.push_back(x + (y * frameWidth));
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

#endif //_ROIS_H
