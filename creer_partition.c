#include "filesystem.h"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h> 
#include <string.h>
#include "inode.h"
#include "block.h"

void creer_filesystem(){
    superblock sb=init_superblock();
    memset(inode_bitmap,0,INODE_BITMAP_SIZE_IN_BYTES);
    memset(block_bitmap,0,BLOCK_BITMAP_SIZE_IN_BYTES);
     // Open filesystem file
    
    int fd = open(PARTITION_FILE, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd == -1) {
        perror("Failed to open partition file");
        exit(1);
    }

    // ecrire le super block
    if (write(fd, &sb, sizeof(sb)) != sizeof(sb)) {
        perror("Failed to write superblock");
        close(fd);
        exit(1);
    }

    // Padding
    char padding[BLOCK_SIZE] = {0};
    write(fd, padding, BLOCK_SIZE - sizeof(sb));

    // ecrire le inode bitmap 
    if (write(fd, inode_bitmap, INODE_BITMAP_SIZE_IN_BYTES) != INODE_BITMAP_SIZE_IN_BYTES) {
        perror("Failed to write inode bitmap");
        close(fd);
        exit(1);
    }

    write(fd, padding, BLOCK_SIZE - INODE_BITMAP_BLOCK);
    // ecrire data block bitmap
    if (write(fd, block_bitmap, BLOCK_BITMAP_SIZE_IN_BYTES) != BLOCK_BITMAP_SIZE_IN_BYTES) {
        perror("Failed to write block bitmap");
        close(fd);
        exit(1);
    }

    write(fd, padding, BLOCK_SIZE - (BLOCK_BITMAP_BLOCK%BLOCK_SIZE));

    inode root_inode = init_inode();  // creer le inode pour root 
    root_inode->file_type=FILE_TYPE_DIRECTORY;
    root_inode->mode=777;
    root_inode->uid=0;
    root_inode->gid=0;
    root_inode->size=0;
    root_inode->double_indirect=0;
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
    printf("Filesystem formatted successfully.\n");
}

