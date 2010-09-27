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
/* $Log: /ILWIS 3.0/FormElements/objlist.h $
 * 
 * 28    10/10/03 2:47p Lichun
 * Merging from $/ILWIS Hydrologic Flow/FormElements/objlist.h
 * 
 * 29    4/22/03 3:13p Lichun
 * Added a class MapListerDomainName to list only maps with the specified
 * domain name. 
 * 
 * 27    2/06/03 12:01p Lichun
 * 
 * 25    9/06/02 4:43p Lichun
 * added TableListerDomainType 
 * 
 * 26    19-11-02 17:21 Willem
 * Removed: Duplicated class declarations
 * 
 * 25    19-11-02 16:09 Retsios
 * 
 * 29    19-11-02 15:04 Retsios
 * Merge from Main
 * 
 * 28    18-11-02 20:00 Retsios
 * Reorganized the fAcceptAttributeTable a bit because more objectlisters
 * need it - now ObjectLister has a public virtual member fHasChildren
 * that (currently) does the same as the old
 * MapListerDomainType::fAcceptAttributeTable - this public function is
 * used in ObjectTreeCtrl to decide if there should be an 'expansion'
 * symbol (a  '+') before the labels.
 * 
 * 26    10-07-02 16:43 Retsios
 * Allow specification of the two extra boolean parameters for selecting
 * attribute cols and maplists
 * 
 * 25    9-07-02 19:46 Retsios
 * Added MapListerDomainTypeAndGeoRef to filter on a domain type and a
 * georef (was handy for SMCE)
 * 
 * 24    14-11-02 18:33 Retsios
 * Merge from SMCE - Added MapListerDomainTypeAndGeoRef to filter on a
 * domain type and a georef
 * 
 * 26    10-07-02 16:43 Retsios
 * Allow specification of the two extra boolean parameters for selecting
 * attribute cols and maplists
 * 
 * 25    9-07-02 19:46 Retsios
 * Added MapListerDomainTypeAndGeoRef to filter on a domain type and a
 * georef (was handy for SMCE)
 * 
 * 24    14-11-02 18:33 Retsios
 * Merge from SMCE - Added MapListerDomainTypeAndGeoRef to filter on a
 * domain type and a georef
 * 
 * 26    10-07-02 16:43 Retsios
 * Allow specification of the two extra boolean parameters for selecting
 * attribute cols and maplists
 * 
 * 25    9-07-02 19:46 Retsios
 * Added MapListerDomainTypeAndGeoRef to filter on a domain type and a
 * georef (was handy for SMCE)
 * 
 * 24    14-11-02 18:33 Retsios
 * Merge from SMCE - Added MapListerDomainTypeAndGeoRef to filter on a
 * domain type and a georef
 * 
 * 26    10-07-02 16:43 Retsios
 * Allow specification of the two extra boolean parameters for selecting
 * attribute cols and maplists
 * 
 * 25    9-07-02 19:46 Retsios
 * Added MapListerDomainTypeAndGeoRef to filter on a domain type and a
 * georef (was handy for SMCE)
 * 
 * 23    8/22/01 15:12 Willem
 * The MapListerGeoRef now considers both Maps and Maplists
 * 
 * 22    7-08-01 19:45 Koolhoven
 * MapListerDomainType has extra boolean to allow also a maplist instead
 * of a raster map
 * 
 * 21    2-08-01 17:14 Koolhoven
 * added GraphType and GraphLister
 * 
 * 20    15-03-01 2:58p Martin
 * see objlist.cpp
 * 
 * 19    8/03/01 17:47 Willem
 * - Added MapListerDomainAndGeoRef to check both Domain and Georef of
 * rastermaps
 * - Improved Maplist handling in Domain and Georef Map listers
 * 
 * 18    28/02/01 10:25 Willem
 * MapListerDomainType now also checks MapLists for internal maps
 * 
 * 17    26/01/01 10:27 Willem
 * The MapListerDomainType constructors now all initialize the
 * fUseAttribColumns member
 * 
 * 16    10-11-00 17:12 Koolhoven
 * added enum csType
 * and class CoordSystemLister
 * 
 * 15    31-08-00 9:24a Martin
 * mapaatribute works now with histograms and rpr's  (?!)
 * 
 * 14    10-08-00 15:07 Koolhoven
 * MapListerDomainType is now derived from ObjectExtensionLister, so it
 * now accepts more than 1 extension
 * 
 * 13    27-07-00 15:16 Koolhoven
 * Added UniqueID option to Create PntMap and SegMap forms
 * 
 * 12    18-07-00 12:54p Martin
 * only legal maps are selectable in th objecttree control. Illegal maps
 * with a legal attrib table are visible but not selectable
 * 
 * 11    23-05-00 17:26 Koolhoven
 * derived DomainLister from ObjectExtensionLister because multiple
 * extensions (.dom and .csy) are now allowed
 * 
 * 10    17-05-00 12:48p Martin
 * added virtual function sFileExt
 * 
 * 9     9-05-00 10:53a Martin
 * attrib columns appear in the tree control. Namedit and objlist have
 * been extended to use this behaviour
 * 
 * 8     4-05-00 12:39p Martin
 * added a second parm to the fOK function (default empty). Is used for
 * columns checking. Added column and attribute checking to the
 * MapDomainTypeLister
 * 
 * 7     4/12/00 3:51p Hendrikse
 * added csy to ObjectExtLister and ObjectExtensionLister  to recognize it
 * as a domain
 * 
 * 6     3/22/00 5:10p Wind
 * add dmCOORD type for domainlister
 * 
 * 5     31-01-00 11:53 Koolhoven
 * Quicker opening of ObjectTreeCtrl by earlier filtering on extension
 * 
 * 4     6-12-99 10:18 Wind
 * changed grType to enum { grALL, grNONONOE, grNO3D, gr3DONLY }
 * for use in GeoRefLister
 * 
 * 3     18-11-99 12:18p Martin
 * //->/*
 * 
 * 2     18-11-99 11:10a Martin
 * changed a private member to protected
// Revision 1.5  1998/09/16 17:37:53  Wim
// 22beta2
//
// Revision 1.4  1997/09/22 13:20:18  Wim
// Removed FillDir(s)
//
// Revision 1.3  1997-09-11 20:27:04+02  Wim
// Added MapListerDomainType() with extension choice
//
// Revision 1.2  1997-07-28 19:03:23+02  Wim
// Added DomainAllExtLister. DomainLister only lists .DOM domains
// Added RepresentationAllExtLister. RepresentationLister only lists .RPR representations
//
/* Interface for ObjectLister
// by Wim Koolhoven, june 1995
// (c) Computer Department ITC
	Last change:  WK   19 Feb 98   12:35 pm
*/

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
  virtual bool fOK(const FileName&, const String& sExtra="")=0;
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




