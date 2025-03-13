#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <stdint.h>

#define MAX_FILENAME_LEN 60
#define PARTITION_FILE "partition.img"
#define BLOCK_SIZE 512
#define INODE_SIZE 64

#define FILE_TYPE_FILE  1  
#define FILE_TYPE_DIRECTORY 2  
#define TOTAL_BLOCKS 65536  
#define TOTAL_INODES 656
#define INODE_BITMAP_SIZE_IN_BYTES (TOTAL_INODES / 8)  
#define BLOCK_BITMAP_SIZE_IN_BYTES (TOTAL_BLOCKS / 8)  
#define INODE_BITMAP_BLOCK 1  
#define BLOCK_BITMAP_BLOCK 2  
#define INODE_TABLE_START 3  
#define DATA_BLOCK_START (INODE_TABLE_START + (TOTAL_INODES / (BLOCK_SIZE / INODE_SIZE)))
extern unsigned char inode_bitmap[INODE_BITMAP_SIZE_IN_BYTES];
extern unsigned char block_bitmap[BLOCK_BITMAP_SIZE_IN_BYTES];

typedef uint32_t block_addr;

typedef struct superblock {
    uint32_t block_size;
    uint32_t total_blocks;
    uint32_t total_inodes;
    uint32_t free_blocks;
    uint32_t free_inodes;
    uint32_t inode_bitmap_start;
    uint32_t block_bitmap_start;
    uint32_t inode_table_start;
    uint32_t data_block_start;
    uint32_t root_inode;
    char reserved[128];
} *superblock;

superblock init_superblock();
#endif // FILESYSTEM_H
