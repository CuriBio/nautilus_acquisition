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
 * @file  Allocator.h
 * 
 * @brief Definition of the Allocator class.
 *********************************************************************/
#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#ifdef _WIN32
#include <Windows.h>
#include <malloc.h> // _aligned_malloc
#else
#include <stdlib.h> // aligned_alloc
#include <unistd.h>
#endif

const static auto cPageBytes = []() -> size_t {
#ifdef _WIN32
    SYSTEM_INFO sysInfo;
    ::GetSystemInfo(&sysInfo);
    return sysInfo.dwPageSize;
#else
    return ::sysconf(_SC_PAGESIZE);
#endif
}();

template<size_t ALIGN>
class Allocator {
    public:
        const size_t alignment{ALIGN};
    public:
        Allocator() {};
        ~Allocator() {};

        void* Allocate(size_t size) {
#ifdef _WIN32
            return ::_aligned_malloc(size, alignment);
#else
            const size_t sizeAligned = (size + (alignment - 1)) & ~(alignment - 1);
            return ::aligned_alloc(alignment, sizeAligned);
#endif
        };

        void Free(void* ptr) {
#ifdef _WIN32
            ::_aligned_free(ptr);
#else
            ::free(ptr);
#endif
        };
};

#endif //ALLOCATOR_H
