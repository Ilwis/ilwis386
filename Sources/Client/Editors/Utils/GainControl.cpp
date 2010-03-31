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
#pragma warning( disable : 4786 )
#include "Client\Editors\Utils\GainControl.h"
#include "Headers\constant.h"
#include "Client\ilwis.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\Editors\LinearFilterDoc.h"
#include "Client\Base\Framewin.h"
#include "Headers\Hs\FILTER.hs"

//--[ GainEdit ]------------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(GainEdit, CEditView)
	ON_CONTROL_REFLECT(EN_CHANGE, OnEditChange)
  ON_WM_KILLFOCUS()
  ON_WM_SETFOCUS()
END_MESSAGE_MAP()


GainEdit::GainEdit()
{
}

GainEdit::~GainEdit()
{
}

BOOL GainEdit::Create(CWnd *parent, CDocument* doc)
{
	CRect rct;
	parent->GetClientRect(&rct);

	LinearFilterDoc* lfdoc = dynamic_cast<LinearFilterDoc*> (doc);
	ASSERT(lfdoc != 0);
	double rGain = lfdoc->fltlin()->rGetGain();
	String s("%f", rGain);

	BOOL iRet = CWnd::Create(0, s.c_str(), WS_CHILD | WS_VISIBLE | WS_BORDER | 
		                        ES_AUTOHSCROLL,
		                        rct, parent, 100);
	doc->AddView(this);
	CEdit& ed = GetEditCtrl();
	ed.SetWindowText(s.scVal());
	ed.SetReadOnly(lfdoc->fReadOnly());

	return iRet;
}

void GainEdit::OnEditChange()
{
	LinearFilterDoc* lfdoc = dynamic_cast<LinearFilterDoc*>(GetDocument());
	if (0 == lfdoc)
		return;

	CString str;
	CEdit& ed = GetEditCtrl();
	ed.GetWindowText(str);
	String sGain(str);
	double rGain = sGain.rVal();
	if (rGain <= 0) 
		return;

	double rGainOld = lfdoc->fltlin()->rGetGain();
	if (abs(rGain - rGainOld) < 1e-20)
		return;
	lfdoc->fltlin()->SetGain(rGain);
}

void GainEdit::OnKillFocus(CWnd* pNewWnd)
{
	CEditView::OnKillFocus(pNewWnd);
	FrameWindow* frm = dynamic_cast<FrameWindow*>(GetParentOwner());
	if (frm )
		frm->HandleAccelerators(true);
 }

void GainEdit::OnSetFocus(CWnd* pNewWnd)
{
	CEditView::OnSetFocus(pNewWnd);
	FrameWindow* frm = dynamic_cast<FrameWindow*>(GetParentOwner());
	if (frm)
		frm->HandleAccelerators(false);
}

//--[ GainControlBar ]------------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(GainControlBar, BaseBar)
	//{{AFX_MSG_MAP(GainControlBar)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


GainControlBar::GainControlBar() 
	: edGain(0)
{
}

GainControlBar::~GainControlBar()
{
}

BOOL GainControlBar::Create(CFrameWnd* fw, CDocument* doc)
{
	if (!BaseBar::Create(fw, ID_GAINCONTROLBAR))
		return FALSE;

	CFont* fnt = const_cast<CFont*>(IlwWinApp()->GetFont(IlwisWinApp::sfWindowMedium));
	edGain = new GainEdit;
	edGain->Create(this, doc);
	edGain->SetFont(fnt);

	String sLabel(SFLRemGain);
	stLabel.Create(sLabel.c_str(), WS_VISIBLE | WS_CHILD, CRect(2, 2, 2, 2), this);
	stLabel.SetFont(fnt);

	// Calculate the REAL size of the string length
  CDC pdc;
  pdc.CreateCompatibleDC(0);
	CFont* fntOld = pdc.SelectObject(fnt);
	szLabel = pdc.GetTextExtent(sLabel.c_str());
	pdc.SelectObject(fntOld);

	stLabel.MoveWindow(CRect(2,3, szLabel.cx + 2, szLabel.cy + 3));
	iHeight = szLabel.cy + 10;
	iLastWidth = szLabel.cx + 80;

	// caption during floating
	SetWindowText(SFLTitleGain.c_str());

	return TRUE;
}

void GainControlBar::OnSize(UINT nType, int cx, int cy)
{
	if (0 == edGain)
		return;
	int iLeft = szLabel.cx + 10;
	edGain->MoveWindow(CRect(iLeft, 1, cx, szLabel.cy + 9));
}

CSize GainControlBar::CalcDynamicLayout(int nLength, DWORD dwMode)
{
	CSize sizeResult;
	sizeResult.cy = iHeight;
	if (nLength > 0 && dwMode & LM_COMMIT) 
		iLastWidth = nLength;
	sizeResult.cx = iLastWidth;
	return sizeResult;
}

