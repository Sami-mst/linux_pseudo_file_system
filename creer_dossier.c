#include "filesystem.h"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h> 
#include <stdio.h>
#include <string.h>
#include "inode.h"
#include "block.h"
#include <string.h>
#include "creer_partition.h"


void creer_dossier(int fd,char* nom,int inode_dossier_parent){
    if (file_exists_in_directory(fd,inode_dossier_parent,nom)==1){
        printf("erreur : dossier existe deja\n");
        return;
    }
    int inod=creer_inode(fd,nom,FILE_TYPE_DIRECTORY);
    int inod_parent_block=inode_dossier_parent/(BLOCK_SIZE/INODE_SIZE)+INODE_TABLE_START;
    int inod_parent_offset=(inode_dossier_parent%(BLOCK_SIZE/INODE_SIZE))*64;
    dir_entry dossier=(dir_entry)malloc(sizeof(struct dir_entry));
    strcpy(dossier->name,nom);
    dossier->inode_number=inod;
    int block=find_free_block(fd);
    write_to_partition(fd,block,dossier,BLOCK_SIZE);


    void * parent_block_buffer=malloc(BLOCK_SIZE);
    memset(parent_block_buffer,0,BLOCK_SIZE);
    read_block(fd,inod_parent_block,parent_block_buffer);
    inode parent_inode=(inode)malloc(INODE_SIZE);
    memcpy(parent_inode,parent_block_buffer+inod_parent_offset,INODE_SIZE);
    for (int i = 0; i < 8; i++) {
        if (parent_inode->block_pointers[i] == 0 || valid_block(fd,parent_inode->block_pointers[i])==0 || inod==parent_inode->block_pointers[i]) { 
            parent_inode->block_pointers[i]=block;
            break;
        }
    }
    memcpy(parent_block_buffer+inod_parent_offset,parent_inode,INODE_SIZE);
    write_to_partition(fd,inod_parent_block,parent_block_buffer,BLOCK_SIZE);
    //print_block_hex(parent_block_buffer);
    printf("created file in %s in block %d in inode %d\n",parent_inode->name,block,inod);
}

void write_to_partition(int fd, int block, void *data, size_t data_size) {

    ssize_t bytesWritten = pwrite(fd, data, data_size, block*BLOCK_SIZE);
    if (bytesWritten == -1) {
        perror("Failed to write to partition");
        close(fd);
    } else {
        printf("Successfully wrote %zd bytes to partition\n", bytesWritten);
    }
}

int creer_inode(int fd,char *nom,int type){
    inode inode = init_inode();  // creer le inode pour root 
    inode->file_type=type;
    inode->mode=777;
    inode->uid=0;
    inode->gid=0;
    inode->size=0;
    strcpy(inode->name,nom);
    inode->single_indirect=0;
    memset(inode->block_pointers,0,sizeof(inode->block_pointers));
    int inod=find_free_inode(fd);
    int inode_block=(inod / (BLOCK_SIZE / INODE_SIZE))+INODE_TABLE_START;
    int inode_offset=(inod%(BLOCK_SIZE / INODE_SIZE))*INODE_SIZE;
    void *inode_buffer = malloc(BLOCK_SIZE);
    memset(inode_buffer,0,BLOCK_SIZE);
    if (!inode_buffer) {
    perror("Memory allocation failed");
    return -1;
}
    read_block(fd,inode_block,inode_buffer);
    
    memcpy((char *)inode_buffer+inode_offset,inode,INODE_SIZE);
    write_to_partition(fd,inode_block,inode_buffer,BLOCK_SIZE);


    printf("inode %d created in block %d \n",inod,inode_block);
    return inod;
}

int file_exists_in_directory(int fd, uint32_t dir_inode_num, const char *filename) {
    // Lire l'inode du répertoire
    int inod_block = dir_inode_num / (BLOCK_SIZE / INODE_SIZE) + INODE_TABLE_START;
    int inod_offset = (dir_inode_num % (BLOCK_SIZE / INODE_SIZE)) * INODE_SIZE;

    void *inode_block_buffer = malloc(BLOCK_SIZE);
    if (!inode_block_buffer) {
        perror("Memory allocation failed");
        return -1;
    }
    read_block(fd, inod_block, inode_block_buffer);
    inode dir_inode = (inode)(inode_block_buffer + inod_offset);

    // Vérifier que l'inode est bien un répertoire
    if (dir_inode->file_type != FILE_TYPE_DIRECTORY) {
        printf("L'inode fourni n'est pas un répertoire.\n");
        free(inode_block_buffer);
        return -1;
    }

    // Parcourir les blocs directs du répertoire
    for (int i = 0; i < 8; i++) {
        uint32_t block_num = dir_inode->block_pointers[i];
        if (block_num == 0) continue;  // Aucun bloc assigné

        // Lire le bloc de données
        void *block_buffer = malloc(BLOCK_SIZE);
        if (!block_buffer) {
            perror("Memory allocation failed");
            free(inode_block_buffer);
            return -1;
        }
        read_block(fd, block_num, block_buffer);

        // Interpréter le bloc comme une liste de dir_entry
        dir_entry entries = (dir_entry)block_buffer;
        int num_entries = BLOCK_SIZE / sizeof(struct dir_entry);

        // Parcourir les entrées
        for (int j = 0; j < num_entries; j++) {
            if (entries[j].inode_number != 0 && valid_inode(fd, entries[j].inode_number)) {
                if (strncmp(entries[j].name, filename, MAX_FILENAME_LEN) == 0) {
                    free(block_buffer);
                    free(inode_block_buffer);
                    return 1;  // Fichier trouvé
                }
            }
        }

        free(block_buffer);
    }

    free(inode_block_buffer);
    return 0;  // Fichier non trouvé
}