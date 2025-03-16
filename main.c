#include <stdio.h>
#include "filesystem.h"
#include "inode.h"
#include "block.h"
#include <stdlib.h>
#include <string.h>
#include "creer_partition.h"

int main() {
    int fd = open(PARTITION_FILE, O_RDONLY);
    creer_filesystem();
    void *buffer = malloc(BLOCK_SIZE);
    get_super_block(fd,buffer);
    print_block_hex(buffer);
    close(fd);
    return 0;
}
