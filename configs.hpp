//---------------------------------------------------------------------------
// Modify the following configs to solve a game
//---------------------------------------------------------------------------
#ifndef CONFIGS_H
#define CONFIGS_H

#include "memory_manager.hpp"


/* board size */
const int N_ROWS = 5;
const int N_COLS = 5;


/* params for transposition (hashing) table */
const unsigned int IDX_BITS = 30;   // num bits: index
const unsigned int CODE_BITS = 10;  // num bits: validation code
const unsigned int ENTRY_SIZE = 2;  // num bytes: of an entry in table


/* which memory manager to use? */
typedef DefaultMemoryManager    MemoryManager;
// typedef CustomMemoryManager     MemoryManager;



#endif
