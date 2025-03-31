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
    read_block(fd,BLOCK_BITMAP_BLOCK,buffer);
    //print_block_hex(buffer);
    unsigned char *bitmap = (unsigned char *)buffer;
    // Iterate through the bitmap to find a free inode (bit = 0)
    for (size_t i = 1; i < BLOCK_BITMAP_SIZE_IN_BYTES; i++) { 
        unsigned char byte = bitmap[i];
        
        for (int bit = 0; bit < 8; bit++) {
            if ((byte & (1 << bit)) == 0) {  // Found a free block (bit is 0)
                // Set the block as allocated by setting the bit to 1
                bitmap[i] |= (1 << bit);  // Mark the inode as allocated
                write_to_partition(fd,BLOCK_BITMAP_BLOCK,(void*)bitmap,BLOCK_SIZE);
                // Return the inode number (the index of the bit)
                
                return i * 8 + bit;
            }
        }
    }
    
    return -1;  // No free inode found
}


int valid_block(int fd, int block_num) {
    if (block_num < 0 || block_num >= TOTAL_INODES) {
        return 0;  // Inode invalide
    }

    void *buffer = malloc(BLOCK_SIZE);
    if (!buffer) {
        perror("Memory allocation failed");
        return 0;
    }

    if (read_block(fd, BLOCK_BITMAP_BLOCK, buffer) < 0) {
        free(buffer); // Libérer avant de retourner en cas d'erreur de lecture
        return 0;
    }

    unsigned char *bitmap = (unsigned char *)buffer;
    int byte_index = block_num / 8;
    int bit_index = block_num % 8;

    int is_reserved = (bitmap[byte_index] & (1 << bit_index)) ? 1 : 0;

    free(buffer); // Libérer correctement la mémoire après utilisation
    return is_reserved;
}
