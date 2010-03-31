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
/* $Log: /ILWIS 3.0/Import_Export/DBF.cpp $
 * 
 * 4     15-01-03 18:04 Willem
 * - Changed: In case an internal domain is used dm->sName() also includes
 * an extension. The fieldname of the column will then become: <Name><ext>
 * where the name is shortened to keep the extension
 * 
 * 3     17-06-99 3:59p Martin
 * // -> /*
 * 
 * 2     17-06-99 2:09p Martin
 * ported files to VS
// Revision 1.4  1998/09/17 09:13:03  Wim
// 22beta2
//
// Revision 1.3  1998/01/08 14:50:16  Willem
// The month of the creation date was incorrectly calculated. It was always set one month
// before the actual month, giving problems only in the month january!
//
// Revision 1.2  1997/08/07 19:42:43  Willem
// DomainBool was not properly exported; the field was falsly set to numerical
// while the values themselves were written correctly (T and F), resulting in
// a mismatch when the data was to be used.
//
/*
  dbf.c
  by Li Fei, March 96
  ILWIS Department ITC
	Last change:  WN    8 Jan 98   10:29 am
*/

/* Revision history:
   22/11/96 : Start version
   14/03/97 : Add functionality to change dBase records such that spaces are converted
              into '_'. ArcView does not accept spaces there. This conversion is only 
              performed for ArcView in case of non-value domains. Trailing spaces are 
              not affected.
              Example: "Canal doble  " will become "Canal_doble  "
*/
#include "Headers\toolspch.h"
#include "Engine\Domain\Dmvalue.h"
#include "Engine\Domain\dmsort.h"
#include "Engine\DataExchange\Convloc.h"
#include "Engine\DataExchange\DBF.H"

#define FORDOMAIN true
#define FORCOLUMN false

dBaseDBF::dBaseDBF()
  : bbufWidth(1), bbufDecimal(1)
{
  bEndDescriptor='\x0D';
  bEndFile='\x1A';
  iCols=1;
  iColIndex=0;
}

dBaseDBF::File_Header::File_Header() {
  bValid = 3;
  iszRecord = 1;
  iszHeader = 32 + 33;
  for (short i = 0; i < 20; i++)
    bReserved[i] = 0;
  time_t timer;
  timer = time(NULL);
  tm* ptm = localtime(&timer);
  bDate[0] = ptm->tm_year;
  bDate[1] = ptm->tm_mon + 1;
  bDate[2] = ptm->tm_mday;
}

void dBaseDBF::MinMaxRecs(long iRecs, long iMin) {
  iMinRec = iMin;
  iMaxRec = iMin + iRecs - 1;
  Header.iNrRecords = iRecs;
}


dBaseDBF::DBF_Descriptor::DBF_Descriptor() {
  for (short j = 0; j < 4; j++)
    bReserved1[j] = 0;
  for (short j = 0; j < 14; j++)
    bReserved2[j] = 0;
}


void dBaseDBF::AllocRecord() {
  carRecord.Resize(Header.iszRecord);
  carRecord[0] = ' ';           // index 0 is used as DBF record separator
}

/*
  This routine is only required because the domain lacks a member function
  to return the maximum length of the string representation of its items
*/
short dBaseDBF::iStrDmWidth() {
  long iMaxLen = 0, iLast = iMaxRec;
  String sItem;
  if (dvs.dm()->pdsrt() == NULL)
    return 16;      // only DomainSort knows the number of domain items
  iLast = dvs.dm()->pdsrt()->iSize();
  for (long i = 1; i <= iLast; i++) {
    sItem = dvs.sValueByRaw(i, 0);
    iMaxLen = max(iMaxLen, sItem.length());
  }
  return (short)iMaxLen;
}

short dBaseDBF::iStrColWidth() {
  long iMaxLen = 0;
  String sItem;
  for (long i = iMinRec; i <= iMaxRec; i++) {
    sItem = colCur->sValue(i, 0);
    iMaxLen = max(iMaxLen, sItem.length());
  }
  return iMaxLen;
}

void dBaseDBF::iWidth(const DomainValueRangeStruct& dvrs, 
                      Column& col, 
                      bool fForDomain) {
  Descriptor.bszField = 0;
  Descriptor.bDecimal = 0;
  if (dvrs.fValues()) {
    short iBeforeDec = 0;
    Descriptor.cField = 'N';
    if (dvrs.dm()->dmt() == dmtBOOL) {
      Descriptor.cField = 'L';
      iBeforeDec = 1;
    }
    else if (dvrs.fRealValues()) {
      double rS = dvrs.rStep();
      RangeReal rr;
      if (col.fValid())
        rr = col->rrMinMax();
      else
        rr = dvrs.rrMinMax();
      if (abs(rS) < 1e-6) {     // added because iDec() returns 0, when rStep() = 0
        iBeforeDec = 8;
        Descriptor.bDecimal = 6;  // choose arbitrary precision
      }
      else {
        Descriptor.bDecimal = (byte)abs(floor(log10(rS)));
        iBeforeDec = floor(log10(max(abs(rr.rLo()), abs(rr.rHi())))) + 1;
      }
      if (rr.rLo() < 0)
        iBeforeDec++;
    }
    else {
      RangeInt ri;
      if (col.fValid())
        ri = col->riMinMax();
      else
        ri = dvrs.riMinMax();
	  double mx = max(abs(ri.iLo()), abs(ri.iHi()));
      iBeforeDec = floor(log10(mx)) + 1;
      if (ri.iLo() < 0)
        iBeforeDec++;
    }
    if (0 != Descriptor.bDecimal)
      iBeforeDec++;
    int iDigits = iBeforeDec + Descriptor.bDecimal;
    if (iDigits > DBASE_MAXNUMERIC) {
      if (iBeforeDec <= DBASE_MAXNUMERIC - 2)
        Descriptor.bDecimal = DBASE_MAXNUMERIC - iBeforeDec;
      else {
        Descriptor.bDecimal = 0;
        iBeforeDec = DBASE_MAXNUMERIC;
      }
    }    
    Descriptor.bszField = (byte)iBeforeDec + Descriptor.bDecimal;
  }
  else {
    Descriptor.cField = 'C';
    if (fForDomain)
      Descriptor.bszField = iStrDmWidth();
    else
      Descriptor.bszField = iStrColWidth();
  }
}

void dBaseDBF::SetDmDescriptor()
{
	String sDomain = dvs.dm()->sName().toUpper();

	// Use only the name after any backslash/colon
	long i = sDomain.find_last_of("\\:");
	sDomain = sDomain.sRight(sDomain.length() - i - 1);

	// In case an internal domain is used dm->sName() also includes an extension
	// The fieldname of the column will then become:
	// <Name><ext> where the name is shortened to keep the extension
	// f.e.: "vlakkenpol.mpa" will give "vlakkenvmpa" for the fieldname
	bool fInternalDomain = !fCIStrEqual(dvs.dm()->fnObj.sExt, ".dom");
	String sName = sDomain.sHead(".");
	String sExt = sDomain.sTail(".");

	long iLen = min(fInternalDomain ? 8 : 11, sName.length() - i);
	long j;
	i = 0;
	for (j = 0; j < iLen; i++, j++)
		Descriptor.cNameField[j] = sName[i];
	if (fInternalDomain)
	{
		iLen += min(3, sExt.length());
		i = 0;
		for (; j < iLen; i++, j++)
			Descriptor.cNameField[j] = sExt[i];
	}
	for (; j < 11; j++)
		Descriptor.cNameField[j] = '\0';

	iWidth(dvs, Column(), FORDOMAIN);    // set Descriptor.bDecimal, Descriptor.bszField and Descriptor.cField
	bbufDecimal[0] = Descriptor.bDecimal;
	bbufWidth[0] = Descriptor.bszField;
	Header.iszRecord += Descriptor.bszField;
	iColIndex = 1;
}

void dBaseDBF::FillRealRecord(double rVal) {
  short i;
  for (i = 1; i < Header.iszRecord; i++)
    carRecord[i] = ' ';
  iRecIndex = 1;
  if (!fHasDm)
    return;
  String ss = String("%*.*lf", bbufWidth[0], bbufDecimal[0], rVal);
  for (i = 0; i < ss.length(); i++)
    carRecord[i + 1] = ss[i];
  iRecIndex += bbufWidth[0];
}

/* the following converts internal spaces into "_", because ArcView does not
   accept spaces there
*/
void dBaseDBF::ConvertSpacesForArcView(short iOffset, const String& ss, Array<char>& carRecord) {
  short i = ss.length() - 1;
  while (ss[i--] == ' ');
  for (i = i; i >= 0; i--)
    if (ss[i] == ' ')
      carRecord[i + iOffset] = '_';
}

/*
  if the domain uses the raw value as the actual value, the iRec parameter
  is this actual value. In all other cases it is possible to use the
  sValueByRaw() function and iRec is the raw value.
*/
void dBaseDBF::SetDmRecord(long iRec) {
  short i;
  for (i = 1; i < Header.iszRecord; i++)
    carRecord[i] = ' ';
  iRecIndex = 1;
  if (!fHasDm)
    return;
  String ss;
  if (dvs.fRawIsValue())
    ss = dvs.sValue(iRec, bbufWidth[0], bbufDecimal[0]);
  else
    ss = dvs.sValueByRaw(iRec, bbufWidth[0], bbufDecimal[0]);
  for (i = 0; i < ss.length(); i++)
    carRecord[i + 1] = ss[i];
  if (fConvertInternalSpaces && !dvs.fValues())
    ConvertSpacesForArcView(1, ss, carRecord);
  iRecIndex += bbufWidth[0];
}

void dBaseDBF::FillRecord(long iRec) {
  SetDmRecord(iRec);
  if( !fHasTbl )
    return;
  short iFirstCol = fHasDm ? 1 : 0;
  short iCurCol = 0;
  String ss;
  for ( short iC = iFirstCol; iC < iCols; iC++, iCurCol++ ) {
    colCur = tbl->col(iCurCol);
    ss = colCur->sValue(iRec, bbufWidth[iC], bbufDecimal[iC]);
    for (short i = 0; i < ss.length(); i++)
      carRecord[i + iRecIndex] = ss[i];

    if (fConvertInternalSpaces && !colCur->dvrs().fValues())
      ConvertSpacesForArcView(iRecIndex, ss, carRecord);
    iRecIndex += bbufWidth[iC];
  }
}

bool dBaseDBF::fFillDescriptor() {
  if (fHasDm && !iColIndex) {
    SetDmDescriptor();
    return true;
  }
  if (iColIndex < iCols) {
    short iCurCol = fHasDm ? iColIndex - 1 : iColIndex;
    colCur = tbl->col(iCurCol);
    String sColName = colCur->sName().toUpper();
    short i, iLen = min(11, sColName.length());
    for ( i = 0; i < iLen; i++)
      Descriptor.cNameField[i] = sColName[i];
    for (; i < 11; i++)
      Descriptor.cNameField[i] = '\0';
    iWidth(colCur->dvrs(), colCur, FORCOLUMN);
    Header.iszRecord += Descriptor.bszField;
    bbufDecimal[iColIndex] = Descriptor.bDecimal;
    bbufWidth[iColIndex] = Descriptor.bszField;
    iColIndex++;
    return true;
  }
  AllocRecord();
  return false;
}

DomainToDBF::DomainToDBF(const DomainValueRangeStruct& dvsDBF, long iRecs, bool fCnvForArcView)
{
  fHasDm = true;
  fConvertInternalSpaces = fCnvForArcView;
  dvs = dvsDBF;
  fHasTbl = false;
  MinMaxRecs(iRecs, 1);
  SetDmDescriptor();
  AllocRecord();
}

TableToDBF::TableToDBF(const Table& tblDBF, long iRecs, bool fCnvForArcView)
{
  fHasTbl = true;
  fConvertInternalSpaces = fCnvForArcView;
  tbl = tblDBF;
  fHasDm = tbl->dm()->pdnone() == 0 ? true : false;
  iCols = tbl->iCols();
  if (fHasDm) {
    iCols++;
    dvs = DomainValueRangeStruct(tbl->dm());
  }
  if (iRecs != 0)
    MinMaxRecs(iRecs, 1);
  else
    MinMaxRecs(tbl->iRecs(), tbl->iOffset());
  bbufWidth.Size(iCols);
  bbufDecimal.Size(iCols);
  Header.iszHeader = iCols * 32 + 33;
}


