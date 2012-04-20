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
// CatalogDocument.cpp : implementation file
//

#include "Client\Headers\formelementspch.h"
#include "Client\Base\IlwisDocument.h"
#include "Engine\Base\System\Engine.h"
#include "Client\MainWindow\Catalog\CatalogDocument.h"
#include "Client\Editors\Utils\OwnerHeaderCtrl.h"
#include "Engine\Base\File\Directory.h"
#include "Client\MainWindow\Catalog\Catalog.h"
#include "Client\MainWindow\CommandCombo.h"
#include "Client\MainWindow\mainwind.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\MainWindow\DirectoryDocument.h"
#include "Engine\Base\DataObjects\URL.h"
#include "Headers\Hs\Mainwind.hs"
#include "Client\ilwis.h"
#include "Headers\constant.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DirectoryDocument

IMPLEMENT_DYNCREATE(DirectoryDocument, CatalogDocument)

BEGIN_MESSAGE_MAP(DirectoryDocument, CatalogDocument)
	//{{AFX_MSG_MAP(DirectoryDocument)
	ON_COMMAND(ID_DIRUP, OnDirUp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

DirectoryDocument::DirectoryDocument()
{
	Init();
}

DirectoryDocument::DirectoryDocument(LPCTSTR lpszPathName)
{
	OnOpenDocument(lpszPathName);
}

DirectoryDocument::~DirectoryDocument()
{
}

BOOL DirectoryDocument::OnOpenDocument(LPCTSTR lpszPathName) 
{
  char sBuf[_MAX_PATH];
	String sPath(lpszPathName);
	if ( sPath.sHead(":") == "!REGKEY")
	{
			sRegID = sPath.sTail(":");
			IlwisSettings settings(String("%S\\%S", MainWindow::sBaseRegKey(), sRegID));
			sPath = settings.sValue("DocName");
	}
	FileName ff(sPath);
	if (sPath != "") 
	{
		Directory dir(sPath);
    SetCurrentDirectory(dir.sFullPath().c_str());
	}
  GetCurrentDirectory(_MAX_PATH, sBuf);
  if ( URL::isUrl(sBuf))
	  return false;

  IlwWinApp()->SetCurDir(sBuf);
	IlwWinApp()->AddToCatalogHistory(sBuf);
  SetPathName(sBuf,FALSE);	
  SetTitle(sBuf);
//	UpdateAllViews(0);
  return TRUE;
}

class FnLess
{
public:
	bool operator()(const FileName& fn1, const FileName& fn2)
  {
		int iExt1 = iExtNr(fn1);
		int iExt2 = iExtNr(fn2);
		if (iExt1 == iExt2)
  		return (fn1.sFile < fn2.sFile);
		else
			return iExt1 < iExt2;
  }
private:
	int iExtNr(const FileName& fn) 
	{
		if ("drive" == fn.sExt)
			return 2100;
		else if ("directory" == fn.sExt)
			return 2000;
		else
			return 0;
	}
};

void DirectoryDocument::GetFileNames(vector<FileName>& vfn)
{
	vfn.clear();
	vector<NameExt>::const_iterator iter;
	CFileFind finder;
	FileName fn;
	vfn.resize(0);
	String sFile = GetPathName();
	int iWhere=0;
	if ( (iWhere = sFile.find("!REGKEY")) != -1 ) // initially (create) path is not correct, delete last part
	{
		sFile = sFile.substr(0, iWhere);
		SetPathName(sFile.c_str());
	}
	
	int iLastPos = sFile.length() - 1;
	if (sFile[iLastPos] == '\\')
		sFile += "file";
	else
		sFile += "\\file";
	fn = sFile.c_str();
	Catalog *cat = dynamic_cast<Catalog *>(this->wndGetActiveView());
	if (!cat) return;
	
	const vector<NameExt> &lsExt = cat->fil.GetNameExt();
	
	for(iter = lsExt.begin(); iter != lsExt.end(); ++iter)
	{
		const NameExt& ne = *iter; 
		fn.sExt = ne.sExt;
		if (ne.fShow) {
			string sMask = GetPathName();
			int iLastPos = sMask.length() - 1;
			if (sMask[iLastPos] == '\\')
				sMask += "*";
			else
				sMask += "\\*";
			if ("directory" == ne.sExt) {
				sMask += ".*";
				BOOL fFound = finder.FindFile(sMask.c_str());
				while (fFound) {
					fFound = finder.FindNextFile();
					if (!finder.IsDirectory())
						continue;
					if (finder.IsHidden())
						continue;
					if (finder.IsDots())
						continue;
					fn.sFile = (const char*)finder.GetFileName();
					vfn.push_back(fn);
				} 
				fn.sFile = "..";
				vfn.push_back(fn);
			}
			else if ("drive" == ne.sExt) {
				DWORD dwMask = GetLogicalDrives();
				DWORD dwDisk = 1;
				char sDrive[3] = "A:";
				for (int i = 0; i < 32; ++i) {
					if (dwDisk & dwMask) {
						fn.sFile = sDrive;
						vfn.push_back(fn);
					}
					dwDisk <<= 1;
					sDrive[0]++;
				}
			}
			else {
				sMask += ne.sExt;
				GetFileNames(vfn, sMask);
				FileName fnPath;
				FileName fnCur(sGetPathName());
				int i=0;
				while((fnPath = IlwWinApp()->fnGetSearchPath(i++)) != FileName() )
				{
					if ( fnCur != fnPath)
						GetFileNames(vfn, fnPath.sPath() + String("*%S" ,ne.sExt));
				}
			}  
		}
	}
	String sExternals = cat->sGetExternalFileExtensions();
	Array<String> arExtExt;
	Split(sExternals, arExtExt, ";");
	for(int iExt=0; iExt < arExtExt.size(); ++iExt)
	{
		String sMask = GetPathName();
		int iLastPos = sMask.length() - 1;
		if (sMask[iLastPos] == '\\')
			sMask += "*";
		else
			sMask += "\\*";			
		String sExt = arExtExt[iExt];
		if ('.' == sExt[0])
			sMask += String("%S", sExt);
		else if (sExt.find("*") == -1)		
			sMask += String (".%S", sExt);
		else
			sMask += sExt;
		
		
		GetFileNames(vfn, sMask);
	}
	if ( cat->getUseBaseMaps()) {
		String pathBase = getEngine()->getContext()->sStdDir() + "\\Basemaps\\";
		String mask = pathBase + "*.*";
		BOOL fFound = finder.FindFile(mask.c_str());
		while (fFound) {
			fFound = finder.FindNextFile();
			if (finder.IsDirectory())
				continue;
			if (finder.IsHidden())
				continue;
			if (finder.IsDots())
				continue;
			FileName fn  = FileName((const char*)finder.GetFileName());
			if ( IOTYPEBASEMAP(fn)) {
				fn = FileName(pathBase + fn.sFile + fn.sExt);
				vfn.push_back(fn);
			}
		} 

	}
	sort(vfn.begin(), vfn.end(), FnLess());
}

void DirectoryDocument::GetFileNames(vector<FileName>& vfn, const String& sMask)
{
	CFileFind finder;
	FileName fn(sMask);
	BOOL fFound = finder.FindFile(sMask.c_str());
	while (fFound) {
		fFound = finder.FindNextFile();
		if (finder.IsDirectory())
			continue;
		if (finder.IsHidden())
			continue;
		// finder returns also files with extensions with more than three chars, where first three chars match
		// so check explicitly on same extension:
		FileName fnNameExt = finder.GetFileName().GetBuffer(1); // avoid calling String(const char* sFormat..)
		if (fnNameExt.sExt != fn.sExt)
			continue;
		fn.sFile = finder.GetFileTitle().GetBuffer(1); // avoid calling String(const char* sFormat..)
		vfn.push_back(fn);
	}
}

String DirectoryDocument::sName() const
{
	return GetPathName();
}

zIcon DirectoryDocument::icon() const
{
  return zIcon("dir32ico");
}

void DirectoryDocument::OnDirUp()
{
	OnOpenDocument("..");
	UpdateAllViews(NULL);
}

// adapted from CDocument::SetPathName()
void DirectoryDocument::SetPathName(LPCTSTR lpszPathName, BOOL bAddToMRU)
{
	if (0 == lpszPathName || 0 == *lpszPathName || '!' == *lpszPathName)
		return;
	m_strPathName = lpszPathName;
	m_bEmbedded = FALSE;
	SetTitle(lpszPathName);
}

