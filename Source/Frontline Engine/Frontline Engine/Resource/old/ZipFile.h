#pragma once
#include "..\FrontlineCommon.h"
#include <ZLIB\zlib.h>
#include <string>
#include <stdio.h>
#include <map>


// This maps a path to a zip content id
typedef std::map<std::string, int> ZipContentsMap;

class ZipFile
{
	struct TZipDirHeader;
	struct TZipDirFileHeader;
	struct TZipLocalHeader;

	FILE* mp_file;
	char* ms_dirData;
	int m_nEntries;

	const TZipDirFileHeader** m_papDir;
public:
	ZipFile() { m_nEntries = 0; mp_file = NULL; ms_dirData = NULL; }
	virtual ~ZipFile() { End(); fclose(mp_file); }

	bool Init(const std::wstring& ps_filename);
	void End();

	int GetNumFiles() const { return m_nEntries; }
	bool ReadFile(int i, void* pBuf);

	bool ReadLargeFile(int i, void* pp_buf, void(*progressCallback)(int, bool&));
	int Find(const std::string ps_path);

	int GetFileLen(int i) const;

	std::string GetFilename(int i) const;

	ZipContentsMap mmap_zipContents;
};