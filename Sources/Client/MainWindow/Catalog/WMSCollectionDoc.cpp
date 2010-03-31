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
// WMSCollectionDoc.cpp : implementation file
//

#include "Client\Headers\formelementspch.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\MainWindow\Catalog\CatalogDocument.h"
#include "Engine\Base\DataObjects\URL.h"
#include "Engine\DataExchange\ForeignFormat.h"
#include "Engine\DataExchange\WMSCollection.h"
#include "Client\MainWindow\Catalog\ObjectCollectionDoc.h"
#include "Client\MainWindow\Catalog\WMSCollectionDoc.h"
#include "Engine\Base\File\Directory.h"
#include "Headers\Hs\Mainwind.hs"
#include "Client\ilwis.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// WMSCollectionDoc

IMPLEMENT_DYNCREATE(WMSCollectionDoc, ObjectCollectionDoc)

BEGIN_MESSAGE_MAP(WMSCollectionDoc, ObjectCollectionDoc)
END_MESSAGE_MAP()

WMSCollectionDoc::WMSCollectionDoc()
{
}

WMSCollectionDoc::~WMSCollectionDoc()
{
}

BOOL WMSCollectionDoc::OnOpenDocument(LPCTSTR lpszPathName, ParmList& pm)
{
	//ParmList pm(lpszPathName);

	FileName fnFile = pm.sGet(0);

	Directory dirOut(fnFile.sPath());
	FileName fnOut = fnFile;
	if (pm.fExist("output"))
	{
		fnOut = FileName(pm.sGet("output"));
		dirOut = Directory(fnOut.sPath());
	}

	if ( dirOut.fReadOnly() )
		throw ErrorObject(String(SMSErrorReadOnlyLocation_S.scVal(), dirOut.sPath()));
	
	if (IlwisObject::iotObjectType(fnFile) == IlwisObject::iotOBJECTCOLLECTION)
	{
		if (!IlwisDocument::OnOpenDocument(lpszPathName))
			return FALSE;

		SetPathName(fnFile.sPath().scVal());		
		oc = WMSCollection(fnFile);
	}
	else
	{
		FileName fnIOC = FileName::fnUnique(FileName(fnFile.sFullPathQuoted(), ".ioc"));
		if ( dirOut.fValid())
			fnIOC.Dir(dirOut.sFullPath());
		WMSCollection fc;
		SetPathName(fnFile.sPath().scVal());				
		//if ( WMSCollection::fWMSFileTimeChanged(fnFile, fnIOC)) 
		//{
		//	SetCollectionPath(fnIOC, pm, "import"); // will do nothing if not present
		//	SetCollectionPath(fnIOC, pm, "output"); // will do nothing if not present
		//	fc= WMSCollection(fnIOC, pm);
		//	fc->Create(pm);
		//	oc = fc;
		//}
		//else
			oc = WMSCollection(fnIOC);

		String sTN = oc->sTypeName();
		SetTitle(sTN.scVal());		
	}
	// If no show is on, dont open this collection and destroy the related ObjectCollection
	if (pm.fExist("noshow") || !oc.fValid() || oc->fErase)
	{
		oc.SetPointer(0);
		return FALSE;
	}
	return TRUE;
}

void WMSCollectionDoc::SetCollectionPath(FileName& fnIOC, ParmList& pm, const String& option )
{
	if (pm.fExist(option))
	{
		String sF = pm.sGet(option).sUnQuote();
		if ( sF != "")
		{
			if ( sF[ sF.size() - 1] == '\\' )
			{
				if ( !((sF[0] == '\\' && sF[1] == '\\') || sF[1] == ':')) // absolute path UNC or DOS
				{
					FileName dir(sF);
					sF = fnIOC.sPath() + dir.sDir + fnIOC.sFileExt();
				}	
				else
					sF = sF + fnIOC.sFileExt();
				fnIOC = FileName(sF);
			}
			SetPathName(fnIOC.sPath().scVal());							
		}
	}	
}

IlwisObject WMSCollectionDoc::obj() const
{
	return oc;
}


