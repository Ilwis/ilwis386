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
#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\TreeSelector.h"

BEGIN_MESSAGE_MAP(Tree, CTreeCtrl)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelchanged)
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDING, OnItemExpanding)
END_MESSAGE_MAP()

Tree::Tree(FormEntry *f, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID ) :
  CTreeCtrl(),
	BaseZapp(f)
{
	CTreeCtrl::Create(dwStyle, rect, pParentWnd, nID);
}

void Tree::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
}

void Tree::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	//fProcess(Event(WM_NOTIFY, GetDlgCtrlID(), (LPARAM)pNMHDR)); 
	fProcess(Event(TVN_SELCHANGED, GetDlgCtrlID(), (LPARAM)pNMHDR)); 
}

void Tree::OnItemExpanding(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMTREEVIEW* nmtv = (NMTREEVIEW*)pNMHDR;
	HTREEITEM hti = nmtv->itemNew.hItem;
	fProcess(Event(TVN_ITEMEXPANDING, GetDlgCtrlID(), (LPARAM)pNMHDR)); 
}

//-----------------------------------------------------------------------------------
TreeSelector::TreeSelector(FormEntry* par)
  : FormEntry(par,0,true),
	tree(NULL),
	keepSelection(true)
{
  psn->iMinWidth = psn->iWidth = FLDNAMEWIDTH + 100;
  psn->iMinHeight = psn->iHeight = 250;
  npExpansion = NULL;
  alternativeHandler = NULL;
  fDynamic = false;
}

void TreeSelector::create()
{
  zPoint pntFld = zPoint(psn->iPosX,psn->iPosY);
  zDimension dimFld = zDimension(psn->iWidth,psn->iMinHeight);
  
  DWORD style = WS_VISIBLE | WS_BORDER | WS_CHILD | WS_TABSTOP | TVS_DISABLEDRAGDROP | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_HASLINES;
  if ( keepSelection)
		style |= TVS_SHOWSELALWAYS;
	tree = new Tree(this, style,
			  CRect(pntFld, dimFld) , frm()->wnd() , Id());
  tree->SetFont(frm()->fnt);
   if ( alternativeHandler) {
	    if ( npExpansion)  tree->setNotify(alternativeHandler, npExpansion, Message(TVN_ITEMEXPANDING));
  }
  else {
      if ( npExpansion)  tree->setNotify(_frm->wnd(), npExpansion, Message(TVN_ITEMEXPANDING));
  }
  CreateChildren();
}

bool TreeSelector::fValid()
{
	if ( !tree) return false;
	return tree->GetSafeHwnd() != NULL;
}

TreeSelector::~TreeSelector()
{
	delete tree;
}

void TreeSelector::show(int sw)
{
  if (tree->GetSafeHwnd( ))
    tree->ShowWindow(sw);
}

void TreeSelector::expand(HTREEITEM hti, int depth, int maxDepth) {
	if (depth == maxDepth)
		return;
	while(hti) {
		tree->Expand(hti,TVE_EXPAND);
		HTREEITEM htiChild = tree->GetNextItem(hti, TVGN_CHILD);
		expand(htiChild, depth + 1, maxDepth);
		hti = tree->GetNextSiblingItem(hti);
	}
}

void TreeSelector::SetNotifyExpansion(NotifyProc np, FormEntry* _alternativeHandler) {
	npExpansion = np;
	alternativeHandler = _alternativeHandler;
}

void TreeSelector::Add(const String& sValue, HTREEITEM hti, DWORD data, bool fLeaf, int depth)
{
	if ( !tree || sValue == "") return;
	HTREEITEM it = tree->GetNextItem(hti, TVGN_CHILD);
	String sCurTxt = sValue.sHead("#");
	fLeaf = sValue.sTail("#")=="";
	while(it != NULL)
	{
		String s = String(tree->GetItemText(it));
		if ( sCurTxt == s )
		{
			TVITEM tvi;
			memset(&tvi, 0, sizeof(TVITEM));
			tvi.hItem = it;
			tvi.mask = TVIF_PARAM | TVIF_HANDLE | TVIF_CHILDREN; 
			tree->GetItem(&tvi);
			if ( tvi.cChildren == 0 && !fLeaf) {
				tvi.cChildren = 1;
				tree->SetItem(&tvi);
			}
			Add(sValue.sTail("#"), it, data, fLeaf, depth + 1);
			return;
		}				
		it = tree->GetNextItem(it, TVGN_NEXT);
	}
	TVITEM tvi;
	memset(&tvi, 0, sizeof(TVITEM));
	tree->GetItem(&tvi);
	tvi.mask = TVIF_CHILDREN;
	//tvi.cChildren = fDynamic && !fLeaf ? 1 : 0;
	tvi.cChildren = !fLeaf ? 1 : 0;

	tvi.hItem = it = tree->InsertItem(sValue.sHead("#").c_str(), -1,-1, hti);
	tree->SetItemData(it, (DWORD) data);
	Add(sValue.sTail("#"), it, data, fLeaf, depth + 1);
}

String TreeSelector::sBranchValue(HTREEITEM _it)
{
	String sValue;
	HTREEITEM it = _it == 0 ? tree->GetSelectedItem() : _it;
	while( it != NULL)
	{
		sValue = String(tree->GetItemText(it)) + (sValue!= "" ? "#" + sValue : "");
		 it = tree->GetNextItem(it, TVGN_PARENT);
	}
	return sValue;
}

DWORD TreeSelector::GetBranchValueData() {
	HTREEITEM it = tree->GetSelectedItem();
	if ( it)
		return GetData(it);

	return 0;
}

String TreeSelector::sLeafValue()
{
	HTREEITEM it = tree->GetSelectedItem();
	if ( !it) return "";
	return String(tree->GetItemText(it));
}

DWORD  TreeSelector::GetData(HTREEITEM hti) {
	return tree->GetItemData(hti);
}

void TreeSelector::SelectNode(const String& path,HTREEITEM hti){
	if ( !tree || path == "") return;
	HTREEITEM it = tree->GetNextItem(hti, TVGN_CHILD);
	String sCurTxt = path.sHead("#");
	while(it != NULL)
	{
		String s = String(tree->GetItemText(it));
		if ( sCurTxt == s && sCurTxt.size() == path.size())
		{
			tree->SelectItem(it);
			return;
		} if ( sCurTxt == s) {
			SelectNode(path.sTail("#"), it);
			break;
		}
		else
			it = tree->GetNextItem(it, TVGN_NEXT);
	}

}
void TreeSelector::ExpandTo(int depth){
	expand(tree->GetRootItem(), 0, depth);
}