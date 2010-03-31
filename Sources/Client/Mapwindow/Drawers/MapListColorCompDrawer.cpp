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
// MapListColorCompDrawer.cpp: implementation of the MapListColorCompDrawer class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Engine\Map\Segment\Seg.h"
#include "Engine\Map\Polygon\POL.H"
#include "Engine\SpatialReference\GR3D.H"
#include "Client\Mapwindow\Positioner.h"
#include "Client\Mapwindow\Drawers\BaseDrawer.h"
#include "Client\Mapwindow\Drawers\Drawer.h"
#include "Headers\Hs\Drwforms.hs"
#include "Headers\Hs\Appforms.hs"
#include "Client\FormElements\fldcol.h"
#include "Client\FormElements\fldrpr.h"
#include "Client\FormElements\fldcolor.h"
#include "Client\Mapwindow\Drawers\BaseMapDrawer.h"
#include "Client\ilwis.h"
#include "Client\Base\datawind.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\MainWindow\Catalog\CatalogDocument.h"
#include "Client\Mapwindow\MapWindow.h"
#include "Engine\Map\Mapview.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Client\FormElements\syscolor.h"
#include "Engine\Domain\dmsort.h"
#include "Client\Mapwindow\Drawers\MapDrawer.h"
#include "Client\Mapwindow\Drawers\MapListColorCompDrawer.h"
#include "Client\FormElements\fldmapml.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(MapListColorCompDrawer, MapDrawer)
	//{{AFX_MSG_MAP(MapListColorCompDrawer)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MapListColorCompDrawer::MapListColorCompDrawer(MapCompositionDoc* mcd, const MapList& ml)
: MapDrawer(mcd, ml[ml->iLower()])
{
  maplist = ml;
  drm = drmCOLOR;
  fNew = true;
  iRed = ml->iUpper();
  iBlue = ml->iLower();
  iGreen = (iRed + iBlue) / 2;
	eMethod = eRGB;
	_dm = Domain("Color");
}

MapListColorCompDrawer::MapListColorCompDrawer(MapCompositionDoc* mcd, const MapView& view, const char* sSection)
: MapDrawer(mcd, view, sSection)
{
  drm = drmCOLOR;
  view->ReadElement(sSection, "MapList", maplist);
  iRed = maplist->iUpper();
  iBlue = maplist->iLower();
  iGreen = (iRed + iBlue) / 2;
  iRed = view->iReadElement(sSection, "Red Band");
  if (maplist[iRed]->fRealValues())
    view->ReadElement(sSection, "Red Range", rrRed);
  else
    view->ReadElement(sSection, "Red Range", riRed);
  iGreen = view->iReadElement(sSection, "Green Band");
  if (maplist[iGreen]->fRealValues())
    view->ReadElement(sSection, "Green Range", rrGreen);
  else
    view->ReadElement(sSection, "Green Range", riGreen);
  iBlue = view->iReadElement(sSection, "Blue Band");
  if (maplist[iBlue]->fRealValues())
    view->ReadElement(sSection, "Blue Range", rrBlue);
  else
    view->ReadElement(sSection, "Blue Range", riBlue);

	String sMethod;
	view->ReadElement(sSection, "Method", sMethod);
	eMethod = eRGB;
	if ("YMC" == sMethod)
		eMethod = eYMC;
	else if ("HSI" == sMethod)
		eMethod = eHSI;
	_dm = Domain("Color");
}

MapListColorCompDrawer::~MapListColorCompDrawer()
{
  ILWISSingleLock csl(&cs, TRUE, SOURCE_LOCATION);
}

void MapListColorCompDrawer::WriteLayer(MapView& view, const char* sSection)
{
  MapDrawer::WriteLayer(view,sSection);
  view->WriteElement(sSection, "Type", "MapListColorCompDrawer");
  view->WriteElement(sSection, "MapList", maplist);

  view->WriteElement(sSection, "Red Band", iRed);
  if (maplist[iRed]->fRealValues())
    view->WriteElement(sSection, "Red Range", rrRed);
  else
    view->WriteElement(sSection, "Red Range", riRed);
  view->WriteElement(sSection, "Green Band", iGreen);
  if (maplist[iGreen]->fRealValues())
    view->WriteElement(sSection, "Green Range", rrGreen);
  else
    view->WriteElement(sSection, "Green Range", riGreen);
  view->WriteElement(sSection, "Blue Band", iBlue);
  if (maplist[iBlue]->fRealValues())
    view->WriteElement(sSection, "Blue Range", rrBlue);
  else
    view->WriteElement(sSection, "Blue Range", riBlue);
	switch (eMethod) {
		case eRGB:
	    view->WriteElement(sSection, "Method", "RGB");
			break;
		case eYMC:
	    view->WriteElement(sSection, "Method", "YMC");
			break;
		case eHSI:
	    view->WriteElement(sSection, "Method", "HSI");
			break;
	}
}

String MapListColorCompDrawer::sName()
{
  return maplist->sName(true);
}

int MapListColorCompDrawer::Setup()
{
  InitArray(maplist[iRed], riRed, aRed);
  InitArray(maplist[iGreen], riGreen, aGreen);
  InitArray(maplist[iBlue], riBlue, aBlue);

  return 0;
}

void MapListColorCompDrawer::GetLine(const Map& mp, const RangeInt& ri, const RangeReal& rr,
                                     const Array<long>& array,
                                     long iLine, ByteBuf& buf, long iFrom, long iNum, int iPyrLayer)
{
  long iLen = buf.iSize();
  if (mp->fRealValues()) {
    RealBuf rb(iLen);
    mp->GetLineVal(iLine, rb, iFrom, iNum, iPyrLayer);
    double rLo = rr.rLo();
    double rHi = rr.rHi();
    double rRange = rHi - rLo;
    for (long i = 0; i < iLen; ++i) {
      long iTmp;
      if (rUNDEF == rb[i])
        iTmp = 0;
      else if (rb[i] <= rLo)
        iTmp = 0;
      else if (rb[i] > rHi)
        iTmp = 255;
      else {
        iTmp = (rb[i] - rLo) * 256 / rRange;
        if (iTmp < 0)
          iTmp = 0;
        else if (iTmp > 255)
          iTmp = 255;
      }
      buf[i] = iTmp;
    }
  }
  else if (mp->st() == stBYTE && mp->fRawIsValue()) {
    mp->GetLineRaw(iLine, buf, iFrom, iNum, iPyrLayer);
    for (long i = 0; i < iLen; ++i) {
      buf[i] = array[buf[i]];
    }
/*
    long iLo = ri.iLo();
    long iHi = ri.iHi();
    long iRange = iHi - iLo;
    for (long i = 0; i < iLen; ++i) {
      long iTmp;
      if (buf[i] <= iLo)
        iTmp = 0;
      else if (buf[i] >= iHi)
        iTmp = 255;
      else {
        iTmp = (buf[i] - iLo) * 256 / iRange;
        if (iTmp < 0)
          iTmp = 0;
        else if (iTmp > 255)
          iTmp = 255;
      }
      buf[i] = iTmp;
    }
*/
  }
  else {
    LongBuf lb(iLen);
    mp->GetLineVal(iLine, lb, iFrom, iNum, iPyrLayer);
    long iLo = ri.iLo();
    long iHi = ri.iHi();
    long iRange = iHi - iLo;
    for (long i = 0; i < iLen; ++i) {
      long iTmp;
      if (iUNDEF == lb[i])
        iTmp = 0;
      else if (lb[i] <= iLo)
        iTmp = 0;
      else if (lb[i] > iHi)
        iTmp = 255;
      else {
        iTmp = (lb[i] - iLo) * 256 / iRange;
        if (iTmp < 0)
          iTmp = 0;
        else if (iTmp > 255)
          iTmp = 255;
      }
      buf[i] = iTmp;
    }
  }
}

void MapListColorCompDrawer::GetLine(long iLine, LongBuf& buf, long iFrom, long iNum, int iPyrLayer)
{
	int i;
  int iSize = buf.iSize();
  ByteBuf bufRed(iSize);
  ByteBuf bufGreen(iSize);
  ByteBuf bufBlue(iSize);
  GetLine(maplist[iRed], riRed, rrRed, aRed, iLine, bufRed, iFrom, iNum, iPyrLayer);
  GetLine(maplist[iGreen], riGreen, rrGreen, aGreen, iLine, bufGreen, iFrom, iNum, iPyrLayer);
  GetLine(maplist[iBlue], riBlue, rrBlue, aBlue, iLine, bufBlue, iFrom, iNum, iPyrLayer);
	switch (eMethod) {
		case eRGB:
			for (i = 0; i < iSize; ++i)
				buf[i] = (long)Color(bufRed[i], bufGreen[i], bufBlue[i]);
			break;
		case eYMC:
			for (i = 0; i < iSize; ++i)
				buf[i] = (long)Color(255-bufBlue[i], 255-bufGreen[i], 255-bufRed[i]);
			break;
		case eHSI: 
			for (i = 0; i < iSize; ++i) {
				Color clr;
				clr.SetHSI(bufRed[i],bufGreen[i],bufBlue[i]);
				buf[i] = (long)clr;
			}
			break;
	}
}

void MapListColorCompDrawer::InitArray(const Map& mp, const RangeInt& ri, Array<long>& array)
{
  if (mp->fRealValues())
    return;
  if (mp->st() != stBYTE || !mp->fRawIsValue())
    return;
  array.Resize(256);

  long iLo = ri.iLo();
  long iHi = ri.iHi();
  long iRange = iHi - iLo;
  for (long i = 0; i <= 255; ++i) {
    long iTmp;
    if (i <= iLo)
      iTmp = 0;
    else if (i > iHi)
      iTmp = 255;
    else {
      iTmp = (i - iLo) * 256 / iRange;
      if (iTmp < 0)
        iTmp = 0;
      else if (iTmp > 255)
        iTmp = 255;
    }
    array[i] = iTmp;
  }
}

int MapListColorCompDrawer::draw(CDC* cdc, zRect rect, Positioner* psn, volatile bool* fDrawStop)
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
//  double rFact = (double)mm.width() / rect.width();
  RowCol rcSize = mp->rcSize();
  long iCols = rcSize.Col;
  long iLines = rcSize.Row;

  if (mm.MinCol() > iCols  ||
      mm.MaxCol() < 0	   ||
      mm.MinRow() > iLines ||
      mm.MaxRow() < 0)
    return 0;
  if (mm.MinCol() < 0) {
    rect.left() -= round(mm.MinCol() / rFact);
    mm.MinCol() = 0;
  }
  if (mm.MinRow() < 0) {
    rect.top() -= round(mm.MinRow() / rFact);
    mm.MinRow() = 0;
  }
  if (mm.MaxCol() >= iCols) {
    rect.right() -= round((mm.MaxCol()-iCols) / rFact);
    mm.MaxCol() = iCols;
  }
  if (mm.MaxRow() >= iLines) {
    rect.bottom() -= round((mm.MaxRow()-iLines) / rFact);
    mm.MaxRow() = iLines;
  }
  if (mm.width() == 0 || mm.height() == 0) return 0;
  lpbi->bmiHeader.biHeight = rect.height(); // no show solution
  lpbi->bmiHeader.biWidth  = rect.width(); // no show solution

/*
  if (mppn->canvas()->type() != WinDisp &&
      mppn->canvas()->type() != MetaDisp)
    bmTmp = new zBitmap(rect.width(),rect.height(),mppn->canvas());
*/
  maplist[iRed]->KeepOpen(true);
  maplist[iGreen]->KeepOpen(true);
  maplist[iBlue]->KeepOpen(true);

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
		int iWidth = (mm.width() + 1)/ rFact;
		int iPyrLayer = maplist->fHasPyramidFiles() ? max(0, log10(rFact) / log10(2.0)) : 0;					
		rFact /= pow(2.0, iPyrLayer);	
		mm.rcMax.Col /= pow(2.0, iPyrLayer);
		mm.rcMax.Row /= pow(2.0, iPyrLayer);				
		mm.rcMin.Col /= pow(2.0, iPyrLayer);			
		mm.rcMin.Row /= pow(2.0, iPyrLayer);		
		iLines /= pow(2.0, iPyrLayer);			
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
			iMapLine = floor(rMapLine);
			if (iMapLine != iLastLine) {
				GetLine(iMapLine, bMap, mm.MinCol(), mm.width(), iPyrLayer);
				iLastLine = iMapLine;
				for (iCol = 0, rMapCol = 0;
						iCol < iWidth;
						iCol++, rMapCol += rFact)
					bDsp[iCol] = bMap[floor(rMapCol)];
			}
			PutLine(cdc, rect, iDspLine, bDsp);
    }
  }
  maplist[iRed]->KeepOpen(false);
  maplist[iGreen]->KeepOpen(false);
  maplist[iBlue]->KeepOpen(false);
  return 0;
}

String MapListColorCompDrawer::sInfo(const Coord& crd)
{
  String sRed = maplist[iRed]->sValue(crd,0);
  String sGreen = maplist[iGreen]->sValue(crd,0);
  String sBlue = maplist[iBlue]->sValue(crd,0);
  String s("(%S,%S,%S)", sRed, sGreen, sBlue);
  return s;
}

class MapListColorCompDrawerForm: public DrawerForm
{
public:
  MapListColorCompDrawerForm(MapListColorCompDrawer* mdr, bool fShow);
  int exec();
	bool fCreatePyramidFiles();
private:
  MapListColorCompDrawer* mdw;
  FieldMapInMapList *fMapRed, *fMapGreen, *fMapBlue;
  FieldRangeReal *fRangeRed, *fRangeGreen, *fRangeBlue;
  FieldRangeInt *fRiRed, *fRiGreen, *fRiBlue;
  int RedCallBack(Event*);
  int GreenCallBack(Event*);
  int BlueCallBack(Event*);
  MapList ml;
  long iRed, iGreen, iBlue;
	bool fPyrCreateFirstDisplay;
};

int MapListColorCompDrawer::Configure(bool fShowForm)
{
	CWnd* wnd = mcd->wndGetActiveView();

  if (0 == maplist[iBlue]->dm()->pdv()) // invalid domain of map in maplist
  {
    wnd->MessageBox(SAFMsgImageOrValueRequired.scVal(),
				SDCTitleMapListColorComp.scVal(),
				MB_OK|MB_ICONSTOP);
    return false;
  }
	CClientDC cdc(wnd);
	bool fPalette = cdc.GetDeviceCaps(RASTERCAPS) & RC_PALETTE ? true : false;
  if (fPalette) {
    wnd->MessageBox(SAFMsgColorDepth16bitOrHigher.scVal(),
				SDCTitleMapListColorComp.scVal(),
				MB_OK|MB_ICONSTOP);
    return false;
  }
  MapListColorCompDrawerForm frm(this, fShowForm);
  bool fOk = frm.fOkClicked();
  if (fOk) 
	{
		if ( !maplist->fHasPyramidFiles()  && frm.fCreatePyramidFiles() )
			maplist->CreatePyramidLayers();		
    Setup();
  }
  return fOk;
}

MapListColorCompDrawerForm::MapListColorCompDrawerForm(MapListColorCompDrawer* mdr, bool fShowForm)
: DrawerForm(mdr, SDCTitleMapListColorComp),
  mdw(mdr)
{
	iImg = IlwWinApp()->iImage(".mpl");
	IlwisSettings settings("DefaultSettings");
  fPyrCreateFirstDisplay = settings.fValue("CreatePyrWhenFirstDisplayed", false);	
  if (!fShowForm) 
    NotShowAlways();
  FieldGroup* fg = new FieldGroup(root,true);
  StaticText *st = new StaticText(fg, mdw->maplist->sTypeName());
  st->SetIndependentPos();
/*
  zFontInfo info(windsp());
  zFont *font = new zFont("", zDimension(0, info.height()*1.25),
                          FW_BOLD,ffSwiss);
  st->Font(font);
*/
  String s = mdw->maplist->sDescription;
  if (s.length() > 80) {
    s[78] = 0;
    s &= "...";
  }
  new StaticText(fg, s);
  st->SetIndependentPos();
  CheckBox* cb = new CheckBox(fg, SDCUiInfo, &mdw->fSelectable);

 	AskScaleLimits();

	RadioGroup* rgMethod = new RadioGroup(root, "", (int*)&mdw->eMethod); 
	RadioButton* rbRGB = new RadioButton(rgMethod, SDCUiRGB);
	RadioButton* rbYMC = new RadioButton(rgMethod, SDCUiYMC);
	RadioButton* rbHSI = new RadioButton(rgMethod, SDCUiHSI);

	FieldGroup* fgRGB = new FieldGroup(rbRGB);
	fgRGB->Align(rbRGB, AL_AFTER);
	StaticText* stRed = new StaticText(fgRGB, SDCUiRedBand);
	StaticText* stGreen = new StaticText(fgRGB, SDCUiGreenBand);
	StaticText* stBlue = new StaticText(fgRGB, SDCUiBlueBand);

	FieldGroup* fgYMC = new FieldGroup(rbYMC);
	fgYMC->Align(rbRGB, AL_AFTER);
	new StaticText(fgYMC, SDCUiYellowBand);
	new StaticText(fgYMC, SDCUiMagentaBand);
	new StaticText(fgYMC, SDCUiCyanBand);

	FieldGroup* fgHSI = new FieldGroup(rbHSI);
	fgHSI->Align(rbRGB, AL_AFTER);
	new StaticText(fgHSI, SDCUiHueBand);
	new StaticText(fgHSI, SDCUiSaturationBand);
	new StaticText(fgHSI, SDCUiIntensityBand);
  
	ml = mdw->maplist;
  iRed = mdw->iRed;
  fMapRed = new FieldMapInMapList(root, "", ml, &iRed);
	fMapRed->Align(stRed, AL_AFTER);
  fMapRed->SetCallBack((NotifyProc)&MapListColorCompDrawerForm::RedCallBack);
  fRangeRed = new FieldRangeReal(root, "", &mdw->rrRed);
  fRangeRed->Align(fMapRed, AL_AFTER);
  fRiRed = new FieldRangeInt(root, "", &mdw->riRed);
  fRiRed->Align(fMapRed, AL_AFTER);

  iGreen = mdw->iGreen;
  fMapGreen = new FieldMapInMapList(root, "", ml, &iGreen);
  fMapGreen->Align(stGreen, AL_AFTER);
  fMapGreen->SetCallBack((NotifyProc)&MapListColorCompDrawerForm::GreenCallBack);
  fRangeGreen = new FieldRangeReal(root, "", &mdw->rrGreen);
  fRangeGreen->Align(fMapGreen, AL_AFTER);
  fRiGreen = new FieldRangeInt(root, "", &mdw->riGreen);
  fRiGreen->Align(fMapGreen, AL_AFTER);

  iBlue = mdw->iBlue;
  fMapBlue = new FieldMapInMapList(root, "", ml, &iBlue);
  fMapBlue->Align(stBlue, AL_AFTER);
  fMapBlue->SetCallBack((NotifyProc)&MapListColorCompDrawerForm::BlueCallBack);
  fRangeBlue = new FieldRangeReal(root, "", &mdw->rrBlue);
  fRangeBlue->Align(fMapBlue, AL_AFTER);
  fRiBlue = new FieldRangeInt(root, "", &mdw->riBlue);
  fRiBlue->Align(fMapBlue, AL_AFTER);

  RadioGroup* rg = new RadioGroup(root, "", (int*)&mdw->drc, true);
  rg->Align(rgMethod, AL_UNDER);
  rg->SetIndependentPos();
  RadioButton *rb = new RadioButton(rg, SDCUiLight);
	rb->SetIndependentPos();
  new RadioButton(rg, SDCUiNormal);
  new RadioButton(rg, SDCUiDark);
  new RadioButton(rg, SDCUiGrey);
	if (!ml->fHasPyramidFiles() && !ml->fReadOnly())
	{
		cb = new CheckBox(rg, SDCUiCreatePyramidFiles, &fPyrCreateFirstDisplay);	
//		cb->Align(rb, AL_UNDER);
//		cb->SetIndependentPos();
	}

  SetMenHelpTopic(htpCnfMapListColorComp);
  create();
}

bool MapListColorCompDrawerForm::fCreatePyramidFiles()
{
	return fPyrCreateFirstDisplay;
}

int MapListColorCompDrawerForm::exec()
{
  FormWithDest::exec();
  mdw->iRed = iRed;
  mdw->iGreen = iGreen;
  mdw->iBlue = iBlue;
  return 0;
}

int MapListColorCompDrawerForm::RedCallBack(Event*)
{
  int iRedOld = iRed;
  fMapRed->StoreData();
  if (ml[iRed]->fRealValues()) {
    fRiRed->Hide();
    fRiRed->SetVal(RangeInt(0,1));
    fRangeRed->Show();
    if (iRed == iRedOld && 0 == fRangeRed->CheckData())
      return 0;
    RangeReal rr = ml[iRed]->rrPerc1(true);
    fRangeRed->SetVal(rr);
  }
  else {
    fRangeRed->Hide();
    fRangeRed->SetVal(RangeReal(0,1));
    fRiRed->Show();
    if (iRed == iRedOld && 0 == fRiRed->CheckData())
      return 0;
    RangeInt ri = ml[iRed]->riPerc1(true);
    fRiRed->SetVal(ri);
  }
  return 0;
}

int MapListColorCompDrawerForm::GreenCallBack(Event*)
{
  int iGreenOld = iGreen;
  fMapGreen->StoreData();
  fMapGreen->StoreData();
  if (ml[iGreen]->fRealValues()) {
    fRiGreen->Hide();
    fRiGreen->SetVal(RangeInt(0,1));
    fRangeGreen->Show();
    if (iGreen == iGreenOld && 0 == fRangeGreen->CheckData())
      return 0;
    RangeReal rr = ml[iGreen]->rrPerc1(true);
    fRangeGreen->SetVal(rr);
  }
  else {
    fRangeGreen->Hide();
    fRangeGreen->SetVal(RangeReal(0,1));
    fRiGreen->Show();
    if (iGreen == iGreenOld && 0 == fRiGreen->CheckData())
      return 0;
    RangeInt ri = ml[iGreen]->riPerc1(true);
    fRiGreen->SetVal(ri);
  }
  return 0;
}

int MapListColorCompDrawerForm::BlueCallBack(Event*)
{
  int iBlueOld = iBlue;
  fMapBlue->StoreData();
  fMapBlue->StoreData();
  if (ml[iBlue]->fRealValues()) {
    fRiBlue->Hide();
    fRiBlue->SetVal(RangeInt(0,1));
    fRangeBlue->Show();
    if (iBlue == iBlueOld && 0 == fRangeBlue->CheckData())
      return 0;
    RangeReal rr = ml[iBlue]->rrPerc1(true);
    fRangeBlue->SetVal(rr);
  }
  else {
    fRangeBlue->Hide();
    fRangeBlue->SetVal(RangeReal(0,1));
    fRiBlue->Show();
    if (iBlue == iBlueOld && 0 == fRiBlue->CheckData())
      return 0;
    RangeInt ri = ml[iBlue]->riPerc1(true);
    fRiBlue->SetVal(ri);
  }
  return 0;
}


IlwisObject MapListColorCompDrawer::obj() const
{
  return maplist;
}

zIcon MapListColorCompDrawer::icon() const
{
	return zIcon("MapListIcon");
}

bool MapListColorCompDrawer::fEditable()
{
	return false;
}



