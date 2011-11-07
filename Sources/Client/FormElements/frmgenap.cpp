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

#include "Client\Headers\formelementspch.h"
#include "Engine\base\system\engine.h"
#include "Headers\messages.h"
#include "Client\FormElements\frmgenap.h"
#include "Client\ilwis.h"
#include <afxdisp.h>        // MFC Automation classes

BEGIN_MESSAGE_MAP(FormGeneralApplication, FormBaseDialog)
  ON_COMMAND(IDHELP, OnHelp)
  ON_COMMAND(ID_HELP, OnHelp)
  ON_COMMAND(IDYES, OnShow)
  ON_COMMAND(IDNO, OnDefine)
END_MESSAGE_MAP()    



FormGeneralApplication::FormGeneralApplication(CWnd* parent, const String& sTitle)
: FormBaseDialog(parent, sTitle, true, false, true),
hwnd(0)
{
	int iImg = IlwWinApp()->iImage("Exe16Ico");
	HICON hIco = IlwWinApp()->ilSmall.ExtractIcon(iImg);
	SetIcon(hIco,FALSE);

	if ( getEngine()->getContext()->pGetThreadLocalVar(IlwisAppContext::tlvMAPWINDOWAPP)==0 )
		fbs	|= fbsBUTTONSUNDER | fbsAPPLIC;
	else {
		fbs |= fbsBUTTONSUNDER;
		long *handle = (long *)getEngine()->getContext()->pGetThreadLocalVar(IlwisAppContext::tlvMAPWINDOWAPP);
		hwnd = *(HWND *)handle;
		getEngine()->getContext()->SetThreadLocalVar(IlwisAppContext::tlvMAPWINDOWAPP, 0);
	}
}

FormGeneralApplication::~FormGeneralApplication()
{
}

void FormGeneralApplication::openMap(const IlwisObject& obj){
	if (!hwnd) {
		if (fShow) {
			String sExec = "show " + obj->sNameQuoted(true);
			IlwWinApp()->Execute(sExec);
		}
	} else {
		String *filename = new String("%S", obj->fnObj.sRelative());
		::PostMessage(hwnd,ILWM_OPENMAP,(WPARAM)filename,0);
	}
}


int FormGeneralApplication::exec()
{
  root->StoreData(); 
  return 0;
}

void FormGeneralApplication::SetCommand(String sCmd)
{
	IlwWinApp()->SetCommandLine(sCmd, true);
}

void FormGeneralApplication::OnHelp()
{
	FormBaseDialog::OnHelp();
}

void FormGeneralApplication::OnShow()
{
	fShow = true;
	OnOK();
}

void FormGeneralApplication::OnDefine()
{
	fShow = false;
	OnOK();
}





