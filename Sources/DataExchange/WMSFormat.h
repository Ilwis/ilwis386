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

class WMSLayerInfo;
class WMSFormat;
class WMSCollectionPtr;

typedef void(__stdcall *GDALCloseFunc)(GDALDatasetH);
typedef void(__stdcall *GDALAllRegisterFunc)();
typedef int (__stdcall *GDALGetNumbersDataSet)(GDALDatasetH);
typedef GDALDatasetH (__stdcall *GDALOpenFunc) (const char *, GDALAccess );
typedef GDALRasterBandH (__stdcall *GDALGetRasterBandFunc) (GDALDatasetH, int);
typedef	GDALColorInterp (__stdcall *GDALGetRasterColorInterpretationFunc)(GDALRasterBandH);
typedef GDALColorTableH (__stdcall *GDALGetRasterColorTableFunc)(GDALRasterBandH);
typedef GDALPaletteInterp (__stdcall *GDALGetPaletteInterpretationFunc)(GDALColorTableH);
typedef int (__stdcall *GDALGetColorEntryCountFunc)(GDALColorTableH);
typedef const GDALColorEntry* (__stdcall *GDALGetColorEntryFunc)(GDALColorTableH, int);
typedef OGRSpatialReferenceH (__stdcall *OSRNewSpatialReferenceFunc)( const char *);
typedef OGRErr (__stdcall *OSRImportFromEPSGFunc)( OGRSpatialReferenceH, int );
typedef int (*OSRIsProjectedFunc)( OGRSpatialReferenceH );
typedef CPLErr 	(__stdcall *GDALRasterIOFunc)(GDALRasterBandH hRBand, GDALRWFlag eRWFlag, int nDSXOff, int nDSYOff, int nDSXSize, int nDSYSize, void *pBuffer, int nBXSize, int nBYSize, GDALDataType eBDataType, int nPixelSpace, int nLineSpace);
typedef const char * (__stdcall *OSRGetAttrValueFunc)( OGRSpatialReferenceH hSRS, const char * pszName, int iChild /* = 0 */ );
typedef void (*CPLPushFinderLocationFunc)( const char * );
typedef int (*VSIFCloseLFunc)( FILE * );
typedef FILE* (*VSIFileFromMemBufferFunc)( const char *, GByte *, vsi_l_offset ,int  );


struct WMSGDALCFunctions {
	GDALCloseFunc close;
	GDALAllRegisterFunc registerAll;
	GDALAllRegisterFunc registerJPEG;
	GDALAllRegisterFunc registerPNG;
	GDALOpenFunc open;
	GDALGetNumbersDataSet count;
	GDALGetRasterBandFunc getBand;
	GDALGetRasterColorInterpretationFunc getRasterColorInterpretation;
	GDALGetRasterColorTableFunc getRasterColorTable;
	GDALGetPaletteInterpretationFunc getPaletteInterpretation;
	GDALGetColorEntryCountFunc getColorEntryCount;
	GDALGetColorEntryFunc getColorEntry;
	OSRNewSpatialReferenceFunc newSRS;
	OSRImportFromEPSGFunc srsImportFromEPSG;
	OSRIsProjectedFunc isProjected;
	GDALRasterIOFunc rasterIO;
	OSRGetAttrValueFunc getAttribute;
	CPLPushFinderLocationFunc finderLoc;
	VSIFCloseLFunc vsiClose;
	VSIFileFromMemBufferFunc vsiFileFromMem;
};

typedef void (WMSFormat::*SetProjectionParm)(Projection& prj, const void * srs);

ForeignFormat *CreateImportObjectWMS(const FileName& fnFO, ParmList& pm);
ForeignFormat *CreateQueryObjectWMS();

struct ProjectionConversionFunctions
{
	String sProjectionName;
	SetProjectionParm parmfunc;
	
	ProjectionConversionFunctions(const String& sName="", SetProjectionParm func=NULL) : sProjectionName(sName), parmfunc(func){}
};

class RemoteObject;

class _export WMSFormat : public ForeignFormat {
public:

	WMSFormat();
	~WMSFormat();
	WMSFormat(const FileName& fn, ParmList& pm);
	virtual bool          fIsCollection(const String& sForeignObject) const;
	virtual bool          fMatchType(const String& fnFileName, const String& sType);
	virtual void          PutDataInCollection(ForeignCollectionPtr* col, ParmList& pm);
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
	FileName createCollectionName(const String& name,ParmList& pm);

private:
	void Init();
	static void PutData(vector<WMSLayerInfo *> layers, WMSCollectionPtr* coll, ParmList& pm);
	static UINT PutDataInThread(LPVOID lp);
	CoordSystem getCoordSystem(const FileName& fnBase, const String& srsName);
	WMSLayerInfo* find(vector<WMSLayerInfo *> layers, const String& sName);
	String getMapRequest(const CoordBounds& cb, const String& layers, const String& srs, const RowCol rc) const;
	void HandleError(const String& sErr) const;
	static String makeCompatible(const String& name);
	String toLegalLayer(const String& name) const;
	WMSLayerInfo *findRelevantSrsLayer(vector<WMSLayerInfo *> layers, WMSLayerInfo *targetlayer, WMSLayerInfo *srsLayer);

	URL	urlWMS;
	URL urlGetMap;

	String layers;
	GeoRef grf;
	GeoRefCornersWMS *grfWMS;
	String srsName;
	String layerName;
	GDALDatasetH gdalDataSet;
	map<String, ProjectionConversionFunctions> mpCsyConvers; // coordsystem conversion functions
	map<String, String> ellipseMapping;
	WMSGDALCFunctions funcs;
	RemoteObject * rxo;
	MemoryStruct *image;
};

