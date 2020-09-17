/******************************************************************************/
  /*!
  \par        Project Octane
  \file       MemoryManager.h
  \author     Brayan Lopez
  \date       09/04/2020
  \brief      Declaration of Memory Manager

  Copyright � 2020 DigiPen, All rights reserved.
  */
  /******************************************************************************/
#pragma once

namespace Octane
{

typedef unsigned char byte;

struct Block
{
  Block* next; //next block in linked list
};

//memory manager's pool
//this information will be stored in the first
//sizeof(Pool) bytes of every pool
struct Pool
{
  Pool * next;        //next pool
  Block* freeList;    //list of free blocks
  byte* nextAlloc;    //start of next allocation
  byte* endAlloc;     //end of next alloc
  size_t pages;       //total pages belonging to pool
  size_t resPages;    //reserved/uncommitted pages
  size_t commPages;   //committed pages
  size_t blockSize;   //size of blocks
  unsigned alignBytes;//bytes after pool header to align all blocks
  unsigned allocAmt;  //amount to allocate per NewPage() call
};

class MemoryManager
{
public:  
  MemoryManager(); //CTOR
  ~MemoryManager(); //DTOR
  void* New(size_t size);     //allocates a block of memory
  void Delete(void* address); //deletes a block of memory
private:
  //makes free blocks out of committed pages
  void InitPage(Pool* pool);
  //commits the next Pool::allocAmt bytes to make free blocks
  void NewPage(Pool* pool);
  //makes a new pool and adds it to pool list
  //pools are automatically sorted by blockSize (by inserting into right spot)
  //specifying original will make a new pool that is a copy of original
  //prev is used for keeping the pool list sorted
  //prev is only looked at if original is specified
  //size is required to be specified if original isnt
  Pool* NewPool(Pool* original, Pool* prev, unsigned size = 0);
  Pool* pools;        //linked list of pools
  size_t smallBlock = 256;  //smallest block size
  size_t bigBlock = 16 * 1024;    //biggest block size
  size_t poolBlockAmt = 1024;//blocks per pool
  size_t alignment = 16; //alignment of blocks in pools
};

}

