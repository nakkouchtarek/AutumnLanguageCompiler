#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <string>

typedef struct Block
{
    void* start;
    void* end;
    bool allocated;
} Block;

typedef const char* String;
static char pool[25000] = {0};

extern size_t heap_size;
extern Block blocks[255];
extern int block_count;

void* allocate(size_t size);
void show_block(int i);
void show_memory();
void show_memory_blocks();
void defragment();
void free(void* t);
void garbage_collector();
