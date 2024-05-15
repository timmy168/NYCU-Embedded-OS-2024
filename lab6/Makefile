CC = gcc
all: client server

client: client.c 
	$(CC) -o client client.c 

server: server.c 
	$(CC) -o server server.c  -lpthread

clean:
	rm server client
