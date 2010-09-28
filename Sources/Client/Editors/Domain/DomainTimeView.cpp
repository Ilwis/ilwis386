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
// DomainTimeView.cpp: implementation of the DomainTimeView class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\Forms\generalformview.h"
#include "Client\FormElements\fentvalr.h"
#include "Client\Editors\Domain\DomDoc.h"
#include "Client\Editors\Domain\DomainTimeView.h"
#include "Headers\Hs\DOMAIN.hs"
#include "Engine\Domain\DomainTime.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(DomainTimeView, GeneralFormView)

using namespace ILWIS;

DomainTimeView::DomainTimeView() : fInterval(0)
{
}

DomainTimeView::~DomainTimeView()
{
}

void DomainTimeView::CreateForm()
{
	GeneralFormView::CreateForm();
	Domain dm = GetDocument()->dm();
	DomainTime* dvt = dm->pdtime();
	interval = dvt->getInterval();
	mode = dvt->getMode();
	if (dm->fReadOnly())
	{
		String s("%S: %S", SDMUiMinMax, interval.toString(dvt->isLocalTime(), mode) );
		new StaticText(root, s);
	}
	else
	{
		switch ( mode) {
			case Time::mDATETIME:
				timeFormat = 0; break;
			case Time::mDATE:
				timeFormat = 1; break;
			case Time::mTIME:
				timeFormat = 2;
		}
		FieldGroup *fg = new FieldGroup(root, true);
		rgTime = new RadioGroup(fg, "",&timeFormat, true);
		rgTime->SetIndependentPos();
		RadioButton * rbDateTime = new RadioButton(rgTime,"Date & Time");
		RadioButton * rbDateOnly = new RadioButton(rgTime,"Date only");
		RadioButton * rbTimeOnly = new RadioButton(rgTime,"Day Time only");
		rgTime->SetCallBack((NotifyProc)&DomainTimeView::SetUseDate, this);
		fInterval =  new FieldTimeInterval(fg, "", &interval,0, &mode);
		FieldBlank *fb = new FieldBlank(root);
	}
	if (dm->fSystemObject())
		SetMenHelpTopic(htpShowDomSystem);
	else if (dm->fReadOnly())
		SetMenHelpTopic(htpShowDomReadOnly);
	else 
		SetMenHelpTopic(htpEditDomValue);
}

int DomainTimeView::exec()
{
	//if ( !fDataHasChanged() ) return 0;
	
	GeneralFormView::exec();
	Domain dm = GetDocument()->dm();
	if (dm->fReadOnly()) return 0;
	fInterval->StoreData();
	DomainTime* dvt = dm->pdtime();
	dvt->_rMin = interval.getBegin();
	dvt->_rMax = interval.getEnd();
	dvt->_rStep = interval.getStep();
	dvt->setMode(mode);
	dm->fChanged = true;
	dvt->Store();
	return 0;
}

int DomainTimeView::SetUseDate(Event *ev) {
	if ( rgTime) {
		rgTime->StoreData();
		//fInterval->StoreData();
		if ( timeFormat == 0)
			fInterval->setMode(ILWIS::Time::mDATETIME);
		else if ( timeFormat == 1)
			fInterval->setMode(ILWIS::Time::mDATE);
		else if ( timeFormat == 2) {
			fInterval->setMode(ILWIS::Time::mTIME);
		}
	}
	return 1;
}

