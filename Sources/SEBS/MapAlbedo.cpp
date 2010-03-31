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

 Created on: 2007-22-11
 ***************************************************************/
// MapAlbedo.cpp: implementation of the MapAlbedo class.
//
//////////////////////////////////////////////////////////////////////
/* SEBS model
   November 2007, by Lichun Wang
*/
#include "SEBS\MapAlbedo.h"
//#include "SEBS\MapSMAC.h"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Hs\map.hs"

using namespace std;

IlwisObjectPtr * createMapAlbedo(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)MapAlbedo::create(fn, (MapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new MapAlbedo(fn, (MapPtr &)ptr);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
MapAlbedo::MapAlbedo(const FileName& fn, MapPtr& p)
: MapFromMap(fn, p)
{
	String sMethod;
    ReadElement("MapAlbedo", "Method", sMethod);
	ReadElement("MapAlbedo", "RedBand", m_mpRedBand);
	ReadElement("MapAlbedo", "NearInfraredBand", m_mpNIR);
	CompitableGeorefs(fn, mp, m_mpNIR);
	if (fCIStrEqual(sMethod, "using2bands")){
	
			m_mMethod = mUsing2Bands;  //method suggested by Valiente et al. (1995)
	}
	else if (fCIStrEqual(sMethod, "ASTER")){
			m_mMethod = mAster;	//method suggested by Liang et al. (2000) for ASTER
			ReadElement("MapAlbedo", "Band5", m_mpBand3);
			ReadElement("MapAlbedo", "Band6", m_mpBand4);
			ReadElement("MapAlbedo", "Band8", m_mpBand5);
			ReadElement("MapAlbedo", "Band9", m_mpBand7);
			CompitableGeorefs(fn, mp, m_mpBand3);
			CompitableGeorefs(fn, mp, m_mpBand4);
			CompitableGeorefs(fn, mp, m_mpBand5);
			CompitableGeorefs(fn, mp, m_mpBand7);
	}
	else{
			m_mMethod = mModis;	//method suggested by Liang et al. (2000) for MODIS
			ReadElement("MapAlbedo", "Band3", m_mpBand3);
			ReadElement("MapAlbedo", "Band4", m_mpBand4);
			ReadElement("MapAlbedo", "Band5", m_mpBand5);
			ReadElement("MapAlbedo", "Band7", m_mpBand7);
			CompitableGeorefs(fn, mp, m_mpBand3);
			CompitableGeorefs(fn, mp, m_mpBand4);
			CompitableGeorefs(fn, mp, m_mpBand5);
			CompitableGeorefs(fn, mp, m_mpBand7);
		}
	fNeedFreeze = true;
	sFreezeTitle = SMAPTextCalculateAlbedo;
}

MapAlbedo::MapAlbedo(const FileName& fn, 
				 MapPtr& p,
				 const Map& mpRed,
				 const Map& mpNIR,
				 Method md
				 )
: MapFromMap(fn, p, mpRed),
	m_mpRedBand(mpRed),
	m_mpNIR(mpNIR),
	m_mMethod(md)
{
	//Verify compatible geo-reference
	CompitableGeorefs(fn, mp, m_mpNIR);
	DomainValueRangeStruct dv(-2,2,0.001);
	SetDomainValueRangeStruct(dv);
	//init();
	objdep.Add(m_mpRedBand);
	objdep.Add(m_mpNIR);
	if (!fnObj.fValid())
     objtime = objdep.tmNewest();
	ptr.Store(); 
	fNeedFreeze = true;
	sFreezeTitle = SMAPTextCalculateAlbedo;
}

//Modis or Aster
MapAlbedo::MapAlbedo(const FileName& fn, 
				 MapPtr& p,
				 const Map& mpRed,
				 const Map& mpNIR,
				 Method md,
				 const Map& mpBand3,
				 const Map& mpBand4,
				 const Map& mpBand5,
				 const Map& mpBand7
				 )
: MapFromMap(fn, p, mpRed),
	m_mpRedBand(mpRed),
	m_mpNIR(mpNIR),
	m_mMethod(md),
	m_mpBand3(mpBand3),
	m_mpBand4(mpBand4),
	m_mpBand5(mpBand5),
	m_mpBand7(mpBand7)

{
	CompitableGeorefs(fn, mp, m_mpNIR);
	CompitableGeorefs(fn, mp, m_mpBand3);
	CompitableGeorefs(fn, mp, m_mpBand4);
	CompitableGeorefs(fn, mp, m_mpBand5);
	CompitableGeorefs(fn, mp, m_mpBand7);
	DomainValueRangeStruct dv(-2,2,0.001);
	SetDomainValueRangeStruct(dv);
	//init();
	objdep.Add(m_mpRedBand);
	objdep.Add(m_mpNIR);
	objdep.Add(m_mpBand3);
	objdep.Add(m_mpBand4);
	objdep.Add(m_mpBand5);
	objdep.Add(m_mpBand7);
	if (!fnObj.fValid())
     objtime = objdep.tmNewest();
	ptr.Store(); 
	fNeedFreeze = true;
	sFreezeTitle = SMAPTextCalculateAlbedo;
}

MapAlbedo::~MapAlbedo()
{

}

const char* MapAlbedo::sSyntax() {
	
  return "MapAlbedo(Visibleband, NearIRMap, using2bands), or \n"
         "MapAlbedo(band1, band2, modis, band3, band4, band5, band7), or \n"
		 "MapAlbedo(band1, band3, aster, band5, band6, band8, band9)"; 
}

MapAlbedo* MapAlbedo::create(const FileName& fn, MapPtr& p, const String& sExpr)
{
  Array<String> as;
  int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  if (iParms < 3 )
      ExpressionError(sExpr, sSyntax());

  	Method  mMethod;
	Map mpRed(as[0], fn.sPath());
	Map mpNIR(as[1], fn.sPath());
	String sMtd = as[2].sVal();
    if (!(fCIStrEqual(sMtd, "using2bands") || fCIStrEqual(sMtd, "modis") || fCIStrEqual(sMtd, "aster")))
      ExpressionError(sExpr, sSyntax());
	if (fCIStrEqual(sMtd, "using2bands")){
		mMethod = mUsing2Bands;
		return new MapAlbedo(fn, p, mpRed,mpNIR, mMethod);
	}
	else {
		if (iParms < 7 )
	      ExpressionError(sExpr, sSyntax());
		if(fCIStrEqual(sMtd, "modis"))
			mMethod = mModis;
		if(fCIStrEqual(sMtd, "aster"))
			mMethod = mAster;
		Map mpRed(as[0], fn.sPath());
		Map mpNIR(as[1], fn.sPath());
		Map mpBand3(as[3], fn.sPath());
		Map mpBand4(as[4], fn.sPath());
		Map mpBand5(as[5], fn.sPath());
		Map mpBand7(as[6], fn.sPath());
		return new MapAlbedo(fn, p, mpRed,mpNIR, mMethod,mpBand3,mpBand4,mpBand5,mpBand7);
	}
}

void MapAlbedo::Store()
{
  MapFromMap::Store();
  WriteElement("MapFromMap", "Type", "MapAlbedo");
  WriteElement("MapAlbedo", "RedBand", m_mpRedBand);
  WriteElement("MapAlbedo", "NearInfraredBand", m_mpNIR);
  String sMethod;
  switch (m_mMethod)
  {
		case mUsing2Bands:
			sMethod = "Using2Bands";
			break;
		case mModis:
			sMethod = "Modis";
			WriteElement("MapAlbedo", "Band3", m_mpBand3);
			WriteElement("MapAlbedo", "Band4", m_mpBand4);
			WriteElement("MapAlbedo", "Band5", m_mpBand5);
			WriteElement("MapAlbedo", "Band7", m_mpBand7);
			break;
		case mAster:
			sMethod = "Aster";
			WriteElement("MapAlbedo", "Band1", m_mpRedBand);
			WriteElement("MapAlbedo", "Band3", m_mpNIR);
			WriteElement("MapAlbedo", "Band5", m_mpBand3);
			WriteElement("MapAlbedo", "Band6", m_mpBand4);
			WriteElement("MapAlbedo", "Band8", m_mpBand5);
			WriteElement("MapAlbedo", "Band9", m_mpBand7);
			break;
  }
  WriteElement("MapAlbedo", "Method", sMethod);
}

String MapAlbedo::sExpression() const
{
  
	String sMethod;
	String sExp;
	switch (m_mMethod)
	{
		case mUsing2Bands:
			sMethod = "using2bands";
			sExp = String("MapAlbedo(%S,%S,%S)", 
						mp->sNameQuoted(true, fnObj.sPath()),
						m_mpNIR->sNameQuoted(true),
						sMethod);
			break;
		case mModis:
			sMethod = "modis";
			sExp=String("MapAlbedo(%S,%S,%S,%S,%S,%S,%S)", 
						mp->sNameQuoted(true, fnObj.sPath()),
						m_mpNIR->sNameQuoted(true),
						sMethod,
						m_mpBand3->sNameQuoted(true),
						m_mpBand4->sNameQuoted(true),
						m_mpBand5->sNameQuoted(true),
						m_mpBand7->sNameQuoted(true));
			break;
		case mAster:
			sMethod = "aster";
			sExp=String("MapAlbedo(%S,%S,%S,%S,%S,%S,%S)", 
						mp->sNameQuoted(true, fnObj.sPath()),
						m_mpNIR->sNameQuoted(true),
						sMethod,
						m_mpBand3->sNameQuoted(true),
						m_mpBand4->sNameQuoted(true),
						m_mpBand5->sNameQuoted(true),
						m_mpBand7->sNameQuoted(true));
			break;
	}
	return sExp;
}

bool MapAlbedo::fDomainChangeable() const
{
  return false;
}

bool MapAlbedo::fGeoRefChangeable() const
{
  return false;
}

bool MapAlbedo::fFreezing()
{
	trq.SetText(SMAPTextCalculateAlbedo);

	RealBuf rBufRed;
	RealBuf rBufNIR;
	RealBuf rBufBand3;
	RealBuf rBufBand4;
    RealBuf rBufBand5;
	RealBuf rBufBand7;
	RealBuf rBufOutput;
	rBufRed.Size(iCols());
	rBufNIR.Size(iCols());
	rBufOutput.Size(iCols());
	rBufBand3.Size(iCols());
	rBufBand4.Size(iCols());
	rBufBand5.Size(iCols());
	rBufBand7.Size(iCols());
	
	// weight factors for albedo (Valiente et al., 1995)
	const double c00 = 0.035;
	const double c11 = 0.545;
	const double c22 = 0.32;

	// weight factors for albedo (Liang et al., 2000)
	double c1;
	double c2;
	double c3;
	double c4;
	double c5;
	double c7;
	double c0;

	switch (m_mMethod)
	{
		case mAster://Albedo=0.484B1+0.335B3-0.324B5+0.551B6+0.305B8-0.367B9-0.0015
			c1 = 0.484;
			c2 = 0.335;
			c3 = -0.324;
			c4 = 0.551;
			c5 = 0.305;
			c7 = -0.367;
			c0 = -0.0015;
			break;
		case mModis:
			c1 = 0.160;
			c2 = 0.291;
			c3 = 0.243;
			c4 = 0.116;
			c5 = 0.112;
			c7 = 0.081;
			c0 = -0.0015;
			break;
	}
	
	for (long iRow = 0; iRow< iLines(); iRow++ )
	{
		m_mpRedBand->GetLineVal(iRow, rBufRed);
		m_mpNIR->GetLineVal(iRow, rBufNIR);
		switch (m_mMethod)
		{
			case mModis:
            case mAster:
				  m_mpBand3->GetLineVal(iRow, rBufBand3);
				  m_mpBand4->GetLineVal(iRow, rBufBand4);
				  m_mpBand5->GetLineVal(iRow, rBufBand5);
				  m_mpBand7->GetLineVal(iRow, rBufBand7);
				
		}
		for (long iCol = 0; iCol< iCols(); iCol++){
			switch (m_mMethod)
			{
				case mUsing2Bands:
				  rBufOutput[iCol] = c00+c11*rBufRed[iCol]+c22*rBufNIR[iCol];
				  break;
				case mModis:
                case mAster:
				  rBufOutput[iCol]=c1*rBufRed[iCol]+
								   c2*rBufNIR[iCol]+
								   c3*rBufBand3[iCol]+
								   c4*rBufBand4[iCol]+	
								   c5*rBufBand5[iCol]+
								   c7*rBufBand7[iCol]+
								   c0;
			}		

		}
		// write the result
		ptr.PutLineVal(iRow, rBufOutput);
		trq.fUpdate(iRow, iLines());	
	}
	trq.fUpdate(iLines(), iLines());
	return true;
}

void MapAlbedo::CompitableGeorefs(FileName fn, Map mp1, Map mp2)
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





