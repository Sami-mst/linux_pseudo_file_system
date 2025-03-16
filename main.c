#include <stdio.h>
#include "filesystem.h"
#include "inode.h"
#include "block.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "creer_partition.h"
#include <unistd.h>
int main() {
    int fd = open(PARTITION_FILE, O_RDONLY);
    creer_filesystem();
    void *buffer = malloc(BLOCK_SIZE);

    print_block_hex(buffer);
    close(fd); 
    return 0;
}
