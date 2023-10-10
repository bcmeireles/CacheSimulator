CC = gcc
CFLAGS=-Wall -Wextra
TARGET=Cache

all:
	$(CC) $(CFLAGS) SimpleProgram.c Cache.c -o $(TARGET)

clean:
	rm $(TARGET)
