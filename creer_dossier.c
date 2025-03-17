#include "filesystem.h"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h> 
#include <stdio.h>
#include <string.h>
#include "inode.h"
#include "block.h"
#include <string.h>
#include "creer_partition.h"

void creer_dossier(int fd,char* nom,int inode_dossier_parent){
    
}

void write_to_partition(int fd, int block, void *data, size_t data_size) {

    ssize_t bytesWritten = pwrite(fd, data, data_size, block*BLOCK_SIZE);
    if (bytesWritten == -1) {
        perror("Failed to write to partition");
        close(fd);
    } else {
        printf("Successfully wrote %zd bytes to partition\n", bytesWritten);
    }
}

int creer_inode(int fd,char *nom,int type){
    inode inode = init_inode();  // creer le inode pour root 
    inode->file_type=type;
    inode->mode=777;
    inode->uid=0;
    inode->gid=0;
    inode->size=0;
    strcpy(inode->name,nom);
    inode->single_indirect=0;
    memset(inode->block_pointers,0,sizeof(inode->block_pointers));
    int inod=find_free_inode(fd);
    int inode_block=(inod / (BLOCK_SIZE / INODE_SIZE))+INODE_TABLE_START;
    int inode_offset=(inod%(BLOCK_SIZE / INODE_SIZE))*INODE_SIZE;
    void *inode_buffer = malloc(BLOCK_SIZE);
    memset(inode_buffer,0,BLOCK_SIZE);
    if (!inode_buffer) {
    perror("Memory allocation failed");
    return -1;
}
    read_block(fd,inode_block,inode_buffer);
    
    memcpy((char *)inode_buffer+inode_offset,inode,INODE_SIZE);
    write_to_partition(fd,inode_block,inode_buffer,BLOCK_SIZE);


    printf("inode %d created in block %d \n",inod,inode_block);
    return 1;
}