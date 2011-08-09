
#include "Headers\toolspch.h"
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Engine\Base\File\Directory.h"
#include "Engine\Base\StlExtensions.h"
#include "Engine\Base\DataObjects\ObjectStructure.h"
#include "Engine\Domain\dmsort.h"
#include "Headers\constant.h"
#include "Engine\Base\File\BaseCopier.h"


BaseCopier::BaseCopier() {
}

BaseCopier::~BaseCopier() {
}

BaseCopier::BaseCopier(const vector<FileName>& files, const Directory& _dirDestination) :
	dirDestination(_dirDestination)
{
	FilesToBeCopied(files) ;
}

BaseCopier::BaseCopier(const FileName& fnFile, const Directory& _dirDestination ) :
		dirDestination(_dirDestination),
		fnDestination(fnFile)
{
}

BaseCopier::BaseCopier(const FileName& fnFile, const FileName& _fnDestination ) :
		dirDestination(Directory(_fnDestination.sPath())),
		fnDestination(fnFile)
{
	fnDestination = _fnDestination;
	FileToBeCopied(fnFile);
}

void BaseCopier::FilesToBeCopied(const vector<FileName>& files) 
{
	fnFiles.resize(files.size());
	copy(files.begin(), files.end(), fnFiles.begin());
}

void BaseCopier::FileToBeCopied( const FileName& fn)
{
	fnFiles.clear();
	fnFiles.push_back(fn);
}

bool BaseCopier::fHasWildcards(FileName & fn)
{
	String sFile = fn.sRelative();
	return (sFile.find('*') != -1 || sFile.find('?') != -1);
}

void BaseCopier::ExpandWildcards(vector <FileName> & fnList)
{
	bool fWildcardsFound = false;
	vector <FileName> fnNewList;
	for(vector<FileName>::iterator cur = fnList.begin(); cur != fnList.end(); ++cur)
	if (fHasWildcards(*cur))
	{
		ExpandWildcards(*cur, fnNewList);
		fWildcardsFound = true;
	}
	else
		fnNewList.push_back(*cur);

	if (fWildcardsFound)
	{
		fnList.resize(fnNewList.size());
		copy(fnNewList.begin(), fnNewList.end(), fnList.begin());
	}
	// else: fnList remains untouched (no wildcards = old behavior)
}

void BaseCopier::ExpandWildcards(FileName & fnWildcard, vector <FileName> & fnList)
{
	CString sPattern = fnWildcard.sFullPath().c_str();
	CFileFind finder;
	BOOL bWorking = finder.FindFile(sPattern);
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		if (!finder.IsDirectory())
		{
			FileName fnNew (finder.GetFilePath());
			if(!fnNew.fIsIlwisDataFile())
				fnList.push_back(fnNew);
		}
	}
}

void BaseCopier::GetDestinationDirFiles(list<FileName>& lstFiles)
{
	CFileFind finder;
	String sDirFiles = dirDestination.sFullPath() + "*.*";
	bool fFound = finder.FindFile(sDirFiles.c_str()) == TRUE;
	if ( !fFound) return;
	finder.FindNextFile();
	while (finder.FindNextFile() ) 
	{
		CString str = finder.GetFilePath();
		if ( !finder.IsDots() && !finder.IsDirectory())
			lstFiles.push_back( FileName( str));
	}
	CString str = finder.GetFilePath();
	if ( !finder.IsDots() && !finder.IsDirectory())
		lstFiles.push_back( FileName( str));
}

typedef BOOL (__stdcall * GetDiskFreeSpaceExFunction)(
	LPCTSTR lpDirectoryName,                 // directory name
	PULARGE_INTEGER lpFreeBytesAvailable,    // bytes available to caller
	PULARGE_INTEGER lpTotalNumberOfBytes,    // bytes on disk
	PULARGE_INTEGER lpTotalNumberOfFreeBytes // free bytes on disk
);

bool BaseCopier::fSufficientDiskSpace(unsigned __int64 & iSpaceRequired, unsigned __int64& iSpaceAvailable, String sDrive)
{
	GetDiskFreeSpaceExFunction pGetDiskFreeSpaceEx = (GetDiskFreeSpaceExFunction)GetProcAddress( GetModuleHandle("kernel32.dll"),
													 "GetDiskFreeSpaceExA");

	ULARGE_INTEGER iAvail, dummy1, dummy2;
	bool fResult;
	if (pGetDiskFreeSpaceEx)
	{
		fResult = pGetDiskFreeSpaceEx (sDrive.c_str(), &iAvail, &dummy1, &dummy2) > 0;
	  iSpaceAvailable = iAvail.QuadPart;
	}
	else 
	{
		DWORD dwSectPerClust, dwBytesPerSect, dwFreeClusters, dwTotalClusters;
		fResult = GetDiskFreeSpace (sDrive.c_str(), 
									&dwSectPerClust, 
									&dwBytesPerSect,
									&dwFreeClusters, 
									&dwTotalClusters) > 0;
		iSpaceAvailable = dwFreeClusters * dwSectPerClust * dwBytesPerSect;
	}
	return iSpaceRequired < iSpaceAvailable;
}

void BaseCopier::BuildCopyStrings(	list<String>& lstFilesUsed, list<String>& lstFilesNew, char **sNameBuf, char **sDestBuf, bool fUseAbsolutePaths)
{
	String sUsedFiles;
	String sNewFiles;
	for( list<String>::iterator cur = lstFilesUsed.begin(); cur != lstFilesUsed.end(); ++cur)
	{
		String sName = (*cur).sUnQuote();
		sUsedFiles = sUsedFiles + sName + "*";
	}

	for( list<String>::iterator cur2 = lstFilesNew.begin(); cur2 != lstFilesNew.end(); ++cur2)
	{
		String sName = (*cur2).sUnQuote();
		sNewFiles = sNewFiles + sName + "*";
	}
	
	size_t iSize = sUsedFiles.size();
	*sNameBuf = new char[iSize + 1];
	memset(*sNameBuf, 0, iSize + 1);
	iSize = sNewFiles.size();
	*sDestBuf = new char[iSize + 1];
	memset(*sDestBuf, 0, iSize + 1);
	
	for(unsigned int i=0 ; i < sUsedFiles.size(); ++i)
	{
		if ( sUsedFiles[i] != '*')
			(*sNameBuf)[i] = sUsedFiles[i];
	}
	for(unsigned int i=0 ; i < sNewFiles.size(); ++i)
	{
		if ( sNewFiles[i] != '*')
			(*sDestBuf)[i] = sNewFiles[i];

	}
}

void BaseCopier::CopyFileList(list<String>& lstFilesUsed, char *sNameBuf, char *sDestBuf)
{
	if ( sNameBuf == 0 || sDestBuf == 0 || 	AfxGetMainWnd() == NULL)
		return;
	for( list<String>::iterator cur = lstFilesUsed.begin(); cur != lstFilesUsed.end(); ++cur)
	{
		FileName fn(*cur);
		DWORD iAttrib = GetFileAttributes( fn.sFullPath().c_str() );
		SetFileAttributes( fn.sFullPath().c_str(), iAttrib & ~FILE_ATTRIBUTE_READONLY );
		DeleteFile(fn.sFullPath().c_str());
	}

	SHFILEOPSTRUCT shFo;
	shFo.hwnd = AfxGetMainWnd()->m_hWnd;
	shFo.wFunc = FO_COPY;
	shFo.pFrom = sNameBuf;
	shFo.pTo = sDestBuf;
	shFo.fFlags = FOF_MULTIDESTFILES ;
	//shFo.fFlags =  
	int iR = SHFileOperation(&shFo);
	
}
