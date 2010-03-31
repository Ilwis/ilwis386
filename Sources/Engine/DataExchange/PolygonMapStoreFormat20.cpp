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
#include "Headers\toolspch.h"
#include "Engine\Domain\dmcoord.h"
#include "Engine\DataExchange\PolygonMapStoreFormat20.h"

PolygonMapStoreFormat20::PolygonMapStoreFormat20(const FileName& fn, PolygonMapPtr& p) :
	PolygonMapStore(fn, p, true)
{
	Load();
}

void PolygonMapStoreFormat20::Load()
{
	Table tblPolygon;
	Table tblTopology;
	File *filPol=NULL, *filPolCode=NULL, *filTop=NULL;
	FileName fnPolygon, fnTop, fnCode;

	if ( ptr.ReadElement("PolygonMapStore", "DataPol", fnPolygon))
		filPol = new File(fnPolygon);
	else
		throw ErrorObject("Polygon map is missing polygon data");

	if ( ptr.ReadElement("PolygonMapStore", "DataTop", fnTop))
		filTop = new File(fnTop);
	else
		throw ErrorObject("Polygon map is missing topology data");

	if ( ptr.ReadElement("PolygonMapStore", "DataPolCode", fnCode))
		filPolCode = new File(fnCode);
	else
		throw ErrorObject("Polygon map is missing polygon codes");

	rAlfa1 = ptr.rReadElement("SegmentMap", "Alfa");
	rBeta1 = ptr.rReadElement("SegmentMap", "Beta1");
	rBeta2 = ptr.rReadElement("SegmentMap", "Beta2");

	int iValueSize = st() != stREAL ? 4 : 8;
	long iSeg = ptr.iReadElement("SegmentMapStore","Segments");
	poltype *polbuf = new poltype[ptr.iFeatures()];
	toptype *topbuf = new toptype[iSeg];
	char *valuebuffer = new char[ iValueSize * ptr.iFeatures()]; 

	filPol->Read(sizeof(poltype) * ptr.iFeatures(), (void *)polbuf);
	filTop->Seek(sizeof(toptype));
	filTop->Read(sizeof(toptype) * iSeg, (void *)topbuf);
	filPolCode->Seek(0); // ?? check
	filPolCode->Read(iValueSize * ptr.iFeatures(), (void *)valuebuffer);

	tblPolygon->iRecNew(ptr.iFeatures());

	for(int iPolygon=0; iPolygon < ptr.iFeatures(); ++iPolygon)
	{
		long iVal = 0;
		double rVal = 0;

		poltype pol = polbuf[iPolygon];
		void *val = &(valuebuffer[iPolygon * iValueSize]);
		if ( ptr.st() == stREAL )
		{
			rVal = *((double *) val );
			tblPolygon->col(ctValue)->PutVal(iPolygon + 1, rVal);
		}
		else
		{
			iVal = *((long *) val);

			tblPolygon->col(ctValue)->PutRaw(iPolygon + 1, iVal);
		}

		Coord crdMin = ToCoord(crdtype(pol.mm.MinX, pol.mm.MinY));
		Coord crdMax = ToCoord(crdtype(pol.mm.MaxX, pol.mm.MaxY));

		tblPolygon->col(ctMinCrd)->PutVal(iPolygon + 1  , crdMin);
		tblPolygon->col(ctMaxCrd)->PutVal(iPolygon + 1 , crdMax);
		tblPolygon->col(ctArea)->PutVal(iPolygon + 1, pol.rArea);
		tblPolygon->col(ctLength)->PutVal(iPolygon + 1, pol.rLength);
		tblPolygon->col(ctTopStart)->PutVal(iPolygon + 1, (long)pol.iSegStart);
		long iDeleted = pol.iSegStart == 0 || iVal == iUNDEF || rVal == rUNDEF;
		tblPolygon->col(ctDeleted)->PutVal(iPolygon + 1, iDeleted); 
	}

	for ( int iTop = 0; iTop < iSeg; ++iTop)
	{
		toptype top = topbuf[iTop];
		tblTopology->col(ctLeft)->PutVal(iTop + 1, (long)top.iPolLeft);
		tblTopology->col(ctRight)->PutVal(iTop + 1,(long)top.iPolRight);
		tblTopology->col(ctForward)->PutVal(iTop + 1,(long)top.iFwd);
		tblTopology->col(ctBackward)->PutVal(iTop + 1,(long)top.iBwd);
	}

	//Updated();
	tblPolygon->Loaded(true);
	tblTopology->Loaded(true);

	delete [] polbuf;
	delete [] topbuf;
	delete [] valuebuffer;
	delete filPol;
	delete filPolCode;
	delete filTop;

}

void PolygonMapStoreFormat20::SetColumns(Table& tblPolygon, Table& tblTopology)
{
	Domain dmcrd;
	dmcrd.SetPointer(new DomainCoord(ptr.cs()->fnObj));
	Domain dmcrdbuf("CoordBuf");
	dmcrd.SetPointer(new DomainCoord(ptr.cs()->fnObj));

	tblPolygon->colNew("MinCoords", dmcrd, ValueRange());
	tblPolygon->colNew("MaxCoords", dmcrd, ValueRange());
	tblPolygon->colNew("Area", DomainValueRangeStruct(-1e307, 1e307, 0.001));
	tblPolygon->colNew("Length", DomainValueRangeStruct(-1e307, 1e307, 0.001));
	tblPolygon->colNew("TopStart", DomainValueRangeStruct(-10000000 , 10000000));
	tblPolygon->colNew("PolygonValue", ptr.dvrs());
	tblPolygon->colNew("Deleted", Domain("bool"));

	tblTopology->DoNotStore(true);
	tblTopology->fErase = tblPolygon->fErase = true;

	Column colm = tblTopology->col("SegmentValue");
	if ( colm.fValid() )
		tblTopology->RemoveCol(colm);
	tblTopology->colNew("MinCoords", dmcrd, ValueRange());
	tblTopology->colNew("MaxCoords", dmcrd, ValueRange());
	tblTopology->colNew("Coords", dmcrdbuf);
	tblTopology->colNew("Deleted", Domain("bool"));


	tblTopology->colNew("LeftPolygon",  DomainValueRangeStruct(-10000000 , 10000000));
	tblTopology->colNew("RightPolygon", DomainValueRangeStruct(-10000000 , 10000000));
	tblTopology->colNew("ForwardLink", DomainValueRangeStruct(-10000000 , 10000000));
	tblTopology->colNew("BackwardLink", DomainValueRangeStruct(-10000000 , 10000000));

	tblTopology->DoNotStore(false);
}


Coord PolygonMapStoreFormat20::ToCoord(const crdtype& crd)
{
	return Coord( crd.x * rAlfa1 + rBeta1, crd.y * rAlfa1 + rBeta2 );
}

void PolygonMapStoreFormat20::SetErase(bool f)
{
	File *filPol=NULL, *filPolCode=NULL, *filTop=NULL;
	FileName fnPolygon, fnTop, fnCode;

	if ( ptr.ReadElement("PolygonMapStore", "DataPol", fnPolygon))
		filPol = new File(fnPolygon);
	if ( ptr.ReadElement("PolygonMapStore", "DataTop", fnTop))
		filTop = new File(fnTop);
	if ( ptr.ReadElement("PolygonMapStore", "DataPolCode", fnCode))
		filPolCode = new File(fnCode);

	if (0 != filPol)
		filPol->SetErase(f);

	if (0 != filTop)
		filTop->SetErase(f);

	if (0 != filPolCode)
		filPolCode->SetErase(f);
} 
