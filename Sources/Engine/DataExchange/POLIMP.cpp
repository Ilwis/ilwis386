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
// $Log: POLIMP.C $
// Revision 1.5  1998/09/16 17:25:24  Wim
// 22beta2
//
// Revision 1.4  1998/06/10 13:08:49  Willem
// The ILWIS 1.4 conversions will now also work from the script.
//
// Revision 1.3  1997/08/06 20:26:08  Willem
// Import now checks the polygon area (must be positiv) to reject codes
// of these polygons from the domain. Before items are added to the domain
// the codes are also checked for undefined's ("?").
//
// Revision 1.2  1997/07/25 16:43:30  Wim
// Willem:
// Drie bugjes opgelost, die met elkaar verwant waren:
// - als bij polygon/segment import een andere naam dan de default wordt
//   gekozen, kan de CRD file niet meer worden gevonden in de Store()
//   function, omdat naar de verkeerde file wordt gekeken, nl naar een
//   CRD file met de nieuwe naam; deze zal echter nooit bestaan.
//   Opgelost door een functie toe te voegen die de alfa, beta1, beta2
//   in de ODF zet (daarom werd de CRD file geopend; de nieuwe,
//   CD# of PD#, bestaat dan nog niet).
// - 1.4 polygonen met een segmenten kaart met een andere naam werden
//   niet geimporteerd. De PLG file wordt nu wel gelezen om de naam van
//   de segmenten files te weten te komen.
//
/* PolygonMapImport
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  WN    6 Aug 97    5:24 pm
*/
#include "Engine\DataExchange\SEGIMP.H"
#include "Engine\DataExchange\POLIMP.H"
#include "Engine\Map\Polygon\POLSTORE.H"
#include "Engine\Domain\dmsort.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Representation\Rpr.h"
#include "Engine\Map\Polygon\POL14.H"
#include "Engine\Base\DataObjects\Tranq.h"
#include "Headers\Hs\polygon.hs"

static void GetCodes(File& filPol, Array<String>& aPolCodes, long& iDeleted)
{
  filPol.Seek(0);
  iDeleted = 0;
  while (!filPol.fEof()) {
    poltype pt;
    filPol.Read(sizeof(poltype),&pt);
    if (pt.rArea < 0)
      continue;
    String sCode = (String)pt.sName;
    if ((sCode != "000000000000000") && (sCode != "xxxxxxxxxxxxxxx") /*&& (sCode != "NoName")*/ && (sCode != "")) {
      for (unsigned int j = 0; j < sCode.length(); ++j)
        if (sCode[j] == '_') sCode[j] = ' ';
      unsigned long i = 0;
      for (;i < aPolCodes.iSize(); i++) {
        if (aPolCodes[i] == sCode)
          break;
      }
      if (i == aPolCodes.iSize())
        aPolCodes &= sCode;
    }
    else
      iDeleted++;   // Huh? Deleted polygons don not exist!
  }
}  

void PolygonMapImport::GetImportInfo(const FileName& fn, DomainType& dmt, ValueRange& vr,
                                     Array<String>& aPolCodes)
{
  File filPol(FileName(fn, ".pol"), facRO);
  long iDeleted;
  GetCodes(filPol, aPolCodes, iDeleted);
  // check on values
  bool fDec = false;
  double r, rMin = DBL_MAX, rMax = -DBL_MAX;
  for (unsigned long i=0; i < aPolCodes.iSize(); i++) {
    r = aPolCodes[i].rVal();
    if (r == rUNDEF)
      break;
    if (!fDec && strchr(aPolCodes[i].scVal(), '.') != 0)
      fDec = true;
    if (r < rMin)
      rMin = r;  
    if (r > rMax)
      rMax = r;  
  }
  if (r != rUNDEF) {
    dmt = dmtVALUE;
    if (fDec)
      vr = ValueRange(rMin, rMax, 0.01);
    else
      vr = ValueRange(longConv(rMin), longConv(rMax));
  }  
  else {
    dmt = dmtCLASS;
    vr = ValueRange();
  }  
}

static void StorePolCodes(const PolygonMap& pm, Tranquilizer& trq)
{
  trq.SetText(SPOLTextStorePolygonCodes);
  File filPolCode(FileName(pm->fnObj, ".pc#"), facRW);
  File filPol(FileName(pm->fnObj, ".pl#"), facRW);
  if (pm->fUseReals())  {
    double* rCodes = new double[pm->iFeatures()];
    for (long i=0; i< pm->iFeatures(); ++i) {
      if (trq.fAborted())
        UserAbort(); // i.e. throw ErrorUserAbort()
      filPol.Seek(i*sizeof(poltype));
      poltype pt;
      filPol.Read(sizeof(poltype),&pt);
      String sCode = (String)pt.sName;
      for (unsigned int j = 0; j < sCode.length(); ++j)
        if (sCode[j] == '_') sCode[j] = ' ';
      if (sCode == "000000000000000" || sCode == "xxxxxxxxxxxxxxx")
        rCodes[i] = rUNDEF;
      else   
        rCodes[i] = sCode.rVal();
      if (pt.rArea == 0)
        rCodes[i] = rUNDEF;
      if (rCodes[i] == rUNDEF) {
        pt.rArea = -1;
        filPol.Seek(i*sizeof(poltype));
        filPol.Write(sizeof(poltype),&pt);
      }  
    }  
    filPolCode.Seek(0);
    filPolCode.Write(sizeof(double)*pm->iFeatures(),rCodes);
    delete [] rCodes;
  } 
  else {
    long* iCodes = new long[pm->iFeatures()];
    for (long i=0; i < pm->iFeatures(); ++i) {
      if (trq.fUpdate(i, pm->iFeatures()))
        UserAbort(); // i.e. throw ErrorUserAbort()
      filPol.Seek(i*sizeof(poltype));
      poltype pt;
      filPol.Read(sizeof(poltype),&pt);
      String sCode = (String)pt.sName;
      for (unsigned int j = 0; j < sCode.length(); ++j)
        if (sCode[j] == '_') sCode[j] = ' ';
      if (sCode == "000000000000000" || sCode == "xxxxxxxxxxxxxxx") 
        iCodes[i] = iUNDEF;
      else {
        long iRaw = pm->dvrs().iRaw(sCode);
        iCodes[i] = iRaw;
      }  
      if (pt.rArea == 0)
        iCodes[i] = iUNDEF;
      if (iCodes[i] == iUNDEF) {
        pt.rArea = -1;
        filPol.Seek(i*sizeof(poltype));
        filPol.Write(sizeof(poltype),&pt);
      }  
    }  
    filPolCode.Seek(0);
    filPolCode.Write(sizeof(long)*pm->iFeatures(),iCodes);
/*    // calc pol areas
    trq.SetText("Calc polygon areas");
    PolygonMapStore* pms = pm->ppms();
    for (i=0; i < pm->iFeatures(); ++i) {
      if (trq.fUpdate(i, pm->iFeatures()))
        UserAbort(); // i.e. throw ErrorUserAbort()
      if (iCodes[i] == iUNDEF) {
        Polygon pl = pm->getFeature(i);
        pl.Area(-1);
      }
//      pms->CalcLengthAndAreaAndBounds(i+1);
    }*/
    delete [] iCodes;
  }  
}

static int iCompPolCodes(const String* a, const String* b)
{
  return _stricmp((*a).scVal(), (*b).scVal());
}

void PolygonMapImport::import(const FileName& fn, const FileName& fnOut)
{
  FileName fnNew = fnOut;
  if (fnOut.sFullPath() == "")
    fnNew = fn;
  DomainType dmt;
  ValueRange vr;
  Array<String> aPolCodes;
  GetImportInfo(fn, dmt, vr, aPolCodes);
  if (dmt == dmtVALUE)
    PolygonMapImport::import(fn, fnNew, dmtVALUE, FileName("value", ".dom"), vr, CoordSystem());
  else
    PolygonMapImport::import(fn, fnNew, dmtCLASS, FileName::fnUnique(FileName(fn, ".dom")), vr, CoordSystem());
}

void PolygonMapImport::import(const FileName& fn, const FileName& fnNew, DomainType dmt, const FileName& fnDom,
                              const ValueRange& vr, const CoordSystem& cs,
                              const String& sDesc)
{
  FileName fnDomNew, fnRprNew;
  try {
    Tranquilizer trq;
    trq.SetTitle(String(SPOLTitleImport14Polygons_S.scVal(), fn.sFile));
    trq.Start();

    File filPLG(FileName(fn, ".PLG", true));
    String sSegName;
    filPLG.ReadLnAscii(sSegName);  // skip status
    filPLG.ReadLnAscii(sSegName);  // get the name of the segment map; may differ from polmap

    // copy data files
    FileName fnSeg14(sSegName);
    FileName fnData14 = fn;
    FileName fnData20 = fnNew;

    trq.fText(SPOLTextCreateODFPol);
    FileName fnPol(fnNew);
    fnPol.sExt = ".mpa";
    { 
      Domain dm = Domain("string");
      CoordSystem cs;
      CoordBounds cb;
      PolygonMapImport pm(fnPol, cs, cb, dm);
      pm.Store();
      pm.WriteAlfaBeta(fnSeg14);
      pm.sDescription = sDesc;
    }
    fnSeg14.sExt = ".seg";
    fnData20.sExt = ".ps#";
    if (!File::fCopy(fnSeg14, fnData20, trq))
      UserAbort();
    fnSeg14.sExt = ".crd";
    fnData20.sExt = ".pd#";
    if (!File::fCopy(fnSeg14, fnData20, trq))
      UserAbort();
    fnData14.sExt = ".pol";
    fnData20.sExt = ".pl#";
    if (!File::fCopy(fnData14, fnData20, trq))
      UserAbort();
    fnData14.sExt = ".top";
    fnData20.sExt = ".tp#";
    if (!File::fCopy(fnData14, fnData20, trq))
      UserAbort();

    PolygonMap pm(fnPol);

    Array<String> aPolCodes;
    File filPol(FileName(pm->fnObj, ".pl#"), facRO);
    trq.fText(SPOLTextRetrievePolygonCodes);
    long iDeleted;
    GetCodes(filPol, aPolCodes, iDeleted);
  //  IlwisObjectPtr::WriteElement("PolygonMapStore", "DeletedPolygons", fnPol, String("%li",iDeleted));
    Domain dm;
    if (File::fExist(fnDom)) // use it 
      dm = Domain(fnDom);
    else if (dmt == dmtVALUE) 
      dm = Domain(FileName("value", ".dom"));
    else {
      trq.fText(SPOLTextCreateNewDomain);
//      typedef int (*iFncCmp)(const void*, const void*);
//      qsort(aPolCodes.buf(), aPolCodes.iSize(), sizeof(String), (iFncCmp)iCompPolCodes);
    //  FileName fnDom = FileName::fnUnique(FileName(fn, ".DOM"));
      dm = Domain(fnDom, aPolCodes.iSize(), dmt);
      fnDomNew = fnDom;
      fnRprNew = fnDom;
      fnRprNew.sExt = ".rpr";
      DomainSort *dsrt = dm->pdsrt();
      dsrt->dsType = DomainSort::dsMANUAL;
      for (unsigned long i=0;i < aPolCodes.iSize(); i++)
        if (aPolCodes[i] != sUNDEF) {
          dsrt->SetVal(i+1, aPolCodes[i]);
          dsrt->SetOrd(i+1, i+1);
        }
      dsrt->dsType = DomainSort::dsALPHA;
      dsrt->SortAlphabetical();
    }  
    pm->SetDomainValueRangeStruct(DomainValueRangeStruct(dm, vr));
    StorePolCodes(pm, trq);

  }
  catch (const ErrorObject& err) {
    err.Show();
    // delete files
    _unlink(FileName(fnNew, ".mpa").sFullName().scVal());
    _unlink(FileName(fnNew, ".ps#").sFullName().scVal());
    _unlink(FileName(fnNew, ".pd#").sFullName().scVal());
    _unlink(FileName(fnNew, ".pl#").sFullName().scVal());
    _unlink(FileName(fnNew, ".tp#").sFullName().scVal());
    _unlink(FileName(fnNew, ".pc#").sFullName().scVal());
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
  }
}  

void PolygonMapImport::WriteAlfaBeta(const FileName& fn) {
  const_cast<FileName&>(fn).sExt = ".crd";
  File filCrd(fn);
  float flAlfa, flBeta1, flBeta2;
  filCrd.Seek(0);
  filCrd.Read(sizeof(float), &flAlfa);
  filCrd.Read(sizeof(float), &flBeta1);
  filCrd.Read(sizeof(float), &flBeta2);
  WriteElement("SegmentMap", "Alfa",  flAlfa );
  WriteElement("SegmentMap", "Beta1", flBeta1);
  WriteElement("SegmentMap", "Beta2", flBeta2);
}

void PolygonMapImport::Store()
{
  FileName fn = fnObj;
  fn.sExt = ".pl#";
  objtime = ObjectTime(fn);
  PolygonMapPtr::Store();
  WriteElement("PolygonMap", "Type", "PolygonMapStore");
  WriteElement("PolygonMapStore", "DataPol", fn);
  fn.sExt = ".pc#";
  WriteElement("PolygonMapStore", "DataPolCode", fn);
  File filPolCode(fn, facCRT);
  fn.sExt = ".tp#";
  WriteElement("PolygonMapStore", "DataTop", fn);
  File filTop(fn);
  toptype Top;
  filTop.Seek(0);
  filTop.Read(sizeof(toptype),&Top);
  WriteElement("PolygonMap", "Polygons", (long)Top.iFwd);
  WriteElement("PolygonMapStore", "Status", 1L);
  fn.sExt = ".ps#";
  WriteElement("SegmentMapStore", "DataSeg", fn);
  fn.sExt = ".pd#";
  WriteElement("SegmentMapStore", "DataCrd", fn);
/*  fn.sExt = ".crd";
  File filCrd(fn);
  float flAlfa, flBeta1, flBeta2;
  filCrd.Seek(0);
  filCrd.Read(sizeof(float), &flAlfa);
  filCrd.Read(sizeof(float), &flBeta1);
  filCrd.Read(sizeof(float), &flBeta2);
  WriteElement("SegmentMap", "Alfa",  flAlfa );
  WriteElement("SegmentMap", "Beta1", flBeta1);
  WriteElement("SegmentMap", "Beta2", flBeta2);*/
  WriteElement("SegmentMapStore", "DataSegCode", (char*)0);
  _unlink(FileName(fn, ".sc#").sFullName().scVal());
}


