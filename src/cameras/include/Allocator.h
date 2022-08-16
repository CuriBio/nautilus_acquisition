#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#ifdef _WIN32
#include <malloc.h> // _aligned_malloc
#else
#include <stdlib.h> // aligned_alloc
#endif

template<int ALIGN>
class Allocator {
    public:
        const uint16_t alignment{ALIGN};
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
