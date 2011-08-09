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
// DomainBoolView.cpp : implementation file
//
#include "Client\Headers\formelementspch.h"
#include "Headers\Htp\Ilwismen.htp"
#include "Client\ilwis.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\Base\datawind.h"
#include "Engine\Domain\Dmvalue.h"

#include "Client\Base\IlwisDocument.h"
#include "Client\Editors\Domain\DomDoc.h"
#include "Client\Forms\generalformview.h"
#include "Client\Editors\Domain\DomainBoolView.h"
#include "Client\FormElements\formbase.h"
#include "winuser.h"
#include "Headers\constant.h"
#include "Headers\Hs\DAT.hs"
#include "Headers\Hs\DOMAIN.hs"

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DomainBoolView

IMPLEMENT_DYNCREATE(DomainBoolView, GeneralFormView)

DomainBoolView::DomainBoolView()
{
}

DomainBoolView::~DomainBoolView()
{
}

/////////////////////////////////////////////////////////////////////////////
// DomainBoolView diagnostics

#ifdef _DEBUG
void DomainBoolView::AssertValid() const
{
//	GeneralFormView::AssertValid();
}

void DomainBoolView::Dump(CDumpContext& dc) const
{
//	GeneralFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// DomainBoolView implementation

DomainDoc* DomainBoolView::GetDocument() const
{
	return dynamic_cast<DomainDoc*>(CView::GetDocument());
} 

void DomainBoolView::CreateForm()
{
	GeneralFormView::CreateForm();
	Domain dm = GetDocument()->dm();
	DomainBool *pdb = dm->pdbool();

	sTrue = pdb->sValue(1, 0);
	sFalse = pdb->sValue(0, 0);

	if (dm->fReadOnly())
	{
		String s("%S: %S", TR("'&True' string"), sTrue);
		new StaticText(root, s);
		s = String("%S: %S", TR("'&False' string"), sFalse);
		new StaticText(root, s);
	}
	else {
		new FieldBlank(root);
		fsTrue = new FieldString(root, TR("'&True' string"), &sTrue);
		fsFalse = new FieldString(root, TR("'&False' string"), &sFalse);
		fsTrue->SetCallBack((NotifyProc)&DomainBoolView::CheckYesNoEqual);
		fsFalse->SetCallBack((NotifyProc)&DomainBoolView::CheckYesNoEqual);
	}  
	if (dm->fSystemObject())
		SetMenHelpTopic("");
	else if (dm->fReadOnly())
		SetMenHelpTopic("");
	else 
		SetMenHelpTopic("ilwismen\\edit_domain_bool.htm");
}

int DomainBoolView::exec()
{
	if ( !fDataHasChanged() ) return 0;

	GeneralFormView::exec();
	Domain dm = GetDocument()->dm();
	if (dm->fReadOnly()) 
		return 0;

	DomainBool* pdb = dm->pdbool();
	if (pdb == 0)
		return 0;

	pdb->SetBoolString(1, sTrue);
	pdb->SetBoolString(0, sFalse);

	dm->fChanged = true;
	return 0;
}

int DomainBoolView::CheckYesNoEqual(Event *)
{
	if (fCIStrEqual(sTrue, sFalse))
		DisableOK();
	else
	{
		EnableOK();
		DataHasChanged(true);
	}
	return 0;
}
