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
/* $Log: /ILWIS 3.0/Import_Export/EXPLAN.cpp $
 * 
 * 8     7-08-01 12:43 Koolhoven
 * removed domain member from MapList, because it served no purpose
 * 
 * 7     14/03/01 15:58 Willem
 * The export succesfully exported to GIS and LAN files, but failed to
 * keep them on disk: the fErase flag was not properly reset
 * 
 * 6     29/02/00 11:46 Willem
 * Replaced the obsolete fWarning() function by a messageBox
 * 
 * 5     25/02/00 18:16 Willem
 * Cleanup of code
 * Functionality is now Checked
 * 
 * 4     30-11-99 12:19 Wind
 * added local copy of Representation to prevent 'inline deletion' of
 * representation object
 * 
 * 3     17-06-99 3:59p Martin
 * // -> /*
 * 
 * 2     17-06-99 2:10p Martin
 * ported files to VS
// Revision 1.4  1998/09/16 17:41:34  Wim
// 22beta2
//
// Revision 1.3  1997/08/11 15:46:16  Wim
// Corrected check on north oriented ness
//
// Revision 1.2  1997-07-29 21:49:54+02  Willem
// Made test for north oriented maps a bit simpler
//
/*
  export ILWIS to .LAN format
  template by Willem Nieuwenhuis, oct 1996
  modified by
  ILWIS Department ITC
	Last change:  WK   11 Aug 97    5:45 pm
*/

/*  Revision history:
    WN 15-07-97: Changed the ExportLAN function so it will accept either a
                 Map or a MapList as input
*/

#include "Headers\toolspch.h"
#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\ModuleMap.h"// for the tranquillizer
#include "Engine\Base\System\Engine.h"
#include "Engine\SpatialReference\Gr.h"             // for georef functions"
#include "Engine\SpatialReference\Grcornrs.h"    // for georef corner functions"
#include "Engine\SpatialReference\coordsys.h "      // for conversion to LatLon"
#include "Engine\Map\Raster\MapList\maplist.h"
#include "Engine\Domain\dmsort.h"
#include "Engine\Domain\dmpict.h"
#include "Engine\Domain\Dmvalue.h"

#include "Headers\Hs\CONV.hs"         // for the strings"
#include "Engine\DataExchange\Convloc.h"       // for the prototypes"
#include "Engine\DataExchange\expgislan.h"       // for the prototypes"


LANExporter::LANExporter(const FileName& fn) 
	: GISExporter(fn)
{
}

LANExporter::~LANExporter() 
{
}

void LANExporter::BuildHeader()
{
	GISExporter::BuildHeader();
	sHeader.iNBANDS = mpl->iSize();
}

/*
   fnObject   : the name of the ilwis map or MapList
   fnFile     : the name of the .LAN file
   The ExportLAN function has its prototype in the CONVLOC.H file
*/
void ImpExp::ExportLAN(const FileName& fnObject, const FileName& fnFile)
{
  int iMapCnt;
  try {
    trq.SetTitle(SCVTitleExportLAN);   // the title in the report window
    trq.SetText(SCVTextProcessing);    // the text in the report window
    LANExporter gisex(fnFile);
    gisex.iIOcase = tpErr;
    if (fnObject.sExt == ".mpr") {     // create a MapList with only one map
      Array<FileName> afnMap;
      afnMap &= fnObject;
      FileName fn = FileName::fnUnique(FileName(fnObject, ".mpl", true));
      gisex.mpl = MapList(fn, afnMap);
      gisex.mpl->fErase = true;        // remove again when done
    }
    else
      gisex.mpl = MapList(fnObject);
// check on empty map list
    int iNrMaps = gisex.mpl->iSize();
    if ( iNrMaps < 1 )
			throw ErrorImportExport( SCVErrEmptyMaplist);

    // check if domains of the different maps are the same
    gisex.mp = gisex.mpl->map(gisex.mpl->iLower()); // temporary
    gisex.dm = gisex.mp->dm();
    for (iMapCnt = gisex.mpl->iLower() + 1; iMapCnt <= gisex.mpl->iUpper() ;iMapCnt++ )
		{
      Domain dmothers = gisex.mpl->map(iMapCnt)->dm();
      if ( dmothers != gisex.dm )
        throw ErrorImportExport(SCVErrDomainNotEqual);
    }
    gisex.gr = gisex.mpl->gr();
    gisex.st = gisex.mp->st();
    if (gisex.st > stINT)                         // if StoreType> 16 bit return error !!!
      throw ErrorImportExport(SCVErrOutOfRange);  // Error input map contains too wide range

    gisex.iLines = gisex.mp->iLines();
    gisex.iCols = gisex.mp->iCols();
    // look if north oriented
    // (not really fool proof, but good enough)
    Coord crTop = gisex.gr->cConv(RowCol(0L, 0L));
    Coord crBot = gisex.gr->cConv(RowCol(gisex.iLines, 0L));
    if (abs(crTop.x - crBot.x) > 0.001)
		{
			int iRet = getEngine()->Message(SCVWarnNotNorthOriented.scVal(), SCVMsgExportWarning.scVal(), MB_ICONEXCLAMATION|MB_YESNO);

      if (iRet != IDYES)
        return;
		}

    gisex.iIOcase = gisex.DetOutSize();
    if (gisex.iIOcase == tpErr)
      throw ErrorImportExport(SCVErrOutOfRange);

    gisex.WriteHeader();
    switch (gisex.iIOcase) {
      case tpRawByte:
      case tpClassByte :
        gisex.bBuf.Size(gisex.iCols);
        break;
      case tpRawInt :
      case tpClassInt :
        gisex.iBuf.Size(gisex.iCols);
        break;
      case tpValByte :
        gisex.lBuf.Size(gisex.iCols);
        gisex.bBuf.Size(gisex.iCols);
        break;
      case tpValInt :
        gisex.lBuf.Size(gisex.iCols);
        gisex.iBuf.Size(gisex.iCols);
        break;
    }

    long iLc, iRc;
    switch (gisex.iIOcase) {
      case tpRawByte :
      case tpClassByte :
        for (iLc = 0; iLc < gisex.iLines ; iLc++) {
          for (iMapCnt=gisex.mpl->iLower();iMapCnt<=gisex.mpl->iUpper() ;iMapCnt++ ) {
            gisex.mp = gisex.mpl->map(iMapCnt);
            if (trq.fUpdate(iLc, gisex.iLines )) return;
            gisex.mp->GetLineRaw(iLc, gisex.bBuf);
            if (gisex.iIOcase == tpClassByte) {  // special case for classes/IDs
              for (long k = 0; k < gisex.iCols; k++)
                gisex.bBuf[k] -= 1;      // undefined becomes 255
            }
          gisex.filGISLAN->Write(gisex.iCols, gisex.bBuf.buf());
          }
        }
        break;

      case tpRawInt :
      case tpClassInt :
        for (iLc = 0; iLc < gisex.iLines ; iLc++) {
          for (iMapCnt=gisex.mpl->iLower();iMapCnt<=gisex.mpl->iUpper() ;iMapCnt++ ) {
            gisex.mp = gisex.mpl->map(iMapCnt);
            if (trq.fUpdate(iLc,gisex.iLines )) return;
            gisex.mp->GetLineRaw( iLc, gisex.iBuf );
            if (gisex.iIOcase == tpClassInt) {  // special case for classes/IDs
              for (long k = 0; k < gisex.iCols; k++)
                if (gisex.iBuf[k] != 0)
                  gisex.iBuf[k] -= 1;
                else
                  gisex.iBuf[k] = SHRT_MAX;
            }
            gisex.filGISLAN->Write(gisex.iCols * 2, gisex.iBuf.buf());
          }
        }
        break;

      case tpValByte :
        for (iLc = 0; iLc < gisex.iLines ; iLc++) {
          for (iMapCnt=gisex.mpl->iLower();iMapCnt<=gisex.mpl->iUpper() ;iMapCnt++ ) {
            gisex.mp = gisex.mpl->map(iMapCnt);
            if (trq.fUpdate(iLc,gisex.iLines )) return;
            gisex.mp->GetLineVal( iLc, gisex.lBuf );
            for (iRc = 0; iRc < gisex.iCols ; iRc++ )
              gisex.bBuf[iRc] =  byteConv(gisex.lBuf[iRc]);
            gisex.filGISLAN->Write(gisex.iCols, gisex.bBuf.buf());
          }
        }

        break;

      case tpValInt :
        for (iLc = 0; iLc < gisex.iLines ; iLc++) {
          for (iMapCnt=gisex.mpl->iLower();iMapCnt<=gisex.mpl->iUpper() ;iMapCnt++ ) {
            gisex.mp = gisex.mpl->map(iMapCnt);
            if (trq.fUpdate(iLc,gisex.iLines )) return;
            gisex.mp->GetLineVal( iLc, gisex.lBuf );
            for (iRc = 0; iRc < gisex.iCols ; iRc++ )
              gisex.iBuf[iRc] = shortConv(gisex.lBuf[iRc]);
            gisex.filGISLAN->Write(gisex.iCols * 2, gisex.iBuf.buf());
          }
        }
            
        break;
    }
    // add fillers to file to extend last block to 512 byte
    long iFileFill = (gisex.filGISLAN->iSize()) % 512;
    if ( iFileFill != 0 ) {
      iFileFill = 512 - iFileFill;
      ByteBuf bb(iFileFill);
      for (long ii = 0; ii < bb.iSize(); ii++)
        bb[ii] = 0;
      gisex.filGISLAN->Write(bb.iSize(), bb.buf());
    }
    trq.fUpdate(gisex.iLines, gisex.iLines);
    trq.SetText(SCVTextErdasTrailer);
    trq.fUpdate(0);
    gisex.WriteTrlFile();
    gisex.SetErase(false);
  }
  catch (ErrorObject& err) {
    err.Show();
  }
}


