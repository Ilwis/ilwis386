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
// DigitizerWizard.cpp: implementation of the DigitizerWizard class.
//
//////////////////////////////////////////////////////////////////////

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

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(DigitizerWizard, CPropertySheet)
  ON_MESSAGE(ILWM_SENDDIGSTRING, OnSendDigString)
END_MESSAGE_MAP()    

BEGIN_MESSAGE_MAP(DigitizerWizardPage, FormBaseWizardPage)
  ON_MESSAGE(ILWM_SENDDIGSTRING, OnSendDigString)
END_MESSAGE_MAP()    



DigitizerWizard::DigitizerWizard(CWnd* wnd, Digitizer* dig)
: CPropertySheet(SDGTitleDigSetup.scVal(), wnd)
{
	SetWizardMode();
	pgSetPort = new DigSetPortForm(this, dig);	
	AddPage(pgSetPort);
	pgPortSettings = new DigPortSettings(this, dig);
	AddPage(pgPortSettings);
	pgAuto = new DigAutoForm(this, dig);
	AddPage(pgAuto);
	pgManual = new DigConfigForm(this, dig);
	AddPage(pgManual);
	pgManual->SetPPDisable();
	pgSize = new DigConfigSizeForm(this, dig);
	AddPage(pgSize);
}

DigitizerWizard::~DigitizerWizard()
{
}

long DigitizerWizard::OnSendDigString(WPARAM wp, LPARAM lp)
{
	GetActivePage()->SendMessage(ILWM_SENDDIGSTRING, wp, lp);
	return 0;
}



DigitizerWizardPage::DigitizerWizardPage(DigitizerWizard* dw, const String& sTitle, Digitizer* dig)
: FormBaseWizardPage(sTitle)
, _dig(dig), wiz(dw)
{
}

void DigitizerWizardPage::Interpret(const char*)
{
	// do nothing
}

long DigitizerWizardPage::OnSendDigString(WPARAM, LPARAM lp)
{
	Interpret((const char*)lp);
	return 0;
}

void DigitizerWizardPage::OnCancel()
{
	FormBaseWizardPage::OnCancel();
	_dig->Disable(false); // prevent calling of callback from digitizer thread
}

BOOL DigitizerWizardPage::OnWizardFinish()
{
	if (!FormBaseWizardPage::OnWizardFinish())
		return FALSE;
	_dig->Disable(false); // prevent calling of callback from digitizer thread
	return TRUE;	
}

