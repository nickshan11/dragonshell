dragonshell: dragonshell.o jobs.o
	gcc -o dragonshell dragonshell.o jobs.o

compile: dragonshell.c jobs.c
	gcc -c dragonshell.c jobs.c

clean:
	rm -f dragonshell dragonshell.o jobs.o

run: clean dragonshell
	./dragonshell