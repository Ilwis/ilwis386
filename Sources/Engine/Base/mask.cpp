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
/*
// $Log: /ILWIS 3.0/BasicDataStructures/mask.cpp $
 * 
 * 4     12-07-00 4:27p Martin
 * mask also takes codes into account
 * 
 * 3     10-01-00 14:47 Wind
 * comment
 * 
 * 2     10-01-00 10:08 Wind
 * mask did not function correct if a space followed a comma
*/
// Revision 1.3  1998/09/16 17:22:46  Wim
// 22beta2
//
// Revision 1.2  1997/08/04 12:28:00  Wim
// fInMask(iRaw) added, it takes also care of the DomainSort code:name phenomena
//
/* dat/mask.c
// Code for Masks for ILWIS 2.0
// sept. 1996, by Jelle Wind
	Last change:  WK    4 Aug 97    2:26 pm
*/
#define MASK_C
#include "Engine\Base\mask.h"
#include "Engine\Domain\dm.h"
#include "Engine\Domain\dmsort.h"

void Mask::SetMask(const String& sMsk)
{
  _sMask = sMsk;
  asMaskParts.Resize(0);
   // split mask in parts
  String s = _sMask;
  char *p = strtok(s.sVal(), ",");
  while (p) {
    asMaskParts &= String(p);
    p = strtok(0, ",");
  }
  for (unsigned int i=0; i < asMaskParts.iSize(); ++i) {
    asMaskParts[i].toLower();
    asMaskParts[i] = asMaskParts[i].sTrimSpaces();
  }
}

void Mask::SetMaskParts(const Array<String>& asMskPrts)
{
  asMaskParts.Resize(0);
   // split mask in parts
  for (unsigned int i=0; i < asMskPrts.iSize(); ++i)
    asMaskParts &= asMskPrts[i];
  for (unsigned int i=0; i < asMskPrts.iSize(); ++i) {
    asMaskParts[i].toLower();
    asMaskParts[i] = asMaskParts[i].sTrimSpaces();
  }
  _sMask = String();
  for (unsigned int i=0; i < asMaskParts.iSize(); ++i) {
    _sMask &= asMaskParts[i];
    if (i < asMaskParts.iSize()-1)
      _sMask &= ',';
  }
}

bool Mask::fInMask(const String& sValue) const
{
  String sVal = sValue;
  sVal.toLower();
  for (unsigned int i=0; i < asMaskParts.iSize(); i++) 
    if (fAcceptPart(sVal, asMaskParts[i]))
      return true;
  return false;
}

bool Mask::fAcceptPart(const String& sValue, const String& sMaskPart) const
{
	int iSz = sValue.size();
	char *sV = new char[iSz + 1];
	strcpy(sV, sValue.c_str());
	int i = -1;
	char *sCode=sV, *sName=sV;
  while( i< iSz && sV[++i] != ':' );
	if ( i < iSz)
	{
		sV[i] = 0;
		sName = sV + i + 1;
		while (*sName == ' ')
			++sName;
	}
	else
		sCode = 0;

	bool fOK = false;
	if ( sName != 0 )
		fOK = fAcceptCodeOrName(sName, sMaskPart);

	if ( !fOK && sCode != 0)
		fOK = fAcceptCodeOrName(sCode, sMaskPart);

	delete [] sV;

	return fOK;
}

bool Mask::fAcceptCodeOrName(const String& sValue, const String& sMaskPart) const
{
  bool fStar = false;
  int k = 0;
  unsigned int j=0;
  for (; (j < sMaskPart.length()) && (k < sValue.length()); ++j) {
    char c = sMaskPart[j];
    if (c == '*') {
      fStar = true;
      if (j == sMaskPart.length()-1)
        return true;
    }
    else if (c == '?') {
      fStar = false;
      k++;
    }
    else {
      if (fStar) { // find char c
        for (; k < sValue.length(); ++k)
          if (sValue[k] == c) {
            bool f = fAcceptPart(sValue.sRight(sValue.length()-k-1), String(sMaskPart.sRight(sMaskPart.length()-j-1)));
            if (f)
              return true;
          }
        return false;
      }
      if (sValue[k] != c)
        return false;
      k++;
    }
  }
  if (j == sMaskPart.length()) {
    if (k == sValue.length())
      return true;
    return sMaskPart[j-1] == '*';
  }
  for (; j < sMaskPart.length(); ++j)
    if (sMaskPart[j] != '*')
      return false;
  return true;
}

bool Mask::fInMask(long iRaw) const
{
  String sVal = dm->sValueByRaw(iRaw, 0);
  if (fInMask(sVal))
    return true;
  DomainSort* ds = dm->pdsrt();
  if (ds) {
    sVal = ds->sCodeByRaw(iRaw,0);
    if (fInMask(sVal))
      return true;
    sVal = ds->sNameByRaw(iRaw,0);
    if (fInMask(sVal))
      return true;
  }
  return false;
}







