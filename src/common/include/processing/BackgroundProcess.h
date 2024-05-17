
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
    void roiAvg(RoiCfg* roi, uint16_t* data, size_t width, uint32_t* out) noexcept {
        *out = 0;

        for (size_t j = 0; j < XW; j++) {
            for (size_t i = 0; i < YW; i++) {
                *out += data[i + j * width];
            }
        }
        *out = *out / (XW * YW);
    }

    void roiAvgGeneric(RoiCfg* roi, uint16_t* data, size_t width, uint32_t* out) noexcept {
        *out = 0;

        for (size_t j = 0; j < roi->height; j++) {
            for (size_t i = 0; i < roi->width; i++) {
                *out += data[i + j * width];
            }
        }
        *out = *out / (roi->width * roi->height);
    }
}

#endif //BACKGROUND_PROCESSING_H
