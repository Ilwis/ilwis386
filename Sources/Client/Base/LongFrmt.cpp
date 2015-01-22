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
#include "Client\Base\LongFrmt.h"

LongFormatter::LongFormatter(int iW) 
: zFormatter(0)
{
	iWidth = iW;
}

bool LongFormatter::fAdd(const char& c)
{
	if (i >= iWidth)
		return false;
  if (isdigit((unsigned char)c))
     return true;
  else if (c == '-') 
  {
    if (i == 0 )
      return true;
  }
  return false;
}

zFormatter::zFormatterStat LongFormatter::addChars(String &str, const char *c, int count, zRange &pos)
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

bool LongFormatter::checkChars(String &str, zRange &pos)
{
  int i = 0;
  int iLen = str.length();
  if (str[i] == '-') i++;
  while (i < iLen && isdigit((unsigned char)str[i])) i++;
  if (i < iLen) 
  {
    pos.lo() = pos.hi() = i;
    return false;
  }
  return true; // all ok
}

int LongFormatter::getChars(String &str, unsigned long )
{
  int i = 0;
  int iLen = str.length();
  if (str[i] == '-') i++;
  return 1;
}




