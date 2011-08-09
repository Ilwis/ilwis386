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
// CoordSysFormulaView.cpp : implementation of the CoordSysFormulaView class
// Created by Martin Schouwenburg 15-6-99
#include "Client\Headers\formelementspch.h"
#include "Client\ilwis.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\Forms\generalformview.h"
#include "Headers\Hs\Mainwind.hs"
#include "Client\Base\Res.h"
#include "Engine\Base\AssertD.h"
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Engine\SpatialReference\csformul.h"
#include "Client\Editors\CoordSystem\CSyDoc.h"
#include "Client\Editors\CoordSystem\CsyView.h"
#include "Client\Editors\CoordSystem\CsyFormulaView.h"
#include "Headers\Hs\Coordsys.hs"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CoordSysFormulaView

IMPLEMENT_DYNCREATE(CoordSysFormulaView, CoordSysView)

BEGIN_MESSAGE_MAP(CoordSysFormulaView, CoordSysView)
END_MESSAGE_MAP()


CoordSysFormulaView::CoordSysFormulaView()
{
	fShowCrdMinMax = false;
}

CoordSysFormulaView::~CoordSysFormulaView()
{
}

void CoordSysFormulaView::CreateForm()
{
	CoordSysFormulaView::create();
}

void CoordSysFormulaView::create()
{
	fDefineCrdMinMax = true; // default assumption; check with lines below if we have CrdMinMax

	CoordSysView::create();

	CoordSystem cs = GetDocument()->CoordSys();

	Coord* cMin = &cs->cb.cMin;
	Coord* cMax = &cs->cb.cMax;
	if (cMin->fUndef() || cMax->fUndef() || cMax->x < cMin->x || cMax->y < cMin->y)
	{
		fDefineCrdMinMax = false;
		*cMin = *cMax = crdUNDEF;
	}
	cbDefineCrdMinMax = new CheckBox(root, TR("Define Coordinate Boundaries"), &fDefineCrdMinMax);
	cbDefineCrdMinMax->SetIndependentPos();
	cbDefineCrdMinMax->SetCallBack((NotifyProc)&CoordSysView::CrdMinMaxCallBack);
	fldCrdMin = new FieldCoord(cbDefineCrdMinMax, TR("&Min X, Y"), cMin);
	fldCrdMin->Align(cbDefineCrdMinMax, AL_UNDER);
	fldCrdMin->SetCallBack((NotifyProc)&CoordSysView::CallBack);
	fldCrdMax = new FieldCoord(cbDefineCrdMinMax, TR("&Max X, Y"), cMax);
	fldCrdMax->Align(fldCrdMin, AL_UNDER);
	fldCrdMax->SetCallBack((NotifyProc)&CoordSysView::CallBack);

	CoordSystemFormula* csf = cs->pcsFormula();
	ISTRUE(fINotEqual, csf, (CoordSystemFormula*) NULL);

	PushButton* pb = new PushButton(cbDefineCrdMinMax, TR("&Defaults"),
                   (NotifyProc)&CoordSysFormulaView::SetDefaults);
  pb->Align(fldCrdMin, AL_AFTER);

  FieldGroup* fg = new FieldGroup(root, true);
  fg->Align(fldCrdMax, AL_UNDER);
  sCsy = csf->csOther->sName();
  fcs = new FieldCoordSystem(fg, TR("&Related CoordSystem"), &sCsy);
  fcs->SetCallBack((NotifyProc)&CoordSysFormulaView::CallBackCsy);
  String sFill('x', 60);
  stBounds = new StaticText(fg, sFill);
  stBounds->SetIndependentPos();
  stBounds->psn->SetBound(0,0,0,0);
  stRemark = new StaticText(fg, sFill);
  stRemark->SetIndependentPos();

  FieldGroup* fgOrigs = new FieldGroup(fg);
  new FieldCoord(fgOrigs, TR("&Related Origin"), &csf->cOther0);
  new FieldCoord(fgOrigs, TR("&Target Origin"), &csf->c0);

  iUserDef = CoordSystemFormula::eUserDef == csf->eTransf ? 1 : 0;
  RadioGroup* rgUserDef = new RadioGroup(fg, "", &iUserDef, true);
  rgUserDef->SetIndependentPos();
  RadioButton* rbTransf = new RadioButton(rgUserDef, TR("&Affine Transformation"));
  RadioButton* rbUserDef = new RadioButton(rgUserDef, TR("&User Defined Expression"));

  FieldGroup* fgUserDef = new FieldGroup(rbUserDef, true);
  fgUserDef->Align(rgUserDef, AL_UNDER);
  FieldString* fs = new FieldString(fgUserDef, TR("out &X ="), &csf->sExprX);
  fs->SetWidth(130);
  fs = new FieldString(fgUserDef, TR("out &Y ="), &csf->sExprY);
  fs->SetWidth(130);
  fInverse = 0 != csf->sExprInvX[0];
  CheckBox* cbInverse = new CheckBox(fgUserDef, TR("&Inverse Formulae"), &fInverse);
  cbInverse->SetIndependentPos();
  FieldGroup* fgInverse = new FieldGroup(cbInverse);
  fgInverse->Align(cbInverse, AL_UNDER);
  fs = new FieldString(fgInverse, TR("inv X ="), &csf->sExprInvX);
  fs->SetWidth(130);
  fs = new FieldString(fgInverse, TR("inv Y ="), &csf->sExprInvY);
  fs->SetWidth(130);

  iTransf = csf->eTransf;
  if (1 == iUserDef)
    iTransf = 0;
  rg = new RadioGroup(rbTransf, "", &iTransf);
  rg->Align(rgUserDef, AL_UNDER);
  RadioButton* rbConformal = new RadioButton(rg, TR("&Conformal"));
  RadioButton* rbDiffScaling = new RadioButton(rg, TR("&Differential Scaling"));
  RadioButton* rbSkewX = new RadioButton(rg, TR("Skew along &X-axis"));
  RadioButton* rbSkewY = new RadioButton(rg, TR("Skew along &Y-axis"));
  RadioButton* rbAffine = new RadioButton(rg, TR("&Matrix Coefficients"));

  FieldGroup* fgConformal = new FieldGroup(rbConformal, true);
  fgConformal->Align(rbConformal, AL_AFTER);
  new FieldReal(fgConformal, TR("&Scaling"), &csf->rScale, ValueRange(1e-12,1e12,0));
  new FieldReal(fgConformal, TR("&Rotation"), &csf->rRot, ValueRange(-180,360,0.1));

  FieldGroup* fgDiffScaling = new FieldGroup(rbDiffScaling, true);
  fgDiffScaling->Align(rbDiffScaling, AL_AFTER);
  new FieldReal(fgDiffScaling, TR("&X-Scaling"), &csf->rScaleX, ValueRange(1e-12,1e12,0));
  new FieldReal(fgDiffScaling, TR("&Y-Scaling"), &csf->rScaleY, ValueRange(1e-12,1e12,0));

  FieldGroup* fgSkewX = new FieldGroup(rbSkewX, true);
  fgSkewX->Align(rg, AL_AFTER);
  new FieldReal(rbSkewX, "", &csf->rSkewX, ValueRange(-89.99,89.99,0.01));

  FieldGroup* fgSkewY = new FieldGroup(rbSkewY, true);
  fgSkewY->Align(rbSkewY, AL_AFTER);
  new FieldReal(fgSkewY, "", &csf->rSkewY, ValueRange(-89.99,89.99,0.01));

  FieldGroup* fgAffine = new FieldGroup(rbAffine, true);
//  fgAffine->Align(rbDiffScaling, AL_AFTER);
  FieldReal* frA11 = new FieldReal(fgAffine, TR("a11"), &csf->a11);
  frA11->Align(rbDiffScaling, AL_AFTER);
  FieldReal* frA12 = new FieldReal(fgAffine, TR("a12"), &csf->a12);
  frA12->Align(frA11, AL_UNDER);
  FieldReal* frA21 = new FieldReal(fgAffine, TR("a21"), &csf->a21);
  frA21->Align(frA12, AL_UNDER);
  FieldReal* frA22 = new FieldReal(fgAffine, TR("a22"), &csf->a22);
  frA22->Align(frA21, AL_UNDER);

  SetMenHelpTopic("ilwismen\\edit_coordinate_system_formula.htm");
}

int CoordSysFormulaView::SetDefaults(Event*)
{
	CoordSystemFormula* csf = GetDocument()->CoordSys()->pcsFormula();
	ISTRUE(fINotEqual, csf, (CoordSystemFormula*) NULL);
  try {
    root->StoreData();
    if (csf->sExprX.sTrimSpaces()=="" || csf->sExprY.sTrimSpaces()=="")
        throw ErrorObject(TR("Formula missing"));
    csf->csOther = CoordSystem(sCsy);
    csf->eTransf =  CoordSystemFormula::csfTransf(iTransf);
    if (1 == iUserDef)
      csf->eTransf =CoordSystemFormula::eUserDef;
    csf->Calc();
    CoordBounds cb = csf->csOther->cb;
		if (!cb.fUndef())
			cb = csf->cbConv(csf->csOther, cb);
		else
			cb = CoordBounds(Coord(),Coord()); // not CoordBounds() coz that would be displayed
    fldCrdMin->SetVal(cb.cMin);
    fldCrdMax->SetVal(cb.cMax);
    csf->init();
  }
  catch (ErrorObject& err) {
    err.Show();
    csf->init();
  }
  return 0;
}

int CoordSysFormulaView::CallBackCsy(Event*)
{
  try {
    fcs->StoreData();
    CoordSystem cs(sCsy);
    String sBnds("%S %S %S",
                cs->sValue(cs->cb.cMin),
                TR("until"), 
                cs->sValue(cs->cb.cMax));
    stBounds->SetVal(sBnds);
    stRemark->SetVal(cs->sDescr());
		DataHasChanged(true);
		return 1;
  }
  catch (ErrorObject& ) {
    stBounds->SetVal("");
    stRemark->SetVal("");
  }

	return 0;
}

int CoordSysFormulaView::exec()
{
	if ( !fDataHasChanged() ) return 0;
  CoordSysView::exec();

	CoordSystemFormula* csf = GetDocument()->CoordSys()->pcsFormula();
	ISTRUE(fINotEqual, csf, (CoordSystemFormula*) NULL);

  try {
    if (csf->sExprX.sTrimSpaces()=="" || csf->sExprY.sTrimSpaces()=="")
        throw ErrorObject(TR("Formula missing"));
    csf->csOther = CoordSystem(sCsy);
    csf->eTransf =  CoordSystemFormula::csfTransf(iTransf);
    if (1 == iUserDef) {
      csf->eTransf = CoordSystemFormula::eUserDef;
      if (!fInverse) {
        csf->sExprInvX = String();
        csf->sExprInvY = String();
      }
    }
    csf->Calc();
		csf->Store();
  }
  catch (ErrorObject& err) {
    err.Show();
  }
	if (!fDefineCrdMinMax)
	{
		csf->cb = CoordBounds(crdUNDEF, crdUNDEF);
	}
  return 0;
}

#ifdef _DEBUG
void CoordSysFormulaView::AssertValid() const
{
	CoordSysView::AssertValid();
}

void CoordSysFormulaView::Dump(CDumpContext& dc) const
{
	CoordSysView::Dump(dc);
}


#endif //_DEBUG

