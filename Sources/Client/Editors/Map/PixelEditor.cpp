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
// PixelEditor.cpp: implementation of the PixelEditor class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Engine\Map\Segment\Seg.h"
#include "Engine\Map\Polygon\POL.H"
#include "Client\Mapwindow\Positioner.h"
#include "Client\Mapwindow\Drawers\BaseDrawer.h"
#include "Client\Editors\Editor.h"
#include "Client\Base\ButtonBar.h"
#include "Client\Editors\Map\PixelEditor.h"
#include "Headers\Hs\Editor.hs"
#include "Client\FormElements\syscolor.h"
#include "Client\Mapwindow\AreaSelector.h"
#include "Client\Mapwindow\IlwisClipboardFormat.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\MainWindow\Catalog\CatalogDocument.h"
#include "Engine\Map\Mapview.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Headers\constant.h"
#include "Engine\Domain\dmsort.h"
#include "Engine\Domain\dmclass.h"
#include "Headers\Htp\Ilwis.htp"
#include "Client\Mapwindow\MapWindow.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//const int iMAXSEL = 100000;

BEGIN_MESSAGE_MAP(PixelEditor, Editor)
	//{{AFX_MSG_MAP(PixelEditor)
	ON_COMMAND(ID_COPY, OnCopy)
  ON_UPDATE_COMMAND_UI(ID_COPY, OnUpdateCopy)
	ON_COMMAND(ID_PASTE, OnPaste)
  ON_UPDATE_COMMAND_UI(ID_PASTE, OnUpdatePaste)
	ON_COMMAND(ID_EDIT, OnEdit)
  ON_UPDATE_COMMAND_UI(ID_EDIT, OnUpdateCopy)
	ON_COMMAND(ID_CLEAR, OnClear)
  ON_UPDATE_COMMAND_UI(ID_CLEAR, OnUpdateCopy)
	ON_COMMAND(ID_CUT, OnCut)
  ON_UPDATE_COMMAND_UI(ID_CUT, OnUpdateCopy)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#define sMen(ID) ILWSF("men",ID).scVal()
#define addmen(ID) men.AppendMenu(MF_STRING, ID, sMen(ID)); 

PixelEditor::PixelEditor(MapPaneView* mpvw, Map m, int iMaxSelection)
: Editor(mpvw), mp(m), 
	rFactVisibleLimit(0.6),
	bmTmp(0), iMAXSEL(iMaxSelection)
{
	iFmtPnt = RegisterClipboardFormat("IlwisPoints");
	iFmtDom = RegisterClipboardFormat("IlwisDomain");

  mp->KeepOpen(true);
  dvs = mp->dvrs();
  _rpr = dvs.dm()->rpr();
	fValues = dvs.fValues();
	fRealValues = dvs.fRealValues();
	fImage = 0 != dvs.dm()->pdi();
	fBool = 0 != dvs.dm()->pdbool();
	fBit = 0 != dvs.dm()->pdbit();
	pdc = dvs.dm()->pdc();

	curActive = zCursor("EditCursor");

  zRect rect;
  mpv->GetClientRect(&rect);
  zPoint pnt;
  pnt.x = rect.left()/2 + rect.right()/2;
  pnt.y = rect.top()/2 + rect.bottom()/2;
  rcCursor = mpv->rcPos(pnt);
  iShift(true);

  CMenu men;
	men.CreateMenu();
	addmen(ID_EXITEDITOR);
	hmenFile = men.GetSafeHmenu();
	men.Detach();

	men.CreateMenu();
  addmen(ID_CUT  );
  addmen(ID_COPY );
  addmen(ID_PASTE);
  addmen(ID_CLEAR);
  men.AppendMenu(MF_SEPARATOR);
  addmen(ID_EDIT);
	hmenEdit = men.GetSafeHmenu();
	men.Detach();
	UpdateMenu();

	DataWindow* dw = mpv->dwParent();
	if (dw) {
		dw->bbDataWindow.LoadButtons("pixedit.but");
		dw->RecalcLayout();
	}
  htpTopic = htpPixelEditor;
	sHelpKeywords = "Pixel editor";
	mp->DeletePyramidFile();
}

PixelEditor::~PixelEditor()
{
	if (0 != bmTmp)
		delete bmTmp;
}

IlwisObject PixelEditor::obj() const
{
	return mp;
}

int PixelEditor::iShift(bool fMessage) const
{
  double rScale = mpv->rScale();
  int iLimit = 2;
  if (rFactVisibleLimit >= 0.5)
    iLimit = 1;
  if (rScale < iLimit) {
    rScale = 0;
/*    if (fMessage)
      mpv->MessageBox(SEDMsgZoomInToEdit.sVal(),
	       SEDMsgPixEditor.sVal(), MB_OK|MB_ICONEXCLAMATION);*/
  }
  return rounding(rScale);
}

int PixelEditor::draw(CDC* cdc, zRect rect, Positioner* psn, volatile bool* fDrawStop)
{
	if (mpv->fBusyDrawing()) // back ground is being drawn
		return 0;
  int iShft = iShift(false);
//  if (iShft == 0)
//    return 0;
  if (rect.width() == 0 || rect.height() == 0) return 0;
	CWaitCursor wc;
	// restrict to invalidated area
  MinMax mm(psn->rcPos(rect.TopLeft()), psn->rcPos(rect.BottomRight()));
	double rFact = 1/(double)iShft;
  bool fIntMap = mp->dm()->stNeeded() > stBYTE;
  if (mm.MinCol() < 0) {
    rect.left() -= round(mm.MinCol() / rFact);
    mm.MinCol() = 0;
  }
  if (mm.MinRow() < 0) {
    rect.top() -= round(mm.MinRow() / rFact);
    mm.MinRow() = 0;
  }
  long iRow, iCol;
  iRow = mp->rcSize().Row;
  iCol = mp->rcSize().Col;
  if (mm.MaxCol() >= iCol) {
    rect.right() -= round((mm.MaxCol()-iCol) / rFact);
    mm.MaxCol() = iCol - 1;
  }
  if (mm.MaxRow() >= iRow) {
    rect.bottom() -= round((mm.MaxRow()-iRow) / rFact);
    mm.MaxRow() = iRow - 1;
  }

  RowCol rc;
  Coord c;
  zPoint pnt;
  mp->KeepOpen(true);
	MapCompositionDoc* mcd = mpv->GetDocument();

  FrameWindow* fw = mpv->fwParent();
  if (fw) {
    if (rFact >= rFactVisibleLimit)
		  fw->status->SetWindowText(SEDMsgZoomInToEdit.sVal());
    else
		  fw->status->SetWindowText("");
	}

  if (rFact < rFactVisibleLimit) {
    Color col = SysColor(COLOR_WINDOW);
    cdc->SetBkColor(col);
    col = SysColor(COLOR_WINDOWTEXT);
    cdc->SetTextColor(col);

		TEXTMETRIC tm;
		cdc->GetTextMetrics(&tm);
		int iH2 = tm.tmHeight / 2;
    UINT iOldAlign = cdc->SetTextAlign(TA_CENTER|TA_TOP);
    int iFact = iShft; //round(1/rFact);
    int iFact2 = iFact / 2;
    int i1, i2;
    i1 = iFact >= 4 ? iFact / 4 : 0;
    i2 = iFact + 1 - i1;

		CGdiObject* penOld = cdc->SelectStockObject(NULL_PEN);
		CGdiObject* brOld = cdc->SelectStockObject(NULL_BRUSH);
    for (iRow = mm.MinRow(); iRow <= mm.MaxRow(); ++iRow) {
 		  if (*fDrawStop)
			  break;
      for (iCol = mm.MinCol(); iCol <= mm.MaxCol(); ++iCol) {
   		  if (*fDrawStop)
	  		  break;
				rc = RowCol(iRow,iCol);
				Color clr(iUNDEF);
				if (fRealValues) {
				  double rVal = mp->rValue(rc);
					if (rVal != rUNDEF)
					  clr = clrVal(rVal);
				}
				else {
				 long iRaw = mp->iRaw(rc);
				 if (iRaw != iUNDEF) 
           clr = clrRaw(iRaw);
				}
        if ((long)clr != iUNDEF) {
					CBrush br(clr);
          CBrush* brOld = cdc->SelectObject(&br);
          pnt = mpv->pntPos(rc);
          cdc->Rectangle(pnt.x+i1,pnt.y+i1,pnt.x+i2,pnt.y+i2);
          cdc->SelectObject(brOld);
        }
				if (rFact < 0.04) {
					c = mp->gr()->cConv(rc);
					pnt = mpv->pntPos(c);
					String str = mp->sValue(rc,0);
					char* s = str.sVal();
					int iLen = strlen(s);
					while (cdc->GetTextExtent(s,iLen).cx > iFact)
						iLen -= 1;
					pnt.y -= iH2;
					cdc->TextOut(pnt.x,pnt.y,s,iLen);
				}
      }
    }

		cdc->SelectObject(penOld);
		cdc->SelectObject(brOld);
    cdc->SetTextAlign(iOldAlign);
  }
  mp->KeepOpen(false);

	drawSelect();
  drawCursor();

	return 0;
}

void PixelEditor::drawCursor()
{
  if (rcCursor.fUndef()) return;
  int iShft = iShift();
  if (iShft == 0)
    return;
  zPoint pnt = mpv->pntPos(rcCursor);
	zRect rect(pnt,pnt);
  rect.right() += iShft;
  rect.bottom() += iShft;

	CClientDC cdc(mpv);

	if (0 != bmTmp)
    delete bmTmp;
	bmTmp = new CBitmap;
	bmTmp->CreateCompatibleBitmap(&cdc,iShft,iShft);

	CDC cdcTmp;
	cdcTmp.CreateCompatibleDC(&cdc);
	CBitmap* bmOld = cdcTmp.SelectObject(bmTmp);
	cdcTmp.BitBlt(0,0,iShft,iShft,&cdc,pnt.x,pnt.y,SRCCOPY);
  cdcTmp.SelectObject(bmOld);

  Color col(0,0,0);
	cdc.SetBkColor(col);
  col = Color(255,255,255);

	CPen pen, *penOld;
	pen.CreatePen(PS_DOT, 1, col);
	penOld = cdc.SelectObject(&pen);
	CGdiObject* brOld = cdc.SelectStockObject(NULL_BRUSH);
	cdc.Rectangle(&rect);
	cdc.SelectObject(penOld);
	cdc.SelectObject(brOld);
}

void PixelEditor::removeCursor()
{
  if (rcCursor.fUndef()) return;
	if (bmTmp == 0)
    return;
  int iShft = iShift();
  if (iShft == 0)
    return;
  zPoint pnt = mpv->pntPos(rcCursor);
  zRect rect(pnt,pnt);
  rect.right() += iShft;
  rect.bottom() += iShft;

	CClientDC cdc(mpv);

	CDC cdcTmp;
	cdcTmp.CreateCompatibleDC(&cdc);
	CBitmap* bmOld = cdcTmp.SelectObject(bmTmp);
	cdc.BitBlt(pnt.x,pnt.y,iShft,iShft,&cdcTmp,0,0,SRCCOPY);
  cdcTmp.SelectObject(bmOld);
}

bool PixelEditor::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
  bool fCursor = false;
  bool fEdit = false;
  RowCol rcNew = rcCursor;

  bool fCtrl = GetKeyState(VK_CONTROL) & 0x8000 ? true : false;
  bool fShift = GetKeyState(VK_SHIFT) & 0x8000 ? true : false;
  switch (nChar)
  {
    case VK_LEFT:
      fCursor = true;
      rcNew.Col -= 1;
      break;
    case VK_RIGHT:
      fCursor = true;
      rcNew.Col += 1;
      break;
    case VK_UP:
      fCursor = true;
      rcNew.Row -= 1;
      break;
    case VK_DOWN:
      fCursor = true;
      rcNew.Row += 1;
      break;
    case VK_RETURN:
    case VK_EXECUTE:
      fEdit = true;
      if (rcSelect.iSize() > 0)
				break;
      // else fall through
    case VK_SPACE:
    case VK_SELECT: 
			removeCursor();
      switchSelect(rcCursor);
			rcCursor = rcUNDEF;
			mpv->UpdateWindow();
			rcCursor = rcNew;
      drawCursor();
			break; 
    case VK_ESCAPE: 
      removeCursor();
      clrSelect();
			rcCursor = rcUNDEF;
			mpv->UpdateWindow();
			rcCursor = rcNew;
      drawCursor();
      break; 
    case VK_DELETE:
//      OnEditClear();
      break;
    default:
      return false;
  }
  if (fCursor) {
    removeCursor();
    rcCursor = rcNew;
    if (fCtrl)
      if (fShift)
				clearSelect(rcCursor);
      else
				addSelect(rcCursor);
    else
      if (fShift)
				switchSelect(rcCursor);
    zRect rect;
    mpv->GetClientRect(rect);
    RowCol rc = rcNew;
    zPoint pnt1 = mpv->pntPos(rc);
    rc.Col += 1;
    rc.Row += 1;
    zPoint pnt2 = mpv->pntPos(rc);
    if (pnt1.x < rect.left()   || pnt2.x < rect.left()  ||
		    pnt1.x > rect.right()  || pnt2.x > rect.right() ||
		    pnt1.y < rect.top()    || pnt2.y < rect.top()   ||
		    pnt1.y > rect.bottom() || pnt2.y > rect.bottom())
    {
			switch (nChar)
			{
				case VK_LEFT:
					mpv->horzLineMove(-1);
		      break;
				case VK_RIGHT:
					mpv->horzLineMove(1);
		      break;
		    case VK_UP:
					mpv->vertLineMove(-1);
		      break;
			  case VK_DOWN:
					mpv->vertLineMove(1);
		      break;
			}
    }
    else
      drawCursor();
  }
  //PutStatusLine(rcNew);
  if (fEdit) {
    Coord c = mpv->cConv(rcCursor);
    Edit(c);
  }
	return true;
}

bool PixelEditor::OnLButtonDown(UINT nFlags, CPoint point)
{
	mpv->SetFocus();
  int iShft = iShift(true);
//  if (iShft == 0)
//    return true;
  bool fCtrl = nFlags & MK_CONTROL ? true : false;
  bool fShift = nFlags & MK_SHIFT ? true : false;
  removeCursor();
  rcCursor = rcUNDEF;
	Coord crd = mpv->crdPnt(point);
  RowCol rc = mp->gr()->rcConv(crd);
  if (!fCtrl && !fShift) {
    clrSelect();
		if (DragDetect(*mpv, point)) {
			mpv->UpdateWindow();
			rcCursor = rc;
			drawCursor();
			if (mpv->as)
				return true;
			mpv->as = new AreaSelector(mpv, this, (NotifyRectProc)&PixelEditor::AreaSelected);
			mpv->as->OnLButtonDown(nFlags, point);
			SetCursor(curActive);
			return true;
		}
  }
  if (fCtrl)
    if (fShift)
      clearSelect(rc);
    else
      addSelect(rc);
  else 
    switchSelect(rc);
  rcCursor = rc;
  drawCursor();
  return true;
}

void PixelEditor::addSelect(RowCol rc)
{
  for(int i = 0; i < rcSelect.iSize(); ++i)
    if (rcSelect[i] == rc)
      return;

  if (rcSelect.iSize() >= iMAXSEL) {
    errorMaxSelect();
    return;
  }
  rcSelect &= rc;
  drawSelect(rc);
}

void PixelEditor::clearSelect(RowCol rc)
{
  for(int i = 0; i < rcSelect.iSize(); ++i)
    if (rcSelect[i] == rc) {
      rcSelect.Remove(i,1);
      clrSelect(rc);
      return;
    }
}

void PixelEditor::switchSelect(RowCol rc)
{
  for(int i = 0; i < rcSelect.iSize(); ++i)
    if (rcSelect[i] == rc) {
      rcSelect.Remove(i,1);
      clrSelect(rc);
    	SelectionChanged();
      return;
    }
  if (rcSelect.iSize() >= iMAXSEL) {
    errorMaxSelect();
    return;
  }
  rcSelect &= rc;
  drawSelect(rc);
	SelectionChanged();
}

void PixelEditor::errorMaxSelect()
{
	String s(SEDErrPixMaxSelect_i.scVal(), iMAXSEL);
	mpv->MessageBox(s.sVal(), SEDErrPixEditor.sVal(), MB_ICONSTOP|MB_OK);
	mpv->as->Stop();
}

void PixelEditor::drawSelect(RowCol rc)
{
  int iShft = iShift();
//	if (iShft == 0)
//		return;
	CClientDC cdc(mpv);

	CBitmap bm;
  short bits[8] = { 0x99, 0x66, 0x66, 0x99, 0x99, 0x66, 0x66, 0x99 };
	bm.CreateBitmap(8,8,1,1,bits);
	CBrush br(&bm);
	CBrush* brOld = cdc.SelectObject(&br);
	CGdiObject* penOld = cdc.SelectStockObject(NULL_PEN);
  Color col = SysColor(COLOR_WINDOW); //zColor(255,255,255);
  cdc.SetBkColor(col);
  col = SysColor(COLOR_HIGHLIGHT); //zColor(0,0,0);
  cdc.SetTextColor(col);
  if (rc.fUndef())
    for(int i = 0; i < rcSelect.iSize(); ++i)
      drawSelectPix(&cdc, rcSelect[i], iShft);
  else
    drawSelectPix(&cdc, rc, iShft);
	cdc.SelectObject(brOld);
	cdc.SelectObject(penOld);
}

void PixelEditor::drawSelectPix(CDC* cdc, RowCol rc, int iShft)
{
  int i1, i2;
  i1 = iShft > 4 ? 1 : 0;
  i2 = iShft + 1 - i1;
  zPoint pnt = mpv->pntPos(rc);
	if (iShft <= 4)
    cdc->Rectangle(pnt.x+i1,pnt.y+i1,pnt.x+i2,pnt.y+i2);
  else {
    int j1, j2;
    j1 = max(3, iShft / 4);
    j2 = iShft + 1 - j1;
    cdc->Rectangle(pnt.x+i1,pnt.y+i1,pnt.x+i2,pnt.y+j1);
    cdc->Rectangle(pnt.x+i1,pnt.y+j1-1,pnt.x+j1,pnt.y+j2+1);
    cdc->Rectangle(pnt.x+j2,pnt.y+j1-1,pnt.x+i2,pnt.y+j2+1);
    cdc->Rectangle(pnt.x+i1,pnt.y+j2,pnt.x+i2,pnt.y+i2);
  }
}

void PixelEditor::clrSelect(RowCol rc)
{
  int iShft = iShift();
//  if (iShft == 0)
//    return;
  if (rc.fUndef()) {
		if (rcSelect.iSize() > 10000) 
			mpv->Invalidate();
		else
			for(int i = 0; i < rcSelect.iSize(); ++i)
				clrSelectPix(rcSelect[i], iShft);
    rcSelect.Reset();
  }
  else
    clrSelectPix(rc, iShft);
}

void PixelEditor::clrSelectPix(RowCol rc, int iShft)
{
  int i1, i2;
  i1 = iShft > 4 ? 1 : 0;
  i2 = iShft - i1;
  zPoint pnt = mpv->pntPos(rc);
	CRect rect(pnt.x+i1-1,pnt.y+i1-1,pnt.x+i2+1,pnt.y+i2+1);
  mpv->InvalidateRect(&rect);
}

void PixelEditor::AreaSelected(CRect rect)
{
  MinMax mm = mpv->mmRect(rect);
	if (mm.width() == 0 && mm.height() == 0 &&
		  rect.Height() < 2 && rect.Width() < 2) {
	  mpv->as->Stop();
		return;
	}
  RowCol rc;
  long iSize = (mm.MaxRow() - mm.MinRow() + 1) *
	       (mm.MaxCol() - mm.MinCol() + 1);
  if (iSize > iMAXSEL) {
    errorMaxSelect();
    return;
  }
  rcSelect.Reset();
  rcSelect.Resize(iSize);
  long i = 0;
  for (rc.Row = mm.MinRow(); rc.Row <= mm.MaxRow(); rc.Row++)
    for (rc.Col = mm.MinCol(); rc.Col <= mm.MaxCol(); rc.Col++, ++i)
      rcSelect[i] = rc;
  int iShft = iShift();
//  if (iShft == 0)
//    return;
  InflateRect((LPRECT)&rect,iShft,iShft);
  mpv->InvalidateRect(&rect);
  mpv->as->Stop();
	SelectionChanged();
}

Color PixelEditor::clrGet(RowCol rc)
{
  Color clr(iUNDEF);
  if (fRealValues) {
    double rVal = mp->rValue(rc);
    if (rVal != rUNDEF)
      clr = clrVal(rVal);
  }
  else {
    long iRaw = mp->iRaw(rc);
    if (iRaw != iUNDEF)
      clr = clrRaw(iRaw);
  }
  return clr;
}

Color PixelEditor::clrRaw(long iRaw) const
{
  Color clr(iUNDEF);
  if (iRaw == iUNDEF) 
		return clr;

  if (fImage) 
    return clrVal(iRaw);
	else if (fBool) {
		switch (iRaw) {
			case 0:						 // undef
				return Color(255,255,255);
			case 1:						 // false
				return Color(168,168,168);
			case 2:						 // true
				return Color(0,176,20);
		}
	}
	else if (fBit) {
		switch (iRaw) {
			case 0:						 // false
				return Color(168,168,168);
			case 1:						 // true
				return Color(0,176,20);
		}
	}
  else if (fValues) {
    double rVal = dvrs().rValue(iRaw);
    return clrVal(rVal);
  }
  if (0 != pdc) {
    if (_rpr.fValid()) 
			return _rpr->clrRaw(iRaw);
  }
  clr = clrPrimary(1+iRaw%31);
  return clr;
}

Color PixelEditor::clrVal(double rVal) const
{
  Color cRet;
  if (!_rpr.fValid() || rVal == rUNDEF)
    return cRet;
  if (0 == _rpr->prv())
    cRet = _rpr->clr(rVal,rrStretchRange());
  else
    cRet = _rpr->clr(rVal);
  return cRet;
}

void PixelEditor::OnEdit()
{
	Coord c = mpv->cConv(rcCursor);
	Edit(c);
}
 
int PixelEditor::Edit(const Coord& c)
{
  return Edit(c,htpPixEditorAskValue);
}

int PixelEditor::Edit(const Coord& c, unsigned int htp)
{
  int iShft = iShift(true);
  if (iShft == 0)
    return 1;
  rcCursor = mpv->rcConv(c);
  if (rcSelect.iSize() == 0) {
    removeCursor();
    switchSelect(rcCursor);
    drawCursor();
  }
  RowCol rc = mpv->rcConv(c);
  sValue = mp->sValue(rc,0);
  if (rcSelect.iSize() == 1)
    EditFieldStart(c, sValue);
  else {
    long iSel = rcSelect.iSize();
    String sRemark(SEDRemPixSel_i.sVal(), iSel);
    if (AskValue(sRemark, htp)) {
			CWaitCursor cw;
			bool fFast = rcSelect.iSize() < 10000;
      for(int i = 0; i < rcSelect.iSize(); ++i) {
				mp->PutVal(rcSelect[i],sValue);
				zPoint pnt = mpv->pntPos(rcSelect[i]);
				zRect rect(pnt,pnt);
				rect.right() += iShft;
				rect.bottom() += iShft;
				if (fFast)
					mpv->InvalidateRect(&rect);
      }
      mp->Updated();
			if (!fFast)
				mpv->Invalidate();
			FrameWindow* fw = mpv->fwParent();
			if (fw) {
				MapCompositionDoc* mcd = mpv->GetDocument();
				if (mp == mcd->mp)
					fw->status->SetWindowText(SEDMsgRedrawToUpdate.sVal());
			}
    }
    else 
      return 0;
  }
  return 1;
}

void PixelEditor::EditFieldOK(Coord crd, const String& s)
{
//  if (_dm->pdid())
//    _dm->pdid()->iAdd(s);
  RowCol rc = mp->gr()->rcConv(crd);
  mp->PutVal(rc,s);
  mp->Updated();
  //PutStatusLine(rc);
  zPoint pnt = mpv->pntPos(rc);
  zRect rect(pnt,pnt);
  int iShft = iShift();
  if (iShft == 0)
    return;
  rect.right() += iShft;
  rect.bottom() += iShft;
  mpv->InvalidateRect(&rect);
	FrameWindow* fw = mpv->fwParent();
	if (fw) {
		MapCompositionDoc* mcd = mpv->GetDocument();
		if (mp == mcd->mp)
			fw->status->SetWindowText(SEDMsgRedrawToUpdate.sVal());
	}
}

RangeReal PixelEditor::rrStretchRange() const
{
  RangeReal rr;
  if (mp->dm()->pdv()) {
    rr = mp->rrPerc1();
    if (!rr.fValid()) {
      rr = mp->rrMinMax();
      if (!rr.fValid())
        if (mp->dm()->pdi())
          rr = RangeReal(0,255);
        else
          rr = mp->vr()->rrMinMax();
    }
  }
  return rr;
}

void PixelEditor::PreDraw()
{
	removeCursor();
}

void PixelEditor::OnUpdateCopy(CCmdUI* pCmdUI)
{
  bool fCopy = rcSelect.iSize() > 0;
  int iShft = iShift();
	if (iShft == 0)
		fCopy = false;
	pCmdUI->Enable(fCopy);
}

void PixelEditor::OnCopy()
{
	CWaitCursor curWait;
	if (!mpv->OpenClipboard())
		return;
	EmptyClipboard();

  // Ilwis Point Format
  long iSize = rcSelect.iSize();
  IlwisPoint* ip = new IlwisPoint[1+iSize];
  ip[0].c = Coord();
  ip[0].iRaw = iSize;
  for (int i = 0; i < iSize; ++i) {
    ip[1+i].c = mpv->cConv(rcSelect[i]);
    ip[1+i].iRaw = mp->iRaw(rcSelect[i]);
  }
	int iLen = (1+iSize) * sizeof(IlwisPoint);
	HANDLE hnd = GlobalAlloc(GMEM_MOVEABLE,iLen);
	void* pv = GlobalLock(hnd);
	memcpy(pv, ip, iLen);
	GlobalUnlock(hnd);
	SetClipboardData(iFmtPnt, hnd);

  // Ilwis Domain Format
  IlwisDomain* id = new IlwisDomain(mp->dm(), mp->vr());
	iLen = sizeof(IlwisDomain);
	hnd = GlobalAlloc(GMEM_MOVEABLE,iLen);
	pv = GlobalLock(hnd);
	memcpy(pv, id, iLen);
	GlobalUnlock(hnd);
	SetClipboardData(iFmtDom, hnd);
  delete id;

  // Text Format
  bool fNone = mp->gr()->fGeoRefNone();
  if (!fNone) {
		int iSIZE = 1000000;
		char* sBuf = new char[iSIZE];
    char* s = sBuf;
    long iTotLen = 0;
    int iLen;
    for (long i = 1; i <= iSize; ++i) {
      String str, sVal;
      sVal = dvrs().sValueByRaw(ip[i].iRaw,0);
      str = String("%.3f\t%.3f\t%S\r\n", ip[i].c.x, ip[i].c.y, sVal);
      iLen = str.length();
      iTotLen += iLen;
      if (iTotLen >= iSIZE)
        break;
      strcpy(s, str.scVal());
      s += iLen;
    }
    *s = '\0';
		hnd = GlobalAlloc(GMEM_FIXED, strlen(sBuf)+2);
		char* pc = (char*)GlobalLock(hnd);
		strcpy(pc,sBuf);
		GlobalUnlock(hnd);
		SetClipboardData(CF_TEXT,hnd);
    delete sBuf;
  }
  delete ip;

  // Bitmap Format
  MinMax mm;
  for (long i = 0; i < iSize; ++i)
    mm += rcSelect[i];
  if (mm.width() <= 2048 && mm.height() <= 2048) {
    BitmapPositioner psnBm(0, mm, mp->gr());
    zRect rect = psnBm.rectSize();
		rect.right() += 1;
		rect.bottom() += 1;

		CClientDC cScr(mpv);
		CDC cdc;
		cdc.CreateCompatibleDC(&cScr);
		CBitmap bm;
		bm.CreateCompatibleBitmap(&cScr,rect.width(),rect.height());
		CBitmap* bmOld = cdc.SelectObject(&bm);

		cdc.SelectStockObject(WHITE_BRUSH);
		cdc.SelectStockObject(WHITE_PEN);
		cdc.Rectangle(&rect);

		MapCompositionDoc* mcd = mpv->GetDocument();
    for (long i = 0; i < iSize; ++i) {
      RowCol rc = rcSelect[i];
      Color clr = clrGet(rc);
      if ((long)clr != iUNDEF) {
        zPoint pnt = psnBm.pntPos(rc);
				cdc.SetPixel(pnt, clr);
      }
    }
		cdc.SelectObject(bmOld);
		SetClipboardData(CF_BITMAP, bm);
		bm.Detach();
  }
	
	// metafile format
	// was commented away in 2.2

	CloseClipboard();
}

void PixelEditor::OnUpdatePaste(CCmdUI* pCmdUI)
{
  bool fPaste = IsClipboardFormatAvailable(iFmtPnt) ? true : false;
	pCmdUI->Enable(fPaste);
}

void PixelEditor::OnPaste()
{
  if (!IsClipboardFormatAvailable(iFmtPnt))
		return;
	CWaitCursor curWait;
	if (!mpv->OpenClipboard())
		return;

  unsigned int iSize;
  bool fConvert = false, fValues = false, fSort = false;
  Domain dmMap, dmCb;
  ValueRange vrCb;
  if (IsClipboardFormatAvailable(iFmtDom))
	{
    dmMap = mp->dm();

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
        mpv->MessageBox(SEDErrNotValueInClipboard.sVal(),SEDErrPixEditor.sVal(),MB_OK|MB_ICONSTOP);
				CloseClipboard();
        return;
      }
      ValueRange vrMap = mp->vr();
      vrCb = id.vr();
      fValues = true;
      if (vrMap != vrCb)
        fConvert = true;
    }
    else if (dmMap->pdc()) {
      if (0 == dmCb->pdc()) {
        mpv->MessageBox(SEDErrNotClassInClipboard.sVal(),SEDErrPixEditor.sVal(),MB_OK|MB_ICONSTOP);
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

  clrSelect();
  rcSelect.Resize(iSize);
  int iShft = iShift();
  for (int i = 0; i < iSize; ++i) {
    RowCol rc =  mpv->rcConv(ip[1+i].c);
    rcSelect[i] = rc;
    long iRaw = ip[1+i].iRaw;
    if (iUNDEF != iRaw) {
      if (fConvert) {
        if (fValues) {
          double rVal;
          if (vrCb.fValid())
            rVal = vrCb->rValue(iRaw);
          else
            rVal = iRaw;
          mp->PutVal(rc, rVal);
        }
        else {
          String sVal;
          if (vrCb.fValid())
            sVal = vrCb->sValueByRaw(dmCb,iRaw, 0);
          else
            sVal = dmCb->sValueByRaw(iRaw, 0);
          if ("?" == sVal)
            continue;
          if (fSort) {
            iRaw = dmMap->iRaw(sVal);
            if (iUNDEF == iRaw) {
              String sMsg(SEDMsgNotInDomain_SS.sVal(), sVal, dmMap->sName());
              int iRet = mpv->MessageBox(sMsg.sVal(),SEDMsgPixEditor.sVal(),MB_YESNOCANCEL|MB_ICONASTERISK);
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
            mp->PutRaw(rc, iRaw);
          }
          else
            if (dmMap->fValid(sVal))
              mp->PutVal(rc, sVal);
        }
      }
      else
        mp->PutRaw(rc, iRaw);
    }
    zPoint pnt = mpv->pntPos(rc);
    zRect rect(pnt,pnt);
    rect.right() += iShft;
    rect.bottom() += iShft;
    mpv->InvalidateRect(&rect);
  }
  mp->Updated();
  delete (char*)ip;

	CloseClipboard();
}

bool PixelEditor::OnContextMenu(CWnd* pWnd, CPoint point)
{
  CMenu men, menSub;
	men.CreatePopupMenu();
	addmen(ID_NORMAL);
	addmen(ID_ZOOMIN);
	addmen(ID_ZOOMOUT);
	addmen(ID_PANAREA);
  men.AppendMenu(MF_SEPARATOR);
  addmen(ID_EDIT);
  bool fEdit = rcSelect.iSize() != 0;
  men.EnableMenuItem(ID_EDIT, fEdit ? MF_ENABLED : MF_GRAYED);
  men.AppendMenu(MF_SEPARATOR);
  addmen(ID_EXITEDITOR);
  menSub.CreateMenu();
  men.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, point.x, point.y, pWnd);
	return true;
}
																									 
void PixelEditor::OnClear()
{
  int iShft = iShift();
	if (iShft == 0)
		return;
  int iRet = mpv->MessageBox(SEDMsgClearSelPixels.sVal(), SEDMsgPixEditor.sVal(),
               MB_YESNO|MB_ICONQUESTION|MB_DEFBUTTON2);
  if (IDYES == iRet) {
	 	CWaitCursor curWait;
		int iSize = rcSelect.iSize();
    for(int i = 0; i < iSize; ++i) {
      mp->PutVal(rcSelect[i],"?");
			if (iSize < 10000) {
				zPoint pnt = mpv->pntPos(rcSelect[i]);
				zRect rect(pnt,pnt);
				rect.right() += iShft;
				rect.bottom() += iShft;
				mpv->InvalidateRect(&rect);
			}
    }
    mp->Updated();
		if (iSize >= 10000)
			mpv->Invalidate();
  }
}

void PixelEditor::OnCut()
{
	OnCopy();
	OnClear();
}

zIcon PixelEditor::icon() const
{
	return zIcon("MapIcon");
}

String PixelEditor::sTitle() const
{
  String s(SEDTitlePixEditor_s.sVal(), mp->sName());
	return s;
}

void PixelEditor::SelectionChanged()
{
	// nothing
}
