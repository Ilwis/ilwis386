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
// StereoPairFromDTM.cpp: implementation of the StereoPairFromDTM class.
//
//////////////////////////////////////////////////////////////////////

#include "Engine\SpatialReference\GrStereoMate.h"
#include "Engine\Stereoscopy\StereoPairFromDTM.h"
#include "Engine\SpatialReference\Coordsys.h"
#include "Engine\SpatialReference\csviall.h"
#include "Headers\Hs\stereoscopy.hs"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Hs\map.hs"
#include "Headers\Htp\Ilwisapp.htp"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

const char* StereoPairFromDTM::sSyntax() {
  return  "StereoPairFromDTM(InputImage, DTM [,look-angle[,\n"
														"referheight [,left|both|right [,fast|accurate]]]])";
}

//static void LookAngleBetwee0and90Error(const FileName& fn)
//{
//  throw ErrorObject(WhatError(String(SStcLookAngleFrom0to90), errStereoPairFromDTM), fn);
//}

StereoPairFromDTM* StereoPairFromDTM::create(const FileName& fn, StereoPairPtr& p, const String& sExpr)
{
  Array<String> as;
  short iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  if ((iParms < 2) || (iParms > 6))
    ExpressionError(sExpr, sSyntax());
  Map mpSrcImage(as[0], fn.sPath());
	Map mpDTM(as[1], fn.sPath());
	RangeReal rrMMDTM = mpDTM->rrMinMax(BaseMapPtr::mmmCALCULATE);  // find minmax height of DTM
	double rAngl = 30.0; //default if iParms < 3
	double rRefH = (rrMMDTM.rHi() + rrMMDTM.rLo()) / 2; //if iParms < 4
	LookModus lm = lmBOTH;															//if iParms < 5
	ResampleModus rm = rmFAST;

	if (iParms > 2) {
		if (as[2].rVal() != rUNDEF)
			rAngl = as[2].rVal();
		if (!((rAngl >= 0 && rAngl <= 90) || rAngl == rUNDEF))
			throw (ErrorObject(SStcErrLookAngleFrom0to90));
	}
	if (iParms > 3) {
		if (as[3].rVal() != rUNDEF)
			rRefH = as[3].rVal();
	}
  if (iParms > 4) 
	{
		if (fCIStrEqual("left", as[4]))
        lm = lmLEFT;
    else if (fCIStrEqual("both", as[4]) || as[4].length() == 0)
			lm = lmBOTH;
    else if (fCIStrEqual("right", as[4]))
      lm = lmRIGHT;
		else
      ExpressionError(sExpr, sSyntax());
	}
	if (iParms > 5) //all parameters supplied by user
	{
		if ((fCIStrEqual("fast", as[5])) || as[5].length() == 0)
			rm = rmFAST;
		else if (fCIStrEqual("accurate", as[5]))
			rm = rmACCURATE;
		else
			ExpressionError(sExpr, sSyntax());
	}
  return new StereoPairFromDTM(fn, p, mpSrcImage, mpDTM, rAngl, rRefH, lm, rm);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
/*
StereoPairFromDTM::StereoPairFromDTM()
{
} */

StereoPairFromDTM::StereoPairFromDTM(const FileName& fn, StereoPairPtr& p)
: StereoPairVirtual(fn, p)
{
	ReadElement("StereoPairFromDTM", "InputImage", mpInputImage);
	GeoRef grIn = mpInputImage->gr();
  ReadElement("StereoPairFromDTM", "DTM", mpDTM);
  ReadElement("StereoPairFromDTM", "LookAngle", rLookAngle);
  ReadElement("StereoPairFromDTM", "RefHeight", rRefHeight);
	String sLookModus;
	ReadElement("StereoPairFromDTM", "LookModus", sLookModus);
	if (fCIStrEqual("left", sLookModus))
    lookModus = lmLEFT;
  else if (fCIStrEqual("both", sLookModus)) 
    lookModus = lmBOTH;
  else if (fCIStrEqual("right", sLookModus))
    lookModus = lmRIGHT;
	double rLookAngleL, rLookAngleR;
	switch (lookModus) {
		case lmLEFT:
			rLookAngleL = -rLookAngle;
			rLookAngleR = 0;
			break;
		case lmBOTH:
			rLookAngleL = -rLookAngle/2.0;
			rLookAngleR = rLookAngle/2.0;
			break;
		case lmRIGHT:
			rLookAngleL = 0;
			rLookAngleR = rLookAngle;
			break;
	}
	ReadElement("StereoPairFromDTM", "ColumnOffset", iColOffSet);
	ReadElement("StereoPairFromDTM", "ColumnsExtended", iColsExtended);
	String sResampleModus;
	ReadElement("StereoPairFromDTM", "ResampleModus", sResampleModus);
	if (fCIStrEqual("fast", sResampleModus))
    resampleModus = rmFAST;
  else if (fCIStrEqual("accurate", sResampleModus)) 
    resampleModus = rmACCURATE; 
	
	objdep.Add(mpInputImage.ptr());
  objdep.Add(mpDTM.ptr());
	Init();
}

StereoPairFromDTM::StereoPairFromDTM(const FileName& fn, StereoPairPtr& p, const Map& mapImage,
										const Map& mapDTM,	double rLookAng, double rRefH, LookModus lm, ResampleModus rm)
: StereoPairVirtual(fn, p, true)
, mpInputImage(mapImage), mpDTM(mapDTM)
{
	GeoRef grIn = mpInputImage->gr();
	double rSrceMapPixSize = grIn->rPixSize();
	resampleModus = rm;
	
	rLookAngle = rLookAng;
	lookModus = lm;
	rRefHeight = rRefH;
	
	double rLookAngleL, rLookAngleR;
	FindSingleLookAngles(rLookAng, lookModus, rLookAngleL, rLookAngleR);
	
	long iRows = grIn->rcSize().Row;
	long iCols = grIn->rcSize().Col;
	double rLowLeft = -rUNDEF; //10^308
	double rHighLeft = rUNDEF; //-10^308
	double rLowRight = -rUNDEF;
	double rHighRight = rUNDEF;
	Coord cSource; //crd in SourceImage
	double rLeftEdge, rRightEdge;
	for (long iRow = 0; iRow < iRows; iRow++)  {  // find left and right edge minmax
		grIn->RowCol2Coord(iRow, 0, cSource);
		rLeftEdge = mapDTM->rValue(cSource);
		if (rLeftEdge < rLowLeft)
			rLowLeft = rLeftEdge;
		if (rLeftEdge > rHighLeft)
			rHighLeft = rLeftEdge;
		grIn->RowCol2Coord(iRow, iCols - 0.5, cSource);
		rRightEdge = mapDTM->rValue(cSource);
		if (rRightEdge < rLowRight)
			rLowRight = rRightEdge;
		if (rRightEdge > rHighRight)
			rHighRight = rRightEdge;
	}
	//size of new grf must be extended to the left with iColLeftExtra, 
	double rA = abs((rRefHeight - rLowLeft) * tan(rLookAngleL*M_PI/180));//needed for left image
	double rB = abs((rRefHeight - rHighLeft) * tan(rLookAngleR*M_PI/180));//needed for right image
	long iExtraLeft = (long)max(rA,rB) / rSrceMapPixSize;
	//size of new grf must be extended to the right  with iColRightExtra, 
	double rC = abs((rLowRight - rRefHeight) * tan(rLookAngleR*M_PI/180));//needed for right image
	double rD = abs((rHighRight - rRefHeight) * tan(rLookAngleL*M_PI/180));//needed for left image
	long iExtraRight = (long)max(rC,rD) / rSrceMapPixSize;
	//size of new grf must be extended to the right  with iColRightExtra, 
	iColOffSet = iExtraLeft;
	iColsExtended = iCols + iExtraLeft + iExtraRight;

	objdep.Add(mpInputImage.ptr());
  objdep.Add(mpDTM.ptr());
	Init();
}

StereoPairFromDTM::~StereoPairFromDTM()
{

}

void StereoPairFromDTM::Init()
{
  fNeedFreeze = true;
  sFreezeTitle = "StereoPairFromDTM";
  help = "ilwisapp\\stereopair_from_dtm_algorithm.htm";
	csDTM = mpDTM->gr()->cs();
	fTransformDTMCoords = csDTM != mpInputImage->cs();
	rSourceMapPixSize = mpInputImage->gr()->rPixSize();

	if (mpInputImage->cs()->pcsLatLon())
	{
		double rEarthRadius = mpInputImage->cs()->pcsViaLatLon()->ell.a;
		rSourceMapPixSize = mpInputImage->gr()->rPixSize() * rEarthRadius * M_PI/180;
	}
}

void StereoPairFromDTM::Store()
{
  StereoPairVirtual::Store();
  WriteElement("StereoPairVirtual", "Type", "StereoPairFromDTM");
	WriteElement("StereoPairFromDTM", "InputImage", mpInputImage);
	WriteElement("StereoPairFromDTM", "DTM", mpDTM);
	WriteElement("StereoPairFromDTM", "LookAngle", rLookAngle);
	WriteElement("StereoPairFromDTM", "RefHeight", rRefHeight);
	WriteElement("StereoPairFromDTM", "LookModus", lookModus);
	String sLookModus;
  switch (lookModus) {
    case lmLEFT: sLookModus = "left"; break; 
    case lmBOTH: sLookModus = "both"; break; 
		case lmRIGHT: sLookModus = "right"; break; 
	} 
	WriteElement("StereoPairFromDTM", "LookModus", sLookModus);
	WriteElement("StereoPairFromDTM", "ColumnOffset", iColOffSet);
	WriteElement("StereoPairFromDTM", "ColumnsExtended", iColsExtended);
	String sResampleModus;
  switch (resampleModus) {
    case rmFAST: sResampleModus = "fast"; break; 
    case rmACCURATE: sResampleModus = "accurate"; break; 
	} 
	WriteElement("StereoPairFromDTM", "ResampleModus", sResampleModus);
}


bool StereoPairFromDTM::fFreezing()
{
	FileName fnLeftOut (ptr.sName());
  fnLeftOut.sFile &= "_Left";
  fnLeftOut.sExt = ".mpr";
	FileName fnRightOut (ptr.sName());
  fnRightOut.sFile &= "_Right";
  fnRightOut.sExt = ".mpr";
	// New target grf names are made using the StereoPair name:
	FileName fnGrfLeft(ptr.sName());
	fnGrfLeft.sFile &= "_Left";
	fnGrfLeft.sExt = ".grf";
		
	FileName fnGrfRight(ptr.sName());
	fnGrfRight.sFile &= "_Right";
	fnGrfRight.sExt = ".grf";

	double rLookAngleL, rLookAngleR;
	FindSingleLookAngles(rLookAngle, lookModus , rLookAngleL, rLookAngleR);
	GeoRefStereoMate * grStMateLeft = new GeoRefStereoMate(fnGrfLeft, mpInputImage->gr(), mpDTM, rLookAngleL, rRefHeight, iColOffSet); 
	GeoRefStereoMate * grStMateRight = new GeoRefStereoMate(fnGrfRight, mpInputImage->gr(), mpDTM, rLookAngleR, rRefHeight, iColOffSet);
	grNewLeft.SetPointer(grStMateLeft);
  grNewRight.SetPointer(grStMateRight);
	grStMateLeft->SetRowCol(RowCol(mpInputImage->rcSize().Row, iColsExtended));
	grStMateRight->SetRowCol(RowCol(mpInputImage->rcSize().Row, iColsExtended));
	grNewLeft->Store();
	grNewRight->Store();
  ObjectInfo::WriteAdditionOfFileToCollection(fnGrfLeft, ptr.fnObj);
	ObjectInfo::WriteAdditionOfFileToCollection(fnGrfRight, ptr.fnObj);
	grStMateLeft->fErase = true;	
	grStMateRight->fErase = true;
	
	if (resampleModus == rmFAST) // fast resampling (Input driven parallel projection)
	{
		ptr.mapLeft = Map(fnLeftOut, grNewLeft, grNewLeft->rcSize(), mpInputImage->dvrs());
		ptr.mapRight = Map(fnRightOut, grNewRight, grNewRight->rcSize(), mpInputImage->dvrs());
		bool fUseReal = mpInputImage->st() == stREAL;
		long iInpCols = mpInputImage->iCols();
		long iInpLines = mpInputImage->iLines();
		long iColsExtended = ptr.mapLeft->iCols();
		RealBuf rBufIn(iInpCols), rBufOut(iColsExtended);
		LongBuf iBufIn(iInpCols), iBufOut(iColsExtended);

		trq.SetText(SStcUiCalcLeftStMate);
		for (long iRow = 0; iRow < iInpLines; ++iRow) 
		{
		  mpInputImage->GetLineRaw(iRow, iBufIn);
		  if (trq.fUpdate(iRow, iInpLines))
		     return false;
			long iColNew; 
			long iColOut = 0;

			RowCol rcIn = RowCol(iRow, 0L);
			iColNew = iParallProjectToStereoMate(rcIn, 
																 grNewLeft->pgStereoMate());
			if (iUNDEF == iColNew)
				iColNew = -1;
			while (iColNew > iColOut)
			{
				if (fUseReal)
					rBufOut[iColOut] = rUNDEF;
				else
					iBufOut[iColOut] = iUNDEF;
				++iColOut;
			}
		  for (long iColIn = 0; iColIn < iInpCols; ++iColIn) 
			{
				rcIn = RowCol(iRow, iColIn);
				iColNew = iParallProjectToStereoMate(rcIn, 
																	 grNewLeft->pgStereoMate());
				if (iUNDEF == iColNew)
					continue;
				while (iColNew >= iColOut && iColOut < iColsExtended)
				{ 
					if (fUseReal)
						rBufOut[iColOut] = rBufIn[iColIn];
					else
						iBufOut[iColOut] = iBufIn[iColIn];
					++iColOut;
				}
			}
			while (iColOut < iColsExtended)
			{ 
				if (fUseReal)
					rBufOut[iColOut] = rUNDEF;
				else
					iBufOut[iColOut] = iUNDEF;
				++iColOut;
			}
			if (fUseReal)
		    ptr.mapLeft->PutLineVal(iRow, rBufOut);
		  else
		    ptr.mapLeft->PutLineRaw(iRow, iBufOut);
		}
		
		trq.SetText(SStcUiCalcRightStMate);
		for (long iRow = 0; iRow < iInpLines; ++iRow) 
		{
		  mpInputImage->GetLineRaw(iRow, iBufIn);
		  if (trq.fUpdate(iRow, iInpLines))
		     return false;
			long iColNew; 
			long iColOut = iColsExtended - 1;

			RowCol rcIn = RowCol(iRow, iInpCols - 1L);
			iColNew = iParallProjectToStereoMate(rcIn, 
																 grNewRight->pgStereoMate());
			if (iUNDEF == iColNew)
				iColNew = iColOut;
			while (iColNew < iColOut)
			{
				if (fUseReal)
					rBufOut[iColOut] = rUNDEF;
				else
					iBufOut[iColOut] = iUNDEF;
				--iColOut;
			}
		  for (long iColIn = iInpCols - 1; iColIn >= 0; --iColIn) 
			{
				rcIn = RowCol(iRow, iColIn);
				iColNew = iParallProjectToStereoMate(rcIn, 
																	 grNewRight->pgStereoMate());
				if (iUNDEF == iColNew)
					continue;
				while (iColNew <= iColOut && iColOut >= 0)
				{ 
					if (fUseReal)
						rBufOut[iColOut] = rBufIn[iColIn];
					else
						iBufOut[iColOut] = iBufIn[iColIn];
					--iColOut;
				}
			}
			while (iColOut >= 0)
			{ 
				if (fUseReal)
					rBufOut[iColOut] = rUNDEF;
				else
					iBufOut[iColOut] = iUNDEF;
				--iColOut;
			}
			if (fUseReal)
		    ptr.mapRight->PutLineVal(iRow, rBufOut);
		  else
		    ptr.mapRight->PutLineRaw(iRow, iBufOut);
		}
	}
	else // Output driven resampling (using GeoRefStereoMate::RowCol2Coord ("ray-tracing")
	{
		String sResampleMethod;
		if(mpInputImage->dm()->pdi() || mpInputImage->dm()->pdv() || mpInputImage->dm()->pdcol())
			sResampleMethod = String("BiLinear");
		else
			sResampleMethod = String("NearestNeighbour");
		EraseOutputMap(ptr.mapLeft);
		ptr.mapLeft = Map(fnLeftOut,
											String("MapResample(%S,%S,%S,Patch,GrNoneAllowed)", 
											mpInputImage->fnObj.sFullNameQuoted(), grNewLeft->fnObj.sFullNameQuoted(),sResampleMethod));
		
		EraseOutputMap(ptr.mapRight);
		ptr.mapRight = Map(fnRightOut,
											String("MapResample(%S,%S,%S,Patch,GrNoneAllowed)", 
											mpInputImage->fnObj.sFullNameQuoted(), grNewRight->fnObj.sFullNameQuoted(),sResampleMethod));

		// Now perform the resampling
		trq.SetTitle(String(SStcTextStereoPfrom_S_andDTM.scVal(),mpInputImage->fnObj.sFullNameQuoted()));
		trq.SetText(SStcUiCalcLeftStMate);
		trq.fUpdate(1,100);
		ptr.mapLeft->Calc();
		if (!ptr.mapLeft->fCalculated())
			return false;
		
		trq.SetText(SStcUiCalcRightStMate);
		trq.fUpdate(50, 100);
		ptr.mapRight->Calc();
		if (!ptr.mapRight->fCalculated())
			return false;
		
		if (trq.fUpdate(100, 100))
		      return false;
		ptr.mapLeft->BreakDependency();
		ptr.mapRight->BreakDependency();
	}
	grStMateLeft->fErase = false;	
	grStMateRight->fErase = false;
	return true;
}

String StereoPairFromDTM::sExpression() const
{
	String sLookModus;
  switch (lookModus) {
    case lmLEFT: sLookModus = "left"; break; 
    case lmBOTH: sLookModus = "both"; break; 
		case lmRIGHT: sLookModus = "right"; break; 
	}
	String sResampleModus;
	switch (resampleModus) {
    case rmFAST: sResampleModus = "fast"; break; 
    case rmACCURATE: sResampleModus = "accurate"; break; 
	}
	return String("StereoPairFromDTM(%S,%S,%.2f,%.2f,%S,%S)",
											mpInputImage->sNameQuoted(true, fnObj.sPath()), 
											mpDTM->sNameQuoted(true, fnObj.sPath()),
											rLookAngle, rRefHeight, 
											sLookModus, sResampleModus);
}

long StereoPairFromDTM::iParallProjectToStereoMate(RowCol rcIn, GeoRefStereoMate* pgStMate)
{
	Coord crdIn = mpInputImage->gr()->cConv(rcIn);
	if (fTransformDTMCoords)
            crdIn = csDTM->cConv(mpInputImage->cs(), crdIn);
	double rZ = mpDTM->rValue(crdIn);
	if (rUNDEF == rZ)
		return iUNDEF;
	double rTan = tan(pgStMate->rGetLookAngle() * M_PI / 180);
	double rHei = pgStMate->rGetRefHeight();
	long iColRes = (long)(rcIn.Col - rTan * (rZ - rHei) / rSourceMapPixSize  + iColOffSet);
  if (iColRes < 0)
    iColRes = 0;
  if (iColRes >= iColsExtended)
    iColRes = iColsExtended - 1;
  return iColRes;
}

void StereoPairFromDTM::EraseOutputMap(Map &mp)
{
	if (mp.fValid())
	{
		mp->fErase = true;
		mp.SetPointer(0);
	} 
}

void StereoPairFromDTM::FindSingleLookAngles(double rLookAng, LookModus lm, double &rLookAngleL, double &rLookAngleR )
{
	switch (lm) {
		case lmLEFT:
			rLookAngleL = -rLookAng;
			rLookAngleR = 0;
			break;
		case lmBOTH:
			rLookAngleL = -rLookAng/2.0;
			rLookAngleR = rLookAng/2.0;
			break;
		case lmRIGHT:
			rLookAngleL = 0;
			rLookAngleR = rLookAng;
			break;
	}
}
