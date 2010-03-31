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
#include "Client\Headers\formelementspch.h"
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Client\Base\Res.h"
#include "Engine\Scripting\Script.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\Editors\TextDoc.h"
#include "Client\Editors\ScriptDoc.h"
#include "Client\Headers\AppFormsPCH.h"
#include "Engine\Scripting\Script.h"
#include "Client\FormElements\fldval.h"
#include "Client\FormElements\fldtbl.h"
#include "Headers\Hs\Script.hs"
#include "Client\Editors\ScriptParamView.h"
#include "Client\Editors\ScriptParamDefaultView.h"

IMPLEMENT_DYNCREATE(ScriptParamDefaultView, FormBaseView)

BEGIN_MESSAGE_MAP(ScriptParamDefaultView, FormBaseView)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

ScriptParamDefaultView::ScriptParamDefaultView() :
  fAllreadyShown(false)
{
	fbs |= fbsNOOKBUTTON | fbsNOCANCELBUTTON | fbsNOBEVEL;
	fbs |= fbsNOPARENTRESIZE;
}

ScriptParamDefaultView::~ScriptParamDefaultView()
{
}

ScriptDoc* ScriptParamDefaultView::GetDocument() 
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(ScriptDoc)));
	return dynamic_cast<ScriptDoc *>(m_pDocument);
}

ScriptParamView *ScriptParamDefaultView::GetParmView()
{

	CDocument *doc = 	GetDocument();
	if ( !doc) return NULL;

  POSITION pos = doc->GetFirstViewPosition();
	for (CView* pView = doc->GetNextView(pos); pView; pView = doc->GetNextView(pos)) 
	{
		ScriptParamView* spv = dynamic_cast<ScriptParamView*>(pView);
		if (spv) 
		{
			return spv;
		}
	}
	return NULL;
}

void ScriptParamDefaultView::OnShowWindow(BOOL bShow, UINT nStatus )
{
	if ( fAllreadyShown ) 
  {
	  fAllreadyShown = bShow == TRUE;
		return;
	}

  fAllreadyShown = bShow == TRUE;
	if (!bShow) 
    return;

	RemoveChildren();
	ScriptParamView *spv = GetParmView();
	spv->FormBaseView::exec();

	int	iParams = spv->iParms();
	if ( iParams == iUNDEF ) return;

  sVal.resize(0);
	sValDummy.resize(0);
	fent.resize(0);

	sVal.resize(iParams);
	sValDummy.resize(iParams);
	fent.resize(iParams);

	Script scr = GetDocument()->scr();	
	bool fReadOnly = scr->fReadOnly();
	DWORD iReadOnlyStyle = ES_AUTOHSCROLL|WS_TABSTOP|WS_GROUP|WS_BORDER|ES_READONLY;
	
	for (int i = 0; i < iParams; ++i) 
	{
		String sQuestion = spv->sParam(i);
		fent[i] = NULL;
		if ( scr.fValid())
		{
			String sDefault = scr->sDefaultValue(i);
			sVal[i] = sDefault;
		}
		if ( fReadOnly )
		{
			if ( spv->ptParam(i) != ScriptPtr::ptSTRING && spv->ptParam(i) != ScriptPtr::ptFILENAME)
				sValDummy[i] = FileName(sVal[i]).sRelative();
			else
				sValDummy[i] = sVal[i];
			
			fent[i] = new FieldString(root, sQuestion, &sValDummy[i], Domain(), true ,iReadOnlyStyle);
			fent[i]->SetWidth(120);
		}			
		else
		{
			switch (spv->ptParam(i))
			{
				case ScriptPtr::ptSTRING:
					fent[i] = new FieldString(root, sQuestion, &sVal[i]);
					break;
				case ScriptPtr::ptVALUE:	{
					DomainValueRangeStruct dvrs(-1e9,1e9,0.001);
					fent[i] = new FieldVal(root, sQuestion, dvrs, &sVal[i]);
					break; }
				case ScriptPtr::ptDOMAIN:
					fent[i] = new FieldDomain(root, sQuestion, &sVal[i]);
					break;
				case ScriptPtr::ptRPR:
					fent[i] = new FieldRepresentation(root, sQuestion, &sVal[i]);
					break;
				case ScriptPtr::ptGEOREF:
					fent[i] = new FieldGeoRefExisting(root, sQuestion, &sVal[i]);
					break;
				case ScriptPtr::ptCOORDSYS:
					fent[i] = new FieldCoordSystem(root, sQuestion, &sVal[i]);
					break;
				case ScriptPtr::ptRASMAP:
					fent[i] = new FieldMap(root, sQuestion, &sVal[i]);
					break;
				case ScriptPtr::ptSEGMAP:
					fent[i] = new FieldSegmentMap(root, sQuestion, &sVal[i]);
					break;
				case ScriptPtr::ptPOLMAP:
					fent[i] = new FieldPolygonMap(root, sQuestion, &sVal[i]);
					break;
				case ScriptPtr::ptPNTMAP:
					fent[i] = new FieldPointMap(root, sQuestion, &sVal[i]);
					break;
				case ScriptPtr::ptTABLE:
					fent[i] = new FieldDataType(root, sQuestion, &sVal[i], ".TBT", true);
					break;
				case ScriptPtr::ptMAPVIEW:
					fent[i] = new FieldDataType(root, sQuestion, &sVal[i], ".MPV", true);
					break;
				case ScriptPtr::ptMAPLIST:
					fent[i] = new FieldDataType(root, sQuestion, &sVal[i], ".MPL", true);
					break;
				case ScriptPtr::ptTBL2D:
					fent[i] = new FieldDataType(root, sQuestion, &sVal[i], ".TA2", true);
					break;
				case ScriptPtr::ptANNTXT:
					fent[i] = new FieldDataType(root, sQuestion, &sVal[i], ".ATX", true);
					break;
				case ScriptPtr::ptSMS:
					fent[i] = new FieldDataType(root, sQuestion, &sVal[i], ".SMS", true);
					break;
				case ScriptPtr::ptMATRIX:
					fent[i] = new FieldDataType(root, sQuestion, &sVal[i], ".MAT", true);
					break;
				case ScriptPtr::ptFILTER:
					fent[i] = new FieldDataType(root, sQuestion, &sVal[i], ".FIL", true);
					break;
				case ScriptPtr::ptFUNCTION:
					fent[i] = new FieldDataType(root, sQuestion, &sVal[i], ".FUN", true);
					break;
				case ScriptPtr::ptSCRIPT:
					fent[i] = new FieldDataType(root, sQuestion, &sVal[i], ".ISL", true);
					break;
				case ScriptPtr::ptFILENAME:
					fent[i] = new FieldString(root, sQuestion, &sVal[i]);
					break;
			}
		}
		if ( fent[i] )
			fent[i]->SetCallBack((NotifyProc)&ScriptParamDefaultView::OnChange);
	}
	create();
}

int ScriptParamDefaultView::OnChange(Event *)
{
	ScriptParamView *spv = GetParmView();
	spv->FormBaseView::exec();

	int	iParams = spv->iParms();
	if ( iParams == iUNDEF ) return 0;

	Script scr = GetDocument()->scr();

  for(int i=0; i<sVal.size(); ++i)
	{
		if (fent[i] )
		{
			fent[i]->StoreData();
		}
		String sOldDefault = scr->sDefaultValue(i);
		String sNewDefault = sVal[i];
		if ( sOldDefault != sNewDefault )
		{
			GetDocument()->SetModifiedFlag();
			break;
		}
	}
	return 0;
}

void ScriptParamDefaultView::CreateForm()
{

}

int ScriptParamDefaultView::exec()
{
	if ( sVal.size() == 0 ) return 0; //nothing has been entered
  FormBaseView::exec();
	String sRes;
	Script scr = GetDocument()->scr();
	int	iParams = scr->iParams();

	for (int i = 0; i < iParams; ++i) 
	{
		scr->SetDefaultParam(i, sVal[i]);
	}
  return 0;
}



