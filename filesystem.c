#include "filesystem.h"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h> 
#include <string.h>
#include <errno.h>

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

int read_block(superblock sb, int fd, uint32_t block_number, void *buffer) {
    if (block_number >= sb->total_blocks) return -1;
    
    lseek(fd, block_number * BLOCK_SIZE, SEEK_SET);
    read(fd,buffer, BLOCK_SIZE );
    
    return 0;
}

void print_block_hex(void *sb) {
    uint8_t *buffer=(uint8_t *) sb;
    for (int i = 0; i < BLOCK_SIZE; i++) {
        printf("%02X ", buffer[i]);
        if ((i + 1) % 16 == 0) printf("\n"); // New line every 16 bytes
    }
    printf("\n");
}

void get_super_block(int fd,void *buffer){
   if (lseek(fd, 0, SEEK_SET) == (off_t)-1) {
        perror("lseek failed");
        return;
    }
    int s=read(fd,buffer, BLOCK_SIZE);
    printf("%d\n",s);
    if(s!=BLOCK_SIZE){
        perror("read failed");
    }
}

