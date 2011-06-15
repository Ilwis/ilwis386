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
/* Digitizer Configuration
// by Wim Koolhoven
// (c) ILWIS System Development ITC
	Last change:  WK   18 Feb 97    5:22 pm
*/
#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\FldOneSelectTextOnly.h"
#include "Client\ilwis.h"
#include "Client\Editors\Digitizer\DIGITIZR.H"
#include "Client\Editors\Digitizer\DIGREF.H"
#include "Client\Editors\Digitizer\DigitizerWizard.h"
#include "Client\Editors\Digitizer\DigConf.h"
#include "Headers\constant.h"
#include "Headers\Hs\Digitizr.hs"
#include "Headers\messages.h"
#include "Client\Editors\Digitizer\WINTAB.H"


DigSetPortForm::DigSetPortForm(DigitizerWizard* dw, Digitizer* dig)
  : DigitizerWizardPage(dw, SDGTitleSelectPort, dig)
{
	if (dig->iPort == 0)
		iPrt = 0;
	else
		iPrt = dig->iPort + 1;  
	if (dig->fWinTab)
		iPrt = 1;

	rg = new RadioGroup(root, SDGUiDigPort, &iPrt);
	rg->SetCallBack((NotifyProc)&DigSetPortForm::CallBack);
	rg->SetIndependentPos();
	RadioButton* rb0 = new RadioButton(rg, SDGUiNone);
	rbWinTab = new RadioButton(rg, SDGUiWinTab);
	rbWinTab->Align(rb0, AL_AFTER);
	RadioButton* rb1 = new RadioButton(rg, SDGUiCom1);
	rb1->Align(rb0, AL_UNDER);
	RadioButton* rb2 = new RadioButton(rg, SDGUiCom2);
	rb2->Align(rb1, AL_AFTER);
	RadioButton* rb3 = new RadioButton(rg, SDGUiCom3);
	rb3->Align(rb1, AL_UNDER);               
	RadioButton* rb4 = new RadioButton(rg, SDGUiCom4);
	rb4->Align(rb3, AL_AFTER);
	SetMenHelpTopic("ilwismen\\digitizer_setup_digitizer_select_port.htm");
	create();
}

int DigSetPortForm::CallBack(Event*)
{
	rg->StoreData();
	if (0 == WTInfo(0,0,NULL))
		rbWinTab->Disable();  // disable before it can be selected

	switch (iPrt) {
		case 1: // wintab
			if (0 == WTInfo(0,0,NULL)) {
				rg->SetVal(0);
				rbWinTab->Disable();
				MessageBox(SDGErrNoWinTab.scVal(), SDGErrDigError.scVal(), MB_ICONSTOP);
			}
			// fall trhough
		case 0: // none
			wiz->SetWizardButtons(PSWIZB_FINISH);
			break;
		default:
			wiz->SetWizardButtons(PSWIZB_NEXT);
	}
	return 0;
}

int DigSetPortForm::exec()
{
	DigitizerWizardPage::exec();
	_dig->iPort = 0;
	_dig->fWinTab = false;
	switch (iPrt) {
		case 0:
			break;
		case 1:
			_dig->fWinTab = true;
			break;
		default:
			_dig->iPort = iPrt - 1;
	}
	return 0;
}

DigPortSettings::DigPortSettings(DigitizerWizard* dw, Digitizer *dig) :
  DigitizerWizardPage(dw, SDGTitleCOMSettings, dig),
	fAdded(false)
{
	String sRem(SDGRemSpecifySettings.scVal());
	StaticText* st = new StaticText(root, sRem);
	st->SetIndependentPos();

  sBaud = String("%d", dig->iBaudRate);
	sData = String("%d", dig->iByteSize);
	sStopBits = dig->iStopBits == ONESTOPBIT   ? "1" :
	            dig->iStopBits == ONE5STOPBITS ? "1.5" :
                                             "2";
  sParity = dig->iParity == EVENPARITY ?   "Even" :
		        dig->iParity == ODDPARITY ?    "Odd" :
    			  dig->iParity == SPACEPARITY ?  "Space" : 
 					  dig->iParity == MARKPARITY ?   "Mark":
						                              "None";

  StaticText *st1 = new StaticText(root, SDGUiBaudRate);
	foBaud = new FieldOneSelectTextOnly(root, &sBaud, false);
	foBaud->Align(st1, AL_AFTER);

  st1 = new StaticText(root, SDGUiDataBits);
	foData = new FieldOneSelectTextOnly(root, &sData, false);
	foData->Align(st1, AL_AFTER);

	st1 = new StaticText(root, SDGUiParity);
	foParity = new FieldOneSelectTextOnly(root, &sParity, false);
	foParity->Align(st1, AL_AFTER);

  st1 = new StaticText(root, SDGUiStopBits);
	foStop = new FieldOneSelectTextOnly(root, &sStopBits, false);
	foStop->Align(st1, AL_AFTER);

	foBaud->SetCallBack((NotifyProc)&DigPortSettings::FillAll);
	SetMenHelpTopic("ilwismen\\digitizer_setup_digitizer_port_settings.htm");
	create();
}

int DigPortSettings::exec()
{
	DigitizerWizardPage::exec();

	_dig->iParity = sParity == "Even"  ? EVENPARITY :
				 				sParity == "Mark"  ? MARKPARITY :
								sParity == "Odd"   ? ODDPARITY :
								sParity == "Space" ? SPACEPARITY :
																		 NOPARITY;
	_dig->iByteSize = sData.iVal();
	_dig->iStopBits = sStopBits == "1"   ? ONESTOPBIT : 
									sStopBits == "1.5" ? ONE5STOPBITS :
																			 TWOSTOPBITS;
	_dig->iBaudRate = sBaud.iVal();	
	
	_dig->Enable(false, false);
	_dig->act = Digitizer::actSETUP;
	return 1;																		  
}

// mainly copied from FormBaseWizardPage::OnWizardNext()
LRESULT DigPortSettings::OnWizardNext()
{
	try {
		FormEntry *pfe = CheckData();
		if (pfe) 
		{
			MessageBeep(MB_ICONEXCLAMATION);
			pfe->SetFocus();
			return -1;
		}
		_fOkClicked = true;
		exec();
	}	
	catch (ErrorObject& err) {
		err.Show();
		return OnWizardBack();
	}
	CPropertySheet* ps = (CPropertySheet*)GetParent();
	int iP = ps->GetPageIndex(this);
  for (int i=iP+1; i < ps->GetPageCount(); i++) {
		FormBaseWizardPage* pp = dynamic_cast<FormBaseWizardPage*>(ps->GetPage(i));
		if (pp->fPPEnabled()) {
			ps->SetActivePage(pp);
			break;
		}
	}	
  return -1;
}


int DigPortSettings::FillAll(Event *)
{
	if ( fAdded ) return 1;
	foBaud->AddString("75");
	foBaud->AddString("110");
	foBaud->AddString("134");
	foBaud->AddString("150");
	foBaud->AddString("300");
	foBaud->AddString("600");
	foBaud->AddString("1200");
	foBaud->AddString("2400");
	foBaud->AddString("4800");
	foBaud->AddString("9600");
  foBaud->AddString("14400");
	foBaud->AddString("19200");
	foBaud->AddString("38400");
	foBaud->AddString("57600");
	foBaud->AddString("115200");
	foData->AddString("8");
	foData->AddString("7");
//	foData->AddString("6");
//  foData->AddString("5");
//  foData->AddString("4");
  foStop->AddString("2");
//  foStop->AddString("1.5");
  foStop->AddString("1");
	foParity->AddString("Even");
	foParity->AddString("Odd");
	foParity->AddString("None");
//	foParity->AddString("Mark");
//	foParity->AddString("Space");
	foBaud->SelectItem(sBaud);
	foData->SelectItem(sData);
  foStop->SelectItem(sStopBits);
	foParity->SelectItem(sParity);

	fAdded = true;

	return 1;
}

DigAutoForm::DigAutoForm(DigitizerWizard* dw, Digitizer* dig)  
  : DigitizerWizardPage(dw, SDGTitleDigConfigure, dig)
{
	fManual = false;
	status = stINIT;
	iCnt = 0;
	iLength = 0;
	String sFill('X', 30);
	StaticText* st = new StaticText(root, SDGRemReceived);
	st->Font(IlwWinApp()->GetFont(IlwisWinApp::sfFORM), false);

	stReceive = new StaticText(root, sFill);
	stReceive->Align(st, AL_AFTER);
	stReceive->SetIndependentPos();
	stReceive->Font(IlwWinApp()->GetFont(IlwisWinApp::sfFORM), false);
	
	FieldBlank* fb = new FieldBlank(root);
	fb->Align(st, AL_UNDER);
	sFill = String('X', 40);
	stMessage = new StaticText(root, sFill);
	stMessage->SetIndependentPos();
	new FieldBlank(root);
	PushButton* pb = new PushButton(root, SDGUiManual,
		(NotifyProc)&DigAutoForm::Manual);
	pb->SetIndependentPos();
	SetMenHelpTopic("ilwismen\\digitizer_setup_digitizer_autodetect.htm");
	create();
}

void DigAutoForm::Interpret(const char* s)
{
	int iStrLen = strlen(s);
	if ( iStrLen == 0 ) return;
  if (iStrLen > 5) 
    stReceive->SetVal(s);
  switch (status) {
    case stINIT: {
      int iLen = iStrLen;
      if (iLen > iLength)
        iLength = iLen;
      if (++iCnt >= 10) 
			{
        iCnt = 0;
        status = stSTART;
      }
    } break;
    case stSTART: 
		{
      if (iStrLen < iLength) 
			{
        if (++iCnt >= 4) 
				{
          stMessage->SetVal(SDGRemNrCharsNotConstant);
          iCnt = 0;
          status = stINIT;
        }
      }  
      _dig->iNrChars = iLength;
      int i = 0, iStart, iLen = 0;
      iFlagPos = 1;
      _dig->iFlagPos = 1;
      _dig->iXPos = 0;
      _dig->iXLen = 0;
      _dig->iYPos = 0;
      _dig->iYLen = 0;
      while (i < iLength && iLen <= 1 ) {
        while (i < iLength && !(isdigit(s[i]) || '.' == s[i])) ++i; 
        iStart = i;  
        while (i < iLength && (isdigit(s[i]) || '.' == s[i])) ++i;
        iLen = i - iStart;
        if (1 == iLen)
          iFlagPos = iStart;
        else {
          _dig->iXPos = 1 + iStart;
          _dig->iXLen = iLen;
        }  
      }
      while (i < iLength) {
        while (i < iLength && !(isdigit(s[i]) || '.' == s[i])) ++i;
        iStart = i;  
        while (i < iLength && (isdigit(s[i]) || '.' == s[i])) ++i;
        iLen = i - iStart;
        if (1 == iLen)
          iFlagPos = iStart;
        else {
          _dig->iYPos = 1 + iStart;
          _dig->iYLen = iLen;
        }  
      }  
      if (0 == _dig->iYLen) { 
        if (_dig->iXLen % 2)
          _dig->iXPos += 1;
        _dig->iXLen /= 2;
        _dig->iYPos = _dig->iXPos + _dig->iXLen;
        _dig->iYLen = _dig->iYLen;  
      }    
      _dig->iFlagPos = 1 + iFlagPos;
      cNone = s[iFlagPos];
      status = stBUTTON;
      iCnt = 0;
      String sBut(SDGRemPressButton_i.scVal(), 0);
      String s("%S.  %S", SDGRemTryingAutoDetect, sBut);
      stMessage->SetVal(s);
      for (i = 0; i < 5; ++i)
        _dig->sCodes[i] = 0;
    } break;
    case stBUTTON: {
      if (strlen(s) < iLength) 
        break;
      if (s[iFlagPos] == cNone)
        break;
      bool fOldCode = false;
      for (int i = 0; i < iCnt; ++i)
        if (s[iFlagPos] == sCodes[i])
          fOldCode = true;
      if (fOldCode)    
        break;
      _dig->sCodes[iCnt] = sCodes[iCnt] = s[iFlagPos];
      iCnt += 1;
      if (iCnt < 4) {   
        String sBut(SDGRemPressButton_i.scVal(), iCnt);
        String s("%S.  %S", SDGRemTryingAutoDetect, sBut);
        stMessage->SetVal(s);
      }
      else {
				fManual = false;
				exec();
				wiz->SetActivePage(wiz->pgSize);
      }  
    } break;
  }  
}

BOOL DigAutoForm::OnSetActive()
{
  fManual = false;
  status = stINIT;
  iCnt = 0;
  iLength = 0;
	FormBaseWizardPage::OnSetActive();
	wiz->SetWizardButtons(PSWIZB_BACK); // no next!
  stReceive->SetVal(SDGRemNothing);
  stMessage->SetVal(SDGRemTryingAutoDetect);
	return TRUE;
}

int DigAutoForm::Manual(Event*) 
{
  fManual = true;
	wiz->SetActivePage(wiz->pgManual);
  return 0;
}

DigConfigForm::DigConfigForm(DigitizerWizard* dw, Digitizer* dig)  
  : DigitizerWizardPage(dw, SDGTitleDigManualSetup, dig)
{
	SetPPDisable();
  String sFill('X', 30);
  StaticText* st = new StaticText(root, SDGRemReceived);
	st->Font(IlwWinApp()->GetFont(IlwisWinApp::sfFORM), false);
  st->SetCallBack((NotifyProc)&DigConfigForm::CallBack);
  stReceive = new StaticText(root, sFill);
	stReceive->Align(st, AL_AFTER);
  stReceive->SetIndependentPos();
  stReceive->Font(IlwWinApp()->GetFont(IlwisWinApp::sfFORM), false);
  sCodes = dig->sCodes;
  FieldBlank* fb = new FieldBlank(root);
	fb->Align(st, AL_UNDER);
  FieldInt* fiNrC = new FieldInt(root, SDGUiNrChars, &dig->iNrChars);
  FieldInt* fiXPos = new FieldInt(root, SDGUiXPos, &dig->iXPos);
  FieldInt* fiXLen = new FieldInt(root, SDGUiXLength, &dig->iXLen);
  fiXLen->Align(fiXPos, AL_AFTER);
  FieldInt* fiYPos = new FieldInt(root, SDGUiYPos, &dig->iYPos);
  fiYPos->Align(fiXPos, AL_UNDER);
  FieldInt* fiYLen = new FieldInt(root, SDGUiYLength, &dig->iYLen);
  fiYLen->Align(fiYPos, AL_AFTER);
  FieldInt* fiFlagPos = new FieldInt(root, SDGUiFlagPos, &dig->iFlagPos);
  fiFlagPos->Align(fiYPos, AL_UNDER);
  FieldString* fiFlagCodes = new FieldString(root, SDGUiFlagCodes, &sCodes);
  fiFlagCodes->Align(fiFlagPos, AL_AFTER);
  SetMenHelpTopic("ilwismen\\digitizer_setup_digitizer_manual.htm");
	create();
}

void DigConfigForm::Interpret(const char* s)
{
  if (strlen(s) > 5)
    stReceive->SetVal(s);
}
int DigConfigForm::CallBack(Event*) 
{
	stReceive->SetVal(SDGRemNothing);
  return 0;
}

int DigConfigForm::exec() 
{
	SetPPEnable();
  DigitizerWizardPage::exec();
  for (int i = 0; i < 4; ++i)
    _dig->sCodes[i] = sCodes[i];
  return 0;  
}



DigConfigSizeForm::DigConfigSizeForm(DigitizerWizard* dw, Digitizer* dig)  
: DigitizerWizardPage(dw, SDGTitleDigSetupSize, dig)
{
  dig->rCurrX = rUNDEF;
  dig->rCurrY = rUNDEF;
  dig->iButton = -1;
  String sFill('X', 30);

	FieldGroup* fg = new FieldGroup(root);
	fg->SetBevelStyle(FormEntry::bsLOWERED);
  stButton = new StaticText(fg, sFill);
  stButton->SetIndependentPos();
  stX = new StaticText(fg, sFill);
  stX->SetIndependentPos();
  stY = new StaticText(fg, sFill);
  stY->SetIndependentPos();
  stXmm = new StaticText(fg, sFill);
	stXmm->Align(stX, AL_AFTER);
  stXmm->SetIndependentPos();
  stYmm = new StaticText(fg, sFill);
  stYmm->SetIndependentPos();

  FieldBlank* fb = new FieldBlank(root);
	fb->Align(stY, AL_UNDER);
  fMetric = true;
  cbMetric = new CheckBox(root, SDGUiMetricSystem, &fMetric);    
  cbMetric->SetCallBack((NotifyProc)&DigConfigSizeForm::MetricCallBack);
  cbMetric->SetIndependentPos();
  rXSizeM = dig->iMaxX / 10.0;
  rYSizeM = dig->iMaxY / 10.0;
  rUnitSizeM = 1.0 / dig->rUnitSize;
  rXSizeInch = dig->iMaxX / 254.0;
  rYSizeInch = dig->iMaxY / 254.0;
  rUnitSizeInch = 25.4 / dig->rUnitSize;
  frUM = new FieldReal(root, SDGUiLinesPerMm, &rUnitSizeM);
  frUM->Align(cbMetric, AL_UNDER);
  frUM->SetCallBack((NotifyProc)&DigConfigSizeForm::CallBack);
  frUI = new FieldReal(root, SDGUiLinesPerInch, &rUnitSizeInch);
  frUI->Align(cbMetric, AL_UNDER);
  frUI->SetCallBack((NotifyProc)&DigConfigSizeForm::CallBack);

//  new FieldBlank(root);
  fb = new FieldBlank(root);
	fb->Align(stYmm, AL_UNDER);
  frXM = new FieldReal(root, SDGUiXSizeM, &rXSizeM);
  frXM->Align(fb, AL_UNDER);
  frYM = new FieldReal(root, SDGUiYSizeM, &rYSizeM);
  frXI = new FieldReal(root, SDGUiXSizeInch, &rXSizeInch);
  frXI->Align(fb, AL_UNDER);
  frYI = new FieldReal(root, SDGUiYSizeInch, &rYSizeInch);
  SetMenHelpTopic("ilwismen\\digitizer_setup_digitizer_size.htm");
	create();
}

BOOL DigConfigSizeForm::OnSetActive()
{
	FormBaseWizardPage::OnSetActive();
	wiz->SetWizardButtons(PSWIZB_BACK | PSWIZB_FINISH);
	return TRUE;
}

int DigConfigSizeForm::exec() 
{
	DigitizerWizardPage::exec();
  if (fMetric) {
    _dig->iMaxX = 10 * rXSizeM;
    _dig->iMaxY = 10 * rYSizeM;
    if (rUnitSizeM < 1e-6)
      rUnitSizeM = 1;
    _dig->rUnitSize = 1 / rUnitSizeM;
  }
  else {
    _dig->iMaxX = 254 * rXSizeInch;
    _dig->iMaxY = 254 * rYSizeInch;
    if (rUnitSizeInch < 1e-6)
      rUnitSizeInch = 1;
    _dig->rUnitSize = 25.4 / rUnitSizeInch;
  }
  return 0;  
}

void DigConfigSizeForm::Interpret(const char* s)
{
  _dig->Interpret(s);
  CallBack(0);
}

int DigConfigSizeForm::CallBack(Event*) 
{
  double rX = _dig->rCurrX;
  double rY = _dig->rCurrY;
  rX /= _dig->rUnitSize;
  rY /= _dig->rUnitSize;
  String sX("X = %.1f", rX);
  stX->SetVal(sX);
  String sY("Y = %.1f", rY);
  stY->SetVal(sY);
  String sButton;
  if (_dig->iButton < 0)
    sButton = String("%S: %S", SDGRemButton, SDGRemNone);
  else
    sButton = String("%S: %i", SDGRemButton, _dig->iButton);
  stButton->SetVal(sButton);
  
  sX = "";
  sY = "";
  if (rUNDEF != rX && rUNDEF != rY) {
    if (fMetric) {
      frUM->StoreData();
      if (rUnitSizeM < 1e-6)
        rUnitSizeM = 1;
      rX /= rUnitSizeM;
      rY /= rUnitSizeM;
      sX = String("X = %.1f mm", rX);
      sY = String("Y = %.1f mm", rY);
    }
    else {
      frUI->StoreData();
      if (rUnitSizeInch < 1e-6)
        rUnitSizeInch = 1;
      rX /= rUnitSizeInch;
      rY /= rUnitSizeInch;
      sX = String("X = %.2f inch", rX);
      sY = String("Y = %.2f inch", rY);
    }
  }  
  stXmm->SetVal(sX);
  stYmm->SetVal(sY);
  return 0;  
}

int DigConfigSizeForm::MetricCallBack(Event*) 
{
  cbMetric->StoreData();
  if (fMetric) {
		frXI->StoreData();
		frYI->StoreData();
		frUI->StoreData();
		rXSizeM = rXSizeInch * 25.4;
		rYSizeM = rYSizeInch * 25.4;
		rUnitSizeM = rUnitSizeInch / 25.4;
		frXM->SetVal(rXSizeM);
		frYM->SetVal(rYSizeM);
		frUM->SetVal(rUnitSizeM);
    frXM->Show();
    frYM->Show();
    frUM->Show();
    frXI->Hide();
    frYI->Hide();
    frUI->Hide();
  }
  else {
		frXM->StoreData();
		frYM->StoreData();
		frUM->StoreData();
		rXSizeInch = rXSizeM / 25.4;
		rYSizeInch = rYSizeM / 25.4;
		rUnitSizeInch = rUnitSizeM * 25.4;
		frXI->SetVal(rXSizeInch);
		frYI->SetVal(rYSizeInch);
		frUI->SetVal(rUnitSizeInch);
    frXM->Hide();
    frYM->Hide();
    frUM->Hide();
    frXI->Show();
    frYI->Show();
    frUI->Show();
  }
  CallBack(0);
  return 0;
}



