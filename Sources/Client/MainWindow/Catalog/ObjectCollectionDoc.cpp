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
// ObjectCollectionDoc.cpp : implementation file
//

#include "Client\Headers\formelementspch.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\MainWindow\Catalog\CatalogDocument.h"
#include "Engine\Base\DataObjects\ObjectCollection.h"
#include "Client\MainWindow\Catalog\ObjectCollectionDoc.h"
#include "Client\MainWindow\CommandCombo.h"
#include "Client\MainWindow\mainwind.h"
#include "Client\Editors\Utils\OwnerHeaderCtrl.h"
#include "Client\MainWindow\Catalog\Catalog.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Headers\Hs\Mainwind.hs"
#include "Client\ilwis.h"
#include "Headers\constant.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ObjectCollectionDoc

IMPLEMENT_DYNCREATE(ObjectCollectionDoc, CatalogDocument)

BEGIN_MESSAGE_MAP(ObjectCollectionDoc, CatalogDocument)
	ON_COMMAND(ID_FILE_OPENMPL_FILM, OnOpenFilm)
END_MESSAGE_MAP()



ObjectCollectionDoc::ObjectCollectionDoc()
{
}

ObjectCollectionDoc::~ObjectCollectionDoc()
{
}

zIcon ObjectCollectionDoc::icon() const
{
  return zIcon("IOC32ICON");
}

BOOL ObjectCollectionDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	String sPath(lpszPathName);
	FileName fnn(lpszPathName);
	if ( fnn.sFile.sHead(":") == "!REGKEY")
	{
		sRegID = fnn.sFile.sTail(":");
		IlwisSettings settings(String("%S\\%S", MainWindow::sBaseRegKey(), sRegID));
		sPath = settings.sValue("DocName");
	}
	SetPathName(sPath.c_str());

	if (!IlwisDocument::OnOpenDocument(sPath.c_str()))
		return FALSE;

	FileName fn(sPath);
  oc = ObjectCollection(fn);
	if (!oc.fValid())
		return FALSE;

	return TRUE;
}

IlwisObject ObjectCollectionDoc::obj() const
{
	return oc;
}

BOOL ObjectCollectionDoc::SaveModified()
{
	if ( IsModified())
		obj()->Store();

	return TRUE;
}

void ObjectCollectionDoc::GetFileNames(vector<FileName>& vfn, vector<FileName>& vfnDsp)
{
	vfn.clear();
	vfnDsp.clear();
	if ( !oc.fValid() ) return;

	Catalog *cat = dynamic_cast<Catalog *>(wndGetActiveView());
	if (!cat) return;

	String sExtensions;
	vector<NameExt> &lsExt = cat->GetFilter().GetNameExt();
	for(unsigned int i= 0; i<lsExt.size(); ++i)
		if ( lsExt[i].fShow )
			sExtensions += String(lsExt[i].sExt);

	int iMax = oc->iNrObjects();
	for(int i = 0; i < iMax; ++i) 
	{
		FileName fn = oc->fnObject(i);
		if ( sExtensions.find(fn.sExt) == -1) continue;

		vfnDsp.push_back(fn);
		vfn.push_back(fn);
	}
}

bool ObjectCollectionDoc::fEditable() const
{
  return !oc->fReadOnly() && !oc->fDependent() && !oc->fUseAs();
}

void ObjectCollectionDoc::AddObject(const FileName& fn)
{
	if (!fEditable())
		return;
	oc->Add(fn);
	Catalog *cat = dynamic_cast<Catalog *>(wndGetActiveView());
	if (!cat) return;
	cat->FillCatalog();
	SetModifiedFlag(true);
}

void ObjectCollectionDoc::RemoveObject(const FileName& fn)
{
	if (!fEditable())
		return;
	oc->Remove(fn);
	SetModifiedFlag(true);
}

String ObjectCollectionDoc::sName() const
{
	return obj()->fnObj.sFullPath();
}

void ObjectCollectionDoc::SaveSettings(IlwisSettings& settings,int iNr)
{
	String sDocKey("%S\\Doc%d", settings.sIlwisSubKey(), iNr);
	POSITION pos = GetFirstViewPosition();
	int iNrView=0;
	while(pos)
	{
		IlwisSettings docsettings(sDocKey);
		docsettings.SetValue("DocName", sName());
		Catalog* cat = (Catalog *)GetNextView(pos) ;
		if ( cat)
			cat->SaveCatalogView(sDocKey, iNrView++);
	}
}

String ObjectCollectionDoc::sAllowedTypes()
{
	// anything goes;
	return IlwisObject::sAllExtensions();
}

String ObjectCollectionDoc::sGetPathName() const
{
	String sName(GetPathName());
	String sPath = FileName(sName).sPath();
	if ( sPath[sPath.size() - 1] != '\\')
			 sPath &= "\\";
	return FileName(sName).sPath();
}

bool ObjectCollectionDoc::fAlreadyInCollection(const FileName& fn) const
{
	return oc->fObjectAlreadyInCollection(fn);
}

void ObjectCollectionDoc::OnOpenFilm()
{
	//String sCmd("slideshow %S", mpl->fnObj.sFullNameQuoted());
	String sCmd("open %S -animation", oc->fnObj.sFullNameQuoted());
	IlwWinApp()->Execute(sCmd);
}
