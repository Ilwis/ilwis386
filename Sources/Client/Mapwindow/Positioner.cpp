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
// Positioner.cpp: implementation of the Positioner class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\ilwis.h"
#include "Client\Base\datawind.h"
#include "Client\Mapwindow\SimpleMapPaneView.h"
#include "Client\Mapwindow\MapWindow.h"
#include "Engine\Map\Segment\Seg.h"
#include "Engine\Map\Polygon\POL.H"
#include "Client\Mapwindow\Positioner.h"
#include "Engine\Base\Zappext.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

zPoint Positioner::pntPos(Coord crd)
{
  if (crd.fUndef())
    return zPoint(0,0);
  double rRow, rCol;
  gr->Coord2RowCol(crd, rRow, rCol);
  return pntPos(rRow, rCol);
}

long Positioner::iSegPos(const ILWIS::Segment* seg, Array<zPoint>& p, const CoordSystem& cs)
{
  long iTot;
  bool fSameCsy = cs == gr->cs();
  CoordBuf crdBuf;
  CoordinateSequence *seq = seg->getCoordinates();
  p.Resize(p.iSize()+ seq->size());
  iTot = -1;
  for (int i = 0; i < seq->size(); ++i) {
    Coord c = seq->getAt(i);
    if (!fSameCsy) {
      c = gr->cs()->cConv(cs, c);
      if (c.fUndef())
        continue;
    }  
    zPoint pnt = pntPos(c);
    if (pnt.x == shUNDEF || pnt.y == shUNDEF)
      continue;
    if (iTot < 0 || p[iTot] != pnt)
      p[++iTot] = pnt;
  }
  iTot += 1;
  delete seq;
  return iTot;
}

void Positioner::iPolPos(const ILWIS::Polygon* pol, vector<Array<zPoint> >& polPoints, const CoordSystem& cs)
{
	zPoint pnt, pntLast, pntStart;
	const LineString* extRing = pol->getExteriorRing();
	CoordinateSequence *seq = extRing->getCoordinates();


	SequenceToPoints(seq, polPoints[0],cs);
	for(int j = 0; j < pol->getNumInteriorRing(); ++j) {
		delete seq;
		seq = pol->getInteriorRingN(j)->getCoordinates();
		SequenceToPoints(seq,polPoints[1 + j],cs);
	}
	delete seq;
}

void Positioner::SequenceToPoints(const CoordinateSequence * seq, Array<zPoint>& p, const CoordSystem& cs) {
	if ( seq->size() == 0)
		return;
	bool fSameCsy = cs == gr->cs();
	for (int i = 0; i < seq->size(); ++i)
	{
		Coord c = seq->getAt(i);
		if (!fSameCsy)
		{
			c = gr->cs()->cConv(cs, c);
			if (c.fUndef())
				continue;
		}    
		zPoint pnt = pntPos(c);
		if (pnt.x == shUNDEF || pnt.y == shUNDEF)
			continue;
		p.push_back(pnt);
	}
}

inline long Positioner::scale(short x, bool fInv) // Windows coordinates -> internal RowCol
{
  return scale(long(x), fInv);
}

inline long Positioner::scale(int x, bool fInv) // Windows coordinates -> internal RowCol
{
  return scale(long(x), fInv);
}

long Positioner::scale(long x, bool fInv) // Windows coordinates -> internal RowCol
{
  if (x == iUNDEF)
    return iUNDEF;
  if (rScale == 0)
    return x;
  if (fInv)
    if (rScale > 0)
      return (long)(x * rScale);
    else
      return (long)(x / -rScale);
  else
    if (rScale > 0)
      return (long)(x / rScale);
    else
      return (long)(x * -rScale);
}

long Positioner::scale(double x, bool fInv) // Windows coordinates -> internal RowCol
{
  if (x == rUNDEF)
    return iUNDEF;
  if (rScale == 0)
    return roundx(x);
  if (fInv)
    if (rScale > 0)
      return roundx(x * rScale);
    else
      return roundx(x / -rScale);
  else
    if (rScale > 0)
      return roundx(x / rScale);
    else
      return roundx(x * -rScale);
}

MinMax Positioner::mmRect(zRect r)
{
  MinMax mm;
  mm.rcMin = rcPos(r.topLeft());
  mm.rcMax = rcPos(r.bottomRight());
  return mm;
}

zPoint Positioner::pntPos(RowCol rc)
{
  double rRow, rCol;
  rRow = rc.Row;
  rCol = rc.Col;
  return pntPos(rRow+0.5,rCol+0.5);
}

zRect Positioner::rectSize()
{
  ExtRect rect = zRect(pntPos(mm.rcMin), pntPos(mm.rcMax));
  rect.Check();
  return rect;
}


DefaultPositioner::DefaultPositioner(SimpleMapPaneView* mps, MinMax minmax, 
  const GeoRef& grf)
: Positioner(grf)  
{
  rScale = mps->rScale();
  rCorr = 0.5;
  if (rScale > 1) 
    rCorr = 1.0 / rScale;
  RowCol rc = mps->rcPos(zPoint(0,0));
  iXpos  = rc.Col;
  iYpos  = rc.Row;
  mm = minmax;
}

RowCol DefaultPositioner::rcPos(zPoint p)  // Windows coordinates -> internal RowCol
{
  RowCol rc;
  rc.Row = iYpos + scale(p.y);
  rc.Col = iXpos + scale(p.x);
  return rc;
}

zPoint DefaultPositioner::pntPos(RowCol rc) // internal RowCol -> Windows coordinates
{
  zPoint p;
  long tmp;
  tmp = scale(rc.Col - iXpos, true);
  if (tmp > SHRT_MAX) tmp = SHRT_MAX;
  if (tmp < -SHRT_MAX) tmp = -SHRT_MAX;
  p.x = tmp;
  tmp = scale(rc.Row - iYpos, true);
  if (tmp > SHRT_MAX) tmp = SHRT_MAX;
  if (tmp < -SHRT_MAX) tmp = -SHRT_MAX;
  p.y = tmp;
  return p;
}

zPoint DefaultPositioner::pntPos(double rRow, double rCol) // internal RowCol -> Windows coordinates
{
  if (rRow == rUNDEF || rCol == rUNDEF)
    return zPoint(shUNDEF, shUNDEF);
  rRow -= rCorr;
  rCol -= rCorr;
  zPoint p;
  long tmp;
  tmp = scale(rCol - iXpos, true);
  if (tmp > SHRT_MAX) tmp = SHRT_MAX;
  if (tmp < -SHRT_MAX) tmp = -SHRT_MAX;
  p.x = tmp;
  tmp = scale(rRow - iYpos, true);
  if (tmp > SHRT_MAX) tmp = SHRT_MAX;
  if (tmp < -SHRT_MAX) tmp = -SHRT_MAX;
  p.y = tmp;
  return p;
}

/*
PrinterPositioner::PrinterPositioner(zDisplay* pd, MinMax minmax,
  const GeoRef& grf)
: Positioner(grf)  
{
  iPixPerInchX = pd->pixPerInchX();
  iPixPerInchY = pd->pixPerInchY();

  mm = minmax;
  iXpos = mm.MinCol();
  iYpos = mm.MinRow();

  rScaleX = (double) mm.width()  / pd->pixWidth();
  rScaleY = (double) mm.height() / pd->pixHeight();

  if (rScaleX * pd->pixPerInchX() < rScaleY * pd->pixPerInchY())
    rScaleX = rScaleY * iPixPerInchY / iPixPerInchX;
  else  
    rScaleY = rScaleX * iPixPerInchX / iPixPerInchY;
}

double PrinterPositioner::rScale() const
{
  double rRowColSize = gr->rPixSize(); // * mm->width() / grf->iCols();
  if (rRowColSize < 1e-20)
    rRowColSize = 1;
//    return rUNDEF;
  double rPixPerXmm = iPixPerInchX / 25.4;             // 25.4 mm per inch
  double rPixPerYmm = iPixPerInchY / 25.4;
  double rInvPixSizeX = rScaleX * 1000 * rPixPerXmm; // 1000 mm per m
  double rInvPixSizeY = rScaleY * 1000 * rPixPerYmm; // 1000 mm per m
  double rInvPixSize = max(rInvPixSizeX, rInvPixSizeY);
  double rSc = rInvPixSize * rRowColSize;
  return rSc;
}

RowCol PrinterPositioner::rcPos(zPoint p) // Windows coordinates -> internal RowCol
{
  RowCol rc;
  rc.Row = iYpos + p.y() * rScaleY;
  rc.Col = iXpos + p.x() * rScaleX;
  return rc;
}

zPoint PrinterPositioner::pntPos(double rRow, double rCol) // internal RowCol -> Windows coordinates
{
  if (rRow == rUNDEF || rCol == rUNDEF)
    return zPoint(shUNDEF, shUNDEF);
  zPoint p;
  long tmp;
  tmp = longConv((rCol - iXpos) / rScaleX);
  if (tmp > SHRT_MAX) tmp = SHRT_MAX;
  if (tmp < -SHRT_MAX) tmp = -SHRT_MAX;
  p.x() = tmp;
  tmp = longConv((rRow - iYpos) / rScaleY);
  if (tmp > SHRT_MAX) tmp = SHRT_MAX;
  if (tmp < -SHRT_MAX) tmp = -SHRT_MAX;
  p.y() = tmp;
  return p;
}

void PrinterPositioner::getBitmapScaling(zRect& rectPrinter, zRect& rectBitmap)
{
  rectBitmap.left() = 0;
  rectBitmap.top()  = 0;
  rectBitmap.right() = mm.width(); 
  rectBitmap.bottom() = mm.height(); 
  rectPrinter.topLeft() = Positioner::pntPos(mm.rcMin);
  rectPrinter.bottomRight() = Positioner::pntPos(mm.rcMax);
}
*/
MetafilePositioner::MetafilePositioner(MinMax minmax, const GeoRef& grf, double rSc)
: Positioner(grf)  
{
  mm = minmax;
  iXpos = mm.MinCol();
  iYpos = mm.MinRow();
	rScale = rSc;
}

MetafilePositioner::MetafilePositioner(MinMax minmax, const GeoRef& grf)
: Positioner(grf)  
{
  mm = minmax;
  iXpos = mm.MinCol();
  iYpos = mm.MinRow();
  rScale = (double) max(mm.width(), mm.height()) / 32767;
}

RowCol MetafilePositioner::rcPos(zPoint p) // Windows coordinates -> internal RowCol
{
  RowCol rc;
  rc.Row = iYpos + (long)(p.y * rScale);
  rc.Col = iXpos + (long)(p.x * rScale);
  return rc;
}

zPoint MetafilePositioner::pntPos(double rRow, double rCol) // internal RowCol -> Windows coordinates
{
  if (rRow == rUNDEF || rCol == rUNDEF)
    return zPoint(shUNDEF, shUNDEF);
  zPoint p;
  long tmp;
  tmp = (long)((rCol - iXpos) / rScale);
//  if (tmp > 32767) tmp = 32767;
//  if (tmp < 0) tmp = 0;
  p.x = tmp;
  tmp = (long)((rRow - iYpos) / rScale);
//  if (tmp > 32767) tmp = 32767;
//  if (tmp < 0) tmp = 0;
  p.y = tmp;
  return p;
}

BitmapPositioner::BitmapPositioner(double rSc, MinMax minmax, 
  const GeoRef& grf)
: Positioner(grf)  
{
  rScale = rSc;
  rCorr = 0.5;
  if (rScale > 1) 
    rCorr = 1.0 / rScale;
  mm = minmax;
  iXpos = mm.MinCol();
  iYpos = mm.MinRow();
}

RowCol BitmapPositioner::rcPos(zPoint p)  // Windows coordinates -> internal RowCol
{
  RowCol rc;
  rc.Row = iYpos + scale(p.y);
  rc.Col = iXpos + scale(p.x);
  return rc;
}

zPoint BitmapPositioner::pntPos(RowCol rc) // internal RowCol -> Windows coordinates
{
  zPoint p;
  long tmp;
  tmp = scale(rc.Col - iXpos, true);
  if (tmp > SHRT_MAX) tmp = SHRT_MAX;
  if (tmp < -SHRT_MAX) tmp = -SHRT_MAX;
  p.x = tmp;
  tmp = scale(rc.Row - iYpos, true);
  if (tmp > SHRT_MAX) tmp = SHRT_MAX;
  if (tmp < -SHRT_MAX) tmp = -SHRT_MAX;
  p.y = tmp;
  return p;
}

zPoint BitmapPositioner::pntPos(double rRow, double rCol) // internal RowCol -> Windows coordinates
{
  if (rRow == rUNDEF || rCol == rUNDEF)
    return zPoint(shUNDEF, shUNDEF);
  rRow -= rCorr;
  rCol -= rCorr;
  zPoint p;
  long tmp;
  tmp = scale(rCol - iXpos, true);
  if (tmp > SHRT_MAX) tmp = SHRT_MAX;
  if (tmp < -SHRT_MAX) tmp = -SHRT_MAX;
  p.x = tmp;
  tmp = scale(rRow - iYpos, true);
  if (tmp > SHRT_MAX) tmp = SHRT_MAX;
  if (tmp < -SHRT_MAX) tmp = -SHRT_MAX;
  p.y = tmp;
  return p;
}
/*
PrintOnScalePositioner::PrintOnScalePositioner(zDisplay* pd, MinMax minmax,
  const GeoRef& grf, double rScal)
: PrinterPositioner(pd, minmax, grf)  
{
  double rPixSize = grf->rPixSize(); // * mm->width() / grf->iCols();
  double rInvPixSize = rScal / rPixSize;
  double rScWidth  = pd->pixPerInchX() / 25.4;             // 25.4 mm per inch
  double rScHeight = pd->pixPerInchY() / 25.4;
  rScaleX = rInvPixSize / 1000.0 / rScWidth;               // 1000 mm per m
  rScaleY = rInvPixSize / 1000.0 / rScHeight;
  iDiffX = pd->pixWidth()  * rScaleX;
  iDiffY = pd->pixHeight() * rScaleY;
  if (iDiffX == 0 || iDiffY == 0) {
    iMaxPageCol = 0;
    iMaxPageRow = 0;
  }
  else {
    if (iDiffX >= mm.width())
      iMaxPageCol = 1;
    else {
      iDiffX = (pd->pixWidth()  - 10 * rScWidth) * rScaleX;    // 10 mm overlap
      iMaxPageCol = 1 + mm.width() / iDiffX;
    }
    if (iDiffY >= mm.height())
      iMaxPageRow = 1;
    else {
      iDiffY = (pd->pixHeight() - 10 * rScHeight) * rScaleY;
      iMaxPageRow = 1 + mm.height() / iDiffY;
    }
  }
  iPageRow = iPageCol = 0;
}

int PrintOnScalePositioner::iNrPages() const
{
  long iNr = (long)iMaxPageCol * iMaxPageRow;
  return shortConv(iNr);
}

int PrintOnScalePositioner::iNextPage()  // returns 0 when finished
{
  iPageCol++;
  if (iPageCol >= iMaxPageCol) {
    iPageCol = 0;
    iPageRow++;
    if (iPageRow >= iMaxPageRow)
      return 0;
  }
  iXpos = mm.MinCol() + iPageCol * iDiffX;
  iYpos = mm.MinRow() + iPageRow * iDiffY;
  return 1 + iPageCol + iMaxPageCol * iPageRow;
}

void PrintOnScalePositioner::getBitmapScaling(zRect& rectPrinter, zRect& rectBitmap)
{
  rectBitmap.left() = iXpos;
  rectBitmap.top()  = iYpos;
  int iXmax = iXpos + 1.5 * iDiffX; // overlap is always smaller than half of a page
  int iYmax = iYpos + 1.5 * iDiffY;
  if (iXmax > mm.MaxCol())
    iXmax = mm.MaxCol();
  if (iYmax > mm.MaxRow())
    iYmax = mm.MaxRow();
  rectBitmap.right() = iXmax;
  rectBitmap.bottom() = iYmax;
  if (mm.MinCol() < 0) {
    rectBitmap.left() -= mm.MinCol();
    rectBitmap.right() -= mm.MinCol();
  }
  if (mm.MinRow() < 0) {
    rectBitmap.top() -= mm.MinRow();
    rectBitmap.bottom() -= mm.MinRow();
  }
  rectPrinter.topLeft() = pntPos(iYpos, iXpos);
  rectPrinter.bottomRight() = pntPos(iYmax, iXmax);
}

*/
