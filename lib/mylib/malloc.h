#pragma once
#include <stddef.h>

void *sbrk(int increment);

int init_mem();
void *malloc(size_t num);
void *calloc(size_t num, size_t size);
void free(void *ptr);

