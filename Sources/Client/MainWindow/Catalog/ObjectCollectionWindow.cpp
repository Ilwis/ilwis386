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
#include "Client\Base\datawind.h"
#include "Headers\messages.h"
#include "Client\Base\IlwisDocument.h"
#include "Headers\constant.h"
#include "Client\MainWindow\Catalog\CatalogFrameWindow.h"
#include "Client\MainWindow\CommandCombo.h"
#include "Client\MainWindow\mainwind.h"
#include "Client\MainWindow\Catalog\ObjectCollectionWindow.h"
#include "Client\ilwis.h"
#include "Client\Base\Menu.h"
#include "Headers\Htp\Ilwis.htp"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ObjectCollectionWindow

IMPLEMENT_DYNCREATE(ObjectCollectionWindow, CatalogFrameWindow)

BEGIN_MESSAGE_MAP(ObjectCollectionWindow, CatalogFrameWindow)
	//{{AFX_MSG_MAP(ObjectCollectionWindow)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

ObjectCollectionWindow::ObjectCollectionWindow()
{
	htpTopic = htpObjectCollection;
	sHelpKeywords = "Object collections";
	SetWindowName("ObjectCollection Window");	// ensure loading and saving settings
}

ObjectCollectionWindow::~ObjectCollectionWindow()
{
}


#define sMen(ID) ILWSF("men",ID).scVal()

void ObjectCollectionWindow::OnUpdateFrameMenu(BOOL bActivate, CWnd* pActivateWnd, HMENU hMenuAlt)
{
	MainWindow *wnd = dynamic_cast<MainWindow *>(GetMDIFrame());
	if ( !wnd) return;
	Menu menu(wnd);
	if ( bActivate )
	{
		menu.InsertMenuItem(ID_ADD_OBJECT, ID_CAT_DEL);
		menu.InsertMenuItem(ID_CAT_REMOVE, ID_CAT_DEL);
		menu.InsertMenuItem(ID_CAT_DEL2, ID_CAT_DEL); // accelarator differs with normal del
		menu.DeleteMenuItem(ID_CAT_DEL);
	}
	else
	{
		menu.InsertMenuItem(ID_CAT_DEL, ID_ADD_OBJECT);
		menu.DeleteMenuItem(ID_ADD_OBJECT);
		menu.DeleteMenuItem(ID_CAT_REMOVE);
		menu.DeleteMenuItem(ID_CAT_DEL2);
	}
}

int ObjectCollectionWindow::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CatalogFrameWindow::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void ObjectCollectionWindow::OnUpdateFrameTitle(BOOL bAddToTitle)
{
	CMDIChildWnd::OnUpdateFrameTitle(bAddToTitle); // pass to update of CatalogFrame
}
