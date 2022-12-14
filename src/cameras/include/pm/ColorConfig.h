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
 * @file  ColorConfig.h
 * 
 * @brief Definition of the ColorConfig class.
 *********************************************************************/
#ifndef PM_COLOR_CONFIG_H
#define PM_COLOR_CONFIG_H
#include <pvcam/pvcam_helper_color.h>

#include <interfaces/ColorConfigInterface.h>

namespace pm {
   /*
    * @breif Color Config class.
    *
    * Implements ColorConfig concept.
    *
    * @tparam ColorCtx type param.
    */
    template<typename ColorCtx>
    class ColorConfig {
        private:
            ColorCtx* m_colorCtx{nullptr};
        public:
            /*
             * @breif Color Config constructor.
             *
             * Constructs color config class.
             *
             * @param colorCtx Pointer to color context.
             */
            ColorConfig(ColorCtx* colorCtx) : m_colorCtx(colorCtx) { }

            /*
             * @breif Color Config destructor.
             */
            ~ColorConfig() = default;


            /*
             * @breif Debayer method.
             *
             * TODO implement when needed.
             */
            void Debayer() {
                return;
            }
    };
};

#endif //PM_COLOR_CONFIG_H
