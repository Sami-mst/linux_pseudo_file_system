#include "block.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
block init_block() {
    block blk=malloc(BLOCK_SIZE);
    if(blk==NULL){
        return NULL;
    }
    memset(blk->data, 0, BLOCK_SIZE);
    return blk;
}