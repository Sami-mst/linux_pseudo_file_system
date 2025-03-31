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

#endif // CREER_FICHIER_H