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
#include "creer_fichier.h"
#include "supprimer.h"
int main() {
    creer_filesystem();
    int fd = open(PARTITION_FILE, O_RDWR | O_CREAT, 0666);
    void *buffer = malloc(BLOCK_SIZE);
    memset(buffer,0,BLOCK_SIZE);
    //creer_inode(fd,"bonjour",1);
    //read_block(fd,INODE_BITMAP_BLOCK,buffer);
    //print_block_hex(buffer);
    //creer_dossier(fd,"hello",0);
    creer_dossier(fd,"hello2",0);
    creer_fichier(fd,"f6.txt",0);
    //supprimer_inode(fd,10);
    move_file(fd,"/f6.txt", "/hello2/f6.txt");
    lister_dossier(fd,0);
    close(fd); 
    return 0; 
}
