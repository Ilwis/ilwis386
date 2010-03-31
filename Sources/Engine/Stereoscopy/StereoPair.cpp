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
// StereoPair.cpp: implementation of the StereoPair class.
//
//////////////////////////////////////////////////////////////////////

#include "Headers\toolspch.h"
#include "Engine\Stereoscopy\StereoPair.h"
#include "Engine\Base\System\mutex.h"
#include "Engine\Stereoscopy\StereoPairVirtual.h"
#include "Engine\Base\DataObjects\ObjectStructure.h"
#include "Headers\Hs\stereoscopy.hs"


IlwisObjectPtrList StereoPair::listStereoPair;

StereoPair::StereoPair()
: IlwisObject(listStereoPair)
{}

StereoPair::StereoPair(const StereoPair& sp)
: IlwisObject(listStereoPair, sp.pointer())
{}

StereoPair::StereoPair(const FileName& fn, bool fOpenExisting)
: IlwisObject(listStereoPair)
{
  FileName fnStp(fn, ".stp");
  MutexFileName mut(fnStp);
  StereoPairPtr* p = static_cast<StereoPairPtr*>(StereoPair::pGet(fnStp));
  if (p) // if already open return it
    SetPointer(p);
  else
    SetPointer(StereoPairPtr::create(fnStp, fOpenExisting));
}

StereoPair::StereoPair(const FileName& fn, const String& sExpression)
: IlwisObject(listStereoPair, StereoPairPtr::create(FileName(fn, ".stp"),sExpression))
{
}
 
StereoPair::StereoPair(const FileName& fn, const Map& mpL, const Map& mpR)
: IlwisObject(listStereoPair, StereoPairPtr::create(fn, mpL, mpR))
{
} 

StereoPairPtr* StereoPair::pGet(const FileName& fn)
{
  return static_cast<StereoPairPtr*>(listStereoPair.pGet(fn));
}

StereoPairPtr::StereoPairPtr(const FileName& fn, bool fOpenExisting)
: IlwisObjectPtr(FileName(fn, ".stp"))
, pspv(0)
{
	try {
		if (fOpenExisting) 
		{
		  ReadElement("StereoPair", "Left", mapLeft);
			ReadElement("StereoPair", "Right", mapRight);
			ReadElement("StereoPair", "UseGeorefLeft", fUseGeorefLeft);
		}
	}
	catch (const ErrorObject&)
	{
	// do nothing
	}
}

StereoPairPtr::StereoPairPtr(const FileName& fn, const String& sExpr)
: IlwisObjectPtr(FileName(fn, ".stp"),true, ".stp")
, pspv(0)
{
  pspv = StereoPairVirtual::create(fnObj, *this, sExpr);
}
	
StereoPairPtr::StereoPairPtr(const FileName& fn, const Map& mpL, const Map& mpR)
: IlwisObjectPtr(FileName(fn, ".stp"),true, ".stp")
{
		pspv = StereoPairVirtual::create(fnObj, *this, mpL, mpR);
} 

StereoPairPtr::~StereoPairPtr()
{
  delete pspv;
}

StereoPairPtr* StereoPairPtr::create(const FileName& fn, bool fOpenExisting)
{
  if (!File::fExist(fn))
    NotFoundError(fn);
  // see if maplist is already open (if it is in the list of open maplists)
  MutexFileName mut(fn);
  StereoPairPtr* p = StereoPair::pGet(fn);
  if (0 != p) // if already open return it
    return p;
	try
	{
		p = new StereoPairPtr(fn, fOpenExisting);
	}
	catch (ErrorObject err)
	{
		err.Show();
		p = 0;
	}
  return p;
}

StereoPairPtr* StereoPairPtr::create(const FileName& fn, const String& sExpression)
{
	FileName fnStp(sExpression);
  MutexFileName mut(fnStp);
  StereoPairPtr* p = StereoPair::pGet(fnStp);
  if (0 != p) // if already open return it
    return p;  

	Array<String> as;
	String sFunc = IlwisObjectPtr::sParseFunc(sExpression);
  if (fCIStrEqual(sFunc.sLeft(10), "StereoPair"))
		return new StereoPairPtr(fn, sExpression);
	else
    return 0;
}

StereoPairPtr* StereoPairPtr::create(const FileName& fn, const Map& mpL, const Map& mpR)
{
  MutexFileName mut(fn);
  StereoPairPtr* p = StereoPair::pGet(fn);
  if (0 != p) // if already open return it
    return p;
	return new StereoPairPtr(fn, mpL, mpR);
} 

String StereoPairPtr::sType() const
{
	if (fDependent())
	    return "Dependent Stereo Pair";
	else
		return "Stereo Pair";
}

void StereoPairPtr::Store()
{
  ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
  if ((0 != pspv) && (sDescription == ""))
    sDescription = pspv->sExpression();
  IlwisObjectPtr::Store();
  WriteElement("Ilwis", "Type", "StereoPair");
  WriteElement("StereoPair", "Left", mapLeft);
  WriteElement("StereoPair", "Right", mapRight);
	WriteElement("StereoPair", "UseGeorefLeft", fUseGeorefLeft);
  if (pspv)
    pspv->Store();
}

bool StereoPairPtr::fDependent() const
{
  ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
  if (0 != pspv)
    return true;
  String s;
  ObjectInfo::ReadElement("StereoPair", "Type", fnObj, s);
  return fCIStrEqual(s, "StereoPairVirtual");
}

String StereoPairPtr::sExpression() const
{
  ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
  if (0 != pspv)
    return pspv->sExpression();
  return IlwisObjectPtr::sExpression();
}

void StereoPairPtr::Calc(bool fMakeUpToDate)
// calculates the result     
{
  ILWISSingleLock sl(&csCalc, TRUE, SOURCE_LOCATION);
  OpenStereoPairVirtual();
  if (fMakeUpToDate)
    if (!objdep.fUpdateAll())
      return;
  if (0 != pspv)
    pspv->Freeze();
}

void StereoPairPtr::DeleteCalc()
// deletes calculated  result     
{
  ILWISSingleLock sl(&csCalc, TRUE, SOURCE_LOCATION);
  OpenStereoPairVirtual();
  if (0 != pspv) 
    pspv->UnFreeze();
}

void StereoPairPtr::OpenStereoPairVirtual()
{
	ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
	if (0 != pspv) // already there
		return;

	if (!fDependent())
		return;

	try
	{
		pspv = StereoPairVirtual::create(fnObj, *this);
		objdep = ObjectDependency(fnObj);
	}
	catch (const ErrorObject& err)
	{
		err.Show();
		pspv = 0;
		objdep = ObjectDependency();
	}
}

IlwisObjectVirtual* StereoPairPtr::pGetVirtualObject() const
{
	return pStereoPairVirtual();
}

StereoPairVirtual* StereoPairPtr::pStereoPairVirtual() const
{
  const_cast<StereoPairPtr*>(this)->OpenStereoPairVirtual();
	return pspv;
}

bool StereoPairPtr::fCalculated() const
{
  if (fDependent())
		return ((mapLeft.fValid()  && mapLeft->fCalculated()  ) && 
		        (mapRight.fValid() && mapRight->fCalculated() ) );
	else
		return IlwisObjectPtr::fCalculated();
} 

void StereoPairPtr::BreakDependency()
{
	if (!fCalculated())
		Calc();
	if (!fCalculated())
		return; 
	ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
	delete pspv;
	pspv = 0;
	fChanged = true;
	
	// Clean up the ODF
	String sType;
	ReadElement("StereoPairVirtual", "Type", sType);  // get the actual stereopair type
	WriteElement("StereoPair", "Type", (char*)0);     // remove the dependent type (we are creating the ordinary stereopair object)
	// remove the dependent sections
	ObjectInfo::RemoveSection(fnObj, "IlwisObjectVirtual"); // IlwisObjectVirtual
	ObjectInfo::RemoveSection(fnObj, "StereoPairVirtual");  //    StereoPairVirtual
	ObjectInfo::RemoveSection(fnObj, sType);                //       StereoPairFromDTM or other
	ObjectInfo::RemoveSection(fnObj, "ObjectDependency");   // This object will not depend on any other anymore
	
	Store();
}

void StereoPairPtr::GetObjectStructure(ObjectStructure& os)
{
	IlwisObjectPtr::GetObjectStructure(os);
	if (os.fGetAssociatedFiles() ||
		( os.caGetCommandAction() != ObjectStructure::caCOPY &&
		  os.caGetCommandAction() != ObjectStructure::caDELETE)
		)
	{	
		os.AddFile(fnObj,"StereoPair", "Left" , ".mpr");				
		os.AddFile(fnObj,"StereoPair", "Right" , ".mpr");						
	}
}

void StereoPairPtr::CreatePyramidLayers()
{
	if (mapLeft.fValid() && mapRight.fValid())
	{
		mapLeft->CreatePyramidLayer();
		mapRight->CreatePyramidLayer();
	}	
}

void StereoPairPtr::DeletePyramidFiles()
{
	if (mapLeft.fValid() && mapRight.fValid())
	{
			mapLeft->DeletePyramidFile();
			mapRight->DeletePyramidFile();
	}		
}

bool StereoPairPtr::fHasPyramidFiles()
{
	if (mapLeft.fValid() && mapRight.fValid())
		return
			mapLeft->fHasPyramidFile() &&	mapRight->fHasPyramidFile();
	else
		return false;
}

const GeoRef& StereoPairPtr::gr() const
{
	if (fUseGeorefLeft)
		return mapLeft->gr();
	else
		return mapRight->gr();
}



