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
// FieldFiducialMark.cpp: implementation of the FieldFiducialMark class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Engine\SpatialReference\Grortho.h"
#include "Client\Editors\Digitizer\DIGITIZR.H"
#include "Client\Editors\Georef\EditFiducialMarksForm.h"
#include "Client\Editors\Georef\FieldFiducialMark.h"
#include "Client\FormElements\syscolor.h"
#include "Headers\Hs\Digitizr.hs"
#include "Headers\Hs\Georef.hs"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


FieldFiducialMark::FieldFiducialMark(FormEntry* par, int iNr, const bool fGrf)
: FieldGroup(par)
, fcPhoto(0)
, frc(0)
, fcDigGrid(0)
, stDRow(0), stDCol(0)
{
  fOk = false;
  String s("&%1i", iNr);
  cb = new CheckBox(this, s, &fOk);
	String sDummy = "12345.67";
  fcPhoto = new FieldCoord(this, "", &crdPhoto);
  fcPhoto->SetStyle(ES_AUTOHSCROLL|WS_TABSTOP|WS_GROUP|WS_BORDER|ES_RIGHT); // make editable numberfields right-justified
  fcPhoto->Align(cb, AL_AFTER);
	fcPhoto->SetWidth(32);
	if (fGrf) {
		frc = new FieldCoord(this, "", &crdRowCol);
		frc->SetStyle(ES_AUTOHSCROLL|WS_TABSTOP|WS_GROUP|WS_BORDER|ES_RIGHT);
		frc->SetStepSize(0.01);//needed for sub-Pixel Precision
		frc->Align(fcPhoto, AL_AFTER);
		frc->SetWidth(32);
		stDRow = new StaticTextSimple(this,sDummy);
		stDRow->Align(frc, AL_AFTER);
		stDCol = new StaticTextSimple(this,sDummy);
		stDCol->Align(stDRow, AL_AFTER);
	}
	else
	{
		fcDigGrid = new FieldCoord(this, "", &crdDigGrid);
		fcDigGrid->SetStyle(ES_AUTOHSCROLL|WS_TABSTOP|WS_GROUP|WS_BORDER|ES_RIGHT);
		fcDigGrid->SetStepSize(0.01);
		fcDigGrid->Align(fcPhoto, AL_AFTER);
		stDRow = new StaticTextSimple(this,sDummy);
		stDRow->Align(fcDigGrid, AL_AFTER);
		stDCol = new StaticTextSimple(this,sDummy);
		stDCol->Align(stDRow, AL_AFTER);
	}
  fAct = false;
  fPhoto = false;
  fRowCol = false;
	fDigGrid =  false;
}

void FieldFiducialMark::create()
{
  FieldGroup::create();
  cb->setNotifyFocusChange(this, (NotifyProc)&FieldFiducialMark::FocusChange);
  fcPhoto->setNotifyFocusChange(this, (NotifyProc)&FieldFiducialMark::FocusChange);
  if (frc) 
		frc->setNotifyFocusChange(this, (NotifyProc)&FieldFiducialMark::FocusChange);
	if (fcDigGrid)
		fcDigGrid->setNotifyFocusChange(this, (NotifyProc)&FieldFiducialMark::FocusChange);
}

FieldFiducialMark::~FieldFiducialMark()
{
  cb->removeNotifyFocusChange(this, (NotifyProc)&FieldFiducialMark::FocusChange);
  fcPhoto->removeNotifyFocusChange(this, (NotifyProc)&FieldFiducialMark::FocusChange);
	if (frc) 
		frc->removeNotifyFocusChange(this, (NotifyProc)&FieldFiducialMark::FocusChange);
	if (fcDigGrid)
		fcDigGrid->removeNotifyFocusChange(this, (NotifyProc)&FieldFiducialMark::FocusChange);
}

void FieldFiducialMark::SetCallBack(NotifyProc np)
{
  cb->SetCallBack(np);
  fcPhoto->SetCallBack(np);
	if (frc)
		frc->SetCallBack(np);
	if (fcDigGrid)
	fcDigGrid->SetCallBack(np);
}

void FieldFiducialMark::SetCallBack(NotifyProc np, CallBackHandler* evh)
{
  cb->SetCallBack(np,evh);
  fcPhoto->SetCallBack(np,evh);
	if (frc)
		frc->SetCallBack(np,evh);
	if (fcDigGrid)
		fcDigGrid->SetCallBack(np,evh);
}

FormEntry* FieldFiducialMark::CheckData()
{
  cb->StoreData();
  if (fOk) {
    FormEntry* fe = FieldGroup::CheckData();
    if (fe)
      return fe;
    // photo coordinates max. 500 mm
    if (abs(crdPhoto.x) > 500 || abs(crdPhoto.y) > 500)
      return fcPhoto;
    else
      return 0;
  }
  else
    return 0;
}

int FieldFiducialMark::FocusChange(Event* Evt)
{
  if (Evt->iMessage == WM_SETFOCUS)
    ((EditFiducialMarksForm*)frm())->SetActDigPoint(this);
  return 0;
}

void FieldFiducialMark::ShowVals()
{
  String s;
//  if (fPhoto && fRowCol) {
  if (fOk) {
    if (abs(rDCol) > 999)
      s = " ***";
    else
      s = String("%7.2f", rDCol);
    stDCol->SetVal(s);
    if (abs(rDRow) > 999)
      s = " ***";
    else
      s = String("%7.2f", rDRow);
    stDRow->SetVal(s);
  }
  else {
    s = " ";
    stDCol->SetVal(s);
    stDRow->SetVal(s);
  }
  stDCol->Show();
  stDRow->Show();
}
