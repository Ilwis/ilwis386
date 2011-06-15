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
/* $Log: /ILWIS 3.0/ApplicationForms/frmmapfp.cpp $
 * 
 * 31    15-02-03 16:46 Hendrikse
 * replaced  by plane and sphere
 * 
 * 30    30-09-02 14:26 Hendrikse
 * implemented sphericaldistance choice in SimpleKriging and in CoKriging
 * 
 * 29    20-09-02 12:59 Hendrikse
 * added use of sDistMeth in FormMapKriging::exec() for sphericDistance
 * option
 * 
 * 28    18-09-02 19:07 Hendrikse
 * removedobsolete parts of outcommented code
 * 
 * 27    18-09-02 19:05 Hendrikse
 * implemented bool fSphericalDistance in new FormMapGriddingWeighted
 * using this flag.
 * and adapted FormMapMovingAverage and FormMapMovingSurface, moving to
 * their common parent the MapCallBack 
 * they make now also use of the parents DistanceMethodCallBack and
 * corresp Checkbox
 * fSphericalDistance  added already to the Krigin variants
 * 
 * 26    30-11-00 17:21 Hendrikse
 * added in FormMapCoKriging 
 * call to initAsk(dmVALUE); and FormRasterize::MapCallBack(0);
 * to activate defaultValuRange setting
 * Removed redundant (?) construction of PoinMap in
 * FormMapCoKriging::exec()
 * 
 * 25    27-11-00 15:26 Koolhoven
 * PointDensity (FormRasPointCount) does no longer accept attribute data,
 * because the value of the map is not used in anyway just the positions
 * so asking for an attribute is superfluous
 * 
 * 24    8-11-00 17:24 Koolhoven
 * use sFullNameQuoted() on input data to ensure selection in forms
 * 
 * 23    10-10-00 11:26 Koolhoven
 * reorganizedFormMapCoKriging, thrown away unsed parts
 * 
 * 22    10/06/00 9:56a Martin
 * covar of cokriging got back its attribcolumns
 * 
 * 21    25-09-00 15:56 Hendrikse
 * Activate the SetCallBack of the different KrigingForms at the right
 * time (after InitAsk)
 * reactivate FormRasterize::MapCallBack(0);
 *  
 * Initialize a value domain as output for all Kriging variants their
 * int FormMapKriging::exec()  functions
 * by means of
 *   sDomain = "value";
 * Thus even a boolean input causes a value domain (changeable) output
 * 
 * 20    21-09-00 11:18a Martin
 * value range is now correctly hidden and shown in point to ras
 * 
 * 19    21-09-00 11:08a Martin
 * removed attrib tables form covariable in cokriging
 * 
 * 18    21-09-00 9:58 Hendrikse
 * implemented MapCallBacks for diff Kriging forms
 * 
 * 17    12-09-00 20:16 Hendrikse
 * debugged replacing sMap by sMapCovar in constructor FormMapCoKriging
 * 
 * 16    6-09-00 19:15 Hendrikse
 * removed  the redundant 0 in  sExpr =
 * String("MapKrigingAnisotropic(%S,%S,%S,%f,0,%.2f,%.....
 * in exec()
 * 
 * 15    30-08-00 2:32p Martin
 * 'attribute table ' nameedits changed/removed
 * 
 * 14    28/08/00 16:13 Willem
 * RasFromPoint now will pass map bounadries to create georef form
 * 
 * 13    5/23/00 12:47p Wind
 * - accept  raster input maps with any domain in FormRasPoint
 * - accept  raster input maps with any domain in FormRasPointCount
 * - accept  raster input maps with any domain in FormNearestPoint
 * 
 * 12    11-05-00 3:57p Martin
 * added 'attribute columns' nameedits
 * 
 * 11    9-05-00 10:52a Martin
 * use of attrib columns in nameedits
 * 
 * 10    28-02-00 11:56 Wind
 * adapted to changes in constructor of FieldColumn
 * 
 * 9     31-01-00 11:27 Hendrikse
 * removed new FieldMapCreate(cbEr, "", &sErrorMap); and related
 * statements in
 * all Kriging Forms  
 * assigned "0" or "1" for sErrormap to use in sExpression in exec
 * functions of these forms
 * 
 * 8     20-12-99 11:56 Wind
 * forgotten stuff from port of 2.23 code
 * 
 * 7     23-11-99 3:00p Martin
 * no more pure virtual calls from the tokenizer
 * 
 * 6     22-11-99 15:39 Hendrikse
 * changed string in FormMapAnisotropicKriging()
 * 
 * 5     9/13/99 12:35p Wind
 * adapted :exec functions to use of quoted file names when building an
 * expression
 * 
 * 4     8-09-99 12:30p Martin
 * Added 2.22 stuff
 * 
 * 3     5-03-99 12:16 Koolhoven
 * Corrected header comments
 * 
 * 2     3/04/99 10:16a Martin
 * show as commandline statement instead of through flags
// Revision 1.3  1998/09/16 17:33:54  Wim
// 22beta2
//
// Revision 1.2  1997/08/13 10:21:23  Wim
// *** empty log message ***
//
/* Form Map Applications from Points
   by Wim Koolhoven
   (c) Ilwis System Development ITC
	Last change:  WK   24 Aug 98    4:33 pm
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
	new FormRasPoint(wnd, s.scVal());
	return -1;
}

FormRasPoint::FormRasPoint(CWnd* mw, const char* sPar)
: FormRasterize(mw, SAFTitleRasPntMap)
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
  feMap = new FieldPointMap(root, SAFUiPntMap, &sMap, new MapListerDomainType(".mpp", 0, true));
  feMap->SetCallBack((NotifyProc)&FormRasterize::MapCallBack);
	initAsk( dmVALUE | dmCLASS | dmIDENT | dmBOOL | dmSTRING | dmUNIQUEID);
  void *adress=&feMap;

  iPointSize = 1;
  new FieldInt(root, SAFUiPointSize, &iPointSize);
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
	new FormRasPointCount(parent, s.scVal());
	return -1;
}

FormRasPointCount::FormRasPointCount(CWnd* mw, const char* sPar)
: FormRasterize(mw, SAFTitleRasPntDensity)
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
  feMap = new FieldPointMap(root, SAFUiPntMap, &sMap);
  initAsk(dmVALUE|dmCLASS|dmIDENT|dmBOOL);

  iPointSize = 1;
  new FieldInt(root, SAFUiPointSize, &iPointSize);
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
	new FormMapNearestPoint(wnd, s.scVal());
	return -1;
}

FormMapNearestPoint::FormMapNearestPoint(CWnd* mw, const char* sPar)
: FormRasterize(mw, SAFTitleInterpolNearestPoint)
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
  feMap = new FieldPointMap(root, SAFUiPntMap, &sMap, new MapListerDomainType(".mpp", 0, true));
	initAsk(dmVALUE|dmCLASS|dmIDENT|dmBOOL);
	CheckBox* cb = new CheckBox(root, SAFUiSphericalDist, &fSphericalDistance);
	cb->SetIndependentPos();
  initMapOutValRange(true);
  SetAppHelpTopic("	ilwisapp\nearest_point_dialogbox.htm");
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
  feMap = new FieldPointMap(root, SAFUiPntMap, &sMap, new MapListerDomainType(".mpp", dmVALUE, true));
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
    RadioGroup* rg = new RadioGroup(root, SAFUiWeightFunction, &iWeight);
    rg->SetIndependentPos();
    new RadioButton(rg, SAFUiInvDist);
    new RadioButton(rg, SAFUiLinDecr);
//    Domain dm(0, 1e6, 0);
    new FieldReal(root, SAFUiPowerValue, &rExp, ValueRangeReal(0, 1e6, 0));
    frLimDist = new FieldReal(root, SAFUiLimDist, &rLimDist, ValueRangeReal(0, 1e8, 1));
		fSphericalDistance = false;
		cbSphDis = new CheckBox(root, SAFUiSphericalDist, &fSphericalDistance);
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
	new FormMapMovingAverage(wnd, s.scVal());
	return -1;
}

FormMapMovingAverage::FormMapMovingAverage(CWnd* mw, const char* sPar)
: FormMapGriddingWeighted(mw, SAFTitleInterpolMovAvg, sPar)
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
    ose->AddString(SAFUiPlane.scVal());
    ose->AddString(SAFUi2Linear.scVal());	    
    ose->AddString(SAFUi2Parabolic.scVal());	    
    ose->AddString(SAFUi2Degree.scVal());	    
    ose->AddString(SAFUi3Degree.scVal());	    
    ose->AddString(SAFUi4Degree.scVal());	    
    ose->AddString(SAFUi5Degree.scVal());	    
    ose->AddString(SAFUi6Degree.scVal());	    
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
	new FormMapMovingSurface(wnd, s.scVal());
	return -1;
}

FormMapMovingSurface::FormMapMovingSurface(CWnd* mw, const char* sPar)
: FormMapGriddingWeighted(mw, SAFTitleInterpolMovSurface, sPar)
{
  fWiderValRange = true;
  iSurface = 0;
  FieldSurface* fs = new FieldSurface(root, SAFUiSurface, &iSurface);
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
	new FormMapTrendSurface(wnd, s.scVal());
	return -1;
}

FormMapTrendSurface::FormMapTrendSurface(CWnd* mw, const char* sPar)
: FormMapGridding(mw, SAFTitleInterpolTrendSurface, sPar)
{
  fWiderValRange = true;
  iSurface = 3;
  new FieldSurface(root, SAFUiSurface, &iSurface);
/*  
  RadioGroup* rg = new RadioGroup(root, SAFUiSurface, &iSurface);
  new RadioButton(rg, SAFUiPlane);
  new RadioButton(rg, SAFUi2Linear);
  new RadioButton(rg, SAFUi2Parabolic);
  new RadioButton(rg, SAFUi2Degree);
  new RadioButton(rg, SAFUi3Degree);
  new RadioButton(rg, SAFUi4Degree);
  new RadioButton(rg, SAFUi5Degree);
  new RadioButton(rg, SAFUi6Degree);
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
	new FormMapKriging(wnd, s.scVal());
	return -1;
} 

FormMapKriging::FormMapKriging(CWnd* mw, const char* sPar)
: FormRasterize(mw, SAFTitleMapKriging)
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
  feMap = new FieldPointMap(root, SAFUiPntMap, &sMap, new MapListerDomainType(".mpp", dmVALUE|dmBOOL, true));
  initAsk(dmVALUE);
	feMap->SetCallBack((NotifyProc)&FormMapKriging::MapCallBack);

  FieldSemiVariogram* fsv = new FieldSemiVariogram(root, SAFUiSemiVar, &smv);
  fsv->SetIndependentPos();
  RadioGroup* rg = new RadioGroup(root, SAFUiMethod, &iSimpleOrdinary);
  rg->Align(fsv, AL_AFTER);
  new RadioButton(rg, SAFUiSimpleKriging);
  RadioButton* rbOrdinary = new RadioButton(rg, SAFUiOrdinaryKriging);

  FieldGroup* fgOrdinary = new FieldGroup(rbOrdinary);
  fgOrdinary->Align(fsv, AL_UNDER);
  new FieldReal(fgOrdinary, SAFUiLimDist, &rLimDist, ValueRangeReal(0, 1e6, 0));
  new FieldRangeInt(fgOrdinary, SAFUiMinMaxNrPnts, &riMinMax, ValueRange(1,100));

  CheckBox* cb = new CheckBox(root, SAFUiRemoveDuplicates, &fDuplicRemoval);
  FieldGroup* fgRemDup = new FieldGroup(cb);
  RadioGroup* rgDup = new RadioGroup(fgRemDup, "", &iDuplicRemoval,true);
  rgDup->Align(cb, AL_AFTER);
  new RadioButton(rgDup, SAFUiRemDupAverage);
  new RadioButton(rgDup, SAFUiRemDupFirstValue);
  FieldReal* frTol = new FieldReal(fgRemDup, SAFUiRemDupToleranceM, &rTolerance, ValueRange(0,1e12,0.1));
  frTol->Align(cb, AL_UNDER);

	CheckBox* cbSphDist = new CheckBox(root, SAFUiSphericalDist, &fSphericalDistance);
	cbSphDist->SetIndependentPos();
  initMapOutValRange(true);
  CheckBox* cbEr = new CheckBox(root, SAFUiErrorMap, &fErrorMap);

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
	new FormMapUniversalKriging(wnd, s.scVal());
	return -1;
} 

FormMapUniversalKriging::FormMapUniversalKriging(CWnd* mw, const char* sPar)
: FormRasterize(mw, SAFTitleMapUniversalKriging)
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
  feMap = new FieldPointMap(root, SAFUiPntMap, &sMap, new MapListerDomainType(".mpp", dmVALUE, true));
  initAsk(dmVALUE);
	feMap->SetCallBack((NotifyProc)&FormMapUniversalKriging::MapCallBack);

  rgTrend = new RadioGroup(root, SAFUiTrend, &iTrend, true);
  rgTrend->SetCallBack((NotifyProc)&FormMapUniversalKriging::TrendCallBack);
  rgTrend->SetIndependentPos();
  new RadioButton(rgTrend, "&1");
  new RadioButton(rgTrend, "&2");

  FieldSemiVariogram* fsv = new FieldSemiVariogram(root, SAFUiSemiVar, &smv);
  fsv->SetIndependentPos();

  CheckBox* cb = new CheckBox(root, SAFUiRemoveDuplicates, &fDuplicRemoval);
  cb->Align(fsv, AL_AFTER);
  FieldGroup* fgRemDup = new FieldGroup(cb);
  RadioGroup* rgDup = new RadioGroup(fgRemDup, "", &iDuplicRemoval,true);
  rgDup->Align(cb, AL_UNDER);
  new RadioButton(rgDup, SAFUiRemDupAverage);
  new RadioButton(rgDup, SAFUiRemDupFirstValue);
  FieldReal* frTol = new FieldReal(fgRemDup, SAFUiRemDupToleranceM, &rTolerance, ValueRange(0,1e12,0.1));
  frTol->Align(rgDup, AL_UNDER);

  FieldReal* fr = new FieldReal(root, SAFUiLimDist, &rLimDist, ValueRangeReal(0, 1e6, 0));
  fr->Align(fsv, AL_UNDER);
  fmm = new FieldRangeInt(root, SAFUiMinMaxNrPnts, &riMinMax, ValueRange(3,100));

  initMapOutValRange(true);
  CheckBox* cbEr = new CheckBox(root, SAFUiErrorMap, &fErrorMap);
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
	new FormMapAnisotropicKriging(wnd, s.scVal());
	return -1;
} 

FormMapAnisotropicKriging::FormMapAnisotropicKriging(CWnd* mw, const char* sPar)
: FormRasterize(mw, SAFTitleMapAnisotropicKriging)
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
  feMap = new FieldPointMap(root, SAFUiPntMap, &sMap, new MapListerDomainType(".mpp", dmVALUE, true));
  initAsk(dmVALUE);
	feMap->SetCallBack((NotifyProc)&FormMapAnisotropicKriging::MapCallBack);

  FieldGroup* fg = new FieldGroup(root);
  fg->SetIndependentPos();
  FieldReal* frAngle = new FieldReal(fg, SAFUiAngle, &rAngle, ValueRange(-360,360,0.01));
  FieldReal* frRatio = new FieldReal(fg, SAFUiRatio, &rRatio, ValueRange(1,10,0.01));
  frRatio->Align(frAngle, AL_AFTER);

  FieldSemiVariogram* fsv = new FieldSemiVariogram(root, SAFUiSemiVar, &smv);
  fsv->Align(fg, AL_UNDER);
  fsv->SetIndependentPos();

  CheckBox* cb = new CheckBox(root, SAFUiRemoveDuplicates, &fDuplicRemoval);
  cb->Align(fsv, AL_AFTER);
  FieldGroup* fgRemDup = new FieldGroup(cb);
  RadioGroup* rgDup = new RadioGroup(fgRemDup, "", &iDuplicRemoval,true);
  rgDup->Align(cb, AL_UNDER);
  new RadioButton(rgDup, SAFUiRemDupAverage);
  new RadioButton(rgDup, SAFUiRemDupFirstValue);
  FieldReal* frTol = new FieldReal(fgRemDup, SAFUiRemDupToleranceM, &rTolerance, ValueRange(0,1e12,0.1));
  frTol->Align(rgDup, AL_UNDER);

  FieldReal* fr = new FieldReal(root, SAFUiLimDist, &rLimDist, ValueRangeReal(0, 1e6, 0));
  fr->Align(fsv, AL_UNDER);
  new FieldRangeInt(root, SAFUiMinMaxNrPnts, &riMinMax, ValueRange(1,100));


  initMapOutValRange(true);
  CheckBox* cbEr = new CheckBox(root, SAFUiErrorMap, &fErrorMap);
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
	new FormMapCoKriging(wnd, s.scVal());
	return -1;
}

FormMapCoKriging::FormMapCoKriging(CWnd* mw, const char* sPar)
: FormRasterize(mw, SAFTitleMapCoKriging)
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
  feMap = new FieldPointMap(root, SAFUiPredictand, &sMap, new MapListerDomainType(".mpp",dmVALUE, true));
	initAsk(dmVALUE);
	feMap->SetCallBack((NotifyProc)&FormMapCoKriging::MapCallBack);

	new FieldPointMap(root, SAFUiCoVariable, &sMapCoVar, new MapListerDomainType(".mpp",dmVALUE, true));

  as[0] = SAFUiSemiVar;
  as[1] = SAFUiSemiVarCoVar;
  as[2] = SAFUiCrossVar;
  FieldSemiVariogramList* fsvl = new FieldSemiVariogramList(root, 3, as, smv);
  fsvl->SetIndependentPos();

  FieldReal* fr = new FieldReal(root, SAFUiLimDist, &rLimDist, ValueRangeReal(0, 1e6, 0));
  fr->Align(fsvl, AL_UNDER);
  new FieldRangeInt(root, SAFUiMinMaxNrPnts, &riMinMax, ValueRange(1,100));
  CheckBox* cb = new CheckBox(root, SAFUiRemoveDuplicates, &fDuplicRemoval);
  FieldGroup* fgRemDup = new FieldGroup(cb);
  RadioGroup* rgDup = new RadioGroup(fgRemDup, "", &iDuplicRemoval,true);
  rgDup->Align(cb, AL_AFTER);
  new RadioButton(rgDup, SAFUiRemDupAverage);
  new RadioButton(rgDup, SAFUiRemDupFirstValue);
  FieldReal* frTol = new FieldReal(fgRemDup, SAFUiRemDupToleranceM, &rTolerance, ValueRange(0,1e12,0.1));
  frTol->Align(cb, AL_UNDER);
	CheckBox* cbSphDist = new CheckBox(root, SAFUiSphericalDist, &fSphericalDistance);
	cbSphDist->SetIndependentPos();
	
  initMapOutValRange(true);
  CheckBox* cbEr = new CheckBox(root, SAFUiErrorMap, &fErrorMap);
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
