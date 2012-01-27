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

#include "Applications\Raster\Mapresmp.h"
#include "Engine\Map\Raster\Grid.h"
#include "Applications\Raster\Mapresmp2.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Base\DataObjects\WPSMetaData.h"
#include "Engine\Base\DataObjects\Patch.h"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Hs\map.hs"

IlwisObjectPtr * createMapResample2(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)MapResample2::create(fn, (MapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new MapResample2(fn, (MapPtr &)ptr);
}

String wpsmetadataMapResample2() {
	WPSMetaData metadata("MapResample2");
	return metadata.toString();
}

ApplicationMetadata metadataMapResample2(ApplicationQueryData *query) {
	ApplicationMetadata md;
	if ( query->queryType == "WPSMETADATA" || query->queryType == "") {
		md.wpsxml = wpsmetadataMapResample2();
	}
	if ( query->queryType == "OUTPUTTYPE" || query->queryType == "")
		md.returnType = IlwisObject::iotRASMAP;
	if ( query->queryType == "EXPERSSION" || query->queryType == "")
		md.skeletonExpression =  MapResample2::sSyntax();

	return md;
}


const char * MapResample2::sSyntax()
 { return "MapResample2(map,georef,ResampleMethod)\n"
"MapResample2(map,georef,ResampleMethod,Patch|NoPatch)"
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


MapResample2* MapResample2::create(const FileName& fn, MapPtr& p, const String& sExpr)
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
	return new MapResample2(fn, p, mp, gr, rm, fPatch, fGrNoneAllowed);
}

MapResample2::MapResample2(const FileName& fn, MapPtr& p)
: MapFromMap(fn, p), rPatchBuffer(0), iPatchBuffer(0), fPatch(true)
{
  String s;
  fNeedFreeze = true;
  ReadElement("MapResample2", "ResampleMethod", s);
  if (0 == ReadElement("MapResample2", "Patch", fPatch))
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

MapResample2::MapResample2(const FileName& fn, MapPtr& p, const Map& mp, const GeoRef& gr,
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

void MapResample2::Init()
{
  sFreezeTitle = "MapResample2";
  htpFreeze = "ilwisapp\\resample_algorithm.htm";
}

void MapResample2::Store()
{
  MapFromMap::Store();
  WriteElement("MapFromMap", "Type", "MapResample2");
  WriteElement("MapResample2", "ResampleMethod", sResampleMethod(rm));
  WriteElement("MapResample2", "Patch", fPatch);
}

MapResample2::~MapResample2()
{
}
template<class T> bool MapResample2::calcMapNearestNeighbour(const ILWIS::Grid<T>& gridIn, double undef) {
	RowCol rcOut = rcSize();
	RowCol rcIn = mp->rcSize();

	csIn = mp->gr()->cs();
	csOut = cs();
	GeoRef grIn = mp->gr();
	fTransformCoords = csIn != csOut;
	Buf<long> line(rcOut.Col);	
	Buf<double> liner(rcOut.Col);

	for(int y=0; y < rcOut.Row; ++y) {
  		if (trq.fUpdate(y, rcOut.Row))
  				return false;
		for(int x = 0; x < rcOut.Col; ++x) {
			Coord cOutCoord=gr()->cConv(RowCol(y,x));
			Coord crdIn = cOutCoord;
			if (fTransformCoords) {
				crdIn = csIn->cConv(csOut, cOutCoord);
			}
			RowCol rc = grIn->rcConv(crdIn);
			if ( rc.Row >=0 && rc.Row < rcIn.Row && rc.Col >= 0 && rc.Col < rcIn.Col) {
					T v = gridIn.at(rc.Row, rc.Col);
					if ( undef == rUNDEF)
						liner[x] = v;
					else
						line[x] = v;
				}
				else {
					if ( undef == rUNDEF)
						liner[x] = (T)undef;
					else
						line[x] = (T)undef;
				}

		}
		if ( undef != rUNDEF)
			ptr.PutLineRaw(y,line);
		else
			ptr.PutLineVal(y,liner);

	}
	return true;
}

template<class T> bool MapResample2::calcMapBilinear(const ILWIS::Grid<T>& gridIn, double undef) {
	RowCol rcOut = rcSize();
	RowCol rcIn = mp->rcSize();

	csIn = mp->gr()->cs();
	csOut = cs();
	GeoRef grIn = mp->gr();
	fTransformCoords = csIn != csOut;
	Buf<double> line(rcOut.Col);	

	for(int y=0; y < rcOut.Row; ++y) {
  		if (trq.fUpdate(y, rcOut.Row))
  				return false;
		for(int x = 0; x < rcOut.Col; ++x) {
			Coord cOutCoord=gr()->cConv(RowCol(y,x));
			line[x] = biLinear<T>(cOutCoord, gridIn, undef,rcIn);
		}
		ptr.PutLineVal(y,line);

	}
	return true;
}

template<class T> bool MapResample2::calcMapBicubic(const ILWIS::Grid<T>& gridIn, double undef) {
	RowCol rcOut = rcSize();
	RowCol rcIn = mp->rcSize();

	csIn = mp->gr()->cs();
	csOut = cs();
	GeoRef grIn = mp->gr();
	fTransformCoords = csIn != csOut;
	Buf<double> line(rcOut.Col);	

	for(int y=0; y < rcOut.Row; ++y) {
  		if (trq.fUpdate(y, rcOut.Row))
  				return false;
		for(int x = 0; x < rcOut.Col; ++x) {
			Coord cOutCoord=gr()->cConv(RowCol(y,x));
			line[x] = biCubic<T>(cOutCoord, gridIn, undef);
		}
		ptr.PutLineVal(y,line);

	}
	return true;
}

template<class T> bool MapResample2::calcMap(const ILWIS::Grid<T>& gridIn, double undef) {
	if ( rm == rmNEARNEIGHB) {
		return calcMapNearestNeighbour<T>(gridIn, undef);
	} else if ( rm == rmBILINEAR) {
		return calcMapBilinear<T>(gridIn, undef);
	} else if ( rm == rmBICUBIC) {
		return calcMapBicubic<T>(gridIn, undef);
	}
	return true;
}

bool MapResample2::fFreezing() {

	clock_t start = clock();
	StoreType st = mp->st();

	if ( st == stBYTE) {
		ILWIS::Grid<byte> gridIn(mp);
		calcMap<byte>(gridIn, 0);
	}
	if ( st == stINT) {
		ILWIS::Grid<short> gridIn(mp);
		calcMap<short>(gridIn, shUNDEF);
	}
	if ( st == stLONG) {
		ILWIS::Grid<long> gridIn(mp);
		calcMap<long>(gridIn, iUNDEF);
	}
	if ( st == stREAL) {
		ILWIS::Grid<double> gridIn(mp);
		calcMap<double>(gridIn, rUNDEF);
	}


	clock_t end = clock();
	double total = (double)(end - start) / CLOCKS_PER_SEC;
	String s("calc new in %2.2f seconds;\n", total);
	TRACE(s.c_str());
	return true;
}

template<class T> double MapResample2::biCubic(const Coord& cOutCoord, const ILWIS::Grid<T>& grid, T undef)
{
 /*   double rRow, rCol;
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
	for(short i=0; i<4; ++i) {
		double v;
		if ( iRow[i] >=0 && iRow[i] < rcIn.Row && iCol[i] >= 0 && iCol[i] < rcIn.Col) {
			rValRow[i]=rBiCubicResult(iRow-1+i, iCol, rDeltaCol, grid,undef);
		}

	}
    if(fResolveRealUndefs(rValRow), undef)
      return rBiCubicPolynom(rValRow, rDeltaRow);*/

    return rUNDEF;
}


double MapResample2::rBiCubicPolynom(double rVal[], const double& rDeltaPow)
{
    double rResult = rVal[1] +
                     rDeltaPow * (( rVal[2] - rVal[1]/2 - rVal[0]/3 -rVal[3]/6) +
                     rDeltaPow * (( (rVal[2] + rVal[0])/2 - rVal[1]) +
                     rDeltaPow * (( rVal[1] - rVal[2])/2 + (rVal[3] - rVal[0])/6)));
    return rResult;
}

template<class T> double MapResample2::rBiCubicResult(long iRow, long iCol, const double& rDCol, const ILWIS::Grid<T>& grid, T undef)
{
  for( int i=0; i<4; ++i)
      rValCol[i]= grid.at(iRow, iCol-1L+i);
  if(fResolveRealUndefs(rValCol))
    return rBiCubicPolynom(rValCol, rDCol);

  return rUNDEF;
}

bool MapResample2::fResolveRealUndefs(double rVal[])
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

template<class T> double MapResample2::biLinear(const Coord& cOutCoord, const ILWIS::Grid<T>& grid, T undef, const RowCol& rcIn)
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
        double val;
		if ( iRow[i] >=0 && iRow[i] < rcIn.Row && iCol[i] >= 0 && iCol[i] < rcIn.Col) {
			val =grid.at(iRow[i], iCol[i]);
			if ( val != undef && mp->fRawAvailable()) {
				val = mp->vr()->rValue(val);
			}
			if(val != undef) {
				rTotValue +=  val * rWeight[i];
				rTotWeight += rWeight[i];
			}
		}
    }
    if ( rTotWeight < 0.1 )
      return 0.0;
    return rTotValue / rTotWeight;
}

long MapResample2::iBiLinearColor(const Coord& cOutCoord)
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

String MapResample2::sResampleMethod(ResampleMethod rm)
{
  switch (rm) {
    case rmNEARNEIGHB:   return "NearestNeighbour";
    case rmBILINEAR:  return "BiLinear";
    case rmBICUBIC:  return "BiCubic";
    default : return sUNDEF;
  }
}

ResampleMethod MapResample2::rmType(const String& sRm)
{
  if (sRm == "NearestNeighbour")
    return rmNEARNEIGHB;
  if (sRm == "BiLinear")
    return rmBILINEAR;
  if (sRm == "BiCubic")
    return rmBICUBIC;
  return rmNEARNEIGHB;
}

String MapResample2::sExpression() const
{
  String sPatch;
  if (!fPatch)
    sPatch = ",NoPatch";
  return String("MapResample2(%S,%S,%S%S)",
                 mp->sNameQuoted(true, fnObj.sPath()), gr()->sNameQuoted(true, fnObj.sPath()),
                 sResampleMethod(rm), sPatch);
}

bool MapResample2::fValueRangeChangeable() const
{
  return rm != rmNEARNEIGHB;
}

double MapResample2::rRealFromPatch(RowCol& rc)
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

long MapResample2::iRawFromPatch(RowCol& rc)
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





