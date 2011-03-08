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
// BaseTblField.cpp: implementation of the BaseTblField class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Headers\constant.h"
#include "Client\ilwis.h"
#include "Client\Base\Framewin.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\TableWindow\TableDoc.h"
#include "Client\TableWindow\BaseTablePaneView.h"
#include "Client\TableWindow\TablePaneView.h"
#include "Engine\Table\tblview.h"
#include "Client\TableWindow\BaseTblField.h"
#include "Engine\Domain\dmclass.h"
#include "Engine\Domain\Dmvalue.h"
#include "Headers\Hs\Editor.hs"
#include "Client\FormElements\syscolor.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



class EditFieldLine: public CEdit
{
public:
  EditFieldLine(CWnd* pane, BaseTblField* btf, bool fReadOnly=false);
	void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	BOOL PreTranslateMessage(MSG* pMsg);
protected:
	BaseTblField* tblField;
private:

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(EditFieldLine, CEdit)
	//{{AFX_MSG_MAP(EditFieldLine)
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

EditFieldLine::EditFieldLine(CWnd* pane, BaseTblField* btf, bool fReadOnly)
: tblField(btf)
{
	CRect rect;
	rect.SetRectEmpty();
	Create(WS_CHILD|WS_BORDER|ES_AUTOHSCROLL | (fReadOnly ? ES_READONLY : 0), 
		rect, pane, 0);
}

BOOL EditFieldLine::PreTranslateMessage(MSG* pMsg)
{
// prevent that RETURN, ESC, TAB are eaten by IsDialogMessage() check
// but allow other accelerators to function
	if (pMsg->message == WM_KEYDOWN) {
		switch (pMsg->wParam) {
	    case VK_TAB:
		  case VK_ESCAPE: 
			case VK_RETURN:
				::TranslateMessage(pMsg);
				::DispatchMessage(pMsg);
				return TRUE;
		}
	}
	return CEdit::PreTranslateMessage(pMsg);
}

void EditFieldLine::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (!tblField->OnKeyDown(nChar, nRepCnt, nFlags))
		CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
};

void EditFieldLine::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
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
};

class EditFieldBool: public EditFieldLine
{
public:
  EditFieldBool(CWnd* pane, BaseTblField* btf, const DomainBool* db);
	void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
private:
  String sTrue, sFalse;
  char cTrue, cFalse;
	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(EditFieldBool, EditFieldLine)
	//{{AFX_MSG_MAP(EditFieldBool)
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


EditFieldBool::EditFieldBool(CWnd* pane, BaseTblField* btf, const DomainBool* db)
: EditFieldLine(pane, btf)
{
  sTrue = db->sValueByRaw(2,0);
  sFalse = db->sValueByRaw(1,0);
  cTrue = tolower(sTrue[0]);
  cFalse = tolower(sFalse[0]);
}

void EditFieldBool::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    bool fValue;
    char ch = tolower(nChar);
//    if (ch >= 0x41 && ch <= 0x5A)   // tolower does not seem to work
//      ch += 0x20;
    if (ch == cTrue)
      fValue = true;
    else if (ch == cFalse)
      fValue = false;
    else switch (ch)
    {
      case 't':
      case 'y':
      case '1':
      case '+':
        fValue = true;
        break;
      case 'f':
      case 'n':
      case '0':
      case '-':
        fValue = false;
        break;
      case '?':
        SetWindowText("?");
				SetModify();
				SetSel(0,-1);
        return;
      case ' ': {
				CString str;
				GetWindowText(str);
        fValue = 0 != _stricmp(sTrue.sVal(), str);
      } break;    
      default:
				tblField->OnKeyDown(nChar, nRepCnt, nFlags);
				// do not give to parent!
        // EditFieldLine::OnKeyDown(nChar,nRepCnt,nFlags);
				return;
    }
    if (fValue)
      SetWindowText(sTrue.sVal());
    else  
      SetWindowText(sFalse.sVal());
		SetModify();
}

void EditFieldBool::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// do nothing!
};


static char* far sNEW = "< new >";
class EditFieldComboBox: public CComboBox
{
public:
  EditFieldComboBox(CWnd* pane, BaseTblField* btf, const DomainClass* dc);
	void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	void OnCloseUp();
private:
	BaseTblField* tblField;
	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(EditFieldComboBox, CComboBox)
	//{{AFX_MSG_MAP(EditFieldLine)
	ON_WM_KEYDOWN()
	ON_CONTROL_REFLECT(CBN_CLOSEUP, OnCloseUp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

EditFieldComboBox::EditFieldComboBox(CWnd* pane, BaseTblField* btf, const DomainClass* dc)
: tblField(btf)
{
	CRect rect;
	rect.SetRectEmpty();
	Create(WS_CHILD|
	       CBS_HASSTRINGS|WS_VSCROLL|
         CBS_DROPDOWNLIST|WS_BORDER|CBS_AUTOHSCROLL|CBS_NOINTEGRALHEIGHT,
	       rect, pane, 0);
	for (int i = 1; i <= dc->iNettoSize(); ++i)
		AddString(dc->sValueByRaw(dc->iKey(i),0).sVal());
  AddString("?");
  AddString(sNEW);
}

void EditFieldComboBox::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (!tblField->OnKeyDown(nChar, nRepCnt, nFlags))
		CComboBox::OnKeyDown(nChar, nRepCnt, nFlags);
};

void EditFieldComboBox::OnCloseUp()
{
  int id = GetCurSel();
	CString str;
	GetLBText(id, str);
  String s = (const char*)str;
  if (sNEW == s) {
		tblField->OnKeyDown(VK_RETURN,1,0);
	}
}

BaseTblField::BaseTblField(BaseTablePaneView* pane, int col, long row, const FileName& fn)
{
	Domain dom;
	IlwisObject::iotIlwisObjectType type = IlwisObject::iotObjectType(fn);
	if ( type == IlwisObject::iotRASMAP || type == IlwisObject::iotSEGMENTMAP || 
		type == IlwisObject::iotPOINTMAP || type == IlwisObject::iotPOLYGONMAP) {
			BaseMap bmp(fn);
			dom = bmp->dvrs().dm();
	} else if ( type == IlwisObject::iotTABLE){
		Table tbl(fn.sPath() + fn.sFile + fn.sExt);
		dom = tbl->col(fn.sCol)->dm();
	}
	if ( dom.fValid()) {
		DomainClass* dc = dom->pdc();
		DomainBool* db = dom->pdbool();
		if (dc)
			ctrl = new EditFieldComboBox(pane, this, dc);
		else if (db)
			ctrl = new EditFieldBool(pane, this, db);
		else
			ctrl = new EditFieldLine(pane, this);
	} else {
		ctrl = new EditFieldLine(pane, this, true);
	}
	tbpn = pane;
	iCol = col;
	iRow = row;
	FrameHandlesAccelerators(false);
}

BaseTblField::BaseTblField(BaseTablePaneView* pane, int col, long row, const Domain& dom)
  : dm(dom)
{
  DomainClass* dc = dom->pdc();
  DomainBool* db = dom->pdbool();
  if (dc)
    ctrl = new EditFieldComboBox(pane, this, dc);
  else if (db)
    ctrl = new EditFieldBool(pane, this, db);
  else
    ctrl = new EditFieldLine(pane, this);
  tbpn = pane;
  iCol = col;
  iRow = row;
	FrameHandlesAccelerators(false);
}

BaseTblField::BaseTblField(BaseTablePaneView* pane, int col, long row)
{
  ctrl = new EditFieldLine(pane, this);
  tbpn = pane;
  iCol = col;
  iRow = row;
	FrameHandlesAccelerators(false);
}

BaseTblField::BaseTblField(BaseTablePaneView* pane, int col, long row, bool fReadOnly)
{
  ctrl = new EditFieldLine(pane, this, fReadOnly);
  tbpn = pane;
  iCol = col;
  iRow = row;
	FrameHandlesAccelerators(false);
}

BaseTblField::~BaseTblField()
{
	FrameHandlesAccelerators(true);
  delete ctrl;
}

class EditDomItemForm: public FormWithDest
{
public:
  EditDomItemForm(CWnd* wPar, const String& sTitle, String* sName,
                  String* sCode)
  : FormWithDest(wPar, sTitle)
  {
    new FieldBlank(root);
    new FieldString(root, SEDUiName, sName, Domain(), false);
    new FieldString(root, SEDUiCode, sCode, Domain(), true);

//    setHelpItem(htpDomainWindow);
    create();
  }
};

String BaseTblField::sText()
{
  String s;
  EditFieldComboBox* efcb = dynamic_cast<EditFieldComboBox*>(ctrl);
  if (efcb) {
    int id = efcb->GetCurSel();
		CString str;
		efcb->GetLBText(id, str);
    s = (const char*)str;
    if (sNEW == s) {
      String sCode;
      Color clr;
      bool fOk;
      retry:
      {
        s = "";
        EditDomItemForm frm(tbpn, SEDTitleAddItemToDomain,
                            &s, &sCode);
        fOk = frm.fOkClicked();
      }
      if (fOk && s[0]) {
        try {
          long iRaw = dm->pdc()->iAdd(s);
          if (iRaw) {
            if (sCode.length() != 0)
              dm->pdc()->SetCode(iRaw, sCode);
          }
        }
        catch (ErrorObject& err) {
          err.Show();
          goto retry;
        }
      }
    }
  }  
  else {
		CString str;
		ctrl->GetWindowText(str);
    s = (const char*)str;
	}
  return s;
}

BOOL BaseTblField::fHasChanged()
{
  CEdit* eb = dynamic_cast<CEdit*>(ctrl);

	CComboBox *ccb = dynamic_cast<CComboBox*>(ctrl);
	if (ccb && ccb->GetSafeHwnd())
	{
		int id = ccb->GetCurSel();
		if (-1 == id)
			return false;
		CString str;
		ccb->GetLBText(id, str);
    TablePaneView* pane = dynamic_cast<TablePaneView*>(tbpn);	
		if (0 == pane) 
			return true;
		if (String(str) != pane->tvw()->sValue(iCol,iRow))
			return true;
	}

	if (!eb || !IsWindow(eb->m_hWnd))
		return false; 
  else
    return eb->GetModify();
}

void BaseTblField::FrameHandlesAccelerators(bool fYes)
{
  CFrameWnd* fw = tbpn->GetParentFrame();
	FrameWindow* frm = dynamic_cast<FrameWindow*>(fw);
  if (0 == frm) {
    CWnd* wnd = fw->GetTopLevelOwner();
  	frm = dynamic_cast<FrameWindow*>(wnd);
  }
	if (frm)
		frm->HandleAccelerators(fYes);
}

void BaseTblField::init(const String& str)
{
  const char* s = str.scVal();
  while (*s == ' ') ++s;
  EditFieldComboBox* efcb = dynamic_cast<EditFieldComboBox*>(ctrl);
  if (efcb)
    efcb->SelectString(-1,s);
  ctrl->SetWindowText(s);
  CEdit* eb = dynamic_cast<CEdit*>(ctrl);
  if (eb) 
    eb->SetModify(FALSE);
  ctrl->SetFont(IlwWinApp()->GetFont(IlwisWinApp::sfTABLE));
  moveTo(iCol,iRow);
  ctrl->SetFocus();
  if (eb) 
//    eb->selection(-1,0); // unselect evrything
    eb->SetSel(0,-1); // select whole text
}

void BaseTblField::setFocus()
{
  ctrl->SetFocus();
}

void BaseTblField::moveTo(int col, long row)
{
  iCol = col;
  iRow = row;
  if (iRow < tbpn->iFirstVisibleRow()) {
    tbpn->vertPixMove(iRow-tbpn->iFirstVisibleRow());
    return;
  }
  if (iRow > tbpn->iLastVisibleRow()) {
    tbpn->vertPixMove(iRow-tbpn->iLastVisibleRow());
    return;
  }
  if (iCol < tbpn->iFirstVisibleColumn()) {
    tbpn->horzPixMove(iCol-tbpn->iFirstVisibleColumn());
    return;
  }
  if (iCol > tbpn->iLastVisibleColumn()) {
    while (iCol > tbpn->iLastVisibleColumn())
      tbpn->horzPixMove(1);
    return;
  }
  update();
}

void BaseTblField::update()
{
  zRect rect;
  int iW, iH;
  tbpn->GetClientRect(&rect);
  int iWidth = rect.width();
  zPoint p = tbpn->pntField(iCol,iRow);
  iH = (int)(1.5 * tbpn->iHeight() - 1);
  int iDiff = (iH - tbpn->iHeight()) / 2 - 1;
  p.x -= 2;
  p.y -= iDiff;
  if (p == pCurr) return;
  pCurr = p;
  int iRight = tbpn->iColumnPix(iCol+1);
  if (iRight > iWidth)
    iRight = iWidth;
  iW = 4 + iRight - p.x;
  EditFieldComboBox* efcb = dynamic_cast<EditFieldComboBox*>(ctrl);
  if (efcb)
    iH *= 8;
  ctrl->MoveWindow(p.x,p.y,iW,iH);
  if (efcb)
    efcb->ShowDropDown(FALSE); 
	ctrl->Invalidate();
  ctrl->ShowWindow(SW_SHOW);
}

BOOL BaseTblField::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
  bool fGotoField = false;
  int iNewCol = iCol;
  long iNewRow = iRow;
  int iStep = 1;
  bool fCtrl = GetKeyState(VK_CONTROL) & 0x8000 ? true : false;
  bool fShift = GetKeyState(VK_SHIFT) & 0x8000 ? true : false;

  switch (nChar)
  {
    case VK_UP:
      iStep = -1;
      // fall though
    case VK_DOWN:
      if (fCtrl)
				iStep *= 100;
      if (fShift)
				iStep *= 10;
      iNewRow += iStep;
      fGotoField = true;
      break;
    case VK_TAB:
      if (fShift)
				iStep = -1;
      if (fCtrl)
				iStep *= 5;
      iNewCol += iStep;
      fGotoField = true;
      break;
    case VK_ESCAPE: {
      CEdit* eb = dynamic_cast<CEdit*>(ctrl);
      if (eb) 
				eb->SetModify(FALSE);
    } // fall though
    case VK_RETURN:
      iNewCol = -1;
      iNewRow = -1;
      fGotoField = true;
      break;
  }
  if (fGotoField) {
    tbpn->PostMessage(ILW_GOTOFIELD,iNewCol,iNewRow);
    return 1;
  }  
  return 0;
}

