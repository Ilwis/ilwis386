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
/* $Log: /ILWIS 3.0/Import_Export/EXPE00.cpp $
 * 
 * 10    14-01-03 10:06 Willem
 * - Added: tranquilizer message when the attribute table is loaded.
 * (Especially loading tables with a large number of columns takes quite a
 * lot of time).
 * 
 * 9     20-02-02 12:24 Willem
 * - The check to count valid polygons now matches the check in the loop
 * to write polygons, preventing more attributes than polygon label points
 * (with a corrupt e00 file as a result)
 * - replaced "%S%*S" by "%S%*s" (small s) in string constructors where an
 * empty string was output. The string constructor cannot handle empty
 * strings in this way.
 * - Added extra check for passing array bounds in segment outoput
 * 
 * 8     6/15/01 11:53 Willem
 * In case of an odd number of segment coordinates the export falsely
 * added an invalid last coordinate at the end to make the number of
 * coordinates even. The export now only writes the actual number of
 * coordinates (be it even or not)
 * 
 * 7     6/12/01 14:29 Willem
 * String constructor String("%S%*S", sNam, ...) could not handle the case
 * where sNam is a NULL pointer. This is now handled separately.
 * 
 * 6     16/03/00 16:08 Willem
 * Replaced the obsolete trq.Message() function with Show() function
 * 
 * 5     3/03/00 18:15 Willem
 * Added a function to be able to format floating point numbers
 * according to the requirements of E00 format;
 * both sprintf() and the String class could not provide the correct
 * formatting
 * 
 * 4     10-01-00 4:05p Martin
 * removed vector rowcol and changed them to true coords
 * 
 * 3     17-06-99 3:59p Martin
 * // -> /*
 * 
 * 2     17-06-99 2:09p Martin
 * ported files to VS
// Revision 1.10  1998/09/17 09:13:06  Wim
// 22beta2
//
// Revision 1.9  1998/06/10 13:11:32  Willem
// Attribute with double precision are now exported correctly.
//
// Revision 1.8  1998/06/05 14:10:05  Willem
// Formatting of negative number added an extra space, causing a wrong alignment
// in the exported E00 file. All numbers will now get correct alignment.
//
// Revision 1.7  1997/12/08 14:37:50  Dick
// repair the problem with sValue the width parameter
//
// Revision 1.6  1997/11/26 18:16:45  Dick
// *** empty log message ***
//
// Revision 1.5  1997/09/30 13:22:30  Willem
// Solved a problem  with writing floating point values to E00 file: E00 expects floating
// point values to have a '.' character, if it is not there values are considered zero!
//
// Revision 1.4  1997/09/29 21:47:10  Willem
// Changed output of map->sValue() to accomodate the asked width. The map (sm, pnt) etc
// ignore the width (and maybe decimals) values passed as parameters.
//
// Revision 1.3  1997/08/08 19:21:31  Willem
// The TIC section of the INFO file was written incorrectly (recsize cumulative was wrong)
//
// Revision 1.2  1997/08/07 19:40:30  Willem
// Polygons are now exported only if their area is larger than zero.
//
/*
  Expe00.c
  Export to ArcInfo .E00
  by Willem Nieuwenhuis 15 oct 1996
  (c) Ilwis System Development ITC
  Last change:  DV    8 Dec 97    2:37 pm
*/
#include "Headers\toolspch.h"
#include "Engine\DataExchange\EXPE00.H"
#include "Engine\Domain\dmsort.h"

/*
  Revision history:
  January 97: Export Ilwis segments only, no codes
*/

E00::E00(const FileName& fnFile, const FileName& fnObject, Tranquilizer& _trq)
     : trq(_trq) {
  iCurCoverage = 1;
  fnIlwis = fnObject;
  fnE00 = fnFile;
  filE00 = new File(fnE00, facCRT);
  rcb.Size(1000);
  cbuf.Size(1000);
}

E00::~E00() {
  if (filE00) delete filE00;
}

// number are formatted correctly in the range -9e-99 upto 9e+99
// number are formatted as:
//  sm.mmmmmmmEqpp
// with:
//  s   sign ('-', ' ')
//  m   digit
//  q   sign of exponent ('-', '+')
//  p   exponent digit
// iPrecision is the number of digits behind the decimal point ('.')
void E00::DoubleConvert(const double& rVal, int iPrecision, char *pcOut)
{
	int iDecPos;
	int iSign;

	char *pcRes = pcOut;
	char *p = ecvt(rVal, iPrecision + 1, &iDecPos, &iSign);
	char *pcBuf = p;

	*pcRes++ = (iSign == 0) ? ' ' : '-';
	*pcRes++ = *p++;  // first digit
	*pcRes++ = '.'; // cDecimalPoint(); so not language independent!

	while (iPrecision-- > 0)
		*pcRes++ = *p++;  // add iPrecision digits after '.'

	// Add exponent
	if (*pcBuf != '0')
		iDecPos--;  // Avoid 0.00e-01
	*pcRes++ = 'E';
	*pcRes++ = (iDecPos < 0) ? ((iDecPos = -iDecPos),'-') : '+';
	*pcRes++ = iDecPos / 10 + '0';
	*pcRes++ = iDecPos % 10 + '0';
	*pcRes = '\0';
}

String E00::DoubleToStr(const double& rVal, bool fDoublePrec)
{
	char pc[30];

	int iPrec = fDoublePrec ? 17 : 7;
	DoubleConvert(rVal, iPrec, pc);

	return String(pc);
}

String E00::CoordToStr(const Coord& crd, bool fDoublePrec)
{
	char pc[60];

	int iPrec = fDoublePrec ? 17 : 7;
	DoubleConvert(crd.x, iPrec, pc);
	DoubleConvert(crd.y, iPrec, pc + strlen(pc));

	return String(pc);
}

String E00::sBuildCoords(Coord crd1, Coord crd2) 
{
	String s("%S%S", CoordToStr(crd1, false), CoordToStr(crd2, false));

  return s;
}

/*
  Build the full filename, place it in the header line and write the header;
  backslashes ('\') are replaced by a slash ('/')
*/
void E00::PutName() {
  String sName = fnE00.sFullName(true);
  for (short i = 0; i < sName.length(); i++) {
    char c = sName[i];
    sName[i] = (c >= 'a') && (c <= 'z') ? c - 32 : c;
    if (c == '\\') sName[i] = '/';
  }
  String s = String("EXP  0 %S", sName.sRight(sName.length() - 2));
  filE00->WriteLnAscii(s);
}

void E00::StartArcFile() {
  filE00->WriteLnAscii("ARC  2");   // 3: use double precision or 2: single precision
}

void E00::EndFileMarker() {
  String s = String(SCVFormatArcStart, -1L, 0L, 0L, 0L, 0L, 0L, 0L);  // end of segmentmap
  filE00->WriteLnAscii(s);
}

/*
  Write a segment to the .E00 file
  Arc/Info segments have a limit of 500 points, while Ilwis can store upto 1000 points.
  Longer segments will be split up into a maximum of three arcs (remember that splitting a
  segment creates a new node, an extra point!).
  Internal codes for Arc/Info are generated:
      #coords           Arc/Info arc ID                 Internal Arc/Info code
     2 - 400:         Ilwis internal segment number    3 * Ilwis sequential#
     401 - 800:       Ilwis internal segment number    3 * Ilwis sequential# + 1
     801 - 1000:      Ilwis internal segment number    3 * Ilwis sequential# + 2
     where Ilwis internal number: 1 - 1000
           Ilwis sequential# is n-th segment in the Ilwis file
*/
void E00::PutSegment(ILWIS::Segment* seg) {
  CoordinateSequence *seq = seg->getCoordinates();
  String s;
  long iRest = seq->size();
  long iToDo;
  short iPart = 0;
  long iOff = 0;
  while (iRest > 0) {
    if (iRest < 400 + 10)     // '+ 10' to be certain last segment part has a least 10 points
      iToDo = iRest;
    else
      iToDo = 400;
    s = String(SCVFormatArcStart, iCurCoverage + iPart, iPart,
                                  0L, 0L, 0L, 0L, iToDo);
    aiSegIndex &= iPart;
    aiArcIndex &= iCurCoverage + iPart;
    iTotalArcs++;
    filE00->WriteLnAscii(s);
    iPart++;
    long iLast = 2 * (iToDo / 2);
    for (long i = 0; i < iLast; i++, i++)   // the loop will write an even #coords
      filE00->WriteLnAscii(sBuildCoords(seq->getAt(i + iOff), seq->getAt(i + iOff + 1)));
// take care of duplicating end node when segment needs to be split
    iOff += iToDo - 1;
    iRest -= iToDo;
    if (iRest > 0) iRest++;  // '++' because intermediate node needs to be done twice
  }
// if iToDo is odd
//   the last single coord needs to be written as well, but on a separate line
  if (0 != iToDo % 2)
	  filE00->WriteLnAscii(CoordToStr(seq->getAt(seq->size() - 1), false));
  iCurCoverage += 3;
  delete seq;
}

void E00::PutPolygonLeader() {
  filE00->WriteLnAscii("PAL  2");
}

void E00::PutSpacial() {
  filE00->WriteLnAscii("SIN  2");
  filE00->WriteLnAscii("EOX");
}

void E00::PutHistory() {
  time_t timer;
  timer = time(NULL);
  tm* ptm = localtime(&timer);
  filE00->WriteLnAscii("LOG  2");
  String s(SCVFormatHistory, 1900 + ptm->tm_year, 1 + ptm->tm_mon, ptm->tm_mday,
                             ptm->tm_hour, ptm->tm_min, 0, 0, 0,
                             "Ilwis export");
  filE00->WriteLnAscii(s);
  filE00->WriteLnAscii("~");
  filE00->WriteLnAscii("EOL");
}

String E00::sBuildTick(short iTick, Coord crd) {
  String s = String("%11i%S", iTick, CoordToStr(crd, false));
  return s;
}

void E00::WriteBounds(CoordBounds& cb) {
  char *sMinMax[] = { "XMIN", "YMIN", "XMAX", "YMAX" };
// write boundary "BND file", required
  String s = String(SCVFormatBoundary, fnE00.sFile.toUpper(), 28 - fnE00.sFile.length(), ' ');
  filE00->WriteLnAscii(s);
  short i;
  for (i = 0; i < 4; i++) {
    s = String(SCVFormatMinMax, sMinMax[i], 1 + i * 4, i + 1);
    filE00->WriteLnAscii(s);
  }
  filE00->WriteLnAscii(sBuildCoords(cb.cMin, cb.cMax));
// end of boundary "BND file"
}

void E00::WriteTicks(CoordBounds& cb) {
// write "TIC" file, required; first version ticks are equal to boundary points
  char *sTickName[] = { "IDTIC", "XTIC ", "YTIC " };     // length of strings = 5
  char *sTickType[] = { "5-1 50-1", "12 3 60-1", "12 3 60-1" };
  String s = String(SCVFormatTicks, fnE00.sFile.toUpper(), 28 - fnE00.sFile.length(), ' ');
  s &= String("%10i", 4);    // fixed to four ticks
  filE00->WriteLnAscii(s);
  for (short i = 0; i < 3; i++) {
    s = String(SCVFormatTickDef, sTickName[i], 1 + i * 4, sTickType[i], i + 1);
    filE00->WriteLnAscii(s);
  }
// write four tick marks, one for each boundary corner
  filE00->WriteLnAscii(sBuildTick(0, cb.cMin));
  filE00->WriteLnAscii(sBuildTick(1, Coord(cb.cMin.x, cb.cMax.y)));
  filE00->WriteLnAscii(sBuildTick(2, Coord(cb.cMax.x, cb.cMin.y)));
  filE00->WriteLnAscii(sBuildTick(3, cb.cMax));
// end of boundary file
}

ColInfo E00::ciColDetails(const DomainValueRangeStruct& dvs) {
  ColInfo ci;
  ci.iNum = 0;
  ci.iForm = 0;
  ci.iDec = 0;
  ci.iType = 2;    // 1 = date, 2 = string, 3 = byte?, 4 = ?, 5 = integer, 6 = real
  ci.iStrSize = 30;
  Domain dm = dvs.dm();
  if (dm->pdbool() != NULL) {   // check here because DomainBool is also DomainValue!
    ci.iType = 2;
    ci.iNum = 30;
    ci.iForm = 30;
  }
  else if (dvs.fValues()) {
    ci.iStrSize = 4;
    ci.iType = 5;
    ci.iForm = 11;
    if (dvs.fRealValues()) {
      ci.iStrSize = 8;
      ci.iType = 6;
      ci.iForm = 24;
      ci.iNum = 16;
      ci.iDec = 3;
    }
    else {
      ci.iNum = 5;
      ci.iForm = 11;
    }
  }
  else {
    ci.iType = 2;
    ci.iNum = 30;
    ci.iForm = 30;
  }
  if (ci.iDec == 0)
    ci.iDec = -1;
  return ci;
}

void E00::GetAttributeInfo(Table& tblAtt, Array<ColInfo>& aciCols, short& iRecSize) {
  long iCols = tblAtt->iCols();
  for (short i = 0; i < iCols; i++) {
    Column col = tblAtt->col(i);
    DomainValueRangeStruct dvs = col->dvrs();
    ColInfo ci = ciColDetails(dvs);
    iRecSize += ci.iStrSize;
    ci.sName = col->sName();
    aciCols &= ci;
  }
}

void E00::WritePATFieldInfo(Array<ColInfo>& aciCols, long iNrAttr, short iRecSize)
{
	short iFields = aciCols.iSize() + 4;    // 4 point fields are required
	String s = String(SCVFormatPolAttr,     // %S %*c %i %i %i %li
		fnE00.sFile.toUpper(), 28 - fnE00.sFile.length(), ' ',
		iFields, iFields, iRecSize, iNrAttr);
	filE00->WriteLnAscii(s);
	
	filE00->WriteLnAscii("AREA              4-1   14-1  12 3 60-1  -1  -1-1                   1");
	filE00->WriteLnAscii("PERIMETER         4-1   54-1  12 3 60-1  -1  -1-1                   2");
	s = fnE00.sFile.toUpper();
	s &= "#";
	filE00->WriteLnAscii(String("%-18S4-1   94-1   5-1 50-1  -1  -1-1                   3", s));
	s = fnE00.sFile.toUpper();
	s &= "-ID";
	filE00->WriteLnAscii(String("%-18S4-1  134-1   5-1 50-1  -1  -1-1                   4", s));
	iRecSize = 17;
	for (long i = 0; i < aciCols.iSize(); i++)
	{
		ColInfo& ciLoc = aciCols[i];
		String sField = String("%-17S%2i-1%4i4-1%4i%2i%2i0-1  -1  -1-1%20i",
			ciLoc.sName,
			ciLoc.iStrSize,
			iRecSize,
			ciLoc.iNum, ciLoc.iDec, ciLoc.iType,
			i + 5);
		if (ciLoc.iDec < 0)
			ciLoc.iDec = 0;
		filE00->WriteLnAscii(sField);
		iRecSize += ciLoc.iStrSize;
	}
}

void E00::WriteArcFieldInfo(Array<ColInfo>& aciCols, long iNrAttr, short iRecSize)
{
	short iFields = aciCols.iSize() + 7;    // 4 point fields are required
	String s = String(SCVFormatSegAttr,     // %S %*c %i %i %i %li
		fnE00.sFile.toUpper(), 28 - fnE00.sFile.length(), ' ',
		iFields, iFields, iRecSize, iNrAttr);
	filE00->WriteLnAscii(s);
	
	filE00->WriteLnAscii("FNODE#            4-1   14-1   5-1 50-1  -1  -1-1                   1");
	filE00->WriteLnAscii("TNODE#            4-1   54-1   5-1 50-1  -1  -1-1                   2");
	filE00->WriteLnAscii("LPOLY#            4-1   94-1   5-1 50-1  -1  -1-1                   3");
	filE00->WriteLnAscii("RPOLY#            4-1  134-1   5-1 50-1  -1  -1-1                   4");
	filE00->WriteLnAscii("LENGTH            4-1  174-1  12 3 60-1  -1  -1-1                   5");
	s = fnE00.sFile.toUpper();
	s &= "#";
	filE00->WriteLnAscii(String("%-18S4-1  214-1   5-1 50-1  -1  -1-1                   6", s));
	s = fnE00.sFile.toUpper();
	s &= "-ID";
	filE00->WriteLnAscii(String("%-18S4-1  254-1   5-1 50-1  -1  -1-1                   7", s));
	iRecSize = 29;
	for (long i = 0; i < aciCols.iSize(); i++)
	{
		ColInfo& ciLoc = aciCols[i];
		String sField = String("%-17S%2i-1%4i4-1%4i%2i%2i0-1  -1  -1-1%20i",
			ciLoc.sName,
			ciLoc.iStrSize,
			iRecSize,
			ciLoc.iNum, ciLoc.iDec, ciLoc.iType,
			i + 8);
		if (ciLoc.iDec < 0) ciLoc.iDec = 0;
		filE00->WriteLnAscii(sField);
		iRecSize += ciLoc.iStrSize;
	}
}

// Built a string for the undefined value for all column types
// taking into account the number of characters the string
// should contain (space filled)
String E00::sBuildUndef(ColInfo& ciCol, long iKey, Column& col)
{
	DomainValueRangeStruct dvs = col->dvrs();
	if (!col->fValues())
		return String("%s%*s", "?", ciCol.iForm - 1, "");  // string representation
	
	// handle values
	if (ciCol.iType == 6)  // double
		return SCVUndefFloat;        // -MAXFLOAT: in double precision
	else // (aciCols[iCol].iType == 5)
		return SCVUndefLong;         // -MAXLONG
}

String E00::sBuildValue(ColInfo& ciCol, long iKey, Column& col)
{
	if (ciCol.iType == 2)  // string column
	{
		String sNam = col->sValue(iKey, 0);
		// Avoid having a NULL pointer mess up the return value
		if (sNam.length() == 0)
			return String(' ', ciCol.iForm);
		else
			return String("%S%*s", sNam, ciCol.iForm - sNam.length(), "");    // col->sValue does NOT take into account width!
	}
	else if (ciCol.iType == 6)  // 6 == real, moeilijk, moeilijk!
	{
		double rVal = col->rValue(iKey);
		return DoubleToStr(rVal, true);    // always double precision
	}
	else  // ciCol.iType == 5
		return String("%*li", ciCol.iForm, col->iValue(iKey));
}

void E00::WriteAAT(SegmentMap& sm)
{
	Array<ColInfo> aciCols;
	DomainValueRangeStruct dvs = sm->dvrs();
	ColInfo ci = ciColDetails(dvs);
	ci.sName = "DOMAIN";
	aciCols &= ci;       // Always add domain of segment map
	short iRecSize = 28 + ci.iStrSize;
	Table tblAtt;
	DomainSort* pds = NULL;
	if (sm->fTblAtt())
	{
		tblAtt = sm->tblAtt();
		pds = tblAtt->dm()->pdsrt();
	}
	long iSegs = sm->iFeatures();
	// only allow DomainSort for the Table
	bool fAttrib = tblAtt.fValid() && (pds != NULL);
	if (fAttrib)
		GetAttributeInfo(tblAtt, aciCols, iRecSize);
	WriteArcFieldInfo(aciCols, iTotalArcs, iRecSize);
	
	trq.SetText(SCVTextWriteAttributes);
	
	long iCurSeg = 0;
	iCurCoverage = 1;
	 for (int i=0 ; i < sm->iFeatures(); ++i) {
		  ILWIS::Segment *seg = (ILWIS::Segment *) sm->getFeature(i);
		  if ( seg == NULL || !seg->fValid())
			  continue;
		if (trq.fUpdate(iCurSeg, iSegs))
			return;
		long iLoop = aiSegIndex[iCurSeg];
		while (iCurSeg < aiSegIndex.size() && iLoop == aiSegIndex[iCurSeg]) // Assignment to iLoop is intentional
		{
			String sSegVal = String("% 11i% 11i% 11i% 11i%S% 11li% 11li", 0, 0, 0, 0, DoubleToStr(seg->rLength(), false), aiArcIndex[iCurSeg], iLoop);
			Coord crd = seg->crdBegin();
			String sNam = seg->sValue(sm->dvrs());
			if (aciCols[0].iType == 2)   // 2 == string
				sSegVal &= String("%S%*s", sNam, aciCols[0].iForm - sNam.length(), "");    // sm->sValue does NOT take into account width!
			else if (aciCols[0].iType == 6)   // 6 == real, moeilijk, moeilijk!
			{
				double rVal = seg->rValue();
				sSegVal &= DoubleToStr(rVal, true);
			}
			else  // aciCols[].iType == 5
				sSegVal &= String("%*li", aciCols[0].iForm, seg->iValue());    // sm->sValue does NOT take into account width!
			
			if (fAttrib)
			{
				long iKey = pds->iKey(pds->iOrd(seg->iValue()));
				for (short iCol = 1; iCol < aciCols.iSize(); iCol++)
				{
					if (trq.fUpdate(iCurSeg, iSegs))    // let the system respond more smoothly
						return;
					Column col = tblAtt->col(iCol - 1);
					if (col->sValue(iKey, 0) != sUNDEF)
						sSegVal &= sBuildValue(aciCols[iCol], iKey, col);
					else   // value is the 'undefined' value
						sSegVal &= sBuildUndef(aciCols[iCol], iKey, col);
				}
			}
			iCurSeg++;
			short iLen = sSegVal.length();
			short iLeft = 0;
			while (iLen > 0)
			{
				String sTemp = sSegVal.sSub(iLeft, 80);
				filE00->WriteLnAscii(sTemp);
				iLeft += 80;
				iLen -= 80;
			}
		}
	}
}

void E00::WritePAT(PointMap& pnt)
{
	Array<ColInfo> aciCols;
	DomainValueRangeStruct dvs = pnt->dvrs();
	ColInfo ci = ciColDetails(dvs);
	ci.sName = "DOMAIN";
	aciCols &= ci;       // Always add domain of segment map
	short iRecSize = 16 + ci.iStrSize;
	Table tblAtt;
	DomainSort* pds = NULL;
	if (pnt->fTblAtt())
	{
		tblAtt = pnt->tblAtt();
		pds = tblAtt->dm()->pdsrt();
	}
	long iPnts = pnt->iFeatures();
	// only allow DomainSort for the Table
	bool fAttrib = tblAtt.fValid() && (pds != NULL);
	if (fAttrib)
		GetAttributeInfo(tblAtt, aciCols, iRecSize);
	WritePATFieldInfo(aciCols, iPnts, iRecSize);
	
	trq.SetText(SCVTextWriteAttributes);
	
	for (long iCurPnt = 1; iCurPnt <= iPnts; ++iCurPnt)
	{
		if (trq.fUpdate(iCurPnt, iPnts))
			return;
		
		String sPntVal = String("%S%S% 11li% 11li", DoubleToStr(0.0, false), DoubleToStr(0.0, false), iCurPnt, iCurPnt);
		String sNam = pnt->sValue(iCurPnt, aciCols[0].iForm, aciCols[0].iDec);
		if (aciCols[0].iType == 2)   // 2 == string
			sPntVal &= String("%S%*s", sNam, aciCols[0].iForm - sNam.length(), "");
		else if (aciCols[0].iType == 6)  // 6 == real, moeilijk, moeilijk!
		{
			double rVal = pnt->rValue(iCurPnt);
			sPntVal &= DoubleToStr(rVal, true);;
		}
		else  // aciCols[].iType == 5
			sPntVal &= String("%*li", aciCols[0].iForm, pnt->iValue(iCurPnt));    // pnt->sValue does NOT take into account width!
		
		if (fAttrib)
		{
			long iKey = pds->iKey(pds->iOrd(pnt->iRaw(iCurPnt)));
			for (short iCol = 1; iCol < aciCols.iSize(); iCol++)
			{
				if (trq.fUpdate(iCurPnt, iPnts))    // let the system respond more smoothly
					return;
				Column col = tblAtt->col(iCol - 1);
				if (col->sValue(iKey, 0) != sUNDEF)
					sPntVal &= sBuildValue(aciCols[iCol], iKey, col);
				else   // value is the 'undefined' value
					sPntVal &= sBuildUndef(aciCols[iCol], iKey, col);
			}
		}
		short iLen = sPntVal.length();
		short iLeft = 0;
		while (iLen > 0)
		{
			String sTemp = sPntVal.sSub(iLeft, 80);
			filE00->WriteLnAscii(sTemp);
			iLeft += 80;
			iLen -= 80;
		}
	}
}

void E00::WritePAT(PolygonMap& pm)
{
	Array<ColInfo> aciCols;
	DomainValueRangeStruct dvs = pm->dvrs();
	ColInfo ci = ciColDetails(dvs);
	ci.sName = "DOMAIN";
	aciCols &= ci;       // Always add domain of segment map
	short iRecSize = 16 + ci.iStrSize;
	Table tblAtt;
	DomainSort* pds = NULL;

	if (pm->fTblAtt())
	{
		trq.fText(SCVTextOpeningAttributes);
		tblAtt = pm->tblAtt();
		pds = tblAtt->dm()->pdsrt();
	}
	long iPols = 0;
	 for (int i=0 ; i < pm->iFeatures(); ++i) {
		  ILWIS::Polygon *pol = (ILWIS::Polygon *) pm->getFeature(i);
		  if ( pol == NULL || !pol->fValid())
			  continue;
		  if ( pol->rArea() > 0)
			  ++iPols;
	 }
	// only allow DomainSort for the Table
	bool fAttrib = tblAtt.fValid() && (pds != NULL);
	if (fAttrib)
		GetAttributeInfo(tblAtt, aciCols, iRecSize);
	WritePATFieldInfo(aciCols, iPols, iRecSize);
	
	trq.SetText(SCVTextWriteAttributes);
	
	long iCurPol = 0;
	for (int i=0 ; i < pm->iFeatures(); ++i) {
		ILWIS::Polygon *pol = (ILWIS::Polygon *) pm->getFeature(i);
		if ( pol == NULL || !pol->fValid())
			continue;
		if (trq.fUpdate(iCurPol, iPols))
			return;
		if (!(pol->rArea() > 0))  // Make sure to use the same check as used above to calculate the number of valid polygons
			continue;
		iCurPol++;

		String sPolVal = String("%S%S% 11li% 11li", DoubleToStr(0.0, false), DoubleToStr(0.0, false), iCurPol, iCurPol);

		String sNam = pol->sValue(pm->dvrs());
		String sEmpty;
		if (aciCols[0].iType == 2)   // 2 == string
			sPolVal &= String("%S%*S", sNam, aciCols[0].iForm - sNam.length(), sEmpty);    // pm->sValue does NOT take into account width!
		else if (aciCols[0].iType == 6)  // 6 == real, moeilijk, moeilijk!
		{
			double rVal = pol->rValue();
			sPolVal &= DoubleToStr(rVal, true);
		}
		else  // aciCols[].iType == 5
			sPolVal &= String("%*li", aciCols[0].iForm, pol->iValue());

		if (fAttrib) {
			long iKey = pol->iValue();
			for (short iCol = 1; iCol < aciCols.iSize(); iCol++)
			{
				Column col = tblAtt->col(iCol - 1);
				if (col->sValue(iKey, 0) != sUNDEF)
					sPolVal &= sBuildValue(aciCols[iCol], iKey, col);
				else   // value is the 'undefined' value
					sPolVal &= sBuildUndef(aciCols[iCol], iKey, col);
			}
		}
		short iLen = sPolVal.length();
		short iLeft = 0;
		while (iLen > 0)
		{
			String sTemp = sPolVal.sSub(iLeft, 80);
			filE00->WriteLnAscii(sTemp);
			iLeft += 80;
			iLen -= 80;
		}
	}
}

/*
  De Arc/Info files are written in alphabetical order:
  .AAT      // if available, only for segment maps
  .BND      // always
  .PAT      // if available, for points or polygon maps
  .TIC      // Tick marks (Tiepoints?)
*/
void E00::PutInfo(BaseMap& mp) {
  filE00->WriteLnAscii("IFO  2");    // start of info section
  SegmentMapPtr *psm = dynamic_cast<SegmentMapPtr *>(mp.ptr());
  if (psm != NULL) {
    SegmentMap sm;
    sm.SetPointer(psm);
    WriteAAT(sm);
  }
  CoordBounds cb = mp->cb();
  WriteBounds(cb);
  if (psm == NULL) {
    PointMapPtr *pptm = dynamic_cast<PointMapPtr *>(mp.ptr());
    if (pptm != NULL) {
      PointMap pnt;
      pnt.SetPointer(pptm);
      WritePAT(pnt);
    }
    else {
      PolygonMapPtr *ppm = dynamic_cast<PolygonMapPtr *>(mp.ptr());
      if (ppm != NULL) {
        PolygonMap pm;
        pm.SetPointer(ppm);
        WritePAT(pm);
      }
    }
  }
  WriteTicks(cb);
  filE00->WriteLnAscii("EOI");       // end of info section
}

void E00::WriteTrailer() {
  filE00->WriteLnAscii("EOS");
}

bool E00::DoSegments() {
  SegmentMap sm(fnIlwis);
  dvs = DomainValueRangeStruct(sm->dm());
  long iSegCnt = 0, iNrSegs = sm->iFeatures();
  if (iNrSegs <= 0)
    return false;
  PutName();
  StartArcFile();
  ILWIS::Segment *seg = sm->segFirst();
  trq.SetText(SCVTextWriteSegments);
  iTotalArcs = 0;
  while (seg && seg->fValid()) {
    PutSegment(seg);
    iSegCnt++;
    if (trq.fUpdate(iSegCnt, iNrSegs))
      return false;
	seg = (ILWIS::Segment *)sm->getFeature(iSegCnt);
  }
  EndFileMarker();
  PutSpacial();
  PutHistory();
  PutInfo(sm);
  WriteTrailer();
  trq.fUpdate(iNrSegs, iNrSegs);
  return true;
}

void E00::StartLabFile() {
  filE00->WriteLnAscii("LAB 2");
}

void E00::EndLabFileMarker() {
  String s("%10i%10i%S%S", -1, 0, DoubleToStr(0.0, false), DoubleToStr(0.0, false));
  filE00->WriteLnAscii(s);
}

void E00::WriteLabel(const Coord& crd, long iPolNr) {
	String sCrd = CoordToStr(crd, false);
  String s("%10li%10li%S", iPolNr, 0L, sCrd);
  filE00->WriteLnAscii(s);
  filE00->WriteLnAscii(sBuildCoords(crd, crd));
}

bool E00::DoPolygons() {
  PolygonMap pm(fnIlwis);
  if (pm->iFeatures() <= 0)
    return false;
  dvs = DomainValueRangeStruct(pm->dm());
  PutName();
  StartArcFile();
  iTotalArcs = 0;
 /* SegmentMap sm = pm->smp();
  ILWIS::Segment *seg = sm->segFirst();
  long iSegCnt = 0, iNrSegs = sm->iFeatures();
  while (seg && seg->fValid()) {
    PutSegment(seg);
    iSegCnt++;
    if (trq.fUpdate(iSegCnt, iNrSegs))
      return false;
	seg = sm->getFeature(iSegCnt);
  }*/
  EndFileMarker();
  trq.SetText(SCVTextWritePolLabels);
  StartLabFile();
  long iPolCnt = 1;
  for (int i=0 ; i < pm->iFeatures(); ++i) {
	  ILWIS::Polygon *pol = (ILWIS::Polygon *) pm->getFeature(i);
	  if ( pol == NULL || !pol->fValid())
		  continue;
	  if ( trq.fUpdate(i, pm->iFeatures()) )
		  return false;
	  if (pol->rArea() < 0)
		  continue;
	  Coord crd = pol->crdFindPointInPol();
	  WriteLabel(crd, iPolCnt);
	  iPolCnt++;
  }
  EndLabFileMarker();
  PutSpacial();
  PutHistory();
  CoordBounds cb = pm->cb();
  PutInfo(pm);
  WriteTrailer();
  trq.fUpdate(pm->iFeatures(), pm->iFeatures());
  return true;
}

bool E00::DoPoints() {
  PointMap pnt(fnIlwis);
  dvs = DomainValueRangeStruct(pnt->dm());
  long iNrPnts = pnt->iFeatures();
  if (iNrPnts <= 0)
    return false;
  PutName();
  StartLabFile();

  trq.SetText(SCVTextWriteLabels);
  for (long i = 1; i <= iNrPnts; ++i) {
    if (trq.fUpdate(i, iNrPnts))
      return false;
    Coord crdPnt = pnt->cValue(i);
    WriteLabel(crdPnt, i);
  }

  EndLabFileMarker();
  PutHistory();
  CoordBounds cb = pnt->cb();
  PutInfo(pnt);
  WriteTrailer();
  trq.fUpdate(iNrPnts, iNrPnts);
  return true;
}
  
void ImpExp::ExportE00(const FileName& fnObject, const FileName& fnFile) {
  try {
    trq.SetTitle(SCVTitleExportE00);
    trq.SetText(SCVTextProcessing);
    E00 e00(fnFile, fnObject, trq);
    if (fnObject.sExt == ".mps") {
      if (!e00.DoSegments())
        return;
    }
    else if (fnObject.sExt == ".mpa") {
      if (!e00.DoPolygons())
        return;
    }
    else if (fnObject.sExt == ".mpp") {
      if (!e00.DoPoints())
        return;
    }
  }
  catch (ErrorObject& err) {
    err.Show();
  }
}


