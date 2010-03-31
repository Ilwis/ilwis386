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
// $Log: BMAPVIRT.C $
// Revision 1.3  1998/09/16 17:22:46  Wim
// 22beta2
//
// Revision 1.2  1997/08/25 17:50:13  Wim
// Added (empty) DeleteInternals() to BaseMapVirtual
// to call in the UnFreeze() routine.
//
/* BaseMapVirtual
   Copyright Ilwis System Development ITC
   april 1997, by Jelle Wind
	Last change:  WK   25 Aug 97    5:17 pm
*/
#include "Engine\Applications\bmapvirt.h"

BaseMapVirtual::BaseMapVirtual(const FileName& fn, BaseMapPtr& _ptr)
: IlwisObjectVirtual(fn, _ptr, _ptr.objdep, false), bptr(_ptr)
{
}

BaseMapVirtual::BaseMapVirtual(const FileName& fn, BaseMapPtr& _ptr, const CoordSystem& cs,
                             const CoordBounds& cb, const DomainValueRangeStruct& dvrs)
: IlwisObjectVirtual(fn, _ptr, _ptr.objdep, true), bptr(_ptr)
{
  bptr._csys = cs;
  bptr.cbOuter = cb;
  bptr.dvs = dvrs;
}

void BaseMapVirtual::DeleteInternals()
{
/*
  Domain dom = dm();
  if (dom.fValid() && dom->fnObj == bptr.fnObj) {
    dom->fErase = true;
    Representation rpr = dom->rpr();
    if (rpr.fValid() && rpr->fnObj == bptr.fnObj)
      rpr->fErase = true;
    bptr.dvs = Domain();
  }
*/
}






