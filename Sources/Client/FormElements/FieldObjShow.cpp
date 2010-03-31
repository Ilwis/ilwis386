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
// FieldObjShow.cpp: implementation of the FieldObjShow class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Headers\messages.h"
#include "Client\FormElements\FieldObjShow.h"
#include "Client\ilwis.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//-------------------------------
// class FieldObjShow
FieldObjShow::FieldObjShow(FormEntry* fe, const IlwisObject& obj)
	: FieldGroup(fe, true)
{
	if (!obj.fValid())
		return;
	fnObj = obj->fnObj;

	String sType = obj->sTypeName();
	String sDsc = obj->sDescription;
	bool fTwoLines = sDsc.length() > 40;

	if (sDsc != sType)
	{
		if (fTwoLines)   // put on two lines
		{
			if (sDsc.length() > 80)
			{
				sDsc = sDsc.sLeft(80);
				sDsc &= "...";
			}
		}
	}
	else
	{
		sDsc = String();
		fTwoLines = false;
	}

	// String sType: result of sTypeName
	// String sDsc:  contains description if not equal to sTypeName
	// bool fTwoLines: is true is sDsc is too long to be placed after sType
	FormEntry* feAfter = 0;
	FormEntry* feUnder = 0;

	int iImage = IlwWinApp()->iImage(fnObj.sExt);
	if (iImage > 0)
	{
		HICON icon = IlwWinApp()->ilSmall.ExtractIcon(iImage);
		feAfter = new FlatIconButton(this, icon, sType, (NotifyProc)&FieldObjShow::PressButton, fnObj, true);
		feAfter->SetIndependentPos();
		if (fTwoLines)
		{
			feUnder = feAfter;
			feAfter = 0;
		}
	}
	else  // internal object or non-Ilwis object
	{
		if (fTwoLines)  // two lines are needed
		{
			feUnder = new StaticText(this, sType);
			feUnder->psn->SetBound(0,0,0,0);
		}
		else
			sDsc = String("%S.  %S", sType, sDsc);
	}
	if (sDsc.length() > 0)
	{
		StaticText* st = new StaticText(this, sDsc);
		if (feAfter)
		{
			st->psn->iHeight = feAfter->psn->iHeight; // necessary to properly align fields with different heights
			st->Align(feAfter, AL_AFTER);
			FieldBlank* fb = new FieldBlank(this, 0);
			fb->Align(feAfter, AL_UNDER);
		}
		else if (feUnder)
			st->Align(feUnder, AL_UNDER);
	}
}

int FieldObjShow::PressButton(Event*) 
{
	String sCmd("show %S", fnObj.sFullNameQuoted());
	char* str = sCmd.sVal();
	IlwWinApp()->GetMainWnd()->SendMessage(ILWM_EXECUTE, 0, (LPARAM)str);

	return 0;
}

FieldObjShow::~FieldObjShow()
{

}
