CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -D_POSIX_C_SOURCE=200809L
LDFLAGS = -lc
OBJ = main.o filesystem.o inode.o block.o creer_partition.o creer_dossier.o

all: main

main: $(OBJ)
	$(CC) $(CFLAGS) -o main $(OBJ) $(LDFLAGS)

main.o: main.c filesystem.h inode.h block.h creer_partition.h creer_dossier.h
	$(CC) $(CFLAGS) -c main.c

filesystem.o: filesystem.c filesystem.h
	$(CC) $(CFLAGS) -c filesystem.c

inode.o: inode.c inode.h creer_dossier.h
	$(CC) $(CFLAGS) -c inode.c

block.o: block.c block.h
	$(CC) $(CFLAGS) -c block.c

creer_partition.o: creer_partition.c creer_partition.h block.h filesystem.h inode.h creer_dossier.h
	$(CC) $(CFLAGS) -c creer_partition.c

creer_dossier.o: creer_dossier.c creer_dossier.h block.h filesystem.h inode.h
	$(CC) $(CFLAGS) -c creer_dossier.c

clean:
	rm -f *.o main
