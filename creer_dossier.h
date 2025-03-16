#ifndef CREER_DOSSIER_H
#define CREER_DOSSIER_H

#include "filesystem.h"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h> 
#include <string.h>
#include "inode.h"
#include "block.h"

void creer_inode(char *nom,int type);
void write_to_partition(int fd, int block, void *data, size_t data_size);
void creer_dossier(char *nom);

#endif // CREER_DOSSIER_H
