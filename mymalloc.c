#include <stdlib.h>
#include <stdio.h>
#include "mymalloc.h"
//define secret key
unsigned short secreteKey = 0x7000; // 111 0000 0000 0000
unsigned short specialNum = 0xf000;
unsigned short fff = 0xfff; // 1111 1111 1111
// create a variable availabelSize to keep track of how much memory we have left
unsigned short avaiMemory = 4094;
// create a variable to flag the first time mymalloc is called
boolean isFirstCall = True;

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
  return (m->data & (1<<15)) >> 15;
};

void set_inUse(metadata *m, unsigned short val)
{ // set the first bit of data
  m->data = val == 0 ? m->data & ~(1 << 15) : m->data | (1 << 15);
  //printf("Set inuse to %hu", val);
  if ( val == 1 ){
    printBinary(m);
  }
};

boolean isAllocated(metadata *m)
{
  //check secrete Key, if it is 7, the address is allocated by mymalloc
  return (m->data & secreteKey) == 7 ? True : False;
};

void setSecreteKey(metadata *m, int val) // val is either 0 or secreteKey
{
  if (val == 0)
  { // set the 9th, 10th, 11th bits to 0
    m->data = m->data & ~(0x7 << 12);
  }
  else
  {
    m->data = m->data | secreteKey;
  }
  //printf("Set secrete key to %d", val);
  //printBinary(m);
};

unsigned short getSize(metadata *m)
{ //get the last 12 bits of data
  return m->data & fff;
};

void setSize(metadata *m, unsigned short size)
{
  m->data = (m->data & specialNum) | size;
  //printf("Set size to %hu", size);
  //printBinary(m);
};

boolean checkValidBlock(metadata *m, unsigned short size)
{
  short inUse = get_inUse(m);
  short mSize = getSize(m);
  printf("inUse: %hu, size: %hu, input Size: %hu\n", inUse, mSize, size);
  if (inUse == 0 && mSize >= size)
  {
    return True;
  }
  return False;
};

// void updateSmallest(metadata **smallest, metadata **ptr)
// {
//   if ( *smallest == NULL || getSize(*ptr) < getSize(*smallest)){
//     printf("update smallest\n");
//     *smallest = *ptr;
//   }
// };
/*
*  MY MALLOC IMPLEMENTATION
*/

// start holds the address of the virtual memory block
static char myblock[HEAPSIZE];
static metadata *start = (metadata *)myblock;
static metadata *tail = (metadata *)myblock;

void *mymalloc(size_t inputSize, char *filename, int line)
{
  unsigned short size = inputSize;
  if (size == 0)
    return NULL;
  if (avaiMemory < size)
  {
    printf("Error: Not enough memory in heap.\n%hu bytes left in heap.\n", avaiMemory);
  }

  if (isFirstCall)
  {
    //setup;
    set_inUse(start, 0);
    setSecreteKey(start, secreteKey);
    setSize(start, HEAPSIZE - sizeof(metadata));
    printf("start Size: %hu\n", getSize(start));
    isFirstCall = False;
  }

  void *result = NULL;

  /*traverse through char array
    check not in-use and sufficient size metadata
    if not, keep traversing until meet the last metadata
    if found, can update the address of smallest possible block?
    if split the tail -> update tail
  */
  metadata *ptr = start;
  metadata *smallest = NULL; 
  boolean foundBlock = False;

  // find metadata that is not the tail
  while (ptr != tail)
  {
    printf("inside while loop\n");
    if (foundBlock && (smallest == NULL || getSize(ptr) < getSize(smallest)))
    {
      printf("here\n");
      smallest = ptr;
    }
    unsigned short block_size = getSize(ptr);
    foundBlock = checkValidBlock(ptr, size);
    ptr = (metadata *) (ptr + sizeof(metadata) + block_size);
    printf("end 1 loop\n");
  }

  //check the tail
  printf("tail: %x ", tail);
  printf("ptr: %x\n", ptr);
  foundBlock = checkValidBlock(ptr, size);
  boolean isTail = False;
  if (foundBlock) {
    if (smallest == NULL || getSize(ptr) < getSize(smallest))
    {
      smallest = ptr;
      printf("smallest: %x\n", smallest);
    }
    isTail = smallest == tail ? True : False;
  }
    if (isTail)
  {
    printf("smallest is tail\n");
  }
  else
  {
    printf("smallest is not tail\n");
  }
  // handling mymalloc starts from here
  /*
  Check list:
  - set in-use to 1
  - set new size
  if there is a split:
  - create another metadata
  - set new size
  - set secret Key
*/
  if (foundBlock)
  {
    // smallest is the current metadata we're at
    // set inuse to 1
    set_inUse(smallest, 1);
    printf("smallest inuse: %hu", get_inUse(smallest) );
    // split ?
    if (getSize(smallest) > size + sizeof(metadata))
    {
      /* because we split smallest into 2 smaller metadata, smallest will have
      size = requested size, and the other metadata has the rest
      */
      // set up next metadata
      metadata *nextMetadata = smallest + sizeof(metadata) + size;
      avaiMemory -= sizeof(metadata);
      set_inUse(nextMetadata, 0);
      setSecreteKey(nextMetadata, secreteKey);
      setSize(nextMetadata, getSize(smallest) - size - sizeof(metadata));
      printf("size of nextMetadat %hu\n", getSize(nextMetadata));
      setSize(smallest, size);
      printf("new size of smallest %hu\n", getSize(smallest));
      if (isTail)
      {
        tail = nextMetadata;
      }
    }
    // result is the first byte of the block of memory right after smallest
    result = smallest + sizeof(metadata);
    avaiMemory -= getSize(smallest);
    printf("avai memory: %hu\n", avaiMemory);
  }
  else
  {
    printf("Error: malloc failed. Cannot find a sufficient space.\n"
           "At file: %s\tLine:%d\n",
           filename, line);
  }
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
