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
// MapListVirtual.cpp: implementation of the MapListVirtual class.
//
//////////////////////////////////////////////////////////////////////

#include "Engine\Applications\MapListVirtual.h"
#include "Engine\Map\Raster\MapList\maplist.h"
#include "Applications\MapList\MapListMatrixMultiply.h"
#include "Applications\MapList\MapListApplic.h"
#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\Engine.h"
#include "Applications\MapList\MapListCalculate.h"

MapListVirtual::MapListVirtual(const FileName& fn, MapListPtr& p, bool fCreate)
: IlwisObjectVirtual(fn, p, p.objdep, fCreate)
, ptr(p)
{
  if (fCreate) {
    SetSize(0);
  }
  else {
    try {
      Domain dm;
      ValueRange vr;
      ptr.ReadElement("MapListVirtual", "Domain", dm);
      ptr.ReadElement("MapListVirtual", "ValueRange", vr);
      dvrsMaps.SetDomain(dm);
      dvrsMaps.SetValueRange(vr);
    }
    catch (const ErrorObject&)
    {
      // if not available do not complain
    }
  }
}

MapListVirtual::MapListVirtual(const FileName& fn, MapListPtr& _ptr, const GeoRef& gr, const RowCol& rc, long iNrBands, const String& sBandPrefix)
: IlwisObjectVirtual(fn, _ptr, _ptr.objdep, true),
  ptr(_ptr)
{
	ptr._gr = gr;
	ptr._rcSize = rc;
	ptr._sBandPreFix = sBandPrefix;
	ptr.iOffset = 0;

}

MapListVirtual _export * MapListVirtual::create(const FileName& fn, MapListPtr& p)
{
   String sType;
  if (0 == ObjectInfo::ReadElement("MapListVirtual", "Type", fn, sType))
    return 0;
  vector<ApplicationInfo *> infos;
  Engine::modules.getAppInfo(sType, infos);
  vector<void *> extraParms = vector<void *>();
  if ( infos.size()>0 ) {
	return (MapListVirtual *)(infos[0]->createFunction)(fn, p, "", extraParms);
  }
  throw ErrorInvalidType(fn, "MapListVirtual", sType);

  return NULL;
}

MapListVirtual _export *MapListVirtual::create(const FileName& fn, MapListPtr& p, const String& sExpression)
{
	String sFunc = IlwisObjectPtr::sParseFunc(sExpression);
	vector<ApplicationInfo *> infos;
    Engine::modules.getAppInfo(sFunc, infos);
	vector<void *> extraParms = vector<void *>();
	if ( infos.size() > 0) {
		return (MapListVirtual *)(infos[0]->createFunction)(fn, p, sExpression, extraParms);
	}

  throw ErrorAppName(fn.sFullName(), sExpression);
  return 0;
}

MapListVirtual::~MapListVirtual()
{
}

void MapListVirtual::Store()
{
  if (fnObj.sFile.length() == 0)  // empty file name
    return;
  IlwisObjectVirtual::Store();
  ptr.WriteElement("MapList", "Type", "MapListVirtual");
  ptr.WriteElement("MapListVirtual", "Domain", dvrsMaps.dm());
  ptr.WriteElement("MapListVirtual", "ValueRange", dvrsMaps.vr());
}

void MapListVirtual::Freeze()
{
  if (!objdep.fMakeAllUsable())
    return;
	trq.Start();
	String sTitle("%S - %S", sFreezeTitle, sName(true));
	trq.SetTitle(sTitle);
	trq.setHelpItem(htpFreeze);
  SetSize(0);
  fFreezing();
  int iMaps = ptr.iSize();
  for (int i = 0; i < iMaps; ++i)
  {
    Map mp = map(i);
    if (mp.fValid())
      ObjectInfo::WriteAdditionOfFileToCollection(mp->fnObj, ptr.fnObj);
  }      
	trq.Stop();
}

void MapListVirtual::UnFreeze()
{
  int iMaps = ptr.iSize();
  for (int i = 0; i < iMaps; ++i)
  {
    Map mp = map(i);
    if (mp.fValid())
      mp->fErase = true;
  }
  SetSize(0);
  ptr.Store();
}

bool MapListVirtual::fFreezing()
{
  return false;
}

String MapListVirtual::sExpression() const
{
  return sUNDEF;
}

String MapListVirtual::sType() const
{
  return "Dependent Map List";
}

void MapListVirtual::SetDomainValueRangeStruct(const DomainValueRangeStruct& dvrs)
{
  dvrsMaps = dvrs;
}
