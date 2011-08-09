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
/* $Log: /ILWIS 3.0/ApplicationForms/frmtblap.cpp $
 * 
 * 24    24-10-03 16:48 Willem
 * - Changed: TableChangeDomain form now handles the default aggregation
 * selection itself instead of the field. This prevents creating
 * references to temporaries
 * 
 * 23    10-03-03 12:22 Hendrikse
 * improved range of rLagSpacing and BursaWolf scaleChange
 * 
 * 22    15-02-03 16:45 Hendrikse
 * added	fSphericalDistance = false
 * replaced  by plane andsphere
 * 
 * 21    6-02-03 21:01 Hendrikse
 * increased precision of input 7 params in constructor and exec()
 * debugged TableCallBack
 * 
 * 20    5-02-03 14:53 Hendrikse
 * added int FormTableBursaWolf::TableCallBack(Event*)
 * 
 * 19    5-02-03 13:56 Hendrikse
 * implemented class _export FormTableBursaWolf: public FormTableCreate
 * 
 * 18    7-10-02 14:18 Hendrikse
 * implemented bool fSphericalDistance for FormTableCrossVariogram and for
 * FormTableSpatCorr as Formfield and in exec()
 * 
 * 17    26-02-02 16:20 Retsios
 * Allow internal domains in FormTableChangeDomain (wish b4558)
 * 
 * 16    28-08-01 11:43 Koolhoven
 * FormTableSpatCorr has got a member pntmap to load the Pointmap in the
 * callback to prevent too often constructing one
 * 
 * 15    23/02/01 9:48 Willem
 * The SpatCorr() form callback now does not check the attribute table
 * anymore. This is not necessary, because attribute maps are handled via
 * MapAttribute
 * 
 * 14    22-01-01 19:13 Koolhoven
 *  FormTableAutoCorr now uses a spinner for the max pix shift
 * 
 * 13    23-11-00 11:48 Koolhoven
 * HistogramSegmentDir now asks for a SegMap instaead of a Map
 * 
 * 12    8-11-00 17:25 Koolhoven
 * use sFullNameQuoted() on input data to ensure selection in forms
 * 
 * 11    21-09-00 10:20a Martin
 * added some attrib columns to the mapcreate fields
 * 
 * 10    30-08-00 12:35p Martin
 * use attribcol nameeidt in spatcor
 * 
 * 9     21-07-00 15:40 Koolhoven
 * removed checkbox for Show of Histograms
 * 
 * 8     18-07-00 11:54a Martin
 * extensions are now correct so a icon will show in the map nameeedit
 * 
 * 7     28-02-00 11:56 Wind
 * adapted to changes in constructor of FieldColumn
 * 
 * 6     14-01-00 18:11 Koolhoven
 * protection against stupid errors
 * 
 * 5     18-11-99 11:02a Martin
 * changed Tokenizer construction to avoid problems with pure virtual
 * functions
 * 
 * 4     9/13/99 12:35p Wind
 * adapted :exec functions to use of quoted file names when building an
 * expression
 * 
 * 3     9-09-99 12:41p Martin
 * //->/*
 * 
 * 2     8-09-99 12:30p Martin
 * Added 2.22 stuff
// Revision 1.3  1998/09/16 17:33:54  Wim
// 22beta2
//
// Revision 1.2  1997/08/13 07:55:23  Wim
// AutoCorr only allow value, image bool maps
//
/* Form Table Applications
   by Wim Koolhoven
   (c) Ilwis System Development ITC
	Last change:  WK   24 Aug 98    4:37 pm
*/
#include "Client\Headers\AppFormsPCH.h"
#include "Headers\Hs\Coordsys.hs"
#include "Headers\Hs\Appforms.hs"
#include "ApplicationsUI\frmtblap.h"
#include "Applications\Table\AUTCSEMV.H"
#include "Client\FormElements\fentvalr.h"
#include "Engine\Table\tblinfo.h"
#include "Engine\Domain\dmcoord.h"
#include "Engine\Table\COLINFO.H"
#include "Client\FormElements\fldaggr.h"
#include "Client\FormElements\fentmisc.h"

LRESULT Cmdhistogram(CWnd *wnd, const String& s)
{
	new FormTableHistogram(wnd, s.c_str());
	return -1;
}

FormTableHistogram::FormTableHistogram(CWnd* mw, const char* sPar)
: FormTableCreate(mw, TR("Calculate Histogram"))
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
      if (fn.sExt == "")
        fn.sExt = ".mpr";
      if (sMap == "" && fn.sExt == ".mpr" || fn.sExt == ".mpa" ||
          fn.sExt == ".mpp" || fn.sExt == ".mps")
        sMap = fn.sFullNameQuoted(true);
    }
  }
  new FieldBaseMap(root, TR("&Map"), &sMap);
  fShow = true;
//  new CheckBox(root, TR("&Show"), &fShow);
  SetHelpItem("ilwisapp\\histogram_dialogbox.htm");
  create();
}                    

int FormTableHistogram::exec() 
{
  FormTableCreate::exec();
  String sExpr;
  FileName fnMap(sMap); 
  sMap = fnMap.sShortNameQuoted(false);
  sOutTable = sMap;
  String sApplic;
  String sType;
  if (fnMap.sExt == ".mpr") {
    sApplic = "TableHistogram";
    sOutTable &= ".his";
  }  
  else if (fnMap.sExt == ".mpa") { 
    sApplic = "TableHistogramPol";
    sType = "Polygon ";
    sOutTable &= ".hsa";
  }  
  else if (fnMap.sExt == ".mps") { 
    sApplic = "TableHistogramSeg";
    sType = "Segment ";
    sOutTable &= ".hss";
  }  
  else if (fnMap.sExt == ".mpp") { 
    sApplic = "TableHistogramPnt";
    sType = "Point ";
    sOutTable &= ".hsp";
  }  
  sExpr = String("%S(%S)", sApplic, sMap);
  sDescr = String("%SHistogram of %S", sType, sMap);
  execTableOut(sExpr);  
  return 0;
}

LRESULT Cmdautocorr(CWnd *wnd, const String& s)
{
	new FormTableAutoCorr(wnd, s.c_str());
	return -1;
}

FormTableAutoCorr::FormTableAutoCorr(CWnd* mw, const char* sPar)
: FormTableCreate(mw, TR("AutoCorrelation and SemiVariogram"))
{
  iPixShift = 100;
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
        sMap = fn.sFullNameQuoted(true);
      else  if (sOutTable == "" && fn.sExt == "" || fn.sExt == ".tbt")
        sOutTable = fn.sFullName(true);
    }
  }
  fldMap = new FieldDataType(root, TR("&Raster Map"), &sMap,
                    new MapListerDomainType(dmVALUE|dmIMAGE|dmBOOL), true);
  fldMap->SetCallBack((NotifyProc)&FormTableAutoCorr::CallBack);
  fiMaxShift = new FieldInt(root, TR("Maximum Pixel &Shift"), &iPixShift, ValueRange(1,8000), true);
  initTableOut(false);
  SetHelpItem("ilwisapp\\auto_correlation_semivariance_dialog_box.htm");
  create();
}                    

int FormTableAutoCorr::exec() 
{
  FormTableCreate::exec();
  String sExpr;
  FileName fn(sOutTable);
  FileName fnMap(sMap); 
  sMap = fnMap.sRelativeQuoted(false,fn.sPath());
  sExpr = String("TableAutoCorrSemiVar(%S,%i)", sMap, iPixShift);
  execTableOut(sExpr);  
  return 0;
}

int FormTableAutoCorr::CallBack(Event*)
{
  fldMap->StoreData();
  if (sMap[0]) {
    try {
      FileName fnMap(sMap); 
      Map map(fnMap);
      iPixShift = TableAutoCorrSemiVar::iMaxShiftDefault(map);
      fiMaxShift->SetVal(iPixShift);
    }
    catch (...) {
    }
  }
  return 0;
}

LRESULT Cmdspatcorr(CWnd *wnd, const String& s)
{
	new FormTableSpatCorr(wnd, s.c_str());
	return -1;
}

FormTableSpatCorr::FormTableSpatCorr(CWnd* mw, const char* sPar)
: FormTableCreate(mw, TR("Spatial Correlation"))
{
  rLagLength = rUNDEF; //1000;
  iOmniBi = 0;
  rDir = 0;
  rTol = 45;
  fBandWidth = false;
  rBandWidth = 1e6;
	fSphericalDistance = false;
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
      if (sMap == "" && fn.sExt == "" || fn.sExt == ".mpp")
        sMap = fn.sFullNameQuoted(false);
      else  if (sOutTable == "" && fn.sExt == "" || fn.sExt == ".tbt")
        sOutTable = fn.sFullName(false);
    }
  }
	fldMap = new FieldPointMap(root, TR("&Point Map"), &sMap, new MapListerDomainType(".mpp", dmVALUE, true));
  fldMap->SetCallBack((NotifyProc)&FormTableSpatCorr::CallBack);

  RadioGroup* rg = new RadioGroup(root, "", &iOmniBi);
  RadioButton* rbOmniDir = new RadioButton(rg, TR("&Omnidirectional"));
  RadioButton* rbBiDir = new RadioButton(rg, TR("&Bidirectional"));

  FieldReal* frLag = new FieldReal(root, TR("&Lag spacing (m)"), &rLagLength, ValueRange(1e-5,1e9,1e-5));

	CheckBox* cbSphDist = new CheckBox(rbOmniDir, TR("Use &Spherical Distance"), &fSphericalDistance);
	cbSphDist->Align(frLag, AL_UNDER);
	cbSphDist->SetIndependentPos();
	
	FieldGroup* fg = new FieldGroup(rbBiDir);
  fg->Align(frLag, AL_UNDER);
  new FieldReal(fg, TR("&Direction (deg)"), &rDir, ValueRange(0,90,0.1));
  new FieldReal(fg, TR("&Tolerance (deg)"), &rTol, ValueRange(0,45,0.1));
  CheckBox* cbBW = new CheckBox(fg, TR("&Band width (m)"), &fBandWidth);
  new FieldReal(cbBW, "", &rBandWidth, ValueRange(0.1,1e9,0.1));
	
  FieldBlank* fb = new FieldBlank(root, 0);
  fb->Align(cbBW, AL_UNDER);
  initTableOut(false);
  SetHelpItem("ilwisapp\\spatial_correlation_dialogbox.htm");
  create();
}                    

int FormTableSpatCorr::exec() 
{
  FormTableCreate::exec();
  String sExpr;
  FileName fn(sOutTable);
  FileName fnMap(sMap); 
  PointMap map(fnMap);
  sMap = fnMap.sRelativeQuoted(false,fn.sPath());	
  if (0 == map->dm()->pdv())
    sMap = String("%S.%S", sMap, sCol);
	String sDistMeth = String("%s", fSphericalDistance ? "sphere" : "plane");
  if (iOmniBi)
    if (fBandWidth)
      sExpr = String("TableSpatCorr(%S,%.2f,%.1f,%.1f,%.1f)",
                     sMap, rLagLength,
                     rDir, rTol, rBandWidth);
    else
      sExpr = String("TableSpatCorr(%S,%.2f,%.1f,%.1f)",
                     sMap, rLagLength,
                     rDir, rTol);
  else
    sExpr = String("TableSpatCorr(%S,%.5f,%S)", sMap, rLagLength, sDistMeth);
  execTableOut(sExpr);
  return 0;
}

int FormTableSpatCorr::CallBack(Event*)
{
	fldMap->StoreData();
	if (sMap.length() > 0)
	{
		try
		{
			// Try to open the Map or AttributeMap
			// Only value maps are valid
			FileName fnMap(sMap); 
      pntmap = PointMap(fnMap);
			if (pntmap->dm()->pdv())
			{
				EnableOK();
				return 0;
			}
		}
		catch (...)
		{
		}
	}
	DisableOK();
	return 0;
}

LRESULT Cmdpattanal(CWnd *wnd, const String& s)
{
	new FormTablePattAnal(wnd, s.c_str());
	return -1;
}

FormTablePattAnal::FormTablePattAnal(CWnd* mw, const char* sPar)
: FormTableCreate(mw, TR("Pattern Analysis"))
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
      if (sMap == "" && fn.sExt == "" || fn.sExt == ".mpp")
        sMap = fn.sFullNameQuoted(false);
      else  if (sOutTable == "" && fn.sExt == "" || fn.sExt == ".tbt")
        sOutTable = fn.sFullName(false);
    }
  }
  new FieldPointMap(root, TR("&Point Map"), &sMap);
  initTableOut(false);
  SetHelpItem("ilwisapp\\pattern_analysis_dialog_box.htm");
  create();
}                    

int FormTablePattAnal::exec() 
{
  FormTableCreate::exec();
  String sExpr;
  FileName fn(sOutTable);
  FileName fnMap(sMap); 
  sMap = fnMap.sRelativeQuoted(false,fn.sPath());
  sExpr = String("TablePattAnal(%S)", sMap);
  execTableOut(sExpr);  
  return 0;
}

LRESULT Cmdhistnbpol(CWnd *wnd, const String& s)
{
	new FormTableNeighbourPol(wnd, s.c_str());
	return -1;
}

FormTableNeighbourPol::FormTableNeighbourPol(CWnd* mw, const char* sPar)
: FormTableCreate(mw, TR("Neighbouring Polygons"))
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
      if (sMap == "" && fn.sExt == "" || fn.sExt == ".mpa")
        sMap = fn.sFullNameQuoted(false);
      else  if (sOutTable == "" && fn.sExt == "" || fn.sExt == ".tbt")
        sOutTable = fn.sFullName(false);
    }
  }
  new FieldPolygonMap(root, TR("&Polygon Map"), &sMap);
  initTableOut(false);
  SetHelpItem("ilwisapp\\neighbour_polygons_dialog_box.htm");
  create();
}                    

int FormTableNeighbourPol::exec() 
{
  FormTableCreate::exec();
  String sExpr;
  FileName fn(sOutTable);
  FileName fnMap(sMap); 
  sMap = fnMap.sRelativeQuoted(false,fn.sPath());
  sExpr = String("TableNeighbourPol(%S)", sMap);
  execTableOut(sExpr);  
  return 0;
}

FormTableLinAnal::FormTableLinAnal(CWnd* mw, const char* sPar)
: FormTableCreate(mw, TR("Segment Direction Histogram"))
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
      else  if (sOutTable == "" && fn.sExt == "" || fn.sExt == ".tbt")
        sOutTable = fn.sFullName(false);
    }
  }
  new FieldSegmentMap(root, TR("&Segment Map"), &sMap);
  initTableOut(false);
  SetHelpItem("ilwisapp\\segment_direction_histogram_dialog_box.htm");
  create();
}                    

LRESULT Cmdhistsegdir(CWnd *wnd, const String& s)
{
	new FormTableLinAnal(wnd, s.c_str());
	return -1;
}

int FormTableLinAnal::exec() 
{
  FormTableCreate::exec();
  String sExpr;
  FileName fn(sOutTable);
  FileName fnMap(sMap); 
  sMap = fnMap.sRelativeQuoted(false,fn.sPath());
  sExpr = String("TableSegDir(%S)", sMap);
  execTableOut(sExpr);  
  return 0;
}

LRESULT Cmdcross(CWnd *wnd, const String& s)
{
	new FormMapCross(wnd, s.c_str());
	return -1;
}

FormMapCross::FormMapCross(CWnd* mw, const char* sPar)
: FormTableCreate(mw, TR("Cross"))
{
  fMap = false;
  fIgnoreUndef1 = true;
  fIgnoreUndef2 = true;
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
      if ((fn.sExt == "" || fn.sExt == ".mpr") && sMap1 == "")
        sMap1 = fn.sFullNameQuoted(false);
      else if ((fn.sExt == "" || fn.sExt == ".mpr") && sMap2 == "")
        sMap2 = fn.sFullNameQuoted(false);
      else if ((fn.sExt == "" || fn.sExt == ".tbt") && sOutTable == "")
        sOutTable = fn.sFullNameQuoted(false);
      else if ((fn.sExt == "" || fn.sExt == ".mpr") && sOutMap == "") {
        sOutMap = fn.sFullNameQuoted(false);
        fMap = true;
      }  
    }
  }
  new FieldMap(root, TR("&1st Map"), &sMap1, new MapListerDomainType(".mpr", 0, true));
  CheckBox* cb = new CheckBox(root, TR("&Ignore Undefs"), &fIgnoreUndef1);
  cb->SetIndependentPos();
  FieldMap* fm = new FieldMap(root, TR("&2nd Map"), &sMap2, new MapListerDomainType(".mpr", 0, true));
  fm->Align(cb, AL_UNDER);
  cb = new CheckBox(root, TR("&Ignore Undefs"), &fIgnoreUndef2);
  cb->SetIndependentPos();
  FieldBlank* fb = new FieldBlank(root, 0);
  fb->Align(cb, AL_UNDER);
  initTableOut(false);
  CheckBox* cbMap = new CheckBox(root, TR("&Output Map"), &fMap);
  new FieldMapCreate(cbMap, "", &sOutMap);
  SetHelpItem("ilwisapp\\cross_dialogbox.htm");
  create();
}                    

int FormMapCross::exec() 
{
  FormTableCreate::exec();
  String sExpr;
  FileName fn(sOutTable);
  FileName fnMap1(sMap1); 
  FileName fnMap2(sMap2); 
  sMap1 = fnMap1.sRelativeQuoted(false,fn.sPath());
  sMap2 = fnMap2.sRelativeQuoted(false,fn.sPath());
  String sIgnore;
  if (fMap) {
    FileName fnOutMap(sOutMap); 
    sOutMap = String(",%S", fnOutMap.sRelativeQuoted(false,fn.sPath()));
  }
  if (fIgnoreUndef1)
    if (fIgnoreUndef2)
      sIgnore = ",IgnoreUndefs";
    else
      sIgnore = ",IgnoreUndef1";
  else
    if (fIgnoreUndef2)
      sIgnore = ",IgnoreUndef2";
  sExpr = String("TableCross(%S,%S%S%S)",
    sMap1, sMap2, sOutMap, sIgnore);
  execTableOut(sExpr);  
  return 0;
}

LRESULT Cmdtranspose(CWnd *wnd, const String& s)
{
	new FormTableTranspose(wnd, s.c_str());
	return -1;
}

FormTableTranspose::FormTableTranspose(CWnd* mw, const char* sPar)
: FormTableCreate(mw, TR("Transpose Table"))
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
      if (sTable == "" && fn.sExt == "" || fn.sExt == ".tbt")
        sTable = fn.sFullNameQuoted(false);
      else if (sDomain == "" && fn.sExt == "" || fn.sExt == ".dom")
        sDomain = fn.sFullNameQuoted(false);
      else  if (sOutTable == "" && fn.sExt == "" || fn.sExt == ".tbt")
        sOutTable = fn.sFullName(false);
    }
  }
  ftbl = new FieldTable(root, TR("&Table"), &sTable);
  ftbl->SetCallBack((NotifyProc)&FormTableTranspose::CallBack);
  fdom = new FieldDomain(root, TR("&Domain for Columns"), &sDomain);
  FieldValueRange* fvr = new FieldValueRange(root, TR("Value &Range"), &vr, fdom);
  fdom->SetCallBack((NotifyProc)&FieldValueRange::DomainCallBack, fvr);
  fvr->Align(fdom, AL_UNDER);  
  initTableOut(false);
  SetHelpItem("ilwisapp\\transpose_table_dialog_box.htm");
  create();
}                    

int FormTableTranspose::exec() 
{
  FormTableCreate::exec();
  String sExpr;
  FileName fn(sOutTable);
  FileName fnTable(sTable); 
  sTable = fnTable.sRelativeQuoted(false,fn.sPath());
  FileName fnDomain(sDomain, ".dom", false);
  String sValRange;
  try {
    Domain dm(fnDomain);
    DomainValueRangeStruct dvs(dm, vr);
    if (dvs.vr().fValid())
      sValRange = dvs.vr()->sRange();
  }
  catch (ErrorObject& err) {
    err.Show();
  }
  sDomain = fnDomain.sRelativeQuoted(false,fn.sPath());
  if (sValRange.length() != 0)
    sExpr = String("TableTranspose(%S,%S,%S)", sTable, sDomain, sValRange);
  else
    sExpr = String("TableTranspose(%S,%S)", sTable, sDomain);
  execTableOut(sExpr);  
  return 0;
}

int FormTableTranspose::CallBack(Event*)
{
  ftbl->StoreData();
  if (sTable[0]) {
    try {
      TableInfo tblinf(sTable);
			if (tblinf.iCols() <= 0)
				return 0;
      ColumnInfo colinf = tblinf.aci[0];
      Domain dm;
      String sDom;
      if (colinf.fValid())
        dm = colinf.dm();
      if (dm.fValid())
        sDom = dm->fnObj.sFile;
      fdom->SetVal(sDom);    
    }
    catch (...) {
    }
  }
  return 0;
}

LRESULT Cmdtblchdom(CWnd *wnd, const String& s)
{
	new FormTableChangeDomain(wnd, s.c_str());
	return -1;
}

FormTableChangeDomain::FormTableChangeDomain(CWnd* mw, const char* sPar)
: FormTableCreate(mw, TR("Change Domain of Table"))
{
	iColDom = 0;
	sDomain = "none";
	fAggregate = false;
	sFunc = 0;
	m_sDefault = String();
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
			if (sTable == "" && fn.sExt == "" || fn.sExt == ".tbt")
				sTable = fn.sFullNameQuoted(false);
			else if (sCol == "" && 0 == iColDom && fn.sExt == "") {
				sCol = sVal;
			}
			else if (0 == iColDom && sCol == "" && fn.sExt == ".dom") {
				iColDom = 1;
				sDomain = fn.sFullNameQuoted(false);
			}
			else  if (sOutTable == "" && fn.sExt == "" || fn.sExt == ".tbt")
				sOutTable = fn.sFullName(false);
		}
	}
	fldTbl = new FieldTable(root, TR("&Table"), &sTable);
	fldTbl->SetCallBack((NotifyProc)&FormTableChangeDomain::TblCallBack);
	StaticText* st = new StaticText(root, TR("Change Domain To:"));
	st->SetIndependentPos();
	RadioGroup* rg = new RadioGroup(root, "", &iColDom);
	RadioButton* rbCol = new RadioButton(rg, TR("&Column"));
	RadioButton* rbDom = new RadioButton(rg, TR("&Domain"));
	new FieldDomainC(rbDom, "", &sDomain, dmCLASS|dmIDENT|dmNONE, true); // Use DomainAllExtLister
	
	FieldGroup* fgCol = new FieldGroup(rbCol);
	fldCol = new FieldColumn(fgCol, "", Table(), &sCol,
		dmCLASS|dmIDENT);
	fldCol->Align(rbCol, AL_AFTER);
	CheckBox* cb = new CheckBox(fgCol, TR("&Aggregate"), &fAggregate);
	cb->Align(rbDom, AL_UNDER);
	new FieldAggrLastFunc(cb, "", &sFunc, m_sDefault);
	
	FieldBlank* fb = new FieldBlank(root, 0);
	fb->Align(cb, AL_UNDER);
	
	initTableOut(false);
	SetHelpItem("ilwisapp\\change_domain_of_table_dialog_box.htm");
	create();
}                    

int FormTableChangeDomain::exec()
{
  FormTableCreate::exec();
  String sExpr;
  FileName fn(sOutTable);
  FileName fnTable(sTable); 
  sTable = fnTable.sRelativeQuoted(false,fn.sPath());

  if (0 == iColDom) {
    if (fAggregate)
      sExpr = String("TableChangeDomain(%S,%S,%S)", sTable, sCol, *sFunc);
    else
      sExpr = String("TableChangeDomain(%S,%S)", sTable, sCol);
  }
  else {
    FileName fnDomain(sDomain, ".dom", false);
    try {
      Domain dm(fnDomain);
    }
    catch (ErrorObject& err) {
      err.Show();
    }
    sDomain = fnDomain.sRelativeQuoted(fnDomain.sExt != ".dom", fn.sPath()); // include the extension if it is an internal domain
    sExpr = String("TableChangeDomain(%S,%S)", sTable, sDomain);
  }
  execTableOut(sExpr);
  return 0;
}

int FormTableChangeDomain::TblCallBack(Event*)
{
  fldTbl->StoreData();
  fldCol->FillWithColumns((TablePtr*)0);
  try {
    FileName fnTbl(sTable);
    fldCol->FillWithColumns(fnTbl);
//    ColCallBack(0);
  }
  catch (ErrorObject&) {}
  return 0;
}

LRESULT Cmdtblglue(CWnd *wnd, const String& s)
{
	new FormTableGlue(wnd, s.c_str());
	return -1;
}

FormTableGlue::FormTableGlue(CWnd* mw, const char* sPar)
: FormTableCreate(mw, TR("Glue Tables"))
{
  iTbls = 2;
  fVertical = false;
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
      if (fn.sExt == "" || fn.sExt == ".tbt")
        if (sTbl1 == "")
          sTbl1 = fn.sFullNameQuoted(false);
        else if (sTbl2 == "")
          sTbl2 = fn.sFullNameQuoted(false);
        else if (sTbl3 == "") {
          iTbls = 3;
          sTbl3 = fn.sFullNameQuoted(false);
        }
        else if (sTbl4 == "") {
          iTbls = 4;
          sTbl4 = fn.sFullNameQuoted(false);
        }
        else  
          sOutTable = fn.sFullName(false);
    }
  }
  iTbls -= 2;
  StaticText* st = new StaticText(root, TR("&Number of Input Tables"));
  st->SetIndependentPos();
  rgTbls = new RadioGroup(root, "", &iTbls, true);
  rgTbls->SetIndependentPos();
  rgTbls->SetCallBack((NotifyProc)&FormTableGlue::TblsCallBack);
  new RadioButton(rgTbls, "&2");
  new RadioButton(rgTbls, "&3");
  new RadioButton(rgTbls, "&4");

  fldTbl1 = new FieldTable(root, TR("&1st Table"), &sTbl1);
  fldTbl1->SetCallBack((NotifyProc)&FormTableGlue::TblCallBack);
  fldTbl2 = new FieldTable(root, TR("&2nd Table"), &sTbl2);
  fldTbl2->SetCallBack((NotifyProc)&FormTableGlue::TblCallBack);
  fldTbl3 = new FieldTable(root, TR("&3rd Table"), &sTbl3);
  fldTbl3->SetCallBack((NotifyProc)&FormTableGlue::TblCallBack);
  fldTbl4 = new FieldTable(root, TR("&4th Table"), &sTbl4);
  fldTbl4->SetCallBack((NotifyProc)&FormTableGlue::TblCallBack);

  initTableOut(false);

  FieldBlank* fb = new FieldBlank(root, 0);
  cbVertical = new CheckBox(root, TR("&Vertical"), &fVertical);
  cbVertical->SetIndependentPos();
  stRemark = new StaticText(root, TR("Incompatible Domains"));
  stRemark->Align(fb, AL_UNDER);
  stRemark->SetIndependentPos();
  SetHelpItem("ilwisapp\\glue_tables_dialog_box.htm");
  create();
}                    

int FormTableGlue::TblsCallBack(Event*)
{
  rgTbls->StoreData();
  iTbls += 2;
  switch (iTbls) {  // lots of fall throughs
    case 2:
      fldTbl3->Hide();
    case 3:
      fldTbl4->Hide();
  }
  switch (iTbls) {
    case 4:
      fldTbl4->Show();
    case 3:
      fldTbl3->Show();
    case 2:
      fldTbl2->Show();
    case 1:
      fldTbl1->Show();
  }
  return 0;
}

int FormTableGlue::TblCallBack(Event*)
{
  rgTbls->StoreData();
  iTbls += 2;
  switch (iTbls) {  // lots of fall throughs
    case 4:
      fldTbl4->StoreData();
    case 3:
      fldTbl3->StoreData();
    case 2:
      fldTbl2->StoreData();
    case 1:
      fldTbl1->StoreData();
  }
  try {
    bool fDomOK = true;
    bool fDomNone;
    if (fDomOK) {
      if (sTbl1 == "") {
        stRemark->Hide();
        cbVertical->Hide();
        DisableOK();
      }
      Table tbl1(sTbl1);
      if (tbl1->dm()->pdnone())
        fDomNone = true;
      else if (tbl1->dm()->pdsrt())
        fDomNone = false;
      else
        fDomOK = false;
    }
    if (fDomOK) {
      if (sTbl2 == "") {
        stRemark->Hide();
        cbVertical->Hide();
        DisableOK();
      }
      Table tbl2(sTbl2);
      if (tbl2->dm()->pdnone())
        fDomOK = fDomNone;
      else if (tbl2->dm()->pdsrt())
        fDomOK = !fDomNone;
      else
        fDomOK = false;
    }
    if (fDomOK && iTbls > 2) {
      if (sTbl3 == "") {
        stRemark->Hide();
        cbVertical->Hide();
        DisableOK();
      }
      Table tbl3(sTbl3);
      if (tbl3->dm()->pdnone())
        fDomOK = fDomNone;
      else if (tbl3->dm()->pdsrt())
        fDomOK = !fDomNone;
      else
        fDomOK = false;
    }
    if (fDomOK && iTbls > 3) {
      if (sTbl4 == "") {
        stRemark->Hide();
        cbVertical->Hide();
        DisableOK();
      }
      Table tbl4(sTbl4);
      if (tbl4->dm()->pdnone())
        fDomOK = fDomNone;
      else if (tbl4->dm()->pdsrt())
        fDomOK = !fDomNone;
      else
        fDomOK = false;
    }
    if (fDomOK) {
      stRemark->Hide();
      if (fDomNone)
        cbVertical->Show();
      else
        cbVertical->Hide();
      EnableOK();
    }
    else {
      stRemark->Show();
      cbVertical->Hide();
      DisableOK();
    }
  }
  catch (ErrorObject&) {
    stRemark->Hide();
    cbVertical->Hide();
    DisableOK();
  }
  return 0;
}

int FormTableGlue::exec()
{
  FormTableCreate::exec();
  iTbls += 2;
  String sExpr;
  FileName fn(sOutTable);
  FileName fnTbl1(sTbl1);
  FileName fnTbl2(sTbl2);
  FileName fnTbl3(sTbl3);
  FileName fnTbl4(sTbl4);
  sTbl1 = fnTbl1.sRelativeQuoted(false,fn.sPath());
  sTbl2 = fnTbl2.sRelativeQuoted(false,fn.sPath());
  sTbl3 = fnTbl3.sRelativeQuoted(false,fn.sPath());
  sTbl4 = fnTbl4.sRelativeQuoted(false,fn.sPath());
  String sEnd;
  switch (iTbls) {
    case 2:
      sExpr = String("TableGlue(%S,%S",sTbl1,sTbl2);
      break;
    case 3:
      sExpr = String("TableGlue(%S,%S,%S",sTbl1,sTbl2,sTbl3);
      break;
    case 4:
      sExpr = String("TableGlue(%S,%S,%S,%S",sTbl1,sTbl2,sTbl3,sTbl4);
      break;
  }
  if (fVertical)
    sExpr &= ",vertical)";
  else
    sExpr &= ')';
  execTableOut(sExpr);
  return 0;
}

LRESULT Cmdcrossvariogram(CWnd *wnd, const String& s)
{
	new FormTableCrossVariogram(wnd, s.c_str());
	return -1;
}

FormTableCrossVariogram::FormTableCrossVariogram(CWnd* mw, const char* sPar)
: FormTableCreate(mw, TR("Cross Variogram"))
{
  rLagSpacing = rUNDEF;
	fSphericalDistance = false;
  if (sPar) 
	{
		TextInput ip(sPar);
    TokenizerBase tokenizer(&ip);
    String sVal;
    for (;;) {
      Token tok = tokenizer.tokGet();
      sVal = tok.sVal();
      if (sVal == "")
        break;
      FileName fn(sVal);
      if (sMap == "" && fn.sExt == "" || fn.sExt == ".mpp")
        sMap = fn.sFullNameQuoted(false);
      else  if (sOutTable == "" && fn.sExt == "" || fn.sExt == ".tbt")
        sOutTable = fn.sFullName(false);
    }
  }
  fldMap = new FieldPointMap(root, TR("&Point Map"), &sMap);
  fldMap->SetCallBack((NotifyProc)&FormTableCrossVariogram::MapCallBack);
  fldColA = new FieldColumn(root, TR("&Column A"), Table(), &sColA, dmVALUE|dmBOOL);
  fldColB = new FieldColumn(root, TR("&Column B"), Table(), &sColB, dmVALUE|dmBOOL);
  new FieldReal(root, TR("&Lag spacing (m)"), &rLagSpacing, ValueRange(1e-5,1e9,1e-5));
	CheckBox* cbSphDist = new CheckBox(root, TR("Use &Spherical Distance"), &fSphericalDistance);
	cbSphDist->SetIndependentPos();
  initTableOut(false);
  SetHelpItem("ilwisapp\\cross_variogram_dialog_box.htm");
  create();
}                    

int FormTableCrossVariogram::exec() 
{
  FormTableCreate::exec();
  String sExpr;
  FileName fn(sOutTable);
  FileName fnMap(sMap); 
  sMap = fnMap.sRelativeQuoted(false,fn.sPath());
	String sDistMeth = String("%s", fSphericalDistance ? "sphere" : "plane");
  sExpr = String("TableCrossVariogram(%S,%S,%S,%.5f,%S)", sMap, sColA, sColB, rLagSpacing, sDistMeth);
  execTableOut(sExpr);
  return 0;
}

int FormTableCrossVariogram::MapCallBack(Event*)
{
  fldMap->StoreData();
  if (sMap[0]) {
    try {
      FileName fnMap(sMap); 
      PointMap map(fnMap);
      if (!map.fValid())
        return 0;
      if (!map->fTblAtt()) {
        fldColA->FillWithColumns((TablePtr*)0);
        fldColB->FillWithColumns((TablePtr*)0);
      }
      else {
        Table tbl = map->tblAtt();
        fldColA->FillWithColumns(&tbl);
        fldColB->FillWithColumns(&tbl);
      }
    }
    catch (ErrorObject&) {
    }
  }
  return 0;
}

LRESULT Cmdbursawolf(CWnd *wnd, const String& s)
{
	new FormTableBursaWolf(wnd, s.c_str());
	return -1;
}

FormTableBursaWolf::FormTableBursaWolf(CWnd* mw, const char* sPar)
: FormTableCreate(mw, TR("Bursa-Wolf Datum Transformation"))
{
  if (sPar) 
	{
		TextInput ip(sPar);
    TokenizerBase tokenizer(&ip);
    String sVal;
    for (;;) {
      Token tok = tokenizer.tokGet();
      sVal = tok.sVal();
      if (sVal == "")
        break;
      FileName fn(sVal);
      if (sTbl == "" && fn.sExt == "" || fn.sExt == ".tbt")
        sTbl = fn.sFullNameQuoted(false);
      else  if (sOutTable == "" && fn.sExt == "" || fn.sExt == ".tbt")
        sOutTable = fn.sFullName(false);
    }
  }
  fldTblIn = new FieldTable(root, TR("&Table"), &sTbl);
  fldTblIn->SetCallBack((NotifyProc)&FormTableBursaWolf::TableCallBack);
  fldColCrds = new FieldColumn(root, TR("Input &Coord Column"), Table(), &sColCrds, dmCOORD);
  fldColHeights = new FieldColumn(root, TR("Input &Heights Column"), Table(), &sColHeights, dmVALUE);
	FieldGroup * fgLeft = new FieldGroup(root);
	fgLeft->SetIndependentPos();
	FieldGroup * fgRight = new FieldGroup(root);
	fgRight->Align(fgLeft, AL_AFTER);
	StaticText* stShifts = new StaticText(fgLeft, TR("Shifts: (m)"));
  //Shifts entered in meters
  FieldReal* flddX = new FieldReal(fgLeft, TR("D&x"), &m_rX, ValueRangeReal(-1e4, 1e4, 0.001));
  FieldReal* flddY = new FieldReal(fgLeft, TR("D&y"), &m_rY, ValueRangeReal(-1e4, 1e4, 0.001));
  FieldReal* flddZ = new FieldReal(fgLeft, TR("D&z"), &m_rZ, ValueRangeReal(-1e4, 1e4, 0.001));
	//rot-angles entered by user in arcsecs
	StaticText* stRotations = new StaticText(fgRight, TR("Rotations: (arcsec)"));
	FieldReal* fldrX = new FieldReal(fgRight, TR("Rx = &alpha"), &m_rXrot, ValueRangeReal(-1e3, 1e3, 0.000001));
  FieldReal* fldrY = new FieldReal(fgRight, TR("Ry = &beta"), &m_rYrot, ValueRangeReal(-1e3, 1e3, 0.000001));
  FieldReal* fldrZ = new FieldReal(fgRight, TR("Rz = &gamma"), &m_rZrot, ValueRangeReal(-1e3, 1e3, 0.000001));
	//scale diff entered by user in ppm (parts per million)
	FieldReal* fldScale = new FieldReal(root, TR("&Scale difference (ppm)"), &m_rScaleDiff, ValueRangeReal(-1e3, 1e3, 0.00001));
	fldScale->Align(fgLeft, AL_UNDER);
  fcs = new FieldCoordSystem(root, TR("&Target Coordinate System"), &m_sCsyOut);
  fcs->SetCallBack((NotifyProc)&FormTableBursaWolf::CallBackCsyName);
  initTableOut(false);
  SetHelpItem("ilwisapp\\cross_variogram_dialog_box.htm");
  create();
}                    

int FormTableBursaWolf::exec() 
{
  FormTableCreate::exec();
  String sExpr;
  FileName fn(sOutTable);
  FileName fnTbl(sTbl); 
  sTbl = fnTbl.sRelativeQuoted(false,fn.sPath());
  sExpr = String("TableBursaWolf(%S,%S,%S,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%S)", 
					sTbl, sColCrds, sColHeights, m_rX,  m_rY,  m_rZ,
					m_rXrot, m_rYrot, m_rZrot, m_rScaleDiff, m_sCsyOut);
  execTableOut(sExpr);
  return 0;
}

int FormTableBursaWolf::TableCallBack(Event*)
{
  fldTblIn->StoreData();
  if (sTbl[0]) {
    try {
      FileName fnTable(sTbl); 
      Table tbl(fnTable);
      if (!tbl.fValid())
        return 0;
      fldColCrds->FillWithColumns(tbl.ptr());
      fldColHeights->FillWithColumns(tbl.ptr());
    }
    catch (ErrorObject&) {
    }
  }
  return 0;
}

int FormTableBursaWolf::CallBackCsyName(Event*)
{
  fcs->StoreData();
  if (m_sCsyOut[0]) {
    try {
      FileName fnCoordSys(m_sCsyOut); 
      CoordSystem cs(fnCoordSys);
      if (!cs.fValid())
        return 0;
      if(!cs->fCoord2LatLon())
		  return 0;
    }
    catch (ErrorObject&) {
    }
  }
  return 0;
}


