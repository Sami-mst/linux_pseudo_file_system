#include "filesystem.h"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h> 
#include <string.h>
#include "inode.h"
#include "block.h"

int read_block(superblock sb,FILE *disk, uint32_t block_number, uint8_t *buffer) {

    if (block_number >= sb->total_blocks) return -1;
    
    fseek(disk, block_number * BLOCK_SIZE, SEEK_SET);
    fread(buffer, 1, BLOCK_SIZE, disk);
    
    return 0;
}
void write_to_partition(int fd, int block, void *data, size_t data_size) {


    // Step 3: Write the modified buffer back to the partition, preserving the null bytes
    ssize_t bytesWritten = pwrite(fd, data, BLOCK_SIZE, block);
    if (bytesWritten == -1) {
        perror("Failed to write to partition");
        close(fd);
    } else {
        printf("Successfully wrote %zd bytes to partition\n", bytesWritten);
    }
}


void print_block_hex(const uint8_t *buffer) {
    for (int i = 0; i < BLOCK_SIZE; i++) {
        printf("%02X ", buffer[i]);
        if ((i + 1) % 16 == 0) printf("\n"); // New line every 16 bytes
    }
    printf("\n");
}

int creer_inode(char *nom,int type){
    int fd = open(PARTITION_FILE, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    inode root_inode = init_inode();  // creer le inode pour root 
    root_inode->file_type=type;
    root_inode->mode=777;
    root_inode->uid=0;
    root_inode->gid=0;
    root_inode->size=0;
    root_inode->single_indirect=0;
    memset(root_inode->block_pointers,0,sizeof(root_inode->block_pointers));

    if (lseek(fd, INODE_TABLE_START * BLOCK_SIZE, SEEK_SET) == -1) {
        perror("Failed to seek to inode table");
        close(fd);
        exit(1);
    }
    if (write(fd, root_inode, INODE_SIZE) != INODE_SIZE) {
        perror("Failed to write root inode");
        close(fd);
        exit(1);
    }

    // marquer le premier inode comme utilisé
    inode_bitmap[0] = 0x01;  // 

    // Update the inode bitmap with the root inode marked
    if (lseek(fd, INODE_BITMAP_BLOCK * BLOCK_SIZE, SEEK_SET) == -1) {
        perror("Failed to seek to inode bitmap");
        close(fd);
        exit(1);
    }
    if (write(fd, inode_bitmap, INODE_BITMAP_SIZE_IN_BYTES) != INODE_BITMAP_SIZE_IN_BYTES) {
        perror("Failed to write inode bitmap");
        close(fd);
        exit(1);
    }

    close(fd);
}