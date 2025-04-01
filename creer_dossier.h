#ifndef CREER_DOSSIER_H
#define CREER_DOSSIER_H

#include "filesystem.h"
#include <stdio.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h> 
#include <string.h>
#include "inode.h"
#include "block.h"
#include <string.h>

int creer_inode(int fd,char *nom,int type);
void write_to_partition(int fd, int block, void *data, size_t data_size);
void creer_dossier(int fd,char* nom,int inode_dossier_parent);
int file_exists_in_directory(int fd, uint32_t dir_inode_num, const char *filename);
//void creer_dossier(char *nom);

#endif // CREER_DOSSIER_H
