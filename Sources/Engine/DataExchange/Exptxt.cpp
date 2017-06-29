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
/*
  Export ILWIS Table to delimited format (.txt)
  Willem Nieuwenhuis, july 1996
  ILWIS Department ITC
	Last change:  WK   17 Sep 98    5:03 pm
*/
#include "Headers\toolspch.h"
#include "Engine\DataExchange\Convloc.h"
#include "Engine\Domain\Dmvalue.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Table\tbl.h"
#include "Engine\DataExchange\RECSTRUC.H"

#define FORDOMAIN true
#define FORCOLUMN false

class TableToDelim {
public:
  TableToDelim(const FileName&, const FileName&);
  ~TableToDelim();
  void SetupRecord(long);
  void WriteRecord();
  void WriteHeader();
  long iMinRec, iMaxRec;
private:
  void SetupStructure();
  short DetermineWidth(short, const DomainValueRangeStruct&, bool);
  short iStrDomainWidth();
  short iStrColWidth();
  Table _tbl;
  Domain _dm;
  Column colCur;
  File* filOut;
  String sRecord;         // will have a fixed length
  bool fHasDomain;
  Array<RecordStruct> arsStruct;
  char *pcOem;
};

// implementation

TableToDelim::TableToDelim(const FileName& fnTable, const FileName& fnDelim) {
  _tbl = Table(fnTable);
  _dm = _tbl->dm();
  fHasDomain = _dm->pdnone() == 0 ? true : false;
  arsStruct.Resize(_tbl->iCols() + (fHasDomain ? 1 : 0));
  SetupStructure();
  filOut = new File(fnDelim, facCRT);
}

TableToDelim::~TableToDelim() {
  if (filOut) delete filOut;
  if (pcOem)  delete[] pcOem;
}

short TableToDelim::iStrDomainWidth() {
  long iMaxLen = 0;
  String sItem;
  for (long i = iMinRec; i <= iMaxRec; i++) {
    sItem = _dm->sValueByRaw(i, 0);
    iMaxLen = max(iMaxLen, sItem.length());
  }
  return iMaxLen;
}

short TableToDelim::iStrColWidth() {
  long iMaxLen = 0;
  String sItem;
  for (long i = iMinRec; i <= iMaxRec; i++) {
    sItem = colCur->sValue(i, 0);
    iMaxLen = max(iMaxLen, sItem.length());
  }
  return iMaxLen;
}

short TableToDelim::DetermineWidth(short iCurCol, const DomainValueRangeStruct& dvrs, bool fForDomain) {
  arsStruct[iCurCol].ftType = ftCHAR;
  arsStruct[iCurCol].iDecimals = 0;
  String sColForm;
  if (dvrs.fValues()) {
    short iBeforeDec = 0;
    arsStruct[iCurCol].ftType = ftINT;
    if (dvrs.fRealValues()) {
      arsStruct[iCurCol].ftType = ftFLOAT;
      double rS = dvrs.rStep();
      RangeReal rr = dvrs.rrMinMax();
      if (abs(rS) < 1e-6) {        // added because iDec() returns 0, when rStep() = 0
        iBeforeDec = 8;
        arsStruct[iCurCol].iDecimals = 4;   // choose a fixed format
      }
      else {
        arsStruct[iCurCol].iDecimals = (byte)abs(floor(log10(rS)));
        iBeforeDec = floor(log10(max(abs(rr.rLo()), abs(rr.rHi())))) + 1;
      }
      if (rr.rLo() < 0)
        iBeforeDec++;
    }
    else {
      RangeInt ri = dvrs.riMinMax();
	  double mx = max(abs(ri.iLo()), abs(ri.iHi()));
      iBeforeDec = floor(log10(mx)) + 1;
      if (ri.iLo() < 0)
        iBeforeDec++;
    }
    arsStruct[iCurCol].iWidth = iBeforeDec;
    if (0 < arsStruct[iCurCol].iDecimals) {  // adjust for decimal point
      iBeforeDec++;
      arsStruct[iCurCol].iWidth += arsStruct[iCurCol].iDecimals;
    }
  }
  else {
    arsStruct[iCurCol].ftType = ftCHAR;
    if (fForDomain)
      arsStruct[iCurCol].iWidth = iStrDomainWidth();
    else
      arsStruct[iCurCol].iWidth = iStrColWidth();
  }
  return arsStruct[iCurCol].iWidth;
}

void TableToDelim::SetupStructure() {
  iMinRec = _tbl->iOffset();
  iMaxRec = iMinRec + _tbl->iRecs() - 1;
  short iColIndex = 0;
  short iRecLength = 0;
  if (fHasDomain) {
    DomainValueRangeStruct dvrsDom(_dm);
    iRecLength += DetermineWidth(iColIndex, dvrsDom, FORDOMAIN);
    iColIndex++;
  }
  for (short iCurCol = 0; iCurCol < _tbl->iCols(); iCurCol++, iColIndex++) {
    colCur = _tbl->col(iCurCol);
    int iLen = DetermineWidth(iColIndex, colCur->dvrs(), FORCOLUMN);
    if (iLen < 20) // fake factor to prevent crashes! 17/9/98 Wim
      iLen = 20;
    iRecLength += iLen;
  }
  pcOem = new char[iRecLength + 1];
}

void TableToDelim::SetupRecord(long iRec) {
  sRecord = "";
  short iCurCol = 0;
  if (fHasDomain) {
    if (ftCHAR == arsStruct[iCurCol].ftType) {
      sRecord = "\"";
      sRecord &= _dm->sValueByRaw(iRec, 0, 1); // setting decimals to 1 strips trailing spaces!
      sRecord &= "\"";
    }
    else
      sRecord = _dm->sValueByRaw(iRec, arsStruct[iCurCol].iWidth, arsStruct[iCurCol].iDecimals);
    iCurCol++;
  }
  for (short iC = 0; iC < _tbl->iCols(); iC++, iCurCol++ ) {
    colCur = _tbl->col(iC);
    if (sRecord.length() > 0)
      sRecord &= ",";
    if (ftCHAR == arsStruct[iCurCol].ftType) {
      sRecord &= "\"";
      sRecord &= colCur->sValue(iRec, 0, 1); // setting decimals to 1 strips trailing spaces!
      sRecord &= "\"";
    }
    else if (ftINT == arsStruct[iCurCol].ftType) {
      long iVal = colCur->iValue(iRec);
      sRecord &= String("%li", iVal);
    }
    else if (ftFLOAT == arsStruct[iCurCol].ftType) {
      double rVal = colCur->rValue(iRec);
      sRecord &= String("%.6g", rVal);
    }
  }
}

void TableToDelim::WriteHeader(){
	String line;

	for(int c=0; c < _tbl->iCols(); ++c){
		if ( line != "")
			line += ",";
		line += "\"" + _tbl->col(c)->sName() + "\"";
	}
	AnsiToOem(line.c_str(), pcOem);
	filOut->Write(line.length(), pcOem);
	filOut->Write(2, "\r\n");
}
void TableToDelim::WriteRecord() {
  AnsiToOem(sRecord.c_str(), pcOem);
  filOut->Write(sRecord.length(), pcOem);
  filOut->Write(2, "\r\n");
}

void ImpExp::ExportDelim(const FileName& fnObject, const FileName& fnFile) {
  trq.SetTitle(TR("Export table to delimited text"));
  trq.SetText(TR("Processing..."));
  TableToDelim Delim(fnObject, fnFile);
  Delim.WriteHeader();
  for (long iRec = Delim.iMinRec; iRec <= Delim.iMaxRec; iRec++ ) {
    Delim.SetupRecord(iRec);
    Delim.WriteRecord();
    if (trq.fUpdate(iRec, Delim.iMaxRec))
      return;
  }
  trq.fUpdate(Delim.iMaxRec, Delim.iMaxRec);
}


