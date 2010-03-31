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
/* $Log: /ILWIS 3.0/BasicDataStructures/objvirt.cpp $
 * 
 * 8     25-02-02 15:20 Willem
 * Added some protection against invalid object collections. Occured when
 * the object collection referred to in ODF of a map did not exist anymore
 * 
 * 7     29-08-01 18:54 Koolhoven
 * in Store() adding to obectcollection is now only done if it is really
 * an object collection (.ioc) and not something similar like a maplist
 * orso
 * 
 * 6     8/10/01 11:54a Martin
 * virtual objects will now add themselves to an ioc if all their input
 * objects are part of an ioc
 * 
 * 5     21-12-00 10:17a Martin
 * added the pGetVirtualObject() function to access the virtual object
 * embedded in the object. 
 * 
 * 4     29-10-99 16:32 Koolhoven
 * last comment caused problems
 * 
 * 3     29/10/99 9:40 Willem
 * Comments changed: // -> in $Log section
 * 
 * 2     29-10-99 9:20 Wind
 * thread save stuff
 */
// Revision 1.3  1998/09/16 17:22:46  Wim
// 22beta2
//
// Revision 1.2  1997/08/21 12:23:47  Wim
// In Store() it is now made sure that the [Ilwis] section is written first
//
/* IlwisObjectVirtual
   Copyright Ilwis System Development ITC
   april 1997, by Jelle Wind
	Last change:  WK   21 Aug 97    2:23 pm
*/

#pragma warning( disable : 4786 )

#include "Engine\Applications\objvirt.h"
#include "Engine\Base\DataObjects\ObjectCollection.h"


IlwisObjectVirtual::IlwisObjectVirtual(const FileName& fn, IlwisObjectPtr& _ptr, ObjectDependency& objdp, bool fCreate)
: fnObj(_ptr.fnObj), optr(_ptr), objdep(objdp), objtime(_ptr.objtime)
{
//  optr._fDataReadOnly = true;
  fNeedFreeze = true;  
  if (fCreate) {
    optr.SetDescription("");
//  fFrozen = false;
    optr.fChanged = true;
  } 
  else {
    bool f;
    if (0 != ReadElement("IlwisObjectVirtual", "DefOnlyPossible", f))
      fNeedFreeze = !f;
    if (optr.sDescription.length() == 0)
      ReadElement("IlwisObjectVirtual", "Expression", optr.sDescription);
  }  
}

IlwisObjectVirtual::~IlwisObjectVirtual()
{
}

void IlwisObjectVirtual::InitFreeze()
{
  // nothing to do
}

void IlwisObjectVirtual::ExitFreeze()
{
  // nothing to do
}

void IlwisObjectVirtual::Store()
{
	optr.WriteBaseInfo("Ilwis"); // to make sure that [ilwis] is at the top
	WriteElement("IlwisObjectVirtual", "Expression", sExpression());
	WriteElement("IlwisObjectVirtual", "DefOnlyPossible", (bool)!fNeedFreeze);
	Array<String> asNames;
	objdep.Names("", asNames);
	// if an object is generated while its part of a collection its output will also be part of the collectio
	// if all the dataobjects needed are also in the collection
	int iDataObjects = 0;	
	map<String, int> mpCount;	
	int iNr;	
	for(unsigned int i=0; i < asNames.iSize(); ++i)
	{
		FileName fn(asNames[i]);
		if (ObjectInfo::fIsDataObject(fn))
		{
			++iDataObjects;
			if (ObjectInfo::fInCollection(fn))
			{
				ObjectInfo::ReadElement("Collection", "NrOfItems", fn, iNr);
				for(unsigned int iC = 0; iC < (unsigned int)iNr; ++iC)
				{
					FileName fnCol;
					ObjectInfo::ReadElement("Collection", String("Item%d", iC).scVal(), fn, fnCol);
					// only for real object colections, not for maplists etc.
					if (".ioc" != fnCol.sExt)
						continue;
					if (mpCount.find(fnCol.sFullPath()) != mpCount.end())
						mpCount[fnCol.sFullPath()]++;
					else
						mpCount[fnCol.sFullPath()] = 1;
					
				}					
			}				
		}			
	}				
	for(map<String, int>::iterator cur = mpCount.begin(); cur != mpCount.end(); ++cur)
	{
		// the object will only be added to a collection if all dataobjects are part of it
		if ((*cur).second == iDataObjects)
		{
			ObjectCollection oc((*cur).first);
			if (oc.fValid())
				oc->Add(fnObj);
		}			
	}		
	objdep.Store(&optr);
}

String IlwisObjectVirtual::sExpression() const
{
  return String();
}

bool IlwisObjectVirtual::fDomainChangeable() const
{
  return false;
}

bool IlwisObjectVirtual::fValueRangeChangeable() const
{
  return false;
}

bool IlwisObjectVirtual::fGeoRefChangeable() const
{
  return false;
}

bool IlwisObjectVirtual::fExpressionChangeable() const
{
  return false;
}


void IlwisObjectVirtual::InitName(const FileName& fn)
{
  // nothing
}

const ObjectDependency& IlwisObjectVirtual::odGetObjectDependencies() const
{
	return objdep;
}



