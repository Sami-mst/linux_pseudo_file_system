#include "inode.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <unistd.h>
inode init_inode() {
    inode inod=malloc(INODE_SIZE);
    if (inod==NULL){
        return NULL;
    }
    memset(inod, 0, INODE_SIZE);
    inod->size = 0;
    return inod;
}

int find_free_inode(int fd) {
    void *buffer=malloc(INODE_BITMAP_SIZE_IN_BYTES);
    read_block(fd,INODE_TABLE_START,buffer);
    unsigned char *bitmap = (unsigned char *)buffer;
    // Iterate through the bitmap to find a free inode (bit = 0)
    for (size_t i = 0; i < INODE_BITMAP_SIZE_IN_BYTES; i++) {
        unsigned char byte = bitmap[i];
        
        for (int bit = 0; bit < 8; bit++) {
            if ((byte & (1 << bit)) == 0) {  // Found a free inode (bit is 0)
                // Set the inode as allocated by setting the bit to 1
                bitmap[i] |= (1 << bit);  // Mark the inode as allocated

                // Return the inode number (the index of the bit)
                return i * 8 + bit +INODE_TABLE_START;
            }
        }
    }
    
    return -1;  // No free inode found
}