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
// DomainValueIntFormView.cpp: implementation of the DomainValueIntFormView class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\Forms\generalformview.h"
#include "Client\FormElements\fentvalr.h"
#include "Client\Editors\Domain\DomDoc.h"
#include "Client\Editors\Domain\DomainValueIntFormView.h"
#include "Headers\Hs\DOMAIN.hs"
#include "Engine\Domain\Dmvalue.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(DomainValueIntFormView, GeneralFormView)


DomainValueIntFormView::DomainValueIntFormView()
{
}

DomainValueIntFormView::~DomainValueIntFormView()
{
}

void DomainValueIntFormView::CreateForm()
{
	GeneralFormView::CreateForm();
	Domain dm = GetDocument()->dm();
	iWidth = dm->iWidth();
	DomainValueInt* dvi = dm->pdvi();
	ValueRange vr(dvi->riMinMax());
	ri = vr->riMinMax();
	if (dm->fReadOnly())
	{
		String s("%S: %S", SDMUiMinMax, vr->sRange());
		new StaticText(root, s);
		if (dvi->fFixedRange())
			new StaticText(root, SDMUiFixedRange);
		s = String("%S: %i", SDMUiWidth, iWidth);
		new StaticText(root, s);

		if (dvi->m_fUnit)
		{
			s = String("%S: %S", SDMUiUnit, dvi->m_sUnit);
			new StaticText(root, s);
		}
	}
	else
	{
		fri = new FieldRangeInt(root, SDMUiMinMax, &ri);
		fri->Align(root, AL_UNDER);
		
		CheckBox* cb = new CheckBox(root, SDMUiFixedRange, &dvi->m_fFixedRange);
		cb->SetIndependentPos();
		
		cb = new CheckBox(root, SDMUiUnit, &dvi->m_fUnit);
		new FieldString(cb, "", &dvi->m_sUnit);
		fiWidth = new FieldInt(root, SDMUiWidth, &iWidth);
		fiWidth->Align(cb, AL_UNDER);
	}
	if (dm->fSystemObject())
		SetMenHelpTopic("");
	else if (dm->fReadOnly())
		SetMenHelpTopic("");
	else 
		SetMenHelpTopic("ilwismen\\edit_domain_value.htm");
}

int DomainValueIntFormView::exec()
{
	if ( !fDataHasChanged() ) return 0;
	
	GeneralFormView::exec();
	Domain dm = GetDocument()->dm();
	if (dm->fReadOnly()) return 0;
	DomainValueInt* dvi = dm->pdvi();
	dvi->_iMin = ri.iLo();
	dvi->_iMax = ri.iHi();
	dvi->init();
	dvi->SetWidth(iWidth);
	dm->fChanged = true;
	return 0;
}

int DomainValueIntFormView::CallBackVriChange(Event*) 
{
	//  fri->StoreData();
	//  fiWidth->SetVal(vr->iWidth());
	return 0;
}

