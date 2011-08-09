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
/* exppntai.c
   Export ArcInfo Generate file
   by Willem Nieuwenhuis, june 3, 1996
   ILWIS Department ITC
   Last change:  WN    6 Jan 97    2:30 pm
*/
#include "Headers\toolspch.h"
#include "Engine\DataExchange\Convloc.h"
#include "Headers\Hs\CONV.hs"
#include "Engine\Map\Point\PNT.H"
#include "Engine\Domain\dm.h"

class ArcInfoPntExporter {
public:
  ArcInfoPntExporter(const FileName&);
  ~ArcInfoPntExporter();
  void WritePoint(const Coord&, const Domain&, long);
  void WriteTrailer();
private:
  File* fileArcInfo;
};

ArcInfoPntExporter::ArcInfoPntExporter(const FileName& fnFile) {
  fileArcInfo = new File(fnFile, facCRT);
}

ArcInfoPntExporter::~ArcInfoPntExporter() {
  if (fileArcInfo) delete fileArcInfo;
}

void ArcInfoPntExporter::WritePoint(const Coord& crd, const Domain& dm, long iPnt) {
  fileArcInfo->WriteLnAscii(String("%4li%15.3lf%15.3lf", iPnt, crd.x, crd.y));
}

void ArcInfoPntExporter::WriteTrailer() {
  fileArcInfo->WriteLnAscii("END");
}

void ImpExp::ExportPntAi(const FileName& fnObject, const FileName& fnFile) {
  try {
    trq.SetTitle(TR("Exporting to Arc/Info Generate"));
    trq.SetText(TR("Processing..."));
    PointMap pm(fnObject);
    long iPntCnt = 0, iNrPoints = pm->iFeatures();
    if (iNrPoints == 0) {
      trq.fUpdate(iNrPoints, iNrPoints);
      return;
    }
    ArcInfoPntExporter aile(fnFile);
    Domain dm = pm->dm();
    for (iPntCnt = 1; iPntCnt <= iNrPoints; iPntCnt++) {
      Coord cCur = pm->cValue(iPntCnt);
      aile.WritePoint(cCur, dm, iPntCnt);
      if (trq.fUpdate(iPntCnt, iNrPoints))
        return;
    }
    aile.WriteTrailer();
    trq.fUpdate(iNrPoints, iNrPoints);
  }
  catch (ErrorObject& err) {
    err.Show();
  }
}


