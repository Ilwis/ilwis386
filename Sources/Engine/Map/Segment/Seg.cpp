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
/* 
// $Log: /ILWIS 3.0/SegmentMap/Seg.cpp $
 * 
 * 36    19-05-05 17:57 Retsios
 * [bug=6424] Properly open the maps in case the supplied attribute column
 * contains spaces. Added a safeguard to p to reduce the chance of reading
 * characters outside the string bounds.
 * 
 * 35    12/05/01 10:27 Willem
 * The histogram of all maptypes is now also copied or deleted with the
 * map it belongs to
 * 
 * 34    8/24/01 13:03 Willem
 * Removed the SetReadOnly() function. This is now handled by
 * IlwisObjectPtr::SetReadOnly() for all ilwis objects
 * 
 * 33    1/03/01 14:32 Willem
 * The DomainValueRangeStruct and ValueRange for the segment map now also
 * set the ValueRange of the proper TableSegment Column as well to have
 * the domain of the segmentmap and that of the TableSegment Column
 * synchronized.
 * 
 * 32    19-02-01 1:56p Martin
 * SetCoordSystem functions are now protected agianst the not existing of
 * a store map. (e.g. GDB)
 * 
 * 31    13-02-01 8:28a Martin
 * when changing the coordsystem of a map the coordsystem of the internal
 * tables (columns) is also changed
 * 
 * 30    2/08/01 12:18 Retsios
 * Solved some potential "dot in path confused for extension's dot" bugs
 * 
 * 29    29-01-01 8:55a Martin
 * deleted the foreignformat object after use
 * 
 * 28    1-12-00 15:44 Koolhoven
 * removed function SegmentMapPtr::crdBounds() because it is a duplicate
 * of BaseMapPtr::cbGetCoordBounds()
 * 
 * 27    24-11-00 12:29 Koolhoven
 * Added Pack() option for segments
 * 
 * 26    21-11-00 15:39 Hendrikse
 * in Open...MapVirtual() load ObjectDependcency after the creation of the
 * ...MapVirtual. This prevents a.o.a problems with internal tables and
 * columns (...MapAttribute)
 * 
 * 25    20-11-00 3:25p Martin
 * the foreignformat file for getobjectstructure() is now in
 * IlwisObject::GetObjectsr..
 * 
 * 24    27-10-00 10:20a Martin
 * added foreignformat files to the getobjectstructure
 * 
 * 23    24-10-00 1:55p Martin
 * changed getobjectstructure function
 * 
 * 22    27-09-00 3:18p Martin
 * files using geogateway may now adjust their coordbounds after all data
 * are read (layerinfo returns). It seems GDB sometimes calculates other
 * coordbounds than actual used.
 * 
 * 21    12-09-00 1:04p Martin
 * prevented automatic conversion (on disk) to 3.0 as long as no changes
 * are made. Delete old datafiles if converted to 3.0
 * 
 * 20    11-09-00 11:30a Martin
 * added function for objectStructure 
 * 
 * 19    27-04-00 12:47 Koolhoven
 * In UndoAllChanges() set fChanged of table on false to prevent Store()
 * (seemed not needed, but by analogy of PointMap done just to be sure)
 * 
 * 18    25-04-00 17:57 Koolhoven
 * Added UndoAllChanges()
 * 
 * 17    25-04-00 17:15 Koolhoven
 * Better default snapping tolerance and tunnel width
 * 
 * 16    25-04-00 16:25 Koolhoven
 * Removed Pack() function
 * 
 * 15    15/03/00 10:29 Willem
 * Renamed SegmentMapStoreFormat14.* into SegmentMapStoreFormat20.*
 * 
 * 14    9-03-00 8:40a Martin
 * tblptr to tblptr use in LayerInfo, better use of tbltop
 * 
 * 13    1-03-00 12:43p Martin
 * cbOuter is filled by foreign format
 * 
 * 12    28-02-00 8:49a Martin
 * Segmentmap can now read foreign formats
 * 
 * 11    17-01-00 11:30 Wind
 * added proximity to iValue(const Coord& ..) etc.
 * 
 * 10    17-01-00 8:17a Martin
 * changed rowcols to coords
 * 
 * 9     10-12-99 11:48a Martin
 * removed internal rowcols and replaced them by true coords
 * 
 * 8     29-10-99 9:20 Wind
 * thread save stuff
 * 
 * 7     25-10-99 13:14 Wind
 * making thread save (2); not yet finished
 * 
 * 6     22-10-99 12:54 Wind
 * thread save access (not yet finished)
 * 
 * 5     9/24/99 10:37a Wind
 * replaced calls to static funcs ObjectInfo::ReadElement and WriteElement
 * by calls to member functions
 * 
 * 4     9/08/99 12:02p Wind
 * comments
 * 
 * 3     9/08/99 10:27a Wind
 * adpated to use of quoted file names
 * 
 * 2     3/11/99 12:17p Martin
 * Added support for Case insesitive 
*/
// Revision 1.11  1998/09/16 17:25:20  Wim
// 22beta2
//
// Revision 1.10  1997/09/22 15:02:51  Willem
// With a small CoordBounds the rAlfa can become very small. If it gets smaller than 1e-6
// the store function does not store the value correctly: it stores only the first
// 6 decimals. A check is made to ensure the rAlfa to be larger than 1e-6 at all times.
//
// Revision 1.9  1997/09/11 18:40:41  Wim
// Allow to break dependencies even when source objects are gone
//
// Revision 1.8  1997-09-09 14:02:07+02  Wim
// In Store() make sure the map remains dependent even when
// the virtual map has not been loaded into memory
//
// Revision 1.7  1997-09-04 12:45:58+02  Wim
// Delete Histogram in DeleteCalc() function
//
// Revision 1.6  1997-08-28 16:47:47+02  martin
// *** empty log message ***
//
// Revision 1.5  1997/08/25 12:56:23  martin
// *** empty log message ***
//
// Revision 1.4  1997/08/22 09:12:53  Wim
// After constructing a new mapptr call Store()
//
// Revision 1.3  1997-08-19 16:50:41+02  Wim
// Added BaseMap::pGetRasMap(), pGetSegMap(), pGetPolMap(), pGetPntMap()
// because all basemapptr's are in the same list.
// This caused problems with polygonmapstore which has a segmentmap
// inside, with the same filename.
// Due to the static casts you got some "funny" results.
//
// Revision 1.2  1997-07-28 17:07:07+02  Wim
// Default number of segments is iUNDEF
//
/* SegmentMap
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  WK    8 Jun 98    6:39 pm
*/
#define ILWSEGMAP_C
#include "Engine\Map\Segment\Seg.h"
#include "Engine\Map\Segment\SEGSTORE.H"
#include "Engine\DataExchange\ForeignFormat.h"
#include "Engine\DataExchange\SegmentMapStoreForeign.h"
#include "Engine\Applications\SEGVIRT.H"
#include "Engine\Base\DataObjects\ObjectStructure.h"
#include "Engine\Domain\Dmvalue.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Base\Algorithm\Tunnel.h"
#include "Engine\Base\Algorithm\Clipline.h"
#include "Engine\Base\System\mutex.h"
#include "SegmentApplications\SEGATTRB.H"
#include "Engine\Table\TBLHSTSG.H"
#include "Engine\Base\File\COPIER.H"

#define EPS10 1.e-10

FileName fnFromExpression(const String& sExpr)
{
  char *p = sExpr.strrchrQuoted('.');
  // Bas Retsios, 8 February 2001: *p is not necessarily an extension's dot
  char *q = sExpr.strrchrQuoted('\\');
  // *p is definitely not an extension's dot if p<q
  if (p<q)
  	p = 0;
  // Now we're (only) sure that if p!=0 it points to the last dot in the filename
  if ((p == 0) || (0 == _strcmpi(p, ".mps")))  // no attrib map
    return FileName(sExpr, ".mps", true);
  return FileName();
}

// for the time being the interface works with the 1.4 datastructures
// this will soon change to a more structured tableinterface

SegmentMap::SegmentMap(const String& sExpression, const String& sPath)
: BaseMap()
{
 // check if it's a potential attribute map
  String sExpr =  sExpression;
  char *p = sExpr.strrchrQuoted('.');
  // Bas Retsios, 8 February 2001: *p is not necessarily an extension's dot
	char *pMax = (sExpr.length() > 0) ? (const_cast<char*>(&(sExpr)[sExpr.length() - 1])) : 0; // last valid char in sExpr
  char *q = sExpr.strrchrQuoted('\\');
  // *p is definitely not an extension's dot if p<q
  if (p<q || p>=pMax)
  	p = 0;
  // Now we're (only) sure that if p!=0 it points to the last dot in the filename
  if (p)
    *p = 0;
  String sFile = sExpr.scVal();
  FileName fn(sFile, ".mps", true);
  if (0 == strchr(sFile.scVal(), ':')) // no path set
    fn.Dir(sPath); 
  MutexFileName mut(fn);
  if (p && (0 != _strcmpi(p+1, "mps"))) {
    if (isalpha(*(p+1)) || ((p<(pMax-1)) && *(p+1) == '\'' && isalpha(*(p+2)))) // attrib column (right side of || checks on quoted column)
//      SetPointer(new SegmentMapAttribute(FileName(), SegmentMap(fn), p+1));
      SetPointer(SegmentMapPtr::create(FileName(), String("SegmentMapAttribute(%S,%s)", fn.sFullNameQuoted(), p+1)));
    else {
      fn.sFile = "";
      fn.sExt = "";
      SetPointer(SegmentMapPtr::create(fn, sExpression));
    }  
  }    
  else  
    SetPointer(SegmentMapPtr::create(fn));
}

SegmentMap::SegmentMap(const FileName& fn)
: BaseMap() //FileName(fn, ".MPS", false))
{
  FileName fnSeg(fn, ".mps", false);
  MutexFileName mut(fnSeg);
  SetPointer(SegmentMapPtr::create(fnSeg));
}

SegmentMap::SegmentMap()
: BaseMap()
{
}

SegmentMap::SegmentMap(const FileName& fnSegmentMap,LayerInfo li) :
	BaseMap()
{
	SetPointer(new SegmentMapPtr(fnSegmentMap, li));
}

SegmentMap::SegmentMap(const FileName& fn, const String& sExpression)
: BaseMap() 
{
  FileName fnSeg(fn, ".mps", false);
  MutexFileName mut(fnSeg);
  SetPointer(SegmentMapPtr::create(fnSeg, sExpression));
//if (fValid())
//  ptr()->Store();
}

SegmentMapPtr* SegmentMapPtr::create(const FileName& fn)
{
  if (!File::fExist(fn))
    NotFoundError(fn);
  MutexFileName mut(fn);
  SegmentMapPtr* p = BaseMap::pGetSegMap(fn);
  if (p) // if already open return it
    return p;
  p = new SegmentMapPtr(fn);
  return p;
}

SegmentMapPtr* SegmentMapPtr::create(const FileName& fn, const String& sExpression)
{
  if (fn.sFile.length() == 0) { // no file name
    // check if sExpression is an existing point map on disk
    // check if it's a potential attribute map
    char *p = sExpression.strrchrQuoted('.');
    // Bas Retsios, 8 February 2001: *p is not necessarily an extension's dot
    char *q = sExpression.strrchrQuoted('\\');
    // *p is definitely not an extension's dot if p<q
    if (p<q)
    	p = 0;
    // Now we're (only) sure that if p!=0 it points to the last dot in the filename
    if ((p == 0) || (0 == _strcmpi(p, ".mps"))) { // no attrib map
      FileName fnMap(sExpression, ".mps", false);
      if (File::fExist(fnMap)) {
        // see if map is already open (if it is in the list of open base maps)
        MutexFileName mut(fnMap);
        SegmentMapPtr* p = BaseMap::pGetSegMap(fnMap);
        if (p) // if already open return it
          return p;
        // open map and return it
        return SegmentMapPtr::create(fnMap);
      }  
    }
  }
  bool fPrevExist = File::fExist(fn);
  SegmentMapPtr* p = new SegmentMapPtr(fn, true);
  p->pmv = SegmentMapVirtual::create(fn, *p, sExpression);
  if ((0 != p->pmv) && fPrevExist) {
    SegmentMapStore::UnStore(fn);
/// unlink(fn.sFullName(true)); // delete previous object def file
    ObjectInfo::WriteElement("ObjectDependency", (char*)0, fn, (char*)0);
    ObjectInfo::WriteElement("SegmentMapVirtual", (char*)0, fn, (char*)0);
    TableHistogramSeg::Erase(fn); // delete previous histogram
  }  
  if (0 != p->pmv) {
    p->SetDomainChangeable(p->pmv->fDomainChangeable());
    p->SetValueRangeChangeable(p->pmv->fValueRangeChangeable());
    p->SetExpressionChangeable(p->pmv->fExpressionChangeable());
  }
  return p;
}

SegmentMapPtr::SegmentMapPtr(const FileName& fnSegmentMap, LayerInfo inf) :
	BaseMapPtr(),
  pmv(0),
	pms(0)
{
	double rSize = max(cb().width(), cb().height());
	rSnapDist = rSize / 100;
	rTunnelWidth = rSnapDist / 10;
	SetCoordSystem(inf.csy);
	SetDomainValueRangeStruct(inf.dvrsMap);
	if ( !inf.tblTop.fValid()) // else it is the segmap belonging to a polmap, no attrtable there
	{
		SetAttributeTable(inf.tblattr);
	}
	SetFileName(inf.fnObj);
	cbOuter = inf.cbMap;

	pms = new SegmentMapStoreForeign(fnSegmentMap, *this, inf);

	_iSeg = inf.tblTop.fValid() ? inf.tblTop->iRecs() : inf.tbl->iRecs();
}

SegmentMapPtr::SegmentMapPtr(const FileName& fn, bool fCreate)
: BaseMapPtr(fn, fCreate), pms(0), pmv(0)
{
  _iSeg = _iSegDeleted = _iCrd = 0;

	double rSize = max(cb().width(), cb().height());
  rSnapDist = rReadElement("SegmentMap", "Snap Distance");
  if (rSnapDist <= 0)
	  rSnapDist = rSize / 100;
  rTunnelWidth = rReadElement("SegmentMap", "Tunnel Width");
  if (rTunnelWidth <= 0)
	  rTunnelWidth = rSnapDist / 10;
  if (sDescription.length() == 0) // downward compatibility with 2.02
    ReadElement("SegmentMapStore", "Expression", sDescription);
  String s;

	DoNotUpdate();
	DoNotStore(true);
  if (0 != ReadElement("SegmentMapStore", (char*)0, s))
  {
	  ILWIS::Version::BinaryVersion fvFormat = ILWIS::Version::bvUNKNOWN;
    ReadElement("SegmentMapStore", "Format", (int &)fvFormat);
		if ( fCreate ) fvFormat = ILWIS::Version::bvFORMAT30;

		switch ( fvFormat)
		{
		case ILWIS::Version::bvFORMAT30:
				pms = new SegmentMapStore(fn, *this, fCreate);
				break;
		case ILWIS::Version::bvUNKNOWN:
		case ILWIS::Version::bvFORMAT14:
		case ILWIS::Version::bvFORMAT20:
				//pms = new SegmentMapStoreFormat20(fn, *this);
				break;
		case ILWIS::Version::bvFORMATFOREIGN:
			{
				ParmList pm;
				ForeignFormat *ff = ForeignFormat::Create(fn, pm);
				LayerInfo inf = ff->GetLayerInfo(this, ForeignFormat::mtSegmentMap, false);
				pms = new SegmentMapStoreForeign(fn, *this, inf);
				delete ff;
			}
			break;
		}
	
	}
	DoNotStore(false);
	DoNotUpdate();
//if (fDependent())
//  _fDataReadOnly = true;
}

SegmentMapPtr::SegmentMapPtr(const FileName& fn, const CoordSystem& cs,
                             const CoordBounds& cb, const DomainValueRangeStruct& dvs)
: BaseMapPtr(fn, cs, cb, dvs), pms(0), pmv(0)
{
	double rSize = max(cb.width(), cb.height());
  rSnapDist = rSize / 100;
  rTunnelWidth = rSnapDist / 10;
  fChanged = true;
  _iSeg = _iSegDeleted = _iCrd = 0;

  ILWIS::Version::BinaryVersion fvFormat = ILWIS::Version::bvUNKNOWN;
  ReadElement("SegmentMapStore", "Format", (int &)fvFormat);
	if ( fvFormat == shUNDEF ) fvFormat = ILWIS::Version::bvFORMAT30;

	switch ( fvFormat)
	{
		case ILWIS::Version::bvFORMAT30:
				pms = new SegmentMapStore(fn, *this, true);
				break;
		case ILWIS::Version::bvUNKNOWN:
		case ILWIS::Version::bvFORMAT14:
		case ILWIS::Version::bvFORMAT20:
				//pms = new SegmentMapStoreFormat20(fn, *this);
				break;
	}
  //pms = new SegmentMapStore(fn, *this, true); // create
}

SegmentMapPtr::~SegmentMapPtr()
{
  if (fErase) {
    FileName fnHis = FileName(fnObj, ".hss", true);
    if (File::fExist(fnHis)) {
      try { 
        Table his(fnHis);
        his->fErase = true;
      }
      catch (const ErrorObject&) {
      }  
    }  
  } 
  if (0 != pms) {
    delete pms;
    pms = 0;
  }  
  if (0 != pmv) {
    delete pmv;
    pmv = 0;
  }  
}

void SegmentMapPtr::Load()
{
  _iSeg = _iSegDeleted = _iCrd = 0;

	double rSize = max(cb().width(), cb().height());
  rSnapDist = rReadElement("SegmentMap", "Snap Distance");
  if (rSnapDist <= 0)
	  rSnapDist = rSize / 100;
  rTunnelWidth = rReadElement("SegmentMap", "Tunnel Width");
  if (rTunnelWidth <= 0)
	  rTunnelWidth = rSnapDist / 10;
  if (sDescription.length() == 0) // downward compatibility with 2.02
    ReadElement("SegmentMapStore", "Expression", sDescription);
  String s;

  if (0 != ReadElement("SegmentMapStore", (char*)0, s))
  {
	  ILWIS::Version::BinaryVersion fvFormat = ILWIS::Version::bvUNKNOWN;
        ReadElement("SegmentMapStore", "Format", (int &)fvFormat);
		FileName fn = fnObj;

		switch ( fvFormat)
		{
		case ILWIS::Version::bvFORMAT30:
				pms = new SegmentMapStore(fn, *this);
				break;
		case ILWIS::Version::bvUNKNOWN:
		case ILWIS::Version::bvFORMAT14:
		case ILWIS::Version::bvFORMAT20:
				//pms = new SegmentMapStoreFormat20(fn, *this);
				break;
		case ILWIS::Version::bvFORMATFOREIGN:
			{
				ParmList pm;
				ForeignFormat *ff = ForeignFormat::Create(fn, pm);
				LayerInfo inf = ff->GetLayerInfo(this, ForeignFormat::mtSegmentMap, false);
				pms = new SegmentMapStoreForeign(fn, *this, inf);
			}
			break;
		}
	}
}

void SegmentMapPtr::Store()
{
  ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
  bool fDep = fDependent();
  if ((0 != pmv) && (sDescription == ""))
    sDescription = pmv->sExpression();
  BaseMapPtr::Store();
  WriteElement("BaseMap", "Type", "SegmentMap");
  WriteElement("SegmentMap", "Snap Distance", rSnapDist);
  WriteElement("SegmentMap", "Tunnel Width", rTunnelWidth);
  if (0 != pms)
    pms->Store();
  else
    SegmentMapStore::UnStore(fnObj);
  if (fDep)
    WriteElement("SegmentMap", "Type", "SegmentMapVirtual");
  if (0 != pmv)
    pmv->Store();
  String s;
  if (0 == ReadElement("SegmentMap", "Type", s))
    WriteElement("SegmentMap", "Type", "SegmentMapStore");
}

void SegmentMapPtr::SetNumberOfCoords(long iNr)
{
	if ( iNr == iUNDEF || iNr >= 0)
		_iCrd = iNr;
}

void SegmentMapPtr::SetNumberOfSegments(long iNr)
{
	if ( iNr == iUNDEF || iNr >= 0)
		_iSeg = iNr;
}

void SegmentMapPtr::SetNumberOfDeletedSegments(long iNr)
{
	if ( iNr == iUNDEF || iNr >= 0)
		_iSegDeleted = iNr;
}

void SegmentMapPtr::SetDomainValueRangeStruct(const DomainValueRangeStruct& dvsNew)
{
	BaseMapPtr::SetDomainValueRangeStruct(dvsNew);

}

void SegmentMapPtr::SetValueRange(const ValueRange& vrNew)
{
	BaseMapPtr::SetValueRange(vrNew);

}

bool SegmentMapPtr::fSatisfyCondition(const ILWIS::Segment& /*seg*/) const
{
  return true;
}  

void SegmentMapPtr::SetAlfa(const CoordBounds& cb)
{ 
}

vector<long> SegmentMapPtr::viRaw(const Coord& crd, double rPrx) const
{
  ILWIS::Segment *s = seg(crd, rPrx);
  vector<long> raws;
  if ( !s || !s->fValid())
    return raws;
  else {
	 if(dvs.fRawAvailable())
		 raws.push_back(s->iValue());
	 if ( dvs.fValues())
		 raws.push_back(dvs.iRaw(s->iValue()));
  }

  return raws;
}

vector<long> SegmentMapPtr::viValue(const Coord& crd, double rPrx) const
{
  ILWIS::Segment *s = seg(crd, rPrx);
  vector<long> vals;
  if ( !s || !s->fValid())
    return vals;
  else {
	 if(dvs.fRawAvailable())
		 vals.push_back(dvs.iValue(s->iValue())) ;
	 if ( dvs.fValues())
		 vals.push_back(s->iValue());
  }
  return vals;
}

vector<double> SegmentMapPtr::vrValue(const Coord& crd, double rPrx) const
{
 ILWIS::Segment *s = seg(crd, rPrx);
 vector<double> vals;
  if ( !s || !s->fValid())
    return vals;
  else   {
	 if(dvs.fRawAvailable())
		 vals.push_back(dvs.rValue(s->rValue())) ;
	 if ( dvs.fValues())
		 vals.push_back(s->rValue());
  }

  return vals;
}

ILWIS::Segment *SegmentMapPtr::seg(const Coord& crd, double rPrx) const 
{
  ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
  ILWIS::Segment *sg = NULL;
  long dummy;
  Coord crdFound = crdPoint(crd,&sg,dummy);
  if (!sg->fValid())
    return sg;
  double rD = rPrx == rUNDEF ? rSnapDist : rPrx;
  if (rDist(crd, crdFound) < rD)
    return sg;  
  return NULL;  
}

vector<String> SegmentMapPtr::vsValue(const Coord& crd, short iWidth, short, double rPrx) const
{
  ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
  vector<String> strings;
  ILWIS::Segment *sg = seg(crd, rPrx);
  if (!sg || !sg->fValid())
    return strings;
  String s = sg->sValue(dvrs());
  strings.push_back(s)	;
  if (iWidth == 0) {
	  if (iWidth < 0) {
		  iWidth = dm()->iWidth();
		  strings[0] = s.sLeft(iWidth);
	  }
  }
  return strings;
}

ILWIS::Segment *SegmentMapPtr::getFeature(const String& s)
{
  ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
  if ( pms) {
	  pms->seg(s);
  }
  return NULL;
}

SegmentMap::SegmentMap(const String& sExpression)
: BaseMap()
{
  FileName fnSeg = fnFromExpression(sExpression); 
  MutexFileName mut(fnSeg);
  if (fnSeg.fValid())
    SetPointer(SegmentMapPtr::create(fnSeg));
  else 
    SetPointer(SegmentMapPtr::create(FileName(),sExpression));
}

SegmentMap::SegmentMap(const FileName& fn, const CoordSystem& cs,
                       const CoordBounds& cb, const DomainValueRangeStruct& dvs)
{
  FileName fnSeg(fn, ".mps", false);
  MutexFileName mut(fnSeg);
  SetPointer(new SegmentMapPtr(fnSeg, cs, cb, dvs));
}


void SegmentMapPtr::BreakDependency()
{
//  OpenSegmentMapVirtual();
//  if (0 == pmv)
//    return; // already done
  if (!fCalculated())
    Calc();
  if (!fCalculated())
    return; 
  ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
  delete pmv;
  pmv = 0;
  fChanged = true;
//_fDataReadOnly = false;
  WriteElement("SegmentMap", "Type", "SegmentMapStore");
  Store();
}

void SegmentMap::Export(const FileName& fn) const
{
  if (!ptr())
    return;
  ptr()->Export(fn);
}

void SegmentMapPtr::CalcMinMax()
{
  ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
  if (fnObj.sFile.length() == 0) {
    _rrMinMax = dvrs().rrMinMax();
    _rrPerc1 = dvrs().rrMinMax();
    _riMinMax = dvrs().riMinMax();
    _riPerc1 = dvrs().riMinMax();
    return;
  }
  if (dm()->pdbit() || (dm()->fnObj.sFile == "bool")/*not a nice check*/) {
    _rrMinMax = RangeReal(0,1);
    _riMinMax = RangeInt(0,1);
    return;
  }
  Table his;
  FileName fnHis(fnObj, ".hss", true);
  if (File::fExist(fnHis))
    his = Table(fnHis);
  else
    his = Table(fnHis, String("TableHistogramSeg(%S)", sNameQuoted()));
  his->Calc(); // sets 0 and 1 perc interval
}

String SegmentMapPtr::sType() const
{
  ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
  if (0 != pmv)
    return pmv->sType();
  else if (fDependent())
    return "Dependent ILWIS::Segment Map";
  else
    return "ILWIS::Segment Map";
}

void SegmentMapPtr::Rename(const FileName& fnNew)
{
}

void SegmentMapPtr::GetDataFiles(Array<FileName>& afnDat, Array<String>* asSection, Array<String>* asEntry) const
{
  BaseMapPtr::GetDataFiles(afnDat, asSection, asEntry);
  ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
  if (0 != pms)
    pms->GetDataFiles(afnDat, asSection, asEntry);
}

void SegmentMapPtr::GetObjectDependencies(Array<FileName>& afnObjDep)
{
  ObjectDependency::Read(fnObj, afnObjDep);
}

void SegmentMapPtr::KeepOpen(bool f)
{
  ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
  BaseMapPtr::KeepOpen(f);
  if (0 != pms)
    pms->KeepOpen(f);
}

void SegmentMapPtr::Export(const FileName& fn) const
{
  ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
  if (0 != pmv)
    pmv->Freeze();
  if (0 != pms)
    pms->Export(fn);
}

bool SegmentMapPtr::fConvertTo(const DomainValueRangeStruct& dvs, const Column& col)
{
  ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
  if (col.fValid() && col->dvrs() == dvs)
    return false;
  else if (dvrs() == dvs)
    return false;
  bool f = false;
  if (0 != pms)
    f = pms->fConvertTo(dvs, col);
  if (col.fValid())
    SetDomainValueRangeStruct(col->dvrs());
  else
    SetDomainValueRangeStruct(dvs);
  return f;
}

long SegmentMapPtr::iFeatures() const
{
  if (0 != pms)
    return pms->iSeg();
  return iUNDEF;
}

long SegmentMapPtr::iSegDeleted() const
{
  if (0 != pms)
    return pms->iSegDeleted();
  return 0;
}

ILWIS::Segment* SegmentMapPtr::newFeature(long iSegNr)
{
	ILWIS::Segment *s = NULL;
	if (0 != pms){
		int iEffectiveSegs= iFeatures() - iSegNr;
		for(int  i =0; i < iEffectiveSegs && iSegNr; ++i)
			s = CSEGMENT(pms->newFeature());
	}
	return s;
}

Feature* SegmentMapPtr::newFeature(geos::geom::Geometry *line)
{
  if (0 != pms)
    return pms->newFeature(line);
  return NULL;
}
 
bool SegmentMapPtr::fSegExist(const ILWIS::Segment& segNew, Tranquilizer* trq)
{
  if (0 != pms)
    return pms->fSegExist(segNew, trq);
  return false;
}


bool SegmentMapPtr::fSegExist(long iCrdNew, const CoordBuf& crdBufNew, const CoordBounds& crdBoundsNew, Tranquilizer* trq)
{
/*	MinMax mmbnd(rcConv(crdBoundsNew.cMin), rcConv(crdBoundsNew.cMax));
  RowColBuf buf(crdBufNew.iSize());
	for(int i = 0; i< buf.iSize(); ++i)
		buf[i] = rcConv(crdBufNew[i]);
	bool fRet = fSegExist(iCrdNew, buf, mmbnd, trq);*/

  return true;
}


ILWIS::Segment* SegmentMapPtr::segFirst(long& index, bool fAcceptDeleted) const
{
  if (0 != pms)
    return pms->segFirst(index, fAcceptDeleted);
  return NULL;
}

ILWIS::Segment* SegmentMapPtr::segFirst(bool fAcceptDeleted) const
{
  long index = 0;
  if (0 != pms)
    return pms->segFirst(index, fAcceptDeleted);
  return NULL;
}

ILWIS::Segment* SegmentMapPtr::segLast() const
{
  if (0 != pms)
    return pms->segLast();
  return NULL;
}

Geometry *SegmentMapPtr::getFeature(long i) const {

 if (0 != pms)
    return pms->seg(i);
  return NULL;
}

Geometry *SegmentMapPtr::getTransformedFeature(long iRec, const CoordSystem& csy) const {
	 if (0 != pms)
		 return pms->getTransformedFeature(iRec,csy);
	  return NULL;
}

long SegmentMapPtr::iNode(Coord crd) const
{
  if (0 != pms)
    return pms->iNode(crd);
  return iUNDEF;
}

Coord SegmentMapPtr::crdNode(Coord crd) const
{
  if (0 != pms)
    return pms->crdNode(crd);
  return crdUNDEF;	
}

Coord SegmentMapPtr::crdCoord(Coord crd, ILWIS::Segment** seg, long& iNr) const
{
  if (0 != pms)
    return pms->crdCoord(crd, seg, iNr);

	return Coord();
}

Coord SegmentMapPtr::crdPoint(Coord crd, ILWIS::Segment** seg, long& iAft, bool fAcceptDeleted) const
{
	if ( 0 != pms )
		return pms->crdPoint(crd, seg, iAft, fAcceptDeleted);

  return crdUNDEF;
}

bool SegmentMapPtr::fDependent() const
{
  if (0 != pmv)
    return true;
  String s;
  ReadElement("SegmentMap", "Type", s);
  return fCIStrEqual(s , "SegmentMapVirtual");
}

String SegmentMapPtr::sExpression() const
{
  if (0 != pmv)
    return pmv->sExpression();
  String s;
  s = IlwisObjectPtr::sExpression();
  if (s.length() == 0) // for downward compatibility with 2.02
    ReadElement("SegmentMapVirtual", "Expression", s);
  return s;
}

bool SegmentMapPtr::fCalculated() const
// returns true if a calculated result exists
{
  if (!fDependent())
    return IlwisObjectPtr::fCalculated();
  return 0 != pms;
} 
/*
bool SegmentMapPtr::fUpToDate() const
// returns true if an up to date calculated result exists
{
  if (!fDependent())
    return IlwisObjectPtr::fUpToDate();
  if (0 == pms)
    return false;
  OpenSegmentMapVirtual();
  if (0 == pmv)
    return false;
  if (!objdep.fUpToDate())
    return false;
  Time timDep = objdep.tmNewest();
  return timDep <= pms->timStore;
}
*/
bool SegmentMapPtr::fDefOnlyPossible() const
// returns true if data can be retrieved without complete calculation (on the fly)
{
  if (!fDependent())
    return IlwisObjectPtr::fDefOnlyPossible();
  bool f;
  if (0 != ReadElement("SegmentMapVirtual", "DefOnlyPossible", f))
    return f;
  return false;
}

void SegmentMapPtr::Calc(bool fMakeUpToDate)
// calculates the result     
{
  ILWISSingleLock sl(&csCalc, TRUE, SOURCE_LOCATION);
  OpenSegmentMapVirtual();
  if (fMakeUpToDate)
    if (!objdep.fUpdateAll())
      return;
  if (0 != pmv) 
      pmv->Freeze();
}

void SegmentMapPtr::DeleteCalc()
// deletes calculated  result     
{
  TableHistogramSeg::Erase(fnObj);
  if (0 == pms)
    return;
  ILWISSingleLock sl(&csCalc, TRUE, SOURCE_LOCATION);
  OpenSegmentMapVirtual();
  if (0 != pmv) 
    pmv->UnFreeze();
}

void SegmentMapPtr::OpenSegmentMapVirtual()
{
  if (0 != pmv) // already there
    return;
  if (!fDependent())
    return;
  try {
    pmv = SegmentMapVirtual::create(fnObj, *this);
    objdep = ObjectDependency(fnObj);
    String s;
    if (0 == ReadElement("IlwisObjectVirtual", (char*)0, s)) {
      // for downward compatibility with 2.02 :
      SetDomainChangeable(pmv->fDomainChangeable());
      SetValueRangeChangeable(pmv->fValueRangeChangeable());
      SetExpressionChangeable(pmv->fExpressionChangeable());
    }
  }
  catch (const ErrorObject& err) {
    err.Show();
    pmv = 0;
    objdep = ObjectDependency();
  }
}

void SegmentMapPtr::CreateSegmentMapStore()
{
  if (0 != pmv)
    pmv->CreateSegmentMapStore();
}             

void SegmentMapPtr::UndoAllChanges()
{
	delete pms;
	Load();
}

void SegmentMapPtr::GetObjectStructure(ObjectStructure& os)
{
	BaseMapPtr::GetObjectStructure(os);
	double  rVersion = rReadElement("Ilwis", "Version");
	if ( rVersion < 3.0)
	{
		os.AddFile(fnObj, "SegmentMapStore", "DataSeg");				
		os.AddFile(fnObj, "SegmentMapStore", "DataSegCode");						
		os.AddFile(fnObj, "SegmentMapStore", "DataCrd");								
	}
	else
	{
		os.AddFile(fnObj, "TableStore", "Data");
	}		
	FileName fnHist(fnObj, ".hss");
	if (File::fExist(fnHist) && IlwisObject::iotObjectType( fnHist ) != IlwisObject::iotANY)
	{
		IlwisObject obj = IlwisObject::obj(fnHist);
		if ( obj.fValid())
			obj->GetObjectStructure(os);
	}					
}

void SegmentMapPtr::DoNotUpdate()
{
	BaseMapPtr::DoNotUpdate();

}

void SegmentMapPtr::Pack()
{
	pms->Pack();
}

void SegmentMapPtr::SetCoordSystem(const CoordSystem& cs) // dangerous function!
{
	BaseMapPtr::SetCoordSystem(cs);
  ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
	if ( pms )
		pms->SetCoordSystem(cs);
}

vector<Geometry *> SegmentMapPtr::getFeatures(Coord crd, double rPrx) {
	ILWIS::Segment *fseg = seg(crd,rPrx);
	if ( !fseg)
		return vector<Geometry *>();
	vector<Geometry *> segs;
	segs.push_back(fseg);
	return segs;
}