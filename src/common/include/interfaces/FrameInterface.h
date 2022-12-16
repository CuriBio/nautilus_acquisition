/*
 * MIT License
 *
 * Copyright (c) 2022 Curi Bio
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
 * @file  FrameInterface.h
 * 
 * @brief Definition of the FrameInterface concept.
 *********************************************************************/
#ifndef FRAME_INTERFACE_H
#define FRAME_INTERFACE_H
#include <concepts>

struct FrameInfo {
    FrameInfo& operator=(const FrameInfo& other) {
        if (&other != this)
        {
            frameNr = other.frameNr;
            timestampBOF = other.timestampBOF;
            timestampEOF = other.timestampEOF;
            readoutTime = other.readoutTime;
            expTime = other.expTime;
            colorWbScaleRed = other.colorWbScaleRed;
            colorWbScaleGreen = other.colorWbScaleGreen;
            colorWbScaleBlue = other.colorWbScaleBlue;
        }
        return *this;
    }

    uint32_t frameNr{ 0 };
    uint64_t timestampBOF{ 0 };
    uint64_t timestampEOF{ 0 };
    uint32_t readoutTime{ 0 };

    uint32_t expTime{ 0 };
    float colorWbScaleRed{ 1.0 };
    float colorWbScaleGreen{ 1.0 };
    float colorWbScaleBlue{ 1.0 };
};

template<typename T>
concept FrameConcept = requires(T c, const T& f, T const cc, void* vptr, const FrameInfo& pFrameInfo) {
    { c.SetData(vptr) } -> std::same_as<void>;
    { c.GetData() } -> std::same_as<void*>;
    { c.CopyData() } -> std::same_as<bool>;
    { cc.GetInfo() } -> std::same_as<FrameInfo*>;
    { c.SetInfo(pFrameInfo) } -> std::same_as<void>;
    { c.Copy(f, bool()) } -> std::same_as<bool>;
};

#endif //FRAME_INTERFACE_H
