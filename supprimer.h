#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h> 
#include <stdio.h>
#include <string.h>
#include "filesystem.h"
#include "inode.h"

void supprimer_inode(int fd, int inode_num);
