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
/* TableInfo
   Copyright Ilwis System Development ITC
   may 1997, by Jelle Wind
	Last change:  JEL   2 Jun 97    3:44 pm
*/

#ifndef ILWTBLINFO_H
#define ILWTBLINFO_H
#include "Engine\Table\tbl.h"
#include "Engine\Table\COLINFO.H"

class TableInfo
{
public:
  _export TableInfo();
  _export TableInfo(const FileName& fnTbl);
  FileName _export fnTbl() const;
  Domain _export dm() const; // opens domain
  DomainInfo _export dminf() const;
  FileName _export fnDom() const;
  DomainType _export dmt() const;
  long _export iCols() const;
  long iOffset() const;
  long iRecs() const;
  Time objtime() const;
  String sDescription() const;
  bool fReadOnly() const;
  bool fDataReadOnly() const;
//  bool fPropReadOnly() const;
  bool fDependent() const;
  String sExpression() const;
  String sType() const;
  void DependencyNames(Array<String>& asNames);  // returns names of dependencies
  // in printable form (columns are preceded by the word Column)
  Array<ColumnInfo> aci;
  static bool _export fExistCol(const FileName& fnTbl, const String& sSectionPrefix, const String& sColName);
private:
  void Read(const FileName& fnTbl);
  FileName _fnTbl;
  long _iCols, _iRecs, _iOffset;
  DomainInfo _dminf;
  Time _objtime;
  String _sDescription;
  bool _fDependent;
  String _sExpression;
  bool _fReadOnly;//, _fDataReadOnly, _fPropReadOnly;
  String _sType;
};

#endif // ILWTBLINFO_H





