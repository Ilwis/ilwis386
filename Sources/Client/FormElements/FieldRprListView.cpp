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
#pragma warning( disable : 4786 )

#include "Client\Headers\formelementspch.h"
#include "Client\Editors\Utils\InPlaceNameEdit.h"
#include "Client\FormElements\objlist.h"
#include "Engine\Base\File\Directory.h"
#include "Client\FormElements\FieldRprListView.h"
#include "Client\ilwis.h"

BEGIN_MESSAGE_MAP(RprListView, CListCtrl)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	ON_NOTIFY_REFLECT(LVN_BEGINLABELEDIT, OnBeginLabelEdit)
	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnEndLabelEdit)
END_MESSAGE_MAP()

RprListView::RprListView(FormEntry *f, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, map<String, String>& _defRpr ) :
  CListCtrl(),
	BaseZapp(f),
	par(f),
	defRpr(_defRpr)
{
	dwStyle |= LVS_NOSORTHEADER | LVS_EDITLABELS | LVS_SHAREIMAGELISTS| LVS_SINGLESEL;
	CListCtrl::Create(dwStyle, rect, pParentWnd, nID);
	SetExtendedStyle(GetExtendedStyle() | LVS_EX_SUBITEMIMAGES | LVS_EX_FULLROWSELECT );
	SetImageList(&IlwWinApp()->ilSmall, LVSIL_SMALL);

	InsertColumn(0, "Value Domain", LVCFMT_LEFT, 100);
	InsertColumn(1, "Representation", LVCFMT_LEFT, 180);
	
	for(map<String, String>::iterator cur2=defRpr.begin(); cur2 != defRpr.end(); ++cur2)
	{
		pair<String, String> value = (*cur2);
		char sName[MAX_PATH];;
		LVITEM lvi;
		int iCount = GetItemCount();
		int iDomImg = IlwWinApp()->iImage(".dom");
		int iRprImg = IlwWinApp()->iImage(".rpr");
		lvi.iItem = iCount;
		lvi.iSubItem = 0;
		lvi.iImage = iDomImg;
		lvi.mask = LVIF_TEXT | LVIF_IMAGE;
		FileName fn(value.first);
		lvi.pszText = fn.sFile.toLower().sVal();
		InsertItem(&lvi);
		lvi.iItem = iCount;
		lvi.iSubItem = 1;
		lvi.iImage = iRprImg;
		lvi.mask = LVIF_TEXT | LVIF_IMAGE;
		FileName fn2(value.second);
		String sSystemDir = IlwWinApp()->Context()->sStdDir();
		sSystemDir = sSystemDir.sLeft(sSystemDir.size());
		if ( fCIStrEqual(fn2.sPath() , sSystemDir + "\\") )
			lvi.pszText = fn2.sFile.toLower().sVal();
		else
		{
			strcpy(sName, fn2.sFullPath(false).toLower().sVal());
			lvi.pszText = sName;
		}

		SetItem(&lvi);
	}
}

void RprListView::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CPoint pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);
	LVHITTESTINFO hti;
	hti.pt = pt;
	HitTest(&hti);
	
	if (hti.flags & LVHT_ONITEM) 
		EditLabel(hti.iItem);

	*pResult = 0;
}

/*
void RprListView::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	fProcess(Event(WM_NOTIFY, GetDlgCtrlID(), (LPARAM)pNMHDR)); 
}
*/

void RprListView::OnBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LVHITTESTINFO hi;
	*pResult = 0;
	NMLVDISPINFO *pNMLV = (NMLVDISPINFO *)pNMHDR ;

	int iSubItem = 1;  // always edit the representation column
	int iItem = pNMLV->item.iItem;
	HitTest(&hi);
	if (iItem == -1)
		return;

	*pResult = 1;

	CRect rct;
	GetSubItemRect(iItem, 1, LVIR_BOUNDS, rct);

	RepresentationLister *ol = new RepresentationLister(Domain("Value"));
	ne = new InPlaceNameEdit(this, par->frm(), iItem, iSubItem, ol, rct, par->Id());
	
	String sLabelText = String(GetItemText(iItem, iSubItem));
	ne->SetVal(sLabelText);
	ne->SetFocus();
}

void RprListView::OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;

	if (pDispInfo->item.pszText != 0)
	{
		LVITEM lvi;
		char sText[MAX_PATH];

		lvi.iItem = pDispInfo->item.iItem;
		lvi.iSubItem = 0;
		lvi.mask = LVIF_TEXT;
		lvi.pszText = sText;
		lvi.cchTextMax = MAX_PATH;
		GetItem(&lvi);
		String sDom(lvi.pszText);
		lvi.iImage = 	IlwWinApp()->iImage(".rpr");
		lvi.iSubItem = 1;
		lvi.mask = LVIF_TEXT | LVIF_IMAGE;
		String sRpr(pDispInfo->item.pszText);
		FileName fn2(pDispInfo->item.pszText);
		lvi.pszText =  fn2.sFile.toLower().sVal();
		defRpr[sDom] = sRpr;
		SetItem(&lvi);
	}

	*pResult = 0;
}

//-----------------------------------------------------------------------------------
FieldRprListView::FieldRprListView(FormEntry* par, map<String, String> &_rprmap)
  : FormEntry(par,0,true),
	rprList(NULL),
	defRpr(_rprmap)
{
  psn->iMinWidth = psn->iWidth = FLDNAMEWIDTH + 200;
  psn->iMinHeight = psn->iHeight = 200;
	for(map<String, String>::iterator cur=_rprmap.begin(); cur != _rprmap.end(); ++cur)
	{
		String sDom = (*cur).first;
		String sRpr = (*cur).second;
		tempRpr[sDom] = sRpr;
	}
}

void FieldRprListView::create()
{
  zPoint pntFld = zPoint(psn->iPosX,psn->iPosY);
  zDimension dimFld = zDimension(psn->iWidth,psn->iMinHeight);
  rprList = new RprListView(this, WS_VISIBLE | WS_BORDER | WS_CHILD | WS_TABSTOP | LVS_REPORT, 
              CRect(pntFld, dimFld) , frm()->wnd() , Id(), tempRpr);
  rprList->SetFont(frm()->fnt);	
  CreateChildren();
}

bool FieldRprListView::fValid()
{
	if ( !rprList) return false;
	return rprList->GetSafeHwnd() != NULL;
}

FieldRprListView::~FieldRprListView()
{
	delete rprList;
}

void FieldRprListView::show(int sw)
{
  if (rprList->GetSafeHwnd( ))
    rprList->ShowWindow(sw);
}

void FieldRprListView::StoreData()
{
	for(map<String, String>::iterator cur = defRpr.begin(); cur != defRpr.end(); ++cur)
		defRpr[(*cur).first ] = tempRpr [ (*cur).first ];
}

