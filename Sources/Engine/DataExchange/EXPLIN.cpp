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
/* $Log: /ILWIS 3.0/Import_Export/EXPLIN.cpp $
 * 
 * 5     16/03/00 16:10 Willem
 * - Replaced the obsolete trq.Message() function with Show() function
 * - cuts large segments into properly sized multiple segments
 * 
 * 4     10-01-00 4:05p Martin
 * removed vector rowcol and changed them to true coords
 * 
 * 3     17-06-99 3:59p Martin
 * // -> /*
 * 
 * 2     17-06-99 2:10p Martin
 * ported files to VS
// Revision 1.5  1998/04/07 18:09:22  Willem
// Maps with domain value are now exported with the real values, if
// the precision is one digit maximum. Otherwise the segment ID is
// exported. (LIN understands only integer values, numbers with one
// decimal are rounded)
//
// Revision 1.4  1998/03/13 19:18:26  Willem
// Segments are now exported only when the iRaw value is not iUNDEF.
//
// Revision 1.3  1997/11/24 13:08:24  janh
// In WriteSegment(seg,dm) I changed if statement so that defined segments
// are exported getting seg->iCurr as (unique) ids, undef segs get code 0
//
// Revision 1.2  1997/09/29 13:57:14  Willem
// Export to LIN now only writes ID, no values anymore.
//
/* explin.c
   Export ArcInfo Generate file
   by Willem Nieuwenhuis, june 3, 1996
   ILWIS Department ITC
   Last change:  WN   13 Mar 98    3:18 pm
*/

#include "Headers\toolspch.h"
#include "Engine\DataExchange\Convloc.h"
#include "Headers\Hs\CONV.hs"
#include "Engine\Map\Segment\seg.h"
#include "Engine\Domain\dm.h"
#include "Engine\Base\DataObjects\valrange.h"

class ArcInfoLinExporter 
{
public:
  ArcInfoLinExporter(const FileName&);
  ~ArcInfoLinExporter();
  void WriteSegment(const ILWIS::Segment*, const Domain&, int index);
  void WriteTrailer();
private:
  File* fileArcInfo;
  CoordBuf cbuf;
};

ArcInfoLinExporter::ArcInfoLinExporter(const FileName& fnFile) 
{
  fileArcInfo = new File(fnFile, facCRT);
}

ArcInfoLinExporter::~ArcInfoLinExporter() 
{
  if (fileArcInfo) 
		delete fileArcInfo;
}

void ArcInfoLinExporter::WriteSegment(const ILWIS::Segment* seg, const Domain& dm, int index) {
  CoordinateSequence *cbuf = seg->getCoordinates();
  long iNrCoord = cbuf->size();

  DomainValueRangeStruct dvs(dm);
  double rVal = rUNDEF;
  String sVal = String("%10li",index);
  if (seg->iValue() != iUNDEF)   // skip segments with iUNDEF
	{
    if (dvs.fValues()) 
		{
      rVal = seg->rValue();
      if (dvs.fRealValues())  // LIN understands only integer values
			{
        if (dvs.vr().fValid() && dvs.vr()->rStep() < 0.1)
          rVal = rUNDEF;
      }  
      if (rVal != rUNDEF)
        sVal = String("%11.0f", rVal);
    }

		// Write the segment:
		//   Split segments in parts of maximum 500 points; this
		//   is the maximum in Arc/Info
		long iToDo;
		long iOff = 0;
		while (iNrCoord > 0) 
		{
			fileArcInfo->WriteLnAscii(sVal);
			if (iNrCoord < 490 + 10)     // '+ 10' to be certain last segment part has a least 10 points
				iToDo = iNrCoord;
			else
				iToDo = 490;

			for (long i = 0; i < iToDo; i++)
				fileArcInfo->WriteLnAscii(String(" %15.6lf %15.6lf", cbuf->getAt(i + iOff).x, cbuf->getAt(i + iOff).y));
			fileArcInfo->WriteLnAscii("END");

			iOff += iToDo - 1;  // node needs to be done twice
			iNrCoord -= iToDo;
			if (iNrCoord > 0) 
				iNrCoord++;  // '++' because intermediate node needs to be done twice
		}
  }
  delete cbuf;
}

void ArcInfoLinExporter::WriteTrailer() {
  fileArcInfo->WriteLnAscii("END");
}

void ImpExp::ExportLIN(const FileName& fnObject, const FileName& fnFile) 
{
	try 
	{
		trq.SetTitle(TR("Exporting to Arc/Info Generate"));
		trq.SetText(TR("Processing..."));
		SegmentMap sm(fnObject);
		long iSegCnt = 0, iNrSegs = sm->iFeatures();
		ArcInfoLinExporter aile(fnFile);
		Domain dm = sm->dm();
		ILWIS::Segment *seg = sm->segFirst();
		while (seg && seg->fValid()) 
		{
			aile.WriteSegment(seg, dm, iSegCnt);
			iSegCnt++;

			if (trq.fUpdate(iSegCnt, iNrSegs))
				return;
			seg = (ILWIS::Segment *)sm->getFeature(iSegCnt);
		}
		aile.WriteTrailer();
		trq.fUpdate(iNrSegs, iNrSegs);
	}
	catch (ErrorObject& err) 
	{
		err.Show();
	}
}


