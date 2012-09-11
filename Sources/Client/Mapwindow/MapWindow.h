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
#ifndef MAPWINDOW_H
#define MAPWINDOW_H
#if !defined(AFX_SCALEBAR_H__BB72FBC4_4BCE_11D3_B7BC_00A0C9D5342F__INCLUDED_)
#include "Client\Mapwindow\ScaleBar.h"
#endif
#if !defined(AFX_GeneralBar_H__A9C4CEB4_A34D_11D3_B821_00A0C9D5342F__INCLUDED_)
#include "Client\Editors\Utils\GeneralBar.h"
#endif
//#if !defined(AFX_LAYERTREEBAR_H__4E781484_676B_11D3_B7DC_00A0C9D5342F__INCLUDED_)
//#include "Client\Mapwindow\LayerTreeBar.h"
//#endif
#ifndef DataWindow_H
#include "Client\Base\datawind.h"
#endif

#undef IMPEXP
#ifdef ILWISCLIENT
#define IMPEXP __declspec(dllexport)
#else
#define IMPEXP __declspec(dllimport)
#endif

class CommandBar;
	
class IMPEXP MapWindow: public DataWindow
{
public:
	void UpdateMenu(HMENU hmFile, HMENU hmEdit);
	void UpdateMenu();
  virtual void InitSettings();
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(MapWindow)
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra,
		AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual void InitialUpdate(CDocument* pDoc, BOOL bMakeVisible);
	virtual void GetMessageString(UINT nID, CString& rMessage) const;
	//}}AFX_VIRTUAL
	CView* vwFirst() { return pFirstView; }
	bool isFullScreen() const { return fullScreen;}
	//ScaleBar barScale;
	GeneralBar ltb;

	//{{AFX_MSG(MapWindow)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLarger();
	afx_msg void OnSmaller();
	afx_msg void OnUpdateLarger(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSmaller(CCmdUI* pCmdUI);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnInitMenu(CMenu* pMenu);
  afx_msg LRESULT OnUpdate(WPARAM, LPARAM);
	afx_msg void OnLayerManagement();
	afx_msg void OnUpdateLayerManagement(CCmdUI* pCmdUI);
	afx_msg void OnOverviewWindow();
	afx_msg void OnUpdateOverviewWindow(CCmdUI* pCmdUI);
	afx_msg void OnScaleControl();
	afx_msg void OnUpdateScaleControl(CCmdUI* pCmdUI);
	afx_msg void OnFullScreen();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) ;
	afx_msg LONG OnExecute(UINT, LONG lParam);
	afx_msg LONG OnSelectFeatures(UINT, LONG lParam);
	//}}AFX_MSG

protected:
	MapWindow();           
	virtual ~MapWindow();
  //CSplitterWnd wndSplitter;
  CMenu menDataLayer, menEditLayer, menPropLayer, 
        menRprLayer, menAttLayer, menDomLayer;
	HMENU hMenFile, hMenEdit;
	GeneralBar gbOverview;
	CommandBar *commBar;
	CView* pFirstView;
	bool fullScreen;
	WINDOWPLACEMENT g_wpPrev;
	DWORD dwStyle;

	DECLARE_DYNCREATE(MapWindow)
	DECLARE_MESSAGE_MAP()
};

#endif MAPWINDOW_H
