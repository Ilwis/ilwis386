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
// $Log: TBLCHDOM.H $
// Revision 1.1  1998/09/16 17:25:32  Wim
// 22beta2
//
/* TableChangeDomain
   Copyright Ilwis System Development ITC
   original by Martin Schouwenburg, may 1998
   extended by Wim Koolhoven, may 1998
	Last change:  WK   29 May 98    3:41 pm
*/
#include "Engine\Applications\TBLVIRT.H"

IlwisObjectPtr * createTableChangeDomain(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms );

class DATEXPORT TableChangeDomain: public TableVirtual
{
  enum enumAggregate { eNO, eAVG, eSUM, eMIN, eMAX, eLAST };
public:
  TableChangeDomain(const FileName &fName, TablePtr& p, const Table& table, const Domain& dom);
  TableChangeDomain(const FileName &fName, TablePtr& p, const Table& tab, const Column& col, enumAggregate eAgg);
  TableChangeDomain(const FileName&, TablePtr&);
  static TableChangeDomain* create(const FileName& fn, TablePtr& p, const String& sExpression);

  static const char* sSyntax();
  virtual void Store();
  virtual void UnFreeze();
  virtual bool fFreezing();
  virtual String sExpression() const;

protected:
  void Init();
  static void CheckColumn(const Column& col, bool fAgg);
  long iInd(DomainSort* pdsrt, long iKey);

private:
  Domain domNew;
  Table tblSource;
  Column colSource;
  enumAggregate eAggregate;
  Column colCount;
};





