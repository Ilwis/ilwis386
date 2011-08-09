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
// LayoutListBar.cpp: implementation of the LayoutListBar class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Client\Editors\Layout\LayoutListBar.h"
#include "Client\Editors\Layout\LayoutListView.h"
#include "Headers\Hs\Layout.hs"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


BEGIN_MESSAGE_MAP( LayoutListBar, CSizingControlBar )
	//{{AFX_MSG_MAP( LayoutListBar )
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


LayoutListBar::LayoutListBar()
: llv(0)
{			 
}

LayoutListBar::~LayoutListBar()
{
}

BOOL LayoutListBar::Create(CWnd* pParent)
{
	if (!CSizingControlBar::Create("", pParent, CSize(200,120), TRUE, 124))
		return FALSE;
  m_dwSCBStyle |= SCBS_SHOWEDGES;
	SetWindowText(TR("Layout Management").c_str());

	SetBarStyle(GetBarStyle() | CBRS_SIZE_DYNAMIC);
	EnableDocking(CBRS_ALIGN_ANY);

	llv = new LayoutListView;
	llv->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
			CRect(0,0,0,0), this, 100, 0);
	return TRUE;
}

void LayoutListBar::OnSize(UINT nType, int cx, int cy)
{
	if (0 == llv)
		return;
	llv->MoveWindow(0,0,cx,cy);
}

