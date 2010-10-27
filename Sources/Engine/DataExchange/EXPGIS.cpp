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
  export ILWIS to .GIS format
  template by Willem Nieuwenhuis, oct 1996
  modified by
  ILWIS Department ITC
	Last change:  WK   11 Aug 97    5:45 pm
*/

#include "Headers\toolspch.h"
#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\Engine.h"
#include "Engine\SpatialReference\Gr.h"             // for georef functions"
#include "Engine\Map\Raster\Map.h"
#include "Engine\Map\Raster\MapList\maplist.h"
#include "Engine\SpatialReference\Grcornrs.h"    // for georef corner functions"
#include "Engine\SpatialReference\Coordsys.h"       // for conversion to LatLon"
#include "Engine\Domain\dmsort.h"
#include "Engine\Domain\dmpict.h"
#include "Engine\Domain\Dmvalue.h"

#include "Headers\Hs\CONV.hs"         // for the strings"
#include "Engine\DataExchange\Convloc.h"       // for the prototypes"
#include "Engine\DataExchange\expgislan.h"       // for the prototypes"

GISExporter::GISExporter(const FileName& fn) 
{
	fnFile = fn;
	filGISLAN = new File(fn ,facCRT);

	SetErase(true);  // make sure output files are removed in case of errors
}

GISExporter::~GISExporter() 
{
	delete filGISLAN;
}

void GISExporter::SetErase(bool fErase)
{
	if (filGISLAN)
		filGISLAN->SetErase(fErase);
}

/*
  This function determines if the the conversion is possible, and if bytes
  or integers will be used to write the GIS file. The conversion assumes the
  GIS file to carry unsigned values only (reasonable for storage of classes)
  The function returns one of the following values:
     tpErr,                    // for cases in which the conversion cannot be done
     tpRawByte, tpRawInt,      // for Images and Pictures
     tpValByte, tpValInt,      // other value maps
     tpClassByte, tpClassInt}; // class maps
*/
short GISExporter::DetOutSize() 
{
	short icase = tpErr;
	RangeReal rRr;
	iNrClass = 0;
	dm = mp->dm();
	DomainValue *pDomVal = dm->pdv();
	if (pDomVal != NULL)  {                           // value input
		DomainImage *pDomValImage = dm->pdi();
		DomainBit *pdBit = dm->pdbit();
		if (pDomValImage != NULL || pdBit != NULL)
		{
			icase = tpRawByte;
			return icase;
		}
		else 
		{
			DomainValueReal *pDomValReal = dm->pdvr();
			if (pDomValReal != NULL)
			{
				rRr = mp->rrMinMax(BaseMapPtr::mmmCALCULATE);
				if ( !(mp->vr()->fRealValues()))    // integer kind of input
				{
					if ((rRr.rLo() < 0) || (rRr.rHi() > 65535L))
						icase = tpErr;  // does not fit in 16 bit
					if (rRr.rHi() < 255)
						icase = tpValByte;
					else
						icase = tpValInt;
					return icase;
				}
				else
				{
					icase = tpErr; 
					return icase;  // noconversion for real values with stepsize != 1
				}
			}
		}
	}
	
	else {
		DomainSort *pDomSort = dm->pdsrt();
		if (pDomSort != NULL)  {
			iNrClass = (*pDomSort).iSize();
			if (iNrClass < 256)          icase = tpClassByte;
			else if (iNrClass < 65535L)  icase = tpClassInt;
			else                         icase = tpErr;
		}
		else if (dm->pdp() != NULL) {   // DomainPicture (max 256)
			long iNrCol = dm->pdp()->iColors();
			if (iNrCol <= 256)         icase = tpRawByte;
			else if (iNrCol < 65535L)  icase = tpRawInt;
			else                       icase = tpErr;
		}
	}
	return icase;
}

void GISExporter::WriteHeader()
{
	BuildHeader();
	filGISLAN->Write(sizeof(sHeader),&sHeader);
}

void GISExporter::BuildHeader()
{
	memset(&sHeader, 0, sizeof(sHeader));
	memcpy(sHeader.cHDWORD, "HEAD74", 6);
	
	switch (iIOcase) {
    case tpRawByte:
    case tpValByte: 
    case tpClassByte: {
		sHeader.iIPACK = 0;
		break;
					  }
    case tpRawInt:
    case tpValInt: 
    case tpClassInt: {
		sHeader.iIPACK = 2;
		break;
					 }
	}
	sHeader.iNBANDS = 1;        // always 1 for GIS
	sHeader.iROWS = iLines;
	sHeader.iCOLS = iCols;
	sHeader.iXSTART = 1;          // Kopie van 1.4
	sHeader.iYSTART = 1;          // Kopie van 1.4
	sHeader.iMAPTYPE = 1;         // None = 0; UTM = 1 etc
	sHeader.iNCLASS = iNrClass;   // is set only for DomainSort
	sHeader.iIAUTYP = 1;          // Area units: 0 = None; 1 = acre; 2 = hectare; 3 = other
	sHeader.rACRE = 1.0;          // Will be computed later
	Coord coTL, coTLplus;
	gr->RowCol2Coord(0.5, 0.5, coTL);
	sHeader.rXLeftUpper = (float)coTL.x;
	sHeader.rYLeftUpper = (float)coTL.y;
	// Dimension of top left pixel is taken as size for ALL pixels !!!!!!
	gr->RowCol2Coord(1.5, 1.5, coTLplus);
	// hangt nog van MAPTYPE af en feet degrees etc !!!!!
	sHeader.rXSize = (float)fabs(coTLplus.x - coTL.x);
	sHeader.rYSize = (float)fabs(coTLplus.y - coTL.y);
	sHeader.rACRE  = (float)sHeader.rXSize * sHeader.rYSize;
}

/*
  Write Trailer file;
  for DomainSort: classes/IDs and representation
  for others: representation (Picture and Image)
*/
void GISExporter::WriteTrlFile() 
{
	if (! (dm->pdsrt() || dm->pdp()))
		return;                 // only write Trailer file for DomainSort/Picture
	
	FileName ft(fnFile, ".TRL");
	File filTRL(ft ,facCRT);
	filTRL.SetErase(true);

	char pcClass[32];
	char pcRecord[128] = "TRAIL74";  // or "TRAILER" for pre-7.4 Erdas
	byte abColors[128 * 6];   // the color scheme: order: Green, Red, Blue (256 bytes each)
	// for a total length of 6 Erdas records
	short iRedOff = 256;
	short iBlueOff = 512;
	
	memset(pcRecord + 7, 0, 128);
	pcRecord[72] = '~';    // reset variable name to length zero
	
	filTRL.Write(sizeof(pcRecord), pcRecord);
	
	// Now write the colour scheme (representation)
	memset(abColors, 0, 128 * 6);
	Representation rpr = dm->rpr();
	if (rpr.fValid())
	{
		ColorBuf bufColor;
		rpr->GetColors(bufColor);
		short iClOff = (iNrClass == 0) ? 0 : 1;    // Class domain is shifted
		for (long iCl = 0; iCl < bufColor.iSize() - iClOff; iCl++)
		{
			abColors[iCl]            = bufColor[iCl + iClOff].green();
			abColors[iRedOff  + iCl] = bufColor[iCl + iClOff].red();
			abColors[iBlueOff + iCl] = bufColor[iCl + iClOff].blue();
		}
		filTRL.Write(sizeof(abColors), abColors);  // fill the color schemes
	}
	
	// Clear the histogram data (no histogram info)
	// The trailer record contains the string "TRAIL74" ONLY if a histogram is filled in
	memset(pcRecord, 0, 128);
	filTRL.Write(sizeof(pcRecord), pcRecord);    // write second TRAILER record (==histogram)
	int i;
	for (i = 0; i < 8; i++)
		filTRL.Write(sizeof(pcRecord), pcRecord);  // clear histograms
	
	// Add the classes for DomainSort only
	if (dm->pdsrt())
	{
		String sName;
		for (i = 1; i <= iNrClass; i++) 
		{
			memset(pcClass, 0, 32);
			sName = dm->sValueByRaw(i, 0).sLeft(31);
			strcpy(pcClass, sName.scVal());
			pcClass[sName.length()] = '~';
			
			filTRL.Write(sizeof(pcClass), pcClass);
		}
	}
	filTRL.SetErase(false);
}

/*
   fnObject   : the name of the ilwis map
   fnFile     : the name of the .GIS file
   The ExportGIS function has its prototype in the CONVLOC.H file
*/
void ImpExp::ExportGIS(const FileName& fnObject, const FileName& fnFile)
{
	try
	{
		trq.SetTitle(SCVTitleExportGIS);   // the title in the report window
		trq.SetText(SCVTextProcessing);    // the text in the report window
		GISExporter gisex(fnFile);
		gisex.iIOcase = tpErr;
		gisex.mp = Map(fnObject);
		gisex.gr = gisex.mp->gr();
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
		switch (gisex.iIOcase) 
		{
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
		switch (gisex.iIOcase) 
		{
			case tpRawByte :
			case tpClassByte :
				for (iLc = 0; iLc < gisex.iLines ; iLc++) {
					if (trq.fUpdate(iLc, gisex.iLines )) return;
					gisex.mp->GetLineRaw(iLc, gisex.bBuf);
					if (gisex.iIOcase == tpClassByte) {  // special case for classes/IDs
						for (long k = 0; k < gisex.iCols; k++)
							gisex.bBuf[k] -= 1;      // undefined becomes 255
					}
					gisex.filGISLAN->Write(gisex.iCols, gisex.bBuf.buf());
				}
				break;
				
			case tpRawInt :
			case tpClassInt :
				for (iLc = 0; iLc < gisex.iLines ; iLc++) {
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
				break;
				
			case tpValByte :
				for (iLc = 0; iLc < gisex.iLines ; iLc++) {
					if (trq.fUpdate(iLc,gisex.iLines )) return;
					gisex.mp->GetLineVal( iLc, gisex.lBuf );
					for (iRc = 0; iRc < gisex.iCols ; iRc++ )
						gisex.bBuf[iRc] = byteConv(gisex.lBuf[iRc]);
					gisex.filGISLAN->Write(gisex.iCols, gisex.bBuf.buf());
				}
				
				break;
				
			case tpValInt :
				for (iLc = 0; iLc < gisex.iLines ; iLc++) {
					if (trq.fUpdate(iLc,gisex.iLines )) return;
					gisex.mp->GetLineVal( iLc, gisex.lBuf );
					for (iRc = 0; iRc < gisex.iCols ; iRc++ )
						gisex.iBuf[iRc] = shortConv(gisex.lBuf[iRc]);
					gisex.filGISLAN->Write(gisex.iCols * 2, gisex.iBuf.buf());
				}
				
				break;
		}
		// add fillers to file to extend last block to 512 byte
		long iFileFill = (gisex.filGISLAN->iSize()) % 512;
		if ( iFileFill != 0 )
		{
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
