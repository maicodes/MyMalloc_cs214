#include "mymalloc.h"

unsigned short avaiMemory = 4094; // keep track of how much memory we have left
boolean isFirstCall = True;       // signal the first time mymalloc is called

static char myblock[HEAPSIZE];
static metadata *start = (metadata *)myblock;
static metadata *tail = (metadata *)myblock;
static metadata *first_not_inuse = NULL;

/*
 *   mymalloc()
 *   @params:
 *       int inputSize: The amount of memory the user requested
 *       const char* filename: Holds the filename name of the parent
 *       int line: Holds the line number of the parent
 *   @returns: void *storage
 *   @comments: Returns a pointer to a user that contains an address to a block of memory in myblock[]
 **/

void *mymalloc(size_t inputSize, char *filename, int line)
{
    unsigned short size = inputSize;

    if (size == 0)
        return NULL;

    if (avaiMemory < size)
    {
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
        isFirstCall = False;
        first_not_inuse = start;
    }

    void *result = NULL;
    metadata *ptr = first_not_inuse;
    boolean foundBlock = False;

    /*traverse through char array and
    check for a valid block of memory
    if we need to split the tail -> update tail
    */

    while (ptr <= tail)
    {
        foundBlock = checkValidBlock(ptr, size);
        if (foundBlock)
        {
            result = ptr;
            break;
        }

        ptr = (metadata *)((void *)ptr + sizeof(metadata) + getSize(ptr));
    }

    boolean isTail = (result == tail) ? True : False;

    if (result == NULL || !foundBlock)
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

    set_inUse(result, 1);
    unsigned short resultSize = getSize(result);
    metadata *nextMetadata = NULL;

    if (resultSize > size + sizeof(metadata))
    { // split metadata
        nextMetadata = (metadata *)((void *)result + sizeof(metadata) + size);
        avaiMemory -= sizeof(metadata);
        set_inUse(nextMetadata, 0);
        setSecreteKey(nextMetadata, SECRETEKEY);
        setSize(nextMetadata, resultSize - size - sizeof(metadata));
        setSize(result, size);

        if (isTail)
            tail = nextMetadata;
    }

    if (first_not_inuse == result)
    { //first_not_inuse is in-use
        first_not_inuse = nextMetadata;
    }

    avaiMemory -= getSize(result);
    result += sizeof(metadata);
    return result;
};

/*
 *   myfree()
 *   @params:
 *       void * memory: pointer which is requested to be deleted.
 *       const char* filename: Holds the filename name of the parent
 *       int line: Holds the line number of the parent
 *   @returns: void
 *   @comments: Changes metadata of node to allow overwritting, then defragments data array,
 *   undefined behavior may occur on big endian machines.
 **/

void myfree(void *memory, char *filename, int line)
{
    boolean foundFlag = False; // Flag used in the testing of whether or not a match has been found between nodes and the pointer passed-in

    if (memory == NULL)
    {
        fprintf(stderr, "\nERROR: Trying to free a NULL address.\n"
                        "filename: %s\n"
                        "Line: %d\n",
                filename, line);
        return;
    }

    if (memory < (void *)start || memory > (void *)start + HEAPSIZE - 1)
    {
        fprintf(stderr, "\nERROR: Address is out of heap bounds.\n"
                        "filename: %s\n"
                        "Line: %d\n",
                filename, line);
        return;
    }

    if (!isAllocated((metadata *)(memory - sizeof(metadata))))
    { // address is not a metadata
        fprintf(stderr, "\nERROR: Address is not dynamically alocated.\n"
                        "filename: %s\n"
                        "Line: %d\n",
                filename, line);
        return;
    }

    /*
     *   Checks to see if pointer has memory address residing in the heap,
     *   if it does, then one little endian machines it will not have been
     *   malloced by mymalloc.
     *
     *   A: Free()ing addresses that are not pointers.
     *   B: Free()ing pointers that were not allocated by malloc().
     **/

    metadata *node = start;
    metadata *prev = node;
    while (node <= tail)
    {
        if ((metadata *)(memory - sizeof(metadata)) == node)
        {
            foundFlag = True; // If the (address - metadata) matches the address of a node, then a match has been found
            break;
        }
        prev = node;
        node = (metadata *)((void *)node + sizeof(metadata) + getSize(node));
    }

    if (!foundFlag) //the address is not in our metadata list
    {
        fprintf(stderr, "\nERROR: Address is not dynamically alocated.\n"
                        "filename: %s\n"
                        "Line: %d\n",
                filename, line);
        return;
    }

    /* C: Redundant free()ing of the same pointer. */
    if (get_inUse(node) == 0) // If memory address matches, but the node is not in use, then it is redundant free()ing
    {
        fprintf(stderr, "\nERROR: Redundant free()ing of the same pointer.\n"
                        "filename: %s\n"
                        "Line: %d\n",
                filename, line);
        return;
    }

    // no error occurs -> ready free node

    if (first_not_inuse > node)
        first_not_inuse = node;

    set_inUse(node, 0);

    if (node != start)
        setSecreteKey(node, 0);

    metadata *nextNode = (metadata *)((void *)node + sizeof(metadata) + getSize(node));
    availablefreenodes(prev, node, nextNode);
}

/*
 *   availablefreenodes
 *   @params:
 *          metadata* prev: curr's previous node
 *          metadata* curr: the current node to free
 *          metadata* next: curr's next node
 *   @returns: void
 *   @comments: Combines consecutive free nodes
 **/

void availablefreenodes(metadata *prev, metadata *curr, metadata *next)
{
    unsigned short neededToFree = getSize(curr);
    // If the present node and prev node are not in use
    if ((prev != curr) && get_inUse(prev) == 0)
    { // Coalesce left
        setSize(prev, getSize(prev) + neededToFree + sizeof(metadata));
        neededToFree += sizeof(metadata);
        setSize(curr, 0);

        if (curr == tail)
        {
            tail = prev;
            setSecreteKey(tail, SECRETEKEY);
            avaiMemory += neededToFree;
            return;
        }

        curr = prev;
    }

    // If the present node and next node are not in use
    if ((next <= tail) && get_inUse(next) == 0)
    { // Coalesce right
        setSize(curr, getSize(curr) + getSize(next) + sizeof(metadata));
        setSize(next, 0);
        neededToFree += sizeof(metadata);

        if (next == tail)
        {
            tail = curr;
            setSecreteKey(next, 0);
            setSecreteKey(tail, SECRETEKEY);
        }
        else
            neededToFree += getSize(next);
    }

    avaiMemory += neededToFree;
}

void printBinary(metadata *m)
{ // use this function to assure the bitwise functions work
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
{
    m->data = val == 0 ? m->data & ~(1 << 15) : m->data | (1 << 15);
};

boolean isAllocated(metadata *m)
{ //check secrete Key, if it is 7, the current address is a metadata
    return (m->data & SECRETEKEY) == SECRETEKEY ? True : False;
};

void setSecreteKey(metadata *m, int val)
{ //set to 0 or SECRETEKEY
    m->data = val == 0 ? m->data & ~(0x7 << 12) : m->data | SECRETEKEY;
};

unsigned short getSize(metadata *m)
{ //get the last 12 bits of data
    return m->data & GET_SIZE_NUM;
};

void setSize(metadata *m, unsigned short size)
{
    m->data = (m->data & SET_SIZE_NUM) | size;
};

boolean checkValidBlock(metadata *address, unsigned short size)
{
    return (!get_inUse(address) && getSize(address) >= size) ? True : False;
};
