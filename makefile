CC = gcc
CFLAGS = -Wall -Wextra -std=c99

OBJ = main.o filesystem.o inode.o block.o creer_partition.o

all: main

main: $(OBJ)
	$(CC) $(CFLAGS) -o main $(OBJ)

main.o: main.c filesystem.h inode.h block.h creer_partition.h
	$(CC) $(CFLAGS) -c main.c

filesystem.o: filesystem.c filesystem.h 
	$(CC) $(CFLAGS) -c filesystem.c

inode.o: inode.c inode.h 
	$(CC) $(CFLAGS) -c inode.c

block.o: block.c block.h
	$(CC) $(CFLAGS) -c block.c

creer_partition.o: creer_partition.c creer_partition.h block.h filesystem.h inode.h 
clean:
	rm -f *.o main
