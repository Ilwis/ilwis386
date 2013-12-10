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
#include "Engine\Base\DataObjects\CriteriaTreeObject.h"
#include "Engine\Base\File\ElementMap.h"


IlwisObjectPtrList CriteriaTreeObject::listCriteriaTreeObject;

CriteriaTreeObjectPtr::CriteriaTreeObjectPtr() 
		: IlwisObjectPtr()
{
}

CriteriaTreeObjectPtr::CriteriaTreeObjectPtr(const ElementContainer& ec) 
	: IlwisObjectPtr(ec)
{
}

String CriteriaTreeObjectPtr::sType() const
{
	return "CriteriaTree";
}

void CriteriaTreeObjectPtr::Store() 
{
  IlwisObjectPtr::Store();
  WriteElement("Ilwis", "Type", "CriteriaTree");
}

bool CriteriaTreeObjectPtr::fUsesDependentObjects() const
{
	return false;
}

void CriteriaTreeObjectPtr::GetObjectStructure(ObjectStructure& os)
{
	IlwisObjectPtr::GetObjectStructure(os);
	if (os.fGetAssociatedFiles() ||
		( os.caGetCommandAction() != ObjectStructure::caCOPY &&
		  os.caGetCommandAction() != ObjectStructure::caDELETE)
		)
	{
		CFile cfCriteriaTreeFile(fnObj.sFullPath().c_str(), CFile::modeRead);
		CArchive ar(&cfCriteriaTreeFile, CArchive::load);
		
		ElementContainer en;
		en.em = new ElementMap; // deleted in ~ElementContainer
		// First read the entire ElementMap
		en.em->Serialize(ar);

		// Read the maps of the criteria tree
		EffectGroupGetObjectStructure("Root", en, os);

		// Include the map or mapview that is used for overlay
		CriteriaTreeDocGetObjectStructure(en, os);
	}
}

void CriteriaTreeObjectPtr::EffectGroupGetObjectStructure(const char* sSection, const ElementContainer& en, ObjectStructure& os)
{
	String sName;
	ObjectInfo::ReadElement(sSection, "Name", en, sName);
	int iNrOutputMaps;
	if (!ObjectInfo::ReadElement(sSection, "NrOutputMaps", en, iNrOutputMaps))
		iNrOutputMaps = 1;
	for (int i=0; i<iNrOutputMaps; ++i) {
		FileName fnOutputMap;
		ObjectInfo::ReadElement(sSection, String("OutputMap%d", i).c_str(), en, fnOutputMap);
		if (fnOutputMap.fExist())
		{
			os.AddFile(fnOutputMap);
			// Retrieve the files belonging to the map (georef, tables, domains)
			if (IlwisObject::iotObjectType(fnOutputMap) != IlwisObject::iotANY)
			{
				IlwisObject obj = IlwisObject::obj(fnOutputMap);
				if ( obj.fValid())
					obj->GetObjectStructure(os);
			}					
		}

		String sSlicedMapName ("%S_sliced", fnOutputMap.sShortName(false));
		FileName fnSlicedMapName (sSlicedMapName.sQuote(), ".mpr");
		if (fnSlicedMapName.fExist()) {
			os.AddFile(fnSlicedMapName);
			// Retrieve the files belonging to the map (georef, tables, domains)
			if (IlwisObject::iotObjectType(fnSlicedMapName) != IlwisObject::iotANY)
			{
				IlwisObject obj = IlwisObject::obj(fnSlicedMapName);
				if ( obj.fValid())
					obj->GetObjectStructure(os);
			}
		}

		String sContourMapName ("%S_contours", fnOutputMap.sShortName(false));
		FileName fnContourMapName (sContourMapName.sQuote(), ".mps");
		if (fnContourMapName.fExist()) {
			os.AddFile(fnContourMapName);
			// Retrieve the files belonging to the map (tables, domains)
			if (IlwisObject::iotObjectType(fnContourMapName) != IlwisObject::iotANY)
			{
				IlwisObject obj = IlwisObject::obj(fnContourMapName);
				if ( obj.fValid())
					obj->GetObjectStructure(os);
			}

			CFileFind finder;
			String strPattern(fnContourMapName.sFullPath(false) + "_*.mps");
			BOOL bWorking = finder.FindFile(strPattern.c_str());
			while (bWorking)
			{
				bWorking = finder.FindNextFile();
				FileName fnDetailedContourMapName(finder.GetFilePath());
				os.AddFile(fnDetailedContourMapName);
				if (IlwisObject::iotObjectType(fnDetailedContourMapName) != IlwisObject::iotANY)
				{
					IlwisObject obj = IlwisObject::obj(fnDetailedContourMapName);
					if ( obj.fValid())
						obj->GetObjectStructure(os);
				}
			}

			finder.Close();
		}
	}

	int iNrChildren;
	if (!ObjectInfo::ReadElement(sSection, "NrChildren", en, iNrChildren))
		iNrChildren = 0;
	for (int i=0; i<iNrChildren; ++i)
	{
		String sType;
		ObjectInfo::ReadElement(sSection, String("Child%dType", i).c_str(), en, sType);
		if ("Group" == sType)
			EffectGroupGetObjectStructure(String("%s_%S_%d", sSection, sName, i).c_str(), en, os);
		else if ("Map" == sType)
			MapEffectGetObjectStructure(String("%s_%S_%d", sSection, sName, i).c_str(), en, os);
	}
}

void CriteriaTreeObjectPtr::MapEffectGetObjectStructure(const char* sSection, const ElementContainer& en, ObjectStructure& os)
{
	// If a map is used for aggregation, read it
	String sOperation;
	bool fAggregate = (0 != ObjectInfo::ReadElement(sSection, "AggregateValueOperation", en, sOperation));
	if (fAggregate)
	{
		FileName fnAdditional;
		if (0 != ObjectInfo::ReadElement(sSection, "AggregateValueAdditionalFilename", en, fnAdditional)) {
			if (fnAdditional.fExist())
			{
				os.AddFile(fnAdditional);
				// Retrieve the files belonging to the map (georef, tables, domains)
				if (IlwisObject::iotObjectType(fnAdditional) != IlwisObject::iotANY)
				{
					IlwisObject obj = IlwisObject::obj(fnAdditional);
					if ( obj.fValid())
						obj->GetObjectStructure(os);
				}
			}
		}
	}

	// Then continue with the map effect

	int iSize;
	if (!ObjectInfo::ReadElement(sSection, "NrInputMaps", en, iSize))
		iSize = 1;
	for (int i=0; i<iSize; ++i)
	{
		String sMapFileName; // this is a workaround -- sCol is lost when reading a FileName
		ObjectInfo::ReadElement(sSection, String("InputMap%d", i).c_str(), en, sMapFileName);
		FileName fnMap = sMapFileName;
		FileName fn (fnMap);
		fn.sCol = ""; // remove sCol otherwise the ObjectStructure functions have a wrong result
		// The only reason for above lines (first add the column, then remove it) is to keep it the same as the original code in CriteiaTreeItem.cpp
		os.AddFile(fn); 
		// Retrieve the files belonging to the map (georef, tables, domains)
		if (IlwisObject::iotObjectType(fn) != IlwisObject::iotANY)
		{
			IlwisObject obj = IlwisObject::obj(fn);
			if ( obj.fValid())
				obj->GetObjectStructure(os);
		}

		String sStandardizedMapName ("%S_standardized", fnMap.sShortName(false));
		FileName fnStandardizedMapName (sStandardizedMapName.sQuote(), ".mpr");
		if (fnStandardizedMapName.fExist())
		{
			os.AddFile(fnStandardizedMapName);
			// Retrieve the files belonging to the map (georef, tables, domains)
			if (IlwisObject::iotObjectType(fnStandardizedMapName) != IlwisObject::iotANY)
			{
				IlwisObject obj = IlwisObject::obj(fnStandardizedMapName);
				if ( obj.fValid())
					obj->GetObjectStructure(os);
			}
		}
	}
}

void CriteriaTreeObjectPtr::CriteriaTreeDocGetObjectStructure(const ElementContainer& en, ObjectStructure& os)
{
	int iOverlayMapsOption = 0;
	if (!ObjectInfo::ReadElement("Display", "Mode", en, iOverlayMapsOption))
		iOverlayMapsOption = 0;

	FileName fnToCopy;
	switch(iOverlayMapsOption)
	{
		case 0:
			break;
		case 1:
			ObjectInfo::ReadElement("Display", "Map", en, fnToCopy);
			break;
		case 2:
			ObjectInfo::ReadElement("Display", "MapView", en, fnToCopy);
			break;
	}

	if (fnToCopy.fValid() && fnToCopy.fExist())
	{
		os.AddFile(fnToCopy);
		// Retrieve the files belonging to the map (georef, tables, domains)
		if (IlwisObject::iotObjectType(fnToCopy) != IlwisObject::iotANY)
		{
			IlwisObject obj = IlwisObject::obj(fnToCopy);
			if ( obj.fValid())
				obj->GetObjectStructure(os);
		}					
	}
}

CriteriaTreeObject::CriteriaTreeObject() 
	: IlwisObject(listCriteriaTreeObject)
{								
	SetPointer(new CriteriaTreeObjectPtr());
}

CriteriaTreeObject::CriteriaTreeObject(const ElementContainer& ec) 
	: IlwisObject(listCriteriaTreeObject)
{								
	SetPointer(new CriteriaTreeObjectPtr(ec));
}

CriteriaTreeObject::CriteriaTreeObject(const CriteriaTreeObject& cto)
	: IlwisObject(listCriteriaTreeObject, cto.pointer()) 
{}


