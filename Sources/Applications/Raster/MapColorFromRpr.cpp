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

#include "Applications\Raster\MapColorFromRpr.H"

IlwisObjectPtr * createMapColorFromRpr(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)MapColorFromRpr::create(fn, (MapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new MapColorFromRpr(fn, (MapPtr &)ptr);
}

const char* MapColorFromRpr::sSyntax() {
  return "MapColorFromRpr(map[,rpr])";
}

MapColorFromRpr* MapColorFromRpr::create(const FileName& fn, MapPtr& p, const String& sExpr)
{
  Array<String> as;
  int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  if ((iParms < 1) || (iParms > 2))
    ExpressionError(sExpr, sSyntax());
  Map mp(as[0], fn.sPath());
  if (iParms > 1) {
	Representation rpr(as[1]);
	return new MapColorFromRpr(fn, p, mp, rpr);
  } else {
	return new MapColorFromRpr(fn, p, mp, mp->dm()->rpr());
  }
}

MapColorFromRpr::MapColorFromRpr(const FileName& fn, MapPtr& p)
: MapFromMap(fn, p)
{
  ReadElement("MapColorFromRpr", "Representation", rpr);
  fNeedFreeze = true;
  sFreezeTitle = "MapColorFromRpr";
}

MapColorFromRpr::MapColorFromRpr(const FileName& fn, MapPtr& p, const Map& mp, const Representation & _rpr)
: MapFromMap(fn, p, mp)
, rpr(_rpr)
{
  Domain dom("Color");
  SetDomainValueRangeStruct(dom);
  fNeedFreeze = true;
  if (!fnObj.fValid())
    objtime = objdep.tmNewest();
  sFreezeTitle = "MapColorFromRpr";
  ptr.Store(); // store domain and att table info
  Store();
}

void MapColorFromRpr::Store()
{
  MapFromMap::Store();
  WriteElement("MapFromMap", "Type", "MapColorFromRpr");
  WriteElement("MapColorFromRpr", "Representation", rpr);
}

MapColorFromRpr::~MapColorFromRpr()
{
}

bool MapColorFromRpr::fFreezing()
{
	if (!rpr.fValid())
		throw ErrorObject(TR("Map has no Representation"));
	long iXSize = mp->rcSize().Col;
	if (!mp->dvrs().fRealValues()) {
		LongBuf bufIn(iXSize);
		LongBuf bufColor(iXSize);
		if (mp->dvrs().fValues()) {
			RangeReal rr = mp->rrMinMax();
			for (long l=0; l<iLines(); l++) {
				mp->GetLineRaw(l, bufIn);
				for (long i = 0; i < iXSize; ++i) {
					double v = mp->dvrs().rValue(bufIn[i]);
					if (v == rUNDEF)
						bufColor[i] = iUNDEF;
					else {
						Color clr = rpr->clr(v, rr);
						bufColor[i] = clr.iVal();
					}
				}
				//drawColor->clrRaw(bufIn.buf(), bufColor.buf(), buf.iSize(), drm);
				pms->PutLineRaw(l, bufColor);
				if (!(l % 10))
					if (trq.fUpdate(l, iLines())) { return false; }
			}
		} else {
			for (long l=0; l<iLines(); l++) {
				mp->GetLineRaw(l, bufIn);
				for (long i = 0; i < iXSize; ++i) {
					long iRaw = bufIn[i];
					if (i == iUNDEF)
						bufColor[i] = iUNDEF;
					else {
						Color clr = rpr->clrRaw(iRaw);
						bufColor[i] = clr.iVal();
					}
				}
				//drawColor->clrRaw(bufIn.buf(), bufColor.buf(), buf.iSize(), drm);
				pms->PutLineRaw(l, bufColor);
				if (!(l % 10))
					if (trq.fUpdate(l, iLines())) { return false; }
			}
		}
	} else { // use real values
		RangeReal rr = mp->rrMinMax();
		RealBuf bufIn(iXSize);
		LongBuf bufColor(iXSize);
		for (long l=0; l<iLines(); l++) {
			mp->GetLineVal(l, bufIn);
			for (long i = 0; i < iXSize; ++i) {
				double v = bufIn[i];
				if (v == rUNDEF) {
					//Color clr = colorUNDEF; //rpr->clrRaw(iUNDEF);
					bufColor[i] = iUNDEF; // Color(255,255,255); // 0; // clr.iVal();
				} else {
					Color clr = rpr->clr(v, rr);
					bufColor[i] = clr.iVal();
				}
			}
			//drawColor->clrVal(bufIn.buf(), bufColor.buf(), buf.iSize());
			pms->PutLineRaw(l, bufColor);
			if (!(l % 10))
				if (trq.fUpdate(l, iLines())) { return false; }
		}
	}

	if (trq.fUpdate(iLines(), iLines())) return false;
	return true;
}

String MapColorFromRpr::sExpression() const
{
  if (rpr.fValid()) {
	String s("MapColorFromRpr(%S,%S)", mp->sNameQuoted(true, fnObj.sPath()), rpr->sName());
	return s;
  } else {
	String s("MapColorFromRpr(%S)", mp->sNameQuoted(true, fnObj.sPath()));
	return s;
  }
}

bool MapColorFromRpr::fDomainChangeable() const
{
  return false;
}

bool MapColorFromRpr::fValueRangeChangeable() const
{
  return false;
}




