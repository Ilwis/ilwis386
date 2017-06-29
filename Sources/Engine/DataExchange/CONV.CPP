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
/* Conv.c
  by Willem Nieuwenhuis 21 feb 1996
  Added: ImportWithHeader for raster import "other"
  Last change:  WK   22 Sep 98   10:51 am
*/
#include "Headers\toolspch.h"
#include <direct.h>
#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\Engine.h"
#include "Engine\DataExchange\Convloc.h"
#include "Engine\DataExchange\CONV.H"
#include "Engine\DataExchange\MAPIMP.H"
#include "Engine\Map\Raster\MAPSTORE.H"
#include "Engine\DataExchange\SEGIMP.H"
#include "Engine\Map\Segment\SEGSTORE.H"
#include "Engine\DataExchange\POLIMP.H"
#include "Engine\Map\Polygon\POLSTORE.H"
#include "Engine\DataExchange\PNTIMP.H"
#include "Engine\Map\Point\PNTSTORE.H"
#include "Engine\DataExchange\Tblimp.h"
#include "Engine\Table\tblstore.h"
#include "Engine\Map\Raster\MapList\maplist.h"
#include "Engine\DataExchange\ForeignCollection.h"
#include "Headers\Hs\IMPEXP.hs"
#include "Engine\Base\File\Directory.h"
#include "Headers\constant.h"

ImpExp::ImpExp(CWnd *w) : win(w)
{
  trq.Start();
}

ImpExp::~ImpExp()
{
  trq.Stop();
}

/* The Import function expects three arguments:
    1. A foreign format, also including extension (fnFile)
    2. An Ilwis object, including extension       (fnObject)
    3. The name of the package to convert from    (sExtFormat)
	4. Extra options
    5. Window for displaying forms
   The Ilwis1.4 conversion is handled separately
*/
void Import(const FileName& fnFile, const FileName& fnObject, const String& sExtFmt, const String& sOptions, CWnd* w)
{
	try {
		ImpExp impexp(w);
		String sExtFormat = sExtFmt.sTrimSpaces();
		if (fCIStrEqual(sExtFormat,"ArcInfoNAS")) {
			File FileIn (fnFile);
			impexp.ImportNAS(FileIn, fnObject);
		}
		else if (fCIStrEqual(sExtFormat, "Erdas"))
			impexp.ImportErdas(fnFile, fnObject);
		else if (fCIStrEqual(sExtFormat, "Ascii")) {
			File FileIn (fnFile);
			impexp.ImportASC(FileIn, fnObject);
		}
		else if (fCIStrEqual(sExtFormat,"IDA")) {
			File FileIn (fnFile);
			impexp.ImportIMG(FileIn, fnObject);
		}
		else if (fCIStrEqual(sExtFormat, "IDRISI")) {
			File FileIn (fnFile);
			impexp.ImportIDRISI(fnFile, fnObject);
		}
		else if (fCIStrEqual(sExtFormat, "Bmp")) {
			File FileIn (fnFile);
			impexp.ImportBMP(FileIn, fnObject);
		}
		else if (fCIStrEqual(sExtFormat, "Gif")) {
			File FileIn (fnFile);
			impexp.ImportGIF(FileIn, fnObject);
		}
		else if (fCIStrEqual(sExtFormat,"Pcx")) {
			File FileIn (fnFile);
			impexp.ImportPCX(FileIn, fnObject);
		}
		else if (fCIStrEqual(sExtFormat, "Tiff")) {
			File FileIn (fnFile);
			impexp.ImportTIF(FileIn, fnObject);
		}
		else if (fCIStrEqual(sExtFormat,"BNA"))
			impexp.ImportBNA(fnFile, fnObject);
		else if (fCIStrEqual(sExtFormat, "DXF"))
			impexp.ImportDXF(fnFile, fnObject);
		else if (fCIStrEqual(sExtFormat, "E00"))
			impexp.ImportE00(fnFile, fnObject, sOptions);
		else if (fCIStrEqual(sExtFormat, "LIN")) {
			if (fCIStrEqual(fnFile.sExt, ".PTS"))
				impexp.ImportPtsAi(fnFile, fnObject);
			else
				impexp.ImportLIN(fnFile, fnObject);
		}
		else if (fCIStrEqual(sExtFormat,"SMT"))
			impexp.ImportSMT(fnFile, fnObject);
		else if (fCIStrEqual(sExtFormat, "dBase"))
		{
			String sCmd = String("%S=table(%S,DBF,convert,none)", 
				                 fnObject.sRelativeQuoted(true), 
				                 fnFile.sRelativeQuoted(true));

			getEngine()->Execute(sCmd);

		}
		else if (fCIStrEqual(sExtFormat, "Shape"))
			impexp.ImportShapeFile(fnFile, fnObject);      
		else if (fCIStrEqual(sExtFormat, "GeoGateway"))
		{
			String sCmd("open %S -import -combine -noshow", fnFile.sFullPathQuoted());
			getEngine()->Execute(sCmd);
			//ForeignCollection fc(fnObject);

			//fc->Create(String("GeoGateway(%S, import, combine)", fnFile.sFullPathQuoted()));
		}
		else if (fCIStrEqual(sExtFormat, "Gartrip"))
			impexp.ImportGartrip(fnFile, fnObject, sOptions);
		else if (fCIStrEqual(sExtFormat, "gdal"))
		{
			// String sGDAL_DATA_Folder = getEngine()->getContext()->sIlwDir() + "gdal_data"; // for --config GDAL_DATA '%S'
			// postponed above statement as this is only supported in gdal 12 and later
			String sCmd("!gdal_translate.exe '-of' ILWIS '%S' '%S'", fnFile.sFullPath(), fnObject.sFullPath());
			getEngine()->Execute(sCmd);

			// above statement will block til the import is completed

			// now update the catalog (unless fNoUpdate is set to true)
			bool *fNoUpdate = (bool *)(getEngine()->pGetThreadLocalVar(IlwisAppContext::tlvNOUPDATECATALOG));
			if (fNoUpdate != 0)  // pointer must be valid
				if (!(*fNoUpdate))
				{
					FileName* pfn = new FileName(fnObject);
					getEngine()->PostMessage(ILW_READCATALOG, 0, (long)pfn);
				}
		}
		else
			throw ErrorObject(String("%s: \"%S\"", TR("Unknown conversion format").c_str(), sExtFormat));
	}
	catch (ErrorImportExport& err)
	{
		String sErr = err.sExcError();
		getEngine()->Message(sErr.c_str(), TR("Import Error").c_str(), MB_ICONSTOP|MB_OK);
	}
}


/* The Import function expects the following arguments:
    1.  A foreign format, also including extension (fnFile)
    2.  An Ilwis object, including extension       (fnObject)
    3.  The size of the header to skip in bytes    (iHeaderSize)
    4.  The number of bands                        (iNrBands}
    5.  The number of columns                      (iNrCols)
    6.  RasterPixelStructure (bits, bytes, floats) (irps)
    7.  Nr of bytes per pixel (bit->bits per pixel) 
    8.  Are the bands stored interleaved?          (rfs)
    9.  Make a copy of the data in the alien file  (fCopyData)
   10.  Perform a byteswap when high-endian data   (fByteSwap)
   The Ilwis1.4 conversion is handled separately
*/
void _export ImportWithHeader(const FileName& fnFile, const FileName& fnObject,
                              long iHeaderSize, long iNrBands, long iNrCols,
                              ImportRasterPixelStructure irps, int iNrBytesPerPixel, ImportRasterFileStructure irfs,
                              bool fUseAs, bool fByteSwap, bool fCreateMPR, const String& sDescr)
{
	try {
		ImpExp impexp;
		impexp.ImportOTH(fnFile, fnObject, iHeaderSize, iNrBands, iNrCols,
			irps, iNrBytesPerPixel, irfs, fUseAs, fByteSwap, fCreateMPR, sDescr);
	}
	catch (CException* err) {
		err->ReportError(MB_OK|MB_ICONHAND);
		err->Delete();
		throw ErrorObject();
	}
}

/* The Export function expects three arguments:
    1. An Ilwis object, including extension       (fnObject)
    2. A foreign format, also including extension (fnFile)
    3. The name of the package to convert to      (sExtFormat)
*/
void Export(const FileName& fnObject, const FileName& fnFile, const String& sExtFmt, const String& sMethod)
{
  try {
    ImpExp impexp;
    String sTemp = fnObject.sExt;
    sTemp.toLower();
    String sExtFormat = sExtFmt.sTrimSpaces();
    if (sTemp == ".mpr")
      impexp.ExpRas(fnObject, fnFile, sExtFormat, sMethod);
    else if (sTemp == ".mps")
      impexp.ExpSeg(fnObject, fnFile, sExtFormat);
    else if (sTemp == ".mpa")
      impexp.ExpPol(fnObject, fnFile, sExtFormat);
    else if (sTemp == ".mpp")
      impexp.ExpPnt(fnObject, fnFile, sExtFormat);
    else if (sTemp == ".mpl")
      impexp.ExpMpl(fnObject, fnFile, sExtFormat, sMethod);
    else if (sTemp == ".csy")
      impexp.ExpCsy(fnObject, fnFile, sExtFormat);
    else //if (sTemp == ".tbt")
      impexp.ExpTbl(fnObject, fnFile, sExtFormat);
  }
  catch (ErrorImportExport& err) {
    String sErr = err.sExcError();
	//AfxGetMainWnd()->MessageBox(sErr.c_str(), SCVErrExportError.c_str(), MB_ICONSTOP|MB_OK);
  }    
}

// syntax of the command:
//    import method(input, IlwisObj[, Options])
// the 'import' command is already stripped off
void Import(const String& sExpr)
{
	String sFormat = IlwisObjectPtr::sParseFunc(sExpr).sTrimSpaces().toLower();
	Array<String> as;
	short iParms = IlwisObjectPtr::iParseParm(sExpr, as);
	try
	{
		//if (iParms < 2)
		//	throw ErrorObject(String(SCVErrIncorrectParam_S.c_str(), sExpr.sTrimSpaces()));

		FileName fn = FileName(as[1]);
		Directory dir(fn); 
		if (!fn.fValid() || !Directory::fIsExistingDir(dir))
			throw ErrorObject(WhatError(TR("Invalid file name"), errNAME), fn);

		if (sFormat.sLeft(7) == "ilwis14")
		{
			ImportFrom14(as[0], as[1], sFormat);
		}
		else
		{
			String sOptions;
			for (int i = 2; i < as.size(); i++)
				sOptions &= String(" %S", as[i]);
			Import(as[0], as[1], sFormat, sOptions);
		}
	}
	catch (ErrorObject& err)
	{
		err.SetTitle(TR("Import Error"));
		err.Show();
	}
	catch (CException* err)
	{
    MessageBeep(MB_ICONHAND);
		err->ReportError(MB_OK|MB_ICONHAND|MB_TOPMOST);
		err->Delete();
	}		
}
 
void ImportFrom14(const FileName& fn, const FileName& fnOut, const String& sFormat)
{
  if (fCIStrEqual(sFormat, "ilwis14ras"))
    MapImport::import(fn, fnOut);
  else if (fCIStrEqual(sFormat, "ilwis14seg"))
    SegmentMapImport::import(fn, fnOut);
  else if (fCIStrEqual(sFormat, "ilwis14pnt"))
    PointMapImport::import(fn, fnOut);
  else if (fCIStrEqual(sFormat, "ilwis14pol"))
    PolygonMapImport::import(fn, fnOut);
  else if (fCIStrEqual(sFormat, "ilwis14tbl"))
    TableImport::import(fn, ".tbt", fnOut);
}


// syntax of the command:
//    export method(IlwisObj, output)
// the 'export' command is already stripped off
void Export(const String& sExpr) {
	Array<String> as;
	Split(sExpr, as, "(,)");
	if (as.iSize() != 0)
	{
		for (int i=0; i<as.iSize(); ++i)
			as[i] = as[i].sTrimSpaces(); // no harm; if not done, one could accidentally add spaces to filenames
		String sFormat = as[0];
		if (fCIStrEqual(sFormat, "ilwis14"))
			ExportTo14(as[1], as[2]);
		else
		{
			String sMethod;
			if ( as.size() == 4 )
				sMethod = as[3];
			FileName fn = FileName(as[2]);
			Directory dir(fn); 
			if (!fn.fValid() || !Directory::fIsExistingDir(dir))
				throw ErrorObject(WhatError(TR("Invalid file name"), errNAME), fn);
			Export(as[1], as[2], sFormat, sMethod);
		}			
	}    
}

// Add this local function to avoid displaying the Tranquilizer for pol export
void InitTrq(Tranquilizer& trq)
{
	trq.SetTitle(SIETitleExportTo14);
	trq.SetDelayShow(false);
	trq.SetNoStopButton(true);
	trq.fText(SIEMOpeningGDB_S);
}

// Export to ILWIS 1.4 is already implemented in dsp, but
// there it uses a zFrameWin*. This one will work without
// DSP dependencies.
void ExportTo14(const FileName& fn, const FileName& fnOut)
{
	try {
		Tranquilizer trq;
		if (fCIStrEqual(".mpr", fn.sExt)) {
			InitTrq(trq);
			Map mp(fn);
			mp.Export(fnOut);
		}
		else if (fCIStrEqual(".mps", fn.sExt)) {
			InitTrq(trq);
			SegmentMap sm(fn);
			sm.Export(fnOut);
		}
		else if (fCIStrEqual(".mpa", fn.sExt)) {
			throw ErrorObject(TR("Polygon export to 1.4 is not supported anymore"));
//			PolygonMap pm(fn);
//			pm.Export(fnOut);
		}
		else if (fCIStrEqual(".mpp", fn.sExt)) {
			InitTrq(trq);
			PointMap ppm(fn);
			ppm.Export(fnOut);
		}
		else if (fCIStrEqual(".tbt", fn.sExt)) {
			InitTrq(trq);
			Table tbl(fn);
			tbl.Export(fnOut);
		}
		else if (fCIStrEqual(".his", fn.sExt)) {
			InitTrq(trq);
			Table tbl(fn);
			tbl.Export(fnOut);
		}
		else if (fCIStrEqual(".hss", fn.sExt)) {
			InitTrq(trq);
			Table tbl(fn);
			tbl.Export(fnOut);
		}
		else if (fCIStrEqual(".hsa", fn.sExt)) {
			InitTrq(trq);
			Table tbl(fn);
			tbl.Export(fnOut);
		}
		else if (fCIStrEqual(".hsp", fn.sExt)) {
			InitTrq(trq);
			Table tbl(fn);
			tbl.Export(fnOut);
		}
		trq.Stop();
	}
	catch (ErrorObject& err) {
		err.Show();
	}    
}
