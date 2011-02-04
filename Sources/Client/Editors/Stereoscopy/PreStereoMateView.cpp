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
// PreStereoMateview.cpp: implementation of the PreStereoMateview class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Client\Editors\Stereoscopy\PreStereoMateView.h"
#include "Client\Editors\Stereoscopy\MakeEpipolarDocument.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Editors\Utils\SYMBOL.H"
#include "Headers\constant.h"
#include "Engine\Base\System\RegistrySettings.h"

IMPLEMENT_DYNCREATE(PreStereoMateView, MapPaneView)

BEGIN_MESSAGE_MAP(PreStereoMateView, MapPaneView)
	//{{AFX_MSG_MAP(PreStereoMateView)
	ON_WM_LBUTTONDOWN()
	ON_WM_SETCURSOR()
	ON_WM_CONTEXTMENU()
	ON_WM_NCPAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

PreStereoMateView::PreStereoMateView()
: med(0) ,
	curSetFiducial01("SetFiducial01"),
	curSetFiducial02("SetFiducial02"),
	curSetFiducial03("SetFiducial03"),
	curSetFiducial04("SetFiducial04"),
	curSetPP("SetPP"),
	curSetTPP("SetTPP"),
	curSetOFP01("SetOFP01"),
	curSetOFP02("SetOFP02"),
	curNormal(Arrow),
	fActive(false)
{
	colFidMarks = Color(255,0,255);  // default purple
	colPrincPnts = Color(255,100,255);  // default light purple
	colScalePnts = Color(255,100,100);  // default red-purple
	colUserPrincPnts = Color(255,200,0);  // default yellow

	IlwisSettings settings("Stereo Maker");
	// Now read the customizable items from registry
	colFidMarks = settings.clrValue("FM", colFidMarks); // entry does not exist => default color
	colPrincPnts = settings.clrValue("PP",colPrincPnts);
	colScalePnts = settings.clrValue("SP",colScalePnts);
	colUserPrincPnts = settings.clrValue("UPP",colUserPrincPnts);
}

PreStereoMateView::~PreStereoMateView()
{
}

void PreStereoMateView::OnDraw(CDC* pDC)
{
	MapPaneView::OnDraw(pDC);
	drawAllElements(pDC);
}

void PreStereoMateView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	// NOTE!! we arrived here either via MapCompositionDoc or via MakeEpipolarDocument
	// Our intention is to react only to the calls from MakeEpipolarDocument
	
	// We defined for this lHint to be between 10000 and 10063 in that case
	if (lHint < 10000 || lHint > 10063) // Not from MakeEpipolarDocument => not for us (how guaranteed is this?)
		MapPaneView::OnUpdate(pSender, lHint, pHint);
	else if (10000 == lHint)
	{
		// refresh all: invalidate all rc's in the element list
		list <Element> eList = med->ElementList();
		for (list <Element>::iterator eIterator = eList.begin(); eIterator != eList.end(); ++eIterator)
			SetDirty(eIterator->crd);
	}
	else
	{
		Coord crdOld (med->crdGetOldRC()); // Erase previous point (if any)
		if (!crdOld.fUndef())
			SetDirty(crdOld);
		Coord crd (med->crdGetRCFromHint(lHint)); // Draw new point (if any)
		if (!crd.fUndef())
			SetDirty(crd);
	}
}

void PreStereoMateView::SetDirty(Coord crd)
{
	// Forces a redraw around rc that is just enough for the area needed for a fiducial or a PP
  Symbol smb;
  smb.smb = smbPlus;
	smb.iSize = 15;
  zPoint p = pntPos(Coord(crd.x+0.5, crd.y+0.5));
  // zPoint p = pntPos(rc);
  zRect rect(p,p);
  rect.top()   -= smb.iSize / 2 + 1;
  rect.left()  -= smb.iSize / 2 + 1;
  rect.bottom()+= smb.iSize / 2 + 2;
  rect.right() += smb.iSize / 2 + 2;

	CClientDC cdc(this);
  zPoint pntText = smb.pntText(&cdc, p);
  CSize siz = cdc.GetTextExtent("TP", 2); // space of "TPP" to be redrawn; 2 is the size of the string
  pntText.x += 10 + siz.cx + 1; // (10,5) is the text offset
  pntText.y += 5 + siz.cy + 1;
  rect.bottom() = max(rect.bottom(), pntText.y);
  rect.right() = max(rect.right(), pntText.x);
  InvalidateRect(&rect);
}

bool PreStereoMateView::fPointElementInCDC(zPoint pnt)
{
	Symbol smb;
  smb.smb = smbPlus;
	smb.iSize = 15;
  // zPoint p = pntPos(rc.Row-0.5, rc.Col-0.5);
  zRect rect(pnt,pnt);
  rect.top()   -= smb.iSize / 2 + 1;
  rect.left()  -= smb.iSize / 2 + 1;
  rect.bottom()+= smb.iSize / 2 + 2;
  rect.right() += smb.iSize / 2 + 2;

	CClientDC cdc(this);
  zPoint pntText = smb.pntText(&cdc, pnt);
  CSize siz = cdc.GetTextExtent("TP", 2); // space of "TPP" to be redrawn; 2 is the size of the string
  pntText.x += 10 + siz.cx + 1; // (10,5) is the text offset
  pntText.y += 5 + siz.cy + 1;
  rect.bottom() = max(rect.bottom(), pntText.y);
  rect.right() = max(rect.right(), pntText.x);
	return (0 != cdc.RectVisible(&rect));
}

void PreStereoMateView::SetEpipolarDocument(MakeEpipolarDocument * d)
{
	med = d;
}

void PreStereoMateView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	throw ErrorObject(String("To Be Done %d %s", __LINE__, __FILE__));
	SetFocus();
	//MapCompositionDoc* mcd = GetDocument();
	//if (mcd )
	//{
	//	CRect rctBounds = rctPos(mcd->mmBounds());
	//	bool fInside = (0 != rctBounds.PtInRect(point));
	//	if (fInside)
	//	{
	//		RowCol rc = rcPos(point);
	//		// rc.Row += 1;
	//		// rc.Col += 1;
	//		if (med)
	//		{
	//			med->SetRowCol(rc);
	//			int lHint = med->iGetHintFromState();
	//			med->AdvanceSubState();

	//			Coord crdOld (med->crdGetOldRC()); // Erase previous point (if any)
	//			if (!crdOld.fUndef())
	//				SetDirty(crdOld);
	//			if (!rc.fUndef())
	//				SetDirty(Coord(rc.Row, rc.Col));

	//			med->UpdateAllEpipolarViews(this, lHint);
	//		}
	//	}
	//}
}

void PreStereoMateView::drawElement(CDC* cdc, zPoint pnt, Symbol smb, String sDescription)
{
	cdc->SetTextAlign(TA_LEFT|TA_TOP);
  cdc->SetBkMode(TRANSPARENT);
  cdc->SetTextColor(smb.col);
  cdc->TextOut(pnt.x + 10, pnt.y + 5, sDescription.sVal()); // (10,5) is the text offset
  smb.drawSmb(cdc, 0, pnt);
}

void PreStereoMateView::drawAllElements(CDC* cdc)
{
	throw ErrorObject(String("To Be Done %d %s", __LINE__, __FILE__));
	// For the time being, we still need a list of elements.
	// However, this is retrieved on-the-fly from the MakeEpipolarDocument
	//list <Element> eList = med->ElementList();

	//for (list <Element>::iterator eIterator = eList.begin(); eIterator != eList.end(); ++eIterator)
	//{
	//	//zPoint pnt (pntPos(eIterator->crd.x+0.5, eIterator->crd.y+0.5)); // Screen point for drawing the element
	//	// zPoint pnt (pntPos(eIterator->rc)); // Screen point for drawing the element
	//	if (fPointElementInCDC(pnt)) // if not in clipregion, no need to (re-)draw it
	//	{
	//		Symbol smb; // Symbol used for drawing the element
	//		String sDescription = eIterator->sDescription; // Description of the element
	//		switch (eIterator->iType)
	//		{
	//			case 0 : // fiducial
	//				smb.smb = smbPlus;
	//				smb.col = colFidMarks;
	//				break;
	//			case 1 : // pp
	//				smb.smb = smbCross;
	//				smb.col = colPrincPnts;
	//				smb.iSize = 15;
	//				break;
	//			case 2 : // tpp
	//				smb.smb = smbCross;
	//				smb.col = colPrincPnts;
	//				smb.iSize = 15;
	//				break;
	//			case 3 : // ofp
	//				smb.smb = smbPlus;
	//				smb.col = colScalePnts;
	//				break;
	//			case 4 : // upp
	//				smb.smb = smbCross;
	//				smb.col = colUserPrincPnts;
	//				smb.iSize = 15;
	//				break;
	//		}
	//		drawElement(cdc, pnt, smb, sDescription);
	//	}
	//}
}

BOOL PreStereoMateView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	throw ErrorObject(String("To Be Done %d %s", __LINE__, __FILE__));
	//bool fStop = false;
	//if (as) // only go to MapPaneView::OnSetCursor if needed (prevents flickering of cursor)
	//	fStop = (TRUE == MapPaneView::OnSetCursor(pWnd, nHitTest, message)); // translate BOOL to bool
	//// if MapPaneView::OnSetCursor() returns TRUE,
	//// we shouldn't replace the cursor with our own (we may be in zoom or pan mode)
	//MapCompositionDoc* mcd = GetDocument(); // also check if we have a map, otherwise the fiducial cursor would appear
	//if (!fStop && (HTCLIENT == nHitTest) && (0 != med) && (0 != mcd) && (mcd->mp.fValid()))
	//{
	//	MakeEpipolarDocument::iFormStateTP ifsState = med->ifsGetState();
	//	int iSubState = med->iGetSubState();
	//	HCURSOR curActive = 0;
	//	switch (ifsState)
	//	{
	//		case MakeEpipolarDocument::ifsFIDUCIALS :
	//			switch (iSubState)
	//			{
	//				case 0 :
	//					curActive = curSetFiducial01;
	//					break;
	//				case 1 :
	//					curActive = curSetFiducial02;
	//					break;
	//				case 2 :
	//					curActive = curSetFiducial03;
	//					break;
	//				case 3 :
	//					curActive = curSetFiducial04;
	//					break;
	//			}
	//			break;
	//		case MakeEpipolarDocument::ifsPP :
	//			curActive = curSetPP;
	//			break;
	//		case MakeEpipolarDocument::ifsTPP :
	//			curActive = curSetTPP;
	//			break;
	//		case MakeEpipolarDocument::ifsOFFFLIGHTPTS :
	//			switch (iSubState)
	//			{
	//				case 0 :
	//					curActive = curSetOFP01;
	//					break;
	//				case 1 :
	//					curActive = curSetOFP02;
	//					break;
	//			}
	//			break;
	//	}
	//	if ((HCURSOR)0 != curActive)
	//	{
	//		SetCursor(curActive);
	//		return TRUE;
	//	}
	//	else
	//		return FALSE; // we're not interested in setting a cursor: allow someone else to do so
	//}
	//else
	//{
	//	if (!as)
	//		SetCursor(curNormal); // otherwise the custom cursor stays too long (even in context menus etc)
	//	return fStop?TRUE:FALSE; // translate bool to BOOL
	//}
}

#define sMen(ID) ILWSF("men",ID).scVal()
#define add(ID) men.AppendMenu(MF_STRING, ID, sMen(ID)); 
#define addBreak men.AppendMenu(MF_SEPARATOR);
void PreStereoMateView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	if (tools.size() > 0)
		return;
	// if (edit && edit->OnContextMenu(pWnd, point))
	// 	return;
  CMenu men, menSub;
	men.CreatePopupMenu();
	add(ID_NORMAL);
  add(ID_ENTIREMAP);
  add(ID_ZOOMIN);
  add(ID_ZOOMOUT);
	add(ID_PANAREA);
  addBreak;
  add(ID_REDRAW);
	addBreak;
	add(ID_SELECTFIDUCIALS);
	add(ID_SELECTPPOINT);
	if (med)
		men.EnableMenuItem(ID_SELECTPPOINT, (med->iGetNrFiducials()<2)? MF_ENABLED : MF_GRAYED);
	add(ID_SELECTTPPOINT);
	add(ID_SELECTSCALINGPTS);
	if (med)
	{
		MakeEpipolarDocument::iFormStateTP ifsState = med->ifsGetState();
		int iToCheck = 0;
		switch (ifsState)
		{
			case MakeEpipolarDocument::ifsFIDUCIALS :
				iToCheck = ID_SELECTFIDUCIALS;
				break;
			case MakeEpipolarDocument::ifsPP :
				iToCheck = ID_SELECTPPOINT;
				break;
			case MakeEpipolarDocument::ifsTPP :
				iToCheck = ID_SELECTTPPOINT;
				break;
			case MakeEpipolarDocument::ifsOFFFLIGHTPTS :
				iToCheck = ID_SELECTSCALINGPTS;
				break;
		}
		if (iToCheck)
			men.CheckMenuRadioItem(ID_SELECTFIDUCIALS, ID_SELECTSCALINGPTS, iToCheck, MF_BYCOMMAND);
	}
  men.AppendMenu(MF_SEPARATOR);
  menSub.CreateMenu();
	GetDocument()->menLayers(menSub, ID_LAYFIRST);
  men.AppendMenu(MF_POPUP, (UINT)menSub.GetSafeHmenu(), sMen(ID_LAYEROPTIONS)); 
	menSub.Detach();    
  men.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, point.x, point.y, pWnd);
}

BOOL PreStereoMateView::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	// To pass messages to the MakeEpipolarDocument that is manually linked to this view
	if (med && med->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		 return TRUE;
	return MapPaneView::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void PreStereoMateView::OnNcPaint() 
{
	// Default OnNcPaint (incl. scrollbars)
	CView::OnNcPaint();

	// get window DC that is clipped to the non-client area
	CWindowDC dc(this);

	CRect rcClient, rcBar;
	GetClientRect(rcClient);
	ClientToScreen(rcClient);
	GetWindowRect(rcBar);
	rcClient.OffsetRect(-rcBar.TopLeft());
	rcBar.OffsetRect(-rcBar.TopLeft());

	// client area and scrollbars is not our bussiness :)
	CRect rcVert (rcClient);
	rcVert.InflateRect(0, GetSystemMetrics(SM_CYHSCROLL));
	dc.ExcludeClipRect(rcVert);
	CRect rcHorz (rcClient);
	rcHorz.InflateRect(GetSystemMetrics(SM_CXVSCROLL), 0);
	dc.ExcludeClipRect(rcHorz);

	COLORREF clrActive = GetSysColor(COLOR_HIGHLIGHT);
	COLORREF clrInactive = GetSysColor(COLOR_SCROLLBAR);
	// Now draw the active or inactive square anyway: the default OnNcPaint doesn't draw
	// unnecessarily, and there is no such thing as NcInvalidate(), so if you only draw
	// when you're active, you won't get cleaned up often enough.
	// COLOR_SCROLLBAR is as close as we can get to the default color of that square
	dc.FillSolidRect(rcBar, fActive?clrActive:clrInactive);

	ReleaseDC(&dc);
}

void PreStereoMateView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	fActive = (TRUE == bActivate); // translate BOOL to bool
	GetParent()->Invalidate();
}
