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
// LinearFilterWindow.cpp : implementation file
//

#include "Client\Headers\formelementspch.h"
#include "Client\Base\datawind.h"
#include "Headers\messages.h"
#include "Headers\constant.h"
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\Base\Res.h"
#include "Headers\Htp\Ilwis.htp"
#include "Client\Editors\LinearFilterWindow.h"
#include "Client\Forms\generalformview.h"
#include "Client\Editors\Utils\GainControl.h"
//#include "Client\Editors\LinFilGainFormView.h"
#include "Headers\Htp\Ilwismen.htp"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// LinearFilterWindow

IMPLEMENT_DYNCREATE(LinearFilterWindow, DataWindow)

BEGIN_MESSAGE_MAP(LinearFilterWindow, DataWindow)
	//{{AFX_MSG_MAP(LinearFilterWindow)
	ON_COMMAND(ID_EXIT, OnClose)
	ON_WM_CREATE()
	ON_COMMAND(ID_DESCRIPTIONBAR, OnDescriptionBar)
	ON_UPDATE_COMMAND_UI(ID_DESCRIPTIONBAR, OnUpdateDescriptionBar)
	ON_COMMAND(ID_GAINCONTROLBAR, OnGainControlBar)
	ON_UPDATE_COMMAND_UI(ID_GAINCONTROLBAR, OnUpdateGainControlBar)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

LinearFilterWindow::LinearFilterWindow()
{
	help = "ilwis\\ilwis_objects_filters.htm";
	sHelpKeywords = "Linear filter editor";
}

LinearFilterWindow::~LinearFilterWindow()
{
}

#define sMen(ID) ILWSF("men",ID).scVal()

#define add(ID) menPopup.AppendMenu(MF_STRING, ID, sMen(ID)); 
#define addBreak menPopup.AppendMenu(MF_SEPARATOR);
#define addMenu(ID) men.AppendMenu(MF_POPUP, (UINT)menPopup.GetSafeHmenu(), sMen(ID)); menPopup.Detach();
#define addSub(ID) menSub.AppendMenu(MF_STRING, ID, sMen(ID)); 
#define addSubMenu(ID) menPopup.AppendMenu(MF_POPUP, (UINT)menSub.GetSafeHmenu(), sMen(ID)); menSub.Detach();

int LinearFilterWindow::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (DataWindow::OnCreate(lpCreateStruct) == -1)
		return -1;

  CMenu men;
	CMenu menPopup;
	CMenu menSub;
  men.CreateMenu();

  menPopup.CreateMenu();
  add(ID_FILE_SAVE_COPY_AS);
  add(ID_PROP);
  add(ID_EXIT);
  addMenu(ID_MEN_FILE);

  menPopup.CreateMenu();
  add(ID_COPY);
  add(ID_PASTE);
  add(ID_EDIT);
  addBreak;
  add(ID_SELALL);
  addMenu(ID_MEN_EDIT);

  menPopup.CreateMenu();
	add(ID_GAINCONTROLBAR);
	add(ID_DESCRIPTIONBAR);
  add(ID_STATUSLINE);
  addMenu(ID_MEN_VIEW);

  SetMenu(&men);
  menPopup.Detach();
  men.Detach();

	AddHelpMenu();
	SetAcceleratorTable();
	
	return 0;
}

void LinearFilterWindow::InitialUpdate(CDocument* pDoc, BOOL bMakeVisible)
{
	descBar.Create(this, pDoc);
	gainBar.Create(this, pDoc);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&gainBar);
	DockControlBar(&descBar);
	CRect rect;
	GetWindowRect(&rect);
	MoveWindow(rect.left, rect.top, 400, 400);
	SetWindowName("LinearFilter Window");	// ensure loading and saving settings
	DataWindow::InitialUpdate(pDoc, bMakeVisible);
}

void LinearFilterWindow::OnDescriptionBar()
{
	if (!descBar.GetSafeHwnd())
		return;
	OnBarCheck(ID_DESCRIPTIONBAR);
}

void LinearFilterWindow::OnUpdateDescriptionBar(CCmdUI* cmd)
{
	if (descBar.IsWindowVisible())
		cmd->SetCheck(1);
	else
		cmd->SetCheck(0);
}

void LinearFilterWindow::OnGainControlBar()
{
	if (!gainBar.GetSafeHwnd())
		return;
	OnBarCheck(ID_GAINCONTROLBAR);
}

void LinearFilterWindow::OnUpdateGainControlBar(CCmdUI* cmd)
{
	if (gainBar.IsWindowVisible())
		cmd->SetCheck(1);
	else
		cmd->SetCheck(0);
}


