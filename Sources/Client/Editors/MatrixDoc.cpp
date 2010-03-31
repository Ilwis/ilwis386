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
// MatrixDoc.cpp : implementation file
//
#include "Headers\toolspch.h"
#include "Headers\constant.h"
#include "Client\ilwis.h"
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Engine\Matrix\Matrxobj.h"
#include "Client\Base\Res.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\Editors\MatrixDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// MatrixDoc

IMPLEMENT_DYNCREATE(MatrixDoc, IlwisDocument)

MatrixDoc::MatrixDoc()
{
}

MatrixDoc::~MatrixDoc()
{
}

BEGIN_MESSAGE_MAP(MatrixDoc, IlwisDocument)
	//{{AFX_MSG_MAP(MatrixDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL MatrixDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	String sFileExpr(lpszPathName);
	sFileExpr.toLower();
	if ( sFileExpr.find("matrix") == 0 )
	{
		matrix = MatrixObject(FileName(), sFileExpr);
		if (!matrix.fValid())
			return FALSE;
		IlwisDocument::OnOpenExpressionDoc(matrix);
		SetTitle(lpszPathName);
	}
	else
	{
		if (!IlwisDocument::OnOpenDocument(lpszPathName))
			return FALSE;
	  FileName fn(lpszPathName);
  
		matrix = MatrixObject(fn);
		if (!matrix.fValid())
			return FALSE;
	}

	matrix->GetColumnText(asCol);
	matrix->GetRowText(asRow);
  return TRUE;
}

IlwisObject MatrixDoc::obj() const
{
  return matrix;
}

MatrixObject MatrixDoc::mat() const
{
  return matrix;
}

zIcon MatrixDoc::icon() const
{
  return zIcon("MatrixIcon");
}

void MatrixDoc::MakeUsable() 
{
  matrix->MakeUsable();
}
