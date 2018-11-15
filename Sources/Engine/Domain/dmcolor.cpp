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
/* DomainColor
   Copyright Ilwis System Development ITC
   may 1995, by Wim Koolhoven
	Last change:  JEL  30 Dec 96    9:55 pm
*/

#include "Engine\Domain\dmcolor.h"

DomainColor::DomainColor()
: DomainPtr(FileName(""))
{
  _iWidth = 13;
}

void DomainColor::Store()
{}

bool DomainColor::fEqual(const IlwisObjectPtr& ptr) const
{
  return 0 != dynamic_cast<const DomainColor*>(&ptr);
}

String DomainColor::sName(bool fExt, const String&) const
{
	if (fExt)
	  return "Color.dom";
	else
		return "Color";
}

StoreType DomainColor::stNeeded() const
{ return stLONG; }

String DomainColor::sValueByRaw(long iRaw, short iWidth, short) const
{
  // future perhaps with real names for some known colors.
  // need a special table for that.
  Color clr = Color(iRaw);
  String s;
  if (iRaw == iUNDEF) {
    s = String("?");
    if (iWidth == 0)
      return s;
  }
  else if (iWidth <= 0 || iWidth >= 13) {
    s = String("(%3i,%3i,%3i)", clr.red(), clr.green(), clr.blue());
    if (iWidth == 0)
      return s;
  }
  else if (iWidth >= 11)
    s = String("%3i,%3i,%3i", clr.red(), clr.green(), clr.blue());
  else if (iWidth >= 6)
    s = String("%2.2X%2.2X%2.2X", clr.red(), clr.green(), clr.blue());
  else
    s = String('*', iWidth);
  return String("%*S", iWidth, s);
}

long DomainColor::iRaw(const String& str) const
{
  int iRed, iGreen, iBlue = shUNDEF;
  char* s = const_cast<char *>(str.c_str()); //beeeuuh; als string geen functie hier bevat maak hem dan!
  while ((*s == ' ') || (*s == '(')) ++s;
  sscanf(s, "%d,%d,%d", &iRed, &iGreen, &iBlue);
  if (iBlue == shUNDEF)
    sscanf(s, "%2x%2x%2x", &iRed, &iGreen, &iBlue);
  if (iBlue == shUNDEF)
    return iUNDEF;
  Color clr(iRed,iGreen, iBlue);
  return long(clr);
}

bool DomainColor::fValid(const String& s) const
{
  long i = iRaw(s);
  return (i != iUNDEF);
}





