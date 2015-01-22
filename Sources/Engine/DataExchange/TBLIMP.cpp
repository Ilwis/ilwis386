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
/* $Log: /ILWIS 3.0/Import14/TBLIMP.cpp $
 * 
 * 6     30/10/00 12:59 Willem
 * - Number of records is now properly determined (went wrong is last line
 * not ended in LF or CRLF)
 * - Column repeat read now only results in 2 for coordinate column. Then
 * two columns need to be read.
 * 
 * 5     2/03/00 14:38 Willem
 * Table and Point import from Ilwis 1.4 now translate a sequence of an X
 * and a Y column into a Coordinate column
 * 
 * 4     9/29/99 10:23a Wind
 * added case insensitive string comparison
 * 
 * 3     20-07-99 13:57 Koolhoven
 * Header comment
 * 
 * 2     13/07/99 12:50 Willem
 * The atof() strtod() fucntion gave strange problems. (also during
 * linking). The functions have been renamed to be unique. They are now
 * called by their real name. The C library functions atof() and strtod()
 * are not used directly anymore
// Revision 1.5  1998/09/16 17:25:24  Wim
// 22beta2
//
// Revision 1.4  1998/06/10 13:08:49  Willem
// The ILWIS 1.4 conversions will now also work from the script.
//
// Revision 1.3  1997/11/18 17:25:30  Willem
// The line scanning routine (strtok) does now recognize the TAB character
// as a whitespace character.
//
// Revision 1.2  1997/09/24 18:57:58  Wim
// Changed default valuerange for column from "value" to -1e38:1e38:0
//
/* TableImport
   Copyright Ilwis System Development ITC
   may 1995, by Jelle Wind
	Last change:  WN   18 Nov 97   12:07 pm
*/

#define _USE_32BIT_TIME_T

#include <direct.h>
#include "Engine\DataExchange\Tblimp.h"
#include "Engine\Base\strtod.h"
#include "Engine\Table\Col.h"
#include "Engine\Table\COLSTORE.H"
#include "Engine\Applications\COLVIRT.H"
#include "Engine\Domain\dmsort.h"
#include "Engine\Domain\dmclass.h"
#include "Engine\Domain\Dmvalue.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Map\Raster\Map.h"
#include "Engine\Map\Segment\Seg.h"
#include "Engine\Map\Polygon\POL.H"
#include "Engine\Map\Point\PNT.H"

static short iFindNameCol(char* sHeader)
{
  short iCol = -1;
  { char *p = strtok(sHeader, " \t");
    while (p) {
      ++iCol;
      if (_strnicmp(p, "name$", 5)==0)
        return iCol;
      p = strtok(0, " \t");
    }
  }
  return -1;
}

void TableImport::GetImportInfo(const FileName& fn, bool& fNameCol)
{
  Array<String> as;
  Array<ValueRange> avr;
  GetImportInfo(fn, fNameCol, as, avr);
}

void TableImport::GetImportInfo(const FileName& fn, bool& fNameCol, Array<String>& asColNames, Array<ValueRange>& avr)
{
  File fil(fn);
//if (fil.err())
//  NotFoundError(fn);
  String sHeader;
  fil.ReadLnAscii(sHeader);
  char *p = strtok(sHeader.sVal(), " \t");
  String sColName;
  // create columns
	bool fHasX = false;
	ValueRange vrPrev;
  while (p) {
    if (*p == '#') // iOffset
      break;
    char c = p[strlen(p)-1]; // last character;
    bool fSkipLastChar = true;
		ValueRange vr;
    switch (c) {
      case '#':
        vr = ValueRange(0, 255);
        break;
      case '%':
        vr = ValueRange(-32766, 32767);
        break;
      case '!':
        vr = ValueRange(-LONG_MAX+1,LONG_MAX-1);
        break;
      case '&':
        vr = ValueRange(-1e38,1e38,0);
        break;
      case '$':
        vr = ValueRange();
        break;
      default :
        vr = ValueRange(-1e38,1e38,0);
        fSkipLastChar = false;
    }
    sColName = "";
    int iEnd = strlen(p)-1;
    if (fSkipLastChar)
      iEnd--;

    for (int i=0; i <= iEnd; i++) 
      sColName &= p[i];

		// delay adding X column; 
		// if the next column is the Y column then create a Coord column instead
		if (fCIStrEqual(sColName, "X"))
		{
			fHasX = true;
			vrPrev = vr; // remember 
		}
		else if (fCIStrEqual(sColName, "Y") && fHasX)
		{
			asColNames &= "Coordinate";
			avr &= ValueRange(); // add invalid valuerange
			fHasX = false;
		}
		else if (fHasX)
		{
			asColNames &= "X";  // delayed add of the X column
			avr &= vrPrev;
			asColNames &= sColName;
			avr &= vr;
			fHasX = false;
		}
		else
		{
			asColNames &= sColName;
			avr &= vr;
		}


    p = strtok(0, " \t");
  }

  fNameCol = false;
  for (int i=0; i < asColNames.iSize(); ++i)
    if (fCIStrEqual(asColNames[i], "Name")) {
      fNameCol = true;
      break;
    } 
}

void TableImport::import(const FileName& fnDat, const String& sTblExt, const FileName& fnOut)
{
  FileName fnNew = fnOut;
  if (fnOut.sFullPath() == "")
    fnNew = fnDat;
  bool fNameCol;
  Array<String> asColNames;
  Array<ValueRange> avr;
  TableImport::GetImportInfo(fnDat, fNameCol, asColNames, avr);
  Array<FileName> afnDmCol;
  for (int i=0; i < avr.iSize(); ++i) 
    if (!avr[i].fValid())
		{
			if (fCIStrEqual(asColNames[i], "Coordinate"))
				afnDmCol &= FileName("unknown", ".csy", true);
			else
				afnDmCol &= FileName("string", ".dom", true);
		}
    else
      afnDmCol &= FileName("value", ".dom", true);

  if (fNameCol)  // options: dmtID
    TableImport::import(fnDat, fnNew, sTblExt, dmtCLASS, FileName::fnUnique(FileName(fnDat, ".dom")), afnDmCol, avr);
  else
    TableImport::import(fnDat, fnNew, sTblExt, dmtNONE, FileName(), afnDmCol, avr);
/*  for (i=0; i < avr.iSize(); ++i) 
    if (0 != avr[i])
      delete avr[i];*/
}

void TableImport::import(const FileName& fnDat, const FileName& fnNew, const String& sTblExt, DomainType dmt,
                         const FileName& fnDom, const String& sDesc)
{                          
  Array<String> asColNames;
  Array<ValueRange> avr;
  bool fNameCol;
  TableImport::GetImportInfo(fnDat, fNameCol, asColNames, avr);
  Array<FileName> afnDmCol;
  for (int i=0; i < avr.iSize(); ++i) 
    if (!avr[i].fValid())
		{
			if (fCIStrEqual(asColNames[i], "Coordinate"))
				afnDmCol &= FileName("unknown", ".csy", true);
			else
				afnDmCol &= FileName("string", ".dom", true);
		}
    else
      afnDmCol &= FileName("value", ".dom", true);
  TableImport::import(fnDat, fnNew, sTblExt, dmt, fnDom, afnDmCol, avr);
/*  for (i=0; i < avr.iSize(); ++i) 
    if (0 != avr[i])
      delete avr[i];*/
} 

void TableImport::import(const FileName& fnDat, const FileName& fnNew, const String& sTblExt, DomainType dmt,
                         const FileName& fnDom, const Array<FileName>& afnDmCol, 
                         const Array<ValueRange>& avrCol,const String& sDesc)
{
	Domain dm;
	FileName fnTable(fnNew);
	fnTable.sExt = sTblExt;
	if (fnTable.sExt.length() == 0)
		fnTable.sExt = ".tbt";
	File fil(fnDat);
	String sHeader;
	fil.ReadLnAscii(sHeader);

	short iOffset=1;
	char *p =strrchr(sHeader.sVal(), '#');
	if (p) {
		p += 1;
		iOffset = atofILW(p);
		if (0 == iOffset) {
			if (*p != '0')
				iOffset = 1;
		}
	}
	else
		iOffset = 1;

	// count records
	long iRecs = -1;
	bool fEofIsLF;
	long iBufSize = 10000;
	{
		long j, iFileSize = fil.iSize();
		char *sBuf = new char[iBufSize + 1];
		fil.Seek(0);
		while (!fil.fEof())
		{
			long iRead = fil.Read(iBufSize, sBuf);
			for (j = 0; j < iRead; j++)
				if (sBuf[j] == '\n')
				{
					iRecs++;
					fEofIsLF = false;
				}
				else if (!fEofIsLF)
					fEofIsLF = true;
		}
		if (fEofIsLF)
			iRecs++;
		delete [] sBuf;  
	}
	bool fFillDomain = false;
	if (dmt == dmtNONE)
		dm = Domain("none");
	else if (File::fExist(fnDom)) // use it
		dm = Domain(fnDom);
	else {
		dm = Domain(fnDom, iRecs, dmt);
		fFillDomain = true;
	}
	FileName fnNewDat = fnTable;
	if (fnNewDat.sExt == ".mpp")
		fnNewDat.sExt = ".pt#";
	else
		fnNewDat.sExt[3] = '#';
	Table tbl;
	tbl.SetPointer(new TableImport(fnTable, fnNewDat, fnDat, dm, iRecs, iOffset, fFillDomain, afnDmCol, avrCol));
	tbl->sDescription = sDesc;
	tbl->Store();
}

TableImport::TableImport(const FileName& fn, const FileName& fnNewDat, const FileName& fnOldDat,
                         const Domain& dm, long iRecs, long iOffset, bool fFillDomain, 
                         const Array<FileName>& afnDmCol, const Array<ValueRange>& avrCol)
 : TablePtr(fn, fnNewDat, dm, String())
{
//	objtime = Time(fnNewDat);
	bool fDomNone = dm->pdnone() != 0;
	if (fDomNone) {
		_iOffset = iOffset;
		iRecNew(iRecs);
	}
	File fil(fnOldDat, facRO);
	String sHeader;
	fil.ReadLnAscii(sHeader);
	int iNameCol = shUNDEF;
	if (!fDomNone)
		iNameCol = iFindNameCol(sHeader.sVal());
	fil.Seek(0);
	fil.ReadLnAscii(sHeader);
	char *p = strtok(sHeader.sVal(), " \t");
	_iCols = 0;

	String sColName;
	Array<int> iColInd;
	int iCol=0;
	int iCoordCol = -1;
	bool fHasX = false;
	{
		Table t;
		t.SetPointer(this);
		iRef++; // to prevent deletion at end of block
		
		// create columns
		while (p)
		{
			if (*p == '#') // iOffset
				break;
			char c = p[strlen(p)-1]; // last character;
			sColName = "";
			int iEnd = strlen(p)-1;
			bool fSkipLastChar = (c=='#') || (c=='%') || (c=='!') || (c=='&') || (c=='$');
			if (fSkipLastChar)
				iEnd--;
			for (int i=0; i <= iEnd; i++) 
				sColName &= p[i];
			
			int iRepeat = 1;
			String sNextCol = sColName;
			// delay adding X column; 
			// if the next column is the Y column then create a Coord column instead
			if (fCIStrEqual(sColName, "X"))
			{
				fHasX = true;
				iRepeat = 0;
			}
			else if (fCIStrEqual(sColName, "Y") && fHasX)
			{
				sColName = "Coordinate";  // combine X and Y column in Coord column
				fHasX = false;
				iRepeat = 1;
			}
			else if (fHasX)
			{
				iRepeat = 2; // do both the X column and the next, which is not Y
				fHasX = false;
				sColName = "X"; // X column first, sNextCol contains name of next column to add
			}
			
			while (iRepeat > 0)
			{
				Column col;
				if (afnDmCol[iCol].fValid())
				{
					try
					{
						Domain dm(afnDmCol[iCol]);
						if (fCIStrEqual(afnDmCol[iCol].sFile, "unknown"))  // CoordCol will have CoordSys Unknown as domain
							iCoordCol = iCol;
						DomainValueRangeStruct dvs(dm, avrCol[iCol]);
						if (!t->col(sColName).fValid())
						{
							col = Column(t, sColName, dvs);
							iColInd &= iCols()-1;
							
							// do not include Name column, because it is already
							// used to create the domain
							if (iCol == iNameCol)
								pts->ac[iCol]->fErase = true;
						}
						else
							iColInd &= shUNDEF;  
					}
					catch (const ErrorObject& err)
					{
						err.Show();
						iColInd &= shUNDEF;
					}
				}
				else  
					iColInd &= shUNDEF;
				
				iCol++;
				iRepeat--;
				sColName = sNextCol;
			}
			p = strtok(0, " \t");
		}
	}  
	iRef--;  // see iRef++
	
	DomainSort *dsrt = dm->pdsrt();
	if (fFillDomain)
		dsrt->dsType = DomainSort::dsMANUAL;
	char *sBuffer = new char[10001];
	// fill columns
	char *sBuf, *str;
	long c;
	for (long r = 0; r < iRecs; r++)
	{
		long rec = r + iOffset;
		sBuf = sBuffer;
		bool fEOL = !fil.ReadLnAscii(sBuf, 10000);
		for (c = 0; c < iColInd.iSize() /* iCols()*/; c++)
		{
			int iRepeats = iCoordCol == c ? 2 : 1;
			// skip spaces
			while (!fEOL && isspace((unsigned char)*sBuf))
				sBuf++;
			str = sBuf;
			while (!fEOL && iRepeats > 0) 
			{
				// skip spaces
				while (isspace((unsigned char)*sBuf))
					sBuf++;
				// find non space
				while (*sBuf && !isspace((unsigned char)*sBuf))
					sBuf++;
				iRepeats--;
				if (iRepeats == 0)
					*sBuf++ = '\0'; 
			}
			if (iColInd[c] == shUNDEF)
				continue;
			
			if (!pts->ac[iColInd[c]]->dvrs().fValues() && c != iCoordCol)
				for (int i=0; i < strlen(str); i++)
					if (str[i] == '_') str[i] = ' ';
					
			if (c == iCoordCol)
				pts->ac[iColInd[c]]->PutVal(rec, str);
			else if (c != iNameCol) 
			{    // skip Name column (already used in domain)
				if (atoi(str) == -32767) // this is otherwise converted to -32766
					pts->ac[iColInd[c]]->PutVal(rec, iUNDEF);
				else if (atofILW(str) == -1e38) // 1.4 undef 
					pts->ac[iColInd[c]]->PutVal(rec, rUNDEF);
				else
					pts->ac[iColInd[c]]->PutVal(rec, str);
			}  
			else if (fFillDomain) {
				rec = r + iOffset;
				dsrt->SetVal(rec, str);
			}
			else {
				rec = dsrt->iRaw(str);
			}  
		} // for c
	} // for r
	delete [] sBuffer;
	for (c = 0; c < iCols(); c++)
		pts->ac[c]->CalcMinMax();
	fChanged = true;
	if (fnObj.sExt != ".tbt") {
		fUpdateCatalog = false;
		return;
	}
	if (fDomNone)
		return;
	if (fnObj.sExt == ".$$$")
		return;
	if (dm->pdsrt() != 0)  // table can't be attribute table of map
		return;
	
	// set attr table if maps have same domain as table
	Domain dmMap;
	FileName fnMap = FileName(fn, ".mpr");
	if (File::fExist(fnMap)) {
		dmMap = Map(fnMap)->dm();
		if (dm == dmMap)
			ObjectInfo::WriteElement("BaseMap", "AttributeTable", fnMap, sName(true, fnMap.sPath()));
	}
	
	fnMap.sExt = ".mps";
	if (File::fExist(fnMap)) {
		dmMap = SegmentMap(fnMap)->dm();
		if (dm == dmMap)
			ObjectInfo::WriteElement("BaseMap", "AttributeTable", fnMap, sName(true, fnMap.sPath()));
	}
	
	fnMap.sExt = ".mpa";
	if (File::fExist(fnMap)) {
		dmMap = PolygonMap(fnMap)->dm();
		if (dm == dmMap)
			ObjectInfo::WriteElement("BaseMap", "AttributeTable", fnMap, sName(true, fnMap.sPath()));
	}
	
	fnMap.sExt = ".mpp";
	if (File::fExist(fnMap)) {
		dmMap = PointMap(fnMap)->dm();
		if (dm == dmMap)
			ObjectInfo::WriteElement("BaseMap", "AttributeTable", fnMap, sName(true, fnMap.sPath()));
	}
	
}
