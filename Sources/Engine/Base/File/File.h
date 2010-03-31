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
/* app/file.h
   File Interface for ILWIS 2.1
// 4 may 1993, by Wim Koolhoven
// changed sep 1993, by Jelle Wind (buffering and ascii support)
   Total rewrite: june 1997 by Wim Koolhoven
   a FileHandle is valid for one application,
   IlwisObjects need to be readable and writable by multiple applications
	Last change:  MLS   4 Jan 99    8:27 am
*/
#ifndef ILWFILE_H
#define ILWFILE_H

#undef IMPEXP
#ifdef ILWISENGINE
#define IMPEXP __export
#else
#define IMPEXP __import
#endif


class _import Tranquilizer;

enum FileAccess {
	facRO     = CFile::modeRead | CFile::shareDenyWrite,
	facRW     = CFile::modeReadWrite | CFile::shareDenyWrite,
	facCRT    = CFile::modeCreate | CFile::modeReadWrite | CFile::shareDenyWrite,
	facRW_All = CFile::modeReadWrite | CFile::shareDenyNone
};

#define errFileOpen      errFile
#define errFileCreate    errFile+1
#define errFileSeek      errFile+2
#define errFileRead      errFile+3
#define errFileReadBuf   errFile+4
#define errFileWrite     errFile+5
#define errFileClose     errFile+6
#define errFileStillOpen errFile+7
#define errFileRename    errFile+8
#define errFileUnlink    errFile+9
#define errIllegalFile   errFile+10
#define errReadOnlyFile  errFile+11


class IMPEXP File : public CFile
{
  File(const File&); // copy constructor not allowed!
  File operator = (const File&); // assignment not allowed!
public:
  File(const FileName& fn, FileAccess fac = facRO);
  File(const String& sName, FileAccess fac = facRO);
  File(const char* sName, FileAccess fac = facRO);
  ~File();

  const String sName() const { return GetFilePath(); }
  inline void SetErase(bool f = true) { fErase = f; }

  void Seek(long iLoc, bool fEnd = false);
  // seek to location iLoc, if fEnd = true then seek from end of file
  long iLoc() ;
  // Return file pointer position
  bool fEof() ;
  // returns true if end of file has been reached
  long iSize() const;

  size_t Read(long iSize, void * p);
  // read iSize bytes from file into p
  char cReadChar();
  // read char from file
  size_t Read(String& s);
  // Read String s from file
  // (checks on terminating 0, so only suitable for binary files)
  size_t ReadLnAscii(char *s, int iMax);
  // Read characters into s, stop on cr/lf or if iMax characters have been read
  // Only for ascii files. s should already be allocated
  size_t ReadLnAscii(String& s);
  // Read characters into String s, stop on cr/lf. Only for ascii files

  size_t Write(long iSize, const void * p);
  // write iSize bytes from p into file
  // size_t Write(long iSize, const void huge* p);
//  size_t Append(long iSize, const void * p);
  // append iSize bytes from p to file
  size_t Write(const String& s) { return Write(s.length()+1, s.scVal()); }
  // write String s to file
  // (including terminating 0, so only suitable for binary files)
  size_t WriteLnAscii(const String& s)
    { return (Write(s.length(),s.scVal()) + Write(2, "\r\n")); }
  // Write String s to file and appends cr/lf. Only for ascii files.

  void KeepOpen(bool f) {};  // to keep file open all the time (true: increase count, false: decrease count)
  bool fKeepOpen() ;  // returns  keep file open flag
 // void ReOpenForWriting();
  void Flush();   // flushes internal buffers to file

  Time filetime();   // return file creation time
	static void SetFileTime(const FileName&);  // set file time at current date and time
	static void SetFileTime(const FileName&,
													const FILETIME *lpCreationTime,   // creation time
													const FILETIME *lpLastAccessTime, // last-access time
													const FILETIME *lpLastWriteTime);   // last-write time

  void Rename(const String& sName);
 // void Truncate(); // reduce file size to 0

  static void Copy(const FileName& fnSrc, const FileName& fnDest);
  static bool fCopy(const FileName& fnSrc, const FileName& fnDest, Tranquilizer& trq);
  static bool fExist(const FileName& fn)
   { return fn.fValid() && _access(fn.sFullPath().scVal(), 0)==0 ; }
  static void SetReadOnly(const String& sFileName, bool f);
  static void GetFileNames(const String& sMask, Array<FileName>& afn, const Array<String>* asExt = 0);
  static bool fIllegalName(const String& sName);
	static bool fIsBinary(const FileName& fn);  // determine if fn is a binary file
	static bool fReadOnly(const FileName& fn);
	bool fReadOnly() const;

private:
  bool fErase;
  long iFileSize;
  int iMode;  // selected as new file access: file reopen needed

private:
  CCriticalSection csAccess;
};

class IMPEXP FileErrorObject: public ErrorObject
{
public:
  FileErrorObject(const String& sErr, int err)
    : ErrorObject(sErr, err) {}
};

#endif








