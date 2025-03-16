#ifndef INODE_H
#define INODE_H

#include "filesystem.h"
#include <unistd.h>
#include <stdio.h>
typedef struct inode {
    uint32_t size;           
    uint32_t block_pointers[12]; 
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

inode init_inode();

#endif // INODE_H
