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
/* impseq.h
   Import from InfoCam
   by Martin Schouwenburg, february, 1997
   (c) ILWIS Department ITC
   Last change:  MS   26 Jan 98   10:36 am
*/

#ifndef IMPSEQ_H
#define IMPSEQ_H

#include "Engine\DataExchange\INFOCAM.H"
#include "Engine\Map\Polygon\POLAREA.H"


class TableParts;
class MapEntry;
class Infocam2IlwisConv;



typedef enum Act{icNextAction, icGetData, icNextHeader, icNewElement, icFlushBuffer, icIsland } icAction;
typedef enum Tp{icPointMap, icSegmentMap, icPolygonMap, icNone } icMapType;

class TablePart
{
    friend MapEntry;
    friend Infocam2IlwisConv;

    public:
        TablePart(const FileName& path, const String& sName);
        ~TablePart();
        void    Add(String& sV);
        bool fDomainSort() { return fHasClasses || sName==EXT_INFOCAM_CODE; }

    protected:
        String      sName;
        Domain      ColDomain;
        double      rMin;
        double      rMax;
        bool        fHasClasses;
};

class MapEntry
{
    friend Infocam2IlwisConv;

    public:
        MapEntry();
        ~MapEntry();

    protected:

         SListP<TablePart>   PartsList;

        Domain              MapDomain;
        Table               attrTable;
        BaseMap             map;
        CoordBounds         cb;
};

class Infocam2IlwisConv
{
    public:
        Infocam2IlwisConv(const FileName& fn1, const FileName& fn2);
        ~Infocam2IlwisConv();

        void Import();

    protected:
        File            *inFile;
        FileName        fnObject;


         SList<String>    attrList;

        MapEntry        maps[3];
        short           iBufCount;
        Tranquilizer    trq;
        int            iIslands;

        int              iNthTopol;
        Topology        tCurSectionLast;
        Topology        tCurSectionFirst;
        Topology        tPrevSectionFirst;
        Topology        tPrevSectionLast;
        PolArea         *area; 
        ilwPolygon         polLast;
        Coord           crdLastOld;
        long            iTotal;
        String          sCurrentClass;

        void            Convert(long iLines);
        void            ClearBuf(CoordinateSequence* cb);
        void            ClosePolygon(PolygonMapPtr *polmap);
        long            InitScan();
        String          Strip(String& sS);
        void            AddToMapDomain(icMapType MapType, String& sVal);
        void            AddToAttributeTables(icMapType MapType);
        void            AddToDomainAndCb(icMapType& MapType, CoordinateSequence* cBuf, short iCount, icAction act);
        void            CreateAttributeTables(icMapType MapType);
        void            AddAttributes(icMapType, const String& sRaw);
        void            Init(String& MapType, String& sCode, String& sPointCode, Coord& c);
        void            Write(icMapType& MapType, CoordBuf& cBuf, short iCount, icAction act);
        icAction        ElementType(String& sRecord, icMapType& MapType);
        void            RecordType(const String& sRecord, icMapType& MapType, int& iToken);
        void            RecordCode(const String& sRecord, int& iToken);
        icAction        DataRecord(const String& sRecord, CoordBuf& cBuf, short& i);
        void            FillPolygon(PolygonMapPtr *polmap, CoordBuf& cBuf, int iCount, String sC, icAction act);
        int             iGetCoord(String& sCoord, String sRecord, int iStart);
        void           SetEdgeDirection(double rArea);
        void           LinkIsland(); 
        
};

#endif


