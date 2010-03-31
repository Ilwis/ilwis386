#ifndef _TRI_COMMON_H_
#define _TRI_COMMON_H_


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <iostream> 
#include <string> 
#include <vector>                   
#include <algorithm>
#include "Headers\baseconst.h"
using namespace std;   

#define DXPORT 
#define EXT_CLASS_TYPE  

#include "DT_Matrix.h"



#ifndef max
#define max( a, b )	(((double)(a) > (double)(b)) ? (a) : (b))
#endif max

#ifndef min
#define min(a,b)	(((double)(a) < (double)(b)) ? (a) : (b))
#endif min

#define PIE		( 3.14159265358979323846 )	
#define PIE05	( 1.57079632679489661923 )	
#define PIE2	( 6.28318530717958647692 )	
#define PIE15	( 4.71238898038468985769 )	

#define LEFT				( 0 )
#define RIGHT				( 1 )
#define UPPER				( 2 )
#define LOWER				( 3 )
#define ACCURACY			( 1.0e-8 )
#define NODATA				( -99999999.0 )

#define ROTATION_CLOCKWISE		( 0 )	// CLOCLWISE
#define ROTATION_COUNTER		( 1 )	// COUNTER_CLOCKWISE
#define ROTATION_UNKNOWN		( 2 )	// UNKNOWN

#define FILE_NAME_SIZE			( 256 )	
#define STANDARD_BUFFER_SIZE	( 256 )
#define DELIMITOUT_SIZE			( 2000 )  


class EXT_CLASS_TYPE TINPoint 
{
private:
	int		id;
public:
	double	x;
	double	y;
	double  z;

public:
	TINPoint( void ); 
	TINPoint( int, double x, double y, double z);
	void	Set( int, double x, double y, double z);
	//void	Set( TINPoint3D* );
	void	Center( TINPoint *p0, TINPoint *p1 );
	TINPoint &operator=(const TINPoint &that);
	int GetID( void ) { return id; }
	double GetX( void ) { return x; }
	double GetY( void ) { return y; }
	double GetZ( void ) { return z; }
	void SetID( int i ) { id = i; }
};


class EXT_CLASS_TYPE Cp_lin2 
{
private:
	int				id;
	unsigned char	type;
	double			x0;
	double			y0;
	double			z;
	double			ff;
	double			gg;
	double			gg_ff;	
							   
public:
	Cp_lin2( void );
	void		OnePointAngleLine( TINPoint *point, double angle );
	bool		PlaneLine( TINPoint*, TINPoint*, int normalize = true );
	void		SetParameters( );
	TINPoint		PointFromLineStart( double );
	int			iWhichSideOfLine( TINPoint *pnt );
};



class EXT_CLASS_TYPE Cp_Circle2
{
private:
	int			m_iId;			// ID Number	
	TINPoint		m_Center;		// coordinate of the center	
	double		m_dRadius;		// radius	
	TINPoint		m_Cmin;			// lower left corner of the bounding box	
	TINPoint		m_Cmax;			// upper right corner of the bounding box
	double		m_dSqRaduis;	// square of the radius

public:
	void Set( TINPoint, double ); 	
	void GetCircleEnd( TINPoint *pnt0, TINPoint *pnt1 );
	//void GetCircleEnd( TINPoint3D *pnt0, TINPoint3D *pnt1 );
	bool bPointInCircle( TINPoint *point );
	//bool bPointInCircle( TINPoint3D *point );
};


class EXT_CLASS_TYPE TINTriangle 
{
private:	
	int	id;	
	int	point[3];

public:
	int iGetID( void ) { return id; }
	int GetPnt( int i ) { return ( 0 > i || 3 <= i ) ? -1 : point[i]; }
	void SetID( int n ) { id = n; }
	void Set( int n, int p0, int p1, int p2 ) { id = n; point[0] = p0; point[1] = p1; point[2] = p2; }
	void Set( int p0, int p1, int p2 ) { Set( id, p0, p1, p2 ); }
};


/*======================================================================
**** pix_algebra.cpp
======================================================================*/

DXPORT double	GoodRadian( double );
//DXPORT bool		ConvertPntList( vector<TINPoint>*, vector<TINPoint3D>* );
//DXPORT bool		ConvertPntList( vector<TINPoint3D>*, vector<TINPoint>* );
DXPORT void		DoubleArrayQsort( double*, int, int* );
DXPORT void		DoubleQsx( double*, int, int, int* );
DXPORT int		iPolygonRotDirection( vector<TINPoint>* );
DXPORT int		iPolygonRotDirection( int, TINPoint* );

DXPORT double	get_3angle( TINPoint*, TINPoint*, TINPoint* );
DXPORT double	get_anglex_diff( double, double );
DXPORT double	get_angle_atan( double, double, double, double );
DXPORT double	get_angle_atan( TINPoint*, TINPoint* );

DXPORT int		CreateConvexHull( vector<TINPoint> *PntList, vector<int> *ConvHull );
DXPORT int		CreateConvexHull( int nPoints, vector<TINPoint> *PntList, vector<int> *ConvHull );
DXPORT int		MinDicrionaryDbl2( int nPoints, vector<TINPoint> *points );
DXPORT double	dbl2_distance( double, double, double, double );
DXPORT double	dbl2_distance( TINPoint*, TINPoint* );
DXPORT bool		bPolygonSwapRotation( vector<TINPoint> *vers, bool );
DXPORT bool		bPolygonSwapRotation( int, TINPoint*, bool );

DXPORT void		PixMiniMax( vector<TINPoint>*, TINPoint*, TINPoint* );
DXPORT void		PixMiniMax( int, vector<TINPoint>*, TINPoint*, TINPoint* );

/*======================================================================
#### pix_utility.cpp
======================================================================*/
			
DXPORT void		chr_translate( char*, char, char );
DXPORT int		iDelimitStr_PIX( char*, char *str[], char, int );

/*======================================================================
**** pix_triangulation.cpp
======================================================================*/

DXPORT int iPsuedoPointCount_TRI( void );
//DXPORT bool bLoadNodeFile_TRI( char* szNodeFile, vector<TINPoint>* );
//DXPORT bool bLoadNodeFile_TRI( char* szNodeFile, vector<TINPoint3D>* );
//DXPORT bool bSaveNodeFile_TRI( char* szNodeFile, vector<TINPoint> );
//DXPORT bool bSaveNodeFile_TRI( char* szNodeFile, vector<TINPoint3D> );
//DXPORT bool bLoadTriFile_TRI( char *szNodeFile, vector<TINTriangle> *TriList );
//DXPORT bool bSaveTriFile_TRI(char* szTriFile, vector<TINTriangle> *TriList);
DXPORT void GenerateRandomSample_TRI( char *szFile );

DXPORT int iPointsRelation_TRI( TINPoint *p0, TINPoint *p1, TINPoint *p2 );
DXPORT int iPointsRelation_TRI( vector<TINPoint> *Pnt2List, int i0, int i1, int i2 );
DXPORT bool bCheckPropIntersection_TRI( TINPoint*, TINPoint*, TINPoint*, TINPoint* );
DXPORT bool bBetweenSegments_TRI( TINPoint *p0, TINPoint	*p1, TINPoint *p2 );	
DXPORT bool bCheckIntersection_TRI( TINPoint *p0, TINPoint *p1, TINPoint *p2, TINPoint	*p3 );
DXPORT bool bCheckIntersection_TRI( vector<TINPoint>*, int i0, int i1, int i2, int i3 );
DXPORT bool bCheckDiagonalie_TRI( vector<TINPoint> *Pnt2List, int i, int j );
DXPORT bool bCheckCone_TRI( vector<TINPoint> *Pnt2List, int i, int j );
DXPORT bool bCheckDiagonal_TRI( vector<TINPoint> *Pnt2List, int i, int j );
DXPORT int	iTriangulatePolygon_TRI( vector<TINPoint>*, vector<TINTriangle>* );
DXPORT void StartPolygonTri_TRI( vector<TINPoint> *Pnt2List, vector<TINTriangle> *Tri2List );

DXPORT bool	bSetPseudoPoints_TRI( int, vector<TINPoint>* );
DXPORT int	iListNeighbour_TRI( int, int, vector<TINPoint>*, int*,   
				CDT_Matrix<int>*, CDT_Matrix<int>*, vector<TINTriangle>* );
DXPORT int	iListConvex_TRI(int iPtr, int opoints, vector<TINPoint>* Pnt2List, 
				vector<int>* Cnv2List, int*, CDT_Matrix<int>*, int*, CDT_Matrix<int>*, 
				vector<TINTriangle>* );
DXPORT bool	bInitNeighbourList( int, CDT_Matrix<int>* );
DXPORT bool	bCmpNbrList( int, int, int, CDT_Matrix<int>* );	
DXPORT bool	bResetTriangleList_TRI( int, vector<int>*, vector<TINTriangle>*, vector<TINTriangle>* );
DXPORT bool	bCreateTriangleVector_TRI( int, vector<TINTriangle>*, vector<TINTriangle>* );
DXPORT bool	bInitBelongList_TRI( int, CDT_Matrix<int>* );
DXPORT bool	bSortByDistance( int, vector<TINPoint>*, int* );
DXPORT bool	bBoxSortPoint_TRI( vector<TINPoint>* PntList, int* iBoxSorted );
DXPORT bool	bBoxSortPoint_TRI( int, vector<TINPoint>* PntList, int* iBoxSorted );


#endif _TRI_COMMON_H_
