#include "filesystem.h"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h> 
#include <string.h>

unsigned char inode_bitmap[INODE_BITMAP_SIZE_IN_BYTES];
unsigned char block_bitmap[BLOCK_BITMAP_SIZE_IN_BYTES];
superblock init_superblock() {
    superblock sb=malloc(BLOCK_SIZE);
    if (sb==NULL){
        return NULL;
    }
    sb->block_size = BLOCK_SIZE;
    sb->total_blocks = TOTAL_BLOCKS;
    sb->total_inodes = TOTAL_INODES;
    sb->free_blocks = TOTAL_BLOCKS - 10; 
    sb->free_inodes = TOTAL_INODES - 1; 
    sb->inode_bitmap_start = INODE_BITMAP_BLOCK;
    sb->block_bitmap_start = BLOCK_BITMAP_BLOCK;
    sb->inode_table_start = INODE_TABLE_START;
    sb->data_block_start = DATA_BLOCK_START;
    sb->root_inode = 0; 

    return sb;
}

