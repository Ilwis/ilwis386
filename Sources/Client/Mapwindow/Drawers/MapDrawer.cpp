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
// MapDrawer.cpp: implementation of the MapDrawer class.
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
#include "Client\FormElements\fldcol.h"
#include "Client\FormElements\fldrpr.h"
#include "Client\FormElements\fldcolor.h"
#include "Client\Mapwindow\Drawers\BaseMapDrawer.h"
#include "Client\Mapwindow\Drawers\MapDrawer.h"
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
//#include "Client\Editors\Utils\BaseBar.h"
#include "Client\Editors\Utils\sizecbar.h"
#include "Client\Editors\Utils\SYMBOL.H"
#include "Client\Editors\Utils\Smbext.h"
#include "Client\Editors\Utils\line.h"
#include "Client\Editors\Representation\RprEditForms.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(MapDrawer, BaseMapDrawer)
	//{{AFX_MSG_MAP(MapDrawer)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void MapDrawer::init()
{
  lpbi = (LPBITMAPINFO) new char[sizeof(BITMAPINFOHEADER)];
  lpbi->bmiHeader.biSize  = sizeof(BITMAPINFOHEADER);
  lpbi->bmiHeader.biWidth = 8192;
  lpbi->bmiHeader.biHeight = 1;
  lpbi->bmiHeader.biPlanes = 1;
  lpbi->bmiHeader.biBitCount = 32;
  lpbi->bmiHeader.biCompression = BI_RGB;
  lpbi->bmiHeader.biSizeImage = 0;
  lpbi->bmiHeader.biClrUsed = 0;
  lpbi->bmiHeader.biClrImportant = 0;

  fImage = 0 != _dm->pdi();
  fDeletePossible = false;
  fText = false;
  clrText = SysColor(COLOR_WINDOWTEXT); 
	fBands = false;
}

MapDrawer::MapDrawer(MapCompositionDoc* mapcd, const Map& _map)
: BaseMapDrawer(mapcd)
, colIndex(0)
{
  mp = _map;
  bmap = mp.ptr();
  _dm = mp->dm();
  _rpr = _map->dm()->rpr();
  init();
  if (fImage)
    drm = drmIMAGE;
  ValueRange vr = mp->vr();
  if (mp->dm()->pdbool())
    vr = ValueRange();
  if (vr.fValid() || fImage) {
    fStretch = true;
    if (!fImage && vr->vrr()) {
      rrStretch = mp->rrPerc1(true);
      if (rrStretch.rLo() >= rrStretch.rHi())
        rrStretch = mp->rrMinMax();
      if (rrStretch.rLo() >= rrStretch.rHi())
        rrStretch = vr->rrMinMax();
      riStretch.iLo() = (long)(rounding(rrStretch.rLo()));
      riStretch.iHi() = (long)(rounding(rrStretch.rHi()));
    }
    else {
      riStretch = mp->riPerc1(true);
      if (riStretch.iLo() >= riStretch.iHi())
        riStretch = mp->riMinMax();
      if (riStretch.iLo() >= riStretch.iHi())
        if (fImage)
          riStretch = RangeInt(0,255);
        else if (vr.fValid())
          riStretch = vr->riMinMax();
      rrStretch.rLo() = doubleConv(riStretch.iLo());
      rrStretch.rHi() = doubleConv(riStretch.iHi());
    }
  }
  if (0 != _dm->pdid())
    drm = drmMULTIPLE;
  else if (0 != _dm->pdp()) {
    drm = drmRPR;
    fSelectable = false;
  }

  String sStretchMethod;
  ObjectInfo::ReadElement("Display", "Stretching", mp->fnObj, sStretchMethod);
  if ("Linear" == sStretchMethod)
    sm = smLINEAR;
  else if ("Logarithmic" == sStretchMethod)
    sm = smLOGARITHMIC;
}

MapDrawer::MapDrawer(MapCompositionDoc* mapcd, const MapView& view, const char* sSection)
: BaseMapDrawer(mapcd, view, sSection, "Map")
, colIndex(0)
{
  view->ReadElement(sSection, "Map", mp);
  if (mp->dm()->pdcol()) 
    drm = drmCOLOR;
  bmap = mp.ptr();
  init();
  view->ReadElement(sSection, "Text", fText);
  view->ReadElement(sSection, "TextColor", (Color&)clrText);
}


void MapDrawer::WriteLayer(MapView& view, const char* sSection)
{
  view->WriteElement(sSection, "Type", "MapDrawer");
  view->WriteElement(sSection, "Map", mp);
  view->WriteElement(sSection, "Text", fText);
  if (fText) 
  {
    view->WriteElement(sSection, "TextColor", clrText);
  }  
  BaseMapDrawer::WriteLayer(view, sSection);
}

MapDrawer::~MapDrawer()
{
  ILWISSingleLock csl(&cs, TRUE, SOURCE_LOCATION);
  delete lpbi;
  delete colIndex;
}

void MapDrawer::SetMap(const Map& _map) 
{ 
  if (mp->dm() != _map->dm())
    _rpr = _map->dm()->rpr();
  mp = _map; 
  bmap = mp.ptr();
  if (_map->fDependent())
    if (!_map->fCalculated())
      _map->Calc();
}

void MapDrawer::PutLine(CDC* cdc, zRect rect, int iLine, const LongBuf& buf)
{
  long iSize = buf.iSize();
  long i;
  char *cBuf = new char[iSize * 4];
  char *c = cBuf;
  for (i = 0; i < iSize; ++i) {
    Color col = Color(buf[i]);
    *c++ = col.blue();
    *c++ = col.green();
    *c++ = col.red();
    *c++ = (unsigned char)255;
  }
	int iYOffset = fBands ? 0 : rect.top();
	SetDIBitsToDevice(*cdc, rect.left(), iYOffset + iLine,
	                  rect.width(), 1, 0, 0,
	                  0, 1, cBuf, lpbi, DIB_RGB_COLORS);
  delete cBuf;
}

void MapDrawer::ConvLine(LongBuf& buf)
{
  long iLen = buf.iSize();

  // handle attribute table
  if (fAttTable) 
  {
    if (colAtt->fValues() && drm==drmRPR) 
    {
      RealBuf rb(iLen);
      for (long i = 0; i < iLen; ++i)
        rb[i] = colAtt->rValue(buf[i]);
      ConvLine(rb, buf);
      return;
    }
    else 
    {
      for (long i = 0; i < iLen; ++i)
        buf[i] = colAtt->iRaw(buf[i]);
    }
  }

  if (fValue && drm == drmRPR) {
    for (long i = 0; i < iLen; ++i)
      if (iUNDEF == buf[i])
        buf[i] = mcd->colBackground;
      else
        buf[i] = clrVal(buf[i]);
  }
  else switch (drm) {
    long i;
    case drmIMAGE:
    case drmRPR:
    case drmBOOL:
      for (i = 0; i < iLen; ++i)
        if (buf[i] != iUNDEF)
          buf[i] = colIndex[buf[i] % 256];
        else
          buf[i] = mcd->colBackground;
      break;
    case drmSINGLE:
      for (i = 0; i < iLen; ++i)
        if (buf[i] != 0 && buf[i] != iUNDEF)
          buf[i] = colIndex[1];
        else
          buf[i] = mcd->colBackground;
      break;
    case drmMULTIPLE:
      switch (iMultColors) {
        case 0:
          for (i = 0; i < iLen; ++i)
            if (buf[i] != iUNDEF)
              buf[i] = colIndex[1 + buf[i] % 7];
            else
              buf[i] = mcd->colBackground;
          break;
        case 1:
          for (i = 0; i < iLen; ++i)
            if (buf[i] != iUNDEF)
              buf[i] = colIndex[1 + buf[i] % 15];
            else
              buf[i] = mcd->colBackground;
          break;
        case 2:
          for (i = 0; i < iLen; ++i)
            if (buf[i] != iUNDEF)
              buf[i] = colIndex[1 + buf[i] % 31];
            else
              buf[i] = mcd->colBackground;
          break;
        case 3:
          for (i = 0; i < iLen; ++i)
            if (buf[i] != iUNDEF)
              buf[i] = clrDraw(clrRandom(buf[i]));  
            else
              buf[i] = mcd->colBackground;
          break;
      }
      break;
  }
}

void MapDrawer::ConvLine(const RealBuf& rb, LongBuf& lb)
{
  long iLen = rb.iSize();
  for (long i = 0; i < iLen; ++i)
    if (rUNDEF == rb[i])
        lb[i] = mcd->colBackground;
      else
        lb[i] = clrVal(rb[i]);
}

String MapDrawer::getForeignType() {
	String s;
	int n = ObjectInfo::ReadElement("ForeignFormat", "Expression", mp->fnObj, s);
	if ( n <= 0 )
		return "";
	return s.sHead("(");

}

int MapDrawer::draw(CDC* cdc, zRect rect, Positioner* psn, volatile bool* fDrawStop)
{
	ILWISSingleLock csl(&cs, TRUE, SOURCE_LOCATION);
	if (!fAct) return 0;
	// restrict to invalidated area
	MinMax mm(psn->rcPos(rect.TopLeft()), psn->rcPos(rect.BottomRight()));
	if (mm.width() == 0 || mm.height() == 0) return 0;
	long iMapLine;
	short iDspLine;
	double rScale = psn->rSc();
	double rFact = rScale > 0 ? 1/rScale : -rScale;
	RowCol rcSize = mp->rcSize();
	long iCols = rcSize.Col;
	long iLines = rcSize.Row;
	bool fRealMap;
	bool fRandomColors = (drmMULTIPLE == drm && 3 == iMultColors);
	ValueRange vr = mp->vr();
	if (mp->dm()->pdbool())
		vr = ValueRange();
	if (vr.fValid()) {
		// always then integers are not good enough to represent the map 
		// treat it as a real map
		fRealMap = (vr->rStep() < 1) || (vr->stUsed() == stREAL);
	}
	else {
		fRealMap = false;
	}
	if (mm.MinCol() > iCols  ||
		mm.MaxCol() < 0	   ||
		mm.MinRow() > iLines ||
		mm.MaxRow() < 0)
		return 0;
	if (mm.MinCol() < 0) {
		rect.left() -= (long)(rounding(mm.MinCol() / rFact));
		mm.MinCol() = 0;
	}
	if (mm.MinRow() < 0) {
		rect.top() -= (long)(rounding(mm.MinRow() / rFact));
		mm.MinRow() = 0;
	}
	if (mm.MaxCol() >= iCols) {
		rect.right() -= (long)(rounding((mm.MaxCol()-iCols) / rFact));
		mm.MaxCol() = iCols;
	}
	if (mm.MaxRow() >= iLines) {
		rect.bottom() -= (long)(rounding((mm.MaxRow()-iLines) / rFact));
		mm.MaxRow() = iLines;
	}
	if (mm.width() == 0 || mm.height() == 0) return 0;
	lpbi->bmiHeader.biHeight = rect.height(); // no show solution
	lpbi->bmiHeader.biWidth  = rect.width(); // no show solution

	if (*fDrawStop)
		return 0;
	mp->KeepOpen(true);
	if (abs(rFact-1) < 1e-6) 
	{                    // a pixel is a rastel
		LongBuf lb(rect.width());
		if (fRealMap) 
		{
			RealBuf buf(rect.width());
			for (int i = 0; i < buf.iSize(); ++i)  // to prevent NAN values in buf.
				buf[i]= 0;
			riTranquilizer = RangeInt(0,rect.Height());
			for (iDspLine = 0, iMapLine = mm.MinRow();
				iDspLine <= rect.height() && iMapLine < iLines;
				iDspLine++, iMapLine++)
			{
				iTranquilizer = iDspLine;
				if (*fDrawStop)
					break;
				mp->GetLineVal(iMapLine, buf, mm.MinCol(), mm.width());
				ConvLine(buf, lb);
				PutLine(cdc, rect, iDspLine, lb);
			}
		}
		else 
		{ // !fRealMap
			LongBuf buf(rect.width());
			riTranquilizer = RangeInt(0,rect.Height());
			for (iDspLine = 0, iMapLine = mm.MinRow();
				iDspLine <= rect.height() && iMapLine < iLines;
				iDspLine++, iMapLine++) 
			{
				iTranquilizer = iDspLine;
				if (*fDrawStop)
					break;
				if (fValue && !fAttTable)
					mp->GetLineVal(iMapLine, buf, mm.MinCol(), mm.width());
				else
					mp->GetLineRaw(iMapLine, buf, mm.MinCol(), mm.width());
				ConvLine(buf);
				PutLine(cdc, rect, iDspLine, buf);
			}
		}                                                                         
	}
	else 
	{ // zoomed in or out
		double rMapLine, rMapCol;
		int iCol;
		if (fRealMap) 
		{
			int iWidth = (int)((mm.width() + 1)/ rFact);				
			int iPyrLayer = (int)(mp->fHasPyramidFile() ? max(0, log10(rFact) / log10(2.0)) : 0);	
			if (iPyrLayer > mp->iNoOfPyramidLayers() ) iPyrLayer = mp->iNoOfPyramidLayers();				
			rFact /= (long)pow(2.0, iPyrLayer);	
			mm.rcMax.Col /= (long)pow(2.0, iPyrLayer);
			mm.rcMax.Row /= (long)pow(2.0, iPyrLayer);				
			mm.rcMin.Col /= (long)pow(2.0, iPyrLayer);			
			mm.rcMin.Row /= (long)pow(2.0, iPyrLayer);		
			iLines /= (long)pow(2.0, iPyrLayer);	
			if (rect.width() > iWidth)
				rect.right() = rect.left() + iWidth;
			if (mm.MaxCol() < floor(iCols / pow(2.0, iPyrLayer))) mm.MaxCol() += 1;				
			RealBuf bMap(mm.width()+1);
			RealBuf bDsp(iWidth);
			LongBuf lb(iWidth);				
			int iLastLine = -1;
			for (int i = 0; i < bDsp.iSize(); ++i)  // to prevent NAN values in bDsp.
				bDsp[i]= 0;
			riTranquilizer = RangeInt(0,rect.Height());
			for (iDspLine = 0, rMapLine = mm.MinRow();
				iDspLine <= rect.height() && rMapLine < iLines;
				iDspLine++, rMapLine += rFact)
			{
				iTranquilizer = iDspLine;
				if (*fDrawStop)
					break;
				iMapLine = (long)floor(rMapLine);
				if (iMapLine != iLastLine) {
					mp->GetLineVal(iMapLine, bMap, mm.MinCol(), mm.width(), iPyrLayer);
					iLastLine = iMapLine;
					for (iCol = 0, rMapCol = 0;
						iCol < iWidth;
						iCol++, rMapCol += rFact)
						bDsp[iCol] = bMap[(long)(floor(rMapCol))];
					ConvLine(bDsp, lb);
				}
				PutLine(cdc, rect, iDspLine, lb);
			}
		}
		else 
		{ // !fRealMap
			int iWidth = (int)((mm.width() + 1)/ rFact);				
			int iPyrLayer = (int)(mp->fHasPyramidFile() ? max(0, log10(rFact) / log10(2.0)) : 0);
			if (iPyrLayer > mp->iNoOfPyramidLayers() ) iPyrLayer = mp->iNoOfPyramidLayers();
			rFact /= (long)pow(2.0, iPyrLayer);
			mm.rcMax.Col /= (long)pow(2.0, iPyrLayer);
			mm.rcMax.Row /= (long)pow(2.0, iPyrLayer);				
			mm.rcMin.Col /= (long)pow(2.0, iPyrLayer);			
			mm.rcMin.Row /= (long)pow(2.0, iPyrLayer);		
			iLines /= (long)pow(2.0, iPyrLayer);
			if (rect.width() > iWidth)
				rect.right() = rect.left() + iWidth;
			if (mm.MaxCol() < floor(iCols / pow(2.0, iPyrLayer))) mm.MaxCol() += 1;				
			LongBuf bMap(mm.width() + 1);
			LongBuf bDsp(iWidth);
			int iLastLine = -1;
			riTranquilizer = RangeInt(0,rect.Height());
			for (iDspLine = 0, rMapLine = mm.MinRow();
				iDspLine <= rect.height() && rMapLine < iLines;
				iDspLine++, rMapLine += rFact)
			{
				iTranquilizer = iDspLine;
				if (*fDrawStop)
					break;
				iMapLine = (long)floor(rMapLine);
				if (iMapLine != iLastLine) 
				{
					if (fValue && !fAttTable)
						mp->GetLineVal(iMapLine, bMap, mm.MinCol(), mm.width(), iPyrLayer);
					else
						mp->GetLineRaw(iMapLine, bMap, mm.MinCol(), mm.width(), iPyrLayer);
					iLastLine = iMapLine;
					for (iCol = 0, rMapCol = 0;
						iCol < iWidth;
						iCol++, rMapCol += rFact)
						bDsp[iCol] = bMap[(int)floor(rMapCol)];
					ConvLine(bDsp);
				}
				PutLine(cdc, rect, iDspLine, bDsp);
			}
		}
	}

	if (fText) 
	{
		int iRow, iCol;
		Color col = SysColor(COLOR_WINDOW);
		cdc->SetBkColor(col);
		cdc->SetTextColor(clrText);

		TEXTMETRIC tm;
		cdc->GetTextMetrics(&tm);
		int iH2 = tm.tmHeight / 2;
		if (rFact < 0.04) {  // every map pixel gets a value text
			UINT iOldAlign = cdc->SetTextAlign(TA_CENTER|TA_TOP);
			cdc->SetBkMode(OPAQUE);
			//int iFact = iScale; //floor(1/rFact);
			for (iRow = mm.MinRow(); iRow <= mm.MaxRow(); ++iRow) 
			{
				if (*fDrawStop)
					break;
				for (iCol = mm.MinCol(); iCol <= mm.MaxCol(); ++iCol) 
				{
					if (*fDrawStop)
						break;
					RowCol rc(iRow,iCol);  
					String s = mp->sValue(rc,0);
					if (s == "?")
						continue;
					int iLen = s.length();
					while (cdc->GetTextExtent(s.scVal(),iLen).cx > rScale)
						iLen -= 1;
					s[iLen] = 0;
					zPoint pnt = psn->pntPos(0.5+iRow,0.5+iCol);
					pnt.y -= iH2;
					cdc->TextOut(pnt.x, pnt.y, s.scVal());
				}  
			}
			cdc->SetTextAlign(iOldAlign);
		}
		else 
		{
			UINT iOldAlign = cdc->SetTextAlign(TA_CENTER|TA_TOP);
			cdc->SetBkMode(TRANSPARENT);
			const int iSTEPX = 50;
			const int iSTEPY = 40;
			int iFactRow = (int)ceil(iSTEPY * rFact);
			int iFactCol = (int)ceil(iSTEPX * rFact);
			int iCnt = 0;
			for (iRow = (0.3 + mm.MinRow() / iFactRow) * iFactRow; 
				iRow < mm.MaxRow(); iRow += iFactRow, ++iCnt) 
			{
				if (*fDrawStop)
					break;
				for (iCol = (0.2 + /*(iCnt%2) * 0.5 +*/ mm.MinCol() / iFactCol) * iFactCol; 
					iCol < mm.MaxCol(); iCol += iFactCol) 
				{
					if (*fDrawStop)
						break;
					RowCol rc(iRow,iCol);
					String s = mp->sValue(rc,0);
					if (s == "?")
						continue;
					int iLen = s.length();
					zPoint pnt = psn->pntPos(0.5+iRow,0.5+iCol);
					pnt.y -= iH2;
					pnt.x -= iH2;
					cdc->TextOut(pnt.x, pnt.y, s.scVal());

					int iExtend = cdc->GetTextExtent(s.scVal(), iLen).cx;
					while (5 + iExtend > iFactCol / rFact) {
						iCol += iFactCol;
						iExtend -= iFactCol / rFact;
					}
				}
			}
			cdc->SetTextAlign(iOldAlign);
		}
	}
	mp->KeepOpen(false);
	return 0;
}

void MapDrawer::SetColIndex(Color* colIndexNew)
{
	// replace colIndex in a thread-safe way
	Color* colIndexOld = colIndex;
	colIndex = colIndexNew;
	delete colIndexOld;
}

int MapDrawer::GreyColor()
{
	Color * colIndexNew = new Color[256];
  int iMin = 0;
  int iMax = 255;
  if (fStretch) {
    iMin = riStretch.iLo();
    iMax = riStretch.iHi();
  }
  int iLen = iMax - iMin;
  for (int i = 0; i < 256; ++i) 
  {
    int iTmp = i;
    if (iTmp < iMin)
      iTmp = iMin;
    else if (iTmp > iMax)
      iTmp = iMax;
    iTmp -= iMin;
    iTmp = 255.0 * iTmp / iLen;
    Color col(iTmp,iTmp,iTmp);
    colIndexNew[i] = clrDraw(col);
  }

	SetColIndex(colIndexNew);

  return 0;
}

int MapDrawer::SingleColor()
{
  Color* colIndexNew = new Color[2];
  colIndexNew[0] = mcd->colBackground;
  colIndexNew[1] = clrDraw(clr);

	SetColIndex(colIndexNew);

  return 0;
}

int MapDrawer::DualColor()
{
  Color* colIndexNew = new Color[3];
  colIndexNew[0] = mcd->colBackground;
  colIndexNew[1] = clrDraw(clr0);
  colIndexNew[2] = clrDraw(clr);

	SetColIndex(colIndexNew);

  return 0;
}

int MapDrawer::PrimaryColor(int iNr)
{

  Color* colIndexNew = new Color[1+iNr];
  colIndexNew[0] = mcd->colBackground;
  for (int i = 1; i <= iNr; ++i)
    colIndexNew[i] = clrDraw(clrPrimary(i));

	SetColIndex(colIndexNew);

  return 0;
}

void MapDrawer::Lut()
{
  if (!_rpr.fValid())
    return;

	Color* colIndexNew = 0;
  
	ColorBuf clr;
  _rpr->GetColorLut(clr);
	if (0 != mp->dm()->pdi()) // image special 0 case
		clr[0] = clr[1];
  int iNrColors = clr.iSize();

  if (fImage)
  {
    colIndexNew = new Color[256];
    if (_rpr->prv())
    {
      for (int i = 0; i < 256; ++i) 
      {
        int iTmp = _rpr->iColor(i);
        colIndexNew[i] = clrDraw(clr[iTmp]);
      }
    }  
    else 
    {
      int iMin = 0;
      int iMax = 255;
      if (fStretch) {
        iMin = riStretch.iLo();
        iMax = riStretch.iHi();
      }
      int iLen = iMax - iMin + 1;
      for (int i = 0; i < 256; ++i) 
      {
        int iTmp = i;
        if (iTmp < iMin)
          iTmp = iMin;
        else if (iTmp > iMax)
          iTmp = iMax;
        iTmp -= iMin;
        iTmp = iNrColors * iTmp / iLen;
        colIndexNew[i] = clrDraw(clr[iTmp]);
      }
    }  
  }  
  else 
  {
    colIndexNew = new Color[iNrColors];
    for (int i = 0; i < iNrColors; ++i)
      colIndexNew[i] = clrDraw(clr[i]);
    if (0 == mp->dm()->pdi() && 0 == mp->dm()->pdp())
      colIndexNew[0] = mcd->colBackground;
  }

	SetColIndex(colIndexNew);
}

class MapDrawerForm: public BaseMapDrawerForm
{
public:
  MapDrawerForm(MapDrawer* mdr, bool fShowForm=true);
  void DomUseChanged();
	bool fCalcPyramids() { return fPyrCreateFirstDisplay; };
  int exec();
private:
  void AskText(FormEntry*);
  int RprValueCallBack(Event*);
  int RprImageCallBack(Event*);
  int RprImageStretchCallBack(Event*);
  int iOption;
  int iStretchOption;
  MapDrawer* mdw;
  CheckBox* cbStretch;
  RadioGroup* rgStretchMethod;
  CheckBox* cbLegend;
  bool fStretch;
	bool fPyrCreateFirstDisplay;	
};

void MapDrawerForm::AskText(FormEntry* fe)
{
  CheckBox* cb = new CheckBox(root, SDCUiText, &mdw->fText);
  new FieldColor(cb, "", &mdw->clrText);  
  FieldBlank* fb = new FieldBlank(fe, 0);
  fb->Align(cb, AL_UNDER);
}
  
MapDrawerForm::MapDrawerForm(MapDrawer* mdr, bool fShowForm)
: BaseMapDrawerForm(mdr, SDCTitleRasMap, fShowForm), mdw(mdr),
  rgStretchMethod(0),
  cbLegend(0),
	fPyrCreateFirstDisplay(false)
{
	iImg = IlwWinApp()->iImage(".mpr");
  FormEntry* feLowest = 0;

	IlwisSettings settings("DefaultSettings");
  fPyrCreateFirstDisplay = settings.fValue("CreatePyrWhenFirstDisplayed", false);	

  Map& mp = mdr->mp;
  if (mdw->fImage) {
    AskText(root);
    fldRprValue = new FieldRepresentationC(root, SDCUiRpr, &sRprName, dmUse);
    fldRprValue->SetCallBack((NotifyProc)&MapDrawerForm::RprImageCallBack);
    fStretch = mdw->fStretch;
    cbStretch = new CheckBox(root, SDCUiStretch, &fStretch);
    cbStretch->SetCallBack((NotifyProc)&MapDrawerForm::RprImageStretchCallBack);
    stStretch = new StaticText(root, SDCUiStretch); 
    stStretch->Align(fldRprValue, AL_UNDER);
    fri = new FieldRangeInt(root, "", &mdw->riStretch, ValueRange(0L,255L));
    fri->Align(cbStretch, AL_AFTER);
    SetMenHelpTopic(htpCnfMapDrawerImage);
    RadioGroup* rg = new RadioGroup(root, "", (int*)&mdw->drc, true);
    rg->Align(cbStretch, AL_UNDER);
    InitColoring(rg);
  }
  else if (!mp->dm()->fValidDomain()) {
    if (mp->dm()->pdv()) {
      sRprName = "gray.rpr";
      fldRprValue = new FieldRepresentationC(root, SDCUiRpr, &sRprName, dmUse);
      InitStretch(root);
    }
    else {
      iOption = 1;
      RadioGroup* rg = new RadioGroup(root, "", &iOption);
      InitSingle(rg);
      InitMultiple(rg);
    }  
  }
  else if (mp->dm()->pdbit()) {
    AskText(root);
    new FieldColor(root, SDCUiColor, &mdw->clr);
    RadioGroup* rg = new RadioGroup(root, "", (int*)&mdw->drc, true);
    InitColoring(rg);
    SetMenHelpTopic(htpCnfMapDrawerBit);
  }
  else if (mp->dm()->pdp()) {
    // nothing (yet)
    RadioGroup* rg = new RadioGroup(root, "", (int*)&mdw->drc, true);
    InitColoring(rg);
    SetMenHelpTopic(htpCnfMapDrawerPicture);
  }
  else {
    bool fTbl = bm->fTblAtt();
    AskText(root);
    InitAtt();
    FieldBlank* fb = new FieldBlank(root, 0);

    if (fTbl || bm->dm()->pdvi() || bm->dm()->pdvr()) {
      fgValue = new FieldGroup(root);
      if (fTbl)
        fgValue->Align(fb, AL_UNDER);
      fldRprValue = new FieldRepresentationC(fgValue, SDCUiRpr, &sRprName, dmUse);
      fldRprValue->SetCallBack((NotifyProc)&MapDrawerForm::RprValueCallBack);

      InitStretch(fgValue);
      iStretchOption = (int)mdw->sm;
      rgStretchMethod = new RadioGroup(fgValue, "", &iStretchOption, true);
      rgStretchMethod->SetIndependentPos();
      rgStretchMethod->Align(stStretch, AL_UNDER);
      new RadioButton(rgStretchMethod, SDCUiLinear);
      new RadioButton(rgStretchMethod, SDCUiLogarithmic);
      RadioGroup* rg = new RadioGroup(fgValue, "", (int*)&mdw->drc, true);
      InitColoring(rg);
      if (bm->dm()->pdv())
        SetMenHelpTopic(htpCnfMapDrawerValue);
    }

    if (fTbl || bm->dm()->pdc()) {
      fgClass = new FieldGroup(root);
      if (fTbl)
        fgClass->Align(fb, AL_UNDER);
      fgClass->SetIndependentPos();
      fldRprClass = new FieldRepresentationC(fgClass, SDCUiRpr, &sRprName, dmUse);
      RadioGroup* rg = new RadioGroup(fgClass, "", (int*)&mdw->drc, true);
      InitColoring(rg);
      feLowest = rg;
      if (bm->dm()->pdc())
        SetMenHelpTopic(htpCnfMapDrawerClass);
    }
    if (fTbl || bm->dm()->pdid()) {
      iOption = 0;
      if (mdw->drm == drmMULTIPLE)
        iOption = 1;
      FieldGroup* fgIdent = new FieldGroup(root);
      RadioGroup* rgID = new RadioGroup(fgIdent, "", &iOption);
      if (fTbl)
        rgID->Align(fb, AL_UNDER);
      InitSingle(rgID);
      InitMultiple(rgID);
      RadioGroup* rg = new RadioGroup(fgIdent, "", (int*)&mdw->drc, true);
      rg->Align(rgID, AL_UNDER);
      InitColoring(rg);
      feLowest = rg;
      fgID = fgIdent;
      if (bm->dm()->pdid())
        SetMenHelpTopic(htpCnfMapDrawerID);
    }
    if (fTbl || bm->dm()->pdbool()) {
      InitBool();
      if (fTbl)
        fgBool->Align(fb, AL_UNDER);
      RadioGroup* rg = new RadioGroup(fgBool, "", (int*)&mdw->drc, true);
      InitColoring(rg);
			if (bm->dm()->pdbool())
				SetMenHelpTopic(htpCnfMapDrawerBool);
    }
		if (fTbl || mp->dm()->pdcol()) {
      fgColor = new FieldGroup(root);
      if (fTbl)
        fgColor->Align(fb, AL_UNDER);
			else {
				CWindowDC wdc(AfxGetMainWnd());
				bool fPalette = (wdc.GetDeviceCaps(RASTERCAPS) & RC_PALETTE) != 0;
				if (fPalette) {
					new StaticText(fgColor, SDCWrnPaletteWithColor1);
					new StaticText(fgColor, SDCWrnPaletteWithColor2);
				}  
				SetMenHelpTopic(htpCnfMapDrawerColor);
			}
			RadioGroup* rg = new RadioGroup(fgColor, "", (int*)&mdw->drc, true);
			InitColoring(rg);
		}
  }

	if (!mp->fHasPyramidFile() && !mp->fReadOnly() && 0==mp->dm()->pdbit())
  {
	  CheckBox* cb = new CheckBox(root, SDCUiCreatePyramidFiles, &fPyrCreateFirstDisplay);				
    if (feLowest)
      cb->Align(feLowest, AL_UNDER);
    cb->SetIndependentPos();
  }

  create();
}

void MapDrawerForm::DomUseChanged()
{
  if (fgValue) fgValue->Hide();
  if (fgClass) fgClass->Hide();
  if (fgID) fgID->Hide();
  if (fgBool) fgBool->Hide();
	if (fgColor) fgColor->Hide();
  if (cbLegend)
    cbLegend->Hide();
  switch (dmtUse()) {
    case dmtCLASS:
      { Representation rpr;
        if (mdw->dm() == dmUse)
          rpr = mdw->rpr();
        if (!rpr.fValid())
          rpr = dmUse->rpr();
        if (rpr.fValid()) {
          sRprName = rpr->sName(true);
          fldRprClass->SetVal(sRprName);
        }
      }
      fgClass->Show();
      if (cbLegend)
        cbLegend->Show();
      break;
    case dmtID:
      fgID->Show();
      if (dmUse->pdsrt()->iNettoSize() < 100)
        if (cbLegend)
          cbLegend->Show();
      break;
    case dmtIMAGE:
      sRprName = "gray.rpr";
      break;
    case dmtVALUE:
      { Representation rpr;
        if (mdw->dm() == dmUse)
          rpr = mdw->rpr();
        if (!rpr.fValid())
          rpr = dmUse->rpr();
        if (rpr.fValid()) {
          sRprName = rpr->sName(true);
        }
        else
          sRprName = "gray";
      }
      fldRprValue->SetVal(sRprName);
      fgValue->Show();
      if (cbLegend)
        cbLegend->Show();
      StretchCallBack(0);
      break;
    case dmtBOOL:
      fgBool->Show();
      break;
    case dmtCOLOR:
      fgColor->Show();
      break;
  }
}

int MapDrawerForm::RprValueCallBack(Event*)
{
  BaseMapDrawerForm::RprValueCallBack(0);
  if (0 == rgStretchMethod)
    return 0;
  try {
    FileName fn = sRprName;
    Representation rpr(fn);
		RepresentationValue* rpv = rpr->prv();
    if (0 == rpv) 
      rgStretchMethod->Show();
    else
      rgStretchMethod->Hide();
  }
  catch (ErrorObject&) {}
  return 0;
}

int MapDrawerForm::RprImageCallBack(Event*)
{
  fldRprValue->StoreData();
  try {
    FileName fn = sRprName;
    if (fn.sFile == "gray") {
      cbStretch->Show();
      stStretch->Hide();
    }
    else {
      Representation rpr(fn);
      cbStretch->Hide();
      if (rpr->prv()) {
        stStretch->Hide();
        fri->Hide();
      }  
      else {
        stStretch->Show();
        fri->Show();
      }  
    }  
  }
  catch (ErrorObject&) {}
  return 0;
}

int MapDrawerForm::RprImageStretchCallBack(Event*)
{
  cbStretch->StoreData();
  if (fStretch)
    fri->Show();
  else  
    fri->Hide();
  return 0;
}

int MapDrawerForm::exec()
{
  BaseMapDrawerForm::exec();
  if (!dmUse->fValidDomain()) {
    if (dmUse->pdv()) {
      mdw->drm = drmRPR;
      mdw->_rpr = Representation(FileName(sRprName));
    }
    else {
      if (iOption)
        mdw->drm = drmMULTIPLE;
      else
        mdw->drm = drmSINGLE;
    }
    return 0;
  }
  switch (dmtUse()) {
    case dmtIMAGE: {
      FileName fn = sRprName;
      if (fn.sFile == "gray") {
        mdw->drm = drmIMAGE;
        mdw->fStretch = fStretch;
      }  
      else {  
        mdw->drm = drmRPR;
      }
      mdw->_rpr = Representation(FileName(sRprName));
    } break;
    case dmtCLASS:
      mdw->drm = drmRPR;
      mdw->_rpr = Representation(FileName(sRprName));
      dmUse->SetRepresentation(mdw->_rpr);
//      if (mdw->_rpr->pra())
//        mdw->_rpr = Representation(mdw->_rpr, mdw->mp->tblAtt());
      break;
    case dmtID:
      if (iOption)
        mdw->drm = drmMULTIPLE;
      else
        mdw->drm = drmSINGLE;
      break;
    case dmtVALUE:
      mdw->drm = drmRPR;
      mdw->_rpr = Representation(FileName(sRprName));
      dmUse->SetRepresentation(mdw->_rpr);
      mdw->sm = (MapDrawer::StretchMethod) iStretchOption;
      break;
    case dmtPICTURE:
      mdw->drm = drmRPR;
      mdw->_rpr = dmUse->rpr();
      break;
    case dmtCOLOR:
      mdw->drm = drmCOLOR;
      break;
    case dmtBIT:
      mdw->drm = drmSINGLE;
      break;
    case dmtBOOL:
      mdw->drm = drmBOOL;
      break;
  }
  return 0;
}

int MapDrawer::Configure(bool fShowForm)
{
  bool fShow = fAct;
  bool fOk = fNew;
  if (fNew) {
    fAct = false;  // to prevent "in between" drawing
    fNew = false;
  }
  {
    MapDrawerForm frm(this,fShowForm);
    fOk = frm.fOkClicked();
    if (fOk) 
		{
      fShow = true;
      
			if ( !mp->fReadOnly() )
				if (frm.fCalcPyramids() && !mp->fHasPyramidFile())
					mp->CreatePyramidLayer();
    }
  }  	
  fAct = fShow;
  if (fOk)
    Setup();
  return fOk;
}

int MapDrawer::Setup()
{
  if (fValue) {
    Domain dm;
    ValueRange vr;  
    if (fAttTable) {
      dm = colAtt->dm();
      if (0 == dm->pdbool())
        vr = colAtt->vr();
    }  
    else {
      dm = mp->dm();  
      if (0 == dm->pdbool())
        vr = mp->vr();
    }  
    DomainValueRangeStruct dvrs(dm, vr);
    RangeReal rr = dvrs.rrMinMax();
    if (rrStretch.rLo() < rr.rLo())
      rrStretch.rLo() = rr.rLo();
    if (rrStretch.rHi() > rr.rHi())
      rrStretch.rHi() = rr.rHi();
    if (dvrs.fRealValues()) {
      riRawStretch = RangeInt(dvrs.iRaw(rrStretch.rLo()),
                              dvrs.iRaw(rrStretch.rHi()));
      riStretch = RangeInt(longConv(rrStretch.rLo()),
                           longConv(rrStretch.rHi()));
    }      
    else if (dvrs.fValues()) {
      riRawStretch = RangeInt(dvrs.iRaw(rrStretch.rLo()),
                              dvrs.iRaw(rrStretch.rHi()));
    }
  }
  switch (drm) {
    case drmIMAGE:
      GreyColor();
      break;
    case drmRPR:
      Lut();
      break;
    case drmSINGLE:
      SingleColor();
      break;
    case drmBOOL:
      DualColor();
      break;
    case drmMULTIPLE:
      switch (iMultColors) {
        case 0: PrimaryColor(7); break;
        case 1: PrimaryColor(15); break;
        case 2: PrimaryColor(31); break;
        case 3: break;          
      }
      break;
  }
  return 0;
}

String MapDrawer::sInfo(const Coord& crd)
{
  String s = mp->sValue(crd,0);
  if (fAttTable) {
    long iRaw = mp->iRaw(crd);
    if (iRaw != iUNDEF) {
      String sAtt = colAtt->sValue(iRaw);
      for (int iLen = sAtt.length(); iLen && sAtt[iLen-1] == ' '; --iLen)
        sAtt[iLen-1] = 0;
      s &= ": ";
      s &= sAtt;
    }
  }
  return s;
}

/*
void MapDrawer::Edit()
{
  mppn->PixelEditStr(mp->fnObj);
}
*/
/*
Record MapDrawer::rec(const Coord& crd)
{
  return recFromMap(mp, crd);
}
*/

FormBaseDialog* MapDrawer::wEditRpr(CWnd* wPar, int iRaw)
{
  if (iRaw > 0 && _rpr.fValid()) {
    RepresentationClass* rpc = _rpr->prc();
    if (rpc) {
      if (fAttTable)
        iRaw = colAtt->iRaw(iRaw);
      return new MapRprEditForm(wPar, rpc, iRaw, true);
    }
  }
  return 0;
}

FormBaseDialog* MapDrawer::wEditRpr(CWnd* wPar, const Coord& crd)
{
  long iRaw = mp->iRaw(crd);
	return wEditRpr(wPar, iRaw);
}


IlwisObject MapDrawer::obj() const
{
  return mp;
}

bool MapDrawer::fEditable()
{
	MapPaneView* mpv = mcd->mpvGetView();
	if (0 == mpv)
		return false;
  if (mp->fDataReadOnly())
    return false;
  Domain dm = mp->dm();
  if (dm->pdsrt() || dm->pdv())
    return true;
  return false;
}

zIcon MapDrawer::icon() const
{
	return zIcon("MapIcon");
}

void MapDrawer::DrawLegendRect(CDC* cdc, CRect rect, int iRaw) const
{
	Color clr = clrRaw(iRaw);
	CBrush br(clr);
	CBrush* brOld = cdc->SelectObject(&br);
	cdc->Rectangle(rect);
  cdc->SelectObject(brOld);
}

void MapDrawer::DrawValueLegendRect(CDC* cdc, CRect rect, double rVal) const
{
  if (!_rpr.fValid())
    return;
	Color c = clrVal(rVal);
  if ((long)c == -2)
		return;
	CBrush br(c);
	CBrush* brOld = cdc->SelectObject(&br);
	cdc->Rectangle(rect);
  cdc->SelectObject(brOld);
}

void MapDrawer::UsesBandedDrawing(bool fYesNo)
{
	fBands = fYesNo;
}

bool MapDrawer::fUsesBandedDrawing()
{
	return fBands;
}
