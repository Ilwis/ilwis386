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
// Editor.h: interface for the Editor class.
//
//////////////////////////////////////////////////////////////////////

#pragma once;

namespace ILWIS {
class NewDrawer;
}

class Editor : public CCmdTarget

{
public:
	Editor(MapPaneView*);
	virtual ~Editor();
	virtual Domain dm() const;
	virtual Representation rpr() const;
	virtual DomainValueRangeStruct dvrs() const;
	bool fOk;
	bool OnSetCursor(); // called by MapPaneView::OnSetCursor
	virtual void PreDraw(); // called before all other draw routines
	virtual int draw(volatile bool* fDrawStop) = 0;
	virtual bool OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual bool OnLButtonDown(UINT nFlags, CPoint point);
	virtual bool OnLButtonUp(UINT nFlags, CPoint point);
	virtual bool OnLButtonDblClk(UINT nFlags, CPoint point);
	virtual bool OnMouseMove(UINT nFlags, CPoint point);
	virtual bool OnContextMenu(CWnd* pWnd, CPoint point);
	virtual LRESULT OnUpdate(WPARAM, LPARAM);
	virtual void PreSaveState();
	virtual zIcon icon() const { return zIcon(); }
	virtual String sTitle() const { return "?"; }

	MapPaneView* pane() { return mpv; }
	virtual void StartBusy();
	virtual void EndBusy();
	virtual int Edit(const Coord&);
	virtual void EditFieldOK(Coord, const String&);
	void UpdateMenu();
	NewDrawer* dr() const { return drw; }
	HelpTopic htp() const { return htpTopic; }
	void ClearEditFieldPointer() { wEditField = 0; }
protected:
	void EditFieldStart(Coord, const String&);
	int AskValue(const String& sRemark, unsigned int htp=0);
	MapPaneView* mpv;
	NewDrawer* drw;
	zCursor curActive;
	DomainValueRangeStruct dvs;
	Representation _rpr;
	String sValue;
	HMENU hmenFile, hmenEdit;
	HelpTopic htpTopic;
	String sHelpKeywords;
	//{{AFX_VIRTUAL(Editor)
	//}}AFX_VIRTUAL
	//{{AFX_MSG(Editor)
	afx_msg void OnEdit();
	afx_msg void OnExit();
	afx_msg void OnHelp();
	afx_msg void OnRelatedTopics();
	String help;
	//}}AFX_MSG
private:
	CWnd* wEditField;
	DECLARE_MESSAGE_MAP()
};


