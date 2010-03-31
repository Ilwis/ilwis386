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
// AnnotationTextDrawer.cpp: implementation of the AnnotationTextDrawer class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Engine\Map\Segment\Seg.h"
#include "engine\map\polygon\POL.H"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Engine\Map\Segment\Seg.h"
#include "Engine\Map\Polygon\POL.H"
#include "Client\Mapwindow\Positioner.h"
#include "Client\Mapwindow\Drawers\BaseDrawer.h"
#include "Client\Mapwindow\Drawers\Drawer.h"
#include "Engine\Map\txtann.h"
#include "Client\Mapwindow\Drawers\AnnotationTextDrawer.h"
#include "Headers\Hs\Drwforms.hs"
#include "Client\FormElements\syscolor.h"
#include "Client\FormElements\fldcol.h"
#include "Client\FormElements\fldrpr.h"
#include "Client\FormElements\fldcolor.h"
#include "Client\Base\datawind.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\MainWindow\Catalog\CatalogDocument.h"
#include "Engine\Map\Mapview.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\ilwis.h"
#include "Client\Editors\Utils\sizecbar.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(AnnotationTextDrawer, Drawer)
END_MESSAGE_MAP()


AnnotationTextDrawer::AnnotationTextDrawer(MapCompositionDoc*  mapcd, const AnnotationText& anntxt)
: Drawer(mapcd), at(anntxt)
{
}

AnnotationTextDrawer::AnnotationTextDrawer(MapCompositionDoc* mapcd, const MapView& view, const char* sSection)
: Drawer(mapcd, view, sSection)
{
  FileName fn;
  view->ReadElement(sSection, "AnnotationText", fn);
  at = AnnotationText(fn);
}

AnnotationTextDrawer::~AnnotationTextDrawer()
{
  ILWISSingleLock csl(&cs, TRUE, SOURCE_LOCATION);
}

void AnnotationTextDrawer::WriteLayer(MapView& view, const char* sSection)
{
  view->WriteElement(sSection, "Type", "AnnotationTextDrawer");
  FileName fn = at->sName(true);
  view->WriteElement(sSection, "AnnotationText", fn);
  Drawer::WriteLayer(view,sSection);
}

bool AnnotationTextDrawer::fEditable()
{
	MapPaneView* mpv = mcd->mpvGetView();
	if (0 == mpv)
		return false;
  return !at->fDataReadOnly();
}

bool AnnotationTextDrawer::fProperty()
{
  return true;
}

IlwisObject AnnotationTextDrawer::obj() const
{
  return at;
}

String AnnotationTextDrawer::sName()
{
  return at->sName(true);
}

zIcon AnnotationTextDrawer::icon() const
{
	return zIcon("LogoIcon");
}

int AnnotationTextDrawer::draw(CDC* cdc, zRect rect, Positioner* psn, volatile bool* fDrawStop)
{
  ILWISSingleLock csl(&cs, TRUE, SOURCE_LOCATION);
  if (!fAct)
    return 0;
  long iNr = at->iSize();
	riTranquilizer = RangeInt(1,iNr);
  for (long i = 1; i <= iNr; ++i) {
		iTranquilizer = i;
		if (*fDrawStop)
			break;
    at->draw(cdc, psn, i);
  }
  return 0;
}

int AnnotationTextDrawer::Configure(bool fShowForm)
{
  class AnnTxtForm : public DrawerForm
  {
  public:
    AnnTxtForm(Drawer* dr, bool fShowForm, bool* fShow)
      : DrawerForm(dr, SDCTitleAnnotation)
    {
			iImg = IlwWinApp()->iImage(".atx");

      if (!fShowForm)
        NotShowAlways();
			AskScaleLimits();
      create();
    }
  };
  bool fShow = fAct;
  if (fNew) {
    fAct = false;  // to prevent "in between" drawing
    fNew = false;
  }
  AnnTxtForm frm(this, fShowForm, &fShow);
  bool fOk = frm .fOkClicked();
  if (fOk)
    fAct = fShow;
  return fOk;
}


void AnnotationTextPtr::draw(CDC* cdc, Positioner* psn, long iRec)
{
  FromRec(iRec);
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
  rSc /= rWorkingScale;
  int iHeight = (int)(-rounding(10 * rFontSize / rSc));
  if (abs(iHeight) < 5)
    return;
  int iWeight = fBold ? FW_BOLD : FW_NORMAL;
  int iEscapement = (int)(10 * rRotation);
  int iOrientation = iEscapement; //fUpright ? 0 : iEscapement;
	CFont fnt;
	LOGFONT logFont;
	memset(&logFont, 0, sizeof(logFont));
	logFont.lfHeight = -iHeight;
	logFont.lfWeight = iWeight;
	logFont.lfPitchAndFamily = VARIABLE_PITCH;
	logFont.lfItalic = fItalic;
	logFont.lfUnderline = fUnderline;
	logFont.lfEscapement = iEscapement;
	logFont.lfOrientation = iOrientation;
	lstrcpy(logFont.lfFaceName, sFontName.scVal());
	fnt.CreatePointFontIndirect(&logFont, cdc);
	CFont* fntOld = cdc->SelectObject(&fnt);
  if (fTransparent)
    cdc->SetBkMode(TRANSPARENT);
  else
    cdc->SetBkMode(OPAQUE);
  cdc->SetTextColor(color);
  int iAlign = 0;
  switch (iJustification) {
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
  if (fUseXY) {
    Coord c;
    c.x = colX->rValue(iRec);
    c.y = colY->rValue(iRec);
    pnt = psn->pntPos(c);
  }
  else {
    double rRow = colRow->rValue(iRec);
    double rCol = colCol->rValue(iRec);
    pnt = psn->pntPos(rRow, rCol);
  }
  String sText = colText->sValue(iRec,0);
  cdc->TextOut(pnt.x, pnt.y, sText.scVal());
	cdc->SelectObject(fntOld);
}
