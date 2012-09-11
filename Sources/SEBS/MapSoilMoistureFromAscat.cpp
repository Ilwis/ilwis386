/***************************************************************
 ILWIS integrates image, vector and thematic data in one unique 
 and powerful package on the desktop. ILWIS delivers a wide 
 range of feautures including import/export, digitizing, editing, 
 analysis and display of data as well as production of 
 quality mapsinformation about the sensor mounting platform
 
 Exclusive rights of use by 52°North Initiative for Geospatial 
 Open Source Software GmbH 2007, Germany

 Copyright (C) 2007 by 52°North Initiative for Geospatiallc

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

#include "SEBS\MapSoilMoistureFromAscat.h"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Err\Ilwisapp.err"
#include "Applications\MapList\MapListApplic.h"
#include "Headers\Hs\map.hs"
#include "SEBS\ascat.h"

using namespace std;

IlwisObjectPtr * createMapSoilMoistureFromAscat(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)MapSoilMoistureFromAscat::create(fn, (MapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new MapSoilMoistureFromAscat(fn, (MapPtr &)ptr);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
MapSoilMoistureFromAscat::MapSoilMoistureFromAscat(const FileName& fn, MapPtr& p)
: MapFromMap(fn, p),
m_fnObject(fn)
{
	ReadElement("MapSMASCAT", "SandMap", mp);
	ReadElement("MapSMASCAT", "ClayMap", m_mpClay);
	ReadElement("MapSMASCAT", "VegetationCoverMap", m_mpFc);
	ReadElement("MapSMASCAT", "LandCoverMap", m_mpLc);
	//ReadElement("MapSMASCAT", "DEMMap", m_mpDEM);
	ReadElement("MapSMASCAT", "AscatFilePath", m_ascat_path);
	CompitableGeorefs(fn, mp, m_mpClay);
	CompitableGeorefs(fn, mp, m_mpFc);
	CompitableGeorefs(fn, mp, m_mpLc);
	//CompitableGeorefs(fn, mp, m_mpDEM);
	fNeedFreeze = true;
	sFreezeTitle = "Estimation of Soil Moisture from ASCAT";
}

MapSoilMoistureFromAscat::MapSoilMoistureFromAscat(const FileName& fn, 
				MapPtr& p,
				const Map& sand_mp,
				const Map& clay_mp,
				const Map& fc_mp,
				const Map& lc_mp,
				String ascat_file
				)	
: MapFromMap(fn, p, sand_mp),
m_fnObject(fn),
m_mpClay(clay_mp),
m_mpFc(fc_mp),
m_mpLc(lc_mp),
m_ascat_path(ascat_file)
{
	DomainValueRangeStruct dv(-100,100,0.001);
	SetDomainValueRangeStruct(dv);
	CompitableGeorefs(fn, mp, m_mpClay);
	CompitableGeorefs(fn, mp, m_mpFc);
	CompitableGeorefs(fn, mp, m_mpLc);
	//CompitableGeorefs(fn, mp, m_mpDEM);
	objdep.Add(m_mpClay);
	objdep.Add(m_mpFc);
	objdep.Add(m_mpLc);
	//objdep.Add(m_mpDEM);
	if (!fnObj.fValid())
     objtime = objdep.tmNewest();
	ptr.Store(); 
	fNeedFreeze = true;
	sFreezeTitle = "Estimation of Soil Moisture from ASCAT";
}

MapSoilMoistureFromAscat::~MapSoilMoistureFromAscat()
{

}

const char* MapSoilMoistureFromAscat::sSyntax() {
  
	return "MapSoilMoistureFromAscat(MapSand,MapClay,MapFc,MapLc,AscatFilePath)";
}

MapSoilMoistureFromAscat* MapSoilMoistureFromAscat::create(const FileName& fn, MapPtr& p, const String& sExpr)
{
	Array<String> as;
	int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
	if (iParms < 5 )
      ExpressionError(sExpr, sSyntax());

  	Map mpSand(as[0], fn.sPath());
	Map mpClay(as[1], fn.sPath());
	Map mpFc(as[2], fn.sPath());
	Map mpLc(as[3], fn.sPath());
	//Map mpDEM(as[4], fn.sPath());
	String ascat_file = as[4].sVal();
	return new MapSoilMoistureFromAscat(fn, p, mpSand, mpClay,mpFc,mpLc,ascat_file);
}

void MapSoilMoistureFromAscat::Store()
{
	MapFromMap::Store();
	WriteElement("MapFromMap", "Type", "MapSoilMoistureFromAscat");
	WriteElement("MapSMASCAT", "SandMap", mp);
	WriteElement("MapSMASCAT", "ClayMap", m_mpClay);
	WriteElement("MapSMASCAT", "VegetationCoverMap", m_mpFc);
	WriteElement("MapSMASCAT", "LandCoverMap", m_mpLc);
	//WriteElement("MapSMASCAT", "DEMMap", m_mpDEM);
	WriteElement("MapSMASCAT", "AscatFilePath", m_ascat_path);
}

String MapSoilMoistureFromAscat::sExpression() const
{
	return String("MapSoilMoistureFromAscat(%S,%S,%S,%S,%S)", 
				mp->sNameQuoted(false, fnObj.sPath()),
				m_mpClay->sNameQuoted(true),
				m_mpFc->sNameQuoted(true),
				m_mpLc->sNameQuoted(true),
				m_ascat_path
				);
}

bool MapSoilMoistureFromAscat::fDomainChangeable() const
{
  return false;
}

bool MapSoilMoistureFromAscat::fGeoRefChangeable() const
{
  return false;
}

///////////////////////////////////
// some useful functions
///////////////////////////////////

double lin2db( double x )
{
 if( x <= 0.0 ) return -99.0;
 return 10.0*log10( x );
}

double db2lin( double x )
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

/*GeoRef MapSoilMoistureFromAscat::grf(CoordSystem& csy, RowCol rc)
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


/*void MapSoilMoistureFromAscat::GetTiePoints(long iNrTiePoints, ModelTiePoint* amtp)
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
		amtp[i].rX   = latlon[i*2+1]*pow(10,-4);
		amtp[i].rY   = latlon[i*2]*pow(10,-4);
	}
}*/
FileName MapSoilMoistureFromAscat::fnObjectName(const String& sExt, String band, int iBand)
{
	FileName fnLoc = m_fnObject;
	fnLoc.sExt = sExt;
	String sName = String("%S_%S%ld", fnLoc.sFile, band, iBand);
	fnLoc.sFile = sName;
	return fnLoc;
	//return FileName::fnUnique(fnLoc);
}

void MapSoilMoistureFromAscat::fillAdjcentCells(long row, long col, int lines, int cols)
{
				
	if (col>0){
		m_vS1[row][col-1] =  m_vS1[row][col];
		m_vS2[row][col-1] =  m_vS2[row][col];
		m_vS3[row][col-1] =  m_vS3[row][col];
		m_vT1[row][col-1] =  m_vT1[row][col];
		m_vT2[row][col-1] =  m_vT2[row][col];
		m_vT3[row][col-1] =  m_vT3[row][col];
	}
	if (col < cols-1){
		m_vS1[row][col+1] =  m_vS1[row][col];
		m_vS2[row][col+1] =  m_vS2[row][col];
		m_vS3[row][col+1] =  m_vS3[row][col];
		m_vT1[row][col+1] =  m_vT1[row][col];
		m_vT2[row][col+1] =  m_vT2[row][col];
		m_vT3[row][col+1] =  m_vT3[row][col];
	}
	if (row>0){
		m_vS1[row-1][col] =  m_vS1[row][col];
		m_vS2[row-1][col] =  m_vS2[row][col];
		m_vS3[row-1][col] =  m_vS3[row][col];
		m_vT1[row-1][col] =  m_vT1[row][col];
		m_vT2[row-1][col] =  m_vT2[row][col];
		m_vT3[row-1][col] =  m_vT3[row][col];
	}
	if (row<lines-1){
		m_vS1[row+1][col] =  m_vS1[row][col];
		m_vS2[row+1][col] =  m_vS2[row][col];
		m_vS3[row+1][col] =  m_vS3[row][col];
		m_vT1[row+1][col] =  m_vT1[row][col];
		m_vT2[row+1][col] =  m_vT2[row][col];
		m_vT3[row+1][col] =  m_vT3[row][col];
	}
}

double MapSoilMoistureFromAscat::calculate(double sigma0, double sigma1, double sigma2, 
										   double sita0, double sita1, double sita2,
										   double sand_f,double clay_f, double LAI, 
										   double Fc,double Fcs,double Fcc,double Fcf,int lc,long row,long col)
{
	//if(row==78 && col==92)
	//	row=78;
	double pi = 3.1415926;
	//converting the incidence angles into radiance
	sita0=sita0*pi/180.0;
	sita1=sita1*pi/180.0;	
	sita2=sita2*pi/180.0;
	sigma0 = db2lin( sigma0 );
	sigma1 = db2lin( sigma1 );
	sigma2 = db2lin( sigma2 );

	double v0,v1,v2; //vegetation volumetric contribution 
	double T0, T1, T2; // canopy transmittance  
	
	//r - Fresnel reflectance r^2
	//s - roughness 2s^2
	//Fc - vegetation cover
	int iterations = 0;	
	double ss0,ss1,ss2,ss0_2,dss0; //soil moisture contribution
	double s; // roughness 2s^2
	double r; // Fresnel reflectance r^2
    
	//Define some constants
	double Wv,tal; //the single scattering albedo and optical thickness
	//T = e^(-0.5*lai/cos(sita)
	//Fc(sita) = Fc/cos(sita)
	//sigma(sita) = [1-fc(sita)(1-T(sita)^2)]*Ss(sita)+fc(sita)(S_interaction(sita)+Sv(sita)
	//lai=-2*log(1-fc)/log(2.71828)
	//double Fc=(1-exp(-0.5*LAI));
	double sr2;

	//case 4
	//calculating the cover fraction of bare soil, low vegetation and forest respectively
	//using the land cover and LAI information
	double KcKl=5.255/1.4; //Kc/Kl=fc/fl=5.255/1.4
	double Wvc,Wvf;
	if(lc==4){
		Wv=0;
		tal=0;
	}
	else if(lc>=10 && lc<=33){  //forest 
		Wv=0.085*Fc/KcKl;
		tal=0.384*LAI*KcKl;
	}
	else{ //if(lc>=34 && lc<=125){ //vegetation
		Wv=0.05*Fc/KcKl;
		tal=0.384*LAI*KcKl;
	}
		
	/*double tal_vc=0.07*LAI*KcKl;
	double tal_vf=0.384*LAI*KcKl;
	double Tvc1=pow(exp(-tal_vc/cos(sita1)),2);
	double Tvc2=pow(exp(-tal_vc/cos(sita2)),2);
	double Tvf1=pow(exp(-tal_vf/cos(sita1)),2);
	double Tvf2=pow(exp(-tal_vf/cos(sita2)),2);
	double sgm_vc1=0.5*Wvc*cos(sita1)*(1-Tvc1);
	double sgm_vc2=0.5*Wvc*cos(sita2)*(1-Tvc2);
	double sgm_vf1=0.5*Wvf*cos(sita1)*(1-Tvf1);
	double sgm_vf2=0.5*Wvf*cos(sita2)*(1-Tvf2);
	
	double a1=Fcs/cos(sita1)+(Fcc/cos(sita1))*Tvc1+(Fcf/cos(sita1))*Tvf1;
	double a2=Fcs/cos(sita2)+(Fcc/cos(sita2))*Tvc2+(Fcf/cos(sita1))*Tvf2;
	double b1=(Fcc/cos(sita1))*sgm_vc1+(Fcf/cos(sita1))*sgm_vf1;
	double b2=(Fcc/cos(sita2))*sgm_vc2+(Fcf/cos(sita2))*sgm_vf2;
	ss1=(sigma1-b1)/(a1);
	ss2=(sigma2-b2)/(a2);
	sr2 = abs((pow(tan(sita1),2)-pow(tan(sita2),2))/(log(ss2*pow(cos(sita2),4)/(ss1*pow(cos(sita1),4))))/log(2.71828));
	Wv=Wvc;
	tal=tal_vc;*/// end of case 4 
	
	//Case 2:assuming single scattering albedo Wv=0, and Tal=0.07*LAI
	//Wv=0;
	//tal=0.07*LAI;
	

	//Case 3:assuming single scattering albedo Wv=0.05*Fc, and Tal=0.07*LAI
	/*double cons=5.255/1.4;
	Wv=0.05*Fc; //0.05*Fc;
	tal=0.07*LAI*cons;*/

	
	T0 = exp(-2*tal/cos(sita0));
	T1 = exp(-2*tal/cos(sita1));
	T2 = exp(-2*tal/cos(sita2));
	v0 = 0.5*Wv*cos(sita0)*(1-exp(-2*tal/cos(sita0)));
	v1 = 0.5*Wv*cos(sita1)*(1-exp(-2*tal/cos(sita1)));
	v2 = 0.5*Wv*cos(sita2)*(1-exp(-2*tal/cos(sita2)));

	ss0=(sigma0-Fc*v0)/(1-Fc*(1-T0));
	ss1=(sigma1-Fc*v1)/(1-Fc*(1-T1));
	ss2=(sigma2-Fc*v2)/(1-Fc*(1-T2));
	//ss0=(sigma0-(Fc/cos(sita0))*v0)/(1-(Fc/cos(sita0))*(1-T0));
	//ss1=(sigma1-(Fc/cos(sita1))*v1)/(1-(Fc/cos(sita1))*(1-T1));
	//ss2=(sigma2-(Fc/cos(sita2))*v2)/(1-(Fc/cos(sita2))*(1-T2));
		
	double sr0,sr1;
	sr2 = abs((pow(tan(sita1),2)-pow(tan(sita2),2))/(log(ss2*pow(cos(sita2),4)/(ss1*pow(cos(sita1),4))))/log(2.71828));
	sr0 = abs((pow(tan(sita2),2)-pow(tan(sita1),2))/(log(ss1*pow(cos(sita1),4)/(ss2*pow(cos(sita2),4))))/log(2.71828));
	sr1 = abs((pow(tan(sita1),2)-pow(tan(sita0),2))/(log(ss0*pow(cos(sita0),4)/(ss1*pow(cos(sita1),4))))/log(2.71828));
    //end of case 3

	s=sr2;
	//s = 0.568; min
	//s = 0.712; //max
	//s = 0.64; //mean
	r = ss1*s*pow(cos(sita1),4)/exp(-pow(tan(sita1),2)/s);
	if(r>1) 
		r=1;
	double r1 = ss0*s*pow(cos(sita0),4)/exp(-pow(tan(sita0),2)/s);
	double r2 = ss2*s*pow(cos(sita2),4)/exp(-pow(tan(sita2),2)/s);
	
	//calculation of Fresnel reflectance r^2 based on the provided roughness
	/*double r0,r1,r2,sr,sr_2;
	sr = m_vDEM[row][col];
	s = 2*sr*sr;
	r1 = ss1*s*pow(cos(sita1),4)/exp(-pow(tan(sita1),2)/(s));
	r2 = ss2*s*pow(cos(sita2),4)/exp(-pow(tan(sita2),2)/(s));
	r0 = ss0*s*pow(cos(sita0),4)/exp(-pow(tan(sita0),2)/(s));*/
	
	double R0 = sqrt(r);
	
	//ER - real part dieletctric constant
	//Er within 3 (dry soil) - 80 (water)
	//From Dobson paper, it is within 3.1 - 25 for soil
	double	Er = pow((1+sqrt(R0))/(1-sqrt(R0)),2); //Er=3 for dry soil, Er=80 for water
	

	double rob,ros,Es,alfa,beta1;
	//Sand=0.1; //mass fraction of sand anc clay respectively (<=1)
	//Clay=0.4;
	rob = 1.3; //bulk density in g/cm^3 
	ros = 2.66; //specific dentity of the soil in g/cm^3
	alfa=0.65;
	double Mv=0.01; //soil moisture
	if(sand_f == rUNDEF || clay_f==rUNDEF || _isnan(r)  )
		Mv = rUNDEF;
	else{
		beta1=1.2748-0.519*sand_f-0.152*clay_f;
		//beta1 = (127.48 - 0.519 * Sand - 0.152 * Clay) / 100.0;
		double Ew_inf=4.9; //dielectric constant in the high-frequency limit
		double Ew0 = 80.1; //dielectric constant for water
		Es = pow((1.01+0.44*ros),2)-0.062; //dieletctric constant of the soil solids
		double f = 5.255*pow(10.0,9); //frenquency shoulb be provided here in Hz C band
		double talw = 0.58*pow(10.0,-10); //2*PI*talw
		double Efw1=Ew_inf+(Ew0-Ew_inf)/(1+pow(f*talw,2)); //effective conductivity of the soil moisture

		//double Mv=0.01; //soil moisture 
		if (Er < 80){
		  double Er_i= Er+1; 
		  iterations =0;
		  while(abs(Er_i-Er)>0.5 && iterations <99)
		  {
			double a=rob*(pow(Es,alfa)-1);
			double b=pow(Mv,beta1);
			double c=pow(Efw1,alfa);
			double d= 1+rob*(pow(Es,alfa)-1)/ros+pow(Mv,beta1)*pow(Efw1,alfa)-Mv;
			Er_i = pow((1+rob*(pow(Es,alfa)-1)/ros+pow(Mv,beta1)*pow(Efw1,alfa)-Mv),1/alfa);
			Mv=Mv+0.01;
			iterations++;
		  }
		  //if (iterations == 99  Er < Er_i)
		  if (iterations == 99)
			Mv = 0.0;
		}
		else
			Mv = 0;
	}
	m_rBufSr[col]=sqrt(abs(0.5*s));
	m_rBufTal[col]=tal;
	m_rBufF_ref[col]=r;
	m_rBufWv[col]=Wv;
	/*m_rBufSr_1[col]=sr0;
	m_rBufSr_2[col]=sr1;
	m_rBufF_ref1[col]=r1;
	m_rBufF_ref2[col]=r2;*/
	return Mv;
}

bool MapSoilMoistureFromAscat::fFreezing()
{
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
	//IlwWinApp()->SetCurDir(sCurDir);
	
	//obtaining Ascat files from a text file 
	/*FileName fnAscat(m_ascat_path);
	File prmFilAscat(m_ascat_path);
	if (!File::fExist(fnAscat)) {
		NotFoundError(fnAscat);	// throws
	}
	String sDir;
	prmFilAscat.ReadLnAscii(sDir);
	string slast = sDir.substr(sDir.length()-1,1);
	if(slast.compare("\\") == 0)
		sDir = sDir.substr(0, sDir.length()-1);
	Array<String> aFiles;
	// Keep reading while the End Of File character is NOT reached
	String line;
	while(prmFilAscat.ReadLnAscii(line)) {
		if(line.length()>0){
			FileName fnaFile(sDir + "\\" + line);
			if (File::fExist(fnaFile))
				aFiles.push_back(sDir + "\\" + line);
		}
	}*/
	// create the output maps
	DomainValueRangeStruct dv(-50,100,0.001);
	Map mpOut[15];
	Array<FileName> afn;
	for (long iBand = 0; iBand < 3; iBand++) {
		FileName fnLoc = fnObjectName(".mpr", "s", iBand+1);
		//mpOut[iBand] = Map(fnLoc,mp->gr(), mp->gr()->rcSize(),dv);
		afn.push_back(fnLoc);
	}
	
	for (long iBand = 0; iBand < 3; iBand++) {
		FileName fnLoc = fnObjectName(".mpr", "t", iBand+1);
		//mpOut[iBand+3] = Map(fnLoc,mp->gr(), mp->gr()->rcSize(),dv);
		afn.push_back(fnLoc);
	}

	FileName fnRoughness = m_fnObject;
	fnRoughness.sExt = ".mpr";
	String sName = String("%S_Sr", fnRoughness.sFile);
	fnRoughness.sFile = sName;
	DomainValueRangeStruct dv1(0,10,0.001);
	mpOut[6] = Map(fnRoughness,mp->gr(), mp->gr()->rcSize(),dv1);
	afn.push_back(fnRoughness);
	

	FileName fnTal = m_fnObject;
	fnTal.sExt = ".mpr";
	fnTal.sFile = String("%S_Tal", fnTal.sFile);
	DomainValueRangeStruct dv2(0,10,0.01);
	mpOut[7] = Map(fnTal,mp->gr(), mp->gr()->rcSize(),dv2);
	afn.push_back(fnTal);

	FileName fnF_ref = m_fnObject;
	fnF_ref.sExt = ".mpr";
	fnF_ref.sFile = String("%S_F_ref", fnF_ref.sFile);
	//DomainValueRangeStruct dv3(0,1,0.001);
	mpOut[8] = Map(fnF_ref,mp->gr(), mp->gr()->rcSize(),dv1);
	afn.push_back(fnF_ref);

	FileName fnWv = m_fnObject;
	fnWv.sExt = ".mpr";
	fnWv.sFile = String("%S_Wv", fnWv.sFile);
	DomainValueRangeStruct dvWv(0,1,0.001);
	mpOut[9] = Map(fnWv,mp->gr(), mp->gr()->rcSize(),dvWv);
	afn.push_back(fnWv);

	/*FileName fnWvf = m_fnObject;
	fnWvf.sExt = ".mpr";
	fnWvf.sFile = String("%S_Wvf", fnWvf.sFile);
	DomainValueRangeStruct dvSr(0,1,0.001);
	mpOut[10] = Map(fnWvf,mp->gr(), mp->gr()->rcSize(),dvSr);
	afn.push_back(fnWvf);
	
	FileName fnTal_f = m_fnObject;
	fnTal_f.sExt = ".mpr";
	fnTal_f.sFile = String("%S_Tal_f", fnTal_f.sFile);
	mpOut[11] = Map(fnTal_f,mp->gr(), mp->gr()->rcSize(),dvSr);
	afn.push_back(fnTal_f);

	FileName fnRef_1 = m_fnObject;
	fnRef_1.sExt = ".mpr";
	fnRef_1.sFile = String("%S_Ref_1", fnRef_1.sFile);
	mpOut[12] = Map(fnRef_1,mp->gr(), mp->gr()->rcSize(),dvSr);
	afn.push_back(fnRef_1);

	FileName fnRef_2 = m_fnObject;
	fnRef_2.sExt = ".mpr";
	fnRef_2.sFile = String("%S_Ref_2", fnRef_2.sFile);
	mpOut[13] = Map(fnRef_2,mp->gr(), mp->gr()->rcSize(),dvSr);
	afn.push_back(fnRef_2);*/

	afn.push_back(m_fnObject); //soil moisture map

	//calculating the cover fraction of bare soil, low vegetation and forest respectively
	//using the land cover and LAI information
	double Fcs,Fcc,Fcf;
	Fcs=0.0;
	Fcc=0.0;
	Fcf=0.0;
	
	/*long Count_fcs,Count_fcc,Count_fcf;
	Fcs=0.0;
	Fcc=0.0;
	Fcf=0.0;
	Count_fcs=0;
	Count_fcc=0;
	Count_fcf=0;
	RealBuf rBufFc;  //Vegetation Cover input
	rBufFc.Size(iCols());
	m_iBufLc.Size(iCols());
	for (long i = 0; i < iLines(); i++){
		m_mpFc->GetLineVal(i, rBufFc);
		m_mpLc->GetLineRaw(i, m_iBufLc);
		for (long j = 0; j < iCols(); j++){
			int lc=m_iBufLc[j];
			
			double lai = rBufFc[j];
			double Fc;
			if(lai!=rUNDEF)
				Fc=(1-exp(-0.5*lai));
			else
				Fc=rUNDEF;
			if(lc==4){ //bare soil
				if(Fc!=rUNDEF){
					Count_fcs++;
					Fcs+=Fc;
				}
			}
			else if(lc>=10 && lc<=33){ //forest
				if(Fc!=rUNDEF){
					Count_fcf++;
					Fcf+=Fc;
				}
			}
			else if(lc>=34 && lc<=125){ //low vegetation
				if(Fc!=rUNDEF){
					Count_fcc++;
					Fcc+=Fc;
				}
			}
			
		}
	}
	Fcc=Fcc/Count_fcc;
	Fcs=Fcs/Count_fcs;
	Fcf=Fcf/Count_fcf;*/

	int lines, cols;
	lines = iLines();
	cols = iCols();
	//allocate memory for the inputs 
	m_vS1.resize(lines);  
	m_vS2.resize(lines);  
	m_vS3.resize(lines);
	m_vT1.resize(lines);
	m_vT2.resize(lines);
	m_vT3.resize(lines);
	//m_vDEM.resize(lines);
	vector<LongBuf> vCount;
	vCount.resize(lines);
    trq.SetText("Estimation of soil moisture");
	for (int iRow = 0; iRow < iLines(); ++iRow)
	{
		m_vS1[iRow].Size(iCols());
		m_vS2[iRow].Size(iCols());
		m_vS3[iRow].Size(iCols());
		m_vT1[iRow].Size(iCols());
		m_vT2[iRow].Size(iCols());
		m_vT3[iRow].Size(iCols());
		vCount[iRow].Size(iCols());
		for (int iCol = 0; iCol < iCols(); ++iCol){
			vCount[iRow][iCol] = 1;
			m_vS1[iRow][iCol] = rUNDEF;
		}
	}

	/*FileName fnGrf;
	fnGrf.sExt = ".grf";
	fnGrf.sFile = String("grfWorld");
	GeoRef gr = GeoRef(fnGrf);*/
	//RowCol rc = mp->gr()->rcSize(); //

	GeoRef gr = mp->gr();
	RowCol rc = gr->rcSize();	
	double minx = gr->cb().MinX();
	double miny = gr->cb().MinY();
	//double miny = gr->cb().cMin.Y;
	//double maxx = gr->cb().cMax.X;
	//double maxy = gr->cb().cMax.Y;
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
		ascat_file a;
		ascat_node b0;
		ascat_node b1;
		a.open(aFiles[count], &nr,&nn);
		for(int i=0;i<nr;i++)
		{
			//a.read_mdr();
			if( a.read_mdr() ) continue;
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

				//if (lon0 >= gr->cb().cMin.X && lon0 <= gr->cb().cMax.X && 
				//	lat0 >= gr->cb().cMin.Y && lat0 <= gr->cb().cMax.Y && b0.asc ==1)  
				if (lon0 >= gr->cb().MinX() && lon0 <= gr->cb().MaxX() && 
					lat0 >= gr->cb().MinY() && lat0 <= gr->cb().MaxY() && b0.asc ==1)  
				{
					Coord crd0 = Coord(  lon0,lat0);
					RowCol rc0 = gr->rcConv(crd0);
					if((b0.s0>-50) && (b0.s0<100) && b0.s1>-50 && b0.s1<100 && b0.s2>-50 && b0.s2<100 && 
						b0.t0>-50 &&b0.t0<100 && b0.t1>-50 && b0.t1<100 && b0.t2>-50 && b0.t2<100){ 	
					  if(m_vS1[rc0.Row][rc0.Col] == rUNDEF){
						vCount[rc0.Row][rc0.Col]=1;
						m_vS1[rc0.Row][rc0.Col] = b0.s0;
						m_vS2[rc0.Row][rc0.Col] = b0.s1;
						m_vS3[rc0.Row][rc0.Col] = b0.s2;
						m_vT1[rc0.Row][rc0.Col] = b0.t0;
						m_vT2[rc0.Row][rc0.Col] = b0.t1;
						m_vT3[rc0.Row][rc0.Col] = b0.t2;
					  }
					}
					//fillAdjcentCells(rc0.Row,rc0.Col, lines,cols);
				}
				
				//if (lon1 >= gr->cb().cMin.X && lon1 <= gr->cb().cMax.X && 
				//	lat1 >= gr->cb().cMin.Y && lat1 <= gr->cb().cMax.Y && b1.asc==1)  
				if (lon1 >= gr->cb().MinX() && lon1 <= gr->cb().MaxX() && 
					lat1 >= gr->cb().MinY() && lat1 <= gr->cb().MaxY() && b1.asc ==1)  
				{
					Coord crd1 = Coord(lon1,lat1);
					RowCol rc1 = gr->rcConv(crd1);
					if(b1.s0>-50 && b1.s0<100 && b1.s1>-50 && b1.s1<100 && b1.s2>-50 && b1.s2<100 && 
						b1.t0>-50 && b1.t0<100 && b1.t1>-50 && b1.t1<100 && b1.t2>-50 && b1.t2<100){ 	
					
					  if(m_vS1[rc1.Row][rc1.Col] == rUNDEF){
						vCount[rc1.Row][rc1.Col]=1;
						m_vS1[rc1.Row][rc1.Col] = b1.s0;
						m_vS2[rc1.Row][rc1.Col] = b1.s1;
						m_vS3[rc1.Row][rc1.Col] = b1.s2;
						m_vT1[rc1.Row][rc1.Col] = b1.t0;
						m_vT2[rc1.Row][rc1.Col] = b1.t1;
						m_vT3[rc1.Row][rc1.Col] = b1.t2;
					  }
					}
					//fillAdjcentCells(rc1.Row,rc1.Col,lines,cols);
				}
				//if (lon0 >= gr->cb().cMin.X && lon0 <= gr->cb().cMax.X && 
				//	lat0 >= gr->cb().cMin.Y && lat0 <= gr->cb().cMax.Y && b0.asc ==0)  
                if (lon0 >= gr->cb().MinX() && lon0 <= gr->cb().MaxX() && 
					lat0 >= gr->cb().MinY() && lat0 <= gr->cb().MaxY() && b0.asc ==0)  
				{
					Coord crd0 = Coord(lon0,lat0);
					RowCol rc0 = gr->rcConv(crd0);
					if((b0.s0>-50) && (b0.s0<100) && b0.s1>-50 && b0.s1<100 && b0.s2>-50 && b0.s2<100 && 
						b0.t0>-50 &&b0.t0<100 && b0.t1>-50 && b0.t1<100 && b0.t2>-50 && b0.t2<100){ 	
						if(m_vS1[rc0.Row][rc0.Col] == rUNDEF){
						vCount[rc0.Row][rc0.Col]=1;
						m_vS1[rc0.Row][rc0.Col] = b0.s0;
						m_vS2[rc0.Row][rc0.Col] = b0.s1;
						m_vS3[rc0.Row][rc0.Col] = b0.s2;
						m_vT1[rc0.Row][rc0.Col] = b0.t0;
						m_vT2[rc0.Row][rc0.Col] = b0.t1;
						m_vT3[rc0.Row][rc0.Col] = b0.t2;
						}
					}
				}
				//if (lon1 >= gr->cb().cMin.X && lon1 <= gr->cb().cMax.X && 
				//	lat1 >= gr->cb().cMin.Y && lat1 <= gr->cb().cMax.Y && b1.asc==0)  
				if (lon1 >= gr->cb().MinX() && lon1 <= gr->cb().MaxX() && 
					lat1 >= gr->cb().MinY() && lat1 <= gr->cb().MaxY() && b1.asc ==1)  
				{
					Coord crd1 = Coord(lon1,lat1);
					RowCol rc1 = gr->rcConv(crd1);
					if(b1.s0>-50 && b1.s0<100 && b1.s1>-50 && b1.s1<100 && b1.s2>-50 && b1.s2<100 && 
						b1.t0>-50 && b1.t0<100 && b1.t1>-50 && b1.t1<100 && b1.t2>-50 && b1.t2<100){ 	
					
					if(m_vS1[rc1.Row][rc1.Col] == rUNDEF){
						vCount[rc1.Row][rc1.Col]=1;
						m_vS1[rc1.Row][rc1.Col] = b1.s0;
						m_vS2[rc1.Row][rc1.Col] = b1.s1;
						m_vS3[rc1.Row][rc1.Col] = b1.s2;
						m_vT1[rc1.Row][rc1.Col] = b1.t0;
						m_vT2[rc1.Row][rc1.Col] = b1.t1;
						m_vT3[rc1.Row][rc1.Col] = b1.t2;
					}
					}
				}	
			}
			trq.fUpdate(i,nr);
		}
		a.close();
	}
	//***a temporary maps holding the input signals    
	Map mpTmpOut[6];
	Array<FileName> aTmpfn;
	for (long iBand = 0; iBand < 6; iBand++) {
		String sName = String("%S_%ld", String("tmpOutput"), iBand);
		FileName fnTmp = FileName(sName);
		fnTmp.sExt = ".mpr";
		mpTmpOut[iBand] = Map(fnTmp,mp->gr(), mp->gr()->rcSize(),dv);
		mpTmpOut[iBand]->fErase = true;
		aTmpfn.push_back(fnTmp);
	}

	//write the temporary output maps 
	trq.SetText("Create the temporary output maps");
	for (long iRow = 0; iRow < iLines(); ++iRow)
	{
		for (int iCol = 0; iCol < iCols(); ++iCol)
		{
			if (m_vS1[iRow][iCol] != rUNDEF)
				m_vS1[iRow][iCol] = m_vS1[iRow][iCol]/vCount[iRow][iCol]; 
			if (m_vS2[iRow][iCol] != rUNDEF)
				m_vS2[iRow][iCol] = m_vS2[iRow][iCol]/vCount[iRow][iCol];
			if (m_vS3[iRow][iCol] != rUNDEF)
				m_vS3[iRow][iCol] = m_vS3[iRow][iCol]/vCount[iRow][iCol];
			if (m_vT1[iRow][iCol] != rUNDEF)
				m_vT1[iRow][iCol] = m_vT1[iRow][iCol]/vCount[iRow][iCol];
			if (m_vT2[iRow][iCol] != rUNDEF)
				m_vT2[iRow][iCol] = m_vT2[iRow][iCol]/vCount[iRow][iCol];
			if (m_vT3[iRow][iCol] != rUNDEF)
				m_vT3[iRow][iCol] = m_vT3[iRow][iCol]/vCount[iRow][iCol];
		}
		RealBuf& dataBuf = m_vS1[iRow];
		mpTmpOut[0]->PutLineVal(iRow, dataBuf);
		dataBuf = m_vS2[iRow];
		mpTmpOut[1]->PutLineVal(iRow, dataBuf);
		dataBuf = m_vS3[iRow];
		mpTmpOut[2]->PutLineVal(iRow, dataBuf);
		dataBuf = m_vT1[iRow];
		mpTmpOut[3]->PutLineVal(iRow, dataBuf);
		dataBuf = m_vT2[iRow];
		mpTmpOut[4]->PutLineVal(iRow, dataBuf);
		dataBuf = m_vT3[iRow];
		mpTmpOut[5]->PutLineVal(iRow, dataBuf);
		trq.fUpdate(iRow, iLines());
	}
	//Clean up
	m_vS1.resize(0);
	m_vS2.resize(0);
	m_vS3.resize(0);
	m_vT1.resize(0);
	m_vT2.resize(0);
	m_vT3.resize(0);
	vCount.resize(0);

	//Apply majority filter  
	for (int iMap = 0; iMap < 6; iMap++) {
		//MapFilter(output11,UndefMajority(3,3),value)
		String sExprFilterMap("MapFilter(%S, %S, %S)", aTmpfn[iMap].sFullPathQuoted(), String("UndefMajority(3,3)"), String("value")); 
		mpOut[iMap] = Map(afn[iMap], sExprFilterMap);
		mpOut[iMap]->Calc();
    }

	/*trq.SetText(SMAPTextReadingDEM);
	trq.Start();
	for (iRow = 0; iRow < iLines(); ++iRow)
	{
		//reading DEM  
		m_vDEM[iRow].Size(iCols()); 
		m_mpDEM->GetLineVal(iRow, m_vDEM[iRow]);
		if (trq.fUpdate(iRow, iLines())) return false;	
	}
	trq.fUpdate(iLines(), iLines());*/
	
	//write the output maps  
	trq.SetText("Create output maps");
	RealBuf rBufMv; 
	RealBuf rBufSand;  //Sand Fraction input 
	RealBuf rBufClay;  //Clay Fraction input
	RealBuf rBufFc;  //Vegetation Cover input
	RealBuf rBufS1,rBufS2,rBufS3,rBufT1,rBufT2,rBufT3;  
	rBufMv.Size(iCols());
	m_rBufSr.Size(iCols());
	m_rBufTal.Size(iCols());
	m_rBufF_ref.Size(iCols());
	m_rBufWv.Size(iCols());
	/*m_rBufSr_1.Size(iCols());
	m_rBufSr_2.Size(iCols());
	m_rBufF_ref1.Size(iCols());
	m_rBufF_ref2.Size(iCols());*/
	rBufSand.Size(iCols());
	rBufClay.Size(iCols());
	rBufFc.Size(iCols());
	m_iBufLc.Size(iCols());
	rBufS1.Size(iCols());
	rBufS2.Size(iCols());
	rBufS3.Size(iCols());
	rBufT1.Size(iCols());
	rBufT2.Size(iCols());
	rBufT3.Size(iCols());

	for (long i = 0; i < iLines(); i++)
	{
		mp->GetLineVal(i, rBufSand);
		m_mpClay->GetLineVal(i, rBufClay);
		m_mpFc->GetLineVal(i, rBufFc);
		m_mpLc->GetLineRaw(i, m_iBufLc);
		mpOut[0]->GetLineVal(i, rBufS1);
		mpOut[1]->GetLineVal(i, rBufS2);
		mpOut[2]->GetLineVal(i, rBufS3);
		mpOut[3]->GetLineVal(i, rBufT1);
		mpOut[4]->GetLineVal(i, rBufT2);
		mpOut[5]->GetLineVal(i, rBufT3);
		for (long j = 0; j < iCols(); j++)
		{
			double sand_fraction = rBufSand[j];
			double clay_fraction = rBufClay[j];
			double lai = rBufFc[j];
			int lc = m_iBufLc[j];
			double Fc;
			if(lai!=rUNDEF)
				Fc=(1-exp(-0.5*lai));
			else
				Fc=rUNDEF;
			double s1 = rBufS1[j]; 
			double s2 = rBufS2[j]; 
			double s3 = rBufS3[j]; 
			double t1 = rBufT1[j]; 
			double t2 = rBufT2[j]; 
			double t3 = rBufT3[j]; 
			if(Fc == rUNDEF || s1==rUNDEF || s2==rUNDEF || s3==rUNDEF || t1==rUNDEF || t2==rUNDEF || t3== rUNDEF
				|| clay_fraction==rUNDEF || sand_fraction==rUNDEF){
				m_rBufSr[j] = rUNDEF;
				m_rBufTal[j] = rUNDEF;
				m_rBufF_ref[j] = rUNDEF;
				m_rBufWv[j] = rUNDEF;
				/*m_rBufF_ref1[j] = rUNDEF;
				m_rBufF_ref2[j] = rUNDEF;
				m_rBufSr_1[j] = rUNDEF;
				m_rBufSr_2[j] = rUNDEF;*/
				rBufMv[j] = rUNDEF; //m_vMv[i][j] = rUNDEF;
			}
			else{
				rBufMv[j]=calculate(rBufS1[j],rBufS2[j],rBufS3[j],rBufT1[j],rBufT2[j],rBufT3[j],
							rBufSand[j],rBufClay[j],rBufFc[j],Fc,Fcs,Fcc,Fcf,lc,i,j);
			}
		}

		ptr.PutLineVal(i, rBufMv);
		mpOut[6]->PutLineVal(i, m_rBufSr);
		mpOut[7]->PutLineVal(i, m_rBufTal);
		mpOut[8]->PutLineVal(i, m_rBufF_ref);
		mpOut[9]->PutLineVal(i, m_rBufWv);
		/*mpOut[10]->PutLineVal(i, m_rBufSr_1);
		mpOut[11]->PutLineVal(i, m_rBufSr_2);
		mpOut[12]->PutLineVal(i, m_rBufF_ref1);
		mpOut[13]->PutLineVal(i, m_rBufF_ref2);*/
		trq.fUpdate(i, iLines());
	}

	//create map list
	FileName fn = m_fnObject;
	fn.sExt = ".mpl";
	MapList ocL1b(fn, afn);
	trq.fUpdate(iLines(), iLines());
	//m_vDEM.resize(0);
	return true;
}

void MapSoilMoistureFromAscat::CompitableGeorefs(FileName fn, Map mp1, Map mp2)
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
