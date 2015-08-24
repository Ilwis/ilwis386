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

#include "Applications\Raster\MapParallaxCorrection.H"
#include "Headers\Err\Ilwisapp.err"

IlwisObjectPtr * createMapParallaxCorrection(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)MapParallaxCorrection::create(fn, (MapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new MapParallaxCorrection(fn, (MapPtr &)ptr);
}

const char* MapParallaxCorrection::sSyntax() {
  return "MapParallaxCorrection(map,dem)";
}

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

MapParallaxCorrection* MapParallaxCorrection::create(const FileName& fn, MapPtr& p, const String& sExpr)
{
	Array<String> as;
	int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
	if ((iParms < 3) || (iParms > 3))
		ExpressionError(sExpr, sSyntax());

	// See also MapResample (conditions taken over from there)
  	// Disable use of attribute maps. Resample expects to use a patched map, which an
	// inline attribute map most likely is not
	String sInputMapName = as[0];
	char *pCh = sInputMapName.strrchrQuoted('.');
	if ((pCh != 0) && (0 != _strcmpi(pCh, ".mpr")))  // attrib map
		throw ErrorObject(WhatError(String(TR("Use of attribute maps is not possible: '%S'").c_str(), as[0]), errMapResample), fn);
	Map mp(as[0], fn.sPath());
	Map dem(as[1], fn.sPath());
	int iRsmMeth = iFind(as[2], sResampleMethods);
	if (shUNDEF == iRsmMeth)
		throw ErrorResample(as[2], fn);
	ResampleMethod rm = ResampleMethod(iRsmMeth);

	return new MapParallaxCorrection(fn, p, mp, dem, rm);
}

MapParallaxCorrection::MapParallaxCorrection(const FileName& fn, MapPtr& p)
: MapResample(fn, p)
{
  ReadElement("MapParallaxCorrection", "Dem", dem);
  fNeedFreeze = true;
  sFreezeTitle = "MapParallaxCorrection";
  objdep.Remove(gr());
  objdep.Add(dem);
}

MapParallaxCorrection::MapParallaxCorrection(const FileName& fn, MapPtr& p, const Map& mp, const Map & _dem, ResampleMethod rsm)
: MapResample(fn, p, mp, mp->gr(), rsm, true, false)
, dem(_dem)
{
  sFreezeTitle = "MapParallaxCorrection";
  objdep.Remove(gr());
  objdep.Add(dem);
}

void MapParallaxCorrection::Store()
{
  MapResample::Store();
  WriteElement("MapFromMap", "Type", "MapParallaxCorrection");
  WriteElement("MapParallaxCorrection", "Dem", dem);
}

MapParallaxCorrection::~MapParallaxCorrection()
{
}

void MapParallaxCorrection::ComputeLocation(const double hcloud, const double fcloud, const double lcloud, double & fcloudcorr, double & lcloudcorr) {
	const double hsat = 42165.39;
	const double fsat = 0 * M_PI / 180.0;
	const double lsat = 0 * M_PI / 180.0;;
	const double requ = 6378.077;
	const double rpole = 6356.577;
	const double rmean = (requ + rpole) / 2.0;
	const double rratio = requ / rpole;
	double fcloudrad = fcloud * M_PI / 180.0;
	double lcloudrad = lcloud * M_PI / 180.0;
	double fsatgeod = atan(tan(fsat) * rratio * rratio);
	double xsat = hsat * cos(fsatgeod) * sin(lsat);
	double ysat = hsat * sin(fsatgeod);
	double zsat = hsat * cos(fsatgeod) * cos(lsat);
	double fcloudgeod = atan(tan(fcloudrad) * rratio * rratio);
	double rlocal = requ / sqrt(cos(fcloudgeod) * cos(fcloudgeod) + rratio * rratio * sin(fcloudgeod) * sin(fcloudgeod));
	double xcloud = rlocal * cos(fcloudgeod) * sin(lcloudrad);
	double ycloud = rlocal * sin(fcloudgeod);
	double zcloud = rlocal * cos(fcloudgeod) * cos(lcloudrad);
	double rratiolocal = ((requ + hcloud) / (rpole + hcloud)) * ((requ + hcloud) / (rpole + hcloud));
	double xdiff = xsat - xcloud;
	double ydiff = ysat - ycloud;
	double zdiff = zsat - zcloud;
	double e1 = xdiff * xdiff + rratiolocal * ydiff * ydiff + zdiff * zdiff;
	double e2 = 2.0 * (xcloud * xdiff + rratiolocal * ycloud * ydiff + zcloud * zdiff);
	double e3 = xcloud * xcloud + rratiolocal * ycloud * ycloud + zcloud * zcloud - (requ + hcloud) * (requ + hcloud);
	double c = (sqrt(e2 * e2 - 4.0 * e1 * e3) - e2) / (2.0 * e1);
	double xcorr = xcloud + c * xdiff;
	double ycorr = ycloud + c * ydiff;
	double zcorr = zcloud + c * zdiff;
	fcloudcorr = atan(ycorr/sqrt(xcorr * xcorr + zcorr * zcorr));
	fcloudcorr = atan(tan(fcloudcorr)/(rratio * rratio)) * 180.0 / M_PI;
	lcloudcorr = atan2(xcorr,zcorr) * 180.0 / M_PI;
}

bool MapParallaxCorrection::fFreezing()
{
	clock_t start = clock();
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
	CoordSystem csLatLon ("LatlonWGS84.csy");
	csIn = mp->gr()->cs();
	csOut = cs();
	fTransformCoords = csIn != csOut;
	
	long iNrLines=iLines(), iNrCols=iCols();
	iInpCols = mp->iCols();
	iInpLines = mp->iLines();
	iNrPatchCols = (iInpCols-1) / PATCH_SIDE + 1;
	iNrPatchRows = (iInpLines-1) / PATCH_SIDE + 1;
	iNrPatches = iNrPatchCols * iNrPatchRows;

	trq.SetText(String(TR("Allocating displacement matrix").c_str()));
	Coord * rcToCoord = new Coord[iNrLines * iNrCols];
	for (long i=0; i < iNrLines; ++i) {
		if (trq.fUpdate(i, iNrLines)) {
			delete [] rcToCoord;
			return false;
		}
		for (long j=0; j < iNrCols; ++j) {
			rcToCoord[i * iNrCols + j] = crdUNDEF;
		}
	}
	trq.SetText(String(TR("Computing displacement using '%S'").c_str(), dem->sName(true, dem->fnObj.sPath())));
	clock_t start2 = clock();
	for (long i=0; i < iNrLines; ++i) {
		RowCol rc(i, 0L);
		if (trq.fUpdate(i, iNrLines)) {
			delete [] rcToCoord;
			return false;
		}
		for (long j=0; j < iNrCols; ++j, rc.Col++) {
			Coord cOrigCoord = gr()->cConv(rc); // this does rc.Row + 0.5, rc.Col + 0.5
			double h = dem->rValue(dem->gr()->rcConv(cOrigCoord));
			if (h != rUNDEF) {
				Coord crdLatLon = csLatLon->cConv(csOut, cOrigCoord);
				ComputeLocation(h / 1000.0, crdLatLon.y, crdLatLon.x, crdLatLon.y, crdLatLon.x);
				Coord cNewCoord = csOut->cConv(csLatLon, crdLatLon);
				double rRowDestination;
				double rColDestination;
				gr()->Coord2RowCol(cNewCoord, rRowDestination, rColDestination);
				RowCol rcDestination(rRowDestination, rColDestination);
				if (rcToCoord[rcDestination.Row * iNrCols + rcDestination.Col] != crdUNDEF)
					continue;
				double diffY = rRowDestination - (double)rcDestination.Row - 0.5; // between 0 and 1 (pixel)
				double diffX = rColDestination - (double)rcDestination.Col - 0.5;
				double rPixSizeY = cOrigCoord.y - gr()->cConv(RowCol(rc.Row + 1, rc.Col)).y;
				double rPixSizeX = cOrigCoord.x - gr()->cConv(RowCol(rc.Row, rc.Col - 1)).x;
				rcToCoord[rcDestination.Row * iNrCols + rcDestination.Col] = Coord(cOrigCoord.x - rPixSizeX * diffX, cOrigCoord.y + rPixSizeY * diffY);
			}
		}
	}
	clock_t end2 = clock();
	double total2 = (double)(end2 - start2) / CLOCKS_PER_SEC;
	String s2("calc matrix in %2.2f seconds;\n", total2);
	TRACE(s2.c_str());

	trq.SetText(String(TR("Determining fixed locations").c_str()));
	for (long i=0; i < iNrLines; ++i) {
		RowCol rc(i, 0L);
		if (trq.fUpdate(i, iNrLines)) {
			delete [] rcToCoord;
			return false;
		}
		for (long j=0; j < iNrCols; ++j, rc.Col++) {
			if (rcToCoord[i * iNrCols + j] != crdUNDEF)
				continue;
			Coord cOrigCoord = gr()->cConv(rc);
			rcToCoord[i * iNrCols + j] = cOrigCoord;
		}
	}

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
			if (trq.fUpdate(i, iNrLines)) {
				delete [] rcToCoord;
  				return false;
			}
  			for (long j=0; j < iNrCols; ++j, rc.Col++) {
				Coord cOutCoord = rcToCoord[rc.Row * iNrCols + rc.Col];
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
  						iBuf[j] = mp->iRaw(grIn->rcConv(crdIn));
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
			if (trq.fUpdate(i, iNrLines)) {
				delete [] rcToCoord;
  				return false;
			}
  			for (long j=0; j < iNrCols; ++j, rc.Col++) {
				Coord cOutCoord = rcToCoord[rc.Row * iNrCols + rc.Col];
          if (fColorDomain)
          {
    				iBuf[j] = iBiLinearColor(cOutCoord);
          }
          else {
    				rBuf[j] = rBiLinear(cOutCoord);
					if (((j & 255)==255) && trq.fAborted()) {
						delete [] rcToCoord;
    					return false;
					}
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
			if (trq.fUpdate(i, iNrLines)) {
				delete [] rcToCoord;
  				return false;
			}
  			for (long j=0; j < iNrCols; ++j, rc.Col++) {
				Coord cOutCoord = rcToCoord[rc.Row * iNrCols + rc.Col];
  				rBuf[j] = rBiCubic(cOutCoord);
				if (((j & 255)==255) && trq.fAborted()) {
					delete [] rcToCoord;
  					return false;
				}
  			}
  			ptr.PutLineVal(i, rBuf);
  		}
    } break;
	}
	mpInpMap.SetPointer(0);
	delete [] rcToCoord;
	clock_t end = clock();
	double total = (double)(end - start) / CLOCKS_PER_SEC;
	String s("calc old in %2.2f seconds;\n", total);
	TRACE(s.c_str());
	return true;
}

String MapParallaxCorrection::sExpression() const
{
  String s("MapParallaxCorrection(%S,%S,%S)", mp->sNameQuoted(true, fnObj.sPath()), dem->sNameQuoted(true, fnObj.sPath()), sResampleMethod(rm));
  return s;
}



