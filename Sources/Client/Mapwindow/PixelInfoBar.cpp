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
// PixelInfoBar.cpp: implementation of the PixelInfoBar class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\ilwis.h"
#include "Engine\Base\DataObjects\ObjectCollection.h"
#include "Client\Base\datawind.h"
#include "Client\Editors\Utils\BaseBar.h"
#include "Client\MainWindow\CommandCombo.h"
#include "Headers\constant.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\TableWindow\TableDoc.h"
#include "Client\TableWindow\BaseTablePaneView.h"
#include "Client\TableWindow\TablePaneView.h"
#include "Client\TableWindow\BaseTblField.h"
#include "Client\Mapwindow\PixelInfoDoc.h"
#include "Client\Mapwindow\PixelInfoView.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Engine\Map\Segment\Seg.h"
#include "Engine\Map\Polygon\POL.H"
#include "Client\Mapwindow\Positioner.h"
#include "Client\Editors\Editor.h"
#include "Client\Editors\Utils\SYMBOL.H"
#include "Client\Editors\Utils\sizecbar.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Client\Mapwindow\PixelInfoBar.h"
#include "Headers\Hs\Mapwind.hs"

BEGIN_MESSAGE_MAP( PixelInfoBar, CSizingControlBar )
	ON_WM_SIZE()
	ON_MESSAGE(ILW_UPDATE, OnUpdate)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


PixelInfoBar::PixelInfoBar()
: pixview(0)
{
	odt = new COleDropTarget;
	fDragging = false;
}

PixelInfoBar::~PixelInfoBar()
{


		//delete pixview;  -- gives crash ???
}

void PixelInfoBar::OnDestroy() {
		AfxGetApp()->PostThreadMessage(ILW_REMOVEDATAWINDOW, (WPARAM)this->m_hWnd, 0);
		CSizingControlBar::OnDestroy();
}

LRESULT PixelInfoBar::OnUpdate(WPARAM wParam, LPARAM lParam)
{
	CoordMessage cm = (CoordMessage) wParam;
	if ( cm & cmZOOMIN)
		return 0;

	PixelInfoDoc* pid = (PixelInfoDoc*)pixview->GetDocument();
	return pid->OnUpdate(wParam, lParam);
}

BOOL PixelInfoBar::Create(CWnd* pParent)
{
	if (!CSizingControlBar::Create(SMWTitleLayerManagement.scVal(), pParent, CSize(150,350), TRUE, 1267))
		return FALSE;
    m_dwSCBStyle |= SCBS_SHOWEDGES;

	SetBarStyle(GetBarStyle() | CBRS_SIZE_DYNAMIC);
	EnableDocking(CBRS_ALIGN_LEFT|CBRS_ALIGN_RIGHT);

	pixview = new PixelInfoView;
	pixview->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
			CRect(0,0,0,0), this, 100, 0);
	AfxGetApp()->PostThreadMessage(ILW_ADDDATAWINDOW, (WPARAM)this->m_hWnd, 0);

	return TRUE;
}

void PixelInfoBar::OnSize(UINT nType, int cx, int cy)
{
	if (0 == pixview)
		return;
	pixview->MoveWindow(0,0,cx,cy);
}

//--------------------------------

