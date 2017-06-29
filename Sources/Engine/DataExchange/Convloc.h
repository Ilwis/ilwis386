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
/* $Log: /ILWIS 3.0/Import_Export/Convloc.h $
 * 
 * 16    25-09-03 16:39 Retsios
 * Merge from GARtrip
 * 
 * 17    19-05-03 19:49 Retsios
 * Added gartrip import
 * 
 * 15    3-09-02 14:07 Hendrikse
 * added functions and prototypes for Export Coordsys to Esri prj file
 * 
 * 13    10-01-01 4:31p Martin
 * ExpRas has an extra parm 'sMethod'. Used for Geogateway
 * 
 * 12    8/11/00 17:26 Willem
 * Extended the import function to deal with extra options for commands.
 * Also updated the import e00 function prototype
 * 
 * 11    27/10/00 18:12 Willem
 * Import general raster can now set a user defined description (only from
 * the form though). Therefore the appropriate functions now have a
 * default string parameter
 * 
 * 10    27/10/00 12:16 Willem
 * - Removed the impras.cpp file. It was no longer used
 * 
 * 9     24/08/00 10:55 Willem
 * Added export to Arc/Info ASCII grid (again)
 * 
 * 8     29/02/00 17:30 Willem
 * Removed superfluous function prototype
 * 
 * 7     29/02/00 11:53 Willem
 * Added Atlas IDA raster export
 * 
 * 6     18-11-99 12:08 Wind
 * import general raster bugs and removal of superfluous fCopyData
 * parameter
 * 
 * 5     9/27/99 11:19a Wind
 * added support for maps in maplists without mpr and use of original data
 * file for general raster import
 * 
 * 4     9/20/99 12:59p Wind
 * added flag fUseAs to general raster import
 * 
 * 3     17-06-99 3:59p Martin
 * // -> /*
 * 
 * 2     17-06-99 2:09p Martin
 * ported files to VS
// Revision 1.6  1998/09/17 09:11:32  Wim
// 22beta2
//
// Revision 1.5  1998/06/10 13:06:42  Willem
// The ILWIS 1.4 conversions will now also work from the script.
//
// Revision 1.4  1998/05/26 18:39:40  Willem
// >> The export from script now correctly calls export functions. Moved the
// >> export to non-1.4 export from src\dsp\app\export.c to here and let the
// >> UI call this code instead. The code is now called from the UI and from
// >> the script.
// (sorry for the extra >>)
//
// Revision 1.3  1997/08/08 11:33:07  Dick
// Added Idrisi import
//
// Revision 1.2  1997/08/05 20:56:58  Willem
// Created new file to be used to implement exception objects for
// all conversion. This will also be used to administrate error
// numbers for conversions.
//
/* Convloc.h
   Interface setup by
   (c) Ilwis System Development ITC
   Last change:  WK   15 Jun 98    2:57 pm
*/

#ifndef ILW_CONVERT_LOCAL
#define ILW_CONVERT_LOCAL
#include "Headers\toolspch.h"
#include "Engine\DataExchange\CONV.H"
#include "Headers\Hs\CONV.hs"
#include "Engine\DataExchange\CONVERR.H"
//#include "dat.h"

class ImpExp
{
public:
  ImpExp(CWnd *w=NULL);
  ~ImpExp();

  void ImpSeg(const FileName& fnFile, const FileName& fnObject);
  void ImpPol(const FileName& fnFile, const FileName& fnObject);
  void ImpPnt(const FileName& fnFile, const FileName& fnObject);
  void ImpTbl(const FileName& fnFile, const FileName& fnObject);
  void ExpRas(const FileName& fnObject, const FileName& fnFile, const String& sExtPormat, const String& sMethod="" );
  void ExpSeg(const FileName& fnObject, const FileName& fnFile, const String& sExtPormat);
  void ExpPol(const FileName& fnObject, const FileName& fnFile, const String& sExtPormat);
  void ExpTbl(const FileName& fnObject, const FileName& fnFile, const String& sExtPormat);
  void ExpPnt(const FileName& fnObject, const FileName& fnFile, const String& sExtPormat);
  void ExpMpl(const FileName& fnObject, const FileName& fnFile, const String& sExtPormat, const String& sMethod="" );
  void ExpCsy(const FileName& fnObject, const FileName& fnFile, const String& sExtPormat);

// import on package
  void ImportIlwis14(const FileName& fnFile, const FileName& fnObject);
  void ImportArcInfo(const FileName& fnFile, const FileName& fnObject);
  void ImportErdas(const FileName& fnFile, const FileName& fnObject);

// export raster
  void ExportGIS(const FileName&, const FileName&);
  void ExportBMP(const FileName&, const FileName&);
  void ExportIDRISI(const FileName&, const FileName&);
  void ExportASCII(const FileName&, const FileName&);
  void ExportGeoTiff(const FileName&, const FileName&);
	void ExportIDA(const FileName& fnObject, const FileName& fnFile);
	void ExportNAS(const FileName&, const FileName&);
	void ExportGeoGateway(const FileName& fnIn, const FileName& fnOut, const String& sFormat );

// export maplist
  void ExportLAN(const FileName&, const FileName&);

	// export coordsystem
  void ExportEsriPrj(const FileName&, const FileName&);

// import raster
  void ImportASC(File& fnFile, const FileName& fnObject);
  void ImportBMP(File& fnFile, const FileName& fnObject);
  void ImportGIF(File& fnFile, const FileName& fnObject);
  void ImportGISLAN(File& fnFile, const FileName& fnObject);
//  void ImportGRD(File& fnFile, const FileName& fnObject);
  void ImportIDRISI(const FileName& fnFile, const FileName& fnObject);
  void ImportIMG(File& fnFile, const FileName& fnObject);
//  void ImportLIP(File& fnFile, const FileName& fnObject);
  void ImportNAS(File& fnFile, const FileName& fnObject);
  void ImportOTH(const FileName& fnFile, const FileName& fnObject,
                 long iHeaderSize, long iNrBands, long iNrCols,
                 ImportRasterPixelStructure, int iNrBytesPerPixel, ImportRasterFileStructure,
                 bool fUseAs, bool fByteSwap, bool fCreateMPR, const String& sDescr = "");
  void ImportPCX(File& fnFile, const FileName& fnObject);
  void ImportTIF(File& fnFile, const FileName& fnObject);

// export tables
  void ExportDBF(const FileName& fnObject, const FileName& fnFile);
  void ExportSDF(const FileName& fnObject, const FileName& fnFile);
  void ExportDelim(const FileName& fnObject, const FileName& fnFile);

// import tables
  void ImportDBF(const FileName& fnObject, const FileName& fnFile);

// export vectors
  void ExpPntToSHP( const FileName& fnObject, const FileName& fnFile);
  void ExpSegToSHP( const FileName& fnObject, const FileName& fnFile );
  void ExpPolToSHP( const FileName& fnObject, const FileName& fnFile );
  void ExportSMT(const FileName& fnObject, const FileName& fnFile);
  void ExportE00(const FileName& fnObject, const FileName& fnFile);
  void ExportLIN(const FileName& fnObject, const FileName& fnFile);
  void ExportPntAi(const FileName& fnObject, const FileName& fnFile);
  void ExportBNA(const FileName& fnObject, const FileName& fnFile);
  void ExportDXF(const FileName& fnObject, const FileName& fnFile);
  void ExportPnt2Infocam(const FileName& fnObject, const FileName& fnFile);
  void ExportSegment2Infocam(const FileName& fnObject, const FileName& fnFile);
  void ExportPolygon2Infocam(const FileName& fnObject, const FileName& fnFile);

// import vectors
  void ImportBNA(const FileName& fnFile, const FileName& fnObject);
  void ImportDXF(const FileName& fnFile, const FileName& fnObject);
  void ImportE00(const FileName& fnFile, const FileName& fnObject, const String& sOptions);
  void ImportLIN(const FileName& fnFile, const FileName& fnObject);
  void ImportPtsAi(const FileName& fnFile, const FileName& fnObject);
  void ImportSMT(const FileName& fnFile, const FileName& fnObject);
  void ImportInfocam2Ilwis(const FileName& fnFile, const FileName& fnObject);
  void ImportShapeFile(const FileName& fnFile, const FileName& fnObject);
	void ImportGartrip(const FileName& fnFile, const FileName& fnObject, const String& sOptions);

private:
  Tranquilizer trq;
  CWnd* win;
};

class ErrorImportExport {
public:
  ErrorImportExport(const String& s) { ss=s; sExcError(); }
  String sExcError() { return ss; }
private:
  String ss;
};

// Import from ILWIS 1.4 is already implemented in the dsp, but
// it is tightly coupled to the user interface. This function will
// limit itself to just calling the correct ILWIS 1.4 import
void ImportFrom14(const FileName& fn, const FileName& fnOut, const String& sFormat);

// Export to ILWIS 1.4 is already implemented in dsp, but
// there it uses a zFrameWin*. This one will work without
// DSP dependencies.
void ExportTo14(const FileName& fn, const FileName& fnOut);

#endif // ILW_CONVERT_LOCAL


