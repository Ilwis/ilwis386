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
/* $Log: /ILWIS 3.0/GeoReference/Gr.h $
 * 
 * 13    13-12-02 18:39 Hendrikse
 * made virtual  the function bool fGeoRefNone() const; to enable
 * re-implementation in derived georefs with related (dominant) georef
 * 
 * 12    29-05-02 13:13 Hendrikse
 * added grStereomate stuff (4X in cpp 2X in h)
 * 
 * 11    11/26/01 18:14 Hendrikse
 * added pgScaleRotate
 * 
 * 10    6/18/01 16:05 Hendrikse
 * added GeoRefEpipolar* GeoRefPtr::pgEpipolar() const
 * 
 * 9     8-12-00 17:28 Hendrikse
 * added class GeoRefParallelProjective;  and pointer to it
 * 
 * 8     8-09-00 4:17p Martin
 * added GetObjectStructureFunction and DoNotUpdate function
 * 
 * 7     22-10-99 12:54 Wind
 * thread save access (not yet finished)
 * 
 * 6     13/08/99 16:40 Willem
 * Added functions to return pointers to GRFactor, GRSubmap and
 * GRMirrorRotate
 * 
 * 5     12/08/99 16:56 Willem
 * Added function to get pointer to GeoRefSmpl from GeoRefPtr
 * 
 * 4     17-06-99 2:27p Martin
 * added _export
 * 
 * 3     17-06-99 1:08p Martin
 * added _exports for import/export dll
// Revision 1.3  1998/09/16 17:22:46  Wim
// 22beta2
//
// Revision 1.2  1997/08/11 15:47:25  Wim
// Added pgc() to test for GeoRefCorners
//
/* GeoRef, GeoRefPtr
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  WK   11 Jun 98    2:01 pm
*/

#ifndef ILWGR_H
#define ILWGR_H
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Engine\SpatialReference\Coordsys.h"

class Map;
class GeoRef;
class GeoRefPtr;
class GeoRefSmpl;
class GeoRefCorners;
class GeoRef3D;
class GeoRefCTP;
class GeoRefOrthoPhoto;
class GeoRefCTPplanar;
class GeoRefDirectLinear;
class GeoRefParallelProjective;
class GeoRefFactor;
class GeoRefSubMap;
class GeoRefMirrorRotate;
class GeoRefEpipolar;
class GeoRefScaleRotate;
class GeoRefStereoMate;
class GeoRefCornersWMS;
class GeoRefOSM;

class GeoRefPtr: public IlwisObjectPtr
{
  friend class GeoRef;
  friend class Map;
  static GeoRefPtr* create(const FileName& fn);
  static GeoRefPtr* create(const FileName& fn, const String& sExpression);
protected:
  GeoRefPtr(const FileName&);
  _export GeoRefPtr(const FileName& fn, RowCol rc);
  _export GeoRefPtr(const FileName& fn, const CoordSystem&, RowCol rc);
public:
  virtual void Store();
  virtual _export ~GeoRefPtr();
  virtual String _export sType() const;
  const CoordSystem& cs() const { return _csys; }
  void _export SetCoordSystem(const CoordSystem&); // dangerous function!
  virtual CoordBounds _export cb() const;
  RowCol rcSize() const { return _rc; }  // size in lines and columns
  inline bool fInside(const RowCol& rc) const;
  virtual double _export rPixSize() const;
  inline RowCol rcConv(const Coord&) const;
  inline Coord	cConv(const RowCol&) const;
  virtual void Coord2RowCol(const Coord& c, double& rR, double& rC) const;
  virtual void RowCol2Coord(double rR, double rC, Coord& c) const;
  virtual bool fEqual(const IlwisObjectPtr&) const;
//  virtual void Enlarge(double rEnlfac) {}
  virtual String _export sName(bool fExt = false, const String& sDirRelative = "") const;
  virtual void _export Rotate(bool fSwapRows, bool fSwapCols, bool fRotate);
  virtual bool _export fGeoRefNone() const;
  virtual void _export GetObjectDependencies(Array<FileName>& afnObjDep);
  virtual bool _export fDependent() const;
  virtual bool _export fNorthOriented() const;
  GeoRefCorners _export *pgc() const;
  GeoRef3D _export *pg3d() const;
  GeoRefCTP _export *pgCTP() const;
  GeoRefCTPplanar _export *pgCTPplanar() const;
  GeoRefOrthoPhoto _export *pgOrthoPhoto() const;
  GeoRefDirectLinear _export *pgDirectLinear() const;
  GeoRefParallelProjective _export *pgParallelProjective() const;
  GeoRefSmpl _export *pgsmpl() const;
  GeoRefFactor _export *pgFac() const;
  GeoRefSubMap _export *pgSub() const;
  GeoRefMirrorRotate _export *pgMirRot() const;
	GeoRefEpipolar _export *pgEpipolar() const;
	GeoRefStereoMate _export *pgStereoMate() const;
	GeoRefScaleRotate _export *pgScalRot() const;
	GeoRefCornersWMS _export *pgWMS() const;
	GeoRefOSM _export *pgOSM() const;
  virtual void _export Rename(const FileName& fnNew);
  virtual bool _export fLinear() const;
  virtual String _export sObjectSection() const;
	virtual void _export GetObjectStructure(ObjectStructure& os);
	virtual void _export DoNotUpdate();	
	virtual void AdjustRC(RowCol rc) {}
protected:
  void SetRowCol(RowCol rc) { _rc = rc; }
  RowCol _rc;
private:
  CoordSystem _csys;
};

class GeoRef: public IlwisObject
{
  static IlwisObjectPtrList listGrf;
public:
  _export GeoRef();
  _export GeoRef(const CoordSystem&, RowCol,
         double a11, double a12, double a21, double a22,
         double b1, double b2);
  GeoRef(RowCol rc,            // obsolete !!!!!!!!
         double a11, double a12, double a21, double a22,
         double b1, double b2);
  _export GeoRef(RowCol rc);
  _export GeoRef(const FileName& fn);
  GeoRef(const FileName& fn, const String& sExpression);
  _export GeoRef(const String& sExpression);
  _export GeoRef(const String& sExpression, const String& sPath, const String& baseName="");
  _export GeoRef(const GeoRef& gr);
  void operator = (const GeoRef& gr) { SetPointer(gr.pointer()); }
  GeoRefPtr* ptr() const { return static_cast<GeoRefPtr*>(pointer()); }
  GeoRefPtr* operator -> () const { return ptr(); }
  static GeoRef _export grFindSameOnDisk(const GeoRef& gr, const String& sSearchDir = "");
  static GeoRefPtr* pGet(const FileName& fn);
};

// Inline functions

inline bool GeoRefPtr::fInside(const RowCol& rc) const
    { return (rc.Row >= 0) && (rc.Col >= 0) &&
             (rc.Row < rcSize().Row) && (rc.Col < rcSize().Col); }
inline RowCol GeoRefPtr::rcConv(const Coord& c) const
{
  double rRow, rCol;
  Coord2RowCol(c, rRow, rCol);
  return RowCol(rRow, rCol);
}

inline Coord GeoRefPtr::cConv(const RowCol& rc) const
{
  Coord c;
  RowCol2Coord(rc.Row + 0.5, rc.Col + 0.5, c);
  return c;
}


#endif // ILWGR_H




