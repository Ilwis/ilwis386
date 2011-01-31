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
// FieldRealSlider.cpp: implementation of the FieldRealSlider class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\FieldIntSlider.h"
#include "Client\FormElements\FieldRealSlider.h"



FieldRealSlider::FieldRealSlider(FormEntry* p, double *prVal, const ValueRange& valrr, DWORD dwStyle)
: FormEntry(p, 0, true)
, _prVal (prVal)
, _rVal (*prVal)
, slc(0)
, vrr(valrr)
, m_dwStyle(dwStyle)
{
	if (m_dwStyle == TBS_HORZ)
	{
		psn->iMinWidth = 120;//FLDSLIDERWIDTH;
		psn->iMinHeight = 30;//FLDSLIDERHEIGHT;
	}
	else // TBS_VERT
	{
		psn->iMinWidth = 30;//FLDSLIDERHEIGHT;
		psn->iMinHeight = 120;//FLDSLIDERWIDTH;
	}
}

FieldRealSlider::~FieldRealSlider()
{
	if (slc)
		delete slc;
}

void FieldRealSlider::create()
{
  zPoint pntFld = zPoint(psn->iPosX, psn->iPosY);
  zDimension dimFld = zDimension(psn->iMinWidth, psn->iMinHeight);

	double rLo = vrr->rrMinMax().rLo();
	double rHi = vrr->rrMinMax().rHi();

	slc = new OwnSliderCtrl(this);
	slc->Create(m_dwStyle,	CRect(pntFld, dimFld), _frm->wnd(), Id());
	slc->SetRange(0, 1000);
	slc->SetPos(scaleValue());
	slc->setContinuous(continuous);

  CreateChildren();
}

double FieldRealSlider::scaleValue() const {
	double v =  1000.0 * ((_rVal - vrr->rrMinMax().rLo()) / vrr->rrMinMax().rWidth());
	return v;
}

void FieldRealSlider::show(int sw)
{
	if (slc)
		slc->ShowWindow(sw);
}

void FieldRealSlider::SetVal(double rVal)
{ 
  _rVal = rVal;
  if (0 == slc)
    return;
  if ((0 != slc) && (rVal != iUNDEF) && (vrr->rrMinMax().fContains(rVal)))
    slc->SetPos(scaleValue());
  if (_npChanged)
    (_cb->*_npChanged)(0);
}

double FieldRealSlider::rVal()
{
  _rVal = slc->GetPos();
  _rVal /= 1000.0;
  _rVal *= vrr->rrMinMax().rWidth();
  _rVal += vrr->rrMinMax().rLo();
  return _rVal;
}

void FieldRealSlider::StoreData()
{
  if (fShow())
	{
    if (slc)
		{
			_rVal = rVal();
			*_prVal = _rVal;
		}
  }
  FormEntry::StoreData();
}

void FieldRealSlider::Enable()
{
	if (slc) slc->EnableWindow(TRUE);
}

void FieldRealSlider::Disable()
{
	if (slc) slc->EnableWindow(FALSE);
}

void FieldRealSlider::SetFocus()
{
  if (slc)
    slc->SetFocus();
}

FormEntry* FieldRealSlider::CheckData()
{
  if (!fShow()) return 0;
  if (slc) 
  {
		_rVal = rVal();
		if (!vrr->rrMinMax().fContains(_rVal))
			return this;
  }  
  return FormEntry::CheckData();
}

//---------------------------------------------------
FieldRealSliderEx::FieldRealSliderEx(FormEntry * parent, const String& question, double *val, const ValueRange& valrange, bool txt) : 
	FormEntry(parent,0,true),
	edit(0),
	slider(0),
	initial(true),
	setRace(-1),
	continuous(0),
	rangeText(txt)
{
	fg = new FieldGroup(parent);
	edit = new FieldReal(fg,question,val,valrange);
	slider = new FieldRealSlider(fg,val,valrange,TBS_HORZ);
	slider->Align(edit,AL_AFTER);
	if ( rangeText) {
		StaticText *st = new StaticText(fg,String("(%S)",valrange->sRange()));
		st->Align(slider, AL_AFTER);
	}
		
	FieldBlank *fb = new FieldBlank(fg,0.0);
	fb->Align(edit, AL_UNDER);
	

}

FieldRealSliderEx::~FieldRealSliderEx(){
}

void FieldRealSliderEx::create(){
	FormEntry::CreateChildren();
	slider->SetCallBack((NotifyProc)&FieldRealSliderEx::SliderCallBackFunc,this);
	edit->SetCallBack((NotifyProc)&FieldRealSliderEx::EditCallBackFunc,this);
	initial = false;
}

void FieldRealSliderEx::show(int sw){
	fg->show(sw);
	if (edit)
		edit->show(sw);
	if ( slider)
		slider->show(sw);
}

void FieldRealSliderEx::setContinuousMode(int m) {
	continuous = m;
}


void FieldRealSliderEx::SetVal(double rVal){
	edit->SetVal(rVal);
	slider->SetVal(rVal);
}
double FieldRealSliderEx::rVal(){
	return slider->rVal();
}
void FieldRealSliderEx::StoreData(){
	edit->StoreData();
}
void FieldRealSliderEx::Enable(){
	edit->Enable();
	slider->Enable();
}
void FieldRealSliderEx::Disable(){
	edit->Disable();
	slider->Disable();
}
void FieldRealSliderEx::SetFocus(){
	edit->SetFocus();
}
FormEntry* FieldRealSliderEx::CheckData(){
	FormEntry *frm = edit->CheckData();
	if ( frm)
		return frm;
	frm = slider->CheckData();
	return frm;
}

int FieldRealSliderEx::SliderCallBackFunc(Event *ev) {
	if ( initial || setRace == 1)
		return 1;
	setRace = 0;
	double val = slider->rVal();
	edit->SetVal(val);
	setRace = -1;
	if ( _npChanged)
		(_cb->*_npChanged)(ev);
	return 1;
}

int FieldRealSliderEx::EditCallBackFunc(Event *ev) {
	if ( initial || setRace == 0)
		return 1;
	setRace = 1;
	double val = edit->rVal();
	slider->SetVal(val);
	if ( _npChanged)
		(_cb->*_npChanged)(ev);
	setRace = -1;
	return 1;
}
