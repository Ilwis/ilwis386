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
#include "Headers\toolspch.h"

#pragma warning( disable : 4715 )

#include <set>
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Engine\SpatialReference\Coordsys.h"
#include "Engine\SpatialReference\Csproj.h"
#include "Engine\SpatialReference\Cslatlon.h"
#include "Engine\SpatialReference\Ellips.h"
#include "Engine\Base\File\objinfo.h"
#include "Engine\SpatialReference\DATUM.H"
#include "Engine\Base\File\objinfo.h"
#include "Engine\SpatialReference\prj.h"
#include "Engine\SpatialReference\UTM.H"
#include "Engine\SpatialReference\Grctppla.h"
#include "Engine\DataExchange\ForeignFormat.h"
#include "Engine\SpatialReference\Grcornrs.h"
#include "Engine\Base\File\Directory.h"
#include "Engine\Map\Raster\Map.h"
#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\Engine.h"
#include "Engine\DataExchange\hdfincludes\hdf.h"
#include "Engine\DataExchange\hdfincludes\vg.h"
#include "Engine\DataExchange\hdfincludes\mfhdf.h"
#include "Engine\DataExchange\HDFReader.h"
#include "DataExchange\ASTERFormat.h"
#include "Engine\Base\DataObjects\Tranq.h"
#include "Headers\constant.h"
#include "Headers\messages.h"
#include "Headers\Hs\IMPEXP.hs"
#include "Headers\Hs\proj.hs"
#include "Headers\Htp\Ilwismen.htp"

#pragma warning( disable : 4786 )

map<ASTERFormat::ASTERBandPaths, String> ASTERFormat::m_BandPaths;
map<ASTERFormat::ASTERBands, String> ASTERFormat::m_BandNames;

const double rPRECISION = 0.001;

class StopASTERConversion : public ErrorObject
{};

ForeignFormat* CreateQueryObjectAster()
{
	return new ASTERFormat();
}

String ASTERFormat::sFormatASTER(const FileName& fnForeign)
{
	HDFData *da = HDFData::Create(fnForeign, HDFData::daDataSet, "");	
	if ( da )	
	{
		String s = da->sGetAttribute("coremetadata.0");
		delete da;		
		if (s != "" )
		{
			String sLv = sGetASTERMetaDataValue(s,"QASTATS" , "PROCESSINGLEVELID");		
			if ( sLv == "\"1A\"" || sLv == "\"1B\"")
				return "ASTER";
			else 
			{
				String sLv = sGetASTERMetaDataValue(s,"ADDITIONALATTRIBUTES", "ADDITIONALATTRIBUTENAME");
				if (sLv == "\"DEMType\"")
				return "ASTERDEM";
			}
		}
	}	
	return "";
}

ForeignFormat *CreateImportObjectAster(const FileName& fnFO, ParmList& pm)
{
	if ( pm.iSize() < 2 ) // existing object
	{
		String sMethod;
		if ( ObjectInfo::ReadElement("ForeignFormat", "method", fnFO, sMethod) != 0)	// oldstyle expressions
		{
			ForeignFormat *ff = ForeignFormat::Create(sMethod);
			if ( ff )
				ff->ReadParameters(fnFO, pm);
			delete ff;
		}	
	}
	ForeignFormat *ff = NULL;
	try
	{

		ff =  new ASTERFormat(fnFO, pm);
		return ff;
	}
	catch (ErrorObject& err)
	{
		delete ff;		
		throw err;
	}		
	return NULL;	
}

ASTERFormat::ASTERFormat() :
	buffer(0),
	fImport(false),
	fUseRadiances(true),
	pImageData(NULL),
	band(abANY),
	aplLevel(aplANY),
	iTirOffset(0),
	fShowCollection(true),
	fThreaded(true)
{
}

ASTERFormat::ASTERFormat(const FileName& fn, ParmList& pm) :
	ForeignFormat(pm.sGet("input"), mtRasterMap),
	buffer(0),
	fImport(false),
	fUseRadiances(true),
	pImageData(NULL),
	band(abANY),
	aplLevel(aplANY),
	iTirOffset(0)
{
	String test = pm.sCmd();
	bool *fDoNotLoadGDB = (bool *)(getEngine()->pGetThreadLocalVar(IlwisAppContext::tlvDONOTLOADGDB));	
	if ( fDoNotLoadGDB && *fDoNotLoadGDB == true )	
		return;		
	Init(); // initialize datastructures
	int dummy;
	if ( ObjectInfo::ReadElement("ForeignFormat", "AssociatedType", fn, dummy) != 0)
		mtLoadType = (ForeignFormat::mtMapType)dummy;		
	
	FileName fnOutput = pm.sGet("output");
	bool fAbsolute = false;
	if ( fnOutput != FileName() )
	{
		if ( fnOutput.sFile != "" ) // empty name, use default name
		{
			fnBaseOutputName = fnOutput;
			if ( !fnBaseOutputName.fValid())
				fnBaseOutputName.sFile = fn.sFile;
		}
		else
			fnBaseOutputName = fn;

		fAbsolute = true;
	}

	if ( fnBaseOutputName == FileName() )
	{
			fnBaseOutputName = fn;
			fnBaseOutputName.sFile = fnBaseOutputName.sFile.sTrimSpaces(); // ilwis can not handle name that start wit a space;
	}
		
	// retrieve processing level of data (1A or 1B)
	if ( !fnGetForeignFile().fExist() )
	{
		CleanUp();
		throw ErrorObject(String(SIErrDataFileMissing_S.scVal(), fnGetForeignFile().sRelative()));
	}		
	
	HDFData *da = HDFData::Create(fnGetForeignFile(), HDFData::daDataSet, "");	
	if ( da == 0)
	{
		CleanUp();
		throw ErrorObject(String(SIErrNoAsterFile_S.scVal(), fnGetForeignFile().sFileExt()));
	}		
	
	String s = da->sGetAttribute("coremetadata.0");
	if (s != "" )
	{
		String sLv = sGetASTERMetaDataValue(s,"QASTATS" , "PROCESSINGLEVELID");		
		if ( sLv == "\"1A\"")
			aplLevel = apl1A;
		else if ( sLv == "\"1B\"")
			aplLevel = apl1B;
		sLv = sGetASTERMetaDataValue(s,"ADDITIONALATTRIBUTES", "ADDITIONALATTRIBUTENAME");
		if ( sLv == "\"DEMType\"")
			aplLevel = apl1DEM;
	}
	delete da;
	if ( aplLevel == aplANY)
	{
		CleanUp();
		throw ErrorObject(String(SIErrNoAsterFile_S.scVal(), fnGetForeignFile().sFileExt()));		
	}	
	if ( pm.fExist("layer")) // an individual band will be opnened. we must open the correct dataset
	{
		String sC = pm.sCmd();
		InitData((ASTERBands)pm.sGet("layer").iVal());
	}
	fImport = pm.fExist("import");
	fUseRadiances = pm.fExist("usednvalues");
	fCsyCreatedAndStored = false;
	fThreaded = !pm.fExist("nothreads");
	fShowCollection = !pm.fExist("noshow");

}	
	
ASTERFormat::~ASTERFormat()
{
	CleanUp();
}

void ASTERFormat::CleanUp()
{
	if ( trq )
	{
		trq->Stop();
		delete trq;
		trq = NULL;
	}
	delete [] buffer;
	buffer = NULL;
	delete pImageData;
	pImageData = NULL;
}

// will retrieve a pointer to the image data, set the radiometric correction table and read dn/radiomteric conversion
// coefficients
void ASTERFormat::InitData(ASTERBands abBand)	
{
	// retrieve a HDFReader object that points to the image data
	// determine band name
	band = abBand;
	String sBandName = m_BandNames[band];
	String sPathImageData; //= sNameToPath(aplLevel == apl1A ? ab1A_X_IMAGEDATA : ab1B_X_IMAGEDATA, sBandName);
	if (aplLevel == apl1A )
		sPathImageData = sNameToPath(ab1A_X_IMAGEDATA, sBandName);
	else if (aplLevel == apl1B)
		sPathImageData = sNameToPath(ab1B_X_IMAGEDATA, sBandName);
	else 
		sPathImageData = sNameToPath(ab1B_DEM_IMAGEDATA, sBandName);
	pImageData = HDFData::Create(fnGetForeignFile(), HDFData::daDataSet, sPathImageData);
	if ( pImageData == NULL )
		throw ErrorObject(String(SIErrrNoImageDataFound_S.scVal(), sPathImageData));
	
	clMapSize = pImageData->clGetSize();

	// read radiometric correction table
	if ( aplLevel == apl1A )
		ReadRadioMetricCorrectionTable(sBandName);	

	// read dn/radiomteric conversion coefficients
	if (aplLevel == apl1A || aplLevel == apl1B)
	{
		HDFData *da = HDFData::Create(fnGetForeignFile(), HDFData::daDataSet, "");	
		double rCoeff = rUNDEF;
		// strip of the first char of the name, this is the postfix for the metadata entry
		char c = (sBandName.toLower())[0];
		String sMetaData("productmetadata.%c", c);
		String s = da->sGetAttribute(sMetaData);
		delete da;	
		// strip of the bandnumber. this is the postfix for the groupname and object name
		String sBandNumber = sBandName.sTail(":");	
		if ( s != "" )
		{
			sBandNumber.toUpper();
			String sGroupName("UNITCONVERSIONCOEFF%S",sBandNumber);
			String sObject("INCL%S", sBandNumber);
			String sCoeff = sGetASTERMetaDataValue(s, sGroupName, sObject);
			rCoeff = sCoeff.rVal();
		}
		if ( abBand >= abTIR10)
		{
			HDFData *da = HDFData::Create(fnGetForeignFile(), HDFData::daDataSet, "");	
			String sAttr = da->sGetAttribute("productmetadata.t");
			String sGroupName("EXTRACTIONFROML%c",sBandNumber);
	//		String sOffset = sGetASTERMetaDataValue(s, sGroupName, ")
			iTirOffset = 6;
			delete da;			
		}			
		rUnitConversion[(int)abBand] = rCoeff;
	}

}
	
void ASTERFormat::Init()
{
	if ( m_BandPaths.size() == 0 )
	{
    m_BandPaths[ab1A_X_SCENELINENUMBER]    = "X\\X_BandY\\Geolocation Fields\\SceneLineNumber";
    m_BandPaths[ab1A_X_LATITUDE]           = "X\\X_BandY\\Geolocation Fields\\Latitude";
    m_BandPaths[ab1A_X_LONGITUDE]          = "X\\X_BandY\\Geolocation Fields\\Longitude";
    m_BandPaths[ab1A_X_SATELLITEPOSITION]  = "X\\X_BandY\\Data Fields\\SatellitePosition";
		m_BandPaths[ab1A_X_SATELLITEVELOCITY]  = "X\\X_BandY\\Data Fields\\SatelliteVelocity";
		m_BandPaths[ab1A_X_OBSERVATIONTIME]		= "X\\X_BandY\\Data Fields\\ObservationTime";
		m_BandPaths[ab1B_X_LATITUDE]           = "X\\X_Swath\\Geolocation Fields\\Latitude";
    m_BandPaths[ab1B_X_LONGITUDE]          = "X\\X_Swath\\Geolocation Fields\\Longitude";

		m_BandPaths[ab1A_X_PARALLAXOFFSET]        = "X\\X_BandY\\Data Fields\\ParallaxOffset";
		m_BandPaths[ab1A_X_INTERTELESCOPEOFFSET]  = "X\\X_BandY\\Data Fields\\IntertelescopeOffset";
    m_BandPaths[ab1A_X_IMAGEDATA]          = "X\\X_BandY\\Data Fields\\ImageData";
    m_BandPaths[ab1A_X_LATTICEPOINT]       = "X\\X_BandY\\Data Fields\\LatticePoint";
    m_BandPaths[ab1A_X_SIGHTVECTOR]        = "X\\X_BandY\\Data Fields\\SightVector";
    m_BandPaths[ab1A_X_RADIOCORRTABLE]     = "X\\X_BandY\\Data Fields\\RadiometricCorrTable";
    m_BandPaths[ab1B_X_IMAGEDATA]          = "X\\X_Swath\\Data Fields\\ImageDataY";

		m_BandPaths[ab1B_DEM_LATITUDE]           = "DefaultSwath\\Geolocation Fields\\MRGFLD_Latitude";
    m_BandPaths[ab1B_DEM_LONGITUDE]          = "DefaultSwath\\Geolocation Fields\\MRGFLD_Longitude";
		m_BandPaths[ab1B_DEM_IMAGEDATA]          = "DefaultSwath\\Data Fields\\Band1";

		m_BandNames[ abVNIR1 	 ] = "VNIR:1";
		m_BandNames[ abVNIR2	 ] = "VNIR:2";
		m_BandNames[ abVNIR3N	 ] = "VNIR:3N";
		m_BandNames[ abVNIR3B	 ] = "VNIR:3B";
		m_BandNames[ abSWIR4	 ] = "SWIR:4";
		m_BandNames[ abSWIR5	 ] = "SWIR:5";
		m_BandNames[ abSWIR6	 ] = "SWIR:6";
		m_BandNames[ abSWIR7	 ] = "SWIR:7";
		m_BandNames[ abSWIR8	 ] = "SWIR:8";
		m_BandNames[ abSWIR9	 ] = "SWIR:9";
		m_BandNames[ abTIR10	 ] = "TIR:10";
		m_BandNames[ abTIR11	 ] = "TIR:11";
		m_BandNames[ abTIR12	 ] = "TIR:12";
		m_BandNames[ abTIR13	 ] = "TIR:13";
		m_BandNames[ abTIR14	 ] = "TIR:14";
		m_BandNames[ abDEM		 ] = "DEM";
  }
}

void ASTERFormat::ReadRadioMetricCorrectionTable(const String& sBand)
{
	String sPathRadioMetricCorrection = sNameToPath(ab1A_X_RADIOCORRTABLE, sBand);
	HDFData *pRadioMetricCorrectionTbl = HDFData::Create(fnGetForeignFile(), HDFData::daDataSet, sPathRadioMetricCorrection);			
	if ( pRadioMetricCorrectionTbl )
	{
		HDFData::Cell cell = pRadioMetricCorrectionTbl->clGetSize();
		vvRadCorr.resize(cell.iCoord(HDFData::Cell::aX));
		int iCols = cell.iCoord(HDFData::Cell::aY);
		RealBuf buf(iCols);		
		for ( unsigned int i = 0; i < vvRadCorr.size(); ++i )
		{
			vvRadCorr[i].resize(iCols);
			pRadioMetricCorrectionTbl->GetRow(i, buf);			
			for (int j = 0; j < buf.iSize(); ++j)
				vvRadCorr[i][j] = buf[j];
		}			
		delete pRadioMetricCorrectionTbl;		
	}
}

bool ASTERFormat::fIsCollection(const String& sForeignObject) const
{
	return true;
}

bool ASTERFormat::fMatchType(const String& fnFileName, const String& sType)
{
	return sType == "ILWIS ForeignCollections";
}


struct ASTERThreadData
{
	ASTERThreadData() : af(NULL), fptr(NULL) {}
	ASTERFormat *af;
	ForeignCollection* fptr;
	Directory dir;
};

void ASTERFormat::PutDataInCollection(ForeignCollectionPtr* col, ParmList& pm)
{
	// fnBaseOutputName indicates a file that may not exist
	// In case it does not exist do not check on R/O
	// In case it exist the file must not be R/O and also the directory it is in
	// must not be R/O, so throw an error if
	String filePath = fnBaseOutputName.sPath();
	filePath = filePath.sQuote(false);

	Directory dir(filePath);
	bool fDirRO = dir.fReadOnly();
	
	if (fDirRO)
		throw ErrorObject(SIErrNotValid);

	bool *fDoNotLoadGDB = (bool *)(getEngine()->pGetThreadLocalVar(IlwisAppContext::tlvDONOTLOADGDB));	
	if ( *fDoNotLoadGDB == true )	
		return;

	ASTERThreadData *data = new ASTERThreadData;
	data->af = this;
	data->fptr = new ForeignCollection(col->fnObj, ParmList());   // pass the complete object as pointer
	                                                  // needed to keep pointer in memory when passing it to thread
	data->dir = Directory(getEngine()->sGetCurDir());

	if ( pm.fExist("nothreads") )
		PutDataInThread((VOID *)data);
	else
		::AfxBeginThread(PutDataInThread, (VOID *)data);
}

UINT ASTERFormat::PutDataInThread(LPVOID lp)
{
	ASTERThreadData *data = NULL;
	FileName fnCol;
	try
	{
		data = (ASTERThreadData *)lp;
		fnCol = (*(data->fptr))->fnObj;
		if ( data->af->fIsThreaded() ) // local vars need not be set, they are alreay set as this is not a seperated thread
			getEngine()->InitThreadLocalVars();
		getEngine()->SetCurDir(data->dir.sFullPath());
		
		data->af->ReadForeignFormat(data->fptr->ptr());

		if ( data->af->fIsThreaded() )
			getEngine()->RemoveThreadLocalVars();
		delete data->fptr;  // remove the foreign collection (because passed as pointer)
		delete data;
		data = 0;
		
	}
	catch(ErrorObject&)
	{
		FileName *fn = new FileName(fnCol);								
		getEngine()->RemoveThreadLocalVars();					
		AfxGetApp()->GetMainWnd()->SendMessage(ILWM_CLOSECOLLECTION, (WPARAM)fn, 0);
		ASTERThreadData *data = (ASTERThreadData *)lp;
		if ( fnCol != FileName() )
		{
			String sCmd("del %S -quiet -force", fnCol.sFullPath());
			getEngine()->Execute(sCmd);
			AfxGetApp()->GetMainWnd()->PostMessage(ILW_READCATALOG, WP_RESUMEREADING, 0);		
		}
		if (data)
			delete data->fptr;
		delete data;		
	}
	return 1;
}

// retrieves info about an ASTER file and stores the created objects on disk.
void  ASTERFormat::ReadForeignFormat(ForeignCollectionPtr* col)
{
	trq = new Tranquilizer();
	trq->Start();
	trq->SetTitle(String(SIEMImportingRaster_S.scVal(), fnGetForeignFile().sShortName()));
	trq->setHelpItem("ilwismen\import_aster.htm");		

	AfxGetApp()->GetMainWnd()->SendMessage(ILW_READCATALOG, WP_STOPREADING, 0);
	LayerInfo li;			
	try
	{
		String sForeignFile = fnGetForeignFile().sRelative();
		if ( fnBaseOutputName.sPath() != fnGetForeignFile().sPath() )
			sForeignFile = fnGetForeignFile().sFullPath();
		for(map<ASTERBands, String>::iterator cur = ASTERFormat::m_BandNames.begin(); 
		    cur != ASTERFormat::m_BandNames.end();
				++cur)
		{
			if ( trq->fText(String(SIEMAddingBand_S.scVal(), m_BandNames[(ASTERBands)(*cur).first])) )
				throw StopASTERConversion();
			if ((aplLevel == apl1DEM) && ((int)(*cur).first < 15)) continue;
			if ((aplLevel != apl1DEM) && ((int)(*cur).first >= 15)) continue;			
			GetRasterInfo(li, String("%i",(*cur).first));
			iLayer = li.layerIndex;
			if ( li.fnObj == FileName() )
				continue;
			Map mp(li.fnObj, li);
			mp->fErase = true;			

			// for a true import all the data must be read and stored in ILWIS format
			if ( fImport )
				ImportRasterMap(li.fnObj, mp, li, (*cur).first);
			mp->SetDescription(String(SIEDscBand_sFromAsterImport_s.scVal(),
												m_BandNames[(ASTERBands)(*cur).first],fnGetForeignFile().sFileExt()));
			mp->Store();
			li.grf->Store();
			if(!fCsyCreatedAndStored)
				li.csy->Store();

			col->Add(li.fnObj);
			col->Add(li.grf->fnObj);
			col->Add(li.csy->fnObj);
			mp->fErase = false;
			li.grf->fErase = false;
			li.csy->fErase = false;
			Store(mp);
			
		}
		AfxGetApp()->GetMainWnd()->SendMessage(ILW_READCATALOG, WP_RESUMEREADING, 0);
		AfxGetApp()->GetMainWnd()->PostMessage(ILW_READCATALOG, 0, 0);
		String sCommand("*open %S", col->fnObj.sRelativeQuoted());
		if ( fShowCollection)
			AfxGetApp()->GetMainWnd()->SendMessage(ILWM_EXECUTE, 0, (LPARAM)sCommand.scVal());		
		delete trq;
		trq = NULL;				
	}			
	catch(StopASTERConversion& err)
	{
		err.Show();
		if ( col != 0 )
		{
			//it could be that the layerinfo contains files that were not yet added to the collection
			// add them to ensure that they are also deleted
			col->Add(li.fnObj);
			if ( li.grf.fValid())
				col->Add(li.grf->fnObj);
			if ( li.csy.fValid())
				col->Add(li.csy->fnObj);
			
			col->DeleteEntireCollection();			
			col->fErase = true;
		}
		CleanUp();
		throw err;
	}			
}

void ASTERFormat::ImportRasterMap(const FileName& fnRasMap, Map& mp ,LayerInfo& li, ASTERBands abBand)
{
	InitData(abBand);

	mp->fChanged = false; // prevents unneccessary stores, it is changed but the store will happen later
	mp = Map(li.fnObj, li.grf, li.grf->rcSize(), li.dvrsMap);
	// for all the lines in the coce
	for ( int iLine = 0; iLine < mp->iLines(); ++iLine)
	{
		if (trq->fUpdate(iLine, mp->iLines()))
			throw StopASTERConversion();
		if ( mp->st() == stREAL )
		{
			RealBuf buf(mp->iCols());
			GetLineVal(iLine, buf, 0, mp->iCols());
			mp->PutLineVal(iLine, buf);
		}				
		else			
		{
			LongBuf buf(mp->iCols());
			GetLineRaw(iLine, buf, 0, mp->iCols());
			mp->PutLineRaw(iLine, buf);			
		}			
	}
	delete pImageData;
	pImageData = NULL;
}

// will replace the two X's in the path string with the sensor name and the Y by the band number
String ASTERFormat::sNameToPath(ASTERBandPaths abpType, const String& sName)
{
	String sSensor = sName.sHead(":");
	String sNumber = sName.sTail(":");
	String sPath = m_BandPaths[abpType];
	if (abpType < ab1B_DEM_LATITUDE)
	{
		sPath.replace(sPath.find("X"), 1, sSensor);
		sPath.replace(sPath.find("X"), 1, sSensor);
		int iPos = sPath.find("Y");
		if (iPos >= 0) // test for 1 B level (because there might be no Y)
			sPath.replace(sPath.find("Y"), 1, sNumber);
	}
	return sPath;
}

void ASTERFormat::GetRasterInfo(LayerInfo& inf, String sChannel)
{
	int iChannel = sChannel.iVal();
	inf.layerIndex = iChannel;
	if (aplLevel == apl1DEM) iChannel = 15;
	String sBandName = m_BandNames[(ASTERBands)iChannel];
	String sPathImageData;// = sNameToPath(aplLevel == apl1A ? ab1A_X_IMAGEDATA : ab1B_X_IMAGEDATA, sBandName);	
	if (aplLevel == apl1A )
		sPathImageData = sNameToPath(ab1A_X_IMAGEDATA, sBandName);
	else if (aplLevel == apl1B)
		sPathImageData = sNameToPath(ab1B_X_IMAGEDATA, sBandName);
	else 
		sPathImageData = sNameToPath(ab1B_DEM_IMAGEDATA, sBandName);
	HDFData *ds = HDFData::Create(fnGetForeignFile(), HDFData::daDataSet, sPathImageData);
	if ( ds != NULL && ds->fValid() ) // 1B data needs not to have all bands present
	{
		// create unique name for file
		String sName = String("%S_%S", fnBaseOutputName.sFile, sBandName);
		if (aplLevel != apl1DEM)		
			sName.replace(sName.find(":"), 1, "_");
		FileName fnNew(sName, ".mpr");
		fnNew.Dir(fnBaseOutputName.sPath());		
		fnNew = FileName::fnUnique(fnNew);
		inf.fnObj = fnNew;
		inf.fnForeign = fnGetForeignFile();
		// set the value domain. If radiances are not used we will make domains that map the (sometimes pseudo) dn values
		if (iChannel == abDEM)
		{
			HDFData *da = HDFData::Create(fnGetForeignFile(), HDFData::daDataSet, "");
			String sDemInfo = da->sGetAttribute("dem_specific.0");
			if (sDemInfo != "" )
			{
				String sPixelSpacing = sGetASTERMetaDataValue(sDemInfo,"DEM_SPECIFIC" , "CELLSPACING");
				String sElevationMin = sGetASTERMetaDataValue(sDemInfo,"DEM_SPECIFIC" , "DEMELEVATIONMIN");
				String sElevationMax = sGetASTERMetaDataValue(sDemInfo,"DEM_SPECIFIC" , "DEMELEVATIONMAX");
				double rPix = sPixelSpacing.rVal();
				double rMinDem = sElevationMin.rVal();		
				double rMaxDem = sElevationMax.rVal();
				inf.dvrsMap = DomainValueRangeStruct(rMinDem, rMaxDem, 1.0);
			}
		}
			
		else if ( iChannel < abTIR10)
			if ( fUseRadiances)
				inf.dvrsMap = DomainValueRangeStruct(0.0, 250.0, rPRECISION);
			else
				inf.dvrsMap = DomainValueRangeStruct(Domain("image"));
		else
			if ( fUseRadiances)
				inf.dvrsMap = DomainValueRangeStruct(0.0, 20.0, rPRECISION);
			else
				inf.dvrsMap = DomainValueRangeStruct(0, 4094);

		ASTERBands abBand = (ASTERBands)iChannel;			

		if ( abBand == abVNIR1 || abBand == abVNIR3B || abBand == abSWIR4 || abBand == abTIR10 
			|| aplLevel == apl1A || (aplLevel == apl1DEM)) // make in level 1a and in dem a georef for each band
		{
			String sGeorefName = String("%S_%S", fnBaseOutputName.sFile, sBandName);
			GetGeoRef( inf.grf, inf.csy, abBand, fnNew);
		}		
		inf.cbActual = inf.cbMap = inf.grf->cb();
	}
	delete ds;
}

template<class T>  void ApplyRadioMetricCorrectionVNIRSWIR(T& buf, int iStart, const vector< vector<double> >& vvRadCorr, 
																											long iSz, bool fUsesRaws, double rCoeff, bool fRadioMetricCorrection)
{
	// when raws are used the numbers read need to be scaled to map the ILWIS idea of a raw value. 
	double rFactor = 1.0;
	if ( fUsesRaws)
		rFactor = 1.0/ rPRECISION;

	// the buf may not have the correct size at all times, for some reason the drawer always uses buffers that are one pixel larger
	// while others use the correct size. We must have the correct size though
	int iCount = min(iSz, buf.iSize());
	// maximum must not be larger than the size of the radiometric correction table, some applications read far beyond this
	int iBound = iStart + iCount;
	bool f1A = vvRadCorr.size() != 0;
	if ( f1A )
		iBound = min(iStart + iCount, vvRadCorr.size());

	// applying the formula to all pixels
	double rPrev = rUNDEF;
	bool fPixelBorder = iStart < 0 && iSz == 2;
	iStart = max(0, iStart);
	for (int i = iStart; i < iBound ; ++i )
	{
		if ( f1A )
		{
			double rA = vvRadCorr[i][1];
			double rG = vvRadCorr[i][2];
			double rD = vvRadCorr[i][0];
			double rV = buf[i - iStart];
			double rL = max(0.0, rA * rV / rG + rD);

		/*	double rTemp = rPrev;
			rPrev = rL;			
			if ( rTemp != rUNDEF )
				rL = (rL + rTemp) / 2.0;*/
			
			if ( !fRadioMetricCorrection )
			{
				buf[i-iStart] = min(255, rFactor * ( rL ) / rCoeff);
        buf[i-iStart] = min(255, max(0,buf[i-iStart]));														
			}
			else
				buf[i - iStart] = max(0.0, rFactor * rL) ;
				
		}
		else
		{
			if ( fRadioMetricCorrection)
			{
				buf[i - iStart] = (buf[ i - iStart] - 1.0 ) * (rFactor * rCoeff);
				buf[i - iStart] = max(0.0,buf[i - iStart]);								
			}				
			else
			{
				buf[i - iStart] = min(255, ((double)buf[i-iStart]) * rFactor);
				buf[i - iStart] = min(255, max(0,buf[i - iStart]));				
			}				
		}
		if ( fPixelBorder)
			buf[1] = buf[0];
	}
}

template<class T> void ApplyRadioMetricCorrectionTIR(T& buf, const vector< vector<double> >& vvRadCorr, int iLine, 
																								long iSz, bool fUsesRaws, int iOffset , double rCoeff, bool fRadioMetricCorr = true)
{
	// when raws are used the numbers read need to be scaled to map the ILWIS idea of a raw value. 	
	double rFactor = 1.0;
	if ( fUsesRaws)
		rFactor = 1.0/ rPRECISION;

	bool f1A = vvRadCorr.size() != 0;
	double rC0, rC1, rC2;
	if ( f1A )
	{
		int iDetector = abs( iLine - iOffset)  % 10;
		rC0 = vvRadCorr[iDetector][0];
		rC1 = vvRadCorr[iDetector][1];
		rC2 = vvRadCorr[iDetector][2];
	}		

	// the buf may not have the correct size at all times, for some reason the drawer always uses buffers that are one pixel larger
	// while others use the correct size. We must have the correct size though	
	int iCount = min(iSz, buf.iSize());
	
	for (int i = 0; i < iCount  ; ++i )	
	{
		double rV = buf[i];
		if ( f1A)
		{
			rV = rFactor * ( rC0 + rC1 * rV + rC2 * rV * rV) ;
			if ( !fRadioMetricCorr )
			{
				buf[i] = rV / rCoeff;
				buf[i] = min(4094, max(0,buf[i]));
			}
			else
			{
				buf[i] = max(0,rV);
			}				
		}				
		else
			if ( fRadioMetricCorr)
			{
				buf[i] = rFactor * (rV - 1.0 ) * rCoeff ;
				buf[i] = max(0, buf[i]);
			}				
			else
			{
				buf[i] = rFactor * rV;
				buf[i] = min(4094, max(0,buf[i]));				
			}				
	}					
}

/*CoordSystem ASTERFormat::csGetCoordSystem()
{
}

void ASTERFormat::GetGeoRef(GeoRef& grf, ASTERBands abpType)
{
}*/

void ASTERFormat::GetLineRaw(long iLine, ByteBuf& buf, long iFrom, long iNum) const
{
	if ( pImageData)
	{
		// get the dn values of the hdf file
		pImageData->GetRow(iLine, buf, iFrom, iNum);
		if (aplLevel != apl1DEM)
		{
			// trasnform them to radiances
			double rCoeff = rUnitConversion[(int)band] ;
			if ( band <= abSWIR9 )
				ApplyRadioMetricCorrectionVNIRSWIR(buf, iFrom, vvRadCorr, clMapSize.iCoord(HDFData::Cell::aY), true && fUseRadiances, rCoeff, fUseRadiances);
			else if ( band <= abTIR14 )
				ApplyRadioMetricCorrectionTIR(buf, vvRadCorr, iLine, clMapSize.iCoord(HDFData::Cell::aY), true && fUseRadiances, iTirOffset, rCoeff , fUseRadiances);
		}		
	}		
}

void ASTERFormat::GetLineRaw(long iLine, IntBuf& buf,  long iFrom, long iNum) const
{
	if ( pImageData)
	{
		// get the dn values of the hdf file		
		pImageData->GetRow(iLine, buf, iFrom, iNum);
		if (aplLevel != apl1DEM)
		{
			// trasnform them to radiances	
			double rCoeff = rUnitConversion[(int)band] ;	
			if ( band <= abSWIR9)
				ApplyRadioMetricCorrectionVNIRSWIR(buf, iFrom, vvRadCorr, clMapSize.iCoord(HDFData::Cell::aY), true && fUseRadiances, rCoeff, fUseRadiances);
			else if ( band <= abTIR14 )
				ApplyRadioMetricCorrectionTIR(buf, vvRadCorr, iLine, clMapSize.iCoord(HDFData::Cell::aY), true && fUseRadiances, iTirOffset, rCoeff, fUseRadiances);
			for ( int i=0; i < buf.iSize(); ++i )
				if ( buf[i] <=0 )
					buf[i] = shUNDEF;	
		}
	}		
}

void ASTERFormat::GetLineRaw(long iLine, LongBuf& buf, long iFrom, long iNum) const
{
	if ( pImageData)
	{
		// get the dn values of the hdf file		
		pImageData->GetRow(iLine, buf, iFrom, iNum);
		if (aplLevel != apl1DEM)
		{
			// trasnform them to radiances	
			double rCoeff = rUnitConversion[(int)band] ;	
			if ( band <= abSWIR9)
				ApplyRadioMetricCorrectionVNIRSWIR(buf, iFrom, vvRadCorr, clMapSize.iCoord(HDFData::Cell::aY), true && fUseRadiances, rCoeff, fUseRadiances);
			else if ( band <= abTIR14 )
				ApplyRadioMetricCorrectionTIR(buf, vvRadCorr, iLine, clMapSize.iCoord(HDFData::Cell::aY), true && fUseRadiances, iTirOffset, rCoeff, fUseRadiances);	
			for ( int i=0; i < buf.iSize(); ++i )
				if ( buf[i] <=0 )
					buf[i] = iUNDEF;
		}	
	}			
}

void ASTERFormat::GetLineVal(long iLine, LongBuf& buf, long iFrom, long iNum) const
{
	if ( pImageData)
	{
		// get the dn values of the hdf file				
		pImageData->GetRow(iLine, buf, iFrom, iNum);
		if (aplLevel != apl1DEM)
		{
			// trasnform them to radiances	
			double rCoeff = rUnitConversion[(int)band];		
			if ( band <= abSWIR9)
				ApplyRadioMetricCorrectionVNIRSWIR(buf, iFrom, vvRadCorr, clMapSize.iCoord(HDFData::Cell::aY), false, rCoeff, fUseRadiances);
			else if ( band <= abTIR14 )
				ApplyRadioMetricCorrectionTIR(buf, vvRadCorr, iLine, clMapSize.iCoord(HDFData::Cell::aY), false, iTirOffset, rCoeff, fUseRadiances);
			for ( int i=0; i < buf.iSize(); ++i )
				if ( buf[i] <=0 )
					buf[i] = iUNDEF;			
		}
	}		
}

void ASTERFormat::GetLineVal(long iLine, RealBuf& buf, long iFrom, long iNum) const
{
	if ( pImageData)
	{
		// get the dn values of the hdf file						
		pImageData->GetRow(iLine, buf, iFrom, iNum);
		if (aplLevel != apl1DEM)
		{
			// trasnform them to radiances	
			double rCoeff = rUnitConversion[(int)band]	;			
			if ( band <= abSWIR9)
				ApplyRadioMetricCorrectionVNIRSWIR(buf, iFrom, vvRadCorr, clMapSize.iCoord(HDFData::Cell::aY), false, rCoeff, fUseRadiances);
			else if ( band <= abTIR14 )
				ApplyRadioMetricCorrectionTIR(buf, vvRadCorr, iLine, clMapSize.iCoord(HDFData::Cell::aY), false, iTirOffset, rCoeff, fUseRadiances);
		
			for ( int i=0; i < buf.iSize(); ++i )
				if ( buf[i] <=0 )
					buf[i] = rUNDEF;
		}
	}	
}

long ASTERFormat::iRaw(RowCol rc) const
{
	LongBuf buf(1);
	if (pImageData)
	{
		RowCol rc2(max(0, rc.Row), max(0, rc.Col));
		buf[0] =  pImageData->iCellValue(HDFData::Cell(rc2.Row, rc2.Col));
		pImageData->GetRow(rc2.Row, buf, rc2.Col, 1);
		double rCoeff = rUnitConversion[(int)band];
		if ( band <= abSWIR9)
			ApplyRadioMetricCorrectionVNIRSWIR(buf, rc2.Col, vvRadCorr, 1, true && fUseRadiances, rCoeff, fUseRadiances);
		else if ( band <= abTIR14 )
			ApplyRadioMetricCorrectionTIR(buf, vvRadCorr, rc2.Row, 1, true && fUseRadiances, iTirOffset, rCoeff, fUseRadiances);
		if(buf[0] <= 0)
			return iUNDEF;
		return buf[0];
	}		
	return iUNDEF;	
}

double ASTERFormat::rValue(RowCol rc) const
{
	RealBuf buf(1); 
	if (pImageData)
	{
		double rCoeff = rUnitConversion[(int)band];		
		buf[0] =  pImageData->rCellValue(HDFData::Cell(rc.Row, rc.Col));
		if ( fUseRadiances && band <= abSWIR9)
			ApplyRadioMetricCorrectionVNIRSWIR(buf, rc.Col, vvRadCorr, 1, false, rCoeff, fUseRadiances);
		else if ( fUseRadiances && band <= abTIR14 )
			ApplyRadioMetricCorrectionTIR(buf, vvRadCorr, rc.Row, 1, false, iTirOffset, rCoeff, fUseRadiances);
		if(buf[0] <= 0)
			return rUNDEF	;	
		return buf[0];
	}		
	return rUNDEF;
}

long ASTERFormat::iValue(RowCol rc) const
{
	LongBuf buf(1); 
	if (pImageData)
	{
		double rCoeff =  rUnitConversion[(int)band];			
		buf[0] =  pImageData->iCellValue(HDFData::Cell(rc.Row, rc.Col));		
		if ( band <= abSWIR9)
			ApplyRadioMetricCorrectionVNIRSWIR(buf, rc.Col, vvRadCorr, 1, false, rCoeff, fUseRadiances);
		else if ( band <= abTIR14 )
			ApplyRadioMetricCorrectionTIR(buf, vvRadCorr, rc.Row, 1, false, iTirOffset, rCoeff, fUseRadiances);
		if(buf[1] <= 0)
			return iUNDEF	;	
		return buf[0];
	}		
	return iUNDEF;
}

bool ASTERFormat::fIsSupported(const FileName& fn, ForeignFormat::Capability dtType) const
{
	return dtType == ForeignFormat::cbIMPORT;
}

// metadata are large chunks of text with lost of whitespace. To facilitate searching in them white space are 
// removed. This makes search sentences easier to formulate. It will probabaly block some searches but for the
// data we need from the metadata this poses no problem
String ASTERFormat::sStripAllWhiteSpace(const String& sMetaData)
{
	String sNewMetaData;

	for(int i=0; i<sMetaData.size(); ++i)
	{
		char c = sMetaData[i];
		if ( c > 0 && isspace(c) == 0)
		{
			sNewMetaData += c;
		}
	}

	return sNewMetaData;
}

// retrieves from the metadata a value string from a group/object structure. if needed the object may be followed by
// a class name with a dot seperator. e.g inlc.1
String ASTERFormat::sGetASTERMetaDataValue(const String& sMetaData, const String& sGroup, const String& sObject)
{
	if ( sMetaData == "")
		return "";
	String sMetaD = sStripAllWhiteSpace(sMetaData);
	String sClass;
	size_t iClassBegin = sObject.find(".");
	if ( iClassBegin != String::npos)
		sClass = sObject.substr(iClassBegin + 1, sObject.size() - iClassBegin - 1);
	String sGr = "GROUP=" + sGroup;
	size_t iBeginGroup=sMetaD.find(sGr);
	size_t iEndGroup = sMetaD.find(sGr, iBeginGroup + 1);
	String sObj;
	if ( sClass != "")
		sObj = "OBJECT=" + sObject.substr(0, iClassBegin ) + "CLASS=\"" + sClass + "\"";
	else
		sObj = "OBJECT=" + sObject;
	size_t iBeginObject = sMetaD.find(sObj, iBeginGroup);
	size_t iBeginValue = sMetaD.find("VALUE", iBeginObject);
	size_t iEndValue = sMetaD.find("END_OBJECT", iBeginObject);
	String sValue = sMetaD.substr(iBeginValue + 6, iEndValue - iBeginValue - 6);;	

	return sValue;;
}

// translates an expression (read from an ODF) into a parmlist
void ASTERFormat::ReadParameters(const FileName& fnObj, ParmList& pm) 
{
	ForeignFormat::ReadParameters(fnObj, pm);
	String sV;
	if ( !pm.fExist("method")) {
		sV = "";
		ObjectInfo::ReadElement("ForeignFormat","Method",fnObj,sV);
		pm.Add(new Parm("method", sV));
	}
	if ( !pm.fExist("layer")) {
		sV = "";
		ObjectInfo::ReadElement("ForeignFormat","Layer",fnObj, sV);
		pm.Add( new Parm("layer",sV));
	}
	if ( !pm.fExist("useradiances")) {
		bool fUR;
		ObjectInfo::ReadElement("ForeignFormat","UseRadiances",fnObj, fUR);
		pm.Add( new Parm("useradiances",fUR));
	}
	iLayer = pm.sGet("layer").iVal();
		
}

void ASTERFormat::Store(IlwisObject ob) {
	ForeignFormat::Store(ob);
	ob->WriteElement("ForeignFormat","Method","ASTER");
	ob->WriteElement("ForeignFormat","Layer",iLayer);
	ob->WriteElement("ForeignFormat","UseRadiances",fUseRadiances);
}

bool ASTERFormat::fIsThreaded()
{
	return fThreaded;
}


#define ONE_TOL	 1.00000000000001
	
double ASTERFormat::aasin(double v) // a safer inverse sine (arcsin) that always works
{
	double av;

	if ((av = abs(v)) >= 1.) {
		if (av > ONE_TOL)
			return rUNDEF;
		return (v < 0. ? - M_PI_2 : M_PI_2);
	}
	return asin(v);
}

double rGetPsi(long iScene) 
{
	return ASTERFormat::aasin(cos(2 * M_PI * (iScene - 0.5)/670) * sin (81.8 * M_PI / 180) );
		//The geocentric latitude for the iSceneNr == k can be calculated as 
		// psi  = asin[cos{360(k - 0.5)/Kmax }sin a ] 
		//where Kmax = 670 and a is the complementary angle of the orbit inclination, that is, 81.8°.
		//The geocentric latitude psi can be converted into the geodetic latitude phi
		//using the following relation.phi = atan(C tan psi) [ Aster User's Guide Jan 2001
}

double rGetLambda(long iK, long iPath) 
{	
	double rLambdaEquator;
	if (iPath < 76)
		rLambdaEquator = -64.6 - 360 *(iPath - 1) / 233;
	else
		rLambdaEquator = 295.4 - 360 *(iPath - 1) / 233;
	rLambdaEquator *= M_PI / 180; // compute in radians
	long Kmax = 670; // total nr of rows in one path
	double a = 81.8 * M_PI / 180; //orbit inclination
	double T = 98.884; //( = 16 x 24 x 60/233 the orbit period (revolution time) in minutes)
	double w = 0.0041666667;//( = 360 / 86400  the angular velocity of the rotating earth in degr/sec)
	double rLambdaLocal, rAngle1, rAngle2;
	if (iK > 0 && iK < 336) {	//descending part of path
		rAngle1 = 360*(168 - iK)/Kmax * M_PI / 180;
		rAngle2 = (168 - iK)*(T*w/Kmax) * M_PI / 180;
		rLambdaLocal = atan(tan(rAngle1)*cos(a)) + rAngle2 + rLambdaEquator;
	}
	else {	// ascending part of path 
		rAngle1 = 360*(503 - iK)/Kmax * M_PI / 180;
		rAngle2 = (180 + (168 - iK)*(T*w/Kmax)) * M_PI / 180;
		rLambdaLocal = atan(tan(rAngle1)*cos(a)) + rAngle2 + rLambdaEquator;
	}
	while (rLambdaLocal >  M_PI) rLambdaLocal -= 2 * M_PI;  
  while (rLambdaLocal < -M_PI) rLambdaLocal += 2 * M_PI;
	return rLambdaLocal;
}

void ASTERFormat::GetCoordSystem(CoordSystem& cs)
{
	FileName fnCoord(fnBaseOutputName.sFile, ".csy");
	fnCoord.Dir(fnBaseOutputName.sPath());		
	fnCoord = FileName::fnUnique(fnCoord);
	CoordSystemProjection *	cspr = new CoordSystemProjection(fnCoord, 1);
	cs.SetPointer(cspr);
	cspr->datum = new MolodenskyDatum("WGS 1984","");
	cspr->ell = cspr->datum->ell;
		
	HDFData *da = HDFData::Create(fnGetForeignFile(), HDFData::daDataSet, "");
	String sInventory = da->sGetAttribute("coremetadata.0");
	String s4Corners = da->sGetAttribute("productmetadata.0");
	String sSWATHgroup = da->sGetAttribute("Structmetadata.0");
	String s = da->sGetAttribute("productmetadata.0");
	delete da;
	//String sPACLASS_1_VNIR = sGetASTERMetaDataValue(s, "POINTINGANGLES", "SENSORNAME.1");
	String sPointingAngleVNIR = sGetASTERMetaDataValue(s, "POINTINGANGLES", "POINTINGANGLE.1");
	double rPointingAngleVNIR = sPointingAngleVNIR.rVal();
	String sPointingAngleSWIR = sGetASTERMetaDataValue(s, "POINTINGANGLES", "POINTINGANGLE.2");
	double rPointingAngleSWIR = sPointingAngleSWIR.rVal();
	String sPointingAngleTIR = sGetASTERMetaDataValue(s, "POINTINGANGLES", "POINTINGANGLE.3");
	double rPointingAngleTIR = sPointingAngleTIR.rVal();
	String sSceneId = sGetASTERMetaDataValue(s, "SCENEINFORMATION", "ASTERSCENEID");
	String sSceneNr = sSceneId.sTail(",").sHead(",");
	long iSceneNr = sSceneNr.iVal();
	double rPsi = rGetPsi(iSceneNr);
	String sPathNr = sSceneId.sTail("(").sHead(",");
	long iPathNr = sPathNr.iVal();
	double rLambda = rGetLambda(iSceneNr, iPathNr);
	if (sInventory != "" )
	{
		String sName = sGetASTERMetaDataValue(sInventory,"INVENTORYMETADATA" , "MAPPROJECTIONNAME");
		String sVw = sGetASTERMetaDataValue(s4Corners,"SCENEFOURCORNERS" , "UPPERLEFT");
		String sVn = sGetASTERMetaDataValue(s4Corners,"SCENEFOURCORNERS" , "LOWERLEFT");
		String sVe = sGetASTERMetaDataValue(s4Corners,"SCENEFOURCORNERS" , "UPPERRIGHT");
		String sVs = sGetASTERMetaDataValue(s4Corners,"SCENEFOURCORNERS" , "LOWERRIGHT");
		String sULLat = sVw.sHead(",");
		String sLLLon = sVn.sTail(",");
		String sURLon = sVe.sTail(",");
		String sLRLat = sVs.sHead(",");
		sULLat = sULLat.sTail("(");
		sLRLat = sLRLat.sTail("(");
		sURLon = sURLon.sHead(")");
		sLLLon = sLLLon.sHead(")");
		
		double rMinLon = sLLLon.rVal();
		double rMinLat = sLRLat.rVal();		
		double rMaxLon = sURLon.rVal();		
		double rMaxLat = sULLat.rVal();	

		cs->SetAdditionalInfoFlag(cs.fValid());
		String sAddInfo = String("Coordinate system: ");
		String sPrj;
		// UniversalTransverseMercator  case:
		double rCentralMeridian = floor((rMinLon + rMaxLon)/12)*6 + 3;
		if (_stricmp(sName.c_str(), "\"UniversalTransverseMercator\"") == 0  // level 1b
			|| _stricmp(sName.c_str(), "\"N/A\"") == 0)  // "N/A" is the case for level 1a  
		{
			Projection prj = cspr->prj;//
			sPrj = String("UTM");
			prj = Projection(sPrj, cspr->ell);
			cspr->prj = prj;
			long iUTMZoneNr = (180 + rCentralMeridian) / 6 + 0.5;
			ProjectionParamValue pv = pvZONE;
			prj->Param(pv, iUTMZoneNr);
			bool fNorthernHemisphere = ((rMinLat + rMaxLat) / 2 ) > 0;
			pv = pvNORTH;
			prj->Param(pv, (long)fNorthernHemisphere);

			sAddInfo &= sPrj;
		  sAddInfo &= String("\r\nZone: %i \r\n", iUTMZoneNr);
			sAddInfo &= String(SPRJInfoCentralMeridian_f.scVal(),cspr->prj->lam0*180/M_PI);
			sAddInfo &= String("\r\n");
			sAddInfo &= String(SPRJInfoCentralScaleF_f.scVal(),cspr->prj->rGetCentralScaleFactor());
		}
		// other projections e.g. 'conformal conic' , 'oblique mercator' etc. still to be implemented
		else {
			sPrj = String("UNKNOWN");
			sAddInfo &= sPrj;
			cs = CoordSystem(sPrj);
		}
		sAddInfo &= String("\r\nGeographic area:");
		sAddInfo &= String("\r\nLatitude limits: %.2f, %.2f", rMinLat, rMaxLat);
		sAddInfo &= String("\r\nLongitude limits: %.2f, %.2f", rMinLon, rMaxLon);
		sAddInfo &= String("\r\nScene (Row-) Number: %i", iSceneNr);
		sAddInfo &= String("\r\nOrbit Path Number: %i", iPathNr); 
		sAddInfo &= String("\r\nGeocentric Latitude at Row %i is %.4f", iSceneNr,rPsi*180/M_PI);
		double rGeodetLat= atan(rLatitudeCorrFactor * tan (rPsi)); // Aster User's guide Part1 Jan 2001
		sAddInfo &= String("\r\nGeodetic Latitude at Row %i is %.4f", iSceneNr,rGeodetLat*180/M_PI);
		sAddInfo &= String("\r\nLongitude at Row %i is %.4f", iSceneNr,rLambda*180/M_PI);

		sAddInfo &= String("\r\nPointing Angle VNIR Sensor:  %.6f deg", rPointingAngleVNIR);
		sAddInfo &= String("\r\nPointing Angle SWIR Sensor:  %.6f deg", rPointingAngleSWIR);
		sAddInfo &= String("\r\nPointing Angle TIR Sensor:  %.6f deg", rPointingAngleTIR);
		if (cs.fValid())
			cs->SetAdditionalInfo(sAddInfo);
		cs->fErase = true;			
		cs->SetDescription(String(SIEDscCsyFor_sUsingAsterImport.scVal(), fnGetForeignFile().sFile));
		cs->Store();
		fCsyCreatedAndStored = true;
		cs->fErase = true;		
	}
}

void ASTERFormat::GetGeoRef(GeoRef& grf, 	CoordSystem& csy, ASTERBands abpType, const FileName& fnNew) 
{
	HDFData *pHdfImage;
	String sName = m_BandNames[abpType];
	String sPath;// = sNameToPath(aplLevel == apl1A ? ab1A_X_IMAGEDATA : ab1B_X_IMAGEDATA, sName);
	if (aplLevel == apl1A )
		sPath = sNameToPath(ab1A_X_IMAGEDATA, sName);
	else if (aplLevel == apl1B)
		sPath = sNameToPath(ab1B_X_IMAGEDATA, sName);
	else 
		sPath = sNameToPath(ab1B_DEM_IMAGEDATA, sName);
	pHdfImage = HDFData::Create(fnGetForeignFile(), HDFData::daDataSet, sPath);
	
	/////squared(!) ratio of the Earth radii a and b [Aster User's Guide Part1 Jan 2001
	rLatitudeCorrFactor = 1.0067394967422764;// =(a*a)/(b*b); a/b = 1.003364089837 (for WGS84) 
	if (!fCsyCreatedAndStored && (aplLevel != apl1DEM ))
		GetCoordSystem(csy);	
	else if (aplLevel == apl1DEM )
		GetCoordSystemDem(csy);	
	
	HDFData::Cell clSize = pHdfImage->clGetSize();
	FileName fnGeo = FileName::fnUnique(FileName(fnNew, ".grf"));
	RowCol rcSize( clSize.iCoord(HDFData::Cell::aX), clSize.iCoord(HDFData::Cell::aY));
	GeoRefCTPplanar* gcp = new GeoRefCTPplanar(fnGeo, csy, rcSize);
	gcp->fnBackgroundMap = fnNew;
	grf.SetPointer(gcp);
	Table tblCTP = gcp->tbl();
	Column colRow = tblCTP->col("Row");
	Column colCol = tblCTP->col("Col");		
	Column colX = tblCTP->col("X");				
	Column colY = tblCTP->col("Y");	

	
	int iCount = 1; // counts the tiepoints read
	LatLon llTie, llWGS84;
	
	if ( pHdfImage && aplLevel == apl1A)
	{
		sPath = sNameToPath(ab1A_X_LATITUDE, sName);
		HDFData *dsLat = HDFData::Create(fnGetForeignFile(), HDFData::daDataSet, sPath);
		sPath = sNameToPath(ab1A_X_LONGITUDE, sName);
		HDFData *dsLong = HDFData::Create(fnGetForeignFile(), HDFData::daDataSet, sPath);
		sPath = sNameToPath(ab1A_X_LATTICEPOINT, sName);
		HDFData *dsLatticeRowCol = HDFData::Create(fnGetForeignFile(), HDFData::daDataSet, sPath);
		if ( ! (dsLat->fValid() && dsLong->fValid() && dsLatticeRowCol->fValid()) )
				throw ErrorObject(String(SIErrrNoGeoReferencingData.scVal(), fnGetForeignFile().sRelative()));

		long iLatticeWidth = dsLatticeRowCol->clGetSize().iCoord(HDFData::Cell::aY);
		long iLatticeHeight = dsLatticeRowCol->clGetSize().iCoord(HDFData::Cell::aX);
		tblCTP->iRecNew(iLatticeWidth * iLatticeHeight);
		sPath = sNameToPath(ab1A_X_SIGHTVECTOR, sName);
		HDFData *dsSightVectors = HDFData::Create(fnGetForeignFile(), HDFData::daDataSet, sPath);
		long iSightVWidth = dsSightVectors->clGetSize().iCoord(HDFData::Cell::aZ);
		long iSightVHeight = dsSightVectors->clGetSize().iCoord(HDFData::Cell::aY);
		long iSightVDepth = dsSightVectors->clGetSize().iCoord(HDFData::Cell::aX);
		DomainValueRangeStruct dvrsCosine(-1,1,0.00000001); 
		Column colcosA = tblCTP->colNew("cosAlpha", dvrsCosine);
		colcosA->SetReadOnly();
	  colcosA->SetOwnedByTable(true);
		colcosA->sDescription = "X-comp of unit vector along sight-line";
		Column colcosB = tblCTP->colNew("cosBeta", dvrsCosine);
		colcosB->SetReadOnly();
	  colcosB->SetOwnedByTable(true);
		colcosB->sDescription = "Y-comp of unit vector along sight-line";
		Column colcosC = tblCTP->colNew("cosGamma", dvrsCosine);
		colcosC->SetReadOnly();
	  colcosC->SetOwnedByTable(true);
		colcosC->sDescription = "Z-comp of unit vector along sight-line";
		for (int i = 0; i < iLatticeHeight; i++) 
		{
			for (int j = 0; j < iLatticeWidth; j++) 
			{
				colRow->PutVal(iCount, dsLatticeRowCol->iCellValue(HDFData::Cell(i,j,1)));
				colCol->PutVal(iCount, dsLatticeRowCol->iCellValue(HDFData::Cell(i,j,0)));
				double rLat = dsLat->rCellValue(HDFData::Cell(i,j,0));
				double rLon = dsLong->rCellValue(HDFData::Cell(i,j,0));
				LatLon llTie(rLat, rLon);
				llWGS84.Lon = llTie.Lon;
				llWGS84.Lat = atan(rLatitudeCorrFactor * tan (llTie.Lat * M_PI / 180)); //  Aster User's guide
				llWGS84.Lat *= 180 / M_PI;
				Coord crdProjected = csy->cConv(llWGS84);
				colX->PutVal(iCount, crdProjected.x);
				colY->PutVal(iCount, crdProjected.y);
				gcp->SetActive(iCount, true);
				colcosA->PutVal(iCount, dsSightVectors->rCellValue(HDFData::Cell(i,j,0)));
				colcosB->PutVal(iCount, dsSightVectors->rCellValue(HDFData::Cell(i,j,1)));
				colcosC->PutVal(iCount, dsSightVectors->rCellValue(HDFData::Cell(i,j,2)));
				++iCount;
			}
		}
		if (iCount > 12)
			gcp->transf = GeoRefCTP::THIRDORDER;
		delete dsLat;
		delete dsLong;  
		delete dsLatticeRowCol;
		delete dsSightVectors;
	}	
	else if (pHdfImage && aplLevel == apl1B )
	{
		sPath = sNameToPath(ab1B_X_LATITUDE, sName);
		HDFData *dsLat = HDFData::Create(fnGetForeignFile(), HDFData::daDataSet, sPath);
		sPath = sNameToPath(ab1B_X_LONGITUDE, sName);
		HDFData *dsLong = HDFData::Create(fnGetForeignFile(), HDFData::daDataSet, sPath);
		
		tblCTP->iRecNew(4);
		//C = 1.000674; // trial
		CoordBounds cbLaLo = CoordBounds();
		long iIncremRow = rcSize.Row / 10;
		long iIncremCol = rcSize.Col / 10;
		long iOffSetRow = 0; 
		if (_stricmp(sName.c_str(), "VNIR:3B") == 0) {
			iOffSetRow = 400; // difference betw 3N and 3B !!
			iIncremRow = 420; // for 3B not size/10 !!
		}
		for (int i = 0; i < 11; i += 10) // USE ONLY FIRST AND LAST PT OF TOP LINE
		{															// AND FIRST AND LAST PT OF BOTTOM LINE
			for (int j = 0; j < 11; j += 10)  // TO MAKE A GEOREF AFFINE (ONE PER SWATH GROUP ?)
			{
				colRow->PutVal(iCount, (long)i * iIncremRow + iOffSetRow);
				colCol->PutVal(iCount, (long)j * iIncremCol);
				double rLat = dsLat->rCellValue(HDFData::Cell(i,j,0));
				double rLon = dsLong->rCellValue(HDFData::Cell(i,j,0));
				LatLon llTie(rLat, rLon);
				llTie.Lon = llTie.Lon;
				llTie.Lat = atan(rLatitudeCorrFactor * tan (llTie.Lat * M_PI / 180)); //Aster User's guide
				llTie.Lat *= 180 / M_PI;
				Coord crdProjected = csy->cConv(llTie);
				colX->PutVal(iCount, crdProjected.x);
				colY->PutVal(iCount, crdProjected.y);
				gcp->SetActive(iCount, true);
				++iCount;
				cbLaLo += Coord(llTie.Lon,llTie.Lat);
			}
		}
		gcp->transf = GeoRefCTP::AFFINE;
		delete dsLat;
		delete dsLong;  
	}
	else if (pHdfImage && aplLevel == apl1DEM )
	{
		sPath = sNameToPath(ab1B_DEM_LATITUDE, sName);
		HDFData *dsLat = HDFData::Create(fnGetForeignFile(), HDFData::daDataSet, sPath);
		
		long iLatticeWidth = dsLat->clGetSize().iCoord(HDFData::Cell::aY);
		long iLatticeHeight = dsLat->clGetSize().iCoord(HDFData::Cell::aZ);
		tblCTP->iRecNew(iLatticeWidth * iLatticeHeight);
		long iIncremRow = 301;
		long iIncremCol = 314;
		iLatticeWidth = 8 ;
		iLatticeHeight = 8 ;
	
		for (int i = 0; i < iLatticeWidth; i++)		//iLatticeWidth = 8 ?
		{
			for (int j = 0; j < iLatticeHeight; j++) // iLatticeHeight = 8 ?
			{
				colRow->PutVal(iCount, (long)i * iIncremRow);
				colCol->PutVal(iCount, (long)j * iIncremCol);
				double rLat = dsLat->rCellValue(HDFData::Cell(0,i,j));
				double rLon = dsLat->rCellValue(HDFData::Cell(1,i,j));
				double rLatt = dsLat->rCellValue(HDFData::Cell(0,7,7));
				double rLont = dsLat->rCellValue(HDFData::Cell(1,7,7));
				LatLon llTie(rLat, rLon);
				llWGS84.Lon = llTie.Lon;
				llWGS84.Lat = llTie.Lat;
				//llWGS84.Lat = atan(rLatitudeCorrFactor * tan (llTie.Lat * M_PI / 180)); //  Aster User's guide
				//llWGS84.Lat *= 180 / M_PI;
				Coord crdProjected = csy->cConv(llWGS84);
				colX->PutVal(iCount, crdProjected.x);
				colY->PutVal(iCount, crdProjected.y);
				gcp->SetActive(iCount, true);
				++iCount;
			}
		}
		if (iCount > 12)
			gcp->transf = GeoRefCTP::THIRDORDER;
		delete dsLat; 
	}
	grf->SetDescription(String(SIEDscGrfFor_sUsingAsterImport.scVal(), fnNew.sFile));
	grf->Updated();
	grf->fErase = true;
	delete pHdfImage;
}

void ASTERFormat::GetCoordSystemDem(CoordSystem& cs)
{
	FileName fnCoord(fnBaseOutputName.sFile, ".csy");
	fnCoord.Dir(fnBaseOutputName.sPath());		
	fnCoord = FileName::fnUnique(fnCoord);
	CoordSystemProjection *	cspr = new CoordSystemProjection(fnCoord, 1);
	cs.SetPointer(cspr);
	cspr->datum = new MolodenskyDatum("WGS 1984","");
	cspr->ell = cspr->datum->ell;
	String sName = "DEM";
	String sPath = sNameToPath(ab1B_DEM_LATITUDE, sName);// also longitudes are there
	HDFData *dsLong = HDFData::Create(fnGetForeignFile(), HDFData::daDataSet, sPath);
	// find proper utm zone and hemisphere
	double rLon, rLonMin = 180;
	double rLonMax = -180;
	double rLat, rLatMin = 90;
	double rLatMax = -90; 
	for (int i = 0; i < 8; i++)		//iLatticeWidth = 8 ?
	{
		for (int j = 0; j < 8; j++) // iLatticeHeight = 8 ?
		{
			rLat = dsLong->rCellValue(HDFData::Cell(0,i,j));
			rLatMin = min(rLat, rLatMin);
			rLatMax = max(rLat, rLatMax);
			rLon = dsLong->rCellValue(HDFData::Cell(1,i,j));
			rLonMin = min(rLon, rLonMin);
			rLonMax = max(rLon, rLonMax);
		}
	}
	double rCentralMeridian = floor((rLonMin + rLonMax)/12)*6 + 3;
	Projection prj = cspr->prj;//
	String sPrj = String("UTM");
	prj = Projection(sPrj, cspr->ell);
	cspr->prj = prj;
	long iUTMZoneNr = (long)((180 + rCentralMeridian) / 6 + 0.5);
	ProjectionParamValue pv = pvZONE;
	prj->Param(pv, iUTMZoneNr);
	bool fNorthernHemisphere = ((rLatMin + rLatMax) / 2 ) > 0;
	pv = pvNORTH;
	prj->Param(pv, (long)fNorthernHemisphere);
	cs->fErase = true;			
	cs->SetDescription(String(SIEDscCsyFor_sUsingAsterImport.scVal(), fnGetForeignFile().sFile));
	cs->Store();
	fCsyCreatedAndStored = true;
	cs->fErase = true;		
}

void ASTERFormat::getImportFormats(vector<ImportFormat>& formats) {
	ImportFormat frm;
	frm.type = ImportFormat::ifRaster;
	frm.name = "Advanced Spaceborne Thermal Emission and Reflection Radiometer";
	frm.shortName = "Aster";
	frm.method = "ASTER";
	frm.provider = frm.method;
	frm.useasSuported = true;
	frm.ext = "*";
	frm.ui = NULL;
	formats.push_back(frm);
}


