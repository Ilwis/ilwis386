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
/* expbna.c
   Export ArcInfo Generate file
   by Willem Nieuwenhuis, june 3, 1996
   ILWIS Department ITC
   Last change:  WN    6 Jan 97    2:27 pm
*/
#include "Headers\toolspch.h"
#include "Engine\DataExchange\Convloc.h"
#include "Headers\Hs\CONV.hs"
#include "Engine\Map\Segment\Seg.h"
#include "Engine\Domain\dm.h"

class AtlasBNAExporter {
public:
  AtlasBNAExporter(const FileName&);
  ~AtlasBNAExporter();
  void WriteSegment(const ILWIS::Segment*, const Domain&);
private:
  File* fileAtlas;
  CoordBuf cbuf;
  RowColBuf rcb;
};

AtlasBNAExporter::AtlasBNAExporter(const FileName& fnFile) {
  fileAtlas = new File(fnFile, facCRT);
  rcb.Size(1000);
  cbuf.Size(1000);
}

AtlasBNAExporter::~AtlasBNAExporter() {
  if (fileAtlas) delete fileAtlas;
}

void AtlasBNAExporter::WriteSegment(const ILWIS::Segment* seg, const Domain& dm) {
	CoordinateSequence *cbuf = seg->getCoordinates();
	fileAtlas->WriteLnAscii(String("\"%S\",%li", dm->sValueByRaw(seg->iValue(), 0), -(long)cbuf->size()));
  for (long i = 0; i < cbuf->size(); i++)
	  fileAtlas->WriteLnAscii(String("%.5lf,%.5lf", cbuf->getAt(i).x, cbuf->getAt(i).y));
  delete cbuf;
}

void ImpExp::ExportBNA(const FileName& fnObject, const FileName& fnFile) {
  try {
    trq.SetTitle(SCVTitleExportBNA);
    trq.SetText(SCVTextProcessing);
    SegmentMap sm(fnObject);
    long iSegCnt = 0, iNrSegs = sm->iFeatures();
    AtlasBNAExporter abe(fnFile);
    Domain dm = sm->dm();
    ILWIS::Segment *seg = sm->segFirst();
    while (seg->fValid()) {
      abe.WriteSegment(seg, dm);
      iSegCnt++;
      if (trq.fUpdate(iSegCnt, iNrSegs))
        return;
      ++seg;
    }
    trq.fUpdate(iNrSegs, iNrSegs);
  }
  catch (ErrorObject& err) {
    err.Show();
  }
}


