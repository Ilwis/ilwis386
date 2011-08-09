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
/* $Log: /ILWIS 3.0/RasterApplication/Mapresmp.cpp $
 * 
 * 12    15-01-03 9:58 Hendrikse
 * if (!fGeoRefNoneAllowed && gr->fGeoRefNone()), like in the case of
 * making a pair of air-photo's with georefNone epipolar, we want to allow
 * resampling. Thus it should not be refused because of the fact that the
 * target georef ScaleRotate is derived from (and hence itself also) a
 * georef with fGeorefNone true.
 * 
 * 11    14-10-02 18:29 Koolhoven
 * enabled bilinear resampling for Color maps
 * 
 * 10    14-10-02 17:55 Koolhoven
 * adapted layout of fFreezing()
 * 
 * 9     14-10-02 17:51 Koolhoven
 * in fFreezing() removed check on max size of map
 * 
 * 8     27-09-02 10:52 Willem
 * Calculation of maximum size is now improved. The replaced erroneous
 * calculation always gave one byte per pixel as a result
 * 
 * 7     3-10-01 20:51 Retsios
 * Added 5th (undocumented) parameter to MapResample, to resample with
 * GeoRefNone when no georef is available.
 * 
 * 6     15/02/01 14:18 Willem
 * Disable use of attribute maps. Resample expects to use a patched map,
 * which an inline attribute map most likely is not.
 * 
 * 5     25-10-00 15:39 Hendrikse
 * added in fFreezing 	mpInpMap.SetPointer(0); to force the intermediate
 * map to be deleted
 * 
 * 4     25/08/00 17:35 Willem
 * Resample will now stop when the Map GeoRef and the target GeoRef are
 * equal
 * 
 * 3     9/08/99 8:57a Wind
 * changed sName() to sNameQuoted() in sExpression() to support long file
 * names
 * 
 * 2     3/11/99 12:16p Martin
 * Added support for Case insesitive 
// Revision 1.8  1998/10/16 08:35:51  Wim
// iNrPatchCols, iNrPatchRows and iNrPatches are now members of the class
// initialized in the fFreezing function instead of static variables.
//
// Revision 1.7  1998-09-16 18:24:31+01  Wim
// 22beta2
//
// Revision 1.6  1997/08/27 15:45:11  Wim
// throw ErrorResample when invalid resample method found
//
// Revision 1.5  1997-08-19 10:18:06+02  martin
// Protected against too large georefs
//
// Revision 1.4  1997/08/18 10:04:25  martin
// *** empty log message ***
//
// Revision 1.3  1997/08/13 09:59:44  martin
// Undef parts of integer maps got the value of 127, i suppose this was a leftover from debugging
//
// Revision 1.2  1997/08/05 15:27:36  Wim
// sSyntax() updated
//
/* MapResample
   Copyright Ilwis System Development ITC
	Last change:  WK   16 Oct 98    9:26 am
*/

#include "Applications\Raster\Mapresmp.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Base\DataObjects\WPSMetaData.h"
#include "Engine\Base\DataObjects\Patch.h"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Hs\map.hs"

IlwisObjectPtr * createMapResample(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)MapResample::create(fn, (MapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new MapResample(fn, (MapPtr &)ptr);
}

String wpsmetadataMapResample() {
	WPSMetaData metadata("MapResample");
	return metadata.toString();
}

ApplicationMetadata metadataMapResample(ApplicationQueryData *query) {
	ApplicationMetadata md;
	if ( query->queryType == "WPSMETADATA" || query->queryType == "") {
		md.wpsxml = wpsmetadataMapResample();
	}
	if ( query->queryType == "OUTPUTTYPE" || query->queryType == "")
		md.returnType = IlwisObject::iotRASMAP;
	if ( query->queryType == "EXPERSSION" || query->queryType == "")
		md.skeletonExpression =  MapResample::sSyntax();

	return md;
}


const char * MapResample::sSyntax()
 { return "MapResample(map,georef,resamplemethod)\n"
"MapResample(map,georef,resamplemethod,Patch|NoPatch)"
"ResampleMethod=NearestNeighbour,BiLinear,BiCubic)\n"; }
// 5th parameter (AllowGrNone) is undocumented for the time being, so don't include
// in syntax

static const char * sResampleMethods[] = { "NearestNeighbour", "BiLinear", "BiCubic", 0 };

static int iFind(const String& s, const char* sArr[])
{
  int i = 0; 
  while (sArr[i]) {
    if (_strnicmp(sArr[i], s.c_str(), s.length()) == 0)
      return i;
    i++;
  }
  return shUNDEF;
}

class DATEXPORT ErrorResample: public ErrorObject
{
public:
  ErrorResample(const String& sResampleType, const WhereError& where)
  : ErrorObject(WhatError(String(TR("Invalid Resample Method: '%S'").c_str(), sResampleType), errMapResample), where) {}
};

static void GeoRefNoneError(const FileName& fn, IlwisError err)
{
  throw ErrorGeoRefNone(fn, err);
}

void ThrowGeorefError(const String& sMessage, const FileName& fn)
{
    throw ErrorObject(WhatError(sMessage, errMapResample+6), fn);
}

MapResample* MapResample::create(const FileName& fn, MapPtr& p, const String& sExpr)
{
	Array<String> as;
	int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
	if ((iParms < 3) || (iParms > 5))
		ExpressionError(sExpr, sSyntax());

	// Disable use of attribute maps. Resample expects to use a patched map, which an
	// inline attribute map most likely is not
	String sInputMapName = as[0];
	char *pCh = sInputMapName.strrchrQuoted('.');
	if ((pCh != 0) && (0 != _strcmpi(pCh, ".mpr")))  // attrib map
		throw ErrorObject(WhatError(String(TR("Use of attribute maps is not possible: '%S'").c_str(), as[0]), errMapResample), fn);
	
	Map mp(as[0], fn.sPath());
	GeoRef gr = GeoRef(as[1], fn.sPath());
	GeoRefPtr* pgr = gr.ptr();
	if (mp->gr()->fEqual(*pgr))
		throw ErrorObject(String(TR("GeoRef of Map %S and GeoRef %S are equal, no resample needed").c_str(), mp->sName(), gr->sName()));
	
	int iRsmMeth = iFind(as[2], sResampleMethods);
	if (shUNDEF == iRsmMeth)
		throw ErrorResample(as[2], fn);
	ResampleMethod rm = ResampleMethod(iRsmMeth);
	bool fPatch = true;
	if (iParms == 4) {
		if ((!fCIStrEqual(as[3] , "Patch")) && (!fCIStrEqual(as[3] , "NoPatch")))
			throw ErrorObject(WhatError(String(TR("Invalid 4th parameter (Patch|NoPatch) '%S'").c_str(), as[3]), errMapResample+5), fn);
		fPatch = fCIStrEqual(as[3] , "Patch");
	}
	bool fGrNoneAllowed = false;
	if (iParms == 5) {
		if ((!fCIStrEqual(as[4] , "GrNoneAllowed")) && (!fCIStrEqual(as[4] , "GrNoneDenied")))
			throw ErrorObject(WhatError(String(TR("Invalid 5th parameter (GrNoneAllowed|GrNoneDenied) '%S'").c_str(), as[4]), errMapResample+6), fn);
		fGrNoneAllowed = fCIStrEqual(as[4] , "GrNoneAllowed");
	}
	return new MapResample(fn, p, mp, gr, rm, fPatch, fGrNoneAllowed);
}

MapResample::MapResample(const FileName& fn, MapPtr& p)
: MapFromMap(fn, p), rPatchBuffer(0), iPatchBuffer(0), fPatch(true)
{
  String s;
  fNeedFreeze = true;
  ReadElement("MapResample", "ResampleMethod", s);
  if (0 == ReadElement("MapResample", "Patch", fPatch))
    fPatch = true;
  rm = rmType(s);
  ReadGeoRefFromFileAndSet();
  try {
    // if (mp->gr()->fGeoRefNone())
    //  GeoRefNoneError(mp->fnObj, errMapResample+2);
    if (!cs()->fConvertFrom(mp->gr()->cs()))
      IncompatibleCoordSystemsError(cs()->sName(true, fnObj.sPath()), mp->gr()->cs()->sName(true, fnObj.sPath()), sTypeName(), errMapResample+4);
  }
  catch (const ErrorObject& err) {
    err.Show();
  }  
  Init();
  objdep.Add(gr().ptr());
}

MapResample::MapResample(const FileName& fn, MapPtr& p, const Map& mp, const GeoRef& gr,
                         ResampleMethod rsm, bool fPat, bool fGeoRefNoneAllowed)
: MapFromMap(fn, p, mp), rm(rsm), rPatchBuffer(0), iPatchBuffer(0), fPatch(fPat)
{
  if ((rsm != rmNEARNEIGHB) && !dvrs().fValues())
    if ((rsm != rmBILINEAR) && 0 != dm()->pdcol())
    ValueDomainError(dm()->sName(true, fnObj.sPath()), fnObj, errMapResample+1);
  if (!fGeoRefNoneAllowed && mp->gr()->fGeoRefNone())
    GeoRefNoneError(mp->fnObj, errMapResample+2);
  if (!fGeoRefNoneAllowed && gr->fGeoRefNone())
    GeoRefNoneError(fnObj, errMapResample+3);
  if (!gr->cs()->fConvertFrom(mp->gr()->cs()))
    IncompatibleCoordSystemsError(gr->cs()->sName(true, fnObj.sPath()), mp->gr()->cs()->sName(true, fnObj.sPath()), sTypeName(), errMapResample+4);
  ptr.SetSize(gr->rcSize());
  ptr.SetGeoRef(gr);
  fNeedFreeze = true;
  Init();
  objdep.Add(gr.ptr());
  if (!fnObj.fValid())
    objtime = objdep.tmNewest();
}

void MapResample::Init()
{
  sFreezeTitle = "MapResample";
  htpFreeze = "ilwisapp\\resample_algorithm.htm";
}

void MapResample::Store()
{
  MapFromMap::Store();
  WriteElement("MapFromMap", "Type", "MapResample");
  WriteElement("MapResample", "ResampleMethod", sResampleMethod(rm));
  WriteElement("MapResample", "Patch", fPatch);
}

MapResample::~MapResample()
{
  int i;
  if (0 != rPatchBuffer) {
    for(i=0; i<MAX_SIZE_PATCH_BUFFER; ++i)
      delete rPatchBuffer[i];
    delete [] rPatchBuffer;
 }
  if (0 != iPatchBuffer) {
    for(i=0; i<MAX_SIZE_PATCH_BUFFER; ++i)
      delete iPatchBuffer[i];
    delete [] iPatchBuffer;
  }
}

bool MapResample::fFreezing()
{
  bool fColorDomain = false;
	bool fUseReal = (rm != rmNEARNEIGHB) || (st() == stREAL);
  // color maps behave different
  if (dm()->pdcol()) 
  {
    fUseReal = false;
    fColorDomain = true;
  }
	
	if (fPatch)
	{
		mpInpMap = Map(FileName::fnUnique(fnObj), mp->gr(), mp->rcSize(), mp->dvrs(), mfPatch);
		mpInpMap->Store();
		mpInpMap->fErase = true;
		MapPtr* p = mpInpMap.ptr();
		switch ( mpInpMap->st() )
		{
		case stBYTE: case stNIBBLE: case stDUET: case stBIT :
			if (!mp->fPatchByte(p, sFreezeTitle, trq))
				return false;
			break;
		case stINT:
			if (!mp->fPatchInt(p, sFreezeTitle, trq))
				return false;
			break;
		case stLONG:
			if (!mp->fPatchLong(p, sFreezeTitle, trq))
				return false;
			break;
		case stREAL:
			if (!mp->fPatchReal(p, sFreezeTitle, trq))
				return false;
			break;
		}
		if (fUseReal)
		{
			rPatchBuffer = new RealPatch*[MAX_SIZE_PATCH_BUFFER];
			for(int i=0; i< MAX_SIZE_PATCH_BUFFER; ++i) {
				rPatchBuffer[i]=NULL;
				iPatchPresent[i]=-1;
			}
		}
		else
		{
			iPatchBuffer = new LongPatch*[MAX_SIZE_PATCH_BUFFER];
			for(int i=0; i< MAX_SIZE_PATCH_BUFFER; ++i) {
				iPatchBuffer[i]=NULL;
				iPatchPresent[i]=-1;
			}
		}
	}
	csIn = mp->gr()->cs();
	csOut = cs();
	fTransformCoords = csIn != csOut;
	
	long iNrLines=iLines(), iNrCols=iCols();
	iInpCols = mp->iCols();
	iInpLines = mp->iLines();
	iNrPatchCols = (iInpCols-1) / PATCH_SIDE + 1;
	iNrPatchRows = (iInpLines-1) / PATCH_SIDE + 1;
	iNrPatches = iNrPatchCols * iNrPatchRows;
	
	// walk through the output map
	trq.SetText(String(TR("Resampling map '%S'").c_str(), mp->sName(true, mp->fnObj.sPath())));
	RealBuf rBuf(iCols());
	LongBuf iBuf(iCols());
	switch (rm) {
    case rmNEARNEIGHB: 
    {
	  	GeoRef grIn = mp->gr();
  		for (long i=0; i < iNrLines; ++i) 
      {
  			RowCol rc(i, 0L);
  			if (trq.fUpdate(i, iNrLines))
  				return false;
  			for (long j=0; j < iNrCols; ++j, rc.Col++) {
  				Coord cOutCoord=gr()->cConv(rc);
  				Coord crdIn = cOutCoord;
  				if (fTransformCoords)
  					crdIn = csIn->cConv(csOut, cOutCoord);
  				if (fUseReal) {
  					if (fPatch)
  						rBuf[j] = rRealFromPatch(grIn->rcConv(crdIn));
  					else
  						rBuf[j] = mp->rValue(grIn->rcConv(crdIn));
  				}
  				else {
  					if (fPatch)
  						iBuf[j] = iRawFromPatch(grIn->rcConv(crdIn));
  					else
  						rBuf[j] = mp->iRaw(grIn->rcConv(crdIn));
  				}
  			}
  			if (fUseReal)
  				ptr.PutLineVal(i, rBuf);
  			else
  				ptr.PutLineRaw(i, iBuf);
  		}
		} break;
    case rmBILINEAR: 
    {
  	  for (long i=0; i < iNrLines; ++i) 
      {
  			RowCol rc(i, 0L);
  			if (trq.fUpdate(i, iNrLines))
  				return false;
  			for (long j=0; j < iNrCols; ++j, rc.Col++) {
  				Coord cOutCoord=gr()->cConv(rc);
          if (fColorDomain)
          {
    				iBuf[j] = iBiLinearColor(cOutCoord);
          }
          else {
    				rBuf[j] = rBiLinear(cOutCoord);
    				if (((j & 255)==255) && trq.fAborted())
    					return false;
          }
  			}
        if (fColorDomain)
  			  ptr.PutLineRaw(i, iBuf);
        else  
  			  ptr.PutLineVal(i, rBuf);
  		}
		}	break;
    case rmBICUBIC: 
    {
  		for (long i=0; i < iNrLines; ++i) {
  			RowCol rc(i, 0L);
  			if (trq.fUpdate(i, iNrLines))
  				return false;
  			for (long j=0; j < iNrCols; ++j, rc.Col++) {
  				Coord cOutCoord=gr()->cConv(rc);
  				rBuf[j] = rBiCubic(cOutCoord);
  				if (((j & 255)==255) && trq.fAborted())
  					return false;
  			}
  			ptr.PutLineVal(i, rBuf);
  		}
    } break;
	}
	mpInpMap.SetPointer(0);
	return true;
}

double MapResample::rBiCubic(const Coord& cOutCoord)
{
    double rRow, rCol;
    Coord crdIn = cOutCoord;
    if (fTransformCoords)
      crdIn = csIn->cConv(csOut, cOutCoord);
    mp->gr()->Coord2RowCol(crdIn, rRow, rCol);
    rRow -= 0.5;
    rCol -= 0.5;
    long iCol = (long)rCol;
    long iRow = (long)rRow;
    double rDeltaRow = rRow - iRow;
    double rDeltaCol = rCol - iCol;
    for(short i=0; i<4; ++i)
      rValRow[i]=rBiCubicResult(iRow-1+i, iCol, rDeltaCol);
    if(fResolveRealUndefs(rValRow))
      return rBiCubicPolynom(rValRow, rDeltaRow);

    return rUNDEF;
}


double MapResample::rBiCubicPolynom(double rVal[], const double& rDeltaPow)
{
    double rResult = rVal[1] +
                     rDeltaPow * (( rVal[2] - rVal[1]/2 - rVal[0]/3 -rVal[3]/6) +
                     rDeltaPow * (( (rVal[2] + rVal[0])/2 - rVal[1]) +
                     rDeltaPow * (( rVal[1] - rVal[2])/2 + (rVal[3] - rVal[0])/6)));
    return rResult;
}

double MapResample::rBiCubicResult(long iRow, long iCol, const double& rDCol)
{
  long i;
  if (fPatch)
    for( i=0; i<4; ++i)
      rValCol[i]= rRealFromPatch(RowCol(iRow, iCol-1L+i));
  else
    for( i=0; i<4; ++i)
      rValCol[i]= mp->rValue(RowCol(iRow, iCol-1L+i));
  if(fResolveRealUndefs(rValCol))
    return rBiCubicPolynom(rValCol, rDCol);

  return rUNDEF;
}

bool MapResample::fResolveRealUndefs(double rVal[])
{
    if ( rVal[1]==rUNDEF)
        if (rVal[2]==rUNDEF) 
          return false;
        else 
          rVal[1]=rVal[2];

    if ( rVal[2] == rUNDEF ) 
      rVal[2]=rVal[1];
    if ( rVal[0] == rUNDEF ) 
      rVal[0]=rVal[1];
    if ( rVal[3] == rUNDEF ) 
      rVal[3]=rVal[2];

    return true;
}

double MapResample::rBiLinear(const Coord& cOutCoord)
{
    double rRow, rCol;
    double rTotWeight=0.0, rTotValue=0.0;

    Coord crdIn = cOutCoord;
    if (fTransformCoords)
      crdIn = csIn->cConv(csOut, cOutCoord);
    mp->gr()->Coord2RowCol(crdIn, rRow, rCol);
    rRow -= 0.5;
    rCol -= 0.5;
    iRow[1] = iRow[0] = (long)rRow;
    iRow[3] = iRow[2] = iRow[1] + 1;
    iCol[2] = iCol[0] = (long)rCol;
    iCol[3] = iCol[1] = iCol[0] + 1;
    double rDeltaRow = rRow - iRow[0];
    double rDeltaCol = rCol - iCol[0];
    rWeight[0] = (1 - rDeltaRow) * ( 1 - rDeltaCol);
    rWeight[1] = (1 - rDeltaRow) * rDeltaCol;
    rWeight[2] = (1 - rDeltaCol) * rDeltaRow;
    rWeight[3] = rDeltaRow * rDeltaCol;
    for(short i=0; i<4; ++i)
    {
        double rVal;
        if (fPatch)
          rVal =rRealFromPatch(RowCol(iRow[i], iCol[i]));
        else
          rVal = mp->rValue(RowCol(iRow[i], iCol[i]));
        if(rVal != rUNDEF) {
            rTotValue +=  rVal * rWeight[i];
            rTotWeight += rWeight[i];
        }
    }
    if ( rTotWeight < 0.1 )
      return 0.0;
    return rTotValue / rTotWeight;
}

long MapResample::iBiLinearColor(const Coord& cOutCoord)
{
  double rRow, rCol;
  double rRed = 0.0, rGreen = 0.0, rBlue = 0.0;  
  double rTotWeight=0.0;

  Coord crdIn = cOutCoord;
  if (fTransformCoords)
    crdIn = csIn->cConv(csOut, cOutCoord);
  mp->gr()->Coord2RowCol(crdIn, rRow, rCol);
  rRow -= 0.5;
  rCol -= 0.5;
  iRow[1] = iRow[0] = (long)rRow;
  iRow[3] = iRow[2] = iRow[1] + 1;
  iCol[2] = iCol[0] = (long)rCol;
  iCol[3] = iCol[1] = iCol[0] + 1;
  double rDeltaRow = rRow - iRow[0];
  double rDeltaCol = rCol - iCol[0];
  rWeight[0] = (1 - rDeltaRow) * ( 1 - rDeltaCol);
  rWeight[1] = (1 - rDeltaRow) * rDeltaCol;
  rWeight[2] = (1 - rDeltaCol) * rDeltaRow;
  rWeight[3] = rDeltaRow * rDeltaCol;
  for(short i=0; i<4; ++i)
  {

    long iClr;
    if (fPatch)
      iClr = iRawFromPatch(RowCol(iRow[i], iCol[i]));
    else
      iClr = mp->iRaw(RowCol(iRow[i], iCol[i]));
    if(iClr != rUNDEF) 
    {
      Color clr(iClr);
      rRed +=  clr.red() * rWeight[i];
      rGreen += clr.green() * rWeight[i];
      rBlue += clr.blue() *  rWeight[i];
      rTotWeight += rWeight[i];
    }
  }
  if ( rTotWeight < 0.1 )
    return 0;
  // normalization
  rRed /= rTotWeight;
  rGreen /= rTotWeight;
  rBlue /= rTotWeight;
  Color clr;
  clr.red() = byteConv(rRed);
  clr.green() = byteConv(rGreen);
  clr.blue() = byteConv(rBlue);
  return (long)clr;
}

String MapResample::sResampleMethod(ResampleMethod rm)
{
  switch (rm) {
    case rmNEARNEIGHB:   return "NearestNeighbour";
    case rmBILINEAR:  return "BiLinear";
    case rmBICUBIC:  return "BiCubic";
    default : return sUNDEF;
  }
}

ResampleMethod MapResample::rmType(const String& sRm)
{
  if (sRm == "NearestNeighbour")
    return rmNEARNEIGHB;
  if (sRm == "BiLinear")
    return rmBILINEAR;
  if (sRm == "BiCubic")
    return rmBICUBIC;
  return rmNEARNEIGHB;
}

String MapResample::sExpression() const
{
  String sPatch;
  if (!fPatch)
    sPatch = ",NoPatch";
  return String("MapResample(%S,%S,%S%S)",
                 mp->sNameQuoted(true, fnObj.sPath()), gr()->sNameQuoted(true, fnObj.sPath()),
                 sResampleMethod(rm), sPatch);
}

bool MapResample::fValueRangeChangeable() const
{
  return rm != rmNEARNEIGHB;
}

double MapResample::rRealFromPatch(RowCol& rc)
{
    RealPatch *currentPatch;

    // ignore all illegal coordinates and return UNDEF
    if ( (rc.Row < 0 || rc.Row >= iInpLines) || (rc.Col < 0 || rc.Col >=iInpCols)  )
        return rUNDEF;

    // calculate a number of values needed for the relation between the rowcol and in
    // which patch a value is needed. The static allocations are to ensure that these
    // calculation are done only once during the lifetime of the object.
//    static long iNrPatchCols = (iInpCols-1) / PATCH_SIDE + 1;
//    static long iNrPatchRows = (iInpLines-1) / PATCH_SIDE + 1;
//    static long iNrPatches = iNrPatchCols * iNrPatchRows;
// 15/10/98 Wim: static is harmful!
//  moved to include file as members of the class
//  initialization now in beginning of fFreezing()
    long iPatchCol =  rc.Col / PATCH_SIDE ;
    long iPatchRow =  rc.Row / PATCH_SIDE ;
    long iPatchNr = iPatchRow * iNrPatchCols + iPatchCol;
    short iY = (short)(rc.Row-iPatchRow * PATCH_SIDE);
    short iX = (short)(rc.Col-iPatchCol * PATCH_SIDE);

    // is the patch in which de rc is located present in the bufferspace?
	short bufNr=0;
    for(; bufNr<MAX_SIZE_PATCH_BUFFER/*iNrPatches*/ &&
                   iPatchPresent[bufNr] != -1 &&
                   iPatchPresent[bufNr] != iPatchNr; ++bufNr);

    // if it is the first one ignore all and use this patch to retrieve the
    // raw value. if it is not the first one place the patch at the first entry
    // of the array and shift the values 0..bufnr one towards the end.
    if ( bufNr!=0 || iPatchPresent[0]==-1 )
    {
        if(bufNr < MAX_SIZE_PATCH_BUFFER && iPatchPresent[bufNr] != -1)
            currentPatch =rPatchBuffer[bufNr];
        else
        {
            // patch not found. Allocate space for new patch and remove the last
            // one.
            if(bufNr >= MAX_SIZE_PATCH_BUFFER) 
              bufNr=MAX_SIZE_PATCH_BUFFER-1;
            currentPatch  = new RealPatch;
            mpInpMap->GetPatchVal(rc, *currentPatch);
            if (0 != rPatchBuffer[bufNr])
              delete rPatchBuffer[bufNr];
            rPatchBuffer[bufNr]=NULL;
            iPatchPresent[bufNr]=-1;
        }

        // shift the patches one place
        for(short j=bufNr; j>0 ; --j)
        {
            if(j < MAX_SIZE_PATCH_BUFFER)
            {
                iPatchPresent[j]=iPatchPresent[j-1];
                rPatchBuffer[j]=rPatchBuffer[j-1];
            }
        }
        // insert the needed patch
        iPatchPresent[0]=iPatchNr;
        rPatchBuffer[0]=currentPatch;
    }
    // retrieve value
    // ( grumble, this could be done in one statement but the compiler gives trouble)
    RealPatch &patch = *rPatchBuffer[0];
    double val= patch( iY, iX  );
    return val;
}

long MapResample::iRawFromPatch(RowCol& rc)
{
    LongPatch *currentPatch;

    // ignore all illegal coordinates and return UNDEF
    if ( (rc.Row < 0 || rc.Row >= iInpLines) || (rc.Col < 0 || rc.Col >=iInpCols)  )
    {
        if ( dm()->pdi() ) return 0;
        else return iUNDEF;
    }

    // calculate a number of values needed for the relation between the rowcol and in
    // which patch a value is needed. The static allocations are to ensure that these
    // calculation are done only once during the lifetime of the object.
//    static long iNrPatchCols = (iInpCols-1) / PATCH_SIDE + 1;
//    static long iNrPatchRows = (iInpLines-1) / PATCH_SIDE + 1;
//    static long iNrPatches = iNrPatchCols * iNrPatchRows;
// 15/10/98 Wim: static is harmful!
//  moved to include file as members of the class
//  initialization now in beginning of fFreezing()

    long iPatchCol =  rc.Col / PATCH_SIDE ;
    long iPatchRow =  rc.Row / PATCH_SIDE ;
    long iPatchNr = iPatchRow * iNrPatchCols + iPatchCol;
    short iY = (short)(rc.Row-iPatchRow * PATCH_SIDE);
    short iX = (short)(rc.Col-iPatchCol * PATCH_SIDE);

    // is the patch in which de rc is located present in the bufferspace?
	short bufNr=0;
    for(; bufNr<MAX_SIZE_PATCH_BUFFER/*iNrPatches*/ &&
                   iPatchPresent[bufNr] != -1 &&
                   iPatchPresent[bufNr] != iPatchNr; ++bufNr);

    // if it is the first one ignore all and use this patch to retrieve the
    // raw value. if it is not the first one place the patch at the first entry
    // of the array and shift the values 0..bufnr one towards the end.
    if ( bufNr!=0 || iPatchPresent[0]==-1 )
    {
        if(bufNr < MAX_SIZE_PATCH_BUFFER && iPatchPresent[bufNr] != -1)
            currentPatch =iPatchBuffer[bufNr];
        else
        {
            // patch not found. Allocate space for new patch and remove the last
            // one.
            if(bufNr >= MAX_SIZE_PATCH_BUFFER) 
              bufNr=MAX_SIZE_PATCH_BUFFER-1;
            currentPatch  = new LongPatch;
            mpInpMap->GetPatchRaw(rc, *currentPatch);
            if (0 != iPatchBuffer[bufNr])
              delete iPatchBuffer[bufNr];
            iPatchBuffer[bufNr]=NULL;
            iPatchPresent[bufNr]=-1;
        }

        // shift the patches one place
        for(short j=bufNr; j>0 ; --j)
        {
            if(j < MAX_SIZE_PATCH_BUFFER)
            {
                iPatchPresent[j]=iPatchPresent[j-1];
                iPatchBuffer[j]=iPatchBuffer[j-1];
            }
        }
        // insert the needed patch
        iPatchPresent[0]=iPatchNr;
        iPatchBuffer[0]=currentPatch;
    }
    // retrieve value
    // ( grumble, this could be done in one statement but the compiler gives trouble)
    LongPatch &patch = *iPatchBuffer[0];
    long val= patch( iY, iX  );
    return val;
}





