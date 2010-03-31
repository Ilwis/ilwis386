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
// PointMapDrawer.cpp: implementation of the PointMapDrawer class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Engine\Map\Segment\Seg.h"
#include "engine\map\polygon\POL.H"
#include "Engine\Map\Point\PNT.H"
#include "Client\Mapwindow\Positioner.h"
#include "Client\Mapwindow\Drawers\BaseDrawer.h"
#include "Client\Mapwindow\Drawers\Drawer.h"
#include "Headers\Hs\Drwforms.hs"
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
#include "Client\Editors\Utils\SYMBOL.H"
#include "Client\Editors\Utils\Smbext.h"
#include "Client\Editors\Utils\MULTICOL.H"
#include "Client\Mapwindow\Drawers\PointMapDrawer.h"
#include "Client\Mapwindow\Drawers\PointMapDrawerForm.h"
#include "Client\Editors\Utils\line.h"
#include "Client\Editors\Representation\RprEditForms.h"
#include "Engine\Representation\Rprclass.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#undef Polygon

BEGIN_MESSAGE_MAP(PointMapDrawer, BaseMapDrawer)
	//{{AFX_MSG_MAP(PointMapDrawer)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

PointMapDrawer::PointMapDrawer(MapCompositionDoc* mapcd, const PointMap& pm)
  : BaseMapDrawer(mapcd), fMask(false), mask(pm->dm()),
  iFHeight(100 * RepresentationClass::iSIZE_FACTOR),
  sFaceName("Arial"),
  iFWeight(FW_BOLD),
  iFStyle(0),
  iWidth(0),
  clrLine(-1)
{
  mp = pm;
  _dm = mp->dm();
  bmap = mp.ptr();
  if (pm->dm()->pdc())
    _rpr = pm->dm()->rpr();
  fText = false;
  fSymbol = true;
  fStretch = false;
  fSelectable = true;
  clrText = -1;
  if (_rpr.fValid())
    drm = drmRPR;
  else
    drm = drmSINGLE;
	smb.Resize(RepresentationClass::iSIZE_FACTOR);
  iSize = smb.iSize;  
  iMinSize = iSize;
  iLinLog = 0;
  iRadiusArea = 0;
  
  ValueRange vr = mp->vr();
  if (mp->dm()->pdbool())
    vr = ValueRange();
  if (vr.fValid()) {
    fStretch = true;
    if (vr->vrr()) {
      rrStretch = mp->rrMinMax(true);
      if (rrStretch.rLo() >= rrStretch.rHi())
        rrStretch = vr->rrMinMax();
      riStretch.iLo() = rounding(rrStretch.rLo());
      riStretch.iHi() = rounding(rrStretch.rHi());
    }
    else {
      riStretch = mp->riMinMax(true);
      if (riStretch.iLo() >= riStretch.iHi())
        riStretch = vr->riMinMax();
      rrStretch.rLo() = doubleConv(riStretch.iLo());
      rrStretch.rHi() = doubleConv(riStretch.iHi());
    }
  }
}

PointMapDrawer::PointMapDrawer(MapCompositionDoc* mapcd, const MapView& view, const char* sSection)
: BaseMapDrawer(mapcd, view, sSection, "PointMap"),
  fMask(false),
  iFHeight(100),
  sFaceName("Arial"),
  iFWeight(FW_BOLD),
  iFStyle(0),
  iWidth(0),
  clrLine(-1)
{
  view->ReadElement(sSection, "PointMap", mp);
  bmap = mp.ptr();
  mask.SetDomain(mp->dm());
//  dm = map->dm();
  fSymbol = true;
  view->ReadElement(sSection, "ShowSymbol", fSymbol);
  smb = ExtendedSymbol(view, sSection);
  iSize = view->iReadElement(sSection, "Size");
  smb.iSize = iSize;
  iMinSize = iSize;
  fStretch = false;
  iLinLog = 0;
  iRadiusArea = 0;
  view->ReadElement(sSection, "Stretch", fStretch);
  if (fStretch) {
    view->ReadElement(sSection, "Range", rrStretch);
    view->ReadElement(sSection, "MinSize", iMinSize);
    String sLinLog;
    view->ReadElement(sSection, "LinLog", sLinLog);
    if ("Linear" == sLinLog)
      iLinLog = 0;
    else if ("Logarithmic" == sLinLog)
      iLinLog = 1;
    String sRadiusArea;
    view->ReadElement(sSection, "RadiusArea", sRadiusArea);
    if ("Radius" == sRadiusArea)
      iRadiusArea = 0;
    else if ("Area" == sRadiusArea)
      iRadiusArea = 1;
  }
  view->ReadElement(sSection, "Text", fText);
  view->ReadElement(sSection, "TextColor", (Color&)clrText);
  if (fText) {
    view->ReadElement(sSection, "FontType", sFaceName);
    iFHeight = (short)view->iReadElement(sSection, "FontSize");
    iFWeight = (short)view->iReadElement(sSection, "FontWeight");
    iFStyle = (short)view->iReadElement(sSection, "FontStyle");
  }
  String sMask;
  view->ReadElement(sSection, "Mask", sMask);
  if (sMask.length()) {
    fMask = true;
    mask.SetMask(sMask);
  }
  Table tbl = mp->tblAtt();
  if (tbl.fValid()) {
    String sCol;
    if (view->ReadElement(sSection, "Column Class", sCol))
      colCls = tbl->col(sCol);
    sCol = String();
    if (view->ReadElement(sSection, "Column Rotation", sCol))
      colRot = tbl->col(sCol);
    sCol = String();
    if (view->ReadElement(sSection, "Column Color", sCol))
      colClr = tbl->col(sCol);
    sCol = String();
    if (view->ReadElement(sSection, "Column Value", sCol))
      colVal = tbl->col(sCol);
    sCol = String();
    if (view->ReadElement(sSection, "Column Width", sCol))
      colWidth = tbl->col(sCol);
    sCol = String();
    if (view->ReadElement(sSection, "Column Text", sCol))
      colTxt = tbl->col(sCol);
    sCol = String();
    if (view->ReadElement(sSection, "Column Justification", sCol))
      colJst = tbl->col(sCol);
  }
  if (drm == drmGRAPH) {
    String sGraphType;
    view->ReadElement(sSection, "Graph Type", sGraphType);
    if ("PieChart" == sGraphType)
      eST = stPIECHART;
    else if ("BarGraph" == sGraphType)
      eST = stBARGRAPH;
    else if ("LineGraph" == sGraphType)
      eST = stGRAPH;
    else if ("CompositeBar" == sGraphType)
      eST = stCOMPBAR;
    else if ("VolumeCubes" == sGraphType)
      eST = stVOLCUBES;
    else
      drm = drmSINGLE;
    iWidth = view->iReadElement(sSection, "Width");
    view->ReadElement(sSection, "Line Color", (Color&)clrLine);
    if (tbl.fValid()) {
      int iCols = view->iReadElement(sSection, "Columns");
      cacarr.Resize(iCols);
      for (int i = 0; i < iCols; ++i) {
        String sCol("Column %i", i);
        String sClr("Color %i", i);
        String sColName;
        view->ReadElement(sSection, sCol.scVal(), sColName);
        cacarr[i].col = tbl->col(sColName);
        view->ReadElement(sSection, sClr.scVal(), cacarr[i].clr);
      }
    }
  }
}

void PointMapDrawer::WriteLayer(MapView& view, const char* sSection)
{
  view->WriteElement(sSection, "Type", "PointMapDrawer");
  view->WriteElement(sSection, "PointMap", mp);
  BaseMapDrawer::WriteLayer(view, sSection);
  view->WriteElement(sSection, "ShowSymbol", fSymbol);
  smb.WriteLayer(view, sSection);
  view->WriteElement(sSection, "Stretch", fStretch);
  if (fStretch) {
    view->WriteElement(sSection, "MinSize", (long)iMinSize);
    view->WriteElement(sSection, "Range", rrStretch);
    String sLinLog;
    switch (iLinLog) {
      case 0:
        sLinLog = "Linear";
        break;
      case 1:
        sLinLog = "Logarithmic";
        break;
    }
    view->WriteElement(sSection, "LinLog", sLinLog);
    String sRadiusArea;
    switch (iRadiusArea) {
      case 0:
        sRadiusArea = "Radius";
        break;
      case 1:
        sRadiusArea = "Area";
        break;
    }
    view->WriteElement(sSection, "RadiusArea", sRadiusArea);
  }
  view->WriteElement(sSection, "Size", (long)iSize);
  view->WriteElement(sSection, "Text", fText);
  if (fText) 
  {
    view->WriteElement(sSection, "TextColor", clrText);
    view->WriteElement(sSection, "FontType", sFaceName);
//    int iTmp = round(iFHeight / (mppn->canvas()->pixPerInchY() / 720.0));
    view->WriteElement(sSection, "FontSize", iFHeight);
    view->WriteElement(sSection, "FontWeight", iFWeight);
    view->WriteElement(sSection, "FontStyle", iFStyle);
  }
  if (fMask)
    view->WriteElement(sSection, "Mask", mask.sMask());
  else  
    view->WriteElement(sSection, "Mask", (char*)0);

  if (colCls.fValid())
    view->WriteElement(sSection, "Column Class", colCls->sName());
  else
    view->WriteElement(sSection, "Column Class", (char*)0);
  if (colRot.fValid())
    view->WriteElement(sSection, "Column Rotation", colRot->sName());
  else
    view->WriteElement(sSection, "Column Rotation", (char*)0);
  if (colClr.fValid())
    view->WriteElement(sSection, "Column Color", colClr->sName());
  else
    view->WriteElement(sSection, "Column Color", (char*)0);
  if (colVal.fValid())
    view->WriteElement(sSection, "Column Value", colVal->sName());
  else
    view->WriteElement(sSection, "Column Value", (char*)0);
  if (colWidth.fValid())
    view->WriteElement(sSection, "Column Width", colWidth->sName());
  else
    view->WriteElement(sSection, "Column Width", (char*)0);
  if (colTxt.fValid())
    view->WriteElement(sSection, "Column Text", colTxt->sName());
  else
    view->WriteElement(sSection, "Column Text", (char*)0);
  if (colJst.fValid())
    view->WriteElement(sSection, "Column Justification", colJst->sName());
  else
    view->WriteElement(sSection, "Column Justification", (char*)0);

  if (drm == drmGRAPH) {
    switch (eST) {
      case stPIECHART:
        view->WriteElement(sSection, "Graph Type", "PieChart");
        break;
      case stBARGRAPH:
        view->WriteElement(sSection, "Graph Type", "BarGraph");
        break;
      case stGRAPH:
        view->WriteElement(sSection, "Graph Type", "LineGraph");
        break;
      case stCOMPBAR:
        view->WriteElement(sSection, "Graph Type", "CompositeBar");
        break;
      case stVOLCUBES:
        view->WriteElement(sSection, "Graph Type", "VolumeCubes");
        break;
    }
    view->WriteElement(sSection, "Width", (long)iWidth);
    view->WriteElement(sSection, "Line Color", clrLine);
    long iCols = cacarr.iSize();
    view->WriteElement(sSection, "Columns", iCols);
    for (int i = 0; i < iCols; ++i) {
      String sCol("Column %i", i);
      String sClr("Color %i", i);
      String sColName = cacarr[i].col->sName();
      view->WriteElement(sSection, sCol.scVal(), sColName);
      view->WriteElement(sSection, sClr.scVal(), cacarr[i].clr);
    }
  }
}


PointMapDrawer::~PointMapDrawer()
{
  ILWISSingleLock csl(&cs, TRUE, SOURCE_LOCATION);
}

bool PointMapDrawer::fPntInMask(long iNr) const
{
  if (fMask) {
    return mp->fPntInMask(iNr, mask);
  }
  return true;
}

int PointMapDrawer::iStretchSize(double rVal, double rMin, double rMax) const
{
  return (int)rounding(rStretchSize(rVal, rMin, rMax));
}

double PointMapDrawer::rStretchSize(double rVal, double rMin, double rMax) const
{
  if (iLinLog) {
    if (rMin < 1e-10)
      rMin = 1;
    rMin = log(rMin);
    if (rMax < 1e-10)
      rMax = 1;
    rMax = log(rMax);
  }
  rVal -= rMin;
  if (rMax <= rMin)
    rMax = rMin + 1;
  rVal /= (rMax - rMin);
  if (rVal < 0)
    rVal = 0;
  else if (rVal > 1)
    rVal = 1;
  double rSiz;
  if (iRadiusArea) {
    rMin = iMinSize;
    rMax = iSize;
    rVal = sqrt(rMin * rMin + rVal * (rMax * rMax - rMin * rMin));
    rSiz = rVal;
  }
  else {
    rSiz = iMinSize + rVal * (iSize - iMinSize);
  }
  if (rSiz < iMinSize)
    rSiz = iMinSize;
  if (rSiz > iSize)
    rSiz = iSize;
  return rSiz;
}

int PointMapDrawer::draw(CDC* cdc, zRect rect, Positioner* psn, volatile bool* fDrawStop)
{
  ILWISSingleLock csl(&cs, TRUE, SOURCE_LOCATION);
  if (!fAct) return 0;
  mp->KeepOpen(true);
  CoordSystem cs = mcd->georef->cs();
  bool fSameCsy = mp->cs() == cs;

	bool fPalette = cdc->GetDeviceCaps(RASTERCAPS) & RC_PALETTE ? true : false;

  CoordBounds cb = cbRect(psn);
  bool fBoundCheck = !cb.fUndef();
	LOGFONT lf;
	lf.lfHeight = -iFHeight;
	lf.lfWidth = 0;
	lf.lfEscapement = 0;
	lf.lfOrientation = 0;
	lf.lfWeight = iFWeight;
	lf.lfItalic =(BYTE)(iFStyle&FS_ITALIC) ;
	lf.lfUnderline = (BYTE)(iFStyle&FS_UNDERLINE);
	lf.lfStrikeOut = (BYTE)(iFStyle&FS_STRIKEOUT);
	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfOutPrecision = OUT_CHARACTER_PRECIS;
	lf.lfClipPrecision = CLIP_CHARACTER_PRECIS;
	lf.lfQuality = DEFAULT_QUALITY;
	lf.lfPitchAndFamily = VARIABLE_PITCH;
	strcpy(lf.lfFaceName, sFaceName.sVal());
	CFont fntText;
	fntText.CreatePointFontIndirect(&lf);
	//  zFont fntText(sFaceName, zDimension(0, iFHeight), iFWeight,
//  zFont fntText(sFaceName, zPrPoint(0, -iFHeight, dsp), iFWeight,
//                            ffDontCare, VariablePitch, iFStyle);
//  if (dsp->type() == MetaDisp) {
//    fntText = zFont(sFaceName, zDimension(0, -iFHeight), iFWeight,
//                            ffDontCare, VariablePitch, iFStyle);
//  }
  if (fBoundCheck) {
    double rW10 = cb.width() / 10;
    double rH10 = cb.height() /10;
    cb.MinX() -= rW10;
    cb.MaxX() += rW10;
    cb.MinY() -= rH10;
    cb.MaxY() += rH10;
    if (!fSameCsy) {
      CoordBounds cbMap;
      double dX = cb.width() / 10;
      double dY = cb.height() / 10;
      Coord c;
      int i, j;
      for (i = 0, c.x = cb.MinX(); i < 10; c.x += dX, ++i)
        for (j = 0, c.y = cb.MinY(); j < 10; c.y += dY, ++j) 
          cbMap += mp->cs()->cConv(mcd->georef->cs(), c);
      cb = cbMap;
    }  
  }
  Color c, fc;
  if (colTxt.fValid())
    fText = true;
	CFont* fntOld;
  if (fText) {
    if ((long)clrText == -1)
      c = SysColor(COLOR_WINDOWTEXT);
    else
      c = clrText;
		
		cdc->SetTextColor(c);
		cdc->SetBkMode(TRANSPARENT);
		fntOld = cdc->SelectObject(&fntText);
    if (fSymbol)
			cdc->SetTextAlign(TA_LEFT|TA_TOP);  // = default
    else
      cdc->SetTextAlign(TA_CENTER|TA_BASELINE);
  }
	riTranquilizer = RangeInt(1,mp->iFeatures());
  for (long r = 0; r < mp->iFeatures(); ++r) {
		iTranquilizer = r;
		if (*fDrawStop)
			break;
    Coord crd = mp->cValue(r);
    if (crd.fUndef()) continue;
    if (fBoundCheck && !cb.fContains(crd)) continue;
    if (fMask) {
      if (!fPntInMask(r))
        continue;
    }
    if (!fSameCsy) 
      crd = cs->cConv(mp->cs(), crd);
    zPoint pnt = psn->pntPos(crd);
    zPoint pntText;
    
    long iRaw = mp->iRaw(r);
    double rVal;
    if (mp->dm()->pdv()) {
      rVal = mp->rValue(r);
      if (rVal == rUNDEF) continue;
    }  
    else {
      if (iRaw == iUNDEF)
        continue;
      if (fAttTable) {
        if (colAtt->dm()->pdv()) {
          rVal = colAtt->rValue(iRaw);
          iRaw = colAtt->iRaw(iRaw);
          if (rVal == rUNDEF) continue;
        }
        else if (colAtt->dm()->fRawAvailable())
        {
          iRaw = colAtt->iRaw(iRaw);
          if (iRaw == iUNDEF)
            continue;
				}
      }
    }

    if (fSymbol) {
      switch (drm) {
        case drmBYATTRIBUTE:
          if (!colCls.fValid())
            drm = drmSINGLE;
          break;
        case drmARROW:
          if (!colRot.fValid())
            drm = drmSINGLE;
          break;
      }
      switch (drm) {
        case drmSINGLE: {
          if (_dm->pdv() && fStretch) {
            double rMin, rMax;
            if (_dm->pdvi()) {
              rMin = riStretch.iLo();
              rMax = riStretch.iHi();
            }
            else {
              rMin = rrStretch.rLo();
              rMax = rrStretch.rHi();
            }
            smb.iSize = iStretchSize(rVal, rMin, rMax);
          }
          smb.drawSmb(cdc,0,pnt);
          pntText = smb.pntText(cdc, pnt);
        }  break;
        case drmBOOL:
          switch (iRaw) {
            case 0: continue;
            case 1: c = clrDraw(clr0); break;
            case 2: c = clrDraw(clr);  break;
          }
          smb.SetColor(c);
          smb.drawSmb(cdc,0,pnt);
          pntText = smb.pntText(cdc, pnt);
          break;
        case drmCOLOR:
					c = clrDraw(iRaw);
          smb.SetColor(c);
          smb.drawSmb(cdc,0,pnt);
          pntText = smb.pntText(cdc, pnt);
          break;
        case drmBYATTRIBUTE:
        {
          long iClass = colCls->iRaw(iRaw);
          //c = (zColor)_rpr->clrRaw(iClass);
          ExtendedSymbol sym(_rpr,iClass);
          if (colRot.fValid())
            sym.rRotation = colRot->rValue(iRaw);
          if (colClr.fValid()) {
            long iClr = colClr->iRaw(iRaw);
            if (0 != colClr->dm()->pdcol())
              c = (Color)iClr;
            else if (0 != colClr->dm()->pdc() || 0 != colClr->dm()->pdp()) {
              Representation rpr = colClr->dm()->rpr();
              if (rpr.fValid())
                c = rpr->clrRaw(iClr);
            }
            sym.SetColor(c);
          }
          if (fStretch && colVal.fValid()) {
            double rMin, rMax;
            rMin = rrStretch.rLo();
            rMax = rrStretch.rHi();
            rVal = colVal->rValue(iRaw);
            sym.iSize = iStretchSize(rVal, rMin, rMax);
          }
          else if (iSize > 0)
            sym.iSize = iSize;
          sym.drawSmb(cdc,0,pnt);
          pntText = sym.pntText(cdc, pnt);
        } break;
        case drmARROW:
        {
          double rLength = 10;
          if (colVal.fValid())
            rLength = colVal->rValue(iRaw);
          double rPixLength = 10 * rLength; // mm
          double rTop = 15; // 5 pt

          c = SysColor(COLOR_WINDOWTEXT);
          if (colClr.fValid()) {
            long iClr = colClr->iRaw(iRaw);
            if (0 != colClr->dm()->pdcol())
              c = (Color) iClr;
            else if (0 != colClr->dm()->pdc() || 0 != colClr->dm()->pdp()) {
              Representation rpr = colClr->dm()->rpr();
              if (rpr.fValid())
                c = rpr->clrRaw(iClr);
            }
          }

          int iWidth = 1;
          if (colWidth.fValid()) {
            double rWidth = colWidth->rValue(iRaw); // mm
            rWidth *= 10;
            iWidth = (int)rounding(rWidth);
          }
          double rRot = colRot->rValue(iRaw);

          CPen pen(PS_SOLID,iWidth,c);
					CPen* penOld = cdc->SelectObject(&pen);
					zPoint p = pnt;
					cdc->MoveTo(p);

          double rRotRad = rRot * M_PI / 180.0;
          p.x += (long)rounding(rPixLength * sin(rRotRad));
          p.y -= (long)rounding(rPixLength * cos(rRotRad));
					cdc->LineTo(p);
          zPoint pTop = p;

          double rAngle = 45;
          double rAngleRad = rAngle * M_PI / 180.0;
          p.x -= (long)rounding(rTop * sin(rRotRad+rAngleRad));
          p.y += (long)rounding(rTop * cos(rRotRad+rAngleRad));
					cdc->LineTo(p);
          p = pTop;
					cdc->MoveTo(p);
          p.x -= rounding(rTop * sin(rRotRad-rAngleRad));
          p.y += rounding(rTop * cos(rRotRad-rAngleRad));
					cdc->LineTo(p);
					cdc->SelectObject(penOld);
          pntText = pnt;
        } break;
        case drmGRAPH:
        {
          double rSiz = iSize;
          if (fStretch && colVal.fValid()) {
            double rMin, rMax;
            rMin = rrStretch.rLo();
            rMax = rrStretch.rHi();
            rVal = colVal->rValue(iRaw);
            rSiz = rStretchSize(rVal, rMin, rMax);
          }
          int iPixSize = (int)(3 * rSiz);
          int iPixWidth = 3 * iWidth;
					CPen pen;
          if ((long)clrLine == -2)
						pen.CreatePen(PS_NULL,0,(Color)0);
          else {
            Color c;
            if ((long)clrLine == -1)
              c = SysColor(COLOR_WINDOWTEXT);
            else
              c = clrLine;
						pen.CreatePen(PS_SOLID,1,c);
          }
					CPen* penOld = cdc->SelectObject(&pen);

          zPoint p1, p2;
          double rSum = 0;
          int iNum = 0; // if only one active, no line to middle
          for (unsigned int i = 0; i < cacarr.iSize(); ++i) {
            double r = cacarr[i].col->rValue(iRaw);
            if (r > 0) {
              rSum += r;
              ++iNum;
            }
          }

          switch (eST) {
            case stPIECHART:
              if (rSum > 0) {
                p1 = pnt;
                p1.y -= iPixSize;
                CPoint pArr[400];
                double rAngle = 0;
                for (unsigned i = 0; i < cacarr.iSize(); ++i) 
								{
                  double r = cacarr[i].col->rValue(iRaw);
                  if (r > 0) {
                    double rOldAngle = rAngle;
                    rAngle += (r / rSum) * 2 * M_PI;
                    p2 = pnt;
                    p2.y -= (long)rounding(cos(rAngle) * iPixSize);
                    p2.x += (long)rounding(sin(rAngle) * iPixSize);
										CBrush brush(cacarr[i].clr);
                    CBrush *oldBrush = cdc->SelectObject(&brush);
                    int i = 0;
                    if (iNum > 1)
                      pArr[i++] = pnt;
                    pArr[i++] = p1;
                    CPoint p;
                    for (double r = rOldAngle; r < rAngle; r += 0.02) {
                      p = pnt;
                      p.y -= (long)rounding(cos(r) * iPixSize);
                      p.x += (long)rounding(sin(r) * iPixSize);
                      pArr[i++] = p;
                    }
                    pArr[i++] = p2;
                    if (iNum > 1)
                      pArr[i++] = pnt;
                    cdc->Polygon(pArr, i);
                    p1 = p2;
										cdc->SelectObject(oldBrush);
                  }
                }
                pntText = pnt;
                pntText.x += (long)(M_SQRT2 * iPixSize / 2);
                pntText.y += (long)(M_SQRT2 * iPixSize / 2);
              }
              else
                pntText = zPoint(shUNDEF,shUNDEF);
              break;
            case stBARGRAPH:
              {
                double rMin = rrStretch.rLo();
                double rMax = rrStretch.rHi();
                int iCols = cacarr.iSize();
                p1 = pnt;
                p1.x -= iCols * iPixWidth / 2;
                p2 = p1;
                p2.x += iCols * iPixWidth + 1;
                cdc->MoveTo(p1);
                cdc->MoveTo(p2);
                pntText = p1;
                p1.y += 1;
                for (int i = 0; i < iCols; ++i) {
                  double rVal = cacarr[i].col->rValue(iRaw);
                  rVal -= rMin;
                  if (rMax <= rMin)
                    rMax = rMin + 1;
                  rVal /= (rMax - rMin);
                  if (rVal < 0)
                    rVal = 0;
                  else if (rVal > 1)
                    rVal = 1;
                  rVal *= iPixSize;
                  int iY = rounding(rVal);
                  p2 = p1;
                  p2.x += iPixWidth + 1;
                  p2.y -= iY;
									CBrush brush(cacarr[i].clr);
                  CBrush *oldBrush = cdc->SelectObject(&brush);
                  cdc->Rectangle(CRect(p1, p2));
                  cdc->SelectObject(&brush);
                  p1.x += iPixWidth;
                }
              } break;
            case stGRAPH:
              {
                double rMin = rrStretch.rLo();
                double rMax = rrStretch.rHi();
                int iCols = cacarr.iSize();
                p1 = pnt;
                p1.x -= (iCols-1) * iPixWidth / 2;
                p2 = p1;
                p2.x += (iCols-1) * iPixWidth + 1;
                cdc->MoveTo(p1);
                cdc->LineTo(p2);
                p2 = p1;
                p2.y -= iPixSize;
                cdc->MoveTo(p1);
                cdc->LineTo(p2);
                pntText = p1;
                bool fLast = false;
								CPen pen(PS_SOLID, 1, smb.color());
								CPen *oldPen = cdc->SelectObject(&pen);
                for (int i = 0; i < iCols; ++i) {
                  double rVal = cacarr[i].col->rValue(iRaw);
                  rVal -= rMin;
                  if (rMax <= rMin)
                    rMax = rMin + 1;
                  rVal /= (rMax - rMin);
                  if (rVal < 0)
                    rVal = 0;
                  else if (rVal > 1)
                    rVal = 1;
                  rVal *= iPixSize;
                  int iY = rounding(rVal);
                  p2 = p1;
                  p2.y -= iY;
                  if (fLast)
                    cdc->LineTo(p2);
                  else
                    cdc->MoveTo(p2);
                  p1.x += iPixWidth;
                  fLast = true;
                }
                cdc->SelectObject(oldPen);
              } break;
            case stCOMPBAR:
              {
                double rMin = rrStretch.rLo();
                double rMax = rrStretch.rHi();
                int iCols = cacarr.iSize();
                p1 = pnt;
                p1.x -= iPixWidth / 2;
                pntText = p1;
                for (int i = 0; i < iCols; ++i) {
                  double rVal = cacarr[i].col->rValue(iRaw);
                  rVal -= rMin;
                  if (rMax <= rMin)
                    rMax = rMin + 1;
                  rVal /= (rMax - rMin);
                  if (rVal < 0)
                    rVal = 0;
                  else if (rVal > 1)
                    rVal = 1;
                  rVal *= iPixSize;
                  int iY = rounding(rVal);
                  p2 = p1;
                  p2.x += iPixWidth + 1;
                  p2.y -= iY;
									CBrush brush(cacarr[i].clr);
                  CBrush *oldBrush = cdc->SelectObject(&brush);
                  cdc->Rectangle(CRect(p1, p2));
                  cdc->SelectObject(oldBrush);
                  p1.y -= iY - 1;
                }
              } break;
            case stVOLCUBES:
              {
                double rMin = rrStretch.rLo();
                double rMax = rrStretch.rHi();
                if (rMax <= rMin)
                  rMax = rMin + 1;
                rSum = 0;
                for (unsigned int i = 0; i < cacarr.iSize(); ++i) {
                  double rVal = cacarr[i].col->rValue(iRaw);
                  rVal -= rMin;
                  rVal /= (rMax - rMin);
                  if (rVal < 0)
                    rVal = 0;
                  else if (rVal > 1)
                    rVal = 1;
                  if (rVal > 0)
                    rSum += pow(rVal,1.0/3.0);
                }
                p1 = pnt;
                p1.x -= rSum * iPixWidth / 2;
                pntText = p1;
                int iCols = cacarr.iSize();
                p1.y += 1;
                for (int i = 0; i < iCols; ++i) {
                  double rVal = cacarr[i].col->rValue(iRaw);
                  rVal -= rMin;
                  rVal /= (rMax - rMin);
                  if (rVal < 0)
                    rVal = 0;
                  else if (rVal > 1)
                    rVal = 1;
                  rVal = pow(rVal, 1.0/3.0);
                  rVal *= iPixSize;
                  int iY = rounding(rVal);
                  zPoint pArr[12];
                  int x1 = p1.x;
                  int x2 = x1 + (int)(0.5 * iY);
                  int x3 = x1 + iY;
                  int x4 = x2 + iY;
                  int y1 = p1.y;
                  int y2 = y1 - (int)(0.5 * iY);
                  int y3 = y1 - iY;
                  int y4 = y2 - iY;
									CBrush brush(cacarr[i].clr);
                  CBrush *oldBrush = cdc->SelectObject(&brush);
                  pArr[0] = p1;
                  pArr[1] = zPoint(x3,y1);
                  pArr[2] = zPoint(x3,y3);
                  pArr[3] = zPoint(x1,y3);
                  cdc->Polygon(pArr, 4);
                  pArr[0] = zPoint(x2,y4);
                  pArr[1] = zPoint(x4,y4);
                  cdc->Polygon(pArr, 4);
                  pArr[0] = zPoint(x4,y2);
                  pArr[3] = zPoint(x3,y1);
                  cdc->Polygon(pArr, 4);
									cdc->SelectObject(oldBrush);
                  p1.x = x3;
                }
              } break;
          } // switch (eST)

          cdc->SelectObject(penOld);
        } break;
        default:
        {
          c = _rpr->clrRaw(iRaw);
          ExtendedSymbol sym(_rpr,iRaw);
          //Symbol sym(rpr,iRaw);
          sym.drawSmb(cdc,0,pnt);
          pntText = sym.pntText(cdc, pnt);
        } break;
      }
    }
    else
      pntText = pnt;
    if (fText) {
      String sText;
      long iRaw = r;
      if (colTxt.fValid()) {
        iRaw = mp->iRaw(iRaw);
        sText = colTxt->sValue(iRaw,0);
      }
      else if (fAttTable) {
        iRaw = mp->iRaw(iRaw);
        sText = colAtt->sValue(iRaw,0); 
      }  
      else  
        sText = mp->sValue(iRaw,0);
      if (fSymbol)
        cdc->SetTextAlign(TA_LEFT|TA_TOP);  // = default
      if (pntText != zPoint(shUNDEF,shUNDEF))
        cdc->TextOut(pntText.x,pntText.y,sText.sVal());
    }
  }
  if (fText)
		cdc->SelectObject(fntOld);
  mp->KeepOpen(false);
  return 0;
}

int PointMapDrawer::Configure(bool fShowForm)
{
  bool fShow = fAct;
  bool fOk;
  if (fNew) {
    fAct = false;  // to prevent "in between" drawing
    fNew = false;
  }
  {

		iSize /= RepresentationClass::iSIZE_FACTOR;
		smb.Resize(1.0/RepresentationClass::iSIZE_FACTOR);
		iMinSize /= RepresentationClass::iSIZE_FACTOR;
		iFHeight /= RepresentationClass::iSIZE_FACTOR;

    PointMapDrawerForm frm(this, fShowForm);

		smb.Resize(RepresentationClass::iSIZE_FACTOR);
		iSize *= RepresentationClass::iSIZE_FACTOR;
		iMinSize *= RepresentationClass::iSIZE_FACTOR;
		iFHeight *= RepresentationClass::iSIZE_FACTOR;

    fOk = frm.fOkClicked();
    if (fOk)
      fShow = true;
  }  	     
  if (fOk) {
    fShow = true;
    Setup();
  }
  fAct = fShow;
  return fOk;
}

int PointMapDrawer::Setup()
{
  return 0;
}

String PointMapDrawer::sInfo(const Coord& crd)
{
//  mp->SetProximity(1e20);
  long iRec = mp->iRec(crd);

/*  Coord cPnt = map->cValue(iRec);
  double rDist = rDist2(crd, cPnt);
  double rRow, rCol;
  Coord c2;
  mcd->georef->Coord2RowCol(crd, rRow, rCol);
  zPoint pnt = mppn->mps->pntPos(rRow, rCol);
  int iDiff = M_SQRT_2/2 * smb.iSize;
  if (iDiff == 0) iDiff = 1;
  pnt.x() += iDiff + 1;
  pnt.y() += iDiff + 1;
  mppn->mps->Pnt2RowCol(pnt, rRow, rCol);
  mppn->georef->RowCol2Coord(rRow, rCol, c2);
  double rD = rDist2(crd,c2);
  map->SetProximity(rD);*/

  String s;
//  if (rDist < rD) {
    s = mp->sValue(iRec,0);
    if (s == "?")
      return "";
    if (fMask)
      if (!fPntInMask(iRec))
        return "";
    if (fAttTable) {  
      long iRaw = mp->iRaw(iRec);
      if (iRaw != iUNDEF) {
        String sAtt = colAtt->sValue(iRaw);
        for (int iLen = sAtt.length(); iLen && sAtt[iLen-1] == ' '; --iLen)
          sAtt[iLen-1] = 0;
        s &= ": ";
        s &= sAtt;
      }  
    }  
//  }    
  return s;
}

/*
void PointMapDrawer::Edit()
{
  mppn->PointEditStr(map->sName());
}

Record PointMapDrawer::rec(const Coord& crd)
{
  return recFromMap(map, crd);
}
*/

FormBaseDialog* PointMapDrawer::wEditRpr(CWnd* wPar, int iRaw)
{
  if ((iRaw > 0) && _rpr.fValid()) {
    RepresentationClass* rpc = _rpr->prc();
    if (rpc) 
      return new PointMapRprEditForm(wPar, rpc, iRaw, true);
  }
  return 0;
}

FormBaseDialog* PointMapDrawer::wEditRpr(CWnd* wPar, const Coord& crd)
{
	vector<long> raws = mp->viValue(crd);
	if ( raws.size() == 1)
		return wEditRpr(wPar, raws[0]);
	return NULL;
}

IlwisObject PointMapDrawer::obj() const
{
  return mp;
}

zIcon PointMapDrawer::icon() const
{
	return zIcon("PntIcon");
}

void PointMapDrawer::DrawLegendRect(CDC* cdc, CRect rect, int iRaw) const
{
	CPoint pnt;
	pnt.x = (rect.left + rect.right) / 2;
	pnt.y = (rect.top + rect.bottom) / 2;
	if (_rpr.fValid()) {
		ExtendedSymbol sym(_rpr,iRaw);
		sym.Resize(1.0/RepresentationClass::iSIZE_FACTOR); // To make the sizes in the MapWindow conform to the Layout
	  sym.drawSmb(cdc,0,pnt);
	}
	else {
		Color c;
		if (drmBOOL == drm) {
      switch (iRaw) {
        case 0: return;
        case 1: c = clr0; break;
        case 2: c = clr;  break;
      }
			ExtendedSymbol sym = smb;
      sym.SetColor(c);
			sym.Resize(1.0/RepresentationClass::iSIZE_FACTOR); // To make the sizes in the MapWindow conform to the Layout
		  sym.drawSmb(cdc,0,pnt);
		}
	}
} 

void PointMapDrawer::DrawValueLegendRect(CDC* cdc, CRect rect, double rVal) const
{
	CPoint pnt;
	pnt.x = (rect.left + rect.right) / 2;
	pnt.y = (rect.top + rect.bottom) / 2;
	ExtendedSymbol sym = smb;
	switch (drm) {
    case drmSINGLE: {
      if (_dm->pdv() && fStretch) {
        double rMin, rMax;
        if (_dm->pdvi()) {
          rMin = riStretch.iLo();
          rMax = riStretch.iHi();
        }
        else {
          rMin = rrStretch.rLo();
          rMax = rrStretch.rHi();
        }
        sym.iSize = iStretchSize(rVal, rMin, rMax);
      }
			sym.Resize(1.0/RepresentationClass::iSIZE_FACTOR); // To make the sizes in the MapWindow conform to the Layout
      sym.drawSmb(cdc,0,pnt);
		}	break;
		// other options ?
	}
}

bool PointMapDrawer::fLegendUseful() const
{
	if (_dm->pdv() && fStretch) 
		return true;
	else
		return BaseMapDrawer::fLegendUseful();
}

