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
/* DomainGroup
   Copyright Ilwis System Development ITC
   december 1995, by Wim Koolhoven
	Last change:  JEL   7 May 97   10:09 am
*/

#ifndef ILWDOMGROUP_H
#define ILWDOMGROUP_H
#include "Engine\Domain\dmclass.h"

#undef IMPEXP
#ifdef ILWISENGINE
#define IMPEXP __declspec(dllexport)
#else
#define IMPEXP __declspec(dllimport)
#endif


class IMPEXP DomainGroup: public DomainClass
{
  friend class Domain;
  friend class DomainPtr;
protected:
  DomainGroup(const FileName&);
  DomainGroup(const FileName&, long iNr);
  ~DomainGroup();
public:
  virtual void        Store();
  virtual String      sType() const;
  long                iClassify(double rVal) const;
  void                SetUpperBound(long iRaw, double rVal);
  double              rLower(long iRaw) const;
  double              rUpper(long iRaw) const;
  RangeReal           rr(long iRaw) const;
  virtual void        Merge(const DomainSort* pdsrt,  Tranquilizer* trq=NULL);
	virtual void        Delete(long raw);
  virtual bool        fEqual(const IlwisObjectPtr& ptr) const;

private:
  void Init();
  void Sort();
  Column colBnd;
  struct clsbnd {
    long cls;
    double bnd;
  };
  clsbnd* buf;
  friend static int sortClsBnd(const void* a, const void* b);
};

#endif // ILWDOMGROUP_H
