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

 Created on: 2007-28-9
 ***************************************************************/
// MapSoilMoistureFromAscat.cpp: implementation of the MapSMAC class.
//
/* Estimation of soil moisture from Ascat sensor
   March 2009, by Lichun Wang
*/
#include "SEBS\MapSoilMoistureFromAscat_l2.h"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Err\Ilwisapp.err"
#include "Applications\MapList\MapListApplic.h"
#include "Headers\Hs\map.hs"
//#include "ascat_l2_v1.h"
#include "ascat_l2.h"

using namespace std;

IlwisObjectPtr * createMapSoilMoistureFromAscat_l2(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)MapSoilMoistureFromAscat_l2::create(fn, (MapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new MapSoilMoistureFromAscat_l2(fn, (MapPtr &)ptr);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
MapSoilMoistureFromAscat_l2::MapSoilMoistureFromAscat_l2(const FileName& fn, MapPtr& p)
: MapFromMap(fn, p),
m_fnObject(fn)
{
	ReadElement("MapSMASCAT", "LandCoverMap", m_mpLc);
	ReadElement("MapSMASCAT", "AscatFilePath", m_ascat_path);
	fNeedFreeze = true;
	sFreezeTitle = "ASCAT Level2 Soil Moisture Products";
}


MapSoilMoistureFromAscat_l2::MapSoilMoistureFromAscat_l2(const FileName& fn, 
				MapPtr& p,
				const Map& lc_mp,
				String ascat_file
				)	
: MapFromMap(fn, p, lc_mp),
m_fnObject(fn),
m_mpLc(lc_mp),
m_ascat_path(ascat_file)
{
	DomainValueRangeStruct dv(-100,100,0.001);
	SetDomainValueRangeStruct(dv);
	if (!fnObj.fValid())
     objtime = objdep.tmNewest();
	ptr.Store(); 
	fNeedFreeze = true;
	sFreezeTitle = "ASCAT Level2 Soil Moisture Products";
}


MapSoilMoistureFromAscat_l2::~MapSoilMoistureFromAscat_l2()
{

}

const char* MapSoilMoistureFromAscat_l2::sSyntax() {
	
	//return "MapSoilMoistureFromAscat_l2(MapSand,MapClay,MapFc,MapLc,AscatFilePath)";
	return "MapSoilMoistureFromAscat_l2(MapLc,AscatFilePath)";
}

MapSoilMoistureFromAscat_l2* MapSoilMoistureFromAscat_l2::create(const FileName& fn, MapPtr& p, const String& sExpr)
{
	Array<String> as;
	int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
	if (iParms == 2 ){
		Map mpLc(as[0], fn.sPath());
		String ascat_file = as[1].sVal();
		return new MapSoilMoistureFromAscat_l2(fn, p, mpLc,ascat_file);
	}
	else
      ExpressionError(sExpr, sSyntax());
}

void MapSoilMoistureFromAscat_l2::Store()
{
	MapFromMap::Store();
	WriteElement("MapFromMap", "Type", "MapSoilMoistureFromAscat_l2");
	WriteElement("MapSMASCAT", "LandCoverMap", m_mpLc);
	WriteElement("MapSMASCAT", "AscatFilePath", m_ascat_path);
}

String MapSoilMoistureFromAscat_l2::sExpression() const
{
	return String("MapSoilMoistureFromAscat_l2(%S,%S)", 
			mp->sNameQuoted(false, fnObj.sPath()),
			m_mpLc->sNameQuoted(true),
			m_ascat_path
			);

}

bool MapSoilMoistureFromAscat_l2::fDomainChangeable() const
{
  return false;
}

bool MapSoilMoistureFromAscat_l2::fGeoRefChangeable() const
{
  return false;
}

///////////////////////////////////
// some useful functions
///////////////////////////////////


double db2lin2( double x )
{
 return pow( 10.0, 0.1*x );
}


inline void SwapLong(long &iVal)    // create intel type long
{
	iVal = ((iVal >> 24) & 0x000000ffL) |   // most significant Motorola
	       ((iVal >>  8) & 0x0000ff00L) |   //
	       ((iVal <<  8) & 0x00ff0000L) |   //
	       ((iVal << 24) & 0xff000000L);    // least significant Motorola
}
inline short Swap(short iVal) 
{
	short p1 = iVal << 8;
	short p2 = (iVal >> 8) & 0xff;
	short p3 = p1 | p2;
	return p3;
}

/*GeoRef MapSoilMoistureFromAscat_l2::grf(CoordSystem& csy, RowCol rc)
{
	GeoRef gr(rc);
	int iNrTiePoints;
	iNrTiePoints = 4;
	FileName fnGeoRef; // = fnBaseObject(".grf");
		
	// given 4 or more tiepoints
	// georeftiepoint (affine transf) needed
	bool fSubPixelPrecision = false;
	//GeoRefCTPplanar* gcp
	m_gcp = new GeoRefCTPplanar(fnGeoRef, csy, rc, fSubPixelPrecision);
			
	gr.SetPointer(m_gcp);
	gr->Updated();
	return gr;
}*/


void MapSoilMoistureFromAscat_l2::GetTiePoints(long iNrTiePoints, ModelTiePoint* amtp)
{
	long latlon[8];
	amtp[0].rCol = 0;
	amtp[0].rRow = nRasterYSize-1;
	amtp[1].rCol = nRasterXSize-1;
	amtp[1].rRow = nRasterYSize-1;
	amtp[2].rCol = 0;
	amtp[2].rRow = 0;
	amtp[3].rCol = nRasterXSize-1;
	amtp[3].rRow = 0;
	
	for (int i = 0; i < 4; i++)
	{
		amtp[i].rX   = latlon[i*2+1]*pow(10.0,-4);
		amtp[i].rY   = latlon[i*2]*pow(10.0,-4);
	}
}
FileName MapSoilMoistureFromAscat_l2::fnObjectName(const String& sExt, String band, int iBand)
{
	FileName fnLoc = m_fnObject;
	fnLoc.sExt = sExt;
	String sName = String("%S_%S%ld", fnLoc.sFile, band, iBand);
	fnLoc.sFile = sName;
	return fnLoc;
	//return FileName::fnUnique(fnLoc);
}


static int get_bit( int num)
{
 int setbit = 0;
 int i = 0;
 while (i < 8){
	 if((num & int(pow(2.0,i))) == int(pow(2.0,i))){
	 	setbit = 1; //bit is set
		break;
	 }
	i+=1;	
 }		

 return setbit; 
}
bool MapSoilMoistureFromAscat_l2::fFreezing()
{
	//SMOM//
	/* Operation time control */
	/*struct timeb start, stop;
	double elapsed_time;
	
	char *p_productPath = NULL;
    char *p_schemaPath = NULL;
    char *p_schemaName = NULL;
	char *p_schemaVersion = NULL;
    ExrwReturnCode returnCode = XRW_ERROR;
	TxrwError xrwError = INITIALIZE_TxrwError;
	const TxrwEEProduct *p_xrwEEProduct = NULL;
	ExrwProductType productType = PRODUCT_TYPE_UNDEFINED;
	TxrwHeader xrwHeader = INITIALIZE_TxrwHeader;
	TxrwDataBlock xrwDataBlock = INITIALIZE_TxrwDataBlock;
	const TxrwDataset *p_xrwDataset = NULL;
	const TxrwDatasetRecord *p_xrwDatasetRecord = NULL;
	
	int redirect = 0;
	
	TIbixUnsignedLong64 numberOfDatasetRecords = 0;

	
	fprintf(stdout, "readAllTest\n\n");
	
	
	
		p_productPath = "";
		p_schemaPath = "";
		p_schemaName = "";
		p_schemaVersion = "";
	
	if (strcmp(p_productPath, "NULL") == 0)
	{
		p_productPath = NULL;
	}
	if (strcmp(p_schemaPath, "NULL") == 0)
	{
		p_schemaPath = NULL;
	}
	if (strcmp(p_schemaName, "NULL") == 0)
	{
		p_schemaName = NULL;
	}
	if (strcmp(p_schemaVersion, "NULL") == 0)
	{
		p_schemaVersion = NULL;
	}*/
	
	/*
	user must call checkEEFormat() method
	{
		ExrwBool xrwBoolEEFormat = XRW_FALSE;
		
		returnCode = checkEEFormat(
				  p_productPath,
				  &xrwBoolEEFormat,
				  &xrwError); 
		if (returnCode == XRW_OK)
		{
			if (xrwBoolEEFormat == XRW_TRUE)
			{
				fprintf(stdout, "checkEEFormat True\n");
			}
			else if (xrwBoolEEFormat == XRW_FALSE)
			{
				fprintf(stdout, "checkEEFormat False\n");
			}
		}
		else
		{
			fprintf(stderr, "Error in checkEEFormat\n");
			printError(&xrwError);
			return -1;
		}
	}
	*/
	
	/*
	user must call validateProduct() method
	
	  returnCode = validateProduct(
	  p_productPath,
	  p_schemaPath,
	  p_schemaName,
	  p_schemaVersion,
	  &xrwError);
	  if (returnCode == XRW_OK)
	  {
	  fprintf(stdout, "validateProduct Ok\n");
	  }
	  else
	  {
	  fprintf(stderr, "Error in validateProduct\n");
	  fprintf(stderr, "Error[%d] %s\n", 1, getErrorMessage(&xrwError, 1));
	  fprintf(stderr, "Error[%d] %s\n", 2, getErrorMessage(&xrwError, 2));
	  printf("\n");
	  printError(&xrwError);
	  return -1;
	  }
	*/
	
	/*ftime( &start );
	
	p_xrwEEProduct = readProduct(
		p_productPath,
		p_schemaPath,
		p_schemaName,
		p_schemaVersion,
		&xrwError);
	if (p_xrwEEProduct)
	{
		fprintf(stdout, "readProduct Ok\n");
	}
	else
	{
		fprintf(stderr, "Error in readProduct\n");
		printError(&xrwError);
		return -1;
	}
	
	returnCode = getHeader(
				    p_xrwEEProduct,
					&xrwHeader,
					&xrwError);
	
	if (returnCode == XRW_OK)
	{
		fprintf(stdout, "getHeader Ok\n");
	}
	else
	{
		fprintf(stderr, "Error in getHeader\n");
		printError(&xrwError);
		return -1;
	}
	
	returnCode = getDataBlock(
				    p_xrwEEProduct,
					&xrwDataBlock,
					&xrwError);
	
	if (returnCode == XRW_OK)
	{
		fprintf(stdout, "getDataBlock Ok\n");
	}
	else
	{
		fprintf(stderr, "Error in getDataBlock\n");
		printError(&xrwError);
		return -1;
	}
	
	returnCode = getProductType(
				    p_xrwEEProduct,
					&productType,
					&xrwError);
	
	if (returnCode == XRW_OK)
	{
		fprintf(stdout, "getProductType Ok\n");
	}
	else
	{
		fprintf(stderr, "Error in getProductType\n");
		printError(&xrwError);
		return -1;
	}
	
	switch ( productType )
	{ 
	case PRODUCT_TYPE_HDRDBL:
		{ 
			TIbixUnsignedLong64 ds;
			TIbixUnsignedLong64 dsr;
			TIbixUnsignedLong64 numberOfDatasets = 0;
			
			returnCode = getCount(
				&xrwDataBlock,
				&numberOfDatasets,
				&xrwError);
			if (returnCode == XRW_OK)
			{
				fprintf(stdout, "getNumberOfDatasets Ok\n");
				fprintf(stdout, "numberOfDatasets = %lld\n", numberOfDatasets);
			}
			else
			{
				fprintf(stderr, "Error in getDataBlock\n");
				printError(&xrwError);
				return -1;
			}
			
			for (ds = 0; ds < numberOfDatasets; ds++)
			{
				
				p_xrwDataset = getDataset(
					&xrwDataBlock,
					ds,
					&xrwError);
				if (p_xrwDataset)
				{
					fprintf(stdout, "getDataset %lld Ok\n", ds);
				}
				else
				{
					fprintf(stderr, "Error in getDataset %lld\n", ds);
					printError(&xrwError);
					return -1;
				}
				
				returnCode = getCount(
					p_xrwDataset,
					&numberOfDatasetRecords,
					&xrwError);
				if (returnCode == XRW_OK)
				{
					fprintf(stdout, "getNumberOfDatasetRecords Ok\n");
					fprintf(stdout, "numberOfDatasetRecords = %lld\n", numberOfDatasetRecords);
				}
				else
				{
					fprintf(stderr, "Error in getNumberOfDatasetRecords\n");
					printError(&xrwError);
					return -1;
				}
				
				for (dsr = 0; dsr < numberOfDatasetRecords; dsr++)
				{
					p_xrwDatasetRecord = getDatasetRecord(
						p_xrwDataset,
						dsr,
						XRW_TRUE,
						&xrwError);
					if (p_xrwDatasetRecord)
					{
						fprintf(stdout, "getDatasetRecord %lld Ok\n", dsr);
					}
					else
					{
						fprintf(stderr, "Error in getDatasetRecord %lld\n", dsr);
						printError(&xrwError);
						return -1;
					}
					deleteObject((TxrwPart*) p_xrwDatasetRecord, &xrwError);
				}
				
				deleteObject((TxrwPart*) p_xrwDataset, &xrwError);
			}
		}
		break;	 
	default:
		break;   
	}
	
	returnCode = closeProduct(
		p_xrwEEProduct,
		&xrwError);
	if (returnCode == XRW_OK)
	{
		fprintf(stdout, "closeProduct Ok\n");
	}
	else
	{
		fprintf(stderr, "Error in closeProduct\n");
		printError(&xrwError);
		return -1;
	}*/
	
	/* Operation time control */
	/*ftime( &stop );
	elapsed_time = (stop.time + stop.millitm / 1000.0) - (start.time + start.millitm / 1000.0);
	fprintf(stdout, "\nElapsed time %0.3f s\n", elapsed_time);
*/
	
	//SMOS

	//String sCurDir = IlwWinApp()->sGetCurDir();
	SetCurrentDirectory(m_ascat_path.sVal());
	Array<String> aFiles;
	CFileFind finder;
	BOOL fFound = finder.FindFile("*.nat");
	while (fFound)
	{
		fFound = finder.FindNextFile();
		if (finder.IsHidden())
			continue;
		if (finder.IsDirectory()) 
			continue;

		String s = finder.GetFilePath();
		//FileName fn(s); 
		aFiles.push_back(finder.GetFilePath());
	}

	finder.Close();
	// create the output maps
	//DomainValueRangeStruct dv(-100,100,0.001);
	Map mpOut[10];
	Array<FileName> afn;

	FileName fnMs = m_fnObject;
	fnMs.sExt = ".mpr";
	fnMs.sFile = String("%S_Ms", fnMs.sFile);
	DomainValueRangeStruct dv1(0,100,0.01);
	//mpOut[0] = Map(fnMs,mp->gr(), mp->gr()->rcSize(),dv1);
	afn.push_back(fnMs);

	FileName fnMs_error = m_fnObject;
	fnMs_error.sExt = ".mpr";
	fnMs_error.sFile = String("%S_Ms_error", fnMs_error.sFile);
	//mpOut[1] = Map(fnMs_error,mp->gr(), mp->gr()->rcSize(),dv1);
	afn.push_back(fnMs_error);
	    
	FileName fnRainfall_flag = m_fnObject;
	fnRainfall_flag.sExt = ".mpr";
	String sName = String("%S_Rainfall_flag", fnRainfall_flag.sFile);
	fnRainfall_flag.sFile = sName;
	DomainValueRangeStruct dv2(0,100,1);
	//mpOut[2] = Map(fnRainfall_flag,mp->gr(), mp->gr()->rcSize(),dv2);
	afn.push_back(fnRainfall_flag);

	FileName fnProcessing_flag = m_fnObject;
	fnProcessing_flag.sExt = ".mpr";
	sName = String("%S_Processing_flag", fnProcessing_flag.sFile);
	fnProcessing_flag.sFile = sName;
	DomainValueRangeStruct dv3(0,1,1);
	//mpOut[3] = Map(fnProcessing_flag,mp->gr(), mp->gr()->rcSize(),dv3);
	afn.push_back(fnProcessing_flag);
	
	FileName fnQuality_flag = m_fnObject;
	fnQuality_flag.sExt = ".mpr";
	fnQuality_flag.sFile = String("%S_Quality_flag", fnQuality_flag.sFile);
	DomainValueRangeStruct dv4(0,255,1);
	//mpOut[4] = Map(fnQuality_flag,mp->gr(), mp->gr()->rcSize(),dv4);
	afn.push_back(fnQuality_flag);

	FileName fnSnowCover_flag = m_fnObject;
	fnSnowCover_flag.sExt = ".mpr";
	fnSnowCover_flag.sFile = String("%S_SnowCover_flag", fnSnowCover_flag.sFile);
	//mpOut[5] = Map(fnSnowCover_flag,mp->gr(), mp->gr()->rcSize(),dv2);
	afn.push_back(fnSnowCover_flag);

	FileName fnFrozenSoil_flag = m_fnObject;
	fnFrozenSoil_flag.sExt = ".mpr";
	fnFrozenSoil_flag.sFile = String("%S_FrozenSoil_flag", fnFrozenSoil_flag.sFile);
	//mpOut[6] = Map(fnFrozenSoil_flag,mp->gr(), mp->gr()->rcSize(),dv2);
	afn.push_back(fnFrozenSoil_flag);

	FileName fnInnudation_Wetland_flag = m_fnObject;
	fnInnudation_Wetland_flag.sExt = ".mpr";
	fnInnudation_Wetland_flag.sFile = String("%S_Innudation_Wetland_flag", fnInnudation_Wetland_flag.sFile);
	//mpOut[7] = Map(fnInnudation_Wetland_flag,mp->gr(), mp->gr()->rcSize(),dv2);
	afn.push_back(fnInnudation_Wetland_flag);

	FileName fnTopographical_Complexity_flag = m_fnObject;
	fnTopographical_Complexity_flag.sExt = ".mpr";
	fnTopographical_Complexity_flag.sFile = String("%S_Topographical_Complexity_flag", fnTopographical_Complexity_flag.sFile);
	//mpOut[8] = Map(fnTopographical_Complexity_flag,mp->gr(), mp->gr()->rcSize(),dv2);
	afn.push_back(fnTopographical_Complexity_flag);
	afn.push_back(m_fnObject); //soil moisture map
	
	int lines, cols;
	lines = iLines();
	cols = iCols();
	//allocate memory for the inputs 
	vector<RealBuf> vMs;
	vector<RealBuf> vMs_error;
	vector<IntBuf> vRainfall_flag;
	vector<IntBuf> vProcessing_flag;
	vector<IntBuf> vQuality_flag;
	vector<IntBuf> vSnowCover_flag;
	vector<IntBuf> vFrozenSoil_flag;
	vector<IntBuf> vInnudation_Wetland_flag;
	vector<IntBuf> vTopographical_Complexity_flag;

	vMs.resize(lines);
	vMs_error.resize(lines);
	vRainfall_flag.resize(lines);
	vProcessing_flag.resize(lines);
	vQuality_flag.resize(lines);
	vSnowCover_flag.resize(lines);
	vFrozenSoil_flag.resize(lines);
	vInnudation_Wetland_flag.resize(lines);
	vTopographical_Complexity_flag.resize(lines);

	vector<LongBuf> vCount;
	vCount.resize(lines);
	short  shUNDEF = ((short)-SHRT_MAX);
    trq.SetText("ASCAT Soil Moisture Level 2 Products");
	for (int iRow = 0; iRow < iLines(); ++iRow)
	{
		
		vMs[iRow].Size(iCols());
		vMs_error[iRow].Size(iCols());
		vRainfall_flag[iRow].Size(iCols());
		vProcessing_flag[iRow].Size(iCols());
		vQuality_flag[iRow].Size(iCols());
		vSnowCover_flag[iRow].Size(iCols());
		vFrozenSoil_flag[iRow].Size(iCols());
		vInnudation_Wetland_flag[iRow].Size(iCols());
		vTopographical_Complexity_flag[iRow].Size(iCols());
		vCount[iRow].Size(iCols());

		for (int iCol = 0; iCol < iCols(); ++iCol){
			vCount[iRow][iCol] = 1;
			vMs[iRow][iCol] = rUNDEF;
			vMs_error[iRow][iCol] = rUNDEF;
			vRainfall_flag[iRow][iCol] = shUNDEF;
			vProcessing_flag[iRow][iCol] = shUNDEF;
			vQuality_flag[iRow][iCol] = shUNDEF;
			vSnowCover_flag[iRow][iCol] = shUNDEF;
			vFrozenSoil_flag[iRow][iCol] = shUNDEF;
			vInnudation_Wetland_flag[iRow][iCol] = shUNDEF;
			vTopographical_Complexity_flag[iRow][iCol] = shUNDEF;
		}
	}

	GeoRef gr = mp->gr();
	RowCol rc = gr->rcSize();	
	double minx = gr->cb().MinX();
	double miny = gr->cb().MinY();
	double maxx = gr->cb().MaxX();
	double maxy = gr->cb().MaxY();
	int counts = aFiles.size(); 
	int swath = 1; //swath_indicator should be 0 or 1;
	trq.SetText("Processing ASCAT files");
	for(int count=0;count<counts;count++)
	{
		FileName fnAscat(aFiles[count]);
		File filAscat(aFiles[count]);
		if (!File::fExist(fnAscat)) {
			continue;
		}
		filAscat.Close();
		int nn,nr;
		//ascat_l2v1_file a;
		ascat_l2_file a;
		ascat_node b0;
		ascat_node b1;
		a.open(aFiles[count], &nr,&nn);
		for(int i=0;i<nr;i++)
		{
			a.read_mdr();
			for (int node=0;node<nn;node++)
			{
				swath = 0; //left swath
				a.get_node(node,swath,&b0);
				swath = 1; //right swath
				a.get_node(node,swath,&b1);
				
				int n = node;
			
				double lat0, lon0, lat1,lon1;
				lat0 = b0.lat;
				lat1 = b1.lat;
				if(b0.lon<=180)
					lon0= b0.lon;
				else
					lon0= b0.lon-360+1;
				if(b1.lon<=180)
					lon1= b1.lon;
				else
					lon1= b1.lon-360+1;

				int setbit;;
				if (lon0 >= gr->cb().MinX() && lon0 <= gr->cb().MaxX() && 
					lat0 >= gr->cb().MinY() && lat0 <= gr->cb().MaxY())  
				{
					Coord crd0 = Coord(lon0,lat0);
					RowCol rc0 = gr->rcConv(crd0);
					if(vMs[rc0.Row][rc0.Col] != rUNDEF && b0.sm_error <= 65){
						vCount[rc0.Row][rc0.Col]=vCount[rc0.Row][rc0.Col]+1;
						vMs[rc0.Row][rc0.Col] += b0.sm;
						vMs_error[rc0.Row][rc0.Col] += b0.sm_error;
						int rf = b0.frainfall;
						vRainfall_flag[rc0.Row][rc0.Col] += b0.frainfall;
						if ((b0.fprocessing & 63) == 1)
						vProcessing_flag[rc0.Row][rc0.Col] = 1;
						vQuality_flag[rc0.Row][rc0.Col] += b0.fquality;
						vSnowCover_flag[rc0.Row][rc0.Col] += b0.fsnow;
						vFrozenSoil_flag[rc0.Row][rc0.Col] += b0.ffrozen;
						vInnudation_Wetland_flag[rc0.Row][rc0.Col] += b0.fwetland;
						vTopographical_Complexity_flag[rc0.Row][rc0.Col] += b0.ftcomplexity;
					}
					else if (vMs[rc0.Row][rc0.Col] == rUNDEF && b0.sm_error <= 65){
						vMs[rc0.Row][rc0.Col] = b0.sm;
						vMs_error[rc0.Row][rc0.Col] = b0.sm_error;
						vRainfall_flag[rc0.Row][rc0.Col] = b0.frainfall;
						if ((b0.fprocessing & 63) == 0)
							setbit = 0;
						else
							setbit = 1;
						vProcessing_flag[rc0.Row][rc0.Col] = setbit;
						vQuality_flag[rc0.Row][rc0.Col] = b0.fquality;
						vSnowCover_flag[rc0.Row][rc0.Col] = b0.fsnow;
						vFrozenSoil_flag[rc0.Row][rc0.Col] = b0.ffrozen;
						vInnudation_Wetland_flag[rc0.Row][rc0.Col] = b0.fwetland;
						vTopographical_Complexity_flag[rc0.Row][rc0.Col] = b0.ftcomplexity;
					}
				}
				
				if (lon1 >= gr->cb().MinX() && lon1 <= gr->cb().MaxX() && 
					lat1 >= gr->cb().MinY() && lat1 <= gr->cb().MaxY() )  
				{
					Coord crd1 = Coord(lon1,lat1);
					RowCol rc1 = gr->rcConv(crd1);
					if(vMs[rc1.Row][rc1.Col] != rUNDEF && b1.sm_error <= 65){
						vCount[rc1.Row][rc1.Col]=vCount[rc1.Row][rc1.Col]+1;
						vMs[rc1.Row][rc1.Col] += b1.sm;
						vMs_error[rc1.Row][rc1.Col] += b1.sm_error;
						vRainfall_flag[rc1.Row][rc1.Col] += b1.frainfall;
						if ((b1.fprocessing & 63) == 1)
							vProcessing_flag[rc1.Row][rc1.Col] = 1;
						vQuality_flag[rc1.Row][rc1.Col] += b1.fquality;
						vSnowCover_flag[rc1.Row][rc1.Col] += b1.fsnow;
						vFrozenSoil_flag[rc1.Row][rc1.Col] += b1.ffrozen;
						vInnudation_Wetland_flag[rc1.Row][rc1.Col] += b1.fwetland;
						vTopographical_Complexity_flag[rc1.Row][rc1.Col] += b1.ftcomplexity;
					}
					else if(vMs[rc1.Row][rc1.Col] == rUNDEF && b1.sm_error <= 65){
						vMs[rc1.Row][rc1.Col] = b1.sm;
						vMs_error[rc1.Row][rc1.Col] = b1.sm_error;
						vRainfall_flag[rc1.Row][rc1.Col] = b1.frainfall;
						if ((b1.fprocessing & 63) == 0)
							setbit = 0;
						else
							setbit = 1;
						vProcessing_flag[rc1.Row][rc1.Col] = setbit;
						vQuality_flag[rc1.Row][rc1.Col] = b1.fquality;
						vSnowCover_flag[rc1.Row][rc1.Col] = b1.fsnow;
						vFrozenSoil_flag[rc1.Row][rc1.Col] = b1.ffrozen;
						vInnudation_Wetland_flag[rc1.Row][rc1.Col] = b1.fwetland;
						vTopographical_Complexity_flag[rc1.Row][rc1.Col] = b1.ftcomplexity;
					}
				}
			}
			trq.fUpdate(i,nr);
		}
		a.close();
	}
	//***a temporary maps holding the input signals    
	Map mpTmpOut[9];
	//DomainValueRangeStruct dvTmp(0,SHRT_MAX,1);
	DomainValueRangeStruct dvTmp(0,255,1);
	DomainValueRangeStruct dvMs(0,100,0.001);
	Array<FileName> aTmpfn;
	for (int iBand = 0; iBand < 9; iBand++) {
		String sName = String("%S_%ld", String("tmpOutput"), iBand);
		FileName fnTmp = FileName(sName);
		fnTmp.sExt = ".mpr";
		if (iBand == 0 || iBand == 1)
			mpTmpOut[iBand] = Map(fnTmp,mp->gr(), mp->gr()->rcSize(),dvMs);
		else
			mpTmpOut[iBand] = Map(fnTmp,mp->gr(), mp->gr()->rcSize(),dvTmp);
		//mpTmpOut[iBand]->fErase = true;
		aTmpfn.push_back(fnTmp);
	}

	//write the temporary output maps 
	trq.SetText("Create the temporary output maps");
	for (long iRow = 0; iRow < iLines(); ++iRow)
	{
		for (int iCol = 0; iCol < iCols(); ++iCol)
		{
			if (vMs[iRow][iCol] != rUNDEF)
				vMs[iRow][iCol] = vMs[iRow][iCol]/vCount[iRow][iCol]; 
			if (vMs_error[iRow][iCol] != rUNDEF)
				vMs_error[iRow][iCol] = vMs_error[iRow][iCol]/vCount[iRow][iCol];
			if (vRainfall_flag[iRow][iCol] != shUNDEF)
				vRainfall_flag[iRow][iCol] = vRainfall_flag[iRow][iCol]/vCount[iRow][iCol];
			//if (vProcessing_flag[iRow][iCol] != shUNDEF)
			//	vProcessing_flag[iRow][iCol] = vProcessing_flag[iRow][iCol]/vCount[iRow][iCol];
			if (vQuality_flag[iRow][iCol] != shUNDEF)
				vQuality_flag[iRow][iCol] = vQuality_flag[iRow][iCol]/vCount[iRow][iCol];
			if (vSnowCover_flag[iRow][iCol] != shUNDEF)
				vSnowCover_flag[iRow][iCol] = vSnowCover_flag[iRow][iCol]/vCount[iRow][iCol];
			if (vFrozenSoil_flag[iRow][iCol] != shUNDEF)
				vFrozenSoil_flag[iRow][iCol] = vFrozenSoil_flag[iRow][iCol]/vCount[iRow][iCol];
			if (vInnudation_Wetland_flag[iRow][iCol] != shUNDEF)
				vInnudation_Wetland_flag[iRow][iCol] = vInnudation_Wetland_flag[iRow][iCol]/vCount[iRow][iCol];
			if (vTopographical_Complexity_flag[iRow][iCol] != shUNDEF)
				vTopographical_Complexity_flag[iRow][iCol] = vTopographical_Complexity_flag[iRow][iCol]/vCount[iRow][iCol];
		}
		RealBuf& dataBuf = vMs[iRow];
		mpTmpOut[0]->PutLineVal(iRow, dataBuf);
		dataBuf = vMs_error[iRow];
		mpTmpOut[1]->PutLineVal(iRow, dataBuf);
		IntBuf& iBuf = vRainfall_flag[iRow];
		mpTmpOut[2]->PutLineRaw(iRow, iBuf);
		iBuf = vProcessing_flag[iRow];
		mpTmpOut[3]->PutLineRaw(iRow, iBuf);
		iBuf = vQuality_flag[iRow];
		mpTmpOut[4]->PutLineRaw(iRow, iBuf);
		iBuf = vSnowCover_flag[iRow];
		mpTmpOut[5]->PutLineRaw(iRow, iBuf);
		iBuf = vFrozenSoil_flag[iRow];
		mpTmpOut[6]->PutLineRaw(iRow, iBuf);
		iBuf = vInnudation_Wetland_flag[iRow];
		mpTmpOut[7]->PutLineRaw(iRow, iBuf);
		iBuf = vTopographical_Complexity_flag[iRow];
		mpTmpOut[8]->PutLineRaw(iRow, iBuf);
		trq.fUpdate(iRow, iLines());
	}
	//Clean up
	vMs.resize(0);
	vMs_error.resize(0);
	vProcessing_flag.resize(0);
	vRainfall_flag.resize(0);
	vQuality_flag.resize(0);
	vSnowCover_flag.resize(0);
	vFrozenSoil_flag.resize(0);
	vInnudation_Wetland_flag.resize(0);
	vTopographical_Complexity_flag.resize(0);
	vCount.resize(0);

	//Apply majority filter  
	for (int iMap = 0; iMap < 9; iMap++) {
		//MapFilter(output11,UndefMajority(3,3),value)
		String sExprFilterMap("MapFilter(%S, %S, %S)", aTmpfn[iMap].sFullPathQuoted(), String("UndefMajority(3,3)"), String("value")); 
		mpOut[iMap] = Map(afn[iMap], sExprFilterMap);
		mpOut[iMap]->Calc();
    }

	//write the output maps  
	trq.SetText("Create output maps");
	RealBuf rBufMv; 
	rBufMv.Size(iCols());
	
	for (long i = 0; i < iLines(); i++)
	{
		//mp->GetLineVal(i, rBufSand);
		mpOut[0]->GetLineVal(i, rBufMv);
		ptr.PutLineVal(i, rBufMv); 
		trq.fUpdate(i, iLines());
	}
	//create map list
	FileName fn = m_fnObject;
	fn.sExt = ".mpl";
	MapList ocL1b(fn, afn);
	trq.fUpdate(iLines(), iLines());
	return true;
}

void MapSoilMoistureFromAscat_l2::CompitableGeorefs(FileName fn, Map mp1, Map mp2)
{
	bool fIncompGeoRef = false;
	if (mp1->gr()->fGeoRefNone() && mp2->gr()->fGeoRefNone())
		  fIncompGeoRef = mp1->rcSize() != mp2->rcSize();
	else
			fIncompGeoRef = mp1->gr() != mp2->gr();
	if (fIncompGeoRef)
			throw ErrorIncompatibleGeorefs(mp1->gr()->sName(true, fn.sPath()),
            mp2->gr()->sName(true, fn.sPath()), fn, 1);
}
