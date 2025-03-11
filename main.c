#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>

#define MAX_FILENAME_LEN 60
#define PARTITION_FILE "partition.img"
#define BLOCK_SIZE 512
#define INODE_SIZE 64


#define FILE_TYPE_FILE  1  // ficher
#define FILE_TYPE_DIRECTORY 2  // dossier
#define TOTAL_BLOCKS 65536  
#define TOTAL_INODES 656
#define INODE_BITMAP_SIZE_IN_BYTES (TOTAL_BLOCKS / 8)  // 1 bit par block
#define BLOCK_BITMAP_SIZE_IN_BYTES (TOTAL_INODES / 8)  // 1 bit par block
#define INODE_BITMAP_BLOCK 1  // block ou le bitmap des inodes commencent
#define BLOCK_BITMAP_BLOCK 2  // Block ou le bitmap des block de données commencent
#define INODE_TABLE_START 3  // block ou la table des inodes commencent
#define DATA_BLCOK_START INODE_TABLE_START+(TOTAL_INODES/(BLOCK_SIZE/INODE_SIZE)) // block ou les blocks de données commencent 

typedef uint32_t block_addr; //poiteur vers block quelconque 

typedef struct indirect{ //structure de block indirect
    block_addr block_pointers[BLOCK_SIZE/sizeof(block_addr)];
}indirect;

unsigned char inode_bitmap[INODE_BITMAP_SIZE_IN_BYTES];  
unsigned char block_bitmap[BLOCK_BITMAP_SIZE_IN_BYTES];

struct superblock {
    uint32_t block_size;
    uint32_t total_blocks;
    uint32_t total_inodes;
    uint32_t free_blocks;
    uint32_t free_inodes;
    uint32_t inode_bitmap_start;
    uint32_t block_bitmap_start;
    uint32_t inode_table_start;
    uint32_t data_block_start;
    uint32_t root_inode;
    char reserved[128];
};

struct inode {
    uint32_t size;            // taille du fichier ou dossier
    uint32_t block_pointers[12]; // poiteurs vers block de donnée
    uint32_t single_indirect; // Pointer vers block indirect
    uint32_t double_indirect; // Pointer vers double block indirect
    uint8_t mode;            // Permissions
    uint8_t uid;             // Owner user ID
    uint8_t gid;             // Owner group ID
    uint8_t file_type;        // type fichier (1 = file , 2 = dossier, etc.)
}inode;

typedef struct dir_entry { //entrée de fichier dans inode dossier 
    char name[MAX_FILENAME_LEN];  // non du fichier
    uint32_t inode_number;        // inode correspondant au fichier
} dir_entry;

struct block {
    char data[BLOCK_SIZE];  // 512 bytees of raw data
};

typedef struct {
    dir_entry entries[BLOCK_SIZE/sizeof(dir_entry)];  // Directory entries per block
} DirBlock;

// Main function for testing
int main() {
    printf("%d bytes\n",sizeof(dir_entry));
    return 0;
}
