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
/* $Log: /ILWIS 3.0/FormElements/fldrpr.cpp $
 * 
 * 14    26-09-00 12:49 Koolhoven
 * FieldRepresentationC::CreateRepresentation() now also works when
 * current rpr does not exist
 * 
 * 13    18/09/00 15:03 Willem
 * Create representation form now first checks if the domain allows this;
 * if not an error is displayed
 * 
 * 12    17-05-00 14:35 Koolhoven
 * Added icon in create forms
 * 
 * 11    10/05/00 14:35 Willem
 * - "Use Percentages" is now only available for system domain "value"
 * - Default representation name is only set if the current representation
 * is not a system representation
 * 
 * 10    27/04/00 15:28 Willem
 * Create representation form now display a checkbox for system domain
 * "value" allowing a rpr-value instead of the default rpr-gradual
 * 
 * 9     25/04/00 16:37 Willem
 * Added field in Create Representation form to allow creation of
 * representation value for system domain "value.dom"
 * 
 * 8     10-02-00 11:12 Leeuwen
 * CreateRepresentation: use sFullPathQuoted() insetad of sFullPath()
 * because produced string will be parsed.
 * 
 * 7     8/12/99 10:29 Willem
 * Changed FileName asignments (added extension in constructor)
 * 
 * 6     7/12/99 17:36 Willem
 * Add extension to filename to make sure the icon becomes visible in the
 * fielddatatype
 * 
 * 5     2/12/99 16:18 Willem
 * changed "ilwis" to "open" or "edit" for the execute function
 * 
 * 4     30-11-99 10:15 Koolhoven
 * Header comment
 * 
 * 3     30/11/99 9:53 Willem
 * Removed all references to winExec(); replaced by IlwWinApp->Execute()
// Revision 1.3  1998/09/16 17:37:53  Wim
// 22beta2
//
// Revision 1.2  1997/09/03 10:27:15  Wim
// Report errors during rpr creation
//
/* FieldRepresentation
   by Wim Koolhoven, september 1995
   Copyright Ilwis System Development ITC
	Last change:  WK   16 Feb 98    4:29 pm
*/
#include "Client\Headers\formelementspch.h"
#include "Client\ilwis.h"
#include "Headers\Hs\Represen.hs"
#include "Headers\Htp\Ilwismen.htp"
#include "Client\FormElements\fldrpr.h"

FieldRepresentationC::FieldRepresentationC(FormEntry* fe, const String& sQuestion, 
                                           String* sRpr, const Domain& dom) :
                      FieldDataTypeC(fe, sQuestion, sRpr,
                                     new RepresentationLister(dom), true,
                                     (NotifyProc)&FieldRepresentationC::CreateRepresentation),              
                                     dm(dom) 
{ 
  sNewName = *sRpr; 
  if (sNewName == "" && dom.fValid())
    sNewName = dom->sName();
}

void FieldRepresentationC::AllowRprAttr(bool fAllow)
{
  RepresentationLister* rl = dynamic_cast<RepresentationLister*>(fld->objLister());
  if (rl)
    rl->fRprAttr = fAllow;
}

void FieldRepresentationC::SetDomain(const Domain& dom)
{
  if (!dom.fValid())
    return;
  if (dm.fValid() && dm == dom)
    return;
  dm = dom; 
  RepresentationLister* rl = dynamic_cast<RepresentationLister*>(fld->objLister());
  if (rl)
    rl->SetDomain(dm);
}


int FieldRepresentationC::CreateRepresentation(void*)
{
  FileName fn(sNewName);
	if (sNewName.length() > 0)
	{
		try {
			Representation rpr(fn);
			if (rpr->fSystemObject())
				sNewName = "";
			else
				sNewName = fn.sFile;
		}
		catch (ErrorObject&) {
		}
	}
  FormCreateRepresentation form(frm()->wnd(), &sNewName, dm);
  if (form.fOkClicked()) {
    FillDir();
    FileName fn(sNewName);
    SetVal(fn.sShortName(true));
    Representation rpr(fn);
    String s = "edit ";
    s &= fn.sFullPathQuoted();
	IlwWinApp()->Execute(s); //        winExec(s.scVal(), SW_SHOWNORMAL);
  }
  return 0;
}

FormCreateRepresentation::FormCreateRepresentation(CWnd* wPar, String* str, const Domain& dom) :
  FormWithDest(wPar, SRPTitleCreateRepresentation),
  sRpr(str),
  fdr(NULL)
{
	// representation can only be set for Class and Value
	bool fOk = dom->pdc() || dom->pdv(); 
	if (!fOk)
	{
		MessageBox(SRPErrOnlyCVdomRpr.sVal(), SRPErrError.sVal(), MB_OK | MB_ICONEXCLAMATION);
	    _fOkClicked = false;
		return;
	}

	iImg = IlwWinApp()->iImage(".rpr");

  sNewName = *sRpr;
  if (dom.fValid())
    sDom = dom->sName(true);
  fdr = new FieldDataTypeCreate(root, SRPUiRprName, &sNewName, ".RPR", true);
  fdr->SetCallBack((NotifyProc)&FormCreateRepresentation::CallBackName);
  StaticText* st = new StaticText(root, SRPUiDescription);
  st->psn->SetBound(0,0,0,0);
  FieldString* fs = new FieldString(root, "", &sDescr);
  fs->SetWidth(120);
  fs->SetIndependentPos();
  String sFill('*', 40);
  fdom = new FieldDataType(root, SRPUiDomain, &sDom, new DomainLister(dmCLASS|dmVALUE|dmIMAGE),true);
  fdom->SetCallBack((NotifyProc)&FormCreateRepresentation::CallBackDomain);

	fcCreateGradual = new CheckBox(root, SRPUiUsePercentages, &fCreateGradual);
  stRemark = new StaticText(root, sFill);
  stRemark->SetIndependentPos();
  SetMenHelpTopic("ilwismen\\create_a_representation.htm");
  create();
}

FormEntry* FormCreateRepresentation::feDefaultFocus()  
{
  if (fdr) fdr->SelectAll();
  return fdr;
}

int FormCreateRepresentation::exec()
{
  FormWithDest::exec();
  *sRpr = sNewName;
  FileName fn = sDom;
  Domain dm(fn);
  fn = FileName(*sRpr, ".rpr");
  bool fOk = dm->pdc() || dm->pdv(); 
  if (!fOk) {
    char *s1=SRPErrOnlyCVdomRpr.sVal();
    char *s2=SRPErrError.sVal();
    MessageBox(s1, s2, MB_OK|MB_ICONINFORMATION);
    return 0;
  }
  try {
    Representation rpr(fn, dm, fCreateGradual);
    if (sDescr != "")
      rpr->sDescription = sDescr;
    rpr->Store();
  }
  catch (ErrorObject& err) {
    _fOkClicked = false;
    err.Show(SUITitleCreateRPR);
  }
  return 0;
}

int FormCreateRepresentation::CallBackName(Event *)
{
  fdr->StoreData();
  FileName fn(sNewName, ".rpr");
  bool fOk = false;
  if (!fn.fValid())
    stRemark->SetVal(SRPRemNotValidRprName);
  else if(File::fExist(fn))   
    stRemark->SetVal(SRPRemRprAlreadyExists);
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

int FormCreateRepresentation::CallBackDomain(Event *)
{
	fdom->StoreData();

  FileName fn = sDom;
  Domain dm(fn);
  bool fValue = fCIStrEqual(dm->sName(), "value"); // only system value domain
	if (fValue)
	{
		fCreateGradual = dm->fSystemObject();
		if (fCreateGradual)
		{
			fcCreateGradual->Show();
			fcCreateGradual->SetVal(fCreateGradual);
		}
		else
			fcCreateGradual->Hide();
	}
	else
		fcCreateGradual->Hide();

	return 0;
}




