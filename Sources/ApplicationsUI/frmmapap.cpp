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

//#include "Client\MainWindow\mainwind.h"
#include "Client\Headers\AppFormsPCH.h"
#include "Client\FormElements\fldlist.h"
#include "Client\FormElements\fldcs.h"
#include "Client\FormElements\fldfil.h"
#include "Client\FormElements\fldtbl.h"
#include "Client\FormElements\fldmsmpl.h"
#include "Engine\Map\Raster\MAPSTORE.H"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Domain\Dmvalue.h"
#include "ApplicationsUI\frmmapap.h"
#include "Applications\Raster\MAPDIST.H"
#include "Headers\Hs\DOMAIN.hs"
#include "Client\FormElements\fldaggr.h"
#include "Applications\Raster\variosrf.h"
#include "Applications\Raster\variosfm.h"
#include "Applications\Raster\MapKrigingFromRaster.h"
#include "Engine\Table\COLINFO.H"
#include "Client\FormElements\fldsmv.h"
#include "Client\ilwis.h"
#include "Headers\constant.h"

LRESULT Cmddistance(CWnd *parent, const String& s) {
	new FormDistanceMap(parent, s.c_str());
	return -1;
}

FormDistanceMap::FormDistanceMap(CWnd* mw, const char* sPar)
: FormMapCreate(mw, TR("Distance Calculation"))
{
  fWeightMap = false;
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
      if (fn.sExt == "" || fn.sExt == ".mpr")
        if (sSourceMap == "")
          sSourceMap = fn.sFullNameQuoted(true);
        else if (sWeightMap == "") {
          fWeightMap = true;
          sWeightMap = fn.sFullNameQuoted(true);
        }  
        else  
          sOutMap = fn.sFullName(false);
    }
  }
  sDomain = "distance.dom";
  fldSourceMap = new FieldDataType(root, TR("&Source Map"), &sSourceMap, 
                                   new MapListerDomainType(".mpr", dmCLASS|dmIDENT|dmBOOL|dmVALUE, true), true);
  fldSourceMap->SetCallBack((NotifyProc)&FormDistanceMap::CallBack);
  CheckBox* cb = new CheckBox(root, TR("&Weight Map"), &fWeightMap);
  fldWeightMap = new FieldDataType(cb, "", &sWeightMap, new MapListerDomainType(".mpr", dmVALUE, true), true);
  fldWeightMap->SetCallBack((NotifyProc)&FormDistanceMap::WeightMapCallBack);
  stWeightMapRemark = new StaticText(root, String('x',50));
  stWeightMapRemark->SetIndependentPos();
  stWeightMapRemark->Align(cb, AL_UNDER);
  
  initMapOut(false, (long)dmVALUE);

  fThiessenMap = false;
  CheckBox* cbTm = new CheckBox(root, TR("&Thiessen Map"), &fThiessenMap);
  new FieldMapCreate(cbTm, "", &sThiessenMap);
  
  SetHelpItem("ilwisapp\\distance_calculation_dialogbox.htm");
  create();
}                    

int FormDistanceMap::exec() 
{
  FormMapCreate::exec();
  String sExpr;
  FileName fn(sOutMap);
  FileName fnSourceMap(sSourceMap); 
  FileName fnWeightMap(sWeightMap); 
  sSourceMap = fnSourceMap.sRelativeQuoted(false,fn.sPath());
  sWeightMap = fnWeightMap.sRelativeQuoted(false,fn.sPath());
  if (fThiessenMap) {
    FileName fnThiessenMap(sThiessenMap);
    sThiessenMap = fnThiessenMap.sRelativeQuoted(false,fn.sPath());
    if (fWeightMap)
      sExpr = String("MapDistance(%S,%S,%S)", 
                      sSourceMap, sWeightMap, sThiessenMap);
    else                  
      sExpr = String("MapDistance(%S,1,%S)", 
                    sSourceMap, sThiessenMap);
  }
  else 
    if (fWeightMap)
      sExpr = String("MapDistance(%S,%S)", 
                      sSourceMap, sWeightMap);
    else                  
      sExpr = String("MapDistance(%S)", 
                    sSourceMap);
  execMapOut(sExpr);  
  return 0;
}

int FormDistanceMap::CallBack(Event*) 
{
  fldSourceMap->StoreData();
  try {
    FileName fnMap(sSourceMap);
    if (fnMap.sFile == "")
      return 0;
    Map mp(fnMap);
    ValueRange vr = MapDistance::vrDefault(mp);
    SetDefaultValueRange(vr);
	if (fgr) fgr->SetBounds(mp->cs(), mp->cb());
  }
  catch (ErrorObject&) {}
  return 0;
}

int FormDistanceMap::WeightMapCallBack(Event*) 
{
  fldWeightMap->StoreData();
  try {
    FileName fnMap(sWeightMap);
    if (fnMap.sFile == "") {
      stWeightMapRemark->SetVal("");
      return 0;
    }  
    Map mp(fnMap);
    DomainValueRangeStruct dvs = mp->dvrs();
    RangeReal rr = mp->rrMinMax();
    String sRemark(TR("Minimum: %S  Maximum: %S").c_str(),
               dvs.sValue(rr.rLo()), dvs.sValue(rr.rHi()));
    stWeightMapRemark->SetVal(sRemark);
  }
  catch (ErrorObject&) {
    stWeightMapRemark->SetVal("");
  }
  return 0;
}
LRESULT Cmdeffectdistance(CWnd *parent, const String& s) {
	new FormEffectDistanceMap(parent, s.c_str());
	return -1;
}

void FormEffectDistanceMap::initWeights()
{
	m_rNorth = 0;
	m_rNorthEast = 0;
	m_rEast = 0;
	m_rSouthEast = 0;
	m_rSouth = 0;
	m_rSouthWest = 0;
	m_rWest = 0;
	m_rNorthWest = 0;
}

FormEffectDistanceMap::FormEffectDistanceMap(CWnd* mw, const char* sPar)
: FormMapCreate(mw, "Generate Effect Distance Map")
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
      if (fn.sExt == "" || fn.sExt == ".mpr")
        if (sSourceMap == "")
          sSourceMap = fn.sFullNameQuoted(true);
        else  
          sOutMap = fn.sFullName(false);
    }
  }
  sDomain = "distance.dom";
  FieldMap *fm = new FieldMap(root, TR("&Source Map"), &sSourceMap);
  fm->SetCallBack((NotifyProc)&FormEffectDistanceMap::OnSelect);
  ValueRange vr1(0, 1e300, 1e-2);
  FieldReal* fldMaxDist = new FieldReal(root, "Max. distance", &m_rDist, vr1);
  
  initMapOut(false, false);
  initWeights();
  m_fDirectionFromSource = false;
  m_chkDirectionFromSource = new CheckBox(root, "Direction from source", &m_fDirectionFromSource);
  //m_chkDirectionFromSource->SetCallBack((NotifyProc)&FormEffectDistanceMap::OnDirectionSelect);
  FieldMapCreate* fmc = new FieldMapCreate(m_chkDirectionFromSource, "", &m_sDirectionFromSourceMap);
  StaticText *stDirection = new StaticText(m_chkDirectionFromSource,"Direction");
  stDirection->Align(m_chkDirectionFromSource, AL_UNDER);
  StaticText *stWeight = new StaticText(m_chkDirectionFromSource,"Weight");
  stWeight->Align(stDirection, AL_AFTER);
  FieldReal* fldNorth = new FieldReal(m_chkDirectionFromSource, "North", &m_rNorth, vr1);
  fldNorth->Align(stDirection, AL_UNDER);
  FieldReal* fldNorthEast = new FieldReal(m_chkDirectionFromSource, "North East", &m_rNorthEast, vr1);	
  fldNorthEast->Align(fldNorth, AL_UNDER);
  FieldReal* fldEast = new FieldReal(m_chkDirectionFromSource, "East", &m_rEast, vr1);	
  fldEast->Align(fldNorthEast, AL_UNDER);
  FieldReal* fldSouthEast = new FieldReal(m_chkDirectionFromSource, "South East", &m_rSouthEast, vr1);	
  fldSouthEast->Align(fldEast, AL_UNDER);
  FieldReal* fldSouth = new FieldReal(m_chkDirectionFromSource, "South", &m_rSouth, vr1);	
  fldSouth->Align(fldSouthEast, AL_UNDER);
  FieldReal* fldSouthWest = new FieldReal(m_chkDirectionFromSource, "South West", &m_rSouthWest, vr1);	
  fldSouthWest->Align(fldSouth, AL_UNDER);
  FieldReal* fldWest = new FieldReal(m_chkDirectionFromSource, "West", &m_rWest, vr1);	
  fldWest->Align(fldSouthWest, AL_UNDER);
  FieldReal* fldNorthWest = new FieldReal(m_chkDirectionFromSource, "NorthWest", &m_rNorthWest, vr1);	
  fldNorthWest->Align(fldWest, AL_UNDER);
  //m_AddString = false;
  //m_fsDirection = new FieldOneSelectTextOnly(m_chkDirectionFromSource, &m_sDirection, false);
  //m_fsDirection->Align(stDirection, AL_AFTER);
  m_fUpslopeFromSource = false;
  m_chkUpslopeFromSource = new CheckBox(root, "Upslope from source", &m_fUpslopeFromSource);
  m_chkUpslopeFromSource->SetCallBack((NotifyProc)&FormEffectDistanceMap::OnUpslopeSelect);
  FieldMapCreate* fmcUpslope = new FieldMapCreate(m_chkUpslopeFromSource, "", &m_sUpslopeFromSourceMap);
  m_fDownslopeFromSource = false;
  m_chkDownslopeFromSource = new CheckBox(root, "Downslope from source", &m_fDownslopeFromSource);
  m_chkDownslopeFromSource->SetCallBack((NotifyProc)&FormEffectDistanceMap::OnDownslopeSelect);
  m_chkDownslopeFromSource->Align(m_chkUpslopeFromSource, AL_UNDER);
  FieldMapCreate* fmcDownslope = new FieldMapCreate(m_chkDownslopeFromSource, "", &m_sDownslopeFromSourceMap);
  m_fldDEMMap = new FieldDataType(root, "Input DEM raster map", &m_sDEM, 
                                   new MapListerDomainType(".mpr", dmVALUE, true), true);
  m_fldDEMMap->Align(m_chkDownslopeFromSource, AL_UNDER);
  m_fldDEMMap->Hide();
  FieldBlank *fb1 = new FieldBlank(m_chkDirectionFromSource, 1);
  fb1->Align(stDirection, AL_UNDER);

  //ValueRange vr1(0, 1e300, 1e-6);
  //FieldReal* fldDist = new FieldReal(m_chkDirectionFromSource, "Distance", &m_rDist, vr1);
  //fldDist->Align(fb1, AL_UNDER);
  create();
}                    
unsigned int GetMemory()
{
	MEMORYSTATUS memoryStatus;

	memset (&memoryStatus, sizeof (MEMORYSTATUS), 0);
	memoryStatus.dwLength = sizeof (MEMORYSTATUS);

	GlobalMemoryStatus (&memoryStatus);
	return memoryStatus.dwTotalPhys;

}
int FormEffectDistanceMap::exec() 
{
  /**
	 * compute the distance from source, direction from source.
	 * @param arg[0] - input raster map. It is expected to be a GDAL supported raster format.    
	 * @param arg[1] - output format, default is ILWIS. use the short format name defined by GDAL.
	 * @param arg[2] - a value to indicate the maximum distance of pixels from source that shoul be added to the output maps.
	 * @param arg[3] - output map for distance from source
	 * @param arg[4] - string indicating how direction from source should be calculated
	 * @param arg[5] - output map for direction from source
	 * @param arg[6] - flag for upslope from source
	 * @param arg[7] - output map for upslope distance from source
	 * @param arg[8] - flag for downslope from source
	 * @param arg[9] - output map for downslope distance from source
	 * @param arg[10] - input DEM map
	 * @param arg[11] - pixel size of the input raster map
	*/	
  FormMapCreate::exec();
  String sExpr;
  FileName fn(sOutMap, ".mpr", TRUE);
  FileName fnSourceMap(sSourceMap); 
  sSourceMap = fnSourceMap.sRelativeQuoted(false,fn.sPath());
  int cellsize;
  try {
    FileName fnMap(sSourceMap);
    Map mpSource(fnMap);
	if (mpSource.fValid())
		cellsize = mpSource->gr()->rPixSize();
	
  }
  catch (ErrorObject&) {}


  if (fn.fExist()) {
	String sErr(TR("File %S already exists.\nOverwrite?").c_str(), fn.sFullPath(true));
	int iRet=mw->MessageBox(sErr.c_str(), TR("File already exists").c_str(), MB_YESNO|MB_ICONEXCLAMATION);
	if (iRet != IDYES)
		return 1;
  }

  String calcDirection;
  String WeightValues;
  if (m_fDirectionFromSource){
	calcDirection = "yes";
	WeightValues = String("%f, %f, %f, %f, %f, %f, %f, %f", 
						  m_rNorth,
						  m_rNorthEast,
						  m_rEast,
						  m_rSouthEast,
						  m_rSouth,
						  m_rSouthWest,
						  m_rWest,
						  m_rNorthWest);
  }
  else{
	calcDirection = "none";
	m_sDirectionFromSourceMap = "no";
	WeightValues = "no";
  }	
  String sFormat = "ILWIS";
  String calcUpslope;
  String calcDownslope;
  if(m_fUpslopeFromSource)
	  calcUpslope = "yes";
  else{
  	  calcUpslope = "no";
	  m_sUpslopeFromSourceMap = "no";
  }
  
  if(m_fDownslopeFromSource){
  	  calcDownslope = "yes";
  }
  else{
  	  calcDownslope = "no";
	  m_sDownslopeFromSourceMap = "no";	
  }
  if(m_fDownslopeFromSource != true && m_fUpslopeFromSource != true )
	m_sDEM = "no";
  unsigned int memorysize = GetMemory()/1024/1024;
  String mSize;
  if (memorysize > 1000)
	mSize = "1024";
  else if (memorysize  > 500 )
	mSize = "512";
  else
	mSize = "64";
  String sCmd = String("!java '-jar' -vmargs '-Xmx%Sm' '%Seffectdistance.jar' '%S' '%S' %.2f '%S' '%S' '%S' %S '%S' %S '%S' %S %S %li", 
                mSize,
				IlwWinApp()->Context()->sIlwDir(), fnSourceMap.sFullName(), 
				sFormat,
				m_rDist,
				fn.sFullName(),
				calcDirection,
				WeightValues,
				m_sDirectionFromSourceMap,
				calcUpslope,
				m_sUpslopeFromSourceMap,
				calcDownslope,
				m_sDownslopeFromSourceMap,
				m_sDEM,
				cellsize);
  IlwWinApp()->Execute(sCmd);
  // now update the catalog (unless fNoUpdate is set to true)
  bool *fNoUpdate = (bool *)(IlwWinApp()->Context()->pGetThreadLocalVar(IlwisAppContext::tlvNOUPDATECATALOG));
  if (fNoUpdate != 0)  // pointer must be valid
	if (!(*fNoUpdate))
	{
		FileName* pfn = new FileName(sOutMap);
		AfxGetApp()->GetMainWnd()->PostMessage(ILW_READCATALOG, 0, (long)pfn);
	}
  if (fn.fExist()){
    if (sDescr.size()>0){
      Map mpOut(fn);
      //mpOut->SetDescription(sDescr);
      mpOut->sDescription = sDescr;
      mpOut->Store();
    }
    //sCmd = String("open '%S'", fn.sFullName());
	sCmd = String("open %S.mpr", sOutMap);
    IlwWinApp()->Execute(sCmd);
  }
  //butDefine.EnableWindow(FALSE); 
  return 0;
}

int FormEffectDistanceMap::OnSelect(Event *)
{
    butDefine.EnableWindow(FALSE); 
    return 1;
}

int FormEffectDistanceMap::OnUpslopeSelect(Event *)
{
    m_chkUpslopeFromSource->StoreData(); 
	if(m_fUpslopeFromSource || m_fDownslopeFromSource )
		m_fldDEMMap->Show();
	else
		m_fldDEMMap->Hide();
    return 1;
}
int FormEffectDistanceMap::OnDownslopeSelect(Event *)
{
    m_chkDownslopeFromSource->StoreData();
	if(m_fUpslopeFromSource || m_fDownslopeFromSource )
		m_fldDEMMap->Show();
	else
		m_fldDEMMap->Hide();
    return 1;
}
int FormEffectDistanceMap::OnDirectionSelect(Event *)
{
  m_chkDirectionFromSource->StoreData();
  m_fsDirection->StoreData();
  if(m_AddString == false)
  {
    m_fsDirection->AddString("North");
	m_fsDirection->AddString("South");
	m_fsDirection->AddString("East");
	m_fsDirection->AddString("West");
	m_fsDirection->AddString("SouthEast");
	m_fsDirection->AddString("SouthWest");
	m_fsDirection->AddString("NorthEast");
	m_fsDirection->AddString("NorthWest");
    m_fsDirection->SelectItem(m_sDirection);
    m_AddString = true;
  }
  return 1;
}

LRESULT Cmdareanumb(CWnd *parent, const String& s) {
	new FormAreaNumbering(parent, s.c_str());
	return -1;
}

FormAreaNumbering::FormAreaNumbering(CWnd* mw, const char* sPar)
: FormMapCreate(mw, TR("Area Numbering"))
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
      if (fn.sExt == "" || fn.sExt == ".mpr")
        if (sMap == "")
          sMap = fn.sFullNameQuoted(true);
        else  
          sOutMap = fn.sFullName(false);
    }
  }
  fdtMapIn = new FieldDataType(root, TR("&Raster Map"), &sMap,
                    new MapListerDomainType(".mpr", dmCLASS|dmIDENT|dmBOOL, true), true);
  fdtMapIn->SetCallBack((NotifyProc)&FormAreaNumbering::MapCallBack);

  iConnect = 1;
  RadioGroup* rg = new RadioGroup(root, TR("&Connect"), &iConnect);
  rg->SetIndependentPos();
  RadioButton* rb4 = new RadioButton(rg, TR("&4"));
  rb4->Align(rg, AL_AFTER);
  RadioButton* rb8 = new RadioButton(rg, TR("&8"));
  rb8->Align(rb4, AL_AFTER);

  initMapOut(false, false);
//  initRemark();
  SetHelpItem("ilwisapp\\areanumbering_dialogbox.htm");
  create();
}                    

int FormAreaNumbering::MapCallBack(Event*)
{
	fdtMapIn->StoreData();
	try
	{
		FileName fnMap(sMap);
		if (fnMap.sFile == "")
			return 0;

		Map mp(fnMap);
		if (fgr) fgr->SetBounds(mp->cs(), mp->cb());
	}
	catch (ErrorObject&) {}

	return 0;
}

int FormAreaNumbering::exec() 
{
  FormMapCreate::exec();
  String sExpr;
  FileName fn(sOutMap);
  FileName fnMap(sMap); 
  sMap = fnMap.sRelativeQuoted(false,fn.sPath());
  String sConnect;
  switch (iConnect) {
    case 0: sConnect = "4"; break;
    case 1: sConnect = "8"; break;
  }
  sExpr = String("MapAreaNumbering(%S,%S)",
                  sMap, sConnect);
  execMapOut(sExpr);  
  return 0;
}

LRESULT Cmdattribras(CWnd *parent, const String& s) {
	new FormAttributeMap(parent, s.c_str());
	return -1;
}

FormAttributeMap::FormAttributeMap(CWnd* mw, const char* sPar)
: FormMapCreate(mw, TR("Attribute Map of Raster Map"))
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
      if (fn.sExt == "" || fn.sExt == ".mpr")
        if (sMap == "")
          sMap = fn.sFullNameQuoted(false);
        else  
          sOutMap = fn.sFullName(false);
    }
  }
  fldMap = new FieldMap(root, TR("&Raster Map"), &sMap,
  	                    new MapListerDomainType(".mpr", dmCLASS | dmIDENT | dmGROUP | dmBOOL | dmUNIQUEID));
  fldMap->SetCallBack((NotifyProc)&FormAttributeMap::MapCallBack);
  fldTbl = new FieldTable(root, TR("&Table"), &sTbl, ".TBT.HIS.RPR");
  fldTbl->SetCallBack((NotifyProc)&FormAttributeMap::TblCallBack);
  fldCol = new FieldColumn(root, TR("&Attribute"), Table(), &sCol, 
    dmCLASS|dmIDENT|dmUNIQUEID|dmVALUE|dmIMAGE|dmPICT|dmCOLOR|dmBOOL|dmBIT);
  fldCol->SetCallBack((NotifyProc)&FormAttributeMap::ColCallBack);
  stColRemark = new StaticText(root, String('x',50));
  stColRemark->SetIndependentPos();
  initMapOut(false,true);
  SetHelpItem("ilwisapp\\attribute_map_of_raster_map_dialogbox.htm");
  // bad trick to prevent that domain call back changes defaults of column callback
  stRemark->SetCallBack((NotifyProc)&FormAttributeMap::ColCallBack);
  create();
}                    

int FormAttributeMap::exec() 
{
  FormMapCreate::exec();
  String sExpr;
  bool fShort = false;
  FileName fn(sOutMap);
  FileName fnTbl(sTbl);
  FileName fnMap(sMap); 
  Map map(fnMap);
  if (map.fValid()) {
    if (map->fTblAtt()) {
      Table tbl = map->tblAtt();
      fShort = (tbl->fnObj == fnTbl);
    }  
  }    
  sMap = fnMap.sRelativeQuoted(false,fn.sPath());
  sTbl = fnTbl.sRelativeQuoted(false,fn.sPath());
	if ( fnTbl.sExt != ".tbl")
		sTbl += fnTbl.sExt;
	
  if (!fShort)
    sCol = String("%S.%S", sTbl, sCol);
  sExpr = String("MapAttribute(%S,%S)", 
                  sMap,sCol);
  execMapOut(sExpr);  
  return 0;
}

int FormAttributeMap::MapCallBack(Event*)
{
	fldMap->StoreData();
	fldCol->FillWithColumns();
	if (sMap == "")
		return 0;
	
	FileName fnMap(sMap); 
	if (!fnMap.fValid())
		return 0;

	try
	{
		Map map(fnMap);
		if (map.fValid())
		{
			fldTbl->SetDomain(map->dm()->sNameQuoted());
			if (map->fTblAtt())
			{
				//String sName("%S\\%S", map->fnObj.sPath(), map->sTblAtt());
				fldTbl->SetVal(map->tblAtt()->fnObj.sFullPath());
			}
			if (fgr) fgr->SetBounds(map->cs(), map->cb());
		}  
	}
	catch (ErrorObject&) {}

	return 0;
}

int FormAttributeMap::TblCallBack(Event*)
{
  fldTbl->StoreData();
  fldCol->FillWithColumns();
  try {
    FileName fnTbl(sTbl, ".tbt", false); 
    fldCol->FillWithColumns(fnTbl);
    ColCallBack(0);
  }
  catch (ErrorObject&) {}
  return 0;
}

int FormAttributeMap::ColCallBack(Event*)
{
  fldCol->StoreData();
  try {
    FileName fnTbl(sTbl); 
    ColumnInfo colinf(fnTbl, sCol);
    if (!colinf.fValid()) {
      stColRemark->SetVal("");
      fdc->Hide();
      sDomain = "";
      fvr->Hide();
      return 0;
    }
    Domain dm = colinf.dm();
    if (dm->pdvi() || dm->pdvr()) {
      DomainValueRangeStruct dvs = colinf.dvrs();
      fdc->SetVal(dm->sName());
      fdc->Show();
      fvr->Show();
      RangeReal rr = colinf.rrMinMax();
      if (rr.fValid()) {
        String sRemark(TR("Minimum: %S  Maximum: %S").c_str(),
                 dvs.sValue(rr.rLo()), dvs.sValue(rr.rHi()));
        stColRemark->SetVal(sRemark);
        ValueRange vr(rr, dvs.rStep());
        SetDefaultValueRange(vr);
      }
      else {
        RangeReal rr = dvs.rrMinMax();
        String sRemark(TR("Ranges from %S to %S").c_str(), 
                 dvs.sValue(rr.rLo()), dvs.sValue(rr.rHi()));
        stColRemark->SetVal(sRemark);
        SetDefaultValueRange(colinf.vr());
      } 
    }
    else {
      String sRemark(dm->sTypeName());
      stColRemark->SetVal(sRemark);
      fdc->Hide();
      sDomain = dm->sName(true);
      fvr->Hide();
    }
    SetRemarkOnBytesPerPixel();
  }
  catch (ErrorObject&) {
    stColRemark->SetVal("");
    fdc->Hide();
    sDomain = "";
    fvr->Hide();
  }
  return 0;
}

//-----------------------------------------------------------------------------
LRESULT Cmdattribtoras(CWnd *parent, const String& s) {
	new FormAttributeToMap(parent, s.c_str());
	return -1;
}

FormAttributeToMap::FormAttributeToMap(CWnd* mw, const char* sPar)
: FormMapCreate(mw, TR("Setting attribute of rastermap"))
{
	mustLink = true;
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
      if (fn.sExt == "" || fn.sExt == ".mpr")
        if (sMap == "")
          sMap = fn.sFullNameQuoted(false);
        else  
          sOutMap = fn.sFullName(false);
    }
  }
  fldMap = new FieldMap(root, TR("&Raster Map"), &sMap,
  	                    new MapListerDomainType(".mpr", dmVALUE|dmIMAGE));
  //fldMap->SetCallBack((NotifyProc)&FormAttributeToMap::MapCallBack);
  fldTbl = new FieldTable(root, TR("&Table"), &sTbl, ".TBT");
  fldTbl->SetCallBack((NotifyProc)&FormAttributeToMap::TblCallBack);
  fldCol = new FieldColumn(root, TR("&Value attribute"), Table(), &sColSource, 
    dmVALUE|dmIMAGE);
  //fldCol->SetCallBack((NotifyProc)&FormAttributeToMap::ColCallBack);

  fldCol2 = new FieldColumn(root, TR("&Class/ID attribute"), Table(), &sColTarget, 
	  dmCLASS | dmIDENT | dmUNIQUEID);
  //fldCol2->SetCallBack((NotifyProc)&FormAttributeToMap::ColCallBack);

  new CheckBox(root, "Attach table as attribute table", &mustLink);

  //stColRemark = new StaticText(root, String('x',50));
  //stColRemark->SetIndependentPos();
  initMapOut(false,false);
  create();
}                    
int FormAttributeToMap::TblCallBack(Event*)
{
  fldTbl->StoreData();
  fldCol->FillWithColumns();
  fldCol2->FillWithColumns();
  try {
    FileName fnTbl(sTbl, ".tbt", false); 
    fldCol->FillWithColumns(fnTbl);
	fldCol2->FillWithColumns(fnTbl);
  }
  catch (ErrorObject&) {}
  return 0;
}

int FormAttributeToMap::exec() 
{
  FormMapCreate::exec();
  String sExpr;
  bool fShort = false;
  FileName fn(sOutMap);
  FileName fnTbl(sTbl);
  FileName fnMap(sMap); 
  Map map(fnMap);
  sMap = fnMap.sRelativeQuoted(false,fn.sPath());
  sTbl = fnTbl.sRelativeQuoted(false,fn.sPath());
	if ( fnTbl.sExt != ".tbl")
		sTbl += fnTbl.sExt;
	
  sExpr = String("MapToAttribute(%S,%S, %S, %S, %s)", 
	  sMap,sTbl, sColSource, sColTarget, mustLink ? "true" : "false");
  execMapOut(sExpr);  
  return 0;
}




//-----------------------------------------------------------------------

LRESULT Cmdslicing(CWnd *parent, const String& s) {
	new FormMapSlicing(parent, s.c_str());
	return -1;
}

FormMapSlicing::FormMapSlicing(CWnd* mw, const char* sPar)
: FormMapCreate(mw, TR("Slicing"))
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
      if (fn.sExt == ".dom")
        sDomain = fn.sFullNameQuoted(false);
      else if (fn.sExt == "" || fn.sExt == ".mpr")
        if (sMap == "")
          sMap = fn.sFullNameQuoted(true);
        else  
          sOutMap = fn.sFullName(false);
    }
  }
  fdtMapIn = new FieldDataType(root, TR("&Raster Map"), &sMap, new MapListerDomainType(".mpr", dmVALUE|dmIMAGE, true),true);
  fdtMapIn->SetCallBack((NotifyProc)&FormMapSlicing::MapCallBack);

  initMapOut(false, (long)dmGROUP);
  SetHelpItem("ilwisapp\\slicing_dialogbox.htm");
  create();
}                    

int FormMapSlicing::MapCallBack(Event*)
{
	fdtMapIn->StoreData();
	try
	{
		FileName fnMap(sMap);
		if (fnMap.sFile == "")
			return 0;

		Map mp(fnMap);
		if (fgr) fgr->SetBounds(mp->cs(), mp->cb());
	}
	catch (ErrorObject&) {}

	return 0;
}

int FormMapSlicing::exec() 
{
  FormMapCreate::exec();
  String sExpr;
  FileName fn(sOutMap);
  FileName fnMap(sMap); 
  sMap = fnMap.sRelativeQuoted(false,fn.sPath());
  sExpr = String("MapSlicing(%S,%S)", sMap, sDomain);
  execMapOut(sExpr);  
  return 0;
}

LRESULT Cmdaggregate(CWnd *wnd, const String& s)
{
	new FormMapAggregate(wnd, s.c_str());
	return -1;
}

FormMapAggregate::FormMapAggregate(CWnd* mw, const char* sPar)
: FormMapCreate(mw, TR("Aggregate"))
{
  iFactor = 4;
  sFunc = 0;
  m_sDefault = String();
  fGroup = true;
  fOffset = false;
  rcOffset = RowCol(0L,0L);
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
        else  
          sOutMap = fn.sFullName(false);
    }
  }
  fldMap = new FieldDataType(root, TR("&Raster Map"), &sMap, new MapListerDomainType(0,true,true), true);
  fldMap->SetCallBack((NotifyProc)&FormMapAggregate::CallBack);
  fldGrf = new FieldInt(root, TR("&Group Factor"), &iFactor, RangeInt(2,1000), true);
  fldGrf->SetCallBack((NotifyProc)&FormMapAggregate::CallBack);
  fldAgf = new FieldAggrFunc(root, TR("&Function"), &sFunc, m_sDefault);
  fldAgf->SetCallBack((NotifyProc)&FormMapAggregate::CallBack);
  new CheckBox(root, TR("&Group"), &fGroup);
  CheckBox* cb = new CheckBox(root, TR("&Offset"), &fOffset);
  new FieldRowCol(cb, "", &rcOffset);
  FieldBlank* fb = new FieldBlank(root,0);
  fb->Align(cb, AL_UNDER);
  initMapOut(false, true);
  SetHelpItem("ilwisapp\\aggregate_dialog_box.htm");
  create();
}                    

int FormMapAggregate::CallBack(Event*)
{
  fldMap->StoreData();
  fldGrf->StoreData();
  fldAgf->StoreData();
  FileName fnMap(sMap);
  if (fnMap.sFile == "") {
    DisableOK();
    return 0;
  }
  if (0 == sFunc || -1 == (long)sFunc) {
    DisableOK();
    return 0;
  }
  try {
    bool fOk = false;
    Map mp;
    if (".mpl" == fnMap.sExt && "" == fnMap.sSectionPostFix)
    {
      MapList mpl(fnMap);
      mp = mpl->map(mpl->iLower());
    }
    else 
      mp = Map(fnMap);
  	if (fgr) 
      fgr->SetBounds(mp->cs(), mp->cb());

    Domain dm = mp->dm();

    if (*sFunc == "Std" || *sFunc == "Avg" || *sFunc == "Sum") {
      if (dm->pdvi() || dm->pdvr() || dm->pdi()) {
        fOk = true;
				stRemark->SetVal("");
			}
      else
        stRemark->SetVal(TR("Map should have a domain value with this function"));
    }
    else if (*sFunc == "Med" || *sFunc == "Min" || *sFunc == "Max") {
      if (dm->pdvi() || dm->pdvr() || dm->pdi() || dm->pdsrt()) {
        fOk = true;
				stRemark->SetVal("");
			}
      else
        stRemark->SetVal(TR("Map should have a sortable domain with this function"));
    }
    else {
      fOk = true;
      stRemark->SetVal("");
    }
    if (fOk)
      EnableOK();
    else {
      DisableOK();
      fdc->Hide();
      fvr->Hide();
      return 0;
    }

    ValueRange valr = vr;
    if (*sFunc == "Cnt") {
      fdc->SetVal("count");
      int iMax = iFactor * iFactor;
      valr = ValueRange(0, iMax, 1);
      fdc->Show();
      SetDefaultValueRange(valr);
    }
    else {
      sDomain = mp->dm()->sName();
      valr = mp->vr();
      bool fImage = 0 != dm->pdi();
      if (fImage)
        valr = ValueRange(0,255);
      if (valr.fValid()) {
        RangeReal rr = mp->rrMinMax();
        if (!rr.fValid())
          rr = valr->rrMinMax();
        double rStep = valr->rStep();
        if (*sFunc == "Avg") {
          int iFact = iFactor * iFactor;
          if (iFact > 100)
            iFact = 100;
          rStep /= iFact;
        }
        if (*sFunc == "Std") {
          rStep /= 100;
          rr = RangeReal(0, rr.rWidth() / 10);
          sDomain = "value.dom";
          fImage = false;
        }
        if (*sFunc == "Sum") {
          rr.rLo() *= iFactor * iFactor;
          rr.rHi() *= iFactor * iFactor;
          sDomain = "value.dom";
          fImage = false;
        }
        valr = ValueRange(rr.rLo(), rr.rHi(), rStep);
      }
      if (*sFunc == "Min" || *sFunc == "Max" ||
          *sFunc == "Prd" || *sFunc == "Med") {
        fdc->Hide();
        fvr->Hide();
      }
      else {
        fdc->SetVal(sDomain);
        fdc->Show();
        if (!fImage)
          SetDefaultValueRange(valr);
      }
    }
    vr = valr;
  }
  catch (ErrorObject&) {
    DisableOK();
    fdc->Hide();
    fvr->Hide();
  }
  return 0;
}

int FormMapAggregate::exec() 
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
  String sEnd;
  if (fOffset)
    sEnd = String(",%li,%li", rcOffset.Row, rcOffset.Col);
  sExpr = String("MapAggregate%S(%S,%i,%s%S)", 
                 *sFunc, sMap, iFactor, 
                 fGroup ? "group" : "nogroup", sEnd);
  if (fOutMapList)
    sExpr = String("MapListApplic(%S,%S)", sMapList, sExpr);
  execMapOut(sExpr);  
  return 0;
}

LRESULT Cmdglueras(CWnd *wnd, const String& s)
{
	new FormMapGlue(wnd, s.c_str());
	return -1;
}

FormMapGlue::FormMapGlue(CWnd* mw, const char* sPar)
: FormMapCreate(mw, TR("Glue Raster Maps"))
{
  iMaps = 2;
  fGeoRef = false;
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
      if (fn.sExt == "" || fn.sExt == ".mpr")
        if (sMap1 == "")
          sMap1 = fn.sFullNameQuoted(false);
        else if (sMap2 == "")
          sMap2 = fn.sFullNameQuoted(false);
        else if (sMap3 == "") {
          iMaps = 3;
          sMap3 = fn.sFullNameQuoted(false);
        }
        else if (sMap4 == "") {
          iMaps = 4;
          sMap4 = fn.sFullNameQuoted(false);
        }
        else  
          sOutMap = fn.sFullName(false);
      if (fn.sExt == ".grf") 
      {
        fGeoRef = true;
        sGeoRef = fn.sFullNameQuoted(false);
      }
    }
  }
  iMaps -= 2;
  StaticText* st = new StaticText(root, TR("&Number of Input Maps"));
  st->SetIndependentPos();
  rgMaps = new RadioGroup(root, "", &iMaps, true);
  rgMaps->SetIndependentPos();
  rgMaps->SetCallBack((NotifyProc)&FormMapGlue::MapsCallBack);
  new RadioButton(rgMaps, "&2");
  new RadioButton(rgMaps, "&3");
  new RadioButton(rgMaps, "&4");

  fldMap1 = new FieldMap(root, TR("&1st Map"), &sMap1, new MapListerDomainType(".mpr", 0, true));
  fldMap1->SetCallBack((NotifyProc)&FormMapGlue::MapCallBack);
  fldMap2 = new FieldMap(root, TR("&2nd Map"), &sMap2, new MapListerDomainType(".mpr", 0, true));
  fldMap2->SetCallBack((NotifyProc)&FormMapGlue::MapCallBack);
  fldMap3 = new FieldMap(root, TR("&3rd Map"), &sMap3, new MapListerDomainType(".mpr", 0, true));
  fldMap3->SetCallBack((NotifyProc)&FormMapGlue::MapCallBack);
  fldMap4 = new FieldMap(root, TR("&4th Map"), &sMap4, new MapListerDomainType(".mpr", 0, true));
  fldMap4->SetCallBack((NotifyProc)&FormMapGlue::MapCallBack);

  fReplaceAll = true;
  new CheckBox(root, TR("&Last Map on top"), &fReplaceAll);
  fNewDom = false;
  cbDom = new CheckBox(root, TR("&New Domain"), &fNewDom);
  cbDom->SetCallBack((NotifyProc)&FormMapGlue::DomCallBack);
  fldDom = new FieldDataTypeCreate(cbDom, "", &sNewDom, ".dom", true);
  fldDom->SetCallBack((NotifyProc)&FormMapGlue::DomCallBack);

  CheckBox* cb = new CheckBox(root, TR("&GeoReference"), &fGeoRef);
  new FieldGeoRefC(cb, "", &sGeoRef);
  cb->Align(cbDom, AL_UNDER);

  FieldBlank* fb = new FieldBlank(root, 0);
  fb->Align(cb, AL_UNDER);
  initMapOut(false, false);
  initRemark();
  SetHelpItem("ilwisapp\\glue_raster_maps_dialog_box.htm");
  create();
}                    

int FormMapGlue::MapsCallBack(Event*)
{
  rgMaps->StoreData();
  iMaps += 2;
  switch (iMaps) {  // lots of fall throughs
    case 2: 
      fldMap3->Hide();
    case 3: 
      fldMap4->Hide();
  }
  switch (iMaps) {
    case 4: 
      fldMap4->Show();
    case 3: 
      fldMap3->Show();
    case 2: 
      fldMap2->Show();
    case 1:
      fldMap1->Show();
  }
  return 0;
}

int FormMapGlue::MapCallBack(Event*)
{
  rgMaps->StoreData();
  iMaps += 2;
  switch (iMaps) {  // lots of fall throughs
    case 4:
      fldMap4->StoreData();
    case 3:
      fldMap3->StoreData();
    case 2:
      fldMap2->StoreData();
    case 1:
      fldMap1->StoreData();
  }
  try {
    bool fShowDom = false;
    Map map1(sMap1);
	if (fgr) fgr->SetBounds(map1->cs(), map1->cb());

    Map map2(sMap2);
    if (map1->dm()->pdsrt() && map2->dm()->pdsrt()) {
      if (2 == iMaps)
        fShowDom = true;
      else {
        Map map3(sMap3);
        if (map3->dm()->pdsrt())
          if (3 == iMaps)
            fShowDom = true;
          else {
            Map map4(sMap4);
            if (map4->dm()->pdsrt())
              fShowDom = true;
          }
      }
    }
    if (fShowDom)
      cbDom->Show();
    else {
      cbDom->Hide();
      fNewDom = false;
      stRemark->SetVal("");
      EnableOK();
    }
  }
  catch (ErrorObject&) {
    cbDom->Hide();
  }
  return 0;
}

int FormMapGlue::DomCallBack(Event*)
{
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
      stRemark->SetVal(TR("Not a valid domain name"));
    else if(fn.fExist())
      stRemark->SetVal(TR("Domain already exists"));
    else {
      fOk = true;
      stRemark->SetVal("");
    }
  }
  if (fOk)
    EnableOK();
  else    
    DisableOK();
  return 0;
}

int FormMapGlue::exec()
{
  FormMapCreate::exec();
  iMaps += 2;
  String sExpr;
  FileName fn(sOutMap);
  FileName fnMap1(sMap1);
  FileName fnMap2(sMap2);
  FileName fnMap3(sMap3);
  FileName fnMap4(sMap4);
  sMap1 = fnMap1.sRelativeQuoted(false,fn.sPath());
  sMap2 = fnMap2.sRelativeQuoted(false,fn.sPath());
  sMap3 = fnMap3.sRelativeQuoted(false,fn.sPath());
  sMap4 = fnMap4.sRelativeQuoted(false,fn.sPath());
  String sFront, sEnd;
  if (fGeoRef) {
    FileName fnGeoRef(sGeoRef);
    sGeoRef = fnGeoRef.sRelativeQuoted(true,fn.sPath());
    sFront = String("MapGlue(%S,", sGeoRef);
  }
  else 
    sFront = String("MapGlue(");
  switch (iMaps) {
    case 2:
      sExpr = String("%S%S,%S",sFront,sMap1,sMap2);
      break;
    case 3:
      sExpr = String("%S%S,%S,%S",sFront,sMap1,sMap2,sMap3);
      break;
    case 4:
      sExpr = String("%S%S,%S,%S,%S",sFront,sMap1,sMap2,sMap3,sMap4);
      break;
  }
  if (fNewDom) {
    FileName fnDom(sNewDom);
    sNewDom = fnDom.sRelativeQuoted(false,fn.sPath());
    sExpr &= String(",%S", sNewDom);
  }
  if (fReplaceAll)
    sExpr &= ",replace)";
  else
    sExpr &= ')';
  execMapOut(sExpr);
  return 0;
}

LRESULT Cmdvariogramsurface(CWnd *wnd, const String& s)
{
	new FormMapVariogramSurface(wnd, s.c_str());
	return -1;
}

FormMapVariogramSurface::FormMapVariogramSurface(CWnd* mw, const char* sPar)
: FormMapCreate(mw, TR("Variogram Surface"))
{
  rLagSpacing = rUNDEF;
  iLags = 10;
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
      if (fn.sExt == "" || fn.sExt == ".mpr" || fn.sExt == ".mpp")
        if (sMap == "")
          sMap = fn.sFullNameQuoted(true);
        else
          sOutMap = fn.sFullName(false);
    }
  }
  fldMap = new FieldDataType(root, TR("&Input Map"), &sMap, 
                    new MapListerDomainType(".mpr.mpp", dmVALUE|dmIMAGE|dmBOOL, true), true);
  fldMap->SetCallBack((NotifyProc)&FormMapVariogramSurface::MapCallBack);
  frLagSpacing = new FieldReal(root, TR("&Lag spacing (m)"), &rLagSpacing, ValueRange(0.01,1e9,0.2));
  new FieldInt(root, TR("&Number of lags"), &iLags, ValueRange(2,45), true);
  initMapOutValRange(false);
  SetHelpItem("ilwisapp\\variogram_surface_dialog_box.htm");
  create();
}

int FormMapVariogramSurface::MapCallBack(Event* Evt)
{
  fldMap->StoreData();
  try {
    FileName fnMap(sMap);
    if (fnMap.sFile == "")
      return 0;
    ValueRange vr;
		bool fValues;
		BaseMap bmp(fnMap);
		if (fgr) fgr->SetBounds(bmp->cs(), bmp->cb());

    if (".mpp" == fnMap.sExt) {
      PointMap mp(fnMap);
			fValues = (0 != mp->dm()->pdv());
      rLagSpacing = MapVariogramSurfacePnt::rDefaultLagSpacing(mp);
      frLagSpacing->SetVal(rLagSpacing);
      frLagSpacing->Show();
			vr = MapVariogramSurfacePnt::vrDefault(mp);
    }
    else {
      Map mp(fnMap);
			fValues = (0 != mp->dm()->pdv());
      frLagSpacing->Hide();
      vr = MapVariogramSurfaceRas::vrDefault(mp);
    }
		if (fValues) 
		{
			SetDefaultValueRange(vr);
			EnableOK();
		}
		else 
		{
			stRemark->SetVal(TR("Only Value domains allowed"));
			DisableOK();
		}
    
  }
  catch (ErrorObject&) {}
  return 0;
}

int FormMapVariogramSurface::exec()
{
  FormMapCreate::exec();
  String sExpr;
  FileName fn(sOutMap);
  FileName fnMap(sMap);
  sMap = fnMap.sRelativeQuoted(false,fn.sPath());
  if (".mpp" == fnMap.sExt) {
    sExpr = String("MapVariogramSurfacePnt(%S,%.3f,%i)", sMap, rLagSpacing, iLags);
  }
  else {
    sExpr = String("MapVariogramSurfaceRas(%S,%i)", sMap, iLags);
  }
  execMapOut(sExpr);
  return 0;
}

LRESULT Cmdkrigingras(CWnd *wnd, const String& s)
{
	new FormMapKrigingFromRaster(wnd, s.c_str());
	return -1;
}

FormMapKrigingFromRaster::FormMapKrigingFromRaster(CWnd* mw, const char* sPar)
: FormMapCreate(mw, TR("Kriging From Raster"))
{
  fWiderValRange = true; 
  riMinMax = RangeInt(1,16);
  fErrorMap = false;
	FileName fn;
  if (sPar) {
    TextInput ip(sPar); 
    TokenizerBase tokenizer(&ip);
    String sVal;
    for (;;) {
      Token tok = tokenizer.tokGet();
      sVal = tok.sVal();
      if (sVal == "")
        break;
      fn = FileName(sVal);
      if (sMap == "")
        sMap = fn.sFullNameQuoted(true);
      else
        sOutMap = fn.sFullName(false);
    }
  }
	fldMap = new FieldDataType(root, TR("&Raster Map"), &sMap, new MapListerDomainType(".mpr", dmVALUE|dmIMAGE|dmBOOL, true), true);
  //initAsk(dmVALUE);  
	fldMap->SetCallBack((NotifyProc)&FormMapKrigingFromRaster::MapCallBack);
	new FieldBlank(root, 0.2);
  FieldSemiVariogram* fsv = new FieldSemiVariogram(root, TR("&SemiVariogram"), &smv);
  fsv->SetIndependentPos();
	new FieldBlank(root, 0);
	FieldGroup* fgUseUnits = new FieldGroup(root);
  rgUnits = new RadioGroup(fgUseUnits, TR("Limiting &Distance"), &iUnitChoice,true);
	rgUnits->SetCallBack((NotifyProc)&FormMapKrigingFromRaster::UnitsCallBack);
  RadioButton *rbMeter = new RadioButton(rgUnits, TR("&Meters:"));
	rbMeter->Align(rgUnits, AL_UNDER);
  frMeter = new FieldReal(fgUseUnits, "", &rRadius, ValueRangeReal(0, 1e6, 0));
	frMeter->SetCallBack((NotifyProc)&FormMapKrigingFromRaster::MeterEditCallBack);
	frMeter->Align(rbMeter, AL_AFTER);
  RadioButton *rbPixels = new RadioButton(rgUnits, TR("&Pixels:"));
	rbPixels->Align(rbMeter, AL_UNDER);
	fiPixels = new FieldInt(fgUseUnits, "", &iPixels, ValueRange(1, 40), true);
	fiPixels->SetCallBack((NotifyProc)&FormMapKrigingFromRaster::PixelEditCallBack);
	fiPixels->Align(rbPixels, AL_AFTER);

  FieldRangeInt *fri = new FieldRangeInt(root, TR("&Min, max nr of input pixels"), &riMinMax, ValueRange(1,100));
	fri->Align(fgUseUnits, AL_UNDER);

  initMapOutValRange(false);
  CheckBox* cbEr = new CheckBox(root, TR("&Error Map"), &fErrorMap);
	initRemark();
  SetHelpItem("ilwisapp\\kriging_from_raster_dialog_box.htm");
  create();
}

int FormMapKrigingFromRaster::MeterEditCallBack(Event*)
{
  if (sMap.length() == 0)
    return 0;
  try {
    Map mp(sMap);
	  if (mp->gr()->fGeoRefNone())
			return 0;
		rPixS = mp->gr()->rPixSize();
		bool fValues = (0 != mp->dm()->pdv());
		rgUnits->StoreData();
		if (iUnitChoice == 0)
		{
			frMeter->StoreData();
			iPixels = round(rRadius / rPixS);
			fiPixels->SetVal(iPixels);
			if (iPixels <= 0 || iPixels > 40)
			{
				double rInMeters = rPixS * 40.0;
				double rMin = round(rPixS / 2.0);
				stRemark->SetVal(String(TR("Radius must be between %.2lf and %.2lf meter (1 to 40 pixels)").c_str(), rMin, rInMeters));
				DisableOK();
			}
			else
			{
				stRemark->SetVal("");
				if (fValues)
					EnableOK();
				else {
					stRemark->SetVal(TR("Only Value domains allowed"));
					DisableOK();
				}
			}
		}
  }
  catch (...) {}  
  return 1;  
}

int FormMapKrigingFromRaster::PixelEditCallBack(Event*)
{
  if (sMap.length() == 0)
    return 0;
  try {
    Map mp(sMap);
	  if (mp->gr()->fGeoRefNone())
			return 0;
		rPixS = mp->gr()->rPixSize();
		bool fValues = (0 != mp->dm()->pdv());
		rgUnits->StoreData();
		if (iUnitChoice == 1)
		{
			fiPixels->StoreData();
			rRadius = iPixels * rPixS;
			frMeter->SetVal(rRadius);
			if (iPixels <= 0 || iPixels > 40)
			{
				stRemark->SetVal(TR("Radius must be between 1 and 40 pixels").c_str());
				DisableOK();
			}
			else
			{
				stRemark->SetVal("");
				if (fValues)
					EnableOK();
				else {
					stRemark->SetVal(TR("Only Value domains allowed"));
					DisableOK();
				}
			}
		}
  }
  catch (...) {}  
  return 1;  
}

int FormMapKrigingFromRaster::UnitsCallBack(Event*)
{
  if (sMap.length() == 0) {
		fiPixels->Disable();
		frMeter->Disable();
    return 0;
	}
  try {
    Map mp(sMap);
	  if (mp->gr()->fGeoRefNone())
			return 0;
		rPixS = mp->gr()->rPixSize();

		rgUnits->StoreData();
		switch (iUnitChoice)
		{
			case 0 : // meters
							fiPixels->Disable();
							frMeter->Enable();
							break;
			case 1 : // pixels
							fiPixels->Enable();
							frMeter->Disable();
							break;
		}
  }
  catch (...) 
	{
		fiPixels->Disable();
		frMeter->Disable();
	}  
  return 1;  
}

int FormMapKrigingFromRaster::MapCallBack(Event*)
{
	FormMapCreate::exec();
	fldMap->StoreData();
	if (sMap.length() == 0)
		return 0;
	try {
		Map mp(sMap);
		if (fgr) fgr->SetBounds(mp->cs(), mp->cb());
		
		bool fValues = (0 != mp->dm()->pdv());
    vr = MapKrigingFromRaster::vrDefault(mp);
    SetDefaultValueRange(vr);
  
		if (mp->gr()->fGeoRefNone())
		{
			stRemark->SetVal(TR("Georeference with coordinates needed"));
			DisableOK();
		}
		else
		{
			rPixS = mp->gr()->rPixSize();
			rRadius = MapKrigingFromRaster::rDefaultRadius(mp);
			iPixels = round(rRadius / rPixS );
			frMeter->SetVal(rRadius);
			fiPixels->SetVal(iPixels);
			
			rgUnits->StoreData();
			switch (iUnitChoice)
			{
			case 0 : // meters
				fiPixels->Disable();
				frMeter->Enable();
				break;
			case 1 : // pixels
				fiPixels->Enable();
				frMeter->Disable();
				break;
			}
			stRemark->SetVal("");
			if (fValues)
				EnableOK();
			else {
				stRemark->SetVal(TR("Only Value domains allowed"));
				DisableOK();
			}
		}
	}
	catch (...) {}  
	return 1;  
}


int FormMapKrigingFromRaster::exec()
{
  FormMapCreate::exec();
	sDomain = "value";
	String sExpr;
	String sSmv = smv.sExpression();
	String sRadUnits;
	double rRadiusInMetersOrPixels = rRadius;
  switch (iUnitChoice){
		case 0 :sRadUnits = "m"; 
			break;
    case 1: sRadUnits = "p";
			rRadiusInMetersOrPixels = rRadius / rPixS;
			break;
	}
  FileName fn(sOutMap);
  FileName fnMap(sMap);
  sMap = fnMap.sRelativeQuoted(false,fn.sPath());
	int iMin = riMinMax.iLo();
  int iMax = riMinMax.iHi();
	sErrorMap = fErrorMap ? String("1") : String("0");
  sExpr = String("MapKrigingFromRaster(%S,%S,%f,%S,%S,%i,%i)",
                     sMap, sSmv, rRadiusInMetersOrPixels, 
										 sRadUnits, sErrorMap, iMin, iMax);
  execMapOut(sExpr);
  return 0;
}
