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
/* FieldCoordSystem
   by Wim Koolhoven, october 1995
   Copyright Ilwis System Development ITC
	Last change:  WK    8 Jul 98   12:07 pm
*/

#include "Client\Headers\formelementspch.h"
#include "Client\ilwis.h"
#include "Client\FormElements\fldcs.h"
#include "Headers\Htp\Ilwismen.htp"
#include "Headers\Hs\Coordsys.hs"
#include "Engine\Map\Raster\Map.h"
#include "Engine\SpatialReference\csbonly.h"
#include "Engine\SpatialReference\CoordSystemTiePoints.H"
#include "Engine\SpatialReference\CoordSystemDirectLinear.h"
#include "Engine\SpatialReference\CoordSystemOrthoPhoto.h"
#include "Engine\SpatialReference\csformul.h"
#include "Engine\SpatialReference\Csproj.h"
#include "Engine\SpatialReference\Cslatlon.h"
#include "Client\FormElements\objlist.h"

int FieldCoordSystemC::CreateCoordSystem(void *)
{
	FileName fn(sNewName);
	if (fn.sFile == "unknown")
		sNewName = "";
	FormCreateCoordSystem form(frm()->wnd(), &sNewName, csTypes);
	if (form.fOkClicked())
	{
		FillDir();
		FileName fn(sNewName, ".csy");
		try
		{
			CoordSystem cs(fn);
			cs->cb = m_cb;
			cs->Updated();
		}
		catch (...)
		{
		}
		SetVal(fn.sFile);
		CallCallBacks();
		String s = "open ";
		s &= fn.sFullPathQuoted();
		IlwWinApp()->Execute(s);
	}
	else
		sNewName = "";
	return 0;
}

FormCreateCoordSystem::FormCreateCoordSystem(CWnd* wPar, String* sCS, long types)
: FormWithDest(wPar, SCSTitleCreateCoordSystem),
  sCoordSystem(sCS), wParent(wPar), csTypes(types)
{
	iImg = IlwWinApp()->iImage(".csy");

  FileName fn(*sCoordSystem);
  sNewName = fn.sFile;
  fcs = new FieldDataTypeCreate(root, SCSUiCoordSysName, &sNewName, ".CSY", false);
  fcs->SetCallBack((NotifyProc)&FormCreateCoordSystem::CallBackName);
  fcs->SetIndependentPos();
  StaticText* st = new StaticText(root, SCSUiDescription);
  st->psn->SetBound(0,0,0,0);
  FieldString* fs = new FieldString(root, "", &sDescr);
  fs->SetWidth(120);
  fs->SetIndependentPos();

  iOption = 0;
  rg = new RadioGroup(root, "", &iOption);
	RadioButton* rb = 0;
	if (csBOUNDSONLY & csTypes)
	  rb = new RadioButton(rg, SCSUiCsyBoundsOnly);
	if (csPROJ & csTypes)
		rb = new RadioButton(rg, SCSUiCsyProjection);
	if (csLATLON & csTypes)
		rb = new RadioButton(rg, SCSUiCsyLatLon);

	if (csFORMULA & csTypes) {
		RadioButton* rbFormu = new RadioButton(rg, SCSUiCsyFormula);
		FieldGroup* fgFormu = new FieldGroup(rbFormu,true);
		FormEntry* fcsRelCsy0 = new FieldCoordSystem(fgFormu, SCSUiReferenceCsy, &sRelCsy0);
		fcsRelCsy0->Align(rbFormu, AL_AFTER);
		rb = rbFormu;
	}

	if (csTIEPOINTS & csTypes) {
		RadioButton* rbCTP = new RadioButton(rg, SCSUiCsyTiePoints);
		FieldGroup* fgCTP = new FieldGroup(rbCTP,true);
		//fgCTP->Align(rg, AL_UNDER);
		FormEntry* fcsRelCsy1 = new FieldCoordSystem(fgCTP, SCSUiReferenceCsy, &sRelCsy1);
		fcsRelCsy1->Align(rb, AL_AFTER);
		FormEntry* fdtRefMap1 = new FieldDataType(fgCTP, SCSUiReferenceMap,
														 &sRefMap, ".mpp.mps.mpa", true); //backgr map
		fdtRefMap1->Align(rbCTP, AL_AFTER);
		rb = rbCTP;
	}
  
	if (csDIRECTLINEAR & csTypes) {
		RadioButton* rbDirLin = new RadioButton(rg, SCSUiCsyDirLin);
		FieldGroup* fgDirLin = new FieldGroup(rbDirLin,true);
		FormEntry* fdtRefMap2 = new FieldDataType(fgDirLin, SCSUiReferenceMap,
														 &sRefMap, ".mpp.mps.mpa", true); //backgr map
		fdtRefMap2->Align(rb, AL_AFTER);
		FormEntry* fdtDtmMap2 = new FieldDataType(fgDirLin, SCSUiDtmMap,
														 &sDTM, new MapListerDomainType(dmVALUE), true);
		fdtDtmMap2->Align(rbDirLin, AL_AFTER);
		rb = rbDirLin;
	}

	if (csORTHOPHOTO & csTypes) {
		RadioButton* rbOrthoPh = new RadioButton(rg, SCSUiCsyOrthoPhoto);
		FieldGroup* fgOrthoPh = new FieldGroup(rbOrthoPh,true);
		FormEntry* fdtRefMap3 = new FieldDataType(fgOrthoPh, SCSUiReferenceMap,
														 &sRefMap, ".mpp.mps.mpa", true); //backgr map
		fdtRefMap3->Align(rb, AL_AFTER);
		FormEntry* fdtDtmMap3 = new FieldDataType(fgOrthoPh, SCSUiDtmMap,
														 &sDTM, new MapListerDomainType(dmVALUE), true);
		fdtDtmMap3->Align(rbOrthoPh, AL_AFTER);
		rb = rbOrthoPh;
	}

  String sFill('*', 70);
  stRemark = new StaticText(root, sFill);
  stRemark->SetVal(String());
  stRemark->Align(rb, AL_UNDER);
  stRemark->SetIndependentPos();
  SetMenHelpTopic("ilwismen\\create_a_coordinate_system.htm");
  create();
}

FormCreateCoordSystem::~FormCreateCoordSystem()
{
//  if (wParent)
//    SetActiveWindow(*wParent);
}

int FormCreateCoordSystem::exec()
{
  FormWithDest::exec();
  *sCoordSystem = sNewName;
  FileName fn(*sCoordSystem, ".csy");
	if (!(csBOUNDSONLY & csTypes))
		++iOption;
	if (!(csPROJ & csTypes))
		if (iOption > 0)
			++iOption;
	if (!(csLATLON & csTypes))
		if (iOption > 1)
			++iOption;
	if (!(csFORMULA & csTypes)) 
		if (iOption > 2)
			++iOption;
	if (!(csTIEPOINTS & csTypes))
		if (iOption > 3)
			++iOption;
	if (!(csDIRECTLINEAR & csTypes))
		if (iOption > 4)
			++iOption;
	if (!(csORTHOPHOTO & csTypes))
		if (iOption > 5)
			++iOption;

  try {
    CoordSystemPtr* csp = 0;
    switch (iOption) {
      case 0:
        csp = new CoordSystemBoundsOnly(fn, 1);
        break;
      case 1:
        csp = new CoordSystemProjection(fn, 1);
        break;
      case 2:
        csp = new CoordSystemLatLon(fn, 1);
        break;
      case 3: {
        CoordSystem csRelated(sRelCsy0);
        csp = new CoordSystemFormula(fn, csRelated);
      } break;
      case 4: {
        CoordSystem csRelated(sRelCsy1);
        csp = new CoordSystemTiePoints(fn, csRelated, sRefMap);
      } break;
			case 5: {
        Map mpDTM(sDTM);
        csp = new CoordSystemDirectLinear(fn, mpDTM, sRefMap);
      } break;
			case 6: {
        Map mpDTM(sDTM);
        csp = new CoordSystemOrthoPhoto(fn, mpDTM, sRefMap);
      } break;
    }
    if (sDescr != "")
      csp->sDescription = sDescr;
		else  
      csp->SetDescription(csp->sTypeName());
    csp->Store();
    delete csp;
    if (iOption >= 4) {
      CoordSystem cs(fn);
      BaseMap map(sRefMap);
      map->SetCoordSystem(cs);
    }
  }
  catch (ErrorObject& err) {
    _fOkClicked = false;
    err.Show();
  }  
  return 0;
}

int FormCreateCoordSystem::CallBackName(Event *)
{
  fcs->StoreData();
  FileName fn(sNewName, ".csy");
  bool fOk = false;
  if (!fn.fValid())
    stRemark->SetVal(SCSRemNotValidCsyName);
  else if(File::fExist(fn))
    stRemark->SetVal(SCSRemCsyExists);
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

FormCreateCoordSystemMW::FormCreateCoordSystemMW(CWnd* wPar, String* sCS)
: FormWithDest(wPar, SCSTitleCreateCoordSystem),
	sCoordSystem(sCS), wParent(wPar)
{
	iImg = IlwWinApp()->iImage(".csy");
	
	StaticText* st = 0;
	FileName fn(*sCoordSystem);
	sNewName = fn.sFile;
	fcs = new FieldDataTypeCreate(root, SCSUiCoordSysName, &sNewName, ".CSY", false);
	fcs->SetCallBack((NotifyProc)&FormCreateCoordSystemMW::CallBackName);
	fcs->SetIndependentPos();
	st = new StaticText(root, SCSUiDescription);
	st->psn->SetBound(0,0,0,0);
	FieldString* fs = new FieldString(root, "", &sDescr);
	fs->SetWidth(120);
	fs->SetIndependentPos();
	
	iOption = 0;
	rg = new RadioGroup(root, "", &iOption);
	RadioButton* rbFormu = new RadioButton(rg, SCSUiCsyFormula);
	FieldGroup* fgFormu = new FieldGroup(rbFormu,true);
	FormEntry* fcsRelCsy0 = new FieldCoordSystem(fgFormu, SCSUiReferenceCsy, &sRelCsy0);
	fcsRelCsy0->Align(rbFormu, AL_AFTER);
	
	RadioButton* rbCTP = new RadioButton(rg, SCSUiCsyTiePoints);
	FieldGroup* fgCTP = new FieldGroup(rbCTP,true);
	FormEntry* fcsRelCsy1 = new FieldCoordSystem(fgCTP, SCSUiReferenceCsy, &sRelCsy1);
	fcsRelCsy1->Align(rbFormu, AL_AFTER);
	
	RadioButton* rbDirLin = new RadioButton(rg, SCSUiCsyDirLin);
	FieldGroup* fgDirLin = new FieldGroup(rbDirLin,true);
	
	FormEntry* fdtDtmMap2 = new FieldDataType(fgDirLin, SCSUiDtmMap,
		&sDTM, new MapListerDomainType(dmVALUE), true);
	fdtDtmMap2->Align(rbDirLin, AL_AFTER);
	
	RadioButton* rbOrthoPh = new RadioButton(rg, SCSUiCsyOrthoPhoto);
	FieldGroup* fgOrthoPh = new FieldGroup(rbOrthoPh,true);
	
	FormEntry* fdtDtmMap3 = new FieldDataType(fgOrthoPh, SCSUiDtmMap,
		&sDTM, new MapListerDomainType(dmVALUE), true);
	fdtDtmMap3->Align(rbOrthoPh, AL_AFTER);
	
	String sFill('*', 70);
	stRemark = new StaticText(root, sFill);
	stRemark->Align(rbOrthoPh, AL_UNDER);
	stRemark->SetIndependentPos();
	SetMenHelpTopic("ilwismen\\create_a_coordinate_system_in_map_window.htm");
	create();
}

FormCreateCoordSystemMW::~FormCreateCoordSystemMW()
{
//  if (wParent)
//    SetActiveWindow(*wParent);
}

int FormCreateCoordSystemMW::exec()
{
  FormWithDest::exec();
  *sCoordSystem = sNewName;
  FileName fn(*sCoordSystem, ".csy");
  try {
    CoordSystemPtr* csp = 0;
    switch (iOption) {
      case 0: {
        CoordSystem csRelated(sRelCsy0);
        csp = new CoordSystemFormula(fn, csRelated);
      } break;
      case 1: {
        CoordSystem csRelated(sRelCsy1);
        csp = new CoordSystemTiePoints(fn, csRelated, "");
      } break;
			case 2: {
        Map mpDTM(sDTM);
        csp = new CoordSystemDirectLinear(fn, mpDTM, "");
      } break;
			case 3: {
        Map mpDTM(sDTM);
        csp = new CoordSystemOrthoPhoto(fn, mpDTM, "");
      } break;
    }
    if (sDescr != "")
      csp->sDescription = sDescr;
		else  
      csp->SetDescription(csp->sTypeName());
    csp->Store();
    delete csp;
  }
  catch (ErrorObject& err) {
    _fOkClicked = false;
    err.Show();
  }  
  return 0;
}

int FormCreateCoordSystemMW::CallBackName(Event *)
{
  fcs->StoreData();
  FileName fn(sNewName, ".csy");
  bool fOk = false;
  if (!fn.fValid())
    stRemark->SetVal(SCSRemNotValidCsyName);
  else if(File::fExist(fn))
    stRemark->SetVal(SCSRemCsyExists);
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




