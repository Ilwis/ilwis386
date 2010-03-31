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
/*
// $Log: /ILWIS 3.0/FormElements/frmgenap.cpp $
 * 
 * 12    5-07-00 9:36 Koolhoven
 * Application Forms now use Show and Define buttons instead of OK
 * 
 * 11    10-05-00 18:06 Koolhoven
 * in OnHelp() do no longer enlarge dialog box for help text but call base
 * function
 * 
 * 10    28-01-00 13:12 Koolhoven
 * New class HelpCtrl derived from CHHCtrl.
 * Clicking in this Ctrl does not open the html page inside the ctrl but
 * opens a HtmlHelp window
 * 
 * 9     12-01-00 12:35 Koolhoven
 * Replaced ilwismen.chm and ilwisapp.chm by ilwis.chm
 * 
 * 8     9-12-99 16:48 Koolhoven
 * Handle F1 (ID_HELP) in frame windows to call HtmlHelp()
 * 
 * 7     30-11-99 10:29a Martin
 * added function for setting commandline string
 * 
 * 6     21-10-99 11:22 Koolhoven
 * Now uses topic numbers to show the hep text in the ilwisapp.chm file
 * 
 * 5     5-10-99 12:55 Koolhoven
 * In application forms show the help in the dialog box itself.
 * Temporary fixed on SegmentTunneling,
 * as soon as the .chm file has a [MAP] section the htp number can be used
 * 
 * 4     10/01/99 1:11p Wind
 * comments
 * 
 * 3     30-09-99 15:22 Koolhoven
 * Application Forms now use the correct icon

 */
// Revision 1.3  1998/09/16 17:37:53  Wim
// 22beta2
//
// Revision 1.2  1997/08/11 16:02:56  Wim
// HideOnOk true.
//
/* FormGeneralApplication
   by Wim Koolhoven, november 1995
   Copyright Ilwis System Development ITC
	Last change:  WK   11 Aug 97    6:01 pm
*/
#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\frmgenap.h"
#include "Client\ilwis.h"
#include <afxdisp.h>        // MFC Automation classes
#include "Client\Help\helpctrl.h"

BEGIN_MESSAGE_MAP(FormGeneralApplication, FormBaseDialog)
  ON_COMMAND(IDHELP, OnHelp)
  ON_COMMAND(ID_HELP, OnHelp)
  ON_COMMAND(IDYES, OnShow)
  ON_COMMAND(IDNO, OnDefine)
END_MESSAGE_MAP()    



FormGeneralApplication::FormGeneralApplication(CWnd* parent, const String& sTitle)
: FormBaseDialog(parent, sTitle, true, false, true)
, hhc(0)
{
	int iImg = IlwWinApp()->iImage("Exe16Ico");
	HICON hIco = IlwWinApp()->ilSmall.ExtractIcon(iImg);
	SetIcon(hIco,FALSE);

	fbs	|= fbsBUTTONSUNDER | fbsAPPLIC;
}

FormGeneralApplication::~FormGeneralApplication()
{
	delete hhc;
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
/*
	if (0 == hhc) {
		CWaitCursor curWait;
		CRect rctWnd;
		GetWindowRect(&rctWnd);
		int iWidth = rctWnd.Width();
		int iHeight = rctWnd.Height();
		rctWnd.right += 400;

		int iCaption = GetSystemMetrics(SM_CYCAPTION); 
		int iBorder = 2 + GetSystemMetrics(SM_CYBORDER);
		int iRight = iWidth + 400 - 2 * iBorder;
		iHeight -= iCaption + 2 * iBorder;
		CRect rect(iWidth,0,iRight,iHeight);
		hhc = new HelpCtrl; //CHHCtrl;
		hhc->Create(this, rect, 0, "main");
		hhc->SetChmFile("ilwis.chm");

		MoveWindow(&rctWnd);
	}
	hhc->NavigateChm(htp().iTopic);
*/
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





