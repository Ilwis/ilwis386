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
  shapefile format
  by Li Fei, March 96
  ILWIS Department ITC
	Last change:  WN    1 Aug 97   10:55 am
*/

#ifndef SHPFILE_H
#define SHPFILE_H

#pragma pack(4)
class Shapefile {
private:
  struct Record_Header {
    long iRecordNumber;
    long iContentLength;
  };

  struct Index_Record {
    long iOffset;
    long iContentLength;
  };

  struct File_Header{
    long  iFileCode;
    long  iUnused1[5];
    long  iFileLength;
    long  iVersion;
    long  iShapeType;
    double rBoundMinX;
    double rBoundMinY;
    double rBoundMaxX;
    double rBoundMaxY;
    long  iUnused2[8];
    File_Header();
  };

public:
  File_Header FileHeader;
  Record_Header RecordHeader;
  Index_Record IndexRecord;
  void SetFileHeader();
  void SetIndexFileHeader();
private:
  long iRec;
protected:
  Shapefile(File&);
  void SetFileBounds(const Coord& cdMin, const Coord& cdMax);
  void Update();
  void SwabLong(long& l);
  void SetContentLength(long iConLen);
  File& fileSHP;
};

class Shapefile_Point : public Shapefile
{
private:
  struct Point{
    long iShapeType;
    Coord cd;
    Point();
  };
public:
  Point aPoint;
  Shapefile_Point(const PointMap&, File& fileSHP);
  void Update(const Coord&);
  void PointWrite();
};


class Shapefile_Arc : public Shapefile
{
private:
  struct Arc{
    long iShapeType;
    double rBox[4];
    long iNumParts;
    long iNumPoints;
    long iBeginPoint;
    CoordinateSequence *crdbuf;
    Arc();
  };
  const SegmentMap _sm;
public:
  Arc aArc;
  Shapefile_Arc(const SegmentMap&, File& fileSHP);
  void Update(const ILWIS::Segment*);
  void ArcWrite();
};
#pragma pack()


class Shapefile_Pol : public Shapefile
{
private:
  long iShapeType;
  double rBox[4];
  Array<long> aiBeginPoint;
  CoordBuf cbufseg;
  const PolygonMap _pm;
public:
  Shapefile_Pol(const PolygonMap&, File& fileSHP);
  void PolWrite(const vector<Coordinate>& coords, int NumParts, int NumPoints);
  void Update(const ILWIS::Polygon*);
};

#endif

