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
/* impseq.c
   Import from InfoCam
   by Martin Schouwenburg, february, 1997
   (c) ILWIS Department ITC
   Last change:  MS   11 Feb 98   12:22 pm
*/
#include "Headers\toolspch.h"
#include "Engine\DataExchange\Convloc.h"
#include "Engine\DataExchange\IMPSEQ.H"

#define rMIN(a,b) (((a<=b && a!=rUNDEF) || b==rUNDEF) ? a : b)
#define rMAX(a,b) ((a>=b && a!=rUNDEF) ? a : b)

void ImpExp::ImportInfocam2Ilwis(const FileName& fnFile, const FileName& fnObject)
{
    Infocam2IlwisConv conv(fnFile, fnObject);

    conv.Import();  // beeuh flauw hoor
}

//-----------------------------------------------------------------------------------------------
MapEntry::MapEntry()
{}

MapEntry::~MapEntry()
{
    for (SLIterP<TablePart> iter2(&PartsList); iter2.fValid();++iter2)
    {
        delete iter2();
    }
}
//-----------------------------------------------------------------------------------------------
TablePart::TablePart(const FileName& path, const String& sN): fHasClasses(false),    // creates a domain for a Column in the attr table
                                                        rMin(rUNDEF),
                                                        rMax(rUNDEF)
{
    sName=sN;
    FileName fn2(sN.sTrimSpaces().scVal(), ".dom");

    FileName fnDomain(FileName::fnUnique(fn2).sFullName(), path);
    ColDomain = Domain(fnDomain, 0, dmtID); //
    ColDomain->pdsrt()->dsType = DomainSort::dsMANUAL;
}

void TablePart::Add(String& sV)                        // adds a value to the column domain
{
    double rV=sV.sTrimSpaces().rVal();
    if ( rV != rUNDEF )                                 // if not undef it could be a value domain
    {
        rMin=rMIN(rMin, rV);
        rMax=rMAX(rMax, rV);
    }
    else                                                // nop it is not a value domain
        fHasClasses=true;

    if (ColDomain->pdsrt()->iRaw(sV) == iUNDEF)
      ColDomain->pdsrt()->iAdd(sV);
}

TablePart::~TablePart()
{
}

//-------------------------------------------------------------------------------------------------
Infocam2IlwisConv::Infocam2IlwisConv(const FileName& fn1, const FileName& fn2) : fnObject(fn2),
                                                           iIslands(0),
                                                           iNthTopol(0),
                                                           area(NULL)
{
    inFile = new File(fn1, facRO);
}

Infocam2IlwisConv::~Infocam2IlwisConv()
{
	if (area)
		delete area;
}

void Infocam2IlwisConv::Import()
{
    long iLines = InitScan();
    if (iLines!=iUNDEF) Convert(iLines);
}

void  Infocam2IlwisConv::AddToMapDomain(icMapType MapType, String& sVal)               // creation and adding to the domain of map and attr table
{
    if (!maps[MapType].MapDomain.fValid() )      // create a domain if it does not exist
    {
        FileName fnDomain = FileName::fnUnique(FileName(fnObject, ".dom"));
        maps[MapType].MapDomain = Domain(fnDomain, 0, dmtID); //
        maps[MapType].MapDomain->pdsrt()->dsType = DomainSort::dsMANUAL;
    }
    if (maps[MapType].MapDomain->pdsrt()->iRaw(sVal) == iUNDEF)
      maps[MapType].MapDomain->pdsrt()->iAdd(sVal);
}

void Infocam2IlwisConv::AddToAttributeTables(icMapType MapType)
{
    SListP<TablePart> &PartList = maps[MapType].PartsList;
    for (SLIter<String> iter(&attrList); iter.fValid();++iter)   // for every attribute that is found
    {
        bool found = false;
        String sColName = iter();
        ++iter; // double steps because attr entries consist always of two strings (id + value)
        String sVal = iter();
        for (SLIterP<TablePart> iter2(&PartList); iter2.fValid();++iter2)   // look it up in the existing domains
        {
            TablePart *pt=iter2();
            if ( pt->sName == sColName)   // Column exists so value maybe added to existing domain
            {
                found=true;
                pt->Add(sVal);
                break;
            }
        }
        if ( !found )                                     // it does not exist so add an new domain/name
        {
            TablePart *tp = new TablePart(fnObject, sColName);      // create new TablePart
            tp->Add(sVal);                                // and add value to domain;
            PartList.append(tp);                          // add the part to the partlist
        }
    }
}

void Infocam2IlwisConv::AddToDomainAndCb(icMapType& MapType, CoordBuf& cBuf, short iCount, icAction act)
{
    if ( iCount==0 ) return;
    String sClass;
    if ( act!=icIsland )
    {
        sClass = String("%d",iTotal++);
        AddToMapDomain(MapType, sClass);
        AddToAttributeTables(MapType);
    }        
    for(short i = 0; i < iCount; ++i )
			maps[MapType].cb += cBuf[i];

    if ( act != icIsland) // attributes must be kept if we are busy reading an island (rest attribs is after(!) island
        for (SLIter<String> iter(&attrList); iter.fValid();) iter.remove();
}

void Infocam2IlwisConv::CreateAttributeTables(icMapType MapType)
{
    FileName fnDomain = FileName::fnUnique(FileName(fnObject, ".tbt"));
    maps[MapType].attrTable = Table(fnDomain, maps[MapType].MapDomain);       // create a table with the MapDomain
    SListP<TablePart> &PartList = maps[MapType].PartsList;
    for (SLIterP<TablePart> iter(&PartList); iter.fValid();++iter)    // add columns to the table
    {
        DomainSort *pds = (iter()->ColDomain)->pdsrt();
        if ( pds->iSize() > 0 && iter()->fDomainSort() )                // if there are no values in the domain
        {
            DomainValueRangeStruct dvs(iter()->ColDomain);
            Table tab = maps[MapType].attrTable;
            Column col = tab->colNew(iter()->sName, dvs);      // add a new column with the existing domain
            col->Fill();
        }
        if (!iter()->fDomainSort() )
        {  
            Domain dom = iter()->ColDomain;
            dom->fErase = true;
            FileName fnDomain = FileName::fnUnique(FileName(iter()->sName.scVal() , ".dom"));
            Domain dm(fnDomain, iter()->rMin, iter()->rMax, 0.01);
            DomainValueRangeStruct dvs(dm);
            Table tab = maps[MapType].attrTable;
            Column col= tab->colNew(iter()->sName, dvs);
            col->Fill();
        }
    }
     maps[MapType].map->SetAttributeTable(maps[MapType].attrTable);
}

long Infocam2IlwisConv::InitScan()
{
    int n;
    iTotal=0;
    icAction Action=icNextHeader;
    icMapType MapType;
    CoordBuf cBuf(1000);
    char record[MAX_RECORD_SIZE];
    short iCount=0;
    long iLines=0;
    long iFileSize=inFile->iSize();
    long iBytesRead=0;
    inFile->Seek(0);
    trq.SetTitle(SCVTextScanningFile);
    trq.SetText(SCVTextScanning);
    while(!inFile->fEof())
    {
        if (Action != icNewElement) 
        {
            n = inFile->ReadLnAscii(record, MAX_RECORD_SIZE);
            ++iLines;
            iBytesRead+=n;
        }
        record[n+1]='\0';
        String sRecord("%s", record);
        if ( Action == icNextHeader || Action == icNewElement)
        {
            iCount=0;
            ClearBuf(cBuf);
            iIslands=0;
            Action = ElementType(sRecord, MapType);
        }
        else
            Action = DataRecord(sRecord, cBuf, iCount);

        if ( Action >= icNewElement )
                AddToDomainAndCb(MapType, cBuf, iCount, Action);
        if ( Action == icFlushBuffer )
        {
            iTotal--;
            iCount=0;
        }
        if ( Action==icIsland )
            iIslands++;

        if ( trq.fUpdate(iBytesRead/1000, iFileSize/1000 )) return iUNDEF;
    }
    // last element must still be written
//    AddToDomainAndCb(MapType, cBuf, iCount, Action);
    for ( int i=icPointMap; i<icNone ; ++i )
    {
        if (maps[i].MapDomain.fValid())
        {
            CoordSystem csUnknown;
            DomainValueRangeStruct dvs(maps[i].MapDomain);
            double rXdif=(maps[i].cb.MaxX() - maps[i].cb.MinX())*0.05;
            double rYdif=(maps[i].cb.MaxY() - maps[i].cb.MinY())*0.05;
            CoordBounds cb(Coord(maps[i].cb.MinX()-rXdif, maps[i].cb.MinY()-rYdif),
                           Coord(maps[i].cb.MaxX()+rXdif, maps[i].cb.MaxY()+rYdif));
            if      ( i == icPointMap   ) maps[i].map = PointMap(fnObject, csUnknown, cb, dvs);
            else if ( i == icSegmentMap ) maps[i].map = SegmentMap(fnObject, csUnknown, cb, dvs);
            else if ( i == icPolygonMap )
			{
				PolygonMap mp(fnObject, csUnknown, cb, dvs);
				mp->TopologicalMap(false);
				maps[i].map = mp;

			}
            maps[i].map->sDescription = SCVTextImportingSEQMap;
            maps[i].map->sDescription &= fnObject.sFile;
            maps[i].map->Store();
            CreateAttributeTables((Tp)i);
        }
    }

    return iLines;
}

void Infocam2IlwisConv::ClearBuf(CoordBuf& cBuf)
{
    Coord c;
    for(short i=0; i<1000; ++i)
        cBuf[i]=c;
}

void Infocam2IlwisConv::Convert(long iLines)
{
    int n;
    iTotal=0;
    iIslands=0;
    icAction Action=icNextHeader;
    icMapType MapType;
    char record[MAX_RECORD_SIZE];
    CoordBuf cBuf(1000);
    short iCount=0;
    long iCurLine=0;
    trq.SetTitle(SCVTitleImportInfocam);
    trq.SetText(SCVTextProcessing);
    inFile->Seek(0);
    while(!inFile->fEof())
    {
        if (Action != icNewElement)
        {
             ++iCurLine;
             n = inFile->ReadLnAscii(record, MAX_RECORD_SIZE);
        }
        record[n+1]='\0';
        String sRecord("%s", record);
        if ( Action == icNextHeader || Action == icNewElement)
        {
            iCount=0;
            ClearBuf(cBuf);
            Action = ElementType(sRecord, MapType);
        }
        else
            Action = DataRecord(sRecord, cBuf, iCount);

        if ( Action >= icNewElement )  // last record is a new Element so last element is saved
                 Write(MapType, cBuf, iCount, Action);
        if ( Action >= icFlushBuffer )   // clear buffers and connect previous segments
        {
            iCount=0;
            Coord old=cBuf[995];
            if (Action==icFlushBuffer ) // islands need not to be connected
            {
                cBuf[0]=old;  //connect to the last segment
                iCount=1;
                iTotal--;
            }

        } // 1000 pnt's per segment
        if ( Action==icIsland )
        {
            iNthTopol=0;
            iIslands++;
        }
        if (trq.fUpdate(iCurLine, iLines)) return;
    }
    // last element must still be written
    //if ( MapType!= icPolygonMap)
    //    Write(MapType, cBuf, iCount, Action);
}

void Infocam2IlwisConv::AddAttributes(icMapType MapType, const String& sRaw)
{
	for (SLIter<String> iter(&attrList); iter.fValid();++iter)
    {
        String sColumn=iter();
        ++iter; // second iteration the list contains 2 entries for every attributes (id + value)
        String sValue=iter();
        Table tab = maps[MapType].attrTable;
				DomainSort *pds = tab->dm()->pdsrt();
        long iKey = pds->iKey(pds->iOrd(sRaw));
        Ilwis::Record rec = tab->rec(iKey);
        long iCol = rec.iCol(sColumn);
        rec.PutVal(iCol, sValue);
    }
}

void Infocam2IlwisConv::Write(icMapType& MapType, CoordBuf& cBuf, short iCount, icAction act)
{
    if ( iCount==0 ) return;

    if ( iIslands <1) // no islandgets a code
        sCurrentClass = String("%d",iTotal++);
    if (maps[MapType].MapDomain.fValid())
    {
        BaseMap map=maps[MapType].map;
        long iRaw =map->dm()->iRaw(sCurrentClass);  // get raw value from domain and put it in the map
        if ( (iRaw != iUNDEF && !(cBuf[0] == crdUNDEF)) ||
             iIslands>0)
        {
            if (MapType == icPointMap)
            {
                PointMapPtr *pntmap = dynamic_cast<PointMapPtr *>(map.ptr());
                if(pntmap) pntmap->iAddRaw(cBuf[0], iRaw);
            }
            else if( MapType == icSegmentMap)
            {
                SegmentMapPtr *segmap = dynamic_cast<SegmentMapPtr *>(map.ptr());
                Segment segCur = segmap->segNew();
                segCur.PutCoords(iCount, cBuf);
                segCur.PutVal(sCurrentClass);
            }
            else if( MapType == icPolygonMap)
            {
                PolygonMapPtr *polmap=dynamic_cast<PolygonMapPtr *>(map.ptr());
                FillPolygon(polmap, cBuf, iCount, sCurrentClass, act); // add points to existing polygon
        
                if ( act==icNewElement || inFile->fEof())
                   ClosePolygon(polmap); // if last point encountered close the polygon
	        }
            AddAttributes(MapType, sCurrentClass);
        }
    }
    for (SLIter<String> iter(&attrList); iter.fValid();) iter.remove();
}

void Infocam2IlwisConv::FillPolygon(PolygonMapPtr *polmap, CoordBuf& cBuf, 
                                    int iCount, String sC, icAction act)
{
    SegmentMap sm = polmap->smp();
    if ( !area) area=new PolArea();
    
    if (!sm.fValid())
      return;
    int iStart=0;
    if (iNthTopol != 0 )
        iStart=1;
    CoordBuf buf(iCount+iStart);
    if (iNthTopol != 0 )
        buf[0]=crdLastOld;
    if ( iIslands>0) sC="";  // islands do not have codes      

    for ( short i=iStart; i<iCount+iStart ;++i )
    {
            buf[i]=cBuf[i-iStart];
    }
    crdLastOld=buf[iCount+iStart-1];
    tCurSectionLast = polmap->AddNonTopolTopology(sC, buf, tCurSectionLast, polLast, iNthTopol++ );
    area->Top(tCurSectionLast);
    if ( act==icIsland || act==icNewElement) // next data will be a new island. Process current buffers
    {
        if (!tCurSectionFirst.fValid()) // only one segment found
            tCurSectionFirst=tCurSectionLast; // starting a section must set the first segment
        SetEdgeDirection(area->rArea()); // turn the section and set right and left
        if ( iIslands>0) // already busy with an islands
            LinkIsland();
        area->Reset();
        tPrevSectionLast=tCurSectionLast;
        if ( act != icNewElement) // invalidate; new island is comming, Cur has no meaning
        {                       // some information has to be retained if the polygon needs to be closed
            tPrevSectionFirst=tCurSectionFirst;
            tCurSectionFirst=tCurSectionLast=Topology();    
        }            
    }
}

void Infocam2IlwisConv::ClosePolygon(PolygonMapPtr *polmap)
{

    if ( iIslands>0)
    {
        tCurSectionLast.Fwl(tCurSectionFirst.iCurr());
        tCurSectionFirst.Bwl(polLast.topStart().iCurr());
    }
   
    polmap->CalcLengthAndAreaAndBounds(polLast.iCurr());
    iNthTopol=0;
    iIslands=0;
    tCurSectionFirst=tCurSectionLast=tPrevSectionFirst=tPrevSectionLast=Topology(); // invalidate cur
}

icAction Infocam2IlwisConv::ElementType(String& sRecord, icMapType& MapType)
{
    int iToken=0;
    //if ( sRecord.sSub(ST_ATTRIB-1, EN_ATTRIB - ST_ATTRIB) == ATTRIB_ID_HEADER)  return icGetData;
    RecordType(sRecord, MapType, iToken);
    if ( MapType == icNone ) return icNextHeader;
    RecordCode(sRecord, iToken);
    return icGetData;
}

void Infocam2IlwisConv::RecordType(const String& sRecord, icMapType& MapType, int& iToken)
{
    MapType=icNone;
    String sH("%S", sRecord.sLeft(HEADER_LENGTH));
    if ( sH.sLeft(LINE_ID_LEN)        == LINE_ID_HEADER   ) { MapType = icSegmentMap;   iToken+=4; }
    else if ( sH.sLeft(EDGE_ID_LEN)   == EDGE_ID_HEADER   ) { MapType = icSegmentMap;   iToken+=4; }
    else if ( sH.sLeft(SYMBOL_ID_LEN) == SYMBOL_ID_HEADER ) { MapType = icPointMap;     iToken+=6; }
    else if ( sH.sLeft(REGION_ID_LEN) == REGION_ID_HEADER ) { MapType = icPolygonMap;   iToken+=6; }
}

void Infocam2IlwisConv::RecordCode(const String& sRecord, int& iToken)
{
    String sCode=Strip(sRecord.sSub(iToken+1, CODE_LENGTH));
    attrList.append(EXT_INFOCAM_CODE);
    attrList.append(sCode);
    iToken += CODE_LENGTH+1;
}

int Infocam2IlwisConv::iGetCoord(String& sCoord, String sRecord, int iStart)
{
    int iCharProcessed=0, i,j=0;
    char cPos[100];
    bool fFirstNumberSeen=false;
    for ( i=iStart; i<sRecord.length() && (sRecord[i]!=' ' || fFirstNumberSeen!=true); ++i)
    {
        ++iCharProcessed;
        if ( sRecord[i] !=' ' )
        {
            cPos[j++]=sRecord[i];
            fFirstNumberSeen=true;
        }
    }
    cPos[j]=0;
    sCoord=String(cPos);
    return iCharProcessed;
}

icAction Infocam2IlwisConv::DataRecord(const String& sRecord, CoordBuf& cBuf, short& iCount)
{
		String sE, sN;
    if ( sRecord.sLeft(ST_DATA) != EDGE_ID_HEADER)//
 //      sRecord.sLeft(COG_LENGTH) != CENTER_OF_GRAVITY)
    {
        if ( sRecord.sLeft(ST_DATA-1) != "   " || inFile->fEof()) // check on EOF takes care of writing of last element
					return icNewElement;

        if ( iCount > 995 ) return icFlushBuffer; // segments must be smaller than 1000 pnts's
        String sPRT= sRecord.sSub(ST_DATA-1,EN_PNT_DATA-ST_DATA+1);
        if ( sPRT == "C " || sPRT == "C1" )  // c & c1 records contains usable information
        {
             int n=iGetCoord(sE, sRecord, ST_PNT_EAST-1);
             iGetCoord(sN, sRecord, ST_PNT_EAST + n);
             double easting, northing;
             sscanf(sE.scVal(),"%lf", &easting);
             sscanf(sN.scVal(), "%lf", &northing);
             cBuf[iCount++] = Coord(easting, northing);
        }
        if ( sRecord.sSub(ST_DATA-1, ISLAND_ID_LEN)==ISLAND_ID_HEADER)
        {
            iNthTopol=0; // counting is no longer needed. Leg has been finished.
            return icIsland;

        }
        if ( sRecord.sSub(ST_ATTRIB-1, EN_ATTRIB - ST_ATTRIB+1) == ATTRIB_ID_HEADER)
        {
            String sCod = sRecord.sSub(ST_ATTRIB_CODE-1,   EN_ATTRIB_CODE - ST_ATTRIB_CODE + 1);
            String sLen = sRecord.sSub(ST_ATTRIB_LENGTH-1, EN_ATTRIB_LENGTH - ST_ATTRIB_LENGTH + 1);
            String sHead = sCod.sLeft(2);
            if ( sHead != "S_" && sHead != "L_" && sHead != "R_" ) // system attributes that are not needed
            {
                long iLen=sLen.sTrimSpaces().iVal();
                String sVal = sRecord.sSub(ST_ATTRIB_VAL-1,    iLen);
                attrList.append(sCod.sTrimSpaces());
                attrList.append(sVal.sTrimSpaces());
            }
         }

    }
    return icGetData;
}

String Infocam2IlwisConv::Strip(String& sS)
{
    int iEnd=0, iStart=0, iLen=sS.length();
    for ( short i=0; i<iLen;++i )
        if ( sS[i]!=' ' ) { iStart=i; break; }
	short j=iLen-1;
    for ( ; j >= 0 ; --j )
        if ( sS[j]!=' ' ) { iEnd=j; break; }

    return sS.sSub(iStart, j+1);
}

void Infocam2IlwisConv::SetEdgeDirection(double rArea)
{
    Topology topCur=tCurSectionFirst;
    long iNr;
    do  
    {
        if ( rArea<0)
        {
            CoordBuf crdBuf(1000), crdBuf2(1000);
            topCur.GetCoords(iNr, crdBuf);
            for ( int i=iNr-1; i>=0; --i)
            {
                crdBuf2[iNr-1-i]=crdBuf[i];
            }
            topCur.PutCoords(iNr, crdBuf2);
        }            
        if ( iIslands==0)
        {
            topCur.LeftPol(polLast.iCurr());
            topCur.RightPol(-1);
        }
        else
        {
            topCur.RightPol(polLast.iCurr());
            topCur.LeftPol(-1);
        }
        topCur = topCur.topNext();
            
    }
    while ( topCur.iCurr()!=0 && tCurSectionFirst.crdBegin() != topCur.crdEnd());
}
        

void Infocam2IlwisConv::LinkIsland()
{
    if ( iIslands==1)
    {
        tPrevSectionLast.Fwl(-tCurSectionLast.iCurr());
        tPrevSectionLast.Bwl(-tPrevSectionFirst.iCurr());
    }
    else
    {
        tPrevSectionLast.Fwl(tPrevSectionLast.iCurr());
        tPrevSectionLast.Bwl(-tCurSectionLast.iCurr());
    }
}    



