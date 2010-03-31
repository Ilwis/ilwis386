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
/*$Log: /ILWIS 3.0/Import14/Pntimp.cpp $
 * 
 * 7     30/10/00 12:56 Willem
 * Detection for possible attribute table did not work, although
 * expression was correct. Code split up to make it work
 * 
 * 6     16/03/00 16:37 Willem
 * - Replaced the obsolete trq.Message() function with MessageBox()
 * function
 * 
 * 5     2/03/00 14:38 Willem
 * Table and Point import from Ilwis 1.4 now translate a sequence of an X
 * and a Y column into a Coordinate column
 * 
 * 4     21-01-00 12:51 Hendrikse
 * Changed colX and colY reference to use colCoord member
 * 
 * 3     30/09/99 8:50 Willem
 * Adjusted comments to compile properly
 * 
 * 2     9/29/99 10:23a Wind
 * added case insensitive string comparison
*/
//Revision 1.6  1998/10/20 12:08:31  Wim
//In import() read names in ArrayLarge instead of Array
//
//Revision 1.5  1998-09-16 18:25:24+01  Wim
//22beta2
//
//Revision 1.4  1998/06/10 13:08:49  Willem
//The ILWIS 1.4 conversions will now also work from the script.
//
//Revision 1.3  1997/09/24 18:52:14  Wim
//Removed superfluous setting of domain values
//
//Revision 1.2  1997-09-24 20:00:13+02  janh
//Added Tranquilizer statements at many places
//Added 3 X fErase = true resp false
//
/* PointMapImport
   Copyright Ilwis System Development ITC
   may 1995, by Jelle Wind
	Last change:  WK   20 Oct 98    1:00 pm
*/

#include "Engine\DataExchange\PNTIMP.H"
#include "Engine\Map\Point\PNTSTORE.H"
#include "Engine\DataExchange\Tblimp.h"
#include "Engine\Table\tbl.h"
#include "Engine\Table\tblstore.h"
#include "Engine\Domain\dm.h"
#include "Engine\Domain\dmsort.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Headers\Hs\point.hs"
#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\Engine.h"

void PointMapImport::GetImportInfo(const FileName& fn, bool& fNameCol, bool& fAttTablePossible)
{
	Array<ValueRange> avr;
	Array<String> asColNames;
	TableImport::GetImportInfo(fn, fNameCol, asColNames, avr);
	if (fNameCol) 
	{
		for (unsigned int i=0; i < asColNames.iSize(); ++i)
			if (fCIStrEqual(asColNames[i], "Name") && avr[i].fValid())  
			{
			  getEngine()->Message(SPNTErrNameColCannotBeValue.scVal(),
                                 String(SPNTTitleImport14Points.scVal(), fn.sFile).scVal(), 
                                 MB_OK | MB_ICONEXCLAMATION);
				UserAbort();
			}
	}
	bool fDum = (fNameCol && (asColNames.iSize() > 2));  // bool expression split up to overcome compiler
	fAttTablePossible = (!fNameCol && (asColNames.iSize() > 3)) || fDum; // problem (false || true became false!)
}

void PointMapImport::import(const FileName& fn, const FileName& fnOut)
{
  FileName fnNew = fnOut;
  if (fnOut.sFullPath() == "")
    fnNew = fn;
  bool fNameCol, fAttTablePossible;
  Array<String> asColNames;
  PointMapImport::GetImportInfo(fn, fNameCol, fAttTablePossible);
  FileName fnAtt;
  if (fAttTablePossible)
    fnAtt = FileName::fnUnique(FileName(fnNew,".tbt", true));
//  if (fNameCol)  // options: dmtID, dmtVALUE
//    PointMapImport::import(fn, fnNew, dmtCLASS, FileName::fnUnique(FileName(fn, ".dom")), CoordSystem(), fnAtt);
//  else
    PointMapImport::import(fn, fnNew, dmtID, FileName::fnUnique(FileName(fn, ".dom")), CoordSystem(), fnAtt);
}

void PointMapImport::import(const FileName& fn, const FileName& fnNew, DomainType dmt, const FileName& fnDom,
                            const CoordSystem& cs, const FileName& fnAttTbl, const String& sDesc)
{
  FileName fnDomNew, fnRprNew;
  try {
    Tranquilizer trq;
    trq.SetTitle(String(SPNTTitleImport14Points.scVal(), fn.sFile));
    trq.Start();
    if (trq.fText(SPNTTextGetImportInfo))
      DummyError();
    bool fNameCol, fAttTablePossible;
    PointMapImport::GetImportInfo(fn, fNameCol, fAttTablePossible);
    FileName fnMap = FileName(fnNew, ".mpp");
    {
      Array<String> asColNames;
      Array<ValueRange> avr;
      TableImport::GetImportInfo(fn, fNameCol, asColNames, avr);
      Array<FileName> afnDmCol;
      for (unsigned int i=0; i < avr.iSize(); ++i) 
        if (!avr[i].fValid())
				{
					if (fCIStrEqual(asColNames[i], "Coordinate"))
						afnDmCol &= FileName("unknown", ".csy", true);
					else
						afnDmCol &= FileName("string", ".dom", true);
				}
        else
          afnDmCol &= FileName("value", ".dom", true);
      // only use x, y and name columns:  
      for (unsigned int i=0; i < asColNames.iSize(); ++i)
        if ((asColNames[i] != "Coordinate") && (asColNames[i] != "Name"))
          afnDmCol[i] = FileName();
      TableImport::import(fn, fnMap, ".mpp", dmtNONE, FileName(), afnDmCol, avr);
    }

    Table tbl;
    tbl.SetPointer(new TablePtr(fnMap, String()));
    tbl->fErase = true;
    tbl->Load(); // load columns
    //trq.SetText(SCVTextReadPntRecords);
    trq.SetText(SPNTTextReadPointRecords);
    Domain dm;
    if (!fNameCol) {
      dm = Domain(FileName::fnUnique(fnDom), tbl->iRecs(), dmt, "pnt");
      dm->fErase = true;
      fnDomNew = fnDom;
      fnRprNew = fnDom;
      fnRprNew.sExt = ".rpr";
//      DomainSort * dsrt = dm->pdsrt();
//      dsrt->dsType = DomainSort::dsMANUAL;
      Column colName = tbl->colNew("Name", dm);
      long iRecs = tbl->iRecs();
      for (long i=1; i<=iRecs; i++)  {
//        dsrt->SetVal(i, String("pnt %li", i));
//      dsrt->dsType = DomainSort::dsALPHA;
//      dsrt->SortAlphabetical();
        if (trq.fUpdate(i, iRecs))
          DummyError();
//      for (i=1; i<=tbl->iRecs(); i++)
//        colName->PutVal(i, String("pnt %li", i));
        colName->PutRaw(i, i);
      }
    }
    else {
      if (File::fExist(fnDom)) {
        dm = Domain(fnDom);
        dm->fErase = true;
      }
      Column colName = tbl->col("Name");
      colName->fErase = true;
      const_cast<String&>(colName->sNam) = "_Name";
      if (dmt == dmtCLASS || dmt == dmtID) {
        if (!dm.fValid()) {
          // fill domain
          trq.SetText(SPNTTextReadFromNameColumn);
          ArrayLarge<String> asValues;
          long iRecs = tbl->iRecs();
          for (long i=1; i<=iRecs; i++) {
            if (trq.fUpdate(i, iRecs))
              DummyError();
            String sVal = colName->sValue(i,0);
			unsigned int j=0;
            for (; j < asValues.iSize(); j++)
              if (asValues[j] == sVal)
                break;
            if (j == asValues.iSize())
              asValues &= sVal;
          }  
          dm = Domain(fnDom, asValues.iSize(), dmt);
          dm->fErase = true;
          fnDomNew = fnDom;
          fnRprNew = fnDom;
          fnRprNew.sExt = ".rpr";
          DomainSort * dsrt = dm->pdsrt();
          dsrt->dsType = DomainSort::dsMANUAL;
          trq.SetText(SPNTTextSetDomainValues);
          for (unsigned int j=0; j < asValues.iSize(); j++) {
            if (trq.fUpdate(j, asValues.iSize()))
              DummyError();
            dsrt->SetVal(j+1, asValues[j]);
          }
        }
        DomainSort * dsrt = dm->pdsrt();
        Column colNew = tbl->colNew("Name", dm);
        dsrt->dsType = DomainSort::dsMANUAL;
        trq.SetText(SPNTTextPutRawValues);
        for (long i=1; i<=tbl->iRecs(); i++) {
          if (trq.fUpdate(i, tbl->iRecs()))
            DummyError();
          String sVal = colName->sValue(i,0);
          colNew->PutRaw(i, dsrt->iRaw(sVal));
        }
      }
      else { // dmtVALUE 
        bool fReal = false;
  //      if (trq.fText(String("With NameCol, dmtValue Search dot")))
//          DummyError();
        for (long i=1; i<=tbl->iRecs(); i++)
          if (strchr(colName->sValue(i,0).scVal(), '.')) {
            fReal = true;
            break;
          }
        ValueRange vr;
        if (fReal)   {
//          if (trq.fSetText(String("With NameCol, dmtValue, reals")))
//            DummyError();
          vr = ValueRange(-1e200,1e200,0);
        }
        else {
//          if (trq.fText(String("With NameCol, dmtValue, integers")))
//            DummyError();
          vr = ValueRange(-LONG_MAX+1L, LONG_MAX-1L);
        }
        DomainValueRangeStruct dvrs = vr;
        Column colNew = tbl->colNew("Name", dvrs);
        for (int i=1; i<=tbl->iRecs(); i++)  {
          if (trq.fUpdate(i, tbl->iRecs()))
            DummyError();
          colNew->PutVal(i, colName->sValue(i,0));
        }
        dm = colNew->dm();
      }
      tbl->RemoveCol(colName);
    }
    DomainValueRangeStruct dvrs(dm);
    Column colName = tbl->col("Name");
    if (colName.fValid())
      dvrs = colName->dvrs();
    PointMapImport map(fnMap, tbl, dvrs, cs);
    map.fErase = true;
    map.sDescription = sDesc;
    map.Store();
    _unlink(FileName(fn, ".BAK").sFullName().scVal()); // an exact copy of the .PNT file
    // has been made by function TableTbl::Store() when importing the table
    if (fnAttTbl.fValid()) {
      Array<String> asColNames;
      Array<ValueRange> avr;
      TableImport::GetImportInfo(fn, fNameCol, asColNames, avr);
      Array<FileName> afnDmCol;
      for (unsigned int i=0; i < avr.iSize(); ++i) 
        if (!avr[i].fValid())
				{
					if (fCIStrEqual(asColNames[i], "Coordinate"))
						afnDmCol &= FileName("unknown", ".csy", true);
					else
						afnDmCol &= FileName("string", ".dom", true);
				}
        else
          afnDmCol &= FileName("value", ".dom", true);
      for (unsigned i=0; i < asColNames.iSize(); ++i)
        if (fCIStrEqual(asColNames[i], "X") || fCIStrEqual(asColNames[i],"Y") || fCIStrEqual(asColNames[i], "Name"))
          afnDmCol[i] = FileName();
      FileName fnAttTblCopy(fnAttTbl, ".tbl", true);  
      File::Copy(fn, fnAttTblCopy);
      TableImport::import(fnAttTblCopy, fnAttTblCopy, ".tbt", dmt, dm->fnObj, afnDmCol, avr);
      _unlink(fnAttTblCopy.sFullName().scVal());
      ObjectInfo::WriteElement("BaseMap", "AttributeTable", fnMap, FileName(fnAttTbl, ".tbt", true).sRelative(true, fnMap.sPath()));
    }
    map.fErase = false;
    tbl->fErase = false;
    dm->fErase = false;
  }  
  catch (const ErrorObject& err) {
    err.Show();
    // delete files
    _unlink(FileName(fnNew, ".mpp").sFullName().scVal());
    _unlink(FileName(fnNew, ".pn#").sFullName().scVal());
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

PointMapImport::PointMapImport(const FileName& fn, const Table& table, 
                               const DomainValueRangeStruct& dvrs, const CoordSystem& cs)
: PointMapPtr(fn, cs, cs->cb, dvrs, table->iRecs())
{

	long iNr = table->iRecs();;
	Column colCrd = table->col("Coordinate");
	Column colValue = table->col("Name");
	
	for (long i = 0; i < iNr; ++i) 
	{
		pms->SetPoint(colCrd->cValue(i), i, colValue);
	}
  
//  pms->tbl = table;
////  _iPoints = tbl->iRecs();
////  pms->colX = table->col("X");
////  pms->colY = table->col("Y");
//	pms->colCoord = table->col("Coordinate");
///*
//  colXY = tbl->col("XY");
//  if (!colXY.fValid()) {
//    Column colX = tbl->col("X");
//    Column colY = tbl->col("Y");
//    colXY = tbl->colNew("XY", Domain(FileName(), cs));
//    long iNr = iPnt();
//    Coord crd;
//    for (long i = 1; i <= iNr; ++i) {
//      crd.x = colX->rValue(i);
//      crd.y = colY->rValue(i);
//      colXY->PutVal(i, crd);
//    }
//    colX->fErase = true;
//    colY->fErase = true;
//  }  
//*/  
//  pms->colVal = table->col("Name");
  Coord crdMin, crdMax;
  Bounds(crdMin, crdMax);
  if (!crdMin.fUndef() && !crdMax.fUndef()) {
    crdMin.x -= 0.05 * (crdMax.x - crdMin.x);
    crdMax.x += 0.05 * (crdMax.x - crdMin.x);
    crdMin.y -= 0.05 * (crdMax.y - crdMin.y);
    crdMax.y += 0.05 * (crdMax.y - crdMin.y);
    cbOuter = CoordBounds(crdMin, crdMax);
  }
  objtime = table->objtime;
}


void PointMapImport::Store()
{
  pms->Store();
  PointMapPtr::Store();
  WriteElement("PointMap", "Type", "PointMapStore");
}


