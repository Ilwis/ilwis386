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
/* imppts.c
   Import Arc/Info .PTS file (point data)
   by Willem Nieuwenhuis, march 18, 1998
   ILWIS Department ITC
   Last change:  WN   23 Mar 98    2:52 pm
*/
#include "Headers\toolspch.h"
#include "Engine\Map\basemap.h"   
#include "Engine\Domain\dm.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\DataExchange\IMPPTS.H"
#include "Engine\SpatialReference\csbonly.h"

// implementation

/*---- Attribute Table functions ---------*/

void ArcInfoPts::MakeColumn(const FieldInfo& fi, Table& tbl) {
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
  for (unsigned short i = 0; i < sT.length(); i++) {
    if (!isalnum((unsigned char)sT[i])) sT[i] = '_';
  }
  Column col(tbl, sT, dvs);
}

ValueRange ArcInfoPts::vrField(const FieldInfo& fi) {
  if ( fi.iDecimal != 0 ) {
    double rStep = pow(10.0, -fi.iDecimal);
    short iBD = fi.iWidth - fi.iDecimal;
    double rMinMax =  pow(10.0, iBD) - 1;
    return ValueRange(-rMinMax, rMinMax, rStep);
  }
  short iBD = fi.iWidth;
  if (iBD < 10) {
    long iMinMax = (long)pow(10.0, iBD) - 1;
    return ValueRange(-iMinMax, iMinMax);
  }
  return ValueRange(Domain("value"));
}

void ArcInfoPts::MakeTable(const Domain& dmTable, Table& tbl) {
  FileName fnTable  = FileName::fnUnique(FileName(fnIlwis, ".tbt", true));
  FileName fnTblDat = FileName(fnTable, ".tb#", true);
  tbl.SetPointer(new TablePtr(fnTable, fnTblDat, dmTable, String()));
  String sD = TR("Table ");
  sD &= fnTable.sFile;
  sD &= fnTable.sExt;
  tbl->sDescription = sD;
}

void ArcInfoPts::AddDomainToAttrib(TObjectInfo& oi) {
  FieldInfo tiDomain;
  long iMin = oi.aiCodes[0], iMax = oi.aiCodes[0];
  for (unsigned int i = 1; i < oi.aiCodes.iSize(); i++) {
    if (oi.aiCodes[i] < iMin)
      iMin = oi.aiCodes[i];
    else if (oi.aiCodes[i] > iMax)
      iMax = oi.aiCodes[i];
  }
  tiDomain.sName = "PtsValue";    // Arc/Info point code
  tiDomain.atctCol = atctVALUE;
  tiDomain.iDecimal = 0;
  if (iMax < -iMin)
    iMax = -iMin;
  tiDomain.iWidth = 1 + (short)log10((double)iMax);
  MakeColumn(tiDomain, oi.tblAttrib);
  Column col = (oi.tblAttrib)->col(tiDomain.sName);
  for (unsigned short iRec = 0; iRec < oi.aiCodes.iSize(); iRec++)
    col->PutVal( iRec + 1, oi.aiCodes[iRec] );
}

/*-------------*/

ArcInfoPts::ArcInfoPts(const FileName& fnFile, Tranquilizer& _trq)
  : trq(_trq) {
  fileArcInfo = new File(fnFile);
  _iLins = 0;
  _iPoint = 0;
  _iTotPoints = 0;
}

ArcInfoPts::~ArcInfoPts() {
  if (fileArcInfo) delete fileArcInfo;
}

void ArcInfoPts::MarkErase(bool fRemove) {
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

void ArcInfoPts::GetNextLine() {
  if (fileArcInfo->fEof())
    UnexpectedEOF(fileArcInfo->sName());

  fileArcInfo->ReadLnAscii(sLine);
  _iLins++;
  if (_iLins % 25 == 0)
    if (!fScanning) {
      if (trq.fUpdate(iPointsRead(), iTotPoints()))
        UserAbort(fileArcInfo->sName());
    }
    else
      if (trq.fUpdate(fileArcInfo->iLoc() / 1000, fileArcInfo->iSize() / 1000))
        UserAbort(fileArcInfo->sName());
}

/* Fill the array of Arc/Info codes. This information
   will be turned into the LinValue column in the attribute table
*/
void ArcInfoPts::AddCode(long iCode, TObjectInfo& oi) {
  oi.iCount++;
  if (oi.fUnique)
    for (unsigned long i = 0; i < oi.aiCodes.iSize(); i++) {
      if (oi.aiCodes[i] == iCode) {
         oi.fUnique = false;
      }
    }
  oi.aiCodes &= iCode;
}

/*
  A typical line looks something like:
    1   242777.81    211533.09
  The first value designates the internal Arc/Info code
  The next two are the X and Y coordinate respectively
*/
bool ArcInfoPts::fArcInfoCoord() {
  Coord crdFile;
  long iCode;
  GetNextLine();
  short iRes = sscanf(sLine.c_str(), "%li %lf %lf", &iCode, &crdFile.x, &crdFile.y);
  if (3 == iRes) {
    _iPoint++;
    if (fScanning) {
      oi.cbMap += crdFile;
      AddCode(iCode, oi);
      if (iCode > oi.iIDMax)
        oi.iIDMax = iCode;
    }
    else
      WritePoint(_iPoint, crdFile);
    return true;
  }
  return false;
}

/* only attempt writing to existing map;
*/
void ArcInfoPts::WritePoint(long iCode, Coord& crd) {
  PointMapPtr *ppm = dynamic_cast<PointMapPtr *>(oi.mp.ptr());
  if (ppm == NULL)
    return;

  ppm->iAddRaw(crd, iCode);
}

/*
  Create the pointmap with the information found. The domain used is a
  domain ID with items looking like: "Pnt %i", the %i being the number of the line
  the point was found to be on.
  The point codes are stored in an attribute table.
  The CoordBounds is made 5% wider to avoid point locations on the edges.
*/
void ArcInfoPts::SetupPointMap(const FileName& fnObject) {
  FileName fnDomain = FileName::fnUnique(FileName(fnObject, ".dom"));
  Domain dm(fnDomain, oi.iCount, dmtID, "Pnt ");
  dm->pdsrt()->dsType = DomainSort::dsMANUAL;
  DomainValueRangeStruct dvs(dm);

// Add 5% to all boundary coordinates to avoid having points exactly
// on the edges
  double dx = (oi.cbMap.cMax.x - oi.cbMap.cMin.x) / 20;
  double dy = (oi.cbMap.cMax.y - oi.cbMap.cMin.y) / 20;
  oi.cbMap += Coord(oi.cbMap.cMax.x + dx, oi.cbMap.cMax.y + dy);
  oi.cbMap += Coord(oi.cbMap.cMin.x - dx, oi.cbMap.cMin.y - dy);

	CoordSystem csy;

  oi.mp = PointMap(fnObject, csy, oi.cbMap, dvs);

  if (oi.mp.fValid()) {
    if (oi.aiCodes.iSize() != 0) {
      MakeTable(dvs.dm(), oi.tblAttrib);
      oi.mp->SetAttributeTable(oi.tblAttrib);
      if (oi.aiCodes.iSize() != 0)
        AddDomainToAttrib(oi);     // Add domain column immediately
    }
  }

  oi.mp->sDescription = String(TR("Point Map %S").c_str(), fnObject.sFile);
  oi.mp->Store();
}

long ArcInfoPts::ScanInfo() {
  trq.SetText(TR("Scanning..."));
  fScanning = true;

  String s;
  while (fArcInfoCoord()) {
    if (fScanning)
      s = TR("Scanning ... Point ");
    else
      s = TR("Scanning ... Point ");
    s &= sPointCode();
    trq.SetText(s);
  }

  return _iLins;
}

void ArcInfoPts::Convert(const FileName& fnObject ) {
  fileArcInfo->Seek(0);
  _iTotPoints = _iPoint;
  _iPoint = 0;
  _iLins = 0;
  fnIlwis = fnObject;
  String s = TR("Scanning Arc/Info Generate");
  fScanning = false;

  SetupPointMap(fnIlwis);    // create point map

  MarkErase(true);
  while (fArcInfoCoord()) {
    s = TR("Importing Point ");
    s &= sPointCode();
    trq.SetText(s);
  }
  MarkErase(false);
}

void ImpExp::ImportPtsAi(const FileName& fnFile, const FileName& fnObject) {
  try {
    trq.SetTitle(TR("Importing from Arc/Info Generate"));
    ArcInfoPts ai(fnFile, trq);

    long iLines = ai.ScanInfo();
    if (iLines != iUNDEF) ai.Convert(fnObject);
  }
  catch (AbortedByUser&) {  // just stop, no message
  }
  catch (ErrorObject& err) {
    err.Show();
  }
}


