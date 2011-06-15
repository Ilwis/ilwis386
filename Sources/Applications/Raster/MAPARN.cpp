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


#include "Applications\Raster\MAPARN.H"
#include "Engine\Table\Col.h"
#include "Engine\Table\COLSTORE.H"
#include "Engine\Table\tblstore.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Domain\dmsort.h"
#include "Engine\Base\DataObjects\WPSMetaData.h"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Hs\map.hs"

#define MAXAREAS 1L

IlwisObjectPtr * createMapAreaNumbering(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)MapAreaNumbering::create(fn, (MapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new MapAreaNumbering(fn, (MapPtr &)ptr);
}

String wpsmetadataMapAreaNumbering() {
	WPSMetaData metadata("MapAreaNumbering");
	metadata.AddTitle("MapAreaNumbering");
	metadata.AddAbstract("A raster operation which assigns unique identifiers to pixels with the same class names or values that are horizontally, vertically or diagonally connected");
	metadata.AddKeyword("spatial");
	metadata.AddKeyword("raster");
	metadata.AddKeyword("Classification");
	WPSParameter *parm1 = new WPSParameter("1","Input Map", WPSParameter::pmtRASMAP);
	parm1->AddAbstract("Input raster map");
	WPSParameter *parm2 = new WPSParameter("2","Connected", WPSParameter::pmtINTEGER);
	parm2->AddAbstract("Indicates whether to construct 8-connected or 4-connected areas");
	metadata.AddParameter(parm1);
	metadata.AddParameter(parm2);
	WPSParameter *parmout = new WPSParameter("Result","Output Map",WPSParameter::pmtRASMAP, false);
	parmout->AddAbstract("Reference Outputmap and supporting data objects");
	metadata.AddParameter(parmout);
	

	return metadata.toString();
}

ApplicationMetadata metadataMapAreaNumbering(ApplicationQueryData *query) {
	ApplicationMetadata md;
	if ( query->queryType == "WPSMETADATA" || query->queryType == "") {
		md.wpsxml = wpsmetadataMapAreaNumbering();
	}
	if ( query->queryType == "OUTPUTTYPE" || query->queryType == "")
		md.returnType = IlwisObject::iotRASMAP;
	if ( query->queryType == "EXPERSSION" || query->queryType == "")
		md.skeletonExpression =  MapAreaNumbering::sSyntax();

	return md;
}
const char* MapAreaNumbering::sSyntax() {
  return "MapAreaNumbering(map,4|8)";
}

MapAreaNumbering* MapAreaNumbering::create(const FileName& fn, MapPtr& p, const String& sExpr)
{
  Array<String> as;
  int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  if ((iParms < 1) || (iParms > 3))
    ExpressionError(sExpr, sSyntax());
  Map mp(as[0], fn.sPath());
  short iCon = 8;
  bool fPolFrmRas = false;
  if (iParms > 1) {
    iCon = as[1].shVal();
    if ((iCon != 4) && (iCon != 8))
      ExpressionError(sExpr, sSyntax());
    if (iParms == 3)
      if ("0" == as[2])
        fPolFrmRas = true;
  }
  return new MapAreaNumbering(fn, p, mp, iCon==8, fPolFrmRas);
}

MapAreaNumbering::MapAreaNumbering(const FileName& fn, MapPtr& p)
: MapFromMap(fn, p)
{
  ReadElement("MapAreaNumbering", "8Connected", f8Connected);
  fNeedFreeze = true;
  fUseForPolFromRas = false;
  sFreezeTitle = "MapAreaNumbering";
  htpFreeze = "ilwisapp\\areanumbering_algorithm.htm";
}

MapAreaNumbering::MapAreaNumbering(const FileName& fn, MapPtr& p,
  const Map& mp, bool f8Con, bool fUseForPolFrmRas)
: MapFromMap(fn, p, mp), f8Connected(f8Con)
{
	Domain dom(fnObj, 0, dmtUNIQUEID, "Area");
  SetDomainValueRangeStruct(dom);
  FileName fnAtt(fnObj, ".tbt", true);
	Table	tblAtt(fnAtt, dm());
	String sColName = mp->fnObj.sFile;
	if ("" == sColName)
		sColName = mp->sName();
	Column col = tblAtt->colNew(sColName, mp->dvrs());
	SetAttributeTable(tblAtt);
  fNeedFreeze = true;
  fUseForPolFromRas = fUseForPolFrmRas;
  if (!fnObj.fValid())
    objtime = objdep.tmNewest();
  sFreezeTitle = "MapAreaNumbering";
  htpFreeze = "ilwisapp\\areanumbering_algorithm.htm";
  ptr.Store(); // store domain and att table info
  Store();
}

void MapAreaNumbering::Store()
{
  MapFromMap::Store();
  WriteElement("MapFromMap", "Type", "MapAreaNumbering");
  WriteElement("MapAreaNumbering", "8Connected", f8Connected);
}

MapAreaNumbering::~MapAreaNumbering()
{
}

bool MapAreaNumbering::fFreezing()
{
	long l, iLastColPlus1;
	long iArn, iTempArn, iCount;

	FileName fnTmp("ilwarn", ".tmp");
	fnTmp.Dir(fnObj.sPath());
	File filTemp(FileName::fnUnique(fnTmp), facCRT);
	filTemp.SetErase(true);
	filTemp.KeepOpen(true);
	// l = current line
	// iLastColPlus1 = Col after final col of the current line track
	// iTempArn = temporary area number
	// iCount = width of the current line track
	// iArn = number of the current areanumbering

	iArn = 0;
	iArnLine.Size(iCols(), 1, 1);
	iPrevArnLine.Size(iCols(), 1, 1);
	//  ArnToBeReplacedWith.Resize(MAXAREAS);
	ArnToBeReplacedWith &= 0;
	if (!mp->dvrs().fRealValues())
		ArnRawAtt &= iUNDEF;
	else
		ArnRealAtt &= rUNDEF;
	long i;
	for (i=-1; i <= iCols(); i++)
		iPrevArnLine[i] = 0;
	for (i=-1; i <= iCols(); i++)
		iArnLine[i] = iUNDEF;
	trq.SetText(SMAPMsgFirstPass);
	if (!mp->dvrs().fRealValues()) {
		long iValue;
		iCurrLine.Size(iCols(), 1, 1);
		iPrevLine.Size(iCols(), 1, 1);
		for (i=-1; i <= iCols(); i++)
			iPrevLine[i] = iUNDEF;
		for (l=0; l<iLines(); l++){
			mp->GetLineRaw(l, iCurrLine);
			if (fUseForPolFromRas) // undefined should also become areas
				// so don't make them undef anymore (by adding one)
				for (i=0; i < iCols(); i++)
					if (iCurrLine[i] == iUNDEF)
						iCurrLine[i]++;

			iCurrLine[-1] = iUNDEF;
			iCurrLine[iCols()] = iUNDEF;
			/*      
			if (iArn > MAXAREAS) {
			ErrorObject(WhatError(String("More than %li areas", MAXAREAS), errMapAreaNumbering+1), sTypeName()).Show();
			return false;
			}*/
			iValue = iCurrLine[0];
			iCount = 1;
			for (iLastColPlus1 = 1; iLastColPlus1 < iCols(); iLastColPlus1++) {
				if (iValue == iCurrLine[iLastColPlus1])
					iCount++;
				else {
					if (iValue != iUNDEF)
						iTempArn = iAreaNumber(iValue, iArn, iLastColPlus1, iCount);
					else
						iTempArn = iUNDEF;
					for (i = iLastColPlus1 - iCount; i < iLastColPlus1; i++)
						iArnLine[i] = iTempArn;
					iValue = iCurrLine[iLastColPlus1];
					iCount = 1;
				}
			}
			if (iValue != iUNDEF)
				iTempArn = iAreaNumber(iValue, iArn, iLastColPlus1, iCount);
			else
				iTempArn = iUNDEF;
			for (i = iLastColPlus1 - iCount; i < iLastColPlus1; i++)
				iArnLine[i] = iTempArn;
			filTemp.Write(iCols()*sizeof(long), iArnLine.buf());

			//      ptr->PutLineRaw(l, iArnLine);
			if (!(l % 5))
				if (trq.fUpdate(l, iLines())) return false;

			Swap(iPrevLine, iCurrLine);
			Swap(iPrevArnLine, iArnLine);
			iPrevArnLine[-1] = 0;
			iPrevArnLine[iCols()] = 0;
		}
		iCurrLine.Size(0);
		iPrevLine.Size(0);
	}
	else { // use real values
		double rValue;
		rCurrLine.Size(iCols(), 1, 1);
		rPrevLine.Size(iCols(), 1, 1);
		for (i=-1; i <= iCols(); i++)
			rPrevLine[i] = rUNDEF;
		for (l=0; l<iLines(); l++){
			mp->GetLineVal(l, rCurrLine);
			if (fUseForPolFromRas) // undefined should also become areas
				// so don't make them undef anymore (by multiplying by 10
				for (i=0; i < iCols(); i++)
					if (rCurrLine[i] == rUNDEF)
						rCurrLine[i] *= 10;
			rCurrLine[-1] = rUNDEF;
			rCurrLine[iCols()] = rUNDEF;
			/*      
			if (iArn > MAXAREAS) {
			ErrorObject(WhatError(String("More than %li areas", MAXAREAS), errMapAreaNumbering+1), sTypeName()).Show();
			return false;
			}*/
			rValue = rCurrLine[0];
			iCount = 1;
			for (iLastColPlus1 = 1; iLastColPlus1 < iCols(); iLastColPlus1++) {
				if (rValue == rCurrLine[iLastColPlus1])
					iCount++;
				else {
					if (rValue != rUNDEF)
						iTempArn = iAreaNumber(rValue, iArn, iLastColPlus1, iCount);
					else
						iTempArn = iUNDEF;
					for (i = iLastColPlus1 - iCount; i < iLastColPlus1; i++)
						iArnLine[i] = iTempArn;
					rValue = rCurrLine[iLastColPlus1];
					iCount = 1;
				}
			}
			if (rValue != rUNDEF)
				iTempArn = iAreaNumber(rValue, iArn, iLastColPlus1, iCount);
			else
				iTempArn = iUNDEF;
			for (i = iLastColPlus1 - iCount; i < iLastColPlus1; i++)
				iArnLine[i] = iTempArn;

			filTemp.Write(iCols()*sizeof(long), iArnLine.buf());
			//    ptr->PutLineRaw(l, iArnLine);
			if (!(l % 5))
				if (trq.fUpdate(l, iLines())) return false;

			Swap(rPrevLine, rCurrLine);
			Swap(iPrevArnLine, iArnLine);
			iPrevArnLine[-1] = 0;
			iPrevArnLine[iCols()] = 0;
		}
		rCurrLine.Size(0);
		rPrevLine.Size(0);
	}
	if (trq.fUpdate(l, iLines())) return false;

	//final areanumbering
	long iFinalArn = 1; // final area number
	// determine nr of final areas
	for (iTempArn = 1; iTempArn <= iArn; iTempArn++)
		if (ArnToBeReplacedWith[iTempArn] == 0)
			iFinalArn++;

	DomainSort *pds = dm()->pdsrt();
	pds->Resize(iFinalArn-1);

	FileName fnAttTbl(fnObj, ".tbt", true);
	Table tbl = tblAtt();
	if (!tbl.fValid()) {
		FileName fnAtt(fnObj, ".tbt", true);
		tbl = Table(fnAtt, dm());
		SetAttributeTable(tbl);
	}
	else
		tbl->CheckNrRecs();
	Column colAr, colNPix;
	String sColName = mp->fnObj.sFile;
	if ("" == sColName)
		sColName = mp->sName();
	Column col = tbl->col(sColName);
	if (!col.fValid())
		col = tbl->colNew(sColName, mp->dvrs());
	col->fErase = true;

	double rPixelSize = mp->gr()->rPixSize();
	bool fLatLon = (0 != mp->gr()->cs()->pcsLatLon());
	bool fKnownPixelSize = !(fLatLon || rPixelSize == rUNDEF);
	//false in case ofunrealistic or unreliable pixsize (e.g. latlon) or no georef att all
	colNPix = tbl->col(String("npix"));
	if (!colNPix.fValid())
		colNPix = tbl->colNew(String("npix"), Domain("count"));
	colNPix->fErase = true;
	if (fKnownPixelSize) 
	{
		colAr = tbl->col(String("Area"));
		if (!colAr.fValid())
			colAr = tbl->colNew(String("Area"), Domain("value"), ValueRange(0,1e10,0.01));
		colAr->fErase = true;
	}

	trq.SetText(SMAPTextCreateAttrTable);
	iFinalArn = 1;
	for (iTempArn = 1; iTempArn <= iArn; iTempArn++) {
		if (ArnToBeReplacedWith[iTempArn] == 0) {
			if (mp->dvrs().fRealValues()) {
				double rVal = ArnRealAtt[iTempArn];
				if (fUseForPolFromRas) // change 10*rUNDEF to rUNDEF
					if (fabs(rVal - rUNDEF)<1e-10)
						rVal = rUNDEF;
				col->PutVal(iFinalArn, ArnRealAtt[iTempArn]);
			}  
			else {
				long iRaw = ArnRawAtt[iTempArn];
				if (fUseForPolFromRas) // change iUNDEF+1 to iUNDEF
					if (iRaw == iUNDEF+1)
						iRaw = iUNDEF;
				col->PutRaw(iFinalArn, ArnRawAtt[iTempArn]);
			}  
			ArnToBeReplacedWith[iTempArn] = iFinalArn++;   // add new areanumbering value
		}
		else
			ArnToBeReplacedWith[iTempArn] = ArnToBeReplacedWith[ArnToBeReplacedWith[iTempArn]]; // take final areanumbering
		if (!(iTempArn % 10))
			if (trq.fUpdate(iTempArn, iArn)) 
				return false;
	}
	col->fErase = false;
	tbl->fErase = false;
	trq.SetText(SMAPMsgSecondPass);
	// renumber
	CreateMapStore();
	Store();
	filTemp.Seek(0);
	long iAreaNum;
	LongArrayLarge aiNrPix; 
	aiNrPix.Resize(iArn);
	RealArrayLarge arArea;
	arArea.Resize(iArn);
	double rPixelArea;

	if (fKnownPixelSize)
		rPixelArea = rPixelSize * rPixelSize; // areas are quadratic

	for (i = 0; i < iArn; i++)
	{
		aiNrPix[i] = 0;	
		arArea[i] = 0;	
	}
	for (l=0; l<iLines(); l++) {
		//  ps->GetLineRaw(l, iArnLine);
		filTemp.Read(iCols()*sizeof(long), iArnLine.buf());
		for (i = 0; i < iCols(); i++){
			if (iArnLine[i] != iUNDEF) {
				iArnLine[i] = ArnToBeReplacedWith[iArnLine[i]];
				iAreaNum = iArnLine[i];
				aiNrPix[iAreaNum-1]++;
				if (fKnownPixelSize)
					arArea[iAreaNum-1] += rPixelArea;
			}
		}
		pms->PutLineRaw(l, iArnLine);
		if (!(l % 10))
			if (trq.fUpdate(l, iLines())) { /*delete ps;*/ return false; }
	}
	for (i = 1; i <= iArn; i++) 
		colNPix->PutVal(i, aiNrPix[i-1]);
	if (fKnownPixelSize)
		for (i = 1; i <= iArn; i++) 
			colAr->PutVal(i, arArea[i-1]);

	//delete ps;
	if (trq.fUpdate(l, iLines())) return false;
	col->fErase = false;
	colNPix->fErase = false;
	if (fKnownPixelSize)
		colAr->fErase = false;
	tbl->Updated();
	iArnLine.Size(0);
	iPrevArnLine.Size(0);
	ArnToBeReplacedWith.Resize(0);
	ArnRawAtt.Resize(0);
	String sName = filTemp.sName();
	return true;
}

long MapAreaNumbering::iAreaNumber (long iValue, long& iArn, long iLastColPlus1,
                 long iCount)
{
  long j, iTempArn, iSta, iEnd, iTempArnChange, iTempArnNew;

// j = current col of the segment
// iTempArn = temporary arenumbering of the current segment
// iSta = start of current segment
// iEnd = end of current segment
// iTempArnChange = temporary store of the areanumbering to change
// iTempArnNew = temporary store of the new areanumbering

  iSta = iLastColPlus1 - iCount;
  if (f8Connected && !fUseForPolFromRas)
    iSta--;
  iEnd = iLastColPlus1 - 1;
  if (f8Connected && !fUseForPolFromRas)
    iEnd++;
  iTempArn = 0;
  j = iSta;
  if (fUseForPolFromRas && f8Connected) {
    if (iValue == iPrevLine[j - 1]) { // check pixel to upper left
      iTempArn = iPrevArnLine[j - 1];
        if ((iCurrLine[j - 1] == iPrevLine[j]) && (iPrevLine[j] > iValue))
          // if pixel to left and pixel to upper right are the same
          // and the pixel values are larger than the current
          iTempArn = 0;
    }
    if (iValue == iPrevLine[iLastColPlus1]) {
      // pixel to upper right of subline equals subline pixel values
      if (! ((iCurrLine[iLastColPlus1] == iPrevLine[iLastColPlus1 - 1]) &&
          (iCurrLine[iLastColPlus1] > iValue))) {
        // if pixel to right and pixel to upper left are the same
        // and the pixel values are larger than the current then they
        // will be connected and not this combination
        if (iTempArn == 0)
          iTempArn = iPrevArnLine[iLastColPlus1];
        else
          iEnd = iLastColPlus1;
      }
    }
  }

  for (j = iSta; j <= iEnd; j++) {
    if (iValue == iPrevLine[j]) {
      if (iTempArn == 0)  // take area number
        iTempArn = iPrevArnLine[j];
      else
        if (iTempArn != iPrevArnLine[j]) {
          // have to list change combination
          if (iTempArn > iPrevArnLine[j]) {
            iTempArnChange = iTempArn;
            iTempArnNew = iPrevArnLine[j];
            iTempArn = iPrevArnLine[j];
          }
          else {
            iTempArnChange = iPrevArnLine[j];
            iTempArnNew = iTempArn;
          }
          while ( 1 ) {
            if (ArnToBeReplacedWith[iTempArnChange] == 0) {
              ArnToBeReplacedWith[iTempArnChange] = iTempArnNew;
              break;                // stores new change
            }
            if (ArnToBeReplacedWith[iTempArnChange] == iTempArnNew)
              break;   // change combination exists already

         // creates new change combination with the new areanumbering values.
         // Case 1: if 8->3 comb. exists already, and 8->5 is new change comb,
         // then new change comb. 5->3 is added, and change comb. 8->3 remains.
         // Case 2: if 9->5 comb. exists already, and 9->2 is new change comb,
         // then new change comb. 5->2 is added, and change comb. 9->5 is
         // replaced by 9->2.
         // The search continues until it could store a change comb. or an already
         // existing is found.

            if (ArnToBeReplacedWith[iTempArnChange] < iTempArnNew) {
              long h  = iTempArnChange;
              iTempArnChange = iTempArnNew;
              iTempArnNew = ArnToBeReplacedWith[h];
            }
            else {
              long h = iTempArnChange;
              iTempArnChange = ArnToBeReplacedWith[iTempArnChange];
              ArnToBeReplacedWith[h] = iTempArnNew;
            }
          }
        }
    }
  }

  if (iTempArn == 0) { // no connection found: create new number
    iTempArn = ++iArn;
//    ArnRawAtt[iTempArn] = iValue;
    ArnToBeReplacedWith &= 0L;
    ArnRawAtt &= iValue;
  }
  return iTempArn;
}

long MapAreaNumbering::iAreaNumber (double rValue, long& iArn, long iLastColPlus1,
                 long iCount)
{
  long j, iTempArn, iSta, iEnd, iTempArnChange, iTempArnNew;

// j = current col of the segment
// iTempArn = temporary arenumbering of the current segment
// iSta = start of current segment
// iEnd = end of current segment
// iTempArnChange = temporary store of the areanumbering to change
// iTempArnNew = temporary store of the new areanumbering

  iSta = iLastColPlus1 - iCount;
  if (f8Connected && !fUseForPolFromRas)
    iSta--;
  iEnd = iLastColPlus1 - 1;
  if (f8Connected && !fUseForPolFromRas)
    iEnd++;
  iTempArn = 0;
  j = iSta;
  if (fUseForPolFromRas) {
    if (rValue == rPrevLine[j - 1]) { // check pixel to upper left
      iTempArn = iPrevArnLine[j - 1];
        if ((rCurrLine[j - 1] == rPrevLine[j]) && (rPrevLine[j] > rValue))
          // if pixel to left and pixel to upper right are the same
          // and the pixel values are larger than the current
          iTempArn = 0;
    }
    if (rValue == rPrevLine[iLastColPlus1]) {
      // pixel to upper right of subline equals subline pixel values
      if (! ((rCurrLine[iLastColPlus1] == rPrevLine[iLastColPlus1 - 1]) &&
          (rCurrLine[iLastColPlus1] > rValue))) {
        // if pixel to right and pixel to upper left are the same
        // and the pixel values are larger than the current then they
        // will be connected and not this combination
        if (iTempArn == 0)
          iTempArn = iPrevArnLine[iLastColPlus1];
        else
          iEnd = iLastColPlus1;
      }
    }
  }

  for (j = iSta; j <= iEnd; j++) {
    if (rValue == rPrevLine[j]) {
      if (iTempArn == 0)  // take area number
        iTempArn = iPrevArnLine[j];
      else
        if (iTempArn != iPrevArnLine[j]) {
          // have to list change combination
          if (iTempArn > iPrevArnLine[j]) {
            iTempArnChange = iTempArn;
            iTempArnNew = iPrevArnLine[j];
            iTempArn = iPrevArnLine[j];
          }
          else {
            iTempArnChange = iPrevArnLine[j];
            iTempArnNew = iTempArn;
          }
          while ( 1 ) {
            if (ArnToBeReplacedWith[iTempArnChange] == 0) {
              ArnToBeReplacedWith[iTempArnChange] = iTempArnNew;
              break;                // stores new change
            }
            if (ArnToBeReplacedWith[iTempArnChange] == iTempArnNew)
              break;   // change combination exists already

         // creates new change combination with the new areanumbering values.
         // Case 1: if 8->3 comb. exists already, and 8->5 is new change comb,
         // then new change comb. 5->3 is added, and change comb. 8->3 remains.
         // Case 2: if 9->5 comb. exists already, and 9->2 is new change comb,
         // then new change comb. 5->2 is added, and change comb. 9->5 is
         // replaced by 9->2.
         // The search continues until it could store a change comb. or an already
         // existing is found.

            if (ArnToBeReplacedWith[iTempArnChange] < iTempArnNew) {
              long h  = iTempArnChange;
              iTempArnChange = iTempArnNew;
              iTempArnNew = ArnToBeReplacedWith[h];
            }
            else {
              long h = iTempArnChange;
              iTempArnChange = ArnToBeReplacedWith[iTempArnChange];
              ArnToBeReplacedWith[h] = iTempArnNew;
            }
          }
        }
    }
  }

  if (iTempArn == 0) { // no connection found: create new number
    iTempArn = ++iArn;
//    ArnRawAtt[iTempArn] = rValue;
    ArnToBeReplacedWith &= 0L;
    ArnRealAtt &= rValue;
  }
  return iTempArn;
}
/*
int MapAreaNumbering::iAreaNumber (double rValue, int& iArn, int iLastColPlus1,
                 int iCount)
{
  int j, iTempArn, iSta, iEnd, iTempArnChange, iTempArnNew;

// j = current col of the subline
// iTempArn = temporary arenumbering of the current subline
// iSta = start of current subline
// iEnd = end of current subline
// iTempArnChange = temporary store of the areanumber to change
// iTempArnNew = temporary store of the new areanumber

  iSta = iLastColPlus1 - iCount;
  if (f8Connected && !fUseForPolFromRas)
    iSta--;
  iEnd = iLastColPlus1 - 1;
  if (f8Connected && !fUseForPolFromRas)
    iEnd++;
  iTempArn = 0;
  j = iSta;
  if (fUseForPolFromRas) {
    if (rValue == rPrevLine[j - 1]) {
      iTempArn = iPrevArnLine[j - 1];
      if ((rCurrLine[j - 1] == rPrevLine[j]) && (rPrevLine[j] > rValue))
        iTempArn = 0;
    } // assigns area number from first left value out of range of subline
    if (iLastColPlus1 < rCurrLine.iUpper()){
      if (rValue == rPrevLine[iLastColPlus1]) {
        iEnd = iLastColPlus1;
        if ((rCurrLine[iLastColPlus1] == rPrevLine[iLastColPlus1 - 1]) &&
           (rCurrLine[iLastColPlus1] > rValue))
           iEnd--;
      }// changes temporary of iEnd to take area number from first right value
    }
  }

  for (j = iSta; j <= iEnd; j++) {
    if (rValue == rPrevLine[j]) {
      if (iTempArn == 0)  // assign area number of pixel in previous line
        iTempArn = iPrevArnLine[j];
      else { // have to list change combination
        if (iTempArn > iPrevArnLine[j]) {
          iTempArnChange = iTempArn;
          iTempArnNew = iPrevArnLine[j];
          iTempArn = iPrevArnLine[j];

        }
        else {
          iTempArnChange = iPrevArnLine[j];
          iTempArnNew = iTempArn;
        }
        while ( 1 ) {
          if (ArnToBeReplacedWith[iTempArnChange] == 0) {
            ArnToBeReplacedWith[iTempArnChange] = iTempArnNew;
            break;  // stores new change combination
          }
          if (ArnToBeReplacedWith[iTempArnChange] == iTempArnNew)
             break;   // change combination exists already

         // creates new change combination with the new areanumbering values.
         // Case 1: if 8->3 comb. exists already, and 8->5 is new change comb,
         // then new change comb. 5->3 is added, and change comb. 8->3 remains.
         // Case 2: if 9->5 comb. exists already, and 9->2 is new change comb,
         // then new change comb. 5->2 is added, and change comb. 9->5 is
         // replaced by 9->2.
         // The search continues until it could store a change comb. or an already
         // existing is found.

          if (ArnToBeReplacedWith[iTempArnChange] < iTempArnNew) { // case 1
            int h  = iTempArnChange;
            iTempArnChange = iTempArnNew;
            iTempArnNew = ArnToBeReplacedWith[h];
          }
          else { // case 2
            int h = iTempArnChange;
            iTempArnChange = ArnToBeReplacedWith[iTempArnChange];
            ArnToBeReplacedWith[h] = iTempArnNew;
          }
        }
      }
    }
  }

//  iEnd = iLastColPlus1 - 1;  // takes the true value of the end of subline
  if (iTempArn == 0) // no connection found: create new number
    iTempArn = ++iArn;
  return iTempArn;
}
*/
String MapAreaNumbering::sExpression() const
{
  String sCon = "8";
  if (!f8Connected)
    sCon = "4";
  String s("MapAreaNumbering(%S,%s)", mp->sNameQuoted(true, fnObj.sPath()),
           f8Connected ? "8" : "4");
  return s;
}

bool MapAreaNumbering::fDomainChangeable() const
{
  return false;
}

bool MapAreaNumbering::fValueRangeChangeable() const
{
  return false;
}




