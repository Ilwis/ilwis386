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


/* ui/fentmisc.h
// Interface for derived classes from FormEntry
// by Wim Koolhoven, aug. 1993
// modified by Jelle Wind, dec 1993 - june 1994
// (c) Computer Department ITC
Last change:  WK   10 Aug 98   10:51 am
*/
#ifndef FENTMISC_H
#define FENTMISC_H
#include "Client\Editors\Utils\FlatButton.h"
#ifndef ILWRES_H
#include "Client\Base\Res.h"
#endif

#include "afxdtctl.h"

class _export FieldGroup: public FieldGroupSimple
{
public:
	FieldGroup(FormEntry* p, bool fIndependentPositioning = false);
	bool fGroup() const { return true; } // overriden
	void create() { CreateChildren(); }
};

class _export FieldBlank: public FormEntry
{
public:
	FieldBlank(FormEntry*, double rRatio = 0.5);
	String sGetText();
	void create();
	// rRatio: height of entry as ratio of default height
	~FieldBlank();
};

class _export FieldSeparatorLine : public FieldBlank
{
public:
	FieldSeparatorLine(FormEntry *fe, int _iLength = iUNDEF, FormEntry::BevelStyle _bsStyle = FormEntry::bsRAISED); 
	void create();

private:
	FormEntry::BevelStyle bsStyle;
	int iLength;
};

class _export StaticText: public StaticTextSimple
{
public:
	StaticText(FormEntry* p, const String& sText, bool fBold=false, bool fRemPrefix = false);
	~StaticText();
	void create();
	void Font(CFont *fnt, bool fDelete = true);
	// set font of formentry, if fDelete than font is automatically deleted
	void FontEnlarge(float rFac); // enlarge font
private:
	void Bold();
	bool fDeleteFont;                  // flag for delete font in destructor
	bool fBold;
};

class _export StaticIcon: public FormEntry
{
public:
	StaticIcon(FormEntry* p, HICON icon, bool fLarge=false);
	~StaticIcon();
	void create();
private:
	HICON icon;
	bool fLarge;
	CStatic* st;
};

class FieldString: public FormEntry
{
public:
	_export FieldString(FormEntry* parent, const String& sQuestion,
		Parm *prm, const Domain& dm = Domain(), bool fAllowEmpty = true, DWORD style = ES_AUTOHSCROLL|WS_TABSTOP|WS_GROUP|WS_BORDER);
	_export FieldString(FormEntry* parent, const String& sQuestion,
		String *psVal, const Domain& dm = Domain(),bool fAllowEmpty = true, DWORD style = ES_AUTOHSCROLL|WS_TABSTOP|WS_GROUP|WS_BORDER);
	_export FieldString(FormEntry* parent, String *psVal, DWORD style = ES_AUTOHSCROLL|WS_TABSTOP|WS_GROUP|WS_BORDER);
	_export ~FieldString();
	void SetVal(const String& sVal)
	{ fld->SetVal(sVal); }
	String sVal()       // return current field value
	{ return fld->sVal(); }
	FormEntry* CheckData()     // overriden
	{ return fld->CheckData(); }
	void SetReadOnly(bool fValue)
	{ 
		if (fld) fld->SetReadOnly(fValue);
	}
	void _export create();             // overriden
	void SelectAll() { fld->SelectAll(); }
	void StoreData()           // overriden
	{ fld->StoreData(); }
	void SetFocus();
	void SetHelpTopic(const HelpTopic& htp) // overriden
	{ fld->SetHelpTopic(htp); }
	void SetCallBack(NotifyProc np)
	{ FormEntry::SetCallBack(np); fld->SetCallBack(np); }
	void SetCallBack(NotifyProc np, CallBackHandler* cb)
	{ FormEntry::SetCallBack(np,cb); fld->SetCallBack(np,cb); }
	void _export SetWidth(short iWidth);
	void _export SetHeight(short iHeight);
	void _export Disable() 
	{ fld->disable(); }
	void _export Enable()
	{ fld->enable(); }

	String _export sGetText();
	void   _export SetTextColor(const Color& clr);
	void   _export SetBackGroundColor(const Color& clr);
protected:
	StaticTextSimple *st;           // text before field
	FieldStringSimple *fld;   // edit field
	StaticTextSimple *stElipses;
};

class FieldStringMulti: public FormEntry
{
public:
	_export FieldStringMulti(FormEntry* parent, String *psVal, DWORD style);
	_export FieldStringMulti(FormEntry* parent, String *psVal, bool fReadOnly=false);
	_export FieldStringMulti(FormEntry* parent, const String& sQuestion, String *psVal, bool fReadOnly=false);
	_export ~FieldStringMulti();
	void SetVal(const String& sVal)
	{ fld->SetVal(sVal); }
	String sVal()       // return current field value
	{ return fld->sVal(); }
	FormEntry* CheckData()     // overriden
	{ return fld->CheckData(); }
	void _export create();             // overriden
	void StoreData()           // overriden
	{ fld->StoreData(); }
	void _export SetFocus();
	void SetHelpTopic(const HelpTopic& htp) // overriden
	{ fld->SetHelpTopic(htp); }
	void SetCallBack(NotifyProc np)
	{ FormEntry::SetCallBack(np); fld->SetCallBack(np); }
	void SetCallBack(NotifyProc np, CallBackHandler* cb)
	{ FormEntry::SetCallBack(np,cb); fld->SetCallBack(np,cb); }
	void _export SetWidth(short iWidth);
	void _export SetHeight(short iHeight);
	void SetFixedFont()
	{ fld->SetFixedFont(); }
	String _export sGetText();
	void _export ScrollToTopLine();
	void _export ClearData();

	virtual void _export Enable();
	virtual void _export Disable();


private:
	StaticTextSimple *st;           // text before field
	FieldStringMultiSimple *fld;   // edit field
};


class FieldInt: public FormEntry
{
public:
	_export FieldInt(FormEntry* parent, const String& sQuestion, Parm *prm,
		const Domain& dm = Domain(), bool fSpinControl=false);
	_export FieldInt(FormEntry* parent, const String& sQuestion, byte *pbVal,
		const ValueRange& vri = ValueRange(0,255), bool fSpinControl=false);
	// pbVal is destination of result
	_export FieldInt(FormEntry* parent, const String& sQuestion, int *piVal,
		const ValueRange& vri = ValueRange(-32766,32767), bool fSpinControl=false);
	// piVal is destination of result
	_export FieldInt(FormEntry* parent, const String& sQuestion, long *piVal,
		const ValueRange& vri = ValueRange(-LONG_MAX+1,LONG_MAX), bool fSpinControl=false);
	// piVal is destination of result
	_export FieldInt(FormEntry* parent, const String& sQuestion, byte *pbVal,
		const Domain& dm, bool fSpinControl=false);
	// pbVal is destination of result
	_export FieldInt(FormEntry* parent, const String& sQuestion, int *piVal,
		const Domain& dm, bool fSpinControl=false);
	// piVal is destination of result
	_export FieldInt(FormEntry* parent, const String& sQuestion, long *piVal,
		const Domain& dm, bool fSpinControl=false);
	_export FieldInt(FormEntry* p, const String& sQuestion,
		long *piVal, const ValueRange& vri, bool fSpinControl, bool fAcceptUndef);
	// piVal is destination of result
	_export ~FieldInt();

	void SetVal(long iVal)
	{ fld->SetVal(iVal); }
	void SetReadOnly(bool fVal)
	{
		fld->SetReadOnly(fVal);
	}
	long iVal() // return current field value
	{ return fld->iVal(); }
	String sGetText();
	void SetvalueRange(const ValueRange& vr)
	{
		fld->SetValueRange(vr);
	}
	FormEntry* CheckData()     // overriden
	{ return fld->CheckData(); }

	void create();             // overriden
	void StoreData()           // overriden
	{ fld->StoreData(); }

	void SetFocus();
	void SetHelpTopic(const HelpTopic& htp) // overriden
	{ fld->SetHelpTopic(htp); }
	void SetCallBack(NotifyProc np)
	{ FormEntry::SetCallBack(np); fld->SetCallBack(np); }
	void SetCallBack(NotifyProc np, CallBackHandler* cb)
	{ FormEntry::SetCallBack(np,cb); fld->SetCallBack(np,cb); }
	void SetWidth(short iWidth);
	void _export Enable();
	void _export Disable();
private:
	StaticTextSimple *st;           // text before field
	FieldIntSimple *fld;   // edit field
};

class FieldReal: public FormEntry
{
public:
	_export FieldReal(FormEntry* parent, const String& sQuestion, Parm *prm,
		const Domain& dm = Domain());
	_export FieldReal(FormEntry* parent, const String& sQuestion,
		float *prVal, const ValueRange& vrr = ValueRange(-1e38,1e38,1e-6));            
	// prVal is destination of result
	_export FieldReal(FormEntry* parent, const String& sQuestion,
		double *prVal, const ValueRange& vrr = ValueRange(-1e300,1e300,1e-6));
	_export FieldReal(FormEntry* parent, const String& sQuestion,
		float *prVal, const Domain& dm);
	// prVal is destination of result
	_export FieldReal(FormEntry* parent, const String& sQuestion,
		double *priVal, const Domain& dm);
	// prVal is destination of result
	_export ~FieldReal();
	//  void Show(int sw) { fld->Show(sw); }  // overriden
	void SetVal(double rVal)
	{ fld->SetVal(rVal); }
	double rVal() // return current field value
	{ return fld->rVal(); }
	void SetStepSize(double rStep)
	{ fld->SetStepSize(rStep); }
	void SetReadOnly(bool fVal)
	{
		fld->SetReadOnly(fVal);
	}	
	FormEntry* CheckData()     // overriden
	{ return fld->CheckData(); }
	void create();             // overriden
	void _export StoreData()           // overriden
	{ fld->StoreData(); }
	void SetFocus();
	void SetHelpTopic(const HelpTopic& htp) // overriden
	{ fld->SetHelpTopic(htp); }
	void SetCallBack(NotifyProc np)
	{ FormEntry::SetCallBack(np); fld->SetCallBack(np); }
	void SetCallBack(NotifyProc np, CallBackHandler* cb)
	{ FormEntry::SetCallBack(np,cb); fld->SetCallBack(np,cb); }
	void SetWidth(short iWidth);
	void _export Enable();
	void _export Disable();
	void show(int s);

	String sGetText();
private:
	StaticTextSimple *st;           // text before field
	FieldRealSimple *fld;   // edit field
};

// range form entries

class FieldRangeInt: public FormEntry
{
public:
	_export FieldRangeInt(FormEntry* parent, const String& sQuestion, Parm *prm,
		const ValueRange& vri = ValueRange(-LONG_MAX+1, LONG_MAX));
	_export FieldRangeInt(FormEntry* parent, const String& sQuestion,
		RangeInt *rng, const ValueRange& vri = ValueRange(-LONG_MAX+1, LONG_MAX));
	_export ~FieldRangeInt();
	void SetVal(RangeInt& rngVal)
	{ fiLo->SetVal(rngVal.iLo());
	fiHi->SetVal(rngVal.iHi()); }
	void SetReadOnly(bool fVal)
	{
		fiLo->SetReadOnly(fVal);
		fiHi->SetReadOnly(fVal);
	}	
	RangeInt rngVal();         // current value
	//  void Show(int sw);         // overriden
	FormEntry* CheckData();    // overriden
	void create();             // overriden
	void StoreData();          // overriden
	void SetFocus();           // overriden
	void SetHelpTopic(const HelpTopic& htp) // overriden
	{ fiLo->SetHelpTopic(htp); fiHi->SetHelpTopic(htp); }
	void SetCallBack(NotifyProc np) // overriden
	{ FormEntry::SetCallBack(np); fiLo->SetCallBack(np); fiHi->SetCallBack(np); }
	void SetCallBack(NotifyProc np, CallBackHandler* cb)
	{ FormEntry::SetCallBack(np,cb); fiLo->SetCallBack(np,cb); fiHi->SetCallBack(np,cb); }
	void SetWidth(short iWidth);
	String sGetText();

	void _export Enable();
	void _export Disable();

private:
	//  void Init(const String& sQuestion, RangeInt rng, const Domain&);
	void Init(const String& sQuestion, RangeInt rng, const ValueRange& vri);
	StaticTextSimple *st;
	FieldIntSimple *fiLo, *fiHi;     // range consists of two form entries
	RangeInt *_rng;
	long _iLo, _iHi;
};

class FieldRangeReal: public FormEntry
{
public:
	_export FieldRangeReal(FormEntry* parent, const String& sQuestion,
		RangeReal *rng, const ValueRange& vrr = ValueRange(-1e300,1e300,1e-6));
	FieldRangeReal(FormEntry* parent, const String& sQuestion, Parm *prm,
		const ValueRange&);

	_export ~FieldRangeReal();
	void SetVal(RangeReal& rngVal)
	{ frLo->SetVal(rngVal.rLo());
	frHi->SetVal(rngVal.rHi()); }
	void SetStepSize(double rStep);
	void SetReadOnly(bool fVal)
	{
		frLo->SetReadOnly(fVal);
		frHi->SetReadOnly(fVal);
	}	
	RangeReal rngVal();        // current value
	//  void Show(int sw);         // overriden
	FormEntry* CheckData();    // overriden
	void create();             // overriden
	void StoreData();          // overriden
	void SetFocus();           // overriden
	String sGetText();
	void SetHelpTopic(const HelpTopic& htp) // overriden
	{ frLo->SetHelpTopic(htp); frHi->SetHelpTopic(htp); }
	void SetCallBack(NotifyProc np) // overriden
	{ FormEntry::SetCallBack(np); frLo->SetCallBack(np); frHi->SetCallBack(np); }
	void SetCallBack(NotifyProc np, CallBackHandler* cb)
	{ FormEntry::SetCallBack(np,cb); frLo->SetCallBack(np,cb); frHi->SetCallBack(np,cb); }
	void SetWidth(short iWidth);

	void _export Enable();
	void _export Disable();

private:
	//  void Init(const String& sQuestion, RangeReal rng, const Domain&);
	void Init(const String& sQuestion, RangeReal rng, const ValueRange&);
	StaticTextSimple *st;
	FieldRealSimple *frLo, *frHi;    // range consists of two form entries
	RangeReal *_rng;
	double _rLo, _rHi;
};


class FieldRowCol: public FormEntry
{
public:
	FieldRowCol(FormEntry* parent, const String& sQuestion, Parm *prm);
	_export FieldRowCol(FormEntry* parent, const String& sQuestion, RowCol *rc, bool fSpin = false);
	_export ~FieldRowCol();
	void SetVal(RowCol rc)
	{ fiRow->SetVal(rc.Row);
	fiCol->SetVal(rc.Col); }
	RowCol rcVal();        // current value
	//  void Show(int sw);         // overriden
	FormEntry* CheckData();    // overriden
	void create();             // overriden
	void StoreData();          // overriden
	void SetFocus();           // overriden
	void SetReadOnly(bool fVal)
	{
		fiRow->SetReadOnly(fVal);
		fiCol->SetReadOnly(fVal);
	}	
	void SetHelpTopic(const HelpTopic& htp) // overriden
	{ fiRow->SetHelpTopic(htp); fiCol->SetHelpTopic(htp); }
	void SetCallBack(NotifyProc np) // overriden
	{ FormEntry::SetCallBack(np); fiRow->SetCallBack(np); fiCol->SetCallBack(np); }
	void SetCallBack(NotifyProc np, CallBackHandler* cb)
	{ FormEntry::SetCallBack(np,cb); fiRow->SetCallBack(np,cb); fiCol->SetCallBack(np,cb); }
	void SetWidth(short iWidth);
	String sGetText();
	void setNotifyFocusChange(CallBackHandler* evh, NotifyProc np)
	{ fiRow->setNotifyFocusChanged(evh, np);
	fiCol->setNotifyFocusChanged(evh, np); }
	void removeNotifyFocusChange(CallBackHandler* evh, NotifyProc np)
	{ fiRow->removeNotifyFocusChange(evh);
	fiCol->removeNotifyFocusChange(evh); }
	bool _export fIncludesHandle(HANDLE hnd);
protected:
	bool fSpinning;
	void Init(const String& sQuestion);
private:
	StaticTextSimple *st;
	FieldIntSimple *fiRow, *fiCol;    // coord consists of two form entries
	RowCol *_rc;
	RowCol rc;
};

class FieldCoord: public FormEntry
{
public:
	FieldCoord(FormEntry* parent, const String& sQuestion, Parm *prm);
	_export FieldCoord(FormEntry* parent, const String& sQuestion, Coord *crd);
	_export ~FieldCoord();
	void SetVal(const Coord& crd)
	{ frX->SetVal(crd.x);
	frY->SetVal(crd.y); }
	Coord _export crdVal();        // current value
	//  void Show(int sw);         // overriden
	FormEntry* CheckData();    // overriden
	void create();             // overriden
	void StoreData();          // overriden
	void SetFocus();           // overriden
	void SetReadOnly(bool fVal)
	{
		frX->SetReadOnly(fVal);
		frY->SetReadOnly(fVal);
	}	
	void SetHelpTopic(const HelpTopic& htp) // overriden
	{ frX->SetHelpTopic(htp); frY->SetHelpTopic(htp); }
	void SetCallBack(NotifyProc np) // overriden
	{ FormEntry::SetCallBack(np); frX->SetCallBack(np); frY->SetCallBack(np); }
	void SetCallBack(NotifyProc np, CallBackHandler* cb)
	{ FormEntry::SetCallBack(np,cb); frX->SetCallBack(np,cb); frY->SetCallBack(np,cb); }
	void SetWidth(short iWidth);
	String sGetText();
	void setNotifyFocusChange(CallBackHandler* evh, NotifyProc np)
	{ frX->setNotifyFocusChanged(evh, np);
	frY->setNotifyFocusChanged(evh, np); }
	void removeNotifyFocusChange(CallBackHandler* evh, NotifyProc np)
	{ frX->removeNotifyFocusChange(evh);
	frY->removeNotifyFocusChange(evh); }
	bool _export fIncludesHandle(HANDLE hnd);
	void SetStyle(unsigned int iStyle)
	{
		frX->SetStyle(iStyle);
		frY->SetStyle(iStyle);
	}
	void _export SetStepSize(double rStep)
	{
		frX->SetStepSize(rStep);
		frY->SetStepSize(rStep);
	}
private:
	void Init(const String& sQuestion);
	StaticTextSimple *st;
	FieldRealSimple *frX, *frY;    // coord consists of two form entries
	Coord *_crd;
	Coord crd;
};

class RadioGroup;

class _export RadBut: public ZappButton
{
public:
	RadBut(FormEntry* f, CWnd *w, const CRect& siz, DWORD style, const char* sQuest =0, int id = 0);

	bool check()
	{ return GetCheck() ? true : false; }

	void check(bool fState)
	{ SetCheck(fState ? 1: 0); }

private:
	IlwisAppContext *ilwapp;


	//int focus(zFocusEvt* ev);
};

class RadioButton : public FormEntry
{
	friend RadioGroup;
public:
	RadioButton(RadioGroup* parent, const String& sQuestion, const String& sParm);
	_export RadioButton(RadioGroup* parent, const String& sQuestion);
	// if iWidth == 0 is is set to default offset of form
	_export ~RadioButton();
	_export bool fVal();               // return current value of radiobutton
	void _export SetVal(bool);         // set value of radio button
	void show(int sw);         // overriden
	void create();             // overriden
	void StoreData();          // overriden
	void SetFocus();           // overriden
	bool fRadioButton() const { return true; }  // overriden
	void Enable(); 
	void Disable();
	String sGetText();
	bool styleSet(DWORD style) { return rb->ModifyStyle(style, 0) == TRUE; }
private:
	RadBut *rb;          // radio button control
	String sQuest;             // text after radio button
};

typedef Array<String*>      StringPArray;

class RadioGroup: public FormEntry
{
	friend class RadioButton;
public:
	RadioGroup(FormEntry* parent, const String& sQuestion, Parm *prm, bool fInRows = false);
	_export RadioGroup(FormEntry* parent, const String& sQuestion, int *piVal, bool fInRows = false);
	// piVal: destination of result value (indicates the ..th radio button,
	// starting from 0)
	_export ~RadioGroup();
	//  void Show(int sw);         // overriden
	void _export ClearData();
	void _export SetVal(int iVal);
	int _export iVal();                // return current value
	String sVal();             // return current value
	void create();             // overriden
	void StoreData();          // overriden
	void SetFocus();           // overriden
	bool fGroup() const { return true; } // overriden
	void Enable();
	void Disable();
	String sGetText();
private:
	int ButtonClicked(void *); // callback for clicking in radio button
	void AddStrParm(const String& sParm); // adds sParam to the list of possible values
	int _iVal, *_piVal;        // place holders for value
	StaticTextSimple *st;            // for title
	StringPArray sParmList;  // parm values for each radio button
	bool fAlignHor;
};

class _export ChckBox: public ZappButton
{
public:
	ChckBox(FormEntry* f, CWnd *w, const CRect& siz, DWORD style,  char* sQuest =0, int id = 0);
	void      SetVal(bool fVal) 
	{ check(fVal); }
	long      check()
	{ return ((GetState() & 0x0003)); }
	void      check(bool fState)
	{ SendMessage(BM_SETCHECK, fState ? BST_CHECKED : BST_UNCHECKED,0); }

private:
	IlwisAppContext *ilwapp;
};


// pfVal destination of result
// if iWidth == 0 is is set to default offset of form
class CheckBox : public FormEntry
{
public:
	CheckBox(FormEntry* parent, const String& sQuestion, Parm *prm);
	_export CheckBox(FormEntry* parent, const String& sQuestion, bool *pfVal);
	_export CheckBox(FormEntry* p, const String& sQuestion, long *piVal);
	_export ~CheckBox();

	bool fCheckBox() const { return true; }
	bool _export fVal();               // return current value of check box
	void _export SetVal(bool fVal);
	void _export ClearData();
	int ButtonClicked(void* Ev); // callback for clicking in check box
	void show(int sw);          // overriden
	void create();             // overriden
	void StoreData();          // overriden
	void SetFocus();           // overriden
	void _export SetStyle(bool fDisabled);
	void disable() { cbx->EnableWindow(FALSE); }	     
	void enable()  { cbx->EnableWindow(); }
	String sGetText();
	bool fClicked;
	HANDLE handle() const { return cbx->m_hWnd; }
	void setDirty() { cbx->Invalidate(); }
	void setNotifyFocusChange(CallBackHandler* evh, NotifyProc np)
	{
		cbx->setNotify(evh, np, WM_KILLFOCUS);
		cbx->setNotify(evh, np, WM_SETFOCUS);
	}
	void removeNotifyFocusChange(CallBackHandler* evh, NotifyProc np)
	{
		cbx->removeNotify(evh, WM_KILLFOCUS);
		cbx->removeNotify(evh, WM_SETFOCUS);
	}
private:
	bool     _fVal, *_pfVal;       // place holders for result
	long     *_piVal;
	ChckBox  *cbx;            // check box
	String   sQuest;             // text after check box
	bool     m_fDisabled;
};


class Check3Box : public FormEntry
{
public:
	_export Check3Box(FormEntry* parent, const String& sQuestion, long *pfVal);
	_export ~Check3Box();
	bool fCheckBox() const { return true; }
	long _export iVal();               // return current value of check box: 0, 1, iUNDEF
	void _export ClearData();
	void _export SetVal(long iVal);
	int ButtonClicked(void* Ev); // callback for clicking in check box
	void show(int sw);          // overriden
	void create();             // overriden
	void _export StoreData();          // overriden
	void SetFocus();           // overriden
	void disable() { cbx->EnableWindow(FALSE); }	     
	void enable()  { cbx->EnableWindow(); }
	String sGetText();
private:
	long _iVal, *_piVal;       // place holders for result
	ChckBox *cbx;            // check box
	String sQuest;             // text after check box
};

class PushButtonSimple: public FormEntry
{
public:
	PushButtonSimple(FormEntry* parent, const String& sQuestion, NotifyProc np, bool fParent, bool fAutoAlign);
	PushButtonSimple(FormEntry* parent, const String& sQuestion, NotifyProc np, FormEntry* _alternativeCBHandler, bool fAutoAlign);
	~PushButtonSimple();
	void show(int sw);          // overriden
	void create();             // overriden
	void SetFocus();           // overriden
	void disable() { pb->EnableWindow(FALSE); }	     
	void enable() { pb->EnableWindow(); }	
	void _export SetDefault( bool fYesNo);
	void _export SetText(const String& sTxt);
	void _export Enable();
	void _export Disable();
	ZappButton *pb;  // zapp PushButton
private:
	String sQuest;
	NotifyProc _np;
	bool fPar; // callback to parent instead of form.
	bool fDefault;
	FormEntry *alternativeCBHandler;
};


class PushButton: public PushButtonSimple
{
public:
	_export PushButton(FormEntry* parent, const String& sQuestion, NotifyProc np, bool fParent = false);
	_export PushButton(FormEntry* parent, const String& sQuestion, NotifyProc np, FormEntry* _alternativeCBHandler)  ;
};

class FlatZappButton;

class FlatIconButton: public FormEntry
{
public:
	enum FlatButtonStyle { fbsNORMAL = 1, fbsTITLE = 2, fbsNOCONTEXTMENU=4 };
	_export FlatIconButton(FormEntry* p, const String& iconName, String sCaption, NotifyProc np, 
		const FileName& fnObject, bool fParent=false, int fbs=fbsNORMAL);
	_export FlatIconButton(FormEntry* parent, HICON hIcon, String sCaption, NotifyProc np, const FileName& fnObject, bool fParent = false, int fbs = fbsNORMAL);
	~FlatIconButton();
	void show(int sw);         // overriden
	void create();             // overriden
	String sGetText();         // overriden
	void SetFocus();           // overriden
	void _export SetText(const String& sTxt); 

protected:
	void setup(const String sCaption, const FileName& fnObject, NotifyProc np);
	int OnContext(MouseClickEvent* evt);

private:
	FlatZappButton  *pb;	       // zapp based flat PushButton
	String          m_sCaption;
	FileName        m_fnObject;
	HICON           hIcon;
	CFont           *m_fntBut;
	NotifyProc      _np;
	bool            fPar; // callback to parent instead of form.
	int							m_fbs;
};

class OwnButtonSimple: public FormEntry
{
public:
	_export OwnButtonSimple(FormEntry* parent, const String& sIconQuestion,  NotifyProc np, bool fParent, bool fAutoAlign);
	_export OwnButtonSimple(FormEntry* p, const String& sQuestion, NotifyProc np, FormEntry* _alternativeCBHandler);
	_export ~OwnButtonSimple();
	void show(int sw);	     // overriden
	void create();             // overriden
	void SetFocus();	     // overriden
	void DrawItem(DRAWITEMSTRUCT* dis);
private:
	ZappButton *pb;  // zapp Button
	String sIcon;
	NotifyProc _np;
	bool fPar; // callback to parent instead of form.
	FormEntry *alternativeCBHandler;
};

class _export FieldValue : public FormEntry
{
public:
	FieldValue(FormEntry* parent, const String& sQuestion,
		Parm *prm, const Domain& dm, bool fAllowEmpty = true);
	FieldValue(FormEntry* parent, const String& sQuestion,
		String *psVal, const Domain& dm, bool fAllowEmpty = true);
	~FieldValue();
	//void SetVal(const String& sVal)
	//  { fld->SetVal(sVal); }
	String sVal()       // return current field value
	{ return fld->sVal(); }
	FormEntry* CheckData()     // overriden
	{ return fld->CheckData(); }
	void create();             // overriden
	void StoreData()           // overriden
	{ fld->StoreData(); }
	void SetFocus();
	void SetHelpTopic(const HelpTopic& htp) // overriden
	{ fld->SetHelpTopic(htp); }
	void SetCallBack(NotifyProc np)
	{ FormEntry::SetCallBack(np); fld->SetCallBack(np); }
	void SetCallBack(NotifyProc np, CallBackHandler* cb)
	{ FormEntry::SetCallBack(np,cb); fld->SetCallBack(np,cb); }
	void SetWidth(short iWidth);
private:
	StaticTextSimple *st;           // text before field
	FieldValueSimple *fld;   // edit field

}; 
class CalendarSelect;

class CalendarForm : public FormWithDest {
public:
	CalendarForm(CWnd *wnd, const ILWIS::Time& date);
	COleDateTime GetDate();
	void SetDate(const ILWIS::Time& date);


private:
	CalendarSelect *calendar;


};


class _export FieldDate: public FormEntry
{
	friend class FieldTime;
public:
	FieldDate(FormEntry* parent, const String& sQuestion, Parm *prm);
	FieldDate(FormEntry* parent, const String& sQuestion, ILWIS::Time *tm);
	~FieldDate();
	void SetVal(const ILWIS::Time& data);
	//  void Show(int sw);         // overriden
	FormEntry* CheckData();    // overriden
	void create();             // overriden
	void StoreData();          // overriden
	void SetFocus();           // overriden
	void SetReadOnly(bool fVal)
	{
		frDay->SetReadOnly(fVal);
		frMonth->SetReadOnly(fVal);
		frYear->SetReadOnly(fVal);
	}
	void Enable();
	void Disable();
	void SetHelpTopic(const HelpTopic& htp) // overriden
	{  }
	void SetCallBack(NotifyProc np) // overriden
	{ FormEntry::SetCallBack(np); frDay->SetCallBack(np); frMonth->SetCallBack(np); frYear->SetCallBack(np); }
	void SetCallBack(NotifyProc np, CallBackHandler* cb)
	{ FormEntry::SetCallBack(np,cb); frDay->SetCallBack(np,cb); frMonth->SetCallBack(np,cb);frYear->SetCallBack(np,cb); }
	void SetWidth(short iWidth);
	String sGetText();
	void show(int mode) ;
	void setNotifyFocusChange(CallBackHandler* evh, NotifyProc np)
	{ frDay->setNotifyFocusChanged(evh, np);
	frMonth->setNotifyFocusChanged(evh, np); 
	frYear->setNotifyFocusChanged(evh, np); }
	void removeNotifyFocusChange(CallBackHandler* evh, NotifyProc np)
	{ frDay->removeNotifyFocusChange(evh);
	frMonth->removeNotifyFocusChange(evh); 
	frYear->removeNotifyFocusChange(evh);}
private:
	void Init(const String& sQuestion);
	int showCalendar(Event *ev);
	StaticTextSimple *st;
	FieldIntSimple *frDay, *frYear, *frMonth;    // coord consists of two form entries
	OwnButtonSimple *pbCalendar;
	ILWIS::Time *time;
	int day, month, year;
};

class _export FieldTime: public FormEntry{
public:
	FieldTime(FormEntry *f, const String& sQuestion,ILWIS::Time *time, const DomainTime* dt=NULL, ILWIS::Time::Mode m=ILWIS::Time::mUNKNOWN);
	void create();
	void SetUseDate(bool yesno);
	void show(int state) ;
	void StoreData();
	void SetVal(const ILWIS::Time& ti, ILWIS::Time::Mode m);

private:
	int checkFormat(Event *ev);
	FieldStringSimple *fsTime;
	StaticTextSimple *st;
	bool includeDate;
	ILWIS::Time *time;
	String sTime;
	ILWIS::Time::Mode mode;

};

#endif // FENTMISC_H




