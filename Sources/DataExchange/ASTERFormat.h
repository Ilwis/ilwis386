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
#ifndef ASTERFORMAT_H
#define ASTERFORMAT_H

class HDFData;

ForeignFormat *CreateQueryObjectAster();
ForeignFormat *CreateImportObjectAster(const FileName& fnFO, ParmList& pm);

class ASTERFormat : public ForeignFormat
{
public:
	ASTERFormat(const FileName& fnForeign, ParmList& pm);
	ASTERFormat();
	~ASTERFormat();

	_export static String ASTERFormat::sFormatASTER(const FileName& fnForeign);

	virtual bool _export         fIsCollection(const String& sForeignObject) const;
	virtual bool _export         fMatchType(const String& fnFileName, const String& sType);
	virtual void _export         PutDataInCollection(ForeignCollectionPtr* col, ParmList& pm); 
	void                         GetRasterInfo(LayerInfo& inf, String sChannel);
  virtual void _export         GetLineRaw(long iLine, ByteBuf&, long iFrom, long iNum) const ;
  virtual void _export         GetLineRaw(long iLine, IntBuf&,  long iFrom, long iNum) const ;
  virtual void _export         GetLineRaw(long iLine, LongBuf&, long iFrom, long iNum) const ;
  virtual void _export         GetLineVal(long iLine, LongBuf&, long iFrom, long iNum) const ;
  virtual void _export         GetLineVal(long iLine, RealBuf&, long iFrom, long iNum) const ;
	virtual long _export         iRaw(RowCol) const;
	virtual long _export         iValue(RowCol) const;
	virtual double _export       rValue(RowCol) const;
	virtual bool _export				 fIsSupported(const FileName& fn, ForeignFormat::Capability dtType = cbANY) const;
	bool													fIsThreaded();
	static double aasin(double v);
	virtual void Store(IlwisObject ob);
	void ReadParameters(const FileName& fnObj, ParmList& pm);
	void getImportFormats(vector<ImportFormat>& formats);
private:
	enum ASTERBandPaths{ab1A_X_SCENELINENUMBER, ab1A_X_LATITUDE, ab1A_X_LONGITUDE,
								 ab1A_X_SATELLITEPOSITION, ab1A_X_SATELLITEVELOCITY, 
								 ab1A_X_OBSERVATIONTIME, ab1A_X_IMAGEDATA, ab1A_X_LATTICEPOINT,
								 ab1A_X_SIGHTVECTOR, ab1A_X_PARALLAXOFFSET, ab1A_X_INTERTELESCOPEOFFSET, 
								 ab1A_X_RADIOCORRTABLE, 
								 ab1B_X_IMAGEDATA, ab1B_X_LATITUDE, ab1B_X_LONGITUDE, 
								 ab1B_DEM_LATITUDE, ab1B_DEM_LONGITUDE, ab1B_DEM_IMAGEDATA,
								 abpANY};


	enum ASTERBands{ abVNIR1, abVNIR2, abVNIR3N, abVNIR3B, abSWIR4, abSWIR5, 
									 abSWIR6, abSWIR7, abSWIR8, abSWIR9, abTIR10, abTIR11, abTIR12, abTIR13,
									 abTIR14, abDEM, abANY };
	enum ASTERProcessingLevel{apl1A, apl1B, apl1DEM, aplANY};
	
	void Init();
	String sNameToPath(ASTERBandPaths abpType, const String& sName);
	void GetCoordSystem(CoordSystem& csy);
	void GetCoordSystemDem(CoordSystem& csy);
	bool fCsyCreatedAndStored;
	void GetGeoRef(GeoRef& grf, CoordSystem& csy, ASTERBands abpType, const FileName& fnNew);
	void ReadRadioMetricCorrectionTable(const String& sBand);
	void ImportRasterMap(const FileName& fnRasMap, Map& mp ,LayerInfo& li, ASTERBands abBand);
	void InitData(ASTERBands abBand);
	void ReadForeignFormat(ForeignCollectionPtr* col);
	static UINT ASTERFormat::PutDataInThread(LPVOID lp);
	void CleanUp();

	static String sStripAllWhiteSpace(const String& sMetaData);
	static String sGetASTERMetaDataValue(const String& sMetaData, const String& sGroup, const String& sObject);

	static map<ASTERBandPaths, String> m_BandPaths;
	static map<ASTERBands, String> m_BandNames;
	unsigned char *buffer;
	HDFData *pImageData;
	vector< vector<double> >  vvRadCorr;
	bool fImport;
	FileName fnBaseOutputName;
	bool fUseRadiances;
	ASTERBands band;
	ASTERProcessingLevel aplLevel;
	HDFData::Cell clMapSize;
	double rUnitConversion[15];
	int iTirOffset;
	bool fShowCollection;
	bool fThreaded;
	double rLatitudeCorrFactor;
	int iLayer;
};

#endif
