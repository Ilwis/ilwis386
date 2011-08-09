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


#include "Engine\Map\Raster\MAPSTORE.H"
#include "Engine\Applications\MAPVIRT.H"
#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\Engine.h"
#include "Engine\Domain\Dmvalue.h"
#include "Engine\Base\DataObjects\WPSMetaData.h"
#include "Engine\Applications\MAPFMAP.H"
#include "Engine\Applications\MAPFMPL.H"
#include "Engine\Applications\MAPFSEG.H"
#include "Engine\Applications\MAPFPOL.H"
#include "Engine\Applications\MAPFPNT.H"
#include "Engine\Base\DataObjects\valrange.h"
#include "Headers\Hs\map.hs"

static void InvalidFileNameError(const FileName& fn)
{
  throw ErrorObject(WhatError(TR("Invalid file name"), errNAME), fn);
}  

MapVirtual* MapVirtual::create(const FileName& fn, MapPtr& p)
{
	String sType;
	if (0 == ObjectInfo::ReadElement("MapVirtual", "Type", fn, sType))
		return NULL;

	MapVirtual *mv = (MapVirtual *)getMapVirtual(fn, p, sType);
	if ( mv) return mv;

	String sSubType;
	if (0 == ObjectInfo::ReadElement(sType.c_str(), "Type", fn, sSubType))
		return NULL;

	mv = (MapVirtual *)getMapVirtual(fn, p, sSubType);
	if ( mv) return mv;

   return 0;
}

IlwisObjectPtr * MapVirtual::getMapVirtual(const FileName& fn, MapPtr& p, const String& sType) {
	vector<CommandInfo *> infos;
	Engine::modules.getCommandInfo(sType, infos);
	if ( infos.size() > 0 ) {
		vector<void *> extraParms = vector<void *>();
		return (infos[0]->createFunction)(fn, p, "", extraParms);
	}
	return NULL;
}

// splits expression in parts seperated by dots; is aware of quoted parts
static bool fSplitOnDots(const String& sExpr, Array<String>& as)
{
  String s;
  bool fInQuotes=false;
  char c;
  for (unsigned int i=0; i < sExpr.length(); i++) {    
    c = sExpr[i];
    if (c == '.') {
      if (!fInQuotes) {
        as &= s;
        s = String();
        continue;
      }
    }
    else if (c == '\'') {
      if (!fInQuotes)
        fInQuotes = true;
      else if (sExpr[i+1] == '\'')
        i++;
      else
        fInQuotes = false;
    }
    else if (!fInQuotes) {
      if (0 != strchr(String::pcCharsNotAllowedOutsideQuotes(), c))
        return false;
    }
    s += c;
  }
  if (s.length() > 0)
    as &= s;
  return as.iSize() > 1;
}

MapVirtual* MapVirtual::create(const FileName& fn, MapPtr& p, const String& sExpression)
{
	String sFunc = IlwisObjectPtr::sParseFunc(sExpression);
	vector<CommandInfo *> infos;
	Engine::modules.getCommandInfo(sFunc, infos);
	vector<void *> extraParms = vector<void *>();
	if ( infos.size() > 0 ) {
		return (MapVirtual *)(infos[0]->createFunction)(fn, p, sExpression, extraParms);
	}
	MapVirtual *mpv = NULL;
	if ( (mpv = dottedExpressionMaps(fn,p, sExpression, extraParms))) {
		return mpv;
	}
	infos.clear();
	Engine::modules.getCommandInfo("MapCalculate", infos);
	if ( infos.size() > 0 ) {
		return (MapVirtual *)(infos[0]->createFunction)(fn, p, sExpression, extraParms);
	}
 
 
return 0;
}

MapVirtual *MapVirtual::dottedExpressionMaps(const FileName& fn, MapPtr& p, const String& sExpression, vector<void *> extraParms ) {
	Array<String> asParts;
	if (fSplitOnDots(sExpression, asParts)) {
		int iNext;
		String sMap = asParts[0];
		String sMapExt;
		if (asParts[1] == "mpr") {
			sMapExt = "mpr";
			iNext = 2;
		}
		else 
			iNext = 1;
		if (iNext < asParts.iSize()) {
			FileName fnMap(sMap, ".mpr", true);
			if (0 == strchr(sMap.c_str(), ':')) // no path set
				fnMap.Dir(fn.sPath());       
			if (!File::fExist(fnMap)) {
				fnMap = FileName(sMap, ".mpr", true);
				if (!File::fExist(fnMap))
					NotFoundError(fn);
			}
			if (Map::iColorType(asParts[iNext]) != shUNDEF) {
				try {
					Map mp(fnMap);
					if (mp->dm()->pdp() || mp->dm()->pdcol()) {
						vector<CommandInfo *> infos;
						Engine::modules.getCommandInfo("MapColorSep", infos);
						if ( infos.size() > 0) {
							extraParms.push_back((void *)&asParts[iNext]);
							extraParms.push_back((void *)&Map(fnMap));
							return (MapVirtual *)(infos[0]->createFunction)(fn, p, "", extraParms);
							}
						//return new MapColorSep(FileName(), p, mp, asParts[iNext]);
					}
				}
				catch (const ErrorObject&) {
				}
			}
			bool fCol = false;
			String sCol = asParts[iNext];
			for (int i=iNext+1; i < asParts.iSize(); i++) {
				sCol &= '.';
				sCol &= asParts[i];
				}
			vector<CommandInfo *> infos;
			Engine::modules.getCommandInfo("MapAttribute", infos);
			if ( infos.size() > 0) {
				Map mp1(fnMap);
				extraParms.push_back((void *)&sCol);
				extraParms.push_back((void *)&mp1);
				return (MapVirtual *)(infos[0]->createFunction)(fn, p, "", extraParms);
				//return new MapAttribute(fn, p, Map(fnMap), sCol);
			}
		}
	}
	return NULL;
}

MapVirtual::MapVirtual(const FileName& fn, MapPtr& _ptr)
: BaseMapVirtual(fn, _ptr),
  ptr(_ptr), pms(_ptr.pms), _rcSize(_ptr._rcSize)
{
}

MapVirtual::MapVirtual(const FileName& fn, MapPtr& _ptr, const GeoRef& gr, const RowCol& rcSize,
                       const DomainValueRangeStruct& dvs, const MapFormat mf)
: BaseMapVirtual(fn, _ptr, gr->cs(), gr->cb(), dvs),
  ptr(_ptr), pms(_ptr.pms), _rcSize(_ptr._rcSize)
{
  _ptr._gr = gr;
  if (!gr->fGeoRefNone())
    _rcSize = gr->rcSize();
  else
    _rcSize = rcSize;
}

void MapVirtual::Store()
{
  IlwisObjectVirtual::Store();
  WriteElement("Map", "Type", "MapVirtual");
  // for downward compatibility to 2.02:
  WriteElement("MapVirtual", "Expression", sExpression());
}

MapVirtual::~MapVirtual()
{
}

String MapVirtual::sExpression() const 
{ 
  return sUNDEF; 
}

String MapVirtual::sType() const
{
  return "Dependent Raster Map";
}


void MapVirtual::ReadDomainValueRangeFromFileAndSet()
{
  Domain dom;
  ValueRange vr;
  ReadElement("BaseMap", "Domain", dom);
  if (0 != dom->pdv())
    ptr.ReadElement("BaseMap", "Range", vr);
  ptr.dvs = DomainValueRangeStruct(dom, vr);
}

void MapVirtual::ReadGeoRefFromFileAndSet()
{
  GeoRef grf;
  ptr.ReadElement("Map", "GeoRef", grf);
	if (!grf.fValid())
		throw ErrorDummy();
  ptr._gr = grf;
  ptr._csys = grf->cs();
  ptr.ReadElement("Map", "Size", ptr._rcSize);
}

#define compute(LineRawVal) \
 for (long i = 0; i < iLines(); ++i) {\
   if (trq.fUpdate(i, iLines()) ) return false; \
   Compute##LineRawVal(i, buf, 0, buf.iSize());\
   ptr->Put##LineRawVal(i, buf, 0, buf.iSize());\
 } \
 if (trq.fUpdate(iLines(), iLines()) ) return false; \


void MapVirtual::Freeze()
{
	if (!fnObj.fValid())
		return;
	
	if (!objdep.fMakeAllUsable())
		return;
	
	UnFreeze();
	InitFreeze();
	CreateMapStore();
	assert(pms != 0);
	pms->KeepOpen(true);
	trq.Start();
	String sTitle("%S - %S", sFreezeTitle, sName(true));
	trq.SetTitle(sTitle);
	trq.setHelpItem(htpFreeze);
	bool fFrozen;
	bool fErrorShown = false;
	try
	{
		fFrozen = fFreezing();
	}
	catch (const ErrorObject& err)
	{
		trq.Stop();
		err.Show();
		fFrozen = false;
		fErrorShown = true;
	}
	catch ( CException *err)
	{
		MessageBeep(MB_ICONHAND);
		err->ReportError(MB_OK|MB_ICONHAND|MB_TOPMOST);
		err->Delete();
		fFrozen = false;
		fErrorShown = true;
	}			
	ptr.KeepOpen(false);
	trq.Stop();
	if (fFrozen)
	{
		ptr.Updated();
		pms->Updated();
		// delete histogram
		FileName fnHis(fnObj, ".his", true);
		if (File::fExist(fnHis))
			_unlink(fnHis.sFullName().c_str());
		fnHis.sExt = ".hi#";
		if (File::fExist(fnHis))
			_unlink(fnHis.sFullName().c_str());
		// reset minmax
		ptr.SetMinMax(RangeInt());
		ptr.SetMinMax(RangeReal());
		ptr.SetPerc1(RangeInt());
		ptr.SetPerc1(RangeReal());
		pms->Flush();
	}  
	else
	{
		pms->SetErase(true);
		try  // class File can throw an error
		{
			delete pms;
		}
		catch (const ErrorObject& err)
		{
			if (!fErrorShown)
				err.Show();
			fErrorShown = true;
		}
		catch (CException *err)
		{	
			if (!fErrorShown)
			{			 
				MessageBeep(MB_ICONHAND);
				err->ReportError(MB_OK|MB_ICONHAND|MB_TOPMOST);
			}
			err->Delete();
			fErrorShown = true;
		}				
		pms = 0;
	}
	ptr.Store();
}

void MapVirtual::UnFreeze()
{
  if (0 == pms) 
    return;
  pms->SetErase(true);
  delete pms;
  pms = 0;
  DeleteInternals();
//fFrozen = false;
  ptr.Store();
/*  ptr.SetMinMax(RangeInt());
  ptr.SetPerc1(RangeInt());
  ptr.SetMinMax(RangeReal());
  ptr.SetPerc1(RangeReal());*/
}

bool MapVirtual::fFreezing()
{
  trq.SetText(String(TR("Calculating '%S'").c_str(), sName(true, fnObj.sPath())));
  switch (st()) {
    case stBIT:
    case stDUET:
    case stNIBBLE:
    case stBYTE:
      { ByteBuf buf(iCols());
        for (long i = 0; i < iLines(); ++i) {
          if (trq.fUpdate(i, iLines()) ) 
            return false; 
          ComputeLineRaw(i, buf, 0, buf.iSize());
          pms->PutLineRaw(i, buf, 0, buf.iSize());
        } 
        if (trq.fUpdate(iLines(), iLines()) ) 
          return false; 
      } 
      break;  
    case stINT: 
      { IntBuf buf(iCols());
        for (long i = 0; i < iLines(); ++i) {
          if (trq.fUpdate(i, iLines()) ) 
            return false; 
          ComputeLineRaw(i, buf, 0, buf.iSize());
          pms->PutLineRaw(i, buf, 0, buf.iSize());
        } 
        if (trq.fUpdate(iLines(), iLines()) ) 
          return false; 
      } 
      break;  
    case stLONG: 
      { LongBuf buf(iCols());
        for (long i = 0; i < iLines(); ++i) {
          if (trq.fUpdate(i, iLines()) ) 
            return false; 
          ComputeLineRaw(i, buf, 0, buf.iSize());
          pms->PutLineRaw(i, buf, 0, buf.iSize());
        } 
        if (trq.fUpdate(iLines(), iLines()) ) 
          return false; 
      } 
      break;  
    case stREAL:
      { RealBuf buf(iCols());
        for (long i = 0; i < iLines(); ++i) {
          if (trq.fUpdate(i, iLines()) ) 
            return false; 
          ComputeLineVal(i, buf, 0, buf.iSize());
          pms->PutLineVal(i, buf, 0, buf.iSize());
        } 
        if (trq.fUpdate(iLines(), iLines()) ) 
          return false; 
      } 
      break;
  }
  return true;
}

bool MapVirtual::fDomainChangeable() const
{
  return false;
}

bool MapVirtual::fValueRangeChangeable() const
{
  return false;
}

bool MapVirtual::fGeoRefChangeable() const
{
  return false;
}

long MapVirtual::iComputePixelRaw(RowCol) const
{
  return iUNDEF;
}

double MapVirtual::rComputePixelVal(RowCol rc) const
{
  if (!fValues())
    return rUNDEF;
  if (fRawIsValue())
    return doubleConv(iComputePixelRaw(rc));
  else if (!fUseReals()) {
    long iRaw = iComputePixelRaw(rc);
    return dvrs().rValue(iRaw);
  }
  return rUNDEF;
}

long MapVirtual::iComputePixelVal(RowCol rc) const
{
  if (!fValues())
    return iUNDEF;
  if (fRawIsValue())
    return iComputePixelRaw(rc);
  else if (!fUseReals()) {
    long iRaw = iComputePixelRaw(rc);
    return vr()->iValue(iRaw);
  }
  else
    return longConv(rComputePixelVal(rc));
}

void MapVirtual::ComputeLineRaw(long line, ByteBuf& buf, long iFrom, long iNr) const
{
  LongBuf b(iNr);
  ComputeLineRaw(line, b, iFrom, iNr);
  for (long i = 0; i < iNr; ++i)
    buf[i] = byteConv(b[i]);
}

void MapVirtual::ComputeLineRaw(long line, IntBuf& buf, long iFrom, long iNr) const
{
  LongBuf b(iNr);
  ComputeLineRaw(line, b, iFrom, iNr);
  for (long i = 0; i < iNr; ++i)
    buf[i] = shortConv(b[i]);
}

void MapVirtual::ComputeLineRaw(long line, LongBuf& buf, long iFrom, long iNr) const
{
  RowCol rc(line, iFrom);
  for (long i = 0; i < iNr; ++i) {
    buf[i] = iComputePixelRaw(rc);
    rc.Col++;
  }
}

void MapVirtual::ComputeLineVal(long line, LongBuf& buf, long iFrom, long iNr) const
{
  RowCol rc(line, iFrom);
  for (long i = 0; i < iNr; ++i) {
    buf[i] = iComputePixelVal(rc);
    rc.Col++;
  }
}

void MapVirtual::ComputeLineVal(long line, RealBuf& buf, long iFrom, long iNr) const
{
  RowCol rc(line, iFrom);
  for (long i = 0; i < iNr; ++i) {
    buf[i] = rComputePixelVal(rc);
    rc.Col++;
  }
}

bool MapVirtual::fMapInObjDep(const Map& mp) const
{
  return objdep.fUses(mp.ptr());
}

bool MapVirtual::fMapInObjDep(const FileName& fn) const
{
  if ((fn.sExt != ".mpr") && (fn.sExt.length() == 0))
    return false;
  try {
    return objdep.fUses(fn);
  }
  catch (const ErrorObject&) {
    return false;
  }
}

void MapVirtual::CreateMapStore()
{
  if (0 != pms) {
    delete pms;
    pms = 0;
  } 
	RowCol rc = rcSize();
	if ( rc.Row == 0 || rc.Col == 0 )
		throw ErrorObject(TR("Map size is illegal (0,0)"));
  pms = new MapStore(fnObj, ptr, gr(), rcSize(), dvrs(), mf());
//  ptr.SetDataReadOnly(); // previous statement sets it on false
}

DomainValueRangeStruct MapVirtual::dvrsDefault(const String& /*sExpression*/) const
{
  return DomainValueRangeStruct();
}

bool MapVirtual::fMergeDomainForCalc(Domain& /*dm*/, const String& /*sExpr*/)
{
  return true;
}




