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
#include "Headers\toolspch.h"
#include "Engine\Domain\DomainUniqueID.h"

DomainUniqueID::DomainUniqueID(const FileName& fn) 
:	DomainIdentifier(fn)
{
}

DomainUniqueID::DomainUniqueID(const FileName& fn, String _sPrefix, int _iNr) :
	DomainIdentifier(fn, _iNr, _sPrefix)
{
}

String DomainUniqueID::sType() const
{
  return "Domain UniqueID";
}

void DomainUniqueID::Store()
{
	fUpdateCatalog = false;
  DomainIdentifier::Store();
  WriteElement("Domain", "Type", "DomainUniqueID");
}

// Added stNeeded() to have a better value for DomainUniqueID
// Otherwise DomainSort::stNeeded() would return stBIT for unique ID domain
// For an application see MapAreaNumbering
StoreType DomainUniqueID::stNeeded() const
{
	return stLONG;
}

long DomainUniqueID::iAdd()
{
	iNr += 1;
	Updated();
	Table::EnlargeTables(this);
	return iNr;
}

void DomainUniqueID::Merge(const DomainSort* pdsrt, Tranquilizer* trq)
{
	for(int i=0; i < pdsrt->iSize(); ++i)
		iAdd();

	Table::EnlargeTables(this);	
}

// DomainUniqueID by definition is not equal to any another domain (object)
// Bas Retsios 14-7-2017: reverted this decision; two DomainUniqueIDs are equal when their nr of elements are the same.
// Strictly spoken they are indeed not equal, but this is very inconvenient, because user can't Join columns when he knows it should be possible.
bool DomainUniqueID::fEqual(const IlwisObjectPtr& ptr) const
{
	const DomainUniqueID* pdu = dynamic_cast<const DomainUniqueID*>(&ptr);
	if (pdu != 0)
		 return iNr == pdu->iNr;
	else
		return false;
}
