/***************************************************************
 ILWIS integrates image, vector and thematic data in one unique 
 and powerful package on the desktop. ILWIS delivers a wide 
 range of feautures including import/export, digitizing, editing, 
 analysis and display of data as well as production of 
 quality mapsinformation about the sensor mounting platform
 
 Exclusive rights of use by 52�North Initiative for Geospatial 
 Open Source Software GmbH 2007, Germany

 Copyright (C) 2007 by 52�North Initiative for Geospatial
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
/* FilterPattern
   Copyright Ilwis System Development ITC
   august 1995, by Jelle Wind
	Last change:  JEL  18 Feb 97    2:38 pm
*/

#ifndef ILWFLTPATTERN_H
#define ILWFLTPATTERN_H
#include "Engine\Function\FILTER.H"

class DATEXPORT FilterPattern: public FilterPtr
{
  friend class DATEXPORT FilterPtr;
  static FilterPattern* create(const FileName&, const String& sExpr);
public:
  virtual ~FilterPattern();
  static const char* sSyntax();
  virtual void Store();
  virtual bool fRawAllowed() const;
  virtual void ExecuteRaw(const LongBufExt* bufListInp, LongBuf& bufRes);
  virtual void ExecuteVal(const RealBufExt* bufLstInp, RealBuf& bufRes);
  virtual String sName(bool fExt, const String& sDirRelative) const;
  virtual Domain dmDefault(const Map&) const;
  virtual ValueRange vrDefault(const Map&, const Domain&) const;
protected:
  FilterPattern(const FileName&);
  FilterPattern(const FileName&, double rThreshold);
  double rThreshold;
};


#endif




