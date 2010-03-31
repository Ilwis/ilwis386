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
/* DataFormWindow
// by Wim Koolhoven
// (c) Ilwis System Development ITC
	Last change:  WK   23 Dec 98   10:40 am
*/
#define DATAFORM_C
#include "Client\Headers\formelementspch.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\Base\datawind.h"
#include "Client\Base\DataFormWindow.h"
#include "Headers\constant.h"
#include "Headers\Hs\Mainwind.hs"

IMPLEMENT_DYNCREATE( DataFormWindow, DataWindow)

BEGIN_MESSAGE_MAP( DataFormWindow, DataWindow)
	//{{AFX_MSG_MAP( DataFormWindow )
	ON_WM_NCPAINT()
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_SYSCOMMAND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

DataFormWindow::DataFormWindow()
{}

DataFormWindow::~DataFormWindow()
{}

BOOL DataFormWindow::PreCreateWindow(CREATESTRUCT& cs)
{
  if (!DataWindow::PreCreateWindow(cs))
    return FALSE;
  cs.style &= ~WS_MAXIMIZEBOX; // form cannot be maximized
  cs.style &= ~WS_MINIMIZEBOX; // form cannot be maximized
	cs.style &= ~WS_THICKFRAME;  // form cannot be sized
  cs.dwExStyle &= ~WS_EX_OVERLAPPEDWINDOW; // no edge!
  return TRUE;
}

int DataFormWindow::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (DataWindow::OnCreate(lpCreateStruct) == -1)
		return -1;
	status->ShowWindow(SW_HIDE);	

  CMenu* menSys = GetSystemMenu(FALSE);
	menSys->DeleteMenu(SC_SIZE,MF_BYCOMMAND);
	menSys->DeleteMenu(SC_MAXIMIZE,MF_BYCOMMAND);
  menSys->AppendMenu(MF_SEPARATOR, -1);
	menSys->AppendMenu( MF_STRING, ID_COPY, SUICopy.sVal());
  menSys->AppendMenu( MF_STRING, ID_PRINT, SUIPrint.sVal());
	return 0;
}

afx_msg void DataFormWindow::OnSysCommand( UINT nID, LPARAM lParam )
{
    if (nID == ID_COPY)
        Copy();
    else if (nID == ID_PRINT)
      Print();

    DataWindow::OnSysCommand(nID, lParam);
}

void DataFormWindow::Print()
{
	FormBaseView *view = (FormBaseView *)GetActiveView();
	view->Print();
}

void DataFormWindow::Copy()
{
	FormBaseView *view = (FormBaseView *)GetActiveView();
	view->Copy();
}
