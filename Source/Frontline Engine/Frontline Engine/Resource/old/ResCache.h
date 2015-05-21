#pragma once
#include "..\FrontlineCommon.h"
#include "IResourceFile.h"
#include "IResourceLoader.h"
#include "DefaultResourceLoader.h"

typedef std::list<ResHandle> ResHandleList;
typedef std::map<std::string, ResHandle> ResHandleMap;
typedef std::list< std::shared_ptr < IResourceLoader > > ResourceLoaders;
class ResCache
{
protected:
	ResHandleList m_lru; // LRU (least recently used) list
	ResHandleMap m_resources; // STL map for fast resource lookup
	ResourceLoaders m_resourceLoaders;
	IResourceFile *m_file; // Object that implements IResourceFile
	unsigned int m_cacheSize; // total memory size
	unsigned int m_allocated; // total memory allocated
	ResHandle Find(Resource * r);
	const void *Update(ResHandle handle);
	ResHandle Load(Resource * r);
	void Free(ResHandle gonner);
	bool MakeRoom(unsigned int size);
	char *Allocate(unsigned int size);
	void FreeOneResource();
	void MemoryHasBeenFreed(unsigned int size);
public:
	ResCache(const unsigned int sizeInMb, IResourceFile *resFile);
	~ResCache();
	bool Init();
	void RegisterLoader(std::shared_ptr<IResourceLoader> loader);
	ResHandle GetHandle(Resource * r);
	int Preload(const std::string pattern, void(*progressCallback)(int, bool &));
	void Flush(void);
};