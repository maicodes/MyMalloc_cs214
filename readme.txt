-- documenting your design and workload data and findings --

// use calloc to set up static char block[4096]

metadata {
    /*  -- total size: 2 bytes
        -- the first bit is to determine a memory block is in-use or not.
           If it is in-use, set the first bit to 1, else set to 0
        -- the next three bits is a secrete number, it determines whether an
           address is a valid address to free or not.
        -- the last 12 bits is the size of a memory block, it can be as big as
           1111 1111 1111 (0xFFF), which is 4095. It is sufficient enough to store the largest
           size of a memory block, which is 4094, since a metadata has size 2
    */
    unsigned short data;
}
