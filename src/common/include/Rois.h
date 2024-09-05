
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

#include <string>
#include <vector>
#include <tuple>

namespace Rois {
    struct RoiCfg {
        double well_spacing;
        double xy_pixel_size;
        double scale;

        size_t rows;
        size_t cols;

        double width;
        double height;

        double h_offset;
        double v_offset;
    };

    std::vector<std::tuple<uint32_t, uint32_t>> roiOffsets(RoiCfg* roi, size_t frameWidth, size_t frameHeight);
    std::string wellName(uint32_t row, uint32_t col);
    uint32_t roiToOffset(uint32_t x, uint32_t y, uint32_t width);
}

#endif //_ROIS_H
