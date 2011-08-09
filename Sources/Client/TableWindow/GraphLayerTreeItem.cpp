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
// GraphLayerTreeItem.cpp: implementation of the GraphLayerTreeItem class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Client\TableWindow\GraphLayerTreeItem.h"
#include "Client\TableWindow\GraphLayerTreeView.h"
#include "Client\TableWindow\GraphTreeField.h"
#include "Client\TableWindow\GraphDoc.h"
#include "Client\GraphWindow\GraphAxis.h"
#include "Client\GraphWindow\GraphLayer.h"
#include "Client\GraphWindow\GraphDrawer.h"
#include "Headers\constant.h"
#include "Headers\Hs\Graph.hs"
#include "Client\FormElements\syscolor.h"
#include "Client\ilwis.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define sMen(ID) ILWSF("men",ID).c_str()
#define pmadd(ID) men.AppendMenu(MF_STRING, ID, sMen(ID)); 


GraphTreeItem::GraphTreeItem(GraphLayerTreeView* ltview) 
: ltv(ltview), fCheckBox(false)
{
}

GraphTreeItem::~GraphTreeItem()
{
}

void GraphTreeItem::OnLButtonDown(UINT nFlags, CPoint point)
{
}

void GraphTreeItem::OnLButtonDblClk(UINT nFlags, CPoint point)
{
  OnEdit();
}

void GraphTreeItem::OnEdit()
{
}

void GraphTreeItem::SwitchCheckBox(bool fOn)
{
}

bool GraphTreeItem::OnContextMenu(CWnd* pWnd, CPoint pos)
{
  return false;
}

void GraphTreeItem::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = CDRF_DODEFAULT;
}

//////////////////////////////////////////////////////////////////////
// GraphLayerTreeItem
//////////////////////////////////////////////////////////////////////

GraphLayerTreeItem::GraphLayerTreeItem(GraphLayerTreeView* gltv, GraphLayer* grlay)
: GraphTreeItem(gltv)
, gl(grlay)
{
}

GraphLayerTreeItem::~GraphLayerTreeItem()
{
}

void GraphLayerTreeItem::OnEdit()
{
  DisplayOptions();
}

void GraphLayerTreeItem::SwitchCheckBox(bool fOn)
{
  gl->fShow = fOn;
  CDocument* doc = ltv->CView::GetDocument();
  doc->SetModifiedFlag();
  doc->UpdateAllViews(ltv);
}

bool GraphLayerTreeItem::OnContextMenu(CWnd* w, CPoint p)
{
	CMenu men;
	men.CreatePopupMenu();
	pmadd(ID_GRPH_OPTIONS);
	men.SetDefaultItem(ID_GRPH_OPTIONS);
	pmadd(ID_REMOVELAYER);
  int iCmd = men.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON|TPM_NONOTIFY|TPM_RETURNCMD, p.x, p.y, w);
	switch (iCmd) 
	{
		case ID_GRPH_OPTIONS:
      DisplayOptions();
			break;
		case ID_REMOVELAYER:
			Remove();
			break;
	}
  return true;
}

void GraphLayerTreeItem::DisplayOptions()
{
  if (gl->fConfig())
  {
    CDocument* doc = ltv->CView::GetDocument();
    doc->SetModifiedFlag();
    doc->UpdateAllViews(0);
  }
}

void GraphLayerTreeItem::Remove()
{
  String s(TR("Remove %S\nAre you sure?").c_str(), gl->sName());
  int iRet = ltv->MessageBox(s.c_str(), TR("Remove Graph").c_str(), MB_YESNO|MB_ICONQUESTION);
  if (IDYES == iRet) {
    GraphDoc* doc = ltv->GetDocument();
    GraphDrawer* gd = doc->grdrGet();
    for(vector<GraphLayer*>::iterator cur = gd->agl.begin(); cur != gd->agl.end(); ++cur)
    {
    	if (*cur == gl) {
        gd->agl.erase(cur);
        doc->SetModifiedFlag();
        doc->UpdateAllViews(0);
    		break;
    	}
    }  
  }  
}

void GraphLayerTreeItem::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
{
  LPNMTVCUSTOMDRAW lptvcd = (LPNMTVCUSTOMDRAW) pNMHDR;
  switch (lptvcd->nmcd.dwDrawStage) 
	{
		case CDDS_ITEMPREPAINT:
			*pResult = CDRF_NOTIFYPOSTPAINT;
			return;
		case CDDS_ITEMPOSTPAINT:
			*pResult = CDRF_DODEFAULT;
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
			CBrush brWhite(clrBack);
			CPen penNull(PS_NULL,0,Color(0,0,0));
			CPen* penOld = cdc.SelectObject(&penNull);
      CBrush* brOld = cdc.SelectObject(&brWhite);
			cdc.Rectangle(rect);
			cdc.SelectObject(penOld);
			cdc.SelectObject(brOld);

      rect.right = rect.left + 35;
      CRgn rgn;
      rgn.CreateRectRgnIndirect(&rect);
      cdc.SelectClipRgn(&rgn);
      enum { eLINE, eSIMPLELINE, ePOINT, eBLOCK } eLegType;
      CartesianGraphLayer* cgl = dynamic_cast<CartesianGraphLayer*>(gl);
      if (0 != cgl)
        switch (cgl->cgt)
        {
          case cgtContineous:
            eLegType = eLINE;
            break;
          case cgtStep:
          case cgtNeedle:
            eLegType = eSIMPLELINE;
            break;
          case cgtBar:
            eLegType = eBLOCK;
            break;
          case cgtPoint:
            eLegType = ePOINT;
            break;
        }
      else // rose diagram
        eLegType = eSIMPLELINE;

      switch (eLegType)
      {
      case eLINE:
        {
          CPoint p1, p2;
          p1.x = rect.left + 5;
          p1.y = (rect.top + rect.bottom) / 2;
          p2 = p1;
          p2.x += 30;
          gl->line.drawLine(cdc, p1, p2);
        } break;
      case eSIMPLELINE:
        {
  				CPen pen(PS_SOLID, 1, gl->color);
  				penOld = cdc.SelectObject(&pen);
          CPoint pt;
          pt.x = rect.left + 5;
          pt.y = (rect.top + rect.bottom) / 2;
					cdc.MoveTo(pt);
          pt.x += 30;
					cdc.LineTo(pt);
		    	cdc.SelectObject(penOld);
        } break;
        case eBLOCK:
        {
          Color color = gl->color;
          if (gl->fRprColor)
            color = SysColor(COLOR_WINDOW);
  				CBrush br(color);
  				CPen pen(PS_SOLID, 1, Color(0,0,0));
	      	penOld = cdc.SelectObject(&pen);
			    brOld = cdc.SelectObject(&br);
          CRect rct;
          rct.top = rect.top + 4;
          rct.bottom = rect.bottom - 4;
          rct.left = rect.left + 5;
          rct.right = rct.left + 30;
          cdc.Rectangle(&rct);
     			cdc.SelectObject(penOld);
		    	cdc.SelectObject(brOld);
        } break;
        case ePOINT:
        {
          CPoint pt;
          pt.x = rect.left + 15;
          pt.y = (rect.top + rect.bottom) / 2;
					gl->smb.drawSmb(&cdc, 0, pt);
        } break;
      }
      cdc.SelectClipRgn(0);
			if (ltv->GetTreeCtrl().GetItemState(hti, TVIS_SELECTED) & TVIS_SELECTED) {
				cdc.SetTextColor(clrTextSel);
				cdc.SetBkColor(clrSel);
			}
			else {
				cdc.SetTextColor(clrText);
				cdc.SetBkColor(clrBack);
			}
			cdc.SetBkMode(OPAQUE);
      cdc.SetTextAlign(TA_LEFT|TA_TOP);
      String sText = gl->sTitle;
      CPoint pt;
      pt.x = rect.left + 40;
      pt.y = rect.top;
			cdc.TextOut(pt.x, pt.y, sText.c_str(), sText.length());
			cdc.Detach();
			return;
	}
}




//////////////////////////////////////////////////////////////////////
// GraphAxisTreeItem
//////////////////////////////////////////////////////////////////////

GraphAxisTreeItem::GraphAxisTreeItem(GraphLayerTreeView* gltv, GraphAxis* graxis)
: GraphTreeItem(gltv)
, ga(graxis)
{
}

GraphAxisTreeItem::~GraphAxisTreeItem()
{
}

void GraphAxisTreeItem::OnEdit()
{
  if (ga->fConfig())
  {
    CDocument* doc = ltv->CView::GetDocument();
    doc->SetModifiedFlag();
    doc->UpdateAllViews(0);
  }
}

void GraphAxisTreeItem::SwitchCheckBox(bool fOn)
{
  ga->fVisible = fOn;
  CDocument* doc = ltv->CView::GetDocument();
  doc->SetModifiedFlag();
  doc->UpdateAllViews(ltv);
}

//////////////////////////////////////////////////////////////////////
// ObjectLayerTreeItem
//////////////////////////////////////////////////////////////////////

ObjectGraphTreeItem::ObjectGraphTreeItem(GraphLayerTreeView* ltv, IlwisObjectPtr* obj) 
: GraphTreeItem(ltv)
{
	ptr = obj;
}

ObjectGraphTreeItem::~ObjectGraphTreeItem()
{
}

void ObjectGraphTreeItem::OnEdit()
{
	if (0 == ptr)
		return;

	if (ptr->fnObj.fValid())
	{
		String sName = ptr->fnObj.sFullPathQuoted();
		IlwWinApp()->OpenDocumentFile(sName.c_str());
	}
}

bool ObjectGraphTreeItem::OnContextMenu(CWnd* w, CPoint p)
{
	if (0 != ptr && ptr->fnObj.fValid()) 
  {			
    IlwWinApp()->ShowPopupMenu(w, p, ptr->fnObj);
    return true;
  }
  return false;    
}

//////////////////////////////////////////////////////////////////////
// StringGraphTreeItem
//////////////////////////////////////////////////////////////////////

StringGraphTreeItem::StringGraphTreeItem(GraphLayerTreeView* gltv, HTREEITEM h, String* s)
: GraphTreeItem(gltv)
, hti(h), str(s), gtf(0)
{
}

StringGraphTreeItem::~StringGraphTreeItem()
{
}

void StringGraphTreeItem::OnEdit()
{
  CRect rectItem, rect;
  ltv->GetClientRect(&rect);
  ltv->GetTreeCtrl().GetItemRect(hti, &rectItem, TRUE);
  rectItem.right = rect.right;
  gtf = new GraphTreeField(ltv, rectItem, *str, this, (NotifyFieldFinished(&StringGraphTreeItem::OnNotifyFieldFinished)));
}

void StringGraphTreeItem::OnNotifyFieldFinished()
{
  if (gtf->GetModify()) {
    CString s;
    gtf->GetWindowText(s);
    *str = (const char*)s;
    CDocument* doc = ltv->CView::GetDocument();
    doc->SetModifiedFlag();
    doc->UpdateAllViews(ltv);
    ltv->GetTreeCtrl().SetItemText(hti,str->c_str());
  }
  delete gtf;
  gtf = 0;
}

//////////////////////////////////////////////////////////////////////
// GraphAxisMinTreeItem
//////////////////////////////////////////////////////////////////////

GraphAxisMinTreeItem::GraphAxisMinTreeItem(GraphLayerTreeView* gltv, HTREEITEM h, GraphAxis* grax)
: GraphTreeItem(gltv)
, hti(h), ga(grax), gtf(0)
{
}

GraphAxisMinTreeItem::~GraphAxisMinTreeItem()
{
}

void GraphAxisMinTreeItem::OnEdit()
{
  CRect rectItem, rect;
  ltv->GetClientRect(&rect);
  ltv->GetTreeCtrl().GetItemRect(hti, &rectItem, TRUE);
  rectItem.right = rect.right;
  String str = ga->sText(ga->rMin(),false);
  gtf = new GraphTreeField(ltv, rectItem, str, this, (NotifyFieldFinished(&GraphAxisMinTreeItem::OnNotifyFieldFinished)));
}

void GraphAxisMinTreeItem::OnNotifyFieldFinished()
{
  if (gtf->GetModify()) {
    CString s;
    gtf->GetWindowText(s);
    String str = (const char*)s;
    double r = str.rVal();
    if ((r != rUNDEF) && (r < ga->rMax()))
    {
      RangeReal rr(r, ga->rMax());
      ga->SetMinMax(rr);
      CDocument* doc = ltv->CView::GetDocument();
      doc->SetModifiedFlag();
      doc->UpdateAllViews(ltv);
      ltv->GetTreeCtrl().SetItemText(hti,str.c_str());
    }
  }
  delete gtf;
  gtf = 0;
}

//////////////////////////////////////////////////////////////////////
// GraphAxisMaxTreeItem
//////////////////////////////////////////////////////////////////////

GraphAxisMaxTreeItem::GraphAxisMaxTreeItem(GraphLayerTreeView* gltv, HTREEITEM h, GraphAxis* grax)
: GraphTreeItem(gltv)
, hti(h), ga(grax), gtf(0)
{
}

GraphAxisMaxTreeItem::~GraphAxisMaxTreeItem()
{
}

void GraphAxisMaxTreeItem::OnEdit()
{
  CRect rectItem, rect;
  ltv->GetClientRect(&rect);
  ltv->GetTreeCtrl().GetItemRect(hti, &rectItem, TRUE);
  rectItem.right = rect.right;
  String str = ga->sText(ga->rMax(),false);
  gtf = new GraphTreeField(ltv, rectItem, str, this, (NotifyFieldFinished(&GraphAxisMaxTreeItem::OnNotifyFieldFinished)));
}

void GraphAxisMaxTreeItem::OnNotifyFieldFinished()
{
  if (gtf->GetModify()) {
    CString s;
    gtf->GetWindowText(s);
    String str = (const char*)s;
    double r = str.rVal();
    if (r > ga->rMin()) 
    {
      RangeReal rr(ga->rMin(), r);
      ga->SetMinMax(rr);
      CDocument* doc = ltv->CView::GetDocument();
      doc->SetModifiedFlag();
      doc->UpdateAllViews(ltv);
      ltv->GetTreeCtrl().SetItemText(hti,str.c_str());
    }
  }
  delete gtf;
  gtf = 0;
}

//////////////////////////////////////////////////////////////////////
// BoolGraphTreeItem
//////////////////////////////////////////////////////////////////////

BoolGraphTreeItem::BoolGraphTreeItem(GraphLayerTreeView* gltv, bool* pf)
: GraphTreeItem(gltv)
, fFlag(pf)
{
  fCheckBox = true;
}

BoolGraphTreeItem::~BoolGraphTreeItem()
{
}

void BoolGraphTreeItem::SwitchCheckBox(bool fOn)
{
  *fFlag = fOn;
  CDocument* doc = ltv->CView::GetDocument();
  doc->SetModifiedFlag();
  doc->UpdateAllViews(ltv);
}

//////////////////////////////////////////////////////////////////////
// GraphTitleTreeItem
//////////////////////////////////////////////////////////////////////

GraphTitleTreeItem::GraphTitleTreeItem(GraphLayerTreeView* gltv, GraphDrawer* grdr)
: GraphTreeItem(gltv)
, gd(grdr)
{
}

GraphTitleTreeItem::~GraphTitleTreeItem()
{
}

void GraphTitleTreeItem::OnEdit()
{
  if (gd->fConfigureTitle())
  {
    CDocument* doc = ltv->CView::GetDocument();
    doc->SetModifiedFlag();
    doc->UpdateAllViews(0);
  }
}
