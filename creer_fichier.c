
#include "creer_fichier.h"

void creer_fichier(int fd, char* nom, int inode_dossier_parent) {
    if (file_exists_in_directory(fd,inode_dossier_parent,nom)==1){
        printf("Erreur : fichier existe deja\n");
        return;
    }
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
int get_inode_by_name(int fd, uint32_t dir_inode_num, const char *filename) {
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
                    int inode_number = entries[j].inode_number;
                    free(block_buffer);
                    free(inode_block_buffer);
                    return inode_number;  // Retourne le numéro de l'inode trouvé
                }
            }
        }

        free(block_buffer);
    }

    free(inode_block_buffer);
    return -1;  // Fichier/dossier non trouvé
}

int copy_file_to_directory(int fd, const char *source_path, const char *dest_path) {
    char src_dir[128] = "";
    char src_filename[MAX_FILENAME_LEN];
    char dest_dir[128] = "";
    char dest_filename[MAX_FILENAME_LEN];

    // Extract source directory and filename
    char *last_slash = strrchr(source_path, '/');
    if (last_slash) {
        if (last_slash == source_path) { // Root directory case
            strcpy(src_filename, last_slash + 1);
        } else {
            strncpy(src_dir, source_path, last_slash - source_path);
            src_dir[last_slash - source_path] = '\0';
            strcpy(src_filename, last_slash + 1);
        }
    } else {
        printf("Invalid source path.\n");
        return -1;
    }

    last_slash = strrchr(dest_path, '/');
if (last_slash) {
    if (last_slash == dest_path) { // Root case ("/hello2")
        strcpy(dest_dir, last_slash + 1);  // Take "hello2" from "/hello2"
        dest_filename[0] = '\0'; // No specific file name
    } else {
        strncpy(dest_dir, dest_path, last_slash - dest_path);
        dest_dir[last_slash - dest_path] = '\0';
        strcpy(dest_filename, last_slash + 1);
    }
} else {
    printf("❌ Invalid destination path.\n");
    return -1;
}

    // 🔹 Get inode of source directory (default to ROOT_INODE if empty)
    int src_dir_inode = ROOT_INODE;
    if (strlen(src_dir) > 0) {
        src_dir_inode = get_inode_by_name(fd, ROOT_INODE, src_dir);
        if (src_dir_inode == -1) {
            printf("❌ Source directory not found: %s\n", src_dir);
            return -1;
        }
    }

    // 🔹 Get inode of source file
    int src_file_inode = get_inode_by_name(fd, src_dir_inode, src_filename);
    if (src_file_inode == -1) {
        printf("❌ Source file not found: %s\n", src_filename);
        return -1;
    }

    // 🔹 Get inode of destination directory
    int dest_dir_inode = ROOT_INODE;
    if (strlen(dest_dir) > 0) {
        dest_dir_inode = get_inode_by_name(fd, ROOT_INODE, dest_dir);
        if (dest_dir_inode == -1) {
            printf("❌ Destination directory not found: %s\n", dest_dir);
            return -1;
        }
    }

    // 🎯 Now we have:
    // ✅ `src_file_inode` → inode of file to copy
    // ✅ `dest_dir_inode` → inode of target directory

    printf("✅ Copying '%s' (inode %d) to directory '%s' (inode %d)\n", src_filename, src_file_inode, dest_dir, dest_dir_inode);

    // 📌 Copy process: create a new entry in destination directory
    creer_fichier(fd, src_filename, dest_dir_inode);

    return 0;
}

int get_inode_by_path(int fd, const char *path) {
    if (strcmp(path, "/") == 0) return ROOT_INODE;  // Special case for root
    
    int current_inode = ROOT_INODE;
    char path_copy[256];
    strcpy(path_copy, path);
    
    char *token = strtok(path_copy, "/");
    while (token != NULL) {
        
        current_inode = get_inode_by_name(fd, current_inode, token);
        if (current_inode == -1) {
            printf("❌ Path not found: %s\n", path);
            return -1;
        }
        
        token = strtok(NULL, "/");
    }

    return current_inode;
}

void move_file(int fd, const char *src_path, const char *dest_path) {
    // Get the inode of the source file/directory
    int src_inode = get_inode_by_path(fd, src_path);
    if (src_inode == -1) {
        printf("❌ Source file not found: %s\n", src_path);
        return;
    }

    // Extract source name from path
    char src_name[MAX_FILENAME_LEN];
    const char *last_slash = strrchr(src_path, '/');
    if (last_slash) {
        strncpy(src_name, last_slash + 1, MAX_FILENAME_LEN);
    } else {
        strncpy(src_name, src_path, MAX_FILENAME_LEN);
    }

    // Get the destination inode (if it's a directory)
    int dest_inode = get_inode_by_path(fd, dest_path);
    
    // If destination is not found, check if it's a rename operation
    if (dest_inode == -1) {
        const char *last_slash_dest = strrchr(dest_path, '/');
        if (!last_slash_dest) {
            printf("❌ Invalid destination: %s\n", dest_path);
            return;
        }
        // Extract new name
        char dest_dir[MAX_FILENAME_LEN];
        strncpy(dest_dir, dest_path, last_slash_dest - dest_path);
        dest_dir[last_slash_dest - dest_path] = '\0';

        // Get the parent directory of the new name
        dest_inode = get_inode_by_path(fd, dest_dir);
        if (dest_inode == -1) {
            printf("❌ Destination directory not found: %s\n", dest_dir);
            return;
        }

        // Remove from old directory and add to new one with a new name
        remove_from_directory(fd, src_inode, src_name);
        add_to_directory(fd, dest_inode, src_inode, last_slash_dest + 1);
        return;
    }

    // Check if destination is a valid directory
    inode dest_inode_data;
    read_inode(fd, dest_inode, &dest_inode_data);
    if (dest_inode_data->file_type != FILE_TYPE_DIRECTORY) {
        printf("❌ Destination is not a directory: %s\n", dest_path);
        return;
    }

    // Check if the file already exists in the destination
    if (get_inode_by_name(fd, dest_inode, src_name) != -1) {
        printf("❌ File already exists in destination: %s\n", dest_path);
        return;
    }

    // Remove from old directory
    remove_from_directory(fd, src_inode, src_name);
    
    // Add to the new directory
    add_to_directory(fd, dest_inode, src_inode, src_name);

    printf("✅ Moved '%s' (inode %d) to '%s' (inode %d)\n", src_name, src_inode, dest_path, dest_inode);
}

