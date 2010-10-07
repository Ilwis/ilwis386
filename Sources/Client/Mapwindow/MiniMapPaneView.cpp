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
#include "Client\Headers\formelementspch.h"
#include "Engine\Map\Segment\Seg.h"
#include "engine\map\polygon\POL.H"
#include "Client\ilwis.h"
#include "Client\Base\datawind.h"
#include "Client\Mapwindow\MapWindow.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\MainWindow\Catalog\CatalogDocument.h"
#include <afxole.h>
#include "Engine\Map\Mapview.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Client\Mapwindow\Positioner.h"
#include "Headers\constant.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\Mapwindow\AreaSelector.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Client\Mapwindow\MiniMapPaneView.h"


BEGIN_MESSAGE_MAP(MiniMapPaneView, MapPaneView)
	//{{AFX_MSG_MAP(MapPaneView)
	ON_WM_SIZE()	
	ON_COMMAND(ID_ENTIREMAP, OnEntireMap)
END_MESSAGE_MAP()

MiniMapPaneView::MiniMapPaneView()
{
	ShowScrollBars(false);
}

MiniMapPaneView::~MiniMapPaneView()
{}

void MiniMapPaneView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, rctLocation.Width(), rctLocation.Height());
	dim = zDimension(rctLocation.Size());
	MoveWindow(&rctLocation);
}

void MiniMapPaneView::OnEntireMap()
{
  _rScale = 1;
  iXpos = iXmin;
  iYpos = iYmin;

	MinMax mm = mmBounds();
  int iWidth = scale(mm.width(), true);
  int iHeight = scale(mm.height(), true);
	double rScaleX = double(dim.width()) / iWidth;
	double rScaleY = double(dim.height()) / iHeight;
	// choose the smallest of the scales as the correct one
	double rScale = min(rScaleX, rScaleY);

	// change to 'Ilwis scale'
	_rScale = rScale >= 1.0 ? rScale : -1.0/rScale;

	if (fStarting) {
		// calc the preferred size
		iWidth = 20 + scale(mm.width(), true);
		iHeight = 20 + scale(mm.height(), true);	
		if (iWidth < 100)
			iWidth = 100;
		if (iHeight < 100)
			iHeight = 100;

		// calc the preferred size of the entire window
		CRect r;
		GetClientRect(&r);
	}
	// needed in case the size was already exactly fitting!
	CalcFalseOffsets();
	setScrollBars();
	fStarting = false;
	SetDirty();
}

void MiniMapPaneView::SetLocation(const CRect& rct)
{
	rctLocation = rct;
}
