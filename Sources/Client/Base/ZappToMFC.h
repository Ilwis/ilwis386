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
#ifndef ZappToMFC_H
#define ZappToMFC_H

#include "Headers\toolspch.h"
#include "Engine\Base\DataObjects\Color.h"
#include "Client\Base\events.h"
#include "Client\Base\NfyMap.h"
#include "Engine\Base\DataObjects\Pair.h"
#include <queue>

class FormEntry;

using namespace std;

typedef queue<CBrush *, list<CBrush *> >	brushQueue;
typedef queue<CPen *, list<CPen *> >		  penQueue;
typedef queue<CFont *, list<CFont *> >	  fontQueue;

enum fileAttrib
{RW=0,RO=1,HIDDEN=2,SYSTEM=4,DIRS=16,ARCHIVE=32,DRIVES=0x4000,ONLY=(unsigned int) 0x8000};

#undef IMPEXP
#ifdef ILWISCLIENT
#define IMPEXP __declspec(dllexport)
#else
#define IMPEXP __declspec(dllimport)
#endif

class _export zDisplay
{
public:
  zDisplay();
	zDisplay(CWnd* owner);
	zDisplay(CDC *dc);
	~zDisplay();

	int				  pushPen(CPen *);
	CPen				*popPen();
	int					pushBrush(CBrush *);
	CBrush			*popBrush();
	int					pushFont(CFont *);
	CFont				*popFont();
	zDimension	getTextDim(const char *s, const int c);
	CFont*		  font();
	int					lock();
	int					unlock();
	CDC*				cdc();
	int         pixPerInchY();
	int         pixPerInchX();
	BOOL        rectangle(CRect& rct);
	BOOL        ellipse(CRect& rct);
	BOOL				lineTo(const int x, const int y);
	zPoint      moveTo(const int x, const int y);
	zPoint      moveTo(const CPoint& pnt);
	BOOL        lineTo(const CPoint& pnt);
	BOOL        polygon(CPoint *pnts, int iCount);
	int         setTextBackMode(int tm);
	BOOL        text(const CPoint& pnt, String s);
	COLORREF    textColor(Color clr);
	Color       backColor(const Color c) ;


protected:
	brushQueue	  _brushes;
	penQueue			_pens;
	fontQueue			_fonts;

private:
	CDC*				  _tempDC;
	CWnd*				  _owner;
	int					  _iLockCount;
  CPen*         _oldPen;
  CBrush*       _oldBrush;
  CFont*        _oldFont;
	PAINTSTRUCT	  _paintStruct;
};

int _export XDIALOG(unsigned int x, CDC *cdc=NULL);
int _export YDIALOG(unsigned int x, CDC *cdc=NULL);

class _export zFontInfo
{
	public:
		zFontInfo(zDisplay *dsp);
		
		int			height();
		int			width();
		int			weight();
		int			iPixWidth();
		int			iPixHeight();
		int			family();
		int			pitch();

	private:
		CFont*		_font;
		zDisplay	*_disp;
		TEXTMETRIC	tm;
};

class zClipboard
{
public:
	_export zClipboard(CWnd *win);

	void  _export	add(char *s, UINT uFormat = CF_TEXT);
	void  _export	clear();
	bool  _export	isTextAvail();
	char  _export	*getText(UINT uFormat = CF_TEXT);
        
private:       
	int				res;
	CWnd*			w;
};

class zFormatter
{
  public:
    _export zFormatter(DWORD mask);
    virtual ~zFormatter() {};

    enum zFormatterStat { zFmtStatBad=0, zFmtStatSimpleIns=1, zFmtStatReplace=2 };
    bool                    hasStyle(DWORD w) 
                              { return (w & flags) != 0; }
    virtual zFormatterStat  addChars(String &s, const char *c, int count,zRange &pos) = 0;
    virtual bool _export    checkChars(String &s, zRange &pos) = 0;
    virtual int             getChars(String &str, unsigned long flags) { return 1;}
    virtual int  _export    getFracDigits() { return iUNDEF; }

  protected :
	  DWORD flags;
};

//typedef pair<CWnd *, NotifyProc> CallBackPair;



//-- zapp edit implementations are  in Edit.cpp
class BaseZapp
{
  public:
		_export BaseZapp(FormEntry *f=NULL);
		_export ~BaseZapp();

    void _export      removeNotify(CallBackHandler *, const Message& mp);
    void _export      setNotify(CallBackHandler *handler, NotifyProc npCh, const Message&);
    bool _export      fProcess(Event& ev);

 protected:
				FormEntry         *_fe;	

 private:
    NotifyMap         Notifies; 
};

class IMPEXP ZappEdit : public CEdit, public BaseZapp
{
public:
  ZappEdit(FormEntry *fe, CWnd *, const CRect& , unsigned int iFlags=0, const String *psInit=0, int iId=0);

  virtual void      setStoreAddr(String* str);
  virtual bool      storeData();
  virtual bool      checkData();
  virtual void      text(const String& sVal);
  virtual String    text();
  zRange            selectionRange();
 

  afx_msg void      OnChange();
  afx_msg void      OnKillFocus( CWnd* pNewWnd );
  afx_msg void      OnSetFocus( CWnd* pNewWnd );

protected:
  NotifyProc        _npChanged;
  String            *_storedString; 
  unsigned long     _iFieldFlags;



DECLARE_MESSAGE_MAP();

};

     
class IMPEXP zEditLine : public ZappEdit
{
public:
  zEditLine(FormEntry *fe, CWnd *, const CRect& , unsigned int iFlags, const String *psInit=0, int iId=0);

  
};

class IMPEXP zFormattedEdit : public zEditLine
{
public:
  zFormattedEdit(FormEntry *fe, CWnd *parent, const CRect& size, unsigned int iStyle, const String  *psInit,  
                int iId, zFormatter *format , unsigned int iFlags=0 );
  ~zFormattedEdit();

	afx_msg void             OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags );
	afx_msg void             OnChar( UINT nChar, UINT nRepCnt, UINT nFlags );
//	virtual afx_msg LRESULT  OnCut(WPARAM , LPARAM);
  virtual afx_msg LRESULT  OnPaste(WPARAM , LPARAM);
 	int                      pasteStr(String &str);
	String                   getRawText(unsigned long options);
	virtual bool             checkData();
	zFormatter               *getFormatter(); 
  void                     setFormatter(zFormatter *fmter);
	afx_msg HBRUSH           CtlColor(CDC *p, UINT nCtlColor );
	void                     SetTextColor(Color clr);
  void                     SetBackGroundColor(Color clr);
    
protected:
	afx_msg BOOL OnEraseBkgnd( CDC* pDC );

	zFormatter *format;
  Color  textColor;
	Color  backColor;
  DECLARE_MESSAGE_MAP();
};

class zStringEdit : public zFormattedEdit
{
public:
   _export zStringEdit(FormEntry *fe, CWnd *parent, const CRect& rct, int iStyle, int iId, const String *psInit, 
               unsigned long iFlags, zFormatter *pcPict=0);

};

// implemented in button.cpp
class IMPEXP ZappButton : public CButton, public BaseZapp
{
public:
	ZappButton(FormEntry *f, CWnd *w, const CRect& siz, DWORD style, const char* sQuest =0, int id = 0);
	~ZappButton();

	afx_msg void OnKillFocus( CWnd* pNewWnd );
	afx_msg void OnSetFocus( CWnd* pNewWnd );
	afx_msg void OnButtonClicked();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
    afx_msg void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

  DECLARE_MESSAGE_MAP()
   
};

#endif




