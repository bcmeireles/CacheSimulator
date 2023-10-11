#ifndef L3CACHE_H
#define L3CACHE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "Cache.h"

#define ASSOCIATIVITY_L2 2

void resetTime();

uint32_t getTime();

/****************  RAM memory (byte addressable) ***************/
void accessDRAM(uint32_t, uint8_t *, uint32_t);

/*********************** Cache *************************/

void initCache();
void accessL1(uint32_t, uint8_t *, uint32_t);
void accessL2(uint32_t, uint8_t *, uint32_t);

typedef struct CacheLine {
  uint8_t Valid;
  uint8_t Dirty;
  uint32_t Tag;
  uint8_t Data[BLOCK_SIZE];
  uint8_t Time;
} CacheLine;

typedef struct CacheL1 {
  uint32_t init;
  CacheLine Lines[L1_SIZE/BLOCK_SIZE];
} CacheL1;

typedef struct CacheL2 {
  uint32_t init;
  CacheLine Lines[L2_SIZE/(BLOCK_SIZE*ASSOCIATIVITY_L2)][ASSOCIATIVITY_L2];
} CacheL2;

typedef struct Cache {
  uint32_t init;
  CacheL1 L1;
  CacheL2 L2;
} Cache;

/*********************** Interfaces *************************/

void read(uint32_t, uint8_t *);

void write(uint32_t, uint8_t *);

#endif
