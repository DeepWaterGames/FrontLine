#include "ZipFile.h"
#include <cctype>
#include <tchar.h>
#include "..\FrontlineCommon.h"

typedef unsigned long dword;
typedef unsigned short word;


#pragma pack(1)

struct ZipFile::TZipLocalHeader
{
	enum
	{
		SIGNATURE = 0x04034b50
	};

	dword sig;
	word version;
	word flag;
	word compression;
	word modTime;
	word modDate;
	dword crc32;
	dword cSize;
	dword ucSize;
	word fnameLen;
	word xtraLen;
};

struct ZipFile::TZipDirHeader
{
	enum
	{
		SIGNATURE = 0x06054b50
	};
	dword sig;
	word nDisk;
	word nStartDisk;
	word nDirEntries;
	word totalDirEntries;
	dword dirSize;
	dword dirOffset;
	word cmntLen;
};

struct ZipFile::TZipDirFileHeader
{
	enum { SIGNATURE = 0x02014b50 };
	dword sig;
	word verMade;
	word verNeeded;
	word flag;
	word compression; // COMP_xxxx
	word modTime;
	word modDate;
	dword crc32;
	dword cSize; // Compressed size
	dword ucSize; // Uncompressed size
	word fnameLen; // Filename string follows header.
	word xtraLen; // Extra field follows filename.
	word cmntLen; // Comment field follows extra field.
	word diskStart;
	word intAttr;
	dword extAttr;
	dword hdrOffset;
	char *GetName() const { return (char *) (this + 1); }
	char *GetExtra() const { return GetName() + fnameLen; }
	char *GetComment() const { return GetExtra() + xtraLen; }
};

#pragma pack()

bool ZipFile::Init(const std::wstring& ps_filename)
{
	End();
	_wfopen_s(&mp_file, ps_filename.c_str(), _T(L"rb"));
	if (!mp_file)
		return false;

	TZipDirHeader l_dirHeader;

	fseek(mp_file, -(int)sizeof(l_dirHeader), SEEK_END);
	long ll_dhOffset = ftell(mp_file);
	memset(&l_dirHeader, 0, sizeof(l_dirHeader));
	fread(&l_dirHeader, sizeof(l_dirHeader), 1, mp_file);

	if (l_dirHeader.sig != TZipDirHeader::SIGNATURE)
		return false;

	fseek(mp_file, ll_dhOffset - l_dirHeader.dirSize, SEEK_SET);

	ms_dirData = FL_NEW char[l_dirHeader.dirSize + l_dirHeader.nDirEntries * sizeof(*m_papDir)];


	if (!ms_dirData)
		return false;
	memset(ms_dirData, 0, l_dirHeader.dirSize + l_dirHeader.nDirEntries*sizeof(*m_papDir));
	fread(ms_dirData, l_dirHeader.dirSize, 1, mp_file);

	// Now process each entry.
	char *pfh = ms_dirData;
	m_papDir = (const TZipDirFileHeader **) (ms_dirData + l_dirHeader.dirSize);

	bool success = true;

	for (int i = 0; i < l_dirHeader.nDirEntries && success; i++)
	{
		TZipDirFileHeader &fh = *(TZipDirFileHeader*) pfh;

		// Store the address of nth file for quicker access.
		m_papDir[i] = &fh;

		// Check the directory entry integrity.
		if (fh.sig != TZipDirFileHeader::SIGNATURE)
			success = false;
		else
		{
			pfh += sizeof(fh);

			// Convert UNIX slashes to DOS backlashes.
			for (int j = 0; j < fh.fnameLen; j++)
			if (pfh[j] == '/')
				pfh[j] = '\\';

			char fileName[_MAX_PATH];
			memcpy(fileName, pfh, fh.fnameLen);
			fileName[fh.fnameLen] = 0;
			_strlwr_s(fileName, _MAX_PATH);
			std::string spath = fileName;
			mmap_zipContents[spath] = i;

			// Skip name, extra and comment fields.
			pfh += fh.fnameLen + fh.xtraLen + fh.cmntLen;
		}
	}
	if (!success)
	{
		SAFE_DELETE_ARRAY(ms_dirData);
	}
	else
	{
		m_nEntries = l_dirHeader.nDirEntries;
	}

	return success;
}

int ZipFile::Find(const std::string ps_path)
{
	std::string lowerCase = ps_path;
	std::transform(lowerCase.begin(), lowerCase.end(), lowerCase.begin(), (int(*)(int)) std::tolower);
	ZipContentsMap::const_iterator i = mmap_zipContents.find(lowerCase);
	if (i == mmap_zipContents.end())
		return -1;

	return i->second;
}



// --------------------------------------------------------------------------
// Function:      End
// Purpose:       Finish the object
// Parameters:    
// --------------------------------------------------------------------------
void ZipFile::End()
{
	mmap_zipContents.clear();
	SAFE_DELETE_ARRAY(ms_dirData);
	m_nEntries = 0;
}

// --------------------------------------------------------------------------
// Function:      GetFilename
// Purpose:       Return the name of a file
// Parameters:    The file index and the buffer where to store the filename
// --------------------------------------------------------------------------
std::string ZipFile::GetFilename(int i) const
{
	std::string fileName = "";
	if (i >= 0 && i < m_nEntries)
	{
		char pszDest[_MAX_PATH];
		memcpy(pszDest, m_papDir[i]->GetName(), m_papDir[i]->fnameLen);
		pszDest[m_papDir[i]->fnameLen] = '\0';
		fileName = pszDest;
	}
	return fileName;
}


// --------------------------------------------------------------------------
// Function:      GetFileLen
// Purpose:       Return the length of a file so a buffer can be allocated
// Parameters:    The file index.
// --------------------------------------------------------------------------
int ZipFile::GetFileLen(int i) const
{
	if (i < 0 || i >= m_nEntries)
		return -1;
	else
		return m_papDir[i]->ucSize;
}

// --------------------------------------------------------------------------
// Function:      ReadFile
// Purpose:       Uncompress a complete file
// Parameters:    The file index and the pre-allocated buffer
// --------------------------------------------------------------------------
bool ZipFile::ReadFile(int i, void *pBuf)
{
	if (pBuf == NULL || i < 0 || i >= m_nEntries)
		return false;

	// Quick'n dirty read, the whole file at once.
	// Ungood if the ZIP has huge files inside

	// Go to the actual file and read the local header.
	fseek(mp_file, m_papDir[i]->hdrOffset, SEEK_SET);
	TZipLocalHeader h;

	memset(&h, 0, sizeof(h));
	fread(&h, sizeof(h), 1, mp_file);
	if (h.sig != TZipLocalHeader::SIGNATURE)
		return false;

	// Skip extra fields
	fseek(mp_file, h.fnameLen + h.xtraLen, SEEK_CUR);

	if (h.compression == Z_NO_COMPRESSION)
	{
		// Simply read in raw stored data.
		fread(pBuf, h.cSize, 1, mp_file);
		return true;
	}
	else if (h.compression != Z_DEFLATED)
		return false;

	// Alloc compressed data buffer and read the whole stream
	char *pcData = FL_NEW char[h.cSize];
	if (!pcData)
		return false;

	memset(pcData, 0, h.cSize);
	fread(pcData, h.cSize, 1, mp_file);

	bool ret = true;

	// Setup the inflate stream.
	z_stream stream;
	int err;

	stream.next_in = (Bytef*) pcData;
	stream.avail_in = (uInt) h.cSize;
	stream.next_out = (Bytef*) pBuf;
	stream.avail_out = h.ucSize;
	stream.zalloc = (alloc_func) 0;
	stream.zfree = (free_func) 0;

	// Perform inflation. wbits < 0 indicates no zlib header inside the data.
	err = inflateInit2(&stream, -MAX_WBITS);
	if (err == Z_OK)
	{
		err = inflate(&stream, Z_FINISH);
		inflateEnd(&stream);
		if (err == Z_STREAM_END)
			err = Z_OK;
		inflateEnd(&stream);
	}
	if (err != Z_OK)
		ret = false;

	delete[] pcData;
	return ret;
}



// --------------------------------------------------------------------------
// Function:      ReadLargeFile
// Purpose:       Uncompress a complete file with callbacks.
// Parameters:    The file index and the pre-allocated buffer
// --------------------------------------------------------------------------
bool ZipFile::ReadLargeFile(int i, void *pBuf, void(*progressCallback)(int, bool &))
{
	if (pBuf == NULL || i < 0 || i >= m_nEntries)
		return false;

	// Quick'n dirty read, the whole file at once.
	// Ungood if the ZIP has huge files inside

	// Go to the actual file and read the local header.
	fseek(mp_file, m_papDir[i]->hdrOffset, SEEK_SET);
	TZipLocalHeader h;

	memset(&h, 0, sizeof(h));
	fread(&h, sizeof(h), 1, mp_file);
	if (h.sig != TZipLocalHeader::SIGNATURE)
		return false;

	// Skip extra fields
	fseek(mp_file, h.fnameLen + h.xtraLen, SEEK_CUR);

	if (h.compression == Z_NO_COMPRESSION)
	{
		// Simply read in raw stored data.
		fread(pBuf, h.cSize, 1, mp_file);
		return true;
	}
	else if (h.compression != Z_DEFLATED)
		return false;

	// Alloc compressed data buffer and read the whole stream
	char *pcData = FL_NEW char[h.cSize];
	if (!pcData)
		return false;

	memset(pcData, 0, h.cSize);
	fread(pcData, h.cSize, 1, mp_file);

	bool ret = true;

	// Setup the inflate stream.
	z_stream stream;
	int err;

	stream.next_in = (Bytef*) pcData;
	stream.avail_in = (uInt) h.cSize;
	stream.next_out = (Bytef*) pBuf;
	stream.avail_out = (128 * 1024); //  read 128k at a time h.ucSize;
	stream.zalloc = (alloc_func) 0;
	stream.zfree = (free_func) 0;

	// Perform inflation. wbits < 0 indicates no zlib header inside the data.
	err = inflateInit2(&stream, -MAX_WBITS);
	if (err == Z_OK)
	{
		uInt count = 0;
		bool cancel = false;
		while (stream.total_in < (uInt) h.cSize && !cancel)
		{
			err = inflate(&stream, Z_SYNC_FLUSH);
			if (err == Z_STREAM_END)
			{
				err = Z_OK;
				break;
			}
			else if (err != Z_OK)
			{
				FL_ASSERT(0 && "Something happened.");
				break;
			}

			stream.avail_out = (128 * 1024);
			stream.next_out += stream.total_out;

			progressCallback(count * 100 / h.cSize, cancel);
		}
		inflateEnd(&stream);
	}
	if (err != Z_OK)
		ret = false;

	delete[] pcData;
	return ret;
}