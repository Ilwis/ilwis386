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
// StereoPair.h: interface for the StereoPair class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STEREOPAIR_H__B57B6765_CE6C_40E4_AFC6_0D8657C9968C__INCLUDED_)
#define AFX_STEREOPAIR_H__B57B6765_CE6C_40E4_AFC6_0D8657C9968C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef ILWMAP_H
#include "Engine\Map\Raster\Map.h"
#endif

class StereoPairVirtual;


class StereoPairPtr: public IlwisObjectPtr
{
  friend class StereoPair;
  friend class StereoPairVirtual;
public:
	virtual String sType() const;
	virtual void Store();
  virtual bool _export fDependent() const; 
  virtual bool _export fCalculated() const; 
  virtual void _export Calc(bool fMakeUpToDate=false); 
  virtual void _export DeleteCalc(); 
  virtual void _export BreakDependency(); 
  virtual _export String sExpression() const;
	virtual IlwisObjectVirtual *pGetVirtualObject() const;
  Map mapLeft, mapRight;
	virtual void _export GetObjectStructure(ObjectStructure& os);
	StereoPairVirtual _export *pStereoPairVirtual() const;
	const _export GeoRef& gr() const;
	bool fUseGeorefLeft;
	void _export      CreatePyramidLayers();
	void _export      DeletePyramidFiles();
	bool _export      fHasPyramidFiles();
private:
  StereoPairPtr(const FileName& fn, bool fOpenExisting);
  StereoPairPtr(const FileName& fn, const String& sExpression);
	StereoPairPtr(const FileName& fn, const Map& mpL, const Map& mpR);
  ~StereoPairPtr();
  static StereoPairPtr* create(const FileName&, bool fOpenExisting);
  static StereoPairPtr* create(const FileName&, const String& sExpression);
	static StereoPairPtr* create(const FileName&, const Map& mpL, const Map& mpR);
  void OpenStereoPairVirtual(); 
  ObjectDependency objdep;
  StereoPairVirtual* pspv;
};

class StereoPair: public IlwisObject  
{
public:
	_export StereoPair();
	_export StereoPair(const StereoPair&);
	_export StereoPair(const FileName&, bool fOpenExisting = true);
	_export StereoPair(const FileName&, const String& sExpression);
	_export StereoPair(const FileName&, const Map& mpL, const Map& mpR);

  void operator = (const StereoPair& sp) { SetPointer(sp.pointer()); }
  StereoPairPtr* ptr() const { return static_cast<StereoPairPtr*>(pointer()); }
  StereoPairPtr* operator -> () const { return ptr(); }
  static StereoPairPtr* pGet(const FileName& fn);
private:
  static IlwisObjectPtrList listStereoPair;
};

#endif // !defined(AFX_STEREOPAIR_H__B57B6765_CE6C_40E4_AFC6_0D8657C9968C__INCLUDED_)
