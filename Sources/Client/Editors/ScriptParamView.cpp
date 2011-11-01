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
// ScriptParamView.cpp: implementation of the ScriptParamView class.
//
//////////////////////////////////////////////////////////////////////

#pragma warning( disable : 4786 )

#include "Client\Headers\formelementspch.h"
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Client\Base\Res.h"
#include "Engine\Scripting\Script.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\Editors\TextDoc.h"
#include "Client\Editors\ScriptDoc.h"
#include "Client\Editors\ScriptParamView.h"
#include "Headers\Hs\Script.hs"
#include "Client\Editors\ScriptParamField.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(ScriptParamView, FormBaseView)

BEGIN_MESSAGE_MAP(ScriptParamView, FormBaseView)
END_MESSAGE_MAP()

ScriptParamView::ScriptParamView() :
iParams(iUNDEF)
{
	fbs |= fbsNOOKBUTTON | fbsNOCANCELBUTTON | fbsNOBEVEL;
	fbs |= fbsNOPARENTRESIZE;
}

ScriptParamView::~ScriptParamView()
{
}

ScriptDoc* ScriptParamView::GetDocument() 
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(ScriptDoc)));
	return dynamic_cast<ScriptDoc *>(m_pDocument);
}

void ScriptParamView::CreateForm()
{
	int i;
	Script scr = GetDocument()->scr();
	iParams = scr->iParams();
	bool fReadOnly = scr->fReadOnly();
	DWORD iStyle = ES_AUTOHSCROLL|WS_TABSTOP|WS_GROUP|WS_BORDER;
	if (fReadOnly)
		iStyle |= ES_READONLY;

	for (i = 0; i < iParams; ++i) {
		iParamType[i] = 0;
	//for (i = 0; i < iParams; ++i) {
		sQuestion[i] = scr->sParam(i);
		iParamType[i] = (long)scr->ptParam(i);
		fIncExt[i] = scr->fParamIncludeExtension(i);
	}

	//::MessageBox(0,"Hier","Ook", MB_OK);
	FieldGroup *fg1 = new FieldGroup(root);
	String txt = TR("&Number of Parameters");
	fiParam = new FieldInt(fg1, txt, &iParams, ValueRange(0,9), true);

	fiParam->SetReadOnly(fReadOnly);
	fiParam->SetCallBack((NotifyProc)&ScriptParamView::CallBack); 
	fiParam->SetIndependentPos();
	FormEntry* feLast = fiParam;

	fgHead = new FieldGroup(fg1);
	fgHead->Align(feLast, AL_UNDER);
	feLast = fgHead;

	FieldBlank* fb = new FieldBlank(fgHead);
	StaticText* st1 = new StaticText(fgHead, TR("Name"));
	st1->Align(fb, AL_AFTER);
	st1->SetCallBack((NotifyProc)&ScriptParamView::Init);
	st1->psn->iBndDown = 0;
	StaticText* st2 = new StaticText(fgHead, TR("Type"));
	st2->Align(st1, AL_AFTER);
	st2->psn->iBndDown = 0;

	for (i = 0; i < 9; ++i) 
	{
		fg[i] = new FieldGroup(root);
		fg[i]->Align(feLast, AL_UNDER);
		feLast = fg[i];
		String s(TR("Parameter %%&%d").c_str(), i+1);
		FieldString *fs = new FieldString(fg[i], s, &sQuestion[i], Domain(), true, iStyle);
		fs->SetCallBack((NotifyProc)&ScriptParamView::Changed);
		if (fReadOnly)
		{
			sValDummy[i] = sParamType(i);
			fe[i] = new FieldString(root, "", &sValDummy[i], Domain(), true ,iStyle);
			fe[i]->SetWidth(70);
			fe[i]->Align(fs, AL_AFTER);
			spf[i] = 0;
		}	
		else
		{
			spf[i] = new ScriptParamField(fg[i], "", &iParamType[i], &fIncExt[i]);
			spf[i]->SetCallBack((NotifyProc)&ScriptParamView::Changed); 		
			spf[i]->Align(fs, AL_AFTER);
			fe[i] = 0;
		}			
	}
}

int ScriptParamView::Init(Event *)
{
	for (int i = 0; i < 9; ++i) 
	{
		if ( fe[i] && sValDummy[i] == "")
			fe[i]->Hide();

		if ( spf[i] )
			spf[i]->SetCallBack((NotifyProc)&ScriptParamView::Changed); 
	}		

	Script scr = GetDocument()->scr();
	bool fReadOnly = scr->fReadOnly();
	fiParam->SetReadOnly(fReadOnly);
	return 1;
}

int ScriptParamView::exec()
{
	FormBaseView::exec();
	Script scr = GetDocument()->scr();
	if (iParams > 9)
		iParams = 9;
	scr->SetParams(iParams);
	for (int i = 0; i < iParams; ++i) 
		scr->SetParam(i, (ScriptPtr::ParamType)iParamType[i], sQuestion[i], fIncExt[i]);
	return 0;
}

int ScriptParamView::DataChanged(Event *)
{
	return Changed(0);
}

int ScriptParamView::Changed(Event *)
{
	for (int i = 0; i < 9; ++i)
	{
		fg[i]->StoreData();
		bool fChanged = fDataHasChanged();		
		ScriptDoc	*doc = GetDocument();
		String sV;
		doc->obj()->ReadElement("Params",String("Param%d", i).c_str(), sV);
		ScriptPtr::ParamType ptType = doc->scr()->ptParam(i);
		bool fExt = doc->scr()->fParamIncludeExtension(i);
		if ( ptType != ScriptPtr::ptANY)
		{
			if ( sV != sQuestion[i] || iParamType[i] != ptType || fIncExt[i] != fExt)
			{
				doc->SetModifiedFlag();
				break;;
			}			
		}			
	}		
	return 1;
}

String ScriptParamView::sParamType(int i)
{
	map<int, String> mpParmNames;
	mpParmNames[ ScriptPtr::ptRASMAP] =  TR("Raster Map.mpr");
	mpParmNames[ ScriptPtr::ptSEGMAP] =  TR("Segment Map.mps");
	mpParmNames[ ScriptPtr::ptPOLMAP] =  TR("Polygon Map.mpa");
	mpParmNames[ ScriptPtr::ptPNTMAP] =  TR("Point Map.mpp");
	mpParmNames[ ScriptPtr::ptTABLE] =  TR("Table.tbt");
	mpParmNames[ ScriptPtr::ptCOLUMN] =  TR("Column.clm");
	mpParmNames[ ScriptPtr::ptMAPLIST] =  TR("Map List.mpl");
	mpParmNames[ ScriptPtr::ptMAPVIEW] =  TR("Map View.mpv");
	mpParmNames[ ScriptPtr::ptCOORDSYS] =  TR("CoordSystem.csy");
	mpParmNames[ ScriptPtr::ptGEOREF] =  TR("GeoReference.grf");
	mpParmNames[ ScriptPtr::ptDOMAIN] =  TR("Domain.dom");
	mpParmNames[ ScriptPtr::ptRPR] =  TR("Representation.rpr");
	mpParmNames[ ScriptPtr::ptFILTER] =  TR("Filter.fil");
	mpParmNames[ ScriptPtr::ptSCRIPT] =  TR("Script.isl");
	mpParmNames[ ScriptPtr::ptFUNCTION] =  TR("Function.fun");
	mpParmNames[ ScriptPtr::ptMATRIX] =  TR("Matrix.mat");
	mpParmNames[ ScriptPtr::ptSMS] =  TR("Sample Set.sms");
	mpParmNames[ ScriptPtr::ptTBL2D] =  TR("2-Dimensional Table.ta2");
	mpParmNames[ ScriptPtr::ptANNTXT] =  TR("Annotation Text.atx");
	mpParmNames[ ScriptPtr::ptSTRING ] =  TR("String");
	mpParmNames[ ScriptPtr::ptVALUE  ] =  TR("Value");
	mpParmNames[ ScriptPtr::ptFILENAME] =  TR("Filename");

	String sParm =  mpParmNames[ptParam(i)];
	return sParm.sHead(".");
}

int ScriptParamView::CallBack(Event*)
{
	if (!fiParam->fShow())
		return 0;
	fiParam->StoreData();
	if (iParams > 9)
		iParams = 9;
	if (iParams != GetDocument()->scr()->iParams())
		GetDocument()->SetModifiedFlag();
	if (iParams)
		fgHead->Show();
	else
		fgHead->Hide();
	int i;
	for (i = 0; i < iParams; ++i)
		fg[i]->Show();
	for (i = iParams; i < 9; ++i)
		fg[i]->Hide();
	return 0;
}

int ScriptParamView::iParms()
{
	return iParams;
}

String ScriptParamView::sParam(int i)
{
	return sQuestion[i];
}

ScriptPtr::ParamType ScriptParamView::ptParam(int i)
{
	return (ScriptPtr::ParamType)iParamType[i];
}
