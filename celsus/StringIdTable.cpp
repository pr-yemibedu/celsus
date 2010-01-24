//*** StringIdTable.cpp ***/
#include <stdafx.h>
#include <string.h>
#include <ctype.h>
#include "celsus.hpp"
#include "StringIdTable.hpp"


StringIdTable* StringIdTable::instance_ = NULL;


//*** GetInstance ***

StringIdTable& StringIdTable::instance()
{
  if (instance_ == NULL) {
    instance_ = new StringIdTable();
    atexit(close);
  }
  return *instance_;
}

void StringIdTable::close()
{
  SAFE_DELETE(instance_);
}

//*** Constructor ***

StringIdTable::StringIdTable():
stringStorageBlockCount_(0),
stringStorageBlockMaxCount_(8),
idStringTableSlots_(256),	///< Note - with this implementation of a hash table, using a prime number of slots gives no benefit over power-of-two number of slots
idStringTableItemCount_(0)
{
  // Allocate the array which stores the string block info
  stringStorageBlocks_=(StringStorageBlock*)malloc(stringStorageBlockMaxCount_*sizeof(StringStorageBlock));

  // Allocated the first string block
  stringStorageBlocks_[0].head=(char*)malloc(stringStorageBlockSize_);
  stringStorageBlocks_[0].tail=stringStorageBlocks_[0].head;
  stringStorageBlockCount_++; 

  // Allocate the hash table
  idStringTable_=(char**)malloc(sizeof(char*)*idStringTableSlots_);

  // Mark all the slots of the string table as unused
  for (int i=0; i<idStringTableSlots_; i++)
  {
    idStringTable_[i]=0;
  }

}

StringIdTable::~StringIdTable()
{
  // Free all the string storage blocks
  if (stringStorageBlocks_ != NULL) {
    for (int i=0; i<stringStorageBlockCount_; i++)
    {
      SAFE_FREE(stringStorageBlocks_[i].head);
    }
  }

  // Free the array holding the string storage block info
  SAFE_FREE(stringStorageBlocks_);
  SAFE_FREE(idStringTable_);
}


//*** CalculateHash ***

unsigned int StringIdTable::CalculateHash(const char* idString) const
{
  unsigned long hash = 5381; // Seed value

  // Modify hash for each character in the string
  const char* stringData=idString;
  while (*stringData)
  {
    // A little bit-manipulation magic to get a nice distribution of values
    hash = ((hash << 5) + hash) ^ toupper(*stringData);
    stringData++;
  }

  // Return the final hash value
  return hash;
}


//*** FindIdString ***

const char* StringIdTable::FindIdString(unsigned int hash, const char* idString)
{
  // Find slot for this string. This simply uses the passed in hash value, and modulates it by the number of slots. 
  // Note that as the number of slots is always a power-of-two-number, we can use a binary AND instead of modulo
  unsigned int slot=hash&(idStringTableSlots_-1);

  // Loop through all the entries until we find the one we are looking for or an empty slot
  char* entry=idStringTable_[slot];
  while(entry)
  {  
    // Is this the one we're looking for?
    unsigned int strhash=(*((unsigned int*)entry));
    if (strhash==hash && _stricmp(entry+4,idString)==0)
    {
      // Yes, so we just return the shared idString
      return entry+4; // Skip the first four bytes, as that's the hash number
    }

    // Not found the one we're looking for, so continue with the next entry 
    slot=(slot+1)&(idStringTableSlots_-1);
    entry=idStringTable_[slot];
  }

  // We didn't find an entry for that string, so we need to add it

  // If the table is more than two-thirds full, double its size and reinsert the strings
  if (idStringTableItemCount_>=(idStringTableSlots_-(idStringTableSlots_/3)))
  {
    // Make the new table twice the size
    int newidStringTableSlots=idStringTableSlots_*2;

    // Allocate memory for it
    char** newIdStringTable=(char**)malloc(sizeof(char*)*newidStringTableSlots);

    // And initialize all slots to unused
    for (int i=0; i<newidStringTableSlots; i++)
    {
      newIdStringTable[i]=0;
    }

    // Reinsert all the existing strings into the new table
    for (int i=0; i<idStringTableSlots_; i++)
    {
      // If slot is in use
      if (idStringTable_[i])
      {				
        // Get hash for string (stored as first four bytes of the string)
        unsigned int existinghash=(*((unsigned int*)idStringTable_[i]));

        // Calculate the slot
        int newslot=existinghash&(newidStringTableSlots-1);

        // Find the nearest unused slot
        while (newIdStringTable[newslot])
        {
          newslot=(newslot+1)&(newidStringTableSlots-1);
        }

        // Store the string in the new table
        newIdStringTable[newslot]=idStringTable_[i];
      }
    }

    // Free memory used by the old table
    free(idStringTable_);

    // And replace the old table with the new one
    idStringTableSlots_=newidStringTableSlots;
    idStringTable_=newIdStringTable;

    // Calculate the new slot for the string we're currently inserting
    slot=hash&(idStringTableSlots_-1);

    // And find the nearest empty slot
    while (idStringTable_[slot])
    {
      slot=(slot+1)&(idStringTableSlots_-1);
    }
  }

  // Create a duplicate of the string
  char* newEntry=StoreString(hash, idString); 

  // And store it in the table
  idStringTable_[slot]=newEntry;

  // Increase the total number of items stored
  idStringTableItemCount_++;

  // Return the shared id string
  return newEntry+4;	// Skip the first four bytes, as that's the hash number
}


//*** StoreString ***

char* StringIdTable::StoreString(unsigned int hash, const char* string)
{
  // Get the length of the string
  int length=(int)strlen(string);

  // Calculate space needed to store the string
  int spaceNeeded=length+1+4; // Add space for terminator (1) and for storing hash number (4)

  // Check if there's space in the current string storage block
  StringStorageBlock* block=&stringStorageBlocks_[stringStorageBlockCount_-1]; // Current block is always the last one
  if ((stringStorageBlockSize_-(block->tail-block->head))<spaceNeeded)
  {
    // No more room - make a new storage block
    if (stringStorageBlockCount_==stringStorageBlockMaxCount_)
    {
      // Need a bigger array for string storage blocks
      stringStorageBlockMaxCount_*=2; // Go with twice the current size

      // Use realloc to get a bigger array while preserving the values it already holds
      stringStorageBlocks_=(StringStorageBlock*)realloc(stringStorageBlocks_,stringStorageBlockMaxCount_*sizeof(StringStorageBlock));
    }

    // Allocate the new string storage block
    stringStorageBlocks_[stringStorageBlockCount_].head=(char*)malloc(stringStorageBlockSize_);
    stringStorageBlocks_[stringStorageBlockCount_].tail=stringStorageBlocks_[stringStorageBlockCount_].head; // Empty, so first free byte is at start

    // Update the current string storage block
    block=&stringStorageBlocks_[stringStorageBlockCount_];
    stringStorageBlockCount_++;
  }

  // Get the next piece of free memory from the current block
  char* strdest=block->tail;

  // Write hash number
  *((unsigned int*)strdest)=hash;

  // Copy the string to the block
#pragma warning(suppress: 4996)
  strcpy(strdest+4,string); // Offset by four to account for hash number

  // Mark space as used
  block->tail+=spaceNeeded; 

  // Return pointer to the copy
  return strdest;
}


