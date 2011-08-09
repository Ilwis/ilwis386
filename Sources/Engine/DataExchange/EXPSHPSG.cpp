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
/*$Log: /ILWIS 3.0/Import_Export/EXPSHPSG.cpp $
 * 
 * 5     13-12-05 11:15 Willem
 * Export to shape now also exports coordinate system to .PRJ file
 * 
 * 4     16/03/00 16:06 Willem
 * Replaced the obsolete trq.Message() function with MessageBox() function
 * 
 * 3     17-06-99 3:59p Martin
 * // -> /*
 * 
 * 2     17-06-99 2:10p Martin
 * ported files to VS
//Revision 1.3  1998/09/17 09:13:06  Wim
//22beta2
//
//Revision 1.2  1997/09/16 17:26:47  janh
//Avoid export empty segment maps and send message
//
/*
  Export ArcView .shp format
  by Li Fei, March 96
  ILWIS Department ITC
	Last change:  JHE  16 Sep 97    5:41 pm
*/

#include "Headers\toolspch.h"
#include "Engine\DataExchange\Convloc.h"
#include "Engine\Domain\Dmvalue.h"
#include "Engine\Map\Point\PNT.H"
#include "Engine\Map\Segment\Seg.h"
#include "Engine\Map\Polygon\POL.H"
#include "Engine\DataExchange\SHPFILE.H"
#include "Engine\DataExchange\DBF.H"
#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\Engine.h"

void ImpExp::ExpSegToSHP( const FileName& fnObject, const FileName& fnFile ) {
  trq.SetTitle(TR("Exporting Segments to Arcview shapefile"));
  trq.SetText(TR("Processing..."));
  const SegmentMap segmap = SegmentMap(fnObject);
/*
// the following lines don't give the correct result (E4!), because the number of
// deleted is NOT always stored in the .MPS file!! Therefore the entire map is scanned
  long iNrSegs = segmap->iFeatures();
  long iNrDel = segmap->iSegDeleted();
  iNrSegs = (iNrDel == iUNDEF) ? iNrSegs : iNrSegs - iNrDel;
*/
  long iNrSegs = segmap->iFeatures();
  if (iNrSegs == 0) {
	  getEngine()->Message(String(TR("%S Map is empty, nothing has been exported").c_str(), "Segment").c_str(),
                                 TR("Exporting Segments to Arcview shapefile").c_str(), 
                                 MB_OK | MB_ICONEXCLAMATION);
    return;
  }
  long iNrDel = segmap->iSegDeleted();
  iNrSegs = (iNrDel == iUNDEF) ? iNrSegs : iNrSegs - iNrDel;
  if (iNrSegs == 0) 
	{
	  getEngine()->Message(String(TR("%S Map is empty, nothing has been exported").c_str(), "Segment").c_str(),
                                 TR("Exporting Segments to Arcview shapefile").c_str(), 
                                 MB_OK | MB_ICONEXCLAMATION);
    return;
  }
  iNrSegs = 0;
  ILWIS::Segment *seg = segmap->segFirst();
  int  i = 0;
  while (seg && seg->fValid()) {
    if (!seg->fDeleted())
      iNrSegs++;
    seg = (ILWIS::Segment *)segmap->getFeature(++i);
  }
  FileName fnSHP(fnFile, ".shp");
  FileName fnSHX(fnFile, ".shx");
  FileName fnDBF(fnFile, ".dbf");
  File fileSHP(fnSHP, facCRT);
  File fileSHX(fnSHX, facCRT);
  File fileDBF(fnDBF, facCRT);
  Shapefile_Arc shpSeg(segmap, fileSHP);
  fileSHP.Write(sizeof(shpSeg.FileHeader), &(shpSeg.FileHeader));
  fileSHX.Write(sizeof(shpSeg.FileHeader), &(shpSeg.FileHeader));
  bool fTable = segmap->fTblAtt();
  dBaseDBF DBF;
  DomainValueRangeStruct dvs = segmap->dvrs();
  if (fTable) {
    DBF = TableToDBF(segmap->tblAtt(), iNrSegs, CHANGE_INTERNAL_SPACES);
    fileDBF.Write(sizeof(DBF.Header), &DBF.Header);
    while (DBF.fFillDescriptor())
      fileDBF.Write(sizeof(DBF.Descriptor), &DBF.Descriptor);
  }
  else {
    DBF = DomainToDBF(dvs, iNrSegs, CHANGE_INTERNAL_SPACES);
    fileDBF.Write(sizeof(DBF.Header), &DBF.Header);
    fileDBF.Write(sizeof(DBF.Descriptor), &DBF.Descriptor);
  }
  fileDBF.Write(1, &DBF.bEndDescriptor);
  long iPos = fileDBF.iLoc();
  fileDBF.Seek(0);
  fileDBF.Write(sizeof(DBF.Header), &DBF.Header);
  fileDBF.Seek(iPos);
  seg = segmap->segFirst();
  long iSegCnt = 0;
  while (seg && seg->fValid()) {
    shpSeg.Update(seg);
    fileSHX.Write(sizeof(shpSeg.IndexRecord), &(shpSeg.IndexRecord));
    if (dvs.fUseReals() && dvs.fValues())
      DBF.FillRealRecord(seg->rValue());
    else
      DBF.FillRecord(seg->iValue());
    fileDBF.Write(DBF.Header.iszRecord, &DBF.carRecord[0]);
    if (trq.fUpdate(++iSegCnt, iNrSegs))
      return;
	seg = (ILWIS::Segment *)segmap->getFeature(iSegCnt);
  }
  fileDBF.Write(1, &DBF.bEndFile);
  fileSHP.Seek(0);
  fileSHX.Seek(0);
  shpSeg.SetFileHeader();
  fileSHP.Write(sizeof(shpSeg.FileHeader), &(shpSeg.FileHeader));
  shpSeg.SetIndexFileHeader();
  fileSHX.Write(sizeof(shpSeg.FileHeader), &(shpSeg.FileHeader));
  trq.fUpdate(iNrSegs, iNrSegs);
    
    // if map contains a coordinate system export this to a .prj file
    if (segmap->cs().fValid()) {
        FileName fnPRJ(fnFile, ".prj");
        ExportEsriPrj(segmap->cs()->fnObj, fnPRJ);
    }
}
