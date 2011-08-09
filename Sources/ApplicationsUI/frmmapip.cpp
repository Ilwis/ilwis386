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
/* $Log: /ILWIS 3.0/ApplicationForms/frmmapip.cpp $
 * 
 * 34    12-05-06 15:19 Retsios
 * Added sum and cnt to the maplist statistics functions
 * 
 * 33    9-01-06 12:59 Willem
 * Allow value maps for clustering
 * 
 * 32    5-01-06 18:22 Hendrikse
 * ClassifierSpectralAngle added in classifierform
 * 
 * 31    20-06-05 16:14 Retsios
 * MapListStatistics form: Now icon is displayed next to statistics
 * function, and the function name is more user-friendly.
 * 
 * 30    16-06-05 13:41 Retsios
 * mapliststatistics: corrected the use of the maplist-offset
 * 
 * 29    13-06-05 13:49 Retsios
 * mapliststatistics: an offset should display maplist bandnrs in a more
 * natural way to the user.
 * 
 * 28    13-06-05 13:39 Retsios
 * Performance improvement (makes sure maplist is not repeatedly opened
 * when form initializes)
 * Start and end band are always put in form when a new maplist is
 * selected (otherwise the user has to guess them by trial/error).
 * 
 * 27    17-05-05 10:49 Retsios
 * Changed stddev to std. The first term may be more beautiful, but the
 * command-line expects the second. Of course I could have spent a bit
 * more time "translating" the string, but ...
 * 
 * 26    22-04-05 10:28 Willem
 * Changed form title to "Maplist Statistics"
 * 
 * 25    21-04-05 13:00 Willem
 * Improved layout of mapliststatistics form
 * 
 * 24    21-04-05 12:25 Willem
 * Added form for MapMaplistStatistics
 * 
 * 23    7-09-01 18:41 Koolhoven
 * removed stImage in FormMapCreate, because it is no longer needed (image
 * is now just a normal domain with fixed range and precision), and it
 * disturbes the layout
 * 
 * 22    8/15/01 17:46 Hendrikse
 * debuged  the construction of expression strings mainly bec of
 * added:String("ClassifierPriorProb(%f,%S,%S)",rDist,sTable,sCol); break;
 * 
 * 21    8-08-01 9:48 Koolhoven
 * adaptions to allow start with maplist as first parameter
 * 
 * 20    7-08-01 19:48 Koolhoven
 * stretch now also allows a maplist as input
 * 
 * 19    6/01/01 20:31 Hendrikse
 * implemented new funcionality and callbacks in Classify form for
 * PriorProbability Table and Probab Column
 * 
 * 18    4/19/01 11:38 Hendrikse
 * removed the redundant CallBack int ClfCheckMult(Event *), because
 * refusal of values < 0.001 is now done by ValueRangeReal  both for
 * Multipl factor (BoxClassifier) and TresholdDistance (other Classifiers)
 * 
 * 17    10-01-01 19:01 Koolhoven
 * FormColorComp nr of colors has now a spinner
 * 
 * 16    10-01-01 18:58 Koolhoven
 * FormColorComp now only accepts maps of type Image, instead of also
 * value
 * 
 * 15    8-11-00 17:24 Koolhoven
 * use sFullNameQuoted() on input data to ensure selection in forms
 * 
 * 14    31-10-00 15:51 Hendrikse
 * changed default treshold checkbox status to "false"
 * 
 * 13    30-10-00 18:47 Hendrikse
 * added outputTable produced with MapCluster from form
 * improved Default dist and treshold in MapClassify form
 * 
 * 12    26-10-00 15:20 Koolhoven
 * improved layout of Cluster form
 * 
 * 11    3-08-00 12:43 Koolhoven
 * Cluster now has spinner for nr of clusters
 * 
 * 10    18-07-00 11:54a Martin
 * extensions are now correct so a icon will show in the map nameeedit
 * 
 * 9     14-01-00 17:27 Koolhoven
 * Include extension (and thus icon) in input map name
 * 
 * 8     30-11-99 12:30 Wind
 * changed "grey" to gray" for MapColorSeparation
 * 
 * 7     24-11-99 12:42p Martin
 * changed default domain from image to image.dom
 * 
 * 6     17-11-99 11:23a Martin
 * Valuerange Multiplication corrected, added check for illegal values
 * with multiplication
 * 
 * 5     19-10-99 2:37p Martin
 * Added valueranges to Cololrs fieldint and mult. fieldreal
 * 
 * 4     9/13/99 12:35p Wind
 * adapted :exec functions to use of quoted file names when building an
 * expression
 * 
 * 3     1-09-99 4:14p Martin
 * //->/*
 * 
 * 2     1-09-99 4:13p Martin
 * Palette check was wrong due to conflicting associativity
// Revision 1.5  1998/09/16 17:33:54  Wim
// 22beta2
//
// Revision 1.4  1997/09/24 16:36:42  Wim
// FormMapStretch upper limit 40.0
//
// Revision 1.3  1997-08-22 11:32:31+02  Wim
// Number of clusters now limited to 2..60
//
// Revision 1.2  1997-08-13 09:58:55+02  Wim
// ColorComposite only allow image, value, bool maps
//
/* Form Map Applications Image Processing
   by Wim Koolhoven
   (c) Ilwis System Development ITC
	Last change:  WK   24 Sep 97    6:31 pm
*/

//#include "Client\MainWindow\mainwind.h"
#include "Client\Headers\AppFormsPCH.h"
#include "Client\FormElements\fldlist.h"
#include "Client\FormElements\fldcs.h"
#include "Client\FormElements\fldmsmpl.h"
#include "Client\FormElements\FldOneSelectTextOnly.h"
#include "ApplicationsUI\frmmapip.h"
#include "Client\Base\WinThread.h"
#include "Client\Ilwis.h"
#include "Engine\Function\CLASSIFR.H"
#include "Headers\Hs\Mainwind.hs"

LRESULT OpenMapListColorComp(CWnd *wnd, const String& sCmd)
{
	new FormColorComp(wnd, sCmd.c_str());
	return -1;
}

//LRESULT Cmdmapcolorcomp(CWnd *wnd, const String& sCmd)
//{
//	CDC *dc = wnd->GetDC();
//
//	bool fPalette = (dc->GetDeviceCaps(RASTERCAPS) & RC_PALETTE) != 0;
//	if (fPalette) {
//		MessageBox(0,
//			TR("To display a map list as color composite,\nthe display settings in the Control Panel\nneed to be set on more than 256 colors").c_str(),
//			TR("Show Map List as Color Composite").c_str(),
//			MB_OK|MB_ICONSTOP);
//		return -1;
//	}
//	if (sCmd.size() == 0 )
//		OpenMapListColorComp(wnd);
//	else {
//		ParmList pm (sCmd);
//		String s = pm.sGet(0);
//		FileName fn(s);
//		if ("" == fn.sExt)
//			fn.sExt = ".mpl";
//		s = fn.sFullNameQuoted();
//		if (pm.fExist("noask")) //  || pm.fExist("quiet")) // quiet is always supplied by script. I leave it up to the user to decide whether to get the form or not.
//		{
//			WinThread* thr = new WinThread(IlwWinApp()->docTemplMapWindow(), s.c_str(), IlwisDocument::otNOASK);
//			if (thr) 
//				thr->CreateThread(0, 0);
//		}
//		else
//			IlwWinApp()->OpenDocumentAsMap(s.c_str());
//	}
//	return -1;
//}

FormColorComp::FormColorComp(CWnd* mw, const char* sPar)
: FormMapCreate(mw, TR("Color Composite"))
{
  iRgbHsi = 0;
  iMethod = 1;
  iInterpol = 0;
  iColors = 100;
  CDC *cdc = mw->GetWindowDC();
  int iRet = cdc->GetDeviceCaps(RASTERCAPS) & RC_PALETTE;
  bool fPalette = ( cdc->GetDeviceCaps(RASTERCAPS) & RC_PALETTE ) != 0;
  f24Bit = !fPalette;
  fPerc = true;
  
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
        if (sMapRed == "")
          sMapRed = fn.sFullNameQuoted(false);
        else if (sMapGreen == "")
          sMapGreen = fn.sFullNameQuoted(false);
        else if (sMapBlue == "")
          sMapBlue = fn.sFullNameQuoted(false);
        else  
          sOutMap = fn.sFullName(false);
    }
  }
  FieldGroup* fgTop = new FieldGroup(root);

  cb24Bit = new CheckBox(fgTop, TR("&24 bit"), &f24Bit);
  cb24Bit->SetCallBack((NotifyProc)&FormColorComp::CallBack24Bit);

  rgRgbHsi = new RadioGroup(fgTop, "", &iRgbHsi);
  rgRgbHsi->Align(cb24Bit, AL_UNDER);
  rgRgbHsi->SetCallBack((NotifyProc)&FormColorComp::RgbHsiCallBack);
  new RadioButton(rgRgbHsi, TR("&RGB"));
  new RadioButton(rgRgbHsi, TR("&HSI"));

  rgMethod = new RadioGroup(fgTop, "", &iMethod);
  rgMethod->Align(cb24Bit, AL_UNDER);
  rgMethod->SetCallBack((NotifyProc)&FormColorComp::StretchCallBack);
  RadioButton* rbStandard = new RadioButton(rgMethod, TR("&Standard"));
  RadioButton* rbDynamic = new RadioButton(rgMethod, TR("&Dynamic"));

  FieldGroup* fgDynamic = new FieldGroup(rbDynamic, true);
  new FieldInt(fgDynamic, TR("&Colors"), &iColors, ValueRange(1,255), true);

  fgStretch = new FieldGroup(root, true);
  fgStretch->Align(cb24Bit, AL_AFTER);

  iInterpol = 0;  
  RadioGroup* rgInterpol = new RadioGroup(fgStretch, "", &iInterpol);
  new RadioButton(rgInterpol, TR("&Linear Stretching"));
  new RadioButton(rgInterpol, TR("&Histogram Equalization"));

  fPerc = true;
  cbPerc = new CheckBox(fgStretch, TR("&Percentage"), &fPerc);
  cbPerc->SetCallBack((NotifyProc)&FormColorComp::PercCallBack);
  cbPerc->Align(rgInterpol, AL_UNDER);

  fgRGB = new FieldGroup(root);  
  fgRGB->Align(fgTop, AL_UNDER);
  FieldDataType* fmRed = new FieldDataType(fgRGB, TR("&Red Band"), &sMapRed,
                             new MapListerDomainType(dmIMAGE), true);
  fmRed->SetWidth(140);
  rrRed = RangeReal(0,255);
  rPercRed = 1;
  fRangeRed = new FieldRangeReal(fgRGB, "", &rrRed);
  fRangeRed->Align(fmRed, AL_AFTER);
  fPercRed = new FieldReal(fgRGB, "", &rPercRed, ValueRange(0,40,0.01));
  fPercRed->Align(fmRed, AL_AFTER);
  
  FieldDataType* fmGreen = new FieldDataType(fgRGB, TR("&Green Band"), &sMapGreen,
                             new MapListerDomainType(dmIMAGE), true);
  fmGreen->SetWidth(140);
  fmGreen->Align(fmRed, AL_UNDER);
  rrGreen = RangeReal(0,255);
  rPercGreen = 1;
  fRangeGreen = new FieldRangeReal(fgRGB, "", &rrGreen);
  fRangeGreen->Align(fmGreen, AL_AFTER);
  fPercGreen = new FieldReal(fgRGB, "", &rPercGreen, ValueRange(0,40,0.01));
  fPercGreen->Align(fmGreen, AL_AFTER);
  
  FieldDataType* fmBlue = new FieldDataType(fgRGB, TR("&Blue Band"), &sMapBlue,
                             new MapListerDomainType(dmIMAGE), true);
  fmBlue->SetWidth(140);
  fmBlue->Align(fmGreen, AL_UNDER);
  rrBlue = RangeReal(0,255);
  rPercBlue = 1;
  fRangeBlue = new FieldRangeReal(fgRGB, "", &rrBlue);
  fRangeBlue->Align(fmBlue, AL_AFTER);
  fPercBlue = new FieldReal(fgRGB, "", &rPercBlue, ValueRange(0,40,0.01));
  fPercBlue->Align(fmBlue, AL_AFTER);

  fgHSI = new FieldGroup(root);  
  fgHSI->Align(fgTop, AL_UNDER);
  new FieldDataType(fgHSI, TR("&Hue"), &sMapHue,
                           new MapListerDomainType(dmIMAGE), true);
  new FieldDataType(fgHSI, TR("&Saturation"), &sMapSat,
                             new MapListerDomainType(dmIMAGE), true);
  new FieldDataType(fgHSI, TR("&Intensity"), &sMapInt,
                             new MapListerDomainType(dmIMAGE), true);

  FieldBlank* fb = new FieldBlank(root, 0);
  fb->Align(fgRGB, AL_UNDER);
  initMapOut(false, false);
  SetHelpItem("ilwisapp\\color_composite_dialogbox.htm");
  create();
}                    

int FormColorComp::CallBack24Bit(Event*)
{
  cb24Bit->StoreData();
  if (f24Bit) {
    rgMethod->Hide();
    rgRgbHsi->Show();
    return RgbHsiCallBack(0);
  }
  else {
    rgMethod->Show();
    rgRgbHsi->Hide();
    fgRGB->Show();
    fgHSI->Hide();
    return StretchCallBack(0);
  }
}

int FormColorComp::RgbHsiCallBack(Event*)
{
  rgRgbHsi->StoreData();
  if (!f24Bit)
    return 0;
  switch (iRgbHsi) {
    case 0: 
      fgRGB->Show();
      fgHSI->Hide();
      fgStretch->Show();
      PercCallBack(0);
      break;
    case 1:  
      fgRGB->Hide();
      fgHSI->Show();
      fgStretch->Hide();
      fRangeRed->Hide();
      fRangeGreen->Hide();
      fRangeBlue->Hide();
      fPercRed->Hide();
      fPercGreen->Hide();
      fPercBlue->Hide();
      break;
  }
  return 0;  
}

int FormColorComp::StretchCallBack(Event*)
{
  if (f24Bit) {
    if (iRgbHsi != 0) {
      fgStretch->Hide();
      return 0;
    }  
    fgStretch->Show();
    PercCallBack(0);
    return 0;
  }  
  rgMethod->StoreData();
  switch (iMethod) {
    case 0: 
      fgStretch->Show();
      break;
    case 1:  
      fgStretch->Hide();
      break;
  }
  PercCallBack(0);
  return 0;
}

int FormColorComp::PercCallBack(Event*)
{
  rgMethod->StoreData();
  if ((!f24Bit && iMethod == 1) || (f24Bit && iRgbHsi == 1)) {
    fRangeRed->Hide();
    fRangeGreen->Hide();
    fRangeBlue->Hide();
    fPercRed->Hide();
    fPercGreen->Hide();
    fPercBlue->Hide();
  }
  else {
    cbPerc->StoreData();
    if (fPerc) {
      fRangeRed->Hide();
      fRangeGreen->Hide();
      fRangeBlue->Hide();
      fPercRed->Show();
      fPercGreen->Show();
      fPercBlue->Show();
    }
    else {
      fRangeRed->Show();
      fRangeGreen->Show();
      fRangeBlue->Show();
      fPercRed->Hide();
      fPercGreen->Hide();
      fPercBlue->Hide();
    }
  }
  return 0;
}

int FormColorComp::exec() 
{
  FormMapCreate::exec();
  bool fStretch = true;
  String sExpr, sApplic;
  FileName fn(sOutMap);
  FileName fnMapRed(sMapRed); 
  sMapRed = fnMapRed.sRelativeQuoted(false,fn.sPath());
  FileName fnMapGreen(sMapGreen); 
  sMapGreen = fnMapGreen.sRelativeQuoted(false,fn.sPath());
  FileName fnMapBlue(sMapBlue); 
  sMapBlue = fnMapBlue.sRelativeQuoted(false,fn.sPath());
  String sMapList("mlist(%S,%S,%S)", sMapRed, sMapGreen, sMapBlue);
  if (f24Bit) {
    switch (iRgbHsi) {
      case 0:
        switch (iInterpol) {
          case 0:
            sApplic = "MapColorComp24Linear";
            break;
          case 1:
            sApplic = "MapColorComp24HistEq";
            break;
        }
        break;
      case 1: {
        FileName fnMapHue(sMapHue); 
        sMapHue = fnMapHue.sRelativeQuoted(false,fn.sPath());
        FileName fnMapSat(sMapSat); 
        sMapSat = fnMapSat.sRelativeQuoted(false,fn.sPath());
        FileName fnMapInt(sMapInt); 
        sMapInt = fnMapInt.sRelativeQuoted(false,fn.sPath());
        sMapList = String("mlist(%S,%S,%S)", sMapHue, sMapSat, sMapInt);
        fStretch = false;
        sApplic = "MapColorComp24HSI";
      } break;  
    }
  }
  else {
    switch (iMethod) {
      case 0: {
        switch (iInterpol) {
          case 0:
            sApplic = "MapColorCompLinear";
            break;
          case 1:
            sApplic = "MapColorCompHistEq";
            break;
        }
      } break;
      case 1: {
        String s = sMapList;
        sMapList = String("%S,%i", s, iColors);
        sApplic = "MapHeckbert";
        fStretch = false;
      }  break;
    }
  }  
  if (!fStretch) 
    sExpr = String("%S(%S)", 
                   sApplic, sMapList);
  else if (fPerc)
    sExpr = String("%S(%S,%f,%f,%f)",
                   sApplic, sMapList, rPercRed, rPercGreen, rPercBlue);
  else               
    sExpr = String("%S(%S,%f:%f,%f:%f,%f:%f)",
                   sApplic, sMapList, rrRed.rLo(), rrRed.rHi(), 
                   rrGreen.rLo(), rrGreen.rHi(), rrBlue.rLo(), rrBlue.rHi());
  execMapOut(sExpr);  
  return 0;
}

LRESULT Cmdclassify(CWnd *wnd, const String& s)
{
	new FormClassifyMap(wnd, s.c_str());
	return -1;
}

FormClassifyMap::FormClassifyMap(CWnd* mw, const char* sPar)
: FormMapCreate(mw, TR("Classification"))
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
      if (fn.sExt == "" || fn.sExt == ".sms")
        if (sMap == "") {
          sMap = fn.sFullNameQuoted(false);
          continue;
        }
			if (sTable == "" && fn.sExt == "" || fn.sExt == ".tbt")
        sTable = fn.sFullNameQuoted(false);
      if (fn.sExt == "" || fn.sExt == ".mpr")
        sOutMap = fn.sFullName(false);
    }
  }
  fssMap = new FieldSampleSet(root, TR("&Sample Set"), &sMap);
	fssMap->SetCallBack((NotifyProc)&FormClassifyMap::SampleSetChanged);
  iClf = 0;
  rg = new RadioGroup(root, TR("&Classification Method:"), &iClf);
  new RadioButton(rg, TR("&Box Classifier"));
  new RadioButton(rg, TR("&Minimum &Distance"));
  new RadioButton(rg, TR("Minimum &Mahalanobis Distance"));
  new RadioButton(rg, TR("Maximum &Likelihood"));
	new RadioButton(rg, TR("&Spectral Angle"));
	RadioButton *rbPriorProb = new RadioButton(rg, TR("Prior &Probability"));
  rg->SetCallBack((NotifyProc)&FormClassifyMap::ClfCallBack);
  rg->SetIndependentPos();
  fDist = false;
  rDist = 100;
  cbDist = new CheckBox(root, TR("&Threshold Distance"), &fDist);
  cbDist->Align(rg, AL_UNDER);
  new FieldReal(cbDist, "", &rDist, ValueRangeReal(0.001, 1e6, 0.001));
  rFact = sqrt(3.0);
  frFact = new FieldReal(root, TR("Multiplication &Factor"), &rFact, ValueRangeReal(0.001, 1e6, 0.001));
  frFact->Align(rg, AL_UNDER);
	FieldGroup *fgPrior = new FieldGroup(rbPriorProb);
	fgPrior->Align(frFact, AL_UNDER);
	fldPriorProbTbl = new FieldTable(fgPrior, TR("&Table"), &sTable);
	fldPriorProbTbl->SetCallBack((NotifyProc)&FormClassifyMap::PriorProbTblChanged);
  fldPriorProbCol = new FieldColumn(fgPrior, TR("&Column"), Table(), &sCol, dmVALUE);
	new FieldBlank(fgPrior, 0.25);
  initMapOut(false,false);
  SetHelpItem("ilwisapp\\classify_dialogbox.htm");
  create();
}                    

int FormClassifyMap::SampleSetChanged(Event*)
{
	FileName fnOldSms(sMap);

	fssMap->StoreData();
	if (sMap.length() == 0)
		return 0;

	try
	{
		FileName fnSms(sMap, ".sms", false);
		SampleSet sms(fnSms);
		if (fnOldSms != fnSms)
			fldPriorProbTbl->SetVal("");
		fldPriorProbTbl->SetDomain(sms->dm()); // does also FillDir()
	}
	catch (ErrorObject&) {}
		
	return 0;
}

int FormClassifyMap::PriorProbTblChanged(Event*)
{
  fldPriorProbTbl->StoreData();

  if (sTable.length() == 0)
		return 0;

	try
	{
		fldPriorProbCol->SetVal("");
		FileName fnTbl(sTable, ".tbt", false);
		if (fnTbl.fValid())
			fldPriorProbCol->FillWithColumns(fnTbl);
	}
	catch (ErrorObject& err)
	{
		err.Show();
	}

	return 0;
}

int FormClassifyMap::ClfCallBack(Event*)
{
  rg->StoreData();
  if (0 == iClf) {
    frFact->Show();
    cbDist->Hide();
  }
  else {
    frFact->Hide();
    cbDist->Show();
  }
  return 0;
}


int FormClassifyMap::exec() 
{
  FormMapCreate::exec();
  String sExpr;
  FileName fn(sOutMap);
  FileName fnMap(sMap); 
  sMap = fnMap.sRelativeQuoted(false,fn.sPath());
	FileName fnTable(sTable); 
  sTable = fnTable.sRelativeQuoted(false,fn.sPath());
  String sClf;
  if (0 == iClf)
    sClf = String("ClassifierBox(%f)", rFact);
  else {
    String s;
		if (fDist)
	    switch (iClf) {
	      case 1: s = String("ClassifierMinDist(%f)",rDist); break;
	      case 2: s = String("ClassifierMinMahaDist(%f)",rDist); break;
	      case 3: s = String("ClassifierMaxLikelihood(%f)",rDist); break;
				case 4: s = String("ClassifierSpectralAngle(%f)",rDist); break;
				case 5: s = String("ClassifierPriorProb(%f,%S,%S)",rDist,sTable,sCol); break;
	    }
		else
	    switch (iClf) {
	      case 1: s = String("ClassifierMinDist()"); break;
	      case 2: s = String("ClassifierMinMahaDist()"); break;
	      case 3: s = String("ClassifierMaxLikelihood()"); break;
				case 4: s = String("ClassifierSpectralAngle()"); break;
				case 5: s = String("ClassifierPriorProb(%S,%S)",sTable,sCol); break;
	    }
		sClf = s; /*
    if (fDist)
      sClf = String("%S%f", s, rDist);
    else  */
      sClf = s;  /*
		if (iClf == 4)  
			sClf &= String("%S,%S",sTable,sCol); */
  }
  sExpr = String("MapClassify(%S,%S)",
                  sMap,sClf);
  execMapOut(sExpr);  
  return 0;
}

LRESULT Cmdstretch(CWnd *wnd, const String& s)
{
	new FormMapStretch(wnd, s.c_str());
	return -1;
}

FormMapStretch::FormMapStretch(CWnd* mw, const char* sPar)
: FormMapCreate(mw, TR("Stretch"))
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
      else if (fn.sExt == "" || fn.sExt == ".mpr" || fn.sExt == ".mpl")
        if (sMap == "")
          sMap = fn.sFullNameQuoted(true);
        else  
          sOutMap = fn.sFullName(false);
    }
  }
  fldMap = new FieldDataType(root, TR("&Raster Map"), &sMap, new MapListerDomainType(dmVALUE|dmIMAGE,false,true),true);
  fldMap->SetCallBack((NotifyProc)&FormMapStretch::MapCallBack);
  iMethod = 0;
  iFrom = 1;
  rPerc = 1;
  iIntervals = 256;
  sDomain = "image.dom";
  rr = RangeReal(0,255);
  rgMethod = new RadioGroup(root, TR("Stretch &Method:"), &iMethod);
  rgMethod->SetIndependentPos();
  new RadioButton(rgMethod, TR("&Linear Stretching"));
  RadioButton* rbHistEq = new RadioButton(rgMethod, TR("&Histogram Equalization"));
  (new FieldInt(rbHistEq, TR("&Intervals"), &iIntervals))->SetIndependentPos();
  rgMethod->SetCallBack((NotifyProc)&FormMapStretch::MethodCallBack);
  RadioGroup* rg = new RadioGroup(root, TR("Stretch &From:"), &iFrom);
  rg->Align(rgMethod, AL_UNDER);
  RadioButton* rb = new RadioButton(rg, TR("&Min, Max"));
  frrMinMax = new FieldRangeReal(rb, "", &rr);
  rb = new RadioButton(rg, TR("&Percentage")); 
  frPerc = new FieldReal(rb, "", &rPerc, ValueRange(0,40,0.01));
  FieldBlank* fb = new FieldBlank(root,0);
  fb->Align(rb, AL_UNDER);
  initMapOut(false, (long)(dmVALUE|dmIMAGE));
  SetHelpItem("ilwisapp\\stretch_dialog_box.htm");
  create();
}                    

int FormMapStretch::exec() 
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
    case 0: 
      sMethod = "MapStretchLinear";
      break;
    case 1:  
      sMethod = "MapStretchHistEq";
      break;
  }
  switch (iFrom) {
    case 0: 
      sExpr = String("%S(%S,%f:%f,", 
                       sMethod, sMap, rr.rLo(), rr.rHi());
      break;
    case 1:  
      sExpr = String("%S(%S,%f,", 
                     sMethod, sMap, rPerc);
      break;
    }  
  switch (iMethod) {
    case 0:  {
      String sValueRange;
      try {
        Domain dm(sDomain);
        if (!dm->pdbit() && (dm->fnObj.sFile != "bool")) {
          if (dm->pdvi()) 
            sValueRange = vr->sRange();
          else if (dm->pdvr()) 
            sValueRange = vr->sRange();
        }  
      }
      catch (const ErrorObject&) {
      }  
      if (sValueRange.length()) 
        sExpr &= String("%S,%S)", sDomain, sValueRange);
      else                 
        sExpr &= String("%S)", sDomain);
      break;
    }
    case 1: 
      sExpr &= String("%li)", iIntervals);
      sDomain = "";
      break;
  }  
  if (fOutMapList)
    sExpr = String("MapListApplic(%S,%S)", sMapList, sExpr);
  execMapOut(sExpr);  
  return 0;
}

int FormMapStretch::MapCallBack(Event*) 
{
  fldMap->StoreData();
  try {
    FileName fnMap(sMap);
    if (fnMap.sFile == "")
      return 0;
    if (".mpl" == fnMap.sExt && "" == fnMap.sSectionPostFix)
    {
      MapList mpl(fnMap);
      mp = mpl->map(mpl->iLower());
    }
    else 
      mp = Map(fnMap);
    rr = mp->rrPerc1();
    if (rr.rHi() > rr.rLo())
      frrMinMax->SetVal(rr);
  }  
  catch (ErrorObject&) {
  }
  return 0;
}

int FormMapStretch::MethodCallBack(Event*) 
{
  if (!mp.fValid())
    try {
      FileName fnMap(sMap);
      if (fnMap.sFile != "")
        mp = Map(fnMap);
    }  
    catch (ErrorObject&) {
    }
  rgMethod->StoreData();
  switch (iMethod) {
    case 0: 
      if (sDomain.length() == 0)
        sDomain = "image";
      frPerc->SetVal(1);
      if (mp.fValid()) {
        rr = mp->rrPerc1();
        if (rr.rHi() > rr.rLo())
          frrMinMax->SetVal(rr);
      }
      fdc->Show();
      stRemark->Show();
      break;
    case 1:  
      frPerc->SetVal(0);
      fdc->Hide();
      fvr->Hide();
      stRemark->Hide();
      break;
  }
  return 0;
}

LRESULT Cmdcluster(CWnd *wnd, const String& s)
{
	new FormMapCluster(wnd, s.c_str());
	return -1;
}

FormMapCluster::FormMapCluster(CWnd* mw, const char* sPar)
: FormMapCreate(mw, TR("Clustering"))
{
  iMaps = 3;
  iClasses = 10;
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
          sMap1 = fn.sFullNameQuoted(true);
        else if (sMap2 == "") {
          sMap2 = fn.sFullNameQuoted(true);
          iMaps = 2;
        }  
        else if (sMap3 == "") {
          sMap3 = fn.sFullNameQuoted(true);
          iMaps = 3;
        }  
        else if (sMap4 == "") {
          sMap4 = fn.sFullNameQuoted(true);
          iMaps = 4;
        }  
        else  if (sOutMap == "") 
          sOutMap = fn.sFullName(false);
				else  if (sOutTable == "" && fn.sExt == "" || fn.sExt == ".tbt")
					sOutTable = fn.sFullName(false);
    }
  }
  iMaps -= 1;
  new StaticText(root, TR("&Number of Input Maps"));
  rgMaps = new RadioGroup(root, "", &iMaps, true);
  rgMaps->SetIndependentPos();
  rgMaps->SetCallBack((NotifyProc)&FormMapCluster::MapsCallBack);
  new RadioButton(rgMaps, "&1");
  new RadioButton(rgMaps, "&2");
  new RadioButton(rgMaps, "&3");
  new RadioButton(rgMaps, "&4");
  fm1 = new FieldDataType(root, "", &sMap1, new MapListerDomainType(dmIMAGE | dmVALUE),true);
	fm1->SetIndependentPos();
  fm2 = new FieldDataType(root, "", &sMap2, new MapListerDomainType(dmIMAGE | dmVALUE),true);
	fm2->SetIndependentPos();
  fm3 = new FieldDataType(root, "", &sMap3, new MapListerDomainType(dmIMAGE | dmVALUE),true);
	fm3->SetIndependentPos();
  fm4 = new FieldDataType(root, "", &sMap4, new MapListerDomainType(dmIMAGE | dmVALUE),true);
	fm4->SetIndependentPos();
  new FieldInt(root, TR("Number of &Clusters"), &iClasses, ValueRange(2,60), true);
  initMapOut(false, false);
	CheckBox* cbTb = new CheckBox(root, TR("&Output Table"), &fStatTable);
  ftc = new FieldTableCreate(cbTb, "", &sOutTable);
  SetHelpItem("ilwisapp\\cluster_dialog_box.htm");
  create();
}                    

int FormMapCluster::MapsCallBack(Event*)
{
  rgMaps->StoreData();
  switch (iMaps) {  // lots of fall throughs
    case 0: 
      fm2->Hide();
    case 1: 
      fm3->Hide();
    case 2: 
      fm4->Hide();
  }
  switch (iMaps) {
    case 3: 
      fm4->Show();
    case 2: 
      fm3->Show();
    case 1: 
      fm2->Show();
    case 0:
      fm1->Show();
  }
  return 0;
}

int FormMapCluster::exec() 
{
  FormMapCreate::exec();
  iMaps += 1;
  FileName fn(sOutMap);
  FileName fnMap1(sMap1);
  sMap1 = fnMap1.sRelativeQuoted(false,fn.sPath());
  FileName fnMap2(sMap2); 
  sMap2 = fnMap2.sRelativeQuoted(false,fn.sPath());
  FileName fnMap3(sMap3); 
  sMap3 = fnMap3.sRelativeQuoted(false,fn.sPath());
  FileName fnMap4(sMap4); 
  sMap4 = fnMap4.sRelativeQuoted(false,fn.sPath());
  String sMapList;
  switch (iMaps) {
    case 1:
      sMapList = String("mlist(%S)", sMap1);
      break;
    case 2:
      sMapList = String("mlist(%S,%S)", sMap1, sMap2);
      break;
    case 3:
      sMapList = String("mlist(%S,%S,%S)", sMap1, sMap2, sMap3);
      break;
    case 4:
      sMapList = String("mlist(%S,%S,%S,%S)", sMap1, sMap2, sMap3, sMap4);
      break;
  }
  String sExpr;
  FileName fnOutTable(sOutTable);
  sOutTable = fnOutTable.sRelativeQuoted(false,fn.sPath());
  if (!fStatTable)
		sExpr = String("MapCluster(%S,%i)", sMapList, iClasses);
	else
		sExpr = String("MapCluster(%S,%i, %S)", sMapList, iClasses, sOutTable);
  execMapOut(sExpr);  
  return 0;
}

LRESULT Cmdcolorsep(CWnd *wnd, const String& s)
{
	new FormMapColorSep(wnd, s.c_str());
	return -1;
}

FormMapColorSep::FormMapColorSep(CWnd* mw, const char* sPar)
: FormMapCreate(mw, TR("Color Separation"))
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
          sOutMap = fn.sFullName(true);
    }
  }
  new FieldDataType(root, TR("&Raster Map"), &sMap, new MapListerDomainType(dmPICT|dmCOLOR), true);
  iOption = 0;
  RadioGroup* rg = new RadioGroup(root, "", &iOption);
  rg->SetIndependentPos();
  RadioButton *rbR = new RadioButton(rg, TR("&Red"));
  RadioButton *rbG = new RadioButton(rg, TR("&Green"));
  rbG->Align(rbR, AL_AFTER);
  RadioButton *rbB = new RadioButton(rg, TR("&Blue"));
  rbB->Align(rbG, AL_AFTER);
  RadioButton *rbY = new RadioButton(rg, TR("&Yellow"));
  rbY->Align(rbR, AL_UNDER);
  RadioButton *rbM = new RadioButton(rg, TR("&Magenta"));
  rbM->Align(rbY, AL_AFTER);
  RadioButton *rbC = new RadioButton(rg, TR("&Cyan"));
  rbC->Align(rbM, AL_AFTER);
  RadioButton *rbH = new RadioButton(rg, TR("&Hue"));
  rbH->Align(rbY, AL_UNDER);
  RadioButton *rbS = new RadioButton(rg, TR("&Saturation"));
  rbS->Align(rbH, AL_AFTER);
  RadioButton *rbI = new RadioButton(rg, TR("&Intensity"));
  rbI->Align(rbS, AL_AFTER);
  RadioButton *rbGr = new RadioButton(rg, TR("&Gray"));
  rbGr->Align(rbH, AL_UNDER);
  
  initMapOut(false, false);
  SetHelpItem("ilwisapp\\color_separation_dialog_box.htm");
  create();
}                    

int FormMapColorSep::exec() 
{
  FormMapCreate::exec();
  String sExpr;
  FileName fn(sOutMap);
  FileName fnMap(sMap); 
  sMap = fnMap.sRelativeQuoted(false,fn.sPath());
  String sOption;
  switch (iOption) {
    case 0: sOption = "Red"; break;
    case 1: sOption = "Green"; break;
    case 2: sOption = "Blue"; break;
    case 3: sOption = "Yellow"; break;
    case 4: sOption = "Magenta"; break;
    case 5: sOption = "Cyan"; break;
    case 6: sOption = "Hue"; break;
    case 7: sOption = "Saturation"; break;
    case 8: sOption = "Intensity"; break;
    case 9: sOption = "Gray"; break;
  }
  sExpr = String("MapColorSep(%S,%S)", sMap, sOption);
  execMapOut(sExpr);  
  return 0;
}

const int iMLSFUNCS = 7;
class FieldMplStatsFuncSimple: public FieldOneSelect
{
public:
	FieldMplStatsFuncSimple(FormEntry* par, String** sFunc, String sDefaultFunction)
		: FieldOneSelect(par, (long*)sFunc, false)
		, m_sDefaultFunction(sDefaultFunction)
	{
		SetWidth(75);
		int i = 0;
		ts[i].sFun = "Avg";
		ts[i++].sName = "Average.fun";
		ts[i].sFun = "Sum";
		ts[i++].sName = "Sum.fun";
		ts[i].sFun = "Cnt";
		ts[i++].sName = "Count.fun";
		ts[i].sFun = "Med";
		ts[i++].sName = "Median.fun";
		ts[i].sFun = "Min";
		ts[i++].sName = "Minimum.fun";
		ts[i].sFun = "Max";
		ts[i++].sName = "Maximum.fun";
		ts[i].sFun = "Std";
		ts[i++].sName = "Std Deviation.fun";
	}
	void create() {
		FieldOneSelect::create();
		for (int i = 0; i < iMLSFUNCS; ++i) {
			int id = ose->AddString(ts[i].sName.sVal());
			SendMessage(*ose,CB_SETITEMDATA,id,(long)&ts[i].sFun);
		}
		if (m_sDefaultFunction.length())
		{
			int i = 0;
			while (i < iMLSFUNCS && !fCIStrEqual(ts[i].sFun, m_sDefaultFunction) && !fCIStrEqual(ts[i].sName, m_sDefaultFunction) && !fCIStrEqual(ts[i].sName, m_sDefaultFunction + ".fun"))
				++i;
			if (i == iMLSFUNCS)
				ose->SetCurSel(0);
			else
				ose->SetCurSel(i);
		}
		else
			ose->SetCurSel(0);
	}
private:
	struct MplStatsStruct {
		String sFun;
		String sName;
	};
	MplStatsStruct ts[iMLSFUNCS];
	String m_sDefaultFunction;
};

//---------- FormMapMaplistStatistics --------
LRESULT Cmdmapliststatistics(CWnd *wnd, const String& s)
{
	new FormMapMaplistStatistics(wnd, s.c_str());
	return -1;
}

FormMapMaplistStatistics::FormMapMaplistStatistics(CWnd* mw, const char* sPar)
    : FormMapCreate(mw, "Maplist Statistics"),
    m_FunctionListInitialized(false), m_iStart(iUNDEF), m_iEnd(iUNDEF), m_fNewMaplist(true), m_psFunction(0)
{
		String sDefaultFunction ("");
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
            if (m_sMaplist == "" && (fn.sExt == "" || fn.sExt == ".mpl")) {
								m_sMaplist = fn.sFullNameQuoted(true);
            }
            else if (sDefaultFunction.length() == 0) {
                sDefaultFunction = sVal;
            }  
            else if (m_iStart == iUNDEF) {
                int iVal = sVal.iVal();
                if (iVal != iUNDEF)
                    m_iStart = iVal;
            }  
            else if (m_iEnd == iUNDEF) {
                int iVal = sVal.iVal();
                if (iVal != iUNDEF)
                    m_iEnd = iVal;
            }  
        }
    }
    m_fieldMaplist = new FieldMapList(root, "MapList", &m_sMaplist, false);
    m_fieldMaplist->SetCallBack((NotifyProc)&FormMapMaplistStatistics::MaplistCallBack);
    StaticText *st = new StaticText(root, "Statistic function");
    st->Align(m_fieldMaplist, AL_UNDER);
    FieldMplStatsFuncSimple * fmss = new FieldMplStatsFuncSimple(root, &m_psFunction, sDefaultFunction);
    fmss->Align(st, AL_AFTER);
    FieldBlank *blank = new FieldBlank(root, 0.5);
    blank->Align(st, AL_UNDER);

    // TR("Number of &Clusters")
    StaticText *st2 = st = new StaticText(root, "Start band");
    st2->Align(blank, AL_UNDER);
    m_fieldMinBand = new FieldInt(root, "", &m_iStart, ValueRange(2,60), true, true);
    m_fieldMinBand->Align(st2, AL_AFTER);
    m_fieldMinBand->SetIndependentPos();
    m_fieldMaxBand = new FieldInt(root, "End band", &m_iEnd, ValueRange(2,60), true, true);
    m_fieldMaxBand->Align(m_fieldMinBand, AL_AFTER);
    m_fieldMaxBand->SetIndependentPos();

    blank = new FieldBlank(root, 0.2);
    blank->Align(st2, AL_UNDER);
    initMapOut(false, false);

//    SetHelpItem("ilwisapp\\cluster_dialog_box.htm");
    create();
}

int FormMapMaplistStatistics::exec()
{
    FormMapCreate::exec();

		if (m_psFunction && (*m_psFunction != 0) && (m_psFunction->length() > 0))
		{
			String sExpr;
			if (m_iStart == iUNDEF || m_iEnd == iUNDEF || (m_iStart == m_iMinBand && m_iEnd == m_iMaxBand))
					sExpr = String("MapMaplistStatistics(%S,%S)", m_sMaplist, *m_psFunction);
			else
					sExpr = String("MapMaplistStatistics(%S,%S, %i, %i)", m_sMaplist, *m_psFunction, m_iStart + m_iOffset, m_iEnd + m_iOffset);

			execMapOut(sExpr);
		}

    return 0;
}

int FormMapMaplistStatistics::MaplistCallBack(Event*)
{
		String sMapListPrev = m_sMaplist; // opening a big maplist takes time .. try to prevent it; initially callback is called 32 x

    m_fieldMaplist->StoreData();
		
		if (sMapListPrev != m_sMaplist)
			m_fNewMaplist = true; // true is set here or by constructor

		if (m_fNewMaplist) // there is some change
		{
			m_fNewMaplist = false;
			try
			{
					if (m_sMaplist.length() > 0)
					{
							MapList mpl(m_sMaplist);
							m_iMinBand = mpl->iLower();
							m_iMaxBand = mpl->iUpper();
							m_iOffset = m_iMinBand - 1; // check implementation of mpl->iLower() .. some improvement may be needed there

							m_iMinBand -= m_iOffset;
							m_iMaxBand -= m_iOffset;

							m_fieldMinBand->SetvalueRange(ValueRange(m_iMinBand, m_iMaxBand));
							m_fieldMaxBand->SetvalueRange(ValueRange(m_iMinBand, m_iMaxBand));
							m_fieldMinBand->SetVal(m_iMinBand);
							m_fieldMaxBand->SetVal(m_iMaxBand);
					}
			}
			catch (...)
			{
					// do nothing
			}
		}

    return 0;
}
