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
// $Log: /ILWIS 3.0/ApplicationForms/frmpntap.cpp $
 * 
 * 42    5-10-04 22:09 Hendrikse
 * implemented  TransformHeights UserInterf Form  to allow 3D
 * transformations when the coordsystems (both input and output) have a
 * userdefined Datum, (BursaWolf or Molodensky) or have the WGS84 Datum
 * 
 * 41    5-07-02 19:20 Koolhoven
 * prevent warnings with Visual Studio .Net
 * 
 * 40    6/21/01 10:58a Martin
 * added support for giving the rowcol/coord infor to a submap form
 * together with the filename
 * 
 * 39    26-03-01 13:58 Koolhoven
 * FormPointMapFromTable::TblCallBack() now shows needed fields when no
 * Coord column is found
 * 
 * 38    16/03/01 12:19 Willem
 * The column selection now also allows UniqueID columns
 * 
 * 37    16/02/01 16:14 Willem
 * - Map selection is now limited to Class/Id/Group/Bool/UniqueID
 * - Table and Map callbacks do not try to open a file with an empty
 * filename anymore
 * 
 * 36    12-02-01 17:23 Koolhoven
 * in FormPointMapGlue::MapsCallBack() set fNewDom on false when cbDom is
 * hidden
 * 
 * 35    12-02-01 16:48 Koolhoven
 * inFormPointMapGlue::MapsCallBack() added EnableOK() to prevent buttons
 * remaining grey
 * 
 * 34    12-02-01 15:32 Koolhoven
 * FormPointMapGlue::MapsCallBack() now disables OK button when impossible
 * combination
 * 
 * 33    12-02-01 15:00 Koolhoven
 * FormPointMapGlue: resize asMaps before refrencing items
 * 
 * 32    1-02-01 17:36 Hendrikse
 * added extra  ->StoreData();
 * and layout improvement
 * 
 * 31    26-01-01 18:24 Hendrikse
 * added use of PointMapGlue::CheckAndFindOutputDomain
 * implemented new callbacks (see h file)
 * 
 * 30    20-12-00 18:23 Koolhoven
 * FormPointMapFromTable: in case of no table specified yet do hide both
 * rg's
 * 
 * 29    20-12-00 14:00 Koolhoven
 * in FormPointMapFromTable::exec removed superfluous ',' in expression
 * 
 * 28    19-12-00 16:55 Koolhoven
 * FormPointMapFromTable: use SAFUiColumnXY instead of SAFUiColumn
 * 
 * 27    4-12-00 18:44 Hendrikse
 * disabled useless new RadioButton(rg, SAFUiLabelPnts);
 * in FormPointMapPolygons::FormPointMapPolygons
 * 
 * 26    27-11-00 12:17 Koolhoven
 * FormPointMapFromRas now asks for rastermaps instead of pointmaps as
 * input
 * 
 * 25    27-11-00 11:34 Koolhoven
 * allow in FormPointMapFromTable also Coord Columns which are not the
 * first column
 * 
 * 24    27-11-00 11:23 Koolhoven
 * readability measures: removed "== true" and replaced "== false" by "!"
 * 
 * 23    27-11-00 10:37 Koolhoven
 * FormPointMapFromTable improved
 * 
 * 22    23-11-00 10:54 Koolhoven
 * FormPointMapSegments::SegmentMapCallBack(), in case of no valid map yet
 * also hide the degree options
 * 
 * 21    14-11-00 9:58a Martin
 * added support for attrib table columns in the mapselectors
 * 
 * 20    8-11-00 17:24 Koolhoven
 * use sFullNameQuoted() on input data to ensure selection in forms
 * 
 * 19    5-10-00 17:40 Koolhoven
 * with Transform operation ask Coordinate System below output map
 * 
 * 18    1-09-00 2:06p Martin
 * prevent use attrib columns in segpnt
 * 
 * 17    1-09-00 12:13p Martin
 * support for coordinate columns in tbltopointmap
 * 
 * 16    5/23/00 1:53p Wind
 * MapListerDomainType instances were used/shared by several formentries,
 * and deleted by them
 * 
 * 15    5/23/00 12:50p Wind
 * - accept  raster input maps with any domain in FormPointMapSegment
 * - accept  raster input maps with any domain in FormPointMapPolygon
 * - accept  raster input maps with any domain in FormPointMap
 * 
 * 14    11-05-00 3:57p Martin
 * added 'attribute columns' nameedits
 * 
 * 13    9-05-00 10:52a Martin
 * use of attrib columns in nameedits
 * 
 * 12    4/06/00 2:26p Hendrikse
 * adapted  FormPointMapGlue::exec() and  FormPointMapSubMap::exec()
 * to have an sExpressiin with sufficient precision in case of Latlon csy
 * 
 * 11    3/30/00 8:30p Hendrikse
 * debugged some initializations (e.g. SetVal added)
 * and implemented void HideCoordsAndLatLon();
 * 
 * 10    3/28/00 3:02p Hendrikse
 * debugged the double declaration of CheckBox*
 * Simplified the Hide and show calls of fldCrd and fldlatlons
 * 
 * 9     3/27/00 5:52p Hendrikse
 * implemented ed in FormPointMapGlue:
 *  int CallBackMinMaxXY(Event*);
 *   int CallBackMinMaxLatLon(Event*); 
 *   int CallBackClipBoundary(Event*); 
 * to handle LatLon maps and modified clip bounds properly
 * 
 * 8     3/25/00 12:25p Hendrikse
 * added stCsy = new StaticText(cb, sFill50); and implemented filling with
 * the name of the csy in case of clipping option
 * 
 * 7     3/24/00 4:27p Hendrikse
 * implemented
 * CallBackMinMaxXY() and  CallBackMinMaxLatLon() 
 * and extende the Form constructor for its use such that for PointMaps
 * witj Latlons one can make submaps defined with latlon bounds (dms
 * fields)
 * 
 * 6     3/22/00 5:46p Hendrikse
 * improved SegmentMapCallBack(Event*) by treating iOptions correctly
 * 
 * 5     14-03-00 18:32 Hendrikse
 * added functionality to treat segmaps with latlon coords and distance in
 * degrees
 * 
 * 4     28-02-00 11:56 Wind
 * adapted to changes in constructor of FieldColumn
 * 
 * 3     9/13/99 1:04p Wind
 * comments
 * 
 * 2     9/13/99 12:35p Wind
 * adapted :exec functions to use of quoted file names when building an
 * expression
*/
// Revision 1.8  1998/10/08 14:45:01  Wim
// Transform has now callback to tell source coordsys type name
//
// Revision 1.7  1998-09-16 18:33:54+01  Wim
// 22beta2
//
// Revision 1.6  1998/02/24 08:53:45  martin
// Attribute maps are now handled correctly (path).
//
// Revision 1.5  1997/09/15 08:47:30  Wim
// Clear Remark and enable OK button in GlueMap when
// the new domain field is hidden
//
// Revision 1.4  1997-09-11 11:40:29+02  Wim
// Strip path from specified new domain name
//
// Revision 1.3  1997-08-27 15:33:44+02  Wim
// GluePnt default is now ned domain
//
// Revision 1.2  1997-07-30 12:50:21+02  Wim
// Ask for sNewDom in PointMapGlue
//
/* Form PointMap Applications
   by Wim Koolhoven
   (c) Ilwis System Development ITC
	Last change:  WK    8 Oct 98    1:17 pm
*/
//#include "Client\MainWindow\mainwind.h"
#include "Client\Headers\AppFormsPCH.h"
#include "Client\FormElements\fldlist.h"
#include "Engine\Applications\PNTVIRT.H"
#include "PointApplications\PNTGLUE.H"
#include "Client\FormElements\fldcs.h"
#include "PointApplicationsUI\frmpntap.h"
#include "Engine\Base\Round.h"
#include "Headers\Hs\DOMAIN.hs"
#include "Engine\Table\COLINFO.H"
#include "Headers\Hs\Coordsys.hs"
#include "Headers\Hs\Appforms.hs"

LRESULT Cmdattribpnt(CWnd *wnd, const String& s)
{
	new FormAttributePointMap(wnd, s.scVal());
	return -1;
}

FormAttributePointMap::FormAttributePointMap(CWnd* mw, const char* sPar)
: FormPointMapCreate(mw, SAFTitleAttribPntMap)
{
  if (sPar) {
    TextInput inp(sPar);
    TokenizerBase tokenizer(&inp);
    String sVal;
    for (;;) {
      Token tok = tokenizer.tokGet();
      sVal = tok.sVal();
      if (sVal == "")
        break;
      FileName fn(sVal);
      if (fn.sExt == "" || fn.sExt == ".mpp")
        if (sPointMap == "")
          sPointMap = fn.sFullNameQuoted(false);
        else  
          sOutMap = fn.sFullName(false);
    }
  }
  fldPointMap = new FieldPointMap(root, SAFUiPntMap, &sPointMap,
	                              new MapListerDomainType(".mpp", dmCLASS | dmIDENT | dmGROUP | dmBOOL | dmUNIQUEID));
  fldPointMap->SetCallBack((NotifyProc)&FormAttributePointMap::PointMapCallBack);
  fldTbl = new FieldTable(root, SAFUiTable, &sTbl);
  fldTbl->SetCallBack((NotifyProc)&FormAttributePointMap::TblCallBack);
  fldCol = new FieldColumn(root, SAFUiAttribute, Table(), &sCol,
    dmCLASS|dmIDENT|dmUNIQUEID|dmVALUE|dmBOOL);
  fldCol->SetCallBack((NotifyProc)&FormAttributePointMap::ColCallBack);
  stColRemark = new StaticText(root, String('x',50));
  stColRemark->SetIndependentPos();
  initPointMapOut(false);
  SetAppHelpTopic(htpPointMapAttribute);
  create();
}                    

int FormAttributePointMap::exec() 
{
  FormPointMapCreate::exec();
  String sExpr;
  bool fShort = false;
  FileName fn(sOutMap);
  FileName fnTbl(sTbl);
  FileName fnPointMap(sPointMap); 
  PointMap map(fnPointMap);
  if (map.fValid()) {
    if (map->fTblAtt()) {
      Table tbl = map->tblAtt();
      fShort = (tbl->fnObj == fnTbl);
    }  
  }    
  sPointMap = fnPointMap.sRelativeQuoted(false,fn.sPath());
  if (!fShort)
    sCol = String("%S.%S", sTbl, sCol);
  sExpr = String("PointMapAttribute(%S,%S)", 
                  sPointMap,sCol);
  execPointMapOut(sExpr);  
  return 0;
}

int FormAttributePointMap::PointMapCallBack(Event*)
{
	fldPointMap->StoreData();
	fldCol->FillWithColumns((TablePtr*)0);
	try
	{
		if (sPointMap.length() == 0)
			return 0;
		
		FileName fnMap(sPointMap); 
		PointMap map(fnMap);
		if (map.fValid())
		{
			fldTbl->SetDomain(map->dm()->sName());
			if (map->fTblAtt())
				fldTbl->SetVal(map->tblAtt()->fnObj.sFullPath());
		}  
	}
	catch (ErrorObject&) {}
	
	return 0;
}

int FormAttributePointMap::TblCallBack(Event*)
{
	fldTbl->StoreData();
	if (sTbl.length() == 0)
		return 0;

	fldCol->FillWithColumns((TablePtr*)0);
	try
	{
		FileName fnTbl(sTbl); 
		fldCol->FillWithColumns(fnTbl);
		ColCallBack(0);
	}
	catch (ErrorObject&) {}
	return 0;
}

int FormAttributePointMap::ColCallBack(Event*)
{
  fldCol->StoreData();
  try {
    String sRemark;
    FileName fnTbl(sTbl); 
    ColumnInfo colinf(fnTbl, sCol);
    if (!colinf.fValid()) {
      stColRemark->SetVal("");
      return 0;
    }
    Domain dm = colinf.dm();
    sRemark = dm->sTypeName();
    stColRemark->SetVal(sRemark);
  }
  catch (ErrorObject&) {
    stColRemark->SetVal("");
  }
  return 0;
}

//--[ FormPointMapFromRas ]-----------------------------------------------------------------------
LRESULT Cmdraspnt(CWnd *wnd, const String& s)
{
	new FormPointMapFromRas(wnd, s.scVal());
	return -1;
}

FormPointMapFromRas::FormPointMapFromRas(CWnd* mw, const char* sPar)
: FormPointMapCreate(mw, SAFTitlePntMapFromRasMap)
{
  if (sPar) {
    TextInput inp(sPar);
    TokenizerBase tokenizer(&inp);
    String sVal;
    for (;;) {
      Token tok = tokenizer.tokGet();
      sVal = tok.sVal();
      if (sVal == "")
        break;
      FileName fn(sVal);
      if (sMap == "" && fn.sExt == "" || fn.sExt == ".mpr")
        sMap = fn.sFullNameQuoted(false);
      else if (sOutMap == "" && fn.sExt == "" || fn.sExt == ".mpp")
        sOutMap = fn.sFullName(false);
    }
  }
  new FieldMap(root, SAFUiRasMap, &sMap, new MapListerDomainType(".mpr", 0, true));
  initPointMapOut(false);
  SetAppHelpTopic(htpPointMapFromRas);
  create();
}                    

int FormPointMapFromRas::exec() 
{
  FormPointMapCreate::exec();
  String sExpr;
  FileName fn(sOutMap);
  FileName fnMap(sMap); 
  sMap = fnMap.sRelativeQuoted(false,fn.sPath());
  sExpr = String("PointMapFromRas(%S)", 
                  sMap);
  execPointMapOut(sExpr);  
  return 0;
}

//--[ FormPointMapFromTable ]-----------------------------------------------------------------------
LRESULT Cmdtblpnt(CWnd *wnd, const String& s)
{
	new FormPointMapFromTable(wnd, s.scVal());
	return -1;
}

FormPointMapFromTable::FormPointMapFromTable(CWnd* mw, const char* sPar)
: FormPointMapCreate(mw, SAFTitleTblToPnts),
  rg(0), rgDmNone(0), fldColCoord(0), fldColX(0), fldColY(0), fUseCoords(true)
{
  sColX = "X";
  sColY = "Y";
  iOption = 0;
  sPrefix = "pnt";
  if (sPar) {
    TextInput inp(sPar);
    TokenizerBase tokenizer(&inp);
    String sVal;
    for (;;) {
      Token tok = tokenizer.tokGet();
      sVal = tok.sVal();
      if (sVal == "")
        break;
      FileName fn(sVal);
      if (sTbl == "" && fn.sExt == "" || fn.sExt == ".tbt")
        sTbl = fn.sFullNameQuoted(false);
      if (sCsy == "" && fn.sExt == "" || fn.sExt == ".csy")
        sCsy = fn.sFullNameQuoted(false);
      else if (sOutMap == "" && fn.sExt == "" || fn.sExt == ".mpp")
        sOutMap = fn.sFullName(false);
    }
  }
  fldTbl = new FieldTable(root, SAFUiTable, &sTbl);
  fldTbl->SetCallBack((NotifyProc)&FormPointMapFromTable::TblCallBack);
  cc = new CheckBox(root, SAFUiUseCoordColumn, &fUseCoords);
	cc->SetCallBack((NotifyProc)&FormPointMapFromTable::ColCoordCallBack);		
	cc->SetIndependentPos();
	fldColCoord = new FieldColumn(cc, SAFUiColumnXY, Table(), &sColCoord, dmCOORD);
	fldColCoord->Align(cc, AL_UNDER);
	
  fldColX = new FieldColumn(root, SAFUiColumnX, Table(), &sColX, dmVALUE);
	fldColX->Align(cc, AL_UNDER);
	fldColY = new FieldColumn(root, SAFUiColumnY, Table(), &sColY, dmVALUE);
	feCs = new FieldCoordSystemC(root, SAFUiCoordSys, &sCsy);

  rg = new RadioGroup(root, SAFUiDomOutMap, &iOption);
  rg->SetCallBack((NotifyProc)&FormPointMapFromTable::OptionCallBack);
  rg->SetIndependentPos();
  new RadioButton(rg, SAFUiUseTableDomain);
  new RadioButton(rg, SAFUiUseAttColumn);

  rgDmNone = new RadioGroup(root, SAFUiDomOutMap, &iOption);
  rgDmNone->SetCallBack((NotifyProc)&FormPointMapFromTable::OptionCallBack);
  rgDmNone->Align(feCs, AL_UNDER);
  rgDmNone->SetIndependentPos();
  RadioButton* rbRec = new RadioButton(rgDmNone, SAFUiUseRecNrsAsIds);
  new RadioButton(rgDmNone, SAFUiUseColOfTable);

  FieldString* fsPrefix = new FieldString(rbRec, SAFUiDomPrefix, &sPrefix, Domain(), false);
  fsPrefix->Align(rg, AL_UNDER);

  fldColAttrib = new FieldColumn(root, SAFUiColumn, Table(), &sColAttrib,
    dmCLASS|dmIDENT|dmVALUE|dmBOOL);
  fldColAttrib->Align(rg, AL_UNDER);

  initPointMapOut(false);
  SetAppHelpTopic(htpPointMapFromTable);
  create();
}                    

int FormPointMapFromTable::ColCoordCallBack(Event*)
{
	if (cc->fVal())
	{
		fldColX->Hide();
		fldColY->Hide();
		feCs->Hide();
	}	
	else
	{
		fldColX->Show();
		fldColY->Show();
		feCs->Show();		
	}	
	return 1;
}

int FormPointMapFromTable::TblCallBack(Event*)
{
  fldTbl->StoreData();
	if (sTbl == "") {
    rg->Hide();
    rgDmNone->Hide();
		return 0;
	}

	Table tbl(sTbl);
	
	fUseCoords = false;
	for(int i = 0; i < tbl->iCols(); ++i)
	{
		Column col = tbl->col(i);
		if (col->dm()->pdcrd() )
		{
			sColCoord = col->sName();
			fUseCoords = true;
			break;
		}
	}		
	if (fUseCoords)
		cc->Show();
	else {
		cc->Hide();
		fldColX->Show();
		fldColY->Show();
		feCs->Show();		
	}
	
  if (0 == sTbl[0])
    return 0;
  try {
    FileName fnTbl(sTbl);
	  fldColX->FillWithColumns(fnTbl);
	  fldColY->FillWithColumns(fnTbl);
		fldColAttrib->FillWithColumns(fnTbl);
		fldColCoord->FillWithColumns(fnTbl);
		
    Domain dm;
    ObjectInfo::ReadElement("Table", "Domain", fnTbl, dm);
    if (0 != dm->pdid()) {
      rg->Show();
      rgDmNone->Hide();
    }
    else if (0 != dm->pdnone()) {
      rg->Hide();
      rgDmNone->Show();
    }
    else {
      rg->Hide();
      rgDmNone->Hide();
    }
  }
  catch (ErrorObject&) 
	{
		// do nothing
	}
  return OptionCallBack(0);
}

int FormPointMapFromTable::OptionCallBack(Event*)
{
  if (0 == sTbl[0])
    return 0;
  try {
    FileName fnTbl(sTbl);
    Domain dm;
    ObjectInfo::ReadElement("Table", "Domain", fnTbl, dm);
    if (0 != dm->pdid())
      rg->StoreData();
    else if (0 != dm->pdnone())
      rgDmNone->StoreData();
    else
      iOption = 1;

    bool fShowCol = 1 == iOption;
    if (fShowCol)
      fldColAttrib->Show();
    else
      fldColAttrib->Hide();
  }
  catch (ErrorObject&) {}
  return 0;
}

int FormPointMapFromTable::exec() 
{
  FormPointMapCreate::exec();
  String sExpr;
  FileName fn(sOutMap);
  FileName fnTbl(sTbl);
  sTbl = fnTbl.sRelativeQuoted(false,fn.sPath());
  FileName fnCsy(sCsy);
  sCsy = fnCsy.sRelativeQuoted(false,fn.sPath());
  Domain dm;
  ObjectInfo::ReadElement("Table", "Domain", fnTbl, dm);
  String sLast;
  switch (iOption) {
    case 0:
      if (0 != dm->pdnone())
        sLast = String(",%S", sPrefix);
      break;
    case 1:
      sLast = String(",%S", sColAttrib);
  }
	if (!fUseCoords)
	{
	  if ("X" == sColX && "Y" == sColY)
		  sExpr = String("PointMapFromTable(%S,%S%S)",
			  sTbl, sCsy, sLast);
		else
			sExpr = String("PointMapFromTable(%S,%S,%S,%S%S)",
				sTbl, sColX, sColY, sCsy, sLast);
	}
	else
	{
		sExpr = String("PointMapFromTable(%S,%S%S)", sTbl, sColCoord, sLast);
	}		
  execPointMapOut(sExpr);
  return 0;
}

LRESULT Cmdsegpnt(CWnd *wnd, const String& s)
{
	new FormPointMapSegments(wnd, s.scVal());
	return -1;
}

FormPointMapSegments::FormPointMapSegments(CWnd* mw, const char* sPar)
: FormPointMapCreate(mw, SAFTitleSegToPnts)
{
  if (sPar) {
    TextInput inp(sPar);
    TokenizerBase tokenizer(&inp);
    String sVal;
    for (;;) {
      Token tok = tokenizer.tokGet();
      sVal = tok.sVal();
      if (sVal == "")
        break;
      FileName fn(sVal);
      if (sMap == "" && fn.sExt == "" || fn.sExt == ".mps")
        sMap = fn.sFullNameQuoted(false);
      else if (sOutMap == "" && fn.sExt == "" || fn.sExt == ".mpp")
        sOutMap = fn.sFullName(false);
    }
  }
	fldSegmentMap = new FieldSegmentMap(root, SAFUiSegMap, &sMap, new MapListerDomainType(".mps", 0, false));
	fldSegmentMap->SetCallBack((NotifyProc)&FormPointMapSegments::SegmentMapCallBack);
	String sFill('x', 50);
  stCsy = new StaticText(root, sFill);
  stCsy->SetIndependentPos();
  iOption = 0;
  rgOptions = new RadioGroup(root, "" , &iOption);
  RadioButton *rbNodes = new RadioButton(rgOptions, SAFUiOnlyNodes);
  rbMeters = new RadioButton(rgOptions, SAFUiDistanceInMeters);
	rbMeters->Align(rbNodes, AL_UNDER);
	rbDegr = new RadioButton(rgOptions, SAFUiDistance);
	rbDegr->Align(rbNodes, AL_UNDER);
	fldMeters = new FieldReal(rbMeters, "", &rDist, ValueRange(0.001,1e12,0));
	fldMeters->Align(rbMeters, AL_AFTER);
	fldDegMinSec = new FieldDMS(rbDegr, "", &rDist, 30, true);
	fldDegMinSec->Align(rbDegr, AL_AFTER);
  new RadioButton(rgOptions, SAFUiAllPnts);
  initPointMapOut(false);
  SetAppHelpTopic(htpPointMapSegments);
  create();
}                    

double FormPointMapSegments::rDefaultDensifyDistance(const SegmentMap& segmap)
{
  double w = segmap->cb().width();
  double h = segmap->cb().height();
  double m = min(w,h);
  long iNrSeg = segmap->iFeatures();
  if (iNrSeg == 0 || !segmap->cs().fValid())
    return rUNDEF;
  else
	if (segmap->cs()->pcsLatLon()) 
		return rRoundDMS(m / 1000); // default distance in DMS
	else								// between denser coords
		return rRound (m / 1000); //default distance in meters
} 
int FormPointMapSegments::SegmentMapCallBack(Event*)
{
  fldSegmentMap->StoreData();
  rgOptions->StoreData();
  try {
    FileName fnMap(sMap); 
    SegmentMap map(fnMap);
    if (map.fValid()) {
			stCsy->SetVal(map->cs()->sTypeName());
			rDist = rDefaultDensifyDistance(map);
			fFromLatLon = ( 0 != map->cs()->pcsLatLon());
			if (fFromLatLon)
			{
				fldDegMinSec->SetVal(rDist);
				if (iOption == 1)  // previously a map was loaded with distance (meters) option
				{
					iOption = 2;   // so now reselect distance for degrees
					rgOptions->SetVal(iOption);
				}
				rbDegr->Show();
				if (iOption == 2)
					fldDegMinSec->Show();
				else
					fldDegMinSec->Hide();

				rbMeters->Hide();
				fldMeters->Hide();
			}
			else {
				fldMeters->SetVal(rDist);
				if (iOption == 2)  // previously a map was loaded with distance (degrees) option
				{
					iOption = 1;   // so now select distance for meters
					rgOptions->SetVal(iOption);
				}
				rbMeters->Show();
				if (iOption == 1)
					fldMeters->Show();
				else
					fldMeters->Hide();

				rbDegr->Hide(); 
				fldDegMinSec->Hide();
			}
		}
		else {
      stCsy->SetVal("");
			rbDegr->Hide(); 
			fldDegMinSec->Hide();
		}
  }
  catch (ErrorObject&) {
    stCsy->SetVal("");
		rbDegr->Hide(); 
		fldDegMinSec->Hide();
	}
  return 0;
}

int FormPointMapSegments::exec() 
{
  FormPointMapCreate::exec();
  String sExpr;
//	String sCol;
//	FieldDataType ::GetAttribParts(sMap, sMap, sCol);
//	sMap = sCol != "" ? String("%S.%S", sMap, sCol) : sMap;
  FileName fn(sOutMap);
  FileName fnMap(sMap); 
  sMap = fnMap.sRelativeQuoted(false,fn.sPath());
  switch (iOption) {
    case 0:  
      sExpr = String("PointMapSegNodes(%S)", sMap);
      break;
    case 1: // distance in meters
		case 2: // distance in degrees (latlon.csy)
      sExpr = String("PointMapSegDist(%S,%f)", sMap, rDist);
      break;
    case 3:  
      sExpr = String("PointMapSegCoords(%S)", sMap);
      break;
  }    
  execPointMapOut(sExpr);  
  return 0;
}

LRESULT Cmdpolpnt(CWnd *wnd, const String& s)
{
	new FormPointMapPolygons(wnd, s.scVal());
	return -1;
}

FormPointMapPolygons::FormPointMapPolygons(CWnd* mw, const char* sPar)
: FormPointMapCreate(mw, SAFTitlePolToPnts)
{
  fIncludeUndefs = false;
  if (sPar) {
    TextInput inp(sPar);
    TokenizerBase tokenizer(&inp);
    String sVal;
    for (;;) {
      Token tok = tokenizer.tokGet();
      sVal = tok.sVal();
      if (sVal == "")
        break;
      FileName fn(sVal);
      if (sMap == "" && fn.sExt == "" || fn.sExt == ".mpa")
        sMap = fn.sFullNameQuoted(false);
      else if (sOutMap == "" && fn.sExt == "" || fn.sExt == ".mpp")
        sOutMap = fn.sFullName(false);
    }
  }
  new FieldPolygonMap(root, SAFUiPolMap, &sMap, new MapListerDomainType(".mpa", 0/*dmVALUE|dmCLASS|dmIDENT|dmBOOL*/, false));
  iOption = 0;
  //RadioGroup* rg = new RadioGroup(root, "" , &iOption);
  //new RadioButton(rg, SAFUiLabelPnts);
  CheckBox* cb = new CheckBox(root, SAFUiIncludeUndefs, &fIncludeUndefs);
  cb->SetIndependentPos();
  initPointMapOut(false);
  SetAppHelpTopic(htpPointMapPolygons);
  create();
}                    

int FormPointMapPolygons::exec() 
{
  FormPointMapCreate::exec();
  String sExpr;
	//String sCol;
	//FieldDataType ::GetAttribParts(sMap, sMap, sCol);
	//sMap = sCol != "" ? String("%S.%S", sMap, sCol) : sMap;
  FileName fn(sOutMap);
  FileName fnMap(sMap); 
  sMap = fnMap.sRelativeQuoted(false,fn.sPath());
  switch (iOption) {
    case 0:  
      sExpr = String("PointMapPolLabels(%S%s)", sMap,
                     fIncludeUndefs ? ",alsoundefs" : "");
      break;
  }    
  execPointMapOut(sExpr);  
  return 0;
}

LRESULT Cmdtransfpnt(CWnd *wnd, const String& s)
{
	new FormPointMapTransform(wnd, s.scVal());
	return -1;
}

FormPointMapTransform::FormPointMapTransform(CWnd* mw, const char* sPar)
: FormPointMapCreate(mw, SAFTitlePntMapTransform)
{
  if (sPar) {
    TextInput inp(sPar);
    TokenizerBase tokenizer(&inp);
    String sVal;
    for (;;) {
      Token tok = tokenizer.tokGet();
      sVal = tok.sVal();
      if (sVal == "")
        break;
      FileName fn(sVal);
      if (fn.sExt == ".csy")
        sCoordSys = fn.sFullNameQuoted(false);
      else if (fn.sExt == "" || fn.sExt == ".mpp")
        if (sPointMap == "")
          sPointMap = fn.sFullNameQuoted(false);
        else  
          sOutMap = fn.sFullName(false);
    }
  }
  fldMap = new FieldPointMap(root, SAFUiPntMap, &sPointMap, new MapListerDomainType(".mpp", 0, true));
  fldMap->SetCallBack((NotifyProc)&FormPointMapTransform::MapCallBack);
  String sFill('x', 50);
  stCsy = new StaticText(root, sFill);
  stCsy->SetIndependentPos();

// do not call initPointMapOut(false) because also csy has to be asked
  fmc = new FieldPointMapCreate(root, SAFUiOutPntMap, &sOutMap);
  fmc->SetCallBack((NotifyProc)&FormPointMapCreate::OutPointMapCallBack);
  new FieldCoordSystemC(root, SAFUiCoordSys, &sCoordSys);
  StaticText* st = new StaticText(root, SAFUiDescription);
  st->psn->SetBound(0,0,0,0);
  FieldString* fs = new FieldString(root, "", &sDescr);
  fs->SetWidth(120);
  fs->SetIndependentPos();
	fTransformHeights = false;
	CheckBox* cb = new CheckBox(root, SAFUiUseHeights, &fTransformHeights);
  cb->SetIndependentPos();
  SetAppHelpTopic(htpPointMapTransform);
  create();
}                    

int FormPointMapTransform::MapCallBack(Event*)
{
  fldMap->StoreData();
  try {
    FileName fnMap(sPointMap); 
    PointMap map(fnMap);
    if (map.fValid()) {
      stCsy->SetVal(map->cs()->sTypeName());
    }
    else
      stCsy->SetVal("");
  }
  catch (ErrorObject&) {
    stCsy->SetVal("");
  }

  return 0;
}

int FormPointMapTransform::exec() 
{
  FormPointMapCreate::exec();
  String sExpr;
  FileName fn(sOutMap);
  FileName fnPointMap(sPointMap); 
  FileName fnCoordSys(sCoordSys); 
  sPointMap = fnPointMap.sRelativeQuoted(false,fn.sPath());
  sCoordSys = fnCoordSys.sRelativeQuoted(false,fn.sPath());
	String sTransfH = fTransformHeights ? String(",TH"): String("");
  sExpr = String("PointMapTransform(%S,%S%S)", 
                  sPointMap,sCoordSys,sTransfH);
  execPointMapOut(sExpr);  
  return 0;
}

LRESULT Cmdsubpnt(CWnd *wnd, const String& s)
{
	new FormPointMapSubMap(wnd, s.scVal());
	return -1;
}

FormPointMapSubMap::FormPointMapSubMap(CWnd* mw, const char* sPar)
: FormPointMapCreate(mw, SAFTitleSubMapPntMap),
  fPreset(0)
{
  cMin = cMax = Coord(0,0);
  if (sPar) 
	{
    TextInput inp(sPar);
    TokenizerBase tokenizer(&inp);
    String sVal;
    for (;;) 
		{
      Token tok = tokenizer.tokGet();
      sVal = tok.sVal();
      if (sVal == "")
        break;
      FileName fn(sVal);
			if ( tok.iPos() == 0 )
			{
				if (fn.sExt == "" || fn.sExt == ".mpp")
	        if (sPointMap == "")
	          sPointMap = fn.sFullNameQuoted(false);
	        else  
	          sOutMap = fn.sFullName(false);
			}				
			if ( tok.iPos() > 0 )
			{
			//	iMethod = 2;
				cMin.x = sVal.iVal();
				tok = tokenizer.tokGet();
				sVal = tok.sVal();
				cMin.y = sVal.iVal();
				tok = tokenizer.tokGet();
				sVal = tok.sVal();
				cMax.x = sVal.iVal();				
				tok = tokenizer.tokGet();
				sVal = tok.sVal();
				cMax.y = sVal.iVal();	
				fPreset = true;
			}								
    }
  }
  fldPointMap = new FieldPointMap(root, SAFUiPntMap, &sPointMap, new MapListerDomainType(".mpp", 0, true));
  fldPointMap->SetCallBack((NotifyProc)&FormPointMapSubMap::PointMapCallBack);
  String sFill('x', 50);
  stCsy = new StaticText(root, sFill);
  stCsy->SetIndependentPos();
  FieldGroup *fg = new FieldGroup(root);
  fg->SetIndependentPos();
  fcMin = new FieldCoord(fg, SAFUiMinXY, &cMin);
  fcMin->SetCallBack((NotifyProc)&FormPointMapSubMap::CallBackMinMaxXY);
  fcMax = new FieldCoord(fg, SAFUiMaxXY, &cMax);
  fcMax->SetCallBack((NotifyProc)&FormPointMapSubMap::CallBackMinMaxXY);
  fldMinLat = new FieldLat(fg, SCSUiMinLat, &llMin.Lat);
  fldMinLat->Align(stCsy, AL_UNDER);
  fldMinLat->SetCallBack((NotifyProc)&FormPointMapSubMap::CallBackMinMaxLatLon);
  fldMinLon = new FieldLon(fg, SCSUiMinLon, &llMin.Lon);
  fldMinLon->SetCallBack((NotifyProc)&FormPointMapSubMap::CallBackMinMaxLatLon);
  fldMaxLat = new FieldLat(fg, SCSUiMaxLat, &llMax.Lat);
  fldMaxLat->Align(fldMinLon, AL_UNDER);
  fldMaxLat->SetCallBack((NotifyProc)&FormPointMapSubMap::CallBackMinMaxLatLon);
  fldMaxLon = new FieldLon(fg, SCSUiMaxLon, &llMax.Lon);
  fldMaxLon->SetCallBack((NotifyProc)&FormPointMapSubMap::CallBackMinMaxLatLon);
  initPointMapOut(false);
  SetAppHelpTopic(htpPointMapSubMap);
  create();
}                    

int FormPointMapSubMap::exec() 
{
  FormPointMapCreate::exec();
  String sExpr;
  FileName fn(sOutMap);
  FileName fnPointMap(sPointMap); 
  sPointMap = fnPointMap.sRelativeQuoted(false,fn.sPath());
  if (fFromLatLon)
  {
    cMin.x = llMin.Lon;
	cMax.x = llMax.Lon;
	cMin.y = llMin.Lat;
	cMax.y = llMax.Lat;
	sExpr = String("PointMapSubMap(%S,%7f,%7f,%7f,%7f)", 
                  sPointMap,cMin.x,cMin.y,cMax.x,cMax.y);
  }
  else
    sExpr = String("PointMapSubMap(%S,%f,%f,%f,%f)", 
                  sPointMap,cMin.x,cMin.y,cMax.x,cMax.y);
  execPointMapOut(sExpr);  
  return 0;
}

int FormPointMapSubMap::PointMapCallBack(Event*)
{
  fldPointMap->StoreData();
  
  try {
    FileName fnMap(sPointMap); 
    PointMap map(fnMap);
    if (map.fValid()) {
	  CoordBounds cb = fPreset ? CoordBounds(cMin, cMax) : map->cb();;
	  CoordSystem csInput = map->cs();   
	  if (csInput.fValid()) {
        cMin = cb.cMin;
	    cMax = cb.cMax;
	    stCsy->SetVal(map->cs()->sTypeName());
	    fFromLatLon = ( 0 !=map->cs()->pcsLatLon());
    	if (!fFromLatLon)
		{
    	  fcMin->SetVal(cMin);
		  fcMax->SetVal(cMax);
		  fcMin->Show();
		  fcMax->Show();
		  fldMinLat->Hide();
		  fldMaxLat->Hide();
		  fldMinLon->Hide();
		  fldMaxLon->Hide();
		}
	    else
		{
		  llMin.Lat = cMin.y; 
	      llMax.Lat = cMax.y;
		  llMin.Lon = cMin.x; 
		  llMax.Lon = cMax.x;
		  fldMinLat->SetVal(llMin.Lat);
		  fldMaxLat->SetVal(llMax.Lat);
		  fldMinLon->SetVal(llMin.Lon);
		  fldMaxLon->SetVal(llMax.Lon);
		  fldMinLat->Show();
		  fldMaxLat->Show();
		  fldMinLon->Show();
		  fldMaxLon->Show();
		  fcMin->Hide();
		  fcMax->Hide();
		}
	  }
	  else {
        stCsy->SetVal("");
		HideCoordsAndLatLon();
	  }
    }
	else {
		stCsy->SetVal("");
		HideCoordsAndLatLon();
	}
  }
  catch (ErrorObject&) {
		stCsy->SetVal("");
		HideCoordsAndLatLon();
  }
  return 0;
}

int FormPointMapSubMap::CallBackMinMaxXY(Event*) 
{
  Coord cMin = fcMin->crdVal();
  Coord cMax = fcMax->crdVal();
  if (cMin.x < cMax.x && cMin.y < cMax.y)
    EnableOK();
  else
    DisableOK();  
  return 0;  
}

int FormPointMapSubMap::CallBackMinMaxLatLon(Event*) 
{
  LatLon laloMin, laloMax;
  laloMin.Lat = fldMinLat->rVal();
  laloMin.Lon = fldMinLon->rVal();
  laloMax.Lat = fldMaxLat->rVal();
  laloMax.Lon = fldMaxLon->rVal();
  if (laloMin.Lat < laloMax.Lat && laloMin.Lon < laloMax.Lon)
    EnableOK();
  else
    DisableOK();
  return 0;  
}

void FormPointMapSubMap::HideCoordsAndLatLon()
{
    fldMinLat->Hide();
    fldMaxLat->Hide();
    fldMinLon->Hide();
    fldMaxLon->Hide();
    fcMin->Hide();
    fcMax->Hide();
}

LRESULT Cmdgluepnt(CWnd *wnd, const String& s)
{
	new FormPointMapGlue(wnd, s.scVal());
	return -1;
}

FormPointMapGlue::FormPointMapGlue(CWnd* mw, const char* sPar)
: FormPointMapCreate(mw, SAFTitleGluePntMap)
{
  iMaps = 2;
	asMaps.resize(4);
  sMask1 = "*";
  sMask2 = "*";
  sMask3 = "*";
  sMask4 = "*";
  fClipBoundary = false;
  if (sPar) {
    TextInput inp(sPar);
    TokenizerBase tokenizer(&inp);
    String sVal;
    for (;;) {
      Token tok = tokenizer.tokGet();
      sVal = tok.sVal();
      if (sVal == "")
        break;
      FileName fn(sVal);
      if (fn.sExt == "" || fn.sExt == ".mpp")
        if (asMaps[0] == "")
          asMaps[0] = fn.sFullNameQuoted(false);
        else if (asMaps[1] == "") {
          asMaps[1] = fn.sFullNameQuoted(false);
          iMaps = 2;
        }  
        else if (asMaps[2] == "") {
          asMaps[2] = fn.sFullNameQuoted(false);
          iMaps = 3;
        }  
        else if (asMaps[3] == "") {
          asMaps[3] = fn.sFullNameQuoted(false);
          iMaps = 4;
        }  
        else  
          sOutMap = fn.sFullName(false);
    }
  }
	iMaps -= 2;
  StaticText* st = new StaticText(root, SAFUiNrInpMaps);
  st->SetIndependentPos();
  rgMaps = new RadioGroup(root, "", &iMaps, true);
  rgMaps->SetIndependentPos();
  rgMaps->SetCallBack((NotifyProc)&FormPointMapGlue::MapsCallBack);
  new RadioButton(rgMaps, "&2");
  new RadioButton(rgMaps, "&3");
  new RadioButton(rgMaps, "&4");

  fg1 = new FieldGroup(root,true);
  fg1->Align(rgMaps, AL_UNDER);
	FieldPointMap* fpm = new FieldPointMap(fg1, SAFUiMap, &(asMaps[0]), new MapListerDomainType(".mpp", 0/*dmVALUE|dmCLASS|dmIDENT|dmBOOL*/, true));
  fpm->SetCallBack((NotifyProc)&FormPointMapGlue::PointMap_1CallBack);
  FieldString* fs = new FieldString(fg1, SAFUiMask, &sMask1);
  fs->Align(fpm, AL_AFTER);
  
  fg2 = new FieldGroup(root,true);
  fg2->Align(fg1, AL_UNDER);
  fpm = new FieldPointMap(fg2, SAFUiMap, &(asMaps[1]), new MapListerDomainType(".mpp", 0/*dmVALUE|dmCLASS|dmIDENT|dmBOOL*/, true));
  fpm->SetCallBack((NotifyProc)&FormPointMapGlue::PointMap_2CallBack);
  fs = new FieldString(fg2, SAFUiMask, &sMask2);
  fs->Align(fpm, AL_AFTER);
  
  fg3 = new FieldGroup(root,true);
  fg3->Align(fg2, AL_UNDER);
  fpm = new FieldPointMap(fg3, SAFUiMap, &(asMaps[2]), new MapListerDomainType(".mpp", 0/*dmVALUE|dmCLASS|dmIDENT|dmBOOL*/, true));
  fpm->SetCallBack((NotifyProc)&FormPointMapGlue::PointMap_3CallBack);
  fs = new FieldString(fg3, SAFUiMask, &sMask3);
  fs->Align(fpm, AL_AFTER);
  
  fg4 = new FieldGroup(root,true);
  fg4->Align(fg3, AL_UNDER);
  fpm = new FieldPointMap(fg4, SAFUiMap, &(asMaps[3]), new MapListerDomainType(".mpp", 0/*dmVALUE|dmCLASS|dmIDENT|dmBOOL*/, true));
  fpm->SetCallBack((NotifyProc)&FormPointMapGlue::PointMap_4CallBack);
  fs = new FieldString(fg4, SAFUiMask, &sMask4);
  fs->Align(fpm, AL_AFTER);
  
  cbClip = new CheckBox(root, SAFUiClipBoundary, &fClipBoundary);
  cbClip->SetCallBack((NotifyProc)&FormPointMapGlue::CallBackClipBoundary);
  cbClip->Align(fg4, AL_UNDER);
  String sFill50('x', 50);
  stCsy = new StaticText(cbClip, sFill50);
  stCsy->Align(cbClip, AL_AFTER);
  FieldGroup* fg = new FieldGroup(cbClip);
  fg->Align(cbClip,AL_UNDER);
  cMin = cMax = Coord(0,0);
  fldCrdMin = new FieldCoord(fg, SAFUiMinXY, &cMin);
  fldCrdMin->SetCallBack((NotifyProc)&FormPointMapGlue::CallBackMinMaxXY);
  fldCrdMax = new FieldCoord(fg, SAFUiMaxXY, &cMax);
  fldCrdMax->SetCallBack((NotifyProc)&FormPointMapGlue::CallBackMinMaxXY);

  fldMinLat = new FieldLat(fg, SCSUiMinLat, &llMin.Lat);
  fldMinLat->Align(cbClip, AL_UNDER);
  fldMinLat->SetCallBack((NotifyProc)&FormPointMapGlue::CallBackMinMaxLatLon);
  fldMinLon = new FieldLon(fg, SCSUiMinLon, &llMin.Lon);
//  fldMinLon->Align(fldMinLat, AL_AFTER);
  fldMinLon->SetCallBack((NotifyProc)&FormPointMapGlue::CallBackMinMaxLatLon);
  fldMaxLat = new FieldLat(fg, SCSUiMaxLat, &llMax.Lat);
  fldMaxLat->Align(fldMinLon, AL_UNDER);
  fldMaxLat->SetCallBack((NotifyProc)&FormPointMapGlue::CallBackMinMaxLatLon);
  fldMaxLon = new FieldLon(fg, SCSUiMaxLon, &llMax.Lon);
//  fldMaxLon->Align(fldMaxLat, AL_AFTER);
  fldMaxLon->SetCallBack((NotifyProc)&FormPointMapGlue::CallBackMinMaxLatLon);

  fNewDom = true;
  cbDom = new CheckBox(root, SAFUiNewDomain, &fNewDom);
  cbDom->SetCallBack((NotifyProc)&FormPointMapGlue::DomCallBack);
  fldDom = new FieldDataTypeCreate(cbDom, "", &sNewDom, ".dom", true);
  fldDom->SetCallBack((NotifyProc)&FormPointMapGlue::DomCallBack);
  FieldBlank* fb = new FieldBlank(root, 0);
  fb->Align(cbDom, AL_UNDER);
	fInsideDomCallBack = false;
  initPointMapOut(false);
  String sFill('X', 40);
  stRemark = new StaticText(root, sFill);
  stRemark->SetIndependentPos();
  SetAppHelpTopic(htpPointMapGlue);
  create();
}                    

int FormPointMapGlue::PointMap_1CallBack(Event*)
{
  fg1->StoreData();
	PointMap_iCallBack(0);
	return 0;
}

int FormPointMapGlue::PointMap_2CallBack(Event*)
{
  fg2->StoreData();
	PointMap_iCallBack(1);
	return 0;
}

int FormPointMapGlue::PointMap_3CallBack(Event*)
{
  fg3->StoreData();
	PointMap_iCallBack(2);
	return 0;
}

int FormPointMapGlue::PointMap_4CallBack(Event*)
{
  fg4->StoreData();
	PointMap_iCallBack(3);
	return 0;
}

void FormPointMapGlue::PointMap_iCallBack(int iCurMap)
{
	fg1->StoreData();
	fg2->StoreData();
	fg3->StoreData();
	fg4->StoreData();
  stCsy->SetVal("");
	try {
		FileName fnMap( asMaps[iCurMap]); /// asMaps[0]);
		PointMap map;
		if (asMaps[iCurMap] != "")
			map = PointMap(fnMap);
		if (map.fValid()) { 
			cbClip->Show();
			CoordBounds cb = map->cb();
			CoordSystem csInput = map->cs();
			if (csInput.fValid()) {
				cMin = cb.cMin;
				cMax = cb.cMax;
				stCsy->SetVal(map->cs()->sTypeName());
				fFromLatLon = ( 0 !=map->cs()->pcsLatLon());
				if (!fFromLatLon)
				{
    			fldCrdMin->SetVal(cMin);
					fldCrdMax->SetVal(cMax);
				}
				else
				{
					llMin.Lat = cMin.y; 
					llMax.Lat = cMax.y;
					llMin.Lon = cMin.x; 
					llMax.Lon = cMax.x;
					fldMinLat->SetVal(llMin.Lat);
					fldMaxLat->SetVal(llMax.Lat);
					fldMinLon->SetVal(llMin.Lon);
					fldMaxLon->SetVal(llMax.Lon);
				}
				if (fClipBoundary) {
					if (!fFromLatLon)
						ShowCoords();
					else
						ShowLatLons();
				}
				else 
					HideCoordsAndLatLon();
				if (map->dm()->pdsrt())
					cbDom->Show();
				else {
					cbDom->Hide();
					fNewDom = false;
					stRemark->SetVal("");
					DomCallBack(0);
					EnableOK();
				}
			}
			else
					stCsy->SetVal("");
		}
		else
			cbClip->Hide();
		MapsCallBack(0);
  }
  catch (ErrorObject&) {}
}

int FormPointMapGlue::MapsCallBack(Event*)
{
  rgMaps->StoreData();
  iMaps += 2;
  switch (iMaps) {  // lots of fall throughs
    case 2: 
      fg3->Hide(); 
    case 3: 
      fg4->Hide(); 
  }
  switch (iMaps) {
    case 4: 
      fg4->Show(); 
    case 3: 
      fg3->Show();
    case 2: 
      fg2->Show(); 
    case 1:
      fg1->Show();  
  }
	Array<PointMap> aMaps;
	aMaps.Resize(iMaps);
	HideCoordsAndLatLon();

	for (int i = 0; i < iMaps ; i++) {
		FileName fnMap(asMaps[i]); 
		if (fnMap.fValid())
			aMaps[i] = PointMap(fnMap);
		else
			return 0;
	} 
	PointMapGlue::OutputDomain odFinal;
	try {
		PointMapGlue::CheckAndFindOutputDomain(iMaps, aMaps, String("PointMapGlue"), odFinal);
	}
	catch (ErrorObject&) {
		fNewDom = false;
		cbDom->SetVal(false);
		cbDom->Hide();
		DomCallBack(0);
    DisableOK();
		return 0;
	}
	//uniqId as glue output is an intern domain
	if (odFinal != PointMapGlue::odID && odFinal != PointMapGlue::odCLASS)
	{
		fNewDom = false;
		cbDom->SetVal(false);
		cbDom->Hide();
		DomCallBack(0);
	}
	else
		cbDom->Show();
	EnableOK();

  return 0;
}

int FormPointMapGlue::CallBackClipBoundary(Event*)
{ 
	HideCoordsAndLatLon();
	if (asMaps[0] == "") return 0;
	cbClip->StoreData();
	FileName fnMap(asMaps[0]); 
    PointMap map(fnMap);
	stCsy->SetVal("");
  if (map.fValid()) {
		if (fClipBoundary )
		{
			stCsy->SetVal(map->cs()->sTypeName());
			if (!fFromLatLon) 
			  ShowCoords();
			else
			  ShowLatLons();
		}
		else 
		{
			stCsy->SetVal("");
			HideCoordsAndLatLon();
		}
	}
	 return 0;  
}

void FormPointMapGlue::ShowCoords()
{

	  fldCrdMin->Show();
	  fldCrdMax->Show();
	  fldMinLat->Hide();
	  fldMaxLat->Hide();
	  fldMinLon->Hide();
	  fldMaxLon->Hide();
}

void FormPointMapGlue::ShowLatLons()
{
    fldMinLat->Show();
    fldMaxLat->Show();
    fldMinLon->Show();
    fldMaxLon->Show();
    fldCrdMin->Hide();
    fldCrdMax->Hide();
}

void FormPointMapGlue::HideCoordsAndLatLon()
{
    fldMinLat->Hide();
    fldMaxLat->Hide();
    fldMinLon->Hide();
    fldMaxLon->Hide();
    fldCrdMin->Hide();
    fldCrdMax->Hide();
}

int FormPointMapGlue::CallBackMinMaxXY(Event*)
{ 
	//fXYBoundariesOK();
  Coord cMin = fldCrdMin->crdVal();
  Coord cMax = fldCrdMax->crdVal();
  if (cMin.x < cMax.x && cMin.y < cMax.y)
    EnableOK();
  else
    DisableOK();
  return 0;  
}

int FormPointMapGlue::CallBackMinMaxLatLon(Event*)
{
	//fLatLonBoundariesOK();
  LatLon laloMin, laloMax;
  laloMin.Lat = fldMinLat->rVal();
  laloMin.Lon = fldMinLon->rVal();
  laloMax.Lat = fldMaxLat->rVal();
  laloMax.Lon = fldMaxLon->rVal();
  if (laloMin.Lat < laloMax.Lat && laloMin.Lon < laloMax.Lon)
    EnableOK();
  else
    DisableOK();
  return 0;  
}

int FormPointMapGlue::DomCallBack(Event*)
{
	if (fInsideDomCallBack)
		return 0;
	fInsideDomCallBack = true; //to prevent recursive calls of this callback
  cbDom->StoreData();
  bool fOk = false;
  if (!fNewDom) {
    stRemark->SetVal("");
    fOk = true;
  }
  else {
    fldDom->StoreData();
    FileName fn(sNewDom);
    if (!fn.fValid())
      stRemark->SetVal(SDMRemNotValidDomainName);
    else if(fn.fExist())
      stRemark->SetVal(SDMRemDomExists);
    else {
      fOk = true;
      stRemark->SetVal("");
    }
  }
  if (fOk)
    EnableOK();
  else    
    DisableOK();
	fInsideDomCallBack = false;
  return 0;
}

int FormPointMapGlue::exec() 
{
	FormPointMapCreate::exec();
	iMaps += 2;
	FileName fn(sOutMap);
	FileName fnMap1(asMaps[0]); 
	asMaps[0] = fnMap1.sRelativeQuoted(false,fn.sPath());
	FileName fnMap2(asMaps[1]); 
	asMaps[1] = fnMap2.sRelativeQuoted(false,fn.sPath());
	FileName fnMap3(asMaps[2]); 
	asMaps[2] = fnMap3.sRelativeQuoted(false,fn.sPath());
	FileName fnMap4(asMaps[3]); 
	asMaps[3] = fnMap4.sRelativeQuoted(false,fn.sPath());
	String sMapList;
	switch (iMaps) {
    case 1:
		sMapList = String("%S,\"%S\"", asMaps[0], sMask1);
		break;
    case 2:
		sMapList = String("%S,\"%S\",%S,\"%S\"", asMaps[0], sMask1, asMaps[1], sMask2);
		break;
    case 3:
		sMapList = String("%S,\"%S\",%S,\"%S\",%S,\"%S\"", 
			asMaps[0], sMask1, asMaps[1], sMask2, asMaps[2], sMask3);
		break;
    case 4:
		sMapList = String("%S,\"%S\",%S,\"%S\",%S,\"%S\",%S,\"%S\"", 
			asMaps[0], sMask1, asMaps[1], sMask2, asMaps[2], sMask3, asMaps[3], sMask4);
		break;
	}
	if (fNewDom)
	{
		FileName fnDom(sNewDom);
		sNewDom = fnDom.sRelativeQuoted(false,fn.sPath());
		sMapList &= String(",%S", sNewDom);
	}
	String sExpr;
	if (fClipBoundary)
	{
		if (fFromLatLon)
		{
			cMin.x = llMin.Lon;
			cMax.x = llMax.Lon;
			cMin.y = llMin.Lat;
			cMax.y = llMax.Lat;
			sExpr = String("PointMapGlue(%7f,%7f,%7f,%7f,%S)", 
				cMin.x, cMin.y, cMax.x, cMax.y, sMapList);
		}
		else
			sExpr = String("PointMapGlue(%lf,%lf,%lf,%lf,%S)", 
			cMin.x, cMin.y, cMax.x, cMax.y, sMapList);
	}
	else                  
		sExpr = String("PointMapGlue(%S)", sMapList);

	execPointMapOut(sExpr);  
	return 0;
}

LRESULT Cmdmaskpnt(CWnd *wnd, const String& s)
{
	new FormPointMapMask(wnd, s.scVal());
	return -1;
}

FormPointMapMask::FormPointMapMask(CWnd* mw, const char* sPar)
: FormPointMapCreate(mw, SAFTitleMaskPntMap)
{
  if (sPar) {
    TextInput inp(sPar);
    TokenizerBase tokenizer(&inp);
    String sVal;
    for (;;) {
      Token tok = tokenizer.tokGet();
      sVal = tok.sVal();
      if (sVal == "")
        break;
      FileName fn(sVal);
      if (fn.sExt == "" || fn.sExt == ".mpp")
        if (sPointMap == "")
          sPointMap = fn.sFullNameQuoted(false);
        else  
          sOutMap = fn.sFullName(false);
    }
  }
  new FieldPointMap(root, SAFUiPntMap, &sPointMap, new MapListerDomainType(".mpp", 0, true));
  new FieldString(root, SAFUiMask, &sMask, Domain(), false);
  initPointMapOut(false);
  SetAppHelpTopic(htpPointMapMask);
  create();
}                    

int FormPointMapMask::exec() 
{
  FormPointMapCreate::exec();
  String sExpr;
  FileName fn(sOutMap);
  FileName fnPointMap(sPointMap); 
  sPointMap = fnPointMap.sRelativeQuoted(false,fn.sPath());
  sExpr = String("PointMapMask(%S,\"%S\")", 
                  sPointMap, sMask);
  execPointMapOut(sExpr);  
  return 0;
}








