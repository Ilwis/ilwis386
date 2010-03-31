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
#include "Client\Headers\formelementspch.h"
#include "Engine\Map\Segment\Seg.h"
#include "engine\map\polygon\POL.H"
#include "Client\Mapwindow\Positioner.h"
#include "Client\FormElements\fldcolor.h"
#include "Client\FormElements\objlist.h"
#include "Client\Mapwindow\Drawers\BaseDrawer.h"
#include "Client\Mapwindow\Drawers\Drawer.h"
#include "Client\Mapwindow\Drawers\GridDrawer.h"
#include "Client\ilwis.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\MainWindow\Catalog\CatalogDocument.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Engine\SpatialReference\GR3D.H"
#include "Engine\SpatialReference\GRCTP.H"
#include "Engine\Domain\dmclass.h"
#include "Engine\Domain\Dmvalue.h"
#include "Engine\Base\Round.h"
#include "Client\Mapwindow\Drawers\Grid3DDrawer.h"
#include "Client\Editors\Utils\SYMBOL.H"
#include "Client\Editors\Utils\Smbext.h"
#include "Client\Editors\Utils\line.h"
#include "Client\Editors\Representation\RprEditForms.h"
#include "Client\FormElements\fldrpr.h"
#include "Headers\Hs\Drwforms.hs"

#undef Polygon

Grid3DDrawer::Grid3DDrawer(MapCompositionDoc *doc) :
  Drawer(doc),
  cStart(0,0),
  iWidth(1),
  col(255,0,0),
  fFast(true), fGrid(true), fFilled(false), fStretch(true),
  grf3d(0)
{
  CRect rect;
  MinMax mm = mcd->mmBounds();
  CoordBounds cb = cbCalc(mm);
  iFillSize = 3;
  rDist = rUNDEF; rRound(min(cb.width(), cb.height() ) / 20);
  riStretch = RangeInt(0,255);
  rrStretch = RangeReal(0,100);
	fSelectable = true;
  init();
}

Grid3DDrawer::Grid3DDrawer(MapCompositionDoc* doc, const MapView& v, const char* sSection) :
  Drawer(doc),
  cStart(0,0),
  iWidth(1),
  col(255,0,0),
  fFast(true), 
	fGrid(true), 
	fFilled(false)
{
  v->ReadElement(sSection, "Fast", fFast);
  v->ReadElement(sSection, "Grid", fGrid);
  v->ReadElement(sSection, "Texture", fFilled);
  v->ReadElement(sSection, "Distance", rDist);
  v->ReadElement(sSection, "Color", col);
  v->ReadElement(sSection, "Texture Map", mp);
  iFillSize = (long)rDist;
  v->ReadElement(sSection, "Texture Distance", iFillSize);

  int iCol = 256;
	fSelectable = true;
	if (fFilled)
		fFilled = mp.fValid();
	if (mp.fValid())
		rpr = mp->dm()->rpr();
  init();
}

Grid3DDrawer::~Grid3DDrawer()
{
  ILWISSingleLock csl(&cs, TRUE, SOURCE_LOCATION);
}

void Grid3DDrawer::WriteLayer(MapView& v, const char* sSection)
{
  Drawer::WriteLayer(v, sSection);
  v->WriteElement(sSection, "Type", "Grid3DDrawer");
  v->WriteElement(sSection, "Fast", fFast);
  v->WriteElement(sSection, "Grid", fGrid);
  v->WriteElement(sSection, "Distance", rDist);
  v->WriteElement(sSection, "Color", col);
  v->WriteElement(sSection, "Texture", fFilled);
  if (fFilled) 
  {
    v->WriteElement(sSection, "Texture Map", mp);
    v->WriteElement(sSection, "Texture Distance", iFillSize);
  }  
}

int Grid3DDrawer::draw(CDC *dc, zRect rect, Positioner* psn, volatile bool *fDrawStop)
{
  ILWISSingleLock csl(&cs, TRUE, SOURCE_LOCATION);
  if (!fAct) return 0;
  if (!grf3d->mapDTM.fValid())
    return 0;

	if (!mp.fValid())
		fFilled = false;
  if (fFilled )
    DrawFilledGrid(dc, rect, psn, fDrawStop);
  if (fGrid) 
    DrawOpenGrid(dc, rect, psn, fDrawStop);
  return 0;
}


CoordBounds Grid3DDrawer::cbCalc(const MinMax& mm)
{
  CoordBounds cb;
  GeoRef3D *grf=dynamic_cast<GeoRef3D *>(mcd->georef.ptr());
  if (!grf) return cb;
  Coord c;
  GeoRef gr;
  if (mp.fValid())
    gr = mp->gr();
  else if (grf->mapDTM.fValid())
    gr = grf->mapDTM->gr();
  if (!gr.fValid())
    return cb;
  gr->RowCol2Coord(mm.MinRow()+0.5, mm.MinCol()+0.5, c);
  cb += c;
  gr->RowCol2Coord(mm.MinRow()+0.5, mm.MaxCol()-0.5, c);
  cb += c;
  gr->RowCol2Coord(mm.MaxRow()-0.5, mm.MinCol()+0.5, c);
  cb += c;
  gr->RowCol2Coord(mm.MaxRow()-0.5, mm.MaxCol()-0.5, c);
  cb += c;
  // to ensure that they are inside the map
  cb.MinX() += 1;
  cb.MaxX() -= 1;
  cb.MinY() += 1;
  cb.MaxY() -= 1;
  return cb;
}

CPoint Grid3DDrawer::Coord2Pnt(Positioner* psn, Coord c)
{
  double rRow, rCol;
  grf3d->Coord2RowCol(c, rRow, rCol);
  return psn->pntPos(rRow, rCol);
}

void Grid3DDrawer::DrawOpenGrid(CDC *dc, CRect rect, Positioner *psn, volatile bool *fDrawStop)
{
//  if (!map.fValid()) return;
  RowCol rc1, rc2;
  CPoint corner[4];
   
  corner[0]=Coord2Pnt(psn, grf->cConv(rc[0])); // top left
  corner[1]=Coord2Pnt(psn, grf->cConv(rc[1])); // top right
  corner[2]=Coord2Pnt(psn, grf->cConv(rc[2])); // bottom left
  corner[3]=Coord2Pnt(psn, grf->cConv(rc[3])); // bottom right

  bool fTop2Bottom;
  int iCrn = 0;
  for (int ic=1; ic <=3; ++ic) {
    if (corner[ic].y !=shUNDEF && corner[ic].y < corner[iCrn].y)
      iCrn = ic;
  }
  switch ( iCrn) {
    case 0: // top left
      fTop2Bottom = true;
      break;
    case 1: // top right
      fTop2Bottom = true;
      break;
    case 2: // bottom right
      fTop2Bottom = false;
      break;
    case 3: // bottom left
      fTop2Bottom = false;
      break;
  }
  
  double rRowDelta, rColDelta;
  long iStartRow, iStartCol, iEndRow, iEndCol;
  double rPixSize = grf3d->mapDTM->gr()->rPixSize();
  if (fTop2Bottom) {
    rRowDelta = rDist/rPixSize;
    iStartRow = 0;
    iEndRow = rc[2].Row;
  }
  else {
    rRowDelta = -rDist/rPixSize;
    iStartRow = rc[2].Row;
    iEndRow = 0;
  }
  rColDelta = rDist/rPixSize;
  iStartCol = 0;
  iEndCol = rc[1].Col;
  Color clr;
  if ((long)col == -1)
    clr = GetSysColor(COLOR_WINDOWTEXT);
  else
    clr = col;

  CPen pen(PS_SOLID, 1, col);
	CPen *oldPen = dc->SelectObject(&pen);

  long iRuns = 2 + (long)(abs(iStartRow-iEndRow) / fabs(rRowDelta));
  long iRow = iStartRow;
  CPoint *TopLine=NULL, *BottomLine=NULL;
  short iTopN=0, iBottomN=0, n=0;
	riTranquilizer = RangeInt(0,iRuns);
	iTranquilizer = 0;	
  for (long iRun = 0; iRun < iRuns; ++iRun) 
	{
		if ( *fDrawStop)
			break;
    RowCol rc1, rc2;
    long iRow = iStartRow+(long)(iRun*rRowDelta);
    if (rRowDelta>0)
      iRow = min(iRow, iEndRow);
    else
      iRow = max(0L, iRow);
    BottomLine = MakePoints(psn, iRow, iStartCol, iEndCol, iBottomN);
    if (iRun == 0)
      n=iBottomN;
    else
      n=min(iTopN, iBottomN);
    DrawLines(dc, rect, TopLine, BottomLine, n, rColDelta);
    if (0 != TopLine)
      delete [] TopLine;
    TopLine=BottomLine;
    iTopN=iBottomN;
		++iTranquilizer;
  }
  delete [] BottomLine;

	dc->SelectObject(oldPen);
}

void Grid3DDrawer::DrawFilledGrid(CDC *dc, CRect rect, Positioner *psn, volatile bool *fDrawStop)
{
  if (!mp.fValid()) return;
  RowCol rc1, rc2;
  CPoint corner[4];
  corner[0]=Coord2Pnt(psn, grf->cConv(rc[0])); // top left
  corner[1]=Coord2Pnt(psn, grf->cConv(rc[1])); // top right
  corner[2]=Coord2Pnt(psn, grf->cConv(rc[2])); // bottom left
  corner[3]=Coord2Pnt(psn, grf->cConv(rc[3])); // bottom right

  bool fTop2Bottom, fLeft2Right;
  int iCrn = 0;
  for (int ic=1; ic <=3; ++ic) {
    if (corner[ic].y!=shUNDEF && corner[ic].y < corner[iCrn].y)
      iCrn = ic;
  }
  switch ( iCrn) {
    case 0: // top left
      fTop2Bottom = true;
      fLeft2Right = true;
      break;
    case 1: // top right
      fTop2Bottom = true;
      fLeft2Right = false;
      break;
    case 2: // bottom right
      fTop2Bottom = false;
      fLeft2Right = false;
      break;
    case 3: // bottom left
      fTop2Bottom = false;
      fLeft2Right = true;
      break;
  }
  long iRowDelta, iColDelta;
  long iStartRow, iStartCol, iEndRow, iEndCol;
  if (fTop2Bottom) {
    iRowDelta = iFillSize;
    iStartRow = 0;
    iEndRow = rc[2].Row;
  }
  else {
    iRowDelta = -iFillSize;
    iStartRow = rc[2].Row;
    iEndRow = 0;
  }
  if (fLeft2Right) {
    iColDelta = iFillSize;
    iStartCol = 0;
    iEndCol = rc[1].Col;
  }
  else {
    iColDelta = -iFillSize;
    iStartCol = rc[1].Col;
    iEndCol = 0;
  }
  
  long iRuns = 2 + abs(iStartRow-iEndRow) / abs(iRowDelta);
  long iRow = iStartRow;
  PointStruct *TopLine=NULL, *BottomLine=NULL;
  short iTopN=0, iBottomN=0, n=0;
	riTranquilizer = RangeInt(0,iRuns);
	iTranquilizer = 0;		
  for (long iRun = 0; iRun < iRuns; ++iRun) {
    RowCol rc1, rc2;

		if ( *fDrawStop)
			break;		
 
    BottomLine = MakePoints(psn, iRow, iStartCol, iEndCol, iBottomN, iColDelta);

    if ( TopLine && BottomLine)
    {
        n=min(iTopN, iBottomN);
        DrawPolygons(dc, rect, TopLine, BottomLine, n);
        delete [] TopLine;
    }
    TopLine=BottomLine;
    iTopN=iBottomN;
    iRow += iRowDelta;
    if (iRowDelta < 0)
      iRow = max(0, iRow);
    else
      iRow = min(iEndRow, iRow);
		++iTranquilizer;
  }
  delete [] BottomLine;
}

CPoint *Grid3DDrawer::MakePoints(Positioner *psn, long iRow, long iStartCol, long iEndCol, short& n)
{
  RowCol rc(iRow, iStartCol);
  long iCols = 2+abs(iStartCol-iEndCol);
  int iColDelta = 1;
  n = (short)iCols;
  CPoint *line = new CPoint[iCols];
  double rRow, rCol;
  int iBufSize = abs(iEndCol-iStartCol)+1;
  RealBuf rHeightBuf(iBufSize);
  bool fSameGrf = true;
  if (fFilled)
    fSameGrf = grf3d->mapDTM->gr() == mp->gr();
  if (fSameGrf) {
    grf3d->mapDTM->GetLineVal(iRow, rHeightBuf, min(iStartCol, iEndCol));
    rHeightBuf[iBufSize-1] = rHeightBuf[iBufSize-2];
  }
  for (long j=0; j < iCols; ++j) {
    Coord c;
    if (fFilled) 
		{
      mp->gr()->RowCol2Coord(rc.Row, rc.Col, c);
      if (fSameGrf)
        grf3d->Crd2RC(c, rRow, rCol, rHeightBuf[rc.Col]);
      else
        grf3d->Coord2RowCol(c, rRow, rCol);
    }
    else {
      grf3d->mapDTM->gr()->RowCol2Coord(rc.Row, rc.Col, c);
      grf3d->Crd2RC(c, rRow, rCol, rHeightBuf[rc.Col]);
    }
    line[j] = psn->pntPos(rRow, rCol);
		if ( line[j].y > 900)
		{
			int k=5;
			k+=1;
		}			
    rc.Col += iColDelta;
    rc.Col = min((long)iBufSize-1, rc.Col);
  }
  return line;
}

Grid3DDrawer::PointStruct *Grid3DDrawer::MakePoints(Positioner *psn, long iRow, long iStartCol, long iEndCol, short& n, long iColDelta)
{
  RowCol rc(iRow, iStartCol);
  long iCols = abs(iEndCol-iStartCol) / abs(iColDelta) + 2;
  n = iCols;
  PointStruct *line = new PointStruct[iCols];
	memset(line, 0, sizeof(PointStruct) * iCols);
  double rRow, rCol;
  bool fImage, fValues, fUseRpr, fColor;
  fImage = 0 != mp->dm()->pdi();
  fValues = !fImage && (0 != dm->pdv()) && rpr.fValid();
  fUseRpr = (dm->pdc() || dm->pdp()) && rpr.fValid();
  fColor = 0 != mp->dm()->pdcol();
  LongBuf iBuf;
  RealBuf rBuf, rHeightBuf;
  ColorBuf cBuf;
  int iBufSize = 2+abs(iEndCol-iStartCol);
  rHeightBuf.Size(iBufSize);
  bool fSameGrf = grf3d->mapDTM->gr() == mp->gr();
  if (fSameGrf) {
    grf3d->mapDTM->GetLineVal(iRow, rHeightBuf, min(iStartCol, iEndCol), iBufSize);
    rHeightBuf[iBufSize-1] = rHeightBuf[iBufSize-2];
  }
  if (fValues) {
    rBuf.Size(iBufSize);
    mp->GetLineVal(iRow, rBuf, min(iStartCol, iEndCol), iBufSize);
    rBuf[iBufSize-1] = rBuf[iBufSize-2];
  }
  else {
    iBuf.Size(iBufSize);
    mp->GetLineRaw(iRow, iBuf, min(iStartCol, iEndCol), iBufSize);
    iBuf[iBufSize-1] = iBuf[iBufSize-2];
  }
  for (long j=0; j < iCols; ++j) {
    Coord c;
    mp->gr()->RowCol2Coord(rc.Row, rc.Col, c);
    if (fSameGrf)
      grf3d->Crd2RC(c, rRow, rCol, rHeightBuf[rc.Col]);
    else
      grf3d->Coord2RowCol(c, rRow, rCol);
    line[j].pnt = psn->pntPos(rRow, rCol);
    if (fUseRpr) 
      line[j].col = clrRaw(iBuf[rc.Col]);
    else if (fValue)
      line[j].col = clrVal(rBuf[rc.Col]);
    else if (fImage)
      line[j].col = clrVal(iBuf[rc.Col]);
    else if (fColor)
      line[j].col = Color(iBuf[rc.Col]);
    rc.Col += iColDelta;
    if (iColDelta < 0)
      rc.Col = max(0L, rc.Col);
    else
      rc.Col = min((long)iBufSize-1, rc.Col);
  }
  return line;
}


Color Grid3DDrawer::clrVal(double rVal)	const
{
	if (fStretch)
		return rpr->clr(rVal,rrStretch);
	else
		return rpr->clr(rVal);
}

Color Grid3DDrawer::clrRaw(long iRaw) const
{
	if (rpr->prg())
		return clrVal(iRaw);
	else
		return rpr->clrRaw(iRaw);
}

void Grid3DDrawer::DrawPolygons(CDC *dc, CRect rect, PointStruct *topLine, PointStruct *bottomLine, short n)
{
  rect.left -= 100;
  rect.top -= 100;
  rect.right += 100;
  rect.bottom += 100;
  CPoint corners[5];
  for ( short i=0; i<n-1 ;++i )
  {
      corners[0]=CPoint(topLine[i].pnt);
      corners[1]=CPoint(topLine[i+1].pnt);
      corners[2]=CPoint(bottomLine[i+1].pnt);
      corners[3]=CPoint(bottomLine[i].pnt);
      corners[4]=CPoint(topLine[i].pnt);
	  int j=0;
      for (; j < 4 ; j++) 
        if (!rect.PtInRect(corners[j]))
          break;
      if (j < 4)
        continue; // skip polygon

      CBrush brush(topLine[i].col);
      CPen pen(PS_SOLID, 1, topLine[i].col);
			CBrush *oldBrush = dc->SelectObject(&brush);
			CPen *oldPen = dc->SelectObject(&pen);
 
      dc->Polygon(corners, 5);

      dc->SelectObject(oldBrush);
			dc->SelectObject(oldPen);
  }
}

void Grid3DDrawer::DrawLines(CDC *dc, CRect rect, CPoint *topLine, CPoint *bottomLine, short n, double rColDelta)
{
  if (n == 0)
    return;
  rect.left -= 100;
  rect.top -= 100;
  rect.right += 100;
  rect.bottom += 100;
  int iCols = 2+ (int)(floor((n-1) / rColDelta));
  if (topLine) {
    short j = 0;
    for ( short i=0; i<iCols ;++i ) 
		{
      if (rect.PtInRect(bottomLine[j]) && rect.PtInRect(topLine[j]))
      {
        dc->MoveTo(bottomLine[j]);
        dc->LineTo(topLine[j]);
      }
      j = (short)((i+1)*rColDelta);
      j = min(j, n-1);
    }
  }
  bool fDraw = false;
  for ( short i=0; i<n ;++i ) 
	{
    if (!rect.PtInRect(bottomLine[i]))    
      fDraw = false;
    else {
      if (fDraw)
        dc->LineTo(bottomLine[i]);
      else
        dc->MoveTo(bottomLine[i]);
      fDraw = true;
    }
  }
}

class Grid3DDrForm : public FormWithDest
{
public:
  Grid3DDrForm(CWnd* wPar, 
               bool* fGrid, double* rDist, Color* col,
               bool* fTexture, String* sMp, String* sRprName, 
							 long* iFillSize, bool* fFast)
  : FormWithDest(wPar, SDCTitle3DGrid),
	  sDrapeMap(sMp)
  {
    CheckBox* cb = new CheckBox(root, SDCUiGrid, fGrid);
    FieldGroup* fg = new FieldGroup(cb);
    fg->Align(cb, AL_UNDER);
    new FieldReal(fg, SDCUiGridDistance, rDist);
    new FieldColor(fg, SDCUiColor, col);
		//new FieldBlank(fg);
    cb = new CheckBox(root, SDCUiTexture, fTexture);
    fg = new FieldGroup(cb);
    fg->Align(cb, AL_UNDER);
    fm = new FieldDataType(fg, SDCUiDrapeMap, sDrapeMap,
                    new MapListerDomainType(dmCLASS|dmIMAGE|dmVALUE|dmPICT|dmCOLOR), false);
		fm->SetCallBack((NotifyProc)&Grid3DDrForm::SetRpr);

    if ("" != *sDrapeMap)
      try
      {
  			FileName fn(*sDrapeMap);	
   			Map mp(fn);
  			dmMap = mp->dm();
      }
      catch (ErrorObject&)
      {
      }

		frpr = new FieldRepresentationC(fg, SDCUiRpr, sRprName, dmMap);
    new FieldInt(fg, SDCUiFillSize, iFillSize, ValueRange(1,100), true);
    SetMenHelpTopic(htpCnfGrid3D);
    
    create();
  }

private:
	Domain dmMap;
	String *sDrapeMap;
	FieldRepresentationC *frpr;
	FieldDataType* fm;

	int SetRpr(Event *)
	{
		fm->StoreData();
    if ("" == *sDrapeMap)
      return 1;
  	FileName fn(*sDrapeMap);	
  	Map mp(fn);
    if (!mp.fValid())
      return 1;
  	Domain dm = mp->dm();
    if (!dm.fValid())
      return 1;
		if ( dmMap->pdcol() )
		{
   		frpr->SetVal("");
   		frpr->Hide();			
		}			
    if (!dmMap.fValid() || dm != dmMap)
    {
      dmMap = dm;
    	Representation rpr = dm->rpr();
    	if (rpr.fValid()) 
    	{
    		frpr->SetDomain(dm);
    		frpr->SetVal(rpr->fnObj.sRelative());
    		frpr->Show();
    	}
    	else
    	{
    		frpr->SetVal("");
    		frpr->Hide();
    	}
    }
		return 1;
	}
};

int Grid3DDrawer::Configure(bool fShow)
{
  if (fNew) {
    fAct = false;  // to prevent "in between" drawing
    fNew = false;
  }
  String sMap;
  if (mp.fValid())
    sMap = mp->sName();
	String sRprName;
	if (rpr.fValid())
		sRprName = rpr->sName();
  Grid3DDrForm frm(mcd->wndGetActiveView(), 
                   &fGrid, &rDist, &col,
                   &fFilled, &sMap, &sRprName, &iFillSize,
                   &fFast);
  bool fOk = frm.fOkClicked();
  if (fOk)
  {
    fAct = true;
    if (fFilled)
    {
      mp = Map(sMap);
      fFilled = mp.fValid();
    }
		else
			mp = Map();
		
		if (mp.fValid() && "" != sRprName) {
			rpr = Representation(sRprName);
			mp->dm()->SetRepresentation(rpr);
		}
		else
			rpr = Representation();
		init();
    Setup();
  }
  return fOk;
}

String Grid3DDrawer::sName()
{
	return grf3d->sName(true);
}

IlwisObject Grid3DDrawer::obj() const
{
  return mcd->georef;
}

//-- -- -- -- --
void Grid3DDrawer::init()
{
	grf3d = dynamic_cast<GeoRef3D*>(mcd->georef.ptr());
	if (!grf3d) 
	{ 
		fAct=false; 
		return; 
	}
	if (mp.fValid())
	{
		dm = mp->dm();
		if (0 != dm->pdi()) 
			drm = drmIMAGE;
		fValue = 0 != dm->pdvi() || 0 != dm->pdvr();
		if (fValue) 
			drm = drmRPR; // tijdelijk als hack
		fAttTable = mp->fTblAtt();
		ValueRange vr = mp->vr();
		if (vr.fValid() || drm == drmIMAGE)
		{
			fStretch = true;
			if (vr.fValid() && vr->vrr())
			{
				rrStretch = mp->rrPerc1(true);
				if (rrStretch.rLo() > rrStretch.rHi())
				rrStretch = mp->rrMinMax();
				if (rrStretch.rLo() > rrStretch.rHi())
				rrStretch = RangeReal(-1e12,1e12);
				riStretch.iLo() = (long)(round(rrStretch.rLo()));
				riStretch.iHi() = (long)(round(rrStretch.rHi()));
			}
			else
			{
				riStretch = mp->riPerc1(true);
				if (riStretch.iLo() > riStretch.iHi())
				riStretch = mp->riMinMax();
				rrStretch.rLo() = doubleConv(riStretch.iLo());
				rrStretch.rHi() = doubleConv(riStretch.iHi());
			}
		}
	}
	if (mp.fValid())
		grf = mp->gr();
	else if (grf3d->mapDTM.fValid())
		grf = grf3d->mapDTM->gr();
	else
		return;
	RowCol rcsz = grf->rcSize();
	rc[0] = RowCol(0L, 0L);
	rc[1] = RowCol(0L, (long)rcsz.Col-1L);
	rc[2] = RowCol((long)rcsz.Row-1L, (long)rcsz.Col-1L);
	rc[3] = RowCol((long)rcsz.Row-1L, 0L);
	MinMax mm(rc[0], rc[2]);
	cb = cbCalc(mm);
  if ( rDist == rUNDEF ) // not yet set
		rDist = rRound(min(cb.width(), cb.height() ) / 20);		;
}

int Grid3DDrawer::Setup()
{
  if (!mp.fValid()) return 0;  
    // setup is in fact partly done in init(), just to confuse
  if (fValue)
  {
    if (!fAttTable)
    {
      if (dm->pdvr())
      {
        RangeReal rr = dm->pdvr()->rrMinMax();
        if (rrStretch.rLo() < rr.rLo())
            rrStretch.rLo() = rr.rLo();
        if (rrStretch.rHi() > rr.rHi())
            rrStretch.rHi() = rr.rHi();
        riRawStretch = RangeInt(mp->vr()->iRaw(rrStretch.rLo()),
                                mp->vr()->iRaw(rrStretch.rHi()));
        riStretch    = RangeInt(longConv(rrStretch.rLo()),
                                longConv(rrStretch.rHi()));
      }
      else if (dm->pdv())
         riRawStretch = RangeInt(mp->vr()->iRaw(riStretch.iLo()),
                                 mp->vr()->iRaw(riStretch.iHi()));
    }
    if (0 != rpr->prv())
      fStretch = false;
  }


  return 0;
}

GeoRef3D *Grid3DDrawer::grfGeoRef()
{
	return grf3d;
}

bool Grid3DDrawer::fLegendUseful() const
{
	return fFilled;
}

FormBaseDialog* Grid3DDrawer::wEditRpr(CWnd* wPar, int iRaw)
{
  if (iRaw > 0 && rpr.fValid()) 
	{
    RepresentationClass* rpc = rpr->prc();
    if (rpc) {
      if (fAttTable && colAtt.fValid())
        iRaw = colAtt->iRaw(iRaw);
      return new MapRprEditForm(wPar, rpc, iRaw, true);
    }
  }
  return 0;
}

FormBaseDialog* Grid3DDrawer::wEditRpr(CWnd* wPar, const Coord& crd)
{
  long iRaw = mp->iRaw(crd);
	return wEditRpr(wPar, iRaw);
}

void Grid3DDrawer::DrawLegendRect(CDC* cdc, CRect rect, int iRaw) const
{
	Color clr = clrRaw(iRaw);
	CBrush br(clr);
	CBrush* brOld = cdc->SelectObject(&br);
	cdc->Rectangle(rect);
  cdc->SelectObject(brOld);
}

void Grid3DDrawer::DrawValueLegendRect(CDC* cdc, CRect rect, double rVal) const
{
  if (!rpr.fValid())
    return;
	Color clr = clrVal(rVal);
  if ((long)clr == -2)
		return;
	CBrush br(clr);
	CBrush* brOld = cdc->SelectObject(&br);
	cdc->Rectangle(rect);
  cdc->SelectObject(brOld);
}

RangeReal Grid3DDrawer::rrStretchRange() const
{
  if (fStretch)	
	{
    return rrStretch;
	}			
	else
		return mp->rrMinMax();
}

Map Grid3DDrawer::Drape()
{
	return mp;
}

String Grid3DDrawer::sInfo(const Coord& crd)
{
  String s;
	if (!mp.fValid())
		return grf3d->mapDTM->sValue(crd, 0);

	s = mp->sValue(crd,0)	;	
  if (fAttTable) {
    long iRaw = mp->iRaw(crd);
    if (iRaw != iUNDEF && colAtt.fValid()) 
		{
      String sAtt = colAtt->sValue(iRaw);
      for (int iLen = sAtt.length(); iLen && sAtt[iLen-1] == ' '; --iLen)
        sAtt[iLen-1] = 0;
      s &= ": ";
      s &= sAtt;
    }
  }
  return s;
}

zIcon Grid3DDrawer::icon() const
{
	return zIcon("GrfIcon");
}

#define Polygon ILWIS::Polygon
