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
/* DomainPicture
   Copyright Ilwis System Development ITC
   may 1995, by Wim Koolhoven
	Last change:  JEL   8 Jul 97    3:26 pm
*/

#include "Headers\toolspch.h"
#include "Engine\Domain\dmpict.h"
#include "Engine\Domain\dmcolor.h"
#include "Engine\Representation\Rprclass.h"


DomainPicture::DomainPicture(const FileName& fn)
: DomainPtr(fn)
{
  _iColors = shortConv(iReadElement("DomainPicture", "Colors"));
  _iWidth = 13;
}

DomainPicture::DomainPicture(const FileName& fn, int iNr)
: DomainPtr(fn, true), _iColors(iNr)
{
  fChanged = true;
  _iWidth = 13;
//  Domain dom;
//  dom.SetPointer(this);
//  _rpr.SetPointer(new RepresentationClass(fn, dom));
}

DomainPicture::~DomainPicture()
{
	if (fErase)
	{
		Representation rp = rpr();
		rp->fErase = true;
	}
}

String DomainPicture::sType() const
{
  return "Domain Picture";
}

void DomainPicture::Store()
{
  DomainPtr::Store();
  WriteElement("Domain", "Type", "DomainPicture");
  WriteElement("DomainPicture", "Colors", iColors());
}

bool DomainPicture::fEqual(const IlwisObjectPtr& ptr) const
{
  const DomainPicture* dp = dynamic_cast<const DomainPicture*>(&ptr);
  if (0 == dp)
    return false;
  else if (&ptr == this)
    return true;
  else {
    DomainPicture* dmp = const_cast<DomainPicture*>(dp);
    Representation rpr1 = const_cast<DomainPicture *>(this)->rpr();
    Representation rpr2 = dmp->rpr();
    if (rpr1.fValid() && rpr2.fValid())
      return rpr1 == rpr2;
    return false;
  }
}

StoreType DomainPicture::stNeeded() const
{ StoreType stp = st(iColors()); 
  if (stp < stBYTE)
    stp = stBYTE;
  return stp;
}

String DomainPicture::sValueByRaw(long iRaw, short iWidth, short) const
{
  Color clr = const_cast<DomainPicture *>(this)->rpr()->clrRaw(iRaw);
  DomainColor dc;
  return dc.sValueByRaw(clr, iWidth);
}

long DomainPicture::iRaw(const Color& col) const
{
  RepresentationClass* rprc = rpr()->prc();
  for (long i = 0; i < iColors(); ++i)
    if (col == rprc->clrRaw(i))
      return i;
  return iUNDEF;
}

void DomainPicture::Add(const Color& col)
{
  long iRaw = DomainPicture::iRaw(col);
  if (iRaw != iUNDEF)
    return;
  _iColors++;
  RepresentationClass* rprc = rpr()->prc();
  rprc->iAdd(col);
}

void DomainPicture::Merge(const DomainPicture* pdpic)
{
  Representation rpr = DomainPicture::rpr(); // to keep it in memory
  Representation rpr2 = const_cast<DomainPicture *>(pdpic)->rpr();
  if (!rpr2.fValid())
    return;
  RepresentationClass* rprc2 = rpr2->prc();
  for (long i = 0; i < pdpic->iColors(); ++i)
    Add(rprc2->clrRaw(i));
}

void DomainPicture::GetDataFiles(Array<FileName>& afnDat, Array<String>* asSection, Array<String>* asEntry) const
{
  IlwisObjectPtr::GetDataFiles(afnDat, asSection, asEntry);
  try {
    Representation rp = rpr();
    if (rp.fValid())
      if (fnObj == rp->fnObj)
        rp->GetDataFiles(afnDat, asSection, asEntry);
  }
  catch (const ErrorObject&) {
  }
}

long DomainPicture::iRaw(const String& str) const
{
  int iRed, iGreen, iBlue = shUNDEF;
  char* s = const_cast<char *>(str.scVal()); //beeeuuh; als string geen functie hier bevat maak hem dan!
  while ((*s == ' ') || (*s == '(')) ++s;
  sscanf(s, "%d,%d,%d", &iRed, &iGreen, &iBlue);
  if (iBlue == shUNDEF)
    sscanf(s, "%2x%2x%2x", &iRed, &iGreen, &iBlue);
  if (iBlue == shUNDEF)
    return iUNDEF;
  return iRaw(Color(iRed, iGreen, iBlue));
}


bool DomainPicture::fValid(const String& sValue) const
{
  long i = iRaw(sValue);
  return (i != iUNDEF);
}


