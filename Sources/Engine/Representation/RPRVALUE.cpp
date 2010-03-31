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
/* $Log: /ILWIS 3.0/Representation/RPRVALUE.cpp $
 * 
 * 4     22/05/00 9:06 Willem
 * header comments... second try
 * 
 * 3     22/05/00 9:06 Willem
 * header comments...
 * 
 * 2     19-05-00 3:08p Martin
 * CheckDomainRange disabled for value.dom. value.dom may now change its
 * ranges
 */
// Revision 1.3  1998/09/16 17:25:53  Wim
// 22beta2
//
// Revision 1.2  1997/09/18 08:29:00  Wim
// Added CheckDomainRange() to update the limits based on the eventually changed
// vr of the domain. This function is called in the constructor and the editor
//
/* RepresentationValue
   by Wim Koolhoven
  (c) Ilwis System Development ITC
	Last change:  WK   18 Sep 97   10:28 am
*/

#include "Engine\Representation\RPRVALUE.H"
#include "Engine\Domain\Dmvalue.h"

RepresentationValue::RepresentationValue(const FileName& fn)
: RepresentationGradual(fn)
{
  CheckDomainRange();
}

RepresentationValue::RepresentationValue(const FileName& fn, const Domain& dom)
: RepresentationGradual(fn,dom)
{
  DomainValue* dv = dom->pdv();
  RangeReal rr = dv->rrMinMax();
  arLimits[0] = rr.rLo();
  arLimits[1] = rr.rHi();
  init();
}

void RepresentationValue::Store()
{
  RepresentationGradual::Store();
  WriteElement("Representation", "Type", "RepresentationValue");
}

RepresentationValue::~RepresentationValue()
{
}
  
String RepresentationValue::sType() const
{
  return "Representation Value";
}

void RepresentationValue::CheckDomainRange()
{
	if ( fCIStrEqual(dm()->fnObj.sShortName(), "value.dom")) //value.dom may have different ranges from the default
		return;
  DomainValue* dv = dm()->pdv();
  RangeReal rr = dv->rrMinMax();
  int iMax = arLimits.iSize() - 1;
  if (arLimits[0] != rr.rLo()) {
    arLimits[0] = rr.rLo();
    for (int i = 1; i <= iMax; ++i)
      if (arLimits[i] < rr.rLo())
        arLimits[i] = rr.rLo();
      else
        break;
  }
  if (arLimits[iMax] != rr.rHi()) {
    arLimits[iMax] = rr.rHi();
    for (int i = iMax; i > 0; --i)
      if (arLimits[i] > rr.rHi())
        arLimits[i] = rr.rHi();
      else
        break;
  }
}





