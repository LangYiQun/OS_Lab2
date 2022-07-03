CMD = mysh
all:
	gcc -c $(CMD).c mylib.c
	ld /usr/local/lib/crt0.o $(CMD).o mylib.o -o $(CMD)
