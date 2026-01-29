
all: server

clean:
	rm httpserver

server: server.c
	gcc server.c -o httpserver -g -fno-stack-protector -fcf-protection=none -no-pie -z execstack