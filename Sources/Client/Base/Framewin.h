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
/* Interface for FrameWindow
// by Wim Koolhoven, nov. 1994
// (c) Ilwis System Development ITC
	Last change:  WK   14 Jan 99   10:20 am
*/
#ifndef FRAMEWINDOW_H
#define FRAMEWINDOW_H

#if !defined(AFX_ACCELERATORMANAGER_H__4563CF77_639B_11D4_B8E4_00A0C9D5342F__INCLUDED_)
#include "Client\MainWindow\AcceleratorManager.h"
#endif

#undef IMPEXP
#ifdef ILWISCLIENT
#define IMPEXP __declspec(dllexport)
#else
#define IMPEXP __declspec(dllimport)
#endif

class IlwisSettings;
class CDocument;

// superclass in ilwis
//! class FrameWindow is the base window class for ILWIS datawindows.
class IMPEXP FrameWindow: public CFrameWnd, public AcceleratorManager
{
public:
	virtual void InitialUpdate(CDocument* pDoc, BOOL bMakeVisible);

	void move(int x, int y, int w, int h) { MoveWindow(x,y,w,h); }
	void move(CRect& r) { MoveWindow(&r); }
	void move(CRect* r) { MoveWindow(r); }
	void resize(CSize&);    // change size (increment)

	virtual void InitSettings();
	virtual void SaveSettings();

	virtual void GetMessageString(UINT nID, CString& rMessage) const;
	virtual void GetToolText(UINT uID, CString& sMess) const;
	virtual void OnUpdateFrameTitle(BOOL bAddToTitle);
	void UpdateFrameTitleForDocument(LPCTSTR lpszDocName);

	void AddHelpMenu();
	void SetAcceleratorTable();

	void SetWindowName(String sName);
	String sWindowName();

	CStatusBar* status;
	int iNewBarID(); // determines bar id using one more than highest existing bar id

protected:
	virtual void SaveState(IlwisSettings& ilwSettings);
	virtual void LoadState(IlwisSettings& ilwSettings);

	void CheckMove();

	FrameWindow();
	virtual ~FrameWindow();
	BOOL ActivateMenuItem(WORD wID);
	BOOL OnToolTipText(UINT id, NMHDR * pNMHDR, LRESULT * pResult);
	void OnShowFloatingControlBars(bool fShow);

	//{{AFX_VIRTUAL(FrameWindow)
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL
	//{{AFX_MSG(FrameWindow)
	afx_msg void OnMove(int x, int y);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnClose();
	afx_msg void OnStatusLine();
	afx_msg void OnUpdateStatusLine(CCmdUI* pCmdUI);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM lParam);
	afx_msg void OnHelp();
	afx_msg void OnRelatedTopics();
	afx_msg void OnIlwisObjects();
	afx_msg BOOL OnHelpInfo(HELPINFO* lpHelpInfo);
	//}}AFX_MSG

	BOOL PreTranslateMessage(MSG* pMsg);
  HelpTopic htpTopic;
  virtual HelpTopic htp() { return htpTopic; }
	String sHelpKeywords;
	bool fStoreToolBars;
	String help;

private:
	String m_sWindowName;

	DECLARE_DYNCREATE(FrameWindow)
	DECLARE_MESSAGE_MAP()
};

/*! \class FrameWindow : public CFrameWnd
*/
/*! \fn void FrameWindow::InitialUpdate(CDocument* pDoc, BOOL bMakeVisible)
 *      This function replaces the CFrameWnd::InitialUpdateFrame() function,
 *      which is not a virtual function. The function is used to initialize
 *      the views of the frame and to display the window.
 *      The function also calls the virtual function InitSettings(), which is
 *      responsible for the restoring of the window and view settings
 *      that are stored in the registry.
 *  \param pDoc The IlwisCDocument that will be displayed in the FrameWindow
 *  \param bMakeVisible The FrameWindow is displayed when this argument is true
*/
/*! \fn virtual void FrameWindow::InitSettings()
 *      This function restores settings from the registry. The base function
 *      restores the window size and position, as well as the state and position
 *      of all toolbars (Status bars, Button bars, Sizable toolbars, etc).
 *      This function can be overruled to extend the number of settings to restore
*/
/*! \fn virtual void FrameWindow::SaveSettings()
 *      This function stores settings to the registry. The base function
 *      stores the window size and position, as well as the state and position
 *      of all toolbars (Status bars, Button bars, Sizable toolbars, etc).
 *      This function can be overruled to extend the number of settings to store.
*/
/*! \fn	virtual void FrameWindow::GetMessageString(UINT nID, CString& rMessage) const
 *      Retrieve the text to be placed on the status bar. The text is retrieved from
 *      DSC.ENG. The text is displayed when the mouse cursor is moved over the
 *      menu items of the menu. If no text could be found the empty string is returned.
 *  \param nID The ID of the menu item for which the text should be retrieved
 *  \param rMessage The text will be returned in this CString.
 */
/*! \fn	virtual void FrameWindow::GetToolText(UINT uID, CString& sMess) const
 *      Retrieve the text to be placed on a tooltip control (the small window
 *      appearing next to a button). The text is the exact same text as on the
 *      menu item with the same ID.  If no text could be found the empty string is returned.
 *  \param uID The ID of button on a buttonbar (or the menu item for that matter). This
 *         determines the string to be retrieved from MEN.ENG
 *  \param sMess The text will be returned in this CString.
 */
/*! \fn	void FrameWindow::SetAcceleratorTable()
 *      This function builds the accelerator key table from the menu entries. That
 *      is: If a menu entry in the MEN.ENG file lists an accelerator key next to
 *      the menu item, this key can also be used to induce the same command as the 
 *      menu item.
 *
 */
/*! \fn	void FrameWindow::SetWindowName(String sName)
 *      Set the name of the registry subkey to store the settings for this window
 * \param sName The name of the registry sub key where the settngs for this window
 *              will be stored. This will always be a subkey of:
 *              HKCU/Software/ITC/Ilwis/<version>, where <version> depends on the
 *              ILWIS version.
 *
 */
/*! \fn	String FrameWindow::sWindowName()
 *      Retrieve the current name of the registry key for storage of the window settings
 *      This will always be a subkey of:
 *      HKCU/Software/ITC/Ilwis/<version>, where <version> depends on the ILWIS version.
 *  \return The registry key name
 */

#endif
