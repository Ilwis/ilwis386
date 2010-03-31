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
// GeneralBar.cpp: implementation of the GeneralBar class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Client\ilwis.h"
#include "Client\Editors\Utils\sizecbar.h"
#include "Client\Editors\Utils\GeneralBar.h"
#include "Headers\constant.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


BEGIN_MESSAGE_MAP( GeneralBar, CSizingControlBar )
	//{{AFX_MSG_MAP( GeneralBar )
	ON_WM_SIZE()
	ON_COMMAND(ID_ALLOWDOCKING, OnAllowDocking)
	ON_UPDATE_COMMAND_UI(ID_ALLOWDOCKING, OnUpdateAllowDocking)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



GeneralBar::GeneralBar()
: view(0)
{
}

GeneralBar::~GeneralBar()
{
}

BOOL GeneralBar::Create(CWnd* pParent, int iID, CSize siz)
{
	if (!CSizingControlBar::Create("", pParent, siz, TRUE, iID))
		return FALSE;
  m_dwSCBStyle |= SCBS_SHOWEDGES;

	SetBarStyle(GetBarStyle() | CBRS_SIZE_DYNAMIC);
	EnableDocking(CBRS_ALIGN_ANY);

	return TRUE;
}

void GeneralBar::OnSize(UINT nType, int cx, int cy)
{
	if (0 == view || 0 == view->m_hWnd)
		return;
	view->MoveWindow(0,0,cx,cy);
}
							
bool GeneralBar::fAllowDocking() const
{
	return m_dwDockStyle & CBRS_ALIGN_ANY ? true : false;
}

void GeneralBar::OnAllowDocking()
{
	if (fAllowDocking()) {
		EnableDocking(0);
		if (!IsFloating())
			m_pDockContext->ToggleDocking();
	}
	else
		EnableDocking(CBRS_ALIGN_ANY);
}

void GeneralBar::OnUpdateAllowDocking(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(fAllowDocking());
}

