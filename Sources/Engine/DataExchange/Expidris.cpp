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
/* $Log: /ILWIS 3.0/Import_Export/Expidris.cpp $
 * 
 * 5     16/03/00 16:35 Willem
 * - Replaced the obsolete trq.Message() function with Show() function of
 * ErrorObject itself
 * 
 * 4     29/02/00 12:12 Willem
 * - Added warning message for not-north oriented maps
 * - Improved error messages related to export of possible storetypes
 * - Changed %s into %S in formatted strings
 * 
 * 3     17-06-99 3:59p Martin
 * // -> /*
 * 
 * 2     17-06-99 2:10p Martin
 * ported files to VS
// Revision 1.4  1998/09/17 08:46:06  Wim
// Changed "catagory" to "category" according to the description in bug 750.
//
// Revision 1.3  1998-09-16 18:41:07+01  Wim
// 22beta2
//
// Revision 1.2  1997/09/12 16:20:16  Dick
// in WriteClrTbl check if rpr is valid
// in ImpExp::ExportIDRISI by ieSort GetLineRaw entered
// in listing marked(wrong) with rev 1.1
//

/*
  export ILWIS to IDRISI format
  template by Willem Nieuwenhuis, oct 1996
  modified by
  ILWIS Department ITC
	Last change:  WK   17 Sep 98    9:45 am
*/
#include "Headers\toolspch.h"
#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\Engine.h"
#include "Engine\DataExchange\Convloc.h"       // for the prototypes"
#include "Headers\Hs\CONV.hs"         // for the strings"
#include "Engine\SpatialReference\Gr.h"             // for georef functions"
#include "Engine\Map\Raster\Map.h"
#include "Engine\Domain\dmsort.h"
#include "Engine\Domain\Dmvalue.h"


class IDRExporter{
  public:
    IDRExporter(const FileName& fnFile);

    void IDRExporter::IDRWriteDOC();
    void IDRExporter::IDRDetOutSize();
    void IDRExporter::IDRWriteClrTbl();
    ~IDRExporter();
    Map mp;
    GeoRef gr;
    Domain dm;
    RangeReal rRr;

    long iLines, iCols;
    enum iconvtype {ieBit, ieBool, ieImage, ieSort255, ieSort,ieInt,
                    ieReal, iePicture, ieNotPos };
    iconvtype iConv;              // Determined in GISDetOutSize
    /* value's of iConv
    ieBit
    ieBool
    ieImage
    ieSort255  Class or Id < 255
    ieSort     Id > 255
    ieInt
    ieReal
    iePicture
    ieNotPos   Conversion not possible
    */
    long iMaxClass;               // Determined in GISDetOutSize

    ByteBuf bBuf;
    IntBuf  iBuf;
    LongBuf lBuf;
    RealBuf rBuf;
    Buf<float> flBuf;
    File* filIDR;
    File* filDOC;
    File* filCLR;
    String sFileName;
  private:


};



IDRExporter::IDRExporter(const FileName& fn)
{
  filIDR = new File(fn ,facCRT);
  FileName ft(fn, ".DOC");
  filDOC = new File(ft ,facCRT);
  FileName fc(fn,".PAL");
  filCLR = new File(fc,facCRT);
  filCLR->SetErase(true);
  sFileName = fn.sFile;
}


IDRExporter::~IDRExporter()
{
  if(filIDR) delete filIDR;
  if(filDOC) delete filDOC;
  if(filCLR) delete filCLR;
}



void IDRExporter::IDRDetOutSize()
{
	iConv = ieNotPos;
	iMaxClass = 0;        // make iMaxClass zero here to have check if DomainSort
	dm = mp->dm();

	DomainSort *pDomSort;
	if ( dm->pdi() != NULL )
		iConv = ieImage; 
	else if (dm->pdbit() != NULL )
		iConv = ieBit;
	else if (dm->pdbool() != NULL )
		iConv = ieBool; 
	else if ((pDomSort = dm->pdsrt()) != NULL )
	{
		iMaxClass = pDomSort->iSize();
		if ( iMaxClass < 256 )
			iConv = ieSort255;
		else if ( iMaxClass < 32767L )
			iConv = ieSort;
		else
			iConv = ieNotPos;
	}
	else if (dm->pdv() != NULL ) 
	{
		rRr = mp->rrMinMax(true);
		if ( (mp->vr()->rStep() == 1.0) && (rRr.rLo() >= -32766.0) &&(rRr.rHi()<= 32767.0) )
			iConv = ieInt;
		else
			iConv = ieReal;
	}
	else if (dm->pdp() != NULL )
		iConv = iePicture;
}



void IDRExporter::IDRWriteDOC()
{
  String strtemp;

  filDOC -> WriteLnAscii( String("file title  : Export File from ILWIS"));
  switch (iConv)   {
    case ieBit      :
    case ieBool     :
    case ieImage    :
    case ieSort255  :
    case iePicture  :
        strtemp= String("data type   : byte");
        break;
    case ieSort  :
    case ieInt   :
        strtemp= String("data type   : integer");
        break;
    case ieReal      :
        strtemp= String("data type   : real");
        break;
  }
  filDOC -> WriteLnAscii(strtemp);
  filDOC -> WriteLnAscii( String("file type   : binary"));

  filDOC -> WriteLnAscii( String("columns     : %i",iCols));
  filDOC -> WriteLnAscii( String("rows        : %i",iLines));
  filDOC -> WriteLnAscii( String("ref. system : plane"));
  filDOC -> WriteLnAscii( String("ref. units  : m   "));
  filDOC -> WriteLnAscii( String("unit dist.  : 1.0000000"));

  CoordBounds cb = mp->gr()->cb();
  double rMinX = cb.MinX(), rMinY = cb.MinY();
  double rMaxX = cb.MaxX(), rMaxY = cb.MaxY();
  double rStep = mp->gr()->rPixSize();
  if (cb.fUndef()) {
    rMinX = 0.0; rMinY = 0.0;
    rMaxX = iCols; rMaxY = iLines;
    rStep = 1.0;
  }
  filDOC -> WriteLnAscii( String("min. X      : %-15.5lf", rMinX));
  filDOC -> WriteLnAscii( String("max. X      : %-15.5lf", rMaxX));
  filDOC -> WriteLnAscii( String("min. Y      : %-15.5lf", rMinY));
  filDOC -> WriteLnAscii( String("max. Y      : %-15.5lf", rMaxY));
  filDOC -> WriteLnAscii( String("pos'n error : unknown "));
  filDOC -> WriteLnAscii( String("resolution  : %lf", rStep));
  String smin,smax;
  switch ( iConv )
  {
    case ieBit :
        smin = String ("0");
        smax = String ("1");
        break;

    case ieBool :
        smin = String ("0");
        smax = String ("1");
        break;

    case ieImage :
    case iePicture :
        smin = String ("0");
        smax = String ("255");
        break;

    case ieSort255 :
    case ieSort    :
        smin = String ("0");
        smax = String ("%i",iMaxClass);
        break;

    case ieInt  :
    case ieReal :
        smin = String ("%lf",rRr.rLo());
        smax = String ("%lf",rRr.rHi());
        break;
    default :
        break;
  }
  filDOC -> WriteLnAscii( String ("min. value  : %S", smin));
  filDOC -> WriteLnAscii( String ("max. value  : %S", smax));
  filDOC -> WriteLnAscii( String ("value units : meter"));
  filDOC -> WriteLnAscii( String ("value error : unknown"));
  switch ( iConv )
  {
    case ieBool :
      filDOC -> WriteLnAscii( String ("flag value  : 255"));
      filDOC -> WriteLnAscii( String ("flag def'n  : missing data"));
      break;

    case ieSort255 :    // UNDEF in CLASS less then 256 is 0
      filDOC -> WriteLnAscii( String ("flag value  : 0"));
      filDOC -> WriteLnAscii( String ("flag def'n  : missing data"));
      break;

    case ieSort :    // UNDEF in CLASS > 255 is shUNDEF
    case ieInt  :
      filDOC -> WriteLnAscii( String ("flag value  : %i",shUNDEF));
      filDOC -> WriteLnAscii( String ("flag def'n  : missing data"));
      break;

     case ieReal    :
       filDOC -> WriteLnAscii( String ("flag value  : %0e",flUNDEF));
       filDOC -> WriteLnAscii( String ("flag def'n  : missing data"));
       break;

    default :
      filDOC -> WriteLnAscii( String ("flag value  : none"));
      filDOC -> WriteLnAscii( String ("flag def'n  : none"));
      break;
  }
  // check if DomainSort AND number off classes < 255 if true add names
  if (iConv == ieSort255)  {
    String sDomName;
    filDOC -> WriteLnAscii( String ("legend cats : %i", iMaxClass + 1));
    filDOC -> WriteLnAscii( String ("category 0  :"));
    for ( int icc = 1;icc <= iMaxClass ; icc++ ) {
      sDomName = dm->sValueByRaw(icc);
      filDOC -> WriteLnAscii( String ("category %3i: %S", icc, sDomName));
    }
  }
  else
  filDOC -> WriteLnAscii( String ("legend cats : 0"));
}

void IDRExporter::IDRWriteClrTbl()
{
  /* write color table                                   */
  /* Ilwis color table is reduced from 0..255 to 0..63   */
  if ( !(dm->rpr().fValid()))  return;  // rev 1.1 return if no user color table
  Representation rpr = dm->rpr();
  float rFac = (float)(63.0 / 255.0);
  ColorBuf bufColor;
  rpr->GetColors(bufColor);
  for (int i = 0; i < bufColor.iSize(); i++) {
    filCLR->WriteLnAscii(String("%3i %3i %3i %3i",i,
                        shortConv(bufColor[i].red()   * rFac),
                        shortConv(bufColor[i].green() * rFac),
                        shortConv(bufColor[i].blue()  * rFac)));
  }
  filCLR->SetErase(false);
}

/*
   fnObject   : the name of the ilwis map
   fnFile     : the name of the .IDRISI file
   The ExportIDRISI function has its prototype in the CONVLOC.H file
*/
void ImpExp::ExportIDRISI(const FileName& fnObject, const FileName& fnFile)
{

  try {
    trq.SetTitle(SCVTitleExportIdrisi);   // the title in the report window
    trq.SetText(SCVTextProcessing);   // the text in the report window
    IDRExporter IDRex(fnFile);
    IDRex.iConv = IDRExporter::ieNotPos;
    IDRex.mp = Map(fnObject);
    IDRex.gr = IDRex.mp->gr();
		if (!IDRex.gr->fNorthOriented())
		{
			int iRet = getEngine()->Message(SCVWarnNotNorthOriented.scVal(), SCVMsgExportWarning.scVal(), MB_ICONEXCLAMATION|MB_YESNO);

      if (iRet != IDYES)
        return;
		}

    IDRex.IDRDetOutSize();
    if ( IDRex.iConv == IDRExporter::ieNotPos )
		{
			String sErr = String(SCVErrTooManyValues_S.scVal(), "IDRISI image");
      throw ErrorImportExport(sErr);
		}

    IDRex.iLines = IDRex.mp->iLines();
    IDRex.iCols = IDRex.mp->iCols();
    IDRex.IDRWriteDOC();
    IDRex.bBuf.Size(IDRex.iCols);
    IDRex.iBuf.Size(IDRex.iCols);
    IDRex.lBuf.Size(IDRex.iCols);
    IDRex.rBuf.Size(IDRex.iCols);
    IDRex.flBuf.Size(IDRex.iCols);

    if ((IDRex.iConv == IDRExporter::ieSort255) ||
        (IDRex.iConv == IDRExporter::iePicture))
      IDRex.IDRWriteClrTbl();

		long iLc, iRc;
    switch (IDRex.iConv) 
		{
      case IDRExporter::ieBit    :
      case IDRExporter::ieImage  :
      case IDRExporter::ieSort255 :
      case IDRExporter::iePicture :
        for ( iLc = 0; iLc < IDRex.iLines ; iLc++) 
				{
          if (trq.fUpdate(iLc,IDRex.iLines )) return;
          IDRex.mp->GetLineRaw( iLc, IDRex.bBuf );
          IDRex.filIDR->Write(IDRex.iCols, IDRex.bBuf.buf());
        }
        break;
      case IDRExporter::ieBool :
        for ( iLc = 0; iLc < IDRex.iLines ; iLc++) 
				{
          if (trq.fUpdate(iLc,IDRex.iLines )) return;
          IDRex.mp->GetLineRaw( iLc, IDRex.bBuf );
          for ( iRc = 0; iRc < IDRex.iCols ; iRc++ )
            IDRex.bBuf[iRc] = IDRex.bBuf[iRc] - 1;
          IDRex.filIDR->Write(IDRex.iCols, IDRex.bBuf.buf());
        }
        break;
      case IDRExporter::ieSort :
        for ( iLc = 0; iLc < IDRex.iLines ; iLc++) 
				{
          if (trq.fUpdate(iLc,IDRex.iLines )) return;
          IDRex.mp->GetLineRaw( iLc, IDRex.iBuf );
          IDRex.filIDR->Write(IDRex.iCols * 2, IDRex.iBuf.buf());
        }                                                          
        break;                                                     
      case IDRExporter::ieInt  :
        for ( iLc = 0; iLc < IDRex.iLines ; iLc++)
				{
          if (trq.fUpdate(iLc,IDRex.iLines )) return;
          IDRex.mp->GetLineVal( iLc, IDRex.lBuf );
          for ( iRc = 0; iRc < IDRex.iCols ; iRc++ )
            IDRex.iBuf[iRc] = shortConv(IDRex.lBuf[iRc]);
          IDRex.filIDR->Write(IDRex.iCols * 2, IDRex.iBuf.buf());
        }
        break;

      case IDRExporter::ieReal :
        for ( iLc = 0; iLc < IDRex.iLines ; iLc++) 
				{
          if (trq.fUpdate(iLc,IDRex.iLines )) return;
          IDRex.mp->GetLineVal( iLc, IDRex.rBuf );
          for ( iRc = 0; iRc < IDRex.iCols ; iRc++ )
            IDRex.flBuf[iRc] = floatConv(IDRex.rBuf[iRc]);
          IDRex.filIDR->Write(IDRex.iCols*4, IDRex.flBuf.buf());
        }
        break;
    }
  }
  catch (ErrorObject& err) {
    err.Show();
  }
}


