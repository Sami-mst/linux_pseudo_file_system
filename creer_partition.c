#include "filesystem.h"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h> 
#include <string.h>
#include "inode.h"
#include "block.h"
#include "creer_dossier.h"
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
    write_to_partition(fd,0,sb,sizeof(struct superblock));

    write_to_partition(fd,INODE_BITMAP_BLOCK,inode_bitmap,INODE_BITMAP_SIZE_IN_BYTES);

    write_to_partition(fd,BLOCK_BITMAP_BLOCK,block_bitmap,BLOCK_BITMAP_SIZE_IN_BYTES);

    inode root_inode = init_inode();  // creer le inode pour root 
    root_inode->file_type=FILE_TYPE_DIRECTORY;
    root_inode->mode=777;
    root_inode->uid=0;
    root_inode->gid=0;
    root_inode->size=0;
    root_inode->single_indirect=0;
    memset(root_inode->block_pointers,0,sizeof(root_inode->block_pointers));

    write_to_partition(fd,INODE_TABLE_START,root_inode,INODE_SIZE);

    // marquer le premier inode comme utilisé
    inode_bitmap[0] = 0x01;  

    write_to_partition(fd,INODE_BITMAP_BLOCK,inode_bitmap,INODE_BITMAP_SIZE_IN_BYTES);


    close(fd);
    printf("Filesystem formatted successfully.\n");
}

