// your malloc headers and definitions
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

void printBinary(metadata *m);

void setup();

unsigned short get_inUse(metadata *m);

void set_inUse(metadata *m, unsigned short val);

void setSecreteKey(metadata *m, int val);

unsigned short getSize(metadata *m);

void setSize(metadata *m, unsigned short size);

void *findEmptyBlock(void *address);

void splitMemBlocks(void *address);

void *mymalloc(size_t size, char *filename, int line);

void myfree(void *address, char *filename, int line);

#endif