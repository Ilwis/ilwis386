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
// $Log: SEGIMP.C $
// Revision 1.4  1998/09/16 17:25:24  Wim
// 22beta2
//
// Revision 1.3  1998/06/10 13:08:49  Willem
// The ILWIS 1.4 conversions will now also work from the script.
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
/* SegmentMapImport
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  WN   25 Jul 97    4:54 pm
*/
#include "Engine\DataExchange\SEGIMP.H"
#include "Engine\Map\Segment\Seg.h"
#include "Engine\Map\Segment\SEG14.H"
#include "Engine\Domain\dmsort.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Representation\Rpr.h"
#include "Headers\Hs\segment.hs"

char lstring15::sTmp[16];
/*
struct lstring15  // compatible with ILWIS 1.4 LString(15)
{
  unsigned char data[16];
};

struct crdtype        // compatible with ILWIS 1.4 record
{
  short x, y;
};

struct minmax  // compatible with ILWIS 1.4 record
{
  short MinX, MinY, MaxX, MaxY;
};

struct segtype        // compatible with ILWIS 1.4 record
{
  lstring15 code;
  crdtype fst, lst;
  minmax mm;
  long fstp, lstp;
};
*/
//static Array<String> aSegCodes;

//char lstring15::sTmp[16];

static int iCompSegCodes(const String* a, const String* b)
{
  return _stricmp((*a).c_str(), (*b).c_str());
}

static void GetCodes(File& filSeg, ArrayLarge<String>& aSegCodes, long& iDeleted, Tranquilizer* trq)
{
  filSeg.Seek(sizeof(segtype));
  iDeleted = 0;
  if (0 != trq)
    trq->SetText(TR("Scan file for codes"));
  long i=0;
  long iNrSeg = filSeg.iSize() / sizeof(segtype) - 1;
  filSeg.KeepOpen(true);
  while (!filSeg.fEof()) {
    segtype st;
    filSeg.Read(sizeof(segtype),&st);
    if (0 != trq)
      if (trq->fUpdate(++i, iNrSeg))
        UserAbort(); // i.e. throw ErrorUserAbort()
    if (st.fstp < 0)
      iDeleted++;
    String sCode = (String)st.code;
    if (sCode.length() == 0)
      continue;
	long i=0;
    for ( ;i < aSegCodes.iSize(); i++) {
      if (aSegCodes[i] == sCode)
        break;
    }
    if (i == aSegCodes.iSize())
      aSegCodes &= sCode;
  }
}  

void SegmentMapImport::GetImportInfo(const FileName& fn, DomainType& dmt, ValueRange& vr,
                                     ArrayLarge<String>& aSegCodes, Tranquilizer* trq)
{
  File filSeg(FileName(fn, ".seg"), facRO);
  long iDeleted;
  GetCodes(filSeg, aSegCodes, iDeleted, trq);
  // check on values
  bool fDec = false;
  double r, rMin = DBL_MAX, rMax = -DBL_MAX;
  for (long i=0; i < aSegCodes.iSize(); i++) {
    r = aSegCodes[i].rVal();
    if (r == rUNDEF)
      break;
    if (!fDec && strchr(aSegCodes[i].c_str(), '.') != 0)
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

void SegmentMapImport::import(const FileName& fn, const FileName& fnOut)
{
  FileName fnNew = fnOut;
  if (fnOut.sFullPath() == "")
    fnNew = fn;
  DomainType dmt;
  ValueRange vr;
  ArrayLarge<String> aSegCodes;
  GetImportInfo(fn, dmt, vr, aSegCodes, 0);
  if (dmt == dmtVALUE)
    SegmentMapImport::import(fn, fnNew, dmtVALUE, FileName("value", ".dom"), vr, CoordSystem());
  else
    SegmentMapImport::import(fn, fnNew, dmtCLASS, FileName::fnUnique(FileName(fn, ".dom")), vr, CoordSystem());
}    

void SegmentMapImport::StoreSegCodes(const SegmentMapPtr* ptr, Tranquilizer& trq)
{
  File filSegCode(FileName(ptr->fnObj, ".sc#"), facRW);
  File filSeg(FileName(ptr->fnObj, ".sg#"), facRO);
  trq.SetText(TR("Store segment codes"));
  if (ptr->fUseReals())  {
    double* rCodes = new double[ptr->iFeatures()];
    for (long i=0; i < ptr->iFeatures(); ++i) {
      if (trq.fUpdate(i, ptr->iFeatures()))
        UserAbort(); // i.e. throw ErrorUserAbort()
      filSeg.Seek((i+1)*sizeof(segtype));
      segtype st;
      filSeg.Read(sizeof(segtype),&st);
      if (trq.fAborted())
        UserAbort(); // i.e. throw ErrorUserAbort()
      String sCode = (String)st.code;
      rCodes[i] = sCode.rVal();
    }  
    double rDummy = rUNDEF;
    filSegCode.Write(sizeof(double),&rDummy); // first is dummy
    filSegCode.Write(sizeof(double)*ptr->iFeatures(),rCodes);
    delete [] rCodes;
  } 
  else {
    long* iCodes = new long[ptr->iFeatures()];
    for (long i=0; i < ptr->iFeatures(); ++i) {
      if (trq.fUpdate(i, ptr->iFeatures()))
        UserAbort(); // i.e. throw ErrorUserAbort()
      filSeg.Seek((i+1)*sizeof(segtype));
      segtype st;
      filSeg.Read(sizeof(segtype),&st);
      if (trq.fAborted())
        UserAbort(); // i.e. throw ErrorUserAbort()
      String sCode = (String)st.code;
      long iRaw = ptr->dvrs().iRaw(sCode);
      iCodes[i] = iRaw;
    }  
    long iDummy = iUNDEF;
    filSegCode.Write(sizeof(long),&iDummy); // first is dummy
    filSegCode.Write(sizeof(long)*ptr->iFeatures(),iCodes);
    delete [] iCodes;
  }  
}

void SegmentMapImport::import(const FileName& fn, const FileName& fnNew, DomainType dmt, const FileName& fnDom,
                              const ValueRange& vr, const CoordSystem& cs, const String& sDesc)
{
  FileName fnDomNew, fnRprNew;
  try {
    Tranquilizer trq;
    trq.SetTitle(String(TR("Importing 1.4 segment map '%S'").c_str(), fn.sFile));
    trq.Start();
    long i;
    FileName fnSeg(fnNew);

    trq.fText(TR("Creating object definition file"));
  //if (File::fExist(FileName(fn, ".pol")))
  //  return; // polygon map should be imported
    fnSeg.sExt = ".mps";
    {
      Domain dm = Domain("string");
      SegmentMapImport sm(fnSeg, cs, cs->cb, dm);
      sm.sDescription = sDesc;
      sm.Store();
      sm.WriteAlfaBeta(fn);    // write correct alfa, beta1, beta2
    }
    // copy data files
    FileName fnData14 = fn;
    FileName fnData20 = fnSeg;
    fnData14.sExt = ".seg";
    fnData20.sExt = ".sg#";
    if (!File::fCopy(fnData14, fnData20, trq))
      UserAbort();
    fnData14.sExt = ".crd";
    fnData20.sExt = ".cd#";
    if (!File::fCopy(fnData14, fnData20, trq))
      UserAbort();

    SegmentMap sm(fnSeg);

    File filSeg(FileName(fnSeg, ".sg#"), facRO);
    ArrayLarge<String> aSegCodes;
    long iDeleted;
    trq.fText(TR("Retrieve segment codes"));
    GetCodes(filSeg, aSegCodes, iDeleted, &trq);
    ObjectInfo::WriteElement("SegmentMapStore", "DeletedSegments", fnSeg, String("%li",iDeleted));
  
    Domain dm;
    if (File::fExist(fnDom)) // use it 
      dm = Domain(fnDom);
    else if (dmt == dmtVALUE) 
      dm = Domain(FileName("value", ".dom"));
    else {
      trq.fText(TR("Create new domain"));
      dm = Domain(fnDom, aSegCodes.iSize(), dmt);
      fnDomNew = fnDom;
      fnRprNew = fnDom;
      fnRprNew.sExt = ".rpr";
      DomainSort *dsrt = dm->pdsrt();
      dsrt->dsType = DomainSort::dsMANUAL;
      for (i=0;i < aSegCodes.iSize(); i++) {
        if (trq.fAborted())
          UserAbort(); // i.e. throw ErrorUserAbort()
        dsrt->SetVal(i+1, aSegCodes[i]);
        dsrt->SetOrd(i+1, i+1);
      }
      dsrt->dsType = DomainSort::dsALPHA;
      dsrt->SortAlphabetical();
    }
    sm->SetDomainValueRangeStruct(DomainValueRangeStruct(dm, vr));
    StoreSegCodes(sm.ptr(), trq);
  }
  catch (const ErrorObject& err) {
    err.Show();  
    // delete files
    _unlink(FileName(fnNew, ".mps").sFullName().c_str());
    _unlink(FileName(fnNew, ".sg#").sFullName().c_str());
    _unlink(FileName(fnNew, ".cd#").sFullName().c_str());
    _unlink(FileName(fnNew, ".sc#").sFullName().c_str());
    if (fnDomNew.fValid()) {
      _unlink(fnDomNew.sFullName().c_str());
      fnDomNew.sExt = ".dm#";
      _unlink(fnDomNew.sFullName().c_str());
    }  
    if (fnRprNew.fValid()) {
      _unlink(fnRprNew.sFullName().c_str());
      fnRprNew.sExt = ".rp#";
      _unlink(fnRprNew.sFullName().c_str());
    }  
  }  
}

void SegmentMapImport::WriteAlfaBeta(const FileName& fn) {
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

void SegmentMapImport::Store()
{
  FileName fn = fnObj;
  fn.sExt = ".sg#";
  objtime = ObjectTime(fn);
  SegmentMapPtr::Store();
  WriteElement("SegmentMap", "Type", "SegmentMapStore");
  WriteElement("SegmentMapStore", "DataSeg", fn);
  File filSeg(fn);
  fn.sExt = ".sc#";
  WriteElement("SegmentMapStore", "DataSegCode", fn);
  File filSegCode(fn, facCRT);
  fn.sExt = ".cd#";
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
  segtype st;
  filSeg.Seek(0);
  filSeg.Read(sizeof(segtype),&st);
  WriteElement("SegmentMapStore", "Status", (long)st.lst.x);
  WriteElement("SegmentMapStore", "Format", (long)st.fst.y);
  WriteElement("SegmentMapStore", "Segments", (long)st.lst.y);
  WriteElement("SegmentMapStore", "Coordinates", (long)st.lstp);
}



