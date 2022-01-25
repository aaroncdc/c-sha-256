all:
	gcc sha256.c -o sha256 -lm

debug:
	gcc -g sha256.c -o sha256 -lm