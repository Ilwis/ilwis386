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
// LinearFilterDoc.cpp : implementation file
//

#include "Headers\toolspch.h"
#include "Headers\constant.h"
#include "Client\ilwis.h"
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Client\Base\Res.h"
#include "Client\Editors\LinearFilterDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// LinearFilterDoc

IMPLEMENT_DYNCREATE(LinearFilterDoc, FilterDoc)

BEGIN_MESSAGE_MAP(LinearFilterDoc, FilterDoc)
	//{{AFX_MSG_MAP(LinearFilterDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

LinearFilterDoc::LinearFilterDoc()
: fl(0)
{
}

LinearFilterDoc::~LinearFilterDoc()
{
}

BOOL LinearFilterDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	if (!FilterDoc::OnOpenDocument(lpszPathName))
		return FALSE;

	fl = dynamic_cast<FilterLinear*>(filter.ptr());
	if (0 == fl)
		return FALSE;

	return TRUE;
}




