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
/* $Log: /ILWIS 3.0/Import_Export/Impidr.cpp $
 * 
 * 6     8/16/01 16:15 Willem
 * Simplified fCheckTitle() check code, now it does not break on strings
 * with null-pointers
 * 
 * 5     29/02/00 12:13 Willem
 * MessageBox is now shown using as parent the mainWindow
 * 
 * 4     8/02/00 11:45 Willem
 * String comparisons are now case insensitiv
 * 
 * 3     17-06-99 3:59p Martin
 * // -> /*
 * 
 * 2     17-06-99 2:11p Martin
 * ported files to VS
// Revision 1.7  1998/09/18 12:17:14  Willem
// The import will now correctly read the PAL file into a representation;
// the array to scan the colors from the file was never cleared.
//
// Revision 1.6  1998/05/14 15:55:15  Willem
// A series is spaces in the Idrisi 'Title' is now also considered empty, giving
// the default description
//
// Revision 1.5  1998/05/14 15:27:55  Willem
// Some code cleanup, reorganising some code.
// - Added description in map from the title of the DOC file
// - Solved problem with floating point error (bug 311)
// - GeoRef is now created (bug 342); this was actually solved in GeoRefCorners
//
// Revision 1.4  1998/03/30 12:36:20  Dick
// no change.
//
// Revision 1.3  1997/08/08 12:02:04  Dick
// Moved warning message to conv.s
//
// Revision 1.2  1997/08/08 11:31:26  Dick
// Changed to handle exception with the new converr.c
//
/* ImporIDRISI
  Import IDRISI raster
  by Dick Visser July 22, 1997
	Last change:  DV    8 Aug 97    1:02 pm
*/

/*
  Revision history:
*/
#include "Headers\toolspch.h"
#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\Engine.h"
#include "Engine\DataExchange\Convloc.h"
#include "Engine\Map\Raster\Map.h"
#include "Engine\SpatialReference\Grcornrs.h"
#include "Engine\Domain\dmclass.h"
#include "Engine\Representation\Rprclass.h"


class IDRImporter {
public:
  IDRImporter(const FileName& fn, Tranquilizer&  );
  IDRImporter::~IDRImporter();

  void IDRImporter::IDRReadDOC(const FileName& fnObject);

private:
  long IDRImporter::fCheckTitle(const String& sIn, const String& sComp, String& sOut);
  void IDRImporter::IDRWriteClrTbl(const FileName& fnObject);
  FileName     fnIdrisi;
  long         iIDRcol,iIDRrow;
  double       rIDRminX, rIDRmaxX, rIDRminY, rIDRmaxY;
  double       rConvDist;           // to convert from feet mile, etc. to Ilwis system in m.
  double       rValMin, rValMax;    // min and max values as given by IDRISI
  long         iNrClass;              // gives number of classes if appropriate
  long         iDocLn;
  Tranquilizer& trq;

  File*        filDOC;
  File*        filIMG;
  File*        filPAL;
  Domain       dm;
};


IDRImporter::IDRImporter(const FileName& fn, Tranquilizer& _trq )
       : trq(_trq), fnIdrisi(fn)
{
   filDOC = new File(fnIdrisi, facRO);
   filIMG = new File(FileName(fnIdrisi, ".IMG", true));
   filPAL = NULL;
   FileName fnPAL = FileName(fnIdrisi, ".PAL", true);
   if (File::fExist(fnPAL))
      filPAL = new File(fnPAL);
}

IDRImporter::~IDRImporter()
{
  if ( filDOC ) delete filDOC;
  if ( filIMG ) delete filIMG;
  if ( filPAL ) delete filPAL;
}


long IDRImporter::fCheckTitle(const String& sIn, const String& sComp, String& sOut)
{
	String sKey = sIn.sHead(":").sTrimSpaces().sLeft(sComp.length());
	String sValue = sIn.sTail(":").sTrimSpaces();
	if (fCIStrEqual(sKey, sComp)) 
	{
		sOut = sValue;
		return 0;
	}
	else 
	{
		String sWarning = String(SCVWarnIdrDoc.scVal(), iDocLn, filDOC->sName(), sIn, sComp);
		int iRet = getEngine()->Message(sWarning.scVal(), SCVMsgExportWarning.scVal(), MB_ICONEXCLAMATION|MB_YESNO);
		if (iRet != IDOK)
			UserAbort(filDOC->sName());
		return 1;
	}
}

void IDRImporter::IDRWriteClrTbl(const FileName& fnObject)
{
//  write color table
//  IDRISI color table is expanded from 0..63 to 0..252
//  Representation rpr = dm->rpr();
//  void SetRepresentation(const Representation& rpr)
  Representation rpr = dm->rpr();
  long iFac = 4;
  String ss;
  Array<String> asParts;
  long iCnr;
  byte iCrd, iCgr, iCbl;

  FileName fnrp = FileName(fnObject, ".RPR", true);
  Representation rp(fnrp, dm);
  RepresentationClass *prc = dynamic_cast<RepresentationClass*>( rp.ptr() );
  while (!filPAL->fEof()) {
    filPAL->ReadLnAscii(ss);
    asParts.Reset();
    Split(ss, asParts);
    iCnr = asParts[0].iVal();
    iCrd = (byte) iFac * asParts[1].iVal();
    iCgr = (byte) iFac * asParts[2].iVal();
    iCbl = (byte) iFac * asParts[3].iVal();
    
    // + 1: Classes start from so adjust for that
    prc->PutColor(iCnr + 1, Color(iCrd, iCgr, iCbl));
  }
  dm->SetRepresentation(rp);
}


void IDRImporter::IDRReadDOC(const FileName& fnObject)
{
  long iErrCnt =0;
  iDocLn = 1;
  enum {ebyte , einteger , ereal, eclass } iInType;
  String sTmp, sTmp1;
  String sIDRTitle;
  bool fIDRundef;
  float rIDRundef;

  filDOC->ReadLnAscii (sIDRTitle);
  iErrCnt += fCheckTitle(sIDRTitle, "file title", sIDRTitle);

  // data type either byte integer or real, class is asigned later
  iDocLn ++;
  String sIDRdataty;
  filDOC->ReadLnAscii (sIDRdataty);
  iErrCnt += fCheckTitle(sIDRdataty, "data type", sIDRdataty);
  if ( fCIStrEqual(sIDRdataty, "byte") ) iInType = ebyte;
  else if( fCIStrEqual(sIDRdataty, "integer") ) iInType = einteger;
  else if( fCIStrEqual(sIDRdataty, "real") ) iInType = ereal;


  String sIDRfilety;
  iDocLn ++;
  filDOC->ReadLnAscii (sIDRfilety);
  iErrCnt += fCheckTitle(sIDRfilety, "file type", sIDRfilety);
  if ( sIDRfilety != "binary" )
    throw ErrorImportExport(SCVErrNotSupported);

  iDocLn ++;
  filDOC->ReadLnAscii (sTmp);
  iErrCnt += fCheckTitle(sTmp, "columns", sTmp);
  iIDRcol = sTmp.iVal();

  iDocLn ++;
  filDOC->ReadLnAscii (sTmp);
  iErrCnt += fCheckTitle(sTmp, "rows", sTmp);
  iIDRrow = sTmp.iVal();

  String sIDRrefs;
  iDocLn ++;
  filDOC->ReadLnAscii (sIDRrefs);
  iErrCnt += fCheckTitle(sIDRrefs, "ref. system", sIDRrefs);
  //iErrCnt += (sIDRrefs != "plane");
  //To be implemented

  String sIDRrefu;
  iDocLn ++;
  filDOC->ReadLnAscii (sIDRrefu);
  iErrCnt += fCheckTitle(sIDRrefu, "ref. units", sIDRrefu);

  rConvDist = 1.0;  // to get a default value
  if (fCIStrEqual(sIDRrefu, "ft")) rConvDist = 0.3048;
  else if (fCIStrEqual(sIDRrefu, "mi")) rConvDist = 1609.344;
  else if (fCIStrEqual(sIDRrefu, "km")) rConvDist = 1000.0;
  //deg and radians to be implemented

  iDocLn ++;
  filDOC->ReadLnAscii (sTmp);
  iErrCnt += fCheckTitle(sTmp, "unit dist.", sTmp);
  // to be implemented

  iDocLn ++;
  filDOC->ReadLnAscii (sTmp);
  iErrCnt += fCheckTitle(sTmp, "min. X", sTmp);
  rIDRminX = rConvDist * sTmp.rVal();

  iDocLn ++;
  filDOC->ReadLnAscii (sTmp);
  iErrCnt += fCheckTitle(sTmp, "max. X", sTmp);
  rIDRmaxX =  rConvDist * sTmp.rVal();

  iDocLn ++;
  filDOC->ReadLnAscii (sTmp);
  iErrCnt += fCheckTitle(sTmp, "min. Y", sTmp);
  rIDRminY =  rConvDist * sTmp.rVal();

  iDocLn ++;
  filDOC->ReadLnAscii (sTmp);
  iErrCnt += fCheckTitle(sTmp, "max. Y", sTmp);
  rIDRmaxY =  rConvDist * sTmp.rVal();

  iDocLn ++;
  filDOC->ReadLnAscii (sTmp);
  iErrCnt += fCheckTitle(sTmp, "pos'n error", sTmp);
  // not used in Ilwis

  iDocLn ++;
  filDOC->ReadLnAscii (sTmp);
  iErrCnt += fCheckTitle(sTmp, "resolution", sTmp);
  // not used in Ilwis

  iDocLn ++;
  filDOC->ReadLnAscii (sTmp);
  iErrCnt += fCheckTitle(sTmp, "min. value", sTmp);
  rValMin = sTmp.rVal();

  iDocLn ++;
  filDOC->ReadLnAscii (sTmp);
  iErrCnt += fCheckTitle(sTmp, "max. value", sTmp);
  rValMax = sTmp.rVal();

  iDocLn ++;
  filDOC->ReadLnAscii (sTmp);
  iErrCnt += fCheckTitle(sTmp, "value units", sTmp);
  // controleer op classes ???

  iDocLn ++;
  filDOC->ReadLnAscii (sTmp);
  iErrCnt += fCheckTitle(sTmp, "value error", sTmp);
  // not used in Ilwis ??

  iDocLn ++;
  filDOC->ReadLnAscii (sTmp);
  iErrCnt += fCheckTitle(sTmp, "flag value", sTmp);
  // is empty if no value is given
  fIDRundef = (sTmp.length() != 0);
  if ( fIDRundef ) 
    rIDRundef = sTmp.rVal();

  iDocLn ++;
  filDOC->ReadLnAscii (sTmp);
  iErrCnt += fCheckTitle(sTmp, "flag def'n", sTmp);
  // IDRISI knows "background" or "missing data"
  // both are translated to UNDEF in Ilwis

  iDocLn ++;
  filDOC->ReadLnAscii (sTmp);
  iErrCnt += fCheckTitle(sTmp, "legend cats", sTmp);
  iNrClass = sTmp.iVal();

  // create GeoRef
  GeoRef gr;
  RowCol rc = RowCol (iIDRrow,iIDRcol);
  Coord crBotLeft  = Coord (rIDRminX,rIDRminY);
  Coord crTopRight = Coord (rIDRmaxX,rIDRmaxY);
  FileName fnGrf = FileName::fnUnique(FileName(fnObject, ".grf", true));
  gr.SetPointer(new GeoRefCorners(fnGrf, rc, true,   // not center of corners
                                  crBotLeft, crTopRight));
  gr->Updated();

  // determine domain
  ValueRange   vr;
  if ( iNrClass !=0 )  {
    iInType = eclass;
    dm = Domain(fnObject,(short) iNrClass);
    String sClass;
    for (short j = 0; j < iNrClass; j++) {
      iDocLn++;
      filDOC->ReadLnAscii (sTmp);
      iErrCnt += fCheckTitle(sTmp, "category", sClass);
      
      String sClassNr = sTmp.sSub(8, 4);   // only get the number of the catagory
      long iClassNr = sClassNr.iVal() + 1; // DomainSort starts at 1

      dm->pdc()->SetVal(iClassNr, sClass);
    }
    if ( filPAL != NULL )
      IDRWriteClrTbl(fnObject);
  }
  else {
    dm = Domain("value");
    switch (iInType) {
      case ebyte:
      case eclass:
        dm = Domain("image");
        break;
      case einteger:
        vr = ValueRange(-SHRT_MAX + 1, SHRT_MAX);
        break;
      case ereal:
        vr = ValueRange(-FLT_MAX, FLT_MAX, 2.0 * FLT_MIN);
        break;
    }
  }
  // error exit if errors found in .DOC file
  if (iErrCnt != 0) {
    FormatProblem(filDOC->sName());
  }

  Map mp = Map(fnObject, gr, gr->rcSize(), DomainValueRangeStruct(dm, vr));
  sIDRTitle = sIDRTitle.sTrimSpaces();
  if (sIDRTitle.length() > 0)
    mp->sDescription = sIDRTitle;
  else {
    mp->sDescription &= SCVText_ImportedFrom_;
    mp->sDescription &= "Idrisi";
  }
  
  short iRowCnt,iColCnt;
  trq.SetText(SCVTextProcessing);
  switch (iInType) {
    case ebyte: {
      ByteBuf bIBuf(iIDRcol);
      for (iRowCnt = 0; iRowCnt < iIDRrow ; iRowCnt++ )  {
        filIMG->Read(iIDRcol, bIBuf.buf());
        mp->PutLineRaw(iRowCnt, bIBuf);
        if (trq.fUpdate(iRowCnt, iIDRrow))
          return;
      }
      break;
    }
    case eclass: {
      ByteBuf bIBuf(iIDRcol);
      byte bNotDef = byteConv(rIDRundef);
      for (iRowCnt = 0; iRowCnt < iIDRrow ; iRowCnt++ ) {
        filIMG->Read(iIDRcol, bIBuf.buf());
        for ( iColCnt = 0; iColCnt < iIDRcol; iColCnt++ )
          if (bIBuf[iColCnt] == bNotDef)
            bIBuf[iColCnt] = 0;
          else
            bIBuf[iColCnt] += 1;
        mp->PutLineRaw(iRowCnt, bIBuf);
        if (trq.fUpdate(iRowCnt, iIDRrow))
          return;
      }
      break;
    }
    case einteger : {
      IntBuf iIBuf(iIDRcol);
      short iNotDef = shortConv(rIDRundef);
      for (iRowCnt = 0; iRowCnt < iIDRrow ; iRowCnt++ ) {
        filIMG->Read(iIDRcol * 2, iIBuf.buf());
        for ( iColCnt = 0; iColCnt < iIDRcol; iColCnt++ )
          if ( iIBuf[iColCnt] == iNotDef ) 
            iIBuf[iColCnt] = shUNDEF;
        mp->PutLineRaw(iRowCnt, iIBuf);
        if (trq.fUpdate(iRowCnt, iIDRrow))
          return;
      }
      break;
    }
    case ereal : {
      Buf<float> flIBuf(iIDRcol);
      RealBuf    rIBuf(iIDRcol);
      for ( iRowCnt = 0; iRowCnt < iIDRrow ; iRowCnt++ ) {
        filIMG->Read (iIDRcol * 4, flIBuf.buf());
        for ( iColCnt = 0; iColCnt < iIDRcol; iColCnt++ ) {
          if ( fIDRundef && (flIBuf[iColCnt] == rIDRundef ))
            rIBuf[iColCnt] = rUNDEF;
          else
            rIBuf[iColCnt] = flIBuf[iColCnt];
        }
        mp->PutLineVal(iRowCnt, rIBuf);
        if (trq.fUpdate(iRowCnt, iIDRrow))
          return;
      }
      break;
    }
  }
}

void  ImpExp::ImportIDRISI (const FileName& fnFile ,const FileName& fnObject)  {
  try {
    trq.SetTitle (SCVTitleImportIDRISI);
    IDRImporter IDRIm(fnFile, trq);

    IDRIm.IDRReadDOC(fnObject);
//    if (iLines != iUNDEF );
  }
  catch (AbortedByUser& ) {
  }
  catch (ErrorObject& err) {
    err.Show();
  }
}


