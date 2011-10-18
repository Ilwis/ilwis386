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
// Table2Doc.cpp : implementation file
//

#include "Client\Headers\formelementspch.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\ilwis.h"
#include "Client\Base\datawind.h"
#include "Client\Editors\Utils\BaseBar.h"
#include "Client\Base\ButtonBar.h"
#include "Client\MainWindow\CommandCombo.h"
#include "Headers\constant.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\TableWindow\BaseTablePaneView.h"
#include "Engine\Table\tbl2dim.h"
#include "Engine\Base\System\LOGGER.H"
#include "Client\Base\BaseCommandHandlerUI.h"
#include "Client\MainWindow\CommandHandlerUI.h"
#include "Client\TableWindow\TableCommandHandler.h"
#include "Client\TableWindow\Table2Doc.h"
#include "Client\TableWindow\Table2PaneView.h"
#include "Client\TableWindow\Table2Window.h"
#include "Headers\Hs\Table.hs"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Table2Doc

IMPLEMENT_DYNCREATE(Table2Doc, IlwisDocument)

BEGIN_MESSAGE_MAP(Table2Doc, IlwisDocument)
	//{{AFX_MSG_MAP(Table2Doc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



Table2Doc::Table2Doc()

{
}

BOOL Table2Doc::OnNewDocument()
{
	if (!IlwisDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

Table2Doc::~Table2Doc()
{

}


/////////////////////////////////////////////////////////////////////////////
// Table2Doc diagnostics

#ifdef _DEBUG
void Table2Doc::AssertValid() const
{
	IlwisDocument::AssertValid();
}

void Table2Doc::Dump(CDumpContext& dc) const
{
	IlwisDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// Table2Doc serialization

void Table2Doc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// Table2Doc commands

const Table2Dim& Table2Doc::table2() const
{
  return tbl2;
}

IlwisObject Table2Doc::obj() const
{
  return tbl2;
}

BOOL Table2Doc::OnOpenDocument(LPCTSTR lpszPathName, int os) 
{
	if (!IlwisDocument::OnOpenDocument(lpszPathName))
		return FALSE;
  FileName fn(lpszPathName);
	Table2Dim table2(fn);
	return OnOpenDocument(table2);
}

BOOL Table2Doc::OnOpenDocument(const Table2Dim& table2)
{
	tbl2 = table2;
  if (!tbl2.fValid())
		return FALSE;
	String s = tbl2->sTypeName();
	if (s != tbl2->sDescription && "" != tbl2->sDescription)
		s = String("%S - %S", s, tbl2->sDescription);
  SetTitle(s.c_str());
	return TRUE;
}
 
zIcon Table2Doc::icon() const
{
  return zIcon("Table2Icon");
}

int Table2Doc::iCols() const
{
	if (!tbl2.fValid())
		return 1;
	return tbl2->iMatrixCols();
}

long Table2Doc::iRecs() const
{
	if (!tbl2.fValid())
		return 1;
	return tbl2->iMatrixRows();
}

