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
#pragma warning( disable : 4786 )

#include "Headers\toolspch.h"

#pragma warning( disable : 4715 )

#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Engine\Base\File\ElementMap.h"
#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\Engine.h"
#include "Engine\DataExchange\ExportCsy2EsriPrj.h"
#include "Engine\Base\DataObjects\ObjectStructure.h"
#include "Headers\Hs\CONV.hs"
#include "Headers\Hs\IMPEXP.hs"
#include "Engine\DataExchange\Convloc.h"
#include "Engine\SpatialReference\prj.h"
#include "Engine\SpatialReference\CONIC.H"


ExportCsy2EsriPrj::ExportCsy2EsriPrj()
: fileESRIprj(0)
{
}

bool ExportCsy2EsriPrj::fInit(const FileName& fnIlwisObject, const FileName& fnEsriPrj)
{
	csy = CoordSystem(fnIlwisObject);
/*
	if (File::fExist(fnEsriPrj)) 
	{
		int iRet = getEngine()->Message(
															String(SIEMFileAlreadyExists_S.scVal(), fnEsriPrj.sFileExt()).scVal(),
                              SCVTitleExportArcGisPRJ.scVal(),
                              MB_YESNO | MB_ICONEXCLAMATION);
		if (iRet==IDNO)
      return false;//don't re-use existing filename, but give a new one
	} */
	fileESRIprj = new File(fnEsriPrj, facCRT);
	fileESRIprj->SetErase(true);
	FillProjNameAssocMap();
	FillEllipsoidNameAssocMap();
	FillDatumNameAssocMap();

	return true;
}

ExportCsy2EsriPrj::~ExportCsy2EsriPrj()
{
	if (fileESRIprj) delete fileESRIprj;
}

bool ExportCsy2EsriPrj::fInitSucces(const FileName& fnObject, const FileName& fnForeign)
{
	if (!fInit(fnObject, fnForeign))
		return false;
	if ( csy->fUnknown()) {
		throw ErrorImportExport(TR("Coordinate System unknown can not be exported to shape"));
	}

	CoordSystemLatLon* csll = csy->pcsLatLon();
  CoordSystemProjection *csprj = csy->pcsProjection();
	csviall = csy->pcsViaLatLon();
	if (!csviall)
		throw ErrorImportExport(SCVWarnNoProjectionOrLL);

	fSpherical = csviall->ell.fSpherical();
  if (0 != csprj)
	{	 
		prj = csprj->prj;
		if (!prj.fValid())
		throw ErrorImportExport(SCVWarnUnknownProjection);

		sProj = prj->sName();
		Datum* da = csprj->datum;
		String  sEllips = String();
		sDatum = String();
		if (fSpherical == 0)
			sEllips = csprj->ell.sName;
		if (!da && fCIStrEqual(sEllips,"WGS 84"))
		//don't make an Esri prj file without datum
			 sDatum = "WGS 1984"; //use WGS 1984 
		else if (!da && !fCIStrEqual(sEllips,"WGS 84"))
		 throw ErrorImportExport(SCVWarnNoSpheroidWithoutDatum);
	}
	return true;
}

// GeoTiff find-code functions similar to Geotiff export code
// were written in version 2 of this .cpp file but not used anymore


void ExportCsy2EsriPrj::WriteContentsPrjFile()
{
	if (fSpherical) 
	{
		sEllipsoid = "Sphere";
		sDatum = "Sphere";
	}
	else
	{
		sEllipsoid = csviall->ell.sName;
		if (csviall->datum && !fCIStrEqual(sDatum,"WGS 1984")) 
			sDatum = csviall->datum->sName();
	}
	if (prj.fValid()) {
		if (prj->ppParam(pvNORTH) != ppNONE) 
			fNorth = prj->iParam(pvNORTH) != 0;
		if (prj->ppParam(pvZONE) != ppNONE)
			iZone = prj->iParam(pvZONE);
		fileESRIprj->WriteLnAscii(sPROJCS());
	}
	else
		fileESRIprj->WriteLnAscii(sGEOGCS());
	// Export succesful, make output stay
	fileESRIprj->SetErase(false);
}

String ExportCsy2EsriPrj::sPROJCS()
{
	String s = "PROJCS[\"";
	if ( fCIStrEqual(sProj,"utm"))
	{
    s &= mpDatumNames[sDatum];
    s &= "_UTM";
    s &= String("_Zone_%i", iZone);
    s &= (fNorth ? "N" : "S");
	///e.g. String s("PROJCS[\"WGS_1972_UTM_Zone_38S\"");//use lookup?
	}
	else if ( fCIStrEqual(sProj,"Gauss-Boaga Italy"))
	{
    s &= "Monte_Mario_Italy";
    switch (iZone) {
			case 1: s &= String("_1"); break; 
			case 2: s &= String("_2"); break; 
		}
	}
	else if ( fCIStrEqual(sProj,"Gauss Colombia"))
	{
		s &= "Bogota_Colombia";
		switch (iZone) {
			case 1: {s &= String("_3W"); break; }
			case 2: {s &= String("Bogota"); break; }
			case 3: {s &= String("_3E"); break; }
			case 4: {s &= String("_6E"); break; }
	
		}
	}
	else if ( fCIStrEqual(sProj,"Gauss-Krueger Germany"))
	{
		s &= "Germany_";
    switch (iZone) {
			case 1: {s &= String("zone_1"); break; }
			case 2: {s &= String("zone_2"); break; }
			case 3: {s &= String("zone_3"); break; }
			case 4: {s &= String("zone_4"); break; }
			case 5: {s &= String("zone_5"); break; }
		}
	}
	else if ( fCIStrEqual(sProj,"Lambert Conform Conic France"))
	{
    s &= "France_";
    switch (iZone) {
			case 1: s &= String("I"); break;
			case 2: s &= String("II"); break;
			case 3: s &= String("III"); break;
			case 4: s &= String("IV"); break; 
		}
	}
	else if ( fCIStrEqual(sProj,"DutchRD"))
	{
    s &= "RD_New";
    }
	else  // name is taken from user's description or default descr
	{	
		String sDesc = csy->sDescription;
		replace(sDesc.begin(), sDesc.end(), '\"',' ');// remove quotes " (using escape \")	
		sDesc.sTrimSpaces();
		replace(sDesc.begin(), sDesc.end(), ' ','_'); // replace space by _
		s &= sDesc;
	}
	s &= "\",";
	s &= sGEOGCS();
	s &= sPROJECTION();
	s &= sPARAMETERS();
	s &= sUNITofLength();
	s &= String("]");
	return s;
}

String sTrimTrailingZeros(String& s)
{
	if (s.size() <= 0 ) 
		return "";

	int iLastDot = s.find_last_of('.');
	if (iLastDot != String::npos)
	{
		int iFindLastZero = s.find_last_not_of('0');
		if (iFindLastZero == String::npos)
			return String();

		if (iFindLastZero <= iLastDot)
			iFindLastZero = iLastDot - 1;
		return s.substr(0, iFindLastZero + 1);
	}

	return s;
}

String ExportCsy2EsriPrj::sGEOGCS()
{
	String s("GEOGCS[\"GCS_");
	if (0 == sProj.length()) // name taken from user's description or def descr if not yet done
	{	
		String sDesc = csy->sDescription;
		replace(sDesc.begin(), sDesc.end(), '\"',' ');// remove quotes " (using escape \")	
		sDesc.sTrimSpaces();
		replace(sDesc.begin(), sDesc.end(), ' ','_'); // replace space by _
		s &= sDesc;
	}
	else
		s &= mpDatumNames[sDatum];
	s &= String("\"");
	s &= sDATUM();	
	s &= sPRIMEM();
	s &= sUNITAngular();
	s &= String("]");
	return s;
}

String ExportCsy2EsriPrj::sDATUM()
{
	String s(",DATUM[\"D_");
	String sEsriDatum = mpDatumNames[sDatum];
	s &= sEsriDatum;
	s &= String("\"");
	s &= sSPHEROID();
	s &= String("]");
	return s;
}

String ExportCsy2EsriPrj::sPRIMEM()
{
	String s(",PRIMEM[\"Greenwich\",0"); //use eventually lookup 
	s &= String("]");
	return s;
}


String ExportCsy2EsriPrj::sUNITAngular()
{
	String s(",UNIT[\"Degree\",0.017453292519943295"); //use eventually lookup 
	s &= String("]");
	return s;
}

String ExportCsy2EsriPrj::sUNITofLength()
{
	String s(",UNIT[\"Meter\",1"); //use eventually lookup 
	s &= String("]");
	return s;
}

String ExportCsy2EsriPrj::sSPHEROID()
{
	String s(",SPHEROID[\"");
	String sEsriSpheroi = mpEllipsoidNames[sEllipsoid];
	s &= sEsriSpheroi;
	s &= String("\"");
	double rA = csviall->ell.a;
	s &= sTrimTrailingZeros(String(",%7.3f",rA));
	if (!fSpherical) {
		double rFinv = 1 / csviall->ell.f;
		s &= sTrimTrailingZeros(String(",%3.9f",rFinv));
	}
	else
		s &= String(",0"); //sphere has no flattening, rFinv is UNDEF, but Esri calls it 0
	s &= String("]");
	return s;
}

String ExportCsy2EsriPrj::sPROJECTION()
{
	String s(",PROJECTION[\""); 
	String sEsriProj = mpProjNames[sProj];
	s &= sEsriProj;
	s &= String("\"");
	///sProjectionEsriName(pn); //to be replaced by lookup? in EsriPrj.def
	s &= String("]");
	return s;
}

String ExportCsy2EsriPrj::sPARAMETERS()
{
	bool             fValue[5];
  long             iValue[5];
  double           rValue[10];
	int iF = 0, iI = 0, iR = 0;
	String s, sPar, sAllProjParms;
	String sZone, sNorth; //for zoned projections, for hemisphere selection
	////////////needed to make the PROJCS string in sPROJCS()
///(((//	ProjectionConic *prjcon = pprjConic();
	for (int i = 0; i <= pvLAT2; ++i) {
		 //cf CoordSysProjectionView::create() 
		s = String();
		ProjectionParamValue ppv = ProjectionParamValue(i);
		ProjectionParamType pp = prj->ppAllParam(ppv);
		if (pp == ppNONE)
			continue;
		String sParam = sParamEsriName(ppv); //new function of ProjectionPtr (in prj.cpp)
		switch (pp) {
			case ppZONE:
				sZone =	String("%S: %li", sParam, iValue[iI]);
				iI++;
				break;
			case ppBOOL: // for fNorth (hemisphere)
				fValue[iF] = prj->iParam(ppv) != 0;
				sNorth = String("%S: %s", sParam, fValue[iF]?"Yes":"No");
				iF++;
				break;
			case ppLAT:
				if (prj->fUseParam[i]  // in order to skip useless standard parallels 
						|| (ppv == pvLAT0))  // in order to comply with esri parameters 
				{
						rValue[iR] = prj->rParam(ppv);
				//s &= String("%S,%S", sParam, LatLon::sLat(rValue[iR]));
					s &= sTrimTrailingZeros(String("\"%S\",%.12f", sParam, rValue[iR]));
				}
				iR++;
				break;
			case ppLON:
				rValue[iR] = prj->rParam(ppv);
				//s &= String("%S,%S", sParam, LatLon::sLon(rValue[iR]));
				s &= sTrimTrailingZeros(String("\"%S\",%.12f", sParam, rValue[iR]));
				iR++;
				break;
			case ppREAL:
				rValue[iR] = prj->rParam(ppv);
				s &= sTrimTrailingZeros(String("\"%S\",%.3f", sParam, rValue[iR]));
				iR++;
				break;
			case ppSCALE:
				rValue[iR] = prj->rParam(ppv);
				s &= sTrimTrailingZeros(String("\"%S\",%.10f", sParam, rValue[iR]));
				iR++;
				break;
		}
		// if (pp == ppZONE || pp == ppBOOL) // not used in Esri .prj
		// 	continue; // test already included in next if
		if ((0 != s.length()) && (0 != sParam.length())) {
			sPar = String(",PARAMETER[") & s;
			sPar &= String("]");
			sAllProjParms &= sPar;
		}
	}
	
	return sAllProjParms;
}

// Esri parameterNames found in Arcgis8.2
/*
False_Easting
False_Northing
Central_Meridian
Scale_Factor
Latitude_Of_Origin
Standard_Parallel_1
Standard_Parallel_2
Azimuth
Longitude_Of_Center
Latitude_Of_Center
Latitude_Of_1st_Point
Latitude_Of_2nd_Point
Longitude_Of_1st_Point
Longitude_Of_2nd_Point
Longitude_Of_Origin
Central_Parallel
Pseudo_Standard_Parallel_1
X_Scale
Y_Scale
XY_Plane_Rotation
Height
*/

String ExportCsy2EsriPrj::sParamEsriName(ProjectionParamValue pv) const
{
  switch (pv) {
    case pvX0:        return "False_Easting";
    case pvY0:        return "False_Northing";
    case pvLON0:      return "Central_Meridian";
    case pvLATTS:     return ""; // no "Latitude_Of_True_Scale"; found
    case pvLAT0:      return "Latitude_Of_Origin";
    case pvLAT1:      return "Standard_Parallel_1";
    case pvLAT2:      return "Standard_Parallel_2";
    case pvK0:        return "Scale_Factor";
    case pvNORTH:     return ""; //no "Northern_Hemisphere" found in ArcGis;
    case pvZONE:      return ""; // no "Zone"; found
    case pvHEIGHT:    return "Height";
    case pvTILTED:    return "Tilted_Rotated_Projection_Plane";
    case pvTILT:      return "Tilt_Of_Projection_Plane";
    case pvAZIMYAXIS: return "XY_Plane_Rotation";
    case pvAZIMCLINE: return "Azimuth";
    case pvPOLE:      return "Pole_Of_Oblique_Cylinder";
    case pvNORIENTED: return "North_Oriented_XY_Coord_System";
    default:          return "Unknown_Parameter";
  }  
}

void ExportCsy2EsriPrj::FillProjNameAssocMap() // associates Ilwis with Esri names
{
	mpProjNames["Azimuthal Equidistant"]	= "Azimuthal_Equidistant";  
	mpProjNames["Albers EqualArea Conic"]	=	"Albers";  
	mpProjNames["Central Cylindrical"]	= "Central_Cylindrical";  
	mpProjNames["Bonne"]		=	"Bonne";
	mpProjNames["Cassini"]	= "Cassini";
	mpProjNames["DutchRD"]	=	"Double_Stereographic";
	mpProjNames["Eckert1"]	=	"Eckert_I";
	mpProjNames["Eckert2"]	= "Eckert_II";
	mpProjNames["Eckert3"]	= "Eckert_III";
	mpProjNames["Equidistant Conic"]		= "Equidistant_Conic";
	mpProjNames["Gauss-Boaga Italy"]		=  "Transverse_Mercator";
	mpProjNames["Gauss Colombia"]				="Transverse_Mercator";
	mpProjNames["Gauss-Krueger Germany"]=  "Transverse_Mercator";
	mpProjNames["General Perspective"]	= "Vertical_Near_Side_Perspective";
	mpProjNames["Gnomonic"]			= "Gnomonic";
	mpProjNames["Goode"]				= "Goode";
	mpProjNames["Hammer Aitoff"]= "Hammer_Aitoff";
	mpProjNames["Lambert Conformal Conic"] = "Lambert_Conformal_Conic";
	mpProjNames["Lambert Cylind EqualArea"]= "Cylindrical_Equal_Area";
	mpProjNames["LPOLEQA"]		= "Polar_Equal_Area";
	mpProjNames["Lambert Azimuthal EqualArea"]	= "Lambert_Azimuthal_Equal_Area";
	mpProjNames["Lambert Conform Conic France"]	= "Lambert_Conformal_Conic";
	mpProjNames["Mercator"]		= "Mercator";
	mpProjNames["Miller"]			= "Miller_Cylindrical";
	mpProjNames["Mollweide"]	= "Mollweide";
	mpProjNames["Oblique Mercator"]	= "Hotine_Oblique_Mercator_Azimuth_Center";
	mpProjNames["Orthographic"]			= "Orthographic";
	mpProjNames["Plate Carree"]			= "Plate_Carree";
	mpProjNames["Plate Rectangle"]	= "Equidistant_Cylindrical";
	mpProjNames["PolyConic"]				= "Polyconic";
	mpProjNames["Robinson"]		= "Robinson";
	mpProjNames["SCHREIB"]		= "Schreiber";
	mpProjNames["Sinusoidal"]	= "Sinusoidal"; 
	mpProjNames["Sinusoidal Interrupted"]	= "Sinusoidal_Interrupted";
	mpProjNames["Sinusoidal 2 x Interrupted"]	= "Sinusoidal_Interrupted";
	mpProjNames["Sinusoidal 3 x Interrupted"]	= "Sinusoidal_Interrupted";
	mpProjNames["Transverse Mercator"]	= "Transverse_Mercator";
	mpProjNames["UPS"]						= "Stereographic";
	mpProjNames["UTM"]						= "Transverse_Mercator";
	mpProjNames["VanderGrinten"]	= "Van_der_Grinten_I";
	mpProjNames["VanHuut"]				= "Van_Huut";
	mpProjNames["Gauss Krueger"]		= "Gauss_Kruger";
	mpProjNames["Equidistant cyl"]	= "Equidistant_Cylindrical";  
}

void ExportCsy2EsriPrj::FillDatumNameAssocMap()
{
	mpDatumNames["Sphere"] = "Sphere";
	mpDatumNames["Adindan"] = "Adindan";
	mpDatumNames["Afgooye"] = "Afgooye";
	mpDatumNames["AGREF (Austrian Geodetic Reference)"] = "MGI";
	mpDatumNames["Ain el Abd 1970"] = "Ain_el_Abd_1970";
	mpDatumNames["American Samoa 1962"] = "American_Samoa_1962";
	mpDatumNames["Anna 1 Astro 1965"] = "Anna_1_1965";
	mpDatumNames["Antigua Island Astro 1943"] = "Antigua_1943";
	mpDatumNames["Arc 1950"] = "Arc_1950";
	mpDatumNames["Arc 1960"] = "Arc_1960";
	mpDatumNames["Ascension Island 1958"] = "Ascension_Island_1958";
	mpDatumNames["Astro Beacon E 1945"] = "Beacon_E_1945";
	mpDatumNames["Astro DOS 71/4"] = "DOS_71_4";
	mpDatumNames["Astro Tern Island (FRIG) 1961"] = "Tern_Island_1961";
	mpDatumNames["Astronomical Station 1952"] = "Astro_1952";
	mpDatumNames["Australian Geodetic 1966"] = "Australian_1966";
	mpDatumNames["Australian Geodetic 1984"] = "Australian_1984";
	mpDatumNames["Ayabelle Lighthouse"] = "Ayabelle";
	mpDatumNames["Bellevue (IGN)"] = "Bellevue_IGN";
	mpDatumNames["Bermuda 1957"] = "Bermuda_1957";
	mpDatumNames["Bissau"] = "Bissau";
	mpDatumNames["Bogota Observatory"] = "Bogota";
	mpDatumNames["Bukit Rimpah"] = "Bukit_Rimpah";
	mpDatumNames["Camp Area Astro"] = "";
	mpDatumNames["Campo Inchauspe"] = "";
	mpDatumNames["Canton Astro 1966"] = "";
	mpDatumNames["Cape"] = "";
	mpDatumNames["Cape Canaveral"] = "Cape_Canaveral";
	mpDatumNames["Carthage"] = "Carthage";
	mpDatumNames["CH1903"] = "CH1903";
	mpDatumNames["Chatham Island Astro 1971"] = "Chatham_Island_1971";
	mpDatumNames["Chua Astro"] = "Chua";
	mpDatumNames["Corrego Alegre"] = "Corrego_Alegre";
	mpDatumNames["Croatia"] = "Croatia";
	mpDatumNames["D-PAF (Orbits)"] = "";
	mpDatumNames["Dabola"] = "Dabola";
	mpDatumNames["Deception Island"] = "Deception_Island";
	mpDatumNames["Djakarta (Batavia)"] = "";
	mpDatumNames["DOS 1968"] = "DOS_1968";
	mpDatumNames["Easter Island 1967"] = "Easter_Island_1967";
	mpDatumNames["Estonia 1937"] = "Estonia_1937";
	mpDatumNames["European 1950 (ED 50)"] = "ED_50";
	mpDatumNames["European 1979 (ED 79)"] = "ED_79";
	mpDatumNames["Fort Thomas 1955"] = "Fort_Thomas_1955";
	mpDatumNames["Gan 1970"] = "Gan_1970";
	mpDatumNames["Geodetic Datum 1949"] = "";
	mpDatumNames["Graciosa Base SW 1948"] = "Graciosa_Base_SW_1948";
	mpDatumNames["Guam 1963"] = "Guam_1963";
	mpDatumNames["Gunung Segara"] = "Gunung_Segara";
	mpDatumNames["GUX 1 Astro"] = "GUX_1";
	mpDatumNames["Herat North"] = "Herat_North";
	mpDatumNames["Hermannskogel"] = "Hermannskogel";
	mpDatumNames["Hjorsey 1955"] = "Hjorsey_1955";
	mpDatumNames["Hong Kong 1963"] = "Hong_Kong_1963";
	mpDatumNames["Hu-Tzu-Shan"] = "Hu_Tzu_Shan";
	mpDatumNames["Indian (Bangladesh)"] = "";
	mpDatumNames["Indian (India, Nepal)"] = "";
	mpDatumNames["Indian (Pakistan)"] = "";
	mpDatumNames["Indian 1954"] = "Indian_1954";
	mpDatumNames["Indian 1960"] = "Indian_1960";
	mpDatumNames["Indian 1975"] = "Indian_1975";
	mpDatumNames["Indonesian 1974"] = "Indonesian_1974";
	mpDatumNames["Ireland 1965"] = "";
	mpDatumNames["ISTS 061 Astro 1968"] = "";
	mpDatumNames["ISTS 073 Astro 1969"] = "";
	mpDatumNames["Johnston Island 1961"] = "Johnston_Island_1961";
	mpDatumNames["Kandawala"] = "Kandawala";
	mpDatumNames["Kerguelen Island 1949"] = "Kerguelen_Island_1949";
	mpDatumNames["Kertau 1948"] = "Kertau";
	mpDatumNames["Kusaie Astro 1951"] = "Kusaie_1951";
	mpDatumNames["L. C. 5 Astro 1961"] = "";
	mpDatumNames["Leigon"] = "Leigon";
	mpDatumNames["Liberia 1964"] = "Liberia_1964";
	mpDatumNames["Luzon"] = "Luzon_1911";
	mpDatumNames["M'Poraloko"] = "Mporaloko";
	mpDatumNames["Mahe 1971"] = "Mahe_1971";
	mpDatumNames["Massawa"] = "Massawa";
	mpDatumNames["Merchich"] = "Merchich";
	mpDatumNames["MGI (Hermannskogel)"] = "MGI";
	mpDatumNames["Midway Astro 1961"] = "";
	mpDatumNames["Minna"] = "Minna";
	mpDatumNames["Montserrat Island Astro 1958"] = "Montserrat_1958";
	mpDatumNames["Nahrwan"] = "Nahrwan_1967";
	mpDatumNames["Naparima BWI"] = "Naparima_1955";
	mpDatumNames["North American 1927 (NAD 27)"] = "North_American_1927";
	mpDatumNames["North American 1983 (NAD 83)"] = "North_American_1983";
	mpDatumNames["North Sahara 1959"] = "Nord_Sahara_1959";	
	mpDatumNames["NTF (Nouvelle Triangulation de France)"] = "";
	mpDatumNames["Observatorio Meteorologico 1939"] = "Observ_Meteorologico_1939";
	mpDatumNames["Old Egyptian 1907"] = "";
	mpDatumNames["Old Hawaiian"] = "Old_Hawaiian";
	mpDatumNames["Oman"] = "Oman";
	mpDatumNames["Ordnance Survey Great Britain 1936"] = "";
	mpDatumNames["Pico de las Nieves"] = "Pico_de_Las_Nieves";
	mpDatumNames["Pitcairn Astro 1967"] = "Pitcairn_1967";
	mpDatumNames["Point 58"] = "Point_58";
	mpDatumNames["Pointe Noire 1948"] = "Pointe_Noire";
	mpDatumNames["Porto Santo 1936"] = "Porto_Santo_1936";
	mpDatumNames["Potsdam (Rauenburg)"] = "Deutsche_Hauptdreiecksnetz";
	mpDatumNames["Provisional South American 1956"] = "Provisional_S_American_1956";
	mpDatumNames["Provisional South Chilean 1963"] = "Provisional_S_American_1963";
	mpDatumNames["Puerto Rico"] = "Puerto_Rico";
	mpDatumNames["Pulkovo 1942"] = "Pulkovo_1942";
	mpDatumNames["Qatar National"] = "Qatar";
	mpDatumNames["Qornoq"] = "Qornoq";
	mpDatumNames["Reunion"] = "Reunion";
	mpDatumNames["Rome 1940"] = "Monte_Mario";
	mpDatumNames["RT90"] = "RT_1990";
	mpDatumNames["Rijks Driehoeksmeting"] = "Amersfoort";
	mpDatumNames["S-42 (Pulkovo 1942)"] = "Pulkovo_1942";
	mpDatumNames["S-JTSK"] = "S_JTSK";
	mpDatumNames["Santo (DOS) 1965"] = "Santo_DOS_1965";
	mpDatumNames["Sao Braz"] = "Sao_Braz";
	mpDatumNames["Sapper Hill 1943"] = "Sapper_Hill_1943";
	mpDatumNames["Schwarzeck"] = "Schwarzeck";
	mpDatumNames["Selvagem Grande 1938"] = "Selvagem_Grande_1938";
	mpDatumNames["vSGS 1985"] = "";
	mpDatumNames["Sierra Leone 1960"] = "Sierra_Leone_1960";
	mpDatumNames["South American 1969"] = "South_American_1969";
	mpDatumNames["South Asia"] = "";
	mpDatumNames["Tananarive Observatory 1925"] = "Tananarive_1925";
	mpDatumNames["Timbalai 1948"] = "Timbalai_1948";
	mpDatumNames["Tokyo"] = "Tokyo";
	mpDatumNames["Tristan Astro 1968"] = "Tristan Astro 1968";
	mpDatumNames["Viti Levu 1916"] = "Viti_Levu_1916";
	mpDatumNames["Voirol 1874"] = "Voirol_1875";
	mpDatumNames["Voirol 1960"] = "Voirol_Unifie_1960";
	mpDatumNames["Wake Island Astro 1952"] = "Wake_Island_1952";
	mpDatumNames["Wake-Eniwetok 1960"] = "Wake_Eniwetok_1960";
	mpDatumNames["WGS 1972"] = "WGS_1972";
	mpDatumNames["WGS 1984"] = "WGS_1984";
	mpDatumNames["Yacare"] = "Yacare";
	mpDatumNames["Zanderij"] = "Zanderij";
}
	
void ExportCsy2EsriPrj::FillEllipsoidNameAssocMap()
{
	mpEllipsoidNames["Sphere"] = "Sphere";
	mpEllipsoidNames["Airy 1830"] = "Airy_1830";
	mpEllipsoidNames["Modified Airy"] = "Airy_Modified";
	mpEllipsoidNames["ATS77"] = "ATS_1977";
	mpEllipsoidNames["Australian National"] = "Australian";
	mpEllipsoidNames["Bessel 1841"] = "Bessel_1841";
	mpEllipsoidNames["Bessel 1841 (Japan By Law)"] = "";
	mpEllipsoidNames["Bessel 1841 (Namibia)"] = "Bessel_Namibia";
	mpEllipsoidNames["Clarke 1866"] = "Clarke_1866";
	mpEllipsoidNames["Clarke 1880"] = "Clarke_1880";
	mpEllipsoidNames["Clarke 1880 (IGN)"] = "Clarke_1880_IGN";
	mpEllipsoidNames["D-PAF (Orbits)"] ="";
	mpEllipsoidNames["Du Plessis Modified"] ="Du_Plessis_Modified";
	mpEllipsoidNames["Du Plessis Reconstituted"] ="Du_Plessis_Reconstituted";
	mpEllipsoidNames["Everest (India 1830)"] ="Everest_1830";
	mpEllipsoidNames["Everest (India 1956)"] ="";
	mpEllipsoidNames["Everest (Malaysia 1969)"] ="Everest_Modified_1969";
	mpEllipsoidNames["Everest (E. Malaysia and Brunei)"] ="";
	mpEllipsoidNames["Everest (Malay. and Singapore 1948)"] ="";
	mpEllipsoidNames["Everest (Pakistan)"] ="";
	mpEllipsoidNames["Everest (Sabah Sarawak)"] ="";
	mpEllipsoidNames["Fischer 1960"] ="Fischer_1960";
	mpEllipsoidNames["Fischer 1960 (Modified)"] ="Fischer_Modified";
	mpEllipsoidNames["Fischer 1968"] ="Fischer_1968";
	mpEllipsoidNames["GRS 80"] =			"GRS_1980";
	mpEllipsoidNames["Helmert 1906"] ="Helmert_1906";
	mpEllipsoidNames["Hough 1960"] =	"Hough_1960";
	mpEllipsoidNames["Indonesian 1974"] =		"Indonesian";
	mpEllipsoidNames["International 1924"] ="International_1924";
	mpEllipsoidNames["Krassovsky 1940"] =		"Krasovsky_1940";
	mpEllipsoidNames["New_International 1967"] ="New_International_1967";
	mpEllipsoidNames["SGS 85"] = "SGS_85";
	mpEllipsoidNames["South American 1969"] ="South_American_1969";
	mpEllipsoidNames["WGS 60"] = "WGS_1960";
	mpEllipsoidNames["WGS 66"] = "WGS_1966";
	mpEllipsoidNames["WGS 72"] = "WGS_1972";
	mpEllipsoidNames["WGS 84"] = "WGS_1984";
}

void ImpExp::ExportEsriPrj(const FileName& fnIlwisObject, const FileName& fnEsriPrj)
{
	ExportCsy2EsriPrj EsriExpPrj;
	if (EsriExpPrj.fInitSucces(fnIlwisObject, fnEsriPrj))
		EsriExpPrj.WriteContentsPrjFile();
}
