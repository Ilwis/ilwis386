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
/* $Log: /ILWIS 3.0/ZappInterface/REALFRMT.cpp $
 * 
 * 4     5-07-02 19:02 Koolhoven
 * prevent warnings in Visual Studio .Net
 * 
 * 3     22-12-99 8:58a Martin
 * //->/*
 
 * 2     20-12-99 11:57 Wind
 * forgotten stuff from port of 2.23 code*/
// Revision 1.4  1998/09/16 17:37:53  Wim
// 22beta2
//
// Revision 1.3  1997/09/13 14:37:16  Wim
// Added getChars() to translate string which start with an 'e' to a valid number
//
// Revision 1.2  1997-09-13 16:02:57+02  Wim
// Should now have all requested functionality
//
// Revision 1.1  1997-09-13 14:53:57+02  Wim
// Initial revision
//
// Real Formatter,
// to format a DoubleEdit
// replacement for zNumFormatter

// make first implementation simple and not dependent on anything
// not even the value range it should allow.

// format: [-][(0-9)][.][(0-9)][e[-](0-9)]
// if the 'e' is used both before and after at least one digit is needed

#include "Client\Base\REALFRMT.H"


RealFormatter::RealFormatter()
: zFormatter(0)
{
}

bool RealFormatter::fAdd(const char& c)
{
  if (isdigit((unsigned char)c)) {
    if (iExp > 0) {
      if (i > iExp + 4)
        return false;
      if (i == iExp + 1 && c == '0')
        return false;
    }
    return true;
  }
  else if (c == '-') {
    if (i == 0 || i == iExp + 1)
      return true;
  }
  else if (iExp < 0) {
    if (c == 'E')
    {
      const_cast<char&>(c)='e';
    } 
    else if (c == ',')
      const_cast<char&>(c) = '.';
    if (c == 'e') {
      iExp = i;
      return true;
    }
    else if (iDot < 0 && c == '.') {
      iDot = i;
      return true;
    }
  }
  return false;
}

int RealFormatter::getFracDigits()
{
  return 10; // zoek dit uit!!
}

zFormatter::zFormatterStat RealFormatter::addChars(String &str,const char *c, int count, zRange &pos)
{
  char* sBuf = new char[str.length() + count + 1];
  char* s = str.sVal();
  char *srcEnd = s + str.length();
  i = 0;

  while (i < pos.lo() && (unsigned)i < str.length())
    sBuf[i++] = *s++;
  if (pos.hi() == pos.lo()) {
    if ((*c == '\b' && count == 1) && i > 0)
      i--;
    else if ((*c == '\b' && count == 2) && (unsigned)i < str.length())
      s++;
  }
  else
    s = str.sVal() + pos.hi();
  if (*c == '\b')
    count = 0;

  iDot = -1;
  iExp = -1;
  for (int j = 0; j < i; ++j)
    if (sBuf[j] == '.')
      iDot = j;
    else if (sBuf[j] == 'e')
      iExp = j;
  for (int j = 0; j < count; ++j, ++c)
    if (fAdd(*c))
      sBuf[i++] = *c;
  pos.lo() = pos.hi() = i;
  for (; s < srcEnd && *s; ++s)
    if (fAdd(*s))
      sBuf[i++] = *s;
  sBuf[i] = '\0';
  str = sBuf;
  delete [] sBuf;
  sBuf = 0;
  return zFmtStatReplace;
}

bool RealFormatter::checkChars(String &str, zRange &pos)
{
  int i = 0;
  int iLen = str.length();
  if (str[i] == '-') i++;
  while (i < iLen && isdigit((unsigned char)str[i])) i++;
  if (str[i] == '.') i++;
  while (i < iLen && isdigit((unsigned char)str[i])) i++;
  if (str[i] == 'e') {
    if (i > 0 && !isdigit((unsigned char)str[i]-1)) {
      i++;
      if (str[i] == '-') i++;
      if (i == iLen) {
        pos.lo() = pos.hi() = i;
        return 0;
      }
      if (i < iLen && isdigit((unsigned char)str[i])) i++;
      if (i < iLen && isdigit((unsigned char)str[i])) i++;
      if (i < iLen && isdigit((unsigned char)str[i])) i++;
    }
  }
  if (i < iLen) {
    pos.lo() = pos.hi() = i;
    return false;
  }
  return true; // all ok
}

int RealFormatter::getChars(String &str, unsigned long flags)
{
  int i = 0;
  int iLen = str.length();
  if (str[i] == '-') i++;
  if (str[i] == '.') i++;
  if (str[i] == 'e') {
    char* sBuf = new char[str.length() + 2];
    int j = 0;
    if (str[0] == '-')
      sBuf[j++] = '-';
    sBuf[j++] = '1';
    while (i < iLen)
      sBuf[j++] = str[i++];
    sBuf[j] = '\0';
    str = sBuf;
    delete [] sBuf;
  }
  return 1;
}





