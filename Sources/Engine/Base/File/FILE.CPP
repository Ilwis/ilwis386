/***************************************************************
 ILWIS integrates image, vector and thematic data in one unique 
 and powerful package on the desktop. ILWIS delivers a wide 
 range of feautures including import/export, digitizing, editing, 
 analysis and display of data as well as production of 
 quality mapsinformation about the sensor mounting platform
 
 Exclusive rights of use by 52°North Initiative for Geospatial 
 Open Source Software GmbH 2007, Germany

 Copyright (C) 2007 by 52°North Initiative for Geospatial
 Open Source Software GmbH

 Author: Jan Hendrikse, Willem Nieuwenhuis,Wim Koolhoven 
 Bas Restsios, Martin Schouwenburg, Lichun Wang, Jelle Wind 

 Contact: Martin Schouwenburg; schouwenburg@itc.nl; 
 tel +31-534874371

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program (see gnu-gpl v2.txt); if not, write to
 the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 Boston, MA 02111-1307, USA or visit the web page of the Free
 Software Foundation, http://www.fsf.org.

 Created on: 2007-02-8
 ***************************************************************/
#define FILE_C
#include "Headers\toolspch.h"
#include "Engine\Base\DataObjects\Tranq.h"
#include <sys/utime.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\Engine.h"
#include "Headers\Hs\ILWISGEN.hs"
#include "Headers\Hs\DAT.hs"
#include "Headers\Err\ILWISDAT.ERR"


char *IllegalFileNames[]={"COM1",  "COM2", "COM3", "COM4", "COM5", 
                          "COM6",  "COM7", "COM8", "COM9",
                          "LPT1", "LPT2", "LPT3", "LPT4", "LPT5", 
                          "LPT6", "LPT7", "LPT8", "LPT9",
                          "AUX", "PRN", "CON" };
const int iNoIllegalFileNames=21;



void FileOpenError(const String& sName, int iErr);
void FileCreateError(const String& sName, int iErr);
void FileSeekError(const String& sName, long iPos);
void FileReadError(const String& sName, int iSize);
void FileReadBufError(const String& sName);
void FileWriteError(const String& sName, int iSize);
void FileCloseError(const String& sName);
void FileStillOpenError(const String& sName);
void RenameError(const String& sName, const String& sNewName, int iErr);
void UnlinkError(const String& sName, int iErr);
void FileReadOnlyError(const String& sName);

File::File(const FileName& fn, FileAccess fac) :
 CFile(fn.sFullPath().scVal(), (UINT)fac ),
 iMode(fac),
 fErase(false)
{
	CFileStatus fs;
  GetStatus(fs);
	iFileSize = (long)fs.m_size;
}

File::File(const String& s, FileAccess fac) : CFile(s.scVal()
, (UINT)fac),
iMode(fac),
fErase(false)
{
	CFileStatus fs;
  GetStatus(fs);
	iFileSize = (long)fs.m_size;		
}

File::File(const char* s, FileAccess fac) :
	CFile(s, (UINT)fac),
 iMode(fac) ,
  fErase(false)	
{
	CFileStatus fs;
  GetStatus(fs);
	iFileSize = (long)fs.m_size;		
}

File::~File()
{
	if ( fErase )
	{
		String sN = sName();
		if ( sN == "") return;
		Close();
		DeleteFile(sN.scVal());
	}
}

bool File::fIllegalName(const String& sName)
{
	FileName fn(sName);
	String sShortName=fn.sFile.toUpper();
	for(int i=0; i<iNoIllegalFileNames; ++i)
	{
		String sIllegalName(IllegalFileNames[i]);
		if ( sShortName == sIllegalName ) return true;
	}        
    return false;        
}

void File::Seek(long iLoc, bool fEnd)
// seek to location iLoc, if fEnd = true then seek from end of file
{
  ILWISSingleLock sl(&csAccess, TRUE,SOURCE_LOCATION);
	if (fEnd)
	{
		CFile::Seek(iLoc, CFile::end);
	}		
	else
	{
		CFile::Seek(iLoc, CFile::begin);
	}		
}

long File::iLoc() 
// Return file pointer position
{
  return (long)GetPosition();
}

inline bool File::fEof() 
{
  return iLoc() == iSize();
}

long File::iSize() const
{
  return iFileSize;
}

size_t File::Read(long iSiz, void * p)
// read iSize bytes from file into p
{
  ILWISSingleLock sl(&csAccess, TRUE,SOURCE_LOCATION);
  long iCand = CFile::Read(p, iSiz);

  return (size_t)iCand;
}

char File::cReadChar()
// read char from file
{
	char c[1];
  Read(1L, (void *)c);
	return c[0];
}

size_t File::Read(String& s)
// Read String s from file
// (checks on terminating 0, so only suitable for binary files)
{
  ILWISSingleLock sl(&csAccess, TRUE,SOURCE_LOCATION);
  s = "";
  char c;
  do {
    c = cReadChar();
    s &= c;
  } while (c != 0 && !fEof());
  return s.length();
}

size_t File::ReadLnAscii(char *s, int iMax)
// Read characters into s, stop on cr/lf or if iMax characters have been read
// Only for ascii files. s should already be allocated
{
  ILWISSingleLock sl(&csAccess, TRUE,SOURCE_LOCATION);
  char *pp = s;
  *pp = cReadChar();
  while (!fEof() && (--iMax)) {
    if (*pp == '\r') {    // carriage return
      ++pp;
      *pp = cReadChar();  // read ahead one char
      if (*pp != '\n')
        Seek(iLoc() - 1); // not a LF, back up one char
      else
        --pp;             // skip char: it is a LF
      break;
    }
    else if (*pp == '\n')
      break;
    ++pp;
    *pp = cReadChar();
  }
  if (*pp != '\r' && *pp != '\n')
	  ++pp;  // keep last char if not EOF
  *pp = 0;
  return (pp - s);
}

size_t File::ReadLnAscii(String& s)
// Read characters into String s, stop on cr/lf. Only for ascii files
{
  ILWISSingleLock sl(&csAccess, TRUE,SOURCE_LOCATION);
  s = "";
  short iSkip = 0;         // count CR and LF chars in line
  while (!fEof()) {
    char c = cReadChar();  // Read character
    if (c == '\r') {       // carriage return
      iSkip++;             // the CR will be skipped
      c = cReadChar();     // read ahead one char
      if (c != '\n')
        Seek(iLoc() - 1);  // not a LF, back up one char
      else
        iSkip++;           // skip the char: it is a LF
      return s.length() + iSkip; // return the correct number of chars read from file
    }
    else if (c == '\n')
      return s.length()+1;
    s &= c;
  }
  return s.length();
}

size_t File::Write(long iSize, const void * p)
// write iSize bytes from p into file
{
  ILWISSingleLock sl(&csAccess, TRUE,SOURCE_LOCATION);
	CFile::Write(p, iSize);
	return iSize;

}

bool File::fKeepOpen()   // returns  keep file open flag
{
 // empty, relic of previous eras 
	return true;
}

void File::Flush()
{
 // empty, relic of previous eras 
}

ObjectTime File::filetime() 
// return file creation time
{
  ILWISSingleLock sl(&csAccess, TRUE,SOURCE_LOCATION);
	CFileStatus fs;
  GetStatus(fs);
	return (long)fs.m_atime.GetTime();
}

// static
void File::SetFileTime(const FileName& fn)  // set file time at current date and time
{
	FILETIME timLoc, timAccess;  // last-access time

	SYSTEMTIME stim;
	GetLocalTime(&stim);
	SystemTimeToFileTime(&stim, &timLoc);
	LocalFileTimeToFileTime(&timLoc, &timAccess);

	SetFileTime(fn, 0, &timAccess, &timAccess);
}

// static 
void File::SetFileTime(const FileName& fn,
												const FILETIME *lpCreationTime,    // creation time
												const FILETIME *lpLastAccessTime,  // last-access time
												const FILETIME *lpLastWriteTime)   // last-write time
{
	if (_access(fn.sFullName().sVal(), 2) == -1) // readonly
		return;

	HANDLE hFile;
	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(sa);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = 1;

	hFile = CreateFile(fn.sFullName().sUnQuote().scVal(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, &sa, OPEN_EXISTING,
		                             FILE_ATTRIBUTE_NORMAL, NULL);

	::SetFileTime(hFile, lpCreationTime, lpLastAccessTime, lpLastWriteTime);
  CloseHandle(hFile);
}

void File::Rename(const String& sNewName)
{
  ILWISSingleLock sl(&csAccess, TRUE,SOURCE_LOCATION);
	CString sOldName = GetFileName();
	bool fOpen = m_hFile != CFile::hFileNull;
	Close();
	CFile::Rename(sOldName, sNewName.scVal());
	if ( fOpen)
	{
    iMode &= ~CFile::modeCreate; // prevent truncation
		Open(sNewName.scVal(), iMode);
	}		
}

bool File::fIsBinary(const FileName& fn)
{
	File fil(fn);

	char bytes[256];
	size_t iRd = fil.Read(256, bytes);

	size_t i = 0;
	bool fBin = false;
	while (i < iRd && !fBin)
	{
		switch (bytes[i])
		{
			case 8:   // BS
			case 9:   // TAB
			case 10:  // LF
			case 13:  // CR
				fBin = false;
				break;
			default:
				fBin = bytes[i] < 32;
		}

		i++;
	}

	return fBin;
}

void File::Copy(const FileName& fnSrc, const FileName& fnDest)
{
#define BFSIZE 60000U
	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(sa);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = 1;

  HFILE fileSrc = (HFILE)CreateFile(fnSrc.sFullName().sUnQuote().scVal(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, &sa, OPEN_EXISTING,
		                             FILE_ATTRIBUTE_NORMAL, NULL);
  if (fileSrc == HFILE_ERROR)
    FileOpenError(fnSrc.sFullName(), GetLastError()); //OfStrSrc.nErrCode);
	HFILE fileDest = (HFILE)CreateFile(fnDest.sFullName().sUnQuote().scVal(), GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, &sa, CREATE_ALWAYS,
		                             FILE_ATTRIBUTE_NORMAL, NULL);
  if (fileDest == HFILE_ERROR)
    FileCreateError(fnDest.sFullName(), GetLastError()); //OfStrDest.nErrCode);
  char* buf = new char[BFSIZE];
 // for (unsigned long i=0; i <= iFileSize / BFSIZE - 1; i++) {
  while (1) {
    long iBytes = _hread(fileSrc, buf, BFSIZE);
    if (iBytes == HFILE_ERROR)
      goto end;
    if (_hwrite(fileDest, buf, iBytes) == HFILE_ERROR)
      goto end;
    if (iBytes < BFSIZE)
      break;
  }

  FILETIME CreationTime,LastAccessTime, LastWriteTime; 
  GetFileTime( (HANDLE)fileSrc, &CreationTime, &LastAccessTime, &LastWriteTime);
  ::SetFileTime( (HANDLE)fileDest, &CreationTime, &LastAccessTime, &LastWriteTime);


end:
  _lclose(fileSrc);
  _lclose(fileDest);
  delete [] buf;
}

bool File::fCopy(const FileName& fnSrc, const FileName& fnDest, Tranquilizer& trq)
{
  char* buf = new char[BFSIZE];
	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(sa);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = 1;

  trq.fText(String("%S %S --> %S", SDATOthCopy, fnSrc.sFullName(), fnDest.sFullName()));

  bool f = false;
  HFILE fileSrc = (HFILE)CreateFile(fnSrc.sFullName().sUnQuote().scVal(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, &sa, OPEN_EXISTING,
		                             FILE_ATTRIBUTE_NORMAL, NULL);
  if (fileSrc == HFILE_ERROR)
    FileOpenError(fnSrc.sFullName(), GetLastError()); //OfStrSrc.nErrCode);

  HFILE fileDest = (HFILE)CreateFile(fnDest.sFullName().sUnQuote().scVal(), GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, &sa, CREATE_ALWAYS,
		                             FILE_ATTRIBUTE_NORMAL, NULL);
  if (fileDest == HFILE_ERROR)
    FileCreateError(fnDest.sFullName(), GetLastError()); //OfStrDest.nErrCode);
  if (fileSrc < 0)
    goto end;
  if (fileDest >= 0) {
    while (1) {
      long iBytes = _hread(fileSrc, buf, BFSIZE);
      if (iBytes == HFILE_ERROR)
        goto end;
      if (trq.fAborted())
        goto end;
      if (_hwrite(fileDest, buf, iBytes) == HFILE_ERROR)
        goto end;
      if (iBytes < BFSIZE)
        break;
    }  
  FILETIME CreationTime,LastAccessTime, LastWriteTime; 
  GetFileTime( (HANDLE)fileSrc, &CreationTime, &LastAccessTime, &LastWriteTime);
  ::SetFileTime( (HANDLE)fileDest, &CreationTime, &LastAccessTime, &LastWriteTime);

  }
  else 
    goto end;
  f = true;
end:
  _lclose(fileSrc);
  _lclose(fileDest);
  delete [] buf;
  return f;
}

void File::GetFileNames(const String& sMask, Array<FileName>& afn, const Array<String> *asExt)
{
	afn.Resize(0);
	FileName fn(sMask);
	String sDir = fn.sPath();
	getEngine()->SetCurDir(sDir);

	String sSearch = fn.sFullPath();
	WIN32_FIND_DATA finddata;
	HANDLE handle = FindFirstFile(sSearch.sVal(), &finddata);
	if (handle != INVALID_HANDLE_VALUE) 
		do
		{ 
			FileName fn = String(finddata.cFileName);
			if (0 != asExt)  // check all possible extensions
			{
				for (unsigned short i = 0; i < asExt->iSize(); ++i)
				{
					if ((*asExt)[i] == fn.sExt)
					{
						afn &= fn;
						break;
					}  
				}
			}
			else
				afn &= fn;  // Add always if not extension mask specified
		} 
		while (FindNextFile(handle, &finddata));
	if (handle != INVALID_HANDLE_VALUE )		
		FindClose(handle);		
} 

bool File::fReadOnly(const FileName& fn)
{
	DWORD iAttrib = GetFileAttributes(fn.sFullPath().scVal());
	return iAttrib & FILE_ATTRIBUTE_READONLY;
}

bool File::fReadOnly() const
{
	return File::fReadOnly(GetFilePath());	
}

void File::SetReadOnly(const String& sFileName, bool f)
{
  if (f)
    _chmod(sFileName.scVal(), S_IREAD);
  else
    _chmod(sFileName.scVal(), S_IREAD | S_IWRITE);
}

String sErrOfStruct(int iErr)
// see page 345 of Programmer's Reference SDK 3.1, OFSTRUCT definition
{
  switch (iErr) {
    case 0x0001: return SDATErrInvalidFunction;
    case 0x0002: return SDATErrFileNotFound;
    case 0x0003: return SDATErrPathNotFound;
    case 0x0004: return SDATErrTooManyOpenFiles;
    case 0x0005: return SDATErrAccessDenied;
    case 0x0006: return SDATErrInvalidHandle;
    case 0x0007: return SDATErrArenaTrashed;
    case 0x0008: return SDATErrNotEnoughMemory;
    case 0x0009: return SDATErrInvalidBlock;
    case 0x000A: return SDATErrBadEnvironment;
    case 0x000B: return SDATErrBadFormat;
    case 0x000C: return SDATErrInvalidAccess;
    case 0x000D: return SDATErrInvalidData;
    case 0x000F: return SDATErrInvalidDrive;
    case 0x0010: return SDATErrCurrentDirectory;
    case 0x0011: return SDATErrNotSameDevice;
    case 0x0012: return SDATErrNoMoreFiles;
    case 0x0013: return SDATErrWriteProtectError;
    case 0x0014: return SDATErrBadUnit;
    case 0x0015: return SDATErrNotReady;
    case 0x0016: return SDATErrBadCommand;
    case 0x0017: return SDATErrCRCError;
    case 0x0018: return SDATErrBadLength;
    case 0x0019: return SDATErrSeekError;
    case 0x001A: return SDATErrNotMSDOSDisk;
    case 0x001B: return SDATErrSectorNotFound;
    case 0x001C: return SDATErrOutOfPaper;
    case 0x001D: return SDATErrWriteFault;
    case 0x001E: return SDATErrReadFault;
    case 0x001F: return SDATErrGeneralFailure;
    case 0x0020: return SDATErrSharingViolation;
    case 0x0021: return SDATErrLockViolation;
    case 0x0022: return SDATErrWrongDisk;
    case 0x0023: return SDATErrFCBUnavailable;
    case 0x0024: return SDATErrSharingBufferExceeded;
    case 0x0032: return SDATErrNotSupported;
    case 0x0033: return SDATErrRemoteNotListed;
    case 0x0034: return SDATErrDuplicateName;
    case 0x0035: return SDATErrBadNetpath;
    case 0x0036: return SDATErrNetworkBusy;
    case 0x0037: return SDATErrDeviceNotExist;
    case 0x0038: return SDATErrTooManyCommands;
    case 0x0039: return SDATErrAdaptorHardwareError;
    case 0x003A: return SDATErrBadNetworkResponse;
    case 0x003B: return SDATErrUnexpectedNetworkError;
    case 0x003C: return SDATErrBadRemoteAdaptor;
    case 0x003D: return SDATErrPrintQueueFull;
    case 0x003E: return SDATErrNoSpoolSpace;
    case 0x003F: return SDATErrPrintCanceled;
    case 0x0040: return SDATErrNetnameDeleted;
    case 0x0041: return SDATErrNetworkAccessDenied;
    case 0x0042: return SDATErrBadDeviceType;
    case 0x0043: return SDATErrBadNetworkName;
    case 0x0044: return SDATErrTooManyNames;
    case 0x0045: return SDATErrTooManySessions;
    case 0x0046: return SDATErrSharingPaused;
    case 0x0047: return SDATErrRequestNotAccepted;
    case 0x0048: return SDATErrRedirectionPaused;
    case 0x0050: return SDATErrFileExists;
    case 0x0051: return SDATErrDuplicateFCB;
    case 0x0052: return SDATErrCannotMake;
    case 0x0053: return SDATErrInterrupt24Failure;
    case 0x0054: return SDATErrOutOfStructures;
    case 0x0055: return SDATErrAlreadyAssigned;
    case 0x0056: return SDATErrInvalidPassword;
    case 0x0057: return SDATErrInvalidParameter;
    case 0x0058: return SDATErrNetWriteFault;
    default: return SDATErrUnknownError;
  }
}

void FileOpenError(const String& sName, int iErr)
{
  String sErr("%S '%S'\n%S", SDATErrFileOpenErrorIn, sName, sErrOfStruct(iErr));
  throw FileErrorObject(sErr, errFileOpen);
}

void FileCreateError(const String& sName, int iErr)
{
  String sErr("%S '%S'\n%S", SDATErrFileCreateErrorIn, sName, sErrOfStruct(iErr));
  throw FileErrorObject(sErr, errFileCreate);
}

void FileSeekError(const String& sName, long iPos)
{
  String sErr("%S '%S'\n%S %li", SDATErrFileSeekErrorIn, sName, SDATOthPosition, iPos);
  throw FileErrorObject(sErr, errFileSeek);
}

void FileReadError(const String& sName, int iSize)
{
  String sErr("%S '%S'\n%S %i %S", SDATErrFileReadErrorIn, sName, SDATOthBlockOf, iSize, SDATOthBytes);
  throw FileErrorObject(sErr, errFileRead);
}

void FileReadBufError(const String& sName)
{
  String sErr("%S '%S'", SDATErrFileReadBufferIn, sName);
  throw FileErrorObject(sErr, errFileReadBuf);
}

void FileWriteError(const String& sName, int iSize)
{
  String sErr("%S '%S'\n%S %i %S", SDATErrFileWriteBufferIn, sName, SDATOthBlockOf, iSize, SDATOthBytes);
  throw FileErrorObject(sErr, errFileWrite);
}

void FileCloseError(const String& sName)
{
  String sErr("%S '%S'", SDATErrFileCloseErrorIn, sName);
  throw FileErrorObject(sErr, errFileClose);
}

void FileStillOpenError(const String& sName)
{
  String sErr("%S '%S'\n%S", SDATErrFileStillOpenErrorIn, sName, SDATOthPleaseReport);
  throw FileErrorObject(sErr, errFileStillOpen);
}

void RenameError(const String& sName, const String& sNewName, int iErr)
{
  String sErr("%S '%S' %S '%S'\n%s", SDATErrRenameErrorFrom, sName, SDATOthTo, sNewName, strerror(iErr));
  throw FileErrorObject(sErr, errFileRename);
}

void UnlinkError(const String& sName, int iErr)
{
  String sErr("%S '%S'\n%s", SDATErrUnlinkErrorIn, sName, strerror(iErr));
  throw FileErrorObject(sErr, errFileUnlink);
}

void FileReadOnlyError(const String& sName)
{
  String sErr("%S '%S'\n", SDATErrFileReadOnly, sName);
  throw FileErrorObject(sErr, errReadOnlyFile);
}


