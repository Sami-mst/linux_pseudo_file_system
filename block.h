#ifndef BLOCK_H
#define BLOCK_H

#include "filesystem.h"
#include "inode.h"
typedef struct block {
    char data[BLOCK_SIZE];  
}*block;

typedef struct {
    dir_entry entries[BLOCK_SIZE / sizeof(dir_entry)];
} *DirBlock;

typedef struct indirect {
    block_addr block_pointers[BLOCK_SIZE / sizeof(block_addr)];
} *indirect;

block init_block();

#endif // BLOCK_H
