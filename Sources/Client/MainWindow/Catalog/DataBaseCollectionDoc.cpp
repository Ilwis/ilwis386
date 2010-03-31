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
// DataBaseCollectionDoc.cpp : implementation file
//

#include "Client\Headers\formelementspch.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\MainWindow\Catalog\CatalogDocument.h"
#include "Engine\Base\DataObjects\ObjectCollection.h"
#include "Engine\DataExchange\DatabaseCollection.h"
#include "Client\MainWindow\Catalog\ObjectCollectionDoc.h"
#include "Client\MainWindow\Catalog\DataBaseCollectionDoc.h"
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
// DataBaseCollectionDoc

IMPLEMENT_DYNCREATE(DataBaseCollectionDoc, ObjectCollectionDoc)

BEGIN_MESSAGE_MAP(DataBaseCollectionDoc, ObjectCollectionDoc)
END_MESSAGE_MAP()



DataBaseCollectionDoc::DataBaseCollectionDoc()
{
}

DataBaseCollectionDoc::~DataBaseCollectionDoc()
{
}

zIcon DataBaseCollectionDoc::icon() const
{
  return zIcon("IOC32ICON");
}

BOOL DataBaseCollectionDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	String sPath(lpszPathName);
	FileName fnn(lpszPathName);
	if ( fnn.sFile.sHead(":") == "!REGKEY")
	{
		sRegID = fnn.sFile.sTail(":");
		IlwisSettings settings(String("%S\\%S", MainWindow::sBaseRegKey(), sRegID));
		sPath = settings.sValue("DocName");
	}
	SetPathName(sPath.scVal());

	if (!IlwisDocument::OnOpenDocument(sPath.scVal()))
		return FALSE;

	FileName fn(sPath);
  oc = DataBaseCollection(fn);
	if (!oc.fValid())
		return FALSE;

	return TRUE;
}

IlwisObject DataBaseCollectionDoc::obj() const
{
	return oc;
}

BOOL DataBaseCollectionDoc::SaveModified()
{
	if ( IsModified())
		obj()->Store();

	return TRUE;
}

void DataBaseCollectionDoc::GetFileNames(vector<FileName>& vfn, vector<FileName>& vfnDsp)
{
	vfn.clear();
	vfnDsp.clear();
	if ( !oc.fValid() ) return;

	Catalog *cat = dynamic_cast<Catalog *>(wndGetActiveView());


	int iMax = oc->iNrObjects();
	for(int i = 0; i < iMax; ++i) 
	{
		FileName fn = oc->fnObject(i);
		String sTab, sView;
		FileName fnTab = fn;
		ObjectInfo::ReadElement("DataBaseCollection", "TableName", fn, sTab);
		ObjectInfo::ReadElement("DataBaseCollection", "TableType", fn, sView);		
		if ( sTab != "" && sView == "")
		{
			int iWhere = sTab.rfind('\\');
			if (iWhere != string::npos )
			{
				sTab = sTab.substr(iWhere + 1);
			}						
			fnTab = FileName(sTab, ".tbt");			
		}			

		vfnDsp.push_back(fnTab);
		vfn.push_back(fn);
	}
}

bool DataBaseCollectionDoc::fEditable() const
{
  return !oc->fReadOnly() && !oc->fDependent() && !oc->fUseAs();
}

void DataBaseCollectionDoc::AddObject(const FileName& fn)
{
	// for the moment only tables of the the type view can be added here
	ObjectInfo::WriteElement("DataBaseCollection", "TableType", fn, "View");
	oc->Add(fn);
	Catalog *cat = dynamic_cast<Catalog *>(wndGetActiveView());
	if (!cat) return;
	cat->FillCatalog();
	SetModifiedFlag(true);
}

void DataBaseCollectionDoc::RemoveObject(const FileName& fn)
{
	oc->Remove(fn);
	SetModifiedFlag(true);
}

String DataBaseCollectionDoc::sName() const
{
	return obj()->fnObj.sFullPath();
}

void DataBaseCollectionDoc::SaveSettings(IlwisSettings& settings,int iNr)
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

String DataBaseCollectionDoc::sAllowedTypes()
{
	return "*.tbt";
}

String DataBaseCollectionDoc::sGetPathName() const
{
	String sName(GetPathName());
	String sPath = FileName(sName).sPath();
	if ( sPath[sPath.size() - 1] != '\\')
			 sPath &= "\\";
	return FileName(sName).sPath();
}

String DataBaseCollectionDoc::sDataBase()
{
	return dynamic_cast<DataBaseCollectionPtr *>(oc.ptr())->sContainer();
}
