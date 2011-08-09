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
/* NameEdit
   by Wim Koolhoven
   (c) Computer Department ITC
	Last change:  WK   24 Sep 97    4:55 pm
*/
#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\nameedit.h"
#include "Client\FormElements\objlist.h"
#include <afxole.h>
#include "Client\FormElements\ObjectTreeCtrl.h"
#include "Client\ilwis.h"

BEGIN_MESSAGE_MAP(BaseNameEdit, CComboBox)
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	ON_CONTROL_REFLECT(CBN_SELCHANGE, OnSelChange)
	ON_CONTROL_REFLECT(CBN_DBLCLK, OnDblClick)
	ON_CONTROL_REFLECT(CBN_SELENDOK, OnSelEndOk)
	ON_CONTROL_REFLECT(CBN_CLOSEUP, OnCloseUp)
	ON_WM_KEYDOWN()
	ON_MESSAGE(WM_GETDLGCODE, OnGetDlgCode) // overrule standard behaviour.
END_MESSAGE_MAP()
  

BaseNameEdit::BaseNameEdit(FormEntry* fe, FormBase* par, CPoint pos, int id, 
                           bool fExist_, int iWidth, unsigned int iStyle) :
              CComboBox(),
              idw(false),
              parent(par)
{
  unsigned int s=iStyle | WS_TABSTOP | WS_VSCROLL | WS_GROUP | CBS_AUTOHSCROLL | CBS_OWNERDRAWFIXED | CBS_HASSTRINGS;
  CComboBox::Create( s, CRect(pos, zDimension(iWidth, 250)), parent->wnd(), id);
  ilwapp = IlwWinApp()->Context();
  _fe = fe;
  fExist = fExist_;
  fEndOk = false;
}

void BaseNameEdit::MeasureItem( LPMEASUREITEMSTRUCT lpMeasureItemStruct )
{
  zDisplay canvas(this);
  canvas.pushFont(parent->fnt);
  zFontInfo info = zFontInfo(&canvas);
  lpMeasureItemStruct->itemHeight = info.iPixHeight();
  canvas.popFont();
}

void BaseNameEdit::OnKillFocus( CWnd* pNewWnd )
{
  CComboBox::OnKillFocus(pNewWnd);

  if (fExist)
    ShowDropDown(FALSE);
}

long BaseNameEdit::OnGetDlgCode(WPARAM wpar, LPARAM lpar) 
{
  if (fExist)
    if ( wpar == VK_RETURN)
      if (CheckData())
        ShowDropDown(FALSE);
      else
        return DLGC_WANTMESSAGE;

  return CComboBox::OnGetDlgCode();
}

String BaseNameEdit::sName()
{
	CheckData();
	if ( sMap != "") // return fully qualified name map|attrib|col
	{
		return String("%S.%S", sMap, _sName.sHead(".")); //, _sName);
	}
	return _sName;
}

void BaseNameEdit::OnSelChange()
{
//  Invalidate();
  //Notify does not work, so do it our self
    if (_fe && _fe->npChanged()) 
    {
//      if (CheckData())
         ((_fe->cb())->*(_fe->npChanged()))(0);
    }
}

void BaseNameEdit::OnClick()
{
}

void BaseNameEdit::OnDblClick()
{
  if (CheckData())
    ShowDropDown(FALSE);
}

void BaseNameEdit::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags )
{
	CComboBox::OnKeyDown(nChar, nRepCnt, nFlags);
}


BaseNameEdit::~BaseNameEdit()
{
  //parent->removeNotify(this, WM_WINDOWPOSCHANGING);
}

void BaseNameEdit::DrawItem(DRAWITEMSTRUCT* dis)
{
  int id = dis->itemID;
  CString s;
  if (id != -1)
		GetLBText(id, s);
  bool fSel = GetCurSel() == id;
  idw.DrawItem(dis, String(s), fSel, true, false);
}

void BaseNameEdit::SetVal(const String& sVal)
{
  SelectString(-1, sVal.c_str());
}

void BaseNameEdit::OnSelEndOk()
{
  fEndOk = true;
}

void BaseNameEdit::OnCloseUp()
{
  if (!fEndOk) 
  {
    _sName = "";
  }
}

void BaseNameEdit::OnSetFocus(CWnd *pOldWin)
{
  CComboBox::OnSetFocus(pOldWin);
//  ilwapp->setHelpItem(_fe->htp());
}

//----[ NameEditDropTarget ]---------------------------------------------------------------------------------------------------

class NameEditDropTarget: public COleDropTarget
{
public:
	NameEditDropTarget(NameEdit* nameedit)
	: ne(nameedit)
	{
		Register(ne);
	}
	DROPEFFECT OnDragEnter(CWnd*, COleDataObject* pDataObject, DWORD, CPoint)
	{
		FileName fn;
		if (fGetFileName(pDataObject, fn) && ne->OnDrag(fn))
			return DROPEFFECT_COPY; 
		else
			return DROPEFFECT_NONE;
	}
	DROPEFFECT OnDragOver(CWnd*, COleDataObject* pDataObject, DWORD, CPoint)
	{
		FileName fn;
		if (fGetFileName(pDataObject, fn) && ne->OnDrag(fn))
			return DROPEFFECT_COPY; 
		else
			return DROPEFFECT_NONE;
	}
	void OnDragLeave(CWnd*)
	{
	}
	BOOL OnDrop(CWnd*, COleDataObject* pDataObject, DROPEFFECT, CPoint)
	{
		FileName fn;
		if (fGetFileName(pDataObject, fn) && ne->OnDrop(fn))
			return TRUE; 
		else
			return FALSE;
	}
private:
	bool fGetFileName(COleDataObject* pDataObject, FileName& fn) 
	{
		if (!pDataObject->IsDataAvailable(CF_HDROP))
			return false;
		HGLOBAL hnd = pDataObject->GetGlobalData(CF_HDROP);
		HDROP hDrop = (HDROP)GlobalLock(hnd);
		int iFiles = DragQueryFile(hDrop,	(UINT)-1, NULL, 0);
		bool fRet = false;
		if (1 == iFiles) {
			char sFileName[MAX_PATH+1];
			DragQueryFile(hDrop, 0, sFileName, MAX_PATH+1);
			fn = FileName(sFileName);
			fRet = true;
		}
		GlobalUnlock(hDrop);
		GlobalFree(hnd);
		return fRet;
	}
	NameEdit* ne;
};


//----[ NameEdit ]---------------------------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(NameEdit, BaseNameEdit)
//  ON_CONTROL_REFLECT(CBN_DROPDOWN, OnDropDown)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_KEYDOWN()
	ON_WM_SYSKEYDOWN()
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

NameEdit::NameEdit(FormEntry* fe, FormBase* parent, CPoint pos, int id,
           const String& s, bool fExist_, ObjectLister* objl, bool fExt_,
           int iWidth, bool full)
  : BaseNameEdit(fe,parent,pos,id,fExist_,iWidth, CBS_DROPDOWNLIST | CBS_SORT),
	  dmTypes(0), otc(0), iMinTreeWidth(0), fullTree(full)
{
	ol = objl;
	ol->ne = this;
	fExt = fExt_;
	strcpy(sDir, IlwWinApp()->sGetCurDir().c_str());  // sDir includes closing backslash
	if ( FileName(s).sFile == "")  {//  folder path 
		strcpy(sDir,s.c_str());
		FillDir();
	}
	else {
		FileName fn = IlwisObjectPtr::fnCheckPath(s);
		if (s.length() != 0)
		{
			String sFileDir = fn.sPath();
			if ("" != sFileDir) {
				String sStdDir = IlwWinApp()->Context()->sStdDir();
				if (sStdDir[sStdDir.length() - 1] != '\\')
					sStdDir &= "\\";
				if (sFileDir != sStdDir)
					strcpy(sDir, String("%S%S", fn.sDrive, fn.sDir).c_str());
			}
		}
		FillDir();
		if ( ol->fOK(fn.sFullPath(), sCHECK_OBJECT_ONLY) )
		{
			String sSel = objl->sDefaultSelectedValue(fn);
			SetVal(sSel);
	//		AddString(sSel.c_str());
	//		SelectString(-1, sSel.c_str());
		}
	}
	nedt = new NameEditDropTarget(this);


	otc = new ObjectTreeCtrl(this);

	ShowWindow(SW_SHOW);
}

NameEdit::~NameEdit()
{
	delete nedt;
	delete ol;
	delete otc;
}

void NameEdit::OnContextMenu( CWnd* pWnd, CPoint point )
{
	FileName fn(_sName);
	IlwWinApp()->ShowPopupMenu(this, point, fn);
}

void NameEdit::addDir(int f,char* s) 
{
  SendMessage(CB_DIR,f,(long)(char*)s);
}

void NameEdit::SetAttribColumnDomainTypes(long dmT)
{
	dmTypes = dmT;
}

void NameEdit::SetObjLister(ObjectLister* objl)  // deletes old one!
{
  delete ol;
  ol = objl;
  ol->ne = this;
  FillDir();
}

void NameEdit::FillDir()
{
//  ol->FillDir();  // removed, bacuase the ObjectTreeCtrl takes care of the reading of the objects
}

void NameEdit::SetFont(CFont* pFont, BOOL bRedraw)
{
	otc->SetFont(pFont, bRedraw);
	BaseNameEdit::SetFont(pFont, bRedraw);

    CDC pdc; 
    pdc.CreateCompatibleDC(0);

	CFont *oldFont = pdc.SelectObject(pFont);
	CSize cs = pdc.GetTextExtent(CString("gh"));
	pdc.SelectObject(oldFont);

	SetItemHeight(-1, cs.cy + 3);
}

void NameEdit::SetVal(const String& sVal)
{
	if (sVal.length() == 0)
	{
		SetCurSel(-1);
		return;
	}
	String sV = sVal;
	if ( sVal.find('|') != -1) // fully qualified map|attribtable|column
	{
		Array<String> parts;
		Split(sVal, parts, "|");
		sV = parts[2];
		sMap = parts[0];
		sAttrib = parts[1];
		AddString(sV.c_str());
		SelectString(-1, sV.c_str());
		if (_fe && _fe->npChanged())
		{
			if (CheckData())
				((_fe->cb())->*(_fe->npChanged()))(0);
		}
		return;
	}
	else
		sMap = String();
	
	FileName fnVal(sV);
	SetVal(fnVal);
}

void NameEdit::SetVal(const FileName& fnVal)
{
	sMap = String();
	String sNewDir("%S%S", fnVal.sDrive, fnVal.sDir); // sNewDir includes closing backslash
	String sStdDir = IlwWinApp()->Context()->sStdDir();
	int iLen = sStdDir.length() - 1;
	if (sStdDir[iLen] != '\\')
		sStdDir &= '\\';
	if (!fCIStrEqual(sNewDir, sStdDir))
	{
		strcpy(sDir, sNewDir.c_str()); // sDir includes closing backslash
		ShowDropDown(FALSE);
		FillDir();
	}
	String s("%S%S%S", fnVal.sFile, fnVal.sExt, fnVal.sSectionPostFix);
	// Make sure the selected string is in the listbox of the combobox to be able to select it
	if (FindStringExact(-1, s.c_str()) == LB_ERR)
		AddString(s.c_str());
	SelectString(-1, s.c_str());  // display the selected object in the combobox edit control

	if (_fe && _fe->npChanged())
	{
		if (CheckData())
			((_fe->cb())->*(_fe->npChanged()))(0);
	}
}

BOOL NameEdit::OnDrag(const FileName& fn)
{
  return ol->fOK(fn);
}

BOOL NameEdit::OnDrop(const FileName& fn)
{
	String sName = fn.sFullPath();
  SetVal(sName);
  return TRUE;
}

int NameEdit::CheckData()
{
	fEndOk = false;
	SetCurrentDirectory(sDir);
	int id = GetCurSel();
	//CString s1 = CString(_sName.sVal());
	String sx =FileName(_sName.c_str()).sFullPath();
	CString s1 = CString(sx.sVal());
	_sName = "";
	if (id >= 0)
		GetLBText(id, s1);
	String s(s1);
	if (s != "") 
	{
		if ( sMap == "")
		{
			FileName fn(s);
			fn.Dir(sDir);
			if (!File::fExist(fn)) 
			{
				String sStd = IlwWinApp()->Context()->svl()->sGet("IlwStdDir");
				fn.Dir(sStd);
				if (!File::fExist(fn))
					fn.Dir(sDir);  // if not in system dir then use local dir
			}
			_sName = fn.sFullPathQuoted(fExt);
		}
		else
		{
			_sName = s;
		}
	}  
	String sCurDir = IlwWinApp()->sGetCurDir();
	SetCurrentDirectory(sCurDir.c_str());
	return 1;
}

void NameEdit::OnDropDown()
{
	TRACE("DROP DOWN\n");
}

void NameEdit::OnLButtonDown(UINT, CPoint)
{
	if (otc->IsWindowVisible())
		HideTreeCtrl();
	else 
		OpenTreeCtrl();
}

void NameEdit::OnLButtonDblClk(UINT, CPoint)
{
	OpenTreeCtrl();
}

void NameEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar) 
	{
		case VK_F4:
			OpenTreeCtrl();
			return;
		default:
			; // nothing
	}

	BaseNameEdit::OnKeyDown(nChar, nRepCnt,nFlags);
}

void NameEdit::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar) 
	{
		case VK_DOWN:	
			OpenTreeCtrl();
			return;
		default:
			; // nothing
	}

	BaseNameEdit::OnSysKeyDown(nChar, nRepCnt,nFlags);
}

void NameEdit::OpenTreeCtrl()
{
	int iTime = GetTickCount();
	if (abs(otc->iHideTime - iTime) < 100) {
		SetFocus();
		return;
	}

	CRect rect;
	GetWindowRect(rect); 
	rect.top = rect.bottom + 1;
	rect.bottom += 250;
	rect.right = max(rect.right + 30, rect.left + iMinTreeWidth);
	otc->SetWindowPos(&wndTop,rect.left,rect.top,rect.Width(),rect.Height(),SWP_SHOWWINDOW);
}

void NameEdit::HideTreeCtrl()
{
	otc->ShowWindow(SW_HIDE);
}

void NameEdit::ResetObjectTreeCtrl()
{
	if (otc)
		otc->Reset();
}

void NameEdit::SelectAttribColumn(const FileName& fnMapWithCol)
{
	// Pre-conditions:
	// 1) fnMapWithCol has an attribute column specified in sCol
	// 2) The files up to the given file are already available in the tree control
	// (call SetVal(FileName(fnMapWithCol)); OpenTreeCtrl(); to achieve this
	// This function will then make sure the attribute table is expanded and sCol is selected
	// it also makes sure the ComboBox selection is the one that would be there if the user
	// had just selected sCol with the mouse
	if (otc)
	{
		// first restore the selection in the NameEdit in case the user cancels
		FileName fnMap (fnMapWithCol); // remove sCol .. fnAttributeTable can't work with it
		FileName fnAttrib = ObjectInfo::fnAttributeTable(fnMap);
		String sName ("%S|%S|%S.clm", fnMap.sFullPath(), fnAttrib.sFullPath(false), fnMapWithCol.sCol);
		SetVal(sName);

		otc->SelectAttribColumn(fnMapWithCol);
	}
}

void NameEdit::DrawItem(DRAWITEMSTRUCT* dis)
{
  int id = dis->itemID;
  CString s;
  if (id != -1)
		GetLBText(id, s);
  String ss = s;
	FileName fn(ss);
	String str;
	if ("" == fn.sSectionPostFix)
		str = String(s);
	else {
		fn.sExt = ".mpr";
		str = String("%S%S%S", fn.sFile, fn.sSectionPostFix, fn.sExt);
	}
  bool fSel = GetCurSel() == id;
  idw.DrawItem(dis, str, fSel, true, false);
}

void NameEdit::SetMinTreeWidth(int iWidth)
{
	iMinTreeWidth = iWidth;
}

//---------[ ColumnNameEdit ]--------------------------------------------------------------------
ColumnNameEdit::ColumnNameEdit(FormEntry* fe, FormBase* parent,
                               zPoint pos, int id, const String& s,
                               Table* tbl, int iWidth)
  : BaseNameEdit(fe,parent,pos,id,true,iWidth, CBS_DROPDOWNLIST)
{
  String sCol;
  for (int i = 0; i < (*tbl)->iCols(); ++i) {
    sCol = (*tbl)->col(i)->sName();
    sCol &= ".clm";
    AddString(sCol.sVal());
  }
  sCol = s;
  sCol &= ".clm";
  SelectString(-1, sCol.sVal());
}

int ColumnNameEdit::CheckData()
{
  fEndOk = false;
  int id = GetCurSel();
  String sCol = _sName.sLeft(_sName.length()-4).sUnQuote(); // strip quotes from column name
  CString s = sCol.sVal();
  if (id >= 0)
    GetLBText(id, s);
  else
    GetWindowText(s);
  if (s == "")
    return 0;
  _sName = String(s.Left(s.GetLength()-4)).sQuote();  // strip ".clm" and add quotes
  return 1;
}




