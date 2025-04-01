#ifndef CREER_FICHIER_H
#define CREER_FICHIER_H

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
#include "creer_dossier.h"
void creer_fichier(int fd, char* nom, int inode_dossier_parent);
int get_inode_by_name(int fd, uint32_t dir_inode_num, const char *filename);
int copy_file_to_directory(int fd, const char *source_path, const char *dest_path);
int get_inode_by_path(int fd, const char *path);
void move_file(int fd, const char *src_path, const char *dest_path);
#endif // CREER_FICHIER_H