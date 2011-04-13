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
/* $Log: /ILWIS 3.0/SegmentMap/SEGVIRT.cpp $
 * 
 * 8     9-11-05 12:40 Retsios
 * Added new application SegmentMapFromRasValueBnd that generates contour
 * maps from raster value maps.
 * 
 * 7     1-03-00 17:21 Wind
 * protect UnFreeze with try..catch
 * 
 * 6     17-01-00 8:17a Martin
 * changed rowcols to coords
 * 
 * 5     22-11-99 12:22 Wind
 * same as previous, but different implementation
 * 
 * 4     22-11-99 12:15 Wind
 * addec object aname to title of report window when calculating
 * 
 * 3     9/08/99 10:27a Wind
 * adpated to use of quoted file names
 * 
 * 2     3/10/99 4:14p Martin
 * Case insensitive support added
// Revision 1.3  1998/09/16 17:25:20  Wim
// 22beta2
//
// Revision 1.2  1997/08/25 17:50:13  Wim
// Added (empty) DeleteInternals() to BaseMapVirtual
// to call in the UnFreeze() routine.
//
/* SegmentMapVirtual
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  WK   25 Aug 97    3:40 pm
*/

#include "Engine\Applications\SEGVIRT.H"
#include "Engine\Map\Segment\SEGSTORE.H"
#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\Engine.h"
#include "Engine\Base\DataObjects\valrange.h"

SegmentMapVirtual* SegmentMapVirtual::create(const FileName& fn, SegmentMapPtr& p)
{
  String sType;
  if (0 == ObjectInfo::ReadElement("SegmentMapVirtual", "Type", fn, sType))
    return 0;
  vector<CommandInfo *> infos;
  Engine::modules.getCommandInfo(sType, infos);
  vector<void *> extraParms = vector<void *>();
  if ( infos.size() > 0 ) {
	return (SegmentMapVirtual *)(infos[0]->createFunction)(fn, p, "", extraParms);
  }
  throw ErrorInvalidType(fn, "SegmentMapVirtual", sType);

  return NULL;
}

SegmentMapVirtual* SegmentMapVirtual::create(const FileName& fn, SegmentMapPtr& p, const String& sExpression)
{

	String sFunc = IlwisObjectPtr::sParseFunc(sExpression);
	vector<CommandInfo *> infos;
	Engine::modules.getCommandInfo(sFunc, infos);
	vector<void *> extraParms = vector<void *>();
	if ( infos.size() > 0 ) {
		return (SegmentMapVirtual *)(infos[0]->createFunction)(fn, p, sExpression, extraParms);
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
		Engine::modules.getCommandInfo("SegmentMapAttribute", infos);
		vector<void *> extraParms = vector<void *>();
		SegmentMap pmap(fnMap);
		extraParms.push_back( &pmap);
		extraParms.push_back( &sCol);
		if ( infos.size() > 0) {
			return (SegmentMapVirtual *)(infos[0]->createFunction)(fn, p, "", extraParms);
		}
	}
	throw ErrorAppName(fn, sFunc);
	
	return NULL;
}

SegmentMapVirtual::SegmentMapVirtual(const FileName& fn, SegmentMapPtr& _ptr)
: BaseMapVirtual(fn, _ptr), ptr(_ptr), pms(_ptr.pms)
{
}

SegmentMapVirtual::SegmentMapVirtual(const FileName& fn, SegmentMapPtr& _ptr, const CoordSystem& cs,
                                     const CoordBounds& cb, const DomainValueRangeStruct& dvs)
: BaseMapVirtual(fn,_ptr,cs,cb,dvs), ptr(_ptr), pms(_ptr.pms)
{
/*  ptr.rAlfa = max(cb.width(), cb.height()) / 30000;
  ptr.rBeta1 = cb.MinX()/2 + cb.MaxX()/2;
  ptr.rBeta2 = cb.MinY()/2 + cb.MaxY()/2;*/
}

SegmentMapVirtual::~SegmentMapVirtual()
{
}

void SegmentMapVirtual::Store()
{
  IlwisObjectVirtual::Store();
  WriteElement("SegmentMap", "Type", "SegmentMapVirtual");
  // for downward compatibility to 2.02:
  WriteElement("SegmentMapVirtual", "Expression", sExpression());
}

String SegmentMapVirtual::sExpression() const
{
  return sUNDEF;
}

String SegmentMapVirtual::sType() const
{
  return "Dependent Segment Map";
}


bool SegmentMapVirtual::fDomainChangeable() const
{
  return fValues();
}

bool SegmentMapVirtual::fValueRangeChangeable() const
{
  return fValues();
}

void SegmentMapVirtual::Freeze()
{
  if (!objdep.fMakeAllUsable())
    return;
  UnFreeze();
  InitFreeze();
  CreateSegmentMapStore();
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
    FileName fnHis(fnObj, ".hss", true);
    _unlink(fnHis.sFullName().scVal());
    fnHis.sExt = ".hs#";
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

void SegmentMapVirtual::UnFreeze()
{
  if (0 == pms) 
    return;
  try {
    pms->SetErase(true);
    delete pms;
  }
  catch (...) {
  }
  pms = 0;
  DeleteInternals();
//  fFrozen = false;
  ptr._iSeg = ptr._iSegDeleted = ptr._iCrd = 0;
  ptr.Store();
}

bool SegmentMapVirtual::fFreezing()
{
  return false;
}

void SegmentMapVirtual::CreateSegmentMapStore()
{
  if (0 != pms) {
    delete pms;
    pms = 0;
  }  
  pms = new SegmentMapStore(fnObj, ptr, true);
//  ptr.SetDataReadOnly(); // previous statement sets it on false
}




