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
/* $Log: /ILWIS 3.0/RasterStorage/MAPSTORE.cpp $
 * 
 * 48    24-01-03 18:01 Willem
 * - Changed: Mapstore now checks on UseAs data files before removing it
 * in Unstore()
 * 
 * 47    24-01-03 15:34 Willem
 * - Added: Synchronization with the UseAs status of the MapPtr containing
 * this MapStore
 * 
 * 46    9-01-03 12:18 Willem
 * - Added: Extra check for the data file in MapStore constructor for
 * opening an existing map. It will throw a Find error if the file is
 * missing. The check is done here to allow ILWIS to catch the error in an
 * appropriate place; the error was found originally but as a CException
 * (thrown by CFile)
 * 
 * 45    11-10-02 15:15 Willem
 * - Changed: MapStoreBase::create now simple ask the needed storetype
 * from its DomainValueRangeStruct instead of trying to calculate it
 * himself
 * 
 * 44    7/08/02 3:23p Martin
 * adapted the call to the xreation of the foreignformat object to reflect
 * the new structure
 * 
 * 43    6/03/02 2:39p Martin
 * Merge from ASTER branch
 * 
 * 43    5/03/02 9:24a Martin
 * protected against foreign format not being created (e.g. data file
 * missing)
 * 
 * 42    3-06-02 11:56 Koolhoven
 * in constructor take st() from dvs, do not use internals from dvs
 * ourselves
 * 
 * 41    2/07/02 8:43a Martin
 * offset of pyramids were not properly initialized when a create was done
 * after a previous aborted create
 * 
 * 40    1/24/02 15:59 Willem
 * The constructor for foreign raster formats now initializes the fUseAs
 * member to true
 * 
 * 39    11/21/01 17:05 Willem
 * Now pyramid file is also opened when map data file is read only:
 * opening with RW_All caused exception and therefore skipping the pyramid
 * loading.
 * 
 * 38    5-11-01 17:59 Koolhoven
 * in MapStore::CreatePyramidLayers() set fChanged = true instead of
 * calling Updated()
 * 
 * 37    8/24/01 13:03 Willem
 * Removed the SetReadOnly() function. This is now handled by
 * IlwisObjectPtr::SetReadOnly() for all ilwis objects
 * 
 * 36    13-08-01 16:57 Koolhoven
 * readability: replaced "== false" by a "!"
 * 
 * 35    6/28/01 2:29p Martin
 * 
 * 34    6/28/01 2:20p Martin
 * added mutexes to protectt against conceurrently creating and displaying
 * 
 * 33    6/28/01 12:54p Martin
 * tranquilzer in CreatePyramidFile does not show the number anymore only
 * the gauge
 * 
 * 32    6/28/01 12:32p Martin
 * the path used by the pyramids was incorrect if a maplist in a different
 * dir used the maps and the pyramids were created from the maplist
 * (current dir was different)
 * 
 * 31    6/28/01 12:16p Martin
 * small mistake when checking if a domain is domain bit, should not check
 * storetype (in createpyramidfiles)
 * 
 * 30    6/28/01 9:08a Martin
 * no pyramids for domain bit
 * 
 * 29    6/19/01 11:08a Martin
 * naming changed for the pyramid file, and the initialization changed.
 * added possibility to return the number of pyramidlayers
 * 
 * 28    6/14/01 2:15p Martin
 * all IterateCreatePyramidLayer functions now have a tranquilzier
 * 
 * 27    6/14/01 11:09a Martin
 * added support for deleting pyramidfiles
 * 
 * 26    6/13/01 4:01p Martin
 * implemented pyramid file handling
 * 
 * 25    6/05/01 4:29p Martin
 * saving and loading of pyramid files is supported
 * 
 * 24    15/03/01 15:58 Willem
 * Before using unlink() now a check on the existance of the file done
 * first
 * 
 * 23    12-03-01 9:57a Martin
 * not the dvrs is passed to foreignformat but the actualmap (filename).
 * More info was needed at export level and only the map could give this.
 * 
 * 22    15-02-01 4:01p Martin
 * the dvrs must be added to the putlineval functions as the rpr must be
 * used when exporting to formats that do not support the ilwis range of
 * domains
 * 
 * 21    5/02/01 13:06 Willem
 * The export function now first copies a file before trying to open it,
 * avoiding sharing violations
 * 
 * 20    23-01-01 11:03a Martin
 * UseAs is now a member of MapStore. Had to change some constructors. No
 * Data entry will now be present in the ODF. the data for the
 * foreignformat is in the foreignformat section
 * 
 * 19    17-01-01 2:59p Martin
 * changed the flags when opening data files to prevent sharing violations
 * 
 * 18    12-01-01 9:31a Martin
 * extended the check for northoriented with georefnone in StoreAs. Georef
 * none may also be eported
 * 
 * 17    10-01-01 4:26p Martin
 * implemented StoreAs which is able to save through foreign format an
 * ilwis file in an external format. StoreAs needs an expression of the
 * type Format(OutPutFile, Method)
 * 
 * 16    22/11/00 14:30 Willem
 * Added virtual function to retrieve the StoreType
 * 
 * 15    26/10/00 15:49 Willem
 * "Use As" data files of Maps are now opened with a new filecreate
 * constant facRW_All. This has the share option ShareDenyNone. This will
 * prevent sharing violations, expecially in case of maplist with for
 * instance a LAN file.
 * 
 * 14    24-10-00 11:52a Martin
 * changed the getobjectstructure function
 * 
 * 13    12-10-00 11:42a Martin
 * will now catch a CFileException when trying to open readonly files (and
 * open it as RO)
 * 
 * 12    11-09-00 11:09a Martin
 * add functions for GetObjectStructure and DoNotUpdate
 * 
 * 11    17-03-00 9:54a Martin
 * extra constructors for foreignformat support
 * 
 * 10    20/01/00 15:10 Willem
 * Map size is set to size of GeoRef (important in case of maps dependent
 * on georefs, such as MapSubMap etc).
 * 
 * 9     11-01-00 16:01 Wind
 * when creating MapStore now the current store type is checked and not
 * the store type of the DomainValueRangeStruct at creation time of the
 * map
 * 
 * 8     26-11-99 13:48 Wind
 * throw error when data file not found
 * 
 * 7     29-10-99 12:52 Wind
 * case sensitive stuff
 * 
 * 6     9/27/99 11:14a Wind
 * changed calls to static funcs ObjectInfo::ReadElement and WriteElement
 * to member function calls
 * 
 * 5     9/20/99 10:50a Wind
 * added support for swapbytes, 4 byte reals and pixel interleaved
 * 
 * 4     9/16/99 12:45p Wind
 * added support for pixel interleaved storage
 * 
 * 3     3/09/99 4:24p Martin
 * //->/*
 * 
 * 2     3/09/99 2:26p Martin
 * _stricmp used for string compares as strings are now case sensitive
// Revision 1.8  1998/09/16 17:24:28  Wim
// 22beta2
//
// Revision 1.7  1997/09/26 14:08:06  Dick
// Cleanup of Export to 1.4 no real changes
//
// Revision 1.6  1997/09/25 14:46:11  Dick
// Added in conversion to Ilwis 1.4 conversion from georef Control Tiepoints to
// Geo Ref simple (if possible) Dick Visser
//
// Revision 1.5  1997/09/18 19:08:01  Willem
// The .INF file may not have internal spaces. Added: replacement of spaces by underscore chars.
//
// Revision 1.4  1997/08/11 10:56:41  Wim
// Creating a MapStore with an unallowed storetyp now throws an exception.
//
// Revision 1.3  1997-08-08 00:08:04+02  Willem
// Export to Ilwis 1.4 truncates strings for the INF table to 20 characters.
//
// Revision 1.2  1997/07/28 20:10:10  Willem
// Exporting a bool map gives now visible colors (0=undef, 127=false, 255=true)
// The LUT still contains grey colors.
//
/* MapStore
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  DV   25 Sep 97    4:13 pm
*/

#include "Engine\Domain\dm.h"
#include "Engine\Domain\Dmvalue.h"
#include "Engine\Map\Raster\MAPSTORE.H"
#include "Engine\Map\Raster\MAPBIT.H"
#include "Engine\Map\Raster\MAPBYT.H"
#include "Engine\Map\Raster\MAPINT.H"
#include "Engine\Map\Raster\MAPLONG.H"
#include "Engine\Map\Raster\MapFloat.h"
#include "Engine\Map\Raster\MAPREAL.H"
#include "Engine\Map\Raster\MPPTBIT.H"
#include "Engine\Map\Raster\MPPTBYT.H"
#include "Engine\Map\Raster\MPPTINT.H"
#include "Engine\Map\Raster\MPPTLONG.H"
#include "Engine\Map\Raster\MPPTREAL.H"
#include "Engine\DataExchange\ForeignFormat.h"
#include "Engine\DataExchange\MapStoreForeignFormat.h"
#include "Engine\SpatialReference\Grfactor.h"
#include "Engine\SpatialReference\Grsmpl.h"
#include "Engine\SpatialReference\Grctppla.h"
#include "Engine\Domain\dmsort.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Base\DataObjects\ObjectStructure.h"
#include "Engine\Base\DataObjects\Tranq.h"
#include "Engine\Base\DataObjects\ERR.H"
#include "Engine\Base\System\mutex.h"
#include "Engine\Base\System\Engine.h"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Hs\map.hs"
#include "Headers\Hs\IMPEXP.hs"

MapStore::MapStore(const FileName& fn, MapPtr& p)
: fnObj(p.fnObj), ptr(p)
{
	String sSection = "MapStore";
	String sType;
	ptr.ReadElement(sSection.c_str(), "Type", sType);
	ptr.ReadElement(sSection.c_str(), "UseAs", fUseAs);
	ptr.SetUseAs(fUseAs);  // make sure the MapPtr also knows the UseAs status
	String sStruc;
	ptr.ReadElement(sSection.c_str(), "Structure", sStruc);
	if (sStruc.length() == 0)
		sStruc = "Line";
	FileName fnDat;
	ILWIS::Version::BinaryVersion fvFormatVersion;
	ptr.ReadElement("MapStore", "Format", (int &)fvFormatVersion);
	if ( fvFormatVersion == shUNDEF)
		fvFormatVersion = ILWIS::Version::bvFORMAT30;
	getEngine()->getVersion()->fSupportsBinaryVersion(fvFormatVersion);
    ptr.setVersionBinary(fvFormatVersion);
	if ( !fCIStrEqual(sType , "foreignformat") )
	{
		if ( 0 == ptr.ReadElement("ForeignFormat", "Filename", fnDat))			
			if (0 == ptr.ReadElement(sSection.c_str(), "Data", fnDat)) 
			{
				mpsb = 0;
				throw(ErrorObject());
			}
			else if (!File::fExist(fnDat))
				throw ErrorNotFound(fnDat);
	}
	
	if (_stricmp("Line" , sStruc.c_str()) == 0) 
	{
		if (_stricmp("bit" ,  sType.c_str()) == 0)
			mpsb = new MapBit(fnDat, *this);
		else if (_stricmp("byte" , sType.c_str()) == 0)
			mpsb = new MapByte(fnDat, *this);
		else if (_stricmp("int" , sType.c_str()) == 0)
			mpsb = new MapInt(fnDat, *this);
		else if (_stricmp("long" , sType.c_str()) == 0)
			mpsb = new MapLong(fnDat, *this);
		else if (_stricmp("float" , sType.c_str()) == 0)
			mpsb = new MapFloat(fnDat, *this);
		else if (_stricmp("real" , sType.c_str()) == 0)
			mpsb = new MapReal(fnDat, *this);
		else if ( fCIStrEqual(sType , "foreignformat"))
		{
			ParmList pm;
			ForeignFormat *ff = ForeignFormat::Create(fn, pm); // This is only a "probe" to check if we can read this format; a 2nd and final "Create" call is triggered by the next call to one of the MapStoreForeignFormat member functions (iRaw, GetLineRaw etc).
			if ( ff ) {
				delete ff; // clean-up the probe-object
				mpsb = new MapStoreForeignFormat(*this); 
			}
			else
				throw ErrorObject(TR("This fileformat is in its native form nnot supported by ILWIS"));
		}
		else
			InvalidTypeError(fn, "Structure", sType);
		MapLine* mpl = static_cast<MapLine*>(mpsb);
		long i;
		ptr.ReadElement("MapStore", "StartOffset", i);
		if (i != iUNDEF)
			mpl->iStartOffset = i;
		else
			mpl->iStartOffset = 0;
		ptr.ReadElement("MapStore", "RowLength", i);
		if (i != iUNDEF)
			mpl->iRowLength = i;
		else
			mpl->iRowLength = iCols();
		bool f;
		ptr.ReadElement(sSection.c_str(), "PixelInterLeaved", f);
		mpl->fPixelInterLeaved = f;
		ptr.ReadElement(sSection.c_str(), "SwapBytes", f);
		mpl->fSwapBytes= f;
		mpl->iNrBands = mpl->iRowLength / iCols();
	}
	else if ("Patch" == sStruc) {
		if (_stricmp("byte" , sType.c_str()) == 0)
			mpsb = new MapPatchByte(fnDat, *this);
		else if (_stricmp("int" , sType.c_str()) == 0)
			mpsb = new MapPatchInt(fnDat, *this);
		else if (_stricmp("long" , sType.c_str()) == 0)
			mpsb = new MapPatchLong(fnDat, *this);
		else if (_stricmp("real" , sType.c_str()) == 0)
			mpsb = new MapPatchReal(fnDat, *this);
		else
			InvalidTypeError(fn, "Structure", sType);
	}
	else
		InvalidTypeError(fn, "Structure", sStruc);
	mpsb->dvs = dvrs();
	mpsb->rcSize = rcSize();
	MapPatch* mpt = dynamic_cast<MapPatch*>(mpsb);
	if (0 != mpt)
		mpt->BuildRedirTable();
	long l;
	if (0 == ptr.ReadElement("MapStore", "StoreTime", l)) {
		if (0 != mpsb->file)
			timStore = mpsb->file->filetime();
	} 
	else
		timStore = l;
}

MapStore::MapStore(const FileName& fn, MapPtr& p, const LayerInfo& inf) :
	fnObj(p.fnObj),
  ptr(p)
{
	mpsb = new MapStoreForeignFormat(*this, inf);
	ptr.setVersionBinary(ILWIS::Version::bvFORMATFOREIGN);
	fUseAs = true;
	ptr.SetUseAs(fUseAs);  // make sure the MapPtr also knows the UseAs status
	if (0 != mpsb->file)
		timStore = mpsb->file->filetime();	
}

MapStore::MapStore(const FileName& fn, MapPtr& p, const GeoRef& gr, const RowCol& rcSiz, 
                           const DomainValueRangeStruct& dvs,
                           const MapFormat mf, bool _fUseAs)
: fnObj(p.fnObj), ptr(p), fUseAs(_fUseAs)
{
	ptr.SetUseAs(fUseAs);  // make sure the MapPtr also knows the UseAs status
	if ( fUseAs)
		ptr.setVersionBinary(ILWIS::Version::bvFORMATFOREIGN);
	FileName fnDat;
	if (fCIStrEqual(fn.sExt, ".mpr") && !fUseAs ) // not for maps in map lists
		fnDat = FileName(fn, ".mp#", true);
	RowCol rcSize;
	if (gr->fGeoRefNone())
	{
		rcSize = rcSiz;
		p.SetSize(rcSiz);
	}
	else
	{
		rcSize = gr->rcSize();
		p.SetSize(gr->rcSize());
	}
	StoreType st = dvs.st();
	if (mfLine == mf) {
		switch (st) {
		case stBIT:
			mpsb = new MapBit(fnDat, *this,rcSize,dvs, 0, rcSize.Col, false, false);
			break;
		case stDUET:
		case stNIBBLE:
		case stBYTE:
			mpsb = new MapByte(fnDat, *this,rcSize,dvs, 0, rcSize.Col, false, false);
			break;
		case stINT:
			mpsb = new MapInt(fnDat, *this,rcSize,dvs, 0, rcSize.Col, false, false);
			break;
		case stLONG:
			mpsb = new MapLong(fnDat, *this,rcSize,dvs, 0, rcSize.Col, false, false);
			break;
		case stFLOAT:
			mpsb = new MapFloat(fnDat, *this,rcSize,dvs, 0, rcSize.Col, false, false);
			break;
		case stREAL:
			mpsb = new MapReal(fnDat, *this,rcSize,dvs, 0, rcSize.Col, false, false);
			break;
		case stCRD:
			InvalidTypeError(fn, "Structure", "Coordinate");
			break;
		case stSTRING:
			InvalidTypeError(fn, "Structure", "String");
			break;
		case stBINARY:
			InvalidTypeError(fn, "Structure", "Binary");
			break;
		default:
			InvalidTypeError(fn, "Structure", "Unknown");
		};
	}
	else if (mfPatch == mf) {
		switch (st) {
		case stBIT:
			//        return new MapPatchBit(fn,gr,dm);
		case stDUET:
		case stNIBBLE:
		case stBYTE:
			mpsb = new MapPatchByte(fnDat, *this,rcSize,dvs);
			break;
		case stINT:
			mpsb = new MapPatchInt(fnDat, *this,rcSize,dvs);
			break;
		case stLONG:
			mpsb = new MapPatchLong(fnDat, *this,rcSize,dvs);
			break;
		case stREAL:
			mpsb = new MapPatchReal(fnDat, *this,rcSize,dvs);
			break;
		};
	}
	//  ptr._fDataReadOnly = false;
	if (0 != mpsb->file)
		timStore = mpsb->file->filetime();
}

MapStoreBase::MapStoreBase(const FileName& fnDat, MapStore& _mps)
: fChanged(_mps.ptr.fChanged),
  filePyramid(0)
{
	file = 0;
	if ( !fnDat.fValid())
		return;

	try
	{
		// for some reason the shareDenyWrite also prevents other acces from other threads to
		// the same file with RW flag on. For the moment the flag below is used, the msdn does not
		// give a hint what could be done
		file = new File(fnDat, facRW_All);
	}
	catch (CFileException *ex) 
	{
		file = new File(fnDat, facRO);
		ex->Delete();
	}
	FileName fnPyr;
	ObjectInfo::ReadElement("MapStore", "PyramidFile", _mps.fnMap(), fnPyr);
	if ( fnPyr.fExist() )
		InitializePyramids(fnPyr);
}


MapStoreBase::MapStoreBase(const FileName& fnDat, MapStore& _mps, const RowCol& rcSiz, const DomainValueRangeStruct& dvrs)
: fChanged(_mps.ptr.fChanged),
  filePyramid(0)
{
  rcSize = rcSiz;
  dvs = dvrs;
  if (!fnDat.fValid()) {
    file = 0;
    return;
  }
  FileName filename = fnDat;
  if (filename.sFile.length()==0)
    filename = FileName::fnUnique(fnDat);
  String sFile = filename.sFullName();
  file = 0;
  file = new File(sFile, facCRT);
}

MapStoreBase* MapStoreBase::create(const FileName& fnDat, MapStore& p,  const RowCol& rcSize, const DomainValueRangeStruct& dvrs)
{
  StoreType st = dvrs.st(); // !dvrs.vr().fValid() ? dvrs.dm()->stNeeded() : dvrs.vr()->stUsed();
  switch (st) {
    case stBIT:
      return new MapBit(fnDat, p,rcSize,dvrs, 0, rcSize.Col, false, false);
    case stDUET:
    case stNIBBLE:
    case stBYTE:
      return new MapByte(fnDat, p,rcSize,dvrs, 0, rcSize.Col, false, false);
    case stINT:
      return new MapInt(fnDat, p,rcSize,dvrs, 0, rcSize.Col, false, false);
    case stLONG:
      return new MapLong(fnDat, p,rcSize,dvrs, 0, rcSize.Col, false, false);
    case stFLOAT:
      return new MapFloat(fnDat, p,rcSize,dvrs, 0, rcSize.Col, false, false);
    case stREAL:
      return new MapReal(fnDat, p,rcSize,dvrs, 0, rcSize.Col, false, false);
  }
  return 0;
}  

void MapStore::Store()
{
	if (0 == mpsb)
		return;
	ptr.WriteElement("MapStore", "StoreTime", (long)timStore);
	if (!fUseAs && 0 != mpsb->file && File::fExist( mpsb->file->sName()))
		ptr.WriteElement("MapStore", "Data", FileName(mpsb->file->sName()));
	MapLine* mpl = dynamic_cast<MapLine*>(mpsb);
	if (0 != mpl) {
		ptr.WriteElement("MapStore", "Structure", "Line");
		ptr.WriteElement("MapStore", "StartOffset", mpl->iStartOffset);
		ptr.WriteElement("MapStore", "RowLength", mpl->iRowLength);
		ptr.WriteElement("MapStore", "PixelInterLeaved", mpl->fPixelInterLeaved);
		ptr.WriteElement("MapStore", "SwapBytes", mpl->fSwapBytes);
	}  
	else
		ptr.WriteElement("MapStore", "Structure", "Patch");
	ptr.WriteElement("MapStore", "Type", mpsb->sType());
	ptr.WriteElement("MapStore", "UseAs", fUseAs);
	ILWIS::Version::BinaryVersion fvFormatVersion = ptr.getVersionBinary();
	ptr.WriteElement("MapStore", "Format", (long)fvFormatVersion);
	if ( mpsb->filePyramid )
	{
		FileName fn(String(mpsb->filePyramid->GetFilePath()));
		ptr.WriteElement("MapStore", "PyramidFile", fn.sRelative());
	}		
	mpsb->Store(); // only implemented in MapStoreForeignFormat
}

// static function
void MapStore::UnStore(const FileName& fnObjFile)
{
	// Do not throw away UseAs data files!
	bool fUseAs;
	ObjectInfo::ReadElement("MapStore", "UseAs", fnObjFile, fUseAs);
	if (!fUseAs)
	{
		FileName fnData;
		ObjectInfo::ReadElement("MapStore", "Data", fnObjFile, fnData);
		if (File::fExist(fnData))
			_unlink(fnData.sFullName(true).c_str()); // delete data file if it's still there
		ObjectInfo::WriteElement("MapStore", (char*)0, fnObjFile, (char*)0);
	}
}  

MapStore::~MapStore()
{
  if (0 != mpsb) {
    if (ptr.fErase && (0 != mpsb->file))
      mpsb->file->SetErase();
    delete mpsb;
  }
}

MapStoreBase::~MapStoreBase()
{
  if (0 != file)
    delete file;
	if ( filePyramid != 0 )
	{
		delete filePyramid;
	}		
}

void MapStore::Updated()
{
  ptr.Updated();
  timStore = ptr.objtime;
}

void MapStore::SetErase(bool f)
{
  if (0 != mpsb)
    mpsb->SetErase(f);
} 

void MapStoreBase::SetErase(bool f)
{
  if (0 != file)
    file->SetErase(f);
}

double MapStoreBase::rPredominant(double rV1, double rV2, double rV3, double rV4)
{
	vector<double> rV(4,0);
	rV[0] = rV1;
	rV[1] = rV2;
	rV[2] = rV3;
	rV[3] = rV4;
	
	vector<long> iC(4,0);
	for( int i=0; i< 4; ++i)
	{
		for(int j=i; j < 4; ++j)
		{
			if (rV[i] == rV[j] )
				iC[i]++;
		}						
	}
	int iMx = 0;
	for(int i=1; i < 4; ++i)
	{
		if ( iC[i] < iC[i-1] )
			iMx = i;
	}
	return rV[iMx];	
}

struct MpiStruct  // as in version 1.x
{
  short iLines, iCols;
  short iMinVal, iMaxVal;
  short iMapType, iPatched;
  short iScale, iCoeff;
  float a11, a12, a21, a22, b1, b2;
  MpiStruct(short iL, short iC) {
    iLines = iL; iCols = iC;
    iMinVal = 0; iMaxVal = -1;
    iMapType = 1; iPatched = 0;
    iScale = iCoeff = 0;
    a11 = a12 = a21 = a22 = b1 = b2 = 0.0;
  }
};

/* --------------- Errors by Exceptions ----------------- */
/* --- Interface ---------------------------------------- */
class ErrorExportDomain : public ErrorObject
{
public:
  ErrorExportDomain(const Domain& dm, IlwisError err);
};

/* --------------- Errors by Exceptions ----------------- */
/* --- Implementation ----------------------------------- */
ErrorExportDomain::ErrorExportDomain(const Domain& dm, IlwisError err)
  : ErrorObject()
{
  String s;
  switch (err - errMapExport) {
    case 1:
      s = String(TR("Too many classes for domain %S").c_str(), dm->sName());
      break;
    case 2:
      s = String(TR("Map with domain %S can't be exported").c_str(), dm->sName());
      break;
    case 3:
      s = String(TR("Value Range of map %S too large to export").c_str(), dm->sName());
      break;
    default :
      s = TR("Unknown error");
      break;
  }
  what = WhatError(s, err);
  where.SetTitle(TR("Map export"));
}

/* --------------- static functions ----------------- */
static void CreateColFile(const FileName& fn, const Representation& rpr, bool fGrey)
{
  const double rFac = 1000.0/255.0;
  if (!rpr.fValid() && !fGrey)
    return;
  try {
    File filCol(FileName(fn, ".col"), facCRT);
    filCol.SetErase(true);
    filCol.WriteLnAscii("Red% Green% Blue% #0");
    ColorBuf bufColor;
    if (fGrey || !rpr.fValid()) {
      bufColor.Size(256);
      for (short i=0; i<256; i++)
        filCol.WriteLnAscii(String("%i %i %i", shortConv(i*rFac), shortConv(i*rFac), shortConv(i*rFac)));
    }
    else {  
      rpr->GetColors(bufColor);
      for (short i=0; i<bufColor.iSize(); i++)
        filCol.WriteLnAscii(String("%i %i %i",
                            shortConv(bufColor[i].red()*rFac),
                            shortConv(bufColor[i].green()*rFac),
                            shortConv(bufColor[i].blue()*rFac)));
    }
    filCol.SetErase(false);
  }
  catch (const FileErrorObject& err) {
    err.Show();
  }
}

void MapStore::Export(const FileName& fn) const
{
	if ((iLines() > SHRT_MAX) || (iCols() > SHRT_MAX))
		return; // error

	File filMpi(FileName(fn, ".MPI"), facCRT);
	filMpi.SetErase(true);
	FileName fnMPD = FileName(fn, ".MPD");
	File *filMpd = 0;
	MpiStruct mpi(shortConv(iLines()), shortConv(iCols()));
	// version 1.6 start
	
	GeoRef grf = gr();
	GeoRefCTPplanar* grCTP = dynamic_cast<GeoRefCTPplanar*>(grf.ptr());
	if (grCTP)
		grf = grCTP->grConvertToSimple();
	GeoRefSmpl* grSmpl = dynamic_cast<GeoRefSmpl*>(grf.ptr());
	if (grSmpl)
	{
		mpi.iCoeff = 1;
		double a11, a12, a21, a22, b1, b2;
		grSmpl->GetSmpl(a11, a12, a21, a22, b1, b2);
		mpi.a11 = (float)a11; mpi.a12 = (float)a12;
		mpi.a21 = (float)a21; mpi.a22 = (float)a22;
		mpi.b1  = (float)b1;  mpi.b2  = (float)b2;
	}
	
	if (0 != mpsb->file)
	{
		if (dm()->pdi()) {            // image?
			File::Copy(mpsb->file->sName(), fnMPD);
			filMpd = new File(fnMPD, facRW);  // open file after copy to avoid sharing problems
			filMpd->SetErase(true);            // this is necessary to allow removal of file again in case of exceptions
			CreateColFile(fn, dm()->rpr(), 0 != dm()->pdv());
		}
		else if (dm()->pdbit()) {     // bit?
			mpi.iMapType = 0;
			File::Copy(mpsb->file->sName(), fnMPD);
			filMpd = new File(fnMPD, facRW);  // open file after copy to avoid sharing problems
			filMpd->SetErase(true);            // this is necessary to allow removal of file again in case of exceptions
		}
		else if (dm()->pdbool())  {   // bool?
			ByteBuf bufOut(iCols());
			filMpd = new File(fnMPD, facCRT);
			filMpd->SetErase(true);
			for (short i=0; i<iLines(); i++)
			{
				GetLineRaw(i, bufOut);
				for (short j=0; j < iCols(); j++)
					bufOut[j] = bufOut[j] == 0 ? 0 : bufOut[j] * 128 - 1;
				filMpd->Write(iCols(), bufOut.buf());
			}
			CreateColFile(fn, dm()->rpr(), 0 != dm()->pdv());
		}
		else if (dvrs().fValues()) {  // values? 
			
			if (dvrs().fRealValues())   // floating point values?
			{
				double rCalcStep = dvrs().rrMinMax().rHi() - dvrs().rrMinMax().rLo();
				rCalcStep /= 65000L;  // calculate realistic stepsize
				if (dvrs().rStep() > rCalcStep)
					rCalcStep = dvrs().rStep();  // stored step size is larger than calculated
				// so use the stored step size
				
				mpi.iMapType = 2;
				mpi.iScale = (short)(ceil(log10(rCalcStep)));
				if (mpi.iScale <= -SHRT_MAX || mpi.iScale >= SHRT_MAX)
					// Cannot convert: value range is too large
					throw ErrorExportDomain(dm(), errMapExport + 3);
				double rFact = pow(10, (double)-mpi.iScale);
				
				IntBuf bufOut(iCols());
				RealBuf bufIn(iCols());
				filMpd = new File(fnMPD, facCRT);
				filMpd->SetErase(true);
				for (long i = 0; i < iLines(); i++)
				{
					GetLineVal(i, bufIn);
					for (long j = 0; j < iCols(); j++)
					{
						double rVal = bufIn[j];
						if (rVal == rUNDEF)
							bufOut[j] = shUNDEF;
						else
							bufOut[j] = shortConv(rVal * rFact);
					}
					filMpd->Write(2 * iCols(), bufOut.buf());
				}
			}
			else { // integer numbers
				RangeInt ri = ptr.riMinMax(BaseMapPtr::mmmCALCULATE);
				long iRange = ri.iHi() - ri.iLo()+1;
				if ((iRange > 256) || (ri.iHi() >= 256) || (ri.iLo() < 0)) { // use 1.41 int map
					mpi.iScale = 0; //shortConv(iRange / (long)0xFFFF); ?????????
					mpi.iMapType = 2;
					short iFact = 1;
					for (short i=0; i<mpi.iScale; i++)
						iFact *= 10;
					IntBuf bufOut(iCols());
					LongBuf bufIn(iCols());
					filMpd = new File(fnMPD, facCRT);
					filMpd->SetErase(true);
					for (short i=0; i<iLines(); i++)
					{
						GetLineVal(i, bufIn);
						for (short j=0; j < iCols(); j++)
							bufOut[j] = shortConv(bufIn[j] / iFact);
						filMpd->Write(2*iCols(), bufOut.buf());
					}
				}
				else { // 1.41 byte map
					ByteBuf bufOut(iCols());
					LongBuf bufIn(iCols());
					filMpd = new File(fnMPD, facCRT);
					filMpd->SetErase(true);
					for (short i=0; i<iLines(); i++) {
						GetLineVal(i, bufIn);
						for (short j=0; j < iCols(); j++)
							bufOut[j] = byteConv(bufIn[j]);
						filMpd->Write(iCols(), bufOut.buf());
					}
					CreateColFile(fn, dm()->rpr(), 0 != dm()->pdv());
				}
			}
		}
		else if (dm()->pdp()) {     // Picture?
			mpi.iScale = 0;
			mpi.iMapType = 1;
			File::Copy(mpsb->file->sName(), fnMPD);
			filMpd = new File(fnMPD, facRW);  // open file after copy to avoid sharing problems
			filMpd->SetErase(true);            // this is necessary to allow removal of file again in case of exceptions
			CreateColFile(fn, dm()->rpr(), 0 != dm()->pdv());
		}
		else if (dm()->pdsrt()) {   // ID or class?
			DomainSort* ds = dm()->pdsrt();
			if (ds->iSize() > 32000)
				throw ErrorExportDomain(dm(), errMapExport+1); // too many classes
			// create INF file
			File filInf(FileName(fn, ".INF"), facCRT);
			filInf.SetErase(true);
			filInf.WriteLnAscii("Name$ #1");
			for (long i=1; i <= ds->iSize(); i++)
			{
				String sLoc = ds->sValueByRaw(i, 0).sLeft(20); // think about 1.4 max string length
				for (unsigned short ii = 0; ii < sLoc.length(); ii++)
					if (sLoc[ii] == ' ') sLoc[ii] = '_';  // replace spaces with underscore characters
					filInf.WriteLnAscii(sLoc);
			}
			filInf.SetErase(false);
			if (ds->iSize() > 255)
				mpi.iMapType = 2;
			else {
				mpi.iMapType = 1;
				CreateColFile(fn, dm()->rpr(), 0 != dm()->pdv());
			}
			File::Copy(mpsb->file->sName(), fnMPD);
			filMpd = new File(fnMPD, facRW);  // open file after copy to avoid sharing problems
			filMpd->SetErase(true);            // this is necessary to allow removal of file again in case of exceptions
		}
		else
		{
			delete filMpd;
			filMpd = 0;
			throw ErrorExportDomain(dm(), errMapExport+2); // improper domain
		}
	}
		
	filMpi.Write(sizeof(MpiStruct), &mpi);
	filMpi.SetErase(false);
	if (filMpd)
	{
		filMpd->SetErase(false);
		delete filMpd;
	}
}

void MapStore::SetSize(RowCol rc)
{
  if (0 != mpsb)
    mpsb->rcSize = rc;
}

void MapStore::KeepOpen(bool f, bool force)
{
//  BaseMapPtr::KeepOpen(f);
  if (0 != mpsb)
    mpsb->KeepOpen(f, force);
}

void MapStoreBase::KeepOpen(bool f, bool force)
{
  if (file)
    file->KeepOpen(f, force);
}

void MapStoreBase::PutVal(RowCol rc, double val)
{
  if (dvs.fRawAvailable()) {
    long raw = dvs.iRaw(val);
    PutRaw(rc,raw);
  }
}

void MapStoreBase::PutVal(RowCol rc, const String& s)
{
  if (dvs.fRawAvailable()) {
    long raw = dvs.iRaw(s);
    PutRaw(rc,raw);
  }  
}
/*
void MapStore::PutRaw(const Coord& crd, long raw)
{
  RowCol rc = gr()->rcConv(crd);
  PutRaw(rc, raw);
}

void MapStore::PutVal(const Coord& crd, double val)
{
  RowCol rc = gr()->rcConv(crd);
  PutVal(rc, val);
}

void MapStore::PutVal(const Coord& crd, const String& s)
{
  RowCol rc = gr()->rcConv(crd);
  PutVal(rc, s);
}
*/
long MapStoreBase::iRaw(RowCol rc) const
{
  return iUNDEF;
}

long MapStoreBase::iValue(RowCol rc) const
{
  if (!dvs.fValues())
    return iUNDEF;
  long raw = iRaw(rc);
  return dvs.iValue(raw);
}

double MapStoreBase::rValue(RowCol rc) const
{
  if (!dvs.fValues())
    return rUNDEF;
  long raw = iRaw(rc);
  return dvs.rValue(raw);
}

void MapStore::FillWithUndef()
{
  // if made virtual no if-statement is needed
  // overrule in MapLineReal etc.
  if (dvrs().fUseReals()) {
    RealBuf bufUndef(1024);
    for (int c=0; c < 1024; c++)
      bufUndef[c] = rUNDEF;
    for (long l=0; l < iLines(); l++ ) {
      int j=0;
      for (; j < (iCols()-1) / 1024 ; j++)
        PutLineVal(l, bufUndef, j*1024L, 1024);
      PutLineVal(l, bufUndef, j*1024L, iCols() % 1024);
    }
  }
  else {
    LongBuf bufUndef(1024);
    for (int c=0; c < 1024; c++)
      bufUndef[c] = iUNDEF;
    for (long l=0; l < iLines(); l++ ) {
		int j=0;
      for (; j < (iCols()-1) / 1024 ; j++)
        PutLineRaw(l, bufUndef, j*1024L, 1024);
      PutLineRaw(l, bufUndef, j*1024L, iCols() % 1024);
    }
  }
}

void MapStoreBase::InitializePyramids(const FileName& fnPyr)
{
	filePyramid = new File(fnPyr);
	if ( filePyramid)
	{
		filePyramid->Seek(filePyramid->GetLength() - 4);
		long iLayers;
		filePyramid->Read(4, &iLayers);
		if ( iLayers > 0 )
		{
			bool f2GB = (file != 0) && (file->GetLength() > ((ULONGLONG)2 * 1024 * 1024 * 1024));
			if (f2GB) {
				filePyramid->Seek(filePyramid->GetLength() - iLayers * sizeof(ULONGLONG) - 4);
				iPyramidLayerOffset.resize(iLayers);
				for(int i=0; i < iLayers; ++i)
				{
					ULONGLONG iOffset;
					filePyramid->Read(sizeof(ULONGLONG), &iOffset);
					iPyramidLayerOffset[i] = iOffset;
				}
			} else {
				filePyramid->Seek(filePyramid->GetLength() - iLayers * sizeof(long) - 4);
				iPyramidLayerOffset.resize(iLayers);
				for(int i=0; i < iLayers; ++i)
				{
					long iOffset;
					filePyramid->Read(sizeof(long), &iOffset);
					if ( iOffset >= 0)
						iPyramidLayerOffset[i] = iOffset;
					else
					{
						filePyramid->SetErase();
						delete filePyramid;
						filePyramid = 0;
						throw ErrorObject("Pyramid file corrupt");
					}
				}
			}
		}		
	}	
}

void MapStore::GetDataFiles(Array<FileName>& afnDat, Array<String>* asSection, Array<String>* asEntry) const
{
//  IlwisObjectPtr::GetDataFiles(afnDat, asSection, asEntry);
  if (0 != mpsb) {
    if (0 == mpsb->file)
      return;
    ObjectInfo::Add(afnDat, FileName(mpsb->file->sName()), fnObj.sPath());
    if (asSection != 0) {
      (*asSection) &= "MapStore";
      (*asEntry) &= "Data";
    }
  }  
}

bool MapStore::fPatchByte(MapPtr* mpPatch, const String& sTitle, Tranquilizer& trq)
{
  trq.SetText(String(TR("Patching map '%S'").c_str(), ptr.sName(true, fnObj.sPath())));
  long iNrLines=iLines(), iNrCols=iCols();
  BytePatch aptCur[16];
  int i, j, iNrPt;
  int iHeight, iWidth;
  int iLin, iPos;
  long iX, iY;
  ByteBuf buf(1024);
  long iWhenDone = iNrLines;
  long iOffset = 0;
  iNrPt  = min(((iNrCols - iOffset-1) / PATCH_SIDE) + 1 , long(16));
  for (iY=0; iY < iNrLines; iY+=PATCH_SIDE) {
    iHeight = min(iNrLines - iY, long(PATCH_SIDE));
    iX = 0;
    do {
      iWidth  = min(iNrCols - iOffset, long(1024));
      iNrPt  = min(((iNrCols - iOffset - 1) / PATCH_SIDE) + 1 , long(16));
      for (iLin = 0; iLin<iHeight; iLin++) {
        GetLineRaw(iY + iLin, buf, iOffset, iWidth);
        iPos = 0;
        for (i=0; i<iNrPt; i++) {
          for (j=0; j<PATCH_SIDE; j++) {
            aptCur[i](iLin, j) = buf[iPos];
            iPos++;
          }
        }
        if (trq.fAborted())
          return false;
      }  // for iLin
      for (i=0; i<iNrPt; i++) {
        mpPatch->PutPatchRaw(RowCol(iY, iX), aptCur[i]);
        iX +=PATCH_SIDE;
      }
      iOffset += 1024;
    }
    while (iOffset < iNrCols);
    iOffset = 0;
    if (trq.fUpdate(iY, iWhenDone))
      return false;
  }   // for iY 
  trq.fUpdate(iWhenDone, iWhenDone);
  return true;
}

bool MapStore::fPatchInt(MapPtr *mpPatch, const String& sTitle, Tranquilizer& trq)
{
  trq.SetText(String(TR("Patching map '%S'").c_str(), ptr.sName(true, fnObj.sPath())));
  long iNrLines=iLines(), iNrCols=iCols();
  IntPatch aptCur[16];
  int i, j, iNrPt;
  int iHeight, iWidth;
  int iLin, iPos;
  long iX, iY;
  IntBuf buf(1024);
  long iWhenDone = iNrLines;
  long iOffset = 0;
  iNrPt  = min(((iNrCols - iOffset-1) / PATCH_SIDE) + 1 , long(16));
  for (iY=0; iY < iNrLines; iY+=PATCH_SIDE) {
    iHeight = min(iNrLines - iY, long(PATCH_SIDE));
    iX = 0;
    do {
      iWidth  = min(iNrCols - iOffset, long(1024));
      iNrPt  = min(((iNrCols - iOffset - 1) / PATCH_SIDE) + 1 , long(16));
      for (iLin = 0; iLin<iHeight; iLin++) {
        GetLineRaw(iY + iLin, buf, iOffset, iWidth);
        iPos = 0;
        for (i=0; i<iNrPt; i++) {
          for (j=0; j<PATCH_SIDE; j++) {
            aptCur[i](iLin, j) = buf[iPos];
            iPos++;
          }
        }
        if (trq.fAborted())
          return false;
      }  // for iLin
      for (i=0; i<iNrPt; i++) {
        mpPatch->PutPatchRaw(RowCol(iY, iX), aptCur[i]);
        iX +=PATCH_SIDE;
      }
      iOffset += 1024;
    }
    while (iOffset < iNrCols);
    iOffset = 0;
    if (trq.fUpdate(iY, iWhenDone))
      return false;
  }   // for iY 
  trq.fUpdate(iWhenDone, iWhenDone);
  return true;
}

bool MapStore::fPatchLong(MapPtr *mpPatch, const String& sTitle, Tranquilizer& trq)
{
  trq.SetText(String(TR("Patching map '%S'").c_str(), ptr.sName(true, fnObj.sPath())));
  long iNrLines=iLines(), iNrCols=iCols();
  LongPatch aptCur[16];
  int i, j, iNrPt;
  int iHeight, iWidth;
  int iLin, iPos;
  long iX, iY;
  LongBuf buf(1024);
  long iWhenDone = iNrLines;
  long iOffset = 0;
  iNrPt  = min(((iNrCols - iOffset-1) / PATCH_SIDE) + 1 , long(16));
  for (iY=0; iY < iNrLines; iY+=PATCH_SIDE) {
    iHeight = min(iNrLines - iY, long(PATCH_SIDE));
    iX = 0;
    do {
      iWidth  = min(iNrCols - iOffset, long(1024));
      iNrPt  = min(((iNrCols - iOffset - 1) / PATCH_SIDE) + 1 , long(16));
      for (iLin = 0; iLin<iHeight; iLin++) {
        GetLineRaw(iY + iLin, buf, iOffset, iWidth);
        iPos = 0;
        for (i=0; i<iNrPt; i++) {
          for (j=0; j<PATCH_SIDE; j++) {
            aptCur[i](iLin, j) = buf[iPos];
            iPos++;
          }
        }
        if (trq.fAborted())
          return false;
      }  // for iLin
      for (i=0; i<iNrPt; i++) {
        mpPatch->PutPatchRaw(RowCol(iY, iX), aptCur[i]);
        iX +=PATCH_SIDE;
      }
      iOffset += 1024;
    }
    while (iOffset < iNrCols);
    iOffset = 0;
    if (trq.fUpdate(iY, iWhenDone))
      return false;
  }   // for iY 
  trq.fUpdate(iWhenDone, iWhenDone);
  return true;
}

bool MapStore::fPatchReal(MapPtr *mpPatch, const String& sTitle, Tranquilizer& trq)
{
  trq.SetText(String(TR("Patching map '%S'").c_str(), ptr.sName(true, fnObj.sPath())));
  long iNrLines=iLines(), iNrCols=iCols();
  RealPatch aptCur[16];
  int i, j, iNrPt;
  int iHeight, iWidth;
  int iLin, iPos;
  long iX, iY;
  RealBuf buf(1024);
  long iWhenDone = iNrLines;
  long iOffset = 0;
  iNrPt  = min(((iNrCols - iOffset-1) / PATCH_SIDE) + 1 , long(16));
  for (iY=0; iY < iNrLines; iY+=PATCH_SIDE) {
    iHeight = min(iNrLines - iY, long(PATCH_SIDE));
    iX = 0;
    do {
      iWidth  = min(iNrCols - iOffset, long(1024));
      iNrPt  = min(((iNrCols - iOffset - 1) / PATCH_SIDE) + 1 , long(16));
      for (iLin = 0; iLin<iHeight; iLin++) {
        GetLineVal(iY + iLin, buf, iOffset, iWidth);
        iPos = 0;
        for (i=0; i<iNrPt; i++) {
          for (j=0; j<PATCH_SIDE; j++) {
            aptCur[i](iLin, j) = buf[iPos];
            iPos++;
          }
        }
        if (trq.fAborted())
          return false;
      }  // for iLin
      for (i=0; i<iNrPt; i++) {
        mpPatch->PutPatchVal(RowCol(iY, iX), aptCur[i]);
        iX +=PATCH_SIDE;
      }
      iOffset += 1024;
    }
    while (iOffset < iNrCols);
    iOffset = 0;
    if (trq.fUpdate(iY, iWhenDone))
      return false;
  }   // for iY 
  trq.fUpdate(iWhenDone, iWhenDone);
  return true;
}

bool MapStore::fUnPatchByte(MapPtr *mpPatch, const String& sTitle, Tranquilizer& trq)
{
  trq.SetText(String(TR("Unpatching map '%S'").c_str(), ptr.sName(true, fnObj.sPath())));
  long iNrLines=iLines(), iNrCols=iCols();
  BytePatch aptCur[16];
  int i, j, iNrPt;
  int iHeight, iWidth;
  int iLin, iPos;
  long iX, iY;
  ByteBuf buf(1024);
  long iProgress = 0;
  long iWhenDone = iNrLines * ((iNrCols - 1) / 1024 + 1);
  long iOffset = 0;
  iNrPt  = min(((iNrCols - iOffset) / PATCH_SIDE) + 1 , long(16));
  for (iY=0; iY < iNrLines; iY+=PATCH_SIDE) {
    iHeight = min(iNrLines - iY, long(PATCH_SIDE));
    iX = 0;
    do {
      iWidth  = min(iNrCols - iOffset, long(1024));
      iNrPt  = min(((iNrCols - iOffset) / PATCH_SIDE) + 1 , long(16));
      for (i=0; i<iNrPt; i++) {
        mpPatch->GetPatchRaw(RowCol(iY, iX), aptCur[i]);
        iX +=PATCH_SIDE;
      }
      for (iLin = 0; iLin<iHeight; iLin++) {
        iPos = 0;
        for (i=0; i<iNrPt; i++) {
          for (j=0; j<PATCH_SIDE; j++) {
            buf[iPos] = aptCur[i](iLin, j);
            iPos++;
          }
        }
        PutLineRaw(iY + iLin, buf, iOffset, iWidth);
        if (trq.fUpdate(iProgress, iWhenDone))
          return false;
        iProgress++;
      }  // for iLin
      iOffset += 1024;
    }
    while (iOffset < iNrCols);
    iOffset = 0;
  }   // for iY
  trq.fUpdate(iWhenDone, iWhenDone);
  return true;
}

bool MapStore::fUnPatchInt(MapPtr *mpPatch, const String& sTitle, Tranquilizer& trq)
{
  trq.SetText(String(TR("Unpatching map '%S'").c_str(), ptr.sName(true, fnObj.sPath())));
  long iNrLines=iLines(), iNrCols=iCols();
  IntPatch aptCur[16];
  int i, j, iNrPt;
  int iHeight, iWidth;
  int iLin, iPos;
  long iX, iY;
  IntBuf buf(1024);
  long iProgress = 0;
  long iWhenDone = iNrLines * ((iNrCols - 1) / 1024 + 1);
  long iOffset = 0;
  iNrPt  = min(((iNrCols - iOffset) / PATCH_SIDE) + 1 , long(16));
  for (iY=0; iY < iNrLines; iY+=PATCH_SIDE) {
    iHeight = min(iNrLines - iY, long(PATCH_SIDE));
    iX = 0;
    do {
      iWidth  = min(iNrCols - iOffset, long(1024));
      iNrPt  = min(((iNrCols - iOffset) / PATCH_SIDE) + 1 , long(16));
      for (i=0; i<iNrPt; i++) {
        mpPatch->GetPatchRaw(RowCol(iY, iX), aptCur[i]);
        iX +=PATCH_SIDE;
      }
      for (iLin = 0; iLin<iHeight; iLin++) {
        iPos = 0;
        for (i=0; i<iNrPt; i++) {
          for (j=0; j<PATCH_SIDE; j++) {
            buf[iPos] = aptCur[i](iLin, j);
            iPos++;
          }
        }
        PutLineRaw(iY + iLin, buf, iOffset, iWidth);
        if (trq.fUpdate(iProgress, iWhenDone))
          return false;
        iProgress++;
      }  // for iLin
      iOffset += 1024;
    }
    while (iOffset < iNrCols);
    iOffset = 0;
  }   // for iY
  trq.fUpdate(iWhenDone, iWhenDone);
  return true;
}  

bool MapStore::fUnPatchLong(MapPtr *mpPatch, const String& sTitle, Tranquilizer& trq)
{
  trq.SetText(String(TR("Unpatching map '%S'").c_str(), ptr.sName(true, fnObj.sPath())));
  long iNrLines=iLines(), iNrCols=iCols();
  LongPatch aptCur[16];
  int i, j, iNrPt;
  int iHeight, iWidth;
  int iLin, iPos;
  long iX, iY;
  LongBuf buf(1024);
  long iProgress = 0;
  long iWhenDone = (iNrLines * iNrCols - 1) / 1024 + 1;
  long iOffset = 0;
  iNrPt  = min(((iNrCols - iOffset-1) / PATCH_SIDE) + 1 , long(16));
  for (iY=0; iY < iNrLines; iY+=PATCH_SIDE) {
    iHeight = min(iNrLines - iY, long(PATCH_SIDE));
    iX = 0;
    do {
      iWidth  = min(iNrCols - iOffset, long(1024));
      iNrPt  = min(((iNrCols - iOffset - 1) / PATCH_SIDE) + 1 , long(16));
      for (i=0; i<iNrPt; i++) {
        mpPatch->GetPatchRaw(RowCol(iY, iX), aptCur[i]);
        iX +=PATCH_SIDE;
      }
      for (iLin = 0; iLin<iHeight; iLin++) {
        iPos = 0;
        for (i=0; i<iNrPt; i++) {
          for (j=0; j<PATCH_SIDE; j++) {
            buf[iPos] = aptCur[i](iLin, j);
            iPos++;
          }
        }
        PutLineRaw(iY + iLin, buf, iOffset, iWidth);
        if (trq.fUpdate(iProgress, iWhenDone))
          return false;
        iProgress++;
      }  // for iLin
      iOffset += 1024;
    }
    while (iOffset < iNrCols);
    iOffset = 0;
  }   // for iY 
  trq.fUpdate(iWhenDone, iWhenDone);
  return true;
}

bool MapStore::fUnPatchReal(MapPtr *mpPatch, const String& sTitle, Tranquilizer& trq)
{
  trq.SetText(String(TR("Unpatching map '%S'").c_str(), ptr.sName(true, fnObj.sPath())));
  long iNrLines=iLines(), iNrCols=iCols();
  RealPatch aptCur[16];
  int i, j, iNrPt;
  int iHeight, iWidth;
  int iLin, iPos;
  long iX, iY;
  RealBuf buf(1024);
  long iProgress = 0;
  long iWhenDone = iNrLines * ((iNrCols - 1) / 1024 + 1);
  long iOffset = 0;
  iNrPt  = min(((iNrCols - iOffset - 1) / PATCH_SIDE) + 1 , long(16));
  for (iY=0; iY < iNrLines; iY+=PATCH_SIDE) {
    iHeight = min(iNrLines - iY, long(PATCH_SIDE));
    iX = 0;
    do {
      iWidth  = min(iNrCols - iOffset, long(1024));
      iNrPt  = min(((iNrCols - iOffset - 1) / PATCH_SIDE) + 1 , long(16));
      for (i=0; i<iNrPt; i++) {
        mpPatch->GetPatchVal(RowCol(iY, iX), aptCur[i]);
        iX +=PATCH_SIDE;
      }
      for (iLin = 0; iLin<iHeight; iLin++) {
        iPos = 0;
        for (i=0; i<iNrPt; i++) {
          for (j=0; j<PATCH_SIDE; j++) {
            buf[iPos] = aptCur[i](iLin, j);
            iPos++;
          }
        }
        PutLineVal(iY + iLin, buf, iOffset, iWidth);
        if (trq.fUpdate(iProgress, iWhenDone))
          return false;
        iProgress++;
      }  // for iLin
      iOffset += 1024;
    }
    while (iOffset < iNrCols);
    iOffset = 0;
  }   // for iY  return true;
  trq.fUpdate(iWhenDone, iWhenDone);
  return true;
}

bool MapStore::fConvertTo(const DomainValueRangeStruct& _dvrsTo, const Column& col)
{
  DomainValueRangeStruct dvrsTo = _dvrsTo;
  if (col.fValid())
    dvrsTo = col->dvrs();
  Tranquilizer trq;
  trq.SetTitle(TR("Domain Conversion"));
  trq.SetText(TR("Converting"));
  if (0 == mpsb->file)
    return false;
  String sDatOld = mpsb->file->sName();
  FileName fnDatNew = FileName::fnUnique(sDatOld);
  MapStoreBase* mpsbNew = 0;
  if (dvrs().fValues()) {
    trq.Start();
    if (dvrsTo.fValues() && !col.fValid()) {
      mpsbNew = MapStoreBase::create(fnDatNew, *this, rcSize(), dvrsTo);
      // read real values and write real values
      RealBuf rBuf(iCols());
      for (long i=0; i < iLines(); i++) {
        if (trq.fUpdate(i, iLines()))
          return false;
        GetLineVal(i, rBuf, 0, iCols());
        mpsbNew->PutLineVal(i, rBuf, 0, iCols());
      }   
      trq.fUpdate(iLines(), iLines());
    }
    else if (col.fValid() && (dvrs().rStep() == 1)) {
      mpsbNew = MapStoreBase::create(fnDatNew, *this, rcSize(), dvrsTo);
      // read long values and use them as record nr. in column col
      LongBuf iBuf(iCols());
      RealBuf rBuf(iCols());
      for (long i=0; i < iLines(); i++) {
        if (trq.fUpdate(i, iLines()))
          return false;
        GetLineVal(i, iBuf, 0, iCols());
        if (dvrsTo.fUseReals()) {
          for (long j=0; j < iCols(); j++) 
            rBuf[j] = col->rValue(iBuf[j]);
          mpsbNew->PutLineVal(i, rBuf, 0, iCols());
        }
        else {
          for (long j=0; j < iCols(); j++) 
            iBuf[j] = col->iRaw(iBuf[j]);
          mpsbNew->PutLineRaw(i, iBuf, 0, iCols());
        } 
      }   
      trq.fUpdate(iLines(), iLines());
    }  
  }
  else if (col.fValid()) {
    mpsbNew = MapStoreBase::create(fnDatNew, *this, rcSize(), dvrsTo);
    trq.Start();
    // read long raws and use them as record nr. in column col
    LongBuf iBuf(iCols());
    RealBuf rBuf(iCols());
    for (long i=0; i < iLines(); i++) {
      if (trq.fUpdate(i, iLines()))
        return false;
      GetLineRaw(i, iBuf, 0, iCols());
      if (dvrsTo.fUseReals()) {
        for (long j=0; j < iCols(); j++) 
          rBuf[j] = col->rValue(iBuf[j]);
        mpsbNew->PutLineVal(i, rBuf, 0, iCols());
      }
      else {
        for (long j=0; j < iCols(); j++) 
          iBuf[j] = col->iRaw(iBuf[j]);
        mpsbNew->PutLineRaw(i, iBuf, 0, iCols());
      }  
    } 
    trq.fUpdate(iLines(), iLines());
  }
  if (0 != mpsbNew) {
    mpsb->file->SetErase(true);
    delete mpsb;
    mpsb = mpsbNew;
    // mpsb has new name and needs old:
    mpsb->file->Rename(sDatOld);
  }  
//  ptr.SetDomainValueRangeStruct(dvrsTo);
  return true;
}


void MapStore::Rename(const FileName& fnNew)
{
  // rename data file
  if (0 == mpsb) 
    return;
  if (0 == mpsb->file)
    return;
  FileName fn = mpsb->file->sName();;
  fn.sFile = fnNew.sFile;
  mpsb->file->Rename(fn.sFullName());
}

void MapStore::Flush()
{
  if (0 != mpsb)
    if (0 != mpsb->file)
      mpsb->file->Flush();
}

void MapStore::GetObjectStructure(ObjectStructure& os)
{
}

void MapStore::StoreAs(const String& sExpression)
{
	Array<String> arsParts;
	if (!ptr.gr()->fNorthOriented() && !ptr.gr()->fGeoRefNone())
	{
		throw ErrorObject(TR("The GeoReference should be North oriented."));
	}		
	Split(sExpression, arsParts, "(,)");
	ParmList pm;
	if ( fCIStrEqual(arsParts[0], "foreignformat"))
	{
		String sMethod = arsParts[1];
		pm.Add(new Parm("export", true));
		pm.Add(new Parm("method", arsParts[1]));
		pm.Add(new Parm("input", fnMap().sFullPath()));
		pm.Add(new Parm("output",arsParts[2]));
		pm.Add(new Parm("format", arsParts[3].sTrimSpaces()));

		FileName fnNew(arsParts[2]);
		if ( fnNew.fExist())	
		{
			int iRet = MessageBox(::AfxGetApp()->GetMainWnd()->m_hWnd, 
  									String(TR("File %S already exists, overwrite ?").c_str(), fnNew.sRelative().c_str()).c_str(),
										"Dummy", MB_YESNO | MB_ICONQUESTION );
				if ( iRet == IDYES)
					DeleteFile(fnNew.sFullPath().c_str());
				else 
					return;
		}
		FileName fnGeoRef = gr()->fnObj;
					
		ForeignFormat *ff = ForeignFormat::Create(fnNew, pm);
	  Tranquilizer trq;
	  trq.SetTitle(TR("Converting"));
	  trq.SetText(String(TR("Converting %S").c_str(), ptr.fnObj.sRelative(false)));
		
	  if (dvrs().fValues()) 
		{
	    trq.Start();
	    RealBuf rBuf(iCols());
	    LongBuf iBuf(iCols());			
	    for (long i=0; i < iLines(); i++) 
			{
				if (trq.fUpdate(i, iLines()))
					return;
	      if ( dvrs().fRealValues() ) 
				{
					GetLineVal(i, rBuf, 0, iCols());
	        ff->PutLineVal(ptr.fnObj, i, rBuf, 0, iCols());
	      }
	      else 
				{
					GetLineVal(i, iBuf, 0, iCols());
	        ff->PutLineVal(ptr.fnObj, i, iBuf, 0, iCols());
	      } 
	    }   
	    trq.fUpdate(iLines(), iLines());
		}		
	  else 
		{
	    LongBuf iBuf(iCols());
	    RealBuf rBuf(iCols());
	    for (long i=0; i < iLines(); i++) 
			{
	      if (trq.fUpdate(i, iLines()))
	        return ;
	      if (dvrs().fUseReals()) 
				{
					GetLineVal(i, rBuf, 0, iCols());
	        ff->PutLineVal(ptr.fnObj, i, rBuf, 0, iCols());
	      }
	      else 
				{
					GetLineRaw(i, iBuf, 0, iCols());
	        ff->PutLineRaw(ptr.fnObj, i, iBuf, 0, iCols());
	      } 
	    }   
	    trq.fUpdate(iLines(), iLines());
	  }
		ff->SetForeignGeoTransformations(ptr.cs(), ptr.gr());
		delete ff;		
  }
}

void MapStore::CreatePyramidLayers()
{ 
	if ( mpsb )
	{
		FileName fnNew = mpsb->file ? mpsb->file->GetFilePath() : fnObj;		
		mpsb->CreatePyramidLayers(fnNew);
		if ( mpsb->filePyramid != 0)
			ptr.fChanged = true; // do not call Updated(), that will update the time
	}		
}

void MapStoreBase::CreatePyramidLayers(const FileName& fn)
{
	FileName fnNew = fn;
	MutexFileName mutDat(fn);
	if ( dm()->dmt() == dmtBIT ) // no pyramids for domain bit
		return;
	fnNew.sExt = ".mpy#";
	if ( filePyramid != 0 )
	{
		filePyramid->SetErase();
		delete filePyramid;
		filePyramid = 0;
	}		
	filePyramid = new File(fnNew.sFullPath().c_str(), facCRT);
	MutexFileName mutD2(fnNew);	
	long iPyrLayer = 0;
	int iDiv = (int)pow(2, (double)iPyrLayer);
	ULONGLONG iLastFilePos = 0;
	Tranquilizer trq;
	trq.SetTitle(TR("Creating Pyramid layer"));
	trq.Start();
	trq.SetOnlyGauge(true);
	iPyramidLayerOffset.resize(0);
	while( iLines() / iDiv > 32 && iCols() / iDiv > 32 )
	{
		String sMessage(TR("Creating layer %d x %d for map %S").c_str(), iLines()/(iDiv*2), iCols()/(iDiv*2), fnNew.sRelative(false));
		if ( trq.fText(sMessage))
		{
			filePyramid->SetErase();
			delete filePyramid;
		}			
		iPyramidLayerOffset.push_back(iLastFilePos);		
		IterateCreatePyramidLayer(iPyrLayer,iLastFilePos, &trq);
		if ( filePyramid == 0) //  operation is aborted
			return;
		iDiv = (int)pow(2, (double)++iPyrLayer);
	}
	bool f2GB = (file != 0) && (file->GetLength() > ((ULONGLONG)2 * 1024 * 1024 * 1024));
	if (f2GB) {
		for (int i=0; i < iPyrLayer ; ++i)
			filePyramid->Write(sizeof(ULONGLONG), &iPyramidLayerOffset[i]);
	} else {
		for (int i=0; i < iPyrLayer ; ++i) {
			long iOffset = (long)(iPyramidLayerOffset[i]);
			filePyramid->Write(sizeof(long), &iOffset);
		}
	}

	filePyramid->Write(sizeof(long), &iPyrLayer);	
//	filePyramid->Close();
}

bool MapStore::fHasPyramidFile()
{
	if (mpsb )
		return mpsb->fHasPyramidFile();

	return false;
}

bool MapStoreBase::fHasPyramidFile()
{
	return filePyramid != 0;
}

void MapStore::DeletePyramidFile()
{
	if ( mpsb )
		mpsb->DeletePyramidFile();

	ObjectInfo::RemoveKey(ptr.fnObj, "MapStore", "PyramidFile");
}

void MapStoreBase::DeletePyramidFile()
{
	try
	{
		if ( filePyramid )
		{
			filePyramid->SetErase();
	//		filePyramid->Abort();
			delete filePyramid;
			filePyramid = 0;
		}	
	}
	catch (CFileException *ex) 
	{
		filePyramid->Abort();
		filePyramid = 0;
		ex->Delete();
	}	
}

int MapStore::iNoOfPyramidLayers()
{
	if ( mpsb )
		return mpsb->iNoOfPyramidLayers();

	return 0;		
}

int MapStoreBase::iNoOfPyramidLayers()
{
	return iPyramidLayerOffset.size();
}
