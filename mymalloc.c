#include <stdlib.h>
#include <stdio.h>
// your malloc function implementations
int main(int argc, char const *argv[]) {
  /* code */
  struct meta {
      short inuse;
      int size;
      short magicnum;
      struct meta * next;
      struct meta * prev;
      // unsigned short a;
  };

  struct meta metadata;

  printf("%lu\n", sizeof(metadata));
  return 0;
}
