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

#include "Client\Headers\AppFormsPCH.h"
#include "Engine\base\system\engine.h"
#include "Client\Forms\frmmapcr.h"
#include "Client\ilwis.h"
#include "Client\FormElements\flddom.h"

FormMapCreate::FormMapCreate(CWnd* mwin, const String& sTitle)
: FormGeneralApplication(mwin, sTitle), fgr(0), fdc(0), fvr(0),
mw(mwin), 
vr(0,100,1),
stRemark(0),
fOutMapList(false)
{
	int iImg = IlwWinApp()->iImage("ExeMap16Ico");
	if (!fModal()) {
		HICON hIco = IlwWinApp()->ilSmall.ExtractIcon(iImg);
		SetIcon(hIco,FALSE);
	}
}                    


void FormMapCreate::initMapOut(bool fAskGeoRef, bool fAskDomain)
{
	initMapOut((long)(fAskGeoRef ? 0xFFFFFFFF : 0L), (long)(fAskDomain ? 0xFFFFFFFF : 0L));
}

void FormMapCreate::initMapOut(bool fAskGeoRef, long dmTypes)
{
	initMapOut((long)(fAskGeoRef ? 0xFFFFFFFF : 0L), dmTypes);
}

void FormMapCreate::initMapOutGeoRef3D()
{
	fmc = new FieldMapCreate(root, TR("&Output Raster Map"), &sOutMap);
	fmc->SetCallBack((NotifyProc)&FormMapCreate::OutMapCallBack);
	FormEntry* fe = fmc;
	fe = new FieldGeoRef3DC(fmc, TR("&GeoReference"), &sGeoRef);
	fe->Align(fmc, AL_UNDER);
	StaticText* st = new StaticText(root, TR("&Description:"));
	st->Align(fe, AL_UNDER);
	st->psn->SetBound(0,0,0,0);
	FieldString* fs = new FieldString(root, "", &sDescr);
	fs->SetWidth(120);
	fs->SetIndependentPos();
}

void FormMapCreate::initMapOut(long grTypes, long dmTypes)
{
	if (fOutMapList)
		fmc = new FieldMapCreate(root, "&Output MapList", &sOutMap);
	else
		fmc = new FieldMapCreate(root, TR("&Output Raster Map"), &sOutMap);
	fmc->SetCallBack((NotifyProc)&FormMapCreate::OutMapCallBack);
	FormEntry* fe = fmc;
	if (0 != grTypes) {
		if (-1 == grTypes)
			grTypes = grALL | grNONONE | grNO3D;
		fgr = new FieldGeoRefC(root, TR("&GeoReference"), &sGeoRef, grTypes);  
		fgr->SetCallBack((NotifyProc)&FormMapCreate::ChangeGeoRef);
		fgr->Align(fmc, AL_UNDER);
		String s('X', 50);
		stGRDesc = new StaticText(root, s);
		stGRDesc->SetVal(String());
		stGRDesc->SetIndependentPos();

		fe = stGRDesc;
	}  
	if (0 != dmTypes) {
		if (-1 == dmTypes)
			dmTypes = dmCLASS|dmIDENT|dmVALUE|dmIMAGE|dmPICT|dmCOLOR|dmBOOL|dmBIT;
		fdc = new FieldDomainC(root, TR("&Domain"), &sDomain, dmTypes);
		fdc->Align(fe, AL_UNDER);
		fe = fdc;
		if (dmTypes & dmVALUE) {
			fdc->SetCallBack((NotifyProc)&FormMapCreate::DomainCallBack);
			fvr = new FieldValueRange(root, TR("Value &Range"), &vr, fdc);
			fvr->SetCallBack((NotifyProc)&FormMapCreate::ValueRangeCallBack);
			fvr->Align(fdc, AL_UNDER);
			fe = fvr;
		}
	}  
	StaticText* st = new StaticText(root, TR("&Description:"));
	st->Align(fe, AL_UNDER);
	st->psn->SetBound(0,0,0,0);
	FieldString* fs = new FieldString(root, "", &sDescr);
	fs->SetWidth(120);
	fs->SetIndependentPos();
	if (dmTypes & dmVALUE)
		initRemark();
}

void FormMapCreate::initMapOutValRange(bool fAskGeoRef)
{
	fmc = new FieldMapCreate(root, TR("&Output Raster Map"), &sOutMap);
	fmc->SetCallBack((NotifyProc)&FormMapCreate::OutMapCallBack);
	if (fAskGeoRef) {
		fgr = new FieldGeoRefC(root, TR("&GeoReference"), &sGeoRef);  
		fgr->SetCallBack((NotifyProc)&FormMapCreate::ChangeGeoRef);
		fgr->Align(fmc, AL_UNDER);
		String s('X', 50);
		stGRDesc = new StaticText(root, s);
		stGRDesc->SetVal(String());
		stGRDesc->SetIndependentPos();
	}  
	else {
		FieldBlank* fb = new FieldBlank(root, 0);
		fb->Align(fmc, AL_UNDER);
	}
	fvr = new FieldValueRange(root, TR("Value &Range"), &vr, 0);
	fvr->SetCallBack((NotifyProc)&FormMapCreate::ValueRangeCallBack);
	StaticText* st = new StaticText(root, TR("&Description:"));
	st->psn->SetBound(0,0,0,0);
	FieldString* fs = new FieldString(root, "", &sDescr);
	fs->SetWidth(120);
	fs->SetIndependentPos();
	initRemark();
}

int FormMapCreate::OutMapCallBack(Event*)
{
	fmc->StoreData();
	// later check on existance etc.  
	return 0;
}

int FormMapCreate::ChangeGeoRef(Event*)
{
	fgr->StoreData();
	if (sGeoRef.length() > 0)
	{
		m_gr = GeoRef(sGeoRef);
		stGRDesc->SetVal(m_gr->sGetDescription());
	}

	return 0;
}

int FormMapCreate::DomainCallBack(Event*)   
{
	fdc->StoreData();
	if (sDomain.length() == 0) {
		if (fvr)
			fvr->Hide();
		SetRemarkOnBytesPerPixel();
		return 0;
	}  
	FileName fn(sDomain);
	Domain dm;
	try { 
		dm = Domain(fn); 
		if (dm.fValid() && fvr)
			fvr->DomainCallBack(0);
	}    
	catch (const ErrorObject&) {
		SetRemarkOnBytesPerPixel();
		return 0;
	}
	SetRemarkOnBytesPerPixel();
	return 0;
}

int FormMapCreate::ValueRangeCallBack(Event*)
{
	if (0 == fvr)
		return 0;
	fvr->StoreData();
	if (fdc != 0)
		fdc->SetValueRange(vr);
	SetRemarkOnBytesPerPixel();
	return 0;
}

void FormMapCreate::SetRemarkOnBytesPerPixel()
{
	if (0 == stRemark)
		return;
	if (sDomain.length() == 0) {
		stRemark->SetVal("");
		return;
	}  
	FileName fn(sDomain);
	try { 
		Domain dm(fn);
		DomainValueRangeStruct dvrs(dm, vr);
		String s;
		StoreType st = dvrs.st();
		int iNr;
		if (st < stBYTE) {
			if (st == stBIT)
				s = TR("1 bit");
			else {  
				switch (st) {
		  case stDUET: iNr = 2; break;
		  case stNIBBLE: iNr = 4; break;
				}
				s = String(TR("%i bits").c_str(), iNr);
			}  
		}
		else {
			if (st == stBYTE)
				s = TR("1 byte");
			else {  
				switch (st) {
		  case stINT: iNr = 2; break;
		  case stLONG: iNr = 4; break;
		  case stREAL: iNr = 8; break;
		  case stCRD: iNr = 16; break;
		  default:
			  stRemark->SetVal(TR("Invalid Domain"));
			  return;
				}
				s = String(TR("%i bytes").c_str(), iNr);
			}  
		}
		String sRemark(TR("Map will use %S per pixel").c_str(), s);
		stRemark->SetVal(sRemark);
	}
	catch (ErrorObject&) 
	{
		stRemark->SetVal(TR("Invalid Domain"));
	}  
}

int FormMapCreate::exec()
{
	FormGeneralApplication::exec();
	FileName fn(sOutMap);
	FileName fnDomain(sDomain); 
	sDomain = fnDomain.sRelativeQuoted(true,fn.sPath());
	FileName fnGeoRef(sGeoRef); 
	sGeoRef = fnGeoRef.sRelativeQuoted(true,fn.sPath());
	fOutMapList = false;
	return 1;
}

void FormMapCreate::execMapOut(const String& sExpr)
{
	FileName fn(sOutMap);
	fn.sExt = ".mpr";
	if (fOutMapList)
		fn.sExt = ".mpl";
	if (fn.fExist()) {
		String sErr(TR("File %S already exists.\nOverwrite?").c_str(), fn.sFullPath(true));
		int iRet=mw->MessageBox(sErr.c_str(), TR("File already exists").c_str(), MB_YESNO|MB_ICONEXCLAMATION);
		if (iRet != IDYES)
			return;
	}
	String sCmd = fn.sShortNameQuoted();
	DomainValueRangeStruct dvs;
	if (sDomain[0]) {
		try {
			FileName fnDomain(sDomain, ".dom", false);
			Domain dm(fnDomain);
			dvs = DomainValueRangeStruct(dm, vr);
		}
		catch (ErrorObject& err) {
			err.Show();
		}
		if (!fOutMapList)
			if (dvs.vr().fValid())
				sCmd &= String("{dom=%S;vr=%S}", sDomain, dvs.vr()->sRange());
			else
				sCmd &= String("{dom=%S}", sDomain);
	}
	sCmd &= " = ";
	sCmd &= sExpr;

	SetCommand(sCmd);
	String sExec;
	try {
		if (!fOutMapList)
		{
			Map mpOut(fn, sExpr);
			if (!mpOut.fValid())
				return;
			if (sDomain[0]) {
				Domain dm = dvs.dm();
				mpOut->fMergeDomainForCalc(dm, sExpr);
				mpOut->SetDomainValueRangeStruct(dvs);
			}
			if (sDescr[0])
				mpOut->sDescription = sDescr;
			mpOut->Store(); // destructor of mpOut could be called later than opening map
			openMap(mpOut);
		}
		else 
		{
			MapList mplOut(fn, sExpr);
			if (!mplOut.fValid())
				return;
			mplOut->SetDomainValueRangeStruct(dvs);
			if (sDescr[0])
				mplOut->sDescription = sDescr;
			mplOut->Store(); // destructor of mplOut could be called later than opening map
			openMap(mplOut);
		}
	}
	catch (ErrorObject& err) {
		err.Show();
		return;
	}  
}

void FormMapCreate::SetDefaultValueRange(const ValueRange& vr)
{
	if (vr.fValid() && fvr)
		fvr->SetVal(vr);
}

void FormMapCreate::initRemark()
{
	if (0 != stRemark)
		return;
	String sFill('x', 60);
	stRemark = new StaticText(root, sFill);
	stRemark->SetIndependentPos();
}
