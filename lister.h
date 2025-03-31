#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h> 
#include <stdio.h>
#include <string.h>
#include "filesystem.h"
#include "inode.h"


void lister_dossier(int fd, uint32_t inode_num);