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
  export ILWIS to Ilwis ASCII format
  template by Willem Nieuwenhuis, oct 1996
  modified by
  ILWIS Department ITC
	Last change:  WN   15 May 97    1:59 pm
*/
#include "Headers\toolspch.h"
#include "Engine\Map\Raster\Map.h"
#include <string.h>
#include "Engine\DataExchange\Convloc.h"
#include "Headers\Hs\CONV.hs"
#include "Engine\SpatialReference\Gr.h"
#include "Engine\Domain\dmsort.h"
#include "Engine\Domain\Dmvalue.h"


class ASCExporter{
  public:
    ASCExporter(const FileName& fnFile);

    void ASCExporter::ASCWriteDOC();
    void ASCExporter::ASCDetMapDomType();
    ~ASCExporter();
    Map mp;
    GeoRef gr;
    Domain dm;
    RangeReal rRr;
    double rStep;
    long iLines, iCols;
    enum iconvtype {ieBit, ieBool, ieImage, ieSort255, ieSort,ieInt254,ieInt,
                    ieReal, ieNotPos };
    iconvtype iConv;              // Determined in ASCDetMapDomType
    /* value's of iConv
    ieBit
    ieBool
    ieImage
    ieSort255  Class or Id < 255
    ieSort     Id > 255
    ieInt254
    ieInt
    ieReal
    ieNotPos   Conversion not possible
    */
    long iCStore;       // Store type 0= bit, 1 = byte, 2 = int
    long iCScale;       // multiplication factor to be raised to power 10
    int  iNdec;         // number of digits after dec point in case real
    bool fExponential;  // is true if in representation exponential format is used
    double rScalef;     // scale factor
    double rspUNDEF;    // UNDEF scaled with rScalef

    long iMaxClass;               // Determined in GISDetMapDomType

    ByteBuf bBuf;
    LongBuf lBuf;
    RealBuf rBuf;
    File* filASC;
    String sFileName;
  private:


};



ASCExporter::ASCExporter(const FileName& fn)
{
  filASC = new File(fn ,facCRT);
}


ASCExporter::~ASCExporter()
{
  if(filASC) delete filASC;
}



void ASCExporter::ASCDetMapDomType()
{
  iConv = ieNotPos;
  iCScale =   0;        // scale for real to be raised to power of 10
  rScalef = 1.0;        // multiplication factor for real
  dm = mp->dm();
  // look for DomainType
  DomainImage *pDomValImage = dm->pdi();
  if ( pDomValImage != NULL )                   {iConv = ieImage; return;}
  DomainBit *pdbit = dm->pdbit();
  if ( pdbit != NULL )                          {iConv = ieBit;return;}
    DomainBool *pdBool = dm->pdbool();
    if ( pdBool != NULL )                       {iConv = ieBool; return;}
   DomainSort *pDomSort = dm->pdsrt();
   if ( pDomSort != NULL )  {
     iMaxClass = (*pDomSort).iSize();
     if ( iMaxClass < 256 )                     {iConv = ieSort255;return;}
     else if ( iMaxClass < 32767L )             {iConv = ieSort;   return;}
     else                                       {iConv = ieNotPos; return;}
   }
   DomainValue *pDomVal = dm->pdv();
   if ( pDomVal != NULL ) {
	   rRr = mp->rrMinMax(BaseMapPtr::mmmCALCULATE);
     rStep = mp->vr()->rStep();
     if ( (rStep == 1.0) && (rRr.rLo() >= 0.0) &&(rRr.rHi()<= 254.0) )
                                                {iConv = ieInt254;   return;}
     else if ( (rStep == 1.0) && (rRr.rLo() >= -32766.0) &&(rRr.rHi()<= 32767.0) )
       {iCScale = 0;
        iNdec = 0;
        rScalef = 1.0;
        fExponential = false;
        rspUNDEF = -32767.0;
                                                    iConv = ieInt; return;}
     else {
       // determine power of 10
       double rTmp = max (-rRr.rLo(),rRr.rHi());
       rTmp /= 32760.0;
       rTmp = log10(rTmp);
       iCScale = (long) ceil (rTmp);
       iNdec = max (0, -iCScale);
       rScalef = pow ((double) 10.0, (double) iCScale);
       rspUNDEF = -32767.0 * rScalef;
       fExponential = ((iCScale < -5) || (iCScale >1));
                                                iConv = ieReal;  return;}
   }
  return;
}



void ASCExporter::ASCWriteDOC()
{
  String strtemp;

  filASC -> WriteLnAscii( String("Extracted from map \"%S\" by ILWIS",
                                sFileName));

  switch (iConv)   {
    case ieBit      :
        iCStore = 0;
        break;
    case ieBool     :
    case ieImage    :
    case ieSort255  :
    case ieInt254   :
        iCStore = 1;
        break;
    case ieSort  :
    case ieInt   :
    case ieReal    :
        iCStore = 2;
        break;

  }
  filASC -> WriteLnAscii( String("      1%7li      1%7li%7li%7li",
                                iCols,iLines,iCStore,iCScale));
}


/*
   fnObject   : the name of the ilwis map
   fnFile     : the name of the .ASCII file
   The ExportASCII function has its prototype in the CONVLOC.H file
*/
void ImpExp::ExportASCII(const FileName& fnObject, const FileName& fnFile)
{

  try {
    trq.SetTitle(SCVTitleExportASCII);   // the title in the report window
    trq.SetText(SCVTextProcessing);      // the text in the report window
    ASCExporter ASCex(fnFile);
    ASCex.sFileName = fnObject.sFile;
    ASCex.mp = Map(fnObject);
    ASCex.gr = ASCex.mp->gr();

    ASCex.ASCDetMapDomType();
    if ( ASCex.iConv == ASCExporter::ieNotPos )  {
      ASCex.filASC->SetErase(true);
      throw ErrorImportExport(SCVErrNotSupported);
    }
    ASCex.iLines = ASCex.mp->iLines();
    ASCex.iCols = ASCex.mp->iCols();
    ASCex.ASCWriteDOC();

    ASCex.bBuf.Size(ASCex.iCols);
    ASCex.lBuf.Size(ASCex.iCols);
    ASCex.rBuf.Size(ASCex.iCols);

    long iLc, iRc;
    String sString;
    switch (ASCex.iConv) {
      case ASCExporter::ieBit     :
        for ( iLc = 0; iLc < ASCex.iLines ; iLc++) {
          if (trq.fUpdate(iLc,ASCex.iLines )) return;
          ASCex.mp->GetLineVal( iLc, ASCex.lBuf );
          for ( iRc = 0; iRc < ASCex.iCols ; iRc++ )  {
            if ( iRc !=0 && ((iRc % 30) == 0 )) ASCex.filASC->WriteLnAscii("");
            sString=ASCex.mp->dvrs().sValue(ASCex.lBuf[iRc],2);
            ASCex.filASC->Write((long)sString.length(),sString.scVal());
          }
         ASCex.filASC->WriteLnAscii("");
        }
        break;


      case ASCExporter::ieImage   :
      case ASCExporter::ieSort255 :
        for ( iLc = 0; iLc < ASCex.iLines ; iLc++) {
          if (trq.fUpdate(iLc,ASCex.iLines )) return;
          ASCex.mp->GetLineRaw( iLc, ASCex.bBuf );
          for ( iRc = 0; iRc < ASCex.iCols ; iRc++ )  {
            if ( iRc !=0 && ((iRc % 15) == 0 )) ASCex.filASC->WriteLnAscii("");
            sString =String("%5i", ASCex.bBuf[iRc]);
            ASCex.filASC->Write((long)sString.length(),sString.scVal());
          }
        ASCex.filASC->WriteLnAscii("");
        }
        break;


      case ASCExporter::ieInt254  :
        for ( iLc = 0; iLc < ASCex.iLines ; iLc++) {
          if (trq.fUpdate(iLc,ASCex.iLines )) return;
          ASCex.mp->GetLineVal( iLc, ASCex.lBuf );
          for ( iRc = 0; iRc < ASCex.iCols ; iRc++ )  {
            if ( iRc !=0 && ((iRc % 15) == 0 )) ASCex.filASC->WriteLnAscii("");
            if ( ASCex.lBuf[iRc] == iUNDEF ) ASCex.lBuf[iRc] = 0;
            sString=ASCex.mp->dvrs().sValue(ASCex.lBuf[iRc],5);
            ASCex.filASC->Write((long)sString.length(),sString.scVal());
          }
         ASCex.filASC->WriteLnAscii("");
        }
        break;

      case ASCExporter::ieBool :
        for ( iLc = 0; iLc < ASCex.iLines ; iLc++) {
          if (trq.fUpdate(iLc,ASCex.iLines )) return;
          ASCex.mp->GetLineRaw( iLc, ASCex.bBuf );
          for ( iRc = 0; iRc < ASCex.iCols ; iRc++ )  {
            if ( iRc !=0 && ((iRc % 15) == 0 )) ASCex.filASC->WriteLnAscii("");
            switch (ASCex.bBuf[iRc]) {
               case 1 :
                 sString = "    0";
                 break;
               case 2 :
                 sString = "    1";
                break;
               case 0 :
                 sString = "    0";
                break;
            }
          ASCex.filASC->Write((long)sString.length(),sString.scVal());
          }
        ASCex.filASC->WriteLnAscii("");
        }
        break;



      case ASCExporter::ieSort    :
        for ( iLc = 0; iLc < ASCex.iLines ; iLc++) {
          if (trq.fUpdate(iLc,ASCex.iLines )) return;
          ASCex.mp->GetLineRaw( iLc, ASCex.lBuf );
          for ( iRc = 0; iRc < ASCex.iCols ; iRc++ )  {
            if ( iRc !=0 && ((iRc % 8) == 0 )) ASCex.filASC->WriteLnAscii("");
            sString =String(" %7i.", ASCex.lBuf[iRc]);
            ASCex.filASC->Write((long)sString.length(),sString.scVal());
          }
        ASCex.filASC->WriteLnAscii("");
        }
        break;


      case ASCExporter::ieInt  :
      case ASCExporter::ieReal :
        for ( iLc = 0; iLc < ASCex.iLines ; iLc++) {
          if (trq.fUpdate(iLc,ASCex.iLines )) return;
          ASCex.mp->GetLineVal( iLc, ASCex.rBuf );
          for ( iRc = 0; iRc < ASCex.iCols ; iRc++ )  {
            if ( iRc !=0 && ((iRc % 15) == 0 )) ASCex.filASC->WriteLnAscii("");
            float rTmp = floatConv(ASCex.rBuf[iRc]);
            if ( rTmp == flUNDEF ) rTmp = ASCex.rspUNDEF;
            if (ASCex.fExponential )
              sString =String("%6e ", rTmp);
            else
              sString =String("%#9.*f", ASCex.iNdec, rTmp);
            ASCex.filASC->Write((long)sString.length(),sString.scVal());
          }
        ASCex.filASC->WriteLnAscii("");
        }
        break;
    }
  }
  catch (ErrorObject& err) {
    err.Show();
  }
}


