#ifndef INODE_H
#define INODE_H

#include "filesystem.h"
#include <unistd.h>
#include <stdio.h>
typedef struct inode {
    char name[16];
    uint32_t size;           
    uint32_t block_pointers[8]; 
    uint32_t single_indirect;
    uint16_t mode;
    uint8_t uid;           
    uint8_t gid;          
    uint8_t file_type;    
} *inode;

typedef struct dir_entry { 
    char name[MAX_FILENAME_LEN];
    uint32_t inode_number;        
}*dir_entry;
int find_free_inode(int fd);
inode init_inode();
int valid_inode(int fd, int inode_num);
#endif // INODE_H
