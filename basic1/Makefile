
all: basic1

clean:
	rm basic1

basic1: basic1.c
	gcc basic1.c -o basic1 -g -fno-stack-protector -fcf-protection=none -no-pie -z execstack