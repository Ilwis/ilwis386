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
#include "Engine\Table\GraphObject.h"
#include "Engine\Base\DataObjects\ObjectStructure.h"

IlwisObjectPtrList GraphObject::listGraphObject;

GraphObjectPtr::GraphObjectPtr() 
		: IlwisObjectPtr()
{
}

GraphObjectPtr::GraphObjectPtr(const ElementContainer& ec) 
	: IlwisObjectPtr(ec)
{
}

String GraphObjectPtr::sType() const
{
	return "Graph";
}

void GraphObjectPtr::Store() 
{
  IlwisObjectPtr::Store();
  WriteElement("Ilwis", "Type", "Graph");
}

bool GraphObjectPtr::fDependent() const
{
	// A graph object USES a table, but is not dependent in the ILWIS sense
	// therefore return false
	return false;
//	long iNr;
//	long iRet = ObjectInfo::ReadElement("ObjectDependency", "NrDepObjects", fnObj, iNr);
//	return iRet > 0 && iNr > 0;
}

void GraphObjectPtr::GetObjectStructure(ObjectStructure& os)
{
	IlwisObjectPtr::GetObjectStructure(os);
	if ( os.fGetAssociatedFiles() )
	{
		int iNr = 0;
		ObjectInfo::ReadElement("Graph", "NrGraphs", fnObj, iNr);
		for (int i = 0; i < iNr; ++i)
		{
			String sSection("Graph %d", i);
			os.AddFile(fnObj, sSection, "Table");
		}
		// Get the information about the axes
		os.AddFile(fnObj, "Graph X", "Domain");
		os.AddFile(fnObj, "Graph X", "DomainData");
		os.AddFile(fnObj, "Graph YLeft", "Domain");
		os.AddFile(fnObj, "Graph YLeft", "DomainData");
		os.AddFile(fnObj, "Graph YRight", "Domain");
		os.AddFile(fnObj, "Graph YRight", "DomainData");
	}
}

GraphObject::GraphObject() 
	: IlwisObject(listGraphObject)
{
	SetPointer(new GraphObjectPtr());
}

GraphObject::GraphObject(const ElementContainer& ec) 
	: IlwisObject(listGraphObject)
{
	SetPointer(new GraphObjectPtr(ec));
}

GraphObject::GraphObject(const GraphObject& lo)
	: IlwisObject(listGraphObject, lo.pointer()) 
{}
