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
/* Digitizer MapReference
   by Wim Koolhoven
   (c) ILWIS Department ITC
	Last change:  WK    3 Mar 97    2:32 pm
*/
#include "Client\Headers\formelementspch.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\Editors\Digitizer\DIGITIZR.H"
#include "Client\Editors\Digitizer\DIGREF.H"
#include "Headers\Hs\Digitizr.hs"
#include "Client\FormElements\fentdms.h"

FormUsingDigitizer::FormUsingDigitizer(CWnd* par, Digitizer* dg, const String& sTitle, bool fModal)
: FormWithDest(par,sTitle,fbsSHOWALWAYS|fbsBUTTONSUNDER|(fModal?fbsMODAL:0))
	, dig(dg), info(0), fOk(false)
{
  if (dig)
    dig->Activate();
}

FormUsingDigitizer::~FormUsingDigitizer()
{
}

void FormUsingDigitizer::ProcessInfo(double rX, double rY, int iBut)
{
  if (rCurrX != rX || rCurrY != rY) {
    rCurrX = rX;
    rCurrY = rY;
    if (info) {
			Coord crd(rCurrX, rCurrY);
			info->SetCoord(crd);
//      String sNew("%7.1f %7.1f", rCurrX, rCurrY);
//      info->title()->SetWindowText(sNew.c_str());
    }  
  }	
  if (iBut != iButton) {
    if (info) {
      if (iButton >= 0 && iButton <= 3)
        info->button(1+iButton)->SetCheck(FALSE);
      if (iBut >= 0 && iBut <= 3)
				info->button(1+iBut)->SetCheck(TRUE);
    }	
    iButton = iBut;
    HandleInfo();
  }
}

void FormUsingDigitizer::OnOK()
{
	if (info)
	{
		info->DestroyWindow(); // "delete this" is performed inside DestroyWindow
		info = 0;
	}
	FormWithDest::OnOK();
}

void FormUsingDigitizer::OnCancel()
{
	if (info)
	{
		info->DestroyWindow(); // "delete this" is performed inside DestroyWindow
		info = 0;
	}
	FormWithDest::OnCancel();
}

FieldRefPoint::FieldRefPoint(FormEntry* par, int iNr, bool fMetric, const CoordSystem& cs)
: FieldGroup(par)
{
  sDigX = "123456.7";
  sDigY = "123456.7";
  stDigX = new StaticTextSimple(this,sDigX);
  stDigY = new StaticTextSimple(this,sDigY);
  stDigY->Align(stDigX, AL_AFTER);
  Load(iNr);
  if (fMetric) 
    fCrd = new FieldCoord(this, "", &coord);
  else 
    fCrd = new FieldCoordAskLatLon(this, cs, &coord);
  fCrd->SetCallBack((NotifyProc)&MapReferenceForm::calc);
  fCrd->Align(stDigY, AL_AFTER);
  sDX = "1234.5";
  sDY = "1234.5";
  stDX = new StaticTextSimple(this,sDX);
  stDX->Align(fCrd, AL_AFTER);
  stDY = new StaticTextSimple(this,sDY);
  stDY->Align(stDX, AL_AFTER);
  fActDig = false;
}

void FieldRefPoint::Load(int iNr)
{
	String sKey("Digitizer\\DigRef\\RefPoint%i", iNr);
  IlwisSettings settings(sKey);

	fDig = false;
  if (settings.fKeyExists())
  {
		coord = settings.crdValue("MapCoordinate");
		Coord crdDig = settings.crdValue("DigitizerCoordinate");
		rDigX = crdDig.x;
		rDigY = crdDig.y;
		fDig = true;
	}
}

FormEntry* FieldRefPoint::CheckData()
{
	if (fDig)
		return FieldGroup::CheckData();
	else
		return 0;
}

void FieldRefPoint::Store(int iNr)
{
	String sKey("Digitizer\\DigRef\\RefPoint%i", iNr);
	try
	{
	  IlwisSettings settings(sKey, IlwisSettings::pkuUSER, true, IlwisSettings::omREADWRITE);

		if (!fDig) {
			settings.DeleteKey();
			return;
		}

		settings.SetValue("MapCoordinate", coord);
		settings.SetValue("DigitizerCoordinate", fDig ? Coord(rDigX, rDigY) : Coord(0,0));
	}
	catch (RegistryError&)
	{
	}
//  String fn = "ilwis.ini";
//  String sSection("RefPoint%i", iNr);
//  String sValX("%f", coord.x);
//  WritePrivateProfileString(sSection.c_str(), "X", sValX.c_str(), fn.c_str());
//  String sValY("%f", coord.y);
//  WritePrivateProfileString(sSection.c_str(), "Y", sValY.c_str(), fn.c_str());
//  if (!fDig) {
//    WritePrivateProfileString(sSection.c_str(), "DigX", 0, fn.c_str());
//    WritePrivateProfileString(sSection.c_str(), "DigY", 0, fn.c_str());
//  }
//  else {
//    String sDigValX("%f", rDigX);
//    WritePrivateProfileString(sSection.c_str(), "DigX", sDigValX.c_str(), fn.c_str());
//    String sDigValY("%f", rDigY);
//    WritePrivateProfileString(sSection.c_str(), "DigY", sDigValY.c_str(), fn.c_str());
//  }  
}

void FieldRefPoint::ReadCoord()
{
  fCrd->StoreData();
}

void FieldRefPoint::Activate()
{
  fCrd->SetFocus();
}

void FieldRefPoint::ShowDig()
{
  String s;
  if (fDig) {
    s = String("%c%5.1f", fActDig ? '*' : ' ', rDigX);
    stDigX->SetVal(s);
    s = String(" %5.1f", rDigY);
    stDigY->SetVal(s);
  }
  else {
    if (fActDig)
      s = "*";
    else
      s = " ";
    stDigX->SetVal(s);
    s = " ";
    stDigY->SetVal(s);
  }
  if (fDig || fActDig) 
    ShowChildren();
  else
    HideChildren();
}

void FieldRefPoint::ShowVals()
{
  String s;
  if (fOk) {
    if (abs(rDX) > 999)
      s = " ***";
    else
      s = String("%5.1f", rDX);
    stDX->SetVal(s);
    if (abs(rDY) > 999)
      s = " ***";
    else
      s = String("%5.1f", rDY);
    stDY->SetVal(s);
  }
  else {
    s = " ";
    stDX->SetVal(s);
    stDY->SetVal(s);
  }
  stDX->Show();
  stDY->Show();
}

bool FieldRefPoint::fDigHandle(HANDLE hnd)
{
  return stDigX->hWnd() == hnd ||
         stDigY->hWnd() == hnd;
}


BEGIN_MESSAGE_MAP(MapReferenceForm, FormBaseDialog)
  ON_WM_CTLCOLOR()
END_MESSAGE_MAP()    


MapReferenceForm::MapReferenceForm(CWnd* par, Digitizer* dg, bool fMetric)
: FormUsingDigitizer(par,dg,TR("Map Referencing - Digitizer Control Points"))
{
  clrNormal = GetSysColor(COLOR_WINDOWTEXT);
  clrActive = GetSysColor(COLOR_HIGHLIGHTTEXT);
  clrBackNormal = GetSysColor(COLOR_WINDOW);
  brNormal = new CBrush(clrBackNormal);
  clrBackActive = GetSysColor(COLOR_HIGHLIGHT);
  brActive = new CBrush(clrBackActive);
	dtf.SetEpsilon(!fMetric); // for Latlons a different point precision is needed
  new FieldBlank(root,(float)1.1);
  for (int i = 0; i < 6; ++i) 
    frp[i] = new FieldRefPoint(root,i,fMetric,dg->cwcs);
  for (int i = 1; i < 6; ++i) 
    frp[i]->Align(frp[i-1], AL_UNDER);
    
  FieldGroup* fg = new FieldGroup(root);
  StaticText *st;
  int iH;
  st = new StaticText(fg, TR("digitizer"));
  iH = frp[0]->stDigX->psn->iHeight + frp[0]->stDigX->psn->iBndDown + 33;
  st->Align(frp[0]->stDigX, AL_UNDER, -iH);
  st = new StaticText(fg, TR("coordinates"));
  iH = frp[0]->fCrd->psn->iHeight + frp[0]->fCrd->psn->iBndDown + 33;
  st->Align(frp[0]->fCrd, AL_UNDER, -iH);
  st = new StaticText(fg, TR("diff X"));
  iH = frp[0]->stDX->psn->iHeight + frp[0]->stDX->psn->iBndDown + 33;
  st->Align(frp[0]->stDX, AL_UNDER, -iH);
  st = new StaticText(fg, TR("diff Y"));
  iH = frp[0]->stDY->psn->iHeight + frp[0]->stDY->psn->iBndDown + 33;
  st->Align(frp[0]->stDY, AL_UNDER, -iH);
    
  iActDigPnt = 0;
  frpCurr()->fActDig = true;
//  new FieldBlank(root);
  iAffine = dg->fAffine;
  FieldGroup* fginfo = new FieldGroup(root);
  fginfo->SetIndependentPos();
  fginfo->Align(frp[5], AL_UNDER);
  rgAffine = new RadioGroup(fginfo, "", &iAffine);
  new RadioButton(rgAffine, TR("&Conformal transformation"));
  new RadioButton(rgAffine, TR("&Affine transformation"));
  rgAffine->SetCallBack((NotifyProc)&MapReferenceForm::calc);

  stSigma = new StaticText(fginfo, TR("Minimum three points required"));
  stSigma->Align(rgAffine, AL_AFTER);
  stScale = new StaticText(fginfo, TR("Scale 1:XXXXXXXXXXXX / XXXXXXXXXXXX"));
  stRot   = new StaticText(fginfo, TR("Rotation XXXXX.x / XXXXX.x degrees"));
  fginfo->SetCallBack((NotifyProc)&MapReferenceForm::initMRF);
  SetMenHelpTopic("ilwismen\\digitizer_map_referencing_dig_ctrl_points.htm");
}

MapReferenceForm::~MapReferenceForm()
{
  if (fOkClicked() && fOk)
    for (int i = 0; i < 6; ++i)
      frp[i]->Store(i);
  delete brNormal;
  delete brActive;
}

void MapReferenceForm::HandleInfo()
{
  FieldRefPoint* frp;
  switch (iButton) {
    case -1:
      break;
    case 0:
      frp = frpCurr();
      frp->rDigX = rCurrX;
      frp->rDigY = rCurrY;
      frp->fDig = true;
      frp->Activate();
      frp->ShowDig();
      calc(0);
      // fall through
    case 1:
      frpNext();
      break;
    case 2:
      frp = frpCurr();
      frp->fDig = false;
      frp->ShowDig();
      frpNext();
      // fall through
    case 3:
      calc(0);
      break;
  }
}

FieldRefPoint* MapReferenceForm::frpNext()
{ 
  frpCurr()->fActDig = false; 
  frpCurr()->ShowDig(); 
  ++iActDigPnt %= 6;
  frpCurr()->fActDig = true;	
  frpCurr()->ShowDig(); 
  return frpCurr(); 
}
  
int MapReferenceForm::initMRF(Event*)
{
  if (info)
    return 0;
  for (int i = 0; i < 6; ++i) {
//    frp[i]->Load(i);
    frp[i]->ShowDig();
  }
	DigitizerInfoWindow* diw = new DigitizerInfoWindow();
	diw->Create(this);
	info = diw;
  info->SetWindowText(TR("Map Referencing").c_str());
  info->button(1)->SetWindowText(TR("Digitize Point").c_str());
  info->button(2)->SetWindowText(TR("Next Point").c_str());
  info->button(3)->SetWindowText(TR("Delete Point").c_str());
  info->ShowWindow(SW_SHOW);
	info->EnableDocking(0);
	return 0;
}

int MapReferenceForm::calc(Event*)
{
  int i, iPoints = 0;
  iAffine = rgAffine->iVal();
  bool fAffine = iAffine > 0;
//  dtf.dp.Reset();
  for (i = 0; i < 6; ++i)
    frp[i]->ReadCoord();
  for (i = 0; i < 6; ++i)
    if (frp[i]->fDig && !frp[i]->coord.fUndef()) {
//      dtf.dp &= DigTransform::DigPoint();
      dtf.dp[iPoints].crdDig = Coord(frp[i]->rDigX, frp[i]->rDigY);
      dtf.dp[iPoints].crdWorld = frp[i]->coord;
      ++iPoints;
    }
  dtf.iPoints = iPoints;
  
  int iErr = dtf.Transform(fAffine);
  switch (iErr) {
    case -1:
      stSigma->SetVal(TR("Not enough points"));
      break;
    case -2:
      stSigma->SetVal(TR("Singular Matrix"));
      break;
    default:
      stSigma->SetVal(" ");
      break;
  }
  stScale->SetVal(" ");
  stRot->SetVal(" ");
  fOk = dtf.fOk && (0 == CheckData()); // also check if data is correct; otherwise form won't close in OnOK
  if (!fOk)
	{
    for (int i = 0; i < 6; ++i)
		{
      frp[i]->fOk = false;
      frp[i]->ShowVals();
    }
    DisableOK();
  }
  else
	{
    x0	= dtf.x0;
    x10 = dtf.x10;
    x01 = dtf.x01;
    y0	= dtf.y0;
    y10 = dtf.y10;
    y01 = dtf.y01;
    int iPoint = 0;
    for (int i = 0; i < 6; ++i) {
      frp[i]->fOk = frp[i]->fDig && !frp[i]->coord.fUndef();
      if (frp[i]->fOk)
			{
				frp[i]->rDX = dtf.dp[iPoint].crdDelta.x;
				frp[i]->rDY = dtf.dp[iPoint++].crdDelta.y;
      }
      frp[i]->ShowVals();
    }
    String s(TR("Sigma = %2.1f mm").c_str(), dtf.rSigma);
    stSigma->SetVal(s);
    double rScX = 1000 * sqrt(x10 * x10 + y10 * y10);   // 1000 mm per m
    double rScY = 1000 * sqrt(x01 * x01 + y01 * y01);
    double rRotX = -atan2(y10,x10) * 180 / M_PI;
    double rRotY =  atan2(x01,y01) * 180 / M_PI;
    String sScale, sRot;
    if (fAffine) {
      sScale = String(TR("Scale 1:%1.0f / 1:%1.0f").c_str(), rScX, rScY);
      sRot = String(TR("Rotation %1.1f / %1.1f degrees").c_str(), rRotX, rRotY);
    }
    else {
      sScale = String(TR("Scale 1:%1.0f").c_str(), rScX);
      sRot = String(TR("Rotation %1.1f degrees").c_str(), rRotX);
    }
    stScale->SetVal(sScale);
    stRot->SetVal(sRot);
    if (dtf.rSigma > 20)
      DisableOK();
    else  
      EnableOK();
  }
  return 0;
}

//long dispatch(Event* Evt)
//{
//  switch (Evt->iMessage) {
//    case WM_CTLCOLOR: 
//      if (Evt->wParm == CTLCOLOR_STATIC) {
//        for (int i = 0; i < 6; ++i)
//          if (frp[i]->fDigHandle((HANDLE)Evt->loParam())) {
//            if (frp[i]->fActDig) {
//              SetTextColor((HDC)Evt->wParam, clrActive);
//              SetBkColor((HDC)Evt->wParam, clrBackActive);
//              return (long)(HANDLE) *brActive;
//            }
//            else {
//              SetTextColor((HDC)Evt->wParam, clrNormal);
//              SetBkColor((HDC)Evt->wParam, clrBackNormal);
//              return (long)(HANDLE) *brNormal;
//            }
//          }
//      }
//      break;
//  }
//  return FormUsingDigitizer::dispatch(Evt);
//}

HBRUSH MapReferenceForm::OnCtlColor(CDC* cdc, CWnd* pWnd, UINT nCtlColor)
{
	if (nCtlColor == CTLCOLOR_STATIC) {
    for (int i = 0; i < 6; ++i) {
      if (frp[i]->fDigHandle(pWnd->m_hWnd)) {
        if (frp[i]->fActDig) {
          cdc->SetTextColor(clrActive);
          cdc->SetBkColor(clrBackActive);
          return *brActive;
        }
        else {
          cdc->SetTextColor(clrNormal);
          cdc->SetBkColor(clrBackNormal);
          return *brNormal;
        }
			}
		}
	}
	return FormBaseDialog::OnCtlColor(cdc, pWnd, nCtlColor);
}

