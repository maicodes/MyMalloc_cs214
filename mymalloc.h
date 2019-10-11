<<<<<<< HEAD
// your malloc headers and definitions
=======
 // your malloc headers and definitions
>>>>>>> 21a79decc1dfb8703ffa96da8902ae89aac9937f
#ifndef mymalloc_h_
#define mymalloc_h_

#include <stdlib.h>
#include <stdio.h>

#define malloc(x) mymalloc(x, __FILE__, __LINE__)
#define free(x) myfree(x, __FILE__, __LINE__)
#define HEAPSIZE 4096
typedef struct _metadata
{
    unsigned short data;
} metadata;

typedef enum _bool
{
    False,
    True
} boolean;

<<<<<<< HEAD
void printBinary(metadata *m);

=======
>>>>>>> 21a79decc1dfb8703ffa96da8902ae89aac9937f
void setup();

unsigned short get_inUse(metadata *m);

void set_inUse(metadata *m, unsigned short val);

<<<<<<< HEAD
void setSecreteKey(metadata *m, int val);

=======
>>>>>>> 21a79decc1dfb8703ffa96da8902ae89aac9937f
unsigned short getSize(metadata *m);

void setSize(metadata *m, unsigned short size);

void *findEmptyBlock(void *address);

void splitMemBlocks(void *address);

void *mymalloc(size_t size, char *filename, int line);

void myfree(void *address, char *filename, int line);

<<<<<<< HEAD
#endif
=======
#endif
>>>>>>> 21a79decc1dfb8703ffa96da8902ae89aac9937f
