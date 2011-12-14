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
// SampleStatWindow.cpp: implementation of the SampleStatWindow class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Client\Editors\Utils\sizecbar.h"
#include "Headers\Hs\Sample.hs"
#include "Client\ilwis.h"
#include "Engine\SampleSet\SAMPLSET.H"
#include "SampleStatWindow.h"


BEGIN_MESSAGE_MAP( SampleStatWindow, CSizingControlBar )
	//{{AFX_MSG_MAP( SampleStatWindow )
	ON_CBN_SELCHANGE(100, SelChange)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/*
class ClassComboBox : public CComboBox
{
public:
ClassComboBox(SampleStatWindow* w) 
{ wStatWin = w; }
SampleStatWindow* wStatWin;
//{{AFX_MSG(SampleStatWindow)
afx_msg void OnSelChanged();
//}}AFX_MSG
DECLARE_MESSAGE_MAP()

};

BEGIN_MESSAGE_MAP( ClassComboBox, CComboBox)
//{{AFX_MSG_MAP( ClassComboBox )
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void ClassComboBox::OnSelChanged()
{
wStatWin->SelChanged();
}
*/
SampleStatWindow::SampleStatWindow(const SampleSet& ss)
: sms(ss)
{
}

SampleStatWindow::~SampleStatWindow()
{
}

BOOL SampleStatWindow::Create(CWnd* pParent)
{
	CFont* fnt;
	fnt = IlwWinApp()->GetFont(IlwisWinApp::sfTABLE);
	CDC cdcTmp;
	cdcTmp.CreateCompatibleDC(0);
	String s(' ', 38);
	CFont* fntOld = cdcTmp.SelectObject(fnt);
	CSize siz = cdcTmp.GetTextExtent(s.c_str());
	cdcTmp.SelectObject(fntOld);

	long ysize = min(550, (2*sms->iBands()+6)*siz.cy);
	if (!CSizingControlBar::Create(TR("    Sample Statistics").c_str(), pParent, 
		CSize(15+siz.cx, ysize), TRUE, 1125))
		return FALSE;
	m_dwSCBStyle |= SCBS_SHOWEDGES;
	SetBarStyle(GetBarStyle() | CBRS_SIZE_DYNAMIC);
	EnableDocking(CBRS_ALIGN_ANY);
	ccb.Create(CBS_DROPDOWNLIST|WS_VSCROLL, CRect(5,5, 150, 200) , this, 100);
	fnt = IlwWinApp()->GetFont(IlwisWinApp::sfWindowMedium);
	ccb.SetFont(fnt);
	DomainClass* dc = sms->dc();
	for (int i = 1; i <= dc->iNettoSize(); ++i)
		ccb.AddString(dc->sValueByRaw(dc->iKey(i),0).c_str());
	ccb.SetCurSel(0);
	//	st.Create("", WS_CHILD | WS_VISIBLE | SS_LEFTNOWORDWRAP ,CRect(5, 35, siz.cx, (2*sms->iBands()+6)*siz.cy), this);
	st.Create(ES_MULTILINE | WS_VSCROLL | ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_READONLY  ,CRect(5, 35, siz.cx, ysize), this, 101);
	fnt = IlwWinApp()->GetFont(IlwisWinApp::sfTABLE);
	st.SetFont(fnt);
	st.SetTabStops(1); // tab space is one dialog unit (so that tabs can be used just for copy to clipboard)
	ccb.ShowWindow(SW_SHOW);
	st.ShowWindow(SW_SHOW);
	return TRUE;
}

void SampleStatWindow::SetText()
{
	SelChange();
	return;
}    

void SampleStatWindow::AddUnique(const String& sVal)
{
	if (sVal == sUNDEF)
		return;
	if (CB_ERR == ccb.FindStringExact(0, sVal.c_str())) { // not found
		ccb.ResetContent();
		for (int i = 1; i <= sms->dc()->iNettoSize(); ++i)
			ccb.AddString(sms->dc()->sValueByRaw(sms->dc()->iKey(i),0).c_str());
	}  
	ccb.SelectString(0,sVal.c_str());
}


void SampleStatWindow::SelChange()
{
	long iSel = ccb.GetCurSel();
	String sText;
	if (iSel >= 0) {
		sText = TR("Band   \tMean  \tStDev   \tNr \tPred  \tTotal");
		CString sSel;
		ccb.GetLBText(iSel, sSel);
		long iRaw = sms->dc()->iRaw(sSel);
		iSel += 1;
		for (int band = 0; band < sms->iBands(); ++band) 
		{
			double rMean, rStd;
			long iPredNr, iTotNr;
			byte bPred;
			sms->StatInfo(iRaw, band, rMean, rStd, bPred, iPredNr, iTotNr);
			int iBand = band + 1;
			sText &= String("\r\n%3i:\t%7.1f\t%7.1f\t%5li\t%5i\t%7li", iBand, rMean, rStd, iPredNr, (int)bPred, iTotNr);
		}
		sText &= "\r\n\r\n";
		sText &= TR(" Current Selection:");
		for (int band = 0; band < sms->iBands(); ++band) 
		{
			double rMean, rStd;
			long iPredNr, iTotNr;
			byte bPred;
			sms->StatInfo(0, band, rMean, rStd, bPred, iPredNr, iTotNr);
			int iBand = band + 1;
			sText &= String("\r\n%3i:\t%7.1f\t%7.1f\t%5li\t%5i\t%7li",
				iBand, rMean, rStd, iPredNr, (int)bPred, iTotNr);
		}                   
	}
	st.SetWindowText(sText.c_str());
}

void SampleStatWindow::OnSize(UINT nType, int cx, int cy)
{
	if (!IsWindow(st.m_hWnd))
		return;
	CRect rct;
	GetClientRect(&rct);
	rct.left += 5;
	rct.top += 35;
	rct.right -= 5;
	rct.bottom -= 5;
	st.MoveWindow(&rct);	
}

