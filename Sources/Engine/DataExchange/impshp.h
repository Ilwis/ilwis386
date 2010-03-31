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
#include "Engine\Base\DataObjects\ARRAY.H"
#include "Engine\Map\Polygon\POL.H"
#include "Engine\DataExchange\Impdbf.h"

typedef enum isfShapeType{isfUnknown=0, 
                 isfPoint=1,   isfArc=3,   isfPolygon=5,   isfMultiPoint=8,
                 isfPointZ=11, isfArcZ=13, isfPolygonZ=15, isfMultiPointZ=18,
                 isfPointM=21, isfArcM=23, isfPolygonM=25, isfMultiPointM=28,
                 isfMultiPatch=31};

#define HEADER_SIZE 100
#define SHAPE_TYPE_POS 32
#define XMIN_POS 36
#define YMIN_POS 44
#define XMAX_POS 52
#define YMAX_POS 60
#define INDEX_HEADER_SIZE 100
#define SIZE_INDEX_RECORD 8

#pragma pack(4)
struct RecordHeader
{
    long iRecordNumber;
    long iContentLength;
};

struct FileHeader
{
    long    iFileCode;
    long    Unused1[5];
    long    iFileLength;
    long    iVersion;
    long    iShapeType;
    double  rBox[4];
    long    Unused2[8];
};

struct PointStruct : public RecordHeader
{
    long iShapeType;
    double rX;
    double rY;
};

struct MultiPointStruct : public RecordHeader
{
    long iShapeType;
    double rBox[4];
    long iNumPoints;
    ArrayLarge<Coord> cBuf;
};

struct ArcStruct : public RecordHeader
{
    long iShapeType;
    double rBox[4];
    long iNumParts;
    long iNumPoints;
    Array<long> iPartIndex;
    ArrayLarge<Coord> cPoints;
};    
#pragma pack()

#define ARC_PART_SIZE (sizeof(RecordHeader)+44)
#define MPNT_PART_SIZE (sizeof(RecordHeader)+40)

typedef ArcStruct PolygonStruct;

class ImpShapeFile
{
	public:
		bool fImport(Tranquilizer& trq);
		ImpShapeFile(FileName fnPrefixName, FileName fnOutName);
		~ImpShapeFile();
		void Init();

	private:
		CoordBounds cbMap;
		CoordSystem csy;
		FileName fnDBF;
		FileName fnMain;
		FileName fnIndex;
		long iMaxRecords;
		isfShapeType isfType;
		FileName fnOutFile;
		File *IndexFile;
		File *shapeFile;


		bool fImportPolygons(const Table& table,Tranquilizer& trq);
		bool fImportArcs(const Table& table, Tranquilizer& trq);
		bool fImportPoints(const Table& table, Tranquilizer& trq);
		bool fReadArcRecord(long iRec, ArcStruct& Arc);
		bool fReadMultiPointRecord(long iRec, MultiPointStruct& pntRecord);
		bool fReadPointRecord(long iRec, PointStruct& pntRecord);
		void AddPolygonPart(long iRec, long iPart, PolygonStruct& polShape, ILWIS::Polygon* pol, PolygonMap& polMap, bool fIsland);
		void AddSegment(long iRec, long iStart, long iEnd, ArcStruct& Arc, SegmentMap& segMap);
		void AddPointRecord(const long iRec, const double X, const double Y, PointMap& pMap);      
		void GetStartAndEnd(const ArcStruct& record, long iPart, long& iStartIndex, long& iEndIndex);
		void ReadPoint(int iPntPos, ArrayLarge<Coord>& cPoints);
		bool GoTo(long iRec);
		void SplitPartsArray(const PolygonStruct& polShape, PolygonMap& polMap, Array<long>& polygons, Array<long>& islands);
       
 };       


