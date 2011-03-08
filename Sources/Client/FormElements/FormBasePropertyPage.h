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
// FormBasePropertyPage.h: interface for the FormBasePropertyPage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FORMBASEPROPERTYPAGE_H__326B98B4_6CDC_11D3_B7E1_00A0C9D5342F__INCLUDED_)
#define AFX_FORMBASEPROPERTYPAGE_H__326B98B4_6CDC_11D3_B7E1_00A0C9D5342F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#undef IMPEXP
#ifdef ILWISCLIENT
#define IMPEXP __declspec(dllexport)
#else
#define IMPEXP __declspec(dllimport)
#endif


class IMPEXP FormBasePropertyPage: public CPropertyPage, public FormBase
{
	friend class IlwisPropertySheet;
public:
	FormBasePropertyPage(const string& sTitle);
	virtual ~FormBasePropertyPage();

	virtual CWnd* wnd();
	virtual void create();
	virtual void OnOK();     
	virtual int DataChanged(Event*);
protected:
	int exec();
	void shutdown(int iReturn=IDCANCEL); 
	int Copy();
	int Print();
	virtual void OnCancel();
	virtual BOOL OnApply();
	virtual BOOL OnKillActive();
	afx_msg void OnDrawItem( int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct );
	afx_msg void OnMeasureItem( int nIDCtl, LPMEASUREITEMSTRUCT mi );
	afx_msg void OnHelp();                             
private:
	bool _fApplyClicked;
	BOOL OnInitDialog();
	struct dlgt 
	{   
		DLGTEMPLATE dlg;
		short cMenu[1];
		short cClass[1];
		short cCaption[1];
	} dlgFormBase;
	DECLARE_MESSAGE_MAP( );
};


class IMPEXP FormBaseWizardPage: public FormBasePropertyPage
{
public:
	FormBaseWizardPage(const string& sTitle);
	virtual ~FormBaseWizardPage();

  void SetPPEnable(bool f = true) { fEnable = f; }
	void SetPPDisable(bool f = true) { fEnable = !f; }
	bool fPPEnabled() const { return fEnable; }
	void EnablePreviousPage();
	void SetPreviousPage(FormBaseWizardPage *page, bool fEnable=false);
	void SwitchToNextPage(FormBaseWizardPage *page);
protected:
	bool fEnable;
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
  virtual LRESULT OnWizardBack();
  virtual LRESULT OnWizardNext();
  virtual BOOL OnWizardFinish();
	FormBaseWizardPage *ppPrevious;
  DECLARE_MESSAGE_MAP( );
};


#endif // !defined(AFX_FORMBASEPROPERTYPAGE_H__326B98B4_6CDC_11D3_B7E1_00A0C9D5342F__INCLUDED_)
