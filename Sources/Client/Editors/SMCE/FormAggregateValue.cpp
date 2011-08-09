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
#include "Client\Headers\formelementspch.h"
#include "Client\Editors\SMCE\FormAggregateValue.h"
#include "Client\FormElements\FieldMultiObjectSelect.h"
#include "Client\FormElements\FldOneSelectTextOnly.h"
#include "Engine\SpatialReference\Gr.h"
#include "Client\ilwis.h"
#include "Engine\Scripting\CalculateAggregateValue.h"
#include "Client\Editors\SMCE\FormAggregateValueMulti.h"
#include "Headers\Hs\Appforms.hs"


AggregateValueAdditional::AggregateValueAdditional() :
  rPower(2.0),
  iBooleanChoice(0),
  fAdditional(false),
  fldCol(0)
{
}

String AggregateValueAdditional::sGetAttributeColumn()
{
	return sAttributeColumn;
}	  

void AggregateValueAdditional::SetAdditionalBlock(FormEntry *feAbove, FormEntry *feAlignUnder, bool fMulti)
{
	frPower = new FieldReal(feAbove, TR("&Power"), &rPower, ValueRange(1.0001, 1000.0, 0));
	frPower->Align(feAlignUnder, AL_UNDER);
	frPower->Hide();

	cbAdditional = new CheckBox(feAbove, TR("Additional para&meters"), &fAdditional );
	if ( fMulti)
		cbAdditional->SetIndependentPos();
	cbAdditional->Align(feAlignUnder, AL_UNDER);
	
	rgBoolean = new RadioGroup(cbAdditional, "", &iBooleanChoice);
	if ( fMulti)
		rgBoolean->SetIndependentPos();
	rgBoolean->Align(cbAdditional, AL_UNDER);
	RadioButton *rbMapBoolean = new RadioButton(rgBoolean, TR("Mask &Map"));
	RadioButton *rbMapWeight = new RadioButton(rgBoolean, TR("&Weight Map"));	
	RadioButton *rbExpression = new RadioButton(rgBoolean, TR("Mask &Expression"));	
	
	fsExpression = new FieldString(rbExpression, "", &sBooleanExpression, Domain(), false);
    fsExpression->Align(rbExpression, AL_AFTER);
	fsExpression->SetWidth(100);

	fmMapBoolean  = new FieldMap(rbMapBoolean, "", &sMapName, new MapListerDomainType(dmBOOL, false));
	fmMapBoolean->Align(rbMapBoolean, AL_AFTER);

	fmMapWeight  = new FieldMap(rbMapWeight, "", &sMapName, new MapListerDomainType(dmVALUE, false));
	fmMapWeight->Align(rbMapWeight, AL_AFTER);	
}

FieldAggregateValueFuncSimple::FieldAggregateValueFuncSimple(FormEntry* par, String* sFunc, const dmType fd, const String& sDef)
  : FieldOneSelect(par, &iSelected, false), sSelectedFunc(sFunc), sDefault(sDef)
	, m_FunctionDomain(fd)
{
    SetWidth(75);
}

void FieldAggregateValueFuncSimple::create() 
{
	FieldOneSelect::create();
	if (((m_FunctionDomain & dmVALUE) == dmVALUE) || ((m_FunctionDomain & dmBOOL) == dmBOOL) || ((m_FunctionDomain & dmIMAGE) == dmIMAGE))
	{
		ose->AddString("Average.fun");
		ose->AddString("Sum.fun");
		ose->AddString("Minimum.fun");
		ose->AddString("Maximum.fun");
	}
	if (((m_FunctionDomain & dmCLASS) == dmCLASS) || ((m_FunctionDomain & dmIDENT) == dmIDENT))
	{
		ose->AddString("ShapeIndex.fun");
		ose->AddString("ConnectivityIndex.fun");
	}

	int iRet = 0;
	if (sDefault.length() > 0)
		iRet = ose->FindString(-1, sDefault.c_str());
	if (iRet != CB_ERR)
		ose->SetCurSel(iRet);
	else	
		ose->SetCurSel(0);
}

void FieldAggregateValueFuncSimple::StoreData()
{
	FieldOneSelect::StoreData();
	if ( iSelected < 0 )
		return ;
	CString str;
	int n = ose->GetLBTextLen( iSelected );
    ose->GetLBText( iSelected, str.GetBuffer(n) );
	String s(str);
	s = s.sHead(".");
	*sSelectedFunc = s;


	return ;
}

FieldAggregateValueFunc::FieldAggregateValueFunc(FormEntry* parent, const String& sQuestion, String* sFunc, const dmType fd, const String& sDefault)
: FieldGroup(parent)
{
  if (sQuestion.length() != 0)
    new StaticTextSimple(this, sQuestion);
  fafs = new FieldAggregateValueFuncSimple(this, sFunc, fd, sDefault);
  if (children.iSize() > 1) // also static text
    children[1]->Align(children[0], AL_AFTER);
}


void FieldAggregateValueFunc::SetCallBack(NotifyProc np) 
{ 
  fafs->SetCallBack(np); 
  FormEntry::SetCallBack(np);

}

void FieldAggregateValueFunc::SetCallBack(NotifyProc np, CallBackHandler* cb) 
{ 
  fafs->SetCallBack(np, cb); 
  FormEntry::SetCallBack(np, cb);
}

//----
FieldColumnWithNone::FieldColumnWithNone(FormEntry* f, const String& sQuestion, const Table& t, String *psName, long types) :
	FieldColumn(f, sQuestion, t, psName, types)
{
	
}

void FieldColumnWithNone::create()
{
	FieldColumn::create();
	fld->ose->AddString("<None>");
}

void FieldColumnWithNone::FillWithColumns(const FileName& fnTbl, long types)
{
	FieldColumn::FillWithColumns(fnTbl, types)	;
	if ( fld->ose->FindString(-1, "<None>") == CB_ERR )
	{
		fld->ose->AddString("<None>");	
	}	
}

void FieldColumnWithNone::FillWithColumns(const FileName& fn)
{
	FieldColumn::FillWithColumns(fn);
	fld->ose->AddString("<None>");		
}

void FieldColumnWithNone::FillWithColumns(const FileName& fn, const Domain& dmCol)
{
	FieldColumn::FillWithColumns(fn, dmCol);	
}

void FieldColumnWithNone::SelectNone()
{
	if ( fld->ose->FindString(-1, "<None>") != CB_ERR )
	{
		fld->ose->SelectString(-1, "<None>");	
	}	
}

void FieldColumnWithNone::StoreData()
{
	if ( fld->ose->GetCurSel() != fld->ose->GetCount() - 1) // <None> is always at the end
		if ( fld->ose->GetCurSel() != CB_ERR )
			FieldColumn::StoreData();
}
