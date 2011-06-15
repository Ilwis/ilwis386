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
/* Forms for Interactive "Applications"
   Copyright Ilwis System Development ITC
   september 1996, by Wim Koolhoven
	Last change:  WK   17 Sep 98    2:14 pm
*/

#include "Client\Headers\AppFormsPCH.h"
#include "Engine\SpatialReference\Coordsys.h"
#include "ApplicationsUI\frmappia.h"
#include "Client\ilwis.h"

FormInteractiveApplication::FormInteractiveApplication(CWnd* parent, const String& sTitle)
: FormBaseDialog(parent, sTitle, fbsSHOWALWAYS|fbsNOOKBUTTON)
{
}

FormInteractiveApplication::~FormInteractiveApplication()
{
}

int FormInteractiveApplication::exec()
{
  return 0;
}  

FormTransformCoordinates::FormTransformCoordinates(CWnd* mw, const char* sPar)
: FormInteractiveApplication(mw, SAFTitleTransfCoords)
{
	int iImg = IlwWinApp()->iImage(".csy");
	HICON hIco = IlwWinApp()->ilSmall.ExtractIcon(iImg);
	SetIcon(hIco,FALSE);

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
      if (fn.sExt == "" || fn.sExt == ".csy")
        if (sCsyInput == "") 
          sCsyInput = fn.sFullName(false);
        else 
          sCsyOutput = fn.sFullName(false);
    }      
  }
  String sFill('x', 100);
  fcsInput = new FieldCoordSystem(root, SAFUiInpCoordSys, &sCsyInput);
  fcsInput->SetCallBack((NotifyProc)&FormTransformCoordinates::CsyInputCallBack);
  stCsyInput = new StaticText(root, sFill);
  stCsyInput->SetIndependentPos();
	fDDD = false;
	cbDDD = new CheckBox(root, SAFRemDDD, &fDDD);
	cbDDD->SetCallBack((NotifyProc)&FormTransformCoordinates::DDDCallBack);
	cbDDD->Align(stCsyInput, AL_UNDER);
	cbDDD->SetIndependentPos();
  fldLL = new FieldLatLon(root, SAFUiInpCoord, &ll);
  fldLL->SetCallBack((NotifyProc)&FormTransformCoordinates::CoordInputCallBack);
	fldLL->Align(cbDDD, AL_UNDER);
	crdLL = Coord(0,0);
	fldLLDDD = new FieldCoord(root, SAFUiInpCoordNE, &crdLL);
	fldLLDDD->SetCallBack((NotifyProc)&FormTransformCoordinates::CoordInputCallBack);
	fldLLDDD->Align(cbDDD, AL_UNDER);
  fldCrd = new FieldCoord(root, SAFUiInpCoord, &crd);
  fldCrd->Align(stCsyInput, AL_UNDER);
  fldCrd->SetCallBack((NotifyProc)&FormTransformCoordinates::CoordInputCallBack);
  fcsOutput = new FieldCoordSystem(root, SAFUiOutCoordSys, &sCsyOutput);
  fcsOutput->Align(fldLL, AL_UNDER);
  fcsOutput->SetCallBack((NotifyProc)&FormTransformCoordinates::CsyOutputCallBack);
  stCsyOutput = new StaticText(root, sFill);
  stCsyOutput->SetIndependentPos();
  StaticText* stOutpCrd = new StaticText(root, SAFRemOutputCoord);
	fsResult = new FieldString(root, &sFill64, ES_AUTOHSCROLL|WS_TABSTOP|WS_GROUP|ES_READONLY);
  fsResult->Align(stOutpCrd, AL_AFTER);
  fsResult->SetIndependentPos();
	fsResult->SetWidth(180);
	SetHelpItem("ilwisapp\\transform_coordinates.htm");
  create();
}

LRESULT Cmdtransform(CWnd *parent, const String& s) {
	new FormTransformCoordinates(parent, s.scVal());
	return -1;
}

int FormTransformCoordinates::CsyInputCallBack(Event*)
{
  fcsInput->StoreData();
  if ("" == sCsyInput) {
    stCsyInput->SetVal("");
    csInput = CoordSystem();
    fldCrd->Hide();
		cbDDD->Hide();
    fldLL->Hide();
		fldLLDDD->Hide();
    return 0;
  }  
  try {
    FileName fnCsy(sCsyInput); 
    csInput = CoordSystem(fnCsy);
    if (csInput.fValid())
      stCsyInput->SetVal(csInput->sDescription);
    if (csInput->pcsLatLon()) {
      fldCrd->Hide();
			cbDDD->Show();
			if (fDDD) {
				fldLL->Hide();
				fldLLDDD->Show();
			}
			else {
				fldLLDDD->Hide();
				fldLL->Show();
			}
    }
    else {
			cbDDD->Hide();
      fldCrd->Show();
			fldLLDDD->Hide();
			fldLL->Hide();
    } 
  }
  catch (ErrorObject& err) {
    err.Show();
    stCsyInput->SetVal(SAFRemInvalidCoordSys);
    csInput = CoordSystem();
    fldCrd->Hide();
    fldLL->Hide();
		fldLLDDD->Hide();
  }  
  return CoordInputCallBack(0);
}
  
int FormTransformCoordinates::CsyOutputCallBack(Event*)
{
  fcsOutput->StoreData();
  if ("" == sCsyOutput) {
    stCsyOutput->SetVal("");
    csOutput = CoordSystem();
    return 0;
  }  
  try {
    FileName fnCsy(sCsyOutput); 
    csOutput = CoordSystem(fnCsy);
    if (csOutput.fValid())
			stCsyOutput->SetVal(csOutput->sDescription);
  }
  catch (ErrorObject& err) {
    err.Show();
    stCsyOutput->SetVal(SAFRemInvalidCoordSys);
    csOutput = CoordSystem();
  }  
  return CoordInputCallBack(0);
}
  
int FormTransformCoordinates::CoordInputCallBack(Event*)
{
  if (!csInput.fValid() || !csOutput.fValid()) {
    fsResult->SetVal("");
		return 0;
  }  
  if (!csOutput->fConvertFrom(csInput)) {
    fsResult->SetVal(SAFRemNoTransfPossible);
		return 0;
  }
  Coord crdOut;
  if (csInput->pcsLatLon())  {
    fldLL->StoreData();
		fldLLDDD->StoreData();
		if (fDDD)
			ll = LatLon(crdLL.x, crdLL.y);
    crd = csInput->cConv(ll);
    crdOut = csOutput->cConv(csInput, crd);
  }
  else {
    fldCrd->StoreData();
    crdOut = csOutput->cConv(csInput, crd);
  }
  String s = csOutput->sValue(crdOut, 0, 5);
  sXout = String("%.10f",crdOut.x);
	sYout = String("%.10f",crdOut.y);
	String sXYout = sXout;
	sXYout &= String("; ");
	sXYout &= sYout;
	fsResult->SetVal(sXYout);
	Array<String> as;
	Split(s, as, ",");
	if (as.size() == 2)
		s = as[0] + ", " + as[1];
	if (csOutput->pcsLatLon() && (crdOut != crdUNDEF)) {
		String sDDDLat = String("%.10f",crdOut.y);
		s &= String("  (= ");
		s &= sDDDLat;
		String sDDDLon = String(", %.10f",crdOut.x);
		s &= sDDDLon;
		s &= String(")");
	}
  fsResult->SetVal(s);
  return 0;
}

int FormTransformCoordinates::DDDCallBack(Event*)
{
  cbDDD->StoreData();
  if ("" == sCsyInput) {
    stCsyInput->SetVal("");
    csInput = CoordSystem();
    fldCrd->Hide();
    fldLL->Hide();
		fldLLDDD->Hide();
    return 0;
  }  
  try {
    FileName fnCsy(sCsyInput); 
    csInput = CoordSystem(fnCsy);
    if (csInput.fValid())
      stCsyInput->SetVal(csInput->sDescription);
    fldCrd->Hide();
		if (fDDD) {
			fldLL->Hide();
			fldLLDDD->Show();
		}
		else {
			fldLLDDD->Hide();
			fldLL->Show();
		}
  }
  catch (ErrorObject& err) {
    err.Show();
    stCsyInput->SetVal(SAFRemInvalidCoordSys);
    csInput = CoordSystem();
    fldCrd->Hide();
    fldLL->Hide();
		fldLLDDD->Hide();
  }  
  return CoordInputCallBack(0);
}

LRESULT Cmdtransformheight(CWnd *parent, const String& s)
{
	new FormTransformCoordinatesH(parent, s.scVal());
	return -1;
}

FormTransformCoordinatesH::FormTransformCoordinatesH(CWnd* mw, const char* sPar)
: FormInteractiveApplication(mw, SAFTitleTransfCoordsHeights)
{
	int iImg = IlwWinApp()->iImage(".csy");
	HICON hIco = IlwWinApp()->ilSmall.ExtractIcon(iImg);
	SetIcon(hIco,FALSE);

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
      if (fn.sExt == "" || fn.sExt == ".csy")
        if (sCsyInput == "") 
          sCsyInput = fn.sFullName(false);
        else 
          sCsyOutput = fn.sFullName(false);
    }      
  }
  fCsyInpValid = false;
  fCsyOutValid = false;
  fCrdInpValid = false;
  fHeightInpValid = false;
  String sFill('x', 100);
	String sFill99('x', 99);
	iNrOfDatumParms = 0;
	rLatIn = 0;
	rLonIn = 0;
  fcsInput = new FieldCoordSystem(root, SAFUiInpCoordSys, &sCsyInput);
  fcsInput->SetCallBack((NotifyProc)&FormTransformCoordinatesH::CsyInputCallBack);
  stCsyInput = new StaticText(root, sFill);
  stCsyInput->SetIndependentPos();
  stDatumName1 = new StaticText(root, sFill99);
  stDatumName1->Align(stCsyInput, AL_UNDER);
  stDatumName1->SetIndependentPos();
	fDDD = false;
	cbDDD = new CheckBox(root, SAFRemDDD, &fDDD);
	cbDDD->SetCallBack((NotifyProc)&FormTransformCoordinatesH::DDDCallBack);
	cbDDD->Align(stDatumName1, AL_UNDER);
	cbDDD->SetIndependentPos();
  fldLLDMS = new FieldLatLon(root, SAFUiInpCoord, &ll);
  fldLLDMS->SetCallBack((NotifyProc)&FormTransformCoordinatesH::CoordInputCallBack);
	fldLLDMS->Align(cbDDD, AL_UNDER);
	fgLLDDD = new FieldGroup(cbDDD);// belonging to this group
  fgLLDDD->Align(cbDDD, AL_UNDER);
	StaticText *stLoc = new StaticText(fgLLDDD, SAFUiInpCoord);
	frLatIn = new FieldReal(fgLLDDD, "", &rLatIn, ValueRange(-90,90,0.000000001));
	frLatIn->Align(stLoc, AL_AFTER);
	frLatIn->SetIndependentPos();
	frLonIn = new FieldReal(fgLLDDD, "", &rLonIn, ValueRange(-360,360,0.000000001));
  frLonIn->Align(frLatIn, AL_AFTER);
  fldCrd = new FieldCoord(root, SAFUiInpCoord, &crd);
  fldCrd->Align(stDatumName1, AL_UNDER);
  fldCrd->SetCallBack((NotifyProc)&FormTransformCoordinatesH::CoordInputCallBack);
  fldCrd->SetIndependentPos();
  FieldBlank* fb = new FieldBlank(root,1);
  fb->Align(fldCrd, AL_UNDER);
  frHeightIn = new FieldReal(root, SAFRemHeightInput, &rHeightIn, ValueRange(-1e4,1e6,0.001));
  frHeightIn->Align(fb, AL_UNDER);
  frHeightIn->SetCallBack((NotifyProc)&FormTransformCoordinatesH::HeightInCallBack);
  fcsOutput = new FieldCoordSystem(root, SAFUiOutCoordSys, &sCsyOutput);
  fcsOutput->Align(frHeightIn, AL_UNDER);
  fcsOutput->SetCallBack((NotifyProc)&FormTransformCoordinatesH::CsyOutputCallBack);
  stCsyOutput = new StaticText(root, sFill);
  stCsyOutput->SetIndependentPos();
  stDatumName2 = new StaticText(root, sFill99);
  stDatumName2->Align(stCsyOutput, AL_UNDER);
  stDatumName2->SetIndependentPos();
  StaticText* stOutpCrd = new StaticText(root, SAFRemOutputCoord);
	fsResult = new FieldString(root, &sFill64, ES_AUTOHSCROLL|WS_TABSTOP|WS_GROUP|ES_READONLY);
  fsResult->Align(stOutpCrd, AL_AFTER);
  fsResult->SetIndependentPos();
	fsResult->SetWidth(180);
  StaticText* stH = new StaticText(root, SAFRemHeightOutput);
  stH->Align(stOutpCrd, AL_UNDER);
  fsHeightOut = new FieldString(root, &sFill38, ES_AUTOHSCROLL|WS_TABSTOP|WS_GROUP|ES_READONLY);
  fsHeightOut->Align(stH, AL_AFTER);
  fsHeightOut->SetIndependentPos();
  SetHelpItem("ilwisapp\\find_datum_trans_params_methodpage.htm");
  create();
}

int FormTransformCoordinatesH::CsyInputCallBack(Event*)
{
  fcsInput->StoreData();
	cbDDD->StoreData();
  stDatumName1->SetVal("");
	//stEllipsoidInput->SetVal("");
  //stEllipsoidOutput->SetVal("");
  iNrOfDatumParms = 0;
  if ("" == sCsyInput) {
    stCsyInput->SetVal("");
	fsHeightOut->SetVal("");
    csInput = CoordSystem();
	fCsyInpValid= false;
    fldCrd->Hide();
		cbDDD->Hide();
    fldLLDMS->Hide();
		fgLLDDD->Hide();
    return 0;
  }  
  try {
    FileName fnCsy(sCsyInput); 
    csInput = CoordSystem(fnCsy);
	bwdat1 = 0;
	badat1 = 0;
    if (csInput.fValid()) {
	  fCsyInpValid = true;
      stCsyInput->SetVal(csInput->sDescription);
	  Datum* datum1 = csInput->pcsViaLatLon()->datum;
	  if (datum1) {
		bwdat1 = dynamic_cast<BursaWolfDatum*>(datum1);
		badat1 = dynamic_cast<BadekasDatum*>(datum1);
	  }
	  else {
		bwdat1 = 0;
		badat1 = 0;
	  }
	  sEllipsoid1 = csInput->pcsViaLatLon()->ell.sName;
	  if(badat1) {	
		sDatum1 = badat1->sType();
		iNrOfDatumParms = 10;
	  }
	  else if (bwdat1) { // BursaWolfDatum is parent of BadekasDatum 
	    sDatum1 = bwdat1->sType();
		iNrOfDatumParms = 7;
	  }
	  else if (sEllipsoid1 == "WGS 84")
		sDatum1 = String("WGS84 Global ");
	  else
		  sDatum1 = String("No 3D- ");
	  sDatum1 &= String(" Datum definition");
	  if (bwdat1 || badat1)
		sDatum1 &= String(SAFUiNrOfDatumParms_i.scVal(), iNrOfDatumParms);
	  stDatumName1->SetVal(sDatum1);
	}
    if (csInput->pcsLatLon()) {
      fldCrd->Hide();
			cbDDD->Show();
			if (fDDD) {
				fldLLDMS->Hide();
				fgLLDDD->Show();
			}
			else {
				fgLLDDD->Hide();
				fldLLDMS->Show();
			}
    }
    else {
			cbDDD->Hide();
      fldCrd->Show();
			fgLLDDD->Hide();
			fldLLDMS->Hide();
    } 
  }
  catch (ErrorObject& err) {
    err.Show();
    stCsyInput->SetVal(SAFRemInvalidCoordSys);
    csInput = CoordSystem();
	fCsyInpValid = false;
    fldCrd->Hide();
    fldLLDMS->Hide();
		fgLLDDD->Hide();
  }  
  return RecomputeRewriteOutput();
  //return CoordInputCallBack(0);
}
  
int FormTransformCoordinatesH::CsyOutputCallBack(Event*)
{
  fcsOutput->StoreData();
  stDatumName2->SetVal("");
  fsHeightOut->SetVal("");
  iNrOfDatumParms = 0;
  if ("" == sCsyOutput) {
    stCsyOutput->SetVal("");
    csOutput = CoordSystem();
	fCsyOutValid = false;
    return 0;
  }  
  try {
    FileName fnCsy(sCsyOutput); 
    csOutput = CoordSystem(fnCsy);
	bwdat2 = 0;
	badat2 = 0;
    if (csOutput.fValid()) {
		fCsyOutValid = true;
		stCsyOutput->SetVal(csOutput->sDescription);
		Datum* datum2 = csOutput->pcsViaLatLon()->datum;
		if (datum2) {
		  bwdat2 = dynamic_cast<BursaWolfDatum*>(datum2);
		  badat2 = dynamic_cast<BadekasDatum*>(datum2);
		}
	    else {
		  bwdat2 = 0;
		  badat2 = 0;
		}
		sEllipsoid2 = csOutput->pcsViaLatLon()->ell.sName;
		if(badat2) {	
			sDatum2 = badat2->sType();
			iNrOfDatumParms = 10;
		}
	    else if (bwdat2) { // BursaWolfDatum is parent BadekasDatum 
	      sDatum2 = bwdat2->sType();
		  iNrOfDatumParms = 7;
		}
		else if (sEllipsoid2 == "WGS 84")
		  sDatum2 = String("WGS84 Global ");
		else
		  sDatum2 = String("No 3D-");
		sDatum2 &= String(" Datum definition");
		if (bwdat2 || badat2)
			sDatum2 &= String(SAFUiNrOfDatumParms_i.scVal(), iNrOfDatumParms);
		stDatumName2->SetVal(sDatum2);
	}
  }
  catch (ErrorObject& err) {
    err.Show();
    stCsyOutput->SetVal(SAFRemInvalidCoordSys);
    csOutput = CoordSystem();
  }
  return RecomputeRewriteOutput();
  //return CoordInputCallBack(0);
}
  
int FormTransformCoordinatesH::CoordInputCallBack(Event*)
{
  if (!csInput.fValid() || !csOutput.fValid()) {
    fsResult->SetVal("");
	fsHeightOut->SetVal("");
	fCsyInpValid = csInput.fValid();
	fCsyOutValid = csOutput.fValid();
    return 0;
  }  
  Coord crdOut;
  if (csInput->pcsLatLon())  {
		if (fDDD) {
			fgLLDDD->StoreData();
			ll = LatLon(frLatIn->rVal(),frLonIn->rVal());
		}
		else
			fldLLDMS->StoreData();		
    crd = csInput->cConv(ll);
  }
  else {
    fldCrd->StoreData();
   // crdOut = csOutput->cConv(csInput, crd);
  }  
  return RecomputeRewriteOutput();
  //return 0;
}

int FormTransformCoordinatesH::DDDCallBack(Event*)
{
  cbDDD->StoreData();
  if ("" == sCsyInput) {
    stCsyInput->SetVal("");
    csInput = CoordSystem();
    fldCrd->Hide();
    fldLLDMS->Hide();
		//fldLLDDD->Hide();
		fgLLDDD->Hide();
    return 0;
  }  
  try {
    FileName fnCsy(sCsyInput); 
    csInput = CoordSystem(fnCsy);
    if (csInput.fValid())
      stCsyInput->SetVal(csInput->sDescription);
    fldCrd->Hide();
		if (fDDD) { ////for precision fldDDD must be replaced by 2 fldReals !!!!!!!!!!!!!!!
			frLatIn->SetVal(fldLLDMS->llVal().Lat);
			frLonIn->SetVal(fldLLDMS->llVal().Lon);
			fldLLDMS->Hide();
			fgLLDDD->Show();
		}
		else {
			LatLon llTmp = LatLon(frLatIn->rVal(),frLonIn->rVal());
			fldLLDMS->SetVal(llTmp);
			fgLLDDD->Hide();
			fldLLDMS->Show();
		}
  }
  catch (ErrorObject& err) {
    err.Show();
    stCsyInput->SetVal(SAFRemInvalidCoordSys);
    csInput = CoordSystem();
    fldCrd->Hide();
    fldLLDMS->Hide();
		fldLLDDD->Hide();
		fgLLDDD->Hide();
  }  
  return CoordInputCallBack(0);
}

int FormTransformCoordinatesH::HeightInCallBack(Event*)
{
  frHeightIn->StoreData();
  fsHeightOut->SetVal("");
  if (!csInput.fValid() || !csOutput.fValid()) {
    fsResult->SetVal("");
	fCsyInpValid = csInput.fValid();
	fCsyOutValid = csOutput.fValid();
    return 0;
  }  
  fldCrd->StoreData();
  fldLLDMS->StoreData();
	fgLLDDD->StoreData();
  sOutpHeight = String();
  if (abs(rHeightIn) > 10000) {
    fsResult->SetVal("Height out of bounds");
    return 0;
  }
  return RecomputeRewriteOutput();
}

int FormTransformCoordinatesH::RecomputeRewriteOutput()
{

  fsResult->SetVal("");
  fsHeightOut->SetVal("");
  if (abs(rHeightIn) > 10000) {
    fsResult->SetVal("Height out of bounds");
    return 0;
  }
  String s = String();
  LatLonHeight llhIn;
  LatLonHeight llhWGS;
  LatLonHeight llhOut;
  csvll1 = csInput->pcsViaLatLon();
  csvll2 = csOutput->pcsViaLatLon();
  if (csvll1 == 0 || csvll2 == 0 )
  {
		fsResult->SetVal(SAFRemNo3DTransfPossible);
		return 0;
  }
  Datum* datum1 = csvll1->datum;
  Datum* datum2 = csvll2->datum;
  bwdat1 = dynamic_cast<BursaWolfDatum*>(datum1);
  bwdat2 = dynamic_cast<BursaWolfDatum*>(datum2);
  badat1 = dynamic_cast<BadekasDatum*>(datum1);
  badat2 = dynamic_cast<BadekasDatum*>(datum2);
  
  MolodenskyDatum* mold1 = dynamic_cast<MolodenskyDatum*>(datum1);
  MolodenskyDatum* mold2 = dynamic_cast<MolodenskyDatum*>(datum2);
  bool fFromWGS = false; bool fToWGS = false;
  if (mold1) {
	fFromWGS = (mold1->dx == 0 && mold1->dy == 0 || mold1->dz == 0);
	fFromWGS &= (mold1->ell.sName == String("WGS 84"));
  }
  if (mold2) {
    fToWGS = (mold2->dx == 0 && mold2->dy == 0 || mold2->dz == 0);
	fToWGS &= (mold2->ell.sName == String("WGS 84"));
  }
  if ((bwdat1 == 0 && badat1 == 0 && !fFromWGS) 
	  || (bwdat2 == 0 && badat2 == 0 && !fToWGS))
  {
		fsResult->SetVal(SAFRemNo3DTransfPossible);
		return 0;
  }
  fBursaWolf1 = (bwdat1 != 0);
  fBursaWolf2 = (bwdat2 != 0);
  fBadekas1 = (badat1 != 0);
  fBadekas2 = (badat2 != 0);
  cspr1 = csInput->pcsProjection();
  cspr2 = csOutput->pcsProjection();
  fProj1 = (0 != cspr1);
  fProj2 = (0 != cspr2);
  Coord cTmp = crd; //input {both formats, XY and LL)
  if (datum1 && datum2)
  {
	LatLon llTmp;
	llTmp = ll;
	if (fDDD)
		llTmp = LatLon(rLatIn, rLonIn);
	CoordCTS ctsIn, ctsOut;
	if (fProj1)
		llTmp = cspr1->llConv(cTmp);
	//else
	//	llTmp = LatLon(cTmp.y, cTmp.x);
	llhIn = LatLonHeight(llTmp, rHeightIn);
	if (fBadekas1)
		llhWGS = badat1->llhToWGS84(llhIn);
	else if (fBursaWolf1)
		llhWGS = bwdat1->llhToWGS84(llhIn);
	else
		llhWGS = llhIn;
	if (fBadekas2)
		llhOut = badat2->llhFromWGS84(llhWGS);
	else if (fBursaWolf2)
		llhOut = bwdat2->llhFromWGS84(llhWGS);
	else
		llhOut = llhWGS;
	llTmp = LatLon(llhOut.Lat, llhOut.Lon);
	if (fProj2)
		cTmp = cspr2->cConv(llTmp);
	else
		cTmp = Coord(llTmp.Lon, llTmp.Lat);
  }
  s = csOutput->sValue(cTmp);
  if (csOutput->pcsLatLon() && (cTmp != crdUNDEF)) 
  {
	String sDDDLat = String("%.10f",cTmp.y);
	s &= String(" (");
	s &= sDDDLat;
	String sDDDLon = String(", %.10f",cTmp.x);
	s &= sDDDLon;
	s &= String(")");
  }
  fsResult->SetVal(s);
  frHeightIn->StoreData();
  sOutpHeight = String("%.5f",llhOut.rHeight);
  fsHeightOut->SetVal(sOutpHeight);
  
  return 1;
}

LRESULT Cmdtransformtogeocentric(CWnd *parent, const String& s) {
	new FormTransformCoordinatesH_to_ctsXYZ(parent, s.scVal());
	return -1;
}

FormTransformCoordinatesH_to_ctsXYZ::FormTransformCoordinatesH_to_ctsXYZ(CWnd* mw, const char* sPar)
: FormInteractiveApplication(mw, SAFTitleTransf_to_Geocentric)
{
	int iImg = IlwWinApp()->iImage(".csy");
	HICON hIco = IlwWinApp()->ilSmall.ExtractIcon(iImg);
	SetIcon(hIco,FALSE);

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
      if (fn.sExt == "" || fn.sExt == ".csy")
        if (sCsyInput == "") 
          sCsyInput = fn.sFullName(false);
//        else 
//          sCsyOutput = fn.sFullName(false);
    }      
  }
  fCsyInpValid = false;
//  fCsyOutValid = false;
  fCrdInpValid = false;
  fHeightInpValid = false;
  String sFill('x', 100);
	String sFill64('x', 64);
	String sFill38('x', 38);
	String sFill99('x', 99);
	rLatIn = 0;
	rLonIn = 0;
  fcsInput = new FieldCoordSystem(root, SAFUiInpCoordSys, &sCsyInput);
  fcsInput->SetCallBack((NotifyProc)&FormTransformCoordinatesH_to_ctsXYZ::CsyInputCallBack);
  stCsyInput = new StaticText(root, sFill);
  stCsyInput->SetIndependentPos();
  //stDatumName = new StaticText(root, sFill99);
  //stDatumName->Align(stCsyInput, AL_UNDER);
  //stDatumName->SetIndependentPos();
	fDDD = false;
	cbDDD = new CheckBox(root, SAFRemDDD, &fDDD);
	cbDDD->SetCallBack((NotifyProc)&FormTransformCoordinatesH_to_ctsXYZ::DDDCallBack);
	cbDDD->Align(stCsyInput, AL_UNDER);
	cbDDD->SetIndependentPos();
  fldLLDMS = new FieldLatLon(root, SAFUiInpCoord, &ll);
  fldLLDMS->SetCallBack((NotifyProc)&FormTransformCoordinatesH_to_ctsXYZ::CoordInputCallBack);
	fldLLDMS->Align(cbDDD, AL_UNDER);
	//crdLL = Coord(0,0);////for precision fldLLDDD must be replaced by 2 fldReals !!!!!!!!!!!!!!!
	fgLLDDD = new FieldGroup(cbDDD);// belonging to this group
  fgLLDDD->Align(cbDDD, AL_UNDER);
	StaticText *stLoc = new StaticText(fgLLDDD, SAFUiInpCoord);
	frLatIn = new FieldReal(fgLLDDD, "", &rLatIn, ValueRange(-90,90,0.000000001));
	frLatIn->Align(stLoc, AL_AFTER);
	frLatIn->SetIndependentPos();
	frLonIn = new FieldReal(fgLLDDD, "", &rLonIn, ValueRange(-360,360,0.000000001));
  frLonIn->Align(frLatIn, AL_AFTER);
  fldCrd = new FieldCoord(root, SAFUiInpCoord, &crd);
  fldCrd->Align(stCsyInput, AL_UNDER);
  fldCrd->SetCallBack((NotifyProc)&FormTransformCoordinatesH_to_ctsXYZ::CoordInputCallBack);
  fldCrd->SetIndependentPos();
  FieldBlank* fb = new FieldBlank(root,1);
  fb->Align(fldCrd, AL_UNDER);
  frHeightIn = new FieldReal(root, SAFRemHeightInput, &rHeightIn, ValueRange(-1e4,1e6,0.001));
  frHeightIn->Align(fb, AL_UNDER);
  frHeightIn->SetCallBack((NotifyProc)&FormTransformCoordinatesH_to_ctsXYZ::HeightInCallBack);
 
  StaticText* stOutpCrd = new StaticText(root, SAFRemOutputCoord);
  stResult = new StaticText(root, sFill64);
  stResult->Align(stOutpCrd, AL_UNDER);
  stResult->SetIndependentPos();
  frXCoordOut = new FieldReal(root, SAFUiXcts, &ctsOut.x, ValueRange(-1e12,1e12,0.001));
  frXCoordOut->Align(stResult, AL_UNDER);
  frXCoordOut->SetIndependentPos();
  frYCoordOut = new FieldReal(root, SAFUiYcts, &ctsOut.y, ValueRange(-1e12,1e12,0.001));
  frYCoordOut->Align(frXCoordOut, AL_UNDER);
    frYCoordOut->SetIndependentPos();
	frZCoordOut = new FieldReal(root, SAFUiZcts, &ctsOut.z, ValueRange(-1e12,1e12,0.001));
  frZCoordOut->Align(frYCoordOut, AL_UNDER);
    frZCoordOut->SetIndependentPos();
  SetHelpItem("ilwisapp\\find_datum_trans_params_methodpage.htm");
  create();
}

int FormTransformCoordinatesH_to_ctsXYZ::CsyInputCallBack(Event*)
{
  fcsInput->StoreData();
	cbDDD->StoreData();
  //stDatumName->SetVal("");
  if ("" == sCsyInput) {
    stCsyInput->SetVal("");
    csInput = CoordSystem();
	fCsyInpValid= false;
    fldCrd->Hide();
		cbDDD->Hide();
    fldLLDMS->Hide();
		fgLLDDD->Hide();
    return 0;
  }  
  try {
    FileName fnCsy(sCsyInput); 
    csInput = CoordSystem(fnCsy);
    if (csInput.fValid()) {
	  fCsyInpValid = true;
      stCsyInput->SetVal(csInput->sDescription);
	  sEllipsoid = csInput->pcsViaLatLon()->ell.sName;
	}
    if (csInput->pcsLatLon()) {
      fldCrd->Hide();
			cbDDD->Show();
			if (fDDD) {
				fldLLDMS->Hide();
				fgLLDDD->Show();
			}
			else {
				fgLLDDD->Hide();
				fldLLDMS->Show();
			}
    }
    else {
			cbDDD->Hide();
      fldCrd->Show();
			fgLLDDD->Hide();
			fldLLDMS->Hide();
    } 
  }
  catch (ErrorObject& err) {
    err.Show();
    stCsyInput->SetVal(SAFRemInvalidCoordSys);
    csInput = CoordSystem();
	fCsyInpValid = false;
    fldCrd->Hide();
    fldLLDMS->Hide();
		fgLLDDD->Hide();
  }  
  return RecomputeRewriteOutput();
  //return CoordInputCallBack(0);
}

int FormTransformCoordinatesH_to_ctsXYZ::CoordInputCallBack(Event*)
{
  fldCrd->StoreData();
  if (!csInput.fValid()) { // || !csOutput.fValid()) {
    stResult->SetVal("");
	fCsyInpValid = csInput.fValid();
    return 0;
  }  
  if (csInput->pcsLatLon())  {
		if (fDDD) {
			fgLLDDD->StoreData();
			ll = LatLon(frLatIn->rVal(),frLonIn->rVal());
		}
		else
			fldLLDMS->StoreData();		
    crd = csInput->cConv(ll);
  }
  else {
    fldCrd->StoreData();
  }  
  return RecomputeRewriteOutput();
}

int FormTransformCoordinatesH_to_ctsXYZ::DDDCallBack(Event*)
{
  cbDDD->StoreData();
  if ("" == sCsyInput) {
    stCsyInput->SetVal("");
    csInput = CoordSystem();
    fldCrd->Hide();
    fldLLDMS->Hide();
		//fldLLDDD->Hide();
		fgLLDDD->Hide();
    return 0;
  }  
  try {
    FileName fnCsy(sCsyInput); 
    csInput = CoordSystem(fnCsy);
    if (csInput.fValid())
      stCsyInput->SetVal(csInput->sDescription);
    fldCrd->Hide();
		if (fDDD) { ////for precision fldDDD must be replaced by 2 fldReals !!!!!!!!!!!!!!!
			frLatIn->SetVal(fldLLDMS->llVal().Lat);
			frLonIn->SetVal(fldLLDMS->llVal().Lon);
			fldLLDMS->Hide();
			fgLLDDD->Show();
		}
		else {
			LatLon llTmp = LatLon(frLatIn->rVal(),frLonIn->rVal());
			fldLLDMS->SetVal(llTmp);
			fgLLDDD->Hide();
			fldLLDMS->Show();
		}
  }
  catch (ErrorObject& err) {
    err.Show();
    stCsyInput->SetVal(SAFRemInvalidCoordSys);
    csInput = CoordSystem();
    fldCrd->Hide();
    fldLLDMS->Hide();
		fldLLDDD->Hide();
		fgLLDDD->Hide();
  }  
  return CoordInputCallBack(0);
}

int FormTransformCoordinatesH_to_ctsXYZ::HeightInCallBack(Event*)
{
  frHeightIn->StoreData();
//  stHeightOut->SetVal("");
  if (!csInput.fValid()) {// || !csOutput.fValid()) {
    stResult->SetVal("");
	fCsyInpValid = csInput.fValid();
//	fCsyOutValid = csOutput.fValid();
    return 0;
  }  
  fldCrd->StoreData();
  fldLLDMS->StoreData();
	fgLLDDD->StoreData();
//  sOutpHeight = String();
  if (abs(rHeightIn) > 100000000) {
    stResult->SetVal("Height out of bounds");
    return 0;
  }
  return RecomputeRewriteOutput();
}

int FormTransformCoordinatesH_to_ctsXYZ::RecomputeRewriteOutput()
{
  if ("" == sCsyInput) return 0;
  stResult->SetVal("");
  if (abs(rHeightIn) > 100000000) {
    stResult->SetVal("Height out of bounds");
    return 0;
  }
  String s = String();
  LatLonHeight llhIn;
  csvll = csInput->pcsViaLatLon();
  if (csvll == 0)
  {
		stResult->SetVal(SAFRemNo3DTransfPossible);
		return 0;
  }
  Ellipsoid ell = csvll->ell;

  cspr = csInput->pcsProjection();
  fProj = (0 != cspr);
  Coord cTmp = crd; //input {both formats, XY and LL)
  if (sEllipsoid.length())
  {
	LatLon llTmp;
	llTmp = ll;
	if (fDDD)
		llTmp = LatLon(rLatIn, rLonIn);
	//CoordCTS ctsIn;//, ctsOut;
	if (fProj)
		llTmp = cspr->llConv(cTmp);
	//else
	//	llTmp = LatLon(cTmp.y, cTmp.x);
	llhIn = LatLonHeight(llTmp, rHeightIn);
	ctsOut = ell.ctsConv(llhIn);
  }
  frXCoordOut->SetVal(ctsOut.x);
  frYCoordOut->SetVal(ctsOut.y);
  frZCoordOut->SetVal(ctsOut.z);
  return 1;
}

LRESULT Cmdtransformfromgeocentric(CWnd *parent, const String& s) {
	new FormTransformCoordinatesH_from_ctsXYZ(parent, s.scVal());
	return -1;
}

FormTransformCoordinatesH_from_ctsXYZ::FormTransformCoordinatesH_from_ctsXYZ(CWnd* mw, const char* sPar)
: FormInteractiveApplication(mw, SAFTitleTransf_from_Geocentric)
{
	int iImg = IlwWinApp()->iImage(".csy");
	HICON hIco = IlwWinApp()->ilSmall.ExtractIcon(iImg);
	SetIcon(hIco,FALSE);

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
      if (fn.sExt == "" || fn.sExt == ".csy")
        if (sCsyOutput == "") 
          sCsyOutput = fn.sFullName(false);
//        else 
//          sCsyOutput = fn.sFullName(false);
    }      
  }
  //fCsyInpValid = false;
  fCsyOutValid = false;
  fCrdInpValid = false;
  String sFill('x', 100);
	String sFill64('x', 64);
	String sFill38('x', 38);
	String sFill99('x', 99);
	rX_In = 0;
	rY_In = 0;
	rZ_In = 0;
  frXCoordIn = new FieldReal(root, SAFUiXcts, &rX_In, ValueRange(-1e12,1e12,0.001));
  //frXCoordIn->Align(stResult, AL_UNDER);
  frXCoordIn->SetIndependentPos();
  frXCoordIn->SetCallBack((NotifyProc)&FormTransformCoordinatesH_from_ctsXYZ::CoordInputCallBack);
  frYCoordIn = new FieldReal(root, SAFUiYcts, &rY_In, ValueRange(-1e12,1e12,0.001));
  //frYCoordIn->Align(frXCoordIn, AL_UNDER);
    frYCoordIn->SetIndependentPos();
    frYCoordIn->SetCallBack((NotifyProc)&FormTransformCoordinatesH_from_ctsXYZ::CoordInputCallBack);
	frZCoordIn = new FieldReal(root, SAFUiZcts, &rZ_In, ValueRange(-1e12,1e12,0.001));
  //frZCoordIn->Align(frYCoordIn, AL_UNDER);
    frZCoordIn->SetIndependentPos();
    frZCoordIn->SetCallBack((NotifyProc)&FormTransformCoordinatesH_from_ctsXYZ::CoordInputCallBack);
  fcsOutput = new FieldCoordSystem(root, SAFUiOutCoordSys, &sCsyOutput);
  fcsOutput->Align(frZCoordIn, AL_UNDER);
  fcsOutput->SetCallBack((NotifyProc)&FormTransformCoordinatesH_from_ctsXYZ::CsyOutputCallBack);
  stCsyOutput = new StaticText(root, sFill);
  stCsyOutput->SetIndependentPos();
  //stDatumName = new StaticText(root, sFill99);
  //stDatumName2->SetWidth(50);
  //stDatumName->Align(stCsyOutput, AL_UNDER);
  //stDatumName->SetIndependentPos();
  StaticText* stOutpCrd = new StaticText(root, SAFRemOutputCoord);
  stResult = new StaticText(root, sFill64);
  stResult->Align(stOutpCrd, AL_AFTER);
  stResult->SetIndependentPos();
  StaticText* stH = new StaticText(root, SAFRemHeightOutput);
  stH->Align(stOutpCrd, AL_UNDER);
  stHeightOut = new StaticText(root, sFill38);
  stHeightOut->Align(stH, AL_AFTER);
  stHeightOut->SetIndependentPos();
  SetHelpItem("ilwisapp\\find_datum_trans_params_methodpage.htm");
  create();
}

int FormTransformCoordinatesH_from_ctsXYZ::CoordInputCallBack(Event*)
{
  if (!csOutput.fValid()) {
    stResult->SetVal("");
	stHeightOut->SetVal("");
	//fCsyInpValid = csInput.fValid();
	fCsyOutValid = csOutput.fValid();
    return 0;
  }  
  frXCoordIn->StoreData();
    frYCoordIn->StoreData();
  frZCoordIn->StoreData();
   // crdOut = csOutput->cConv(csInput, crd); 
  return RecomputeRewriteOutput();
  //return 0;
}

int FormTransformCoordinatesH_from_ctsXYZ::CsyOutputCallBack(Event*)
{
  fcsOutput->StoreData();
  //stDatumName->SetVal("");
  stHeightOut->SetVal("");
  if ("" == sCsyOutput) {
    stCsyOutput->SetVal("");
    csOutput = CoordSystem();
	fCsyOutValid = false;
    return 0;
  }  
  try {
    FileName fnCsy(sCsyOutput); 
    csOutput = CoordSystem(fnCsy);
    if (csOutput.fValid()) {
		fCsyOutValid = true;
		stCsyOutput->SetVal(csOutput->sDescription);
		sEllipsoid = csOutput->pcsViaLatLon()->ell.sName;
	}
  }
  catch (ErrorObject& err) {
    err.Show();
    stCsyOutput->SetVal(SAFRemInvalidCoordSys);
    csOutput = CoordSystem();
  }
  return RecomputeRewriteOutput();
  //return CoordInputCallBack(0);
}

int FormTransformCoordinatesH_from_ctsXYZ::RecomputeRewriteOutput()
{
  stResult->SetVal("");
    frXCoordIn->StoreData();
	frYCoordIn->StoreData();
    frZCoordIn->StoreData();
 CoordCTS ctsIn;
 LatLonHeight llhOut;
 Coord cTmp;
 csvll = csOutput->pcsViaLatLon();
  if (csvll == 0)
  {
		stResult->SetVal(SAFRemNo3DTransfPossible);
		return 0;
  }
  Ellipsoid ell = csvll->ell;
  cspr = csOutput->pcsProjection();
  fProj = (0 != cspr);
  if (sEllipsoid.length())
  {	
	ctsIn.x = rX_In; ctsIn.y = rY_In; ctsIn.z = rZ_In;
	llhOut = ell.llhConv(ctsIn);
	LatLon llTmp = LatLon(llhOut.Lat, llhOut.Lon);
	if (fProj)
		cTmp = cspr->cConv(llTmp);
	else
		cTmp = Coord(llTmp.Lon, llTmp.Lat);
  }
  String s = csOutput->sValue(cTmp);
  if (csOutput->pcsLatLon() && (cTmp != crdUNDEF)) 
  {
	String sDDDLat = String("%.10f",cTmp.y);
	s &= String(" (");
	s &= sDDDLat;
	String sDDDLon = String(", %.10f",cTmp.x);
	s &= sDDDLon;
	s &= String(")");
  }
  stResult->SetVal(s);
  String sOutpHeight = String("%.5f",llhOut.rHeight);
  stHeightOut->SetVal(sOutpHeight);
  return 1;
}

LRESULT Cmdtransformviageocentric(CWnd *parent, const String& s) {
	new FormTransformCoordinatesH_via_ctsXYZ(parent, s.scVal());
	return -1;
}

FormTransformCoordinatesH_via_ctsXYZ::FormTransformCoordinatesH_via_ctsXYZ(CWnd* mw, const char* sPar)
: FormInteractiveApplication(mw, SAFTitleTransf_via_Geocentric)
{
	int iImg = IlwWinApp()->iImage(".csy");
	HICON hIco = IlwWinApp()->ilSmall.ExtractIcon(iImg);
	SetIcon(hIco,FALSE);

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
      if (fn.sExt == "" || fn.sExt == ".csy")
        if (sCsyInput == "") 
          sCsyInput = fn.sFullName(false);
        else 
          sCsyOutput = fn.sFullName(false);
    }      
  }
  fCsyInpValid = false;
  fCsyOutValid = false;
  fCrdInpValid = false;
  fHeightInpValid = false;
  String sFill('x', 100);
	String sFill90('x', 90);
	String sFill25('x', 25);
	iNrOfDatumParms = 0;
	rLatIn = 0;
	rLonIn = 0;
  fcsInput = new FieldCoordSystem(root, SAFUiInpCoordSys, &sCsyInput);
  fcsInput->SetCallBack((NotifyProc)&FormTransformCoordinatesH_via_ctsXYZ::CsyInputCallBack);
  stCsyInput = new StaticText(root, sFill);
  stCsyInput->SetIndependentPos();
  stDatumName1 = new StaticText(root, sFill90);
  stDatumName1->Align(stCsyInput, AL_UNDER);
  stDatumName1->SetIndependentPos();
	fDDD = false;
	cbDDD = new CheckBox(root, SAFRemDDD, &fDDD);
	cbDDD->SetCallBack((NotifyProc)&FormTransformCoordinatesH_via_ctsXYZ::DDDCallBack);
	cbDDD->Align(stDatumName1, AL_UNDER);
	cbDDD->SetIndependentPos();
  fldLLDMS = new FieldLatLon(root, SAFUiInpCoord, &ll);
  fldLLDMS->SetCallBack((NotifyProc)&FormTransformCoordinatesH_via_ctsXYZ::CoordInputCallBack);
	fldLLDMS->Align(cbDDD, AL_UNDER);
	fgLLDDD = new FieldGroup(cbDDD);// belonging to this group
  fgLLDDD->Align(cbDDD, AL_UNDER);
	StaticText *stLoc = new StaticText(fgLLDDD, SAFUiInpCoord);
	frLatIn = new FieldReal(fgLLDDD, "", &rLatIn, ValueRange(-90,90,0.000000001));
	frLatIn->Align(stLoc, AL_AFTER);
	frLatIn->SetIndependentPos();
	frLonIn = new FieldReal(fgLLDDD, "", &rLonIn, ValueRange(-360,360,0.000000001));
  frLonIn->Align(frLatIn, AL_AFTER);
  fldCrd = new FieldCoord(root, SAFUiInpCoord, &crd);
  fldCrd->Align(stDatumName1, AL_UNDER);
  fldCrd->SetCallBack((NotifyProc)&FormTransformCoordinatesH_via_ctsXYZ::CoordInputCallBack);
  fldCrd->SetIndependentPos();
  FieldBlank* fb = new FieldBlank(root,1);
  fb->Align(fldCrd, AL_UNDER);
  frHeightIn = new FieldReal(root, SAFRemHeightInput, &rHeightIn, ValueRange(-1e4,1e9,0.001));
  frHeightIn->Align(fb, AL_UNDER);
  frHeightIn->SetCallBack((NotifyProc)&FormTransformCoordinatesH_via_ctsXYZ::HeightInCallBack);
  frXCoordIn = new FieldReal(root, SAFUiX1Coord, &rX_In, ValueRange(-1e12,1e12,0.001));
  frXCoordIn->Align(frHeightIn, AL_UNDER);
  frXCoordIn->SetIndependentPos();
  frYCoordIn = new FieldReal(root, SAFUiY1Coord, &rY_In, ValueRange(-1e12,1e12,0.001));
  frYCoordIn->Align(frXCoordIn, AL_AFTER);
  frYCoordIn->SetIndependentPos();
  frZCoordIn = new FieldReal(root, SAFUiZ1Coord, &rZ_In, ValueRange(-1e12,1e12,0.001));
  frZCoordIn->Align(frYCoordIn, AL_AFTER);
    frZCoordIn->SetIndependentPos();
	stEllipsoidInput = new StaticText(root, sFill25);
	stEllipsoidInput->Align(frZCoordIn, AL_AFTER);
frXCoordOut = new FieldReal(root, SAFUiX2Coord, &rX_Out, ValueRange(-1e12,1e12,0.001));
  frXCoordOut->Align(frXCoordIn, AL_UNDER);
  frXCoordOut->SetIndependentPos();
  frYCoordOut = new FieldReal(root, SAFUiY2Coord, &rY_Out, ValueRange(-1e12,1e12,0.001));
  frYCoordOut->Align(frXCoordOut, AL_AFTER);
      frYCoordOut->SetIndependentPos();
	frZCoordOut = new FieldReal(root, SAFUiZ2Coord, &rZ_Out, ValueRange(-1e12,1e12,0.001));
  frZCoordOut->Align(frYCoordOut, AL_AFTER);
    frZCoordOut->SetIndependentPos();
	stEllipsoidOutput = new StaticText(root, sFill25);
	stEllipsoidOutput->Align(frZCoordOut, AL_AFTER);
  fcsOutput = new FieldCoordSystem(root, SAFUiOutCoordSys, &sCsyOutput);
  fcsOutput->Align(frXCoordOut, AL_UNDER);
  fcsOutput->SetCallBack((NotifyProc)&FormTransformCoordinatesH_via_ctsXYZ::CsyOutputCallBack);
  stCsyOutput = new StaticText(root, sFill);
  stCsyOutput->SetIndependentPos();
  stDatumName2 = new StaticText(root, sFill90);
  stDatumName2->Align(stCsyOutput, AL_UNDER);
  stDatumName2->SetIndependentPos();
  StaticText* stOutpCrd = new StaticText(root, SAFRemOutputCoord);
  fsResult = new FieldString(root, &sFill64, ES_AUTOHSCROLL|WS_TABSTOP|WS_GROUP|ES_READONLY);
  fsResult->Align(stOutpCrd, AL_AFTER);
  fsResult->SetIndependentPos();
	fsResult->SetWidth(180);
  StaticText* stH = new StaticText(root, SAFRemHeightOutput);
  stH->Align(stOutpCrd, AL_UNDER);
  fsHeightOut = new FieldString(root, &sFill38, ES_AUTOHSCROLL|WS_TABSTOP|WS_GROUP|ES_READONLY);
  fsHeightOut->Align(stH, AL_AFTER);
  fsHeightOut->SetIndependentPos();
  SetHelpItem("ilwisapp\\find_datum_trans_params_methodpage.htm");
  create();
}

int FormTransformCoordinatesH_via_ctsXYZ::CsyInputCallBack(Event*)
{
  fcsInput->StoreData();
	cbDDD->StoreData();
  stDatumName1->SetVal("");
  stEllipsoidInput->SetVal("");
  stEllipsoidOutput->SetVal("");
  iNrOfDatumParms = 0;
  if ("" == sCsyInput) {
    stCsyInput->SetVal("");
	fsHeightOut->SetVal("");
    csInput = CoordSystem();
	fCsyInpValid= false;
    fldCrd->Hide();
		cbDDD->Hide();
    fldLLDMS->Hide();
		fgLLDDD->Hide();
    return 0;
  }  
  try {
    FileName fnCsy(sCsyInput); 
    csInput = CoordSystem(fnCsy);
	bwdat1 = 0;
	badat1 = 0;
    if (csInput.fValid()) {
	  fCsyInpValid = true;
      stCsyInput->SetVal(csInput->sDescription);
	  Datum* datum1 = csInput->pcsViaLatLon()->datum;
	  if (datum1) {
		bwdat1 = dynamic_cast<BursaWolfDatum*>(datum1);
		badat1 = dynamic_cast<BadekasDatum*>(datum1);
	  }
	  else {
		bwdat1 = 0;
		badat1 = 0;
	  }
	  sEllipsoid1 = csInput->pcsViaLatLon()->ell.sName;
	  if(badat1) {	
		sDatum1 = badat1->sType();
		iNrOfDatumParms = 10;
	  }
	  else if (bwdat1) { // BursaWolfDatum is parent of BadekasDatum 
	    sDatum1 = bwdat1->sType();
		iNrOfDatumParms = 7;
	  }
	  else if (sEllipsoid1 == "WGS 84")
		sDatum1 = String("WGS84 Global ");
	  else
		  sDatum1 = String("No 3D- ");
	  sDatum1 &= String(" Datum definition");
	  if (bwdat1 || badat1)
		sDatum1 &= String(SAFUiNrOfDatumParms_i.scVal(), iNrOfDatumParms);
	  stDatumName1->SetVal(sDatum1);
	}
    if (csInput->pcsLatLon()) {
      fldCrd->Hide();
			cbDDD->Show();
			if (fDDD) {
				fldLLDMS->Hide();
				fgLLDDD->Show();
			}
			else {
				fgLLDDD->Hide();
				fldLLDMS->Show();
			}
    }
    else {
			cbDDD->Hide();
      fldCrd->Show();
			fgLLDDD->Hide();
			fldLLDMS->Hide();
    } 
  }
  catch (ErrorObject& err) {
    err.Show();
    stCsyInput->SetVal(SAFRemInvalidCoordSys);
    csInput = CoordSystem();
	fCsyInpValid = false;
    fldCrd->Hide();
    fldLLDMS->Hide();
		fgLLDDD->Hide();
  }  
  return RecomputeRewriteOutput();
  //return CoordInputCallBack(0);
}
  
int FormTransformCoordinatesH_via_ctsXYZ::CsyOutputCallBack(Event*)
{
  fcsOutput->StoreData();
  stDatumName2->SetVal("");
  stEllipsoidOutput->SetVal("");
  fsHeightOut->SetVal("");
  iNrOfDatumParms = 0;
  if ("" == sCsyOutput) {
    stCsyOutput->SetVal("");
    csOutput = CoordSystem();
	fCsyOutValid = false;
    return 0;
  }  
  try {
    FileName fnCsy(sCsyOutput); 
    csOutput = CoordSystem(fnCsy);
	bwdat2 = 0;
	badat2 = 0;
    if (csOutput.fValid()) {
		fCsyOutValid = true;
		stCsyOutput->SetVal(csOutput->sDescription);
		Datum* datum2 = csOutput->pcsViaLatLon()->datum;
		if (datum2) {
		  bwdat2 = dynamic_cast<BursaWolfDatum*>(datum2);
		  badat2 = dynamic_cast<BadekasDatum*>(datum2);
		}
	    else {
		  bwdat2 = 0;
		  badat2 = 0;
		}
		sEllipsoid2 = csOutput->pcsViaLatLon()->ell.sName;
		if(badat2) {	
			sDatum2 = badat2->sType();
			iNrOfDatumParms = 10;
		}
	    else if (bwdat2) { // BursaWolfDatum is parent BadekasDatum 
	      sDatum2 = bwdat2->sType();
		  iNrOfDatumParms = 7;
		}
		else if (sEllipsoid2 == "WGS 84")
		  sDatum2 = String("WGS84 Global ");
		else
		  sDatum2 = String("No 3D-");
		sDatum2 &= String(" Datum definition");
		if (bwdat2 || badat2)
			sDatum2 &= String(SAFUiNrOfDatumParms_i.scVal(), iNrOfDatumParms);
		stDatumName2->SetVal(sDatum2);
	}
  }
  catch (ErrorObject& err) {
    err.Show();
    stCsyOutput->SetVal(SAFRemInvalidCoordSys);
    csOutput = CoordSystem();
  }
  return RecomputeRewriteOutput();
  //return CoordInputCallBack(0);
}
  
int FormTransformCoordinatesH_via_ctsXYZ::CoordInputCallBack(Event*)
{
  if (!csInput.fValid() || !csOutput.fValid()) {
    fsResult->SetVal("");
	fsHeightOut->SetVal("");
	fCsyInpValid = csInput.fValid();
	fCsyOutValid = csOutput.fValid();
    return 0;
  }  
  Coord crdOut;
  if (csInput->pcsLatLon())  {
		if (fDDD) {
			fgLLDDD->StoreData();
			ll = LatLon(frLatIn->rVal(),frLonIn->rVal());
		}
		else
			fldLLDMS->StoreData();		
    crd = csInput->cConv(ll);
  }
  else {
    fldCrd->StoreData();
   // crdOut = csOutput->cConv(csInput, crd);
  }  
  return RecomputeRewriteOutput();
  //return 0;
}

int FormTransformCoordinatesH_via_ctsXYZ::DDDCallBack(Event*)
{
  cbDDD->StoreData();
  if ("" == sCsyInput) {
    stCsyInput->SetVal("");
    csInput = CoordSystem();
    fldCrd->Hide();
    fldLLDMS->Hide();
		//fldLLDDD->Hide();
		fgLLDDD->Hide();
    return 0;
  }  
  try {
    FileName fnCsy(sCsyInput); 
    csInput = CoordSystem(fnCsy);
    if (csInput.fValid())
      stCsyInput->SetVal(csInput->sDescription);
    fldCrd->Hide();
		if (fDDD) { ////for precision fldDDD must be replaced by 2 fldReals !!!!!!!!!!!!!!!
			frLatIn->SetVal(fldLLDMS->llVal().Lat);
			frLonIn->SetVal(fldLLDMS->llVal().Lon);
			fldLLDMS->Hide();
			fgLLDDD->Show();
		}
		else {
			LatLon llTmp = LatLon(frLatIn->rVal(),frLonIn->rVal());
			fldLLDMS->SetVal(llTmp);
			fgLLDDD->Hide();
			fldLLDMS->Show();
		}
  }
  catch (ErrorObject& err) {
    err.Show();
    stCsyInput->SetVal(SAFRemInvalidCoordSys);
    csInput = CoordSystem();
    fldCrd->Hide();
    fldLLDMS->Hide();
		fldLLDDD->Hide();
		fgLLDDD->Hide();
  }  
  return CoordInputCallBack(0);
}

int FormTransformCoordinatesH_via_ctsXYZ::HeightInCallBack(Event*)
{
  frHeightIn->StoreData();
  fsHeightOut->SetVal("");
  if (!csInput.fValid() || !csOutput.fValid()) {
    fsResult->SetVal("");
	fCsyInpValid = csInput.fValid();
	fCsyOutValid = csOutput.fValid();
    return 0;
  }  
  fldCrd->StoreData();
  fldLLDMS->StoreData();
	fgLLDDD->StoreData();
  sOutpHeight = String();
  if (abs(rHeightIn) > 1000000000) {
    fsResult->SetVal("Height out of bounds");
    return 0;
  }
  return RecomputeRewriteOutput();
}

int FormTransformCoordinatesH_via_ctsXYZ::RecomputeRewriteOutput()
{

  frXCoordIn->StoreData();
  frYCoordIn->StoreData();
  frZCoordIn->StoreData();
  frXCoordOut->StoreData();
  frYCoordOut->StoreData();
  frZCoordOut->StoreData();
  stEllipsoidInput->StoreData();
  stEllipsoidOutput->StoreData();
  fsResult->SetVal("");
  fsHeightOut->SetVal("");
  csvll1 = csInput->pcsViaLatLon();
  csvll2 = csOutput->pcsViaLatLon();
  //******************** check input coord system csvll1 and height *********************//
  if (abs(rHeightIn) > 1000000000) {
    fsResult->SetVal("Height out of bounds");
    return 0;
  }
  String s = String();
  LatLonHeight llhIn;
  LatLonHeight llhWGS;
  LatLonHeight llhOut;
  if (csvll1 == 0)
  {
		fsResult->SetVal(SAFRemNo3DTransfPossible);
		return 0;
  }
  Ellipsoid ell1 = csvll1->ell;
  stEllipsoidInput->SetVal(ell1.sName);
  Datum* datum1 = csvll1->datum; 
  bwdat1 = dynamic_cast<BursaWolfDatum*>(datum1);
  badat1 = dynamic_cast<BadekasDatum*>(datum1);
  MolodenskyDatum* mold1 = dynamic_cast<MolodenskyDatum*>(datum1);  

  bool fFromWGS = false; bool fToWGS = false;
  if (mold1) {
	fFromWGS = (mold1->dx == 0 && mold1->dy == 0 || mold1->dz == 0);
	fFromWGS &= (mold1->ell.sName == String("WGS 84"));
  }
  
  if (bwdat1 == 0 && badat1 == 0 && !fFromWGS) 
  {
		fsResult->SetVal(SAFRemNo3DTransfPossible);
		return 0;
  }
  fBursaWolf1 = (bwdat1 != 0); 
  fBadekas1 = (badat1 != 0); 
  cspr1 = csInput->pcsProjection(); 
  fProj1 = (0 != cspr1);
  
  Coord cTmp = crd; //input {both formats, XY and LL)
  LatLon llTmp;
  llTmp = ll;
  CoordCTS ctsIn, ctsOut;
  if (datum1)
  {
	if (fDDD)
		llTmp = LatLon(rLatIn, rLonIn);
	if (fProj1)
		llTmp = cspr1->llConv(cTmp);
	//else
	//	llTmp = LatLon(cTmp.y, cTmp.x);
	llhIn = LatLonHeight(llTmp, rHeightIn);
	ctsIn = ell1.ctsConv(llhIn);
	if (fBadekas1)
		llhWGS = badat1->llhToWGS84(llhIn);
	else if (fBursaWolf1)
		llhWGS = bwdat1->llhToWGS84(llhIn);
	else
		llhWGS = llhIn;
  }
    //******************** check output coord system csvll2 and height *********************//
  if (csvll2 == 0 )
  {
		fsResult->SetVal(SAFRemNo3DTransfPossible);
		return 0;
  }
  Ellipsoid ell2 = csvll2->ell;
  stEllipsoidOutput->SetVal(ell2.sName);
  Datum* datum2 = csvll2->datum;
  bwdat2 = dynamic_cast<BursaWolfDatum*>(datum2);
  badat2 = dynamic_cast<BadekasDatum*>(datum2);
  MolodenskyDatum* mold2 = dynamic_cast<MolodenskyDatum*>(datum2);
  if (mold2) {
    fToWGS = (mold2->dx == 0 && mold2->dy == 0 || mold2->dz == 0);
	fToWGS &= (mold2->ell.sName == String("WGS 84"));
  }
  if (bwdat2 == 0 && badat2 == 0 && !fToWGS)
  {
		fsResult->SetVal(SAFRemNo3DTransfPossible);
		return 0;
  }
  fBursaWolf2 = (bwdat2 != 0);
  fBadekas2 = (badat2 != 0);
  cspr2 = csOutput->pcsProjection();
  fProj2 = (0 != cspr2);
  if (datum1 && datum2)
  {
	if (fBadekas2)
		llhOut = badat2->llhFromWGS84(llhWGS);
	else if (fBursaWolf2)
		llhOut = bwdat2->llhFromWGS84(llhWGS);
	else
		llhOut = llhWGS;
	llTmp = LatLon(llhOut.Lat, llhOut.Lon);
	ctsOut = ell2.ctsConv(llhOut);
	if (fProj2)
		cTmp = cspr2->cConv(llTmp);
	else
		cTmp = Coord(llTmp.Lon, llTmp.Lat);
	frXCoordIn->SetVal(ctsIn.x);
    frYCoordIn->SetVal(ctsIn.y);
    frZCoordIn->SetVal(ctsIn.z);
    frXCoordOut->SetVal(ctsOut.x);
    frYCoordOut->SetVal(ctsOut.y);
    frZCoordOut->SetVal(ctsOut.z);
  }
  s = csOutput->sValue(cTmp);
  if (csOutput->pcsLatLon() && (cTmp != crdUNDEF)) 
  {
	String sDDDLat = String("%.10f",cTmp.y);
	s &= String(" (");
	s &= sDDDLat;
	String sDDDLon = String(", %.10f",cTmp.x);
	s &= sDDDLon;
	s &= String(")");
  }
  fsResult->SetVal(s);
  frHeightIn->StoreData();
  sOutpHeight = String("%.5f",llhOut.rHeight);
  fsHeightOut->SetVal(sOutpHeight);
  
  return 1;
}
