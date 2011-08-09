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
// SMCEWindow.cpp: implementation of the SMCEWindow class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Headers\constant.h"
#include "Client\Editors\SMCE\SMCEWindow.h"
#include "Headers\Htp\Ilwis.htp"

IMPLEMENT_DYNCREATE(SMCEWindow, DataWindow)

BEGIN_MESSAGE_MAP(SMCEWindow, DataWindow)
	ON_WM_CREATE()
	ON_COMMAND(ID_CT_TOOLBAR, OnToolBar)
	ON_UPDATE_COMMAND_UI(ID_CT_TOOLBAR, OnUpdateToolBar)
	ON_COMMAND(ID_CT_EXIT, OnClose)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SMCEWindow construction/destruction

SMCEWindow::SMCEWindow()
{
	help = "ilwis\\smce_window.htm";
	sHelpKeywords = "SMCE Window";
}

SMCEWindow::~SMCEWindow()
{
}

#define sMen(ID) ILWSF("men",ID).c_str()
#define add(ID) menPopup.AppendMenu(MF_STRING, ID, sMen(ID)); 
#define addBreak menPopup.AppendMenu(MF_SEPARATOR);
#define addMenu(ID) men.AppendMenu(MF_POPUP, (UINT)menPopup.GetSafeHmenu(), sMen(ID)); menPopup.Detach();
#define addSub(ID) menSub.AppendMenu(MF_STRING, ID, sMen(ID)); 
#define addSubBreak menSub.AppendMenu(MF_SEPARATOR);
#define addSubMenu(ID) menPopup.AppendMenu(MF_POPUP, (UINT)menSub.GetSafeHmenu(), sMen(ID)); menSub.Detach();

int SMCEWindow::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (DataWindow::OnCreate(lpCreateStruct) == -1)
		return -1;
	EnableDocking(CBRS_ALIGN_ANY);

  CMenu men;
	CMenu menPopup;
	CMenu menSub;
  men.CreateMenu();

  menPopup.CreateMenu();
	add(ID_CT_FILE_NEW);
	add(ID_CT_FILE_OPEN);
	add(ID_CT_FILE_SAVE);
	add(ID_CT_FILE_SAVE_AS);
  addBreak;
	add(ID_CT_EXIT);
  addMenu(ID_MEN_FILE);

  menPopup.CreateMenu();
		menSub.CreateMenu();
	  addSub(ID_CT_GROUP);
		addSubBreak;
	  addSub(ID_CT_FACTOR);
		addSub(ID_CT_CONSTRAINT);
		addSubBreak;
		addSub(ID_CT_SCOREFACTOR);
		addSub(ID_CT_SCORECONSTRAINT);
	  addSubMenu(ID_CT_INSERT_SUB);
	add(ID_CT_EDIT);
	add(ID_CT_DELETE);
	add(ID_CT_SETALTERNATIVES);
	// add(ID_CT_DESCRIPTION); not (yet) implemented
	addMenu(ID_MEN_EDIT);

  menPopup.CreateMenu();
	add(ID_CT_EDITTREE);
	add(ID_CT_STDWEIGH);
	addMenu(ID_CT_MODE);

  menPopup.CreateMenu();
	add(ID_CT_MAPAGGREGATION);
	add(ID_CT_NOMAPAGGREGATION);
	addBreak;
	add(ID_CT_STANDARDIZE);
	add(ID_CT_SHOW_STANDARDIZED);
	addBreak;
	add(ID_CT_WEIGH);
	addMenu(ID_CT_ANALYSIS);

  menPopup.CreateMenu();
	add(ID_CT_SELECTEDMAP);
	add(ID_CT_ALLMAPS);
	addBreak;
	add(ID_CT_HISTOGRAM);
	add(ID_CT_AGGREGATE_VALUES);
	add(ID_CT_SLICE);
	add(ID_CT_SHOW_SLICED);
	add(ID_CT_HISTOGRAM_SLICED);
	add(ID_CT_AGGREGATE_VALUES_SLICED);
	addBreak;
	add(ID_CT_GENERATE_CONTOUR_MAPS);
	add(ID_CT_SHOW_CONTOUR_MAPS);
	addMenu(ID_CT_GENERATE);

	menPopup.CreateMenu();
	add(ID_CT_OVERLAYMAPS);
	add(ID_CT_TOOLBAR);
	add(ID_STATUSLINE);
	addMenu(ID_MEN_VIEW);

  SetMenu(&men);
  menPopup.Detach();
  men.Detach();

	AddHelpMenu();
	SetAcceleratorTable();

	bbSMCE.Create(this, "smce.but", "SMCE Bar", 100);
	DockControlBar(&bbSMCE, AFX_IDW_DOCKBAR_TOP);

	SetWindowName("SMCE Window");
	
	return 0;
}

void SMCEWindow::OnToolBar()
{
	if (bbSMCE.IsWindowVisible())
		ShowControlBar(&bbSMCE,FALSE,FALSE);
	else
		ShowControlBar(&bbSMCE,TRUE,FALSE);
}

void SMCEWindow::OnUpdateToolBar(CCmdUI* pCmdUI)
{
	bool fCheck = bbSMCE.IsWindowVisible() != 0;
	pCmdUI->SetCheck(fCheck);
}
