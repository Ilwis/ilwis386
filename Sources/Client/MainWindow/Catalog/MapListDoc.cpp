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
// MapListDoc.cpp : implementation file
//

#include "Client\Headers\formelementspch.h"
#include "Client\Base\IlwisDocument.h"
#include "Engine\Base\DataObjects\ObjectCollection.h"
#include "Client\MainWindow\Catalog\CatalogDocument.h"
#include "Client\MainWindow\Catalog\ObjectCollectionDoc.h"
#include "Client\MainWindow\Catalog\MapListDoc.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\Editors\Utils\OwnerHeaderCtrl.h"
#include "Client\MainWindow\Catalog\Catalog.h"
#include "Client\MainWindow\CommandCombo.h"
#include "Client\MainWindow\mainwind.h"
#include "Headers\Hs\Mainwind.hs"
#include "Client\ilwis.h"
#include "Headers\constant.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// MapListDoc

IMPLEMENT_DYNCREATE(MapListDoc, CatalogDocument)

MapListDoc::MapListDoc()
{
}

MapListDoc::~MapListDoc()
{
}


BEGIN_MESSAGE_MAP(MapListDoc, CatalogDocument)
	//{{AFX_MSG_MAP(MapListDoc)
	ON_COMMAND(ID_FILE_OPENMPL_COLORCOMP, OnOpenColorComp)
	ON_COMMAND(ID_FILE_OPENMPL_FILM, OnOpenFilm)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


zIcon MapListDoc::icon() const
{
  return zIcon("MapListIcon");
}

BOOL MapListDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	String sPath(lpszPathName);
	FileName fnn(lpszPathName);	
	if ( fnn.sFile.sHead(":") == "!REGKEY")
	{
		sRegID = fnn.sFile.sTail(":");
		IlwisSettings settings(String("%S\\%S", MainWindow::sBaseRegKey(), sRegID));
		sPath = settings.sValue("DocName");
	}

	if (!CatalogDocument::OnOpenDocument(sPath.c_str()))
		return FALSE;

	FileName fn(sPath);
  mpl = MapList(fn);  
	if (!mpl.fValid())
		return FALSE;

	return TRUE;
}

IlwisObject MapListDoc::obj() const
{
	return mpl;
}

MapList MapListDoc::maplist() const
{
	return mpl;
}

String MapListDoc::sGetPathName() const
{
	String sName(GetPathName());
	String sPath = FileName(sName).sPath();
	if ( sPath[sPath.size() - 1] != '\\')
			 sPath &= "\\";
	return FileName(sName).sPath();
}

void MapListDoc::GetFileNames(vector<FileName>& vfn, vector<FileName>& vfnDsp)
{
	vfn.clear();
	vfnDsp.clear();
	for (int i = mpl->iLower(); i <= mpl->iUpper(); ++i) 
  {
    Map mp = mpl[i];
    if (!mp.fValid())
      return;
		String s = mpl->sDisplayName(mp->fnObj);
		FileName fn(s);
		fn.sExt = ".mpr";
		vfnDsp.push_back(fn);
		vfn.push_back(mp->fnObj);
	}
}

String MapListDoc::sName() const
{
	return obj()->fnObj.sFullPath();
}

void MapListDoc::SaveSettings(IlwisSettings& settings,int iNr)
{
	String sDocKey("%S\\Doc%d", settings.sIlwisSubKey(), iNr);
	POSITION pos = GetFirstViewPosition();
	int iNrView=0;
	while(pos)
	{
		IlwisSettings docsettings(sDocKey);
		docsettings.SetValue("DocName", sName());
		Catalog* cat = dynamic_cast<Catalog *>(GetNextView(pos));
		if ( cat)
			cat->SaveCatalogView(sDocKey, iNrView++);
	}
}

void MapListDoc::AddObject(const FileName& fn)
{
  if (!fEditable())
    return;
	mpl->AddMap(Map(fn));
}

void MapListDoc::RemoveObject(const FileName& fn)
{
  if (!fEditable())
    return;
	mpl->RemoveMap(fn);
}

bool MapListDoc::fEditable() const
{
  return !mpl->fReadOnly() && !mpl->fDependent() && !mpl->fUseAs();
}

String MapListDoc::sAllowedTypes()
{
	return ".mpr";
}

Domain MapListDoc::dm()
{
	if (mpl->iSize() > 0)
		return mpl[mpl->iLower()]->dm();
	else 
		return Domain();
}

GeoRef MapListDoc::gr()
{
	return mpl->gr();
}

RowCol MapListDoc::rcSize()
{
	return mpl->rcSize();
}

int MapListDoc::iFindIndex(const FileName& fn)
{
	int iLo = mpl->iLower();
	int iUpper = mpl->iUpper();
	for(int i=iLo; i <= iUpper; ++i)
		if (mpl[i]->fnObj == fn)
			return i;
	return iUNDEF;
}

void MapListDoc::MoveMap(int iNewIndex, const FileName& fn)
{
  if (!fEditable())
    return;
	mpl->MoveMap(iNewIndex, Map(fn));
}

void MapListDoc::OnOpenColorComp()
{
	String sCmd("colorcomp %S", mpl->fnObj.sFullNameQuoted());
	IlwWinApp()->Execute(sCmd);
}

void MapListDoc::OnOpenFilm()
{
	//String sCmd("slideshow %S", mpl->fnObj.sFullNameQuoted());
	String sCmd("open %S -animation", mpl->fnObj.sFullNameQuoted());
	IlwWinApp()->Execute(sCmd);
}
