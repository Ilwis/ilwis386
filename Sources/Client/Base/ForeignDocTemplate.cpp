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
#include "Client\Headers\formelementspch.h"
#include "Client\ilwis.h"
#include "Engine\Table\tbl.h"
#include "Client\Base\BaseDocTemplate.h"
#include "Client\Base\IlwisDocTemplate.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\Base\Framewin.h"
#include "Client\MainWindow\Catalog\CatalogFrameWindow.h"
#include "Headers\Hs\Mainwind.hs"
#include "Client\Base\datawind.h"
#include "Client\MainWindow\Catalog\CatalogDocument.h"
#include "Client\Editors\Utils\OwnerHeaderCtrl.h"
#include "Client\MainWindow\Catalog\Catalog.h"
#include "Engine\Base\DataObjects\ObjectCollection.h"
#include "Client\MainWindow\Catalog\ObjectCollectionDoc.h"
#include "Client\MainWindow\Catalog\CollectionCatalog.h"
#include "Client\MainWindow\Catalog\ObjectCollectionWindow.h"
#include "Engine\Table\tbl.h"
#include "Engine\DataExchange\ForeignFormat.h"
#include "Engine\DataExchange\ForeignCollection.h"
#include "Client\MainWindow\Catalog\ForeignCollectionDoc.h"
#include "Client\Base\ForeignDocTemplate.h"
#include "Client\MainWindow\Catalog\ForeignCatalog.h"

BOOL AFXAPI AfxComparePath(LPCTSTR lpszPath1, LPCTSTR lpszPath2);

IMPLEMENT_DYNAMIC(ForeignMultiDocTemplate, IlwisMDIDocTemplate)

ForeignMultiDocTemplate::ForeignMultiDocTemplate() :	
   IlwisMDIDocTemplate(".ioc", 
			"Foreign Collection", 
			"ILWIS Foreign Collections",
			RUNTIME_CLASS(ForeignCollectionDoc),
			RUNTIME_CLASS(ObjectCollectionWindow),
			RUNTIME_CLASS(ForeignCatalog))
{
}

ForeignMultiDocTemplate::ForeignMultiDocTemplate( const char* pcType, const char* pcObjectName,  CRuntimeClass* pDocClass,
		CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass)
:	IlwisMDIDocTemplate(".ioc", pcType, pcObjectName, pDocClass, pFrameClass,	pViewClass)
{
}

ForeignMultiDocTemplate::~ForeignMultiDocTemplate()
{}

CDocTemplate::Confidence ForeignMultiDocTemplate::MatchDocType(LPCTSTR lpszPathName,
	CDocument*& rpDocMatch)
{
	POSITION pos = GetFirstDocPosition();
	while (pos != NULL)
	{
		CDocument* pDoc = GetNextDoc(pos);
		if (AfxComparePath(pDoc->GetPathName(), lpszPathName))
		{
			switch (MessageBox(0, TR("Map is already shown in a MapWindow.\nDo you wish to open the map in a new map window again?").c_str(), "ILWIS", MB_YESNOCANCEL|MB_ICONEXCLAMATION|MB_DEFBUTTON2|MB_TOPMOST))
			{
        case IDCANCEL:
				  return noAttempt;
        case IDNO:
					rpDocMatch = pDoc;
					return yesAlreadyOpen;
        case IDYES:
					pos = 0;
          break;
      }
		}
	}
	String type = "";
	FileName fnObj(lpszPathName);
	if ( fTypeOk(lpszPathName)) {
			//return yesAttemptForeign;
		return yesAttemptNative;
	}
	return noAttempt;
	
}

CDocTemplate::Confidence ForeignMultiDocTemplate::MatchDocType(const FileName& fnFO, CDocument*& rpDocMatch, const String& sMethod)
{
	POSITION pos = GetFirstDocPosition();
	ForeignFormat *ff = NULL;
	if ( IlwisObject::iotObjectType(fnFO) == IlwisObject::iotANY )
	{
		ff = ForeignFormat::Create(sMethod);
	}		
	while (pos != NULL)
	{
		CDocument* pDoc = GetNextDoc(pos);
		if (AfxComparePath(pDoc->GetPathName(), fnFO.sFullPath().c_str()))
		{
			switch (MessageBox(0, TR("Map is already shown in a MapWindow.\nDo you wish to open the map in a new map window again?").c_str(), "ILWIS", MB_YESNOCANCEL|MB_ICONEXCLAMATION|MB_DEFBUTTON2|MB_TOPMOST))
			{
        case IDCANCEL:
				  return noAttempt;
        case IDNO:
					rpDocMatch = pDoc;
					return yesAlreadyOpen;
        case IDYES:
					pos = 0;
          break;
      }
		}
	}
	if ( ff )
	{
		String sObjectType = sGetObjectType();
		bool fCanUse = ff->fMatchType(fnFO.sFullPath(), sObjectType);
		
		delete ff;
		if ( fCanUse )
		{
			return yesAttemptForeign;			
		}			
	}	
  return noAttempt;	
}
