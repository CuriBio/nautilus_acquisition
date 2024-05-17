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
#ifndef WRITE_RAW_FRAME_H
#define WRITE_RAW_FRAME_H

#include <pm/Camera.h>
#include <interfaces/FrameInterface.h>

namespace processing {
    template<FrameConcept F>
    void writeRawFrame(FrameCtx* ctx, F* frame) noexcept {
        RawFile<4> raw(ctx->path, ctx->bitDepth, ctx->width, ctx->height);
        raw.Write(frame->GetData(), 0);
        raw.Close();
    }
}

#endif //WRITE_RAW_FRAME_H
