#include "ResCache.h"
#include <tchar.h>

ResCache::ResCache(const unsigned int sizeInMb, IResourceFile *resFile)
{
	m_cacheSize = sizeInMb * 1024 * 1024;
	m_allocated = 0;
	m_file = resFile;
}


ResCache::~ResCache()
{
	while (!m_lru.empty())
	{
		FreeOneResource();
	}
	SAFE_DELETE(m_file);
}

bool ResCache::Init()
{
	bool retValue = false;
	if (m_file->VOpen())
	{
		RegisterLoader(std::shared_ptr<IResourceLoader>(FL_NEW DefaultResourceLoader()));
		retValue = true;
	}
	return retValue;
}

ResHandle ResCache::GetHandle(Resource * r)
{
	ResHandle handle(Find(r));
	if (handle == NULL)
		handle = Load(r);
	else
		Update(handle);
	return handle;
}

ResHandle ResCache::Load(Resource *r)
{
	std::shared_ptr<IResourceLoader> loader;
	ResHandle handle;
	for (ResourceLoaders::iterator it = m_resourceLoaders.begin();
		it != m_resourceLoaders.end(); ++it)
	{
		std::shared_ptr<IResourceLoader> testLoader = *it;
		if (WildcardMatch(testLoader->VGetPattern().c_str(), r->m_name.c_str()))
		{
			loader = testLoader;
			break;
		}
	}
	if (!loader)
	{
		FL_ASSERT(loader && _T("Default resource loader not found!"));
		return handle; // Resource not loaded!
	}
	unsigned int rawSize = m_file->VGetRawResourceSize(*r);
	char *rawBuffer = loader->VUseRawFile() ?
		Allocate(rawSize) : FL_NEW char[rawSize];
	if (rawBuffer == NULL)
	{
		// resource cache out of memory
		return ResHandle();
	}
	m_file->VGetRawResource(*r, rawBuffer);
	char *buffer = NULL;
	unsigned int size = 0;
	if (loader->VUseRawFile())
	{
		buffer = rawBuffer;
		handle = ResHandle(r);
	}
	else
	{
		size = loader->VGetLoadedResourceSize(rawBuffer, rawSize);
		buffer = Allocate(size);
		if (rawBuffer == NULL || buffer == NULL)
		{
			// resource cache out of memory
			return ResHandle();
		}
		handle = ResHandle(r);
		bool success = loader->VLoadResource(rawBuffer, "").get(); // TODO fix name thing
		SAFE_DELETE_ARRAY(rawBuffer);
		if (!success)
		{
			// resource cache out of memory
			return ResHandle();
		}
	}
	if (handle)
	{
		m_lru.push_front(handle);
		m_resources[r->m_name] = handle;
	}
	FL_ASSERT(loader && _T("Default resource loader not found!"));
	return handle; // ResCache is out of memory!
}

char* ResCache::Allocate(unsigned int size)
{
	if (!MakeRoom(size))
		return NULL;
	char *mem = FL_NEW char[size];
	if (mem)
		m_allocated += size;
	return mem;
}

bool ResCache::MakeRoom(unsigned int size)
{
	if (size > m_cacheSize)
	{
		return false;
	}
	// return null if there’s no possible way to allocate the memory
	while (size > (m_cacheSize - m_allocated))
	{
		// The cache is empty, and there’s still not enough room.
		if (m_lru.empty())
			return false;
		FreeOneResource();
	}
	return true;
}

void ResCache::FreeOneResource()
{
	ResHandleList::iterator gonner = m_lru.end();
	gonner--;
	ResHandle handle = *gonner;
	m_lru.pop_back();
	m_resources.erase(handle->m_name);
}