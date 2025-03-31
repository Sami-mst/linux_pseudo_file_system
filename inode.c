#include "inode.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "creer_dossier.h"
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
    void *buffer=malloc(BLOCK_SIZE);
    read_block(fd,INODE_BITMAP_BLOCK,buffer);
    //print_block_hex(buffer);
    unsigned char *bitmap = (unsigned char *)buffer;
    // Iterate through the bitmap to find a free inode (bit = 0)
    for (size_t i = 1; i < INODE_BITMAP_SIZE_IN_BYTES; i++) {
        unsigned char byte = bitmap[i];
        
        for (int bit = 0; bit < 8; bit++) {
            if ((byte & (1 << bit)) == 0) {  // Found a free inode (bit is 0)
                // Set the inode as allocated by setting the bit to 1
                bitmap[i] |= (1 << bit);  // Mark the inode as allocated
                write_to_partition(fd,INODE_BITMAP_BLOCK,(void*)bitmap,BLOCK_SIZE);
                // Return the inode number (the index of the bit)
                
                return i * 8 + bit;
            }
        }
    }
    
    return -1;  // No free inode found
}

int valid_inode(int fd, int inode_num) {
    if (inode_num < 0 || inode_num >= TOTAL_INODES) {
        return 0;  // Inode invalide
    }

    void *buffer = malloc(BLOCK_SIZE);
    if (!buffer) {
        perror("Memory allocation failed");
        return 0;
    }

    if (read_block(fd, INODE_BITMAP_BLOCK, buffer) < 0) {
        free(buffer); // Libérer avant de retourner en cas d'erreur de lecture
        return 0;
    }

    unsigned char *bitmap = (unsigned char *)buffer;
    int byte_index = inode_num / 8;
    int bit_index = inode_num % 8;

    int is_reserved = (bitmap[byte_index] & (1 << bit_index)) ? 1 : 0;

    free(buffer); // Libérer correctement la mémoire après utilisation
    return is_reserved;
}
