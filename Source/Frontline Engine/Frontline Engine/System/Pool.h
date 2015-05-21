#pragma once
#include <list>
#include <cstdint>

class Pool
{
	struct AllocationHeader
	{
		size_t size;
		uint8_t adjustment;
	};
	struct FreeBlock
	{
		size_t size;
		FreeBlock* next;
	};

	void* mp_begin;
	size_t mi_size;
	size_t mi_usedMem;
	size_t mi_numAllocations;

	Pool(const Pool&);
	Pool& operator=(const Pool&);

	FreeBlock* mp_freeBlocks;
public:
	Pool(int pi_KB, void* pp_begin);
	~Pool();

	void* Alloc(size_t pi_size, uint8_t pi_alignment);

	void DeAlloc(void* pp_toDelete);

	int CombineFreeBlocks();
	
	template <typename T>
	T* create()
	{
		T* lp_retval = (T*) (alloc->Alloc(sizeof(T), 4));
		new(test) TestGame();
	}
};

template<class T> void destroy(T* pt, Pool* a) //<-you can't do this with 'delete'
{
	if (pt) {
		pt->~T();        // run the destructor
		a->DeAlloc(pt);   // deallocate the object
		pt = NULL; // nulls the pointer on the caller side.
	}
}

#define FL_POOL_NEW(pool, t) new(pool->Alloc(sizeof(t), 4))
#define FL_POOL_DELETE destroy