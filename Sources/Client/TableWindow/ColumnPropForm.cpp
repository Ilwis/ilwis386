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
/* ColumnManageForm, ColumnSelector
   by Wim Koolhoven, may 1996
  (c) Ilwis System Development ITC
	Last change:  WK   14 Jul 97    9:02 am
*/ 

#include "Client\Headers\formelementspch.h"
#include "Client\ilwis.h"
#include "Engine\Table\tblview.h"
#include "Client\FormElements\fentmisc.h"
#include "Client\FormElements\fentvalr.h"
#include "Client\FormElements\flddom.h"
#include "Client\FormElements\FieldObjShow.h"
#include "Client\TableWindow\ColumnPropForm.h"
#include "Engine\Domain\DomainTime.h"
#include "Headers\Hs\Table.hs"
#include "Headers\Hs\Mainwind.hs"
#include "Headers\Hs\DOMAIN.hs"

ColumnPropForm::ColumnPropForm(CWnd* parent, ColumnView& cv,
							   int* iPos, bool fNew)
							   : FormWithDest(parent, TR("Column properties")), _cv(cv), vr(0,100,1),
							   stRemark(0), fiDec(0), fvr(0)
{
	if (!cv.fValid())
		return;
	fsExpr = 0;
	stMinMax = 0;
	String s = cv->sTypeName();
	StaticText *st = new StaticText(root,s);

	st->SetIndependentPos();
	s = cv->objtime.sDateTime();
	st = new StaticText(root,s);
	st->SetIndependentPos();

	fReadOnly = cv->fReadOnly();
	fWasReadOnly = fReadOnly;
	if (cv->fOwnedByTable()) {
		st->psn->SetBound(0,0,0,0);
		st = new StaticText(root, TR("Column is table-owned"));
		st->SetIndependentPos();
		if (fReadOnly) {
			st->psn->SetBound(0,0,0,0);
			st = new StaticText(root, TR("Column is read only"));
			st->SetIndependentPos();
		}
		fReadOnly = true;
	}
	else {
		new CheckBox(root, TR("&Read Only"), &fReadOnly);
	}

	fSetDescFromExpr = false;
	// Expression
	if (cv->fDependent()) {
		sExpr = cv->sExpression();
		if (fReadOnly || !cv->fExpressionChangeable()) {
			st = new StaticText(root, sExpr);
			st->SetIndependentPos();
		}
		else {
			fSetDescFromExpr = sExpr == cv->sDescription;
			st = new StaticText(root,TR("&Expression"));
			st->psn->SetBound(0,0,0,0);
			st->SetIndependentPos();
			fsExpr = new FieldString(root, "", &sExpr);
			fsExpr->SetWidth(120);
			fsExpr->SetIndependentPos();
		}
	}
	// Domain
	if (fReadOnly) {
		new FieldObjShow(root, cv->dm());
		if (cv->vr().fValid() && 0 == cv->dm()->pdbool()) {
			st->psn->SetBound(0,0,0,0);
			String s("%S: %S", TR("Value Range"), cv->vr()->sRange());
			st = new StaticText(root, s);
			st->SetIndependentPos();
		}  
	}
	else {
		sDomain = cv->dm()->sName(true);
		fd = new FieldDomainC(root, TR("&Domain"), &sDomain, 0xffff, true); // also internal domains
		fd->SetCallBack((NotifyProc)&ColumnPropForm::CallBackDomainChange);
		if (0 != fsExpr) {
			PushButton* pb = new PushButton(root, TR("Defaults"),
				(NotifyProc)&ColumnPropForm::SetDefaults);
			pb->SetIndependentPos();
			pb->Align(fd, AL_AFTER);
		}
		String sFill('x', 50);
		stRemark = new StaticText(root, sFill);
		stRemark->Align(fd, AL_UNDER);
		stRemark->SetIndependentPos();
		if (cv->vr().fValid() && !cv->dm()->pdnone())
			vr = cv->vr();
		if ( cv->dm()->pdtime()) {
			FieldGroup *fgTime = new FieldGroup(root,true);
			DomainTime *dt = cv->dm()->pdtime();
			begin = dt->getInterval().getBegin();
			end = dt->getInterval().getEnd();
			duration = 	dt->getInterval().getStep();
			ft1 = new FieldTime(fgTime,TR("Value &Range"),&begin,dt);
			ft2 = new FieldTime(fgTime,"",&end,dt);
			ft2->Align(ft1, AL_AFTER);
			FieldBlank* fb = new FieldBlank(fgTime);
			fb->Align(ft1, AL_UNDER);

		}
		else {
			fvr = new FieldValueRange(root, TR("Value &Range"), &vr, fd);
			fvr->Align(stRemark, AL_UNDER);
			fvr->SetCallBack((NotifyProc)&ColumnPropForm::ValueRangeCallBack);
			FieldBlank* fb = new FieldBlank(root);
			fb->Align(fvr, AL_UNDER);
		}
	}
	// MinMax
	if (!fNew && cv->dm()->pdv() && !cv->dm()->pdbool()) {
		if (cv->dm()->pdvr()) {
			RangeReal rr = cv->rrMinMax();
			if (rr.rHi() >= rr.rLo()) {
				s = String(TR("Minimum: %S  Maximum: %S").c_str(), cv->dvrs().sValue(rr.rLo()), cv->dvrs().sValue(rr.rHi()));
				stMinMax = new StaticText(root, s);
				stMinMax->SetIndependentPos();
			}  
		}
		else {
			RangeInt ri = cv->riMinMax();
			if (ri.iHi() >= ri.iLo()) {
				s = String(TR("Minimum: %li  Maximum: %li").c_str(), ri.iLo(), ri.iHi());
				stMinMax = new StaticText(root, s);
				stMinMax->SetIndependentPos();
			}  
		}
	}
	// Depend Info
	bool fUpToDate = true;
	if (!fNew && cv->fDependent()) {
		Array<String> as;
		cv->DependencyNames(as);
		if (as.iSize() > 0) {
			String s = TR("Depends on");
			s &= " ";
			unsigned int i;
			for (i = 0; i < as.iSize() - 1; ++i) {
				s &= as[i];
				s &= ", ";
				if (s.length() > 50) {
					StaticText* st = new StaticText(root, s);
					st->psn->SetBound(0,0,-1,0);
					st->SetIndependentPos();
					s = "";
				}
			}
			s &= as[i];
			s &= ".";
			StaticText* st = new StaticText(root, s);
			st->psn->SetBound(0,0,0,0);
			st->SetIndependentPos();
			ObjectTime timNewest = 0;
			String sObjName;
			cv->GetNewestDependentObject(sObjName, timNewest);
			fUpToDate = timNewest == 0;
			switch (timNewest) {
		case 0:
			st = new StaticText(root, TR("Object is up-to-date"));
			break;
		case -1:
			st = new StaticText(root, String("%S: %S", TR("Object is missing"), sObjName));
			break;
		default:
			st = new StaticText(root, String("%S: %S (%S)", TR("Object is not up-to-date"), sObjName, timNewest.sDateTime()));
			}
			st->SetIndependentPos();
		}
	}
	// ColumnView Info
	if (*iPos > 0)
		new FieldInt(root, TR("&Position"), iPos, ValueRange(1L,10000L));
	new FieldInt(root, TR("&Width"), &cv.iWidth, ValueRange(1L,100L));
	if (cv->dm()->pdv() && !cv->dm()->pdbool()) {
		if (fNew)
			cv.iDec = vr->iDec();
		fiDec = new FieldInt(root, TR("&Decimals"), &cv.iDec, ValueRange(0L,50L));
	}
	// Description
	st = new StaticText(root,TR("&Description:"));
	st->psn->SetBound(0,0,0,0);
	st->SetIndependentPos();
	if (fReadOnly) {
		st = new StaticText(root, cv->sDescription);
		st->SetIndependentPos();
	}
	else {
		fsDesc = new FieldString(root, "", &cv->sDescription);
		fsDesc->SetWidth(120);
		fsDesc->SetIndependentPos();
	}
	// Radical Changes
	if (!fReadOnly && !fNew && cv->dm()->pds()) {
		PushButton* pb = new PushButton(root, TR("Create new &Domain from Strings in column"),
			(NotifyProc)&ColumnPropForm::CreateDomain);
		pb->SetIndependentPos();
	}

	if (!fUpToDate) {
		PushButton* pb = new PushButton(root, TR("&Make Up to Date"),
			(NotifyProc)&ColumnPropForm::MakeUpToDate);
		pb->SetIndependentPos();
	}

	if (!fReadOnly && !fNew && cv->fDependent() /*&& !cv.fUsedInOpenColumns()*/) {
		PushButton* pb = new PushButton(root, TR("&Break Dependency Link"),
			(NotifyProc)&ColumnPropForm::BreakLink);
		pb->SetIndependentPos();
	}
	AdditionalInfoButton();

	SetMenHelpTopic("ilwismen\\column_properties.htm");
	create();
}

void ColumnPropForm::AdditionalInfoButton()
{
	if (_cv->fAdditionalInfo())
		new PushButton(root, TR("&Additional Info"), (NotifyProc)&ColumnPropForm::AdditionalInfo);
}

int ColumnPropForm::AdditionalInfo(Event*)
{
	class AdditionalInfoBox: public FormBaseDialog
	{
	public:
		AdditionalInfoBox(CWnd* wPar, const String& sTypeName, String* sInfo)
			: FormBaseDialog(wPar, TR("Additional Info"),
			fbsSHOWALWAYS|fbsMODAL|fbsNOCANCELBUTTON|fbsBUTTONSUNDER)
		{
			new StaticText(root, sTypeName);
			FieldStringMulti* fsm = new FieldStringMulti(root, sInfo, true);
			fsm->SetFixedFont();
			fsm->SetWidth(240);
			fsm->SetHeight(100);
			create();
		}
		int exec() {return 1;}
	};
	String sInfo = _cv->sAdditionalInfo();
	AdditionalInfoBox(this, _cv->sTypeName(), &sInfo);
	return 0;
}
FormEntry* ColumnPropForm::feDefaultFocus()
{
	if (0 != fvr) {
		if (_cv->vr().fValid() && 0 == _cv->dm()->pdbool())
			fvr->SetVal(_cv->vr());
	}
	return FormBase::feDefaultFocus();
}


int ColumnPropForm::CallBackDomainChange(Event*)
{
	fd->StoreData();
	if (0 == sDomain.length()) {
		stRemark->SetVal("");
		return 0;
	}
	FileName fn(sDomain, ".dom", false);
	Domain dm;
	try {
		dm = Domain(fn);
	}
	catch (ErrorObject& err) {
		err.Show(TR("Domain Change"));
		stRemark->SetVal("");
	}  
	if (dm.fValid()) {
		stRemark->SetVal(dm->sDescription);
		if ( fvr)
			fvr->DomainCallBack(0);
	}
	return 0;
}

int ColumnPropForm::ValueRangeCallBack(Event*)
{
	fvr->StoreData();
	int iDec = 0;
	if (vr.fValid())
		if (vr == _cv->vr())
			iDec = _cv.iDec;
		else
			iDec = vr->iDec();
	if (fiDec)
		fiDec->SetVal(iDec);
	return 0;
}

int ColumnPropForm::SetDefaults(Event*)
{
	if (0 != stMinMax)
		stMinMax->Hide();
	fsExpr->StoreData();
	try {
		DomainValueRangeStruct dvrs = _cv->dvrsDefault(sExpr);
		if (!dvrs.dm().fValid())
			return 0;
		sDomain = dvrs.dm()->fnObj.sFile;
		ValueRange vrh = dvrs.vr();
		fd->SetVal(sDomain);
		if (vrh.fValid() && 0 == dvrs.dm()->pdbool())
			fvr->SetVal(vrh);
	}
	catch (const ErrorObject& err) {
		err.Show();
	}
	if (fSetDescFromExpr)
		fsDesc->SetVal(sExpr);
	return 0;
}

int ColumnPropForm::BreakLink(Event*)
{
	TableView* tvw = _cv.tview();

	int iRes = MessageBox(TR("Break Dependency Link\nAre you sure?").c_str(), TR("Break Dependency Link").c_str(), MB_YESNO|MB_ICONQUESTION);
	if (IDYES == iRes) {
		tvw->VirtualToStore(_cv);
		OnCancel();  
	}  
	return 0;
}

int ColumnPropForm::MakeUpToDate(Event*)
{
	try {
		_cv->MakeUpToDate();
	}
	catch (ErrorObject& err) {
		err.Show();
	}
	OnCancel();
	return 0;
}

int ColumnPropForm::exec()
{
	FormWithDest::exec();
	try {
		if  (!fWasReadOnly && !_cv->fOwnedByTable()) {
			FileName fnDom(sDomain);
			Domain dm(fnDom);
			if (!_cv->fMergeDomainForCalc(dm, sExpr))
				return 0;
			if (_cv->fDependent()) {
				String sOld = _cv->sExpression();
				if (sOld != sExpr)
					_cv->Replace(sExpr);
			}
			DomainValueRangeStruct dvrs(dm, vr);
			if ( _cv->fDependent() && _cv->dvrs() != dvrs)
			{
				Table tbl(_cv->fnObj);
				String sName = _cv->sName();
				String sExpr = _cv->sExpression();
				const TableView *tview = _cv.tview();
				tbl->RemoveCol(_cv);
				Column col(tbl, sName, sExpr, dvrs);
				col->fErase = false;
				tbl->AddCol(col);
				tbl->Store();  // update ALL new column info, not only the col: section
				_cv = ColumnView(tview, col);
				tbl->Store();  // make sure ColumnView changes are stored
			}				
			if (!_cv->fDependent() &&
				(0 != dm->pdvi() || 0 != dm->pdvr() || 0 != _cv->dm()->pds())) {
					if (dm != _cv->dm() || vr != _cv->vr())
						_cv->fConvertTo(dvrs,Column());
			}
			else
				_cv->SetDomainValueRangeStruct(dvrs);
			_cv->MakeUsable();
			_cv->SetReadOnly(fReadOnly);
			_cv->Store();
			_cv.Store();
		}
		else if (fWasReadOnly && !fReadOnly) {
			_cv->SetReadOnly(fReadOnly);
			_cv->Store();
			_cv.Store();
		}
	}
	catch (ErrorObject& err) {
		err.Show();
	}
	return 0;
}

int ColumnPropForm::CreateDomain(Event*)
{
	class FormDomain: public FormWithDest 
	{
	public:
		FormDomain(CWnd* wPar, String* sDom, int* iType, bool* fApply)
			: FormWithDest(wPar, TR("Create Domain from String"))
		{
			sDomain = sDom;
			fdm = new FieldDataTypeCreate(root, TR("&Domain Name"), sDom, ".DOM", true);
			fdm->SetCallBack((NotifyProc)&FormDomain::CallBackName);
			RadioGroup* rg = new RadioGroup(root, "", iType);
			new RadioButton(rg, TR("&Class"));
			new RadioButton(rg, TR("&Identifier"));
			new CheckBox(root, TR("&Apply Domain to Column"), fApply);
			String sFill('*', 40);
			stRemark = new StaticText(root, sFill);
			stRemark->SetIndependentPos();
			SetMenHelpTopic("ilwismen\\create_domain_class_or_id_from_string_column.htm");
			create();
		}
	private:
		int CallBackName(Event*) {
			fdm->StoreData();
			FileName fn(*sDomain);
			fn.sExt = ".dom";
			bool fOk = false;
			if (!fn.fValid())
				stRemark->SetVal(TR("Not a valid domain name"));
			else if(File::fExist(fn))
				stRemark->SetVal(TR("Domain already exists"));
			else {
				fOk = true;
				stRemark->SetVal("");
			}
			if (fOk)
				EnableOK();
			else
				DisableOK();
			return 0;
		}
		FieldDataTypeCreate* fdm;
		StaticText* stRemark;
		String* sDomain;
	};
	String sDom = _cv->sName();
	int iType = 0;
	bool fApply = true;
	FormDomain form(this, &sDom, &iType, &fApply);
	if (form.fOkClicked()) {
		DomainType dmt;
		switch (iType) {
		case 0: dmt = dmtCLASS; break;
		case 1: dmt = dmtID; break;
		case 2: dmt = dmtVALUE; break;
		}
		FileName fn(sDom);
		CWaitCursor wc;
		Domain dm = _cv->dmFromStrings(fn, dmt);
		if (fApply) {
			_cv->fConvertTo(dm,Column());
			OnCancel(); // remove prop form from screen (info is not correct anymore)
		}
	}
	return 0;
}
