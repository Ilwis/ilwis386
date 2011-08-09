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
/* ui/fentsmpl.h
// Interface for simple derived classes from FormEntry
// Jelle Wind, august 1994
	Last change:  WK   10 Aug 98   10:56 am
*/
#ifndef FENTSMPL_H
#define FENTSMPL_H

#include "Engine\Base\DataObjects\valrange.h"

class _export FieldGroupSimple: public FormEntry
{
public:
    FieldGroupSimple(FormEntry*, bool fIndependentPositioning = false, bool fAutoAlign = false);
    ~FieldGroupSimple();

    void                    create();  // overriden
    String                  sGetText();
};


class StaticTextSimple: public FormEntry
{
public:
    _export StaticTextSimple(FormEntry*, const String& sText, bool fAutoAlign = false, bool fRemPrefix = false);
    _export ~StaticTextSimple();

    void _export    SetVal(const String& sVal);
    void _export    show(int sw);  // overriden
    void _export    create();    // overriden
    void _export    Enable();
    void _export    Disable();
    String _export  sGetText();
    HANDLE          hWnd() 
                      { return txt->m_hWnd; }
protected:
    CStatic*                txt;
    String                  sText;
    CFont*                  fnt;
	bool                    fRemovePrefix;
};

class StringEdit: public zStringEdit
{
    public:
        StringEdit(FormEntry* f, CWnd *w, const CRect& rct, DWORD style, int id, String *dest, unsigned long flags = 0) ;
 
        void          SetVal(const String& sVal)
                        { text(sVal); }
        afx_msg void  OnSetFocus( CWnd* pOldWnd );

    private:
        IlwisAppContext*         ilwapp;

 	DECLARE_MESSAGE_MAP()
};


class _export StringMultiEdit: public ZappEdit
{
public:
    StringMultiEdit(FormEntry* f, CWnd *w, const CRect& rct, DWORD style, int id, char *pcInit);

    void                    SetVal(const String& sVal)
                                { SetWindowText(sVal.c_str()); }
    afx_msg void            OnSetFocus(CWnd*);
    afx_msg void            OnKillFocus(CWnd*);


private:
    IlwisAppContext*        ilwapp;

    DECLARE_MESSAGE_MAP();
};



class _export FieldStringSimple: public FormEntry
{
public:
  FieldStringSimple(FormEntry* parent, Parm *prm, const Domain& dm, bool fAllowEmpty, DWORD style = ES_AUTOHSCROLL|WS_TABSTOP|WS_GROUP|WS_BORDER);
  FieldStringSimple(FormEntry* parent, String *psVal, const Domain& dm, bool fAllowEmpty, DWORD style = ES_AUTOHSCROLL|WS_TABSTOP|WS_GROUP|WS_BORDER);
  FieldStringSimple(FormEntry* parent, String *psVal, DWORD style);
  ~FieldStringSimple();

  void            SetVal(const String& sVal);
  String          sVal();             // return current field value
  void            show(int sw);         // overriden
  FormEntry       *CheckData();    // overriden
  void            SetFocus();           // overriden
  void            enable() 
                    { if (se) se->EnableWindow(TRUE); }
  void            disable() 
                    { if (se) se->EnableWindow(FALSE); }
  void            create();             // overriden
  void            StoreData();          // overriden
  void            SelectAll() 
                    { if (se) se->SetSel(0,-1); }
  String          sGetText();
	bool            fReadOnly();
	bool            fShowElipses();
	void            SetTextColor(const Color& clr);
	void            SetBackGroundColor(const Color& clr);
	void            SetReadOnly(bool fVal)
									{ if (se) se->SetReadOnly(fVal); }
	void			ClearData();
protected:
  StringEdit      *se;           // string edit control
  String          _sVal;
  String          *_psVal;     // place holders for result
  bool            fAllowEmpty;
  Domain          dm;
  DWORD           style;

private:
  String          str;
  Color           textColor;
	Color           backColor;
};


class _export FieldStringMultiSimple: public FormEntry
{
public:
  FieldStringMultiSimple(FormEntry* parent, String *psVal, DWORD style);
  ~FieldStringMultiSimple();

  void            SetVal(const String& sVal)
                    { se->SetVal(sVal); }
  String          sVal();             // return current field value
  void            show(int sw);         // overriden
  FormEntry       *CheckData();    // overriden
  void            SetFixedFont();
  void            SetFocus();           // overriden
  void            create();             // overriden
  void            StoreData();          // overriden
  void            Enable();
  void            Disable();
  String          sGetText();
	void            ScrollToTopLine(); // not overridden

protected:
  StringMultiEdit *se;           // string edit control
  String          _sVal;
  String          *_psVal;     // place holders for result
  bool            fAllowEmpty;
  bool            fFixedFont;
  DWORD           style;

private:
  String str;  
};

enum FldDataType { tpBYTE, tpINT, tpLONG, tpFLOAT, tpDOUBLE };

class FormattedEdit: public zFormattedEdit
{
public:
  FormattedEdit(FormEntry* f, CWnd *parent, const CRect& rct, unsigned int iStyle, const String* psInit,  
                int iId, zFormatter *form, char *pcFormat , unsigned int iFlags);

  virtual afx_msg void     OnSetFocus(CWnd* pOldWnd);
  virtual afx_msg void     OnKillFocus(CWnd* pOldWnd);

protected:
  IlwisAppContext *ilwapp;

  DECLARE_MESSAGE_MAP();
};


class _export LongEdit: public FormattedEdit
{
public:
  LongEdit(FormEntry* fe, CWnd* w, const CRect& siz, DWORD style, int id, long *dest, int iWidth);

  void SetVal(long iVal)
    { text(String("%li", iVal)); }
  bool storeData();

protected:
  long  *_storedLong;
};

class _export DoubleEdit: public FormattedEdit
{
public:
  DoubleEdit(FormEntry* fe, CWnd *w, const CRect& siz, DWORD style, int id, double *dest, unsigned long flags = 0);

  void        text(const String& s);
  String      text();
  double*     setStoreAddr(double *d);
  bool        storeData();

private:
  int         setToDefault() ;

  double      *_storedDouble;
};



class _export FieldIntSimple: public FormEntry
{
public:
	FieldIntSimple(FormEntry* parent, Parm *prm, const Domain& dm, bool fSpinControl);
	FieldIntSimple(FormEntry* parent, byte *pbVal, const ValueRange& vri, bool fSpinControl);
	// pbVal is destination of result
	FieldIntSimple(FormEntry* parent, int *piVal, const ValueRange& vri, bool fSpinControl);
	// piVal is destination of result
	FieldIntSimple(FormEntry* parent, long *piVal, const ValueRange& vri, bool fSpinControl);
	// piVal is destination of result
	FieldIntSimple(FormEntry* parent, byte *pbVal, const Domain& dm, bool fSpinControl);
	// pbVal is destination of result
	FieldIntSimple(FormEntry* parent, int *piVal, const Domain& dm, bool fSpinControl);
	// piVal is destination of result
	FieldIntSimple(FormEntry* parent, long *piVal, const Domain& dm, bool fSpinControl);
	FieldIntSimple(FormEntry* p, long *piVal, const ValueRange& valri, bool fSpinControl, bool _fAcceptUndef);
	// piVal is destination of result
	~FieldIntSimple();

	void SetValueRange(const ValueRange& vr);
	void SetVal(long iVal);
	long iVal();               // return current field value
	void show(int sw);         // overriden
	FormEntry* CheckData();    // overriden
	void SetFocus();           // overriden
	void create();             // overriden
	void StoreData();          // overriden
	String sGetText();
	void ClearData();

	void setNotifyFocusChanged(CallBackHandler * wnd, NotifyProc np)
    {
		le->setNotify(wnd, np, WM_KILLFOCUS);  
		le->setNotify(wnd, np, WM_SETFOCUS); 
    }
	void removeNotifyFocusChange(CallBackHandler * wnd)
    {
		le->removeNotify(wnd, WM_KILLFOCUS); 
		le->removeNotify(wnd, WM_SETFOCUS);
    }
	void            SetReadOnly(bool fVal)
		{ if (le) le->SetReadOnly(fVal); }	
	void Enable();
	void Disable();
  HANDLE hWnd() 
         { return le->m_hWnd; }
private:
	LongEdit *le;             // long edit field
	bool fSpinCtrl;
	bool fAcceptUndef;
	CSpinButtonCtrl* sbc;
	long _iVal;                // place holder for result
	void *_pVal;               // place holder for pointer to result
	FldDataType _tp;              // type of result (byte, int or long)
	ValueRange vri;
};



class FieldRealSimple: public FormEntry
{
public:
  FieldRealSimple(FormEntry* parent, Parm *prm, const Domain& dm = 0);
  FieldRealSimple(FormEntry* parent, float *prVal, const ValueRange& vrr = ValueRange(-1e38,1e38,1e-6));
  // prVal is destination of result
  FieldRealSimple(FormEntry* parent, double *prVal, const ValueRange& vrr = ValueRange(-1e300,1e300,1e-6));
  // prVal is destination of result
  FieldRealSimple(FormEntry* parent, float *prVal, const Domain& dm);
  // prVal is destination of result
  FieldRealSimple(FormEntry* parent, double *prVal, const Domain& dm);
  // prVal is destination of result
  ~FieldRealSimple();
  void _export SetVal(double rVal);
  void _export SetStepSize(double rStep);
  void _export SetStyle(unsigned int iStyle);
  double _export rVal();     // return current value of control
  void show(int sw);         // overriden
  FormEntry* CheckData();    // overriden
  void ClearData();
  void SetFocus();           // overriden
  void create();             // overriden
  void StoreData();          // overriden
	virtual void Enable();
	virtual void Disable();
	void _export setValueRange(const ValueRange& vr) { vrr = vr;}
  String sGetText();
  void _export setNotifyFocusChanged(CallBackHandler * wnd, NotifyProc np);
  void _export removeNotifyFocusChange(CallBackHandler * wnd);
	void            SetReadOnly(bool fVal)
									{ if (de) de->SetReadOnly(fVal); }	
  HANDLE          hWnd() 
                    { return de->m_hWnd; }

private:
  DoubleEdit *de;           // double edit field
  double _rVal;              // place holder for result
  void *_pVal;               // place holder for pointer to result
  FldDataType _tp;              // type of result (float or double)
  ValueRange vrr;
// Domain dm;               // domain for validation of result
  unsigned int m_iStyle;
};

class _export FieldValueSimple: public FieldGroupSimple
{
public:
  FieldValueSimple(FormEntry* parent, Parm *prm,
                   const Domain& dm, bool fAllowEmpty);
  FieldValueSimple(FormEntry* parent, String *psVal,
                   const Domain& dm, bool fAllowEmpty);
  ~FieldValueSimple();
  String sVal();             // return current field value
  void SetFocus();           // overriden
  void StoreData();          // overriden
private:
  FormEntry* fe;
  String _sVal, *_psVal;     // place holders for result
  double rVal;
  long iVal;
  bool fAllowEmpty;
  Domain dm;
};
/*
class _export FieldStringPictSimple: public FieldStringSimple
{
public:
  FieldStringPictSimple(FormEntry* parent, String *psVal, const char* sPict);
  void create();       
private:
  const char* sPict;
  String str;  
};


*/


#endif // FENTSMPL_H




