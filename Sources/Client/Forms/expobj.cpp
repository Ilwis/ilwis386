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
/* ExportObject
   by Wim Koolhoven
   (c) Ilwis System Development ITC
	Last change:  WK    5 Dec 96   12:04 pm
*/

#include "Client\Headers\AppFormsPCH.h"
#include "Client\ilwis.h"
#include "Engine\Map\Raster\Map.h"
#include "Engine\Map\Segment\Seg.h"
#include "Engine\Map\Polygon\POL.H"
#include "Engine\Map\Point\PNT.H"
#include "Engine\Table\tbl.h"
#include "Headers\hourglss.h"
#include "Client\Forms\expobj.h"
#include "Headers\Htp\Ilwismen.htp"
#include "Headers\Hs\IMPEXP.hs"
#include "Client\FormElements\fldtbl.h"

void ExportObject(CWnd* wPar, const FileName& fn, const String& s)
{
	try {
		Tranquilizer trq;
		trq.SetTitle(SIETitleExportTo14);
		trq.SetDelayShow(false);
		trq.SetNoStopButton(true);
		trq.fText(SIEMOpeningGDB_S);
		if (".mpr" == fn.sExt) {
			Map mp(fn);
			ExportObject(wPar,mp,s);
		}  
		else if (".mps" == fn.sExt) {
			SegmentMap mp(fn);
			ExportObject(wPar,mp,s);
		}  
		else if (".mpa" == fn.sExt) {
			PolygonMap mp(fn);
			ExportObject(wPar,mp,s);
		}  
		else if (".mpp" == fn.sExt) {
			PointMap mp(fn);
			ExportObject(wPar,mp,s);
		}  
		else if (".tbt" == fn.sExt) {
			Table tbl(fn);
			ExportObject(wPar,tbl,s);
		}  
		else
		{
			wPar->MessageBox(SIEErrImpossibleExpOpr.scVal(), SIEErrError.scVal(), MB_OK | MB_ICONEXCLAMATION);
		}
		trq.Stop();
	}
	catch (ErrorObject& err) {
		err.Show();
	}    
}

void ExportObject(CWnd* wPar, Map& mp, const String& s)
{
  class ExportMapForm: public FormWithDest
  {
  public:
    ExportMapForm(CWnd* wPar, Map& mp, String* sMap)
    : FormWithDest(wPar, SIETitleExportRasMap)
    {
      String sInf(SIERemExportMapTo_S.scVal(), mp->sName());
      StaticText* st = new StaticText(root,sInf);
      st->SetIndependentPos();
      new FieldMapCreate(root, SIEUiMapName, sMap);
//      SetMenHelpTopic("ilwismen\\export.htm"Map);      
      create();
    }
  };  
  String sMap = s;
  bool fOk = sMap != "";
  if (!fOk) { 
    ExportMapForm frm(wPar, mp, &sMap);
    fOk = frm.fOkClicked();
  }
  if (fOk) {
    FileName fn(sMap);
    HourglassCursor cur(wPar);
    mp.Export(fn);
  }
}

void ExportObject(CWnd* wPar, SegmentMap& mp, const String& s)
{
  class ExportSegmentMapForm: public FormWithDest
  {
  public:
    ExportSegmentMapForm(CWnd* wPar, SegmentMap& mp, String* sMap)
    : FormWithDest(wPar, SIETitleExportSegMap)
    {
      String sInf(SIERemExportSegMapTo_S.scVal(), mp->sName());
      StaticText* st = new StaticText(root,sInf);
      st->SetIndependentPos();
      new FieldSegmentMapCreate(root, SIEUiMapName, sMap);
//      SetMenHelpTopic("ilwismen\\export.htm"SegMap);      
      create();
    }
  };  
  String sMap = s;
  bool fOk = sMap != "";
  if (!fOk) { 
    ExportSegmentMapForm frm(wPar, mp, &sMap);
    fOk = frm.fOkClicked();
  }
  if (fOk) {
    FileName fn(sMap);
    HourglassCursor cur(wPar);
    mp.Export(fn);
  }
}

void ExportObject(CWnd* wPar, PolygonMap& mp, const String& s)
{
  class ExportPolygonMapForm: public FormWithDest
  {
  public:
    ExportPolygonMapForm(CWnd* wPar, PolygonMap& mp, String* sMap)
    : FormWithDest(wPar, SIETitleExportPolMap)
    {
      String sInf(SIERemExportPolMapTo_S.scVal(), mp->sName());
      StaticText* st = new StaticText(root,sInf);
      st->SetIndependentPos();
      new FieldPolygonMapCreate(root, SIEUiMapName, sMap);
//      SetMenHelpTopic("ilwismen\\export.htm"PolMap);      
      create();
    }
  };  
  String sMap = s;
  bool fOk = sMap != "";
  if (!fOk) { 
    ExportPolygonMapForm frm(wPar, mp, &sMap);
    fOk = frm.fOkClicked();
  }
  if (fOk) {
    FileName fn(sMap);
    HourglassCursor cur(wPar);
    mp.Export(fn);
  }
}

void ExportObject(CWnd* wPar, PointMap& mp, const String& s)
{
  class ExportPointMapForm: public FormWithDest
  {
  public:
    ExportPointMapForm(CWnd* wPar, PointMap& mp, String* sMap)
    : FormWithDest(wPar, SIETitleExportPntMap)
    {
      String sInf(SIERemExportPntMapTo_S.scVal(), mp->sName());
      StaticText* st = new StaticText(root,sInf);
      st->SetIndependentPos();
      new FieldPointMapCreate(root, SIEUiMapName, sMap);
//      SetMenHelpTopic("ilwismen\\export.htm"PntMap);      
      create();
    }
  };  
  String sMap = s;
  bool fOk = sMap != "";
  if (!fOk) { 
    ExportPointMapForm frm(wPar, mp, &sMap);
    fOk = frm.fOkClicked();
  }
  if (fOk) {
    FileName fn(sMap);
    HourglassCursor cur(wPar);
    mp.Export(fn);
  }
}

void ExportObject(CWnd* wPar, Table& tbl, const String& s)
{
  class ExportTableForm: public FormWithDest
  {
  public:
    ExportTableForm(CWnd* wPar, Table& tbl, String* stbl)
    : FormWithDest(wPar, SIETitleExportTable)
    {
      String sInf(SIERemExportTableTo_S.scVal(), tbl->sName());
      StaticText* st = new StaticText(root,sInf);
      st->SetIndependentPos();
      new FieldTableCreate(root, SIEUiTblName, stbl);
//      SetMenHelpTopic("ilwismen\\export.htm"Table);
      create();
    }
  };  
  String sTbl = s;
  bool fOk = sTbl != "";
  if (!fOk) { 
    ExportTableForm frm(wPar, tbl, &sTbl);
    fOk = frm.fOkClicked();
  }
  if (fOk) {
    FileName fn(sTbl);
    fn.sExt = ".tbl";
    HourglassCursor cur(wPar);
    tbl.Export(fn);
  }
}






