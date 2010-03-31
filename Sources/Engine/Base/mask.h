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
/* $Log: /ILWIS 3.0/BasicDataStructures/mask.h $
 * 
 * 4     13-07-00 2:00p Martin
 * comment bug
 /* 
 * 2     13-07-00 10:12a Martin
 * added function to check names or codes
// Revision 1.5  1998/09/16 17:22:46  Wim
// 22beta2
//
// Revision 1.4  1997/08/04 13:20:52  Wim
// Typing error
//
// Revision 1.3  1997-08-04 14:31:55+02  Wim
// Added ways to set a domain in the Mask
//
// Revision 1.2  1997-08-04 14:27:04+02  Wim
// Domain is now a member of Mask to enable the fInMask(iRaw) function,
// it takes care of code:name values in DomainSort also.
//
/* dat/mask.h
// Interface for Masks for ILWIS 2.0
// sept. 1996, by Jelle Wind
	Last change:  WK    4 Aug 97    3:20 pm
*/
#ifndef ILWMASK_H
#define ILWMASK_H

#include "Headers\toolspch.h"
#include "Engine\Domain\dm.h"

class DATEXPORT Mask
{
public:
  Mask() {}
  Mask(const Domain& dom): dm(dom) {}
  Mask(const Domain& dom, const String& sMsk): dm(dom) { SetMask(sMsk); }
  Mask(const String& sMsk) { SetMask(sMsk); }
  ~Mask() {}
  void SetDomain(const Domain& dom) { dm = dom; }
  void _export SetMask(const String& sMsk);
  bool _export fInMask(const String& sValue) const;
  bool _export fInMask(long iRaw) const;
  String sMask() const  { return _sMask; } 
  String _export sMaskPart(int i) const { return asMaskParts[i]; } 
  int iMaskParts() const { return asMaskParts.iSize(); } 
  void _export SetMaskParts(const Array<String>& asMskPrts);
private:
  bool fAcceptPart(const String& sValue, const String& sMaskPart) const;
	bool fAcceptCodeOrName(const String& sValue, const String& sMaskPart) const;
  String _sMask;
  Array<String> asMaskParts;
  Domain dm;
};

#endif





