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
/* $Log: /ILWIS 3.0/CoordinateSystem/Coordsys.h $
 * 
 * 13    25-02-02 12:14 Hendrikse
 * inserted '_export' 
 *  CoordSystemBoundsOnly _export *pcsBoundsOnly() const;
 * needed to check on csy-type to solve wish about required coordbounds
 * when making a coordsystem projection
 * 
 * 12    8-09-00 3:43p Martin
 * added function for setting the update status and getting the
 * objectstructure
 * 
 * 11    13-10-99 11:45 Hendrikse
 * added  CoordSystemDirectLinear  twice
 * 
 * 10    29-09-99 16:11 Hendrikse
 * inserted _export before cValue(const String&)
 * 
 * 9     28-09-99 18:12 Hendrikse
 * added many _export qualifiers and a call to pcsCTP
 * 
 * 8     20-09-99 17:42 Koolhoven
 * export
 * 
 * 7     7-09-99 3:34p Martin
 * added 2.22 changes
 * 
 * 6     30-07-99 14:48 Koolhoven
 * exports
 * 
 * 5     23-06-99 12:21p Martin
 * added exports
 * 
 * 4     21-06-99 2:01p Martin
 * added export
 * 
 * 3     17-06-99 2:27p Martin
 * // -> /*
 * 
 * 2     17-06-99 1:08p Martin
 * added _exports for import/export dll
// Revision 1.2  1998/09/16 17:22:46  Wim
// 22beta2
//
/* CoordSystem, CoordSystemPtr
   Copyright Ilwis System Development ITC
   october 1995, by Wim Koolhoven
	Last change:  WK    8 Apr 98    1:07 pm
*/

#ifndef ILWCOORDSYS_H
#define ILWCOORDSYS_H
#include "Engine\Base\DataObjects\ilwisobj.h"

class CoordSystem;
class CoordSystemPtr;
class DATEXPORT CoordWithCoordSystem;
class DATEXPORT CoordSystemBoundsOnly;
class DATEXPORT CoordSystemViaLatLon;
class DATEXPORT CoordSystemLatLon;
class DATEXPORT CoordSystemProjection;
class DATEXPORT CoordSystemDirect;
class DATEXPORT CoordSystemFormula;
class DATEXPORT CoordSystemCTP;
class DATEXPORT CoordSystemTiePoints;
class DATEXPORT CoordSystemOrthoPhoto;
class DATEXPORT CoordSystemDirectLinear;

class CoordSystemPtr: public IlwisObjectPtr
{
  friend class CoordSystem;
  static CoordSystemPtr* create(const FileName&);
  static _export CoordSystemPtr* create(const FileName&, const String&);
private:  
  _export CoordSystemPtr();
protected:
  CoordSystemPtr(const FileName&);
  CoordSystemPtr(const FileName&, int);
public:
  virtual void _export Store();
  virtual String _export sType() const;
  short iWidth() const { return _iWidth; }
  short iDec() const { return _iDec; }
  virtual String _export sValue(const Coord&, short iWidth=-1, short iDec=-1) const;
  virtual Coord _export cValue(const String&) const;
  virtual bool _export fConvertFrom(const CoordSystem&) const;
  virtual Coord _export cConv(const CoordSystem&, const Coord&) const;
  virtual bool _export fLatLon2Coord() const;
  virtual bool _export fCoord2LatLon() const;
  virtual LatLon _export llConv(const Coord&) const;
  virtual Coord _export cConv(const LatLon&) const;
  CoordBounds _export cbConv(const CoordSystem&, const CoordBounds&);
  CoordBounds cb;
  virtual void _export Rename(const FileName& fnNew);
  bool _export fUnknown() const;
  virtual bool _export fConvertTo(const CoordSystemPtr*) const;
	virtual void _export GetObjectStructure(ObjectStructure& os);
	virtual void _export DoNotUpdate();		
  virtual Coord _export cInverseConv(const CoordSystemPtr*, const Coord&) const;
// dynamic casts:
  CoordSystemBoundsOnly _export *pcsBoundsOnly() const;
  CoordSystemViaLatLon _export *pcsViaLatLon() const;
  CoordSystemLatLon _export *pcsLatLon() const;
  CoordSystemProjection _export *pcsProjection() const;
  CoordSystemDirect _export *pcsDirect() const;
  CoordSystemFormula _export *pcsFormula() const;
  CoordSystemCTP _export *pcsCTP() const;
  CoordSystemTiePoints _export *pcsTiePoints() const;
  CoordSystemOrthoPhoto _export *pcsOrthoPhoto() const;
  CoordSystemDirectLinear _export *pcsDirectLinear() const;
protected:
  short _iWidth;
  short _iDec;
  double rUnitSize; // in meters
};

class CoordSystem: public IlwisObject
{
public:  
    _export CoordSystem();
    _export CoordSystem(const FileName& fn);
    _export CoordSystem(const FileName& fn, const String& sExpr);
    _export CoordSystem(const String& sExpr, const String& sPath);
    _export CoordSystem(const CoordSystem& cs);
		_export CoordSystem(const char* sExpr);
    _export CoordSystem(const String& sExpr);

    void                    operator = (const CoordSystem& cs) 
                                { SetPointer(cs.pointer()); }
    CoordSystemPtr*         ptr() const 
                                { return static_cast<CoordSystemPtr*>(pointer()); }
    CoordSystemPtr*         operator -> () const 
                                { return ptr(); }
    static CoordSystemPtr*  pGet(const FileName& fn);

private:
    static IlwisObjectPtrList listCS;
};

class CoordWithCoordSystem
{
public:
  CoordWithCoordSystem(const Coord& c): crd(c) {}
  CoordWithCoordSystem(const Coord& c, const CoordSystem& cs): crd(c), csys(cs) {}
  CoordWithCoordSystem(const CoordSystem& cs): csys(cs) {}
  operator const Coord&() const { return crd; }
  operator const CoordSystem&() const { return csys; }
  void operator = (const Coord& c) { crd = c; }
  Coord& c() { return crd; }
  const Coord& c() const { return crd; }
  CoordSystemPtr* operator -> () const { return csys.ptr(); }
private:  
  Coord crd;
  CoordSystem csys;
};


#endif // ILWCOORDSYS_H




