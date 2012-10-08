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
/* $Log: /ILWIS 3.0/BasicDataStructures/objinfo.cpp $
 * 
 * 43    22-04-04 10:17 Willem
 * [Bug=6471]
 * - Changed: the call to GetNewestDependentObject now needs an array.
 * This array is used to keep track of all files that already have been
 * checked. Because of the recursive nature of the function indefinite
 * loops are a danger. This is now prevented by first checking the array
 * of checked objects
 * 
 * 42    2/25/03 12:28p Martin
 * when removing files from collection, especially those files that are in
 * multiple collections the indexing of files must be adapted if a file is
 * removed
 * 
 * 41    24-01-03 15:36 Willem
 * - Added: Before trying to open a MapList check for its existance (this
 * caused problems when deleting the maplist before its maps)
 * 
 * 40    5-12-02 18:12 Hendrikse
 * added type == IlwisObject::iotSTEREOPAIR, needed for Anaglyph grf in
 * property sheet
 * 
 * 39    11/18/02 10:59a Martin
 * maps that belong to more than one container where only removed from
 * object collections they are linked to, not from maplists
 * 
 * 38    7/08/02 3:41p Martin
 * when removing a file from a collection (and adapting the collection) it
 * was not taken into account that the collection may already be deleted
 * (e.g. delete collection plus contents)
 * 
 * 37    6/24/02 3:10p Martin
 * added a function to remove a file from all collections it belongs to
 * 
 * 36    6/05/02 4:12p Martin
 * Merged from the ASTER branch
 * 
 * 35    12/04/01 16:03 Willem
 * The fUsedInOtherObjects functions now all use a Tranquilizer*. When no
 * Tranquilizer is passed by the caller a local one is defined.
 * Passing a valid Tranquilizer* by the caller prevents the constant focus
 * changes  in the MainWindow when more than one object is selected for
 * deletion.
 * 
 * 34    27-11-01 11:50 Retsios
 * Corrected the fDependent() check for .stp's: they can be both dependent
 * and independent -> read odf to decide.
 * 
 * 33    10/31/01 9:00a Martin
 * added a function to check if an object is useas
 * 
 * 32    10/04/01 12:57 Hendrikse
 * added asExtMask &= "*.stp"; in the mpr part of fUsedInOtherObjects()
 * 
 * 31    9/21/01 10:14a Martin
 * added function to check if a file is part of an database
 * 
 * 30    9/05/01 9:17 Hendrikse
 * added in fDependent the check: if (fCIStrEqual(sType , "StereoPair")
 * 
 * 29    3-09-01 19:04 Koolhoven
 * fUsedInOtherObjects() now also checks .mpv files for Layouts
 * fDependent() returns now true for Layout, to check the ObjectDependency
 * section
 * 
 * 28    8/30/01 3:05p Martin
 * Write objectcol info only to a file if the target file exists. else its
 * is created and we donnt want that
 * 
 * 27    16-08-01 19:00 Koolhoven
 * fDependent() now also recognizes matrices
 * 
 * 26    8/10/01 11:53a Martin
 * added a function to check if a file is an ilwis data file (and not
 * service)
 * 
 * 25    9-08-01 12:20 Koolhoven
 * added WriteAdditionOfFileToCollection() and
 * WriteRemovalOfFileFromCollection()
 * 
 * 24    7-08-01 11:26 Koolhoven
 * also take care of Graph and dependent MapLists
 * 
 * 23    20-04-01 12:53 Koolhoven
 * ObjectInfo::fUses() protected against no column
 * 
 * 22    19-03-01 10:48 Koolhoven
 * ObjectInfo::fSystemObject() no longer returns true for objects which do
 * not have a filename but do have a proper ElementMap (like Layout)
 * 
 * 21    15/03/01 15:55 Willem
 * Added check on val;id handle in fUsedInOtherObjects() function
 * 
 * 20    13-02-01 10:49a Martin
 * fUses checks now also if a map is in use with a georef as background
 * map
 * 
 * 19    2/08/01 11:57 Retsios
 * Undo last change: "prevent potential dot in pathname being detected as
 * file extension"; by looking more carefully, it looks like the object is
 * not a filename.
 * 
 * 18    2/08/01 11:48 Retsios
 * Solved a potential "dot in pathname detected as file-extension" problem
 * 
 * 17    31/01/01 15:28 Willem
 * Used by tree now also checks column dependencies when checking if an
 * object is using a table.
 * 
 * 16    26/01/01 12:12 Willem
 * The fDomain() function has now an extra bool parameter. If set this is
 * used to reject picturedomain reguardless. This parameter is only used
 * when building the popup menu for maps with internal domain to reject
 * the menu option "Show as Domain", which is not very useful for
 * DomainPicture anyway.
 * 
 * 15    10/01/01 16:19 Willem
 * Added SampleSet as dependent to fDependent() function
 * 
 * 14    29/11/00 16:43 Willem
 * "Used By" now does not check rastermaps anymore for coordsystems
 * dependencies. This is arranged through the GeoRef instead.
 * 
 * 13    17-11-00 15:25 Koolhoven
 * fnAttributeTable() now also returns a filename when the domain of the
 * map has an attribute table
 * 
 * 12    25/08/00 10:35 Willem
 * GeoRef is now also checked for Used By from raster maps (Background map
 * in GRCTP)
 * 
 * 11    18-07-00 11:08a Martin
 * fDependencies for copy checks ioc for dependencies
 * 
 * 10    18-07-00 8:54a Martin
 * fInCollection checks if a collection refered to actually exists
 * 
 * 9     15-05-00 10:21a Martin
 * added function to check if a object is member of a collection
 * 
 * 8     4-05-00 12:34p Martin
 * added function to retrieve the attrib table
 * 
 * 7     3-11-99 12:31 Wind
 * case insensitive comparison
 * 
 * 6     29-10-99 12:58 Wind
 * case sensitive stuff
 * 
 * 5     20-10-99 15:55 Wind
 * changed call to OpenFile to call to CreateFile, because OpenFile only
 * allows file names of up to 128 characters
 * 
 * 4     9/20/99 9:30a Wind
 * debugged iGetTotalFileSize
 * 
 * 3     9/08/99 10:08a Wind
 * adpated to use of quoted file and column names
 * 
 * 2     3/11/99 12:15p Martin
 * Added support for Case insesitive 
// Revision 1.7  1998/10/07 16:00:22  Wim
// fDomainUsedInOtherObjects() will only include .rpr with strict
//
// Revision 1.6  1998-02-09 16:00:26+01  Wim
// MapView is now also checked for dependencies.
// To make this possible every MapView is reported to be dependent.
//
/* ObjectInfo
   Copyright Ilwis System Development ITC
   may 1996, by Jelle Wind
	Last change:  WK    7 Oct 98    4:55 pm
*/
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
#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\Engine.h"
#include <sys/stat.h>
#include <direct.h>

String ObjectInfo::sDescr(const FileName& fn)
{
	String s;
	ReadElement("Ilwis", "Description", fn, s);
	return s;
}

String ObjectInfo::sExpression(const FileName& fn)
{
	String sType, s, sExpr;
	if (0 != ReadElement("IlwisObjectVirtual", "Expression", fn, sExpr))
		return sExpr;
	// downward compatibility with 2.02:
	ReadElement("Ilwis", "Type", fn, sType);
	if (fCIStrEqual(sType , "BaseMap"))
		ReadElement("BaseMap", "Type", fn, sType);
	ReadElement(sType.c_str(), "Type", fn, s);
	ReadElement(s.c_str(), "Expression", fn, sExpr);
	return sExpr;
}  

String ObjectInfo::sCalcExpression(const FileName& fn)
{
	String s;
	ReadElement("MapVirtual", "Type", fn, s);
	if (!fCIStrEqual(s ,"MapCalculate"))
		return sUNDEF;
	ReadElement("MapVirtual", "Expression", fn, s);
	return s;
}

void ObjectInfo::Add(Array<FileName>& afn, const FileName& fn, const String& sPath)
{
	if (!fn.fValid())
		return;
	for (unsigned int i=0; i < afn.iSize(); ++i)
		if (fn == afn[i])
			return;
	FileName fnx(fn);
	fnx.Dir(sPath);
	for (unsigned int i=0; i < afn.iSize(); ++i)
		if (fnx == afn[i])
			return;
	afn &= fnx;  
}

bool ObjectInfo::fDependent(const FileName& fn)
{
	String sType;
	ReadElement("Ilwis", "Type", fn, sType);
	if (fCIStrEqual(sType , "BaseMap"))
		ReadElement("BaseMap", "Type", fn, sType);
	else if (fCIStrEqual(sType , "MapView"))
		return true;
	else if (fCIStrEqual(sType , "GeoRef"))
		return true;
	else if (fCIStrEqual(sType , "SampleSet"))
		return true;
	else if (fCIStrEqual(sType , "Graph"))
		return true;
	else if (fCIStrEqual(sType , "Layout"))
		return true;
	String s;
	ReadElement(sType.c_str(), "Type", fn, s);
	if (fCIStrEqual(sType , "Matrix"))
		return !fCIStrEqual(s, "RealMatrix");
	else if (fCIStrEqual(s, "Table2Dim"))
		ReadElement("Table2Dim", "Type", fn, s);
	sType &= "Virtual";
	return fCIStrEqual(s, sType);
}

// return true if object contains dependencies that should be copied with the object
bool ObjectInfo::fDependenciesForCopy(const FileName& fn)
{
	bool f = fDependent(fn);
	if (f)
		return true;
	String sType;
	ReadElement("Ilwis", "Type", fn, sType);
	if ( fCIStrEqual(sType , "ObjectCollection"))
	{
		int iN;
		int iRet = ObjectInfo::ReadElement("ObjectCollection", "NrObjects", fn, iN);
		if ( iRet == 0 || iN <= 0 ) return false;
		FileName fnOb;
		for(int i = 0; i < iN; ++i )
		{
			String sKey("Object%d", i);
			ObjectInfo::ReadElement("ObjectCollection", sKey.c_str(), fn, fnOb);
			if ( File::fExist(fnOb) ) 
			{
				if ( fDependent(fnOb) )
					return true;
			}
		}
		return false;
	}
	if (fCIStrEqual(sType , "Table"))
		ReadElement("Table", "Type", fn, sType);
	if (!fCIStrEqual(sType ,"TableStore"))
		return false;
	// check if columns contain dependencies from other tables
	String s, sCol;
	int iCols=0, iObjs;
	ReadElement("Table", "Columns", fn, iCols);
	for (int i=0; i < iCols; ++i)
	{
		ReadElement("TableStore", String("Col%i", i).c_str(), fn, sCol);
		ReadElement(String("Col:%S", sCol).c_str(), "Type", fn, s);
		if (s == "ColumnStore")
			continue;
		if (0 == ReadElement(String("Col:%S", sCol).c_str(), "NrDepObjects", fn, iObjs))
			continue;
		for (int j=0; j < iObjs; ++j)
		{
			ReadElement(String("Col:%S", sCol).c_str(), String("Object%i", j).c_str(), fn, s);
			if (!fCIStrEqual(s.sLeft(7) , "Column "))
				continue;
			s = s.sRight(s.length()-7);
			char* p = s.strrchrQuoted('.');
			if (0 != p)
			{
				*p = 0;
				//s = (char*)s;
				FileName fnTable(s, ".tbt", false);
				if (fnTable != fn)
					return true;
			}  
		}   
	}
	return false;
}

bool ObjectInfo::fUses(const FileName& fn, const Domain& dm)
{
	String sType;
	FileName fnDom;
	ReadElement("Ilwis", "Type", fn, sType);
	ReadElement(sType.c_str(), "Domain", fn, fnDom);
	if (!fnDom.fValid())
		return false;
	if (fnDom == dm->fnObj)
		return true;
	// check columns of tabl
	String s;
	ReadElement("Table", "Columns", fn, s);
	int iCols = s.shVal();
	for (int i=0; i < iCols; i++) {
		ReadElement("TableStore", String("Col%i", i).c_str(), fn, s); // column name
		fnDom = FileName();
		ReadElement(String("Col:%S", s).c_str(), "Domain", fn, fnDom);
		if (!fnDom.fValid())
			continue;
		if (fnDom == dm->fnObj)
			return true;
	}
	ReadElement("Table", "Type", fn, s);
	if (fCIStrEqual(s , "Table2Dim")) {
		ReadElement("Table2Dim", "Domain1", fn, fnDom);
		if (!fnDom.fValid())
			return false;
		if (fnDom == dm->fnObj)
			return true;
		ReadElement("Table2Dim", "Domain2", fn, fnDom);
		if (!fnDom.fValid())
			return false;
		if (fnDom == dm->fnObj)
			return true;
	}
	return false;
}

bool ObjectInfo::fUses(const FileName& fn, const GeoRef& gr)
{
	FileName fnGrf;
	if (fCIStrEqual(fn.sExt, ".mpr")) {
		ReadElement("Map", "GeoRef", fn, fnGrf);
		if (!fnGrf.fValid())
			return false;
		return fnGrf == gr->fnObj;
	}  
	if (fCIStrEqual(fn.sExt, ".grf")) {
		String sType;
		ReadElement("GeoRef", "Type", fn, sType);
		ReadElement(sType.c_str(), "GeoRef", fn, fnGrf);
		if (!fnGrf.fValid())
			return false;
		return fnGrf == gr->fnObj;
	}
	return false;
}

bool ObjectInfo::fUses(const FileName& fn, const CoordSystem& cs)
{
	if (fCIStrEqual(fn.sExt, ".mpr"))  // raster maps use coordsystem through GeoRef, not directly
		return false;

	FileName fnCsy;
	ReadElement("BaseMap", "CoordSystem", fn, fnCsy);
	if (!fnCsy.fValid())
	{
		ReadElement("GeoRef", "CoordSystem", fn, fnCsy);
		if (!fnCsy.fValid()) 
			return false;
	}  
	return fnCsy == cs->fnObj;
}

bool ObjectInfo::fUses(const FileName& fn, const Representation& rpr)
{
	FileName fnRpr;
	ReadElement("Domain", "Representation", fn, fnRpr);
	if (!fnRpr.fValid())
		return false;
	return fnRpr == rpr->fnObj;
}

bool ObjectInfo::fUses(const FileName& fn, const String& sSect, const FileName& fnCheck)
{
	// checks the dependencies
	String s;
	ReadElement(sSect.c_str(), "NrDepObjects", fn, s);
	int iObjDep = s.shVal();
	String sPath = fn.sPath();
	FileName filnam;
	for (int i=0; i < iObjDep; i++) {
		ReadElement(sSect.c_str(), String("Object%i", i).c_str(), fn, filnam);
		if (filnam == fnCheck)
			return true;
	}
	return false;
}

bool ObjectInfo::fUses(const FileName& fn, const FileName& fnCheck)
{
	if (fCIStrEqual(fnCheck.sExt, ".tbt"))
	{
		FileName filnam;
		ReadElement("BaseMap", "AttributeTable", fn, filnam);
		if (!filnam.fValid())
			ReadElement("DomainSort", "AttributeTable", fn, filnam);
		if (filnam == fnCheck)
			return true;
		
		int iCols = 0;
		ReadElement("Table", "Columns", fnCheck, iCols);
		for (int i = 0; i < iCols; ++i)
		{
			String sCol;
			ReadElement("TableStore", String("Col%i", i).c_str(), fnCheck, sCol);
			if ("" == sCol)
				continue;
			Table tbl(fnCheck);
			Column col = tbl->col(sCol);
			if (!col.fValid())
				continue;
			if (fUses(fn, col))
				return true;
		}
	}  
	if (fDependent(fn))
		if (fUses(fn, "ObjectDependency", fnCheck))
			return true;

	if ( IlwisObject::iotObjectType(fn) == IlwisObject::iotGEOREF)	
	{
		FileName fnMap;
		ReadElement("GeoRefCTP", "Reference Map", fn, fnMap);
		if ( fnMap == fnCheck )
			return true;
		ReadElement("GeoRefCTP", "Background Map", fn, fnMap);		
		if ( fnMap == fnCheck )
			return true;
	}		

	String s, sColName;
	ReadElement("Table", "Columns", fn, s);
	int iCols = s.shVal();
	for (int i=0; i < iCols; i++) {
		ReadElement("TableStore", String("Col%i", i).c_str(), fn, sColName);
		if (fUses(fn, String("Col:%S", sColName), fnCheck))
			return true;
	}
	return false;
}

bool ObjectInfo::fUses(const FileName& fn, const String& sSection, const Column& col)
{
	String sColName("Column %S.%S", col->fnTbl.sRelativeQuoted(false, fn.sPath()), col->sNameQuoted());
	String s;
	ReadElement(sSection.c_str(), "NrDepObjects", fn, s);
	int iObjDep =  s.shVal();
	String sPath = fn.sPath();
	for (int i=0; i < iObjDep; i++) {
		ReadElement(sSection.c_str(), String("Object%i", i).c_str(), fn, s);
		if (fCIStrEqual(s, sColName))
			return true;
	}
	return false;
}

bool ObjectInfo::fUses(const FileName& fn, const Column& col)
{
	if (fUses(fn, "ObjectDependency", col))
		return true;
	String s, sColName;
	ReadElement("Table", "Columns", fn, s);
	int iCols = s.shVal();
	for (int i=0; i < iCols; i++) {
		ReadElement("TableStore", String("Col%i", i).c_str(), fn, sColName);
		if (fUses(fn, sColName, col))
			return true;
	}
	return false;
}

// Static function
bool ObjectInfo::fUsedInOtherObjects(Tranquilizer* trq, const FileName& fnSelf, const Array<String>& asExtMask, Array<String>* as)
{
	WIN32_FIND_DATA findData;
	HANDLE handle;
	for (unsigned int i = 0; i < asExtMask.iSize(); ++i) 
	{
		if (trq->fAborted())
			return true;
		String sMask = fnSelf.sPath();
		sMask &= asExtMask[i];
		if ((handle = FindFirstFile(sMask.sVal(), &findData)) != INVALID_HANDLE_VALUE) 
		{
			do
			{
				if (trq->fAborted())
					return true;
				FileName fn = String(findData.cFileName);
				if (fn == fnSelf)
					continue;
				if (fUses(fn, fnSelf)) 
				{
					if (0 != as)
						(*as) &= String(findData.cFileName);
					else
						return true;
				}
			} while (FindNextFile(handle, &findData)!=FALSE);
		}
		if (handle != INVALID_HANDLE_VALUE)
			FindClose(handle);
	}
	if (0 != as)
		return 0 != as->iSize();
	return false;
}

void ObjectInfo::GetDataFiles(const FileName& fn, Array<FileName>& afnDat)
{
	FileName fnData;
	if (fCIStrEqual(fn.sExt, ".mpv")) {
		return; // no data files
	}
	else if (fCIStrEqual(fn.sExt, ".isl")) {
		int i = ReadElement("Script", "ScriptFile", fn, fnData);
		if (i > 0)
			afnDat &= fnData;
	}
	else if (fCIStrEqual(fn.sExt, ".his") || fCIStrEqual(fn.sExt,".hsa") || fCIStrEqual(fn.sExt, ".hsp") || fCIStrEqual(fn.sExt, ".hss") 
		|| fCIStrEqual(fn.sExt, ".tbt") || fCIStrEqual(fn.sExt, ".ta2")
		|| fCIStrEqual(fn.sExt, ".dom") || fCIStrEqual(fn.sExt, ".rpr") ) {
		int i = ReadElement("TableStore", "Data", fn, fnData);
		if (i > 0)
			afnDat &= fnData;
	}
	else if (fCIStrEqual(fn.sExt, ".fun")) {
		int i = ReadElement("FuncUser", "FuncDefFile", fn, fnData);
		if (i > 0)
			afnDat &= fnData;
	}
	else if (fCIStrEqual(fn.sExt, ".mat")) {
		return;
	}
	else if (fCIStrEqual(fn.sExt, ".grf")) {
		int i = ReadElement("TableStore", "Data", fn, fnData);
		if (i > 0)
			afnDat &= fnData;
		// 3D Georef ??
	}
	else if (fCIStrEqual(fn.sExt, ".mpl")) {
		return;
	}
	else if (fCIStrEqual(fn.sExt, ".csy")) {
		return;
	}
	else if (fCIStrEqual(fn.sExt, ".fil")) {
		return;
	}
	else if (fCIStrEqual(fn.sExt, ".mpr")) {
		int i = ReadElement("MapStore", "Data", fn, fnData);
		if (i > 0)
			afnDat &= fnData;
	}
	else if (fCIStrEqual(fn.sExt, ".mps") || fCIStrEqual(fn.sExt, ".mps")) {
		int i = ReadElement("SegmentMapStore", "DataSeg", fn, fnData);
		if (i > 0)
			afnDat &= fnData;
		i = ReadElement("SegmentMapStore", "DataSegCode", fn, fnData);
		if (i > 0)
			afnDat &= fnData;
		i = ReadElement("SegmentMapStore", "DataCrd", fn, fnData);
		if (i > 0)
			afnDat &= fnData;
		if (fCIStrEqual(fn.sExt, ".mpa")) {
			i = ReadElement("PolygonMapStore", "DataPol", fn, fnData);
			if (i > 0)
				afnDat &= fnData;
			i = ReadElement("PolygonMapStore", "DataPolCode", fn, fnData);
			if (i > 0)
				afnDat &= fnData;
			i = ReadElement("PolygonMapStore", "DataTop", fn, fnData);
			if (i > 0)
				afnDat &= fnData;
		}
	}
	else if (fCIStrEqual(fn.sExt, ".mpp")) {
		int i = ReadElement("TableStore", "Data", fn, fnData);
		if (i > 0)
			afnDat &= fnData;
	}
}

bool ObjectInfo::fDomainUsedInOtherObjects(const FileName& fnSelf, Array<String>* as, bool fStrict, Tranquilizer* trq)
{
    WIN32_FIND_DATA findData;
    HANDLE handle;

    Domain dm(fnSelf);
    Array<String> asExtMask;
    asExtMask &= "*.mpr";
    asExtMask &= "*.mps";
    asExtMask &= "*.mpa";
    asExtMask &= "*.mpp";
    asExtMask &= "*.tbt";
    asExtMask &= "*.ta2";
    if (fStrict)
        asExtMask &= "*.rpr";
    for (unsigned int i = 0; i < asExtMask.iSize(); ++i) 
    {
        if (trq->fAborted())
            return true;
        String sMask = fnSelf.sPath();
        sMask &= asExtMask[i];
        if ((handle=FindFirstFile(sMask.sVal(), &findData))!=INVALID_HANDLE_VALUE ) 
        {
            do 
            {
                if (trq->fAborted())
                    return true;
                FileName fn = String(findData.cFileName);
                fn.Dir(fnSelf.sPath());
                if (fn == fnSelf)
                    continue;
                if (fUses(fn, dm)) 
                {
                    if (0 != as)
                        (*as) &= String(findData.cFileName);
                    else
                        return true;
                }
            } while (FindNextFile(handle, &findData)!=FALSE);
        }
        FindClose(handle);
    }
    if (0 != as)
        return 0 != as->iSize();
    return false;
}

// Static function
bool ObjectInfo::fUsedInOtherObjects(const FileName& fnSelf, Array<String>* as, bool fStrict, Tranquilizer* trqPar)
{
	// if fStrict == false not all uses are checked, 
	// because the function call is then used to know if an object may be deleted
	WIN32_FIND_DATA findData;
	HANDLE handle;
	if (fCIStrEqual(fnSelf.sExt, ".isl"))
		return false; // difficult to check at the moment
	else if (fCIStrEqual(fnSelf.sExt, ".his") || fCIStrEqual(fnSelf.sExt, ".hsa") || 
	         fCIStrEqual(fnSelf.sExt, ".hsp") || fCIStrEqual(fnSelf.sExt, ".hss"))
		return false; 
	else if (fCIStrEqual(fnSelf.sExt, ".fun"))
		return false; // info about use of function is not yet stored

	bool fTrqLocal = trqPar == 0;
	bool fRet = false;
	Tranquilizer* trq = trqPar;
	if (fTrqLocal)
		trq = new Tranquilizer();
	trq->Start();
	trq->SetTitle(String("Use of %S", fnSelf.sFullNameQuoted()));
	trq->fText(String("Check on use %S", fnSelf.sFullNameQuoted()));

	if (fCIStrEqual(fnSelf.sExt, ".dom"))
		fRet = fDomainUsedInOtherObjects(fnSelf, as, fStrict, trq);
	else if (fCIStrEqual(fnSelf.sExt, ".grf")) 
	{
		GeoRef grf(fnSelf);
		Array<String> asExtMask;
		asExtMask &= "*.mpr";
		asExtMask &= "*.grf";
		asExtMask &= "*.mpv";
		asExtMask &= "*.mpl";
		for (unsigned int i = 0; i < asExtMask.iSize(); ++i) 
		{
			if (trq->fAborted())
				fRet = true;
			String sMask = fnSelf.sPath();
			sMask &= asExtMask[i];
			if ((handle=FindFirstFile(sMask.sVal(), &findData))!=INVALID_HANDLE_VALUE)
			{
				do 
				{
					if (trq->fAborted())
						fRet = true;
					FileName fn = String(findData.cFileName);
					if (fUses(fn, grf)) 
					{
						if (0 != as)
							(*as) &= String(findData.cFileName);
						else
							fRet = true;
					}
				} while (FindNextFile(handle, &findData)!=FALSE);
			}
			FindClose(handle);
		}
		if (0 != as)
			fRet = 0 != as->iSize();   
	}
	else if (fCIStrEqual(fnSelf.sExt, ".rpr")) 
	{
		if (!fStrict)
			fRet = false;
		Representation rpr(fnSelf);
		String sMask = fnSelf.sPath();
		sMask &= "*.dom";
		if ((handle=FindFirstFile(sMask.sVal(), &findData))!=INVALID_HANDLE_VALUE)
		{    
			do 
			{
				if (trq->fAborted())
					fRet = true;
				FileName fn = String(findData.cFileName);
				fn.Dir(fnSelf.sPath());
				if (fUses(fn, rpr)) 
				{
					if (0 != as)
						(*as) &= String(findData.cFileName);
					else
						fRet = true;
				}
			} while (FindNextFile(handle, &findData)!=FALSE);
		}
		FindClose(handle);
		if (0 != as)
			fRet = 0 != as->iSize();   
	}
	else if (fCIStrEqual(fnSelf.sExt, ".mpl")) 
	{
		Array<String> asExtMask;
		asExtMask &= "*.mpr";
		asExtMask &= "*.mat";
		asExtMask &= "*.sms";
		asExtMask &= "*.mpv";
		asExtMask &= "*.mpl";
		fRet = fUsedInOtherObjects(trq, fnSelf, asExtMask, as);
	} 
	else if (fCIStrEqual(fnSelf.sExt, ".csy")) 
	{
		CoordSystem csy(fnSelf);
		Array<String> asExtMask;
		asExtMask &= "*.mps";
		asExtMask &= "*.mpa";
		asExtMask &= "*.mpp";
		asExtMask &= "*.grf";
		asExtMask &= "*.mpv";
		asExtMask &= "*.mpl";
		for (unsigned int i = 0; i < asExtMask.iSize(); ++i) 
		{
			if (trq->fAborted())
				fRet = true;
			String sMask = fnSelf.sPath();
			sMask &= asExtMask[i];
			if ((handle=FindFirstFile(sMask.sVal(), &findData))!=INVALID_HANDLE_VALUE) 
			{
				do 
				{
					if (trq->fAborted())
						fRet = true;
					FileName fn = String(findData.cFileName);
					if (fUses(fn, csy)) 
					{
						if (0 != as)
							(*as) &= String(findData.cFileName);
						else
							fRet = true;
					}
				} while (FindNextFile(handle, &findData)!=FALSE);
			}
			FindClose(handle);
		}
		if (0 != as)
			fRet = 0 != as->iSize();   
	} 
	else if (fCIStrEqual(fnSelf.sExt, ".tbt")) 
	{
		Array<String> asExtMask;
		asExtMask &= "*.mpr";
		asExtMask &= "*.mps";
		asExtMask &= "*.mpa";
		asExtMask &= "*.mpp";
		asExtMask &= "*.tbt";
		asExtMask &= "*.mpv";
		asExtMask &= "*.mpl";
		asExtMask &= "*.grh";
		if (fStrict)
			asExtMask &= "*.dom";
		fRet = fUsedInOtherObjects(trq, fnSelf, asExtMask, as);
	} 
	else if (fCIStrEqual(fnSelf.sExt, ".fil")) 
	{
		Array<String> asExtMask;
		asExtMask &= "*.mpr";
		asExtMask &= "*.mpl";
		fRet = fUsedInOtherObjects(trq, fnSelf, asExtMask, as);
	} 
	else if (fCIStrEqual(fnSelf.sExt, ".mat")) 
	{
		Array<String> asExtMask;
		asExtMask &= "*.mpl";
		fRet = fUsedInOtherObjects(trq, fnSelf, asExtMask, as);
	} 
	else if (fCIStrEqual(fnSelf.sExt, ".mpr")) 
	{
		Array<String> asExtMask;
		asExtMask &= "*.mpl";
		asExtMask &= "*.sms";
		asExtMask &= "*.mpr";
		asExtMask &= "*.mps";
		asExtMask &= "*.mpa";
		asExtMask &= "*.mpp";
		asExtMask &= "*.mpv";
		asExtMask &= "*.tbt";
		asExtMask &= "*.grf";
		asExtMask &= "*.stp";
		bool fUsed = fUsedInOtherObjects(trq, fnSelf, asExtMask, as);
		bool fDmUsed = fDomain(fnSelf) && fDomainUsedInOtherObjects(fnSelf, as, fStrict, trq);
		fRet = fUsed || fDmUsed;
	}
	else if (fCIStrEqual(fnSelf.sExt, ".mpv")) 
	{
		Array<String> asExtMask;
		asExtMask &= "*.ilo";
		fRet = fUsedInOtherObjects(trq, fnSelf, asExtMask, as);
	} 
	else 
	{
		Array<String> asExtMask;
		asExtMask &= "*.mpr";
		asExtMask &= "*.mps";
		asExtMask &= "*.mpa";
		asExtMask &= "*.mpp";
		asExtMask &= "*.mpv";
		asExtMask &= "*.tbt";
		asExtMask &= "*.mpl";
		bool fUsed = fUsedInOtherObjects(trq, fnSelf, asExtMask, as);
		bool fDmUsed = fDomain(fnSelf) && fDomainUsedInOtherObjects(fnSelf, as, fStrict, trq);
		fRet = fUsed || fDmUsed;
	}
	if (fTrqLocal)
		delete trq;
	return fRet;
}

char ObjectInfo::cLastDataFileExtChar(const FileName& fnObj, const String& sExt)
{
	char c = '#';
	if (fCIStrEqual(fnObj.sExt,".mpr"))
		c = '%';
	else if (fCIStrEqual(fnObj.sExt, ".mps"))
		c = '$';
	else if (fCIStrEqual(fnObj.sExt, ".mpa"))
		c = '@';
	else if (fCIStrEqual(fnObj.sExt, ".mpp"))
		c = '!';
	else if (fCIStrEqual(fnObj.sExt,".tbt") && !fCIStrEqual(sExt, ".tb#"))
		c = '&';
	else if (fCIStrEqual(fnObj.sExt,".dom") && !fCIStrEqual(sExt, ".dm#"))
		c = '^';
	return c;
}

bool ObjectInfo::fTable(const FileName& fn)
{
  String s;
  return 0 != ReadElement("Table", (const char*)0, fn, s);
}

bool ObjectInfo::fDomain(const FileName& fn, bool fAllowDomPicture)
{
	String s;
	ReadElement("Ilwis", "ContainsDom", fn, s);
	bool fDomOk = fCIStrEqual(s,"True");
	if (fDomOk && !fAllowDomPicture)
	{
		String sDomType;
		ReadElement("Domain", "Type", fn, sDomType);
		fDomOk &= fDomOk && !fCIStrEqual(sDomType, "DomainPicture");
	}
	return fDomOk;
}

bool ObjectInfo::fRepresentation(const FileName& fn)
{
	String s;
	ReadElement("Ilwis", "ContainsRpr", fn, s);
	return fCIStrEqual(s, "True");
}

bool ObjectInfo::fGeoRef(const FileName& fn)
{
	String s;
	return 0 != ReadElement("GeoReference", (const char*)0, fn, s);
}

long ObjectInfo::iTotalFileSize(const Array<FileName>& afn)
{
	long iFileSize = 0;
	for (unsigned int i=0; i < afn.iSize(); ++i) {
		//    OFSTRUCT OfStr;
		//    int file = OpenFile(afn[i].sFullName().c_str(), &OfStr, OF_READ | OF_SHARE_COMPAT);
		HFILE file = (HFILE)CreateFile(afn[i].sFullName().c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL, NULL);
		if (file < 0)
			continue;
		iFileSize += ::GetFileSize((HANDLE)file, NULL);
		_lclose(file);
	}
	return iFileSize;
}

bool ObjectInfo::fUpToDate(const FileName& fn)
{
	ObjectTime objtime;
	ReadElement("Ilwis", "Time", fn, objtime);
	String sObjName;
	ObjectTime tmNewer;
	Array<FileName> afnChecked;
	ObjectDependency::GetNewestDependentObject(fn, String(), objtime, sObjName, tmNewer, afnChecked);
	return sObjName.length() == 0;
}

bool ObjectInfo::fSystemObject(const FileName& fn)
{
	if ("" == fn.sFile)	
	{
		const ElementContainer* ec = dynamic_cast<const ElementContainer*>(&fn);
		if (ec && 0 != ec->em) 
			return false;
		else			
			return true;
	}
	String sSystemDir = getEngine()->getContext()->sStdDir();
	sSystemDir &= "\\";
	String sPath = fn.sPath();
	return fCIStrEqual(sPath, sSystemDir) || fCIStrEqual(sPath, sSystemDir + "Basemaps\\");
}

bool ObjectInfo::fUpToDate(const FileName& fn, const String& sCol)
{
	ObjectTime objtime;
	ReadElement(String("Col:%S", sCol).c_str(), "Time", fn, objtime);
	String sObjName;
	ObjectTime tmNewer;
	Array<FileName> afnChecked;
	ObjectDependency::GetNewestDependentObject(fn, sCol, objtime, sObjName, tmNewer, afnChecked);
	return sObjName.length() == 0;
}

FileName ObjectInfo::fnAttributeTable(const FileName& fn)
{
	FileName fnAttrib;
	int iRet = ReadElement("BaseMap", "AttributeTable", fn, fnAttrib);		
	if (0 == iRet) {
		FileName fnDom;
		iRet = ReadElement("BaseMap", "Domain", fn, fnDom);		
		if (iRet)
			iRet = ReadElement("DomainSort", "AttributeTable", fnDom, fnAttrib);		
		if (0 == iRet)
			return FileName();
	}
	return fnAttrib;
}

bool ObjectInfo::fInCollection(const FileName& fn)
{
	int iN;
	int iRet = ReadElement("Collection", "NrOfItems", fn, iN);
	if ( iRet == 0 || iN <= 0 ) 
		return false;
	
	FileName fnCol;
	for(int i = 0; i < iN; ++i )
	{
		String sKey("Item%d", i);
		ReadElement("Collection", sKey.c_str(), fn, fnCol);
		if ( File::fExist(fnCol) ) return true;
	}
	return false;
}

void ObjectInfo::WriteAdditionOfFileToCollection(const FileName& filename, const FileName& fnCollection)
{
	if ( filename.fExist() == false) // else it will be created
		return;
	int iNr = 0;
	ReadElement("Collection", "NrOfItems", filename, iNr);
	if (iNr < 0)
		iNr = 0;
	for(int i=0; i < iNr; ++i)
	{
		FileName fnItem;
		ObjectInfo::ReadElement("Collection", String("Item%d", i).c_str(), filename, fnItem);
		if (fnItem == fnCollection)
			return;
	}
	WriteElement("Collection", "NrOfItems", filename, iNr+1);
	WriteElement("Collection", String("Item%d", iNr).c_str(), filename, fnCollection); 
}

void ObjectInfo::WriteRemovalOfFileFromCollection(const FileName& filename, const FileName& fnCollection)
{
	int iNr = 0;
	ReadElement("Collection", "NrOfItems", filename, iNr);
	if (iNr < 0)
		iNr = 0;
	int i=0;
	while( i < iNr)
	{
		FileName fnItem;
		String sEntry("Item%d", i);
		ObjectInfo::ReadElement("Collection", sEntry.c_str(), filename, fnItem);
		if (fnItem == fnCollection)
			IlwisObjectPtr::RemoveCollectionFromODF(filename, sEntry);
		int iNrNew = 0;
		ReadElement("Collection", "NrOfItems", filename, iNrNew);	
		if ( iNr != iNrNew ) //  because of the remove the number of items could be reduced
		{
			iNr = iNrNew;
		}			
		else
			++i;
	}
}

void ObjectInfo::RemoveFileFromLinkedCollections(const FileName& filename)
{
	int iNr = 0;
	ReadElement("Collection", "NrOfItems", filename, iNr);
	if (iNr < 0)
		iNr = 0;
	int i=0;
	while( i < iNr)
	{
		FileName fnItem;
		String sEntry("Item%d", i);
		ObjectInfo::ReadElement("Collection", sEntry.c_str(), filename, fnItem);
		if ( IlwisObject::iotObjectType(fnItem) == IlwisObject::iotOBJECTCOLLECTION )
		{
			ObjectCollection obc(fnItem);
			if ( obc.fValid()) // could be deleted (already), no remove needed anymore
				obc->Remove(filename);

		}	
		if ( IlwisObject::iotObjectType(fnItem) == IlwisObject::iotMAPLIST )		
		{
			if (fnItem.fExist())
			{
				MapList mpl(fnItem);
				if ( mpl.fValid() )
					mpl->RemoveMap(filename);
			}
		}
		//  because of the remove the number of items could be reduced			
		int iNrNew = 0;			
		ReadElement("Collection", "NrOfItems", filename, iNrNew);	
		if ( iNr != iNrNew ) 
		{
			iNr = iNrNew;
		}			
		else
			++i;				
	}
}

bool ObjectInfo::fIsDataObject(const FileName& fn)
{
	IlwisObject::iotIlwisObjectType type = IlwisObject::iotObjectType(fn);
	if (type == IlwisObject::iotRASMAP ||
		type == IlwisObject::iotHISTPNT ||
		type == IlwisObject::iotHISTPOL ||
		type == IlwisObject::iotHISTRAS ||
		type == IlwisObject::iotHISTSEG ||
		type == IlwisObject::iotMAPLIST ||
		type == IlwisObject::iotPOINTMAP ||
		type == IlwisObject::iotPOLYGONMAP ||
		type == IlwisObject::iotSEGMENTMAP ||
		type == IlwisObject::iotTABLE ||
		type == IlwisObject::iotTABLE2DIM||
		type == IlwisObject::iotSTEREOPAIR)
		return true;
	else
		return false;
}

bool ObjectInfo::fIsPartOfDataBase(const FileName& fn, const FileName& fnDB)
{	
	String sName;
	if ( fn.fExist() )
	{
		ReadElement("DataBaseCollection", "DataBaseName", fn, sName);	
		if ( sName != "" )
		return true;
	}		
	else if ( fnDB.fExist() )
	{
		int iN;
		int iRet = ReadElement("ObjectCollection", "NrObjects", fnDB, iN);
		if ( iRet == 0 || iN <= 0 ) 
			return false;
		
		for(int i = 0; i < iN; ++i )
		{
			String sKey("Object%d", i);
			FileName fnFile;
			ReadElement("ObjectCollection", sKey.c_str(), fnDB, fnFile);
			if ( fnFile == fn)  return true;
		}
	}		
	return false;			
}

bool ObjectInfo::fUseAs(const FileName& fn)
{
	bool fUseAs;
	ReadElement("Ilwis", "UseAs", fn, fUseAs);
	if ( !fUseAs)
		ReadElement("MapStore", "UseAs", fn, fUseAs);	
	return fUseAs;
}
