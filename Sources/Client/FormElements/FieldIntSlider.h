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
// FieldIntSlider.h: interface for the FieldIntSlider class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FIELDINTSLIDER_H__1B5F582F_0B46_4B10_B793_E9EF001C8F10__INCLUDED_)
#define AFX_FIELDINTSLIDER_H__1B5F582F_0B46_4B10_B793_E9EF001C8F10__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Client\FormElements\fentbase.h"

class OwnSliderCtrl : public CSliderCtrl, public BaseZapp
{
public:
	OwnSliderCtrl(FormEntry *f);
	afx_msg void HScroll(UINT nSBCode, UINT nPos);
	afx_msg void VScroll(UINT nSBCode, UINT nPos);
	DECLARE_MESSAGE_MAP();
};

class _export FieldIntSlider : public FormEntry  
{
public:
	FieldIntSlider(FormEntry* parent, int *piVal, const ValueRange& valri, DWORD dwStyle = TBS_HORZ|TBS_AUTOTICKS|TBS_BOTTOM);
	// for dwStyle options see CSlideCtrl::Create()
	virtual ~FieldIntSlider();
	void create();             // overriden
	void show(int sw);         // overriden
	void SetVal(int iVal);
	int iVal();               // return current value
	void StoreData();
	void Enable();
	void Disable();
	void SetFocus();
	FormEntry* CheckData();

private:
	int *_piVal;                // place holder for pointer to result
	int _iVal;									// local copy with most recent value; StoreData writes this to _piVal
	CSliderCtrl *slc;
	ValueRange vri;
	DWORD m_dwStyle;
};

#endif // !defined(AFX_FIELDINTSLIDER_H__1B5F582F_0B46_4B10_B793_E9EF001C8F10__INCLUDED_)
