CC=gcc
CFLAGS=-Wall
DEBUGFLAGS=-g

all: server client

server: server.c
	$(CC) $(CFLAGS) server.c -o server

client: client.c
	$(CC) $(CFLAGS) client.c -o client

dev: server_debug client_debug

server_debug: server.c
		$(CC) $(CFLAGS) $(DEBUGFLAGS) server.c -o server

client_debug: client.c
		$(CC) $(CFLAGS) $(DEBUGFLAGS) client.c -o client

debug_run: dev
	gdb ./client -ex "run localhost 4000 1364"

clean:
	rm -f server client
