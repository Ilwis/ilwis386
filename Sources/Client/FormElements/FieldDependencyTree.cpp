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
// FieldDependencyTree.cpp: implementation of the FieldDependencyTree class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\FieldDependencyTree.h"
#include "Client\ilwis.h"
#include "Headers\constant.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

class DependencyTreeCtrl: public CTreeCtrl
{
public:
	DependencyTreeCtrl(const RECT& rect, CWnd* wnd, UINT id, const IlwisObject& obj);
private:
	void Expand(HTREEITEM hti, const FileName& fn);
	void OnItemExpanding(NMHDR* pNMHDR, LRESULT* pResult);
	void OnContextMenu(CWnd* pWnd, CPoint point);
	void OnLButtonDblClk(UINT nFlags, CPoint point);
	void OnRButtonDown(UINT nFlags, CPoint point);
	Array<FileName> afn;
	HTREEITEM htiRoot;
 	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(DependencyTreeCtrl, CTreeCtrl)
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDING, OnItemExpanding)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()


DependencyTreeCtrl::DependencyTreeCtrl(const RECT& rect, CWnd* wnd, UINT id, const IlwisObject& obj)
{
	Create(WS_CHILD|WS_VISIBLE|WS_BORDER|TVS_HASLINES|TVS_HASBUTTONS, 
		rect, wnd, id);
	SetImageList(&IlwWinApp()->ilSmall, TVSIL_NORMAL);
	FileName fn = obj->fnObj;
	int iImg = IlwWinApp()->iImage(fn.sExt);
	htiRoot = InsertItem(fn.sFile.c_str(), iImg, iImg, TVI_ROOT);
	afn &= fn;
	SetItemData(htiRoot, afn.iSize());
	Expand(htiRoot, fn);
	CTreeCtrl::Expand(htiRoot, TVE_EXPAND);
}

bool fSplitTblCol(String& sCol, FileName& fnTbl, String& sColName)
{
	char* ps = sCol.sVal();
	bool fIsCol = (0 == strncmp(ps, "Column ", 7));

	sColName = String();
	if (fIsCol) // skip Column prefix
		ps = ps + 7;
	else
		return fIsCol;

	String s = ps;
	char* p = s.strrchrQuoted('.');
	if (p != 0)
	{
		int iTblNameSize = p - (char*)&s[0];
		fnTbl = FileName(s.sLeft(iTblNameSize), ".tbt", false);
		sColName = p + 1;
	}
	else
		sColName = s;

	return fIsCol;
}

void DependencyTreeCtrl::Expand(HTREEITEM hti, const FileName& fn)
{
	TVITEM tvi;
	tvi.mask = TVIF_CHILDREN;
	Array<String> as;
	ObjectDependency::ReadNames(fn, as);
	if (0 == as.iSize()) {
		tvi.cChildren = 0;
		tvi.hItem = hti;
		SetItem(&tvi);
		return;
	}
	for (int i = 0; i < as.iSize(); ++i)
	{
		FileName fn(as[i]);
		String sCol;
		String sImg = fn.sExt;
		String sItem = as[i];
		if (fSplitTblCol(as[i], fn, sCol))
			sImg = "column";
		else
			sItem = fn.sFile;

		int iImg = IlwWinApp()->iImage(sImg);
		tvi.hItem = InsertItem(sItem.c_str(), iImg, iImg, hti);
		tvi.cChildren = ObjectInfo::fDependent(fn) ? 1 : 0;
		SetItem(&tvi);
		afn &= fn;
		SetItemData(tvi.hItem, afn.iSize());
	}
	SortChildren(hti);
	int iNr = GetItemData(hti);
	iNr = -abs(iNr);
	SetItemData(hti, iNr);
}

void DependencyTreeCtrl::OnItemExpanding(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMTREEVIEW* nmtv = (NMTREEVIEW*)pNMHDR;
	HTREEITEM hti = nmtv->itemNew.hItem;
	*pResult = 0;
	if (hti == htiRoot) {
		if (nmtv->action & TVE_COLLAPSE)  // refuse to collapse!
			*pResult = TRUE;
		return;
	}
	int iNr = GetItemData(hti);
	if (iNr > 0) {
		try {
			Expand(hti, afn[iNr-1]);
		}
		catch (ErrorObject& err) {
			err.Show();
			TVITEM tvi;
			tvi.mask = TVIF_CHILDREN;
			tvi.cChildren = 0;
			tvi.hItem = hti;
			SetItem(&tvi);
		}
	}
}

void DependencyTreeCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	HTREEITEM hti = HitTest(point);
	if (hti == 0)
		return;
	int iNr = GetItemData(hti);
	FileName fn = afn[abs(iNr)-1];
	String sCmd("prop %S", fn.sFullPathQuoted());
	IlwWinApp()->Execute(sCmd);
}

void DependencyTreeCtrl::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	CPoint pt = point;
	ScreenToClient(&pt);
	HTREEITEM hti = HitTest(pt);
	Select(hti, TVGN_CARET);
	SetFocus();
	if (hti == 0)
		return;
	int iNr = GetItemData(hti);
	FileName fn = afn[abs(iNr)-1];
	FileName fnReal = IlwisObjectPtr::fnCheckPath(fn);
	IlwWinApp()->ShowPopupMenu(pWnd, point, fnReal, 0, ID_CAT_PROP);
}

void DependencyTreeCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{ // just here to prevent that default implementation 
	// prevents a WM_CONTEXTMENU to be sent
}

FieldDependencyTree::FieldDependencyTree(FormEntry* fe, const IlwisObject& object)
: FormEntry(fe, 0, true), obj(object), dtc(0)
{
	psn->iMinWidth = 200;
	psn->iHeight = psn->iMinHeight = 180;
}

FieldDependencyTree::~FieldDependencyTree()
{
	if (dtc)
		delete dtc;
}

void FieldDependencyTree::show(int sw)
{
	dtc->ShowWindow(sw);
}

void FieldDependencyTree::SetFocus()
{
	dtc->SetFocus();
}

void FieldDependencyTree::create()
{
	CPoint pntFld = CPoint(psn->iPosX, psn->iPosY);
	zDimension dimFld = zDimension(psn->iMinWidth, psn->iMinHeight);
	dtc = new DependencyTreeCtrl(CRect(pntFld, dimFld), _frm->wnd(), Id(), obj);
	dtc->SetFont(frm()->fnt);
}														

