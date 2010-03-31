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
// MatrixWindow.cpp : implementation file
//

#include "Headers\toolspch.h"
#include "Client\Base\datawind.h"
#include "Client\MainWindow\AddInfoView.h"
#include "Headers\messages.h"
#include "Headers\constant.h"
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Client\Base\Res.h"
#include "Headers\Htp\Ilwismen.htp"
#include "Client\Editors\MatrixWindow.h"
#include "Headers\Htp\Ilwis.htp"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// MatrixWindow

IMPLEMENT_DYNCREATE(MatrixWindow, DataWindow)

BEGIN_MESSAGE_MAP(MatrixWindow, DataWindow)
	//{{AFX_MSG_MAP(MatrixWindow)
	ON_COMMAND(ID_EXIT, OnClose)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

MatrixWindow::MatrixWindow()
{
	htpTopic = htpMatrixObject;
	sHelpKeywords = "Matrices";
	SetWindowName("Matrix Window");	// ensure loading and saving settings
}

MatrixWindow::~MatrixWindow()
{
}


/////////////////////////////////////////////////////////////////////////////
// MatrixWindow message handlers

#define sMen(ID) ILWSF("men",ID).scVal()

#define add(ID) menPopup.AppendMenu(MF_STRING, ID, sMen(ID)); 
#define addBreak menPopup.AppendMenu(MF_SEPARATOR);
#define addMenu(ID) men.AppendMenu(MF_POPUP, (UINT)menPopup.GetSafeHmenu(), sMen(ID)); menPopup.Detach();
#define addSub(ID) menSub.AppendMenu(MF_STRING, ID, sMen(ID)); 
#define addSubMenu(ID) menPopup.AppendMenu(MF_POPUP, (UINT)menSub.GetSafeHmenu(), sMen(ID)); menSub.Detach();


int MatrixWindow::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (DataWindow::OnCreate(lpCreateStruct) == -1)
		return -1;
	
  CMenu men;
	CMenu menPopup;
	CMenu menSub;
  men.CreateMenu();

  menPopup.CreateMenu();
  add(ID_PROP);
  add(ID_EXIT);
  addMenu(ID_MEN_FILE);

  menPopup.CreateMenu();
  add(ID_COPY );
  add(ID_SELALL);
  addMenu(ID_MEN_EDIT);

  menPopup.CreateMenu();
  add(ID_STATUSLINE);
  addMenu(ID_MEN_VIEW);

  SetMenu(&men);
  menPopup.Detach();
  men.Detach();

	AddHelpMenu();
	SetAcceleratorTable();

	return 0;
}

BOOL MatrixWindow::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	wndSplitter.CreateStatic(this, 2, 1, WS_CHILD|WS_VISIBLE);
	wndSplitter.CreateView(1, 0, pContext->m_pNewViewClass, CSize(100, 500), pContext);
	wndSplitter.CreateView(0, 0, RUNTIME_CLASS(AddInfoView), CSize(500,100), pContext);
	SetActiveView((CView*)wndSplitter.GetPane(1,0));
	return TRUE;
}

