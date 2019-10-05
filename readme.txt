-- documenting your design and workload data and findings --
I - INTRODUCTION
    In this project, we will write a C library which implements a better version of the system's 
    dynamic memory functions malloc() and free(). Dealing with dynamic memory is cumbersome because the system only output the 
    SEGMENTATION FAULT error without further explanation when users make mistake. It can create lots of frustrations
    for users since they have no insight about what is wrong with their applications. The new malloc() and free() 
    functions will not only handle the essensial functionality of the system malloc library, but will also detect and 
    friendly resolve common programming and usage errors occur when working with dynamic memory.

II - WHAT IS MALLOC AND FREE ?
    Malloc is a C function which allows users to allocate a block of memory from the 
    system memory heap. The data inside this block will persist until the block is freed by users.
    Malloc returns a void pointer that points to the first address of a block of memory of at least 
    the requested size. 

    The free function allows users to free a dynamically-allocated memory block, which is created by the 
    malloc function. Freeing malloced memory before terminating the application is crutial to avoid memory leaks 
    to occur. When there is enough memory leaks, the application can consume more memory than is physically available
    and can cause programs to crash.

IV - MYMALLOC() IMPLEMENTATION 

 mymalloc() implementation {
  --  returning a
      pointer to at least as many byte as asked for, and NULL if that is not possible
  --   keep track of how much and what other memory
      you have allocated before, but you can not require the user to hand you that information
  --  
 }

V - MYFREE() IMPLEMENTATION 

VI - METADATA EXPLANATION 
    Struct metadata only contains a single element:
        struct metadata {
            unsigned short data;
        }
    
    The data element of type short is used to store information about a memory block. The size of unsigned 
    short data is 2 bytes, equivalent to 16 bits. 

    We use the first bit of the data element to determine whether a memory block is in-use or not. If a memory block 
    is in-use, we set the first bit to 1, otherwise set it to 0.

    The next three bits contain a secrete number that determines whether an address is a valid address to 
    free or not. Indeed, the secrete number is set to 0x7 to indicate that the current memory block is created with 
    mymalloc, if the screte number found in a memory block does not match 0x7, we know that this memory block 
    is not created with mymalloc.
    
    Finally, the last 12 bits of data is the size of the requested memory block, it can be as big as
    1111 1111 1111 (0xFFF), which infers 4095 bytes. It is sufficient enough to store the largest size of a memory 
    block, which is 4094 bytes, since the first metadata consumes 2 bytes.

    Overall, the total size of struct metadata is 2 bytes. 







