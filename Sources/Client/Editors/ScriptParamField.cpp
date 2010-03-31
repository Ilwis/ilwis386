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
// ScriptParamField.cpp: implementation of the ScriptParamField class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Engine\Scripting\Script.h"
#include "Client\Editors\ScriptParamField.h"
#include "Headers\Hs\Script.hs"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const int iPARAMTYPES = 22;

class FieldScriptParamSimple: public FieldOneSelect
{
public:
  FieldScriptParamSimple(FormEntry* par, long* paramtype)
  : FieldOneSelect(par, paramtype, false)
  {
		partyp = (ScriptPtr::ParamType*) paramtype;
    SetWidth(75);
    int i = 0;
    pt.push_back(ParamStruct(ScriptPtr::ptRASMAP  , SSCRPtRasMap));
    pt.push_back(ParamStruct(ScriptPtr::ptPOLMAP  , SSCRPtPolMap));		
    pt.push_back(ParamStruct(ScriptPtr::ptSEGMAP  , SSCRPtSegMap));
    pt.push_back(ParamStruct(ScriptPtr::ptPNTMAP  , SSCRPtPntMap));
    pt.push_back(ParamStruct(ScriptPtr::ptTABLE   , SSCRPtTable));
    pt.push_back(ParamStruct(ScriptPtr::ptCOLUMN  , SSCRPtColumn));
    pt.push_back(ParamStruct(ScriptPtr::ptMAPLIST , SSCRPtMapList));
    pt.push_back(ParamStruct(ScriptPtr::ptMAPVIEW , SSCRPtMapView));
    pt.push_back(ParamStruct(ScriptPtr::ptANNTXT  , SSCRPtAnnTxt));		
    pt.push_back(ParamStruct(ScriptPtr::ptDOMAIN  , SSCRPtDomain));
    pt.push_back(ParamStruct(ScriptPtr::ptRPR     , SSCRPtRpr));		
    pt.push_back(ParamStruct(ScriptPtr::ptGEOREF  , SSCRPtGeoRef));		
    pt.push_back(ParamStruct(ScriptPtr::ptCOORDSYS, SSCRPtCoordSys));
    pt.push_back(ParamStruct(ScriptPtr::ptSMS     , SSCRPtSms));		
    pt.push_back(ParamStruct(ScriptPtr::ptTBL2D   , SSCRPtTbl2d));		
    pt.push_back(ParamStruct(ScriptPtr::ptMATRIX  , SSCRPtMatrix));		
    pt.push_back(ParamStruct(ScriptPtr::ptFILTER  , SSCRPtFilter));
    pt.push_back(ParamStruct(ScriptPtr::ptFUNCTION, SSCRPtFunction));		
    pt.push_back(ParamStruct(ScriptPtr::ptSCRIPT  , SSCRPtScript));
    pt.push_back(ParamStruct(ScriptPtr::ptSTRING  , SSCRPtString));
    pt.push_back(ParamStruct(ScriptPtr::ptVALUE   , SSCRPtValue));
    pt.push_back(ParamStruct(ScriptPtr::ptFILENAME, SSCRPtFileName));
  }
  void create() 
  {
    FieldOneSelect::create();
    for (int i = 0; i < pt.size(); ++i) {
      int id = ose->AddString(pt[i].sName.scVal());
      ose->SetItemDataPtr(id,(void*)(long)pt[i].pt);
			if (*partyp == pt[i].pt)
		    ose->SetCurSel(id);
    }
  }
  ScriptPtr::ParamType ptGet() const { return *partyp; }
private:
  struct ParamStruct 
  {
		ParamStruct() : pt(ScriptPtr::ParamType(0)), sName() {}
		ParamStruct(ScriptPtr::ParamType p, String sN) : pt(p), sName(sN) {}
		ScriptPtr::ParamType pt;
    String sName;
  };
  vector<ParamStruct> pt;
	ScriptPtr::ParamType* partyp;
};

ScriptParamField::ScriptParamField(FormEntry* parent, const String& sQuestion, long* pt, bool* fExt)
: FieldGroup(parent)
{
  if (sQuestion.length() != 0)
    new StaticTextSimple(this, sQuestion);
  fsps = new FieldScriptParamSimple(this, pt);
  fsps->SetCallBack((NotifyProc)&ScriptParamField::CallBack, this);
  fsps->psn->iBndRight += 10;
  cb = new CheckBox(this, SSCRUiIncludeExt, fExt);
	cb->SetCallBack((NotifyProc)&ScriptParamField::CallBackCheckbox, this);
  if (children.iSize() > 1) // also static text
    children[1]->Align(children[0], AL_AFTER);
}

ScriptParamField::~ScriptParamField()
{
}

int ScriptParamField::CallBack(Event*)
{
  ScriptPtr::ParamType pt = fsps->ptGet();
  switch (pt) 
  {
    case ScriptPtr::ptSTRING: 
    case ScriptPtr::ptVALUE: 
    case ScriptPtr::ptFILENAME: 
    case ScriptPtr::ptCOLUMN: 
      cb->Hide();
      break;
    default:
      cb->Show();
      break;
  }
  
  DoCallBack();
  return 0;
}

int ScriptParamField::CallBackCheckbox(Event*)
{
  DoCallBack();
  return 0;
}
