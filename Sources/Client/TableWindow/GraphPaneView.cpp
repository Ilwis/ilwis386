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
// GraphPaneView.cpp: implementation of the GraphPaneView class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Client\TableWindow\GraphPaneView.h"
#include "Client\TableWindow\GraphDoc.h"
#include "Client\GraphWindow\GraphDrawer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(GraphPaneView, GraphView)

BEGIN_MESSAGE_MAP(GraphPaneView, GraphView)
  ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

GraphPaneView::GraphPaneView()
{
}

GraphPaneView::~GraphPaneView()
{
}

GraphDoc* GraphPaneView::GetDocument()
{
  return (GraphDoc*)m_pDocument;
}

const GraphDoc* GraphPaneView::GetDocument() const
{
  return (const GraphDoc*)m_pDocument;
}

void GraphPaneView::OnInitialUpdate()
{
  GraphDoc* gd = GetDocument();
  grdrw = gd->grdrGet();
  grdrw->SetView(this);
  GraphView::OnInitialUpdate();
}

void GraphPaneView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
  GraphDoc* gd = GetDocument();
  if (0 == gd)
    return;
  grdrw = gd->grdrGet();
  if (0 == grdrw)
    return;
  grdrw->SetView(this);
  OnGraphFitInWindow();
  GraphView::OnUpdate(pSender, lHint, pHint);
}

void GraphPaneView::OnContextMenu(CWnd* pWnd, CPoint point)
{
  // do nothing
}
