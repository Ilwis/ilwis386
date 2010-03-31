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
// AnaglyphDrawer.cpp: implementation of the AnaglyphDrawer class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Engine\Map\Segment\Seg.h"
#include "engine\map\polygon\POL.H"
#include "Client\Mapwindow\Drawers\AnaglyphDrawer.h"
#include "Client\Mapwindow\Positioner.h"
#include "Client\ilwis.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Headers\Hs\Drwforms.hs"
#include "Headers\Hs\Appforms.hs"
#include "Engine\Base\System\RegistrySettings.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


AnaglyphDrawer::AnaglyphDrawer(MapCompositionDoc* mcd, const StereoPair& stereopair)
: MapDrawer(mcd, stereopair->mapLeft)
, stp(stereopair)
, eColors(eREDGREEN)
{
  drm = drmCOLOR;
  fNew = true;
	_dm = Domain("Color");
  rrLeft = stp->mapLeft->rrPerc1(true);
  rrRight = stp->mapRight->rrPerc1(true);
  fSelectable = false;
	iPixelOffSet = 0; //default no offset shift
}

AnaglyphDrawer::AnaglyphDrawer(MapCompositionDoc* mcd, const MapView& view, const char* sSection)
: MapDrawer(mcd, view, sSection)
, eColors(eREDGREEN)
{
  drm = drmCOLOR;
	_dm = Domain("Color");
  FileName fn;
  view->ReadElement(sSection, "StereoPair", fn);
  stp = StereoPair(fn);
  view->ReadElement(sSection, "Left Range", rrLeft);
  view->ReadElement(sSection, "Right Range", rrRight);
  String sColors;
  view->ReadElement(sSection, "Colors", sColors);
  if ("Red-Blue" == sColors)
    eColors = eREDBLUE;
  fSelectable = false;
	iPixelOffSet = 0; //default no offset shift
	view->ReadElement(sSection, "Pixel Shift", iPixelOffSet);
}

AnaglyphDrawer::~AnaglyphDrawer()
{
}

void AnaglyphDrawer::WriteLayer(MapView& view, const char* sSection)
{
  MapDrawer::WriteLayer(view,sSection);
  view->WriteElement(sSection, "Type", "AnaglyphDrawer");
  view->WriteElement(sSection, "StereoPair", stp);
  view->WriteElement(sSection, "Left Range", rrLeft);
  view->WriteElement(sSection, "Right Range", rrRight);
	view->WriteElement(sSection, "Pixel Shift", iPixelOffSet);
  String sColors;
  switch (eColors)
  {
  case eREDGREEN:
    sColors = "Red-Green";
    break;
  case eREDBLUE:
    sColors = "Red-Blue";
    break;
  }
  view->WriteElement(sSection, "Colors", sColors);
}

String AnaglyphDrawer::sName()
{
  return stp->sName(true);
}

IlwisObject AnaglyphDrawer::obj() const
{
  return stp;
}

zIcon AnaglyphDrawer::icon() const
{
	return zIcon("StereoPairIcon");
}

bool AnaglyphDrawer::fEditable()
{
  return false;
}

String AnaglyphDrawer::sInfo(const Coord&)
{
  return "";
}

int AnaglyphDrawer::Setup()
{
  return 0;
}

int AnaglyphDrawer::draw(CDC* cdc, zRect rect, Positioner* psn, volatile bool* fDrawStop)
{
  ILWISSingleLock csl(&cs, TRUE, SOURCE_LOCATION);
  if (!fAct) return 0;
  if (rect.width() == 0 || rect.height() == 0) return 0;
  MinMax mm = psn->mmSize();
  if (mm.width() == 0 || mm.height() == 0) return 0;
  long iMapLine;
  short iDspLine;
	double rScale = psn->rSc();
	double rFact = rScale > 0 ? 1/rScale : -rScale;
  RowCol rcSize = mp->rcSize();
  long iCols = rcSize.Col;
  long iLines = rcSize.Row;

  if (mm.MinCol() > iCols  ||
      mm.MaxCol() < 0	   ||
      mm.MinRow() > iLines ||
      mm.MaxRow() < 0)
    return 0;
  if (mm.MinCol() < 0) {
    rect.left() -= rounding(mm.MinCol() / rFact);
    mm.MinCol() = 0;
  }
  if (mm.MinRow() < 0) {
    rect.top() -= rounding(mm.MinRow() / rFact);
    mm.MinRow() = 0;
  }
  if (mm.MaxCol() >= iCols) {
    rect.right() -= rounding((mm.MaxCol()-iCols) / rFact);
    mm.MaxCol() = iCols;
  }
  if (mm.MaxRow() >= iLines) {
    rect.bottom() -= rounding((mm.MaxRow()-iLines) / rFact);
    mm.MaxRow() = iLines;
  }
  if (mm.width() == 0 || mm.height() == 0) return 0;
  lpbi->bmiHeader.biHeight = rect.height(); // no show solution
  lpbi->bmiHeader.biWidth  = rect.width(); // no show solution

  stp->mapLeft->KeepOpen(true);
  stp->mapRight->KeepOpen(true);

	riTranquilizer = RangeInt(0,rect.Height());
  if (abs(rFact-1) < 1e-6) {
    LongBuf buf(rect.width());
    for (iDspLine = 0, iMapLine = mm.MinRow();
	   iDspLine <= rect.height() && iMapLine < iLines;
	   iDspLine++, iMapLine++) 
		{
			iTranquilizer = iDspLine;
			if (*fDrawStop)
				break;
			GetLine(iMapLine, buf, mm.MinCol(), mm.width());
			PutLine(cdc, rect, iDspLine, buf);
    }
  }
  else // zoomed in or out
	{ 
		int iWidth = (int)((mm.width() + 1)/ rFact);
		int iPyrLayer = (int)(mp->fHasPyramidFile() ? max(0, log10(rFact) / log10(2.0)) : 0);					
		rFact /= pow(2.0, iPyrLayer);	
		mm.rcMax.Col /= (long)pow(2.0, iPyrLayer);
		mm.rcMax.Row /= (long)pow(2.0, iPyrLayer);				
		mm.rcMin.Col /= (long)pow(2.0, iPyrLayer);			
		mm.rcMin.Row /= (long)pow(2.0, iPyrLayer);		
		iLines /= (long)pow(2.0, iPyrLayer);			
    if (rect.width() > iWidth)
			rect.right() = rect.left() + iWidth;
    if (mm.MaxCol() < iCols /pow(2.0, iPyrLayer)) mm.MaxCol() += 1;
    LongBuf bMap(mm.width());
    LongBuf bDsp(iWidth);
		double rMapLine, rMapCol;
    int iCol;
		int iLastLine = -1;
    for (iDspLine = 0, rMapLine = mm.MinRow();
				iDspLine <= rect.height() && rMapLine < iLines;
				iDspLine++, rMapLine += rFact)
		{
			iTranquilizer = iDspLine;
			if (*fDrawStop)
				break;
			iMapLine = (long)floor(rMapLine);
			if (iMapLine != iLastLine) {
				GetLine(iMapLine, bMap, mm.MinCol(), mm.width(), iPyrLayer);
				iLastLine = iMapLine;
				for (iCol = 0, rMapCol = 0;
						iCol < iWidth;
						iCol++, rMapCol += rFact)
					bDsp[iCol] = bMap[(long)floor(rMapCol)];
			}
			PutLine(cdc, rect, iDspLine, bDsp);
    }
  }
  stp->mapLeft->KeepOpen(false);
  stp->mapRight->KeepOpen(false);

  return 0;
}

class AnaglyphDrawerForm: public DrawerForm
{
private:
	bool fPyrCreateFirstDisplay;	
public:
  AnaglyphDrawerForm(AnaglyphDrawer* adr, bool fShow)
  : DrawerForm(adr, SDCTitleAnaglyph),
	fPyrCreateFirstDisplay(false)
  {
  	iImg = IlwWinApp()->iImage(".stp");
		IlwisSettings settings("DefaultSettings");
	  fPyrCreateFirstDisplay = settings.fValue("CreatePyrWhenFirstDisplayed", false);	

    if (!fShow) 
      NotShowAlways();
  	String ss = adr->stp->sTypeName();
  	replace(ss.begin(), ss.end(), '\"', ' ');
  	StaticText *st = new StaticText(root, ss, false, true); // true: do not use & as accelerator
    st->SetIndependentPos();
  	CFont *font = IlwWinApp()->GetFont(IlwisWinApp::sfFORM);
  	LOGFONT lfont;
  	font->GetLogFont(&lfont);
  	lfont.lfHeight *= 1.2;
  	lfont.lfWeight = FW_SEMIBOLD;
  	CFont *fnt2 = new CFont();
  	fnt2->CreateFontIndirect(&lfont);
  	st->Font(fnt2);

    String s = adr->stp->sDescription;
    if (s.length() > 50)
      s[50] = 0;
    st = new StaticText(root, s);
    st->SetIndependentPos();
    
   	// AskScaleLimits(); - removed on request of Petra, bug 5781

  	RadioGroup* rg = new RadioGroup(root, "", (int*)&adr->eColors, true); 
    new RadioButton(rg, SDCUiRedGreen);
    new RadioButton(rg, SDCUiRedBlue);
    rg->SetIndependentPos();

    new FieldRangeReal(root, SDCUiLeftRange, &adr->rrLeft);
    new FieldRangeReal(root, SDCUiRightRange, &adr->rrRight);

		
		int iMaxShift = adr->mpGet()->rcSize().Col;
		new FieldInt(root, SDCUiPixelShift, &adr->iPixelOffSet, ValueRange(-iMaxShift, iMaxShift), true);

		if ((!adr->stp->mapLeft->fHasPyramidFile() && 
			!adr->stp->mapLeft->fReadOnly()) ||
			(!adr->stp->mapRight->fHasPyramidFile() && 
			!adr->stp->mapRight->fReadOnly()))
			new CheckBox(root, SDCUiCreatePyramidFiles, &fPyrCreateFirstDisplay);				

		SetMenHelpTopic(htpCnfAnaglyphDrawer);
    create();
  }
	bool fCalcPyramids() { return fPyrCreateFirstDisplay; };
};


int AnaglyphDrawer::Configure(bool fShow)
{
	CWnd* wnd = mcd->wndGetActiveView();

  if (0 == stp->mapLeft->dm()->pdv() || 0 == stp->mapRight->dm()->pdv()) // invalid domain of map 
  {
    wnd->MessageBox(SAFMsgImageOrValueRequired.scVal(),
				SDCTitleAnaglyph.scVal(),
				MB_OK|MB_ICONSTOP);
    return false;
  }
	CClientDC cdc(wnd);
	bool fPalette = cdc.GetDeviceCaps(RASTERCAPS) & RC_PALETTE ? true : false;
  if (fPalette) {
    wnd->MessageBox(SAFMsgColorDepth16bitOrHigher.scVal(),
				SDCTitleAnaglyph.scVal(),
				MB_OK|MB_ICONSTOP);
    return false;
  }
  AnaglyphDrawerForm frm(this, fShow);
  bool fOk = frm.fOkClicked();
  if (fOk)
	{
		if ( !stp->mapLeft->fReadOnly() && !stp->mapRight->fReadOnly())
			if (frm.fCalcPyramids())
			{
				if (!stp->mapLeft->fHasPyramidFile())
					stp->mapLeft->CreatePyramidLayer();
				if (!stp->mapRight->fHasPyramidFile())
					stp->mapRight->CreatePyramidLayer();
			}
    Setup();
  }
  return fOk;
}

// copied from MapListColorCompDrawer and simplified
void AnaglyphDrawer::GetLine(const Map& mp, const RangeReal& rr,
                             long iLine, ByteBuf& buf, long iFrom, long iNum, int iPyrLayer)
{
  long iLen = buf.iSize();
  RealBuf rb(iLen);
  mp->GetLineVal(iLine, rb, iFrom, iNum, iPyrLayer);
  double rLo = rr.rLo();
  double rHi = rr.rHi();
  double rRange = rHi - rLo;
  for (long i = 0; i < iLen; ++i) {
    long iTmp;
    if (rUNDEF == rb[i])
      iTmp = 0;
    else if (rb[i] < rLo)
      iTmp = 0;
    else if (rb[i] > rHi)
      iTmp = 255;
    else {
      iTmp = (long)((rb[i] - rLo) * 256 / rRange);
      if (iTmp < 0)
        iTmp = 0;
      else if (iTmp > 255)
        iTmp = 255;
    }
    buf[i] = (byte)iTmp;
  }
}

void AnaglyphDrawer::GetLine(long iLine, LongBuf& buf, long iFrom, long iNum, int iPyrLayer)
{
	int i;
  int iSize = buf.iSize();
  ByteBuf bufLeft(iSize);
  ByteBuf bufRight(iSize);
  GetLine(stp->mapLeft, rrLeft, iLine, bufLeft, iFrom, iNum, iPyrLayer);
  GetLine(stp->mapRight, rrRight, iLine, bufRight, iFrom-iPixelOffSet, iNum, iPyrLayer);

	int iShift = abs(iPixelOffSet);
	// beveiliging nodig voor het geval iShift > iSize (bij inzoomen) ?
  switch (eColors)
  {
		 case eREDGREEN:
			for (i = 0; i < iSize; ++i)
				buf[i] = (long)Color(bufLeft[i], bufRight[i], 0);
      break;
    case eREDBLUE:
			for (i = 0; i < iSize; ++i)
				buf[i] = (long)Color(bufLeft[i], bufRight[i], bufRight[i]);
      break;
		/*
    case eREDGREEN:
			if (iPixelOffSet >= 0) {
				for (i = 0; i < iShift; ++i)
					buf[i] = (long)Color(bufLeft[i], 0, 0);
				for (i = iShift; i < iSize; ++i)
					buf[i] = (long)Color(bufLeft[i], bufRight[i - iShift], 0);
			}
			else {
				for (i = 0; i < iSize - iShift; ++i)
					buf[i] = (long)Color(bufLeft[i], bufRight[i + iShift], 0);
				for (i = iSize - iShift; i < iSize; ++i)
					buf[i] = (long)Color(bufLeft[i], 0, 0);
			}
			break;
    case eREDBLUE:
			if (iPixelOffSet >= 0) {
				for (i = 0; i < iShift; ++i)
					buf[i] = (long)Color(bufLeft[i], 0, 0);
				for (i = iShift; i < iSize; ++i)
					buf[i] = (long)Color(bufLeft[i], bufRight[i - iShift], bufRight[i - iShift]);
			}
			else {
				for (i = 0; i < iSize - iShift; ++i)
					buf[i] = (long)Color(bufLeft[i], bufRight[i + iShift], bufRight[i + iShift]);
				for (i = iSize - iShift; i < iSize; ++i)
					buf[i] = (long)Color(bufLeft[i], 0, 0);
			}
      break;*/
	}
}

