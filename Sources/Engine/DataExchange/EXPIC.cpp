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
/*$Log: /ILWIS 3.0/Import_Export/EXPIC.cpp $
 * 
 * 3     17-06-99 3:59p Martin
 * // -> /*
 * 
 * 2     17-06-99 2:10p Martin
 * ported files to VS
//Revision 1.3  1998/09/17 09:13:06  Wim
//22beta2
//
//Revision 1.2  1997/08/07 19:38:11  Willem
//Polygons are now exported only if their area is larger than zero
//
/* expic.c
   Export to InfoCam
   by Martin Schouwenburg, february, 1997
   (c) ILWIS Department ITC
   Last change:  WN    7 Aug 97    4:38 pm
*/

#include "Headers\toolspch.h"
#include "Engine\DataExchange\Convloc.h"
#include "Headers\Hs\CONV.hs"
#include "Engine\Map\Point\PNT.H"
#include "Engine\Domain\dm.h"
#include "Engine\Domain\dmsort.h"
#include "Engine\DataExchange\INFOCAM.H"
#include "Engine\DataExchange\EXPSEQ.H"

void ImpExp::ExportPnt2Infocam(const FileName& fnObject, const FileName& fnFile)
{
    trq.SetTitle(SCVTitleExpPntInfocam);
    trq.SetText(SCVTextProcessing);
    PointMap pm(fnObject);
    long iNrPoints=pm->iFeatures();
    if ( iNrPoints==0 )
    {
        trq.fUpdate(iNrPoints, iNrPoints);
        return;
    }
    Ilwis2InfocamConv ipc(fnFile);
    Table tab;
    if ( pm->fTblAtt() ) tab=pm->tblAtt();
    for ( long i=1; i<=iNrPoints ; ++i )
    {
         ipc.WriteSymbol(pm->cValue(i), pm->sValue(i), pm->iRaw(i), tab);
        if (trq.fUpdate(i, iNrPoints))
            return;
    }
}

void ImpExp::ExportSegment2Infocam(const FileName& fnObject, const FileName& fnFile)
{
    trq.SetTitle(SCVTitleExpSegInfocam);
    trq.SetText(SCVTextProcessing);
    SegmentMap segmap(fnObject);
    if ( !segmap.fValid() ) return; // foutmelding geven
    long i=0,iNrSegs = 0;
    ILWIS::Segment *seg = segmap->segFirst();
	while (seg && seg->fValid()) 
    {
        if (!seg->fDeleted())
            iNrSegs++;
        seg = (ILWIS::Segment *)segmap->getFeature(++i);
    }
    Ilwis2InfocamConv isc(fnFile);
    seg = segmap->segFirst();
	i = 0;
    Table tab;
    if ( segmap->fTblAtt() ) tab=segmap->tblAtt();
    while ( seg && seg->fValid() )
    {
        if ( !seg->fDeleted() )
			isc.WriteSegment(segmap->dvrs(), seg, tab);
        seg = (ILWIS::Segment *)segmap->getFeature(++i);
        if(trq.fUpdate(i++,iNrSegs)) break;
    }
}

void ImpExp::ExportPolygon2Infocam(const FileName& fnObject, const FileName& fnFile)
{
    trq.SetTitle(SCVTitleExpPolygonInfocam);
    trq.SetText(SCVTextProcessing);
    PolygonMap polmap(fnObject);
    if ( !polmap.fValid() ) return; // foutmelding geven
    long iPol = polmap->iFeatures();
    Ilwis2InfocamConv isp(fnFile);
    ILWIS::Polygon* pol = polmap->polFirst();
    long i = 0;
    Table tab;
    if (polmap->fTblAtt()) tab = polmap->tblAtt();
    while ( pol && pol->fValid() )
    {
        if (pol->rArea() > 0) {
          isp.WritePolygon(polmap->dvrs(), pol, tab);
          if (trq.fUpdate(i, iPol))
            break;
        }
        pol = (ILWIS::Polygon *)polmap->getFeature(++i);
    }
}


