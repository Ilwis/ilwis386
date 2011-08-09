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
/* Miscellaneous  Form entries, 2nd part

// by Wim Koolhoven, aug. 1993
// changed by Jelle Wind, dec. 1993 - june 1994
	Last change:  WK    1 Sep 97    9:13 pm
*/
#include "Client\Headers\formelementspch.h"
#include "Client\ilwis.h"
#include "Client\Base\Res.h"
#include "Client\Editors\Utils\FlatButton.h"

//----------[ Radio Button ]-------------------------------------------------------------------------------------
RadioButton::RadioButton(RadioGroup* p, const String& sQuestion,
                         const String& sParam)
  : FormEntry(p, 0, true)
{
  sQuest = sQuestion;
  p->AddStrParm(sParam);
  zDimension dim = Dim(sQuest);
  psn->iMinWidth = dim.width() + 20; //dim.height();

  // align under last child of radio group
  short iChild = _par->childlist().iSize();
  if (iChild > 1)
    Align(_par->childlist()[iChild-2], (p->fAlignHor) ? AL_AFTER : AL_UNDER);

  setHelpItem(htpUiRadioButton);
}

RadioButton::RadioButton(RadioGroup* p, const String& sQuestion)
  : FormEntry(p, 0, true), rb(0)
{
  sQuest = sQuestion;

  zDimension dim = Dim(sQuest);
  psn->iMinWidth = dim.width() + 20; // dim.height();
  // align under last child of radio group
  short iChild = _par->childlist().iSize();
  if (iChild > 1)
    Align(_par->childlist()[iChild-2], (p->fAlignHor) ? AL_AFTER : AL_UNDER);

  setHelpItem(htpUiRadioButton);
}


RadioButton::~RadioButton()
{
	if (rb) {
		if (_npChanged != 0)
			rb->removeNotify(_cb, Notify(BN_CLICKED));
		rb->removeNotify(_par, Notify(BN_CLICKED));
		delete rb;
	}
}

String RadioButton::sGetText()
{
    if ( !fShow() ) return "";
    return sQuest;
}

void RadioButton::create()
{
  zPoint pntFld = zPoint(psn->iPosX, psn->iPosY);
  zDimension dimFld = zDimension(psn->iMinWidth , psn->iMinHeight);
  rb = new RadBut(this, _frm->wnd(), CRect(pntFld, dimFld), 0, sQuest.c_str(), Id());
  rb->SetFont(_frm->fnt);                                         
  if (_npChanged != 0)
    rb->setNotify(_cb, _npChanged, Notify(BN_CLICKED));
  rb->setNotify(_par, (NotifyProc)&RadioGroup::ButtonClicked, Notify(BN_CLICKED));
}

bool RadioButton::fVal()
{
  if (rb && rb->GetSafeHwnd() != NULL) 
    return rb->check();
  return false;
}

void RadioButton::SetVal(bool f)
{
  if (rb) 
	  rb->check(f);
  if (f) 
    ShowChildren();
  else
    HideChildren();
	DoCallBack();
}

void RadioButton::show(int sw)
{
	if (rb && rb->GetSafeHwnd() !=NULL) 
		if (sw == SW_HIDE) {
			rb->ShowWindow(SW_HIDE);
			rb->SetWindowText("");
		}
		else {
			rb->SetWindowText(sQuest.c_str());
			rb->ShowWindow(SW_SHOW);
		}
  if (fVal() && (sw == SW_SHOW))
    ShowChildren();
  else
    HideChildren();
}

void RadioButton::SetFocus()
{
  rb->SetFocus();
  _frm->ilwapp->setHelpItem(htp());
}

void RadioButton::StoreData()
{
  if (fShow() && fVal())
      FormEntry::StoreData();
}

void RadioButton::Enable()
{
	if ( rb )
		rb->EnableWindow();
}

void RadioButton::Disable()
{
	if ( rb )
		rb->EnableWindow(FALSE);
}

//-------[ Radio Group ]----------------------------------------------------------------------------------------
RadioGroup::RadioGroup(FormEntry* p, const String& sQuestion, Parm *prm, bool fInRows)
	: FormEntry(p, prm, true)
{
	_iVal = 0;
	_piVal = 0;
	fAlignHor = fInRows;
	st = 0;
	if (sQuestion.length() != 0)
		st = new StaticText(this, sQuestion);
}

RadioGroup::RadioGroup(FormEntry* p, const String& sQuestion, int *piVal, bool fInRows)
	: FormEntry(p, 0, true)
{
	_piVal = piVal;
	_iVal = *piVal;
	st = 0;
	fAlignHor = fInRows;
	if (sQuestion.length() != 0)
		st = new StaticText(this, sQuestion);
}

RadioGroup::~RadioGroup()
{
	for (unsigned int i=0; i < sParmList.iSize(); i++)
		delete sParmList[i];
}

String RadioGroup::sGetText()
{
	if ( !fShow() ) 
		return String();
	
	String s;
	FormEntry* fe = childlist()[iVal() + (st != 0)];
	if (st)
		s = String("%S:\t%S", st->sGetText(), fe->sGetText());
	else
		s = fe->sGetText();

	String sChildText = fe->sChildrensText();
	if (sChildText.length() > 0)
	{
		s &= "\r\n";
		s &= sChildText;
	}
	return s;
}

void RadioGroup::create()
{
	RadioButton *rb;
	// Do not set callback's for individual radiobuttons here
	// This will result in the callbacks being called before the OnButtonClicked()
	// The callback will then be executed too soon.
	// The callback is now called from the OnButtonClicked()
	
	// create RadioButtons
	for (unsigned int i=0; i < childlist().iSize(); i++)
		childlist()[i]->create();
	// create childlist() of RadioButtons
	for (unsigned int i=0; i < childlist().iSize(); i++)
	{
		childlist()[i]->CreateChildren();
		if (childlist()[i]->fRadioButton())
			((RadioButton*)childlist()[i])->SetVal(false);
	}

	// set tab stop for buttons in radio group
	short iFirstBut = 0;
	if (st != 0) ++iFirstBut;
	rb = 0;
	if (childlist()[iFirstBut]->fRadioButton())
	{
		rb = (RadioButton *)childlist()[iFirstBut];
		rb->styleSet(WS_GROUP);
	}

	// activate proper radio button (iPos)
	if (_piVal && (*_piVal == -1))  // do not select any radio button yet
		return;
	
	short iPos = iFirstBut;
	if (_piVal != 0)
		iPos = *_piVal + iFirstBut;
	else if (_prm != 0)  // look for default selection as passed on command line
	{
		unsigned int i;
		for (i = 0; i < sParmList.iSize(); i++)
			if (*sParmList[i] == _prm->sVal())
				break;
			else
				iPos++;

		if (i >= sParmList.iSize())  // unknown option: select the first button
			iPos = iFirstBut;
	}

	if ((iPos < iFirstBut) || iPos > childlist().iSize()) // out of range
		iPos = iFirstBut;
	if (childlist()[iPos]->fRadioButton())
		((RadioButton*)childlist()[iPos])->SetVal(true);
}

int RadioGroup::iVal()
{
	unsigned int i = 0;
	if (st != 0) i++;
	for (; i < childlist().iSize(); i++)
		if (childlist()[i]->fRadioButton() && ((RadioButton*)childlist()[i])->fVal())
			return (st != 0) ? i - 1 : i;

	return _iVal; // 0;
}

void RadioGroup::ClearData() {
	SetVal(0);
}

void RadioGroup::SetVal(int iVal)
{
	unsigned int i = 0;
	if (st != 0)
	{
		i++; iVal++;
	}
	for (; i < childlist().iSize(); i++)
		if (childlist()[i]->fRadioButton())
			((RadioButton*)childlist()[i])->SetVal(i == iVal);
}

String RadioGroup::sVal()
{
	return *(sParmList[iVal()]);
}

void RadioGroup::StoreData()
{
	if (fShow())
	{
		if (_piVal)
			*_piVal = iVal();
		else
			if (_prm != 0) _prm->Replace(sVal());
	}
	FormEntry::StoreData();
}

void RadioGroup::SetFocus()
{
	childlist()[iVal()+(st != 0)]->SetFocus();
}

int RadioGroup::ButtonClicked(void *parm)
{
	if (parm == 0)
		throw ErrorObject("XXUnitialized use of radio button");

	Event *ev = reinterpret_cast<Event *>(parm);
	RadioButton *rb;
	unsigned int i = 0;
	if (st != 0) i++;
	for (; i < childlist().iSize(); i++) 
	{
		if (childlist()[i]->fRadioButton())
		{
			rb = (RadioButton*)childlist()[i];
			if (rb->Id() == ev->iLowW())
				rb->SetVal(true);
			else
				rb->SetVal(false);
		}
	}
	DoCallBack();
	return 0;
}

void RadioGroup::AddStrParm(const String& sParm)
{
	sParmList &= new String(sParm);
}

void RadioGroup::Enable()
{
	EnableChildren();
}
 
void RadioGroup::Disable()
{
	DisableChildren();
}
 

//-------[ Check Box ]------------------------------------------------------------------------------------------

CheckBox::CheckBox(FormEntry* p, const String& sQuestion, Parm *prm)
  : FormEntry(p, prm, true), cbx(0), _piVal(0), fClicked(false), m_fDisabled(false)
{
  _fVal = _prm->fVal();
  _pfVal = 0;
  sQuest = sQuestion;
  zDimension dim = Dim(sQuest);
  psn->iMinWidth = dim.width() + 30; // dim.height();
  setHelpItem(htpUiCheckBox);
}

CheckBox::CheckBox(FormEntry* p, const String& sQuestion, bool *pfVal)
  : FormEntry(p, 0, true), cbx(0), _piVal(0), fClicked(false), m_fDisabled(false)
{
  _pfVal = pfVal;
  _fVal = *pfVal;
  sQuest = sQuestion;
  zDimension dim = Dim(sQuest);
  psn->iMinWidth = dim.width() + 30; //dim.height();
  setHelpItem(htpUiCheckBox);
}

CheckBox::CheckBox(FormEntry* p, const String& sQuestion, long *piVal)
  : FormEntry(p, 0, true), cbx(0), _pfVal(0), fClicked(false), m_fDisabled(false)
{
  _piVal = piVal;
  _fVal = *piVal > 0 ? true : false;
  sQuest = sQuestion;
  zDimension dim = Dim(sQuest);
  psn->iMinWidth = dim.width() + 30; //dim.height();
  setHelpItem(htpUiCheckBox);
}

void CheckBox::SetStyle(bool fDisabled)
{
	m_fDisabled = fDisabled;
}

void CheckBox::create()
{
  zPoint pntFld = zPoint(psn->iPosX, psn->iPosY);
  zDimension dimFld = zDimension(psn->iMinWidth, psn->iMinHeight);
  cbx = new ChckBox(this, _frm->wnd(), CRect(pntFld, dimFld), WS_TABSTOP|WS_GROUP|BS_AUTOCHECKBOX , sQuest.sVal(), Id());
  cbx->SetFont(_frm->fnt);                                         
  cbx->setNotify(this, (NotifyProc)&CheckBox::ButtonClicked, Notify(BN_CLICKED));
  if (_npChanged)
    cbx->setNotify(_cb, _npChanged, Notify(BN_CLICKED));
  CreateChildren();
  cbx->check(_fVal);
  if (m_fDisabled)
	  disable();
}

void CheckBox::ClearData() {
	SetVal(false);
}

void CheckBox::SetVal(bool fVal)
{
  _fVal = fVal;
  if (cbx)
    cbx->check(fVal);
}
bool CheckBox::fVal()
{
  if (cbx)
    return cbx->check()==BST_CHECKED;
  return _fVal;
}

int CheckBox::ButtonClicked(void* Ev)
{
  if (Ev==0)
    throw ErrorObject("XXUnitialized use of radio button");
  Event *ev=reinterpret_cast<Event *>(Ev);
  if ( ev->message() == Notify(BN_CLICKED))// Notify(BN_CLICKED))
  {
		fClicked = true;
    _fVal = !_fVal;
    if (_fVal)
      ShowChildren();
    else
      HideChildren();
    return 1;
  }
  return 0;
}

void CheckBox::show(int sw)
{
	if (cbx && cbx->GetSafeHwnd() != NULL)  
		if (sw == SW_HIDE) {
			cbx->ShowWindow(SW_HIDE);
			cbx->SetWindowText("");
		}
		else {
			cbx->SetWindowText(sQuest.c_str());
			cbx->ShowWindow(SW_SHOW);
		}
  if (sw == SW_HIDE || !fVal())
    HideChildren();
  else
    ShowChildren();
}


void CheckBox::StoreData()
{
  if (fShow()) 
	{
    if (_pfVal != 0)
      if (cbx)
        *_pfVal = cbx->check()==BST_CHECKED;
      else  
        *_pfVal = _fVal;
		else if ( _piVal != 0 )
		{
			if (cbx)
        *_piVal = cbx->check()==BST_CHECKED;
      else  
        *_piVal = _fVal == true ? 1 : 0;
		}			
		else
      _prm->Replace(_fVal);
  }
  FormEntry::StoreData();
}

String CheckBox::sGetText()
{
  if ( !fShow() ) return "";
  String s("%S:\t%s", sQuest, cbx->check() ? "yes" : "no");
  if (cbx->check()) {
    String sChildText = sChildrensText();
    if (sChildText.length() > 0) {
      s &= "\r\n";
      s &= sChildText;
    }
  }
  return s;
}

void CheckBox::SetFocus()
{
  if (cbx)
    cbx->SetFocus();
  _frm->ilwapp->setHelpItem(htp());
}

CheckBox::~CheckBox()
{
	if (cbx) {
		if (_npChanged && cbx)
			cbx->removeNotify(_cb, Notify(BN_CLICKED));
		delete cbx;
	}
}

//--[ Check 3 Box ]-----------------------------------------------------------------------------------------------
Check3Box::Check3Box(FormEntry* p, const String& sQuestion, long *piVal)
  : FormEntry(p, 0, true), cbx(0)
{
  _piVal = piVal;
  _iVal = *piVal;
  sQuest = sQuestion;
  zDimension dim = Dim(sQuest);
  psn->iMinWidth = dim.width() + 20; //dim.height();
}

void Check3Box::create()
{
  zPoint pntFld = zPoint(psn->iPosX, psn->iPosY);
  zDimension dimFld = zDimension(psn->iMinWidth, psn->iMinHeight);
  cbx = new ChckBox(this, _frm->wnd(), CRect(pntFld, dimFld), WS_TABSTOP|WS_GROUP|BS_AUTO3STATE, sQuest.sVal(), Id());
  cbx->SetFont(_frm->fnt);                                         
  cbx->setNotify(this, (NotifyProc)&Check3Box::ButtonClicked, Notify(BN_CLICKED));
  if (_npChanged)
    cbx->setNotify(_cb, _npChanged, Notify(BN_CLICKED));
  CreateChildren();
  SetVal(_iVal);
}

String Check3Box::sGetText()
{
    if ( !fShow() ) return "";
    String s1;
    switch (cbx->check()) {
      case 0: s1 = "no"; break;
      case 1: s1 = "yes"; break;
      case 2: s1 = "?"; break;
    }
    return String("%S:\t%S", sQuest, s1);
}

void Check3Box::ClearData() {
	SetVal(2);
}

void Check3Box::SetVal(long iVal)
{
  if (0 != iVal && 1 != iVal)
    iVal = 2;
  cbx->check(iVal==BST_CHECKED);
}

long Check3Box::iVal()
{
  if (cbx) {
    _iVal = cbx->check();
    if (2 == _iVal)
      _iVal = iUNDEF;
  }
  return _iVal;
}

int Check3Box::ButtonClicked(void* Ev)
{
  if (Ev==0)
    throw ErrorObject("XXUnitialized use of radio button");
  Event *ev=reinterpret_cast<Event *>(Ev);
  if ( ev->message() == Notify(BN_CLICKED))
  {
    if (1 == iVal())
      ShowChildren();
    else
      HideChildren();
    return 1;
  }
  return 0;
}

void Check3Box::show(int sw)
{
  if (sw == SW_HIDE || 1 != iVal())
    HideChildren();
  else
    ShowChildren();
  if (cbx)  
		if (sw == SW_HIDE) {
			cbx->ShowWindow(SW_HIDE);
			cbx->SetWindowText("");
		}
		else {
			cbx->SetWindowText(sQuest.c_str());
			cbx->ShowWindow(SW_SHOW);
		}
}


void Check3Box::StoreData()
{
  if (fShow()) {
    if (cbx) {
      _iVal = cbx->check();
      if (2 == _iVal)
        _iVal = iUNDEF;
    }
    if (_piVal != 0)
      *_piVal = _iVal;
  }
  FormEntry::StoreData();
}

void Check3Box::SetFocus()
{
  if (cbx)
    cbx->SetFocus();
  _frm->ilwapp->setHelpItem(htp());
}

Check3Box::~Check3Box()
{
	if (cbx) {
		if (_npChanged)
			cbx->removeNotify(_cb, Notify(BN_CLICKED));
		delete cbx;
	}
}


//-- [ PushButtonSimple ] ----------------------------------------------------------------------------------------

PushButtonSimple::PushButtonSimple(FormEntry* p, const String& sQuestion,
		       NotifyProc np, bool fParent, bool fAutoAlign)
 : FormEntry(p, 0, fAutoAlign), fPar(fParent),
   fDefault(false), alternativeCBHandler(NULL)
{
  _np = np;
  sQuest = sQuestion;
  zDimension dim = Dim(sQuest);
  psn->iMinWidth = dim.width() + 25;
  psn->iHeight = psn->iMinHeight = dim.height() + 9;
  setHelpItem(htpUiPushButton);
  pb = 0;
}

PushButtonSimple::PushButtonSimple(FormEntry* parent, const String& sQuestion, NotifyProc np, FormEntry* _alternativeCBHandler, bool fAutoAlign) :
   FormEntry(parent, 0, fAutoAlign), fPar(false),
   fDefault(false), alternativeCBHandler(_alternativeCBHandler)
{
  _np = np;
  sQuest = sQuestion;
  zDimension dim = Dim(sQuest);
  psn->iMinWidth = dim.width() + 25;
  psn->iHeight = psn->iMinHeight = dim.height() + 9;
  setHelpItem(htpUiPushButton);
  pb = 0;
}

PushButtonSimple::~PushButtonSimple()
{
	if (pb)
		delete pb;
}

void PushButtonSimple::create()
{
	zPoint pntFld = zPoint(psn->iPosX, psn->iPosY);
	zDimension dimFld = zDimension(psn->iMinWidth, psn->iMinHeight);
	pb = new ZappButton(this, _frm->wnd(), CRect(pntFld, dimFld), WS_TABSTOP|WS_GROUP|BS_PUSHBUTTON, sQuest.sVal(), Id());
	pb->SetFont(_frm->fnt);
	if ( fDefault)
	{
		UINT iStyle = pb->GetButtonStyle();		
		pb->SetButtonStyle(iStyle & BS_DEFPUSHBUTTON);
	}
	if( alternativeCBHandler != NULL) {
		pb->setNotify(alternativeCBHandler, _np, Notify(BN_CLICKED));
	}
	else if (fPar)
		pb->setNotify(_par, _np, Notify(BN_CLICKED));
	else
		pb->setNotify(_frm->wnd(), _np, Notify(BN_CLICKED));
	CreateChildren();
	pb->EnableWindow(_enabled ? TRUE : FALSE);
}

void PushButtonSimple::SetDefault(bool fYesNo)
{
	fDefault = fYesNo;
}

void PushButtonSimple::Enable()
{
	if ( pb)
		pb->EnableWindow(TRUE);
	_enabled = true;
}

void PushButtonSimple::Disable()
{
	if (pb)
		pb->EnableWindow(FALSE);
	_enabled = false;
}

void PushButtonSimple::show(int sw)
{
  if (pb)
		if (sw == SW_HIDE) {
			pb->ShowWindow(SW_HIDE);
			pb->SetWindowText("");
		}
		else {
			pb->SetWindowText(sQuest.c_str());
			pb->ShowWindow(SW_SHOW);
		}
}

void PushButtonSimple::SetFocus()
{
  if (pb)
    pb->SetFocus();
  _frm->ilwapp->setHelpItem(htp());
}

void PushButtonSimple::SetText(const String& sTxt)
{
	if ( pb )
		pb->SetWindowText(sTxt.c_str());
}

PushButton::PushButton(FormEntry* parent, const String& sQuestion, NotifyProc np, FormEntry* _alternativeCBHandler)
: PushButtonSimple(parent, sQuestion, np, _alternativeCBHandler, true)
{}

PushButton::PushButton(FormEntry* parent, const String& sQuestion, NotifyProc np, bool fParent)
: PushButtonSimple(parent, sQuestion, np, fParent, true)
{}


FlatIconButton::FlatIconButton(FormEntry* p, const String& iconName, String sCaption, NotifyProc np, 
							   const FileName& fnObject, bool fParent, int fbs) 
: FormEntry(p, 0, true), fPar(fParent), m_fbs(fbs)
{
	int iImage = IlwWinApp()->iImage(iconName);
	HICON icon = IlwWinApp()->ilSmall.ExtractIcon(iImage);
	hIcon = icon;
	setup(sCaption, fnObject, np);
}

FlatIconButton::FlatIconButton(FormEntry* p, HICON hicon, String sCaption, NotifyProc np, 
							   const FileName& fnObject, bool fParent, int fbs)
 : FormEntry(p, 0, true), fPar(fParent), m_fbs(fbs)
{
	hIcon = hicon;
	setup(sCaption, fnObject, np);
}

void FlatIconButton::setup(const String sCaption, const FileName& fnObject, NotifyProc np) {
	_np = np;
	m_sCaption = sCaption;
	m_fnObject = fnObject;

	LOGFONT logFont;
	memset(&logFont, 0, sizeof(logFont));
	CFont *fnt = ILWISAPP->GetFont(IlwisWinApp::sfFORM);
	fnt->GetLogFont(&logFont);
	switch (m_fbs)
	{
		case fbsTITLE:
			logFont.lfWeight = FW_BOLD;
			logFont.lfHeight *= 1.34;  // increase by 33%
			break;
		default:   // fbsNORMAL
			break;
	}
	m_fntBut = new CFont;
	m_fntBut->CreateFontIndirect(&logFont);

	ICONINFO ii;
	ZeroMemory(&ii, sizeof(ICONINFO));
	::GetIconInfo(hIcon, &ii);
	int cxIcon = (int)(ii.xHotspot * 2);
	int cyIcon = (int)(ii.yHotspot * 2);

	zDimension dim = Dim(m_sCaption, m_fntBut);
	psn->iMinWidth = dim.width() + cxIcon + 19;  // cxIcon is button width, 19 is spacing
	psn->iHeight = psn->iMinHeight = max(dim.height(), cyIcon) + 6;  // 6 is vertical spacing

	setHelpItem(htpUiPushButton);
	pb = 0;
}

FlatIconButton::~FlatIconButton()
{
	if (pb)       delete pb;
	if (m_fntBut) delete m_fntBut;
}

void FlatIconButton::create()
{
	zPoint pntFld = zPoint(psn->iPosX, psn->iPosY);
	zDimension dimFld = zDimension(psn->iMinWidth, psn->iMinHeight);
	pb = new FlatZappButton(this, _frm->wnd(), CRect(pntFld, dimFld), WS_TABSTOP|WS_GROUP|BS_ICON|BS_OWNERDRAW, 0, Id());
	pb->SetIcon(hIcon);
	pb->SetCaption(m_sCaption);
	pb->SetFont(m_fntBut);
	if (fPar)
		pb->setNotify(_par, _np, Notify(BN_CLICKED));
	else
		pb->setNotify(_frm->wnd(), _np, Notify(BN_CLICKED));

	if (m_fnObject.fValid())
		pb->setNotify(this, (NotifyProc)&FlatIconButton::OnContext, WM_RBUTTONUP);
	CreateChildren();
}

void FlatIconButton::show(int sw)
{
  if (pb)
    pb->ShowWindow(sw);
}

String FlatIconButton::sGetText() 
{
    if ( !fShow() ) return "";
    return m_sCaption;
}

void FlatIconButton::SetText(const String& sTxt)
{
	if ( pb )
		m_sCaption = sTxt.c_str();
}

void FlatIconButton::SetFocus()
{
  if (pb)
    pb->SetFocus();
  _frm->ilwapp->setHelpItem(htp());
}

int FlatIconButton::OnContext(MouseClickEvent* evt)
{
	if ( m_fbs & fbsNOCONTEXTMENU )
		return 0;
	CPoint point = evt->pos();
	pb->ClientToScreen(&point);
	IlwWinApp()->ShowPopupMenu(_frm->wnd(), point, m_fnObject);
	::ReleaseCapture();
	return 0;
}


//------ [ OwnButtonSimple ]-----------------------------------------------------------------------------
OwnButtonSimple::OwnButtonSimple(FormEntry* p, const String& sQuestion, NotifyProc np, bool fParent, bool fAutoAlign)
 : FormEntry(p, 0, fAutoAlign), fPar(fParent),alternativeCBHandler(NULL)
{
  _np = np;
  sIcon = sQuestion;
//  zDimension dim(18,23);
  zDimension dim = Dim("gh");	 
  dim.width() = 18;
  dim.height() = dim.height() + 10;  // 6 = marges (borders, fixed), 4 = font white space)

  psn->iMinWidth = dim.width();
  psn->iMinHeight = dim.height();
  setHelpItem(htpUiPushButton);
  pb = 0;
}

OwnButtonSimple::OwnButtonSimple(FormEntry* p, const String& sQuestion, NotifyProc np, FormEntry* _alternativeCBHandler) :
	FormEntry(p, 0, false), fPar(false), alternativeCBHandler(_alternativeCBHandler)
{
  _np = np;
  sIcon = sQuestion;
//  zDimension dim(18,23);
  zDimension dim = Dim("gh");
  dim.width() = 18;
  dim.height() = dim.height() + 10;  // 6 = marges (borders, fixed), 4 = font white space)

  psn->iMinWidth = dim.width();
  psn->iMinHeight = dim.height();
  setHelpItem(htpUiPushButton);
  pb = 0;
}

OwnButtonSimple::~OwnButtonSimple()
{
  if (pb)
    delete pb;
}

void OwnButtonSimple::create()
{
  zPoint pntFld = zPoint(psn->iPosX, psn->iPosY);
  zDimension dimFld = zDimension(psn->iMinWidth, psn->iMinHeight - 1);
  pb = new ZappButton(this, _frm->wnd(), CRect(pntFld, dimFld), WS_TABSTOP | BS_ICON, 0, Id());
  if (sIcon.length()) 
  {
    zIcon ic(sIcon.c_str());
    pb->SetIcon(ic);
    
  }	
  if( alternativeCBHandler != NULL) {
		pb->setNotify(alternativeCBHandler, _np, Notify(BN_CLICKED));
  }
  else  if (fPar)
    pb->setNotify(_par, _np, Notify(BN_CLICKED));
  else
    pb->setNotify(_frm->wnd(), _np, Notify(BN_CLICKED));
  CreateChildren();
}

void OwnButtonSimple::show(int sw)
{
  if (pb)
    pb->ShowWindow(sw);
}

void OwnButtonSimple::SetFocus()
{
  if (pb)
    pb->SetFocus();
  _frm->ilwapp->setHelpItem(htp());
}

void OwnButtonSimple::DrawItem(DRAWITEMSTRUCT* dis)
{
  CBrush br(GetSysColor(COLOR_BTNFACE));
  CPen penFrame(PS_SOLID ,1, GetSysColor(COLOR_WINDOWFRAME));
  CPen penLight(PS_SOLID ,1, GetSysColor(COLOR_BTNHIGHLIGHT));
  CPen penDark(PS_SOLID ,1, GetSysColor(COLOR_BTNSHADOW));
  HANDLE hBrush;
  HANDLE hPen;

  RECT rect = dis->rcItem;
  hBrush = SelectObject(dis->hDC,br);
  hPen = SelectObject(dis->hDC,penFrame);
  rect.bottom -= 1;
  rect.right -= 1;
  Rectangle(dis->hDC,rect.left,rect.top,rect.right,rect.bottom);

  bool fPress = (dis->itemState & ODS_SELECTED);
  if (sIcon.length()) {
    zIcon ic(sIcon.c_str());
    DrawIcon(dis->hDC, rect.left, rect.top-1, ic);
  }
  rect.right  -= 1;
  rect.bottom -= 1;
  for (int i = 0; i < 2; ++i) {
    rect.left	+= 1;
    rect.top	+= 1;
    rect.right	-= 1;
    rect.bottom -= 1;
    if (i == 0) {
      MoveToEx(dis->hDC,rect.left,rect.bottom, NULL);
      SelectObject(dis->hDC, hPen);
      if (fPress)
	      hPen = SelectObject(dis->hDC,penDark);
      else
	      hPen = SelectObject(dis->hDC,penLight);
      LineTo(dis->hDC,rect.left,rect.top);
      LineTo(dis->hDC,rect.right,rect.top);
    }
    else
      MoveToEx(dis->hDC,rect.right,rect.top, NULL);
    if (fPress)
      break;
      //SelectObject(dis->hDC,penLight);
    else {
      SelectObject(dis->hDC, hPen);
      hPen = SelectObject(dis->hDC,penDark);
    }
    LineTo(dis->hDC,rect.right,rect.bottom);
    LineTo(dis->hDC,rect.left-1,rect.bottom);
  }
  SelectObject(dis->hDC,hBrush);
  SelectObject(dis->hDC,hPen);
}

//--[ FieldLine ]----------------------------------------------------------------
FieldSeparatorLine::FieldSeparatorLine(FormEntry *fe, int _iLength, FormEntry::BevelStyle _bsStyle ) :
	FieldBlank(fe, 0.05),
	iLength(_iLength),
	bsStyle(_bsStyle)
{
	if ( iLength == iUNDEF)
		iLength = 200;
	psn->iMinWidth = _iLength;
	psn->iWidth = _iLength;
	SetIndependentPos();
	SetBevelStyle(bsStyle);
}

void FieldSeparatorLine::create()
{
	FieldBlank::create();
}
