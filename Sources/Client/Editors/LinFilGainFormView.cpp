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
// LinFilGainFormView.cpp: implementation of the LinFilGainFormView class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Client\ilwis.h"
#include "Client\Base\datawind.h"
#include "Headers\constant.h"
#include "Client\Base\IlwisDocument.h"
#include "Engine\Function\FILTER.H"
#include "Engine\Function\FLTLIN.H"
#include "Client\Editors\FilterDoc.h"
#include "Client\Editors\LinearFilterDoc.h"
#include "Client\Forms\generalformview.h"
#include "Client\Editors\LinFilGainFormView.h"
#include "Headers\Hs\FILTER.hs"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(LinFilGainFormView, GeneralFormView)

BEGIN_MESSAGE_MAP(LinFilGainFormView, GeneralFormView)
	//{{AFX_MSG_MAP(LinFilGainFormView)
	ON_WM_DESTROY()
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


LinFilGainFormView::LinFilGainFormView()
{
	fbs |= fbsNOOKBUTTON | fbsNOCANCELBUTTON;
}

LinFilGainFormView::~LinFilGainFormView()
{
}

LinearFilterDoc* LinFilGainFormView::GetDocument()
{
	return dynamic_cast<LinearFilterDoc*>(CView::GetDocument());
}

const LinearFilterDoc* LinFilGainFormView::GetDocument() const
{
	return dynamic_cast<const LinearFilterDoc*>(CView::GetDocument());
}

void LinFilGainFormView::CreateForm()
{
	GeneralFormView::CreateForm();
	const LinearFilterDoc* fd = GetDocument();
	if (0 == fd || !fd->flt().fValid())
		return;
	rGain = fd->fltlin()->rGetGain();
	frGain = new FieldReal(root, TR("&Gain"), &rGain);
	frGain->SetCallBack((NotifyProc)&LinFilGainFormView::CallBackGainChange);
}

int LinFilGainFormView::CallBackGainChange(Event*)
{
	frGain->StoreData();
	LinearFilterDoc* fd = GetDocument();
	if (fd->fltlin()->rGetGain() != rGain) 
		fd->fltlin()->SetGain(rGain);
	return 0;
}


void LinFilGainFormView::OnDestroy() 
{
	exec();
	GeneralFormView::OnDestroy();
}
