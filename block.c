#include "block.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
block init_block() {
    block blk=malloc(BLOCK_SIZE);
    if(blk==NULL){
        return NULL;
    }
    memset(blk->data, 0, BLOCK_SIZE);
    return blk;
}


int find_free_block(int fd) {
    void *buffer=malloc(BLOCK_SIZE);
    read_block(fd,DATA_BLOCK_START,buffer);
    //print_block_hex(buffer);
    unsigned char *bitmap = (unsigned char *)buffer;
    // Iterate through the bitmap to find a free inode (bit = 0)
    for (size_t i = 1; i < BLOCK_BITMAP_SIZE_IN_BYTES; i++) { 
        unsigned char byte = bitmap[i];
        
        for (int bit = 0; bit < 8; bit++) {
            if ((byte & (1 << bit)) == 0) {  // Found a free block (bit is 0)
                // Set the block as allocated by setting the bit to 1
                bitmap[i] |= (1 << bit);  // Mark the inode as allocated
                write_to_partition(fd,INODE_BITMAP_BLOCK,(void*)bitmap,BLOCK_SIZE);
                // Return the inode number (the index of the bit)
                
                return i * 8 + bit;
            }
        }
    }
    
    return -1;  // No free inode found
}