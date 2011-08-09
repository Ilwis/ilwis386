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
// ForeignCollectionDoc.cpp : implementation file
//

#include "Client\Headers\formelementspch.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\MainWindow\Catalog\CatalogDocument.h"
#include "Engine\DataExchange\ForeignCollection.h"
#include "Client\MainWindow\Catalog\ObjectCollectionDoc.h"
#include "Client\MainWindow\Catalog\ForeignCollectionDoc.h"
#include "Client\MainWindow\CommandCombo.h"
#include "Client\MainWindow\mainwind.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Engine\Base\File\Directory.h"
#include "Headers\Hs\Mainwind.hs"
#include "Client\ilwis.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ForeignCollectionDoc

IMPLEMENT_DYNCREATE(ForeignCollectionDoc, ObjectCollectionDoc)

BEGIN_MESSAGE_MAP(ForeignCollectionDoc, ObjectCollectionDoc)
END_MESSAGE_MAP()

ForeignCollectionDoc::ForeignCollectionDoc()
{
}

ForeignCollectionDoc::~ForeignCollectionDoc()
{
}

BOOL ForeignCollectionDoc::OnOpenDocument(LPCTSTR lpszPathName)
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
	oc = ForeignCollection(fn);
	if (!oc.fValid())
		return FALSE;

	return TRUE;
}

BOOL ForeignCollectionDoc::OnOpenDocument(LPCTSTR lpszPathName, ParmList& pm)
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
		throw ErrorObject(String(TR("Target location, %S,  is Read-Only").c_str(), dirOut.sPath()));
	
	if (IlwisObject::iotObjectType(fnFile) == IlwisObject::iotOBJECTCOLLECTION)
	{
		if (!IlwisDocument::OnOpenDocument(lpszPathName))
			return FALSE;

		SetPathName(fnFile.sPath().c_str());		
		oc = ForeignCollection(fnFile);
	}
	else
	{
		FileName fnIOC = FileName::fnUnique(FileName(fnFile.sFullPathQuoted(), ".ioc"));
		if ( dirOut.fValid())
			fnIOC.Dir(dirOut.sFullPath());
		ForeignCollection fc;
		SetPathName(fnFile.sPath().c_str());				
		if ( ForeignCollection::fForeignFileTimeChanged(fnFile, fnIOC)) 
		{
			SetCollectionPath(fnIOC, pm, "import"); // will do nothing if not present
			SetCollectionPath(fnIOC, pm, "output"); // will do nothing if not present
			fc= ForeignCollection(fnIOC, pm);
			fc->Create(pm);
			oc = fc;
		}
		else
			oc = ForeignCollection(fnIOC);

		String sTN = oc->sTypeName();
		SetTitle(sTN.c_str());		
	}
	// If no show is on, dont open this collection and destroy the related ObjectCollection
	if (pm.fExist("noshow") || !oc.fValid() || oc->fErase)
	{
		oc.SetPointer(0);
		return FALSE;
	}
	return TRUE;
}

void ForeignCollectionDoc::SetCollectionPath(FileName& fnIOC, ParmList& pm, const String& option )
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
			SetPathName(fnIOC.sPath().c_str());							
		}
	}	
}

IlwisObject ForeignCollectionDoc::obj() const
{
	return oc;
}

String ForeignCollectionDoc::sContainer() {
	return dynamic_cast<ForeignCollectionPtr *>(oc.ptr())->sContainer();
}


