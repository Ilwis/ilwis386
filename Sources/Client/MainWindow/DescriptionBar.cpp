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

#include "Headers\constant.h"
#include "Client\ilwis.h"
#include "Engine\Base\AssertD.h"
//#include "UserInterfaceObjects\UiTextInfo.h"
#include "Client\Base\IlwisDocument.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\Base\BaseView.h"
#include "Headers\messages.h"
#include "Client\Base\Framewin.h"
#include "Headers\Hs\Userint.hs"

static const int DESCRIPTION_ED_ID = 200;
static const int STRING_NOT_FOUND = -1;

//--[ DescEdit ]------------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(DescEdit, CEditView)
	ON_CONTROL_REFLECT(EN_CHANGE, OnEditChange)
  ON_WM_KILLFOCUS()
  ON_WM_SETFOCUS()
END_MESSAGE_MAP()


DescEdit::DescEdit()
{
	fActive = false;
}

DescEdit::~DescEdit()
{
}

BOOL DescEdit::Create(CWnd *parent, CDocument* doc)
{
	CRect rct;
	parent->GetClientRect(&rct);

	IlwisDocument* ilwdoc = dynamic_cast<IlwisDocument*> (doc);
	ASSERT(ilwdoc != 0);
	String s = ilwdoc->sObjectDescription();

	BOOL iRet = CWnd::Create(0, s.c_str(), WS_CHILD | WS_VISIBLE | WS_BORDER | 
		                        ES_AUTOHSCROLL,
		                        rct, parent, DESCRIPTION_ED_ID);
	doc->AddView(this);
	CEdit& ed = GetEditCtrl();
	ed.SetWindowText(s.c_str());
	ed.SetReadOnly(ilwdoc->fReadOnly());

	return iRet;
}

void DescEdit::OnEditChange()
{
	IlwisDocument* doc = dynamic_cast<IlwisDocument*>(GetDocument());
	if (0 == doc)
		return;

	CString str;
	CEdit& ed = GetEditCtrl();
	ed.GetWindowText(str);
	String sDes(str);

	String s = doc->sObjectDescription();
	if (!fCIStrEqual(s, sDes))
		if (doc->fDelaySaveDoc())	// TextDocument uses this 
			doc->SetModifiedFlag();
		else
			doc->SetObjectDescription(sDes);
}

void DescEdit::OnUpdateDescriptionEdit()
{
	if (fActive)
		return;

	IlwisDocument* doc = dynamic_cast<IlwisDocument*>(GetDocument());
	ISTRUE(fINotEqual, doc, (IlwisDocument *)NULL);
	
	String s = doc->sObjectDescription();
	CEdit& ed = GetEditCtrl();
	CString str;
	ed.GetWindowText(str);

	if (String(s) != str)
		if (!doc->fDelaySaveDoc() && !doc->IsModified())
			SetWindowText(s.c_str());
	ed.SetReadOnly(doc->fReadOnly());
}

void DescEdit::OnKillFocus(CWnd* pNewWnd)
{
	fActive = false;
	CEditView::OnKillFocus(pNewWnd);
	FrameWindow* frm = dynamic_cast<FrameWindow*>(GetParentOwner());
	if (frm )
		frm->HandleAccelerators(true);
 }

void DescEdit::OnSetFocus(CWnd* pNewWnd)
{
	fActive = true;
	CEditView::OnSetFocus(pNewWnd);
	FrameWindow* frm = dynamic_cast<FrameWindow*>(GetParentOwner());
	if (frm)
		frm->HandleAccelerators(false);
}

//--[ DescriptionBar ]------------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(DescriptionBar, BaseBar)
	//{{AFX_MSG_MAP(DescriptionBar)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


DescriptionBar::DescriptionBar() 
	: descEdit(0)
{
}

DescriptionBar::~DescriptionBar()
{
}

BOOL DescriptionBar::Create(CFrameWnd* fw, CDocument* doc)
{
	if (!BaseBar::Create(fw, ID_DESCRIPTIONBAR))
		return FALSE;

	CFont* fnt = const_cast<CFont*>(IlwWinApp()->GetFont(IlwisWinApp::sfWindowMedium));
	descEdit = new DescEdit;
	descEdit->Create(this, doc);
	descEdit->SetFont(fnt);

	stText.Create(TR("Description").c_str(), WS_VISIBLE | WS_CHILD, CRect(2, 2, 2, 2), this);
	stText.SetFont(fnt);

	// Calculate the REAL size of the string length
  CDC pdc;
  pdc.CreateCompatibleDC(0);
	CFont* fntOld = pdc.SelectObject(fnt);
	CSize sz = pdc.GetTextExtent(TR("Description").c_str());
	pdc.SelectObject(fntOld);
	m_csLabelSize = sz;

	stText.MoveWindow(CRect(2,3, sz.cx + 2, sz.cy + 3));
	iHeight = m_csLabelSize.cy + 10;

	// caption during floating
	SetWindowText(TR("Description").c_str());

	return TRUE;
}

void DescriptionBar::SetText(String sText)
{
	descEdit->SetWindowText(sText.c_str());
}

String DescriptionBar::sGetText()
{
	CString sStr;
	descEdit->GetEditCtrl().GetWindowText(sStr);
	return String(sStr);
}

void DescriptionBar::SetReadOnly(bool fRO)
{
	descEdit->GetEditCtrl().SetReadOnly((BOOL)fRO);
}

void DescriptionBar::OnUpdateCmdUI(CFrameWnd* pParent, BOOL)
{
	descEdit->OnUpdateDescriptionEdit();
}

void DescriptionBar::OnSize(UINT nType, int cx, int cy)
{
	if (0 == descEdit)
		return;
	int iLeft = m_csLabelSize.cx + 10;
	descEdit->MoveWindow(CRect(iLeft, 1, cx, m_csLabelSize.cy + 9));
}
