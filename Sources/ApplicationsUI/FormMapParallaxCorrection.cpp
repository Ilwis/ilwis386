#include "Client\Headers\AppFormsPCH.h"
#include "Client\Forms\frmmapcr.h"
//#include "client\formelements\fldlist.h"
#include "ApplicationsUI\FormMapParallaxCorrection.h"
#include "Headers\messages.h"
#include "Client\ilwis.h"
#include "Engine\SpatialReference\Csproj.h"
#include "Engine\SpatialReference\prj.h"
#include "Headers\constant.h"

LRESULT Cmdcorrectparallax(CWnd *wnd, const String& s)
{
	new FormCorrectParallaxMap(wnd, s.c_str());
	return -1;
}

FormCorrectParallaxMap::FormCorrectParallaxMap(CWnd* mw, const char* sPar)
: FormMapCreate(mw, TR("Parallax correct a Map using a DTM"))
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
        if (sMap == "")
          sMap = fn.sFullNameQuoted(true);
        else if (sDem == "")
          sDem = fn.sFullNameQuoted(true);
        else  
          sOutMap = fn.sFullName(false);
    }
  }
  fldMap = new FieldDataType(root, TR("&Raster Map"), &sMap, new MapListerDomainType(0,false,true),true);
  fldMap->SetCallBack((NotifyProc)&FormCorrectParallaxMap::MapCallBack);
  stMapRemark = new StaticText(root, String('x',60));
  stMapRemark->SetIndependentPos();
  fldDem = new FieldDataType(root, TR("&DTM"), &sDem, new MapListerDomainType(dmVALUE),true);
  fldDem->SetCallBack((NotifyProc)&FormCorrectParallaxMap::DemCallBack);
  stDemRemark = new StaticText(root, String('x',60));
  stDemRemark->SetIndependentPos();

  iMeth = 2;
  rg = new RadioGroup(root, TR("&Resampling Method"), &iMeth);
  rg->SetCallBack((NotifyProc)&FormCorrectParallaxMap::MethodCallBack);
  rg->SetIndependentPos();
  rbNearest = new RadioButton(rg, TR("&Nearest Neighbour"));
  rbBiLin = new RadioButton(rg, TR("Bi&linear"));
  rbBiCub = new RadioButton(rg, TR("Bi&cubic"));

  rLatSat = 0;
  rLonSat = 0;
  frLonSat = new FieldReal(root, TR("&Satellite Longitude"), &rLonSat);

  fFill = true;
  cbFill = new CheckBox(root, TR("&Fill Obstructed Pixels"), &fFill);
  initMapOutValRange(false);

  SetHelpItem("ilwisapp\\resample_dialog_box.htm");
  create();
}                    

int FormCorrectParallaxMap::exec() 
{
  FormMapCreate::exec();
  String sExpr;
  FileName fn(sOutMap);
  FileName fnMap(sMap);
  FileName fnDem(sDem);
  fOutMapList = ".mpl" == fnMap.sExt;
  sMap = fnMap.sRelativeQuoted(false,fn.sPath());
  sDem = fnDem.sRelativeQuoted(false,fn.sPath());
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
  String sFill;
  if (fFill)
	  sFill = "fill";
  else
	  sFill = "nofill";
  sExpr = String("MapParallaxCorrection(%S,%S,%S,%lg,%lg,%S)", 
                  sMap,sDem,sMethod,rLatSat,rLonSat,sFill);
  if (fOutMapList)
    sExpr = String("MapListApplic(%S,%S)", sMapList, sExpr);
  execMapOut(sExpr);  
  return 0;
}

int FormCorrectParallaxMap::MethodCallBack(Event*)
{
  rg->StoreData();
  //if (0 == iMeth) {
  //  return 0;
  //}
  fldMap->StoreData();
  FileName fnMap(sMap);
  if (fnMap.sFile == "") {
    stMapRemark->SetVal("");
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

int FormCorrectParallaxMap::MapCallBack(Event*) 
{
	fldMap->StoreData();
	try
	{
		FileName fnMap(sMap);
		if (fnMap.sFile == "")
		{
			stMapRemark->SetVal("");
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
		
		GeoRef gr = mp->gr();
		if (gr.fValid()) {
			CoordSystem cs = gr->cs();
			if (cs.fValid()) {
				CoordSystemProjection * pcs = cs->pcsProjection();
				if (pcs) {
					Projection prj = pcs->prj;
					if (prj.fValid()) {
						frLonSat->SetVal(prj->rParam(pvLON0));
					}
				}
			}
		}
		
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
			stMapRemark->SetVal(TR("Domain Image ranges from 0 to 255"));
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
				sRemark = String(TR("Minimum: %S  Maximum: %S").c_str(),
				dvs.sValue(rr.rLo()), dvs.sValue(rr.rHi()));
			else
			{
				rr = dvs.rrMinMax();
				sRemark = String(TR("Ranges from %S to %S").c_str(),
					dvs.sValue(rr.rLo()), dvs.sValue(rr.rHi()));
			}
			stMapRemark->SetVal(String("%S: %S", TR("Value &Range"), sRemark));
			
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
	}
	catch (ErrorObject& err)
	{
		err.Show();
		fvr->Hide();
	}
	return 0;
}

int FormCorrectParallaxMap::DemCallBack(Event*) 
{
	fldDem->StoreData();
	try
	{
		FileName fnDem(sDem);
		if (fnDem.sFile == "")
		{
			stDemRemark->SetVal("");
			return 0;
		}  
        dem = Map(fnDem);
		Domain dm = dem->dm();
		if (!dm.fValid())
		{
			stDemRemark->SetVal("");
			return 0;
		} 
		String sDomain = dm->sName(true);
		if (dm->pdi())
		{
			stDemRemark->SetVal(TR("Domain Image ranges from 0 to 255"));
		}
		else if (dm->pdvr() || dm->pdvi())
		{
			DomainValueRangeStruct dvs = dem->dvrs();
			RangeReal rr = dem->rrMinMax();
			String sRemark;
			if (rr.fValid())
				sRemark = String(TR("Minimum: %S  Maximum: %S").c_str(),
				dvs.sValue(rr.rLo()), dvs.sValue(rr.rHi()));
			else
			{
				rr = dvs.rrMinMax();
				sRemark = String(TR("Ranges from %S to %S").c_str(),
					dvs.sValue(rr.rLo()), dvs.sValue(rr.rHi()));
			}
			stDemRemark->SetVal(String("%S: %S", TR("Value &Range"), sRemark));
		}
		else
		{
			String sRemark(dm->sTypeName());
			stDemRemark->SetVal(sRemark);
		}
	}
	catch (ErrorObject& err)
	{
		err.Show();
	}
	return 0;
}
