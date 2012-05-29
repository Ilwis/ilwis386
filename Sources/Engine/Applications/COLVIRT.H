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
/* ColumnVirtual
   Copyright Ilwis System Development ITC
   may 1995, by Jelle Wind
	Last change:  JHE  11 Aug 97    3:15 pm
*/

#ifndef ILWCOLVIRT_H
#define ILWCOLVIRT_H
#include "Engine\Table\Col.h"
#include "Engine\Applications\objvirt.h"
#include "Engine\Base\objdepen.h"
#include "Engine\Table\COLSTORE.H"

class TableStore;

class ColumnVirtual: public IlwisObjectVirtual
{
friend ColumnPtr;
friend ColumnStore;
friend TableStore;

public:
    virtual void _export  Freeze();
    virtual void _export  UnFreeze();
    virtual bool _export  fFreezing();
    virtual String        sExpression() const;
    virtual String _export   sType() const;
    virtual void _export  Store();
    virtual bool          fDomainChangeable() const;
    virtual bool _export      fValueRangeChangeable() const;
    virtual void _export  SetDomainValueRangeStruct(const DomainValueRangeStruct&);
    virtual DomainValueRangeStruct _export dvrsDefault(const String& sExpression) const;
    virtual bool _export         fMergeDomainForCalc(Domain& dm, const String& sExpr);
    void   _export               CreateColumnStore();
    //  void DependencyNames(Array<String>& asNames) const;
    //  virtual void GetObjectDependencies(Array<FileName>& afnObjDep);
    virtual void _export  Replace(const String& sExpression);
    String                sSection() const 
                            { return ptr.sSection(); }
    Domain                dm() const 
                            { return ptr.dm(); }
    const ValueRange&     vr() const  
                            { return ptr.vr(); }
    const DomainValueRangeStruct&   dvrs() const 
                            { return ptr.dvrs(); }
    StoreType             st() const
                            { return ptr.st(); }
    long                  iRecs() const 
                            { return ptr.iRecs(); }
    long                  iOffset() const 
                            { return ptr.iOffset(); }
    bool                  WriteEntry(const char* sEntry, const String& sValue)
                            { return  ptr.WriteEntry(sEntry, sValue); }
    bool                  WriteEntry(const char* sEntry, const char* sValue)
                            { return  ptr.WriteEntry(sEntry, sValue); }
    bool                  WriteEntry(const char* sEntry, bool fValue)
                            { return  ptr.WriteEntry(sEntry, fValue); }
    bool                  WriteEntry(const char* sEntry, long iValue)
                            { return  ptr.WriteEntry(sEntry, iValue); }
    bool                  WriteEntry(const char* sEntry, double rValue)
                            { return  ptr.WriteEntry(sEntry, rValue); }

    FileName              fnTblSearch;
    ColumnPtr&            ptr; // the ColumnPtr of which this is a member
    ColumnStore*&         pcs; // the ColumnStore for the calculated result

protected:
    _export ColumnVirtual(const Table& tbl, const String& sColName, ColumnPtr&);
    _export ColumnVirtual(const Table& tbl, const String& sColName, ColumnPtr&,
                const DomainValueRangeStruct& dvs, const Table& tblSearch);
    _export ~ColumnVirtual();
    virtual long _export         iComputeRaw(long iKey) const;
    virtual long _export  iComputeVal(long iKey) const;
    virtual double _export       rComputeVal(long iKey) const;
    virtual Coord  _export    cComputeVal(long iKey) const;
	virtual ILWIS::Time  _export    tComputeVal(long iKey) const;
    virtual String _export    sComputeVal(long iKey, short iWidth=-1, short iDec=-1) const;
    virtual void _export  ComputeBufRaw(ByteBuf&, long iKey, long iNum) const;
    virtual void _export  ComputeBufRaw(IntBuf& , long iKey, long iNum) const;
    virtual void _export  ComputeBufRaw(LongBuf&, long iKey, long iNum) const;
    virtual void _export  ComputeBufVal(LongBuf&, long iKey, long iNum) const;
    virtual void _export  ComputeBufVal(RealBuf&, long iKey, long iNum) const;
	virtual void _export  ComputeBufVal(Buf<ILWIS::Time>&, long iKey, long iNum) const;
    virtual void _export  ComputeBufVal(StringBuf&, long iKey, long iNum, short iWid=-1, short iDec=-1) const;
    virtual void _export  ComputeBufVal(CoordBuf&, long iKey, long iNum) const;
    static String _export sTblSearch(String& sExpression);
private:
    static ColumnVirtual* create(const Table& tbl, const String& sColName, ColumnPtr&);
    static ColumnVirtual* create(const Table& tbl, const String& sColName, ColumnPtr&,
                                 const String& sExpression, const DomainValueRangeStruct& dvs);
    virtual bool _export      fInit();

};

#endif // ILWCOLVIRT_H




