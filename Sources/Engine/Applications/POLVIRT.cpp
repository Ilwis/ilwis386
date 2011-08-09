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


#include "Engine\Applications\POLVIRT.H"
#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\Engine.h"
#include "Engine\Base\DataObjects\valrange.h"

PolygonMapVirtual* PolygonMapVirtual::create(const FileName& fn, PolygonMapPtr& p)
{
   String sType;
  if (0 == ObjectInfo::ReadElement("PolygonMapVirtual", "Type", fn, sType))
    return 0;
  vector<CommandInfo *> infos;
  Engine::modules.getCommandInfo(sType, infos);
  vector<void *> extraParms = vector<void *>();
  if ( infos.size() > 0 ) {
	return (PolygonMapVirtual *)(infos[0]->createFunction)(fn, p, "", extraParms);
  }
  throw ErrorInvalidType(fn, "PolygonMapVirtual", sType);

  return NULL;
}

PolygonMapVirtual* PolygonMapVirtual::create(const FileName& fn, PolygonMapPtr& p, const String& sExpression)
{
	String sFunc = IlwisObjectPtr::sParseFunc(sExpression);
	vector<CommandInfo *> infos;
    Engine::modules.getCommandInfo(sFunc, infos);
	vector<void *> extraParms = vector<void *>();
	if ( infos.size() > 0 ) {
		return (PolygonMapVirtual *)(infos[0]->createFunction)(fn, p, sExpression, extraParms);
	}


	FileName fnMap(sExpression);

	String sCol = fnMap.sCol;  // Get column from filename
	if (sCol.length() > 0)
	{
		fnMap.sCol = String();    // Reset the column

		if (sExpression.find(":") == string::npos)  // no path information in sExpression
			fnMap.Dir(fn.sPath());      // so try in same dir as fn
		if (!File::fExist(fnMap))       // if file not found
		{
			fnMap = FileName(sExpression); // then try current active directory
			if (!File::fExist(fnMap))
				throw ErrorNotFound(fnMap);
		}
		vector<CommandInfo *> infos;
		Engine::modules.getCommandInfo("PolygonMapAttribute", infos);
		vector<void *> extraParms = vector<void *>();
		PolygonMap pmap(fnMap);
		extraParms.push_back( &pmap);
		extraParms.push_back( &sCol);
		if ( infos.size() > 0) {
			return (PolygonMapVirtual *)(infos[0]->createFunction)(fn, p, "", extraParms);
		}
	}
	throw ErrorAppName(fn, sFunc);
	
	return NULL;
}

PolygonMapVirtual::PolygonMapVirtual(const FileName& fn, PolygonMapPtr& _ptr)
: BaseMapVirtual(fn, _ptr), ptr(_ptr), pms(_ptr.pms)
{
}

PolygonMapVirtual::PolygonMapVirtual(const FileName& fn, PolygonMapPtr& _ptr, const CoordSystem& cs,
                                     const CoordBounds& cb, const DomainValueRangeStruct& dvs)
: BaseMapVirtual(fn,_ptr,cs,cb,dvs), ptr(_ptr), pms(_ptr.pms)
{
}

PolygonMapVirtual::~PolygonMapVirtual()
{
}

void PolygonMapVirtual::Store()
{
  IlwisObjectVirtual::Store();
  WriteElement("PolygonMap", "Type", "PolygonMapVirtual");
  // for downward compatibility to 2.02:
  WriteElement("PolygonMapVirtual", "Expression", sExpression());
}


String PolygonMapVirtual::sExpression() const
{
  return sUNDEF;
}

String PolygonMapVirtual::sType() const
{
  return "Dependent Polygon Map";
}


bool PolygonMapVirtual::fDomainChangeable() const
{
  return fValues();
}

bool PolygonMapVirtual::fValueRangeChangeable() const
{
  return fValues();
}

void PolygonMapVirtual::Freeze()
{
  if (!objdep.fMakeAllUsable())
    return;
  UnFreeze();
  InitFreeze();
  CreatePolygonMapStore();
  Store();
  ptr.KeepOpen(true);
  trq.Start();
  String sTitle("%S - %S", sFreezeTitle, sName(true));
  trq.SetTitle(sTitle); 
  trq.setHelpItem(htpFreeze);
  bool fFrozen;
  try {
    fFrozen = fFreezing();
  }  
  catch (const ErrorObject& err) {
    err.Show();
    fFrozen = false;
  }  
  trq.Stop();
  ptr.KeepOpen(false);
  if (fFrozen) {
    ptr.Updated();
    pms->Updated();
    // delete histogram
    FileName fnHis(fnObj, ".hsa", true);
    _unlink(fnHis.sFullName().c_str());
    fnHis.sExt = ".ha#";
    _unlink(fnHis.sFullName().c_str());
    // reset minmax
    ptr.SetMinMax(RangeInt());
    ptr.SetMinMax(RangeReal());
    ptr.SetPerc1(RangeInt());
    ptr.SetPerc1(RangeReal());
  }  
  else if (0 != pms) {
    pms->SetErase(true);
    delete pms;
    pms = 0;
  }
  ptr.Store();
}

void PolygonMapVirtual::UnFreeze()
{
  if (0 == pms) 
    return;
  pms->SetErase(true);
  delete pms;
  pms = 0;
  DeleteInternals();
//  fFrozen = fase;
  ptr._iPol = ptr._iPolDeleted = 0;
  ptr.Store();
}

bool PolygonMapVirtual::fFreezing()
{
  return false;
}

void PolygonMapVirtual::CreatePolygonMapStore()
{
  if (0 != pms) {
    delete pms;
    pms = 0;
  }  
  pms = new PolygonMapStore(fnObj, ptr, true);
//ptr.SetDataReadOnly(); // previous statement sets it on false
}





