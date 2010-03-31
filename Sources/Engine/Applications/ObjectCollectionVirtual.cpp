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
// ObjectCollectionVirtual.cpp: implementation of the ObjectCollectionVirtual class.
//
//////////////////////////////////////////////////////////////////////

#include "Engine\Base\DataObjects\ObjectCollection.h"
#include "Engine\Applications\ObjectCollectionVirtual.h"
#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\Engine.h"


ObjectCollectionVirtual::ObjectCollectionVirtual(const FileName& fn, ObjectCollectionPtr& p, bool fCreate)
: IlwisObjectVirtual(fn, p, p.objdep, fCreate)
, ptr(p)
{
}

ObjectCollectionVirtual _export * ObjectCollectionVirtual::create(const FileName& fn, ObjectCollectionPtr& p)
{
   String sType;
  if (0 == ObjectInfo::ReadElement("ObjectCollectionVirtual", "Type", fn, sType))
    return 0;
  ApplicationInfo * info = Engine::modules.getAppInfo(sType);
  vector<void *> extraParms = vector<void *>();
  if ( info != NULL ) {
	return (ObjectCollectionVirtual *)(info->createFunction)(fn, p, "", extraParms);
  }
  throw ErrorInvalidType(fn, "ObjectCollectionVirtual", sType);

  return NULL;
}

ObjectCollectionVirtual _export *ObjectCollectionVirtual::create(const FileName& fn, ObjectCollectionPtr& p, const String& sExpression)
{
	String sFunc = IlwisObjectPtr::sParseFunc(sExpression);
	ApplicationInfo * info = Engine::modules.getAppInfo(sFunc);
	vector<void *> extraParms = vector<void *>();
	if ( info != NULL ) {
		return (ObjectCollectionVirtual *)(info->createFunction)(fn, p, sExpression, extraParms);
	}

  throw ErrorAppName(fn.sFullName(), sExpression);
  return 0;
}

ObjectCollectionVirtual::~ObjectCollectionVirtual()
{
}

void ObjectCollectionVirtual::Store()
{
  if (fnObj.sFile.length() == 0)  // empty file name
    return;
  IlwisObjectVirtual::Store();
  ptr.WriteElement("ObjectCollection", "Type", "ObjectCollectionVirtual");
}

void ObjectCollectionVirtual::Freeze()
{
  if (!objdep.fMakeAllUsable())
    return;
	trq.Start();
	String sTitle("%S - %S", sFreezeTitle, sName(true));
	trq.SetTitle(sTitle);
	trq.SetHelpTopic(htpFreeze);
  fFreezing();
  int number = ptr.iNrObjects();
  for (int i = 0; i < number; ++i)
  {
    IlwisObject o = obj(i);
    if (o.fValid())
      ObjectInfo::WriteAdditionOfFileToCollection(o->fnObj, ptr.fnObj);
  }      
	trq.Stop();
}

void ObjectCollectionVirtual::UnFreeze()
{
  int number = ptr.iNrObjects();
  for (int i = 0; i < number; ++i)
  {
    IlwisObject o = obj(i);
    if (o.fValid())
      o->fErase = true;
  }
  ptr.Store();
}

bool ObjectCollectionVirtual::fFreezing()
{
  return false;
}

String ObjectCollectionVirtual::sExpression() const
{
  return sUNDEF;
}

String ObjectCollectionVirtual::sType() const
{
  return "Dependent Object Collection";
}


