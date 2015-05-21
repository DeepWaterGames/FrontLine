#include "ZipResourceFile.h"

bool ZipResourceFile::VOpen(const wchar_t* ps_filename)
{
	mp_ZipFile = std::unique_ptr<ZipFile>(FL_NEW ZipFile());
	if (mp_ZipFile)
	{
		return mp_ZipFile->Init(ps_filename);
	}
	return false;
}

bool ZipResourceFile::VClose()
{
	mp_ZipFile->End();
	return true;
}

int ZipResourceFile::VGetResource(char** buffer, const char* ps_resname)
{
	int size = 0;
	optional<int> resourceNum = mp_ZipFile->Find(ps_resname);
	if (resourceNum.valid())
	{
		size = mp_ZipFile->GetFileLen(*resourceNum);
		mp_ZipFile->ReadFile(*resourceNum, *buffer);
	}
	return size;
}

int ZipResourceFile::VGetRawResourceSize(const char* ps_resname)
{
	int resourceNum = mp_ZipFile->Find(ps_resname);
	if (resourceNum == -1)
		return -1;

	return mp_ZipFile->GetFileLen(resourceNum);
}

int ZipResourceFile::VGetNumResources() const
{
	return (mp_ZipFile == NULL) ? 0 : mp_ZipFile->GetNumFiles();
}