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
 * @file  FileInterface.h
 * 
 * @brief Definition of the FileInterface concept.
 *********************************************************************/
#ifndef FILE_INTERFACE_H
#define FILE_INTERFACE_H
#include <concepts>

#include "FrameInterface.h"

/*
* Defines File concept for any class that needs to fulfill the
* file interface.
*
* @tparam T Class type.
* @param F FrameConcept type.
*/
template<typename T, typename F>
concept FileConcept = FrameConcept<F> and requires(T c, T const cc, F* pframe) {
    { c.Open(std::string()) } -> std::same_as<bool>;
    { c.Close() } -> std::same_as<void>;
    { c.WriteFrame(pframe) } -> std::same_as<bool>;
    { cc.IsOpen() } -> std::same_as<bool>;
    { cc.Name() } -> std::same_as<const std::string&>;
};

#endif //FILE_INTERFACE_H
