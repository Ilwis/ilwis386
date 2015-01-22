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
#define FILENAME_C
#include "Headers\toolspch.h"
#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\Engine.h"

//static char* pcIllegalChars ="\\/:*?\"<>|%";

bool operator != (const FileName& fnA, const FileName& fnB) 
{ return !(fnA == fnB); }

//---[ FilenameIter ]----------------------------------------------------------------------------------------------
FilenameIter::FilenameIter(const string& sFilenameMask, bool fSubdirectories) :
              _fSubdirectories(fSubdirectories),
              _fValid(false),
              _findHandle(0),
              _BaseLength(0)
{
	FileName fn(sFilenameMask);
	_sMask = fn.sShortName();

  string sStartDir = fn.sPath();

  _BaseLength = sStartDir.size();
  _dirTree.push(""); // top needs to be empty to catch the first pop()
  MakeDirectoryTree(sStartDir);
  Reset(_sMask);
}

// search handle must not(!) be copied but created anew. So move again to the correct file (expensive!)
FilenameIter::FilenameIter(const FilenameIter& iter) :
  _sMask(iter._sMask),
  _fValid(false),
  _fSubdirectories(iter._fSubdirectories),
  _dirTree(iter._dirTree),
  _BaseLength(iter._BaseLength)
{
  _findHandle = FindFirstFile(_sMask.c_str(),&_findData);

  if ( _findHandle == INVALID_HANDLE_VALUE )
    return; // probably always false but with multthreading you never know;

  SetCurrentDirectory(_dirTree.front().c_str());
  _dirTree.pop();
  do
  {
    if (string(iter._findData.cFileName) == string(_findData.cFileName))
    {
      _fValid = true;
      break;
    }
  } while(FindNextFile(_findHandle, &_findData));
}

FilenameIter::~FilenameIter()
{
  if (_findHandle!=0 && _findHandle != INVALID_HANDLE_VALUE )
    FindClose(_findHandle);
}

void FilenameIter::MakeDirectoryTree(string s)
{
    bool fCurDirAdded=false;
    if (! _fSubdirectories)
    {
      _dirTree.push(s);
      return ;
    }
    WIN32_FIND_DATA findData;
    string sNewDir=s + "\\*.*";
    HANDLE findHandle = FindFirstFile(sNewDir.c_str(),&findData);
    if ( findHandle != INVALID_HANDLE_VALUE)
    {
        do
        {
            string filename(findData.cFileName);
            if ( findData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
            {
                if ( filename != ".." && filename != ".")
                    MakeDirectoryTree(s + "\\" + filename);

            }
            else
            {
                 if ( !fCurDirAdded)
                 {
                    _dirTree.push(s);
                    fCurDirAdded=true;
                }
            }                    
        } while(FindNextFile(findHandle, &findData));
    }
    FindClose(findHandle);
}

// reiintialize the search
void FilenameIter::Reset(string sFilenameMask)
{
  if ( _findHandle && _findHandle != INVALID_HANDLE_VALUE)
  {
      FindClose(_findHandle);
      _findHandle=0;
      _fValid=false;
      queue<string, list<string> > dummy;
  }
  _sMask=sFilenameMask;
  _dirTree.pop();
  if (!_dirTree.empty())
  {
    _sCurMask = _dirTree.front() + "\\" + _sMask.c_str();
    while ((_findHandle = FindFirstFile(_sCurMask.c_str(),&_findData)) == INVALID_HANDLE_VALUE && !_dirTree.empty() )
    {
        _dirTree.pop();
        if ( !_dirTree.empty())
                _sCurMask = _dirTree.front() + "\\" + _sMask.c_str();

    }
    if (_findHandle != INVALID_HANDLE_VALUE )
    {
        _fValid=true;
    }
  }
}

const string FilenameIter::operator*() const
{
  if ( !_fValid ) return "";
  if ( _dirTree.empty() ) return "";
  string sFile=_dirTree.front() + "\\" + string(_findData.cFileName); 
  string sFileName=sFile.substr(_BaseLength + 1); // strip the base path from the absolute path
  return sFileName;
}

FilenameIter& FilenameIter::operator++()
{
  if (_findHandle != INVALID_HANDLE_VALUE && _findHandle != 0)
  {
    _fValid = FindNextFile(_findHandle, &_findData) != 0;
    if (!_fValid ) // go to the next directory
    {
       Reset(_sMask);
    }
  }
  else
     throw ErrorObject("XXTrying to use Illegal file handle");

  return *this;
}



//-----------------------------------------------------------------------------------------------------

void FileName::init(const char* sName, bool includeSystemDir)
{
	if (sName == 0)  return;

	Split(sName);
	sExt.toLower();

	if ((sDrive.length()==0) && (sDir.length()==0))
	// set drive and path from current drive an path
	if (includeSystemDir) {
		String path = getEngine()->getContext()->sStdDir() + "\\";
		String name = String(sName);
		String file = path + name;
		if ( _access(file.c_str(), 0)==0)
			Dir(path);
		else {
			path += "Basemaps\\";
			file = path + name;
			if ( _access(file.c_str(), 0)==0){
				Dir(path);
			} else
				Dir(getEngine()->sGetCurDir());
		}
	}
	else
		Dir(getEngine()->sGetCurDir());
	else if (0 != sFile.length())  // complete path to full path
	{
		char *sFullPath = new char[MAX_PATH];
		String sPath = sDrive+sDir;
		_fullpath(sFullPath, sPath.c_str(), MAX_PATH);
		Dir(sFullPath);
		delete [] sFullPath;
	}
}

void FileName::initExt(const char* sName, const char* sExtension, bool fReplaceExtension)
{
	init(sName);

	if (fReplaceExtension || (sExt.length()==0)) 
	{
		sExt = sExtension;
		sExt.toLower();
		sSectionPostFix = "";
	}
}

void FileName::initCopyFrom(const FileName& fn)
{
	sDrive = fn.sDrive;
	sDir = fn.sDir;
	sFile = fn.sFile;
	sExt = fn.sExt;
	sCol = fn.sCol;
}

// Constructors: name
FileName::FileName() 
{
}

FileName::FileName(const char* sName, bool includeSystemDir)
{
	init(sName,includeSystemDir);
}

FileName::FileName(const String& sName, bool includeSystemDir)
{
	init(sName.c_str(),includeSystemDir);
}

FileName::FileName(const CString& sName, bool includeSystemDir)
{
	init((LPCTSTR)sName,includeSystemDir);
}

// Constructors: name and extension
FileName::FileName(const char* sName, const char* sExtension,
           bool fReplaceExtension)
{
	initExt(sName, sExtension, fReplaceExtension);
}

FileName::FileName(const CString& sName, const CString& sExtension,
           bool fReplaceExtension)
{
	initExt((LPCTSTR)sName, (LPCTSTR)sExtension, fReplaceExtension);
}

FileName::FileName(const String& sName, const String& sExtension,
           bool fReplaceExtension)
{
	initExt(sName.c_str(), sExtension.c_str(), fReplaceExtension);
}

FileName::FileName(const String& sName, const char* sExtension,
           bool fReplaceExtension)
{
	initExt(sName.c_str(), sExtension, fReplaceExtension);
}

// Copy constructors
FileName::FileName(const FileName& fn)
{
	initCopyFrom(fn);
	sSectionPostFix = fn.sSectionPostFix;
}

FileName::FileName(const FileName& fn,const String& sExtension,
           bool fReplaceExtension)
{
	initCopyFrom(fn);

	if (fReplaceExtension || (sExt.length()==0))
		sExt = sExtension;
	else
		sSectionPostFix = fn.sSectionPostFix;
}

FileName::FileName(const FileName& fn, const char* sExtension,
           bool fReplaceExtension)
{
	initCopyFrom(fn);

	if (fReplaceExtension || (sExt.length()==0))
		sExt = sExtension;
	else
		sSectionPostFix = fn.sSectionPostFix;
}

FileName::~FileName()
{
}

void FileName::Split(const char* sFileName)
{
// added support for quoted filenames: single quotes are removed
/*  general format: path\name.ext:postfix  
  sFileName can have the following forms:
  1. without quotes
  2. quotes around path and file name, without an extension and/or section post fix
      (quotes are first and last character of sFileName)
  3. quotes around path and file name with extension and/or section post fix
      (quotes are first character and character before the extension)
*/
	String s(const_cast<char*>(sFileName)); // avoid calling String(const char* sFormat..)
	
	bool fQuoted = s[0] == '\'';
	bool fAllQuoted = fQuoted && s[s.length()-1] == '\''; // no ext, no section postfix
	if (!fAllQuoted) { // check if there is a closing quote before the extension
		if (s.find_last_of("'.") < 0) { // if not assume option 2
			s &= '\'';
			fAllQuoted = true;
		}
	}
	bool fIsDir = s[s.length()] == '\\';

	char *pcExt = s.strrchrQuoted('.');
	char slash = '\\';
	if ( s.find('/') != string::npos)
		slash = '/';
	char *pcSlash = s.strrchrQuoted(slash);
	if (pcSlash != 0 && pcSlash > pcExt)
		pcExt = 0;  // the dot belongs to the path!

	if (pcExt != 0)  // Extension?
	{
		char *pcPostFix = strchr(pcExt, ':');
		if (pcPostFix != 0)    // ? ext:section postfix
		{
			sSectionPostFix = pcPostFix;
			*pcPostFix = '\0';
		}
		char *pcCol = s.strrchrQuoted('.', (int)(pcExt - s.c_str())-1);
		if (pcCol == 0)
			sExt = pcExt;
		else   // a column ID may be present
		{
			sCol = pcExt + 1; //first byte is the '.'
			*pcExt = '\0';
			if ( fIsIlwisExtension(pcCol) )
			{
				sExt = pcCol;
			}
			else
			{
				sExt = "." + sCol; // foreign format extensions
				sCol = "";
			}
		}
		int iColLength = sCol != "" ? sCol.length()+1 : 0;
		s = s.sLeft(s.length() - iColLength - sExt.length()-sSectionPostFix.length());
	}
/*
	else
	{
		sExt = "";
		char *pcPostFix = s.strrchrQuoted(':');
		if ((0 != pcPostFix) && (*(pcPostFix+1) != '\\')) { 
			sSectionPostFix = pcPostFix;
			s = s.sLeft(s.length()-sSectionPostFix.length());
		}
	}*/

	if (fQuoted)  // remove quotes
		s = s.sUnQuote();
	sDrive = "";
	pcExt = strrchr(s.sVal(), ':');
	if (pcExt != 0) {
		int len = pcExt - &s[0];
		if (len == 1)
		{
			sDrive &= s[0];
			sDrive &= s[1];
			s = s.sRight(s.length()-2);
		}
	}
	sDir = "";
	pcExt = strrchr(s.sVal(),slash);
	if (pcExt != 0)
	{
		int len = pcExt - &s[0] + 1;
		for (int i=0; i< len; i++)
			sDir &= s[i];
		s = s.sRight(s.length() - len);
	}
	sFile = s;
}

String FileName::sPath() const
{
	String sBuf;
//	if (sDrive.length() != 0) { // for UNC
		sBuf = sDrive;
		if (sDir.length() != 0) {
			sBuf &= sDir;
			if (sBuf[sBuf.length() - 1] != '\\')
				sBuf &= "\\";
		}
		else if (sBuf.length() > 0)
			sBuf &= '\\';
//		sBuf.toLower();
//	}
	return sBuf;
};

String FileName::sPhysicalPath() const{
  String sBuf;
  if (sFile.length()==0) 
    return String();
  sBuf = sPath();
  if (sBuf == "")  {
		sBuf = getEngine()->sGetCurDir();
  	if ((sBuf.length() > 0) && (sBuf[sBuf.length() - 1] != '\\'))
			sBuf &= "\\";
  }
  sBuf &= sFile;
  sBuf &= sExt;
  return sBuf;
}

String FileName::sFullPath(bool fExt) const
{
  String sBuf;
  if (sFile.length()==0) 
    return String();
  sBuf = sPath();
  if (sBuf == "")  {
		sBuf = getEngine()->sGetCurDir();
  	if ((sBuf.length() > 0) && (sBuf[sBuf.length() - 1] != '\\'))
			sBuf &= "\\";
  }
  sBuf &= sFile;
  if (fExt)
	{
    sBuf &= sExt;
	}
	if ( sCol != "")
		sBuf &= "." + sCol.sQuote(false);
  return sBuf;
};

String FileName::sShortName(bool fExt) const
{
	String sBuf= sPath();
	IlwisAppContext* ilwapp = getEngine()->getContext();
	String sDirComp = getEngine()->sGetCurDir();
	String sStdDir = getEngine()->getContext()->sStdDir();
	if (sDirComp[sDirComp.length()-1] != '\\')
		sDirComp &= '\\';
	if (sStdDir[sStdDir.length()-1] != '\\')
		sStdDir &= '\\';
	if ((fCIStrEqual(sBuf, sDirComp)) || (fCIStrEqual(sBuf, sStdDir)))
		sBuf = "";
	sBuf &= sFile;
	if (fExt)
	{
		sBuf &= sExt;
	}
//	sBuf.toLower();
	if ( sCol != "")
		sBuf &= "." + sCol.sQuote(false);
	return sBuf;
};

FileName::FileName(const String& sName, const FileName& fn)
{
  if (sName.length() == 0)  return;

  Split(const_cast<char *>(sName.c_str()));

  if ((sDrive.length()==0) && (sDir.length()==0)) {
		String sPath = fn.sPath();
		if ("" == sPath)
			sPath = getEngine()->sGetCurDir();
    Dir(sPath);
  }
  else { // complete path to full path
    char sFullPath[MAX_PATH];
    String sPath = sDrive+sDir;
    _fullpath(sFullPath, sPath.c_str(), MAX_PATH);
    Dir(sFullPath);
  }
}


void FileName::Dir(const String& s)
{
  if (s.length() == 0) {
    sDir = String();
    sDrive = String();
    return;
  }  
  char * c = const_cast<char *>(s.c_str());
  char* tmp = strrchr(c, ':');
  if (tmp == NULL)
    sDir = s;
	else {
    sDrive = String("%c:", c[0]);
    ++tmp;
    if (0 != *tmp)
      sDir = tmp;
    else
      sDir = "\\";
  }
}

bool FileName::fExist(bool checkSystemToo) const 
{

	bool ok = _access(sPhysicalPath().c_str(), 0)==0 ; 
	if ( !ok && checkSystemToo) {
		String sStdDir = getEngine()->getContext()->sStdDir() + "\\System\\";
		String sysFile = sStdDir + sFile + sExt;
		if((ok = (_access(sysFile.c_str(), 0)!=0))) {
			sysFile = sStdDir + "\\System\\Basemaps\\" + sFile + sExt;
			return _access(sysFile.c_str(), 0)==0;
		}
	}
	return ok;
}

bool FileName::makePath(bool checkSystemToo)  
{

	bool ok = _access(sPhysicalPath().c_str(), 0)==0 ; 
	if ( !ok) {
		if ( sDrive == "" && sDir == "") {
			String curdir = getEngine()->sGetCurDir();
			String file = curdir + "\\" + sFile + sExt;
			ok = _access(file.c_str(), 0)==0;
			if ( ok) {
				*this = FileName(file);
				return ok;
			}
		}
		if ( checkSystemToo) {
			String sStdDir = getEngine()->getContext()->sStdDir();
			String sysFile = sStdDir + "\\"+ sFile + sExt;
			if((ok = (_access(sysFile.c_str(), 0)!=0))) {
				sysFile = sStdDir + "\\Basemaps\\" + sFile + sExt;
				ok =  _access(sysFile.c_str(), 0)==0;
				if (ok) {
					FileName fnTemp(sysFile);
					*this = fnTemp;
					return ok;
				}
			} else {
				FileName fnTemp(sysFile);
				*this = fnTemp;
				return true;
			}
		}
	}
	return ok;
}

bool FileName::fValid() const
{
	String s = sFullPath();
	bool fTooLong = (s.length() > (short)(MAX_PATH - 5));
	if (sFile.length() == 0 || fTooLong)
		return false;

	if ( sFile == "\'")
		return false;

	char c;

	for (unsigned int i = 0; i < sFile.length(); i++) 
	{
		c = sFile[i];
		if ((0 != strchr(pcIllegalChars, c)) || ((c >= 0) && (c <= 31)) ) 
			return false;
	}

	return true;
}

// create a unqiue old fashioned 8.3 filename
// this is needed for export to Ilwis 2.2 and older
FileName FileName::fnUniqueShort(const FileName& fnTemplate)
{
	FileName fn = fnTemplate;
	
	if ((fn.sDrive.length()==0) || (fn.sDir.length()==0))
		// set drive and path from current drive an path
		fn.Dir(getEngine()->sGetCurDir());
	FileName fnSys;
	fnSys = fn;
	fnSys.Dir(getEngine()->sGetCurDir());
	
	if (fn.sFile.length() != 0)
	{
		// add numbers to end of the file name to make it unique
		int iChangeInd = 7;  // index in string to be changed
		if (fn.sFile.length() != 8)
			for (int j = fn.sFile.length() + 1; j <= 8; j++)
				fn.sFile &= '0';
		else  // find last non numerical character
		{
			int j=7;
			for (; j >= 0; j--)
				if (!isdigit((unsigned char)fn.sFile[j]))
					break;
			iChangeInd = j;
		}
		fn.sExt = fn.sExt.sLeft(4);  // truncate extension

	    long i=0;
		String sNum;
		while (i++ < 10000000L)
		{
			sNum = String("%li", i);
			fn.sFile = fn.sFile.sLeft(8 - sNum.length());
			
			fn.sFile &= sNum;
			if (!File::fExist(fn))
			{
				fnSys.sFile = fn.sFile;
				if (!File::fExist(fnSys)) 
					return fn;
			}
		}
	}

	// if all else fails, create unique file name using proces id
	long iProcess = getEngine()->getContext()->iProcess();
	fn.sFile = String("uniq%04li", iProcess);
	while (File::fExist(fn))  // increment number
		fn.sFile = String("uniq%04li", (++iProcess) % 10000);
	
	return fn;
}

FileName FileName::fnUnique(const FileName& fnTemplate)
{
  FileName fn = fnTemplate;

  if ((fn.sDrive.length()==0) || (fn.sDir.length()==0))
    // set drive and path from current drive an path
    fn.Dir(getEngine()->sGetCurDir());
  FileName fnSys;
  fnSys = fn;
  fnSys.Dir(getEngine()->sGetCurDir());

  if (fn.sFile.length()!=0) {

    if (!File::fExist(fn)) {
      fnSys.sFile = fn.sFile;
      if (!File::fExist(fnSys)) 
        return fn;
    }  

    // add numbers to end of the file name to make it unique
/*    int iChangeInd = 7;  // index in string to be changed
    if (fn.sFile.length() != 8)
      for (int j=fn.sFile.length()+1; j<=8; j++)
        fn.sFile &= '0';
    else { // find last non numerical character
      for (int j=7; j >= 0; j--)
        if (!isdigit((unsigned char)fn.sFile[j]))
          break;
      iChangeInd = j;
    }*/
    long i=0;
    String sNum;
    while (i++ < 10000000L) {
      sNum = String("__%li", i);
      //fn.sFile = fn.sFile.sLeft(8-sNum.length());
      fn.sFile = fnTemplate.sFile;
      fn.sFile &= sNum;
      if (!File::fExist(fn)) {
        fnSys.sFile = fn.sFile;
        if (!File::fExist(fnSys)) 
          return fn;
      }  
    }
  }

  // create unique file name using proces id
  long iProcess = getEngine()->getContext()->iProcess();
  fn.sFile = String("uniq%04li", iProcess);
  while (File::fExist(fn))  // increment number
    fn.sFile = String("uniq%04li", (++iProcess) % 10000);
  return fn;
}

FileName FileName::fnUniqueWithIndex(const FileName& fnTemplate, int iStartIndex)
{
  FileName fn = fnTemplate;

  if ((fn.sDrive.length()==0) || (fn.sDir.length()==0))
    // set drive and path from current drive an path
    fn.Dir(getEngine()->sGetCurDir());

  long i=iStartIndex;
  do
	{
    String sNum = String("__%li", i++);
    fn.sFile = fnTemplate.sFile;
    fn.sFile &= sNum;
  } while (File::fExist(fn));
	
	return fn;
}

String FileName::sRelative(bool fExt, const String& sDirRelative) const
{
  if (sFile.length() == 0)
    return sFile;
  String sDirComp = sDirRelative;
  if (sDirComp.length() == 0) {
    sDirComp = getEngine()->sGetCurDir();
    if (sDirComp[sDirComp.length()-1] != '\\')
      sDirComp &= '\\';
  }
  String sPth = sPath();
  String sStdDir = getEngine()->getContext()->sStdDir();
  sStdDir &= '\\';
  String basemaps = sStdDir + "Basemaps\\";
  if (sPth != ""){
		if (!fCIStrEqual(sPth, sDirComp) && !fCIStrEqual(sPth, sStdDir) && !fCIStrEqual(sPth, basemaps))
			return sFullPath(fExt);
  }
  if (fExt) {
    String sBuf = sFile;
    sBuf &= sExt;
		if ( sCol != "")
			sBuf &= "." + sCol.sQuote(false);
    return sBuf;
  }
  else
	{
		String sBuf = sFile;
		if ( sCol != "")
			sBuf &= "." + sCol.sQuote(false);
    return sBuf;
	}
}

FileName FileName::fnPathOnly(const String& sPath) {
  FileName fn;
  fn.Dir(sPath);
  return fn;
}

void FileName::FileSet(Array<String>& sFileSet)
{
  HANDLE findHandle;
  WIN32_FIND_DATA findData;
  if ((findHandle = FindFirstFile(sFullPath().c_str(),&findData)) != INVALID_HANDLE_VALUE )
  { 
    do 
    {
      String s(findData.cFileName);
			FileName fnNew(s);

			if (s == "." || s == "..")
				continue;
			if (sExt == ".*" || fCIStrEqual(sExt, fnNew.sExt))
				sFileSet &= s;
    } 
		while (FindNextFile(findHandle, &findData)==TRUE);
   }
   FindClose(findHandle);
}

bool FileName::operator == (const FileName& fn) const
{
  if (!fCIStrEqual(sExt, fn.sExt) ) return false;
  if (!fCIStrEqual(sFile, fn.sFile)) return false;
  if (!fCIStrEqual(sSectionPostFix, fn.sSectionPostFix)) return false;
  String s1 = sDir;
  int iLen1 = s1.length();
  if (s1.length() > 0 && s1[iLen1-1] != '\\')
    s1 &= "\\";
  String s2 = fn.sDir;
  int iLen2 = s2.length();
  if (s2.length() > 0 && s2[iLen2-1] != '\\')
    s2 &= "\\";
  if (!fCIStrEqual(s1, s2)) return false;
  if (!fCIStrEqual(sDrive, fn.sDrive)) return false;
  if (!fCIStrEqual(sCol, fn.sCol)) return false;
  return true;
}

void AddFileName(const FileName& fn, Array<FileName>& afn, const String& sPath)
{
  if (!fn.fValid())
    return;
  for (unsigned short i=0; i < afn.iSize(); ++i)
    if (fCIStrEqual(fn.sFullName(), afn[i].sFullName()))
      return;
  if (sPath.length() != 0) {
    FileName fnx(fn);
    fnx.Dir(sPath);
    for (unsigned int i=0; i < afn.iSize(); ++i)
      if (fCIStrEqual(fnx.sFullName(), afn[i].sFullName()))
        return;
    afn &= fnx; 
  }
  else
    afn &= fn;
}

String FileName::sFullPathQuoted(bool fExt) const
{
	if (sCol.length() == 0) {
		String sBuf = sFullPath(false);
		sBuf = sBuf.sQuote(false);
		if (fExt)
			sBuf &= sExt;
		sBuf &= sSectionPostFix;
		return sBuf;
	}
	else {
  	FileName fn(*this);
	  fn.sCol = "";
		return String("%S.%S",  fn.sFullPathQuoted(fExt), sCol.sQuote(false));
	}
}

String FileName::sRelativeQuoted(bool fExt, const String& sDirRelative) const
{
  String sBuf = sRelative(false, sDirRelative);
	if (sCol != "")
	{
		char *p = sBuf.strrchrQuoted('.');
		sBuf = sBuf.substr(0, (unsigned int)(p - sBuf.c_str()));
	}
  sBuf = sBuf.sQuote(false);
  if (fExt)
    sBuf &= sExt;
	if ( sCol != "" )
		sBuf &= "." + sCol.sQuote(false);
	sBuf &= sSectionPostFix;
  return sBuf;
}

String FileName::sShortNameQuoted(bool fExt) const
{
  String sBuf = sShortName(false);
  sBuf = sBuf.sQuote(false);
  if (fExt)
    sBuf &= sExt;
	sBuf &= sSectionPostFix;

  return sBuf;
}

bool FileName::fIsIlwisExtension(const String& sExtension)
{
	return sAllExtensions().find(sExtension) != -1;
}

const String FileName::sAllExtensions()
{
	return ".mpr.mpa.mps.mpp.tbt.mpl.ioc.mpv.ilo.atx.grh.dom.rpr.grf.csy.his.hsa.hss.hsp.sms.stp.smc.ta2.mat.fil.fun.isl";
}

String FileName::sFileExt() const
{
	return sFile + sExt;
}

String FileName::sFileExtQuoted() const
{
  return sFile.sQuote(false) + sExt;
}

bool FileName::fIsIlwisDataFile() const
{
	if (sExt[sExt.size() - 1 ] == '#')
		return true;
	if ( sExt[sExt.size() - 1 ] == '%' )
		return true;
	if ( fCIStrEqual(sExt, ".isf") || fCIStrEqual(sExt, ".fnd"))
		return true;

	return false;
}

bool FileName::fUsesAbsolutePath(const String& sFileName)
{
	if ( sFileName.size() < 3 )
		return false;	
	if ( sFileName[1] == ':' )
		return true;
	if ( sFileName[0] == '\\'  && sFileName[1] == '\\' )
		return true;

	return false;
}

bool FileName::fUsesNoPath(const String& sFileName)
{
	FileName fn(sFileName);
	String sFilePart = fn.sFileExt();
	if ( sFileName == sFilePart )
		return true;
	return false;
}

bool FileName::fUsesRelativePath(const String& sFileName)
{
	if ( fUsesAbsolutePath(sFileName))
		return false;
	int iWhere = sFileName.find('\\');
	if ( iWhere != string::npos )
		return true;

	return false;
}

bool FileName::fIsContainerFor(const FileName& fnCont, const String& sExtensions)
{
	String sFileType = fnCont.sExt;
	sFileType.toLower();
	if (sFileType.length() == 0)
		return false;

	// ObjectCollection can contain any object, including itself
	if (sFileType == ".ioc")
		return true;

	// Maplist can contain only raster maps (".mpr" or ".mpl:nn")
	if (sFileType == ".mpl")
	{
		String sExtLoc = sExtensions;
		sExtLoc.toLower();
		return (sExtLoc.find(".mpr") != String::npos ||
			sExtLoc.find(".mpl") != String::npos);
	}
	return false;
}

bool _export FileName::fIsContainer(const FileName& fnCont)
{
	String sExt = fnCont.sExt;
	sExt.toLower();
	return String(".mpl.ioc").find(sExt) != String::npos;
}
