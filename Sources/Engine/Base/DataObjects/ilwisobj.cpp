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
/* IlwisObject, IlwisObjectPtr, IlwisObjectPtrList
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  J    21 Oct 99   10:03 am
*/
#define ILWISOBJ_C
#include "Headers\constant.h"
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Engine\Domain\dm.h"
#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\Engine.h"
#include "Engine\Domain\Dmvalue.h"
#include "Engine\SpatialReference\Gr.h"
#include "Engine\SpatialReference\Coordsys.h"
#include "Engine\Representation\Rpr.h"
#include "Engine\Map\basemap.h"
#include "Engine\Table\tbl.h"
#include "Engine\Map\Raster\MapList\maplist.h"
#include "Engine\Matrix\Matrxobj.h"
#include "Engine\Function\FUNCTION.H"
#include "Engine\Function\FILTER.H"
#include "Engine\Function\CLASSIFR.H"
#include "Engine\SampleSet\SAMPLSET.H"
#include "Engine\Scripting\Script.h"
#include "Engine\Base\Tokbase.h"
#include "Engine\Base\System\mutex.h"
#include "Engine\Table\Col.h"
#include "Engine\Map\Point\PNT.H"
#include "Engine\Map\Mapview.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Base\File\objinfo.h"
#include "Applications\Raster\MAPCOLSP.H"
#include "Engine\Domain\dminfo.h"
#include "Engine\Map\txtann.h"
#include "Engine\Base\DataObjects\ObjectCollection.h"
#include "Engine\Map\LayoutObject.h"
#include "Engine\Table\GraphObject.h"
#include "Engine\Stereoscopy\StereoPair.h"
#include "Engine\Base\DataObjects\CriteriaTreeObject.h"
#include "Engine\DataExchange\ForeignCollection.h"
#include "Engine\DataExchange\DatabaseCollection.h"
#include "Engine\Base\StlExtensions.h"
#include "Engine\Base\DataObjects\ObjectStructure.h"
#include <sys/stat.h>
#include <dos.h>
#include <direct.h>

static IlwisObjectPtrList dummyIOPL;

void IllegalNameError(const String& sName)
{
	String sErr("Illegal filename : '%S\'", sName);
	throw FileErrorObject(sErr, errNAME);//errIllegalFile);
}

static void UpdateCatalog(const FileName& fn)
{
	bool *fNoUpdate = (bool *)(getEngine()->pGetThreadLocalVar(IlwisAppContext::tlvNOUPDATECATALOG));
	if (fNoUpdate != 0)  // pointer must be valid
		if (*fNoUpdate)
			return;
		
	FileName* pfn = new FileName(fn);
	getEngine()->PostMessage(ILW_READCATALOG, 0, (long)pfn);
	//AfxGetApp()->GetMainWnd()->PostMessage(ILW_READCATALOG, 0, (long)pfn);
}


//-------------------------------------------------------------------------------------
IlwisObject::IlwisObject() : ptr(0), list(dummyIOPL)
{}

inline void IlwisObject::operator=(const IlwisObject& obj)
{ 
	//IlwisObject::list = obj.list; 
	SetPointer(obj.pointer()); 
}

bool IlwisObject::fEqual(const IlwisObject& obj) const
{
	if (fValid() && obj.fValid()) {
		if ((void*)ptr == (void*)obj.ptr)
			return true;
		return ptr->fEqual(*obj.ptr);
	}  
	if (fValid())
		throw ErrorObject(String("Comparison of %S with uninitialized Object", ptr->sTypeName()) , 1000);
	if (obj.fValid())
		throw ErrorObject(String("Comparison of %S with uninitialized Object", obj->sTypeName()) , 1000);
	throw ErrorObject("Comparison with uninitialized Object", 1000);
	
	return false;
}

bool IlwisObject::fEqual(const IlwisObjectPtr* obj) const
{
	if (fValid() && 0 != obj) {
		if ((void*)ptr == (void*)obj)
			return true;
		return ptr->fEqual(*obj);
	}
	return false;
}

bool IlwisObjectPtr::fEqual(const IlwisObjectPtr& ptr) const
{ 
	return &ptr == this; 
}

IlwisObject::IlwisObject(IlwisObjectPtrList& lst)
: list(lst)
{ 
	ptr = 0; 
}

IlwisObject::IlwisObject(IlwisObjectPtrList& lst, const FileName& filename)
: list(lst)
{
	ILWISSingleLock sl(&list.csAccess, TRUE,SOURCE_LOCATION);
	ptr = list.pGet(filename);
	if (ptr) {
		ILWISSingleLock sl(&ptr->csAccess, TRUE,SOURCE_LOCATION);
		ptr->iRef += 1;
	}
}

IlwisObject::IlwisObject(IlwisObjectPtrList& lst, IlwisObjectPtr* p)
: list(lst)
{
	ptr = 0;
	SetPointer(p);
}

IlwisObject::~IlwisObject()
{
	try 
	{
		SetPointer(0);
	}
	catch (ErrorObject& err) 
	{
		err.Show();
	}
	catch (CException* err)
	{
		MessageBeep(MB_ICONHAND);
		err->ReportError(MB_OK|MB_ICONHAND|MB_TOPMOST);
		err->Delete();
	}		
}

IlwisObject IlwisObject::obj(const FileName& fil)
{
	if (File::fIllegalName(fil.sFile))
		IllegalNameError(fil.sFile);
	
	FileName fn = fil;
	if (!File::fExist(fn)) { // check std dir
		fn.Dir(getEngine()->getContext()->sStdDir());
		if (!File::fExist(fn))
			NotFoundError(fil);
	}
	String sType;
	if (fil.sSectionPostFix.length() > 0) {
		char *p = const_cast<char*>(&fil.sSectionPostFix[1]);
		long iBandNr = String(p).iVal();
		long iOffsetForBands;
		fn.sSectionPostFix = "";
		ObjectInfo::ReadElement("MapList", "Offset", fn, iOffsetForBands);
		FileName fnMap;
		ObjectInfo::ReadElement("MapList", String("Map%li", iBandNr-1+iOffsetForBands).c_str(), fn, fnMap);
		if (fnMap.sSectionPostFix.length() > 0) {
			fn.sSectionPostFix = fnMap.sSectionPostFix;
			fn.sExt = ".mpl";
			return Map(fn,iBandNr);
		}
		else 
			return Map(fnMap);
	}
	
	ObjectInfo::ReadElement("Ilwis", "Type", fn, sType);
	if (fCIStrEqual("BaseMap" , sType)) 
		return BaseMap(fn);
	else if (fCIStrEqual("Table" , sType))
		return Table(fn);
	else if (fCIStrEqual("MapView" , sType))
		return MapView(fn);
	else if (fCIStrEqual("AnnotationText" , sType))
		return AnnotationText(fn);
	else if (fCIStrEqual("Domain" , sType))
		return Domain(fn);
	else if (fCIStrEqual("CoordSystem" , sType))
		return CoordSystem(fn);
	else if (fCIStrEqual("GeoRef" , sType))
		return GeoRef(fn);
	else if (fCIStrEqual("Representation" , sType))
		return Representation(fn);
	else if (fCIStrEqual("MapList" , sType))
		return MapList(fn);
	else if (fCIStrEqual("Function" , sType))
		return Function(fn);
	else if (fCIStrEqual("Filter" , sType))
		return Filter(fn);
	else if (fCIStrEqual("Classifier" , sType))
		return Classifier(fn);
	else if (fCIStrEqual("SampleSet" , sType))
		return SampleSet(fn);
	else if (fCIStrEqual("Matrix" , sType))
		return MatrixObject(fn);
	else if (fCIStrEqual("Script" , sType))
		return Script(fn);
	else if (fCIStrEqual("ObjectCollection" , sType)) 
		return ObjectCollection(fn);
	else if (fCIStrEqual("ForeignCollection" , sType)) 
		return ForeignCollection(fn);
	else if (fCIStrEqual("DataBaseCollection" , sType))
		return DataBaseCollection(fn);
	else if (fCIStrEqual("Layout" , sType)) 
		return LayoutObject(fn);
	else if (fCIStrEqual("Graph" , sType)) 
		return GraphObject(fn);
	else if (fCIStrEqual("StereoPair" , sType)) 
		return StereoPair(fn);
	else if (fCIStrEqual("CriteriaTree" , sType)) 
		return CriteriaTreeObject(fn);
	else
		return IlwisObject();
}

IlwisObject IlwisObject::objInvalid()
{
	return IlwisObject();
}

void IlwisObject::SetPointer(IlwisObjectPtr* p)
{
	if (p == ptr)
		return;
	if (p == 0 && ptr == 0)
		return;
	ILWISSingleLock sl(&list.csAccess, TRUE,SOURCE_LOCATION);
	if (ptr)
	{
		{
			ILWISSingleLock sl(&ptr->csAccess, TRUE,SOURCE_LOCATION);
			ptr->iRef -= 1;
			if (ptr->fChanged && !ptr->fErase)
			{
				try
				{
					if ( ptr->fStore() )
						ptr->Store();
				}
				catch (const ErrorObject&)
				{
				}
				ptr->fChanged = false;
			}
		}
		if (0 == ptr->iRef)
		{
			String sDeleteFile;
			if (ptr->fErase)
				if (0 == dynamic_cast<ColumnPtr*>(ptr))
					sDeleteFile = ptr->fnObj.sFullPath(true);

			bool fUpdateCat = (sDeleteFile.length() != 0) || ptr->fUpdateCatalog;
			list.Remove(ptr);
			// ptr becomes invalid by previous operation !
			ptr = 0;
			if (sDeleteFile.length() != 0)
				_unlink(sDeleteFile.sVal());
			if (fUpdateCat)
			{
				FileName fn(sDeleteFile);
				UpdateCatalog(fn);
			}
		}
	}
	if (p) 
		list.Add(p);
	ptr = p;
	if (ptr) 
		ptr->iRef += 1;
}

IlwisObjectPtr::IlwisObjectPtr(const FileName& fn, bool fCreate, const char* pcDefExt)
: fnObj(fn), fErase(false), fUpdateCatalog(false), _fAdditionalInfo(false),	_fReadOnly(false),
  fChanged(fCreate), iRef(0), objtime(0), 
  _fDomainChangeable(false), _fValueRangeChangeable(false), _fGeoRefChangeable(false),
  _fExpressionChangeable(false), fDoNotStore(false), _fUseAs(false),versionBinary(ILWIS::Version::bvFORMAT30)
{
	const ElementContainer* ec = dynamic_cast<const ElementContainer*>(&fn);
	if (ec && 0 != ec->em) {
		_fReadOnly = false;
		const_cast<ElementContainer&>(fnObj).em = ec->em;
		//		return;
	}
	else {
		if (File::fIllegalName(fnObj.sFile)) 
		{
			IllegalNameError(fnObj.sFile);

			if ((fnObj.sFile.length() != 0) && !fnObj.fValid())
				throw ErrorObject(WhatError("Invalid file name", errNAME), fn);
		}
		if (fCreate && fnObj.fValid())
		{
			FileName fnSystem(fnObj);
			fnSystem.Dir(getEngine()->getContext()->sStdDir());
			if (File::fExist(fnSystem)) {
				// not allowed to create object that has same name
				// as object in standard dir
				NotCreatedErrorSystem(fnObj);
			}
			_fReadOnly = false;

			if (fnObj.sExt != "")
				fUpdateCatalog = true;
			objtime = ObjectTime::timCurr();
			fErase = fnObj.sFile[0] == '$';
			return;
		}
		else
		{
			_fReadOnly = _access(fnObj.sFullPath().c_str(), 2)==-1;
		}
		fUpdateCatalog = false;
	}
	ReadBaseInfo("Ilwis");  // get Description, Time and UseAs
	
	int iInfoLines = iUNDEF;
	ReadElement("AdditionalInfo", "Lines", iInfoLines);
	if (iInfoLines > 0) {
		_fAdditionalInfo = true;
		for (int i=0; i < iInfoLines; ++i) {
			if (i > 0)
				_sAdditionalInfo &= "\r\n";
			String s;
			ReadElement("AdditionalInfo", String("Line%i", i).c_str(), s);
			_sAdditionalInfo &= s;
		}
	}
}

IlwisObjectPtr::~IlwisObjectPtr()
{
	// garbage collect for objects belonging to collections. 
	// If an external action has removed the collection or made the
	// entries in the section useless remove the entry. it prevents other problems.

	// Wim 11/10/00: but check on validity of filename, 
	// otherwise problems with MapViews
	if (fnObj.fValid()) {
		int iN;
		int iRet = ObjectInfo::ReadElement("Collection", "NrOfItems", fnObj, iN);
		if ( iRet == 0 || iN <= 0 ) return;
		csAccess.Lock();
		FileName fnCol;
		for(int i = 0; i < iN; ++i )
		{
			String sKey("Item%d", i);
			ObjectInfo::ReadElement("Collection", sKey.c_str(), fnObj, fnCol);
			if ( !File::fExist(fnCol) ) 
				RemoveCollectionFromODF(fnObj, sKey);			
		}
		csAccess.Unlock();
	}
}

void IlwisObjectPtr::Store()
{
	if ( fDoNotStore )
		return;
	WriteBaseInfo("Ilwis");
	if (!fAdditionalInfo())
		WriteElement("AdditionalInfo", (char*)NULL, (char*)NULL);
	else {
		String s;
		short iLines = 0;
		for (unsigned int i=0; i < _sAdditionalInfo.length(); ++i) {
			char c = _sAdditionalInfo[i];
			if ('\r' == c)
				continue;
			if ('\n' == c) {
				WriteElement("AdditionalInfo", String("Line%i", iLines).c_str(), s);
				s = String();
				iLines++;
			}
			else
				s &= c;
		}
		if ("" != s) {
			WriteElement("AdditionalInfo", String("Line%i", iLines).c_str(), s);
			iLines++;
		}
		WriteElement("AdditionalInfo", "Lines", iLines);
	}
	fChanged = false;
	if (fUpdateCatalog && fnObj.sFile.length() != 0) {
		UpdateCatalog(fnObj);
		fUpdateCatalog = false;
	}
}

void IlwisObjectPtr::ReadBaseInfo(const char* sSection)
{
	ReadElement(sSection, "Description", sDescription);
	ReadElement(sSection, "Time", objtime);
	if (objtime == iUNDEF)
	{
		if (File::fExist(fnObj))
			objtime = ObjectTime(fnObj);
	}
	if (objtime == 0)
		objtime = ObjectTime::timCurr();
	
	if (0 == ReadElement(sSection, "DomainChangeable", _fDomainChangeable))
		_fDomainChangeable = false;
	if (0 == ReadElement(sSection, "ValueRangeChangeable", _fValueRangeChangeable))
		_fValueRangeChangeable = false;
	if (0 == ReadElement(sSection, "GeoRefChangeable", _fGeoRefChangeable))
		_fGeoRefChangeable = false;
	if (0 == ReadElement(sSection, "ExpressionChangeable", _fExpressionChangeable))
		_fExpressionChangeable = false;
	if (0 == ReadElement(sSection, "UseAs", _fUseAs))
		_fUseAs = false;
}

void IlwisObjectPtr::WriteBaseInfo(const char* sSection)
{
	WriteElement(sSection, "Description", sDescription);
	WriteElement(sSection, "Time", objtime);
	WriteElement(sSection, "Version", "3.1");
	WriteElement(sSection, "Class", sType());
	if (fDomainChangeable())
		WriteElement(sSection, "DomainChangeable", fDomainChangeable());
	else
		WriteElement(sSection, "DomainChangeable", (char*)0);
	if (fValueRangeChangeable())
		WriteElement(sSection, "ValueRangeChangeable", fValueRangeChangeable());
	else
		WriteElement(sSection, "ValueRangeChangeable", (char*)0);
	if (fGeoRefChangeable())
		WriteElement(sSection, "GeoRefChangeable", fGeoRefChangeable());
	else
		WriteElement(sSection, "GeoRefChangeable", (char*)0);
	if (fExpressionChangeable())
		WriteElement(sSection, "ExpressionChangeable", fExpressionChangeable());
	else
		WriteElement(sSection, "ExpressionChangeable", (char*)0);
	if (fUseAs())
		WriteElement(sSection, "UseAs", fUseAs());
	else
		WriteElement(sSection, "UseAs", (char*)0);
}

String IlwisObjectPtr::sName(bool fExt, const String& sDirRelative) const
{
	if (fnObj.fValid())
		return fnObj.sRelative(fExt, sDirRelative);
	return sExpression();
}

String IlwisObjectPtr::sType() const
{
	return "Unspecified Ilwis Object";
}

String IlwisObjectPtr::sTypeName() const
{
	String s = sType();
	s &= " \"";
	
	String sNam = sName(); 
	s &= sNam;
	s &= "\"";
	return s;
}

bool IlwisObjectPtr::fReadOnly() const
{
	(const_cast<CCriticalSection*>(&csAccess))->Lock();
	bool f = _fReadOnly || fSystemObject();
	(const_cast<CCriticalSection*>(&csAccess))->Unlock();
	return f;
}

bool IlwisObjectPtr::fDataReadOnly() const
{
	return fReadOnly() || fDependent();
}

void IlwisObjectPtr::SetReadOnly(bool f)
{
	csAccess.Lock();
	_fReadOnly = f;
	csAccess.Unlock();
    if (0 != dynamic_cast<ColumnPtr*>(this))
		return;
	
	ObjectStructure os;
	os.GetAssociatedFiles(false);  // only get ODF and data files
	GetObjectStructure(os);
	list<String> lstFiles;		
	os.GetUsedFiles(lstFiles, false);		
    // loop through all files in lstFiles and set/reset the readonly flag
	for (list<String>::iterator cur = lstFiles.begin(); cur != lstFiles.end(); ++cur)
	{
		FileName fn(*cur);
		File::SetReadOnly(fn.sFullName(), f);
	}
}

FileName IlwisObjectPtr::fnCopy(const FileName& fn, bool fCopyAsMember) const
{
	if (!fCopyAsMember)
		return fn;
	FileName fnNew = fnObj;
	fnNew.Dir(fn.sPath());
	return fnNew;
}


short IlwisObjectPtr::iParseParm(const String& sExpr, Array<String>& as)
{
	// too simple implementation
	// function will probably replace by functionality of parser/calculater
	TextInput inp(sExpr);
	TokenizerBase tokenize(&inp);
	Token tok = tokenize.tokGet(); // function name
	tok = tokenize.tokGet();  // '('
	if (tok.sVal() != "(")
		return shUNDEF;
	while ((tok.sVal() != ")") && (tok.sVal().length() != 0)) {
		tok = tokenize.tokGet();
		if (tok.sVal() == ",") { // empty parm
			as &= String();
			continue;
		}
		as &= tok.sVal();
		if (tok.sVal() == "-") {
			tok = tokenize.tokGet();
			as[as.iSize()-1] &= tok.sVal();
		}
		tok = tokenize.tokGet();
		if (tok.sVal() == ":") { // range
			as[as.iSize()-1] &= ':';
			tok = tokenize.tokGet();
			as[as.iSize()-1] &= tok.sVal();
			if (tok.sVal() == "-") {
				tok = tokenize.tokGet();
				as[as.iSize()-1] &= tok.sVal();
			}
			tok = tokenize.tokGet();
			if (tok.sVal() == ":") {
				as[as.iSize()-1] &= ':';
				tok = tokenize.tokGet();
				as[as.iSize()-1] &= tok.sVal();
				tok = tokenize.tokGet();
			}
		}
		if (tok.sVal() == ",")
			continue;
		if (tok.sVal() == "(") {
			as[as.iSize()-1] &= tok.sVal();
			tok = tokenize.tokToCloseBracket();
			if ((tok.sVal() == ""))
				return shUNDEF;
			as[as.iSize()-1] &= tok.sVal();
			tok = tokenize.tokGet();  // next token ',' or ')'
		}
		if ((tok.sVal() != ",") && (tok.sVal() != ")"))
			return shUNDEF;
	}
	if (tok.sVal() != ")")
		return shUNDEF;
	tok = tokenize.tokGet();  // skip ')'
	if (tok.sVal().length() != 0)
		return shUNDEF;
	return as.iSize();
}

bool IlwisObjectPtr::fParseParm(const String& sExpr, Array<String>& as)
{
	// too simple implementation
	// function will probably replace by functionality of parser/calculater
	TextInput inp(sExpr);
	TokenizerBase tokenize(&inp);
	Token tok = tokenize.tokGet(); // function name
	tok = tokenize.tokGet();  // '('
	if (tok.sVal() != "(")
		return false;
	bool fMinus = false;
	unsigned int i=0;
	for (; i < as.iSize(); i++) {
		tok = tokenize.tokGet();
		if (tok.sVal() == ",") {
			as[i] &= String(); // empty parm
			continue;
		}  
		if (fMinus) {
			as[i] &= tok.sVal();
			fMinus = false;
		}
		else
			as[i] = tok.sVal();
		if (tok.sVal() == "-") {
			if (fMinus)
				return false;
			fMinus = true;
			i--;
			continue;
		}
		else if (tok.sVal() == ",")
			continue;
		tok = tokenize.tokGet();
		if (tok.sVal() == ":") {
			as[i] &= ':';
			tok = tokenize.tokGet();
			as[i] &= tok.sVal();
			tok = tokenize.tokGet();
		}
		if (tok.sVal() == "(") {
			as[i] &= tok.sVal();
			tok = tokenize.tokToCloseBracket();
			if ((tok.sVal() == ""))
				return shUNDEF;
			as[i] &= tok.sVal();
			tok = tokenize.tokGet();  // next token ',' or ')'
		}
		if ((tok.sVal() != ",") && (tok.sVal() != ")"))
			return false;
	}
	if (tok.sVal() != ")")
		return false;
	if (i != as.iSize())
		return false;
	tok = tokenize.tokGet();  // skip ')'
	return tok.sVal().length() == 0;
}

String IlwisObjectPtr::sParseFunc(const String& sExpression)
{
	// too simple implementation
	// function will probably replace by functionality of parser/calculater
	String sExpr = sExpression;
	TextInput inp(sExpr);
	TokenizerBase tokenize(&inp);
	Token tok = tokenize.tokGet();
	if (tok.sVal() == "{") { // skip to "}"
		while ((tok.sVal() != "}") && tok.sVal().length() != 0) {
			tok = tokenize.tokGet();
		}
		if (tok.sVal().length() == 0)
			return sUNDEF;
		tok = tokenize.tokGet();
	}
	String sFunc = tok.sVal(); // function name
	tok = tokenize.tokGet();
	if (tok.sVal() != "(")
		return sUNDEF;
	return sFunc;
}


void IlwisObjectPtrList::Remove(IlwisObjectPtr* ptr)
{
	ILWISSingleLock sl(&csAccess, TRUE,SOURCE_LOCATION);
	
	for (DLIterP<IlwisObjectPtr> iter(this); iter.fValid(); ++iter)
		if (iter() == ptr) {
			delete iter.remove();
			return;
		}
}

void IlwisObjectPtrList::Add(IlwisObjectPtr* ptr)
{
	ILWISSingleLock sl(&csAccess, TRUE,SOURCE_LOCATION); 
	// check if already there
	for (DLIterP<IlwisObjectPtr> iter(this); iter.fValid(); ++iter) {
		if (iter() == ptr){ 
			return;
		}
	}
	insert(ptr);
}


IlwisObjectPtr* IlwisObjectPtrList::pGet(const FileName& fn) const
{
	ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE,SOURCE_LOCATION);
	for (DLIterCP<IlwisObjectPtr> iter(this); iter.fValid(); ++iter)
		if (fn == iter()->fnObj) 
			return iter();
		return 0;
}

IlwisObjectPtr::IlwisObjectPtr()
: fnObj(), fErase(false), _fReadOnly(false),
fChanged(false), iRef(0), objtime(iUNDEF),
fUpdateCatalog(false), _fAdditionalInfo(false),
_fDomainChangeable(false), _fValueRangeChangeable(false), _fGeoRefChangeable(false),
_fExpressionChangeable(false), fDoNotStore(false), _fUseAs(false)
{
}

double IlwisObjectPtr::rReadElement(const char* sSection, const char* sEntry) const
{
	String sValue;
	if (!ReadElement(sSection, sEntry, sValue))
		return rUNDEF;
	return sValue.rVal();
}

long IlwisObjectPtr::iReadElement(const char* sSection, const char* sEntry) const
{
	String sValue;
	if (!ReadElement(sSection, sEntry, sValue))
		return iUNDEF;
	return sValue.iVal();
}

void IlwisObjectPtr::Updated()
{
	if (fReadOnly()) return;
	fChanged = true;
	objtime = ObjectTime::timCurr();
}

void IlwisObjectPtr::UpdateTime()
{
	Updated();
	WriteElement(sObjectSection().c_str(), "Time", objtime);
}

String IlwisObjectPtr::sObjectSection() const
{
	return "Ilwis";
}

String IlwisObjectPtr::sAdditionalInfo() const
{
	return _sAdditionalInfo;
}

bool IlwisObjectPtr::fAdditionalInfo() const
{
	return _fAdditionalInfo;
}

void IlwisObjectPtr::SetAdditionalInfo(const String& s)
{
	_sAdditionalInfo = s;
	_fAdditionalInfo = "" != s;
}

void IlwisObjectPtr::SetAdditionalInfoFlag(bool f)
{
	_fAdditionalInfo = f;
}

void IlwisObjectPtr::GetDataFiles(Array<FileName>& afnDat, Array<String>* asSection, Array<String>* asEntry) const
{
	ObjectInfo::Add(afnDat, fnObj, fnObj.sPath());
	if (asSection != 0) {
		(*asSection) &= String();
		(*asEntry) &= String();
	}
}

long IlwisObjectPtr::iDataFileSize() const
{
	Array<FileName> afnData;
	GetDataFiles(afnData);
	return ObjectInfo::iTotalFileSize(afnData);
}

void IlwisObjectPtr::GetObjectDependencies(Array<FileName>&/* afnObjDep*/)
{
}

// Waarom werkt deze function via ObjectInfo???
String IlwisObjectPtr::sDescr() const
{ 
	return ObjectInfo::sDescr(fnObj);
}

String IlwisObjectPtr::sGetDescription() const
{ 
	(const_cast<CCriticalSection*>(&csAccess))->Lock();
	String s = sDescription;
	(const_cast<CCriticalSection*>(&csAccess))->Unlock();
	return s;
}

void IlwisObjectPtr::SetDescription(const String& sDes)
{
	csAccess.Lock();
	sDescription = sDes;
	csAccess.Unlock();
}

bool IlwisObjectPtr::fUsedInOtherObjects(Array<String>* as, bool fStrict, Tranquilizer* trq) const
{ return ObjectInfo::fUsedInOtherObjects(fnObj, as, fStrict, trq); }

DomainInfo IlwisObjectPtr::dminf() const
{
	return DomainInfo();
}

void IlwisObjectPtr::Rename(const FileName& fnNew)
{
	Array<FileName> afnData;
	Array<String> asSection, asEntry;
	GetDataFiles(afnData, &asSection, &asEntry);
	for (unsigned int i=0; i < afnData.iSize(); i++) {
		FileName fn = afnData[i];
		fn.sFile = fnNew.sFile;
		if (File::fExist(fn))
			throw ErrorRename(afnData[i]);
	}  
	csAccess.Lock();
	rename(fnObj.sFullName().c_str(), fnNew.sFullName().c_str());
	const_cast<String&>(fnObj.sFile) = fnNew.sFile;
	const_cast<String&>(fnObj.sExt) = fnNew.sExt;
	fChanged = true;
	csAccess.Unlock();
}

String IlwisObjectPtr::sExpression() const
{
	String s;
	ReadElement("IlwisObjectVirtual", "Expression", s);
	return s;
}

bool IlwisObjectPtr::fDependent() const
{
	return false;
}

bool IlwisObjectPtr::fCalculated() const
{
	return true;
}

void IlwisObjectPtr::BreakDependency()
{
	// empty 
	// should be implemented on a lower level!
}

bool IlwisObjectPtr::fUpToDate() const
{
	if (!fDependent())
		return true;
	if (!fCalculated())
		return false;
	String s;
	ObjectTime tim;
	GetNewestDependentObject(s, tim);
	return s.length() == 0;
}

bool IlwisObjectPtr::fDefOnlyPossible() const
{
	return true;
}

void IlwisObjectPtr::Calc(bool /* fMakeUpToDate*/)
{
	// nothing to do
}

void IlwisObjectPtr::DeleteCalc()
{
	// nothing to do
}

void IlwisObjectPtr::MakeUsable()
{
	if (!fDependent())
		return;
	if (!fCalculated() && !fDefOnlyPossible())
		Calc();
}

void IlwisObjectPtr::MakeUpToDate()
{
	if (!fDependent())
		return;
	if (!fUpToDate())
		Calc(true); // calc and also update
}

void IlwisObjectPtr::DependencyNames(Array<String>& asNames) const
{ 
	ObjectDependency::ReadNames(fnObj, asNames);
}

bool IlwisObjectPtr::fDomainChangeable() const
{
	return _fDomainChangeable;
}

bool IlwisObjectPtr::fValueRangeChangeable() const
{
	return _fValueRangeChangeable;
}

bool IlwisObjectPtr::fGeoRefChangeable() const
{
	return _fGeoRefChangeable;
}

bool IlwisObjectPtr::fExpressionChangeable() const
{
	return _fExpressionChangeable;
}

void IlwisObjectPtr::SetDomainChangeable(bool f)
{
	_fDomainChangeable = f;
}

void IlwisObjectPtr::SetValueRangeChangeable(bool f)
{
	_fValueRangeChangeable = f;
}

void IlwisObjectPtr::SetGeoRefChangeable(bool f)
{
	_fGeoRefChangeable = f;
}

void IlwisObjectPtr::SetExpressionChangeable(bool f)
{
	_fExpressionChangeable = f;
}


bool IlwisObjectPtr::fSystemObject() const
{
	return ObjectInfo::fSystemObject(fnObj);
}

void IlwisObjectPtr::InitName(const FileName& fn)
{
}

void IlwisObjectPtr::GetNewestDependentObject(String& sObjName, ObjectTime& tmNewer) const
{
	Array<FileName> afnChecked;
	afnChecked &= fnObj;
	ObjectDependency::GetNewestDependentObject(fnObj, String(), objtime, sObjName, tmNewer, afnChecked);
}

String IlwisObjectPtr::sNameQuoted(bool fExt, const String& sDirRelative) const
{
	String s = sName(false, sDirRelative);
	if (fnObj.fValid()) {
		s = s.sQuote(false);
		if (fExt)
			s &= fnObj.sExt;
	}  
	return s;
}

void IlwisObjectPtr::RemoveCollectionFromODF(const FileName& fnCol, const String& sEntry)
{
	int iNr;
	ObjectInfo::ReadElement("Collection", "NrOfItems", fnCol, iNr);
	if (iNr > 1)
	{
		int j = 0;
		// Remove the item with Key==sEntry
		// Move all items after that up one position
		for (int i = 0; i < iNr; ++i)
		{
			String sOldKey("Item%d", i);
			if ( sOldKey != sEntry )
			{
				String sText;
				ObjectInfo::ReadElement("Collection", sOldKey.c_str(), fnCol, sText);
				String sNewKey("Item%d", j++);
				ObjectInfo::WriteElement("Collection", sNewKey.c_str(), fnCol, sText);
			}
		}
		// now adjust the number of collections referred to
		ObjectInfo::WriteElement("Collection", "NrOfItems", fnCol, iNr - 1);
		// Remove the now unused collection item from the ODF
		String sKey("Item%d", iNr - 1);
		WritePrivateProfileString("Collection", sKey.c_str(), NULL, fnCol.sFullPath().c_str());
	}
	else
		WritePrivateProfileString("Collection", NULL, NULL, fnCol.sFullPath().c_str());
}

void IlwisObjectPtr::GetObjectStructure(ObjectStructure& os)
{
	if ( os.caGetCommandAction() == ObjectStructure::caCOPY && fDependent() )
	{
		// set the current dir at the correct place. As the current curdir may have changed due to clicking
		// on a target window when copying
		String sCurDir = getEngine()->sGetCurDir();
		getEngine()->SetCurDir( fnObj.sPath());
		if ( os.fBreakDependencies() )
		{
			if ( ! fUpToDate() )
				Calc();
		}
		getEngine()->SetCurDir( sCurDir);
	}
	if ( os.fGetAssociatedFiles() )
	{
		os.AddFile(fnObj, "ForeignFormat", "Filename"); // can be included here, if it is not present it will be ignored anyway
		FileName fnFile;
		ObjectInfo::ReadElement("ForeignFormat", "FileName", fnObj, fnFile);
		// get possible associated other datafiles (e.g. .shp.dbf.etc)
		if ( fnFile.fExist() )
		{
			Array<FileName> arFiles;
			ForeignFormat::GetDataFiles(fnFile, arFiles);
			for(unsigned int i=0; i < arFiles.size(); ++i)
				if ( arFiles[i].fExist())
					os.AddFile(arFiles[i]);
		}
		// Also take care of dependent data files
		if (fDependent() && !os.fBreakDependencies()) 
		{
			int iNr;
			ObjectInfo::ReadElement("ObjectDependency", "NrDepObjects", fnObj, iNr);
			if (iNr != iUNDEF)
				for (int i = 0; i < iNr; ++i)
					os.AddFile(fnObj, "ObjectDependency", String("Object%d", i));
		}
		
	}
	
	os.AddFile(fnObj);
}

void IlwisObjectPtr::DoNotUpdate()
{
	fChanged = false;
}

FileName IlwisObjectPtr::fnCheckPath(const FileName& fn)
{
	int iIndex=0;
	FileName filen = fn;
	FileName fnSearchPath = getEngine()->fnGetSearchPath(iIndex);
	while ( fnSearchPath.sDir != "" )
	{
		filen.Dir(fnSearchPath.sPath());
		if ( filen.fExist() )
			return filen;
		fnSearchPath = getEngine()->fnGetSearchPath(++iIndex);
	}
	
	return fn;
}


IlwisObject::iotIlwisObjectType IlwisObject::iotObjectType(const FileName& fn) 
{
	if ( fn.sExt == "")
		return iotANY;
	
	int iRet = (int)sAllExtensions().find(fn.sExt);
	if ( iRet == -1)
		return iotANY;
	
	if ( (iotIlwisObjectType) (iRet/4) == iotMAPLIST && fn.sSectionPostFix != "" )
		return iotRASMAP;
	
	return (iotIlwisObjectType) (iRet/4);
}

const String IlwisObject::sAllExtensions() 
{
	return FileName::sAllExtensions();
	// note: the order of the extensions is important as this is used in iotObjectType function
}

void IlwisObject::GetAllObjectExtensions(Array<String>& asExt)
{
	asExt.Resize(0);
	String sAllExt = IlwisObject::sAllExtensions();
	
	size_t iFPos = sAllExt.find(".");
	while (iFPos != string::npos)
	{
		size_t iNext = sAllExt.find(".", iFPos + 1);
		if (iNext == string::npos)
			asExt &= sAllExt.sSub(iFPos, sAllExt.length() - iFPos);
		else
			asExt &= sAllExt.sSub(iFPos, iNext - iFPos);
		iFPos = iNext;
	}
}

void IlwisObjectPtr::DoNotStore(bool fValue)
{
	fDoNotStore = fValue;
}

bool IlwisObjectPtr::fStore()
{
	return !fDoNotStore;
}

void IlwisObjectPtr::SetUseAs(bool f)
{
	_fUseAs = f;
}

bool IlwisObjectPtr::fUseAs()
{
	return _fUseAs;
}

bool IlwisObjectPtr::fUsesDependentObjects() const
{
	return false;
}

IlwisObjectVirtual *IlwisObjectPtr::pGetVirtualObject() const
{
	return NULL;
}

ILWIS::Version::BinaryVersion IlwisObjectPtr::getVersionBinary() { return versionBinary; }
String IlwisObjectPtr::getVersionODF() { return versionODF; }

void IlwisObjectPtr::setVersionBinary(ILWIS::Version::BinaryVersion v) {
	versionBinary  =v;
}