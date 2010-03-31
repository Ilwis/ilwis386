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
/* DomainNone
   Copyright Ilwis System Development ITC
   sept 1995, by Jelle Wind
	Last change:  JEL  28 May 97    1:06 am
*/

#include "Engine\Domain\dmnone.h"
#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\Engine.h"

DomainNone::DomainNone()
: DomainPtr(FileName())
{
    const String fs = getEngine()->getContext()->sStdDir();
    FileName fn;
    fn.Dir(fs);
    fn.sFile = String("none");
    fn.sExt = String(".dom");
		SetFileName(fn);
}

void DomainNone::Store()
{
}

bool DomainNone::fEqual(const IlwisObjectPtr& ptr) const
{
  const DomainNone* dn = dynamic_cast<const DomainNone*>(&ptr);
  return 0 != dn;
}

String DomainNone::sName(bool fExt, const String&) const
{
	if (fExt)
		return "None.dom";
	else
		return "None";
}

StoreType DomainNone::stNeeded() const
{ return stBYTE; }


String DomainNone::sValueByRaw(long iRaw, short iWidth, short) const
{
  if (iWidth == -1)
    return String("%li", iRaw); 
  else
    return String("%*li", iWidth, iRaw);
}





