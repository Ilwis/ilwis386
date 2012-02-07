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
/* $Log: /ILWIS 3.0/GeoReference/Gr.cpp $
 * 
 * 24    13-12-02 18:40 Hendrikse
 * made virtual  the function bool fGeoRefNone() const; and changed it to
 * enable re-implementation in derived georefs with related (dominant)
 * georef
 * 
 * 23    29-05-02 13:13 Hendrikse
 * added grStereomate stuff (4X in cpp 2X in h)
 * 
 * 22    4-12-01 18:44 Retsios
 * Repaired LFN problem
 * 
 * 21    11/26/01 18:15 Hendrikse
 * added use of pgScalRot
 * 
 * 18    6/22/01 13:22 Hendrikse
 * added inline-defined georefEpipolar in constructor and improved string
 * comparisons
 * 
 * 17    6/18/01 16:05 Hendrikse
 * added : GeoRefEpipolar* GeoRefPtr::pgEpipolar() const
 * 
 * 16    8-12-00 17:30 Hendrikse
 * added and implemented class GeoRefParallelProjective;  and pointer to
 * it,
 * and necessary includes
 * 
 * 15    24-10-00 11:52a Martin
 * changed the getobjectstructure function
 * 
 * 14    8-09-00 4:17p Martin
 * added GetObjectStructureFunction and DoNotUpdate function
 * 
 * 13    17-07-00 8:10a Martin
 * changed sCurDir to sGetCurDir from IlwisWinApp
 * 
 * 12    20-12-99 18:15 Koolhoven
 * Reading of a MapView object (.mpv file) is now possible
 * 
 * 11    2-12-99 13:14 Wind
 * added GeoRefDifferential (was not ported from 2.23)
 * 
 * 10    23-11-99 12:58 Wind
 * added georefsubmapcorners and gerorefsubmapcoords
 * 
 * 9     29-10-99 12:53 Wind
 * case sensitive stuff
 * 
 * 8     29-10-99 9:19 Wind
 * thread save stuff
 * 
 * 7     22-10-99 12:54 Wind
 * thread save access (not yet finished)
 * 
 * 6     9/29/99 10:21a Wind
 * added case insensitive string comparison
 * 
 * 5     23-09-99 18:10 Hendrikse
 * Changed includes into #include "Engine\SpatialReference\SolveOblique.h"
 * and  #include "Engine\SpatialReference\SolveOrthoPhoto.h"
 * 
 * 4     13/08/99 16:40 Willem
 * Added functions to return pointers to GRFactor, GRSubmap and
 * GRMirrorRotate
 * 
 * 3     12/08/99 16:56 Willem
 * Added function to get pointer to GeoRefSmpl from GeoRefPtr
 * 
 * 2     3/11/99 12:16p Martin
 * Added support for Case insesitive 
// Revision 1.3  1998/09/16 17:22:46  Wim
// 22beta2
//
// Revision 1.2  1997/08/11 15:47:50  Wim
// Agged pgc() to get a GeoRefCorners*
//
/* GeoRef, GeoRefPtr
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  J    21 Oct 99   10:03 am
*/

#include "Engine\SpatialReference\Gr.h"
#include "Engine\SpatialReference\GRNONE.H"
#include "Engine\SpatialReference\Grsmpl.h"
#include "Engine\SpatialReference\Grcornrs.h"
#include "Engine\SpatialReference\GRCTP.H"
#include "Engine\SpatialReference\SolveOblique.h"
#include "Engine\SpatialReference\Grctppla.h"
#include "Engine\SpatialReference\SolveOrthoPhoto.h"
#include "Engine\SpatialReference\Grortho.h"
#include "Engine\SpatialReference\SolveParallelProjective.h"
#include "Engine\SpatialReference\GrParallProj.h"
#include "Engine\SpatialReference\Grfactor.h"
#include "Engine\SpatialReference\GRMRROT.H"
#include "Engine\SpatialReference\Grsub.h"
#include "Engine\SpatialReference\GR3D.H"
//#include "Engine\SpatialReference\SolveDirectLinear.h"
#include "Engine\SpatialReference\grdirlin.h"
#include "Engine\SpatialReference\Grfactor.h"
#include "Engine\SpatialReference\Grsub.h"
#include "Engine\SpatialReference\GRMRROT.H"
#include "Engine\SpatialReference\Grdiff.H"
#include "Engine\SpatialReference\GrEpipolar.h"
#include "Engine\SpatialReference\GrStereoMate.h"
#include "Engine\SpatialReference\GrScaleRotate.h"
#include "Engine\Base\DataObjects\ObjectStructure.h"
#include "Engine\SpatialReference\GrcWMS.h"
#include "Engine\DataExchange\gdalproxy.h"
#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\Engine.h"
#include "Engine\Base\System\mutex.h"


IlwisObjectPtrList GeoRef::listGrf;

GeoRef::GeoRef()
: IlwisObject(listGrf)
{}

GeoRef::GeoRef(const GeoRef& gr)
: IlwisObject(listGrf, gr.pointer())
{}

GeoRef::GeoRef(RowCol rc)
: IlwisObject(listGrf)
{
  SetPointer(new GeoRefNone(FileName(),rc));
}

GeoRef::GeoRef(const CoordSystem& cs, RowCol rc,
               double a11, double a12, double a21, double a22,
               double b1, double b2)
: IlwisObject(listGrf)
{
  SetPointer(new GeoRefSmpl(FileName(),cs,rc,a11,a12,a21,a22,b1,b2));
}

GeoRef::GeoRef(RowCol rc,
               double a11, double a12, double a21, double a22,
               double b1, double b2)
: IlwisObject(listGrf)
{
  SetPointer(new GeoRefSmpl(FileName(),CoordSystem(),rc,a11,a12,a21,a22,b1,b2));
}

GeoRef::GeoRef(const FileName& filnam)
: IlwisObject(listGrf)
{
  FileName fn = FileName(filnam, ".grf");
  GeoRefPtr* p = GeoRef::pGet(fn);
  if (p) { // if already open return it
    SetPointer(p);
    return;
  }
  MutexFileName mut(fn);
  p = GeoRef::pGet(fn);
  if (p)  // if already open return it
    SetPointer(p);
  else
    SetPointer(GeoRefPtr::create(fn));
}

GeoRef::GeoRef(const FileName& fn, const String& sExpression)
: IlwisObject(listGrf) // GeoRefPtr::create(fn, sExpression))
{
  String sFunc = IlwisObjectPtr::sParseFunc(sExpression);
  if (sFunc == sUNDEF && fn.sFile.length() == 0) {
    FileName filnam(sExpression,".grf",true);
    GeoRef grf(filnam);
    SetPointer(grf.ptr());
  }
  else {
    SetPointer(GeoRefPtr::create(fn, sExpression));
    if (ptr()->sDescription == "")
      ptr()->sDescription = ptr()->sTypeName();
  }
}

GeoRef::GeoRef(const String& sExpression)
: IlwisObject(listGrf) //, GeoRefPtr::create(FileName(), sExpression))
{
  GeoRef grf(FileName(), sExpression);
  SetPointer(grf.ptr());
}

GeoRef::GeoRef(const String& sExpression, const String& sPath, const String& baseName)
: IlwisObject(listGrf)
{
	int iPos = sExpression.find('(');
	String sGeoRef = sExpression.sLeft(iPos);
	if (fCIStrEqual(sGeoRef, "GeoRefSmpl"))
		SetPointer(GeoRefPtr::create(FileName(), sExpression));
	else if (fCIStrEqual(sGeoRef, "GeoRefDifferential"))
		SetPointer(GeoRefDifferential::create(FileName(), sExpression));
	else if (fCIStrEqual(sGeoRef, "GeoRefEpipolar"))
		SetPointer(GeoRefEpipolar::create(FileName(), sExpression));
	else if (fCIStrEqual(sGeoRef, "GeoRefStereoMate"))
		SetPointer(GeoRefStereoMate::create(FileName(), sExpression));
	else if ( fCIStrEqual(sGeoRef,"geometry")) {
		iPos = sExpression.find_first_of(",");
		String name = sExpression.substr(9, iPos - 9);
		String expr = "geometry(" + sExpression.substr(iPos + 1, sExpression.size());
		FileName fn = String("%S%S", sPath, name);
		fn.sExt = ".grf";
		iPos = expr.find_last_of(",");
		String sEpsg = expr.substr(iPos + 1, expr.size() - iPos - 2);
		int epsg = sEpsg.iVal();
		expr =expr.substr(0, iPos) + ")"; // remove epsg part; grd corners doesnt understand it
		SetPointer(GeoRefCorners::create(fn, expr));
		ptr()->Store();
		CoordSystem cs = getEngine()->gdal->getCoordSystem(fn,epsg);
		ptr()->SetCoordSystem(cs);
	}
	else
	{
		FileName fn(FileName(sExpression, FileName(sPath)), ".grf", true);
		GeoRef grf(fn);
		SetPointer(grf.ptr());
	}  
	if (ptr()->sDescription == "")
		ptr()->sDescription = ptr()->sTypeName();
}

GeoRefPtr* GeoRef::pGet(const FileName& fn)
{
  return static_cast<GeoRefPtr*>(listGrf.pGet(fn));
}

String GeoRefPtr::sType() const
{
  return "GeoReference";
}

void GeoRefPtr::Store()
{
  if (fCIStrEqual(fnObj.sExt, ".grf")) {
    IlwisObjectPtr::Store();
    WriteElement("Ilwis", "Type", "GeoRef");
  }
  else
    WriteBaseInfo("GeoRef");  
  WriteElement("GeoRef", "CoordSystem", cs());
  WriteElement("GeoRef", "Lines", rcSize().Row);
  WriteElement("GeoRef", "Columns", rcSize().Col);
}

GeoRefPtr::GeoRefPtr(const FileName& fn)
: IlwisObjectPtr(fn)
{
  if (fnObj.sExt != ".grf")
    ReadBaseInfo("GeoRef");  
  ReadElement("GeoRef", "CoordSystem", _csys);
  _rc.Row = iReadElement("GeoRef", "lines");
  _rc.Col = iReadElement("GeoRef", "columns");
}

GeoRefPtr::GeoRefPtr(const FileName& fn, RowCol rc)
: IlwisObjectPtr(fn, true, ".grf")
{
 _rc = rc;
}

GeoRefPtr::GeoRefPtr(const FileName& fn, const CoordSystem& cs, RowCol rc)
: IlwisObjectPtr(fn, true, ".grf")
{
  _csys = cs;
  _rc = rc;
}

GeoRefPtr::~GeoRefPtr()
{
  if (fErase && (fnObj.sExt != ".grf")) {
    WriteElement("GeoRef", (const char*)0, (const char *)0);
    fErase = false;
  } 
}

String GeoRefPtr::sName(bool fExt, const String& sDirRelative) const
{
  String s = IlwisObjectPtr::sName(fExt, sDirRelative);
  if (s.length() != 0)
    return s;
  if (fExt)
    return String("none.grf");
  else
    return String("none");
 //Unknown(%li,%li)", rcSize().Row, rcSize().Col);
}

GeoRefPtr* GeoRefPtr::create(const FileName& fn)
{
  FileName filnam = fn;
  if (!File::fExist(filnam)) { // check std dir
    filnam.Dir(getEngine()->getContext()->sStdDir());
    if (!File::fExist(filnam))
      NotFoundError(fn);
      //throw ErrorNotFound(fn);
  }
  String sType;
  GeoRefPtr* p = GeoRef::pGet(fn);
  if (p) // if already open return it
    return p;
  MutexFileName mut(fn);
  p = GeoRef::pGet(fn);
  if (p) // if already open return it
    return p;

  ObjectInfo::ReadElement("GeoRef", "Type", filnam, sType);
  if (fCIStrEqual("GeoRefSmpl" , sType))
    return new GeoRefSmpl(filnam);
  if (fCIStrEqual("GeoRefCorners" , sType))
    return new GeoRefCorners(filnam);
  if (fCIStrEqual("GeoRefCornersWMS" , sType))
    return new GeoRefCornersWMS(filnam);
  if (fCIStrEqual("GeoRefCTP" , sType))
    return new GeoRefCTPplanar(filnam);
  if (fCIStrEqual("GeoRefOrthoPhoto" , sType))
    return new GeoRefOrthoPhoto(filnam);
  if (fCIStrEqual("GeoRefDirectLinear" , sType))
    return new GeoRefDirectLinear(filnam);
  if (fCIStrEqual("GeoRefParallelProjective" , sType))
    return new GeoRefParallelProjective(filnam);
  if (fCIStrEqual("GeoRefNone" , sType))
    return new GeoRefNone(filnam);
  if (fCIStrEqual("GeoRefFactor" , sType))
    return new GeoRefFactor(filnam);
  if (fCIStrEqual("GeoRefMirrorRotate" , sType))
    return new GeoRefMirrorRotate(filnam);
	if (fCIStrEqual("GeoRefScaleRotate" , sType))
    return new GeoRefScaleRotate(filnam);
  if (fCIStrEqual("GeoRef3D" , sType))
    return new GeoRef3D(filnam);
	if (fCIStrEqual("GeoRefStereoMate" , sType))
    return new GeoRefStereoMate(filnam);
  if (fCIStrEqual("GeoRefSubMap" , sType.sLeft(12)))
    return GeoRefSubMap::create(filnam);
  InvalidTypeError(fn, "GeoRef", sType);
  //throw ErrorInvalidType(fn, "GeoRef", sType);
  return 0;
}

GeoRefPtr* GeoRefPtr::create(const FileName& fn, const String& sExpression)
{
  String sFunc = IlwisObjectPtr::sParseFunc(sExpression);
//  if (sFunc == sUNDEF && fn.sFile.length() == 0)
//    return GeoRefPtr::create(FileName(sExpression,".grf",true));
  if (fCIStrEqual(sFunc, "Unknown"))
    return GeoRefNone::create(fn, sExpression);
  if (fCIStrEqual(sFunc, "GeoRefSmpl"))
    return GeoRefSmpl::create(fn, sExpression);
  if (fCIStrEqual(sFunc, "GeoRefCorners"))
    return GeoRefCorners::create(fn, sExpression);
  NotFoundError(String("GeoRef: %S", sExpression));
  //throw ErrorNotFound(String("GeoRef: %S", sExpression));

  return NULL;
}

double GeoRefPtr::rPixSize() const
{
  return rUNDEF;
}

void GeoRefPtr::Coord2RowCol(const Coord&, double& rR, double& rC) const
{
  rC = rUNDEF;
  rR = rUNDEF;
}

void GeoRefPtr::RowCol2Coord(double, double, Coord& c) const
{
  c.x = rUNDEF;
  c.y = rUNDEF;
}

CoordBounds GeoRefPtr::cb() const
{
  CoordBounds cb;
  if (rcSize().fUndef())
    return cb;
  Coord crd;
  RowCol2Coord(0,0,crd);
  cb += crd;
  RowCol2Coord(0,rcSize().Col,crd);
  cb += crd;
  RowCol2Coord(rcSize().Row,0,crd);
  cb += crd;
  RowCol2Coord(rcSize().Row,rcSize().Col,crd);
  cb += crd;
  return cb;
}

bool GeoRefPtr::fEqual(const IlwisObjectPtr& ptr) const
{
  const GeoRefPtr* grp = dynamic_cast<const GeoRefPtr*>(&ptr);
  if (grp)
    return rcSize() == grp->rcSize();
  else
    return false;
}

void GeoRefPtr::Rotate(bool/* fSwapRows*/, bool /*fSwapCols*/, bool fRotate)
{
  if (fRotate)
    SetRowCol(RowCol(_rc.Col, _rc.Row));
  Updated();
}


GeoRef GeoRef::grFindSameOnDisk(const GeoRef& gr, const String& sSearchDir)
{
  HANDLE findHandle;
  WIN32_FIND_DATA findData;
  String sSearchFile;
  if (sSearchDir.length())
    sSearchFile = sSearchDir;
  else
    sSearchFile = getEngine()->sGetCurDir();
  sSearchFile &= "*.grf";
  if ((findHandle=FindFirstFile(sSearchFile.c_str(), &findData))!= (void *)INVALID_HANDLE_VALUE)
  {  
    do 
    {
        String s(findData.cFileName);
        FileName fn(s);
        if (sSearchDir.length() != 0)
        fn.Dir(sSearchDir);
        try {
            GeoRef grExist(fn);
            if (grExist == gr)
                return grExist;
        }
        catch (const ErrorObject&) {
        }  
    } while (FindNextFile(findHandle, &findData)==TRUE);
    FindClose(findHandle);
  }
  return GeoRef();
}

void GeoRefPtr::SetCoordSystem(const CoordSystem& cs)
{
  _csys = cs;
  Updated();
}

bool GeoRefPtr::fGeoRefNone() const
{
   return false;
}

GeoRefSmpl* GeoRefPtr::pgsmpl() const
{
  return dynamic_cast<GeoRefSmpl*> (const_cast<GeoRefPtr*>(this));
}

GeoRefCorners* GeoRefPtr::pgc() const
{
  return dynamic_cast<GeoRefCorners*> (const_cast<GeoRefPtr*>(this));
}

GeoRef3D* GeoRefPtr::pg3d() const
{
  return dynamic_cast<GeoRef3D*> (const_cast<GeoRefPtr*>(this));
}

GeoRefCTP* GeoRefPtr::pgCTP() const
{
  return dynamic_cast<GeoRefCTP*> (const_cast<GeoRefPtr*>(this));
}

GeoRefCTPplanar* GeoRefPtr::pgCTPplanar() const
{
  return dynamic_cast<GeoRefCTPplanar*>(const_cast<GeoRefPtr*>(this));
}

GeoRefOrthoPhoto* GeoRefPtr::pgOrthoPhoto() const
{
  return dynamic_cast<GeoRefOrthoPhoto*>(const_cast<GeoRefPtr*>(this));
}

GeoRefDirectLinear* GeoRefPtr::pgDirectLinear() const
{
  return dynamic_cast<GeoRefDirectLinear*>(const_cast<GeoRefPtr*>(this));
}

GeoRefParallelProjective* GeoRefPtr::pgParallelProjective() const
{
  return dynamic_cast<GeoRefParallelProjective*>(const_cast<GeoRefPtr*>(this));
}

GeoRefFactor* GeoRefPtr::pgFac() const
{
	return dynamic_cast<GeoRefFactor*>(const_cast<GeoRefPtr*>(this));
}

GeoRefSubMap* GeoRefPtr::pgSub() const
{
	return dynamic_cast<GeoRefSubMap*>(const_cast<GeoRefPtr*>(this));
}

GeoRefMirrorRotate* GeoRefPtr::pgMirRot() const
{
	return dynamic_cast<GeoRefMirrorRotate*>(const_cast<GeoRefPtr*>(this));
}

GeoRefEpipolar* GeoRefPtr::pgEpipolar() const
{
	return dynamic_cast<GeoRefEpipolar*>(const_cast<GeoRefPtr*>(this));
}

GeoRefStereoMate* GeoRefPtr:: pgStereoMate() const
{
	return dynamic_cast<GeoRefStereoMate*>(const_cast<GeoRefPtr*>(this));
}   

GeoRefScaleRotate* GeoRefPtr::pgScalRot() const
{
	return dynamic_cast<GeoRefScaleRotate*>(const_cast<GeoRefPtr*>(this));
}

GeoRefCornersWMS* GeoRefPtr::pgWMS() const
{
	return dynamic_cast<GeoRefCornersWMS*>(const_cast<GeoRefPtr*>(this));
}

void GeoRefPtr::GetObjectDependencies(Array<FileName>& afnObjDep)
{
  String sType;
  ReadElement("GeoRef", "Type", sType);
  GeoRef gr;
  ReadElement(sType.c_str(), "GeoRef", gr);
  if (gr.fValid())
    afnObjDep &= gr->fnObj;
}

bool GeoRefPtr::fDependent() const
{
  return false;
}

bool GeoRefPtr::fNorthOriented() const
{
  return false;
}

void GeoRefPtr::Rename(const FileName& fnNew)
{
}

bool GeoRefPtr::fLinear() const
{
  return false;
}

String GeoRefPtr::sObjectSection() const
{
  if (fnObj.sExt != ".grf")
    return "GeoReference";
  return IlwisObjectPtr::sObjectSection();
}

void GeoRefPtr::DoNotUpdate()
{
	IlwisObjectPtr::DoNotUpdate();
	
}

void GeoRefPtr::GetObjectStructure(ObjectStructure& os)
{
	IlwisObjectPtr::GetObjectStructure( os );
	if ( os.fGetAssociatedFiles() )
	{
		os.AddFile(fnObj, "GeoRef", "CoordSystem");		
	}		
	
}



