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
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Engine\Drawers\SimpleDrawer.h"
#include "Engine\Drawers\TextDrawer.h"
#include "Engine\Drawers\OpenGLText.h"

#include "Client/Editors/Editor.h"

class StereoscopySymbols : public Editor
{
public:
	StereoscopySymbols(MapPaneView * mpv)
		: Editor(mpv)
	{
	};
	virtual int draw(volatile bool* fDrawStop)
	{
		((PreStereoMateView*)mpv)->drawSymbols(fDrawStop);
		return 0;
	};
};

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

	edit = new StereoscopySymbols(this);
}

PreStereoMateView::~PreStereoMateView()
{
	Editor * tmp = edit;
	edit = 0;
	delete tmp;
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

void PreStereoMateView::SetEpipolarDocument(MakeEpipolarDocument * d)
{
	med = d;
}

void PreStereoMateView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	SetFocus();
	MapCompositionDoc* mcd = GetDocument();
	if (mcd )
	{
		Coord c = mcd->rootDrawer->screenToOpenGL(RowCol(point.y,point.x));
		RowCol rc (-c.y, c.x);
		if (med)
		{
			med->SetRowCol(rc);
			int lHint = med->iGetHintFromState();
			med->AdvanceSubState();

			Coord crdOld (med->crdGetOldRC()); // Erase previous point (if any)
			if (!crdOld.fUndef())
				SetDirty(crdOld);
			if (!rc.fUndef())
				SetDirty(Coord(rc.Row, rc.Col));

			med->UpdateAllEpipolarViews(this, lHint);
		}
	}
}

void PreStereoMateView::drawSymbols(volatile bool* fDrawStop)
{
	if (*fDrawStop)
		return;
	RootDrawer * rootDrawer = GetDocument()->rootDrawer;
	ILWIS::DrawerParameters dpLayerDrawer (rootDrawer, 0);
	ILWIS::TextLayerDrawer *textLayerDrawer = (ILWIS::TextLayerDrawer *)NewDrawer::getDrawer("TextLayerDrawer", "ilwis38",&dpLayerDrawer);	
	ILWIS::DrawerParameters dpTextDrawer (rootDrawer, textLayerDrawer);
	ILWIS::TextDrawer *textDrawer = (ILWIS::TextDrawer *)NewDrawer::getDrawer("TextDrawer","ilwis38",&dpTextDrawer);
	OpenGLText * font = new OpenGLText (rootDrawer, "arial.ttf", 15, true, 1, -15);
	textLayerDrawer->setFont(font);
	const CoordBounds& cbZoom = rootDrawer->getCoordBoundsZoom();
	double symbolScale = cbZoom.width() / 100;

	list <Element> & eList = med->ElementList();
	for (list <Element>::iterator eIterator = eList.begin(); !*fDrawStop && eIterator != eList.end(); ++eIterator)
	{
		Coord crd (eIterator->crd.y + 0.5, -eIterator->crd.x - 0.5);
		Color col;
		SymbolType smbt;
		int iSize;
		switch (eIterator->iType)
		{
			case 0 : // fiducial
				smbt = smbPlus;
				col = colFidMarks;
				iSize = 1;
				break;
			case 1 : // pp
				smbt = smbCross;
				col = colPrincPnts;
				iSize = 2;
				break;
			case 2 : // tpp
				smbt = smbCross;
				col = colPrincPnts;
				iSize = 2;
				break;
			case 3 : // ofp
				smbt = smbPlus;
				col = colScalePnts;
				iSize = 1;
				break;
			case 4 : // upp
				smbt = smbCross;
				col = colUserPrincPnts;
				iSize = 2;
				break;
		}
		glColor4d(col.redP(), col.greenP(), col.blueP(), 1);
		glPushMatrix();
		glTranslated(crd.x,crd.y,0);
		glScaled(symbolScale *iSize, symbolScale * iSize, 1);
		glBegin(GL_LINES);
		switch(smbt) {
			case smbPlus:
				glVertex3f(-1, 0, 0);
				glVertex3f(1, 0, 0);
				glVertex3f(0, -1, 0);
				glVertex3f(0, 1, 0);
				break;
			case smbCross:
				glVertex3f(-1, -1, 0);
				glVertex3f(1, 1, 0);
				glVertex3f(1, -1, 0);
				glVertex3f(-1, 1, 0);
				break;
		}
		glEnd();
		glPopMatrix();
		textDrawer->addDataSource(&(eIterator->sDescription));
		textDrawer->setCoord(Coordinate(crd.x + 2 * symbolScale, crd.y - symbolScale, 0));
		font->setColor(col);
		textDrawer->draw(ILWIS::NewDrawer::drl2D);
	}
	delete textLayerDrawer;
}

BOOL PreStereoMateView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	bool fStop = false;
	if (iActiveTool) // only go to MapPaneView::OnSetCursor if needed (prevents flickering of cursor)
		fStop = (TRUE == MapPaneView::OnSetCursor(pWnd, nHitTest, message)); // translate BOOL to bool
	// if MapPaneView::OnSetCursor() returns TRUE,
	// we shouldn't replace the cursor with our own (we may be in zoom or pan mode)
	MapCompositionDoc* mcd = GetDocument(); // also check if we have a map, otherwise the fiducial cursor would appear
	bool fMapValid = false;
	if (mcd) {
		for(int i = 0; i < mcd->rootDrawer->getDrawerCount(); ++i) {
			SpatialDataDrawer *dataDrw = dynamic_cast<SpatialDataDrawer *>(mcd->rootDrawer->getDrawer(i));
			if ( !dataDrw)
				continue;
			IlwisObjectPtr *ptr = dataDrw->getObject();
			if (ptr) {
				if ( IOTYPE(ptr->fnObj) == IlwisObject::iotRASMAP) {
					fMapValid = true;
					break;
				} else if ( IOTYPE(ptr->fnObj) == IlwisObject::iotMAPLIST) {
					fMapValid = true;
					break;
				}
			}
		}
	}

	if (!fStop && (HTCLIENT == nHitTest) && (0 != med) && (0 != mcd) && fMapValid)
	{
		MakeEpipolarDocument::iFormStateTP ifsState = med->ifsGetState();
		int iSubState = med->iGetSubState();
		HCURSOR curActive = 0;
		switch (ifsState)
		{
			case MakeEpipolarDocument::ifsFIDUCIALS :
				switch (iSubState)
				{
					case 0 :
						curActive = curSetFiducial01;
						break;
					case 1 :
						curActive = curSetFiducial02;
						break;
					case 2 :
						curActive = curSetFiducial03;
						break;
					case 3 :
						curActive = curSetFiducial04;
						break;
				}
				break;
			case MakeEpipolarDocument::ifsPP :
				curActive = curSetPP;
				break;
			case MakeEpipolarDocument::ifsTPP :
				curActive = curSetTPP;
				break;
			case MakeEpipolarDocument::ifsOFFFLIGHTPTS :
				switch (iSubState)
				{
					case 0 :
						curActive = curSetOFP01;
						break;
					case 1 :
						curActive = curSetOFP02;
						break;
				}
				break;
		}
		if ((HCURSOR)0 != curActive)
		{
			SetCursor(curActive);
			return TRUE;
		}
		else
			return FALSE; // we're not interested in setting a cursor: allow someone else to do so
	}
	else
	{
		if (!iActiveTool)
			SetCursor(curNormal); // otherwise the custom cursor stays too long (even in context menus etc)
		return fStop?TRUE:FALSE; // translate bool to BOOL
	}
}

#define sMen(ID) ILWSF("men",ID).c_str()
#define add(ID) men.AppendMenu(MF_STRING, ID, sMen(ID)); 
#define addBreak men.AppendMenu(MF_SEPARATOR);
void PreStereoMateView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
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
	add(ID_SELECTTPPOINT);
	add(ID_SELECTSCALINGPTS);

	// grey out options that are not available
	CCmdUI cmdUIstate;
	cmdUIstate.m_pMenu = &men;
	cmdUIstate.m_pSubMenu = 0;
	cmdUIstate.m_nIndexMax = 12;
	// loop from ID_NORMAL til ID_SELECTSCALINGPTS
	for (cmdUIstate.m_nIndex = 0; cmdUIstate.m_nIndex < cmdUIstate.m_nIndexMax; cmdUIstate.m_nIndex++) {
		cmdUIstate.m_nID = men.GetMenuItemID(cmdUIstate.m_nIndex);
		cmdUIstate.DoUpdate(this, 0);
	}

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

void PreStereoMateView::SetSiblingPane(PreStereoMateView * psmv)
{
	psmvSiblingPane = psmv;
	//smwParent = dynamic_cast<StereoMapWindow*>(fwParent());
}

void PreStereoMateView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	fActive = (TRUE == bActivate); // translate BOOL to bool
	GetParent()->Invalidate();
}

