#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "mymalloc.h"

/* A: malloc() 1 byte and immediately free it - do this 150 times */
int testA()
{
    struct timeval start, end;
    gettimeofday(&start, NULL);

    int i = 0;
    while (i < 150)
    {
        char *a = malloc(1); // Malloc() 1 byte then immediately free()
        free(a);
        i++;
    }

    gettimeofday(&end, NULL);

    return (int)((end.tv_sec - start.tv_sec) / 10000000 + (end.tv_usec - start.tv_usec));
}

int testB()
{
    struct timeval start, end;
    gettimeofday(&start, NULL);

    char *array[50];
    int i = 0;
    int j = 0;
    int k = 0;

    while (i < 3) // Each i = 50 operations completed
    {
        // Malloc
        while (j < 50) // Malloc 50 bytes
        {
            array[j] = malloc(1);
            j += 1;
        }
        j = 0;

        // Free
        while (k < 50) // Free 50 bytes
        {
            free(array[k]);
            k += 1;
        }
        k = 0;

        i += 1;
    }

    gettimeofday(&end, NULL);
    return (int)((end.tv_sec - start.tv_sec) / 10000000 + (end.tv_usec - start.tv_usec));
}

int testC()
{
    struct timeval start, end;
    gettimeofday(&start, NULL);

    int boolean;            // Random number.
    int operationCount = 0; // Total amount of malloc() pointers.
    int totalPointers = 0;  // Amount of pointers in array.
    int *slots[50];         // Pointer storage.

    unsigned long int randomNumber = time(NULL); // Seed for random number.

    while (operationCount < 50) // do this until you have allocated 50 times
    {
        randomNumber = (unsigned int)(((randomNumber * 1103515245 + 12345) / 65536) % 32768);
        boolean = randomNumber % 2;
        if (boolean) // If malloc()ing
        {
            operationCount += 1;              // Increment operation count
            slots[totalPointers] = malloc(1); // Malloc 1 byte to array at index
            totalPointers += 1;               // Increment pointer count
        }
        else // If free()ing
        {
            if (totalPointers > 0) // Only free if there is at least one pointer
            {
                free(slots[totalPointers - 1]); // Free pointer at index
                totalPointers -= 1;             // Update pointer count
            }
        }
    }

    // Loop frees any remaining pointers
    while (totalPointers > 0)
    {
        free(slots[totalPointers - 1]); // Must subtract 1 from totalPointers to account for the increment after adding a pointer
        totalPointers -= 1;             // Update pointer count
    }

    gettimeofday(&end, NULL);
    return (int)((end.tv_sec - start.tv_sec) / 10000000 + (end.tv_usec - start.tv_usec));
}

int testD()
{
    struct timeval start, end;
    gettimeofday(&start, NULL);

    int boolean = 0;                             // Random number.
    int memoryAmount = 0;                        // Randomly chosen amount of memory (between 1-64).
    int *randomMemoryArray[200];                 // Holds pointers.
    int sizeOfMemory[200];                       // Holds size of pointers.
    int operationCount = 0;                      // Number of operations.
    int remainingMemory = 4096;                  // Amount of remaining memory from malloc.
    int totalPointers = 0;                       // Total number of pointers.
    unsigned long int randomNumber = time(NULL); // Seed for random number.

    while (operationCount < 50) // Keep track of each operation so that you eventually malloc() 50 times.
    {
        randomNumber = (unsigned int)(((randomNumber * 1103515245 + 12345) / 65536) % 32768);
        boolean = randomNumber % 2;

        if (boolean) // Randomly choose between a randomly-sized malloc() or free()ing a pointer – do this many times.
        {
            memoryAmount = (randomNumber % 64) + 1; // Choose a random allocation size between 1 and 64 bytes.

            if ((memoryAmount + sizeof(metadata)) <= remainingMemory) // Keep track of each malloc so that all mallocs do not exceed your total memory capacity.
            {
                remainingMemory -= memoryAmount + sizeof(metadata);            // Update remaining memory to use
                randomMemoryArray[totalPointers] = malloc(memoryAmount);       // Allocates memory to address in array
                sizeOfMemory[totalPointers] = memoryAmount + sizeof(metadata); // Respective array to randomMemoryArray that keeps track of what size each pointer is
                totalPointers += 1;                                            // Increment pointer count
                operationCount += 1;                                           // Increment operation count
            }
        }
        else // Execute if freeing a pointer was randomly chosen
        {
            if (totalPointers > 0) // Only free if there are available pointers to free
            {
                remainingMemory += sizeOfMemory[totalPointers - 1]; // Update remaining memory to use
                free(randomMemoryArray[totalPointers - 1]);         // Free memory at the array index

                totalPointers -= 1; // Update pointer count
            }
        }
    }

    while (totalPointers > 0) // If at the end, there are still pointers, then free any remaining pointers
    {
        remainingMemory += sizeOfMemory[totalPointers - 1]; // Update remaining memory
        free(randomMemoryArray[totalPointers - 1]);         // Free pointer at array index
        totalPointers -= 1;                                 // Update pointer count
    }

    gettimeofday(&end, NULL);
    return (int)((end.tv_sec - start.tv_sec) / 10000000 + (end.tv_usec - start.tv_usec));
}

int testE()
{
    struct timeval start, end;
    gettimeofday(&start, NULL);
    int row = 50;
    int col = 50;
    int i = 0;

    //create a 2D array
    char ***twoDArray = (char ***)malloc(row * sizeof(char **));
    for (i = 0; i < row; i++)
    {
        twoDArray[i] = (char *)malloc(col * sizeof(char));
    }

    //free a 2D array
    for (i = 0; i < row; i++)
    {
        free(twoDArray[i]);
    }
    free(twoDArray);

    gettimeofday(&end, NULL);

    return (int)((end.tv_sec - start.tv_sec) / 10000000 + (end.tv_usec - start.tv_usec));
}

int main(int argc, const char *argv[])
{
    float totalForTestA = 0.0; // Holds the sum of all the test times
    float totalForTestB = 0.0;
    float totalForTestC = 0.0;
    float totalForTestD = 0.0;
    float totalForTestE = 0.0;

    int i;
    for (i = 0; i < 100; ++i)
    {
        totalForTestA += testA();
    }

    for (i = 0; i < 100; ++i)
    {
        totalForTestB += testB();
    }

    for (i = 0; i < 100; ++i)
    {
        totalForTestC += testC();
    }

    for (i = 0; i < 100; ++i)
    {
        totalForTestD += testD();
    }

    for (i = 0; i < 100; ++i)
    {
        totalForTestE += testE();
    }

    printf("%sAFTER 100 TRIALS:%s\n", "\x1B[32m", "\033[0m"); // Outputs averaged test times
    printf("%sTEST A AVERAGE: %.2f microseconds%s\n", "\x1B[32m", totalForTestA / 100, "\033[0m");
    printf("%sTEST B AVERAGE: %.2f microseconds%s\n", "\x1B[32m", totalForTestB / 100, "\033[0m");
    printf("%sTEST C AVERAGE: %.2f microseconds%s\n", "\x1B[32m", totalForTestC / 100, "\033[0m");
    printf("%sTEST D AVERAGE: %.2f microseconds%s\n", "\x1B[32m", totalForTestD / 100, "\033[0m");
    printf("%sTEST E AVERAGE: %.2f microseconds%s\n", "\x1B[32m", totalForTestE / 100, "\033[0m");

    return 0;
}
