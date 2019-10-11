all: memgrind

mymalloc: mymalloc.c
		gcc -g -Wall mymalloc.c -o mymalloc
memgrind: mymalloc.c memgrind.c
		gcc -g -Wall -c mymalloc.c
		gcc -g -Wall memgrind.c mymalloc.o -o memgrind
clean:
	 	rm -f mymalloc 
