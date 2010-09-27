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
/* Miscellaneous  Form entries

// by Wim Koolhoven, aug. 1993
// changed by Jelle Wind, dec. 1993 - june 1994
	Last change:  WK   11 Aug 98   10:25 am
*/

#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\CalendarCtrl.h"
#include "Engine\Domain\DomainTime.h"
#include "Client\ilwis.h"



FieldGroup::FieldGroup(FormEntry* p, bool fIndependentPositioning)
  : FieldGroupSimple(p, fIndependentPositioning, true) {}


FieldBlank::FieldBlank(FormEntry* p, double rRatio): FormEntry(p, 0, true)
{
  psn->iMinWidth = XDIALOG(_frm->iDefFldWidth());
  psn->iHeight = psn->iMinHeight = max(1, YDIALOG( (int)(rRatio * _frm->iDefFldHeight())));
};

//--------------------------------------------------------------------------------------------------------------
FieldBlank::~FieldBlank()
{}

String FieldBlank::sGetText()
{
    return ""; //"\r\n";
}

void FieldBlank::create()
{
//	FormEntry::create();
	CreateChildren();
}

//--------------------------------------------------------------------------------------------------------------
StaticText::StaticText(FormEntry *p, const String& sQuestion, bool fB, bool fRemPrefix)
  : StaticTextSimple(p, sQuestion, true, fRemPrefix),
	  fBold(fB),
		fDeleteFont(false) // default = incoming font is created and deleted elsewhere
{ }

StaticText::~StaticText()
{
  if (fnt && fDeleteFont)
    delete fnt;
}

void StaticText::create()
{
	StaticTextSimple::create();
	if ( fBold)
		Bold();
	if (fnt)
		txt->SetFont(fnt);
	CreateChildren();
}

void StaticText::Font(CFont *font, bool fDelete)
{
	if (fnt && fDeleteFont)
		delete fnt;
	fDeleteFont = fDelete;
	fnt = font;
	String sTmp = sText;
	if (sTmp.length() == 0)
		sTmp = String('X', 50);
	zDimension dm = Dim(sTmp, fnt);
	psn->iMinWidth = dm.width();
	psn->iHeight = psn->iMinHeight = dm.height();
	if (txt)
		txt->SetWindowPos(0, 0, 0, dm.width(), dm.height(), SWP_NOMOVE);
};

void StaticText::FontEnlarge(float rFac)
{
  zFontInfo *info;
  if (!fnt)
    info = new zFontInfo(_frm->windsp());
  else {
    _frm->windsp()->pushFont(fnt);
    info = new zFontInfo(_frm->windsp());
    _frm->windsp()->popFont();
  }
  CFont *fnt= new CFont();
  fnt->CreateFont(info->height()*rFac, 0, 0, 0, info->weight(), 0, 0, 0, ANSI_CHARSET,
                 OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, 
                 BYTE(info->pitch() | info->family()),"");
  Font(fnt);
	delete info;
}

void StaticText::Bold()
{
	CFont *oldFont = fnt;
	if ( oldFont == 0)
		oldFont = IlwWinApp()->GetFont(IlwisWinApp::sfFORM);
	LOGFONT lf;
	oldFont->GetLogFont(&lf);
	lf.lfWeight = FW_BOLD;
	CFont *font = new CFont();
	font->CreateFontIndirect(&lf);
	Font(font);
}


StaticIcon::StaticIcon(FormEntry* p, HICON icn, bool fLrg)
: FormEntry(p, 0, true)
{
	icon = icn;
	fLarge = fLrg;
  psn->iMinWidth = fLarge ? 32 : 16;
}

StaticIcon::~StaticIcon()
{
	if ( st )
		delete st;
}

void StaticIcon::create()
{
  zDimension dimTxt;
	if (fLarge)
		dimTxt = zDimension(32,32);
	else
		dimTxt = zDimension(16,16);
  CPoint pntTxt(psn->iPosX, psn->iPosY);
  pntTxt.y += (psn->iHeight - dimTxt.height()) / 2; // center vertically
  st = new CStatic();
  st->Create("", WS_CHILD|WS_VISIBLE|SS_ICON, CRect(pntTxt, dimTxt), _frm->wnd());
	st->SetIcon(icon);
}

//------------------------------------------------------------------------------------------------------------
FieldString::FieldString(FormEntry* p, const String& sQuestion,
                         Parm *prm, const Domain& dm, bool fAllowEmpty, DWORD style)
 : FormEntry(p, prm, true)
{
  if (sQuestion.length() != 0)
    st = new StaticTextSimple(this, sQuestion);
  else
    st = 0;
  fld = new FieldStringSimple(this, prm, dm, fAllowEmpty, style);
  if (st)
    fld->Align(st, AL_AFTER);

	if (fld->fReadOnly())
	{
			stElipses = new StaticTextSimple(this,"...");
			stElipses->Align(fld, AL_AFTER);
	}
	else
		stElipses = 0;
}

FieldString::FieldString(FormEntry* p, const String& sQuestion,
                   String *psVal, const Domain& dm, bool fAllowEmpty, DWORD style)
  : FormEntry(p, 0, true)
{
  if (sQuestion.length() != 0)
    st = new StaticTextSimple(this, sQuestion);
  else
    st = 0;
  fld = new FieldStringSimple(this, psVal, dm, fAllowEmpty, style);

  if (st)
    fld->Align(st, AL_AFTER);

	if (fld->fReadOnly())
	{
			stElipses = new StaticTextSimple(this,"...");
			stElipses->Align(fld, AL_AFTER);
	}
	else
		stElipses = 0;

}

FieldString::FieldString(FormEntry* p, String *psVal, DWORD style)
  : FormEntry(p, 0, true)
{
  st = 0;
  fld = new FieldStringSimple(this, psVal, style);
	if (fld->fReadOnly())
	{
		stElipses = new StaticTextSimple(this,"...");
		stElipses->Align(fld, AL_AFTER);
	}
	else
		stElipses = 0;
}


FieldString::~FieldString()
{
 // children are removed by ~FormEntry
}

String FieldString::sGetText()
{
    if ( !fShow() ) return "";
    String s1, s2;
    if ( st ) s1 = st->sGetText();
    if ( fld ) s2 = fld->sGetText();
    if ( st ) return String("%S:\t%S", s1, s2);
    return s2;
}

void FieldString::create()
{
  CreateChildren();
	if ( stElipses && !fld->fShowElipses() )
		stElipses->SetVal("");
}

void FieldString::SetFocus() 
{ 
  fld->SetFocus(); 
}   

void FieldString::SetWidth(short iWidth) 
{ 
  fld->SetWidth(iWidth); 
}

void FieldString::SetHeight(short iHeight)
{
  fld->SetHeight(iHeight);
}

void FieldString::SetTextColor(const Color& clr)
{
	fld->SetTextColor(clr);
}

void FieldString::SetBackGroundColor(const Color& clr)
{
	fld->SetBackGroundColor(clr);
}

//--------------------------------------------------------------------------------------------------------------

FieldStringMulti::FieldStringMulti(FormEntry* p, String *psVal, bool fReadOnly)
  : FormEntry(p, 0, true)
{
	DWORD style = WS_GROUP|WS_TABSTOP|ES_MULTILINE|ES_WANTRETURN|
					ES_AUTOVSCROLL|WS_VSCROLL | WS_BORDER;
	if (fReadOnly)
		style |= ES_READONLY;        

	st = 0;
	fld = new FieldStringMultiSimple(this, psVal, style);
}

FieldStringMulti::FieldStringMulti(FormEntry* p, const String& sQuestion, String *psVal, bool fReadOnly)
  : FormEntry(p, 0, true)
{
	DWORD style = WS_GROUP|WS_TABSTOP|ES_MULTILINE|ES_WANTRETURN|
					ES_AUTOVSCROLL|WS_VSCROLL | WS_BORDER;
	if (fReadOnly)
		style |= ES_READONLY;        

	st = 0;
	if (sQuestion.length() != 0)
		st = new StaticTextSimple(this, sQuestion);
	else
		st = 0;
	fld = new FieldStringMultiSimple(this, psVal, style);
	if (st)
		fld->Align(st, AL_AFTER);

}

FieldStringMulti::FieldStringMulti(FormEntry* p, String *psVal, DWORD style)
  : FormEntry(p, 0, true)
{
	if (style == 0)
		style = WS_GROUP|WS_TABSTOP|ES_MULTILINE|ES_WANTRETURN|
				ES_AUTOVSCROLL|WS_VSCROLL|WS_BORDER;
	st = 0;
	fld = new FieldStringMultiSimple(this, psVal, style);
}


FieldStringMulti::~FieldStringMulti()
{
 // children are removed by ~FormEntry
}

void FieldStringMulti::create()
{
  CreateChildren();
}

void FieldStringMulti::SetFocus()
{ 
  fld->SetFocus(); 
}   

void FieldStringMulti::SetWidth(short iWidth) 
{ 
  fld->SetWidth(iWidth); 
}

void FieldStringMulti::SetHeight(short iHeight)
{
  fld->SetHeight(iHeight);
}

void FieldStringMulti::ClearData() {
	fld->SetVal("");
}

String FieldStringMulti::sGetText()
{
    if ( !fShow() ) return "";
    String s1, s2;
    if ( st ) s1 = st->sGetText();
    if ( fld ) s2 = fld->sGetText();
    if ( st ) return String("%S:\t%S", s1, s2);
    return s2;
}

void FieldStringMulti::Enable()
{
	fld->Enable();
}

void FieldStringMulti::Disable()
{
	fld->Disable();
}

void FieldStringMulti::ScrollToTopLine()
{
	if (fld != 0)
		fld->ScrollToTopLine();
}

//-----------------------------------------------------------------------------------------------------[ FieldInt ]------
FieldInt::FieldInt(FormEntry* p, const String& sQuestion,
                   Parm *prm, const Domain& dm, bool fSpinControl)
  : FormEntry(p, prm, true)
{
  if (sQuestion.length() != 0)
    st = new StaticTextSimple(this, sQuestion);
  else
    st = 0;
  fld = new FieldIntSimple(this, prm, dm, fSpinControl);
  if (st)
    fld->Align(st, AL_AFTER);
  SetHelpTopic(htpUiInt);
}

FieldInt::FieldInt(FormEntry* p, const String& sQuestion,
                   byte *pbVal, const ValueRange& vri, bool fSpinControl)
  : FormEntry(p, 0, true)
{
  if (sQuestion.length() != 0)
    st = new StaticTextSimple(this, sQuestion);
  else
    st = 0;
  fld = new FieldIntSimple(this, pbVal, vri, fSpinControl);
  if (st)
    fld->Align(st, AL_AFTER);
  SetHelpTopic(htpUiInt);
}

FieldInt::FieldInt(FormEntry* p, const String& sQuestion,
                   int *piVal, const ValueRange& vri, bool fSpinControl)
  : FormEntry(p, 0, true)
{
  if (sQuestion.length() != 0)
    st = new StaticTextSimple(this, sQuestion);
  else
    st = 0;
  fld = new FieldIntSimple(this, piVal, vri, fSpinControl);
  if (st)
    fld->Align(st, AL_AFTER);
  SetHelpTopic(htpUiInt);
}

FieldInt::FieldInt(FormEntry* p, const String& sQuestion,
                   long *piVal, const ValueRange& vri, bool fSpinControl, bool fAcceptUndef)
  : FormEntry(p, 0, true)
{
  if (sQuestion.length() != 0)
    st = new StaticTextSimple(this, sQuestion);
  else
    st = 0;
  fld = new FieldIntSimple(this, piVal, vri, fSpinControl, fAcceptUndef);
  if (st)
    fld->Align(st, AL_AFTER);
  SetHelpTopic(htpUiInt);
}


FieldInt::FieldInt(FormEntry* p, const String& sQuestion,
                   long *piVal, const ValueRange& vri, bool fSpinControl)
  : FormEntry(p, 0, true)
{
  if (sQuestion.length() != 0)
    st = new StaticTextSimple(this, sQuestion);
  else
    st = 0;
  fld = new FieldIntSimple(this, piVal, vri, fSpinControl);
  if (st)
    fld->Align(st, AL_AFTER);
  SetHelpTopic(htpUiInt);
}

FieldInt::FieldInt(FormEntry* p, const String& sQuestion,
                   byte *pbVal, const Domain& dm, bool fSpinControl)
  : FormEntry(p, 0, true)
{
  if (sQuestion.length() != 0)
    st = new StaticTextSimple(this, sQuestion);
  else
    st = 0;
  fld = new FieldIntSimple(this, pbVal, dm, fSpinControl);
  if (st)
    fld->Align(st, AL_AFTER);
  SetHelpTopic(htpUiInt);
}

FieldInt::FieldInt(FormEntry* p, const String& sQuestion,
                   int *piVal, const Domain& dm, bool fSpinControl)
  : FormEntry(p, 0, true)
{
  if (sQuestion.length() != 0)
    st = new StaticTextSimple(this, sQuestion);
  else
    st = 0;
  fld = new FieldIntSimple(this, piVal, dm, fSpinControl);
  if (st)
    fld->Align(st, AL_AFTER);
  SetHelpTopic(htpUiInt);
}

FieldInt::FieldInt(FormEntry* p, const String& sQuestion,
                   long *piVal, const Domain& dm, bool fSpinControl)
  : FormEntry(p, 0, true)
{
  if (sQuestion.length() != 0)
    st = new StaticTextSimple(this, sQuestion);
  else
    st = 0;
  fld = new FieldIntSimple(this, piVal, dm, fSpinControl);
  if (st)
    fld->Align(st, AL_AFTER);
  SetHelpTopic(htpUiInt);
}


FieldInt::~FieldInt()
{
 // children are removed by ~FormEntry
}

String FieldInt::sGetText()
{
    if ( !fShow() ) return "";
    String s1, s2;
    if ( st ) s1 = st->sGetText();
    if ( fld ) s2 = fld->sGetText();
    if ( st ) return String("%S:\t%S", s1, s2);
    return s2;
}

void FieldInt::create()
{
  CreateChildren();
}

void FieldInt::SetFocus() 
{ 
  fld->SetFocus(); 
}   

void FieldInt::SetWidth(short iWidth) 
{ 
  fld->SetWidth(iWidth); 
}

void FieldInt::Enable()
{
	fld->Enable();
}

void FieldInt::Disable()
{
	fld->Disable();
}

//--------------------------------------------------------------------------------------------[ FieldReal ]---------------
FieldReal::FieldReal(FormEntry* p, const String& sQuestion, Parm *prm,
                     const Domain& dm)
  : FormEntry(p, prm, true)
{
  if (sQuestion.length() != 0)
    st = new StaticTextSimple(this, sQuestion);
  else
    st = 0;
  fld = new FieldRealSimple(this, prm, dm);
  if (st)
    fld->Align(st, AL_AFTER);
  SetHelpTopic(htpUiReal);
}

FieldReal::FieldReal(FormEntry* p, const String& sQuestion,
                     float *prVal, const ValueRange& vrr)
  : FormEntry(p, 0, true)
{
  if (sQuestion.length() != 0)
    st = new StaticTextSimple(this, sQuestion);
  else
    st = 0;
  fld = new FieldRealSimple(this, prVal, vrr);
  if (st)
    fld->Align(st, AL_AFTER);
  SetHelpTopic(htpUiReal);
}

FieldReal::FieldReal(FormEntry* p, const String& sQuestion,
                     double *prVal, const ValueRange& vrr)
  : FormEntry(p, 0, true)
{
  if (sQuestion.length() != 0)
    st = new StaticTextSimple(this, sQuestion);
  else
    st = 0;
  fld = new FieldRealSimple(this, prVal, vrr);
  if (st)
    fld->Align(st, AL_AFTER);
  SetHelpTopic(htpUiReal);
}

FieldReal::FieldReal(FormEntry* p, const String& sQuestion,
                     float *prVal, const Domain& dm)

  : FormEntry(p, 0, true)
{
  if (sQuestion.length() != 0)
    st = new StaticTextSimple(this, sQuestion);
  else
    st = 0;
  fld = new FieldRealSimple(this, prVal, dm);
  if (st)
    fld->Align(st, AL_AFTER);
  SetHelpTopic(htpUiReal);
}

FieldReal::FieldReal(FormEntry* p, const String& sQuestion,
                     double *prVal, const Domain& dm)
  : FormEntry(p, 0, true)
{
  if (sQuestion.length() != 0)
    st = new StaticTextSimple(this, sQuestion);
  else
    st = 0;
  fld = new FieldRealSimple(this, prVal, dm);
  if (st)
    fld->Align(st, AL_AFTER);
  SetHelpTopic(htpUiReal);
}

FieldReal::~FieldReal()
{
 // children are removed by ~FormEntry
}

String FieldReal::sGetText()
{
        if ( !fShow() ) return "";
    String s1, s2;
    if ( st ) s1 = st->sGetText();
    if ( fld ) s2 = fld->sGetText();
    if ( st ) return String("%S:\t%S", s1, s2);
    return s2;
}

void FieldReal::create()
{
  CreateChildren();
}

void FieldReal::SetFocus() 
{ 
  fld->SetFocus(); 
}   

void FieldReal::SetWidth(short iWidth) 
{ 
  fld->SetWidth(iWidth); 
}

void FieldReal::Enable()
{
	fld->Enable();
}

void FieldReal::Disable()
{
	fld->Disable();
}

//---------------------------------------------------------------------------------------------------[ FieldRangeInt ]-----
FieldRangeInt::FieldRangeInt(FormEntry* p, const String& sQuestion,
                             Parm *prm, const ValueRange& vri)

  : FormEntry(p, prm, true)
{
  _rng = 0;
  Init(sQuestion, RangeInt(prm->sVal()), vri);
}

FieldRangeInt::FieldRangeInt(FormEntry* p, const String& sQuestion,
                             RangeInt* rng, const ValueRange& vri)
 : FormEntry(p, 0, true)
{
  _rng = rng;
  Init(sQuestion, *_rng, vri);
}
void FieldRangeInt::Init(const String& sQuestion, RangeInt rng, const ValueRange& vri)
{
  _iLo = rng.iLo();
  _iHi = rng.iHi();
  if (_iHi <= _iLo || _iHi <= iUNDEF + 2)
    _iHi = iUNDEF;
  FormEntry* parGroup = this;
  if (sQuestion.length() != 0) {
    st = new StaticTextSimple(this, sQuestion);
    parGroup = new FieldGroupSimple(this);
    parGroup->Align(st, AL_AFTER);
  }
  else
    st = 0;
  parGroup->SetIndependentPos();
  fiLo = new FieldIntSimple(parGroup, &_iLo, vri, false);
  fiHi = new FieldIntSimple(parGroup, &_iHi, vri, false);
  fiHi->Align(fiLo, AL_AFTER, 10);
  if (_npChanged)
    SetCallBack(_npChanged,_cb);
  SetHelpTopic(htpUiRangeInt);
}

FieldRangeInt::~FieldRangeInt()
{
}

void FieldRangeInt::create()
{
  CreateChildren();
}

RangeInt FieldRangeInt::rngVal()
{
  return RangeInt(fiLo->iVal(), fiHi->iVal());
}

String FieldRangeInt::sGetText()
{
        if ( !fShow() ) return "";
    String s3;
    if ( st ) s3=st->sGetText() ;
    String s1 = fiLo->sGetText();
    String s2 = fiHi->sGetText();
    return String("%S:\t%S : %S", s3 , s1, s2);
}

FormEntry* FieldRangeInt::CheckData()
{ FormEntry *fe;
  fe = fiLo->CheckData();
  if (fe) return fe;
  fe = fiHi->CheckData();
  if (fe) return fe;
  if (rngVal().iLo() > rngVal().iHi()) return this;
  return FormEntry::CheckData();
}

void FieldRangeInt::StoreData()
{
  fiLo->StoreData();
  fiHi->StoreData();
  if (_rng != 0) {
    _rng->iLo() = _iLo;
    _rng->iHi() = _iHi;
  }
  else {
    RangeInt rng = RangeInt(_iLo, _iHi);
    _prm->Replace(rng.s());
  }
  FormEntry::StoreData();
}

void FieldRangeInt::SetFocus()
{
  fiLo->SetFocus();
  _frm->ilwapp->SetHelpTopic(htp());
}

void FieldRangeInt::SetWidth(short iWidth) 
{ 
  fiLo->SetWidth(iWidth); 
  fiHi->SetWidth(iWidth); 
}

void FieldRangeInt::Enable()
{
	fiLo->Enable();
	fiHi->Enable();
}

void FieldRangeInt::Disable()
{
	fiLo->Disable();
	fiHi->Disable();
}

//------------------------------------------------------------------------------------------[ FieldRangeReal ]-----------
FieldRangeReal::FieldRangeReal(FormEntry* p, const String& sQuestion,
                               Parm *prm, const ValueRange& vrr)
  : FormEntry(p, prm, true)
{
  _rng = 0;
  Init(sQuestion, RangeReal(prm->sVal()), vrr);
}

FieldRangeReal::FieldRangeReal(FormEntry* p, const String& sQuestion,
                               RangeReal* rng, const ValueRange& vrr)
  : FormEntry(p, 0, true)
{
  _rng = rng;
  Init(sQuestion, *rng, vrr);
}

void FieldRangeReal::Init(const String& sQuestion, RangeReal rng,
                          const ValueRange& vrr)
{
  _rLo = rng.rLo();
  _rHi = rng.rHi();

  FormEntry* parGroup = this;
  if (sQuestion.length() != 0) {
    st = new StaticTextSimple(this, sQuestion);
    parGroup = new FieldGroupSimple(this);
    parGroup->Align(st, AL_AFTER);
  }
  else
    st = 0;
  parGroup->SetIndependentPos();
  frLo = new FieldRealSimple(parGroup, &_rLo, vrr);
  frHi = new FieldRealSimple(parGroup, &_rHi, vrr);
  frHi->Align(frLo, AL_AFTER, 10);
  if (_npChanged)
    SetCallBack(_npChanged,_cb);
  SetHelpTopic(htpUiRangeReal);
}


FieldRangeReal::~FieldRangeReal()
{
 // children are removed by ~FormEntry
}

void FieldRangeReal::create()
{
  CreateChildren();
}

RangeReal FieldRangeReal::rngVal()
{
  return RangeReal(frLo->rVal(), frHi->rVal());
}

String FieldRangeReal::sGetText()
{
        if ( !fShow() ) return "";
    String s3;
    if ( st ) s3=st->sGetText() ;
    String s1 = frLo->sGetText();
    String s2 = frHi->sGetText();
//    s1=s1.sSub(0, s1.length());
//    s2=s2.sSub(0, s2.length());
    return String("%S:\t%S : %S", s3, s1, s2);
}

FormEntry* FieldRangeReal::CheckData()
{ FormEntry *fe;
  fe = frLo->CheckData();
  if (fe) return fe;
  fe = frHi->CheckData();
  if (fe) return fe;
  if (rngVal().rLo() > rngVal().rHi()) return this;
  return FormEntry::CheckData();
}

void FieldRangeReal::StoreData()
{
  frLo->StoreData();
  frHi->StoreData();
  if (_rng != 0) {
    _rng->rLo() = _rLo;
    _rng->rHi() = _rHi;
  }
  else {
    RangeReal rng = RangeReal(_rLo, _rHi);
    _prm->Replace(rng.s());
  }
  FormEntry::StoreData();
}


void FieldRangeReal::SetFocus()
{
  frLo->SetFocus();
  _frm->ilwapp->SetHelpTopic(htp());
}

void FieldRangeReal::SetWidth(short iWidth)
{ 
  frLo->SetWidth(iWidth); 
  frHi->SetWidth(iWidth); 
}

void FieldRangeReal::SetStepSize(double rStep)
{ 
  double rLo = frLo->rVal();
  double rHi = frHi->rVal();
  frLo->SetStepSize(rStep); 
  frHi->SetStepSize(rStep); 
  double rTmp = frLo->rVal();
  if (rTmp - rLo > 1.0e-6)    // allow a margin
    frLo->SetVal(rTmp - rStep);
  rTmp = frHi->rVal();
  if (rHi - rTmp > 1.0e-6)    // allow a margin
    frHi->SetVal(rTmp + rStep);
}

void FieldRangeReal::Enable()
{
	frLo->Enable();
	frHi->Enable();
}

void FieldRangeReal::Disable()
{
	frLo->Disable();
	frHi->Disable();
}

//-----------------------------------------------------------------------------------------------------[ FieldRowCol ]---------

FieldRowCol::FieldRowCol(FormEntry* p, const String& sQuestion,
                               Parm *prm)
  : FormEntry(p, prm, true)
{
  rc = RowCol(0L,0L); // RowCol(prm->sVal())
  _rc = 0;
	fSpinning = false;
  Init(sQuestion);
}

FieldRowCol::FieldRowCol(FormEntry* p, const String& sQuestion,
                               RowCol* rowcol, bool fSpin)
  : FormEntry(p, 0, true)
{
  _rc = rowcol;
  rc = *rowcol;
	fSpinning = fSpin;
  Init(sQuestion);
}

String FieldRowCol::sGetText()
{
        if ( !fShow() ) return "";
    String s3;
    if ( st ) s3=st->sGetText() ;
    String s1 = fiRow->sGetText();
    String s2 = fiCol->sGetText();
  //  s1=s1.sSub(0, s1.length()-1);
  //  s2=s2.sSub(0, s2.length()-1);
    return String("%S:\t(%S,%S)", s3, s1, s2);
}

void FieldRowCol::Init(const String& sQuestion)
{
  FormEntry* parGroup = this;
  if (sQuestion.length() != 0) {
    st = new StaticTextSimple(this, sQuestion);
    parGroup = new FieldGroupSimple(this);
    parGroup->Align(st, AL_AFTER);
  }
  else
    st = 0;
  parGroup->SetIndependentPos();
  fiRow = new FieldIntSimple(parGroup, &rc.Row, ValueRange(0,99999), fSpinning);
  fiCol = new FieldIntSimple(parGroup, &rc.Col, ValueRange(0,99999), fSpinning);
  fiCol->Align(fiRow, AL_AFTER, 10);
  if (_npChanged)
    SetCallBack(_npChanged,_cb);
  SetHelpTopic(htpUiRowCol);
}


FieldRowCol::~FieldRowCol()
{
 // children are removed by ~FormEntry
}

void FieldRowCol::create()
{
  CreateChildren();
}

RowCol FieldRowCol::rcVal()
{
  return RowCol(fiRow->iVal(), fiCol->iVal());
}

FormEntry* FieldRowCol::CheckData()
{ FormEntry *fe;
  fe = fiRow->CheckData();
  if (fe) return fe;
  fe = fiCol->CheckData();
  if (fe) return fe;
  return FormEntry::CheckData();
}

void FieldRowCol::StoreData()
{
  fiRow->StoreData();
  fiCol->StoreData();
  if (_rc != 0) 
    *_rc = rc;
  else {
//    RowCol rng = RowCol(_rLo, _rHi);
//    _prm->Replace(rng.s());
  }
  FormEntry::StoreData();
}


void FieldRowCol::SetFocus()
{
  fiRow->SetFocus();
  _frm->ilwapp->SetHelpTopic(htp());
}

void FieldRowCol::SetWidth(short iWidth)
{ 
  fiRow->SetWidth(iWidth); 
  fiCol->SetWidth(iWidth); 
}

bool FieldRowCol::fIncludesHandle(HANDLE hnd)
{
  return fiRow->hWnd() == hnd ||
         fiCol->hWnd() == hnd;
}

//--------------------------------------------------------------------------------------------------[ FieldCoord ]-----------
FieldCoord::FieldCoord(FormEntry* p, const String& sQuestion,
                               Parm *prm)
  : FormEntry(p, prm, true)
{
  crd = Coord(0,0); // Coord(prm->sVal())
  _crd = 0;
  Init(sQuestion);
}

FieldCoord::FieldCoord(FormEntry* p, const String& sQuestion,
                               Coord* coord)
  : FormEntry(p, 0, true)
{
  _crd = coord;
  crd = *coord;
  Init(sQuestion);
}

void FieldCoord::Init(const String& sQuestion)
{
  FormEntry* parGroup = this;
  if (sQuestion.length() != 0) {
    st = new StaticTextSimple(this, sQuestion);
    parGroup = new FieldGroupSimple(this);
    parGroup->Align(st, AL_AFTER);
  }
  else
    st = 0;
  parGroup->SetIndependentPos();
//  Domain dm(-1e20,1e20,0.001);
  ValueRange vrr(-1e20,1e20,0.001);
  frX = new FieldRealSimple(parGroup, &crd.x, vrr);
  frX->SetFieldWidth(FLDNAMEWIDTH);
  frY = new FieldRealSimple(parGroup, &crd.y, vrr);
  frY->SetFieldWidth(FLDNAMEWIDTH);
  frY->Align(frX, AL_AFTER, 10);
  if (_npChanged)
    SetCallBack(_npChanged,_cb);
  SetHelpTopic(htpUiCoord);
}


FieldCoord::~FieldCoord()
{
 // children are removed by ~FormEntry
}

String FieldCoord::sGetText()
{
        if ( !fShow() ) return "";
    String s3;
    if ( st ) s3=st->sGetText() ;
    String s1 = frX->sGetText();
    String s2 = frY->sGetText();
//    s1=s1.sSub(0, s1.length()-1);
//    s2=s2.sSub(0, s2.length()-1);
    return String("%S:\t(%S,%S)", s3, s1, s2);
}

void FieldCoord::create()
{
  CreateChildren();
}

Coord FieldCoord::crdVal()
{
  return Coord(frX->rVal(), frY->rVal());
}

FormEntry* FieldCoord::CheckData()
{ FormEntry *fe;
  fe = frX->CheckData();
  if (fe) return fe;
  fe = frY->CheckData();
  if (fe) return fe;
  return FormEntry::CheckData();
}

void FieldCoord::StoreData()
{
  frX->StoreData();
  frY->StoreData();
  if (_crd != 0) 
    *_crd = crd;
  FormEntry::StoreData();
}


void FieldCoord::SetFocus()
{
  frX->SetFocus();
  _frm->ilwapp->SetHelpTopic(htp());
}

void FieldCoord::SetWidth(short iWidth)
{ 
  frX->SetWidth(iWidth); 
  frY->SetWidth(iWidth); 
} 

bool FieldCoord::fIncludesHandle(HANDLE hnd)
{
  return frX->hWnd() == hnd ||
         frY->hWnd() == hnd;
}

//--
FieldDate::FieldDate(FormEntry* p, const String& sQuestion,
                               Parm *prm)
  : FormEntry(p),year(0),month(0),day(0)
{
   Init(sQuestion);
}

FieldDate::FieldDate(FormEntry* p, const String& sQuestion,
					 ILWIS::Time* tm )
  : FormEntry(p),year(0),month(0),day(0)
{
  time = tm;
  day = time->get(ILWIS::Time::tpDAYOFMONTH);
  year = time->get(ILWIS::Time::tpYEAR);
  month = time->get(ILWIS::Time::tpMONTH);
  Init(sQuestion);
}

void FieldDate::Init(const String& sQuestion)
{
  FormEntry* parGroup = this;
  if (sQuestion.length() != 0) {
    st = new StaticTextSimple(this, sQuestion);
    parGroup = new FieldGroupSimple(this);
    parGroup->Align(st, AL_AFTER);
  }
  else
    st = 0;
  parGroup->SetIndependentPos();
//  Domain dm(-1e20,1e20,0.001);
  ValueRange vrrMonth(RangeInt(1,12));
  ValueRange vrrDay(RangeInt(1,31));
  ValueRange vrrYear(RangeInt(1900,2100));
  frDay = new FieldIntSimple(parGroup, &day, vrrDay, false);
  frDay->SetFieldWidth(20);
  frMonth = new FieldIntSimple(parGroup, &month, vrrMonth, false);
  frMonth->SetFieldWidth(20);
  frMonth->Align(frDay, AL_AFTER,2);
  frYear = new FieldIntSimple(parGroup, &year, vrrYear, false);
  frYear->SetFieldWidth(30);
  frYear->Align(frMonth, AL_AFTER,2);
  pbCalendar = new OwnButtonSimple(parGroup,"time",(NotifyProc)&FieldDate::showCalendar, this);
  pbCalendar->SetHeight(frYear->psn->iHeight);
  pbCalendar->Align(frYear, AL_AFTER);

  if (_npChanged)
    SetCallBack(_npChanged,_cb);
  //SetHelpTopic(htpUiCoord);
}

int FieldDate::showCalendar(Event *ev) {
	frDay->StoreData();
	frMonth->StoreData();
	frYear->StoreData();
	int x = pbCalendar->psn->iPosX;
	int y = pbCalendar->psn->iPosY;
	CalendarForm *frm = new CalendarForm(this->frm()->wnd(),ILWIS::Time(year,month,day));
	if ( frm->fOkClicked()) {
		COleDateTime dt = frm->GetDate();
		frDay->SetVal(dt.GetDay());
		frMonth->SetVal(dt.GetMonth());
		frYear->SetVal(dt.GetYear());
	}

	return 1;
}

void FieldDate::SetVal(const ILWIS::Time& tim) {

	long year, month, day;
	year = time->get(ILWIS::Time::tpYEAR);
	month = time->get(ILWIS::Time::tpMONTH); //0 based
	day = time->get(ILWIS::Time::tpDAYOFMONTH); // 0 based
	frYear->SetVal(year);
	frMonth->SetVal(month);
	frDay->SetVal(day);
}


FieldDate::~FieldDate()
{
 // children are removed by ~FormEntry
}

String FieldDate::sGetText()
{
    if ( !fShow() ) return "";
    String s1 = frDay->sGetText();
    String s2 = frMonth->sGetText();
    String s3 = frYear->sGetText();
	if ( s1.size()==0 && s2.size()==0 && s3.size()==0)
		return "";
	if ( s2.size() == 1)
		s2 = "0" + s2;
	if ( s1.size() == 1)
		s1 = "0" + s1;
	if ( s3.size() < 4) {
		int yr = s3.iVal() + 2000;
		s3 = String("%d", yr);
	}

	return String("%S-%S-%S", s3, s2, s1);
}

void FieldDate::create()
{
  CreateChildren();
}

FormEntry* FieldDate::CheckData()
{ FormEntry *fe;
  fe = frDay->CheckData();
  if (fe) return fe;
  fe = frMonth->CheckData();
  if (fe) return fe;
   fe = frYear->CheckData();
  if (fe) return fe;
  long iDay = frDay->iVal();
  long iMonth = frMonth->iVal();
  long iYr = frYear->iVal();
  if ( iMonth == iUNDEF && iYr == iUNDEF && iDay == iUNDEF)	 // we accept no date
	return FormEntry::CheckData();

  if ( iMonth % 2 == 0) {
	if ( iDay > 30)
		return frDay;
    bool fLeap = ((iYr % 4 == 0) && (iYr % 100 != 0)) || (iYr || 400 == 0);
	if ( iMonth == 2 && !fLeap && iDay > 28)
		return frDay;
  }
  return FormEntry::CheckData();
}

void FieldDate::StoreData()
{
  frDay->StoreData();
  frMonth->StoreData();
  frYear->StoreData();
  FormEntry::StoreData();
}


void FieldDate::SetFocus()
{
  frDay->SetFocus();
  //_frm->ilwapp->SetHelpTopic(htp());
}

void FieldDate::SetWidth(short iWidth)
{ 
  frDay->SetWidth(iWidth); 
  frMonth->SetWidth(iWidth); 
  frYear->SetWidth(iWidth); 
}

void FieldDate::show(int mode)            // set show or hide
{
	frDay->show(mode);
	frYear->show(mode);
	frMonth->show(mode);
	pbCalendar->show(mode);
}

void FieldDate::Enable() {
    if(frDay == 0)
		return;
	frDay->Enable();
	frYear->Enable();
	frMonth->Enable();
}

void FieldDate::Disable() {
	if   (frDay==0)
		return;
	frDay->Disable();
	frYear->Disable();
	frMonth->Disable();
}

CalendarForm::CalendarForm(CWnd *wnd, const ILWIS::Time& date) : FormWithDest(wnd,"Select Date") {
	calendar = new CalendarSelect(root);
	calendar->SetDate(date);

	create();
}

COleDateTime CalendarForm::GetDate() {
	if ( calendar)
		return calendar->GetDate();
	return COleDateTime();
}

void CalendarForm::SetDate(const ILWIS::Time& date) {
	if ( calendar)
			calendar->SetDate(date);
}

RadBut::RadBut(FormEntry* f, CWnd *w, const CRect& siz, DWORD style, const char* sQuest, int id) :
      ZappButton(f, w, siz, BS_RADIOBUTTON | style , sQuest, id),
		  ilwapp(IlwWinApp()->Context())
  {}


 ChckBox::ChckBox(FormEntry* f, CWnd *w, const CRect& siz, DWORD style,  char* sQuest, int id)
    : ZappButton(f, w, siz, style, sQuest, id)
{ ilwapp = IlwWinApp()->Context(); }


FieldTime::FieldTime(FormEntry *f, const String& sQuestion,ILWIS::Time *ti,  const DomainTime* dt, ILWIS::Time::Mode m) :
	FormEntry(f,0,true),
	time(ti),
	mode(m)
{
	if ( dt == 0 && mode == ILWIS::Time::mUNKNOWN)
		mode = ILWIS::Time::mDATETIME;
	if ( dt != 0 && mode == ILWIS::Time::mUNKNOWN)
		mode = dt->getMode();

	//int fieldsize = mode == ILWIS::Time::mDATETIME ? 60 : 30;
	int fieldsize = 55;

	if (sQuestion.length() != 0)
		st = new StaticTextSimple(this, sQuestion);
	else
		st = 0;
	sTime = ti->toString(true, mode);
	fsTime = new FieldStringSimple(this,&sTime,ES_AUTOHSCROLL|WS_TABSTOP|WS_GROUP|WS_BORDER);
	fsTime->SetWidth(fieldsize);
	fsTime->SetCallBack((NotifyProc)&FieldTime::checkFormat,this);
	fsTime->Hide();
	if (st)
		fsTime->Align(st, AL_AFTER);
}

int FieldTime::checkFormat(Event *ev) {
	fsTime->StoreData();
	sTime = fsTime->sVal();
	for(int i=0; i < sTime.size(); ++i) {
		bool valid = false;
		char c = sTime[i];
		if ( sTime[i] >= '0' && sTime[i] <= '9')
			valid=true;
		if ( c == '-' || c == '+' || c == '*')
			valid = true;
		if ( mode != ILWIS::Time::mDURATION && (c == '.' || c == 'T' || c == ':' )) 
			valid = true;
		if ( mode == ILWIS::Time::mDURATION && ( c == 'P' || c == 'Y' || c =='D' || c == 'M' ||
			 c == 'H' || c == 'S' || c == 'T' || c == '.'))
			 valid = true;
		if ( valid == false) {
			sTime = sTime.substr(0,i);
			fsTime->SetVal(sTime);
			break;
		}
	}

	return 1;
}

void FieldTime::create()
{
	CreateChildren();
}

void FieldTime::show(int state)            // set show or hide
{
	fsTime->show(state);
}

void FieldTime::SetUseDate(bool yesno) {
}

void FieldTime::StoreData()
{
	fsTime->StoreData();
	if ( mode != ILWIS::Time::mDURATION) {
		ILWIS::Time t(sTime);
		*time = (double)t;
	} else {
		ILWIS::Duration d(sTime);
		*time = (double)d;
	}
}

void FieldTime::SetVal(const ILWIS::Time ti, ILWIS::Time::Mode m) {
	mode = m;
	fsTime->SetVal(ti.toString(true, m));
}