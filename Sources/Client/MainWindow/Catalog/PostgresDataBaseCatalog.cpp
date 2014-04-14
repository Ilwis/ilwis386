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
#include "Client\MainWindow\Catalog\Catalog.h"
#include "Client\MainWindow\Catalog\ObjectCollectionDoc.h"
#include "Client\MainWindow\Catalog\CollectionCatalog.h"
#include "Client\MainWindow\Catalog\DataBaseCatalog.h"
#include "Client\MainWindow\Catalog\DataBaseCollectionDoc.h"
#include "Client\MainWindow\Catalog\PostgresDataBaseCatalog.h"
#include "Client\ilwis.h"
#include "Headers\messages.h"

IMPLEMENT_DYNCREATE(PostgresDataBaseCatalog, DataBaseCatalog)

BEGIN_MESSAGE_MAP(PostgresDataBaseCatalog, DataBaseCatalog)
	//{{AFX_MSG_MAP(DataBaseCatalog)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void PostgresDataBaseCatalog::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;
	NMLISTVIEW* nmlv = (NMLISTVIEW*) pNMHDR;
	int iItem = nmlv->iItem;
	if (iItem == -1)
		return;
	iItem = viIndex[iItem];
	FileName& fn = vfn[iItem];
	DataBaseCollectionDoc *doc = GetDocument();
	String sDBName = doc->obj()->fnObj.sRelative();
	FileName fnObj = doc->obj()->fnObj;
	String sMethod;
	ObjectInfo::ReadElement("ForeignFormat","method", fnObj, sMethod);

	String sCmd;
	if (fn.fExist()) // just open it
		sCmd = String("open %S", fn.sRelativeQuoted());
	else // first time, force implicit object open
		sCmd = String("open %S\\%S -method=%S", sDBName.sQuote(), fn.sRelativeQuoted(), sMethod);
	
	char* str = sCmd.sVal();
	IlwWinApp()->GetMainWnd()->SendMessage(ILWM_EXECUTE, 0, (LPARAM)str);	
}

void PostgresDataBaseCatalog::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	FileName fn, fnFileSub;
	int iNr, iSub;
	bool fReport;
	CalcMenuProps(pWnd, point, fn, fnFileSub, iNr, iSub, fReport);

	try
	{
		IlwWinApp()->ShowPopupMenu(this, point, fn, &assSel);	
	}
	catch (...)
	{
		// silent catch: CreateTable already displays a message
	}
}