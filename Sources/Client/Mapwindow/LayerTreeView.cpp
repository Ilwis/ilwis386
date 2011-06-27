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
// LayerTreeView.cpp : implementation file
//

#include "Client\Headers\formelementspch.h"
#include "Engine\Map\Segment\Seg.h"
#include "engine\map\polygon\POL.H"
#include "Client\ilwis.h"
#include "Client\Base\datawind.h"
#include "Client\Mapwindow\MapWindow.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\MainWindow\Catalog\CatalogDocument.h"
#include <afxole.h>
#include "Engine\Map\Mapview.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Drawers\SimpleDrawer.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\Mapwindow\Drawers\DrawerTool.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\Positioner.h"
#include "Headers\Hs\Drwforms.hs"
#include "Client\FormElements\fldcol.h"
#include "Client\FormElements\fldrpr.h"
#include "Client\FormElements\fldcolor.h"
#include "Client\Mapwindow\LayerTreeItem.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Engine\Domain\dmclass.h"
#include "Client\Base\Menu.h"
#include "Headers\constant.h"
#include "Headers\Hs\Mapwind.hs"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// LayerTreeView
BEGIN_MESSAGE_MAP(LayerTreeView, CTreeView)
	//{{AFX_MSG_MAP(LayerTreeView)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_KEYDOWN()
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
	ON_WM_DESTROY()
	ON_WM_RBUTTONDOWN()
	ON_WM_CONTEXTMENU()
	ON_NOTIFY_REFLECT(TVN_BEGINDRAG, OnBeginDrag)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()	
	ON_COMMAND(ID_REMOVELAYER, OnRemoveLayer)
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDING, OnExpanding)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

LayerTreeView::LayerTreeView()
{
	odt = new COleDropTarget;
	fDragging = false;
	drwTool = 0;
}

LayerTreeView::~LayerTreeView()
{
	delete odt;
	if ( drwTool)
		drwTool->clear();
	delete drwTool;
}

void LayerTreeView::OnDestroy()
{
	DeleteAllItems();
	CTreeView::OnDestroy();
}


MapCompositionDoc* LayerTreeView::GetDocument()
{
	return (MapCompositionDoc*)m_pDocument;
}


/////////////////////////////////////////////////////////////////////////////
// LayerTreeView message handlers

void LayerTreeView::DeleteAllItems()
{
	if (!IsWindow(m_hWnd))
		return;
	CTreeCtrl& tc = GetTreeCtrl();
	if (!IsWindow(tc.m_hWnd))
		return;
	HTREEITEM htiChild = tc.GetChildItem(TVI_ROOT);
	while (htiChild) 
	{
		DeleteAllItems(htiChild);
		HTREEITEM htiNext = tc.GetNextSiblingItem(htiChild);
		tc.DeleteItem(htiChild);
		htiChild = htiNext;
		if (htiNext == 0)
			htiChild = tc.GetChildItem(TVI_ROOT);
	}
}

void LayerTreeView::DeleteAllItems(HTREEITEM hti, bool childerenOnly)
{
	CTreeCtrl& tc = GetTreeCtrl();
	if ( hti == 0)
		return ;

	if (tc.ItemHasChildren(hti)) {
		HTREEITEM htiChild = tc.GetChildItem(hti);
		while (htiChild) 
		{
			HTREEITEM htiNext = tc.GetNextSiblingItem(htiChild);
			DeleteAllItems(htiChild);
			htiChild = htiNext;
		}
	}
	if ( !childerenOnly) {
		LayerTreeItem* lti = (LayerTreeItem*)tc.GetItemData(hti);
		if (lti) {
			delete lti;
			tc.SetItemData(hti, 0);
		}
		tc.DeleteItem(hti);
	}
	collectStructure();
}

void LayerTreeView::NextNode(HTREEITEM hItem, const String& name) {
	TreeItem titem;

	if ( getItem(hItem, TVIF_TEXT | TVIF_HANDLE,titem)) {
		String part("%S|%s", name, titem.item.pszText);
		nodes[part] = NodeInfo(hItem);
		collectStructure(hItem,part);
	}
}

void LayerTreeView::collectStructure(HTREEITEM parent, const String& name) {
	if ( name == "") { // start at root
		nodes.clear();
		HTREEITEM hItem = GetTreeCtrl().GetNextItem(TVI_ROOT, TVGN_ROOT);
		TreeItem titem;
		if (getItem(hItem,TVIF_TEXT | TVIF_HANDLE,titem))
			collectStructure(hItem, String("%s", titem.item.pszText));
		else
			return;
	}
	set<String> names;
	HTREEITEM hItem= GetTreeCtrl().GetNextItem(parent, TVGN_CHILD);
	if ( hItem) {
		NextNode(hItem,name);

	}
	hItem= GetTreeCtrl().GetNextItem(hItem, TVGN_NEXT);
	while ( hItem) {
		NextNode(hItem,name);
		hItem= GetTreeCtrl().GetNextItem(hItem, TVGN_NEXT);
	}
}

HTREEITEM LayerTreeView::addMapItem(ILWIS::SpatialDataDrawer *mapDrawer, HTREEITEM after, int lastTool) {
	CTreeCtrl& tc = GetTreeCtrl();
	BaseMapPtr *bmp = mapDrawer->getBaseMap();
	int iImg = IlwWinApp()->iImage(mapDrawer->iconName());
	String sName = mapDrawer->description();

	HTREEITEM htiMap = tc.InsertItem(sName.scVal(),iImg,iImg,TVI_ROOT,after);
	tc.SetItemData(htiMap, (DWORD_PTR)new DrawerLayerTreeItem(this, mapDrawer));		
	tc.SetCheck(htiMap, mapDrawer->isActive());

	Domain dm = bmp->dm();


	if ( mapDrawer->getType() == "AnimationDrawer") {
		DrawerTool *dt = DrawerTool::createTool("AnimationTool",GetDocument()->mpvGetView(),this,mapDrawer);
		if ( dt) {
			drwTool->addTool(dt, lastTool);
			dt->configure(htiMap);
		}
	} else 	if ( mapDrawer->getType() == "CollectionDrawer") {
		DrawerTool *dt = DrawerTool::createTool("CollectionTool",GetDocument()->mpvGetView(),this,mapDrawer);
		if ( dt) {
			if ( dt->isToolUseableFor(mapDrawer)) { 
				drwTool->addTool(dt, lastTool);
				dt->configure(htiMap);
			}
		}
	} else {
		for( int  i=0; i < mapDrawer->getDrawerCount(); ++i) {
			NewDrawer *drw = mapDrawer->getDrawer(i);
			if ( drw) {
				DrawerTool *dt = DrawerTool::createTool("LayerDrawerTool",GetDocument()->mpvGetView(),this,drw);
				if ( dt) {
					drwTool->addTool(dt, lastTool);
					dt->configure(htiMap);
					DrawerTool *threeDStack = drwTool->getTool("ThreeDStack");
					if ( threeDStack) {
						threeDStack->update();
					}
				}
			}
		}
	}

	sName = "Properties";
	int iImgProp = IlwWinApp()->iImage("prop");
	HTREEITEM htiProp = tc.InsertItem(sName.scVal(), iImgProp, iImgProp, htiMap);
	if (0 == htiProp)
		return htiMap;
	tc.SetItemData(htiProp, (DWORD_PTR)new PropertiesLayerTreeItem(this, bmp));		

	if (bmp != 0) {

		sName = String("%S - %S", dm->sName(), dm->sType());
		iImg = IlwWinApp()->iImage(".dom");
		HTREEITEM htiDom = tc.InsertItem(sName.scVal(), iImg, iImg, htiProp);
		if (0 == htiDom)
			return htiMap;

		tc.SetItemData(htiDom, (DWORD_PTR)new ObjectLayerTreeItem(this, dm.pointer()));		

		Representation rpr = mapDrawer->getRepresentation();
		if (rpr.fValid()) {
			sName = String("%S - %S", rpr->sName(), rpr->sType());
			iImg = IlwWinApp()->iImage(".rpr");
			HTREEITEM htiRpr = tc.InsertItem(sName.scVal(), iImg, iImg, htiDom);
			if (0 == htiRpr)
				return htiMap;
			tc.SetItemData(htiRpr, (DWORD_PTR)new ObjectLayerTreeItem(this, rpr.ptr()));		
		}
		HTREEITEM htiGrf = htiProp;
		MapPtr* mp = dynamic_cast<MapPtr*>(bmp);
		if (mp) {
			GeoRef grf =  mp->gr();
			sName = String("%S - %S", grf->sName(), grf->sType());
			iImg = IlwWinApp()->iImage(".grf");
			htiGrf = tc.InsertItem(sName.scVal(), iImg, iImg, htiProp);
			if (0 == htiGrf)
				return htiMap;

			tc.SetItemData(htiGrf, (DWORD_PTR)new ObjectLayerTreeItem(this, grf.pointer()));		
		}
		CoordSystem csy = bmp->cs();
		sName = String("%S - %S", csy->sName(), csy->sType());
		iImg = IlwWinApp()->iImage(".csy");
		HTREEITEM htiCsy = tc.InsertItem(sName.scVal(), iImg, iImg, htiGrf);
		if (0 != htiCsy)
			tc.SetItemData(htiCsy, (DWORD_PTR)new ObjectLayerTreeItem(this, csy.pointer()));		

		if (bmp->fTblAtt()) {	
			Table tbl = bmp->tblAtt();
			if (tbl.fValid()) {
				sName = tbl->sName();
				String sDescr = tbl->sDescr();
				if ("" != sDescr) 
					sName = String("%S - %S", sName, sDescr);
				iImg = IlwWinApp()->iImage(".tbt");
				HTREEITEM htiTbl = tc.InsertItem(sName.scVal(), iImg, iImg, htiProp);
				if (0 == htiTbl)
					return htiMap;
				//						AddPropItems(htiTbl, iImg, tbl);
				tc.SetItemData(htiTbl, (DWORD_PTR)new ObjectLayerTreeItem(this, tbl.pointer()));		
			}
		}
	}
	return htiMap;
}

void LayerTreeView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	if (lHint == 4) // maplistdrawer replaces map
		return; 
	if (lHint == 5) // setdirty report from mappaneview
		return; 
	if (!IsWindow(m_hWnd))
		return;
	CTreeCtrl& tc = GetTreeCtrl();

	tc.SetRedraw(FALSE);
	DeleteAllItems();

	MapCompositionDoc* mcd = GetDocument();

	vector<NewDrawer *> allDrawers;
	HTREEITEM lastNode = TVI_ROOT;
	if ( drwTool != 0)
		delete drwTool;

	drwTool = new DrawerTool("Root", mcd->mpvGetView(),this,0);
	drwTool->addDrawer(mcd->rootDrawer);
	lastNode = drwTool->configure(lastNode);


	mcd->rootDrawer->getDrawers(allDrawers);

	HTREEITEM item = 0;
	int lastTool = drwTool->getToolCount();
	//for(int index = 0; index < allDrawers.size(); ++index) 
	for(int index = allDrawers.size() - 1; index >=0; --index) 
	{
		ComplexDrawer* dr = (ComplexDrawer *)allDrawers.at(index);
		//ILWISSingleLock csl(&dr->cs, TRUE, SOURCE_LOCATION);
		SpatialDataDrawer *mapDrawer = dynamic_cast<SpatialDataDrawer *>(dr);
		if (mapDrawer != 0 && mapDrawer->getBaseMap() != 0){
			item = addMapItem(mapDrawer, lastNode, lastTool);
		} 
	}
	DrawerTool *dt = DrawerTool::createTool("BackgroundTool",mcd->mpvGetView(),this,mcd->rootDrawer);
	drwTool->addTool(dt);
	dt->configure(item);
	resetState();
	GetTreeCtrl().Expand(GetTreeCtrl().GetFirstVisibleItem(),TVE_COLLAPSE);
	tc.SetRedraw(TRUE);
	Invalidate();
}

void LayerTreeView::resetState() {
	HTREEITEM hItem = GetTreeCtrl().GetNextItem(TVI_ROOT, TVGN_ROOT);
	GetTreeCtrl().Expand(hItem,TVE_EXPAND);
	BOOL ret;
	for(map<String,NodeInfo>::iterator cur = nodes.begin(); cur != nodes.end(); ++cur) {
		if ( (*cur).second.expanded ){
			ret = GetTreeCtrl().Expand((*cur).second.hItem,TVE_EXPAND);
		}
	}


}
void LayerTreeView::OnInitialUpdate() 
{
	odt->Register(this);
	CTreeView::OnInitialUpdate();

	CTreeCtrl& tc = GetTreeCtrl();
	tc.SetImageList(&IlwWinApp()->ilSmall, TVSIL_NORMAL);

	ilStates.Create(16,16,TRUE,3,1);
	ilStates.SetImageCount(3);
	zIcon icoEmpty("CheckBoxEmptyIco");
	zIcon icoChecked("CheckBoxCheckedIco");
	ilStates.Replace(1, icoEmpty);
	ilStates.Replace(2, icoChecked);
	tc.SetImageList(&ilStates, TVSIL_STATE);

	collectStructure();

}

void LayerTreeView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CTreeCtrl& tc = GetTreeCtrl();
	UINT uFlags=0;	
	HTREEITEM hti = tc.HitTest(point,&uFlags);
	GetDocument()->setSelectedDrawer(0);
	if(uFlags & TVHT_ONITEMSTATEICON)	
		SwitchCheckBox(hti);
	else {
		CTreeView::OnLButtonDown(nFlags, point);
		LayerTreeItem* lti = 0;
		if (hti) {
			lti = (LayerTreeItem*)tc.GetItemData(hti);
			DrawerLayerTreeItem *drawerItem;
			if ( (drawerItem = dynamic_cast<DrawerLayerTreeItem *>(lti))) {
				GetDocument()->setSelectedDrawer(drawerItem->drw());
			} 
		}
		if (lti)
			lti->OnLButtonDown(nFlags, point);
	}
}

void LayerTreeView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	CTreeCtrl& tc = GetTreeCtrl();
	UINT uFlags=0;	
	HTREEITEM hti = tc.HitTest(point,&uFlags);
	LayerTreeItem* lti = 0;
	if (hti)
		lti = (LayerTreeItem*)tc.GetItemData(hti);
	if (lti)
		lti->OnLButtonDblClk(nFlags, point);
	else
		CTreeView::OnLButtonDblClk(nFlags, point);
}

void LayerTreeView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (nChar == VK_SPACE)	
	{		
		CTreeCtrl& tc = GetTreeCtrl();
		HTREEITEM hti = tc.GetSelectedItem();
		SwitchCheckBox(hti);
	}
	if ( nChar == VK_DELETE) {
		OnRemoveLayer();
	}
	else
		CTreeView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void LayerTreeView::SwitchCheckBox(HTREEITEM hti)
{
	CTreeCtrl& tc = GetTreeCtrl();
	BOOL fCheck = tc.GetCheck(hti);
	LayerTreeItem* lti = (LayerTreeItem*)tc.GetItemData(hti);
	if (lti)
		lti->SwitchCheckBox(!fCheck);
	tc.SetCheck(hti, !fCheck);
}

void LayerTreeView::OnExpanding(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	NMTREEVIEW* pnmtv = (NMTREEVIEW*) pNMHDR;
	HTREEITEM hItem = pnmtv->itemNew.hItem;

	String nodeName;
	for(map<String,NodeInfo>::iterator cur = nodes.begin(); cur != nodes.end(); ++cur) {
		if ( (*cur).second.hItem == hItem) {
			nodeName = (*cur).first;
			break;
		}
	}

	if ( nodeName == "")
		return;

	if ( pnmtv->action == TVE_EXPAND )
	{
		nodes[nodeName].expanded = true;
	}	
	else if ( pnmtv->action == TVE_COLLAPSE )
	{
		nodes[nodeName].expanded = false;
	}

	*pResult = 0;
}

void LayerTreeView::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LPNMTVCUSTOMDRAW lptvcd = (LPNMTVCUSTOMDRAW) pNMHDR;
	*pResult = 0;
	switch (lptvcd->nmcd.dwDrawStage) 
	{	
	case CDDS_PREPAINT:
		*pResult = CDRF_NOTIFYITEMDRAW;
		return;
	default: {
		*pResult = CDRF_DODEFAULT;
		LayerTreeItem* lti = (LayerTreeItem*) lptvcd->nmcd.lItemlParam;
		if (lti)
			lti->OnCustomDraw(pNMHDR, pResult);
			 }
	}
}

#define sMen(ID) ILWSF("men",ID).scVal()
#define pmadd(ID) men.AppendMenu(MF_STRING, ID, sMen(ID)); 

void LayerTreeView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	CPoint pt = point;
	ScreenToClient(&pt);
	CTreeCtrl& tc = GetTreeCtrl();	
	HTREEITEM hti = tc.HitTest(pt);
	if (0 != hti) {
		tc.SelectItem(hti);
		LayerTreeItem* lti = (LayerTreeItem*)tc.GetItemData(hti);
		if (lti) {
			lti->OnContextMenu(pWnd, point);
		}
		return;
	}
	CMenu men;
	men.CreatePopupMenu();
	pmadd(ID_ADDLAYER);
	pmadd(ID_ADD_GRID);
	pmadd(ID_ADD_GRATICULE);
	men.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, point.x, point.y, pWnd);
}

void LayerTreeView::OnRButtonDown(UINT nFlags, CPoint point)
{ // just here to prevent that default implementation 
	// prevents a WM_CONTEXTMENU to be sent
}

void LayerTreeView::EditNamedLayer(const FileName& fn)
{
	CFrameWnd* fw = GetTopLevelFrame();
	MapWindow* mw = dynamic_cast<MapWindow*>(fw);
	if (0 == mw)
		return;
	CView* vw = mw->vwFirst();
	MapPaneView* mv = dynamic_cast<MapPaneView*>(vw);
	if (0 == mv)
		return;
	mv->EditNamedLayer(fn);
}

DROPEFFECT LayerTreeView::OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
{
	if (!pDataObject->IsDataAvailable(CF_HDROP))
		return DROPEFFECT_NONE;
	HGLOBAL hnd = pDataObject->GetGlobalData(CF_HDROP);
	HDROP hDrop = (HDROP)GlobalLock(hnd);
	MapCompositionDoc* mcd = GetDocument();
	bool fOk = false;
	if (hDrop) {
		char sFileName[MAX_PATH+1];
		int iFiles = DragQueryFile(hDrop,	(UINT)-1, NULL, 0);
		for (int i = 0; i < iFiles; ++i) {
			DragQueryFile(hDrop, i, sFileName, MAX_PATH+1);
			FileName fn(sFileName);
			if (mcd->fAppendable(fn))	{
				fOk = true;
				break;
			}
		}
		GlobalUnlock(hDrop);
	}
	GlobalFree(hnd);
	if (fOk)
		return DROPEFFECT_COPY;
	else
		return DROPEFFECT_NONE;
}

void LayerTreeView::OnDragLeave() 
{
	// TODO: Add your specialized code here and/or call the base class
}

DROPEFFECT LayerTreeView::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
{
	return OnDragEnter(pDataObject, dwKeyState, point);
	//return CView::OnDragOver(pDataObject, dwKeyState, point);
}

BOOL LayerTreeView::OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point) 
{
	if (!pDataObject->IsDataAvailable(CF_HDROP))
		return FALSE;
	HGLOBAL hnd = pDataObject->GetGlobalData(CF_HDROP);
	HDROP hDrop = (HDROP)GlobalLock(hnd);
	int iFiles = 0;
	FileName* afn = 0;
	if (hDrop) {
		char sFileName[MAX_PATH+1];
		iFiles = DragQueryFile(hDrop,	(UINT)-1, NULL, 0);
		afn = new FileName[iFiles];
		for (int i = 0; i < iFiles; ++i) {
			DragQueryFile(hDrop, i, sFileName, MAX_PATH+1);
			afn[i] = FileName(sFileName);
		}
		GlobalUnlock(hDrop);
	}
	GlobalFree(hnd);
	// if send by SendMessage() prevent deadlock
	ReplyMessage(0);

	bool fOk = false;
	MapCompositionDoc* mcd = GetDocument();
	for (int i = 0; i < iFiles; ++i) {
		FileName fn = afn[i];
		if (!fn.fExist()) {
			FileName fnSys = fn;
			String sStdDir = IlwWinApp()->Context()->sStdDir();
			fnSys.Dir(sStdDir);
			if (fnSys.fExist())
				fn = fnSys;
		}
		mcd->drAppend(fn);
	}
	if (fOk)
		mcd->UpdateAllViews(0,2);
	return fOk;
}

void LayerTreeView::OnBeginDrag(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTREEVIEW nmtv = (LPNMTREEVIEW) pNMHDR;
	*pResult = 0;

	if (fDragging)
		return;
	LayerTreeItem* lti = (LayerTreeItem*) nmtv->itemNew.lParam;
	if (0 == lti)
		return;
	DrawerLayerTreeItem* dlti = dynamic_cast<DrawerLayerTreeItem*>(lti);
	if (0 == dlti)
		return;

	CTreeCtrl& tree = GetTreeCtrl();			
	tree.SetCapture();
	fDragging = true;			
	hDraggedItem = nmtv->itemNew.hItem;
	tree.Select(hDraggedItem, TVGN_CARET);
	pDragImageList = tree.CreateDragImage(hDraggedItem);
	pDragImageList->DragEnter(&tree, nmtv->ptDrag);
	pDragImageList->BeginDrag(0, CPoint(0, 0));
}

void LayerTreeView::OnMouseMove(UINT nFlags, CPoint point)
{
	if (fDragging)	
	{		
		CTreeCtrl& tree = GetTreeCtrl();
		pDragImageList->DragEnter(&tree, point);		
		pDragImageList->DragMove(point);
	}		
	CTreeView::OnMouseMove(nFlags, point);
}

void LayerTreeView::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (fDragging)	
	{		
		ReleaseCapture();		
		fDragging = false;
		pDragImageList->EndDrag();		
		delete pDragImageList;
		pDragImageList = NULL;		
		CTreeCtrl& tree = GetTreeCtrl();		
		UINT flags;
		HTREEITEM hti = tree.HitTest(point, &flags);
		DrawerLayerTreeItem* dlti = 0;

		enum { eNONE, eBEGIN, eEND, eITEM } eType = eNONE;
		if (flags & TVHT_ABOVE)
			eType = eBEGIN;
		else if (flags & TVHT_BELOW)
			eType = eEND;
		else if (flags & TVHT_NOWHERE)
			eType = eEND;
		else if (flags & 0x007e)
			eType = eITEM;

		if (eType == eNONE)
			return;

		if (eType == eITEM) {
			while (0 == dlti) {
				LayerTreeItem* lti = (LayerTreeItem*)tree.GetItemData(hti);
				dlti = dynamic_cast<DrawerLayerTreeItem*>(lti);
				if (0 == dlti) {
					hti = tree.GetParentItem(hti);
					if (hti == TVI_ROOT)
						return;
					if (0 == hti)
						return;
				}
			}
			if (hti == hDraggedItem)
				return;
		}

		LayerTreeItem* lti = (LayerTreeItem*)tree.GetItemData(hDraggedItem);
		DrawerLayerTreeItem* dltiDrag = dynamic_cast<DrawerLayerTreeItem*>(lti);
		NewDrawer* drDrag = dltiDrag->drw();
		MapCompositionDoc* mcd = GetDocument();	 
		mcd->ChangeState();
		mcd->rootDrawer->removeDrawer(drDrag->getId(),false);

		switch (eType) 
		{
		case eBEGIN:
			//mcd->dl.push_back(drDrag);
			break;
		case eEND:
			//mcd->dl.push_front(drDrag);
			break;
		case eITEM:
			{
				NewDrawer* drTarget = dlti->drw();
				int index = mcd->rootDrawer->getDrawerIndex(drTarget);
				mcd->rootDrawer->insertDrawer(index,drDrag);

			} break;
		}
		mcd->ChangeState();
		mcd->UpdateAllViews(0,2);
		hDraggedItem = NULL;	
	}	
	CTreeView::OnLButtonUp(nFlags, point);
}

void LayerTreeView::OnRemoveLayer()
{
	CTreeCtrl& tc = GetTreeCtrl();
	HTREEITEM hti = tc.GetSelectedItem();
	if (0 == hti)
		return;
	DrawerLayerTreeItem* dlti = 0;
	while (0 == dlti) {
		LayerTreeItem* lti = (LayerTreeItem*)tc.GetItemData(hti);
		dlti = dynamic_cast<DrawerLayerTreeItem*>(lti);
		if (0 == dlti) {
			hti = tc.GetParentItem(hti);
			if (hti == TVI_ROOT)
				return;
			if (0 == hti)
				return;
		}
	}
	NewDrawer* drw = dlti->drw();

	String str = drw->getName();
	String s(SMWMsgRemoveLayer_s.scVal(), str);
	int iRet = MessageBox(s.scVal(), SMWMsgRemoveLayer.scVal(), MB_YESNO|MB_ICONQUESTION);
	if (IDYES == iRet) {
		MapCompositionDoc* mcd = GetDocument();	 
		mcd->rootDrawer->removeDrawer(drw->getId());
		mcd->UpdateAllViews(0);
	}  
}

String LayerTreeView::getItemName(HTREEITEM item) const {
	for(map<String, NodeInfo>::const_iterator cur = nodes.begin(); cur != nodes.end(); ++cur) {
		if ( (*cur).second.hItem == item)
			return (*cur).first;
	}
	return sUNDEF;
}

HTREEITEM LayerTreeView::getItemHandle(const String& name) const {
	map<String, NodeInfo>::const_iterator cur = nodes.find(name);
	if ( cur != nodes.end())
		return (*cur).second.hItem;
	return 0;;
}


HTREEITEM LayerTreeView::getAncestor(HTREEITEM current, int depth) {
	String name = getItemName(current);
	while(depth != 0) {
		int index = name.find_last_of("|");
		name = name.sLeft(index);
		--depth;
	}
	for(map<String, NodeInfo>::const_iterator cur = nodes.begin(); cur != nodes.end(); ++cur) {
		if ( (*cur).first == name)
			return (*cur).second.hItem;
	}
	return 0;

}

bool LayerTreeView::getItem(HTREEITEM hItem, UINT mask, TreeItem& treeitem) const{
	treeitem.item.hItem = hItem;
	treeitem.item.mask = mask;
	BOOL ok =  GetTreeCtrl().GetItem(&(treeitem.item));
	return ok == TRUE;
}

ILWIS::DrawerTool *LayerTreeView::getRootTool() {
	return drwTool;
}

#ifdef _DEBUG
// prevent useless assertion errors
void LayerTreeView::AssertValid() const
{
}
#endif //_DEBUG




