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
/* AggregateFunctions
   Copyright Ilwis System Development ITC
   april 1996, by Jelle Wind
	Last change:  WK   28 Jan 97    9:23 am
*/

#define AGGFUNCS_C
#include "Applications\Table\AGGFUNCS.H"

static int iHash(const HashAggLL& hall) {
  word *pw = static_cast<word*>((void*)&hall.iRaw);
  word w = *pw;
  ++pw;
  for (int i = 1; i <= 1; ++i, ++pw)
    w ^= *pw;
  return w % 16000;
}

static int iHash(const HashAggRL& harl) {
  word *pw = static_cast<word*>((void*)&harl.rVal);
  word w = *pw;
  ++pw;
  for (int i = 1; i <= 3; ++i, ++pw)
    w ^= *pw;
  return w % 16000;
}

static int iHash(const HashAggLR& halr) {
  word *pw = static_cast<word*>((void*)&halr.iRaw);
  word w = *pw;
  ++pw;
  for (int i = 1; i <= 1; ++i, ++pw)
    w ^= *pw;
  return w % 16000;
}
static int iHash(const HashAggSL& hasl) {
  unsigned long h = 0;
  int i=0;  
  char *ps = const_cast<char *>(hasl.sVal.c_str());
  while (*ps)
    h = (h + (i++) * tolower(*ps++)) % 16001;
  return (int)h;  
}

static int iHash(const HashAggSR& hasr) {
  unsigned long h = 0;
  int i=0;  
  char *ps = const_cast<char *>(hasr.sVal.c_str());
  while (*ps)
    h = (h + (i++) * tolower(*ps++)) % 16001;
  return (int)h;  
}

static int iHash(const HashAggRR& harr) {
  word *pw = static_cast<word*>((void*)&harr.rVal);
  word w = *pw;
  ++pw;
  for (int i = 1; i <= 3; ++i, ++pw)
    w ^= *pw;
  return w % 16000;
}

static int iHash(const HashAggLRR& halrr) {
  word *pw = static_cast<word*>((void*)&halrr.iRaw);
  word w = *pw;
  ++pw;
  for (int i = 1; i <= 1; ++i, ++pw)
    w ^= *pw;
  return w % 16000;
}

static int iHash(const HashAggRRR& harrr) {
  word *pw = static_cast<word*>((void*)&harrr.rVal);
  word w = *pw;
  ++pw;
  for (int i = 1; i <= 3; ++i, ++pw)
    w ^= *pw;
  return w % 16000;
}

static int iHash(const HashAggSRR& hasrr) {
  unsigned long h = 0;
  int i=0;  
  char *ps = const_cast<char *>(hasrr.sVal.c_str());
  while (*ps)
    h = (h + (i++) * tolower(*ps++)) % 16001;
  return (int)h;  
}

static int iHash(const HashAggLRRR& halrrr) {
  word *pw = static_cast<word*>((void*)&halrrr.iRaw);
  word w = *pw;
  ++pw;
  for (int i = 1; i <= 1; ++i, ++pw)
    w ^= *pw;
  return w % 16000;
}

static int iHash(const HashAggRRRR& harrrr) {
  word *pw = static_cast<word*>((void*)&harrrr.rVal);
  word w = *pw;
  ++pw;
  for (int i = 1; i <= 3; ++i, ++pw)
    w ^= *pw;
  return w % 16000;
}

static int iHash(const HashAggSRRR& hasrrr) {
  unsigned long h = 0;
  int i=0;  
  char *ps = const_cast<char *>(hasrrr.sVal.c_str());
  while (*ps)
    h = (h + (i++) * tolower(*ps++)) % 16001;
  return (int)h;  
}







