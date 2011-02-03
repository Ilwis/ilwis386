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
#include "Engine\DataExchange\impshp.h"
#include "Engine\Map\Point\PNT.H"
#include "Engine\DataExchange\Convloc.h"
#include "Engine\SpatialReference\csbonly.h"


void ThrowUnsupportedShapeType(const FileName& fn, const String& sErr)
{
    throw ErrorObject(WhatError(sErr, 1), fn);
}

void ThrowImportingTableError(const FileName& fn)
{
    throw ErrorObject(WhatError(String(SCVErrImpTableError), 1), fn);
}

void ThrowReadFileError(const FileName& fn)
{
    throw ErrorObject(WhatError(String(SCVErrReading), 1), fn);
}

void ThrowAbortImportError(const FileName& fn)
{
    throw ErrorObject(WhatError(String(SCVErrAbortImport), 1), fn);
}

void ThrowFileDoesNotExistError(const FileName& fn)
{
    throw ErrorObject(WhatError(String(SCVErrFileNotFound), 1), fn);
}

void ThrowRecordOutOfRangeError(const FileName& fn)
{
    throw ErrorObject(WhatError(String(SCVErrRecordOutOfRange), 1), fn);
}

void ThrowCouldNotCreatePolygonError(const FileName& fn)
{
    throw ErrorObject(WhatError(String(SCVErrNoPol), 1), fn);
}

inline void BigToLittle(unsigned long &iVal)    // create intel type long
{
    iVal = ((iVal >> 24) & 0x000000ffL) |   // most significant Motorola
           ((iVal >>  8) & 0x0000ff00L) |   //
           ((iVal <<  8) & 0x00ff0000L) |   //
           ((iVal << 24) & 0xff000000L);    // least significant Motorola
}

//---------------------------------------------------------------------------------
//wrapper for entering the import functions
void ImpExp::ImportShapeFile(const FileName& fnInFile, const FileName& fnOutFile)
{
    ImpShapeFile imp(fnInFile, fnOutFile);
    imp.Init();
    if(!imp.fImport(trq))
        ThrowAbortImportError(fnInFile);
}

// ctor determines mainly the filenames
ImpShapeFile::ImpShapeFile(FileName fnPrefixName, FileName fnOutName) : 
        iMaxRecords(-1), 
        fnOutFile(fnOutName),
        IndexFile(NULL),
        shapeFile(NULL)
{
    String sPrefix=fnPrefixName.sFullPath(false);
    fnDBF=FileName(String("%S.dbf", sPrefix));
    fnMain=FileName(String("%S.shp", sPrefix));
    fnIndex=FileName(String("%S.shx", sPrefix));
}

ImpShapeFile::~ImpShapeFile()
{
    delete shapeFile;
    delete IndexFile;
}

//creation of files(input) than can be used and header information is used to determine
// the bounding box and the file size.
void ImpShapeFile::Init()
{
    FileHeader header;
    if (!File::fExist(fnMain))
        ThrowFileDoesNotExistError(fnMain);
    if (!File::fExist(fnIndex))
        ThrowFileDoesNotExistError(fnIndex);

    shapeFile=new File(fnMain);
    IndexFile=new File(fnIndex);
    shapeFile->Read(sizeof(FileHeader), &header);
    isfType=(isfShapeType)header.iShapeType;
		switch (isfType)
    {
      case isfPoint:
      case isfArc:
      case isfPolygon:
      case isfMultiPoint:
      case isfPointZ:
      case isfArcZ:
      case isfPolygonZ:
      case isfMultiPointZ:
      case isfPointM:
      case isfArcM:
      case isfPolygonM:
      case isfMultiPointM:
        break;
      default:
        ThrowUnsupportedShapeType(fnMain, String(SCVErrUnsuppShapeType_I.scVal(), isfType));
    }
    double rMinX=header.rBox[0];
    double rMinY=header.rBox[1];
    double rMaxX=header.rBox[2];
    double rMaxY=header.rBox[3];
    cbMap=CoordBounds(Coord(rMinX, rMinY), Coord(rMaxX, rMaxY));

    iMaxRecords=( IndexFile->iSize() - INDEX_HEADER_SIZE) / SIZE_INDEX_RECORD;
    
}    

//creation of the output file(s).
bool ImpShapeFile::fImport(Tranquilizer& trq)
{
    FileName fnDomain = FileName::fnUnique(FileName(String("%S.dom",fnOutFile.sFullPath(false))));
    FileName fnTable = FileName::fnUnique(FileName(String("%S.tbl",fnOutFile.sFullPath(false))));
    Domain dmSimpleId = Domain(fnDomain, iMaxRecords, dmtID);
    DBIVImport dbTable(fnDBF, fnTable, trq);
    dbTable.SetTableDomain(dmSimpleId); // the imported table will have a ID domain
    //if (!dbTable.fImport()) // importing the dbf file. The result will be used as attribute table
    //   ThrowImportingTableError(fnDBF) ;
    //trq.SetTitle(SCVTitleImportShape);
    //trq.SetText(SCVTextProcessing);
    //dbTable.GetTable()->fErase = true;        // keep table only if import shape succeeds
    //dbTable.GetTable()->dm()->fErase = true;  // same for the table domain    

    bool fFailed = true;
    switch(isfType)
    {
        case isfPoint:
        case isfPointZ:
        case isfPointM:
        case isfMultiPoint: // points and multi points are almost the same
        case isfMultiPointZ:
        case isfMultiPointM:
            fFailed = fImportPoints(dbTable.GetTable(), trq); 
            break;
        case isfArc:
        case isfArcZ:
        case isfArcM:
            fFailed = fImportArcs(dbTable.GetTable(), trq);
            break;
        case isfPolygon: // arcs and polygons use the same structure but their handling is different.
        case isfPolygonZ:
        case isfPolygonM:
            fFailed = fImportPolygons(dbTable.GetTable(), trq);
            break;
        default:
            ThrowUnsupportedShapeType(fnMain, String(SCVErrUnsuppShapeType_I.scVal(), isfType));
    }

    dbTable.GetTable()->fErase = !fFailed;        // keep table only if import shape succeeds
    dbTable.GetTable()->dm()->fErase = !fFailed;  // same for the table domain    
    
    return fFailed;
}

// importing segments
bool ImpShapeFile::fImportArcs(const Table& table, Tranquilizer& trq)
{
    long iStartIndex, iEndIndex;
    DomainValueRangeStruct dvs(table->dm());
    SegmentMap segMap( FileName::fnUnique(FileName(String("%S.mps",fnOutFile.sFullPath(false)))), csy, cbMap, dvs);
    segMap->SetAttributeTable(table); // the ex dbf file becomes the attribute table
    
    for(long iRec=0; iRec<iMaxRecords; ++iRec)
    {
        if ( trq.fUpdate(iRec, iMaxRecords )) return false;

        ArcStruct arcRecord;
        if (!fReadArcRecord(iRec, arcRecord)) return false;
        {
            for(long iPart=0; iPart<arcRecord.iNumParts; ++iPart)
            {
                GetStartAndEnd(arcRecord, iPart, iStartIndex, iEndIndex); // translate index into locations in shp file
                AddSegment(iRec, iStartIndex, iEndIndex, arcRecord, segMap);
            }
        }
    }
    return true;
}    
                
bool ImpShapeFile::fImportPoints(const Table& table, Tranquilizer& trq)
{
    DomainValueRangeStruct dvs(table->dm());
    String sName("%S.mpp",fnOutFile.sFullPath(false));
    FileName fnNew=FileName::fnUnique(FileName(sName));
    PointMap pMap( fnNew, csy, cbMap, dvs);
    pMap->SetAttributeTable(table);
    
    for(long iRec=0; iRec<iMaxRecords; ++iRec)
    {
        if ( trq.fUpdate(iRec, iMaxRecords )) return false;
        if ( isfType==isfPoint)
        {
            PointStruct pntRecord;
            if (!fReadPointRecord(iRec, pntRecord)) return false;
			if (pntRecord.rX != rUNDEF && pntRecord.rY != rUNDEF)
				AddPointRecord(iRec, pntRecord.rX, pntRecord.rY , pMap);
        }
        else
        {
            MultiPointStruct mpntRecord;
            if (!fReadMultiPointRecord(iRec, mpntRecord)) return false;
            for( long iPnt=0; iPnt<mpntRecord.iNumPoints; ++iPnt) // for all points
                AddPointRecord(iRec,mpntRecord.cBuf[iPnt].x, mpntRecord.cBuf[iPnt].y, pMap);
        }                
    }
    return true;
}

bool ImpShapeFile::fReadArcRecord(long iRec, ArcStruct& Arc)
{
    try
    {
        if (!GoTo(iRec))
            ThrowRecordOutOfRangeError(fnMain);
        if (shapeFile->Read(ARC_PART_SIZE, &Arc)==0) // read first part of header;
            ThrowReadFileError(fnMain);

		if (Arc.iShapeType == 0)  // null shape: initialize
		{
			Arc.iNumParts = 0;
			Arc.iContentLength = 0;
			Arc.iNumPoints = 0;

			return true;
		}

		Arc.iPartIndex.Resize(Arc.iNumParts);
        for(long iPart=0; iPart<Arc.iNumParts; ++iPart)
        {
            long iIndex;
            shapeFile->Read(4, &iIndex);
            Arc.iPartIndex[iPart] = iIndex;
        }
		Arc.cPoints.Resize(Arc.iNumPoints);
        for (long iPoint=0; iPoint<Arc.iNumPoints; ++iPoint)
            ReadPoint(iPoint, Arc.cPoints);
    }
    catch(ErrorObject& err)
    {
        err.Show(SCVErrErrorTitle);
        return false;
    }        

    return true;    
}    
    
bool ImpShapeFile::fReadMultiPointRecord(long iRec, MultiPointStruct& pntRecord)
{
    try
    {
        if (!GoTo(iRec))
            ThrowRecordOutOfRangeError(fnMain);
        if(shapeFile->Read(MPNT_PART_SIZE, &pntRecord)==0) // read first part of header;
            ThrowReadFileError(fnMain);

		if (pntRecord.iShapeType == 0)  // null shape: initialize
		{
			pntRecord.iNumPoints = 0;
			pntRecord.iContentLength = 0;

			return true;
		}
		pntRecord.cBuf.Resize(pntRecord.iNumPoints);
        for(long i=0; i<pntRecord.iNumPoints; ++i)
            ReadPoint(i, pntRecord.cBuf);
        
        return true;
    }        
    catch(ErrorObject& err)
    {
        err.Show(SCVErrErrorTitle);
        return false;
    } 
}

bool ImpShapeFile::fReadPointRecord(long iRec, PointStruct& pntRecord)
{
    try
    {
        if (!GoTo(iRec)) 
            ThrowRecordOutOfRangeError(fnMain);
        if( shapeFile->Read(sizeof(PointStruct), &pntRecord)==0) 
            ThrowReadFileError(fnMain);
    
		if (pntRecord.iShapeType == 0)  // null shape: initialize
		{
			pntRecord.rX = rUNDEF;
			pntRecord.rY = rUNDEF;
			pntRecord.iContentLength = 0;
		}

        return true;
    }
    catch(ErrorObject& err)
    {
        err.Show(SCVErrErrorTitle);
        return false;
    } 
    
}    

// adds a ring to a polygon
void ImpShapeFile::AddPolygonPart(long iRec, long iPart, PolygonStruct& polShape, ILWIS::Polygon* pol, PolygonMap& polMap, bool fIsland)
{
	vector<Coordinate> *coords = new vector<Coordinate>();
    long iStart, iEnd;
    GetStartAndEnd(polShape, iPart, iStart, iEnd);
    long iLastPoint=0;
    for(long iPnt=iStart; iPnt<iEnd; ++iPnt) // max 1000 points per topology
    {

		coords->push_back(polShape.cPoints[iPnt]); // convert coords in to internal row/cols

	}
	CoordinateSequence *seq = new CoordinateArraySequence(coords);
	LinearRing *ring = new LinearRing(seq, new GeometryFactory());
	if ( fIsland) {
		pol->addHole(ring);
	}
	else{
		pol->PutVal(iRec+1);
		pol->addBoundary(ring);
	}
	double r4 = pol->rArea();
}
            
bool ImpShapeFile::fImportPolygons(const Table& table, Tranquilizer& trq)
{
    DomainValueRangeStruct dvs(table->dm());
    PolygonMap polMap( FileName::fnUnique(FileName(String("%S.mpa",fnOutFile.sFullPath(false)))), csy, cbMap, dvs);
	polMap->TopologicalMap(false);
    ILWIS::Polygon *pol;
    polMap->SetAttributeTable(table);
    Array<long> polygonRecords;
    Array<long> islandRecords;
    
    for(long iRec=0; iRec<iMaxRecords; ++iRec)
    {
        if ( trq.fUpdate(iRec, iMaxRecords )) return false;

        PolygonStruct polShape;
        if (!fReadArcRecord(iRec, polShape)) return false;
        {
			if ( iRec == 160) {
				int i =1;
				++i;
			}
            SplitPartsArray(polShape, polMap, polygonRecords, islandRecords); // split in islands and holes
            for(int iPol=0; iPol<polygonRecords.iSize(); ++iPol) // for all 'true' polygons
            {
                pol=CPOLYGON(polMap->newFeature());
                AddPolygonPart(iRec, polygonRecords[iPol], polShape, pol, polMap, false);

            }
            for(int i=0; i<islandRecords.iSize(); ++i) // for all holes
            {
                long iStart, iEnd, iIsland=islandRecords[i];
                GetStartAndEnd(polShape, iIsland, iStart, iEnd);
                if (pol->fValid())
                {
                    AddPolygonPart(iRec, iIsland, polShape, pol, polMap,true);
                }                    
           
            }

        }

    }
    return true;
}    

void ImpShapeFile::AddSegment(long iRec, long iStart, long iEnd, ArcStruct& Arc, SegmentMap& segMap)
{
    CoordBuf cBuf(1000);
    RowColBuf rcBuf(1000);
    long iLastPoint=0;
    for(long iPnt=iStart; iPnt<iEnd; iPnt+=1000)
    {
        ILWIS::Segment *segCur = CSEGMENT(segMap->newFeature());
        if (iPnt!=iStart) cBuf[0]=cBuf[999]; // connect to the last one

        long iLastBufEnd=iLastPoint+iStart;
        // convert 1000 points; keep in mind that the first point may already be filled (connect predecessor)
		long iCount= iPnt==iStart ? 0 : 1;
        for( ; iCount<1000 && iLastBufEnd+iCount<iEnd; ++iCount)
            cBuf[iCount]=Arc.cPoints[iLastBufEnd+iCount];
    
        segCur->PutCoords(iCount, cBuf);
		segCur->PutVal(iRec+1);
        iLastPoint+=1000;
    }        
}    
    
void ImpShapeFile::AddPointRecord(const long iRec, const double rX, const double rY, PointMap& pMap)
{
    Coord cPoint(rX, rY);
    pMap->iAddRaw(cPoint, iRec+1);
} 

void ImpShapeFile::GetStartAndEnd(const ArcStruct& record, long iPart, long& iStartIndex, long& iEndIndex)
{
       iStartIndex=record.iPartIndex[iPart];
       iEndIndex=iPart+1<record.iNumParts ? record.iPartIndex[iPart+1] : record.cPoints.iSize();
} 

void ImpShapeFile::ReadPoint(int iPntPos, ArrayLarge<Coord>& cPoints)
{
    double rX, rY;
    if( shapeFile->Read(8, &rX)==0) ThrowReadFileError(fnMain);
    if( shapeFile->Read(8, &rY)==0) ThrowReadFileError(fnMain);
    cPoints[iPntPos] = Coord(rX, rY);
}

bool ImpShapeFile::GoTo(long iRec)
{
    if ( iRec*8 > IndexFile->iSize()) return false;
    unsigned long iOffset;
    IndexFile->Seek(HEADER_SIZE + iRec*8); //size of index record is eight bytes
    IndexFile->Read(4, &iOffset); // offset is long
    BigToLittle(iOffset);
    shapeFile->Seek(iOffset*2); // offset is 2 bytes

    return true;
}

class PolArea {
public:
	void Buf(CoordBuf& buf, long iSize)
	{
		for(long i=1; i<iSize; ++i)
			Line(buf[i-1], buf[i]);
	}

	void Line(Coord p, Coord q)
	{
	  rBkw += q.x * p.y;
	  rFrw += p.x * q.y;
	  rLen += rDist(p,q);
	}
	double rArea() const 
	{ 
		return (rFrw - rBkw)/2 ; 
	}
	void Reset()
	{ 
		rBkw=rFrw=rLen=0; 
	}
private:
  double rBkw, rFrw, rLen;

};

//creates two arrays that contain (indexs to) all polygons and holes
void ImpShapeFile::SplitPartsArray(const PolygonStruct& polShape, PolygonMap& polMap, Array<long>& polygons, Array<long>& islands)
{
    polygons.Reset();
    islands.Reset();
    CoordBuf crdBuf(1000);
    long iStart, iEnd;
	PolArea area;
    for(long iPart=0; iPart<polShape.iNumParts; ++iPart)
    {
        GetStartAndEnd(polShape, iPart, iStart, iEnd);
        long iLastPoint=0;
        for(long iPnt=iStart; iPnt<iEnd; iPnt+=1000)
        {
            if (iPnt!=iStart) crdBuf[0]=crdBuf[999]; // connect to the last one

            long iLastBufEnd=iLastPoint+iStart;
            // convert 1000 points; keep in mind that the first point may already be filled (connect predecessor)
			long iCount= iPnt==iStart ? 0 : 1 ;
            for(; iCount<1000 && iLastBufEnd+iCount<iEnd; ++iCount)
            {   
                Coord c=polShape.cPoints[iLastBufEnd+iCount];
                crdBuf[iCount]=c;
            }    
		    area.Buf(crdBuf, iCount);
            iLastPoint+=1000;
            
        } 
 		double rAr=area.rArea(); // if area is positive it is a hole else it is a polygon
		if (  rAr> 0.0) islands&=iPart;
		else polygons&=iPart;        
		area.Reset();
    }
}    


