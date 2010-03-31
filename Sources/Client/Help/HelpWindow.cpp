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
// HelpWindow.cpp : implementation file
//

#include "Headers\stdafx.h"
#include "Client\Help\HHCTRL.H"
#include "Client\Help\helpview.h"
#include "Client\Help\helpwindow.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(HelpWindow, CFrameWnd)
	//{{AFX_MSG_MAP(HelpWindow)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


HelpWindow::HelpWindow(CWnd* wndParent, const char* sTitle, const char* sHtml)           
{
	Create("IlwisView", sTitle, WS_OVERLAPPEDWINDOW, CRect(100,100,600,600), wndParent);
	hhc.Create(this, rectDefault, 0, "main");
	hhc.SetChmFile("ilwis.chm");
	hhc.Navigate(sHtml);
	ShowWindow(SW_SHOW);
}

HelpWindow::~HelpWindow()
{
}



/////////////////////////////////////////////////////////////////////////////
// HelpWindow message handlers

void HelpWindow::OnSize(UINT nType, int cx, int cy) 
{
	CFrameWnd::OnSize(nType, cx, cy);
	CRect rct;
	GetClientRect(&rct);
	hhc.MoveWindow(&rct, TRUE);
}

BOOL HelpWindow::PreCreateWindow(CREATESTRUCT& cs) 
{
 	cs.dwExStyle |= WS_EX_WINDOWEDGE;
	return TRUE;
}
