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

 Created on: 2014-05-02
 ***************************************************************/

#include "Client\Headers\AppFormsPCH.h"
#include "Client\FormElements\frmgenap.h"
#include "FormProbabilityDensity.h"
#include "Client\FormElements\fldcol.h"
#include "Client\ilwis.h"

LRESULT Cmdfrmprobabilitydensity(CWnd *wnd, const String& s)
{
	new FormProbabilityDensity(wnd, s.c_str());
	return -1;
}

FormProbabilityDensity::FormProbabilityDensity(CWnd* mw, const char* sPar)
: FormGeneralApplication(mw, TR("Create Probability Density Maps From Point Map"))
{
	if (sPar) {
		TextInput inp(sPar);
		TokenizerBase tokenizer(&inp);
		String sVal;
		for (;;) {
			Token tok = tokenizer.tokGet();
			sVal = tok.sVal();
			if (sVal == "")
				break;
			FileName fn(sVal);
			if (fn.sExt == ".mpp" || fn.sExt == "") {
				if (sPointMap == "")
					sPointMap = fn.sFullNameQuoted(true);
			} else if (fn.sExt == ".ioc" || fn.sExt == "") {
				if (sOutCollection == "")
					sOutCollection = fn.sFullName(false);
			}
		}
	}
	fldPointMap = new FieldPointMap(root, TR("Input Point Map"), &sPointMap, new MapListerDomainType(".mpp", dmCLASS | dmIDENT | dmGROUP | dmBOOL | dmUNIQUEID));
	fldPointMap->SetCallBack((NotifyProc)&FormProbabilityDensity::PointMapCallBack);
	fcDistance = new FieldColumn(root,TR("Distance (meters)"), Table(), &distanceAttr, dmVALUE);
	fcDistance->SetCallBack((NotifyProc)&FormProbabilityDensity::ColCallBack);
	fcSigmaDistance = new FieldColumn(root,TR("Sigma Distance (meters)"), Table(), &sigmaDistanceAttr, dmVALUE);
	fcSigmaDistance->SetCallBack((NotifyProc)&FormProbabilityDensity::ColCallBack);
	fcDirection = new FieldColumn(root,TR("Direction (radians)"), Table(), &directionAttr, dmVALUE);
	fcDirection->SetCallBack((NotifyProc)&FormProbabilityDensity::ColCallBack);
	fcSigmaDirection = new FieldColumn(root,TR("Sigma Direction (radians)"), Table(), &sigmaDirectionAttr, dmVALUE);
	fcSigmaDirection->SetCallBack((NotifyProc)&FormProbabilityDensity::ColCallBack);
	outCollection = new FieldDataTypeCreate(root, TR("&Output Collection"), &sOutCollection, ".ioc", false);
	outCollection->SetCallBack((NotifyProc)&FormProbabilityDensity::OutCollectionCallBack);
	StaticText* st = new StaticText(root, TR("&Description:"));
	st->Align(outCollection, AL_UNDER);
	st->psn->SetBound(0,0,0,0);
	FieldString* fs = new FieldString(root, "", &sDescr);
	fs->SetWidth(120);
	fs->SetIndependentPos();
	create();
}

int FormProbabilityDensity::PointMapCallBack(Event*)
{
	fldPointMap->StoreData();
	fcDistance->FillWithColumns((TablePtr*)0);
	fcSigmaDistance->FillWithColumns((TablePtr*)0);
	fcDirection->FillWithColumns((TablePtr*)0);
	fcSigmaDirection->FillWithColumns((TablePtr*)0);
	try
	{
		if (sPointMap.length() == 0)
			return 0;

		FileName fnMap(sPointMap); 
		PointMap map(fnMap);
		if (map.fValid())
		{
			if (map->fTblAtt()) {
				fcDistance->FillWithColumns(map->tblAtt().ptr());
				fcSigmaDistance->FillWithColumns(map->tblAtt().ptr());
				fcDirection->FillWithColumns(map->tblAtt().ptr());
				fcSigmaDirection->FillWithColumns(map->tblAtt().ptr());
			}
		}  
	}
	catch (ErrorObject&) {}

	return 0;
}

int FormProbabilityDensity::ColCallBack(Event*)
{
	fcDistance->StoreData();
	fcSigmaDistance->StoreData();
	fcDirection->StoreData();
	fcSigmaDirection->StoreData();
	return 0;
}

int FormProbabilityDensity::OutCollectionCallBack(Event*)
{
	outCollection->StoreData();
	// later check on existance.  
	return 0;
}

int FormProbabilityDensity::exec() 
{
	FormGeneralApplication::exec();
	FileName fn(sOutCollection, ".ioc");
	FileName fnMap(sPointMap, ".mpp"); 
	sPointMap = fnMap.sRelativeQuoted(true, fn.sPath());
	String sExpr = String("ProbabilityDensityFromPoints(%S,%S,%S,%S,%S)", sPointMap, distanceAttr, sigmaDistanceAttr, directionAttr, sigmaDirectionAttr);

	if (fn.fExist()) {
		String sErr(TR("File %S already exists.\nOverwrite?").c_str(), fn.sFullPath(true));
		int iRet=par->MessageBox(sErr.c_str(), TR("File already exists").c_str(), MB_YESNO|MB_ICONEXCLAMATION);
		if (iRet != IDYES)
			return 0;
	}
	String sCmd = fn.sShortNameQuoted();
	sCmd &= " = ";
	sCmd &= sExpr;

	SetCommand(sCmd);
	String sExec;
	try {
		ObjectCollection ocOut(fn, sExpr);
		if (!ocOut.fValid())
			return 0;
		if (sDescr[0])
			ocOut->sDescription = sDescr;
		ocOut->Store(); // destructor of ocOut could be called later than opening map
		openMap(ocOut);
	}
	catch (ErrorObject& err) {
		err.Show();
		return 0;
	}  

	return 0;
}