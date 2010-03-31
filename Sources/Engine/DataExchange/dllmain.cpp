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

#include "Headers\stdafx.h"
#define DomainInfo ggDomainInfo

#undef DomainInfo

#pragma warning( disable : 4786 )

#include "Headers\toolspch.h"

#pragma warning( disable : 4715 )

#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Engine\SpatialReference\prj.h"
#include "Engine\Table\Col.h"
#include "Engine\DataExchange\hdfincludes\hdf.h"
#include "Engine\DataExchange\HDFReader.h"
#include "Engine\DataExchange\ForeignFormat.h"
#include "Engine\DataExchange\GeoGateway.h"
#include "Engine\DataExchange\ASTERFormat.h"
#include "Engine\DataExchange\ADOTable.h"
#include <set>
#include <afxdllx.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static AFX_EXTENSION_MODULE ForeignFormatDLL = { NULL, NULL };

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// Remove this if you use lpReserved
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("ForeignFormatDLL Initializing!\n");
		
		// Extension DLL one-time initialization
		if (!AfxInitExtensionModule(ForeignFormatDLL, hInstance))
			return 0;

		if ( ForeignFormat::fFunctionsInitialized() == false ) //  temporary location for init of the list of functions
		{
	//		ForeignFormat::AddStaticFunctions("GeoGateway", GeoGatewayFormat::CreateImportObject, GeoGatewayFormat::CreateQueryObject);
			ForeignFormat::AddStaticFunctions("ASTER", ASTERFormat::CreateImportObject, ASTERFormat::CreateQueryObject, ASTERFormat::sFormatASTER);			
			ForeignFormat::AddStaticFunctions("ADO", ADOTable::CreateImportObject, ADOTable::CreateQueryObject);						
		}

		new CDynLinkLibrary(ForeignFormatDLL);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("ForeignFormatDLL Terminating!\n");
		// Terminate the library before destructors are called
		AfxTermExtensionModule(ForeignFormatDLL);
	}
	return 1;   // ok
}
