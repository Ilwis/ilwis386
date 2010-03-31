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
// GraphLayerTreeView.cpp: implementation of the GraphLayerTreeView class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Client\TableWindow\GraphLayerTreeView.h"
#include "Client\TableWindow\GraphLayerTreeItem.h"
#include "Client\TableWindow\CartesianGraphDoc.h"
#include "Client\TableWindow\RoseDiagramDoc.h"
#include "Client\GraphWindow\GraphLayer.h"
#include "Client\GraphWindow\RoseDiagramLayer.h"
#include "Client\GraphWindow\GraphDrawer.h"
#include "Client\GraphWindow\GraphLegend.h"
#include "Client\ilwis.h"
#include "Headers\constant.h"
#include "Headers\Hs\Graph.hs"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


BEGIN_MESSAGE_MAP(GraphLayerTreeView, CTreeView)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
  ON_WM_MOUSEMOVE()
	ON_WM_KEYDOWN()
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
	ON_WM_DESTROY()
	ON_WM_RBUTTONDOWN()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_REMOVELAYER, OnRemoveLayer)
	ON_NOTIFY_REFLECT(TVN_BEGINDRAG, OnBeginDrag)
END_MESSAGE_MAP()



GraphLayerTreeView::GraphLayerTreeView()
{
	fDragging = false;
}

GraphLayerTreeView::~GraphLayerTreeView()
{
}

GraphDoc* GraphLayerTreeView::GetDocument()
{
  return (GraphDoc*)m_pDocument;
}

void GraphLayerTreeView::OnDestroy()
{
	DeleteAllItems();
	CTreeView::OnDestroy();
}

BOOL GraphLayerTreeView::PreTranslateMessage(MSG* pMsg)
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
	return CTreeView::PreTranslateMessage(pMsg);
}

void GraphLayerTreeView::DeleteAllItems()
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

void GraphLayerTreeView::DeleteAllItems(HTREEITEM hti)
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
	GraphTreeItem* gti = (GraphTreeItem*)tc.GetItemData(hti);
  if (gti)
		delete gti;
}

void GraphLayerTreeView::OnInitialUpdate() 
{
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

void GraphLayerTreeView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	if (!IsWindow(m_hWnd))
		return;
	CTreeCtrl& tc = GetTreeCtrl();

	tc.SetRedraw(FALSE);
	DeleteAllItems();

	int iImgBase = IlwWinApp()->iImage(".grh");
	htiGraph = tc.InsertItem("Graph",iImgBase,iImgBase,TVI_ROOT,TVI_FIRST);
	GraphDoc* gd = GetDocument();
  GraphDrawer* grdr = gd->grdrGet();
  if (0 == grdr)
    return;

  int iImgAxis = IlwWinApp()->iImage("Axis");
  int iImgText = IlwWinApp()->iImage("Text");
  int iImgMinMax = IlwWinApp()->iImage("ScaleBar");
	int iImgTable = IlwWinApp()->iImage(".tbt");
  int iImgLegend = IlwWinApp()->iImage("Legend");

  HTREEITEM hti = tc.InsertItem(grdr->sTitle.scVal(),iImgText,iImgText,htiGraph);
  tc.SetItemData(hti, (DWORD_PTR)new GraphTitleTreeItem(this, grdr));		
  hti = tc.InsertItem(SGPRemLegend.scVal(),iImgLegend,iImgLegend,htiGraph);
  tc.SetItemData(hti, (DWORD_PTR)new BoolGraphTreeItem(this, &grdr->grleg->fShow));		
	tc.SetCheck(hti, grdr->grleg->fShow);

  if (gd->table().fValid()) 
  {
    Table tbl = gd->table();
		String sName = tbl->sName();
		String sDescr = tbl->sDescr();
	  if ("" != sDescr) 
			sName = String("%S - %S", sName, sDescr);
		HTREEITEM htiTbl = tc.InsertItem(sName.scVal(), iImgTable, iImgTable, htiGraph);
    if (0 == htiTbl)
      return;
    tc.SetItemData(htiTbl, (DWORD_PTR)new ObjectGraphTreeItem(this, tbl.ptr()));		
  }

  CartesianGraphDoc* cgd = dynamic_cast<CartesianGraphDoc*>(gd);
  RoseDiagramDoc* rdd = dynamic_cast<RoseDiagramDoc*>(gd);
  if (cgd) 
  {
    GraphAxis* ga = cgd->ga(GraphAxis::gapX);
    if (0 == ga)
      return;
  	HTREEITEM htiAxis = tc.InsertItem("X-Axis",iImgAxis,iImgAxis,htiGraph);
    if (0 == htiAxis)
      return;
    tc.SetItemData(htiAxis, (DWORD_PTR)new GraphAxisTreeItem(this, ga));		
    HTREEITEM hti = tc.InsertItem(ga->sTitle.scVal(),iImgText,iImgText,htiAxis);
    tc.SetItemData(hti, (DWORD_PTR)new StringGraphTreeItem(this, hti, &ga->sTitle));	
    String sMin, sMax;
    if (ga->dvrs.fValues())
    {
      sMin = ga->sText(ga->rMin(), false);
      hti = tc.InsertItem(sMin.scVal(),iImgMinMax,iImgMinMax,htiAxis);
      tc.SetItemData(hti, (DWORD_PTR)new GraphAxisMinTreeItem(this, hti, ga));		
      sMax = ga->sText(ga->rMax(), false);
      hti = hti = tc.InsertItem(sMax.scVal(),iImgMinMax,iImgMinMax,htiAxis);
      tc.SetItemData(hti, (DWORD_PTR)new GraphAxisMaxTreeItem(this, hti, ga));		
    }
  	tc.Expand(htiAxis, TVE_EXPAND);

    ga = cgd->ga(GraphAxis::gapYLeft);
    htiAxis = tc.InsertItem("Y-Axis Left",iImgAxis,iImgAxis,htiGraph);
    if (0 == htiAxis)
      return;
    tc.SetItemData(htiAxis, (DWORD_PTR)new GraphAxisTreeItem(this, ga));		
    hti = tc.InsertItem(ga->sTitle.scVal(),iImgText,iImgText,htiAxis);
    tc.SetItemData(hti, (DWORD_PTR)new StringGraphTreeItem(this, hti, &ga->sTitle));		
    sMin = ga->sText(ga->rMin(), false);
    hti = tc.InsertItem(sMin.scVal(),iImgMinMax,iImgMinMax,htiAxis);
    tc.SetItemData(hti, (DWORD_PTR)new GraphAxisMinTreeItem(this, hti, ga));		
    sMax = ga->sText(ga->rMax(), false);
    hti = tc.InsertItem(sMax.scVal(),iImgMinMax,iImgMinMax,htiAxis);
    tc.SetItemData(hti, (DWORD_PTR)new GraphAxisMaxTreeItem(this, hti, ga));		
  	tc.Expand(htiAxis, TVE_EXPAND);

    ga = cgd->ga(GraphAxis::gapYRight);
    htiAxis = tc.InsertItem("Y-Axis Right",iImgAxis,iImgAxis,htiGraph);
    if (0 == htiAxis)
      return;
    GraphAxisTreeItem* gati = new GraphAxisTreeItem(this, ga);
    gati->fCheckBox = true;
    tc.SetItemData(htiAxis, (DWORD_PTR)gati);		
		tc.SetCheck(htiAxis, ga->fVisible);
    hti = tc.InsertItem(ga->sTitle.scVal(),iImgText,iImgText,htiAxis);
    tc.SetItemData(hti, (DWORD_PTR)new StringGraphTreeItem(this, hti, &ga->sTitle));	
    sMin = ga->sText(ga->rMin(), false);
    hti = tc.InsertItem(sMin.scVal(),iImgMinMax,iImgMinMax,htiAxis);
    tc.SetItemData(hti, (DWORD_PTR)new GraphAxisMinTreeItem(this, hti, ga));		
    sMax = ga->sText(ga->rMax(), false);
    hti = tc.InsertItem(sMax.scVal(),iImgMinMax,iImgMinMax,htiAxis);
    tc.SetItemData(hti, (DWORD_PTR)new GraphAxisMaxTreeItem(this, hti, ga));		
  	tc.Expand(htiAxis, TVE_EXPAND);
  }
  else if (rdd) 
  {
    GraphAxis* ga = rdd->ga(GraphAxis::gapXRose);
    if (0 == ga)
      return;
  	HTREEITEM htiAxis = tc.InsertItem("X-Axis",iImgAxis,iImgAxis,htiGraph);
    if (0 == htiAxis)
      return;
    HTREEITEM hti;
    tc.SetItemData(htiAxis, (DWORD_PTR)new GraphAxisTreeItem(this, ga));		
  	tc.Expand(htiAxis, TVE_EXPAND);

    ga = rdd->ga(GraphAxis::gapYRose);
    htiAxis = tc.InsertItem("Y-Axis",iImgAxis,iImgAxis,htiGraph);
    if (0 == htiAxis)
      return;
    tc.SetItemData(htiAxis, (DWORD_PTR)new GraphAxisTreeItem(this, ga));		
    String sMin = ga->sText(ga->rMin(), false);
    hti = tc.InsertItem(sMin.scVal(),iImgMinMax,iImgMinMax,htiAxis);
    tc.SetItemData(hti, (DWORD_PTR)new GraphAxisMinTreeItem(this, hti, ga));		
    String sMax = ga->sText(ga->rMax(), false);
    hti = tc.InsertItem(sMax.scVal(),iImgMinMax,iImgMinMax,htiAxis);
    tc.SetItemData(hti, (DWORD_PTR)new GraphAxisMaxTreeItem(this, hti, ga));		
  	tc.Expand(htiAxis, TVE_EXPAND);
  }
  
	int iImgGraph = IlwWinApp()->iImage("Graph");
  const int iLayers = gd->iLayers();
  for (int iLayer = iLayers-1; iLayer >= 0; --iLayer)
  {
    GraphLayer* gl = gd->gl(iLayer);
    String sName = gl->sTitle;
		HTREEITEM hti = tc.InsertItem(sName.scVal(),iImgGraph,iImgGraph,htiGraph);
    if (0 == hti)
      return;
    GraphLayerTreeItem* glti = new GraphLayerTreeItem(this, gl);
    glti->fCheckBox = true;
		tc.SetItemData(hti, (DWORD_PTR)glti);	
		tc.SetCheck(hti, gl->fShow);
    ColumnGraphLayer* cgl = dynamic_cast<ColumnGraphLayer*>(gl);
    RoseDiagramLayer* rdl = dynamic_cast<RoseDiagramLayer*>(gl);
    if (0 != cgl) {
      if (gd->table() != cgl->tbl) {
				sName = cgl->tbl->sName();
				String sDescr = cgl->tbl->sDescr();
				if ("" != sDescr) 
					sName = String("%S - %S", sName, sDescr);
				HTREEITEM htiTbl = tc.InsertItem(sName.scVal(), iImgTable, iImgTable, hti);
        if (0 == htiTbl)
          return;
        tc.SetItemData(htiTbl, (DWORD_PTR)new ObjectGraphTreeItem(this, cgl->tbl.ptr()));		
      }
			int iImgCol = IlwWinApp()->iImage("column");
      if (cgl->colX.fValid()) {
        sName = cgl->colX->sName();
  			String sDescr = cgl->colX->sDescr();
	  		if ("" != sDescr) 
		  		sName = String("%S - %S", sName, sDescr);
				tc.InsertItem(sName.scVal(), iImgCol, iImgCol, hti);
      }
      if (cgl->colY.fValid()) {
        sName = cgl->colY->sName();
  			String sDescr = cgl->colY->sDescr();
	  		if ("" != sDescr) 
		  		sName = String("%S - %S", sName, sDescr);
  			tc.InsertItem(sName.scVal(), iImgCol, iImgCol, hti);
      }
    }
    else if (0 != rdl) {
      if (gd->table() != rdl->tbl) {
				sName = rdl->tbl->sName();
				String sDescr = rdl->tbl->sDescr();
				if ("" != sDescr) 
					sName = String("%S - %S", sName, sDescr);
				int iImg = IlwWinApp()->iImage(".tbt");
				HTREEITEM htiTbl = tc.InsertItem(sName.scVal(), iImg, iImg, hti);
        if (0 == htiTbl)
          return;
      }
			int iImgCol = IlwWinApp()->iImage("column");
      if (rdl->colX.fValid()) {
        sName = rdl->colX->sName();
				String sDescr = rdl->colX->sDescr();
				if ("" != sDescr) 
					sName = String("%S - %S", sName, sDescr);
				tc.InsertItem(sName.scVal(), iImgCol, iImgCol, hti);
      }
      if (rdl->colY.fValid()) {
        sName = rdl->colY->sName();
			  String sDescr = rdl->colY->sDescr();
			  if ("" != sDescr) 
				  sName = String("%S - %S", sName, sDescr);
			  tc.InsertItem(sName.scVal(), iImgCol, iImgCol, hti);
      }        
    }
    else {
  		sName = gl->sName();
			tc.InsertItem(sName.scVal(), iImgGraph, iImgGraph, hti);
    }
  	tc.Expand(hti, TVE_EXPAND);
  }
	tc.Expand(htiGraph, TVE_EXPAND);
	tc.SetRedraw(TRUE);
	Invalidate();
}

void GraphLayerTreeView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CTreeCtrl& tc = GetTreeCtrl();
	UINT uFlags=0;	
	HTREEITEM hti = tc.HitTest(point,&uFlags);
	if(uFlags & TVHT_ONITEMSTATEICON)	
		SwitchCheckBox(hti);
	else {
		CTreeView::OnLButtonDown(nFlags, point);
		GraphTreeItem* gti = 0;
		if (hti)
			gti = (GraphTreeItem*)tc.GetItemData(hti);
		if (gti)
			gti->OnLButtonDown(nFlags, point);
	}
}

void GraphLayerTreeView::OnLButtonUp(UINT nFlags, CPoint point)
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
		GraphLayerTreeItem* glti = 0;

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
			while (0 == glti) {
				GraphTreeItem* gti = (GraphTreeItem*)tree.GetItemData(hti);
				glti = dynamic_cast<GraphLayerTreeItem*>(gti);
				if (0 == glti) {
					hti = tree.GetParentItem(hti);
          if (hti == TVI_ROOT || 0 == hti) {
            eType = eBEGIN;
            break;
          }
				}
			}
			if (hti == hDraggedItem)
				return;
		}
		
		GraphTreeItem* gti = (GraphTreeItem*)tree.GetItemData(hDraggedItem);
		GraphLayerTreeItem* gltiDrag = dynamic_cast<GraphLayerTreeItem*>(gti);
		GraphLayer* glDrag = gltiDrag->glGet();

		GraphDoc* gd = GetDocument();	 
    GraphDrawer* grdr = gd->grdrGet();
		Array<GraphLayer*>::iterator iter = find(grdr->agl.begin(), grdr->agl.end(), glDrag);
    grdr->agl.erase(iter);

		switch (eType) 
		{
			case eBEGIN:
        grdr->agl.push_back(glDrag);
				break;
			case eEND:
				grdr->agl.insert(grdr->agl.begin(), glDrag);
				break;
			case eITEM:
			{
				GraphLayer* glTarget = glti->glGet();
    		iter = find(grdr->agl.begin(), grdr->agl.end(), glTarget);
				grdr->agl.insert(iter, glDrag);
			} break;
		}
    gd->SetModifiedFlag();
		gd->UpdateAllViews(0);
		hDraggedItem = NULL;	
	}	
	CTreeView::OnLButtonUp(nFlags, point);
}

void GraphLayerTreeView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	CTreeCtrl& tc = GetTreeCtrl();
	UINT uFlags=0;	
	HTREEITEM hti = tc.HitTest(point,&uFlags);
	GraphTreeItem* gti = 0;
	if (hti)
		gti = (GraphTreeItem*)tc.GetItemData(hti);
	if (gti)
		gti->OnLButtonDblClk(nFlags, point);
}

void GraphLayerTreeView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
  switch (nChar)
  {
    case VK_SPACE:
    {
    	CTreeCtrl& tc = GetTreeCtrl();
	  	HTREEITEM hti = tc.GetSelectedItem();
      if (0 == hti)
        return;
	    SwitchCheckBox(hti);
      return;
    } 
    case VK_RETURN:
    {
    	CTreeCtrl& tc = GetTreeCtrl();
	  	HTREEITEM hti = tc.GetSelectedItem();
      if (0 == hti)
        return;
    	GraphTreeItem* gti = (GraphTreeItem*)tc.GetItemData(hti);
      if (0 == gti)
        return;
      gti->OnEdit();
      return;
    } 
    default:
  		CTreeView::OnKeyDown(nChar, nRepCnt, nFlags);
  }
}

void GraphLayerTreeView::SwitchCheckBox(HTREEITEM hti)
{
	CTreeCtrl& tc = GetTreeCtrl();
	BOOL fCheck = tc.GetCheck(hti);
	GraphTreeItem* gti = (GraphTreeItem*)tc.GetItemData(hti);
  if (gti && gti->fCheckBox) 
  {
		gti->SwitchCheckBox(!fCheck);
  	tc.SetCheck(hti, !fCheck);
  }
}

void GraphLayerTreeView::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult) 
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
			GraphTreeItem* gti = (GraphTreeItem*) lptvcd->nmcd.lItemlParam;
			if (gti)
				gti->OnCustomDraw(pNMHDR, pResult);
		}
	}
}

#define sMen(ID) ILWSF("men",ID).scVal()
#define pmadd(ID) men.AppendMenu(MF_STRING, ID, sMen(ID)); 
#define addSub(ID) menSub.AppendMenu(MF_STRING, ID, sMen(ID)); 
#define addSubMenu(ID) men.AppendMenu(MF_POPUP, (UINT)menSub.GetSafeHmenu(), sMen(ID)); menSub.Detach();

void GraphLayerTreeView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	CPoint pt = point;
	ScreenToClient(&pt);
	CTreeCtrl& tc = GetTreeCtrl();	
  UINT uFlags;
	HTREEITEM hti = tc.HitTest(pt, &uFlags);
	if (0 != hti && htiGraph != hti) 
  {
		GraphTreeItem* gti = (GraphTreeItem*)tc.GetItemData(hti);
		if (gti)
    {
  		tc.SelectItem(hti);
      if (gti->OnContextMenu(pWnd, point))
        return;
    }
    // only if click at the right of the item drop through to show the general menu
    if (0 == (uFlags & TVHT_ONITEMRIGHT)) 
      return;
	}
	GraphDoc* gd = GetDocument();
  CartesianGraphDoc* cgd = dynamic_cast<CartesianGraphDoc*>(gd);
  RoseDiagramDoc* rdd = dynamic_cast<RoseDiagramDoc*>(gd);
	CMenu men, menSub;
	men.CreatePopupMenu();
  if (cgd && cgd->fEnableAddFormulaGraph()) 
  {
    menSub.CreateMenu();
		addSub(ID_GRPH_ADD_COLUMN);
		addSub(ID_GRPH_ADD_FORMULA);
		addSub(ID_GRPH_ADD_LSF);
		addSub(ID_GRPH_ADD_SVM);
    addSubMenu(ID_GRPH_ADD);
  }
  else {
    pmadd(ID_GRPH_ADD)
  }
  men.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, point.x, point.y, pWnd);
}

void GraphLayerTreeView::OnRButtonDown(UINT nFlags, CPoint point)
{ // just here to prevent that default implementation 
	// prevents a WM_CONTEXTMENU to be sent
}

void GraphLayerTreeView::OnRemoveLayer()
{
	CTreeCtrl& tc = GetTreeCtrl();
	HTREEITEM hti = tc.GetSelectedItem();
	if (0 == hti)
		return;
	GraphLayerTreeItem* glti = 0;
	while (0 == glti) {
		GraphTreeItem* gti = (GraphTreeItem*)tc.GetItemData(hti);
		glti = dynamic_cast<GraphLayerTreeItem*>(gti);
		if (0 == glti) {
			hti = tc.GetParentItem(hti);
			if (hti == TVI_ROOT)
				return;
			if (0 == hti)
				return;
		}
	}
  glti->Remove();
}

void GraphLayerTreeView::OnBeginDrag(NMHDR* pNMHDR, LRESULT* pResult)
{
  LPNMTREEVIEW nmtv = (LPNMTREEVIEW) pNMHDR;
	*pResult = 0;

	if (fDragging)
		return;
	GraphTreeItem* gti = (GraphTreeItem*) nmtv->itemNew.lParam;
	if (0 == gti)
		return;
	GraphLayerTreeItem* glti = dynamic_cast<GraphLayerTreeItem*>(gti);
	if (0 == glti)
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

void GraphLayerTreeView::OnMouseMove(UINT nFlags, CPoint point)
{
	if (fDragging)	
	{		
		CTreeCtrl& tree = GetTreeCtrl();
		pDragImageList->DragEnter(&tree, point);		
		pDragImageList->DragMove(point);
	}		
	CTreeView::OnMouseMove(nFlags, point);
}

