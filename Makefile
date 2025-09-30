dragonshell: dragonshell.o jobs.o builtins.o
	gcc -o dragonshell dragonshell.o jobs.o builtins.o

compile: dragonshell.c jobs.c builtins.c
	gcc -c dragonshell.c jobs.c builtins.c

clean:
	rm -f dragonshell dragonshell.o jobs.o builtins.o

run: clean dragonshell
	./dragonshell