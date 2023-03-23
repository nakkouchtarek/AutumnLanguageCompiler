#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <allocator.h>


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

    blocks[block_count] = b;
    block_count += 1;
    
    return block;
}

// show memory blocks

void show_block(int i)
{
    printf("-> Start: %p || End: %p || Allocated memory: %lld\n", blocks[i].start, blocks[i].end, (char*)(blocks[i].end) - (char*)(blocks[i].start));

    int* ptr = (int*)blocks[i].start;
        
    for(int j=0; j < (int)((char*)(blocks[i].end) - (char*)(blocks[i].start)) ; j = j + sizeof(int))
    {
        printf("Address %p : Content %d\n", ptr, *ptr);
        ptr = ptr + 'a';
    } 
}

void show_memory()
{
    for(int j=0; j<block_count; j++)
    {
        show_block(j);
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
            memset(blocks[i].start, 0x00, (size_t)((char*)(blocks[i].end) - (char*)(blocks[i].start)));
        }
    } 

    block_count -= 1;
}

void garbage_collector()
{
    for(int i=0; i < block_count ; i++)
    {
        blocks[i].allocated = false;
        memset(blocks[i].start, 0x00, (size_t)((char*)(blocks[i].end) - (char*)(blocks[i].start)));
    }

    block_count = 0;
}

int main()
{
    int* t = (int*)allocate(sizeof(int));
    int* t2 = (int*)allocate(sizeof(int));
    char** test = (char**)allocate(sizeof(char)*50);

    *t = 123;
    *t2 = 401;
    *test = "test hello 123";

    printf("%s\n", *test);

    show_memory();

    garbage_collector();
    
    return 0;
}