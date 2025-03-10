/***************************************************************
 ILWIS integrates image, vector and thematic data in one unique 
 and powerful package on the desktop. ILWIS delivers a wide 
 range of feautures including import/export, digitizing, editing, 
 analysis and display of data as well as production of 
 quality mapsinformation about the sensor mounting platform
 
 Exclusive rights of use by 52�North Initiative for Geospatial 
 Open Source Software GmbH 2007, Germany

 Copyright (C) 2007 by 52�North Initiative for Geospatial
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
#include "Client\Base\datawind.h"
#include "Headers\constant.h"
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Client\Base\Res.h"
#include "Engine\Base\AssertD.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Engine\Scripting\Script.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\Base\BaseView2.h"
#include "Client\Editors\TextDoc.h"
#include "Client\Base\ButtonBar.h"
#include "Client\Editors\ScriptDoc.h"
#include "Client\Editors\TextView.h"
#include "Client\Editors\ScriptView.h"
#include "Client\Editors\ScriptParamView.h"
#include "Client\Editors\ScriptParamDefaultView.h"
#include "Client\Editors\TextWindow.h"
#include "Client\Editors\ScriptWindow.h"
#include "Headers\Hs\Script.hs"
#include "Headers\Htp\Ilwis.htp"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ScriptWindow

IMPLEMENT_DYNCREATE(ScriptWindow, TextWindow)

BEGIN_MESSAGE_MAP(ScriptWindow, TextWindow)
	//{{AFX_MSG_MAP(ScriptWindow)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	ON_WM_CREATE()
	ON_COMMAND(ID_SCRIPTBUTTONBAR,	OnButtonBar)
	ON_UPDATE_COMMAND_UI(ID_SCRIPTBUTTONBAR,	OnUpdateBar)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ScriptWindow construction/destruction

ScriptWindow::ScriptWindow()
{
	help = "ilwis\\script_editor_functionality.htm";
	sHelpKeywords = "Scripts";
	SetWindowName("ScriptEditor");
}

ScriptWindow::~ScriptWindow()
{
}

#define sMen(ID) ILWSF("men",ID).c_str()

#define add(ID) menPopup.AppendMenu(MF_STRING, ID, sMen(ID)); 
#define addBreak menPopup.AppendMenu(MF_SEPARATOR);
#define addMenu(ID) men.AppendMenu(MF_POPUP, (UINT)menPopup.GetSafeHmenu(), sMen(ID)); menPopup.Detach();

int ScriptWindow::OnCreate(LPCREATESTRUCT lpc)
{
	if (-1 == TextWindow::OnCreate(lpc))
		return -1;

  CMenu men;
	CMenu menPopup;
  men.CreateMenu();

  menPopup.CreateMenu();
	add(ID_FILE_OPEN);
	add(ID_FILE_SAVE);
	add(ID_FILE_SAVE_COPY_AS);
	add(ID_FILE_RUN_SCRIPT);
	add(ID_FILE_PRINT);
	addBreak;
	add(ID_PROP);
	addBreak;
	add(ID_FILE_CLOSE);
  addMenu(ID_MEN_FILE);

  menPopup.CreateMenu();
	add(ID_EDIT_UNDO);
	add(ID_EDIT_DELETE);
	add(ID_EDIT_CUT);
	add(ID_EDIT_COPY);
	add(ID_EDIT_PASTE);
	addBreak;
	add(ID_EDIT_SELECT_ALL);
	addBreak;
	add(ID_EDIT_FIND);
	add(ID_EDIT_REPLACE);
  addMenu(ID_MEN_EDIT);

  menPopup.CreateMenu();
	add(ID_DESCRIPTIONBAR)
	add(ID_SCRIPTBUTTONBAR);
	add(ID_STATUSLINE)
  addMenu(ID_MEN_VIEW);

  menPopup.CreateMenu();
  add(ID_HLPKEY);
	add(ID_HELP_RELATED_TOPICS);
	addBreak
  add(ID_HLPCONTENTS);
  add(ID_HLPINDEX);
  add(ID_HLPSEARCH);
	addBreak
  add(ID_HLPMAPTABCALC);
  add(ID_HLPILWEXPRES);
	add(ID_HLPSCRIPTSYNTAX);
	addBreak
	add(ID_ABOUT)
  addMenu(ID_MEN_HELP);

  SetMenu(&men);
  menPopup.Detach();
  men.Detach();

	SetAcceleratorTable();
	return 0;
}

BOOL ScriptWindow::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	tabWnd.Create(WS_VISIBLE|WS_CHILD, this, 0x1245);
	tabWnd.CreateView(TR("Script").c_str(), RUNTIME_CLASS(ScriptView), pContext);
	tabWnd.CreateView(TR("Parameters").c_str(), RUNTIME_CLASS(ScriptParamView), pContext);
	tabWnd.CreateView(TR("Default Values").c_str(), RUNTIME_CLASS(ScriptParamDefaultView), pContext);
	ModifyStyleEx(WS_EX_CLIENTEDGE, 0, SWP_FRAMECHANGED);
	return TRUE;	
}

void ScriptWindow::LoadState(IlwisSettings& settings)
{
	TextWindow::LoadState(settings);

	long iActTab = settings.iValue("ActiveTabPage");
	if (iActTab >= 0 && iActTab <= 2)
		tabWnd.SwitchView(iActTab);
}

void ScriptWindow::SaveState(IlwisSettings& settings)
{
	TextWindow::SaveState(settings);

	settings.SetValue("ActiveTabPage", tabWnd.m_selTab);
}

/////////////////////////////////////////////////////////////////////////////
// ScriptWindow diagnostics

#ifdef _DEBUG
void ScriptWindow::AssertValid() const
{
	TextWindow::AssertValid();
}

void ScriptWindow::Dump(CDumpContext& dc) const
{
	TextWindow::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// ScriptWindow message handlers

void ScriptWindow::OnButtonBar()
{
	OnBarCheck(ID_SCRIPTBUTTONBAR);
}

void ScriptWindow::OnUpdateBar(CCmdUI* pCmdUI)
{
	OnUpdateControlBarMenu(pCmdUI);
}

