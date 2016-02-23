all: Server Client

Server:
        gcc -o Server Server.c

Client:
        gcc -o Client Client.c