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
/* ColumnInfo
   Copyright Ilwis System Development ITC
   april 1997, by Jelle Wind
	Last change:  JEL  14 May 97   10:59 am
*/

#ifndef ILWCOLINFO_H
#define ILWCOLINFO_H
#include "Engine\Table\Col.h"
#include "Engine\Domain\dminfo.h"

class ColumnInfo
{
public:
    _export ColumnInfo();
    _export ColumnInfo(const String& sTblColName);
    _export ColumnInfo(const FileName& fnTbl, const String& sColName);
    _export ColumnInfo(const FileName& fnTbl, long iCol);
    _export ColumnInfo(const ColumnInfo& colinf);

    void _export            operator = (const ColumnInfo& colinf);
    void                    Read(const FileName& fnTbl, const String& sColName);
    bool _export            fValid() const;
    Domain _export          dm() const; // opens domain
    DomainValueRangeStruct  _export dvrs() const; // opens domain
    FileName                fnTbl() const;
    String _export          sName() const;
    DomainInfo _export      dminf() const;
    FileName                fnDom() const;
    DomainType _export      dmt() const;
    ValueRange _export      vr() const;
    StoreType _export       st() const;
    long                    iOffset() const;
    long                    iRecs() const;
    RangeInt                riMinMax() const;
    RangeReal _export       rrMinMax() const;
    ObjectTime              objtime() const;
    String                  sDescription() const;
    bool                    fReadOnly() const;
    bool                    fDataReadOnly() const;
    bool                    fOwnedByTable() const;
    bool                    fDependent() const;
    String                  sExpression() const;
    void                    DependencyNames(Array<String>& asNames);  // returns names of dependencies
    // in printable form (columns are preceded by the word Column)
private:
    void                    Init(const ColumnInfo& colinf);
    String                  _sName;
    FileName                _fnTbl;
    DomainInfo              _dminf;
    ValueRange              _vr;
    StoreType               _st;
    long                    _iOffset;
    long                    _iRecs;
    RangeInt                _riMinMax;
    RangeReal               _rrMinMax;
    ObjectTime              _objtime;
    String                  _sDescription;
    bool                    _fDependent;
    bool                    _fOwnedByTable;
    String                  _sExpression;
    bool                    _fReadOnly; //, _fDataReadOnly, _fPropReadOnly;
};

#endif // ILWCOLINFO_H




