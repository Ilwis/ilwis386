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
// DomainValueFormView.cpp: implementation of the DomainValueFormView class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Engine\Base\strtod.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\Forms\generalformview.h"
#include "Client\FormElements\fentvalr.h"
#include "Client\Editors\Domain\DomDoc.h"
#include "Client\Editors\Domain\DomainValueFormView.h"
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

IMPLEMENT_DYNCREATE(DomainValueFormView, GeneralFormView)


DomainValueFormView::DomainValueFormView()
{
}

DomainValueFormView::~DomainValueFormView()
{
}

void DomainValueFormView::CreateForm()
{
	GeneralFormView::CreateForm();
	Domain dm = GetDocument()->dm();
	iWidth = dm->iWidth();
	DomainValueReal* dvr = dm->pdvr();
	vr = ValueRange(dvr->rrMinMax(), dvr->rStep());
	if (dm->fReadOnly())
	{
		String s("%S: %S", SDMUiMinMax, vr->sRange());
		new StaticText(root, s);
		if (dvr->fFixedRange())
			new StaticText(root, SDMUiFixedRange);
		if (dvr->fFixedPrecision())
			new StaticText(root, SDMUiFixedPrecision);
		s = String("%S: %i", SDMUiWidth, iWidth);
		new StaticText(root, s);

		if (dvr->m_fUnit)
		{
			s = String("%S: %S", SDMUiUnit, dvr->m_sUnit);
			new StaticText(root, s);
		}
	}
	else
	{
		fvr = new FieldValueRange(root, SDMUiMinMax, &vr, 0);
		fvr->Align(root, AL_UNDER);
		
		CheckBox* cb = new CheckBox(root, SDMUiFixedRange, &dvr->m_fFixedRange);
		cb->SetIndependentPos();
		cb = new CheckBox(root, SDMUiFixedPrecision, &dvr->m_fFixedPrecision);
		cb->SetIndependentPos();
		
		cb = new CheckBox(root, SDMUiUnit, &dvr->m_fUnit);
		new FieldString(cb, "", &dvr->m_sUnit);
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

int DomainValueFormView::exec()
{
	if ( !fDataHasChanged() ) return 0;
	
	GeneralFormView::exec();
	Domain dm = GetDocument()->dm();
	if (dm->fReadOnly()) return 0;
	DomainValueReal* dvr = dm->pdvr();
	dvr->_rMin = vr->rrMinMax().rLo();
	dvr->_rMax = vr->rrMinMax().rHi();
	dvr->_rStep = vr->rStep();
	dvr->init();
	dvr->SetWidth(iWidth);
	dm->fChanged = true;
	return 0;
}

int DomainValueFormView::CallBackVrrChange(Event*) 
{
	fvr->StoreData();
	fiWidth->SetVal(vr->iWidth());
	DataHasChanged(true);
	return 0;
}

