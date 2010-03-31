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
/* DataWindow
// by Wim Koolhoven
// (c) Ilwis System Development ITC
	Last change:  WK   23 Dec 98   10:40 am
*/
#include "Headers\version.h"
#include "Client\Headers\formelementspch.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\Base\datawind.h"
//#include "Client\Base\ZappTools.h"
#include "Headers\messages.h"
#include "Client\Base\IlwisDocument.h"
#include "Headers\constant.h"

IMPLEMENT_DYNCREATE( DataWindow, FrameWindow )

BEGIN_MESSAGE_MAP( DataWindow, FrameWindow )
	//{{AFX_MSG_MAP( DataWindow )
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SYSCOMMAND()
	ON_COMMAND(ID_BUTTONBAR, OnButtonBar)
	ON_UPDATE_COMMAND_UI(ID_BUTTONBAR, OnUpdateButtonBar)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

DataWindow::DataWindow()
{}

DataWindow::~DataWindow()
{
}

void DataWindow::OnDestroy() 
{
	AfxGetApp()->PostThreadMessage(ILW_REMOVEDATAWINDOW, (WPARAM)m_hWnd, 0);
	FrameWindow::OnDestroy();
}


void DataWindow::OnSysCommand(UINT nID, LPARAM lParam)
{
  if (nID == ID_TOROOT)
    SwitchToRoot();    
  else
    CFrameWnd::OnSysCommand(nID, lParam);
}

void DataWindow::SwitchToRoot()
{
  CWnd* wnd = CWnd::FindWindow(ILWIS_VERSION_NAME, 0);
  if (0 == wnd)
    return;
  wnd->BringWindowToTop();
  wnd->ShowWindow(SW_SHOWNORMAL);
  
}

int DataWindow::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (FrameWindow::OnCreate(lpCreateStruct) == -1)
		return -1;

  CMenu* menSys = GetSystemMenu(FALSE);
  menSys->AppendMenu(MF_STRING, ID_TOROOT, "&ILWIS");	
	
	AfxGetApp()->PostThreadMessage(ILW_ADDDATAWINDOW, (WPARAM)m_hWnd, 0);
	
	return 0;
}

void DataWindow::OnButtonBar()
{
	if (bbDataWindow.IsWindowVisible())
		ShowControlBar(&bbDataWindow,FALSE,FALSE);
	else
		ShowControlBar(&bbDataWindow,TRUE,FALSE);
}

void DataWindow::OnUpdateButtonBar(CCmdUI* pCmdUI)
{
	bool fCheck = bbDataWindow.IsWindowVisible() != 0;
	pCmdUI->SetCheck(fCheck);
}

