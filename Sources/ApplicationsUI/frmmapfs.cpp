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
// $Log: /ILWIS 3.0/ApplicationForms/frmmapfs.cpp $
 * 
 * 16    28-08-01 11:43 Koolhoven
 * FormRasterize got a member bmap to store in the callback the input map,
 * to prevent too often constructing one
 * 
 * 15    19/02/01 14:39 Willem
 * Disabled polygon map attribute selection for PolygonToRaster
 * 
 * 14    14-11-00 9:58a Martin
 * added support for attrib table columns in the mapselectors
 * 
 * 13    9-11-00 19:06 Hendrikse
 * implemented default ValueRange seetting in FormInterpolMap::
 * MapCallBack
 * 
 * 12    8-11-00 17:24 Koolhoven
 * use sFullNameQuoted() on input data to ensure selection in forms
 * 
 * 11    21-09-00 10:20a Martin
 * added some attrib columns to the mapcreate fields
 * 
 * 10    30-08-00 2:32p Martin
 * 'attribute table ' nameedits changed/removed
 * 
 * 9     5/23/00 12:47p Wind
 * - accept  raster input maps with any domain in FormRasSeg
 * - accept  raster input maps with any domain in FormRasPol
 * 
 * 8     11-05-00 3:57p Martin
 * added 'attribute columns' nameedits
 * 
 * 7     9-05-00 10:52a Martin
 * use of attrib columns in nameedits
 * 
 * 6     28-02-00 11:56 Wind
 * adapted to changes in constructor of FieldColumn
 * 
 * 5     14-01-00 17:27 Koolhoven
 * Include extension (and thus icon) in input map name
 * 
 * 4     9/13/99 9:34a Wind
 * comments
 * 
 * 3     9/08/99 12:10p Wind
*/
// Revision 1.5  1998/09/16 17:33:54  Wim
// 22beta2
//
// Revision 1.4  1997/09/18 09:55:07  Wim
// FormInterpolMap should ask for domain and valuerange
//
// Revision 1.3  1997-09-11 20:26:37+02  Wim
// Removed attribute options from segras, polras and interpolras
//
// Revision 1.2  1997-08-21 16:59:31+02  Wim
// Set correct valuerange when colinf.rrMinMax() is onvalid
// (in FormRasterize::MapCallBack()
//
/* Form Map Applications from Segments
   by Wim Koolhoven
   (c) Ilwis System Development ITC
	Last change:  WK    6 Jul 98    6:34 pm
*/
//#include "Client\MainWindow\mainwind.h"
#include "Client\Headers\AppFormsPCH.h"
#include "ApplicationsUI\frmmapap.h"
#include "Engine\Table\COLINFO.H"


FormRasterize::FormRasterize(CWnd* mw, const String& sTitle)
: FormMapCreate(mw, sTitle), feMap(0), feAttrib(0), stMapRemark(0), 
  fAttrib(false), fOnlyValue(false), fWiderValRange(false)
{}

void FormRasterize::initAsk(long dmTypes)
{
  feMap->SetCallBack((NotifyProc)&FormRasterize::MapCallBack);
  if (0 != dmTypes) 
	{
    fOnlyValue = (dmVALUE == dmTypes);
    stMapRemark = new StaticText(root, String('x',50));
    stMapRemark->Align(feMap, AL_UNDER);
    stMapRemark->SetIndependentPos();
  }
}

int FormRasterize::MapCallBack(Event*)
{
  static bool fInside = false;
	String sM, sC;
  if (fInside)
    return 0;
  if (0 == feMap)
    return 0;
  fInside = true;
  void *adress=&feMap;
  feMap->StoreData();
//	FieldDataType::GetAttribParts(sMap, sM, sC);
  if (sMap.length() == 0) {
    if (stMapRemark)
      stMapRemark->SetVal("");
    if (fvr)  
      fvr->Hide();
    fInside = false;
    return 0;
  }
  FileName fnMap(sMap);
  if (fnMap.sFile == "") {
    if (stMapRemark)
      stMapRemark->SetVal("");
    if (fvr)  
      fvr->Hide();
    fInside = false;
    return 0;
  }  
  fmc->SetVal(fnMap.sFile);
  try {
    bmap = BaseMap(fnMap);
    if (bmap.fValid() && fgr) {
      fgr->SetNewName(bmap->fnObj.sFile);
      fgr->SetBounds(bmap->cs(), bmap->cb());
    }  
    Domain dm = bmap->dm();
    DomainValueRangeStruct dvs = bmap->dvrs();
    RangeReal rr = bmap->rrMinMax();
    if (bmap->fTblAtt()) 
		{
			FileName fnTbl(bmap->sTblAtt(), bmap->fnObj);
      if (fnMap.sCol != "") 
			{
				ColumnInfo colinf(fnTbl, fnMap.sCol);
        if (colinf.fValid()) 
				{
					dm = colinf.dm();
          dvs = colinf.dvrs();
          rr = colinf.rrMinMax();
        }  
      }  
    }    
    if (stMapRemark) {
      sDomain = dm->sName();
      if (dm->pdvi() || dm->pdvr()) {
        if (rr.fValid()) {
          String sRemark(SAFInfMinMax_SS.scVal(),
                   dvs.sValue(rr.rLo()), dvs.sValue(rr.rHi()));
          stMapRemark->SetVal(sRemark);
        }
        else {
          rr = dvs.rrMinMax();
          String sRemark(SAFInfRangeMinMax_SS.scVal(), 
                   dvs.sValue(rr.rLo()), dvs.sValue(rr.rHi()));
          stMapRemark->SetVal(sRemark);
        }
        if (fWiderValRange) {
          double rWidth = rr.rWidth();
          rWidth *= 0.3;
          rr.rLo() -= rWidth;
          rr.rHi() += rWidth;
        }
        ValueRange vr(rr, dvs.rStep());
        SetDefaultValueRange(vr);
        if (fvr)  
          fvr->Show();
      }
      else {
        if (fvr)  
          fvr->Hide();
        String sRemark(dm->sTypeName());
        stMapRemark->SetVal(sRemark);
      }  
    }  
    ValueRangeCallBack(0);
    if (fOnlyValue)
      if (!dm->pdv()) {
        stRemark->SetVal(SAFRemOnlyValueDomain);
        DisableOK();
      }  
      else
        EnableOK();
  }
  catch (ErrorObject&) {
    if (stMapRemark)
      stMapRemark->SetVal("");
  }
  fInside = false;
  return 0;  
}

int FormRasterize::exec() 
{
  FormMapCreate::exec();
//	FieldDataType ::GetAttribParts(sMap, sMap, sCol);
  FileName fnMap(sMap);
  BaseMap map(fnMap);
  FileName fn(sOutMap);
  sMap = fnMap.sRelativeQuoted(false,fn.sPath());
  Domain dm = map->dm();
  return 0;
}


LRESULT Cmdsegras(CWnd *wnd, const String& s)
{
	new FormRasSegment(wnd, s.scVal());
	return -1;
}

FormRasSegment::FormRasSegment(CWnd* mw, const char* sPar)
: FormRasterize(mw, SAFTitleSegRas)
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
      if (sGeoRef == "" && fn.sExt == ".grf")
        sGeoRef = fn.sFullNameQuoted(false);
      else if (sMap == "" && fn.sExt == "" || fn.sExt == ".mps")
        sMap = fn.sFullNameQuoted(false);
      else if (sOutMap == "" && fn.sExt == "" || fn.sExt == ".mpr") 
        sOutMap = fn.sFullName(false);
    }
  }
  feMap = new FieldSegmentMap(root, SAFUiSegMap, &sMap, new MapListerDomainType(".mps", 0, false)); /*dmVALUE|dmCLASS|dmIDENT|dmBOOL*/
  feMap->SetCallBack((NotifyProc)&FormRasterize::MapCallBack);
//  initAsk(dmVALUE|dmCLASS|dmIDENT|dmBOOL);
//  initMapOutValRange(true);
  initMapOut(true,false);
  SetHelpItem("ilwisapp\\segments_to_raster_dialogbox.htm");
  create();
}                    

int FormRasSegment::exec() 
{
  FormRasterize::exec();
  String sExpr;
  sExpr = String("MapRasterizeSegment(%S,%S)", 
                  sMap,sGeoRef);
  execMapOut(sExpr);  
  return 0;
}

LRESULT Cmdsegdensity(CWnd *wnd, const String& s)
{
	new FormMapSegmentDensity(wnd, s.scVal());
	return -1;
}

FormMapSegmentDensity::FormMapSegmentDensity(CWnd* mw, const char* sPar)
: FormRasterize(mw, SAFTitleRasSegDensity)
{
  sMask = "*";
  fMask = false;
  sDomain = "value.dom";
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
      if (sGeoRef == "" && fn.sExt == ".grf")
        sGeoRef = fn.sFullNameQuoted(false);
      else if (sMap == "" && fn.sExt == "" || fn.sExt == ".mps")
        sMap = fn.sFullNameQuoted(false);
      else if (sOutMap == "" && fn.sExt == "" || fn.sExt == ".mpr") 
        sOutMap = fn.sFullName(false);
    }
  }
  feMap = new FieldSegmentMap(root, SAFUiSegMap, &sMap, new MapListerDomainType(".mps", 0, true));
	//initAsk(dmVALUE|dmCLASS|dmIDENT|dmBOOL);
  feMap->SetCallBack((NotifyProc)&FormRasterize::MapCallBack);
  CheckBox* cb = new CheckBox(root, SAFUiMask, &fMask);
  new FieldString(cb, "", &sMask);
  FieldBlank* fb = new FieldBlank(root, 0);
  fb->Align(cb, AL_UNDER);
  initMapOut(true,(long)dmVALUE);
  SetHelpItem("ilwisapp\\segment_density_dialog_box.htm");
  create();
}                    

int FormMapSegmentDensity::exec() 
{
  FormRasterize::exec();
  String sAppl = "MapSegmentDensity";
  String sExpr;
  if (fMask) 
    sExpr = String("%S(%S,\"%S\",%S)", sAppl, sMap, sMask, sGeoRef);
  else
    sExpr = String("%S(%S,%S)", sAppl, sMap, sGeoRef);
  execMapOut(sExpr);  
  return 0;
}

LRESULT Cmdpolras(CWnd *wnd, const String& s)
{
	new FormRasPolygon(wnd, s.scVal());
	return -1;
}

FormRasPolygon::FormRasPolygon(CWnd* mw, const char* sPar)
: FormRasterize(mw, SAFTitlePolRas)
{
	if (sPar)
	{
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
			if (sGeoRef == "" && fn.sExt == ".grf")
				sGeoRef = fn.sFullNameQuoted(false);
			else if (sMap == "" && fn.sExt == "" || fn.sExt == ".mpa")
				sMap = fn.sFullNameQuoted(false);
			else if (sOutMap == "" && fn.sExt == "" || fn.sExt == ".mpr") 
				sOutMap = fn.sFullName(false);
		}
	}
	feMap = new FieldPolygonMap(root, SAFUiPolMap, &sMap, new MapListerDomainType(".mpa", 0));
	feMap->SetCallBack((NotifyProc)&FormRasterize::MapCallBack);
	
	initMapOut(true,false);
	SetHelpItem("ilwisapp\\polygons_to_raster_dialogbox.htm");
	create();
}                    

int FormRasPolygon::exec() 
{
  FormRasterize::exec();
  String sExpr;
  sExpr = String("MapRasterizePolygon(%S,%S)", 
                  sMap,sGeoRef);
  execMapOut(sExpr);  
  return 0;
}

LRESULT Cmdinterpolseg(CWnd *wnd, const String& s)
{
	new FormInterpolMap(wnd, s.scVal());
	return -1;
}

FormInterpolMap::FormInterpolMap(CWnd* mw, const char* sPar)
: FormRasterize(mw, SAFTitleInterpolContours)
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
      if (sGeoRef == "" && fn.sExt == ".grf")
        sGeoRef = fn.sFullNameQuoted(false);
      else if (sMap == "" && fn.sExt == "" || fn.sExt == ".mps")
        sMap = fn.sFullNameQuoted(true);
      else if (sOutMap == "" && fn.sExt == "" || fn.sExt == ".mpr") 
        sOutMap = fn.sFullName(false);
    }
  }
//  feMap = new FieldSegmentMap(root, SAFUiContourMap, &sMap);
  feMap = new FieldDataType(root, SAFUiContourMap, &sMap, new MapListerDomainType(".mps",dmVALUE, false),true);
  feMap->SetCallBack((NotifyProc)&FormInterpolMap::MapCallBack);
//  initAsk(dmVALUE);
//  initMapOutValRange(true);
  initMapOut(true,(long)dmVALUE);
  SetHelpItem("ilwisapp\\contour_interpolation_dialog_box.htm");
  create();
}                    

int FormInterpolMap::exec() 
{
  FormRasterize::exec();
  String sExpr;
  sExpr = String("MapInterpolContour(%S,%S)", 
                  sMap,sGeoRef);
  execMapOut(sExpr);  
  return 0;
}

int FormInterpolMap::MapCallBack(Event*)
{
  FormRasterize::MapCallBack(0);
  if (sMap.length() == 0)
    return 0;
  try {
    FileName fnMap(sMap);
    if (fnMap.sFile == "")
      return 0;
    SegmentMap segmp(fnMap);
    Domain dm = segmp->dm();
    fdc->SetVal(dm->sName());
		ValueRange vr = MapInterpolContour::vrDefault(segmp);
    SetDefaultValueRange(vr);
  }
  catch (ErrorObject&) {
  }
  return 0;
}




