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
/* CoordSystemViaLatLon
   Copyright Ilwis System Development ITC
   april 1998, by Wim Koolhoven
	Last change:  WK   10 Jun 98    5:55 pm
*/

#include "Engine\SpatialReference\csviall.h"
#include "Engine\SpatialReference\DATUM.H"

CoordSystemViaLatLon::CoordSystemViaLatLon(const FileName& fn)
: CoordSystemPtr(fn), datum(0)
{
  String s;
  if (ReadElement("CoordSystem", "Ellipsoid", s)) 
  {
    if (fCIStrEqual("User Defined" , s))
    {
      double a, f1;
      ReadElement("Ellipsoid", "a", a);
      ReadElement("Ellipsoid", "1/f", f1);
      ell = Ellipsoid(a,f1);
    }
    else {
      ell = Ellipsoid(s);
      if (ell.fSpherical()) {
        double a = ell.a;
        if (ReadElement("CoordSystem", "Sphere Radius", a))
          ell = Ellipsoid(a,0);
      }
    }
  }
  s = "";
  if (ReadElement("CoordSystem", "Datum", s)) 
  {
    if (fCIStrEqual("User Defined" , s.sSub(0,12)))
    {
			double dx, dy, dz;
			String sType;
      ReadElement("Datum", "dx", dx);
      ReadElement("Datum", "dy", dy);
      ReadElement("Datum", "dz", dz);
			ReadElement("Datum", "Type", sType);
			if (fCIStrEqual("User Defined B" , sType.sSub(0,14)))//for BursaWolf & Badekas
			{
				double rRx, rRy, rRz, rDs;
				ReadElement("Datum", "rotX", rRx);
				ReadElement("Datum", "rotY", rRy);
				ReadElement("Datum", "rotZ", rRz);
				ReadElement("Datum", "dS", rDs);	
				if (fCIStrEqual("User Defined Badekas" , sType.sSub(0,20)))
				{
					double X0, Y0, Z0;
					ReadElement("Datum", "X0", X0);
					ReadElement("Datum", "Y0", Y0);
					ReadElement("Datum", "Z0", Z0);
					datum = new BadekasDatum(ell, dx, dy, dz, rRx, rRy, rRz, rDs, X0, Y0, Z0);
				}
				else
					datum = new BursaWolfDatum(ell, dx, dy, dz, rRx, rRy, rRz, rDs);
			}
      else
				datum = new MolodenskyDatum(ell, dx, dy, dz);
    }
    else 
    {
      String sArea;
      ReadElement("CoordSystem", "Datum Area", sArea);
      datum = new MolodenskyDatum(s, sArea);
      ell = datum->ell;
    }
  }
}

CoordSystemViaLatLon::CoordSystemViaLatLon(const FileName& fn, int iOpt)
: CoordSystemPtr(fn,iOpt), datum(0)
{
}

CoordSystemViaLatLon::~CoordSystemViaLatLon()
{
  if (datum)
    delete datum;
}

String CoordSystemViaLatLon::getIdentification(bool wkt) {
	if ( datum){
		return datum->getIdentification(wkt);
	} else {
		return ell.getIdentification(wkt);
	}
	return CoordSystemPtr::getIdentification(wkt);
}

void CoordSystemViaLatLon::Store()
{
  CoordSystemPtr::Store();
  ObjectInfo::RemoveSection(fnObj, "Datum");
  ObjectInfo::RemoveSection(fnObj, "Ellipsoid");
  WriteElement("CoordSystem", "Datum", (char*)NULL);
  WriteElement("CoordSystem", "Datum Area", (char*)NULL);
  WriteElement("CoordSystem", "Ellipsoid", (char*)NULL);
  WriteElement("CoordSystem", "Sphere Radius", (char*)NULL);
  if (datum) 
		datum->Store(fnObj);
	else
		if (ell.sName.length() > 0) 
	  {
	    WriteElement("CoordSystem", "Ellipsoid", ell.sName);
	    if (fCIStrEqual("User Defined" , ell.sName))
	    {
	      WriteElement("Ellipsoid", "a", ell.a);
	      WriteElement("Ellipsoid", "1/f", 1/ell.f);
	    }
	    else if (ell.fSpherical())
	      WriteElement("CoordSystem", "Sphere Radius", ell.a);
	  }
}

bool CoordSystemViaLatLon::fIsLatLon() const
{
  return (fCIStrEqual(fnObj.sFile , "latlon"));
}

Coord CoordSystemViaLatLon::cConv(const CoordSystem& cs, const Coord& crd) const
{
  if (cs->fEqual(*this))
    return crd;
  if (fLatLon2Coord() && cs->fCoord2LatLon()) {
    LatLon ll = cs->llConv(crd);
    if (ll.fUndef())
      return Coord();
    CoordSystemViaLatLon* csvll = dynamic_cast<CoordSystemViaLatLon*>(cs.ptr());
    if (csvll) {
      if (datum && csvll->datum) {              // datums given, datum shift possible
        if (!datum->fEqual(csvll->datum)) {     // datum shift needed
          ll = csvll->datum->llToWGS84(ll);
          ll = datum->llFromWGS84(ll);
        }
        else if (ell != csvll->ell)           // diff ellipsoids but equal datums (not realistic)
          ll = ell.llhConv(csvll->ell,ll);         // this was the case in Ilwis1.4,
                                              // in Ilwis 2.1 not offered to the user
      }
      else                                  // no complete datum info, or one of them is syst latlon.csy
        if (!csvll->fIsLatLon() && !fIsLatLon()   // none of the 2 csy's is latlon.csy
             && (ell != csvll->ell) )             // and ellipsoids different
          ll = ell.llhConv(csvll->ell,ll);
                                          // else one of them is latlon, or both have the same ellipsoid
                                          // both latlon (with diff ellipsoid (and datums)) is not yet in Ilwis
    }
    if (ll.fUndef())
      return Coord();
    return cConv(ll);
  }
  else {
    return cs->cInverseConv(this, crd);
  }
  return Coord();
}

Coord CoordSystemViaLatLon::cConv(const LatLon&) const
{
  return Coord();
}



