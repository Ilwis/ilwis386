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
/* $Log: /ILWIS 3.0/Tools/SET.CPP $
 * 
 * 3     20-07-99 12:52 Koolhoven
 * header comment
 * 
 * 2     13/07/99 12:50 Willem
 * The atof() strtod() fucntion gave strange problems. (also during
 * linking). The functions have been renamed to be unique. They are now
 * called by their real name. The C library functions atof() and strtod()
 * are not used directly anymore
// Revision 1.2  1997/07/30 17:43:27  Wim
// Allow single colon to indicate invalid (0..-1) range,
// needed to create a valuerange with an invalid range
//
/* set.c
// Sets for ILWIS 2.0
// dec. 1993, by Jelle Wind
// (c) Computer Department ITC
	Last change:  WK   28 Aug 98    4:43 pm
*/
#define SET_C
#include "Headers\toolspch.h"
#include "Engine\Base\DataObjects\Set.h"
#include "Engine\Base\strtod.h"

RangeInt::RangeInt(const String& s) : SetBaseInt()
{
  if (":" == s) {
    iLo() = 0;
    iHi() = -1;
    return;
  }
  char *p1 = new char[s.length()+1];
  strcpy(p1, s.c_str());
  char *p2 = strchr(p1, ':');
  if (p2 != 0) {
    *p2 = 0;
    iLo() = atol(p1);
    iHi() = atol(p2+1);
  }
  else {
    iLo() = atol(p1);
    iHi() = iLo();
  }  
  delete [] p1;
//  assert (iHi() >= iLo());
}


RangeReal::RangeReal(const String& s) : SetBaseReal()
{
  if (":" == s) {
    rLo() = 0;
    rHi() = -1;
    return;
  }
  char *p1 = new char[s.length()+1];
  strcpy(p1, s.c_str());
  char *p2 = strchr(p1, ':');
  if (p2 != 0) {
    *p2 = 0;
    rLo() = atofILW(p1);
    rHi() = atofILW(p2+1);
  }
  else {
    rLo() = atofILW(p1);
    rHi() = rLo();
  }  
  delete [] p1;
//  assert (rHi() >= rLo());
}








