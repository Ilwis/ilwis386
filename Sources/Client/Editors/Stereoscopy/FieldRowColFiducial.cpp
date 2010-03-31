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
// FieldRowColFiducial.cpp: implementation of the FieldRowColFiducial class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Client\Editors\Stereoscopy\MakeEpiPolarFormView.h"
#include "Client\Editors\Stereoscopy\FieldRowColFiducial.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

FieldRowColFiducial::FieldRowColFiducial(FormEntry* par, int iNr, String sQuestion)
: FieldGroup(par)
{
	fOk = false;
  String s("&%1i", iNr);
	if (sQuestion.length()>0)
		cb = new CheckBox(this, sQuestion, &fOk);
	else	
		cb = new CheckBox(this, s, &fOk);
	//String sDummy = "1234.5";
	frc = new FieldRowCol(this, "", &rcRowCol);
	frc->SetWidth(MakeEpipolarFormView::iFieldSize);
	frc->Align(cb, AL_AFTER);
	frc->psn->SetBound(0,0,0,0);
	cb->psn->SetBound(0,0,0,0);
	fAct = false;
}

FieldRowColFiducial::~FieldRowColFiducial()
{

}

void FieldRowColFiducial::create()
{
  FieldGroup::create();
  cb->setNotifyFocusChange(this, (NotifyProc)&FieldRowColFiducial::FocusChange);
}

void FieldRowColFiducial::SetCallBack(NotifyProc np)
{
  cb->SetCallBack(np);
	frc->SetCallBack(np);
}

void FieldRowColFiducial::SetCallBack(NotifyProc np, CallBackHandler* evh)
{
  cb->SetCallBack(np,evh);
	frc->SetCallBack(np,evh);
}

int FieldRowColFiducial::FocusChange(Event* Evt)
{
  //if (Evt->iMessage == WM_SETFOCUS)
  //  ((MakeEpipolarFormView*)frm())->SetActDigPoint(this);
  return 0;
}
