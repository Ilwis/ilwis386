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
	Last change:  WK   21 Aug 98   12:10 pm
*/

#ifndef ILWISOBJ_H
#define ILWISOBJ_H
#include "Headers\toolspch.h"
#include "Engine\Base\DataObjects\Version.h"
#include "Engine\Base\Algorithm\Basemat.h"
#include "Engine\Base\Algorithm\Realmat.h"
#include "Engine\Base\File\ElementContainer.h"

//#define Polygon ILWIS::Polygon

class IlwisObject;
class IlwisObjectPtr;
class IlwisObjectPtrList;
class Domain;
class DomainPtr;
class Representation;
class RepresentationPtr;
class RepresentationClass;
class RepresentationValue;
class GeoRef;
class GeoRefPtr;
class CoordSystem;
class CoordSystemPtr;
class BaseMap;
class BaseMapPtr;
class MapPtr;
class SegmentMap;
class SegmentMapPtr;
class PolygonMap;
class PolygonMapPtr;
class PointMap;
class PointMapPtr;
class Table;
class TablePtr;
class Column;
class ColumnPtr;
class Map;
class MapList;
class MapListPtr;
class MapView;
class MapViewPtr;
class ValueRange;
class ValueRangeInt;
class ValueRangeReal;
class DomainInfo;
class AnnotationText;
class AnnotationTextPtr;
class ObjectStructure;
class IlwisObjectVirtual;

#include "Engine\Base\File\objinfo.h"

#undef IMPEXP
#ifdef ILWISENGINE   
#define IMPEXP __declspec(dllexport)
#else
#define IMPEXP __declspec(dllimport)
#endif

//! Base class for all Ilwis objects
//! IlwisObject has a pointer to IlwisObjectPtr

class IMPEXP IlwisObjectPtr
{
friend class IlwisObject;
friend class ObjectInfo;
friend class IlwisObjectVirtual;

public:
	                
	virtual                ~IlwisObjectPtr();

	String                 sAdditionalInfo() const;
	String                 sDescr() const;  // Waarom bestaat deze??? omdat hij gebruikt wordt! In CoordsysformulaView (waarom?)
	String        		   sGetDescription() const;
	void        		   SetDescription(const String& sDes);
	static String          sParseFunc(const String& sExpr);
	virtual String         sName(bool fExt = false, const String& sDirRelative = "") const;
	virtual String         sNameQuoted(bool fExt = false, const String& sDirRelative = "") const;
	virtual String         sType() const;
	virtual String         sTypeName() const;
	virtual String         sObjectSection() const;
	virtual String         sExpression() const;// checks if object is dependent by checking the file
	bool                   fReadOnly() const;
	bool                   fDomainChangeable() const;
	bool                   fValueRangeChangeable() const;
	bool                   fGeoRefChangeable() const;
	bool                   fExpressionChangeable() const;
	bool                   fSystemObject() const; 
	static FileName        fnCheckPath(const FileName& fn);
	bool                   fUsedInOtherObjects(Array<String>* as = 0, bool fStrict = true, Tranquilizer* trq = 0) const;
	bool                   fUpToDate() const; // returns true if an up to date calculate result exists
	bool                   fAdditionalInfo() const;
	bool                   operator == (const IlwisObjectPtr& op) const
							 { return fEqual(op); }
	bool                   operator != (const IlwisObjectPtr& op) const
							 { return !fEqual(op); }
	static bool            fParseParm(const String& sExpr, Array<String>& as);
	virtual bool           fDataReadOnly() const;// default: returns fReadOnly() || fDependent()
	virtual bool           fDefOnlyPossible() const; // returns true if data can be retrieved without complete calculation (on the fly)        
	virtual bool           fDependent() const; // reads expression directly from file 
	virtual bool           fUsesDependentObjects() const; // containers could use dependent objects
	virtual bool           fCalculated() const; // returns true if a calculated result exists
	virtual bool           fEqual(const IlwisObjectPtr&) const;
	void                   MakeUpToDate(); // calc if result is not up to date anymore
	void                   MakeUsable();  // calc only if !fCalculated() && !fDefinitionOnlyPossible()
	void                   SetDomainChangeable(bool f=true);
	void                   SetValueRangeChangeable(bool f=true);
	void                   SetGeoRefChangeable(bool f=true);
	void                   SetExpressionChangeable(bool f=true);
	void                   SetAdditionalInfo(const String& s);
	void                   SetAdditionalInfoFlag(bool f=true);
	void                   ReadBaseInfo(const char* sSection);        
	void                   WriteBaseInfo(const char* sSection);
	virtual void           Updated();
	virtual void           UpdateTime(); // calls Updated and writes time to object def file
	virtual void           GetObjectDependencies(Array<FileName>& afnObjDep);
	virtual void           GetNewestDependentObject(String& sObjName, ObjectTime& tmNewer) const;        
	virtual void           Store();
	void                   SetReadOnly(bool f = true);
	virtual void           GetDataFiles(Array<FileName>& afnDat, Array<String>* asSection=0, Array<String>* asEntry=0) const;
	virtual void           DependencyNames(Array<String>& asNames) const; // read from file
	virtual void           Rename(const FileName& fnNew);
	virtual void           Calc(bool fMakeUpToDate=false); // calculates the result
	virtual void           DeleteCalc(); // delete calculated result     
	virtual void           BreakDependency(); // only keeps calculated result, if nothing has been calculated it calculates first
	virtual void           InitName(const FileName& fn);
	virtual DomainInfo     dminf() const;
	virtual void           GetObjectStructure(ObjectStructure& os);
	virtual void           DoNotUpdate();
	void                   DoNotStore(bool fValue);
	bool                   fUseAs();
	void                   SetUseAs(bool fUseAs);
	bool                   fStore();
	long                   iDataFileSize() const;
	static short           iParseParm(const String& sExpr, Array<String>& as);
	FileName               fnCopy(const FileName& fn, bool fCopyAsMember) const;
	virtual IlwisObjectVirtual     *pGetVirtualObject() const;
	virtual void SetFileName(const FileName& fn)
		{ const_cast<ElementContainer&>(fnObj) = fn; }
	ILWIS::Version::BinaryVersion getVersionBinary();
	String getVersionODF();
	void					setVersionBinary(const ILWIS::Version::BinaryVersion);

	static void RemoveCollectionFromODF(const FileName& fnCol, const String& sEntry);

	long                   iReadElement(const char* sSection, const char* sEntry) const;
	double                 rReadElement(const char* sSection, const char* sEntry) const;
	int                    ReadElement(const char* sSection, const char* sEntry, String& sValue) const     //String
							 { return ObjectInfo::ReadElement(sSection, sEntry, fnObj, sValue); }
	int                    ReadElement(const char* sSection, const char* sEntry,bool& fValue) const        //bool
							 { return ObjectInfo::ReadElement(sSection, sEntry, fnObj, fValue); }
	int                    ReadElement(const char* sSection, const char* sEntry, int& iValue) const        //int
							 { return ObjectInfo::ReadElement(sSection, sEntry, fnObj, iValue); }
	int                    ReadElement(const char* sSection, const char* sEntry,long& iValue) const        //long
							 { return ObjectInfo::ReadElement(sSection, sEntry, fnObj, iValue); }
	int                    ReadElement(const char* sSection, const char* sEntry, double& rValue) const     //double
							 { return ObjectInfo::ReadElement(sSection, sEntry, fnObj, rValue); }
	int                    ReadElement(const char* sSection, const char* sEntry, RangeInt& rng) const      //RangeInt
							 { return ObjectInfo::ReadElement(sSection, sEntry, fnObj, rng); }
	int                    ReadElement(const char* sSection, const char* sEntry, RangeReal& rng) const     //RangeReal
							 { return ObjectInfo::ReadElement(sSection, sEntry, fnObj, rng); }
	int                    ReadElement(const char* sSection, const char* sEntry, ILWIS::TimeInterval& interval) const     //Time interval
							 { return ObjectInfo::ReadElement(sSection, sEntry, fnObj, interval); }
	int                    ReadElement(const char* sSection, const char* sEntry, FileName& fn) const       //FileName
							 { return ObjectInfo::ReadElement(sSection, sEntry, fnObj, fn); }
	int                    ReadElement(const char* sSection, const char* sEntry, ObjectTime& tim) const          //ObjectTime
							 { return ObjectInfo::ReadElement(sSection, sEntry, fnObj, tim); }
	int                    ReadElement(const char* sSection, const char* sEntry, Domain& dm) const         //Domain
							 { return ObjectInfo::ReadElement(sSection, sEntry, fnObj, dm); }
	int                    ReadElement(const char* sSection, const char* sEntry, Map& mp) const            //Map
							 { return ObjectInfo::ReadElement(sSection, sEntry, fnObj, mp); }
	int                    ReadElement(const char* sSection, const char* sEntry, MapList& ml) const        //MapList
							 { return ObjectInfo::ReadElement(sSection, sEntry, fnObj, ml); }
	int                    ReadElement(const char* sSection, const char* sEntry, SegmentMap& mp) const     //SegmentMap
							 { return ObjectInfo::ReadElement(sSection, sEntry, fnObj, mp); }
	int                    ReadElement(const char* sSection, const char* sEntry, PolygonMap& mp) const     //PolygonMap
							 { return ObjectInfo::ReadElement(sSection, sEntry, fnObj, mp); }
	int                    ReadElement(const char* sSection, const char* sEntry, PointMap& mp) const       //PointMap
							 { return ObjectInfo::ReadElement(sSection, sEntry, fnObj, mp); }
	int                    ReadElement(const char* sSection, const char* sEntry, Table& tbl) const         //Table
							 { return ObjectInfo::ReadElement(sSection, sEntry, fnObj, tbl); }
	int                    ReadElement(const char* sSection, const char* sEntry, Representation& rpr) const    //Representation
							 { return ObjectInfo::ReadElement(sSection, sEntry, fnObj, rpr); }
	int                    ReadElement(const char* sSection, const char* sEntry, GeoRef& gr) const         //Georef
							 { return ObjectInfo::   ReadElement(sSection, sEntry, fnObj, gr); }
	int                    ReadElement(const char* sSection, const char* sEntry, CoordSystem& cs) const    //Coordsystem
							 { return ObjectInfo::ReadElement(sSection, sEntry, fnObj, cs); }
	int                    ReadElement(const char* sSection, const char* sEntry, RealArray& arr) const     //RealArray
							 { return ObjectInfo::ReadElement(sSection, sEntry, fnObj, arr); }
	int                    ReadElement(const char* sSection, const char* sEntry, RealMatrix& mat) const    //RealMatrix
							 { return ObjectInfo::ReadElement(sSection, sEntry, fnObj, mat); }
	int                    ReadElement(const char* sSection, const char* sEntry, IntMatrix& mat) const     //IntMAtrix
							 { return ObjectInfo::ReadElement(sSection, sEntry, fnObj, mat); }
	int                    ReadElement(const char* sSection, const char* sEntry, Coord& crd) const         //Coord
							 { return ObjectInfo::ReadElement(sSection, sEntry, fnObj, crd); }
	int                    ReadElement(const char* sSection, const char* sEntry, LatLon& ll) const         //LAtLon
							 { return ObjectInfo::ReadElement(sSection, sEntry, fnObj, ll); }
	int                    ReadElement(const char* sSection, const char* sEntry, RowCol& rc) const         //RowCol
							 { return ObjectInfo::ReadElement(sSection, sEntry, fnObj, rc); }
	int                    ReadElement(const char* sSection, const char* sEntry, MinMax& mm) const         //MinMax
							 { return ObjectInfo::ReadElement(sSection, sEntry, fnObj, mm); }
	int                    ReadElement(const char* sSection, const char* sEntry, CoordBounds& cb) const    //CoordBounds
							 { return ObjectInfo::ReadElement(sSection, sEntry, fnObj, cb); }
	int                    ReadElement(const char* sSection, const char* sEntry, ValueRange& vr) const     //ValueRange
							 { return ObjectInfo::ReadElement(sSection, sEntry, fnObj, vr); }
	int                    ReadElement(const char* sSection, const char* sEntry, Color& col) const         //Color
							 { return ObjectInfo::ReadElement(sSection, sEntry, fnObj, col); }
	int                    ReadElement(const char* sSection, const char* sEntry, Filter& flt)
							 { return ObjectInfo::ReadElement(sSection, sEntry, fnObj, flt); }
	int                    ReadElement(const char* sSection, const char* sEntry, Classifier& clf)
							 { return ObjectInfo::ReadElement(sSection, sEntry, fnObj, clf); }
	int                    ReadElement(const char* sSection, const char* sEntry, SampleSet& sms)
							 { return ObjectInfo::ReadElement(sSection, sEntry, fnObj, sms); }

	bool                   WriteElement(const char* sSection, const char* sEntry, const char* sValue)      //char*
							 { return ObjectInfo::WriteElement(sSection, sEntry, fnObj, sValue); }
	bool                   WriteElement(const char* sSection, const char* sEntry, const String& sValue)    //String
							 { return ObjectInfo::WriteElement(sSection, sEntry, fnObj, sValue); }
	bool                   WriteElement(const char* sSection, const char* sEntry, bool fValue)             //bool
							 { return ObjectInfo::WriteElement(sSection, sEntry, fnObj, fValue); }
	bool                   WriteElement(const char* sSection, const char* sEntry, short iValue)            //short
							 { return ObjectInfo::WriteElement(sSection, sEntry, fnObj, (long)iValue); }
	bool                   WriteElement(const char* sSection, const char* sEntry, int iValue)              //int
							 { return ObjectInfo::WriteElement(sSection, sEntry, fnObj, (long)iValue); }
	bool                   WriteElement(const char* sSection, const char* sEntry, long iValue)             //long
							 { return ObjectInfo::WriteElement(sSection, sEntry, fnObj, iValue); }
	bool                   WriteElement(const char* sSection, const char* sEntry, double rValue)           //double
							 { return ObjectInfo::WriteElement(sSection, sEntry, fnObj, rValue); }
	bool                   WriteElement(const char* sSection, const char* sEntry, const RangeInt& rng)     //RangeInt
							 { return ObjectInfo::WriteElement(sSection, sEntry, fnObj, rng); }
	bool				   WriteElement(const char* sSection, const char* sEntry, const ILWIS::TimeInterval& interval)     //Time interval
							 { return ObjectInfo::WriteElement(sSection, sEntry, fnObj, interval); }
	bool                   WriteElement(const char* sSection, const char* sEntry, const RangeReal& rng)    //RangeReal
							 { return ObjectInfo::WriteElement(sSection, sEntry, fnObj, rng); }
	bool                   WriteElement(const char* sSection, const char* sEntry, const ObjectTime& tim)         //ObjectTime
							 { return ObjectInfo::WriteElement(sSection, sEntry, fnObj, tim); }
	bool                   WriteElement(const char* sSection, const char* sEntry, const FileName& fn)      //FileName
							 { return ObjectInfo::WriteElement(sSection, sEntry, fnObj, fn); }
	bool                   WriteElement(const char* sSection, const char* sEntry, const IlwisObject& obj)  //IlwisObject
							 { return ObjectInfo::WriteElement(sSection, sEntry, fnObj, obj); }
	bool                   WriteElement(const char* sSection, const char* sEntry, const Domain& dm)        //Domain
							  { return ObjectInfo::WriteElement(sSection, sEntry, fnObj, dm); }
	bool                   WriteElement(const char* sSection, const char* sEntry, const Representation& rpr)   //Representation
							  { return ObjectInfo::WriteElement(sSection, sEntry, fnObj, rpr); }
	bool                   WriteElement(const char* sSection, const char* sEntry, const GeoRef& gr)        //Georef
							  { return ObjectInfo::WriteElement(sSection, sEntry, fnObj, gr); }
	bool                   WriteElement(const char* sSection, const char* sEntry, const RealArray& arr)    //RealArray
							  { return ObjectInfo::WriteElement(sSection, sEntry, fnObj, arr); }
	bool                   WriteElement(const char* sSection, const char* sEntry, const RealMatrix& mat)   //RealMAtrix
							  { return ObjectInfo::WriteElement(sSection, sEntry, fnObj, mat); }
	bool                   WriteElement(const char* sSection, const char* sEntry, const IntMatrix& mat)    //IntMatrix
							  { return ObjectInfo::WriteElement(sSection, sEntry, fnObj, mat); }
	bool                   WriteElement(const char* sSection, const char* sEntry, const Coord& crd)        //Coord
							  { return ObjectInfo::WriteElement(sSection, sEntry, fnObj, crd); }
	bool                   WriteElement(const char* sSection, const char* sEntry, const LatLon& ll)        //Latlon
							  { return ObjectInfo::WriteElement(sSection, sEntry, fnObj, ll); }
	bool                   WriteElement(const char* sSection, const char* sEntry, const RowCol& rc)        //RowCol
							  { return ObjectInfo::WriteElement(sSection, sEntry, fnObj, rc); }
	bool                   WriteElement(const char* sSection, const char* sEntry, const MinMax& mm)        //MinMax
							  { return ObjectInfo::WriteElement(sSection, sEntry, fnObj, mm); }
	bool                   WriteElement(const char* sSection, const char* sEntry, const CoordBounds& cb)   //CoordBounds
							  { return ObjectInfo::WriteElement(sSection, sEntry, fnObj, cb); }
	bool                   WriteElement(const char* sSection, const char* sEntry, const ValueRange& vr)    //ValueRange
							  { return ObjectInfo::WriteElement(sSection, sEntry, fnObj, vr); }
	bool                   WriteElement(const char* sSection, const char* sEntry, Color col)               //Color
							  { return ObjectInfo::WriteElement(sSection, sEntry, fnObj, col); }
//------Data members
    
	bool                   fChanged;
	int                    iRef;
	const ElementContainer fnObj;
	String                 sDescription;
	ObjectTime                   objtime;
	bool                   fErase;
	bool                   fUpdateCatalog;

protected:
	IlwisObjectPtr();
	IlwisObjectPtr(const FileName& filename, bool fCreate=false, const char* pcDefExt=0);

	bool                   _fReadOnly;
	CCriticalSection       csAccess, csCalc;
	ILWIS::Version::BinaryVersion   versionBinary;
	String					versionODF;

private:
	String                 _sAdditionalInfo;
	bool                   _fAdditionalInfo;
	bool                   _fDomainChangeable;
	bool                   _fValueRangeChangeable;
	bool                   _fGeoRefChangeable;
	bool                   _fExpressionChangeable;
	bool                   fDoNotStore;
	bool                   _fUseAs;
};

class IlwisObjectPtrList: public DListP<IlwisObjectPtr>
{
public:
	IlwisObjectPtrList() {}

	IlwisObjectPtr _export *pGet(const FileName&) const;
	void Add(IlwisObjectPtr*);
	void Remove(IlwisObjectPtr*);
	CCriticalSection csAccess;
};

//! IlwisObject is a smart pointer to an IlwisObjectPtr
class IMPEXP IlwisObject
{
friend class Array<IlwisObject*>;

public:
	// note: the order of the enums is important as this is used in iotObjectType function
	//! sequence of iotIlwisObjectType is the same as in FileName::sAllExtensions()
	enum iotIlwisObjectType{
		iotRASMAP, iotPOLYGONMAP, iotSEGMENTMAP, iotPOINTMAP, iotTABLE, 
		iotMAPLIST, iotOBJECTCOLLECTION, iotMAPVIEW, iotLAYOUT, iotANNOTATIONTEXT, iotGRAPH, 
		iotDOMAIN, iotREPRESENTATION, iotGEOREF, iotCOORDSYSTEM, 
		iotHISTRAS, iotHISTPOL, iotHISTSEG, iotHISTPNT, 
		iotSAMPLESET, iotSTEREOPAIR, iotCRITERIATREE, 
		iotTABLE2DIM, iotMATRIX, iotFILTER, iotFUNCTION, iotSCRIPT, 
		iotANY};

	static       iotIlwisObjectType iotObjectType(const FileName& fn) ;
	static const String             sAllExtensions();                             // get all object extensions as string
	static       void               GetAllObjectExtensions(Array<String>& asExt); // get all object extensions in string array

	IlwisObject(const IlwisObject& obj) : list(obj.list), ptr(0)
			{ SetPointer(obj.pointer()); }
	~IlwisObject();

	bool                fEqual(const IlwisObject& obj) const;
	bool                fEqual(const IlwisObjectPtr* obj) const;
	bool                operator==(const IlwisObject& obj) const
							{ return fEqual(obj); }
	bool                operator!=(const IlwisObject& obj) const
							{ return !fEqual(obj); }
	void                SetPointer(IlwisObjectPtr*);
	IlwisObjectPtr*     pointer() const 
							{ return ptr; }
	IlwisObjectPtr*     operator->() const 
							{ return pointer(); }
	bool                fValid() const 
							{ return ptr != 0; }
	static IlwisObject  obj(const FileName&);
	static IlwisObject  objInvalid();

	//The operator is probably not correct as the list is not assigned. 
	// It seems to work OK (as far as we know). In the future a proper solution must be found
	void                operator=(const IlwisObject& obj)
                            { SetPointer(obj.pointer()); }
protected:
	IlwisObject(IlwisObjectPtrList& lst);
	IlwisObject(IlwisObjectPtrList&, const FileName&);
	IlwisObject(IlwisObjectPtrList&, IlwisObjectPtr*);
	IlwisObject();

private:

	IlwisObjectPtrList&     list;
	IlwisObjectPtr*         ptr;
};
class DATEXPORT Ellipsoid;

#define IOTYPE(name) IlwisObject::iotObjectType(name)
#define IOTYPEBASEMAP(name) ( IOTYPE(name) == IlwisObject::iotRASMAP || IOTYPE(name) == IlwisObject::iotPOLYGONMAP || IOTYPE(name) == IlwisObject::iotPOINTMAP || IOTYPE(name) == IlwisObject::iotSEGMENTMAP)
#define IOTYPEFEATUREMAP(name) ( IOTYPE(name) == IlwisObject::iotPOLYGONMAP || IOTYPE(name) == IlwisObject::iotPOINTMAP || IOTYPE(name) == IlwisObject::iotSEGMENTMAP)
typedef IlwisObject::iotIlwisObjectType IObjectType;

#endif // ILWISOBJ_H



