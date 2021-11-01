#ifndef _ALLOCATOR_H_
    #define _ALLOCATOR_H_

    #include <stddef.h>

    void* _malloc(size_t size);
    void* _calloc(size_t n, size_t size);
    void* _realloc(void* ptr, size_t new_size);
    void _free(void* block);

    #ifdef DEBUG
        void print_memory_blocks();
    #endif

#endif