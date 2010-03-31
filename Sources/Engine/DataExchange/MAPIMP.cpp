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
// $Log: /ILWIS 3.0/Import14/MAPIMP.cpp $
 * 
 * 5     23-08-02 16:23 Willem
 * Removed completely redundant setting of the objtime. This is already
 * taken care of in IlwisObjectPtr. The line however prevented the import
 * from working, because of a sharing violation
 * 
 * 4     27-09-99 16:35 Hendrikse
 * changed fnReference into fnBackgroundMap
 * 
 * 3     9/10/99 9:02a Wind
 * comment
 * 
 * 2     9/10/99 9:00a Wind
 * change call to FileName(..) in MpiStruct::gr() 
*/
// Revision 1.5  1998/09/16 17:25:24  Wim
// 22beta2
//
// Revision 1.4  1998/06/10 13:08:49  Willem
// The ILWIS 1.4 conversions will now also work from the script.
//
// Revision 1.3  1997/09/18 21:10:51  Willem
// Multi words are not allowed, changed it back to reading first word only, but create
// unique domain items for duplicates (adding: '_n', n being a number).
//
/* MapImport
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  WN   18 Sep 97    6:03 pm
*/

#include "Engine\DataExchange\MAPIMP.H"
#include "Engine\Domain\dmclass.h"
#include "Engine\Representation\Rprclass.h"
#include "Engine\DataExchange\Tblimp.h"
#include "Engine\SpatialReference\GRNONE.H"
#include "Engine\SpatialReference\Grcornrs.h"
#include "Engine\SpatialReference\Grctppla.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Headers\Hs\map.hs"

struct MpiStruct  // as in version 1.x
{
  short iLines, iCols;
  short iMinVal, iMaxVal;
  short iMapType, iPatched;
  short iScale, iCoeff;
  float a11, a12, a21, a22, b1, b2;
  RowCol rc() { return RowCol((long)iLines, (long)iCols); }
  GeoRef gr(const FileName& fn);
  bool fNone() { return !((iCoeff == 1) && (abs(a11 * a22 - a12 * a22) > 1e-10)); }
//  Domain dm();
  double rStep() const;
  RangeReal rrMinMax();
  const char * sType();
};

GeoRef MpiStruct::gr(const FileName& fn) 
{ 
  GeoRef grf(rc());
  if ((iCoeff == 1) && (abs(a11 * a22 - a12 * a22) > 1e-10)) {
    if ((a12 == 0) && (a21 == 0)) {
      GeoRef grSmpl(CoordSystem(),rc(),a11,a12,a21,a22,b1,b2);
//      Coord crdMin = grSmpl->cConv(RowCol(0L, 0L));
//      Coord crdMax = grSmpl->cConv(rc());
      Coord crdMin;
      grSmpl->RowCol2Coord(0, 0, crdMin);
      Coord crdMax;
      grSmpl->RowCol2Coord(rc().Row, rc().Col, crdMax);
      if (crdMin.x > crdMax.x) {
        double r = crdMin.x; crdMin.x = crdMax.x; crdMax.x = r; 
      } 
      if (crdMin.y > crdMax.y) {
        double r = crdMin.y; crdMin.y = crdMax.y; crdMax.y = r; 
      } 
      grf.SetPointer(new GeoRefCorners(fn,CoordSystem(), rc(), true, // corners of corners
                                       crdMin, crdMax));
    }                 
    else {
      GeoRef grSmpl(CoordSystem(),rc(),a11,a12,a21,a22,b1,b2);
      GeoRefCTPplanar* grCtp = new GeoRefCTPplanar(fn,CoordSystem(), rc());
      // use 3 corners as tiepoints
      Coord crd = grSmpl->cConv(RowCol(0L,0L));
      grCtp->AddRec(RowCol(1L, 1L)/* add 1 to rows and cols*/, crd); 
      crd = grSmpl->cConv(RowCol(rc().Row-1L, rc().Col-1L));
      grCtp->AddRec(rc()/* add 1 to rows and cols*/, crd);
      crd = grSmpl->cConv(RowCol((long)rc().Row-1L, 0L));
      grCtp->AddRec(RowCol(rc().Row, 1L)/* add 1 to rows and cols*/, crd);
      grCtp->fnBackgroundMap = FileName(fn, ".mpr");
      grf.SetPointer(grCtp);
    }  
  } 
  return grf;
}

RangeReal MpiStruct::rrMinMax()
{
  if (iMinVal > iMaxVal)
    return RangeReal();
  double rMin = iMinVal;
  double rMax = iMaxVal;
  rMin *= rStep();
  rMax *= rStep();
  return RangeReal(rMin, rMax);
}

double MpiStruct::rStep() const
{
  double rStp = 1;
  int i;
  for (i = 0; i < iScale; ++i)
    rStp *= 10;
  for (i = 0; i > iScale; --i)
    rStp /= 10;
  return rStp;
}

const char * MpiStruct::sType()
{
  switch (iMapType) {
    case 0:
      return "bit";
    case 1:
      return "byte";
    case 2:
      return "int";
  }
  return "";
}

void MapImport::GetImportInfo(const FileName& fn, int& iMapType14, int& iScale,
                              bool& fINF, bool& fCOL, bool& fGeoRefNone, FileName& fnGrf, const String& sSearchGrfPath)
{
  // map type
  File filMPI(FileName(fn, ".mpi"));
//if (filMPI.err())
//  NotFoundError(FileName(fn, ".mpi"));
  if (!File::fExist(FileName(fn, ".mpd")))
    NotFoundError(FileName(fn, ".mpd"));
  MpiStruct mpi;
  filMPI.Read(sizeof(mpi), &mpi);
  iMapType14 = mpi.iMapType;
  iScale = mpi.iScale;
  fGeoRefNone = mpi.fNone();
  if (!fGeoRefNone) {
    try {
      String sSearch;
      if (sSearchGrfPath.length() != 0)
       sSearch = sSearchGrfPath;
      else
        sSearch = fn.sPath();
      // check georef
      GeoRef gr = GeoRef::grFindSameOnDisk(mpi.gr(FileName()), /*fn.sPath()*/sSearch);
      if (gr.fValid())
        fnGrf = gr->fnObj;
    }
    catch (const ErrorObject&) {
    }  
  }
  // INF file
  fINF = File::fExist(FileName(fn, ".inf"));
  // COL file
  fCOL = File::fExist(FileName(fn, ".col"));
}

void MapImport::import(const FileName& fn, const FileName& fnOut)
{
  FileName fnNew = fnOut;
  if (fnOut.sFullPath() == "")
    fnNew = fn;
  int iMapType, iScale;
  bool fInf, fCol, fGeoRefNone;
  FileName fnGrf;
  MapImport::GetImportInfo(fn, iMapType, iScale, fInf, fCol, fGeoRefNone, fnGrf);
  DomainType dmt;
  FileName fnDom = FileName(fnNew, ".dom");
  if (fInf) {
    if (fCol)
      dmt = dmtCLASS;
    else if (iMapType == 2)
      dmt = dmtID;  // option: dmtCLASS
    else
      dmt = dmtCLASS;  // option: dmtID
  }
  else {
    if (fCol)
      dmt = dmtPICTURE; // option: dmtCLASS
    else
      if (iMapType == 1)
        dmt = dmtIMAGE; // options: dmtVALUE, dmtCLASS, dmtID
      else
        dmt = dmtVALUE;
  }
  MapImport::import(fn, fnNew, dmt, fnDom, fnGrf);
}

void MapImport::import(const FileName& fn, const FileName& fnNew, DomainType dmt,
                       const FileName& fnDom, const FileName& fnGrf,
                       const String& sDesc)
{
  FileName fnDomNew, fnGrfNew, fnRprNew;
  FileName fnMap(fnNew);
  fnMap.sExt = ".mpr";
  FileName fnMPD(fn);
  fnMPD.sExt = ".mpd";
  try {
    Tranquilizer trq;
    trq.SetTitle(String(SMAPTextImportRasterFrom14.scVal(), fn.sFile));
    trq.Start();

    // open 1.4 mpi-file
    File filMPI(FileName(fnMPD, ".mpi"));
    MpiStruct mpi;
    filMPI.Read(sizeof(mpi), &mpi);
//  if (filMPI.err())
//    NotFoundError(FileName(fnMPD, ".mpi"));

    ValueRange vr;

    // create georef
    GeoRef gr;
    if (File::fExist(fnGrf))
      gr = GeoRef(fnGrf);
    else {
      trq.SetText(SMAPTextCreateGeoReference);
  /*    if (fnGrf.fValid()) // use user supplied name
        const_cast<FileName&>(gr->fnObj) = fnGrf;
      else if (!mpi.fNone()) // use name of map as name for georef
        const_cast<FileName&>(gr->fnObj) = FileName(fnMap, ".grf");
      gr->fChanged = true;*/
      if (fnGrf.fValid()) // use user supplied name
        gr = mpi.gr(fnGrf);
      else {
        gr = mpi.gr(FileName(fnMap, ".grf"));
        fnGrfNew = gr->fnObj;
      }  
    }

    // create domain
    Domain dm;
    if ((dmt == dmtCLASS) || (dmt == dmtID)) { // use fnDOM as domain name
      if (File::fExist(fnDom)) { // use it
        dm = Domain(fnDom);
        if (dm->pdc())
          dmt = dmtCLASS;
        else
          dmt = dmtID;  
      }  
      else if (File::fExist(FileName(fnMPD, ".inf"))) {
        trq.SetText(SMAPTextCreateDomain);
        File filINF(FileName(fnMPD, ".inf"));
//      if (filMPI.err())
//        NotFoundError(FileName(fnMPD, ".inf"));
        // determine nr records
        long iRecs = -1;
        { long i, j, l = filINF.iSize();
          char *sBuf = new char[10001];
          filINF.Seek(0);
          for (i = 0; i < l / 10000; i++) {
            filINF.Read(10000, sBuf);
            for (j = 0; j < 10000; j++)
              if (sBuf[j] == '\n') iRecs++;
          }
          long k = filINF.Read(10000, sBuf);
          for (j = 0; j < k; j++)
            if (sBuf[j] == '\n') iRecs++;
          delete sBuf;
        }
        if (trq.fUpdate(0, iRecs))
          UserAbort();
        dm = Domain(FileName(fnDom, ".dom"), iRecs, dmt);
        fnDomNew = dm->fnObj;
        fnRprNew = dm->fnObj;
        fnRprNew.sExt = ".rpr";
        String s;
        long rec = 1;
        filINF.Seek(0);
        filINF.ReadLnAscii(s); // skip header
        DomainSort* ds = dm->pdsrt();
        ds->dsType = DomainSort::dsMANUAL;
        while (!filINF.fEof()) {
          filINF.ReadLnAscii(s);
          if (trq.fUpdate(rec, iRecs))
            UserAbort();
          Array<String> as;
          Split(s, as);
          s = as[0].sTrimSpaces();
          // remove underscores
          for (unsigned short ii = 0; ii < s.length(); ii++) {
            if (s[ii] == '_') s[ii] = ' ';
          }
          String sTemp = s;
          short iDup = 1;
          while (ds->iRaw(sTemp) != iUNDEF) {
            sTemp = String("%S_%d", s, iDup++);
          }
          ds->SetVal(rec, sTemp);
          if (0 == strcmp(sTemp, "?"))
            ds->Delete(rec);
          rec++;
        }
        ds->SortRawValues();
        dm->Store();
        _unlink(FileName(fnMap, ".BAK").sFullName().scVal());
      }
      else {
        if (dmt == dmtCLASS)
          dm = Domain(FileName(fnDom, ".dom"), 255, dmt, "Class");
        else  
          dm = Domain(FileName(fnDom, ".dom"), 255, dmt, "Nr");
        fnDomNew = dm->fnObj;
        fnRprNew = dm->fnObj;
        fnRprNew.sExt = ".rpr";
      }
    }
    else if (dmt == dmtPICTURE) { // ignore domain name
      dm = Domain(FileName(fnMap, ".dom"), 256, dmtPICTURE);
      fnDomNew = dm->fnObj;
      fnRprNew = dm->fnObj;
      fnRprNew.sExt = ".rpr";
    }
    else if (dmt == dmtIMAGE) {
      dm = Domain("Image");
    }
    else { // domain value, ignore domain name
      if (mpi.iMapType == 1) {
        dm = Domain("value");
        vr = ValueRange(1,255);
      }
      else if (mpi.iMapType == 0)
        dm = Domain("bit");
      else if (mpi.iMapType == 2) {
        if (mpi.iScale == 0) {
          dm = Domain("value");
          vr = ValueRange(-32766L,32767L);
        }
        else {
          dm = Domain("value");
          double rStp = mpi.rStep();
          double rMin = -32766;
          double rMax = 32767;
          rMin *= rStp;
          rMax *= rStp;
          vr = ValueRange(rMin, rMax, rStp);
        }
      }
    }

    // create representation if necessary
    if ((dmt == dmtCLASS) || (dmt == dmtPICTURE)) {
      Representation rpr = dm->rpr();
      RepresentationClass* rprc = rpr->prc();
      fnDomNew = dm->fnObj;
      FileName fnCol(fnMPD, ".col");
      FileName fnTmp(fnCol, ".$$$");
      if (File::fExist(fnCol)) {  // use .COL to create representation
        trq.fText(SMAPTextCreateReprFromLUT);
        try {
          TableImport::import(fnCol, ".$$$");
          Table tLut(fnTmp);
          tLut->Store();
          tLut->fErase = true;
          Column cRed   = tLut["red"];
          Column cGreen = tLut["green"];
          Column cBlue  = tLut["blue"];
          if (cRed.fValid() && cGreen.fValid() && cBlue.fValid()) {
            String sHeader;
            File filCol(fnCol);
            filCol.ReadLnAscii(sHeader);
            if (trq.fAborted())
              UserAbort();
            sHeader.toLower();
            bool fRed1000   = strstr(sHeader.scVal(), "red%") != 0;
            bool fGreen1000 = strstr(sHeader.scVal(), "green%") != 0;
            bool fBlue1000  = strstr(sHeader.scVal(), "blue%") != 0;
            int red, green, blue;
            int iStart = dmt == dmtPICTURE ? 0 : 1;
            int iMax = dmt == dmtPICTURE ? 255 : dm->pdc()->iSize();
            for (int i = iStart; i <= iMax; ++i) {
              red = cRed->iValue(i);
              if (fRed1000)
                red *= 255.0/1000.0;
              green = cGreen->iValue(i);
              if (fGreen1000)
                green *= 255.0/1000.0;
              blue = cBlue->iValue(i);
              if (fBlue1000)
                blue *= 255.0/1000.0;
              rprc->PutColor(i, Color(red,green,blue));
            }
            rprc->Store();
          }
          dm->SetRepresentation(rpr);
        }
        catch (const ErrorObject& err) {
          rpr->fErase = true;
          throw err;
        }
      }
    }
    FileName fnData20 = fnMap;
    fnData20.sExt = ".mp#";
    String sData20 = fnData20.sFullName();
    {
      MapImport map(fnMap, gr, dm, sData20, mpi.sType(), mpi.rrMinMax(), vr);
      map.sDescription = sDesc;
      map.Store();
    }
    // copy data file
    File::fCopy(fnMPD, fnData20, trq);
  }
  catch (const ErrorObject& err) {
    err.Show();
    // delete files
    _unlink(FileName(fnMap, ".mpr").sFullName().scVal());
    _unlink(FileName(fnMap, ".mp#").sFullName().scVal());
    if (fnDomNew.fValid()) {
      _unlink(fnDomNew.sFullName().scVal());
      fnDomNew.sExt = ".dm#";
      _unlink(fnDomNew.sFullName().scVal());
    }  
    if (fnRprNew.fValid()) {
      _unlink(fnRprNew.sFullName().scVal());
      fnRprNew.sExt = ".rp#";
      _unlink(fnRprNew.sFullName().scVal());
    }  
    if (fnGrfNew.fValid()) {
      _unlink(fnGrfNew.sFullName().scVal());
      fnGrfNew.sExt = ".gr#";
      _unlink(fnGrfNew.sFullName().scVal());
    }  
  }
}

MapImport::MapImport(const FileName& fn, const GeoRef& gr, const Domain& dm,
          const String& sD, const String& sT, const RangeReal& rrMinMax, const ValueRange& vr)
: MapPtr(fn,gr,RowCol(),DomainValueRangeStruct(dm,vr)), sData(sD), sType(sT)
{
  SetMinMax(rrMinMax);
  SetMinMax(RangeInt(rounding(rrMinMax.rLo()), rounding(rrMinMax.rHi())));
}


void MapImport::Store()
{
  MapPtr::Store();
  WriteElement("Map", "Type", "MapStore");
  WriteElement("MapStore", "Data", FileName(sData));
  WriteElement("MapStore", "Type", sType);
}


