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
#ifndef FOREIGNFORMAT_H
#define FOREIGNFORMAT_H

#include "Engine\DataExchange\ForeignCollection.h"
#include "Engine\SpatialReference\Gr.h"
#include <set>

typedef String (*CheckFormat)(const FileName&);
typedef ForeignFormat* (*CreateFunction1)(const FileName& fnObj, ParmList& pm);
typedef ForeignFormat* (*CreateFunction2)();

struct ImportFormat{
	enum IFType{ifRaster = 1, ifTable = 2, ifSegment = 4, ifPoint = 8,ifPolygon = 16,ifUnknown=32};

	ImportFormat() { buttonText=name=shortName=ext=provider=method="";type=ifUnknown;ui=NULL;useasSuported=false;}
	bool operator<(const ImportFormat& f) { return name < f.name;}
	String name;
	String shortName;
	int type;
	String ext;
	String provider; // GDAL;ILWIS etc...
	String method;
	void *ui; //this pointer must be a deravitave of FieldGroup, but because I don't want to create a link dependency we use a void* pointer
	String buttonText;
	bool useasSuported;
	String command; // alternative for going the document way; just a direct command
};

struct ImportDriver{
	String driverName;
	vector<ImportFormat> formats; 
};

typedef  void (*ImportDrivers)(vector<ImportDriver>& drivers);

struct LayerInfo
{
	LayerInfo() { iShapes=iTopologies=0;}
	Table                           tbl;
	Table                           tblTop; // topology table for polygon maps
	Table                           tblattr;
	DomainValueRangeStruct          dvrsMap;
	int                             iShapes;
	int                             iTopologies;
	FileName                        fnObj;
	CoordSystem                     csy;
	String                          sExpr;
	FileName                        fnForeign;
	CoordBounds                     cbMap; // as GDB returns it
	CoordBounds                     cbActual;	// as the actual extents of the elements indicate
	GeoRef                          grf;
	int								layerIndex;
};

struct clmInfo
{
	clmInfo() {}
	clmInfo(DomainValueRangeStruct dv, const String& sName) : dvrs(dv), sColName(sName) {}
	DomainValueRangeStruct dvrs;
	String sColName;
};


class ForeignFormat
{
	public:
		enum mtMapType{mtPointMap, mtSegmentMap, mtPolygonMap, mtRasterMap, mtTable, mtTopology, mtUnknown};
		enum Capability{cbEXPBYTE, cbEXPUBYTE, cbEXPSHORT, cbEXPUSHORT, cbEXPLONG, cbEXPULONG, cbEXPREAL, 
			              cbEDIT, cbCREATE, cbIMPORT,
			              cbANY};

		_export ForeignFormat();
		_export ForeignFormat(const String& fn, mtMapType _mtType=mtUnknown);
		_export virtual ~ForeignFormat();

		virtual bool _export		 fIsSupported(const FileName& fn, Capability type = cbANY) const { return false; }
		virtual bool _export         fIsCollection(const String& sForeignObject) const { return true; }
		virtual bool _export         fMatchType(const String& fnFileName, const String& sType) { return true; }
		virtual void _export         IterateLayer(vector<LayerInfo>& objects, bool fCreate=true) {}
		virtual void _export         PutDataInCollection(ForeignCollectionPtr* collection, ParmList& pm) {}
		void virtual _export         LoadTable(TablePtr* tbl) {}
		void virtual _export         Init() {}
		virtual FileName _export	 createCollectionName(const String& desc, ParmList& pm) { return desc; }
		virtual void _export         ReadParameters(const FileName& fnFO, ParmList& pm) ;

		static ForeignFormat _export *Create(const FileName& fnObj, ParmList& pm );
		static ForeignFormat _export *Create(const String& sMethod)		;

//		static bool	                 fFunctionsInitialized() { return mpCreateFunction1.size() != 0; }
//		static void                  AddStaticFunctions(const String& sMethod, CreateFunction1 func1,  CreateFunction2 func2, CheckFormat func3 = NULL);
		virtual void _export         Store(IlwisObject ob);
		bool                         GetTime(FILETIME& LastWrite);
		LayerInfo _export            GetLayerInfo(BaseMapPtr *ptr, ForeignFormat::mtMapType mtType, bool fCreate);
		virtual LayerInfo _export	 GetLayerInfo(ParmList& parms);

		virtual void _export         GetLineRaw(long iLine, ByteBuf&, long iFrom, long iNum) const {};
		virtual void _export         GetLineRaw(long iLine, IntBuf&,  long iFrom, long iNum) const {};
		virtual void _export         GetLineRaw(long iLine, LongBuf&, long iFrom, long iNum) const {};
		virtual void _export         GetLineVal(long iLine, LongBuf&, long iFrom, long iNum) const {};
		virtual void _export         GetLineVal(long iLine, RealBuf&, long iFrom, long iNum) const {};
		virtual void _export         PutLineRaw(const FileName& fnMap, long iLine, const ByteBuf&, long iFrom, long iNum) {};
		virtual void _export         PutLineRaw(const FileName& fnMap, long iLine, const IntBuf&,  long iFrom, long iNum) {};
		virtual void _export         PutLineRaw(const FileName& fnMap, long iLine, const LongBuf&, long iFrom, long iNum) {};
		virtual void _export         PutLineVal(const FileName& fnMap, long iLine, const LongBuf&, long iFrom, long iNum) {};
		virtual void _export         PutLineVal(const FileName& fnMap, long iLine, const RealBuf&, long iFrom, long iNum) {};   
		virtual void _export         PutStringField(const String& sColumn, long iRecord, const String& sValue) {};
		virtual void _export         PutUByteField(const String& sColumn, long iRecord, unsigned char iValue) {};		
		virtual void _export         PutByteField(const String& sColumn, long iRecord, char iValue) {};				
		virtual void _export         PutLongField(const String& sColumn, long iRecord, long iValue) {};						
		virtual void _export         PutULongField(const String& sColumn, long iRecord, unsigned long iValue) {};								
		virtual void _export         PutRealField(const String& sColumn, long iRecord, double rValue) {};								
		virtual void _export         PutBoolField(const String& sColumn, long iRecord, bool fValue) {};	
		virtual void _export         PutCoordField(const String& sColumn, long iRecord, Coord cValue) {};										
		virtual	long _export         iRaw(RowCol) const   { return iUNDEF;}
		virtual	long _export         iValue(RowCol) const { return iUNDEF; }
		virtual	double _export       rValue(RowCol) const { return rUNDEF; }
		virtual CoordSystem _export  GetCoordSystem();
		virtual void _export         SetForeignGeoTransformations(const CoordSystem &cs, const GeoRef& grf) {}
		virtual void                 GetRasterInfo(LayerInfo& inf, String  sChannel="") {};
		static void _export          GetDataFiles(const FileName& fnFor, vector<FileName>& fnFiles);
		static String _export        sFindDefaultMethod(const FileName& fnForeignFile);
		static String _export        sFindFormatName(const String& sFO) ;
		_export Domain               *GetTableDomain();
		String _export               sGetForeignInput();
		FileName _export			 fnGetForeignFile(); //convenience method;
		
;
		ForeignFormat::mtMapType _export  AssociatedMapType();

protected:
		bool fForeignFileTimeChanged(const FileName& fnObj);
		
		Column                   columns[7][8];
		String                   sForeignInput;
		FileName				 fnOutputName;
		set<String>              AddedFiles;		
		vector<clmInfo>          attrDomains;
		mtMapType                mtLoadType;
		long                     iTopRecord;
		String                   m_sTitle;
		ForeignCollectionPtr     *collection;
		Tranquilizer             *trq;
		Domain					 *dmKey;
		String                   sKeyColumn;
		bool					 fUseAs;
		String					 sMethod;
		
private:		
		static list<CheckFormat> lstCheckFormatFunctions;           
		static map<String, list<String> > mapFormatParts;		
};

#endif

