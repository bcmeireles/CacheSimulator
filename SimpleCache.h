#ifndef SIMPLECACHE_H
#define SIMPLECACHE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "Cache.h"

void resetTime(); // Resets the time counter

uint32_t getTime(); // Returns the current time

/****************  RAM memory (byte addressable) ***************/
void accessDRAM(uint32_t, uint8_t *, uint32_t); // Simulates access to the main memory (DRAM) by taking a byte address, a pointer to data, and a mode (read or write)

/*********************** Cache *************************/

void initCache(); // initializes the cache
void accessL1(uint32_t, uint8_t *, uint32_t); // Simulates access to the L1 cache by taking a byte address, a pointer to data, and a mode (read or write)

typedef struct CacheLine { // Simulates access to the L1 cache by taking a byte address, a pointer to data, and a mode (read or write)
  uint8_t Valid;
  uint8_t Dirty;
  uint32_t Tag;
} CacheLine;

typedef struct Cache { // Simulates access to the L1 cache by taking a byte address, a pointer to data, and a mode (read or write)
  uint32_t init;
  CacheLine line;
} Cache;

/*********************** Interfaces *************************/

void read(uint32_t, uint8_t *); // Simulates a read operation from the cache by taking a byte address and a pointer to store the read data

void write(uint32_t, uint8_t *); // Simulates a write operation to the cache by taking a byte address and the data to be written

#endif
