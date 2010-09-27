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


#ifndef FIELDDOM_H
#define FIELDDOM_H
#include "Client\FormElements\fldrpr.h"
#include "Client\FormElements\fentvalr.h"
#include "Engine\Domain\dminfo.h"


class _export FieldDomainC: public FieldDataTypeC
{
public:
	FieldDomainC(FormEntry* fe, const String& sQuestion, String* sDomain)
		: FieldDataTypeC(fe, sQuestion, sDomain, new DomainLister(0), true,
			(NotifyProc)&FieldDomainC::CreateDomain),
			dmTypes(dmCLASS|dmIDENT|dmVALUE) 
	{}
	FieldDomainC(FormEntry* fe, const String& sQuestion, String* sDomain, long types)
		: FieldDataTypeC(fe, sQuestion, sDomain, new DomainLister(types), true,
			(NotifyProc)&FieldDomainC::CreateDomain),
			dmTypes(types) 
	{}
	FieldDomainC(FormEntry* fe, const String& sQuestion, String* sDomain, long types, bool fAll)
		: FieldDataTypeC(fe, sQuestion, sDomain, new DomainAllExtLister(types), true,
			(NotifyProc)&FieldDomainC::CreateDomain),
			dmTypes(types) 
	{}
	void SetVal(const String& sDomain);
	void SetValueRange(const ValueRange&);

private:
	long dmTypes;
	String sNewName;
	ValueRange m_vr;
	int CreateDomain(void *);
};

class _export FormCreateDomain: public FormWithDest
{
public:
	FormCreateDomain(CWnd* wPar, String* sDom, long types = dmCLASS|dmIDENT|dmVALUE|dmBOOL|dmTIME);
	FormCreateDomain(CWnd* wPar, String* sDom, const DomainInfo&, long types = dmCLASS|dmIDENT|dmVALUE|dmBOOL|dmTIME);
	~FormCreateDomain();
private:
	void init(dmType);
	int exec();
	int CallBackName(Event *);
	long dmTypes;
	FieldDataTypeCreate* fdm;
	String* sDomain;
	String sNewName;
	String sDescr;
	RadioGroup* rg;
	long dmt[5];
	int iRgVal;
	bool fValueInt, fDomainGroup;
	long iNr;
	String sPrefix;
	StaticText* stRemark;
	CWnd* wParent;
	bool fDomainInfo;
	DomainInfo dmInf;
	bool useDate;
	int timeFormat;
	
	int CallBackVrrChange(Event*);
	int CallBackPrefixChange(Event*);
	int CallBackNrItemsChange(Event*);
	int SetUseDate(Event *ev);
	FieldInt* fiNrItems;
	FieldString* fsPrefix;
	FieldInt* fiWidth;
	CheckBox *cbDate;
	FieldTimeInterval *fInterval;
	RadioGroup *rgTime;

	int iWidth;
	bool fVrOk, fPrefixOk;
	FieldValueRange* fvr;
	DomainValueReal* dvr;
	ValueRange vr;
	ILWIS::TimeInterval interval;
	ILWIS::Time::Mode mode;
	/*ILWIS::Time endTime;
	ILWIS::Time beginTime;
	ILWIS::Duration timeStep;*/
};


#endif // FIELDDOMAIN_H




