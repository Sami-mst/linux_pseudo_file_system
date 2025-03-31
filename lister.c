
#include "lister.h"


void lister_dossier(int fd, uint32_t inode_num) {
    // Lire l'inode du répertoire
    int inod_block = inode_num / (BLOCK_SIZE / INODE_SIZE) + INODE_TABLE_START;
    int inod_offset = (inode_num % (BLOCK_SIZE / INODE_SIZE)) * INODE_SIZE;

    void *inode_block_buffer = malloc(BLOCK_SIZE);
    if (!inode_block_buffer) {
        perror("Memory allocation failed");
        return;
    }
    memset(inode_block_buffer, 0, BLOCK_SIZE);
    read_block(fd, inod_block, inode_block_buffer);

    inode dossier_inode = (inode)malloc(INODE_SIZE);
    if (!dossier_inode) {
        perror("Memory allocation failed");
        free(inode_block_buffer);
        return;
    }
    memcpy(dossier_inode, inode_block_buffer + inod_offset, INODE_SIZE);

    // Vérifier que l'inode est bien un répertoire
    if (dossier_inode->file_type != FILE_TYPE_DIRECTORY) {
        printf("L'inode n'est pas un répertoire\n");
        free(inode_block_buffer);
        free(dossier_inode);
        return;
    }

    // Parcourir les blocs directs du répertoire
    for (int i = 0; i < 8; i++) {
        uint32_t block_num = dossier_inode->block_pointers[i];
        if (block_num == 0) continue;  // Aucun bloc assigné

        // Lire le bloc entier (BLOCK_SIZE)
        void *block_buffer = malloc(BLOCK_SIZE);
        if (!block_buffer) {
            perror("Memory allocation failed");
            break;
        }
        read_block(fd, block_num, block_buffer);

        // Interpréter le bloc comme une liste de `dir_entry`
        dir_entry entries = (dir_entry)block_buffer;
        int num_entries = 1;
        if (entries->inode_number != 0 && valid_inode(fd, (int)entries->inode_number)) {
                printf("Nom : %s inode : %d\n", entries->name,entries->inode_number);
            }
        

        free(block_buffer);
    }

    free(inode_block_buffer);
    free(dossier_inode);
}
