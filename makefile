CCFLAGS = -Wall -Wextra 

all:  client server

client:  client.c
	gcc $(CCFLAGS)  -O2 -g -o  client client.c send_packet.c

server: server.c
	gcc $(CCFLAGS)  -O2 -g -o  server server.c pgmread.c




clean:  
	- rm -f client
	- rm -f server

