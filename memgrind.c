// your memory test and profiling code as described above

#include <stdio.h>
#include <stdlib.h>
#include "mymalloc.h"
int* pointers[100];

int main(int argc, const char **argv)
{
    //printf("here");
    /* test code here */

    int i;
    for (i = 0; i < 10; i++)
    {
        pointers[i] = (int*) malloc(10);
        printf("pointers at %d : %d\n", i, pointers[i]);
        printf("\n\n");
    }

    return 0;
};