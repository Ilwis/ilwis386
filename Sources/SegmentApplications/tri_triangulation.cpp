#include "tricommon.h"
#include "Headers\toolspch.h"
#include "Engine\Base\DataObjects\Tranq.h"


#define FPARAM_PITCH 		( 10.0 ) // angle in degree
#define DISTANCE_PITCH 		( 5.0 )
#define PSEUDO_POINT_COUNT 	( 12 )
#define STANDARD_BOX_COUNT	( 10 )

int iPsuedoPointCount_TRI( void ) { return PSEUDO_POINT_COUNT; }

/**********************************************************************/
/**********************************************************************/
/*** Triangulate Polygon **********************************************/
/**********************************************************************/
/**********************************************************************/

//----------------------------------------------------------------------
// check the positional relationship of three 2-D points
// 
// Returns:
// -1: error
// 0: point p2 is on the line connecting p0 and p1
// 1: point p2 is in the left hand side of the line p0 -> p1
// 2: point p2 is in the right hand side of the line p0 -> p1
// Notes:
//
int iPointsRelation_TRI( 
		TINPoint *p0,
		TINPoint *p1,
		TINPoint *p2 )
{
	double dDet;
	dDet = p0->x * p1->y - p0->y * p1->x +
			p0->y * p2->x - p0->x * p2->y +
			p1->x * p2->y - p2->x * p1->y;
	
	if( dDet > 0.0 ) {
		return 1; // left
	} else if( dDet < 0.0 ) {
		return 2; // right
	} else {
		return 0; // on the line
	}
}	

int iPointsRelation_TRI( 
		vector<TINPoint> *Pnt2List,
		int				i0,
		int				i1,
		int				i2 )
{
	int nPoints = (int)Pnt2List->size( );
	if( 0 > i0 || nPoints <= i0 ) return -1;
	if( 0 > i1 || nPoints <= i1 ) return -1;
	if( 0 > i2 || nPoints <= i2 ) return -1;
	TINPoint *p0 = &Pnt2List->at( i0 );
	TINPoint *p1 = &Pnt2List->at( i1 );
	TINPoint *p2 = &Pnt2List->at( i2 );
	return iPointsRelation_TRI( p0, p1, p2 );
}

//----------------------------------------------------------------------
// check if two segment intersect properly
// 
// Returns:
// true or false
//
bool bCheckPropIntersection_TRI( 
		TINPoint	*p0,
		TINPoint	*p1,
		TINPoint	*p2,
		TINPoint	*p3 )
{
	int iPos012 = iPointsRelation_TRI( p0, p1, p2 );
	if( 0 == iPos012 ) return false;

	int iPos013 = iPointsRelation_TRI( p0, p1, p3 );
	if( 0 == iPos013 ) return false;
	
	int iPos230 = iPointsRelation_TRI( p2, p3, p0 );
	if( 0 == iPos230 ) return false;
	
	int iPos231 = iPointsRelation_TRI( p2, p3, p1 );
	if( 0 == iPos231 ) return false;

	int iXor0 = ( 1 != iPos012 ) ^ ( 1 != iPos013 );
	if( !iXor0 ) return false;

	int iXor1 = ( 1 != iPos230 ) ^ ( 1 != iPos231 );
	return iXor1 ? true : false;
}

//----------------------------------------------------------------------
// check if the given point is strictly on the segment defined by two 
// other points
// 
// Returns:
// true or false
//
bool bBetweenSegments_TRI( 
		TINPoint	*p0,	// [i] segment starting point
		TINPoint	*p1,	// [i] segment another point
		TINPoint	*p2 )	// [i] point to check
{
	int iPos = iPointsRelation_TRI( p0, p1, p2 );
	if( 0 != iPos ) return false;

	if( p0->x != p1->x ) {
		double dMinX = min( p0->x, p1->x );
		if( p2->x < dMinX ) return false;
		double dMaxX = max( p0->x, p1->x );
		return( p2->x > dMaxX ) ? false : true;
	} else {
		// p0->p1 is vertical!
		double dMinY = min( p0->y, p1->y );
		if( p2->y < dMinY ) return false;
		double dMaxY = max( p0->y, p1->y );
		return( p2->y > dMaxY ) ? false : true;
	}
}

//----------------------------------------------------------------------
// check if two segment intersect anyway
// 
// Returns:
// true or false
//
bool bCheckIntersection_TRI( 
		TINPoint	*p0,
		TINPoint	*p1,
		TINPoint	*p2,
		TINPoint	*p3 )
{
	if( bCheckPropIntersection_TRI( p0, p1, p2, p3 ) ) return true;

	if( bBetweenSegments_TRI( p0, p1, p2 ) ) return true;
	if( bBetweenSegments_TRI( p0, p1, p3 ) ) return true;
	if( bBetweenSegments_TRI( p2, p3, p0 ) ) return true;
	if( bBetweenSegments_TRI( p2, p3, p1 ) ) return true;

	return false;
}

bool bCheckIntersection_TRI( 
		vector<TINPoint> *Pnt2List, 
		int				i0,
		int				i1,
		int				i2,
		int				i3 )
{
	if( 0 > i0 ) return false;
	if( 0 > i1 ) return false;
	if( 0 > i2 ) return false;
	if( 0 > i3 ) return false;
	int nPoints = (int)Pnt2List->size( );
	if( nPoints <= i0 ) return false;
	if( nPoints <= i1 ) return false;
	if( nPoints <= i2 ) return false;
	if( nPoints <= i3 ) return false;
	
	TINPoint	*p0 = &Pnt2List->at( i0 );
	TINPoint	*p1 = &Pnt2List->at( i1 );
	TINPoint	*p2 = &Pnt2List->at( i2 );
	TINPoint	*p3 = &Pnt2List->at( i3 );
	return( bCheckPropIntersection_TRI( p0, p1, p2, p3 ) );
}

//----------------------------------------------------------------------
// 
// 
// Returns:
// true or false
//
// Notes:
// polygon must be counter-clockwise
//
bool bCheckDiagonalie_TRI( vector<TINPoint> *Pnt2List, int i, int j )
{
	int k, nPoints;
	if( 0 > i || 0 > j ) return false;
	nPoints = (int)Pnt2List->size( );
	if( nPoints <= i || nPoints <= j ) return false;

	for( k = 0 ; k < nPoints ; k++ ) {
		if( k == i ) continue;
		if( k == j ) continue;
		int k1 = ( k + 1 ) % nPoints;
		if( k1 == i ) continue;
		if( k1 == j ) continue;
		if( bCheckIntersection_TRI( Pnt2List, i, j, k, k1 ) ) return false;
	}

	return true;
}

//----------------------------------------------------------------------
// check if segment connecting two vertices of the polygon is strictly 
// in the polygon
// 
// Returns:
// true or false
//
// Notes:
// polygon must be counter-clockwise
//
bool bCheckCone_TRI( vector<TINPoint> *Pnt2List, int i, int j )
{
	int iPos, nPoints;
	if( 0 > i || 0 > j ) return false;
	nPoints = (int)Pnt2List->size( );
	if( nPoints <= i || nPoints <= j ) return false;

	int i1 = ( i + 1 ) % nPoints;
	int in1 = ( i + nPoints - 1 ) % nPoints;

	iPos = iPointsRelation_TRI( Pnt2List, in1, i, i1 );
	if( iPos = 0 || iPos == 1 ) {
		int iPos0 = iPointsRelation_TRI( Pnt2List, i, j, in1 );
		if( 1 != iPos0 ) return false;
		int iPos1 = iPointsRelation_TRI( Pnt2List, j, i, i1 );
		return 1 == iPos1;
	} else {
		int iPos0 = iPointsRelation_TRI( Pnt2List, i, j, i1 );
		int iPos1 = iPointsRelation_TRI( Pnt2List, j, i, in1 );
		return !( 1 == iPos0 && 1 == iPos1 );
	}
}

//----------------------------------------------------------------------
// 
// 
// Returns:
// true or false
//
// Notes:
// polygon must be counter-clockwise
//
bool bCheckDiagonal_TRI( vector<TINPoint> *Pnt2List, int i, int j )
{
	if( 0 > i || 0 > j ) return false;
	int nPoints = (int)Pnt2List->size( );
	if( nPoints <= i || nPoints <= j ) return false;

	if( !bCheckCone_TRI( Pnt2List, i, j ) ) return false;
	return bCheckDiagonalie_TRI( Pnt2List, i, j );
}


//----------------------------------------------------------------------
// devide polygon into triangles
// 
// Returns:
// true or false
//
// Notes:
// polygon must be counter-clockwise
//
int iTriangulatePolygon_TRI( 
		vector<TINPoint> *Pnt2List,	// [i/o] polygon
		vector<TINTriangle> *Tri2List )	// [i/o] list of triangles
{
	int i, j, k, nTri, nPoints;
	TINTriangle tri;

	nPoints = (int)Pnt2List->size( );
	if( 3 >= nPoints ) goto PIX_EXIT;

	for( i = 0 ; i < nPoints ; i++ ) {
		j = ( i+1 ) % nPoints;
		k = ( i+2 ) % nPoints;
		if( bCheckDiagonal_TRI( Pnt2List, i, k ) ) {
			tri.Set( (int)Tri2List->size( ), Pnt2List->at( i ).GetID( ), 
				Pnt2List->at( j ).GetID( ), Pnt2List->at( k ).GetID( ) );
			Tri2List->push_back( tri );
			Pnt2List->erase( Pnt2List->begin( ) + j );
			nTri = iTriangulatePolygon_TRI( Pnt2List, Tri2List );
			break;
		}
	}

	// --- Done ---
PIX_EXIT:
	return nPoints;
}


//----------------------------------------------------------------------
// devide polygon into triangles
// 
// Returns:
// true or false
//
// Notes:
// polygon must be counter-clockwise
//

void StartPolygonTri_TRI(
		vector<TINPoint> *Pnt2List,	// [i]
		vector<TINTriangle> *Tri2List )
{
	int nTri;
	TINPoint ep;
	vector<TINPoint> PntList;
	vector<TINPoint>::iterator Iter;

	// --- copy the polygon ---
	PntList.clear( );
	for ( Iter = Pnt2List->begin( ); Iter != Pnt2List->end( ); Iter++ ) {
		ep = *Iter;
		PntList.push_back( ep );
	}

	// --- do the triangulation ---
	Tri2List->clear( );
	nTri = iTriangulatePolygon_TRI( &PntList, Tri2List );

	// --- Done ---
	
	PntList.clear( );
	return;
}


/**********************************************************************/
/**********************************************************************/
/*** triangulated Irregular Network ***********************************/
/**********************************************************************/
/**********************************************************************/

//======================================================================
// 
//
bool bInitNeighbourList( 
		int				npoints,
		CDT_Matrix<int>	*nbr )
{
	int i, j, n;
	n = 0;
	for ( i = 0; i < npoints; i++ ) {
		for( j = 0; j < npoints; j++ ) {
			nbr->Set( i, j, n );
		}
	}

	return true;
}


//======================================================================
// prepare PSEUDO_POINT_COUNT pseudo points
//
bool bSetPseudoPoints_TRI( 	
		int				opoints,	// [i] number of original points
		vector<TINPoint> *PntList )	// [i/o] list of input points (including pseudo points )
{
	int		i;
	double	xsize, ysize;
	TINPoint	ep, p_min, p_max;

	/*------------------------------------------------------------------
	#### pseudo points.
	#### prepare PSEUDO_POINT_COUNT pseudo points which are added around 
	#### the input to points to suppport the quick configulation of TIN. 
	#### points are located at the following positions.
	####  X--------------X--------------X--------------X ... p_max.y
	####  |3              4              5            6|      /\
	####  |                                            |      ||
	####  |        +----------------*---------+        | ... p_max.y
	####  |        |                        * |        |
	####  |        |     *                    |        |
	####  X 2      |                          |       7X
	####  |        |           *              *        |
	####  |        |                          |        |
	####  |        |       *           *      |        |
	####  |        |                          |        |
	####  |        |                *         |        |
	####  |        |      *                   |        |
	####  X 1      |                          |       8X
	####  |        |                *       * |        |
	####  |        |  *                       |        |
	####  |        +-----*--------------------+        | ... p_min.y
	####  |      Minimum rectangle which includes      |      ||
	####  |0       .     all input points.    .       9|      \/
	####  X--------------X--------------X--------------X ... p_min.y
	####  .        .     11             10    .        .
	####  . <===== .                          . =====> .
	####  .        .                          .        .
	####  p_min.x                          p_max.x    
	####           p_min.x                          p_max.x 
	------------------------------------------------------------------*/
	PixMiniMax( opoints, PntList, &p_min, &p_max );
	xsize = p_max.x - p_min.x;
	ysize = p_max.y - p_min.y;
	
	p_min.x = p_min.x - xsize / 2;
	p_min.y = p_min.y - ysize / 2;
	p_max.x = p_max.x + xsize / 2;
	p_max.y = p_max.y + ysize / 2;

	i = opoints;
	ep.Set( i, p_min.x, p_min.y, p_min.z );
	PntList->push_back( ep );

	i++;
	ep.Set( i, p_min.x, p_min.y + ( p_max.y - p_min.y ) / 3, (p_max.z + p_min.z) /2);
	PntList->push_back( ep );

	i++;
	ep.Set( i, p_min.x, p_min.y + ( p_max.y - p_min.y ) * 2 / 3 , (p_max.z + p_min.z) /2);
	PntList->push_back( ep );

	i++;
	ep.Set( i, p_min.x, p_max.y, (p_max.z + p_min.z) /2 );
	PntList->push_back( ep );

	i++;
	ep.Set( i, p_min.x + ( p_max.x - p_min.x ) / 3, p_max.y , (p_max.z + p_min.z) /2);
	PntList->push_back( ep );

	i++;
	ep.Set( i, p_min.x + ( p_max.x - p_min.x ) * 2 / 3, p_max.y, (p_max.z + p_min.z) /2 );
	PntList->push_back( ep );

	i++;
	ep.Set( i, p_max.x, p_max.y,p_max.z );
	PntList->push_back( ep );

	i++;
	ep.Set( i, p_max.x, p_min.y + ( p_max.y - p_min.y ) * 2 / 3, (p_max.z + p_min.z) /2 );
	PntList->push_back( ep );

	i++;
	ep.Set( i, p_max.x, p_min.y + ( p_max.y - p_min.y ) / 3 , (p_max.z + p_min.z) /2);
	PntList->push_back( ep );

	i++;
	ep.Set( i, p_max.x, p_min.y, (p_max.z + p_min.z) /2 );
	PntList->push_back( ep );

	i++;
	ep.Set( i, p_min.x + ( p_max.x - p_min.x ) * 2 / 3, p_min.y , (p_max.z + p_min.z) /2);
	PntList->push_back( ep );

	i++;
	ep.Set( i, p_min.x + ( p_max.x - p_min.x ) / 3, p_min.y, (p_max.z + p_min.z) /2 );
	PntList->push_back( ep );

	return true;
}


int iListNeighbour_TRI(
		int 			p,				// [i] point currently being looked at
		int 			opoints, 
		vector<TINPoint> *PntList,
		int 			*iBelngCount,
		CDT_Matrix<int>	*iNbrList,
		CDT_Matrix<int>	*iBlgList, 
		vector<TINTriangle> *TriVector )
{
	bool bRet;
	int i, j, k, nneib, ptr0, ptr1, ptr2, ptr3, ninside;
	int bt, npoints, nright, ptr2_in_trilist;
	short sStatus;
	double dMaxAngle, dRadiusStep, dRadius, dDistance, angle, angle2, dx, dy;
	TINTriangle tri;
	Tranquilizer trq;
	trq.SetText("Find neighbours");


	int *iPntSorted = NULL;
	int *iRight = NULL;
	int *iIncluded = NULL;

	Cp_Circle2	Circl, Circl2;
	TINPoint		ep, t_cross, t_midd, t_plus;
	Cp_lin2		TmpLine, line_ptr01;

	/*------------------------------------------------------------------
	#### iIncluded[i]	TRUE if i-th points ( include the pseudo points )  
	#### 				is already in the neighbour list around the 
	#### 				input point "p".
	#### iRight[i]		TRUE if i-th points ( include the pseudo points ) 
	#### 				is on the "right" side of the line connecting 
	#### 				"p" ( neighbour list[p][0] ) and first connected  
	#### 				point in the neighbour list. FALSE if not on the 
	#### 				right side or unknown.
	------------------------------------------------------------------*/
	
	npoints = (int)PntList->size( ); 

	iIncluded = new int[npoints];
	if( NULL == iIncluded ) {
		nneib = -1;
		goto PIX_EXIT;
	}

	iRight = new int[npoints];
	if( NULL == iRight ) {
		nneib = -1;
		goto PIX_EXIT;
	}

	for ( i = 0; i < npoints; i++ ) {
		iIncluded[i] = false;
		iRight[i] = false;
	}

	// --- create a sorted list according to the distance from p ---
	
	iPntSorted = new int[npoints];
	bRet = bSortByDistance( p, PntList, iPntSorted );
	if( !bRet ) goto PIX_EXIT;

	/*------------------------------------------------------------------
	#### set "p" itself as the first point in the neighbour list around 
	#### "p". it is also set to "ptr0".
	------------------------------------------------------------------*/
	ptr0 = p;
	nneib = 0;
	iNbrList->Set( p, nneib, ptr0 );
	iIncluded[ptr0] = true;
	nneib++;

	/*------------------------------------------------------------------
	#### decide the second point to be connected to the first. the 
	#### point is decided to be the second if no other point is 
	#### included in the circle, of which diamter is defined by the 
	#### first and the current point. 
	#### 
	#### any point which first meets this condition will be the second 
	#### point. the second point obtained is named "ptr2" and it is set 
	#### in the "neighbour list". 
	#### 
	#### ptr0: first point.
	#### ptr1: first neighbour.
	#### get_circle_end: get and return the circle in the two dimensional space, of which 
	#### two end of the diamter are on two given points.
	------------------------------------------------------------------*/
	
	ptr1 = iPntSorted[1];
	if( ptr1 >= npoints ) ptr1 = 0;
	Circl.GetCircleEnd( &PntList->at( ptr0 ), &PntList->at( ptr1 ) );

	while( true ) {
		ninside = 0;
		//trq.fUpdate(nneib);
		for ( i = 0; i < npoints; i++ ) {
			k = iPntSorted[i];
			if( k == ptr0 ) continue;
			if( k == ptr1 ) continue;
			if( Circl.bPointInCircle( &PntList->at( k ) ) ) {
				ptr1 = k;
				Circl.GetCircleEnd( &PntList->at( ptr0 ), &PntList->at( ptr1 ) );
				ninside++;
				break;
			}
		}

		if( ninside == 0 ) break;
	}

	iNbrList->Set( p, nneib, ptr1 );
	iIncluded[ptr1] = true;
	nneib++;
	
	/*------------------------------------------------------------------
	#### t_plus mugen enten
	------------------------------------------------------------------*/
	t_plus.x = PntList->at( ptr0 ).x + 10000000.0;
	t_plus.y = PntList->at( ptr0 ).y;

	while( true ) {
		/*--------------------------------------------------------------
		#### look for the triangle which include "ptr0" as vertex in 
		#### the triangle list around "ptr1". if found, and if the 
		#### third vertex of the triangle is not in the neighbour list 
		#### of ptr0, and it also is on the right side of the vector 
		#### connecting ptr0 and ptr1 ( it is equivalent to that the 
		#### angle defined by ptr0 -> ptr1 -> ( the third vertex ) is 
		#### smaller than PI ), the third vertex is decided to 
		#### be the next point in the neighbour list. 
		--------------------------------------------------------------*/
		ptr2 = -1;
		ptr2_in_trilist = false;
		for ( i = 0; i < iBelngCount[ptr1]; i++ ) {
			ptr3 = -1;
		
			bt = iBlgList->Get( ptr1, i );			
			tri = TriVector->at( bt );
	
			if( tri.GetPnt(0) == ptr0 ) {
				if( tri.GetPnt(1) == ptr1 ) {
					ptr3 = tri.GetPnt(2);
				} else if( tri.GetPnt(2) == ptr1 ) {
					ptr3 = tri.GetPnt(1);
				}
			} else if( tri.GetPnt(1) == ptr0 ) {
				if( tri.GetPnt(0) == ptr1 ) {
					ptr3 = tri.GetPnt(2);
				} else if( tri.GetPnt(2) == ptr1 ) {
					ptr3 = tri.GetPnt(0);
				}
			} else if( tri.GetPnt(2) == ptr0 ) {
				if( tri.GetPnt(0) == ptr1 ) {
					ptr3 = tri.GetPnt(1); 
				} else if( tri.GetPnt(1) == ptr1 ) {
					ptr3 = tri.GetPnt(0);
				}
			}

			if( ptr3 < 0 ) continue;
			if( bCmpNbrList( p, 1, ptr3, iNbrList ) && !bCmpNbrList( p, 2, ptr1, iNbrList ) ) {
				ptr2_in_trilist = true;
				ptr2 = ptr3;
				break;
			}

			if( iIncluded[ptr3] ) continue;
			angle = get_3angle( &PntList->at( ptr0 ), &PntList->at( ptr1 ), &PntList->at( ptr3 ) );
			if( angle < PIE ) {
				ptr2_in_trilist = true;
				ptr2 = ptr3;
				break;
			}
		}

		/*--------------------------------------------------------------
		#### searching for virgins. if no connecting point is found 
		#### using the above ( triangle ) step, try to find it from the 
		#### gemetrical conditions.
		--------------------------------------------------------------*/
		if( ptr2 < 0 ) {
			/*----------------------------------------------------------
			#### check for all points, if it is on the right side of 
			#### the vector connecting "ptr0"->"ptr1".
			----------------------------------------------------------*/
			nright = 0;
			line_ptr01.PlaneLine( &PntList->at( ptr0 ), &PntList->at( ptr1 ) );
			for ( j = 0; j < npoints; j++ ) {
				iRight[j] = false;
				if( j == ptr0 ) continue;
				if( j == ptr1 ) continue;
				if( RIGHT == line_ptr01.iWhichSideOfLine( &PntList->at( j ) ) ) {
					nright++;
					iRight[j] = true;
				}
			}

			/*----------------------------------------------------------
			#### if nopoint is found on the right side of "ptr0" -> 
			#### "ptr1", stop the searching.
			----------------------------------------------------------*/
			if( nright <= 0 ) break;

			/*----------------------------------------------------------
			### define a straight line to pass the center of ptr0 and 
			### ptr1 at the right angle to ptr0->ptr1 
			----------------------------------------------------------*/
			angle = get_3angle( &t_plus, &PntList->at( ptr0 ), &PntList->at( ptr1 ) );
			t_midd.Center( &PntList->at( ptr0 ), &PntList->at( ptr1 ) );
			angle2 = GoodRadian( angle - PIE05 );
			TmpLine.OnePointAngleLine( &t_midd, angle2 );
			dRadius = dbl2_distance( &PntList->at( ptr0 ), &PntList->at( ptr1 ) ) / 2;
			//dDegStep = 0.0;
			dRadiusStep = dRadius * DISTANCE_PITCH;
			dDistance = 0.0;
			
			/*----------------------------------------------------------
			----------------------------------------------------------*/
			sStatus = true;
			while( sStatus ) {
				/*------------------------------------------------------
				#### define a circle which pass "ptr0" and "ptr1" which 
				#### is "little bit" widened to the right of "ptr0" -> 
				#### "ptr1" fromn the previous circle.
				------------------------------------------------------*/
				dDistance = dDistance + dRadiusStep;
				trq.fUpdate(dDistance);
				t_cross = TmpLine.PointFromLineStart( dDistance );
				dx = PntList->at( ptr0 ).x - t_cross.x;
				dy = PntList->at( ptr0 ).y - t_cross.y;
				Circl2.Set( t_cross, sqrt( dx * dx + dy * dy ) );

				/*------------------------------------------------------
				#### look for the point which is on the right side of 
				#### the vector and included in the just defined circle.
				#### among these points, select the one which minimize 
				#### the angle "ptr0"->"new point"->"ptr1".
				------------------------------------------------------*/
				ninside = 0;
				ptr2 = -1;
				for ( i = 0; i < npoints; i++ ) {
					if( i == ptr0 ) continue;
					if( i == ptr1 ) continue;
					if( !iRight[i] ) continue;
					if( !Circl2.bPointInCircle( &PntList->at( i ) ) ) continue;
					angle = get_3angle( &PntList->at( ptr1 ), &PntList->at( i ), &PntList->at( ptr0 ) );
					if( 0 == ninside ) {
						dMaxAngle = angle;
						ptr2 = i;
						ninside++;
					} else if( angle >= dMaxAngle ) {
						ptr2 = i;
						dMaxAngle = angle;
						ninside++;
					}
				}

				/*------------------------------------------------------
				#### if any point which can be connected is found, break. 
				#### if not, restart the step with widened circle.
				------------------------------------------------------*/
				if( ptr2 >= 0 ) 
					sStatus = false;
			}
		}

		/*--------------------------------------------------------------
		#### welcome back!!!
		#### if the next connected point is the second point in the 
		#### neighbour list, stop the search and exit from the loop.
		--------------------------------------------------------------*/
		if( bCmpNbrList( p, 1, ptr2, iNbrList ) ) {
			if( !ptr2_in_trilist ) {
				int iTri = (int)TriVector->size( );
				TINTriangle tmptri;
				tmptri.Set( iTri, ptr0, ptr1, ptr2 );

				TriVector->push_back( tmptri );

				iBlgList->Set( ptr0, iBelngCount[ptr0], iTri );
				iBelngCount[ptr0]++;
				iBlgList->Set( ptr1, iBelngCount[ptr1], iTri );
				iBelngCount[ptr1]++;
				iBlgList->Set( ptr2, iBelngCount[ptr2], iTri );
				iBelngCount[ptr2]++;
			}
			break;
		}

		/*--------------------------------------------------------------
		#### if the connecting point is found in the second step, i.e., 
		#### it is not found from the existing triangle list.....
		--------------------------------------------------------------*/
		if( !ptr2_in_trilist ) {
			/*----------------------------------------------------------
			#### create new triangle and set "ptr0", "ptr1" and "ptr2" 
			#### as the vertices.
			----------------------------------------------------------*/
			int iTri = (int)TriVector->size( );
			TINTriangle tmptri;
			tmptri.Set( iTri, ptr0, ptr1, ptr2 );
			TriVector->push_back( tmptri );

		
			iBlgList->Set( ptr0, iBelngCount[ptr0], iTri );
			iBelngCount[ptr0]++;
			iBlgList->Set( ptr1, iBelngCount[ptr1], iTri );
			iBelngCount[ptr1]++;
			iBlgList->Set( ptr2, iBelngCount[ptr2], iTri );
			iBelngCount[ptr2]++;
		}

		// ---  add ptr2 to the neighbour list of p ---
	
		iNbrList->Set( p, nneib, ptr2 );
		iIncluded[ptr2] = true;
		nneib++;
		ptr1 = ptr2;
	}
	
	// --- DOne ---

PIX_EXIT:

	if( iRight ) delete[] iRight;
	if( iIncluded ) delete[] iIncluded;
	if( iPntSorted ) delete[] iPntSorted;

	return nneib;
}


bool bResetTriangleList_TRI(
		int				nInPoints,
		vector<int>		*iPntTables,
		vector<TINTriangle>	*TriVector,
		vector<TINTriangle>	*TriVector2 )
{
	int i, iPnt, iPtr[3];
	TINTriangle tri;
	vector <TINTriangle>::iterator Iter;

	/*------------------------------------------------------------------
	#### 三角形リストを再構成する
	------------------------------------------------------------------*/
	TriVector2->clear( );
	for ( Iter = TriVector->begin( ); Iter != TriVector->end( ); Iter++ ) {

		tri = *Iter;
		for( i = 0 ; i < 3 ; i++ ) {
			iPnt = tri.GetPnt(i);
			iPtr[i] = iPntTables->at( iPnt );
		}
		
		if( iPtr[0] >= nInPoints ) continue;
		if( iPtr[1] >= nInPoints ) continue;
		if( iPtr[2] >= nInPoints ) continue;

		tri.Set( iPtr[0], iPtr[1], iPtr[2] );  

		TriVector2->push_back( tri );
	}

	return true;
}


//======================================================================
// 
//
bool bCreateTriangleVector_TRI(	
		int				ntri,
		vector<TINTriangle> *TriVector,
		vector<TINTriangle> *TriList )
{	
	bool bRetCode = false;
	int i;

	/*------------------------------------------------------------------
	**** TRIANGLE( S )
	------------------------------------------------------------------*/
TINTriangle tri;

	TriList->clear( );
	for ( i = 0; i < ntri; i++ ) {
		tri = TriVector->at( i );
		TriList->push_back( tri );
	}

	bRetCode = true;
	return bRetCode;
}


//======================================================================
// 
// iNbrList[i][j]	
// j-th point which is currently connected to point "i". Note that this
// the sequential number of point, not a point ID. Also it is important
// that points are added to this list as it is found and it is not
// sorted by any numerical nor geometrical conditions. 
// The first point in the list is always "i" itself.
//	 	
//	 				example )   12       13
//	 	       				     *     /
//	 	     				      \  /
//	 	    				    14 *--------* 25
//	 	     				      /               
//	 	    				     /      * 19
//	 	  				        /
//	 	 				       * 20
// in this example, number of points connected to point 14 is three. 
// points connected to "14" are 12, 25 and 20.
// **** nneib[14] = 4
// **** iNbrList[14][0] = 12
// **** iNbrList[14][1] = 14
// **** iNbrList[14][2] = 25
// **** iNbrList[14][3] = 20
//
bool bCmpNbrList(
		int		row,
		int		col,
		int		value,
		CDT_Matrix<int>	*iNbrList )
{
	int data = iNbrList->Get( row, col );
	return( data == value ); 
}


//======================================================================
// 
//
bool bInitBelongList_TRI(
		int				npoints,
		CDT_Matrix<int>	*iBlgList )
{
	int n = 0;
	for ( int i = 0; i < npoints; i++ ) {
		for( int j = 0; j < npoints; j++ ) {
			iBlgList->Set( i, j, n );
		}
	}

	return true;
}


//======================================================================
// bSortByDistance
// sort list of the points according to the distance from one of the point
//
bool bSortByDistance(
		int				iCenter,		// [i] point# to be centered
		vector<TINPoint>	*PntList,		// [i] input point vector
		int				*iPntSorted )	// [o] sorted vector, must be assigned a same size as PntList
{
	bool bRetCode = false;
	int i, iSize;
	TINPoint Pnt, Cen;
	double dX, dY, dDist;

	double *dDistList = NULL;

	iSize = (int)PntList->size( );
	if( 0 > iCenter || iCenter >= iSize ) {
		goto PIX_EXIT;
	}

	dDistList = new double[iSize];
	if( NULL == dDistList ) goto PIX_EXIT;

	Cen = PntList->at( iCenter );
	for( i = 0 ; i < iSize ; i++ ) {
		Pnt = PntList->at( i );
		dX = Cen.x - Pnt.x;
		dY = Cen.y - Pnt.y;
		dDist = dX * dX + dY * dY;
		iPntSorted[i] = i;
		dDistList[i] = sqrt( dDist );
	}
	
	DoubleArrayQsort( dDistList, iSize, iPntSorted );

	// --- Done ---
	
	bRetCode = true;

PIX_EXIT:

	if( dDistList ) delete[] dDistList;

	return bRetCode;
}


bool bBoxSortPoint_TRI( vector<TINPoint>* PntList, int* iBoxSorted )
{
	int oPoints = (int)PntList->size( );
	return bBoxSortPoint_TRI( oPoints, PntList, iBoxSorted );
}

bool bBoxSortPoint_TRI( int oPoints, vector<TINPoint>* PntList, int* iBoxSorted )
{
	bool bRetCode = false;
	int iLoc, iCol, iRow, i, j, k, iRowCount, iColCount;
	double dBoxSize, dXSize, dYSize;
	TINPoint	ep, p_min, p_max;
	int *iColBelong = NULL;
	int *iRowBelong = NULL;
	int *iBelong = NULL;

	PixMiniMax( oPoints, PntList, &p_min, &p_max );
	dXSize = p_max.x - p_min.x;
	dYSize = p_max.y - p_min.y;

	dBoxSize = min( dXSize, dYSize ) / STANDARD_BOX_COUNT;

	iColCount = (int)floor( dXSize / dBoxSize );
	iRowCount = (int)floor( dYSize / dBoxSize );
	

	iColBelong = new int[oPoints];
	if( !iColBelong ) goto PIX_EXIT;

	iRowBelong = new int[oPoints];
	if( !iRowBelong ) goto PIX_EXIT;

	iBelong = new int[oPoints];
	if( !iBelong ) goto PIX_EXIT;

	for( k = 0 ; k < oPoints ; k++ ) {
		ep = PntList->at( k );
		iCol = (int)floor( ( ep.x - p_min.x ) / dBoxSize );
		iRow = (int)floor( ( ep.y - p_min.y ) / dBoxSize );
		iColBelong[k] = iCol;
		iRowBelong[k] = iRow;
		iBelong[k] = -1;
	}

	iLoc = 0;
	for( i = 0 ; i <= iRowCount ; i++ ) {
		for( j = 0 ; j <= iColCount ; j++ ) {
			for( k = 0 ; k < oPoints ; k++ ) {
				iCol = iColBelong[k];
				if( iCol != j ) continue;
				iRow = iRowBelong[k];
				if( iRow != i ) continue;
				iBelong[k] = iLoc;
				iBoxSorted[iLoc++] = k;
			}
		}
	}

	
	for( k = 0 ; k < oPoints ; k++ ) {
		if( 0 > iBelong[k] ) {
			i = -1;
		}
	}


	// --- Done ---
	
	bRetCode = true;

PIX_EXIT:

	if( iColBelong ) delete[] iColBelong;
	if( iRowBelong ) delete[] iRowBelong;
	if( iBelong ) delete[] iBelong;

	return bRetCode;
}


//----------------------------------------------------------------------
//
//
int iListConvex_TRI(
		int				iPtr,			// [i] point number on converx hull
		int				opoints,		// [i] number of original points
		vector<TINPoint>	*Pnt2List,		// [i] list of points
		vector<int>		*Cnv2List,		// [i/o] 
		int				*iNbrCount,		// [i/o] 
		CDT_Matrix<int>	*iNbrList,		// [i/o] 
		int				*iBlgCount,		// [i/o] 
		CDT_Matrix<int>	*iBlgList,		// [i/o] 
		vector<TINTriangle> *TriVector )	// [i/o] 
{
	short sFound, sPseudoFound;
	int iRot, iPrev, iCur, i, j, nNbr, iNbr, jNbr, nCnv, iCnv, iCnvNext, iNext;
	int iTri, nTri;
	double dAngle, dMinAngle;
	TINPoint Ptr0, Ptr1, Ptr2;
	TINTriangle tri;
	vector<int> Chain;
	vector<TINPoint>	Ply2List;
	vector<TINTriangle> Tri2List;

	// --- get points and the next ---

	nCnv = (int)Cnv2List->size( );
	iCnv = Cnv2List->at( iPtr );
	iCnvNext = ( iPtr == nCnv-1) ? Cnv2List->at( 0 ) :  Cnv2List->at( iPtr+1 );

	Ptr0 = Pnt2List->at( iCnv );
	Ptr1 = Pnt2List->at( iCnvNext );
	
	// --- if two points on the convex hull are already connected, leave ---
	sFound = false;
	nNbr = iNbrCount[iCnv];
	for( i = 1 ; !sFound && i < nNbr ; i++ ) { 
		iNbr = iNbrList->Get( iCnv, i );
		if( iNbr == iCnvNext ) {
			sFound = true;
		}
	}
	
	if( sFound ) goto PIX_EXIT;

	// --- find the point to be connected to the iCnv ---
	
	Chain.clear( );
	Chain.push_back( iCnv );
	
	nNbr = iNbrCount[iCnv];
	iNext = -1;
	for( i = 1 ; i < nNbr ; i++ ) {
		iNbr = iNbrList->Get( iCnv, i );
		
		// --- check if this point is connected to the pseudo points ---
		sPseudoFound = false;
		for( j = 1 ; !sPseudoFound && j < iNbrCount[iNbr] ; j++ ) {
			jNbr = iNbrList->Get( iNbr, j );
			if( jNbr >= opoints ) sPseudoFound = true;
		}
		if( !sPseudoFound ) continue;

		Ptr2 = Pnt2List->at( iNbr );
		dAngle = get_3angle( &Ptr1, &Ptr0, &Ptr2 );
		//if( PIE / 2.0 < dAngle ) continue;
		if( 0 > iNext ) {
			dMinAngle = dAngle;
			iNext = iNbr;
		} else if( dMinAngle > dAngle ) {
			dMinAngle = dAngle;
			iNext = iNbr;
		}
	}

	if( 0 > iNext ) goto PIX_EXIT;
	Chain.push_back( iNext );

	// --- follow the chain until the next point on the hull is found ---
	
	sFound = false;
	iPrev = iCnv;
	while( !sFound ) {
		iCur = iNext;
		Ptr0 = Pnt2List->at( iPrev );
		Ptr1 = Pnt2List->at( iCur );
		nNbr = iNbrCount[iCur];
		iNext = -1;
		for( i = 1 ; !sFound && i < nNbr ; i++ ) {
			iNbr = iNbrList->Get( iCur, i );
			if( iNbr >= opoints ) continue;
			if( iNbr == iPrev ) continue;
			if( iNbr == iCnvNext ) {
				// --- the node is connected to the next point in the hull ---
				Chain.push_back( iCnvNext );
				sFound = true;
				continue;
			}

			// --- check if iNbr is connected to the pseudo points ---
			sPseudoFound = false;
			for( j = 1 ; !sPseudoFound && j < iNbrCount[iNbr] ; j++ ) {
				jNbr = iNbrList->Get( iNbr, j );
				if( jNbr >= opoints ) sPseudoFound = true;
			}
			if( !sPseudoFound ) continue;
	
			Ptr2 = Pnt2List->at( iNbr );
			dAngle = get_3angle( &Ptr0, &Ptr1, &Ptr2 );
			//if( PIE < dAngle ) continue;
			if( 0 > iNext ) {
				dMinAngle = dAngle;
				iNext = iNbr;
			} else if( dMinAngle > dAngle ) {
				dMinAngle = dAngle;
				iNext = iNbr;
			}
		}

		if( sFound ) continue;
	
		if( 0 > iNext ) goto PIX_EXIT;
		Chain.push_back( iNext );
		iPrev = iCur;
	}

	// --- construct polygon ---
	nNbr = (int)Chain.size( );

	if( 3 > nNbr ) goto PIX_EXIT;
	if( 3 == nNbr ) {
		tri.Set( (int)TriVector->size( ), (int)Chain.at( 0 ), (int)Chain.at( 1 ), (int)Chain.at( 2 ) );
		TriVector->push_back( tri );
		goto PIX_EXIT;
	}

	Ply2List.clear( );
	for( i = 0 ; i < nNbr ; i++ ) {
		iNbr = Chain.at( i );
		Ptr0 = Pnt2List->at( iNbr );
		Ptr0.SetID( iNbr );
		Ply2List.push_back( Ptr0 );
	}

	// --- triangulate the polygon ---
	Tri2List.clear( );
	iRot = iPolygonRotDirection( &Ply2List );
	if( !bPolygonSwapRotation( &Ply2List, true ) ) goto PIX_EXIT;
	StartPolygonTri_TRI( &Ply2List, &Tri2List );

	// --- add to triangle lists ---
	
	nTri = (int)Tri2List.size( );
	for( i = 0 ; i < nTri ; i++ ) {
		iTri = (int)TriVector->size( );
		tri = Tri2List.at( i );
		tri.SetID( iTri );
		TriVector->push_back( tri );
	}

	// --- Done ---
PIX_EXIT:

	Chain.clear( );
	Tri2List.clear( );
	return 0;
}