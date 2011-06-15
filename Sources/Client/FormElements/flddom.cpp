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
/* FieldDomain
   by Wim Koolhoven, may 1995
   Copyright Ilwis System Development ITC
	Last change:  WK    8 Oct 98   10:11 am
*/


#include "Client\Headers\formelementspch.h"
#include "Client\ilwis.h"
#include "Client\FormElements\flddom.h"
#include "Headers\Hs\DOMAIN.hs"
#include "Engine\Domain\Dmvalue.h"
#include "Engine\Base\System\RegistrySettings.h"

int FieldDomainC::CreateDomain(void *)
{
	Domain dm;
	DomainInfo dminfo;
	try {
		if (fld->sName() != "")
		{
			dm = Domain(fld->sName());
			dminfo = DomainInfo(dm);
			if (dm->pdi())
			{
				RangeInt ri = dm->pdi()->riMinMax();
				dminfo.SetValueRange(ValueRangeInt(ri.iLo(), ri.iHi()));
			}
		}
	}
	catch (const ErrorObject&) {
		dm = Domain();
		dminfo = DomainInfo();
	}
	if (m_vr.fValid())
		dminfo.SetValueRange(m_vr);  // use value range specified by user
	                               // this may not be the same as the vr of the domain!

	sNewName = "";
	bool fOK;
	if (dm.fValid())
	{
		FormCreateDomain form(frm()->wnd(), &sNewName, dminfo, dmTypes);
		fOK = form.fOkClicked();
	}
	else
	{
		FormCreateDomain form(frm()->wnd(), &sNewName, dmTypes);
		fOK = form.fOkClicked();
	}

	if (fOK) 
	{
		FileName fn(sNewName);
		Domain dm(fn);
		if (!dm.fValid())
			return 0;

		FillDir();

		if (dm->pdv()) 
			dm->Updated();
		else 
		{
			String s = "Show ";
			s &= fn.sFullPathQuoted();
			IlwWinApp()->Execute(s);
		}
		SetVal(String("%S.dom",fn.sFile));
		CallCallBacks();
	}
	return 0;
}

void FieldDomainC::SetVal(const String& sDomain)
{
	fld->SetVal(sDomain);
}

void FieldDomainC::SetValueRange(const ValueRange& vr)
{
	m_vr = vr;
}

// FormCreateDomain implementation
FormCreateDomain::FormCreateDomain(CWnd* wPar, String* sDom, long types)
	: FormWithDest(wPar, SDMTitleCreateDomain)
	, sDomain(sDom)
	, dmTypes(types)
	, wParent(wPar)
	, fDomainInfo(false)
	, fiNrItems(0)
	, useDate(true),
	timeFormat(0)

{
	if (dmTypes == 0)
		dmTypes = dmCLASS | dmIDENT | dmVALUE | dmBOOL | dmTIME;

	sNewName = *sDomain;
	dmType type = dmCLASS;
	iWidth = 15;
	Domain dm;
	vr = ValueRange(0, 100, 0.01);
	if (sDomain->length() > 0) 
	{
		try 
		{
			dm = Domain(*sDomain);
			if (dm.fValid())
				if (dm->pdc())         type = dmCLASS;
				else if (dm->pdid())   type = dmIDENT;
				else if (dm->pdbool()) type = dmBOOL;
				else if ( dm->pdtime()) type = dmTIME;
				else if (dm->pdv()) 
				{
					type = dmVALUE;
					vr = ValueRange(dm->pdv()->rrMinMax(), dm->pdv()->rStep());
					iWidth = vr->iWidth();
				}
		}
		catch (ErrorObject& ) {}
	}  
	iNr = 0;
	init(type);
}

FormCreateDomain::FormCreateDomain(CWnd* wPar, String* sDom, const DomainInfo& dminf, long types) 
	: FormWithDest(wPar, SDMTitleCreateDomain)
	, sDomain(sDom)
	, dmTypes(types)
	, wParent(wPar)
	, fDomainInfo(true)
	, dmInf(dminf)
	, fiNrItems(0)
	, rgTime(0)
{
	if (dmTypes == 0)
		dmTypes = dmCLASS | dmIDENT | dmVALUE | dmBOOL;

	sNewName = *sDomain;
	iNr = dmInf.iNr();
	vr = dmInf.vr();
	iWidth = 15;

	dmType type;
	switch (dmInf.dmt())
	{
		case dmtCLASS: type = dmCLASS; break;
		case dmtGROUP: type = dmGROUP; break;
		case dmtID:    type = dmIDENT; break;
		case dmtBOOL:  type = dmBOOL;  break;
		case dmtVALUE: type = dmVALUE; break;
		case dmtTIME: type = dmTIME; break;
		default:
			type = dmVALUE; break;
	}
	init(type);
}

void FormCreateDomain::init(dmType type)
{
	iImg = IlwWinApp()->iImage(".dom");

	fdm = new FieldDataTypeCreate(root, SDMUiDomName, &sNewName, ".dom", true);
	fdm->SetIndependentPos();
	fdm->SetCallBack((NotifyProc)&FormCreateDomain::CallBackName);

	iRgVal = 0;  // default: first choice, to prevent crashes
	rg = new RadioGroup(root, SDMUiType, &iRgVal);
	rg->SetCallBack((NotifyProc)&FormCreateDomain::CallBackVrrChange); 

	int iRg = 0;
	if ((dmCLASS & dmTypes) || dmGROUP & dmTypes) 
	{
		if ((dmCLASS == type) || dmGROUP == type)
			iRgVal = iRg;
		dmt[iRg++] = dmCLASS; 
		RadioButton* rb = new RadioButton(rg, SDMUiClass);
		fDomainGroup = (dmGROUP == type) && (dmGROUP & dmTypes) && !(dmCLASS & dmTypes);
		new CheckBox(rb, SDMUiGroup, &fDomainGroup);
	}  
	RadioButton* rbId = 0;
	FieldGroup* fgId = 0;
	if (dmIDENT & dmTypes) 
	{
		if (dmIDENT == type)
			iRgVal = iRg;
		dmt[iRg++] = dmIDENT; 
		rbId = new RadioButton(rg, SDMUiIdentifier);
		fgId = new FieldGroup(rbId);
	}

	if (dmBOOL & dmTypes)
	{
		if (type == dmBOOL)
			iRgVal = iRg;
		dmt[iRg++] = dmBOOL;
		new RadioButton(rg, SDMUiBool);
	}
	FieldGroup *fgValue = 0;
	if (dmVALUE & dmTypes) 
	{
		if (dmVALUE == type)
			iRgVal = iRg;
		dmt[iRg++] = dmVALUE; 
		RadioButton* rbValue = new RadioButton(rg, SDMUiValue);
		fgValue = new FieldGroup(rbValue);
		fValueInt = false;
	}  

	if (fgId) 
	{
		fiNrItems = new FieldInt(fgId, SDMUiNrItems, &iNr, ValueRange(0, 9999999));
		fiNrItems->Align(rbId, AL_AFTER);
		sPrefix = "nr";
		fsPrefix = new FieldString(fgId, SDMUiPrefix, &sPrefix, Domain(), false);
		fsPrefix->Align(fiNrItems, AL_UNDER);
		fiNrItems->SetCallBack((NotifyProc)&FormCreateDomain::CallBackNrItemsChange); 
		fsPrefix->SetCallBack((NotifyProc)&FormCreateDomain::CallBackPrefixChange); 
	}  

	if (fgValue)
	{
		fvr = new FieldValueRange(fgValue, SDMUiMinMax, &vr, 0);
		fvr->SetCallBack((NotifyProc)&FormCreateDomain::CallBackVrrChange); 
		if (rbId)
			fvr->Align(rbId, AL_AFTER);
	}

	// Width field
	RadioButton *rbTime = 0;
	if ( dmTIME & dmTypes) {
		mode = ILWIS::Time::mDATETIME;
		if ( type == dmTIME)
			iRgVal = iRg;
		dmt[iRg++] = dmTIME;
		rbTime = new RadioButton(rg, "Time");
		rgTime = new RadioGroup(rbTime, "",&timeFormat, true);
		rgTime->SetIndependentPos();
		rgTime->Align(rbTime, AL_AFTER);
		RadioButton * rbDateTime = new RadioButton(rgTime,"Date & Time");
		RadioButton * rbDateOnly = new RadioButton(rgTime,"Date only");
		RadioButton * rbTimeOnly = new RadioButton(rgTime,"Day Time only");
		rgTime->SetCallBack((NotifyProc)&FormCreateDomain::SetUseDate, this);
		interval = ILWIS::TimeInterval(ILWIS::Time("*"),ILWIS::Time("*"),ILWIS::Duration(""));
		fInterval =  new FieldTimeInterval(rbTime, "", &interval,0,&mode);
		fInterval->Align(rgTime, AL_UNDER);
		FieldBlank *fb = new FieldBlank(root);
		fb->Align(rbTime, AL_UNDER, 20);
	}



	fiWidth = new FieldInt(root, SDMUiWidth, &iWidth);
	//fiWidth->Align(fb, AL_UNDER);

	// Description field
	StaticText* st = new StaticText(root, SDMUiDescription);
	st->psn->SetBound(0,0,0,0);
	st->Align(fiWidth, AL_UNDER);
	FieldString* fs = new FieldString(root, "", &sDescr);
	fs->SetWidth(175);
	fs->SetIndependentPos();
	fs->Align(st, AL_UNDER);

	// Remark field
	String sFill('x', 50);      // some text to set the width of the remark field
	stRemark = new StaticText(root, sFill);
	stRemark->SetVal(String()); // clear the remark field
	stRemark->Align(fs, AL_UNDER);
	stRemark->SetIndependentPos();

	SetMenHelpTopic("ilwismen\\create_a_domain.htm");
	create();
}

int FormCreateDomain::SetUseDate(Event *ev) {
	if ( rgTime) {
		rgTime->StoreData();
		if ( timeFormat == 0)
			fInterval->setMode(ILWIS::Time::mDATETIME);
		else if ( timeFormat == 1)
			fInterval->setMode(ILWIS::Time::mDATE);
		else if ( timeFormat == 2) {
			fInterval->setMode(ILWIS::Time::mTIME);
		}
	}
	return 1;
}
FormCreateDomain::~FormCreateDomain()
{
}

int FormCreateDomain::exec()
{
	try
	{
		FormWithDest::exec();
		*sDomain = sNewName;
		FileName fn(*sDomain, ".dom");
		Domain dm;
		switch (dmt[iRgVal]) 
		{
			case dmCLASS:
				if (fDomainGroup)
					dm = Domain(fn, 0, dmtGROUP);
				else
					dm = Domain(fn, 0, dmtCLASS);
				dm->SetWidth(iWidth);
				break;
			case dmIDENT: 
				dm = Domain(fn, iNr, dmtID, iNr == 0 ? String() : sPrefix);
				dm->SetWidth(iWidth);
				break;
			case dmBOOL:
				dm = Domain(fn, "False", "True");
				dm->SetWidth(iWidth);
				break;
			case dmTIME:
				{
				if ( timeFormat == 0) mode = ILWIS::Time::mDATETIME;
				else if ( timeFormat == 0) mode = ILWIS::Time::mDATE;
				else if ( timeFormat == 0) mode = ILWIS::Time::mTIME;
				dm = Domain(fn,interval,mode);
				}
				break;
			case dmVALUE:
				{
					dm = Domain(fn, vr->rrMinMax().rLo(), vr->rrMinMax().rHi(), vr->rStep());
					dvr = dm->pdvr();
					if (dvr)
						dvr->SetWidth(iWidth);
					else
						dm = Domain(0, 100, 0.01);

					IlwisSettings settings("DefaultSettings");
					String sDefaultRpr = settings.sValue("value", "");
					FileName fnRpr;
					if (sDefaultRpr.length() > 0 && File::fExist(sDefaultRpr))
						fnRpr = FileName(sDefaultRpr);
					else
					{
						FileName fn = IlwisObjectPtr::fnCheckPath(FileName("value.dom"));
						ObjectInfo::ReadElement("Domain", "Representation", fn, fnRpr);
					}

					dm->SetRepresentation(fnRpr);
					break;
				}
		}
		if (sDescr.length() > 0)
			dm->sDescription = sDescr;
		dm->Store();
	}
	catch (ErrorObject& err) 
	{
		_fOkClicked = false;
		err.Show();
	}
	return 0;
}

int FormCreateDomain::CallBackNrItemsChange(Event *) 
{
	fiNrItems->StoreData();
	if (iNr > 0)
		fsPrefix->Show();
	else
		fsPrefix->Hide();

	return 0;
}

int FormCreateDomain::CallBackVrrChange(Event *ev) 
{
	int iRgV = rg->iVal();
	switch (dmt[iRgV]) 
	{
		case dmVALUE: 
			fvr->StoreData();
			if (vr.fValid())
				fiWidth->SetVal(vr->iWidth());
			break;
		case dmBOOL :
			fiWidth->SetVal(6);
			break;
		case dmIDENT:
			CallBackNrItemsChange(ev);  // and let it fall through
		default:
			fiWidth->SetVal(iWidth);
	}
	
	return 0;
}

int FormCreateDomain::CallBackPrefixChange(Event *) 
{
	fsPrefix->StoreData();
	fPrefixOk = sPrefix.length() > 0;

	if (fPrefixOk)
		stRemark->SetVal("");
	else
		stRemark->SetVal(SDMRemNoEmptyPrefixAllowed);

	if (fPrefixOk && fVrOk)
		EnableOK();
	else
		DisableOK();

	return 0;
}

int FormCreateDomain::CallBackName(Event*)
{
	fdm->StoreData();
	FileName fn(sNewName, ".dom");
	fVrOk = false;
	if (!fn.fValid())
		stRemark->SetVal(SDMRemNotValidDomainName);
	else if(File::fExist(fn))   
		stRemark->SetVal(SDMRemDomExists);
	else
	{
		FileName fnSys = IlwisObjectPtr::fnCheckPath(fn);
		if (ObjectInfo::fSystemObject(fnSys))
			stRemark->SetVal(String("%S %S", SDMRemDomExists, SDMRemAsSystemDomain));
		else
		{
			fVrOk = true;  
			stRemark->SetVal(String());
		}
	}
	
	if (fiNrItems)
		fiNrItems->StoreData();
	bool fOk = (iNr <= 0) || (iNr > 0 && fPrefixOk);
	if (fOk && fVrOk)
		EnableOK();
	else    
		DisableOK();
	return 0;
}
