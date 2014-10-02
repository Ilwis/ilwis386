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
#ifndef PostGisMaps_H
#define PostGisMaps_H

class DataBaseCollection;

ForeignFormat *CreateQueryObjectPostGis() ;//create query object
ForeignFormat *CreateImportObjectPostGis(const FileName& fnObj, ParmList& pm); //create import object

class PostGisRasterTileset
{
public:
	PostGisRasterTileset(String sConnectionString, String schema, String tableName, String geometryColumn, String id, const GeoRef & gr, String srid, int x_pixels_tile, int y_pixels_tile, double nodata_value, StoreType stPostgres);
	~PostGisRasterTileset();
	void GetLineVal(long iLine, LongBuf& buf, long iFrom, long iNum);
	void GetLineVal(long iLine, RealBuf& buf, long iFrom, long iNum);
	void RenewTiles(long iLine, long iFrom, long iNum);
	static char hex2dec(char * str);
private:
	const GeoRef gr;
	const String srid;
	PostGreSQL * db;
	int iNumTiles;
	long iTop;
	long iBottom;
	long iLeft;
	long iRight;
	const int x_pixels_tile;
	const int y_pixels_tile;
	const String schema;
	const String tableName;
	const String geometryColumn;
	const String id;
	const int headerSize;
	const int bandHeaderSize;
	const double nodata_value;
	const StoreType stPostgres;
};

class PostGisMaps : public PostgreSQLTables
{
public:
	PostGisMaps() ;	
	PostGisMaps(const FileName& fn, const FileName & fnDomAttrTable, ParmList& pm);
	~PostGisMaps();

	static String _export sFormatPostGis(const FileName& fnForeign);
	
	virtual bool _export				 fIsSupported(const FileName& fn, ForeignFormat::Capability dtType = cbANY) const;
	//virtual bool _export         fIsCollection(const String& sForeignObject) const;
	virtual bool _export         fMatchType(const String& sFileName, const String& sType) ;	
	void virtual _export         LoadTable(TablePtr* tbl) ;
	virtual void _export         PutDataInCollection(ForeignCollectionPtr* collection, ParmList& pm);	
	virtual void _export         PutCoordField(const String& sColumn, long iRecord, Coord cValue);
	virtual void _export         GetLineRaw(long iLine, ByteBuf&, long iFrom, long iNum) const;
	virtual void _export         GetLineRaw(long iLine, IntBuf&,  long iFrom, long iNum) const;
	virtual void _export         GetLineRaw(long iLine, LongBuf&, long iFrom, long iNum) const;
	virtual void _export         GetLineVal(long iLine, LongBuf&, long iFrom, long iNum) const;
	virtual void _export         GetLineVal(long iLine, RealBuf&, long iFrom, long iNum) const;
	virtual	long                 iRaw(RowCol) const;
	virtual	long                 iValue(RowCol) const;
	virtual	double               rValue(RowCol) const;
	_export LayerInfo			 GetLayerInfo(ParmList& parms);
	_export void				 IterateLayer(vector<LayerInfo>& objects, bool fCreate);
	void						 Store(IlwisObject obj);
	void                         ReadParameters(const FileName& fnObj, ParmList& pm);
	//_export FileName             createCollectionName(const String& name, ParmList& pm);
	_export void                 getImportFormats(vector<ImportFormat>& formats);
protected:

	//DomainValueRangeStruct       dmTranslateDataTypeToIlwis(String type, bool &fReadOnly, const String& sColName, TablePtr *tbl);
	void						 CreateColumns(PostGreSQL& db, TablePtr *tbl, int iNumColumns, int &iKeyColumn, vector<String>& vDataTypes);
	void						 FillRecords(PostGreSQL& db, TablePtr* tbl, int iNumRecords,const vector<String>& vDataTypes);
	CoordSystem					 getCoordSystem(const FileName& fnBase, const String& srsName);
	void						 createFeatureColumns(TablePtr* tbl);
	void                         PutData(Column& col, long iRec, const String& data, CoordBounds *cb=NULL);
	void                         CreateKeyDomain(const String& sKeyDomain);
	String						 geometryColumn;
	CoordSystem					 csy;
	PostGisRasterTileset*		 rasterTiles;
private:
	static void					 SetStoreType(String pixel_type, LayerInfo & inf, StoreType & stPostgres);
	static double				 getNodataVal(String & val, String & pixel_type);
	int                          x_pixels;
	int                          y_pixels;
	DomainValueRangeStruct       dvrsMap;
};

#endif
