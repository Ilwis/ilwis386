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
// MapListWindow.cpp : implementation file
//

#include "Client\Headers\formelementspch.h"
#include "Headers\messages.h"
#include "Client\Base\IlwisDocument.h"
#include "Headers\constant.h"
#include "Client\MainWindow\Catalog\CatalogFrameWindow.h"
#include "Client\MainWindow\CommandCombo.h"
#include "Client\MainWindow\mainwind.h"
#include "Client\ilwis.h"
#include "Client\MainWindow\Catalog\ObjectCollectionWindow.h"
#include "Client\MainWindow\Catalog\MapListWindow.h"
#include "Client\Editors\Utils\OwnerHeaderCtrl.h"
#include "Client\MainWindow\Catalog\Catalog.h"
#include "Client\MainWindow\Catalog\CollectionCatalog.h"
#include "Client\MainWindow\Catalog\MapListCatalog.h"
#include "Headers\Htp\Ilwis.htp"
#include "Client\MainWindow\AddInfoView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// MapListWindow

IMPLEMENT_DYNCREATE(MapListWindow, ObjectCollectionWindow)

BEGIN_MESSAGE_MAP(MapListWindow, ObjectCollectionWindow)
	//{{AFX_MSG_MAP(MapListWindow)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

MapListWindow::MapListWindow()
{
	htpTopic = htpMapListObject;
	sHelpKeywords = "Map lists";
	SetWindowName("MapList Window");	// ensure loading and saving settings
}

MapListWindow::~MapListWindow()
{
}

int MapListWindow::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	ObjectCollectionWindow::OnCreate(lpCreateStruct);
	EnableDocking(CBRS_ALIGN_ANY);
	bbMapList.Create(this, "maplist.but", "", 100);
	DockControlBar(&bbMapList, AFX_IDW_DOCKBAR_TOP);
	return 0;
}

BOOL MapListWindow::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	wndSplitter.CreateStatic(this, 2, 1, WS_CHILD|WS_VISIBLE);
	CRect rct;
	GetClientRect(&rct);
	int iYSize = rct.Height();
	if (iYSize > 300)
		iYSize -= 150;
	else
		iYSize *= 0.7;
	wndSplitter.CreateView(0, 0, RUNTIME_CLASS(MapListCatalog), CSize(0, iYSize), pContext);
	wndSplitter.CreateView(1, 0, RUNTIME_CLASS(AddInfoView), CSize(0, 0), pContext);
	SetActiveView((CView*)wndSplitter.GetPane(0,0));
	return TRUE;
}

