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
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\Positioner.h"
#include "Client\Mapwindow\Drawers\BaseDrawer.h"
#include "Client\Mapwindow\Drawers\Drawer.h"
#include "Engine\SpatialReference\GR3D.H"
#include "Client\Mapwindow\Drawers\Grid3DDrawer.h"
#include "Headers\Hs\Drwforms.hs"
#include "Client\FormElements\fldcol.h"
#include "Client\FormElements\fldrpr.h"
#include "Client\FormElements\fldcolor.h"
#include "Client\Mapwindow\Drawers\BaseMapDrawer.h"
#include "Client\Mapwindow\Drawers\MapDrawer.h"
#include "Client\Mapwindow\Drawers\drawer_n.h"
#include "Client\Mapwindow\Drawers\AbstractObjectdrawer.h"
#include "Client\Mapwindow\Drawers\AbstractMapDrawer.h" 
#include "Client\Mapwindow\LayerTreeItem.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Engine\Domain\dmclass.h"
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
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

LayerTreeView::LayerTreeView()
{
	odt = new COleDropTarget;
	fDragging = false;
}

LayerTreeView::~LayerTreeView()
{
	delete odt;
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
	}
}

void LayerTreeView::DeleteAllItems(HTREEITEM hti)
{
	CTreeCtrl& tc = GetTreeCtrl();
	if (tc.ItemHasChildren(hti)) {
		HTREEITEM htiChild = tc.GetChildItem(hti);
		while (htiChild) 
		{
			DeleteAllItems(htiChild);
			HTREEITEM htiNext = tc.GetNextSiblingItem(htiChild);
			tc.DeleteItem(htiChild);
			htiChild = htiNext;
		}
	}
	LayerTreeItem* lti = (LayerTreeItem*)tc.GetItemData(hti);
	if (lti)
		delete lti;
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

	int iImgProp = IlwWinApp()->iImage("prop");

	MapCompositionDoc* mcd = GetDocument();
	int drCount = mcd->rootDrawer->getDrawerCount();
	for(int index = 0; index < drCount; ++index)
	//for (list<Drawer*>::iterator iter = mcd->dl.begin(); iter != mcd->dl.end(); ++iter) 
	{
		AbstractDrawer* dr = (AbstractDrawer *)mcd->rootDrawer->getDrawer(index);
		AbstractMapDrawer *adr = dynamic_cast<AbstractMapDrawer *>(dr);
		ILWISSingleLock csl(&dr->cs, TRUE, SOURCE_LOCATION);
		FileName fn;
		if ( adr)
			fn = adr->getObject()->fnObj;
		String sName = fn.sShortName(false);
		BaseMap bmp = adr->getBaseMap();
		if (bmp.fValid()) {
			String sDescr = bmp->sDescr();
			if ("" != sDescr) 
				sName = String("%S - %S", sName, sDescr);
		}

		int iImg = IlwWinApp()->iImage(fn.sExt);
		HTREEITEM hti = tc.InsertItem(sName.scVal(),iImg,iImg,TVI_ROOT,TVI_FIRST);
		if (0 == hti)
			return;
		tc.SetItemData(hti, (DWORD_PTR)new DrawerLayerTreeItem(this, dr));		
		tc.SetCheck(hti, dr->isActive());

		if (adr == 0) 
			continue;
		Domain dm;
		dm = bmp->dm();
		if (adr) {
			sName = String("Display options");
			iImg = IlwWinApp()->iImage(".mpv");
			HTREEITEM htiDisplayOptions = tc.InsertItem(sName.scVal(), iImg, iImg, hti);
			dr->configure(this, htiDisplayOptions);
			tc.SetItemData(htiDisplayOptions, (DWORD_PTR)new DisplayOptionTree(this,htiDisplayOptions));

			sName = "Properties";
			HTREEITEM htiProp = tc.InsertItem(sName.scVal(), iImgProp, iImgProp, hti);
			if (0 == htiProp)
				return;
			tc.SetItemData(htiProp, (DWORD_PTR)new PropertiesLayerTreeItem(this, bmp.ptr()));		

			if (bmp.fValid()) {

				sName = String("%S - %S", dm->sName(), dm->sType());
				iImg = IlwWinApp()->iImage(".dom");
				HTREEITEM htiDom = tc.InsertItem(sName.scVal(), iImg, iImg, htiProp);
				if (0 == htiDom)
					return;

				tc.SetItemData(htiDom, (DWORD_PTR)new ObjectLayerTreeItem(this, dm.pointer()));		

				Representation rpr = adr->getRepresentation();
				if (rpr.fValid()) {
					sName = String("%S - %S", rpr->sName(), rpr->sType());
					iImg = IlwWinApp()->iImage(".rpr");
					HTREEITEM htiRpr = tc.InsertItem(sName.scVal(), iImg, iImg, htiDom);
					if (0 == htiRpr)
						return;
					tc.SetItemData(htiRpr, (DWORD_PTR)new ObjectLayerTreeItem(this, rpr.pointer()));		
				}
				HTREEITEM htiGrf = htiProp;
				MapPtr* mp = dynamic_cast<MapPtr*>(bmp.ptr());
				if (mp) {
					GeoRef grf =  mp->gr();
					sName = String("%S - %S", grf->sName(), grf->sType());
					iImg = IlwWinApp()->iImage(".grf");
					htiGrf = tc.InsertItem(sName.scVal(), iImg, iImg, htiProp);
					if (0 == htiGrf)
						return;

					tc.SetItemData(htiGrf, (DWORD_PTR)new ObjectLayerTreeItem(this, grf.pointer()));		
				}
				CoordSystem csy = bmp->cs();
				sName = String("%S - %S", csy->sName(), csy->sType());
				iImg = IlwWinApp()->iImage(".csy");
				HTREEITEM htiCsy = tc.InsertItem(sName.scVal(), iImg, iImg, htiGrf);
				if (0 == htiCsy)
					return;
				//				AddPropItems(htiCsy, iImg, csy);
				tc.SetItemData(htiCsy, (DWORD_PTR)new ObjectLayerTreeItem(this, csy.pointer()));		
				//tc.SetItemData(htiCsy, (DWORD)csy.pointer());		
				//tc.InsertItem(csy->sDescr().scVal(), iImgProp, iImgProp, htiCsy);

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
							return;
						//						AddPropItems(htiTbl, iImg, tbl);
						tc.SetItemData(htiTbl, (DWORD_PTR)new ObjectLayerTreeItem(this, tbl.pointer()));		
					}
				}
			}
			//MapListPtr* mpl = dynamic_cast<MapListPtr*>(obp);
			//if (mpl) {
			//	HTREEITEM htiGrf = htiProp;
			//	GeoRef grf =  mpl->gr();
			//	sName = String("%S - %S", grf->sName(), grf->sType());
			//	iImg = IlwWinApp()->iImage(".grf");
			//	htiGrf = tc.InsertItem(sName.scVal(), iImg, iImg, htiProp);
			//	if (0 == htiGrf)
			//		return;
			//	//				AddPropItems(htiGrf, iImg, grf);
			//	tc.SetItemData(htiGrf, (DWORD_PTR)new ObjectLayerTreeItem(this, grf.pointer()));		
			//	//tc.SetItemData(htiGrf, (DWORD)grf.pointer());		
			//	//tc.InsertItem(grf->sDescr().scVal(), iImgProp, iImgProp, htiGrf);
			//	CoordSystem csy = grf->cs();
			//	sName = String("%S - %S", csy->sName(), csy->sType());
			//	iImg = IlwWinApp()->iImage(".csy");
			//	HTREEITEM htiCsy = tc.InsertItem(sName.scVal(), iImg, iImg, htiGrf);
			//	if (0 == htiCsy)
			//		return;
			//	//				AddPropItems(htiCsy, iImg, csy);
			//	tc.SetItemData(htiCsy, (DWORD_PTR)new ObjectLayerTreeItem(this, csy.pointer()));		
			//}
		}

		bool fLegend = adr->isLegendUsefull();
		if (fLegend) {
			if (dm->pdc()) {
				sName = SDCRemLegend;
				int iImgLeg = IlwWinApp()->iImage("legend");
				HTREEITEM htiLeg = tc.InsertItem(sName.scVal(), iImgLeg, iImgLeg, hti);
				if (0 == htiLeg)
					return;
				tc.SetItemData(htiLeg, (DWORD_PTR)new LegendLayerTreeItem(this, dr));		
				DomainClass* dc = dm->pdc();
				int iItems = dc->iNettoSize();
				for (int i = 1; i <= iItems; ++i) {
					int iRaw = dc->iKey(i);
					String sName = dc->sValueByRaw(iRaw, 0);
					HTREEITEM hti = tc.InsertItem(sName.scVal(), htiLeg);
					tc.SetItemData(hti, (DWORD_PTR)new LegendClassLayerTreeItem(this, dr, dm, iRaw));		
				}
				tc.Expand(htiLeg, TVE_EXPAND);
			}
			else if (dm->pdbool()) {
				sName = SDCRemLegend;
				int iImgLeg = IlwWinApp()->iImage("legend");
				HTREEITEM htiLeg = tc.InsertItem(sName.scVal(), iImgLeg, iImgLeg, hti);
				if (0 == htiLeg)
					return;
				int iItems = 2;
				for (int i = 2; i > 0; --i) {
					int iRaw = i;
					String sName = dm->sValueByRaw(iRaw, 0);
					HTREEITEM hti = tc.InsertItem(sName.scVal(), htiLeg);
					tc.SetItemData(hti, (DWORD_PTR)new LegendClassLayerTreeItem(this, dr, dm, iRaw));		
				}
				tc.Expand(htiLeg, TVE_EXPAND);
			}
			else if (dm->pdid()) {
				DomainSort* ds = dm->pdsrt();
				int iItems = ds->iNettoSize();
				if (iItems <= 128 && dr->getDrawMethod() == NewDrawer::drmMULTIPLE) {
					sName = SDCRemLegend;
					int iImgLeg = IlwWinApp()->iImage("legend");
					HTREEITEM htiLeg = tc.InsertItem(sName.scVal(), iImgLeg, iImgLeg, hti);
					if (0 == htiLeg)
						return;
					for (int i = 1; i <= iItems; ++i) {
						int iRaw = ds->iKey(i);
						String sName = ds->sValueByRaw(iRaw, 0);
						HTREEITEM hti = tc.InsertItem(sName.scVal(), htiLeg);
						tc.SetItemData(hti, (DWORD_PTR)new LegendClassLayerTreeItem(this, dr, dm, iRaw));		
					}
				}
				//tc.Expand(htiLeg, TVE_EXPAND);
			}
			else if (dm->pdi()) {
				sName = SDCRemLegend;
				int iImgLeg = IlwWinApp()->iImage("legend");
				HTREEITEM htiLeg = tc.InsertItem(sName.scVal(), iImgLeg, iImgLeg, hti);
				if (0 == htiLeg)
					return;
				Representation rpr = adr->getRepresentation();
				tc.SetItemData(htiLeg, (DWORD_PTR)new ObjectLayerTreeItem(this, rpr.pointer()));		
				DomainValueRangeStruct dvs = bmp->dvrs();					
				RangeReal rr = adr->getStretchRange();
				int iItems = 5;
				for (int i = 0; i < iItems; ++i) {
					double rMaxItem = iItems - 1;
					long iVal = rr.rLo() + i / rMaxItem * rr.rWidth();
					String sName = dvs.sValue(iVal, 0);
					HTREEITEM hti = tc.InsertItem(sName.scVal(), htiLeg);
					tc.SetItemData(hti, (DWORD_PTR)new LegendClassLayerTreeItem(this, dr, dm, iVal));		
				}
				tc.Expand(htiLeg, TVE_EXPAND);
			}
			else if (dm->pdbit()) {
				sName = SDCRemLegend;
				int iImgLeg = IlwWinApp()->iImage("legend");
				HTREEITEM htiLeg = tc.InsertItem(sName.scVal(), iImgLeg, iImgLeg, hti);
				if (0 == htiLeg)
					return;
				HTREEITEM hti = tc.InsertItem("0", htiLeg);
				tc.SetItemData(hti, (DWORD_PTR)new LegendClassLayerTreeItem(this, dr, dm, iUNDEF));		
				hti = tc.InsertItem("1", htiLeg);
				tc.SetItemData(hti, (DWORD_PTR)new LegendClassLayerTreeItem(this, dr, dm, 1));		
				tc.Expand(htiLeg, TVE_EXPAND);
			}
			else if (dm->pdv()) {
				sName = SDCRemLegend;
				int iImgLeg = IlwWinApp()->iImage("legend");
				HTREEITEM htiLeg = tc.InsertItem(sName.scVal(), iImgLeg, iImgLeg, hti);
				if (0 == htiLeg)
					return;
				Representation rpr = adr->getRepresentation();
				tc.SetItemData(htiLeg, (DWORD_PTR)new ObjectLayerTreeItem(this, rpr.pointer()));
				DomainValueRangeStruct dvs = bmp->dvrs() ;					
				RangeReal rr = adr->getLegendRange();
				int iItems = 5;
				double rStep = dvs.rStep();
				if (rStep > 1e-6) {
					int iSteps = 1 + round(rr.rWidth() / rStep);
					if (iSteps < 2)
						iSteps = 2;
					if (iSteps <= 11)
						iItems = iSteps;
				}
				for (int i = 0; i < iItems; ++i) {
					double rMaxItem = iItems - 1;
					double rVal = rr.rLo() + i / rMaxItem * rr.rWidth();
					String sName = dvs.sValue(rVal, 0);
					HTREEITEM hti = tc.InsertItem(sName.scVal(), htiLeg);
					tc.SetItemData(hti, (DWORD_PTR)new LegendValueLayerTreeItem(this, adr, dvs, rVal));		
				}
				tc.Expand(htiLeg, TVE_EXPAND);
			}
		}

		tc.Expand(hti, TVE_EXPAND);
	}

	tc.SetRedraw(TRUE);
	Invalidate();
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
}

void LayerTreeView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CTreeCtrl& tc = GetTreeCtrl();
	UINT uFlags=0;	
	HTREEITEM hti = tc.HitTest(point,&uFlags);
	if(uFlags & TVHT_ONITEMSTATEICON)	
		SwitchCheckBox(hti);
	else {
		CTreeView::OnLButtonDown(nFlags, point);
		LayerTreeItem* lti = 0;
		if (hti)
			lti = (LayerTreeItem*)tc.GetItemData(hti);
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
		Drawer* dr = mcd->drAppend(fn);
		// configure new drawer (option: no show?)
		if (dr) {
			if (dr->Configure()) 
				fOk = true;
			else
				dr->fAct = false;
		}
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

	/*	LayerTreeItem* lti = (LayerTreeItem*)tree.GetItemData(hDraggedItem);
		DrawerLayerTreeItem* dltiDrag = dynamic_cast<DrawerLayerTreeItem*>(lti);
		NewDrawer* drDrag = dltiDrag->drw();

		MapCompositionDoc* mcd = GetDocument();	 
		mcd->ChangeState();
		mcd->rootDrawer->removeDrawer(drDrag->getId());

		switch (eType) 
		{
		case eBEGIN:
			mcd->dl.push_back(drDrag);
			break;
		case eEND:
			mcd->dl.push_front(drDrag);
			break;
		case eITEM:
			{
				NewDrawer* drTarget = dlti->drw();
				list<Drawer*>::iterator iter = find(mcd->dl.begin(), mcd->dl.end(), drTarget);
				mcd->dl.insert(iter, drDrag);
			} break;
		}
		mcd->ChangeState();
		mcd->UpdateAllViews(0,2);
		hDraggedItem = NULL;	*/
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

#ifdef _DEBUG
// prevent useless assertion errors
void LayerTreeView::AssertValid() const
{
}
#endif //_DEBUG




