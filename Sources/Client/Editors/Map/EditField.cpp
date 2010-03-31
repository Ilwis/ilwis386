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
// EditField.cpp: implementation of the EditField class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Engine\Map\Segment\Seg.h"
#include "Engine\Map\Polygon\POL.H"
#include "Client\Mapwindow\Positioner.h"
#include "Client\Mapwindow\Drawers\BaseDrawer.h"
#include "Client\Editors\Editor.h"
#include "Headers\constant.h"
#include "Client\Editors\Map\EditField.h"
#include "Engine\Domain\dmclass.h"
#include "Headers\Hs\Editor.hs"
#include "Client\FormElements\fldcolor.h"
#include "Engine\Representation\Rprclass.h"
#include "Client\Base\Framewin.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(EditField, CEdit)
	//{{AFX_MSG_MAP(EditField)
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	ON_WM_KILLFOCUS()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(EditFieldClass, CListBox)
	//{{AFX_MSG_MAP(EditFieldClass)
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	ON_WM_KILLFOCUS()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


EditField::EditField(Editor* ed, Coord c, const DomainValueRangeStruct& dvs, const String& s)
: edit(ed), crd(c), dvrs(dvs)
{
  edit->pane()->fwParent()->HandleAccelerators(false);

	CRect rect;
	rect.SetRectEmpty();
	CreateEx(0, _T("EDIT"), NULL, WS_POPUP|WS_BORDER|ES_AUTOHSCROLL, rect, edit->pane(), 0);

  zPoint pnt = edit->pane()->pntPos(crd);
  CClientDC cdc(edit->pane());
	TEXTMETRIC tm;
	cdc.GetTextMetrics(&tm);
  int iHght = tm.tmHeight;
	
  if (tm.tmExternalLeading == 0)
    iHght += 1;
  else
    iHght += tm.tmExternalLeading;
  iHght *= 1.5;
  iHght -= 1;
  int iWdth = tm.tmAveCharWidth + cdc.GetTextCharacterExtra();
  if (dvrs.dm()->iWidth() > 8)
    iWdth *= dvrs.dm()->iWidth();
  else
    iWdth *= 8;
  ed->pane()->GetWindowRect(rect);
  pnt.x += 2 + rect.left;
  pnt.y += 2 + rect.top;
  SetWindowText(s.scVal());
  SetSel(0,-1);
	SetWindowPos(&wndTop,pnt.x,pnt.y,iWdth,iHght,SWP_SHOWWINDOW);
  SetFocus();
  edit->StartBusy();
}

EditField::~EditField()  
{
	if ( edit && edit->pane() && edit->pane()->fwParent()) {
		edit->pane()->fwParent()->HandleAccelerators(true);
	}
  edit->EndBusy();
}
									
void EditField::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar) 
	{
    case VK_TAB:
    case VK_ESCAPE: 
    case VK_RETURN:
			break;
		default:
			CEdit::OnChar(nChar, nRepCnt, nFlags);
	}
}

void EditField::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar) 
	{
    case VK_TAB:
    case VK_RETURN:
      {
				try {
					CString str;
					GetWindowText(str);
					String s = (const char*)str;
					if ("?" != s && !dvrs.fValid(s)) 
					{
						DomainSort* ds = dvrs.dm()->pdsrt();
						if (0 != ds) 
							ds->iAdd(s);
						else {
							MessageBeep(MB_ICONASTERISK);
							return;
						}
					}
					// note: EditFieldOK() can show a message which causes the EditField to close and delete itself
					Editor* ed = edit;
          ed->EditFieldOK(crd,s);
		      ed->EndBusy();
					if (IsWindow(m_hWnd))
						PostMessage(WM_CLOSE, 0,0);
					return;
        }
        catch (ErrorObject& err) {
          err.Show();
          return;
        }
      }
    case VK_ESCAPE: 
			if (IsWindow(m_hWnd))
				PostMessage(WM_CLOSE, 0,0);
      edit->EndBusy();
			break;
		default:
			CEdit::OnChar(nChar, nRepCnt, nFlags);
	}
}

void EditField::OnKillFocus(CWnd* wnd)
{
  PostMessage(WM_CLOSE, 0,0);
  edit->EndBusy();
	CEdit::OnKillFocus(wnd);
}

void EditField::OnClose()
{
	edit->ClearEditFieldPointer();
	Default();
	delete this;
}

static char* far sNEW = "< new >"; //SEDRemNew;  // can't initialize before IlwisApp() exists!

EditFieldClass::EditFieldClass(Editor* ed, Coord c, 
  DomainClass* domc, const String& s)
: edit(ed), crd(c), dc(domc), fBusy(false)
{
	zRect rect;
	rect.SetRectEmpty();
	CreateEx(0, _T("LISTBOX"), NULL, WS_POPUP|WS_BORDER, rect, edit->pane(), 0);

  for (int i = 1; i <= dc->iNettoSize(); ++i) {
		String str = dc->sValueByRaw(dc->iKey(i),0);
    AddString(str.sVal());
	}
  AddString("?");
  AddString(sNEW);
  SelectString(-1,s.scVal());
  zPoint pnt = edit->pane()->pntPos(crd);

  CClientDC cdc(edit->pane());
	TEXTMETRIC tm;
	cdc.GetTextMetrics(&tm);
  int iHght = GetItemHeight(0);
  iHght *= min(15, (int)dc->iNettoSize()+3);
  int iWdth = tm.tmAveCharWidth + cdc.GetTextCharacterExtra();
  if (domc->iWidth() > 8)
    iWdth *= domc->iWidth();
  else
    iWdth *= 8;
  ed->pane()->GetWindowRect(rect);
  pnt.x += 2 + rect.left();
  pnt.y += 2 + rect.top();

  CRect rectWorkArea;
  SystemParametersInfo(SPI_GETWORKAREA, 0, &rectWorkArea, 0);
  int iScrHeight = rectWorkArea.bottom; //cdc.GetDeviceCaps(VERTRES);
  int iScrWidth = rectWorkArea.right; //cdc.GetDeviceCaps(HORZRES);
  if (pnt.x + iWdth > iScrWidth)
    pnt.x = iScrWidth - iWdth;
  if (pnt.y + iHght > iScrHeight)
    pnt.y = iScrHeight - iHght;
  MoveWindow(pnt.x,pnt.y,iWdth,iHght);
  ShowWindow(SW_SHOW);
  SetFocus();
  edit->StartBusy();
}

EditFieldClass::~EditFieldClass()
{
  edit->EndBusy();
}

void EditFieldClass::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar) 
	{
    case VK_TAB:
    case VK_ESCAPE: 
    case VK_RETURN:
			break;
		default:
			CListBox::OnChar(nChar, nRepCnt, nFlags);
	}
}

class EditDomItemForm: public FormWithDest
{
public:
  EditDomItemForm(CWnd* wPar, const String& sTitle, String* sName,
                  String* sCode, Color* clr)
  : FormWithDest(wPar, sTitle)
  {
    new FieldBlank(root);
    new FieldString(root, SEDUiName, sName, Domain(), false);
    new FieldString(root, SEDUiCode, sCode, Domain(), true);
    new FieldColor(root, SEDUiColor, clr);
    
//    SetHelpTopic(htpDomainWindow);
    create();
  }
};

void EditFieldClass::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar) 
	{
    case VK_TAB:
    case VK_RETURN:
      {
				int iSel = GetCurSel();
				CString str;
				GetText(iSel, str);
				String s = (const char*)str;
				if (s == sNEW) {
					s = "";
          String sCode;
          Color clr;
					bool fOk;
          start:
					{
						fBusy = true;
            EditDomItemForm frm(edit->pane(), SEDTitleAddItemToDomain, 
                                &s, &sCode, &clr);
            fOk = frm.fOkClicked();
						fBusy = false;
          }  
					if (fOk && s != "") {
            try {
              long iRaw = dc->iAdd(s);
							if (iRaw) {
                if (sCode.length() != 0)
                  dc->SetCode(iRaw, sCode);
                Representation rpr = dc->rpr();
                if (rpr.fValid())
                  rpr->prc()->PutColor(iRaw, clr);
								edit->pane()->CView::GetDocument()->UpdateAllViews(edit->pane(),5); // only update layer view
                s = dc->sNameByRaw(iRaw, 0); // name without potential code
                edit->EditFieldOK(crd,s);
              }
            }
            catch (ErrorObject& err) {
              err.Show(SEDTitleAddItemToDomain);
              goto start;
            }
          }    
				}
				else
          edit->EditFieldOK(crd,s);
      }
      // fall through
    case VK_ESCAPE:
			if (IsWindow(m_hWnd))
	      PostMessage(WM_CLOSE, 0,0);
			break;
		default:
			CListBox::OnChar(nChar, nRepCnt, nFlags);
  }
}

void EditFieldClass::OnLButtonDblClk(UINT nFlags, CPoint point)
{
  PostMessage(WM_KEYDOWN, VK_RETURN, 0L);
}  

void EditFieldClass::OnKillFocus(CWnd* wnd)
{
	if (!fBusy) {
		PostMessage(WM_CLOSE, 0,0);
		edit->EndBusy();
	}
	CListBox::OnKillFocus(wnd);
}

void EditFieldClass::OnClose()
{
	edit->ClearEditFieldPointer();
	Default();
	delete this;
}

