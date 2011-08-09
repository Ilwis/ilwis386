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
// PixelInfoWindow.cpp : implementation file
//

#include "Client\Headers\formelementspch.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\ilwis.h"
#include "Engine\Base\DataObjects\ObjectCollection.h"
#include "Client\Base\datawind.h"
#include "Headers\constant.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\Mapwindow\PixelInfoDoc.h"
#include "Client\Mapwindow\PixelInfoWindow.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Headers\Htp\Ilwis.htp"
#include "Client\TableWindow\BaseTablePaneView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// PixelInfoWindow

IMPLEMENT_DYNCREATE(PixelInfoWindow, DataWindow)

BEGIN_MESSAGE_MAP(PixelInfoWindow, DataWindow)
	//{{AFX_MSG_MAP(PixelInfoWindow)
	ON_WM_CREATE()
	ON_COMMAND(ID_EXIT, OnClose)
	ON_COMMAND(ID_ALWAYSONTOP, OnAlwaysOnTop)
	ON_UPDATE_COMMAND_UI(ID_ALWAYSONTOP, OnUpdateAlwaysOnTop)
	ON_MESSAGE(ILW_UPDATE, OnUpdate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


PixelInfoWindow::PixelInfoWindow()
: fAlwaysOnTop(false)
{
  help = "ilwis\\pixel_information_window.htm";
	sHelpKeywords = "Pixel information window";
}

PixelInfoWindow::~PixelInfoWindow()
{
}



/////////////////////////////////////////////////////////////////////////////
// PixelInfoWindow message handlers

#define sMen(ID) ILWSF("men",ID).c_str()

#define add(ID) menPopup.AppendMenu(MF_STRING, ID, sMen(ID)); 
#define addBreak menPopup.AppendMenu(MF_SEPARATOR);
#define addMenu(ID) men.AppendMenu(MF_POPUP, (UINT)menPopup.GetSafeHmenu(), sMen(ID)); menPopup.Detach();
#define addSub(ID) menSub.AppendMenu(MF_STRING, ID, sMen(ID)); 
#define addSubMenu(ID) menPopup.AppendMenu(MF_POPUP, (UINT)menSub.GetSafeHmenu(), sMen(ID)); menSub.Detach();

int PixelInfoWindow::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (DataWindow::OnCreate(lpCreateStruct) == -1)
		return -1;
	
  CMenu men;
	CMenu menPopup;
	CMenu menSub;
  men.CreateMenu();

  menPopup.CreateMenu();
  add(ID_ADDMAPS);
  add(ID_ADDCSYS);
  add(ID_ADDGRF);
  addBreak;
  add(ID_FILE_PRINT);
//  add(ID_PRINTSETUP);
  addBreak;
  add(ID_EXIT);
  addMenu(ID_MEN_FILE);

  menPopup.CreateMenu();
  add(ID_COPY );
  add(ID_SELALL );
  addMenu(ID_MEN_EDIT);

  menPopup.CreateMenu();
  add(ID_PIXCONFIGURE);
  add(ID_ALWAYSONTOP);
  add(ID_STATUSLINE);
  addMenu(ID_MEN_OPTIONS);

  SetMenu(&men);
  menPopup.Detach();
  men.Detach();

	AddHelpMenu();
	SetAcceleratorTable();
	SetWindowName("PixelInfo");	

	return 0;
}


void PixelInfoWindow::OnAlwaysOnTop()
{
  fAlwaysOnTop = !fAlwaysOnTop;
  if (fAlwaysOnTop)
    SetWindowPos(&wndTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
  else
    SetWindowPos(&wndNoTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
}

void PixelInfoWindow::OnUpdateAlwaysOnTop(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(fAlwaysOnTop);
}

LRESULT PixelInfoWindow::OnUpdate(WPARAM wParam, LPARAM lParam)
{
	PixelInfoDoc* pid = (PixelInfoDoc*)GetActiveDocument();
	return pid->OnUpdate(wParam, lParam);
}

void PixelInfoWindow::InitSettings()
{
	DataWindow::InitSettings();
	BaseTablePaneView* vw = dynamic_cast<BaseTablePaneView*>(GetActiveView());
	vw->SetButWidth(20);
	CRect rect;
	GetWindowRect(rect);
	IlwisSettings settings("PixelInfo");
	if (!settings.fKeyExists("WindowPosition"))
	{
		rect.right = rect.left + vw->iChrWidth() * (vw->iButWidth() + 50);
		rect.bottom = rect.top + 20 * vw->iHeight();
		MoveWindow(rect.left, rect.top, rect.Width(), rect.Height());
	}		
}
