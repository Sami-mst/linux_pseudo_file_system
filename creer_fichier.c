
#include "creer_fichier.h"

void creer_fichier(int fd, char* nom, int inode_dossier_parent) {
    // Step 1: Create an inode for the new file
    int new_inode = creer_inode(fd, nom, FILE_TYPE_FILE);
    if (new_inode < 0) {
        printf("Erreur : Impossible d'allouer un inode pour le fichier %s\n", nom);
        return;
    }

    // Step 2: Read the parent inode
    int inode_parent_block = inode_dossier_parent / (BLOCK_SIZE / INODE_SIZE) + INODE_TABLE_START;
    int inode_parent_offset = (inode_dossier_parent % (BLOCK_SIZE / INODE_SIZE)) * INODE_SIZE;

    void* parent_block_buffer = malloc(BLOCK_SIZE);
    memset(parent_block_buffer, 0, BLOCK_SIZE);
    read_block(fd, inode_parent_block, parent_block_buffer);

    inode parent_inode = (inode)malloc(INODE_SIZE);
    memcpy(parent_inode, parent_block_buffer + inode_parent_offset, INODE_SIZE);

    // Step 3: Find a free data block to store the new directory entry
    int data_block = find_free_block(fd);
    if (data_block < 0) {
        printf("Erreur : Pas de bloc libre pour stocker l'entrée de répertoire.\n");
        free(parent_block_buffer);
        free(parent_inode);
        return;
    }

    // Step 4: Create the directory entry for the new file
    dir_entry fichier = (dir_entry)malloc(sizeof(struct dir_entry));
    strcpy(fichier->name, nom);
    fichier->inode_number = new_inode;

    // Step 5: Write the directory entry to the allocated block
    write_to_partition(fd, data_block, fichier, BLOCK_SIZE);

    // Step 6: Update the parent directory's inode to point to the new block
    for (int i = 0; i < 8; i++) {
        if (parent_inode->block_pointers[i] == 0 || valid_block(fd,parent_inode->block_pointers[i])==0 || new_inode==parent_inode->block_pointers[i]) { 
            parent_inode->block_pointers[i] = data_block;
            //printf("in pointer %d\n",i);
            break;
        }
    }

    // Write back the updated inode to disk
    memcpy(parent_block_buffer + inode_parent_offset, parent_inode, INODE_SIZE);
    write_to_partition(fd, inode_parent_block, parent_block_buffer, BLOCK_SIZE);

    printf("Fichier '%s' créé dans le dossier inode %d (stocké dans le bloc %d) avec inode %d\n", nom, inode_dossier_parent, data_block,new_inode);

    // Free allocated memory
    free(parent_block_buffer);
    free(parent_inode);
    free(fichier);
}