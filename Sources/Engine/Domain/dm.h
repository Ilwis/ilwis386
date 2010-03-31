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
/* $Log: /ILWIS 3.0/BasicDataStructures/dm.h $
 * 
 * 17    12/14/01 16:25 Willem
 * - Added constructor to create a new bool domain
 * - Added a function to remove duplicate code (for creation of
 * Representation)
 * - Chnaged some code that still used char*
 * 
 * 16    23/11/00 10:42 Willem
 * Added stFOREIGNFORMAT StoreType
 * 
 * 15    23-10-00 4:09p Martin
 * added getobjectstructure function
 * 
 * 14    19/05/00 17:45 Willem
 * sDomainType is now exported
 * 
 * 13    19/05/00 17:09 Willem
 * Removed duplicate copy of sDomainType
 * 
 * 12    19/05/00 17:05 Willem
 * Renamed DomainTypeAsString to sDomainType
 * 
 * 11    19/05/00 14:00 Willem
 * Added static member function Domain::DomainTypeAsString() returning the
 * string representation of the DomainType.
 * 
 * 10    21-02-00 4:35p Martin
 * Added a function to quickly add a whole set of values (AddValues)
 * 
 * 9     10/02/00 16:20 Willem
 * Updated rpr() and rprLoad() member functions. The fShowOnError
 * parameter is now only valid for rprLoad().
 * 
 * 8     10/02/00 12:14 Willem
 * Added default parameter fShowOnError to rpr() and rprLoad() member
 * functions. When the flag is false no error message will be shown. This
 * behaviour is only used in the destructor of DomainPtr to be able a.o.
 * to delete a Domain and Representation pair without unnecessary
 * messages.
 * 
 * 7     21-12-99 12:58p Martin
 * added a domain and column Coordbuf based on domain binary to be able to
 * read and store dynamically coordbufs in a table
 * 
 * 6     10/12/99 10:06a Wind
 * debugged for internal domains and representation
 * 
 * 5     29-09-99 9:25a Martin
 * Addede lement to enum domain types
 * 
 * 4     9/20/99 1:03p Wind
 * added stFLOAT
 * 
 * 3     16-04-99 17:37 Koolhoven
 * header  comment
 * 
 * 2     16-04-99 17:25 Koolhoven
 * export function
// Revision 1.3  1998/09/16 17:22:46  Wim
// 22beta2
//
// Revision 1.2  1997/08/14 18:50:32  Wim
// Added sName() to report always the extension when different from
// the normal one.
//
/* Domain, DomainPtr, StoreType
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  WK   14 Aug 97    8:47 pm
*/

#ifndef ILWDOMAIN_H
#define ILWDOMAIN_H

#include "Engine\Base\DataObjects\ilwisobj.h"

class Domain;
class DomainPtr;
class DATEXPORT DomainSort;
class DATEXPORT DomainClass;
class DATEXPORT DomainIdentifier;
class DATEXPORT DomainGroup;
class DATEXPORT DomainValue;
class DATEXPORT DomainValueInt;
class DATEXPORT DomainValueReal;
class DATEXPORT DomainString;
class DATEXPORT DomainImage;
class DATEXPORT DomainBit;
class DATEXPORT DomainPicture;
class DATEXPORT DomainColor;
class DATEXPORT DomainNone;
class DATEXPORT DomainBinary;
class DATEXPORT DomainCoord;
class DATEXPORT DomainBool;
class DomainUniqueID;
class DomainCoordBuf;
class ObjectStructure;

enum DomainType { dmtCLASS, dmtID, dmtVALUE, dmtIMAGE, dmtPICTURE,
                  dmtBIT, dmtNONE, dmtSTRING, dmtGROUP, dmtCOLOR, dmtCOORD, 
                  dmtBOOL, dmtBINARY, dmtCOORDBUF, dmtUNIQUEID, dmtUNKNOWN};
enum StoreType { stBIT, stDUET, stNIBBLE, stBYTE, stINT, stLONG,
                 stREAL, stCRD, stSTRING, stBINARY, stFLOAT, stCOORDBUF, stFOREIGNFORMAT, stCRD3D,stCOORDBUF3D};
                  
class Domain: public IlwisObject
{
public:
    _export Domain();
    _export Domain(const FileName&, double rMin, double rMax, double rStep=0); // DomainValueReal
    _export Domain(const FileName&, long iMin, long iMax); // DomainValueInt
	_export Domain(const FileName&, const String& sFalse, const String& sTrue); // DomainBool
    _export Domain(const FileName&, long iNr, DomainType dmt, const String& sPrefix); // DomainClass/Identifier
    _export Domain(const FileName&, long iNr, DomainType dmt = dmtCLASS);  // DomainClass/Group/Identifier/Picture
    _export Domain(const FileName& fn, const CoordSystem& csy, bool threedD=false);
    _export Domain(const FileName&);
    _export Domain(const Domain& dm);
    _export Domain(const String& sExpression);
    _export Domain(const String& sExpression, const String& sPath);
    _export Domain(const char *sExpression);

    void                operator = (const Domain& dm) 
                            { SetPointer(dm.pointer()); }
    inline DomainPtr* ptr() const;
    DomainPtr*          operator -> () const 
                            { return ptr(); }
    static Domain       dmCopy(const Domain& dm, const FileName& fnObj, const FileName& fnTo);
    static Domain       dmFindSameOnDisk(const Domain& dm, const String& sSearchDir = "");
    void _export        ClassToIdent();
    void _export        IdentToClass();
    void _export        PictureToClass();
    static DomainPtr*   pGet(const FileName& fn);
    static String _export  sDomainType(DomainType dmt);
    static String       sStoreType(StoreType st);
    static StoreType    st(const String& sStoreType);
    static DomainType _export dmt(const FileName& fn);
    static DomainType _export dmt(const String& sDomType);

    static IlwisObjectPtrList listDom;

private:
	void InitRepresentation(const FileName& fnDom);
};

#include "Engine\Representation\Rpr.h"

class DomainPtr: public IlwisObjectPtr
{
friend class Domain;

public:
    virtual _export ~DomainPtr();

    virtual void             Store();
    virtual String           sType() const;
    virtual String           sName(bool fExt = false, const String& sDirRelative = "") const;
    virtual String           sNameQuoted(bool fExt = false, const String& sDirRelative = "") const;
    short                    iWidth() const 
                                { return _iWidth; }
    void                     SetWidth(int iWidth) 
                                { _iWidth = iWidth; fChanged = true; }
    virtual StoreType        stNeeded() const=0;
    virtual                  String sValueByRaw(long iRaw, short iWidth=-1, short iDec=-1) const;
    virtual long             iRaw(const String& sValue) const;
    bool                     fRawAvailable() const 
                                { return _fRawAvail; }
    virtual bool             fValid(const String& sValue) const;
    _export bool             fValidDomain() const;
		void _export						 GetObjectStructure(ObjectStructure& os);
    Representation _export   rpr() const;
    void _export             SetRepresentation(const Representation& rpr);
    virtual String           sObjectSection() const;
    DomainType  _export      dmt() const;
    DomainValue _export      *pdv() const;
    DomainValueInt _export   *pdvi() const;
    DomainValueReal _export  *pdvr() const;
    DomainClass _export      *pdc() const;
    DomainIdentifier _export *pdid() const;
    DomainString _export     *pds() const;
    DomainImage _export      *pdi() const;
    DomainBit _export        *pdbit() const;
    DomainPicture _export    *pdp() const;
    DomainSort _export       *pdsrt() const;
    DomainGroup _export      *pdgrp() const;
    DomainNone _export       *pdnone() const;
    DomainCoord _export      *pdcrd() const;
    DomainBinary _export     *pdbin() const;
    DomainColor _export      *pdcol() const;
    DomainBool _export       *pdbool() const;
		DomainCoordBuf _export   *pdmcb() const;
		DomainUniqueID _export   *pdUniqueID() const;
    static StoreType         st(unsigned long iNr);
    virtual long             iMaxStringWidth() const;
    virtual void             Rename(const FileName& fnNew);

protected:
    DomainPtr(const FileName&, bool fCreate=false);

    virtual Representation rprLoad(bool fShowOnError = true);

    short             _iWidth;
    bool              _fRawAvail;

private:
    static DomainPtr* create(const FileName& fn);
    static DomainPtr* create(const String& sExpression);
};

inline DomainPtr* Domain::ptr() const { return static_cast<DomainPtr*>(pointer()); }
/*
inline Domain undef(const Domain&)
{ return Domain(); }*/
#endif // ILWDOMAIN_H





