#include "Pool.h"
#include "..\FrontlineCommon.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline uint8_t alignForwardAdjustment(const void* address, uint8_t alignment)
{
	uint8_t adjustment = alignment - (reinterpret_cast<uintptr_t>(address) & static_cast<uintptr_t>(alignment - 1));

	if (adjustment == alignment)
		return 0; //already aligned

	return adjustment;
}

inline uint8_t alignForwardAdjustmentWithHeader(const void* address, uint8_t alignment, uint8_t headerSize)
{
	uint8_t adjustment = alignForwardAdjustment(address, alignment);

	uint8_t neededSpace = headerSize;

	if (adjustment < neededSpace)
	{
		neededSpace -= adjustment;

		//Increase adjustment to fit header
		adjustment += alignment * (neededSpace / alignment);

		if (neededSpace % alignment > 0)
			adjustment += alignment;
	}

	return adjustment;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Pool::Pool(int pi_KB, void* pp_begin)
	: mp_freeBlocks((FreeBlock*) pp_begin)
{
	if (pi_KB < 1)
	{
		ERRLOG("Pool", "Pool cannot have a size of zero.");
		return;
	}

	mp_begin = pp_begin;
	mi_numAllocations = 0;
	mi_size = pi_KB * 1024;
	mi_usedMem = 0;

	mp_freeBlocks->next = nullptr;
	mp_freeBlocks->size = pi_KB * 1024;
}

Pool::~Pool()
{
	if (mi_numAllocations != 0 || mi_usedMem != 0)
	{
		ERRLOG("Pool", "Not all memory has been deallocated.");
		return;
	}
	mp_freeBlocks = nullptr;
	memset(mp_begin, 0xcd, mi_size);
}

void* Pool::Alloc(size_t pi_size, uint8_t pi_alignment)
{
	FreeBlock* lp_currBlock = mp_freeBlocks, *lp_prevBlock = nullptr; // Current block in list, previous block
	while (lp_currBlock != nullptr) // Loop through the list
	{
		uint8_t adjustment = alignForwardAdjustmentWithHeader(lp_currBlock, pi_alignment, sizeof(AllocationHeader)); // Find the adjustment to byte - align the object

		size_t total_size = adjustment + pi_size; // The total amount of memory occupied by the object

		if (lp_currBlock->size < total_size) // If the current free block is not large enough
		{
			lp_prevBlock = lp_currBlock;
			lp_currBlock = lp_currBlock->next;
			continue;
		}
		if (lp_currBlock->size - total_size <= sizeof(AllocationHeader)) // If there is not enough space for another object
		{
			total_size = lp_currBlock->size; // Occupy the entire block
		}
		else
		{
			/// TODO order list from smallest to largest
			FreeBlock* next_block = (FreeBlock*) ((int) lp_currBlock + total_size); // Create a free block after the allocated space
			next_block->next = lp_currBlock->next; // Set up the new block
			next_block->size = lp_currBlock->size - total_size;

			if (lp_prevBlock != nullptr)
				lp_prevBlock->next = next_block; // Let the previous block know about the new one
			else
				mp_freeBlocks = next_block; // Set the first block to the new one
		}

		void* alignedAddress = lp_currBlock + adjustment; // Find the actual address of initialized data
		AllocationHeader* header = (AllocationHeader*) lp_currBlock; // Create the header
		header->adjustment = adjustment; // Set the header properties
		header->size = total_size;
		mi_usedMem += total_size; // Find the total memory used by the pool
		mi_numAllocations++; // Increment the number of allocations

		return (void*) alignedAddress; // Return the memory address
	}
}

int Pool::CombineFreeBlocks()
{
	FreeBlock* lp_currBlock = mp_freeBlocks;
	FreeBlock* lp_prevBlock = nullptr;
	while (lp_currBlock != nullptr)
	{
		FreeBlock* adjacent = lp_currBlock + lp_currBlock->size;
		if (adjacent == lp_currBlock->next)
		{
			lp_currBlock->next = adjacent->next;
			lp_currBlock->size += adjacent->size;
		}
		lp_prevBlock = lp_currBlock;
		lp_currBlock = lp_currBlock->next;
	}
	return 0;
}

void Pool::DeAlloc(void* pp_toDelete)
{
	if (pp_toDelete == nullptr)
	{
		ERRLOG("Pool", "Tried to delete NULL pointer.");
		return;
	}

	AllocationHeader* header = (AllocationHeader*) pp_toDelete - sizeof(AllocationHeader);

	FreeBlock* lp_currBlock = mp_freeBlocks;
	FreeBlock* lp_prevBlock = nullptr;
	size_t li_size = header->size;

	while (lp_currBlock != nullptr)
	{
		if (lp_currBlock > (void*) header)
			break;
		lp_prevBlock = lp_currBlock;
		lp_currBlock = lp_prevBlock->next;
	}

	FreeBlock* lp_newBlock = (FreeBlock*) pp_toDelete - header->adjustment;
	lp_newBlock->next = lp_currBlock;

	if (lp_prevBlock == nullptr)
	{
		mp_freeBlocks = lp_newBlock;
	}
	else if (lp_prevBlock + lp_prevBlock->size >= lp_newBlock)
	{
		lp_prevBlock->size += lp_newBlock->size;
		lp_prevBlock->next = lp_newBlock->next;
		lp_newBlock = lp_prevBlock;
	}
	else
	{
		lp_prevBlock->next = lp_newBlock;
	}

	FreeBlock* lp_nextBlock = lp_newBlock->next;
	if (lp_nextBlock != nullptr)
	{
		if (lp_newBlock + lp_newBlock->size >= lp_nextBlock)
		{
			lp_newBlock->size += lp_nextBlock->size;
			lp_newBlock->next = lp_nextBlock->next;
		}
	}

	mi_numAllocations--;
	mi_usedMem -= li_size;
}