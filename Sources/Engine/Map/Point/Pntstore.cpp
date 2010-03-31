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
// $Log: /ILWIS 3.0/PointMap/Pntstore.cpp $
 * 
 * 30    15-10-03 13:54 Retsios
 * Added function to fill the pointmap from a buffer (performance reason)
 * 
 * 29    2-05-03 9:34 Willem
 * - Changed: Simplified the table version check to its essentials
 * 
 * 28    29-04-03 17:35 Retsios
 * Some data contains [ilwis] version 2.2 and "Coordinate" column instead
 * of X/Y (product of ILWIS 3.0 to ILWIS 3.1x).
 * 
 * 27    21-01-03 16:55 Willem
 * - Added: Compatibility check: there seem to be maps with mixed
 * versioning: table info in version 3 format, including .pt# data file
 * but with an [ilwis] version=2.2 section and thus X and Y column instead
 * of Coordinate
 * 
 * 26    10-01-03 11:02 Hendrikse
 * added  SetCoordSystem(const CoordSystem&); cto be used by property
 * sheet when user changes csy of pointmap. It calls
 * pointmapstore::SetCoordSystem(const CoordSystem&); to ensure that the
 * coordinate column also gets the new csy as domain
 * 
 * 25    2-10-02 9:32 Willem
 * - Removed: unnecessary code to change storetype. In Column this is
 * already handled
 * - Replaced: loop to change spaces into underscores with a single
 * function call
 * 
 * 24    2/21/02 12:45p Martin
 * iRec function used a case sensitive compare
 * 
 * 23    8/24/01 13:03 Willem
 * Removed the SetReadOnly() function. This is now handled by
 * IlwisObjectPtr::SetReadOnly() for all ilwis objects
 * 
 * 22    20-03-01 12:35p Martin
 * X,Y column of old 2.0 files are only removed when a store is done. As a
 * remove will delete a section from the odf it can not be done earlier
 * 
 * 21    16-03-01 17:56 Koolhoven
 * with fFormat20 and no vlaid X or Y column throw an error instead of
 * silently returning
 * 
 * 20    3/16/01 13:30 Retsios
 * Make columns table-owned for showastbl
 * 
 * 19    5/03/01 10:34 Willem
 * Removed superfluous member _iPoints from PointMapPtr: iPnt() now gets
 * the number of points from the PointMapStore or PointMapVirtual
 * 
 * 18    27-11-00 11:24 Koolhoven
 * readability measures: removed "== true" and replaced "== false" by "!"
 * 
 * 17    11/01/00 2:01p Martin
 * removed superflous GetObjectStructure. They are all handled in the
 * relevant ilwisobject
 * 
 * 16    9/26/00 11:37a Martin
 * for 2.2 format the table will be loaded directly into memory (would
 * have happenend anyhow) instead of load-on-demand. 
 * 
 * 15    12-09-00 9:49a Martin
 * pn# is removed after a conversion
 * 
 * 14    12-09-00 9:13a Martin
 * added guards to prevent a store to ODF when not wanted
 * 
 * 13    11-09-00 11:33a Martin
 * added function for objectstructure and DoNotUpdate
 * 
 * 12    8-09-00 4:40p Martin
 * added function GetObjectStructure and DoNotUpdate
 * 
 * 11    15-02-00 8:55a Martin
 * Added changes for use of foreign formats
 * 
 * 10    8-02-00 18:01 Wind
 * set fUpdateCatalog flag to false after creation of internal table
 * 
 * 9     7-02-00 11:59a Martin
 * removing old pn# file
 * 
 * 8     21-01-00 3:55p Martin
 * Oops order of tbl creation wrong
 * 
 * 7     17-01-00 3:58p Martin
 * changed xy coords in true column coordinate
 * 
 * 6     17-01-00 11:29 Wind
 * added proximity to iValue(const Coord& ..) etc.
 * 
 * 5     29-10-99 9:19 Wind
 * thread save stuff
 * 
 * 4     22-10-99 12:54 Wind
 * thread save access (not yet finished)
 * 
 * 3     9/24/99 10:37a Wind
 * replaced calls to static funcs ObjectInfo::ReadElement and WriteElement
 * by calls to member functions
 * 
 * 2     9/08/99 11:59a Wind
 * comments
*/
// Revision 1.6  1998/09/16 17:26:27  Wim
// 22beta2
//
// Revision 1.5  1997/09/10 15:09:28  Wim
// Delete() now updates the number of points
//
// Revision 1.4  1997-08-21 13:08:38+02  martin
// Illegal columns are now checked in the constructor.
//
// Revision 1.3  1997/08/13 07:34:57  Wim
// Default range for coordinates set.
//
// Revision 1.2  1997-08-08 00:07:32+02  Willem
// Export to Ilwis 1.4 now truncates the point codes to 20 characters.
//
/* PointMapStore
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  WK   10 Jun 98    6:30 pm
*/
#include "Headers\toolspch.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Map\Point\ilwPoint.h"
#include "Engine\Map\Point\PNTSTORE.H"
#include "Engine\Table\tblstore.h"
#include "Engine\Table\COLSTORE.H"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Table\Rec.h"
#include "Engine\Base\DataObjects\Tranq.h"
#include "Engine\Domain\dmcoord.h"
#include "Engine\Base\System\Engine.h"
#include "Engine\Base\DataObjects\ObjectStructure.h"
#include "Engine\Table\tblinfo.h"
#include "Headers\Hs\point.hs"

#define EPS10 1.e-10

PointMapStore::PointMapStore(const FileName& fn, PointMapPtr& p, bool fDoNotLoad)
: MultiPoint(NULL, new GeometryFactory()),
fnObj(p.fnObj), ptr(p)
{
	if ( fDoNotLoad ) // loading and constructing will be done elsewhere (foreignformat)
		return;

	Table tbl = Table(fn);
	tbl->Load();
	tbl->DoNotStore(true);

	ILWIS::Version::BinaryVersion fvFormatVersion;
	ptr.ReadElement("PointMapStore", "Format", (int &)fvFormatVersion);
	if ( fvFormatVersion == shUNDEF)
		fvFormatVersion = ILWIS::Version::bvFORMAT30;
	getEngine()->getVersion()->fSupportsBinaryVersion(fvFormatVersion);
    ptr.setVersionBinary(fvFormatVersion);
	geometries = new vector<Geometry *>();

	// determine the format of the pointmap by means of the Coordinate column
	// This Column did not exist in version 2.2 and lower
	bool fFormat20 = !TableInfo::fExistCol(p.fnObj, tbl->sSectionPrefix, "Coordinate");
	Column colValue = tbl->col("Name");

	if (fFormat20)
	{
		tbl->LoadData();
		Column colX = tbl->col("X");
		Column colY = tbl->col("Y");
		if (!colX.fValid() || !colY.fValid())
			throw ErrorObject("PointMap does not have valid X and/or Y column");
		long iNr = tbl->iRecs();
		
		for (long i = 1; i <= iNr; ++i) 
		{
			double x = colX->rValue(i);
			double y = colY->rValue(i);
			SetPoint(Coord(x,y), i, colValue);
			
		}
	}
	else {
		Column colCrd = tbl->col("Coordinate");
			if (!colCrd.fValid())
				throw ErrorObject("PointMap does not have valid Coordinate column");
		long iNr = tbl->iRecs();;
		
		for (long i = 1; i <= iNr; ++i) 
		{
			SetPoint(colCrd->cValue(i), i, colValue);
		}
	}

}

PointMapStore::PointMapStore(const FileName& fn, PointMapPtr& p, long iPnts)
: MultiPoint(NULL, new GeometryFactory()), fnObj(p.fnObj), ptr(p)
{
	geometries = new vector<Geometry *>();
	for(int i=0; i<iPnts; ++i) 
		pntNew();

}

Feature *PointMapStore::pntNew(geos::geom::Geometry *pnt) {
	ILWIS::Point *p;
	if ( ptr.dvrs().fRealValues()) {
		p = new ILWIS::RPoint((geos::geom::Point*)(pnt));
	} else {
		p = new ILWIS::LPoint((geos::geom::Point*)(pnt));
	}
	geometries->push_back(p);
	return p;
}

void PointMapStore::SetPoint(const Coord& crd, int i, const Column& colValue) {
	if ( colValue->dvrs().fRealValues()) {
	 	double v = colValue->rValue(i);
		ILWIS::RPoint *p = new ILWIS::RPoint(crd, v);
		geometries->push_back(p);
	} else {
		long v = ptr.dvrs().fValues() ? colValue->iValue(i) : colValue->iRaw(i);
		ILWIS::LPoint *p = new ILWIS::LPoint(crd, v);
		geometries->push_back(p);
	} 
}

PointMapStore::~PointMapStore()
{
}

void PointMapStore::Store()
{
	FileName fnData(ptr.fnObj,".pt#");
	Table tbl;
	tbl.SetPointer(new TablePtr(ptr.fnObj, fnData, Domain("none"), Table::sSectionPrefix(".mpp")));
	tbl->fUpdateCatalog = false; // was set by constructor
	Domain dmcrd;
	dmcrd.SetPointer(new DomainCoord(ptr.cs()->fnObj));
	Column colCoord = tbl->colNew("Coordinate", dmcrd, ValueRange());
	
	DomainValueRangeStruct dvrs = ptr.dvrs();
	Column colVal;
	colVal = tbl->colNew("Name", dvrs);
	colVal->SetOwnedByTable(true);
	colVal->SetDescription("Point name/value");
	
	tbl->iRecNew(geometries->size() + 1);

	for(int i = 0 ; i < geometries->size(); ++i) {
		ILWIS::Point *point = CPOINT(geometries->at(i));
		if (!point)
			continue;
     	const Coordinate *c =  point->getCoordinate();
		Coord crd(c->x,c->y);
		colCoord->PutVal(i+1,crd);
		if (ptr.dvrs().fRawAvailable() ) {
			long v = point->iValue();
			colVal->PutRaw(i + 1,v);
		} else if (ptr.dvrs().fRealValues() ){
			double v = point->rValue();
			colVal->PutVal(i + 1,v);
		}
	}
	tbl->Store();
	//  ObjectInfo::WriteElement("PointMap", "Type", fnObj, "PointMapStore");
	ptr.WriteElement("PointMapStore", "StoreTime", (long)timStore);
	ILWIS::Version::BinaryVersion fvFormatVersion = ptr.getVersionBinary();
	ptr.WriteElement("PointMapStore", "Format", (long)fvFormatVersion);
}

void PointMapStore::UnStore(const FileName& fn)
{
	FileName fnData;
	if (ObjectInfo::ReadElement("TableStore", "Data", fn, fnData))
		_unlink(fnData.sFullName(true).scVal()); // delete data file if it's still there
	// not in all cases next unlink works, so also delete the following two sections
	ObjectInfo::WriteElement("Table", (char*)0, fn, (char*)0);
	ObjectInfo::WriteElement("TableStore", (char*)0, fn, (char*)0);
	ObjectInfo::WriteElement("PointMapStore", (char*)0, fn, (char*)0);
}

void PointMapStore::Export(const FileName& fn) const
{
	File filPnt(FileName(fn, ".pnt"), facCRT);
	filPnt.SetErase(true);
	if (ptr.dvrs().fValues())
		filPnt.WriteLnAscii("X! Y! Value&");
	else
		filPnt.WriteLnAscii("X! Y! Name$");
	Coord crd;
	String s, sName;
	bool fVals = ptr.dvrs().fRealValues();
	for (long i=0; i < iPnt(); i++) {
		Geometry *pnt = geometries->at(i);
		s = String("%li %li ", longConv(pnt->getCoordinate()->x), longConv(pnt->getCoordinate()->y));
		String sName;
		sName = ((ILWIS::Point *)pnt)->sValue(ptr.dvrs()); 

		if (!fVals)
		{
			// replace spaces by underscore
			sName = sName.sLeft(20);     // maximum size of Ilwis 1.4 strings
			replace(sName.begin(), sName.end(), ' ', '_');
		}
		s &= sName;  
		filPnt.WriteLnAscii(s);
	}
	filPnt.SetErase(false);
}

void PointMapStore::SaveAsFormat20()
{
	//TODO


	//FileName fnData;
	//ptr.ReadElement("TableStore", "Data", fnData);
	//Domain dmReal("value");
	//ValueRange vr(-1e8,1e8,0.001);
	//Column colX = tbl->colNew("X", dmReal,vr);
	//colX->SetOwnedByTable(true);
	//colX->SetDescription("X-coord");
	//Column colY = tbl->colNew("Y", dmReal,vr);
	//colY->SetOwnedByTable(true);
	//colY->SetDescription("Y-coord");
	//fnData.sExt = ".pn#";
	//tbl->SetDataFile( fnData);
	//
	//for (long i = 1; i <= iPnt(); ++i) 
	//{
	//	Coord crd = colCoord->cValue(i);
	//	colX->PutVal(i, crd.x);
	//	colY->PutVal(i, crd.y);
	//}
	//tbl->RemoveCol(colCoord);
	//Updated();
}

long PointMapStore::iPnt() const
{
	return geometries->size();
}

void PointMapStore::Updated()
{
	ptr.Updated();
	timStore = ptr.objtime;
}

void PointMapStore::SetErase(bool f)
{
	fErase = f;
} 

long PointMapStore::iRaw(long iRec) const
{
	Geometry *pnt = geometries->at(iRec);
	long raw = ((ILWIS::Point *)pnt)->iValue();
	return raw;
}

long PointMapStore::iValue(long iRec) const
{
	long value = iUNDEF;
	if ( iRec < geometries->size() && geometries->at(iRec) != NULL)	{
		ILWIS::LPoint *pnt = (ILWIS::LPoint *)geometries->at(iRec - 1);
		value  = pnt->iValue();
		value = ptr.dvrs().iValue(value);
	}

	return value;
}

double PointMapStore::rValue(long iRec) const
{
	double value = rUNDEF;
	if ( iRec < geometries->size() && geometries->at(iRec) != NULL)	{
		if ( ptr.dvrs().fUseReals()) {
			value  = ((ILWIS::RPoint *)(geometries->at(iRec)))->rValue();
		} else {
			value = ((ILWIS::LPoint *)(geometries->at(iRec)))->iValue();
			value = ptr.dvrs().rValue(value);
		}
	}
	return value;
}

String PointMapStore::sValue(long iRec, short iWidth, short iDec) const
{
	if ( iRec < geometries->size() && geometries->at(iRec) != NULL)	{
		if (ptr.dvrs().fUseReals()) {
			double val = rValue(iRec);
			return ptr.dvrs().sValue(val, iWidth, iDec);
		}
		long raw = iRaw(iRec);
		return ptr.dvrs().sValueByRaw(raw, iWidth, iDec);
	}
	 return sUNDEF;
}

Coord PointMapStore::cValue(long iRec) const
{
	if ( iRec  < geometries->size() && geometries->at(iRec) != NULL)	{
		Point *pnt = (Point *)geometries->at(iRec);
		const CoordinateSequence *p = pnt->getCoordinates();
		Coord c(p->getX(0),p->getY(0));
		delete p;
		return c;
	}
	return Coord();
}

long PointMapStore::iRec(const String& s) const
{
	for (long i = 0; i < geometries->size(); ++i) {
		ILWIS::Point *pnt = (ILWIS::Point *)geometries->at(i);
		if ( fCIStrEqual(s, pnt->sValue(ptr.dvrs(),0)))
		  return i;
	}
  return iUNDEF;
}

long PointMapStore::iRec(long iR) const
{
	for (long i = 0; i < geometries->size(); ++i)
		if (iR == iRaw(i))
			return i;

	return iUNDEF;
}

long PointMapStore::iRec(const Coord& c, double rPrx) const
{
	long iRes = iUNDEF;
	double r2Res = rPrx == rUNDEF ? ptr.rProximity() : rPrx;
	r2Res *= r2Res;
	double r2;
	Coord crd;
	for (long i = 0; i < geometries->size(); ++i) {
		crd = cValue(i);
		if (crd.fUndef())
			continue;
		r2 = rDist2(c, crd);
		if (r2 <= r2Res + EPS10) {
			if (r2 == 0)
				return i;
			r2Res = r2;
			iRes = i;
		}
	}
	if ( iRes != iUNDEF)
		return iRes;
	return iUNDEF;
}

void PointMapStore::PutRaw(long iRec, long iRaw)
{
	if ( iRec >= geometries->size() )
		return;
	((ILWIS::Point *)geometries->at(iRec))->PutVal(iRaw);
	Updated();
}

void PointMapStore::PutVal(long iRec, double rVal)
{
	if ( iRec >= geometries->size() )
		return;
	if(!ptr.dvrs().fValues())
		return;
	if ( ptr.dvrs().fUseReals()) {
		((ILWIS::Point *)geometries->at(iRec))->PutVal(rVal);	
	} else {
		long iRaw = ptr.dvrs().iRaw(rVal);
		((ILWIS::Point *)geometries->at(iRec))->PutVal(iRaw);
	}
	Updated();
}

void PointMapStore::PutVal(long iRec, const String& sVal)
{
	if ( iRec >= geometries->size() )
		return;
	if(!ptr.dvrs().fValues())
		return;
	if ( ptr.dvrs().fUseReals()) {
		((ILWIS::Point *)geometries->at(iRec - 1))->PutVal(sVal.rVal());	
	} else {
		long iRaw = ptr.dvrs().iRaw(sVal);
		((ILWIS::Point *)geometries->at(iRec - 1))->PutVal(iRaw);
	}
	Updated();
}

void PointMapStore::PutVal(long iRec, const Coord& c)
{
	if ( iRec >= geometries->size() )
		return;
	ILWIS::Point *p = (ILWIS::Point *)((*geometries)[iRec]);
	p->setCoord(c);
	Updated(); 
}

void PointMapStore::Delete(long iRec)
{
	geometries->erase(geometries->begin() + iRec);
	Updated();
}

long PointMapStore::iAdd(long iRecs)
{
	for(int i=geometries->size() - 1; i < iRecs; ++i){
		geometries->push_back((Geometry *)pntNew());
	}

	return geometries->size();
}

long PointMapStore::iAddRaw(const Coord& c, long iRaw)
{
	ILWIS::LPoint *p = new ILWIS::LPoint(c, iRaw);
	geometries->push_back(p);
	Updated();
	return geometries->size();
}

long PointMapStore::iAddVal(const Coord& c, const String& s)
{
	//if(!ptr.dvrs().fValues())
	//	return iUNDEF;
	ILWIS::Point *p;
	if ( ptr.dvrs().fUseReals()) {
		p = new ILWIS::RPoint(c, s.rVal());
	} else {
		long iRaw = ptr.dvrs().iRaw(s);
		p = new ILWIS::LPoint(c, iRaw);
	}
	geometries->push_back(p);
	Updated();
	return geometries->size();
}

long PointMapStore::iAddVal(const Coord& c, double rValue)
{
	if(!ptr.dvrs().fValues())
		return iUNDEF;
	ILWIS::Point *p;
	if ( ptr.dvrs().fUseReals()) {
		p = new ILWIS::RPoint(c, rValue);
	} else {
		long iRaw = ptr.dvrs().iRaw(rValue);
		p = new ILWIS::LPoint(c, iRaw);
	}
	geometries->push_back(p);
	return geometries->size();
}

void PointMapStore::PutBufRaw(const CoordBuf& cb, const LongBuf& lb)
{
	for(int i = 0; i < cb.iSize(); ++i) {
		iAddRaw(cb[i], lb[i]);
	}
}

void PointMapStore::GetDataFiles(Array<FileName>& afnDat, Array<String>* asSection, Array<String>* asEntry) const
{
	FileName fnDat(fnObj, ".pt#", true);
	ObjectInfo::Add(afnDat, fnDat, fnObj.sPath());
	if (asSection != 0) {
		(*asSection) &= "TableStore";
		(*asEntry) &= "Data";
	}
}

void PointMapStore::KeepOpen(bool f)
{
	// points are stored in table in memory
}

bool PointMapStore::fConvertTo(const DomainValueRangeStruct& _dvrsTo, const Column& col)
{
	DomainValueRangeStruct dvrsTo = _dvrsTo;
	if (col.fValid())
		dvrsTo = col->dvrs();
	Tranquilizer trq;
	trq.SetTitle(SPNTTitleDomainConversion);
	trq.SetText(SPNTTextConverting);
	//  ColumnStore* pcsNewVal = colNewVal->pcs();
	if (ptr.dvrs().fValues()) {
		trq.Start();
		if (dvrsTo.fUseReals()) {
			double r = rUNDEF;
			for (long i=0; i < iPnt(); i++) {
				if (trq.fUpdate(i, iPnt()))
					return false;
				if (col.fValid())
					r = col->rValue(iValue(i));
				else
					r = rValue(i);
				PutVal(i,r);
			}   
			trq.fUpdate(iPnt(), iPnt());
		}
		else if (dvrsTo.fValues()) {
			long iRaw = iUNDEF;
			double r;
			for (long i=1; i <= iPnt(); i++) {
				if (trq.fUpdate(i, iPnt()))
					return false;
				if (col.fValid())
					iRaw = col->iRaw(iValue(i));
				else {
					r = rValue(i);
					iRaw = dvrsTo.iRaw(r);
				}
				PutRaw(i,iRaw);
			}   
			trq.fUpdate(iPnt(), iPnt());
		}
	}
	else if (col.fValid()) {
		trq.Start();
		// read long raws and use them as record nr. in column col
		String s;
		for (long i=1; i <= iPnt(); i++) {
			if (trq.fUpdate(i, iPnt()))
				return false;
			s = col->sValue(iRaw(i));
			PutVal(i, s);
		}   
		trq.fUpdate(iPnt(), iPnt());
	}
	else  {
		trq.Start();
		String s;
		for (long i=0; i < iPnt(); i++) {
			if (trq.fUpdate(i, iPnt()))
				return false;
			s = sValue(i);
			PutVal(i, s);
		}   
		trq.fUpdate(iPnt(), iPnt());
	}
	ObjectInfo::WriteElement("Col:Name", (char*)0, fnObj, (char*)0);
	return true;
}

void PointMapStore::Flush()
{
//	tbl->Store();
}
								 
void PointMapStore::DoNotUpdate()
{
}

void PointMapStore::SetCoordSystem(const CoordSystem& cs)
{
}

Geometry * PointMapStore::getFeature(long i) const {
	if ( geometries && i < geometries->size())
		return geometries->at(i);
	return NULL;
}

vector<Geometry *> PointMapStore::getFeatures(Coord crd, double rPrx) {
	vector<Geometry *> points;
	for(int i = 0; i< geometries->size(); ++i) {
		Geometry *g = geometries->at(i);
		if ( crd.equals(*(g->getCoordinate())))
			points.push_back(g);
	}
	return points;
}
