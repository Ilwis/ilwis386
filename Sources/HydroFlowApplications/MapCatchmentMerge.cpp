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
// MapCatchmentExtraction.cpp: implementation of the MapCatchmentExtraction class.
//
//////////////////////////////////////////////////////////////////////

#include "HydroFlowApplications\MapCatchmentMerge.h"
#include "Engine\Table\tbl.h"
#include "Engine\Domain\dmsort.h"
#include "Engine\Domain\dmcoord.h"
#include "Engine\SpatialReference\Coordsys.h"
#include "Engine\SpatialReference\Ellips.h"
#include "Engine\SpatialReference\Csproj.h"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Hs\map.hs"

IlwisObjectPtr * createMapCatchmentMerge(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)MapCatchmentMerge::create(fn, (MapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new MapCatchmentMerge(fn, (MapPtr &)ptr);
}

const double rDefaultEarthRadius = 6371007.0;

using namespace std;

static CoordSystem csyLamCyl(FileName fn)
{
    CoordSystem csy;
    FileName fnCoordSyst(fn, ".csy", true);

    CoordSystemProjection * cspr = new CoordSystemProjection(fnCoordSyst, 1);
    csy.SetPointer(cspr);
    cspr->datum = new MolodenskyDatum("WGS 1984","");
    cspr->ell = cspr->datum->ell;
    
    Projection prj = cspr->prj;
    String sPrj ("Lambert Cylind EqualArea");
    prj = Projection(sPrj, cspr->ell);
    cspr->prj = prj;

    return csy;
}

static double rSphericalDistance(double rRadius, const LatLon& ll_1, const LatLon& ll_2)
{
		if (ll_1.fUndef() || ll_2.fUndef())
		return rUNDEF;
		double phi1 = ll_1.Lat * M_PI/180.0; //conversion to radians
		double lam1 = ll_1.Lon * M_PI/180.0; 
		double phi2 = ll_2.Lat * M_PI/180.0; ;
		double lam2 = ll_2.Lon * M_PI/180.0; ;
		double sinhalfc = fabs(sin((phi2- phi1)/2)* sin((phi2- phi1)/2) +
		 cos(phi1)*cos(phi2)* sin((lam2 - lam1)/2)* sin((lam2 - lam1)/2));
		sinhalfc = sqrt(sinhalfc);
		double c; // the shortest spherical arc
		if (sinhalfc < sqrt(2.0)/2)
			c = 2.0 * asin(sinhalfc);
		else 
		{
			phi2 = -phi2;
			lam2 = M_PI + lam2;
			sinhalfc = fabs(sin((phi2- phi1)/2)* sin((phi2- phi1)/2)+
				cos(phi1)*cos(phi2)*sin((lam2 - lam1)/2)*sin((lam2 - lam1)/2));
			sinhalfc = sqrt(sinhalfc);
			c = M_PI - 2.0 * asin(sinhalfc);
		}
		return c * rRadius;
}
static double rComputeSlope(double rDrop, double rLength, bool fDegree)
{
		if (rLength > 0)
		{
			if (fDegree  ) 
				return (180/M_PI)*atanl(abs(rDrop/rLength)); //degree
			else
				return (rDrop/rLength)*100; //percent
		}
		else
			return 0;
}

static double rComputeSinuosity(double rLength, double rStraightLenght)
{
	if (rStraightLenght > 0)
	 return rLength/rStraightLenght;
	else
	 return 0;
}

static Table VerifyAttributes(Map mpMap)
{
	
	Table tblAtt = mpMap->tblAtt();
	if (!tblAtt.fValid())
				throw ErrorNoAttTable(mpMap->fnObj);
	return tblAtt;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
MapCatchmentMerge::MapCatchmentMerge(const FileName& fn, MapPtr& p)
: MapFromMap(fn, p)
{

	Table tblDrainage = VerifyAttributes(mp);
	if (!tblDrainage[String("Strahler")].fValid())
					ColumnNotFoundError(tblDrainage->fnObj, String("Strahler"));

	if (!tblDrainage[String("Shreve")].fValid())
					ColumnNotFoundError(tblDrainage->fnObj, String("Shreve"));

	
	ReadElement("CatchmentMerge", "FlowDirection", m_mpFlow);
	Domain dm = m_mpFlow->dm();
	if (!(dm.fValid() && (fCIStrEqual(dm->fnObj.sFileExt(), "FlowDirection.dom"))))
			throw ErrorObject(WhatError(TR("Use an input map with domain FlowDirection   "), errMapCatchmentMerging), m_mpFlow->fnObj);

	ReadElement("CatchmentMerge", "FlowAccumulation", m_mpFacc);
	ReadElement("CatchmentMerge", "DEM", m_mpDEM);
	ReadElement("CatchmentMerge", "UseOutlets", m_UseOutlets);
	if (m_UseOutlets)
	{
		ReadElement("CatchmentMerge", "OutletPointMap", m_pmpOutlets);
		ReadElement("CatchmentMerge", "UndefinedPixels", m_fUndefined);
		ReadElement("CatchmentMerge", "ExtractStreamNetwork", m_fExtractStreams);
	}
	else
	{
		ReadElement("CatchmentMerge", "OrderSystem", m_sOrderSystem);
		ReadElement("CatchmentMerge", "StreamOrders", m_iStreamOrders);	
		ReadElement("CatchmentMerge", "ExtractOriginalOrder", m_bExtractOriginalOrder);
	}

	ReadElement("CatchmentMerge", "LongestFlowPathSegmentMap", m_sLongestFlowPathSegmentMap);
	
	dm = mp->dm();
	if (!(dm.fValid())) 
			throw ErrorObject(WhatError(TR("Map should have identifier domain"), errMapCatchmentMerging), mp->fnObj);

	CompitableGeorefs(fn, mp, m_mpFlow);
	CompitableGeorefs(fn, mp, m_mpFacc);
	
	m_dm = p.dm();
	fNeedFreeze = true;
	sFreezeTitle = "MapCatchmentMerge";
	htpFreeze = "ilwisapp\\catchment_merge_algorithm.htm";
}

MapCatchmentMerge::MapCatchmentMerge(const FileName& fn, 
									 MapPtr& p,
									 const Domain& dm,
									 const Map& mpStreamNetwork,
									 const Map& mpFlow,
                                     const Map& mpFacc,
                                     const Map& mpDEM,
									 const PointMap& pmpOutlets,
                                     bool fUndefined,
                                     bool bExtractStreams)
: MapFromMap(fn, p, mpStreamNetwork),
	m_dm(dm),
	m_mpFlow(mpFlow),
  m_mpFacc(mpFacc),
  m_mpDEM(mpDEM),
	m_pmpOutlets(pmpOutlets),
	m_UseOutlets(true),
  m_fUndefined(fUndefined),
  m_fExtractStreams(bExtractStreams),
  m_sLongestFlowPathSegmentMap("")
{
	//Verify compatible geo-reference
	CompitableGeorefs(fn, mp, m_mpFlow);
	CompitableGeorefs(fn, mp, m_mpFacc);
  CompitableGeorefs(fn, mp, m_mpDEM);
	init();
	
  if (!fnObj.fValid())
    objtime = objdep.tmNewest();
	ptr.Store(); 
}

MapCatchmentMerge::MapCatchmentMerge(const FileName& fn, 
									 MapPtr& p,
									 const Domain& dm,
									 const Map& mpStreamNetwork,
									 const Map& mpFlow,
                                     const Map& mpFacc,
                                     const Map& mpDEM,
									 const PointMap& pmpOutlets,
                                     bool bUndefined,
                                     bool bExtractStreams,
                                     String sLongestFlowPathSegmentMap)
: MapFromMap(fn, p, mpStreamNetwork),
	m_dm(dm),
	m_mpFlow(mpFlow),
	m_mpFacc(mpFacc),
	m_mpDEM(mpDEM),
	m_pmpOutlets(pmpOutlets),
	m_UseOutlets(true),
	m_fUndefined(bUndefined),
	m_fExtractStreams(bExtractStreams),
	m_sLongestFlowPathSegmentMap(sLongestFlowPathSegmentMap)
{
	//Verify compatible geo-reference
	CompitableGeorefs(fn, mp, m_mpFlow);
	CompitableGeorefs(fn, mp, m_mpFacc);
	CompitableGeorefs(fn, mp, m_mpDEM);
	init();
	
	if (!fnObj.fValid())
		objtime = objdep.tmNewest();
	ptr.Store(); 
}

MapCatchmentMerge::MapCatchmentMerge(const FileName& fn, 
									 MapPtr& p,
	   								 const Domain& dm,
									 const Map& mpStreamNetwork,
									 const Map& mpFlow,
                                     const Map& mpFacc,
                                     const Map& mpDEM,
									 String sOrderSystem,
									 long iStreamOrders,
									 bool bExtractOriginalOrder)
: MapFromMap(fn, p, mpStreamNetwork),
	m_dm(dm),
	m_mpFlow(mpFlow),
	m_mpFacc(mpFacc),
	m_mpDEM(mpDEM),
	m_sOrderSystem(sOrderSystem),
	m_UseOutlets(false),
	m_iStreamOrders(iStreamOrders),
	m_bExtractOriginalOrder(bExtractOriginalOrder),
	m_sLongestFlowPathSegmentMap("")
{
	init();
	//Verify compatible geo-reference
	CompitableGeorefs(fn, mp, m_mpFlow);
	CompitableGeorefs(fn, mp, m_mpFacc);
	CompitableGeorefs(fn, mp, m_mpDEM);
	if (!fnObj.fValid())
		objtime = objdep.tmNewest();
	ptr.Store(); 
}

MapCatchmentMerge::MapCatchmentMerge(const FileName& fn, 
									 MapPtr& p,
									 const Domain& dm,
									 const Map& mpStreamNetwork,
									 const Map& mpFlow,
                                     const Map& mpFacc,
                                     const Map& mpDEM,
									 String sOrderSystem,
									 long iStreamOrders,
									 bool bExtractOriginalOrder,
									 String sLongestFlowPathSegmentMap)
:	MapFromMap(fn, p, mpStreamNetwork),
	m_dm(dm),
	m_mpFlow(mpFlow),
	m_mpFacc(mpFacc),
	m_mpDEM(mpDEM),
	m_sOrderSystem(sOrderSystem),
	m_UseOutlets(false),
	m_iStreamOrders(iStreamOrders),
	m_bExtractOriginalOrder(bExtractOriginalOrder),
	m_sLongestFlowPathSegmentMap(sLongestFlowPathSegmentMap)
{
	init();
	//Verify compatible geo-reference
	CompitableGeorefs(fn, mp, m_mpFlow);
	CompitableGeorefs(fn, mp, m_mpFacc);
	CompitableGeorefs(fn, mp, m_mpDEM);
	if (!fnObj.fValid())
		objtime = objdep.tmNewest();
	ptr.Store(); 
}

MapCatchmentMerge::~MapCatchmentMerge()
{

}

void MapCatchmentMerge::init()
{
	
	//Verify domain and attribute table
	Domain dm = mp->dm();
	if (!(dm.fValid())) 
			throw ErrorObject(WhatError(TR("Map should have identifier domain"), errMapCatchmentMerging), mp->fnObj);

	Table tblDrainage = VerifyAttributes(mp);
	if (!tblDrainage[String("Strahler")].fValid())
					ColumnNotFoundError(tblDrainage->fnObj, String("Strahler"));
	if (!tblDrainage[String("Shreve")].fValid())
					ColumnNotFoundError(tblDrainage->fnObj, String("Shreve"));

	Domain flowdm = m_mpFlow->dm();
	if (!(flowdm.fValid() && (fCIStrEqual(flowdm->fnObj.sFileExt(), "FlowDirection.dom"))))
			throw ErrorObject(WhatError(TR("Use an input map with domain FlowDirection   "), errMapCatchmentMerging), m_mpFlow->fnObj);

	SetDomainValueRangeStruct(m_dm);  
	fNeedFreeze = true;
	sFreezeTitle = "MapCatchmentMerge";
	htpFreeze = "ilwisapp\\catchment_merge_algorithm.htm";

	objdep.Add(m_mpFlow);
	objdep.Add(m_mpFacc);
	objdep.Add(m_mpDEM);
	if(m_UseOutlets)
		objdep.Add(m_pmpOutlets);
	
}

const char* MapCatchmentMerge::sSyntax()
{ 
		return "MapCatchmentMerge(DrainageNetworkOrderMap, FlowDirectionMap, FlowAccumulationMap, DEM, OutletLocations, true|false, true|false), or \n"
           "MapCatchmentMerge(DrainageNetworkOrderMap, FlowDirectionMap, FlowAccumulationMap, DEM, OutletLocations, true|false, true|false, LongestFlowPathSegmentMap) or \n" 
           "MapCatchmentMerge(DrainageNetworkOrderMap, FlowDirectionMap, FlowAccumulationMap, DEM, Strahler|Shreve,StreamOrders, true|false) or \n" 
           "MapCatchmentMerge(DrainageNetworkOrderMap, FlowDirectionMap, FlowAccumulationMap, DEM, Strahler|Shreve,StreamOrders, true|false, LongestFlowPathSegmentMap)\n";
}

static void SameNameError(const FileName& fn)
{
  throw ErrorObject(WhatError(TR("Same name used for Longest flow path segment map\nand output Catchment map"), errMapCatchmentMerging+4), fn);
}

MapCatchmentMerge* MapCatchmentMerge::create(const FileName& fn, 
																						 MapPtr& p, 
																						 const String& sExpr)
{
	Array<String> as;
	int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
	if (iParms < 7)
      ExpressionError(sExpr, sSyntax());

	Map mpstream(as[0], fn.sPath());
	Map mpflowdir(as[1], fn.sPath());
	Map mpFacc(as[2], fn.sPath());
	Map mpDEM(as[3], fn.sPath());
	
	FileName fnDom(fn, ".dom");
	fnDom = FileName::fnUnique(fnDom);
	Domain dm = Domain(fnDom, 0, dmtID);
	DomainSort *pds = dm->pdsrt();
	pds->dsType = DomainSort::dsMANUAL;

	bool fUseOrderSystem;
	String sOrderSystem = as[4].sVal();
	if (fCIStrEqual(sOrderSystem, "Strahler") || fCIStrEqual(sOrderSystem, "Shreve"))
		fUseOrderSystem = true;
	else
		fUseOrderSystem = false;

	String sLongestFlowPathSegmentMap = "";
	if (fUseOrderSystem)
	{
		long iStreamOrder = as[5].iVal();
		bool bExtractOriginalOrder = as[6].fVal();
		if (iParms == 8 )
			sLongestFlowPathSegmentMap = as[7].sVal();
		if (iStreamOrder<=0) 
			throw ErrorObject(WhatError(TR("Stream order must be greater than 0"), errMapCatchmentMerging));
  
   		return new MapCatchmentMerge(fn, p, dm, mpstream, mpflowdir, mpFacc, mpDEM, sOrderSystem, iStreamOrder, bExtractOriginalOrder, sLongestFlowPathSegmentMap);
	} 
	else
	{
		PointMap pmpOutlets(as[4], fn.sPath());
		bool fUndefinedPixels = as[5].fVal();
		bool fExtractStreams = as[6].fVal();
		if (iParms == 8 )
		sLongestFlowPathSegmentMap = as[7].sVal();
		return new MapCatchmentMerge(fn, p, dm, mpstream, mpflowdir, mpFacc, mpDEM, pmpOutlets, fUndefinedPixels, fExtractStreams,sLongestFlowPathSegmentMap);
	}
}

void MapCatchmentMerge::Store()
{
	MapFromMap::Store();
	WriteElement("MapFromMap", "Type", "MapCatchmentMerge");
	WriteElement("CatchmentMerge", "DrainageNetworkMap", mp);
	WriteElement("CatchmentMerge", "FlowDirection", m_mpFlow);
	WriteElement("CatchmentMerge", "FlowAccumulation", m_mpFacc);
	WriteElement("CatchmentMerge", "DEM", m_mpDEM);
	WriteElement("CatchmentMerge", "UseOutlets", m_UseOutlets);
	if (m_UseOutlets)
	{  
		WriteElement("CatchmentMerge", "OutletPointMap", m_pmpOutlets);
		WriteElement("CatchmentMerge", "UndefinedPixels", m_fUndefined);
		WriteElement("CatchmentMerge", "ExtractStreamNetwork", m_fExtractStreams);
	}
	else
	{
		WriteElement("CatchmentMerge", "OrderSystem", m_sOrderSystem);
		WriteElement("CatchmentMerge", "StreamOrders", m_iStreamOrders);
		WriteElement("CatchmentMerge", "ExtractOriginalOrder", m_bExtractOriginalOrder);
	}
	if (m_sLongestFlowPathSegmentMap.length() != 0)
		WriteElement("CatchmentMerge", "LongestFlowPathSegmentMap", m_sLongestFlowPathSegmentMap);
}

String MapCatchmentMerge::sExpression() const
{
  String sExp;
  if(m_UseOutlets)
  {
    if (m_sLongestFlowPathSegmentMap.length() == 0)
  	    sExp = String("MapCatchmentMerge(%S,%S,%S,%S,%S,%li,%li)", 
							 mp->sNameQuoted(true, fnObj.sPath()),
							 m_mpFlow->sNameQuoted(true),
               m_mpFacc->sNameQuoted(true),
               m_mpDEM->sNameQuoted(true),
							 m_pmpOutlets->sNameQuoted(true),
               m_fUndefined,
               m_fExtractStreams);
    else
        sExp = String("MapCatchmentMerge(%S,%S,%S,%S,%S,%li,%li,%S)", 
							mp->sNameQuoted(true, fnObj.sPath()),
							m_mpFlow->sNameQuoted(true),
							m_mpFacc->sNameQuoted(true),
							m_mpDEM->sNameQuoted(true),
							m_pmpOutlets->sNameQuoted(true),
							m_fUndefined,
							m_fExtractStreams, 
							m_sLongestFlowPathSegmentMap);
  }
  else if (m_sLongestFlowPathSegmentMap.length() == 0)
  		sExp = String("MapCatchmentMerge(%S,%S,%S,%S,%S,%li,%li)", 
							 mp->sNameQuoted(true, fnObj.sPath()),
							 m_mpFlow->sNameQuoted(true),
               m_mpFacc->sNameQuoted(true),
               m_mpDEM->sNameQuoted(true),
							 m_sOrderSystem, 
							 m_iStreamOrders,
							 m_bExtractOriginalOrder);
  else
      sExp = String("MapCatchmentMerge(%S,%S,%S,%S,%S,%li,%li,%S)", 
							 mp->sNameQuoted(true, fnObj.sPath()),
							 m_mpFlow->sNameQuoted(true),
							 m_mpFacc->sNameQuoted(true),
							 m_mpDEM->sNameQuoted(true),
							 m_sOrderSystem, 
							 m_iStreamOrders,
							 m_bExtractOriginalOrder,
							 m_sLongestFlowPathSegmentMap);

  return sExp;
}

bool MapCatchmentMerge::fDomainChangeable() const
{
  return false;
}

bool MapCatchmentMerge::fGeoRefChangeable() const
{
  return false;
}

class SortOutletsLessClass //compare two elements for sort algorithm
{
public:
	SortOutletsLessClass(vector<OutletLocation>& vol) :
	m_vOutlet(vol)
	{
	}
	bool operator()(OutletLocation ol1, OutletLocation ol2)
	{ 
		return ol1.StreveOrder < ol2.StreveOrder;
	}
private:
		vector<OutletLocation>& m_vOutlet;
};

bool IsOutletExists(vector<OutletLocation> vOutlet, RowCol rc)
{ 
	for (vector<OutletLocation>::iterator pos = vOutlet.begin(); pos < vOutlet.end(); ++pos)
	{
		OutletLocation ol = (*pos);
		bool fExist = (ol.rc.Row == rc.Row) && (ol.rc.Col == rc.Col);		
		if (fExist)
			return fExist;
	}
	return false;
}

void MapCatchmentMerge::InitInOutMaps()
{
  
	trq.SetText(TR("Initialize map"));
	trq.Start();
	m_vStreamMap.Open(iLines(), iCols());
	m_vFlowDir.Open(iLines(), iCols());
	m_vOutput.Open(iLines(), iCols());

	for (long iRow = 0; iRow< iLines(); iRow++ )
	{
		m_vStreamMap[iRow].Size(iCols()); 
		mp->GetLineRaw(iRow, m_vStreamMap[iRow]);

		m_vFlowDir[iRow].Size(iCols());
		m_vOutput[iRow].Size(iCols());
		m_mpFlow->GetLineRaw(iRow, m_vFlowDir[iRow]);
		for (long iCol = 0; iCol < iCols(); ++iCol)
		{
			m_vOutput[iRow][iCol] = iUNDEF;
		}
		trq.fUpdate(iRow, iLines());	
	}
	trq.fUpdate(iLines(), iLines());
} 

void MapCatchmentMerge::WriteOutputRaster()
{
  trq.SetText(TR("Write output map"));
	for (long iRow = 0; iRow < iLines(); iRow++ )
	{
		LongBuf& bCatchment = m_vOutput[iRow];
    ptr.PutLineRaw(iRow, bCatchment);
		trq.fUpdate(iRow, iLines());
	}
	trq.fUpdate(iLines(), iLines());
}

void MapCatchmentMerge::CreatePolygonMap(FileName fn)  
{
  trq.SetText(TR("Create catchment polygon map"));
  //including the udefined pixels within the catchment merged
  //this option is allowed only to a catchment merged using one outlet location  
  if(m_UseOutlets && m_fUndefined && m_vOutlet.size() == 1)
  {
    FileName fnTmpPol = FileName::fnUnique(fnObj);
    fnTmpPol.sExt = ".mpa";
    String sExprTmpPol("PolygonMapFromRas(%S, %li, %S)", fnObj.sFullPathQuoted(), 8, String("smooth")); 
    PolygonMap polMapTmp; 
	  polMapTmp = PolygonMap(fnTmpPol, sExprTmpPol);
	  polMapTmp->Calc();
    polMapTmp->fErase = true;

	  FileName fnTmpSeg1(fnObj, ".mps");
		fnTmpSeg1 = FileName::fnUnique(fnTmpSeg1);
    //SegmentMapPolBoundaries(m_cm5,"*",unique)
    String sExprseg1("SegmentMapPolBoundaries(%S,\"%S\", %S)", fnTmpPol.sFullPathQuoted(), String("*"), String("unique")); 
    SegmentMap segMap1; 
	  segMap1 = SegmentMap(fnTmpSeg1, sExprseg1);
	  segMap1->Calc();
    segMap1->fErase = true;
    
    FileName fnTmpSeg2(fnObj, ".mps");
		fnTmpSeg2 = FileName::fnUnique(fnTmpSeg2);
    String sExprseg2("SegmentMapAttribute(%S,%S)", fnTmpSeg1.sFullPathQuoted(), String("pol1")); 
    SegmentMap segMap2; 
	  segMap2 = SegmentMap(fnTmpSeg2, sExprseg2);
	  segMap2->Calc();
    segMap2->fErase = true;
        
    //PolygonMapFromSegmentNonTopo(segatt1,"*",segments)
    String sExprPol1("PolygonMapFromSegmentNonTopo(%S, \"%S\", %S)", fnTmpSeg2.sFullPathQuoted(), String("*"), String("segments")); 
    polMapTmp = PolygonMap(fnTmpPol, sExprPol1);
	  polMapTmp->Calc();

    //MapRasterizePolygon(pol1.mpa,sub_fill.grf)
    FileName fnTmpRas1(fnObj, ".mpr");
    fnTmpRas1 = FileName::fnUnique(fnTmpRas1);
	  String sExprRas1("MapRasterizePolygon(%S,%S)", fnTmpPol.sFullPathQuoted(), mp->gr()->fnObj.sFullNameQuoted()); 
    Map mpRas = Map(fnTmpRas1, sExprRas1);
    mpRas->Calc();
    mpRas->fErase = true;
    
    trq.SetText(TR("Write output map"));
    for (long iRow = 0; iRow< iLines(); iRow++ )
	  {
      LongBuf bCatchment;
      bCatchment.Size(iCols());
		  mpRas->GetLineRaw(iRow, bCatchment);
      ptr.PutLineRaw(iRow, bCatchment);
		  trq.fUpdate(iRow, iLines());	
	  }
  }
  FileName fnPol(fnObj,".mpa");
  fnPol = FileName::fnUnique(fnPol);
  String sExprPol("PolygonMapFromRas(%S, %li, %S)", fnObj.sFullPathQuoted(), 8, String("smooth")); 
  PolygonMap polMap; 
	polMap = PolygonMap(fnPol, sExprPol);
	polMap->Calc();
	polMap->SetAttributeTable(m_tbl);
}

void MapCatchmentMerge::GeneratePolygonStatistics(FileName fnPol, Table& tblHsa)
{
  FileName fnTmpHsa;
	String sExprTbl;
	//Check the type of coordinate system  
	if (fLatLonCoords())
	{
		//Transform the map to Lambert Cylind EqualArea projection coordinate system needed to be able
		//to calculate the perimeter and area parameters
		//LAMCYL is a pre-defined coordinate system, should be placed in the ILWIS system directory
		FileName fnTmpTFPol(fnObj, ".mpa");
		fnTmpTFPol = FileName::fnUnique(fnTmpTFPol);
		//String sExprPMT("PolygonMapTransform(%S, %S, %g)", fnPol.sFullPathQuoted(false), String("lamcyl"), 0.000000); 
    CoordSystem csy = csyLamCyl(fnTmpTFPol);
	csy->Store(); // explicit Store(), due to different behavior between Debug and Release build!! (csyLamCyl will auto-store when the internal csy object is destructed (as it is supposed to do) in the debug build, but not in the release build)
    csy->fErase = true;
		String sExprPMT("PolygonMapTransform(%S, %S, %g)", fnPol.sFullPathQuoted(false), csy->sName(), 0.000000); 
		PolygonMap polTmpTFMap; 
		polTmpTFMap = PolygonMap(fnTmpTFPol, sExprPMT);
		polTmpTFMap->Calc();
		polTmpTFMap->fErase = true;
		//---a temporary histogram file needed to retrieve attributes about Area and Perimeter   
		fnTmpHsa = FileName(fnTmpTFPol, ".hsa");
		sExprTbl = String("TableHistogramPol(%S)", fnTmpTFPol.sFullPathQuoted()); 
		tblHsa = Table(fnTmpHsa, sExprTbl);
	}
	else
	{
		fnTmpHsa = FileName(fnObj, ".hsa");
		sExprTbl = String("TableHistogramPol(%S)", fnPol.sFullPathQuoted()); 
		tblHsa = Table(fnTmpHsa, sExprTbl);
	}

  tblHsa->Calc();
	tblHsa->fErase = true;
}

bool MapCatchmentMerge::fFreezing()
{
	trq.SetTitle(sFreezeTitle);
	InitInOutMaps();
	
	//---Create attibute table associated with output map
	CreateTable();
	AddLink2StreamSegments();
	
	long id = 0;
	if (m_UseOutlets)
	{
		InitOutletVector();
		//---Sort outlets by using operator(elem1, elem2) sorting algorithms by Streve number
		if (m_vOutlet.size() > 0)
		{
			SortOutletsLessClass streve(m_vOutlet);
			sort(m_vOutlet.begin(), m_vOutlet.end(), streve);
		}
	}
	else //---Use stream orders option
	{
		//---Evaluate joint down-flow coords/outlet locations by stream order
		EvaluateJointOutletsbyOrder();
	}
	//Merging sub-catchments here
	trq.SetText(TR("Merge sub-catchments"));
	for (vector<OutletLocation>::iterator pos = m_vOutlet.begin(); pos < m_vOutlet.end(); ++pos)
	{		
		RowCol rc = pos->rc;
		OutletLocation ol = (*pos);
		id++;
		m_vUpCatchmentIDs.resize(0);
		m_vStreamsInCatchment.resize(0);
		m_sUpLinkCatchment = String("");
		m_iOutletVal = m_vStreamMap[rc.Row][rc.Col];
		AddDomainItem(m_dm, id);

		//---put stream id, defined by the joint outlet location in the catchment, if it is a no-flow stream
		if((m_UseOutlets && ol.isOnNode != true) || (!m_UseOutlets)) 
			m_vStreamsInCatchment.push_back(m_vStreamMap[rc.Row][rc.Col]);
		MergeCatchment(rc.Row, rc.Col, id, false);
		UpdateUpLinkCatchment(id);
		UpdateDownLinkCatchment(id);		
		UpdateLink2StreamSegments(id, ol);
		if (trq.fUpdate(id, (int)m_vOutlet.size())) return false;
	}
	trq.fUpdate((int)m_vOutlet.size(), (int)m_vOutlet.size());
	
	//--- If merging using stream order, we also need to 
	//--- "copy" the unmerged sub-catchments to the output and
	//--- rebuild flow connectivity topology as well
	if (!m_UseOutlets && m_bExtractOriginalOrder) //&& m_iStreamOrders < m_iMaxOrderNumber)
	{
		//MergeCatchmentsRemained(id);
		ExtractOriginalOrder(id);
	}
	
	//Clean up
	m_vStreamMap.Close();
	m_vUpCatchmentIDs.resize(0);
	m_dm->pdsrt()->Resize(id);
	
	//Write output catchment raster
	WriteOutputRaster();

	//---Polygonize the catchment map 
	FileName fnPol(fnObj,".mpa");
	CreatePolygonMap(fnPol);  

	trq.SetText(TR("Compute catchment attributes"));
	Table tblHsa;
	GeneratePolygonStatistics(fnPol,tblHsa);

	ComputeCatchmentArea(tblHsa);
	ComputeTotalUpStreamCatchmentArea(tblHsa);
	
	//Generate longest flow path ID segment map
	if(m_sLongestFlowPathSegmentMap.length() > 0)
	{
		FileName fnLongestSegmap(m_sLongestFlowPathSegmentMap, fnObj);
		fnLongestSegmap.sExt = ".mps";
		m_segLongestSegmap = SegmentMap(fnLongestSegmap, cs(), cb(), m_dm);
	}

	if (m_fExtractStreams == true)
	{
		SegmentMap segDesmap = ExtractSegments();
		CreateTableSegmentsExtracted(segDesmap); 
	}

	ReadFlowAccumulation();
	ComputeOtherAttributes();
	ComputeCenterPolygon(fnPol);

	m_vOutput.Close();
	m_vOutlet.resize(0);
	m_vFlowDir.Close();
	m_vFAcc.Close();
	m_vStream.resize(0);
	m_vStreamCoord.resize(0);
	m_vDrainageAtt.resize(0);
	m_vStreamsInCatchment.resize(0);
	return true;
}

SegmentMap MapCatchmentMerge::ExtractSegments()
{
    //m_dm->pdsrt()->Resize(id);

    //***a temporary segment map needed to be able to remove/patch a channel   
	  FileName fnSrcSeg = mp->fnObj; //input segments 
	  fnSrcSeg.sExt = ".mps";
    String sMask = BuildMaskIDs();
    String sExprSeg = String("SegmentMapMask(%S,\"%S\")",fnSrcSeg.sFullPathQuoted(), sMask);
    //String sExprSeg("SegmentMapMask(%S, %S)", fnSrcSeg.sFullPathQuoted(), sMask); 

    FileName fnSegTmp = FileName::fnUnique(fnSrcSeg);
	  fnSegTmp.sExt = ".mps";
		SegmentMap mpSrcSeg = SegmentMap(fnSegTmp, sExprSeg);
		mpSrcSeg->Calc();
    mpSrcSeg->fErase = true;

	  //Create extracted segment map
    FileName fnDom(fnObj, ".dom");
  	fnDom = FileName::fnUnique(fnDom);
    Domain dm = Domain(fnDom,0, dmtID);  

    FileName fnDesSeg(fnObj, ".mps");
    fnDesSeg = FileName::fnUnique(fnDesSeg);
	  fnDesSeg.sExt = ".mps";
	  SegmentMap segDesmap = SegmentMap(fnDesSeg, cs(), cb(), dm);
	
    //Remove the part of segments splited by the outlets, if needed
    CleanSegment(segDesmap, mpSrcSeg);
    return segDesmap;
}

void MapCatchmentMerge::ReadFlowAccumulation()
{
  trq.SetText(TR("Reading Flow Accumulation Map"));
	m_vFAcc.Open(iLines(), iCols());
	for (long iRow = 0; iRow< iLines(); iRow++ )
	{
		m_vFAcc[iRow].Size(iCols()); 
		m_mpFacc->GetLineVal(iRow, m_vFAcc[iRow]);
		trq.fUpdate(iRow, iLines());	
	}
	trq.fUpdate(iLines(), iLines());
}

void MapCatchmentMerge::AddDomainItem(Domain dm, long iItem )
{
	String sUniqueID = String("%li", iItem);
	dm->pdsrt()->iAdd(sUniqueID,true);
}

void MapCatchmentMerge::CompitableGeorefs(FileName fn, Map mp1, Map mp2)
{
	bool fIncompGeoRef = false;
	if (mp1->gr()->fGeoRefNone() && mp2->gr()->fGeoRefNone())
		  fIncompGeoRef = mp1->rcSize() != mp2->rcSize();
	else
			fIncompGeoRef = mp1->gr() != mp2->gr();
	if (fIncompGeoRef)
			throw ErrorIncompatibleGeorefs(mp1->gr()->sName(true, fn.sPath()),
            mp2->gr()->sName(true, fn.sPath()), fn, errMapCatchmentExtraction);

}

bool MapCatchmentMerge::IsEdgeCell(long iRow, long iCol)
{
		if (iRow <= 0 || iRow >= iLines() - 1 ||
				iCol <= 0 || iCol >= iCols() - 1)
				return true;
		else
				return false;
}

static void SplitString(String s, vector<long> &results)
{
	Array<String> as;
	Split(s, as, ",{}");
	//results.clear();
	for (unsigned int i=0; i < as.size(); i++)
	{
		long res = as[i].iVal();
		if (res != iUNDEF)
			results.push_back(res);
	}
}

static void SplitString(String s, vector<double> &results)
{
	Array<String> as;
	Split(s, as, ",{}");
	results.clear();
	for (unsigned int i=0; i < as.size(); i++)
	{
		long res = (long)as[i].rVal();
		if (res != iUNDEF)
			results.push_back(res);
	}
}

//Put the outlet locations and ite attributes e.g. Streve in to a vector
void MapCatchmentMerge::InitOutletVector()
{
	long iPoints = m_pmpOutlets->iFeatures();
	Coord crd;
  
	bool fTransformCoords = cs() != m_pmpOutlets->cs();

	Table tblAtt = mp->tblAtt();
	Column colStreve = tblAtt->col(String("Shreve"));
	OutletLocation ol;
	for (long p = 0; p < iPoints; p++) {
		crd = m_pmpOutlets->cValue(p);
		if (fTransformCoords)
			crd = cs()->cConv(m_pmpOutlets->cs(), crd);
			ol.rc = gr()->rcConv(crd);
    
			if (ol.rc.fUndef() || IsEdgeCell(ol.rc.Row, ol.rc.Col))
				continue;

			//Catch stream ID, if outlet locates away from the stream, relocat to the neast
			//drainage line in a 5 by 5 pixel window, otherwis just ignore it 
			ol.StreamID = m_vStreamMap[ol.rc.Row][ol.rc.Col];
			if (ol.StreamID == iUNDEF || ol.StreamID <= 0)
			{
				if (fRelocatOutlet(ol.rc,1) == false)  // based on 3 * 3 window
					if (fRelocatOutlet(ol.rc,2) == false) // based on 5 * 5 window
						continue;
				ol.StreamID = m_vStreamMap[ol.rc.Row][ol.rc.Col];  
			}
			ol.StreveOrder = colStreve->iValue(ol.StreamID);  
			RowCol rcFrom = m_vDrainageAtt[ol.StreamID-1].UpstreamCoord;
			if (rcFrom.Row != ol.rc.Row || rcFrom.Col != ol.rc.Col)
			{
				ol.rLen1 = CalculateLength(rcFrom, ol.rc, ol.StreamID);
				ol.rLen2 = m_vDrainageAtt[ol.StreamID-1].rLenght - ol.rLen1;
				ol.isOnNode = false;
			}
			else
			{
				ol.rLen1 = rUNDEF;
				ol.rLen2 = rUNDEF;
				ol.isOnNode = true;
			}
			ol.fExtractOverlandFlowPath = true;
		
		//ol.StreveOrder = iUNDEF;  //outlet is located on the junction! 
		m_vOutlet.push_back(ol);
	}
}

//if the outlet is off the drainage, relocate it to the neast drainage line
//in defined iSize by iSize pixel window
bool MapCatchmentMerge::fRelocatOutlet(RowCol& rc, int iSize)
{
  for (int i=-iSize; i<=iSize; ++i)
	{
		for(int j=-iSize; j<=iSize; ++j)
    {  
			if (m_vStreamMap[rc.Row+i][rc.Col+j] > 0)
      {
        rc.Row = rc.Row+i;
        rc.Col = rc.Col+j;
        return true;
      }
    }
  }
  return false;
}
long MapCatchmentMerge::MergeCatchment(long iRow, long iCol, const long iFlag, const bool fExtractOriginalOrder)
{
	//****Merge sub-catchments and assigned id iFlag to the merged catchment 
	//For the specified outlet cell in loaction rc, 
	//check whether its neighboring cells flow to it,
	//If true, flag the cells with iFlag in m_vOutput, 
	//the function is called recursively to all the neighboring cells that flow into it. 
	//The recursion stops when it reaches a cell that has no flow to it, or a cell that 
	//has been evaluated.  

	//****Build uplink catchments topology, which are stored in m_sUpLinkCatchment
	//****Store streams in a merged catchment in m_vStreamsInCatchment  

	//location number
	//	-------
	//	|6|7|8|
	//	-------
	//	|5| |1|
	//	-------
	//	|4|3|2|
	//	-------
	// 
	
	long iFlow = 1;
	if (IsEdgeCell(iRow, iCol)) return iFlow;
	for (int iNr = 1; iNr < 9; iNr++)
	{
		long in = iRow + ((iNr >= 2 && iNr <= 4) ? 1 : ((iNr >=6 && iNr <=8) ? -1 : 0));
		long jn = iCol + ((iNr == 1 || iNr == 2 || iNr == 8) ? 1 : ((iNr >= 4 && iNr <= 6) ? -1 : 0));
		bool isFlow = ((m_vFlowDir[in][jn] == (1 + (iNr + 3) % 8)) && (m_vOutput[in][jn] == iUNDEF)); //determine if the neighboring cell flows to the cell in location rc
		BuildUpLinkCatchment(in, jn, 1 + (iNr + 3) % 8, iFlag);
		if (isFlow && (!fExtractOriginalOrder || (m_vStreamMap[in][jn] == -2 || m_vStreamMap[in][jn] == m_iOutletVal)))
			iFlow += MergeCatchment(in, jn, iFlag, fExtractOriginalOrder);
		m_vOutput[iRow][iCol] = iFlag;
		//Identify streams in the merged catchment and put stream IDs into m_vStreamsInCatchment
		if(!fExtractOriginalOrder)
			IdentifyStreamsInCatchment(iRow,iCol);
	}
	return iFlow;
}

void MapCatchmentMerge::CreateTable()
{
	//Create a table associated with the merged catchment map
	//A catchment has a unique ID, which will act as a link to relate with
	//its corresponding drainage. 
	 
	FileName fnTbl(fnObj, ".tbt");
	fnTbl = FileName::fnUnique(fnTbl);
	m_tbl = Table(fnTbl, m_dm);
	SetAttributeTable(m_tbl);


	Column cDrainageID = m_tbl->colNew(String("DrainageID"), Domain("string")); 
  cDrainageID->SetOwnedByTable(true);
  cDrainageID->SetReadOnly(true);
	
	Column cUpstreamLinkCatchment = m_tbl->colNew(String("UpstreamLinkCatchment"), Domain("string"));
  cUpstreamLinkCatchment->SetOwnedByTable(true);
  cUpstreamLinkCatchment->SetReadOnly(true);
	Column cDownstreamLinkCatchment = m_tbl->colNew(String("DownstreamLinkCatchment"), Domain("value"), ValueRange(1,32767,1));
  cDownstreamLinkCatchment->SetOwnedByTable(true);
  cDownstreamLinkCatchment->SetReadOnly(true);
	
  //this column will be used to store the length of segments 
  if(m_UseOutlets)
  {
    Column cDrainageLen = m_tbl->colNew(String("DrainageLen"), Domain("string"));
  }
}

//find the up-link-catchment IDs, 
//store the uplink catchment ID in m_sUpLinkCatchment   
//A catchment has more than one catchment(s) that flow into it.
void MapCatchmentMerge::BuildUpLinkCatchment(long in, long jn, int iFlow, long iFlag)
{
	long iVal = m_vOutput[in][jn];
	if ((m_vFlowDir[in][jn] == iFlow) && (iVal != iUNDEF) && (iVal != iFlag))
	{
			bool IsExists = find(m_vUpCatchmentIDs.begin(), m_vUpCatchmentIDs.end(), iVal) != m_vUpCatchmentIDs.end();
			if (IsExists != true)
			{
					m_vUpCatchmentIDs.push_back(iVal);
					String sUpstreamLink = m_sUpLinkCatchment.length() != 0 ? String(",%li",iVal) : String("%li", iVal);
					m_sUpLinkCatchment &= sUpstreamLink;
			}
	}
}

void MapCatchmentMerge::UpdateUpLinkCatchment(long id)
{
		Column colUpstreamLink = m_tbl->col(String("UpStreamLinkCatchment")); 
		if (m_sUpLinkCatchment.length() == 0)
			m_sUpLinkCatchment = "0";
		else
			m_sUpLinkCatchment = String("{%S}", m_sUpLinkCatchment); 
		colUpstreamLink->PutVal(id, m_sUpLinkCatchment);
}

void MapCatchmentMerge::UpdateDownLinkCatchment(long id)
{
		Column colDownLinkCatchment = m_tbl->col(String("DownStreamLinkCatchment")); 

		vector<long>::iterator pos;
		for (pos = m_vUpCatchmentIDs.begin(); pos < m_vUpCatchmentIDs.end(); ++pos)
		{
			long iUpLinkCatchmentID = *pos;
			colDownLinkCatchment->PutVal(iUpLinkCatchmentID, id);
		}
}

//after merging, a catchment will have more than one streams corresponding with      
//this function will identify the streams in a catchment, put stream's ID into m_vStreamsInCatchment
void MapCatchmentMerge::IdentifyStreamsInCatchment(long iRow, long iCol)
{
	long iVal = m_vStreamMap[iRow][iCol];
	if (iVal != iUNDEF && iVal != m_iOutletVal && iVal > 0)
	{
		bool IsExists = find(m_vStreamsInCatchment.begin(), m_vStreamsInCatchment.end(), iVal) != m_vStreamsInCatchment.end();
		if (IsExists != true)
			m_vStreamsInCatchment.push_back(iVal);
	}
}

void MapCatchmentMerge::AddLink2StreamSegments()
{
	//Add a catchment link for drainage attribute
	Table tblAtt = mp->tblAtt();

	Column colOrder;
	if (fCIStrEqual(m_sOrderSystem, "Strahler"))
		colOrder = tblAtt->col(String("Strahler"));		
	else
		colOrder = tblAtt->col(String("Shreve"));		
	Column colDownStreamLink = tblAtt->col(String("DownstreamLinkID"));
	Column colUpstreamLink = tblAtt->col(String("UpstreamLinkID"));
	Column colDownstreamCoord = tblAtt->col(String("DownStreamCoord"));
	Column colTostreamCoord = tblAtt->col(String("ToStreamCoord"));
	Column colUpstreamCoord = tblAtt->col(String("UpstreamCoord"));
	Column colLength = tblAtt->col(String("Length"));

	DomainSort* pdsrt = colUpstreamLink->dmKey()->pdsrt();
	if ( !pdsrt )
		throw ErrorObject(TR("Source map must have domain class or id"));
	long iSize = pdsrt->iSize();

	m_iMaxOrderNumber = 0;
	DrainageAtt da;
	for (long i=1; i<= iSize ; i++ ) {
		da.ID = pdsrt->iOrd(i); 
		if (da.ID == iUNDEF)
			continue;
		da.UpstreamID.clear();
		SplitString(colUpstreamLink->sValue(i), da.UpstreamID);
		da.iOrder = colOrder->iValue(i);
		if (da.iOrder > m_iMaxOrderNumber)
			m_iMaxOrderNumber = da.iOrder;
		da.iDownStreamID = colDownStreamLink->iValue(i);
		RowCol rc = mp->gr()->rcConv(colDownstreamCoord->cValue(i)); 
		da.DownStreamCoord.Row = rc.Row;
		da.DownStreamCoord.Col = rc.Col;
		rc = mp->gr()->rcConv(colTostreamCoord->cValue(i));
		da.TostreamCoord.Row = rc.Row;
		da.TostreamCoord.Col = rc.Col;
		rc = mp->gr()->rcConv(colUpstreamCoord->cValue(i));
		da.UpstreamCoord.Row = rc.Row;
		da.UpstreamCoord.Col = rc.Col;
		da.CatchmentLink = iUNDEF;
		da.rLenght = colLength->rValue(i);
		m_vDrainageAtt.push_back(da);
	}
}

void MapCatchmentMerge::UpdateLink2StreamSegments(long iCatchmentID, OutletLocation ol)
{
	//Update catchment link for each drainage
	vector<long>::iterator pos;
	for (pos = m_vStreamsInCatchment.begin(); pos < m_vStreamsInCatchment.end(); ++pos)
	{		
			m_vDrainageAtt[(*pos)-1].CatchmentLink = iCatchmentID;
	}	

	//---Catchment table
	String sStreamInCatchment;
	String sStreamsInCatchment;
	for (pos = m_vStreamsInCatchment.begin(); pos < m_vStreamsInCatchment.end(); ++pos)
	{
		sStreamInCatchment = sStreamsInCatchment.length() != 0 ? String(",%li",(*pos)) : String("%li", (*pos));	
		if (*pos > 0)
			sStreamsInCatchment &= sStreamInCatchment;
	}
	sStreamsInCatchment = String("{%S}", sStreamsInCatchment); 	
	Column colDrainageID = m_tbl->col(String("DrainageID"));
	colDrainageID->PutVal(iCatchmentID, sStreamsInCatchment);

  //Also update column for length, this column will be used in Horton Plot function
  if (m_UseOutlets)
  {
    vector<long> vStreamID;
    String sLength;
    vStreamID.clear();
    SplitString(sStreamsInCatchment, vStreamID);
    for (pos = vStreamID.begin(); pos < vStreamID.end(); ++pos)
    {
      String sLen = "0,";
      long iStreamID = (*pos);
      if (iStreamID == ol.StreamID)
      {
           sLen = String("%g,", ol.rLen1);
      }
      else
      {
        double rLen2 = GetSplitSegmentLength( iStreamID );
        if ( rLen2 > 0.001)
          sLen = String("%g,", rLen2);
      }  
      sLength &= sLen;
    }
    sLength.substr(0, sLength.length()-1);
    sLength = String("{%S}", sLength); 	
    Column colLength = m_tbl->col(String("DrainageLen"));
    colLength->PutVal(iCatchmentID, sLength);
  }  
}

bool MapCatchmentMerge::fLatLonCoords()
{
	CoordSystemLatLon* csll = mp->cs()->pcsLatLon();
	return (0 != csll);
}

void MapCatchmentMerge::ComputeCatchmentArea(Table tblHsa)
{
	trq.SetText(TR("Create merged catchment attribute table"));

	//retrive area and perimeter attributes from histogram table 
	Column cArea = tblHsa->col("Area");
	Column cPerimeter = tblHsa->col("Perimeter");

	//update area and perimeter attributes
	Column colCatchmentPerimeter = m_tbl->colNew(String("Perimeter"), Domain("value"), ValueRange(1,1.0e300,0.01));
	Column colCatchmentArea = m_tbl->colNew(String("CatchmentArea"), Domain("value"), ValueRange(1,1.0e300,0.01));
    Column colTotalUpstreamArea = m_tbl->colNew(String("TotalUpstreamArea"), Domain("value"), ValueRange(1,1.0e300,0.01));	
	Column colUpstreamLinkCatchment = m_tbl->col(String("UpstreamLinkCatchment"));
	
	DomainSort* pdsrt = colCatchmentArea->dmKey()->pdsrt();
	if ( !pdsrt )
		throw ErrorObject(TR("Source map must have domain class or id"));
    long iSize = pdsrt->iSize();

	long index = 0;	
	for (long i=1; i<= iSize ; i++ ) 
	{
		long iCatchmentID = pdsrt->iOrd(i); 
		if (iCatchmentID == iUNDEF)
			continue;
        colCatchmentPerimeter->PutVal(iCatchmentID, cPerimeter->rValue(i));
		colCatchmentArea->PutVal(iCatchmentID, cArea->rValue(i));
		
		//Init the field for total upstream catchment area
		vector<long> vLinks;
        vLinks.clear();
		SplitString(colUpstreamLinkCatchment->sValue(i), vLinks);
		if ((vLinks.size() == 1) && (vLinks[0] == 0))
				colTotalUpstreamArea->PutVal(iCatchmentID, colCatchmentArea->rValue(i));
		if (trq.fUpdate(index, iSize)) return;
		index++;
	}
	trq.fUpdate(iSize, iSize);
}

void MapCatchmentMerge::ComputeTotalUpStreamCatchmentArea(Table tblHsa)
{
	Column colUpstreamLinkCatchment = m_tbl->col(String("UpstreamLinkCatchment"));
	Column colCatchmentArea = m_tbl->col(String("CatchmentArea"));
	Column colTotalUpstreamArea = m_tbl->col(String("TotalUpstreamArea"));
	
	DomainSort* pdsrt = colCatchmentArea->dmKey()->pdsrt();
	if ( !pdsrt )
		throw ErrorObject(TR("Source map must have domain class or id"));
    long iSize = pdsrt->iSize();

	bool fComputeTotalArea = true;

	while(fComputeTotalArea)
	{
		fComputeTotalArea = false;
		for (long i=1; i<= iSize ; i++)
		{	
			long iCatchmentID = pdsrt->iOrd(i); 
		  if (iCatchmentID == iUNDEF)
			  continue;
			  	
			if(colTotalUpstreamArea->rValue(i) == rUNDEF)
			{
				fComputeTotalArea = true;
				vector<long> vLinks; //upstream links in vLinks
                vLinks.clear();
				SplitString(colUpstreamLinkCatchment->sValue(i), vLinks);
				double rArea = colCatchmentArea->rValue(i);
				for (vector<long>::iterator pos = vLinks.begin(); 
								 pos < vLinks.end(); ++pos)
				{	
					//search ID in domain, return index, if the ID is found 
					String sLbl("%li", (*pos)); 
					long iRaw = pdsrt->iOrd(sLbl);
					if (colTotalUpstreamArea->rValue(iRaw) == rUNDEF)
						break;
					rArea +=colTotalUpstreamArea->rValue(iRaw);
					colTotalUpstreamArea->PutVal(iCatchmentID, rArea);
				}
			} 
			 if (trq.fUpdate(i, iSize)) return;
		} 
	} //while()
}

RowCol MapCatchmentMerge::GetRowCol(Column col, long iRaw)
{
  RowCol rc = mp->gr()->rcConv(col->cValue(iRaw));
  rc.Row -=1;
  rc.Col -=1;
  return rc;
}

Coord MapCatchmentMerge::ComputeCenterDrainage(long iDrainageID, double rLength, SegmentMap sm)
{
  //Retrieve attributes from drainage table 
	Table tblAtt = mp->tblAtt();
	Column cDownstreamLinkID = tblAtt->col("DownstreamLinkID");
	Column colFlowLength = tblAtt->col("Length");
	DomainSort* pdsrt = colFlowLength->dmKey()->pdsrt();
	if ( !pdsrt )
		throw ErrorObject(TR("Source map must have domain class or id"));

	double rLenDrainage = 0;
	Coord c1;
	if (m_rSourceWaterFlowPathLen > rLength)
	{
		c1 = m_vStream[0];
		for (unsigned long index=0; index < m_vStream.size(); ++index)
		{		
			Coord c2 = m_vStream[index];
			rLenDrainage += rDistance(c1,c2);
			if(rLenDrainage>=rLength)
				return c1;
			else
				c1=c2;
		}
		return c1;
	}	  
	bool fCondition = true;
	while(fCondition && iDrainageID != iUNDEF)
	{
		String sLbl("%li", iDrainageID); 
		long iRaw = pdsrt->iOrd(sLbl);
		rLenDrainage += colFlowLength->rValue(iRaw);
		if(rLenDrainage > rLength)
			fCondition = false;
		else
			iDrainageID = cDownstreamLinkID->iValue(iRaw);
	}
	for (int i=0; i<sm->iFeatures(); ++i)
	{
		ILWIS::Segment *seg = (ILWIS::Segment *)sm->getFeature(i);
		if ( !seg || !seg->fValid())
			continue;

		if (seg->iValue() == iDrainageID)
		{
			CoordinateSequence *crdbufFrom;
			crdbufFrom = seg->getCoordinates();
			c1 = crdbufFrom->getAt(0);
			for (long i=0; i<crdbufFrom->size(); ++i)
			{
				Coord c2 = crdbufFrom->getAt(i);
				rLenDrainage += rDistance(c1,c2);
				if(rLenDrainage>=rLength)
					return c1;
				else
					c1=c2;
			}
			delete crdbufFrom;
		}

	}
	return c1;
}

void MapCatchmentMerge::ComputeOtherAttributes()
{
  //Add fields to the table for the output catchments
  Column cTotalDrainageLength = m_tbl->colNew(String("TotalDrainageLength"), Domain("value"), ValueRange(0,1.0e300,0.01));
  Column cDrainageDensity = m_tbl->colNew(String("DrainageDensity(m/km2)"), Domain("value"), ValueRange(0,1.0e300,0.01));
  Column cLongestFlowPathLength = m_tbl->colNew(String("LongestFlowPathLength"), Domain("value"), ValueRange(0,1.0e300,0.01));
  Column cLongestDrainageLength = m_tbl->colNew(String("LongestDrainageLength"), Domain("value"), ValueRange(0,1.0e300,0.01));
  Domain dmcrd;
	dmcrd.SetPointer(new DomainCoord(mp->cs()->fnObj));
  Column colCenterDrainage = m_tbl->colNew(String("CenterDrainage"), dmcrd);
  Column colOutletCoord = m_tbl->colNew(String("OutletCoord"), dmcrd);
  Column colOutletElevation = m_tbl->colNew(String("OutletElevation"), Domain("value"), ValueRange(0,1.0e300,0.01));
  Column colLFPUpstreamCoord = m_tbl->colNew(String("LFPUpstreamCoord"), dmcrd);
  Column colLFPUpstreamElevation = m_tbl->colNew(String("LFPUpstreamElevation"), Domain("value"), ValueRange(0,1.0e300,0.01));
  Column colLDPUpstreamCoord = m_tbl->colNew(String("LDPUpstreamCoord"), dmcrd);
  Column colLDPUpstreamElevation = m_tbl->colNew(String("LDPUpstreamElevation"), Domain("value"), ValueRange(0,1.0e300,0.01));
  
  //Retrieve attributes from catchment table
  Column colDrainageID = m_tbl->col(String("DrainageID"));
  Column colArea = m_tbl->col(String("CatchmentArea"));
  Column colDrainageLen;
  if (m_UseOutlets)
    colDrainageLen = m_tbl->col(String("DrainageLen"));

  //Retrieve attributes from drainage table 
  Table tblAtt = mp->tblAtt();
  Column colFlowLength = tblAtt->col("Length");
  Column cDownstreamLinkID = tblAtt->col("DownstreamLinkID");
  Column cUpstreamCoord = tblAtt->col("UpstreamCoord");
  Column cDownstreamCoord = tblAtt->col("DownstreamCoord");
  Column cTostreamCoord = tblAtt->col("TostreamCoord");
	
  DomainSort* pdsrtCatchment = colDrainageID->dmKey()->pdsrt();
  if ( !pdsrtCatchment )
		throw ErrorObject(TR("Source map must have domain class or id"));
  long iCatchments = pdsrtCatchment->iSize();

  DomainSort* pdsrtDrainage = colFlowLength->dmKey()->pdsrt();
  if ( !pdsrtDrainage )
		throw ErrorObject(TR("Source map must have domain class or id"));

  FileName fnSrcSeg = mp->fnObj; //input segments 
  fnSrcSeg.sExt = ".mps";
  SegmentMap smSource = SegmentMap(fnSrcSeg);

  AttLongestPath FlowPathAtt;
  vector<AttLongestPath> vFlowPathAtt;
  trq.SetText(TR("Generate Catchment Attributes"));
  for (long i=1; i<= iCatchments ; i++)
  {	
    double rTotlaDrainageLen = 0; 
    double rLenDownDrainages;
    vector<double> vLenDownDrainages;
	vLenDownDrainages.resize(0);

    //Get drainages per catchment
	vector<long> vDrainageIDs; 
    vDrainageIDs.clear();
	SplitString(colDrainageID->sValue(i), vDrainageIDs);
	if (vDrainageIDs.size() <= 0)
		continue;

    //Retrieve lengths per drainage per catchment  
    vector<double> vLenPerDrainage;
    if (m_UseOutlets)
      SplitString(colDrainageLen->sValue(i), vLenPerDrainage);
	if (vLenPerDrainage.size() <= 0)
		continue;
    
    long iRaw;
    //Compute the total drainage length per catchment
    //Compute down stream drainage length per drainage 
	for (vector<long>::iterator pos=vDrainageIDs.begin(); pos<vDrainageIDs.end(); ++pos)
	{	
	  String sLbl("%li", (*pos)); 
	  iRaw = pdsrtDrainage->iOrd(sLbl);
	  if (colFlowLength->rValue(iRaw) == rUNDEF)
		break;
     
      //The sum of the drainage lengths for each drainage line in the catchment rTotlaDrainageLen
      //Compute the total lengths of the up drainages till to the outlet rLenUpDrainages 
      long iIndex = (long)(pos - vDrainageIDs.begin());
      if ((m_UseOutlets == true) && (vLenPerDrainage[iIndex] > 0.0))
      {
        rTotlaDrainageLen += vLenPerDrainage[iIndex];
        rLenDownDrainages = vLenPerDrainage[iIndex];
      }
      else
      {
        rTotlaDrainageLen += colFlowLength->rValue(iRaw);
        rLenDownDrainages = colFlowLength->rValue(iRaw);
      }  

      long iDownFlowID = cDownstreamLinkID->iValue(iRaw);
      bool IsExist;
      if (iDownFlowID != iUNDEF)
      {
        do
        {
          vector<long>::iterator posID = find(vDrainageIDs.begin(), vDrainageIDs.end(), iDownFlowID);
          IsExist = (posID != vDrainageIDs.end());
          if(IsExist)
          {
            sLbl = String("%li", iDownFlowID); 
			      iRaw = pdsrtDrainage->iOrd(sLbl);
			
            iIndex = (long)(posID - vDrainageIDs.begin());
            if ((m_UseOutlets == true) && (vLenPerDrainage[iIndex] > 0.0))
              rLenDownDrainages += vLenPerDrainage[iIndex];
            else
              rLenDownDrainages += colFlowLength->rValue(iRaw);
            iDownFlowID = cDownstreamLinkID->iValue(iRaw);
          }

        }while(IsExist && iDownFlowID != iUNDEF);
      }
      vLenDownDrainages.push_back(rLenDownDrainages);
	}
		
    vector<double>::iterator posMax = max_element(vLenDownDrainages.begin(), vLenDownDrainages.end());
	double rLongestDrainageLength = (*posMax);

    //Update longest drainage length, total drainage length, drainage density attributes
	cLongestDrainageLength->PutVal(i, rLongestDrainageLength);
    cTotalDrainageLength->PutVal(i,rTotlaDrainageLen);
    double rDensity = (rTotlaDrainageLen / colArea->rValue(i)) * 1000000;
    cDrainageDensity->PutVal(i, rDensity);

    //Find the headwater drainage for the longest drainage path
    long iIndex = (long)(posMax -  vLenDownDrainages.begin());
    long iSourceID = vDrainageIDs[iIndex];
    
    String sLbl("%li", iSourceID); 
	iRaw = pdsrtDrainage->iOrd(sLbl);
    RowCol rcDownstream = mp->gr()->rcConv(cDownstreamCoord->cValue(iRaw)); //GetRowCol(cDownstreamCoord, iRaw);
    RowCol rcUpstream = mp->gr()->rcConv(cUpstreamCoord->cValue(iRaw)); //GetRowCol(cUpstreamCoord, iRaw);
    colLDPUpstreamCoord->PutVal(i, mp->gr()->cConv(rcUpstream));
    double rHeight = rGetElevation(rcUpstream);
    colLDPUpstreamElevation->PutVal(i, rHeight);
    
    if(m_vOutput[rcDownstream.Row][rcDownstream.Col] != i && m_UseOutlets)
    { 
      //Locate the downstream cell starting at upstream cell
      RowCol rc = rcUpstream;
      RowCol rcDownCell;
      while(m_vOutput[rc.Row][rc.Col]==i)
      {
        GetDownStreamCell(rc);
      }
      rcDownstream = rc;
    }
    //Construct headwater drainage segment per catchment at downstream cell
    InitPars();
    m_vStream.resize(0);
    m_rSourceWaterFlowPathLen=0;
    m_vStream.push_back(mp->gr()->cConv(rcDownstream));
	if(m_vOutlet[i-1].fExtractOverlandFlowPath)
		ExtractUpstreamFlowPath(rcDownstream, i);
	else
	{
		m_rSourceWaterFlowPathLen = rLongestDrainageLength;
		long streamid = m_vOutlet[i-1].StreamID;
		m_vStream.push_back(mp->gr()->cConv(m_vDrainageAtt[streamid-1].DownStreamCoord));
		m_vStream.push_back(mp->gr()->cConv(m_vDrainageAtt[streamid-1].UpstreamCoord));
		String sidLbl("%li", streamid); 
	    iRaw = pdsrtDrainage->iOrd(sidLbl);
		Coord cdDownstreamCoord = StoreSegment(smSource,iRaw,i);
	}

    //Compute the longest flow path length
    double rLongestFlowPathLength;
    if ((m_UseOutlets == true) && (vLenPerDrainage[iIndex] > 0.0))
      rLongestFlowPathLength = rLongestDrainageLength - vLenPerDrainage[iIndex] + m_rSourceWaterFlowPathLen;
    else
      rLongestFlowPathLength = rLongestDrainageLength - colFlowLength->rValue(iRaw) + m_rSourceWaterFlowPathLen;
    cLongestFlowPathLength->PutVal(i, rLongestFlowPathLength);
    colLFPUpstreamCoord->PutVal(i, m_vStream[m_vStream.size()-1]);
    rHeight = rGetElevation(mp->gr()->rcConv(m_vStream[m_vStream.size()-1]));
    colLFPUpstreamElevation->PutVal(i, rHeight);

    Coord crdCenterDrainage = ComputeCenterDrainage(iSourceID,rLongestFlowPathLength/2, smSource);
    colCenterDrainage->PutVal(i,crdCenterDrainage);
    
    colOutletCoord->PutVal(i, mp->gr()->cConv(m_vOutlet[i-1].rc));
    rHeight = rGetElevation(m_vOutlet[i-1].rc);
    colOutletElevation->PutVal(i, rHeight);
    
    if(m_sLongestFlowPathSegmentMap.length() > 0)
    { 
      FlowPathAtt.UpstreamCoord = m_vStream[m_vStream.size()-1];
	  FlowPathAtt.DownstreamCoord = m_vStream[0];
      //Coord cdDownstreamCoord;
      //StoreSourceSegment(rLongestFlowPathLength);
	  if(m_vOutlet[i-1].fExtractOverlandFlowPath)
		StoreSourceSegment(i);
      double rFlowPathLen = m_rSourceWaterFlowPathLen;
      long iDownFlowID = cDownstreamLinkID->iValue(iRaw);
      bool IsExist;
      if (iDownFlowID != iUNDEF)
      {
        do
        {
          vector<long>::iterator posID = find(vDrainageIDs.begin(), vDrainageIDs.end(), iDownFlowID);
          IsExist = (posID != vDrainageIDs.end());
          if(IsExist)
          {
            sLbl = String("%li", iDownFlowID); 
			iRaw = pdsrtDrainage->iOrd(sLbl);
			iIndex = (long)(posID - vDrainageIDs.begin());
            if ((m_UseOutlets == true) && (vLenPerDrainage[iIndex] > 0.0))
            {
              rFlowPathLen += vLenPerDrainage[iIndex];
              RowCol rcUpstream = GetRowCol(cUpstreamCoord, iRaw);
              FlowPathAtt.DownstreamCoord = SplitSegment(smSource,iRaw,rLongestFlowPathLength,i, rcUpstream);
            }
            else
            {
              //StoreSegment(smSource,iRaw,rLongestFlowPathLength);
              FlowPathAtt.DownstreamCoord = StoreSegment(smSource,iRaw,i);
              rFlowPathLen += colFlowLength->rValue(iRaw);
            }
            iDownFlowID = cDownstreamLinkID->iValue(iRaw);
          }
        }while(IsExist && iDownFlowID != iUNDEF);
      }
      FlowPathAtt.rLength = rLongestFlowPathLength;
      vFlowPathAtt.push_back(FlowPathAtt);
    }
    if (trq.fUpdate(i, iCatchments)) return;
  }
  if(m_sLongestFlowPathSegmentMap.length() > 0)
  {
    CreateTableLongestFlowPath(m_segLongestSegmap->fnObj, vFlowPathAtt);
    vFlowPathAtt.resize(0);
  }
}

double MapCatchmentMerge::rGetElevation(RowCol rc)
{
  LongBuf lbuf;
  lbuf.Size(iCols());
  m_mpDEM->GetLineVal(rc.Row, lbuf);
  double rHeight = lbuf[rc.Col];
  return rHeight;  
}

void MapCatchmentMerge::CreateTableLongestFlowPath(FileName fn, vector<AttLongestPath> vAtt)
{
  fn.sExt = ".tbt";
  fn = FileName::fnUnique(fn);
	Table tbl = Table(fn, m_dm);
  m_segLongestSegmap->SetAttributeTable(tbl);

  Domain dmcrd;
	dmcrd.SetPointer(new DomainCoord(mp->cs()->fnObj));
  Column cUpstreamCoord = tbl->colNew(String("UpstreamCoord"), dmcrd);
  Column cDownstreamCoord = tbl->colNew(String("DownstreamCoord"), dmcrd);
  Column cLength = tbl->colNew(String("Length"), Domain("value"));
  Column cStraightLength = tbl->colNew(String("StraightLength"), Domain("value"));
  Column cSinuosity = tbl->colNew(String("Sinuosity"), Domain("value"), ValueRange(0,1e10,0.001));

  long i = 0;
  for (vector<AttLongestPath>::iterator pos = vAtt.begin(); pos < vAtt.end(); ++pos)
  {
    i++;
    AttLongestPath atts = (*pos);
    cUpstreamCoord->PutVal(i, atts.UpstreamCoord);
    cDownstreamCoord->PutVal(i, atts.DownstreamCoord);
    cLength->PutVal(i, atts.rLength);
    double rStraightLength = rDistance(atts.UpstreamCoord,atts.DownstreamCoord);
    cStraightLength->PutVal(i, rStraightLength);
    double rSinuousity = rComputeSinuosity(atts.rLength, rStraightLength);
    cSinuosity->PutVal(i, rSinuousity);
  }
} 

double MapCatchmentMerge::rComputeSinuosity(double rLength, double rStraightLenght)
{
	if (rStraightLenght > 0)
	 return rLength/rStraightLenght;
	else
	 return 0;
}

Coord MapCatchmentMerge::StoreSegment(SegmentMap smpFrom, long id, long val)
{
	Coord cd;
	for (int i =0; i < smpFrom->iFeatures(); ++i)
	{
		ILWIS::Segment *segFrom = (ILWIS::Segment *)smpFrom->getFeature(i);
		if ( !segFrom || !segFrom->fValid())
			continue;

		if (segFrom->iValue() == id)
		{
			CoordinateSequence *crdbufFrom;
			crdbufFrom = segFrom->getCoordinates();

			ILWIS::Segment * segTo = CSEGMENT(m_segLongestSegmap->newFeature());
			segTo->PutCoords(crdbufFrom->clone());
			//segTo.PutVal(val);	
			segTo->PutVal(val);
			cd = crdbufFrom->getAt(crdbufFrom->size()-1);
			delete crdbufFrom;
		}

	}
	return cd; 
}

Coord MapCatchmentMerge::SplitSegment(SegmentMap smpFrom, long iRaw, double val, long id, RowCol rc)
{
  Coord cd;  
  for (int i =0; i < smpFrom->iFeatures(); ++i)
	{
		ILWIS::Segment *segFrom = (ILWIS::Segment *)smpFrom->getFeature(i);
		if ( !segFrom || !segFrom->fValid())
			continue;
        if (segFrom->iValue() == iRaw)
        {
          CoordinateSequence *crdbufFrom;
  		  crdbufFrom = segFrom->getCoordinates();
        
          int iFlow = 1;
          bool fCalculate = true;
          long iCount = 1;
          while((iFlow != 0) && fCalculate)
          {
            fCalculate = IsEdgeCell(rc.Row, rc.Col) != true; 
            iFlow = GetDownStreamCell(rc);
            if (m_vOutput[rc.Row][rc.Col] == id && fCalculate && iFlow != 0)   //valid flow
		          iCount++;
            else
              fCalculate = false;
          }
          ILWIS::Segment * segTo = CSEGMENT(m_segLongestSegmap->newFeature());
		  segTo->PutCoords(iCount-1, CoordBuf(crdbufFrom));
		  segTo->PutVal(id);
		  cd = crdbufFrom->getAt(iCount-1);
		  delete crdbufFrom;
        }
	}
  return cd;
}

void MapCatchmentMerge::ExtractUpstreamFlowPath(RowCol rc, long id)
{
    RowCol pos;
    RowCol rcUpstream;
    int iNb = 0;
    long iAcc = 0;
    for (int i=-1; i<=1; ++i) 			//Evaluate neighbors for cell at rc
	{		
		pos.Row = rc.Row + i;
		for(int j=-1; j<=1; ++j)		
		{
			pos.Col = rc.Col + j;
		    int iFlow = m_vFlowDir[pos.Row][pos.Col];
			int iFlowTo = m_vFlowSelf[iNb];
			if (m_vFlowSelf[iNb] == m_vFlowDir[pos.Row][pos.Col])
			{
				if(m_vFAcc[pos.Row][pos.Col] > iAcc && m_vOutput[pos.Row][pos.Col] == id )
				{
					iAcc = m_vFAcc[pos.Row][pos.Col];
					rcUpstream.Row=pos.Row;
					rcUpstream.Col=pos.Col;
				}
			}	
			iNb++;
		}
	}

    if((!rcUpstream.fUndef()) && (m_vOutput[rcUpstream.Row][rcUpstream.Col] == id))
    {
      Coord c1 = mp->gr()->cConv(rc);
	  Coord c2 = mp->gr()->cConv(rcUpstream);
	  m_rSourceWaterFlowPathLen += rDistance(c1 ,c2);
      m_vStream.push_back(c2);
      ExtractUpstreamFlowPath(rcUpstream, id);
    }
}

void MapCatchmentMerge::StoreSourceSegment(long val)
{
		ILWIS::Segment * seg;
		CoordBuf crdBuf((int)(m_vStream.size()));
	
		for (unsigned long index=0; index < m_vStream.size(); ++index)
		{		
			  crdBuf[index] = m_vStream[index];
		}
		if (m_vStream.size() > 0)
		{
			seg = CSEGMENT(m_segLongestSegmap->newFeature());
			seg->PutCoords(crdBuf.iSize(), crdBuf);
			seg->PutVal(val);	
		}
}

void MapCatchmentMerge::ComputeCenterPolygon(FileName fn)
{
	//First lable point map of the polygon
	//then, put the point coordinates to the catchment attribute table
	trq.SetText(TR("Compute the center of polygon"));
	FileName fnTmpPoint(fnObj, ".mpp");
	fnTmpPoint = FileName::fnUnique(fnTmpPoint);
	String sExpr("PointMapPolLabels(%S)", fn.sFullPathQuoted(false)); 
	PointMap ptTmpMap; 
	ptTmpMap = PointMap(fnTmpPoint, sExpr);
	ptTmpMap->Calc();
	
	Domain dmcrd;
	dmcrd.SetPointer(new DomainCoord(ptTmpMap->cs()->fnObj));
	Column cCenterCatchment = m_tbl->colNew(String("CenterCatchment"), dmcrd);
	
	Coord crd;
	long iPoint = ptTmpMap->iFeatures();
	for (long i=0; i < iPoint; ++i) {
		long iRaw = ptTmpMap->iRaw(i); 
		if (iRaw == iUNDEF)
			continue;
		Coord crd = ptTmpMap->cValue(i);
		cCenterCatchment->PutVal(iRaw, crd);
		if (trq.fUpdate(i, iPoint)) return;
	}
	ptTmpMap->fErase = true;
}

void MapCatchmentMerge::PutOutlet(long id, RowCol rc, long iFlag, bool fExtractOverlandFlowPath)
{
	OutletLocation outlet;
	outlet.StreamID = id;
	outlet.rc.Row = rc.Row;
	outlet.rc.Col = rc.Col;
	outlet.StreveOrder = iFlag;
	outlet.fExtractOverlandFlowPath = fExtractOverlandFlowPath;
	outlet.isOnNode = false;
	outlet.rLen1 = rUNDEF;
	outlet.rLen2 = rUNDEF;
	//if (IsOutletExists(m_vOutlet, outlet.rc) == false)
	m_vOutlet.push_back(outlet);
}

bool MapCatchmentMerge::IsJointOutlet(long id)
{
	//---Return true, if downstream order greater than the specified order, or downstream order is undefined  
	bool fCheck = false;
	if (m_vDrainageAtt[id-1].iOrder > m_iStreamOrders || m_vDrainageAtt[id-1].iOrder == iUNDEF)
	{
		fCheck = true;
	}	
	return fCheck;
}

bool MapCatchmentMerge::IsUpstreamsMerged(vector<long> vUpstreams)
{
	//---Return true, if it's upstreams have been merged  
	bool fCheck = true;
	for (vector<long>::iterator pos = vUpstreams.begin(); pos < vUpstreams.end(); ++pos)
	{
			if ((m_vDrainageAtt[(*pos)-1].iOrder > m_iStreamOrders) && (m_vDrainageAtt[(*pos)-1].CatchmentLink == iUNDEF))
			{
				fCheck = false;
				break;
			}	
	}
	return fCheck;
}

//---Scan the stream topology for joint outlet locations by order
//---Put the joint outlet locations in m_vOutlet for merging process  
void MapCatchmentMerge::EvaluateJointOutletsbyOrder()
{
	//Catch based on stream connectivity topology info.
	m_vOutlet.resize(0);
	for (vector<DrainageAtt>::iterator pos = m_vDrainageAtt.begin(); pos < m_vDrainageAtt.end(); ++pos)
	{
		DrainageAtt datt = (*pos);
		
		if(datt.iOrder == m_iStreamOrders)
		{
			//---Determine the joint outlet 
			long downstreamID = datt.iDownStreamID;
			//if (downstreamID==339)
			//	long id = m_vDrainageAtt[downstreamID-1].iOrder;
			if (m_vDrainageAtt[downstreamID-1].iOrder > m_iStreamOrders)
			{
				PutOutlet(datt.ID, datt.TostreamCoord, iUNDEF, true);
			}
			else if (datt.iDownStreamID == iUNDEF || datt.iDownStreamID == shUNDEF)
			{
				PutOutlet(datt.ID, datt.DownStreamCoord,iUNDEF, true);
			}
		}
	} 
}

void MapCatchmentMerge::MergeCatchmentsRemained(long& id)
{
	for (vector<DrainageAtt>::iterator pos = m_vDrainageAtt.begin(); pos < m_vDrainageAtt.end(); ++pos)
	{
		DrainageAtt datt = (*pos);
		if ((datt.iOrder == m_iStreamOrders) && (datt.CatchmentLink == iUNDEF) && (datt.iDownStreamID == iUNDEF))
		{
			id++;
            AddDomainItem(m_dm, id); 
			Merge(id, datt,false);
			RowCol rc = datt.DownStreamCoord;
			PutOutlet(datt.ID, rc, iUNDEF, false);			
		}
	}

	//---If the draiange order number equal to the specified order number,
	//---then merge its down-flow draianges joint at the down-stream-coord of the drainage.  
	for ( vector<DrainageAtt>::iterator pos = m_vDrainageAtt.begin(); pos < m_vDrainageAtt.end(); ++pos)
	{
		DrainageAtt datt = (*pos);
		if ((datt.iOrder == m_iStreamOrders) && (datt.CatchmentLink == iUNDEF) &&
         (m_vDrainageAtt[datt.iDownStreamID-1].iOrder > m_iStreamOrders))
		{
          id++;
          AddDomainItem(m_dm, id); 
	      Merge(id, datt,false);
		  RowCol rc = datt.DownStreamCoord;
		  PutOutlet(datt.ID, rc, iUNDEF,false);
		}
	}
}

void MapCatchmentMerge::ExtractOriginalOrder(long& id)
{
	trq.SetText("Extract Original Order");
	bool fContinue = true;
	while(fContinue)
	{
		fContinue = false;
		long iCont=0;
		for (vector<DrainageAtt>::iterator pos = m_vDrainageAtt.begin(); pos < m_vDrainageAtt.end(); ++pos)
		{
			DrainageAtt datt = (*pos);  //This is the current drainage to be evaluated
			vector<long>  vUpStreamLink = datt.UpstreamID;
			if (datt.CatchmentLink == iUNDEF && IsUpstreamsMerged(vUpStreamLink)) 
			{
				fContinue = true;
				id++;
				AddDomainItem(m_dm, id);
				Merge(id, datt, true);
				RowCol rc = datt.DownStreamCoord;
				bool fExtractOverlandFlowPath = true;
				if (datt.UpstreamID[0] != 0)  
					fExtractOverlandFlowPath = false;
				PutOutlet(datt.ID, rc, iUNDEF, fExtractOverlandFlowPath);
			}
			iCont++;
			if (trq.fUpdate(iCont, (long)m_vDrainageAtt.size())) return;
		}
	}
}


void MapCatchmentMerge::Merge(long id, DrainageAtt datt, bool fExtractOriginalOrder)
{
	m_vStreamsInCatchment.resize(0);
	m_vUpCatchmentIDs.resize(0);
	m_sUpLinkCatchment = String("");
	//RowCol rc = datt.DownStreamCoord; 
    //m_vStreamsInCatchment.push_back(m_vStreamMap[datt.DownStreamCoord.Row][datt.DownStreamCoord.Col]);
    RowCol rc2 = datt.TostreamCoord; 
    //m_vStreamsInCatchment.push_back(m_vStreamMap[rc2.Row][rc2.Col]);
	m_vStreamsInCatchment.push_back(datt.ID);
	m_iOutletVal = datt.ID; //m_vStreamMap[rc2.Row][rc2.Col];
	MergeCatchment(rc2.Row, rc2.Col, id, fExtractOriginalOrder);
	UpdateUpLinkCatchment(id);
	UpdateDownLinkCatchment(id);	
    OutletLocation ol;
	UpdateLink2StreamSegments(id, ol);
}

String MapCatchmentMerge::BuildMaskIDs()
{
  Column colDrainageID = m_tbl->col(String("DrainageID"));
	DomainSort* pdsrt = colDrainageID->dmKey()->pdsrt();
	if ( !pdsrt )
		throw ErrorObject(TR("Source map must have domain class or id"));
  long iSize = pdsrt->iSize();

  String sMask = "";
  for (long i=1; i<= iSize ; i++ ) {
		long iID = pdsrt->iOrd(i); 
		if (iID == iUNDEF)
			continue;
    String sID = colDrainageID->sValue(i);
    sID = sID.sTrimSpaces();
    sMask &= sID.substr(1, sID.length()-2);
    if (i < iSize )
      sMask &= ",";
  }
  //sMask.substr(0, sMask.length()-1);
  //return sMask & "'";
  return sMask;
}

double MapCatchmentMerge::rDistance(Coord cd1, Coord cd2)
{
	double rDist;
	if (fLatLonCoords())
	{
			double rRadi = rDefaultEarthRadius;
			if(mp->cs()->pcsViaLatLon())
				rRadi = mp->cs()->pcsViaLatLon()->ell.a;
			
			LatLon llStart = LatLon(cd1.y, cd1.x);
			LatLon llEnd  = LatLon(cd2.y, cd2.x);
			if (fEllipsoidalCoords()) 
			{
				CoordSystemViaLatLon* pcsvll= mp->cs()->pcsViaLatLon();
				if ( (llStart.Lat == llEnd.Lat) && (llStart.Lon == llEnd.Lon))
					rDist = 0; //seems a bug in rEllipsoidalDistance, always get some value, even when llStart and llEnd the same? 
				else
					rDist = pcsvll->ell.rEllipsoidalDistance(llStart, llEnd);
				if (rDist < 8000)
					return rDist;
			}
			rDist = rSphericalDistance(rRadi, llStart, llEnd);
	}
	else
	{	
		double dx = (cd1.x - cd2.x);
		double dy = (cd1.y - cd2.y);
		rDist = sqrt(dx * dx + dy * dy);
	}
	return rDist;
}

bool MapCatchmentMerge::fEllipsoidalCoords()
{
	CoordSystemViaLatLon* csviall = mp->cs()->pcsViaLatLon();
	bool fSpheric = true;
	if (csviall)
		fSpheric= (csviall->ell.fSpherical());
	return (0 != csviall &&  0 == fSpheric);
}

int MapCatchmentMerge::GetDownStreamCell(RowCol& rc)
{
	//Return a cell that the current given cell rc flows to,
	//otherwise, return the same cell as the given cell, This means
	//that the given cell doesn't flow to any other cell.   

	int iPos = m_vFlowDir[rc.Row][rc.Col];
	switch (iPos)
	{
				case 1: 	//East
						rc.Row = rc.Row;
						rc.Col = rc.Col+1;
						break;
				case 2:  //South East 
						rc.Row = rc.Row+1;
						rc.Col = rc.Col+1;
						break;
				case 3: 	//South
						rc.Row = rc.Row+1;
						rc.Col = rc.Col;
						break;
				case 4: //South West
						rc.Row = rc.Row+1;
						rc.Col = rc.Col-1;
						break;
				case 5:	//West
						rc.Row = rc.Row;
						rc.Col = rc.Col-1;
						break;
				case 6:	//North West 
						rc.Row = rc.Row-1;
						rc.Col = rc.Col-1;
						break;
				case 7:	//North
						rc.Row = rc.Row-1;
						rc.Col = rc.Col;
						break;
				case 8:	//North East
						rc.Row = rc.Row-1;
						rc.Col = rc.Col+1;
						break;
        default:
          iPos = 0;
	}
	return iPos;
}


double MapCatchmentMerge::CalculateLength(RowCol rc1, RowCol rc2, long iDrainage)
{
  double rLength = 0;
  RowCol rcDownCell = rc1;
  int iFlow = 1; //anyway should do onece
  bool fCalculate = true;

  Coord cd = mp->gr()->cConv(rc1);
  m_vStreamCoord.push_back(cd);

  while((iFlow != 0) && fCalculate)
  {
    fCalculate = ((rcDownCell != rc2) && (IsEdgeCell(rcDownCell.Row, rcDownCell.Col) != true)); 
    iFlow = GetDownStreamCell(rcDownCell);
    if ((m_vStreamMap[rcDownCell.Row][rcDownCell.Col] == iDrainage) &&
			(m_vFlowDir[rcDownCell.Row][rcDownCell.Col] != 0))   //valid flow
		{
      Coord c1 = mp->gr()->cConv(rc1);
		  Coord c2 = mp->gr()->cConv(rcDownCell);
		  rLength = rLength + rDistance(c1 ,c2);
      rc1 = rcDownCell;
      m_vStreamCoord.push_back(c2);
    }			
  }
  return rLength;
}

void MapCatchmentMerge::CleanSegment(SegmentMap smpTo, SegmentMap smpFrom)
{
	vector<RowCol> vOutlet;
  vector<long> vOutletID;
  trq.SetText(TR("Extract stream segments"));
  //check if the outlet has a downflow to catchment merged
  //if no, the rest of the segment at the outlet should be removed from
  //otherwise, just copy the complete segment
  for (vector<OutletLocation>::iterator pos = m_vOutlet.begin(); pos < m_vOutlet.end(); ++pos)
  {		
	  RowCol rc = pos->rc;
	  int iFlow = GetDownStreamCell(rc);
	  if(m_vOutput[rc.Row][rc.Col] == iUNDEF )
	  {
		  vOutlet.push_back(pos->rc);
		  vOutletID.push_back(pos->StreamID);
	  }
  }

  std::map<long, long> mapSrcDstIDs; // keep track of the mapping of the source-segment-ids to the destination-segment-ids
  for (int i =0; i < smpFrom->iFeatures(); ++i)
  {
	  ILWIS::Segment *segFrom = (ILWIS::Segment *)smpFrom->getFeature(i);
	  if ( !segFrom || !segFrom->fValid())
		  continue;
	  long iSegVal = segFrom->iValue(); // some iSegVals will be duplicates; only add an item to the domain if its not a duplicate
	  long iDestSegVal;
	  map<long, long>::iterator item = mapSrcDstIDs.find(iSegVal);
	  if (item == mapSrcDstIDs.end()) {
		  iDestSegVal = mapSrcDstIDs.size() + 1;
		  mapSrcDstIDs[iSegVal] = iDestSegVal;
		  AddDomainItem(smpTo->dm(), iDestSegVal);
	  } else {
		  iDestSegVal = item->second;
	  }	  
	  long iNumC2 = 1;
	  CoordinateSequence *crdbufFrom;
      crdbufFrom = segFrom->getCoordinates();
	  ILWIS::Segment * segTo = CSEGMENT(smpTo->newFeature());
	  vector<long>::iterator pos = find(vOutletID.begin(), vOutletID.end(), iSegVal);
	  if (pos != vOutletID.end())
	  {
		  int iIndex = (long)(pos - vOutletID.begin());
		  RowCol rcFrom = m_vDrainageAtt[iSegVal-1].UpstreamCoord;
		  RowCol rcOutlet = vOutlet[iIndex]; 
		  for (int i = 0; i < crdbufFrom->size(); ++i) 
		  {
			  int iFlow = GetDownStreamCell(rcFrom);
			  if (rcFrom != rcOutlet)
				  iNumC2++;
			  else
			  {
				  iNumC2++;  //fix to include the outlet point
				  break;
			  }
		  }
	  }
	  else 
		  iNumC2 = crdbufFrom->size();
	  segTo->PutCoords(iNumC2, CoordBuf(crdbufFrom));
	  segTo->PutVal(iDestSegVal);
	  trq.fUpdate(i, smpFrom->iFeatures());
	  delete crdbufFrom;
  }
  trq.fUpdate(smpFrom->iFeatures(), smpFrom->iFeatures());
}

void MapCatchmentMerge::CreateTableSegmentsExtracted(SegmentMap sm)
{
  //Create a table to store the attributes for the segments extracted
  trq.SetText(TR("Extract stream segment attributes"));
  //FileName fnTbl(fnObj, ".tbt");
	//fnTbl = FileName::fnUnique(fnTbl);
	//m_tbl = Table(fnTbl, m_dm);
  //SetAttributeTable(m_tbl);
  FileName fnTbl(fnObj, ".tbt");
  fnTbl = FileName::fnUnique(fnTbl);
  Table tbl = Table(fnTbl, sm->dm());
  sm->SetAttributeTable(tbl);

  Table tblSource = mp->tblAtt();
	Column cUpstreamLinkSrc = tblSource->col(String("UpstreamLinkID"));
  Column cUpstreamCoordSrc = tblSource->col(String("UpstreamCoord"));
  Column cUpstreamHeightSrc = tblSource->col(String("UpstreamElevation"));
  Column cDownstreamLinkSrc = tblSource->col(String("DownstreamLinkID"));
  Column cDownstreamCoordSrc = tblSource->col(String("DownstreamCoord"));
  Column cDownstreamHeightSrc = tblSource->col(String("DownstreamElevation"));
  Column cDropSrc = tblSource->col(String("ElevationDifference"));
  Column cStrahlerSrc = tblSource->col(String("Strahler"));
  Column cStrahlerClassSrc = tblSource->col(String("StrahlerClass"));
  Column cStreveSrc = tblSource->col(String("Shreve"));
  Column cLengthSrc = tblSource->col(String("Length"));
  Column cStraightLengthSrc = tblSource->col(String("StraightLength"));
  Column cSlopAlongDrainageSrc = tblSource->col(String("SlopeAlongDrainagePerc"));
  Column cSlopAlongDrainageDegreeSrc = tblSource->col(String("SlopeAlongDrainageDegree"));
  Column cSlopDrainageStraightSrc = tblSource->col(String("SlopeDrainageStraightPerc"));
  Column cSlopDrainageStraightDegreeSrc = tblSource->col(String("SlopeDrainageStraightDegree"));
  Column cSinuositySrc = tblSource->col(String("Sinuosity"));
  Column cTotalUpstreamLengthSrc = tblSource->col(String("TotalUpstreamAlongDrainageLength"));

  Domain dmcrd;
	dmcrd.SetPointer(new DomainCoord(mp->cs()->fnObj));
  Column cUpstreamLink = tbl->colNew(String("UpstreamLinkID"), Domain("string")); 
	Column cUpstreamCoord = tbl->colNew(String("UpstreamCoord"), dmcrd);
	Column cUpstreamHeight = tbl->colNew(String("UpstreamElevation"), Domain("value"));
	Column cDownstreamLink = tbl->colNew(String("DownstreamLinkID"), Domain("value"), ValueRange(1,32767,1));

	Column cDownstreamCoord = tbl->colNew(String("DownstreamCoord"), dmcrd);
	Column cDownstreamHeight = tbl->colNew(String("DownstreamElevation"), Domain("value"));
	Column cDrop = tbl->colNew(String("ElevationDifference"), Domain("value"));
	
	Column cStrahler = tbl->colNew(String("Strahler"), Domain("value"), ValueRange(1,32767,1));
	Column cStreve = tbl->colNew(String("Shreve"), Domain("value"), ValueRange(1,32767,1));

	Column cLength = tbl->colNew(String("Length"), Domain("value"));
	Column cStraightLength = tbl->colNew(String("StraightLength"), Domain("value"));
	Column cSlopAlongDrainage = tbl->colNew(String("SlopeAlongDrainagePerc"), Domain("value"), ValueRange(0,1e10,0.01));
	Column cSlopAlongDrainageDegree = tbl->colNew(String("SlopeAlongDrainageDegree"), Domain("value"), ValueRange(0,1e10,0.001));
	Column cSlopDrainageStraight = tbl->colNew(String("SlopeDrainageStraightPerc"), Domain("value"), ValueRange(0,1e10,0.01));
	Column cSlopDrainageStraightDegree = tbl->colNew(String("SlopeDrainageStraightDegree"), Domain("value"), ValueRange(0,1e10,0.001));
	Column cSinuosity = tbl->colNew(String("Sinuosity"), Domain("value"), ValueRange(0,1e10,0.001));
	Column cTotalUpstreamLength = tbl->colNew(String("TotalUpstreamAlongDrainageLength"), Domain("value"));
  Column cStrahlerClass;
  if (!tblSource[String("StrahlerClass")].fValid())
  {
    FileName fnDom(fnObj, ".dom");
	  fnDom = FileName::fnUnique(fnDom);
	  Domain dm = Domain(fnDom, 10, dmtCLASS);  
	  cStrahlerClass = tbl->colNew(String("StrahlerClass"), dm);
  }
  else 
    cStrahlerClass = tbl->colNew(String("StrahlerClass"), Domain(cStrahlerClassSrc->dm()));
  
	DomainSort* pdsrt = cUpstreamLinkSrc->dmKey()->pdsrt();
	if ( !pdsrt )
		throw ErrorObject(TR("Source map must have domain class or id"));
  long iSize = pdsrt->iSize();

  vector<long> vSegIDs = GetSegmentIDsExtracted();
  long iRecs = 0;
  for (long i=1; i<= iSize ; i++ ) 
  {
		long iID = pdsrt->iOrd(i); 
		if (iID == iUNDEF)
			continue;
    bool IsExists = find(vSegIDs.begin(), vSegIDs.end(), iID) != vSegIDs.end();
		if (IsExists == true)
		{
      iRecs++;
      cUpstreamLink->PutVal(iRecs, cUpstreamLinkSrc->sValue(iID));
      cUpstreamCoord->PutVal(iRecs,cUpstreamCoordSrc->cValue(iID));
      cUpstreamHeight->PutVal(iRecs,cUpstreamHeightSrc->rValue(iID)); 
      cDownstreamLink->PutVal(iRecs,cDownstreamLinkSrc->iValue(iID));
      cDownstreamCoord->PutVal(iRecs,cDownstreamCoordSrc->cValue(iID)); 
      cDownstreamHeight->PutVal(iRecs,cDownstreamHeightSrc->rValue(iID)); 
      cDrop->PutVal(iRecs,cDropSrc->rValue(iID));  
      cStrahler->PutVal(iRecs,cStrahlerSrc->iValue(iID)); 
      cStrahlerClass->PutRaw(iRecs,cStrahlerSrc->iValue(iID));
      cStreve->PutVal(iRecs,cStreveSrc->iValue(iID)); 
      cLength->PutVal(iRecs,cLengthSrc->rValue(iID)); 
      cStraightLength->PutVal(iRecs,cStraightLengthSrc->rValue(iID)); 
      cSlopAlongDrainage->PutVal(iRecs,cSlopAlongDrainageSrc->rValue(iID));  
      cSlopAlongDrainageDegree->PutVal(iRecs,cSlopAlongDrainageDegreeSrc->rValue(iID)); 
      cSlopDrainageStraight->PutVal(iRecs,cSlopDrainageStraightSrc->rValue(iID)); 
      cSlopDrainageStraightDegree->PutVal(iRecs,cSlopDrainageStraightDegreeSrc->rValue(iID)); 
      cSinuosity->PutVal(iRecs,cSinuositySrc->rValue(iID)); 
      cTotalUpstreamLength->PutVal(iRecs,cTotalUpstreamLengthSrc->rValue(iID));
      
      //update the extracted drainage attributes splited at the outlet location 
      if (m_UseOutlets)
      {
        for (vector<OutletLocation>::iterator pos = m_vOutlet.begin(); pos < m_vOutlet.end(); ++pos)
	      {		
			    RowCol rc = pos->rc;
			    OutletLocation ol = (*pos);
          if (ol.StreamID == iID && ol.isOnNode != true){
            Coord cdDownstream = mp->gr()->cConv(rc);
            cDownstreamCoord->PutVal(iRecs, cdDownstream);
            double rDownstreamHeight = rGetElevation(rc);
            cDownstreamHeight->PutVal(iRecs,rDownstreamHeight); 
            cLength->PutVal(iRecs,ol.rLen1);
            double rStraightLength = rDistance(cUpstreamCoordSrc->cValue(iID),cdDownstream);
            cStraightLength->PutVal(iRecs, rStraightLength);
            
            double rDrop = cUpstreamHeightSrc->rValue(iID) - rDownstreamHeight;
				    cDrop->PutVal(iRecs, rDrop);
            double rSlop = rComputeSlope(rDrop, ol.rLen1, false);
				    cSlopAlongDrainage->PutVal(iRecs, rSlop);

				    rSlop = rComputeSlope(rDrop, ol.rLen1, true);
				    cSlopAlongDrainageDegree->PutVal(iRecs, rSlop);

				    rSlop = rComputeSlope(rDrop, rStraightLength, false);
				    cSlopDrainageStraight->PutVal(iRecs, rSlop);

				    rSlop = rComputeSlope(rDrop, rStraightLength, true);
				    cSlopDrainageStraightDegree->PutVal(iRecs, rSlop);

				    double rSinuosity = rComputeSinuosity(ol.rLen1, rStraightLength);
				    cSinuosity->PutVal(iRecs, rSinuosity);
            break;
          }
        }  
			}
    }
    trq.fUpdate(i, iSize);
  }
  trq.fUpdate(iSize, iSize);
}

vector<long> MapCatchmentMerge::GetSegmentIDsExtracted()
{
  Column colDrainageID = m_tbl->col(String("DrainageID"));
	DomainSort* pdsrt = colDrainageID->dmKey()->pdsrt();
	if ( !pdsrt )
		throw ErrorObject(TR("Source map must have domain class or id"));
  long iSize = pdsrt->iSize();

  vector<long> vSegIDs;
  vSegIDs.clear();
  for (long i=1; i<= iSize ; i++ ) {
		long iID = pdsrt->iOrd(i); 
		if (iID == iUNDEF)
			continue;
    SplitString(colDrainageID->sValue(i), vSegIDs);
  }
  return vSegIDs;
}

double MapCatchmentMerge::GetSplitSegmentLength(long iStreamID)
{
  for (vector<OutletLocation>::iterator pos = m_vOutlet.begin(); pos < m_vOutlet.end(); ++pos)
	{
		OutletLocation ol = (*pos);
		if (iStreamID == ol.StreamID)
    {
      return ol.rLen2;
    }
	}
  return 0;
}

void MapCatchmentMerge::InitPars()
{
		//	Location number				Order in m_vDirection
		//	-------								-------	 looping order of the neighbors 	
	  //	|6|7|8|								|0|1|2|
		//	-------								-------
		//	|5| |1|								|3|4|5|
		//	-------								-------
		//	|4|3|2|								|6|7|8|
		//	-------								-------
		//
		
		m_vFlowSelf.resize(9);
		m_vFlowSelf[0] = 2;
		m_vFlowSelf[1] = 3;
		m_vFlowSelf[2] = 4;
		m_vFlowSelf[3] = 1;
		m_vFlowSelf[4] = 0;
		m_vFlowSelf[5] = 5;
		m_vFlowSelf[6] = 8;
		m_vFlowSelf[7] = 7;
		m_vFlowSelf[8] = 6;
}

