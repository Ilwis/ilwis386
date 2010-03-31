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
/* Filter, FilterPtr
   Copyright Ilwis System Development ITC
   june 1995, by Jelle Wind
	Last change:  JEL  18 Feb 97    2:37 pm
*/

#ifndef ILWFILTER_H
#define ILWFILTER_H
#include "Engine\Function\FUNCTION.H"
#include "Engine\Base\DataObjects\ERR.H"

class DATEXPORT FilterPtr: public FunctionPtr
{
  friend class DATEXPORT Filter;
  friend class DATEXPORT FunctionPtr;
  static FilterPtr* create(const FileName&);
  static FilterPtr* create(const FileName&, const String& sExpression);
public:
  virtual ~FilterPtr();
  short iRows() const { return iFltRows; }
  short iCols() const { return iFltCols; }
  virtual void Store();
  virtual String sType() const;
  virtual bool fRawAllowed() const;
//  virtual void ExecuteRaw(const LongBufExtList& bufListInp, LongBuf& bufRes)=0;
//  virtual void ExecuteVal(const RealBufExtList& bufLstInp, RealBuf& bufRes)=0;
  virtual void ExecuteRaw(const LongBufExt* bufListInp, ByteBuf& bufRes);
  virtual void ExecuteRaw(const LongBufExt* bufListInp, IntBuf& bufRes);
  virtual void ExecuteRaw(const LongBufExt* bufListInp, LongBuf& bufRes)=0;
  virtual void ExecuteVal(const RealBufExt* bufListInp, LongBuf& bufRes);
  virtual void ExecuteVal(const RealBufExt* bufListInp, RealBuf& bufRes)=0;
  virtual Domain dmDefault(const Map&) const;
  virtual ValueRange vrDefault(const Map&, const Domain&) const;
protected:
  FilterPtr(const FileName&);
  FilterPtr(const FileName&, short iRows, short iCols,
            const Domain& dmDefault = Domain());
  short iFltRows, iFltCols;
};

class  Filter: public Function
{
protected:
  Filter(IlwisObjectPtr* ptr);
public:
  _export Filter();
  _export Filter(const FileName& filename);
  _export Filter(const Filter& flt);
  _export Filter(const FileName& filename, const String& sExpression);
  _export Filter(const String& sExpression);
  _export Filter(const String& sExpression, const String& sPath);
  Filter(const char* sExpression);
  void operator = (const Filter& flt) { SetPointer(flt.pointer()); }
  FilterPtr* ptr() const { return static_cast<FilterPtr*>(pointer()); }
  FilterPtr* operator -> () const { return ptr(); }
  static void ErrorThreshold(double rThres, const String& sObjName);
};

#endif




