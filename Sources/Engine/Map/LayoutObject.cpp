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
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Engine\Base\DataObjects\ObjectStructure.h"
#include "Engine\Map\LayoutObject.h"

IlwisObjectPtrList LayoutObject::listLayoutObject;

LayoutObjectPtr::LayoutObjectPtr() 
		: IlwisObjectPtr()
{
}

LayoutObjectPtr::LayoutObjectPtr(const ElementContainer& ec) 
	: IlwisObjectPtr(ec)
{
}

String LayoutObjectPtr::sType() const
{
	return "Layout";
}

void LayoutObjectPtr::Store() 
{
  IlwisObjectPtr::Store();
  WriteElement("Ilwis", "Type", "Layout");
}

void LayoutObjectPtr::GetObjectStructure( ObjectStructure& os)
{
	IlwisObjectPtr::GetObjectStructure( os );
	if ( os.fGetAssociatedFiles() )
	{
		int iNr=0;
		ObjectInfo::ReadElement("Layout", "Items", fnObj, iNr);
		for(int i=1; i <= iNr; ++i)
		{
			String sWhere("Item %d", i);
			String sKey;				
			ObjectInfo::ReadElement("Layout", sWhere.c_str(), fnObj, sKey);
			FileName fnFile;					
			ObjectInfo::ReadElement(sWhere.c_str(), sKey.c_str(), fnObj, fnFile);
			if ( IlwisObject::iotObjectType( fnFile) != IlwisObject::iotANY || 
				   fCIStrEqual(fnFile.sExt, ".bmp") || 
					 fCIStrEqual(fnFile.sExt, ".wmf"))
			  os.AddFile(fnObj, sWhere, sKey, "", os.fRetrieveContentsContainer() ? false : true);				
		}				
	}				
}

bool LayoutObjectPtr::fUsesDependentObjects() const
{
	int iNr=0;
	ObjectInfo::ReadElement("Layout", "Items", fnObj, iNr);
	for(int i=1; i <= iNr; ++i)
	{
		String sWhere("Item %d", i);
		String sKey;				
		ObjectInfo::ReadElement("Layout", sWhere.c_str(), fnObj, sKey);
		FileName fnFile;					
		ObjectInfo::ReadElement(sWhere.c_str(), sKey.c_str(), fnObj, fnFile);
		if ( IlwisObject::iotObjectType( fnFile) != IlwisObject::iotANY )
		{
			IlwisObject obj = IlwisObject::obj(fnFile);
			if ( obj.fValid() && obj->fDependent() )
				return true;
		}				
	}			
	return false;
}

LayoutObject::LayoutObject() 
	: IlwisObject(listLayoutObject)
{								
	SetPointer(new LayoutObjectPtr());
}

LayoutObject::LayoutObject(const ElementContainer& ec) 
	: IlwisObject(listLayoutObject)
{								
	SetPointer(new LayoutObjectPtr(ec));
}

LayoutObject::LayoutObject(const LayoutObject& lo)
	: IlwisObject(listLayoutObject, lo.pointer()) 
{}


