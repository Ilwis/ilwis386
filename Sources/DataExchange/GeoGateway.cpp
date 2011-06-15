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
#define DomainInfo ggDomainInfo

#define ILWIS_LIC

#include "GeoGateway\idb.h"
#include "GeoGateway\gdbfrmts.h"
#include "GeoGateway\ccltask.h"
#include "GeoGateway\Gdbtopo.h"

#undef DomainInfo

#pragma warning( disable : 4786 )

#include "Headers\toolspch.h"

#pragma warning( disable : 4715 )

#include <set>
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Engine\SpatialReference\Coordsys.h"
#include "Engine\SpatialReference\Csproj.h"
#include "Engine\SpatialReference\Cslatlon.h"
#include "Engine\SpatialReference\Ellips.h"
#include "Engine\Domain\dmcoord.h"
#include "Engine\Domain\dminfo.h"
#include "Engine\Domain\dmsort.h"
#include "Engine\Domain\DomainUniqueID.h"
#include "Engine\DataExchange\TableForeign.h"
#include "Engine\Base\File\objinfo.h"
#include "Engine\SpatialReference\prj.h"
#include "Engine\Map\Segment\Seg.h"
#include "Engine\Map\Polygon\POLAREA.H"
#include "Engine\Map\Point\PNT.H"
#include "Engine\DataExchange\ForeignFormat.h"
#include "Engine\DataExchange\GeoGateway.h"
#include "Engine\SpatialReference\Grcornrs.h"
#include "Engine\Base\File\Directory.h"
#include "Engine\Map\Raster\Map.h"
#include "Engine\Map\Raster\MapList\maplist.h"
#include "Client\ilwis.h"
#include "Engine\Base\DataObjects\Tranq.h"
#include "Headers\constant.h"
#include "Headers\messages.h"
#include "Headers\Hs\IMPEXP.hs"
#include "Headers\Htp\Ilwismen.htp"


map<int, FormatInfo> mapDummy;
map<int, FormatInfo> GeoGatewayFormat::mapFormatInfo = mapDummy;

static const int iGDB_NOT_FOUND = -1;
const String sZCOLUMNNAME("Z_Value");
CCriticalSection GeoGatewayFormat::m_CriticalSection;

class StopConversion : public ErrorObject
{};

class ReadingLayerError : public ErrorObject
{
public:
		ReadingLayerError(int iLayer) :
			ErrorObject(String(SIEErrErrorReadingLayer_i.scVal(), iLayer)) {}
};

//----------------------------------------------------------------------------------------
bool FormatInfo::fSupports(FormatInfo::ExpProp prop)
{
	int iPlace = (int)prop;
	return sExportProperties[iPlace] == '1';
}
//----------------------------------------------------------------------------------------

GeoGatewayFormat::GeoGatewayFormat()
{
	iLayer = iUNDEF;
	for(int i = 0; i < 3; ++i)
		fdp[i] = NULL;	
	buffer = NULL;
	fCombine = f4BytesInt = false;
	iGDBID = iUNDEF;
	fShowCollection = false;
}

ForeignFormat *GeoGatewayFormat::CreateQueryObject() //create query object
{
	return new GeoGatewayFormat();
}

ForeignFormat *GeoGatewayFormat::CreateImportObject(const FileName& fnObj, ParmList& pm) //create import object
{
	String sExpr;
	String sMethod = pm.sGet("method");
	FileName fnFO = pm.sGet(0);
	if ( pm.iSize() < 2 ) // existing object
	{
		if ( ObjectInfo::ReadElement("ForeignFormat", "Expression", fnObj, sExpr) != 0)	// oldstyle expressions
		{
			ForeignFormat *ff = ForeignFormat::Create(sMethod);
			if ( ff )
				ff->ReadExpression(fnObj, pm, sExpr);
			delete ff;
		}	
	}
//	else // new object
//	{
		return new GeoGatewayFormat(fnObj, pm);
//	}		
	return NULL;
}

GeoGatewayFormat::GeoGatewayFormat(const FileName& fn, ParmList& pm, ForeignFormat::mtMapType _mtType) :
	ForeignFormat(pm.sGet(0), _mtType),
	buffer(0),
	f4BytesInt(false),
	fCombine(false),
	iGDBID(iUNDEF),
	fImport(false),
	fExport(false),
	fShowCollection(false)
{
	for(int i = 0; i < 3; ++i)
		fdp[i] = NULL;

	int dummy;
	if ( ObjectInfo::ReadElement("ForeignFormat", "AssociatedType", fn, dummy) != 0)
		mtLoadType = (ForeignFormat::mtMapType)dummy;		
	
	fExport = pm.fExist("export");
	fImport = pm.fExist("import");
	fShowCollection = !pm.fExist("noshow");	
	
	if (!fExport)
	{

		FileName fnOutput = pm.sGet("output");
		bool fAbsolute = false;
	  if ( fnOutput != FileName() )
		{
			if ( fnOutput.sFile == "" ) // empty name, use default name
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

		fCombine = pm.fExist("combine");
		if ( pm.fExist("layer"))
			iLayer = pm.sGet("layer").iVal();

	}
	else
		InitExportMaps(fn, pm);

}

GeoGatewayFormat::~GeoGatewayFormat()
{
	CleanUp();
}

void GeoGatewayFormat::CleanUp()
{
	if ( trq )
	{
		trq->Stop();
		delete trq;
		trq = NULL;
	}
	for(int i = 0; i < 3; ++i)
	{
		if ( fdp[i] ) 
		{
			ILWISSingleLock lock(&m_CriticalSection, TRUE);	 			
			GDBClose(fdp[i] );
			fdp[i] = NULL;	
		}			
	}		
	if ( buffer) delete [] buffer;
	buffer = NULL;	
}

void GeoGatewayFormat::InitExportMaps(const FileName& fn, ParmList& pm)
{
	String sFormat = pm.sGet("format");
	FileName fnSource = pm.sGet("input");

	ILWISSingleLock lock(&m_CriticalSection, TRUE);	 	
	InitGeoGateway();		
	if ( (iGDBID = iFormatID(sFormat)) != iUNDEF)
	{
			FileName fnNew(fn);
			if ( fnNew.sExt == "")
				fnNew.sExt = mapFormatInfo[iGDBID].sExt;		
			Map mp(fnSource);	
			int iRow = mp->rcSize().Row;
			int iCol = mp->rcSize().Col;			
			DomainValueRangeStruct dvrs = mp->dvrs();
			int iBands, iDataType;
			DetermineOutputType(fnSource, iBands, iDataType);
			bool fRGB = false;
			if ( iDataType == iUNDEF) // will go the RGB
			{
				iDataType = CHN_8U;
				fRGB = true;
			}				
			if ( mapFormatInfo[iGDBID].fSupports(FormatInfo::epBANDS))
				fdp[0] = GDBCreate(iGDBID, fnNew.sFullPath().scVal(), iCol, iRow, iBands, iDataType, "");
			else
			{
				for(int i=0; i < iBands; ++i)
				{
					FileName fnN;
					if ( fRGB)
					{
						String sCol = i == 0 ? "red" : i == 1 ? "green" : "blue";
						fnN = FileName::fnUnique(FileName(String("%S_%S", fnNew.sFullPath(false), sCol), fnNew.sExt));
  				}
					else
						fnN = FileName::fnUnique(fnNew);
					fdp[i] = GDBCreate(iGDBID, fnN.sFullPath().scVal(), iCol, iRow, 1, iDataType, "");
				}					
			}				
			if ( !fdp[0] )
				throw ErrorObject(String(SIEErrNoConversionPossible.scVal(), fnForeign.sFullPath()));	
			iLayer = 1;
	}
	else
		throw ErrorObject(SIEErrFormatNotRecognized);
}

void GeoGatewayFormat::DetermineOutputType(const FileName& fnMap, int& iBands, int& iDataType) 
{
	iBands = 1;
	BaseMap mp(fnMap);

	RangeInt riRange = mp->riMinMax();
	bool fRecalc = !riRange.fValid()  ;

	
	iDataType = iUNDEF; //  if iUNDEF is the result, the map will be split into three RGB bands based on the rpr
	if ( iGDBID == iUNDEF )
	{
		FormatInfo fi = fiGetFormatInfo(fnForeign.sExt);
		iGDBID = iFormatID(fi.sFormatName);
	}		
	if ( mp->dm()->pdc() )
	{
		if ( mapFormatInfo[iGDBID].fSupports(FormatInfo::ep8UBIT) )
			iDataType = CHN_8U;		
		else if ( mapFormatInfo[iGDBID].fSupports(FormatInfo::ep16SBIT) )
			iDataType = CHN_16S;				
		else if ( mapFormatInfo[iGDBID].fSupports(FormatInfo::ep16UBIT) )
			iDataType = CHN_16U;			
	}
	else if ( mp->dm()->pdid() )
	{
		int iSizeNeeded = mp->dm()->pdsrt()->iNettoSize();
		if ( iSizeNeeded < 255 && mapFormatInfo[iGDBID].fSupports(FormatInfo::ep8UBIT) )
			iDataType = CHN_8U;
		else if ( mapFormatInfo[iGDBID].fSupports(FormatInfo::ep16UBIT) )
			iDataType = CHN_16U;
		else if ( mapFormatInfo[iGDBID].fSupports(FormatInfo::ep16SBIT) )
			iDataType = CHN_16S;			
	}
	else if ( mp->fRealValues() )
	{
		if ( mapFormatInfo[iGDBID].fSupports(FormatInfo::epREAL) )
			iDataType = CHN_32R;
	}			
	else if (  RangeInt(0, 255).fContains( mp->riMinMax(fRecalc)))
	{
		if ( mapFormatInfo[iGDBID].fSupports(FormatInfo::ep8UBIT) )
			iDataType = CHN_8U;
		else if ( mapFormatInfo[iGDBID].fSupports(FormatInfo::ep16UBIT) )
			iDataType = CHN_16U;
		else if ( mapFormatInfo[iGDBID].fSupports(FormatInfo::ep16SBIT) )
			iDataType = CHN_16S;			
	}		
	else if (  RangeInt(0, 65535).fContains( mp->riMinMax(fRecalc)))
	{
		if ( mapFormatInfo[iGDBID].fSupports(FormatInfo::ep16UBIT) )
			iDataType = CHN_16U;
	}		
	else if (  RangeInt(-32767, 32768).fContains( mp->riMinMax(fRecalc)))
	{
		if ( mapFormatInfo[iGDBID].fSupports(FormatInfo::ep16SBIT) )
			iDataType = CHN_16S;		
	}	
	if ( iDataType == iUNDEF)
		iBands = 3; // export 3 RGB colors;
}

void GeoGatewayFormat::InitGeoGateway()
{
	if ( fdp[0] ) return; // already done
	
	ILWISIMPInit( "Ilwis30", IMPFL_SGL | IMPFL_QUIET, 0, 0 );
	ALLRegister();
	IMPFatal(FALSE);

	mpCsyConvers["ACEA"]			= ProjectionConversionFunctions("Albers EqualArea Conic", SetENLLP1P2, SetForeignENLLP1P2);
	mpCsyConvers["AE"]				= ProjectionConversionFunctions("Azimuthal Equidistant", SetENLL, SetForeignENLL);	
	mpCsyConvers["AE0"]				= ProjectionConversionFunctions("Azimuthal Equidistant", SetENLL, SetForeignENLL);
	mpCsyConvers["EC"]				= ProjectionConversionFunctions( "Equidistant Conic", SetENLLP1P2,SetForeignENLLP1P2);
	mpCsyConvers["ER"]				= ProjectionConversionFunctions( "Plate Rectangle",SetENL, SetForeignENL);
	mpCsyConvers["GNO"]				= ProjectionConversionFunctions( "Gnomonic",SetENLL, SetForeignENLL );
	mpCsyConvers["LAEA"]			= ProjectionConversionFunctions( "Lambert Azimuthal EqualArea",SetENLL, SetForeignENLL );
	mpCsyConvers["LCC"]				= ProjectionConversionFunctions( "Lambert Conformal Conic",SetENLLP1P2, SetForeignENLLP1P2 );
	mpCsyConvers["MC"]				= ProjectionConversionFunctions( "Miller Cylindrical", SetENLL, SetForeignENLL );
	mpCsyConvers["MER" ]			= ProjectionConversionFunctions( "Mercator", SetENLL, SetForeignENLL );
	mpCsyConvers["OM"]				= ProjectionConversionFunctions( "Oblique Mercator", SetENLLAS, SetForeignENLLAS);
	mpCsyConvers["OG"]				= ProjectionConversionFunctions( "Orthographic", SetENLL, SetForeignENLL);
	mpCsyConvers["PC"]				= ProjectionConversionFunctions( "PolyConic", SetENNLS, SetForeignENNLS);
	mpCsyConvers["ROB"]				= ProjectionConversionFunctions( "Robinson", SetENLL, SetForeignENLL);
	mpCsyConvers["SG"]				= ProjectionConversionFunctions( "Stereographic", SetENLL, SetForeignENLL);
	mpCsyConvers["SIN"]				= ProjectionConversionFunctions( "Sinusoidal", SetENNLS, SetForeignENNLS);
	mpCsyConvers["SPCS"]			= ProjectionConversionFunctions( "Lambert Conformal Conic", SetENLL, SetForeignENLL);
	mpCsyConvers["TM"]				= ProjectionConversionFunctions( "Transverse Mercator", SetENNLS, SetForeignENNLS);
	mpCsyConvers["UTM"]				= ProjectionConversionFunctions( "UTM", NULL );
	mpCsyConvers["LONG/LAT"]	= ProjectionConversionFunctions( "LatLon", NULL);

	if ( mapFormatInfo.size() == 0 )
	{
		String sPath = getEngine()->getContext()->sIlwDir() + "System";
		File FormatDef(sPath + "\\GDBFormatInfo.def");

		while (!FormatDef.fEof() )	
		{
			String sLine;
			FormatDef.ReadLnAscii(sLine);	
			if ( sLine[0] !=';')
			{
				Array<String> arParts;
				Split(sLine, arParts, ",");
				if ( arParts.size() > 6  && arParts[3].iVal() != -1 )
				{
					for(int j=0; j<arParts.size(); ++j )
						arParts[j] = arParts[j].sTrimSpaces();
					FormatInfo fi(arParts[0], arParts[1], arParts[2], (FormatInfo::Type)(arParts[4].iVal()), arParts[5].fVal(), arParts[6].fVal(), arParts[7]);  
					mapFormatInfo[arParts[3].iVal()] = fi;
				}				
			}			
		}
	}
}

void GeoGatewayFormat::Init()
{
	if ( fdp[0] != NULL ) return; // already done
	
	if ( !File::fExist(fnForeign) )
		throw ErrorObject(String(SIEErrFileNotFound_S.scVal(), fnForeign.sFullPath()));

	ILWISSingleLock lock(&m_CriticalSection, TRUE);	 	
	InitGeoGateway();
	
	int iRet = GDBTestOpen(fnForeign.sFullPath().scVal());
	if ( iRet != 0 )
	{
		if (0 == trq)
			trq = new Tranquilizer();
		trq->SetDelayShow(false);
		trq->SetNoStopButton(true);
		trq->SetTitle(SIETitleGeoGateway);
		trq->setHelpItem("ilwismen\import_geogateway.htm");
		trq->fText(String(SIEMOpeningGDB_S.scVal(), fnForeign.sRelative()));
		fdp[0] = GDBOpen(fnForeign.sFullPath().scVal(), "r+");
		if ( fdp[0] == NULL) //  if not, try  again but this time only read access
			fdp[0] = GDBOpen(fnForeign.sFullPath().scVal(), "r");
		if ( !fdp[0] )
			throw ErrorObject(String(SIEErrFileNotFound_S.scVal(), fnForeign.sFullPath()));
		delete trq;
		trq = 0;
	}
	else
		throw ErrorObject(SIEErrFormatNotRecognized);
}

struct GGThreadData
{
	GGThreadData() : gg(NULL), fptr(NULL) {}
	GeoGatewayFormat *gg;
	ForeignCollection* fptr;
	Directory dir;
};

void GeoGatewayFormat::PutDataInCollection(ForeignCollectionPtr* col, ParmList&)
{
	bool *fDoNotLoadGDB = (bool *)(getEngine()->pGetThreadLocalVar(IlwisWinApp::tlvDONOTLOADGDB));	
	if ( *fDoNotLoadGDB == true )	
		return;

	GGThreadData *data = new GGThreadData;
	data->gg = this;
	data->fptr = new ForeignCollection(col->fnObj, ParmList());   // pass the complete object as pointer
	                                                  // needed to keep pointer in memory when passing it to thread
	data->dir = Directory(getEngine()->sGetCurDir());

	::AfxBeginThread(PutDataInThread, (VOID *)data);
}

UINT GeoGatewayFormat::PutDataInThread(LPVOID lp)
{
	GGThreadData *data = NULL;
	FileName fnCol;
	try
	{
		data = (GGThreadData *)lp;
		fnCol = (*(data->fptr))->fnObj;
		getEngine()->InitThreadLocalVars();
		getEngine()->SetCurDir(data->dir.sFullPath());
		
		data->gg->ReadForeignFormat(data->fptr->ptr());

		getEngine()->RemoveThreadLocalVars();
		delete data->fptr;  // remove the foreign collection (because passed as pointer)
		delete data;
		data = 0;
	}
	catch(ErrorObject&)
	{
		FileName *fn = new FileName(fnCol);								
		getEngine()->RemoveThreadLocalVars();					
		AfxGetApp()->GetMainWnd()->SendMessage(ILWM_CLOSECOLLECTION, (long)fn, 0);
		GGThreadData *data = (GGThreadData *)lp;
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

void GeoGatewayFormat::ReadForeignFormat(ForeignCollectionPtr* col)
{
	try
	{
		AfxGetApp()->GetMainWnd()->SendMessage(ILW_READCATALOG, WP_STOPREADING, 0);

		collection = col;
		Init();

		ILWISSingleLock lock(&m_CriticalSection, TRUE);	 		
		int iMaxChannels = GDBChanNum( fdp[0] );
		int iBitMapLayer = GDBSegNext(fdp[0], SEG_BIT, -1);
		lock.Unlock();
		
		if ( iMaxChannels >= 1 || iBitMapLayer > 0)
		{
			CreateRasterLayer(iMaxChannels, iBitMapLayer > 0);
		}

		if ( !fCombine)
		{
			ReadLayers();	
		}
		else
			ReadAllLayers();

		if ( collection->iNrObjects() == 0 )
			throw ErrorObject(SIEErrNotRecognizedLayer);
	
		AfxGetApp()->GetMainWnd()->SendMessage(ILW_READCATALOG, WP_RESUMEREADING, 0);
		collection->Store();
		AfxGetApp()->GetMainWnd()->PostMessage(ILW_READCATALOG, 0, 0);
		String sCommand("*open %S", col->fnObj.sRelativeQuoted());
		if ( fShowCollection)
			AfxGetApp()->GetMainWnd()->SendMessage(ILWM_EXECUTE, 0, (LPARAM)sCommand.scVal());				
		delete trq;
		trq = NULL;
	}
	catch(ErrorObject& err)
	{
		err.Show();
		if ( collection != 0 )
		{
			collection->DeleteEntireCollection();			
			collection->fErase = true;
		}
		CleanUp();
		throw err;
	}
}

void GeoGatewayFormat::ReadLayers()
{
	ILWISSingleLock lock(&m_CriticalSection, TRUE);	 			
	iLayer = GDBSegNext(fdp[0], SEG_VEC, -1);
	lock.Unlock();
	while ( iLayer != iGDB_NOT_FOUND )
	{
		vector<LayerInfo> li;
		try
		{
			lock.Lock();			
			GDBLayer psLayer = GDBGetLayer( fdp[0], iLayer);
			lock.Unlock();
			if ( psLayer != NULL )
			{

				lock.Lock();				
				GDBLayerType type = GDBIsTopoLayer(psLayer);
				lock.Unlock();
				if ( type != GDBLUnknown)			
				{
					 if ( type == GDBLTopoArea || type == GDBLWholePolygon )
					 {
						 IterateLayer(li);
						 		 
						 CreateLayer(li);
					 }
				}
				else
				{
						IterateLayer(li);
						CreateLayer(li);
				}
			}
		}			
		catch(ReadingLayerError& err)
		{
			err.Show();
		}	

		lock.Lock();	
		iLayer = GDBSegNext(fdp[0], SEG_VEC, iLayer);
		lock.Unlock();
	}
}

void GeoGatewayFormat::ReadAllLayers(	)
{
	if ( fCombinePossible())
	{
		vector<LayerInfo> li(6);
		IterateLayer(li);
		CreateLayer(li);
	}
	else
		throw ErrorObject(SIEErrCombineImpossible);
}

bool GeoGatewayFormat::fCombinePossible()
{
	ILWISSingleLock lock = ILWISSingleLock(&m_CriticalSection, TRUE);	 				
	iLayer = GDBSegNext(fdp[0], SEG_VEC, -1);
	if ( iLayer != iGDB_NOT_FOUND ) // needs not to be an error, could only contain rasters
	{
		vector<GDBFieldType> attrDoms;
		bool fFirst = true;
		while ( iLayer != iGDB_NOT_FOUND )
		{
			GDBLayer psLayer = GDBGetLayer( fdp[0], iLayer);
			try
			{
				if ( psLayer == NULL)
					throw ReadingLayerError(iLayer);
				if ( !fFirst && GDBGetNumFields(psLayer) != attrDoms.size() )
					return false;

				for (int i = 0; i < GDBGetNumFields(psLayer); i++)
				{
					GDBFieldType eFieldType = GDBGetFieldType(psLayer, i);
					if ( fFirst )
						attrDoms.push_back(eFieldType);
					else
						if ( attrDoms[i] != eFieldType )
							return false;
				}
				fFirst = false;
			}
			catch(ReadingLayerError& err)
			{
				err.Show();
			}	

			iLayer = GDBSegNext(fdp[0], SEG_VEC, iLayer);
		}
	}
	return true;
}

// creates a vector map for each layer described in the layerinfo structs
void GeoGatewayFormat::CreateLayer(vector<LayerInfo> &li)
{

	BaseMap bmp;
	bool fUseAs = !fImport;
	String sMethod = fUseAs ? ": use as" : ": import ";
	if ( fCombine)
		sMethod += sMethod == "" ? " : combine" : ", combine";
  if ( li.size() == 0 )
		return;
	
	if ( li[ForeignFormat::mtPointMap].iShapes > 0)
	{
		FileName fnPointMap(FileName::fnUnique(fnBaseOutputName), ".mpp");
		PointMap mp(fnPointMap, li[ForeignFormat::mtPointMap]);
		mp->SetDescription(String(SIEDscPntMapFromUsingGDB_s.scVal(), fnForeign.sRelative(), sMethod));
		mp->SetUseAs(fUseAs);
		mp->Updated();
		mp->Store();
		collection->Add(mp);
//		mp->SetReadOnly(true); // read only does not seem to work properly, to many store attempts from tables
		li[ForeignFormat::mtPointMap].tblattr->SetDescription(String(SIEDscTableFromUsingGDB_s.scVal(), fnForeign.sRelative(), sMethod));
		li[ForeignFormat::mtPointMap].tblattr->Store();
	}
	if ( li[ForeignFormat::mtSegmentMap].iShapes > 0)
	{
		FileName fnSegMap(FileName::fnUnique(FileName(fnBaseOutputName, ".mps")));
		SegmentMap mp(fnSegMap, li[ForeignFormat::mtSegmentMap]);
		mp->SetDescription(String(SIEDscSegMapFromUsingGDB_s.scVal(), fnForeign.sRelative(), sMethod));
		mp->SetUseAs(fUseAs);
		mp->Updated();		
		mp->Store();
		collection->Add(mp);
//		mp->SetReadOnly(true);
		li[ForeignFormat::mtSegmentMap].tblattr->SetDescription(String(SIEDscTableFromUsingGDB_s.scVal(), fnForeign.sRelative(), sMethod));
		li[ForeignFormat::mtSegmentMap].tblattr->Store();
	}
	if ( li[ForeignFormat::mtPolygonMap].iShapes > 0)
	{
		FileName fnPolMap(FileName::fnUnique(fnBaseOutputName), ".mpa");
		PolygonMap mp(fnPolMap, li[ForeignFormat::mtPolygonMap]);
		mp->SetDescription(String(SIEDscPolMapFromUsingGDB_s.scVal(), fnForeign.sRelative(), sMethod));
		li[ForeignFormat::mtPolygonMap].tblattr->SetDescription(String(SIEDscTableFromUsingGDB_s.scVal(), fnForeign.sRelative(), sMethod));
		li[ForeignFormat::mtPolygonMap].tblattr->Store();
		mp->SetUseAs(fUseAs);
		mp->Updated();		
		mp->TopologicalMap(false);		
		mp->Store();
		collection->Add(mp);
	//	mp->SetReadOnly(true);
	}
	for( set<String>::iterator cur=AddedFiles.begin(); cur != AddedFiles.end(); ++cur)
		collection->Add(FileName(*cur));

	collection->Updated();
}

void GeoGatewayFormat::GetRasterLayer(int iLayerIndex, Map& mp, Array<FileName>& arMaps, GeoRef& grf, 	Domain& dm,  bool fBitMap)
{
		LayerInfo li;
		String s("GeoGateway(%d)", iLayerIndex);
		if ( grf.fValid() ) // only one georef has to be created;
			li.grf = grf;
		// get csy, grf and domain info
		GetRasterInfo(li, iLayerIndex, fBitMap);
		String sPath =  fnForeign.sPath() != fnBaseOutputName.sPath() ? 
				            fnForeign.sFullPathQuoted() : fnForeign.sRelativeQuoted();
		li.sExpr = 		String("GeoGateway(%S, %d)", sPath, fBitMap ? -iLayerIndex : iLayerIndex); // bitmaps will have a negative index as they use the segment routines
		FileName fnM(fnBaseOutputName, ".mpr");
		ILWISSingleLock lock(&m_CriticalSection, TRUE);	 					
		int iMaxChannels = GDBChanNum( fdp[0] );
		lock.Unlock();
		
		FileName fnRasMap;
		if ( iMaxChannels > 1)
			fnRasMap = FileName(FileName::fnUniqueWithIndex(fnM));
		else
			fnRasMap = FileName(FileName::fnUnique(fnM));			

		li.fnObj = fnRasMap;

		bool fUseAs = !fImport;
		String sMethod = fUseAs ? ": use as" : ": import ";
		if (fUseAs)
			mp = Map(fnRasMap, this, li);
		else
			ImportRasterMap(li.fnObj, mp, li, iLayerIndex);

		// sets MinMax to undef. MinMax will be calculated when first showing
		mp->SetMinMax(RangeInt());
		mp->SetMinMax(RangeReal());
		mp->SetDescription(String(SIEDscRasMapFromUsingGDB_s.scVal(), fnForeign.sRelative(), sMethod));
		mp->Store();

		// check if the domains are equal, needed if a maplist is created
		if ( !dm.fValid() || mp->dm() == dm )
		{
			arMaps &= mp->fnObj;
			if ( !dm.fValid())
				dm = mp->dm();
		}

		collection->Add(mp);
		for( set<String>::iterator cur=AddedFiles.begin(); cur != AddedFiles.end(); ++cur)
			collection->Add(FileName(*cur));

		grf = li.grf;	
	
}
void GeoGatewayFormat::CreateRasterLayer(int iMaxChannels, bool fBitMap)
{
	GeoRef grf;
	Domain dm;
	Array<FileName> arMaps;
	Map mp;

	if ( fBitMap )
	{
		ILWISSingleLock lock(&m_CriticalSection, TRUE);	 					
		int iBitMapLayer = GDBSegNext(fdp[0], SEG_BIT, -1);
		lock.Unlock();
		while ( iBitMapLayer != iGDB_NOT_FOUND )		
		{
			GetRasterLayer(iBitMapLayer, mp, arMaps, grf, dm, fBitMap);
			ILWISSingleLock lock(&m_CriticalSection, TRUE);	 						
			iBitMapLayer = GDBSegNext(fdp[0], SEG_BIT, iBitMapLayer);		
			lock.Unlock();
		}
	}		
	for(int iChannel = 1 ; iChannel <= iMaxChannels; ++iChannel)
	{
		try
		{
			GetRasterLayer(iChannel, mp, arMaps, grf, dm, fBitMap);
		}
		catch(StopConversion&)
		{
			if (mp.fValid())  
			{
				mp->fErase = true;
				if (mp->gr().fValid())
				{
					mp->gr()->fErase = true;				
					if ( mp->gr()->cs().fValid() )
						mp->gr()->cs()->fErase = true;
				}				
			}				
			if (grf.fValid())
			{
				grf->fErase = true;				
				if ( grf->cs().fValid() )
					grf->cs()->fErase = true;
			}
			if (dm.fValid())  dm->fErase = true;
			if ( collection )
			{
				collection->DeleteEntireCollection();
				collection->fErase = true;
			}	
			return;
		}
	}
	// a maplist will be created if there are more than two raster channels and the domains are
	// compatible
	if ( dm.fValid() && arMaps.size() > 2 )
	{
		FileName fnM = FileName::fnUnique(FileName(fnBaseOutputName, ".mpl"));
		MapList mplst(fnM, arMaps);
		mplst->Store();
		collection->Add(mplst);
	}
	collection->Updated();
}

void GeoGatewayFormat::ImportRasterMap(const FileName& fnRasMap, Map& mp ,LayerInfo& li, int iChannel)
{
	if (0 == trq)
		trq = new Tranquilizer();
	trq->SetTitle(String(SIEMImportingRaster_S.scVal(), fnForeign.sShortName()));
	trq->setHelpItem("ilwismen\import_geogateway.htm");

	iLayer = iChannel;
	mp = Map(li.fnObj, li.grf, li.grf->rcSize(), li.dvrsMap);
	for ( int iLine = 0; iLine < mp->iLines(); ++iLine)
	{
		if (trq->fUpdate(iLine, mp->iLines()))
			throw StopConversion();
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
}

GDBLayer GeoGatewayFormat::OpenLayer(int iType)
{
	Init();

	ILWISSingleLock lock(&m_CriticalSection, TRUE);	 				
	if ( iType == SEG_UNKNOWN) return NULL; // general open, no interest in layer
	if ( iLayer == iUNDEF )
		throw ErrorObject(SIEErrNoLayerDefined);

	GDBLayer psLayer = GDBGetLayer( fdp[0], iLayer);

	return psLayer;
}

void GeoGatewayFormat::SetENLL(Projection& proj, const ProjInfo_t& projInfo)
{
	proj->Param(pvX0,    projInfo.FalseEasting);
	proj->Param(pvY0,    projInfo.FalseNorthing);
	proj->Param(pvLON0,  projInfo.RefLong);
	proj->Param(pvLAT0,  projInfo.RefLat);
}

void GeoGatewayFormat::SetENLLP1P2(Projection& proj, const ProjInfo_t& projInfo)
{
	GeoGatewayFormat::SetENLL(proj, projInfo);
	proj->Param(pvLAT1, projInfo.StdParallel1);
	proj->Param(pvLAT2, projInfo.StdParallel2);
}

void GeoGatewayFormat::SetENLLAS(Projection& proj, const ProjInfo_t& projInfo)
{
	GeoGatewayFormat::SetENLL(proj, projInfo);
	proj->Param(pvAZIMYAXIS, projInfo.Azimuth);
	proj->Param(pvK0, projInfo.Scale);
}

void GeoGatewayFormat::SetENL(Projection& proj, const ProjInfo_t& projInfo)
{
	proj->Param(pvX0,    projInfo.FalseEasting);
	proj->Param(pvY0,    projInfo.FalseNorthing);
	proj->Param(pvLON0,  projInfo.RefLong);
}

void GeoGatewayFormat::SetENNLS(Projection& proj, const ProjInfo_t& projInfo)
{
	GeoGatewayFormat::SetENLL(proj, projInfo);
	proj->Param(pvK0, projInfo.Scale);
}

//--
void GeoGatewayFormat::SetForeignENLL(Projection& proj, ProjInfo_t& projInfo)
{
	projInfo.FalseEasting = proj->iParam(pvX0);
	projInfo.FalseNorthing = proj->iParam(pvY0);
	projInfo.RefLong = proj->rParam(pvLON0);
	projInfo.RefLat = proj->rParam(pvLAT0);
}

void GeoGatewayFormat::SetForeignENLLP1P2(Projection& proj, ProjInfo_t& projInfo)
{
	GeoGatewayFormat::SetForeignENLL(proj, projInfo);
	projInfo.StdParallel1 = proj->rParam(pvLAT1);
	projInfo.StdParallel2 = proj->rParam(pvLAT2);
}

void GeoGatewayFormat::SetForeignENLLAS(Projection& proj, ProjInfo_t& projInfo)
{
	GeoGatewayFormat::SetForeignENLL(proj, projInfo);
	projInfo.Azimuth = proj->iParam(pvAZIMYAXIS);
	projInfo.Scale = proj->iParam(pvK0);
}

void GeoGatewayFormat::SetForeignENL(Projection& proj, ProjInfo_t& projInfo)
{
	projInfo.FalseEasting = proj->iParam(pvX0);
	projInfo.FalseNorthing = proj->iParam(pvY0);
	projInfo.RefLong = proj->iParam(pvLON0);
}

void GeoGatewayFormat::SetForeignENNLS(Projection& proj, ProjInfo_t& projInfo)
{
	GeoGatewayFormat::SetENLL(proj, projInfo);
	projInfo.Scale = proj->iParam(pvK0);
}


CoordSystem GeoGatewayFormat::GetCoordSystem()
{
//	return CoordSystem("unknown");
	
	ProjInfo_t  projInfo;
	ILWISSingleLock lock(&m_CriticalSection, TRUE);	 				
	int iRet = GDBProjectionIO( fdp[0], GDB_READ, &projInfo );

	String sGGProjString = projInfo.Units;
	String sGGProj = sGGProjString.sHead(" ");
	map<String, ProjectionConversionFunctions>::iterator where = mpCsyConvers.find(sGGProj);
	
	FileName fnCsy(FileName::fnUnique(FileName(fnBaseOutputName, ".csy")));
	if ( where != mpCsyConvers.end() )
	{
		CoordSystemViaLatLon *csv=NULL;

		if ( (*where).second.sProjectionName == "LatLon")
		{
			csv = new CoordSystemLatLon(fnCsy, 1);
		}
		else
			csv = new CoordSystemProjection(fnCsy, 1);

		csv->datum = new MolodenskyDatum("WGS 1984",""); // default datum for geogateway
		csv->ell = csv->datum->ell; // default ellipsoid for geogateway
		String sCodes = sGGProjString.sTail(" ").sTrimSpaces();
		String sEllipsDatum = sCodes.sTail(" ").sTrimSpaces();
		if ( sEllipsDatum == "") sEllipsDatum = sCodes;

		if ( (*where).second.sProjectionName != "LatLon")
		{
			CoordSystemProjection *csp = dynamic_cast<CoordSystemProjection *> (csv);
			String sIlwProj = (*where).second.sProjectionName;

			csp->prj = Projection(sIlwProj, csp->ell);
			if ( sIlwProj != "UTM" )
			{
				SetProjectionParm func = (*where).second.parmfunc;
				(func)(csp->prj, projInfo);
			}
			else
			{
				Array<String> arrsParts;
				Split(sCodes, arrsParts, " ");
				long iZone = arrsParts[0].iVal();
				if ( iZone != iUNDEF)
				{
					csp->prj->Param(pvZONE, iZone);
					if ( arrsParts.size() > 1 )
						if (arrsParts[1].size() == 1 )
							//utm gridzones of 8 deg latitu: C to M southern, N to X north hemisphere 
							csp->prj->Param(pvNORTH, (long)(arrsParts[1] > "M" ? TRUE : FALSE));
						else
							csp->prj->Param(pvNORTH, (long)TRUE);
					if ( arrsParts.size() > 2 )
					{
						SetEllipsDatum(csp, arrsParts[2]);						
					}						
				}					
			}
		}
		else
			SetEllipsDatum(csv, sEllipsDatum);			

		CoordSystem cs;
		cs.SetPointer(csv);
		for( list<CoordSystem>::iterator cur = addedCsy.begin(); cur != addedCsy.end(); ++cur)
		{
			if ( (*cur)->fEqual(*(cs->pcsViaLatLon())) )
			{
				cs->fErase = true;
				return (*cur);
			}
		}
		if ( cs->fnObj.fExist() == false)
			cs->Store();
		if ( collection)
			collection->Add(cs);
		cs->SetDescription(String(SIEDscCsyFromUsingGDB_s.scVal(), fnForeign.sRelative()));
		cs->Store();
		addedCsy.push_back(cs);
		return cs;
	}
 	return CoordSystem("unknown");
}

void GeoGatewayFormat::SetEllipsDatum(CoordSystemViaLatLon *csv, const String& sEllipsDatum)
{
		String sIlwEllipsDatum = sIlwisEllipsoid(sEllipsDatum);
		if ( sIlwEllipsDatum != "UNDEF")
		{													
			Ellipsoid ell(sIlwEllipsDatum);
			csv->ell = ell;
			if ( csv->datum )
			{
				delete csv->datum;			
				csv->datum = NULL;
			}				
		}
		else 
		{
			String sName, sArea;
			GetDatumNameArea(sEllipsDatum, sName, sArea);
			if ( sName != "UNDEF" && sName != "")
			{
				if ( csv->datum )
					delete csv->datum;

				csv->datum = new MolodenskyDatum(sName, sArea);
				csv->ell = csv->datum->ell;
			}									
		}				
	
}

void GeoGatewayFormat::GetDatumNameArea(const String& sDatumCode, String& sName, String& sArea)
{
	String sPath = getEngine()->getContext()->sIlwDir() + "System";
	File datumDef(sPath + "\\GDBDatum.def");
	while (!datumDef.fEof() )
	{
		String sLine;
		datumDef.ReadLnAscii(sLine);
		String sDatum = sLine.sHead(",").sTrimSpaces();
		if ( sDatum == sDatumCode)
		{
			String sNameArea = sLine.sTail(",");
			sName = sNameArea.sHead(",");
			sArea = sNameArea.sTail(",");
		}
	}
}

String GeoGatewayFormat::sGetForeignDatumCode(const String& sDatumName, String& sArea)
{
	String sPath = getEngine()->getContext()->sIlwDir() + "System";
	File datumDef(sPath + "\\GDBDatum.def");
	while (!datumDef.fEof() )
	{
		String sLine;
		datumDef.ReadLnAscii(sLine);
		String sDatumCode = sLine.sHead(",").sTrimSpaces();
		String sNameArea = sLine.sTail(",");		
		String sIlwisDatum = sNameArea.sHead(",");
		String sIlwisArea = sNameArea.sTail(",");
		if ( sDatumName == sIlwisDatum && sArea == sIlwisArea)
		{
			return sDatumCode;
		}
	}
}

String GeoGatewayFormat::sIlwisEllipsoid(const String& sGDBEllips)
{
	String sPath = getEngine()->getContext()->sIlwDir() + "System";
	File ellipsDef(sPath + "\\GDBEllips.def");
	while (!ellipsDef.fEof() )
	{
		String sLine;
		ellipsDef.ReadLnAscii(sLine);
		String sEll = sLine.sHead(",").sTrimSpaces();
		if ( sEll == sGDBEllips)
			return sLine.sTail(",");
	}
	return "UNDEF";
}

String GeoGatewayFormat::sForeignIlwisEllipsoidCode(const String& sEllips)
{
	String sPath = getEngine()->getContext()->sIlwDir() + "System";
	File ellipsDef(sPath + "\\GDBEllips.def");
	while (!ellipsDef.fEof() )
	{
		String sLine;
		ellipsDef.ReadLnAscii(sLine);
		String sEll = sLine.sTail(",").sTrimSpaces();
		if ( sEll == sEllips)
			return sLine.sHead(",");
	}
	return "UNDEF";
}

void GeoGatewayFormat::SetRecord(long iRec, LayerInfo& obj, mtMapType mtType, GDBShape *psShape, GDBLayer psLayer) 
{
	if ( psShape->pasVertices == 0 ) return; // check this for tables
	CoordBounds cbBounds = obj.cbMap;
	switch(mtType)
	{
		case mtPointMap:
			{
				Coord crd(psShape->pasVertices[0].x, psShape->pasVertices[0].y);
				if ( cbBounds.fContains(crd) )
				{
					columns[mtType][0]->PutVal(iRec, crd);
					columns[mtType][1]->PutRaw(iRec, iRec);
					obj.cbMap += crd;
				}
			}
			break;
		case mtSegmentMap:
			{
				CoordBounds cb;
				CoordBuf cBuf(psShape->nCount) ;
				for ( int i=0; i < psShape->nCount; ++i)
				{
					Coord crd(psShape->pasVertices[i].x, psShape->pasVertices[i].y);
				//	if ( cbBounds.fContains(crd) )
					{
						cBuf[i] = crd;
						cb += crd;
						obj.cbMap += crd;
						obj.cbActual += crd;
					}
				}
				
				columns[mtType][0]->PutVal(iRec, cb.cMin);
				columns[mtType][1]->PutVal(iRec, cb.cMax);
				columns[mtType][2]->PutVal(iRec, cBuf, cBuf.iSize());
				columns[mtType][3]->PutVal(iRec, (long)false);
				columns[mtType][4]->PutRaw(iRec, iRec);
			}
			break;
		case mtPolygonMap:
			{
				ILWISSingleLock lock(&m_CriticalSection, TRUE);	 							
				int iNParts = GDBGetRingCount(psLayer, psShape, NULL);
				lock.Unlock();
				bool fNewRing = false;
				int iTop= 0;
				for ( int iRing=0; iRing < iNParts; ++iRing)
				{
				  lock.Lock();	 								
					GDBShape *psRing = GDBGetRingRef(psLayer, psShape, iRing, NULL);
					lock.Unlock();
					CoordBounds cb, cbTop;
					CoordBuf cBuf(psRing->nCount);
					cBuf[0] = Coord(psRing->pasVertices[0].x, psRing->pasVertices[0].y);
					if ( !cbBounds.fContains(cBuf[0]))
						return;
					int iPnt = 0;

					for ( int i=1; i < psRing->nCount; ++i)
					{
						cBuf[++iPnt] = Coord(psRing->pasVertices[i].x, psRing->pasVertices[i].y);
						cbTop += cBuf[iPnt];
						cb += cBuf[iPnt];
						obj.cbMap += cBuf[iPnt];

						if ( cBuf[0] == cBuf[iPnt] )
						{
							AddTopology(obj, cBuf, cbTop, iPnt, iRec, ++iTop, fNewRing);
							if ( i < psShape->nCount - 1)
							{
								cBuf[0] = Coord(psRing->pasVertices[i+1].x, psRing->pasVertices[i+1].y);
								iPnt = 0;
								// jump one coord
								++i;
							}
							// every toplogy added after this belongs to this ring, so no new ring any more
							fNewRing = false;
							cbTop = CoordBounds();
						}
						//non closed rings
						if ( cBuf[0] != cBuf[iPnt] && i == psRing->nCount - 1)
						{
							CoordBuf buf2(psRing->nCount + 1);
							for(int j = 0; j < psRing->nCount; ++j )
								buf2[j] = cBuf[j];
							buf2[psRing->nCount] = cBuf[0];
							AddTopology(obj, buf2, cbTop, iPnt + 1, iRec, ++iTop, fNewRing);							
							iPnt = 0;
							++i;
						}							
					}
					// a new topology is a new ring, so add a new ring
					fNewRing = true;
				}
				break;
			}
	};
}

int GeoGatewayFormat::iCountTopologies(GDBShape *psShape)
{
	Coord crdStart;
	int iTotal=0;
	bool fStart = true;
	Coord crdCur;

	for ( int i=0; i < psShape->nCount; ++i)
	{
		crdCur = Coord(psShape->pasVertices[i].x, psShape->pasVertices[i].y);

		if ( fStart )
		{
			crdStart = crdCur;
			fStart = false;
		}
		else
		{

			if ( crdStart == crdCur )
			{
				++iTotal;
				fStart = true;
			}
		}

	}
	// single topologies in a not closed ring
	if (!fStart && crdStart != crdCur)
		return iTotal + 1;

	return iTotal;
}

void GeoGatewayFormat::AddTopology(LayerInfo& obj, CoordBuf& cBuf, const CoordBounds& cb, long iPnt, long iRec, long iTop, bool fNewRing)
{
		PolArea pa;
		pa.Buf(cBuf, iPnt+1);
		if ( mtLoadType == mtUnknown) // true at create time
			iTopRecord = obj.tblTop->iRecNew();
		else
			++iTopRecord;

		double rArea = pa.rArea();
		if ( (iTop == 1 && rArea < 0) || fNewRing) 
		{
			cBuf.Reverse(0, iPnt+1);
			rArea = -rArea;
		}else if ( rArea > 0 )
		{
			cBuf.Reverse(0, iPnt+1);
			rArea = -rArea;
		}

		double rNewArea = columns[mtPolygonMap][2]->rValue(iRec);
		double rNewLength = columns[mtPolygonMap][3]->rValue(iRec);
		if ( rNewArea == rUNDEF) rNewArea = 0.0;
		if ( rNewLength = rUNDEF ) rNewLength = 0.0;
		rNewLength += pa.rLength();
		if ( iTop == 1 || fNewRing )
			rNewArea += abs(rArea);
		else 
			rNewArea -= abs(rArea);
    if ( iTop == 1)
		{
			columns[mtPolygonMap][4]->PutVal(iRec, iTopRecord);
			columns[mtPolygonMap][5]->PutRaw(iRec, iRec);
			columns[mtPolygonMap][6]->PutVal(iRec, (long)false);
		}

		columns[mtPolygonMap][2]->PutVal(iRec, rNewArea ); // correct for islands
		columns[mtPolygonMap][3]->PutVal(iRec, rNewLength); 

		CoordBounds cbNew(columns[mtPolygonMap][0]->cValue(iRec), columns[mtPolygonMap][1]->cValue(iRec));
		cbNew += cb;
		columns[mtPolygonMap][0]->PutVal(iRec, cbNew.cMin);
		columns[mtPolygonMap][1]->PutVal(iRec, cbNew.cMax);

		columns[mtTopology][0]->PutVal(iTopRecord, cb.cMin);
		columns[mtTopology][1]->PutVal(iTopRecord, cb.cMax);
		columns[mtTopology][2]->PutVal(iTopRecord, cBuf, iPnt + 1);
		columns[mtTopology][3]->PutVal(iTopRecord, (long)false);
		columns[mtTopology][4]->PutVal(iTopRecord, iRec);
		columns[mtTopology][5]->PutVal(iTopRecord, iUNDEF);
		if ( iTop == 1 ) // first
		{
			columns[mtTopology][6]->PutVal(iTopRecord, iTopRecord); 
//			columns[mtTopology][7]->PutVal(iTopRecord, -iTopRecord);
		}
		else
		{
			columns[mtTopology][6]->PutVal(iTopRecord - 1, iTopRecord);
			columns[mtTopology][6]->PutVal(iTopRecord, columns[mtPolygonMap][4]->iValue(iRec));
//			if ( iTop > 2)
//				columns[mtTopology][7]->PutVal(iTopRecord - 1, -iTopRecord);
//			columns[mtTopology][7]->PutVal(iTopRecord, -iTopRecord );
		}
}

void GeoGatewayFormat::SetColumns(LayerInfo& obj, mtMapType mpType)
{
	Domain dmcrd;
	Domain dmcrdbuf("CoordBuf");
	dmcrd.SetPointer(new DomainCoord(obj.csy->fnObj));
	DomainValueRangeStruct dvReal(-1e100, 1e100, 0.001);
	DomainValueRangeStruct dvInt(-10000000 , 10000000);

	switch(mpType)
	{
		case mtPointMap:
			{
				columns[mtPointMap][0] = obj.tbl->colNew("Coordinate", dmcrd, ValueRange());
				columns[mtPointMap][1] = obj.tbl->colNew("Name", obj.dvrsMap);
			}
			break;
		case mtSegmentMap:
			{
				columns[mtSegmentMap][0] = obj.tbl->colNew("MinCoords", dmcrd, ValueRange());
				columns[mtSegmentMap][1] = obj.tbl->colNew("MaxCoords", dmcrd, ValueRange());
				columns[mtSegmentMap][2] = obj.tbl->colNew("Coords", dmcrdbuf);
				columns[mtSegmentMap][3] = obj.tbl->colNew("Deleted", Domain("bool"));
				columns[mtSegmentMap][4] = obj.tbl->colNew("SegmentValue", obj.dvrsMap);
			}
			break;
		case mtPolygonMap:
			{
				Domain dmcrd;
				dmcrd.SetPointer(new DomainCoord(obj.csy->fnObj));

				columns[mtPolygonMap][0] = obj.tbl->colNew("MinCoords", dmcrd, ValueRange());
				columns[mtPolygonMap][1] = obj.tbl->colNew("MaxCoords", dmcrd, ValueRange());
				columns[mtPolygonMap][2] = obj.tbl->colNew("Area", dvReal);
				columns[mtPolygonMap][3] = obj.tbl->colNew("Length", dvReal);
				columns[mtPolygonMap][4] = obj.tbl->colNew("TopStart", dvInt);
				columns[mtPolygonMap][5] = obj.tbl->colNew("PolygonValue", obj.dvrsMap);
				columns[mtPolygonMap][6] = obj.tbl->colNew("Deleted", Domain("bool"));

				columns[mtTopology][0] = obj.tblTop->colNew("MinCoords", dmcrd, ValueRange());
				columns[mtTopology][1] = obj.tblTop->colNew("MaxCoords", dmcrd, ValueRange());
				columns[mtTopology][2] = obj.tblTop->colNew("Coords", dmcrdbuf);
				columns[mtTopology][3] = obj.tblTop->colNew("Deleted", Domain("bool"));
				columns[mtTopology][4] = obj.tblTop->colNew("LeftPolygon",  dvInt);
				columns[mtTopology][5] = obj.tblTop->colNew("RightPolygon", dvInt);
				columns[mtTopology][6] = obj.tblTop->colNew("ForwardLink", dvInt);
				columns[mtTopology][7] = obj.tblTop->colNew("BackwardLink", dvInt);

			}
	};
}

void GeoGatewayFormat::CreateTables(LayerInfo& obj, mtMapType mtType)
{	
	if ( mtType == mtTable ) return; // no tables yet
	String sExt;
	switch ( mtType )
	{
		case mtPolygonMap:
			sExt = ".mpap#"; break;
		case mtSegmentMap:
			sExt = ".mps#"; break;
		case mtPointMap:
			sExt = ".pt#"; break;
	}		

	FileName fnDataOut(obj.fnObj, sExt);

	bool fUseAs = !fImport;
	if (fUseAs)
		obj.tbl.SetPointer(new TablePtr(obj.fnObj, Domain("none"), obj.fnForeign, obj.sExpr, ForeignFormat::mtTable));
	else
		obj.tbl.SetPointer(new TablePtr(obj.fnObj, fnDataOut, Domain("none"), ""));

	obj.tbl->iRecNew(obj.iShapes);
	obj.tbl->fUpToDate();
	obj.tbl->SetUseAs(fUseAs);
	if ( mtType == mtPolygonMap )
	{
		if (fUseAs)
			obj.tblTop.SetPointer(new TablePtr(obj.fnObj, Domain("none"), obj.fnForeign, obj.sExpr, ForeignFormat::mtTable, "top:"));
		else
			obj.tblTop.SetPointer(new TablePtr(obj.fnObj, FileName(fnDataOut, ".mpat#"), Domain("none"), "top:"));
		obj.tblTop->iRecNew(obj.iTopologies);
	}

	SetColumns(obj,mtType);
	obj.tbl->Store();
	// name of aatribute tables will be extended with a type string
	String sFile("%S_%S.tbt", obj.fnObj.sFullPath(false), sType(mtType)); 
	FileName fnAtr(sFile);
	AddNewFiles(fnAtr);

	if (fUseAs)
	{
		obj.tblattr.SetPointer(new TablePtr(fnAtr, obj.dvrsMap.dm(), obj.fnForeign, obj.sExpr, mtType)); 
		obj.tblattr->SetUseAs(true);
	}		
	else
		obj.tblattr.SetPointer(new TablePtr(fnAtr, FileName(fnAtr, ".tb#"), obj.dvrsMap.dm(), ""));

	if (0 == trq)
		trq = new Tranquilizer();
	trq->SetText(String(SIETitleAddingToColumns_S.scVal(), obj.tblattr->fnObj.sFile));	
	for (int i = 0; i < attrDomains.size(); i++)
	{
		if ( trq->fUpdate(i, attrDomains.size()))
			throw StopConversion();
		
    if ( attrDomains[i].dvrs.dm()->pdnone() ) continue; // columns with values that cann't be mapped

		DomainValueRangeStruct dvs = attrDomains[i].dvrs;
		obj.tblattr->colNew(attrDomains[i].sColName, dvs);
	}
	obj.tblattr->colNew(sZCOLUMNNAME, DomainValueRangeStruct(-1e100, 1e100, 0.01));
	trq->fUpdate(attrDomains.size(), attrDomains.size());
}

bool GeoGatewayFormat::fReUseExisting(const FileName& fn)
{
	// if it does not exist it can be used (the name)
	if ( !fn.fExist() ) return false;
	int iNr;
	ObjectInfo::ReadElement("Collection", "NrOfItems", fn, iNr);
	if ( iNr == iUNDEF ) return false;
	FileName fnForeignCollection(fnForeign, ".ioc");
	//the domain must be used already within this collection, if it is it may be reused
	for(int i=0; i<iNr; ++i)
	{
		String sItem("Item%d", i);
		FileName fnCollection;
		ObjectInfo::ReadElement("Collection", sItem.scVal(), fn, fnCollection);
		if ( fnForeignCollection == fnCollection )
			return true;
	}
	return true;	
}

void GeoGatewayFormat::BasicColumnInfo(GDBLayer psLayer)
{
	DomainValueRangeStruct dvInt(-LONG_MAX + 2, LONG_MAX -2 );
	DomainValueRangeStruct dvReal(-1e307, 1e307, 0.0);
	DomainValueRangeStruct dvSort; // must be filled later
	DomainValueRangeStruct dvNone(Domain("none"));

	ILWISSingleLock lock(&m_CriticalSection, TRUE);	 				
	for (int i = 0; i < GDBGetNumFields(psLayer); i++)
	{
		GDBFieldType eFieldType = GDBGetFieldType(psLayer, i);
		String sColName(GDBGetFieldName(psLayer, i));
		if ( sColName == "" ) sColName = String("Column%d", i);

		switch (eFieldType)
		{

			case GDBFieldTypeInteger:
				attrDomains.push_back(clmInfo(dvInt, sColName));
				break;
			case GDBFieldTypeFloat:
			case GDBFieldTypeDouble:
				attrDomains.push_back(clmInfo(dvReal, sColName));;
				break;
			case GDBFieldTypeString:
				attrDomains.push_back(clmInfo(dvSort, sColName));
				break;
			default: // can't be mapped (GDBFieldTypeCountedInt)
				attrDomains.push_back(clmInfo(dvNone, sColName));
				break;
		}
	}
}

void GeoGatewayFormat::ScanValues(GDBLayer psLayer, vector<LayerInfo>& objects, vector < set<String> > &values)
{
	if (0 == trq)
		trq = new Tranquilizer();
	trq->SetTitle(SIEMScanningValues);
	trq->setHelpItem("ilwismen\import_geogateway.htm");
	int iTotalShapes = GDBGetNumShapes(psLayer);
	ILWISSingleLock lock(&m_CriticalSection, TRUE);	 				
	bool fPolygon = GDBIsTopoLayer(psLayer) != GDBLUnknown; // || psLayer->iRingStartField != -1;
  int iShape = GDBGetFirst(psLayer);
	lock.Unlock();
	
	trq->SetText(SIEMScanningValues);	
	int iCur = 0;
	bool fDisplayError = false;
	int iTopoError;
	
	while (iShape != GDBNullShapeId)
	{
		GDBShape *psShape;
		if (trq->fUpdate(iCur++, iTotalShapes))
			throw StopConversion();
		lock.Lock();	 					
		if ( fPolygon )
			psShape = GDBGetWholePolygon(psLayer, iShape, NULL, fDisplayError, &iTopoError);
		else
			psShape = GDBGetShapeRef(psLayer, iShape);

		if (psShape && psShape->pasVertices)
		{
  		mtMapType mtBelongsIn = fPolygon ? mtPolygonMap : mtGetShapeType(psShape);
			objects[mtBelongsIn].iShapes++;
			if (fPolygon)
			{
				int iTops = iCountTopologies(psShape);
				objects[mtBelongsIn].iTopologies += iTops;
			}
			for (int i = 0; i < GDBGetNumFields(psLayer); i++)
			{
				if (attrDomains[i].dvrs.dm().fValid())	
					continue; // already defined
				String sValue(psShape->pasFields[i].pszString);
				if (sValue != "" )
					values[i].insert(sValue);
			}
		}
		iShape = GDBGetNext(psLayer, iShape);
  	lock.Unlock();		
	}
	if (trq->fUpdate(iTotalShapes, iTotalShapes))
		throw StopConversion();	
}

// creates attrib column domains, it may reuse existing domains if they are generated by the same
// external file
void GeoGatewayFormat::CreateDomains(vector < set<String> > &values)
{
	for (int i = 0; i < attrDomains.size(); i++)
	{
		//all value columns already have domains, these need no new domain
		if ( attrDomains[i].dvrs.dm().fValid() )	continue;

		FileName fnBaseName = FileName(attrDomains[i].sColName, ".dom");
		fnBaseName.Dir(fnBaseOutputName.sPath());
		// see if there is an earlier made domain, use this instead of creating a new one.
		bool fUseExisting = fReUseExisting(fnBaseName);

		if ( !fUseExisting )
		{
			// if empty columns are added make them domain string (do not remove them, they are
			// present in the data set)
			FileName fnDomain(FileName::fnUnique(fnBaseName));
			//fnDomain.Dir(fnBaseOutputName.sPath());
			DomainType dmt = values[i].size() > 50 ? dmtID : dmtCLASS;
			if ( attrDomains.size() > 0 )
			{
				if ( values[i].size() > 0 )
				{
					attrDomains[i].dvrs = Domain(fnDomain, 0, dmt);
					attrDomains[i].dvrs.dm()->SetDescription(String(SIEDscDomainFromUsingGDB_s.scVal(), fnForeign.sRelative()));
				}
				else
					attrDomains[i].dvrs = Domain("String");
			}
			else
			{
				// will only happen for domain == counted type, not supported by ilwis
				attrDomains[i].dvrs = Domain("none");
				continue;
			}
			if ( values[i].size() > 0 )
			{	
				// if a domain is created, add it to the object collection
//				AddedFiles.insert(fnDomain.sFullPath().scVal());
				AddNewFiles(fnDomain);
				if ( dmt == dmtCLASS)
//					AddedFiles.insert(FileName(fnDomain, ".rpr").sFullPath().scVal());
						AddNewFiles(FileName(fnDomain, ".rpr"));
			}
		}
		else
			attrDomains[i].dvrs = Domain(fnBaseName);
	}
}

void GeoGatewayFormat::AddDomainValues(vector <set<String> > &values)
{
	for (int i = 0; i < attrDomains.size(); i++)
	{
		// if empty columns are added make them domain string (do not remove them, they are
		// present in the data set)
		if ( attrDomains[i].dvrs.dm()->pdsrt())
		{
			if ( attrDomains[i].dvrs.dm()->pdsrt()->iSize() == 0 )
			{
			// new domain sort, use the efficient AddValues function to add. Sadly enough MFC sets can not be
			// passed over DLL boundaries.
				vector<String> vals(values[i].size());
				copy(values[i].begin(), values[i].end(), vals.begin());
				attrDomains[i].dvrs.dm()->pdsrt()->AddValues( vals );
			}
			else
			{
				//Add values the oldfashioned way to an existing domain.
				set<String> &vals = values[i];
				DomainSort::DomainSortingType dsType = attrDomains[i].dvrs.dm()->pdsrt()->dsType;
				attrDomains[i].dvrs.dm()->pdsrt()->dsType = DomainSort::dsMANUAL;
				if (0 == trq)
					trq = new Tranquilizer();
				trq->SetText(SIETitleAddingToDomain);
				int iNr=0, iSize = vals.size();
				for(set<String>::iterator cur = vals.begin(); cur != vals.end(); ++cur)
				{
					if ( trq->fUpdate(iNr++, iSize))
						throw StopConversion();
					try
					{ 
						attrDomains[i].dvrs.dm()->pdsrt()->iAdd(*cur);
					}
					catch(ErrorNotUnique&)
					{
					}
				}
				if ( trq->fUpdate(iSize, iSize))				
				attrDomains[i].dvrs.dm()->pdsrt()->dsType = dsType;				
			}
		}
	}
}

// gather info about the layer and create attrib column domains. The function will fill
// a layerinfo struct with relevant information
void GeoGatewayFormat::ScanLayer(GDBLayer psLayer, vector<LayerInfo>& objects )
{
	try
	{
		BasicColumnInfo(psLayer);

		vector < set<String> > values(attrDomains.size());
		ScanValues(psLayer, objects, values);
		CreateDomains(values);
		AddDomainValues(values);
		GDBVertex psMin, psMax;
		ILWISSingleLock lock(&m_CriticalSection, TRUE);	 							
		GDBCalcExtents(psLayer, &psMin, &psMax);
		CoordBounds cb(Coord(psMin.x, psMin.y), Coord(psMax.x, psMax.y));
		for(int i=0; i<ForeignFormat::mtUnknown; i++)
		{
			mtMapType mtType = (mtMapType)i;
			if ( objects[i].iShapes > 0 )
				objects[i].cbMap = cb;
		}
	}
	catch(StopConversion& )
	{
	}
}

ForeignFormat::mtMapType GeoGatewayFormat::mtGetShapeType(GDBShape *psShape)
{
	if ( psShape->nCount == 1)
		return ForeignFormat::mtPointMap;
	else if (psShape->nCount > 1)
		return ForeignFormat::mtSegmentMap;
//	else if ( psShape->nCount == 0 )
//		return ForeignFormat::mtTable;

	return ForeignFormat::mtUnknown;
}

// recreates the internal tables from the info in the odf. Columns are stored seperately for efficiency
void GeoGatewayFormat::SetTable(	LayerInfo& li, ForeignFormat::mtMapType mtType)
{
	li.tbl.SetPointer(new TablePtr(li.fnObj, "",false));
	li.tbl->Load();
	// set loaded on true as GDB tables are not loaded from any ilwis file
	li.tbl->Loaded(true);
	// use an array of columns for more efficient access to the data columns of the tables
	switch(mtType)
	{
		case mtPointMap:
			{
				columns[mtPointMap][0] = li.tbl->col("Coordinate");
				columns[mtPointMap][1] = li.tbl->col("Name");
			}
		break;
		case mtSegmentMap:
			{
				columns[mtSegmentMap][0] = li.tbl->col("MinCoords");
				columns[mtSegmentMap][1] = li.tbl->col("MaxCoords");
				columns[mtSegmentMap][2] = li.tbl->col("Coords");
				columns[mtSegmentMap][3] = li.tbl->col("Deleted");
				columns[mtSegmentMap][4] = li.tbl->col("SegmentValue");
			}
		break;
		case mtPolygonMap:
			{
				li.tblTop.SetPointer(new TablePtr(li.fnObj, "top:",false));
				li.tblTop->Load();
				li.tblTop->Loaded(true);
				Domain dmcrd;
				dmcrd.SetPointer(new DomainCoord(li.csy->fnObj));

				columns[mtPolygonMap][0] = li.tbl->col("MinCoords");
				columns[mtPolygonMap][1] = li.tbl->col("MaxCoords");
				columns[mtPolygonMap][2] = li.tbl->col("Area");
				columns[mtPolygonMap][3] = li.tbl->col("Length");
				columns[mtPolygonMap][4] = li.tbl->col("TopStart");
				columns[mtPolygonMap][5] = li.tbl->col("PolygonValue");
				columns[mtPolygonMap][6] = li.tbl->col("Deleted");

				columns[mtTopology][0] = li.tblTop->col("MinCoords");
				columns[mtTopology][1] = li.tblTop->col("MaxCoords");
				columns[mtTopology][2] = li.tblTop->col("Coords");
				columns[mtTopology][3] = li.tblTop->col("Deleted");
				columns[mtTopology][4] = li.tblTop->col("LeftPolygon");
				columns[mtTopology][5] = li.tblTop->col("RightPolygon");
				columns[mtTopology][6] = li.tblTop->col("ForwardLink");
				columns[mtTopology][7] = li.tblTop->col("BackwardLink");

			}
	};
}

void GeoGatewayFormat::LinkLayers()
{
	ILWISSingleLock	lock(&m_CriticalSection, TRUE);	 						
	int iLayer = GDBSegNext(fdp[0], SEG_VEC, -1);
	GDBLayer psNodeLayer=0, psAreaLayer=0, psArcLayer=0;
	// check if both a ArcLayer and a AreaLayer are present. If so we can link the layers. This must
	// be done to be able to access the whole polygons in the layer(s).
	while ( iLayer != -1 )
	{
		GDBLayer psLayer = GDBGetLayer(fdp[0], iLayer);
		GDBLayerType type = GDBIsTopoLayer(psLayer);
		switch(type)
		 {
			case GDBLTopoArc:
				psArcLayer = psLayer;	break;
			case GDBLTopoArea:
			case GDBLWholePolygon:	
				psAreaLayer = psLayer; break;
			case GDBLTopoNode:
				psNodeLayer = psLayer; break;

		 };
		iLayer = GDBSegNext(fdp[0], SEG_VEC, iLayer);
	}
	if ( psArcLayer && psAreaLayer )
	{
		GDBLinkTopoLayers(psArcLayer, psNodeLayer, psAreaLayer);
	}
}

// main routine for reading a vector layer. It construct all data objects (tables. csy)
void GeoGatewayFormat::IterateLayer(vector<LayerInfo>& objects, bool fCreate)
{
	bool *fDoNotLoadGDB = (bool *)(getEngine()->pGetThreadLocalVar(IlwisWinApp::tlvDONOTLOADGDB));	
	if ( *fDoNotLoadGDB == true )	
		return;
	if ( mtLoadType != mtUnknown) // checking if datafile has changed
	{
		//FileName fn(fnBaseOutputName.sShortName(false), sExt(mtLoadType));
		if (  mtLoadType >= objects.size())
			return;
		
		if (ForeignCollection::fForeignFileTimeChanged(fnForeign, objects[mtLoadType].fnObj) )
		{
			fCreate = true;
			mtLoadType = mtUnknown; // everything has to be processed
		}
		else
			fCreate = false;

	}

	Init();
  if ( fCombine ) // combine will read all layers and create a map per type
	{
		if ( fCreate )
		{
			vector < set<String> > values;
			bool fFirst=true;
			ILWISSingleLock lock(&m_CriticalSection, TRUE);	 								
			iLayer = GDBSegNext(fdp[0], SEG_VEC, -1);
			lock.Unlock();
			if (0 == trq)
				trq = new Tranquilizer();
			while ( iLayer != iGDB_NOT_FOUND )
			{
				if ( trq->fText(String(SIEMConvertingLayer_S.scVal(), iLayer)))
					break;
				lock.Lock(); 												
				GDBLayer psLayer = GDBGetLayer( fdp[0], iLayer);
				lock.Unlock();
				// determine basic structure of attribute tables
				// how many columsn, which domains (sort will detailed later)
				if ( fFirst )
				{
					BasicColumnInfo(psLayer);
					values.resize(attrDomains.size());
				}
				// read the values for the sort domains and stores them in a set.
				ScanValues(psLayer, objects, values);
				fFirst = false;
				// set the coordbounds for each of the maps
				lock.Lock();	 												
				GDBVertex psMin, psMax;
				GDBCalcExtents(psLayer, &psMin, &psMax);
				CoordBounds cb(Coord(psMin.x, psMin.y), Coord(psMax.x, psMax.y));
				for(int i=0; i<ForeignFormat::mtUnknown; i++)
					objects[(mtMapType)i].cbMap = cb;

				iLayer = GDBSegNext(fdp[0], SEG_VEC, iLayer);
				lock.Unlock();
			}
			// create all sort domains
			CreateDomains(values);
			// fills the layerinfo structure(in objects) with the basic info about a layer
			// csy, attrib table, domain etc.
			AddDomainValues(values);
			CreateBasicLayer(objects);

		}
		else
			SetTable(objects[mtLoadType], mtLoadType);

		if ( mtLoadType == mtUnknown && !fImport )	return; // no load needed at initial create

		// for a true import loading the table can not be deferred until opening of the table
		// loading must be done now as the data needs to be stored.
		if ( fImport)
			StoreAttrTables(objects);

		ILWISSingleLock lock(&m_CriticalSection, TRUE);	 										
		iLayer = GDBSegNext(fdp[0], SEG_VEC, -1);
		lock.Unlock();
		int iRec=0, iPoint=1, iSeg=1, iPol=1, iTabRec=1;
    // read all data from all layers
		while ( iLayer != iGDB_NOT_FOUND )
		{
			lock.Lock(); 											
			GDBLayer psLayer = GDBGetLayer( fdp[0], iLayer);
			lock.Unlock();
			ReadData(psLayer, objects, iRec, iPoint, iSeg, iPol, iTabRec);
			lock.Lock();	 											
			iLayer = GDBSegNext(fdp[0], SEG_VEC, iLayer);
			lock.Unlock();
		}
	}
	else
	{
		ILWISSingleLock lock(&m_CriticalSection, TRUE);	 										
		GDBLayer psLayer = OpenLayer(SEG_VEC);
		if (  objects.size() < 6)
			objects.resize(6);

		bool fTopology = GDBIsTopoLayer(psLayer) != GDBLUnknown ;
		lock.Unlock();
		if ( fTopology)
			LinkLayers();

//		bool fPolygon = fTopology || pLayer->iRingStartField != -1; // no true topo but non topo polygons

		// when creating Filename, domain, csy and some other details have to be set for a vectormap. LayerInfo
		// sets the information for all the three types. It first scans the data to retrieve the necessary info.
		if ( fCreate )
		{
			attrDomains.resize(0);
			ScanLayer(psLayer, objects);
			CreateBasicLayer(objects);
			if ( fImport)
				StoreAttrTables(objects);

		}
		else
			SetTable(objects[mtLoadType], mtLoadType);


		if ( mtLoadType == mtUnknown && !fImport)	return; // no load needed at initial create

		int iRec=0, iPoint=1, iSeg=1, iPol=1, iTabRec=1;
		ReadData(psLayer, objects, iRec, iPoint, iSeg, iPol, iTabRec); 

	}
}

void GeoGatewayFormat::StoreAttrTables(vector<LayerInfo>& objects)
{
	mtMapType mtType = mtLoadType; // temporary loadtype, needs be remembered
	for(int i=0; i<ForeignFormat::mtUnknown; i++)
	{
		mtLoadType = (mtMapType)i;
		if ( objects[i].iShapes > 0 )
		{
			LoadTable((TablePtr *)(objects[i].tblattr.pointer()));
			objects[i].tblattr->Store();
		}
	}
	mtLoadType = mtType;
}

void GeoGatewayFormat::CreateBasicLayer(vector<LayerInfo>& objects)
{

	for(int i=0; i<ForeignFormat::mtUnknown; i++)
	{
		mtMapType mtType = (mtMapType)i;
		if ( objects[i].iShapes > 0 )
		{
			objects[i].fnObj = fnCreateFileName(FileName(fnBaseOutputName.sShortName(false), sExt(mtType)));
			objects[i].fnForeign = fnForeign; 
			objects[i].dvrsMap = Domain(objects[i].fnObj, objects[i].iShapes, dmtUNIQUEID, sType(mtType));
			if ( objects[i].cbMap.fContains( objects[i].cbActual) )
				objects[i].cbMap = objects[i].cbActual;
			objects[i].csy = GetCoordSystem();
			objects[i].csy->cb = objects[i].cbMap;

			String sPath = fnForeign.sDrive != fnBaseOutputName.sDrive ? fnForeign.sFullPathQuoted() : fnForeign.sRelativeQuoted(); 
			if ( fImport)
				objects[i].sExpr = "import";
			else if ( fCombine )
				objects[i].sExpr = String("GeoGateway(%S, combine)", sPath);
			else
				objects[i].sExpr = String("GeoGateway(%S,%d)", sPath, iLayer);

			CreateTables(objects[i], mtType);
		}
	}
}

FileName GeoGatewayFormat::fnCreateFileName(const FileName& fnBase)
{
	int iCount = 1;
	FileName fnNewFile = fnBase;
	bool fFound = true;
	while ( fFound )		
	{
		fFound = find(fnUsedFileNames.begin(), fnUsedFileNames.end(), fnNewFile ) != fnUsedFileNames.end();
		fFound |= fnNewFile.fExist();
		fFound |= ObjectInfo::fSystemObject(fnNewFile);
		if ( fFound )
		{
			fnNewFile.sFile = String("%S_%d", fnBase.sFile, iCount++);
		}
	}
	fnUsedFileNames.push_back(fnNewFile);
	
	return fnNewFile;
}

void GeoGatewayFormat::ReadData(GDBLayer psLayer, vector<LayerInfo>& objects, 
															 int &iRec, int &iPoint, int &iSeg, int &iPol, int &iTabRec)
{
	ILWISSingleLock lock(&m_CriticalSection, TRUE);	
  int iShape = GDBGetFirst(psLayer);
	if ( iShape == -1 ) return;
	bool fPolygon = GDBIsTopoLayer(psLayer) != GDBLUnknown; // || psLayer->iRingStartField != -1;

	int  iTotalShapes = GDBGetNumShapes(psLayer);

	CWaitCursor curs;
	bool fDisplayError = false;
	int iTopoError;	
	lock.Unlock();

  while (iShape != GDBNullShapeId)
  {

		GDBShape *psShape;
		lock.Lock();		
		if ( fPolygon )
			psShape = GDBGetWholePolygon(psLayer, iShape, NULL, fDisplayError, &iTopoError);
		else
		  psShape = GDBGetShapeRef(psLayer, iShape);

		if ( psShape == NULL ) break; // should not happen

		mtMapType mtBelongsIn = fPolygon ? mtPolygonMap : mtGetShapeType(psShape);

		switch(mtBelongsIn)
		{
			case mtPointMap:
				iRec = iPoint++;
				break;
			case mtSegmentMap:
				iRec = iSeg++;
				break;
			case mtTable:
				iRec = iTabRec++;
			case mtPolygonMap:
				iRec = iPol++;
		}
		lock.Unlock();
		// only records will be set of a requested maptype. If a pointmap is opened only the pointmap data is read
		// even if there is segmentdata in the samelayer. 
		if ( (mtBelongsIn == mtLoadType) || mtLoadType == mtUnknown)
		{
			SetRecord(iRec, objects[mtBelongsIn], mtBelongsIn, psShape, psLayer);
		}
		lock.Lock();		
		iShape = GDBGetNext(psLayer, iShape);
		lock.Unlock();
	}
}

void GeoGatewayFormat::LoadTable(TablePtr* tbl)
{
	bool *fDoNotLoadGDB = (bool *)(getEngine()->pGetThreadLocalVar(IlwisWinApp::tlvDONOTLOADGDB));	
	if ( *fDoNotLoadGDB == true )	
		return;	
	if ( mtLoadType == mtUnknown) return;
	Init();

	vector<Column> columns(tbl->iCols());
	tbl->Loaded(true);
	for (int iCol = 0; iCol < tbl->iCols(); iCol++)
		columns[iCol] = tbl->col(iCol);

	int iPoint=1;
	ILWISSingleLock lock(&m_CriticalSection, TRUE);		
	if ( fCombine )
	{

			iLayer = GDBSegNext(fdp[0], SEG_VEC, -1);
			while ( iLayer != iGDB_NOT_FOUND )
			{
				GDBLayer psLayer = GDBGetLayer( fdp[0], iLayer);

				LoadTableFromLayer(psLayer, columns, iPoint);
				iLayer = GDBSegNext(fdp[0], SEG_VEC, iLayer);
			}
	}
	else
	{
		GDBLayer psLayer = OpenLayer(SEG_VEC);
		if ( psLayer )
			LoadTableFromLayer(psLayer, columns, iPoint);
	}		

}

void GeoGatewayFormat::LoadTableFromLayer(GDBLayer psLayer, vector<Column> &columns, int &iPoint)
{
	bool fTopology = GDBIsTopoLayer(psLayer) != GDBLUnknown;
	if ( fTopology)
		LinkLayers();

	bool fPolygon = fTopology; // || psLayer->iRingStartField != -1;

  int iShape = GDBGetFirst(psLayer);
	int iRec=0,  iTopoError =0;
	int  iTotalShapes = GDBGetNumShapes(psLayer);
	bool fDisplayError = false;
	Column colZ;
	for (int iC=0; iC < columns.size(); ++iC)
	{
		if ( columns[iC].fValid() && columns[iC]->sName() == sZCOLUMNNAME )	
		{
			colZ = columns[iC];
		}
	}		
	double rMaxColZ=rUNDEF;
  while (iShape != GDBNullShapeId)
  {
		GDBShape *psShape;
		if ( fPolygon )
			psShape = GDBGetWholePolygon(psLayer, iShape, NULL, fDisplayError, &iTopoError);
		else
			psShape = GDBGetShapeRef(psLayer, iShape);

		if ( mtGetShapeType(psShape) == mtLoadType || ( fPolygon && mtLoadType == mtPolygonMap ))
		{
			int iCol=0;
			if ( colZ.fValid() )
			{
				rMaxColZ = max(rMaxColZ, psShape->pasVertices[0].z);
				colZ->PutVal(iPoint, psShape->pasVertices[0].z);			
			}				
			for (int i = 0; i < GDBGetNumFields(psLayer); i++)
			{
				GDBFieldType eFieldType = GDBGetFieldType(psLayer, i);
				switch (eFieldType)
				{
					case GDBFieldTypeInteger:
						{
						columns[iCol++]->PutVal(iPoint, (long)psShape->pasFields[i].nInteger);
						String iv = columns[iCol-1]->sValue(iPoint);
						}	
						break;
					case GDBFieldTypeFloat:
						columns[iCol++]->PutVal(iPoint, psShape->pasFields[i].fFloat);
						break;
					case GDBFieldTypeDouble :
						columns[iCol++]->PutVal(iPoint, psShape->pasFields[i].dDouble);
						break;
					case GDBFieldTypeString:
						columns[iCol++]->PutVal(iPoint, psShape->pasFields[i].pszString);
						break;
				}
			}
		}
		iShape = GDBGetNext(psLayer, iShape);
		++iPoint;
	}
	if ( rMaxColZ < 100000 || rMaxColZ > 100000 )
		colZ->Fill();
}

String GeoGatewayFormat::sType(ForeignFormat::mtMapType mtType)
{
	if ( mtType == mtPointMap) return "pnt";
	else if (mtType == mtSegmentMap) return "seg";
	else if (mtType == mtPolygonMap) return "pol";
	else if (mtType == mtTable) return "tab";
}

String GeoGatewayFormat::sExt(ForeignFormat::mtMapType mtType)
{
	if ( mtType == mtPointMap) return ".mpp";
	else if (mtType == mtSegmentMap) return ".mps";
	else if (mtType == mtPolygonMap) return ".mpa";
	else if (mtType == mtTable) return ".tbt";
}

//-- rasters
void GeoGatewayFormat::GetRasterInfo(LayerInfo& inf, int iChannel, bool fBitMap)
{
	if (!fdp[0] )
		OpenLayer();

	GetGeoRef(inf.grf);
	if ( fBitMap)
		inf.dvrsMap = DomainValueRangeStruct(0,255);
	else
	{
		ILWISSingleLock lock(&m_CriticalSection, TRUE);			
	  switch ( GDBChanType (fdp[0], iChannel ))
		{
			case CHN_8U:
				inf.dvrsMap = DomainValueRangeStruct(Domain("image"));
				break;
			case CHN_16S:
				inf.dvrsMap = DomainValueRangeStruct(-SHRT_MAX + 2, SHRT_MAX -2 );
				break;
			case CHN_16U:
				inf.dvrsMap = DomainValueRangeStruct(0, LONG_MAX -2 );
				break;
			case CHN_32R:
				inf.dvrsMap = DomainValueRangeStruct(-1e100, 1e100, 0.0);;
				break;
		}
		lock.Unlock();
	}		
}
void GeoGatewayFormat::GetGeoRef(GeoRef& grf)
{
	if ( grf.fValid() ) return; // all channels have the same georef, this one was set by previous run
	double a1, a2, b1, b2;
	ILWISSingleLock lock(&m_CriticalSection, TRUE);	
	RowCol rcSize(GDBChanYSize( fdp[0] ), GDBChanXSize( fdp[0] ));
	char geosys[17];
	if ( GDBGeorefIO(fdp[0], GDB_READ, geosys, &a1, &a2, &b1, &b2) != GEO_UNKNOWN)
	{	
		lock.Unlock();
		FileName fnG(fnBaseOutputName, ".grf");
		FileName fnGeo(FileName::fnUnique(fnG));
		Coord crdLeftup( a1 , b1);
		Coord crdRightDown(a1 + rcSize.Col * a2, b1 + rcSize.Row * b2 ) ;
		Coord cMin( min(crdLeftup.X, crdRightDown.X), min(crdLeftup.Y, crdRightDown.Y));
		Coord cMax( max(crdLeftup.X, crdRightDown.X), max(crdLeftup.Y, crdRightDown.Y));
		CoordSystem cs = GetCoordSystem();
		if ( cs->pcsLatLon() && abs(cMin.Y) > 360 && cs != CoordSystem("Unknown"))
		{
			cs->fErase = true;
			collection->Remove(cs->fnObj);
			cs = CoordSystem("Unknown");
		}			
		grf.SetPointer(new GeoRefCorners(fnGeo, cs, rcSize, true, cMin, cMax)); 
		grf->SetDescription(String(SIEDscGrfFromUsingGDB_s.scVal(), fnForeign.sRelative()));
		grf->cs()->cb = CoordBounds(cMin, cMax);
		cs->cb = grf->cs()->cb;
		cs->Updated();
		grf->cs()->Updated();
	}
	else
	{
		grf = GeoRef(rcSize);
	}
	grf->Store();
	//AddedFiles.insert(grf->fnObj.sFullPathQuoted());
	AddNewFiles(grf->fnObj);

}

#define iRAW0 0L
#define iRAW1 1L

void GeoGatewayFormat::GetBitValues(unsigned char * buffer, LongBuf& buf, long iNum) const
{
	for( int iX = 0; iX < iNum; iX++ )
	{
     if( buffer[ iX >> 3] & (0x80 >> (iX & 0x7)) )
        buf[iX] = iRAW1;
    else
        buf[iX] = iRAW0;
	}				
}
	
void GeoGatewayFormat::GetLineRaw(long iLine, ByteBuf& buf, long iFrom, long iNum) const
{
	ILWISSingleLock lock(&m_CriticalSection, TRUE);			
	if ( buffer == NULL)
		(const_cast<GeoGatewayFormat *>(this))->CreateLineBuffer(iNum);

	int iChannel = iLayer;

	if ( iChannel < 0 )
	{
		int iBitMapSegment = abs(iChannel);
		GDBBitmapIO(fdp[0], GDB_READ, iBitMapSegment, iFrom, iLine, iNum,1, buffer, iNum,1 );
	//	GetBitValues(buffer, buf, iNum);
	}			
  else
	{
		GDBByteChanIO (fdp[0], GDB_READ, iFrom,iLine, iNum,1, buffer, iNum,1, 1 ,&iChannel);
		for( int i = 0; i< iNum; ++i)
			buf[i] = buffer[i];		
	}		
	
}

void GeoGatewayFormat::GetLineRaw(long iLine, IntBuf& buf, long iFrom, long iNum) const
{
	ILWISSingleLock lock(&m_CriticalSection, TRUE);				
	if ( buffer == NULL)
		(const_cast<GeoGatewayFormat *>(this))->CreateLineBuffer(iNum);

	int iChannel = iLayer;
	GDBIntChanIO (fdp[0], GDB_READ, iFrom,iLine, iNum,1, (int *)buffer, iNum,1, 1 ,&iChannel);

	for( int i = 0; i< iNum; ++i)
		buf[i] = ((short *)buffer)[i * 2];
}

void GeoGatewayFormat::GetLineRaw(long iLine, LongBuf& buf, long iFrom, long iNum) const
{
	ILWISSingleLock lock(&m_CriticalSection, TRUE);				
	if ( buffer == NULL)
		(const_cast<GeoGatewayFormat *>(this))->CreateLineBuffer(iNum);
	else
	{
		if ( iLayer < 0) // == bitmap segment
			memset(buffer, 0, iNum/8 + 1);
		else	// all other raster		
			memset(buffer, 0, iNum);
	}		

	int iChannel = iLayer;
	if ( iChannel < 0 )
	{
		int iBitMapSegment = abs(iChannel);
		GDBBitmapIO(fdp[0], GDB_READ, iBitMapSegment, iFrom, iLine, iNum,1, buffer, iNum,1 );
		GetBitValues(buffer, buf, iNum);
		return;
		
	}		
	if ( !f4BytesInt) 
	  GDBByteChanIO (fdp[0], GDB_READ, iFrom,iLine,iNum,1, (unsigned char *)buffer, iNum,1, 1 ,&iChannel);
	else
		GDBIntChanIO (fdp[0], GDB_READ, iFrom,iLine,iNum,1, (int *)buffer, iNum,1, 1 ,&iChannel);

  int i=0;
	switch(GDBChanType(fdp[0], iLayer ))
	{
		case CHN_16S:
			for( i = 0; i< iNum; ++i)
				buf[i] = ((short *)buffer)[i * 2];
			break;
		case CHN_16U:
		{
			DomainValueRangeStruct dvrs(0, USHRT_MAX - 2);			
			for( i = 0; i< iNum; ++i)
			{
				//long iR = dvrs.iRaw((long)(((unsigned int *)buffer)[i]));
				buf[i] = (((unsigned int *)buffer)[i]); 
			}
		}			
		break;
    case CHN_8U:
			for( i = 0; i< iNum; ++i)
				buf[i] = buffer[i];
			break;
	};
}

void GeoGatewayFormat::GetLineVal(long iLine, RealBuf& buf, long iFrom, long iNum) const
{
	ILWISSingleLock lock(&m_CriticalSection, TRUE);				
	if ( buffer == NULL)
		(const_cast<GeoGatewayFormat *>(this))->CreateLineBuffer(iNum);

	int iChannel = iLayer;
	
	int iNChannels = GDBChanNum( fdp[0] );
	int iDataType = GDBChanType(fdp[0], iLayer );
  if (iDataType == CHN_32R )
		GDBRealChanIO (fdp[0], GDB_READ, iFrom,iLine, iNum,1, (float*)buffer, iNum,1, 1 ,&iChannel);
	else
  {
		if ( !f4BytesInt) 
		  GDBByteChanIO (fdp[0], GDB_READ, iFrom,iLine,iNum,1, (unsigned char *)buffer, iNum,1, 1 ,&iChannel);
		else
			GDBIntChanIO (fdp[0], GDB_READ, iFrom,iLine,iNum,1, (int *)buffer, iNum,1, 1 ,&iChannel);		
  }
	int i;
	
	switch(GDBChanType(fdp[0], iLayer ))
	{
		case CHN_16S:
			for( i = 0; i< iNum; ++i)
				buf[i] = ((short *)buffer)[i * 2];
			break;
		case CHN_16U:
		{
			DomainValueRangeStruct dvrs(0, USHRT_MAX - 2);			
			for( i = 0; i< iNum; ++i)
			{
				//long iR = dvrs.iRaw((long)(((unsigned int *)buffer)[i]));
				buf[i] = (((unsigned int *)buffer)[i]); 
			}
		}			
		break;
	  case CHN_8U:
			for( i = 0; i< iNum; ++i)
				buf[i] = buffer[i];
			break;
		case CHN_32R:
			for( i = 0; i< iNum; ++i)				
 				buf[i] = ((float *)buffer)[i];
			break;
	};
}

void GeoGatewayFormat::GetLineVal(long iLine, LongBuf& buf, long iFrom, long iNum) const
{
	ILWISSingleLock lock(&m_CriticalSection, TRUE);				
	GetLineRaw(iLine, buf, iFrom, iNum);
}

void GeoGatewayFormat::CreateLineBuffer(long iSize)
{
	OpenLayer();
	int iLineSize = GDBChanXSize( fdp[0] );			
	if ( iLayer < 0 )
	{
		buffer = new unsigned char[iLineSize / 8 + 1];
		f4BytesInt = false;		
	}
	else
	{
		int iNChannels = GDBChanNum( fdp[0] );
		int iDataType = GDBChanType(fdp[0], iLayer );
		int iDataSize =  iDataType == CHN_8U ? 1 : 4;
		buffer = new unsigned char [ iLineSize * iDataSize ];
		memset(buffer, 0, iLineSize * iDataSize);
		f4BytesInt = iDataType == CHN_16S || iDataType == CHN_16U || iDataType == CHN_32R;		
	}	
}

long GeoGatewayFormat::iRaw(RowCol rc) const
{
	ILWISSingleLock lock(&m_CriticalSection, TRUE);			
	if ( buffer == NULL )
	{
		(const_cast<GeoGatewayFormat *>(this))->CreateLineBuffer(0);
	}
	int iChannel = iLayer;
	if ( iChannel < 0 )
	{
		int iLineSize = GDBChanYSize( fdp[0] );		
		int iBitMapSegment = abs(iChannel);		
		//GDBBitmapIO(fdp[0], GDB_READ, iBitMapSegment, rc.Col ,rc.Row, 1,1, buffer, 1,1 );
		int iCol = (rc.Col / 8 ) * 8;
		int iOffSet = rc.Col % 8;
		GDBBitmapIO(fdp[0], GDB_READ, iBitMapSegment, iCol, rc.Row, 8, 1, buffer, 8,1 );				
		//GDBBitmapIO(fdp[0], GDB_READ, iBitMapSegment, 0, rc.Row, iLineSize, 1, buffer, iLineSize,1 );		
		LongBuf buf(8);
		GetBitValues(buffer, buf, 8);		
		return buf[iOffSet];
	}
	else
	{
		switch ( GDBChanType(fdp[0], iLayer ))
		{
			case CHN_8U:
					GDBByteChanIO (fdp[0], GDB_READ, rc.Col ,rc.Row, 1, 1, buffer, 1,1, 1 ,&iChannel);
					return buffer[0];
					break;
			case CHN_16S:
					GDBIntChanIO (fdp[0], GDB_READ, rc.Col ,rc.Row, 1, 1, (int *)buffer, 1,1, 1 ,&iChannel);
					return (((int *)buffer)[0]);
					break;
			case CHN_16U:
				{
					DomainValueRangeStruct dvrs(0, USHRT_MAX - 2);			
					GDBIntChanIO (fdp[0], GDB_READ, rc.Col ,rc.Row, 1, 1, (int *)buffer, 1,1, 1 ,&iChannel);
					return (long)(((int *)buffer)[0]);
				}
				break;
		};
	}

	return iUNDEF;
}

long GeoGatewayFormat::iValue(RowCol rc) const
{
	return iRaw(rc);
}

double GeoGatewayFormat::rValue(RowCol rc) const
{
	ILWISSingleLock lock(&m_CriticalSection, TRUE);					
	if ( buffer == NULL )
	{
		int iLineSize = GDBChanYSize( fdp[0] );
		(const_cast<GeoGatewayFormat *>(this))->CreateLineBuffer(iLineSize);
	}
	int iChannel = iLayer;
	GDBRealChanIO (fdp[0], GDB_READ, rc.Col ,rc.Row, 1, 1, (float *)buffer, 1,1, 1 ,&iChannel);
	double rVal =  (double)((float *)buffer)[0];
	return rVal;
}

void GeoGatewayFormat::AddNewFiles(const FileName& fnNew)
{
	if (0 == trq)
		trq = new Tranquilizer();
	trq->SetDelayShow(false);	
	trq->fText(String(SIEMAddingFile_S.scVal(), fnNew.sRelative()));	
	AddedFiles.insert(fnNew.sFullPath().scVal());	
}

void GeoGatewayFormat::PutLineVal(const FileName& fnMap, long iLine, const RealBuf& buf, long iFrom, long iNum) 
{
	ILWISSingleLock lock(&m_CriticalSection, TRUE);				
	if ( buffer == NULL)
		(const_cast<GeoGatewayFormat *>(this))->CreateLineBuffer(iNum);

	int iChannel = iLayer;
	int iDataType, iBands;

	Map mp(fnMap);

	DetermineOutputType(fnMap, iBands, iDataType);

	if ( iDataType == iUNDEF)	// goto RGB
	{
		Representation rpr = mp->dm()->rpr();		
		for(int iBand=0; iBand<3; ++iBand)
		{
			for( int i = 0; i< iNum; ++i)
			{						
				Color clr = rpr->clr(buf[i]);		
				switch(iBand)
				{
					case 0:
						buffer[i] = clr.red(); break;
					case 1:
						buffer[i] = clr.green(); break;						
					case 2:
						buffer[i] = clr.blue(); break;
				}					
			}				
			if ( mapFormatInfo[iGDBID].fSupports(FormatInfo::epBANDS))
				GDBByteChanIO (fdp[0], GDB_WRITE, iFrom, iLine,iNum,1, (unsigned char *)buffer, iNum,1, 1 ,&iBand);								
			else
				GDBByteChanIO (fdp[iBand], GDB_WRITE, iFrom, iLine,iNum,1, (unsigned char *)buffer, iNum,1, 1 ,&iChannel);							
		}		
	}
	else
	{
		for( int i = 0; i< iNum; ++i)
		{
		 ((float *)buffer)[i] = (float )(buf[i]);
		}		 
	
		GDBRealChanIO (fdp[0], GDB_WRITE, iFrom,iLine, iNum,1, (float*)buffer, iNum,1, 1 ,&iChannel);	
	}		
}

void GeoGatewayFormat::PutLineVal(const FileName& fnMap,long iLine, const LongBuf& buf, long iFrom, long iNum) 
{
	ILWISSingleLock lock(&m_CriticalSection, TRUE);				
	if ( buffer == NULL)
		(const_cast<GeoGatewayFormat *>(this))->CreateLineBuffer(iNum);

	int iDataType, iBands;
	int iChannel = iLayer;

	Map mp(fnMap);
	DetermineOutputType(fnMap, iBands, iDataType);
	int i=0;

	switch(iDataType)
	{
		case CHN_16S:
			for( i = 0; i< iNum; ++i)
				((short *)buffer)[i * 2] = (short)buf[i] ;
			break;
		case CHN_16U:
			for( i = 0; i< iNum; ++i)
				((unsigned int *)buffer)[i] = (unsigned int)(buf[i]);
			break;
    case CHN_8U:
			for( i = 0; i< iNum; ++i)
				buffer[i] = buf[i];
			break;
	};	

	if ( iDataType == iUNDEF)	// goto RGB
	{
		Representation rpr = mp->dm()->rpr();
		for(int iBand=0; iBand<3; ++iBand)
		{
			for( int i = 0; i< iNum; ++i)
			{	
				Color clr = rpr->clr(buf[i]);						
				switch(iBand)
				{
					case 0:
						buffer[i] = clr.red(); break;
					case 1:
						buffer[i] = clr.green(); break;						
					case 2:
						buffer[i] = clr.blue(); break;
				}					
			}				
			if ( mapFormatInfo[iGDBID].fSupports(FormatInfo::epBANDS))
				GDBByteChanIO (fdp[0], GDB_WRITE, iFrom, iLine,iNum,1, (unsigned char *)buffer, iNum,1, 1 ,&iBand);								
			else
				GDBByteChanIO (fdp[iBand], GDB_WRITE, iFrom, iLine,iNum,1, (unsigned char *)buffer, iNum,1, 1 ,&iChannel);							
		}		
	}
	else
	{
		if ( !f4BytesInt) 
		  GDBByteChanIO (fdp[0], GDB_WRITE, iFrom, iLine,iNum,1, (unsigned char *)buffer, iNum,1, 1 ,&iChannel);
		else
			GDBIntChanIO (fdp[0], GDB_WRITE, iFrom, iLine,iNum,1, (int *)buffer, iNum,1, 1 ,&iChannel);	
	}	
}

void GeoGatewayFormat::PutLineRaw(const FileName& fnMap, long iLine, const LongBuf& buf, long iFrom, long iNum) 
{
	ILWISSingleLock lock(&m_CriticalSection, TRUE);			
	if ( buffer == NULL)
		(const_cast<GeoGatewayFormat *>(this))->CreateLineBuffer(iNum);

	Map mp(fnMap);
	RangeInt ri = mp->dvrs().riMinMax();
	short iUnd = ri.iHi() + 1;
	int iChannel = iLayer;
	
  int i=0;
	int iDataType = GDBChanType(fdp[0], iLayer );
	switch(iDataType)
	{
		case CHN_16S:
		case CHN_16U:
		{
			DomainSort *pdsrt = mp->dm()->pdsrt();
			double rStretch = 1.0;
			for( i = 0; i< iNum; ++i)
			{
				if ( iDataType == CHN_16S)
				{
					if ( mp->dvrs().fValues())				
						((short *)buffer)[i * 2] = (short)(mp->dvrs().iValue(buf[i]));
					else
						((short *)buffer)[i * 2] = (short)( buf[i] != iUNDEF ? buf[i] : iUnd );					
				}
				else
				{
					if ( mp->dvrs().fValues())				
						((unsigned int *)buffer)[i ] = (unsigned short)(mp->dvrs().iValue(buf[i]));
					else
						((unsigned int *)buffer)[i ] = (unsigned short)(buf[i]);							
				}					
					
			}			
		}			
		break;
    case CHN_8U:
		{
			DomainSort *pdsrt = mp->dm()->pdsrt();
			double rStretch = 1.0;
			if ( pdsrt )
			{
				rStretch = 255.0 / pdsrt->iSize();
			}					
			for( i = 0; i< iNum; ++i)
			{
				if ( mp->dvrs().fValues())
					buffer[i] = mp->dvrs().iValue(buf[i]);
				else
				{
					buffer[i] = min(255, buf[i] * rStretch);
				}					
			}				
		}			
		break;
	};

	
	if ( !f4BytesInt) 
	  GDBByteChanIO (fdp[0], GDB_WRITE, iFrom, iLine,iNum,1, (unsigned char *)buffer, iNum,1, 1 ,&iChannel);
	else
		GDBIntChanIO (fdp[0], GDB_WRITE, iFrom, iLine,iNum,1, (int *)buffer, iNum,1, 1 ,&iChannel);	
}

void GeoGatewayFormat::PutLineRaw(const FileName& fnMap, long iLine, const IntBuf& buf, long iFrom, long iNum) 
{
	ILWISSingleLock lock(&m_CriticalSection, TRUE);				
	if ( buffer == NULL)
		(const_cast<GeoGatewayFormat *>(this))->CreateLineBuffer(iNum);

	Map mp(fnMap);
	for( int i = 0; i< iNum; ++i)
		((short *)buffer)[i * 2] = (short)(mp->dvrs().iValue(buf[i]));

	int iChannel = iLayer;
	GDBIntChanIO (fdp[0], GDB_WRITE, iFrom,iLine, iNum,1, (int *)buffer, iNum,1, 1 ,&iChannel);	
}

void GeoGatewayFormat::PutLineRaw(const FileName& fnMap, long iLine, const ByteBuf& buf, long iFrom, long iNum) 
{
	ILWISSingleLock lock(&m_CriticalSection, TRUE);				
	if ( buffer == NULL)
		(const_cast<GeoGatewayFormat *>(this))->CreateLineBuffer(iNum);

	int iChannel = iLayer;
	Map mp(fnMap);
	
	for( int i = 0; i< iNum; ++i)
		buffer[i] = mp->dvrs().iValue(buf[i]);	

	GDBByteChanIO (fdp[0], GDB_WRITE, iFrom,iLine, iNum,1, buffer, iNum,1, 1 ,&iChannel);

}

void GeoGatewayFormat::SetForeignGeoTransformations(const CoordSystem &cs, const GeoRef& grf)
{
	ILWISSingleLock lock(&m_CriticalSection, TRUE);				
	double a1=0.0, a2=0.0, b1=1.0, b2=1.0;
	char geosys[17];
	memset( geosys, 0, 17);

	if (grf.fValid() && !grf->fGeoRefNone())
	{
		CoordBounds cb = grf->cb();
		RowCol rcSize = grf->rcSize();
		a1 = cb.cMin.X;
		b1 = cb.cMax.Y;
		b2 = - cb.height() / rcSize.Row;
		a2 = cb.width() / rcSize.Col;
	}						
	
	for ( map<String, ProjectionConversionFunctions>::iterator cur = mpCsyConvers.begin(); cur != mpCsyConvers.end(); ++cur)
	{
		String sName = (*cur).second.sProjectionName;
		CoordSystemProjection *csp = dynamic_cast<CoordSystemProjection *> (cs.ptr());
		if ( csp)
		{
			if ( fCIStrEqual(csp->prj->sName(), sName) )
			{
				ProjInfo_t  projInfo;
				memset(&projInfo, 0, sizeof(projInfo));
				
				Projection prj = csp->prj;
				Datum* molDatum = csp->datum;  // Use shortcut
				String sUnitCode = (*cur).first;
				if ( fCIStrEqual(sName, "UTM") )
				{
					sUnitCode += String(" %d", csp->prj->iParam(pvZONE));
					sUnitCode += " " + sForeignIlwisEllipsoidCode(csp->ell.sName);					
				}					
				else if ( molDatum )
				{
					sUnitCode += " " + sGetForeignDatumCode(molDatum->sName(), molDatum->sArea);
				}
				else
				{
					Ellipsoid ell = csp->ell;
					sUnitCode += " " + sForeignIlwisEllipsoidCode(ell.sName);
				}
				strcpy(projInfo.Units, sUnitCode.scVal());				
				SetForeignProjectionParm func = (*cur).second.forparmfunc;
				if ( func)
					(func)(prj, projInfo);
				
				Geosys2ProjInfo(&projInfo, sUnitCode.sVal(), a1, b1, a2, b2);				
				int iRet = GDBProjectionIO( fdp[0], GDB_WRITE, &projInfo );
				break;
			}				
		}			
	}		
}

int GeoGatewayFormat::iFormatID(const String& sFormat) const
{
	for(map<int, FormatInfo>::const_iterator cur = mapFormatInfo.begin(); cur != mapFormatInfo.end(); ++cur)
	{
		String sF = (*cur).second.sFormatName;
		if ( fCIStrEqual( sF, sFormat) )
			return (*cur).first;
	}	
	return iUNDEF;
}

FormatInfo	GeoGatewayFormat::fiGetFormatInfo(const String sExt) const
{
	for(map<int, FormatInfo>::const_iterator cur = mapFormatInfo.begin(); cur != mapFormatInfo.end(); ++cur)
	{
		String sE = (*cur).second.sExt;
		if ( fCIStrEqual( sE, sExt) )	
		{
			return (*cur).second;
		}			
	}		

	return FormatInfo();
}

bool GeoGatewayFormat::fIsSupported(const FileName& fn, ForeignFormat::Capability cbType) const
{
	for(map<int, FormatInfo>::const_iterator cur = mapFormatInfo.begin(); cur != mapFormatInfo.end(); ++cur)
	{
		if ( (*cur).second.sExt != fn.sExt )
			continue;
		switch (cbType) 
		{
			case ForeignFormat::cbEDIT:
				return (*cur).second.fUpdate;
			case ForeignFormat::cbCREATE:
				return (*cur).second.fCreate;
			case ForeignFormat::cbIMPORT:
				return (*cur).second.fImport;
			case ForeignFormat::cbANY:
				return true;
			default:
				return true;
		}
	}		
	return false;
}

bool GeoGatewayFormat::fIsCollection(const FileName& fnForeignObject) const 
{ 
	return true; 
}

void GeoGatewayFormat::ReadParameters(const FileName& fnFO, ParmList& pm)
{
	
}

void GeoGatewayFormat::ReadExpression(const FileName& fnObj, ParmList& pm, const String& sExpr) 
{
	if ( sExpr == "")
		return;
	pm.Clear();
	Array<String> arr;
	Split(sExpr, arr, "(,)");
	pm.Add(new Parm("method", arr[0]));
	pm.Add(new Parm(0, arr[1]));
	if ( arr.size() > 2 )
	pm.Add(new Parm("layer", arr[2].sTrimSpaces()));
		
}

bool GeoGatewayFormat::fMatchType(const FileName& , const String& sType)
{
	// Templates object type name
	return sType == "ILWIS ForeignCollections";
}*/
