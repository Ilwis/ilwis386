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
/* implin.c
   Import Ilwis Segment List
   by Willem Nieuwenhuis, march 11, 1996
   ILWIS Department ITC
   Last change:  WN   20 Mar 98    4:32 pm
*/
#include "Headers\toolspch.h"
#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\Engine.h"
#include "Engine\Map\Raster\Map.h"
#include "Engine\DataExchange\IMPLIN.H"
#include "Engine\SpatialReference\csbonly.h"

// implementation

/*-------------*/

ArcInfoLin::ArcInfoLin(const FileName& fnFile, Tranquilizer& _trq, CWnd *w)
  : trq(_trq), m_wnd(w)
{
  fileArcInfo = new File(fnFile);
  _iLins = 0;
  _iNrSeg = 0;
  _iTotSegs = 0;
  fInSegment = false;
  rcbuf.Size(1000);
  cbuf.Size(1000);
}

ArcInfoLin::~ArcInfoLin() {
  if (fileArcInfo) delete fileArcInfo;
}

void ArcInfoLin::MarkErase(bool fRemove) {
  oi.mp->fErase = fRemove;
  if (!oi.mp->dm()->fSystemObject())
    oi.mp->dm()->fErase = fRemove;
  Table& tbl = oi.tblAttrib;
  if (tbl.fValid()) {
    tbl->fErase = fRemove;
    for (short i = 0; i < tbl->iCols(); i++)
      if (!tbl->col(i)->dm()->fSystemObject())
        tbl->col(i)->dm()->fErase = fRemove;
  }
}

void ArcInfoLin::GetNextLine() {
  if (fileArcInfo->fEof())
    UnexpectedEOF(fileArcInfo->sName());

  fileArcInfo->ReadLnAscii(sLine);
  _iLins++;
  if (_iLins % 10 == 0)
    if (!fScanning) {
      if (trq.fUpdate(iSegmentsRead(), iTotSeg()))
        UserAbort(fileArcInfo->sName());
    }
    else
      if (trq.fUpdate(fileArcInfo->iLoc() / 1000, fileArcInfo->iSize() / 1000))
        UserAbort(fileArcInfo->sName());
}

bool ArcInfoLin::fArcInfoCoord(Coord& cFile) {
  GetNextLine();
  short iRes = sscanf(sLine.c_str(), "%lf %lf", &cFile.x, &cFile.y);
  return (2 == iRes);
}

String ArcInfoLin::sArcInfoCode() {
  char pc[40];
  GetNextLine();
  short iRes = sscanf(sLine.c_str(), "%s", pc);
  if (0 == iRes)
    FormatProblem(String(TR("%S, in line %ld").c_str(), fileArcInfo->sName().c_str(), iLinesRead()));
  return String(pc);
}

/* Arc/Info segments do not have more than 500 coordinates
*/
bool ArcInfoLin::fArcInfoSegment()
{
  if (_iNrSeg > MAX_SEGMENTS) {
    if (fScanning) {
      int iAns = getEngine()->Message(String(TR("Too many segments in input, only first %ld will be converted.\nContinue ?").c_str(), MAX_SEGMENTS).c_str(),
                               TR("Importing from Arc/Info Generate").c_str(),
                               MB_YESNO | MB_ICONEXCLAMATION);
      if (iAns == IDNO)
        UserAbort(fileArcInfo->sName());
    }
    return false;
  }
  long iNrCoord = 0;

  if (fInSegment) {
    fInSegment = false;
    cbuf[0] = cbuf[1000 - 1];
    iNrCoord = 1;
  }
  else {
    _sSegCode = sArcInfoCode();
    if (_sSegCode.sLeft(3) == "END")     // end of input detected
      return false;
  }

  while ( fArcInfoCoord(cbuf[iNrCoord]) ) {
    if (fScanning) {
      oi.cbMap += cbuf[iNrCoord];
    }
    iNrCoord++;
    if (iNrCoord >= 1000) {
      fInSegment = true;
      break;
    }
  }
// end while: sLine should now hold 'END' if fInSegment is false

// Always write segment when not scanning
  _iNrSeg++;
  if (fScanning) {
    long iCode = _sSegCode.iVal();
    AddCode(iCode, oi);
    if (iCode > oi.iIDMax)
      oi.iIDMax = iCode;
  }
  else
    WriteSegment(_iNrSeg, iNrCoord);
  if (!fInSegment && (sLine.sLeft(3) != "END"))
    FormatProblem(String(TR("%S, in line %ld").c_str(), fileArcInfo->sName().c_str(), iLinesRead()));
  return true;
}

void ArcInfoLin::MakeColumn(const FieldInfo& fi, Table& tbl) {
  ValueRange vr;
  String sDomain;
  switch (fi.atctCol) {
    case atctVALUE :
      sDomain = "value";
      vr = vrField(fi);
      break;
    default :            // should be only atctSTRING
      sDomain = "string";
      break;
  }
  FileName fnDom(sDomain, ".dom", true);
  Domain dm(fnDom);
  DomainValueRangeStruct dvs(dm, vr);
  String sT = fi.sName;
  for (short i = 0; i < sT.length(); i++) {
    if (!isalnum(sT[i])) sT[i] = '_';
  }
  Column col(tbl, sT, dvs);
}

ValueRange ArcInfoLin::vrField(const FieldInfo& fi) {
  if ( fi.iDecimal != 0 ) {
    double rStep = pow(10.0, -fi.iDecimal);
    short iBD = fi.iWidth - fi.iDecimal;
    double rMinMax =  pow(10.0, iBD) - 1;
		if (abs(rMinMax) < 1)
			rMinMax = 1;
    return ValueRange(-rMinMax, rMinMax, rStep);
  }
  short iBD = fi.iWidth;
  if (iBD < 10) {
    long iMinMax = (long)pow(10.0, iBD) - 1;
		if (abs(iMinMax) < 1)
			iMinMax = 1;
    return ValueRange(-iMinMax, iMinMax);
  }
  return ValueRange(Domain("value"));
}

void ArcInfoLin::MakeTable(const Domain& dmTable, Table& tbl) {
  FileName fnTable  = FileName::fnUnique(FileName(fnIlwis, ".tbt", true));
  FileName fnTblDat = FileName(fnTable, ".tb#", true);
  tbl.SetPointer(new TablePtr(fnTable, fnTblDat, dmTable, String()));
  String sD = TR("Table ");
  sD &= fnTable.sFile;
  sD &= fnTable.sExt;
  tbl->sDescription = sD;
}

void ArcInfoLin::AddDomainToAttrib(TObjectInfo& oi) {
  FieldInfo tiDomain;
  long iMin = oi.aiCodes[0], iMax = oi.aiCodes[0];
  for (int i = 1; i < oi.aiCodes.iSize(); i++) {
    if (oi.aiCodes[i] < iMin)
      iMin = oi.aiCodes[i];
    else if (oi.aiCodes[i] > iMax)
      iMax = oi.aiCodes[i];
  }
  tiDomain.sName = "LinValue";    // Arc/Info segment code
  tiDomain.atctCol = atctVALUE;
  tiDomain.iDecimal = 0;
  if (iMax < -iMin)
    iMax = -iMin;
  tiDomain.iWidth = 1 + (short)log10((double)(iMax));
  MakeColumn(tiDomain, oi.tblAttrib);
  Column col = (oi.tblAttrib)->col(tiDomain.sName);
  for (short iRec = 0; iRec < oi.aiCodes.iSize(); iRec++)
    col->PutVal( iRec + 1, oi.aiCodes[iRec] );
}

/* Fill the array of Arc/Info codes. This information
   will be turned into the LinValue column in the attribute table
*/
void ArcInfoLin::AddCode(long iCode, TObjectInfo& oi) {
  oi.iCount++;
  if (oi.fUnique)
    for (long i = 0; i < oi.aiCodes.iSize(); i++) {
      if (oi.aiCodes[i] == iCode) {
         oi.fUnique = false;
      }
    }
  oi.aiCodes &= iCode;
}

long ArcInfoLin::ScanInfo() {
  trq.SetText(TR("Scanning..."));
  fScanning = true;

  String s;
  while (fArcInfoSegment()) {
    if (fScanning)
      s = TR("Scanning ... Segment ");
    else
      s = TR("Importing Segment ");
    s &= sSegmentCode();
    trq.SetText(s);
  }

  return _iLins;
}

void ArcInfoLin::SetupSegMap(const FileName& fnObject) {
  FileName fnDomain = FileName::fnUnique(FileName(fnObject, ".dom"));
  Domain dm(fnDomain, oi.iCount, dmtID, "Seg ");
  dm->pdsrt()->dsType = DomainSort::dsMANUAL;
  DomainValueRangeStruct dvs(dm);

	CoordSystem csy;

  oi.mp = SegmentMap(fnObject, csy, oi.cbMap, dvs);

  if (oi.mp.fValid()) {
    if (oi.aiCodes.iSize() != 0) {
      MakeTable(dvs.dm(), oi.tblAttrib);
      oi.mp->SetAttributeTable(oi.tblAttrib);
      if (oi.aiCodes.iSize() != 0)
        AddDomainToAttrib(oi);     // Add domain column immediately
    }
  }

  oi.mp->sDescription = String(TR("Segment map %S").c_str(), fnObject.sFile);
  oi.mp->Store();
}

/* only attempt writing to existing map;
*/
void ArcInfoLin::WriteSegment(long iCode, long iNrCoord) {
  if (iNrCoord < 2)
    return;
  SegmentMapPtr *psm = dynamic_cast<SegmentMapPtr *>(oi.mp.ptr());
  if (psm == NULL)
    return;
  ILWIS::Segment *segCur = CSEGMENT(psm->newFeature());
  segCur->PutCoords(iNrCoord, cbuf);
  segCur->PutVal(iCode);    // let Domain take care of prefix
}

void ArcInfoLin::Convert(const FileName& fnObject ) {
  fileArcInfo->Seek(0);
  _iTotSegs = _iNrSeg;
  _iNrSeg = 0;
  _iLins = 0;
  fnIlwis = fnObject;
  String s = TR("Scanning Arc/Info Generate");
  fScanning = false;

  SetupSegMap(fnIlwis);    // create segment map

// Take care to delete garbage when stop button is pressed
  MarkErase(true);

  while (fArcInfoSegment()) {
    s = TR("Importing Segment ");
    s &= sSegmentCode();
    trq.SetText(s);
  }

// All went well, lets keep all stuff
  MarkErase(false);
  trq.fUpdate(iTotSeg(), iTotSeg());
}

void ImpExp::ImportLIN(const FileName& fnFile, const FileName& fnObject) {
  try {
    trq.SetTitle(TR("Importing from Arc/Info Generate"));
    ArcInfoLin ai(fnFile, trq, win);

    long iLines = ai.ScanInfo();
    if (iLines != iUNDEF) ai.Convert(fnObject);
  }
  catch (AbortedByUser& ) {  // just stop, no message
  }
  catch (ErrorObject& err) {
    err.Show();
  }
}


