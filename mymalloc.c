#include "mymalloc.h"

unsigned short avaiMemory = 4094; // keep track of how much memory we have left
boolean isFirstCall = True;       // flag the first time mymalloc is called

/*
*  MY MALLOC IMPLEMENTATION
*/

static char myblock[HEAPSIZE];
static metadata *start = (metadata *)myblock;
static metadata *tail = (metadata *)myblock;

void *mymalloc(size_t inputSize, char *filename, int line)
{
    unsigned short size = inputSize;

    if (size == 0)
        return NULL;

    if (avaiMemory < size) {
        printf("Error: Not enough memory in heap.\n%hu bytes left in heap.\n",
               avaiMemory);
        return NULL;
    }

    if (isFirstCall)
    {
        //setup
        set_inUse(start, 0);
        setSecreteKey(start, SECRETEKEY);
        setSize(start, HEAPSIZE - sizeof(metadata));
        printf("start Size: %hu\n", getSize(start));
        isFirstCall = False;
    }

    void *result = NULL;
    metadata *ptr = start;
    metadata *smallest = NULL;
    boolean foundBlock = False;

    /*traverse through char array
    check not in-use and sufficient size metadata
    if not, keep traversing until meet the last metadata
    if found, can update the address of smallest possible block?
    if split the tail -> update tail
    */

    while (ptr != tail)
    {
        printf("inside while loop\n");

        if (foundBlock &&
            (smallest == NULL ||
             getSize(ptr) < getSize(smallest)))
        {
            printf("here\n");
            smallest = ptr;
        }

        foundBlock = checkValidBlock(ptr, size);
        ptr = (metadata *)(ptr + sizeof(metadata) + getSize(ptr));
        printf("end 1 loop\n");
    }

    //check the tail
    foundBlock = checkValidBlock(ptr, size);
    boolean isTail = False;

    if (foundBlock &&
        (smallest == NULL ||
         getSize(ptr) < getSize(smallest)))
    {
        smallest = ptr;
        printf("smallest: %x\n", smallest);
    }

    isTail = smallest == tail ? True : False;

    if (isTail)
        printf("smallest is tail\n");
    else
        printf("smallest is not tail\n");

    if (!foundBlock)
    {
        printf("Error: malloc failed. Cannot find a sufficient space.\n"
               "At file: %s\tLine:%d\n",
               filename, line);
        return result;
    }

    /*
    Check list:
    - set smallest in-use to 1
    if there is a split:
    - create another metadata
    - set smallest size and newMetadata size
    - set secret Key
    */

    set_inUse(smallest, 1);
    unsigned short smallestSize = getSize(smallest);

    if (getSize(smallest) > size + sizeof(metadata))
    {
        metadata *nextMetadata = smallest + sizeof(metadata) + size;
        avaiMemory -= sizeof(metadata);
        set_inUse(nextMetadata, 0);
        setSecreteKey(nextMetadata, SECRETEKEY);
        setSize(nextMetadata, smallestSize - size - sizeof(metadata));
        printf("size of nextMetadata %hu\n", getSize(nextMetadata));
        setSize(smallest, size);
        printf("new size of smallest %hu\n", getSize(smallest));

        if (isTail)
            tail = nextMetadata;
    }

    result = smallest + sizeof(metadata);
    avaiMemory -= smallestSize;
    printf("avai memory: %hu\n", avaiMemory);

    return result;
};

/*
*  MY FREE IMPLEMENTATION
*/

/*
Check list when myfree is called:
  - if there is a merge :
    - set size of the left metadata to new size; set size of the right metadata to 0
    - zero out secrete key + in-use of the right metadata
  - if no merge:
    - set in-use = 0, update new size
*/

void printBinary(metadata *m)
{
    unsigned short n = m->data;
    while (n)
    {
        if (n & 1)
            printf("1");
        else
            printf("0");
        n >>= 1;
    }
    printf("\n");
};

unsigned short get_inUse(metadata *m)
{ // get the first bit of data - if in-use, 1st bit is 1, else 0
    return (m->data & (1 << 15)) >> 15;
};

void set_inUse(metadata *m, unsigned short val)
{ // set the first bit of data
    m->data = val == 0 ? m->data & ~(1 << 15) : m->data | (1 << 15);
    //printf("Set inuse to %hu", val);
    if (val == 1)
        printBinary(m);
};

boolean isAllocated(metadata *m)
{
    //check secrete Key, if it is 7, the address is allocated by mymalloc
    return (m->data & SECRETEKEY) == 7 ? True : False;
};

void setSecreteKey(metadata *m, int val) // val is either 0 or secreteKey
{
    m->data = val == 0 ? m->data & ~(0x7 << 12) : m->data | SECRETEKEY;
    //printf("Set secrete key to %d", val);
    //printBinary(m);
};

unsigned short getSize(metadata *m)
{ //get the last 12 bits of data
    return m->data & GET_SIZE_NUM;
};

void setSize(metadata *m, unsigned short size)
{
    m->data = (m->data & SET_SIZE_NUM) | size;
    //printf("Set size to %hu", size);
    //printBinary(m);
};

boolean checkValidBlock(metadata *address, unsigned short size)
{
    return (!get_inUse(address) && getSize(address) >= size) ? True : False;
};
