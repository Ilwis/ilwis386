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
/* ObjectInfo WriteElement functions
Copyright Ilwis System Development ITC
may 1996, by Jelle Wind
Last change:  WK   10 Jun 98    6:38 pm
*/

#pragma warning( disable : 4503 )
#pragma warning( disable : 4786 )

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
#include "Engine\Base\File\ElementMap.h"
#include "Engine\Base\File\ElementContainer.h"
#include <sys/stat.h>
#include <direct.h>

bool ObjectInfo::WriteElement(const char* sSection, const char* sEntry,
							  const FileName& fn, const char* sValue)
{
	if (File::fExist(fn) && _access(fn.sFullPath().c_str(), 2) == -1) // file readonly
		return false;

	const ElementContainer* ec = dynamic_cast<const ElementContainer*>(&fn);
	if (ec && 0 != ec->em) {
		if (sSection == 0 || *sSection == 0)
			return 0;
		if (sEntry == 0 || *sEntry == 0)
			return 0;
		if (sValue == 0)
			return 0;
		ElementMap& em = const_cast<ElementMap&>(*ec->em);
		em(sSection, sEntry) = const_cast<char*>(sValue); // to call the correct string constructor
		return true;
	}
	if (!fn.fValid())
		return false;
	String sSec(const_cast<char*>(sSection));
	sSec &= fn.sSectionPostFix;
	String sVal;
	if (sValue)
	{
		sVal = String(const_cast<char*>(sValue));
		// if sVal is quoted, add extra quotes because the ReadPrivateProfileString removes the quotes
		if ((sVal[0] == '\'') && (sVal[sVal.length()-1] == '\''))
			sVal = String("\'%S\'", sVal);
		return WritePrivateProfileString(sSec.c_str(), sEntry,
			sVal.c_str(), fn.sFullName().c_str()) == TRUE;
	}
	else
		return WritePrivateProfileString(sSec.c_str(), sEntry,
		sValue, fn.sFullName().c_str()) == TRUE;
}

bool ObjectInfo::WriteElement(const char* sSection, const char* sEntry,
							  const FileName& fn, const String& sValue)
{
	return WriteElement(sSection, sEntry, fn, sValue.c_str());
	//  return WritePrivateProfileString(sSection, sEntry,
	//                                   sValue.c_str(), fn.sFullName().c_str()) == TRUE;
}

bool ObjectInfo::WriteElement(const char* sSection, const char* sEntry,
							  const FileName& fn, double rValue)
{
	if (rUNDEF == rValue)
		return WriteElement(sSection, sEntry, fn, "?");
	else {
		if ( abs(rValue) < 1e12)
			return WriteElement(sSection, sEntry, fn, String("%.12f", rValue));
		else
			return WriteElement(sSection, sEntry, fn, String("%e", rValue));
	}
}

bool ObjectInfo::WriteElement(const char* sSection, const char* sEntry,
							  const FileName& fn, long iValue)
{
	if (iUNDEF == iValue)
		return WriteElement(sSection, sEntry, fn, "?");
	else
		return WriteElement(sSection, sEntry, fn, String("%li", iValue));
}

bool ObjectInfo::WriteElement(const char* sSection, const char* sEntry,
							  const FileName& fn, bool fValue)
{
	if ( fValue)
		return WriteElement(sSection, sEntry, fn, "Yes");
	//     return WritePrivateProfileString(sSection, sEntry, "Yes", fn.sFullName().c_str())==TRUE;
	else
		return WriteElement(sSection, sEntry, fn, "No");
	//     return WritePrivateProfileString(sSection, sEntry, "No", fn.sFullName().c_str())==TRUE;
}

bool ObjectInfo::WriteElement(const char* sSection, const char* sEntry,
							  const FileName& fnObj, const FileName& fn)
{
	return WriteElement(sSection, sEntry, fnObj, fn.sRelativeQuoted(true, fnObj.sPath()));
}

bool ObjectInfo::WriteElement(const char* sSection, const char* sEntry,
							  const FileName& fnObj, const IlwisObject& obj)
{
	if (obj.fValid())	{
		String sPath = fnObj.sPath();
		if (sPath == "")
			sPath = "---"; // to prevent use of current directory
		return WriteElement(sSection, sEntry, fnObj, obj->sNameQuoted(true, sPath));
	}
	else
		return WriteElement(sSection, sEntry, fnObj, (const char*)0);
}

bool ObjectInfo::WriteElement(const char* sSection, const char* sEntry,
							  const FileName& fnObj, const RangeInt& rng)
{
	if (rng.iLo() <= rng.iHi())
		return WriteElement(sSection, sEntry, fnObj, rng.s());
	else
		return WriteElement(sSection, sEntry, fnObj, (char*)0);
}

bool ObjectInfo::WriteElement(const char* sSection, const char* sEntry,
							  const FileName& fnObj, const RangeReal& rng)
{
	if (rng.rLo() <= rng.rHi())
		return WriteElement(sSection, sEntry, fnObj, rng.s());
	else
		return WriteElement(sSection, sEntry, fnObj, (char*)0);
}

bool ObjectInfo::WriteElement(const char* sSection, const char* sEntry,
							  const FileName& fnObj, const Coord& crd)
{
	return WriteElement(sSection, sEntry, fnObj, String("%lg %lg", crd.x, crd.y));
}

bool ObjectInfo::WriteElement(const char* sSection, const char* sEntry,
							  const FileName& fnObj, const ILWIS::TimeInterval& interval)
{
	double b = interval.getBegin();
	double e = interval.getEnd();
	double s = interval.getStep();
	return WriteElement(sSection, sEntry, fnObj, String("%f::%f::%f", b, e, s));
}

bool ObjectInfo::WriteElement(const char* sSection, const char* sEntry,
							  const FileName& fnObj, const LatLon& ll)
{
	return WriteElement(sSection, sEntry, fnObj, String("%lg %lg", ll.Lat, ll.Lon));
}

bool ObjectInfo::WriteElement(const char* sSection, const char* sEntry,
							  const FileName& fnObj, const RowCol& rc)
{
	return WriteElement(sSection, sEntry, fnObj, String("%li %li", rc.Row, rc.Col));
}

bool ObjectInfo::WriteElement(const char* sSection, const char* sEntry,
							  const FileName& fnObj, const MinMax& mm)
{
	return WriteElement(sSection, sEntry, fnObj,
		String("%li %li %li %li",
		mm.rcMin.Row, mm.rcMin.Col,
		mm.rcMax.Row, mm.rcMax.Col));
}

bool ObjectInfo::WriteElement(const char* sSection, const char* sEntry,
							  const FileName& fnObj, const CSize& sz)
{
	return WriteElement(sSection, sEntry, fnObj, String("%li %li", sz.cx, sz.cy));
}

bool ObjectInfo::WriteElement(const char* sSection, const char* sEntry,
							  const FileName& fnObj, const CRect& rect)
{
	return WriteElement(sSection, sEntry, fnObj,
		String("%li %li %li %li",
		rect.top, rect.left,
		rect.bottom, rect.right));
}

bool ObjectInfo::WriteElement(const char* sSection, const char* sEntry,
							  const FileName& fnObj, const CoordBounds& cb)
{
	return WriteElement(sSection, sEntry, fnObj,
		String("%lg %lg %lg %lg",
		cb.cMin.x, cb.cMin.y,
		cb.cMax.x, cb.cMax.y));
}

bool ObjectInfo::WriteElement(const char* sSection, const char* sEntry, 
							  const FileName& fnObj, Color col)
{
	String sValue("%lx", (long)col);
	return WriteElement(sSection, sEntry, fnObj, sValue.c_str());
}

bool ObjectInfo::WriteElement(const char* sSection, const char* sEntry,
							  const FileName& fnObj, const ObjectTime& tim)
{
	if (tim != 0)
		return WriteElement(sSection, sEntry, fnObj, String("%li", (long)tim));
	else
		return WriteElement(sSection, sEntry, fnObj, (char*)0);
}

bool ObjectInfo::WriteElement(const char* sSection, const char* sEntry,
							  const FileName& fnObj, const Domain& dm)
{
	if (dm.fValid()) {
		long i = WriteElement(sSection, sEntry, fnObj, dm->sNameQuoted(true, fnObj.sPath()));
		DomainInfo dinf(dm);
		dinf.Store(fnObj, sSection);
		if (dm->fnObj == fnObj)
			dm->Store();
		return i!=0;
	}  
	return WriteElement(sSection, sEntry, fnObj, (char*)0);
}

bool ObjectInfo::WriteElement(const char* sSection, const char* sEntry,
							  const FileName& fnObj, const Representation& rpr)
{
	if (rpr.fValid())
		return WriteElement(sSection, sEntry, fnObj, rpr->sNameQuoted(true, fnObj.sPath()));
	else
		return WriteElement(sSection, sEntry, fnObj, (char*)0);
}

bool ObjectInfo::WriteElement(const char* sSection, const char* sEntry,
							  const FileName& fnObj, const GeoRef& gr)
{
	String sGr;
	if (gr.fValid()) {
		String sPath = fnObj.sPath();
		if (sPath == "")
			sPath = "---"; // to prevent use of current directory
		sGr = gr->sNameQuoted(true, sPath);
		if (gr->fnObj == fnObj)
			gr->Store();
	}
	else
		sGr = "None";
	return WriteElement(sSection, sEntry, fnObj, sGr);
}

bool ObjectInfo::WriteElement(const char* sSection, const char* sEntry,
							  const FileName& fnObj, const RealArray& arr)
{
	//  String s = String("%i %i ", arr.iSize(), arr.iLower());
	String s = String("%i %i ", arr.iSize(), 0);
	//  for (int j=arr.iLower(); j <= arr.iUpper(); j++ )
	for (int j=0; j < arr.iSize(); j++ )
		s &= String("%f ", arr[j]);
	return WriteElement(sSection, sEntry, fnObj, s);
}

bool ObjectInfo::WriteElement(const char* sSection, const char* sEntry,
							  const FileName& fnObj, const RealMatrix& mat)
{
	bool f;
	String sEnt = sEntry;
	sEnt &= "_Size";
	if ((mat.iRows() == 0) || (mat.iCols() == 0)) {
		f = WriteElement(sSection, sEnt.c_str(), fnObj, 0L);
		return f;
	}   
	f = WriteElement(sSection, sEnt.c_str(), fnObj, String("%i %i", mat.iRows(), mat.iCols()));
	for (int i=0; i < mat.iRows(); i++) {
		sEnt = sEntry;
		sEnt &= String("_Row%i", i);
		String sRow;
		for (int j=0; j < mat.iCols(); j++ )
			sRow &= String("%f ", mat(i, j));
		f = WriteElement(sSection, sEnt.c_str(), fnObj, sRow);
	}
	return f;
}

bool ObjectInfo::WriteElement(const char* sSection, const char* sEntry,
							  const FileName& fnObj, const IntMatrix& mat)
{
	bool f;
	String sEnt = sEntry;
	sEnt &= "_Size";
	f = WriteElement(sSection, sEnt.c_str(), fnObj, String("%i %i", mat.iRows(), mat.iCols()));
	for (int i=0; i < mat.iRows(); i++) {
		sEnt = sEntry;
		sEnt &= String("_Row%i", i);
		String sRow;
		for (int j=0; j < mat.iCols(); j++ )
			sRow &= String("%i ", mat(i, j));
		f = WriteElement(sSection, sEnt.c_str(), fnObj, sRow);
	}
	return f;
}

bool ObjectInfo::WriteElement( const char* sSection, const char* sEntry, 
							  const FileName& fnObj, const ValueRange& vr)
{
	if (vr.fValid())
		return WriteElement(sSection, sEntry, fnObj, vr->sRange(true));
	else
		return WriteElement(sSection, sEntry, fnObj, (char*)0);
}

bool ObjectInfo::WriteElement(const char* sSection, const char* sEntry,
							  const FileName& fnObj, const char* pc, int iLen)
{
	char* pcBuf = new char[1+2*iLen];
	char* s = pcBuf;
	for (int i = 0; i < iLen; ++i, ++pc) 
	{
		char c = (*pc & 0xF0) >> 4;
		if (c < 0x0A)
			c += '0';
		else
			c += 'A' - 0x0A;
		*s++ = c;
		c = *pc & 0x0F;
		if (c < 0x0A)
			c += '0';
		else
			c += 'A' - 0x0A;
		*s++ = c;
	}
	*s = 0;
	bool fRet = WriteElement(sSection, sEntry, fnObj, pcBuf);
	delete [] pcBuf;
	return fRet;
}

bool ObjectInfo::WriteElement(const char* sSection, const char* sEntry, const FileName& fnObj, CObject& cobj)
{
	CMemFile memobj;
	{ CArchive ca(&memobj, CArchive::store);
	cobj.Serialize(ca);
	}
	memobj.Seek(0, CFile::begin);
	int iLen = memobj.GetLength();
	char* sMem = new char[iLen];
	memobj.Read(sMem, iLen);
	String sEntryLen("%sLen", sEntry);
	ObjectInfo::WriteElement(sSection, sEntryLen.c_str(), fnObj, iLen);
	return ObjectInfo::WriteElement(sSection, sEntry, fnObj, sMem, iLen);
}

void ObjectInfo::RemoveSection(const FileName& fn, const String& sSection)
{
	WritePrivateProfileString(sSection.c_str(), NULL, NULL, fn.sFullPath().c_str() );
}

void ObjectInfo::RemoveKey(const FileName& fn, const String& sSection, const String& sKey)
{
	WritePrivateProfileString(sSection.c_str(), sKey.c_str(), NULL, fn.sFullPath().c_str() );
}
