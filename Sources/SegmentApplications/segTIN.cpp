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
#include "Engine\Map\Point\PNT.H"
#include "tricommon.h"
#include "SegmentApplications\segTIN.H"
#include "SegmentApplications\segTin.h"
#include "Engine\Map\basemap.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Hs\segment.hs"

IlwisObjectPtr * createSegmentMapTIN(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)SegmentMapTIN::create(fn, (SegmentMapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new SegmentMapTIN(fn, (SegmentMapPtr &)ptr);
}

const char* SegmentMapTIN::sSyntax() {
	return "SegmentMapTIN(inputmap, use convexhull)";
}

SegmentMapTIN* SegmentMapTIN::create(const FileName& fn, SegmentMapPtr& p, const String& sExpr)
{
	Array<String> as(2);
	if (!IlwisObjectPtr::fParseParm(sExpr, as))
		ExpressionError(sExpr, sSyntax());
	String sInputMapName1 = as[0];
	bool convex = as[1].fVal();
	FileName fnInput1(sInputMapName1);
	PointMap pm1(fnInput1);
	Domain dm = Domain(fn, pm1->iFeatures() * 3, dmtUNIQUEID, "Seg");

	//BaseMap basemp(FileName(ssInputMapName));
	return new SegmentMapTIN(fn, p, dm, pm1, convex);
}

SegmentMapTIN::SegmentMapTIN(const FileName& fn, SegmentMapPtr& p)
: SegmentMapVirtual(fn, p)
{
	fNeedFreeze = true;
	String sColName;
	try {
		FileName fn1;
		ReadElement("SegmentMapTIN", "InputMap1", fn1);
		points = PointMap(fn1);
		ReadElement("SegmentMapTIN", "UseConvexHull", fConvexHull);
	}
	catch (const ErrorObject& err) {  // catch to prevent invalid object
		err.Show();
		return;
	}
	Init();
	objdep.Add(points.ptr());
}

SegmentMapTIN::SegmentMapTIN(const FileName& fn, SegmentMapPtr& p, const Domain& dm, const PointMap& pm1, bool convex)
: SegmentMapVirtual(fn, p, pm1->cs(), pm1->cb(),dm), 
points(pm1),
fConvexHull(convex)
{
	fNeedFreeze = true;
	Init();
	objdep.Add(points.ptr());
	if (!fnObj.fValid()) // 'inline' object
		objtime = objdep.tmNewest();
}


void SegmentMapTIN::Store()
{
	SegmentMapVirtual::Store();
	WriteElement("SegmentMapVirtual", "Type", "SegmentMapTIN");
	WriteElement("SegmentMapTIN", "InputMap1", points);
	WriteElement("SegmentMapTIN", "UseConvexHull", fConvexHull);
}

SegmentMapTIN::~SegmentMapTIN()
{
}

String SegmentMapTIN::sExpression() const
{
	return String("SegmentMapTIN(%S,%s)", points->fnObj.sRelativeQuoted(false), fConvexHull ? "true" : "false");
}

bool SegmentMapTIN::fDomainChangeable() const
{
	return false;
}

void SegmentMapTIN::Init()
{
	//htpFreeze = htpSegmentMapTINT;
	sFreezeTitle = "SegmentMapTIN";
}

bool SegmentMapTIN::fFreezing()
{
	trq.SetText("Collecting points" );
	long total = points->iFeatures();
	vector<ILWIS::Point *> pnts;
	for( int i=0; i < points->iFeatures(); ++i) {
		ILWIS::Point *pnt = CPOINT(points->getFeature(i));
		if ( !pnt || pnt->fValid() == false)
			continue;
		if(trq.fUpdate(i, points->iFeatures()))
			return false;
		pnts.push_back(pnt);
	}
	vector<TINPoint> tinPoints(pnts.size());
	for(int i=0; i < pnts.size(); ++i) {
		
		const Coordinate *crd = pnts[i]->getCoordinate();
		tinPoints[i].Set(pnts[i]->iValue(),crd->x, crd->y, crd->z);
	}
	trq.fUpdate(points->iFeatures(),points->iFeatures());
	vector<TINTriangle> TriList;
	MakeTin(&tinPoints, &TriList,fConvexHull, trq);
	set<String> pairs;
	for(long i=0; i < TriList.size(); ++i) {
		TINTriangle triangle = TriList[i];
		int lastIndex = -1;
		set<int> nodes;
		for(int j = 0; j < 3; ++j) {
			int pntIndex = triangle.GetPnt(j);
			nodes.insert(pntIndex);
		}
		set<int>::iterator cur = nodes.begin();
		int i1 = *cur;
		int iStart = i1;
		++cur;
		for(; cur != nodes.end(); ++cur) {
			int i2 = (*cur);
			pairs.insert(String("%d,%d",i1,i2));
			i1 = i2;
		}
		pairs.insert(String("%d,%d",iStart,i1));

	}
	long count = 0;
	for(set<String>::iterator cur = pairs.begin(); cur != pairs.end(); ++cur) {
		String pair = *cur;
		int begin = pair.sHead(",").iVal();
		int end = pair.sTail(",").iVal();
		CoordBuf buf(2);
		TINPoint pnt1 = tinPoints[begin];
		TINPoint pnt2 = tinPoints[end];
		buf[0] = Coord(pnt1.GetX(), pnt1.GetY(), pnt1.GetZ());
		buf[1] = Coord(pnt2.GetX(), pnt2.GetY(), pnt2.GetZ());
		ILWIS::Segment *seg = CSEGMENT(ptr.newFeature());
		seg->PutCoords(buf.size(),buf);
		seg->PutVal(++count);
	}
	return true;
}

bool SegmentMapTIN::MakeTin(vector<TINPoint>* Pnt2List, vector<TINTriangle>* TriList, bool iUseConvexHul, Tranquilizer& trq )	
{
	bool bRet, bRetCode = false;
	int iChPoints, i, j, k, opoints, npoints, nInPoints;
	int *iBelongCount = NULL;
	int *iBoxSorted = NULL;
	int *iNbrCount = NULL;

	TINPoint ep;
	CDT_Matrix<int> iNbrList;
	CDT_Matrix<int> iBlgList;
	vector<TINTriangle> TriVector;
	vector<int> Cnv2List;
	vector<int> PntTable;
	try{
		TriList->clear( );

		// --- ptables: point number in triangulation <-> original number table. ---

		npoints = (int)Pnt2List->size( );
		nInPoints = npoints;

		// --- temporarily remove points with identical X and Y coordinates ---

		PntTable.clear( );
		for ( i = 0; i < npoints + iPsuedoPointCount_TRI( ); i++ ) {
			PntTable.push_back( i );
		}


		for ( i = 0; i < npoints - 1; i++ ) {
			for ( j = i + 1; j < npoints; j++ ) {
				if( Pnt2List->at( i ).x != Pnt2List->at( j ).x ) continue;
				if( Pnt2List->at( i ).y != Pnt2List->at( j ).y ) continue;
				Pnt2List->erase( Pnt2List->begin( ) + j );
				PntTable.erase( PntTable.begin( ) + j );

				npoints--;
				j--;
			}
		}


		i = (int)Pnt2List->size( );
		for( i = 0 ; i < npoints ; i++ ) {
			ep = Pnt2List->at( i );
		}

		if( 3 > npoints ) {
			throw ErrorObject(String(TR("Less than 3 unique points! %d").c_str(), npoints ));
		}

		/*------------------------------------------------------------------
		#### memory allocation for the neighbouring points list.
		#### 
		#### opoints	
		#### number of original points, excluding the duplicated points ( if any ).
		#### 
		#### npoints
		#### number of points including the pseudo points.
		#### 
		#### iNbrCount[i]	 
		#### number of points connected to point i including "i" itself.
		------------------------------------------------------------------*/
		opoints = npoints;
		npoints += iPsuedoPointCount_TRI( );

		iNbrCount = new int[npoints];
		if( NULL == iNbrCount ) {
			throw ErrorObject(TR("Failed in Memory Allocation"));
		}

		// --- initilize neibur list ---

		if( ! iNbrList.Alloc( npoints, npoints ) ) {
			throw ErrorObject(TR("Failed in Memory Allocation"));
		}

		for ( i = 0; i < npoints; i++ ) iNbrCount[i] = 0;	
		if( !bInitNeighbourList( npoints, &iNbrList ) ) {
			throw ErrorObject();
		}

		// --- iBelongCount[i]	number of triangles to which node i belongs ---
		// --- iBlgList[i][j]	j-th triangle to which node i belongs ---

		// --- initialize belong list ---

		if( ! iBlgList.Alloc( npoints, npoints ) ) {
			throw ErrorObject(TR("Failed in Memory Allocation"));
		}

		iBelongCount = new int[npoints];
		if( NULL == iBelongCount ) {
			throw ErrorObject(TR("Failed in Memory Allocation"));
		}

		for ( i = 0; i < npoints; i++ ) iBelongCount[i] = 0; 
		if( !bInitBelongList_TRI( npoints, &iBlgList ) ) {
			throw ErrorObject();
		}

		// --- generate pseudo points‚ð”­¶‚·‚é ---

		bRet = bSetPseudoPoints_TRI( opoints, Pnt2List );

		// --- BOX-sort points ---

		iBoxSorted = new int[npoints];
		if( NULL == iBoxSorted ) {
			throw ErrorObject(TR("Failed in Memory Allocation"));
		}

		bRet = bBoxSortPoint_TRI( opoints, Pnt2List, iBoxSorted );

		// --- START!! constructing neighbour and triangle list. ---

		TriVector.clear( );
		trq.SetText(TR("Processing Points"));
		for( i = 0; i < opoints; i++ ) {
			if( trq.fUpdate(i,npoints))
				return 0;
			k = iBoxSorted[i];
			// --- get triangle and neigbour list around point "p". ---
			iNbrCount[k] = iListNeighbour_TRI( k, opoints, Pnt2List, 
				iBelongCount, &iNbrList, &iBlgList, &TriVector );
		/*	if( iNbrCount[k] < 0 ) { 
				printf( "\7\n" ); 
				break; 
			}*/
		}
		trq.fUpdate(npoints,npoints);


		// --- process points on ConvexHull ---

		if( iUseConvexHul ) {	

			// --- generate convex hull ---	
			Cnv2List.clear( );
			iChPoints = CreateConvexHull( opoints, Pnt2List, &Cnv2List );
			iChPoints = (int)Cnv2List.size( );

			// --- process points on ConvexHull ---
			trq.SetText(TR("Processing convex hulls"));
			for( i = 0; i < iChPoints; i++ ) {
				if( trq.fUpdate(i,iChPoints))
					return 0;
				iListConvex_TRI( i, opoints, Pnt2List, &Cnv2List,  
					iNbrCount, &iNbrList, iBelongCount, &iBlgList, &TriVector );
			}
			trq.fUpdate(iChPoints,iChPoints);
		}

		// --- reconstruct triangle list ---

		bRet = bResetTriangleList_TRI( nInPoints, &PntTable, &TriVector, TriList );

		// --- Done ---

		bRetCode = true;
	} catch(ErrorObject& err) {

		if( iBelongCount ) delete[] iBelongCount;
		if( iBoxSorted ) delete[] iBoxSorted;
		if( iNbrCount ) delete[] iNbrCount;

		iNbrList.Free( );
		iBlgList.Free( );
		Cnv2List.clear( );
		TriVector.clear( );
		PntTable.clear( );
		if ( err.sWhat() != "")
			throw err;
		return false;
	}
	if( iBelongCount ) delete[] iBelongCount;
	if( iBoxSorted ) delete[] iBoxSorted;
	if( iNbrCount ) delete[] iNbrCount;

	iNbrList.Free( );
	iBlgList.Free( );
	Cnv2List.clear( );
	TriVector.clear( );
	PntTable.clear( );

	return bRetCode;
}









