
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
 * @file  WriteRawFrame.h
 *********************************************************************/
#ifndef BACKGROUND_PROCESSING_H
#define BACKGROUND_PROCESSING_H

#include <pm/Camera.h>
#include <interfaces/FrameInterface.h>
#include <Rois.h>

namespace processing {
    template<size_t XW, size_t YW>
    double roiAvg(Rois::RoiCfg* roi, uint16_t* data, size_t x, size_t y, size_t width) noexcept {
        double out = 0.0;

        uint16_t* dataStart = data + Rois::roiToOffset(x, y, width);

        for (size_t j = 0; j < YW; j++) {
            for (size_t i = 0; i < XW; i++) {
                out += dataStart[i + j * width];
            }
        }
        return out / double((XW * YW));
    }

    double roiAvgGeneric(Rois::RoiCfg* roi, uint16_t* data, size_t x, size_t y, size_t width) noexcept {
        double out = 0.0;

        uint16_t* dataStart = data + Rois::roiToOffset(x, y, width);

        for (size_t j = 0; j < roi->height / roi->scale; j++) {
            for (size_t i = 0; i < roi->width / roi->scale; i++) {
                out += dataStart[i + j * width];
            }
        }
        return out / double(((roi->width / roi->scale) * (roi->height / roi->scale)));
    }
}

#endif //BACKGROUND_PROCESSING_H
