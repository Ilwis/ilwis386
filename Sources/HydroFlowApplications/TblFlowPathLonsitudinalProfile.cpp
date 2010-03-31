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
/* TblFlowPathLongitudinalProfile
   Copyright Ilwis System Development ITC
   April 2006, by Lichun Wang
*/

#include "HydroFlowApplications\TblFlowPathLonsitudinalProfile.h"
#include "Engine\Table\tblstore.h"
#include "Engine\Table\COLSTORE.H"
#include "Engine\Applications\COLVIRT.H"
#include "Engine\Applications\MAPVIRT.H"
#include "Engine\Domain\dmsort.h"
#include "Engine\Domain\Dmvalue.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Domain\dmcoord.h"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Hs\tbl.hs"
#include "Headers\Hs\map.hs"

const double rDefaultEarthRadius = 6371007.0;

const char* TblFlowPathLongitudinalProfile::sSyntax()
{
  return "TblFlowPathLongitudinalProfile(LongestFlowPathSegmentMap,SegmentID,Distance,AttributeMap)\n";
}

String TblFlowPathLongitudinalProfile::sExpression() const
{
  String sMap1 = m_segmpFlowPath->sNameQuoted(true, fnObj.sPath());
  String sMap2 = m_mpAttributeMap->sNameQuoted(true, fnObj.sPath());
  return String("TblFlowPathLongitudinalProfile(%S,%li,%.2f,%S)", sMap1, m_iSegmentID, m_rDist, sMap2);
}

TblFlowPathLongitudinalProfile* TblFlowPathLongitudinalProfile::create(const FileName& fn, TablePtr& p, const String& sExpr)
{
  Array<String> as;
  int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  if (iParms !=4 ) 
    throw ErrorExpression(sExpr, sSyntax());
  SegmentMap segmpFlowLength(as[0], fn.sPath());
  long iSegmentID = as[1].iVal();
  double rDist = as[2].rVal();  
  Map mpAttribute(as[3], fn.sPath());
  return new TblFlowPathLongitudinalProfile(fn, p, segmpFlowLength, iSegmentID, rDist,mpAttribute);
}

TblFlowPathLongitudinalProfile::TblFlowPathLongitudinalProfile(const FileName& fn, TablePtr& p)
: TableVirtual(fn, p)
{
  ReadElement("TblFlowPathLongitudinalProfile", "LongestFlowPathSegmentMap", m_segmpFlowPath);
  ReadElement("TblFlowPathLongitudinalProfile", "SegmentID", m_iSegmentID);
  ReadElement("TblFlowPathLongitudinalProfile", "Distance", m_rDist);
  ReadElement("TblFlowPathLongitudinalProfile", "AttributeMap", m_mpAttributeMap);
  Init();
}

TblFlowPathLongitudinalProfile::TblFlowPathLongitudinalProfile(const FileName& fn, TablePtr& p, 
									   const SegmentMap& segmpFlowPath,
									   int iSegmentID,
									   double rDistance,
									   const Map& mpAttributeMap)
: TableVirtual(fn, p, true),
  m_segmpFlowPath(segmpFlowPath), 
  m_iSegmentID(iSegmentID),
  m_rDist(rDistance), 
  m_mpAttributeMap(mpAttributeMap)
{
  Init();
  FileName fnDom = FileName::fnUnique(FileName(fnObj, ".dom"));
  ptr.SetDomain(Domain(fnDom, 0, dmtID));
  if (!fnObj.fValid())
    objtime = objdep.tmNewest();
}

TblFlowPathLongitudinalProfile::~TblFlowPathLongitudinalProfile()
{
}

void TblFlowPathLongitudinalProfile::Store()
{
  TableVirtual::Store();
  WriteElement("TableVirtual", "Type", "TblFlowPathLongitudinalProfile");
  WriteElement("TblFlowPathLongitudinalProfile", "LongestFlowPathSegmentMap", m_segmpFlowPath);
  WriteElement("TblFlowPathLongitudinalProfile", "SegmentID", m_iSegmentID);
  WriteElement("TblFlowPathLongitudinalProfile", "Distance", m_rDist);
  WriteElement("TblFlowPathLongitudinalProfile", "AttributeMap", m_mpAttributeMap);
}

void TblFlowPathLongitudinalProfile::AddDomainItem(Domain dm, long iItem )
{
	String sUniqueID = String("%li", iItem);
	dm->pdsrt()->iAdd(sUniqueID,true);
}

bool TblFlowPathLongitudinalProfile::fLatLonCoords()
{
	CoordSystemLatLon* csll = m_segmpFlowPath->cs()->pcsLatLon();
	return (0 != csll);
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

double TblFlowPathLongitudinalProfile::rDistance(Coord cd1, Coord cd2)
{
	double rDist;
	if (fLatLonCoords())
	{
			double rRadi = rDefaultEarthRadius;
			if(m_segmpFlowPath->cs()->pcsViaLatLon())
				rRadi = m_mpAttributeMap->cs()->pcsViaLatLon()->ell.a;
			LatLon llStart = LatLon(cd1.y, cd1.x);
			LatLon llEnd  = LatLon(cd2.y, cd2.x);
			if (fEllipsoidalCoords()) 
			{
				CoordSystemViaLatLon* pcsvll= m_segmpFlowPath->cs()->pcsViaLatLon();
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

bool TblFlowPathLongitudinalProfile::fEllipsoidalCoords()
{
	CoordSystemViaLatLon* csviall = m_segmpFlowPath->cs()->pcsViaLatLon();
	bool fSpheric = true;
	if (csviall)
		fSpheric= (csviall->ell.fSpherical());
	return (0 != csviall &&  0 == fSpheric);
}

bool TblFlowPathLongitudinalProfile::fFreezing()
{
  vector<LongitudinalProfile> vlp;
  trq.SetTitle(sFreezeTitle);
  trq.SetText(SMAPTextInitializeMap);
  trq.Start();
  bool fTransformCoords = m_mpAttributeMap->cs() != m_segmpFlowPath->cs();
  long iLines = m_mpAttributeMap->iLines();
  long iCols = m_mpAttributeMap->iCols();
  m_vAttributeMap.resize(iLines);  
  
  //Reading input attribute map
  for (long iRow = 0; iRow< iLines; iRow++ )
  {
		m_vAttributeMap[iRow].Size(iCols); 
		m_mpAttributeMap->GetLineVal(iRow, m_vAttributeMap[iRow]);
    	if (trq.fUpdate(iRow, iLines)) return false;	
  }
  
  LongitudinalProfile lp;
  //Reading flow path segment map
  vector<Coord> vpoints;
  Coord cFrom, cTo ;
  RowCol rcFrom, rcTo;
  long i;
  for( int j=0 ;j < m_segmpFlowPath->iFeatures(); ++j)
  {
	  ILWIS::Segment *seg = (ILWIS::Segment *)m_segmpFlowPath->getFeature(j);
	  if ( !seg || !seg->fValid())
		  continue;
    long iRaw = seg->iValue(); 
	if (iRaw == m_iSegmentID )
	{
		if ( (trq.fUpdate(j, m_segmpFlowPath->iFeatures())) )
			return false;

        CoordinateSequence *crdSegbuf;
		crdSegbuf = seg->getCoordinates();
		cFrom = crdSegbuf->getAt(0);
		cTo = crdSegbuf->getAt(crdSegbuf->size()-1);
		if (fTransformCoords)
			cFrom = m_mpAttributeMap->cs()->cConv(m_segmpFlowPath->cs(), cFrom);
		rcFrom = m_mpAttributeMap->gr()->rcConv(cFrom);
		rcTo = m_mpAttributeMap->gr()->rcConv(cTo);
		if (m_vAttributeMap[rcFrom.Row][rcFrom.Col]>=m_vAttributeMap[rcTo.Row][rcTo.Col] )
		{
			for (i = 0; i < crdSegbuf->size()-1; ++i) 
			{
				vpoints.push_back(crdSegbuf->getAt(i));
			}
		}
		else
		{
			for (i = crdSegbuf->size()-1; i > 0; --i) 
			{
				vpoints.push_back(crdSegbuf->getAt(i));
			}
		}
		delete crdSegbuf;
	}
  }
  
  long iPointID=1;
  vector<Coord>::iterator pos;
  cFrom = *vpoints.begin();
  if (fTransformCoords)
     cFrom = m_mpAttributeMap->cs()->cConv(m_segmpFlowPath->cs(), cFrom);
  rcFrom = m_mpAttributeMap->gr()->rcConv(cFrom);
  lp.X = iPointID;
  lp.Y = m_vAttributeMap[rcFrom.Row][rcFrom.Col];
  lp.cd = cFrom;
  vlp.push_back(lp);
  double rDist = 0;		
  for (pos = vpoints.begin(); pos < vpoints.end(); ++pos)
  {
	  if (trq.fAborted())
		return false;
	  Coord cTo = *pos;
	  if (fTransformCoords)
         cTo = m_mpAttributeMap->cs()->cConv(m_segmpFlowPath->cs(), cTo);
      rcTo = m_mpAttributeMap->gr()->rcConv(cTo);
      rDist = rDist + rDistance(cFrom, cTo);
	  if (rDist >= (m_rDist - 0.5)) 
	  {
		  iPointID++;
		  lp.X = iPointID;
		  lp.Y = m_vAttributeMap[rcTo.Row][rcTo.Col];
		  lp.cd = cTo;
		  vlp.push_back(lp);
		  rDist = 0;
	  }		  
      cFrom = cTo;
  }
  //Create columns in the output table
  CreateColumns(vlp);
  m_vAttributeMap.resize(0);
  vpoints.resize(0);
  vlp.resize(0);
  return true;
}
void TblFlowPathLongitudinalProfile::Init()
{
  if (!m_segmpFlowPath.fValid() || !m_mpAttributeMap.fValid())
		throw ErrorDummy();

  objdep.Add(m_mpAttributeMap.ptr());
  fNeedFreeze = true;
  sFreezeTitle = "TblFlowPathLongitudinalProfile";
  //htpFreeze = htpHorntonPlotsT;
}

void TblFlowPathLongitudinalProfile::CreateColumns(vector<LongitudinalProfile> vlp)
{
  Column cX = pts->colNew("X", Domain("value"), ValueRange(0,32767,1));   
  Column cY = pts->colNew("Attribute", Domain("value"), ValueRange(-1,1.0e300,0.01)); 
  Domain dmcrd;
  dmcrd.SetPointer(new DomainCoord(m_mpAttributeMap->cs()->fnObj));
  Column cCoord = pts->colNew("Coord",dmcrd);
  vector<LongitudinalProfile>::iterator pos;
  for (pos = vlp.begin(); pos < vlp.end(); ++pos)
  {
	  LongitudinalProfile lp;
	  lp = *pos;
	  AddDomainItem(ptr.dm(), lp.X);	
	  cX->PutVal(lp.X,lp.X);
	  cY->PutVal(lp.X,lp.Y);
	  cCoord->PutVal(lp.X,lp.cd);
  }
}
