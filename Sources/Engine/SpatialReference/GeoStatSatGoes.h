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

 Created on: 2020-03-16
 ***************************************************************/
/* ProjectionGeoStationarySatelliteGoes
   Copyright Ilwis System Development ITC
   March 2020, by Ambro Gieske and Jan Hendrikse
 */

#ifndef ILWGEOSTATGOES_H
#define ILWGEOSTATGOES_H
#include "Engine\SpatialReference\AZIMUTHA.H"

class _export ProjectionGeoStationarySatelliteGoes: public ProjectionAzimuthal
{
public:
	ProjectionGeoStationarySatelliteGoes(const Ellipsoid&); 
	virtual XY xyConv(const PhiLam&) const;
	virtual PhiLam plConv(const XY&) const;
	virtual void Prepare();
	//Datum* datumDefault() const;
	bool fLatLonInVisible(const PhiLam pl) const;

private:
	double rSin2Phi0;
	double rDisttoCenter;
	double a, e2, b;
	double rLatitudeCorrFactor;
	double rHorizonRadius;// spehre
	double rVisibleHalfArc; // greatcircle arc on sphere 
};

#endif // ILWGEOSTATGOES_H






