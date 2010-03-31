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
/* DomainInfo
   Copyright Ilwis System Development ITC
   feb 1997, by Jelle Wind
	Last change:  JEL   9 May 97    8:24 pm
*/

#ifndef ILWDOMINFO_H
#define ILWDOMINFO_H
#include "Engine\Domain\dm.h"
#include "Engine\Base\DataObjects\valrange.h"
                  
class DomainInfo 
{
public:
  _export DomainInfo();
  _export DomainInfo(const Domain& dm);
  _export DomainInfo(const DomainInfo& dinf);
  _export DomainInfo(const FileName& fn, const char *sSection);
  void _export operator = (const DomainInfo& dinf);
  FileName _export fnDom() const;
  StoreType _export st() const;
  DomainType _export dmt() const;
  long _export iNr() const;
  ValueRange _export vr() const;
	void _export SetValueRange(const ValueRange&);
  bool _export fValues() const;
  bool _export fRealValues() const;
  bool fRawAvailable() const;
  void _export Load(const FileName& fn, const char *sSection, const char* sEntry=0);
  void _export Store(const FileName& fn, const char *sSection, const char* sEntry=0) const;
  Domain dmUnknown() const;
	Domain _export dm() const;
private:  
  FileName _fnDom;
  StoreType _st;
  DomainType _dmt;
  long _iNr;
  ValueRange _vr;
};

#endif // ILWDOMINFO_H





