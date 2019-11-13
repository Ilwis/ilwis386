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
#pragma warning( disable : 4503 )
#pragma warning( disable : 4786 )

#include "Headers\toolspch.h"
#include "Engine\Domain\dm.h"
#include "Engine\Domain\dminfo.h"
#include "Engine\SpatialReference\Gr.h"
#include "Engine\Base\DataObjects\ERR.H"
#include "Engine\Table\Col.h"
#include "Engine\Map\Raster\Map.h"
#include "Engine\Map\Raster\MapList\maplist.h"
#include "Engine\Map\Segment\Seg.h"
#include "Engine\Map\Polygon\POL.H"
#include "Engine\Map\Point\PNT.H"
#include "Engine\Table\tbl.h"
#include "Engine\Function\FILTER.H"
#include "Engine\Function\CLASSIFR.H"
#include "Engine\SampleSet\SAMPLSET.H"
#include "Engine\Base\DataObjects\Tranq.h"
#include "Engine\Base\strtod.h"
#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\Engine.h"
#include "Headers\Hs\DAT.hs"
#include "Engine\Base\File\ElementMap.h"
#include <sys/stat.h>
#include <direct.h>

int ObjectInfo::ReadElement(const char* sSection, const char* sEntry,
							const FileName& filename, String& sValue)
{
	char sBuf[32000];
	if (sSection == 0 || *sSection == 0)
		return 0;
	if (sEntry && *sEntry == 0)
		return 0;
	const ElementContainer* ec = dynamic_cast<const ElementContainer*>(&filename);
	if (ec && 0 != ec->em) {
		ElementMap& em = const_cast<ElementMap&>(*ec->em);
		sValue = em(sSection, sEntry);
		return sValue.length();
	}
	if (!filename.fValid())
		return 0;
	if ( IlwisObject::iotObjectType( filename) == IlwisObject::iotANY )
		return 0;
	String s = filename.sPhysicalPath();
	const char* sFileName = s.c_str();
	int iRet;
	if (filename.sSectionPostFix.length() == 0)
		iRet = GetPrivateProfileString(sSection, sEntry, sValue.c_str(),
		sBuf, 32000, sFileName);
	else {
		String sSec(const_cast<char*>(sSection));
		sSec &= filename.sSectionPostFix;
		iRet = GetPrivateProfileString(sSec.c_str(), sEntry, sValue.c_str(),
			sBuf, 32000, sFileName);
	}
	sValue = String(sBuf);
	return iRet;
}

int ObjectInfo::ReadElement(const char* sSection, const char* sEntry,
							const FileName& filename, int& iValue)
{
	String sValue;
	int iRet = ReadElement(sSection, sEntry, filename, sValue);
	if (iRet) 
		iValue = sValue.iVal();
	else
		iValue = shUNDEF;
	return iRet;  
}                         

int ObjectInfo::ReadElement(const char* sSection, const char* sEntry,
							const FileName& filename, long& iValue)
{
	String sValue;
	int iRet = ReadElement(sSection, sEntry, filename, sValue);
	if (iRet) 
		iValue = sValue.iVal();
	else
		iValue = iUNDEF;
	return iRet;  
}                         

int ObjectInfo::ReadElement(const char* sSection, const char* sEntry,
							const FileName& filename, double& rValue)
{
	String sValue;
	int iRet = ReadElement(sSection, sEntry, filename, sValue);
	if (iRet) 
		rValue = sValue.rVal();
	else
		rValue = rUNDEF;
	return iRet;  
}                         

int ObjectInfo::ReadElement(const char* sSection, const char* sEntry,
							const FileName& filename, bool& fValue)
{
	String s;
	int iRet = ReadElement(sSection, sEntry, filename, s);
	if (iRet) 
		fValue = fCIStrEqual(s, "yes")  || fCIStrEqual(s, "true");
	else
		fValue = false;
	return iRet;  
}                         

int ObjectInfo::ReadElement(const char* sSection, const char* sEntry, const FileName& fnObj,
							ILWIS::TimeInterval& interval)
{
	String sValue;
	int iRet = ReadElement(sSection, sEntry, fnObj, sValue);	
	Array<String> parts;
	Split(sValue, parts,"::");
	interval = ILWIS::TimeInterval(parts[0].rVal(), parts[1].rVal(), parts[2].rVal());

	return iRet;
}

int ObjectInfo::ReadElement(const char* sSection, const char* sEntry,
							const FileName& fnObj, FileName& fn)
{
	String sNam;
	int iRet = ReadElement(sSection, sEntry, fnObj, sNam);
	if (iRet != 0)
	{
		FileName filnam(sNam, fnObj);
		FileName fnRead = filnam;
		if ((filnam.sExt.length() != 0) && (sNam.strchrQuoted(':') == 0))
		{
			// first check in system directory
			String sysDir = getEngine()->getContext()->sStdDir();
			filnam.Dir(sysDir);
			if (!File::fExist(filnam)){
				filnam.Dir(sysDir + "\\Basemaps");
				if (!File::fExist(filnam))
					filnam = fnRead; 
			}// restore actual filename read from ODF
		}
		fn = filnam;
	}
	else
		fn = FileName();
	return iRet;
}

int ObjectInfo::ReadElement(const char* sSection, const char* sEntry,
							const FileName& fnObj, RangeInt& rng)
{
	String s;
	int iRet = ReadElement(sSection, sEntry, fnObj, s);
	if (iRet > 0)
		rng = RangeInt(s);
	else
		rng = RangeInt();
	return iRet;
}

int ObjectInfo::ReadElement(const char* sSection, const char* sEntry,
							const FileName& fnObj, RangeReal& rng)
{
	String s;
	int iRet = ReadElement(sSection, sEntry, fnObj, s);
	if (iRet > 0)
		rng = RangeReal(s);
	else
		rng = RangeReal();
	return iRet;
}

int ObjectInfo::ReadElement(const char* sSection, const char* sEntry,
							const FileName& fnObj, ObjectTime& tim)
{
	String sValue;
	int iRet = ReadElement(sSection, sEntry, fnObj, sValue);
	long l =sValue.iVal();
	if (l != iUNDEF)
		tim = ObjectTime(l);
	else
		tim = ObjectTime(0);
	return iRet;
}

int ObjectInfo::ReadElement(const char* sSection, const char* sEntry,
							const FileName& fnObj, RealArray& arr)
{
	int iRet;
	String s;
	iRet = ReadElement(sSection, sEntry, fnObj, s);
	if (iRet == 0)
		return iRet;
	char *p = s.sVal();
	p = strtok(s.sVal(), " ");
	long iSize = atol(p);
	p = strtok(0, " ");
	long iOffset = atol(p);
	p = strtok(0, " ");
	//  arr.Resize(iSize, iOffset);
	arr.Resize(iSize+iOffset); // arrays now are zero based
	for (int j=iOffset; j < iOffset+iSize; j++ ) {
		arr[j] = atofILW(p);
		p = strtok(0, " ");
	}
	return iRet;
}

int ObjectInfo::ReadElement(const char* sSection, const char* sEntry,
							const FileName& fnObj, RealMatrix& mat)
{
	int iRet;
	String sEnt = sEntry;
	sEnt &= "_Size";
	String sSize;
	iRet = ReadElement(sSection, sEnt.c_str(), fnObj, sSize);
	if (iRet == 0)
		return iRet;
	int iRows, iCols;
	sscanf(sSize.c_str(), "%i %i", &iRows, &iCols);
	if (iRows <= 0 || iCols <= 0) 
	{
		mat = RealMatrix();
		return 0;
	}
	mat = RealMatrix(iRows, iCols);
	for (int i=0; i < mat.iRows(); i++) {
		sEnt = sEntry;
		sEnt &= String("_Row%i", i);
		String sRow;
		iRet += ReadElement(sSection, sEnt.c_str(), fnObj, sRow);
		char * p = strtok(sRow.sVal(), " ");
		for (int j=0; j < mat.iCols(); j++ ) {
			mat(i, j) = atofILW(p);
			p = strtok(0, " ");
		}
	}
	return iRet;
}

int ObjectInfo::ReadElement(const char* sSection, const char* sEntry,
							const FileName& fnObj, IntMatrix& mat)
{
	int iRet;
	String sEnt = sEntry;
	sEnt &= "_Size";
	String sSize;
	iRet = ReadElement(sSection, sEnt.c_str(), fnObj, sSize);
	int iRows, iCols;
	sscanf(sSize.c_str(), "%i %i", &iRows, &iCols);
	mat = IntMatrix(iRows, iCols);
	for (int i=0; i < mat.iRows(); i++) {
		sEnt = sEntry;
		sEnt &= String("_Row%i", i);
		String sRow;
		iRet += ReadElement(sSection, sEnt.c_str(), fnObj, sRow);
		char * p = strtok(sRow.sVal(), " ");
		for (int j=0; j < mat.iCols(); j++ ) {
			mat(i, j) = atoi(p);
			p = strtok(0, " ");
		}
	}
	return iRet;
}

int ObjectInfo::ReadElement(const char* sSection, const char* sEntry,
							const FileName& fnObj, Coord& crd)
{
	int iRet;
	String s;
	iRet = ReadElement(sSection, sEntry, fnObj, s);
	int n = sscanf(s.sVal(), "%lg %lg %lg", &crd.x, &crd.y, &crd.z);
	if ( n == 2)
		crd.z =  rUNDEF;

	return iRet;
}

int ObjectInfo::ReadElement(const char* sSection, const char* sEntry,
							const FileName& fnObj, LatLon& ll)
{
	int iRet;
	String s;
	iRet = ReadElement(sSection, sEntry, fnObj, s);
	sscanf(s.sVal(), "%lg %lg", &ll.Lat, &ll.Lon);
	return iRet;
}

int ObjectInfo::ReadElement(const char* sSection, const char* sEntry,
							const FileName& fnObj, RowCol& rc)
{
	int iRet;
	String s;
	iRet = ReadElement(sSection, sEntry, fnObj, s);
	sscanf(s.sVal(), "%li %li", &rc.Row, &rc.Col);
	return iRet;
}

int ObjectInfo::ReadElement(const char* sSection, const char* sEntry,
							const FileName& fnObj, MinMax& mm)
{
	int iRet;
	String s;
	iRet = ReadElement(sSection, sEntry, fnObj, s);
	sscanf(s.sVal(), "%li %li %li %li",
		&mm.rcMin.Row, &mm.rcMin.Col,
		&mm.rcMax.Row, &mm.rcMax.Col);
	return iRet;
}

int ObjectInfo::ReadElement(const char* sSection, const char* sEntry,
							const FileName& fnObj, CSize& sz)
{
	int iRet;
	String s;
	iRet = ReadElement(sSection, sEntry, fnObj, s);
	sscanf(s.sVal(), "%li %li", &sz.cx, &sz.cy);
	return iRet;
}

int ObjectInfo::ReadElement(const char* sSection, const char* sEntry,
							const FileName& fnObj, CRect& rect)
{
	int iRet;
	String s;
	iRet = ReadElement(sSection, sEntry, fnObj, s);
	sscanf(s.sVal(), "%li %li %li %li",
		&rect.top, &rect.left, 
		&rect.bottom, &rect.right);
	return iRet;
}

int ObjectInfo::ReadElement(const char* sSection, const char* sEntry,
							const FileName& fnObj, CoordBounds& cb)
{
	int iRet;
	String s;
	iRet = ReadElement(sSection, sEntry, fnObj, s);
	sscanf(s.sVal(), "%lg %lg %lg %lg",
		&cb.cMin.x, &cb.cMin.y,
		&cb.cMax.x, &cb.cMax.y);
	return iRet;
}

int ObjectInfo::ReadElement(const char* sSection, const char* sEntry,
							const FileName& fnObj, GeoRef& gr)
{
	String sGr;
	int iRet = ReadElement(sSection, sEntry, fnObj, sGr);
	if (iRet)
	{
		try
		{
			// There are three input possibilities:
			// 1. Regular GeoRef with extension
			// 2. Regular GeoRef without extension
			// 3. Internal GeoRef
			// 3. is tried first, if it fails 1. and 2. are combined by setting the extension and tried
			FileName fnGrf(sGr, fnObj);
			// try to open internal GeoRef
			try
			{
				gr = GeoRef(sGr, fnObj.sPath());
				return iRet;
			}
			catch (...)
			{
			}
			// It is no internal georef, add extension and try to open as regular georef
			fnGrf.sExt = ".grf";
			if (!File::fExist(fnGrf))  { // try in current directory
				fnGrf.Dir(getEngine()->getContext()->sStdDir()); // try in system directory
				if (!File::fExist(fnGrf))
					fnGrf.Dir(getEngine()->getContext()->sStdDir() + "\\Basemaps"); // try in system\basemaps directory
			}
			if (File::fExist(fnGrf))
				gr = GeoRef(fnGrf);
			else 
			{
				bool *fDoNotShowError = (bool *)(getEngine()->pGetThreadLocalVar(IlwisAppContext::tlvDONOTSHOWFINDERROR));
				if ( fDoNotShowError == 0 || *fDoNotShowError == false)
					ErrorNotFound(FileName(sGr, fnObj)).Show();
				gr = GeoRef(FileName("none.grf"));
			}  
		}
		catch (const ErrorObject& err) 
		{
			bool *fDoNotShowError = (bool *)(getEngine()->pGetThreadLocalVar(IlwisAppContext::tlvDONOTSHOWFINDERROR));
			gr = GeoRef();
			if ( fDoNotShowError == 0 || *fDoNotShowError == false)
				throw err;
		}
	}
	else
		gr = GeoRef();
	return iRet;
}

int ObjectInfo::ReadElement(const char* sSection, const char* sEntry,
							const FileName& fnObj, Domain& dm)
{
	String sDom;
	const ElementContainer* ec = dynamic_cast<const ElementContainer*>(&fnObj);
	if ((0 == ec || 0 == ec->em) && !fnObj.fExist())
	{
		DomainInfo dinf(fnObj, sSection);
		dm = dinf.dmUnknown();		
		return 0;
	}		
	int iRet = ReadElement(sSection, sEntry, fnObj, sDom);
	bool *fDoNotShowError = (bool *)(getEngine()->pGetThreadLocalVar(IlwisAppContext::tlvDONOTSHOWFINDERROR));
	if (iRet) {
		FileName fnDom(sDom, fnObj);
		try {
			if (fCIStrEqual(fnDom.sExt, ".DOM")) {
				if (!File::fExist(fnDom)) {
					fnDom.Dir(getEngine()->getContext()->sStdDir());
					if (!File::fExist(fnDom))
						fnDom.Dir(getEngine()->getContext()->sStdDir() + "\\Basemaps");
				}
				if (File::fExist(fnDom))
					dm = Domain(fnDom);
				else {
					if (fCIStrEqual(fnDom.sFile.c_str() , "int") ||
						fCIStrEqual(fnDom.sFile.c_str() , "long")  ||
						fCIStrEqual(fnDom.sFile.c_str() , "real")) {
							dm = Domain("value.dom");
					}
					else {
						ErrorObject err = ErrorNotFound(WhereError(FileName(sDom, fnObj), "Domain"));
						err.SetTitle(fnObj.sFullName());
						DomainInfo dinf(fnObj, sSection);
						dm = Domain();
						if ( fDoNotShowError == 0 || *fDoNotShowError == false)
							throw err;
					}
				}
			}
			else
				dm = Domain(sDom, fnObj.sPath());
		}
		catch (const ErrorObject& err) 
		{
			DomainInfo dinf(fnObj, sSection);
			dm = Domain();
			throw err;
		}
	}
	else 
	{
		//throw ErrorObject(WhatError(String(TR("Empty or no '%s=' entry found in section '%s'").c_str(), sEntry, sSection), errOPEN), fnObj); //.Show();
		DomainInfo dinf(fnObj, sSection);
		dm = dinf.dmUnknown();
	}  
	return iRet;
}

int ObjectInfo::ReadElement(const char* sSection, const char* sEntry,
							const FileName& fnObj, Representation& rpr)
{
	String sRpr;
	int iRet = ReadElement(sSection, sEntry, fnObj, sRpr);
	if (iRet) {
		try {
			FileName fnRpr(sRpr, fnObj);
			if (fCIStrEqual(fnRpr.sExt , ".RPR")) {
				if (!File::fExist(fnRpr)) {
					fnRpr.Dir(getEngine()->getContext()->sStdDir());
					if (!File::fExist(fnRpr))
						fnRpr.Dir(getEngine()->getContext()->sStdDir() + "\\Basemaps");
					if (!File::fExist(fnRpr))
						fnRpr = FileName(sRpr, fnObj); // restore actual filename read from ODF (for error reporting)
				}
				rpr = Representation(fnRpr);
			}
			else
				rpr = Representation(sRpr);
		}
		catch (const ErrorObject& err) 
		{
			bool *fDoNotShowError = (bool *)(getEngine()->pGetThreadLocalVar(IlwisAppContext::tlvDONOTSHOWFINDERROR));
			rpr = Representation();
			if ( fDoNotShowError == 0 || *fDoNotShowError == false)
				throw err;
		}
	}
	else
		rpr = Representation();
	return iRet;
}

int ObjectInfo::ReadElement(const char* sSection, const char* sEntry,
							const FileName& fnObj, CoordSystem& cs)
{
	String sCS;
	int iRet = ReadElement(sSection, sEntry, fnObj, sCS);
	if (iRet){
		try {
			FileName fnCSy(sCS, fnObj);
			if (fCIStrEqual(fnCSy.sExt, ".csy")) {
				if (!File::fExist(fnCSy)) {
					fnCSy.Dir(getEngine()->getContext()->sStdDir());
					if (!File::fExist(fnCSy))
						fnCSy.Dir(getEngine()->getContext()->sStdDir() + "\\Basemaps");
					if (!File::fExist(fnCSy))
						fnCSy = FileName(sCS, fnObj); // restore actual filename read from ODF (for error reporting)
				}
				cs = CoordSystem(fnCSy);
			}
			else
				cs = CoordSystem(sCS);
		}
		catch (const ErrorObject& err) 
		{
			bool *fDoNotShowError = (bool *)(getEngine()->pGetThreadLocalVar(IlwisAppContext::tlvDONOTSHOWFINDERROR));
			cs = CoordSystem();
			if ( fDoNotShowError == 0 || *fDoNotShowError == false)
				throw err;
		}
	}
	else
		cs = CoordSystem();
	return iRet;
}

int ObjectInfo::ReadElement(const char* sSection, const char* sEntry, const FileName& fnObj,
							Map& mp)
{
	mp = Map();
	String sMap;
	int iRet = ReadElement(sSection, sEntry, fnObj, sMap);
	if (iRet) {
		try {
			FileName fnMap(sMap, fnObj);
			if (fCIStrEqual(fnMap.sExt, ".mpr")) {
				if (!File::fExist(fnMap)) {
					fnMap.Dir(getEngine()->getContext()->sStdDir());
					if (!File::fExist(fnMap))
						fnMap.Dir(getEngine()->getContext()->sStdDir() + "\\Basemaps");
					if (!File::fExist(fnMap))
						fnMap = FileName(sMap, fnObj); // restore actual filename read from ODF (for error reporting)
				}
				mp = Map(fnMap);
			}
			else
				mp = Map(sMap, fnObj.sPath());
		}
		catch (const ErrorObject& err) 
		{
			bool *fDoNotShowError = (bool *)(getEngine()->pGetThreadLocalVar(IlwisAppContext::tlvDONOTSHOWFINDERROR));
			if ( fDoNotShowError == 0 || *fDoNotShowError == false)
				throw err;
		}
	}
	return iRet;
}

int ObjectInfo::ReadElement(const char* sSection, const char* sEntry, const FileName& fnObj,
							SegmentMap& mp)
{
	mp = SegmentMap();
	String sMap;
	int iRet = ReadElement(sSection, sEntry, fnObj, sMap);
	if (iRet) {
		try {
			FileName fnMap(sMap, fnObj);
			if (fCIStrEqual(fnMap.sExt, ".mps")) {
				if (!File::fExist(fnMap)) {
					fnMap.Dir(getEngine()->getContext()->sStdDir());
					if (!File::fExist(fnMap))
						fnMap.Dir(getEngine()->getContext()->sStdDir() + "\\Basemaps");
					if (!File::fExist(fnMap))
						fnMap = FileName(sMap, fnObj); // restore actual filename read from ODF (for error reporting)
				}
				mp = SegmentMap(fnMap);
			}
			else
				mp = SegmentMap(sMap, fnObj.sPath());
		}
		catch (const ErrorObject& err) 
		{
			bool *fDoNotShowError = (bool *)(getEngine()->pGetThreadLocalVar(IlwisAppContext::tlvDONOTSHOWFINDERROR));
			if ( fDoNotShowError == 0 || *fDoNotShowError == false)
				throw err;
		}
	}
	return iRet;
}

int ObjectInfo::ReadElement(const char* sSection, const char* sEntry, const FileName& fnObj,
							PolygonMap& mp)
{
	mp = PolygonMap();
	String sMap;
	int iRet = ReadElement(sSection, sEntry, fnObj, sMap);
	if (iRet) {
		try {
			FileName fnMap(sMap, fnObj);
			if (fCIStrEqual(fnMap.sExt, ".mpa")) {
				if (!File::fExist(fnMap)) {
					fnMap.Dir(getEngine()->getContext()->sStdDir());
					if (!File::fExist(fnMap))
						fnMap.Dir(getEngine()->getContext()->sStdDir() + "\\Basemaps");
					if (!File::fExist(fnMap))
						fnMap = FileName(sMap, fnObj); // restore actual filename read from ODF (for error reporting)
				}
				mp = PolygonMap(fnMap);
			}
			else
				mp = PolygonMap(sMap, fnObj.sPath());
		}
		catch (const ErrorObject& err) 
		{
			bool *fDoNotShowError = (bool *)(getEngine()->pGetThreadLocalVar(IlwisAppContext::tlvDONOTSHOWFINDERROR));
			if ( fDoNotShowError == 0 || *fDoNotShowError == false)
				throw err;
		}
	}
	return iRet;
}

int ObjectInfo::ReadElement(const char* sSection, const char* sEntry, const FileName& fnObj,
							PointMap& mp)
{
	mp = PointMap();
	String sMap;
	int iRet = ReadElement(sSection, sEntry, fnObj, sMap);
	if (iRet) {
		try {
			FileName fnMap(sMap, fnObj);
			if (fCIStrEqual(fnMap.sExt, ".mpp")) {
				if (!File::fExist(fnMap)) {
					fnMap.Dir(getEngine()->getContext()->sStdDir());
					if (!File::fExist(fnMap))
						fnMap.Dir(getEngine()->getContext()->sStdDir() + "\\Basemaps");
					if (!File::fExist(fnMap))
						fnMap = FileName(sMap, fnObj); // restore actual filename read from ODF (for error reporting)
				}
				mp = PointMap(fnMap);
			}
			else
				mp = PointMap(sMap, fnObj.sPath());
		}
		catch (const ErrorObject& err) 
		{
			bool *fDoNotShowError = (bool *)(getEngine()->pGetThreadLocalVar(IlwisAppContext::tlvDONOTSHOWFINDERROR));
			if ( fDoNotShowError == 0 || *fDoNotShowError == false)
				throw err;
		}
	}
	return iRet;
}

int ObjectInfo::ReadElement(const char* sSection, const char* sEntry, const FileName& fnObj,
							Table& tbl)
{
	tbl = Table();
	String sTbl;
	int iRet = ReadElement(sSection, sEntry, fnObj, sTbl);
	if (iRet) {
		try {
			FileName fnTbl(sTbl, fnObj);
			if (fCIStrEqual(fnTbl.sExt, ".tbt")) {
				if (!File::fExist(fnTbl)) {
					fnTbl.Dir(getEngine()->getContext()->sStdDir());
					if (!File::fExist(fnTbl))
						fnTbl.Dir(getEngine()->getContext()->sStdDir() + "\\Basemaps");
					if (!File::fExist(fnTbl))
						fnTbl = FileName(sTbl, fnObj); // restore actual filename read from ODF (for error reporting)
				}
				tbl = Table(fnTbl);
			}
			else
				tbl = Table(sTbl, fnObj.sPath());
		}
		catch (const ErrorObject& err) 
		{
			bool *fDoNotShowError = (bool *)(getEngine()->pGetThreadLocalVar(IlwisAppContext::tlvDONOTSHOWFINDERROR));
			if ( fDoNotShowError == 0 || *fDoNotShowError == false)
				throw err;
		}
	}
	return iRet;
}

int ObjectInfo::ReadElement(const char* sSection, const char* sEntry, const FileName& fnObj,
							MapList& mpl)
{
	mpl = MapList();
	String sMpl;
	int iRet = ReadElement(sSection, sEntry, fnObj, sMpl);
	if (iRet) {
		try {
			FileName fnMpl(sMpl, fnObj);
			if (fCIStrEqual(fnMpl.sExt, ".mpl")) {
				if (!File::fExist(fnMpl)) {
					fnMpl.Dir(getEngine()->getContext()->sStdDir());
					if (!File::fExist(fnMpl))
						fnMpl.Dir(getEngine()->getContext()->sStdDir() + "\\Basemaps");
					if (!File::fExist(fnMpl))
						fnMpl = FileName(sMpl, fnObj); // restore actual filename read from ODF (for error reporting)
				}
				mpl = MapList(fnMpl);
			}
			else
				mpl = MapList(sMpl, fnObj.sPath());
		}
		catch (const ErrorObject& err) 
		{
			bool *fDoNotShowError = (bool *)(getEngine()->pGetThreadLocalVar(IlwisAppContext::tlvDONOTSHOWFINDERROR));
			if ( fDoNotShowError == 0 || *fDoNotShowError == false)
				throw err;
		}
	}
	return iRet;
}

int ObjectInfo::ReadElement(const char* sSection, const char* sEntry,
							const FileName& fnObj, Filter& flt)
{
	String sFlt;
	int iRet = ReadElement(sSection, sEntry, fnObj, sFlt);
	if (iRet) {
		try {
			FileName fnFlt(sFlt, fnObj);
			if (fCIStrEqual(fnFlt.sExt, ".fil")) {
				if (!File::fExist(fnFlt)) {
					fnFlt.Dir(getEngine()->getContext()->sStdDir());
					if (!File::fExist(fnFlt))
						fnFlt.Dir(getEngine()->getContext()->sStdDir() + "\\Basemaps");
					if (!File::fExist(fnFlt))
						fnFlt = FileName(sFlt, fnObj); // restore actual filename read from ODF (for error reporting)
				}
				flt = Filter(fnFlt);
			}
			else
				flt = Filter(sFlt, fnObj.sPath());
		}
		catch (const ErrorObject& err) 
		{
			bool *fDoNotShowError = (bool *)(getEngine()->pGetThreadLocalVar(IlwisAppContext::tlvDONOTSHOWFINDERROR));
			flt = Filter();
			if ( fDoNotShowError == 0 || *fDoNotShowError == false)
				throw err;
		}
	}
	else
		flt = Filter();
	return iRet;
}

int ObjectInfo::ReadElement(const char* sSection, const char* sEntry,
							const FileName& fnObj, Classifier& clf)
{
	String sClf;
	int iRet = ReadElement(sSection, sEntry, fnObj, sClf);
	if (iRet) {
		try {
			FileName fnClf(sClf, fnObj);
			if (fCIStrEqual(fnClf.sExt.c_str() , ".clf")) {
				if (!File::fExist(fnClf)) {
					fnClf.Dir(getEngine()->getContext()->sStdDir());
					if (!File::fExist(fnClf))
						fnClf.Dir(getEngine()->getContext()->sStdDir() + "\\Basemaps");
					if (!File::fExist(fnClf))
						fnClf = FileName(sClf, fnObj); // restore actual filename read from ODF (for error reporting)
				}
				clf = Classifier(fnClf);
			}
			else
				clf = Classifier(sClf, fnObj.sPath());
		}
		catch (const ErrorObject& err) 
		{
			clf = Classifier();
			throw err;
		}
	}
	else
		clf = Classifier();
	return iRet;
}

int ObjectInfo::ReadElement(const char* sSection, const char* sEntry,
							const FileName& fnObj, SampleSet& sms)
{
	String sSms;
	int iRet = ReadElement(sSection, sEntry, fnObj, sSms);
	if (iRet) {
		try {
			FileName fnSms(sSms, fnObj);
			if (fCIStrEqual(fnSms.sExt, ".sms")) {
				if (!File::fExist(fnSms)) {
					fnSms.Dir(getEngine()->getContext()->sStdDir());
					if (!File::fExist(fnSms))
						fnSms.Dir(getEngine()->getContext()->sStdDir() + "\\Basemaps");
					if (!File::fExist(fnSms))
						fnSms = FileName(sSms, fnObj); // restore actual filename read from ODF (for error reporting)
				}
				sms = SampleSet(fnSms);
			}
			else
				sms = SampleSet(sSms, fnObj.sPath());
		}
		catch (const ErrorObject& err) 
		{
			sms = SampleSet();
			bool *fDoNotShowError = (bool *)(getEngine()->pGetThreadLocalVar(IlwisAppContext::tlvDONOTSHOWFINDERROR));
			if ( fDoNotShowError == 0 || *fDoNotShowError == false)
				throw err;

		}
	}
	else
		sms = SampleSet();
	return iRet;
}

int ObjectInfo::ReadElement(const char* sSection, const char* sEntry,
							const FileName& fnObj, ValueRange& vr)
{
	int iRet;
	String s;
	iRet = ReadElement(sSection, sEntry, fnObj, s);
	if (iRet) {
		vr = ValueRange(s);
		//    ValueRangeReal* vrr = vr->vrr();
		//    if (0 != vrr)
		//      vrr->AdjustRangeToStep();
	}
	else
		vr = ValueRange();
	return iRet;
}

int ObjectInfo::ReadElement(const char* sSection, const char* sEntry,
							const FileName& fnObj, Color& col)
{
	int iRet;
	String s;
	iRet = ReadElement(sSection, sEntry, fnObj, s);
	sscanf(s.sVal(),"%lx",&col);
	col.alpha() = 255 - col.alpha(); // on-file = transparency; in-memory = alpha, for backward compatibilty; WriteElement adjusted too
	return iRet;
}

int ObjectInfo::ReadElement(const char* sSection, const char* sEntry,
							const FileName& fnObj, char* pc, int iLen)
{
	String sVal;
	int iRet = ReadElement(sSection, sEntry, fnObj, sVal);
	const char* s = sVal.c_str();

	for (int i = 0; i < iLen; ++i) 
	{
		if (i > iRet) {
			*pc++ = 0;
			continue;
		}
		int iVal;
		char c = *s++; 
		if (isdigit((unsigned char)c))
			iVal = c - '0';
		else
			iVal = c - 'A' + 10;
		iVal *= 16;
		c = *s++; 
		if (isdigit((unsigned char)c))
			iVal += c - '0';
		else
			iVal += c - 'A' + 10;
		*pc++ = iVal;
	}
	return iRet;
}

int ObjectInfo::ReadElement(const char* sSection, const char* sEntry, const FileName& fnObj, CObject* cobj)
{
	int iRead, iLen;
	String sEntryLen("%sLen", sEntry);
	ObjectInfo::ReadElement(sSection, sEntryLen.c_str(), fnObj, iLen);
	if (iLen > 0) {
		char* sMem = new char[iLen];
		iRead = ObjectInfo::ReadElement(sSection, sEntry, fnObj, sMem, iLen);
		CMemFile memobj;
		memobj.Write(sMem, iLen);
		memobj.Seek(0, CFile::begin);
		CArchive ca(&memobj, CArchive::load);
		cobj->Serialize(ca);
		return iRead;
	}
	else
		return 0;
}
