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

#define MAPITPCN_C

#include "Applications\Raster\MAPITPCN.H"
#include "Applications\Raster\SEGRAS.H"
#include "Engine\Base\DataObjects\WPSMetaData.h"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Hs\map.hs"

IlwisObjectPtr * createMapInterpolContour(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)MapInterpolContour::create(fn, (MapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new MapInterpolContour(fn, (MapPtr &)ptr);
}

String wpsmetadataMapInterpolContour() {
	WPSMetaData metadata("MapInterpolContour");
	metadata.AddTitle("MapInterpolContour");
	metadata.AddAbstract("an operation which first rasterizes contour lines of a segment map with a value domain, and then calculates values for pixels that are not covered by segments by means of a linear interpolation");
	metadata.AddKeyword("spatial");
	metadata.AddKeyword("raster");
	metadata.AddKeyword("linestring");
	metadata.AddKeyword("interpolation");

	WPSParameter *parm1 = new WPSParameter("1","Input Segment map",WPSParameter::pmtSEGMENTMAP);
	parm1->AddAbstract("input segment map (value domain).");
	WPSParameter *parm2 = new WPSParameter("2","Georeference", WPSParameter::pmtINTEGER);
	parm2->AddAbstract("georeference that should be used for the output raster map");
	parm2->setOptional(true);

	metadata.AddParameter(parm1);
	metadata.AddParameter(parm2);
	WPSParameter *parmout = new WPSParameter("Result","Output Map", WPSParameter::pmtRASMAP, false);
	parmout->AddAbstract("Reference Outputmap and supporting data objects");
	metadata.AddParameter(parmout);
	

	return metadata.toString();
}

ApplicationMetadata metadataMapInterpolContour(ApplicationQueryData *query) {
	ApplicationMetadata md;
	if ( query->queryType == "WPSMETADATA" || query->queryType == "") {
		md.wpsxml = wpsmetadataMapInterpolContour();
	}
	if ( query->queryType == "OUTPUTTYPE" || query->queryType == "")
		md.returnType = IlwisObject::iotRASMAP;
	if ( query->queryType == "EXPERSSION" || query->queryType == "")
		md.skeletonExpression =  MapInterpolContour::sSyntax();

	return md;
}


#define HIVAL 16000

const char* MapInterpolContour::sSyntax() {
  return "MapInterpolContour(segmap,georef)\nMapInterpolContour(rasmap)";
}

ValueRange MapInterpolContour::vrDefault(const SegmentMap& segmap)
{
  RangeReal rr = segmap->rrMinMax(BaseMapPtr::mmmCALCULATE);
  if (!rr.fValid())
    rr = segmap->dvrs().rrMinMax();
	//double rRangeWidth = abs(rr.rHi() - rr.rLo());
  //double rLowestOut = rr.rLo() - rRangeWidth;
  //double rHighestOut = rr.rHi() + rRangeWidth;
  double rStep = segmap->dvrs().rStep()/10.0;
  //return ValueRange(rLowestOut, rHighestOut, rStep);
	return ValueRange(rr.rLo(), rr.rHi(), rStep);
}

MapInterpolContour* MapInterpolContour::create(const FileName& fn, MapPtr& p, const String& sExpr)
{
  Array<String> as;
  int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  if ((iParms < 1) || (iParms > 2))
    ExpressionError(sExpr, sSyntax());
  if (iParms == 1) {
    Map mp(as[0], fn.sPath());
    return new MapInterpolContour(fn, p, mp);
  }  
  SegmentMap sm(as[0], fn.sPath());
  GeoRef gr(as[1], fn.sPath());
  FileName fnSegRas = sm->fnObj;
  fnSegRas.sExt = ".mpr";
  fnSegRas.sFile &= "Rasterized";
  // mpSegRas should only be created in the fFreezing() function
  // so MapInterpolContour should not be derived from MapFromMap
  Map mpSegRas = Map(FileName::fnUnique(fnSegRas), String("MapRasterizeSegment(%S,%S)", sm->sNameQuoted(), gr->sNameQuoted())); // 8 connected !!!!
  return new MapInterpolContour(fn, p, mpSegRas, sm, gr);
}


MapInterpolContour::MapInterpolContour(const FileName& fn, MapPtr& p)
: MapFromMap(fn, p)
{
  String s;
  ReadElement("MapInterpolContour", "SegmentMap", s);
  if (s.length() != 0) {
    try {
      ReadElement("MapInterpolContour", "SegmentMap", segmap);
    }
    catch (const ErrorObject&) {
    }
    grf = gr();
  }
  fNeedFreeze = true;
  Init();
  objdep.Add(gr().ptr());
}

MapInterpolContour::MapInterpolContour(const FileName& fn, MapPtr& p,
           const Map& mp)
: MapFromMap(fn, p, mp, mfLine)
{
  if (!dvrs().fValues())
    ValueDomainError(dm()->sName(true, fnObj.sPath()), sTypeName(), errMapInterpolContour);
  fNeedFreeze = true;
  Init();
  if (!fnObj.fValid())
    objtime = objdep.tmNewest();
}

MapInterpolContour::MapInterpolContour(const FileName& fn, MapPtr& p,
           const Map& mp, const SegmentMap& sm, const GeoRef& gr)
: MapFromMap(fn, p, mp, mfLine), segmap(sm), grf(gr)
{
  if (!dvrs().fValues())
    ValueDomainError(dm()->sName(true, fnObj.sPath()), sTypeName(), errMapInterpolContour);
  if (gr->fGeoRefNone())
    throw ErrorGeoRefNone(gr->fnObj, errMapInterpolContour+1);
  fNeedFreeze = true;
  Init();
  if (segmap.fValid())
    objdep.Remove(mp); // remove raster map that has been added by 'MapFromMap' constructor
  objdep.Add(segmap.ptr());
  objdep.Add(grf.ptr());
  if (!fnObj.fValid())
    objtime = objdep.tmNewest();
}

void MapInterpolContour::Init()
{
  sFreezeTitle = "MapInterpolContour";
  htpFreeze = htpMapInterpolContourT;
}

void MapInterpolContour::Store()
{
  MapFromMap::Store();
  WriteElement("MapFromMap", "Type", "MapInterpolContour");
  if (segmap.fValid())
    WriteElement("MapInterpolContour", "SegmentMap", segmap);
}

MapInterpolContour::~MapInterpolContour()
{
}

String MapInterpolContour::sExpression() const
{
  if (segmap.fValid())
    return String("MapInterpolContour(%S,%S)",
                  segmap->sNameQuoted(true, fnObj.sPath()),
                  grf->sNameQuoted(true, fnObj.sPath()));
  else                
    return String("MapInterpolContour(%S)", mp->sNameQuoted(true, fnObj.sPath()));
}

bool MapInterpolContour::fFreezing()
{
	mp->Calc();
  trq.SetTitle(sFreezeTitle);
  trq.setHelpItem(htpFreeze);
  FileName filen=FileName::fnUnique(FileName("ic.tmp"));
  filTemp = new File(filen, facCRT);
	if ( filTemp == NULL || filTemp->m_hFile == CFile::hFileNull)
		throw ErrorObject(String(SMAPErrFileCouldNotBeCreated_S.scVal(), filen.sRelative()));
  filTemp->SetErase();
  filTemp->KeepOpen(true);

  iCurrDistToNearestContour1.Size(iCols());
  rCurrNearestContour1.Size(iCols());
  iCurrDistToNearestContour2.Size(iCols());
  rCurrNearestContour2.Size(iCols());
  iPrevDistToNearestContour1.Size(iCols());
  rPrevNearestContour1.Size(iCols());
  iPrevDistToNearestContour2.Size(iCols());
  rPrevNearestContour2.Size(iCols());
  cand.Resize(10);
  fLineChanged.Resize(iLines());
  for (long i=0; i < iLines(); ++i)
    fLineChanged[i] = true;

  long iChanges;
  bool fFirstPass = true;
  bool fStop;
  do {
    fStop = !fForwardDistances(iChanges, fFirstPass);
    if (fStop) break;
    fFirstPass = false;
    if (iChanges > 0)
      fStop = !fBackwardDistances(iChanges);
    if (fStop) break;
  } while (iChanges > 0);
  if (fStop)
    trq.ResetAborted();
  fStop = !fInterpolate();
	delete filTemp;
  iCurrDistToNearestContour1.Size(0);
  rCurrNearestContour1.Size(0);
  iCurrDistToNearestContour2.Size(0);
  rCurrNearestContour2.Size(0);
  iPrevDistToNearestContour1.Size(0);
  rPrevNearestContour1.Size(0);
  iPrevDistToNearestContour2.Size(0);
  rPrevNearestContour2.Size(0);
  fLineChanged.Resize(0);
  return !fStop;
}

bool MapInterpolContour::fSkip(long iCurLine) const
{
//   return false;  
   if (fLineChanged[iCurLine])
     return false;
   if ((iCurLine < iLines()-1) && fLineChanged[iCurLine+1])
     return false;
   if ((iCurLine > 0) && fLineChanged[iCurLine-1]) 
     return false;
   return true; 
}            

bool MapInterpolContour::fForwardDistances(long& iChanges, bool fFirstPass)
{
  int iCurLine, iCurCol,
      iK, iMinDistToNearestContour1, iMinDistToNearestContour2,
      iLineChanges;
  double rMinNearestContour1, rMinNearestContour2;
//  String sForward = SDItpForward;

  long iStart;

// iCurLine = current line of the input file
// iCurCol =  current column of the input file and the buffers
// iK = counter
// iMinDistToNearestContour1 = minimun distance from (i, j) to first nearest contour
// iMinDistToNearestContour2 = minimun distance from (i, j) to second nearest contour
// rMinNearestContour1 = the value of the first nearest contour
// rMinNearestContour2 = the value of the second nearest contour
// iLineChanges = number of changes of distances or values in a line
// iStart = position to read/write in temporary file
  short*  iCurrDistToNearestCont1;
  double* rCurrNearestCont1;
  short*  iCurrDistToNearestCont2;
  double* rCurrNearestCont2;
  short*  iPrevDistToNearestCont1;
  double* rPrevNearestCont1;
  short*  iPrevDistToNearestCont2;
  double* rPrevNearestCont2;

  iChanges = 0; iLineChanges = 0;
  trq.SetText(SMAPTextScanningForward);
  for (iCurLine = 0; iCurLine < iLines(); iCurLine++) {
    if (iLineChanges != 0) {
      String s = String(SMAPTextForwardChanges_i.scVal(), iChanges);
      trq.SetText(s);
    }
    if (trq.fUpdate(iCurLine, iLines()))
      return false;
    iLineChanges = 0;
    if (fFirstPass) {
      mp->GetLineVal(iCurLine, rCurrNearestContour1);
//      CheckErrP(mpSegRas);
      for (iCurCol = 0; iCurCol < iCols(); iCurCol++) {
        if (rCurrNearestContour1[iCurCol] == rUNDEF)
          iCurrDistToNearestContour1[iCurCol] = HIVAL;
        else
          iCurrDistToNearestContour1[iCurCol] = 0;
        iCurrDistToNearestContour2[iCurCol] = HIVAL;
        rCurrNearestContour2[iCurCol] = rUNDEF;
      }
    }
    else {
      iStart = iCurLine * 2L * (sizeof(short) + sizeof(double)) * iCols();
      filTemp->Seek(iStart);
      filTemp->Read(iCols()*sizeof(short), iCurrDistToNearestContour1.buf());
      filTemp->Read(iCols()*sizeof(double), rCurrNearestContour1.buf());
      filTemp->Read(iCols()*sizeof(short), iCurrDistToNearestContour2.buf());
      filTemp->Read(iCols()*sizeof(double), rCurrNearestContour2.buf());
//      CheckErr(filTemp);
    }
//    if (!fSkip(iCurLine))
      iCurrDistToNearestCont1 = iCurrDistToNearestContour1.buf();
      rCurrNearestCont1 = rCurrNearestContour1.buf();
      iCurrDistToNearestCont2 = iCurrDistToNearestContour2.buf();
      rCurrNearestCont2 = rCurrNearestContour2.buf();
      iPrevDistToNearestCont1 = iPrevDistToNearestContour1.buf();
      rPrevNearestCont1 = rPrevNearestContour1.buf();
      iPrevDistToNearestCont2 = iPrevDistToNearestContour2.buf();
      rPrevNearestCont2 = rPrevNearestContour2.buf();
      for (iCurCol = 0; iCurCol < iCols(); iCurCol++) {

        if (*iCurrDistToNearestCont1 != 0) {
          cand[0].iDistToNearestContour = *iCurrDistToNearestCont1;
          cand[0].rNearestContour = *rCurrNearestCont1;
          cand[1].iDistToNearestContour = *iCurrDistToNearestCont2;
          cand[1].rNearestContour = *rCurrNearestCont2;

          if (iCurCol > 0) {
            cand[2].iDistToNearestContour = *(iCurrDistToNearestCont1-1) + 5;
            cand[2].rNearestContour = *(rCurrNearestCont1- 1);
            cand[3].iDistToNearestContour = *(iCurrDistToNearestCont2-1) + 5;
            cand[3].rNearestContour = *(rCurrNearestCont2-1);
          }
          else {
            cand[2].iDistToNearestContour = HIVAL;
            cand[3].iDistToNearestContour = HIVAL;
          }

          if (iCurLine > 0) {
            cand[4].iDistToNearestContour = *iPrevDistToNearestCont1 + 5;
            cand[4].rNearestContour = *rPrevNearestCont1;
            cand[5].iDistToNearestContour = *iPrevDistToNearestCont2 + 5;
            cand[5].rNearestContour = *rPrevNearestCont2;
          }
          else {
            cand[4].iDistToNearestContour = HIVAL;
            cand[5].iDistToNearestContour = HIVAL;
          }

          if ((iCurLine > 0) && (iCurCol > 0)) {
            if ((*(iCurrDistToNearestCont1-1) != 0) &&
                              (*iPrevDistToNearestCont1 != 0)) {
              cand[6].iDistToNearestContour = *(iPrevDistToNearestCont1-1) + 7;
              cand[6].rNearestContour = *(rPrevNearestCont1-1);
              cand[7].iDistToNearestContour = *(iPrevDistToNearestCont2-1) + 7;
              cand[7].rNearestContour = *(rPrevNearestCont2-1);
            }
            else {
              cand[6].iDistToNearestContour = HIVAL;
              cand[7].iDistToNearestContour = HIVAL;
            }
          }
          else {
            cand[6].iDistToNearestContour = HIVAL;
            cand[7].iDistToNearestContour = HIVAL;
          }

          if ((iCurLine > 0) && (iCurCol < iCols() - 1)) {
            if ((*(iCurrDistToNearestCont1+1) != 0) &&
                              (*iPrevDistToNearestCont1 != 0)) {
              cand[8].iDistToNearestContour = *(iPrevDistToNearestCont1+1) + 7;
              cand[8].rNearestContour = *(rPrevNearestCont1+1);
              cand[9].iDistToNearestContour = *(iPrevDistToNearestCont2+1) + 7;
              cand[9].rNearestContour = *(rPrevNearestCont2+1);
            }
            else {
              cand[8].iDistToNearestContour = HIVAL;
              cand[9].iDistToNearestContour = HIVAL;
            }
          }
          else {
            cand[8].iDistToNearestContour = HIVAL;
            cand[9].iDistToNearestContour = HIVAL;
          }

          iMinDistToNearestContour1 = HIVAL;
          for ( iK = 0; iK <= 9; iK++) {
            if (cand[iK].iDistToNearestContour < iMinDistToNearestContour1) {
              iMinDistToNearestContour1 = cand[iK].iDistToNearestContour;
              rMinNearestContour1 = cand[iK].rNearestContour;
            }
          }

          iMinDistToNearestContour2 = HIVAL;
          for ( iK = 0; iK <= 9; iK++) {
            if (cand[iK].iDistToNearestContour < iMinDistToNearestContour2) {
              if (cand[iK].rNearestContour !=  rMinNearestContour1) {
                iMinDistToNearestContour2 = cand[iK].iDistToNearestContour;
                rMinNearestContour2 = cand[iK].rNearestContour;
              }
            }
          }

          if ( iMinDistToNearestContour2 == HIVAL)
            rMinNearestContour2 = rUNDEF;
          if ((*iCurrDistToNearestCont1 != iMinDistToNearestContour1) ||
              (*rCurrNearestCont1 != rMinNearestContour1) ||
              (*iCurrDistToNearestCont2 != iMinDistToNearestContour2) ||
              (*rCurrNearestCont2 != rMinNearestContour2)) {
            iLineChanges = iLineChanges++;
            *iCurrDistToNearestCont1 = iMinDistToNearestContour1;
            *rCurrNearestCont1 = rMinNearestContour1;
            *iCurrDistToNearestCont2 = iMinDistToNearestContour2;
            *rCurrNearestCont2 = rMinNearestContour2;
          }
        }
        iCurrDistToNearestCont1++;
        rCurrNearestCont1++;
        iCurrDistToNearestCont2++;
        rCurrNearestCont2++;
        iPrevDistToNearestCont1++;
        rPrevNearestCont1++;
        iPrevDistToNearestCont2++;
        rPrevNearestCont2++;
      }
    if (fFirstPass || iLineChanges != 0) {
      iStart = iCurLine * 2L * (sizeof(short) + sizeof(double)) * iCols();
      filTemp->Seek(iStart);
      filTemp->Write(iCols()*sizeof(short), iCurrDistToNearestContour1.buf());
      filTemp->Write(iCols()*sizeof(double), rCurrNearestContour1.buf());
      filTemp->Write(iCols()*sizeof(short), iCurrDistToNearestContour2.buf());
      filTemp->Write(iCols()*sizeof(double), rCurrNearestContour2.buf());
//      CheckErr(filTemp);
    }
    fLineChanged[iCurLine] = iLineChanges != 0;
    Swap(iPrevDistToNearestContour1, iCurrDistToNearestContour1);
    Swap(rPrevNearestContour1, rCurrNearestContour1);
    Swap(iPrevDistToNearestContour2, iCurrDistToNearestContour2);
    Swap(rPrevNearestContour2, rCurrNearestContour2);
    iChanges = iChanges + iLineChanges;
  }
  trq.fUpdate(iLines(), iLines());
  return true;
}

bool MapInterpolContour::fBackwardDistances(long& iChanges)
{
  int iCurLine, iCurCol,
      iK, iMinDistToNearestContour1, iMinDistToNearestContour2,
      iLineChanges;
  double rMinNearestContour1, rMinNearestContour2;
//  String sBackward = SDItpBackward;
//  String sChanges = SDItpChanges;

  long iStart;

// iCurLine = current line of the input file
// iCurCol =  current column of the input file and the buffers
// iK = counter
// iMinDistToNearestContour1 = minimun distance from (i, j) to first nearest contour
// iMinDistToNearestContour2 = minimun distance from (i, j) to second nearest contour
// rMinNearestContour1 = the value of the first nearest contour
// rMinNearestContour2 = the value of the second nearest contour
// iLineChanges = number of changes of distances or values in a line
// iStart = position to read/write in temporary file
  short*  iCurrDistToNearestCont1;
  double* rCurrNearestCont1;
  short*  iCurrDistToNearestCont2;
  double* rCurrNearestCont2;
  short*  iPrevDistToNearestCont1;
  double* rPrevNearestCont1;
  short*  iPrevDistToNearestCont2;
  double* rPrevNearestCont2;

  trq.SetText(SMAPTextScanningBackward);
  iChanges = 0; iLineChanges = 0;
  if (trq.fUpdate(iChanges))
    return false;
  for (iCurLine = iLines() - 1; iCurLine >= 0; iCurLine--) {
    if (iLineChanges != 0) {
      String s = String(SMAPTextBackwardChanges_i.scVal(), iChanges);
      trq.SetText(s);
    }
    if (trq.fUpdate(iCurLine, iLines()))
      return false;
    iLineChanges = 0;
//  if (fSkip(iCurLine)) {
//    fLineChanged[iCurLine] = false;
//    continue;
//  }  
    iStart = iCurLine * 2L * (sizeof(short) + sizeof(double)) * iCols();
    filTemp->Seek(iStart);
    filTemp->Read(iCols()*sizeof(short), iCurrDistToNearestContour1.buf());
    filTemp->Read(iCols()*sizeof(double), rCurrNearestContour1.buf());
    filTemp->Read(iCols()*sizeof(short), iCurrDistToNearestContour2.buf());
    filTemp->Read(iCols()*sizeof(double), rCurrNearestContour2.buf());
    
    iCurrDistToNearestCont1 = iCurrDistToNearestContour1.buf()+iCols()-1;
    rCurrNearestCont1 = rCurrNearestContour1.buf()+iCols()-1;
    iCurrDistToNearestCont2 = iCurrDistToNearestContour2.buf()+iCols()-1;
    rCurrNearestCont2 = rCurrNearestContour2.buf()+iCols()-1;
    iPrevDistToNearestCont1 = iPrevDistToNearestContour1.buf()+iCols()-1;
    rPrevNearestCont1 = rPrevNearestContour1.buf()+iCols()-1;
    iPrevDistToNearestCont2 = iPrevDistToNearestContour2.buf()+iCols()-1;
    rPrevNearestCont2 = rPrevNearestContour2.buf()+iCols()-1;
//    CheckErr(filTemp);
//    if (!fSkip(iCurLine))
      for (iCurCol = iCols() - 1; iCurCol >= 0; iCurCol--) {
        if (*iCurrDistToNearestCont1 != 0) {
          cand[0].iDistToNearestContour = *iCurrDistToNearestCont1;
          cand[0].rNearestContour = *rCurrNearestCont1;
          cand[1].iDistToNearestContour = *iCurrDistToNearestCont2;
          cand[1].rNearestContour = *rCurrNearestCont2;

          if (iCurCol < iCols() - 1) {
            cand[2].iDistToNearestContour = *(iCurrDistToNearestCont1+1) + 5;
            cand[2].rNearestContour = *(rCurrNearestCont1+1);
            cand[3].iDistToNearestContour = *(iCurrDistToNearestCont2+1) + 5;
            cand[3].rNearestContour = *(rCurrNearestCont2+1);
          }
          else {
            cand[2].iDistToNearestContour = HIVAL;
            cand[3].iDistToNearestContour = HIVAL;
          }


          if (iCurLine < iLines() - 1) {
            cand[4].iDistToNearestContour = *iPrevDistToNearestCont1 + 5;
            cand[4].rNearestContour = *rPrevNearestCont1;
            cand[5].iDistToNearestContour = *iPrevDistToNearestCont2 + 5;
            cand[5].rNearestContour = *rPrevNearestCont2;
          }
          else {
            cand[4].iDistToNearestContour = HIVAL;
            cand[5].iDistToNearestContour = HIVAL;
          }
          if ((iCurLine < iLines() - 1) && (iCurCol > 0)) {
            if ((*(iCurrDistToNearestCont1-1) != 0) &&
                              (*iPrevDistToNearestCont1 != 0)) {
              cand[6].iDistToNearestContour = *(iPrevDistToNearestCont1-1) + 7;
              cand[6].rNearestContour = *(rPrevNearestCont1-1);
              cand[7].iDistToNearestContour = *(iPrevDistToNearestCont2-1) + 7;
              cand[7].rNearestContour = *(rPrevNearestCont2-1);
            }
            else {
              cand[6].iDistToNearestContour = HIVAL;
              cand[7].iDistToNearestContour = HIVAL;
            }
          }
          else {
            cand[6].iDistToNearestContour = HIVAL;
            cand[7].iDistToNearestContour = HIVAL;
          }

          if ((iCurLine < iLines() - 1) && (iCurCol < iCols() - 1)) {
            if ((*(iCurrDistToNearestCont1+1) != 0) &&
                              (*iPrevDistToNearestCont1 != 0)) {
              cand[8].iDistToNearestContour = *(iPrevDistToNearestCont1+1) + 7;
              cand[8].rNearestContour = *(rPrevNearestCont1+1);
              cand[9].iDistToNearestContour = *(iPrevDistToNearestCont2+1) + 7;
              cand[9].rNearestContour = *(rPrevNearestCont2+1);
            }
            else {
              cand[8].iDistToNearestContour = HIVAL;
              cand[9].iDistToNearestContour = HIVAL;
            }
          }
          else {
            cand[8].iDistToNearestContour = HIVAL;
            cand[9].iDistToNearestContour = HIVAL;
          }

          iMinDistToNearestContour1 = HIVAL;
          for ( iK = 0; iK <= 9; iK++) {
            if (cand[iK].iDistToNearestContour < iMinDistToNearestContour1) {
              iMinDistToNearestContour1 = cand[iK].iDistToNearestContour;
              rMinNearestContour1 = cand[iK].rNearestContour;
            }
          }

          iMinDistToNearestContour2 = HIVAL;
          for ( iK = 0; iK <= 9; iK++) {
            if (cand[iK].iDistToNearestContour < iMinDistToNearestContour2) {
              if (cand[iK].rNearestContour !=  rMinNearestContour1) {
                iMinDistToNearestContour2 = cand[iK].iDistToNearestContour;
                rMinNearestContour2 = cand[iK].rNearestContour;
              }
            }
          }

        if ( iMinDistToNearestContour2 == HIVAL)
          rMinNearestContour2 = rUNDEF;
        if ((*iCurrDistToNearestCont1 != iMinDistToNearestContour1) ||
            (*rCurrNearestCont1 != rMinNearestContour1) ||
            (*iCurrDistToNearestCont2 != iMinDistToNearestContour2) ||
            (*rCurrNearestCont2 != rMinNearestContour2)) {
          iLineChanges = iLineChanges++;
          *iCurrDistToNearestCont1 = iMinDistToNearestContour1;
          *rCurrNearestCont1 = rMinNearestContour1;
          *iCurrDistToNearestCont2 = iMinDistToNearestContour2;
          *rCurrNearestCont2 = rMinNearestContour2;
        }
      }
      iCurrDistToNearestCont1--;
      rCurrNearestCont1--;
      iCurrDistToNearestCont2--;
      rCurrNearestCont2--;
      iPrevDistToNearestCont1--;
      rPrevNearestCont1--;
      iPrevDistToNearestCont2--;
      rPrevNearestCont2--;
    }
    fLineChanged[iCurLine] = iLineChanges != 0;
    if (iLineChanges != 0) {
      iStart = iCurLine * 2L * (sizeof(short) + sizeof(double)) * iCols();
      filTemp->Seek(iStart);
      filTemp->Write(iCols()*sizeof(short), iCurrDistToNearestContour1.buf());
      filTemp->Write(iCols()*sizeof(double), rCurrNearestContour1.buf());
      filTemp->Write(iCols()*sizeof(short), iCurrDistToNearestContour2.buf());
      filTemp->Write(iCols()*sizeof(double), rCurrNearestContour2.buf());
//      CheckErr(filTemp);
    }
    Swap(iPrevDistToNearestContour1, iCurrDistToNearestContour1);
    Swap(rPrevNearestContour1, rCurrNearestContour1);
    Swap(iPrevDistToNearestContour2, iCurrDistToNearestContour2);
    Swap(rPrevNearestContour2, rCurrNearestContour2);
    iChanges = iChanges + iLineChanges;
  }
  trq.fUpdate(0, iLines());
  return true;
}

bool MapInterpolContour::fInterpolate()
{
  int iCurCol;
  int  iCurLine;
  double rVal, rW1, rW2;
  long iStart;
  IntBuf iDist1(iCols());
  RealBuf rNearestContour1(iCols());
  IntBuf iDist2(iCols());
  RealBuf rNearestContour2(iCols());
  // iCurCol = current column of buffers
  // rVal = interpolated (elevation) value
  // rW1 =  weight of the distance to the first nearest contour
  // rW2 =  weight of the distance to the second nearest contour
  // iStart = position to read/write in temporary file
  trq.SetText(SMAPTextInterpolate);
  for (iCurLine = 0; iCurLine < iLines(); iCurLine++) {
    if (trq.fUpdate(iCurLine, iLines()))
      return false;
    iStart = iCurLine * 2L * (sizeof(short) + sizeof(double)) * iCols();
    filTemp->Seek(iStart);
    filTemp->Read(iCols()*sizeof(short), iDist1.buf());
    filTemp->Read(iCols()*sizeof(double), rNearestContour1.buf());
    filTemp->Read(iCols()*sizeof(short), iDist2.buf());
    filTemp->Read(iCols()*sizeof(double), rNearestContour2.buf());

    for (iCurCol = 0; iCurCol < iCols(); iCurCol++) {
      if ((iDist2[iCurCol] == HIVAL) || (iDist1[iCurCol] == 0))
        rVal = rNearestContour1[iCurCol];
      else {
        rW1 = (1.0 / iDist1[iCurCol]);
        rW2 = (1.0 / iDist2[iCurCol]);
        rVal = (rNearestContour1[iCurCol] * rW1 + rNearestContour2[iCurCol] * rW2) /
                                                                 (rW1 + rW2);
      }
      rNearestContour1[iCurCol] = rVal;  //shortConv(rVal);  - why rounding ?? - Wim 24/7/96
    }
    pms->PutLineVal(iCurLine, rNearestContour1);
  }
  return true;
}

bool MapInterpolContour::fDomainChangeable() const
{
  return true;
}

bool MapInterpolContour::fValueRangeChangeable() const
{
  return true;
}




