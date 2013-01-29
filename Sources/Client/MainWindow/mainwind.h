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
#ifndef MAINWIND_H
#define MAINWIND_H

#ifndef FRAMEWINDOW_H
#include "Client\Base\Framewin.h"
#endif

#ifndef COMMAND_HANDLER_H
#endif

#if !defined(AFX_ACTIONVIEWBAR_H__964C78F3_7650_11D3_B7EE_00A0C9D5342F__INCLUDED_)
#include "Client\MainWindow\ActionViewBar.h"
#endif

#if !defined(AFX_NAVIGATORBAR_H__A4CC1334_D7C4_11D3_B84B_00A0C9D5342F__INCLUDED_)
#include "Client\MainWindow\NavigatorBar.h"
#endif

#if !defined(AFX_BUTTONBAR_H__054E1556_3E6F_11D3_9CE3_00A0C9D5320B__INCLUDED_)
#include "Client\Base\ButtonBar.h"
#endif

#if !defined(AFX_ACCELERATORMANAGER_H__4563CF77_639B_11D4_B8E4_00A0C9D5342F__INCLUDED_)
#include "Client\MainWindow\AcceleratorManager.h"
#endif

class CommandHandlerUI;

#undef IMPEXP
#ifdef ILWISCLIENT
#define IMPEXP __declspec(dllexport)
#else
#define IMPEXP __declspec(dllimport)
#endif

class Menu;
class SimpleCalcResultForm;

class IMPEXP MainWindow : public CMDIFrameWnd, public AcceleratorManager
{
public:
	enum spStatusPanes{spDESCRIPTION, spFILTER_PANE, spDEP, spCALC, spUPD, spREAD, spEMPTY, spANY};

	MainWindow();
	~MainWindow();

	void                    SetCommandLine(const String& , bool fAddToHistory=false);
	LONG                    OnIlwisStart(const char*);
	virtual void            GetMessageString(UINT nID, CString& sMessage) const;
	void                    InitialUpdate(CDocument* pDoc, BOOL bMakeVisible);
	void                    UpdateCatalogs(const FileName& fnPath=FileName());
	static                  String sBaseRegKey();
	void                    AddToHistory(const String& sDir);
	//{{AFX_VIRTUAL(MainWindow)
	virtual				BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL
	map<String, String>& mapQueries() {return mpQueries; }
	void					SetQueryMenu(Menu& menSub);
	SimpleCalcResultForm* frmSimpleCalcResults();
	void          RefreshNavigator();
	void          SetStatusLine(const String& sTxt, MainWindow::spStatusPanes=MainWindow::spDESCRIPTION);
	int           iBeginOwnerQueries();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	int	 OnCreate(LPCREATESTRUCT);
	LONG OnCopyData(UINT, LONG);
	LONG OnIlwisReportMsg(UINT, LONG);
	LONG OnExecute(UINT, LONG);
	LONG OnCommandUI(UINT, LONG);
	LRESULT OnSetCommandLine(WPARAM wPar, LPARAM);
	LRESULT OnSendUpdateAnimMessages(WPARAM p1, LPARAM p2);
	//{{AFX_MSG(MainWindow)
	afx_msg void OnDestroy();
	afx_msg void OnClose();
	afx_msg void OnCommandLine();
	afx_msg void OnUpdateCommandLine(CCmdUI* pCmdUI);
	afx_msg void OnOperationList();
	afx_msg void OnUpdateOperationList(CCmdUI* pCmdUI);
	afx_msg void OnSelBar();
	afx_msg void OnUpdateSelBar(CCmdUI* pCmdUI);
	afx_msg void OnStandardBar();
	afx_msg void OnUpdateStandardBar(CCmdUI* pCmdUI);
	afx_msg void OnRestoreAll();
	afx_msg void OnMinimizeAll();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG

private:
	void AddHelpMenu();
	void SaveStateOfMDIChildren(IlwisSettings& settings);
	void SaveState(IlwisSettings& ilwSettings);
	void LoadState(IlwisSettings& ilwSettings);
	void OnNewCatalog();
	void OnStatusLine();
	void OnUpdateStatusLine(CCmdUI* pCmdUI);
	void OnCreateObject(UINT iID);
	void OnGraph();
	void OnRoseDiagram();
	void OnSize( UINT nType, int cx, int cy );
	void OnExit();
	void OnCloseCatalogs();
	void InitSettings();
	void OnExport();
	void OnImport();
	void OnImportMap();
	void OnImportTable();
	void OnImportGdal();
	void OnImportGeoGateway();
	void OnImportGenRaster();
	void OnCatRedraw();
	void OnOpenMap();
	void OnOpenAsTable();
	void OnPreferences();
	void OnConnectDataBase();
	void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	void OnSetFocus(CWnd* pOldWnd);
	LRESULT	OnReadCatalog(WPARAM iW, LPARAM iL)	;
	LRESULT OnCloseCollection(WPARAM iW, LPARAM iL);
	LRESULT OnOpenCatalog(WPARAM iW, LPARAM iL);

	void SetAcceleratorTable();

	ButtonBar	buttonBar;
	ButtonBar	selBar;
	CommandBar commBar;
	ActionViewBar avBar;
	CStatusBar status;
	HelpTopic	htpTopic;
	map<String, String> mpQueries;
	int	iStartOwnerQueries;
	int iNrReportWindows;
	bool fAllowCatalogUpdates;
	SimpleCalcResultForm *frmSimpleCalc;
	ProgressListWindow *plwProgress;

	DECLARE_MESSAGE_MAP()
};

#endif // MAINWIND_H

