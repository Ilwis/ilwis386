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
#ifndef ILWTABLEGLUE_H
#define ILWTABLEGLUE_H

#include "Engine\Applications\TBLVIRT.H"
#include "Engine\Domain\dmsort.h"
#include "Engine\Domain\Dmvalue.h"
#include "Engine\Domain\dmnone.h"
#include "Headers\Err\Ilwisapp.err"

IlwisObjectPtr * createTableGlue(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms );


class DATEXPORT TableGlue: public TableVirtual
{
  enum tmDomainCombinations
    { tmAllNone, tmAllId, tmAllClass, tmAllSame, tmAllUniqueID, tmUnknown };

	private:

     Array<Table> tableAr;
        

	public:
    TableGlue(const FileName _fileName, TablePtr& p, Domain& dom, const Array<Table>& _tabs);
    TableGlue(const FileName _fileName, TablePtr& p, const Array<Table>& _tabs, bool fUn=false);
    TableGlue(const FileName& fName, TablePtr &tab);
    static TableGlue*           create(const FileName& fn, TablePtr& p, const String& sExpression);

    void                        Store();
    bool                        fFreezing();
    String                      sExpression() const;
    static const char*          sSyntax();
   
	protected:        
    bool                        fMakeRecodeTable(DomainSort *pdsrt2, ArrayLarge<long>& aiRecode, long& iStartRecNo);
    bool                        fMergeTable(Table& SourceTable, ArrayLarge<long>& aiRecode, long& iInsertRecNo);
    Column                      NewColumn(const Column& col);
    tmDomainCombinations        CheckDomain();
    ValueRange                  MergeColumnValueRanges(const Column& col1, const Column& col2);

    tmDomainCombinations tmComb;
    bool fUnder;
		bool fTablesHaveSameDom;
};  

/*#pragma option -Jgd
static Array<Table> dummyTabAr;
#pragma option -Jgx*/
#endif







