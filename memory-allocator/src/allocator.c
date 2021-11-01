#include <time.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>

#include "../include/allocator.h"

#ifdef DEBUG
    #include <stdio.h>

    #define LOG(str) do{ \
        printf("\n\e[0;92m%s - (%s) Line %d -> %s\e[0m\n", __TIME__, __func__, __LINE__, str); \
        fflush(stdout); \
    }while(0)
#else
    #define LOG(str) 
#endif

#define WORD_SIZE sizeof(void*)
#define META_HEADER_SIZE sizeof(header_t)

#define NO_FREE_BLOCK (header_t*)-1

#define FLG_IS_FREE (0x1 << 0)

#define ALIGN_SIZE(size) ((size + (WORD_SIZE - 1)) & -WORD_SIZE)
#define RETRIVE_HEADER(block) ((header_t*)block)-1
#define RETRIVE_START_REGION(header) (header->end_region - (header->size+META_HEADER_SIZE))
#define IS_FREE_BLOCK(header) (header->flags & FLG_IS_FREE)

typedef struct _block_header{
    size_t size;
    void* end_region;
    uint8_t flags;
    struct _block_header* next;
    struct _block_header* prev;
}header_t;

static header_t* head = NULL;
static header_t* tail = NULL;

#ifdef DEBUG
    void print_memory_blocks(){
        int i = 0;
        header_t* current = head;
        putchar('\n');
        puts("Current blocks in list:");
        if(current != NULL){
            while(current != NULL){
                printf(
                    "-> (Block %d - %lu bytes - free[%d]) ",
                    ++i, current->size, IS_FREE_BLOCK(current)
                );
                current = current->next;
            }
        }else{
            puts("None");
        }
        
    }
#endif

static header_t* split_block(header_t* block, size_t size){
    
    header_t* new_block;
    size_t tmp = block->size - size; 

    block->size -= tmp;

    new_block = block+block->size;
    new_block->end_region = block->end_region;
    new_block->size = tmp;
    new_block->flags = block->flags;
    new_block->next = block->next;
    new_block->prev = block;

    block->end_region = new_block;
    block->next = new_block;

    LOG("SPLIT BLOCKS...");

    return block;
}

static void scribble_memory_block(void* block, size_t size){
    unsigned char* ptr = block;
    while (size-- > 0) *(ptr++) = (clock() % 0xFF);
}

static void merge_blocks(header_t* block){
    block->end_region = block->next->end_region;
    block->size += block->next->size;
    block->next = block->next->next;
}

static void merge_all_free_blocks(){

    header_t* current = head;

    if(head == tail) return;

    while (current != NULL){
        if(current->next != NULL){
            if(IS_FREE_BLOCK(current) && IS_FREE_BLOCK(current->next)){
                merge_blocks(current);
                LOG("MERGE BLOCKS...");
            }
        }
        current = current->next;
    }
}

static header_t* check_for_free_block(size_t size){
    header_t* current = head;

    if(!head) return NO_FREE_BLOCK;

    while (current != NULL){
        if(current->size >= size && IS_FREE_BLOCK(current)) break;
        current = current->next;
    }

    if(current != NULL){
        if(current->size > size){
            current = split_block(current, size);
            current->flags &= ~FLG_IS_FREE; 
        }
        return current;
    }
    
    return NO_FREE_BLOCK;
}

void* _malloc(size_t size){  

    header_t* block = NULL;

    if(size <= 0){
       return NULL; 
    }

    size = ALIGN_SIZE(size);
    block = check_for_free_block(size);

    if(block != NO_FREE_BLOCK){
        LOG("Reuse block of memory...");
        scribble_memory_block(block+1, block->size); // scribble memory block when reuse it
        return block+1; 
    } 

    LOG("Allocate block of memory...");
    block = sbrk(size + META_HEADER_SIZE);

    if(block == (void*)-1) return NULL;

    block->end_region = sbrk(0);
    block->flags = 0;
    block->next = NULL;
    block->size = size;

    if(!head){
        head = tail = block;
        head->prev = tail->prev = NULL;
    }else{
        tail->next = block;
        block->prev = tail;
        tail = block;
    }

    return block+1;
}

void* _calloc(size_t n, size_t size){
    void *ptr = _malloc(n * size);
    if(ptr != NULL) memset(ptr, 0x0, n*size);
    LOG("CALLOC...");
    return ptr;
}

void* _realloc(void* ptr, size_t new_size){
    void* new_ptr = _malloc(new_size);
    header_t* header;

    if(new_ptr != NULL){
        header = RETRIVE_HEADER(ptr);
        memcpy(new_ptr, ptr, header->size);
        _free(ptr);
        LOG("REALLOC...");
    }

    return new_ptr;
}

void _free(void* block){

    header_t* header;
    void* program_break;

    if(block == NULL) return;

    header = RETRIVE_HEADER(block);
    program_break = sbrk(0);

    if(program_break == (void*)-1) return;

    header->flags = FLG_IS_FREE;

    merge_all_free_blocks();

    if(header == tail){

        if(tail == head) {
            head = NULL;
            tail = NULL;
        }else{
            header->prev->next = NULL;
            tail = header->prev; 
        }

        LOG("FREE...");
        int result = brk(header->end_region - (header->size+META_HEADER_SIZE));
        if(result == -1){
            LOG("Error");
        }
        return;
    }

    if(IS_FREE_BLOCK(tail)){
        LOG("FREE...");
        tail = tail->prev;
        tail->next = NULL;
        int result = brk(tail->end_region - (tail->size+META_HEADER_SIZE));
        if(result == -1){
            LOG("ERROR...");
        }
    }
}