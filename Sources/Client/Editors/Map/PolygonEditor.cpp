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
// PolygonEditor.cpp: implementation of the PolygonEditor class.
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
#include "Client\Editors\Map\PolygonEditor.h"
#include "Client\Mapwindow\IlwisClipboardFormat.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\MainWindow\Catalog\CatalogDocument.h"
#include "Engine\Map\Mapview.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Headers\Hs\Editor.hs"
#include "Headers\constant.h"
#include "Client\FormElements\syscolor.h"
#include "Client\FormElements\fldcolor.h"
#include "Headers\Hs\Appforms.hs"
#include "Client\FormElements\flddom.h"
#include "Headers\Htp\Ilwis.htp"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\Mapwindow\MapWindow.h"
#include "Engine\Domain\DomainUniqueID.h"
#include "Engine\Table\Rec.h"
#include "Client\ilwis.h"

// special ILWIS::Polygon Editor modes not implemented
// only SelectMode is implemented
// see ILWIS 2.2 poledit.c etc. to port (and debug?)

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(PolygonEditor, DigiEditor)
	//{{AFX_MSG_MAP(PolygonEditor)
	ON_COMMAND(ID_EDIT, OnEdit)
  ON_UPDATE_COMMAND_UI(ID_EDIT, OnUpdateEdit)
	ON_COMMAND(ID_COPY, OnCopy)
  ON_UPDATE_COMMAND_UI(ID_COPY, OnUpdateCopy)
	ON_COMMAND(ID_FINDUNDEFS, OnFindUndefs)
  ON_UPDATE_COMMAND_UI(ID_FINDUNDEFS, OnUpdateMode)
	ON_COMMAND(ID_CONFIGURE, OnConfigure)
	ON_COMMAND(ID_POLCREATELBL, OnCreateLabels)
	ON_COMMAND(ID_POLAPPLYLBL, OnApplyLabels)
	ON_COMMAND(ID_POLBOUNDARIES, OnExtractBoundaries)
  ON_COMMAND(ID_FILE_SAVE, OnFileSave)
  ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
  ON_UPDATE_COMMAND_UI(ID_UNDOALL, OnUpdateFileSave)
  ON_COMMAND(ID_UNDOALL, OnUndoAllChanges)
	ON_COMMAND(ID_SELALL, OnSelectAll)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#define sMen(ID) ILWSF("men",ID).scVal()
#define add(ID) men.AppendMenu(MF_STRING, ID, sMen(ID)); 
#define addSub(ID) menSub.AppendMenu(MF_STRING, ID, sMen(ID)); 

PolygonEditor::PolygonEditor(MapPaneView* mpvw, PolygonMap mp)
: DigiEditor(mpvw,mp->cb()),
	pol(mp->polFirst()),
  curEdit("EditCursor"),
  curSegEdit("EditPntCursor"),
  curSegMove("EditPntMoveCursor"),
  curSegMoving("EditPntMovingCursor")
{
	iFmtPnt = RegisterClipboardFormat("IlwisPoints");
	iFmtDom = RegisterClipboardFormat("IlwisDomain");

  coords = 0;
  iNrCoords = 0;
  cNode = Coord();
  fDigBusy = false;
  fRetouching = false;
  fUndelete = false;
	fFindUndefs = false;
//  htpTopic = htpPolygonEditor;
/*
  fnSave = map->fnObj;
  FileName fnEdit = fnSave;
  fnEdit.sFile[0] = '#';
  bool fCopy = true;
  if (File::fExist(fnEdit)) {
    PolygonMap mpOld(fnEdit);
    if (mpOld.fValid()) {
      zMessage msg(mappane->parent(), 
        SEDMsgDelBackup,
        SEDMsgPolEditor, MB_ICONEXCLAMATION|MB_YESNOCANCEL);
      if (msg.isCancel()) {
        fOk = false;
        return;
      }    
      if (msg.isNo())
        fCopy = false;
    }
  }
  if (fCopy)
    Copier::Copy(fnSave, fnEdit);
//    map.Copy(fnEdit);
*/
  pm = mp;  
  dvs = mp->dvrs();
  ILWIS::Polygon *pol = pm->polFirst();
  if (pm->fDependent() || pm->fDataReadOnly() || !pm->dm()->fValidDomain()) {
    mpv->MessageBox(SEDErrNotEditablePolMap.sVal(),SEDErrPolEditor.sVal(),MB_OK|MB_ICONSTOP);
    fOk = false;
    return;
  }
  pm->KeepOpen(true);

  MapCompositionDoc* mcd = mpv->GetDocument();
  for (list<Drawer*>::iterator iter = mcd->dl.begin(); iter != mcd->dl.end(); ++iter) 
	{
		Drawer* dr = *iter;
		if (dr->obj() == pm) {
			drw = dr;
			break;
		}
	}
	if (0 == drw) {
		drw = mcd->drAppend(pm);
		mcd->UpdateAllViews(mpv,2);
	}

//  String s(SEDTitlePolEditor_s.sVal(), mp->sName());
/*
  if (GetPrivateProfileInt("ilwis", "polygoneditor", 0, "ilwis.ini"))
    bbr = ((MapWindow*)mappane->parent())->Buttons("poledit.but");
  else
    bbr = ((MapWindow*)mappane->parent())->Buttons("genedit.but");
*/
	curActive = curEdit;
  if (GetPrivateProfileInt("ilwis", "polygoneditor", 0, "ilwis.ini"))
    AddPolygons(Coord());
  else
    SimpleEditPolygons(Coord());
  mode = modeSELECT;
  col = Color(0,255,0); // green
  colFindUndef = Color(255,0,0); // red
  colRetouch = Color(255,0,0); // red
  colDeleted = Color(0,255,255); // cyan
  
	IlwisSettings settings("Map Window\\Segment Editor");

  String fn = IlwWinApp()->Context()->fnUserINI().sFullName();
  iSnapPixels = GetPrivateProfileInt("Segment Editor", "Snap Pixels", 5, fn.sVal());
	iSnapPixels = settings.iValue("Snap Pixels", iSnapPixels);
  if (iSnapPixels < 1)
    iSnapPixels = 1;
  char sBuf[80];
  String sVal = "yes";
  GetPrivateProfileString("Segment Editor", "Auto Snap", sVal.sVal(), sBuf, 79, fn.sVal());
  fAutoSnap = strcmp(sBuf, "no") ? true : false;
	fAutoSnap = settings.fValue("Auto Snap", fAutoSnap);
  sVal = "yes";
  GetPrivateProfileString("Segment Editor", "Show Nodes", sVal.sVal(), sBuf, 79, fn.sVal());
  fShowNodes = strcmp(sBuf, "no") ? true : false;
  fShowNodes = strcmp(sBuf, "no") ? true : false;
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
	CMenu menSub;
	men.CreateMenu();
  add(ID_UNDOALL);
  add(ID_FILE_SAVE);
	men.AppendMenu(MF_SEPARATOR);
  add(ID_POLCREATELBL);
  add(ID_POLAPPLYLBL);
  add(ID_POLBOUNDARIES);
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
  if (GetPrivateProfileInt("ilwis", "polygoneditor", 0, "ilwis.ini")) 
	{
		add(ID_CUT  );
		add(ID_COPY );
		add(ID_PASTE);
		add(ID_CLEAR);
		men.AppendMenu(MF_SEPARATOR);
	}
	else
		add(ID_COPY );
  add(ID_SELALL);
  add(ID_EDIT);
  if (GetPrivateProfileInt("ilwis", "polygoneditor", 0, "ilwis.ini")) {
    add(ID_SETVAL);
    add(ID_UNDELBND);
		men.AppendMenu(MF_SEPARATOR);
    add(ID_SELECTMODE);
    add(ID_TOPMODE);
    if (GetPrivateProfileInt("ilwis", "debug", 0, "ilwis.ini"))
      add(ID_MERGEMODE);
    add(ID_MOVEMODE);
    add(ID_INSERTMODE);
  }
  add(ID_FINDUNDEFS);

	hmenEdit = men.GetSafeHmenu();
	men.Detach();
	UpdateMenu();
	DataWindow* dw = mpv->dwParent();
	if (dw) {
		dw->bbDataWindow.LoadButtons("poledit.but");
		dw->RecalcLayout();
	}
  htpTopic = htpPolygonEditor;
	sHelpKeywords = "ILWIS::Polygon editor";
}

PolygonEditor::~PolygonEditor()
{
  if (pm.fValid()) {
    bool fDel = true;
    if (pm->fChanged) {
      pm->Updated();
      pm->Store();
    }  
/*
    if (fDel) {
      FileName fnEdit = fnSave;
      fnEdit.sFile[0] = '#';
      PolygonMap mpOld(fnEdit);
      mpOld->fErase = true;
    }
*/
    if (coords)
      delete coords;
    pm->KeepOpen(false);
  }    
}

bool PolygonEditor::OnContextMenu(CWnd* pWnd, CPoint point)
{
  CMenu men;
	men.CreatePopupMenu();
	add(ID_NORMAL);
	add(ID_ZOOMIN);
	add(ID_ZOOMOUT);
	add(ID_PANAREA);
  men.AppendMenu(MF_SEPARATOR);
  if (mode == modeSELECT) {
    add(ID_EDIT);
	  bool fEdit = polList.iSize() != 0;
		men.EnableMenuItem(ID_EDIT, fEdit ? MF_ENABLED : MF_GRAYED);
	}
  if (GetPrivateProfileInt("ilwis", "polygoneditor", 0, "ilwis.ini")) {
    add(ID_SETVAL);
	  men.AppendMenu(MF_SEPARATOR);
    if (mode == modeTOP)
      add(ID_CLEAR);
    add(ID_UNDELBND);
	  men.AppendMenu(MF_SEPARATOR);
  }
  add(ID_FINDUNDEFS);
	men.CheckMenuItem(ID_FINDUNDEFS, fFindUndefs ? MF_CHECKED : MF_UNCHECKED);

  men.AppendMenu(MF_SEPARATOR);
  add(ID_CONFIGURE);
  add(ID_EXITEDITOR);
  men.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, point.x, point.y, pWnd);
	return true;
}

IlwisObject PolygonEditor::obj() const
{
  return pm;
}

void PolygonEditor::OnFindUndefs()
{
  fFindUndefs = !fFindUndefs;
  mpv->Invalidate();
}

void PolygonEditor::OnUpdateMode(CCmdUI* pCmdUI)
{
	switch (pCmdUI->m_nID) {
		case ID_FINDUNDEFS:
			pCmdUI->SetCheck(fFindUndefs);
			return;
	}
}

int PolygonEditor::draw(CDC* cdc, zRect rect, Positioner* psn, volatile bool* fDrawStop)
{
	MapCompositionDoc* mcd = mpv->GetDocument();

	Color c;
	removeDigCursor();

	cdc->SetBkMode(TRANSPARENT);

	MinMax mm = psn->mmSize();
	CoordBounds cbIntern;
	cbIntern.cMin = mcd->georef->cConv(mm.rcMin);
	cbIntern.cMax = mcd->georef->cConv(mm.rcMax);
	cbIntern.Check();
	Domain dm = pm->dm();
	Representation rpr = dm->rpr();

	// draw the segments
	CPen pen(PS_SOLID,1,PALETTEINDEX(col));
	CPen penNul(PS_NULL,1,Color(0));
	CPen* penOld = cdc->SelectObject(&pen);
	CGdiObject* brOld = cdc->SelectStockObject(NULL_BRUSH);
	//    mappane->canvas()->pushBrush(new zBrush(HollowBrush));
	for(int i = 0; i < pm->iFeatures(); ++i) {
		ILWIS::Polygon *pol = (ILWIS::Polygon *)pm->getFeature(i);
		if ( pol == NULL || !pol->fValid())
			continue;
		if (cbIntern.fContains(pol->cbBounds())) 
			drawSegment(cdc,pol->getBoundary());
	}    
	//    delete mappane->canvas()->popBrush();

	// draw the polygons
	if (mode == modeSELECT || mode == modeTOP || mode == modeMERGE) {
		Color cFgBr = SysColor(COLOR_HIGHLIGHT);
		for(int i = 0; i < pm->iFeatures(); ++i) {
			ILWIS::Polygon *pol = (ILWIS::Polygon *)pm->getFeature(i);
			if ( pol == NULL || !pol->fValid())
				continue;
			if (pol->rArea() < 0)
				continue;
			if (cbIntern.fContains(pol->cbBounds())) {
				CPen* penO = cdc->SelectObject(&penNul);
				if (!fFindUndefs) {
					Color color;
					if (pm->dm()->pdvi() || pm->dm()->pdvr()) {
						double rVal = pol->rValue();
						if (rVal == rUNDEF)
							continue;
						color = dr()->clrVal(rVal);
					}
					else {
						long iRaw = pol->iValue();
						if (iRaw == iUNDEF)
							continue;
						color = dr()->clrRaw(iRaw);
					}  
					CBrush br(color);
					CBrush* brO = cdc->SelectObject(&br);
					drawPolygon(cdc,pol);
					cdc->SelectObject(brO); 
				}
				cdc->SelectObject(penO); 
				c = SysColor(COLOR_WINDOW);
				CPen pen3(PS_SOLID, 3, c);
				penO = cdc->SelectObject(&pen3);
				drawPolygonBoundary(cdc,pol);
				cdc->SelectObject(penO); 
				Color cTxt = SysColor(COLOR_WINDOWTEXT);
				CPen penTxt(PS_SOLID,1,cTxt);
				penO = cdc->SelectObject(&penTxt);
				drawPolygonBoundary(cdc,pol);
				cdc->SelectObject(penO);
			}  
		}  
		CPen penFg(PS_SOLID,3,cFgBr);
		CPen* penO = cdc->SelectObject(&penFg);
		for (SLIter<ILWIS::Polygon *> iterPol(&polList); iterPol.fValid(); ++iterPol) {
			//      if (mappane->fDrawCheck())
			//        break;
			if (cbIntern.fContains(iterPol()->cbBounds())) 
				drawPolygonBoundary(cdc,iterPol());
		}    
		cdc->SelectObject(penO);
		CBrush brBlue(HS_DIAGCROSS, cFgBr);
		CBrush* brO = cdc->SelectObject(&brBlue);
		penO = cdc->SelectObject(&penNul);
		c = SysColor(COLOR_WINDOW);
		//    zBrush brWhite(c, UpHatch);
		for (SLIter<ILWIS::Polygon *> iter(&polList); iter.fValid(); ++iter) {
			if (cbIntern.fContains(iter()->cbBounds())) {
				drawPolygon(cdc,iter());
			}  
		} 
		cdc->SelectObject(penO);
		cdc->SelectObject(brO);
	}  
	//  else {
	//  }

	if (fUndelete) {
		CPen penDel(PS_SOLID,1,colDeleted);
		CPen* penO = cdc->SelectObject(&penDel);
		for(int i = 0; i < pm->iFeatures(); ++i) {
			ILWIS::Polygon *pol = (ILWIS::Polygon *)pm->getFeature(i);
			if ( pol == NULL || !pol->fValid())
				continue;break;
			if (pol->fDeleted() && cbIntern.fContains(pol->cbBounds()))
				drawSegment(cdc,pol->getBoundary());
		}	
		cdc->SelectObject(penO);
	}
	if (fFindUndefs) {
		for(int i = 0; i < pm->iFeatures(); ++i) {
			ILWIS::Polygon *pol = (ILWIS::Polygon *)pm->getFeature(i);
			if ( pol == NULL || !pol->fValid())
				continue;
			CoordBounds cb = pol->cbBounds();
			if (!cbIntern.fContains(cb))
				continue;
			drawPolUndef(cdc,pol);
		}      
	}

	drawCoords(cdc, col);
	drawDigCursor();
	cdc->SelectObject(penOld);
	cdc->SelectObject(brOld);
	return 0;
}

void PolygonEditor::drawPolUndef(CDC* cdc, ILWIS::Polygon *pol)
{
  if (pol->rArea() < 0)
    return;
	if (iUNDEF != pol->iValue())
		return;
  CBrush brUndefs(HS_DIAGCROSS, colFindUndef);
	CBrush* brO = cdc->SelectObject(&brUndefs);
	CGdiObject* penOld = cdc->SelectStockObject(NULL_PEN);
  drawPolygon(cdc,pol);
	cdc->SelectObject(penOld);
	CPen penUndefs(PS_SOLID,3,colFindUndef);
	CPen* penO = cdc->SelectObject(&penUndefs);
	drawPolygonBoundary(cdc,pol);

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
	cdc->SelectObject(penO);
  cdc->SelectObject(brO);
}

void PolygonEditor::drawPol(CDC* cdc, ILWIS::Polygon *pol)
{
	if (mode == modeSELECT) {
		Color color;
		if (pm->dm()->pdvi() || pm->dm()->pdvr()) {
			double rVal = pol->rValue();
			color = dr()->clrVal(rVal);
		}
		else {
			long iRaw = pol->iValue();
			color = dr()->clrRaw(iRaw);
		}  
		CBrush br(color);
		CBrush* brOld = cdc->SelectObject(&br);
		CGdiObject* penOld = cdc->SelectStockObject(NULL_PEN);
		drawPolygon(cdc,pol);
		cdc->SelectObject(brOld);
		Color c = SysColor(COLOR_WINDOW);
		CPen pen3(PS_SOLID,c,3);
		cdc->SelectObject(&pen3);
		drawPolygonBoundary(cdc,pol);
		c = SysColor(COLOR_WINDOWTEXT);
		CPen pen1(PS_SOLID,c,1);
		cdc->SelectObject(&pen1);
		drawPolygonBoundary(cdc,pol);
		cdc->SelectObject(penOld);
	}
	else {
		CPen pen(PS_SOLID,col,1);
		CPen* penOld = cdc->SelectObject(&pen);
		drawPolygonBoundary(cdc,pol);
		cdc->SelectObject(penOld);
	}
}

void PolygonEditor::drawSegment(CDC* cdc, Geometry *s)
{
  if (s == NULL || !s->isValid())
    return;
  long iTot;
  MultiLineString  *ml = (MultiLineString *)s;
  for(int j = 0; j < ml->getNumGeometries(); ++j) {
      iTot = -1;
	  CoordinateSequence *coords = ml->getGeometryN(j)->getCoordinates();
	  if (coords->size() == 0)
		return;
	  zPoint* p = new zPoint[coords->size()];
	  for (int i = 0; i < coords->size(); ++i) {
			Coord c = coords->getAt(i);;
			zPoint pnt = mpv->pntPos(c);
			p[++iTot] = pnt;
	  }
	  cdc->Polyline(p, coords->size());
	  if (fRetouching || fShowNodes) {
			CGdiObject* brOld = cdc->SelectStockObject(NULL_BRUSH);
		zPoint pnt;
		pnt = p[0];
		cdc->Rectangle(pnt.x-3,pnt.y-3,pnt.x+4,pnt.y+4);
		pnt = p[iTot];
		cdc->Rectangle(pnt.x-3,pnt.y-3,pnt.x+4,pnt.y+4);
			cdc->SelectObject(brOld);
	  }  
	  if (fRetouching) {
		for (int i = 1; i < coords->size(); ++i) {
		  zPoint pnt;
		  pnt = p[i];
		  cdc->MoveTo(pnt.x-3,pnt.y-3);
		  cdc->LineTo(pnt.x+4,pnt.y+4);
		  cdc->MoveTo(pnt.x+3,pnt.y-3);
		  cdc->LineTo(pnt.x-4,pnt.y+4);
		}
	  }
	  delete coords;
	  delete p;
  }
}

void PolygonEditor::drawPolygon(CDC* cdc, ILWIS::Polygon *pol)
{
//  zPoint *p = new zPoint[16000];
	vector<Array<zPoint>> p;
	p.resize(1 + pol->getNumInteriorRing());
	zRect rect;
	MinMax mm = mpv->mmRect(rect);
	MapCompositionDoc* mcd = mpv->GetDocument();
	DefaultPositioner psn(mpv,mm,mcd->georef);
	psn.iPolPos(pol, p, mcd->georef->cs());
	cdc->Polygon(p[0].buf(), p[0].size());

	for(int j=1; j < p.size(); ++j) {
		cdc->SelectStockObject(WHITE_BRUSH);
		cdc->Polygon(p[j].buf(), p[j].size());
	}
}


void PolygonEditor::drawPolygonBoundary(CDC* cdc, ILWIS::Polygon *pol)
{
	drawSegment(cdc,pol->getBoundary());
}

void PolygonEditor::drawCoords(CDC* cdc, Color clr)
{
  if (iNrCoords && coords != 0) {
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
    zPoint pPivot = mpv->pntPos(cPivot);
    zPoint pLast = mpv->pntPos(cLast);
		cdc->SetROP2(R2_NOT);
    if (pPivot != pLast) {
      cdc->MoveTo(pPivot);
      cdc->LineTo(pLast);
    }
		cdc->SelectObject(penOld);
		cdc->SetROP2(R2_COPYPEN);
  }
}

zIcon PolygonEditor::icon() const
{
	return zIcon("PolIcon");
}

String PolygonEditor::sTitle() const
{
  String s(SEDTitlePolEditor_s.sVal(), pm->sName());
	return s;
}

bool PolygonEditor::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
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
					else
						UnselectAll();
					return true;
/*
				case modeTOP:
					if (fUndelete) {
						fUndelete = false;
						mpv->Invalidate();
						return true;
				  }
				case modeADD:
				case modeMOVE:
					Mode(modeSELECT);
					return true;
				case modeMOVING:
					Mode(modeMOVE);
					return true;
        case modeBOUNDARY:
        case modeISLAND:
				  while (coords != 0)
						DeleteLastPoint(cPivot);
					Mode(modeADD);
					return true;
*/
      }
      break;
/*
    case VK_INSERT:
      if (!fShift && !fCtrl) {
				if (mode == modeADD)
					Mode(modeSELECT);
				else if (mode == modeSELECT)
					Mode(modeADD);
				return true;
      }
      break;
*/
    case VK_RETURN:
      if (!fShift && !fCtrl && mode == modeSELECT) {
				Edit(Coord());
				return true;
      }
      break;
  }
  return false;
}

bool PolygonEditor::OnMouseMove(UINT nFlags, CPoint point)
{
/*
	Coord crd = mpv->crdPnt(point);
  switch (mode) {
    case modeMOVING:
      if (!cb.fUndef() && !cb.fContains(crd)) 
        MessageBeep(-1);
      if (MK_LBUTTON & nFlags)
				MoveCursorPoint(crd);
      else 
				Mode(modeMOVE);
      break;
    case modeNODEMOVING:
      if (!cb.fUndef() && !cb.fContains(crd)) 
        MessageBeep(-1);
      if (MK_LBUTTON & nFlags)
				MoveCursorNode(crd);
      else 
				Mode(modeMOVE);
      break;
    case modeISLAND:
    case modeBOUNDARY:
      if (!cb.fUndef() && !cb.fContains(crd)) {
        MessageBeep(-1);
        return false;
      }  
      if (MK_RBUTTON & nFlags) {
				DeleteLastPoint(crd);
				if (coords == 0)
					Mode(modeADD);
      }
      else if (MK_LBUTTON & nFlags)
				EnterPoints(crd);
      else
				MovePivot(crd);
      break;
  }
*/
	return false;
}

bool PolygonEditor::OnLButtonDown(UINT nFlags, CPoint point)
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
			removeDigCursor();
			CClientDC cdc(mpv);
			cdc.SetBkMode(TRANSPARENT);
			vector<Geometry*> pols = pm->getFeatures(crd);
			if ( pols.size() == 0)
				return true;
			ILWIS::Polygon *pol = (ILWIS::Polygon *)pols[0];
			if (!fShift && !fCtrl && polList.iSize()) {
				UnselectAll();
			}	
			if (!pol->fValid()) {
				MessageBeep(MB_ICONASTERISK);
				return true;
			}  
			bool fAppend = true;
			if (fShift) {
				fAppend = !fCtrl;
				SLIter<ILWIS::Polygon *> iter(&polList);
				for (; iter.fValid(); ++iter) {
					if (iter() == pol) {
						iter.remove();
						fAppend = false;
						Color color;
						if (pm->dm()->pdvi() || pm->dm()->pdvr()) {
							double rVal = pol->rValue();
							color = dr()->clrVal(rVal);
						}
						else {
							long iRaw = pol->iValue();
							color = dr()->clrRaw(iRaw);
						}  
						if (fFindUndefs)
							color = SysColor(COLOR_WINDOW);
						CBrush br(color);
						CBrush* brOld = cdc.SelectObject(&br);
						CPen penNul(PS_NULL,1,Color(0));
						CPen* penOld = cdc.SelectObject(&penNul);
						drawPolygon(&cdc,pol);
						cdc.SelectObject(brOld);
						cdc.SelectObject(penOld);
						Color c = SysColor(COLOR_WINDOW);
						CPen pen3(PS_SOLID,3,c);
						penOld = cdc.SelectObject(&pen3);
						drawPolygonBoundary(&cdc,pol);
						cdc.SelectObject(penOld);
						Color cTxt = SysColor(COLOR_WINDOWTEXT);
						CPen penTxt(PS_SOLID,1,cTxt);
						penOld = cdc.SelectObject(&penTxt);
						drawPolygonBoundary(&cdc,pol);
						cdc.SelectObject(penOld);
						break;
					}
				}  
			}
			if (fAppend) {
				polList.append(pol);
				Color cFgBr = SysColor(COLOR_HIGHLIGHT);
				Color c = SysColor(COLOR_WINDOW);
				Color cTxt = SysColor(COLOR_WINDOWTEXT);
				CPen penFg(PS_SOLID,3,cFgBr);
				CPen* penOld = cdc.SelectObject(&penFg);
				drawPolygonBoundary(&cdc,pol);
				cdc.SelectObject(penOld);
				CBrush brSel(HS_DIAGCROSS,cFgBr);
				CPen penNul(PS_NULL,1,Color(0));
				CBrush* brOld = cdc.SelectObject(&brSel);
				penOld = cdc.SelectObject(&penNul);
				drawPolygon(&cdc,pol);
				cdc.SelectObject(brOld);
				cdc.SelectObject(penOld);
			}  
			drawDigCursor();
		} break;
	
	}
	return true;
}

bool PolygonEditor::OnLButtonUp(UINT nFlags, CPoint point)
{
	Coord crd = mpv->crdPnt(point);

  if (!cb.fUndef() && !cb.fContains(crd)) {
    MessageBeep(-1);
    return true;
  }  
/*
  switch (mode) {
    case modeMOVING:
      NewPosPoint(crd);
      if (coords == 0)
				Mode(modeMOVE);
      break;
    case modeNODEMOVING:
      NewPosNode(crd);
      if (cNode.fUndef())
				Mode(modeMOVE);
      break;
    case modeISLAND:
      if (Evt->isButton(1) && !Evt->isButtonDown(2)) {
        if (Evt->isShift() || fAutoSnap) {
          if (0 == EndIslandMouse(crd))
            Mode(modeADD);
        }    
      }
      break;
    case modeBOUNDARY:  
      if (Evt->isButton(1) && !Evt->isButtonDown(2)) {
        if (Evt->isCtrl()) {
          if (0 == SnapSplitEndBoundaryMouse(crd))
            Mode(modeADD);
        }    
        else if (Evt->isShift()) {
          if (0 == SnapEndBoundaryMouse(crd))
            Mode(modeADD);
        }    
        else if (fAutoSnap) {
          if (0 == AutoEndBoundaryMouse(crd))
            Mode(modeADD);
        }    
      }
      break;
  }    
*/
	return true;
}

void PolygonEditor::UnselectAll()
{
	CClientDC cdc(mpv);
	cdc.SetBkMode(TRANSPARENT);
	Color c = SysColor(COLOR_WINDOW);
	Domain dm = pm->dm();
	Representation rpr = dm->rpr();
	for (SLIter<ILWIS::Polygon *> iter(&polList); iter.fValid();) 
	{
		ILWIS::Polygon *pol = iter();
		CPen penNul(PS_NULL,1,Color(0));
		CPen* penOld = cdc.SelectObject(&penNul);
		Color color;
		bool fUndef = false;
		if (pm->dm()->pdvi() || pm->dm()->pdvr()) {
			double rVal = pol->rValue();
			fUndef = rVal == rUNDEF;
			color = dr()->clrVal(rVal);
		}
		else {
			long iRaw = pol->iValue();
			fUndef = iRaw == iUNDEF;
			color = dr()->clrRaw(iRaw);
		}  
		if (fFindUndefs) 
			color = SysColor(COLOR_WINDOW);     
		if (fFindUndefs && fUndef)
			drawPolUndef(&cdc,pol);
		else
		{
			CBrush br(color);
			CBrush* brOld = cdc.SelectObject(&br);
			drawPolygon(&cdc,iter());
			cdc.SelectObject(brOld);
			cdc.SelectObject(penOld);
			CPen pen3(PS_SOLID,3,c);
			penOld = cdc.SelectObject(&pen3);
			drawPolygonBoundary(&cdc,iter());
			cdc.SelectObject(penOld);
			Color cTxt = SysColor(COLOR_WINDOWTEXT);
			CPen penTxt(PS_SOLID,1,cTxt);
			penOld = cdc.SelectObject(&penTxt);
			drawPolygonBoundary(&cdc,iter());
		}
		cdc.SelectObject(penOld);
		iter.remove();
	}  
}

void PolygonEditor::OnUpdateEdit(CCmdUI* pCmdUI)
{
  BOOL fEdit = polList.iSize() != 0;
	pCmdUI->Enable(fEdit);
}
 
void PolygonEditor::EditAttrib(int iRec)
{
  Table tbl = pm->tblAtt();
  if (tbl.fValid()) {
    Ilwis::Record rec = tbl->rec(iRec);
		mpv->ShowRecord(rec);
	}
}

int PolygonEditor::Edit(const Coord& crd)
{
	SLIter<ILWIS::Polygon *> iter(&polList);
	if (polList.iSize() == 0) 
		return 0;
	pol = iter();
	sValue = pol->sValue(pm->dvrs());
	DomainUniqueID* duid = dm()->pdUniqueID();

	if (polList.iSize() == 1) {
		if (duid) {
			int iRaw = iter()->iValue();
			EditAttrib(iRaw);
		}
		else if (crd.fUndef()) 
		{
AskID:      
			String sRemark(SEDRemAreaLength_rr.scVal(), pol->rArea()/1e4, pol->rLen());
			if (AskValue(sRemark, htpPolEditorAskValue)) 
			{
				if (dm()->pdid()) 
				{
					ILWIS::Polygon *pol = (ILWIS::Polygon *)pm->getFeature(sValue);
					if (pol->fValid()) 
					{
						int iRet = mpv->MessageBox(SEDMsgValInUse.sVal(), SEDMsgPolEditor.sVal(),
							MB_YESNO|MB_DEFBUTTON2|MB_ICONASTERISK);
						if (IDYES != iRet) 
							goto AskID;
					}
				}  
				for (; iter.fValid(); ++iter) {
					iter()->PutVal(pm->dvrs(),sValue);
					SetDirty(iter());  
				}  
				pm->Updated();
			}
		}
		else
			EditFieldStart(crd, sValue);
	}
	else {
		if (dm()->pdid()) {
			mpv->MessageBox(SEDMsgPolEdOnlyIndiv.sVal(), SEDMsgPolEditor.sVal());
		}
		else {
			long iSel = polList.iSize();
			String sRemark;
			if (iSel > 1)
				sRemark = String(SEDRemPolSel_i.sVal(), iSel);
			else
				sRemark = String(SEDRemAreaLength_rr.scVal(), pol->rArea()/1e4, pol->rLen());
			if (AskValue(sRemark, htpPolEditorAskValue)) {
				for (; iter.fValid(); ++iter) {
					iter()->PutVal(pm->dvrs(), sValue);
					SetDirty(iter()); 
				}  
				pm->Updated();
			}
		}
	}
	return 1;
}

void PolygonEditor::EditFieldOK(Coord crd, const String& s)
{
  if (dm()->pdid()) {
    ILWIS::Polygon *pol = (ILWIS::Polygon *)pm->getFeature(s);
    if (pol->fValid()) {
      int iRet = mpv->MessageBox(SEDMsgValInUse.sVal(), SEDMsgPolEditor.sVal(),
                   MB_YESNO|MB_DEFBUTTON2|MB_ICONASTERISK);
      if (IDYES != iRet) {
        EditFieldStart(crd, s);
        return;
      }           
    }
  }
  vector<Geometry *> pols = pm->getFeatures(crd);
  ((ILWIS::Polygon *)pols[0])->PutVal(pm->dvrs(),s);
  int iRaw = pol->iValue();
  EditAttrib(iRaw);
  pm->Updated();
  SetDirty(pols[0]);
}

void PolygonEditor::SetDirty(const CoordBounds& cb, bool fEraseBackground)
{
  Coord c1 = cb.cMin;
  Coord c2 = cb.cMax;
  zPoint p1 = mpv->pntPos(c1);
  zPoint p2 = mpv->pntPos(c2);
  if (p1.x > p2.x) {
    int iTmp = p1.x; p1.x = p2.x; p2.x = iTmp; 
  }
  if (p1.y > p2.y) {
    int iTmp = p1.y; p1.y = p2.y; p2.y = iTmp; 
  }
  p1.x -= 2; p2.x += 2;
  p1.y -= 2; p2.y += 2;
  zRect rect(p1, p2);
  mpv->InvalidateRect(&rect,fEraseBackground);
}

void PolygonEditor::SetDirty(ILWIS::Polygon *pol, bool fEraseBackground)
{
  if (!pol->fValid())
    return;
  CoordBounds cb = pol->cbBounds();
  SetDirty(cb,fEraseBackground);
}

void PolygonEditor::SetDirty(Geometry *seg, bool fEraseBackground)
{
	if (!seg->isValid())
    return;
  Geometry *geom = seg->getEnvelope();
  if ( geom) {
	  CoordBounds cb;
	  CoordinateSequence *seq = geom->getCoordinates();
	  cb.cMin = seq->getAt(0);
	  cb.cMax = seq->getAt(2);
	  delete seq;
	SetDirty(cb,fEraseBackground);
  }
}

void PolygonEditor::OnUpdateCopy(CCmdUI* pCmdUI)
{
	bool fCopyOk = polList.iSize() != 0;
	pCmdUI->Enable(fCopyOk);
}

void PolygonEditor::OnCopy()
{
}

  class PolConfigForm: public FormWithDest
  {
  public:
    PolConfigForm(CWnd* wPar, PolygonEditor* edit)
    : FormWithDest(wPar, SEDTitleCnfPolEdit)
    {
      new FieldColor(root, SEDUiCursorColor, &edit->colDig);
			new FieldColor(root, SEDUiFindUndefColor, &edit->colFindUndef);
			if (GetPrivateProfileInt("ilwis", "polygoneditor", 0, "ilwis.ini")) {
				new CheckBox(root, SEDUiAutoSnap, &edit->fAutoSnap);
				new CheckBox(root, SEDUiShowNodes, &edit->fShowNodes);
				//new FieldReal(root, SEDUiSnapTolerance, &edit->sm->rSnapDist);
				new FieldInt(root, SEDUiSnapPixels, &edit->iSnapPixels);
				//new FieldReal(root, SEDUiTunnelTolerance, &edit->sm->rTunnelWidth);
				new FieldColor(root, SEDUiNormalColor, &edit->col);
				new FieldColor(root, SEDUiRetoucheColor, &edit->colRetouch);
				new FieldColor(root, SEDUiDeletedColor, &edit->colDeleted);
			}
//    SetHelpTopic(htpPolygonEditor);
      SetMenHelpTopic(htpPolEditCnf);
      create();
    }
  };



void PolygonEditor::OnConfigure()
{
  PolConfigForm frm(mpv, this);
  if (frm.fOkClicked()) {
		IlwisSettings settings("Map Window\\Segment Editor");
		settings.SetValue("Cursor Color", colDig);
		settings.SetValue("Find Undef Color", colFindUndef);
		settings.SetValue("Auto Snap", fAutoSnap);
		settings.SetValue("Show Nodes", fShowNodes);
		settings.SetValue("Snap Pixels", iSnapPixels);
		settings.SetValue("Normal Color", col);
		settings.SetValue("Retouch Color", colRetouch);
		settings.SetValue("Deleted Color", colDeleted);
  }  
}


class CreateLabelsForm: public FormWithDest
{
public:
  CreateLabelsForm(CWnd* wnd, String* sLblPntMap, bool* fEdit)
  : FormWithDest(wnd, SAFTitleCreateLbls),
    sPntMap(sLblPntMap)
  {
    fmc = new FieldPointMapCreate(root, SAFUiOutPntMap, sLblPntMap);
    fmc->SetCallBack((NotifyProc)&CreateLabelsForm::CallBackName);
//    new CheckBox(root, SAFUiEdit, fEdit);
    String sFill('*', 40);
    stRemark = new StaticText(root, sFill);
    stRemark->SetIndependentPos();
    SetMenHelpTopic(htpPolEditCreateLabels);
    create();
  }
private:
  int CallBackName(Event*)
  {
    fmc->StoreData();
    FileName fn(*sPntMap);
    fn.sExt = ".mpp";
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
  FieldPointMapCreate* fmc;
  StaticText* stRemark;
  String* sPntMap;
};

class ApplyLabelsForm: public FormWithDest
{
public:
  ApplyLabelsForm(CWnd* wnd, String* sLblPntMap, const Domain& dm)
  : FormWithDest(wnd, SAFTitlePolMapLabels)
  {
    new FieldDataType(root, SAFUiLabelPnts, sLblPntMap,
                      new PointMapListerDomain(dm->fnObj), true);
    SetMenHelpTopic(htpPolEditApplyLabels);
    create();
  }
};

class _export CreateSegMapForm: public FormWithDest
{
public:
  CreateSegMapForm(CWnd* wnd, String* sSegmentMap, String* sDomain, bool* fEdit)
  : FormWithDest(wnd, SAFTitlePolBndSegMap),
    sSegMap(sSegmentMap)
  {
    fmc = new FieldSegmentMapCreate(root, SAFUiOutSegMap, sSegmentMap);
    fmc->SetCallBack((NotifyProc)&CreateSegMapForm::CallBackName);
    CheckBox* cb = new CheckBox(root, SAFUiEdit, fEdit);
    cb->Align(fmc, AL_AFTER);
    FieldDomainC* fd = new FieldDomainC(root, SAFUiDomain, sDomain);
    fd->Align(fmc, AL_UNDER);
    String sFill('*', 40);
    stRemark = new StaticText(root, sFill);
    stRemark->SetIndependentPos();
    SetMenHelpTopic(htpPolEditCreateSegMap);
    create();
  }
private:
  int CallBackName(Event*)
  {
    fmc->StoreData();
    FileName fn(*sSegMap);
    fn.sExt = ".mps";
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
  FieldSegmentMapCreate* fmc;
  StaticText* stRemark;
  String* sSegMap;
};

void PolygonEditor::OnCreateLabels()
{
  try {
    String sLblPnt = pm->sName(false);
    bool fEdit = false;
    CreateLabelsForm frm(mpv, &sLblPnt, &fEdit);
    if (frm.fOkClicked()) {
      FileName fn(sLblPnt);
      fn.sExt = ".mpp";
      String sExpr("PointMapPolLabels(%S)", pm->sNameQuoted());
      PointMap mpOut(fn, sExpr);
      mpOut->BreakDependency();
    }
  }
  catch (ErrorObject& err) {
    err.Show();
  }
}

void PolygonEditor::OnApplyLabels()
{
  try {
    String sLblPnt = pm->sName(false);
    ApplyLabelsForm frm(mpv, &sLblPnt, dm());
    if (frm.fOkClicked()) {
      Tranquilizer trq(SAFTitleApplyLabels);
      trq.SetText(SAFRemApplyLabelPoints);
      PointMap pntmap(sLblPnt);
      bool fUseReals = pntmap->dvrs().fUseReals();
      long iPnt = pntmap->iFeatures();
      CoordSystem csPol = pm->cs();
      CoordSystem csLbl = pntmap->cs();
      bool fTransformCoords = csPol != csLbl;
      for (long i = 1; i <= iPnt; ++i) {
        if (trq.fUpdate(i, iPnt))
          return;
        Coord crdPnt = pntmap->cValue(i);
        if (fTransformCoords)
           crdPnt = csPol->cConv(csLbl, crdPnt);
		ILWIS::Polygon *pol = (ILWIS::Polygon *)(pm->getFeatures(crdPnt)[0]); // point in polygon
        if (pol->fValid())
          if (fUseReals)
            pol->PutVal(pntmap->rValue(i));
          else
            pol->PutVal(pntmap->iRaw(i));
      }
      mpv->Invalidate();
    }
  }
  catch (ErrorObject& err) {
    err.Show();
  }
}

void PolygonEditor::OnExtractBoundaries()
{
  try {
    String sSegMap = pm->sName(false);
    String sDomain;
    bool fEdit = false;
    CreateSegMapForm frm(mpv, &sSegMap, &sDomain, &fEdit);
    FileName fn(sSegMap);
    bool fOk = frm.fOkClicked();
  }
  catch (ErrorObject& err) {
    err.Show();
  }
}



int PolygonEditor::AddPolygons(Coord)
{
	// see version 2 for the code
	return 0;
}

int PolygonEditor::SimpleEditPolygons(Coord crd)
{
  dc = dcCROSS;
  MoveCursor(crd);
  ChangeWindowBasis = (DigiFunc)&PolygonEditor::SimpleEditPolygons;
  return SetDigiFunc(SEDDigEditPolygons, (DigiFunc)&DigiEditor::MoveCursor,
		     (DigiFunc)&PolygonEditor::EditValue, SEDDigEditValue,
		     NULL, "",
		     (DigiFunc)&DigiEditor::ChangeWindow, SEDDigChangeWindow,
		     NULL, "");
}

void PolygonEditor::OnUpdateFileSave(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(pm->fChanged);
}

void PolygonEditor::OnFileSave()
{
	pm->Store();
	MapCompositionDoc* mcd = mpv->GetDocument();
	mcd->UpdateAllViews(mpv,0);
}

int PolygonEditor::EditValue(Coord crd)
{
  if (iLastButton == 1)
    return 1;
  ILWIS::Polygon *pol = (ILWIS::Polygon *)pm->getFeatures(crd)[0];
  if (pol == NULL || !pol->fValid()) {
    MessageBeep(MB_ICONASTERISK);
    return 1;
  }
  sValue = pol->sValue(pm->dvrs());
  String sRemark(SEDRemAreaLength_rr.scVal(), pol->rArea()/1e4, pol->rLen());
  if (AskValue(sRemark, htpPolEditorAskValue)) {
    pol->PutVal(pm->dvrs(),sValue);
    pm->Updated();
		CClientDC cdc(mpv);
    drawPol(&cdc, pol);
  }
  return 1;
}

void PolygonEditor::OnUndoAllChanges()
{
	int iRet = mpv->MessageBox( SEDMsgSegMapUndoAll.sVal(),
				SEDMsgSegEditor.sVal(), MB_ICONQUESTION|MB_OKCANCEL|MB_DEFBUTTON2);
	if (IDOK == iRet) {
		UnselectAll();    
		CWaitCursor curWait;
		pm->UndoAllChanges();
    mpv->Invalidate();
	}
}

void PolygonEditor::OnSelectAll()
{
	for (SLIter<ILWIS::Polygon *> iter(&polList); iter.fValid(); iter.first()) 
		iter.remove();
}
