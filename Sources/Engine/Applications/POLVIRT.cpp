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
/* $Log: /ILWIS 3.0/PolygonMap/POLVIRT.cpp $
 * 
 * 8     2/05/03 11:30a Lichun
 * merge ID grid branch 
 * 
 * 9     9/05/02 9:05a Lichun
 * Addede PolygonMapGrid
 * 
 * 7     11-12-00 17:26 Koolhoven
 * added PolygonMapFromSegmentNonTopological
 * 
 * 6     29-11-00 13:39 Koolhoven
 * made PolygonMapVirtual::smp() virtual and call this function in
 * PolygonMapPtr::smp()
 * 
 * 5     22-11-99 12:23 Wind
 * same as prvious,but different implementation
 * 
 * 4     22-11-99 12:12 Wind
 * added object name to title of report window
 * 
 * 3     9/08/99 10:23a Wind
 * adpated to use of quoted file names
 * 
 * 2     3/10/99 4:08p Martin
 * Case insensitive support added
// Revision 1.5  1998/09/16 17:25:54  Wim
// 22beta2
//
// Revision 1.4  1997/09/02 15:24:19  Wim
// Extra safety check
//
// Revision 1.3  1997-08-25 19:50:13+02  Wim
// Added (empty) DeleteInternals() to BaseMapVirtual
// to call in the UnFreeze() routine.
//
// Revision 1.2  1997-07-30 12:42:09+02  Wim
// Removed PolygonMapSubMap because it does not work
//
/* PolygonMapVirtual
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  WK   29 Aug 97   12:54 pm
*/

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
  ApplicationInfo * info = Engine::modules.getAppInfo(sType);
  vector<void *> extraParms = vector<void *>();
  if ( info != NULL ) {
	return (PolygonMapVirtual *)(info->createFunction)(fn, p, "", extraParms);
  }
  throw ErrorInvalidType(fn, "PolygonMapVirtual", sType);

  return NULL;
}

PolygonMapVirtual* PolygonMapVirtual::create(const FileName& fn, PolygonMapPtr& p, const String& sExpression)
{
	String sFunc = IlwisObjectPtr::sParseFunc(sExpression);
	ApplicationInfo * info = Engine::modules.getAppInfo(sFunc);
	vector<void *> extraParms = vector<void *>();
	if ( info != NULL ) {
		return (PolygonMapVirtual *)(info->createFunction)(fn, p, sExpression, extraParms);
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
		ApplicationInfo * info = Engine::modules.getAppInfo("PolygonMapAttribute");
		vector<void *> extraParms = vector<void *>();
		PolygonMap pmap(fnMap);
		extraParms.push_back( &pmap);
		extraParms.push_back( &sCol);
		if ( info != NULL ) {
			return (PolygonMapVirtual *)(info->createFunction)(fn, p, "", extraParms);
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
  trq.SetHelpTopic(htpFreeze);
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
    _unlink(fnHis.sFullName().scVal());
    fnHis.sExt = ".ha#";
    _unlink(fnHis.sFullName().scVal());
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





