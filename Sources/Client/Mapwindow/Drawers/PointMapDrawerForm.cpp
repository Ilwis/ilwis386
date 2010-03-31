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
// PointMapDrawerForm.cpp: implementation of the PointMapDrawerForm class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Engine\Map\Segment\Seg.h"
#include "engine\map\polygon\POL.H"
#include "Engine\Map\Point\PNT.H"
#include "Client\Mapwindow\Positioner.h"
#include "Client\Mapwindow\Drawers\BaseDrawer.h"
#include "Client\Mapwindow\Drawers\Drawer.h"
#include "Client\FormElements\fldcolor.h"
#include "Client\FormElements\fldcol.h"
#include "Client\FormElements\fldrpr.h"
#include "Client\Mapwindow\Drawers\BaseMapDrawer.h"
#include "Client\ilwis.h"
#include "Client\Base\datawind.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\MainWindow\Catalog\CatalogDocument.h"
#include "Client\Mapwindow\MapWindow.h"
#include "Engine\Map\Mapview.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Client\Editors\Utils\SYMBOL.H"
#include "Client\Editors\Utils\Smbext.h"
#include "Client\Editors\Utils\MULTICOL.H"
#include "Client\Mapwindow\Drawers\PointMapDrawer.h"
#include "Client\Mapwindow\Drawers\PointMapDrawerForm.h"
#include "Headers\Hs\Drwforms.hs"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


class PointMapSymbolForm: public FormWithDest
{
public:
  PointMapSymbolForm(PointMapDrawerForm* frm) 
  : FormWithDest(frm, SDCTitleSymbol)
  {
    new FieldExtendedSymbol(root, frm->smb);
    SetMenHelpTopic(htpCnfPointMapDrawerSymbol);
    create();
  }
};

class PointMapSymbolStretchForm: public FormWithDest
{
public:
  PointMapSymbolStretchForm(PointMapDrawerForm* frm) 
  : FormWithDest(frm, SDCTitleSymbol)
  {
    new FieldExtendedSymbol(root, frm->smb, false);
    
    cbStretch = new CheckBox(root, SDCUiStretch, &frm->fStretch);
    cbStretch->SetCallBack((NotifyProc)&PointMapSymbolStretchForm::StretchCallBack);

    fiSize = new FieldInt(root, SDCUiSize, &frm->iSize, ValueRange(1L,250L));
    fiSize->Align(cbStretch, AL_UNDER);
    fgStretch = new FieldGroup(root);
    
    if (frm->dmUse->pdvr()) {
      FieldRangeReal* frr = new FieldRangeReal(fgStretch, "", &frm->rrStretch);
      frr->Align(cbStretch, AL_AFTER);
    }
    else {
      FieldRangeInt* fri = new FieldRangeInt(fgStretch, "", &frm->riStretch);
      fri->Align(cbStretch, AL_AFTER);
    }
    FieldRangeInt* fri = new FieldRangeInt(fgStretch, SDCUiSize, &frm->riSize, ValueRange(1L,250L));
    fri->Align(cbStretch, AL_UNDER);
    
    RadioGroup* rgLinLog = new RadioGroup(fgStretch, "", &frm->iLinLog, true);
    rgLinLog->Align(fri, AL_UNDER);
    rgLinLog->SetIndependentPos();
    new RadioButton(rgLinLog, SDCUiLinear);
    new RadioButton(rgLinLog, SDCUiLogarithmic);
    RadioGroup* rgRadiusArea = new RadioGroup(fgStretch, "", &frm->iRadiusArea, true);
    rgRadiusArea->SetIndependentPos();
    new RadioButton(rgRadiusArea, SDCUiRadius);
    new RadioButton(rgRadiusArea, SDCUiArea);
    
    SetMenHelpTopic(htpCnfPointMapDrawerSymbolStretch);
    create();
  }
  int StretchCallBack(Event*) {
    if (cbStretch->fVal()) {
      fiSize->Hide();
      fgStretch->Show();
    }
    else {
      fgStretch->Hide();
      fiSize->Show();
    }
    return 1;
  }
private:
  CheckBox* cbStretch;
  FieldInt* fiSize;
  FieldGroup* fgStretch;
};

class PointMapByAttributeForm: public FormWithDest
{
public:
  PointMapByAttributeForm(PointMapDrawerForm* form)
  : FormWithDest(form, SDCTitleSymbolByAttribute)
  {
    frm = form;
    mdw = frm->mdw;
    tbl = mdw->mp->tblAtt();
    Domain dmCls;
    if (mdw->colCls.fValid()) {
      sClass = mdw->colCls->sName();
      dmCls = mdw->colCls->dm();
			if (mdw->_rpr.fValid())
				sRpr = mdw->_rpr->sName(true);
    }
    fRot  = mdw->colRot.fValid();
    if (fRot)
      sRot = mdw->colRot->sName();
    fClr  = mdw->colClr.fValid();
    if (fClr)
      sClr = mdw->colClr->sName();
    fStretch = mdw->colVal.fValid();
    if (fStretch)
      sSize = mdw->colVal->sName();
    fSize = frm->iSize > 0;
    iSize = abs(frm->iSize);
    fText = mdw->colTxt.fValid();
    if (fText)
      sText = mdw->colTxt->sName();
    fJust = mdw->colJst.fValid();
    if (fJust)
      sJust = mdw->colJst->sName();

    fcCls = new FieldColumn(root, SDCUiClass, tbl, &sClass, dmCLASS);
    fcCls->SetCallBack((NotifyProc)&PointMapByAttributeForm::ClassCallBack);
    fldRprClass = new FieldRepresentationC(root, SDCUiRpr, &sRpr, dmCls);

    CheckBox* cbRot = new CheckBox(root, SDCUiRotation, &fRot);
    new FieldColumn(cbRot, "", tbl, &sRot, dmVALUE);

    CheckBox* cbClr = new CheckBox(root, SDCUiColor, &fClr);
    cbClr->Align(cbRot, AL_UNDER);
    new FieldColumn(cbClr, "", tbl, &sClr, dmCOLOR|dmCLASS|dmPICT);

    cbStretch = new CheckBox(root, SDCUiStretch, &fStretch);
    cbStretch->Align(cbClr, AL_UNDER);
    cbStretch->SetCallBack((NotifyProc)&PointMapByAttributeForm::StretchCallBack);

    cbSize = new CheckBox(root, SDCUiSize, &fSize);
    cbSize->Align(cbStretch, AL_UNDER);
    new FieldInt(cbSize, "", &iSize, ValueRange(1L,250L));

    fgStretch = new FieldGroup(cbStretch);
    fcSize = new FieldColumn(fgStretch, "", tbl, &sSize, dmVALUE);
    fcSize->Align(cbStretch, AL_AFTER);
    fcSize->SetCallBack((NotifyProc)&PointMapByAttributeForm::ColValCallBack);

    frr = new FieldRangeReal(fgStretch, SDCUiStretch, &frm->rrStretch);
    frr->Align(cbStretch, AL_UNDER);

    FieldRangeInt* fri = new FieldRangeInt(fgStretch, SDCUiSize, &frm->riSize, ValueRange(1L,250L));

    RadioGroup* rgLinLog = new RadioGroup(fgStretch, "", &frm->iLinLog, true);
    rgLinLog->Align(fri, AL_UNDER);
    rgLinLog->SetIndependentPos();
    new RadioButton(rgLinLog, SDCUiLinear);
    new RadioButton(rgLinLog, SDCUiLogarithmic);
    RadioGroup* rgRadiusArea = new RadioGroup(fgStretch, "", &frm->iRadiusArea, true);
    rgRadiusArea->SetIndependentPos();
    new RadioButton(rgRadiusArea, SDCUiRadius);
    new RadioButton(rgRadiusArea, SDCUiArea);

    CheckBox* cbTxt = new CheckBox(root, SDCUiText, &fText);
    cbTxt->Align(rgRadiusArea, AL_UNDER);
    new FieldColumn(cbTxt, "", tbl, &sText, dmCLASS|dmIDENT|dmVALUE|dmSTRING);

//    CheckBox* cbJst = new CheckBox(root, SDCUiJustification, &fJust);
//    cbJst->Align(rgRadiusArea, AL_UNDER);
//    new FieldColumn(cbJst, "", tbl, &sJust,
    
    SetMenHelpTopic(htpCnfPointMapDrawerAttribute);
    create();
  }
  int ClassCallBack(Event*) {
    String sOld = sClass;
    fcCls->StoreData();
    if (sOld != sClass) {
      Column col = tbl->col(sClass);
      if (col.fValid()) {
        Domain dm = col->dm();
        fldRprClass->SetDomain(dm);
        Representation rpr = dm->rpr();
        if (rpr.fValid()) {
          sRpr = rpr->sName(true);
          fldRprClass->SetVal(sRpr);
        }
      }
    }
    return 1;
  }
  int StretchCallBack(Event*) {
    if (cbStretch->fVal()) {
      cbSize->Hide();
      fgStretch->Show();
    }
    else {
      fgStretch->Hide();
      cbSize->Show();
    }
    return 1;
  }
  int ColValCallBack(Event*) {
    String sOldSize = sSize;
    fcSize->StoreData();
    if (sOldSize != sSize) {
      Column col = tbl->col(sSize);
      frr->SetVal(col->rrMinMax());
    }
    return 1;
  }
  int exec() {
    FormWithDest::exec();
    mdw->colCls = tbl->col(sClass);
    mdw->_rpr = Representation(FileName(sRpr));
    if (fRot)
      mdw->colRot = tbl->col(sRot);
    else
      mdw->colRot = Column();
    if (fClr)
      mdw->colClr = tbl->col(sClr);
    else
      mdw->colClr = Column();
    if (fStretch)
      mdw->colVal = tbl->col(sSize);
    else
      mdw->colVal = Column();
    frm->fStretch = fStretch;
    if (!fStretch)
      if (fSize)
        frm->iSize = iSize;
      else
        frm->iSize = -iSize;
    if (fText)
      mdw->colTxt = tbl->col(sText);
    else
      mdw->colTxt = Column();
    if (fJust)
      mdw->colJst = tbl->col(sJust);
    else
      mdw->colJst = Column();
    return 0;
  }
private:
  PointMapDrawerForm* frm;
  PointMapDrawer* mdw;
  Table tbl;
  bool fRot, fClr, fStretch, fSize, fText, fJust;
  int iSize;
  String sClass, sRpr, sRot, sClr, sSize, sText, sJust;
  FieldColumn *fcCls, *fcSize;
  FieldRepresentationC* fldRprClass;
  FieldRangeReal* frr;
  CheckBox *cbStretch, *cbSize;
  FieldGroup* fgStretch;
};

class PointMapArrowForm: public FormWithDest
{
public:
  PointMapArrowForm(PointMapDrawerForm* frm)
  : FormWithDest(frm, SDCTitleSymbolAsArrow)
  {
    mdw = frm->mdw;
    tbl = mdw->mp->tblAtt();
    fClr = mdw->colClr.fValid();
    if (fClr)
      sClr = mdw->colClr->sName();
    fLength = mdw->colVal.fValid();
    if (fLength)
      sLength = mdw->colVal->sName();
    fWidth = mdw->colWidth.fValid();
    if (fWidth)
      sWidth = mdw->colWidth->sName();
    fText = mdw->colTxt.fValid();
    if (fText)
      sText = mdw->colTxt->sName();
    if (mdw->colRot.fValid())
      sRot = mdw->colRot->sName();
    CheckBox* cbColor = new CheckBox(root, SDCUiColor, &fClr);
    new FieldColumn(cbColor, "", tbl, &sClr, dmCLASS|dmCOLOR|dmPICT);
    CheckBox* cbLength = new CheckBox(root, SDCUiLengthMM, &fLength);
    cbLength->Align(cbColor, AL_UNDER);
    new FieldColumn(cbLength, "", tbl, &sLength, dmVALUE);
    CheckBox* cbWidth = new CheckBox(root, SDCUiLineWidthMM, &fWidth);
    new FieldColumn(cbWidth, "", tbl, &sWidth, dmVALUE);
    cbWidth->Align(cbLength, AL_UNDER);
    FieldColumn* fc = new FieldColumn(root, SDCUiDirection, tbl, &sRot, dmVALUE);
    fc->Align(cbWidth, AL_UNDER);
    CheckBox* cbTxt = new CheckBox(root, SDCUiText, &fText);
    cbTxt->Align(fc, AL_UNDER);
    new FieldColumn(cbTxt, "", tbl, &sText, dmCLASS|dmIDENT|dmVALUE|dmSTRING);

    SetMenHelpTopic(htpCnfPointMapDrawerArrow);
    create();
  }
  int exec() {
    FormWithDest::exec();
    mdw->colClr = tbl->col(sClr);
    mdw->colVal = tbl->col(sLength);
    if (fClr)
      mdw->colClr = tbl->col(sClr);
    else
      mdw->colClr = Column();
    if (fLength)
      mdw->colVal = tbl->col(sLength);
    else
      mdw->colVal = Column();
    if (fWidth)
      mdw->colWidth = tbl->col(sWidth);
    else
      mdw->colWidth = Column();
    mdw->colRot = tbl->col(sRot);
    if (fText)
      mdw->colTxt = tbl->col(sText);
    else
      mdw->colTxt = Column();
    return 1;
  }
private:
  PointMapDrawer* mdw;
  Table tbl;
  bool fClr, fLength, fWidth, fText;
  String sClr, sLength, sWidth, sRot, sText;
};


class PointMapGraphForm: public FormWithDest
{
public:
  PointMapGraphForm(PointMapDrawerForm* form)
  : FormWithDest(form, SDCTitleSymbolAsGraph)
  {
    frm = form;
    mdw = frm->mdw;
    tbl = mdw->mp->tblAtt();

    fStretch = mdw->colVal.fValid();
    if (fStretch)
      sSize = mdw->colVal->sName();
    iSize = abs(frm->iSize);
    fText = mdw->colTxt.fValid();
    if (fText)
      sText = mdw->colTxt->sName();

    switch (mdw->eST) {
      case PointMapDrawer::stPIECHART:
        iType = 0;
        break;
      case PointMapDrawer::stBARGRAPH:
        iType = 1;
        break;
      case PointMapDrawer::stGRAPH:
        iType = 2;
        break;
      case PointMapDrawer::stCOMPBAR:
        iType = 3;
        break;
      case PointMapDrawer::stVOLCUBES:
        iType = 4;
        break;
      default:
        iType = 0;
    }
    rgType = new RadioGroup(root, "", &iType);
    rgType->SetCallBack((NotifyProc)&PointMapGraphForm::TypeCallBack);
    RadioButton* rbPieChart = new RadioButton(rgType, SDCUiPieChart);
    RadioButton* rbBarGraph = new RadioButton(rgType, SDCUiBarGraph);
    RadioButton* rbGraph    = new RadioButton(rgType, SDCUiLineGraph);
    RadioButton* rbCompBar  = new RadioButton(rgType, SDCUiCompBar);
    RadioButton* rbVolCubes = new RadioButton(rgType, SDCUiVolCubes);

    clrLine = mdw->clrLine;
    fLineColor = true;
    if (-2 == (long)clrLine) {
      fLineColor = false;
      clrLine = -1;
    }
    // PieChart
    FieldGroup* fgPieChart = new FieldGroup(rbPieChart, true);
    fgPieChart->Align(rgType, AL_UNDER);

    cbPCStretch = new CheckBox(fgPieChart, SDCUiStretch, &fStretch);
    cbPCStretch->SetCallBack((NotifyProc)&PointMapGraphForm::PCStretchCallBack);

    fiPCSize = new FieldInt(fgPieChart, SDCUiSize, &iSize, ValueRange(1L,250L));
    fiPCSize->Align(cbPCStretch, AL_UNDER);

    fgPCStretch = new FieldGroup(cbPCStretch);
    fcPCSize = new FieldColumn(fgPCStretch, "", tbl, &sSize, dmVALUE);
    fcPCSize->Align(cbPCStretch, AL_AFTER);
    fcPCSize->SetCallBack((NotifyProc)&PointMapGraphForm::PCColValCallBack);

    frrPC = new FieldRangeReal(fgPCStretch, SDCUiStretch, &frm->rrStretch);
    frrPC->Align(cbPCStretch, AL_UNDER);

    FieldRangeInt* fri = new FieldRangeInt(fgPCStretch, SDCUiSize, &frm->riSize, ValueRange(1L,250L));

    RadioGroup* rgRadiusArea = new RadioGroup(fgPCStretch, "", &frm->iRadiusArea, true);
    rgRadiusArea->SetIndependentPos();
    new RadioButton(rgRadiusArea, SDCUiRadius);
    new RadioButton(rgRadiusArea, SDCUiArea);

    CheckBox* cbClr = new CheckBox(fgPieChart, SDCUiLineColor, &fLineColor);
    new FieldColor(cbClr, "", &clrLine);
//  new FieldMultiColumn(fgPieChart, SDCUiPieChart, tbl, mdw->cacarr, dmVALUE, true);

    // Bar Graph
    if (mdw->iWidth <= 0)
      mdw->iWidth = 10;
    FieldGroup* fgBarGraph = new FieldGroup(rbBarGraph, true);
    fgBarGraph->Align(rgType, AL_UNDER);
    new FieldRangeReal(fgBarGraph, SDCUiStretch, &mdw->rrStretch);
    new FieldInt(fgBarGraph, SDCUiHeightMM, &iSize, ValueRange(1L,250L));
    new FieldInt(fgBarGraph, SDCUiWidth, &mdw->iWidth, ValueRange(1L,50L));
    cbClr = new CheckBox(fgBarGraph, SDCUiLineColor, &fLineColor);
    new FieldColor(cbClr, "", &clrLine);
//  new FieldMultiColumn(fgBarGraph, SDCUiBarGraph, tbl, mdw->cacarr, dmVALUE, true);

    // Graph
    FieldGroup* fgGraph = new FieldGroup(rbGraph, true);
    fgGraph->Align(rgType, AL_UNDER);
    new FieldRangeReal(fgGraph, SDCUiStretch, &mdw->rrStretch);
    new FieldInt(fgGraph, SDCUiHeightMM, &iSize, ValueRange(1L,250L));
    new FieldInt(fgGraph, SDCUiWidth, &mdw->iWidth, ValueRange(1L,250L));
    col = frm->smb.color();
    new FieldColor(fgGraph, SDCUiLineColor, &col);
    new FieldColor(fgGraph, SDCUiAxesColor, &clrLine);
//  new FieldMultiColumn(fgGraph, SDCUiGraph, tbl, mdw->cacarr, dmVALUE, false);

    // Composed Bar
    FieldGroup* fgCompBar = new FieldGroup(rbCompBar, true);
    fgCompBar->Align(rgType, AL_UNDER);
    new FieldRangeReal(fgCompBar, SDCUiStretch, &mdw->rrStretch);
    new FieldInt(fgCompBar, SDCUiHeightMM, &iSize, ValueRange(1L,250L));
    new FieldInt(fgCompBar, SDCUiWidth, &mdw->iWidth, ValueRange(1L,250L));
    cbClr = new CheckBox(fgCompBar, SDCUiLineColor, &fLineColor);
    new FieldColor(cbClr, "", &clrLine);
//  new FieldMultiColumn(fgCompBar, SDCUiCompBar, tbl, mdw->cacarr, dmVALUE, true);

    // Volume Cubes
    FieldGroup* fgVolCubes = new FieldGroup(rbVolCubes, true);
    fgVolCubes->Align(rgType, AL_UNDER);
    new FieldRangeReal(fgVolCubes, SDCUiStretch, &mdw->rrStretch);
    new FieldInt(fgVolCubes, SDCUiSize, &iSize, ValueRange(1L,250L));
    new FieldColor(fgVolCubes, SDCUiLineColor, &clrLine);

    FieldBlank* fb = new FieldBlank(root,0.1f);
    fb->Align(fgPieChart, AL_UNDER);
    fmc = new FieldMultiColumn(root, "", tbl.ptr(), mdw->cacarr, dmVALUE, true);

    CheckBox* cbTxt = new CheckBox(root, SDCUiText, &fText);
    cbTxt->Align(fmc, AL_UNDER);
    cbTxt->SetIndependentPos();
    new FieldColumn(cbTxt, "", tbl, &sText, dmCLASS|dmIDENT|dmVALUE|dmSTRING);

    SetMenHelpTopic(htpCnfPointMapDrawerGraph);
    create();
  }
	BOOL DestroyWindow()
	{
		fmc->CleanUp();
		FormWithDest::DestroyWindow();
		return 0;
  }
  int exec() {
    FormWithDest::exec();
    switch (iType) {
      case 0: mdw->eST = PointMapDrawer::stPIECHART; break;
      case 1: mdw->eST = PointMapDrawer::stBARGRAPH; break;
      case 2: mdw->eST = PointMapDrawer::stGRAPH;    break;
      case 3: mdw->eST = PointMapDrawer::stCOMPBAR;  break;
      case 4: mdw->eST = PointMapDrawer::stVOLCUBES; break;
    }
    if (0 != iType)
      fStretch = false;
    if (2 == iType) {
      frm->smb.SetColor(col);
      fLineColor = true;
    }
    if (4 == iType)
      fLineColor = true;
    if (!fLineColor)
      clrLine = -2;
    mdw->clrLine = clrLine;
    if (fStretch)
      mdw->colVal = tbl->col(sSize);
    else
      mdw->colVal = Column();
    frm->fStretch = fStretch;
    frm->iSize = iSize;
    if (fText)
      mdw->colTxt = tbl->col(sText);
    else
      mdw->colTxt = Column();
		return 0;
  }
private:
  int TypeCallBack(Event*) {
    rgType->StoreData();
    fmc->ShowColor(iType != 2);
    return 1;
  }
  int PCStretchCallBack(Event*) {
    if (cbPCStretch->fVal()) {
      fiPCSize->Hide();
      fgPCStretch->Show();
    }
    else {
      fgPCStretch->Hide();
      fiPCSize->Show();
    }
    return 1;
  }
  int PCColValCallBack(Event*) {
    String sOldSize = sSize;
    fcPCSize->StoreData();
    if (sOldSize != sSize) {
      Column col = tbl->col(sSize);
      frrPC->SetVal(col->rrMinMax());
    }
    return 1;
  }

  PointMapDrawerForm* frm;
  PointMapDrawer* mdw;
  Table tbl;
  int iType, iSize;
  String sSize, sText;
  bool fStretch, fText;
  RadioGroup *rgType;
  FieldMultiColumn* fmc;
  CheckBox *cbPCStretch;
  FieldInt *fiPCSize;
  FieldGroup *fgPCStretch;
  FieldColumn *fcPCSize;
  FieldRangeReal *frrPC;
  Color col, clrLine;
  bool fLineColor;

//	DECLARE_MESSAGE_MAP();
};

//BEGIN_MESSAGE_MAP( PointMapGraphForm, FormWithDest )
	//{{AFX_MSG_MAP( PointMapGraphForm )

	//}}AFX_MSG_MAP
//END_MESSAGE_MAP()

PointMapDrawerForm::PointMapDrawerForm(PointMapDrawer* mdr, bool fShowForm)
: BaseMapDrawerForm(mdr, SDCTitlePntMap, fShowForm), mdw(mdr)
{
	iImg = IlwWinApp()->iImage(".mpp");

  CheckBox* cbMask = new CheckBox(root, SDCUiMask, &mdw->fMask);
  sMask = mdw->mask.sMask();
  FieldString* fs = new FieldString(cbMask, "", &sMask);
	fs->SetWidth(80);
  FieldBlank* fb = new FieldBlank(root, 0);
  fb->Align(cbMask, AL_UNDER);
  bool fTbl = bm->fTblAtt();
  if (fTbl) {
    InitAtt(true);
    cbAtt->SetCallBack((NotifyProc)&PointMapDrawerForm::AttCallBack);
    fcAtt->SetCallBack((NotifyProc)&PointMapDrawerForm::AttCallBack);
  }  
  CheckBox* cb = new CheckBox(root, SDCUiText, &mdw->fText);
  new PushButton(cb, SDCUiFont, (NotifyProc)&PointMapDrawerForm::SelectFont);
  if (fTbl)
    cb->Align(stAttDom, AL_UNDER);
  FieldGroup* fgText = new FieldGroup(cb);  
  fgText->Align(cb, AL_UNDER);
  new FieldColor(fgText, SDCUiTextColor, &mdw->clrText);  

  smb = mdw->smb;
  iSize = smb.iSize;
  if (fTbl || bm->dm()->pdvi() || bm->dm()->pdvr()) {  
    iValueOption = 0; //(int) mdw->drm;
//    fgValue = new CheckBox(root, SDCUiShowSymbol, &mdr->fSymbol);
//    fgValue = new StaticText(root, SDCUiShowSymbol);
//    FieldGroup* fg = new FieldGroup(fgValue);
    fgValue = new FieldGroup(root);
    fgValue->Align(fgText, AL_UNDER);
//    fg->Align(fgValue, AL_UNDER);
    fStretch = mdw->fStretch;
    iSize = mdw->iMinSize;
    riStretch = mdw->riStretch;
    if (riStretch.iHi() <= riStretch.iLo())
      riStretch.iHi() = riStretch.iLo() + 1;
    rrStretch = mdw->rrStretch;
    if (rrStretch.rHi() <= rrStretch.rLo())
      rrStretch.rHi() = rrStretch.rLo() + 1;
    riSize = RangeInt(abs(mdw->iMinSize), abs(mdw->iSize));
    if (riSize.iHi() <= iSize)
      riSize.iHi() = 2 * iSize;
    iLinLog = mdw->iLinLog;
    iRadiusArea = mdw->iRadiusArea;
    new PushButton(fgValue, SDCUiSymbol, (NotifyProc)&PointMapDrawerForm::ValueSymbol);
  }

  if (fTbl || bm->dm()->pdc()) {  
//    fgClass = new CheckBox(root, SDCUiShowSymbol, &mdr->fSymbol);
//    fgClass = new StaticText(root, SDCUiShowSymbol);
    iClassOption = (int) mdw->drm;
//    RadioGroup* rgClass = new RadioGroup(fgClass, "", &iClassOption);
    RadioGroup* rgClass = new RadioGroup(root, "", &iClassOption);
    fgClass = rgClass;
    fgClass->Align(fgText, AL_UNDER);
//    rgClass->Align(fgClass, AL_UNDER);
    InitRprClass(rgClass);
    RadioButton* rb = new RadioButton(rgClass, SDCUiSingleSymbol);
    FieldGroup* fg = new FieldGroup(rb);
    new PushButton(fg, SDCUiSymbol, (NotifyProc)&PointMapDrawerForm::Symbol);
  }
  if (fTbl || bm->dm()->pdid()) {  
//    fgID = new CheckBox(root, SDCUiShowSymbol, &mdr->fSymbol);
//    fgID = new StaticText(root, SDCUiShowSymbol);
//    fgID->Align(fgText, AL_UNDER);
    iIdentOption = (int) mdw->drm - 1;
//    FieldGroup* fgIdent = new FieldGroup(fgID);
    FieldGroup* fgIdent = new FieldGroup(root);
    fgID = fgIdent;
    fgID->Align(fgText, AL_UNDER);
//    fgIdent->Align(fgID, AL_UNDER);
    if (bm->fTblAtt()) {
      iSymbolType = 0;
      switch(mdw->drm) {
        case drmBYATTRIBUTE:
          iSymbolType = 1;
          break;
        case drmARROW:
          iSymbolType = 2;
          break;
        case drmGRAPH:
          iSymbolType = 3;
          break;
      }
      RadioGroup* rg = new RadioGroup(fgIdent, "", &iSymbolType);
      RadioButton* rb = new RadioButton(rg, SDCUiSingleSymbol);
      new PushButton(rb, SDCUiSymbol, (NotifyProc)&PointMapDrawerForm::Symbol);
      rb = new RadioButton(rg, SDCUiSmbByAttribute);
      new PushButton(rb, SDCUiDetails, (NotifyProc)&PointMapDrawerForm::ByAttribute);
      rb = new RadioButton(rg, SDCUiSmbArrow);
      new PushButton(rb, SDCUiDetails, (NotifyProc)&PointMapDrawerForm::Arrow);
      rb = new RadioButton(rg, SDCUiSmbGraph);
      new PushButton(rb, SDCUiDetails, (NotifyProc)&PointMapDrawerForm::Graph);
    }
    else
      new PushButton(fgIdent, SDCUiSymbol, (NotifyProc)&PointMapDrawerForm::Symbol);
  }
  if (fTbl || bm->dm()->pdbool()) {
    InitBool();
    if (fTbl)
      fgBool->Align(stAttDom, AL_UNDER);
  }
	if (fTbl) {
    fgColor = new FieldGroup(root);
    fgColor->Align(stAttDom, AL_UNDER);
    RadioGroup* rg = new RadioGroup(fgColor, "", (int*)&mdw->drc, true);
    InitColoring(rg);
	}
  SetMenHelpTopic(htpCnfPointMapDrawer);
  create();
}

int PointMapDrawerForm::AttCallBack(Event*)
{
  String sOldAtt = sAttCol;
  bool fOldAtt = fAtt;
  if (cbAtt) {
    cbAtt->StoreData();
    fldRprClass->AllowRprAttr(!fAtt);
  }  
  if (fAtt) {
    HCURSOR hCurSave = GetCursor();
    SetCursor(LoadCursor(NULL, IDC_WAIT));
    Column colAtt;
    try {
      Table tblAtt = bm->tblAtt();
      if (tblAtt.fValid())
        colAtt = tblAtt->col(sAttCol);
    }
    catch (ErrorObject& err) {
      err.Show();
    }  
    if (colAtt.fValid()) {
      dmUse = colAtt->dm();
      stAttDom->SetVal(dmUse->sTypeName());
      if (dmUse->pds())
        dmUse = dmMap;
      if ((!fOldAtt || sOldAtt != sAttCol) && 0 != dmUse->pdv()) {
        riStretch = colAtt->riMinMax();
        rrStretch = colAtt->rrMinMax();
      }
    }
    else {
      fAtt = false;
      sAttCol = "";
      stAttDom->SetVal(SDCRemNoValidCol);
    }
    SetCursor(hCurSave);
  }
  if (!fAtt)  
    dmUse = dmMap;
  if (fldRprClass)
    fldRprClass->SetDomain(dmUse);
  DomUseChanged();  
  return 0;
}

int PointMapDrawerForm::Symbol(Event*)
{
  PointMapSymbolForm frm(this);
  if (frm.fOkClicked())
    iSize = smb.iSize;
  return 1;
}

int PointMapDrawerForm::ValueSymbol(Event*)
{
  PointMapSymbolStretchForm frm(this);
  return 1;
}

int PointMapDrawerForm::ByAttribute(Event*)
{
  PointMapByAttributeForm frm(this);
  return 1;
}

int PointMapDrawerForm::Arrow(Event*)
{
  PointMapArrowForm frm(this);
  return 1;
}

int PointMapDrawerForm::Graph(Event*)
{
  PointMapGraphForm frm(this);
  return 1;
}

int PointMapDrawerForm::SelectFont(Event*)
{
	CWindowDC wdc(CWnd::GetDesktopWindow());
	LOGFONT lf;
	lf.lfHeight = MulDiv(mdw->iFHeight, GetDeviceCaps(wdc.GetSafeHdc(), LOGPIXELSY), 720);
	lf.lfWidth = 0;
	lf.lfEscapement = 0;
	lf.lfOrientation = 0;
	lf.lfWeight = mdw->iFWeight;
	lf.lfItalic =(BYTE)(mdw->iFStyle&FS_ITALIC) ;
	lf.lfUnderline = (BYTE)(mdw->iFStyle&FS_UNDERLINE);
	lf.lfStrikeOut = (BYTE)(mdw->iFStyle&FS_STRIKEOUT);
	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfOutPrecision = OUT_CHARACTER_PRECIS;
	lf.lfClipPrecision = CLIP_CHARACTER_PRECIS;
	lf.lfQuality = DEFAULT_QUALITY;
	lf.lfPitchAndFamily = VARIABLE_PITCH;
	strcpy(lf.lfFaceName, mdw->sFaceName.sVal());

    CHOOSEFONT chf;
//    zFontSpec *fs = new zFontSpec(mdw->sFaceName, zPrPoint(0,mdw->iFHeight,mdw->mppn->canvas()),
//                        mdw->iFWeight, ffDontCare, VariablePitch, mdw->iFStyle);
    chf.Flags = CF_INITTOLOGFONTSTRUCT;
    chf.iPointSize = mdw->iFHeight; // fs->pointSize();
    chf.lStructSize = sizeof(CHOOSEFONT);
    chf.hwndOwner = m_hWnd;
    chf.lpLogFont = &lf;
    chf.hDC = 0; //*mdw->mppn->prDisplay();
    chf.Flags |= CF_BOTH| CF_PRINTERFONTS;//|CF_SHOWHELP;
    chf.rgbColors = 0;
    chf.lCustData = 0;
    chf.lpfnHook = 0;
    chf.lpTemplateName = 0;
    chf.hInstance = 0;
    chf.lpszStyle = 0;
    chf.nFontType = 0;
    chf.nSizeMin = 0;
    chf.nSizeMax = 0;
//    BaseWindow* bw = (BaseWindow*)mdw->mppn->parent();
//    bw->SetDialogHelpTopic(htpChooseFont);
    if (ChooseFont(&chf))
    {
        mdw->iFHeight = lf.lfHeight;
        mdw->iFHeight = chf.iPointSize; // fs->pointSize();
        mdw->iFWeight = lf.lfWeight;
        mdw->iFStyle = lf.lfItalic    ? FS_ITALIC : 0  
					           | lf.lfStrikeOut ? FS_STRIKEOUT : 0
										 | lf.lfUnderline ? FS_UNDERLINE : 0;
        mdw->sFaceName = lf.lfFaceName;
    }
  return 1;
}

void PointMapDrawerForm::DomUseChanged()
{
  if (fgValue) fgValue->Hide();
  if (fgClass) fgClass->Hide();
  if (fgID) fgID->Hide();
  if (fgBool) fgBool->Hide();
  if (fgColor) fgColor->Hide();
  switch (dmtUse()) {
    case dmtCLASS:
      { Representation rpr;
        if (mdw->_dm == dmUse)
          rpr = mdw->rpr();
        if (!rpr.fValid())
          rpr = dmUse->rpr();
        if (rpr.fValid()) {
          sRprName = rpr->sName(true);
          fldRprClass->SetVal(sRprName);
        }  
      }    
      fgClass->Show();
      break;
    case dmtID:
      fgID->Show();
      break;
    case dmtVALUE:  
      fgValue->Show();
//      StretchCallBack(0);
      break;
    case dmtBOOL:
      fgBool->Show();
      break;
    case dmtCOLOR:
      fgColor->Show();
      break;
  }
}

int PointMapDrawerForm::exec()
{
  BaseMapDrawerForm::exec();
  if (mdw->fMask)
    mdw->mask.SetMask(sMask);
  mdw->smb = smb;
  if (iSize > 0) {
    mdw->smb.iSize = iSize;
    mdw->iSize = iSize;
	}
  mdw->fSymbol = true;
  switch (dmtUse()) {
    case dmtCLASS:
      mdw->drm = (DrawMethod) iClassOption;
      if (mdw->drm == drmRPR) {
        mdw->_rpr = Representation(FileName(sRprName));
        dmUse->SetRepresentation(mdw->_rpr);
//        if (mdw->_rpr->pra())
//          mdw->_rpr = Representation(mdw->_rpr, mdw->mp->tblAtt());
      }  
      break;
    case dmtID:
      mdw->drm = drmSINGLE;
      switch (iSymbolType) {
        case 0:
          mdw->drm = drmSINGLE;
					break;
        case 1:
          mdw->drm = drmBYATTRIBUTE;
          mdw->fStretch = fStretch;
          if (!fStretch) {
            mdw->iMinSize = iSize;
            mdw->iSize = iSize;
          }
          else {
            mdw->rrStretch = rrStretch;
            mdw->riStretch = riStretch;
            mdw->iMinSize = riSize.iLo();
            mdw->iSize = riSize.iHi();
            mdw->smb.iSize = riSize.iHi();
            mdw->iLinLog = iLinLog;
            mdw->iRadiusArea = iRadiusArea;
          }
          break;
        case 2:
          mdw->drm = drmARROW;
          break;
        case 3:
          mdw->drm = drmGRAPH;
          mdw->fStretch = fStretch;
          if (!fStretch) {
            mdw->iMinSize = iSize;
            mdw->iSize = iSize;
          }
          else {
            mdw->rrStretch = rrStretch;
            mdw->riStretch = riStretch;
            mdw->iMinSize = riSize.iLo();
            mdw->iSize = riSize.iHi();
            mdw->smb.iSize = riSize.iHi();
            mdw->iLinLog = 0;
            mdw->iRadiusArea = iRadiusArea;
          }
          break;
      }
      break;
    case dmtVALUE:
//      mdw->drm = (DrawMethod) iValueOption;
      mdw->drm = drmSINGLE;
      mdw->fStretch = fStretch;
      if (!fStretch) {
        mdw->iMinSize = iSize;
        mdw->iSize = mdw->iMinSize;
      }  
      else {
        mdw->rrStretch = rrStretch;
        mdw->riStretch = riStretch;
        mdw->iMinSize = riSize.iLo();
        mdw->iSize = riSize.iHi();
        mdw->smb.iSize = riSize.iHi();
        mdw->iLinLog = iLinLog;
        mdw->iRadiusArea = iRadiusArea;
      }  
//      mdw->smb.fillCol = mdw->clr;
      if (mdw->drm == drmRPR) {
        mdw->_rpr = Representation(FileName(sRprName));
        dmUse->SetRepresentation(mdw->_rpr);
      }  
      break;
    case dmtBOOL:
      mdw->drm = drmBOOL;
      break;
    case dmtCOLOR:
      mdw->drm = drmCOLOR;
      break;
  }
  if (mdw->drm == drmRPR) 
    if (!mdw->_rpr.fValid())
      mdw->drm = drmSINGLE;
  return 0;
}
  

