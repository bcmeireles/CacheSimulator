#ifndef CACHE_H
#define CACHE_H

#define WORD_SIZE 4   // in bytes, i.e 32 bit words // The size of a word in bytes, typically set to 4 bytes (32-bit words)
#define BLOCK_SIZE (16 * WORD_SIZE)    // in bytes // The size of a cache block in bytes. It's calculated as 16 times the size of a word
#define DRAM_SIZE (1024 * BLOCK_SIZE) // in bytes // The size of the main memory (DRAM) in bytes. It's calculated as 1024 times the BLOCK_SIZE
#define L1_SIZE (256 * BLOCK_SIZE)   // in bytes // The size of the L1 cache in bytes. It's calculated as 256 times the BLOCK_SIZE
#define L2_SIZE (512 * BLOCK_SIZE)  // in bytes // The size of the L2 cache in bytes. It's calculated as 512 times the BLOCK_SIZE

#define MODE_READ 1
#define MODE_WRITE 0

#define DRAM_READ_TIME 100 // The time it takes to read data from main memory (DRAM) in some unit of time (e.g., cycles)
#define DRAM_WRITE_TIME 50 // The time it takes to write data to main memory (DRAM) in some unit of time
#define L2_READ_TIME 10 // The time it takes to read data from the L2 cache in some unit of time
#define L2_WRITE_TIME 5 // The time it takes to write data to the L2 cache in some unit of time
#define L1_READ_TIME 1 // The time it takes to read data from the L1 cache in some unit of time
#define L1_WRITE_TIME 1 // The time it takes to write data to the L1 cache in some unit of time

#endif
