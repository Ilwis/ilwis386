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
// RecordBar.cpp: implementation of the RecordBar class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Client\ilwis.h"
#include "Client\TableWindow\BaseTablePaneView.h"
#include "Client\FormElements\syscolor.h"
#include "Headers\constant.h"
#include "Headers\Hs\Table.hs"
#include "Client\TableWindow\BaseTblField.h"
#include "Engine\Table\Rec.h"
#include "Client\TableWindow\RecordView.h"
#include "Client\Editors\Utils\sizecbar.h"
#include "Client\TableWindow\RecordBar.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP( RecordBar, CSizingControlBar )
	//{{AFX_MSG_MAP( RecordBar )
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


RecordBar::RecordBar()
: view(0)
{
}

RecordBar::~RecordBar()
{
}

BOOL RecordBar::Create(CWnd* pParent, int iID)
{
	if (!CSizingControlBar::Create("", pParent, CSize(300,150), TRUE, iID))
		return FALSE;
  m_dwSCBStyle |= SCBS_SHOWEDGES;

	SetBarStyle(GetBarStyle() | CBRS_SIZE_DYNAMIC);
	EnableDocking(CBRS_ALIGN_ANY);

	view = new RecordView;
	view->Create(this);
	view->OnInitialUpdate();
	return TRUE;
}

void RecordBar::OnSize(UINT nType, int cx, int cy)
{
	if (0 == view)
		return;
	view->MoveWindow(0,0,cx,cy);
}


