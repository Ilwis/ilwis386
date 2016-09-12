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
*/
//#include "Client\MainWindow\mainwind.h"
#include "Client\Headers\AppFormsPCH.h"
#include "Client\FormElements\fldcs.h"
#include "ApplicationsUI\frmmapap.h"
#include "Applications\Raster\MAPMVAVG.H"
#include "Applications\Raster\MAPMVSRF.H"
#include "Applications\Raster\Mapkrig.h"
#include "Applications\Raster\MAPCOKRI.H"
#include "Client\FormElements\fldsmv.h"
#include "Engine\Applications\MAPFPNT.H"

LRESULT Cmdpntras(CWnd *wnd, const String& s)
{
	new FormRasPoint(wnd, s.c_str());
	return -1;
}

FormRasPoint::FormRasPoint(CWnd* mw, const char* sPar)
: FormRasterize(mw, TR("Rasterize Point Map"))
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
      else if (sMap == "" && fn.sExt == "" || fn.sExt == ".mpp")
        sMap = fn.sFullNameQuoted(false);
      else if (sOutMap == "" && fn.sExt == "" || fn.sExt == ".mpr") 
        sOutMap = fn.sFullName(false);
    }
  }
  feMap = new FieldPointMap(root, TR("&Point Map"), &sMap, new MapListerDomainType(".mpp", 0, true));
  feMap->SetCallBack((NotifyProc)&FormRasterize::MapCallBack);
	initAsk( dmVALUE | dmCLASS | dmIDENT | dmBOOL | dmSTRING | dmUNIQUEID);
  void *adress=&feMap;

  iPointSize = 1;
  new FieldInt(root, TR("Point &Size"), &iPointSize);
  initMapOutValRange(true);
  SetHelpItem("ilwisapp\\points_to_raster_dialogbox.htm");
  create();
}                    

int FormRasPoint::exec() 
{
  FormRasterize::exec();
  String sExpr;
  sExpr = String("MapRasterizePoint(%S,%S,%i)", 
                  sMap, sGeoRef, iPointSize);
  execMapOut(sExpr);  
  return 0;
}

LRESULT Cmdpntdensity(CWnd *parent, const String& s) {
	new FormRasPointCount(parent, s.c_str());
	return -1;
}

FormRasPointCount::FormRasPointCount(CWnd* mw, const char* sPar)
: FormRasterize(mw, TR("Point Density"))
{
  sDomain = "count.dom";
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
      else if (sMap == "" && fn.sExt == "" || fn.sExt == ".mpp")
        sMap = fn.sFullNameQuoted(false);
      else if (sOutMap == "" && fn.sExt == "" || fn.sExt == ".mpr") 
        sOutMap = fn.sFullName(false);
    }
  }
  feMap = new FieldPointMap(root, TR("&Point Map"), &sMap);
  initAsk(dmVALUE|dmCLASS|dmIDENT|dmBOOL);

  iPointSize = 1;
  new FieldInt(root, TR("Point &Size"), &iPointSize);
  initMapOut(true,(long)dmVALUE);
  SetHelpItem("ilwisapp\\point_density_dialog_box.htm");
  create();
}                    

int FormRasPointCount::exec() 
{
  FormRasterize::exec();
  String sExpr;
  sExpr = String("MapRasterizePointCount(%S,%S,%i)", 
                  sMap, sGeoRef, iPointSize);
  execMapOut(sExpr);  
  return 0;
}

LRESULT Cmdnearestpnt(CWnd *wnd, const String& s)
{
	new FormMapNearestPoint(wnd, s.c_str());
	return -1;
}

FormMapNearestPoint::FormMapNearestPoint(CWnd* mw, const char* sPar)
: FormRasterize(mw, TR("Nearest Point"))
{
	fSphericalDistance = false;
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
      else if (sMap == "" && fn.sExt == "" || fn.sExt == ".mpp")
        sMap = fn.sFullNameQuoted(false);
      else if (sOutMap == "" && fn.sExt == "" || fn.sExt == ".mpr") 
        sOutMap = fn.sFullName(false);
    }
  }
  feMap = new FieldPointMap(root, TR("&Point Map"), &sMap, new MapListerDomainType(".mpp", 0, true));
	initAsk(dmVALUE|dmCLASS|dmIDENT|dmBOOL);
	CheckBox* cb = new CheckBox(root, TR("Use &Spherical Distance"), &fSphericalDistance);
	cb->SetIndependentPos();
  initMapOutValRange(true);
  SetAppHelpTopic("ilwisapp\\nearest_point_dialogbox.htm");
  create();
}                    

int FormMapNearestPoint::exec() 
{
  FormRasterize::exec();
  String sExpr;
	String sDistMeth = String("%s", fSphericalDistance ? "sphere" : "plane");
  sExpr = String("MapNearestPoint(%S,%S,%S)", 
                  sMap, sGeoRef, sDistMeth);
  execMapOut(sExpr);  
  return 0;
}

FormMapGridding::FormMapGridding(CWnd* mw, const String& sTitle, 
                                 const char *sPar)
: FormRasterize(mw, sTitle)
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
      else if (sMap == "" && fn.sExt == "" || fn.sExt == ".mpp")
        sMap = fn.sFullNameQuoted(false);
      else if (sOutMap == "" && fn.sExt == "" || fn.sExt == ".mpr") 
        sOutMap = fn.sFullName(false);
    }
  }
  feMap = new FieldPointMap(root, TR("&Point Map"), &sMap, new MapListerDomainType(".mpp", dmVALUE, true));
  initAsk(dmVALUE);
}


int FormMapGridding::exec() 
{
  FormRasterize::exec(); 
 
  return 0;
}

FormMapGriddingWeighted::FormMapGriddingWeighted(CWnd* mw, const String& sTitle, 
                                 const char *sPar)
: FormMapGridding(mw, sTitle, sPar), frLimDist(0)
{
		iWeight = 0;
    rExp = 1;
    rLimDist = 100;
		feMap->SetCallBack((NotifyProc)&FormMapGriddingWeighted::MapCallBack);
    RadioGroup* rg = new RadioGroup(root, TR("&Weight Function:"), &iWeight);
    rg->SetIndependentPos();
    new RadioButton(rg, TR("&Inverse Distance"));
    new RadioButton(rg, TR("&Linear Decrease"));
//    Domain dm(0, 1e6, 0);
    new FieldReal(root, TR("Weight &Exponent"), &rExp, ValueRangeReal(0, 1e6, 0));
    frLimDist = new FieldReal(root, TR("Limiting &Distance"), &rLimDist, ValueRangeReal(0, 1e8, 1));
		fSphericalDistance = false;
		cbSphDis = new CheckBox(root, TR("Use &Spherical Distance"), &fSphericalDistance);
		cbSphDis->SetIndependentPos();
		cbSphDis->SetCallBack((NotifyProc)&FormMapGriddingWeighted::DistanceMethodCallBack);
}

int FormMapGriddingWeighted::MapCallBack(Event*)
{
  FormMapGridding::MapCallBack(0);
  if (sMap.length() == 0)
    return 0;
  try {
    PointMap map(sMap);
    double rDflt = MapFromPointMap::rLimDistDefault(map, fSphericalDistance);
    frLimDist->SetVal(rDflt);
  }
  catch (...) {}  
  return 1;  
}

int FormMapGriddingWeighted::DistanceMethodCallBack(Event*)
{
	cbSphDis->StoreData();
  if (sMap.length() == 0)
    return 0;
  try {
    PointMap map(sMap);
    double rDflt = MapFromPointMap::rLimDistDefault(map, fSphericalDistance);
    frLimDist->SetVal(rDflt);
  }
  catch (...) {}  
  return 1;  
}
		
int FormMapGriddingWeighted::exec() 
{
	FormMapGridding::exec();
	if (frLimDist) {  
    switch (iWeight) {
      case 0:
        sWeight = String("InvDist(%g,%g)",rExp,rLimDist);
        break;
      case 1:  
        sWeight = String("Linear(%g,%g)",rExp,rLimDist);
        break;
    }
  }
	return 0;
}

LRESULT Cmdmovaverage(CWnd *wnd, const String& s)
{
	new FormMapMovingAverage(wnd, s.c_str());
	return -1;
}

FormMapMovingAverage::FormMapMovingAverage(CWnd* mw, const char* sPar)
: FormMapGriddingWeighted(mw, TR("Moving Average"), sPar)
{
  initMapOutValRange(true);
  SetHelpItem("ilwisapp\\moving_average_dialogbox.htm");
  create();
}

int FormMapMovingAverage::exec()
{
  FormMapGriddingWeighted::exec();
  String sExpr;
	String sDistMeth = String("%s", fSphericalDistance ? "sphere" : "plane");
  sExpr = String("MapMovingAverage(%S,%S,%S,%S)", 
                  sMap, sGeoRef, sWeight, sDistMeth);
  execMapOut(sExpr);  
  return 0;
}

class FieldSurfaceSimple: public FieldOneSelect
{
public:
  FieldSurfaceSimple(FormEntry* parent, long* value)
  : FieldOneSelect(parent, value, false)
  {
    SetWidth(75);
  }
  void create() {
    FieldOneSelect::create();
    ose->AddString(TR("Plane.srf").c_str());
    ose->AddString(TR("2nd degree Linear.srf").c_str());	    
    ose->AddString(TR("2nd degree Parabolic.srf").c_str());	    
    ose->AddString(TR("2nd degree.srf").c_str());	    
    ose->AddString(TR("3rd degree.srf").c_str());	    
    ose->AddString(TR("4th degree.srf").c_str());	    
    ose->AddString(TR("5th degree.srf").c_str());	    
    ose->AddString(TR("6th degree.srf").c_str());	    
    ose->SetCurSel(*val);
  }
};

class FieldSurface: public FieldGroup
{
public:
  FieldSurface(FormEntry* parent, const String& sQuestion, long* val)
  : FieldGroup(parent)
  {
    if (sQuestion.length() != 0)
      new StaticTextSimple(this, sQuestion);
    fss = new FieldSurfaceSimple(this, val);
    if (children.iSize() > 1) // also static text
      children[1]->Align(children[0], AL_AFTER);
  }
  void SetCallBack(NotifyProc np) {
    fss->SetCallBack(np);
  }
private:
  FieldSurfaceSimple* fss;
};

LRESULT Cmdmovsurface(CWnd *wnd, const String& s)
{
	new FormMapMovingSurface(wnd, s.c_str());
	return -1;
}

FormMapMovingSurface::FormMapMovingSurface(CWnd* mw, const char* sPar)
: FormMapGriddingWeighted(mw, TR("Moving Surface"), sPar)
{
  fWiderValRange = true;
  iSurface = 0;
  FieldSurface* fs = new FieldSurface(root, TR("&Surface"), &iSurface);
  initMapOutValRange(true);
  SetHelpItem("ilwisapp\\moving_surface_dialog_box.htm");
  create();
}

int FormMapMovingSurface::exec()
{
  FormMapGriddingWeighted::exec();
  String sExpr;
  String sSurface;
  switch (iSurface) {
    case 0: sSurface = "Plane"; break;
    case 1: sSurface = "Linear2"; break;
    case 2: sSurface = "Parabolic2"; break;
    case 3: sSurface = "2"; break;
    case 4: sSurface = "3"; break;
    case 5: sSurface = "4"; break;
    case 6: sSurface = "5"; break;
    case 7: sSurface = "6"; break;
  }
	String sDistMeth = String("%s", fSphericalDistance ? "sphere" : "plane");
  sExpr = String("MapMovingSurface(%S,%S,%S,%S,%S)",
                  sMap, sGeoRef, sSurface, sWeight, sDistMeth);
  execMapOut(sExpr);  
  return 0;
}

LRESULT Cmdtrendsurface(CWnd *wnd, const String& s)
{
	new FormMapTrendSurface(wnd, s.c_str());
	return -1;
}

FormMapTrendSurface::FormMapTrendSurface(CWnd* mw, const char* sPar)
: FormMapGridding(mw, TR("Trend Surface"), sPar)
{
  fWiderValRange = true;
  iSurface = 3;
  new FieldSurface(root, TR("&Surface"), &iSurface);
/*  
  RadioGroup* rg = new RadioGroup(root, TR("&Surface"), &iSurface);
  new RadioButton(rg, TR("Plane.srf"));
  new RadioButton(rg, TR("2nd degree Linear.srf"));
  new RadioButton(rg, TR("2nd degree Parabolic.srf"));
  new RadioButton(rg, TR("2nd degree.srf"));
  new RadioButton(rg, TR("3rd degree.srf"));
  new RadioButton(rg, TR("4th degree.srf"));
  new RadioButton(rg, TR("5th degree.srf"));
  new RadioButton(rg, TR("6th degree.srf"));
*/  
  initMapOutValRange(true);
  SetHelpItem("ilwisapp\\trend_surface_dialog_box.htm");
  create();
}                    

int FormMapTrendSurface::exec() 
{
  FormMapGridding::exec();
  String sExpr;
  String sSurface;
  switch (iSurface) {
    case 0: sSurface = "Plane"; break;
    case 1: sSurface = "Linear2"; break;
    case 2: sSurface = "Parabolic2"; break;
    case 3: sSurface = "2"; break;
    case 4: sSurface = "3"; break;
    case 5: sSurface = "4"; break;
    case 6: sSurface = "5"; break;
    case 7: sSurface = "6"; break;
  }
  sExpr = String("MapTrendSurface(%S,%S,%S)",
                  sMap, sGeoRef, sSurface);
  execMapOut(sExpr);  
  return 0;
}

LRESULT Cmdkriging(CWnd *wnd, const String& s)
{
	new FormMapKriging(wnd, s.c_str());
	return -1;
} 

FormMapKriging::FormMapKriging(CWnd* mw, const char* sPar)
: FormRasterize(mw, TR("Kriging"))
{
  fWiderValRange = true;
  iSimpleOrdinary = 1;
  fDuplicRemoval = true;
  iDuplicRemoval = 0;
  rLimDist = 1000;
  riMinMax = RangeInt(1,16);
  rTolerance = 0.1;
  fErrorMap = false;
	fSphericalDistance = false;

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
      else if (sMap == "" && fn.sExt == "" || fn.sExt == ".mpp")
        sMap = fn.sFullNameQuoted(false);
      else if (sOutMap == "" && fn.sExt == "" || fn.sExt == ".mpr") 
        sOutMap = fn.sFullName(false);
    }
  }
  feMap = new FieldPointMap(root, TR("&Point Map"), &sMap, new MapListerDomainType(".mpp", dmVALUE|dmBOOL, true));
  initAsk(dmVALUE);
	feMap->SetCallBack((NotifyProc)&FormMapKriging::MapCallBack);

  FieldSemiVariogram* fsv = new FieldSemiVariogram(root, TR("&SemiVariogram"), &smv);
  fsv->SetIndependentPos();
  RadioGroup* rg = new RadioGroup(root, TR("&Method:"), &iSimpleOrdinary);
  rg->Align(fsv, AL_AFTER);
  new RadioButton(rg, TR("&Simple Kriging"));
  RadioButton* rbOrdinary = new RadioButton(rg, TR("&Ordinary Kriging"));

  FieldGroup* fgOrdinary = new FieldGroup(rbOrdinary);
  fgOrdinary->Align(fsv, AL_UNDER);
  new FieldReal(fgOrdinary, TR("Limiting &Distance"), &rLimDist, ValueRangeReal(0, 1e6, 0));
  new FieldRangeInt(fgOrdinary, TR("&Min, max nr of points"), &riMinMax, ValueRange(1,100));

  CheckBox* cb = new CheckBox(root, TR("&Remove Duplicates"), &fDuplicRemoval);
  FieldGroup* fgRemDup = new FieldGroup(cb);
  RadioGroup* rgDup = new RadioGroup(fgRemDup, "", &iDuplicRemoval,true);
  rgDup->Align(cb, AL_AFTER);
  new RadioButton(rgDup, TR("&Average"));
  new RadioButton(rgDup, TR("&First Value"));
  FieldReal* frTol = new FieldReal(fgRemDup, TR("&Tolerance (m)"), &rTolerance, ValueRange(0,1e12,0.1));
  frTol->Align(cb, AL_UNDER);

	CheckBox* cbSphDist = new CheckBox(root, TR("Use &Spherical Distance"), &fSphericalDistance);
	cbSphDist->SetIndependentPos();
  initMapOutValRange(true);
  CheckBox* cbEr = new CheckBox(root, TR("&Error Map"), &fErrorMap);

  SetHelpItem("ilwisapp\\kriging_dialog_box.htm");
  create();
}                    

int FormMapKriging::MapCallBack(Event*)
{
	FormRasterize::MapCallBack(0);
  if (sMap.length() == 0)
    return 0;
  try {
    PointMap pmap(sMap);
    vr = MapKriging::vrDefault(pmap);
    SetDefaultValueRange(vr);
  }
	catch (...) {}  
	return 1;  
}

int FormMapKriging::exec() 
{
  sDomain = "value";
	FormRasterize::exec();
  String sExpr;
  String sSmv = smv.sExpression();
  String sDupRem;
	String sDistMeth = String("%s", fSphericalDistance ? "sphere" : "plane");

  FileName fnOutMap(sMap);

  if (fDuplicRemoval)
    sDupRem = String("%s,%.4f",
                     iDuplicRemoval ? "firstval" : "average" ,
                     rTolerance);
  else
    sDupRem = "no";
	sErrorMap = fErrorMap ? String("1") : String("0");
  switch (iSimpleOrdinary) {
    case 0:
      sExpr = String("MapKrigingSimple(%S,%S,%S,%S,%S,%S)",
                     sMap, sGeoRef, sSmv, sDistMeth,
                     sErrorMap, sDupRem);
      break;
    case 1: {
      int iMin = riMinMax.iLo();
      int iMax = riMinMax.iHi();
      sExpr = String("MapKrigingOrdinary(%S,%S,%S,%f,%S,%S,%i,%i,%S)",
                     sMap, sGeoRef, sSmv, rLimDist, sDistMeth,
                     sErrorMap, iMin, iMax, sDupRem);
    }  break;
  }
  execMapOut(sExpr);
  return 0;
}

LRESULT Cmdunivkriging(CWnd *wnd, const String& s)
{
	new FormMapUniversalKriging(wnd, s.c_str());
	return -1;
} 

FormMapUniversalKriging::FormMapUniversalKriging(CWnd* mw, const char* sPar)
: FormRasterize(mw, TR("Universal Kriging"))
{
  fWiderValRange = true;
  iTrend = 0;
  fDuplicRemoval = true;
  iDuplicRemoval = 0;
  rLimDist = 1000;
  riMinMax = RangeInt(3,16);
  rTolerance = 0.1;
  fErrorMap = false;

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
      if (sGeoRef == "" && fn.sExt == ".grf")
        sGeoRef = fn.sFullNameQuoted(false);
      else if (sMap == "" && fn.sExt == "" || fn.sExt == ".mpp")
        sMap = fn.sFullNameQuoted(false);
      else if (sOutMap == "" && fn.sExt == "" || fn.sExt == ".mpr") 
        sOutMap = fn.sFullName(false);
    }
  }
  feMap = new FieldPointMap(root, TR("&Point Map"), &sMap, new MapListerDomainType(".mpp", dmVALUE, true));
  initAsk(dmVALUE);
	feMap->SetCallBack((NotifyProc)&FormMapUniversalKriging::MapCallBack);

  rgTrend = new RadioGroup(root, TR("&Trend"), &iTrend, true);
  rgTrend->SetCallBack((NotifyProc)&FormMapUniversalKriging::TrendCallBack);
  rgTrend->SetIndependentPos();
  new RadioButton(rgTrend, "&1");
  new RadioButton(rgTrend, "&2");

  FieldSemiVariogram* fsv = new FieldSemiVariogram(root, TR("&SemiVariogram"), &smv);
  fsv->SetIndependentPos();

  CheckBox* cb = new CheckBox(root, TR("&Remove Duplicates"), &fDuplicRemoval);
  cb->Align(fsv, AL_AFTER);
  FieldGroup* fgRemDup = new FieldGroup(cb);
  RadioGroup* rgDup = new RadioGroup(fgRemDup, "", &iDuplicRemoval,true);
  rgDup->Align(cb, AL_UNDER);
  new RadioButton(rgDup, TR("&Average"));
  new RadioButton(rgDup, TR("&First Value"));
  FieldReal* frTol = new FieldReal(fgRemDup, TR("&Tolerance (m)"), &rTolerance, ValueRange(0,1e12,0.1));
  frTol->Align(rgDup, AL_UNDER);

  FieldReal* fr = new FieldReal(root, TR("Limiting &Distance"), &rLimDist, ValueRangeReal(0, 1e6, 0));
  fr->Align(fsv, AL_UNDER);
  fmm = new FieldRangeInt(root, TR("&Min, max nr of points"), &riMinMax, ValueRange(3,100));

  initMapOutValRange(true);
  CheckBox* cbEr = new CheckBox(root, TR("&Error Map"), &fErrorMap);
  //new FieldMapCreate(cbEr, "", &sErrorMap);

  SetHelpItem("ilwisapp\\universal_kriging_dialog_box.htm");
  create();
}                    

int FormMapUniversalKriging::TrendCallBack(Event*)
{
  rgTrend->StoreData();
  switch (iTrend) {
    case 0: // 1
      fmm->SetVal(RangeInt(3,16));
      break;
    case 1: // 2
      fmm->SetVal(RangeInt(6,16));
      break;
  }
  return 0;
}

int FormMapUniversalKriging::MapCallBack(Event*)
{
	FormRasterize::MapCallBack(0);
  if (sMap.length() == 0)
    return 0;
  try {
    PointMap pmap(sMap);
    vr = MapKriging::vrDefault(pmap);
    SetDefaultValueRange(vr);
  }
	catch (...) {}  
	return 1;  
}

int FormMapUniversalKriging::exec() 
{
  FormRasterize::exec();
  String sExpr;
  String sSmv = smv.sExpression();
  String sDupRem;
  FileName fnErr(sErrorMap);
  FileName fnOutMap(sMap);
  sErrorMap = fnErr.sRelativeQuoted(false,fnOutMap.sPath());
  if (fDuplicRemoval)
    sDupRem = String("%s,%.4f",
                     iDuplicRemoval ? "firstval" : "average" ,
                     rTolerance);
  else
    sDupRem = "no";

  int iMin = riMinMax.iLo();
  int iMax = riMinMax.iHi();
	sErrorMap = fErrorMap ? String("1") : String("0");
  sExpr = String("MapKrigingUniversal(%S,%S,%S,%f,%i,0,1,%S,%i,%i,%S)",
                     sMap, sGeoRef, sSmv, rLimDist,
                     1+iTrend,
                     sErrorMap, iMin, iMax, sDupRem);
  execMapOut(sExpr);
  return 0;
}


LRESULT Cmdanisotrkriging(CWnd *wnd, const String& s)
{
	new FormMapAnisotropicKriging(wnd, s.c_str());
	return -1;
} 

FormMapAnisotropicKriging::FormMapAnisotropicKriging(CWnd* mw, const char* sPar)
: FormRasterize(mw, TR("Anisotropic Kriging"))
{
  rRatio = 1;
  rAngle = 0;
  fWiderValRange = true;
  fDuplicRemoval = true;
  iDuplicRemoval = 0;
  rLimDist = 1000;
  riMinMax = RangeInt(1,16);
  rTolerance = 0.1;
  fErrorMap = false;

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
      else if (sMap == "" && fn.sExt == "" || fn.sExt == ".mpp")
        sMap = fn.sFullNameQuoted(false);
      else if (sOutMap == "" && fn.sExt == "" || fn.sExt == ".mpr") 
        sOutMap = fn.sFullName(false);
    }
  }
  feMap = new FieldPointMap(root, TR("&Point Map"), &sMap, new MapListerDomainType(".mpp", dmVALUE, true));
  initAsk(dmVALUE);
	feMap->SetCallBack((NotifyProc)&FormMapAnisotropicKriging::MapCallBack);

  FieldGroup* fg = new FieldGroup(root);
  fg->SetIndependentPos();
  FieldReal* frAngle = new FieldReal(fg, TR("&Angle"), &rAngle, ValueRange(-360,360,0.01));
  FieldReal* frRatio = new FieldReal(fg, TR("&Ratio"), &rRatio, ValueRange(1,10,0.01));
  frRatio->Align(frAngle, AL_AFTER);

  FieldSemiVariogram* fsv = new FieldSemiVariogram(root, TR("&SemiVariogram"), &smv);
  fsv->Align(fg, AL_UNDER);
  fsv->SetIndependentPos();

  CheckBox* cb = new CheckBox(root, TR("&Remove Duplicates"), &fDuplicRemoval);
  cb->Align(fsv, AL_AFTER);
  FieldGroup* fgRemDup = new FieldGroup(cb);
  RadioGroup* rgDup = new RadioGroup(fgRemDup, "", &iDuplicRemoval,true);
  rgDup->Align(cb, AL_UNDER);
  new RadioButton(rgDup, TR("&Average"));
  new RadioButton(rgDup, TR("&First Value"));
  FieldReal* frTol = new FieldReal(fgRemDup, TR("&Tolerance (m)"), &rTolerance, ValueRange(0,1e12,0.1));
  frTol->Align(rgDup, AL_UNDER);

  FieldReal* fr = new FieldReal(root, TR("Limiting &Distance"), &rLimDist, ValueRangeReal(0, 1e6, 0));
  fr->Align(fsv, AL_UNDER);
  new FieldRangeInt(root, TR("&Min, max nr of points"), &riMinMax, ValueRange(1,100));


  initMapOutValRange(true);
  CheckBox* cbEr = new CheckBox(root, TR("&Error Map"), &fErrorMap);
  //new FieldMapCreate(cbEr, "", &sErrorMap);

  SetHelpItem("ilwisapp\\anisotropic_kriging_dialog_box.htm");
  create();
}                    

int FormMapAnisotropicKriging::MapCallBack(Event*)
{
	FormRasterize::MapCallBack(0);
  if (sMap.length() == 0)
    return 0;
  try {
    PointMap pmap(sMap);
    vr = MapKriging::vrDefault(pmap);
    SetDefaultValueRange(vr);
  }
	catch (...) {}  
	return 1;  
}

int FormMapAnisotropicKriging::exec()
{
  FormRasterize::exec();
  String sExpr;
  String sSmv = smv.sExpression();
  String sDupRem;
  FileName fnErr(sErrorMap);
  FileName fnOutMap(sMap);
  sErrorMap = fnErr.sRelativeQuoted(false,fnOutMap.sPath());
  if (fDuplicRemoval)
    sDupRem = String("%s,%.4f",
                     iDuplicRemoval ? "firstval" : "average" ,
                     rTolerance);
  else
    sDupRem = "no";

  int iMin = riMinMax.iLo();
  int iMax = riMinMax.iHi();
	sErrorMap = fErrorMap ? String("1") : String("0");
  sExpr = String("MapKrigingAnisotropic(%S,%S,%S,%f,%.2f,%.2f,%S,%i,%i,%S)",
                     sMap, sGeoRef, sSmv, rLimDist,
                     rAngle, rRatio,
                     sErrorMap, iMin, iMax, sDupRem);
  execMapOut(sExpr);
  return 0;
}


LRESULT Cmdcokriging(CWnd *wnd, const String& s)
{
	new FormMapCoKriging(wnd, s.c_str());
	return -1;
}

FormMapCoKriging::FormMapCoKriging(CWnd* mw, const char* sPar)
: FormRasterize(mw, TR("CoKriging"))
{
  fWiderValRange = true;
  fDuplicRemoval = true;
  iDuplicRemoval = 0;
  rLimDist = 1000;
  riMinMax = RangeInt(1,16);
  rTolerance = 0.1;
  fErrorMap = false;
	fSphericalDistance = false;

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
      else if (sMap == "" && fn.sExt == "" || fn.sExt == ".mpp")
        sMap = fn.sFullNameQuoted(false);
      else if (sMapCoVar == "" && fn.sExt == "" || fn.sExt == ".mpp")
        sMapCoVar = fn.sFullNameQuoted(false);
      else if (sOutMap == "" && fn.sExt == "" || fn.sExt == ".mpr") 
        sOutMap = fn.sFullName(false);
    }
  }
  feMap = new FieldPointMap(root, TR("&Predictand"), &sMap, new MapListerDomainType(".mpp",dmVALUE, true));
	initAsk(dmVALUE);
	feMap->SetCallBack((NotifyProc)&FormMapCoKriging::MapCallBack);

	new FieldPointMap(root, TR("&Covariable"), &sMapCoVar, new MapListerDomainType(".mpp",dmVALUE, true));

  as[0] = TR("&SemiVariogram");
  as[1] = TR("CoVariogram");
  as[2] = TR("CrossVariogram");
  FieldSemiVariogramList* fsvl = new FieldSemiVariogramList(root, 3, as, smv);
  fsvl->SetIndependentPos();

  FieldReal* fr = new FieldReal(root, TR("Limiting &Distance"), &rLimDist, ValueRangeReal(0, 1e6, 0));
  fr->Align(fsvl, AL_UNDER);
  new FieldRangeInt(root, TR("&Min, max nr of points"), &riMinMax, ValueRange(1,100));
  CheckBox* cb = new CheckBox(root, TR("&Remove Duplicates"), &fDuplicRemoval);
  FieldGroup* fgRemDup = new FieldGroup(cb);
  RadioGroup* rgDup = new RadioGroup(fgRemDup, "", &iDuplicRemoval,true);
  rgDup->Align(cb, AL_AFTER);
  new RadioButton(rgDup, TR("&Average"));
  new RadioButton(rgDup, TR("&First Value"));
  FieldReal* frTol = new FieldReal(fgRemDup, TR("&Tolerance (m)"), &rTolerance, ValueRange(0,1e12,0.1));
  frTol->Align(cb, AL_UNDER);
	CheckBox* cbSphDist = new CheckBox(root, TR("Use &Spherical Distance"), &fSphericalDistance);
	cbSphDist->SetIndependentPos();
	
  initMapOutValRange(true);
  CheckBox* cbEr = new CheckBox(root, TR("&Error Map"), &fErrorMap);
  //new FieldMapCreate(cbEr, "", &sErrorMap);

  SetHelpItem("ilwisapp\\cokriging_dialog_box.htm");
  create();
}                    

int FormMapCoKriging::MapCallBack(Event*)
{
	FormRasterize::MapCallBack(0);
  if (sMap.length() == 0)
    return 0;
  try {
    PointMap pmap(sMap);
    vr = MapCoKriging::vrDefault(pmap);
    SetDefaultValueRange(vr);
  }
	catch (...) {}  
	return 1;  
}

int FormMapCoKriging::exec()
{
  FormRasterize::exec();
  String sExpr;
  String sSmvA = smv[0].sExpression();
  String sSmvB = smv[1].sExpression();
  String sSmvAB = smv[2].sExpression();
  String sDupRem;
	String sDistMeth = String("%s", fSphericalDistance ? "sphere" : "plane");
	
  FileName fn(sMap);

  FileName fnMapCoVar(sMapCoVar);
  sMapCoVar = fnMapCoVar.sRelativeQuoted(false,fn.sPath());
  //PointMap mp(fnMapCoVar);

  FileName fnErr(sErrorMap);
  sErrorMap = fnErr.sRelativeQuoted(false,fn.sPath());
  if (fDuplicRemoval)
    sDupRem = String("%s,%.4f",
                     iDuplicRemoval ? "firstval" : "average" ,
                     rTolerance);
  else
    sDupRem = "no";
  int iMin = riMinMax.iLo();
  int iMax = riMinMax.iHi();
	sErrorMap = fErrorMap ? String("1") : String("0");
  sExpr = String("MapCoKriging(%S,%S,%S,%S,%S,%S,%f,%S,%i,%i,%S,%S)",
                     sMap, sMapCoVar, sGeoRef, sSmvA, sSmvB, sSmvAB, rLimDist,
                     sErrorMap, iMin, iMax, sDupRem, sDistMeth);
  execMapOut(sExpr);
  return 0;
}
