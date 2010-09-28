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
//$Log: /ILWIS 3.0/BasicDataStructures/maplist.cpp $
 * 
 * 60    13-12-05 11:25 Willem
 * Added a check to handle corrupted maplist ODF (with no key for the
 * number of maps)
 * 
 * 59    20-04-04 14:05 Willem
 * [Bug=6503, 6525] SetGeoRef now also sets the rcSize in the ODF. This
 * solves a problem with dependent maplists
 * 
 * 58    29-01-03 10:20 Willem
 * - Changed: GetObjectStructure will not collect its maps in case of a
 * delete command, when the data files is a UseAs datafile. This prevents
 * unintended removal of UseAs datafiles
 * - Removed: some obsolete commented code
 * 
 * 57    1/14/02 15:12 Willem
 * The GetObjectStructure used a wrong check to add the contents of the
 * container to the selection. This resulted in always copying/deleting
 * the contents. Corrected now.
 * 
 * 56    11/22/01 15:37 Willem
 * When the size of the maplist was needed sometimes the size of the array
 * in which the maps were stored was queried instead of using the correct
 * MapList::iSize() function.
 * 
 * 55    11/20/01 15:12 Willem
 * - GetObjectStructure did not handle a non-zero offset correctly, so not
 * all bands from the maplist were added to the list
 * 
 * 54    12-11-01 14:37 Koolhoven
 * Updated() throws away additonal info because this contains the now
 * incorrect statistical info
 * 
 * 53    21-09-01 15:25 Koolhoven
 * mtVarCov() and mtCorr() now check properly on dates of input maps
 * 
 * 52    8/23/01 17:14 Willem
 * - Removed the SetReadOnly() function. This is now handled by
 * IlwisObjectPtr::SetReadOnly() for all ilwis objects
 * - The GetObjectStructure() function always adds all contained maps
 * except when COPY or DELETE is choosen, in which case the caller
 * determines it.
 * 
 * 51    9-08-01 12:42 Koolhoven
 * member maps are now registered to be a part of a collection
 * 
 * 50    8-08-01 10:21 Koolhoven
 * SetDomainValueRangeStruct() on MapList now sets the member dvrsMaps in
 * MapListVirtual which is used during the freezing operation
 * 
 * 49    7-08-01 12:43 Koolhoven
 * removed domain member from MapList, because it served no purpose
 * 
 * 48    7-08-01 11:25 Koolhoven
 * added ObjectDependency info
 * 
 * 47    6-08-01 15:24 Koolhoven
 * added fCalculated()
 * 
 * 46    6-08-01 15:17 Koolhoven
 * added option of MapListVirtual
 * 
 * 45    6/15/01 11:50a Martin
 * changed the name of fHasPyramidFile to ...Files
 * 
 * 44    6/15/01 11:20a Martin
 * usage of pyramid files for maplists
 * 
 * 43    4/09/01 10:26 Willem
 * Create function now also accepts a valid MapList filename in the
 * expression.
 * 
 * 42    2/23/01 13:06 Retsios
 * Solves bug # 4110: "No long file name in MapColorComp":
 * MapListPtr::sName now returns quoted object names where needed.
 * 
 * 41    23/01/01 17:22 Willem
 * AddMap() function now throws an error in case map cannot be added to
 * the MapList
 * 
 * 40    1/12/00 15:31 Willem
 * Added SetReadyOnly function: the maplist will now also set the R/O flag
 * for the contained maps
 * 
 * 39    23-11-00 12:03p Martin
 * default parm in getobjectstructure was not correct
 * 
 * 38    20-11-00 9:18a Martin
 * implemented fUsesDependent objects. Checks if the list contains
 * dependent objects
 * 
 * 37    14/11/00 17:22 Willem
 * The import general raster generates a string with full quoting, that is
 * the extension is included. Therefore the string unquote is needed here.
 * 
 * 36    14/11/00 14:25 Willem
 * The description from the import general raster is now also passed to
 * the import function
 * 
 * 35    27-10-00 4:20p Martin
 * getobjectstructure extended for the use of internal maplist
 * 
 * 34    26-10-00 8:21a Martin
 * changed getobjectstructure function
 * 
 * 33    18/09/00 12:16 Willem
 * - Maplist now stores UseAs flag if appropriate
 * - Removed object dependencies for the maps in the map list
 * - Change the section name in the GetObjectStructure() function
 * 
 * 32    15/09/00 10:54 Willem
 * Added functions:
 * - fnRealName() to retrieve the filename of an internal band in the
 * maplist as needed by Ilwis. The input can be both the realname or the
 * displayname
 * - fnDisplayName() to get the displayname calculated from the real name
 * 
 * 31    14/09/00 11:55 Willem
 * The maplistPtr functions now do not make assumptions about the position
 * of the first map in the internal list. They now all use the iLower()
 * and iUpper() functions
 * 
 * 30    11-09-00 10:06a Martin
 * changed the structure of ObjectStructure object to include odf entry
 * information
 * 
 * 29    8-09-00 3:21p Martin
 * added function to set the fChanged member of all the 'members of an
 * object.
 * added function to retrieve the 'structure' of an object (filenames)
 * 
 * 28    17-07-00 8:06a Martin
 * changed sCurDir to sGetCurDir from IlwsWinApp
 * 
 * 27    27-04-00 10:44a Martin
 * many changes to make sorting in the maplist catalog possible. A number
 * of functions have become virtual (sort, SaveSettings and LoadSettings)
 * 
 * 26    4/25/00 3:43p Hendrikse
 * computing  _mtVarCov(i, j) did the last division as integer division
 * before casting it to double; this is now corrected, als for input maps
 * with longs as values in their domain
 * 
 * 25    25-04-00 12:06p Martin
 * added add and remove function to maplist
 * 
 * 24    9-02-00 10:54 Wind
 * replaced \n with \r\n in additional info
 * 
 * 23    8-02-00 14:11 Wind
 * import error
 * 
 * 22    23-12-99 9:39 Wind
 * maplist import bug
 * 
 * 21    13-12-99 12:35 Wind
 * adapted to zero based arrays
 * 
 * 20    8-12-99 9:50 Wind
 * rasterfilestructure was not properly set from parameters
 * 
 * 19    24-11-99 17:15 Wind
 * removed thread for import of data
 * 
 * 18    18-11-99 12:07 Wind
 * import general raster bugs
 * 
 * 17    18-11-99 10:24 Wind
 * added error messages for import og maplist
 * 
 * 16    29-10-99 12:58 Wind
 * case sensitive stuff
 * 
 * 15    25-10-99 13:14 Wind
 * making thread save (2); not yet finished
 * 
 * 14    22-10-99 12:56 Wind
 * thread save access (not yet finished)
 * 
 * 13    10/07/99 1:19p Wind
 * set the path in import thread to current dir
 * 
 * 12    10/07/99 12:42p Wind
 * 
 * 11    10/07/99 12:23p Wind
 * debugged import general raster in thread
 * 
 * 10    10/07/99 12:04p Wind
 * put general raster import in it's own thread
 * 
 * 9     10/06/99 5:16p Wind
 * allow import of general raster maplists
 * 
 * 8     10/05/99 1:28p Wind
 * busy with import of maplist
 * 
 * 7     1-10-99 17:03 Koolhoven
 * Changed implementation of sDisplayName
 * 
 * 6     10/01/99 1:37p Wind
 * support for map syntax maplist:bandnr
 * 
 * 5     9/29/99 5:16p Wind
 * 
 * 4     9/29/99 3:09p Wind
 * added a band prefix for bands in a maplist
 * 
 * 3     9/29/99 11:30a Wind
 * comments
 * 
 * 2     9/29/99 11:15a Wind
 * added support for maplists with raster maps without mpr files
	Last change:  J    21 Oct 99   10:03 am
*/
//Revision 1.6  1998/09/16 17:22:46  Wim
//22beta2
//
//Revision 1.4  1998-01-27 12:45:43+01  Wim
//Bug 17: MapListPtr::CalculateStats() started on iLower()+1
//
//Revision 1.3  1997-08-25 16:40:32+01  janh
//implemented use of Checkdomains
//
//Revision 1.2  1997/08/22 16:45:29  janh
//void CheckDomains() added
//
/* MapList for ILWIS 2.0
// october 1994, by Wim Koolhoven
// (c) Computer Department ITC
	Last change:  WK    7 Jul 98   11:43 am
*/
#include "Engine\Map\Raster\MapList\maplist.h"
#include "Engine\Applications\MAPVIRT.H"
#include "Engine\Domain\dm.h"
#include "Engine\Domain\Dmvalue.h"
#include "Engine\Domain\dmclass.h"
#include "Engine\Base\DataObjects\ObjectStructure.h"
#include "Engine\DataExchange\CONV.H"
#include "Engine\Base\Algorithm\Qsort.h"
#include "Engine\Base\System\mutex.h"
#include "Engine\Base\DataObjects\ObjectStructure.h"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Hs\maplist.hs"
#include "Headers\Hs\CONV.hs"
#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\Engine.h"
#include "Engine\Applications\MapListVirtual.h"
#include <math.h>

IlwisObjectPtrList MapList::listMapList;

MapList::MapList()
: IlwisObject(listMapList)
{}

MapList::MapList(const MapList& mp)
: IlwisObject(listMapList, mp.pointer())
{}

MapList::MapList(const FileName& fn)
: IlwisObject(listMapList)
{
	FileName fnMpl(fn, ".mpl");
	MutexFileName mut(fnMpl);
	MapListPtr* p = static_cast<MapListPtr*>(MapList::pGet(fnMpl));
	if (p) // if already open return it
		SetPointer(p);
	else
		SetPointer(MapListPtr::create(fnMpl));
}

MapList::MapList(const FileName& fn, const String& sExpression)
: IlwisObject(listMapList, MapListPtr::create(FileName(fn, ".mpl"),sExpression))
{
}

MapList::MapList(const String& sExpression)
: IlwisObject(listMapList, MapListPtr::create(FileName(), sExpression))
{
}

MapList::MapList(const String& sExpression, const String& sPath)
: IlwisObject(listMapList, MapListPtr::create(FileName::fnPathOnly(sPath), sExpression))
{
}

MapList::MapList(const FileName& fn,
                 const Array<FileName>& fnaMaps)
: IlwisObject(listMapList, new MapListPtr(fn, fnaMaps))
{
}

MapList::MapList(const FileName& fn,
                 const DomainValueRangeStruct& dvsMap, const GeoRef& gr, 
                 const RowCol& rc, long iNrBands, const String& sBandPreFix)
: IlwisObject(listMapList, new MapListPtr(fn, dvsMap, gr, rc, iNrBands, sBandPreFix))
{
}

MapList::MapList(IlwisObjectPtr* ptr)
: IlwisObject(listMapList, ptr)
{}

MapListPtr* MapList::pGet(const FileName& fn)
{
	return static_cast<MapListPtr*>(listMapList.pGet(fn));
}

MapListPtr* MapListPtr::create(const FileName& fn)
{
	if (!File::fExist(fn))
		NotFoundError(fn);
	// see if maplist is already open (if it is in the list of open maplists)
	MutexFileName mut(fn);
	MapListPtr* p = MapList::pGet(fn);
	if (0 != p) // if already open return it
		return p;  
	return new MapListPtr(fn);
}

bool fImportMapList(const FileName& fnMpl, const Array<String>& as)
{
	String sErr;
	// parms: (for format == genras
	// input, format, conversion type [UseAs|Convert], NrOfColumns, NrOfBands,iHeaderSize,
	// FileStructure [BIL|BSQ|BIP],  
	// StoreType [Bit|Byte|Int|Real], [1 Byte|2 Byte|4 Byte|8 Byte], 
	// [SwapBytes], [CreateMPR|NoMPR], [Description]
	try {
		int iParms = as.iSize();
		if (iParms < 6) {
			// too little parms 
			sErr = String(SCVErrMapTooLittleParms_i.scVal(), 6);
			throw ErrorObject(sErr);
		}
		
		// The import general raster generates a string with full quoting, that is
		// the extension is included. Therefore the string unquote is needed here.
		String sInput = as[0].sUnQuote();
		FileName fnInp(sInput);
		String sFormat = as[1];
		if (fCIStrEqual(sFormat, "genras")) {
			// check fixed parms
			String sConversionType = as[2];
			bool fUseAs;
			if (fCIStrEqual(sConversionType, "useas")) 
				fUseAs = true;
			else if (fCIStrEqual(sConversionType, "convert")) 
				fUseAs =  false;
			else {
				// invalid conversion type
				sErr = String(SCVErrMapConvType_S.scVal(), as[2]);
				throw ErrorObject(sErr);
			}
			long iCols = as[3].iVal();
			if (iCols <= 0) {
				// invalid nr of columns
				sErr = String(SCVErrMapColumns_S.scVal(), as[3]);
				throw ErrorObject(sErr);
			}
			long iBands = as[4].iVal();
			if (iBands <= 0) {
				// invalid nr of bands
				sErr = String(SCVErrMapHeaderSize_S.scVal(), as[4]);
				throw ErrorObject(sErr);
			}
			// check non fixed parms
			long iHeaderSize = 0;
			if ((iParms > 5) && (as[5].length() > 0)) {
				iHeaderSize = as[5].iVal();
				if (iHeaderSize < 0) {
					// invalid header size
					sErr = String(SCVErrMapHeaderSize_S.scVal(), as[5]);
					throw ErrorObject(sErr);
				}
			}
			ImportRasterFileStructure irfs = irfsBANDINTERLEAVED;
			if ((iParms > 6)  && (as[6].length() > 0)) {
				String s = as[6];
				if (fCIStrEqual(s, "bil")) 
					irfs = irfsBANDINTERLEAVED;
				else if (fCIStrEqual(s, "bsq")) 
					irfs = irfsBANDSEQUENTIAL;
				else if (fCIStrEqual(s, "bip")) 
					irfs = irfsPIXELINTERLEAVED;
				else {
					// invalid raster file structure
					sErr = String(SCVErrMapListFileStruct_S.scVal(), as[6]);
					throw ErrorObject(sErr);
				}
			}
			ImportRasterPixelStructure irps = irpsBYTE;
			int iNrBytesPerPixel = 1;
			if ((iParms > 7) && (as[7].length() > 0)){
				String s = as[7];
				if (fCIStrEqual(s, "bit")) 
					irps = irpsBIT;
				else if (fCIStrEqual(s, "byte"))
					irps = irpsBYTE;
				else if (fCIStrEqual(s, "int")) {
					irps = irpsBYTE;
					iNrBytesPerPixel = 2;
				}
				else if (fCIStrEqual(s, "real")) {
					irps = irpsFLOAT;
					iNrBytesPerPixel = 4;
				}
				else {
					// invalid raster pixel structure
					sErr = String(SCVErrMapPixStruct_S.scVal(), as[7]);
					throw ErrorObject(sErr);
				}
			}
			if ((iParms > 8) && (as[8].length() > 0)){
				int i = as[8].iVal();
				if ((i != 1) && (i != 2) && (i != 4) && (i != 8)) {
					// invalid nr of bytes
					sErr = String(SCVErrMapBytes_S.scVal(), as[8]);
					throw ErrorObject(sErr);
				}
				iNrBytesPerPixel = i;
			}
			bool fCopyData = !fUseAs;
			bool fByteSwap = false;
			if ((iParms > 9) && (as[9].length() > 0)){
				String s = as[9];
				if (fCIStrEqual(s, "swap") || fCIStrEqual(s, "swapbytes"))
					fByteSwap = true;
				else if (fCIStrEqual(s, "noswap") || fCIStrEqual(s, "noswapbytes"))
					fByteSwap = false;
				else {
					// invalid parm swap bytes
					sErr = String(SCVErrMapSwap_S.scVal(), as[9]);
					throw ErrorObject(sErr);
				}
			} 
			bool fCreateMPR = false;
			if ((iParms > 10) && (as[10].length() > 0)){
				String s = as[10];
				if (fCIStrEqual(s, "creatempr"))
					fCreateMPR = true;
				else if (fCIStrEqual(s,"nompr"))
					fCreateMPR = false;
				else {
					// invalid parm create mpr
					sErr = String(SCVErrMapListCreateMPR_S.scVal(), as[10]);
					throw ErrorObject(sErr);
				}
			}
			String sDescr;
			if ((iParms > 11) && (as[11].length() > 0))
				sDescr = as[11];
			
			ImportWithHeader(fnInp, fnMpl, iHeaderSize, iBands, iCols,
				irps, iNrBytesPerPixel, irfs, fUseAs, fByteSwap, fCreateMPR, sDescr);
			
		}
		else {
		// invalid format
		sErr = String(SCVErrMapFormat_S.scVal(), as[1]);
		throw ErrorObject(sErr);
		}
	}
	catch (const ErrorObject& err) {
		err.Show();
		return false;
	}
	return true;
}

MapListPtr* MapListPtr::create(const FileName& fn, const String& sExpression)
{
	FileName fnMpl(sExpression);

	Array<String> as;
	short iParms = IlwisObjectPtr::iParseParm(sExpression, as);
	if (iParms == shUNDEF)
	{
		// It is no valid function so now check if the expression is a maplist filename
		if (!(fnMpl.fValid() && fCIStrEqual(fnMpl.sExt, ".mpl")))
		{
			String sErr(SCVErrMapParamNr_i.scVal(), as.iSize());
			ErrorObject(sErr.scVal()).Show();
			return 0;
		}
	}
	String sFunc = IlwisObjectPtr::sParseFunc(sExpression);
	if (fCIStrEqual(sFunc, "mlist"))
	{
		if (iParms <= 0)
			throw ErrorObject(WhatError(SMPLErrEmptyMapList, errMapList+1), fn);

		Array<Map> mpArr(iParms);
		for (int i=0; i<iParms; i++)
			mpArr[i] = Map(as[i], fn.sPath());

		return new MapListPtr(fn, mpArr);
	}
	else if (fCIStrEqual(sFunc, "maplist"))
	{
		Array<String> *pas = new Array<String>;
		for (int i=0; i < as.iSize(); i++)
			(*pas) &= as[i];

		FileName fnMpl(fn, ".mpl", true);
		if (!fImportMapList(fnMpl, as))
			return 0;

		return new MapListPtr(fnMpl);
	}
	else if (fCIStrEqual(sFunc.sLeft(7), "MapList"))
		return new MapListPtr(fn, sExpression);
	else
	{
		fnMpl.Dir(fn.sPath());
		if (fnMpl.sPath().length() == 0)
			fnMpl.Dir(getEngine()->sGetCurDir());
		fnMpl.sExt = ".mpl";

		return MapListPtr::create(fnMpl);
	}
}

MapListPtr::~MapListPtr()
{
  delete pmlv;
}

MapListPtr::MapListPtr(const FileName& fn)
: IlwisObjectPtr(FileName(fn, ".mpl"))
, pmlv(0)
{
	ObjectTime tim = objtime;

	bool fUseAs;

	if (0 == ReadElement("Ilwis", "UseAs", fUseAs))
		fUseAs = false;
	SetUseAs(fUseAs);
	ReadElement("MapList", "AttributeTable", attTable);
	int iMaps = iReadElement("MapList", "Maps");
    // Corrupted ODF can have no key for the number of maps, so check
    if (iMaps == iUNDEF)
        iMaps = 0;

	iOffset = 0;
	long iOff = iReadElement("MapList", "Offset");
	if (iOff != iUNDEF)
		iOffset = iOff;
	ma.Resize(iMaps+iOffset);
	FileName fnMap;
	int i = iLower();
	for (; i <= iUpper(); i++) {
		ReadElement("MapList", String("Map%i", i).scVal(), fnMap);
		try {
			ma[i] = Map(fnMap);
		}
		catch (const ErrorObject& err) {
			err.Show();
		}
	}
	// remove not existing maps
	for (i = iLower(); i <= iUpper(); i++)
		if (!map(i).fValid()) {
			ma.Remove(i, 1);
			i--;
		}  
		try {
			CheckDomains(); // only for non-removed maps
		}
		catch (const ErrorObject& err) {
			err.Show();
		}  
		// retrieve Georef
		ReadElement("MapList", "GeoRef", _gr);
		ReadElement("MapList", "Size", _rcSize);
		if (iSize() == 0)
			return;
		try {
			CheckGeoRefs();
		}
		catch (const ErrorObject& err) {
			err.Show();
		}
		if (_rcSize == rcUNDEF)
			_rcSize = map(iLower())->rcSize();
		if (0 == ReadElement("MultiBandStat", "CalcStatTime", tmCalcStat))
			tmCalcStat = 0;
		if (tmCalcStat != 0) {
			// retrieve multi band stats
			ReadElement("MultiBandStat", "VarCov", _mtVarCov);
			ReadElement("MultiBandStat", "Correlation", _mtCorr);
			if ((_mtVarCov.iRows() != iSize()) || (_mtVarCov.iCols() != iSize()))
				tmCalcStat = 0;
			if ((_mtCorr.iRows() != iSize()) || (_mtCorr.iCols() != iSize()))
				tmCalcStat = 0;
		}
		if (tmCalcStat != 0) {
			RealMatrix matVec;
			ReadElement("MultiBandStat", "Mean", matVec);
			_vecMean = CVector(iSize());
			if (matVec.iRows() != iSize())
				tmCalcStat = 0;
			else {
				for (i=0; i < matVec.iRows(); i++) 
					_vecMean(i) = matVec(i, 0);
				_vecStd = CVector(iSize());
				ReadElement("MultiBandStat", "StandardDev", matVec);
				if (matVec.iRows() != iSize())
					tmCalcStat = 0;
				else
					for (i=0; i < matVec.iRows(); i++) 
						_vecStd(i) = matVec(i, 0);
			}  
		}  
		ReadElement("MapList", "BandPreFix", _sBandPreFix);
		fChanged = false;
		objtime = tim;
}

MapListPtr::MapListPtr(const FileName& fn, const Array<FileName>& fnaMaps)
: IlwisObjectPtr(FileName(fn, ".mpl"),true, ".mpl")
, pmlv(0)
{
	if (fnaMaps.iSize() == 0)
		throw ErrorObject(WhatError(SMPLErrEmptyMapList, errMapList+1), fn);
	// retrieve maplist
	iOffset = 0; //fnaMaps.iLower();
	ma.Resize(fnaMaps.iSize()+iOffset);
	FileName fnMap;
	for (int i  = iLower(); i <= iUpper(); i++) {
		ma[i] = Map(fnaMaps[i]);
		ObjectInfo::WriteAdditionOfFileToCollection(fnaMaps[i], fnObj);
	}
	CheckDomains();
	
	// get georef from first map
	_gr = map(iLower())->gr();
	if (gr()->fGeoRefNone())
		_rcSize =  map(iLower())->rcSize();
	else
		_rcSize =  gr()->rcSize();
	CheckGeoRefs();
	if (fn.sFile.length() == 0) {
		objtime = map(iLower())->objtime;
		for (int i  = iLower()+1; i <= iUpper(); i++)
			objtime = min((long)objtime, (long)map(i)->objtime);
	}
	tmCalcStat = 0;
}

MapListPtr::MapListPtr(const FileName& fn, const Array<Map>& aMaps)
: IlwisObjectPtr(FileName(fn, ".mpl"),true, ".mpl")
, pmlv(0)
{
	if (aMaps.iSize() == 0)
		throw ErrorObject(WhatError(SMPLErrEmptyMapList, errMapList+1), fn);
	// retrieve maplist
	iOffset = 0; // aMaps.iLower();
	ma.Resize(aMaps.iSize()+iOffset);
	FileName fnMap;
	for (int i  = iLower(); i <= iUpper(); i++) {
		ma[i] = aMaps[i];
		ObjectInfo::WriteAdditionOfFileToCollection(map(i)->fnObj, fnObj);
	}
	CheckDomains();
	
	// get georef from first map
	_gr = map(iLower())->gr();
	if (gr()->fGeoRefNone())
		_rcSize =  map(iLower())->rcSize();
	else
		_rcSize =  gr()->rcSize();
	CheckGeoRefs();
	if (fn.sFile.length() == 0) {
		objtime = map(iLower())->objtime;
		for (int i  = iLower()+1; i <= iUpper(); i++)
			objtime = min((long)objtime, (long)map(i)->objtime);
	}
	tmCalcStat = 0;
}

MapListPtr::MapListPtr(const FileName& fn,
	const DomainValueRangeStruct& dvsMap, const GeoRef& grMap, const RowCol& rc, 
	long iNrBands, const String& sBandPref)
: IlwisObjectPtr(FileName(fn, ".mpl"),true, ".mpl")
, pmlv(0)
{
	_sBandPreFix = sBandPref;
	iOffset = 1;
	ma.Resize(iNrBands+iOffset);
	for (long i = iLower(); i <= iUpper(); i++) {
		ma[i] = Map(fnObj, grMap, rc, dvsMap, mfLine, i);
	}
	SetGeoRef(grMap);
	tmCalcStat = 0;
	if (gr()->fGeoRefNone())
		_rcSize =  map(iLower())->rcSize();
	else
		_rcSize =  gr()->rcSize();
	Updated();
}

MapListPtr::MapListPtr(const FileName& fn, const String& sExpr)
: IlwisObjectPtr(FileName(fn, ".mpl"), true, ".mpl")
, pmlv(0)
{
	pmlv = MapListVirtual::create(fnObj, *this, sExpr);
}

String MapListPtr::sType() const
{
	if (0 != pmlv)
		return pmlv->sType();
	else if (fDependent()) 
		return "Dependent Map List";
	else
		return "Map List";
}

void MapListPtr::Store()
{
	ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
	if ((0 != pmlv) && (sDescription == ""))
		sDescription = pmlv->sExpression();
	IlwisObjectPtr::Store();
	WriteElement("Ilwis", "Type", "MapList");
	WriteElement("MapList", "GeoRef", gr());
	WriteElement("MapList", "AttributeTable", attTable);
	WriteElement("MapList", "Size", rcSize());
	WriteElement("MapList", "Maps", iSize());
	WriteElement("MapList", "BandPreFix", _sBandPreFix);
	WriteElement("MapList", "Offset", iLower());
	int i;
	for (i  = iLower(); i <= iUpper(); i++)
		WriteElement("MapList", String("Map%i", i).scVal(), map(i)->fnObj);
	WriteElement("MultiBandStat", "VarCov", _mtVarCov);
	WriteElement("MultiBandStat", "CalcStatTime", tmCalcStat);
	WriteElement("MultiBandStat", "Correlation", _mtCorr);
	WriteElement("MultiBandStat", "Mean", _vecMean);
	WriteElement("MultiBandStat", "StandardDev", _vecStd);
	if (pmlv)
		pmlv->Store();
}

void MapListPtr::Updated()
{
	IlwisObjectPtr::Updated();
	tmCalcStat = 0;
	SetAdditionalInfoFlag(false); // to prevent that statistical info is still here
}

void MapListPtr::SetGeoRef(const GeoRef& grf) // set georef of all maps, dangerous function!
{
	for (int i = iLower(); i <= iUpper(); i++)
		ma.ind(i)->SetGeoRef(grf);
	_gr = grf;
	_rcSize = grf->rcSize();
	fChanged = true;
}

String MapListPtr::sName(bool fExt, const String& sDirRelative) const
{
	String s = IlwisObjectPtr::sName(fExt, sDirRelative);
	if (s.length() != 0)
		return s;
	s = "mlist(";
	for (int i = iLower(); i <= iUpper(); i++) {
		if (ma[i]->fnObj.fValid() && (ma[i]->fnObj.sPath() == sDirRelative))
			s &= ma[i]->fnObj.sRelativeQuoted(false);
		else
			s &= ma[i]->sName(false, sDirRelative);
		if (i < iUpper())
			s &= ',';
	}
	s &= ')';
	return s;
}


void MapListPtr::CheckDomains() const
{ 
	if (iSize() == 0)
		return;
	Domain dom0 = ma[iLower()]->dm();
	if (dom0 == Domain("none"))
		throw ErrorObject(WhatError(SMPLErrDomainNoneNotAllowed, errMapList+2), fnObj.sPath());
	for (int i = iLower()+1; i <= iUpper(); i++)
		if (dom0 != ma[i]->dm())
			IncompatibleDomainsError(dom0->sName(true, fnObj.sPath()),
			ma[i]->dm()->sName(true, fnObj.sPath()), sTypeName(), errMapList+3);
}

void MapListPtr::CheckGeoRefs() const
{
	if (iSize() == 0)
		return;
	GeoRef grf = ma[iLower()]->gr();
	if (grf->fGeoRefNone()) { // still should have same size
		for (int i = iLower()+1; i <= iUpper(); i++) 
			if (ma[i]->rcSize()!= rcSize())
				throw ErrorIncompatibleMapSize(ma[iLower()]->sName(true, fnObj.sPath()), ma[i]->sName(true, fnObj.sPath()),
						sTypeName(), errMapList+4);
	}
	else
		for (int i = iLower()+1; i <= iUpper(); i++) 
			if (ma[i]->gr()!= grf)
				throw ErrorIncompatibleGeorefs(grf->sName(true, fnObj.sPath()), 
						ma[i]->gr()->sName(true, fnObj.sPath()), sTypeName(), errMapList+5);
}

RealMatrix* MapListPtr::mtVarCov(bool fForce)
{
	ObjectTime tmMaps = objtime;
	for (int i = iLower(); i <= iUpper(); ++i) 
	{
		ObjectTime tm = ma[i]->objtime;
		if (tm > tmMaps)
			tmMaps = tm;
	}
	
	if (fForce || tmCalcStat < tmMaps)
		CalculateStats();

	return &_mtVarCov;
}

RealMatrix* MapListPtr::mtCorr(bool fForce)
{
	ObjectTime tmMaps = objtime;
	for (int i = iLower(); i <= iUpper(); ++i) 
	{
		ObjectTime tm = ma[i]->objtime;
		if (tm > tmMaps)
			tmMaps = tm;
	}
	
	if (fForce || tmCalcStat < tmMaps)
		CalculateStats();

	return &_mtCorr;
}

CVector* MapListPtr::vecMean(bool fForce)
{
	if (fForce)
		CalculateStats();
	if (tmCalcStat > objtime)
		return &_vecMean;
	return 0;
}

CVector* MapListPtr::vecStd(bool fForce)
{
	if (fForce)
		CalculateStats();
	if (tmCalcStat > objtime)
		return &_vecStd;
	return 0;
}

void MapListPtr::CalculateStats()
{
	if (tmCalcStat >= objtime)
		return;
	for (int i = iLower(); i <= iUpper(); i++) {
		if(!ma[i]->fCalculated())
			ma[i]->Calc();
	}  
	fChanged = true;
	bool f;
	f = fCalcRealVarCov();
	if (!f) {
		tmCalcStat = 0;
		return;
	}
	CalcCorr();
	tmCalcStat = ObjectTime::timCurr();
	
	// to check OIF calculation, can be removed in final version
	Array<String> asBands;
	IntArray aiBnd1, aiBnd2, aiBnd3;
	GetOIFList(asBands, aiBnd1, aiBnd2, aiBnd3);
	for (int ii=0;ii < asBands.iSize(); ++ii)
		WriteElement("OIF", String("OIF%i", ii).scVal(), asBands[ii]);
	SetAdditionalInfoFlag(true);
	SetAdditionalInfo(sOIF());
	Store();
}

#define DeleteBuf()                      \
	{                                    \
		for (int n = 0; n < iMaps; ++n)  \
		delete mapBuf[n];                \
	}

bool MapListPtr::fCalcRealVarCov()
{
	if (iSize() == 0)
		return false;
	int nr, nx, r, c, i, j,
		iCols = ma[iLower()]->iCols(),
		iRows = ma[iLower()]->iLines(),
		iMaps = iSize();    
	double  ni, nj, ninj;
	LongMatrix matNnij(iMaps);
	RealMatrix matSumX(iMaps), 
		matSumY(iMaps), 
		matSumXY(iMaps);
	
	_mtVarCov = RealMatrix(iMaps);
	_vecMean = CVector(iMaps);
	_vecStd = CVector(iMaps);
	
	// Buffer initialisation for storage of Map.GetLine()
	Array<RealBuf*> mapBuf(iMaps);
	for (nr = 0; nr < iMaps; ++nr)
		mapBuf[nr] = new RealBuf(iCols);
	Tranquilizer trq;
	trq.SetTitle(SMPLTextCalculateVarcovMatrix);
	trq.SetText(SMPLTextCalculating);
	trq.Start();
	
	for (r = 0; r < iRows; ++r) {
		if (trq.fUpdate(r, iRows)) {
			DeleteBuf();
			return false;
		}
		for (nr = 0; nr < iMaps; ++nr)
			ma[nr+iLower()]->GetLineVal(r, *(mapBuf[nr]));
		for (c = 0; c < iCols; ++c) {
			for (nr = 0; nr < iMaps; ++nr) {
				ni = (*(mapBuf[nr]))[c];
				if (ni == rUNDEF) continue;
				for (nx = nr; nx < iMaps; ++nx) {
					nj = (*(mapBuf[nx]))[c];
					if (nj == rUNDEF) continue;
					ninj = ni * nj;
					matNnij(nr, nx) += 1;
					matSumX(nr, nx) += ni;
					matSumY(nr, nx) += nj;
					matSumXY(nr, nx) += ninj;
				}
			}
		}
	}
	for (i = 0; i < iMaps; ++i) {
		for (j = 0; j < iMaps; ++j) {
			if (j >= i)
				_mtVarCov(i, j) =
				(matNnij(i, j) > 1 ?
					matSumXY(i, j) / (matNnij(i, j) - 1) -
					matSumX(i, j) / matNnij(i, j) *
					matSumY(i, j) / matNnij(i, j) *
					matNnij(i, j) / (matNnij(i, j) - 1)
					: 0.0);
			
			else
				_mtVarCov(i, j) = _mtVarCov(j, i);
		}
	}
	for (nr = 0; nr < iMaps; ++nr) {
		_vecMean(nr) = ((double)matSumX(nr, nr)) / matNnij(nr, nr);
		_vecStd(nr) = sqrt(_mtVarCov(nr, nr));
	}
	trq.fUpdate(iRows, iRows);
	trq.HelpEnable(true);
	DeleteBuf();
	return true;
}


bool MapListPtr::fCalcLongVarCov()
{
	if (iSize() == 0)
		return false;
	int nr, nx, r, c, i, j,
		iCols     = ma[iLower()]->iCols(),
		iRows     = ma[iLower()]->iLines(),
		iMaps     = iSize();
	long ni, nj;
	long ninj;
	LongMatrix matNnij(iMaps),
		matSumX(iMaps),
		matSumY(iMaps);
	RealMatrix matSumXY(iMaps);
	
	_mtVarCov = RealMatrix(iMaps);
	_vecMean = CVector(iMaps);
	_vecStd = CVector(iMaps);
	
	// Buffer initialisation for storage of Map.GetLine()
	Array<LongBuf*> mapBuf(iMaps);
	for (nr = 0; nr < iMaps; ++nr)
		mapBuf[nr] = new LongBuf(iCols);
	Tranquilizer trq;
	trq.SetTitle(SMPLTextCalculateVarcovMatrix);
	trq.SetText(SMPLTextCalculating);
	trq.Start();
	
	for (r = 0; r < iRows; ++r) {
		for (nr = 0; nr < iMaps; ++nr) {
			if (trq.fUpdate(r, iRows)) {
				DeleteBuf();
				return false;
			}
			ma[nr+iLower()]->GetLineVal(r, *(mapBuf[nr]));
		}
		for (c = 0; c < iCols; ++c) {
			for (nr = 0; nr < iMaps; ++nr) {
				ni = (*(mapBuf[nr]))[c];
				for (nx = nr; nx < iMaps; ++nx) {
					nj = (*(mapBuf[nx]))[c];
					ninj = ni * nj;
					matNnij(nr, nx)  += 1;
					matSumX(nr, nx)  += ni;
					matSumY(nr, nx)  += nj;
					matSumXY(nr, nx) += ninj;
				}
			}
		}
	}
	for (i = 0; i < iMaps; ++i) {
		for (j = 0; j < iMaps; ++j) {
			if (j >= i)
				_mtVarCov(i, j) =
					(matNnij(i, j) > 1 ?
					matSumXY(i, j) / (matNnij(i, j) - 1) -
					(double)matSumX(i, j) / matNnij(i, j) *
					matSumY(i, j) / matNnij(i, j) *
					matNnij(i, j) / (matNnij(i, j) - 1)
					: 0.0);
			else
				_mtVarCov(i, j) = _mtVarCov(j, i);
		}
	}
	for (nr = 0; nr < iMaps; ++nr) {
		_vecMean(nr) = ((double)matSumX(nr, nr)) / matNnij(nr, nr);
		_vecStd(nr) = sqrt(_mtVarCov(nr, nr));
	}
	trq.fUpdate(iRows, iRows);
	trq.HelpEnable(true);
	DeleteBuf();
	return true;
}

void MapListPtr::CalcCorr()
{
	_mtCorr = RealMatrix(_mtVarCov.iRows());
	for (int i = 0; i < _mtVarCov.iRows(); ++i)
		for (int j = 0; j < _mtVarCov.iCols(); ++j)
			_mtCorr(i, j) = (abs(_mtVarCov(i, i)) > DBL_EPSILON &&
			abs(_mtVarCov(j, j)) > DBL_EPSILON ?
			_mtVarCov(i, j) /
			sqrt(_mtVarCov(i, i) * _mtVarCov(j, j)) : 0.0);
}

double MapListPtr::rOIF(int b1, int b2, int b3) const
{
	return	(_vecStd(b1) + _vecStd(b2) + _vecStd(b3)) / 
		(_mtCorr(b1,b2) + _mtCorr(b1,b3) + _mtCorr(b2, b3));
}

void MapListPtr::GetObjectDependencies(Array<FileName>& afnObjDep)
{
	objdep.Names(afnObjDep);
}

// for OIF list: all combination should be calculated and sorted on largest OIF 
class OIFStruct {
public:  
	double rOIF;
	int b1, b2, b3;
};


static bool fLessOIF(long i, long j, void* p)
{
	OIFStruct* aOIF = static_cast<OIFStruct*>(p);
	return aOIF[i].rOIF < aOIF[j].rOIF;
}

static void SwapOIF(long i, long j, void* p)
{
	OIFStruct* aOIF = static_cast<OIFStruct*>(p);
	OIFStruct oif = aOIF[i];
	aOIF[i] = aOIF[j];
	aOIF[j] = oif;
}

void MapListPtr::GetOIFList(Array<String>& asBands, IntArray& aiBnd1, IntArray& aiBnd2, IntArray& aiBnd3)
{
	CalculateStats();
	int iComb = iSize() * (iSize() - 1) * (iSize() - 2) / 6;
	asBands.Resize(iComb);
	aiBnd1.Resize(iComb);
	aiBnd2.Resize(iComb);
	aiBnd3.Resize(iComb);
	if (iComb == 0)
		return;

	// fill array with OIF's
	OIFStruct* aOIF = new OIFStruct[iComb];
	int n = 0;
	for (int i=iLower(); i <= iUpper(); i++)
		for (int j=i+1; j <= iUpper(); j++)
			for (int k=j+1; k <= iUpper(); k++)
			{
				aOIF[n].rOIF = rOIF(i-iLower(), j-iLower(), k-iLower());
				aOIF[n].b1 = i;
				aOIF[n].b2 = j;
				aOIF[n].b3 = k;
				n++;
			}
	::QuickSort(0, iComb-1, fLessOIF, SwapOIF, (void*)aOIF);
	for (int i=0; i < iComb; ++i)
	{
		asBands[i] = String("%3i:  %-8S %-8S %-8S   (%6.2f)", i+1,
			map(aOIF[iComb-i-1].b1)->fnObj.sFile,
			map(aOIF[iComb-i-1].b2)->fnObj.sFile,
			map(aOIF[iComb-i-1].b3)->fnObj.sFile,
			aOIF[iComb-i-1].rOIF);
		aiBnd1[i] = aOIF[iComb-i-1].b1;
		aiBnd2[i] = aOIF[iComb-i-1].b2;
		aiBnd3[i] = aOIF[iComb-i-1].b3;
	}
	delete [] aOIF;
}

void MapListPtr::Rename(const FileName& fnNew)
{
}

String MapListPtr::sOIF()
{
	Array<String> asBands;
	IntArray aiBnd1, aiBnd2, aiBnd3;
	GetOIFList(asBands, aiBnd1, aiBnd2, aiBnd3);
	String s = "OIF Index Highest Ranking\r\n";
	for (int i=0; i <= min(5, asBands.iSize()-1); ++i)
	{
		s &= asBands[i];
		s &= "\r\n";
	}
	return s;
}

void MapListPtr::SetBandPreFix(const String& sBandPreFix)
{
	_sBandPreFix = sBandPreFix;
	WriteElement("MapList", "BandPreFix", _sBandPreFix);
}

const String& MapListPtr::sBandPreFix() const
{
	return _sBandPreFix;
}

String MapListPtr::sDisplayName(long iBandNr) const
{
	Map mp = map(iBandNr);
	if ("" == mp->fnObj.sSectionPostFix)
		return mp->sName();

	if (_sBandPreFix.length() > 0) 
		return String("%S %li", _sBandPreFix, iBandNr);
	else
		return String("Band %li", iBandNr);
}

String MapListPtr::sDisplayName(const FileName& fn) const
{
	if ("" == fn.sSectionPostFix)
		return fn.sRelative();

	String sBand = fn.sSectionPostFix.sTail(":");
	int iBandNr = sBand.iVal();
	if (_sBandPreFix.length() > 0) 
		return String("%S %li", _sBandPreFix, iBandNr);
	else
		return String("Band %li", iBandNr);
}

// The input name may be a display filename 
FileName MapListPtr::fnRealName(const FileName& fn) const
{
	String sBand = fn.sFile.sHead(" ");
	if (fCIStrEqual(sBand, _sBandPreFix))
	{
		sBand = fn.sFile.sTail(" ");
		FileName fnReal = fnObj;
		fnReal.sSectionPostFix = String(":%S", sBand);

		return fnReal;
	}

	return fn;
}

void MapListPtr::AddMap(const Map& mp)
{
	if (!fOK(mp))
		throw ErrorObject(String(SMPLErrDomainGeoRefMismatch_S.scVal(), mp->sName()));
	int i = iLower();
	for(; i <= iUpper(); ++i)
	{
		if (ma[i]->fnObj == mp->fnObj)
			break;
	}
	if (i > iUpper())
		ma &= mp;
	ObjectInfo::WriteAdditionOfFileToCollection(mp->fnObj, fnObj);
		Updated();
}

bool MapListPtr::fOK(const Map& mp)
{
	if (iSize() == 0 ) return true;
		return ma[iLower()]->dm() == mp->dm() && gr() == mp->gr();
}

void MapListPtr::RemoveMap(const FileName& fn)
{
	for(int i = iLower(); i <= iUpper(); ++i)
	{
		if (ma[i]->fnObj == fn)
		{
			ma.Remove(i,1);
			break;
		}
	}
	ObjectInfo::WriteRemovalOfFileFromCollection(fn, fnObj);
	Updated();
}

void MapListPtr::MoveMap(int iNewIndex, const Map& mp)
{
	if (iNewIndex < iSize() && mp.fValid())
	{
		for(int i = iLower(); i <= iUpper(); ++i)
		{
			if (ma[i]->fnObj == mp->fnObj)
			{
				ma.Remove(i,1);
				break;
			}
		}
		ma.insert(ma.begin() + iNewIndex, mp);
	}
	Updated();
}

void MapListPtr::GetObjectStructure(ObjectStructure& os)
{
	IlwisObjectPtr::GetObjectStructure(os);
	if ( os.fGetAssociatedFiles() )
	{
		bool fLeaveFiles = os.fRetrieveContentsContainer() ? false : true;
		os.AddFile(fnObj,"MapList", "Domain", ".dom" );
		os.AddFile(fnObj,"MapList", "GeoRef", ".grf");
		int iNr;
		ObjectInfo::ReadElement("MapList", "Maps", fnObj, iNr);
		if (iNr != iUNDEF &&
			((os.caGetCommandAction() != ObjectStructure::caDELETE) || !fUseAs()) )
		{
			String sMap;
			ObjectInfo::ReadElement("MapList", "Map1", fnObj, sMap);
			if ( sMap.find(":1") != String::npos)
			{
				os.AddFile(fnObj, "MapStore:1", "Data");
				os.AddFile(fnObj, sMap, "Domain", ".dom");
			}
			else
			{
				for(int i = iLower(); i <= iUpper(); ++i)
				{
					String sEntry("Map%d", i);
					os.AddFile(fnObj, "MapList", sEntry, "", fLeaveFiles);
				}	
			}
		}
	}
	else if (os.caGetCommandAction() != ObjectStructure::caCOPY &&
			 os.caGetCommandAction() != ObjectStructure::caDELETE)
	{
		for (int i = iLower(); i <= iUpper(); i++)
			ma[i]->GetObjectStructure(os);
	}
}

bool MapListPtr::fUsesDependentObjects() const
{
	if (fDependent())
		return true;
	for(int i = iLower(); i <= iUpper(); ++i)
	{
		Map mp = map(i);
		if (mp.fValid())
		{
			if (mp->fDependent())
				return true;
		}
	}
	return false;
}

void MapListPtr::CreatePyramidLayers()
{
	for(int i=iLower(); i <= iUpper(); ++i)
	{
		Map mp = map(i);
		if ( mp.fValid() )
		{
			mp->CreatePyramidLayer();
		}
	}
}

void MapListPtr::DeletePyramidFiles()
{
	for(int i=iLower(); i <= iUpper(); ++i)
	{
		Map mp = map(i);
		if ( mp.fValid() )
		{
			mp->DeletePyramidFile();
		}
	}
}

bool MapListPtr::fHasPyramidFiles()
{
	int iCount = 0;
	for(int i=iLower(); i <= iUpper(); ++i)
	{
		Map mp = map(i);
		if ( mp.fValid() )
		{
			if ( mp->fHasPyramidFile() )
				iCount++;
		}
	}
	return iCount == iSize() && iCount > 0 ? true : false;
}

void MapListPtr::BreakDependency()
{
	if (!fCalculated())
		Calc();
	if (!fCalculated())
		return; 
	ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
	delete pmlv;
	pmlv = 0;
	fChanged = true;
	
	ObjectInfo::WriteElement("MapList", "Type", fnObj, (char*)0);
	Store();
}

bool MapListPtr::fDependent() const
{
	ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
	if (0 != pmlv)
		return true;
	String s;
	ObjectInfo::ReadElement("MapList", "Type", fnObj, s);
	return fCIStrEqual(s, "MapListVirtual");
}

String MapListPtr::sExpression() const
{
	ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
	if (0 != pmlv)
		return pmlv->sExpression();
	return IlwisObjectPtr::sExpression();
}

void MapListPtr::Calc(bool fMakeUpToDate)
// calculates the result
{
	ILWISSingleLock sl(&csCalc, TRUE, SOURCE_LOCATION);
	OpenMapListVirtual();
	if (fMakeUpToDate)
		if (!objdep.fUpdateAll())
			return;

	if (0 != pmlv)
		pmlv->Freeze();
}

void MapListPtr::DeleteCalc()
// deletes calculated  result     
{
	ILWISSingleLock sl(&csCalc, TRUE, SOURCE_LOCATION);
	OpenMapListVirtual();
	if (0 != pmlv) 
		pmlv->UnFreeze();
}

void MapListPtr::OpenMapListVirtual()
{
	ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
	if (0 != pmlv) // already there
		return;

	if (!fDependent())
		return;

	try
	{
		pmlv = MapListVirtual::create(fnObj, *this);
		objdep = ObjectDependency(fnObj);
	}
	catch (const ErrorObject& err)
	{
		err.Show();
		pmlv = 0;
		objdep = ObjectDependency();
	}
}

IlwisObjectVirtual* MapListPtr::pGetVirtualObject() const
{
	return pmlv;
}

bool MapListPtr::fCalculated() const
{
	if (!fDependent())
		return IlwisObjectPtr::fCalculated();
	if (iSize() == 0)
		return false;
	for(int i = iLower(); i <= iUpper(); ++i)
	{
		Map mp = map(i);
		if (!mp.fValid())
			return false;
		if (!mp->fCalculated())
			return false;
	}
	return true;
}

void MapListPtr::SetSize(int iSize)
{
	iOffset = 0;
	ma.Resize(iSize);
}

void MapListPtr::SetDomainValueRangeStruct(const DomainValueRangeStruct& dvrs)
{
	OpenMapListVirtual();
	if (0 != pmlv)
		pmlv->SetDomainValueRangeStruct(dvrs);
}

bool MapListPtr::fTblAtt() const {
	return attTable.fValid();
}
Table MapListPtr::tblAtt() const {
	return attTable;
}
void MapListPtr::SetAttributeTable(const Table& tbl){
	attTable = tbl;
	fChanged = true;
}