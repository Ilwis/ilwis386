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
/* $Log: /ILWIS 3.0/RasterStorage/Map.cpp $
 * 
 * 67    19-05-05 17:57 Retsios
 * [bug=6424] Properly open the maps in case the supplied attribute column
 * contains spaces. Added a safeguard to p to reduce the chance of reading
 * characters outside the string bounds.
 * 
 * 66    24-01-03 15:35 Willem
 * - Added: Do not add the datafile of the Map to the list of files to
 * delete, when the Map is UseAs
 * 
 * 65    20-01-03 15:18 Willem
 * - Added: catch for UserAbort error. This is thrown when a user
 * interrupts the import process and it also prevents messages to the user
 * (which are not needed)
 * 
 * 64    12/05/01 10:27 Willem
 * The histogram of all maptypes is now also copied or deleted with the
 * map it belongs to
 * 
 * 63    12/04/01 18:14 Willem
 * The histogram of a rastermap (if any) is now also considered to belong
 * to the object
 * 
 * 62    19-11-01 14:45 Retsios
 * solved LFN bug (#5386): "path widh dot is causing problems" because it
 * is quoted.
 * 
 * 61    10/29/01 12:58p Martin
 * fHasPyramidFile is nu const geworden
 * 
 * 60    8/23/01 17:19 Willem
 * Removed the SetReadOnly() function. This is now handled by
 * IlwisObjectPtr::SetReadOnly() for all ilwis objects
 * 
 * 59    6/19/01 11:06a Martin
 * maps can now tell how many pyramid layers they have (to ensure that not
 * an impossible layer is used)
 * 
 * 58    6/15/01 10:23a Martin
 * corrected a typo in the mapstore section
 * 
 * 57    6/15/01 10:22a Martin
 * pyramidfile added to the getobjectstructure
 * 
 * 56    6/14/01 11:08a Martin
 * added support for deleting pyramidfiles
 * 
 * 55    6/13/01 4:08p Martin
 * map can check if it has a pyramid file
 * 
 * 54    6/05/01 4:28p Martin
 * added interface for creating pyramid files and accessing the layers in
 * a file
 * 
 * 53    13-03-01 11:17 Koolhoven
 * MapPtr::create() was leaving Map in memory when  MapVirtual::create()
 * throwed an error
 * 
 * 52    6/03/01 12:15 Willem
 * Map will now silently updates the CSY of the map, when it is different
 * from that of the GeoRef. It does not change the ILWIS date, but does
 * change the filedate of the ODF
 * 
 * 51    27/02/01 14:39 Willem
 * The name of internal maplist bands now includes the prefix as well.
 * 
 * 50    2/08/01 12:15 Retsios
 * Solved some potential "dot in path confused for extension's dot" bugs.
 * 
 * 49    29/01/01 10:46 Willem
 * The coordinate system of a map is always synchronized with the
 * coordinate system of the georef now, the map takes over the csy of the
 * georef if they are different.
 * 
 * 48    23-01-01 11:03a Martin
 * useas is passed as a parm with the constructor
 * 
 * 47    10-01-01 4:25p Martin
 * link through of the the StoreAs function in MapStore
 * 
 * 46    21-12-00 10:05a Martin
 * added the pGetVirtualObject() function to access the virtual object
 * embedded in the object. 
 * 
 * 45    6-12-00 14:57 Koolhoven
 * in MapPtr::create() handle maplist:nr properly also in case extension
 * is not mentioned
 * 
 * 44    24-11-00 17:31 Koolhoven
 * in case 0 == pms return in st() the dvs.st() 
 * 
 * 43    23/11/00 10:43 Willem
 * Added st() virtual function: the StoreType is now retrieved from
 * MapStore
 * 
 * 42    20-11-00 3:23p Martin
 * moved the foreignfromat file from the getobjectstructure to ilwisobject
 * 
 * 41    14/11/00 16:42 Willem
 * The description parameter was checked at the wrong index (11 instead of
 * 8)
 * 
 * 40    14/11/00 14:25 Willem
 * The description from the import general raster is now also passed to
 * the import function
 * 
 * 39    27-10-00 10:00a Martin
 * added foreign format to objectstructure
 * 
 * 38    24-10-00 11:52a Martin
 * changed the getobjectstructure function
 * 
 * 37    13-10-00 15:07 Hendrikse
 * in OpenMapVirtual() load ObjectDependcency after the creation of the
 * MapVirtual. This prevents a.o.a problems with internal tables and
 * columns (MapAttribute)
 * 
 * 36    3-10-00 2:44p Martin
 * maps which are created with unbound parms will throw a StopScriptError
 * 
 * 35    13/09/00 14:46 Willem
 * The map constructor added the map postfix a second time
 * 
 * 34    11-09-00 11:09a Martin
 * add functions for GetObjectStructure and DoNotUpdate
 * 
 * 33    17-03-00 9:54a Martin
 * extra constructors for foreignformat support
 * 
 * 32    8-03-00 9:32 Wind
 * problem with : for maps in maplist
 * 
 * 31    8-02-00 14:16 Wind
 * small change in general raster import
 * 
 * 30    17-01-00 15:57 Wind
 * Set default proximity to pixelsize
 * 
 * 29    17-01-00 11:30 Wind
 * added proximity to iValue(const Coord& ..) etc.
 * 
 * 28    12/01/00 9:36 Willem
 * Removed superfluous statement that caused confusion
 * 
 * 27    4-01-00 15:18 Wind
 * debugged cleanup after error when creating dependent map
 * 
 * 26    23-12-99 10:17 Wind
 * bug in general raster import
 * 
 * 25    22-12-99 18:13 Koolhoven
 * Derived ElementContainer from FileName
 * it has ElementMap* as a member.
 * In ObjectInfo::ReadElement and ObjectInfo::WriteElement the ElementMap
 * is used when available instead of the file.
 * ElementMap is initialized in MapCompositionDoc::Serialize to load/store
 * a MapView.
 * Further has IlwisObjectPtr now a SetFileName() function to prevent
 * everythere the const_casts of fnObj.
 * 
 * 24    10-12-99 12:44 Wind
 * s = as[i].toLower() doesn't work with new array; replaced by other code
 * 
 * 23    6-12-99 10:57 Wind
 * delete ODF if error occurs in creation of MapVirtual 
 * 
 * 22    26-11-99 10:50 Wind
 * small change to make MapComputedElsewhere working
 * 
 * 21    24-11-99 19:09 Koolhoven
 * did not compile, so corrected on first view
 * 
 * 20    24-11-99 17:15 Wind
 * removed thread for import of data
 * 
 * 19    18-11-99 12:07 Wind
 * import general raster bugs
 * 
 * 18    18-11-99 10:24 Wind
 * added error messages for import of general raster maps
 * 
 * 17    29-10-99 9:20 Wind
 * thread save stuff
 * 
 * 16    25-10-99 13:14 Wind
 * making thread save (2); not yet finished
 * 
 * 15    22-10-99 12:54 Wind
 * thread save access (not yet finished)
 * 
 * 14    10/07/99 1:19p Wind
 * set the path in import thread to current dir
 * 
 * 13    10/07/99 12:43p Wind
 * 
 * 12    10/07/99 12:22p Wind
 * import general raster in it's own thread
 * 
 * 11    10/06/99 5:17p Wind
 * added import of general raster 
 * 
 * 10    5-10-99 18:51 Koolhoven
 * Using of maplist:nr syntax to access a rastermap in a maplist
 * 
 * 9     10/01/99 1:09p Wind
 * debugged for maps with syntax maplist:bandnr
 * 
 * 8     9/29/99 4:26p Wind
 * syntax maplist:bandnr now also works for maplists with maps with mpr
 * file
 * 
 * 7     9/29/99 11:05a Wind
 * added support for maps thata are a band in a maplist without mpr and
 * objectinfo in maplist
 * 
 * 6     9/20/99 12:35p Wind
 * allow syntax maplist[^band_nr^] for mapname
 * 
 * 5     9-09-99 8:15a Martin
 * added 2.22 stuff
 * 
 * 4     9/08/99 10:25a Wind
 * adpated to use of quoted file names
 * 
 * 3     24-03-99 15:59 Koolhoven
 * Header comment
 * 
 * 2     3/22/99 11:06a Martin
 * Case problem solved
 */
// Revision 1.12  1998/09/16 17:22:46  Wim
// 22beta2
//
// Revision 1.11  1997/12/01 08:03:31  Wim
// MapPtr() constructor now sets the coordsystem to the
// coordsystem of its georef.
//
// Revision 1.10  1997-09-16 18:55:46+02  Wim
// in SetGeoRef() check if the georef is not a georef3d and
// the map is its mapDTM
//
// Revision 1.9  1997-09-11 20:40:41+02  Wim
// Allow to break dependencies even when source objects are gone
//
// Revision 1.8  1997-09-11 18:03:20+02  Wim
// First remove old rubbish before creating a new map in MapPtr::create()
//
// Revision 1.7  1997-09-09 14:02:07+02  Wim
// In Store() make sure the map remains dependent even when
// the virtual map has not been loaded into memory
//
// Revision 1.6  1997-09-04 12:45:58+02  Wim
// Delete Histogram in DeleteCalc() function
//
// Revision 1.5  1997-08-22 11:06:17+02  Wim
// Store() in Map() constructor after MapPtr is succesfully created
//
/* MapPtr
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  WK   24 Apr 98    4:38 pm
*/
#define MAP_C
#include "Engine\Map\Raster\Map.h"
#include "Engine\Map\Raster\MAPSTORE.H"
#include "Engine\Applications\MAPVIRT.H"
//#include "Applications\Raster\MAPATTRB.H"
//#include "Applications\Raster\MAPCOLSP.H"
#include "Engine\Domain\Dmvalue.h"
#include "Engine\Map\Raster\MapList\maplist.h"
#include "Engine\DataExchange\ForeignFormat.h"
#include "Engine\Table\TBLHIST.H"
#include "Engine\Base\DataObjects\ObjectStructure.h"
#include "Engine\SpatialReference\Grsmpl.h"
#include "Engine\SpatialReference\GR3D.H"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Base\DataObjects\Tranq.h"
#include "Engine\Base\System\mutex.h"
#include "Headers\Hs\map.hs"
#include "Headers\Hs\CONV.hs"
#include "Engine\DataExchange\CONV.H"
#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\Engine.h"


static void InvalidFileNameError(const FileName& fn)
{
  throw ErrorObject(WhatError(TR("Invalid file name"), errNAME), fn);
}  

FileName Map::fnFromExpression(const String& sExpr)
{
  char *p = sExpr.strrchrQuoted('.');
  // Bas Retsios, 8 February 2001: *p is not necessarily an extension's dot
  char *q = sExpr.strrchrQuoted('\\');
  // *p is definitely not an extension's dot if p<q
  if (p<q)
  	p = 0;
  // Now we're (only) sure that if p!=0 it points to the last dot in the filename
  if ((p == 0) || (0 == _strcmpi(p, ".mpr"))) { // no attrib map
    FileName fn(sExpr);
    if (fn.sSectionPostFix.length() > 0)
      fn.sExt = ".mpl";
    else
      fn.sExt = ".mpr";
    return fn;
  }
  return FileName();
}

Map::Map()
{
}

Map::Map(const FileName& fn, const String& sExpression)
{
  FileName fnMap(fn, ".mpr", false);
  MutexFileName mut(fnMap);
	String sHead = sExpression.sTail("(");
  Array<String> arParts;
	Split(sHead, arParts, ",");
	for(unsigned int i=0; i<arParts.size(); ++i)
	{
		if (arParts[i][0] == '%' && isdigit(arParts[i][1]))
			throw StopScriptError(String(TR("Missing parameter : %S").c_str(), arParts[i]));
	}		
  SetPointer(MapPtr::create(fnMap,sExpression));
  if (fValid())
    ptr()->Store();
}

Map::Map(const FileName& fn, long iBandNr)
{
  FileName filnam = fn;
  if (iBandNr != iUNDEF)
    filnam.sSectionPostFix = String(":%li", iBandNr);
  SetPointer(new MapPtr(filnam, false, iBandNr));
}


Map::Map(const String& sExpression)
{
  FileName fnMap = fnFromExpression(sExpression); 
  MutexFileName mut(fnMap);
  if (fnMap.fValid())
    SetPointer(MapPtr::create(fnMap));
  else 
    SetPointer(MapPtr::create(FileName(),sExpression));
}

Map::Map(const FileName& fn, const LayerInfo& inf)
{
	SetPointer(new MapPtr(fn, inf));
}

int Map::iColorType(const String& sColor)
{
  if (fCIStrEqual(sColor , "red"))
    return 0; 
  if (fCIStrEqual(sColor , "green"))
    return 1; 
  if (fCIStrEqual(sColor , "blue"))
    return 2; 
  if (fCIStrEqual(sColor , "yellow"))
    return 3; 
  if (fCIStrEqual(sColor , "magenta"))
    return 4; 
  if (fCIStrEqual(sColor , "cyan"))
    return 5; 
  if (fCIStrEqual(sColor , "hue"))
    return 6; 
  if ((fCIStrEqual(sColor , "sat")) || (fCIStrEqual(sColor , "saturation")))
    return 7; 
  if ((fCIStrEqual(sColor , "intens")) || (fCIStrEqual(sColor , "intensity")))
    return 8; 
  if ((fCIStrEqual(sColor , "grey")) || (fCIStrEqual(sColor , "gray")))
    return 9; 
  return shUNDEF;
}


Map::Map(const String& sExpression, const String& sPath)
{
  String sExpr =  sExpression;
   // check if it's a dependent map
  if (0 != sExpr.strchrQuoted('(')) {
    FileName fn;
    fn.Dir(sPath);
    MutexFileName mut(fn);
    SetPointer(MapPtr::create(fn, sExpression));
    return;
  }
 // check if it's a potential attribute map or color separation (i.e. map.red etc)
  char *p = sExpr.strrchrQuoted('.');
  // Bas Retsios, 8 February 2001: *p is not necessarily an extension's dot
	char *pMax = (sExpr.length() > 0) ? (const_cast<char*>(&(sExpr)[sExpr.length() - 1])) : 0; // last valid char in sExpr
  char *q = sExpr.strrchrQuoted('\\');
  // *p is definitely not an extension's dot if p<q
  if (p<q || p>=pMax)
  	p = 0;
  // Now we're (only) sure that if p!=0 it points to the last dot in the filename
  if (p)
    *p = 0;
  String sFile = sExpr.sVal();
  // FileName fn(sFile, ".mpr", true);
	FileName fn(FileName(sFile, FileName(sPath)), ".mpr", true);
  // if (0 == sFile.strchrQuoted(':') && "" != sPath) // no path set and sPath has a value
  //   fn.Dir(sPath);
  MutexFileName mut(fn);
  if (p && (0 != _strcmpi(p+1, "mpr")) && (0 != _strnicmp(p+1, "mpl:", 4))) {
    String sColor = p+1;
    if (shUNDEF != iColorType(sColor)) {
      try {
        Map mp(fn);
        if (mp->dm()->pdp() || mp->dm()->pdcol()) {
//        SetPointer(new MapColorSep(FileName(), ptr(), mp, p+1));
          SetPointer(MapPtr::create(FileName(), String("MapColorSep(%S,%s)", fn.sFullNameQuoted(), p+1)));
          return;
        }  
      }
      catch (const ErrorObject&) {
      }
    }
		if (isalpha(*(p+1)) || ((p<(pMax-1)) && *(p+1) == '\'' && isalpha(*(p+2)))) // attrib column (right side of || checks on quoted column)
      SetPointer(MapPtr::create(FileName(), String("MapAttribute(%S,%s)", fn.sFullNameQuoted(), p+1)));
    else {
      fn.sFile = "";
      fn.sExt = "";
      SetPointer(MapPtr::create(fn, sExpression));
    }
    return;
  }
  // check syntax for map from maplist   'maplist:bandnr'
  p = sExpr.strrchrQuoted(':');
  if ((p != 0) && isdigit(*(p+1))) {
    *p = 0;
    FileName fnMpl(sExpr, ".mpl", true);
    long iBandNr = String(p+1).iVal();
    long iOffsetForBands;
    fnMpl.sSectionPostFix = "";
    ObjectInfo::ReadElement("MapList", "Offset", fnMpl, iOffsetForBands);
    String sMap;
    ObjectInfo::ReadElement("MapList", String("Map%li", iBandNr-1+iOffsetForBands).c_str(), fnMpl, sMap);
    char *p = sMap.strrchrQuoted(':');
    if ((p != 0) && isdigit(*(p+1))) {
      fnMpl.sSectionPostFix = String(":%li", iBandNr);
      MutexFileName mut(fnMpl);
      SetPointer(new MapPtr(fnMpl, false, iBandNr));
    }
    else 
      SetPointer(new MapPtr(sMap));
    return;
  } 
  SetPointer(MapPtr::create(fn));
}

Map::Map(const Map& mp)
: BaseMap(mp.pointer())
{
}

Map::Map(const FileName& fn, const GeoRef& gr, const RowCol& rcSize, const DomainValueRangeStruct& dvs, const MapFormat mf, long iBandNr, bool fUseAs)
{
  FileName filnam = fn;
  if (iBandNr != iUNDEF)
    filnam.sSectionPostFix = String(":%li", iBandNr);
  MutexFileName mut(filnam);
  SetPointer(new MapPtr(FileName(filnam, ".mpr", false), gr, rcSize, dvs, mf, iBandNr, fUseAs));
}


Map::Map(const FileName& fn)
 : BaseMap()
{
    String sMap = fn.sFullNameQuoted(true);
	Map mp(sMap, String());
	SetPointer(mp.ptr());
}

void MapPtr::BreakDependency()
{
  if (!fCalculated())
    Calc();
  if (!fCalculated())
    return; 
  delete pmv;
  pmv = 0;
  fChanged = true;

  WriteElement("Map", "Type", "MapStore");
  Store();
}


StoreType MapPtr::st() const
{
	if (0 == pms)
		return dvs.st();
	return pms->st();
}

MapPtr::~MapPtr()
{
  if (fErase) { // delete histogram
    FileName fnHis = FileName(fnObj, ".his", true);
    if (File::fExist(fnHis)) {
      try { 
        Table his(fnHis);
        his->fErase = true;
      }
      catch (const ErrorObject&) {
      }  
    }  
  }
  if (0 != pms) {
    delete pms;
    pms = 0;
  }  
  if (0 != pmv) {
    delete pmv;
    pmv = 0;
  }  
}


MapPtr* MapPtr::create(const FileName& fn)
{
  if (!File::fExist(fn))
    NotFoundError(fn);
  if (!fn.fValid())
    InvalidFileNameError(fn);
  // see if map is already open (if it is in the list of open base maps)
  MutexFileName mut(fn);
  MapPtr* p = BaseMap::pGetRasMap(fn);
  if (p) // if already open return it
    return p;
  p = new MapPtr(fn);
  return p;
}

bool fImportMap(const FileName& fnMpr, const Array<String>& as)
{
  String sErr;
  try {
    int iParms = as.iSize();
    // parms: (for format == genras
    // input, format, conversion type [UseAs|Convert], NrOfColumns, iHeaderSize,
    // StoreType [Bit|Byte|Int|Real], [1 Byte|2 Byte|4 Byte|8 Byte], 
    // [SwapBytes], [Description]
    if (iParms < 5) {
      // too little parms 
      sErr = String(TR("Too little parameters, at least %i needed").c_str(), 5);
      throw ErrorObject(sErr);
    }
    FileName fnInp(as[0]);
    String sFormat = as[1];
    sFormat.toLower();
    if (sFormat == "genras") {
      // check fixed parms
      String sConversionType = as[2];
      sConversionType.toLower();
      bool fUseAs;
      if (sConversionType == "useas") 
        fUseAs = true;
      else if (sConversionType == "convert") 
        fUseAs =  false;
      else {
        // invalid conversion type
        sErr = String(TR("Incorrect 'conversion type' parameter: %S").c_str(), as[2]);
        throw ErrorObject(sErr);
      }
      long iCols = as[3].iVal();
      if (iCols <= 0) {
        // invalid nr of columns
        sErr = String(TR("Incorrect 'nr. of columns' parameter: %S").c_str(), as[3]);
        throw ErrorObject(sErr);
      }
      // check non fixed parms
      long iHeaderSize = 0;
      if ((iParms > 4) && (as[4].length() > 0)) {
        iHeaderSize = as[4].iVal();
        if (iHeaderSize < 0) {
          // invalid header size
          sErr = String(TR("Incorrect 'header size' parameter: %S").c_str(), as[4]);
          throw ErrorObject(sErr);
        }
      }
      ImportRasterPixelStructure irps = irpsBYTE;
      int iNrBytesPerPixel = 1;
      if ((iParms > 5) && (as[5].length() > 0)){
        String s = as[5];
        s.toLower();
        if (s == "bit") 
          irps = irpsBIT;
        else if (s == "byte") 
          irps = irpsBYTE;
        else if (s == "int") {
          irps = irpsBYTE;
          iNrBytesPerPixel = 2;
        }
        else if (s == "real") {
          irps = irpsFLOAT;
          iNrBytesPerPixel = 4;
        }
        else {
          // invalid raster pixel structure
          sErr = String(TR("Incorrect 'pixel structure' parameter: %S").c_str(), as[5]);
          throw ErrorObject(sErr);
        }
      }
      if ((iParms > 6) && (as[6].length() > 0)){
        int i = as[6].iVal();
        if ((i != 1) && (i != 2) && (i != 4) && (i != 8)) {
          // invalid nr of bytes
          sErr = String(TR("Incorrect 'nr of bytes' parameter: %S").c_str(), as[6]);
          throw ErrorObject(sErr);
        }
        iNrBytesPerPixel = i;
      }
      bool fCopyData = !fUseAs;
      bool fByteSwap = false;
      if ((iParms > 7) && (as[7].length() > 0)){
        String s = as[7];
        s = s.toLower();
        if ((s == "swap") || (s == "swapbytes"))
          fByteSwap = true;
        else if ((s == "noswap") || (s == "noswapbytes"))
          fByteSwap = false;
        else {
          sErr = String(TR("Incorrect 'swap bytes' parameter: %S").c_str(), as[7]);
          throw ErrorObject(sErr);
        }
      }
  	  String sDescr;
	  if ((iParms > 8) && (as[8].length() > 0))
		  sDescr = as[8];

      ImportWithHeader(fnInp, fnMpr, iHeaderSize, 0 /* don't create maplist*/, iCols,
                       irps, iNrBytesPerPixel, irfsBANDSEQUENTIAL, fUseAs, fByteSwap, true, sDescr);
    }
    else {
      sErr = String(TR("Incorrect raster import format: %S").c_str(), as[1]);
      throw ErrorObject(sErr);
    }
  }
  catch (const ErrorUserAbort&)
  {
	  return false;
  }
  catch (const ErrorObject& err) {
    err.Show();
    return false;
  }
  return true;
}

MapPtr* MapPtr::create(const FileName& fn, const String& sExpression)
{
  bool fVirtual = false;
  if (fn.sFile.length() == 0) {
    // no file name, this means that:
    // case 1: sExpression contains map name
    // case 2: sExpression contains map name and attribute column: map.col
    // case 3: sExpression contains any other dependent map
    // case 4: sExpression contains map list name and band nr. 'maplist:bandnr'

    String sExpr =  sExpression;
    // check if it's any other dependent map
    char *p = sExpr.strrchrQuoted('(');
    if (0 == p) {
      // check if it's a potential attribute map
      p = sExpr.strrchrQuoted('.');
      // Bas Retsios, 8 February 2001: *p is not necessarily an extension's dot
      char *q = sExpr.strrchrQuoted('\\');
      // *p is definitely not an extension's dot if p<q
      if (p<q)
      	p = 0;
      // Now we're (only) sure that if p!=0 it points to the last dot in the filename
      if (p && (0 == _strcmpi(p, ".mpr"))) {
        *p = 0;
        sExpr = sExpression;
        p = 0;
      }
      if (p == 0) {  // no case 2
        // check case 4   'maplist:bandnr'
        p = sExpr.strrchrQuoted(':');
        if ((p != 0) && isdigit(*(p+1))) {
					String sPostFix = p;
          *p = 0;
          FileName fnMpl(sExpr, ".mpl", true);
          fnMpl.sSectionPostFix = sPostFix;
          long iBandNr = String(p+1).iVal();
          MutexFileName mut(fnMpl);
          return new MapPtr(fnMpl, false, iBandNr);
        } 
        // check if sExpression is an existing map on disk
        FileName fnMap(sExpr, ".mpr", true);
        if (fnMap.fValid()) { // case 1: map name
          if (File::fExist(fnMap)) {
            MutexFileName mut(fnMap);
            MapPtr* p = BaseMap::pGetRasMap(fnMap);
            if (p) // if already open return it
              return p;
            return MapPtr::create(fnMap);
          } 
          NotFoundError(fnMap);
          //throw ErrorNotFound(fnMap);
        }
        else  // case 3: map virtual
          fVirtual = true;
      }
      else  // case 2: attribute map
        fVirtual = true;
    }  
  }
  else if (!fVirtual && File::fExist(fn)) {
    // see if map is already open (if it is in the list of open base maps)
    MutexFileName mut(fn);
    MapPtr* p = BaseMap::pGetRasMap(fn);
    if (p) // if already open then can't create it
      NotCreatedError(fn);
  }
  bool fPrevExist = File::fExist(fn);
  if (fPrevExist) {
    MapStore::UnStore(fn); // delete previous calculated result
    ObjectInfo::WriteElement("ObjectDependency", (char*)0, fn, (char*)0);
    ObjectInfo::WriteElement("MapVirtual", (char*)0, fn, (char*)0);
    _unlink(fn.sFullName(true).c_str()); // delete previous object def file
    TableHistogram::Erase(fn); // delete previous histogram
  }  

  String sFunc = IlwisObjectPtr::sParseFunc(sExpression).toLower();
  if (sFunc == "map") {
    Array<String> as;
    short iParms = IlwisObjectPtr::iParseParm(sExpression, as);
    if (iParms == shUNDEF) {
      String sErr(TR("Incorrect parameter %i").c_str(), as.iSize());
      ErrorObject(sErr.c_str()).Show();
      return 0;
    }
    FileName fnMap(fn, ".mpr", true);
    if (!fImportMap(fnMap, as))
      return 0;
    return new MapPtr(fnMap);
  }
  MapPtr* p = new MapPtr(fn, true);
  
  {  /* The next three lines are needed because in MapVirtual::create the object being created 
        already could be opened (f.e. application in MapComputedElsewhere like by the definition 
        of MapKrigingError in the  construction of MapKriging) */
    Map mp; 
    mp.SetPointer(p);
    p->iRef++; // prevent deletion of p at end of block
    bool fErase = p->fErase;
    p->fErase = true; // if an error occurs ODF will not be written
		try {
			p->pmv = MapVirtual::create(fn, *p, sExpression);
		}
		catch (...)
		{
      p->iRef--; // see p.iRef++
      mp.SetPointer(0); // p has iRef == 1 and will be deleted
			throw;
		}
    if (0 == p->pmv) { // clean up
      p->iRef--; // see p.iRef++
      mp.SetPointer(0); // p has iRef == 1 and will be deleted
      return 0;
    }
    p->fErase = fErase;
  }
  p->iRef--; // see p.iRef++ in previous block

  p->SetDomainChangeable(p->pmv->fDomainChangeable());
  p->SetValueRangeChangeable(p->pmv->fValueRangeChangeable());
  p->SetGeoRefChangeable(p->pmv->fGeoRefChangeable());
  p->SetExpressionChangeable(p->pmv->fExpressionChangeable());
  return p;
}

MapPtr::MapPtr(const FileName& fn, bool fCreate, long iBndNr)
: BaseMapPtr(fn, fCreate), 
  pms(0), pmv(0), _mf(mfLine), im(imNEARNEIGHB), iBandNr(iBndNr)
{
	if ((iBandNr == iUNDEF) && (fnObj.sSectionPostFix.length() > 0)) 
		// skip colon and only use the number
		iBandNr = String (1+const_cast<char*>(fnObj.sSectionPostFix.c_str())).iVal();
	if (fCreate)
		return;
	GeoRef gr;
	ReadElement("Map", "GeoRef", gr);
	if (!gr.fValid())
		gr = GeoRef(RowCol());
	if (!gr->fGeoRefNone())
	{
		cbOuter = gr->cb();   // always ! - Wim 26/7/96
		
		// Silently replace the CSY in the map with that of the GeoRef if they differ.
		// In that case this is also stored in the ODF without changing the ILWIS date of the ODF
		CoordSystem cs;
		bool *fDoNotShowError = (bool *)(getEngine()->pGetThreadLocalVar(IlwisAppContext::tlvDONOTSHOWFINDERROR));
		bool fPrev = *fDoNotShowError;
		*fDoNotShowError = true;

		ReadElement("BaseMap", "CoordSystem", cs);  // not read in BaseMapPtr for ".mpr"
		*fDoNotShowError = fPrev;

		if (cs != gr->cs())
		{
			_csys = gr->cs();
			WriteElement("BaseMap", "CoordSystem", _csys);
			DoNotUpdate();
		}
		else
			_csys = cs;
	}
	String sInterpolMethod;
	if (ReadElement("Map", "InterpolMethod", sInterpolMethod))
	{
		if ("bilinear" == sInterpolMethod)
			im = imBILINEAR;
		else if ("bicubic" == sInterpolMethod)
			im = imBICUBIC;
	}
	if (!ReadElement("Map", "Size", _rcSize))
		_rcSize = gr->rcSize();
	_gr = gr;
	String s;  
	if (0 != ReadElement("MapStore", (char*)0, s))
	{
		try
		{
			pms = new MapStore(fn, *this); 
		}
		catch (const ErrorObject& err)
		{
			err.Show();
		}
	}
	// for downward compatibility with 2.02
	if (0 != ReadElement("MapVirtual", "Type", s))
		if (fCIStrEqual(s,"MapCalculate"))
		{
			SetDomainChangeable(true);
			SetValueRangeChangeable(true);
			SetExpressionChangeable(true);
		}
}

MapPtr::MapPtr(const FileName& fn, const LayerInfo& inf) :
	BaseMapPtr(fn, inf.grf->cs(), inf.grf->cb(), inf.dvrsMap),
  _rcSize(inf.grf->rcSize()),
	_gr(inf.grf),
  _mf(mfLine),
	im(imNEARNEIGHB),
	iBandNr(0),
  pmv(0),
	pms(0)
{
	pms = new MapStore(fn, *this, inf);
}

MapPtr::MapPtr(const FileName& fn, const GeoRef& gr, const RowCol& rcSize, const DomainValueRangeStruct& dvs, MapFormat mf, long iBndNr, bool fUseAs)
: BaseMapPtr(fn, gr->cs(), gr->cb(), dvs), _gr(gr), _mf(mf), pms(0), pmv(0), im(imNEARNEIGHB), iBandNr(iBndNr)
{
  assert(((iBndNr != iUNDEF) && fnObj.sSectionPostFix.length()>0) || (iBndNr == iUNDEF));

  sDescription = String("Map %S", fn.sFile);
  _rcSize = gr->rcSize();
  if (_rcSize == rcUNDEF)
    _rcSize = rcSize;
  assert(_rcSize != rcUNDEF);
  pms = new MapStore(fnObj, *this, gr, _rcSize, dvs, _mf, fUseAs);
  SetProximity(gr->rPixSize());
}

String MapPtr::sType() const
{
  ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
  if (0 != pmv)
    return pmv->sType();
  else if (fDependent()) { //check on map calculate
    String s;
    if (0 != ReadElement("MapVirtual", "Type", s))
      if (fCIStrEqual(s, "MapCalculate"))
        return "Map Calculate";
    return "Dependent Raster Map";
  }
  else
    return "Raster Map";
}

void MapPtr::Store()
{
  ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
  bool fDep = fDependent();
  if ((0 != pmv) && (sDescription == ""))
    sDescription = pmv->sExpression();
  BaseMapPtr::Store();
  WriteElement("BaseMap", "Type", "Map");
  WriteElement("Map", "GeoRef", gr());
  WriteElement("Map", "Size", rcSize());
  switch (im) {
    case imBILINEAR:
      WriteElement("Map", "InterpolMethod", "bilinear");
      break;
    case imBICUBIC:
      WriteElement("Map", "InterpolMethod", "bicubic");
      break;
    default:
      WriteElement("Map", "InterpolMethod", (char*)0);
      break;
  }
  if (0 != pms) {
    pms->Store();
  }
  else 
    MapStore::UnStore(fnObj);
  if (fDep)
    WriteElement("Map", "Type", "MapVirtual");
  if (0 != pmv)
    pmv->Store();
  String s;
  if (0 == ReadElement("Map", "Type", s))
    WriteElement("Map", "Type", "MapStore");
}
  
void MapPtr::SetGeoRef(const GeoRef& gr)
{
  ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
  // check on ... ???
  if (_gr.fValid() && (_gr->fnObj == gr->fnObj))
    return;
  // if georef is georef3d it should not be its own mapdtm.
  GeoRef3D* g3d = gr->pg3d();
  if (0 != g3d && g3d->mapDTM.fEqual(this))
    return;
  _gr = gr;
  Updated();
  SetCoordSystem(gr->cs());
}

void MapPtr::SetSize(RowCol rc)
{
  ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
  _rcSize = rc;
  if (0 != pms)
    pms->SetSize(rc);
}

long MapPtr::iRaw(const Coord& crd, double rPrx) const
{
  ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
  RowCol rc = gr()->rcConv(crd);
  return iRaw(rc);
}

long MapPtr::iValue(const Coord& crd, double rPrx) const
{
  ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
  if (imNEARNEIGHB == im) {
    RowCol rc = gr()->rcConv(crd);
    return iValue(rc);
  }
  else {
    double val = rValue(crd);
    return longConv(val);
  }
}

double MapPtr::rValue(const Coord& crd, double rPrx) const
{
  ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
  if (imNEARNEIGHB == im) {
    RowCol rc = gr()->rcConv(crd);
    return rValue(rc);
  }
  double rRow, rCol, rRes;
  gr()->Coord2RowCol(crd, rRow, rCol);
  switch (im) {
    case imBILINEAR:
      rRes = rValueBiLinear(rRow, rCol);
      break;
    case imBICUBIC:
      rRes = rValueBiCubic(rRow, rCol);
      break;
    default:
      return rUNDEF;
  }
  if (dvs.fRawAvailable()) {
    long iRaw = dvs.iRaw(rRes);
    rRes = dvs.rValue(iRaw);
  }
  return rRes;
}

double MapPtr::rValueBiLinear(double rRow, double rCol) const
{
  ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
  if ((rRow < 0.5) || (rRow > rcSize().Row - 0.5) ||
      (rCol < 0.5) || (rCol > rcSize().Col - 0.5))
    return rValue(RowCol(rRow,rCol));
  rRow -= 0.5;
  rCol -= 0.5;
  long iRow[4], iCol[4];
  double rWeight[4];
  iRow[1] = iRow[0] = (int)rRow;
  iRow[3] = iRow[2] = iRow[1] + 1;
  iCol[2] = iCol[0] = (int)rCol;
  iCol[3] = iCol[1] = iCol[0] + 1;
  double rDeltaRow = rRow - iRow[0];
  double rDeltaCol = rCol - iCol[0];
  rWeight[0] = (1 - rDeltaRow) * (1 - rDeltaCol);
  rWeight[1] = (1 - rDeltaRow) * rDeltaCol;
  rWeight[2] = rDeltaRow * (1 - rDeltaCol);
  rWeight[3] = rDeltaRow * rDeltaCol;
  double rTotWeight=0.0, rTotValue=0.0;
  for (int i = 0; i < 4; ++i) {
    double rVal = rValue(RowCol(iRow[i], iCol[i]));
    if (rVal != rUNDEF) {
      rTotValue +=  rVal * rWeight[i];
      rTotWeight += rWeight[i];
    }
  }
  if (rTotWeight < 0.1)
    return rUNDEF;
  else
    return rTotValue / rTotWeight;
}

double MapPtr::rValueBiCubic(double rRow, double rCol) const
{
  ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
  if ((rRow < 2) || (rRow > rcSize().Row - 2) ||
      (rCol < 2) || (rCol > rcSize().Col - 2))
    return rValueBiLinear(rRow, rCol);
  rRow -= 0.5;
  rCol -= 0.5;

  double rValRow[4];
  long iCol = (int)rCol;
  long iRow = (int)rRow;
  double rDeltaRow = rRow - iRow;
  double rDeltaCol = rCol - iCol;
  for (int i = 0; i < 4; ++i)
    rValRow[i] = rBiCubicResult(iRow-1+i, iCol, rDeltaCol);
  if (fResolveRealUndefs(rValRow))
    return rBiCubicPolynom(rValRow, rDeltaRow);
  else
    return rValueBiLinear(rRow, rCol);
}

double MapPtr::rBiCubicPolynom(double rVal[], double rDeltaPow) const
{
  ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
    double rResult = rVal[1] +
                     rDeltaPow * (( rVal[2] - rVal[1]/2 - rVal[0]/3 -rVal[3]/6) +
                     rDeltaPow * (( (rVal[2] + rVal[0])/2 - rVal[1]) +
                     rDeltaPow * (( rVal[1] - rVal[2])/2 + (rVal[3] - rVal[0])/6)));
    return rResult;
}

double MapPtr::rBiCubicResult(long iRow, long iCol, double rDCol) const
{
  ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
  double rValCol[4];
  for(int i = 0; i < 4; ++i)
    rValCol[i]= rValue(RowCol((int)iRow, (int)iCol-1+i));
  if(fResolveRealUndefs(rValCol))
    return rBiCubicPolynom(rValCol, rDCol);
  else
    return rUNDEF;
}

bool MapPtr::fResolveRealUndefs(double rVal[]) const
{
    if ( rVal[1]==rUNDEF)
        if (rVal[2]==rUNDEF) 
          return false;
        else 
          rVal[1]=rVal[2];

    if ( rVal[2] == rUNDEF ) 
      rVal[2]=rVal[1];
    if ( rVal[0] == rUNDEF ) 
      rVal[0]=rVal[1];
    if ( rVal[3] == rUNDEF ) 
      rVal[3]=rVal[2];

    return true;
}


String MapPtr::sValue(const Coord& crd, short iWidth, short iDec, double rPrx) const
{
  ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
  if (imNEARNEIGHB == im) {
    RowCol rc = gr()->rcConv(crd);
    return sValue(rc, iWidth, iDec);
  }
  else {
    double val = rValue(crd);
    return dvs.sValue(val, iWidth, iDec);
  }
}

long MapPtr::iRaw(RowCol rc) const
{
  ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
  if (0 != pms)
    return pms->iRaw(rc);
  else if (0 != pmv)
    return pmv->iComputePixelRaw(rc);
  return iUNDEF;
}

long MapPtr::iValue(RowCol rc) const
{
  ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
  if (!dvs.fValues())
    return iUNDEF;
  if (0 != pms)
    return pms->iValue(rc);
  else if (0 != pmv)
    return pmv->iComputePixelVal(rc);
  return iUNDEF;
}

double MapPtr::rValue(RowCol rc) const
{
  ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
  if (!dvs.fValues())
    return rUNDEF;
  if (0 != pms)
    return pms->rValue(rc);
  else if (0 != pmv)
    return pmv->rComputePixelVal(rc);
  return rUNDEF;
}

String MapPtr::sValue(RowCol rc, short iWidth, short iDec) const
{
  ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
  if (dvs.fRawAvailable()) {
    long raw = iRaw(rc);
    return dvs.sValueByRaw(raw, iWidth, iDec);
  }
  double val = rValue(rc);
  return dvs.sValue(val, iWidth, iDec);
}

void MapPtr::CalcMinMax()
{

  { 
    ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
    if (fnObj.sFile.length() == 0) {
      _rrMinMax = dvrs().rrMinMax();
      _rrPerc1 = dvrs().rrMinMax();
      _riMinMax = dvrs().riMinMax();
      _riPerc1 = dvrs().riMinMax();
      return;
    }
    if (dm()->pdbit() || dm()->pdbool()) {
      _rrMinMax = RangeReal(0,1);
      _riMinMax = RangeInt(0,1);
      return;
    }
  }
  if (st() <= stINT) {   // larger storetypes take too long time in sorting in Histogram
//if (dm()->pdi()) {
    Map mp;
    mp.SetPointer(const_cast<MapPtr*>(this));
    TableHistogramInfo histinf(mp);
    if (dvs.fValues()) {
      if (dvs.fRealValues()) {
        RangeReal rrmm = histinf.rrMinMax(0);
        RangeReal rrp1 = histinf.rrMinMax(1);
        ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
        _rrMinMax = rrmm;
        _rrPerc1 = rrp1;
        _riMinMax = RangeInt(longConv(_rrMinMax.rLo()),
                             longConv(_rrMinMax.rHi()));
        _riPerc1 = RangeInt(longConv(_rrPerc1.rLo()),
                            longConv(_rrPerc1.rHi()));
      }
      else {
        RangeInt rimm = histinf.riMinMax(0);
        RangeInt rip1 = histinf.riMinMax(1);
        ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
        _riMinMax = rimm;
        _riPerc1 = rip1;
        _rrMinMax = RangeReal(_riMinMax.iLo(),_riMinMax.iHi());
        _rrPerc1 = RangeReal(_riPerc1.iLo(),_riPerc1.iHi());
      }
    }
    Store();
//    his.Store();
  }  
  else {
    // calculate min max from map
// should be optimized for patches
    if (!fCalculated())
      Calc();
    Tranquilizer trq;
    trq.SetTitle(TR("Calculate minimum and maximum"));
    trq.SetText(String("%S %S", TR("Calculating minmax of map"), sNameQuoted(false, fnObj.sPath())));
    trq.HelpEnable(false);
    trq.Start();
    if (!dvs.fRealValues()) {
      long iMin = LONG_MAX;
      long iMax = -LONG_MAX;
      LongBuf buf(iCols());
      for (long i=0; i < iLines(); i++) {
        GetLineVal(i, buf);
        if (trq.fUpdate(i, iLines()))
          return;
        for (long j=0; j < iCols(); j++) {
          if (buf[j] == iUNDEF)
            continue;
          if (buf[j] < iMin)
            iMin = buf[j];
          else if (buf[j] > iMax)
            iMax = buf[j];
        }
      }
      ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
      _riMinMax = RangeInt(iMin, iMax);
      _rrMinMax = RangeReal(iMin, iMax);
    }
    else {
      double rMin = DBL_MAX;
      double rMax = -DBL_MAX;
      RealBuf buf(iCols());
      for (long i=0; i < iLines(); i++) {
        GetLineVal(i, buf);
        if (trq.fUpdate(i, iLines()))
          return;
        for (long j=0; j < iCols(); j++) {
          if (buf[j] == rUNDEF)
            continue;
          if (buf[j] < rMin)
            rMin = buf[j];
          else if (buf[j] > rMax)
            rMax = buf[j];
        }
      }
      ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
      _riMinMax = RangeInt(longConv(rMin), longConv(rMax));
      _rrMinMax = RangeReal(rMin, rMax);
    }
    trq.fUpdate(iLines(), iLines());
    trq.HelpEnable(true);
  }
  fChanged = true;
}

void MapPtr::GetLineRaw(long iLine, ByteBuf& buf, long iFrom, long iNum, int iPyrLayer ) const
{
  ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
  if (0 == iNum)
    iNum = iCols();
  if (0 != pms)
    pms->GetLineRaw(iLine, buf, iFrom, iNum, iPyrLayer);
  else if (0 != pmv)
    pmv->ComputeLineRaw(iLine, buf, iFrom, iNum);
}

void MapPtr::GetLineRaw(long iLine, IntBuf& buf, long iFrom, long iNum, int iPyrLayer ) const
{
  ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
  if (0 == iNum)
    iNum = iCols();
  if (0 != pms)
    pms->GetLineRaw(iLine, buf, iFrom, iNum, iPyrLayer);
  else if (0 != pmv)
    pmv->ComputeLineRaw(iLine, buf, iFrom, iNum);
}

void MapPtr::GetLineRaw(long iLine, LongBuf& buf, long iFrom, long iNum, int iPyrLayer) const
{
  ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
  if (0 == iNum)
    iNum = iCols();
  if (0 != pms)
    pms->GetLineRaw(iLine, buf, iFrom, iNum, iPyrLayer);
  else if (0 != pmv)
    pmv->ComputeLineRaw(iLine, buf, iFrom, iNum);
}

void MapPtr::GetLineVal(long iLine, LongBuf& buf, long iFrom, long iNum, int iPyrLayer ) const
{
  ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
  if (0 == iNum)
    iNum = iCols();

  if (0 != pms)
    pms->GetLineVal(iLine, buf, iFrom, iNum, iPyrLayer);
  else if (0 != pmv)
    pmv->ComputeLineVal(iLine, buf, iFrom, iNum);
}

void MapPtr::GetLineVal(long iLine, RealBuf& buf, long iFrom, long iNum, int iPyrLayer) const
{
  ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
  if (0 == iNum)
    iNum = iCols();

  if (0 != pms)
    pms->GetLineVal(iLine, buf, iFrom, iNum, iPyrLayer);
  else if (0 != pmv)
    pmv->ComputeLineVal(iLine, buf, iFrom, iNum);
}

void MapPtr::GetPatchRaw(RowCol rc, BytePatch& p) const 
{
  ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
  if (0 != pms)
    pms->GetPatchRaw(rc, p);
}

void MapPtr::GetPatchRaw(RowCol rc, IntPatch& p) const
{
  ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
  if (0 != pms)
    pms->GetPatchRaw(rc, p);
}

void MapPtr::GetPatchRaw(RowCol rc, LongPatch& p) const
{
  ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
  if (0 != pms)
    pms->GetPatchRaw(rc, p);
}

void MapPtr::GetPatchVal(RowCol rc, LongPatch& p) const
{
  ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
  if (0 != pms)
    pms->GetPatchVal(rc, p);
}

void MapPtr::GetPatchVal(RowCol rc, RealPatch& p) const
{
  ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
  if (0 != pms)
    pms->GetPatchVal(rc, p);
}

bool MapPtr::fPatchByte(MapPtr* mpPatch, const String& sTitle, Tranquilizer& trq)
{
  ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
  if (0 != pms)
    return pms->fPatchByte(mpPatch, sTitle, trq);
  return false;
} 

bool MapPtr::fPatchInt(MapPtr* mpPatch, const String& sTitle, Tranquilizer& trq)
{
  ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
  if (0 != pms)
    return pms->fPatchInt(mpPatch, sTitle, trq);
  return false;
} 

bool MapPtr::fPatchLong(MapPtr* mpPatch, const String& sTitle, Tranquilizer& trq)
{
  ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
  if (0 != pms)
    return pms->fPatchLong(mpPatch, sTitle, trq);
  return false;
} 

bool MapPtr::fPatchReal(MapPtr* mpPatch, const String& sTitle, Tranquilizer& trq)
{
  ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
  if (0 != pms)
    return pms->fPatchReal(mpPatch, sTitle, trq);
  return false;
} 

bool MapPtr::fUnPatchByte(MapPtr* mpPatch, const String& sTitle, Tranquilizer& trq)
{
  ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
  if (0 != pms)
    return pms->fUnPatchByte(mpPatch, sTitle, trq);
  return false;
} 

bool MapPtr::fUnPatchInt(MapPtr* mpPatch, const String& sTitle, Tranquilizer& trq)
{
  ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
  if (0 != pms)
    return pms->fUnPatchInt(mpPatch, sTitle, trq);
  return false;
} 

bool MapPtr::fUnPatchLong(MapPtr* mpPatch, const String& sTitle, Tranquilizer& trq)
{
  ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
  if (0 != pms)
    return pms->fUnPatchLong(mpPatch, sTitle, trq);
  return false;
} 

bool MapPtr::fUnPatchReal(MapPtr* mpPatch, const String& sTitle, Tranquilizer& trq)
{
  ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
  if (0 != pms)
    return pms->fUnPatchReal(mpPatch, sTitle, trq);
  return false;
} 

void MapPtr::Rename(const FileName& fnNew)
{
  IlwisObjectPtr::Rename(fnNew);
}

void MapPtr::GetDataFiles(Array<FileName>& afnDat, Array<String>* asSection, Array<String>* asEntry) const
{
  ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
  BaseMapPtr::GetDataFiles(afnDat, asSection, asEntry);
  if (0 != pms)
    pms->GetDataFiles(afnDat, asSection, asEntry);
}

void MapPtr::GetObjectDependencies(Array<FileName>& afnObjDep)
{
  ObjectDependency::Read(fnObj, afnObjDep);
}

void MapPtr::KeepOpen(bool f, bool force)
{
  ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
  BaseMapPtr::KeepOpen(f, force);
  if (0 != pms)
    pms->KeepOpen(f, force);
}

void Map::Export(const FileName& fn) 
{
  if (!ptr())
    return;
  ptr()->Export(fn);
}

void MapPtr::Export(const FileName& fn) 
{
  if (!fCalculated())
    Calc();
  if (0 != pms)
    pms->Export(fn);
}

void MapPtr::PutRaw(RowCol rc, long iRaw) 
{
  ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
  if (0 != pms)
    pms->PutRaw(rc, iRaw); 
}

void MapPtr::PutVal(RowCol rc, double rVal)  
{
  ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
  if (0 != pms)
    pms->PutVal(rc, rVal); 
}

void MapPtr::PutVal(RowCol rc, const String& sVal) 
{
  ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
  if (0 != pms)
    pms->PutVal(rc, sVal); 
}

void MapPtr::PutRaw(const Coord& crd, long iRaw) 
{
  ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
  if (0 != pms)
    PutRaw(gr()->rcConv(crd), iRaw);
}

void MapPtr::PutRaw(const Coord& crd, double rVal) 
{
  ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
  if (0 != pms)
    PutVal(gr()->rcConv(crd), rVal);
}
    
void MapPtr::PutVal(const Coord& crd, const String& sVal) 
{
  ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
  if (0 != pms)
    PutVal(gr()->rcConv(crd), sVal); 
}

void MapPtr::PutLineRaw(long iLine, const ByteBuf& buf, long iFrom, long iNum)
{
  ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
  if (0 == iNum)
    iNum = iCols();
  if (0 != pms)
    pms->PutLineRaw(iLine, buf, iFrom, iNum);
}

void MapPtr::PutLineRaw(long iLine, const IntBuf& buf, long iFrom, long iNum)
{
  ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
  if (0 == iNum)
    iNum = iCols();
  if (0 != pms)
    pms->PutLineRaw(iLine, buf, iFrom, iNum);
}

void MapPtr::PutLineRaw(long iLine, const LongBuf& buf, long iFrom, long iNum)
{
  ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
  if (0 == iNum)
    iNum = iCols();
  if (0 != pms)
    pms->PutLineRaw(iLine, buf, iFrom, iNum);
}

void MapPtr::PutLineVal(long iLine, const LongBuf& buf, long iFrom, long iNum)
{
  ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
  if (0 == iNum)
    iNum = iCols();
  if (0 != pms)
    pms->PutLineVal(iLine, buf, iFrom, iNum); 
}

void MapPtr::PutLineVal(long iLine, const RealBuf& buf, long iFrom, long iNum)
{
  ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
  if (0 == iNum)
    iNum = iCols();
  if (0 != pms)
    pms->PutLineVal(iLine, buf, iFrom, iNum); 
}

void MapPtr::PutPatchRaw(RowCol rc, const BytePatch& pt)
{
  ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
  if (0 != pms)
    pms->PutPatchRaw(rc, pt); 
}

void MapPtr::PutPatchRaw(RowCol rc, const IntPatch& pt)
{
  ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
  if (0 != pms)
    pms->PutPatchRaw(rc, pt); 
}

void MapPtr::PutPatchRaw(RowCol rc, const LongPatch& pt)
{
  ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
  if (0 != pms)
    pms->PutPatchRaw(rc, pt);
}

void MapPtr::PutPatchVal(RowCol rc, const LongPatch& pt)
{
  ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
  if (0 != pms)
    pms->PutPatchVal(rc, pt); 
}

void MapPtr::PutPatchVal(RowCol rc, const RealPatch& pt)
{
  ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
  if (0 != pms)
    pms->PutPatchVal(rc, pt); 
}

bool MapPtr::fConvertTo(const DomainValueRangeStruct& dvs, const Column& col)
{
  ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
  if (col.fValid() && col->dvrs() == dvs)
    return false;
  else if (dvrs() == dvs)
    return false;
  bool f = false;
  if (0 != pms)
    f = pms->fConvertTo(dvs, col);
  if (col.fValid())
    SetDomainValueRangeStruct(col->dvrs());
  else
    SetDomainValueRangeStruct(dvs);
  return f;
}

bool MapPtr::fDependent() const
{
  ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
  if (0 != pmv)
    return true;
  String s;
  ReadElement("Map", "Type", s);
  return fCIStrEqual(s , "MapVirtual");
}

String MapPtr::sExpression() const
{
  ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
  if (0 != pmv)
    return pmv->sExpression();
  String s;
  s = IlwisObjectPtr::sExpression();
  if (s.length() == 0) // for downward compatibility with 2.02
    ReadElement("MapVirtual", "Expression", s);
  return s;
}

bool MapPtr::fCalculated() const
// returns true if a calculated result exists
{
  if (!fDependent())
    return IlwisObjectPtr::fCalculated();
  ILWISSingleLock sl(const_cast<CCriticalSection*>(&csCalc), TRUE);
  return 0 != pms;
} 

bool MapPtr::fDefOnlyPossible() const
// returns true if data can be retrieved without complete calculation (on the fly)
{
  ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
  if (!fDependent())
    return IlwisObjectPtr::fDefOnlyPossible();
  if (pmv)
    return pmv->fDefOnlyPossible();
  bool f;
  if (0 != ReadElement("MapVirtual", "DefOnlyPossible", f))
    return f;
  return false;
}

void MapPtr::Calc(bool fMakeUpToDate)
// calculates the result     
{
  ILWISSingleLock sl(&csCalc, TRUE, SOURCE_LOCATION);
  OpenMapVirtual();
  if (fMakeUpToDate)
    if (!objdep.fUpdateAll())
      return;
  if (0 != pmv) 
    pmv->Freeze();
}


void MapPtr::DeleteCalc()
// deletes calculated  result     
{
  TableHistogram::Erase(fnObj);
  if (0 == pms)
    return;
  ILWISSingleLock sl(&csCalc, TRUE, SOURCE_LOCATION);
  OpenMapVirtual();
  if (0 != pmv)
    pmv->UnFreeze();
}

void MapPtr::OpenMapVirtual()
{
  if (0 != pmv) // already there
    return;
  if (!fDependent())
    return;
  try {
    pmv = MapVirtual::create(fnObj, *this);
    objdep = ObjectDependency(fnObj);
    String s;
    if (0 == ReadElement("IlwisObjectVirtual", (char*)0, s)) {
      // for downward compatibility with 2.02 :
      SetDomainChangeable(pmv->fDomainChangeable());
      SetValueRangeChangeable(pmv->fValueRangeChangeable());
      SetGeoRefChangeable(pmv->fGeoRefChangeable());
      SetExpressionChangeable(pmv->fExpressionChangeable());
    }
  }
  catch (const ErrorObject& err) {
    err.Show();
    pmv = 0;
    objdep = ObjectDependency();
  }
}

void MapPtr::Replace(const String& sExpr)
{
  ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
  MapVirtual* pmvx = MapVirtual::create(fnObj, *this, sExpr);
  if (0 == pmvx)
    return;
  if (0 != pmv)
    delete pmv;
  pmv = pmvx;
  Updated();
  pmv->UnFreeze();
}

void MapPtr::FillWithUndef()
{
  ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
  if (0 != pms)
    pms->FillWithUndef();
}

static void CyclicDefError(const FileName& fn)
{
  throw ErrorObject(WhatError(TR("Cyclic definition"), errEXPRESSION), fn);
}

static void FileExistsError(const FileName& fn)
{
  throw ErrorObject(WhatError(TR("Output Object Name already in use"), errEXPRESSION), fn);
}

void MapPtr::InitName(const FileName& fn)
{
  ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
  if (pmv)
    if (pmv->fMapInObjDep(fn))
      CyclicDefError(fn);
  if (fn.fExist())
    FileExistsError(fn);
	SetFileName(fn);
  MapStore::UnStore(fn); // delete previous calculated result
  _unlink(fn.sFullName(true).c_str()); // delete previous object def file
  TableHistogram::Erase(fn); // delete previous histogram
  if (0 != pmv)
    pmv->InitName(fn);
}

void MapPtr::CreateMapStore()
{
  ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
  if (0 != pmv)
    pmv->CreateMapStore();
}

DomainValueRangeStruct MapPtr::dvrsDefault(const String& sExpression) const
{
  ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
  const_cast<MapPtr *>(this)->OpenMapVirtual();
  if (0 == pmv)
    return DomainValueRangeStruct();
  return pmv->dvrsDefault(sExpression);
}

bool MapPtr::fMergeDomainForCalc(Domain& dm, const String& sExpr)
{
  OpenMapVirtual();
  if (0 == pmv)
    return true;
  return pmv->fMergeDomainForCalc(dm, sExpr);
}

void MapPtr::SetValueRange(const ValueRange& vr)
{
  BaseMapPtr::SetValueRange(vr);
  if (fChanged && pmv) {
    delete pms;
    pms = 0;
  }
}

String MapPtr::sName(bool fExt, const String& sDirRelative) const
{
	if (!fnObj.fValid())
		return sExpression();
	
	String s = fnObj.sRelative(fExt, sDirRelative);
	if (iBandNr != iUNDEF)
	{
		MapList ml(fnObj);
		String sPrefix = ml->sBandPreFix();
		s &= String(":%S %li", sPrefix, iBandNr);
	}
	return s;
}

String MapPtr::sNameQuoted(bool fExt, const String& sDirRelative) const
{
  if (!fnObj.fValid())
    return sExpression();
  String s = fnObj.sRelativeQuoted(fExt, sDirRelative);

  return s;
}

bool MapPtr::fMapInMapList() const
{
  return iBandNr != iUNDEF;
}

void MapPtr::GetObjectStructure(ObjectStructure& os)
{
	BaseMapPtr::GetObjectStructure(os);
	if (!fUseAs() || (os.caGetCommandAction() != ObjectStructure::caDELETE))
		os.AddFile(fnObj, "MapStore", "Data");
	os.AddFile(fnObj, "MapStore", "PyramidFile");
	if ( os.fGetAssociatedFiles() )
	{
		os.AddFile(fnObj, "Map", "GeoRef", ".grf");
	}		
	FileName fnHist(fnObj, ".his");
	if (File::fExist(fnHist) && IlwisObject::iotObjectType( fnHist ) != IlwisObject::iotANY)
	{
		IlwisObject obj = IlwisObject::obj(fnHist);
		if ( obj.fValid())
			obj->GetObjectStructure(os);
	}					
}

void MapPtr::DoNotUpdate()
{
	BaseMapPtr::DoNotUpdate();

}

IlwisObjectVirtual *MapPtr::pGetVirtualObject() const
{
	return pmv;
}

void MapPtr::StoreAs(const String& sExpression)
{
	if ( pms)
		pms->StoreAs(sExpression);
}

void MapPtr::CreatePyramidLayer()
{
	if ( pms )
		pms->CreatePyramidLayers();
}

bool MapPtr::fHasPyramidFile() const
{
	if ( pms )
		return pms->fHasPyramidFile();

	return false;
}

void MapPtr::DeletePyramidFile()
{
	if ( pms)
		pms->DeletePyramidFile();
}

int MapPtr::iNoOfPyramidLayers()
{
	if ( pms )
		return pms->iNoOfPyramidLayers();

	return 0;
}

vector<String> MapPtr::vsValue(const Coord& crd, short iWidth, short iDec, double rPrx) const
{
	vector<String> values;
	values.push_back(sValue(crd, iWidth, iDec, rPrx));

	return values;
}

RangeReal MapPtr::rrMinMaxSampled() {
	RangeReal rr;
	RealBuf buf(rcSize().Col);
	GetLineVal(0,buf);
	rr += buf[0];
	rr += buf[rcSize().Col - 1];
	rr += buf[rcSize().Col / 2];
	GetLineVal(rcSize().Row / 2,buf);
	rr += buf[0];
	rr += buf[rcSize().Col - 1];
	rr += buf[rcSize().Col / 2];
	GetLineVal(rcSize().Row - 1, buf);
	rr += buf[0];
	rr += buf[rcSize().Col - 1];
	rr += buf[rcSize().Col / 2];
	for(int i = 1; i < 7; ++i) {
		int line = i * rcSize().Col / 7;
		GetLineVal(i,buf);
		for(int j = 1; j < 7; ++j) {
			int col = min(buf.iSize()-1, j * rcSize().Row / 7);
			rr += buf[col];
		}
	}
	return rr;

}