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
  Export ILWIS Table to dBase .sdf format
  Willem Nieuwenhuis, july 1996
  ILWIS Department ITC
	Last change:  WN    6 Jan 97    2:36 pm
*/
#include "Headers\toolspch.h"
#include "Engine\DataExchange\Convloc.h"
#include "Engine\Domain\Dmvalue.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Table\tbl.h"
#include "Engine\DataExchange\RECSTRUC.H"

#define FORDOMAIN true
#define FORCOLUMN false

class TableToSDF {
public:
  TableToSDF(const FileName&, const FileName&);
  ~TableToSDF();
  void SetupRecord(long);
  void WriteRecord();
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

TableToSDF::TableToSDF(const FileName& fnTable, const FileName& fnSDF) {
  _tbl = Table(fnTable);
  _dm = _tbl->dm();
  fHasDomain = _dm->pdnone() == 0 ? true : false;
  arsStruct.Resize(_tbl->iCols() + (fHasDomain ? 1 : 0));
  SetupStructure();
  filOut = new File(fnSDF, facCRT);
}

TableToSDF::~TableToSDF() {
  if (filOut) delete filOut;
  if (pcOem)  delete[] pcOem;
}

short TableToSDF::iStrDomainWidth() {
  long iMaxLen = 0;
  String sItem;
  for (long i = iMinRec; i <= iMaxRec; i++) {
    sItem = _dm->sValueByRaw(i, 0);
    iMaxLen = max(iMaxLen, sItem.length());
  }
  return iMaxLen;
}

short TableToSDF::iStrColWidth() {
  long iMaxLen = 0;
  String sItem;
  for (long i = iMinRec; i <= iMaxRec; i++) {
    sItem = colCur->sValue(i, 0);
    iMaxLen = max(iMaxLen, sItem.length());
  }
  return iMaxLen;
}

short TableToSDF::DetermineWidth(short iCurCol, const DomainValueRangeStruct& dvrs, bool fForDomain) {
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
    if (arsStruct[iCurCol].iDecimals > 0) {  // adjust for decimal point
      iBeforeDec++;
      arsStruct[iCurCol].iWidth += arsStruct[iCurCol].iDecimals + 1;
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

void TableToSDF::SetupStructure() {
  iMinRec = _tbl->iOffset();
  iMaxRec = iMinRec + _tbl->iRecs() - 1;
  short iColIndex = 0;
  short iRecLength = 0;
  if (fHasDomain) {
    DomainValueRangeStruct dvrsDom(_dm);
    iRecLength += DetermineWidth(iColIndex, dvrsDom, FORDOMAIN);
    iColIndex++;
  }
  for (short iCurCol = 0; iCurCol < _tbl->iCols(); iCurCol++, iColIndex++) 
	{
    colCur = _tbl->col(iCurCol);
    iRecLength += DetermineWidth(iColIndex, colCur->dvrs(), FORCOLUMN) + 1;  // + 1 to account for spaces between fields
  }
  pcOem = new char[iRecLength + 1];
}

void TableToSDF::SetupRecord(long iRec) {
  sRecord = "";
  short iCurCol = 0;
  if (fHasDomain) {
    sRecord &= _dm->sValueByRaw(iRec, arsStruct[iCurCol].iWidth, arsStruct[iCurCol].iDecimals);
    iCurCol++;
  }
  for (short iC = 0; iC < _tbl->iCols(); iC++, iCurCol++ ) {
    colCur = _tbl->col(iC);
    sRecord &= " ";    // added for readability
    sRecord &= colCur->sValue(iRec, arsStruct[iCurCol].iWidth, arsStruct[iCurCol].iDecimals);
  }
}

void TableToSDF::WriteRecord() {
  CharToOem(sRecord.scVal(), pcOem);
  filOut->Write(sRecord.length(), pcOem);
  filOut->Write(2, "\r\n");
}

void ImpExp::ExportSDF(const FileName& fnObject, const FileName& fnFile) {
  trq.SetTitle(SCVTitleExportSDF);
  trq.SetText(SCVTextProcessing);
  TableToSDF SDF(fnObject, fnFile);
  for (long iRec = SDF.iMinRec; iRec <= SDF.iMaxRec; iRec++ ) {
    SDF.SetupRecord(iRec);
    SDF.WriteRecord();
    if (trq.fUpdate(iRec, SDF.iMaxRec))
      return;
  }
  trq.fUpdate(SDF.iMaxRec, SDF.iMaxRec);
}


