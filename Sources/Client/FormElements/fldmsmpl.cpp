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
/* FieldSampleSet
   by Wim Koolhoven, august 1995
   Copyright Ilwis System Development ITC
	Last change:  WK    5 Mar 98    4:36 pm
*/

#include "Client\Headers\formelementspch.h"
#include "Engine\SampleSet\SAMPLSET.H"
#include "Engine\Map\Raster\MapList\maplist.h"
#include "Client\FormElements\flddom.h"
#include "Client\FormElements\fldlist.h"
#include "Client\FormElements\fldmsmpl.h"
#include "Headers\Htp\Ilwismen.htp"
#include "Headers\Hs\Sample.hs"
#include "Headers\Hs\DAT.hs"   // for the "only image domain allowed" message
#include "Client\ilwis.h"

//static fspArray fspaDummy; // trick for unresolved external

int FieldSampleSetC::CreateSampleSet(void*)
{
  FormCreateSampleSet form(frm()->wnd(), &sNewName);
  if (form.fOkClicked()) {
    FillDir();
    FileName fn(sNewName);
    SetVal(fn.sFile);
  }  
  return 0;
}

FormCreateSampleSet::FormCreateSampleSet(CWnd* wPar, String* sm,
                                         const String& sMpr, const String& sMpl)
: FormWithDest(wPar, TR("Create Sample Set")),
  sMap(sm), sBgMap(sMpr), sMapList(sMpl)
{
	iImg = IlwWinApp()->iImage(".sms");

  fdss = new FieldDataTypeCreate(root, TR("&Sample Set Name"), sMap, ".SMS", false);
  fdss->SetCallBack((NotifyProc)&FormCreateSampleSet::CallBackName);
  StaticText* st = new StaticText(root, TR("&Description:"));
  st->psn->SetBound(0,0,0,0);
  FieldString* fs = new FieldString(root, "", &sDescr);
  fs->SetWidth(120);
  fs->SetIndependentPos();
  new FieldDomainC(root, TR("&Domain"), &sDom, dmCLASS);
  m_fml = new FieldMapList(root, TR("&MapList"), &sMapList);
  m_fml->SetCallBack((NotifyProc)&FormCreateSampleSet::ChangeMapList);

  String sFill('X', 50);
  stRemark = new StaticText(root, sFill);
  stRemark->SetIndependentPos();
  SetMenHelpTopic("ilwismen\\create_a_sample_set.htm");
  create();
}

void FormCreateSampleSet::SetOKButton()
{
	String sErr;
	if (!m_fOkDomain)
		sErr = m_sDomErr;
	if (!m_fOkName)
		sErr = m_sNameErr;
	
	stRemark->SetVal(sErr);
	
	if (m_fOkName && m_fOkDomain)
		EnableOK();
	else    
		DisableOK();
}

int FormCreateSampleSet::CallBackName(Event *)
{
	fdss->StoreData();
	FileName fn(*sMap, ".sms");
	if (!fn.fValid())
		m_sNameErr = TR("Not a valid sample set name");
	else if(File::fExist(fn))   
		m_sNameErr = TR("Sample Set already exists");
	else
		m_sNameErr = String();

	m_fOkName = m_sNameErr.length() == 0;

	SetOKButton();

	return 0;
}

int FormCreateSampleSet::ChangeMapList(Event*)
{
	m_fml->StoreData();

	if (sMapList.length() == 0)
		return 0;

	FileName fnML(sMapList);
	MapList ml(fnML);

	Domain dm = ml[ml->iLower()]->dm();
	m_fOkDomain = dm->pdi() != 0;
	if (!m_fOkDomain)
		m_sDomErr = String(TR("Only image domain allowed, found: '%S'").c_str(), dm->sName(true, dm->fnObj.sPath()));
	else
		m_sDomErr = String();
	
	SetOKButton();

	return 0;
}

int FormCreateSampleSet::exec()
{
  FormWithDest::exec();
  FileName fn(*sMap, ".sms");
  try {
    Domain dm(sDom);
    FileName fnML(sMapList);
    MapList ml(fnML);
    SampleSet ms(fn, ml, dm);
    ms->sDescription = sDescr;
    ms->fInitStat();
  }
  catch (ErrorObject& err) {
    err.Show(TR("Create Sample Set"));
  }  
  return 0;
}
