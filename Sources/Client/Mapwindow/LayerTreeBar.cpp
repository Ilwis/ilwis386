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
// LayerTreeBar.cpp: implementation of the LayerTreeBar class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\ilwis.h"
#include "Client\Base\datawind.h"
#include "Client\Editors\Utils\BaseBar.h"
#include "Client\MainWindow\CommandCombo.h"
#include "Headers\constant.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\TableWindow\TableDoc.h"
#include "Client\TableWindow\BaseTablePaneView.h"
#include "Client\TableWindow\TablePaneView.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Engine\Map\Segment\Seg.h"
#include "Engine\Map\Polygon\POL.H"
#include "Client\Mapwindow\Positioner.h"
#include "Client\Mapwindow\Drawers\BaseDrawer.h"
#include "Client\Editors\Editor.h"
#include "Client\Editors\Utils\SYMBOL.H"
#include "Client\Editors\Utils\sizecbar.h"
#include "Client\Mapwindow\Drawers\ComplexDrawer.h"
#include "Client\Mapwindow\LayerTreeBar.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Headers\Hs\Mapwind.hs"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP( LayerTreeBar, CSizingControlBar )
	//{{AFX_MSG_MAP( LayerTreeBar )
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


LayerTreeBar::LayerTreeBar()
: ltv(0)
{
}

LayerTreeBar::~LayerTreeBar()
{
//	delete ltv;  -- gives crash ???
}

BOOL LayerTreeBar::Create(CWnd* pParent)
{
	if (!CSizingControlBar::Create(SMWTitleLayerManagement.scVal(), pParent, CSize(150,350), TRUE, 124))
		return FALSE;
  m_dwSCBStyle |= SCBS_SHOWEDGES;

	SetBarStyle(GetBarStyle() | CBRS_SIZE_DYNAMIC);
	EnableDocking(CBRS_ALIGN_LEFT|CBRS_ALIGN_RIGHT);

	ltv = new LayerTreeView;
	ltv->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW|TVS_HASLINES|TVS_LINESATROOT|TVS_HASBUTTONS,
			CRect(0,0,0,0), this, 100, 0);
	return TRUE;
}

void LayerTreeBar::OnSize(UINT nType, int cx, int cy)
{
	if (0 == ltv)
		return;
	ltv->MoveWindow(0,0,cx,cy);
}

