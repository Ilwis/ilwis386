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
#if !defined(AFX_NAVIGATORTREECTRL_H__0B38BC94_D98C_11D3_B84F_00A0C9D5342F__INCLUDED_)
#define AFX_NAVIGATORTREECTRL_H__0B38BC94_D98C_11D3_B84F_00A0C9D5342F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NavigatorTreeCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// NavigatorTreeCtrl window

class NavigatorTreeCtrlDropTarget;

class NavigatorTreeCtrl : public CTreeCtrl
{
// Construction

	friend class NavigatorTreeCtrlDropTarget;
	
public:
	NavigatorTreeCtrl(CWnd* wParent);
	virtual ~NavigatorTreeCtrl();
	void AddToHistory(const Directory& fn);
	void OnUpdateCmdUI();

	long iHideTime;
	//{{AFX_VIRTUAL(NavigatorTreeCtrl)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void Refresh();
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(NavigatorTreeCtrl)
	afx_msg void OnItemExpanding(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	void ExpandDir(HTREEITEM hti);
	void ExpandPath(HTREEITEM hti, const String& sPath);
	void Fill();	
	Array<FileName> afn;
private:
	void AddToHistoryNode(const String& fn);
	BOOL OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
	DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	DROPEFFECT OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 	;
	void OnContextMenu(CWnd* pWnd, CPoint point) 	;
	void OnRButtonDown(UINT nFlags, CPoint point);
	void OnDragLeave();
	void handleWMSMenu(CPoint pt);
	void AddToWMSNode(const String& sFn);
	void handleUrl(CPoint point, HTREEITEM hit);
	void AddToUrlNode(HTREEITEM hti, const FileName& fn);
	URL completeWMSUrl(const String& txt);

	HTREEITEM htiHistory;
	HTREEITEM htiWMS;
	list<Directory> lsHistory;
	list<String> lsWMSUrls;
	String sLastScrollDir;
	int iFmtCopy;

	NavigatorTreeCtrlDropTarget* odt;	

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NAVIGATORTREECTRL_H__0B38BC94_D98C_11D3_B84F_00A0C9D5342F__INCLUDED_)
