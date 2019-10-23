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
// TiePointEditor.cpp: implementation of the TiePointEditor class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Engine\Map\Segment\Seg.h"
#include "Engine\Map\Polygon\POL.H"
#include "Client\Mapwindow\Positioner.h"
#include "Client\Editors\Editor.h"
#include "Client\Editors\Utils\SYMBOL.H"
#include "Client\Editors\Utils\sizecbar.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\TableWindow\TableDoc.h"
#include "Client\TableWindow\BaseTablePaneView.h"
#include "Client\TableWindow\TablePaneView.h"
#include "Client\Editors\Georef\GeoRefEditorTableView.h"
#include "Client\Editors\Georef\GeoRefEditorTableBar.h"
#include "Client\Editors\Georef\TransformationComboBox.h"
#include "Client\Editors\Georef\TiePointEditor.h"
#include "Headers\constant.h"
#include "Engine\Base\System\RegistrySettings.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(TiePointEditor, Editor)
	//{{AFX_MSG_MAP(TiePointEditor)
	ON_COMMAND(ID_TIEPOINTTABLE, OnTiePointTable)
	ON_UPDATE_COMMAND_UI(ID_TIEPOINTTABLE, OnUpdateTiePointTable)
	ON_COMMAND(ID_QUALITYINFOBAR, OnEditQualityInfoText)
	ON_UPDATE_COMMAND_UI(ID_QUALITYINFOBAR,OnUpdateEditQualityInfoText)
	ON_COMMAND(ID_NORMAL, OnNoTool)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define sMen(ID) ILWSF("men",ID).c_str()

TiePointEditor::TiePointEditor(MapPaneView* mpvw)
: Editor(mpvw)
, curAdd("PlusCursor")
, curChange("PlusBoxCursor")
, iSelPnt(0)
{
	curActive = curAdd;

  colActive = Color(255,255,0);  // default yellow
  colActGood = Color(0,255,0);  // default green
  colActBad = Color(255,0,0);  // default red
  colPassive = Color(0,0,255); // default blue
	IlwisSettings settings("Map Window\\TiePoint Editor");

	colActive = settings.clrValue("Active Color", colActive);
	colActGood = settings.clrValue("Good Color", colActGood);
	colActBad = settings.clrValue("Bad Color", colActBad);
	colPassive = settings.clrValue("Passive Color", colPassive);
	colActive.alpha() = 255; // from the registry they come with alpha=0
	colActGood.alpha() = 255;
	colActBad.alpha() = 255;
	colPassive.alpha() = 255;
  smb.smb = smbPlus;
	smb.iSize =	settings.iValue("Symbol Size", smb.iSize);

	//  Create a font for the combobox
	if (!::GetSystemMetrics(SM_DBCSENABLED))
	{
		LOGFONT logFont;
		memset(&logFont, 0, sizeof(logFont));
		// Since design guide says toolbars are fixed height so is the font.
		logFont.lfHeight = -12;
		logFont.lfWeight = FW_BOLD;
		logFont.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS;
		lstrcpy(logFont.lfFaceName, "MS Sans Serif");
		fnt.CreateFontIndirect(&logFont);
	}

	ilReBar.Create(16, 15, ILC_COLOR, 0, 4);
	IlwisBitmap bmTransf;
	bmTransf.Load("GrfTransf16Ico");
	ilReBar.Add(&bmTransf,(CBitmap*)0);

	CFrameWnd* fw = mpv->GetTopLevelFrame();
	if (fw) {
		CMenu* men = fw->GetMenu();
		int iNr = men->GetMenuItemCount();
		CMenu* menOptions = men->GetSubMenu(iNr-2);
		menOptions->AppendMenu(MF_STRING, ID_QUALITYINFOBAR, sMen(ID_QUALITYINFOBAR));
		menOptions->AppendMenu(MF_STRING, ID_TIEPOINTTABLE, sMen(ID_TIEPOINTTABLE));
		menOptions->AppendMenu(MF_STRING, ID_SHOWADDINFO, sMen(ID_SHOWADDINFO));
	}
}

TiePointEditor::~TiePointEditor()
{
	CFrameWnd* fw = mpv->GetTopLevelFrame();
	if (fw) {
		CMenu* men = fw->GetMenu();
		men->RemoveMenu(ID_SHOWADDINFO, MF_BYCOMMAND);
		men->RemoveMenu(ID_TIEPOINTTABLE, MF_BYCOMMAND);
		men->RemoveMenu(ID_QUALITYINFOBAR, MF_BYCOMMAND);
	}
}

void TiePointEditor::OnTiePointTable()
{
	CFrameWnd* fw = mpv->GetTopLevelFrame();
	if (0 == fw)
		return;
	if (gretBar.IsWindowVisible())
		fw->ShowControlBar(&gretBar,FALSE,FALSE);
	else
		fw->ShowControlBar(&gretBar,TRUE,FALSE);
}

void TiePointEditor::OnUpdateTiePointTable(CCmdUI* pCmdUI)
{
	if (!IsWindow(gretBar.m_hWnd))
		return;
	bool fCheck = gretBar.IsWindowVisible() != 0;
	pCmdUI->SetCheck(fCheck);
}

void TiePointEditor::SelectPoint(int iNr)
{
	mpv->SetFocus();
	iSelPnt = iNr;
	if (iSelPnt)
		curActive = curChange;
	else
		curActive = curAdd;
  OnSetCursor();
}

bool TiePointEditor::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
  bool fCtrl = GetKeyState(VK_CONTROL) & 0x8000 ? true : false;
  bool fShift = GetKeyState(VK_SHIFT) & 0x8000 ? true : false;
  switch (nChar) {
    case VK_ESCAPE:
			if (iSelPnt) {
				iSelPnt = 0;
				curActive = curAdd;
			  OnSetCursor();
				return true;
			}
      break;
	}
  return Editor::OnKeyDown(nChar, nRepCnt, nFlags);
}

void TiePointEditor::OnNoTool()
{
	if (iSelPnt) {
		iSelPnt = 0;
		curActive = curAdd;
	  OnSetCursor();
	}
	mpv->OnNoTool();
}

void TiePointEditor::OnEditQualityInfoText()
{
	
	CFrameWnd* fw = mpv->GetTopLevelFrame();
	if (0 == fw)
		return;
	
	/*
	if (edTxt.IsWindowVisible())
		edTxt.ShowWindow(SW_HIDE);
	else
		edTxt.ShowWindow(SW_SHOW);
	*/
	
	if (bbTxt.IsWindowVisible())
		fw->ShowControlBar(&bbTxt,FALSE,FALSE);
	else
		fw->ShowControlBar(&bbTxt,TRUE,FALSE);
	
}

void TiePointEditor::OnUpdateEditQualityInfoText(CCmdUI* pCmdUI)
{
	bool fCheck = edTxt.IsWindowVisible() != 0;
	pCmdUI->SetCheck(fCheck);
}
