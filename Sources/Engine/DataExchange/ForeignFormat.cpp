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
#define DomainInfo ggDomainInfo

#undef DomainInfo
#pragma warning( disable : 4786 )

#include "Headers\toolspch.h"

#pragma warning( disable : 4715 )

#include <set>
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\Engine.h"
#include "Engine\Table\tbl.h"
#include "Engine\Map\basemap.h"
#include "Engine\DataExchange\ForeignFormat.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Engine\DataExchange\ForeignFormatMap.h"
#include "Headers\Hs\IMPEXP.hs"

list<CheckFormat> ForeignFormat::lstCheckFormatFunctions;

map<String, list<String> > mapDummy;
map<String, list<String> > ForeignFormat::mapFormatParts = mapDummy;;

ForeignFormat::ForeignFormat() :
	iTopRecord(0),
  trq(NULL),
	collection(0),
	dmKey(0),
	fUseAs(true)
{}

ForeignFormat::ForeignFormat(const String& fn, ForeignFormat::mtMapType _mtType) :
	sForeignInput(fn),
	mtLoadType(_mtType),
	iTopRecord(0),
	trq(NULL),
	collection(0),
	dmKey(0),
	fUseAs(true)
{
}

ForeignFormat::~ForeignFormat()
{
	delete dmKey;	
}

// Query object will be created in this create function
ForeignFormat* ForeignFormat::Create(const String& sMethod)
{
	ForeignFormatFuncs *funcs = getEngine()->formats[sMethod];
	if (funcs) {
		CreateQueryFunction func = funcs->queryFunc;
		return (func)();
	}
	return NULL;
}

ForeignFormat* ForeignFormat::Create(const FileName& fnObj, ParmList& pm )
{
	String sMethod = pm.sGet("method");
	if ( sMethod == "") //  can we retrieve it from the file
	{
		ObjectInfo::ReadElement("ForeignFormat", "Method", fnObj, sMethod);
		pm.Add(new Parm("method", sMethod));
	}

	// temporary init of create functions will be moved to a proper place
	ForeignFormatFuncs *funcs = getEngine()->formats[sMethod];
	if ( funcs)
	{
		CreateFunction1 func = funcs->importFunc;
		return (func)(fnObj, pm);
	}		
	return NULL;
}	

//// Import functionality
//ForeignFormat* ForeignFormat::Create(const FileName& fnObj)
//{
//	ParmList pm;
//	return ForeignFormat::Create(fnObj, pm);
//
//}

// Export functionality
//ForeignFormat* ForeignFormat::Create(const FileName& fnForeign, const IlwisObject& obj)
//{
//	if (!obj.fValid())
//		return NULL;
//	
//	String sType;
//	FileName fn = obj->fnObj;
//	FileName fnExpr;
//
//	IlwisObject::iotIlwisObjectType iotType = IlwisObject::iotObjectType(obj->fnObj);
//
//	Array<String> arParts;
//	Split(sExpres, arParts, "(,)");
//
//	if ( fCIStrEqual(arParts[0], "GeoGateway"))
//	{
//		// still to do
//	}
//	else if ( fCIStrEqual(arParts[0].sLeft(6), "Ilwis2"))
//	{
//		switch ( iotType )
//		{
//			case IlwisObject::iotPOINTMAP:
//				return new PointMapIlwis20(FileName::fnUniqueShort(FileName(fnForeign, ".mpp")));
//				break;
//			case IlwisObject::iotSEGMENTMAP:
//				return new SegmentMapIlwis20(FileName::fnUniqueShort(FileName(fnForeign, ".mps")));
//			case IlwisObject::iotPOLYGONMAP:
//				return new PolygonMapIlwis20(FileName::fnUniqueShort(FileName(fnForeign, ".mpa")));
//			case IlwisObject::iotRASMAP:
//				return 0;
//				break;
//			case IlwisObject::iotTABLE:
//			case IlwisObject::iotHISTRAS:
//			case IlwisObject::iotHISTSEG:
//			case IlwisObject::iotHISTPOL:
//			case IlwisObject::iotHISTPNT:
//				return new TableIlwis20(FileName::fnUniqueShort(FileName(fnForeign, ".tbt")));
//				break;
//			default:
//				return 0;
//		}
//	}
//}

bool ForeignFormat::GetTime(FILETIME& LastWrite)
{
	HANDLE hFile = CreateFile(sForeignInput.c_str(), 
	                          GENERIC_READ,
														FILE_SHARE_READ,
														NULL,
														OPEN_EXISTING,
														FILE_ATTRIBUTE_NORMAL,
														NULL);

	if ( hFile == INVALID_HANDLE_VALUE) return false;

	GetFileTime(hFile, NULL, NULL, &LastWrite);
	return true;
}

bool ForeignFormat::fForeignFileTimeChanged(const FileName& fnObj)
{
	String sTime;
	if ( ObjectInfo::ReadElement("ForeignFormat", "LastStoreTime", fnObj, sTime))
		return true;

	long iLowTime = sTime.sHead(",").iVal();
	long iHighTime = sTime.sTail(",").iVal();

	FILETIME LastWrite;
	if ( GetTime( LastWrite ))
	{
		return LastWrite.dwLowDateTime == iLowTime && LastWrite.dwHighDateTime == iHighTime;
	}
	return true;
}

void ForeignFormat::Store(IlwisObject obj)
{
	FILETIME LastWrite;
	if ( GetTime(LastWrite) )
	{
		obj->WriteElement("ForeignFormat", "LastStoreTime", String("%d,%d", LastWrite.dwLowDateTime, LastWrite.dwHighDateTime));
	}

	if ( sForeignInput != "")
		obj->WriteElement("ForeignFormat", "Input", sForeignInput.sUnQuote());
	obj->WriteElement("ForeignFormat", "AssociatedType",(long)mtLoadType);
	if ( fnOutputName.sFile != "")
		obj->WriteElement("ForeignFormat","Output",fnOutputName);
	obj->WriteElement("ForeignFormat","UseAs",fUseAs);
}

CoordSystem ForeignFormat::GetCoordSystem()
{
	throw ErrorObject(String(TR("Function %S should be implemented").c_str(), "GetCoordSystem()"));
	return CoordSystem("unknown");
}

LayerInfo ForeignFormat::GetLayerInfo(BaseMapPtr *ptr, ForeignFormat::mtMapType mtType, bool fCreate)
{
	mtLoadType = mtType;
  vector<LayerInfo> objects(6);
	objects[mtType].dvrsMap = ptr->dvrs();
	objects[mtType].fnObj = ptr->fnObj;
	objects[mtType].csy = ptr->cs();
	objects[mtType].cbMap = ptr->cb();
	IterateLayer(objects, fCreate);
	return objects[mtType];
}

ForeignFormat::mtMapType  ForeignFormat::AssociatedMapType()
{
	return mtLoadType;
}

// retrieves the 'other' parts belonging to a datafile (e.g. shape with four other files)
void ForeignFormat::GetDataFiles(const FileName& fnForeign, vector<FileName>& fnFiles)
{
	if (!fnForeign.fValid())
		return;

	// initially rwad the file
	if ( mapFormatParts.size() == 0 )
	{
		String sPath = getEngine()->getContext()->sIlwDir() + "\\Resources\\Def";
		File FormatParts(sPath + "\\FormatParts.def");

		while (!FormatParts.fEof() )	
		{
			String sLine;
			FormatParts.ReadLnAscii(sLine);	
			if ( sLine[0] !=';') // not a comment
			{
				Array<String> arParts;
				Split(sLine, arParts, ";");	
				for ( int i = 1; i < arParts.size(); ++i )
				{
					mapFormatParts[arParts[0] ].push_back(arParts[i]);
				}					
			}
		}
	}

  fnFiles.push_back(fnForeign);		

	list<String> &lstParts = mapFormatParts[fnForeign.sExt];
	FileName fnOther(fnForeign);	
	for(list<String>::iterator cur = lstParts.begin(); cur != lstParts.end(); ++cur )
	{
		fnOther.sExt = *cur;
		if ( fnOther.fExist() )
			fnFiles.push_back(fnOther);
	}		
}

Domain *ForeignFormat::GetTableDomain()
{
	return dmKey;
}

String ForeignFormat::sFindDefaultMethod(const FileName& fnForeignFile)
{
	IlwisSettings setting("DefaultSettings\\DefaultForeignMethod");

	// find the format
	String sFormat = sFindFormatName(fnForeignFile.sFullName());
	if ( sFormat == "" )
		return "";

	HKEY hKey;
	char sValueName[100];
	unsigned char sData[100];
	DWORD iSize1 = 100, iSize2 = 100, iType;
	int iIndex = 0;
	// enumrate trhough the keys in the registry to fund to see if formats match and return method
	LONG iRet = RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\ITC\\Ilwis\\3.0\\DefaultSettings\\DefaultForeignMethod", 0, KEY_READ, &hKey);	

	if ( iRet == ERROR_SUCCESS )
	{
		iRet = RegEnumValue(hKey, iIndex, sValueName, &iSize1, NULL, &iType, sData, &iSize2);
		while(iRet == ERROR_SUCCESS || iRet == ERROR_MORE_DATA)
		{
			String sMethod(sData);
			String sKeyFormat(sValueName);
			// have we found a format which matches the format have the file, return ist preferred method
			if ( sKeyFormat == sFormat)
			{
				RegCloseKey(hKey);
				return sMethod;
			}					

			iSize1 = iSize2 = 100;
			iRet = RegEnumValue(hKey, ++iIndex, sValueName, &iSize1, NULL, &iType, sData, &iSize2);
		}	
	}
	else // lets try the default of the default, in the def file.
	{
	// try to find the find in the def file 
		String sSystemDir = getEngine()->getContext()->sIlwDir() + "\\Resources\\Def\\ForeignFormatNames.def";
		char sNameBuf[50];
		GetPrivateProfileString("Default method", sFormat.c_str(), "", sNameBuf, 50, sSystemDir.c_str());
		String sName(sNameBuf);
		if ( sName != "")
			return sName;
	}		
	return "GDAL";
}

String ForeignFormat::sFindFormatName(const String& sFO) 
{
	// try to find the find in the def file 
	FileName fnFO(sFO);
	String sSystemDir = getEngine()->getContext()->sIlwDir() + "\\Resources\\Def\\ForeignFormatNames.def";
	FileName fnff(sSystemDir);
	if ( !fnff.fExist())
		return "";
	char sNameBuf[20];
	sNameBuf[0] = 0;
	if ( fnFO.sExt != "")
		GetPrivateProfileString("Formats", fnFO.sExt.c_str(), "", sNameBuf, 20, fnff.sFullPath().c_str());
	String sName(sNameBuf);
	// if present this is the name we need
	if ( sName != "" )
		return sName;
	// else look if there are format functions for this type
	for(list<CheckFormat>::iterator cur = ForeignFormat::lstCheckFormatFunctions.begin();
	    cur != ForeignFormat::lstCheckFormatFunctions.end();
			++cur )
	{
		CheckFormat func = *cur;
		sName = (func)(fnFO);	
		if ( sName != "" )
			return sName;
			
	}
	return "GDAL";
}

String ForeignFormat::sGetForeignInput()
{
	return sForeignInput;
}

FileName ForeignFormat::fnGetForeignFile() {
	return FileName(sForeignInput);
}

LayerInfo ForeignFormat::GetLayerInfo(ParmList& parms) {
	return LayerInfo();
}

void ForeignFormat::ReadParameters(const FileName& fnObj, ParmList& pm)
{
	String sV;
	ObjectInfo::ReadElement("ForeignFormat","Input",fnObj,sV);
	pm.Add(new Parm("input", sV));
	ObjectInfo::ReadElement("ForeignFormat","Output",fnObj,fnOutputName);
	pm.Add(new Parm("output",fnOutputName.sRelative()));
	ObjectInfo::ReadElement("ForeignFormat","UseAs",fnObj,fUseAs);
	pm.Add(new Parm("import", fUseAs));
	ObjectInfo::ReadElement("ForeignFormat","UseAs",fnObj,sMethod);
	pm.Add(new Parm("import", sMethod));
}

/*
 reads a long integer, that was written by WriteBuf
 b == the start of the memory block to be read, IN/OUT parameter: after completion b points to the next byte that was not yet read, or to the end of the buffer
 nr == the long integer number that was read, OUT parameter:  the existing number is overwritten
*/

void FFBlobUtils::ReadBuf(char ** b, long & nr)
{
	nr = *(long*)(*b);
	*b += sizeof(long);
}

/*
 reads a vector of LinearRing* from "b", that was written by WriteBuf
 b == the start of the memory block to be read, IN/OUT parameter: after completion b points to the next byte that was not yet read, or to the end of the buffer
 rings == the LinearRing* vector that was read: IN/OUT parameter:  the rings read are appended to the existing vector
*/

void FFBlobUtils::ReadBuf(char ** b, vector<LinearRing*> & rings)
{
	long nrRings;
	ReadBuf(b, nrRings);
	for (long i = 0; i < nrRings; ++i) {
		long nrCoords;
		ReadBuf(b, nrCoords);
		vector<Coordinate> *coords = new vector<Coordinate>();
		for (long j = 0; j < nrCoords; ++j) {
			Coordinate c;
			c.x = ((double*)(*b))[j * 2];
			c.y = ((double*)(*b))[j * 2 + 1];
			coords->push_back(c);
		}
		*b += 2 * sizeof(double) * nrCoords;
		if (coords->size() > 0 && coords->at(coords->size() - 1) != coords->at(0)) // geos only accepts closed sequences to construct a LinearRing
			coords->push_back(coords->at(0));
		CoordinateSequence *seq = new CoordinateArraySequence(coords);
		LinearRing * ring = new LinearRing(seq, new GeometryFactory());
		rings.push_back(ring);
	}
}

/*
 counterpart of String::Split and String::SplitOn
 splits "str" based on "sDelim" and divides the parts into "as"
 unlike String::Split and String::SplitOn, SplitOnString only splits when all the characters in "sDelim" are encountered, in the same sequence
 the sequence sDelim will not be part of the strings in "as"
*/

void FFBlobUtils::SplitOnString(const String& str, Array<String>& as, const String sDelim)
{
	int iPlace = str.find(sDelim);
	if (iPlace == -1 || iPlace + sDelim.size() > str.size()) {
		as &= str;
	} else {
		as &= str.substr(0, iPlace);
		SplitOnString(str.substr(iPlace+sDelim.size()), as, sDelim);
	}
}

/*
 Grows the memory block pointed to by "start" by 4 bytes and appends "nr" to the block
 start == the start of the entire buffer (it may be re-allocated), IN/OUT parameter
 pos == the current write-position, which is the end of the buffer (memory has to be allocated for the next write, before a byte is written), IN/OUT parameter: after completion pos has advanced by 4 bytes
 iBufSize == the buffer-size (since there is no other mechanism to have the size of malloc-ed blocks), IN/OUT parameter
 nr == the "long" integer to be written (4 bytes), IN parameter
*/

void FFBlobUtils::WriteBuf(char **start, char **pos, long & iBufSize, const long nr)
{
	if (*start == 0) {
		iBufSize = sizeof(long);
		*start = (char*)malloc(iBufSize);
		*pos = *start;
	} else {
		iBufSize += sizeof(long);
		char * oldstart = *start;
		long delta = *pos - *start;
		*start = (char*)realloc(*start, iBufSize);
		*pos = *start + delta;
	}
	*(long*)(*pos) = nr;
	*pos += sizeof(long);
}

/*
 Grows the memory block pointed to by "start" by coords.size() * 2 * sizeof(double) bytes and appends the coordinates as x,y IEEE doubles to the block
 start == the start of the entire buffer (it may be re-allocated), IN/OUT parameter
 pos == the current write-position, which is the end of the buffer (memory has to be allocated for the next write, before a byte is written), IN/OUT parameter: after completion pos has advanced by coords.size() * 2 * sizeof(double) bytes
 iBufSize == the buffer-size (since there is no other mechanism to have the size of malloc-ed blocks), IN/OUT parameter
 coords == the coordinate array (as strings to be parsed! x and y are space-separated!) to be written as x, y doubles in the memory block, IN parameter
 cb == the coordbounds that is to be "extended" by every coordinate that is encountered in "coords", IN/OUT parameter
*/

void FFBlobUtils::WriteBuf(char **start, char **pos, long & iBufSize, const Array<String> & coords, CoordBounds *cb)
{
	if (*start == 0) {
		iBufSize = 2 * sizeof(double) * coords.size();
		*start = (char*)malloc(iBufSize);
		*pos = *start;
	} else {
		iBufSize += 2 * sizeof(double) * coords.size();
		char * oldstart = *start;
		long delta = *pos - *start;
		*start = (char*)realloc(*start, iBufSize);
		*pos = *start + delta;
	}

	for (long i = 0; i < coords.size(); ++i) {
		double x = coords[i].sHead(" ").rVal();
		double y = coords[i].sTail(" ").rVal();
		((double*)(*pos))[2 * i] = x;
		((double*)(*pos))[2 * i + 1] = y;
		(*cb) += Coord(x,y);
	}

	*pos += 2 * sizeof(double) * coords.size();
}

//----------------------------------------------------------------------------------------
