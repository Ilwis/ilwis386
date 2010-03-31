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
/* ProjectionGoodeInterrupted
   Copyright Ilwis System Development ITC
   may 2005, by Jan Hendrikse
	Last change:  JHE   9 June 2001    1:10 pm
*/
#include "Engine\SpatialReference\GoodeINT.h"

ProjectionGoodeInterrupted::ProjectionGoodeInterrupted(const Ellipsoid& ell)
: ProjectionPtr(ell)
{
	fUseParam[pvLON0] = false;
	fEll = true;
}

#define C1R     .90031631615710606956  // == sqrt(8) / M_PI
#define C2R     1.41421356237309504880 // == sqrt(2)
#define EPS     1e-15
#define EPS10   1e-10
#define NITER   10
#define Y_COR		0.05280  // Y-correction for mollweide
const double PI_18 = (M_PI/18); // 10 degrees
// define central meridians for 2 different north gores
const	double rCM_N_100west = -10*PI_18;
const	double rCM_N_30east = 3*PI_18;
// define central meridians for 4 different south gores
const	double rCM_S_160west = -16*PI_18;
const	double rCM_S_60west = -6*PI_18;
const	double rCM_S_20east = 2*PI_18;
const	double rCM_S_140east = 14*PI_18;

void ProjectionGoodeInterrupted::Prepare()
{
	prjSinusoidal = new ProjectionCylindSinusoidal(ell); 
	prjMollweide = new ProjectionMollweide(ell);
	// find the latitude of true scale for Mollw projection on given ell
	prjMollweide->Prepare();
		rPhiJoin = rLatitudeAtJoin();
	PhiLam plJoin;
	plJoin.Phi = rPhiJoin;
	plJoin.Lam = 0.0;
	XY xyJoin = prjMollweide->xyConv(plJoin);
	Ymollweide_at_Join = xyJoin.y;
	double rSinusYcoordAtJoin = prjSinusoidal->xyConv(plJoin).y;
	Ycorr = Ymollweide_at_Join - rSinusYcoordAtJoin;	
///	prjSinusoidal->lam0 = rCM_N_100west;
///	prjMollweide->lam0 = rCM_N_100west;
}


XY ProjectionGoodeInterrupted::xyConv(const PhiLam& _pl) const
{
  XY xy;
	PhiLam pl = _pl;
	if (abs(pl.Phi)> M_PI_2) return xy;// beyond poles -/+ 90
	if (abs(pl.Lam)> M_PI) return xy; // beyond limiting meridians -/+180
	
	double LamCentral;
	if (pl.Phi > 0) { // northern hemisphere	
		 // Northern Hemisphere splitted in 2 parts (gores):
		if (pl.Lam < - 4 * PI_18) { // west of 40W    CM = 100W = -10*PI_18 
				LamCentral = rCM_N_100west;
				pl.Lam -= LamCentral;
		}
		else	{											// east of 40W  CM =30E = 3*PI_18 
				LamCentral = rCM_N_30east;
				pl.Lam -= LamCentral;
				//prjSinusoidal->lam0 = rCM_N_30east;
				//prjMollweide->lam0 = rCM_N_30east;
				///lam0 = rCM_N_30east;
				//prjSinusoidal->x0 = rCM_N_30east;
				//prjMollweide->x0 = rCM_N_30east;
				///x0 = rCM_N_30east;
		}
		// PhiJoin = 40:44':11.8"  = 40.7366111111 = 0.71098798999339450624749364956177
		// according to D.R.Steinwand, "Mapping Raster Imagery to the Interrup Goode
		// Int Journ of Remote Sensing , 1994 Vol. 15, No. 17, 3463 -3471
		if (abs(pl.Phi) > rPhiJoin) { // apply MOLLWEIDE	
			xy = prjMollweide->xyConv(pl);
			xy.y -= Ycorr; 
			xy.x += LamCentral;
		}	
		else { // else apply SINUSOIDAL if pl.Phi < 40:44N
			xy = prjSinusoidal->xyConv(pl);
			xy.x +=  LamCentral;
		}
	}
	else { //southern hemisphere
		 // Southern Hemisphere splitted in 4 parts (gores):
		if (pl.Lam < - 10 * PI_18) { // west of 100W    CM =160W = -16*PI_18 
			LamCentral = rCM_S_160west;
			pl.Lam -= LamCentral;
		}
		else if	(pl.Lam < - 2 * PI_18){// between 100W. 20W  CM = 60W = -6*PI_18 
			LamCentral = rCM_S_60west;
			pl.Lam -= LamCentral;
		}
		else if (pl.Lam < 8 * PI_18) { // between 20W and 80E CM = 20E = 2*PI_18
			LamCentral = rCM_S_20east;
			pl.Lam -= LamCentral;
		}
		else	{												// east of 80E  CM = 140E = 14*PI_18
			LamCentral = rCM_S_140east;
			pl.Lam -= LamCentral;
		}
		
		if (pl.Phi < - rPhiJoin) { // apply MOLLWEIDE
			xy = prjMollweide->xyConv(pl);
			xy.y += Ycorr; 
			xy.x += LamCentral;
		}	
		else { // else apply SINUSOIDAL if  -40:44S  < pl.Phi < 0
			xy = prjSinusoidal->xyConv(pl);
			xy.x +=  LamCentral;
		}
	}
  return xy;
}

PhiLam ProjectionGoodeInterrupted::plConv(const XY& xy) const
{
  PhiLam pl;
	XY _xy = xy;
	double xyYmoll = xy.y; //modifieded Y for mollw
	bool fNorth = (xyYmoll > 0); 
	xyYmoll += fNorth ? Ycorr : -Ycorr;
	double XCentral, XLeftLimit, XRightLimit;
	double cosfi = cos(xy.y);
	double trylam;
	if (fNorth) {
		 // Northern Hemisphere splitted in 2 parts:
		if (xy.x < - 4 * PI_18) { // west of 40W    CM = 100W = -10*PI_18 
				//prjSinusoidal->lam0 = rCM_N_100west;
				XCentral = rCM_N_100west;
				_xy.x -= XCentral;
				XLeftLimit = -M_PI;
				XRightLimit = -4*PI_18;
		}
		else	if (xy.x > - 4 * PI_18 && xy.x < M_PI){// east of 40W  CM =30E = 3*PI_18 
				XCentral = rCM_N_30east;
				_xy.x -= XCentral;
				XLeftLimit = -4*PI_18;
				XRightLimit = M_PI;
		}
		else return pl; 
		if (xy.y > rPhiJoin) { //  apply MOLLWEIDE
			double th, s;
			th = xyYmoll / C2R;
			s = abs(th);
			if (s < 1) {
				th = asin(th);
				trylam = (xy.x - XCentral) / (C1R * cos(th)) + XCentral;
				if (trylam < XLeftLimit || trylam > XRightLimit) return pl;
				pl.Lam = trylam;
				th += th;
				pl.Phi = asin((th + sin(th)) / M_PI);
			} 
			else if ((s - EPS10) > 1.)
				{}
			else {
				pl.Lam = 0.;
				pl.Phi = th < 0. ? -M_PI_2 : M_PI_2;
			}
		}
		else { // apply SINUSOIDAL within -40:44,40:44
			if (cosfi < EPS10) return pl;
			trylam =  (xy.x - XCentral) /cosfi + XCentral;
			//trylam =  xy.x /cosfi;
			if (trylam < XLeftLimit || trylam > XRightLimit) return pl;
			//prjSinusoidal->lam0 = XCentral;
			
			pl = prjSinusoidal->plConv(_xy);
			pl.Lam += XCentral;
			//pl.Phi = xy.y;
			//pl.Lam = trylam; 
		}
	}
	else
	{		// Southern Hemisphere splitted in 4 parts:
		if (xy.x < - 10* PI_18) { // west of 100W    CM =160W = -16*PI_18 
				XCentral = -16*PI_18;
				_xy.x -= XCentral;
				XLeftLimit = -M_PI;
				XRightLimit = -10*PI_18;
		}
		else if (xy.x < - 2 * PI_18){	// between 100W. 20W  CM = 60W = -6*PI_18 
				XCentral = -6*PI_18;
				_xy.x -= XCentral;
				XLeftLimit = -10*PI_18;;
				XRightLimit = -2*PI_18;
		}
		else if (xy.x < 8 * PI_18) { // between 20W and 80E CM = 20E = 2*PI_18
				XCentral = 2*PI_18;
				_xy.x -= XCentral;
				XLeftLimit = -2*PI_18;;
				XRightLimit = 8*PI_18;
		}
		else										{			// east of 80E  CM = 140E = 14*PI_18
				XCentral = 14*PI_18;
				_xy.x -= XCentral;
				XLeftLimit = 8*PI_18;
				XRightLimit = M_PI;
		}
		
		if (xy.y < - rPhiJoin) { //  apply MOLLWEIDE
				double th, s;
				th = xyYmoll / C2R;
				s = abs(th);
				if (s < 1) {
					th = asin(th);
					trylam = (xy.x - XCentral) / (C1R * cos(th)) + XCentral;
					if (trylam < XLeftLimit || trylam > XRightLimit) return pl;
					pl.Lam = trylam;
					th += th;
					pl.Phi = asin((th + sin(th)) / M_PI);
				} 
				else if ((s - EPS10) > 1.)
					{}
				else {
					pl.Lam = 0.;
					pl.Phi = th < 0. ? -M_PI_2 : M_PI_2;
				}
		}
		else { // apply SINUSOIDAL within -40:44,40:44
				if (cosfi < EPS10) return pl;
				trylam =  (xy.x - XCentral) /cosfi + XCentral;
				if (trylam < XLeftLimit || trylam > XRightLimit) return pl;
				pl = prjSinusoidal->plConv(_xy);
				pl.Lam += XCentral;
				//pl.Phi = xy.y;
				//pl.Lam = trylam;
				//pl.Lam += XCentral;
		}
  }
	return pl;
}

Datum* ProjectionGoodeInterrupted::datumDefault() const
{
  return new MolodenskyDatum("WGS 1984", "");
}

double ProjectionGoodeInterrupted::rLatitudeAtJoin()
{ 
	return prjMollweide->rLatitudeWithTrueScale();
}
