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
/* GeoRefStereoMate
   made for StereoFromOrthoImage
   Copyright Ilwis System Development ITC
   november 1998, by Jan Hendrikse,
*/
#include "Headers\toolspch.h"
#include "Headers\Err\ILWISDAT.ERR"
#include "Engine\SpatialReference\Gr.h"
#include "Engine\SpatialReference\Grsmpl.h"
#include "Engine\SpatialReference\Coordsys.h"
#include "Engine\SpatialReference\Cslatlon.h"
#include "Engine\SpatialReference\GrStereoMate.h"
#include "Engine\Base\DataObjects\ObjectStructure.h"
#include "Headers\Hs\stereoscopy.hs"
#include "Headers\Hs\Georef.hs"
#define MAXANGLE 90
#define EPS10 1.e-10


GeoRefStereoMate::GeoRefStereoMate(const FileName& fn)
: GeoRefPtr(fn)
{ 
	ReadElement("GeoRefStereoMate","SourceGeoRef", grSourceMap);
	ReadElement("GeoRefStereoMate", "DTM", mapDTM);
  ReadElement("GeoRefStereoMate", "Angle", rAngle);
  ReadElement("GeoRefStereoMate", "Reference Height", rRefHeight);
	ReadElement("GeoRefStereoMate", "ColumnOffset", iColLeftExtra);
	Init();
}

GeoRefStereoMate::GeoRefStereoMate(const FileName& fn, const GeoRef& grSource, const Map& mpDTM,
																	 const double rAngl, const double rHeigh, const long iColOffs)
: GeoRefPtr(fn, CoordSystem(), RowCol()),grSourceMap(grSource), mapDTM(mpDTM),
																					rAngle(rAngl), rRefHeight(rHeigh), iColLeftExtra(iColOffs)
{
	if (abs(rAngle) > MAXANGLE)
		throw (ErrorObject(TR("Look Angle must be between 0 and 90 degrees")));
	Init();
}

GeoRefStereoMate::~GeoRefStereoMate()
{
}

CoordBounds GeoRefStereoMate::cb() const
{
	if ( mapDTM.fValid() ) {
		return mapDTM->cb();
	} else if ( grSourceMap.fValid()) {
		return grSourceMap->cb();
	}
	return GeoRefPtr::cb();

}
void GeoRefStereoMate::Init()
{
	if (!mapDTM.fValid())
		throw ErrorObject(TR("DTM is not valid"));
	GeoRef grDTM = mapDTM->gr();
	csDTM = mapDTM->gr()->cs();
	//mapDTM->SetInterpolMethod(imBILINEAR);
	if (!grDTM.fValid())
		throw ErrorObject(String("DTM has invalid georef"));
	CoordSystem csNew = grSourceMap->cs();
	//csNew.SetPointer(new GeoRefStereoMate());
	SetCoordSystem(csNew);
	mmSourceMap = MinMax(RowCol(0.0,0.0),grSourceMap->rcSize());
	rSourceMapPixSize = rPixSize();
	if (csNew->pcsLatLon())
	{
		double rEarthRadius = csNew->pcsViaLatLon()->ell.a;
		rSourceMapPixSize = grSourceMap->rPixSize() * rEarthRadius * M_PI/180;
	}
	cbDTM = grDTM->cb();//CoordBounds(Coord(mm.MinRow(), mm.MinCol()), Coord(mm.MaxRow(), mm.MaxCol()));
	rrMMDTM = mapDTM->rrMinMax(BaseMapPtr::mmmCALCULATE);  // find minmax height of DTM
	
	rEstimTerrainHeight = (4 * rrMMDTM.rHi() + rrMMDTM.rLo()) / 5;  // First terrainHeightGuess
	iMaxHeightCount = 15;
	if (mapDTM.fValid() && mapDTM->vr().fValid()) // protect against vr() == 0
		rHeightAccuracy = mapDTM->vr()->rStep();    
	else
		rHeightAccuracy = 10;
	rTanAngle = tan(rAngle*M_PI/180);
	fTransformDTMCoords = csDTM != csNew;
	_fValid = true;
}

double GeoRefStereoMate::rPixSize() const
{
	return grSourceMap->rPixSize();;
}

void GeoRefStereoMate::SetRowCol(RowCol rcSize)
{
	GeoRefPtr::SetRowCol(rcSize);
}

void GeoRefStereoMate::Coord2RowCol(const Coord& crd, double& rRow, double& rCol) const
{
	if (!fValid() || crd.fUndef()) {
    rRow = rUNDEF;
    rCol = rUNDEF;										
    return;
  }  
	double rRowTemp;
	double rColTemp;
	Coord crdDTM = crd;
	if (fTransformDTMCoords)
            crdDTM = csDTM->cConv(cs(), crd);
	double rZ = mapDTM->rValue(crdDTM);
  if (rZ == rUNDEF || abs(rZ) > 1e6 || rZ > rrMMDTM.rHi() || rZ < rrMMDTM.rLo()){
    rRow = rUNDEF;
    rCol = rUNDEF;
    return;
  }
	grSourceMap->Coord2RowCol(crd, rRowTemp, rColTemp);
  rRow = rRowTemp;
  rCol = rColTemp - rTanAngle * (rZ - rRefHeight) / rSourceMapPixSize + iColLeftExtra;
}

void GeoRefStereoMate::RowCol2Coord(double rRow, double rCol, Coord& crd) const
{
	if (!fValid()) {
    crd = crdUNDEF;
		return;
  }    
  if (rRow == rUNDEF || rCol == rUNDEF) {
    crd = crdUNDEF;
		return;
  }
	Coord cSource; //crd in SourceImage to be found and temporarely used for DTM_height search
	double rColSource = rCol- (double)iColLeftExtra; 
	//rDv piercing pnt of projection ray in ReferenceHeight Plane
	// (naming similar to direct linear parameters)
	double rDv = rCol - iColLeftExtra;///prSrce.second;/// + iColLeftExtra; 
	grSourceMap->RowCol2Coord(rRow, rColSource, cSource);
	if (abs(rTanAngle) < EPS10) {    // lightrays are vertical, no iteration needed
		if (cbDTM.fContains(cSource)) 
			rColSource = rDv;
    else {                        // sensor not above DTM area
      crd = crdUNDEF;							// hence no cSource found
			return;
		}
	}
	else 
	{
		double rW[2];  // 2 successive estimations of terrain height    
		double rH[2], rD[2];   // 2 succ computations of DTM heigth and vertical move resp   
		rW[0] = rRefHeight;/// rEstimTerrainHeight; 
		// rW[0] = rPrevTerrainHeight; (in case of segment digitizing in monoplot
		// rW[0] = rEstimTerrainHeight; //start at height found in previous RowCol2Coord call
		int iCount = 0; 
		double rN; // difference of 2 succesive vertical moves
		bool fFirst = true; // true if iteration just started
		int i = 0;
		rD[0] = 1000;  // initialize vertical shifts
		rD[1] = 1000;
	
		while ((abs(rD[i]) > rHeightAccuracy) && ( iCount < iMaxHeightCount))  
		{ 
			rColSource = rTanAngle * (rW[i] - rRefHeight)/rSourceMapPixSize + rDv;  //  first terrainpoint U, V guess
																//  where the light ray pierces the W = W[0] level plane
			grSourceMap->RowCol2Coord(rRow, rColSource, cSource);
			Coord crdDTM = cSource;
			if (fTransformDTMCoords)
            crdDTM = csDTM->cConv(cs(), cSource);
			rH[i] = mapDTM->rValue(crdDTM);
			if (rH[i] == rUNDEF) {
				crd = crdUNDEF;
				return;
			}  
			rD[i] = rW[i] - rH[i];
			if (abs(rD[i]) < rHeightAccuracy) {
				////prSrce = prMate;	// succes	
				break;           // Ground coord found with sufficient height-accuracy
			}
			else  {
				i = 1 - i;    // 1 "toggles" between 0 and 1
			}  
			if (fFirst) {
				rN = 0;
				fFirst = false;
				rW[1] = rH[0];
			}
			else
				rN = rD[1] - rD[0];  //  compare lightray heights at 2 successive places
			if ( rN /(rW[1] - rW[0]) < 0.5)        //  lightray parallel to terrain slope if rN == 0
				rW[i] = rH[1-i];                     //  rectangular spiral converging to endpoint ("stepwise")
			else
				rW[i] = (rD[1] * rW[0] - rD[0] * rW[1]) / rN; // triangular spiral converging faster and safer
			iCount++;
		} // end while iterating towards accurate ground coordinates   */
		///rEstimTerrainHeight = rW[i]; // last height is kept for next RowCol2Coordc call
	}
	grSourceMap->RowCol2Coord(rRow, rColSource, crd);
}

void GeoRefStereoMate::Store()
{
	if (!mapDTM.fValid())
    return;
  GeoRefPtr::Store();
  WriteElement("GeoRef", "Type", "GeoRefStereoMate");
	WriteElement("GeoRefStereoMate", "SourceGeoRef", grSourceMap);
	WriteElement("GeoRefStereoMate", "DTM", mapDTM);
  WriteElement("GeoRefStereoMate", "Angle", rAngle);
  WriteElement("GeoRefStereoMate", "Reference Height", rRefHeight);
	WriteElement("GeoRefStereoMate", "ColumnOffset", iColLeftExtra);
}

String GeoRefStereoMate::sType() const
{
  return "GeoRefStereoMate";
}

RowCol GeoRefStereoMate::rcGetRowColOffSet() const
{
	return RowCol();//rCoeff[7], rCoeff[3]);
}

double GeoRefStereoMate::rGetLookAngle() const
{
	return rAngle;
}

double GeoRefStereoMate::rGetRefHeight() const
{
	return rRefHeight;
}

GeoRef GeoRefStereoMate::grGetGeoRefSourceMap() const
{
	return grSourceMap;
}

void GeoRefStereoMate:: SetRowColOffSet(const RowCol rcOffSet)
{
}

void GeoRefStereoMate::SetLookAngle(const double rAng)
{
	rAngle = rAng;
}

void GeoRefStereoMate::SetRefHeight(const double rRefH)
{
	rRefHeight = rRefH;
}

void GeoRefStereoMate::SetGeoRefSourceMap(const GeoRef grSrc)
{
	grSourceMap = grSrc;
}

bool GeoRefStereoMate::fEqual(const IlwisObjectPtr& obj) const
{
	const GeoRefPtr* grp = dynamic_cast<const GeoRefPtr*>(&obj);
  if (grp == 0)
    return false;
  if (grp == this)
    return true;
  if (rcSize() != grp->rcSize())
    return false;
	const GeoRefStereoMate* grstm = dynamic_cast<const GeoRefStereoMate*>(grp);
  if (0 == grstm) return false;

	return rGetLookAngle() == grstm->rGetLookAngle() &&
				 rGetRefHeight() == grstm->rGetRefHeight() &&
				 grGetGeoRefSourceMap() == grstm->grGetGeoRefSourceMap();
		
}

void GeoRefStereoMate::GetObjectStructure(ObjectStructure& os)
{
	GeoRefPtr::GetObjectStructure( os );
	if ( os.fGetAssociatedFiles() )
	{
		os.AddFile(fnObj, "GeoRefStereoMate", "DTM");		
		os.AddFile(fnObj, "GeoRefStereoMate", "SourceGeoRef");
	}		
}	

bool GeoRefStereoMate::fGeoRefNone() const
{
   return grSourceMap->fGeoRefNone();
}
