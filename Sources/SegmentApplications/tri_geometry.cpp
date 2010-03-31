#include "Headers\baseconst.h"
#include "tricommon.h"

/**********************************************************************/
/**********************************************************************/
/*** TINPoint **********************************************************/
/**********************************************************************/
/**********************************************************************/

TINPoint::TINPoint( void )
{
	this->id = 0;
	this->x = NODATA;
	this->y = NODATA;
	return;
}

TINPoint::TINPoint( int idx, double xx, double yy, double zz )
{
	Set( idx, xx, yy,zz );
	return;
}

void TINPoint::Set( int idx, double xx, double yy, double zz )
{
	this->id = idx;
	this->x = xx;
	this->y = yy;
	this->z = zz;
	return;
}

void TINPoint::Center( TINPoint *p0, TINPoint *p1 )
{
	this->id = ( p0->id + p1->id ) / 2;
	this->x = ( p0->x + p1->x ) / 2.0;
	this->y = ( p0->y + p1->y ) / 2.0;
	return;
}

TINPoint &TINPoint::operator=(const TINPoint &that )
{
	this->id = that.id;
	this->x = that.x;
	this->y = that.y;
	return *this;
}


/**********************************************************************/
/**********************************************************************/
/*** TINPoint3D **********************************************************/
/**********************************************************************/
/**********************************************************************/

//TINPoint3D::TINPoint3D( void )
//{
//	this->id = 0;
//	this->x = NODATA;
//	this->y = NODATA;
//	this->z = NODATA;
//	return;
//}
//
//TINPoint3D::TINPoint3D(	int		idx,
//					double	xx,
//					double	yy,
//					double	zz )
//{
//	Set( idx, xx, yy, zz );
//	return;
//}
//
//void TINPoint3D::Set( TINPoint *ep2 )
//{
//	Set( ep2->GetID( ), ep2->x, ep2->y, 0.0 );
//	return;
//}
//
//void TINPoint3D::Set(	int		idx,
//					double	xx,
//					double	yy,
//					double	zz  )
//{
//	this->id = idx;
//	this->x = xx;
//	this->y = yy;
//	this->z = zz;
//	return;
//}
//
//TINPoint3D &TINPoint3D::operator=(const TINPoint3D &that )
//{
//	this->id = that.id;
//	this->x = that.x;
//	this->y = that.y;
//	this->z = that.z;
//	return *this;
//}

/**********************************************************************/
/**********************************************************************/
/*** Cp_Circle2 *******************************************************/
/**********************************************************************/
/**********************************************************************/

//----------------------------------------------------------------------
// get the circle with a given center coo and radius
//
void Cp_Circle2::Set( 
		TINPoint		Center, 
		double		dRadius )
{
	m_Center = Center;
	m_dRadius = dRadius;
	
	m_dSqRaduis = m_dRadius * m_dRadius;

	m_Cmin.x = m_Center.x - m_dRadius;
	m_Cmin.y = m_Center.y - m_dRadius;
	m_Cmax.x = m_Center.x + m_dRadius;
	m_Cmax.y = m_Center.y + m_dRadius;

	return;
}

//----------------------------------------------------------------------
// get and return the circle in the two dimensional space, of which two 
// end of the diamter are on two given points.
//
void Cp_Circle2::GetCircleEnd(
		TINPoint	*pnt0,	// [i] first point on the diameter	 
		TINPoint	*pnt1 )	// [i] first point on the diameter
{
	m_Center.x = ( pnt0->x + pnt1->x ) / 2.0;
	m_Center.y = ( pnt0->y + pnt1->y ) / 2.0;
	m_dRadius = dbl2_distance( pnt0, pnt1 ) / 2.0;

	m_dSqRaduis = m_dRadius * m_dRadius;

	m_Cmin.x = m_Center.x - m_dRadius;
	m_Cmin.y = m_Center.y - m_dRadius;
	m_Cmax.x = m_Center.x + m_dRadius;
	m_Cmax.y = m_Center.y + m_dRadius;

	return;
}

//----------------------------------------------------------------------
// check if the given point is included or on the primeter of the  
// given angle.
//
bool Cp_Circle2::bPointInCircle( TINPoint *point )
{
	if( point->x < m_Cmin.x ) return false;
	if( point->x > m_Cmax.x ) return false;
	if( point->y < m_Cmin.y ) return false;
	if( point->y > m_Cmax.y ) return false;

	double dX = point->x - m_Center.x;
	double dY = point->y - m_Center.y;
	double dDist = dX * dX + dY * dY;
	
	return dDist <= m_dSqRaduis;
}

/**********************************************************************/
/**********************************************************************/
/*** Cp_lin2 **********************************************************/
/**********************************************************************/
/**********************************************************************/

Cp_lin2::Cp_lin2( void )
{
	this->id = 0;
	this->type = 0;
	this->x0 = 0;
	this->y0 = 0;
	this->ff = 0; 
	this->gg = 0;
	SetParameters( );
	return;
}

void Cp_lin2::SetParameters( )
{
	if( 0.0 == ff ) {
		gg_ff = NODATA;
	} else {
		gg_ff = gg / ff;
	}

	return;
}


int	Cp_lin2::iWhichSideOfLine( TINPoint *pnt )
{
int side;
	
	if( ff == 0.0 ) {
		if( gg > 0 ) {
			if( pnt->x < x0 ) {
				side = LEFT;
			} else if ( pnt->x > x0 ) {
				side = RIGHT;
			} else {
				side = UPPER;
			}
		} else {
			/* ‚’¼‰ºŒü‚« */
			if( pnt->x < x0 ) {
				side = RIGHT;
			} else if ( pnt->x > x0 ) {
				side = LEFT;
			} else {
				side = UPPER;
			}
		}
	} else {
		double xd = pnt->x - x0;
		double yd = pnt->y - y0;
		double det = yd - gg_ff * xd;
		if( ff > 0 ) {
			if( det > 0 ) {
				side = LEFT;
			} else if ( det < 0 ) {
				side = RIGHT;
			} else {
				side = UPPER;
			}
		} else {
			if( det > 0 ) {
				side = RIGHT;
			} else if ( det < 0 ) {
				side = LEFT;
			} else {
				side = UPPER;
			}
		}
	}

	return side;
}


void Cp_lin2::OnePointAngleLine( 
		TINPoint	*point,  
		double	angle )
{
TINPoint	 second;
	second.x = point->x + 1.0 * cos( angle );
	second.y = point->y + 1.0 * sin( angle );
	second.z = point->z;
	this->PlaneLine( point, &second, true );
	
	SetParameters( );
	return;
}


TINPoint Cp_lin2::PointFromLineStart( double dist )
{
	TINPoint pp( 0, this->x0 + dist * this->ff, this->y0 + dist * this->gg, this->z );
	return pp;
}


bool Cp_lin2::PlaneLine(	
		TINPoint	*pk, 
		TINPoint	*pl, 
		int		normalize /* = TRUE */ )
{
double det;

	if( pk->x == pl->x && pk->y == pl->y ) return false;

	this->type = 0;
	this->x0 = pk->x;
	this->y0 = pk->y;
	this->z = pk->z;
	this->ff = pl->x - pk->x; 
	this->gg = pl->y - pk->y; 

	if( 0.0 == this->ff && 0.0 == this->gg ) return false;

	/*--------------------------------------------------------------
	******** add following three ( 3 ) lines to normalize the
	******** parameters.
	--------------------------------------------------------------*/
	if( normalize ) {
		det = sqrt( this->ff * this->ff + this->gg * this->gg );
		if( det == 0.0 ) return false;
		this->ff /= det;
		this->gg /= det;
	}

	SetParameters( );

	return true;
}