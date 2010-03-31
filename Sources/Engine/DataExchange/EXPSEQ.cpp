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
/* $Log: /ILWIS 3.0/Import_Export/EXPSEQ.cpp $
 * 
 * 8     6-03-02 10:32 Willem
 * Prevented indefinitely looping which can appear because of polygon
 * structure not conforming to specification
 * 
 * 7     8/01/01 4:17p Martin
 * Record is now put in a namespace
 * 
 * 6     22/03/00 17:07 Willem
 * Optimized char buffer reuse
 * 
 * 5     16/03/00 16:36 Willem
 * The existence of an attribute table is now checked before checking the
 * domain of this attribute table ...
 * 
 * 4     10-01-00 4:05p Martin
 * removed vector rowcol and changed them to true coords
 * 
 * 3     17-06-99 3:59p Martin
 * // -> /*
 * 
 * 2     17-06-99 2:10p Martin
 * ported files to VS
// Revision 1.4  1998/03/30 16:38:45  martin
// Added some whishes of columbia
// - If there is an ICCCODE column, it will be used for the thematic code
// - Undefs are skipped
// -The original attribute names are used (though this is less secure) instead of generated ones
//
// Revision 1.3  1998/02/20 10:30:45  martin
// Changed enumerated
//
// Revision 1.2  1997/08/07 19:36:23  Willem
// Small changes to prevent compiler warnings 'temporary created'
// Polygons are now only exported when their area is larger than zero
//
/* expseq.c
   Export to InfoCam
   by Martin Schouwenburg, february, 1997
   (c) ILWIS Department ITC
   Last change:  WN    7 Aug 97    4:36 pm
*/
#include "Headers\toolspch.h"
#include "Engine\DataExchange\EXPSEQ.H"

Ilwis2InfocamConv::Ilwis2InfocamConv(const FileName& fn) : record(NULL), m_iRecSize(0)
{
    outFile = new File(fn, facCRT);
}

Ilwis2InfocamConv::~Ilwis2InfocamConv()
{
    outFile->Flush();
    delete outFile;
    if (record) 
			delete [] record;
}

void Ilwis2InfocamConv::ResizeAndClearRecord(short iSize)
{
	if (iSize > m_iRecSize)
	{
    if ( record ) 
			delete [] record;
		m_iRecSize = iSize;
    record = new char[iSize];
	}
	memset(&record[0], ' ', iSize);
  record[iSize-1] = '\n';
}

void Ilwis2InfocamConv::WriteSegment(const DomainValueRangeStruct& dvs, ILWIS::Segment* seg, const Table& attTab)
{
    String sValue=seg->sValue(dvs);
    if ( sValue.sTrimSpaces()=="?") return;
	CoordinateSequence *seq = seg->getCoordinates();
    
    ResizeAndClearRecord(END_LINE_REC);
    String sICCCODE=sGetICCCode(attTab, SEG_CODE, seg->iValue());
    Put(record, ST_LINE,      EN_LINE, LINE_ID_HEADER);
    Put(record, ST_LINE_CODE, EN_LINE_CODE, SEG_CODE);   // Line code
    Put(record, ST_LINE_VAL,  EN_LINE_VAL, " 5");   // validity 5=not defined
    outFile->Write(END_LINE_REC, record);
    WriteSegmentPoints(seq);

    WriteAttribute(DEF_ATTRIB, sValue);
    OtherAttributes(seg->iValue(), attTab);
}

void Ilwis2InfocamConv::WriteSegmentPoints(const CoordinateSequence* seq)
{
    for ( short i=0; i<seq->size() ; ++i )
    {
        WriteEdge();
		WritePoint(Coord(seq->getAt(i)), "");
    }
}

void Ilwis2InfocamConv::WriteAttribute(const String& sCode, const String& sVal)
{
    ResizeAndClearRecord(END_ATTRIB_RECORD);
    Put(record, ST_ATTRIB, EN_ATTRIB, ATTRIB_ID_HEADER);
    Put(record, ST_ATTRIB_CODE, EN_ATTRIB_CODE, sCode);
    Put(record, ST_ATTRIB_LENGTH, EN_ATTRIB_LENGTH, String("%d", sVal.length()));
    Put(record, ST_ATTRIB_VAL, EN_ATTRIB_VAL, sVal);

    outFile->Write(END_ATTRIB_RECORD, record);
}

// searches for a column named ICCCode and uses the value of that column for the element code, else default
String Ilwis2InfocamConv::sGetICCCode(const Table& attTab, const String& sDefault, long iRaw)
{
    String sCode = sDefault;
    if ( ! (attTab.fValid() && attTab->dm()->pdsrt() != NULL) )
			return sCode;
    
    const Column& col = attTab->col(EXT_INFOCAM_CODE);
    if ( col.fValid())
    {
        String sValue = col->sValue(iRaw);
        if (sValue.sTrimSpaces() !="?")  // skip undef values
            sCode=sValue;
    }
    return sCode;
}

void Ilwis2InfocamConv::WritePolygon(const DomainValueRangeStruct& dvs, ILWIS::Polygon* pol, const Table& attTab)
{
    String sValue=pol->sValue(dvs);
    if ( sValue.sTrimSpaces()=="?") return; // undefs cannot be interpreted for Infocam;
    String sICCCODE=sGetICCCode(attTab, POL_CODE, pol->iValue());
    ResizeAndClearRecord(END_REGION_RECORD);
    Put(record, ST_REGION,EN_REGION, REGION_ID_HEADER);
    Put(record, ST_REGION_CODE,EN_REGION_CODE, sICCCODE);  // region code
    Put(record, ST_REGION_VAL, EN_REGION_VAL, " 5");      // validity

    outFile->Write(END_REGION_RECORD, record);
    int iNIslands=pol->getNumInteriorRing();
	CoordBuf crdBuf;
	const LineString *extRing = pol->getExteriorRing();
	const CoordinateSequence *seq = extRing->getCoordinates();


    WriteSegmentPoints(seq);
	for(int i = 0; i < iNIslands; ++i) {
		const LineString *intRing = pol->getInteriorRingN(i);
		seq = intRing->getCoordinates();
		if ( i == 0 )
			WriteIslandHeader();
		WriteSegmentPoints(seq);

	}
    WriteAttribute(DEF_ATTRIB, sValue);
    OtherAttributes(pol->iValue(), attTab);
}

void Ilwis2InfocamConv::OtherAttributes(long iRaw, const Table& attTab)
{
    if ( !attTab.fValid() ) return;                               // if there is a attribute table
                                                                  // its values are added
    DomainSort *dm;
    for(short i=0; i<attTab->iCols(); ++i)
    {
        if ( (dm = attTab->dm()->pdsrt()) == NULL) break;
        long iKey = dm->iKey(dm->iOrd(iRaw));
        Ilwis::Record rec=attTab->rec(iKey);
        String sValue = rec.sValue(i);
        if (sValue.sTrimSpaces()=="?") continue; // skip undef values
        sValue = sValue.sLeft(ATTRIB_LEN);
        String sName = rec.sCol(i);
        sName=sName.sLeft(EN_ATTRIB_CODE - ST_ATTRIB_CODE); 
        //sName&=String("%d", i);                               
        WriteAttribute(sName.toUpper(), sValue);
    }
}

void Ilwis2InfocamConv::WriteIslandHeader()
{
    ResizeAndClearRecord(END_ISLAND_RECORD);
    Put(record, ST_ISLAND,10, ISLAND_ID_HEADER);

    outFile->Write(END_ISLAND_RECORD, record);
}

void Ilwis2InfocamConv::WriteEdge()
{
    ResizeAndClearRecord(END_EDGE_REC);
    Put(record, ST_EDGE_TYPE, EN_EDGE_TYPE, "STRAIG");   // line type (arc, spline, straig);
    Put(record, ST_EDGE_VAL , EN_EDGE_VAL,  " 5");        // validity 5=not defined
    outFile->Write(END_EDGE_REC, record);
}

void Ilwis2InfocamConv::WriteSymbol(Coord& c, const String& sV, long iRaw, const Table& attTab)
{
    ResizeAndClearRecord(END_SYMBOL_REC);
    if ( sV.sTrimSpaces()=="?") return;
    String sICCCODE=sGetICCCode(attTab, POINT_CODE, iRaw);
    Put(record, ST_SYMBOL,      EN_SYMBOL,      SYMBOL_ID_HEADER);
    Put(record, ST_SYMBOL_CODE, EN_SYMBOL_CODE, sICCCODE);          // symbol code
    Put(record, ST_SYMBOL_ROT,  EN_SYMBOL_ROT,  "0.000000"); // rotation
    Put(record, ST_SYMBOL_VAL,  EN_SYMBOL_VAL,  " 5");        // validity 5=not defined
    
    outFile->Write(END_SYMBOL_REC, record);

    WritePoint(c, sV);

    WriteAttribute(DEF_ATTRIB, sV);
    OtherAttributes(iRaw, attTab);
}

void Ilwis2InfocamConv::WritePoint(Coord& c, const String& sV)
{
    ResizeAndClearRecord(END_POINT_REC);

    String sX("%.3f",c.x);
    String sY("%.3f",c.y);
    Put(record, ST_PNT_DATA,  EN_PNT_DATA, POINT_ID_HEADER);
    Put(record, ST_PNT_NAME,  EN_PNT_NAME, sV);     // point name
    Put(record, ST_PNT_EAST,  EN_PNT_EAST, sX);     // easting
    Put(record, ST_PNT_NORTH, EN_PNT_NORTH, sY);    // northing

    outFile->Write(END_POINT_REC, record);
}

void Ilwis2InfocamConv::Put(char buf[], int iStart, int iEnd, char * s)
{
	const String sV(s);
	Put(buf, iStart, iEnd, sV);
}

void Ilwis2InfocamConv::Put(char buf[], int iStart, int iEnd, const String& s)
{
    int j=0;
    int l=s.size();
    for ( int i=iStart-1; j<l && i<= iEnd-1 ; ++i )
    {
        buf[i]=s[j++];
    }
}



