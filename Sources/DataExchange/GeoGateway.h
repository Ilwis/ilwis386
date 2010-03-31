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
#ifndef ILWGEOGATEWAY_H
#define ILWGEOGATEWAY_H

struct GDBLayer_t;

struct PolPart
{
	PolPart() : fExclude(false) {}

	CoordBuf buf;
	list<int> holes;
	bool fExclude;
	CoordBounds cb;
};

typedef void (*SetProjectionParm)(Projection& proj, const ProjInfo_t& projInfo);
typedef void (*SetForeignProjectionParm)(Projection& proj, ProjInfo_t& projInfo);

struct ProjectionConversionFunctions
{
	String sProjectionName;
	SetProjectionParm parmfunc;
	SetForeignProjectionParm forparmfunc;
	
	ProjectionConversionFunctions(const String& sName="", SetProjectionParm func=NULL, SetForeignProjectionParm func2=NULL) : sProjectionName(sName), parmfunc(func), forparmfunc(func2) {}
};

struct FormatInfo
{
	enum Type{ tRASTER, tVECTOR, tTABLE};
	enum ExpProp{ ep8SBIT, ep8UBIT, ep16SBIT, ep16UBIT, epREAL, epBANDS};
	
	FormatInfo() : sDisplayName(""), fUpdate(false), fCreate(false), fImport(true) {}
	FormatInfo(String sDN, String sF, String sEx, Type t, bool fUpd, bool fCr, const String& sExp) : 
		sDisplayName(sDN), sFormatName(sF), sExt(sEx), fUpdate(fUpd), fCreate(fCr), fImport(true), sExportProperties(sExp) {}

	bool fSupports(ExpProp prop);
	
	String sDisplayName;
	String sFormatName;
	String sExt;
	Type tType;
	bool fUpdate;
	bool fCreate;
	bool fImport;
	String sExportProperties;
};

class GeoGatewayFormat : public ForeignFormat
{
	friend UINT PutDataInThread(LPVOID lp);

	public:
	  GeoGatewayFormat();
		GeoGatewayFormat(const FileName& fn, ParmList& pm, ForeignFormat::mtMapType _mtType = ForeignFormat::mtUnknown);
		
		~GeoGatewayFormat();
		static ForeignFormat         *CreateQueryObject();
		static ForeignFormat         *CreateImportObject(const FileName& fnFO, ParmList& pm) ;		
		void                         IterateLayer(vector<LayerInfo>& objects, bool fCreate=true);
		virtual bool _export         fIsCollection(const FileName& fnForeignObject) const;
		virtual bool _export         fMatchType(const FileName& fnFileName, const String& sType);
		virtual void _export         PutDataInCollection(ForeignCollectionPtr* col, ParmList& pm); 
		void                         GetRasterInfo(LayerInfo& inf, int iChannel, bool fBitMap);
		static void                  ReadParameters(const FileName& fnFO, ParmList& pm) ;
		virtual void                 ReadExpression(const FileName& fnFO, ParmList& pm, const String& sExpr) ; // for old style expressions		
		void                         GetGeoRef(GeoRef& grf);
    virtual void _export         GetLineRaw(long iLine, ByteBuf&, long iFrom, long iNum) const ;
    virtual void _export         GetLineRaw(long iLine, IntBuf&,  long iFrom, long iNum) const ;
    virtual void _export         GetLineRaw(long iLine, LongBuf&, long iFrom, long iNum) const ;
    virtual void _export         GetLineVal(long iLine, LongBuf&, long iFrom, long iNum) const ;
    virtual void _export         GetLineVal(long iLine, RealBuf&, long iFrom, long iNum) const ;
    virtual void _export         PutLineRaw(const FileName& fnMap,long iLine, const ByteBuf&, long iFrom, long iNum) ;
    virtual void _export         PutLineRaw(const FileName& fnMap,long iLine, const IntBuf&,  long iFrom, long iNum) ;
    virtual void _export         PutLineRaw(const FileName& fnMap,long iLine, const LongBuf&, long iFrom, long iNum) ;
    virtual void _export         PutLineVal(const FileName& fnMap,long iLine, const LongBuf&, long iFrom, long iNum) ;
    virtual void _export         PutLineVal(const FileName& fnMap,long iLine, const RealBuf&, long iFrom, long iNum) ;   		
		virtual long _export         iRaw(RowCol) const;
		virtual long _export         iValue(RowCol) const;
		virtual double _export       rValue(RowCol) const;
		virtual bool _export				 fIsSupported(const FileName& fn, ForeignFormat::Capability dtType = cbANY) const;

		virtual CoordSystem _export  GetCoordSystem();
		virtual void _export         SetForeignGeoTransformations(const CoordSystem &cs, const GeoRef& grf);

	private:
		void           Init();
		void           SetRecord(long iRec, LayerInfo& li, mtMapType mpType, GDBShape *psShape, GDBLayer psLayer) ;
		void           SetColumns(LayerInfo& obj, mtMapType mpType);
		void           CreateTables(LayerInfo& obj,mtMapType mtType);
		void           ScanLayer(GDBLayer psLayer, vector<LayerInfo>& objects );
		ForeignFormat::mtMapType mtGetShapeType(GDBShape *psShape);
		void				   AddTopology(LayerInfo& obj, CoordBuf& cBuf, const CoordBounds& cb, long iPnt, long iRec, long iTop, bool fNewRing);
		String         sType(ForeignFormat::mtMapType);
		String         sExt(ForeignFormat::mtMapType);
		void           SetTable(LayerInfo& li, ForeignFormat::mtMapType mtType);
		void					 LoadTable(TablePtr* tbl);
		void           LinkLayers();
		int	  				 iCountTopologies(GDBShape *psShape);
		static void		 SetENLL(Projection& proj, const ProjInfo_t& projInfo);
		static void    SetENLLP1P2(Projection& proj, const ProjInfo_t& projInfo);
		static void    SetENLLAS(Projection& proj, const ProjInfo_t& projInfo);
		static void    SetENL(Projection& proj, const ProjInfo_t& projInfo);
		static void    SetENNLS(Projection& proj, const ProjInfo_t& projInfo);
		static void	   SetForeignENLL(Projection& proj, ProjInfo_t& projInfo);
		static void    SetForeignENLLP1P2(Projection& proj, ProjInfo_t& projInfo);
		static void    SetForeignENLLAS(Projection& proj, ProjInfo_t& projInfo);
		static void    SetForeignENL(Projection& proj, ProjInfo_t& projInfo);
		static void    SetForeignENNLS(Projection& proj, ProjInfo_t& projInfo);
		String				 sForeignIlwisEllipsoidCode(const String& sEllips);
		String         sGetForeignDatumCode(const String& sDatumName, String& sArea);
		bool           fReUseExisting(const FileName& fn);
		void           CreateLayer(vector<LayerInfo> &li);

		GDBLayer       OpenLayer(int iType=SEG_UNKNOWN);
		void           CreateLineBuffer(long iSize);
		void					 CreateRasterLayer(int iMaxChannels, bool fBitMap);
		void					 AddDomainValues(vector <set<String> > &values);
		void					 CreateDomains(vector < set<String> > &values);
		void           ScanValues(GDBLayer psLayer, vector<LayerInfo>& objects, vector < set<String> > &values);
		void           BasicColumnInfo(GDBLayer psLayer);
		void           CreateBasicLayer(vector<LayerInfo>& objects);
		void           StoreAttrTables(vector<LayerInfo>& objects);
		void           ImportRasterMap(const FileName& fnRasMap, Map& mp ,LayerInfo& li, int iChannel);
		void					 GetRasterLayer(int iLayerIndex, Map& mp, Array<FileName>& arMaps, GeoRef& grf, 	Domain& dm,  bool fBitMap);
		void           GetBitValues(unsigned char * buffer, LongBuf& buf, long iNum) const;

		void           ReadLayers();
		void           ReadAllLayers();
		void           ReadData(GDBLayer psLayer, vector<LayerInfo>& objects, int &iRec, int &iPoint, int &iSeg, int &iPol, int &iTabRec);
		void           LoadTableFromLayer(GDBLayer psLayer, vector<Column> &columns, int &iPoint);
		bool           fCombinePossible();
		FileName       fnCreateFileName(const FileName& fnBase);

		String         sIlwisEllipsoid(const String& sGDBEllips);
		void           GetDatumNameArea(const String& sDatumCode, String& sName, String& sArea);

		static UINT    PutDataInThread(LPVOID lp);
		void           ReadForeignFormat(ForeignCollectionPtr* col);
		void					 AddNewFiles(const FileName& fnNew);
		int            iFormatID(const String& sFormat) const;
		void					 InitGeoGateway();
		FormatInfo		 fiGetFormatInfo(const String sExt) const;
		void					 SetEllipsDatum(CoordSystemViaLatLon *csv, const String& sEllipsDatum);
		void					 CleanUp();
		void           InitExportMaps(const FileName& fn, ParmList& pm);
		void           DetermineOutputType(const FileName& fnMp, int& iBands, int& iDataType)		;

		int iLayer;
		FILE *fdp[3] ; //multiple are only used when exporting (bands)
		unsigned char *buffer;
		bool fCombine;
		bool fImport;
		bool fExport;
		bool fShowCollection;
		bool f4BytesInt; // used for raster; 
		FileName fnBaseOutputName;
		list<CoordSystem> addedCsy;
		map<String, ProjectionConversionFunctions> mpCsyConvers; // coordsystem conversion functions
		static map<int, FormatInfo> mapFormatInfo;
		int iGDBID; // export datat type, id as in ccltask.h
		list<FileName> fnUsedFileNames; // files do not yet exist on disk, but uniqueness must be guaranteed
		static CCriticalSection m_CriticalSection;

		//creation info
};

#endif*/
