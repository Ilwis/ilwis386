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
/* $Log: /ILWIS 3.0/FormElements/fldantxt.cpp $
 * 
 * 8     9/24/03 4:19p Martin
 * [bug 4216] added handling of text columns for annotation text
 * 
 * 7     14-11-00 11:06 Koolhoven
 * FormCreateAnnotationText has now also the default map name as input as
 * paramter
 * 
 * 6     14-11-00 10:54 Koolhoven
 * FormCreateAnnotationText now also allows attribute maps as input
 * 
 * 5     17-05-00 14:35 Koolhoven
 * Added icon in create forms
 * 
 * 4     8/12/99 10:29 Willem
 * Changed FileName asignments (added extension in constructor)
 * 
 * 3     29-10-99 16:33 Koolhoven
 * Header comment
 * 
 * 2     29-10-99 12:55 Wind
 * case sensitive stuff
// Revision 1.2  1998-10-22 10:45:44+01  Wim
// Added CallBackMap() to only show the radiogroup Code,Name,Both with
// maps of domain sort.
//
// Revision 1.1  1998-09-16 18:37:53+01  Wim
// 22beta2
//
/* FormCreateAnnotationText
   by Wim Koolhoven, may 1998
   Copyright Ilwis System Development ITC
	Last change:  WK   22 Oct 98   10:42 am
*/
#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\fldantxt.h"
#include "Engine\Map\Segment\Seg.h"
#include "Engine\Map\Polygon\POL.H"
#include "Engine\Map\Point\PNT.H"
#include "Engine\Map\txtann.h"
#include "Headers\Hs\Mainwind.hs"
#include "Headers\Hs\Editor.hs"
#include "Headers\Htp\Ilwismen.htp"
#include "Client\ilwis.h"
#include "Client\FormElements\objlist.h"

FormCreateAnnotationText::FormCreateAnnotationText(CWnd* wPar, String* satx, double rWorkScale, const String& sDfltMap)
  : FormWithDest(wPar, TR("Create Annotation Text")), 
	sAtx(satx), rScale(rWorkScale), sMap(sDfltMap)
{
	iImg = IlwWinApp()->iImage(".atx");

  if (sAtx)
    sNewName = *sAtx;
  fdc = new FieldDataTypeCreate(root, TR("&AnnText Name"), &sNewName, ".atx", false);
  fdc->SetCallBack((NotifyProc)&FormCreateAnnotationText::CallBackName);
  StaticText* st = new StaticText(root, TR("&Description:"));
  st->psn->SetBound(0,0,0,0);
  FieldString* fs = new FieldString(root, "", &sDescr);
  fs->SetWidth(120);
  fs->SetIndependentPos();
  fBasedOnMap = true;
  CheckBox* cb = new CheckBox(root, TR("&Based on Map"), &fBasedOnMap);
  FieldGroup* fg = new FieldGroup(cb);
  fdt = new FieldDataType(fg, "", &sMap, new MapListerDomainType(".mpp.mps.mpa",dmCLASS|dmIDENT|dmVALUE|dmSTRING,true),true);
//  fdt = new FieldDataType(fg, "", &sMap, ".mpp.mps.mpa", true);
  fdt->SetCallBack((NotifyProc)&FormCreateAnnotationText::CallBackMap);
  fdt->Align(cb, AL_AFTER);
  iCodeName = 2;
  rg = new RadioGroup(fg, "", &iCodeName, true);
  rg->Align(cb, AL_UNDER);
  rg->SetIndependentPos();
  new RadioButton(rg, TR("&Code"));
  new RadioButton(rg, TR("&Name"));
  new RadioButton(rg, TR("&Both"));

  FieldReal* fr = new FieldReal(root, TR("Working &Scale 1:"), &rScale, ValueRange(1,1e12,1));
  fr->Align(rg, AL_UNDER);

  String sFill('*', 40);
  stRemark = new StaticText(root, sFill);
  stRemark->SetIndependentPos();

  SetMenHelpTopic("ilwismen\\create_an_annotation_text_object.htm");
  create();
}

int FormCreateAnnotationText::CallBackName(Event *)
{
  fdc->StoreData();
  FileName fn(sNewName, ".atx");
  bool fOk = false;
  if (!fn.fValid())
    stRemark->SetVal(TR("Not a valid name"));
  else if(File::fExist(fn))   
    stRemark->SetVal(TR("Object already exists"));
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

int FormCreateAnnotationText::CallBackMap(Event *)
{
  fdt->StoreData();
  try {
    BaseMap map(sMap);
    if (map->dm()->pdsrt())
      rg->Show();
    else
      rg->Hide();
  }
  catch (ErrorObject&) {
    rg->Hide();
  }

  return 0;
}

Column FormCreateAnnotationText::colGetPossibleStringColumn(const FileName& fn)
{
	BaseMap mp(fn);
	Column col;
	if ( mp->fTblAtt())
	{
		if ( fn.sCol != "")
		{
			Table tbl = mp->tblAtt();
			col = tbl->col(fn.sCol);
			if ( col.fValid() && col->dm()->dmt() == dmtSTRING)
			{
				return col;
			}				
		}			
	}
	return Column();
}

AnnotationTextPtr* FormCreateAnnotationText::ptrCreateAnnotationText(const FileName& fn)
{
	AnnotationTextPtr* atx = 0;
	AnnotationTextPtr::eCodeName ecn = AnnotationTextPtr::eCodeName(iCodeName);
	FileName fnMap(sMap);
	Column colPossibleStringColumn = colGetPossibleStringColumn(fnMap);
	if ( colPossibleStringColumn.fValid() )
		fnMap.sCol = "";
	
	if (fCIStrEqual(fnMap.sExt, ".mpp")) 
	{
		PointMap map = colPossibleStringColumn.fValid() ? PointMap(fnMap) : PointMap(sMap,"");
		atx = new AnnotationTextPtr(fn, map, ecn);
	}
	else if (fCIStrEqual(fnMap.sExt, ".mps")) 
	{
		SegmentMap map = colPossibleStringColumn.fValid() ? SegmentMap(fnMap) : SegmentMap(sMap,"");		
		atx = new AnnotationTextPtr(fn, map, ecn);
	}
	else if (fCIStrEqual(fnMap.sExt, ".mpa")) 
	{
		PolygonMap map = colPossibleStringColumn.fValid() ? PolygonMap(fnMap) : PolygonMap(sMap,"");		
		atx = new AnnotationTextPtr(fn, map, ecn);
	}
	else
	_fOkClicked = false;

	if ( atx && colPossibleStringColumn.fValid())
	{
		for(int iRec = 1; iRec <= colPossibleStringColumn->iRecs(); ++iRec)
		{
			String sVal = colPossibleStringColumn->sValue(iRec, 0);
			atx->colText->PutVal(iRec, sVal);
		}			
	}		

	return atx;
}

int FormCreateAnnotationText::exec()
{
  FormWithDest::exec();
  if (sAtx)
    *sAtx = sNewName;
  FileName fn(sNewName, ".atx");
  AnnotationTextPtr* atx = 0;
  try {
    if (fBasedOnMap) {
		atx = ptrCreateAnnotationText(fn);
    }
    else
      atx = new AnnotationTextPtr(fn, 1);
    if (atx) {
      if (sDescr != "")
        atx->sDescription = sDescr;
      atx->rWorkingScale = rScale;
      atx->CreateColumnFontName();
      atx->CreateColumnFontSize();
      atx->CreateColumnFontBold();
      atx->CreateColumnFontItalic();
      atx->CreateColumnFontUnderline();
      atx->CreateColumnJustification();
      atx->CreateColumnColor();
      atx->CreateColumnTransparent();
      atx->CreateColumnRotation();
      atx->Store();
      delete atx;
    }
  }
  catch (ErrorObject& err) {
    _fOkClicked = false;
    err.Show();
  }  
  return 0;
}




