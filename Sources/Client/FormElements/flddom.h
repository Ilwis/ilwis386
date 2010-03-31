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
/* $Log: /ILWIS 3.0/FormElements/flddom.h $
 * 
 * 9     12/14/01 16:20 Willem
 * - Added option to create new bool domains
 * - Domain name callback now also checks on system domains
 * 
 * 8     18/08/00 18:26 Willem
 * Both FormCreateDomain constructors now accept types to limit the
 * possible domain types that can be created
 * 
 * 7     28/03/00 16:20 Willem
 * Removed superfluous member variable
 * 
 * 6     28/03/00 9:56 Willem
 * The CreateDomain form now displays the prefix for ID's only when the
 * user selects to create more than zero items
 * 
 * 5     27/03/00 17:28 Willem
 * Added callbacks 
 * 
 * 4     2/02/00 15:31 Willem
 * Added ValueRange member and a SetValueRange member function to
 * be able to set the ValueRange in the FormCreateDomain
 * 
 * 3     23/11/99 12:13 Willem
 * Comments for the $Log
 * 
 * 2     23/11/99 11:28 Willem
 * - The DomainCreateForm now also handles value ranges for DomainValue
 * - removed form classes: DomainPtrForm, DomainValueRealForm and
 * DomainValueIntForm
 */
// Revision 1.4  1998/10/08 09:15:03  Wim
// Added DomainPtrForm, DomainValueIntForm and DomainValueRealForm
// from domedit.c
//
// Revision 1.3  1998-09-16 18:37:53+01  Wim
// 22beta2
//
// Revision 1.2  1997/07/28 17:07:10  Wim
// Added FieldDomainC constructor with extra boolean to use DomainAllExtLister.
//
/* FieldDomain
   by Wim Koolhoven, may 1995
   Copyright Ilwis System Development ITC
	Last change:  WK    8 Oct 98    9:59 am
*/

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
	FormCreateDomain(CWnd* wPar, String* sDom, long types = dmCLASS|dmIDENT|dmVALUE|dmBOOL);
	FormCreateDomain(CWnd* wPar, String* sDom, const DomainInfo&, long types = dmCLASS|dmIDENT|dmVALUE|dmBOOL);
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
	long dmt[4];
	int iRgVal;
	bool fValueInt, fDomainGroup;
	long iNr;
	String sPrefix;
	StaticText* stRemark;
	CWnd* wParent;
	bool fDomainInfo;
	DomainInfo dmInf;
	
	int CallBackVrrChange(Event*);
	int CallBackPrefixChange(Event*);
	int CallBackNrItemsChange(Event*);
	FieldInt* fiNrItems;
	FieldString* fsPrefix;
	FieldInt* fiWidth;
	int iWidth;
	bool fVrOk, fPrefixOk;
	FieldValueRange* fvr;
	DomainValueReal* dvr;
	ValueRange vr;
};


#endif // FIELDDOMAIN_H




