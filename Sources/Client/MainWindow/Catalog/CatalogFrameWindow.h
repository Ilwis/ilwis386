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
/* Interface for CatalogFrameWindow
// by Wim Koolhoven, nov. 1994
// (c) Ilwis System Development ITC
	Last change:  WK   14 Jan 99   10:20 am
*/
#ifndef CATALOGFRAMEWINDOW_H
#define CATALOGFRAMEWINDOW_H

#undef IMPEXP
#ifdef ILWISCLIENT
#define IMPEXP __declspec(dllexport)
#else
#define IMPEXP __declspec(dllimport)
#endif

class IlwisSettings;
class CDocument;

class IMPEXP CatalogFrameWindow: public CMDIChildWnd
{
public:
	void move(int x, int y, int w, int h) { MoveWindow(x,y,w,h); }
	void move(CRect& r) { MoveWindow(&r); }
	void move(CRect* r) { MoveWindow(r); }
	void resize(CSize&);    // change size (increment)

	virtual void InitSettings();
	void SaveSettings(IlwisSettings& ilwSettings, const WINDOWPLACEMENT& place, int iNr);

	virtual void GetMessageString(UINT nID, CString& rMessage) const;
	virtual void GetToolText(UINT uID, CString& sMess) const;
	void AddHelpMenu();

	void SetWindowName(String sName);
	String sWindowName();
protected:
	virtual void SaveState(IlwisSettings& ilwSettings);
	virtual void LoadState(IlwisSettings& ilwSettings);

	CatalogFrameWindow();
	virtual ~CatalogFrameWindow();
  int OnCreate(LPCREATESTRUCT lpCreateStruct);
	BOOL OnToolTipText(UINT id, NMHDR * pNMHDR, LRESULT * pResult);

	//{{AFX_VIRTUAL(CatalogFrameWindow)
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL
	//{{AFX_MSG(CatalogFrameWindow)
	afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM lParam);
	afx_msg void OnHelp();
	afx_msg void OnRelatedTopics();
	afx_msg void OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd);
	//}}AFX_MSG

	virtual void OnUpdateFrameTitle(BOOL bAddToTitle);
  HelpTopic htpTopic;
  virtual HelpTopic htp() { return htpTopic; }
	String sHelpKeywords;

private:
	String m_sWindowName;
	DECLARE_DYNCREATE(CatalogFrameWindow)
	DECLARE_MESSAGE_MAP()
};

#endif
