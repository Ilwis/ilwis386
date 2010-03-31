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
// FieldGraph.cpp: implementation of the FieldGraph class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\FieldGraph.h"
#include "Client\ilwis.h" // for IlwisWinApp

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(SimpleGraphWindowWrapper, SimpleGraphWindow)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

SimpleGraphWindowWrapper::SimpleGraphWindowWrapper(FormEntry *f)
: SimpleGraphWindow()
, BaseZapp(f)
{
	CFont * fnt = IlwWinApp()->GetFont(IlwisWinApp::sfGRAPH);
	SetAxisFont(fnt);
}

void SimpleGraphWindowWrapper::OnMouseMove(UINT nFlags, CPoint point) 
{
	bool fNotify = m_fDragging && m_pFunc; // get this info before OnMouseMove where it may change (unlikely here though)
	SimpleGraphWindow::OnMouseMove(nFlags, point);
	if (fNotify)
		fProcess(NotificationEvent(GetDlgCtrlID(), WM_LBUTTONUP, (LPARAM)m_hWnd));
}

void SimpleGraphWindowWrapper::OnLButtonUp(UINT nFlags, CPoint point)
{
	bool fNotify = m_fDragging && m_pFunc; // get this info before OnLButtonUp where it may change
	SimpleGraphWindow::OnLButtonUp(nFlags, point);
	if (fNotify)
		fProcess(NotificationEvent(GetDlgCtrlID(), WM_LBUTTONUP, (LPARAM)m_hWnd));
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

FieldGraph::FieldGraph(FormEntry* p)
: FormEntry(p, 0, true)
, sgw(0)
{

}

FieldGraph::~FieldGraph()
{
	if (sgw)
		delete sgw;
}

void FieldGraph::create()
{
  zPoint pntFld = zPoint(psn->iPosX, psn->iPosY);
  zDimension dimFld = zDimension(psn->iMinWidth, psn->iMinHeight);

	sgw = new SimpleGraphWindowWrapper(this);
	sgw->Create(NULL,	"Graph", WS_CHILD | WS_VISIBLE, CRect(pntFld, dimFld), _frm->wnd(), Id());

  CreateChildren();
}

void FieldGraph::show(int sw)
{
	if (sgw)
		sgw->ShowWindow(sw);
}

void FieldGraph::StoreData()
{
  if (fShow())
	{
    if (sgw)
		{
			//_iVal = sgw->GetPos();
			//*_piVal = _iVal;
		}
  }
  FormEntry::StoreData();
}

void FieldGraph::Enable()
{
	if (sgw) sgw->EnableWindow(TRUE);
}

void FieldGraph::Disable()
{
	if (sgw) sgw->EnableWindow(FALSE);
}

void FieldGraph::SetFunction(SimpleFunction * funPtr)
{
	if (sgw) sgw->SetFunction(funPtr);
}

void FieldGraph::Replot()
{
	if (sgw) sgw->Replot();
}

void FieldGraph::SetBorderThickness(int l, int t, int r, int b)
{
	if (sgw) sgw->SetBorderThickness(l, t, r, b);
}
