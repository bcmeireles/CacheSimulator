#include "L1Cache.h"

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

/*********************** L1 cache *************************/

void initCache() { cache.init = 0; } // Initializes the L1 cache

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
  uint8_t TempBlock[BLOCK_SIZE]; // A temporary buffer to hold a block of data from memory.

  /* init cache */
  if (cache.init == 0) { // Checks if the cache is initialized and initializes it if not
    for (int i = 0; i < (L1_SIZE / BLOCK_SIZE); i++) {
      cache.Lines[i].Valid = 0;
      cache.Lines[i].Dirty = 0;
      cache.Lines[i].Tag = 0;
      for (int j = 0; j < BLOCK_SIZE; j+=WORD_SIZE) {
        cache.Lines[i].Data[j] = 0;
      }
    }
    cache.init = 1;
  }

  Tag = address / ((L1_SIZE / BLOCK_SIZE) * BLOCK_SIZE); // Calculates the tag of the memory address by dividing the address by the product of the size of the cache (L1_SIZE) and the size of a block (BLOCK_SIZE)
  index = (address / BLOCK_SIZE) % (L1_SIZE / BLOCK_SIZE); // Calculates the index of the cache line by dividing the address by the size of a block (BLOCK_SIZE) and then taking the remainder of that division when divided by the size of the cache (L1_SIZE)
  offset = address % BLOCK_SIZE; // Calculates the offset of the memory address by taking the remainder of the address when divided by the size of a block (BLOCK_SIZE)



  CacheLine *Line = &cache.Lines[index]; // A pointer to the cache line within the cache

  /* access Cache*/

  /*
  When a cache miss occurs, it fetches the required data from main memory, updates the cache with the new data, 
  and handles any necessary write-backs of modified data to memory. Ultimately, it ensures that the cache contains 
  the correct data for subsequent access
  */

  if (!Line->Valid || Line->Tag != Tag) { // checks for a cache miss. True if so (no data in the cache at this line or tag mismatch meaning the data in cache line is not the data we need)
    // accessDRAM(MemAddress, TempBlock, MODE_READ); // If there's a cache miss, this line of code simulates fetching a new block of data from main memory (DRAM) located at the address MemAddress and stores it in the TempBlock buffer
    accessDRAM(address - offset, TempBlock, MODE_READ); // If there's a cache miss, this line of code simulates fetching a new block of data from main memory (DRAM) located at the address MemAddress and stores it in the TempBlock buffer

    if ((Line->Valid) && (Line->Dirty)) { // A dirty cache line means that it contains modified data
      // If the cache line is both valid and dirty, it proceeds to write back the old block of data to memory
      accessDRAM((Line->Tag * (L1_SIZE / BLOCK_SIZE)) + (index * BLOCK_SIZE), Line->Data, MODE_WRITE);
    }

     // This line of code copies the new block of data (TempBlock) obtained from DRAM into the cache (L1Cache)
    memcpy(Line->Data, TempBlock, BLOCK_SIZE);
    Line->Valid = 1; // Marks the cache line as valid, indicating that it now contains valid data
    Line->Tag = Tag; // Updates the tag in the cache line to match the tag of the newly fetched data
    Line->Dirty = 0; // Resets the "dirty" flag since the cache line now contains fresh data
  }

  if (mode == MODE_READ) {    // read data from cache line
    if (0 == (address % 8)) { // even word on block
      memcpy(data, &(Line->Data[0]), WORD_SIZE);
    } else { // odd word on block
      memcpy(data, &(Line->Data[WORD_SIZE]), WORD_SIZE);
    }
    time += L1_READ_TIME;
  }

  if (mode == MODE_WRITE) { // write data from cache line
    if (!(address % 8)) {   // even word on block
      memcpy(&(Line->Data[0]), data, WORD_SIZE);
    } else { // odd word on block
      memcpy(&(Line->Data[WORD_SIZE]), data, WORD_SIZE);
    }
    time += L1_WRITE_TIME;
    Line->Dirty = 1;
  }
}

void read(uint32_t address, uint8_t *data) { // Calls accessL1 to perform a read operation from the cache
  accessL1(address, data, MODE_READ);
}

void write(uint32_t address, uint8_t *data) { // Calls accessL1 to perform a write operation to the cache
  accessL1(address, data, MODE_WRITE);
}
