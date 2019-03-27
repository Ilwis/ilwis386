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

#pragma once

class OpenStreetMapLayerInfo;
class OpenStreetMapFormat;
class OpenStreetMapCollectionPtr;

typedef void(__stdcall *GDALCloseFunc)(GDALDatasetH);
typedef void(__stdcall *GDALAllRegisterFunc)();
typedef GDALDatasetH (__stdcall *GDALOpenFunc) (const char *, GDALAccess );
typedef GDALRasterBandH (__stdcall *GDALGetRasterBandFunc) (GDALDatasetH, int);
typedef OGRSpatialReferenceH (__stdcall *OSRNewSpatialReferenceFunc)( const char *);
typedef OGRErr (__stdcall *OSRImportFromEPSGFunc)( OGRSpatialReferenceH, int );
typedef int (*OSRIsProjectedFunc)( OGRSpatialReferenceH );
typedef CPLErr 	(__stdcall *GDALRasterIOFunc)(GDALRasterBandH hRBand, GDALRWFlag eRWFlag, int nDSXOff, int nDSYOff, int nDSXSize, int nDSYSize, void *pBuffer, int nBXSize, int nBYSize, GDALDataType eBDataType, int nPixelSpace, int nLineSpace);
typedef const char * (__stdcall *OSRGetAttrValueFunc)( OGRSpatialReferenceH hSRS, const char * pszName, int iChild /* = 0 */ );
typedef void (*CPLPushFinderLocationFunc)( const char * );
typedef int (*VSIFCloseLFunc)( FILE * );
typedef FILE* (*VSIFileFromMemBufferFunc)( const char *, GByte *, vsi_l_offset ,int  );


//typedef void (OpenStreetMapFormat::*SetProjectionParm2)(Projection& prj, const void * srs);

ForeignFormat *CreateImportObjectOpenStreetMap(const FileName& fnFO, ParmList& pm);
ForeignFormat *CreateQueryObjectOpenStreetMap();

class RemoteObject;

class _export OpenStreetMapFormat : public ForeignFormat {
public:

	OpenStreetMapFormat();
	~OpenStreetMapFormat();
	OpenStreetMapFormat(const FileName& fn, ParmList& pm);
	virtual bool          fIsCollection(const String& sForeignObject) const;
	virtual bool          fMatchType(const String& fnFileName, const String& sType);
	//virtual void          PutDataInCollection(ForeignCollectionPtr* col, ParmList& pm);
	virtual void		  GetRasterInfo(LayerInfo& inf, String sChannel);
	virtual void          GetLineRaw(long iLine, ByteBuf&, long iFrom, long iNum) const ;
	virtual void          GetLineRaw(long iLine, IntBuf&,  long iFrom, long iNum) const ;
	virtual void          GetLineRaw(long iLine, LongBuf&, long iFrom, long iNum) const ;
	virtual void          GetLineVal(long iLine, LongBuf&, long iFrom, long iNum) const ;
	virtual void          GetLineVal(long iLine, RealBuf&, long iFrom, long iNum) const ;
	void				ReadParameters(const FileName& fnObj, ParmList& pm);
	void				Store(IlwisObject on);
	virtual long          iRaw(RowCol) const;
	virtual long          iValue(RowCol) const;
	virtual double        rValue(RowCol) const;
	bool retrieveImage(const CoordBounds & cb, const RowCol & rc);

private:
	//void Init();
	static void PutData(vector<OpenStreetMapLayerInfo *> layers, OpenStreetMapCollectionPtr* col, ParmList& pm);
	static UINT PutDataInThread(LPVOID lp);
	CoordSystem getCoordSystem(const FileName& fnBase, const String& srsName);
	String getMapRequest(const CoordBounds& cb, const RowCol rc) const;
	int long2tilex(double lon, int z) const;	
	int lat2tiley(double lat, int z) const;

	URL	urlOpenStreetMap;
	std::vector<String> vsRequestHeaders;

	GeoRef grf;
	GeoRefOSM *grfOpenStreetMap;
	GDALDatasetH gdalDataSet;
	RemoteObject * rxo;
	CoordSystem pseudoMercator;
	CoordSystem llwgs84;
};

