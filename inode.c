#include "inode.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
inode init_inode() {
    inode inod=malloc(INODE_SIZE);
    if (inod==NULL){
        return NULL;
    }
    memset(inod, 0, INODE_SIZE);
    inod->size = 0;
    return inod;
}
