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
#include "Headers\toolspch.h"
#include "Engine\Base\File\Directory.h"
#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\Engine.h"

Directory::Directory(const String& sPath) :
	FileName(sPath)
{
	sDir += sFile + sExt;		
	sFile="";
	sExt="";
}

Directory::Directory(const FileName& fnPath) :
	FileName(fnPath)
{
	sFile = "";
	sExt = "";
}


String Directory::sFullPath(bool fExt) const
{
  String sBuf;
  sBuf = sPath();
  if (sBuf == "")  {
		sBuf = getEngine()->sGetCurDir();
  	if ((sBuf.length() > 0) && (sBuf[sBuf.length() - 1] != '\\'))
			sBuf &= "\\";
  }
  return sBuf;	
}

bool Directory::fValid() const
{
	for (unsigned int i = 1; i < sDir.length(); i++) 
	{
		char c = sDir[i];
		if ((0 != strchr("*?\"<>|%", c)) || ((c >= 0) && (c <= 31)) ) 
			return false;
	}	
	return true;
}


String Directory::sRelative(const String& sDirRelative) const
{
  String sDirComp = sDirRelative;
  if (sDirComp.length() == 0) {
    sDirComp = getEngine()->sGetCurDir();
    if (sDirComp[sDirComp.length()-1] != '\\')
      sDirComp &= '\\';
  }
  String sPth = sPath();
  String sStdDir = getEngine()->sGetCurDir();
  sStdDir &= '\\';
  if (!fCIStrEqual(sPth, sDirComp) && !fCIStrEqual(sPth, sStdDir))
    return sFullPath();

	return "";
}

String Directory::sPath() const
{
	return FileName::sPath();
}

String Directory::sFullPathQuoted() const
{
	return FileName::sFullPathQuoted();
}

String Directory::sDriveName() const
{
	return sDrive;
}

bool Directory::fExist() const
{
	return FileName::fExist();
}

bool Directory::operator==(const Directory& dir) const
{
	return FileName::operator==(dir);
}

bool Directory::fIsExistingDir(const Directory& dir)
{
	DWORD attr = GetFileAttributes(dir.sFullPath().scVal());
	if (attr == 0xFFFFFFFF) // the non-existing constant INVALID_FILE_ATTRIBUTES
		return false; // 'dir' does not exist
	attr &= FILE_ATTRIBUTE_DIRECTORY;
	bool fRet = attr == FILE_ATTRIBUTE_DIRECTORY;
	return fRet;
}

bool Directory::fReadOnly() const
{
	String s("%S\\702v2as.ypr@", sFullPath());
	HANDLE hH = CreateFile(s.scVal(), 0, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	bool fR = hH == INVALID_HANDLE_VALUE;
	CloseHandle(hH);
	DeleteFile(s.scVal());
	return fR;
}
