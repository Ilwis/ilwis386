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
// TiePointEditor.h: interface for the TiePointEditor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TIEPOINTEDITOR_H__242AF8D8_464E_11D3_B7B4_00A0C9D5342F__INCLUDED_)
#define AFX_TIEPOINTEDITOR_H__242AF8D8_464E_11D3_B7B4_00A0C9D5342F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class TiePointEditor: public Editor  
{
	friend class GeoRefEditorTableView;
public:
	TiePointEditor(MapPaneView*);
	virtual ~TiePointEditor();
	virtual void OnTransfChanged()=0;
	virtual void OnAddPoint()=0;
	virtual void DelPoints(int iDel, int iRecs)=0; 
	virtual void Calc()=0;
	void SelectPoint(int iNr); 
	virtual bool OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
//{{AFX_MSG(TiePointEditor)
	afx_msg void OnTiePointTable();
	afx_msg void OnUpdateTiePointTable(CCmdUI* pCmdUI);
	afx_msg void OnNoTool();
//}}AFX_MSG
protected:
	GeoRefEditorTableBar gretBar;
	CImageList ilReBar;
	CFont fnt;
	zCursor curAdd, curChange;
	CToolBar bbTxt;
	CEdit edTxt;
	//CControlBar edTxt;
	void OnEditQualityInfoText();
	void OnUpdateEditQualityInfoText(CCmdUI* pCmdUI);
	TransformationComboBox cbTransf;

  Symbol smb;
  Color colActive, colPassive, colActGood, colActBad;
	int iSelPnt;
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_TIEPOINTEDITOR_H__242AF8D8_464E_11D3_B7B4_00A0C9D5342F__INCLUDED_)
