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
/* CoordSystemProjection
   Copyright Ilwis System Development ITC
   april 1998, by Wim Koolhoven
	Last change:  WK    8 Apr 98    6:25 pm
*/

#include "Engine\SpatialReference\Csproj.h"
#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\Engine.h"

CoordSystemProjection::CoordSystemProjection(const FileName& fn)
: CoordSystemViaLatLon(fn)
{
  String s;
  if (!ReadElement("CoordSystem", "Projection", s))
    return;
  FileName fnPrj(s);
  if (!File::fExist(fnPrj))
    fnPrj.Dir(getEngine()->getContext()->sStdDir());
  if (File::fExist(fnPrj))
    prj = Projection(fnPrj, ell);
  else
    prj = Projection(s, ell);
  if (prj.fValid()) {
    for (int i = 0; i < pvLAST; ++i) {
      ProjectionParamValue ppv = ProjectionParamValue(i);
      ProjectionParamType pp = prj->ppParam(ppv);
      if (pp == ppNONE)
        continue;
      try {
        String sParam = prj->sParamName(ppv);
        switch (pp) {
          case ppLAT:
          case ppLON:
          case ppREAL:
          case ppSCALE:
          case ppPOSREAL:
          case ppANGLE:
          case ppACUTANGLE:{
            double rValue;
            if (ReadElement("Projection", sParam.scVal(), rValue))
              prj->Param(ppv, rValue);
          } break;
				  case ppZONE:
          case ppINT: {
            long iValue;
            if (ReadElement("Projection", sParam.scVal(), iValue))
              prj->Param(ppv, iValue);
          } break;
          case ppBOOL: {
            bool fValue;
            if (ReadElement("Projection", sParam.scVal(), fValue))
              prj->Param(ppv, (long)fValue);
          } break;
          case ppLATLON: {
            LatLon ll;
            if (ReadElement("Projection", sParam.scVal(), ll))
              prj->Param(ppv, ll);
          } break;
          case ppCOORD: {
            Coord crd;
            if (ReadElement("Projection", sParam.scVal(), crd))
              prj->Param(ppv, crd);
          } break;
        }
      }
      catch (ErrorObject& err) {
        err.Show();
      }
    }
    try {
      prj->Prepare();
    }
    catch (ErrorObject& err) {
      err.Show();
    }
  }
}

CoordSystemProjection::CoordSystemProjection(const FileName& fn, int iOpt)
: CoordSystemViaLatLon(fn,iOpt)
{
}

CoordSystemProjection::~CoordSystemProjection()
{
}

void CoordSystemProjection::Store()
{
  CoordSystemViaLatLon::Store();
  WriteElement("CoordSystem", "Type", "Projection");
  WriteElement("CoordSystem", "Projection", prj);
  if (prj.fValid()) {
    for (int i = 0; i < pvLAST; ++i) {
      ProjectionParamValue ppv = ProjectionParamValue(i);
      ProjectionParamType pp = prj->ppParam(ppv);
      String sParam = prj->sParamName(ppv);
      try {
        switch (pp) {
          case ppNONE:
            WriteElement("Projection", sParam.scVal(), (char*)0);
            break;
          case ppLAT:
          case ppLON:
          case ppREAL:
          case ppPOSREAL: 
          case ppANGLE: 
          case ppACUTANGLE:{
            double rValue = prj->rParam(ppv);
            WriteElement("Projection", sParam.scVal(), rValue);
          } break;
          case ppSCALE: {
            double rValue = prj->rParam(ppv);
						String str("%.10f", rValue);
            WriteElement("Projection", sParam.scVal(), str);
          } break;
				  case ppZONE:
          case ppINT: {
            long iValue = prj->iParam(ppv);
            WriteElement("Projection", sParam.scVal(), iValue);
          } break;
          case ppBOOL: {
            bool fValue = prj->iParam(ppv)!=0;
            WriteElement("Projection", sParam.scVal(), fValue);
          } break;
          case ppLATLON: {
            LatLon llValue = prj->llParam(ppv);
            WriteElement("Projection", sParam.scVal(), llValue);
          } break;
          case ppCOORD: {
            Coord cValue = prj->cParam(ppv);
            WriteElement("Projection", sParam.scVal(), cValue);
          } break;
        }
      }
      catch (ErrorObject& err) {
          err.Show();
      }   
    }    
  }
}

String CoordSystemProjection::sType() const
{
  return "Coordinate System Projection";
}

bool CoordSystemProjection::fLatLon2Coord() const
{
  if (prj.fValid())
    return prj->fLatLon2Coord();
  else  
    return false;
}

bool CoordSystemProjection::fCoord2LatLon() const
{
  if (prj.fValid())
    return prj->fCoord2LatLon();
  else  
    return false;
}

LatLon CoordSystemProjection::llConv(const Coord& crd) const
{
  if (prj.fValid() && !crd.fUndef()) {
    XY xy;
    xy.x = (crd.x - prj->x0) / prj->ell.a * rUnitSize;
    xy.y = (crd.y - prj->y0) / prj->ell.a * rUnitSize;
    PhiLam pl = prj->plConv(xy);  
    if (pl.fUndef())
      return LatLon();
    if (abs(pl.Phi) > M_PI_2)
      return LatLon();
    pl.Lam += prj->lam0;  
    pl.AdjustLon();
    LatLon ll;
    ll.Phi(pl.Phi);
    ll.Lambda(pl.Lam);
    return ll;
  }
  else
    return LatLon();
}

Coord CoordSystemProjection::cConv(const LatLon& ll) const
{
  if (prj.fValid() && !ll.fUndef()) {
    PhiLam pl;
    pl.Phi = ll.Phi();
    pl.Lam = ll.Lambda();
    if (pl.Phi > M_PI_2)
      pl.Phi = M_PI_2;
    else if (pl.Phi < -M_PI_2)
      pl.Phi = -M_PI_2;
    pl.Lam -= prj->lam0;  
    //pl.AdjustLon();
    XY xy = prj->xyConv(pl);
    if (xy.fUndef())
      return Coord();
    Coord crd;
    crd.x = xy.x * prj->ell.a / rUnitSize + prj->x0;
    crd.y = xy.y * prj->ell.a / rUnitSize + prj->y0;
    return crd;
  }
  else
    return crdUNDEF;
}

bool CoordSystemProjection::fEqual(const IlwisObjectPtr& obj) const
{
	if (this == (const CoordSystemProjection*)(&obj))
		return true;
	
	const CoordSystemProjection* csPrj = dynamic_cast<const CoordSystemProjection*>(&obj);
	bool fYes = false;
	if (csPrj)
	{
		if (prj.fValid() && csPrj->prj.fValid())
		{
			fYes = prj->fEqual(*(csPrj->prj.pointer()));
			if (( datum ) && (csPrj->datum))
				fYes &= datum->fEqual(csPrj->datum);
			if((0 == datum ) && (0 == csPrj->datum))
				fYes &= true;
			fYes &= ell.fEqual(csPrj->ell);
		}
	}
	return fYes;
}




