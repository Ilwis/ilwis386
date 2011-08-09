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
/*	if (os.fGetAssociatedFiles() ||
		( os.caGetCommandAction() != ObjectStructure::caCOPY &&
		  os.caGetCommandAction() != ObjectStructure::caDELETE)
		)
	{
		CFile cfCriteriaTreeFile(fnObj.sFullPath().c_str(), CFile::modeRead);
		CArchive ar(&cfCriteriaTreeFile, CArchive::load);
		//ElementMap em = new ElementMap;
		//em->Serialize(ca);
		
		ElementContainer en;
		en.em = new ElementMap; // deleted in ~ElementContainer
		// First read the entire ElementMap
		en.em->Serialize(ar);
		// Now reconstruct the entire criteria tree
		EffectGroup egCriteriaTreeRoot ((CriteriaTreeDoc*)0,"");
		egCriteriaTreeRoot.ReadElements("Root", en);

		// Then use its GetObjectStructure function to fill os
		egCriteriaTreeRoot.GetObjectStructure(os);
		
		// Include the map or mapview that is used for overlay
		CriteriaTreeDoc::GetObjectStructure(en, os); // static function }
}*/
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


