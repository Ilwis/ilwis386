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
// RepresentationView.cpp : implementation of the RepresentationView class
//
#include "Headers\toolspch.h"
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Headers\messages.h"
#include "Client\Base\ZappToMFC.h"
#include "Client\Base\Res.h"
#include "Engine\Base\AssertD.h"
#include "Client\Base\IlwisDocument.h"
#include "Engine\Domain\dm.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\Base\BaseView2.h"
#include "Engine\Representation\Rprgrad.h"
#include "Engine\Representation\Rprclass.h"
#include "Client\Editors\Representation\RepresentationDoc.h"
#include "Client\Editors\Representation\RepresentationView.h"
#include "Headers\constant.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// RepresentationView

IMPLEMENT_DYNCREATE(RepresentationView, CView)

BEGIN_MESSAGE_MAP(RepresentationView, CView)
	//{{AFX_MSG_MAP(RepresentationView)
	ON_WM_CREATE()
	ON_COMMAND(ID_DESCRIPTIONBAR, OnDescriptionBar)
	ON_UPDATE_COMMAND_UI(ID_DESCRIPTIONBAR, OnUpdateDescriptionBar)
	ON_MESSAGE(ILWM_DESCRIPTIONTEXT, OnDescriptionChanged)
	ON_MESSAGE(ILWM_VIEWSETTINGS,    OnViewSettings)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// RepresentationView construction/destruction

RepresentationView::RepresentationView() 
{
}

RepresentationView::~RepresentationView()
{
}

BOOL RepresentationView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CView::PreCreateWindow(cs);
}

int RepresentationView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	return CView::OnCreate(lpCreateStruct);
}

LRESULT RepresentationView::OnDescriptionChanged(WPARAM wP, LPARAM lP)
{
	return BaseView2::OnDescriptionChanged(wP, lP);
}

LRESULT RepresentationView::OnViewSettings(WPARAM wP, LPARAM lP)
{
	return BaseView2::OnViewSettings(wP, lP);
}

void RepresentationView::OnInitialUpdate() 
{
  BaseView2::OnInitialUpdate();
	CreateDescriptionBar();
  ColorBuf clrBuf;
  GetDocument()->rpr()->GetColorLut(clrBuf);
}

void RepresentationView::OnDraw(CDC* pDC)
{
	RepresentationDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

void	RepresentationView::OnDescriptionBar()
{
	if ( descBar.GetSafeHwnd())
	{
		if (descBar.IsWindowVisible())
		{
			SetMenuState(ID_DESCRIPTIONBAR, MF_UNCHECKED);
			GetParentFrame()->OnBarCheck(ID_DESCRIPTIONBAR);
		}
		else
		{
			SetMenuState(ID_DESCRIPTIONBAR, MF_CHECKED);
			GetParentFrame()->OnBarCheck(ID_DESCRIPTIONBAR);
		}
	}
}

void	RepresentationView::OnUpdateDescriptionBar(CCmdUI *cmd)
{
	if ( descBar.IsWindowVisible() == TRUE )
		cmd->SetCheck(1);
	else
		cmd->SetCheck(0);
}

#ifdef _DEBUG
void RepresentationView::AssertValid() const
{
	BaseView2::AssertValid();
}

void RepresentationView::Dump(CDumpContext& dc) const
{
	BaseView2::Dump(dc);
}
#endif //_DEBUG

RepresentationDoc* RepresentationView::GetDocument() 
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(RepresentationDoc)));
	return dynamic_cast<RepresentationDoc *>(m_pDocument);
}



