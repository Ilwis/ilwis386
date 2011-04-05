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
// $Log: /ILWIS 3.0/ApplicationForms/frmsegap.cpp $
 * 
 * 36    5-07-02 19:20 Koolhoven
 * prevent warnings with Visual Studio .Net
 * 
 * 35    6/21/01 10:58a Martin
 * added support for giving the rowcol/coord infor to a submap form
 * together with the filename
 * 
 * 34    20-03-01 10:28 Hendrikse
 * corrected constructor use : fnMap(asMaps[iCurMap] in SegmentMapGlue
 * form
 * 
 * 33    16/03/01 12:19 Willem
 * The column selection now also allows UniqueID columns
 * 
 * 32    22-02-01 21:39 Hendrikse
 * constr of FieldSegmentMap in FormSegmentMapGlue  now prevents use of
 * attrib columns in input segmaps
 * 
 * 31    19/02/01 10:48 Willem
 * Disbaled attribute selection in PolygonMaps for SegmentFromPolygon
 * 
 * 30    16/02/01 17:01 Willem
 * Map selection for raster attribute maps is now limited to
 * Class/Id/Group/Bool/UniqueID
 * 
 * 29    16-02-01 16:41 Hendrikse
 * - asMaps.resize added because its size must be known before use.
 * - added try catch in MapsCallBack
 * - added extra 	fInsideDomCallBack = false;
 * 
 * 28    8-02-01 17:03 Hendrikse
 * changed MapListerDomainType(".mps", 0, false) to disable attrib choice
 * SegTunneling, SegMask, SegTransform and SegSubMap
 * 
 * 27    1-02-01 17:35 Hendrikse
 * implemented added int SegmentMap_4CallBack(Event*); etc
 * 	void SegmentMap_iCallBack(int iCurMap);
 * and fInsideDomCallBack() for better domain control
 * also call to SegmentMapGlue::CheckAndFindOutputDomain  implemented
 * 
 * 26    29-11-00 12:15 Koolhoven
 * for poltoseg and rastoseg use single|unique options instead of
 * single|composite. 
 * SingleName checkbox now by default off
 * 
 * 25    24-11-00 19:12 Koolhoven
 * simplified FormSegmentMapPolBoundaries 
 * 
 * 24    14-11-00 9:58a Martin
 * added support for attrib table columns in the mapselectors
 * 
 * 23    11-11-00 14:07 Hendrikse
 * tunneling form restricts now to tunnelwidth in  ValueRangeReal(0, 1e6,
 * 0)
 * 
 * 22    8-11-00 17:25 Koolhoven
 * use sFullNameQuoted() on input data to ensure selection in forms
 * 
 * 21    5-10-00 17:40 Koolhoven
 * with Transform operation ask Coordinate System below output map
 * 
 * 20    30-08-00 2:32p Martin
 * 'attribute table ' nameedits changed/removed
 * 
 * 19    3-08-00 17:37 Koolhoven
 * also 4th map asked in glue segments asks now for segment maps
 * 
 * 18    3-08-00 13:02 Koolhoven
 * hide degree fields also in catch blocks
 * 
 * 17    20-07-00 10:51a Martin
 * wrong syntax was generated  for segglue
 * 
 * 16    18-07-00 11:54a Martin
 * extensions are now correct so a icon will show in the map nameeedit
 * 
 * 15    14-06-00 15:24 Koolhoven
 * DensifyCallBack() no longer shows both degrees and meters field when
 * SegMep construction fails
 * 
 * 14    14-06-00 15:21 Koolhoven
 * protected DensifyCallBack() against no segmentmap filled in yet or
 * other errors
 * 
 * 13    5/23/00 1:53p Wind
 * MapListerDomainType instances were used/shared by several formentries,
 * and deleted by them
 * 
 * 12    5/23/00 12:52p Wind
 * - accept  raster input maps with any domain in FormSegmentMapLabels
 * - accept  raster input maps with any domain in FormSegmentMapGlue
 * 
 * 11    11-05-00 3:57p Martin
 * added 'attribute columns' nameedits
 * 
 * 10    4/06/00 2:27p Hendrikse
 * adapted  FormSegmentMapGlue::exec() and  FormSegmentMapSubMap::exec()
 * to have an sExpressiin with sufficient precision in case of Latlon csy
 * 
 * 9     4/03/00 5:58p Hendrikse
 * implemented the additions mentioned in frmsegap.h
 * 
 * 8     27-03-00 14:50 Koolhoven
 * prevent warning of casting from a pointer to a bool
 * 
 * 7     14-03-00 18:33 Hendrikse
 * replaced SAFUiDistanceInDegrees by SAFUiDistance
 * 
 * 6     14-03-00 13:16 Hendrikse
 * added callbacks etc for FieldDMS* use with densif dist in degrees
 * added rDistance as new transformation parameter with default
 * 
 * 5     28-02-00 11:56 Wind
 * adapted to changes in constructor of FieldColumn
 * 
 * 4     16-11-99 10:21 Wind
 * implemented wish (bug 1280), optional domain for
 * segmentmapfrompolboundaries
 * 
 * 3     9/13/99 1:05p Wind
 * comments
 * 
 * 2     9/13/99 12:35p Wind
 * adapted :exec functions to use of quoted file names when building an
 * expression
*/
// Revision 1.13  1998/10/08 14:45:01  Wim
// Transform has now callback to tell source coordsys type name
//
// Revision 1.12  1998-09-16 18:33:54+01  Wim
// 22beta2
//
// Revision 1.11  1998/02/24 08:45:53  martin
// Attributemaps may be in different directories.
//
// Revision 1.10  1997/09/15 08:47:30  Wim
// Clear Remark and enable OK button in GlueMap when
// the new domain field is hidden
//
// Revision 1.9  1997-09-11 11:40:29+02  Wim
// Strip path from specified new domain name
//
// Revision 1.8  1997-09-09 18:33:49+02  Wim
// SegmentMapTunneling checkbox remove nodes independent, because it is a long text
//
// Revision 1.7  1997-08-27 15:33:57+02  Wim
// GlueSeg is now default new domain
//
// Revision 1.6  1997-08-13 12:20:19+02  Wim
// FieldDataType instead FieldMap in segfromras
//
// Revision 1.5  1997-08-13 10:18:44+02  Wim
// SegFromRas only allow id, class and bool maps.
//
// Revision 1.4  1997-07-30 16:52:33+02  Wim
// Added stRemark field
//
// Revision 1.3  1997-07-30 16:49:00+02  Wim
// Added New Domain option in SegmentMapGlue to create a new domain instead of an internal domain
//
// Revision 1.2  1997-07-30 09:57:12+02  Wim
// Added fSingleName option in polseg and rasseg.
//
/* Form SegmentMap Applications
   by Wim Koolhoven
   (c) Ilwis System Development ITC
	Last change:  WK    8 Oct 98    2:11 pm
*/
//#include "Client\MainWindow\mainwind.h"
#include "Client\Headers\AppFormsPCH.h"
#include "Engine\Applications\SEGVIRT.H"
#include "SegmentApplications\SEGGLUE.H"
#include "Client\FormElements\fldcs.h"
#include "SegmentApplicationsUI\frmsegap.h"
#include "Engine\Table\COLINFO.H"
#include "Headers\Hs\DOMAIN.hs"
#include "SegmentApplications\SEGTRNSF.H"
#include "Headers\Hs\Coordsys.hs"

LRESULT Cmdattribseg(CWnd *wnd, const String& s)
{
	new FormAttributeSegmentMap(wnd, s.scVal());
	return -1;
}

FormAttributeSegmentMap::FormAttributeSegmentMap(CWnd* mw, const char* sPar)
: FormSegmentMapCreate(mw, SAFTitleAttribSegMap)
{
  if (sPar) {
    TextInput ip(sPar);
    TokenizerBase tokenizer(&ip);
    String sVal;
    for (;;) {
      Token tok = tokenizer.tokGet();
      sVal = tok.sVal();
      if (sVal == "")
        break;
      FileName fn(sVal);
      if (fn.sExt == "" || fn.sExt == ".mps")
        if (sSegmentMap == "")
          sSegmentMap = fn.sFullNameQuoted(false);
        else  
          sOutMap = fn.sFullName(false);
    }
  }
  fldSegmentMap = new FieldSegmentMap(root, SAFUiSegMap, &sSegmentMap,
  	                                  new MapListerDomainType(".mps", dmCLASS | dmIDENT | dmGROUP | dmBOOL | dmUNIQUEID));
  fldSegmentMap->SetCallBack((NotifyProc)&FormAttributeSegmentMap::SegmentMapCallBack);
  fldTbl = new FieldTable(root, SAFUiTable, &sTbl);
  fldTbl->SetCallBack((NotifyProc)&FormAttributeSegmentMap::TblCallBack);
  fldCol = new FieldColumn(root, SAFUiAttribute, Table(), &sCol,
    dmCLASS|dmIDENT|dmUNIQUEID|dmVALUE|dmBOOL);
  fldCol->SetCallBack((NotifyProc)&FormAttributeSegmentMap::ColCallBack);
  stColRemark = new StaticText(root, String('x',50));
  stColRemark->SetIndependentPos();
  initSegmentMapOut(false);
  SetAppHelpTopic(htpSegmentMapAttribute);
  create();
}                    

int FormAttributeSegmentMap::exec() 
{
  FormSegmentMapCreate::exec();
  String sExpr;
  bool fShort = false;
  FileName fn(sOutMap);
  FileName fnTbl(sTbl);
  FileName fnSegmentMap(sSegmentMap); 
  SegmentMap map(fnSegmentMap);
  if (map.fValid()) {
    if (map->fTblAtt()) {
      Table tbl = map->tblAtt();
      fShort = (tbl->fnObj == fnTbl);
    }  
  }    
  sSegmentMap = fnSegmentMap.sRelativeQuoted(false,fn.sPath());
  if (!fShort)
    sCol = String("%S.%S", sTbl, sCol);
  sExpr = String("SegmentMapAttribute(%S,%S)", 
                  sSegmentMap,sCol);
  execSegmentMapOut(sExpr);  
  return 0;
}

int FormAttributeSegmentMap::SegmentMapCallBack(Event*)
{
  fldSegmentMap->StoreData();
  fldCol->FillWithColumns((TablePtr*)0);
  try {
    FileName fnMap(sSegmentMap); 
    SegmentMap map(fnMap);
    if (map.fValid()) {
      fldTbl->SetDomain(map->dm()->sName());
      if (map->fTblAtt()) {
        fldTbl->SetVal(map->tblAtt()->fnObj.sFullPath());
      }
    }  
  }
  catch (ErrorObject&) {}
  return 0;
}

int FormAttributeSegmentMap::TblCallBack(Event*)
{
  fldTbl->StoreData();
  fldCol->FillWithColumns((TablePtr*)0);
  try {
    FileName fnTbl(sTbl); 
    fldCol->FillWithColumns(fnTbl);
    ColCallBack(0);
  }
  catch (ErrorObject&) {}
  return 0;
}

int FormAttributeSegmentMap::ColCallBack(Event*)
{
  fldCol->StoreData();
  try {
    FileName fnTbl(sTbl); 
    ColumnInfo colinf(fnTbl, sCol);
    Domain dm = colinf.dm();
    String sRemark(dm->sTypeName());
    stColRemark->SetVal(sRemark);
  }
  catch (ErrorObject&) {
    stColRemark->SetVal("");
  }
  return 0;
}

LRESULT Cmdtunnelseg(CWnd *wnd, const String& s)
{
	new FormSegmentMapTunneling(wnd, s.scVal());
	return -1;
}

FormSegmentMapTunneling::FormSegmentMapTunneling(CWnd* mw, const char* sPar)
: FormSegmentMapCreate(mw, SAFTitleTunnelSegMap)
{
  if (sPar) {
    TextInput ip(sPar);
    TokenizerBase tokenizer(&ip);
    String sVal;
    for (;;) {
      Token tok = tokenizer.tokGet();
      sVal = tok.sVal();
      if (sVal == "")
        break;
      FileName fn(sVal);
      if (fn.sExt == "" || fn.sExt == ".mps")
        if (sSegmentMap == "")
          sSegmentMap = fn.sFullNameQuoted(false);
        else  
          sOutMap = fn.sFullName(false);
    }
  }
  new FieldSegmentMap(root, SAFUiSegMap, &sSegmentMap, new MapListerDomainType(".mps", 0, false));
  rTunnelWidth = 1.0;
  new FieldReal(root, SAFUiTunnelWidth, &rTunnelWidth, ValueRangeReal(0, 1e6, 0));
  fRemoveNodes = false;
  CheckBox* cb = new CheckBox(root, SAFUIRemoveNodes, &fRemoveNodes);
  cb->SetIndependentPos();
  initSegmentMapOut(false);
  SetAppHelpTopic(htpSegmentMapTunneling);
  create();
}                    

int FormSegmentMapTunneling::exec() 
{
  FormSegmentMapCreate::exec();
  String sExpr;
  FileName fn(sOutMap);
  FileName fnSegmentMap(sSegmentMap); 
  sSegmentMap = fnSegmentMap.sRelativeQuoted(false,fn.sPath());
  sExpr = String("SegmentMapTunneling(%S,%f,%s)", 
                  sSegmentMap,rTunnelWidth,fRemoveNodes?"yes":"no");
  execSegmentMapOut(sExpr);  
  return 0;
}

LRESULT Cmdlabelseg(CWnd *wnd, const String& s)
{
	new FormSegmentMapLabels(wnd, s.scVal());
	return -1;
}

FormSegmentMapLabels::FormSegmentMapLabels(CWnd* mw, const char* sPar)
: FormSegmentMapCreate(mw, SAFTitleLabelSegMap)
{
  if (sPar) {
    TextInput ip(sPar);
    TokenizerBase tokenizer(&ip);
    String sVal;
    for (;;) {
      Token tok = tokenizer.tokGet();
      sVal = tok.sVal();
      if (sVal == "")
        break;
      FileName fn(sVal);
      if (fn.sExt == "" || fn.sExt == ".mps")
        if (sSegmentMap == "")
          sSegmentMap = fn.sFullNameQuoted(false);
        else  
          sOutMap = fn.sFullName(false);
    }
  }
  new FieldSegmentMap(root, SAFUiSegMap, &sSegmentMap);
  new FieldPointMap(root, SAFUiLabelPnts, &sLabels, new MapListerDomainType(".mpp", 0/*dmVALUE|dmCLASS|dmIDENT|dmBOOL*/, true));
  initSegmentMapOut(false);
  SetAppHelpTopic(htpSegmentMapLabels);
  create();
}                    

int FormSegmentMapLabels::exec() 
{
  FormSegmentMapCreate::exec();
  String sExpr;
  FileName fn(sOutMap);
  FileName fnSegmentMap(sSegmentMap); 
  FileName fnLabels(sLabels); 
  sSegmentMap = fnSegmentMap.sRelativeQuoted(false,fn.sPath());
  sLabels = fnLabels.sRelativeQuoted(false,fn.sPath());
  sExpr = String("SegmentMapLabels(%S,%S)", 
                  sSegmentMap, sLabels);
  execSegmentMapOut(sExpr);  
  return 0;
}

LRESULT Cmdmaskseg(CWnd *wnd, const String& s)
{
	new FormSegmentMapMask(wnd, s.scVal());
	return -1;
}

FormSegmentMapMask::FormSegmentMapMask(CWnd* mw, const char* sPar)
: FormSegmentMapCreate(mw, SAFTitleMaskSegMap)
{
  if (sPar) {
    TextInput ip(sPar);
    TokenizerBase tokenizer(&ip);
    String sVal;
    for (;;) {
      Token tok = tokenizer.tokGet();
      sVal = tok.sVal();
      if (sVal == "")
        break;
      FileName fn(sVal);
      if (fn.sExt == "" || fn.sExt == ".mps")
        if (sSegmentMap == "")
          sSegmentMap = fn.sFullNameQuoted(false);
        else  
          sOutMap = fn.sFullName(false);
    }
  }
  new FieldSegmentMap(root, SAFUiSegMap, &sSegmentMap, new MapListerDomainType(".mps", 0, false));
  new FieldString(root, SAFUiMask, &sMask, Domain(), false);
  initSegmentMapOut(false);
  SetAppHelpTopic(htpSegmentMapMask);
  create();
}                    

int FormSegmentMapMask::exec() 
{
  FormSegmentMapCreate::exec();
  String sExpr;
  FileName fn(sOutMap);
  FileName fnSegmentMap(sSegmentMap); 
  sSegmentMap = fnSegmentMap.sRelativeQuoted(false,fn.sPath());
  sExpr = String("SegmentMapMask(%S,\"%S\")", 
                  sSegmentMap, sMask);
  execSegmentMapOut(sExpr);  
  return 0;
}

LRESULT Cmdcleanseg(CWnd *wnd, const String& s)
{
	new FormSegmentMapCleaning(wnd, s.scVal());
	return -1;
}

FormSegmentMapCleaning::FormSegmentMapCleaning(CWnd* mw, const char* sPar)
: FormSegmentMapCreate(mw, SAFTitleCleanSegMap)
{
  if (sPar) {
    TextInput ip(sPar);
    TokenizerBase tokenizer(&ip);
    String sVal;
    for (;;) {
      Token tok = tokenizer.tokGet();
      sVal = tok.sVal();
      if (sVal == "")
        break;
      FileName fn(sVal);
      if (fn.sExt == "" || fn.sExt == ".mps")
        if (sSegmentMap == "")
          sSegmentMap = fn.sFullNameQuoted(false);
        else  
          sOutMap = fn.sFullName(false);
    }
  }
  new FieldSegmentMap(root, SAFUiSegMap, &sSegmentMap);
  initSegmentMapOut(false);
  SetAppHelpTopic(htpSegmentMapCleaning);
  create();
}                    

int FormSegmentMapCleaning::exec() 
{
  FormSegmentMapCreate::exec();
  String sExpr;
  FileName fn(sOutMap);
  FileName fnSegmentMap(sSegmentMap); 
  sSegmentMap = fnSegmentMap.sRelativeQuoted(false,fn.sPath());
  sExpr = String("SegmentMapCleaning(%S)", 
                  sSegmentMap);
  execSegmentMapOut(sExpr);  
  return 0;
}

LRESULT Cmdtransfseg(CWnd *wnd, const String& s)
{
	new FormSegmentMapTransform(wnd, s.scVal());
	return -1;
}

FormSegmentMapTransform::FormSegmentMapTransform(CWnd* mw, const char* sPar)
: FormSegmentMapCreate(mw, SAFTitleTransfSegMap)
{
  if (sPar) {
    TextInput ip(sPar);
    TokenizerBase tokenizer(&ip);
    String sVal;
    for (;;) {
      Token tok = tokenizer.tokGet();
      sVal = tok.sVal();
      if (sVal == "")
        break;
      FileName fn(sVal);
      if (fn.sExt == ".csy")
        sCoordSys = fn.sFullNameQuoted(false);
      else if (fn.sExt == "" || fn.sExt == ".mps")
        if (sSegmentMap == "")
          sSegmentMap = fn.sFullNameQuoted(false);
        else  
          sOutMap = fn.sFullName(false);
    }
  }
  fldMap = new FieldSegmentMap(root, SAFUiSegMap, &sSegmentMap, new MapListerDomainType(".mps", 0, false));
  fldMap->SetCallBack((NotifyProc)&FormSegmentMapTransform::MapCallBack);
  String sFill('x', 50);
  stCsy = new StaticText(root, sFill);
  stCsy->SetIndependentPos();
	fDensify = false;
	cbDensify = new CheckBox(root, SAFUiDensify, &fDensify);
	cbDensify->SetCallBack((NotifyProc)&FormSegmentMapTransform::DensifyCallBack);

	cbDensify->SetIndependentPos();

	fldDegMinSec = new FieldDMS(cbDensify, SAFUiDistance, &rDistance, 30, true);
	fldDegMinSec->Align(cbDensify, AL_UNDER);
	fldMeters = new FieldReal(cbDensify, SAFUiDistanceInMeters, &rDistance, ValueRange(0.001,1e12,0));
	fldMeters->Align(cbDensify, AL_UNDER);

// do not call initSegmentMapOut(false) because also csy has to be asked
  fmc = new FieldSegmentMapCreate(root, SAFUiOutSegMap, &sOutMap);
  fmc->SetCallBack((NotifyProc)&FormSegmentMapCreate::OutSegmentMapCallBack);
	new FieldCoordSystemC(root, SAFUiCoordSys, &sCoordSys);
  StaticText* st = new StaticText(root, SAFUiDescription);
  st->psn->SetBound(0,0,0,0);
  FieldString* fs = new FieldString(root, "", &sDescr);
  fs->SetWidth(120);
  fs->SetIndependentPos();
	
  SetAppHelpTopic(htpSegmentMapTransform);
  create();
}                    

int FormSegmentMapTransform::MapCallBack(Event*)
{
  fldMap->StoreData();
  try {
    FileName fnMap(sSegmentMap); 
    SegmentMap map(fnMap);
    if (map.fValid()) {
      stCsy->SetVal(map->cs()->sTypeName());
			fFromLatLon = 0 != map->cs()->pcsLatLon();
			if (fDensify) 
			{
				rDistance = SegmentMapTransform::rDefaultDensifyDistance(map);
				if (fFromLatLon) 
				{
					fldDegMinSec->SetVal(rDistance);
					fldMeters->Hide();
					fldDegMinSec->Show();
				}
				else {
					fldMeters->SetVal(rDistance);
					fldMeters->Show();
					fldDegMinSec->Hide();
				}     
			}
			else
			{
				rDistance = 0;
				fldMeters->Hide();
				fldDegMinSec->Hide();
			}
		}
		else
			stCsy->SetVal("");
  }
  catch (ErrorObject&) {
    stCsy->SetVal("");
		fldMeters->Hide();
		fldDegMinSec->Hide();
  }

  return 0;
}

int FormSegmentMapTransform::DensifyCallBack(Event*)
{
	cbDensify->StoreData();
  try {
		FileName fnMap(sSegmentMap); 
		SegmentMap map(fnMap);
		if (map.fValid()) {
			stCsy->SetVal(map->cs()->sTypeName());
			fFromLatLon = 0 != map->cs()->pcsLatLon();
			if (fDensify) 
			{
				rDistance = SegmentMapTransform::rDefaultDensifyDistance(map);
				if (fFromLatLon) 
				{
					fldDegMinSec->SetVal(rDistance);
					fldMeters->Hide();
					fldDegMinSec->Show();
				}
				else 
				{
					fldMeters->SetVal(rDistance);
					fldMeters->Show();
					fldDegMinSec->Hide();
				}     
			}
			else
			{
				rDistance = 0;
				fldMeters->Hide();
				fldDegMinSec->Hide();
			}
		}
		else {
			stCsy->SetVal("");
			rDistance = 0;
			fldMeters->Hide();
			fldDegMinSec->Hide();
		}
	}
  catch (ErrorObject&) {
    stCsy->SetVal("");
		rDistance = 0;
		fldMeters->Hide();
		fldDegMinSec->Hide();
  }
	return 0;
}

int FormSegmentMapTransform::exec() 
{
  FormSegmentMapCreate::exec();
  String sExpr;
  FileName fn(sOutMap);
  FileName fnSegmentMap(sSegmentMap); 
  FileName fnCoordSys(sCoordSys); 
  sSegmentMap = fnSegmentMap.sRelativeQuoted(false,fn.sPath());
  sCoordSys = fnCoordSys.sRelativeQuoted(false,fn.sPath());
  sExpr = String("SegmentMapTransform(%S,%S,%lf)", 
                  sSegmentMap, sCoordSys, rDistance);
  execSegmentMapOut(sExpr);  
  return 0;
}

LRESULT Cmdsubseg(CWnd *wnd, const String& s)
{
	new FormSegmentMapSubMap(wnd, s.scVal());
	return -1;
}

FormSegmentMapSubMap::FormSegmentMapSubMap(CWnd* mw, const char* sPar)
: FormSegmentMapCreate(mw, SAFTitleSubMapSegMap),
  fPreset(false)
{
  cMin = cMax = Coord(0,0);	
  if (sPar) {
    TextInput ip(sPar);
    TokenizerBase tokenizer(&ip);
    String sVal;
    for (;;) 
		{
      Token tok = tokenizer.tokGet();
      sVal = tok.sVal();
      if (sVal == "")
        break;
      FileName fn(sVal);
			if ( tok.iPos() == 0)
			{
	      if (fn.sExt == "" || fn.sExt == ".mps")
		      if (sSegmentMap == "")
			      sSegmentMap = fn.sFullNameQuoted(false);
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
  fldSegmentMap = new FieldSegmentMap(root, SAFUiSegMap, &sSegmentMap, new MapListerDomainType(".mps", 0, false));
  fldSegmentMap->SetCallBack((NotifyProc)&FormSegmentMapSubMap::SegmentMapCallBack);
  String sFill('x', 50);
  stCsy = new StaticText(root, sFill);
  stCsy->SetIndependentPos();
  FieldGroup *fg = new FieldGroup(root);
  fg->SetIndependentPos();
  fldCrdMin = new FieldCoord(fg, SAFUiMinXY, &cMin);
  fldCrdMin->SetCallBack((NotifyProc)&FormSegmentMapSubMap::CallBackMinMaxXY);
  fldCrdMax = new FieldCoord(fg, SAFUiMaxXY, &cMax);
  fldCrdMax->SetCallBack((NotifyProc)&FormSegmentMapSubMap::CallBackMinMaxXY);
  fldMinLat = new FieldLat(fg, SCSUiMinLat, &llMin.Lat);
  fldMinLat->Align(stCsy, AL_UNDER);
  fldMinLat->SetCallBack((NotifyProc)&FormSegmentMapSubMap::CallBackMinMaxLatLon);
  fldMinLon = new FieldLon(fg, SCSUiMinLon, &llMin.Lon);
  fldMinLon->SetCallBack((NotifyProc)&FormSegmentMapSubMap::CallBackMinMaxLatLon);
  fldMaxLat = new FieldLat(fg, SCSUiMaxLat, &llMax.Lat);
  fldMaxLat->Align(fldMinLon, AL_UNDER);
  fldMaxLat->SetCallBack((NotifyProc)&FormSegmentMapSubMap::CallBackMinMaxLatLon);
  fldMaxLon = new FieldLon(fg, SCSUiMaxLon, &llMax.Lon);
  fldMaxLon->SetCallBack((NotifyProc)&FormSegmentMapSubMap::CallBackMinMaxLatLon);
  initSegmentMapOut(false);
  SetAppHelpTopic(htpSegmentMapSubMap);
  create();
}                    

int FormSegmentMapSubMap::SegmentMapCallBack(Event*)
{
  fldSegmentMap->StoreData();
  try 
	{
	    FileName fnMap(sSegmentMap); 
	    SegmentMap map(fnMap);
	    if (map.fValid()) 
			{
				CoordBounds cb = fPreset ? CoordBounds(cMin, cMax) : map->cb();
			  CoordSystem csInput = map->cs();   
			  if (csInput.fValid()) 
				{
			      cMin = cb.cMin;
				    cMax = cb.cMax;
				    stCsy->SetVal(map->cs()->sTypeName());
				    fFromLatLon = ( 0 !=map->cs()->pcsLatLon());
			    if (!fFromLatLon)
					{
					  fldCrdMin->SetVal(cb.cMin);
					  fldCrdMax->SetVal(cb.cMax);
					  fldCrdMin->Show();
					  fldCrdMax->Show();
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
					  fldCrdMin->Hide();
					  fldCrdMax->Hide();
					}
			  }
			  else 
				{
		      stCsy->SetVal("");
					HideCoordsAndLatLon();
				}
	    }
		else 
		{
			stCsy->SetVal("");
			HideCoordsAndLatLon();
		}
  }
  catch (ErrorObject&) 
	{
		stCsy->SetVal("");
		HideCoordsAndLatLon();
  }
  return 0;
}

int FormSegmentMapSubMap::CallBackMinMaxXY(Event*) 
{
  Coord cMin = fldCrdMin->crdVal();
  Coord cMax = fldCrdMax->crdVal();
  if (cMin.x < cMax.x && cMin.y < cMax.y)
    EnableOK();
  else
    DisableOK();  
  return 0;  
}

int FormSegmentMapSubMap::CallBackMinMaxLatLon(Event*) 
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

int FormSegmentMapSubMap::exec() 
{
  FormSegmentMapCreate::exec();
  String sExpr;
  FileName fn(sOutMap);
  FileName fnSegmentMap(sSegmentMap);
  sSegmentMap = fnSegmentMap.sRelativeQuoted(false,fn.sPath());
  if (fFromLatLon)
  {
    cMin.x = llMin.Lon;
	cMax.x = llMax.Lon;
	cMin.y = llMin.Lat;
	cMax.y = llMax.Lat;
	sExpr = String("SegmentMapSubMap(%S,%.7f,%.7f,%.7f,%.7f)", 
                  sSegmentMap, cMin.x, cMin.y, cMax.x, cMax.y);
  }
  else
	sExpr = String("SegmentMapSubMap(%S,%lf,%lf,%lf,%lf)", 
                  sSegmentMap, cMin.x, cMin.y, cMax.x, cMax.y);
  execSegmentMapOut(sExpr);  
  return 0;
}

void FormSegmentMapSubMap::HideCoordsAndLatLon()
{
    fldMinLat->Hide();
    fldMaxLat->Hide();
    fldMinLon->Hide();
    fldMaxLon->Hide();
    fldCrdMin->Hide();
    fldCrdMax->Hide();
}

LRESULT Cmddensseg(CWnd *wnd, const String& s)
{
	new FormSegmentMapDensify(wnd, s.scVal());
	return -1;
}

FormSegmentMapDensify::FormSegmentMapDensify(CWnd* mw, const char* sPar)
: FormSegmentMapCreate(mw, SAFTitleDensifySegMap)
{
  if (sPar) {
    TextInput ip(sPar);
    TokenizerBase tokenizer(&ip);
    String sVal;
    for (;;) {
      Token tok = tokenizer.tokGet();
      sVal = tok.sVal();
      if (sVal == "")
        break;
      FileName fn(sVal);
      if (fn.sExt == "" || fn.sExt == ".mps")
        if (sSegmentMap == "")
          sSegmentMap = fn.sFullNameQuoted(false);
        else  
          sOutMap = fn.sFullName(false);
    }
  }
  fldMap = new FieldSegmentMap(root, SAFUiSegMap, &sSegmentMap);
	fldMap->SetCallBack((NotifyProc)&FormSegmentMapDensify::SegmentMapCallBack);

	fldMeters = new FieldReal(root, SAFUiDistanceInMeters, &rDistance, ValueRange(0.001,1e12,0));
	fldMeters->Align(fldMap, AL_UNDER);
	fldDegMinSec = new FieldDMS(root, SAFUiDistance, &rDistance, 30, true);
	fldDegMinSec->Align(fldMap, AL_UNDER);

  initSegmentMapOut(false);
  SetAppHelpTopic(htpSegmentMapDensifyCoords);
  create();
}                    

int FormSegmentMapDensify::SegmentMapCallBack(Event*)
{
  fldMap->StoreData();
	CoordSystem csInput;
	fFromLatLon = false;
  try {
    FileName fnMap(sSegmentMap); 
    SegmentMap map(fnMap);
    if (map.fValid()) {
			rDistance = SegmentMapTransform::rDefaultDensifyDistance(map);
			csInput = map->cs();
			if (csInput->pcsLatLon()) {
				fFromLatLon = true;
				fldDegMinSec->SetVal(rDistance);
				fldMeters->Hide();
				fldDegMinSec->Show();
			}
			else {
				fldMeters->SetVal(rDistance);
				fldMeters->Show();
				fldDegMinSec->Hide();    
			}
		}
  }
  catch (ErrorObject&) 
	{
		fldMeters->Show();
		fldDegMinSec->Hide();    
	}
  return 0;
}
int FormSegmentMapDensify::exec() 
{
  FormSegmentMapCreate::exec();
  String sExpr;
  FileName fn(sOutMap);
  FileName fnSegmentMap(sSegmentMap); 
  sSegmentMap = fnSegmentMap.sRelativeQuoted(false,fn.sPath());
  sExpr = String("SegmentMapDensifyCoords(%S,%lf)", 
                  sSegmentMap, rDistance);
  execSegmentMapOut(sExpr);  
  return 0;
}

LRESULT Cmdglueseg(CWnd *wnd, const String& s)
{
	new FormSegmentMapGlue(wnd, s.scVal());
	return -1;
}

FormSegmentMapGlue::FormSegmentMapGlue(CWnd* mw, const char* sPar)
: FormSegmentMapCreate(mw, SAFTitleGlueSegMap)
{
  iMaps = 2;
	asMaps.resize(4);
  sMask1 = "*";
  sMask2 = "*";
  sMask3 = "*";
  sMask4 = "*";
  fClipBoundary = false;
  if (sPar) {
    TextInput ip(sPar);
    TokenizerBase tokenizer(&ip);
    String sVal;
    for (;;) {
      Token tok = tokenizer.tokGet();
      sVal = tok.sVal();
      if (sVal == "")
        break;
      FileName fn(sVal);
      if (fn.sExt == "" || fn.sExt == ".mps")
        if (asMaps[0] == "")
          (asMaps[0]) = fn.sFullNameQuoted(false);
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
  rgMaps->SetCallBack((NotifyProc)&FormSegmentMapGlue::MapsCallBack);
  new RadioButton(rgMaps, "&2");
  new RadioButton(rgMaps, "&3");
  new RadioButton(rgMaps, "&4");

  fg1 = new FieldGroup(root,true);
  fg1->Align(rgMaps, AL_UNDER);
  FieldSegmentMap* fsm = new FieldSegmentMap(fg1, SAFUiMap, &(asMaps[0]), new MapListerDomainType(".mps", 0, false));
  fsm->SetCallBack((NotifyProc)&FormSegmentMapGlue::SegmentMap_1CallBack);
  FieldString* fs = new FieldString(fg1, SAFUiMask, &sMask1);
  fs->Align(fsm, AL_AFTER);
  
  fg2 = new FieldGroup(root,true);
  fg2->Align(fg1, AL_UNDER);
  fsm = new FieldSegmentMap(fg2, SAFUiMap, &(asMaps[1]), new MapListerDomainType(".mps", 0/*dmVALUE|dmCLASS|dmIDENT|dmBOOL*/, false));
  fsm->SetCallBack((NotifyProc)&FormSegmentMapGlue::SegmentMap_2CallBack);
	fs = new FieldString(fg2, SAFUiMask, &sMask2);
  fs->Align(fsm, AL_AFTER);
  
  fg3 = new FieldGroup(root,true);
  fg3->Align(fg2, AL_UNDER);
  fsm = new FieldSegmentMap(fg3, SAFUiMap, &(asMaps[2]), new MapListerDomainType(".mps", 0/*dmVALUE|dmCLASS|dmIDENT|dmBOOL*/, false));
  fsm->SetCallBack((NotifyProc)&FormSegmentMapGlue::SegmentMap_3CallBack);
	fs = new FieldString(fg3, SAFUiMask, &sMask3);
  fs->Align(fsm, AL_AFTER);
  
  fg4 = new FieldGroup(root,true);
  fg4->Align(fg3, AL_UNDER);
  fsm = new FieldSegmentMap(fg4, SAFUiMap, &(asMaps[3]), new MapListerDomainType(".mps", 0/*dmCLASS|dmIDENT|dmBOOL|dmVALUE|dmIMAGE*/, false));
  fsm->SetCallBack((NotifyProc)&FormSegmentMapGlue::SegmentMap_4CallBack);
	fs = new FieldString(fg4, SAFUiMask, &sMask4);
  fs->Align(fsm, AL_AFTER);
  
  cbClip = new CheckBox(root, SAFUiClipBoundary, &fClipBoundary);
  cbClip->SetCallBack((NotifyProc)&FormSegmentMapGlue::CallBackClipBoundary);
  cbClip->Align(fg4, AL_UNDER);
  String sFill50('x', 50);
  stCsy = new StaticText(cbClip, sFill50);
  stCsy->Align(cbClip, AL_AFTER);
  FieldGroup* fg = new FieldGroup(cbClip);
  fg->Align(cbClip,AL_UNDER);
  cMin = cMax = Coord(0,0);
  fldCrdMin = new FieldCoord(fg, SAFUiMinXY, &cMin);
  fldCrdMin->SetCallBack((NotifyProc)&FormSegmentMapGlue::CallBackMinMaxXY);
  fldCrdMax = new FieldCoord(fg, SAFUiMaxXY, &cMax);
  fldCrdMax->SetCallBack((NotifyProc)&FormSegmentMapGlue::CallBackMinMaxXY);

  fldMinLat = new FieldLat(fg, SCSUiMinLat, &llMin.Lat);
  fldMinLat->Align(cbClip, AL_UNDER);
  fldMinLat->SetCallBack((NotifyProc)&FormSegmentMapGlue::CallBackMinMaxLatLon);
  fldMinLon = new FieldLon(fg, SCSUiMinLon, &llMin.Lon);
//  fldMinLon->Align(fldMinLat, AL_AFTER);
  fldMinLon->SetCallBack((NotifyProc)&FormSegmentMapGlue::CallBackMinMaxLatLon);
  fldMaxLat = new FieldLat(fg, SCSUiMaxLat, &llMax.Lat);
  fldMaxLat->Align(fldMinLon, AL_UNDER);
  fldMaxLat->SetCallBack((NotifyProc)&FormSegmentMapGlue::CallBackMinMaxLatLon);
  fldMaxLon = new FieldLon(fg, SCSUiMaxLon, &llMax.Lon);
//  fldMaxLon->Align(fldMaxLat, AL_AFTER);
  fldMaxLon->SetCallBack((NotifyProc)&FormSegmentMapGlue::CallBackMinMaxLatLon);

  fNewDom = true;
  cbDom = new CheckBox(root, SAFUiNewDomain, &fNewDom);
  cbDom->SetCallBack((NotifyProc)&FormSegmentMapGlue::DomCallBack);
  fldDom = new FieldDataTypeCreate(cbDom, "", &sNewDom, ".dom", true);
  fldDom->SetCallBack((NotifyProc)&FormSegmentMapGlue::DomCallBack);
  FieldBlank* fb = new FieldBlank(root, 0);
  fb->Align(cbDom, AL_UNDER);
	fInsideDomCallBack = false;
  initSegmentMapOut(false);
  SetAppHelpTopic(htpSegmentMapGlue);
  String sFill('X', 40);
  stRemark = new StaticText(root, sFill);
  stRemark->SetIndependentPos();
	SetAppHelpTopic(htpSegmentMapGlue);
  create();
}                    

int FormSegmentMapGlue::SegmentMap_1CallBack(Event*)
{
  fg1->StoreData();
	SegmentMap_iCallBack(0);
	return 0;
}

int FormSegmentMapGlue::SegmentMap_2CallBack(Event*)
{
  fg2->StoreData();
	SegmentMap_iCallBack(1);
	return 0;
}

int FormSegmentMapGlue::SegmentMap_3CallBack(Event*)
{
  fg3->StoreData();
	SegmentMap_iCallBack(2);
	return 0;
}

int FormSegmentMapGlue::SegmentMap_4CallBack(Event*)
{
  fg4->StoreData();
	SegmentMap_iCallBack(3);
	return 0;
}

void FormSegmentMapGlue::SegmentMap_iCallBack(int iCurMap)
{
  fg1->StoreData();
	fg2->StoreData();
	fg3->StoreData();
	fg4->StoreData();
  stCsy->SetVal("");
  try {
    FileName fnMap(asMaps[iCurMap]); 
    SegmentMap map;
		if (fnMap.fValid())
			map = SegmentMap(fnMap);
		if (map.fValid()) 
		{
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
					fldCrdMin->SetVal(cb.cMin);
					fldCrdMax->SetVal(cb.cMax);
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

int FormSegmentMapGlue::MapsCallBack(Event*)
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
	Array<SegmentMap> aMaps;
	aMaps.Resize(iMaps);
	HideCoordsAndLatLon();

	for (int i = 0; i < iMaps ; i++) {
		FileName fnMap(asMaps[i]); 
		if (fnMap.fValid())
			aMaps[i] = SegmentMap(fnMap);
		else
			return 0;
	} 
	SegmentMapGlue::OutputDomain odFinal;
	try {
		SegmentMapGlue::CheckAndFindOutputDomain(iMaps, aMaps, String("PointMapGlue"), odFinal);
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
	if (odFinal != SegmentMapGlue::odID && odFinal != SegmentMapGlue::odCLASS)
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

int FormSegmentMapGlue::CallBackClipBoundary(Event*)
{ 
	cbClip->StoreData();
	if (asMaps[0] == "") return 0;
	FileName fnMap(asMaps[0]); 
    SegmentMap map(fnMap);
	stCsy->SetVal("");
	HideCoordsAndLatLon();
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

void FormSegmentMapGlue::ShowCoords()
{

	  fldCrdMin->Show();
	  fldCrdMax->Show();
	  fldMinLat->Hide();
	  fldMaxLat->Hide();
	  fldMinLon->Hide();
	  fldMaxLon->Hide();

}

void FormSegmentMapGlue::ShowLatLons()
{
    fldMinLat->Show();
    fldMaxLat->Show();
    fldMinLon->Show();
    fldMaxLon->Show();
    fldCrdMin->Hide();
    fldCrdMax->Hide();
}

void FormSegmentMapGlue::HideCoordsAndLatLon()
{
    fldMinLat->Hide();
    fldMaxLat->Hide();
    fldMinLon->Hide();
    fldMaxLon->Hide();
    fldCrdMin->Hide();
    fldCrdMax->Hide();
}

int FormSegmentMapGlue::CallBackMinMaxXY(Event*)
{
  Coord cMin = fldCrdMin->crdVal();
  Coord cMax = fldCrdMax->crdVal();
  if (cMin.x < cMax.x && cMin.y < cMax.y)
    EnableOK();
  else
    DisableOK();  
  return 0;  
}

int FormSegmentMapGlue::CallBackMinMaxLatLon(Event*)
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

int FormSegmentMapGlue::DomCallBack(Event*)
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

int FormSegmentMapGlue::exec() 
{
  FormSegmentMapCreate::exec();
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
  if (fNewDom) {
    FileName fnDom(sNewDom);
    sNewDom = fnDom.sRelativeQuoted(false,fn.sPath());
    sMapList &= String(",%S", sNewDom);
  }
  String sExpr;
  if (fClipBoundary) {
	if (fFromLatLon)
	{
		cMin.x = llMin.Lon;
		cMax.x = llMax.Lon;
		cMin.y = llMin.Lat;
		cMax.y = llMax.Lat;
		sExpr = String("SegmentMapGlue(%7f,%7f,%7f,%7f,%S)", 
                    cMin.x, cMin.y, cMax.x, cMax.y, sMapList);
	}
	else
		sExpr = String("SegmentMapGlue(%lf,%lf,%lf,%lf,%S)", 
                    cMin.x, cMin.y, cMax.x, cMax.y, sMapList);
  }
  else                  
    sExpr = String("SegmentMapGlue(%S)", 
                    sMapList);
  execSegmentMapOut(sExpr);  
  return 0;
}

LRESULT Cmdpolseg(CWnd *wnd, const String& s)
{
	new FormSegmentMapPolBoundaries(wnd, s.scVal());
	return -1;
}

FormSegmentMapPolBoundaries::FormSegmentMapPolBoundaries(CWnd* mw, const char* sPar)
: FormSegmentMapCreate(mw, SAFTitlePolBndSegMap)
{
  if (sPar) {
    TextInput ip(sPar);
    TokenizerBase tokenizer(&ip);
    String sVal;
    for (;;) {
      Token tok = tokenizer.tokGet();
      sVal = tok.sVal();
      if (sVal == "")
        break;
      FileName fn(sVal);
      if (sPolygonMap == "" && (fn.sExt == "" || fn.sExt == ".mpa"))
        sPolygonMap = fn.sFullNameQuoted(false);
      else if (sOutMap == "" && (fn.sExt == "" || fn.sExt == ".mps"))
        sOutMap = fn.sFullName(false);
    }
  }
  new FieldPolygonMap(root, SAFUiPolMap, &sPolygonMap, new MapListerDomainType(".mpa", 0));
  sMask = "*";
  new FieldString(root, SAFUiMask, &sMask);
  fSingleName = false;
  CheckBox* cb = new CheckBox(root, SAFUiSingleName, &fSingleName);
  FieldBlank* fb = new FieldBlank(root, 0);
  fb->Align(cb, AL_UNDER);
  initSegmentMapOut(false);
  SetAppHelpTopic(htpSegmentMapPolBoundaries);
  create();
}                    

int FormSegmentMapPolBoundaries::exec() 
{
  FormSegmentMapCreate::exec();
  String sExpr;
  FileName fn(sOutMap);
  FileName fnPolygonMap(sPolygonMap); 
  sPolygonMap = fnPolygonMap.sRelativeQuoted(false,fn.sPath());
  sExpr = String("SegmentMapPolBoundaries(%S,\"%S\",%s)",
                  sPolygonMap, sMask,
                  fSingleName?"single":"unique");
  execSegmentMapOut(sExpr);  
  return 0;
}

LRESULT Cmdrasseg(CWnd *parent, const String& s) {
	new FormSegmentMapFromRaster(parent, s.scVal());
	return -1;
}

FormSegmentMapFromRaster::FormSegmentMapFromRaster(CWnd* mw, const char* sPar)
: FormSegmentMapCreate(mw, SAFTitleSegMapFromRas)
{
  iType = 0;
  fSmooth = true;
  iConnect = 1;
  if (sPar) {
    TextInput ip(sPar);
    TokenizerBase tokenizer(&ip);
    String sVal;
    for (;;) {
      Token tok = tokenizer.tokGet();
      sVal = tok.sVal();
      if (sVal == "")
        break;
      FileName fn(sVal);
      if ((fn.sExt == "" || fn.sExt == ".mpr") && sMap == "")
        sMap = fn.sFullNameQuoted(true);
      else
        sOutMap = fn.sFullName(true);
    }
  }
  new FieldDataType(root, SAFUiRasMap, &sMap,
                 new MapListerDomainType(dmCLASS|dmIDENT|dmBOOL|dmUNIQUEID), true);
  
  RadioGroup* rgConnect = new RadioGroup(root, SAFUiConnect, &iConnect);
  rgConnect->SetIndependentPos();
  RadioButton* rb4 = new RadioButton(rgConnect, SAFUi4Connected);
  rb4->Align(rgConnect, AL_AFTER);
  RadioButton* rb8 = new RadioButton(rgConnect, SAFUi8Connected);
  rb8->Align(rb4, AL_AFTER);

  new CheckBox(root, SAFUiSmoothLines, &fSmooth);
  fSingleName = false;
  new CheckBox(root, SAFUiSingleName, &fSingleName);
  
  initSegmentMapOut(false);
  SetAppHelpTopic(htpSegmentMapFromRas);
  create();
}                    

int FormSegmentMapFromRaster::exec() 
{
  FormSegmentMapCreate::exec();
  String sExpr;
  FileName fn(sOutMap);
  FileName fnMap(sMap); 
  sMap = fnMap.sRelativeQuoted(false,fn.sPath());
  String sConnect;
  switch (iConnect) {
    case 0: sConnect = "4"; break;
    case 1: sConnect = "8"; break;
  }
  sExpr = String("SegmentMapFromRasAreaBnd(%S,%S,%s,%s)",
            sMap, sConnect, fSmooth?"smooth":"nosmooth",
            fSingleName?"single":"unique");
  execSegmentMapOut(sExpr);  
  return 0;
}
