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
// FieldRealSlider.h: interface for the FieldRealSlider class.
//
//////////////////////////////////////////////////////////////////////

#pragma once


#include "Client\FormElements\fentbase.h"
class OwnSliderCtrl;

class _export FieldRealSlider : public FormEntry  
{
public:
	FieldRealSlider(FormEntry* parent, double *prVal, const ValueRange& valri, DWORD dwStyle = TBS_HORZ|TBS_AUTOTICKS|TBS_BOTTOM);
	// for dwStyle options see CSlideCtrl::Create()
	virtual ~FieldRealSlider();
	void create();             // overriden
	void show(int sw);         // overriden
	void SetVal(double rVal);
	double rVal();               // return current value
	void StoreData();
	void Enable();
	void Disable();
	void SetFocus();
	FormEntry* CheckData();
	void setContinuous(bool v) { continuous = v; }

private:
	double *_prVal;                // place holder for pointer to result
	double _rVal;									// local copy with most recent value; StoreData writes this to _piVal
	OwnSliderCtrl *slc;
	ValueRange vrr;
	DWORD m_dwStyle;
	bool continuous;
};

class _export FieldRealSliderEx : public FormEntry {
public:
	FieldRealSliderEx(FormEntry * parent, const String& question, double *val, const ValueRange& valrang, bool txt);
	virtual ~FieldRealSliderEx();
	void create();             // overriden
	void show(int sw);         // overriden
	void SetVal(double rVal);
	double rVal();               // return current value
	void StoreData();
	void Enable();
	void Disable();
	void SetFocus();
	FormEntry* CheckData();
	void setContinuous(bool v) { if (slider) slider->setContinuous(v); }
private:
	int SliderCallBackFunc(Event *ev);
	int EditCallBackFunc(Event *ev);
	void setContinuousMode(int m);
	FieldRealSlider *slider;
	FieldReal *edit;
	bool rangeText;
	int continuous;
	bool initial;
	int setRace;

};


