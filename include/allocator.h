#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

typedef struct Block
{
    void* start;
    void* end;
    bool allocated;
} Block;

typedef char* String;
static char pool[25000] = {0x0};
size_t heap_size = 0;
Block blocks[255] = {0};
int block_count = 0;

void* allocate(size_t size);
void show_block(int i);
void show_memory();
void free(void* t);
void garbage_collector();
