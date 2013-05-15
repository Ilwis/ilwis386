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
#define ZAPP_EDIT_C

#include "Client\Headers\formelementspch.h"
#include "Client\Base\ZappToMFC.h"


BEGIN_MESSAGE_MAP(ZappEdit, CEdit)
  ON_CONTROL_REFLECT(EN_CHANGE, OnChange)
  ON_WM_KILLFOCUS()
  ON_WM_SETFOCUS()
END_MESSAGE_MAP()

ZappEdit::ZappEdit(FormEntry *f, CWnd *parent, const CRect& rct, unsigned int iFlags, const String *psInit, int id): 
          BaseZapp(f),
          CEdit(),
          _npChanged(NULL),
          _storedString(NULL)
{
  Create(iFlags, rct, parent, id);
  if(psInit) SetWindowText(psInit->c_str());
}


bool ZappEdit::storeData()
{
  if (_storedString) 
  {
		*_storedString = text();
		return true;
  } else return false;
}

void ZappEdit::setStoreAddr(String *sVal)
{
  _storedString=sVal;
}

bool ZappEdit::checkData() 
{
  return (text().length() !=0) || (_iFieldFlags & FLD_NOTREQUIRED);   
}

void ZappEdit::text(const String& sVal)
{
	  if ( this->GetSafeHwnd() == 0)
	  return ;
  SetWindowText(sVal.c_str());
}

String ZappEdit::text()
{ 
  if ( this->GetSafeHwnd() == 0)
	  return "";
  CString str;
  GetWindowText(str);
  return String(str);
}

zRange ZappEdit::selectionRange()
{
  long rep;
	rep=SendMessage(EM_GETSEL,0,0);
	return zRange(LOWORD(rep),HIWORD(rep));
}

afx_msg void  ZappEdit::OnChange()
{
  fProcess(NotificationEvent(GetDlgCtrlID(), EN_CHANGE));
}

afx_msg void ZappEdit::OnKillFocus( CWnd* pNewWnd )
{
	CEdit::OnKillFocus(pNewWnd);
  fProcess(Event(WM_KILLFOCUS));
}

afx_msg void ZappEdit::OnSetFocus( CWnd* pNewWnd )
{
	CEdit::OnSetFocus(pNewWnd);
  fProcess(Event(WM_SETFOCUS));
}

//------------------------------------------------------------------------------------------------------
zEditLine:: zEditLine(FormEntry *f, CWnd *parent, const CRect& rct, unsigned int iFlags, const String *psInit, int id): 
            ZappEdit(f, parent, rct, iFlags, psInit, id)
{
 
}

//------------------------------------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(zFormattedEdit, zEditLine)
  ON_WM_KEYDOWN()
  ON_WM_CHAR()
  ON_MESSAGE(WM_PASTE, OnPaste) 
//	ON_WM_CTLCOLOR_REFLECT() // for the time being disabled, seems to interupt the DEL button.
//	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

zFormattedEdit::zFormattedEdit(FormEntry *f, CWnd *parent, const CRect& rct, unsigned int iStyle, const String* psInit,  
                               int iId, zFormatter *form , unsigned int iFlags) :
                zEditLine(f, parent, rct, iStyle, psInit, iId),
                format(form),
								textColor(iUNDEF),
								backColor(iUNDEF)
{
}

zFormattedEdit::~zFormattedEdit()
{
  if (format) delete format;
}

afx_msg HBRUSH zFormattedEdit::CtlColor( CDC* pDC, UINT nCtlColor )
{
	bool fReadOnly = (GetStyle() & ES_READONLY) == ES_READONLY;
	Color cBCol = fReadOnly ? GetSysColor(COLOR_BTNFACE) : GetSysColor(COLOR_WINDOW);
	Color cTCol = fReadOnly ? GetSysColor(COLOR_3DDKSHADOW) : GetSysColor(COLOR_WINDOWTEXT);

  CBrush br(cBCol);
	pDC->SetBkColor(cBCol);

	bool fValidMessage = nCtlColor == CTLCOLOR_STATIC || nCtlColor == CTLCOLOR_EDIT;

	if ( fValidMessage && backColor != Color(iUNDEF))
	{
		pDC->SetBkColor(backColor);
		br.CreateSolidBrush(backColor);
		pDC->SetBkMode(TRANSPARENT);
	}
	if (fValidMessage && textColor != Color(iUNDEF))
	{
			pDC->SetTextColor(textColor);
	}
	else
		pDC->SetTextColor(cTCol);

	return br;
}

afx_msg BOOL zFormattedEdit::OnEraseBkgnd( CDC* pDC )
{
	bool fReadOnly = (GetStyle() & ES_READONLY) == ES_READONLY;
	Color cBCol = fReadOnly ? GetSysColor(COLOR_BTNFACE) : GetSysColor(COLOR_WINDOW);
	CRect rct;
	GetClientRect(&rct);
	CBrush brNew(cBCol);
	CPen pNew(PS_SOLID, 1, cBCol);
  CBrush *brOld = pDC->SelectObject(&brNew);
	CPen *cpOld = pDC->SelectObject(&pNew);
	pDC->Rectangle(&rct);
	pDC->SelectObject(brOld);
	pDC->SelectObject(cpOld);

	return TRUE;
}

void zFormattedEdit::SetTextColor(Color clr)
{
	textColor = clr;
}

void zFormattedEdit::SetBackGroundColor(Color clr)
{
	backColor = clr;
}

afx_msg void zFormattedEdit::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags )
{
  if (!format) {
		zEditLine::OnKeyDown(nChar, nRepCnt, nFlags); 
		return;
	}
	switch (nChar)
  {
	  case VK_DELETE:	
    {
		  zRange r(selectionRange());
		  String cur(text());
			if ( cur == String("%e", rUNDEF)) cur="";
			else if ( cur == String("%d", iUNDEF)) cur="";
		  char c = '\b';
		  zFormatter::zFormatterStat fmtAction= format->addChars(cur, &c, 2, r);
		  if (fmtAction == zFormatter::zFmtStatReplace) 
      {
			  text(cur);
			  SetSel(r.lo(), r.hi());
		  } 
      //else if (fmtAction == zFormatterStat::zFmtStatBad) return 1;
		  return;
		}
	}
	zEditLine::OnKeyDown(nChar, nRepCnt, nFlags); 
}

afx_msg void zFormattedEdit::OnChar( UINT nChar, UINT nRepCnt, UINT nFlags )
{
	unsigned int kk= GetKeyState(VK_CONTROL);
  if ((GetKeyState(VK_CONTROL)&0x8000)==0x8000)  // to prevent that Ctrl-C and Ctrl-V do not work
	{
    zEditLine::OnChar(nChar, nRepCnt, nFlags); 
		return;
	}
	if (!format) {
		zEditLine::OnChar(nChar, nRepCnt, nFlags); 
		return;
	}
	char c = nChar; // ?? check this
	if (c == '\r') 
  	return;
  else if (c == 27) 
  {
		GetOwner()->PostMessage(WM_COMMAND, IDCANCEL, 0);
		return ;
	}
	zRange r(selectionRange());
	CString sCur;
	GetWindowText(sCur);
	rawString = sCur;
	String cur(sCur.GetBuffer(sCur.GetLength() + 1));
	zFormatter::zFormatterStat fmtAction= format->addChars(cur, &c, 1, r);
	if (fmtAction == zFormatter::zFmtStatReplace) 
  {
		text(cur);
		SetSel(r.lo(), r.hi());
		return ;
	} 
  else if (fmtAction == zFormatter::zFmtStatBad) return;
	/*else if (fmtAction == zFormatter::zFmtStatSimpleIns)
  {
		ke->wParam = c;
		return zEditLine::ch(ke);
	}*/
	zEditLine::OnChar(nChar, nRepCnt, nFlags); 
}

//afx_msg LRESULT zFormattedEdit::OnCut(WPARAM , LPARAM )
//{
//
//	CEdit::Cut();
//  return 1;
//}
//
afx_msg LRESULT zFormattedEdit::OnPaste(WPARAM , LPARAM )
{
	zClipboard clip(this);
	char *sText = clip.getText(); 
	if (0 == sText)
		return 0;
	CString s(sText);
    rawString = s;
	String txt = s;
	pasteStr(txt);
	delete [] sText;
  
  return 1;
}

String	zFormattedEdit::getRawString() const {
	return rawString;
}

int zFormattedEdit::pasteStr(String &s) 
{
	zRange r(selectionRange());
  if (!format) 
  {
		CString str;
		GetWindowText(str);
		String strBefore(str.Left(r.lo()));
		String strAfter(str.Right(str.GetLength() - r.hi()));
    String strFinal = strBefore + s + strAfter;
		text(strFinal);
		SetSel(r.lo(), strFinal.size() - strAfter.size());
		return 1;
	}
	String cur(text());
	if ( cur == String("%e", rUNDEF)) 
		cur="";
	else if ( cur == String("%d", iUNDEF)) 
		cur="";
	format->addChars(cur, s.sVal(), s.length(), r);
	text(cur);
	return 1;
}

String zFormattedEdit::getRawText(unsigned long options) 
{
  String s(text());
	if (format)	format->getChars(s, options);
	return s;
}

bool zFormattedEdit::checkData() 
{
	String str(text());
	zRange pos(0, 0x7fff);
	if (format && !format->checkChars(str, pos)) 
  {
		SetSel(pos.lo(), pos.hi());
		return false;
	}
	return true;
}

zFormatter* zFormattedEdit::getFormatter() 
{
  return format;  
}

void zFormattedEdit::setFormatter(zFormatter *fmter) 
{ 
  format = fmter; 
}


//------------------------------------------------------------------------------------------------------------
zStringEdit::zStringEdit(FormEntry *f, CWnd *parent, const CRect& rct, int iStyle, int iId, const String *psInit, 
                         unsigned long iFlags, zFormatter *pcPict) :
             zFormattedEdit(f, parent, rct, iStyle, psInit, iId, pcPict)
{
}
//------------------------------------------------------------------------------------------------------------
zFormatter::zFormatter(DWORD mask) :
            flags(mask)
{}







