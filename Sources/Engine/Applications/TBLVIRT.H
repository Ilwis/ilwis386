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
/* TableVirtual
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  JEL  15 May 97    1:54 pm
*/

#ifndef ILWTBLVIRT_H
#define ILWTBLVIRT_H
#include "Engine\Table\tbl.h"
#include "Engine\Applications\objvirt.h"

class _export TableVirtual : public IlwisObjectVirtual
{
  friend class TablePtr;
  friend class ObjectDependency;
protected:
  TableVirtual(const FileName&, TablePtr&, bool fCreate=false);
//  TableVirtual(const FileName&, TablePtr&, const Domain&);
  static TableVirtual *create(const FileName&, TablePtr&);
  static TableVirtual *create(const FileName&, TablePtr&, const String& sExpression);
  ~TableVirtual();
  virtual void Store();
public:
  virtual void Freeze();
  virtual void UnFreeze();
  virtual bool fFreezing();
  virtual String sExpression() const;
  virtual String sType() const;
  void CreateTableStore();
//  virtual bool fAdditionalInfo() const;
//  virtual String sAdditionalInfo() const;
  long iRecs() const { return ptr.iRecs(); }
  long iOffset() const { return ptr.iOffset(); }
  long iCols() const { return ptr.iCols(); }
  Domain dm() const { return ptr.dm(); }
protected:
  TablePtr& ptr; // the TablePtr of which this is a member
  TableStore*& pts; // the TableStore for the calculated result
};

#endif




