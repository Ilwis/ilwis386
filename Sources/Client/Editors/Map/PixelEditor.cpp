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
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Engine\Drawers\ComplexDrawer.h"

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
#include "Engine\Drawers\TextDrawer.h"
#include "Engine\Drawers\OpenGLText.h"

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
	ON_COMMAND(ID_SELECTAREA, OnAreaSelected)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#define sMen(ID) ILWSF("men",ID).c_str()
#define addmen(ID) men.AppendMenu(MF_STRING, ID, sMen(ID)); 

PixelEditor::PixelEditor(MapPaneView* mpvw, Map m, int iMaxSelection)
: Editor(mpvw), mp(m),
	rFactVisibleLimit(0.1),
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

  RowCol rc = RowCol(pnt.y,pnt.x);

  Coord crd = mpv->GetDocument()->rootDrawer->screenToOpenGL(rc);
  rcCursor = mp->gr()->rcConv(crd);
  rcCursor = mpv->rcPos(pnt);
  iShift(true);

	MapCompositionDoc* mcd = mpv->GetDocument();
	vector<NewDrawer *> allDrawers;
	mcd->rootDrawer->getDrawers(allDrawers);
	for(int i = 0; i < allDrawers.size(); ++i) {
		SpatialDataDrawer *dr = dynamic_cast<SpatialDataDrawer *>(allDrawers.at(i));
		if ( dr) {
			if ( dr->getBaseMap()->fnObj == mp->fnObj) {
				Editor::drw = dr->getDrawer(0);
				break;
			}
		}
	}

	if (0 == Editor::drw) {
		SpatialDataDrawer *dr = dynamic_cast<SpatialDataDrawer *>(mcd->drAppend(mp, IlwisDocument::otEDIT, IlwisWinApp::osNormal));
		if (dr) {
			Editor::drw = dr->getDrawer(0);
		}
		mcd->UpdateAllViews(mpv,2);
	}

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
  help = "ilwis\\pixel_editor_functionality.htm";
	sHelpKeywords = "Pixel editor";
	mp->DeletePyramidFile();

	drawSelect(rc);
}

PixelEditor::~PixelEditor()
{
	if (0 != bmTmp)
		delete bmTmp;
	mp->KeepOpen(false);
	if (Editor::drw) {
		PreparationParameters pp(NewDrawer::ptREDRAW, 0);
		Editor::drw->prepare(&pp);
	}
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
				Coord crd = mp->gr()->cConv(rc);
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
          pnt = mpv->pntPos(crd);
          cdc->Rectangle(pnt.x+i1,pnt.y+i1,pnt.x+i2,pnt.y+i2);
          cdc->SelectObject(brOld);
        }
				if (rFact < 0.04) {
					pnt = mpv->pntPos(crd);
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
 // drawCursor();

	return 0;
}

void PixelEditor::drawCursor()
{
}

void PixelEditor::removeCursor()
{
  if (rcCursor.fUndef()) return;
	if (bmTmp == 0)
    return;
  int iShft = iShift();
  if (iShft == 0)
    return;

  Coord crd = mp->gr()->cConv(rcCursor);
  zPoint pnt = mpv->pntPos(crd);
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
      //drawCursor();
			break; 
    case VK_ESCAPE: 
      removeCursor();
      clrSelect();
			rcCursor = rcUNDEF;
			mpv->UpdateWindow();
			rcCursor = rcNew;
      //drawCursor();
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
	Coord crd = mp->gr()->cConv(rcNew);
    zPoint pnt1 = mpv->pntPos(crd);
    crd.x += 1;
    crd.y += 1;
    zPoint pnt2 = mpv->pntPos(crd);
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
  //else
  //     drawCursor();
  }
  //PutStatusLine(rcNew);
  if (fEdit) {
	  Edit(mp->gr()->cConv(rcCursor));
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
    clrSelect(RowCol());
		if (DragDetect(*mpv, point)) {
			mpv->UpdateWindow();
			rcCursor = rc;
			//drawCursor();
			AreaSelector * as = new AreaSelector(mpv, this, (NotifyRectProc)&PixelEditor::AreaSelected);
			as->OnLButtonDown(nFlags, point);
			mpv->addTool(as, ID_SELECTAREA);
//			SetCursor(curActive);
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
  //drawCursor();
  SelectionChanged();
  mpv->PostMessage(WM_COMMAND, ID_SELECTAREA);
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
}

void PixelEditor::clearSelect(RowCol rc)
{
  for(int i = 0; i < rcSelect.iSize(); ++i)
    if (rcSelect[i] == rc) {
	  drawSelectPix(rc,false,false);
      rcSelect.Remove(i,1);
      //clrSelect(rc);
      return;
    }
}

void PixelEditor::switchSelect(RowCol rc)
{
  for(int i = 0; i < rcSelect.iSize(); ++i)
    if (rcSelect[i] == rc) {
      rcSelect.Remove(i,1);
      drawSelectPix(rc,false,false);
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
	String s(TR("Maximum %i pixels can be selected").c_str(), iMAXSEL);
	mpv->MessageBox(s.c_str(), SEDErrPixEditor.sVal(), MB_ICONSTOP|MB_OK);
}

void PixelEditor::drawSelect(RowCol rc) // to do openGL
{
}

void PixelEditor::drawSelectPix(CDC* cdc, RowCol rc, int iShft)
{
  int i1, i2;
  i1 = iShft > 4 ? 1 : 0;
  i2 = iShft + 1 - i1;
  Coord crd = mp->gr()->cConv(rc);
  zPoint pnt = mpv->pntPos(crd);
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
  drawSelectPix(rc,false,false);
 // int i1, i2;
 // i1 = iShft > 4 ? 1 : 0;
 // i2 = iShft - i1;
 // Coord crd = mp->gr()->cConv(rc);
 // zPoint pnt = mpv->pntPos(crd);
	//CRect rect(pnt.x,pnt.y,pnt.x+i2+1,pnt.y+i2+1);
 // mpv->InvalidateRect(&rect);
}

void PixelEditor::AreaSelected(CRect rect)
{
	Coord c1 = mpv->crdPnt(rect.TopLeft());
	Coord c2 = mpv->crdPnt(rect.BottomRight());

	RowCol rcTLeft = mp->gr()->rcConv(c1);
	RowCol rcBRight = mp->gr()->rcConv(c2);

	long iSize = (rcBRight.Row - rcTLeft.Row + 1) *
	       (rcBRight.Col - rcTLeft.Col + 1);
	if (iSize > iMAXSEL)
	{
		errorMaxSelect();
		return;
	}

	RowCol rc;
	rcSelect.Reset();
    rcSelect.Resize(iSize);
	long i=0;
	for (rc.Row = rcTLeft.Row; rc.Row <= rcBRight.Row; rc.Row++)
	{
		for (rc.Col = rcTLeft.Col; rc.Col <= rcBRight.Col; rc.Col++, ++i)
			rcSelect[i] = rc;
	}
    mpv->InvalidateRect(&rect);

    SelectionChanged();
	mpv->PostMessage(WM_COMMAND, ID_SELECTAREA);
	if (rcSelect.size()>0)
		drawSelect();
}

void PixelEditor::OnAreaSelected()
{
	mpv->changeStateTool(ID_SELECTAREA, false);
	mpv->Invalidate();
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
  clr = Color::clrPrimary(1+iRaw%31);
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
	Coord c = mp->gr()->cConv(rcCursor);
	Edit(c);
}

int PixelEditor::Edit(const Coord& c)
{
  return Edit(c,htpPixEditorAskValue);
}

int PixelEditor::Edit(const Coord& c, unsigned int htp)
{
  rcCursor = mp->gr()->rcConv(c);
  if (rcSelect.iSize() == 0)
	  return 1;

  sValue = mp->sValue(c,0);
  if (rcSelect.iSize() == 1)
  {
    EditFieldStart(c, sValue);
  }
  else {
    long iSel = rcSelect.iSize();
    String sRemark(SEDRemPixSel_i.sVal(), iSel);
    if (AskValue(sRemark, "ilwismen\\pixsel_editor_edit.htm"))
	{
		CWaitCursor cw;
		for(int i = 0; i < rcSelect.iSize(); ++i) {
			mp->PutVal(rcSelect[i],sValue);
		}

		mp->Updated();
		FrameWindow* fw = mpv->fwParent();
		if (fw) {
			//MapCompositionDoc* mcd = mpv->GetDocument();
			//if (mp == mcd->mp)
			fw->status->SetWindowText(SEDMsgRedrawToUpdate.sVal());
		}

		PreparationParameters pp(NewDrawer::ptREDRAW, 0);
		Editor::drw->prepare(&pp);
		mpv->Invalidate();
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
	FrameWindow* fw = mpv->fwParent();
	if (fw) {
		//MapCompositionDoc* mcd = mpv->GetDocument();
		//if (mp == mcd->mp)
		fw->status->SetWindowText(SEDMsgRedrawToUpdate.sVal());
	}
	PreparationParameters pp(NewDrawer::ptREDRAW, 0);
	Editor::drw->prepare(&pp);
	mpv->Invalidate();
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

//void PixelEditor::PreDraw()
//{
//	removeCursor();
//}

void PixelEditor::OnUpdateCopy(CCmdUI* pCmdUI)
{
  bool fCopy = rcSelect.iSize() > 0;
//  int iShft = iShift();
//	if (iShft == 0)
//		fCopy = false;
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
  if (0 == ip)
	  return;
  ip[0].x = rUNDEF;
  ip[0].y = rUNDEF;
  ip[0].iRaw = iSize-1;

  for (int i = 0; i < iSize; ++i) {
	ip[1+i].x = rcSelect[i].Col;
	ip[1+i].y = rcSelect[i].Row;
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
      str = String("%.3f\t%.3f\t%S\r\n", ip[i].x, ip[i].y, sVal);
      iLen = str.length();
      iTotLen += iLen;
      if (iTotLen >= iSIZE)
        break;
	  strcpy(s, str.c_str());
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
	Coord crd = Coord(ip[1+i].y,ip[1+i].x);
	RowCol rc = mp->gr()->rcConv(crd);
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
        mp->PutRaw(crd, iRaw);
    }
    zPoint pnt = mpv->pntPos(crd);
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
				zPoint pnt = mpv->pntPos(mp->gr()->cConv(rcSelect[i]));
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

MinMax PixelEditor::getMinMax(CoordBounds cb)
{
	Coord minpnt;
	Coord maxpnt;

	minpnt.x = cb.MinX();
	minpnt.y = cb.MinY();
	minpnt.z = 0.0;

	maxpnt.x = cb.MaxX();
	maxpnt.y = cb.MaxY();
	maxpnt.z = 0.0;

	RowCol minrc = mp->gr()->rcConv(minpnt);
	RowCol maxrc = mp->gr()->rcConv(maxpnt);
	return MinMax(minrc,maxrc);
}

int PixelEditor::draw(volatile bool* fDrawStop)
{
	double rsz = mp->gr()->rPixSize();
	drawSelection();

	if (mpv->fBusyDrawing()) // back ground is being drawn
		return 0;

	double zfactor = 0;

	MapCompositionDoc* mcd = mpv->GetDocument();
	RowCol pixArea (mcd->rootDrawer->getViewPort());
	GeoRef gr = mp->gr();
	if (gr.fValid()) {
		CoordBounds cb = gr->cb();
		if (gr->cs().fValid() && mcd->rootDrawer->fConvNeeded(gr->cs()))
			cb = mcd->rootDrawer->getCoordinateSystem()->cbConv(gr->cs(), cb);
		RowCol rcSize = gr->rcSize();
		CoordBounds cbZoom = mcd->rootDrawer->getCoordBoundsZoom();
		if (pixArea.Col > pixArea.Row) { // the largest side will give the most accurate result
			zfactor = ((double)rcSize.Col * cbZoom.width()) / ((double)pixArea.Col * cb.width());
		} else {	
			zfactor = ((double)rcSize.Row * cbZoom.height()) / ((double)pixArea.Row * cb.height());
		}
	}

	FrameWindow* fw = mpv->fwParent();
    if (fw)
    {
		if (zfactor >= rFactVisibleLimit)
			  fw->status->SetWindowText(SEDMsgZoomInToEdit.sVal());
		else
			  fw->status->SetWindowText("");
    }

	if (zfactor > rFactVisibleLimit || zfactor>0.4)
		return 0;

	CoordBounds mpcb = mcd->rootDrawer->getMapCoordBounds();

	MinMax mpmm = getMinMax(mpcb);
	if (mpmm.rcMin.Row<0)
		mpmm.rcMin.Row = 0;

	if (mpmm.rcMin.Col<0)
		mpmm.rcMin.Col = 0;

	CRect rect;
	rect.top = mpmm.rcMin.Row;
	rect.left = mpmm.rcMin.Col;
	rect.right = mpmm.rcMax.Col;
	rect.bottom = mpmm.rcMax.Row;

	CoordBounds cb = mcd->rootDrawer->getCoordBoundsZoomExt();
	if (cb.fUndef())
		return 0;

	MinMax mm = getMinMax(cb);

	bool fIntMap = mp->dm()->stNeeded() > stBYTE;
	if (mm.MinCol() < 0) {
		rect.left -= round(mm.MinCol() / zfactor);
		mm.MinCol() = 0;
	}
	if (mm.MinRow() < 0) {
		rect.top -= round(mm.MinRow() / zfactor);
		mm.MinRow() = 0;
	}
	long iRow, iCol;
	iRow = mp->rcSize().Row;
	iCol = mp->rcSize().Col;
	if (mm.MaxCol() >= iCol) {
		rect.right -= round((mm.MaxCol()-iCol) / zfactor);
		mm.MaxCol() = iCol - 1;
	}
	if (mm.MaxRow() >= iRow) {
		rect.bottom -= round((mm.MaxRow()-iRow) / zfactor);
		mm.MaxRow() = iRow - 1;
	}
	RowCol rc;
	Coord c;
	zPoint pnt;
	mp->KeepOpen(true);

    Color col = SysColor(COLOR_WINDOW);

	RootDrawer * rootDrawer = mcd->rootDrawer;
	CoordBounds cbZoom = rootDrawer->getCoordBoundsZoom();
	double delta = 3 * cbZoom.width() / 2000.0;

	Color cText, cBack, cFgBr, cBgBr;
	cFgBr = SysColor(COLOR_HIGHLIGHT);
	cBgBr = SysColor(COLOR_WINDOW);

	Color colText, colBack;
	colText = (Color)-1;
	colBack = (Color)-2;

	cText.alpha() = 255;
	cText = SysColor(COLOR_WINDOWTEXT);
	if ((long)colBack != -2)
	{
		cBack = colBack;
	}

	ILWIS::DrawerParameters dpLayerDrawer (rootDrawer, 0);
	ILWIS::TextLayerDrawer *textLayerDrawer = (ILWIS::TextLayerDrawer *)NewDrawer::getDrawer("TextLayerDrawer", "ilwis38",&dpLayerDrawer);
	ILWIS::DrawerParameters dpTextDrawer (rootDrawer, textLayerDrawer);
	ILWIS::TextDrawer *textDrawer = (ILWIS::TextDrawer *)NewDrawer::getDrawer("TextDrawer","ilwis38",&dpTextDrawer);
	OpenGLText * font = new OpenGLText (rootDrawer, "arial.ttf", 14, true, 0, -14);
	textLayerDrawer->setFont(font);

	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
	glLineWidth(1.0);

	///

	for (iRow = mm.MinRow(); iRow <= mm.MaxRow(); ++iRow)
	{
		if (*fDrawStop || zfactor==0 )
			break;
		for (iCol = mm.MinCol(); iCol <= mm.MaxCol(); ++iCol)
		{
			if (*fDrawStop || zfactor==0 )
				break;
			rc = RowCol(iRow,iCol);
			Color clr(iUNDEF);
			if ((long)clr == iUNDEF)
				int a =0;

			if (fRealValues)
			{
				double rVal = mp->rValue(rc);
				if (rVal != rUNDEF)
					clr = clrVal(rVal);
			}
			else
			{
				long iRaw = mp->iRaw(rc);
				if (iRaw != iUNDEF)
					clr = clrRaw(iRaw);
			}
			if (clr.iVal() != iUNDEF)
			{
				drawSelectPix(rc,true,true);
			}
			if (zfactor < 0.04 )
			{
				// draw selected pixels
				c = mp->gr()->cConv(rc);
				if (!rootDrawer->is3D()) c.z = 0;

				String str = mp->sValue(rc,0);
				char* s = str.sVal();

				if ((long)colBack != -2)
				{
					glColor4f(cBack.redP(), cBack.greenP(), cBack.blueP(), 1);
					CoordBounds cbText = font->getTextExtent(s);

					c.x -= cbText.width()/2.0;
					c.y += cbText.height()/2.0;

					cbText.cMin += c;
					cbText.cMax += c;
					cbText.cMin.y -= textDrawer->getHeight();
					cbText.cMax.y -= textDrawer->getHeight();
					cbText *= 1.1;
					double heightIncrease = cbText.height() / 8.0;

					cbText.cMax.y += heightIncrease;
					cbText.cMin.y -= heightIncrease;

					glBegin(GL_QUADS);
					glColor3f(1.0f, 1.0f, 1.0f); // white
					glVertex3f(cbText.cMin.x, cbText.cMin.y, 0);
					glVertex3f(cbText.cMax.x, cbText.cMin.y, 0);
					glVertex3f(cbText.cMax.x, cbText.cMax.y, 0);
					glVertex3f(cbText.cMin.x, cbText.cMax.y, 0);
					glEnd();
				}
				textDrawer->setText(c,s);
				textDrawer->draw(ILWIS::NewDrawer::drl2D);
			}
		}
	}

	delete textLayerDrawer;
	return 0;
}

void PixelEditor::drawSelection()
{
	if (rcSelect.size()==0)
		return;
	for (int i=0; i<rcSelect.size();i++)
		drawSelectPix(rcSelect.at(i),false,true);
}

void PixelEditor::drawSelectPix(RowCol rc,bool inside,bool enable)
{
	if (rc.fUndef())
		return;
	double rsz = mp->gr()->rPixSize();
	Coord c = mp->gr()->cConv(rc);

	Coord org;
	org.x = c.x-rsz/2;
	org.y = c.y-rsz/2;

	//struct PLGCRD { Coord c1,c2,c3,c4;};
	PLGCRD p1,p2,p3,p4,p5,p6;

	// polygon 1, bottom
	p1.c1.x = org.x;
	p1.c1.y = org.y;

	p1.c2.x = org.x+rsz;
	p1.c2.y = org.y;

	p1.c3.x = p1.c2.x;
	p1.c3.y = org.y+rsz/4;

	p1.c4.x = p1.c1.x;
	p1.c4.y = p1.c3.y;

	// polygon 2, left
	p2.c1 = p1.c4;
	p2.c2.x = p2.c1.x+rsz/4;
	p2.c2.y = p2.c1.y;

	p2.c3.x = p2.c2.x;
	p2.c3.y = p2.c2.y+2*rsz/4;

	p2.c4.x = p2.c1.x;
	p2.c4.y = p2.c3.y;

	// polygon 3, top

	p3.c1 = p2.c4;

	p3.c2.x = p3.c1.x+rsz;
	p3.c2.y = p3.c1.y;

	p3.c3.x = p3.c2.x;
	p3.c3.y = p3.c2.y+rsz/4;

	p3.c4.x = p3.c1.x;
	p3.c4.y = p3.c3.y;

	// polygon 4, right

	p4.c1.x = p3.c2.x-rsz/4;
	p4.c1.y = p3.c2.y;

	p4.c2.x = p4.c1.x + rsz/4;
	p4.c2.y = p4.c1.y;

	p4.c3.x = p4.c2.x;
	p4.c3.y = p4.c2.y-2*rsz/4;

	p4.c4.x = p4.c1.x;
	p4.c4.y = p4.c3.y;

	// polygon 5, inside polygon
	p5.c1 = p2.c2;
	p5.c2 = p2.c3;
	p5.c3 = p4.c1;
	p5.c4 = p4.c4;

	// polygon 5, outside polygon
	p6.c1 = p1.c1;
	p6.c2 = p1.c2;
	p6.c3 = p3.c3;
	p6.c4 = p3.c4;

	if (!inside)
	{
		drawPolygon(p1,false,Color(1.0f,1.0f,1.0f),enable);
		drawPolygon(p2,false,Color(1.0f,1.0f,1.0f),enable);
		drawPolygon(p3,false,Color(1.0f,1.0f,1.0f),enable);
		drawPolygon(p4,false,Color(1.0f,1.0f,1.0f),enable);

		// draw inside and outside box
		drawPolygon(p5,true,Color(),enable);
		drawPolygon(p6,true,Color(),enable);
	}
	else
	{
		Color clr;
		if (fRealValues)
		{
			double rVal = mp->rValue(rc);
			if (rVal != rUNDEF)
				clr = clrVal(rVal);
		}
		else
		{
			long iRaw = mp->iRaw(rc);
			if (iRaw != iUNDEF)
				clr = clrRaw(iRaw);
		}
		drawPolygon(p5,false,clr,enable);
	}
}

void PixelEditor::drawPolygon(PLGCRD pcrd,bool drawbox, Color insideclr, bool enable)
{
	if( !drawbox)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glColor4f(insideclr.red(), insideclr.green(), insideclr.blue(),enable);
		glRectf(pcrd.c1.x,pcrd.c1.y, pcrd.c3.x,pcrd.c3.y);
		glDisable(GL_BLEND);
		enable = false;
	}
	GLubyte halftone[] ={
		0x99, 0x99, 0x99, 0x99, 0x66, 0x66, 0x66, 0x66,
		0x99, 0x99, 0x99, 0x99, 0x66, 0x66, 0x66, 0x66,
		0x99, 0x99, 0x99, 0x99, 0x66, 0x66, 0x66, 0x66,
		0x99, 0x99, 0x99, 0x99, 0x66, 0x66, 0x66, 0x66,
		0x99, 0x99, 0x99, 0x99, 0x66, 0x66, 0x66, 0x66,
		0x99, 0x99, 0x99, 0x99, 0x66, 0x66, 0x66, 0x66,
		0x99, 0x99, 0x99, 0x99, 0x66, 0x66, 0x66, 0x66,
		0x99, 0x99, 0x99, 0x99, 0x66, 0x66, 0x66, 0x66,
		0x99, 0x99, 0x99, 0x99, 0x66, 0x66, 0x66, 0x66,
		0x99, 0x99, 0x99, 0x99, 0x66, 0x66, 0x66, 0x66,
		0x99, 0x99, 0x99, 0x99, 0x66, 0x66, 0x66, 0x66,
		0x99, 0x99, 0x99, 0x99, 0x66, 0x66, 0x66, 0x66,
		0x99, 0x99, 0x99, 0x99, 0x66, 0x66, 0x66, 0x66,
		0x99, 0x99, 0x99, 0x99, 0x66, 0x66, 0x66, 0x66,
		0x99, 0x99, 0x99, 0x99, 0x66, 0x66, 0x66, 0x66,
		0x99, 0x99, 0x99, 0x99, 0x66, 0x66, 0x66, 0x66};

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_POLYGON_STIPPLE);
		glColor4f(insideclr.red(), insideclr.green(), insideclr.blue(),enable);
		glPolygonStipple(halftone);
		glRectf(pcrd.c1.x,pcrd.c1.y, pcrd.c3.x,pcrd.c3.y);
		glDisable(GL_POLYGON_STIPPLE);		// Disable the POLYGON STIPPLE

	glBegin(GL_LINE_LOOP);                 // Each set of 4 vertices form a quad
	glColor4f(1.0f, 1.0f, 1.0f, enable);   // white
	glVertex3f(pcrd.c1.x,pcrd.c1.y,0);     // Define vertices in counter-clockwise (CCW) order
	glVertex3f(pcrd.c2.x,pcrd.c2.y,0);     //  so that the normal (front-face) is facing you
	glVertex3f(pcrd.c3.x,pcrd.c3.y,0);
	glVertex3f(pcrd.c4.x,pcrd.c4.y,0);
	glEnd();
}