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
/* expseq.h
   Export to InfoCam
   by Martin Schouwenburg, february, 1997
   (c) ILWIS Department ITC
   Last change:  MS   21 Nov 97   10:20 am
*/

#ifndef EXPSEQ_H
#define EXPSEQ_H


#include "Engine\DataExchange\INFOCAM.H"

class Ilwis2InfocamConv
{
    public:

        Ilwis2InfocamConv(const FileName& fn);
        ~Ilwis2InfocamConv();

        void        WriteSegment(const DomainValueRangeStruct& dvs, ILWIS::Segment* seg, const Table& tab);
        void        WriteSegmentPoints(const CoordinateSequence *seq);
        void        WritePolygon(const DomainValueRangeStruct& dvs,ILWIS::Polygon* pol, const Table& attTab);
        void        WriteTopology(Topology& top, int iN);
        void        WriteAttribute(const String& sCode, const String& sVal);
        void        OtherAttributes(long iRaw, const Table& attTab);
        void        WriteIslandHeader();
        void        WriteEdge();
        void        WriteSymbol(Coord& c, const String& sV, long iRaw, const Table& attTab);
        void        WritePoint(Coord& c, const String& sV);
        int         CountIslands(const ILWIS::Polygon& pol);
        void        Put(char *buf, int iStart, int iEnd, char *);
        void        Put(char *buf, int iStart, int iEnd, const String& s);

    protected:
        File        *outFile;
        char        *record;
				short       m_iRecSize;

        void        ResizeAndClearRecord(short iSize);
        String       sGetICCCode(const Table& attTab, const String& sDefault, long iRaw);


};

#endif


