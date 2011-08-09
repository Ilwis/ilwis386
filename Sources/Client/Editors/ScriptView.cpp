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
// ScriptView.cpp : implementation of the ScriptView class
//
#include "Headers\toolspch.h"
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Headers\constant.h"
#include "Client\Base\Res.h"
#include "Engine\Base\AssertD.h"
#include "Engine\Scripting\Script.h"
#include "Client\Base\IlwisDocument.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\Base\Framewin.h"
#include "Client\Base\BaseView2.h"
#include "Client\Base\ButtonBar.h"
#include "Client\Editors\TextDoc.h"
#include "Client\Editors\ScriptDoc.h"
#include "Client\Editors\TextView.h"
#include "Client\Editors\ScriptView.h"
#include "Headers\Hs\Script.hs"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ScriptView

IMPLEMENT_DYNCREATE(ScriptView, TextView)

BEGIN_MESSAGE_MAP(ScriptView, TextView)
	//{{AFX_MSG_MAP(ScriptView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ScriptView construction/destruction

ScriptView::ScriptView()
{
	// TODO: add construction code here

}

ScriptView::~ScriptView()
{
}

BOOL ScriptView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	BOOL bPreCreated = TextView::PreCreateWindow(cs);
	cs.style &= ~(ES_AUTOHSCROLL|WS_HSCROLL);	// Enable word-wrapping

	return bPreCreated;
}

void ScriptView::OnInitialUpdate() 
{
	TextView::OnInitialUpdate();

	FrameWindow* pfrm = dynamic_cast<FrameWindow*>(GetParentFrame());
	if (pfrm)
		pfrm->SetWindowName("ScriptEditor");

	ScriptDoc *scrdoc = GetDocument();
	ISTRUE(fINotEqual, scrdoc, (ScriptDoc *)NULL);
	string sTxt = scrdoc->sGetText();
	CEdit& ed = edv->GetEditCtrl();
	ed.SetWindowText(sTxt.c_str());

	m_bbTools.Create(pfrm, "script.but", TR("Script editor").c_str(), ID_SCRIPTBUTTONBAR);

	pfrm->DockControlBar(&m_bbTools, AFX_IDW_DOCKBAR_TOP);
}

/////////////////////////////////////////////////////////////////////////////
// ScriptView drawing

void ScriptView::OnDraw(CDC* pDC)
{
	ScriptDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// ScriptView diagnostics

#ifdef _DEBUG
void ScriptView::AssertValid() const
{
	TextView::AssertValid();
}

void ScriptView::Dump(CDumpContext& dc) const
{
	TextView::Dump(dc);
}
#endif //_DEBUG

ScriptDoc* ScriptView::GetDocument() 
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(ScriptDoc)));
	return dynamic_cast<ScriptDoc *>(m_pDocument);
}


/////////////////////////////////////////////////////////////////////////////
// ScriptView message handlers


/////////////////////////////////////////////////////////////////////////////
/// Commands

const string ScriptView::sText() const
{
	CString sTxt;
	CEdit& ed = edv->GetEditCtrl();
	ed.GetWindowText(sTxt);
	return string(sTxt);
}

void ScriptView::LoadState(IlwisSettings& settings)
{
	TextView::LoadState(settings);
}

void ScriptView::SaveState(IlwisSettings& settings)
{
	TextView::SaveState(settings);
}


