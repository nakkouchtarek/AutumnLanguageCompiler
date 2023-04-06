#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include <string>
#include <iostream>

#include <allocator.hpp>

size_t heap_size = 0;
Block blocks[25000] = {{0}};
int block_count = 0;

// allocation

void* allocate(size_t size)
{
    for(int j=0; j<block_count; j++)
    {
        if( !(blocks[j].allocated) && (char*)(blocks[j].end) - (char*)(blocks[j].start) >= size )
        {
            blocks[j].allocated = true;
            return (void*)(blocks[j].start);
        } 
    }

    void* block = pool + heap_size;
    heap_size += size;

    Block b = {
        .start = block,
        .end = pool + heap_size,
        .allocated = true,
    }; 

    memset(b.start, 0, (size_t)((char*)(b.end) - (char*)(b.start)));

    blocks[block_count] = b;
    block_count += 1;
    
    return block;
}

// show memory blocks

void show_block(int i)
{
    for(size_t j=0; j < (size_t)((char*)(blocks[i].end) - (char*)(blocks[i].start)) ; j++)
    {
        std::cout << (int*)((char*)blocks[i].start + j) << "->" << *((char*)blocks[i].start + j) << std::endl;
    } 
}

void show_memory()
{
    for(int j=0; j<block_count; j++)
    {
        show_block(j);
    }
}

void show_memory_blocks()
{
    for(int i=0; i<block_count; i++)
    {
        printf("-> Start: %p || End: %p || Allocated: %d || Allocated memory: %lld\n", blocks[i].start, blocks[i].end, blocks[i].allocated ,(char*)(blocks[i].end) - (char*)(blocks[i].start));
    }
}


// free memory


void free(void* t)
{
    for(int i=0; i < block_count ; i++)
    {
        if(blocks[i].start == t)
        {
            blocks[i].allocated = false;
            memset(blocks[i].start, 0, size_t((char*)(blocks[i].end) - (char*)(blocks[i].start)));
        }
    }

    defragment();
}

void garbage_collector()
{
    for(int i=0; i < block_count ; i++)
    {
        memset(blocks[i].start, 0, (size_t)((char*)(blocks[i].end) - (char*)(blocks[i].start)));
    }

    memset(blocks, 0, sizeof(blocks));
    block_count = 0;
}

// defragment

void defragment()
{
    size_t diff;
    Block tmp;

    for(int i=0; i<block_count; i++)
    {
        if(blocks[i].allocated == 0)
        {
            diff = (size_t)((char*)(blocks[i].end) - (char*)(blocks[i].start));
            
            for(int j=i+1; j<block_count; j++)
            {
                
                char* adr = (char*)(blocks[j].start);

                for(size_t k=0; k < (size_t)((char*)(blocks[j].end) - (char*)(blocks[j].start)) ; k++)
                {
                    *(char*)(adr-(size_t)((char*)(blocks[i].end) - (char*)(blocks[i].start))) = *adr;
                    adr++;
                } 

                tmp = blocks[j];
                blocks[j] = blocks[j+1];
                blocks[j] = tmp;   
            }
        }
    }
    
    memset(blocks[block_count-1].start, 0, (size_t)((char*)(blocks[block_count-1].end) - (char*)(blocks[block_count-1].start)));
    blocks[block_count-1] = {0};
    block_count -= 1;
    heap_size -= diff;

    for(int i=0; i<block_count; i++)
    {
        blocks[i].allocated = true;
    }
}