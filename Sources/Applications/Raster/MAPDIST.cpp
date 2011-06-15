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


#include "Applications\Raster\MAPDIST.H"
#include "Applications\Raster\MAPTHIES.H"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Base\DataObjects\WPSMetaData.h"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Hs\map.hs"

IlwisObjectPtr * createMapDistance(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)MapDistance::create(fn, (MapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new MapDistance(fn, (MapPtr &)ptr);
}

String wpsmetadataMapDistance() {
	WPSMetaData metadata("MapDistance");
	metadata.AddTitle("MapDistance");
	metadata.AddAbstract("each pixel is assigned the distance in meters towards user-specified source pixels, for example distance to schools, distance to roads etc.");
	metadata.AddKeyword("spatial");
	metadata.AddKeyword("raster");
	metadata.AddKeyword("distance");
	WPSParameter *parm1 = new WPSParameter("1","Input Map",WPSParameter::pmtRASMAP);
	parm1->AddAbstract("is the name of the source map");

	WPSParameterGroup *grp = new WPSParameterGroup("Weight",0,"Weight");

	WPSParameter *parm2 = new WPSParameter("0","Weight map", WPSParameter::pmtRASMAP);
	parm2->AddAbstract("is the name of the weight map; the weight map should be a map with a value domain.");

	WPSParameter *parm3= new WPSParameter("2","Thiessen Map", WPSParameter::pmtENUM);
	parm3->AddAbstract("Optional calculation of a Thiessen map. When calculating a Thiessen map, also a distance map is calculated");
	parm3->setOptional(true);

	grp->addParameter(parm2);
	grp->addParameter(parm3);
	grp->setOptional(true);

	metadata.AddParameter(parm1);
	metadata.AddParameter(grp);

	WPSParameter *parmout = new WPSParameter("Result","Output Map", WPSParameter::pmtRASMAP, false);
	parmout->AddAbstract("reference Outputmap and supporting data objects");
	metadata.AddParameter(parmout);
	

	return metadata.toString();
}

ApplicationMetadata metadataMapDistance(ApplicationQueryData *query) {
	ApplicationMetadata md;
	if ( query->queryType == "WPSMETADATA" || query->queryType == "") {
		md.wpsxml = wpsmetadataMapDistance();
	}
	if ( query->queryType == "OUTPUTTYPE" || query->queryType == "")
		md.returnType = IlwisObject::iotRASMAP;
	if ( query->queryType == "EXPERSSION" || query->queryType == "")
		md.skeletonExpression =  MapDistance::sSyntax();

	return md;
}

#define HIVAL (LONG_MAX >> 1)

const char * MapDistance::sSyntax()
 { return "MapDistance(sourcemap,weightmap)\n"
 "MapDistance(sourcemap)\n"
 "MapDistance(sourcemap,weightmap,thiessenmap)"; }

Domain MapDistance::dmDefault()
{
  return Domain("distance");
}

ValueRange MapDistance::vrDefault(const Map& mp)
{
  CoordBounds cb = mp->gr()->cb();
  double rXDif = cb.MaxX() - cb.MinX();
  double rYDif = cb.MaxY() - cb.MinY();
  double r = sqrt(rXDif*rXDif+rYDif*rYDif);
  r *= 5; // for resistance, could be an other value!
  // takes smallest 10 power larger than r as upperbound
  return ValueRange(0, pow(10, ceil(log10(r))),1);
}

static void SameNameDistThiessenError(const FileName& fn)
{
  throw ErrorObject(WhatError(SMAPErrDuplicateName, errMapDistance+4), fn);
}

MapDistance* MapDistance::create(const FileName& fn, MapPtr& p, const String& sExpr)
{
  Array<String> as;
  short iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  if ((iParms < 1) || (iParms > 3))
    ExpressionError(sExpr, sSyntax());
  Map mpSrc(as[0], fn.sPath());
  if (iParms == 1)
    return new MapDistance(fn, p, mpSrc, Map(), String());
  Map mpWgth;
  if ((as[1] != "1") && (as[1].length() != 0))
    mpWgth = Map(as[1], fn.sPath());
  String sThiessen;
  if (iParms == 3) {
    sThiessen = as[2];
    FileName fnThiessen(sThiessen, fn);
    fnThiessen.sExt = ".mpr";
    if (fn == fnThiessen)
      SameNameDistThiessenError(fn);
    if (fn.fExist())
      FileAlreadyExistError(fnThiessen);
  }
//  Domain dm;
//  if (iParms == 3)
//    dm = Domain(as[2]);
  return new MapDistance(fn, p, mpSrc, mpWgth, sThiessen);
}

MapDistance::MapDistance(const FileName& fn, MapPtr & p)
: MapFromMap(fn, p)
{
  ReadElement("MapDistance", "WeightMap", mpWeight);
  if (mpWeight.fValid())
    objdep.Add(mpWeight.ptr());
  ReadElement("MapDistance", "ThiessenMap", sMapThiessen);
  fNeedFreeze = true;
  Init();
}

MapDistance::MapDistance(const FileName& fn, MapPtr& p, const Map& mapSrc, const Map& mapWght, const String& sThiessen)
: MapFromMap(fn, p, mapSrc), mpWeight(mapWght), sMapThiessen(sThiessen)
{
//if (dmDflt.fValid())
//  SetDomainValueRangeStruct(dmDflt);
//else
    SetDomainValueRangeStruct(Domain("distance"));
  if (!dvrs().fValues())
    ValueDomainError(dm()->sName(true, fnObj.sPath()), sTypeName(), errMapDistance);
  if (mpWeight.fValid()) {
    if (!mpWeight->dvrs().fValues())
      ValueDomainError(mpWeight->dm()->sName(true, fnObj.sPath()), mpWeight->sTypeName(), errMapDistance+1);
    if (mpWeight->gr() != mapSrc->gr())
      throw ErrorIncompatibleGeorefs(mpWeight->gr()->sName(true, fnObj.sPath()), mapSrc->gr()->sName(true, fnObj.sPath()), fnObj, errMapDistance+2);
    if (mpWeight->rcSize() != mapSrc->rcSize())
      throw ErrorIncompatibleMapSize(mpWeight->sName(true, fnObj.sPath()), mapSrc->sName(true, fnObj.sPath()), fnObj, errMapDistance+3);
    objdep.Add(mpWeight.ptr());
  }  
  fNeedFreeze = true;
  Init();
  if (!fnObj.fValid())
    objtime = objdep.tmNewest();
}

void MapDistance::Init()
{
  sFreezeTitle = "MapDistance";
  htpFreeze = "ilwisapp\\distance_calculation_algorithm.htm";
}

void MapDistance::Store()
{
  MapFromMap::Store();
  WriteElement("MapFromMap", "Type", "MapDistance");
  if (mpWeight.fValid())
    WriteElement("MapDistance", "WeightMap", mpWeight);
  if (sMapThiessen.length() != 0)
    WriteElement("MapDistance", "ThiessenMap", sMapThiessen);
}

MapDistance::~MapDistance()
{
}

bool MapDistance::fDomainChangeable() const
{
  return true;
}

bool MapDistance::fValueRangeChangeable() const
{
  return true;
}

bool MapDistance::fFreezing()
{
  FileName fnTmp("ilwdst", ".tmp");
  fnTmp.Dir(fnObj.sPath());
  filTemp = new File(FileName::fnUnique(fnTmp), facCRT);
  filTemp->SetErase();
  filTemp->KeepOpen(true); 

  iCurrDist.Size(iCols());
  iPrevDist.Size(iCols());
  brCurrWeight.Size(iCols());
  iCurrSource.Size(iCols());
  iPrevSource.Size(iCols());

  long iChanges;
  bool fFirstPass = true;
  bool fStop;
  do {
    fStop = !fForwardDistances(iChanges, fFirstPass);
    if (fStop) break;
    if (fFirstPass) {
      iChanges++;    // always go back at least once !
      fFirstPass = false;
    }
    if (iChanges > 0) 
      fStop = !fBackwardDistances(iChanges);
    if (fStop) break;
  } while (iChanges > 0);
  if (!fStop)
    fStop = !fCorrectDistances();

  delete filTemp;
  iCurrDist.Size(0);
  iPrevDist.Size(0);
  brCurrWeight.Size(0);
  iCurrSource.Size(0);
  iPrevSource.Size(0);
  return !fStop;
}

String MapDistance::sExpression() const
{
  if (sMapThiessen.length() == 0) {
   if (!mpWeight.fValid())
     return String("MapDistance(%S)", 
                   mp->sNameQuoted(true, fnObj.sPath()));
   return String("MapDistance(%S,%S)", 
                 mp->sNameQuoted(true, fnObj.sPath()),
                 mpWeight->sNameQuoted(true, fnObj.sPath()));
  }               
  String sWeight = "1";
  if (mpWeight.fValid())
    sWeight = mpWeight->sNameQuoted(true, fnObj.sPath());
            
  return String("MapDistance(%S,%S,%S)", 
                mp->sNameQuoted(true, fnObj.sPath()), sWeight, 
                sMapThiessen);
}

bool MapDistance::fForwardDistances(long& iChanges, bool fFirstPass)
{
	long iCurLine, iCurCol, iMinSource, iLineChanges, iWeight;
	long iMinDist, iDist, iW1, iW2;
	long iStart;
	long iColumns = iCols();

// iCurLine = current line of the input file
// iCurCol =  current column of the input file and the buffers
// iMinSource =  input source map value of the nearest neighbour
// iLineChanges = number of changes of distances or values in a line
// iWeight = value of input weight map
// wMinDist = minimun distance from (i, j) to nearest neighbour
// wDist = temporary minimun distance from (i, j) to nearest neighbour
// iW1 = first value of filter of distance
// iW2 = second value of filter of distance
// iStart = position to read/write in temporary file

	bool fCalcThiessen = sMapThiessen.length() != 0;
	trq.SetText(SMAPTextForward);
	iChanges = 0;
	trq.fUpdate(iChanges); 
	iLineChanges = 0;
	for (iCurLine = 0; iCurLine < iLines(); iCurLine++)
	{
		if (iLineChanges != 0)
		{
			String s = String(SMAPTextForwardChanges_i.scVal(), iChanges);
			trq.SetText(s);
		}
		if (trq.fUpdate(iCurLine, iLines()))
			return false;

		iLineChanges = 0;
		
		if (mpWeight.fValid())
			mpWeight->GetLineVal(iCurLine, brCurrWeight);
		else
			for (iCurCol = 0; iCurCol < iColumns; iCurCol++)
				brCurrWeight[iCurCol] = 1;
		if (fFirstPass)
		{
			mp->GetLineRaw(iCurLine, iCurrSource);
			for (iCurCol = 0; iCurCol < iColumns; iCurCol++)
			{
				if (iCurrSource[iCurCol] != iUNDEF && iCurrSource[iCurCol] != 0)  // byte maps have 0 as undefined !
					iCurrDist[iCurCol] = 0;
				else if (brCurrWeight[iCurCol] == iUNDEF)
					iCurrDist[iCurCol] = iUNDEF;
				else
					iCurrDist[iCurCol] = HIVAL;
			}
		}
		else
		{
			iStart = iCurLine * 1L * iColumns * sizeof(long);
			if (fCalcThiessen)
				iStart = iCurLine * 2L * iColumns * sizeof(long);
			filTemp->Seek(iStart);
			filTemp->Read(iColumns*sizeof(long), iCurrDist.buf());
			if (fCalcThiessen)
				filTemp->Read(iColumns*sizeof(long), iCurrSource.buf());
		}

		for ( iCurCol = 0; iCurCol < iColumns; iCurCol++)
		{
			iWeight = (long)brCurrWeight[iCurCol];
			if (iWeight == iUNDEF)
				continue;
			if (iWeight >= 0)
			{
				if (iWeight == 0)
				{
					iW1 = 0;
					iW2 = 0;
				}
				else
				{
					if (iWeight != 1)
					{
						iW1 = 5 * iWeight;
						iW2 = 7 * iWeight;
					}
					else
					{
						iW1 = 5;
						iW2 = 7;
					}
				}
				iMinDist = iCurrDist[iCurCol];
				if (iMinDist == iUNDEF)
					continue;
				if (fCalcThiessen) 
					iMinSource = iCurrSource[iCurCol];

				if (iCurCol > 0)
				{
					if (iCurrDist[iCurCol - 1] != iUNDEF)
					{
						iDist = iCurrDist[iCurCol - 1] + iW1;
						if (iDist < iMinDist)
						{
							iMinDist = iDist;
							if (fCalcThiessen) 
								iMinSource = iCurrSource[iCurCol-1];
						}
					}
				}

				if (iCurCol < iColumns - 1)
				{
					if (iCurrDist[iCurCol + 1] != iUNDEF)
					{
						iDist = iCurrDist[iCurCol + 1] + iW1;
						if (iDist < iMinDist)
						{
							iMinDist = iDist;
							if (fCalcThiessen) 
								iMinSource = iCurrSource[iCurCol + 1];
						}
					}
				}

				if (iCurLine > 0)
				{
					if (iPrevDist[iCurCol] != iUNDEF)
					{
						iDist = iPrevDist[iCurCol] + iW1;
						if (iDist < iMinDist)
						{
							iMinDist = iDist;
							if (fCalcThiessen) 
								iMinSource = iPrevSource[iCurCol];
						}
					}
				}

				if ((iCurLine > 0) && (iCurCol > 0))
				{
					if (iPrevDist[iCurCol - 1] != iUNDEF)
					{
						iDist = iPrevDist[iCurCol - 1] + iW2;
						if (iDist < iMinDist)
						{
							iMinDist = iDist;
							if (fCalcThiessen) 
								iMinSource = iPrevSource[iCurCol - 1];
						}
					}
				}

				if ((iCurLine > 0) && (iCurCol < iColumns - 1))
				{
					if (iPrevDist[iCurCol + 1] != iUNDEF)
					{
						iDist = iPrevDist[iCurCol + 1] + iW2;
						if (iDist < iMinDist)
						{
							iMinDist = iDist;
							if (fCalcThiessen) 
								iMinSource = iPrevSource[iCurCol + 1];
						}
					}
				}

				if (iCurrDist[iCurCol] != iMinDist)
				{
					iLineChanges = iLineChanges++;
					iCurrDist[iCurCol] = iMinDist;
					if (fCalcThiessen)
						iCurrSource[iCurCol] = iMinSource;
				}
			}
		}

		if (fFirstPass || iLineChanges != 0)
		{
			iStart = iCurLine * 1L * iColumns * sizeof(long);
			if (fCalcThiessen)
				iStart = iCurLine * 2L * iColumns * sizeof(long);
			filTemp->Seek(iStart);
			filTemp->Write(iColumns*sizeof(long), iCurrDist.buf());
			if (fCalcThiessen)
				filTemp->Write(iColumns*sizeof(long), iCurrSource.buf());
		}
		Swap(iPrevDist, iCurrDist);
		Swap(iPrevSource, iCurrSource);
		iChanges = iChanges + iLineChanges;
	}
	trq.fUpdate(iLines(), iLines());
	return true;
}

bool MapDistance::fBackwardDistances(long& iChanges)
{
  long iCurLine, iCurCol, iMinSource, iLineChanges, iWeight;
//  int iCols = mpWeight->iCols();
//  int iLines = mpWeight->iLines();
//  bool fComplex = Mp.fComplex;
  long iMinDist, iDist, iW1, iW2;
  long iStart;
  long iColumns = iCols();

// iCurLine = current line of the input file
// iCurCol =  current column of the input file and the buffers
// iMinSource =  input source map value of the nearest neighbour
// iLineChanges = number of changes of distances or values in a line
// iWeight = value of input weigth mape
// iMinDist = minimun distance from (i, j) to nearest neighbour
// wDist = temporary minimun distance from (i, j) to nearest neighbour
// iW1 = first value of filter of distance
// iW2 = second value of filter of distance
// iStart = position to read/write in temporary file


  bool fCalcThiessen = sMapThiessen.length() != 0;
  trq.SetText(SMAPTextBackward);
  iChanges = 0;
  iLineChanges = 0;
  trq.fUpdate(iChanges); 
  for (iCurLine = iLines() - 1; iCurLine >= 0; iCurLine--) {
    if (iLineChanges != 0) {
      String s = String(SMAPTextBackwardChanges_i.scVal(), iChanges);
      trq.SetText(s);
    }
    if (trq.fUpdate(iCurLine, iLines()))
      return false;
    iLineChanges = 0;
    if (mpWeight.fValid())
      mpWeight->GetLineVal(iCurLine, brCurrWeight);
    else
      for (iCurCol = 0; iCurCol < iColumns; iCurCol++)
        brCurrWeight[iCurCol] = 1;
//    CheckErrP(mpWeight);
    iStart = iCurLine * 1L * iColumns * sizeof(long);
    if (fCalcThiessen)
      iStart = iCurLine * 2L * iColumns * sizeof(long);
    filTemp->Seek(iStart);
    filTemp->Read(iColumns*sizeof(long), iCurrDist.buf());
    if (fCalcThiessen)
      filTemp->Read(iColumns*sizeof(long), iCurrSource.buf());

    for (iCurCol = iColumns - 1; iCurCol >= 0; iCurCol--) {
      iWeight = (long)brCurrWeight[iCurCol];
      if (iWeight == iUNDEF)
        continue;
      if (iWeight >= 0) {
        if (iWeight == 0) {
          iW1 = 0;
          iW2 = 0;
        }
        else {
          if (iWeight != 1) {
  //          if (iWeight > 1000)  iWeight = 1000;
            iW1 = 5 * iWeight;
            iW2 = 7 * iWeight;
          }
          else {
            iW1 = 5;
            iW2 = 7;
          }
        }
        iMinDist = iCurrDist[iCurCol];
        if (iMinDist == iUNDEF)
          continue;
        if (iCurCol < iColumns - 1) {
          if (iCurrDist[iCurCol + 1] != iUNDEF) {
            iDist = iCurrDist[iCurCol + 1] + iW1;
            if (iDist < iMinDist) {
              iMinDist = iDist;
              if (fCalcThiessen) 
                iMinSource = iCurrSource[iCurCol + 1];
            }
          }
        }

        if (iCurCol > 0) {
          if (iCurrDist[iCurCol - 1] != iUNDEF) {
            iDist = iCurrDist[iCurCol - 1] + iW1;
            if (iDist < iMinDist) {
              iMinDist = iDist;
              if (fCalcThiessen) 
                iMinSource = iCurrSource[iCurCol - 1];
            }
          }
        }

        if (iCurLine < iLines() - 1) {
          if (iPrevDist[iCurCol] != iUNDEF) {
            iDist = iPrevDist[iCurCol] + iW1;
            if (iDist < iMinDist) {
              iMinDist = iDist;
              if (fCalcThiessen) 
                iMinSource = iPrevSource[iCurCol];
            }
          }
        }

        if ((iCurLine < iLines() - 1) && (iCurCol > 0)) {
          if (iPrevDist[iCurCol - 1] != iUNDEF) {
            iDist = iPrevDist[iCurCol - 1] + iW2;
            if (iDist < iMinDist) {
              iMinDist = iDist;
              if (fCalcThiessen) 
                iMinSource = iPrevSource[iCurCol - 1];
            }
          }
        }

        if ((iCurLine < iLines() - 1) && (iCurCol < iColumns - 1)) {
          if (iPrevDist[iCurCol + 1] != iUNDEF) {
            iDist = iPrevDist[iCurCol + 1] + iW2;
            if (iDist < iMinDist) {
              iMinDist = iDist;
              if (fCalcThiessen) 
                iMinSource = iPrevSource[iCurCol + 1];
            }
          }
        }

        if (iCurrDist[iCurCol] != iMinDist) {
          iLineChanges = iLineChanges++;
          iCurrDist[iCurCol] = iMinDist;
          if (fCalcThiessen) 
            iCurrSource[iCurCol] = iMinSource;
        }
      }
    }

    if (iLineChanges != 0) {
      iStart = iCurLine * 1L * iColumns * sizeof(long);
      if (fCalcThiessen)
        iStart = iCurLine * 2L * iColumns * sizeof(long);
      filTemp->Seek(iStart);
      filTemp->Write(iColumns*sizeof(long), iCurrDist.buf());
      if (fCalcThiessen)
        filTemp->Write(iColumns*sizeof(long), iCurrSource.buf());
    }
    Swap(iPrevDist, iCurrDist);
    Swap(iPrevSource, iCurrSource);
    iChanges = iChanges + iLineChanges;
  }
  trq.fUpdate(iLines(), iLines());
  return true;
}

bool MapDistance::fCorrectDistances()
{
  Map mpThiessen;
  if (sMapThiessen.length() != 0) { // create thiessen map
    FileName fnCrMap(sMapThiessen, fnObj);
    fnCrMap.sExt = ".mpr";
    try {
      mpThiessen = Map(fnCrMap);
    }
    catch (const ErrorObject&) {  // not found
      String sWeight = "1";
      if (mpWeight.fValid())
        sWeight = mpWeight->sNameQuoted(true, fnObj.sPath());
      mpThiessen = Map(fnCrMap, String("MapThiessen(%S,%S,%S)", 
                mp->sNameQuoted(true, fnObj.sPath()), sWeight, 
                ptr.sNameQuoted(true, fnObj.sPath())));
      mpThiessen->CreateMapStore();
    }
  }    
  if (mpThiessen.fValid())
    mpThiessen->KeepOpen(true);
  long iColumns = iCols();
  float rCorrFac = (float)gr()->rPixSize() / 5 * 0.968; // 0.9523212; // 28/10/96 Wim - the value 0.968 seems to be better
  // Best correction factor, smallest variance
  RealBuf rBufDist(iColumns);
  LongBuf iThiess(iColumns);
  trq.SetText(SMAPTextFinal);
  filTemp->Seek(0);
  for (long iLine = 0; iLine < iLines(); iLine++) {
    if (!(iLine % 10))
      if (trq.fUpdate(iLine, iLines()))
        return false;
    filTemp->Read(iColumns*sizeof(long), iCurrDist.buf());
    if (mpThiessen.fValid())
      filTemp->Read(iColumns*sizeof(long), iThiess.buf());
    for (long iCol = 0; iCol < iColumns; iCol++) {
      if (iCurrDist[iCol] == HIVAL)
        rBufDist[iCol] = rUNDEF;
      else
        rBufDist[iCol] = iCurrDist[iCol] * rCorrFac;
    }
    if (mpThiessen.fValid())
      mpThiessen->PutLineRaw(iLine, iThiess);
    pms->PutLineVal(iLine, rBufDist);
  }
  trq.fUpdate(iLines(), iLines());
  if (mpThiessen.fValid())
    mpThiessen->KeepOpen(false);
  return true;
}




