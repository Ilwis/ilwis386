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
// LayerTreeItem.cpp: implementation of the LayerTreeItem class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Client\ilwis.h"
#include "Client\Base\datawind.h"
#include "Client\Mapwindow\MapWindow.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\MainWindow\Catalog\CatalogDocument.h"
#include <afxole.h>
#include "Engine\Map\Mapview.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Headers\Hs\Drwforms.hs"
#include "Client\FormElements\fldcol.h"
#include "Client\FormElements\fldrpr.h"
#include "Client\FormElements\fldcolor.h"
#include "Client\Mapwindow\Drawers\drawer_n.h"
#include "Client\Mapwindow\Drawers\AbstractMapDrawer.h"
#include "Client\Mapwindow\LayerTreeItem.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Headers\constant.h"
#include "Client\FormElements\syscolor.h"
#include "Engine\Domain\Dmvalue.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


#define sMen(ID) ILWSF("men",ID).scVal()
#define pmadd(ID) men.AppendMenu(MF_STRING, ID, sMen(ID)); 

using namespace ILWIS;


//////////////////////////////////////////////////////////////////////
// LayerTreeItem
//////////////////////////////////////////////////////////////////////

LayerTreeItem::LayerTreeItem(LayerTreeView* ltview) 
{
	ltv = ltview;
}

LayerTreeItem::~LayerTreeItem()
{
}

void LayerTreeItem::OnLButtonDown(UINT nFlags, CPoint point)
{
}

void LayerTreeItem::OnLButtonDblClk(UINT nFlags, CPoint point)
{
}

void LayerTreeItem::SwitchCheckBox(bool fOn)
{
}

void LayerTreeItem::OnContextMenu(CWnd* pWnd, CPoint pos)
{
}

void LayerTreeItem::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = CDRF_DODEFAULT;
}

//////////////////////////////////////////////////////////////////////
// DrawerLayerTreeItem
//////////////////////////////////////////////////////////////////////

DrawerLayerTreeItem::DrawerLayerTreeItem(LayerTreeView* ltv, NewDrawer* drw)
: LayerTreeItem(ltv)
{
	dr = drw;
	eText = eNAME;
}

DrawerLayerTreeItem::~DrawerLayerTreeItem()
{
}

void DrawerLayerTreeItem::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	//HTREEITEM hItem = htiStart;
	//HTREEITEM hStop= ltv->GetTreeCtrl().GetNextItem(hItem, TVGN_CHILD);

	//while (hItem != NULL && hItem != hStop)
	//{
	//	ltv->GetTreeCtrl().Expand(hItem,TVE_EXPAND);
	//	hItem= ltv->GetTreeCtrl().GetNextItem(hItem, TVGN_NEXTVISIBLE);
	//}
}

void DrawerLayerTreeItem::SwitchCheckBox(bool fOn)
{
	dr->setActive(fOn);
	MapCompositionDoc* doc = ltv->GetDocument();
	doc->ChangeState();
	doc->UpdateAllViews(ltv,0);
}

void DrawerLayerTreeItem::OnContextMenu(CWnd* w, CPoint p)
{
	AbstractMapDrawer *mapdrw = dynamic_cast<AbstractMapDrawer *>(dr);
	if (!mapdrw)
		return;
	int types = ComplexDrawer::dtMAIN | ComplexDrawer::dtPOST | ComplexDrawer::dtPRE;
	BaseMapPtr *mptr = mapdrw->getBaseMap();
	CMenu men;
	men.CreatePopupMenu();
	pmadd(ID_LAYEROPTIONS);
	men.SetDefaultItem(ID_LAYEROPTIONS);
	if (dr->isEditable()) {
		if (mapdrw->getDrawerCount(types) == 1 && 
			mapdrw->getDrawer(0)->isEditable()) {
				pmadd(ID_EDITLAYER);
		}
	}
	if (mapdrw)
		pmadd(ID_PROPLAYER);
	pmadd(ID_REMOVELAYER);
	int iCmd = men.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON|TPM_NONOTIFY|TPM_RETURNCMD, p.x, p.y, w);
	switch (iCmd) 
	{
	case ID_EDITLAYER:
		{
			if( mapdrw->getDrawerCount(types) == 1) {
				ComplexDrawer *drw = (ComplexDrawer *)mapdrw->getDrawer(0);
				mapdrw->setEditMode(true);
				if ( !drw->isSimple()) {
					drw->setEditMode(true);
					ltv->GetDocument()->mpvGetView()->createEditor(drw);
				}
			}
		}
		break;
	case ID_PROPLAYER:
		IlwWinApp()->Execute(String("prop %S", mptr->fnObj.sFullNameQuoted()));
		break;
	case ID_REMOVELAYER:
		ltv->OnRemoveLayer();
		break;
	}
}

void DrawerLayerTreeItem::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTVCUSTOMDRAW lptvcd = (LPNMTVCUSTOMDRAW) pNMHDR;
	switch (lptvcd->nmcd.dwDrawStage) 
	{
	case CDDS_ITEMPREPAINT:
		*pResult = CDRF_NOTIFYPOSTPAINT;
		return;
	case CDDS_ITEMPOSTPAINT:
		*pResult = CDRF_DODEFAULT;
		if (dr->hasInfo())
		{
			int	iImgOvlInfo =	IlwWinApp()->iImage("OverlayInfo");
			CDC cdc;
			cdc.Attach(lptvcd->nmcd.hdc);
			CRect rect = lptvcd->nmcd.rc;
			CPoint pt = rect.TopLeft();
			pt.x += 3;
			IlwWinApp()->ilSmall.Draw(&cdc, iImgOvlInfo, pt, ILD_TRANSPARENT); 				
			cdc.Detach();
		} 
		return;
	}
}

//////////////////////////////////////////////////////////////////////
// ObjectLayerTreeItem
//////////////////////////////////////////////////////////////////////

ObjectLayerTreeItem::ObjectLayerTreeItem(LayerTreeView* ltv, IlwisObjectPtr* obj) 
: LayerTreeItem(ltv)
{
	ptr = obj;
}

ObjectLayerTreeItem::~ObjectLayerTreeItem()
{
}

void ObjectLayerTreeItem::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if (0 == ptr)
		return;

	if (ptr->fnObj.fValid())
	{
		String sName = ptr->fnObj.sFullPathQuoted();
		IlwWinApp()->OpenDocumentFile(sName.scVal());
	}
}

void ObjectLayerTreeItem::OnContextMenu(CWnd* w, CPoint p)
{
	if (0 != ptr)
		if (ptr->fnObj.fValid())
			IlwWinApp()->ShowPopupMenu(w, p, ptr->fnObj);
}

//////////////////////////////////////////////////////////////////////
// LegendLayerTreeItem
//////////////////////////////////////////////////////////////////////

LegendLayerTreeItem::LegendLayerTreeItem(LayerTreeView* ltv, NewDrawer* drw) 
: LayerTreeItem(ltv)
{
	dr = drw;
}

LegendLayerTreeItem::~LegendLayerTreeItem()
{
}

void LegendLayerTreeItem::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	//	dr->EditRepresentation();
}

void LegendLayerTreeItem::OnContextMenu(CWnd* w, CPoint p)
{
	AbstractMapDrawer *mdrw = dynamic_cast<AbstractMapDrawer *>(dr);
	if (!mdrw)
		return;
	Representation rpr = mdrw->getRepresentation();
	if (rpr.fValid())
		IlwWinApp()->ShowPopupMenu(w, p, rpr->fnObj);
}

//////////////////////////////////////////////////////////////////////
// PropertiesLayerTreeItem
//////////////////////////////////////////////////////////////////////

PropertiesLayerTreeItem::PropertiesLayerTreeItem(LayerTreeView* ltv, IlwisObjectPtr* obj) 
: LayerTreeItem(ltv)
{
	ptr = obj;
}

PropertiesLayerTreeItem::~PropertiesLayerTreeItem()
{
}

void PropertiesLayerTreeItem::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	String sName = ptr->sNameQuoted(true);
	IlwWinApp()->Execute(String("prop %S", sName));
}

//////////////////////////////////////////////////////////////////////
// ColumnLayerTreeItem
//////////////////////////////////////////////////////////////////////

class ChooseColumnComboBox: public CComboBox
{
public:
	ChooseColumnComboBox(CWnd* wnd, ColumnLayerTreeItem* clti, CRect rect);
	void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	void OnKillFocus(CWnd* pWnd);
	void OnSelChange(NMHDR* pNotifyStruct, LRESULT* result);
private:
	ColumnLayerTreeItem* clti;
	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(ChooseColumnComboBox, CComboBox)
	ON_WM_KEYDOWN()
	ON_WM_KILLFOCUS()
	ON_NOTIFY_REFLECT(CBN_SELCHANGE, OnSelChange)
END_MESSAGE_MAP()

ChooseColumnComboBox::ChooseColumnComboBox(CWnd* wnd, ColumnLayerTreeItem* lti, CRect rect)
{
	clti = lti;
	Create(WS_CHILD|
		CBS_HASSTRINGS|WS_VSCROLL|
		CBS_DROPDOWNLIST|WS_BORDER|CBS_AUTOHSCROLL|CBS_NOINTEGRALHEIGHT,
		rect, wnd, 0);

	Table tbl = clti->mdr()->getBaseMap()->tblAtt();
	for (int i = 0; i < tbl->iCols(); ++i) {
		Column col = tbl->col(i);
		Domain dm = col->dm();
		if (dm->pdc() || dm->pdid() || dm->pdv() ||
			dm->pdp() || dm->pdcol())
		{
			String sCol = col->sName();
			AddString(sCol.scVal());
		}
	}
	//if (clti->mdr()->getAtttributeColumn().fValid()) {
	//	String sCol = clti->mdr()->getAtttributeColumn()->sName();
	//	SelectString(-1, sCol.scVal());
	//}
	ShowWindow(SW_SHOW);
}

void ChooseColumnComboBox::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CComboBox::OnKeyDown(nChar, nRepCnt, nFlags);
}

void ChooseColumnComboBox::OnKillFocus(CWnd* pWnd)
{
	clti->FinishColumnField();
}

void ChooseColumnComboBox::OnSelChange(NMHDR* pNotifyStruct, LRESULT* result)
{
	CString str;
	GetWindowText(str);
	Table tbl = clti->mdr()->getBaseMap()->tblAtt();
	//clti->mdr()->setAttributeColumn(String(str));
	//clti->ltv->GetDocument()->UpdateAllViews(clti->ltv, 0);
}



ColumnLayerTreeItem::ColumnLayerTreeItem(LayerTreeView* ltv, AbstractMapDrawer* drw, HTREEITEM htiClm) 
: LayerTreeItem(ltv), cccb(0), hti(htiClm)
{
	dr = drw;
}

ColumnLayerTreeItem::~ColumnLayerTreeItem()
{
}

void ColumnLayerTreeItem::OnLButtonDown(UINT nFlags, CPoint point)
{
	ShowColumnField();
}

void ColumnLayerTreeItem::OnContextMenu(CWnd* w, CPoint p)
{
}

void ColumnLayerTreeItem::SwitchCheckBox(bool fOn)
{
//	mdr()->setUseAttributeTable(fOn);
	if (fOn)
		ShowColumnField();
	MapCompositionDoc* doc = ltv->GetDocument();
	doc->ChangeState();
	doc->UpdateAllViews(ltv,0);
}

void ColumnLayerTreeItem::ShowColumnField()
{
	CRect rect, rectWnd;
	ltv->GetClientRect(rectWnd);
	ltv->GetTreeCtrl().GetItemRect(hti, &rect, TRUE);
	rect.bottom += 150;
	rect.right = rectWnd.right;
	if (0 == cccb)
		cccb = new ChooseColumnComboBox(ltv, this, rect);
}

void ColumnLayerTreeItem::FinishColumnField()
{
	if (cccb)
		delete cccb;
	cccb = 0;
}

//////////////////////////////////////////////////////////////////////
// LegendClassLayerTreeItem
//////////////////////////////////////////////////////////////////////

LegendClassLayerTreeItem::LegendClassLayerTreeItem(LayerTreeView* ltv, NewDrawer* _dr, Domain _dm, int iR)
: LayerTreeItem(ltv), dr(_dr), iRaw(iR), dm(_dm)
{
}

LegendClassLayerTreeItem::~LegendClassLayerTreeItem()
{
}

void LegendClassLayerTreeItem::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	//FormBaseDialog* frm = dr->wEditRpr(ltv, iRaw);
	//if (0 == frm)
	//	return;
	//if (frm->fOkClicked()) {
	//	MapCompositionDoc* doc = ltv->GetDocument();
	//	doc->ChangeState();
	//	doc->UpdateAllViews(ltv, 0);
	//	ltv->Invalidate();
	//}
	//delete frm;
	//ltv->SetFocus();
}

void LegendClassLayerTreeItem::SwitchCheckBox(bool fOn)
{
	PreparationParameters pp(NewDrawer::ptRENDER);
	pp.filteredRaws.push_back(fOn ? iRaw : -iRaw);
	dr->prepare(&pp);
	MapCompositionDoc* doc = ltv->GetDocument();
	doc->mpvGetView()->Invalidate();

}

void LegendClassLayerTreeItem::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTVCUSTOMDRAW lptvcd = (LPNMTVCUSTOMDRAW) pNMHDR;
	switch (lptvcd->nmcd.dwDrawStage) 
	{
	case CDDS_ITEMPREPAINT:
		// post paint otherwise line is not drawn at left side
		*pResult = CDRF_NOTIFYPOSTPAINT;
		return;
	case CDDS_ITEMPOSTPAINT:
		{
			CDC cdc;
			cdc.Attach(lptvcd->nmcd.hdc);
			CRect rect = lptvcd->nmcd.rc;
			HTREEITEM hti = ltv->GetTreeCtrl().HitTest(rect.TopLeft());
			ltv->GetTreeCtrl().GetItemRect(hti, &rect, TRUE);
			rect.left -= 20;
			rect.bottom += 1;
			rect.right += 20;
			Color clrText = SysColor(COLOR_WINDOWTEXT);
			Color clrBack = SysColor(COLOR_WINDOW);
			Color clrTextSel = SysColor(COLOR_HIGHLIGHTTEXT);
			Color clrSel  = SysColor(COLOR_HIGHLIGHT);
			CPen penNull(PS_NULL,0,Color(0,0,0));
			CPen penBlack(PS_SOLID,1,clrText);
			CBrush brWhite(clrBack);
			CPen* penOld = cdc.SelectObject(&penBlack);
			CBrush* brOld = cdc.SelectObject(&brWhite);
			penOld = cdc.SelectObject(&penBlack);
			rect.top += 1;
			rect.bottom -= 1;
			int iHeight = rect.Height();
			int iWidth = 1.5 * iHeight;
			rect.right = rect.left + iWidth;

			CRgn rgn;
			rgn.CreateRectRgnIndirect(&rect);
			cdc.SelectClipRgn(&rgn);
			dr->drawLegendItem(&cdc, rect, iRaw);
			cdc.SelectClipRgn(0);

			String sText = dm->sValueByRaw(iRaw,0);
			CPoint pt;
			pt.x = rect.right + 2;
			pt.y = rect.top;
			if (ltv->GetTreeCtrl().GetItemState(hti, TVIS_SELECTED) && false) {
				cdc.SetTextColor(clrTextSel);
				cdc.SetBkColor(clrSel);
			}
			else {
				cdc.SetTextColor(clrText);
				cdc.SetBkColor(clrBack);
			}
			cdc.SetBkMode(OPAQUE);
			cdc.TextOut(pt.x, pt.y, sText.scVal(), sText.length());
			cdc.SelectObject(penOld);
			cdc.SelectObject(brOld);
			cdc.Detach();
		}
		return;
	}
}


LegendValueLayerTreeItem::LegendValueLayerTreeItem(LayerTreeView* ltv, NewDrawer* _dr, DomainValueRangeStruct _dvrs, double rValue)
: LayerTreeItem(ltv), dr(_dr), rVal(rValue), dvrs(_dvrs)
{
}

LegendValueLayerTreeItem::~LegendValueLayerTreeItem()
{
}

void LegendValueLayerTreeItem::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTVCUSTOMDRAW lptvcd = (LPNMTVCUSTOMDRAW) pNMHDR;
	switch (lptvcd->nmcd.dwDrawStage) 
	{
	case CDDS_ITEMPREPAINT:
		// post paint otherwise line is not drawn at left side
		*pResult = CDRF_NOTIFYPOSTPAINT;
		return;
	case CDDS_ITEMPOSTPAINT:
		{
			CDC cdc;
			cdc.Attach(lptvcd->nmcd.hdc);
			CRect rect = lptvcd->nmcd.rc;
			HTREEITEM hti = ltv->GetTreeCtrl().HitTest(rect.TopLeft());
			ltv->GetTreeCtrl().GetItemRect(hti, &rect, TRUE);
			rect.left -= 20;
			rect.bottom += 1;
			rect.right += 1000;
			Color clrText = SysColor(COLOR_WINDOWTEXT);
			Color clrBack = SysColor(COLOR_WINDOW);
			Color clrTextSel = SysColor(COLOR_HIGHLIGHTTEXT);
			Color clrSel  = SysColor(COLOR_HIGHLIGHT);
			CPen penNull(PS_NULL,0,Color(0,0,0));
			CPen penBlack(PS_SOLID,1,clrText);
			CBrush brWhite(clrBack);
			CPen* penOld = cdc.SelectObject(&penNull);
			CBrush* brOld = cdc.SelectObject(&brWhite);
			cdc.Rectangle(rect);
			cdc.SelectObject(penOld);
			penOld = cdc.SelectObject(&penBlack);
			rect.top += 1;
			rect.bottom -= 1;
			int iHeight = rect.Height();
			int iWidth = 1.5 * iHeight;
			rect.right = rect.left + iWidth;
			dr->drawLegendItem(&cdc, rect, rVal);
			String sText = dvrs.sValue(rVal,0);
			DomainValue* dv = dvrs.dm()->pdv();
			if (dv->fUnit())
				sText = String("%S %S", sText, dv->sUnit());

			CPoint pt;
			pt.x = rect.right + 2;
			pt.y = rect.top;
			if (ltv->GetTreeCtrl().GetItemState(hti, TVIS_SELECTED)) {
				cdc.SetTextColor(clrTextSel);
				cdc.SetBkColor(clrSel);
			}
			else {
				cdc.SetTextColor(clrText);
				cdc.SetBkColor(clrBack);
			}
			cdc.SetBkMode(OPAQUE);
			cdc.TextOut(pt.x, pt.y, sText.scVal(), sText.length());
			cdc.SelectObject(penOld);
			cdc.SelectObject(brOld);
			cdc.Detach();
		}
		return;
	}
}
//-----------------------------------
DisplayOptionTreeItem::DisplayOptionTreeItem(LayerTreeView* ltv, HTREEITEM _parent, NewDrawer *dr, SetCheckFunc f,DisplayOptionItemFunc fun, NewDrawer *_altHandler)
: LayerTreeItem(ltv),
func(fun),
drw(dr),
hti(0),
checks(0),
setCheckFunc(f),
altHandler(_altHandler),
parent(_parent)
{
}

DisplayOptionTreeItem::DisplayOptionTreeItem(LayerTreeView* ltv, HTREEITEM _parent, NewDrawer *dr, DisplayOptionItemFunc f,HTREEITEM item, SetChecks *ch, SetCheckFunc cf)
: LayerTreeItem(ltv),
func(f),
drw(dr),
hti(item),
checks(ch),
setCheckFunc(cf),
altHandler(0),
parent(_parent)
{
}

void DisplayOptionTreeItem::setTreeItem(HTREEITEM it) 
{ 
	hti = it; 
	if ( checks)
		checks->addItem(hti);

}
DisplayOptionTreeItem::~DisplayOptionTreeItem()
{
}

void DisplayOptionTreeItem::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if ( func) {
		(drw->*func)(ltv);
		SwitchCheckBox(true);
	}
}

void DisplayOptionTreeItem::SwitchCheckBox(bool fOn) {
	if (checks) {
		checks->checkItem(hti,fOn);
	}
	if ( setCheckFunc && altHandler == 0)
		(drw->*setCheckFunc)(&fOn, ltv);
	if ( altHandler != 0)
		(altHandler->*setCheckFunc)(&fOn, ltv);
}

void DisplayOptionTreeItem::OnContextMenu(CWnd* pWnd, CPoint pos)
{
}

//----------------------------------
DisplayOptionTree::DisplayOptionTree(LayerTreeView* ltv, HTREEITEM hti, NewDrawer *draw)
: LayerTreeItem(ltv),
htiStart(hti),
drw(draw)

{
}

DisplayOptionTree::~DisplayOptionTree()
{
}

void DisplayOptionTree::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	HTREEITEM hItem = htiStart;
	HTREEITEM hStop= ltv->GetTreeCtrl().GetNextItem(hItem, TVGN_NEXTVISIBLE);
	HTREEITEM hti = ltv->GetTreeCtrl().HitTest(point);

	while (hItem != NULL && hItem != hStop)
	{
		ltv->GetTreeCtrl().Expand(hItem,TVE_EXPAND);
		hItem= ltv->GetTreeCtrl().GetNextItem(hItem, TVGN_NEXTVISIBLE);
	}
}

void DisplayOptionTree::OnContextMenu(CWnd* pWnd, CPoint pos){
	AbstractMapDrawer *mapdrw = dynamic_cast<AbstractMapDrawer *>(drw);
	if (!mapdrw)
		return;
	CPoint pnt = pos;
	ltv->GetTreeCtrl().ScreenToClient(&pnt);
	HTREEITEM hti = ltv->GetTreeCtrl().HitTest(pnt);
	int types = ComplexDrawer::dtMAIN | ComplexDrawer::dtPOST | ComplexDrawer::dtPRE;
	BaseMapPtr *mptr = mapdrw->getBaseMap();
	CMenu men;
	men.CreatePopupMenu();
	vector<MapPaneViewTool *> tools;
	IlwWinApp()->getDrawerTools(mapdrw,tools); 
	for(int i = 0; i < tools.size(); ++i) {
		String sMenu = tools[i]->getMenuString();
		if ( sMenu != "")
			men.AppendMenu(MF_STRING, tools[i]->getId(),sMenu.scVal());
	}
	int iCmd = men.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON|TPM_NONOTIFY|TPM_RETURNCMD, pos.x, pos.y, pWnd);
	for(int i = 0; i < tools.size(); ++i) {
		if ( iCmd == tools[i]->getId()){
			if ( hti){
				ILWIS::DrawerParameters parms(drw->getRootDrawer(),drw);
				tools[i]->insertTool(ltv,&parms);
			}
		}
	}
}

//--------------------------------------------
DisplayOptionColorItem::DisplayOptionColorItem(const String& sTxt, LayerTreeView* t, HTREEITEM parent, ILWIS::NewDrawer *dr, DisplayOptionItemFunc f,HTREEITEM item, SetChecks *checks, SetCheckFunc cf) :
DisplayOptionTreeItem(t, parent, dr,f,item,checks, cf) ,
sText(sTxt)
{
}

void DisplayOptionColorItem::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult){
	LPNMTVCUSTOMDRAW lptvcd = (LPNMTVCUSTOMDRAW) pNMHDR;
	switch (lptvcd->nmcd.dwDrawStage) 
	{
	case CDDS_ITEMPREPAINT:
		// post paint otherwise line is not drawn at left side
		*pResult = CDRF_NOTIFYPOSTPAINT;
		return;
	case CDDS_ITEMPOSTPAINT:
		{
			CDC cdc;
			cdc.Attach(lptvcd->nmcd.hdc);
			CRect rect = lptvcd->nmcd.rc;
			HTREEITEM hti = ltv->GetTreeCtrl().HitTest(rect.TopLeft());
			ltv->GetTreeCtrl().GetItemRect(hti, &rect, TRUE);
			rect.left -= 20;
			rect.bottom += 1;
			rect.right += 1000;
			Color clrText = SysColor(COLOR_WINDOWTEXT);
			Color clrBack = SysColor(COLOR_WINDOW);
			Color clrTextSel = SysColor(COLOR_HIGHLIGHTTEXT);
			Color clrSel  = SysColor(COLOR_HIGHLIGHT);
			CPen penNull(PS_NULL,0,Color(0,0,0));
			CPen penBlack(PS_SOLID,1,clrText);
			CBrush brWhite(clrBack);
			CPen* penOld = cdc.SelectObject(&penNull);
			CBrush* brOld = cdc.SelectObject(&brWhite);
			cdc.Rectangle(rect);
			cdc.SelectObject(penOld);
			penOld = cdc.SelectObject(&penNull);
			CBrush brushColor(color);
			cdc.SelectObject(&brushColor);
			rect.top += 1;
			rect.bottom -= 1;
			int iHeight = rect.Height() ;
			int iWidth = 1.5 * iHeight;
			rect.right = rect.left + iWidth ;
			CRect rctColor(rect);
			rctColor.DeflateRect(iWidth * 0.8, iHeight * 0.7);

			cdc.Rectangle(rctColor);

			CPoint pt;
			pt.x = rect.left + 1.5 * iHeight + 2;
			pt.y = rect.top;
			if (ltv->GetTreeCtrl().GetItemState(hti, TVIS_SELECTED)) {
				cdc.SetTextColor(clrTextSel);
				cdc.SetBkColor(clrSel);
			}
			else {
				cdc.SetTextColor(clrText);
				cdc.SetBkColor(clrBack);
			}
			cdc.SetBkMode(OPAQUE);
			cdc.TextOut(pt.x, pt.y, sText.scVal(), sText.length());
			cdc.SelectObject(penOld);
			cdc.SelectObject(brOld);
			cdc.Detach();
		}
		return;
	}
}

void DisplayOptionColorItem::setColor(Color c) {
	color = c;
	ltv->UpdateWindow();
}

SetChecks::~SetChecks() {
}

SetChecks::SetChecks(LayerTreeView *v, NewDrawer *dr,SetCheckFunc _f){
	tv = v;
	drw = dr;
	fun = _f;
}
void SetChecks::addItem(HTREEITEM hti){
	checkedItems.push_back(hti);
}

void SetChecks::checkItem(HTREEITEM hti,bool fOn) {
	CTreeCtrl& tree = tv->GetTreeCtrl();
	for(int i = 0; i< checkedItems.size(); ++i) {
		HTREEITEM ht = checkedItems.at(i);
		tree.SetCheck(ht, FALSE);
	}
	(drw->*fun)(&hti, tv);
	tree.SetCheck(hti,fOn);
}
