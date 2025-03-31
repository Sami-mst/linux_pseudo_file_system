
#include "lister.h"


void lister_dossier(int fd, uint32_t inode_num) {
    // Lire l'inode du répertoire

    int inod_block=inode_num/(BLOCK_SIZE/INODE_SIZE)+INODE_TABLE_START;
    int inod_offset=(inode_num%(BLOCK_SIZE/INODE_SIZE))*64;
    void * inode_block_buffer=malloc(BLOCK_SIZE);
    memset(inode_block_buffer,0,BLOCK_SIZE);
    read_block(fd,inod_block,inode_block_buffer);
    inode dossier_inode=(inode)malloc(INODE_SIZE);
    memcpy(dossier_inode,inode_block_buffer+inod_offset,INODE_SIZE);


    // Vérifier que l'inode est bien un répertoire
    if (dossier_inode->file_type != FILE_TYPE_DIRECTORY) {
        printf("L'inode n'est pas un répertoire\n");
        return;
    }

    // Parcourir les blocs directs du répertoire
    for (int i = 0; i < 12; i++) {
        uint32_t block_num = dossier_inode->block_pointers[i];
        if (block_num == 0) continue;  // Aucun bloc assigné

        // Lire le bloc de données du répertoire
        dir_entry entries=(dir_entry)malloc(sizeof(struct dir_entry));
        read_block(fd, block_num, entries);
        // Afficher les entrées de répertoire
        if (entries->inode_number != 0) {  // Entrée valide
                printf("Nom : %s, Inode : %d\n", entries->name, entries->inode_number);
            }
        
    }
}