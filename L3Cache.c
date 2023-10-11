#include "L3Cache.h"

uint8_t L1Cache[L1_SIZE]; // Represents the L1 cache memory.
uint8_t L2Cache[L2_SIZE]; // Represents the L2 cache memory
uint8_t DRAM[DRAM_SIZE]; // Represents the main memory (DRAM)
uint32_t time; // A global variable to keep track of time
Cache cache;

/**************** Time Manipulation ***************/
void resetTime() { time = 0; } // Resets the global time counter

uint32_t getTime() { return time; } // Returns the current time

/****************  RAM memory (byte addressable) ***************/
void accessDRAM(uint32_t address, uint8_t *data, uint32_t mode) {
  /* 
  Simulates access to main memory (DRAM) by taking a byte address, 
  a pointer to data, and a mode (read or write). It also updates the 
  global time variable based on the mode
  */

  if (address >= DRAM_SIZE - WORD_SIZE + 1)
    exit(-1);

  if (mode == MODE_READ) {
    memcpy(data, &(DRAM[address]), BLOCK_SIZE);
    time += DRAM_READ_TIME;
  }

  if (mode == MODE_WRITE) {
    memcpy(&(DRAM[address]), data, BLOCK_SIZE);
    time += DRAM_WRITE_TIME;
  }
}

void initCache() { cache.init = 0; } // Initializes the L1 cache

void accessL2(uint32_t address, uint8_t *data, uint32_t mode) {
  uint32_t index, Tag, offset;

  Tag = address / ((L2_SIZE / (BLOCK_SIZE * ASSOCIATIVITY_L2)) * BLOCK_SIZE);
  index = (address / BLOCK_SIZE) % (L2_SIZE / (BLOCK_SIZE * ASSOCIATIVITY_L2));
  offset = address % BLOCK_SIZE;

  int match = 0;

  for (int i = 0; i < ASSOCIATIVITY_L2; i++) {
    if (cache.L2.Lines[index][i].Valid && cache.L2.Lines[index][i].Tag == Tag) {
      switch (mode) {
        case MODE_READ:
          memcpy(data, &(cache.L2.Lines[index][i].Data[offset]), WORD_SIZE);
          time += L1_READ_TIME;
          break;
        case MODE_WRITE:
          memcpy(&(cache.L2.Lines[index][i].Data[offset]), data, WORD_SIZE);
          time += L1_WRITE_TIME;
          cache.L2.Lines[index][i].Dirty = 1;
          break;
      }
      cache.L2.Lines[index][i].Time = time;
      match = 1;
      break;
    }
  }

  if (match == 0) {
    int i = 0;
    while (i < ASSOCIATIVITY_L2 && cache.L2.Lines[index][i].Valid) {
      i++;
    }
    if (i == ASSOCIATIVITY_L2) {
      i = 0;
      unsigned int min = cache.L2.Lines[index][0].Time;
      for (int j = 1; j < ASSOCIATIVITY_L2; j++) {
        if (cache.L2.Lines[index][j].Time < min) {
          min = cache.L2.Lines[index][j].Time;
          i = j;
        }
      }
    }
    if (cache.L2.Lines[index][i].Dirty) {
      accessDRAM(cache.L2.Lines[index][i].Tag * (L2_SIZE / (BLOCK_SIZE * ASSOCIATIVITY_L2)) * BLOCK_SIZE + index * BLOCK_SIZE, cache.L2.Lines[index][i].Data, MODE_WRITE);
      cache.L2.Lines[index][i].Data[0] = 0;
      cache.L2.Lines[index][i].Data[WORD_SIZE] = 0;
    }

    accessDRAM(address - offset, cache.L2.Lines[index][i].Data, MODE_READ);

    cache.L2.Lines[index][i].Valid = 1;
    cache.L2.Lines[index][i].Dirty = 0;
    cache.L2.Lines[index][i].Tag = Tag;
    cache.L2.Lines[index][i].Time = time;

    if (mode == MODE_READ) {
      memcpy(data, &(cache.L2.Lines[index][i].Data), BLOCK_SIZE);
      time += L2_READ_TIME;
    }

  }
}

void accessL1(uint32_t address, uint8_t *data, uint32_t mode) {
  /*
  Simulates access to the L1 cache. It uses a 1-line directly mapped cache. 
  This function checks if the requested data is in the cache and handles cache 
  misses accordingly. It updates the global time variable based on the mode

  address : The byte address of the memory location being accessed
  data : A pointer to the data that will be read from or written to the cache
  mode : Indicates the access mode, either MODE_READ or MODE_WRITE
  */

  uint32_t index, Tag, offset;
  /*
  The index of the cache line (in this case, it's always 0 since it's a 1-line cache)
  The tag associated with the memory address, which helps identify if the data is in the cache
  The address of the cache block in memory
  */

  /* init cache */
  if (cache.init == 0) { // Checks if the cache is initialized and initializes it if not
    for (int i = 0; i < (L1_SIZE / BLOCK_SIZE); i++) {
      cache.L1.Lines[i].Valid = 0;
      cache.L1.Lines[i].Dirty = 0;
      cache.L1.Lines[i].Tag = 0;
      for (int j = 0; j < BLOCK_SIZE; j+=WORD_SIZE) {
        cache.L1.Lines[i].Data[j] = 0;
      }
    }

    for (int i = 0; i < (L2_SIZE / (BLOCK_SIZE * ASSOCIATIVITY_L2)); i++) {
      for (int j = 0; j < ASSOCIATIVITY_L2; j++) {
        cache.L2.Lines[i][j].Valid = 0;
        cache.L2.Lines[i][j].Dirty = 0;
        cache.L2.Lines[i][j].Tag = 0;
        for (int k = 0; k < BLOCK_SIZE; k+=WORD_SIZE) {
          cache.L2.Lines[i][j].Data[k] = 0;
        }
      }
    }

    cache.init = 1;
  }

  Tag = address / (L1_SIZE / BLOCK_SIZE); // Calculates the tag of the memory address by dividing the address by the size of the cache (L1_SIZE)
  index = (address / BLOCK_SIZE) % (L1_SIZE / BLOCK_SIZE); // Calculates the index of the cache line by dividing the address by the size of a block (BLOCK_SIZE) and then taking the remainder of that division when divided by the size of the cache (L1_SIZE)
  offset = address % BLOCK_SIZE; // Calculates the offset of the memory address by taking the remainder of the address when divided by the size of a block (BLOCK_SIZE)

  CacheLine *Line = &cache.L1.Lines[index]; // A pointer to the cache line within the cache

  /* access Cache*/

  /*
  When a cache miss occurs, it fetches the required data from main memory, updates the cache with the new data, 
  and handles any necessary write-backs of modified data to memory. Ultimately, it ensures that the cache contains 
  the correct data for subsequent access
  */

  if (!Line->Valid || Line->Tag != Tag) { // checks for a cache miss. True if so (no data in the cache at this line or tag mismatch meaning the data in cache line is not the data we need)
    // // accessDRAM(MemAddress, TempBlock, MODE_READ); // If there's a cache miss, this line of code simulates fetching a new block of data from main memory (DRAM) located at the address MemAddress and stores it in the TempBlock buffer
    // accessDRAM(address - offset, TempBlock, MODE_READ); // If there's a cache miss, this line of code simulates fetching a new block of data from main memory (DRAM) located at the address MemAddress and stores it in the TempBlock buffer

    // if ((Line->Valid) && (Line->Dirty)) { // A dirty cache line means that it contains modified data
    //   // If the cache line is both valid and dirty, it proceeds to write back the old block of data to memory
    //   accessDRAM((Line->Tag * (L1_SIZE / BLOCK_SIZE)) + (index * BLOCK_SIZE), Line->Data, MODE_WRITE);
    // }

    //  // This line of code copies the new block of data (TempBlock) obtained from DRAM into the cache (L1Cache)
    // memcpy(Line->Data, TempBlock, BLOCK_SIZE);
    // Line->Valid = 1; // Marks the cache line as valid, indicating that it now contains valid data
    // Line->Tag = Tag; // Updates the tag in the cache line to match the tag of the newly fetched data
    // Line->Dirty = 0; // Resets the "dirty" flag since the cache line now contains fresh data

    if ((Line->Valid) && (Line->Dirty)) {
      accessL2((Line->Tag * (L1_SIZE / BLOCK_SIZE)) * BLOCK_SIZE + index * BLOCK_SIZE, Line->Data, MODE_WRITE);
      Line->Data[0] = 0; // reset data
      Line->Data[WORD_SIZE] = 0; // reset data
    }

    accessL2(address - offset, Line->Data, MODE_READ);

    switch (mode) {
      case MODE_READ:
        memcpy(data, &(Line->Data[offset]), WORD_SIZE);
        time += L1_READ_TIME;
        Line->Dirty = 0;
        break;
      case MODE_WRITE:
        memcpy(&(Line->Data[offset]), data, WORD_SIZE);
        time += L1_WRITE_TIME;
        Line->Dirty = 1;
        break;
    }

    Line->Valid = 1;
    Line->Tag = Tag;    

  } else {
    switch (mode) {
      case MODE_READ:
        memcpy(data, &(Line->Data[offset]), WORD_SIZE);
        time += L1_READ_TIME;
        break;
      case MODE_WRITE:
        memcpy(&(Line->Data[offset]), data, WORD_SIZE);
        time += L1_WRITE_TIME;
        Line->Dirty = 1;
        break;
    }
  }
}

void read(uint32_t address, uint8_t *data) { // Calls accessL1 to perform a read operation from the cache
  accessL1(address, data, MODE_READ);
}

void write(uint32_t address, uint8_t *data) { // Calls accessL1 to perform a write operation to the cache
  accessL1(address, data, MODE_WRITE);
}
