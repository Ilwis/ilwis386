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
// MapSMAC.cpp: implementation of the MapSMAC class.
//
/* SMAC adaptation of SMAC (Rahman, H and Dedieu G., 1994) for the	
   atmospheric correction of visible and near visible bands of several
   satellite sensors.  	
   January 2008, by Lichun Wang
*/
#include "SEBS\MapSMAC.h"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Hs\map.hs"

using namespace std;

IlwisObjectPtr * createMapSMAC(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)MapSMAC::create(fn, (MapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new MapSMAC(fn, (MapPtr &)ptr);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
MapSMAC::MapSMAC(const FileName& fn, MapPtr& p)
: MapFromMap(fn, p)
{
	ReadElement("MapSMAC", "Reflectance", mp_r_toa);
	ReadElement("MapSMAC", "CoefficientFile", m_coef_filename);
	ReadElement("MapSMAC", "useOpticalDepth550Map", is_mp_taup550);
	ReadElement("MapSMAC", "useWaterVaporMap", is_mp_uh2o);
	ReadElement("MapSMAC", "useOzoneContentMap", is_mp_uo3);
	ReadElement("MapSMAC", "useSurfacePressure", is_mp_pression);
	ReadElement("MapSMAC", "useSolarZenithAngleMap", is_mp_tetas);
	ReadElement("MapSMAC", "useSolarAzimutAngleMap", is_mp_phis);
	ReadElement("MapSMAC", "useViewZenithAngleMap", is_mp_tetav);
	ReadElement("MapSMAC", "useViewAzimutAngleMap", is_mp_phiv);
	ReadElement("MapSMAC", "useCloudMaskMap", is_mp_cloudmask);
	
	ReadElement("MapSMAC", "OpticalDepth550", s_taup550);
	ReadElement("MapSMAC", "WaterVapor", s_uh2o);
	ReadElement("MapSMAC", "OzoneContent", s_uo3);
	ReadElement("MapSMAC", "SurfacePressure", s_pression);
	ReadElement("MapSMAC", "SolarZenithAngle", s_tetas);
	ReadElement("MapSMAC", "SolarAzimutAngle", s_phis);
	ReadElement("MapSMAC", "ViewZenithAngle", s_tetav);
	ReadElement("MapSMAC", "ViewAzimutAngle", s_phiv);
	
	if (is_mp_taup550){
		ReadElement("MapSMAC", "OpticalDepth550", mp_taup550);
		CompitableGeorefs(fn, mp_r_toa, mp_taup550);
	}
	else
		ReadElement("MapSMAC", "OpticalDepth550", s_taup550);
	if (is_mp_uh2o){
		ReadElement("MapSMAC", "WaterVapor", mp_uh2o);
		CompitableGeorefs(fn, mp_r_toa, mp_uh2o);
	}
	else
		ReadElement("MapSMAC", "WaterVapor", s_uh2o);
	if (is_mp_uo3){
		ReadElement("MapSMAC", "OzoneContent", mp_uo3);
		CompitableGeorefs(fn, mp_r_toa, mp_uo3);
	}
	else
		ReadElement("MapSMAC", "OzoneContent", s_uo3);
	if (is_mp_pression){
		ReadElement("MapSMAC", "SurfacePressure", mp_pression);
		CompitableGeorefs(fn, mp_r_toa, mp_pression);
	}
	else
		ReadElement("MapSMAC", "SurfacePressure", s_pression);

	if (is_mp_tetas){
		ReadElement("MapSMAC", "SolarZenithAngle", mp_tetas);
		CompitableGeorefs(fn, mp_r_toa, mp_tetas);
	}
	else
		ReadElement("MapSMAC", "SolarZenithAngle", s_tetas);
	if (is_mp_phis){
		ReadElement("MapSMAC", "SolarAzimutAngle", mp_phis);
		CompitableGeorefs(fn, mp_r_toa, mp_phis);
	}
	else
		ReadElement("MapSMAC", "SolarAzimutAngle", s_phis);
	if (is_mp_tetav){
		ReadElement("MapSMAC", "ViewZenithAngle", mp_tetav);
		CompitableGeorefs(fn, mp_r_toa, mp_tetav);
	}
	else
		ReadElement("MapSMAC", "ViewZenithAngle", s_tetav);
	if (is_mp_phiv){
		ReadElement("MapSMAC", "ViewAzimutAngle", mp_phiv);
		CompitableGeorefs(fn, mp_r_toa, mp_phiv);
	}
	else
		ReadElement("MapSMAC", "ViewAzimutAngle", s_phiv);

	if (is_mp_cloudmask){
		ReadElement("MapSMAC", "CloudMask", mp_cloudmask);
		CompitableGeorefs(fn, mp_r_toa, mp_cloudmask);
	}
	
	fNeedFreeze = true;
	sFreezeTitle = "Atmospheric Effect Correction (SMAC)";
	htpFreeze = "ilwisapp\\smac_algorithm.htm";
}

MapSMAC::MapSMAC(const FileName& fn, 
				MapPtr& p,
				const Map& ref_mp,
				String coef_file,
				bool fOpticalDepthMap, String staup550,
				bool fWaterVaporMap, String suh2o,
				bool fOzoneContentMap, String suo3,
				bool fSurfacePressureMap, String spression,
				bool fSolarZenithAngleMap, String stetas,
				bool fSolarAzimutAngleMap, String sphis,
				bool fViewZenithAngleMap, String stetav,
				bool fViewAzimutAngleMap, String sphiv,
				bool fCloudMaskMap, String scloudmask)	
: MapFromMap(fn, p, ref_mp),
  mp_r_toa(ref_mp),	
  m_coef_filename(coef_file),
  is_mp_taup550(fOpticalDepthMap),
  is_mp_uh2o(fWaterVaporMap),
  is_mp_uo3(fOzoneContentMap),
  is_mp_pression(fSurfacePressureMap),
  is_mp_tetas(fSolarZenithAngleMap),
  is_mp_phis(fSolarAzimutAngleMap),
  is_mp_tetav(fViewZenithAngleMap),
  is_mp_phiv(fViewAzimutAngleMap),
  s_taup550(staup550),
  s_uh2o(suh2o),
  s_uo3(suo3),
  s_pression(spression),
  s_tetas(stetas),
  s_phis(sphis),
  s_tetav(stetav),
  s_phiv(sphiv),
  is_mp_cloudmask(fCloudMaskMap),
  s_cloudmask(scloudmask)
  
{
	DomainValueRangeStruct dv(-1000,1000,0.00001);
	SetDomainValueRangeStruct(dv);
	if (is_mp_taup550){
		mp_taup550 = Map(staup550, fn.sPath());
		CompitableGeorefs(fn, mp, mp_taup550);
		objdep.Add(mp_taup550);
	}
	if (is_mp_uh2o){
		mp_uh2o = Map(suh2o, fn.sPath());
		CompitableGeorefs(fn, mp, mp_uh2o);
		objdep.Add(mp_uh2o);
	}
	if (is_mp_uo3){
		mp_uo3 = Map(suo3, fn.sPath());
		CompitableGeorefs(fn, mp, mp_uo3);
		objdep.Add(mp_uo3);
	}
	if (is_mp_pression){
		mp_pression = Map(spression, fn.sPath());
		CompitableGeorefs(fn, mp, mp_pression);
		objdep.Add(mp_pression);
	}

	if (is_mp_tetas){
		mp_tetas = Map(stetas, fn.sPath());
		CompitableGeorefs(fn, mp, mp_tetas);
		objdep.Add(mp_tetas);
	}
	if (is_mp_phis){
		mp_phis = Map(sphis, fn.sPath());
		CompitableGeorefs(fn, mp, mp_phis);
		objdep.Add(mp_phis);
	}
	if (is_mp_tetav){
		mp_tetav = Map(stetav, fn.sPath());
		CompitableGeorefs(fn, mp, mp_tetav);
		objdep.Add(mp_tetav);
	}
	if (is_mp_phiv){
		mp_phiv = Map(sphiv, fn.sPath());
		CompitableGeorefs(fn, mp, mp_phiv);
		objdep.Add(mp_phiv);
	}
	if (is_mp_cloudmask){
		mp_cloudmask = Map(scloudmask, fn.sPath());
		CompitableGeorefs(fn, mp, mp_cloudmask);
		objdep.Add(mp_cloudmask);
	}

    if (!fnObj.fValid())
     objtime = objdep.tmNewest();
	ptr.Store(); 
	fNeedFreeze = true;
	sFreezeTitle = TR("Atmospheric Effect Correction (SMAC)");
	htpFreeze = "ilwisapp\\smac_algorithm.htm";
}

MapSMAC::MapSMAC(const FileName& fn, 
				MapPtr& p,
				const Map& ref_mp,
				String coef_file,
				bool fOpticalDepthMap, String staup550,
				bool fWaterVaporMap, String suh2o,
				bool fOzoneContentMap, String suo3,
				bool fSurfacePressureMap, String spression,
				bool fSolarZenithAngleMap, String stetas,
				bool fSolarAzimutAngleMap, String sphis,
				bool fViewZenithAngleMap, String stetav,
				bool fViewAzimutAngleMap, String sphiv)
: MapFromMap(fn, p, ref_mp),
  mp_r_toa(ref_mp),	
  m_coef_filename(coef_file),
  is_mp_taup550(fOpticalDepthMap),
  is_mp_uh2o(fWaterVaporMap),
  is_mp_uo3(fOzoneContentMap),
  is_mp_pression(fSurfacePressureMap),
  is_mp_tetas(fSolarZenithAngleMap),
  is_mp_phis(fSolarAzimutAngleMap),
  is_mp_tetav(fViewZenithAngleMap),
  is_mp_phiv(fViewAzimutAngleMap),
  s_taup550(staup550),
  s_uh2o(suh2o),
  s_uo3(suo3),
  s_pression(spression),
  s_tetas(stetas),
  s_phis(sphis),
  s_tetav(stetav),
  s_phiv(sphiv),
  is_mp_cloudmask(false)
{
	DomainValueRangeStruct dv(-1000,1000,0.00001);
	SetDomainValueRangeStruct(dv);
	if (is_mp_taup550){
		mp_taup550 = Map(staup550, fn.sPath());
		CompitableGeorefs(fn, mp, mp_taup550);
		objdep.Add(mp_taup550);
	}
	if (is_mp_uh2o){
		mp_uh2o = Map(suh2o, fn.sPath());
		CompitableGeorefs(fn, mp, mp_uh2o);
		objdep.Add(mp_uh2o);
	}
	if (is_mp_uo3){
		mp_uo3 = Map(suo3, fn.sPath());
		CompitableGeorefs(fn, mp, mp_uo3);
		objdep.Add(mp_uo3);
	}
	if (is_mp_pression){
		mp_pression = Map(spression, fn.sPath());
		CompitableGeorefs(fn, mp, mp_pression);
		objdep.Add(mp_pression);
	}

	if (is_mp_tetas){
		mp_tetas = Map(stetas, fn.sPath());
		CompitableGeorefs(fn, mp, mp_tetas);
		objdep.Add(mp_tetas);
	}
	if (is_mp_phis){
		mp_phis = Map(sphis, fn.sPath());
		CompitableGeorefs(fn, mp, mp_phis);
		objdep.Add(mp_phis);
	}
	if (is_mp_tetav){
		mp_tetav = Map(stetav, fn.sPath());
		CompitableGeorefs(fn, mp, mp_tetav);
		objdep.Add(mp_tetav);
	}
	if (is_mp_phiv){
		mp_phiv = Map(sphiv, fn.sPath());
		CompitableGeorefs(fn, mp, mp_phiv);
		objdep.Add(mp_phiv);
	}
	
    if (!fnObj.fValid())
     objtime = objdep.tmNewest();
	ptr.Store(); 
	fNeedFreeze = true;
	sFreezeTitle = TR("Atmospheric Effect Correction (SMAC)");
	htpFreeze = "ilwisapp\\smac_algorithm.htm";
}

MapSMAC::~MapSMAC()
{

}

const char* MapSMAC::sSyntax() {
  
  return "MapSMAC(MapReflectance,CoefficientFile,\n"
			"true|false,MapOpticalDepth|Value, \n"
			"true|false,MapWaterVapor|Value, \n"
			"true|false,MapOzoneContent|Value, \n"
			"true|false,MapSurfacePressure|Value, \n"
			"true|false,MapSolarZenithAngle|Value, \n"
			"true|false,MapSolarAzimutAngle|Value, \n"
			"true|false,MapViewZenithAngle|Value \n";
			
}

MapSMAC* MapSMAC::create(const FileName& fn, MapPtr& p, const String& sExpr)
{
	Array<String> as;
	int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
	if (iParms < 17 )
      ExpressionError(sExpr, sSyntax());

  	Map mp(as[0], fn.sPath());
	String coef_file = as[1].sVal();

	bool ftaup550 = as[2].fVal();
	String staup550 = as[3].sVal();
	bool fuh2o = as[4].fVal();
	String suh2o = as[5].sVal();
	bool fuo3 = as[6].fVal();
	String suo3 = as[7].sVal();
	bool fpression = as[8].fVal();
	String spression = as[9].sVal();
	bool ftetas = as[10].fVal();
	String stetas = as[11].sVal();
	bool fphis = as[12].fVal();
	String sphis = as[13].sVal();
	bool ftetav = as[14].fVal();
	String stetav = as[15].sVal();
	bool fphiv = as[16].fVal(); 
	String sphiv = as[17].sVal(); 

	if (iParms == 19 ){
		bool fcloudmask = as[18].fVal(); 
		String scloudmask = as[19].sVal();
		return new MapSMAC(fn, p, mp, coef_file,
					ftaup550, staup550,
					fuh2o, suh2o,
					fuo3, suo3,
					fpression, spression,
					ftetas, stetas,
					fphis, sphis,
					ftetav, stetav,
					fphiv, sphiv,
					fcloudmask, scloudmask);
	}
	else
		return new MapSMAC(fn, p, mp, coef_file,
					ftaup550, staup550,
					fuh2o, suh2o,
					fuo3, suo3,
					fpression, spression,
					ftetas, stetas,
					fphis, sphis,
					ftetav, stetav,
					fphiv, sphiv);
}

void MapSMAC::Store()
{
	MapFromMap::Store();
	WriteElement("MapFromMap", "Type", "MAPSMAC");
	WriteElement("MapSMAC", "Reflectance", mp_r_toa);
	WriteElement("MapSMAC", "CoefficientFile", m_coef_filename);

	WriteElement("MapSMAC", "useOpticalDepth550Map", is_mp_taup550);
	WriteElement("MapSMAC", "useWaterVaporMap", is_mp_uh2o);
	WriteElement("MapSMAC", "useOzoneContentMap", is_mp_uo3);
	WriteElement("MapSMAC", "useSurfacePressure", is_mp_pression);
	WriteElement("MapSMAC", "useSolarZenithAngleMap", is_mp_tetas);
	WriteElement("MapSMAC", "useSolarAzimutAngleMap", is_mp_phis);
	WriteElement("MapSMAC", "useViewZenithAngleMap", is_mp_tetav);
	WriteElement("MapSMAC", "useViewAzimutAngleMap", is_mp_phiv);

	WriteElement("MapSMAC", "OpticalDepth550", s_taup550);
	WriteElement("MapSMAC", "WaterVapor", s_uh2o);
	WriteElement("MapSMAC", "OzoneContent", s_uo3);
	WriteElement("MapSMAC", "SurfacePressure", s_pression);
	WriteElement("MapSMAC", "SolarZenithAngle", s_tetas);
	WriteElement("MapSMAC", "SolarAzimutAngle", s_phis);
	WriteElement("MapSMAC", "ViewZenithAngle", s_tetav);
	WriteElement("MapSMAC", "ViewAzimutAngle", s_phiv);
	WriteElement("MapSMAC", "useCloudMaskMap", is_mp_cloudmask);
	WriteElement("MapSMAC", "CloudMask", s_cloudmask);
}

String MapSMAC::sExpression() const
{
    String sExp("MapSMAC(%S,%S,%li,%S,%li,%S,%li,%S,%li,%S,%li,%S,%li,%S,%li,%S,%li,%S)", 
				mp->sNameQuoted(false, fnObj.sPath()), 
				m_coef_filename,
				is_mp_taup550, s_taup550,
				is_mp_uh2o, s_uh2o,
				is_mp_uo3, s_uo3,
				is_mp_pression, s_pression,
				is_mp_tetas, s_tetas,
				is_mp_phis, s_phis,
				is_mp_tetav, s_tetav,
				is_mp_phiv, s_phiv);
  return sExp;
}

bool MapSMAC::fDomainChangeable() const
{
  return false;
}

bool MapSMAC::fGeoRefChangeable() const
{
  return false;
}

bool MapSMAC::fFreezing()
{
	is_mp_cloudmask = false;
	
	double c3,c4;
	File coef_file(m_coef_filename);  
	getCoefs(coef_file,ca.ah2o, ca.nh2o, c3,c4);
	getCoefs(coef_file,ca.ao3,  ca.no3, c3,c4);
	getCoefs(coef_file,ca.ao2,  ca.no2, ca.po2,c4);
	getCoefs(coef_file,ca.aco2, ca.nco2, ca.pco2,c4);
	getCoefs(coef_file,ca.ach4, ca.nch4, ca.pch4,c4);
	getCoefs(coef_file,ca.ano2, ca.nno2, ca.pno2,c4);
	getCoefs(coef_file,ca.aco,  ca.nco, ca.pco,c4);
	getCoefs(coef_file,ca.a0s, ca.a1s,ca.a2s,ca.a3s);
	getCoefs(coef_file,ca.a0T, ca.a1T, ca.a2T, ca.a3T);
	getCoefs(coef_file,ca.taur,ca.sr, c3,c4);
	getCoefs(coef_file,ca.a0taup, ca.a1taup, c3,c4);
	getCoefs(coef_file,ca.wo, ca.gc, c3,c4);
	getCoefs(coef_file,ca.a0P,ca.a1P,ca.a2P,c3);
	getCoefs(coef_file,ca.a3P,ca.a4P, c3, c4);
	getCoefs(coef_file,ca.Rest1,ca.Rest2, c3, c4);
	getCoefs(coef_file,ca.Rest3,ca.Rest4, c3, c4);
	getCoefs(coef_file,ca.Resr1, ca.Resr2, ca.Resr3, c4);
	getCoefs(coef_file,ca.Resa1,ca.Resa2, c3, c4);
	getCoefs(coef_file,ca.Resa3,ca.Resa4, c3, c4);

	RealBuf buf_toa;
	RealBuf buf_tetas;
	RealBuf buf_tetav;
	RealBuf buf_phis;
	RealBuf buf_phiv;
	RealBuf buf_uh2o;
	RealBuf buf_uo3;
	RealBuf buf_taup550;
	RealBuf buf_pression;
	RealBuf buf_surf;
	IntBuf process;
	buf_toa.Size(iCols());
	buf_tetas.Size(iCols());
	buf_tetav.Size(iCols());
	buf_phis.Size(iCols());
    buf_phiv.Size(iCols());
	buf_uh2o.Size(iCols());
	buf_uo3.Size(iCols());
	buf_taup550.Size(iCols());
	buf_pression.Size(iCols());
	buf_surf.Size(iCols());
	process.Size(iCols());
	double tetas,tetav, phis, phiv, uh2o, uo3, taup550,pression, r_toa;

	if (!is_mp_uh2o) uh2o = s_uh2o.rVal();
	if (!is_mp_uo3) uo3 = s_uo3.rVal();
	if (!is_mp_taup550) taup550 = s_taup550.rVal();
	if (!is_mp_pression) pression = s_pression.rVal();
	if (!is_mp_tetas) tetas = s_tetas.rVal();
	if (!is_mp_phis) phis = s_phis.rVal();
	if (!is_mp_tetav) tetav = s_tetav.rVal();
	if (!is_mp_phiv) phiv = s_phiv.rVal();


	double invalidvalue = 0.f;
	trq.SetText(TR("Atmospheric Effect Correction (SMAC)"));
	for (int iRow = 0; iRow < iLines(); ++iRow)
	{
		mp_r_toa->GetLineVal(iRow, buf_toa);
		if (is_mp_tetas)
			mp_tetas->GetLineVal(iRow, buf_tetas);
	    if (is_mp_tetav)
			mp_tetav->GetLineVal(iRow, buf_tetav);
		if (is_mp_phis)
			mp_phis->GetLineVal(iRow, buf_phis);
		if (is_mp_phiv)
			mp_phiv->GetLineVal(iRow, buf_phiv);
		if (is_mp_uh2o)
			mp_uh2o->GetLineVal(iRow, buf_uh2o);
		if (is_mp_uo3)
			mp_uo3->GetLineVal(iRow, buf_uo3);
		if (is_mp_taup550)
			mp_taup550->GetLineVal(iRow, buf_taup550);
		if (is_mp_pression)
			mp_pression->GetLineVal(iRow, buf_pression);
		if (is_mp_cloudmask)
			mp_cloudmask->GetLineRaw(iRow, process);
		
		for (int iCol = 0; iCol < iCols(); ++iCol)
		{
		  if (buf_toa[iCol] != rUNDEF){
			r_toa = buf_toa[iCol];  
			if (is_mp_tetas)
				tetas = buf_tetas[iCol];
			if (is_mp_tetav)
				tetav = buf_tetav[iCol];
			if (is_mp_phis)
				phis = buf_phis[iCol];
			if (is_mp_phiv)
				phiv = buf_phiv[iCol];
			if (is_mp_uh2o)
				uh2o = buf_uh2o[iCol];
			if (is_mp_uo3)
				uo3 = buf_uo3[iCol];
			if (is_mp_taup550)
				taup550 = buf_taup550[iCol];
			if (is_mp_pression)
				pression = buf_pression[iCol];
			//invalid pixels and process the next
			if(is_mp_cloudmask)
				if(process[iCol]==1)
					buf_surf[iCol] = applySMAC(tetas,tetav,phis,phiv,uh2o,uo3,taup550,pression,r_toa);
				else
					buf_surf[iCol] = invalidvalue;
			else
				buf_surf[iCol] = applySMAC(tetas,tetav,phis,phiv,uh2o,uo3,taup550,pression,r_toa);
          }
		  else
			  buf_surf[iCol] = rUNDEF;
		}
		// write the result
		ptr.PutLineVal(iRow, buf_surf);
		trq.fUpdate(iRow, iLines());
	}	
	trq.fUpdate(iLines(), iLines());
	return true;
}

void MapSMAC::getCoefs(File& coef_file, double& c1, double& c2, double& c3, double& c4)
{
	String line;
	Array<String> as;
	coef_file.ReadLnAscii(line);
	Split(line, as);
	if (as.size() == 2 ){
		 c1 = as[0].rVal();	
		 c2 = as[1].rVal();	
	}
	else if (as.size() == 3 ){
		c1 = as[0].rVal();	
		c2 = as[1].rVal();	
		c3 = as[2].rVal();	
	}
	else if (as.size() == 4 ){
		c1 = as[0].rVal();	
		c2 = as[1].rVal();	
		c3 = as[2].rVal();	
		c4 = as[3].rVal();	
	}
}

double MapSMAC::applySMAC(double tetas, double tetav, double phis, double phiv, double uh2o, double uo3, double taup550, double pression, double r_toa)
{
			double cksi;
			double s;
			double m;
			double tg;
			double us,uv,dphi;
			
			double crd = 180 / M_PI;
			double cdr = M_PI / 180;

			double to3,th2o,to2, tco2;
			double tco, tno2,tch4;
			double ttetas,ttetav,ksiD;
			double atm_ref;

			double ak2, ak, e, f, dp, d, b, del, ww, ss, q1, q2, q3, c1, c2, cp1 ;
			double cp2, z, x, y, aa1, aa2, aa3 ;

			double uo2, uco2, uch4, uco, uno2  ;
			double taup,tautot,taurz;
			double Peq ;
			double Res_ray, Res_aer, Res_6s;
			double ray_phase, ray_ref, aer_ref, aer_phase ;
			
			us = cos(tetas*cdr); //saa
			uv = cos(tetav*cdr); //sza
			dphi = (phis-phiv)*cdr;
			Peq = pression/1013.0;
			
			/*--1) air mass */
			m = 1/us+1/uv;

			/*------  2) aerosol optical depth in the spectral band, taup  */
			taup = (ca.a0taup)+(ca.a1taup)*taup550;

			/*------  3) gaseous transmissions (downward and upward paths)*/
			to3 = 1;
			th2o = 1;
			to2 = 1;
			tco2 = 1;
			tch4 = 1;
			tno2 = 0;
			tco = 0;
			
			uo2= pow(Peq,ca.po2);
			uco2= pow(Peq,ca.pco2);
			uch4= pow(Peq,ca.pch4);
			uno2= pow(Peq,ca.pno2);
			uco = pow(Peq,ca.pco);

			/*------  4) if uh2o <= 0 and uo3 <= 0 no gaseous absorption is computed*/
			if( (uh2o > 0) || ( uo3 > 0) )
			{
					to3   = exp((ca.ao3)*pow((uo3 * m),(ca.no3)));
					th2o  = exp((ca.ah2o)*pow((uh2o*m),(ca.nh2o)));
					to2   = exp((ca.ao2)*pow((uo2*m),(ca.no2))) ;
					tco2  = exp((ca.aco2)*pow((uco2*m),(ca.nco2)));
					tch4  = exp((ca.ach4)*pow((uch4*m),(ca.nch4)));
					tno2  = exp((ca.ano2)*pow((uno2*m),(ca.nno2)));
					tco   = exp((ca.aco)*pow((uco*m),(ca.nco)));
			}
			 
			/*------  5) Total scattering transmission */
			ttetas = (ca.a0T)+(ca.a1T)*taup550/us+((ca.a2T)*Peq+(ca.a3T))/(1+us); /* downward */
			ttetav = (ca.a0T)+(ca.a1T)*taup550/uv+((ca.a2T)*Peq+(ca.a3T))/(1+uv); /* upward   */

			/*------  6) spherical albedo of the atmosphere */
			s = (ca.a0s)*Peq+(ca.a3s)+(ca.a1s)*taup550+(ca.a2s)*pow(taup550,2);

			/*------  7) scattering angle cosine */
			cksi = -((us*uv)+(sqrt(1-us*us)*sqrt(1-uv*uv)*cos(dphi)));
			if (cksi < -1 ) 
				cksi=-1.0 ;

			/*------  8) scattering angle in degree */
			ksiD = crd*acos(cksi);

			/*------  9) rayleigh atmospheric reflectance */
			/* pour 6s on a delta = 0.0279 */
			ray_phase = 0.7190443*(1+(cksi*cksi))+0.0412742;

			taurz=(ca.taur)*Peq;

			ray_ref = (taurz*ray_phase)/(4*us*uv);

			/*-----------------Residu Rayleigh ---------*/
			Res_ray = (ca.Resr1)+(ca.Resr2)*taurz*ray_phase/(us*uv)+ 
					(ca.Resr3)*pow((taurz*ray_phase/(us*uv)),2); 

			/*------  10) aerosol atmospheric reflectance */
			aer_phase = (ca.a0P)+(ca.a1P)*ksiD+(ca.a2P)*ksiD*ksiD+
						(ca.a3P)*pow(ksiD,3)+(ca.a4P)*pow(ksiD,4); 

			ak2 = (1-(ca.wo))*(3-(ca.wo)*3*(ca.gc));
			ak  = sqrt(ak2);
			e   = -3*us*us*(ca.wo)/(4*(1-ak2*us*us));
			f   = -(1-(ca.wo))*3*(ca.gc)*us*us*(ca.wo)/(4*(1-ak2*us*us));
			dp  = e/(3*us)+us*f;
			d   = e+f;
			b   = 2*ak/(3-(ca.wo)*3*(ca.gc));
			del = exp(ak*taup)*(1+b)*(1+b)-exp(-ak*taup)*(1-b)*(1-b) ;
			ww  = (ca.wo)/4;
			ss  = us/(1- ak2*us*us);
			q1  = 2+3*us+(1-(ca.wo))*3*(ca.gc)*us*(1+ 2*us);
			q2  = 2-3*us-(1-(ca.wo))*3*(ca.gc)*us*(1 - 2*us);
			q3  = q2*exp(-taup/us);
			c1  = ((ww*ss)/del)*(q1*exp(ak*taup)*(1+b)+q3*(1-b));
			c2  = -((ww*ss)/del)*(q1*exp(-ak*taup)*(1-b)+q3*(1+b));
			cp1 = c1*ak/(3-(ca.wo)*3*(ca.gc));
			cp2 = -c2*ak/(3-(ca.wo)*3*(ca.gc));
			z   = d-(ca.wo)*3*(ca.gc)*uv*dp+(ca.wo)*aer_phase/4;
			x   = c1-(ca.wo)*3*(ca.gc)*uv*cp1;
			y   = c2-(ca.wo)*3*(ca.gc)*uv*cp2;
			aa1 = uv/(1+ak*uv);
			aa2 = uv/(1-ak*uv);
			aa3 = us*uv/(us+uv);

			aer_ref = x*aa1*(1-exp(-taup/aa1));
			aer_ref = aer_ref+y*aa2*(1-exp(-taup/aa2));
			aer_ref = aer_ref+z*aa3*(1-exp(-taup/aa3));
			aer_ref = aer_ref/(us*uv);

			/*--------Residu Aerosol --------*/
			Res_aer= ((ca.Resa1)+(ca.Resa2)*(taup*m*cksi)+(ca.Resa3)*
					 pow((taup*m*cksi),2))+(ca.Resa4)*pow((taup*m*cksi),3);

			/*---------Residu 6s-----------*/
			tautot=taup+taurz;
			Res_6s= ((ca.Rest1)+(ca.Rest2)*(tautot*m*cksi) 
					+(ca.Rest3)*pow((tautot*m*cksi),2))
					+(ca.Rest4)*pow((tautot*m*cksi),3);

			/*------  11) total atmospheric reflectance */
			atm_ref = ray_ref - Res_ray + aer_ref - Res_aer + Res_6s;

			/*-------- reflectance at toa*/
			tg = th2o * to3 * to2 * tco2 * tch4* tco * tno2 ;

			/* reflectance at surface */
			double r_surf = r_toa-(atm_ref*tg);
			return r_surf = r_surf/((tg*ttetas*ttetav)+(r_surf*s));
}

void MapSMAC::CompitableGeorefs(FileName fn, Map mp1, Map mp2)
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