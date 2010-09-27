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
/* TableStore
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  WK   28 Apr 98   12:29 pm
*/

#ifndef ILWTBLFILE_H
#define ILWTBLFILE_H
#include "Headers\toolspch.h"
#include "Engine\Table\tbl.h"
#include "Engine\Table\Col.h"
#include "Engine\Table\COLINFO.H"

class TableStore//: public TablePtr
{
	friend class DATEXPORT TableImport;
	friend class TableView;
	friend class TablePtr;
	
public:
    virtual _export       ~TableStore();
	
    virtual void _export  Store();
    void                  Export(const FileName& fn) const;
    void _export          SetErase(bool f=true);
    void                  GetDataFiles(Array<FileName>& afnDat, Array<String>* asSection=0, Array<String>* asEntry=0) const;
    void                  GetObjectDependencies(Array<FileName>& afnObjDep);
    static void _export   UnStore(const FileName& fnObj);
    const Column _export  &col(int) const;
    const Column _export  &col(const String&) const;
    long                  iCol(const String& sName) const;
    void                  DeleteRec(long iStartRec, long iRecs=1);
    long _export          iRecNew(long iRecs = 1);
    const Column _export  &colNew(const String&, const Domain&, const ValueRange& vr);
    const Column _export  &colNew(const String&, const DomainValueRangeStruct& dvs);
    const Column&         AddCol(const Column&);
    void _export          RemoveCol(Column&);
    virtual void _export  LoadBinary();
    void _export          StoreAsBinary(const FileName& fnDat);
    void                  StoreAsTBL(const FileName& fnDat);
    long _export          iRecNewWithoutRecsUpdate(long iRecs);
    virtual bool _export  fWriteAvailable() const;
    virtual void _export  DoNotUpdate();
    void                  Loaded(bool fValue);
    void _export          DoNotStore(bool fValue);
	virtual void _export  PutStringInForeign(const String& sCol, long iRec, const String& str){}
	virtual void _export  PutByteInForeign(const String& sCol, long iRec, char str){}
	virtual void _export  PutUByteInForeign(const String& sCol, long iRec, unsigned char bV){}
	virtual void _export  PutULongInForeign(const String& sCol, long iRec, unsigned long iV){}
	virtual void _export  PutLongInForeign(const String& sCol, long iRec, long iV){}	
	virtual void _export  PutRealInForeign(const String& sCol, long iRec, double rV){}		
	virtual void _export  PutBoolInForeign(const String& sCol, long iRec, bool fV){}
	virtual void _export  PutCoordInForeign(const String& sCol, long iRec, Coord cV){}

	bool _export          fIsLoaded();
	
protected:
    _export TableStore(const FileName&, TablePtr& p/*, const String& sSecPrf*/);
    _export TableStore(const FileName&, TablePtr& p, const FileName&);
	
	void                  StoreColNames();
    const FileName&       fnData() const
	                          { return ptr.fnDat; }
	void                  SetDataFile(const FileName& fn);
    long                  iRecs() const 
	                          { return ptr.iRecs(); }
    long                  iOffset() const 
	                          { return ptr.iOffset(); }
    long                  iCols() const 
	                          { return ptr.iCols(); }
    TablePtr&             ptr;  // the TablePtr of which this is a member
    Array<Column>         ac;
	
private:
    ObjectTime                  timStore; // time of last change in data
    bool                  fErase;
	bool                  fLoaded;
};

#endif // ILWTBLSTORE_H





