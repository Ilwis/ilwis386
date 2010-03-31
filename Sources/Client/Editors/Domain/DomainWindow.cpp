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
// DomainWindow.cpp: implementation of the DomainWindow class.
//
//////////////////////////////////////////////////////////////////////

#define DOMAINWIND_C

#include "Client\Headers\formelementspch.h"
#include "Headers\Htp\Ilwismen.htp"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\ilwis.h"
#include "Client\Base\datawind.h"
#include "Headers\stdafx.h"
#include "Client\Editors\Domain\DomainWindow.h"
#include "Headers\Htp\Ilwis.htp"
#include "Client\Editors\Domain\DomainView.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(DomainWindow, DataWindow)

BEGIN_MESSAGE_MAP(DomainWindow, DataWindow)
	//{{AFX_MSG_MAP(DomainWindow)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	ON_WM_GETMINMAXINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DomainWindow construction/destruction

DomainWindow::DomainWindow()
{
	// TODO: add member initialization code here
	htpTopic = htpDomainWindow;
	sHelpKeywords = "Domain Class/ID editor";
}

DomainWindow::~DomainWindow()
{
}

BOOL DomainWindow::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !DataWindow::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// DomainWindow diagnostics

#ifdef _DEBUG
void DomainWindow::AssertValid() const
{
	DataWindow::AssertValid();
}

void DomainWindow::Dump(CDumpContext& dc) const
{
	DataWindow::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// DomainWindow message handlers

void DomainWindow::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI)
{
	CRect rectWind, rectView;
	GetWindowRect(&rectWind);

	lpMMI->ptMinTrackSize.x = 260;
	lpMMI->ptMinTrackSize.y = 260;
	DomainSortView* view = dynamic_cast<DomainSortView*>(GetActiveView());
	if (view != 0)
	{
		view->GetWindowRect(&rectView);
		CSize size(0, 0);
		view->GetMinViewSize(size, CSize(rectWind.Width() - rectView.Width(), rectWind.Height() - rectView.Height()));
		lpMMI->ptMinTrackSize.x = size.cx;
		lpMMI->ptMinTrackSize.y = size.cy;
	}
}

