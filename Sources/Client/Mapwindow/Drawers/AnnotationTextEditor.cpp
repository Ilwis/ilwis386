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
// AnnotationTextEditor.cpp: implementation of the AnnotationTextEditor class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Engine\Map\Segment\Seg.h"
#include "engine\map\polygon\POL.H"
#include "Client\Mapwindow\MapPaneView.h"
#include "Engine\Map\Segment\Seg.h"
#include "Engine\Map\Polygon\POL.H"
#include "Engine\Map\Point\PNT.H"
#include "Client\Mapwindow\Positioner.h"
#include "Client\Mapwindow\Drawers\BaseDrawer.h"
#include "Client\Mapwindow\Drawers\Drawer.h"
#include "Client\Editors\Editor.h"
#include "Client\Editors\Utils\SYMBOL.H"
#include "Engine\Map\txtann.h"
#include "Client\Base\ButtonBar.h"
#include "Client\Mapwindow\Drawers\AnnotationTextEditor.h"
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
#include "Client\FormElements\fldfontn.h"
#include "Client\FormElements\fldjust.h"
#include "Client\ilwis.h"
#include "Headers\Htp\Ilwis.htp"
#include "Client\Mapwindow\MapWindow.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(AnnotationTextEditor, Editor)
	//{{AFX_MSG_MAP(AnnotationTextEditor)
	ON_COMMAND(ID_ANNTXTOPTIONS, OnOptions)
	ON_COMMAND(ID_COPY, OnCopy)
	ON_COMMAND(ID_PASTE, OnPaste)
	ON_COMMAND(ID_CUT, OnCut)
	ON_COMMAND(ID_CLEAR, OnClear)
	ON_COMMAND(ID_EDIT, OnEdit)
  ON_UPDATE_COMMAND_UI(ID_COPY, OnUpdateCopy)
  ON_UPDATE_COMMAND_UI(ID_CLEAR, OnUpdateCopy)
  ON_UPDATE_COMMAND_UI(ID_CUT, OnUpdateCopy)
  ON_UPDATE_COMMAND_UI(ID_PASTE, OnUpdatePaste)
	ON_COMMAND(ID_ADDTEXT, OnAddText)
	ON_COMMAND(ID_ANNTXTFONT,OnChangeFont)
	ON_COMMAND(ID_ANNTXTSIZE,OnChangeSize)
	ON_COMMAND(ID_ANNTXTBOLD,OnChangeBold)
	ON_COMMAND(ID_ANNTXTITALIC,OnChangeItalic)
	ON_COMMAND(ID_ANNTXTUNDERLINE,OnChangeUnderline)
	ON_COMMAND(ID_ANNTXTCOLOR,OnChangeColor)
	ON_COMMAND(ID_ANNTXTJUST,OnChangeJust)
	ON_COMMAND(ID_ANNTXTTRANSPARENT,OnChangeTransparent)
	ON_COMMAND(ID_ANNTXTROTATION,OnChangeRotation)
	ON_COMMAND(ID_SELALL,OnSelectAll)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#define sMen(ID) ILWSF("men",ID).scVal()
#define addmen(ID) men.AppendMenu(MF_STRING, ID, sMen(ID)); 
#define addSub(ID) menSub.AppendMenu(MF_STRING, ID, sMen(ID)); 
#define addSubMenu(ID) men.AppendMenu(MF_POPUP, (UINT)menSub.GetSafeHmenu(), sMen(ID)); menSub.Detach();

AnnotationTextEditor::AnnotationTextEditor(MapPaneView* mpvw, const AnnotationText& anntxt)
: Editor(mpvw),  atx(anntxt),
	pPoly(0)//,	aSelect(0,1)
{
	iFmtPnt = RegisterClipboardFormat("IlwisPoints");
	iFmtDom = RegisterClipboardFormat("IlwisDomain");
	curActive = zCursor("EditCursor");
//  aSelect.Resize(atx->iSize(),1);
  aSelect.Resize(atx->iSize()); // zero based now
  for (long i = 1; i <= atx->iSize(); ++i)
    aSelect[i-1] = false;

	MapCompositionDoc* mcd = mpv->GetDocument();
  for (list<Drawer*>::iterator iter = mcd->dl.begin(); iter != mcd->dl.end(); ++iter) 
	{
		Drawer* dr = *iter;
		IlwisObject obj = dr->obj();
		if (obj.fValid() && obj == atx) {
			 drw = dr;
			 break;
		}
	}
	if (0 == drw) { 
		drw = mcd->drAppend(atx);
		mcd->UpdateAllViews(mpv,2);
	}

  mode = modeSELECT;
  smb.smb = smbPlus;
  String fn = IlwWinApp()->Context()->fnUserINI().sFullName();
	smb.iSize = GetPrivateProfileInt("AnnotationText Editor", "Symbol Size", smb.iSize, fn.scVal());

  CMenu men;
	men.CreateMenu();
  addmen(ID_ANNTXTOPTIONS);
	addmen(ID_EXITEDITOR);
	hmenFile = men.GetSafeHmenu();
	men.Detach();

	men.CreateMenu();
  addmen(ID_CUT  );
  addmen(ID_COPY );
  addmen(ID_PASTE);
  addmen(ID_CLEAR);
  men.AppendMenu(MF_SEPARATOR);
  addmen(ID_SELALL);
  addmen(ID_ADDTEXT);
  addmen(ID_EDIT);
		CMenu menSub;
	  menSub.CreateMenu();
		addSub(ID_ANNTXTFONT);
		addSub(ID_ANNTXTSIZE);
		addSub(ID_ANNTXTBOLD);
		addSub(ID_ANNTXTITALIC);
		addSub(ID_ANNTXTUNDERLINE);
		addSub(ID_ANNTXTCOLOR);
		addSub(ID_ANNTXTJUST);
		addSub(ID_ANNTXTTRANSPARENT);
		addSub(ID_ANNTXTROTATION);
	addSubMenu(ID_ANNTXTCHANGE);
	hmenEdit = men.GetSafeHmenu();
	men.Detach();
	UpdateMenu();

	DataWindow* dw = mpv->dwParent();
	if (dw) {
		dw->bbDataWindow.LoadButtons("atxedit.but");
		dw->RecalcLayout();
	}
  htpTopic = htpAnnotationTextEditor;
	sHelpKeywords = "Annotation Text editor";
}

AnnotationTextEditor::~AnnotationTextEditor()
{
}

bool AnnotationTextEditor::OnContextMenu(CWnd* pWnd, CPoint point)
{
  CMenu men, menSub;
	men.CreatePopupMenu();
	addmen(ID_NORMAL);
	addmen(ID_ZOOMIN);
	addmen(ID_ZOOMOUT);
	addmen(ID_PANAREA);
  men.AppendMenu(MF_SEPARATOR);
  addmen(ID_EDIT);
  addmen(ID_ADDTEXT);
	  menSub.CreateMenu();
		addSub(ID_ANNTXTFONT);
		addSub(ID_ANNTXTSIZE);
		addSub(ID_ANNTXTBOLD);
		addSub(ID_ANNTXTITALIC);
		addSub(ID_ANNTXTUNDERLINE);
		addSub(ID_ANNTXTCOLOR);
		addSub(ID_ANNTXTJUST);
		addSub(ID_ANNTXTTRANSPARENT);
		addSub(ID_ANNTXTROTATION);
	addSubMenu(ID_ANNTXTCHANGE);
  men.AppendMenu(MF_SEPARATOR);
  addmen(ID_CUT  );
  addmen(ID_COPY );
  addmen(ID_PASTE);
  addmen(ID_CLEAR);
  men.AppendMenu(MF_SEPARATOR);
  addmen(ID_ANNTXTOPTIONS);
  addmen(ID_EXITEDITOR);
  menSub.CreateMenu();
  men.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, point.x, point.y, pWnd);
	return true;
}


IlwisObject AnnotationTextEditor::obj() const
{
	return atx;
}

zIcon AnnotationTextEditor::icon() const
{
	return zIcon("AtxIcon");
}

String AnnotationTextEditor::sTitle() const
{
  String s(SEDTitleAnnTextEditor_s.scVal(), atx->sName());
	return s;
}

int AnnotationTextEditor::draw(CDC* cdc, zRect, Positioner* psn, volatile bool* fDrawStop)
{
	MapCompositionDoc* mcd = mpv->GetDocument();
  long iNr = atx->iSize();
  for (long i = 1; i <= iNr; ++i) {
//    if (mppn->fDrawCheck())
//      break;
		atx->FromRec(i);
		MinMax mm = psn->mmSize();
		zRect rect = psn->rectSize();
		double rScaleY = (double) mm.height() / rect.height(); // (rowcol per pixel)
		int iPixPerInchY = cdc->GetDeviceCaps(LOGPIXELSY);
		double rPixPerYmm = iPixPerInchY / 25.4;         // (pixel per mm)
		double rInvPixSize = rScaleY * 1000 * rPixPerYmm;      // (rowcol per m)
		double rRowColSize = 1;
	//  if (fUseXY) {
			rRowColSize = psn->georef()->rPixSize();             // (m per rowcol)
			if (rRowColSize <= 0)
				rRowColSize = 1;
	//  }
		double rSc = rInvPixSize * rRowColSize;                // (m per m)
		rSc /= atx->rWorkingScale;
		int iHeight = -round(10 * atx->rFontSize / rSc);
		if (abs(iHeight) < 5)
			return 0;
		int iWeight = atx->fBold ? FW_BOLD : FW_NORMAL;
		int iEscapement = 10 * atx->rRotation;
		int iOrientation = iEscapement; //fUpright ? 0 : iEscapement;
		CFont fnt;
		LOGFONT logFont;
		memset(&logFont, 0, sizeof(logFont));
		logFont.lfHeight = -iHeight;
		logFont.lfWeight = iWeight;
		logFont.lfPitchAndFamily = VARIABLE_PITCH;
		logFont.lfItalic = atx->fItalic;
		logFont.lfUnderline = atx->fUnderline;
		logFont.lfEscapement = iEscapement;
		logFont.lfOrientation = iOrientation;
		lstrcpy(logFont.lfFaceName, atx->sFontName.scVal());
		fnt.CreatePointFontIndirect(&logFont, cdc);
		CFont* fntOld = cdc->SelectObject(&fnt);

		if (aSelect[i-1]) {
			cdc->SetBkMode(OPAQUE);
			Color c = atx->color;
			Color cHL = SysColor(COLOR_HIGHLIGHTTEXT);
			c.red()   /= 2;
			c.green() /= 2;
			c.blue()  /= 2;
			c.red()   += cHL.red() / 2;
			c.green() += cHL.green() / 2;
			c.blue()  += cHL.blue() / 2;
			cdc->SetTextColor(c);
			c = SysColor(COLOR_HIGHLIGHT);
			cdc->SetBkColor(c);
		}
		else {
			if (atx->fTransparent)
				cdc->SetBkMode(TRANSPARENT);
			else {
				cdc->SetBkMode(OPAQUE);
				cdc->SetBkColor(mcd->colBackground);
			}
			cdc->SetTextColor(atx->color);
		}
		int iAlign = 0;
		switch (atx->iJustification) {
			case 1: iAlign = TA_LEFT   | TA_BOTTOM;   break;
			case 2: iAlign = TA_LEFT   | TA_BASELINE; break;
			case 3: iAlign = TA_LEFT   | TA_TOP;      break;
			case 4: iAlign = TA_CENTER | TA_BOTTOM;   break;
			case 5: iAlign = TA_CENTER | TA_BASELINE; break;
			case 6: iAlign = TA_CENTER | TA_TOP;      break;
			case 7: iAlign = TA_RIGHT  | TA_BOTTOM;   break;
			case 8: iAlign = TA_RIGHT  | TA_BASELINE; break;
			case 9: iAlign = TA_RIGHT  | TA_TOP;      break;
		}
		cdc->SetTextAlign(iAlign);
		zPoint pnt;
		if (atx->fUseXY) {
			Coord c;
			c.x = atx->colX->rValue(i);
			c.y = atx->colY->rValue(i);
			pnt = psn->pntPos(c);
		}
		else {
			double rRow = atx->colRow->rValue(i);
			double rCol = atx->colCol->rValue(i);
			pnt = psn->pntPos(rRow, rCol);
		}
		String sText = atx->colText->sValue(i,0);
		cdc->TextOut(pnt.x, pnt.y, sText.scVal());
		cdc->SelectObject(fntOld);

	  smb.col = atx->color;
		smb.drawSmb(cdc, 0, pnt);
  }
	return 0;
}


class _export AnnTextEditGeneralForm: public FormWithDest
{
protected:
  AnnTextEditGeneralForm(CWnd* wPar, const String& sTitle,
                 AnnotationTextEditor* ated, AnnotationText& at)
  : FormWithDest(wPar, sTitle), atx(at), ate(ated)
  {
  }
  void ShowGeneralButton() {
    bool fShow = !atx->colFontName.fValid() ||
                 !atx->colFontSize.fValid() ||
                 !atx->colFontBold.fValid() ||
                 !atx->colFontItalic.fValid() ||
                 !atx->colFontUnderline.fValid() ||
                 !atx->colJustification.fValid() ||
                 !atx->colColor.fValid() ||
                 !atx->colTransparent.fValid() ||
                 !atx->colRotation.fValid();
    if (fShow) {
      PushButton* pb = new PushButton(root, SEDUiGeneral,
              (NotifyProc)&AnnTextEditGeneralForm::ShowGeneralForm);
      pb->SetIndependentPos();
    }
  }
  void ShowGeneral(bool fColExist);
  int exec() {
    FormWithDest::exec();
    atx->Updated();
    return 1;
  }
  AnnotationText atx;
  AnnotationTextEditor* ate;
private:
  int ShowGeneralForm(Event*);
};

void AnnTextEditGeneralForm::ShowGeneral(bool fColExist)
{
  if (fColExist == atx->colFontName.fValid())
    new FieldFontName(root, SEDUiFontName, &atx->sFontName);
  if (fColExist == atx->colFontSize.fValid())
    new FieldReal(root, SEDUiFontSize, &atx->rFontSize, ValueRange(0.1,10000,0.1));
  if (fColExist == atx->colFontBold.fValid())
    new CheckBox(root, SEDUiBold, &atx->fBold);
  if (fColExist == atx->colFontItalic.fValid())
    new CheckBox(root, SEDUiItalic, &atx->fItalic);
  if (fColExist == atx->colFontUnderline.fValid())
    new CheckBox(root, SEDUiUnderline, &atx->fUnderline);
  if (fColExist == atx->colJustification.fValid())
    new FieldJustification(root, SEDUiJustification, &atx->iJustification);
  if (fColExist == atx->colColor.fValid())
    new FieldColor(root, SEDUiTextColor, &atx->color);
  if (fColExist == atx->colTransparent.fValid())
    new CheckBox(root, SEDUiTransparent, &atx->fTransparent);
  if (fColExist == atx->colRotation.fValid())
    new FieldReal(root, SEDUiRotation, &atx->rRotation, ValueRange(-360,360,0.1));
}

class AnnTextEditGenForm: public AnnTextEditGeneralForm
{
public:
  AnnTextEditGenForm(CWnd* wPar,
           AnnotationTextEditor* ated, AnnotationText& at)
    : AnnTextEditGeneralForm(wPar, SEDTitleGenTextEdit, ated, at)
  {
    ShowGeneral(false);
    create();
  }
  int exec() {
    for (long i = 1; i < atx->iSize(); ++i)
      ate->setDirty(i);
    AnnTextEditGeneralForm::exec();
    for (long i = 1; i < atx->iSize(); ++i)
      ate->setDirty(i);
    return 1;
  }
};

int AnnTextEditGeneralForm::ShowGeneralForm(Event*)
{
  AnnTextEditGenForm frm(this, ate, atx);
  return 1;
}

class AnnTextEditSingleForm: public AnnTextEditGeneralForm
{
public:
  AnnTextEditSingleForm(CWnd* wPar, AnnotationTextEditor* ated,
       AnnotationText& at, long iNr)
    : AnnTextEditGeneralForm(wPar, SEDTitleSingleTextEdit, ated, at),
      iText(iNr)
  {
    sText = atx->colText->sValue(iText,0);
    fs = new FieldString(root, "", &sText, Domain(), false);
    fs->SetWidth(120);
    fs->SetIndependentPos();
    atx->FromRec(iText);
    if (atx->fUseXY) {
      crd.x = atx->colX->rValue(iText);
      crd.y = atx->colY->rValue(iText);
      new FieldCoord(root, SEDUiXY, &crd);
    }
    else {
      crd.x = atx->colRow->rValue(iText);
      crd.y = atx->colCol->rValue(iText);
      new FieldCoord(root, SEDUiRowCol, &crd);
    }
    ShowGeneral(true);
    ShowGeneralButton();
    SetMenHelpTopic(htpAnnTextEditSingle);
    create();
  }
  FormEntry* feDefaultFocus() {
    return fs;
  }
  int exec() {
    ate->setDirty(iText);
    AnnTextEditGeneralForm::exec();
    if (atx->fUseXY) {
      atx->colX->PutVal(iText, crd.x);
      atx->colY->PutVal(iText, crd.y);
    }
    else {
      atx->colRow->PutVal(iText, crd.x);
      atx->colCol->PutVal(iText, crd.y);
    }
    atx->colText->PutVal(iText, sText);
    atx->FillRec(iText);
    atx->Updated();
    ate->setDirty(iText);
    return 0;
  }
private:
  FieldString* fs;
  long iText;
  Coord crd;
  String sText;
};

class AnnTextEditMultipleForm: public AnnTextEditGeneralForm
{
public:
  AnnTextEditMultipleForm(CWnd* wPar,
     AnnotationTextEditor* ated, AnnotationText& at, long iSel)
    : AnnTextEditGeneralForm(wPar, SEDTitleMultipleTextEdit, ated, at)
  {
    String s(SEDRemSelTxt_i.scVal(), iSel);
    StaticText* st = new StaticText(root, s);
    st->SetIndependentPos();
    ShowGeneral(true);
    ShowGeneralButton();
    SetMenHelpTopic(htpAnnTextEditMultiple);
    create();
  }
};

class AnnTextGeneralForm: public FormWithDest
{
public:
  AnnTextGeneralForm(CWnd* wPar, AnnotationText& at, const GeoRef& gr)
    : FormWithDest(wPar, SEDTitleOptTextEdit), atx(at), grf(gr)
  {
    rScale = atx->rWorkingScale;
    new FieldReal(root, SEDUiWorkingScale, &rScale, ValueRange(1,1e12,1));

    iXyRowcol = atx->fUseXY ? 0 : 1;
    RadioGroup* rg = new RadioGroup(root, "", &iXyRowcol);
    rg->SetIndependentPos();
    new RadioButton(rg, SEDUiUseXY);
    new RadioButton(rg, SEDUiUseRowCol);

    fFontName = atx->colFontName.fValid();
    fFontSize = atx->colFontSize.fValid();
    fBold = atx->colFontBold.fValid();
    fItalic = atx->colFontItalic.fValid();
    fUnderline = atx->colFontUnderline.fValid();
    fJustification = atx->colJustification.fValid();
    fColor = atx->colColor.fValid();
    fTransparent = atx->colTransparent.fValid();
    fRotation = atx->colRotation.fValid();
    if (!fFontName || !fFontSize ||
        !fBold || !fItalic || !fUnderline ||
        !fColor || !fJustification ||
        !fTransparent || !fRotation)
    {
      StaticText* st = new StaticText(root, SEDRemAttrAskPerText);
      st->SetIndependentPos();
      if (!fFontName)
        new CheckBox(root, SEDUiFontName, &fFontName);
      if (!fFontSize)
        new CheckBox(root, SEDUiFontSize, &fFontSize);
      if (!fBold)
        new CheckBox(root, SEDUiBold, &fBold);
      if (!fItalic)
        new CheckBox(root, SEDUiItalic, &fItalic);
      if (!fUnderline)
        new CheckBox(root, SEDUiUnderline, &fUnderline);
      if (!fColor)
        new CheckBox(root, SEDUiColor, &fColor);
      if (!fJustification)
        new CheckBox(root, SEDUiJustification, &fJustification);
      if (!fTransparent)
        new CheckBox(root, SEDUiTransparent, &fTransparent);
      if (!fRotation)
        new CheckBox(root, SEDUiRotation, &fRotation);
    }
    SetMenHelpTopic(htpAnnTextEditOptions);
    create();
  }
  int exec() {
    FormWithDest::exec();
    if (abs(rScale - atx->rWorkingScale) > 1) {
			double rFact = atx->rWorkingScale / rScale;
      switch (MessageBox(SEDMsgUpdateFontSizes.scVal(), SEDMsgAnnTextEditor.scVal(),
                   MB_YESNO|MB_ICONQUESTION))
			{
				case IDYES:
				{
					if (atx->colFontSize.fValid())
						for (int i = 1; i < atx->iSize(); ++i) {
							double rSize = atx->colFontSize->rValue(i);
							rSize *= rFact;
							atx->colFontSize->PutVal(i, rSize);
						}
				} break;
				default:
          atx->rFontSize *= rFact;
      }
      atx->rWorkingScale = rScale;
    }

    if (atx->fUseXY && 1 == iXyRowcol)
      atx->UseRowCol(grf);
    else if (!atx->fUseXY && 0 == iXyRowcol)
      atx->UseXY(grf);

    if (fFontName != atx->colFontName.fValid())
      if (fFontName)
        atx->CreateColumnFontName();
      else {
      }
    if (fFontSize != atx->colFontSize.fValid())
      if (fFontSize)
        atx->CreateColumnFontSize();
      else {
      }
    if (fBold != atx->colFontBold.fValid())
      if (fBold)
        atx->CreateColumnFontBold();
      else {
      }
    if (fItalic != atx->colFontItalic.fValid())
      if (fItalic)
        atx->CreateColumnFontItalic();
      else {
      }
    if (fUnderline != atx->colFontUnderline.fValid())
      if (fUnderline)
        atx->CreateColumnFontUnderline();
      else {
      }
    if (fJustification != atx->colJustification.fValid())
      if (fJustification)
        atx->CreateColumnJustification();
      else {
      }
    if (fColor != atx->colColor.fValid())
      if (fColor)
        atx->CreateColumnColor();
      else {
      }
    if (fTransparent != atx->colTransparent.fValid())
      if (fTransparent)
        atx->CreateColumnTransparent();
      else {
      }
    if (fRotation != atx->colRotation.fValid())
      if (fRotation)
        atx->CreateColumnRotation();
      else {
      }
    atx->Updated();
    return 1;
  }
private:
  AnnotationText atx;
  GeoRef grf;
  bool fFontName, fFontSize, fBold, fItalic, fColor,
       fUnderline, fJustification, fTransparent, fRotation;
  double rScale;
  int iXyRowcol;
};

void AnnotationTextEditor::OnOptions()
{
	MapCompositionDoc* mcd = mpv->GetDocument();
  AnnTextGeneralForm frm(mpv, atx, mcd->georef);
}

int AnnotationTextEditor::Edit(const Coord& crd)
{
  long iNr = 0;
  long iSel = 0;
  for (long i = 1; i <= aSelect.iSize(); ++i)
    if (aSelect[i-1]) {
      iSel += 1;
      if (iNr == 0) 
        iNr = i;  
    }  
  if (iSel == 0) {
    bool fShow = !atx->colFontName.fValid() ||
                 !atx->colFontSize.fValid() ||
                 !atx->colFontBold.fValid() ||
                 !atx->colFontItalic.fValid() ||
                 !atx->colFontUnderline.fValid() ||
                 !atx->colJustification.fValid() ||
                 !atx->colColor.fValid() ||
                 !atx->colTransparent.fValid() ||
                 !atx->colRotation.fValid();
    if (fShow)
      AnnTextEditGenForm frm(mpv, this, atx);
    else
      MessageBeep(MB_ICONASTERISK);
    return 1;
  }
  else if (iSel == 1) {
    AnnTextEditSingleForm frm(mpv, this, atx, iNr);
    if (!frm.fOkClicked())
      return 1;
  }
  else {
    atx->FromRec(iNr);
    AnnTextEditMultipleForm frm(mpv, this, atx, iSel);
    if (!frm.fOkClicked())
      return 1;
    for (long i = 1; i <= aSelect.iSize(); ++i)
      if (aSelect[i-1]) {
        atx->FillRec(i);
        setDirty(i);
      }
    atx->Updated();
  }
  return 1;
}

void AnnotationTextEditor::setDirty(long iNr)
{
  zPoint* p = p4Point(iNr);
	CRgn rgn;
	rgn.CreatePolygonRgn(p, 4, ALTERNATE);
  mpv->InvalidateRgn(&rgn);
  delete [] p;
}

zPoint AnnotationTextEditor::pPoint(long iNr)
{
  zPoint pnt;
  if (atx->fUseXY) {
    Coord c;
    c.x = atx->colX->rValue(iNr);
    c.y = atx->colY->rValue(iNr);
    pnt = mpv->pntPos(c);
  }
  else {
    double rRow = atx->colRow->rValue(iNr);
    double rCol = atx->colCol->rValue(iNr);
    pnt = mpv->pntPos(rRow, rCol);
  }
  return pnt;
}

zPoint* AnnotationTextEditor::p4Point(long iNr)
{
  zPoint pnt = pPoint(iNr);
	CClientDC cdc(mpv);
  atx->FromRec(iNr);
  int iHeight = round(10 * atx->rFontSize / rSc);
  int iWeight = atx->fBold ? FW_BOLD : FW_NORMAL;
  int iStyle = 0;
  int iEscapement = 10 * atx->rRotation;
  int iOrientation = iEscapement; //atx->fUpright ? 0 : iEscapement;

	CFont fnt;
	LOGFONT logFont;
	memset(&logFont, 0, sizeof(logFont));
	logFont.lfHeight = -iHeight;
	logFont.lfWeight = iWeight;
	logFont.lfPitchAndFamily = VARIABLE_PITCH;
	logFont.lfItalic = atx->fItalic;
	logFont.lfUnderline = atx->fUnderline;
	logFont.lfEscapement = iEscapement;
	logFont.lfOrientation = iOrientation;
	lstrcpy(logFont.lfFaceName, atx->sFontName.scVal());
	fnt.CreatePointFontIndirect(&logFont, &cdc);
	CFont* fntOld = cdc.SelectObject(&fnt);
  String sText = atx->colText->sValue(iNr,0);
  CSize sz = cdc.GetTextExtent(sText.scVal(), sText.length());
  double rW = sz.cx + 1;
//  double rH = HIWORD(dw);
  double rRotRad = atx->rRotation * M_PI / 180.0;
  double rFactX = cos(rRotRad);
  double rFactY = -sin(rRotRad);
//  zPoint pL = pnt;
//  zPoint pR = pnt;
  double rFactL = 0;
  switch (atx->iJustification) {
    case 1:
    case 2:
    case 3:
      rFactL = 0;
      break;
    case 4:
    case 5:
    case 6:
      rFactL = 0.5;
      break;
    case 7:
    case 8:
    case 9:
      rFactL = 1;
      break;
  }
  double rFactR = 1 - rFactL;
//  pL.x() -= round(rFactL * rFactX * rW);
//  pL.y() -= round(rFactL * rFactY * rW);
//  pR.x() += round(rFactR * rFactX * rW);
//  pR.y() += round(rFactR * rFactY * rW);

  TEXTMETRIC tm;
  cdc.GetTextMetrics(&tm);
	cdc.SelectObject(fntOld);
  double rFactU, rFactD;
  switch (atx->iJustification) {
    case 1:
    case 4:
    case 7: // bottom
      rFactD = 0;
      rFactU = tm.tmHeight + 1;
      break;
    case 2:
    case 5:
    case 8: // base
      rFactD = tm.tmDescent + 1;
      rFactU = tm.tmAscent + 1;
      break;
    case 3:
    case 6:
    case 9: // top
      rFactD = tm.tmHeight + 1;
      rFactU = 0;
      break;
  }
  zPoint pLU = pnt, pLD = pnt, pRU = pnt, pRD = pnt;

  pLU.x += round(-rFactL * rFactX * rW + rFactU * rFactY);
  pLU.y += round(-rFactL * rFactY * rW - rFactU * rFactX);
  pLD.x += round(-rFactL * rFactX * rW - rFactD * rFactY);
  pLD.y += round(-rFactL * rFactY * rW + rFactD * rFactX);
  pRU.x += round( rFactR * rFactX * rW + rFactU * rFactY);
  pRU.y += round( rFactR * rFactY * rW - rFactU * rFactX);
  pRD.x += round( rFactR * rFactX * rW - rFactD * rFactY);
  pRD.y += round( rFactR * rFactY * rW + rFactD * rFactX);

	// one more line down
	pLD.y += 1;
	pRD.y += 1;

  zPoint* p = new zPoint[4];
  p[0] = pLU;
  p[1] = pLD;
  p[2] = pRD;
  p[3] = pRU;

/*
  // prevent rounding errors:
  for (int i = 0; i < 4; ++i) {
    if (p[i].x() < pnt.x())
      p[i].x() -= 1;
    else
      p[i].x() += 1;
    if (p[i].y() < pnt.y())
      p[i].y() -= 1;
    else
      p[i].y() += 1;
  }
*/
  return p;
}

bool AnnotationTextEditor::fInside(long iPoint, zPoint pnt)
{
  zPoint* p = p4Point(iPoint);
	CRgn rgn;
	rgn.CreatePolygonRgn(p, 4, ALTERNATE);
  delete [] p;
  return rgn.PtInRegion(pnt)?true:false;
}

long AnnotationTextEditor::iPoint(zPoint pnt)
{
  long iPoint = iUNDEF;
  if (atx->fUseXY) {
    double rPointD2 = 1e100;
    for (long i = 1; i <= atx->iSize(); ++i) {
      Coord crd = mpv->crdPnt(pnt);
      double rX = atx->colX->rValue(i);
      double rY = atx->colY->rValue(i);
      Coord c(rX, rY);
      double rD2 = rDist2(crd, c);
      if (rD2 < rPointD2 && fInside(i, pnt)) {
        iPoint = i;
        rPointD2 = rD2;
      }
    }
  }
  else {
    double rRow, rCol;
    mpv->Pnt2RowCol(pnt, rRow, rCol);
    double rPointD2 = 1e100;
    for (long i = 1; i <= atx->iSize(); ++i) {
      double rDRow = rRow - atx->colRow->rValue(i);
      double rDCol = rCol - atx->colCol->rValue(i);
      double rD2 = rDRow * rDRow + rDCol * rDCol;
      if (rD2 < rPointD2 && fInside(i, pnt)) {
        iPoint = i;
        rPointD2 = rD2;
      }
    }
  }
  return iPoint;
}

void AnnotationTextEditor::drawActPoint()
{
  zPoint p[5];
  zPoint pShift = pCurr - pAct;
	CClientDC cdc(mpv);
	CGdiObject* penOld = cdc.SelectStockObject(WHITE_PEN);
  cdc.SetROP2(R2_XORPEN);
  for (int i = 0; i < iActNr; ++i) {
    for (int j = 0; j < 4; ++j)
      p[j] = pPoly[4*i+j] + pShift;
    p[4] = p[0];
    cdc.Polyline(p, 5);
  }
	cdc.SelectObject(penOld);
	cdc.SetROP2(R2_COPYPEN);
}

/*
void AnnotationTextEditor::drawPoint(Positioner* psn, long i)
{
	CClientDC cdc(mpv);
  atx->FromRec(abs(i));
  bool fTransparent = atx->fTransparent;
  if (i < 0) {
    fTransparent = false;
    i = abs(i);
  }
  int iHeight = -round(10 * atx->rFontSize / rSc);
  if (abs(iHeight) < 5)
    return;
  int iWeight = atx->fBold ? FW_BOLD : FW_NORMAL;
  int iStyle = 0;
  int iEscapement = 10 * atx->rRotation;
  int iOrientation = iEscapement; //atx->fUpright ? 0 : iEscapement;
	CFont fnt;
	LOGFONT logFont;
	memset(&logFont, 0, sizeof(logFont));
	logFont.lfHeight = -iHeight;
	logFont.lfWeight = iWeight;
	logFont.lfPitchAndFamily = VARIABLE_PITCH;
	logFont.lfItalic = atx->fItalic;
	logFont.lfUnderline = atx->fUnderline;
	logFont.lfEscapement = iEscapement;
	logFont.lfOrientation = iOrientation;
	lstrcpy(logFont.lfFaceName, atx->sFontName.scVal());
	fnt.CreatePointFontIndirect(&logFont, &cdc);
	CFont* fntOld = cdc.SelectObject(&fnt);
  dsp->pushFont(new zFont(atx->sFontName,
                          zPrPoint(0, -iHeight, dsp), iWeight,
                          ffDontCare, VariablePitch, iStyle,
                          iEscapement, iOrientation));
  if (aSelect[i]) {
		cdc.SetBkMode(OPAQUE);
    Color c = atx->color;
    Color cHL = SysColor(COLOR_HIGHLIGHTTEXT);
    c.red()   /= 2;
    c.green() /= 2;
    c.blue()  /= 2;
    c.red()   += cHL.red() / 2;
    c.green() += cHL.green() / 2;
    c.blue()  += cHL.blue() / 2;
	  cdc.SetTextColor(c);
		cdc.SetBkColor(SysColor(COLOR_HIGHLIGHT));
  }
  else {
    if (fTransparent)
	    cdc.SetBkMode(TRANSPARENT);
    else {
	    cdc.SetBkMode(OPAQUE);
			MapCompositionDoc* mcd = mpv->GetDocument();
			cdc.SetBkColor(mcd->colBackground);
    }
	  cdc.SetTextColor(atx->color);
  }
  int iAlign = 0;
  switch (atx->iJustification) {
    case 1: iAlign = TA_LEFT   | TA_BOTTOM;   break;
    case 2: iAlign = TA_LEFT   | TA_BASELINE; break;
    case 3: iAlign = TA_LEFT   | TA_TOP;      break;
    case 4: iAlign = TA_CENTER | TA_BOTTOM;   break;
    case 5: iAlign = TA_CENTER | TA_BASELINE; break;
    case 6: iAlign = TA_CENTER | TA_TOP;      break;
    case 7: iAlign = TA_RIGHT  | TA_BOTTOM;   break;
    case 8: iAlign = TA_RIGHT  | TA_BASELINE; break;
    case 9: iAlign = TA_RIGHT  | TA_TOP;      break;
  }
  cdc.SetTextAlign(iAlign);
  zPoint pnt = pntPoint(psn,i);
  String sText = atx->colText->sValue(i,0);
  dsp->text(pnt, sText);
  smb.col = atx->color;
  smb.drawSmb(&cdc, 0, pnt);
	cdc.SelectObject(fntOld);
}
*/

bool AnnotationTextEditor::fCopyOk()
{
  long iSel = 0;
  for (long i = 1; i <= aSelect.iSize(); ++i)
    if (aSelect[i-1]) {
      iSel += 1;
    }
  return iSel > 0;
}

bool AnnotationTextEditor::fPasteOk()
{
  return IsClipboardFormatAvailable(iFmtPnt) ||
	       IsClipboardFormatAvailable(CF_TEXT)
				 ? true : false;
}


void AnnotationTextEditor::OnCut()
{
	OnCopy();
	OnClear();
}

void AnnotationTextEditor::OnCopy()
{
  if (!fCopyOk()) return;
	CWaitCursor curWait;
	if (!mpv->OpenClipboard())
		return;
	EmptyClipboard();
	const int iSIZE = 1000000;
	char* sBuf = new char[iSIZE];
  char* s = sBuf;
  char* sMax = &sBuf[iSIZE-1];
  for (long i = 1; i <= aSelect.iSize(); ++i) {
    if (aSelect[i-1]) {
      String str = atx->colText->sValue(i,0);
      strcpy(s, str.scVal());
      s += str.length();
      *s++ = '\r';
      *s++ = '\n';
      if (s > sMax)
        break;
    }
  }
  s -= 2;
  *s = '\0';
	HANDLE hnd = GlobalAlloc(GMEM_FIXED, strlen(sBuf)+2);
	char* pc = (char*)GlobalLock(hnd);
	strcpy(pc,sBuf);
	GlobalUnlock(hnd);
	SetClipboardData(CF_TEXT,hnd);
  delete sBuf;
}

void AnnotationTextEditor::OnPaste()
{
  if (!fPasteOk()) return;
	MapCompositionDoc* mcd = mpv->GetDocument();
  DeselectAll();
	CClientDC cdc(mpv);
  unsigned int iSize;

	if (!mpv->OpenClipboard())
		return;

  if (IsClipboardFormatAvailable(iFmtPnt)) {
    DomainValueRangeStruct dvrs;
    if (IsClipboardFormatAvailable(iFmtDom)) {

			HANDLE hnd = GetClipboardData(iFmtDom);
			iSize = (unsigned int)GlobalSize(hnd);
			IlwisDomain id;
			if (sizeof(id) < iSize)
				iSize = sizeof(id);
			memcpy(&id, (char*)GlobalLock(hnd),iSize);
			GlobalUnlock(hnd);
      Domain dmCb = id.dm();
      ValueRange vrCb = id.vr();
      dvrs = DomainValueRangeStruct(dmCb, vrCb);
    }
		HANDLE hnd = GetClipboardData(iFmtPnt);
		iSize = (unsigned int)GlobalSize(hnd);
		char* cp = new char[iSize];
		memcpy(cp, (char*)GlobalLock(hnd),iSize);
		IlwisPoint* ip = (IlwisPoint*) cp;
		iSize /= sizeof(IlwisPoint);
		iSize = ip[0].iRaw;
    for (int j = 0; j < iSize; ++j) {
      Coord crd = ip[1+j].c;
      if (crd.fUndef())
        continue;
      long iRaw = ip[1+j].iRaw;
      String sVal = dvrs.sValueByRaw(iRaw);
      long iPoint;
      if (atx->fUseXY)
        iPoint = atx->iAdd(crd, sVal);
      else {
        double rRow, rCol;
        mcd->georef->Coord2RowCol(crd, rRow, rCol);
        iPoint = atx->iAdd(rRow, rCol, sVal);
      }
      aSelect &= true;
      setDirty(iPoint);
    }
  }
  else {
    zPoint pnt;
    GetCursorPos((POINT*)&pnt);
    mpv->ScreenToClient((POINT*)&pnt);
    zRect rect;
    mpv->GetClientRect(rect);
    if (!rect.PtInRect(pnt)) {
      pnt.x = (rect.left() + rect.right()) / 2;
      pnt.y = (rect.top() + rect.bottom()) / 2;
    }
		HANDLE hnd = GetClipboardData(CF_TEXT);
		if (0 != hnd) {
			char* sText = new char[(unsigned int)GlobalSize(hnd)];
			strcpy(sText, (char*)GlobalLock(hnd));
			GlobalUnlock(hnd);
	    char* sTmp = strchr(sText,'\n');
		  if (sTmp)
			  *sTmp = 0;
			String sVal = sText;
			delete sText;
			long iPoint;
			if (atx->fUseXY) {
				Coord crd = mpv->crdPnt(pnt);
				iPoint = atx->iAdd(crd, sVal);
			}
			else {
				double rRow, rCol;
				mpv->Pnt2RowCol(pnt, rRow, rCol);
				iPoint = atx->iAdd(rRow, rCol, sVal);
			}
			aSelect &= true;
			setDirty(iPoint);
		}
//  drawPoint(&psn, iPoint);
  }
	CloseClipboard();
}

void AnnotationTextEditor::OnSelectAll()
{
  for (long i = 1; i <= aSelect.iSize(); ++i)
    if (!aSelect[i-1]) {
      aSelect[i-1] = true;
      setDirty(i);
    }
}

bool AnnotationTextEditor::DeselectAll()
{
  bool fChange = false;
  for (long i = 1; i <= aSelect.iSize(); ++i)
    if (aSelect[i-1]) {
      fChange = true;
      aSelect[i-1] = false;
      setDirty(i);
    }
  return fChange;
}

void AnnotationTextEditor::OnUpdateCopy(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(fCopyOk());
}

void AnnotationTextEditor::OnUpdatePaste(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(fPasteOk());
}

void AnnotationTextEditor::OnClear()
{
  long i;
  long iNr = 0;
  long iSel = 0;
  for (i = 1; i <= aSelect.iSize(); ++i)
    if (aSelect[i-1]) {
      iSel += 1;
      if (iNr == 0) 
        iNr = i;  
    }  
  if (iSel == 0)
    return;  
  String s;  
//    s = SEDMsgDelSelPnt;
  bool fDel = true;
  if (iSel > 1) {
    s = String(SEDMsgDelSelTxt_i.scVal(), iSel);
    fDel = IDYES == mpv->MessageBox(s.scVal(), SEDMsgAnnTextEditor.scVal(),
                 MB_YESNO|MB_ICONQUESTION);
  }
  bool fChange = false;
  for (i = aSelect.iSize(); i > 0; --i)
    if (aSelect[i-1]) {
      fChange = true;
      if (fDel)
        atx->Delete(i);
    }
  if (fChange) {
    aSelect.Reset();
//    aSelect.Resize(atx->iSize(),1);
    aSelect.Resize(atx->iSize()); // zero based
    for (i = 1; i <= aSelect.iSize(); ++i)
      aSelect[i-1] = false;
    mpv->Invalidate();
  }
}

bool AnnotationTextEditor::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
  bool fCtrl = GetKeyState(VK_CONTROL) & 0x8000 ? true : false;
  bool fShift = GetKeyState(VK_SHIFT) & 0x8000 ? true : false;
  switch (nChar) {
    case VK_ESCAPE:
      switch (mode) {
        case modeSELECT:
          DeselectAll();
          break;
        case modeMOVING:
          drawActPoint();
          mode = modeSELECT;
          break;
      }
      return true;
    case VK_INSERT:
      if (!fShift && !fCtrl) {
        OnAddText();
				return true;
      }
      break;
    case VK_DELETE:
      if (!fShift && !fCtrl) {
        OnClear();
	      return true;
      }
      break;
    case VK_RETURN:
      if (!fShift && !fCtrl) {
				Edit(Coord());
	      return true;
      }
      break;
    case 'F':
      if (fShift) {
        OnChangeFont();
        return true;
      }
      break;
    case 'S':
      if (fShift) {
        OnChangeSize();
        return true;
      }
      break;
    case 'B':
      if (fShift) {
        OnChangeBold();
        return true;
      }
      break;
    case 'I':
      if (fShift) {
        OnChangeItalic();
        return true;
      }
      break;
    case 'U':
      if (fShift) {
        OnChangeUnderline();
        return true;
      }
      break;
    case 'C':
      if (fShift) {
        OnChangeColor();
        return true;
      }
      break;
    case 'J':
      if (fShift) {
        OnChangeJust();
        return true;
      }
      break;
    case 'T':
      if (fShift) {
        OnChangeTransparent();
        return true;
      }
      break;
    case 'R':
      if (fShift) {
        OnChangeRotation();
        return true;
      }
      break;
  }
  return false;
}

void AnnotationTextEditor::OnAddText()
{
  zPoint pnt;
  GetCursorPos((POINT*)&pnt);
  mpv->ScreenToClient((POINT*)&pnt);
  zRect rect;
  mpv->GetClientRect(rect);
  if (!rect.PtInRect(pnt)) {
    pnt.x = (rect.left() + rect.right()) / 2;
    pnt.y = (rect.top() + rect.bottom()) / 2;
  }
  Coord crd = mpv->crdPnt(pnt);
  double rRow, rCol;
  mpv->Pnt2RowCol(pnt, rRow, rCol);
  AddText(crd, rRow, rCol);
}

void AnnotationTextEditor::AddText(Coord crd, double rRow, double rCol)
{
  long iText;
  if (atx->fUseXY)
    iText = atx->iAdd(crd, "");
  else
    iText = atx->iAdd(rRow, rCol, "");
  atx->FillRec(iText);
  AnnTextEditSingleForm frm(mpv, this, atx, iText);
  if (!frm.fOkClicked()) {
    setDirty(iText);
    atx->Delete(iText);
  }
  else {
    aSelect &= true;
    setDirty(iText);
  }
}

bool AnnotationTextEditor::OnMouseMove(UINT nFlags, CPoint point)
{
  if (modeMOVING == mode) {
    drawActPoint();
    pCurr = point;
    drawActPoint();
		return true;
  }
  return false;
}

bool AnnotationTextEditor::OnLButtonDblClk(UINT nFlags, CPoint point)
{
  bool fSel = false;
  for (long i = 1; i <= aSelect.iSize(); ++i)
    if (aSelect[i-1]) {
      fSel = true;
      break;
    }
	if (fSel) {
		Edit(Coord());
		return true;
	}
  else {
    Coord crd = mpv->crdPnt(point);
    double rRow, rCol;
    mpv->Pnt2RowCol(point, rRow, rCol);
    AddText(crd, rRow, rCol);
		return true;
  }
}

bool AnnotationTextEditor::OnLButtonDown(UINT nFlags, CPoint point)
{
  int i;
  fCtrl = nFlags & MK_CONTROL ? true:false;
  fShft = nFlags & MK_SHIFT ? true:false;
  if (modeMOVING == mode) {
    drawActPoint();
    mode = modeSELECT;
  }
  long iPnt = iPoint(point);
  if (DragDetect(*mpv,point)) {
    if (fShft) {
      MessageBeep(MB_ICONASTERISK);
      return 1;
    }
    iActNr = 0;
    for (i = 1; i <= atx->iSize(); ++i)
      if (aSelect[i-1])
        iActNr += 1;
    if (iActNr <= 1 && iUNDEF == iPnt && !fCtrl && !fShft) {
      DeselectAll();
			if (mpv->as)
				return true;
			mpv->as = new AreaSelector(mpv, this, (NotifyRectProc)&AnnotationTextEditor::SelectArea);
			mpv->as->OnLButtonDown(nFlags, point);
    }
    else {
      bool fSingle = !fCtrl && !fShft && iUNDEF != iPnt;
      if (fCtrl) {
        if (iActNr == 0) {
          if (iPnt) {
            aSelect[iPnt-1] = 1;
            setDirty(iPnt);
            iActNr = 1;
          }
          else {
            MessageBeep(MB_ICONASTERISK);
            return 1;
          }
        }
        int iSize = atx->iSize();
        for (i = 1; i <= iSize; ++i) {
          if (aSelect[i-1]) {
            aSelect[i-1] = false;
            atx->FromRec(i);
            String sText = atx->colText->sValue(i,0);
            if (atx->fUseXY) {
              Coord crd;
              crd.x = atx->colX->rValue(i);
              crd.y = atx->colY->rValue(i);
              atx->iAdd(crd, sText);
            }
            else {
              double rRow = atx->colRow->rValue(i);
              double rCol = atx->colCol->rValue(i);
              atx->iAdd(rRow, rCol, sText);
            }
            aSelect &= true;
          }
        }
      }
      if (fSingle) {
        DeselectAll();
        aSelect[iPnt-1] = 1;
        setDirty(iPnt);
        iActNr = 1;
      }
      else {
        iActNr = 0;
        for (int i = 1; i <= atx->iSize(); ++i)
          if (aSelect[i-1])
            iActNr += 1;
      }
      if (iActNr > 4000)
        iActNr = 0;
      if (0 == iActNr) {
        MessageBeep(MB_ICONASTERISK);
        return 1;
      }
//      iActNr = iPnt;
      mode = modeMOVING;
      if (pPoly)
        delete [] pPoly;
      pPoly = 0;
      pPoly = new zPoint[4*iActNr];

      int j = 0;
      for (i = 1; i <= atx->iSize(); ++i)
        if (aSelect[i-1]) {
          zPoint* p = p4Point(i);
          for (int k = 0; k < 4; ++k)
            pPoly[4*j+k] = p[k];
          delete [] p;
          j += 1;
        }
      pAct = pCurr = point;
      drawActPoint();
    }
    return 1;
  }
  else { // no drag
    if (fCtrl) {
      MessageBeep(MB_ICONASTERISK);
      return 1;
    }
    if (!fCtrl && !fShft)
      DeselectAll();
    if (iPnt == iUNDEF) {
      MessageBeep(MB_ICONASTERISK);
    }
    else {
      if (fCtrl)
        if (fShft)
          aSelect[iPnt-1] = 0;
        else
          aSelect[iPnt-1] = 1;
      else
        aSelect[iPnt-1] = aSelect[iPnt-1] ? 0 : 1;
      setDirty(iPnt);
    }
  }
  return 1;
}

bool AnnotationTextEditor::OnLButtonUp(UINT nFlags, CPoint point)
{
  if (modeMOVING == mode) {
    drawActPoint();
    zPoint pShift = pCurr - pAct;
    for (int i = 1; i <= atx->iSize(); ++i) {
      if (aSelect[i-1]) {
        setDirty(i);
        zPoint pnt = pPoint(i);
        pnt += pShift;
        if (atx->fUseXY) {
          Coord crd = mpv->crdPnt(pnt);
          atx->colX->PutVal(i, crd.x);
          atx->colY->PutVal(i, crd.y);
        }
        else {
          double rRow, rCol;
          mpv->Pnt2RowCol(pnt, rRow, rCol);
          atx->colRow->PutVal(i, rRow);
          atx->colCol->PutVal(i, rCol);
        }
        atx->Updated();
        setDirty(i);
      }
    }
    mode = modeSELECT;
  }
  if (pPoly) {
    delete [] pPoly;
    pPoly = 0;
  }
  return true;
}

int AnnotationTextEditor::SelectArea(zRect rect)
{
  for (long i = 1; i <= atx->iSize(); ++i) {
    zPoint p = pPoint(i);
    if (rect.PtInRect(p)) {
      if (fCtrl)
        if (fShft)
          aSelect[i-1] = 0;
        else
          aSelect[i-1] = 1;
        else
          aSelect[i-1] = aSelect[i-1] ? 0 : 1;
      setDirty(i);
    }
  }
	mpv->as->Stop();
  return 1;
}

void AnnotationTextEditor::OnChangeFont()
{
  class ChangeFontForm: public FormWithDest
  {
  public:
    ChangeFontForm(CWnd* wPar, String* sFontName)
      : FormWithDest(wPar, SEDMsgAnnTextEditor)
    {
      fe = new FieldFontName(root, SEDUiFontName, sFontName);
      create();
    }
    FormEntry* feDefaultFocus()
    {
      return fe;
    }
  private:
    FormEntry* fe;
  };
  if (atx->colFontName.fValid())
    for (long i = 1; i <= atx->iSize(); ++i)
      if (aSelect[i-1]) {
        atx->FromRec(i);
        break;
      }
  String sFontName = atx->sFontName;
  ChangeFontForm frm(mpv, &sFontName);
  if (frm.fOkClicked()) {
    atx->sFontName = sFontName;
    if (atx->colFontName.fValid()) {
      for (long i = 1; i <= atx->iSize(); ++i)
        if (aSelect[i-1]) {
          setDirty(i);
          atx->colFontName->PutVal(i, sFontName);
          setDirty(i);
        }
    }
    else
      mpv->Invalidate();
  }
}

void AnnotationTextEditor::OnChangeSize()
{
  class ChangeSizeForm: public FormWithDest
  {
  public:
    ChangeSizeForm(CWnd* wPar, double* rFontSize)
      : FormWithDest(wPar, SEDMsgAnnTextEditor)
    {
      fe = new FieldReal(root, SEDUiFontSize, rFontSize, ValueRange(0.1,10000,0.1));
      create();
    }
    FormEntry* feDefaultFocus()
    {
      return fe;
    }
  private:
    FormEntry* fe;
  };
  if (atx->colFontSize.fValid())
    for (long i = 1; i <= atx->iSize(); ++i)
      if (aSelect[i-1]) {
        atx->FromRec(i);
        break;
      }
  double rFontSize = atx->rFontSize;
  ChangeSizeForm frm(mpv, &rFontSize);
  if (frm.fOkClicked()) {
    atx->rFontSize = rFontSize;
    if (atx->colFontSize.fValid()) {
      for (long i = 1; i <= atx->iSize(); ++i)
        if (aSelect[i-1]) {
          setDirty(i);
          atx->colFontSize->PutVal(i, rFontSize);
          setDirty(i);
        }
    }
    else
      mpv->Invalidate();
    atx->Updated();
  }
}

void AnnotationTextEditor::OnChangeBold()
{
  atx->fBold = atx->fBold ? 0 : 1;
  if (atx->colFontBold.fValid()) {
    for (long i = 1; i <= atx->iSize(); ++i)
      if (aSelect[i-1]) {
        setDirty(i);
        bool fBold = atx->colFontBold->iValue(i) ? true:false;
        fBold = fBold ? 0 : 1;
        atx->colFontBold->PutVal(i, (long)fBold);
        setDirty(i);
      }
  }
  else
    mpv->Invalidate();
  atx->Updated();
}

void AnnotationTextEditor::OnChangeItalic()
{
  atx->fItalic = atx->fItalic ? 0 : 1;
  if (atx->colFontItalic.fValid()) {
    for (long i = 1; i <= atx->iSize(); ++i)
      if (aSelect[i-1]) {
        setDirty(i);
        bool fItalic = atx->colFontItalic->iValue(i) ? true:false;
        fItalic = fItalic ? 0 : 1;
        atx->colFontItalic->PutVal(i, (long)fItalic);
        setDirty(i);
      }
  }
  else
    mpv->Invalidate();
  atx->Updated();
}

void AnnotationTextEditor::OnChangeUnderline()
{
  atx->fUnderline = atx->fUnderline ? 0 : 1;
  if (atx->colFontUnderline.fValid()) {
    for (long i = 1; i <= atx->iSize(); ++i)
      if (aSelect[i-1]) {
        setDirty(i);
        bool fUnderline = atx->colFontUnderline->iValue(i) ? true:false;
        fUnderline = fUnderline ? 0 : 1;
        atx->colFontUnderline->PutVal(i, (long)fUnderline);
        setDirty(i);
      }
  }
  else
    mpv->Invalidate();
  atx->Updated();
}

void AnnotationTextEditor::OnChangeColor()
{
  class ChangeColorForm: public FormWithDest
  {
  public:
    ChangeColorForm(CWnd* wPar, Color* color)
      : FormWithDest(wPar, SEDMsgAnnTextEditor)
    {
      fe = new FieldColor(root, SEDUiTextColor, color);
      create();
    }
    FormEntry* feDefaultFocus()
    {
      return fe;
    }
  private:
    FormEntry* fe;
  };
  if (atx->colColor.fValid())
    for (long i = 1; i <= atx->iSize(); ++i)
      if (aSelect[i-1]) {
        atx->FromRec(i);
        break;
      }
  Color color = atx->color;
  ChangeColorForm frm(mpv, &color);
  if (frm.fOkClicked()) {
    atx->color = color;
    if (atx->colColor.fValid()) {
      for (long i = 1; i <= atx->iSize(); ++i)
        if (aSelect[i-1]) {
          setDirty(i);
          atx->colColor->PutRaw(i, color);
          setDirty(i);
        }
    }
    else
      mpv->Invalidate();
    atx->Updated();
  }
}

void AnnotationTextEditor::OnChangeJust()
{
  class ChangeJustForm: public FormWithDest
  {
  public:
    ChangeJustForm(CWnd* wPar, int* just)
      : FormWithDest(wPar, SEDMsgAnnTextEditor)
    {
      fe = new FieldJustification(root, SEDUiJustification, just);
      create();
    }
    FormEntry* feDefaultFocus()
    {
      return fe;
    }
  private:
    FormEntry* fe;
  };
  if (atx->colJustification.fValid())
    for (long i = 1; i <= atx->iSize(); ++i)
      if (aSelect[i-1]) {
        atx->FromRec(i);
        break;
      }
  int iJustification = atx->iJustification;
  ChangeJustForm frm(mpv, &iJustification);
  if (frm.fOkClicked()) {
    atx->iJustification = iJustification;
    if (atx->colJustification.fValid()) {
      for (long i = 1; i <= atx->iSize(); ++i)
        if (aSelect[i-1]) {
          setDirty(i);
          atx->colJustification->PutVal(i, (long)iJustification);
          setDirty(i);
        }
    }
    else
      mpv->Invalidate();
    atx->Updated();
  }
}

void AnnotationTextEditor::OnChangeTransparent()
{
  atx->fTransparent = atx->fTransparent ? 0 : 1;
  if (atx->colTransparent.fValid()) {
    for (long i = 1; i <= atx->iSize(); ++i)
      if (aSelect[i-1]) {
        setDirty(i);
        bool fTransparent = atx->colTransparent->iValue(i) ? true:false;
        fTransparent = fTransparent ? 0 : 1;
        atx->colTransparent->PutVal(i, (long)fTransparent);
        setDirty(i);
      }
  }
  else
    mpv->Invalidate();
  atx->Updated();
}

void AnnotationTextEditor::OnChangeRotation()
{
  class ChangeRotationForm: public FormWithDest
  {
  public:
    ChangeRotationForm(CWnd* wPar, double* rRotation)
      : FormWithDest(wPar, SEDMsgAnnTextEditor)
    {
      fe = new FieldReal(root, SEDUiRotation, rRotation, ValueRange(-360,360,0.1));
      create();
    }
    FormEntry* feDefaultFocus()
    {
      return fe;
    }
  private:
    FormEntry* fe;
  };
  if (atx->colRotation.fValid())
    for (long i = 1; i <= atx->iSize(); ++i)
      if (aSelect[i-1]) {
        atx->FromRec(i);
        break;
      }
  double rRotation = atx->rRotation;
  ChangeRotationForm frm(mpv, &rRotation);
  if (frm.fOkClicked()) {
    atx->rRotation = rRotation;
    if (atx->colRotation.fValid()) {
      for (long i = 1; i <= atx->iSize(); ++i)
        if (aSelect[i-1]) {
          setDirty(i);
          atx->colRotation->PutVal(i, rRotation);
          setDirty(i);
        }
    }
    else
      mpv->Invalidate();
    atx->Updated();
  }
}

