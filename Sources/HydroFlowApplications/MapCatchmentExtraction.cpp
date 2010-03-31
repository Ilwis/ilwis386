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

#include "HydroFlowApplications\MapCatchmentExtraction.h"
#include "Engine\Table\tbl.h"
#include "Engine\Domain\dmsort.h"
#include "Engine\Domain\dmcoord.h"
#include "Engine\SpatialReference\Coordsys.h"
#include "Engine\SpatialReference\Ellips.h"
#include "Engine\SpatialReference\Csproj.h"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Hs\map.hs"

IlwisObjectPtr * createMapCatchmentExtraction(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)MapCatchmentExtraction::create(fn, (MapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new MapCatchmentExtraction(fn, (MapPtr &)ptr);
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

static void VerifyColumns(Map mpMap)
{
	
	Table tblAtt = mpMap->tblAtt();
	if (!tblAtt.fValid())
				throw ErrorNoAttTable(mpMap->fnObj);
	
	if (!tblAtt[sUpstreamLink].fValid())
					ColumnNotFoundError(tblAtt->fnObj, sUpstreamLink);

	if (!tblAtt[sDownstreamLink].fValid())
					ColumnNotFoundError(tblAtt->fnObj, sDownstreamLink);

	if (!tblAtt[sDownstreamCoord ].fValid())
					ColumnNotFoundError(tblAtt->fnObj, String("TostreamCoord")); //sDownstreamCoord );
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
MapCatchmentExtraction::MapCatchmentExtraction(const FileName& fn, MapPtr& p)
: MapFromMap(fn, p)
{

	ReadElement("CatchmentExtraction", "FlowDirection", m_mpFlow);
	Domain dm = m_mpFlow->dm();
	if (!(dm.fValid() && (fCIStrEqual(dm->fnObj.sFileExt(), "FlowDirection.dom"))))
			throw ErrorObject(WhatError(SMAPErrInvalidDomain_S, errMapCatchmentExtraction), m_mpFlow->fnObj);


	ReadElement("CatchmentExtraction", "DrainageNetworkOrderingMap", mp);
	dm = mp->dm();
	if (!(dm.fValid())) // && (fCIStrEqual(dm.sDomainType , dmtID))))
			throw ErrorObject(WhatError(SMAPErrIDDomain_S, errMapCatchmentExtraction), mp->fnObj);

	VerifyColumns(mp);

	CompitableGeorefs(fn, mp, m_mpFlow);
	
	fNeedFreeze = true;
  sFreezeTitle = "MapCatchmentExtraction";
	htpFreeze = htpCatchmentExtractionT;
}

MapCatchmentExtraction::MapCatchmentExtraction(const FileName& fn, 
																											 MapPtr& p,
																											 const Map& mpDrainage,
																											 const Map& mpFlowDir)
																											  
: MapFromMap(fn, p, mpDrainage),
  m_mpFlow(mpFlowDir)
{
	//Verify compatible geo-reference
	CompitableGeorefs(fn, mpDrainage, mpFlowDir);
	
	//Verify domain and attribute table
	Domain dm = mpDrainage->dm();
	if (!(dm.fValid())) // && (fCIStrEqual(dm.sDomainType , dmtID))))
			throw ErrorObject(WhatError(SMAPErrIDDomain_S, errMapCatchmentExtraction), mpDrainage->fnObj);

	VerifyColumns(mpDrainage);
	
	Domain flowdm = mpFlowDir->dm();
	if (!(flowdm.fValid() && (fCIStrEqual(flowdm->fnObj.sFileExt(), "FlowDirection.dom"))))
			throw ErrorObject(WhatError(SMAPErrInvalidDomain_S, errMapCatchmentExtraction), mpFlowDir->fnObj);

	SetDomainValueRangeStruct(dm);  
	fNeedFreeze = true;
	sFreezeTitle = "MapCatchmentExtraction";
	htpFreeze = htpCatchmentExtractionT;

	objdep.Add(m_mpFlow);
	objdep.Add(mpDrainage);

  if (!fnObj.fValid())
    objtime = objdep.tmNewest();
	ptr.Store(); 
}

MapCatchmentExtraction::~MapCatchmentExtraction()
{
}

const char* MapCatchmentExtraction::sSyntax()
{ 
  return "MapCatchmentExtraction(DrainageNetworkOrderMap, FlowDirectionMap)";
}

MapCatchmentExtraction* MapCatchmentExtraction::create(const FileName& fn, 
																															 MapPtr& p, 
																															 const String& sExpr)
{
  Array<String> as;
  int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
	if (iParms != 2 )
      ExpressionError(sExpr, sSyntax());

  Map mpDrainage(as[0], fn.sPath());
	Map mpflowdir(as[1], fn.sPath());
	
	return new MapCatchmentExtraction(fn, p, mpDrainage, mpflowdir);
}

void MapCatchmentExtraction::Store()
{
  MapFromMap::Store();
  WriteElement("MapFromMap", "Type", "MapCatchmentExtraction");
	WriteElement("CatchmentExtraction", "FlowDirection", m_mpFlow);
	WriteElement("CatchmentExtraction", "DrainageNetworkOrderingMap", mp);
}

String MapCatchmentExtraction::sExpression() const
{
  
	String sExp("MapCatchmentExtraction(%S,%S)", 
							 mp->sNameQuoted(true, fnObj.sPath()),
               m_mpFlow->sNameQuoted(true));
  return sExp;
}

bool MapCatchmentExtraction::fDomainChangeable() const
{
  return false;
}

bool MapCatchmentExtraction::fGeoRefChangeable() const
{
  return false;
}

bool MapCatchmentExtraction::fLatLonCoords()
{
	CoordSystemLatLon* csll = mp->cs()->pcsLatLon();
	return (0 != csll);
}

bool MapCatchmentExtraction::fFreezing()
{
	trq.SetTitle(sFreezeTitle);
	trq.SetText(SMAPTextInitializeMap);
	trq.Start();

	m_vDrainageMap.resize(iLines());  
	m_vFlowDir.resize(iLines());  
	m_vFlag.resize(iLines());

  for (long iRow = 0; iRow< iLines(); iRow++ )
	{
		//Reading input drainage map
		m_vDrainageMap[iRow].Size(iCols()); 
		mp->GetLineRaw(iRow, m_vDrainageMap[iRow]);

		//Reading input flow direction map
		m_vFlowDir[iRow].Size(iCols());
		m_vFlag[iRow].Size(iCols());
		m_mpFlow->GetLineRaw(iRow, m_vFlowDir[iRow]);
		for (long iCol = 0; iCol < iCols(); ++iCol)
		{
			if (IsEdgeCell(iRow, iCol))
				m_vFlowDir[iRow][iCol] = 0;
			m_vFlag[iRow][iCol] = iUNDEF;
		}
		if (trq.fUpdate(iRow, iLines())) return false;	
	}
	trq.fUpdate(iLines(), iLines());

	//Reading the related drainage attributes into record set - m_vRecords 
	GetAttributes();
	
	//Delineate catchment per drainage channel
	trq.SetText(SMAPTextExtractCatchment);
	while (m_vRecords.size() != 0 )
	{
		for (unsigned long index=0; index < m_vRecords.size(); ++index)
		{
			AttCols attFields (m_vRecords[index]);
      
			//should start with a no upstream channel 
			//or with a channel that upstream(s) have been processed.  
			if (attFields.UpstreamLink.size() == 1 && attFields.UpstreamLink[0] == 0)
			{
				//Extract catchment, and flag the cells with DrainageID that flow to Downstream Coord 
				long iRow = attFields.DownstreamCoord.Row;
				long iCol = attFields.DownstreamCoord.Col;
				DelineateCatchment(iRow, iCol, attFields.DrainageID);

				//Remove the upstreamlink for the downstream that it drains to	
				UpdateUpstreamLinkID(attFields.DownstreamLink, attFields.DrainageID);
				
				EraseDrainage(attFields.DrainageID);
				if (trq.fUpdate(index, (int)m_vRecords.size())) return false;
			}
		}	
	} 

	//Write catchment map
	trq.SetText(SMAPTextCreateCatchmentMap);
  for (long iRow = 0; iRow < iLines(); iRow++ )
	{
		//LongBuf& bDrainage = m_vDrainageMap[iRow];
		LongBuf& bDrainage = m_vFlag[iRow];
    ptr.PutLineRaw(iRow, bDrainage);
		if (trq.fUpdate(iRow, iLines())) return false;
	}
	trq.fUpdate(iLines(), iLines());

	//Cleanup
	m_vDrainageMap.resize(0);
	m_vFlag.resize(0);
	m_vRecords.resize(0);

	//Create attibute table associated with catchment map
	FileName fnTbl(fnObj, ".tbt");
	m_tbl = Table(fnTbl, mp->dm());
  SetAttributeTable(m_tbl);

	//Vectorize the raster catchment map 
	trq.SetText(SMAPTextCreateCatchmentPolygonMap);
	FileName fnPol(fnObj,".mpa");
	String sExprPol("PolygonMapFromRas(%S, %li, %S)", fnObj.sFullPathQuoted(), 8, String("smooth")); 
						                                        
	PolygonMap polMap; 
	polMap = PolygonMap(fnPol, sExprPol);
	polMap->Calc();
	polMap->SetAttributeTable(m_tbl);

	trq.SetText(SMAPTextComputeCatchmentAttributes);
	FileName fnTmpHsa;
	String sExprTbl;
	Table tblHsa;
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
    String sExprPMT("PolygonMapTransform(%S, %S, %g)", fnPol.sFullPathQuoted(false), csy->sName(), 0.000000); 
    csy->fErase = true;
		PolygonMap polTmpTFMap; 
		polTmpTFMap = PolygonMap(fnTmpTFPol, sExprPMT);
		polTmpTFMap->Calc();
		polTmpTFMap->fErase = true;
		//***a temporary histogram file needed to retrieve attributes about Area and Perimeter   
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
	m_cArea = tblHsa->col("Area");
	m_cPerimeter = tblHsa->col("Perimeter");
	ComputeCatchmentAttributes();
	ComputerCenterPolygon(fnPol);
	ComputeCenterDrainage();
	m_vFlowDir.resize(0);
	return true;
}

void MapCatchmentExtraction::CompitableGeorefs(FileName fn, Map mp1, Map mp2)
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

bool MapCatchmentExtraction::IsEdgeCell(long iRow, long iCol)
{
		if (iRow == 0 || iRow == iLines() - 1 ||
				iCol == 0 || iCol == iCols() - 1)
				return true;
		else
				return false;
}

void SplitString(String s, vector<long> &results)
{
	Array<String> as;
	Split(s, as, ",{}");
	results.clear();
	for (unsigned int i=0; i < as.size(); i++)
	{
		long res = as[i].iVal();
		if (res != iUNDEF)
			results.push_back(res);
	}
}

void MapCatchmentExtraction::GetAttributes()
{
	//Retrieve the network link attributes needed to be able 
	//to extract the catchment area
	Table tblAtt = mp->tblAtt();
	Column colUpstreamLink = tblAtt->col(sUpstreamLink);
	Column colDownstreamLink = tblAtt->col(sDownstreamLink);
	Column colDownstreamCoord = tblAtt->col(String("TostreamCoord")); //sDownstreamCoord);

	DomainSort* pdsrt = colUpstreamLink->dmKey()->pdsrt();
	if ( !pdsrt )
		throw ErrorObject(SMAPErrNoDomainSort);
  long iSize = pdsrt->iSize();
	
	AttCols ac;
	for (long i=1; i<= iSize ; i++ ) {
		ac.DrainageID = pdsrt->iOrd(i); 
		if (ac.DrainageID == iUNDEF)
			continue;

		ac.DownstreamLink = colDownstreamLink->iValue(i);  
		ac.DownstreamCoord = mp->gr()->rcConv(colDownstreamCoord->cValue(i)); 
		SplitString(colUpstreamLink->sValue(i), ac.UpstreamLink);
		m_vRecords.push_back(ac);
	}
}

void MapCatchmentExtraction::ComputeCatchmentAttributes()
{
	trq.SetText(SMAPTextcreateCatchmentAttributeTable);

	//Retrieve attributes from drainage network attribute table needed 
	//for updating the catchment attributes  
	Table tblAtt = mp->tblAtt();
	Column colUpstreamLink = tblAtt->col(sUpstreamLink);
	Column colDownstreamLink = tblAtt->col(sDownstreamLink);
	Column colFlowLength = tblAtt->col(String("Length"));

	DomainSort* pdsrt = colUpstreamLink->dmKey()->pdsrt();
	if ( !pdsrt )
		throw ErrorObject(SMAPErrNoDomainSort);
  long iSize = pdsrt->iSize();

	//Define catchment attributes
	Column cDrainageID = m_tbl->colNew(String("DrainageID"), Domain("value"), ValueRange(1,32767,1));
  cDrainageID->SetOwnedByTable(true);
  cDrainageID->SetReadOnly(true);
	Column cUpstreamLinkCatchment = m_tbl->colNew(String("UpstreamLinkCatchment"), Domain("string")); 
  cUpstreamLinkCatchment->SetOwnedByTable(true);
  cUpstreamLinkCatchment->SetReadOnly(true);
	Column cDownstreamLinkCatchment = m_tbl->colNew(String("DownstreamLinkCatchment"), Domain("value"), ValueRange(1,32767,1));
  cDownstreamLinkCatchment->SetOwnedByTable(true);
  cDownstreamLinkCatchment->SetReadOnly(true);
	Column cPerimeter = m_tbl->colNew(String("Perimeter"), Domain("value"), ValueRange(1,1.0e300,0.01));
	Column cCatchmentArea = m_tbl->colNew(String("CatchmentArea"), Domain("value"), ValueRange(1,1.0e300,0.01));
	Column cTotalUpstreamArea = m_tbl->colNew(String("TotalUpstreamArea"), Domain("value"), ValueRange(1,1.0e300,0.01));
	Column cLongestFlowLength = m_tbl->colNew(String("LongestFlowLength"), Domain("value"), ValueRange(1,1.0e300,0.01));
	
	//Retrieve upstream link attributes needed to compute the total area
	AttUpstreamLink vUpstreamLinks;
	for (long i=1; i<= iSize ; i++ ) 
	{
		long iDrainageID = pdsrt->iOrd(i);
		vUpstreamLinks.DrainageID = iDrainageID; 
		SplitString(colUpstreamLink->sValue(i), vUpstreamLinks.UpstreamLink);
		m_vvUpstreamLinks.push_back(vUpstreamLinks);
	}

	long index = 0;	
	for (long i=1; i<= iSize ; i++ ) 
	{
		long iDrainageID = pdsrt->iOrd(i); 
		if (iDrainageID == iUNDEF)
			continue;

		cDrainageID->PutVal(iDrainageID, iDrainageID);
		cUpstreamLinkCatchment->PutVal(iDrainageID, colUpstreamLink->sValue(i));
		cDownstreamLinkCatchment->PutVal(iDrainageID, colDownstreamLink->iValue(i));
		cPerimeter->PutVal(iDrainageID, m_cPerimeter->rValue(i));
		cCatchmentArea->PutVal(iDrainageID, m_cArea->rValue(i));

		//Initialize the totalarea  to the catchment area itsself, if it is a source link, it should be
		//Otherwise, assign an no data value, this will be computed later
		AttUpstreamLink vULs (m_vvUpstreamLinks[i-1]);
		if(vULs.UpstreamLink.size() == 1 && vULs.UpstreamLink[0] == 0)
			cTotalUpstreamArea->PutVal(iDrainageID, m_cArea->rValue(i));
		else
			cTotalUpstreamArea->PutVal(iDrainageID, rUNDEF);

		cLongestFlowLength->PutVal(iDrainageID, colFlowLength->rValue(i));

		if (trq.fUpdate(index, (int)m_vRecords.size())) return;
		index++;
	}
	trq.fUpdate((int)m_vRecords.size(), (int)m_vRecords.size());

	ComputeTotalUpstreamArea(pdsrt, cCatchmentArea, cTotalUpstreamArea);
	m_vvUpstreamLinks.resize(0);
}

void MapCatchmentExtraction::ComputeTotalUpstreamArea(DomainSort* pdsrt, Column cArea, Column cTotalArea)
{
	bool fComputeTotalArea = true;
	long iSize = pdsrt->iSize();
	trq.SetText(SMAPTextCalculateTotalUpstreamCatchmentArea);
	while(fComputeTotalArea)
	{
		fComputeTotalArea = false;
		for (long i=1; i<= iSize ; i++)
		{	
			long iDrainageID = pdsrt->iOrd(i); 
		  if (iDrainageID == iUNDEF)
			  continue;
			  	
			if(cTotalArea->rValue(i) == rUNDEF)
			{
				fComputeTotalArea = true;
				AttUpstreamLink vULs (m_vvUpstreamLinks[i-1]);
				vector<long> vLinks = vULs.UpstreamLink;
				double rArea = 0;
				for (vector<long>::iterator pos = vLinks.begin(); 
								 pos < vLinks.end(); ++pos)
				{	
					//search ID in domain, return index, if the ID is found 
					String sLbl("%li", (*pos)); 
					long iRaw = pdsrt->iOrd(sLbl);
					if (cTotalArea->rValue(iRaw) == rUNDEF)
						break;
					rArea +=cTotalArea->rValue(iRaw);
					cTotalArea->PutVal(iDrainageID, rArea);
				}
			} 
			 if (trq.fUpdate(i, iSize)) return;
		} 
	} //while()
}

void MapCatchmentExtraction::ComputerCenterPolygon(FileName fn)
{
	//First lable point map of the polygon
	//then, put the point coordinates to the catchment attribute table
	trq.SetText(SMAPTextCalculateCenterPolygon);
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
  for (long i=1; i <= iPoint; ++i) {
		long iRaw = ptTmpMap->iRaw(i); 
		if (iRaw == iUNDEF)
			continue;
		Coord crd = ptTmpMap->cValue(i);
    cCenterCatchment->PutVal(iRaw, crd);
		if (trq.fUpdate(i, iPoint)) return;
  }
	ptTmpMap->fErase = true;
}

double MapCatchmentExtraction::GetDistance(RowCol& rc)
{
	//Return a cell that the current given cell rc flows to,
	//otherwise, return the same cell as the given cell, This means
	//that the given cell doesn't flow to any other cell.   
	int iPos = m_vFlowDir[rc.Row-1][rc.Col-1];
	RowCol rc2;
	switch (iPos)
	{
				case 1: 	//East
						rc2.Row = rc.Row;
						rc2.Col = rc.Col+1;
						break;
				case 2:  //South East 
						rc2.Row = rc.Row+1;
						rc2.Col = rc.Col+1;
						break;
				case 3: 	//South
						rc2.Row = rc.Row+1;
						rc2.Col = rc.Col;
						break;
				case 4: //South West
						rc2.Row = rc.Row+1;
						rc2.Col = rc.Col-1;
						break;
				case 5:	//West
						rc2.Row = rc.Row;
						rc2.Col = rc.Col-1;
						break;
				case 6:	//North West 
						rc2.Row = rc.Row-1;
						rc2.Col = rc.Col-1;
						break;
				case 7:	//North
						rc2.Row = rc.Row-1;
						rc2.Col = rc.Col;
						break;
				case 8:	//North East
						rc2.Row = rc.Row-1;
						rc2.Col = rc.Col+1;
						break;
	}

	Coord c1 = mp->gr()->cConv(rc);
	Coord c2 = mp->gr()->cConv(rc2);
	double dist = rDistance(c1, c2);
	rc.Row = rc2.Row;
	rc.Col = rc2.Col;
	return dist;
}

double MapCatchmentExtraction::rDistance(Coord cd1, Coord cd2)
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

void MapCatchmentExtraction::ComputeCenterDrainage()
{
	//Retrieve UpstreamCoord attributes from drainage network attribute table needed
  //to be able to trace the flow path and get the center of drainage.
	trq.SetText(SMAPTextCalculateCenterDrainage);
	Table tblAtt = mp->tblAtt();
	Column colUpstreamCoord = tblAtt->col(String("UpstreamCoord"));
	Column colLength = tblAtt->col(String("Length"));
	
	Domain dmcrd;
	dmcrd.SetPointer(new DomainCoord(mp->cs()->fnObj));
  Column cCenterDrainage = m_tbl->colNew(String("CenterDrainage"), dmcrd);

	DomainSort* pdsrt = colUpstreamCoord->dmKey()->pdsrt();
	if ( !pdsrt )
		throw ErrorObject(SMAPErrNoDomainSort);
  long iSize = pdsrt->iSize();
	for (long i=1; i<= iSize ; i++ ) 
	{
		long iDrainageID = pdsrt->iOrd(i); 
		if (iDrainageID == iUNDEF)
			continue;

		RowCol rc = mp->gr()->rcConv(colUpstreamCoord->cValue(i));
		double rLength = colLength->rValue(i)/2; 
		double rDistance = 0;
		while (rDistance < rLength )
		{
			rDistance = rDistance + GetDistance(rc);
		}
		rc.Row -= 1;
		rc.Col -= 1;
		Coord crd = mp->gr()->cConv(rc);
		cCenterDrainage->PutVal(iDrainageID, crd);
		if (trq.fUpdate(i, iSize)) return;
	}
}

long MapCatchmentExtraction::DelineateCatchment(long iRow, long iCol, long iFlag)
{
	//For the specified downstream cell in loaction rc, 
	//check whether its neighboring cells flow to it,
	//If true, flag the cells with iFlag in m_vDrainage, then call the function recursively   
	//The recursion stops when it reaches a cell that has no flow to it 
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
	bool isFlow; //determine if the neighboring cell flows to the cell in location rc 
	long in, jn;  
	for (int iNr = 1; iNr < 9; iNr++)
	{
		isFlow = false;
		switch (iNr)
		{
				case 1: {	//East
					if (iCol != iCols()-1)
					{	
						in = iRow;
						jn = iCol+1;
						isFlow = ((m_vFlowDir[in][jn] == 5) && (m_vFlag[in][jn] == iUNDEF));
					}
				}
				break;
				case 2: { //South East 
					if (iCol != iCols()-1 && iRow != iLines()-1)
					{
						in = iRow+1;
						jn = iCol+1;
						isFlow = ((m_vFlowDir[in][jn] == 6) && (m_vFlag[in][jn] == iUNDEF));
					}
				}
				break;
				case 3: {	//South
					if (iRow != iLines()-1)
					{
						in = iRow+1;
						jn = iCol;
						isFlow = ((m_vFlowDir[in][jn] == 7) && (m_vFlag[in][jn] == iUNDEF));
					}
				}
				break;
				case 4:{ //South West
					if (iCol != 0 && iRow != iLines()-1)
					{
						in = iRow+1;
						jn = iCol-1;
						isFlow = ((m_vFlowDir[in][jn] == 8) && (m_vFlag[in][jn] == iUNDEF));
					}
				}
				break;
				case 5:{	//West
					if (iCol != 0 )
					{
						in = iRow;
						jn = iCol-1;
						isFlow = ((m_vFlowDir[in][jn] == 1) && (m_vFlag[in][jn] == iUNDEF));
					}
				}
				break;
				case 6:{	//North West 
					if (iCol != 0 && iRow != 0)
					{
						isFlow = false;
						in = iRow-1;
						jn = iCol-1;
						isFlow = ((m_vFlowDir[in][jn] == 2) && (m_vFlag[in][jn] == iUNDEF));
					}	
				}
				break;
				case 7:{	//North
					if (iRow != 0)
					{
						in = iRow-1;
						jn = iCol;
						isFlow = ((m_vFlowDir[in][jn] == 3) && (m_vFlag[in][jn] == iUNDEF));
					}
				}
				break;
				case 8:{	//North East
					if (iCol != iCols()-1 && iRow != 0)
					{
						in = iRow-1;
						jn = iCol+1;
						isFlow = ((m_vFlowDir[in][jn] == 4) && (m_vFlag[in][jn] == iUNDEF));
					}
				}
				break;
		}
		if (isFlow)
		{
			//m_vFlag[in][jn] = 1;
			//m_vDrainageMap[in][jn] = iFlag;
			iFlow += DelineateCatchment(in, jn, iFlag);
			
		}
		m_vFlag[iRow][iCol] = iFlag;
		//m_vDrainageMap[iRow][iCol] = iFlag;
	}
	return iFlow;
}

long MapCatchmentExtraction::FindDownstreamIndex(long DowmstreamID)
{
	
	for (vector<AttCols>::iterator pos = m_vRecords.begin(); pos < m_vRecords.end(); ++pos)
	{		
			AttCols ac (*pos);
			if (ac.DrainageID == DowmstreamID)
				return (long)(pos - m_vRecords.begin());
	}
	return -1;
}

void MapCatchmentExtraction::UpdateUpstreamLinkID(long DrainageID, long UpstreamID)
{

	if ( DrainageID != iUNDEF )
	{
			//Find the downstream index in m_vRecords
			//returns position for the downstream in m_vRecords
			long iIndex = FindDownstreamIndex(DrainageID);
			
			if (iIndex > 0)
			{
				AttCols attFields (m_vRecords[iIndex]);

				if ((attFields.UpstreamLink.size() == 1) && (attFields.UpstreamLink[0] == UpstreamID))
						m_vRecords[iIndex].UpstreamLink[0] = 0;
				else if (attFields.UpstreamLink.size() > 1)
				{
					vector<long>::iterator pos;
					pos = find(attFields.UpstreamLink.begin(), attFields.UpstreamLink.end(), UpstreamID);
					if (pos != attFields.UpstreamLink.end())
					{
						attFields.UpstreamLink.erase(pos);
						m_vRecords[iIndex].UpstreamLink = attFields.UpstreamLink;
					}
				}
			}
	} 
}

void MapCatchmentExtraction::EraseDrainage(long DrainageID)
{
	
	for (vector<AttCols>::iterator pos = m_vRecords.begin(); pos < m_vRecords.end(); ++pos)
	{		
			AttCols ac (*pos);
			if (ac.DrainageID == DrainageID)
			{
				m_vRecords.erase(pos);
				break;
			}
	}
}

bool MapCatchmentExtraction::fEllipsoidalCoords()
{
	CoordSystemViaLatLon* csviall = mp->cs()->pcsViaLatLon();
	bool fSpheric = true;
	if (csviall)
		fSpheric= (csviall->ell.fSpherical());
	return (0 != csviall &&  0 == fSpheric);
}
