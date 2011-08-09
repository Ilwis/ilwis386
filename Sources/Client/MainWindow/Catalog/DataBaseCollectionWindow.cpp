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
#include "Client\MainWindow\Catalog\ObjectCollectionWindow.h"
#include "Client\MainWindow\Catalog\DataBaseCollectionWindow.h"
#include "Client\MainWindow\CommandCombo.h"
#include "Client\MainWindow\mainwind.h"
#include "Client\MainWindow\Catalog\DataBaseCollectionWindow.h"
#include "Client\ilwis.h"
#include "Client\Base\Menu.h"
#include "Headers\Htp\Ilwis.htp"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DataBaseCollectionWindow

IMPLEMENT_DYNCREATE(DataBaseCollectionWindow, ObjectCollectionWindow)

BEGIN_MESSAGE_MAP(DataBaseCollectionWindow, ObjectCollectionWindow)
	//{{AFX_MSG_MAP(DataBaseCollectionWindow)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

DataBaseCollectionWindow::DataBaseCollectionWindow()
{
	help = "ilwis\\ilwis_objects_object_collections.htm";
	sHelpKeywords = "DataBase collections";
	SetWindowName("DataBaseCollection Window");	// ensure loading and saving settings
}

DataBaseCollectionWindow::~DataBaseCollectionWindow()
{
}


#define sMen(ID) ILWSF("men",ID).c_str()

void DataBaseCollectionWindow::OnUpdateFrameMenu(BOOL bActivate, CWnd* pActivateWnd, HMENU hMenuAlt)
{
	MainWindow *wnd = dynamic_cast<MainWindow *>(GetMDIFrame());
	if ( !wnd) return;
	Menu menu(wnd);
	if ( bActivate )
	{
		menu.InsertMenuItem(ID_OPEN_WITH_QUERY, ID_OPENPIXELINFO);	
	}
	else
	{
		menu.DeleteMenuItem(ID_OPEN_WITH_QUERY);
	}		
}

int DataBaseCollectionWindow::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (ObjectCollectionWindow::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

