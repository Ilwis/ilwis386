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


#ifndef OBJLIST_H
#define OBJLIST_H

#include "Engine\Domain\dm.h"

static const String sCHECK_OBJECT_ONLY("@CheckOb!jectOnly!@"); // special constant to use if no attribute tables are to be checked

enum dmType { dmCLASS = 0x01, dmIDENT = 0x02, dmVALUE = 0x04, dmGROUP = 0x08,
              dmIMAGE = 0x10, dmNONE = 0x20, dmPICT = 0x40, dmCOLOR = 0x80,
              dmBOOL = 0x100, dmBIT = 0x200, dmSTRING = 0x400, dmCOORD = 0x800,
							dmUNIQUEID = 0x1000, dmTIME=0x2000 };

inline int dmtype(DomainType dmt)
{
  switch (dmt) {
    case dmtCLASS:    return dmCLASS;
    case dmtID:       return dmIDENT;
    case dmtUNIQUEID: return dmUNIQUEID;
    case dmtVALUE:    return dmVALUE;
    case dmtIMAGE:    return dmIMAGE;
    case dmtPICTURE:  return dmPICT;
    case dmtBIT:      return dmBIT;
    case dmtNONE:     return 0;
    case dmtSTRING:   return dmSTRING;
    case dmtGROUP:    return dmGROUP;
    case dmtCOLOR:    return dmCOLOR;
    case dmtCOORD:    return dmCOORD;
    case dmtBOOL:     return dmBOOL;
	case dmtTIME:	  return dmTIME;
    case dmtBINARY:   return 0;
    default: return 0;
  }
};

enum grType { grALL = 0x01, grNO3D = 0x02, grNONONE = 0x04, gr3DONLY = 0x08 };

enum filType { filLINEAR = 0x01, filRANK = 0x02, filMAJORITY = 0x04, filRADAR = 0x08,
               filBINARY = 0x10 };  

enum csType { csBOUNDSONLY = 0x01, csPROJ = 0x02, csLATLON = 0x04,
						  csFORMULA = 0x08, csTIEPOINTS = 0x10,
							csORTHOPHOTO = 0x20, csDIRECTLINEAR = 0x40 };

enum GraphType { grhGRAPH = 0x01, grhROSEDIAGRAM = 0x02 };

class _export ObjectLister
{
  friend class _export NameEdit;
public:
  ObjectLister(NameEdit* nameedit=0): ne(nameedit) {}
  virtual ~ObjectLister() {}
  void FillDir();
  virtual bool fOK(const FileName&, const String& sExtra="") { return false;}
  virtual void AddObjects()=0;
	virtual String sFileExt();
	virtual String sDefaultSelectedValue(const FileName& fn);
	virtual bool fHasChildren(const FileName& fn); // used in ObjectTreeCtrl to decide whether a '+' should be displayed before the item
protected:
  NameEdit* ne;
};

class _export ObjectExtensionLister: public ObjectLister
{
public:
  ObjectExtensionLister(NameEdit*, const String& sExtensions);
  virtual ~ObjectExtensionLister();
  virtual bool fOK(const FileName&, const String& sExtra="");
  virtual void AddObjects();
  void AddObjects(const String& sExt);
	virtual String sFileExt();
private:
  Array<String*> asExt;
};

class _export ObjectExtLister: public ObjectLister
{
public:
  ObjectExtLister(const String& s)
    : sExt(s) {}
  virtual ~ObjectExtLister();
  virtual void AddObjects();
	virtual String sFileExt();
protected:
  String sExt;
};

class _export TableLister: public ObjectExtensionLister
{
public:
  TableLister(const Domain& dom, const String& sExtensions=".tbt")
  : ObjectExtensionLister(0, sExtensions), dm(dom) {}
  virtual bool fOK(const FileName&, const String& sExtra="");
private:
  Domain dm;
};

class _export TableListerDomainType: public ObjectExtensionLister
{
public:
  TableListerDomainType(int dmTypes, const String& sExtensions=".tbt")
  : ObjectExtensionLister(0, sExtensions), m_dmTypes(dmTypes) {}
  virtual bool fOK(const FileName&, const String& sExtra="");
private:
  int m_dmTypes;
};

class _export TableAsLister: public ObjectExtensionLister
{
public:
  TableAsLister(const String& sExtensions)
  : ObjectExtensionLister(0,sExtensions) {}
  virtual bool fOK(const FileName&, const String& sExtra="");
};

class _export DomainLister: public ObjectExtensionLister
{
public:
  DomainLister(long types = dmCLASS|dmIDENT|dmVALUE|dmGROUP)
  : ObjectExtensionLister(0, ".dom.csy"), dmTypes(types) {}
  virtual bool fOK(const FileName&, const String& sExtra="");
private:
  long dmTypes;
};

class _export DomainAllExtLister: public ObjectExtensionLister
{
public:
  DomainAllExtLister(long types = dmCLASS|dmIDENT|dmVALUE|dmGROUP)
  :  ObjectExtensionLister(0,".dom.mps.mpr.mpa.mpp.csy"), dmTypes(types) {}
  virtual bool fOK(const FileName&, const String& sExtra="");
private:
  long dmTypes;
};

class _export RepresentationLister: public ObjectExtLister
{
public:
  RepresentationLister(const Domain& dom)
  : ObjectExtLister(".rpr"), dm(dom), fRprAttr(false) {}
  virtual bool fOK(const FileName&, const String& sExtra="");
  void SetDomain(const Domain& dom);
  bool fRprAttr;
private:
  Domain dm;
};

class _export RepresentationAllExtLister: public ObjectExtensionLister
{
public:
  RepresentationAllExtLister(const Domain& dom)
  : ObjectExtensionLister(0,".rpr.dom.mps.mpr.mpa.mpp"), dm(dom), fRprAttr(false) {}
  virtual bool fOK(const FileName&, const String& sExtra="");
  void SetDomain(const Domain& dom);
  bool fRprAttr;
private:
  Domain dm;
};

class _export GeoRefLister: public ObjectExtLister
{
public:
  GeoRefLister(long types)
  : ObjectExtLister(".grf"), grTypes(types) {}
  virtual bool fOK(const FileName&, const String& sExtra="");
private:
  long grTypes;
};

class _export FilterLister: public ObjectExtLister
{
public:
  FilterLister(long types)
  : ObjectExtLister(".fil"), filTypes(types) {}
  virtual bool fOK(const FileName&, const String& sExtra="");
private:
  long filTypes;
};

class _export MapListerDomainType: public ObjectExtensionLister
{
public:
	MapListerDomainType(long types = dmCLASS|dmIDENT|dmVALUE|dmGROUP|dmIMAGE, bool fUseAttrib=false, bool fAllowMapLst=false)
		: ObjectExtensionLister(0, ".mpr.mpl")
    , dmTypes(types), fUseAttribColumns(fUseAttrib), fAllowMapList(fAllowMapLst)
	{}
	MapListerDomainType(const String& sExt, long types = dmCLASS|dmIDENT|dmVALUE, bool fUseAttrib=false, bool fAllowMapLst=false)
		: ObjectExtensionLister(0, sExt)
    , dmTypes(types), fUseAttribColumns(fUseAttrib), fAllowMapList(fAllowMapLst)
	{}
	virtual bool fOK(const FileName&, const String& sColName="");
	virtual bool fHasChildren(const FileName& fn);
	virtual String sDefaultSelectedValue(const FileName& fn);
	
private:
	bool fAcceptAttributeTable(const FileName& fn);
	bool fAcceptDomain(FileName fnDom);
	bool fAcceptColumn(const FileName& fnAttrib, const String& sColName);
	bool fUseAttribColumns, fAllowMapList;
	long dmTypes;
};

class _export MapListerDomainTypeAndGeoRef: public MapListerDomainType
{
public:
	MapListerDomainTypeAndGeoRef(const FileName& fnGeoRef, long types = dmCLASS|dmIDENT|dmVALUE|dmGROUP|dmIMAGE, bool fUseAttrib=false, bool fAllowMapLst=false)
		: MapListerDomainType(types, fUseAttrib, fAllowMapLst)
		, fnGrf(fnGeoRef)
	{}
	virtual bool fOK(const FileName&, const String& sColName="" );
private:
	const FileName fnGrf;
};

class _export MapListerGeoRef: public ObjectExtLister
{
public:
	MapListerGeoRef(const FileName& fnGeoRef)
		: ObjectExtLister(".mpr.mpl"), fnGrf(fnGeoRef)
	{}
	virtual bool fOK(const FileName&, const String& sExtra="");
private:
	const FileName fnGrf;
};

class _export MapListerDomainName: public ObjectExtLister
{
public:
	MapListerDomainName(const FileName& fnDomain)
		: ObjectExtLister(".mpr.mpl")
	{// Make sure that system objects have the correct directory
		fnDom = IlwisObjectPtr::fnCheckPath(fnDomain);
	}
	virtual bool fOK(const FileName&, const String& sExtra="");
private:
	FileName fnDom;
};


class _export MapListerDomainAndGeoRef: public ObjectExtLister
{
public:
	MapListerDomainAndGeoRef(const FileName& fnGeoRef, const RowCol& rcSize, const FileName& fnDom, bool fUseAttrib = false)
		: ObjectExtLister(".mpr"), m_fnGrf(fnGeoRef), m_rcSize(rcSize), m_fnDom(fnDom), m_fUseAttribColumns(fUseAttrib)
	{}
	virtual bool fOK(const FileName&, const String& sColName="" );
	virtual bool fHasChildren(const FileName& fn);
private:
	bool fAcceptAttributeTable(const FileName& fn);
	bool fAcceptDomain(FileName fnDom);
	bool fAcceptColumn(const FileName& fnAttrib, const String& sColName);

	const FileName m_fnGrf;
	const FileName m_fnDom;
	const RowCol   m_rcSize;
	const bool m_fUseAttribColumns; // also search in attrib cols for suitable domain
};

class _export BaseMapListerDomain: public ObjectExtLister
{
public:
  BaseMapListerDomain(const String& sExt, const FileName& fnDomain, bool fUseAttrib = false)
  : ObjectExtLister(sExt), m_fnDom(fnDomain), m_fUseAttribColumns(fUseAttrib) {}
  virtual bool fOK(const FileName&, const String& sColName="");
	virtual bool fHasChildren(const FileName& fn);
private:
	bool fAcceptAttributeTable(const FileName& fn);
	bool fAcceptDomain(FileName fnDom);
	bool fAcceptColumn(const FileName& fnAttrib, const String& sColName);

  const FileName m_fnDom;
	const bool m_fUseAttribColumns; // also search in attrib cols for suitable domain
};

class _export PointMapListerDomain: public BaseMapListerDomain
{
public:
  PointMapListerDomain(const FileName& fnDomain)
  : BaseMapListerDomain(".mpp", fnDomain) {}
};
                         
class _export CoordSystemLister: public ObjectExtLister
{
public:
  CoordSystemLister(long types)
  : ObjectExtLister(".csy"), csTypes(types) {}
  virtual bool fOK(const FileName&, const String& sExtra="");
private:
  long csTypes;
};

class _export GraphLister: public ObjectExtLister
{
public:
  GraphLister(long types)
  : ObjectExtLister(".grh"), grTypes(types) {}
  virtual bool fOK(const FileName&, const String& sExtra="");
private:
  long grTypes;
};


#endif // OBJLIST_H




