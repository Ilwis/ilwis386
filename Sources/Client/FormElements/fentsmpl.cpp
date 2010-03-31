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
/* $Log: /ILWIS 3.0/FormElements/fentsmpl.cpp $
 * 
 * 41    20-04-05 16:49 Willem
 * Changing the value range of FieldIntSimple now also adjusts the range
 * for the associated spin control
 * 
 * 40    7-01-05 14:04 Hendrikse
 * Allow custom style and stepsize (used in Orthophoto fiducial mark form)
 * 
 * 39    4-10-04 19:00 Hendrikse
 * added  ScrollToTopLine() for the Output page of the DatumWizard
 * 
 * 38    29-01-03 22:14 Hendrikse
 * delayed call to SetVal(_iVal); to avoid replacing undef value by 1 in
 * fieldintsimple with spinner
 * 
 * 37    20-01-03 14:24 Hendrikse
 * In FieldIntSimple::create() changed SetRange into SetRange32 ( see its
 * definition in CSpinButtonCtrl::SetRange , MSDN) so that the FieldRowCol
 * using a spinner can make use of limit 99999 when using new
 * FieldIntSimple. Hence the nr of displayed and accepted digits for Row
 * and col are clearly limited
 * 
 * 36    12/03/01 17:24 Willem
 * Added SetValueRange function to FieldInt
 * 
 * 35    6-03-01 16:07 Koolhoven
 * in FieldIntSimple initialize sbc on 0 
 * 
 * 34    3/06/01 14:16 Retsios
 * Small memory leak: Spin Control
 * 
 * 33    5-02-01 20:08 Koolhoven
 * in SetVal() maximize on 12 chars instead of 9 or 10 (more than 12
 * really do not fit)
 * 
 * 32    20-09-00 18:51 Koolhoven
 * removed superfluous SetVal() in  FieldRealSimple::CheckData() which
 * caused problems !?
 * 
 * 31    31-08-00 14:39 Koolhoven
 * destructors of several FormElements were not prepared to destruct when
 * create() was never called. This will happen with property pages which
 * are never activated
 * 
 * 30    24/08/00 18:14 Willem
 * StaticText and StaticTextSimple now have an extra parameter:
 * fRemovePrefix. If this is set to true an '&' character will be
 * displayed as-is instead of as an underscore under the following
 * character
 * 
 * 29    6-07-00 11:52a Martin
 * bug removed from FieldIntSimple
 * 
 * 28    6-07-00 11:26a Martin
 * new fieldint that accepts a valuerange but also the UNDEF value
 * 
 * 27    6/07/00 9:23 Willem
 * The Y-pos of the text in the statictext is now calculated correctly
 * 
 * 26    3/07/00 9:52 Willem
 * Attempted to position the caret in FieldStringMulti at the beginning of
 * the text (no succes yet)
 * 
 * 25    26/06/00 12:19 Willem
 * Added Enable() and Disable() functions to FieldStringMulti and
 * FieldStringMultiSimple
 * 
 * 24    5-06-00 17:21 Koolhoven
 * Remove text when text is hidden to prevent accelerator problems
 * 
 * 23    3-04-00 19:19 Koolhoven
 * SpinCtrl now shows/hides together with FieldInt
 * 
 * 22    28/03/00 17:21 Willem
 * StaticText now has NoWrap style
 * 
 * 21    28/03/00 16:17 Willem
 * FieldSimplexxxx now check member pointer first
 * 
 * 20    9/02/00 15:39 Willem
 * Allow a small margin to compensate for rounding effects
 * 
 * 19    1-02-00 10:14a Martin
 * an ellipses will be added to the end of a string if it does not fit in
 * the 'static edit'.
 * Colors (font and background) of an String edit can now be set.
 * 
 * 18    28-01-00 12:27p Martin
 * the _psVal meber of a FieldStringSimple remains zero in case of read
 * only strings. This makes it possible to use local variable in this
 * semi-static.
 * 
 * 17    28-01-00 11:44 Hendrikse
 * added Enable() and Disable() for FiedInt and FieldIntSimple
 * 
 * 16    25/01/00 13:08 Willem
 * When FieldStringSimple has ES_READONLY style the the fAllowEmpty flag
 * is set to false; this allows the checkdata to return true when the
 * string is empty, although the fAllowEmpty originally is false.
 * 
 * 15    29/11/99 15:28 Willem
 * - Added (default) parameter style to constructors of FieldString and
 * FieldStringSimple to be able to set the style of them to R/O
 * - Added OnSetFocus() message handler to reset a possible selection in a
 * R/O FieldString
 * 
 * 14    18-10-99 3:43p Martin
 * some protections against to early calls of window functions (hwnd must
 * exist)
 * 
 * 13    18-10-99 11:03a Martin
 * size of static text is not recalculated when value changes (must
 * already be set)
 * 
 * 12    21-09-99 15:35 Koolhoven
 * StringMultiEdit improved
 * 
 * 11    20-09-99 12:47 Koolhoven
 * calculation of dimension and setting of font of StaticText improved
 * 
 * 10    16-09-99 16:08 Koolhoven
 * FieldInt now refuses longer (more positions) numbers when fit in
 * ValueRange
 * FieldInt has extra boolean parameter to indicate if a spincontrol
 * should be added.
 * 
 * 9     14-09-99 2:21p Martin
 * corrected string retrieval for text that includes '&'
 * 
 * 8     25-08-99 9:14a Martin
 * Static text are now the correct length
 * 
 * 7     23/08/99 15:10 Willem
 * The StaticTextSimple.SetVal() function now sets the member Text even if
 * the window is not (yet) created.
 * 
 * 6     13/07/99 12:50 Willem
 * The atof() strtod() fucntion gave strange problems. (also during
 * linking). The functions have been renamed to be unique. They are now
 * called by their real name. The C library functions atof() and strtod()
 * are not used directly anymore
 * 
 * 5     9-06-99 3:37p Martin
 * Changes for general callbacks
 * 
 * 4     3/05/99 15:37 Willem
 * Changed the comment character around the RCS log message
 * 
 * 3     4/22/99 2:53p Martin
 * Needed a disable for fieldreal
// Revision 1.4  1998/09/16 17:37:53  Wim
// 22beta2
//
// Revision 1.3  1997/09/13 14:38:27  Wim
// DoubleEdit::text(s) now uses its formatter
//
// Revision 1.2  1997-09-13 14:57:53+02  Wim
// Use RealFormatter in DoubleEdit
//
/* fentsmpl.c
// Simple  Form entries

// by Jelle Wind aug. 1994
	Last change:  WK   10 Aug 98   10:57 am
*/

/*#include "ui.h"
#include "Engine\Domain\dm.h"
#include "Engine\Domain\Dmvalue.h"
#include "ZappInterface\realfrmt.h*/

#include "Client\Headers\formelementspch.h"
#include "Client\Base\LongFrmt.h"
#include "Client\Base\REALFRMT.H"
#include "Engine\Domain\Dmvalue.h"
#include "Engine\Base\strtod.h"
#include "Client\ilwis.h"

const unsigned int iDefaultStyle=ES_AUTOHSCROLL|WS_TABSTOP|WS_GROUP|WS_BORDER;


FieldGroupSimple::FieldGroupSimple(FormEntry* p, bool fIndependentPositioning, bool fAutoAlign)
  : FormEntry(p, 0, fAutoAlign)
{
    psn->fIndependentPos = fIndependentPositioning;
}

FieldGroupSimple::~FieldGroupSimple()
{}

void FieldGroupSimple::create()
{
    CreateChildren();
}

String FieldGroupSimple::sGetText()
{
    return sChildrensText();
}

StaticTextSimple::StaticTextSimple(FormEntry* p, const String& sQuestion, bool fAutoAlign, bool fRemPrefix)
  : FormEntry(p, 0, fAutoAlign), fRemovePrefix(fRemPrefix)
{
    sText = sQuestion;
    txt = 0;
    fnt = 0;
    zDimension dimTxt = Dim(sText);
    psn->iMinWidth = dimTxt.width();
    psn->iMinHeight = dimTxt.height();
    if (!fAutoAlign)
        psn->iBndRight = 10;
};


StaticTextSimple::~StaticTextSimple()
{

    if (txt) delete txt;
}

String StaticTextSimple::sGetText()
{
    CString s;
    txt->GetWindowText(s);
    return String(s);
}

void StaticTextSimple::create()
{
    zDimension dimTxt( psn->iWidth, psn->iMinHeight );

    CPoint pntTxt(psn->iPosX, psn->iPosY);
    pntTxt.y += (psn->iHeight - dimTxt.height()) / 2; // center vertically

	DWORD style = WS_CHILD | WS_VISIBLE | SS_LEFTNOWORDWRAP;
	if (fRemovePrefix)
		style |= SS_NOPREFIX;  // remove ampersands
    txt = new CStatic();
    txt->Create( sText.sVal(), style ,CRect(pntTxt, dimTxt),_frm->wnd());
		if (0 == fnt)
			txt->SetFont(_frm->fnt);
    //Hide();
    txt->ShowWindow(SW_HIDE);
}

void StaticTextSimple::SetVal(const String& sVal)
{
	sText = sVal;
	if (txt && txt->GetSafeHwnd() != NULL) 
	{
		txt->SetWindowText(sVal.scVal());
	}  
}


void StaticTextSimple::show(int sw)
{
	if (txt && txt->GetSafeHwnd() != NULL) {
		if (sw == SW_HIDE) {
			txt->ShowWindow(SW_HIDE);
			txt->SetWindowText("");
		}
		else {
			txt->SetWindowText(sText.scVal());
			txt->ShowWindow(SW_SHOW);
		}
	}
}

void StaticTextSimple::Enable()
{
  if (txt && txt->GetSafeHwnd() != NULL)
    txt->EnableWindow(TRUE);
}

void StaticTextSimple::Disable()
{
  if (txt && txt->GetSafeHwnd() != NULL)
    txt->EnableWindow(FALSE);
}

BEGIN_MESSAGE_MAP(StringEdit, zStringEdit)
    ON_WM_SETFOCUS()
END_MESSAGE_MAP()

void StringEdit::OnSetFocus(CWnd* wnd)
{
	zStringEdit::OnSetFocus(wnd);

	// disable the selection when it is read-only, and place cursor at beginning
	if ((GetStyle() & ES_READONLY) == ES_READONLY)
		SetSel(0,0);
}

StringEdit::StringEdit(FormEntry* f, CWnd *w, const CRect& rct, DWORD style, int id, String *dest, unsigned long flags ) :
           zStringEdit(f, w, rct, style, id, dest, flags)
{ ilwapp = IlwWinApp()->Context(); }

//-----------------------------------------------------------------------------------------------[ StringMultiEdit ]-----
BEGIN_MESSAGE_MAP(StringMultiEdit, ZappEdit)
    ON_WM_SETFOCUS()
    //ON_WM_KILLFOCUS()
END_MESSAGE_MAP()

StringMultiEdit::StringMultiEdit(FormEntry* fe, CWnd *w, const CRect& rct, DWORD style, int id, char *pcInit) 
: ZappEdit(fe, w, rct, style, 0, id),
    ilwapp(IlwWinApp()->Context())
{
    SetWindowText(pcInit);
}


afx_msg void StringMultiEdit::OnSetFocus(CWnd* wnd)
{
	ZappEdit::OnSetFocus(wnd);
	SetSel(-1,0);
//  ilwapp->SetHelpTopic(_fe->htp());
//    static String s= text();
//    text(s);
}

/*
afx_msg void StringMultiEdit::OnKillFocus(CWnd*)
{
    ilwapp->SetHelpTopic(_fe->frm()->htp());
    SetSel(-1,0);
}
*/

//---------------------------------------------------------------------------------------------[ FieldStringSimple ]-------
FieldStringSimple::FieldStringSimple(FormEntry* p, Parm *prm, const Domain& dom, bool fAllowEmptyString, DWORD dwstyle)
  : FormEntry(p, prm), style(dwstyle),
	textColor(iUNDEF),
	backColor(iUNDEF)
{
    if (dom.fValid())
        dm = dom;
    else
        dm = Domain("string");
    fAllowEmpty = fAllowEmptyString;
		if ((dwstyle & ES_READONLY) == ES_READONLY)
			fAllowEmpty = true;
    _sVal = _prm->sVal();
    _psVal = 0;
    psn->iMinWidth = FLDSTRINGWIDTH;
    se = 0;
}

FieldStringSimple::FieldStringSimple(FormEntry* p, String *psVal, const Domain& dom, bool fAllowEmptyString, DWORD dwstyle)
  : FormEntry(p), style(dwstyle),
	textColor(iUNDEF),
	backColor(iUNDEF)
{
    if (dom.fValid())
        dm = dom;
    else
        dm = Domain("string");
    fAllowEmpty = fAllowEmptyString;
		if (fReadOnly())
			fAllowEmpty = true;
    _sVal = *psVal;
    _psVal = fReadOnly() ? NULL : psVal;
    psn->iMinWidth = FLDSTRINGWIDTH;
    se = 0;
}

FieldStringSimple::FieldStringSimple(FormEntry* p, String *psVal, DWORD dwstyle)
  : FormEntry(p), style(dwstyle),
	textColor(iUNDEF),
	backColor(iUNDEF)
{
    dm = Domain("string");
    fAllowEmpty = false;
		if ( fReadOnly())
		{
			fAllowEmpty = true;
		}
    _sVal = *psVal;
    _psVal = fReadOnly() ? NULL : psVal;
    psn->iMinWidth = FLDSTRINGWIDTH;

    se = 0;
}

FieldStringSimple::~FieldStringSimple()
{
    if (se)  delete se;
}

void FieldStringSimple::ClearData() {
	SetVal("");
}

void FieldStringSimple::create()
{
    CPoint pntFld = CPoint(psn->iPosX, psn->iPosY);
    zDimension dimFld = zDimension(psn->iMinWidth, psn->iMinHeight);
    if (_psVal) 
    _sVal = *_psVal;
    se = new StringEdit(this, _frm->wnd(), CRect(pntFld, dimFld), style, Id(), &_sVal);
    se->setStoreAddr(&str);
    se->SetFont(_frm->fnt);
		se->SetBackGroundColor(backColor);
		se->SetTextColor(textColor);

    if (_npChanged)
        se->setNotify(_cb, _npChanged, Notify(EN_CHANGE));
    CreateChildren();
}

void FieldStringSimple::SetTextColor(const Color& clr)
{
	textColor = clr;
}

void FieldStringSimple::SetBackGroundColor(const Color& clr)
{
	backColor = clr;
}

bool FieldStringSimple::fReadOnly()
{
	return (style & ES_READONLY) == ES_READONLY;
}

bool FieldStringSimple::fShowElipses()
{
	if ( fReadOnly())
	{
		zDimension dim = Dim(_sVal);
		if ( dim.cx > psn->iMinWidth + 2) // allow small margin
			return true;
	}
	return false;
}

String FieldStringSimple::sGetText()
{
	if (se)
    return se->text();
	else
		return String("");
}

void FieldStringSimple::SetVal(const String& sVal)
{ 
	if (se)
	{
		se->SetVal(sVal); 
		if (_npChanged)
			(_cb->*_npChanged)(0);
	}
}


String FieldStringSimple::sVal()
{
	if (se)
	{
		se->storeData();
		_sVal = str.sVal();
		return _sVal;
	}
	return String("");
}

void FieldStringSimple::show(int sw)
{
	if (se)
		se->ShowWindow(sw);
}

FormEntry* FieldStringSimple::CheckData()
{
  if (!fShow() || !se) 
		return 0;

	se->storeData();
	_sVal = str.sVal();
	if (!fAllowEmpty && (_sVal.length() == 0))
		return this;
	if (!dm->fValid(_sVal)) return this;
	return se->checkData() ? FormEntry::CheckData() : this;
}

void FieldStringSimple::StoreData()
{
  if (fShow() && se) 
  {
    se->storeData();
    _sVal = str.sVal();
    if (_psVal != 0)
      *_psVal = _sVal;
    else
			if ( !fReadOnly())
				_prm->Replace(_sVal);
  }
  FormEntry::StoreData();
}

void FieldStringSimple::SetFocus()
{
	if (se)
		se->SetFocus();
  _frm->ilwapp->SetHelpTopic(htp());
}

//-------------------------------------------------------------------------------------------------[ FieldStringMultiSimple ]----------
FieldStringMultiSimple::FieldStringMultiSimple(FormEntry* p, String *psVal, DWORD styl)
  : FormEntry(p), style(styl), fFixedFont(false)
{
  fAllowEmpty = true;
  _sVal = *psVal;
  _psVal = psVal;
  psn->iMinWidth = FLDSTRINGWIDTH;
  se = 0;
}

FieldStringMultiSimple::~FieldStringMultiSimple()
{
  if (se) {
    if (_npChanged)
      se->removeNotify(_cb, Notify(EN_CHANGE));
    delete se;
  }
}

void FieldStringMultiSimple::create()
{
  zPoint pntFld = zPoint(psn->iPosX, psn->iPosY);
  zDimension dimFld = zDimension(psn->iMinWidth, psn->iMinHeight);
  if (_psVal) 
    _sVal = *_psVal;
  se = new StringMultiEdit(this, _frm->wnd(), CRect(pntFld, dimFld), style, Id(), _sVal.sVal());
  se->LimitText(50000);
  se->setStoreAddr(&str);
  if (fFixedFont) 
  {
    CFont fnt;
    se->SetFont(fnt.FromHandle((HFONT)GetStockObject(ANSI_FIXED_FONT)));
  }
  else
    se->SetFont(_frm->fnt);
  if (_npChanged)
    se->setNotify(_cb, _npChanged, Notify(EN_CHANGE));
	// disable the selection when it is read-only, and place cursor at beginning
  if ((se->GetStyle() & ES_READONLY) == ES_READONLY) {
		se->LineScroll(-se->GetLineCount() + 1, 0);
  }
  else
	se->SetSel(-1,0);

	CreateChildren();
}

void FieldStringMultiSimple::SetFixedFont()
{
  fFixedFont = true;
}

String FieldStringMultiSimple::sVal()
{
	if (se)
	{
		se->storeData();
	 _sVal = str.sVal();
		return _sVal;
	}
	else
		return String("");
}

void FieldStringMultiSimple::show(int sw)
{
	if (se)
		se->ShowWindow(sw);
}

FormEntry* FieldStringMultiSimple::CheckData()
{
  if (!fShow() || (!se)) 
		return 0;

  se->storeData();
  _sVal = str.sVal();
  if (!fAllowEmpty && (_sVal.length() == 0))
    return this;
  return se->checkData() ? FormEntry::CheckData() : this;
  return 0;
}

void FieldStringMultiSimple::StoreData()
{
  if (fShow() && se) 
  {
    se->storeData();
    _sVal = str.sVal();
    if (_psVal != 0)
      *_psVal = _sVal;
    else
      _prm->Replace(_sVal);
  }
  FormEntry::StoreData();
}

void FieldStringMultiSimple::SetFocus()
{
	if (se)
		se->SetFocus();
  _frm->ilwapp->SetHelpTopic(htp());
}

void FieldStringMultiSimple::Enable()
{
	if (se) se->EnableWindow(TRUE);
}

void FieldStringMultiSimple::Disable()
{
	if (se) se->EnableWindow(FALSE);
}

String FieldStringMultiSimple::sGetText()
{
	if (se)
    return (String)se->text();
	else
		return String("");
}

void FieldStringMultiSimple::ScrollToTopLine()
{
	if (se !=0)
	{
		int nFirstVisible = se->GetFirstVisibleLine();
		if (nFirstVisible > 0)
			se->LineScroll(-nFirstVisible);
	}
}
////-----------------------------------------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(FormattedEdit, zFormattedEdit)
    ON_WM_SETFOCUS()
    ON_WM_KILLFOCUS()
END_MESSAGE_MAP()

FormattedEdit::FormattedEdit(FormEntry* fe, CWnd *parent, const CRect& rct, unsigned int iStyle, const String* psInit,  
                               int iId, zFormatter *form, char *pcPict, unsigned int iFlags) :
               zFormattedEdit(fe, parent, &rct, iStyle, psInit, iId, form, iFlags),
               ilwapp(IlwWinApp()->Context())
{
}

afx_msg void FormattedEdit::OnSetFocus(CWnd* oldWindow)
{
  CEdit::OnSetFocus(oldWindow);           
  ilwapp->SetHelpTopic(_fe->htp());       
                            
}

afx_msg void FormattedEdit::OnKillFocus(CWnd* oldWindow)
{
  CEdit::OnKillFocus(oldWindow);
  ilwapp->SetHelpTopic(_fe->frm()->htp()); 
  SetSel(-1, 0); 
}                
//----------------------------------------------------------------------------------------[ LongEdit ]--------

LongEdit::LongEdit(FormEntry* fe, CWnd* w, const CRect& siz, DWORD styl, int id, long *dest, int iWidth) :
          FormattedEdit(fe, w, siz, styl, NULL, id, new LongFormatter(iWidth), 0, 0),
          _storedLong(dest)
{ 
}

bool LongEdit::storeData() 
{
	if (_storedLong) {
		String s = text();
		*_storedLong = atol(s.sVal());
		return true;
	} else return false;
}
//---------------------------------------------------------------------------------------[ DoubleEdit ]-------

DoubleEdit::DoubleEdit(FormEntry* fe, CWnd *w, const CRect& siz, DWORD styl, int id, double *dest, unsigned long flags) :
            FormattedEdit(fe, w, siz, styl, NULL, id, new RealFormatter, NULL, flags),
            _storedDouble(dest)
{  
}

void DoubleEdit::text(const String& s)
{
  String str;
  zRange pos(0,0);
  int iLen = strlen(s.scVal());
  format->addChars(str, s.scVal(), iLen, pos);
  ZappEdit::text(str);
}

String DoubleEdit::text()
{ 
  String s = ZappEdit::text();
  if ( s != "")
    return s;
  return String("%e", rUNDEF);      
}

double* DoubleEdit::setStoreAddr(double *d) 
{
  double *temp = _storedDouble;
	_storedDouble = d;
	return temp;
}

bool DoubleEdit::storeData() 
{
	if (_storedDouble) 
  {
		String s = text();
    format->getChars(s, _iFieldFlags); // switched on Wim 13/9/97
		*_storedDouble = atofILW(s.sVal());
		return true;
	} else return false;
}

int DoubleEdit::setToDefault() 
{
	char buf[60];
	char buf2[60];
	char cnt[10];
	char fmt[10];
  int n=0;
	
	if ((n=format->getFracDigits())!=iUNDEF) 
  {
    if (format) 
    {
      
      itoa(n, cnt, 10);
  		strcpy(fmt, "%.");
		  strcat(fmt, cnt);
		  strcat(fmt, "f");
		  sprintf(buf, fmt, *_storedDouble);
    }
    else
		  sprintf(buf, "%f", *_storedDouble);

		char *s = buf;
		while(*s && *s != '.') s++;
		if (*s == '.') 
    {
			for (s = buf + strlen(buf) - 1; *s != '.'; *s--) 
      {
				if (*s == '0') *s = 0;
				else break;
			}
		}
		if (*_storedDouble >= 0.0) 
    {
			buf2[0] = '+';
			strcpy(buf2+1,buf);
			strcpy(buf, buf2);
		}
		String str(buf);
		zRange r(0, 0x7fff);
		SetSel(r.lo(), r.hi());
		pasteStr(str);
		return 1;
	}
	return 0;
}

//-------------------------------------------------------------------------------------------------[ FieldIntSimple ]-----
FieldIntSimple::FieldIntSimple(FormEntry* p, Parm *prm, const Domain& dom, bool fSpinControl)
  : FormEntry(p, prm), le(0), sbc(0), vri(-LONG_MAX+1,LONG_MAX), fSpinCtrl(fSpinControl), fAcceptUndef(false)
{
  if (dom.fValid()) {
    DomainValueRangeStruct dvrs(dom);
    ValueRange valri = dvrs.vr();
    if (valri.fValid())
      vri = ValueRange(valri->riMinMax());
  }
  _iVal = _prm->iVal();
  _pVal = 0;
  _tp = tpLONG;
  psn->iMinWidth = FLDINTWIDTH;
  if (vri->riMinMax().iHi() > 999999L)
    psn->iMinWidth = FLDREALWIDTH;
}

FieldIntSimple::FieldIntSimple(FormEntry* p, byte *pbVal, const ValueRange& valri, bool fSpinControl)
  : FormEntry(p), le(0), sbc(0), vri(valri), fSpinCtrl(fSpinControl), fAcceptUndef(false)
{
  _pVal = pbVal;
  _iVal = *pbVal;
  _tp = tpBYTE;
  psn->iMinWidth = FLDINTWIDTH;
}

FieldIntSimple::FieldIntSimple(FormEntry* p, int *piVal, const ValueRange& valri, bool fSpinControl)
  : FormEntry(p), le(0), sbc(0), vri(valri), fSpinCtrl(fSpinControl), fAcceptUndef(false)
{
  _pVal = piVal;
  _iVal = *piVal;
  _tp = tpINT;
  psn->iMinWidth = FLDINTWIDTH;
}

FieldIntSimple::FieldIntSimple(FormEntry* p, long *piVal, const ValueRange& valri, bool fSpinControl)
  : FormEntry(p), le(0), sbc(0), vri(valri), fSpinCtrl(fSpinControl), fAcceptUndef(false)
{
  _pVal = piVal;
  _iVal = *piVal;
  _tp = tpLONG;
  psn->iMinWidth = FLDINTWIDTH;
  if (vri->riMinMax().iHi() > 999999L)
    psn->iMinWidth = FLDREALWIDTH;
}


FieldIntSimple::FieldIntSimple(FormEntry* p, long *piVal, const ValueRange& valri, bool fSpinControl, bool _fAcceptUndef)
  : FormEntry(p), le(0), sbc(0), vri(valri), fSpinCtrl(fSpinControl), fAcceptUndef(_fAcceptUndef)
{
  _pVal = piVal;
  _iVal = *piVal;
  _tp = tpLONG;
  psn->iMinWidth = FLDINTWIDTH;
  if (vri->riMinMax().iHi() > 999999L)
    psn->iMinWidth = FLDREALWIDTH;
}

FieldIntSimple::FieldIntSimple(FormEntry* p, byte *pbVal, const Domain& dom, bool fSpinControl)
  : FormEntry(p), le(0), sbc(0), vri(0, 255), fSpinCtrl(fSpinControl), fAcceptUndef(false)
{
  if (dom.fValid()) {
    DomainValueRangeStruct dvrs(dom);
    ValueRange valri = dvrs.vr();
    if (valri.fValid())
      vri = ValueRange(valri->riMinMax());
  }
  _pVal = pbVal;
  _iVal = *pbVal;
  _tp = tpBYTE;
  psn->iMinWidth = FLDINTWIDTH;
}

FieldIntSimple::FieldIntSimple(FormEntry* p, int *piVal, const Domain& dom, bool fSpinControl)
  : FormEntry(p), le(0), sbc(0), vri(-32766, 32767), fSpinCtrl(fSpinControl), fAcceptUndef(false)
{
  if (dom.fValid()) {
    DomainValueRangeStruct dvrs(dom);
    ValueRange valri = dvrs.vr();
    if (valri.fValid())
      vri = ValueRange(valri->riMinMax());
  }
  _pVal = piVal;
  _iVal = *piVal;
  _tp = tpINT;
  psn->iMinWidth = FLDINTWIDTH;
}

FieldIntSimple::FieldIntSimple(FormEntry* p, long *piVal, const Domain& dom, bool fSpinControl)
  : FormEntry(p), le(0), sbc(0), vri(-LONG_MAX+1,LONG_MAX), fSpinCtrl(fSpinControl), fAcceptUndef(false)
{
  if (dom.fValid()) {
    DomainValueRangeStruct dvrs(dom);
    ValueRange valri = dvrs.vr();
    if (valri.fValid())
      vri = ValueRange(valri->riMinMax());
  }
  _pVal = piVal;
  _iVal = *piVal;
  _tp = tpLONG;
  psn->iMinWidth = FLDINTWIDTH;
  if (vri->riMinMax().iHi() > 999999L)
    psn->iMinWidth = FLDREALWIDTH;
}


FieldIntSimple::~FieldIntSimple()
{
	if (le) {
		if (_npChanged)
			le->removeNotify(_cb, Notify(EN_CHANGE));
		delete le;
	}
	if (fSpinCtrl)
		delete sbc;
}

void FieldIntSimple::create()
{
  zPoint pntFld = zPoint(psn->iPosX, psn->iPosY);
  zDimension dimFld = zDimension(psn->iMinWidth, psn->iMinHeight);
	int iLo = vri->riMinMax().iLo();
	int iHi = vri->riMinMax().iHi();
  String sLo("%li", iLo);
  String sHi("%li", iHi);
  int iWidth = max(sLo.length(), sHi.length());
/*
  String s;
  if (vri->riMinMax().iLo() < 0)
    s = "-";
  s &= String('#', iWidth);  
*/  
  le = new LongEdit(this, _frm->wnd(), CRect(pntFld, dimFld),
        iDefaultStyle, Id(), &_iVal, iWidth);
  le->SetFont(_frm->fnt);
	zPoint pnt = pntFld;
	pnt.x += dimFld.width();
	zDimension dim = dimFld;
	dim.width() = 10;
	if (fSpinCtrl) {
		sbc = new CSpinButtonCtrl();
		sbc->Create(UDS_ARROWKEYS|UDS_SETBUDDYINT|UDS_NOTHOUSANDS|
			UDS_AUTOBUDDY|UDS_ALIGNRIGHT|WS_VISIBLE|WS_CHILD,
			CRect(pnt, dim), _frm->wnd(), Id());
		sbc->SetRange32(iLo, iHi);
		sbc->SetBase(10);
		sbc->SetPos(_iVal);
	}
  SetVal(_iVal);      
  if (_npChanged)
    le->setNotify(_cb, _npChanged, Notify(EN_CHANGE));
  CreateChildren();
}

void FieldIntSimple::ClearData() {
  _iVal = iUNDEF;
  SetVal(_iVal);
}

String FieldIntSimple::sGetText()
{
    return (String)le->text();
}

void FieldIntSimple::SetVal(long iVal)
{ 
  _iVal = iVal;
  if (0 == le)
    return;
  if (iVal == iUNDEF || !vri->riMinMax().fContains(iVal))
    le->text("");
  else  
    le->SetVal(iVal);
  if (_npChanged)
    (_cb->*_npChanged)(0);
}

long FieldIntSimple::iVal()
{
  if (le)
    le->storeData();
  return _iVal;
}

void FieldIntSimple::show(int sw)
{
  if (le) {
    le->ShowWindow(sw);
		if (fSpinCtrl)
			sbc->ShowWindow(sw);
	}
}

void FieldIntSimple::SetValueRange(const ValueRange& vr)
{
    vri = vr;
    if (!vr.fValid() || sbc == 0)
        return;

    int iLo = vri->riMinMax().iLo();
    int iHi = vri->riMinMax().iHi();
    sbc->SetRange32(iLo, iHi);
    // check if the new range contains the current value
    // otherwise change the current value to fit into the range
    if (_iVal >= 0)
    {
        if (_iVal < iLo)
            _iVal = iLo;
        if (_iVal > iHi)
            _iVal = iHi;
        sbc->SetPos(_iVal);
    }
}

FormEntry* FieldIntSimple::CheckData()
{
  if (!fShow()) return 0;
  if (le) 
  {
    String s = le->text();
    if (0 == s.length() && !fAcceptUndef)
      return this;
	
		else
		{
			le->storeData();
			if ( !(fAcceptUndef && ( s == "" || s == "?" )))
				if (!vri->riMinMax().fContains(_iVal))
					return this;
		}
  }  
  return FormEntry::CheckData();
}

void FieldIntSimple::StoreData()
{
  if (fShow()) {
    if (le)
		{
			if ( fAcceptUndef && (le->text() == "" || le->text() =="?"))
			{
				if ( _tp == tpBYTE )
					_iVal = 0;
				else
					_iVal=	iUNDEF;
			}
			else
				le->storeData();
		}
    if (_pVal != 0)
      switch (_tp) {
        case tpBYTE: *((byte*)_pVal) = _iVal; break;
        case tpINT:  *((int*) _pVal) = _iVal; break;
        case tpLONG: *((long*)_pVal) = _iVal; break;
      }
    else
      _prm->Replace((double)_iVal);
  }
  FormEntry::StoreData();
}

void FieldIntSimple::Enable()
{
	if (le) le->EnableWindow(TRUE);
}

void FieldIntSimple::Disable()
{
	if (le) le->EnableWindow(FALSE);
}

void FieldIntSimple::SetFocus()
{
  if (le)
    le->SetFocus();
  _frm->ilwapp->SetHelpTopic(htp());
}

//-----------------------------------------------------------------------------------[ FieldRealSimple ]----------------
FieldRealSimple::FieldRealSimple(FormEntry* p, Parm *prm, const Domain& dom)
  : FormEntry(p, prm), de(0), vrr(-1e300, 1e300, 0)
	, m_iStyle (iDefaultStyle)
{
  if (dom.fValid()) 
  {
    DomainValueRangeStruct dvrs(dom);
    ValueRange valrr = dvrs.vr();
    if (valrr.fValid())
      vrr = valrr;
  }
  _rVal = _prm->rVal();
  _pVal = 0;
  psn->iMinWidth = FLDREALWIDTH;
}

FieldRealSimple::FieldRealSimple(FormEntry* p, float *prVal,
                                 const ValueRange& vr)
  : FormEntry(p), de(0), vrr(vr)
	, m_iStyle (iDefaultStyle)
{
  _rVal = *prVal;
  _pVal = prVal;
  _tp = tpFLOAT;
  psn->iMinWidth = FLDREALWIDTH;
}

FieldRealSimple::FieldRealSimple(FormEntry* p,
                     double *prVal, const ValueRange& vr)
  : FormEntry(p), de(0), vrr(vr)
	, m_iStyle (iDefaultStyle)
{
  _rVal = *prVal;
  _pVal = prVal;
  _tp = tpDOUBLE;
  psn->iMinWidth = FLDREALWIDTH;
}

FieldRealSimple::FieldRealSimple(FormEntry* p, float *prVal,
                                 const Domain& dom)
  : FormEntry(p), de(0), vrr(-1e38, 1e38, 0)
	, m_iStyle (iDefaultStyle)
{
  if (dom.fValid()) {
    DomainValueRangeStruct dvrs(dom);
    ValueRange valrr = dvrs.vr();
    if (valrr.fValid())
      vrr = valrr;
  }
  _rVal = *prVal;
  _pVal = prVal;
  _tp = tpFLOAT;
  psn->iMinWidth = FLDREALWIDTH;
}

FieldRealSimple::FieldRealSimple(FormEntry* p,
                     double *prVal, const Domain& dom)
  : FormEntry(p), de(0), vrr(-1e300, 1e300, 0)
	, m_iStyle (iDefaultStyle)
{
  if (dom.fValid()) {
    DomainValueRangeStruct dvrs(dom);
    ValueRange valrr = dvrs.vr();
    if (valrr.fValid())
      vrr = valrr;
  }
  _rVal = *prVal;
  _pVal = prVal;
  _tp = tpDOUBLE;
  psn->iMinWidth = FLDREALWIDTH;
}

//  de->setToDefault();
FieldRealSimple::~FieldRealSimple()
{
	if (de) {
		if (_npChanged)
			de->removeNotify(_cb, Notify(EN_CHANGE));
		delete de;
	}
}

void FieldRealSimple::ClearData() {
  _rVal = rUNDEF;
  SetVal(_rVal);
}

void FieldRealSimple::SetStyle(unsigned int iStyle)
{
	m_iStyle = iStyle;
}

void FieldRealSimple::create()
{
  zPoint pntFld = zPoint(psn->iPosX, psn->iPosY);
  zDimension dimFld = zDimension(psn->iMinWidth, psn->iMinHeight);

  de = new DoubleEdit(this, _frm->wnd(), CRect(pntFld, dimFld),
                         m_iStyle, Id(), 
                         &_rVal);
  de->SetFont(_frm->fnt);
	
  SetVal(_rVal);

  CreateChildren();
  if (_npChanged)
    de->setNotify(_cb, _npChanged, Notify(EN_CHANGE));
}

void FieldRealSimple::Enable()
{
	if (de) de->EnableWindow(TRUE);
}

void FieldRealSimple::Disable()
{
	if (de) de->EnableWindow(FALSE);
}

void FieldRealSimple::setNotifyFocusChanged(CallBackHandler* evh, NotifyProc np)
{ 
  de->setNotify(evh, np, WM_KILLFOCUS); 
  de->setNotify(evh, np, WM_SETFOCUS); 
}

void FieldRealSimple::removeNotifyFocusChange(CallBackHandler *wnd)
{ 
  de->removeNotify(wnd, WM_KILLFOCUS); 
  de->removeNotify(wnd, WM_SETFOCUS); 
}

double FieldRealSimple::rVal()
{
  if (de)
    de->storeData();
  return _rVal;
}

String FieldRealSimple::sGetText()
{
    return (String)de->text();
}

void FieldRealSimple::SetVal(double rVal)
{
  bool fChanged = (_rVal != rVal);
  _rVal = rVal;
  if (de) {
//    de->setToDefault();
    if (rVal == rUNDEF || !vrr->rrMinMax().fContains(rVal))
      de->text("");
    else {  
      String sVal = vrr->sValue(_rVal,0);
			if (sVal.length() > 12)          // to prevent "too long" text
				sVal = vrr->sValue(_rVal,12);
      if (sVal != de->text())
        de->text(sVal);
    }  
//    char* s = sVal;
//    while (*s == ' ') ++s;
  }  
  if (fChanged && _npChanged)
    (_cb->*_npChanged)(0);
}

void FieldRealSimple::SetStepSize(double rStep)
{
  vrr.SetStep(rStep);
  StoreData();
  SetVal(_rVal);
}

void FieldRealSimple::show(int sw)
{
  if (de  && de->GetSafeHwnd() != NULL)
    de->ShowWindow(sw);
}

FormEntry* FieldRealSimple::CheckData()
{
  if (!fShow()) return 0;
  if (de) {
    String s = de->text();
    if (0 == s.length())
      return this;
    de->storeData();
    if (vrr->stUsed() == stREAL) {
      if (!vrr->rrMinMax().fContains(_rVal))
        return this;
    }
    else {
      if (iUNDEF == vrr->iRaw(_rVal))
        return this;
    }
  }  
  return FormEntry::CheckData();
}

void FieldRealSimple::StoreData()
{
  if (fShow()) {
    if (de)
      de->storeData();
    if (_pVal != 0)
      switch (_tp) {
        case tpFLOAT: *((float*)_pVal) = _rVal; break;
        case tpDOUBLE:  *((double*) _pVal) = _rVal; break;
      }
    else
      _prm->Replace(_rVal);
  }
  FormEntry::StoreData();
}


void FieldRealSimple::SetFocus()
{
  if (de)
    de->SetFocus();
  _frm->ilwapp->SetHelpTopic(htp());
}
//--------------------------------------------------------------------------------------[ FieldValueSimple ]--------------
/*
#define genfocus()                          \
 if (ev->gainFocus())                       \
    ilwapp->SetHelpTopic(_fe->htp());       \
  else {                                    \
    ilwapp->SetHelpTopic(_fe->frm()->htp());  \
     selection(-1, 0);                      \
  }                                         \
  return 0;

int StringEdit::focus(zFocusEvt* ev)
{ genfocus(); }

int StringMultiEdit::focus(zFocusEvt* ev)
{ genfocus(); }

int LongEdit::focus(zFocusEvt* ev)
{ genfocus(); }

int DoubleEdit::focus(zFocusEvt* ev)
{ genfocus(); }*/

FieldValueSimple::FieldValueSimple(FormEntry* p, Parm *prm,
                                   const Domain& dom, bool fAllowEmpty)
  : FieldGroupSimple(p)
{
  _sVal = prm->sVal();
  _psVal = 0;
  dm = dom;
  if (dm->pdvi())
    fe = new FieldIntSimple(p, prm, dm, false);
  else if (dm->pdvr())
    fe = new FieldRealSimple(p, prm, dm);
  else
    fe = new FieldStringSimple(p, prm, dm, fAllowEmpty);
}

FieldValueSimple::FieldValueSimple(FormEntry* p, String *psVal,
                                   const Domain& dom, bool fAllowEmpty)
  : FieldGroupSimple(p)
{
  _sVal = *psVal;
  _psVal = psVal;
  dm = dom;
  if (dm->pdvi())
    fe = new FieldIntSimple(p, &iVal, dm, false);
  else if (dm->pdvr())
    fe = new FieldRealSimple(p, &rVal, dm);
  else
    fe = new FieldStringSimple(p, psVal, dm, fAllowEmpty);
}


FieldValueSimple::~FieldValueSimple()
{
}


String FieldValueSimple::sVal()
{
  if (dm->pdv()) {
    if (dm->pdvr())
      return dm->pdvr()->sValue(((FieldRealSimple*)fe)->rVal());
    else
      return dm->pdvi()->sValue(((FieldIntSimple*)fe)->iVal());
  }
  else
    return ((FieldStringSimple*)fe)->sVal();
}

void FieldValueSimple::StoreData()
{
  if (fShow()) {
    if (_psVal != 0)
      *_psVal = sVal();
  }
  FormEntry::StoreData();
}


void FieldValueSimple::SetFocus()
{
  fe->SetFocus();
  _frm->ilwapp->SetHelpTopic(htp());
} 





