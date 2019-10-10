#include "mymalloc.h"

unsigned short secreteKey = 1792; // 111 0000 0000
// create a variable availabelSize to keep track of how much memory we have left
unsigned short avaiMemory = 4094;
// create a variable to flag the first time mymalloc is called
boolean isFirstCall = True;
// start holds the address of the virtual memory block
metadata *start;
metadata *end;
static char myblock[HEAPSIZE] = {0};

unsigned short get_inUse(metadata *m)
{ // get the first bit of data
  return (m->data & (1 << 15)) >> 15;
}

void set_inUse(metadata *m, unsigned short val)
{ // set the first bit of data
  m->data = val == 0 ? m->data & ~(1 << 15) : m->data | (1 << 15);
}

boolean isAllocated(metadata *m)
{
  //check secrete Key, if it is 7, the address is allocated
  return (m->data & secreteKey) == 7 ? True : False;
}

void setSecreteKey(metadata *m)
{
  m->data = m->data | secreteKey;
}

unsigned short getSize(metadata *m)
{ //get the last 12 bits of data
  return m->data & ~(0 << 11);
}

void setSize(metadata *m, unsigned short size)
{
  m->data = (m->data & (0 << 11)) | size;
}

void setup()
{ // set the first metadata to not in-use, 4094 bytes
  start = (metadata *)myblock;
  end = start;
  setSize(start, HEAPSIZE - sizeof(metadata));
  setSecreteKey(start);
}

boolean checkValidBlock(metadata *address, unsigned short size)
{
  if (!get_inUse(address) && getSize(address) >= size)
  {
    return True;
  }
  return False;
}

void updateSmallest(metadata *smallest, metadata *ptr)
{
  if (getSize(ptr) < getSize(smallest))
  {
    smallest = ptr;
  }
}
/*
*  MY MALLOC IMPLEMENTATION 
*/

void *mymalloc(size_t size, char *filename, int line)
{
  if (size == 0)
    return NULL;
  if (avaiMemory < size)
  {
    printf("Error: Not enough memory in heap.\n%hu bytes left in heap.\n", avaiMemory);
  }

  if (isFirstCall)
  {
    setup();
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
  while (ptr != end)
  {
    if (foundBlock)
    {
      updateSmallest(smallest, ptr);
      printf("update smallest");
    }
    foundBlock = checkValidBlock(ptr, size);
    printf("inside loop\n");
    ptr += getSize(ptr) + sizeof(ptr);
  }

  // check the tail
  foundBlock = checkValidBlock(ptr, size);
  if (foundBlock)
    updateSmallest(smallest, ptr);
  boolean isTail = smallest == end ? True : False;
  if (foundBlock)
  {
    avaiMemory -= size;
    result = smallest;
    // split ?
    if (getSize(result) > size + sizeof(result))
    {
      // update result metadata
      // set up next metadata
      metadata *nextMetadata = result + sizeof(result) + size;
      if (isTail)
      {
        end = nextMetadata;
      }
    }
  }

  if (result == NULL)
  {
    printf("Error: malloc failed. Cannot find a sufficient space.\n"
           "At file: %s\tLine:%d",
           filename, line);
    return NULL;
  }

  return result;
};

/*
*  MY FREE IMPLEMENTATION 
*/

int main(int argc, char const *argv[])
{

  /* code */
  struct meta
  {
    unsigned short a;
  };

  struct meta metadata;

  printf("%lu\n", sizeof(metadata));
  return 0;
}
