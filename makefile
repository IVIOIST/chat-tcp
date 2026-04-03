CC = gcc
CFLAGS = -Wall -Wextra -std=c11

all: server client

server: server.c common.c
	$(CC) $(CFLAGS) -o server server.c common.c

client: client.c common.c
	$(CC) $(CFLAGS) -o client client.c common.c

clean:
	rm -f server client