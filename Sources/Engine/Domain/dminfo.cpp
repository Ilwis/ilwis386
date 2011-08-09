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
/* DomainInfo
   Copyright Ilwis System Development ITC
   feb 1997, by Jelle Wind
	Last change:  JEL   9 May 97    8:19 pm
*/

#include "Engine\Domain\dminfo.h"
#include "Engine\Domain\dmsort.h"
#include "Engine\Domain\dmgroup.h"
#include "Engine\Domain\dmpict.h"
#include "Engine\Domain\Dmvalue.h"
#include "Engine\Base\File\objinfo.h"
#include "Engine\Base\DataObjects\valrange.h"

//enum DomainType { dmtCLASS, dmtID, dmtVALUE, dmtIMAGE, dmtPICTURE,
//                  dmtBIT, dmtNONE, dmtSTRING, dmtGROUP, dmtCOLOR, dmtCOORD };


DomainInfo::DomainInfo() :
  _iNr(0)
{
}

DomainInfo::DomainInfo(const Domain& dm) :
  _iNr(0)
{
  if (!dm.fValid())
    return;
  _fnDom = dm->fnObj;
  _dmt = dm->dmt();
  _st = dm->stNeeded();
  switch (_dmt) {
    case dmtCLASS: case dmtID:
      { DomainSort* pdsrt = dm->pdsrt();
        if (0 != pdsrt)
          _iNr = pdsrt->iSize();
      }
      break;
    case dmtGROUP : 
      { DomainGroup* pdgrp = dm->pdgrp();
        if (0 != pdgrp)
          _iNr = pdgrp->iSize();
      }
    case dmtPICTURE : 
      { DomainPicture* pdp = dm->pdp();
        if (0 != pdp)
          _iNr = pdp->iColors();
      }
    case dmtVALUE : 
      { 
        _vr = ValueRange(dm);
      }
    default :
      break;
  }
}

DomainInfo::DomainInfo(const DomainInfo& dminf) 
{
  _fnDom = dminf.fnDom();
  _dmt = dminf.dmt();
  _st = dminf.st();
  _iNr = dminf.iNr();
  _vr = dminf.vr();
}

void DomainInfo::operator = (const DomainInfo& dminf)
{
  _fnDom = dminf.fnDom();
  _dmt = dminf.dmt();
  _st = dminf.st();
  _iNr = dminf.iNr();
  _vr = dminf.vr();
}

DomainInfo::DomainInfo(const FileName& fn, const char *sSection)
{
  Load(fn, sSection);
}

FileName DomainInfo::fnDom() const
{ 
  return _fnDom;
}

StoreType DomainInfo::st() const
{
  return _st;
}

DomainType DomainInfo::dmt() const
{
  return _dmt;
}

long DomainInfo::iNr() const
{
  return _iNr;
}

ValueRange DomainInfo::vr() const
{
  return _vr;
}

bool DomainInfo::fValues() const
{
  return (dmt() == dmtIMAGE) || (dmt() == dmtBIT) || (dmt() == dmtBOOL) || (dmt() == dmtVALUE);
}

bool DomainInfo::fRealValues() const
{
  if (!vr().fValid())
    return false;
  return vr()->fRealValues();
}

void DomainInfo::SetValueRange(const ValueRange& vr)
{
	_vr = vr;
}

bool DomainInfo::fRawAvailable() const
{
  return (st() != stREAL) && (st() != stSTRING) && (st() != stCRD) && (st() != stBINARY);
}

void DomainInfo::Store(const FileName& fn, const char* sSect, const char* sEntry) const
{
  String s = _fnDom.sRelative(true, fn.sPath());
  s &= ';';
  s &= Domain::sStoreType(_st);
  s &= ';';
  s &= Domain::sDomainType(_dmt);
  s &= ';';
  s &= String("%li", _iNr);
  s &= ';';
  if (_vr.fValid())
    s &= _vr->sRange(true);
  s &= ';';
  String sEnt = "DomainInfo";
  if (0 != sEntry)
    sEnt = sEntry;
  ObjectInfo::WriteElement(sSect, sEnt.c_str(), fn, s);
}


void DomainInfo::Load(const FileName& fn, const char* sSect, const char* sEntry)
{
  _vr = ValueRange();
  _iNr = 0;
  String s;
  String sEnt = "DomainInfo";
  if (0 != sEntry)
    sEnt = sEntry;
  if (0 == ObjectInfo::ReadElement(sSect, sEnt.c_str(), fn, s))
    return;
  char *p = strtok(s.sVal(), ";");
  if (p == 0)
    return;
  _fnDom = FileName(p, fn);
	if ("" == _fnDom.sExt)
		_fnDom.sExt = ".dom";
//  _fnDom = FileName(p, ".dom", false);
  p = strtok(0, ";");
  if (p == 0)
    return;
  String dmName(p);
  _st = Domain::st(dmName.toLower());
  if ( _st == stUNKNOWN) {
	  _st = Domain::st(_fnDom.sFile.toLower());
  }
  p = strtok(0, ";");
  if (p == 0)
	{
    return;
	}		
	if (stCRD == _st) {
		//_fnDom = FileName(); // "Coord.dom" does noet exist!
		_dmt = dmtCOORD;
	}
	else
		_dmt = Domain::dmt(String(p));
  p = strtok(0, ";");
  if (p == 0)
    return;
  _iNr =String(p).iVal();
  p = strtok(0, ";");
  if (p == 0)
    return;
  if ( _dmt == dmtVALUE) {
	  String range;
	ObjectInfo::ReadElement(sSect, "Range", fn, range);
	double min=0, max=0, step=1, offset=0;
	Array<String> parts;
	Split(range,parts,":");
	if ( parts.size() < 4) {
		min = parts[0].rVal();
		max = parts[1].rVal();
	}
	if ( parts.size() == 3) {
		offset = parts[2].sTail("=").rVal();
	}
	if ( parts.size() == 4) {
		step = parts[2].rVal();
		offset = parts[3].sTail("=").rVal();
	}
	_vr = ValueRange(min,max,step);
  }
  else {
	_vr = ValueRange(String(p));
  }
}

Domain DomainInfo::dmUnknown() const
{
  switch (_dmt) {
    case dmtCLASS: 
      return Domain(FileName()/*_fnDom*/, _iNr, dmtCLASS, "class");
    case dmtID:
      return Domain(FileName()/*_fnDom*/, _iNr, dmtID, "id");
    case dmtGROUP : 
      return Domain(FileName()/*_fnDom*/, _iNr, dmtCLASS, "group");
    case dmtPICTURE : 
      return Domain(FileName()/*_fnDom*/, _iNr, dmtPICTURE);
    case dmtIMAGE : 
      return Domain("image");
    case dmtBIT : 
      return Domain("bit");
    case dmtBOOL: 
      return Domain("bool");
    case dmtSTRING: 
      return Domain("string");
    case dmtNONE : 
      return Domain("none");
    case dmtCOLOR: 
      return Domain("color");
    case dmtCOORD: 
      return Domain("unknown.csy");
    case dmtVALUE : 
      if (_vr.fValid())
        return Domain(FileName()/*_fnDom*/,  _vr->rrMinMax().rLo(), _vr->rrMinMax().rHi(), _vr->rStep());
    default : /* nothing */;
  }
  return Domain("none");
}

Domain DomainInfo::dm() const
{
	FileName fnDomain = _fnDom;
	if ( !fnDomain.fValid() || !File::fExist(fnDomain))
		fnDomain = FileName();
  switch (_dmt) {
    case dmtCLASS: 
      return Domain(fnDomain);
    case dmtID:
      return Domain(fnDomain);
    case dmtGROUP : 
      return Domain(fnDomain);
    case dmtPICTURE : 
      return Domain(fnDomain);
    case dmtIMAGE : 
      return Domain("image");
    case dmtBIT : 
      return Domain("bit");
    case dmtBOOL: 
      return Domain("bool");
    case dmtSTRING: 
      return Domain("string");
    case dmtNONE : 
      return Domain("none");
    case dmtCOLOR: 
      return Domain("color");
    case dmtCOORD: 
      return Domain(); // not yet implemented
    case dmtVALUE : 
      if (_vr.fValid())
        return Domain(fnDomain,  _vr->rrMinMax().rLo(), _vr->rrMinMax().rHi(), _vr->rStep());
    default : /* nothing */;
  }
  return Domain("none");
}





