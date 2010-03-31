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
// GeneralFormView.cpp: implementation of the GeneralFormView class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Client\ilwis.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\Forms\generalformview.h"
#include "Headers\Hs\Mainwind.hs"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(GeneralFormView, FormBaseView)

BEGIN_MESSAGE_MAP(GeneralFormView, FormBaseView)
	//{{AFX_MSG_MAP(GeneralFormView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

GeneralFormView::GeneralFormView()
{
}

GeneralFormView::~GeneralFormView() 
{
}


void GeneralFormView::CreateForm()
{
	GeneralFormView::create();
}

void GeneralFormView::create()
{
	IlwisObject obj = GetDocument()->obj();
	m_sDesc = obj->sDescription;

	StaticText* st = new StaticText(root, obj->sTypeName());
	st->SetIndependentPos();

	CFont *fntSys = const_cast<CFont *> ( IlwWinApp()->GetFont(IlwisWinApp::sfFORM));
	LOGFONT lf;
	fntSys->GetLogFont(&lf);
	lf.lfHeight *= 1.25;
	lf.lfWidth = 0;
	CFont* fnt = new CFont;
	fnt->CreateFontIndirect(&lf);

	st->Font(fnt);

	st = new StaticText(root, SMSUiDescription);
	st->psn->SetBound(0,0,0,0);
	st->SetIndependentPos();
	if (obj->fReadOnly()) {
		st = new StaticText(root, obj->sDescription);
		st->SetIndependentPos();
		if (obj->fSystemObject()) {
			st->psn->SetBound(0,0,0,0);
			st = new StaticText(root, SMSRemSystemObject);
			st->SetIndependentPos();
		}
	 }
	else 
	{
		m_fsDesc = new FieldString(root, "", &m_sDesc);
		m_fsDesc->SetCallBack((NotifyProc)&GeneralFormView::DescriptionChanged);
		m_fsDesc->SetWidth(150);
		m_fsDesc->SetIndependentPos();
	}
}

int GeneralFormView::DescriptionChanged(Event *)
{
	m_fsDesc->StoreData();
	DataHasChanged(true);
	return 0;
}

String GeneralFormView::sDesc()
{
	return m_sDesc;
}

int GeneralFormView::exec()
{
	FormBaseView::exec();
	IlwisObject obj = GetDocument()->obj();
	if ( obj->fReadOnly() )
		return 0;
	else
	{
		if ( m_sDesc != obj->sDescr() )
			obj->SetDescription(m_sDesc);
//		obj->Updated();
	}

	return 0;
}

BOOL GeneralFormView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!FormBaseView::PreCreateWindow(cs))
    return FALSE;
  cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
  return TRUE;
}
