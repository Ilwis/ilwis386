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

 Created on: 2014-05-02
 ***************************************************************/

#ifndef ILWMapProbabilityDensity_H
#define ILWMapProbabilityDensity_H

#include "Engine\Applications\MAPVIRT.H"

IlwisObjectPtr * createMapProbabilityDensity(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms=vector<void*>() );

class DATEXPORT MapProbabilityDensity: public MapVirtual
{
public:  
  static const char* sSyntax();  
  virtual bool fFreezing();
  virtual String sExpression() const;
  static MapProbabilityDensity* create(const FileName& fn, MapPtr& p, const String& sExpr);
  MapProbabilityDensity(const FileName& fn, MapPtr& p);
protected:
  virtual void Store();
  MapProbabilityDensity(const FileName& fn, MapPtr& p, const GeoRef& gr, Coord & _crdwgs, double _distance, double _sigma_distance, double _direction, double _sigma_direction);
  ~MapProbabilityDensity();
  
private:
  static GeoRef CreateGeoRef(const FileName & fnObj, const Coord & crdwgs, double distance, double sigma_distance, double direction, double sigma_direction);
  static CoordSystem pay_utmzone_from_point(const Coord & p);
  double pay_modified_besseli_zero(double x);
  double pay_modified_besseli_one(double x);
  double pay_kappa_estimation(double sigma_input);
  double pay_probability_density_circular(double distance, double dist_sigma, double azimuth, double direction_mean, double kappa);
  double st_area(Coord & c1, Coord & c2, Coord & c3, Coord & c4);
  double st_azimuth(Coord & c1, Coord & c2);
  Coord crdwgs;
  double distance;
  double sigma_distance;
  double direction;
  double sigma_direction;
  static const double scalewgs;
};

#endif // ILWMapProbabilityDensity_H




