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
// HistogramGraphView.cpp: implementation of the HistogramGraphView class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Client\ilwis.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\TableWindow\HistogramDoc.h"
#include "Client\TableWindow\HistogramGraphView.h"
#include "Client\GraphWindow\GraphDrawer.h"
#include "Client\GraphWindow\GraphLayer.h"
#include "Client\GraphWindow\GraphAxis.h"
#include "Engine\Table\tblview.h"
#include "Headers\messages.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(HistogramGraphView, GraphPaneView)

BEGIN_MESSAGE_MAP(HistogramGraphView, GraphPaneView)
  ON_MESSAGE(ILWM_VIEWSETTINGS, OnViewSettings)
END_MESSAGE_MAP()

HistogramGraphView::HistogramGraphView()
{
}

HistogramGraphView::~HistogramGraphView()
{
}

void HistogramGraphView::OnInitialUpdate()
{
  HistogramDoc* hd = dynamic_cast<HistogramDoc*>(CView::GetDocument());
  if (hd) {
    hd->RemoveView(this);
    hd->hgd.AddView(this);
  }
	GraphPaneView::OnInitialUpdate();
}

LRESULT HistogramGraphView::OnViewSettings(WPARAM wP, LPARAM lP)
{
	if (wP == SAVESETTINGS) 
  {
    GraphDoc* doc = GetDocument();
    doc->RemoveView(this);
	}
	return TRUE;
}

