#include "supprimer.h"
void supprimer_inode(int fd, int inode_num) {
    // Lire l'inode à supprimer
    int inode_block = (inode_num / (BLOCK_SIZE / INODE_SIZE)) + INODE_TABLE_START;
    int inode_offset = (inode_num % (BLOCK_SIZE / INODE_SIZE)) * INODE_SIZE;
    void *inode_buffer = malloc(BLOCK_SIZE);
    read_block(fd, inode_block, inode_buffer);
    
    inode target_inode = (inode)(inode_buffer + inode_offset);
    
    // Libérer les blocs de données
    void *block_bitmap_buffer = malloc(BLOCK_SIZE);
    read_block(fd, BLOCK_BITMAP_BLOCK, block_bitmap_buffer);
    unsigned char *block_bitmap = (unsigned char *)block_bitmap_buffer;
    
    for (int i = 0; i < 8; i++) {
        if (target_inode->block_pointers[i] != 0) {
            int block_num = target_inode->block_pointers[i];
            int byte_index = block_num / 8;
            int bit_index = block_num % 8;
            block_bitmap[byte_index] &= ~(1 << bit_index); // Libérer le bloc
        }
    }
    write_to_partition(fd, BLOCK_BITMAP_BLOCK, block_bitmap_buffer, BLOCK_SIZE);
    
    // Libérer l'inode dans le bitmap
    void *inode_bitmap_buffer = malloc(BLOCK_SIZE);
    read_block(fd, INODE_BITMAP_BLOCK, inode_bitmap_buffer);
    unsigned char *inode_bitmap = (unsigned char *)inode_bitmap_buffer;
    
    int byte_index = inode_num / 8;
    int bit_index = inode_num % 8;
    inode_bitmap[byte_index] &= ~(1 << bit_index); // Marquer l'inode comme libre
    write_to_partition(fd, INODE_BITMAP_BLOCK, inode_bitmap_buffer, BLOCK_SIZE);
    
    // Effacer l'inode dans la table des inodes
    memset(target_inode, 0, INODE_SIZE);
    memcpy(inode_buffer + inode_offset, target_inode, INODE_SIZE);
    write_to_partition(fd, inode_block, inode_buffer, BLOCK_SIZE);
    
    free(inode_buffer);
    free(block_bitmap_buffer);
    free(inode_bitmap_buffer);
    
    printf("Inode %d supprimé.\n", inode_num);
}