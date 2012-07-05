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

class _export TableStore//: public TablePtr
{
	friend class DATEXPORT TableImport;
	friend class TableView;
	friend class TablePtr;
	
public:
    virtual        ~TableStore();
	
    virtual void   Store();
    void                  Export(const FileName& fn) const;
    void           SetErase(bool f=true);
    void                  GetDataFiles(Array<FileName>& afnDat, Array<String>* asSection=0, Array<String>* asEntry=0) const;
    void                  GetObjectDependencies(Array<FileName>& afnObjDep);
    static void    UnStore(const FileName& fnObj);
    const Column   &col(int) const;
    const Column   &col(const String&) const;
    virtual long           iCol(const String& sName) const;
    virtual void   DeleteRec(long iStartRec, long iRecs=1);
    virtual long   iRecNew(long iRecs = 1);
    const Column   &colNew(const String&, const Domain&, const ValueRange& vr);
    const Column   &colNew(const String&, const DomainValueRangeStruct& dvs);
    virtual const Column& AddCol(const Column&);
    virtual void   RemoveCol(Column&);
    virtual void   LoadBinary();
    virtual void   StoreAsBinary(const FileName& fnDat);
    void                  StoreAsTBL(const FileName& fnDat);
    long           iRecNewWithoutRecsUpdate(long iRecs);
    virtual bool   fWriteAvailable() const;
    virtual void   DoNotUpdate();
    void                  Loaded(bool fValue);
    void           DoNotStore(bool fValue);
	virtual void   PutStringInForeign(const String& sCol, long iRec, const String& str){}
	virtual void   PutByteInForeign(const String& sCol, long iRec, char str){}
	virtual void   PutUByteInForeign(const String& sCol, long iRec, unsigned char bV){}
	virtual void   PutULongInForeign(const String& sCol, long iRec, unsigned long iV){}
	virtual void   PutLongInForeign(const String& sCol, long iRec, long iV){}	
	virtual void   PutRealInForeign(const String& sCol, long iRec, double rV){}		
	virtual void   PutBoolInForeign(const String& sCol, long iRec, bool fV){}
	virtual void   PutCoordInForeign(const String& sCol, long iRec, Coord cV){}

	bool           fIsLoaded();
	
protected:
	 TableStore();
     TableStore(const FileName&, TablePtr& p/*, const String& sSecPrf*/);
     TableStore(const FileName&, TablePtr& p, const FileName&);
	
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
	bool loaderOnly; // this tablestore isnt part of a normal table; it is used as a loader and doesnt need to store anything
	
private:
    ObjectTime                  timStore; // time of last change in data
    bool                  fErase;
	bool                  fLoaded;
	Table				  tblDummy; //
};

#endif // ILWTBLSTORE_H





