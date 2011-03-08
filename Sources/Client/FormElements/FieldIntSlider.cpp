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
// FieldIntSlider.cpp: implementation of the FieldIntSlider class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\FieldIntSlider.h"

BEGIN_MESSAGE_MAP(OwnSliderCtrl, CSliderCtrl)
	ON_WM_HSCROLL_REFLECT()
	ON_WM_VSCROLL_REFLECT()
END_MESSAGE_MAP()

OwnSliderCtrl::OwnSliderCtrl(FormEntry *f)
: CSliderCtrl()
, BaseZapp(f),
continuous(false)
{
}

void OwnSliderCtrl::HScroll(UINT nSBCode, UINT nPos)
{
	if (nSBCode == TB_THUMBTRACK && continuous) {
		fProcess(NotificationEvent(GetDlgCtrlID(), TB_THUMBTRACK, (LPARAM)m_hWnd));
	} 
	if (nSBCode == TB_ENDTRACK)
		fProcess(NotificationEvent(GetDlgCtrlID(), TB_ENDTRACK, (LPARAM)m_hWnd));
}

void OwnSliderCtrl::VScroll(UINT nSBCode, UINT nPos)
{
	if (nSBCode == TB_ENDTRACK)
		fProcess(NotificationEvent(GetDlgCtrlID(), TB_ENDTRACK, (LPARAM)m_hWnd));
	if (nSBCode == TB_THUMBTRACK && continuous) {
		fProcess(NotificationEvent(GetDlgCtrlID(), TB_THUMBTRACK, (LPARAM)m_hWnd));
	} 
}

//------------------------------------------------------------

FieldIntSlider::FieldIntSlider(FormEntry* p, int *piVal, const ValueRange& valri, DWORD dwStyle)
: FormEntry(p, 0, true)
, _piVal (piVal)
, _iVal (*piVal)
, slc(0)
, vri(valri)
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

FieldIntSlider::~FieldIntSlider()
{
	if (slc)
		delete slc;
}

void FieldIntSlider::create()
{
  zPoint pntFld = zPoint(psn->iPosX, psn->iPosY);
  zDimension dimFld = zDimension(psn->iMinWidth, psn->iMinHeight);

	int iLo = vri->riMinMax().iLo();
	int iHi = vri->riMinMax().iHi();

	slc = new OwnSliderCtrl(this);
	slc->Create(m_dwStyle,	CRect(pntFld, dimFld), _frm->wnd(), Id());
	slc->SetRange(iLo, iHi);
	slc->SetPos(_iVal);
	slc->setContinuous(continuous);

  CreateChildren();
}

void FieldIntSlider::show(int sw)
{
	if (slc)
		slc->ShowWindow(sw);
}

void FieldIntSlider::SetVal(int iVal)
{ 
  _iVal = iVal;
  if (0 == slc)
    return;
  if ((0 != slc) && (iVal != iUNDEF) && (vri->riMinMax().fContains(iVal)))
    slc->SetPos(iVal);
  if (_npChanged)
    (_cb->*_npChanged)(0);
}

int FieldIntSlider::iVal()
{
	_iVal = slc->GetPos();
  return _iVal;
}

void FieldIntSlider::StoreData()
{
  if (fShow())
	{
    if (slc)
		{
			_iVal = slc->GetPos();
			*_piVal = _iVal;
		}
  }
  FormEntry::StoreData();
}

void FieldIntSlider::Enable()
{
	if (slc) slc->EnableWindow(TRUE);
}

void FieldIntSlider::Disable()
{
	if (slc) slc->EnableWindow(FALSE);
}

void FieldIntSlider::SetFocus()
{
  if (slc)
    slc->SetFocus();
}

FormEntry* FieldIntSlider::CheckData()
{
  if (!fShow()) return 0;
  if (slc) 
  {
		_iVal = slc->GetPos();
			if (!vri->riMinMax().fContains(_iVal))
				return this;
  }  
  return FormEntry::CheckData();
}

//---------------------------------------------------
FieldIntSliderEx::FieldIntSliderEx(FormEntry * parent, const String& question, int *val, const ValueRange& valrange, bool rangeText) : 
	FormEntry(parent,0,false),
	edit(0),
	slider(0),
	initial(true),
	setRace(-1)
{
	FieldGroup *fg = new FieldGroup(this);
	new FieldBlank(fg,0.1);
	edit = new FieldInt(fg,question,val,valrange);
	slider = new FieldIntSlider(fg,val,valrange,TBS_HORZ);
	slider->Align(edit,AL_AFTER);
	if ( rangeText) {
		StaticText *st = new StaticText(fg,String("(%S)",valrange->sRange()));
		st->Align(slider, AL_AFTER);
	}
	psn->iMinWidth = 75;
	fg->SetIndependentPos();
	// new FieldBlank(fg);

}

FieldIntSliderEx::~FieldIntSliderEx(){
}

void FieldIntSliderEx::create(){
	FormEntry::CreateChildren();
	if ( slider)
		slider->SetCallBack((NotifyProc)&FieldIntSliderEx::SliderCallBackFunc,this);
	if (edit)
		edit->SetCallBack((NotifyProc)&FieldIntSliderEx::EditCallBackFunc,this);
	initial = false;
}

void FieldIntSliderEx::show(int sw){
	if (edit)
		edit->show(sw);
	if ( slider)
		slider->show(sw);
}
void FieldIntSliderEx::SetVal(int iVal){
	edit->SetVal(iVal);
	slider->SetVal(iVal);
}
int FieldIntSliderEx::iVal(){
	return slider->iVal();
}
void FieldIntSliderEx::StoreData(){
	if (edit)
		edit->StoreData();
}
void FieldIntSliderEx::Enable(){
	if (edit)
		edit->Enable();
	if ( slider)
		slider->Enable();
}
void FieldIntSliderEx::Disable(){
	edit->Disable();
	if ( slider)
		slider->Disable();
}
void FieldIntSliderEx::SetFocus(){
	if (edit)
		edit->SetFocus();
}
FormEntry* FieldIntSliderEx::CheckData(){
	FormEntry *frm = edit->CheckData();
	if ( frm)
		return frm;
	if ( slider)
		frm = slider->CheckData();
	return frm;
}

int FieldIntSliderEx::SliderCallBackFunc(Event *ev) {
	if ( initial || setRace == 1)
		return 1;
	setRace = 0;
	if ( slider){
		int val = slider->iVal();
		if (edit)
			edit->SetVal(val);
	}
	setRace = -1;
	if ( _npChanged)
		(_cb->*_npChanged)(ev);
	return 1;
}

int FieldIntSliderEx::EditCallBackFunc(Event *ev) {
	if ( initial || setRace == 0)
		return 1;
	setRace = 1;
	if (edit) {
		int val = edit->iVal();
		if ( slider)
			slider->SetVal(val);
	}
	setRace = -1;
	return 1;
}
