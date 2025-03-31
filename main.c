#include <stdio.h>
#include "filesystem.h"
#include "inode.h"
#include "block.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "creer_partition.h"
#include "lister.h"
#include <unistd.h>
int main() {
    creer_filesystem();
    int fd = open(PARTITION_FILE, O_RDWR | O_CREAT, 0666);
    void *buffer = malloc(BLOCK_SIZE);
    memset(buffer,0,BLOCK_SIZE);
    //creer_inode(fd,"bonjour",1);
    //read_block(fd,INODE_BITMAP_BLOCK,buffer);
    //print_block_hex(buffer);
    creer_dossier(fd,"hello",0);
    lister_dossier(fd,0);
    close(fd); 
    return 0; 
}
