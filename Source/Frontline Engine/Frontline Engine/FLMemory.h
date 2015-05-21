#pragma once
#include "System\Pool.h"
typedef void functype(void*);
template<typename T>
class FLStrongPtr
{
	int* mp_refCount;
	T* mp_object;
	Pool* mp_pool;
public:
	int GetRefCount() { return mp_refCount; }
	FLStrongPtr(T* pp_rawPtr, Pool* pool)
	{
		mp_refCount = new int();
		*mp_refCount = 1;
		mp_pool = pool;
	}
	FLStrongPtr(FLStrongPtr<T>& copy)
	{
		mp_refCount = copy->GetRefCount();
		mp_pool = copy->mp_pool;
	}
	~FLStrongPtr();
};