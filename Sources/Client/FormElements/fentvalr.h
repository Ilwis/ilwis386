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
/* $Log: /ILWIS 3.0/FormElements/fentvalr.h $
* 
* 6     2/26/02 11:17a Martin
* added show functions
* 
* 5     26-09-01 19:28 Koolhoven
* made Domain dm a member, because otherwise the pointer dv becomes
* invalid
* 
* 4     17-08-01 15:11 Koolhoven
* take fFixedRange() and fFixedPrecision() of domain into account by
* making these fields read only
* 
* 3     27/07/00 13:00 Willem
* Adjust $Log:
* 
* 2     27/07/00 11:58 Willem
* Added Enable() and Disable() functions for FieldRangeInt,
* FieldRangeReal and FieldValueRange
*/
// Revision 1.4  1998/09/16 17:37:53  Wim
// 22beta2
//
// Revision 1.3  1997/08/13 15:44:05  Wim
// Corrected SetCallBack() functions, so that step is included
//
// Revision 1.2  1997-07-24 18:03:53+02  Wim
// ShowHide() function added, same as DomainCallBack() but no SetVal()
// called for vr.
//
/* ui/fentvalr.h
Interface for ValueRangeReal formentries
by Wim Koolhoven, oct 1996
(c) Ilwis System Development ITC
Last change:  WK   13 Aug 97    5:35 pm
*/
#ifndef FENTVALRANGE_H
#define FENTVALRANGE_H
#include "Engine\Base\DataObjects\valrange.h"


class _export FieldDomainC;

class FieldValueRange: public FieldGroup
{
public:
	_export FieldValueRange(FormEntry* parent, const String& sQuestion,
		ValueRange* rng, FieldDataType* fdc, 
		const ValueRange& = ValueRange(-1e300,1e300,1e-6));
	void _export SetVal(const ValueRange& rng);
	void SetCallBack(NotifyProc np)
	{ FieldGroup::SetCallBack(np);
	frr->SetCallBack(np);
	}      
	void SetCallBack(NotifyProc np, CallBackHandler* cb)
	{ FieldGroup::SetCallBack(np, cb);
	frr->SetCallBack(np,cb);
	}      
	void StoreData();          // overriden
	virtual void Align(FormEntry*, enumAlign, int iOffSet = 0); // align entry under or after other

	void _export Enable();
	void _export Disable();
	void _export Show();

private:
	int StepChangeCallBack(void *);   
public:  
	int _export DomainCallBack(Event *);
private:
	ValueRange* vr;
	FieldRangeReal* frr;
	FieldReal* frStep;
	RangeReal rr;
	double rStep;
	FieldDataType* fdc;
	Domain dm;
	DomainValue* dv;
};

class _export FieldTimeInterval : public FieldGroup {
public:
	FieldTimeInterval(FormEntry* parent, const String& sQuestion,
		ILWIS::TimeInterval* rng, FieldDataType* fdc, 
		ILWIS::Time::Mode* m=NULL);
	void SetVal(const ILWIS::TimeInterval& tiv);
	void Show();
	int DomainCallBack(Event *);
	void setMode(ILWIS::Time::Mode m);
	void StoreData();
private:
	ILWIS::TimeInterval *interval;
	FieldDataType* fdc;
	FieldTime  *ft1;
	FieldTime  *ft2;
	FieldTime  *ft3;
	ILWIS::Time begin, end;
	ILWIS::Duration duration;
	ILWIS::Time::Mode mode;
    ILWIS::Time::Mode* modeOld;
	Domain dm;

	int StepChangeCallBack(Event *);
	FormEntry *CheckData();
	void Enable();
	void Disable();

};


#endif // FENTVALRANGE_H





