/******************************************************************************/
	/*!
	\par        Project Octane
	\file       MemoryManager.h
	\author     Brayan Lopez
	\date       09/04/2020
	\brief      Implementation of Memory Manager

	Copyright � 2020 DigiPen, All rights reserved.
	*/
	/******************************************************************************/
#include "MemoryManager.h"

#include <Windows.h>

#ifdef _DEBUG
	#include <iostream>
#endif

#define RCAST(x, type) reinterpret_cast<type>(x) //shorthand reinterpret_cast

namespace Octane
{

	//generic add to front of linked list
	template <typename T>
	T* PushFront(T* item, T** list);

	//generic pop front of linked list
	template <typename T>
	T* PopFront(T** list);

	//pushes to after prev
	template<typename T>
	T* PushAfter(T* item, T** prev);


	// kilobytes operator to make dealing with page sizes easier
	constexpr size_t operator""k(size_t n)
	{
		return n * 1024;
	}

	// gigabytes operator to make dealing with page sizes easier
	constexpr std::size_t operator""G(std::size_t n)
	{
		return n * 1024 * 1024 * 1024;
	}

	//pushes to front, returns new front
	template<typename T>
	T* PushFront(T* item, T** list)
	{
		if (item) //if item exists
		{
			item->next = *list;
			*list = item;
		}

		return *list;
	}

	//pushes to after prev
	//returns item
	template<typename T>
	T* PushAfter(T* item, T** prev)
	{
		if (*prev) //if prev exists
		{
			(*prev)->next = item;
		}
		return item;
	}

	//pops the front and returns it
	template<typename T>
	T* PopFront(T** list)
	{
		Block* block = *list;
		if (block)
		{
			*list = block->next;
		}
		return block;
	}

	//pops an item and returns the item
	//else, returns nullptr
	template <typename T>
	T* Pop(T* item, T** list)
	{
		T* prev = nullptr;
		T* temp = *list;
		while (temp)
		{
			if (temp == item) //if we find it
			{
				if (prev) //connect prev with next
				{
					prev->next = temp->next;
				}
				else
				{
					*list = item->next;
				}
				return item;
			}
			temp = temp->next; //traverse
		}
		return nullptr; //should never happen
	}

MemoryManager::MemoryManager()
{
	for (size_t currBlocksize = bigBlock; currBlocksize >= smallBlock; currBlocksize /= 2)
	{
		//make new page and add it to page linked list
		PushFront(NewPool(nullptr, nullptr, currBlocksize),&pools);
	}
}

MemoryManager::~MemoryManager()
{
	for(Pool* pool = pools; pool != nullptr;)
	{
		Pool* next = pool->next;
		//free the allocated pools
		VirtualFree(pool, 0, MEM_RELEASE);
		pool = next;
	}
}

void* MemoryManager::New(size_t size)
{
	Pool* prev = nullptr;
	Pool* pool = nullptr;
	//info: pools is sorted by increasing block size
	//finds first pool with big enough blocks
	for (pool = pools; pool != nullptr;)
	{
		if (pool->blockSize >= size)
		{
			if (pool->resPages && !pool->freeList)
			{
				NewPage(pool);
			}
			if (pool->freeList)
			{
				//remove block from free list, put on use list
				//and return it
				return PopFront(&pool->freeList);
			}
			else
			{
				Pool* newPool = NewPool(pool, prev);
				if (newPool)
				{
					pool = newPool;
					break;
				}
			}
		}
		prev = pool;
		pool = pool->next;
	}

	if (pool)
	{
		return PopFront(&pool->freeList);
	}
	//allocate a new pool with blocks big enough
	pool = PushAfter(NewPool(nullptr, nullptr, size), &prev);

	if (pool)
		return PopFront(&pool->freeList);
	return nullptr;
#ifdef _DEBUG
	std::cout << "Octane::MemoryManager::New failed to allocate " << size << " bytes"<<std::endl;
#endif
}

void MemoryManager::Delete(void* address)
{
	for (Pool* pool = pools; pool != nullptr; pool = pool->next)
	{
		//first and last bytes of pool
		const byte* beginning = RCAST(pool, byte*);
		const byte* end = beginning + pool->pages * 4k;
		//check if address lies within this range
		if (address < end && address > beginning)
		{
			PushFront(RCAST(address,Block*), &pool->freeList);
			return;
		}
	}
#ifdef _DEBUG
	if(address)
		throw;//delete didnt find what it was looking for
#endif
}

void MemoryManager::InitPage(Pool* pool)
{
	byte* allocStart = pool->nextAlloc;
	// //end of allocation range, usually 4k
	//adjust ptr in page if this is the page
	//in the pool with the pool header
	if (allocStart == RCAST(pool, byte*))
	{
		allocStart += sizeof(Pool);
		allocStart += pool->alignBytes;
	}
	//byte* lastAlloc = allocStart;
	while (allocStart + pool->blockSize <= pool->endAlloc)
	{
		//make a new block 
		Block* block = RCAST(allocStart, Block*);
		//add block to free list
		PushFront(block, &pool->freeList);
		//traverse ptr
		allocStart += pool->blockSize;
	}

	//update reserved and committed pages
	const unsigned pagesAffected = (pool->endAlloc-pool->nextAlloc) / 4k;
	pool->commPages += pagesAffected;
	pool->resPages -= pagesAffected;
	pool->nextAlloc = allocStart;
	pool->endAlloc = allocStart + pool->allocAmt;
}

void MemoryManager::NewPage(Pool* pool)
{
	//allocate next area of pool
	VirtualAlloc(pool->nextAlloc, pool->allocAmt, MEM_COMMIT, PAGE_READWRITE);
	InitPage(pool); //init page, add blocks to free list
}

Pool* MemoryManager::NewPool(Pool* original, Pool* prev, unsigned blockSize)
{
  //make sure all blocks are size 2^x
	blockSize = 1 << static_cast<int>(ceil(log2(blockSize)));
	if (original)
		blockSize = original->blockSize;

	//amount of alignment needed after pool header so all blocks
	//are properly aligned
	const size_t alignNeeded = sizeof(Pool) % alignment;
	//total amount of space that will be used up
	size_t totalSize = blockSize * poolBlockAmt + sizeof(Pool) + alignNeeded;
	//amount of pages this pool will get
	size_t pages = totalSize / 4k + !!(totalSize % 4k);
	//total amount of space allocated
	size_t totalPageSize = 4k * pages;
	//amount of space to allocate when there are no free blocks
	const unsigned allocAmount = max(4k, blockSize);

  //allocate pool of blocks
	Pool* pool = RCAST(
		VirtualAlloc(nullptr,
								  totalPageSize,
								 MEM_RESERVE, PAGE_NOACCESS),
		                 Pool*);

	if (pool)
	{
		const unsigned firstAlloc = allocAmount + 4k;
		//commit first page to be able to make page header
		//and make first blocks
		VirtualAlloc(pool, firstAlloc,
			MEM_COMMIT, PAGE_READWRITE);
	
		//res pages and comm pages are inited in InitPage()
		pool->blockSize = blockSize;
		pool->resPages = pages;
		pool->alignBytes = alignNeeded;
		pool->pages = pages;
		pool->nextAlloc = RCAST(pool, byte*);
		pool->allocAmt = allocAmount;
		pool->endAlloc = pool->nextAlloc + firstAlloc;
		//put this pool with free list
		//before pool this is a copy of
		//in pools list
    PushFront(pool, &original);
		if (prev) //if there was a pool before original
		{
			prev->next = pool;
		}
		//init first page
		InitPage(pool);
	}

	return pool;
}

}//namespace Octane
