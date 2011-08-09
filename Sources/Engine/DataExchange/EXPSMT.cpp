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
/* expsmt.c
   Export Ilwis Segment List
   by Willem Nieuwenhuis, march 11, 1996
   ILWIS Department ITC
   Last change:  WN   23 Sep 97    2:24 pm
*/

#include "Headers\toolspch.h"
#include "Engine\DataExchange\Convloc.h"
#include "Headers\Hs\CONV.hs"
#include "Engine\Map\Segment\Seg.h"
#include "Engine\Domain\dm.h"
#include "Engine\Base\File\File.h"

class SMTListExporter {
public:
  SMTListExporter(const FileName&);
  ~SMTListExporter();
  void WriteHeader(const CoordBounds&);
  void WriteSegment(const ILWIS::Segment*, const DomainValueRangeStruct&);
private:
  File* fileSMT;
  CoordBuf cbuf;
  RowColBuf rcb;
};

SMTListExporter::SMTListExporter(const FileName& fnFile) {
  fileSMT = new File(fnFile, facCRT);
  rcb.Size(1000);
  cbuf.Size(1000);
}

SMTListExporter::~SMTListExporter() {
  if (fileSMT) delete fileSMT;
}

void SMTListExporter::WriteHeader(const CoordBounds& cbVec) {
  fileSMT->WriteLnAscii(TR("Segments"));
  String sCoord(" %10ld %10ld", long(cbVec.MinX()), long(cbVec.MinY()));
  fileSMT->WriteLnAscii(sCoord);
  sCoord = String(" %10ld %10ld", long(cbVec.MaxX()), long(cbVec.MaxY()));
  fileSMT->WriteLnAscii(sCoord);
}

void SMTListExporter::WriteSegment(const ILWIS::Segment* seg, const DomainValueRangeStruct& dvs) {
  CoordinateSequence *cbuf = seg->getCoordinates();
  long iNrCoord = cbuf->size();
  String ss;
  if (dvs.fRawIsValue())
	  ss = dvs.sValue(seg->iValue(), 0);
  else
	  ss = dvs.sValueByRaw(seg->iValue(), 0);
  fileSMT->WriteLnAscii(ss);
//  fileSMT->WriteLnAscii(dvs.sValueByRaw(seg.iRaw()));
  for (long i = 0; i < iNrCoord; i++)
	  fileSMT->WriteLnAscii(String(" %15.6lf %15.6lf", cbuf->getAt(i).x, cbuf->getAt(i).y));
  delete cbuf;
}

void ImpExp::ExportSMT(const FileName& fnObject, const FileName& fnFile) {
  try {
    trq.SetTitle(TR("Exporting to SMT list"));
    trq.SetText(TR("Processing..."));
    SegmentMap sm(fnObject);
    long iSegCnt = 0, iNrSegs = sm->iFeatures();
    SMTListExporter smt(fnFile);
    smt.WriteHeader(sm->cb());
    DomainValueRangeStruct dvs = sm->dvrs();
    ILWIS::Segment *seg = (ILWIS::Segment *) sm->segFirst();
    while (seg->fValid()) {
      smt.WriteSegment(seg, dvs);
      iSegCnt++;
      if (trq.fUpdate(iSegCnt, iNrSegs))
        return;
	  seg = (ILWIS::Segment *)sm->getFeature(iSegCnt);
    }
    trq.fUpdate(iNrSegs, iNrSegs);
  }
  catch (ErrorObject& err) {
    err.Show();
  }
}


