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
// FormBaseView.h: interface for the FormBaseView class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(FORMBASEVIEW_H)
#define FORMBASEVIEW_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#undef IMPEXP  
#ifdef ILWISCLIENT
#define IMPEXP _export
#else
#define IMPEXP __declspec(dllimport)
#endif

class IMPEXP FormBaseView : 
	public CScrollView,
	public FormBase
{
protected:
	FormBaseView();
  DECLARE_DYNCREATE(FormBaseView)
	~FormBaseView();
  virtual void CreateForm();
private:
	void OnInitialUpdate();
public:
  virtual CWnd* wnd();
  virtual void create();
  int exec();
  int Copy();
  int Print();
	bool fDataHasChanged();
	void DataHasChanged(bool fV);
	virtual int DataChanged(Event *);

  // Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(FormBaseView)
	protected:
	virtual void OnDraw(CDC* pDC);      // empty!
  BOOL PreCreateWindow(CREATESTRUCT& cs);
  BOOL PreTranslateMessage(MSG* pMsg);
  //}}AFX_VIRTUAL
	//{{AFX_MSG(GeneralFormView)
	afx_msg void OnPaint();
  void OnDrawItem( int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct );
  void OnMeasureItem( int nIDCtl, LPMEASUREITEMSTRUCT mi );
  void OnOK();                                 // called if Ok button clicked
  void OnCancel();                             // called if Cancel button clicked
  void OnHelp();
	//}}AFX_MSG
protected:
  void shutdown(int iReturn=IDCANCEL); 
  void OnSetFocus( CWnd* pOldWnd );
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	void RecalculateForm();
	void ResizeParentToFit(BOOL bShrinkOnly, const zDimension& dmForm);

	bool fDataChanged;

  DECLARE_MESSAGE_MAP( );
};


#endif // !defined(FORMBASEVIEW_H)
