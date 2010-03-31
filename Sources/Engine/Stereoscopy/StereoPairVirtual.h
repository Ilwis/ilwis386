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
// StereoPairVirtual.h: interface for the StereoPairVirtual class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STEREOPAIRVIRTUAL_H__374B5E86_87EE_40A6_AD44_3AEEB6A1019F__INCLUDED_)
#define AFX_STEREOPAIRVIRTUAL_H__374B5E86_87EE_40A6_AD44_3AEEB6A1019F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef ILWOBJECTVIRT_H
#include "Engine\Applications\objvirt.h"
#endif

#if !defined(AFX_STEREOPAIR_H__B57B6765_CE6C_40E4_AFC6_0D8657C9968C__INCLUDED_)
#include "Engine\Stereoscopy\StereoPair.h"
#endif


class StereoPairVirtual : public IlwisObjectVirtual   
{
  friend class StereoPairPtr;
public:
  virtual void Freeze();
  virtual void UnFreeze();
  virtual bool fFreezing();
  virtual String sExpression() const;
protected:
  StereoPairVirtual(const FileName&, StereoPairPtr&);
	StereoPairVirtual(const FileName& fn, StereoPairPtr& p, bool fCreate);
	StereoPairVirtual(const FileName&, StereoPairPtr&, const Map& mpL, const Map& mpR);
  static StereoPairVirtual _export *create(const FileName&, StereoPairPtr&);
  static StereoPairVirtual _export *create(const FileName&, StereoPairPtr&, const String& sExpression);
	static StereoPairVirtual _export *create(const FileName&, StereoPairPtr&, 
																					const Map& mp1, const Map& mp2);
  virtual ~StereoPairVirtual();
  virtual void Store();

  StereoPairPtr& ptr;
};

#endif // !defined(AFX_STEREOPAIRVIRTUAL_H__374B5E86_87EE_40A6_AD44_3AEEB6A1019F__INCLUDED_)
