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
#include "Engine\Drawers\drawer_n.h"
#include "Engine\Drawers\simpledrawer.h"
#include "Engine\Domain\DmSort.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\Mapwindow\Drawers\DrawerTool.h"
#include "Client\Mapwindow\LayerTreeItem.h"
#include "Headers\constant.h"
#include "Client\FormElements\syscolor.h"
#include "Engine\Domain\Dmvalue.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


#define sMen(ID) ILWSF("men",ID).c_str()
#define pmadd(ID) men.AppendMenu(MF_STRING, ID, sMen(ID)); 

using namespace ILWIS;


//////////////////////////////////////////////////////////////////////
// LayerTreeItem
//////////////////////////////////////////////////////////////////////

LayerTreeItem::LayerTreeItem(LayerTreeView* ltview) 
{
	ltv = ltview;
	htiStart = 0;
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
	HTREEITEM hItem = ltv->GetTreeCtrl().HitTest(point);
	HTREEITEM hStop= ltv->GetTreeCtrl().GetNextItem(hItem, TVGN_NEXTVISIBLE);
	CString txt = ltv->GetTreeCtrl().GetItemText(hStop);
	ltv->GetTreeCtrl().Expand(hItem,TVE_TOGGLE);
	vector<HTREEITEM> items;
	while (hItem != NULL)
	{
		hItem = ltv->GetTreeCtrl().GetNextItem(hItem, TVGN_NEXTVISIBLE);
		if ( hItem == hStop)
			break;
		CString txt = ltv->GetTreeCtrl().GetItemText(hItem);
		if ( String(txt) == TR("Operations") )
			continue;
		items.push_back(hItem);
	}
	for(int i = 0; i < items.size(); ++i)
		ltv->GetTreeCtrl().Expand(items[i], TVE_EXPAND);
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
	SpatialDataDrawer *mapdrw = dynamic_cast<SpatialDataDrawer *>(dr);
	if (!mapdrw)
		return;
	int types = ComplexDrawer::dtMAIN | ComplexDrawer::dtPOST | ComplexDrawer::dtPRE;
	BaseMapPtr *mptr = mapdrw->getBaseMap();
	CMenu men;
	men.CreatePopupMenu();
	if (mapdrw) {
		pmadd(ID_EDITLAYER);
		pmadd(ID_PROPLAYER);
		pmadd(ID_ZOOM_TO_LAYER);
		pmadd(ID_ZOOM_TO_SELECTED);
	}
	pmadd(ID_REMOVELAYER);
	int iCmd = men.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON|TPM_NONOTIFY|TPM_RETURNCMD, p.x, p.y, w);
	switch (iCmd) 
	{
	case ID_EDITLAYER:
		if (mapdrw->isEditable()) {
			MapCompositionDoc* doc = ltv->GetDocument();
			MapPaneView* mpv = doc->mpvGetView();
			if (0 != mpv)
				mpv->EditNamedLayer(mapdrw->getBaseMap()->fnObj);
		}
		break;
	case ID_PROPLAYER:
		IlwWinApp()->Execute(String("prop %S", mptr->fnObj.sFullNameQuoted()));
		break;
	case ID_REMOVELAYER:
		ltv->OnRemoveLayer();
		break;
	case ID_ZOOM_TO_LAYER:
		{
			CoordBounds cb = mptr->cb();
			CoordBounds cbConv = mapdrw->getRootDrawer()->getCoordinateSystem()->cbConv(mptr->cs(),cb);
			mapdrw->getRootDrawer()->setCoordBoundsZoom(cbConv);
			ltv->GetDocument()->mpvGetView()->Invalidate();

		}
		break;
	case ID_ZOOM_TO_SELECTED:
		{
			ComplexDrawer *cdrw = dynamic_cast<ComplexDrawer *>(mapdrw->getDrawer(0));
			if ( !cdrw)
				return;
			CoordBounds cbSelected;
			for(int i = 0; i < cdrw->getDrawerCount(); ++i) {
				SimpleDrawer *sdrw = static_cast<SimpleDrawer *>(cdrw->getDrawer(i));
				if (!sdrw)
					continue;
				if ( sdrw->getSpecialDrawingOption(NewDrawer::sdoSELECTED) != 0) {
					cbSelected += sdrw->getBounds();
				}
			}
			if ( cbSelected.fValid()) {
				RowCol rc = mapdrw->getRootDrawer()->getViewPort();
				double fac = (double)rc.Col / rc.Row;
				double xmin, xmax, ymin, ymax;
				if ( cbSelected.width() > cbSelected.height()){
					xmin = cbSelected.cMin.x;
					xmax = cbSelected.cMax.x;
					ymin = cbSelected.middle().y -  cbSelected.width() * 1.0 / (2 * fac);
					ymax = cbSelected.middle().y + cbSelected.width() * 1.0 / (2 * fac);
				} else if ( cbSelected.width() > cbSelected.height()){
					xmin = cbSelected.cMin.x;
					xmax = xmin + cbSelected.height() * fac;
					ymin = cbSelected.cMin.y;
					ymax = cbSelected.cMax.y;
				}
				cbSelected = CoordBounds(Coord(xmin,ymin), Coord( xmax, ymax));
					
				cbSelected *= 1.1;
				CoordBounds cbConv = mapdrw->getRootDrawer()->getCoordinateSystem()->cbConv(mptr->cs(),cbSelected);
				cdrw->getRootDrawer()->setCoordBoundsZoom(cbConv);
				ltv->GetDocument()->mpvGetView()->Invalidate();
			}

		}
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
		IlwWinApp()->OpenDocumentFile(sName.c_str());
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
	SpatialDataDrawer *mdrw = dynamic_cast<SpatialDataDrawer *>(dr);
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
			AddString(sCol.c_str());
		}
	}
	if (clti->mdr()->getAtttributeColumn().fValid()) {
		String sCol = clti->mdr()->getAtttributeColumn()->sName();
		SelectString(-1, sCol.c_str());
	}
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
	clti->mdr()->setAttributeColumn(String(str));
	clti->ltv->GetDocument()->UpdateAllViews(clti->ltv, 0);
}



ColumnLayerTreeItem::ColumnLayerTreeItem(LayerTreeView* ltv, SpatialDataDrawer* drw, HTREEITEM htiClm) 
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
	mdr()->setUseAttributeTable(fOn);
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

LegendClassLayerTreeItem::LegendClassLayerTreeItem(LayerTreeView* ltv, HTREEITEM parent, NewDrawer* dr, Domain _dm, int iR, const Column& col)
: DisplayOptionTreeItem(ltv,parent, dr), iRaw(iR), dm(_dm), attCol(col)
{
}

LegendClassLayerTreeItem::~LegendClassLayerTreeItem()
{
}

void LegendClassLayerTreeItem::SwitchCheckBox(bool fOn)
{
	PreparationParameters pp(NewDrawer::ptRENDER);
	if ( attCol.fValid()) {
		for(int i = 1; i <= attCol->iRecs(); ++i) {
			long iRawRec = attCol->iRaw(i);
			if ( iRawRec == iRaw) {
				long r = i;
				pp.filteredRaws.push_back(fOn ? r : -r);
			}
		}
	}
	else
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
			((ComplexDrawer *)dr)->drawLegendItem(&cdc, rect, iRaw);
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
			cdc.TextOut(pt.x, pt.y, sText.c_str(), sText.length());
			cdc.SelectObject(penOld);
			cdc.SelectObject(brOld);
			cdc.Detach();
		}
		return;
	}
}


LegendValueLayerTreeItem::LegendValueLayerTreeItem(LayerTreeView* ltv, HTREEITEM parent, NewDrawer* dr, DomainValueRangeStruct _dvrs, double rValue)
: DisplayOptionTreeItem(ltv,parent, dr), rVal(rValue), dvrs(_dvrs)
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
			double domstep = dvrs.rStep();
			int w =  abs(domstep - (int)domstep) < 0.0000001 ? 0 : -1;
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
			((ComplexDrawer *)dr)->drawLegendItem(&cdc, rect, rVal);
			String sText = dvrs.sValue(rVal,w,w);
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
			cdc.TextOut(pt.x, pt.y, sText.c_str(), sText.length());
			cdc.SelectObject(penOld);
			cdc.SelectObject(brOld);
			cdc.Detach();
		}
		return;
	}
}
//-----------------------------------
DisplayOptionTreeItem::DisplayOptionTreeItem(LayerTreeView* ltv, HTREEITEM _parent, NewDrawer *drw)
: LayerTreeItem(ltv),
dr(drw),
hti(0),
checks(0),
altHandler(0),
parent(_parent),
dbctool(0),
chctool(0),
dbcAction(0),
dtSetCheckFunc(0),
tool(0)
{
}

void DisplayOptionTreeItem::setDoubleCickAction(ILWIS::DrawerTool *_tool, DTDoubleClickActionFunc f) {
	dbctool = _tool;
	dbcAction = f;
}

void DisplayOptionTreeItem::setCheckAction(ILWIS::DrawerTool *_tool, SetChecks *_checks, DTSetCheckFunc _cf) {
	chctool = _tool;
	checks = _checks;
	dtSetCheckFunc = _cf;
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
	if ( dbctool && dbcAction) {
		(dbctool->*dbcAction)();
	} else {
		HTREEITEM hItem = htiStart;
		HTREEITEM hStop= ltv->GetTreeCtrl().GetNextItem(hItem, TVGN_NEXTVISIBLE);
		HTREEITEM hti = ltv->GetTreeCtrl().HitTest(point);
		ltv->GetTreeCtrl().Expand(hti,TVE_TOGGLE);
		while (hItem != NULL && hItem != hStop)
		{
			ltv->GetTreeCtrl().Expand(hItem,TVE_EXPAND);
			hItem= ltv->GetTreeCtrl().GetNextItem(hItem, TVGN_NEXTVISIBLE);
		}
	}
	//SwitchCheckBox(true);
}

void DisplayOptionTreeItem::SwitchCheckBox(bool fOn) {
	if (checks) {
		checks->checkItem(hti);
	}
	if ( dtSetCheckFunc && altHandler == 0)
		(chctool->*dtSetCheckFunc)(&fOn, hti);
	//if ( altHandler != 0)
	//	(altHandler->*setCheckFunc)(&fOn, ltv);
}

void DisplayOptionTreeItem::OnContextMenu(CWnd* pWnd, CPoint pos)
{
}

void DisplayOptionTreeItem::expand(){
	if ( tool)
		tool->doAction();
}

void DisplayOptionTreeItem::setTool(DrawerTool *t){
	tool = t;
}

void DisplayOptionTreeItem::setImage(int im){
	image = im;
}

//----------------------------------
DisplayOptionTree::DisplayOptionTree(LayerTreeView* ltv, HTREEITEM hti, NewDrawer *draw, DrawerTool *t)
: LayerTreeItem(ltv),
htiStart(hti),
drw(draw),
tool(t)
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
	ltv->GetTreeCtrl().Expand(hti,TVE_TOGGLE);
	while (hItem != NULL && hItem != hStop)
	{
		ltv->GetTreeCtrl().Expand(hItem,TVE_EXPAND);
		hItem= ltv->GetTreeCtrl().GetNextItem(hItem, TVGN_NEXTVISIBLE);
	}
}

void DisplayOptionTree::addMenuItem(ILWIS::DrawerTool *parentTool, CMenu& men, vector<DrawerTool *>& tools) {
	for(int i = 0; i < parentTool->getToolCount(); ++i) {
		DrawerTool *tool = parentTool->getTool(i);
		String sMenu = tool->getMenuString();
		if ( sMenu != "") {
			men.AppendMenu(MF_STRING, tool->getId(),sMenu.c_str());
			tools.push_back(tool);
			if (tool->isActive())
				men.CheckMenuItem(tool->getId(), MF_CHECKED);
		}
		addMenuItem(tool,men, tools);
	}
}

void DisplayOptionTree::OnContextMenu(CWnd* pWnd, CPoint pos){
	CPoint pnt = pos;
	ltv->GetTreeCtrl().ScreenToClient(&pnt);
	HTREEITEM hti = ltv->GetTreeCtrl().HitTest(pnt);
	int types = ComplexDrawer::dtMAIN | ComplexDrawer::dtPOST | ComplexDrawer::dtPRE;
	DisplayOptionTree *doTree = (DisplayOptionTree * )ltv->GetTreeCtrl().GetItemData(hti);
	DrawerTool *setTool = doTree->getTool();
	CMenu men;
	men.CreatePopupMenu();
	vector<DrawerTool *> tools; // tools that are in the context menu;
	addMenuItem(setTool, men, tools);

	int iCmd = men.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON|TPM_NONOTIFY|TPM_RETURNCMD, pos.x, pos.y, pWnd);
	for(int i = 0; i < tools.size(); ++i) {
		DrawerTool *tool =tools[i];
		if ( iCmd == tool->getId()){
			if ( hti){
				UINT state = men.GetMenuState(iCmd,MF_BYCOMMAND);
				// it still has the state it had so invert the logic
				if ( state & MF_CHECKED)
					tool->setActiveMode(false);
				else
					tool->setActiveMode(true);
				tool->prepare();
			}
		}
	}
}

//--------------------------------------------
DisplayOptionColorItem::DisplayOptionColorItem(const String& sTxt, LayerTreeView* t, HTREEITEM parent, ILWIS::NewDrawer *dr) :
DisplayOptionTreeItem(t, parent, dr) ,
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
			cdc.TextOut(pt.x, pt.y, sText.c_str(), sText.length());
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

//-------------------------------------------------------------------------------------
DisplayOptionRadioButtonItem::DisplayOptionRadioButtonItem(const String& sTxt, LayerTreeView* t, HTREEITEM parent, ILWIS::NewDrawer *dr) :
DisplayOptionTreeItem(t, parent, dr) ,
sText(sTxt),
isSelected(false)

{
}

void DisplayOptionRadioButtonItem::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (checks) {
		checks->checkItem(hti);

	}
	if ( dtSetCheckFunc && altHandler == 0)
		(chctool->*dtSetCheckFunc)(&isSelected,hti);
}


void DisplayOptionRadioButtonItem::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult){
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
			CBrush brBlack(clrText);
			CPen* penOld = cdc.SelectObject(&penNull);
			CBrush* brOld = cdc.SelectObject(&brWhite);
			cdc.Rectangle(rect);
			cdc.SelectObject(penOld);
			cdc.SelectObject(&penNull);
			CBrush brushColor(clrText);
			cdc.SelectObject(&brWhite);
			cdc.SelectObject(&penBlack);
			rect.top += 1;
			rect.bottom -= 1;
			int iHeight = rect.Height() * 0.8;
			int iWidth = iHeight;
			rect.right = rect.left + iWidth ;
			rect.bottom = rect.top + iHeight;
			CRect rctColor(rect);
			rctColor.MoveToY(rect.top + 2);
			cdc.Ellipse(rctColor);

			if ( isSelected) {
				rctColor.DeflateRect(iWidth * 0.9, iHeight * 0.9);
				cdc.SelectObject(&brBlack);
				cdc.SelectObject(&penBlack);
				cdc.Ellipse(rctColor);
			} 

			CPoint pt;
			pt.x = rect.left + iHeight + 2;
			pt.y = rect.top;
			if ( tool && image != iUNDEF) {
				IlwWinApp()->ilSmall.Draw(&cdc, image, pt, ILD_NORMAL); 
				pt.x = pt.x + 18;
			} else

			if (ltv->GetTreeCtrl().GetItemState(hti, TVIS_SELECTED)) {
				cdc.SetTextColor(clrTextSel);
				cdc.SetBkColor(clrSel);
			}
			else {
				cdc.SetTextColor(clrText);
				cdc.SetBkColor(clrBack);
			}
			cdc.SetBkMode(OPAQUE);
			cdc.TextOut(pt.x, pt.y, sText.c_str(), sText.length());
			cdc.SelectObject(penOld);
			cdc.SelectObject(brOld);
			cdc.Detach();
		}
		return;
	}
}

void DisplayOptionRadioButtonItem::setState(bool yesno) {
	isSelected = yesno;
}

bool DisplayOptionRadioButtonItem::getState() const {
	return isSelected;
}

//------------------------------------------
//-------------------------------------------------------------------------------------
DisplayOptionButtonItem::DisplayOptionButtonItem(const String& sTxt, LayerTreeView* t, HTREEITEM parent, ILWIS::NewDrawer *dr) :
DisplayOptionTreeItem(t, parent, dr) ,
sText(sTxt),
isSelected(false)

{
}

void DisplayOptionButtonItem::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (checks) {
		checks->checkItem(hti);

	}
	if ( dtSetCheckFunc && altHandler == 0)
		(chctool->*dtSetCheckFunc)(&isSelected,hti);
}


void DisplayOptionButtonItem::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult){
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
			Color clrBack = SysColor(COLOR_BTNSHADOW);
			Color clrTextSel = SysColor(COLOR_HIGHLIGHTTEXT);
			Color clrSel  = SysColor(COLOR_BTNHIGHLIGHT);
			CPen penNull(PS_NULL,0,Color(0,0,0));
			CPen penBlack(PS_SOLID,1,clrText);
			CBrush brWhite(clrBack);
			CBrush brBlack(clrText);
			CBrush brSel(clrSel);
			CPen* penOld = cdc.SelectObject(&penNull);
			CBrush* brOld = cdc.SelectObject(&brWhite);
			cdc.Rectangle(rect);
			cdc.SelectObject(penOld);
			cdc.SelectObject(&penNull);
			CBrush brushColor(clrText);
			cdc.SelectObject(&brWhite);
			cdc.SelectObject(&penBlack);
			rect.top += 1;
			rect.bottom -= 1;
			int iHeight = rect.Height() * 0.8;
			int iWidth = iHeight;
			rect.right = rect.left + iWidth ;
			rect.bottom = rect.top + iHeight;
			CRect rctColor(rect);
			rctColor.MoveToY(rect.top + 2);
			cdc.Rectangle(rctColor);

			if ( !isSelected) {
				rctColor.DeflateRect(iWidth * 0.9, iHeight * 0.9);
				cdc.SelectObject(&brSel);
				cdc.SelectObject(&penBlack);
				cdc.Rectangle(rctColor);
			} 

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
			cdc.TextOut(pt.x, pt.y, sText.c_str(), sText.length());
			cdc.SelectObject(penOld);
			cdc.SelectObject(brOld);
			cdc.Detach();
		}
		return;
	}
}

void DisplayOptionButtonItem::setState(bool yesno) {
	isSelected = yesno;
}

bool DisplayOptionButtonItem::getState() const {
	return isSelected;
}
//----------------------------------------

SetChecks::~SetChecks() {
}

SetChecks::SetChecks(LayerTreeView *v, DrawerTool *dt,DTSetCheckFunc _f){
	tv = v;
	tool = dt;
	fun = _f;
	state = 0;
}

void SetChecks::addItem(HTREEITEM hti){
	checkedItems.push_back(hti);
}

void SetChecks::setActive(bool yesno) {
	if ( yesno) {
	}
	else {
		CTreeCtrl& tree = tv->GetTreeCtrl();
		for(int i = 0; i< checkedItems.size(); ++i) {
			HTREEITEM ht = checkedItems.at(i);
			DisplayOptionRadioButtonItem *item = dynamic_cast<DisplayOptionRadioButtonItem * >((LayerTreeItem *)(tree.GetItemData(ht)));
			item->setState(false);
		}
		tv->Invalidate();
		tv->UpdateWindow();
	}
}

void SetChecks::clear() {
	checkedItems.clear();
}

void SetChecks::checkItem(HTREEITEM hti) {
	CTreeCtrl& tree = tv->GetTreeCtrl();
	/*TV_ITEM treeItem;
	treeItem.mask = TVIF_STATE;
	treeItem.hItem = hti;
	BOOL valid = tree.GetItem(&treeItem);
	if (!valid)
		return;*/
	DisplayOptionRadioButtonItem *item = dynamic_cast<DisplayOptionRadioButtonItem * >((LayerTreeItem *)(tree.GetItemData(hti)));
	if ( item) {
		bool currentState = item->getState();
		if ( currentState) // it was selected and will be selected, nothing to do;
			return;
		currentState  = !currentState;
		item->setState(currentState);
	}
	for(int i = 0; i< checkedItems.size(); ++i) {
		HTREEITEM ht = checkedItems.at(i);
		DisplayOptionRadioButtonItem *item = dynamic_cast<DisplayOptionRadioButtonItem * >((LayerTreeItem *)(tree.GetItemData(ht)));
		if ( ht == hti) {
			state = i;
			continue;
		}
		if ( item) {
			item->setState(false);
		}
	}
	(tool->*fun)(&hti,hti);
	tv->Invalidate();
	tv->UpdateWindow();
}

void SetChecks::checkItem(int index) {
	if ( checkedItems.size() > index) {
		HTREEITEM hti = checkedItems[index];
		checkItem(hti);
	}
}

HTREEITEM SetChecks::getHTI(int index){
	if ( index < checkedItems.size())
		return checkedItems[index];

	return 0;
}

