all: server.o
	gcc server.o -o server -lpthread -O2

server.o: server.c
	gcc -c server.c

clean:
	rm -r server.o server
