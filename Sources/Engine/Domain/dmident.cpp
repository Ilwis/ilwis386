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
/* DomainIdentifier
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  WK    2 Jun 97    6:10 pm
*/

#include "Engine\Domain\dmident.h"
#include "Engine\Domain\DomainUniqueID.h"


DomainIdentifier::DomainIdentifier(const FileName& fn)
: DomainSort(fn)
{
	iNr = iReadElement("DomainIdentifier", "Nr");
}

void DomainIdentifier::Store()
{
  DomainSort::Store();
  WriteElement("Domain", "Type", "DomainIdentifier");
  WriteElement("DomainIdentifier", "Nr", iNr);
}

String DomainIdentifier::sType() const
{
  return "Domain Identifier";
}

DomainIdentifier::DomainIdentifier(const FileName& fn, long nr, const String& sPrefix)
: DomainSort(fn, nr, sPrefix)
{
  if (sPrefix.length())
    _iWidth = sPrefix.length() + 5;
  else  
    _iWidth = 6;
}

Representation DomainIdentifier::rprLoad(bool fShowOnError)
{
  return Representation();
}


bool DomainIdentifier::fEqual(const IlwisObjectPtr& ptr) const
{
	// DomainUniqueID by definition is not equal to any another domain (object)
	// Bas Retsios 14-7-2017: reverted this decision; two DomainUniqueIDs are equal when their nr of elements are the same.
	// Strictly spoken they are indeed not equal, but this is very inconvenient, because user can't Join columns when he knows it should be possible.
	const DomainUniqueID* pdu = dynamic_cast<const DomainUniqueID*>(&ptr);
	if (pdu != 0)
		return pdu->fEqual(*this);

	const DomainIdentifier* pdi = dynamic_cast<const DomainIdentifier*>(&ptr);
	if (pdi == 0)
		return false;

	return DomainSort::fEqual(ptr);
}




