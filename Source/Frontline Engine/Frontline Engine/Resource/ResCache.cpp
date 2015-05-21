#include "ResCache.h"
#include <tchar.h>
#include "DefaultResource.h"

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
	alloc = fastdelegate::MakeDelegate(this, &ResCache::Allocate);
	bool retValue = false;
	if (m_file->VOpen(m_file->GetFilename().c_str()))
	{
		RegisterLoader(std::shared_ptr<IResourceLoader>(FL_NEW DefaultResourceLoader()));
		retValue = true;
	}
	return retValue;
}

ResHandle ResCache::GetHandle(const char* ps_resname)
{
	ResHandle handle(m_resources[std::string(ps_resname)]);
	if (handle == NULL)
		handle = Load(ps_resname);
	else
		Update(handle);
	return handle;
}

ResHandle ResCache::Load(const char* ps_resname)
{
	std::shared_ptr<IResourceLoader> loader; // The resource loader
	ResHandle handle(NULL); // The pointer to the resource

	/**************** FIND THE CORRECT LOADER ****************/
	for (ResourceLoaders::iterator it = m_resourceLoaders.begin();
		it != m_resourceLoaders.end(); ++it)
	{
		std::shared_ptr<IResourceLoader> testLoader = *it;
		if (WildcardMatch(testLoader->VGetPattern().c_str(), ps_resname))
		{
			loader = testLoader;
			break;
		}
	}

	/**************** IF THE LOADER DOES NOT EXIST ****************/
	if (!loader)
	{
		FL_ASSERT(loader && _T(std::string("Default resource loader not found! Resource Name: ").append(ps_resname).c_str()));
		return handle; // Resource not loaded!
	}

	/**************** ALLOCATE SPACE FOR THE FILE BUFFER ****************/
	unsigned int rawSize = m_file->VGetRawResourceSize(ps_resname); // File size
	char *rawBuffer = loader->VUseRawFile() ? Allocate(rawSize) : FL_NEW char[rawSize]; // The raw file buffer

	/**************** IF THERE IS NOT ENOUGH MEMORY ****************/
	if (rawBuffer == NULL)
	{
		ERRLOG("ResCache", std::string("ResCache", "Out of memory in resource cache. Resource Name: ").append(ps_resname)); // Generate an error
		return ResHandle(); // Return a null resource handle
	}

	/**************** READ THE FILE ****************/
	m_file->VGetResource(&rawBuffer, ps_resname); // Read from IResourceFile
	char *buffer = NULL; // The resource memory buffer
	unsigned int size = 0; // The size in memory of the loaded resource

	/**************** RAW TEXT ****************/
	if (loader->VUseRawFile())
	{
		buffer = rawBuffer;
	}
	/**************** CUSTOM LOADER ****************/
	else
	{
		size = loader->VGetLoadedResourceSize(rawBuffer, rawSize); // The loaded size in memory

		/**************** NOT INITIALIZED ****************/
		if (rawBuffer == NULL)
		{
			ERRLOG("ResCache", "Out of memory in resource cache."); // Out of memory error
			return handle;
		}

		bool success = loader->VLoadResource(handle, rawBuffer, ps_resname, alloc); // Load the resource
		SAFE_DELETE_ARRAY(rawBuffer);

		/**************** NOT INITIALIZED ****************/
		if (!success)
		{
			ERRLOG("ResCache", "Out of memory in resource cache."); // Out of memory error
			return handle;
		}
	}

	/**************** INITIALIZED ****************/
	if (handle)
	{
		m_lru.push_front(handle);
		m_resources[ps_resname] = handle;
	}
	return handle;
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
	m_resources.erase(handle->GetName());
}