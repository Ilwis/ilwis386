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
// $Log: /ILWIS 3.0/ApplicationForms/frmmapsb.cpp $
 * 
 * 30    17-10-02 17:59 Hendrikse
 * debugged SetCallBack((NotifyProc)&FormResampleMap::MethodCallBack);
 * in FormMapResample constructor
 * 
 * 29    14-10-02 18:36 Koolhoven
 * FormResampleMap: added option for Color maps using bilinear
 * interpolation
 * 
 * 28    6-08-02 14:09 Koolhoven
 * FormDensifyMap now allows any domain type as input map
 * 
 * 27    5-08-02 15:43 Koolhoven
 * FormResampleMap allow any type of domain, just as in 3.0 and before
 * 
 * 26    8-07-02 14:01 Koolhoven
 * FormMapResample now also allows PICTURE maps as input (bug 6167)
 * 
 * 25    19-11-01 14:41 Koolhoven
 * FormMapSubMap constructor check on eventual extra parameters improved
 * (no GPF anymore)
 * 
 * 24    24-10-01 18:52 Koolhoven
 * FormMapMirrorRotate now fills in maplists specified
 * 
 * 23    1-10-01 17:59 Koolhoven
 * MirrorRotate and SubMap now support maplistapplic 
 * 
 * 22    13-08-01 16:57 Koolhoven
 * readability: replaced "== false" by a "!"
 * 
 * 21    8-08-01 9:48 Koolhoven
 * adaptions to allow start with maplist as first parameter
 * 
 * 20    7-08-01 19:49 Koolhoven
 * densify and resample now also allow a maplist as input
 * 
 * 19    6/21/01 1:59p Martin
 * RowCols in submap may contain negative rowcols (outside map) parsing
 * was not correct in this case
 * 
 * 18    6/21/01 10:58a Martin
 * added support for giving the rowcol/coord infor to a submap form
 * together with the filename
 * 
 * 17    15/02/01 14:40 Willem
 * Added Value Range before the minimum and maximum values of the map in
 * Resample form
 * 
 * 16    15/02/01 13:59 Willem
 * Disabled attribute selection for MapResample. Resample needs patched
 * input, which is not provided for by inline MapAttribute maps.
 * 
 * 15    14-11-00 9:58a Martin
 * added support for attrib table columns in the mapselectors
 * 
 * 14    8-11-00 17:24 Koolhoven
 * use sFullNameQuoted() on input data to ensure selection in forms
 * 
 * 13    26-09-00 12:05 Hendrikse
 * set outdomain equal to input dom in FormMapDensify::exec()
 * 
 * 12    25/08/00 17:34 Willem
 * The resample form now detects possile equality of the map georef and
 * the selected georef. If this is the case the Show/Define buttons will
 * be disbaled and a remark will be displayed on the form
 * 
 * 11    18-07-00 11:54a Martin
 * extensions are now correct so a icon will show in the map nameeedit
 * 
 * 10    5/23/00 12:48p Wind
 * - accept  raster input maps with any domain in FormMapResample
 * - accept  raster input maps with any domain in FormMapAggr
 * 
 * 9     11-05-00 3:57p Martin
 * added 'attribute columns' nameedits
 * 
 * 8     3/02/00 9:47 Willem
 * The Coords option now correctly uses the CoordBounds of the map
 * 
 * 7     26/01/00 12:05 Willem
 * fUndef() now has an empty parameter list (it is a function not a
 * member) 
 * 
 * 6     25/01/00 9:57 Willem
 * No values will be visible in the lines/columns and coord fields when no
 * map is selected
 * 
 * 5     24/01/00 17:38 Willem
 * Updated the MapSubMap form:
 * - The default lines and columns now indicate the total size of the
 * input map
 * - The default coordinates are taken from the input map, but they are
 * available only in case the input map is north-oriented
 * - Added a remark to indicate whether the input map is north-oriented or
 * not
 * 
 * 4     23-11-99 13:45 Wind
 * adapted form to mapsubmapcorners and mapsubmapcoords
 * 
 * 3     9/13/99 1:04p Wind
 * comments
 * 
 * 2     9/13/99 12:35p Wind
 * adapted :exec functions to use of quoted file names when building an
 * expression
*/
// Revision 1.7  1998/09/16 17:33:54  Wim
// 22beta2
//
// Revision 1.6  1997/09/16 14:42:13  Wim
// FormDensifyMap: domain bool should not display a value range
//
// Revision 1.5  1997-09-03 11:49:19+02  Wim
// FormDensifyMap: also allow class and id maps but only for nearest neighbour
//
// Revision 1.4  1997-08-25 22:10:16+02  Wim
// In FormResampleMap use map->dvrs().rr() if map->rrMinMax() is invalid
//
// Revision 1.3  1997-08-19 17:31:11+02  Wim
// FormDensifyMap, enlargment factor LARGER than 1
//
// Revision 1.2  1997-08-14 16:41:26+02  Wim
// Resample nearest neighbour no value range asked
//
/* Form Map Applications with GeoRef change
   by Wim Koolhoven
   (c) Ilwis System Development ITC
	Last change:  WK   10 Jun 98    5:18 pm
*/
//#include "Client\MainWindow\mainwind.h"
#include "Client\Headers\AppFormsPCH.h"
#include "Engine\Function\FILTER.H"
#include "ApplicationsUI\frmmapap.h"

LRESULT Cmddensras(CWnd *wnd, const String& s)
{
	new FormDensifyMap(wnd, s.scVal());
	return -1;
}

FormDensifyMap::FormDensifyMap(CWnd* mw, const char* sPar)
: FormMapCreate(mw, SAFTitleDensifyMap)
{
  rEnl = 2;
  iMeth = 1;
  if (sPar) {
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
      if (fn.sExt == "" || fn.sExt == ".mpr" || fn.sExt == ".mpl")
        if (sMap == "")
          sMap = fn.sFullNameQuoted(true);
        else  
          sOutMap = fn.sFullName(false);
    }
  }
  fldMap = new FieldDataType(root, SAFUiRasMap, &sMap, new MapListerDomainType(0,false,true), true);
  fldMap->SetCallBack((NotifyProc)&FormDensifyMap::MapCallBack);
  stMapRemark = new StaticText(root, String('x',50));
  stMapRemark->SetIndependentPos();
//  Domain dm(1,100,0.1);
  new FieldReal(root, SAFUiDensFact, &rEnl, ValueRangeReal(1.0001,100,0.1));
  rg = new RadioGroup(root, SAFUiInterpolMethod, &iMeth);
  rg->SetCallBack((NotifyProc)&FormDensifyMap::MethodCallBack);
  rg->SetIndependentPos();
  RadioButton* rbNN = new RadioButton(rg, SAFUiNearestNeighbour);
  rbLin = new RadioButton(rg, SAFUiBilinear);
  rbLin->Align(rbNN, AL_AFTER);
  rbCub = new RadioButton(rg, SAFUiBicubic);
  rbCub->Align(rbLin, AL_AFTER);
  initMapOutValRange(false);
  SetHelpItem("ilwisapp\\densify_dialogbox.htm");
  create();
}

int FormDensifyMap::MethodCallBack(Event*)
{
  rg->StoreData();
  fldMap->StoreData();
  FileName fnMap(sMap);
  if (fnMap.sFile == "") {
    stMapRemark->SetVal("");
    return 0;
  } 
  RangeReal rr; 
  double rStep;
  try {
    Map mp;
    if (".mpl" == fnMap.sExt && "" == fnMap.sSectionPostFix)
    {
      MapList mpl(fnMap);
      mp = mpl->map(mpl->iLower());
    }
    else 
      mp = Map(fnMap);
    DomainValueRangeStruct dvs = mp->dvrs();
    if (!dvs.fValues() || 0!=dvs.dm()->pdbool()) {
      fvr->Hide();
      return 0;
    }
    rr = mp->rrMinMax();
    rStep = mp->dvrs().rStep();
    if (2 == iMeth) {
      double rWidth = rr.rWidth();
      rWidth /= 10;
      rr.rLo() -= rWidth;
      rr.rHi() += rWidth;
    }
  }
  catch (ErrorObject&) {
    return 0;
  }  
  ValueRange vr(rr, rStep);
  fvr->Show();
  fvr->SetVal(vr);
  return 0;
}

int FormDensifyMap::exec() 
{
  FormMapCreate::exec();
  String sExpr;
  FileName fn(sOutMap);
  FileName fnMap(sMap); 
  fOutMapList = ".mpl" == fnMap.sExt;
  sMap = fnMap.sRelativeQuoted(false,fn.sPath());
  String sMapList;
  bool fGeoRefNone;
  if (fOutMapList) {
    sMapList = sMap;
    sMap = "##";
    MapList mpl(fnMap);
    GeoRef gr = mpl->gr();
    fGeoRefNone = 0 != dynamic_cast<GeoRefNone*>(gr.ptr());
  }
  else {
    Map map(fnMap);
    GeoRef gr = map->gr();
    fGeoRefNone = 0 != dynamic_cast<GeoRefNone*>(gr.ptr());
  }
  if (fGeoRefNone) {
    MessageBox(SAFErrDensNonGrfMap.scVal(), SAFError.scVal(), MB_OK | MB_ICONEXCLAMATION);
    return 0;
  }
  String sMethod;
  switch (iMeth) {
    case 0: sMethod = "nearestneighbour"; break;
    case 1: sMethod = "bilinear"; break;
    case 2: sMethod = "bicubic"; break;
  }
  sExpr = String("MapDensify(%S,%g,%S)", sMap, rEnl, sMethod);
  if (fOutMapList)
    sExpr = String("MapListApplic(%S,%S)", sMapList, sExpr);
  execMapOut(sExpr);  
  return 0;
}

int FormDensifyMap::MapCallBack(Event*) 
{
  fldMap->StoreData();
  try {
    FileName fnMap(sMap);
    if (fnMap.sFile == "") {
      stMapRemark->SetVal("");
      return 0;
    }  
    Map mp;
    if (".mpl" == fnMap.sExt && "" == fnMap.sSectionPostFix)
    {
      MapList mpl(fnMap);
      mp = mpl->map(mpl->iLower());
    }
    else 
      mp = Map(fnMap);
    DomainValueRangeStruct dvs = mp->dvrs();
    String sRemark;
    if (!dvs.fValues() || 0!=dvs.dm()->pdbool()) {
      rg->SetVal(0);
      rbLin->Disable();
      rbCub->Disable();
      sRemark = dvs.dm()->sTypeName();
    }
    else {
      rbLin->Enable();
      rbCub->Enable();
      RangeReal rr = mp->rrMinMax();
      if (rr.fValid()) {
        sRemark = String(SAFInfMinMax_SS.scVal(),
                     dvs.sValue(rr.rLo()), dvs.sValue(rr.rHi()));
      }
      else {
        RangeReal rr = dvs.rrMinMax();
        sRemark = String(SAFInfRangeMinMax_SS.scVal(),
                     dvs.sValue(rr.rLo()), dvs.sValue(rr.rHi()));
      }
    }
    stMapRemark->SetVal(sRemark);
    return MethodCallBack(0);
  }
  catch (ErrorObject&) {
    stMapRemark->SetVal("");
  }
  return 0;
}

LRESULT Cmdresample(CWnd *wnd, const String& s)
{
	new FormResampleMap(wnd, s.scVal());
	return -1;
}

FormResampleMap::FormResampleMap(CWnd* mw, const char* sPar)
: FormMapCreate(mw, SAFTitleResampleMap)
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
      if (fn.sExt == ".grf")
        sGeoRef = fn.sFullNameQuoted(true);
      else if (fn.sExt == "" || fn.sExt == ".mpr" || fn.sExt == ".mpl")
        if (sMap == "")
          sMap = fn.sFullNameQuoted(true);
        else  
          sOutMap = fn.sFullName(false);
    }
  }
  fldMap = new FieldDataType(root, SAFUiRasMap, &sMap, new MapListerDomainType(0,false,true),true);
  fldMap->SetCallBack((NotifyProc)&FormResampleMap::MapCallBack);
  stRemGeoRef = new StaticText(root, String('x',50));
  stRemGeoRef->SetIndependentPos();
  stMapRemark = new StaticText(root, String('x',50));
  stMapRemark->SetIndependentPos();

  iMeth = 2;
  rg = new RadioGroup(root, SAFUiResampleMethod, &iMeth);
  rg->SetCallBack((NotifyProc)&FormResampleMap::MethodCallBack);
  rg->SetIndependentPos();
  rbNearest = new RadioButton(rg, SAFUiNearestNeighbour);
  rbBiLin = new RadioButton(rg, SAFUiBilinear);
  rbBiCub = new RadioButton(rg, SAFUiBicubic);
  initMapOutValRange(true);
  // re-assign the GRF call back
  fgr->SetCallBack((NotifyProc)&FormResampleMap::ChangeGeoRef);

  SetHelpItem("ilwisapp\\resample_dialog_box.htm");
  create();
}                    

int FormResampleMap::exec() 
{
  FormMapCreate::exec();
  String sExpr;
  FileName fn(sOutMap);
  FileName fnMap(sMap); 
  fOutMapList = ".mpl" == fnMap.sExt;
  sMap = fnMap.sRelativeQuoted(false,fn.sPath());
  String sMapList;
  if (fOutMapList) {
    sMapList = sMap;
    sMap = "##";
  }
  String sMethod;
  switch (iMeth) {
    case 0: sMethod = "nearest"; break;
    case 1: sMethod = "bilinear"; break;
    case 2: sMethod = "bicubic"; break;
  }
  sExpr = String("MapResample(%S,%S,%S)", 
                  sMap,sGeoRef,sMethod);
  if (fOutMapList)
    sExpr = String("MapListApplic(%S,%S)", sMapList, sExpr);
  execMapOut(sExpr);  
  return 0;
}

int FormResampleMap::ChangeGeoRef(Event* ev)
{
	FormMapCreate::ChangeGeoRef(ev);

	if (sMap.length() == 0)
		return 0;  // nothing to compare

  if (!mp.fValid()) 
	  mp = Map(sMap);  // Input map
	if (!m_gr.fValid())
		return 0;  // nothing to compare
	
	String sRem;
	GeoRefPtr* pgr = m_gr.ptr();
	if (mp->gr()->fEqual(*pgr))
	{
		sRem = SAFRemMapGeoRefEqual;
		DisableOK();
	}
	else
		EnableOK();
	
	stRemark->SetVal(sRem);
	
	return 0;
}

int FormResampleMap::MethodCallBack(Event*)
{
  rg->StoreData();
  if (0 == iMeth) {
    return 0;
  }
  fldMap->StoreData();
  FileName fnMap(sMap);
  if (fnMap.sFile == "") {
    stMapRemark->SetVal("");
	stRemGeoRef->SetVal("");
    return 0;
  } 
  RangeReal rr; 
  double rStep;
  try {
    if (!mp.fValid()) 
	    mp = Map(sMap);  // Input map
    rr = mp->rrMinMax();
    if (!rr.fValid())
      rr = mp->dvrs().rrMinMax();
    rStep = mp->dvrs().rStep();
    if (2 == iMeth) {
      double rWidth = rr.rWidth();
      rWidth /= 10;
      rr.rLo() -= rWidth;
      rr.rHi() += rWidth;
    }
  }
  catch (ErrorObject&) {
    return 0;
  }  
  ValueRange vr(rr, rStep);
  fvr->SetVal(vr);
  return 0;
}

int FormResampleMap::MapCallBack(Event*) 
{
	fldMap->StoreData();
	try
	{
		FileName fnMap(sMap);
		if (fnMap.sFile == "")
		{
			stMapRemark->SetVal("");
			stRemGeoRef->SetVal("");
			fvr->Hide();
			return 0;
		}  
    if (".mpl" == fnMap.sExt && "" == fnMap.sSectionPostFix)
    {
      MapList mpl(fnMap);
      mp = mpl->map(mpl->iLower());
    }
    else 
      mp = Map(fnMap);
		
		m_gr = mp->gr();
		if (m_gr.fValid())
			stRemGeoRef->SetVal(m_gr->sTypeName());
		else
			stRemGeoRef->SetVal("");
		
		Domain dm = mp->dm();
		if (!dm.fValid())
		{
			stMapRemark->SetVal("");
			fvr->Hide();
			return 0;
		} 
		sDomain = dm->sName(true);
		if (dm->pdi())
		{
			stMapRemark->SetVal(SAFRemImageRange);
			fvr->Hide();
			rbBiLin->Enable();
			rbBiCub->Enable();
		}
		else if (dm->pdvr() || dm->pdvi())
		{
			DomainValueRangeStruct dvs = mp->dvrs();
			RangeReal rr = mp->rrMinMax();
			String sRemark;
			if (rr.fValid())
				sRemark = String(SAFInfMinMax_SS.scVal(),
				dvs.sValue(rr.rLo()), dvs.sValue(rr.rHi()));
			else
			{
				rr = dvs.rrMinMax();
				sRemark = String(SAFInfRangeMinMax_SS.scVal(),
					dvs.sValue(rr.rLo()), dvs.sValue(rr.rHi()));
			}
			stMapRemark->SetVal(String("%S: %S", SAFUiValRange, sRemark));
			
			MethodCallBack(0);
			if (iMeth == 0)
				fvr->Hide();
			else
				fvr->Show();
			rbBiLin->Enable();
			rbBiCub->Enable();
		}
		else if (dm->pdcol())
    {
			String sRemark(dm->sTypeName());
			stMapRemark->SetVal(sRemark);
			rbNearest->SetVal(false);
			rbBiLin->SetVal(true);
			rbBiCub->SetVal(false);
			rbBiLin->Enable();
			rbBiCub->Disable();
			fvr->Hide();
    }
		else
		{
			String sRemark(dm->sTypeName());
			stMapRemark->SetVal(sRemark);
			rbNearest->SetVal(true);
			rbBiLin->SetVal(false);
			rbBiCub->SetVal(false);
			rbBiLin->Disable();
			rbBiCub->Disable();
			fvr->Hide();
		}
		if (mp.fValid() && fgr) 
			fgr->SetBounds(mp->cs(), mp->cb());
	}
	catch (ErrorObject& err)
	{
		err.Show();
		fvr->Hide();
	}
	return 0;
}

LRESULT Cmdapply3d(CWnd *wnd, const String& s)
{
	new FormMapApply3D(wnd, s.scVal());
	return -1;
}

FormMapApply3D::FormMapApply3D(CWnd* mw, const char* sPar)
: FormMapCreate(mw, SAFTitleMapApply3D)
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
      if (fn.sExt == ".grf")
        sGeoRef = fn.sFullNameQuoted(true);
      else if (fn.sExt == "" || fn.sExt == ".mpr")
        if (sMap == "")
          sMap = fn.sFullNameQuoted(false);
        else  
          sOutMap = fn.sFullName(false);
    }
  }
  new FieldMap(root, SAFUiRasMap, &sMap);
  initMapOutGeoRef3D();
  SetHelpItem("ilwisapp\\apply_3d_dialog_box.htm");
  create();
}

int FormMapApply3D::exec()
{
  FormMapCreate::exec();
  String sExpr;
  FileName fn(sOutMap);
  FileName fnMap(sMap); 
  sMap = fnMap.sRelativeQuoted(false,fn.sPath());
  sExpr = String("MapApply3D(%S,%S)",
                  sMap,sGeoRef);
  execMapOut(sExpr);  
  return 0;
}

long Cmdmirror(CWnd *parent, const String& s) {
	new FormMapMirrorRotate(parent, s.scVal());
	return -1;
}

FormMapMirrorRotate::FormMapMirrorRotate(CWnd* mw, const char* sPar)
: FormMapCreate(mw, SAFTitleMirrorRotate)
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
      if (fn.sExt == "" || fn.sExt == ".mpr" || fn.sExt == ".mpl")
        if (sMap == "") {
          sMap = fn.sFullNameQuoted(true);
          continue;
        }  
        else
          sOutMap = fn.sFullName(false);
    }
  }
  new FieldDataType(root, SAFUiRasMap, &sMap, new MapListerDomainType(0, 0, true), true);
  iMethod = 0;
  rg = new RadioGroup(root, "", &iMethod);
  new RadioButton(rg, SAFUiMirrorHorizontal);
  new RadioButton(rg, SAFUiMirrorVertical);
  new RadioButton(rg, SAFUiTranspose);
  new RadioButton(rg, SAFUiMirrorDiagonal);
  new RadioButton(rg, SAFUiRotate90);
  new RadioButton(rg, SAFUiRotate180);
  new RadioButton(rg, SAFUiRotate270);
  rg->SetIndependentPos();
  initMapOut(false,false);
  SetHelpItem("ilwisapp\\mirror_rotate_dialogbox.htm");
  create();
}                    

int FormMapMirrorRotate::exec() 
{
  FormMapCreate::exec();
  String sExpr;
  FileName fn(sOutMap);
  FileName fnMap(sMap); 
  fOutMapList = ".mpl" == fnMap.sExt;
  sMap = fnMap.sRelativeQuoted(false,fn.sPath());
  String sMapList;
  if (fOutMapList) {
    sMapList = sMap;
    sMap = "##";
  }
  String sMethod;
  switch (iMethod) {
    case 0: sMethod = "MirrHor"; break;
    case 1: sMethod = "MirrVert"; break;
    case 2: sMethod = "Transpose"; break;
    case 3: sMethod = "MirrDiag"; break;
    case 4: sMethod = "Rotate90"; break;
    case 5: sMethod = "Rotate180"; break;
    case 6: sMethod = "Rotate270"; break;
  }
  sExpr = String("MapMirrorRotate(%S,%S)", 
                  sMap,sMethod);
  if (fOutMapList)
    sExpr = String("MapListApplic(%S,%S)", sMapList, sExpr);
  execMapOut(sExpr);  
  return 0;
}

LRESULT Cmdsubras(CWnd *wnd, const String& s)
{
	new FormMapSubMap(wnd, s.scVal());
	return -1;
}

FormMapSubMap::FormMapSubMap(CWnd* mw, const char* sPar)
: FormMapCreate(mw, SAFTitleMapSubMap),
 	fPreSet(false)
{
	iFirstLine = iFirstCol = iUNDEF;
	iLines = iLastLine = iUNDEF;
	iCols = iLastCol = iUNDEF;	
  if (sPar) 
	{
    TextInput inp(sPar);
    TokenizerBase tokenizer(&inp);
    String sVal;
    Token tok = tokenizer.tokGet();
    sVal = tok.sVal();
    if (sVal != "")
		{

	    FileName fn(sVal);
      if (fn.sExt == "" || fn.sExt == ".mpr" || fn.sExt == ".mpl")
		    if (sMap == "")
			    sMap = fn.sFullNameQuoted(false);
				else  
					sOutMap = fn.sFullName(false);

      int iLenVal = sVal.length();
      String str(sPar);
      if (str.length() > iLenVal)
      {
  			String sSub(str.substr(iLenVal + 1));
  			if (sSub != "")
  			{
  				Array<String> arsPos;
  				Split(sSub, arsPos);
  				iMethod = 2;
  				iFirstLine = arsPos[0].iVal();
  				iFirstCol = arsPos[1].iVal();
  				iLastLine = arsPos[2].iVal();				
  				iLastCol = arsPos[3].iVal();	
  				fPreSet = true;
  			}
      }
		}
	}		

  fldMap = new FieldDataType(root, SAFUiRasMap, &sMap, new MapListerDomainType(0,true,true), true);
  fldMap->SetCallBack((NotifyProc)&FormMapSubMap::MapCallBack);
	iMethod = 0;
	rgMethod = new RadioGroup(root, "", &iMethod);
  rgMethod->SetCallBack((NotifyProc)&FormMapSubMap::MethodCallBack);
  RadioButton* rb1 = new RadioButton(rgMethod, SAFUiLinesColumns);
  RadioButton* rb2 = new RadioButton(rgMethod, SAFUiOppositeCorners);
  rbCoords = new RadioButton(rgMethod, SAFUiOppositeCoordinates);
  FieldBlank* fb = new FieldBlank(root, 0);

	// In case of regular GeoRefSubMap
  feFirstLine = new FieldInt(rb1, SAFUiFirstLine, &iFirstLine);
  feFirstLine->Align(fb, AL_UNDER);
  feFirstCol = new FieldInt(rb1, SAFUiFirstCol, &iFirstCol);
  feFirstCol->Align(feFirstLine, AL_UNDER);

  feNrLines = new FieldInt(rb1, SAFUiNrLines, &iLines);
  feNrLines->Align(feFirstCol, AL_UNDER);
  feNrCols = new FieldInt(rb1, SAFUiNrCols, &iCols);
  feNrCols->Align(feNrLines, AL_UNDER);

	// In case of GeoRefSubMapCorners
  feFirstLineCrnr = new FieldInt(rb2, SAFUiFirstLine, &iFirstLine);
  feFirstLineCrnr->Align(fb, AL_UNDER);
  feFirstColCrnr = new FieldInt(rb2, SAFUiFirstCol, &iFirstCol);
  feFirstColCrnr->Align(feFirstLineCrnr, AL_UNDER);

  feLastLineCrnr = new FieldInt(rb2, SAFUiLastLine, &iLastLine);
  feLastLineCrnr->Align(feFirstColCrnr, AL_UNDER);
  feLastColCrnr = new FieldInt(rb2, SAFUiLastCol, &iLastCol);
  feLastColCrnr->Align(feLastLineCrnr, AL_UNDER);

	// In case of GeoRefSubMapCoords
  fcFirst = new FieldCoord(rbCoords, SAFUiFirstCoord, &crd1);
  fcFirst->Align(fb, AL_UNDER);
  fcSecond = new FieldCoord(rbCoords, SAFUiOppositeCoord, &crd2);
  fcSecond->Align(fcFirst, AL_UNDER);

  FieldBlank* fbEmpty = new FieldBlank(rbCoords,  1);
  fbEmpty->Align(fcSecond, AL_UNDER);
  FieldBlank* fbEmpty2 = new FieldBlank(rbCoords,  1);
  fbEmpty2->Align(fbEmpty, AL_UNDER);

	// For all GeoRefSubMaps:
  initMapOut(false, false);
  initRemark();

  SetHelpItem("ilwisapp\\submap_of_raster_map_dialogbox.htm");
  create();
}                    

int FormMapSubMap::exec() 
{
  FormMapCreate::exec();
  String sExpr;
  FileName fn(sOutMap);
  FileName fnMap(sMap); 
  fOutMapList = ".mpl" == fnMap.sExt;
  sMap = fnMap.sRelativeQuoted(false,fn.sPath());
  String sMapList;
  if (fOutMapList) {
    sMapList = sMap;
    sMap = "##";
  }
  switch (iMethod) {
    case 0: sExpr = String("MapSubMap(%S,%li,%li,%li,%li)",
                     sMap, iFirstLine, iFirstCol, iLines, iCols);
            break;
    case 1: sExpr = String("MapSubMapCorners(%S,%li,%li,%li,%li)",
                     sMap, iFirstLine, iFirstCol, iLastLine, iLastCol);
            break;
    case 2: sExpr = String("MapSubMapCoords(%S,%lg,%lg,%lg,%lg)",
                     sMap, crd1.x, crd1.y, crd2.x, crd2.y);
            break;
  }
  if (fOutMapList)
    sExpr = String("MapListApplic(%S,%S)", sMapList, sExpr);
  execMapOut(sExpr);  
  return 0;
}

int FormMapSubMap::MapCallBack(Event*)
{
	try
	{
		fldMap->StoreData();
    FileName fnMap(sMap);
    if (fnMap.sFile == "") 
		{
      stRemark->SetVal("");
      return 0;
    }
    Map mp;
    if (".mpl" == fnMap.sExt && "" == fnMap.sSectionPostFix)
    {
      MapList mpl(fnMap);
      mp = mpl->map(mpl->iLower());
    }
    else 
      mp = Map(fnMap);
		if (!fPreSet)
		{
			iFirstLine = 1;
			iFirstCol = 1;
			iLines = mp->iLines();
			iLastLine = mp->iLines();
			iCols = mp->iCols();
			iLastCol = mp->iCols();
			CoordBounds cb = mp->gr()->cb();
			crd1 = cb.cMin;
			crd2 = cb.cMax;
		}
		else
		{
			crd1 = mp->gr()->cConv(RowCol(iFirstLine, iFirstCol));
			crd2 = mp->gr()->cConv(RowCol(iLastLine, iLastCol));
			iCols = iLastCol - iFirstCol;
			iLines = iLastLine - iFirstLine;
		}			
		UpdateSize();  // update the fields

		String sNorth;
		if (mp->gr()->fNorthOriented())
		{
			sNorth = String(SAFRemInMapIsNorthOriented);
			rbCoords->Enable();
		}
		else
		{
			sNorth = String(SAFRemInMapNotNorthOriented);
			rbCoords->Disable();
			if (iMethod == 2)   // Coordinates can not be handled for not-north oriented maps
			{
				iMethod = 0;
				rgMethod->SetVal(iMethod);
			}
		}

		stRemark->SetVal(sNorth);
	}
  catch (ErrorObject&)
	{
    stRemark->SetVal("");
  }
  return 0;
} 

void FormMapSubMap::UpdateSize()
{
	feFirstLine->SetVal(iFirstLine);
	feFirstCol->SetVal(iFirstCol);
	feNrLines->SetVal(iLines);
	feNrCols->SetVal(iCols);
	feFirstLineCrnr->SetVal(iFirstLine);
	feFirstColCrnr->SetVal(iFirstCol);
	feLastLineCrnr->SetVal(iLastLine);
	feLastColCrnr->SetVal(iLastCol);
	fcFirst->SetVal(crd1);
	fcSecond->SetVal(crd2);
}

int FormMapSubMap::MethodCallBack(Event*)
{
	// only the mapcallback can reset the FieldInt's and the FieldCoord's to
	// anything other than undefined values, so prevent initialisation here
	if (iFirstLine == iUNDEF || crd1.fUndef())
		return 0;

	rgMethod->StoreData();
	switch (iMethod) {
		case 0: feFirstLine->SetVal(iFirstLine);
						feFirstCol->SetVal(iFirstCol);
						feNrLines->SetVal(iLines);
						feNrCols->SetVal(iCols);
						break;
		case 1: feFirstLineCrnr->SetVal(iFirstLine);
						feFirstColCrnr->SetVal(iFirstCol);
						feLastLineCrnr->SetVal(iLastLine);
						feLastColCrnr->SetVal(iLastCol);
						break;
		case 2: fcFirst->SetVal(crd1);
						fcSecond->SetVal(crd2);
						break;
	}

  return 0;
} 
