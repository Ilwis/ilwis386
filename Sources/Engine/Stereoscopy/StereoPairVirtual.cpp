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
// StereoPairVirtual.cpp: implementation of the StereoPairVirtual class.
//
//////////////////////////////////////////////////////////////////////

#include "Engine\Stereoscopy\StereoPairVirtual.h"
#include "Engine\Stereoscopy\StereoPairEpiPolar.h"
#include "Engine\Stereoscopy\StereoPairFromDTM.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

StereoPairVirtual::StereoPairVirtual(const FileName& fn, StereoPairPtr& p)
: IlwisObjectVirtual(fn, p, p.objdep, false)
, ptr(p)
{
}

StereoPairVirtual::StereoPairVirtual(const FileName& fn, StereoPairPtr& p, bool fCreate)
: IlwisObjectVirtual(fn, p, p.objdep, fCreate)
, ptr(p)
{
}

StereoPairVirtual::StereoPairVirtual(const FileName& fn, StereoPairPtr& p, const Map& mpL, const Map& mpR)
: IlwisObjectVirtual(fn, p, p.objdep, true)
, ptr(p)
{
	FileName fnStereoPair = FileName(fn);
	fnStereoPair.sExt = ".stp";
	if (fnStereoPair.fExist())
        FileAlreadyExistError(fnStereoPair);
	objdep.Add(mpL);
	objdep.Add(mpR);
}

StereoPairVirtual::~StereoPairVirtual()
{
}

StereoPairVirtual _export * StereoPairVirtual::create(const FileName& fn, StereoPairPtr& p)
{
  String sType;
  ObjectInfo::ReadElement("StereoPairVirtual", "Type", fn, sType);

  if (fCIStrEqual("StereoPairEpiPolar" , sType))
    return new StereoPairEpiPolar(fn, p);
	if (fCIStrEqual("StereoPairFromDTM" , sType))
    return new StereoPairFromDTM(fn, p);

  throw ErrorInvalidType(fn, "StereoPairVirtual", sType);
  return 0;
}

StereoPairVirtual _export *StereoPairVirtual::create(const FileName& fn, StereoPairPtr& p, const String& sExpression)
{
  String sFunc = IlwisObjectPtr::sParseFunc(sExpression);

  //if (fCIStrEqual("StereoPairEpiPolar" , sFunc))
  //  return new StereoPairEpiPolar(fn, p, sExpression);
	if (fCIStrEqual("StereoPairFromDTM" , sFunc))
    return StereoPairFromDTM::create(fn, p, sExpression);
  //throw ErrorAppName(fn.sFullName(), sExpression);
  return 0;
} 

StereoPairVirtual _export *StereoPairVirtual::create(const FileName& fn, StereoPairPtr& p, 
																										 const Map& mp1, const Map& mp2) 
																									
{
  return new StereoPairEpiPolar(fn, p, mp1, mp2);
}

void StereoPairVirtual::Store()
{
  if (fnObj.sFile.length() == 0)  // empty file name
    return;
  IlwisObjectVirtual::Store();
  ptr.WriteElement("StereoPair", "Type", "StereoPairVirtual");
}

void StereoPairVirtual::Freeze()
{
  if (!objdep.fMakeAllUsable())
    return;
	trq.Start();
	String sTitle("%S - %S", sFreezeTitle, sName(true));
	trq.SetTitle(sTitle);
	trq.SetHelpTopic(htpFreeze);
  UnFreeze();
  if (fFreezing())
	{
    ptr.Updated();
		ptr.Store();
	  if (ptr.mapLeft.fValid())
	    ObjectInfo::WriteAdditionOfFileToCollection(ptr.mapLeft->fnObj, ptr.fnObj);
	  if (ptr.mapRight.fValid())
	    ObjectInfo::WriteAdditionOfFileToCollection(ptr.mapRight->fnObj, ptr.fnObj);
		if (ptr.mapLeft.fValid() && ptr.mapLeft->gr().fValid())
	    ObjectInfo::WriteAdditionOfFileToCollection(ptr.mapLeft->gr()->fnObj, ptr.fnObj);
		if (ptr.mapRight.fValid() && ptr.mapRight->gr().fValid())
	    ObjectInfo::WriteAdditionOfFileToCollection(ptr.mapRight->gr()->fnObj, ptr.fnObj);
	}
	else
		UnFreeze();
	trq.Stop();
}

void StereoPairVirtual::UnFreeze()
{
  if (ptr.mapLeft.fValid()) 
  {
    ptr.mapLeft->fErase = true;
    ptr.mapLeft.SetPointer(0);
  }
  if (ptr.mapRight.fValid())
  {
    ptr.mapRight->fErase = true;
    ptr.mapRight.SetPointer(0);
  }
  ptr.Store();
}

bool StereoPairVirtual::fFreezing()
{
  return false;
}

String StereoPairVirtual::sExpression() const
{
  return sUNDEF;
}


