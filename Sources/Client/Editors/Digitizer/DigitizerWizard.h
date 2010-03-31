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
// DigitizerWizard.h: interface for the DigitizerWizard class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIGITIZERWIZARD_H__5A0F82E6_32DA_11D4_B8BA_00A0C9D5342F__INCLUDED_)
#define AFX_DIGITIZERWIZARD_H__5A0F82E6_32DA_11D4_B8BA_00A0C9D5342F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_FORMBASEPROPERTYPAGE_H__326B98B4_6CDC_11D3_B7E1_00A0C9D5342F__INCLUDED_)
#include "Client\FormElements\FormBasePropertyPage.h"
#endif

class DigitizerWizardPage: public FormBaseWizardPage
{
protected:
	DigitizerWizardPage(DigitizerWizard* dw, const String& sTitle, Digitizer* dig);
  virtual void Interpret(const char*);
	virtual void OnCancel();
  virtual BOOL OnWizardFinish();

  DigitizerWizard* wiz;
	Digitizer* _dig;
private:
	afx_msg long OnSendDigString(WPARAM, LPARAM);
  DECLARE_MESSAGE_MAP()
};


class DigitizerWizard: public CPropertySheet
{
	friend class DigAutoForm;
public:
	DigitizerWizard(CWnd* wnd, Digitizer* dig);
	virtual ~DigitizerWizard();
private:
	afx_msg long OnSendDigString(WPARAM, LPARAM);
	FormBaseWizardPage *pgSetPort, *pgPortSettings, 
		*pgAuto, *pgManual, *pgSize;
  DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_DIGITIZERWIZARD_H__5A0F82E6_32DA_11D4_B8BA_00A0C9D5342F__INCLUDED_)
