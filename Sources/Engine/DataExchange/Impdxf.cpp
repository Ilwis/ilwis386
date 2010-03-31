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
/* $Log: /ILWIS 3.0/Import_Export/Impdxf.cpp $
 * 
 * 13    18-02-02 15:31 Willem
 * - Flag was not properly initialized
 * - Import tried to stored coordinates in uninitialized array
 * 
 * 12    1/12/00 15:02 Willem
 * The polygon imports now set the topology status to Non-Topological
 * 
 * 11    27/11/00 16:13 Willem
 * fSegIsPol is now set in time for LWPOLYLINE to put coordinates in the
 * correct buffer
 * 
 * 10    27/11/00 11:51 Willem
 * Added handling of LWPOLYGON sections (DXF14 and higher)
 * 
 * 9     23/10/00 12:20 Willem
 * The DomainSort constructor that accepts the number of elements in the
 * domain also creates these elements. These have the values "1" until the
 * number of elements. However these elements cannot be added anymore
 * without an error, even though no actual items have been added by the
 * user!
 * Rearranged code to add the codes in a different way
 * 
 * 8     16/03/00 16:29 Willem
 * - Replaced the obsolete trq.Message() function with MessageBox()
 * function
 * 
 * 7     29/02/00 11:33 Willem
 * MessageBox is now shown using as parent the mainWindow
 * 
 * 6     15/02/00 16:02 Willem
 * Some code cleanup
 * 
 * 5     10-01-00 4:05p Martin
 * removed vector rowcol and changed them to true coords
 * 
 * 4     9-09-99 2:39p Martin
 * ported 2.22 stuff
 * 
 * 3     17-06-99 3:59p Martin
 * // -> /*
 * 
 * 2     17-06-99 2:11p Martin
 * ported files to VS
// Revision 1.9  1998/10/19 12:01:17  Wim
// Set iNrCoord=0 when starting a polyline.
// This prevents strange results when segments occur which exist only of the same point
//
// Revision 1.8  1998-09-17 10:12:41+01  Wim
// 22beta2
//
// Revision 1.7  1998/03/23 20:44:38  Willem
// Changed the coordbounds of the map to reflect the coordinates as they appear
// in the DXF file; the coordbounds is NOT taken from the header anymore. There
// were some cases in which these boundaries were by far much larger than the
// real coordinates.
//
// Revision 1.6  1997/11/19 09:26:32  Dick
// Added test valid CoordBounds (AutoCad sets Min > Max indicating undef)
//
// Revision 1.5  1997/11/17 17:11:50  Dick
// *** empty log message ***
//
// Revision 1.4  1997/09/29 13:58:35  Willem
// Changed to import items as ID domain, when all codes are identical.
//
// Revision 1.3  1997/08/08 11:30:36  Dick
// Corrected typing mistake
//
// Revision 1.2  1997/08/08 11:20:44  Dick
// Changed to handle the exceptions via the converr.c construction.
//
/* impdxf.c
   Import AutoCad DXF
   by Willem Nieuwenhuis, march 11, 1996
   ILWIS Department ITC
   Last change:  WK   19 Oct 98   12:59 pm
*/

/*
   Revision history:
   24/03/97: Solved a problem that caused a crash when the HEADER section was there, but
             was empty. In that case the CoordBounds cbVect was set incorrectly. Added a
             check in the ScanHeader function
   24/03/97: Binary DXF files are now detected and an appropriate message is displayed;
             they are not supported.
*/
#include "Headers\toolspch.h"
#include "Engine\Map\basemap.h"
#include "Engine\DataExchange\IMPDXF.H"
#include "Engine\DataExchange\CONVERR.H"
#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\Engine.h"
#include "Engine\Base\DataObjects\ObjectCollection.h"

#define IMPORTDXF_C

// AutoCadDXF
AutoCadDXF::AutoCadDXF(const FileName& fnFile, const Tranquilizer* trq)
  : _trq(const_cast<Tranquilizer*>(trq)) {
  fileDXF = new File(fnFile);
  _iLins = 0;
  _iLinesTotal = 0;
  _iNrSeg = 0;
  _iNrPol = 0;
  _iNrPts = 0;
  _cb.Size(1001);
  _rcb.Size(1001);     // one extra needed for possible longer topologies
  fCancelled = false;
  fBoundsOK = false;
}

AutoCadDXF::~AutoCadDXF() {
  if (fileDXF) delete fileDXF;
}

void AutoCadDXF::GetNextLine() {
  if (fileDXF->fEof())
    UnexpectedEOF(fileDXF->sName());
  fileDXF->ReadLnAscii(_dg.sValue);
  _iLins++;
  if (_iLins % 25 == 0) 
    if (_iLinesTotal == 0) {
      if (_trq->fUpdate(fileDXF->iLoc() / 1000, fileDXF->iSize() / 1000))
        UserAbort(fileDXF->sName());
    }
    else
      if (_trq->fUpdate(_iLins, _iLinesTotal))
        UserAbort(fileDXF->sName());
}

/*
  Set the fErase flag for a single open ILWIS map to fRemove
  Function is called first just before the conversion starts, but 
  after the scan phase and the creation of all objects. This first
  time the map and its sevice objects are marked as erasable. In 
  case of an exception nothing will remain on disk.
  When the conversion is finished succesfully, the function is called 
  once more to mark the same objects as NOT erasable, to preserve the
  newly imported data.
*/
void AutoCadDXF::MarkErase(BaseMap& bm, bool fRemove) {
  if (!bm.fValid())
    return;
  bm->fErase = fRemove;
  if (!bm->dm()->fSystemObject()) {
    bm->dm()->fErase = fRemove;
//    if (bm->dm()->rpr().fValid())
//      bm->dm()->rpr()->fErase = fRemove;
  }
  Table tbl = bm->tblAtt();
  if (tbl.fValid()) {
    tbl->fErase = fRemove;
    for (short i = 0; i < tbl->iCols(); i++)
      if (!tbl->col(i)->dm()->fSystemObject())
        tbl->col(i)->dm()->fErase = fRemove;
  }
}

void AutoCadDXF::ReadGroup() {
  GetNextLine();
  if (_dg.sValue.length() == 0)
    FormatProblem(String(SCVTextInLine_SI.scVal(), fileDXF->sName().scVal(), iLinesRead()));
  _dg.iCode = iGroupValue();
  GetNextLine();
}

double AutoCadDXF::rGroupValue() {
  double r;
  short iRes = sscanf(_dg.sValue.scVal(), "%lf", &r);
  if (0 == iRes)
    FormatProblem(String(SCVTextInLine_SI.scVal(), fileDXF->sName().scVal(), iLinesRead()));
  return r;
}

long AutoCadDXF::iGroupValue() {
  long i;
  short iRes = sscanf(_dg.sValue.scVal(), "%ld", &i);
  if (0 == iRes)
    FormatProblem(String(SCVTextInLine_SI.scVal(), fileDXF->sName().scVal(), iLinesRead()));
  return i;
}

String AutoCadDXF::sSegmentCode() {
	short i = _sSegCode.length() - 1;
  for (; i >= 0; i--)
    if (!isspace(_sSegCode[i]))
      break;
  return _sSegCode.sLeft(i + 1);
}

Coord AutoCadDXF::cDXF() {
  Coord c;
  ReadGroup();
  c.x = rGroupValue();
  ReadGroup();
  c.y = rGroupValue();
  return c;
}

bool AutoCadDXF::fIsDXFBinary() {
  GetNextLine();
  _iLins = 0;
  fileDXF->Seek(0);   // back to the beginning
  return (_dg.sValue == "AutoCAD Binary DXF");
}

//------  Code to scan the DXF file -----------

/*
  Check in DXF header for boundary info: 
  - set the CoordSystem csVect to default csNone
  - set the CoordBounds cbVect to the found boundary. The cbVect is invalid if one
    of the Coord's is UNDEFined; the boundary must be extracted from the entire file
  The return value of true indicates a valid cbVect
*/
bool AutoCadDXF::fScanHeader(CoordSystem& csVect, CoordBounds& cbVect) {
  sReason = SCVErrWrongBoundary;
  Coord cMin, cMax;
  bool fBoundsFound = false;
  bool fLow, fHigh;

  while (!fBoundsFound) {
    ReadGroup();
    switch (iGroupCode()) {
      case 0 :
        if (sGroupValue() == "ENDSEC")
          fBoundsFound = true;
        break;
      case 9 :
        fLow  = (sGroupValue() == "$EXTMIN");
        fHigh = (sGroupValue() == "$EXTMAX");
        if ((!fLow || !fHigh) && (sGroupValue() == "CLAYER"))
          ReadGroup();
        break;
      case 10 : {
        if (fLow)
          cMin.x = rGroupValue();
        else if (fHigh)
          cMax.x = rGroupValue();
        break;
      }
      case 20 : {
        if (fLow)
          cMin.y = rGroupValue();
        else if (fHigh)
          cMax.y = rGroupValue();
        break;
      }
      default :
        break;
    }  // end switch
  }
  csVect = CoordSystem();            // Default: the unknown coordinate system

// below added check cMin < cMax because cMin > cMax is used as
// undef by AutoCad DV version 1.6
  bool fValid = (!cMin.fUndef() && !cMax.fUndef() &&
                    (cMin.x < cMax.x) && (cMin.y < cMax.y));
  return fValid;
}

bool AutoCadDXF::fScanTable() {  // no info taken from tables yet
  do {
    ReadGroup();
  }
  while (sGroupValue() != "ENDSEC");
  return true;
}

bool AutoCadDXF::fScanBlock() {  // no info taken from blocks yet
  do {
    ReadGroup();
  }
  while (sGroupValue() != "ENDSEC");
  return true;
}

void AutoCadDXF::UpdateMinMax(double& rMin, double& rMax) {
  double rVal = rGroupValue();
  if (rVal < rMin)
    rMin = rVal;
  else if (rVal > rMax)
    rMax = rVal;
}

void AutoCadDXF::UpdateMinMax(RangeReal& rr) {
  UpdateMinMax(rr.rLo(), rr.rHi());
}

void AutoCadDXF::ScanPoint(String& sCode, CoordBounds& cbTot, RangeReal& rrHeight) {
  Coord cPt;
  while (true) {
    ReadGroup();
    switch (iGroupCode()) {
      case 0 :
        if (!cPt.fUndef()) {
          cbTot += cPt;
        }
        return;
      case 8 :
        sCode = sGroupValue();
        break;
      case 10 :
        cPt.x = rGroupValue();
        break;
      case 20 : 
        cPt.y = rGroupValue();
        break;
      case 30 : 
        UpdateMinMax(rrHeight);
        break;
      default :
        break;
    }
  }
  return;  // impossible to come here
}

void AutoCadDXF::ScanLine(String& sCode, CoordBounds& cbTot, RangeReal& rrHeight) {
  Coord cStart, cEnd;
  sCode = "";
  while (true) {
    ReadGroup();
    switch (iGroupCode()) {
      case 0 :
        if (!cStart.fUndef() && !cEnd.fUndef()) {
          cbTot += cStart;
          cbTot += cEnd;
          if (cVertexPrev == cStart) {   // optimize if line pieces are connected
            fLineOptimize = true;
            cVertexPrev = cEnd;
            iNrCoord++;
            if (++iNrCoord >= 1000) {
              iSegments++;
              iNrCoord = 0;
            }
          }
          else {
            iSegments++;
            iNrCoord = 0;
            fLineOptimize = false;
          }
          if (iNrCoord == 0) {  // start anew
            cVertexPrev = cEnd;
            iNrCoord = 2;
          }
        }
        return;
      case 8 :
        sCode = sGroupValue();
        break;
      case 10 :
        cStart.x = rGroupValue();
        break;
      case 20 :
        cStart.y = rGroupValue();
        break;
      case 11 :
        cEnd.x = rGroupValue();
        break;
      case 21 :
        cEnd.y = rGroupValue();
        break;
      case 30 : 
      case 31 :
        UpdateMinMax(rrHeight);
        break;
      default :
        break;
    }  // end switch
  }    // end while
  return;   // should be impossible to come here
}

void AutoCadDXF::ScanArc(String& sCode, CoordBounds& cbTot, RangeReal& rrHeight) {
  double rRadius = rUNDEF, rStartAngle = rUNDEF, rEndAngle = rUNDEF;
  Coord cCenter;
  while (true) {
    ReadGroup();
    switch (iGroupCode()) {
      case 0 : 
        if (!cCenter.fUndef() && (rRadius != rUNDEF) &&
            (rStartAngle != rUNDEF) && (rEndAngle != rUNDEF)) {
          if (rEndAngle < rStartAngle)
            rEndAngle += 2 * M_PI;
          double rAngle = rStartAngle;
          while (rAngle < rEndAngle) {
            Coord cNew = Coord(cCenter.x + rRadius * cos(rAngle),
                               cCenter.y + rRadius * sin(rAngle));
            cbTot += cNew;
            rAngle += 0.03;
          }
        }
        return;
      case 8 :
        sCode = sGroupValue();
        break;
      case 10 :
        cCenter.x = rGroupValue();
        break;
      case 20 :
        cCenter.y = rGroupValue();
        break;
      case 30 : 
        UpdateMinMax(rrHeight);
        break;
      case 40 :
        rRadius = rGroupValue();
        break;
      case 50 : 
        rStartAngle = 2 * M_PI * rGroupValue() / 360.0;
        break;
      case 51 : 
        rEndAngle = 2 * M_PI * rGroupValue() / 360.0;
        break;
      default :
        break;
    }  // end switch
  }    // end while
  return;   // should be impossible to come here
}

/*
  Return value:
    true  : the polyline is taken to be a polygon
    false : the polyline is just a segment
*/
bool AutoCadDXF::fScanPolyline(String& sCode, CoordBounds& cbTot, RangeReal& rrHeight) {
  bool fIsPol = false;
  while (true) {
    ReadGroup();
    switch (iGroupCode()) {
      case 0 : 
        return fIsPol;

      case 8 :
        sCode = sGroupValue();
        break;
      case 30 : 
        UpdateMinMax(rrHeight);
        break;
      case 70 :
        fIsPol = (iGroupValue() & 1) == 1;
        break;
      default :
        break;
    }
  }
  return false;   // should be impossible to come here
}

bool AutoCadDXF::fScanLWPolyline(String& sCode, CoordBounds& cbTot, RangeReal& rrHeight) {
	bool fIsPol = false;
	Coord cNew, cPrevCoord;
	while (true)
	{
		ReadGroup();
		switch (iGroupCode())
		{
			case 0 : 
				if (!cNew.fUndef())
					if (abs(rBulge) < 0.001)
						cbTot += cNew;
					else
					{
						if (cPrevCoord.fUndef())
							cbTot += cNew;
						else
							ScanBulge(cbTot, cPrevCoord, cNew, rBulge);
						cPrevCoord = cNew;
					}
				return fIsPol;
			case 8 :
				sCode = sGroupValue();
				break;
			case 10:
				if (!cNew.fUndef())
				{
					cbTot += cNew;
				}
				cNew.x = rGroupValue();
				break;
			case 20:
				cNew.y = rGroupValue();
				break;
			case 38 : 
				UpdateMinMax(rrHeight);
				break;
			case 42:
				rBulge = rGroupValue();
				if (rUNDEF == rBulge)
					rBulge = 0;
				break;
			case 70 :
				fIsPol = (iGroupValue() & 1) == 1;
				break;
			default :
				break;
		}
	}
	return false;   // should be impossible to come here
}

void AutoCadDXF::ScanVertex(CoordBounds& cbTot, RangeReal& rrHeight) {
  Coord cNew;
  bool fMeshFace = false;
	bool fSplineCtrl = false;
  while (true) {
    ReadGroup();
    switch (iGroupCode()) {
      case 0 :
        if (fMeshFace || fSplineCtrl)
          return;
        if (fPolyLine) {
          if (!cNew.fUndef()) {
            if (fStartPoly) {
              cStart = cNew;
              cbTot += cNew;
              fStartPoly = false;
            }
            else
              ScanBulge(cbTot, cVertexPrev, cNew, rPrevBulge);
            cVertexPrev = cNew;   // VERTEX only reads into cNew
          }
        }
        return;

      case 8 :  // accept the name of the polyline, ignore the vertex name
        break;
      case 10 :
        if (fPolyLine)
          cNew.x = rGroupValue();
        break;
      case 20 : 
        if (fPolyLine)
          cNew.y = rGroupValue();
        break;
      case 30 : 
        if (fPolyLine)
          UpdateMinMax(rrHeight);
        break;
      case 42 :
        if (fPolyLine) {
          rBulge = rGroupValue();
          if (rUNDEF == rBulge)
            rBulge = 0;
        }
        break;
      case 70 :
        fMeshFace = (iGroupValue() & 192) == 128;
  		  fSplineCtrl = (iGroupValue() & 16) == 16;
        break;
      default :
        break;
    }
  }
  return;   // should be impossible to come here
}

/*
  Scan a new vertex of the polyline
  The new Coord is given in cNew.
  If the line piece from the previous point (in cStart) until cNew has a bulge value:
    0  : the line piece is straight, the Coord cStart is not needed
    !0 : the line piece follows a curve. The cStart is now needed to calculate the Arc
         parameters.
*/
void AutoCadDXF::ScanBulge(CoordBounds& cbTot, const Coord& cStart, const Coord& cNew, double rBulge) {
  double rAbsBulge = fabs(rBulge);
  if (rAbsBulge < 0.001) {   // Handle the regular VERTEX also
    cbTot += cNew;
    iNrCoord++;
    return;
  }
  double rDx = cNew.x - cStart.x;
  double rDy = cNew.y - cStart.y;
  double rDist = sqrt(rDx * rDx + rDy * rDy);
  bool fClockWise = rBulge < 0;
  double rHalfAlpha = 2 * atan(rAbsBulge);
  if (!fClockWise)
    rHalfAlpha *= -1;
  double rCosBeta = sin(rHalfAlpha);
  double rTanBeta = 1 / tan(rHalfAlpha);
  double rRadius = abs(rDist / (2 * rCosBeta));
  Coord cCenter = Coord(cStart.x + rDx / 2 + rDy * rTanBeta / 2,
                        cStart.y + rDy / 2 - rDx * rTanBeta / 2);
  double rGammaStart = atan2(cStart.y - cCenter.y, cStart.x - cCenter.x);
  double rGammaEnd = atan2(cNew.y - cCenter.y, cNew.x - cCenter.x);
  if (fClockWise) {
    if (rGammaStart < rGammaEnd)
      rGammaStart += 2 * M_PI;
    double rGamma = rGammaStart;
    while (rGamma > rGammaEnd) {
      Coord cNext = Coord(cCenter.x + rRadius * cos(rGamma),
                          cCenter.y + rRadius * sin(rGamma));
      cbTot += cNext;
      iNrCoord++;
      rGamma -= 0.03;
    }
  }
  else {  // Counter clockwise
    if (rGammaEnd < rGammaStart)
      rGammaEnd += 2 * M_PI;
    double rGamma = rGammaStart;
    while (rGamma < rGammaEnd) {
      Coord cNext = Coord(cCenter.x + rRadius * cos(rGamma),
                          cCenter.y + rRadius * sin(rGamma));
      cbTot += cNext;
      iNrCoord++;
      rGamma += 0.03;
    }
  }
  cbTot += cNew;
  iNrCoord++;
}

void AutoCadDXF::UpdateCodes(const String& sCode, Array<String>& asDXFCodes, bool& fFirstVal) {
  long iL;
  for (iL = 0; iL < asDXFCodes.iSize(); iL++)
    if (sCode == asDXFCodes[iL])
      break;
  if (iL == asDXFCodes.iSize())
    asDXFCodes &= sCode;
  if (fValues) {
    double rVal = sCode.rVal();
    fValues = fValues && (rVal != rUNDEF);
    if (fValues) {
      fDecimals = fDecimals || (strchr(sCode.scVal(), '.') != 0);    // detect floats
      if (fFirstVal) {
        fFirstVal = false;
        _rMin = _rMax = rVal;
      }
      else {
        _rMin = min(_rMin, rVal);
        _rMax = max(_rMax, rVal);
      }
    }
  }
}

bool AutoCadDXF::fScanInformation(CoordBounds& cbVect, CoordSystem& csVect,
                                 Array<String>& asDXFCodes,
                                 RangeReal& rrHeight, bool fBoundsOK) {
  bool fFirstX = true, fFirstY = true, fFirstZ = true;
  bool fFirstVal = true;
  String sCode;
	bool fIsPol = false;

  iNrCoord = 0;
  fPolyLine = false;
  fSegIsPol = false;
  fStartPoly = false;
  fIsPrevLine = false;
  fLineOptimize = false;
  cVertexPrev = Coord();
  ReadGroup();
  String sGroup;
  while (true) {
    switch (iGroupCode()) {
      case 0 :
        if (sCode.length() > 0) {
          UpdateCodes(sCode, asDXFCodes, fFirstVal);
          sCode = "";
        }
        sGroup = sGroupValue();
        if ((sGroup != "LINE") && (fLineOptimize || fIsPrevLine)) {   // write optimized line pieces as one segment
          iSegments++;
          iNrCoord = 0;
          fLineOptimize = false;
          fIsPrevLine = false;
        }
        if (sGroup == "LINE") {
          ScanLine(sCode, cbVect, rrHeight);  // also takes care of the segment count
          fIsPrevLine = true;
        }
        else if (sGroup == "ARC") {
          ScanArc(sCode, cbVect, rrHeight);
          iSegments++;
        }
        else if (sGroup == "POLYLINE") {
          fStartPoly = true;
          fPolyLine = true;
          rBulge = 0;
          iNrCoord = 0;
          fIsPol = fScanPolyline(sCode, cbVect, rrHeight);
          if (fIsPol) iPolygons++;
          if (!fIsPol) iSegments++;
        }
        else if (sGroup == "LWPOLYLINE") {
          fStartPoly = true;
          fPolyLine = true;
          rBulge = 0;
          iNrCoord = 0;
          fIsPol = fScanLWPolyline(sCode, cbVect, rrHeight);
          if (fIsPol) iPolygons++;
          if (!fIsPol) iSegments++;
        }
        else if (sGroup == "VERTEX") {
          rPrevBulge = rBulge;
          rBulge = 0;
          ScanVertex(cbVect, rrHeight);
        }
        else if (sGroup == "POINT") {
          ScanPoint(sCode, cbVect, rrHeight);
          iPoints++;
        }
        else if (sGroup == "SEQEND") {
          if (fPolyLine) {
            ScanBulge(cbVect, cVertexPrev, cStart, rBulge);
          }
          if (fSegIsPol)
            iMaxPolCoord = max(iMaxPolCoord, iNrCoord);
          fPolyLine = false;
          ReadGroup();
        }
        else if (sGroup == "TEXT") {
          ReadGroup();
// Is annotation: skip for now (30 may 1996)
//          sText = "";
//          iTextColor = -1;
//          iTextHeight = 1;
//          fGetText();
        }
        else if (sGroup == "ENDSEC") {
          return true;
        }
        else
          ReadGroup();
        break;
      default :
        ReadGroup();
        break;
    }  // end switch 0
  }    // end while
  return false; //should not be able to reach this point
}      // ReadInformation

bool AutoCadDXF::fScanSection(CoordBounds& cbVect, CoordSystem& csVect,
                              Array<String>& asDXFCodes, RangeReal& rrHeight) {
  bool fScanOK = true;
  while (true) {
    ReadGroup();
    String sGroup = sGroupValue();
    switch (iGroupCode()) {
      case 0 :
        if (sGroup == "ENDSEC")
          return fScanOK;
        break;
      case 2 :
        if (sGroup == "HEADER") {
          fBoundsOK = fScanHeader(csVect, cbVect);
          return fScanOK;
        }
        else if (sGroup == "TABLES") {   // skip tables for now
          fScanOK = fScanOK && fScanTable();
          return fScanOK;
        }
        else if (sGroup == "BLOCKS") {   // skip blocks
          fScanOK = fScanOK && fScanBlock();
          return fScanOK;
        }
        else if (sGroup == "ENTITIES") {
          fScanOK = fScanOK && fScanInformation(cbVect, csVect, asDXFCodes, rrHeight, fBoundsOK);
          return fScanOK;
        }
        break;
      default :
        break;
    }
  }
  return fScanOK;
}

/*
  Scan the DXF file structure to find all codes for all supported items
  and check if all these codes represent numeric values. If so the function
  sets member fValues to true, otherwise, if not entirely numeric, to false.
  Also find the boundary for the DXF file.
  The function returns false if the user interrupted the scan by pressing the 
  stop button in the tranquilizer.
*/
bool AutoCadDXF::fScanDXFInfo(CoordBounds& cbVect, CoordSystem& csVect, 
                              Array<String>& asDXFCodes, RangeReal& rrHeight) {
  _trq->SetText(SCVTextDXFImportScan);
  long iFileSize = fileDXF->iSize();
  String sCode;
  fileDXF->Seek(0);   // back to start of file
  bool fScanOK = true;
  iSegments = 0;
  iPolygons = 0;
  iPoints = 0;
  iMaxPolCoord = 0;
  fValues = true;     // Assume we can use a value domain
  fDecimals = false;  // Assume integers first
  do {
    ReadGroup();
    if (iGroupCode() != 0)
      continue;
    if (sGroupValue() == "SECTION")
      fScanOK = fScanOK && fScanSection(cbVect, csVect, asDXFCodes, rrHeight);
  }
  while ((sGroupValue() != "EOF") && fScanOK);

  // if polygons will be created reserve storage for the coordinates
  // of ALL topologies in one array.
  if (iMaxPolCoord > 0)
    _acrd.Resize(iMaxPolCoord  + 1);
    
  _trq->fUpdate(iFileSize / 1000, iFileSize / 1000);
  return fScanOK;
}

//------- Code to convert the DXF file to Ilwis

void AutoCadDXF::ReadLayer() {
  _sSegCode = "";
  while (true) 
	{
    ReadGroup();
    if (0 == iGroupCode() || 2 == iGroupCode()) 
		{
      if (0 != _sSegCode.length())
			{
        // write info into rpr
        // init color, type
			}
      _sSegCode = "";  // delete previous value
    }
    switch (iGroupCode()) 
		{
      case  0 :
        if (sGroupValue() == "ENDTAB")
          return;
        break;
      case  2 :   // Layer name
        _sSegCode = sGroupValue();
        break;
      case  6 :   // Linetype name
        sLineType = sGroupValue();
        break;
      case 62 :   // Color number
        if (iUNDEF == iGroupValue())
          //   set color undefined
        break;
      default :
        break;
    }  // end switch
  }   // end while
}

void AutoCadDXF::ReadTables() {
  while (true) {
    ReadGroup();
    switch (iGroupCode()) 
		{
      case 0 :
        if (sGroupValue() == "ENDSEC")
          return;
        break;
      case 2 :
        if (sGroupValue() == "LAYER")
          ReadLayer();
        break;
      default :
        break;
    } // end switch
  }   // end while
}

bool AutoCadDXF::fGetPoint() {
  Coord cPt;
  while (true) {
    ReadGroup();
    switch (iGroupCode()) {
      case 0 :
        if (!cPt.fUndef()) {
          _cb[0] = cPt;
          FlushPoint();
          return true;
        }
        return false;
      case 8 :
        _sSegCode = sGroupValue();
        break;
      case 10 :
        cPt.x = rGroupValue();
        break;
      case 20 : 
        cPt.y = rGroupValue();
        break;
      case 30 : 
        rHeight = rGroupValue();
        break;
      default :
        break;
    }
  }
  return false;  // impossible to come here
}

bool AutoCadDXF::fGetLine() {
  Coord cStart, cEnd;
  while (true) {
    ReadGroup();
    switch (iGroupCode()) {
      case 0 : 
        if (!cStart.fUndef() && !cEnd.fUndef()) {
          if (iNrCoord > 0) {
            if (_cb[iNrCoord - 1] == cStart) {   // optimize if line pieces are connected
              fLineOptimize = true;
              AddCoord(cEnd);
              sOptimCode = _sSegCode;
              rOptimHeight = rHeight;
            }
            else {
              FlushSegment();   // write the optimize lines first
              fLineOptimize = false;
            }
          }
          if (iNrCoord == 0) {  // start anew
            _cb[0] = cStart;
            _cb[1] = cEnd;
            iNrCoord = 2;
          }
          return true;
        }
        return false;
//        break;
      case 8 :
        _sSegCode = sGroupValue();
        break;
      case 10 :
        cStart.x = rGroupValue();
        break;
      case 20 :
        cStart.y = rGroupValue();
        break;
      case 11 :
        cEnd.x = rGroupValue();
        break;
      case 21 :
        cEnd.y = rGroupValue();
        break;
      case 30 : 
      case 31 :
        rHeight = rGroupValue();
        break;
      default :
        break;
    }  // end switch
  }    // end while
  return false;   // should be impossible to come here
}

bool AutoCadDXF::fGetArc() {
  double rRadius = rUNDEF, rStartAngle = rUNDEF, rEndAngle = rUNDEF;
  Coord cCenter;
  while (true) {
    ReadGroup();
    switch (iGroupCode()) {
      case 0 : 
        if (!cCenter.fUndef() && (rRadius != rUNDEF) &&
            (rStartAngle != rUNDEF) && (rEndAngle != rUNDEF)) {
          if (rEndAngle < rStartAngle)
            rEndAngle += 2 * M_PI;
          double rAngle = rStartAngle;
          while (rAngle < rEndAngle) {
            Coord cNew = Coord(cCenter.x + rRadius * cos(rAngle),
                               cCenter.y + rRadius * sin(rAngle));
            AddCoord(cNew);
            rAngle += 0.03;
          }
          FlushSegment();
          return true;
        }
        return false;
//        break;  // impossible to come here, of course
      case 8 :
        _sSegCode = sGroupValue();
        break;
      case 10 :
        cCenter.x = rGroupValue();
        break;
      case 20 :
        cCenter.y = rGroupValue();
        break;
      case 30 : 
        rHeight = rGroupValue();
        break;
      case 40 :
        rRadius = rGroupValue();
        break;
      case 50 : 
        rStartAngle = 2 * M_PI * rGroupValue() / 360.0;
        break;
      case 51 : 
        rEndAngle = 2 * M_PI * rGroupValue() / 360.0;
        break;
      default :
        break;
    }  // end switch
  }    // end while
  return false;   // should be impossible to come here
}

bool AutoCadDXF::fGetPolyline() {
  bool fIsPol = false;
  while (true) {
    ReadGroup();
    switch (iGroupCode()) {
      case 0 : 
        return fIsPol;

      case 8 :
        _sSegCode = sGroupValue();
        break;
      case 30 : 
        rHeight = rGroupValue();
        break;
      case 70 :
        fIsPol = (iGroupValue() & 1) == 1;
        break;
      default :
        break;
    }
  }
  return false;   // should be impossible to come here
}

bool AutoCadDXF::fGetLWPolyline() {
	fSegIsPol = false;
	Coord cNew, cPrevCoord;
	while (true)
	{
		ReadGroup();
		switch (iGroupCode())
		{
			case 0 : 
				if (!cNew.fUndef())
					if (abs(rBulge) < 0.001)
						AddCoord(cNew);
					else
					{
						if (cPrevCoord.fUndef())
							AddCoord(cNew);
						else
							AddBulge(cPrevCoord, cNew, rBulge);
						cPrevCoord = cNew;
					}
				return fSegIsPol;
				
			case 8 :
				_sSegCode = sGroupValue();
				break;
			case 10 :
				if (!cNew.fUndef())
				{
					AddCoord(cNew);   // store previous coordinate now
					cPrevCoord = cNew;
				}
				cNew.x = rGroupValue();
				break;
			case 20 : 
					cNew.y = rGroupValue();
				break;
			case 38 : 
				rHeight = rGroupValue();
				break;
			case 42 :
				rBulge = rGroupValue();
				if (rUNDEF == rBulge)
					rBulge = 0;
				break;
			case 70 :
				fSegIsPol = (iGroupValue() & 1) == 1;
				break;
			default :
				break;
		}
	}
	return false;   // should be impossible to come here
}

bool AutoCadDXF::fGetVertex() {
  Coord cNew;
  bool fMeshFace = false;
  bool fSplineCtrl = false;
  while (true) {
    ReadGroup();
    switch (iGroupCode()) {
      case 0 :
        if (fMeshFace || fSplineCtrl)
          return false;
        if (fPolyLine) {
          if (!cNew.fUndef()) {
            if (fStartPoly) {
              AddCoord(cNew);
              fStartPoly = false;
            }
            else
              AddBulge(cVertexPrev, cNew, rPrevBulge);
            cVertexPrev = cNew;   // VERTEX only reads into cNew
          }
        }
        return true;

      case 8 :  // accept the name of the polyline, ignore the vertex name
        break;
      case 10 :
        if (fPolyLine)
          cNew.x = rGroupValue();
        break;
      case 20 : 
        if (fPolyLine)
          cNew.y = rGroupValue();
        break;
      case 30 : 
        if (fPolyLine)
          rHeight = rGroupValue();
        break;
      case 42 :
        if (fPolyLine) {
          rBulge = rGroupValue();
          if (rUNDEF == rBulge)
            rBulge = 0;
        }
        break;
      case 70 :
        // mask bit 6 and 7. if only bit 7 is set the coordinate
        // is very likely to be (0,0,0) so skip it
        fMeshFace = (iGroupValue() & 192) == 128;
	      fSplineCtrl = (iGroupValue() & 16) == 16;
        break;
      default :
        break;
    }
  }
  return false;   // should be impossible to come here
}

bool AutoCadDXF::fEndSequence() {
  while (true) {
    ReadGroup();
    if (0 == iGroupCode()) {
      return true;
    }
  }
  return false;   // should be impossible to come here
}

// Imports annotation text; is not called just yet (may 30, 1996)
bool AutoCadDXF::fGetText() {
  Coord cStart;
  while (true) {
    ReadGroup();
    switch (iGroupCode()) {
      case 0 :
        if (sGroupValue().length() == 0)
          _dg.sValue = "NoName";
        if (!cStart.fUndef())
          return true;
        else
          return false;
//        break;
      case 1 : 
        _dg.sValue = sGroupValue();
        break;
      case 10 :
        cStart.x = rGroupValue();
        break;
      case 20 :
        cStart.y = rGroupValue();
        break;
      case 40 :
        iTextHeight = iGroupValue();
        if (iTextHeight == shUNDEF)
          iTextHeight = 1;
        break;
      case 62 :
        iTextColor = iGroupValue();
          if (iTextColor == shUNDEF)
            iTextColor = -1;
        break;
      default :
        break;
    }  // end switch
  }    // end while
  return false;   // should be impossible to come here
}

void AutoCadDXF::ReadInformation() {
  iNrCoord = 0;
  fPolyLine = false;
  fStartPoly = false;
  fSegIsPol = false;
  fLineOptimize = false;
  _sSegCode = "NoName";
  iNrTopPol = 0;
  ReadGroup();
  String sGroup;
  while (true) {
    switch (iGroupCode()) {
      case 0 :
        sGroup = sGroupValue();
        if ((sGroup != "LINE") && (fLineOptimize || fIsPrevLine)) {   // write optimized line pieces as one segment
          FlushSegment();
          fLineOptimize = false;
          fIsPrevLine = false;
        }
        if (sGroup == "LINE") {
          fPolyLine = false;
          fGetLine();
          fIsPrevLine = true;
        }
        else if (sGroup == "ARC") {
          fPolyLine = false;
          fGetArc();
        }
        else if (sGroup == "POLYLINE") {
          fStartPoly = true;
          fPolyLine = true;
          iNrCoord = 0;
          fTopTooLong = false;
          rBulge = 0;
          fSegIsPol = fGetPolyline();
        }
        else if (sGroup == "LWPOLYLINE") {
          fStartPoly = true;
          fPolyLine = true;
          iNrCoord = 0;
          fTopTooLong = false;
          rBulge = 0;
          fSegIsPol = fGetLWPolyline();
		  if (fSegIsPol)
			  FlushPolygon();
		  else
			  FlushSegment();
        }
        else if (sGroup == "VERTEX") {
          rPrevBulge = rBulge;
          rBulge = 0;
          fGetVertex();
        }
        else if (sGroup == "POINT") {
          fGetPoint();
        }
        else if (sGroup == "SEQEND") {
          if (fSegIsPol) {
            if (iNrTopPol == 0)
              AddBulge(cVertexPrev, _acrd[0], rBulge);   // close the polygon when only one segment
            FlushPolygon();    // write last topology
          }
          else
            FlushSegment();
          fSegIsPol = false;
          fPolyLine = false;
          _sSegCode = "NoName";
          ReadGroup();
        }
        else if (sGroup == "TEXT") {
          ReadGroup();
// Is annotation: skip for now (30 may 1996)
//          sText = "";
//          iTextColor = -1;
//          iTextHeight = 1;
//          fGetText();
        }
        else if (sGroup == "ENDSEC") {
          return;
        }
        else
          ReadGroup();
        break;
      default :
        ReadGroup();
        break;
    }  // end switch 0
  }    // end while
}      // ReadInformation

void AutoCadDXF::ReadHeader() {  // Skip: header info already parsed
  do {
    ReadGroup();
  }
  while ((sGroupValue() != "ENDSEC"));
}

void AutoCadDXF::ReadBlocks() {  // no info taken from blocks yet
  do {
    ReadGroup();
  }
  while ((sGroupValue() != "ENDSEC"));
}

void AutoCadDXF::ReadUnknownSection() 
{
  do {
    ReadGroup();
  }
  while ((sGroupValue() != "ENDSEC"));
}

void AutoCadDXF::ReadSection() {
  ReadGroup();
  while (true) {
    String sGroup = sGroupValue();
    switch (iGroupCode()) {
      case 0 :
        if (sGroup == "ENDSEC")
          return;
        break;
      case 2 :
        if (sGroup == "HEADER") {
          ReadHeader();
          break;
        }
        else if (sGroup == "TABLES") {
          ReadTables();
          break;
        }
        else if (sGroup == "BLOCKS") {
          ReadBlocks();
          break;
        }
        else if (sGroup == "ENTITIES") {
          ReadInformation();
          break;
        }
        else {  // unrecognized sections go here
          ReadUnknownSection();
          break;
        }
      default :
        ReadGroup();
        break;
    }
  }
}

void AutoCadDXF::FlushPoint() {
  if (fUseHght)
    _pt->iAddVal(_cb[0], rHeight);
  else
    if (fUseID)
      _pt->iAddVal(_cb[0], String("%li", _iNrPts + 1));
    else
      _pt->iAddVal(_cb[0], sSegmentCode());
  _iNrPts++;
  iNrCoord = 0;
}

void AutoCadDXF::FlushSegment() {
  if (iNrCoord < 2)
    return;
  ILWIS::Segment *segCur = CSEGMENT(_sm->newFeature());
  segCur->PutCoords(iNrCoord, _cb);
  if (fUseHght) {
    if (fLineOptimize)
      segCur->PutVal(rOptimHeight);
    else
      segCur->PutVal(rHeight);
  }
  else {
    if (fLineOptimize)
      segCur->PutVal(_sm->dvrs(), sOptimCode);
    else
      if (fUseID)
        segCur->PutVal(_iNrSeg + 1L);
      else
		  segCur->PutVal(_sm->dvrs(),sSegmentCode());
  }
  _iNrSeg++;
  iNrCoord = 0;
}

void AutoCadDXF::FlushPolygon() {
	if (iNrCoord < 2)
		return;
	if (!_pm->fCalculated())
		return;

	vector<Coordinate> *coords = new vector<Coordinate>();
	for (short i = 0; i < iNrCoord; i++) {
		coords->push_back(_acrd[i]);
	}

	CoordinateSequence *seq = new CoordinateArraySequence(coords);
	LinearRing *ring = new LinearRing(seq, new GeometryFactory());
	ILWIS::Polygon *pol = CPOLYGON(_pm->newFeature());
	pol->addBoundary(ring);


	String sVal;
	if (fUseHght)
		sVal = String("%10.1f", rHeight);
	else {
		if (fUseID)
			sVal = String("%li", _iNrPol + 1);
		else
			sVal = sSegmentCode();
	}
	pol->PutVal(_pm->dvrs(), sVal);
	iNrCoord = 0;
}

void AutoCadDXF::AddCoord(const Coord& cNew)
{
	if (fSegIsPol)
	{
		if (iNrCoord >= _acrd.size() )
		{
			if (_acrd.size() == 0)
				_acrd.Resize(1000);
			else
				_acrd.Resize(_acrd.size() * 2);
		}
		_acrd[iNrCoord] = cNew;
		iNrCoord++;
	}
	else
	{
		_cb[iNrCoord] = cNew;
		iNrCoord++;
		if (iNrCoord >= 1000)
		{
			FlushSegment();
			_cb[0] = _cb[1000 - 1];   // keep node as start for next segment
			iNrCoord = 1;
		}
	}
}

/*
  Add a new vertex of the polyline to the coordinate buffer
  The new Coord is given in cNew.
  If the line piece from the previous point (in cStart) until cNew has a bulge value:
    0  : the line piece is straight, the Coord cStart is not needed
    !0 : the line piece follows a curve. The cStart is now needed to calculate the Arc
         parameters.
*/
void AutoCadDXF::AddBulge(const Coord& cStart, const Coord& cEnd, double rBulge) {
  double rAbsBulge = fabs(rBulge);
  if (rAbsBulge < 0.001) {   // Handle the regular VERTEX also
    if (cEnd != cStart)
      AddCoord(cEnd);
    return;
  }
  double rDx = cEnd.x - cStart.x;
  double rDy = cEnd.y - cStart.y;
  double rDist = sqrt(rDx * rDx + rDy * rDy);
  bool fClockWise = rBulge < 0;
  double rHalfAlpha = 2 * atan(rAbsBulge);
  if (!fClockWise)
    rHalfAlpha *= -1;
  double rCosBeta = sin(rHalfAlpha);
  double rTanBeta = 1 / tan(rHalfAlpha);
  double rRadius = abs(rDist / (2 * rCosBeta));
  Coord cCenter = Coord(cStart.x + rDx / 2 + rDy * rTanBeta / 2,
                        cStart.y + rDy / 2 - rDx * rTanBeta / 2);
  double rGammaStart = atan2(cStart.y - cCenter.y, cStart.x - cCenter.x);
  double rGammaEnd = atan2(cEnd.y - cCenter.y, cEnd.x - cCenter.x);
  if (fClockWise) {
    if (rGammaStart < rGammaEnd)
      rGammaStart += 2 * M_PI;
    double rGamma = rGammaStart;
    while (rGamma > rGammaEnd) {
      Coord cNew = Coord(cCenter.x + rRadius * cos(rGamma),
                         cCenter.y + rRadius * sin(rGamma));
      AddCoord(cNew);
      rGamma -= 0.03;
    }
  }
  else {  // Counter clockwise
    if (rGammaEnd < rGammaStart)
      rGammaEnd += 2 * M_PI;
    double rGamma = rGammaStart;
    while (rGamma < rGammaEnd) {
      Coord cNew = Coord(cCenter.x + rRadius * cos(rGamma),
                         cCenter.y + rRadius * sin(rGamma));
      AddCoord(cNew);
      rGamma += 0.03;
    }
  }
  AddCoord(cEnd);
}

static int iCompCodes(const String* s1, const String* s2) {
  return stricmp((*s1).scVal(), (*s2).scVal());
}

bool AutoCadDXF::SetupAllMaps(const FileName& fnObject, const FileName& fnFile) {
  if (fIsDXFBinary()) 
	{
	  getEngine()->Message(SCVErrNoBinaryDXFAllow.scVal(),
                                 SCVTitleImportFromDXF.scVal(), 
                                 MB_OK | MB_ICONEXCLAMATION);
    return false;
  }
  CoordSystem csDXF;
  CoordBounds cbDXF;
  Array<String> asDXFCodes;
  RangeReal rrHeight;
  if (!fScanDXFInfo(cbDXF, csDXF, asDXFCodes, rrHeight))
    return false;

  _iLinesTotal = _iLins;
/*
  Now check if the number of classes is larger than 1.
  If not check if there is height info with a minimal difference of at
  least 10 meters. If so create a height domain (domain value)
  Otherwise if all 'classes' represent numeric values create a domain value.
  If no value domain is applicable, stick to class domain
*/
  bool fHeight = ((rrHeight.rLo() > -DBL_MAX) && (rrHeight.rHi() < DBL_MAX));
  fUseHght = false;
  if (fHeight) {
    fUseHght = (asDXFCodes.iSize() <= 1 ) && fHeight;
    if (rrHeight.rHi() - rrHeight.rLo() < 10.0)
      fUseHght = false;
  }
  fileDXF->Seek(0);   // back to the beginning
  _iLins = 0;


  ObjectCollection col(FileName(fnObject,".ioc"),"ObjectCollection",ParmList());
  col->fErase = true;
  FileName fnDomain = FileName::fnUnique(FileName(fnObject, ".dom"));
  Domain dm;
  ValueRange vr;
  fUseID = false;
  if (fValues && (_rMax - _rMin > 1.0)) {
    dm = Domain(FileName("value", ".DOM"));
    if (fDecimals)
      vr = ValueRangeReal(_rMin, _rMax, 0.01);
    else
      vr = ValueRangeInt(longConv(_rMin), longConv(_rMax));
  }
  else if (fUseHght) {
    dm = Domain(FileName("value", ".DOM"));
    vr = ValueRangeReal(rrHeight.rLo(), rrHeight.rHi(), 0.1);
  }
  else {
    _trq->SetText(SCVTextNewDomain);
    if (asDXFCodes.iSize() == 0) {
      fUseID = true;
    }
    else {
      typedef int (*iFncCmp)(const void*, const void*);
      qsort(asDXFCodes.buf(), asDXFCodes.iSize(), sizeof(String), (iFncCmp)iCompCodes);
      dm = Domain(FileName::fnUnique(FileName(fnObject, ".dom")), 0 /*asDXFCodes.iSize()*/, dmtCLASS);
      DomainSort *pdsrt = dm->pdsrt();
      pdsrt->dsType = DomainSort::dsMANUAL;

	  vector<String> values;
      for (short iC = 0; iC < asDXFCodes.iSize(); iC++)
        values.push_back(asDXFCodes[iC]);
	  pdsrt->AddValues(values);
    }
  }

  DomainValueRangeStruct dvs(dm, vr);
  String sDXF = String("%S%S", fnFile.sFile, fnFile.sExt);
  if (iSegments > 0) {
    if (fUseID) {
      dm = Domain(FileName::fnUnique(FileName(fnObject, ".dom")), iSegments, dmtID, "Seg ");
      dvs = DomainValueRangeStruct(dm, ValueRange());
    }
    SegmentMap sm(FileName(fnObject,".mps"), csDXF, cbDXF, dvs);
    sm->sDescription  = SCVTextSegmentMap;
    sm->sDescription &= SCVText_ImportedFrom_;
    sm->sDescription &= sDXF;
    sm->Store();
	col->Add(sm);
	if ( sm->dm()->fSystemObject() == false)
		col->Add(sm->dm());
  }
  if (iPolygons > 0) {
    if (fUseID) {
      dm = Domain(FileName::fnUnique(FileName(fnObject, ".dom")), iPolygons, dmtID, "Pol ");
      dvs = DomainValueRangeStruct(dm, ValueRange());
    }
    PolygonMap pm(FileName(fnObject,".mpa"), csDXF, cbDXF, dvs);
    pm->sDescription  = SCVTextPolygonMap;
    pm->sDescription &= SCVText_ImportedFrom_;
    pm->sDescription &= sDXF;
	pm->TopologicalMap(false);
    pm->Store();
	col->Add(pm);
	if ( pm->dm()->fSystemObject() == false)
		col->Add(pm->dm());
  }
  if (iPoints > 0) {
    if (fUseID) {
      dm = Domain(FileName::fnUnique(FileName(fnObject, ".dom")), iPoints, dmtID, "Pnt ");
      dvs = DomainValueRangeStruct(dm, ValueRange());
    }
    PointMap pt(FileName(fnObject,".mpp"), csDXF, cbDXF, dvs);
    pt->sDescription  = SCVTextPointMap;
    pt->sDescription &= SCVText_ImportedFrom_;
    pt->sDescription &= sDXF;
    pt->Store();
	col->Add(pt);
	if ( pt->dm()->fSystemObject() == false)
		col->Add(pt->dm());
  }
  if ( col->iNrObjects() > 0)
	  col->fErase = false;
  return true;
}

void AutoCadDXF::SetAllMaps(SegmentMap& sm, PolygonMap& pm, PointMap& pt) {
  _sm = sm;
  _pm = pm;
  _pt = pt;
}

void ImpExp::ImportDXF(const FileName& fnFile, const FileName& fnObject ) {
  try {
    trq.SetTitle(SCVTitleImportFromDXF);
    trq.fUpdate(0);                           // make the reportwindow fill out immediately
    AutoCadDXF dxf(fnFile, &trq);
    if (!dxf.SetupAllMaps(fnObject, fnFile))  // create the segment (& polygon) map
      return;
    SegmentMap sm;
    PolygonMap pm;
    PointMap pt;
    if (dxf.iSegments > 0)
      sm = SegmentMap(FileName(fnObject,".mps"));           // reuse the segment map, forcing a mapstore
    if (dxf.iPolygons > 0)
		pm = PolygonMap(FileName(fnObject,".mpa"));           // if created reuse polygon map
    if (dxf.iPoints > 0)
      pt = PointMap(FileName(fnObject,".mpp"));
    if (dxf.iSegments + dxf.iPolygons + dxf.iPoints == 0) {
      getEngine()->Message(SCVWarnNoElements.scVal(),
                    SCVTitleImportFromDXF.scVal(),
                    MB_OK | MB_ICONEXCLAMATION);
      UserAbort();
    }
    dxf.sReason = SCVErrCreateVector;
    if (dxf.iSegments > 0) {
      if (sm->fDependent()) {
        dxf.sReason &= String(SCVTextInLine_l.scVal(), dxf.iLinesRead());
        ErrorObject(dxf.sReason, 6500).Show();
        return;
      }
    }
    dxf.SetAllMaps(sm, pm, pt);
    RowColBuf rcb;
    rcb.Size(1000);
    CoordBuf cbSegment(1000);
    dxf.sReason = SCVErrIllegalDXF;
    if (dxf.iSegments > 0) {
      sm->Updated();
      dxf.MarkErase(sm, true);
    }
    if (dxf.iPolygons > 0) {
      pm->Updated();
      dxf.MarkErase(pm, true);
    }
    if (dxf.iPoints > 0) {
      pt->Updated();
      dxf.MarkErase(pt, true);
    }
    trq.SetText(SCVTextProcessing);
    do {
      dxf.ReadGroup();
      if (dxf.iGroupCode() != 0)
        continue;
      if (dxf.sGroupValue() == "SECTION")
        dxf.ReadSection();
    }
    while ((dxf.sGroupValue() != "EOF")/* && !dxf.fCancelled*/);
    if (dxf.iSegments > 0)
      dxf.MarkErase(sm, false);
    if (dxf.iPolygons > 0)
      dxf.MarkErase(pm, false);
    if (dxf.iPoints > 0)
      dxf.MarkErase(pt, false);
  }
  catch (AbortedByUser& ) {  // just stop, no message
  }
  catch (ErrorObject& err) {
    err.Show();
  }
}


