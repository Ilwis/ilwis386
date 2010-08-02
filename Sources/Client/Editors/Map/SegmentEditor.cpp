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
 GNU General Public License for more details->

 You should have received a copy of the GNU General Public License
 along with this program (see gnu-gpl v2.txt); if not, write to
 the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 Boston, MA 02111-1307, USA or visit the web page of the Free
 Software Foundation, http://www.fsf.org.

 Created on: 2007-02-8
 ***************************************************************/
// SegmentEditor.cpp: implementation of the SegmentEditor class->
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Engine\Map\Segment\Seg.h"
#include "Engine\Map\Polygon\POL.H"
#include "Engine\Map\Point\PNT.H"
#include "Client\Mapwindow\Positioner.h"
#include "Client\Mapwindow\Drawers\BaseDrawer.h"
#include "Client\Mapwindow\Drawers\Drawer.h"
#include "Client\Editors\Editor.h"
#include "Client\Editors\Digitizer\DigiEditor.h"
#include "Client\Base\ButtonBar.h"
#include "Client\Editors\Map\SegmentEditor.h"
#include "Client\Editors\Utils\SYMBOL.H"
#include "Headers\Hs\Editor.hs"
#include "Client\FormElements\syscolor.h"
#include "Client\Mapwindow\AreaSelector.h"
#include "Client\Mapwindow\IlwisClipboardFormat.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\MainWindow\Catalog\CatalogDocument.h"
#include "Engine\Map\Mapview.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Headers\constant.h"
#include "Engine\Domain\dmident.h"
#include "Client\FormElements\fldcolor.h"
#include "Engine\Base\Algorithm\Tunnel.h"
#include "Headers\Hs\Appforms.hs"
#include "Client\FormElements\flddom.h"
#include "PolygonApplications\POLFRMSG.H"
#include "Client\ilwis.h"
#include "Headers\Htp\Ilwis.htp"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\Mapwindow\MapWindow.h"
#include "Engine\Domain\DomainUniqueID.h"
#include "Engine\Table\Rec.h"
#include "Headers\Hs\Coordsys.hs"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//#define sqr(x) (x * x)

BEGIN_MESSAGE_MAP(SegmentEditor, DigiEditor)
	//{{AFX_MSG_MAP(SegmentEditor)
	ON_COMMAND(ID_COPY, OnCopy)
  ON_UPDATE_COMMAND_UI(ID_COPY, OnUpdateCopy)
	ON_COMMAND(ID_PASTE, OnPaste)
  ON_UPDATE_COMMAND_UI(ID_PASTE, OnUpdatePaste)
	ON_COMMAND(ID_EDIT, OnEdit)
  ON_UPDATE_COMMAND_UI(ID_EDIT, OnUpdateEdit)
	ON_COMMAND(ID_CLEAR, OnClear)
  ON_UPDATE_COMMAND_UI(ID_CLEAR, OnUpdateClear)
	ON_COMMAND(ID_CUT, OnCut)
  ON_UPDATE_COMMAND_UI(ID_CUT, OnUpdateCopy)
	ON_COMMAND(ID_SETVAL, OnSetVal)
  ON_UPDATE_COMMAND_UI(ID_SETVAL, OnUpdateSetVal)
	ON_COMMAND(ID_SELECTMODE, OnSelectMode)
  ON_UPDATE_COMMAND_UI(ID_SELECTMODE, OnUpdateMode)
	ON_COMMAND(ID_MOVEMODE, OnMoveMode)
  ON_UPDATE_COMMAND_UI(ID_MOVEMODE, OnUpdateMode)
	ON_COMMAND(ID_INSERTMODE, OnInsertMode)
  ON_UPDATE_COMMAND_UI(ID_INSERTMODE, OnUpdateMode)
	ON_COMMAND(ID_SPLITMODE, OnSplitMode)
  ON_UPDATE_COMMAND_UI(ID_SPLITMODE, OnUpdateMode)
	ON_COMMAND(ID_UNDELSEG, OnUnDelSeg)
  ON_UPDATE_COMMAND_UI(ID_UNDELSEG, OnUpdateMode)
	ON_COMMAND(ID_FINDUNDEFS, OnFindUndefs)
  ON_UPDATE_COMMAND_UI(ID_FINDUNDEFS, OnUpdateMode)
	ON_COMMAND(ID_CONFIGURE, OnConfigure)
  ON_COMMAND(ID_SEGCHECKSELF, OnCheckSelf)
  ON_COMMAND(ID_SEGCHECKDEADENDS, OnCheckConnected)
  ON_COMMAND(ID_SEGCHECKINTERSECT, OnCheckIntersects)
  ON_COMMAND(ID_SEGCHECKCODECONS, OnCheckCodeConsistency)
  ON_COMMAND(ID_SEGCHECKCLOSEDSEGMENTS, OnCheckClosedSegments)
  ON_COMMAND(ID_SEGREMOVEREDUNDANT, OnRemoveRedundantNodes)
  ON_COMMAND(ID_SEGPOLYGONIZE, OnPolygonize)
  ON_COMMAND(ID_UNDOALL, OnUndoAllChanges)
  ON_COMMAND(ID_FILE_SAVE, OnFileSave)
  ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
  ON_UPDATE_COMMAND_UI(ID_UNDOALL, OnUpdateFileSave)
	ON_COMMAND(ID_SEGPACK, OnSegPack)
	ON_UPDATE_COMMAND_UI(ID_SEGPACK, OnUpdateSegPack)
	ON_COMMAND(ID_SEGSETBOUNDS, OnSetBoundaries)
	ON_COMMAND(ID_SELALL, OnSelectAll)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


const int iMAXCOORDS = 20000;

#define sMen(ID) ILWSF("men",ID).scVal()
#define add(ID) men.AppendMenu(MF_STRING, ID, sMen(ID)); 
#define addSub(ID) menSub.AppendMenu(MF_STRING, ID, sMen(ID)); 

SegmentEditor::SegmentEditor(MapPaneView* mpvw, SegmentMap mp)
: DigiEditor(mpvw,mp->cb()),
currentSeg(mp->segFirst()), 
curEdit("EditCursor"),
curSegEdit("EditPntCursor"),
curSegMove("EditPntMoveCursor"),
curSegMoving("EditPntMovingCursor"),
curSegSplit("EditSplitCursor"),
curSegSplitting("EditSplittingCursor"),
fOnlySelected(false)
{
	iFmtPnt = RegisterClipboardFormat("IlwisPoints");
	iFmtDom = RegisterClipboardFormat("IlwisDomain");

	dvs = mp->dvrs();
	_rpr = dvs.dm()->rpr();
	coords.clear();
	iNrCoords = 0;
	fDigBusy = false;
	fRetouching = false;
	fUndelete = false;
	fFindUndefs = false;
	drw = drwNORMAL;
	sm = mp;
	if (sm->fDependent() || sm->fDataReadOnly() || !sm->dm()->fValidDomain()) {
		mpv->MessageBox(SEDErrNotEditableSegMap.sVal(),SEDErrSegEditor.sVal(),MB_OK|MB_ICONSTOP);
		fOk = false;
		return;
	}
	sm->KeepOpen(true);

	MapCompositionDoc* mcd = mpv->GetDocument();
	for (list<Drawer*>::iterator iter = mcd->dl.begin(); iter != mcd->dl.end(); ++iter) 
	{
		Drawer* dr = *iter;
		if (dr->obj() == sm) {
			Editor::drw = dr;
			break;
		}
	}
	if (0 == Editor::drw) {
		Editor::drw = mcd->drAppend(sm);
		mcd->UpdateAllViews(mpv,2);
	}

	mode = modeSELECT;

	curActive = curEdit;
	AddSegments(Coord());

	col = Color(0,255,0); // green
	colFindUndef = Color(255,0,0); // red
	colRetouch = Color(255,0,0); // red
	colDeleted = Color(0,255,255); // cyan
	iSnapPixels = 5;								 

	IlwisSettings settings("Map Window\\Segment Editor");

	String fn = IlwWinApp()->Context()->fnUserINI().sFullName();
	iSnapPixels = GetPrivateProfileInt("Segment Editor", "Snap Pixels", 5, fn.sVal());
	iSnapPixels = settings.iValue("Snap Pixels", iSnapPixels);
	if (iSnapPixels < 1)
		iSnapPixels = 1;
	char sBuf[80];
	String sVal;
	sVal = "Normal";
	GetPrivateProfileString("Segment Editor", "Segment Colors", sVal.sVal(), sBuf, 79, fn.sVal());
	sVal = settings.sValue("Segment Colors", sBuf);
	if (sVal == "Primary")
		drw = drwPRIMARY;
	else if (sVal == "Domain" && sm->dm()->rpr().fValid())
		drw = drwDOMAIN;
	else
		drw = drwNORMAL;

	sVal = "yes";
	GetPrivateProfileString("Segment Editor", "Auto Snap", sVal.sVal(), sBuf, 79, fn.sVal());
	fAutoSnap = strcmp(sBuf, "no") ? true : false;
	fAutoSnap = settings.fValue("Auto Snap", fAutoSnap);
	sVal = "yes";
	GetPrivateProfileString("Segment Editor", "Show Nodes", sVal.sVal(), sBuf, 79, fn.sVal());
	fShowNodes = strcmp(sBuf, "no") ? true : false;
	fShowNodes = settings.fValue("Show Nodes", fShowNodes);
	sVal = String("%06lx", (long)col);
	GetPrivateProfileString("Segment Editor", "Normal Color", sVal.sVal(), sBuf, 79, fn.sVal());
	sscanf(sBuf,"%lx",&col);
	col = settings.clrValue("Normal Color", col);
	sVal = String("%06lx", (long)colRetouch);
	GetPrivateProfileString("Segment Editor", "Retouch Color", sVal.sVal(), sBuf, 79, fn.sVal());
	sscanf(sBuf,"%lx",&colRetouch);
	colRetouch = settings.clrValue("Retouch Color", colRetouch);
	sVal = String("%06lx", (long)colDeleted);
	GetPrivateProfileString("Segment Editor", "Deleted Color", sVal.sVal(), sBuf, 79, fn.sVal());
	sscanf(sBuf,"%lx",&colDeleted);
	colDeleted = settings.clrValue("Deleted Color", colDeleted);
	colFindUndef = settings.clrValue("Find Undef Color", colFindUndef);

	CMenu men;
	men.CreateMenu();
	add(ID_UNDOALL);
	add(ID_FILE_SAVE);
	CMenu menSub;
	menSub.CreateMenu();
	addSub(ID_SEGCHECKSELF);
	addSub(ID_SEGCHECKDEADENDS);
	addSub(ID_SEGCHECKINTERSECT);
	menSub.AppendMenu(MF_SEPARATOR);
	addSub(ID_SEGCHECKCODECONS);
	menSub.AppendMenu(MF_SEPARATOR);
	addSub(ID_SEGCHECKCLOSEDSEGMENTS);
	men.AppendMenu(MF_POPUP, (UINT)menSub.GetSafeHmenu(), sMen(ID_SEGCHECK)); 
	menSub.Detach();
	add(ID_SEGREMOVEREDUNDANT);
	add(ID_SEGPACK);
	add(ID_SEGPOLYGONIZE);
	add(ID_SEGSETBOUNDS);
	men.AppendMenu(MF_SEPARATOR);
	add(ID_CONFIGURE);
	menSub.CreateMenu();
	addSub(ID_FILE_DIGREF);
	addSub(ID_DIGACTIVE);
	men.AppendMenu(MF_POPUP, (UINT)menSub.GetSafeHmenu(), sMen(ID_DIG)); 
	menSub.Detach();
	add(ID_EXITEDITOR);
	hmenFile = men.GetSafeHmenu();
	men.Detach();

	men.CreateMenu();
	add(ID_CUT  );
	add(ID_COPY );
	add(ID_PASTE);
	add(ID_CLEAR);
	men.AppendMenu(MF_SEPARATOR);
	add(ID_SELALL);
	add(ID_EDIT);
	add(ID_SETVAL);
	men.AppendMenu(MF_SEPARATOR);
	add(ID_SELECTMODE);
	add(ID_MOVEMODE);
	add(ID_INSERTMODE);
	add(ID_SPLITMODE);
	men.AppendMenu(MF_SEPARATOR);
	add(ID_UNDELSEG);
	add(ID_FINDUNDEFS);
	hmenEdit = men.GetSafeHmenu();
	men.Detach();
	UpdateMenu();

	DataWindow* dw = mpv->dwParent();
	if (dw) {
		dw->bbDataWindow.LoadButtons("segedit.but");
		dw->RecalcLayout();
	}
	htpTopic = htpSegmentEditor;
	sHelpKeywords = "Segment editor";
}

SegmentEditor::~SegmentEditor()
{
  if (sm.fValid()) {
//    bool fDel = true;
    if (sm->fChanged) {
      sm->Updated();
      sm->Store();
    }  
/*
    if (fDel) {
      FileName fnEdit = fnSave;
      fnEdit.sFile[0] = '#';
      SegmentMap mpOld(fnEdit);
      mpOld->fErase = true;
    }
*/
    sm->KeepOpen(false);
  }    
}

bool SegmentEditor::OnContextMenu(CWnd* pWnd, CPoint point)
{
	if (modeENTERING == mode)
		return true;
	CMenu men;
	men.CreatePopupMenu();
	add(ID_NORMAL);
	add(ID_ZOOMIN);
	add(ID_ZOOMOUT);
	add(ID_PANAREA);
	men.AppendMenu(MF_SEPARATOR);
	switch (mode) {
		case modeSELECT: {
			add(ID_EDIT);
			BOOL fEdit = segList.iSize() != 0;
			men.EnableMenuItem(ID_EDIT, fEdit ? MF_ENABLED : MF_GRAYED);
			add(ID_CLEAR);
			men.EnableMenuItem(ID_CLEAR, fEdit ? MF_ENABLED : MF_GRAYED);
			add(ID_UNDELSEG);
			men.CheckMenuItem(ID_UNDELSEG, fUndelete ? MF_CHECKED : MF_UNCHECKED);
			add(ID_FINDUNDEFS);
			men.CheckMenuItem(ID_FINDUNDEFS, fFindUndefs ? MF_CHECKED : MF_UNCHECKED);
			men.AppendMenu(MF_SEPARATOR);
						 }	break;
		case modeADD:
			add(ID_SETVAL);
			men.AppendMenu(MF_SEPARATOR);
			break;
	}
	add(ID_CONFIGURE);
	add(ID_EXITEDITOR);
	men.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, point.x, point.y, pWnd);
	return true;
}

IlwisObject SegmentEditor::obj() const
{
  return sm;
}

RangeReal SegmentEditor::rrStretchRange() const
{
	RangeReal rr;
	if (sm->dm()->pdv()) {
		rr = sm->rrPerc1();
		if (!rr.fValid()) {
			rr = sm->rrMinMax();
			if (!rr.fValid())
				rr = sm->vr()->rrMinMax();
		}
	}
	return rr;
}

Color SegmentEditor::clrRaw(long iRaw) const
{
	Color clr;
	switch (drw) {
	case drwNORMAL:
		return col;
	case drwPRIMARY:
		return clrPrimary(1+iRaw%31);
	case drwDOMAIN:
		if (iRaw != iUNDEF) {
			if (dvrs().fValues()) {
				double rVal = sm->dvrs().rValue(iRaw);
				return clrVal(rVal);
			}
			if (dm()->pdc()) {
				if (_rpr.fValid()) {
					clr = _rpr->clrRaw(iRaw);
					return clr;
				}
			}
			clr = clrPrimary(1+iRaw%16);
		}
		break;
	}
	return clr;
}

Color SegmentEditor::clrVal(double rVal) const
{
  Color cRet;
  switch (drw) {
    case drwNORMAL:
      return col;
    case drwDOMAIN:
      if (!_rpr.fValid())
        return cRet;
      if (0 == _rpr->prv())
        cRet = _rpr->clr(rVal,rrStretchRange());
      else
        cRet = _rpr->clr(rVal);
      break;
  }
  return cRet;
}

int SegmentEditor::draw(CDC* cdc, zRect rect, Positioner* psn, volatile bool* fDrawStop)
{
	MapCompositionDoc* mcd = mpv->GetDocument();

	/*
	-- naar DigiEditor::PreDraw ?? remove cursor?
	zRect r;
	mappane->getInterior(r);
	if (r == rect)
	crdDig = Coord();
	*/

	CoordBounds cbIntern;
	CoordBounds cb = cbRect(psn);
	bool fBoundCheck = !cb.fUndef();
	if (fBoundCheck) {
		double rW10 = cb.width() / 10;
		double rH10 = cb.height() /10;
		cb.MinX() -= rW10;
		cb.MaxX() += rW10;
		cb.MinY() -= rH10;
		cb.MaxY() += rH10;
		cbIntern += cb.cMin;
		cbIntern += cb.cMax;
		bool fSameCsy = sm->cs() == mcd->georef->cs();
		if (!fSameCsy) {
			double dX = cb.width() / 10;
			double dY = cb.height() / 10;
			Coord c;
			int i, j;
			for (i = 0, c.x = cb.MinX(); i < 10; c.x += dX, ++i)
				for (j = 0, c.y = cb.MinY(); j < 10; c.y += dY, ++j) {
					Coord crd = sm->cs()->cConv(mcd->georef->cs(), c);
					if (!crd.fUndef())
						cbIntern += crd;
				}  
		}      
	}  

	Color c;
	if ((long)col == -1)
		c = SysColor(COLOR_WINDOWTEXT);
	else
		c = col;
	CPen pen(PS_SOLID,1,c);
	CPen* penOld = cdc->SelectObject(&pen);

	if (segList.iSize()) {
		Color cFgBr = SysColor(COLOR_HIGHLIGHT);
		CPen penSel(PS_SOLID,3,cFgBr);
		CPen* penO;
		if (!fOnlySelected) {
			cdc->SelectObject(penOld);
			penO = cdc->SelectObject(&penSel);
		}
		for (SLIter<ILWIS::Segment *> iter(&segList); iter.fValid(); ++iter) {
			//      if (mappane->fDrawCheck())
			//        break;
			if (cbIntern.fContains(iter()->cbBounds())) 
				drawSegment(cdc,iter(),true);
		}	
		if (!fOnlySelected) 
			cdc->SelectObject(penO);
		cdc->SelectObject(penOld);
		penOld = cdc->SelectObject(&pen);
	}
	if (fOnlySelected) 
		return 0;

	for (int i = 0; i < sm->iFeatures(); ++i) {
		ILWIS::Segment *s = (ILWIS::Segment *)sm->getFeature(i);
		if ( !(s && s->fValid()))
			continue;
		//    if (mappane->fDrawCheck())
		//      break;
		if (cbIntern.fContains(s->cbBounds())) 
			drawSegment(cdc,s,false);
	}
	if (fUndelete) {
		CPen penDel(PS_SOLID,1,colDeleted);
		CPen* penO = cdc->SelectObject(&penDel);
		for (int i = 0; i < sm->iFeatures(); ++i) {
			ILWIS::Segment *s = (ILWIS::Segment *)sm->getFeature(i);
			if ( !(s && s->fValid()))
				continue;
			if (s->fDeleted() && cbIntern.fContains(s->cbBounds()))
				drawSegment(cdc,s,true);
		}	
		cdc->SelectObject(penO);
	}
	if (fFindUndefs) {
		CPen penUndefs(PS_SOLID,1,colFindUndef);
		CPen* penO = cdc->SelectObject(&penUndefs);
		for (int i = 0; i < sm->iFeatures(); ++i) {
			ILWIS::Segment *s = (ILWIS::Segment *)sm->getFeature(i);
			if ( !(s && s->fValid()))
				continue;
			if (iUNDEF != s->iValue())
				continue;
			CoordBounds cb = s->cbBounds();
			if (!cbIntern.fContains(cb))
				continue;
			drawSegment(cdc,s,true);
			// if too small draw a rectangle
			CPoint p1 = mpv->pntPos(cb.cMin);
			CPoint p2 = mpv->pntPos(cb.cMax);
			CRect rct(p1,p2);
			rct.NormalizeRect();
			if (rct.Height() + rct.Width() < 25) {
				CPoint pt = rct.CenterPoint();
				cdc->MoveTo(pt.x-13,pt.y-13);
				cdc->LineTo(pt.x+14,pt.y-13);
				cdc->LineTo(pt.x+14,pt.y+14);
				cdc->LineTo(pt.x-13,pt.y+14);
				cdc->LineTo(pt.x-13,pt.y-13);
			}
		}
		cdc->SelectObject(penO);
	}
	cdc->SelectObject(penOld);
	drawCoords(cdc,col);
	drawActNode(cdc);
	return 0;
}

void SegmentEditor::drawSegment(CDC* cdc, ILWIS::Segment *s, bool fExact)
{
	CPen pen;
	CPen* penOld = 0;
	if (!fExact) {
		long iRaw = s->iValue();
		Color clr = clrRaw(iRaw);
		if ((long)clr == iUNDEF)
			fExact = true;
		else {
			pen.CreatePen(PS_SOLID,1,clr);
			penOld = cdc->SelectObject(&pen);
		}
	}
	long iNr, iTot;
	iTot = -1;
	CoordinateSequence *crdBuf = s->getCoordinates();
	iNr = crdBuf->size();
	zPoint* p = new zPoint[iNr];
	for (int i = 0; i < iNr; ++i) {
		Coord c = crdBuf->getAt(i);
		zPoint pnt = mpv->pntPos(c);
		if (iTot < 0 || p[iTot] != pnt)
			p[++iTot] = pnt;
	}
	delete crdBuf;
	if (iTot >= 1)
		cdc->Polyline(p, iTot+1);
	zRect rect;
	mpv->GetClientRect(&rect);
	if (fRetouching || fShowNodes) {
		CGdiObject* brOld = cdc->SelectStockObject(HOLLOW_BRUSH);
		zPoint pnt;
		pnt = p[0];
		if (rect.PtInRect(pnt))
			cdc->Rectangle(pnt.x-3,pnt.y-3,pnt.x+4,pnt.y+4);
		pnt = p[iTot];
		if (rect.PtInRect(pnt))
			cdc->Rectangle(pnt.x-3,pnt.y-3,pnt.x+4,pnt.y+4);
		cdc->SelectObject(brOld);
	}
	if (fRetouching) {  
		for (int i = 0; i < iTot; ++i) {
			zPoint pnt = p[i];
			if (rect.PtInRect(pnt)) {
				cdc->MoveTo(pnt.x-3,pnt.y-3);
				cdc->LineTo(pnt.x+4,pnt.y+4);
				cdc->MoveTo(pnt.x+3,pnt.y-3);
				cdc->LineTo(pnt.x-4,pnt.y+4);
			}
		}
	}
	if (penOld)
		cdc->SelectObject(penOld);
	delete [] p;
}

int SegmentEditor::drawCoords(CDC* cdc, Color clr)
{
  if (iNrCoords) {
    zPoint p;
    int i;
    Color c;
    if ((long)clr == -1)
      c = SysColor(COLOR_WINDOWTEXT);
    else
      c = clr;
		CPen pen(PS_SOLID,1,c);
		CPen* penOld = cdc->SelectObject(&pen);
    p = mpv->pntPos(coords[0]);
    cdc->MoveTo(p);
    for (i = 1; i < iNrCoords; ++i) {
      p = mpv->pntPos(coords[i]);
      cdc->LineTo(p);
    }
		cdc->SelectObject(penOld);
		if (!cLast.fUndef()) {
			zPoint pPivot = mpv->pntPos(cPivot);
			zPoint pLast = mpv->pntPos(cLast);
			if (pPivot != pLast) {
				cdc->SetROP2(R2_NOT);
				cdc->MoveTo(pPivot);
				cdc->LineTo(pLast);
				cdc->SetROP2(R2_COPYPEN);
			}
		}
  }
  return 0;
}

void SegmentEditor::drawActNode(CDC* cdc)
{
	if (crdActNode.fUndef())
		return;
	CGdiObject* brOld = cdc->SelectStockObject(HOLLOW_BRUSH);
	CPen pen(PS_SOLID,1,colRetouch);
	CPen* penOld = cdc->SelectObject(&pen);
	Coord c =crdActNode;
	zPoint pnt = mpv->pntPos(c);
	zPoint p1(pnt.x-3, pnt.y-3);
	zPoint p2(pnt.x+4, pnt.y+4);
	cdc->Rectangle(p1.x,p1.y, p2.x,p2.y);
	cdc->SelectObject(brOld);
	cdc->SelectObject(penOld);
}

bool SegmentEditor::OnLButtonDown(UINT nFlags, CPoint point)
{
	mpv->SetFocus();
	Coord crd = mpv->crdPnt(point);
	if (fDigBusy) {
		MessageBeep(MB_ICONASTERISK);
		return true;
	}
	if (!cb.fUndef() && !cb.fContains(crd)) {
		MessageBeep(-1);
		return true;
	}  
	iLastButton = 0;
	bool fShift = MK_SHIFT & nFlags ? true : false;
	bool fCtrl = MK_CONTROL & nFlags ? true : false;
	switch (mode) {
	case modeSELECT:
		{
			CClientDC cdc(mpv);
			if (!fShift && !fCtrl && segList.iSize()) {
				/*	  
				zColor cWin = SysColor(COLOR_WINDOW);
				dsp->pushPen(new zPen(cWin,Solid,3));
				for (SLIter<Segment> iter(&segList); iter.fValid(); ++iter)
				drawSegment(iter(),true);
				delete dsp->popPen();
				zColor c;
				if ((long)col == -1)
				c = SysColor(COLOR_WINDOWTEXT);
				else
				c = col;
				*/	    
				//dsp->pushPen(new zPen(c,Solid,1));
				for (SLIter<ILWIS::Segment *> iter(&segList); iter.fValid(); iter.first()) {
					SetDirty(iter());
					//drawSegment(iter(),false);
					iter.remove();
				}
				//delete dsp->popPen();
			}
			long iAft;
			long index = 0;
			ILWIS::Segment *seg = NULL;
			Coord crd1 = crd;
			crd1 = sm->crdPoint(crd1, &seg, iAft, fUndelete && !fShift && !fCtrl);
			Coord cFnd = crd1;
			zPoint p = mpv->pntPos(crd);
			zPoint pFnd = mpv->pntPos(cFnd);
			zPoint pTst = pFnd - p;
			if (sqr((double)pTst.x) + sqr((double)pTst.y) > sqr(iSnapPixels))
				MessageBeep(MB_ICONASTERISK);
			else {
				bool fAppend = true;
				if (fShift) {
					fAppend = !fCtrl;
					SLIter<ILWIS::Segment *> iter(&segList);
					for (int count=0; iter.fValid(); ++iter,++count)
						if (count == index) {
							iter.remove();
							fAppend = false;
							Color cWin = SysColor(COLOR_WINDOW);
							CPen pen3(PS_SOLID,3,cWin);
							CPen* penOld = cdc.SelectObject(&pen3);
							drawSegment(&cdc,seg,true);
							Color c;
							if ((long)col == -1)
								c = SysColor(COLOR_WINDOWTEXT);
							else
								c = col;
							cdc.SelectObject(penOld);
							CPen pen(PS_SOLID,1,c);
							penOld = cdc.SelectObject(&pen);
							drawSegment(&cdc,seg,false);
							cdc.SelectObject(penOld);
							break;
						}
				}
				if (fAppend) {
					if (seg->fDeleted())
						seg->Delete(false);
					segList.append(seg);
					Color cFgBr = SysColor(COLOR_HIGHLIGHT);
					CPen penFg(PS_SOLID,3,cFgBr);
					CPen* penOld = cdc.SelectObject(&penFg);
					drawSegment(&cdc,seg,true);
					cdc.SelectObject(penOld);
					Color c;
					if ((long)col == -1)
						c = SysColor(COLOR_WINDOWTEXT);
					else
						c = col;
					CPen pen(PS_SOLID,1,c);
					penOld = cdc.SelectObject(&pen);
					drawSegment(&cdc,seg,false);
					cdc.SelectObject(penOld);
				}
			}
		}
		break;
	case modeMOVE:
		removeDigCursor();
		crdDig = Coord();
		TakePointMouse(crd);
		fDigBusy = false;
		if (coords.size() > 0)
			Mode(modeMOVING);
		break;
	case modeMOVING:
		if (fCtrl) {
			int iRet = MergeSplit(crd);
			curActive = curSegMoving;
			OnSetCursor();
			if (0 != iRet)
				break;
		}
		if (fShift)
			SnapPointMouse(crd);
		else
			NewPosPointMouse(crd);
		if (coords.size() > 0)
			Mode(modeMOVE);
		break;
	case modeADD:
		if (fCtrl)
			SnapSplitSegmentMouse(crd);
		else if (fShift)
			SnapSegmentMouse(crd);
		else
			BeginSegmentMouse(crd);
		fDigBusy = false;
		if (coords.size() > 0)
			Mode(modeENTERING);
		break;
	case modeENTERING:
		if (MK_LBUTTON & nFlags)
			EnterPoints(crd);
		else if (MK_RBUTTON & nFlags) {
			DeleteLastPoint(crd);
			if (coords.size() > 0)
				Mode(modeADD);
		}
		break;
	case modeSPLIT:
		MergeSplit(crd);
		break;
	}
	return true;
}

bool SegmentEditor::OnLButtonUp(UINT nFlags, CPoint point)
{
	Coord crd = mpv->crdPnt(point);

  if (!cb.fUndef() && !cb.fContains(crd)) {
    MessageBeep(-1);
    return true;
  }  
	bool fShift = MK_SHIFT & nFlags ? true : false;
	bool fCtrl = MK_CONTROL & nFlags ? true : false;
  switch (mode) {
    case modeMOVING:
      if (fCtrl) {
        int iRet = MergeSplit(crd);
		    curActive = curSegMoving;
        OnSetCursor();
        if (0 != iRet)
          break;
      }
      if (fShift)
				SnapPointMouse(crd);
      else {
        zPoint pOrig = mpv->pntPos(cPivot);
        zPoint pNewPos = mpv->pntPos(crd);
        zPoint pTst = pOrig - pNewPos;
        if (sqr((double)pTst.x) + sqr((double)pTst.y) <= sqr(min(3, iSnapPixels)))
          return true; // just accept release of mouse button
				NewPosPointMouse(crd);
      }
      if (coords.size() > 0)
				Mode(modeMOVE);
      break;
    case modeENTERING:
      if (0 == (MK_RBUTTON & nFlags))
        if (fCtrl)
          SnapSplitEndSegmentMouse(crd);
				else if (fShift) 
          SnapEndSegmentMouse(crd);
        else if (fAutoSnap)  
          AutoEndSegmentMouse(crd);
      break;
  }
  return true;
}

  class PointCoordForm: public FormWithDest
  {
  public:
    PointCoordForm(CWnd* parent, const char* sTitle, Coord* crd)
    : FormWithDest(parent, sTitle)
    {
			new FieldReal(root, SEDUiX, &crd->x);
			new FieldReal(root, SEDUiY, &crd->y);
//      SetMenHelpTopic(htpPntEditAdd);
      create();
    }
  };

bool SegmentEditor::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	Coord crd = mpv->crdPnt(point);
  if (fDigBusy) {
    return true;
  }
  if (!cb.fUndef() && !cb.fContains(crd)) {
    return true;
  }  
  switch (mode) {
		case modeSELECT:
			Edit(crd);
			return true;
    case modeENTERING:
      EndSegment(crd);
      Mode(modeADD);
			return true;
    case modeMOVE: 
		case modeMOVING:
		{
			MovePoints(Coord());			
			TakePointMouse(crd);
		  fDigBusy = false;
      if (coords.size() > 0)
				return true;
			crd = cPivot;
			PointCoordForm frm(mpv, SEDTitleEditPoint.scVal(), &crd);
			if (frm.fOkClicked())
				NewPosPoint(crd);
			Mode(modeMOVE);
		} return true;
	}
	MessageBeep(-1);
	return true;
}

bool SegmentEditor::OnMouseMove(UINT nFlags, CPoint point)
{
	Coord crd = mpv->crdPnt(point);
  switch (mode) {
    case modeMOVING:
      if (!cb.fUndef() && !cb.fContains(crd)) 
        MessageBeep(-1);
      MoveCursorPoint(crd);
      return true;
    case modeENTERING:
      if (!cb.fUndef() && !cb.fContains(crd)) {
        MessageBeep(-1);
        return true;
      }  
      if (MK_RBUTTON & nFlags) {
				DeleteLastPoint(crd);
				if (coords.size() > 0)
					Mode(modeADD);
			}
			else if ((MK_LBUTTON & nFlags) && 0 == mpv->as)
				EnterPoints(crd);
      else
				MovePivot(crd);
      return true;
    default:  
      return false;
  }
}

void SegmentEditor::OnUpdateEdit(CCmdUI* pCmdUI)
{
  BOOL fEdit = segList.iSize() != 0;
	pCmdUI->Enable(fEdit);
}

void SegmentEditor::OnUpdateClear(CCmdUI* pCmdUI)
{
	BOOL fClear = FALSE;
  switch (mode) {
    case modeENTERING:
			fClear = TRUE;
      break;
    case modeSELECT:
      if (segList.iSize()) 
				fClear = TRUE;
      break;
  }
	pCmdUI->Enable(fClear);
}
 
void SegmentEditor::EditAttrib(int iRec)
{
  Table tbl = sm->tblAtt();
  if (tbl.fValid()) {
		tbl->CheckNrRecs();
    Ilwis::Record rec = tbl->rec(iRec);
		mpv->ShowRecord(rec);
	}
}

int SegmentEditor::Edit(const Coord& c)
{
	SLIter<ILWIS::Segment*> iter(&segList);
	if (segList.iSize() == 0)
		return 0;
	sValue = iter()->sValue(sm->dvrs());

	DomainUniqueID* duid = dm()->pdUniqueID();
	if (segList.iSize() == 1) {
		if (duid) {
			int iRaw = iter()->iValue();
			EditAttrib(iRaw);
		}
		else if (c.fUndef()) 
		{
AskID:      
			if (AskValue("", htpSegEditorAskValue)) 
			{
				if (dm()->pdid()) 
				{
					ILWIS::Segment *seg = (ILWIS::Segment *)sm->getFeature(sValue);
					if (seg->fValid()) {
						MessageBeep(MB_ICONEXCLAMATION);
						int iRet = mpv->MessageBox(SEDMsgValInUse.sVal(), SEDMsgSegEditor.sVal(),
							MB_YESNO|MB_DEFBUTTON2|MB_ICONASTERISK);
						if (IDYES != iRet) 
							goto AskID;
					}  
				}  
				for (; iter.fValid(); ++iter) {
					iter()->PutVal(sm->dvrs(), sValue);
					SetDirty(iter());
				}
				sm->Updated();
			}
		}
		else
			EditFieldStart(c, sValue);
	}
	else {
		if (dm()->pdid()) {
			mpv->MessageBox(SEDMsgSegEdOnlyIndiv.sVal(), SEDMsgSegEditor.sVal());
		}
		else {
			long iSel = segList.iSize();
			String sRemark(SEDRemSegSel_i.sVal(), iSel);
			if (AskValue(sRemark, htpSegEditorAskValue)) {
				for (; iter.fValid(); ++iter) {
					iter()->PutVal(sm->dvrs(), sValue);
					sm->Updated();
					SetDirty(iter());
				}
			}
		}
	}
	return 1;
}

void SegmentEditor::EditFieldOK(Coord crd, const String& s)
{
  if (dm()->pdid()) {
    ILWIS::Segment *seg = (ILWIS::Segment *)sm->getFeature(s);
    if (seg->fValid()) {
      MessageBeep(MB_ICONEXCLAMATION);
      int iRet = mpv->MessageBox(SEDMsgValInUse.sVal(), SEDMsgSegEditor.sVal(),
                   MB_YESNO|MB_DEFBUTTON2|MB_ICONASTERISK);
      if (IDYES != iRet) {
        EditFieldStart(crd, s);
        return;
      }           
    }
  }  
  if (segList.iSize() == 1) {
    SLIter<ILWIS::Segment *> iter(&segList);
	iter()->PutVal(sm->dvrs(), s);
    sm->Updated();
    SetDirty(iter());
	int iRaw = iter()->iValue();
	EditAttrib(iRaw);
  }
}

bool SegmentEditor::fCopyOk()
{
  return segList.iSize() != 0;
}

bool SegmentEditor::fPasteOk()
{
  return IsClipboardFormatAvailable(iFmtPnt) ? true : false;
}

void SegmentEditor::OnUpdateCopy(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(fCopyOk());
}

void SegmentEditor::OnUpdatePaste(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(fPasteOk());
}

void SegmentEditor::OnCopy()
{
	if (!fCopyOk())
		return;
	CWaitCursor curWait;
	if (!mpv->OpenClipboard())
		return;
	EmptyClipboard();

	long iSize = 1;
	long iNr;
	CoordinateSequence *crdBuf;
	for (SLIter<ILWIS::Segment *> iterInit(&segList); iterInit.fValid(); ++iterInit) {
		crdBuf = iterInit()->getCoordinates();
		iSize += crdBuf->size() + 1;
		delete crdBuf;
	}
	IlwisPoint* ip = new IlwisPoint[iSize];
	if (0 == ip)
		return;
	ip[0].c = Coord();
	ip[0].iRaw = iSize-1;
	long k = 1;
	Coord crd;
	const int iSIZE = 1000000;
	char* sBuf = new char[iSIZE];
	char* s = sBuf;
	String str, sVal;
	long iTotLen = 0;
	int iLen;

	for (SLIter<ILWIS::Segment *> iter(&segList); iter.fValid(); ++iter) {
		const ILWIS::Segment *seg = iter();
		crdBuf = iter()->getCoordinates();
		iNr = crdBuf->size();
		long iRaw = iter()->iValue();
		for (int j = 0; j < iNr; ++j) {
			ip[k].c = crdBuf->getAt(j);
			ip[k].iRaw = iRaw;
			++k;
		}
		ip[k].c = Coord();
		ip[k].iRaw = iUNDEF;
		++k;

		if (iTotLen > iSIZE) 
			continue;
		for (int j = 0; j < iNr; ++j) {
			crd = crdBuf->getAt(j);
			if (0 == j)
				str = String("%g\t%g\t%S\r\n", crd.x, crd.y, iter()->sValue(sm->dvrs()));
			else
				str = String("%g\t%g\r\n", crd.x, crd.y);
			iLen = str.length();
			iTotLen += iLen;
			if (iTotLen > iSIZE) 
				continue;
			strcpy(s, str.sVal());
			s += iLen;
		} 
		delete crdBuf;
		iTotLen++;
		*s++ = '\r';
		*s++ = '\n';
	}
	iLen = (1+iSize) * sizeof(IlwisPoint);
	HANDLE hnd = GlobalAlloc(GMEM_MOVEABLE,iLen);
	void* pv = GlobalLock(hnd);
	memcpy(pv, ip, iLen);
	GlobalUnlock(hnd);
	SetClipboardData(iFmtPnt, hnd);

	// Ilwis Domain Format
	IlwisDomain* id = new IlwisDomain(sm->dm(), sm->vr());
	iLen = sizeof(IlwisDomain);
	hnd = GlobalAlloc(GMEM_MOVEABLE,iLen);
	pv = GlobalLock(hnd);
	memcpy(pv, id, iLen);
	GlobalUnlock(hnd);
	SetClipboardData(iFmtDom, hnd);
	delete id;

	*s = '\0';
	hnd = GlobalAlloc(GMEM_FIXED, strlen(sBuf)+2);
	char* pc = (char*)GlobalLock(hnd);
	strcpy(pc,sBuf);
	GlobalUnlock(hnd);
	SetClipboardData(CF_TEXT,hnd);
	delete ip;
	delete sBuf;

	MapCompositionDoc* mcd = mpv->GetDocument();
	zRect rect;
	mpv->GetClientRect(rect);
	MinMax mm = mpv->mmRect(rect);
	MetafilePositioner psnMf(mm,mcd->georef); 
	zRect rectMFD = psnMf.rectSize();
	CClientDC cdc(mpv);
	CMetaFileDC mfd;
	int iWidthMM = cdc.GetDeviceCaps(HORZSIZE);
	int iWidthPels = cdc.GetDeviceCaps(HORZRES);
	int iMMPerPel = (iWidthMM * 100)/iWidthPels;
	zRect rectMM = rectMFD;
	rectMM.left() *= iMMPerPel;
	rectMM.top() *= iMMPerPel;
	rectMM.right() *= iMMPerPel;
	rectMM.bottom() *= iMMPerPel;
	String sDescr("ILWIS 3.0\0%S\0", sm->sDescr());
	//	mfd.CreateEnhanced(&cdc,0,rectMM,sDescr.sVal());
	mfd.CreateEnhanced(&cdc,0,0,sDescr.sVal());
	mfd.SetMapMode(MM_ISOTROPIC);
	mfd.SetWindowOrg(0,0);
	mfd.SetWindowExt(32767,32767);
	mfd.SetViewportOrg(0,0);
	mfd.SetViewportExt(32767,32767);
	fOnlySelected = true;
	bool fDummyDraw = false;
	draw(&mfd,rect,&psnMf, &fDummyDraw);
	fOnlySelected = false;
	HENHMETAFILE hEMF = mfd.CloseEnhanced();
	SetClipboardData(CF_ENHMETAFILE,hEMF);

	CloseClipboard();
}

void SegmentEditor::OnPaste()
{
  if (!fPasteOk()) return;
  CoordBuf crdBuf(1000);
  unsigned int iSize;
	
	CWaitCursor curWait;
	if (!mpv->OpenClipboard())
		return;

  bool fConvert = false, fValues = false, fSort = false;
  Domain dmMap, dmCb;
  ValueRange vrCb;
  if (IsClipboardFormatAvailable(iFmtDom)) {
    dmMap = sm->dm();
		HANDLE hnd = GetClipboardData(iFmtDom);
		iSize = (unsigned int)GlobalSize(hnd);
    IlwisDomain id;
		if (sizeof(id) < iSize)
			iSize = sizeof(id);
		memcpy(&id, (char*)GlobalLock(hnd),iSize);
		GlobalUnlock(hnd);
    dmCb = id.dm();
    if (dmMap->pdv()) {
      if (0 == dmCb->pdv()) {
        mpv->MessageBox(SEDErrNotValueInClipboard.sVal(),SEDErrSegEditor.sVal(),MB_OK|MB_ICONSTOP);
 				CloseClipboard();
        return;
      }
      ValueRange vrMap = sm->vr();
      vrCb = id.vr();
      fValues = true;
      if (vrMap != vrCb)
        fConvert = true;
    }
    else if (dmMap->pdc()) {
      if (0 == dmCb->pdc()) {
        mpv->MessageBox(SEDErrNotClassInClipboard.sVal(),SEDErrSegEditor.sVal(),MB_OK|MB_ICONSTOP);
 				CloseClipboard();
        return;
      }
      fSort = true;
      if (dmMap != dmCb)
        fConvert = true;
    }
    else {
      if (dmMap != dmCb)
        fConvert = true;
      if (dmMap->pdsrt())
        fSort = true;
    }
  }

	HANDLE hnd = GetClipboardData(iFmtPnt);
	iSize = (unsigned int)GlobalSize(hnd);
	char* cp = new char[iSize];
	memcpy(cp, (char*)GlobalLock(hnd),iSize);
  IlwisPoint* ip = (IlwisPoint*) cp;
  iSize /= sizeof(IlwisPoint);
  iSize = ip[0].iRaw;
  unsigned int i;
  int j = 0;
  long iRaw = iUNDEF;
  double rVal = rUNDEF;
  for (i = 0; i < iSize; ++i) {
    if (ip[1+i].c.fUndef()) {
      if (j > 1) {
        ILWIS::Segment *seg = CSEGMENT(sm->newFeature());
        seg->PutCoords(j,crdBuf);
        if (fValues)
          seg->PutVal(rVal);
        else
			seg->PutVal(iRaw);
        sm->Updated();
		CClientDC cdc(mpv);
        drawSegment(&cdc,seg,false);
      }  
      j = 0;
      continue;
    }  
    if (j == 0) {
      iRaw = ip[1+i].iRaw;
      if (fConvert) {
        if (fValues) {
          if (vrCb.fValid())
            rVal = vrCb->rValue(iRaw);
          else
            rVal = iRaw;
        }
        else {
          String sVal;
          if (vrCb.fValid())
            sVal = vrCb->sValueByRaw(dmCb,iRaw, 0);
          else
            sVal = dmCb->sValueByRaw(iRaw, 0);
          if (fSort) {
            if ("?" == sVal) {
              iRaw = iUNDEF;
              continue;
            }
            iRaw = dmMap->iRaw(sVal);
            if (iUNDEF == iRaw) {
              String sMsg(SEDMsgNotInDomain_SS.sVal(), sVal, dmMap->sName());
              int iRet = mpv->MessageBox(sMsg.sVal(),SEDMsgSegEditor.sVal(),MB_YESNOCANCEL|MB_ICONASTERISK);
              if (IDYES == iRet)
                try {
                  iRaw = dmMap->pdsrt()->iAdd(sVal);
                }
                catch (ErrorObject& err) {
                  err.Show();
                  iRaw = iUNDEF;
                }
              else if (IDCANCEL == iRet)
                break;
              else
                continue;
            }
          }
        }
      }
    }
    crdBuf[j++] = ip[1+i].c;
  }
  if (j > 1) {
    ILWIS::Segment *seg = CSEGMENT(sm->newFeature());
    seg->PutCoords(j,crdBuf);
    if (fValues)
      seg->PutVal(rVal);
    else
      seg->PutVal(iRaw);
    sm->Updated();
	CClientDC cdc(mpv);
    drawSegment(&cdc,seg,false);
  }  
	CloseClipboard();
}

void SegmentEditor::OnSelectMode()
{
	Mode(modeSELECT);
}

void SegmentEditor::OnMoveMode()
{
	Mode(modeMOVE);
}

void SegmentEditor::OnInsertMode()
{
  if (mode == modeENTERING) 
  {
  	mpv->OnNoTool();
    return;
  }
	Mode(modeADD);
}

void SegmentEditor::OnSplitMode()
{
	Mode(modeSPLIT);
}

void SegmentEditor::OnUnDelSeg()
{
  if (mode != modeSELECT) 
    Mode(modeSELECT);
  fUndelete = !fUndelete;
  mpv->Invalidate();
}

void SegmentEditor::OnFindUndefs()
{
  if (mode != modeSELECT) 
    Mode(modeSELECT);
  fFindUndefs = !fFindUndefs;
  mpv->Invalidate();
}

void SegmentEditor::OnUpdateMode(CCmdUI* pCmdUI)
{
	BOOL fCheck, fEnable;
	switch (pCmdUI->m_nID) {
		case ID_SELECTMODE:
			fCheck = modeSELECT == mode;
      fEnable = modeENTERING != mode && modeMOVING != mode;
			break;
		case ID_MOVEMODE:
			fCheck = modeMOVE == mode || modeMOVING == mode;
      fEnable = modeENTERING != mode;
			break;
		case ID_INSERTMODE:
			fCheck = modeADD == mode || modeENTERING == mode;
      fEnable = modeMOVING != mode;
			break;
		case ID_SPLITMODE:
			fCheck = modeSPLIT == mode;
      fEnable = modeENTERING != mode && modeMOVING != mode;
			break;
		case ID_UNDELSEG:
			pCmdUI->SetCheck(fUndelete);
			return;
		case ID_FINDUNDEFS:
			pCmdUI->SetCheck(fFindUndefs);
			return;
	}
	if (0 != mpv->as)
		fCheck = false;
	pCmdUI->SetRadio(fCheck);
  pCmdUI->Enable(fEnable);
}

void SegmentEditor::Mode(enumMode Mode) 
{
	mpv->OnNoTool();
	if (mode == Mode)
		return;
	if (mode == modeMOVE && Mode != modeMOVING) {
		mode = Mode;
		AddSegments(Coord());
	}  
	if (modeENTERING == mode)
		while (coords.size() > 0)
			DeleteLastPoint(cPivot);
	if (fUndelete) {
		fUndelete = false;
		mpv->Invalidate();
	}
	if (fFindUndefs) {
		fFindUndefs = false;
		mpv->Invalidate();
	}
	if (segList.iSize()) {
		for (SLIter<ILWIS::Segment *> iter(&segList); iter.fValid(); iter.first()) {
			SetDirty(iter());
			iter.remove();
		}  
	}
	mode = Mode;
	switch (mode) {
	case modeSELECT:
		GreyDigitizer(false);
		curActive = curEdit;
		break;
	case modeMOVE:
		GreyDigitizer(false);
		iLastButton = 0;
		MovePoints(Coord());
		curActive = curSegMove;
		break;
	case modeMOVING:
		GreyDigitizer(true);
		curActive = curSegMoving;
		//zCursor cur;
		//cur.sysId() = 0;
		//mappane->setCursor(cur);
		break;
	case modeADD:
		GreyDigitizer(false);
		curActive = curSegEdit;
		break;
	case modeENTERING:
		GreyDigitizer(true);
		break;
	case modeSPLIT:
		GreyDigitizer(false);
		curActive = curSegSplit;
		break;
	}
	OnSetCursor();
}

void SegmentEditor::SetDirty(ILWIS::Segment *s)
{
  CoordBounds cb = s->cbBounds();
  zPoint p1 = mpv->pntPos(cb.cMin);
  zPoint p2 = mpv->pntPos(cb.cMax);
  zPoint p3 = mpv->pntPos(Coord(cb.MinX(), cb.MaxY()));
  zPoint p4 = mpv->pntPos(Coord(cb.MaxX(), cb.MinY()));
  zRect rect;
  rect.left() = min(min(p1.x,p2.x),min(p3.x,p4.x)) - 4;
  rect.right() = max(max(p1.x,p2.x),max(p3.x,p4.x)) + 5;
  rect.top() = min(min(p1.y,p2.y),min(p3.y,p4.y)) - 4;
  rect.bottom() = max(max(p1.y,p2.y),max(p3.y,p4.y)) + 5;
  mpv->InvalidateRect(&rect);
}

bool SegmentEditor::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
  bool fCtrl = GetKeyState(VK_CONTROL) & 0x8000 ? true : false;
  bool fShift = GetKeyState(VK_SHIFT) & 0x8000 ? true : false;
  switch (nChar) {
    case VK_ESCAPE:
      switch (mode) {
				case modeSELECT:
					if (fUndelete) {
						fUndelete = false;
						mpv->Invalidate();
					}
					if (fFindUndefs) {
						fFindUndefs = false;
						mpv->Invalidate();
					}
					return true;
				case modeADD:
				case modeMOVE:
				case modeSPLIT:
					Mode(modeSELECT);
					return true;
				case modeMOVING:
					Mode(modeMOVE);
					return true;
				case modeENTERING:
					Mode(modeADD);
					return true;
      }
      break;
    case VK_INSERT:
      if (!fShift && !fCtrl) {
	      switch (mode) {
					case modeSELECT:
						Mode(modeADD);
						return true;
					case modeADD:
						Mode(modeSELECT);
						return true;
				}
				return true;
      }
      break;
    case VK_DELETE:
      if (!fShift && !fCtrl && mode == modeSELECT) {
        OnClear();
        return true;
      }
			if (mode == modeENTERING) {
				DeleteLastPoint(cPivot);
				return true;
			}
      break;
    case VK_RETURN:
      if (!fShift && !fCtrl && mode == modeSELECT) {
				Edit(Coord());
				return true;
      }
      break;
    case VK_SPACE:
      if (mode == modeENTERING) {
        EndSegment(cLast);
        Mode(modeADD);
				return true;
      }
      break;
  }
  return false;
}

void SegmentEditor::OnUpdateSetVal(CCmdUI* pCmdUI)
{
  bool fID = 0 != dm()->pdid();
  pCmdUI->Enable(!fID);
}

void SegmentEditor::OnSetVal()
{
  AskValue(SEDRemValForSeg, htpSegEditorAskInsertCode);
}

void SegmentEditor::OnCut()
{
	OnCopy();
	OnClear();
}

void SegmentEditor::OnClear()
{
	switch (mode) {
	case modeENTERING:
		DeleteLastPoint(cPivot);
		if (coords.size() > 0)
			Mode(modeADD);
		break;
	case modeSELECT:
		if (segList.iSize()) {
			for (SLIter<ILWIS::Segment *> iter(&segList); iter.fValid(); iter.first()) {
				SetDirty(iter());
				iter()->Delete(true);
				iter.remove();
				sm->Updated();
			}
		}
		break;
	}
}



int SegmentEditor::AddSegments(Coord crd)
{
  if (iLastButton == 4)
    return 1;
  SetActNode(Coord());
  if (mode == modeMOVE) {
    Mode(modeADD);
  }
//  else {  
    fDigBusy = false;
    if (fRetouching) {
      fRetouching = false;
      mpv->Invalidate();
    }
    dc = dcCROSS;
    MoveCursor(crd);
    ChangeWindowBasis = (DigiFunc)&SegmentEditor::AddSegments;
    return SetDigiFunc(SEDDigAddSegments, (DigiFunc)&DigiEditor::MoveCursor,
		     (DigiFunc)&SegmentEditor::BeginSegment, SEDDigBeginSegment,
		     (DigiFunc)&SegmentEditor::SnapSegment, SEDDigSnapSegment,
		     (DigiFunc)&DigiEditor::ChangeWindow, SEDDigChangeWindow,
		     (DigiFunc)&SegmentEditor::MovePoints, SEDDigMovePoints);
//  }		     
}

int SegmentEditor::BeginSegment(Coord c)
{
  if (iLastButton == 1)
    return 1;
  SetActNode(Coord());
  fDigBusy = true;
  coords.clear();
  coords.push_back(c);
  iNrCoords = 1;
  cPivot = cLast = c;
  return StartEnteringPoints(c);
}

int SegmentEditor::SnapSegment(Coord c)
{
  if (iLastButton == 2)
    return 1;
  fDigBusy = true;
  Coord crdSnap = sm->crdNode(c);
  Coord cSnap = crdSnap;
  double rSnapDist = rDist(c, cSnap);
  if (!crdSnap.fUndef() && (rSnapDist < sm->rSnapDist)) {
    SetActNode(crdSnap);
    cLast = cSnap;
    return SetDigiFunc(SEDDigSnapSegment, (DigiFunc)&DigiEditor::MoveCursor,
		(DigiFunc)&SegmentEditor::SnapSegmentAccept, SEDDigAccept,
		(DigiFunc)&SegmentEditor::SnapSegment, SEDDigSnapOther,
		(DigiFunc)&SegmentEditor::SnapSplitSegment, SEDDigSnapSplit,
		(DigiFunc)&SegmentEditor::AddSegments, SEDDigNoSnap);
  }
  else {
    MessageBeep(MB_ICONASTERISK);
    return SetDigiFunc(SEDDigSnapSegment,
                (DigiFunc)&DigiEditor::MoveCursor,
		NULL, "",
		(DigiFunc)&SegmentEditor::SnapSegment, SEDDigSnapOther,
		(DigiFunc)&SegmentEditor::SnapSplitSegment, SEDDigSnapSplit,
		(DigiFunc)&SegmentEditor::AddSegments, SEDDigNoSnap);
  }
/*
  if (rDist2(c, cSnap) > sqr(sm->rSnapDist))
    return SnapSplitSegment(c);
  else
    return BeginSegment(cSnap);
*/
}

int SegmentEditor::SnapSegmentAccept(Coord)
{
  return BeginSegment(cLast);
}

int SegmentEditor::BeginSegmentMouse(Coord c)
{
  if (fAutoSnap) {
    Coord crdSnap = sm->crdNode(c);
    Coord cSnap = crdSnap;
    zPoint p = mpv->pntPos(c);
    zPoint pSnap = mpv->pntPos(cSnap);
    zPoint pTst = pSnap - p;
    if (!crdSnap.fUndef() && (sqr((double)pTst.x) + sqr((double)pTst.y) <= sqr(iSnapPixels)))
      return BeginSegment(cSnap);
    else {
      long iAft;
      crdSnap = sm->crdPoint(c,NULL,iAft);
      cSnap = crdSnap;
      zPoint pSnap = mpv->pntPos(cSnap);
      zPoint pTst = pSnap - p;
      if (!crdSnap.fUndef() && (sqr((double)pTst.x) + sqr((double)pTst.y) <= sqr(iSnapPixels))) {
        if (0 == AskSplit(currentSeg, iAft, crdSnap))
          return BeginSegment(cSnap);
        else
          return 0;
      }
    }
  }
  return BeginSegment(c);
}

int SegmentEditor::SnapSegmentMouse(Coord c)
{
  Coord crdSnap = sm->crdNode(c);
  Coord cSnap = crdSnap;
  zPoint p = mpv->pntPos(c);
  zPoint pSnap = mpv->pntPos(cSnap);
  zPoint pTst = pSnap - p;
  if (crdSnap.fUndef() || (sqr((double)pTst.x) + sqr((double)pTst.y) > sqr(iSnapPixels))) {
    MessageBeep(MB_ICONASTERISK);
    return 1;
  }
  return BeginSegment(cSnap);
}

int SegmentEditor::SnapSplitSegmentMouse(Coord c)
{
  long iAft;
  Coord crdSnap = sm->crdPoint(c,NULL,iAft);
  Coord cSnap = crdSnap;
  zPoint p = mpv->pntPos(c);
  zPoint pSnap = mpv->pntPos(cSnap);
  zPoint pTst = pSnap - p;
  if (crdSnap.fUndef() || (sqr((double)pTst.x) + sqr((double)pTst.y) > sqr(iSnapPixels))) {
    MessageBeep(MB_ICONASTERISK);
    return 0;
  }
  if (0 == AskSplit(currentSeg, iAft, crdSnap))
    return BeginSegment(cSnap);
  else
    return 0;
}

int SegmentEditor::MovePivot(Coord c)
{
  zPoint pLast = mpv->pntPos(cLast);
  zPoint p = mpv->pntPos(c);
  cLast = c; // always even if point in windows remains the same
  if (pLast != p) {
		CClientDC cdc(mpv);
		cdc.SetROP2(R2_NOT);
    zPoint pPivot = mpv->pntPos(cPivot);
    if (pPivot != pLast) {
      cdc.MoveTo(pPivot);
      cdc.LineTo(pLast);
    }
    pLast = p;
    cLast = c;
    if (pPivot != pLast) {
      cdc.MoveTo(pPivot);
      cdc.LineTo(pLast);
    }
		cdc.SetROP2(R2_COPYPEN);
  }
  MoveCursor(c);
  return 0;
}

int SegmentEditor::EnterPoints(Coord c)
{
  MovePivot(c);
  if (iNrCoords >= iMAXCOORDS) {
    MessageBeep(MB_ICONASTERISK);
    TunnelSegment();
    return 0;
  }
  iNrCoords++;
  cLast = c;
  coords.push_back(c);
  zPoint pLast = mpv->pntPos(cLast);
  zPoint pPivot = mpv->pntPos(cPivot);
  cPivot = cLast;
  if (pPivot != pLast) {
    Color c;
    if ((long)col == -1)
      c = SysColor(COLOR_WINDOWTEXT);
    else
      c = col;
		CClientDC cdc(mpv);
		CPen pen(PS_SOLID,1,c);
		CPen* penOld = cdc.SelectObject(&pen);
    cdc.MoveTo(pPivot);
    cdc.LineTo(pLast);
    cdc.SelectObject(penOld);
    pPivot = pLast;
  }
  return 0;
}

int SegmentEditor::EndSegment(Coord c)
{
	EnterPoints(c);
	TunnelSegment();
	if (!fDigBusy && iNrCoords == 2) {
		zPoint p1 = mpv->pntPos(coords[0]);
		zPoint p2 = mpv->pntPos(coords[1]);
		zPoint pTst = p1 - p2;
		unsigned long iDist = (unsigned long)(sqr((double)pTst.x) + sqr((double)pTst.y));
		if (iDist < sqr(iSnapPixels)) {
			MessageBeep(MB_ICONASTERISK);
			while (coords.size() > 0)
				DeleteLastPoint(c);
			return 1;
		}
	}

	currentSeg = CSEGMENT(sm->newFeature());

	CoordBuf crdBuf(iNrCoords);
	for(int i=0; i< iNrCoords; ++i)
		crdBuf[i] = coords[i];

	currentSeg->PutCoords(iNrCoords,crdBuf);
	sm->Updated();
	Domain _dm = dm();
	if (_dm->pdid()) {
		sValue = "";
		//    long iRaw = _dm->iRaw(sValue);
		//    if (iRaw != iUNDEF)
		//      sValue = String("seg%i", iRaw);
		DomainUniqueID* duid = dm()->pdUniqueID();
		if (duid) {
			int iRaw = duid->iAdd();
			sValue = duid->sValueByRaw(iRaw);
		}
		else {
AskID:      
			AskValue(SEDRemEnterID, htpSegEditorAskValueNewSeg);
			if (_dm->iRaw(sValue) == iUNDEF) {
				DomainSort* pds = _dm->pdsrt();
				pds->iAdd(sValue);
			}  
			else 
			{
				ILWIS::Segment *seg = (ILWIS::Segment *)sm->getFeature(sValue);
				if (seg && seg->fValid()) 
				{
					MessageBeep(MB_ICONEXCLAMATION);
					int iRet = mpv->MessageBox(SEDMsgValInUse.sVal(), SEDMsgSegEditor.sVal(),
						MB_YESNO|MB_DEFBUTTON2|MB_ICONASTERISK);
					if (IDYES != iRet) 
						goto AskID;
				}          
			}
		}
	}
	else if (_dm->pdv()) {
		AskValue(SEDRemEnterValue, htpSegEditorAskValueNewSeg);
	}
	else {
		if (_dm->iRaw(sValue) == iUNDEF) {
			MessageBeep(MB_ICONEXCLAMATION);
			AskValue(SEDRemNoValueSpecified, htpSegEditorAskValueNewSeg);
			if (_dm->iRaw(sValue) == iUNDEF)
				sValue = _dm->sValueByRaw(1);
		}
	}
	currentSeg->PutVal(dvs, sValue);
	sm->Updated();

	// remove old drawing
	Color clr = SysColor(COLOR_WINDOW);
	CClientDC cdc(mpv);
	drawCoords(&cdc,clr);

	iNrCoords = 0;
	coords.clear();
	drawSegment(&cdc,currentSeg,false);
	if (_dm->pdid()) {
		int iRaw = currentSeg->iValue();
		EditAttrib(iRaw);
	}
	return AddSegments(Coord());
}

int SegmentEditor::SnapEndSegmentMouse(Coord c)
{
  Coord crdSnap = sm->crdNode(c);
  Coord cSnap = crdSnap;
  zPoint p = mpv->pntPos(c);
  zPoint pSnap = mpv->pntPos(cSnap);
  zPoint pTst = pSnap - p;
  unsigned long iSnapDist = (unsigned long)(sqr((double)pTst.x) + sqr((double)pTst.y));
  zPoint pStart = mpv->pntPos(coords[0]);
  pTst = pStart - p;
  unsigned long iOwnDist = (unsigned long)(sqr((double)pTst.x) + sqr((double)pTst.y));
	if (!crdSnap.fUndef())
	{
		if (iSnapDist < iOwnDist) {
			if ( iSnapDist < sqr(iSnapPixels)) {
				EndSegment(cSnap);
				Mode(modeADD);
				return 1;
			}
		}
		else {
			if (iOwnDist < sqr(iSnapPixels)) {
				MovePivot(coords[0]);
				EndSegment(coords[0]);
				Mode(modeADD);
				return 1;
			}
		}
	}
  MessageBeep(MB_ICONASTERISK);
  return 1;
}

int SegmentEditor::SnapSplitEndSegmentMouse(Coord c)
{
  currentSeg = sm->segFirst();
  long iAft = 0;
  Coord crdSnap = sm->crdPoint(c,NULL,iAft);
  Coord cSnap = crdSnap;
  zPoint p = mpv->pntPos(c);
  zPoint pSnap = mpv->pntPos(cSnap);
  zPoint pTst = pSnap - p;
  if (crdSnap.fUndef() || (sqr((double)pTst.x) + sqr((double)pTst.y) > sqr(iSnapPixels))) {
    MessageBeep(MB_ICONASTERISK);
    return 1;
  }
  if (0 == AskSplit(currentSeg, iAft, crdSnap)) {
    MovePivot(cSnap);
    EndSegment(cSnap);
    Mode(modeADD);
    return 1;
  }
  else
    return 1;
}

int SegmentEditor::AutoEndSegmentMouse(Coord c)
{
  Coord crdSnap = sm->crdNode(c);
  Coord cSnap =crdSnap;
  zPoint p = mpv->pntPos(c);
  zPoint pSnap = mpv->pntPos(cSnap);
  zPoint pTst = pSnap - p;
  unsigned long iSnapDist = (unsigned long)(sqr((double)pTst.x) + sqr((double)pTst.y));
  zPoint pStart = mpv->pntPos(coords[0]);
  pTst = pStart - p;
  unsigned long iOwnDist = (unsigned long)(sqr((double)pTst.x) + sqr((double)pTst.y));
	if (!crdSnap.fUndef())
	{
		if (iSnapDist < iOwnDist) {
			if (iSnapDist < sqr(iSnapPixels)) {
				if (iNrCoords > 2) {
					EndSegment(cSnap);
					Mode(modeADD);
				}  
				return 1;
			}
		}
		else {
			if (iOwnDist < sqr(iSnapPixels)) {
				if (iNrCoords > 2) {
					MovePivot(coords[0]);
					EndSegment(coords[0]);
					Mode(modeADD);
				}  
				return 1;
			}
		}
	}
  currentSeg = sm->segFirst();
  long iAft = 0;
  crdSnap = sm->crdPoint(c,&currentSeg,iAft);
  cSnap = crdSnap;
  p = mpv->pntPos(c);
  pSnap = mpv->pntPos(cSnap);
  pTst = pSnap - p;
  if (crdSnap.fUndef() || (sqr((double)pTst.x) + sqr((double)pTst.y) > sqr(iSnapPixels)))
    return 1;
  if (0 == AskSplit(currentSeg, iAft, crdSnap)) {
    MovePivot(cSnap);
    EndSegment(cSnap);
    Mode(modeADD);
    return 1;
  }
  else
    return 1;
}

int SegmentEditor::SnapEndSegment(Coord c)
{
	if (iLastButton == 2)
		return 1;
	SetActNode(Coord()); // to clear the previously set active node (at SnapSplit)
	Coord crdSnap = sm->crdNode(c);
	Coord cSnap =crdSnap;
	double rSnapDist = rDist(c, cSnap);
	double rOwnDist = rDist(c, coords[0]);
	bool fAcceptPossible = false;
	if (rSnapDist >= 0 && rSnapDist < rOwnDist) {
		if (rSnapDist < sm->rSnapDist) {
			MovePivot(cSnap);
			fAcceptPossible = true;
		}
	}
	else {
		if (rOwnDist < sm->rSnapDist) {
			MovePivot(coords[0]);
			fAcceptPossible = true;
		}
	}
	if (fAcceptPossible) {
		return SetDigiFunc(SEDDigEndSegment, (DigiFunc)&DigiEditor::MoveCursor,
			(DigiFunc)&SegmentEditor::SnapAccept, SEDDigAccept,
			(DigiFunc)&SegmentEditor::SnapEndSegment, SEDDigSnapOther,
			(DigiFunc)&SegmentEditor::SnapSplit, SEDDigSnapSplit,
			(DigiFunc)&SegmentEditor::StartEnteringPoints, SEDDigNoSnap);
	}
	else {
		MessageBeep(MB_ICONASTERISK);
		return SetDigiFunc(SEDDigEndSegment, 
			//(DigiFunc)&DigiEditor::MoveCursor,
			(DigiFunc)&SegmentEditor::MovePivot,
			NULL, "",
			(DigiFunc)&SegmentEditor::SnapEndSegment, SEDDigSnapOther,
			(DigiFunc)&SegmentEditor::SnapSplit, SEDDigSnapSplit,
			(DigiFunc)&SegmentEditor::StartEnteringPoints, SEDDigNoSnap);
	}
}

int SegmentEditor::SnapAccept(Coord)
{
	if (!crdActNode.fUndef())
	{
		// Solution taken from SnapSplitPointAccept
		ILWIS::Segment *seg = NULL;
		long iAft = 0;
		Coord crdSnap = sm->crdPoint(crdActNode,&seg,iAft);
		// crdSnap is identical to crdActNode coz in ::SnapSplit crdActNode is set to this
		// The only reason for doing the above line is to recalculate the iAft that is needed
		// below to split the segment.
		seg->segSplit(iAft, crdSnap);
		return EndSegment(crdSnap); // Not cLast; in case cLast != crdSnap; We mean crdSnap
	}
	return EndSegment(cLast);
}

int SegmentEditor::SnapSplit(Coord c)
{
	if (iLastButton == 3)
		return 1;
	currentSeg = sm->segFirst();
	long iAft = 0;
	Coord crdSnap = sm->crdPoint(c,NULL,iAft);
	Coord cSnap = crdSnap;
	double rD2 = rDist2(c, cSnap);
	if (rD2 == rUNDEF || rD2 > sqr(sm->rSnapDist)) { 
		MessageBeep(MB_ICONASTERISK);
		return 0;
	}
	// seg.segSplit(iAft,crdSnap);
	// this would actually split the segment when choosing a snap point; split moved to ::SnapAccept
	SetActNode(crdSnap);
	MovePivot(cSnap);
	return SetDigiFunc(SEDDigEndSegment, 
		(DigiFunc)&DigiEditor::MoveCursor,
		(DigiFunc)&SegmentEditor::SnapAccept, SEDDigAccept,
		(DigiFunc)&SegmentEditor::SnapEndSegment, SEDDigSnapOther,
		(DigiFunc)&SegmentEditor::SnapSplit, SEDDigSnapSplit,
		(DigiFunc)&SegmentEditor::StartEnteringPoints, SEDDigNoSnap);
}

int SegmentEditor::StartEnteringPoints(Coord c)
{
	SetActNode(Coord()); // to clear the previously set active node (at SnapSplit)
	iLastTick = GetTickCount();
	return SetDigiFunc(SEDDigDigSegment,
		(DigiFunc)&SegmentEditor::MovePivot,
		(DigiFunc)&SegmentEditor::EnterPoints, SEDDigEnterPoint,
		(DigiFunc)&SegmentEditor::SnapEndSegment, SEDDigSnapEndSegment,
		(DigiFunc)&SegmentEditor::EndSegment, SEDDigEndSegment,
		(DigiFunc)&SegmentEditor::DeleteLastPoint, SEDDigDeleteLastPoint);
}

int SegmentEditor::DeleteLastPoint(Coord crd)
{
	long iTick = GetTickCount();
	if (iTick < iLastTick + 100)
		return 1;
	iLastTick = iTick;
	MovePivot(cPivot);
	iNrCoords -= 1;
	if (iNrCoords == 0) {
		coords.clear();
		iLastButton = 0;
		return AddSegments(crd);
	}
	if (iNrCoords < 0) {  // impossible, but just to be sure
		iNrCoords = 0;
		return 1;
	}
	cPivot = coords[iNrCoords-1];
	zPoint pPivot = mpv->pntPos(cPivot);
	zPoint pLast = mpv->pntPos(cLast);
	CClientDC cdc(mpv);
	Color c = SysColor(COLOR_WINDOW);
	CPen pen(PS_SOLID,1,c);
	CPen* penOld = cdc.SelectObject(&pen);
	cdc.MoveTo(pPivot);
	cdc.LineTo(pLast);
	cdc.SelectObject(penOld);
	cLast = cPivot;
	return MovePivot(crd);
}

int SegmentEditor::TunnelSegment()
{
  // remove old drawing
  Color c = SysColor(COLOR_WINDOW);
	CClientDC cdc(mpv);
  drawCoords(&cdc,c);

  // tunneling
  double rT2 = sqr(sm->rTunnelWidth);
  while (iNrCoords > 2) {
    int iLst = iNrCoords - 1;
    iNrCoords = 1;
	int b = 0;
    for (; b + 2 <=iLst;) {
      if (fInTunnel(rT2, coords[b], coords[b+1], coords[b+2]))
				b += 2;
      else
				b += 1;
      coords[iNrCoords++] = coords[b];
    }
    if (b != iLst)
      coords[iNrCoords++] = coords[iLst];
    if (iLst == iNrCoords - 1)
      break;
  }

  drawCoords(&cdc,col);
  return 0;
}

int SegmentEditor::MovePoints(Coord crd)
{
  if (mode != modeMOVE) {
    if (iLastButton == 4)
      return 1;
    else  
      Mode(modeMOVE);
  }
  SetActNode(Coord()); // to clear the previously set active node (at SnapSplit)
//  else {
    fDigBusy = false;
	/*if ( coords.size() > 0 ) {
		MoveCursorPoint(Coord());
		coords.clear();
		iNrCoords = 0;
	}*/
    dc = dcBOX;
    MoveCursor(crd);
    if (!fRetouching) {
      fRetouching = true;
      mpv->Invalidate();
    }
    ChangeWindowBasis = (DigiFunc)&SegmentEditor::MovePoints;
    return SetDigiFunc(SEDDigMovePoints, (DigiFunc)&DigiEditor::MoveCursor,
		     NULL, "",
		     (DigiFunc)&SegmentEditor::TakePoint, SEDDigTakePoint,
		     (DigiFunc)&DigiEditor::ChangeWindow, SEDDigChangeWindow,
		     (DigiFunc)&SegmentEditor::AddSegments, SEDDigReturn);
//  }		     
}

int SegmentEditor::TakePointMouse(Coord crd)
{
  ILWIS::Segment *seg = NULL;
  long iAft;
  Coord crdSnap = sm->crdPoint(crd,NULL,iAft);
  Coord cSnap = crdSnap;
  zPoint p = mpv->pntPos(crd);
  zPoint pSnap = mpv->pntPos(cSnap);
  zPoint pTst = pSnap - p;
  if (crdSnap.fUndef() || (sqr((double)pTst.x) + sqr((double)pTst.y) > sqr(iSnapPixels))) {
    MessageBeep(MB_ICONASTERISK);
    return 0;
  }
  return TakePoint(cSnap);
}

int SegmentEditor::TakePoint(Coord crd)
{
	if (iLastButton == 2)
		return 1;
	fDigBusy = true;
	if (coords.size() > 0) {
		MoveCursorPoint(Coord());
		CClientDC cdc(mpv);
		CPen pen(PS_SOLID,1,col);
		CPen* penOld = cdc.SelectObject(&pen);
		drawSegment(&cdc,currentSeg,false);
		cdc.SelectObject(penOld);
	}
	Coord crd2 = crd;
	crd2 = sm->crdCoord(crd, &currentSeg, iActCrd);
	Coord cSnap = crd2;
	if (rDist2(crd, cSnap) > sqr(sm->rSnapDist)) { 
		MessageBeep(MB_ICONASTERISK);
		if (coords.size() > 0) {
			coords.clear();
			iNrCoords = 0;
			return MovePoints(crd);
		}
		return 0;
	}
	CoordinateSequence *buf = currentSeg->getCoordinates();
	iNrCoords = buf->size();
	coords.clear();
	for (int i = 0; i < iNrCoords; ++i)
		coords.push_back(buf->getAt(i));
	MoveCursorPoint(Coord());
	CClientDC cdc(mpv);
	CPen pen(PS_SOLID,1,colRetouch);
	CPen* penOld = cdc.SelectObject(&pen);
	MinMax mm;
	drawSegment(&cdc,currentSeg,true);
	cdc.SelectObject(penOld);
	cLast = Coord();
	cPivot = cSnap; // to allow mouse button up very close to here
	return SetDigiFunc(SEDDigRetouchePoint, (DigiFunc)&SegmentEditor::MoveCursorPoint,
		(DigiFunc)&SegmentEditor::NewPosPoint, SEDDigNewPosition,
		(DigiFunc)&SegmentEditor::TakePoint, SEDDigTakePoint,
		(DigiFunc)&SegmentEditor::SnapPoint, SEDDigSnap,
		(DigiFunc)&SegmentEditor::MovePoints, SEDDigReturn);
	delete buf;
}

int SegmentEditor::MoveCursorPoint(Coord crd)
{
	zPoint pCurr = mpv->pntPos(coords[iActCrd]);
	bool fNew = !crd.fUndef();
	if (fNew)
		coords[iActCrd] = crd;
	zPoint pNew = mpv->pntPos(crd);
	zPoint pBef, pAft;
	if (pCurr != pNew) {
		CClientDC cdc(mpv);
		cdc.SetROP2(R2_NOT);
		if (iActCrd != 0) {
			pBef = mpv->pntPos(coords[iActCrd - 1]);
			cdc.MoveTo(pBef);
			cdc.LineTo(pCurr);
			if (fNew) {
				cdc.MoveTo(pBef);
				cdc.LineTo(pNew);
			}
		}
		if (iActCrd != iNrCoords - 1) {
			pAft = mpv->pntPos(coords[iActCrd + 1]);
			cdc.MoveTo(pCurr);
			cdc.LineTo(pAft);
			if (fNew) {
				cdc.MoveTo(pNew);
				cdc.LineTo(pAft);
			}
		}
		cdc.SetROP2(R2_COPYPEN);
	}
	MoveCursor(crd);
	return 0;
}

int SegmentEditor::NewPosPointMouse(Coord crd)
{
  if (fAutoSnap) {
    Coord crdSnap = sm->crdNode(crd);
    Coord cSnap = crdSnap;
    zPoint p = mpv->pntPos(crd);
    zPoint pSnap = mpv->pntPos(cSnap);
    zPoint pTst = pSnap - p;
    if (!crdSnap.fUndef() && (sqr((double)pTst.x) + sqr((double)pTst.y) <= sqr(iSnapPixels))) {
      return NewPosPoint(cSnap);
    }
  }
  return NewPosPoint(crd);
}

int SegmentEditor::NewPosPoint(Coord crd)
{
  MoveCursorPoint(crd);
  removeDigCursor();
	CClientDC cdc(mpv);
  Color c = SysColor(COLOR_WINDOW);
	CPen pen(PS_SOLID,1,c);
	CPen* penOld = cdc.SelectObject(&pen);
  drawSegment(&cdc,currentSeg,true);
  cdc.SelectObject(penOld);
  coords[iActCrd] = crd;
  CoordBuf buf(iNrCoords);
  for (int i = 0; i < iNrCoords; ++i)
    buf[i] = coords[i];
  currentSeg->PutCoords(iNrCoords,buf);
  sm->Updated();
	CPen penCol(PS_SOLID,1,col);
	penOld = cdc.SelectObject(&penCol);

  drawSegment(&cdc,currentSeg,false);
	cdc.SelectObject(penOld);
  return MovePoints(Coord());
}

int SegmentEditor::SnapPointMouse(Coord crd)
{
  Coord crdSnap = sm->crdNode(crd);
  Coord cSnap = crdSnap;
  zPoint p = mpv->pntPos(crd);
  zPoint pSnap = mpv->pntPos(cSnap);
  zPoint pTst = pSnap - p;
  if (crdSnap.fUndef() || (sqr((double)pTst.x) + sqr((double)pTst.y) > sqr(iSnapPixels))) {
    MessageBeep(MB_ICONASTERISK);
    ILWIS::Segment *seg = NULL;
    long iAft;
    crdSnap = sm->crdPoint(crd,NULL,iAft);
    cSnap = crdSnap;
    pSnap = mpv->pntPos(cSnap);
    pTst = pSnap - p;
    if (crdSnap.fUndef() || (sqr((double)pTst.x) + sqr((double)pTst.y) > sqr(iSnapPixels)))
      return 0;
    if (0 != AskSplit(seg, iAft, crdSnap))
      return 0;
  }
  return SnapPoint(cSnap);
}

int SegmentEditor::SnapPoint(Coord crd)
{
  Coord crdSnap = sm->crdNode(crd);
  Coord cSnap = crdSnap;
  if (rDist2(crd, cSnap) > sqr(sm->rSnapDist)) {
    MessageBeep(MB_ICONASTERISK);
    return SnapSplitPoint(crd);
  }
  if (iActCrd == 0 || iActCrd == iNrCoords - 1)
    return NewPosPoint(cSnap);
  NewPosPoint(cSnap);
  currentSeg->segSplit(iActCrd, crdSnap);
  return MovePoints(Coord());
}

int SegmentEditor::SnapSplitPoint(Coord crd)
{
  ILWIS::Segment *seg = NULL;
  long iAft;
  Coord crdSnap = sm->crdPoint(crd,NULL,iAft);
  Coord cSnap = crdSnap;
  if (rDist2(crd, cSnap) > sqr(sm->rSnapDist)) {
    MessageBeep(MB_ICONASTERISK);
    return MoveCursorPoint(crd);
  }
  SetActNode(crdSnap);
  return SetDigiFunc(SEDDigRetouchePoint, (DigiFunc)&SegmentEditor::MoveCursorPoint,
		     (DigiFunc)&SegmentEditor::SnapSplitPointAccept, SEDDigAccept,
		     (DigiFunc)&SegmentEditor::SnapPoint, SEDDigSnap,
		     (DigiFunc)&SegmentEditor::SnapSplitPoint, SEDDigSnapSplit,
		     (DigiFunc)&SegmentEditor::MovePoints, SEDDigNoSnap);
}

int SegmentEditor::SnapSplitPointAccept(Coord)
{
  ILWIS::Segment *seg = NULL;
  long iAft;
  Coord crdSnap = sm->crdPoint(crdActNode,NULL,iAft);
  Coord cSnap = crdSnap;
  seg->segSplit(iAft, crdSnap);
  return SnapPoint(cSnap);
}

int SegmentEditor::MergeSplit(Coord crd)
{
  int iRet = 1;
	curActive = curSegSplitting;
	OnSetCursor();

  Coord crd2 = crd;
  Coord crdNode = sm->crdNode(crd2);
  Coord cNode = crdNode;
  zPoint p = mpv->pntPos(crd);
  zPoint pFnd = mpv->pntPos(cNode);
  zPoint pTst = pFnd - p;
  if (!crdNode.fUndef() && (sqr((double)pTst.x) + sqr((double)pTst.y) <= sqr(iSnapPixels))) {
    AskMerge(crdNode);
  }
  else {
    long iAft;
	ILWIS::Segment *seg = sm->segFirst();
    crd2 = sm->crdPoint(crd, NULL, iAft);
    Coord cFnd = crd2;
    pFnd = mpv->pntPos(cFnd);
    pTst = pFnd - p;
    if (sqr((double)pTst.x) + sqr((double)pTst.y) > sqr(iSnapPixels))
      MessageBeep(MB_ICONASTERISK);
    else
      iRet = AskSplit(seg, iAft, crd2);
  }
	curActive = curSegSplit;
	OnSetCursor();
  return iRet;
}

void SegmentEditor::AskMerge(Coord crdNode)
{
  SetActNode(crdNode);
  ILWIS::Segment *seg1, *seg2;
  int iFound = 0;
  for (int i = 0; i < sm->iFeatures(); ++i) {
	  ILWIS::Segment *seg = (ILWIS::Segment *)sm->getFeature(i);
	  if ( !(seg && seg->fValid()))
		  continue;
	  bool fBegin = seg->crdBegin() == crdNode;
	  bool fEnd = seg->crdEnd() == crdNode;
	  if (fBegin && fEnd) {
		  MessageBeep(MB_ICONEXCLAMATION);
		  mpv->MessageBox(SEDMsgNodeInCircSegNoRem.sVal(), SEDMsgSegEditor.sVal(), MB_ICONEXCLAMATION|MB_OK);
		  SetActNode(Coord());
		  return;
	  }
	  if (fBegin || fEnd) {
		  switch (++iFound) {
		case 1:
			seg1 = seg;
			break;
		case 2:
			seg2 = seg;
			break;
		default:
			MessageBeep(MB_ICONEXCLAMATION);
			mpv->MessageBox(SEDMsgMoreThan2SegNoRem.sVal(), SEDMsgSegEditor.sVal(), MB_ICONEXCLAMATION|MB_OK);
			SetActNode(Coord());
			return;
		  }
	  }
  }
  switch (iFound) {
    case 0:
      MessageBeep(MB_ICONEXCLAMATION);
      // impossible !!
      SetActNode(Coord());
      return;
    case 1:
      MessageBeep(MB_ICONEXCLAMATION);
      mpv->MessageBox(SEDMsgDeadEndNoRem.sVal(), SEDMsgSegEditor.sVal(), MB_ICONEXCLAMATION|MB_OK);
      SetActNode(Coord());
      return;
  }

  String sVal1, sVal2;
  CoordinateSequence *buf1, *buf2;
  long iNr1, iNr2;
  buf1 = seg1->getCoordinates();
  iNr1 = buf1->size();
  buf2 = seg2->getCoordinates();
  iNr2 = buf2->size();
/*
  if (iNr1 + iNr2 > 1002) {
    MessageBeep(MB_ICONEXCLAMATION);
    mpv->MessageBox(SEDMsgTogetherTooLongNoMerge.sVal(), SEDMsgSegEditor.sVal(), MB_ICONEXCLAMATION|MB_OK);
    SetActNode(Coord());
    return;
  }
*/
  sVal1 = seg1->sValue(sm->dvrs());
  sVal2 = seg2->sValue(sm->dvrs());
  String sMsg(SEDMsgMergeSeg_SS.sVal(), sVal1, sVal2);
  int iRet = mpv->MessageBox(sMsg.sVal(), SEDMsgSegEditor.sVal(), MB_ICONQUESTION|MB_YESNO);
  if (IDYES == iRet) {
    int i, j;
		int iSize = iNr1 + iNr2;
    CoordBuf buf(iSize);
    if (buf1->getAt(0) == crdNode)
      for (j = 0, i = iNr1 - 1; i >= 0; --i, ++j)
        buf[j] = buf1->getAt(i);
    else
      for (j = 0, i = 0; i < iNr1; ++i, ++j)
        buf[j] = buf1->getAt(i);
    if (buf2->getAt(0) == crdNode)
      for (i = 1; i < iNr2; ++i, ++j)
        buf[j] = buf2->getAt(i);
    else
      for (i = iNr2 - 2; i >= 0; --i, ++j)
		  buf[j] = buf2->getAt(i);
    seg1->PutCoords(j, buf);
    seg2->Delete();
  }
  delete buf1;
  delete buf2;
  SetActNode(Coord());
}

int SegmentEditor::AskSplit(ILWIS::Segment *seg, long iAfter, Coord crdAt)
{
  int iRet = 1;
  SetActNode(crdAt);

  String sVal = seg->sValue(sm->dvrs());
  String sMsg(SEDMsgSplitSeg_S.sVal(), sVal);
  int iMsg = mpv->MessageBox(sMsg.sVal(), SEDMsgSegEditor.sVal(), MB_ICONQUESTION|MB_YESNO);
  if (IDYES == iMsg) {
	  ILWIS::Segment *s = CSEGMENT(sm->newFeature());
    seg->segSplit(iAfter, crdAt, &s);
    iRet = 0;
  }

  SetActNode(Coord());
  return iRet;
}

void SegmentEditor::SetActNode(Coord crd)
{
	if (!crdActNode.fUndef())
	{
		zPoint pnt = mpv->pntPos(crdActNode);
		zPoint p1(pnt.x-3, pnt.y-3);
		zPoint p2(pnt.x+4, pnt.y+4);
		zRect rect(p1, p2);
		mpv->InvalidateRect(&rect);
	}
  crdActNode = crd;
  if (!crdActNode.fUndef())
	{
		CClientDC cdc(mpv);
    drawActNode(&cdc);
  }
}

int SegmentEditor::SnapSplitSegment(Coord c)
{
  ILWIS::Segment *seg = NULL;
  long iAft;
  Coord crdSnap = sm->crdPoint(c,NULL,iAft);
  Coord cSnap = crdSnap;
  if (rDist2(c, cSnap) > sqr(sm->rSnapDist)) {
    MessageBeep(MB_ICONASTERISK);
    return 0;
  }
  SetActNode(crdSnap);
  return SetDigiFunc(SEDDigSplitSegment, (DigiFunc)&DigiEditor::MoveCursor,
        	     (DigiFunc)&SegmentEditor::SnapSplitSegmentAccept, SEDDigAccept,
		     (DigiFunc)&SegmentEditor::SnapSegment, SEDDigSnapSegment,
		     (DigiFunc)&SegmentEditor::SnapSplitSegment, SEDDigSnapSplit,
		     (DigiFunc)&SegmentEditor::AddSegments, SEDDigReturn);
}

int SegmentEditor::SnapSplitSegmentAccept(Coord c)
{
  ILWIS::Segment *seg = NULL;
  long iAft;
  Coord crdSnap = sm->crdPoint(crdActNode,NULL,iAft);
  Coord cSnap =crdSnap;
  seg->segSplit(iAft, crdSnap);
  return BeginSegment(cSnap);
}


  class SegConfigForm: public FormWithDest
  {
  public:
    SegConfigForm(CWnd* wPar, SegmentEditor* edit, int* iDrw, bool fDom)
    : FormWithDest(wPar, SEDTitleCnfSegEdit)
    {
      new FieldColor(root, SEDUiCursorColor, &edit->colDig);
      new CheckBox(root, SEDUiAutoSnap, &edit->fAutoSnap);
      new CheckBox(root, SEDUiShowNodes, &edit->fShowNodes);
      new FieldReal(root, SEDUiSnapTolerance, &edit->sm->rSnapDist);
      new FieldInt(root, SEDUiSnapPixels, &edit->iSnapPixels, ValueRange(2,30), true);
      new FieldReal(root, SEDUiTunnelTolerance, &edit->sm->rTunnelWidth);
      new FieldColor(root, SEDUiNormalColor, &edit->col);
      new FieldColor(root, SEDUiRetoucheColor, &edit->colRetouch);
      new FieldColor(root, SEDUiDeletedColor, &edit->colDeleted);
			new FieldColor(root, SEDUiFindUndefColor, &edit->colFindUndef);
      RadioGroup* rg = new RadioGroup(root, SEDUiSegmentColors, iDrw);
      new RadioButton(rg, SEDUiNormal);
      if (fDom)
        new RadioButton(rg, SEDUiSegDomain);
      new RadioButton(rg, SEDUiPrimCol);
//      SetHelpTopic(htpSegmentEditor);
      SetMenHelpTopic(htpSegEditCnf);
      create();
    }
  };
  

void SegmentEditor::OnConfigure()
{
  int iDrw = 0;
  bool fDom = sm->dm()->rpr().fValid();
  switch (drw) {
    case drwNORMAL:  iDrw = 0; break;
    case drwDOMAIN:  iDrw = 1; break;
    case drwPRIMARY: iDrw = 2; break;
  }
  if (!fDom && iDrw > 1)
    iDrw -= 1;
//  iDrw += 1;  
  SegConfigForm frm(mpv, this, &iDrw, fDom);
  if (frm.fOkClicked()) {
    if (!fDom && iDrw >= 1)
      iDrw += 1;
    switch (iDrw) {
      case 0: drw = drwNORMAL; break;
      case 1: drw = drwDOMAIN; break;
      case 2: drw = drwPRIMARY; break;
    }
		IlwisSettings settings("Map Window\\Segment Editor");
    String sVal;
    switch (drw) {
      case drwDOMAIN:
        sVal = "Domain";
        break;
      case drwPRIMARY:
        sVal = "Primary";
        break;
      case drwNORMAL:
        sVal = "Normal";
        break;
    }
		settings.SetValue("Segment Colors", sVal);
		settings.SetValue("Cursor Color", colDig);
		settings.SetValue("Auto Snap", fAutoSnap);
		settings.SetValue("Show Nodes", fShowNodes);
		settings.SetValue("Snap Pixels", iSnapPixels);
		settings.SetValue("Normal Color", col);
		settings.SetValue("Retouch Color", colRetouch);
		settings.SetValue("Deleted Color", colDeleted);
		settings.SetValue("Find Undef Color", colFindUndef);
		mpv->Invalidate();
  }  
}


class CreatePolForm: public FormWithDest
{
public:
  CreatePolForm(CWnd* wnd, const String& sDfltMap, const String& sMsk)
  : FormWithDest(wnd, SAFTitlePolMapFromSeg),
    sOutMap(sDfltMap), sMask(sMsk)
  {									
		iImg = IlwWinApp()->iImage("ExePol16Ico");
    fMask = sMask.length() > 0;
		fTopology = true;
		iOption = 0;
    CheckBox* cbMask = new CheckBox(root, SAFUiMask, &fMask);
    new FieldString(cbMask, "", &sMask);

		cbTopology = new CheckBox(root, SAFUiTopology, &fTopology);
		cbTopology->Align(cbMask, AL_UNDER);
		cbTopology->SetCallBack((NotifyProc)&CreatePolForm::CallBack);

		fgTop = new FieldGroup(root);
		fgTop->Align(cbTopology, AL_UNDER);

    RadioGroup* rg = new RadioGroup(fgTop, SAFUiPolCodes, &iOption);
		rg->SetCallBack((NotifyProc)&CreatePolForm::CallBackOption);
    RadioButton* rbDom = new RadioButton(rg, SAFUiDomain);
    new FieldDomainC(rbDom, "", &sDomain, dmCLASS|dmIDENT|dmVALUE|dmBOOL);
    RadioButton* rbLbl = new RadioButton(rg, SAFUiLabelPnts);
    new FieldPointMap(rbLbl, "", &sPointMap);
    new RadioButton(rg, SAFUiPolAutoIdent);

		fgNonTop = new FieldGroup(root);
		fgNonTop->Align(cbTopology, AL_UNDER);

    rg = new RadioGroup(fgNonTop, SAFUiPolCodes, &iOption);
		rg->SetCallBack((NotifyProc)&CreatePolForm::CallBackOption);
	  rbDom = new RadioButton(rg, SAFUiDomain);
    new FieldDomainC(rbDom, "", &sDomain, dmCLASS|dmIDENT|dmVALUE|dmBOOL);
		new RadioButton(rg, SAFUiUseSegCode);
    rbLbl = new RadioButton(rg, SAFUiLabelPnts);
    new FieldPointMap(rbLbl, "", &sPointMap);
    new RadioButton(rg, SAFUiPolAutoIdent);

    fmc = new FieldPolygonMapCreate(root, SAFUiOutPolMap, &sOutMap);
    fmc->SetCallBack((NotifyProc)&CreatePolForm::CallBackName);

    fStartEdit = true;
    fShow = true;
    cbEdit = new CheckBox(root, SAFUiEdit, &fStartEdit);
    cbEdit->Align(fmc, AL_AFTER);
    cbShow = new CheckBox(root, SAFUiShow, &fShow);
    cbShow->Align(fmc, AL_AFTER);

    StaticText* st = new StaticText(root, SAFUiDescription);
    st->Align(fmc, AL_UNDER);
    st->psn->SetBound(0,0,0,0);
    FieldString* fs = new FieldString(root, "", &sDescr);
    fs->SetWidth(120);
    fs->SetIndependentPos();

    String sFill('*', 40);
    stRemark = new StaticText(root, sFill);
    stRemark->SetIndependentPos();
    SetMenHelpTopic(htpSegEditPolMapFromSeg);
		HideOnOk(true);
    create();
  }
  int CallBackName(Event*)
  {
    fmc->StoreData();
    FileName fn(sOutMap);
    fn.sExt = ".mpa";
    bool fOk = false;
    if (!fn.fValid())
      stRemark->SetVal(SAFRemNotValidMapName);
    else if(File::fExist(fn))
      stRemark->SetVal(SAFRemMapExists);
    else {
      fOk = true;
      stRemark->SetVal("");
    }
    if (fOk)
      EnableOK();
    else
      DisableOK();
    return 0;
  }
	int CallBack(Event*) 
	{
		cbTopology->StoreData();
		if (fTopology) {
			fgTop->Show();
			fgNonTop->Hide();
		}
		else {
			fgTop->Hide();
			fgNonTop->Show();
		}
		return 0;
	}
	int CallBackOption(Event*)
	{
		if (fTopology)
			fgTop->StoreData();
		else
			fgNonTop->StoreData();
		if (0 == iOption) {
			cbEdit->Show();
			cbShow->Hide();
		}
		else {
			cbEdit->Hide();
			cbShow->Show();
		}
		return 0;
	}
  int exec() {
    FormWithDest::exec();
    if (!fMask)
      sMask = "";
    return 0;
  }
  int iOption;
  String sOutMap, sDomain, sPointMap, sDescr, sMask;
  bool fStartEdit, fShow, fMask, fTopology;
private:
  FieldPolygonMapCreate* fmc;
  StaticText* stRemark;
	CheckBox *cbTopology, *cbShow, *cbEdit;
	FieldGroup *fgTop, *fgNonTop;
};

void SegmentEditor::OnPolygonize()
{
  String sMap = sm->sName(false);
  CreatePolForm frm(mpv, sMap, sMask);
  if (!frm.fOkClicked())
    return;
  sMask = frm.sMask;
  FileName fn(frm.sOutMap);
  fn.sExt = ".mpa";
  if (fn.fExist()) {
    String sErr(SAFMsgAlreadyExistsOverwrite_S.sVal(), fn.sFullPath(true));
    int iRet = mpv->MessageBox(sErr.sVal(), SAFMsgAlreadyExists.sVal(), MB_YESNO|MB_ICONEXCLAMATION);
    if (IDYES != iRet)
      return;
  }

//  YieldActive ya;
  Tranquilizer trq(SEDMsgCheckSegments);

	if (frm.fTopology) {
	// Check Individual segments
		if (!fCheckSelf(trq))
			return;
	// Check that all nodes are connected and not overlaying
		if (!fCheckConnected(trq))
			return;
	// Check if no hidden intersections and no overlaying
		if (!fCheckIntersects(trq))
			return;
	}
	else {
		if (!fCheckClosedSegments(trq))
			return;
	}

  if (sm->fChanged) {
    sm->Updated();
    sm->Store();
  }

// Create Polygon Map
  trq.Stop();
	bool fBreakDep = false;
  try {
    String sExpr;
		if (frm.fTopology) {
			switch(frm.iOption) {
				case 0:
					sExpr = String("PolygonMapFromSegment(%S,\"%S\",%S)",
												 sm->sNameQuoted(), sMask, frm.sDomain);
					fBreakDep = true;
					break;
				case 1:
					sExpr = String("PolygonMapFromSegment(%S,\"%S\",%S)",
												 sm->sNameQuoted(), sMask, frm.sPointMap);
					break;
				case 2:
					sExpr = String("PolygonMapFromSegment(%S,\"%S\")",
												 sm->sNameQuoted(), sMask);
					break;
			}
		}
		else {
			switch(frm.iOption) {
				case 0:
					sExpr = String("PolygonMapFromSegmentNonTopo(%S,\"%S\",%S)",
												 sm->sNameQuoted(), sMask, frm.sDomain);
					fBreakDep = true;
					break;
				case 1:
					sExpr = String("PolygonMapFromSegmentNonTopo(%S,\"%S\",\"segments\")",
												 sm->sNameQuoted(), sMask);
					break;
				case 2:
					sExpr = String("PolygonMapFromSegmentNonTopo(%S,\"%S\",%S)",
												 sm->sNameQuoted(), sMask, frm.sPointMap);
					break;
				case 3:
					sExpr = String("PolygonMapFromSegmentNonTopo(%S,\"%S\",\"unique\")",
												 sm->sNameQuoted(), sMask);
					break;
			}
		}
    {
      PolygonMap mpOut(fn, sExpr);
      if (frm.sDescr[0])
        mpOut->sDescription = frm.sDescr;
      mpOut->Store(); // destructor of mpOut could be called later than opening map
      PolygonMapFromSegment* pmfs = dynamic_cast<PolygonMapFromSegment*>(mpOut->pmv);
			if (pmfs) 
				pmfs->fSegmentsChecked = true;
      mpOut->Calc();
      if (!mpOut->fCalculated())
        return;
      if (fBreakDep)
        mpOut->BreakDependency();
    }
    if (!fBreakDep) {
      if (frm.fShow) {												
				String sFile = fn.sFullPathQuoted(true);
				IlwWinApp()->OpenDocumentFile(sFile.sVal());
      }
    }
    else {
      if (frm.fStartEdit) {
				// remove SegmentDrawer before editor Polygon Map
				if (Editor::drw) {
					MapCompositionDoc* mcd = mpv->GetDocument();
					mcd->RemoveDrawer(Editor::drw);
					Editor::drw = 0;
				}
				mpv->SetFocus();
        mpv->EditNamedLayer(fn);  // gets "confused" when mpOut still open
      // nothing after this statement, "this" is now deleted
      }
    }
  }
  catch (ErrorObject& err) {
    err.Show("Polygonize");
  }
}

bool SegmentEditor::fCheckSelf(Tranquilizer& trq, long iStartSeg) // returns true if ok
{
	SetActNode(Coord());
	trq.SetText(SEDMsgSelfCheck);
	Mask mask(sm->dm(), sMask);
	bool fMask = sMask.length() > 0;
	if (trq.fUpdate(iStartSeg, sm->iFeatures()))
		return false;
	for (int j = iStartSeg; j < sm->iFeatures(); ++j) {
		ILWIS::Segment *seg = (ILWIS::Segment *)sm->getFeature(j);
		if ( !(seg && seg->fValid()))
			continue;
		if (trq.fUpdate(j-1, sm->iFeatures()))
			return false;
		if (fMask && !seg->fInMask(sm->dvrs(), mask))
			continue;
		long iFirst, iSecond;
		Coord crdAt;
		while (!seg->fSelfCheck(iFirst, iSecond, crdAt)) {
			trq.Stop();
			SetActNode(crdAt);
			MessageBeep(MB_ICONEXCLAMATION);
			String sVal = String("%S (nr %i)", seg->sValue(sm->dvrs()), j);
			if (-1 == iFirst) { // self overlay
				String sMsg(SEDMsgSegSelfOverlay_S.sVal(), sVal);
				String str("%S\n%S", sMsg, SEDMsgZoomInOnError);
				int iRet = mpv->MessageBox(str.sVal(), SEDMsgCheckSegments.sVal(), MB_ICONEXCLAMATION|MB_YESNO);
				if (IDYES == iRet)
					ZoomInOnError();
				Mode(modeMOVE);
				SetActNode(crdAt);
				return false;
			}
			else { // self cross
				int iRet = mpv->MessageBox(SEDMsgZoomInOnError.sVal(), SEDMsgCheckSegments.sVal(), MB_ICONEXCLAMATION|MB_YESNO);
				if (IDYES == iRet)
					ZoomInOnError();
				String sMsg(SEDMsgSegSelfCrossRemFalsePol_S.sVal(), sVal);
				iRet = mpv->MessageBox(sMsg.sVal(), SEDMsgCheckSegments.sVal(), MB_ICONEXCLAMATION|MB_YESNOCANCEL);
				if (IDYES == iRet) {
					seg->segSplit(iSecond, crdAt);
					ILWIS::Segment *s = CSEGMENT(sm->newFeature());
					seg->segSplit(iFirst, crdAt,&s);
					if (s->rLength() > seg->rLength()) {
						ILWIS::Segment *sTmp = s;
						s = seg;
						seg = sTmp;
					}
					SetDirty(s);
					s->Delete();
					trq.Start();
					continue;
				}
				else if (IDNO == iRet) {
					if (0 == AskSplit(seg, iSecond, crdAt)) {
						seg->segSplit(iFirst, crdAt);
						trq.Start();
						continue;
					}
				}
				Mode(modeMOVE);
				SetActNode(crdAt);
				return false;
			}
		}
	}
	if (trq.fUpdate(sm->iFeatures(), sm->iFeatures()))
		return false;
	SetActNode(Coord());
	trq.Stop();
	return true;
}

bool SegmentEditor::fCheckConnected(Tranquilizer& trq, long iStartSeg) // returns true if ok
{
	SetActNode(Coord());
	trq.SetText(SEDMsgConnectCheck);
	Mask mask(sm->dm(), sMask);
	bool fMask = sMask.length() > 0;
	if (trq.fUpdate(iStartSeg, sm->iFeatures()))
		return false;

	int iSeg = sm->iFeatures();
	//  ArrayLarge<Coord> crdF(iSeg,1), crdL(iSeg,1);
	ArrayLarge<Coord> crdF(iSeg+1), crdL(iSeg+1); // zero based
	ILWIS::Segment *s;
	for (int j = 0; j < sm->iFeatures(); ++j) {
		s = (ILWIS::Segment *)sm->getFeature(j);
		if ( !(s && s->fValid()))
			continue;
		if (fMask && !s->fInMask(sm->dvrs(), mask))
			continue;
		int i = j;
		crdF[i] = s->crdBegin();
		crdL[i] = s->crdEnd();
	}

	for (int j = iStartSeg; j < sm->iFeatures(); ++j) {
		ILWIS::Segment *seg = (ILWIS::Segment *)sm->getFeature(j);
		if ( !(seg && seg->fValid()))
			continue;
		if (trq.fUpdate(j, sm->iFeatures()))
			return false;
		if (fMask && !seg->fInMask(sm->dvrs(), mask))
			continue;
		bool fBegin = false;
		bool fEnd = false;
		Coord crdBegin = seg->crdBegin();
		Coord crdEnd = seg->crdEnd();
		double rAzimBegin = seg->rAzim(false);
		double rAzimEnd = seg->rAzim(true);
		if (crdBegin == crdEnd) {
			fBegin = fEnd = true;
			if (abs(rAzimBegin - rAzimEnd) < 1e-6) {
				trq.Stop();
				SetActNode(crdBegin);
				MessageBeep(MB_ICONEXCLAMATION);
				String sVal = String("%S (nr %i)", seg->sValue(sm->dvrs()), j);
				String sMsg(SEDMsgSegSelfOverlay_S.sVal(), sVal);
				String str("%S\n%S", sMsg, SEDMsgZoomInOnError);
				int iRet = mpv->MessageBox(str.sVal(), SEDMsgCheckSegments.sVal(), MB_ICONEXCLAMATION|MB_YESNO);
				if (IDYES == iRet)
					ZoomInOnError();
				Mode(modeMOVE);
				SetActNode(crdBegin);
				return false;
			}
		}
		for (int i = 0; i < iSeg; ++i) {
			if (trq.fAborted())
				return false;
			if (i == j)
				continue;
			bool fBeginError = false;
			bool fEndError = false;
			if (crdF[i] == crdBegin) {
				fBegin = true;
				s = (ILWIS::Segment *)sm->getFeature(i);
				if (abs(s->rAzim(false) - rAzimBegin) < 1e-6) {
					fBeginError = true;
					SetActNode(crdBegin);
				}
			}
			if (crdL[i] == crdBegin) {
				fBegin = true;
				s = (ILWIS::Segment *)sm->getFeature(i);
				if (abs(s->rAzim(true) - rAzimBegin) < 1e-6) {
					fBeginError = true;
					SetActNode(crdBegin);
				}
			}
			if (!fBeginError && !fEndError && crdBegin == crdEnd)
				continue;
			if (crdF[i] == crdEnd) {
				fEnd = true;
				s = (ILWIS::Segment *)sm->getFeature(i);
				if (abs(s->rAzim(false) - rAzimEnd) < 1e-6) {
					fEndError = true;
					SetActNode(crdEnd);
				}
			}
			if (crdL[i] == crdEnd) {
				fEnd = true;
				s = (ILWIS::Segment *)sm->getFeature(i);
				if (abs(s->rAzim(true) - rAzimEnd) < 1e-6) {
					fEndError = true;
					SetActNode(crdEnd);
				}
			}
			if (fBeginError || fEndError) {
				trq.Stop();
				MessageBeep(MB_ICONEXCLAMATION);
				String sVal = String("%S (nr %i)", seg->sValue(sm->dvrs()), i);
				String sMsg(SEDMsgSegOverlayAtNode_S.sVal(), sVal);
				String str("%S\n%S", sMsg, SEDMsgZoomInOnError);
				int iRet = mpv->MessageBox(str.sVal(), SEDMsgCheckSegments.sVal(), MB_ICONEXCLAMATION|MB_YESNO);
				if (IDYES == iRet)
					ZoomInOnError();
				Mode(modeMOVE);
				if (fBeginError)
					SetActNode(crdBegin);
				else
					SetActNode(crdEnd);
				return false;
			}
		}
		if (!fBegin || !fEnd) {
			if (!fBegin)
				SetActNode(crdBegin);
			else 
				SetActNode(crdEnd);
			trq.Stop();
			MessageBeep(MB_ICONEXCLAMATION);
			String sVal = String("%S (nr %i)", seg->sValue(sm->dvrs()), j);
			String sMsg(SEDMsgSegDeadEnd_S.sVal(), sVal);
			String str("%S\n%S", sMsg, SEDMsgZoomInOnError);
			int iRet = mpv->MessageBox(str.sVal(), SEDMsgCheckSegments.sVal(), MB_ICONEXCLAMATION|MB_YESNO);
			if (IDYES == iRet)
				ZoomInOnError();
			Mode(modeMOVE);
			if (!fBegin)
				SetActNode(crdBegin);
			else
				SetActNode(crdEnd);
			return false;
		}
	}
	if (trq.fUpdate(sm->iFeatures(), sm->iFeatures()))
		return false;
	SetActNode(Coord());
	trq.Stop();
	return true;
}

bool SegmentEditor::fCheckIntersects(Tranquilizer& trq, long iStartSeg) // returns true if ok
{
	SetActNode(Coord());
	trq.SetText(SEDMsgIntersectCheck);
	Mask mask(sm->dm(), sMask);
	bool fMask = sMask.length() > 0;
	if (trq.fUpdate(iStartSeg, sm->iFeatures()))
		return false;

	bool fCheckNeeded = true;
	while (fCheckNeeded) {
		fCheckNeeded = false;
		int iSeg = sm->iFeatures();
		//  ArrayLarge<CoordBounds> acb(iSeg,1);
		ArrayLarge<CoordBounds> acb(iSeg+1); // zero based
		ILWIS::Segment *s;
		for (int j = 0; j < sm->iFeatures(); ++j) {
			s = (ILWIS::Segment *)sm->getFeature(j);
			if ( !( s && s->fValid()))
				continue;
			if (fMask && !s->fInMask(sm->dvrs(), mask))
				continue;
			int i = j;
			acb[i] = s->cbBounds();
		}

		for (int j = iStartSeg; j < sm->iFeatures(); ++j) {
			ILWIS::Segment *seg = (ILWIS::Segment *)sm->getFeature(j);
			if ( !(seg && seg->fValid()))
				continue;
			if (trq.fUpdate(j, sm->iFeatures()))
				return false;
			if (fMask && !seg->fInMask(sm->dvrs(), mask))
				continue;
			int iCurr = j;
			CoordBounds cbCurr = acb[iCurr];
			for (int i = 0; i <= iSeg; ++i) {
				if (trq.fAborted())
					return false;
				if (i == iCurr)
					continue;
				if (!cbCurr.fContains(acb[i]))
					continue;
				bool fOverlay;
				long iAft, iAft2;
				Coord crdAt;
				ILWIS::Segment *s2 = (ILWIS::Segment *)sm->getFeature(i);
				if (seg->fIntersects(fOverlay, iAft, crdAt, s2, iAft2, trq)) {
					trq.Stop();
					SetActNode(crdAt);
					MessageBeep(MB_ICONEXCLAMATION);
					String sVal = String("%S (nr %i)", seg->sValue(sm->dvrs()), i);
					if (fOverlay) {
						String sMsg(SEDMsgSegOverlayingSeg_S.sVal(), sVal);
						String str("%S\n%S", sMsg, SEDMsgZoomInOnError);
						int iRet = mpv->MessageBox(str.sVal(), SEDMsgCheckSegments.sVal(), MB_ICONEXCLAMATION|MB_YESNO);
						if (IDYES == iRet)
							ZoomInOnError();
						Mode(modeMOVE);
						SetActNode(crdAt);
						return false;
					}
					else {
						int iRet = mpv->MessageBox(SEDMsgZoomInOnError.sVal(), SEDMsgCheckSegments.sVal(), MB_ICONEXCLAMATION|MB_YESNO);
						if (IDYES == iRet)
							ZoomInOnError();
						String sMsg(SEDMsgSegIntersectWithoutNodeSplitSeg_S.sVal(), sVal);
						iRet = mpv->MessageBox(sMsg.sVal(), SEDMsgCheckSegments.sVal(), MB_ICONEXCLAMATION|MB_YESNO);
						if (IDYES == iRet) {
							seg->segSplit(iAft, crdAt);
							s2->segSplit(iAft2, crdAt);
							SetActNode(Coord());
							trq.Start();
							fCheckNeeded = true;
						}
						else {
							Mode(modeMOVE);
							SetActNode(crdAt);
							return false;
						}
					}
				}
			}
		}
	}
	if (trq.fUpdate(sm->iFeatures(), sm->iFeatures()))
		return false;
	SetActNode(Coord());
	trq.Stop();
	return true;
}

bool SegmentEditor::fCheckCodeConsistency(Tranquilizer& trq, long iStartSeg)
{
	SetActNode(Coord());
	trq.SetText(SEDMsgCheckCodeConsistency);
	Mask mask(sm->dm(), sMask);
	bool fMask = sMask.length() > 0;
	if (trq.fUpdate(iStartSeg, sm->iFeatures()))
		return false;
	for (int j = iStartSeg; j < sm->iFeatures(); ++j) {
		ILWIS::Segment *seg = (ILWIS::Segment *)sm->getFeature(j);
		if ( !(seg && seg->fValid()))
			continue;
		if (trq.fUpdate(j, sm->iFeatures()))
			return false;
		if (fMask && !seg->fInMask(sm->dvrs(), mask))
			continue;
		Coord crdBegin = seg->crdBegin();
		Coord crdEnd = seg->crdEnd();
		String sVal = seg->sValue(sm->dvrs());
		ILWIS::Segment *s;
		for (int i = 0; i < sm->iFeatures(); ++i) {
			s = (ILWIS::Segment *)sm->getFeature(i);
			if ( !(s && s->fValid()))
				continue;
			if (trq.fAborted())
				return false;
			if (i == j)
				continue;
			if (fMask && !s->fInMask(sm->dvrs(), mask))
				continue;
			bool fBegin = s->crdBegin() == crdBegin || s->crdEnd() == crdBegin;
			bool fEnd = s->crdBegin() == crdEnd || s->crdEnd() == crdEnd;
			if (fBegin || fEnd) {
				if (s->sValue(sm->dvrs()) != sVal) {
					trq.Stop();
					if (fBegin)
						SetActNode(crdBegin);
					else
						SetActNode(crdEnd);
					MessageBeep(MB_ICONEXCLAMATION);
					String sMsg(SEDMsgDifferentCodes_SS.sVal(), sVal, s->sValue(sm->dvrs()));
					String str("%S\n%S", sMsg, SEDMsgZoomInOnError);
					int iRet = mpv->MessageBox(str.sVal(), SEDMsgCheckSegments.sVal(), MB_ICONEXCLAMATION|MB_YESNO);
					if (IDYES == iRet)
						ZoomInOnError();
					Mode(modeSELECT);
					if (fBegin)
						SetActNode(crdBegin);
					else
						SetActNode(crdEnd);
					return false;
				}
			}
		}
	}
	SetActNode(Coord());
	trq.Stop();
	return true;
}

bool SegmentEditor::fCheckClosedSegments(Tranquilizer& trq, long iStartSeg)
{
	SetActNode(Coord());
	trq.SetText(SEDMsgCheckClosedSegments);
	Mask mask(sm->dm(), sMask);
	bool fMask = sMask.length() > 0;
	if (trq.fUpdate(iStartSeg, sm->iFeatures()))
		return false;
	for (int j = iStartSeg; j < sm->iFeatures(); ++j) {
		ILWIS::Segment *seg = (ILWIS::Segment *)sm->getFeature(j);
		if ( !(seg && seg->fValid()))
			continue;
		if (trq.fUpdate(j, sm->iFeatures()))
			return false;
		if (fMask && !seg->fInMask(sm->dvrs(), mask))
			continue;
		Coord crdBegin = seg->crdBegin();
		Coord crdEnd = seg->crdEnd();
		if (crdBegin != crdEnd) {
			trq.Stop();
			SetActNode(crdEnd);
			MessageBeep(MB_ICONEXCLAMATION);
			String sMsg(SEDMsgSegNotClosed_S.scVal(), String("%S (nr %i)", seg->sValue(sm->dvrs()), j));
			String str("%S\n%S", sMsg, SEDMsgZoomInOnError);
			int iRet = mpv->MessageBox(str.sVal(), SEDMsgCheckSegments.sVal(), MB_ICONEXCLAMATION|MB_YESNO);
			if (IDYES == iRet)
				ZoomInOnError();
			return false;
		}
	}
	SetActNode(Coord());
	trq.Stop();
	return true;
}

class AskStartSegForm: public FormWithDest
{
public:
  AskStartSegForm(CWnd* wnd, const String& sRemark,
                  String* sMsk, long iMax, long* iStartSeg, long htp)
  : FormWithDest(wnd, SEDMsgCheckSegments),
    iStart(iStartSeg), sMask(sMsk)
  {
    StaticText* st = new StaticText(root, sRemark);
    st->SetIndependentPos();
    fMask = sMask->length() > 0;
    CheckBox* cbMask = new CheckBox(root, SAFUiMask, &fMask);
    new FieldString(cbMask, "", sMask);
    iOption = 0;
    if (*iStartSeg < 0)
      *iStartSeg = 0;
//    else if (*iStartSeg > 1)
//      iOption = 1;
    RadioGroup* rg = new RadioGroup(root, "", &iOption);
    rg->Align(cbMask, AL_UNDER);
    new RadioButton(rg, SEDUiStartBegin);
    RadioButton* rb = new RadioButton(rg, SEDUiStartSegNum);
    RangeInt ri(1, iMax);
    new FieldInt(rb, "", iStart, ri, true);
    SetMenHelpTopic(htp);
    create();
  }
private:
  int exec() {
    FormWithDest::exec();
    if (!fMask)
      *sMask = "";
    if (0 == iOption)
      *iStart = 0;
    else
      *iStart -= 1;
    return 0;
  }
  long* iStart;
  String* sMask;
  bool fMask;
  int iOption;
};

void SegmentEditor::OnCheckSelf()
{
  long iStart = 0;
  AskStartSegForm frm(mpv, SEDMsgSelfCheck, &sMask, sm->iFeatures()-1, &iStart,
                      htpSegEditCheckSelf);
  if (frm.fOkClicked()) {
//    YieldActive ya;
    Tranquilizer trq(SEDMsgCheckSegments);
		if (fCheckSelf(trq, iStart))
	    mpv->MessageBox(SEDMsgOkSelfCheck.scVal(),SEDErrSegEditor.sVal(),MB_OK|MB_ICONINFORMATION);
  }
}

void SegmentEditor::OnCheckConnected()
{
  long iStart = 0;
  AskStartSegForm frm(mpv, SEDMsgConnectCheck, &sMask, sm->iFeatures()-1, &iStart,
                      htpSegEditCheckConnected);
  if (frm.fOkClicked()) {
  //  YieldActive ya;
    Tranquilizer trq(SEDMsgCheckSegments);
    if (fCheckConnected(trq, iStart))
	    mpv->MessageBox(SEDMsgOkCheckConnected.scVal(),SEDErrSegEditor.sVal(),MB_OK|MB_ICONINFORMATION);
  }
}

void SegmentEditor::OnCheckIntersects()
{
  long iStart = 0;
  AskStartSegForm frm(mpv, SEDMsgIntersectCheck, &sMask, sm->iFeatures()-1, &iStart,
                      htpSegEditCheckIntersects);
  if (frm.fOkClicked()) {
  //  YieldActive ya;
    Tranquilizer trq(SEDMsgCheckSegments);
    if (fCheckIntersects(trq, iStart))
	    mpv->MessageBox(SEDMsgOkIntersects.scVal(),SEDErrSegEditor.sVal(),MB_OK|MB_ICONINFORMATION);
  }
}

void SegmentEditor::OnCheckCodeConsistency()
{
  long iStart = 0;
  AskStartSegForm frm(mpv, SEDMsgCheckCodeConsistency, &sMask, sm->iFeatures()-1, &iStart,
                      htpSegEditCheckCodeConsistency);
  if (frm.fOkClicked()) {
  //  YieldActive ya;
    Tranquilizer trq(SEDMsgCheckSegments);
    if (fCheckCodeConsistency(trq, iStart))
	    mpv->MessageBox(SEDMsgOkCodeConsistency.scVal(),SEDErrSegEditor.sVal(),MB_OK|MB_ICONINFORMATION);
  }
}

void SegmentEditor::OnCheckClosedSegments()
{
  long iStart = 0;
  AskStartSegForm frm(mpv, SEDMsgCheckClosedSegments, &sMask, sm->iFeatures()-1, &iStart,
                      htpSegEditCheckClosedSegments);
  if (frm.fOkClicked()) {
  //  YieldActive ya;
    Tranquilizer trq(SEDMsgCheckSegments);
    if (fCheckClosedSegments(trq, iStart))
	    mpv->MessageBox(SEDMsgOkClosedSegments.scVal(),SEDErrSegEditor.sVal(),MB_OK|MB_ICONINFORMATION);
  }
}

void SegmentEditor::OnRemoveRedundantNodes()
{
  int iRet = mpv->MessageBox(SEDMsgRemoveRedundantNodes.sVal(),
        SEDMsgSegEditor.sVal(), MB_ICONQUESTION|MB_OKCANCEL);
  if (IDOK == iRet) {
    for (SLIter<ILWIS::Segment *> iter(&segList); iter.fValid(); iter.first())
      iter.remove();
		CWaitCursor curWait;
    Tranquilizer trq(SEDMsgSegEditor);
    sm->RemoveRedundantNodes(trq);
    mpv->Invalidate();
  }
}

zIcon SegmentEditor::icon() const
{
	return zIcon("SegIcon");
}

String SegmentEditor::sTitle() const
{
  String s(SEDTitleSegEditor_s.sVal(), sm->sName());
	return s;
}

void SegmentEditor::OnUndoAllChanges()
{
	int iRet = mpv->MessageBox( SEDMsgSegMapUndoAll.sVal(),
				SEDMsgSegEditor.sVal(), MB_ICONQUESTION|MB_OKCANCEL|MB_DEFBUTTON2);
	if (IDOK == iRet) {
    for (SLIter<ILWIS::Segment *> iter(&segList); iter.fValid(); iter.first())
      iter.remove();
		CWaitCursor curWait;
		sm->UndoAllChanges();
		Mode(modeSELECT);
    mpv->Invalidate();
	}
}

void SegmentEditor::OnUpdateFileSave(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(sm->fChanged);
}

void SegmentEditor::OnFileSave()
{
	sm->Store();
	MapCompositionDoc* mcd = mpv->GetDocument();
	mcd->UpdateAllViews(mpv,0);
}

void SegmentEditor::OnSegPack()
{
	CWaitCursor curWait;
	sm->Pack();
}

void SegmentEditor::OnUpdateSegPack(CCmdUI* pCmdUI)
{
	bool fDelSeg = sm->iSegDeleted() > 0;
	pCmdUI->Enable(fDelSeg);
}

void SegmentEditor::ZoomInOnError()
{
	CoordBounds cb = sm->cb();
	double rDist = (cb.width() + cb.height())/20;
	mpv->ZoomInOn(crdActNode, rDist);
}

namespace {
class EditBoundsForm: public FormWithDest
{
public:
  EditBoundsForm(CWnd* wPar, SegmentMap segmap, CoordBounds* cb, bool* fAdaptWindow)
  : FormWithDest(wPar, SCSTitleBoundaries)
  , sm(segmap)
  , cbRef(cb)
  , fDefaultCalculated(false)
  {
		iImg = IlwWinApp()->iImage(".mps");

    fcMin = new FieldCoord(root, SCSUiMinXY, &cb->cMin);
    fcMax = new FieldCoord(root, SCSUiMaxXY, &cb->cMax);
    new PushButton(root, SEDUiDefault, (NotifyProc)&EditBoundsForm::DefaultButton);
    new CheckBox(root, SEDUiAdaptWindow, fAdaptWindow);
    SetMenHelpTopic(htpSegEditSetBoundaries);
    create();
  }
private:
  void CheckBounds(CoordBounds& cb)
  {
	  for (int i = 0; i < sm->iFeatures(); ++i)  {
		ILWIS::Segment *s = (ILWIS::Segment *)sm->getFeature(i);
		if ( s && s->fValid())
			cb += s->cbBounds();
	  }
  }
  int DefaultButton(Event*)
  {
    if (!fDefaultCalculated)
    {
      CoordBounds cb;
      CheckBounds(cb);
      cbDefault = cb;
      fDefaultCalculated = true;
    }
    fcMin->SetVal(cbDefault.cMin);    
    fcMax->SetVal(cbDefault.cMax);    
    return 0;    
  }
  int exec()
  {
    FormWithDest::exec();
    if (fDefaultCalculated)
      *cbRef += cbDefault;
    else
      CheckBounds(*cbRef);
    return 0;
  }
  SegmentMap sm;
  FieldCoord *fcMin, *fcMax;
  bool fDefaultCalculated;
  CoordBounds cbDefault, *cbRef;
};
}


void SegmentEditor::OnSetBoundaries()
{
	CoordBounds cb = sm->cbGetCoordBounds();
	bool fAdaptWindow = false;
	EditBoundsForm frm(mpv, sm, &cb, &fAdaptWindow);
	if (frm.fOkClicked()) 
	{
		sm->SetCoordBounds(cb);
		if (fAdaptWindow) 
		{
			pane()->GetDocument()->SetBounds(cb);
			pane()->OnEntireMap();
		}
	}
}

void SegmentEditor::OnSelectAll()
{
	for (SLIter<ILWIS::Segment *> iter(&segList); iter.fValid(); iter.first()) 
		iter.remove();
	for (int i = 0; i < sm->iFeatures(); ++i)  {
		ILWIS::Segment *s = (ILWIS::Segment *)sm->getFeature(i);
		if ( s && s->fValid())
			segList.append(s);
	}
  pane()->RedrawWindow();
}
