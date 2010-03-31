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
/* $Log: /ILWIS 3.0/PointMap/Pntvirt.cpp $
 * 
 * 10    6/03/01 15:10 Willem
 * PointMapVirtual:: create() now properly checks on attribute map. It can
 * now also handle directories with dots
 * 
 * 9     5/03/01 10:35 Willem
 * Removed superfluous member _iPoints from PointMapPtr: iPnt() now gets
 * the number of points from the PointMapStore or PointMapVirtual
 * 
 * 8     10-08-00 15:15 Koolhoven
 * attribute maps of the form filename.mpp.column now work again
 * 
 * 7     16-06-00 12:06 Koolhoven
 * prevent extra storage of old table data file
 * 
 * 6     21-02-00 16:59 Wind
 * Freeze did set proximity to zero
 * 
 * 5     22-11-99 12:24 Wind
 * same as previous, but different implementation
 * 
 * 4     22-11-99 12:14 Wind
 * added object name to title of report window when calculating
 * 
 * 3     9/08/99 10:22a Wind
 * adpated to use of quoted file names
 * 
 * 2     3/10/99 4:17p Martin
 * Case insensitive support added
// Revision 1.4  1998/09/16 17:26:27  Wim
// 22beta2
//
/* PointMapVirtual
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  WK   20 Mar 98   12:46 pm
*/

#include "Engine\Applications\PNTVIRT.H"
#include "Engine\Map\Point\PNTSTORE.H"
#include "Engine\Table\Rec.h"
#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\Engine.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Headers\Hs\point.hs"

void PointMapVirtual::LoadPlugins() {
	
}

PointMapVirtual* PointMapVirtual::create(const FileName& fn, PointMapPtr& ptr)
{
  String sType;
  if (0 == ObjectInfo::ReadElement("PointMapVirtual", "Type", fn, sType))
    return 0;
  ApplicationInfo * info = Engine::modules.getAppInfo(sType);
  vector<void *> extraParms = vector<void *>();
  if ( info != NULL ) {
	return (PointMapVirtual *)(info->createFunction)(fn, ptr, "", extraParms);
  }
  throw ErrorInvalidType(fn, "PointMapVirtual", sType);

  return NULL;
}

PointMapVirtual* PointMapVirtual::create(const FileName& fn, PointMapPtr& ptr, const String& sExpression)
{
	String sFunc = IlwisObjectPtr::sParseFunc(sExpression);
	ApplicationInfo * info = Engine::modules.getAppInfo(sFunc);
	vector<void *> extraParms = vector<void *>();
	if ( info != NULL ) {
		return (PointMapVirtual *)(info->createFunction)(fn, ptr, sExpression, extraParms);
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
		ApplicationInfo * info = Engine::modules.getAppInfo("PointMapAttribute");
		vector<void *> extraParms = vector<void *>();
		PointMap pmap(fnMap);
		extraParms.push_back( &pmap);
		extraParms.push_back( &sCol);
		if ( info != NULL ) {
			return (PointMapVirtual *)(info->createFunction)(fn, ptr, "", extraParms);
		}
		//return new PointMapAttribute(fn, ptr, PointMap(fnMap), sCol);
	}
	throw ErrorAppName(fn, sFunc);
	
	return NULL;
}

PointMapVirtual::PointMapVirtual(const FileName& fn, PointMapPtr& _ptr)
: BaseMapVirtual(fn, _ptr),
  ptr(_ptr), pms(_ptr.pms)
{
	_iPoints = _ptr.iFeatures();
}

PointMapVirtual::PointMapVirtual(const FileName& fn, PointMapPtr& _ptr, const CoordSystem& cs,
                             const CoordBounds& cb, const DomainValueRangeStruct& dvs)
: BaseMapVirtual(fn, _ptr, cs, cb, dvs),
  ptr(_ptr), pms(_ptr.pms)
{
	_iPoints = _ptr.iFeatures();
}

PointMapVirtual::~PointMapVirtual()
{
}

void PointMapVirtual::Store()
{
  IlwisObjectVirtual::Store();
  WriteElement("PointMap", "Type", "PointMapVirtual");
  // for downward compatibility to 2.02:
  WriteElement("PointMapVirtual", "Expression", sExpression());
}

String PointMapVirtual::sExpression() const
{
  return sUNDEF;
}

String PointMapVirtual::sType() const
{
  return "Dependent Point Map";
}


bool PointMapVirtual::fDomainChangeable() const
{
  return fValues();
}

bool PointMapVirtual::fValueRangeChangeable() const
{
  return fValues();
}

void PointMapVirtual::Freeze()
{
//  if (fFrozen && fFrozenValid())
//    return;
  if (!objdep.fMakeAllUsable())
    return;
  UnFreeze();
  double rProxSav = ptr.rProximity();
  ptr.SetProximity(0);
  InitFreeze();
  CreatePointMapStore();
  Store();
  
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
  ptr.SetProximity(rProxSav);
  trq.Stop();
  if (fFrozen) {
    ptr.Updated();
    pms->Updated();
    // delete histogram
    FileName fnHis(fnObj, ".hsp", true);
    _unlink(fnHis.sFullName().scVal());
    fnHis.sExt = ".hp#";
    _unlink(fnHis.sFullName().scVal());
    // reset minmax
    ptr.SetMinMax(RangeInt());
    ptr.SetMinMax(RangeReal());
    ptr.SetPerc1(RangeInt());
    ptr.SetPerc1(RangeReal());
    pms->Flush();
  }  
  else {
    pms->SetErase(true);
    delete pms;
    pms = 0;
  }
  ptr.Store();
}

void PointMapVirtual::UnFreeze()
{
  if (0 == pms) 
    return;
  pms->SetErase(true);
  delete pms;
  pms = 0;
  DeleteInternals();
//  fFrozen = false;
  _iPoints = 0;
  ptr.Store();
}

bool PointMapVirtual::fFreezing()
{
  trq.SetText(String(SPNTTextCalculating_S.scVal(), sName(true, fnObj.sPath())));
  StoreType stp = dvrs().st();
  long iPoints = iPnt();
  for (long i=0; i < iPoints; i++) {
    if (trq.fUpdate(i, iPoints))
      return false;
    pms->PutVal(i, cValue(i));
    if (stp == stREAL)
      pms->PutVal(i, rValue(i));
    else if (stp == stSTRING)
      pms->PutVal(i, sValue(i));
    else
      pms->PutRaw(i, iRaw(i));
  }
  trq.fUpdate(iPoints, iPoints);
  return true;
}

long PointMapVirtual::iRaw(long iRec) const
{
  return iUNDEF;
}

long PointMapVirtual::iValue(long iRec) const
{
  return iUNDEF;
}

double PointMapVirtual::rValue(long iRec) const
{
    return rUNDEF;
}

String PointMapVirtual::sValue(long iRec, short iWidth, short iDec) const
{
  return sUNDEF;
}

Coord PointMapVirtual::cValue(long iRec) const
{
  return crdUNDEF;
}

void PointMapVirtual::CreatePointMapStore()
{
  if (0 != pms) {
    delete pms;
    pms = 0;
  }  
  pms = new PointMapStore(fnObj, ptr, iPnt());
//  ptr.SetDataReadOnly(); // previous statement sets it on false
}




