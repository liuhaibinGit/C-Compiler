#include "malloc.h"
#include <stdint.h>
#include <stddef.h>

#define HEAP_LIMIT (1 << 24)
#define HEAP_BLOCKS 64

typedef struct Block Block;
struct Block{
    void *addr;
    Block *next;
    size_t size;
};

typedef struct {
    Block *free;   // first free block
    Block *used;   // first used block
    Block *fresh;  // first available blank block
    size_t top;    // top free addr
    Block blocks[HEAP_BLOCKS];
}Heap;

static Heap manager;
static Heap *heap = &manager;
static size_t available = 0;


//返回可用空间的起始地址
void *sbrk(int increment) {
    if (increment < 0)
        return (void *)-1;
    size_t total = __builtin_wasm_current_memory() << 16;
    if (available < increment) {
        if (__builtin_wasm_grow_memory(((increment - available - 1) >> 16) + 1) < 1) {
            return (void *)-1;
        }
        size_t new_total = __builtin_wasm_current_memory() << 16;
        available += new_total - total;
        total = new_total;
    }
    total -= available;
    available -= increment;
    return (void *)total;
}

static void insert_block(Block *block) {
    block->next = heap->free;
    heap->free  = block;
}

int init_mem() {
    heap->free   = NULL;
    heap->used   = NULL;
    heap->fresh  = heap->blocks;
    heap->top    = (size_t)sbrk(0);
    Block *block = heap->blocks;
    size_t i     = HEAP_BLOCKS - 1;
    while (i--) {
        block->next = block + 1;
        block++;
    }
    return 0;
}

void free(void *free) {
    Block *block = heap->used;
    Block *prev  = NULL;
    while (block != NULL) {
        if (free == block->addr) {
            if (prev) {
                prev->next = block->next;
            } else {
                heap->used = block->next;
            }
            insert_block(block);
            return;
        }
        prev  = block;
        block = block->next;
    }
    return;
}

static Block *alloc_block(size_t num) {
    Block *ptr  = heap->free;
    Block *prev = NULL;
    size_t top  = heap->top;
    num         = (num + 7U) & ~7U;
    while (ptr != NULL) {
        if ( ptr->size >= num) {
            if (prev != NULL) {
                prev->next = ptr->next;
            } else {
                heap->free = ptr->next;
            }
            ptr->next  = heap->used;
            heap->used = ptr;
            return ptr;
        }
        prev = ptr;
        ptr  = ptr->next;
    }
	sbrk(num);	
    size_t new_top = top + num;
    if (heap->fresh != NULL && new_top <= HEAP_LIMIT) {
        ptr         = heap->fresh;
        heap->fresh = ptr->next;
        ptr->addr   = (void *)top;
        ptr->next   = heap->used;
        ptr->size   = num;
        heap->used  = ptr;
        heap->top   = new_top;
        return ptr;
    }
    return NULL;
}

void *malloc(size_t num) {
    Block *block = alloc_block(num);
    if (block != NULL) {
        return block->addr;
    }
    return NULL;
}

static void memclear(void *ptr, size_t num) {
    size_t *ptrw = (size_t *)ptr;
    size_t numw  = (num & -sizeof(size_t)) / sizeof(size_t);
    while (numw--) {
        *ptrw++ = 0;
    }
    num &= (sizeof(size_t) - 1);
    uint8_t *ptrb = (uint8_t *)ptrw;
    while (num--) {
        *ptrb++ = 0;
    }
}

void *calloc(size_t num, size_t size) {
    num *= size;
    Block *block = alloc_block(num);
    if (block != NULL) {
        memclear(block->addr, num);
        return block->addr;
    }
    return NULL;
}

static size_t count_blocks(Block *ptr) {
    size_t num = 0;
    while (ptr != NULL) {
        num++;
        ptr = ptr->next;
    }
    return num;
}

size_t num_free() {
    return count_blocks(heap->free);
}

size_t num_used() {
    return count_blocks(heap->used);
}

size_t num_fresh() {
    return count_blocks(heap->fresh);
}

int check() {
    if(HEAP_BLOCKS == num_free() + num_used() + num_fresh()){
		return 0;
	}
	return 1;
}
