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
// CoordSysProjectionView.cpp : implementation of the CoordSysProjectionView class
// Created by Martin Schouwenburg 15-6-99
#include "Client\Headers\formelementspch.h"
#include "Client\ilwis.h"
#include "Headers\messages.h"
#include "Client\Base\IlwisDocument.h"
#include "Headers\Hs\Mainwind.hs"
#include "Client\Base\Res.h"
#include "Engine\Base\AssertD.h"
#include "Client\Forms\generalformview.h"
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Engine\SpatialReference\csviall.h"
#include "Engine\SpatialReference\Csproj.h"
#include "Engine\SpatialReference\DATUM.H"
#include "Client\Editors\CoordSystem\CSyDoc.h"
#include "Client\Editors\CoordSystem\CSyDoc.h"
#include "Client\Editors\CoordSystem\CsyView.h"
#include "Client\Editors\CoordSystem\CsyViaLatLonView.h"
#include "Client\Editors\CoordSystem\CsyProjectionView.h"
#include "Client\FormElements\fldprj.h"
#include "Client\FormElements\fentdms.h"
#include "Headers\Hs\Coordsys.hs"
#include "Headers\Hs\proj.hs"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CoordSysProjectionView

IMPLEMENT_DYNCREATE(CoordSysProjectionView, CoordSysViaLatLonView)

BEGIN_MESSAGE_MAP(CoordSysProjectionView, CoordSysViaLatLonView)
END_MESSAGE_MAP()


CoordSysProjectionView::CoordSysProjectionView() :
  stRemark(NULL)
{
	fShowCrdMinMax = false;
}

CoordSysProjectionView::~CoordSysProjectionView() 
{
}

void CoordSysProjectionView::CreateForm()
{
  CoordSysProjectionView::create();
}

void CoordSysProjectionView::create()
{
	fDefineCrdMinMax = true; // default assumption; check with lines below if we have CrdMinMax

	CoordSysViaLatLonView::create();

	CoordSystem cs = GetDocument()->CoordSys();
	
	Coord* cMin = &cs->cb.cMin;
	Coord* cMax = &cs->cb.cMax;
	if (cMin->fUndef() || cMax->fUndef() || cMax->x < cMin->x || cMax->y < cMin->y)
	{
		fDefineCrdMinMax = false;
		*cMin = *cMax = crdUNDEF;
	}
	cbDefineCrdMinMax = new CheckBox(root, SCSUiDefineCoordBounds, &fDefineCrdMinMax);
	cbDefineCrdMinMax->SetIndependentPos();
	cbDefineCrdMinMax->SetCallBack((NotifyProc)&CoordSysView::CrdMinMaxCallBack);
	fldCrdMin = new FieldCoord(cbDefineCrdMinMax, SCSUiMinXY, cMin);
	fldCrdMin->Align(cbDefineCrdMinMax, AL_UNDER);
	fldCrdMin->SetCallBack((NotifyProc)&CoordSysView::CallBack);
	fldCrdMax = new FieldCoord(cbDefineCrdMinMax, SCSUiMaxXY, cMax);
	fldCrdMax->Align(fldCrdMin, AL_UNDER);
	fldCrdMax->SetCallBack((NotifyProc)&CoordSysView::CallBack);
	
	CoordSystemProjection *csprj = cs->pcsProjection();
	ISTRUE(fINotEqual, csprj, (CoordSystemProjection*) NULL);
	
	FieldGroup* fg = 0;
	StaticText* st = 0;
	FieldGroup* fgButtons = 0;
	if (!csprj->fDataReadOnly()) 
	{
		fgButtons = new FieldGroup(root, true);
		fgButtons->Align(fldCrdMax, AL_UNDER);
		PushButton *pbPrj, *pbEll, *pbDat;
		pbPrj = new PushButton(fgButtons, SCSUiProj, (NotifyProc)&CoordSysProjectionView::ButtonProjection);
		if (csprj->prj.fValid() && csprj->prj->fEllipsoid()) {
			pbEll = new PushButton(fgButtons, SCSUiEll, (NotifyProc)&CoordSysViaLatLonView::ButtonEllipsoid);
			pbEll->Align(pbPrj, AL_AFTER);
			pbDat = new PushButton(fgButtons, SCSUiDatum, (NotifyProc)&CoordSysViaLatLonView::ButtonDatum);
			pbDat->Align(pbEll, AL_AFTER);
		}
	}
	if (fgButtons) {
		FieldBlank* fb = new FieldBlank(root, 0);
		fb->Align(fgButtons, AL_UNDER);
	}
	
	if (csprj->prj.fValid()) {
		Projection prj = csprj->prj;
		ShowProjInfo();
		ShowDatumEllInfo();
		//if (!fKeepProjectionParams)
		{
			int iF = 0, iI = 0, iR = 0;
			for (int i = 0; i < pvLAST; ++i) {
				ProjectionParamValue ppv = ProjectionParamValue(i);
				ProjectionParamType pp = prj->ppParam(ppv);
				if (pp == ppNONE)
					continue;
				String sParam = prj->sParamUiName(ppv);
				switch (pp) {
					case ppINT: {
						iValue[iI] = prj->iParam(ppv);
						if (csprj->fDataReadOnly()) {
							String s("%S: %li", sParam, iValue[iI]);
							st = new StaticText(root, s);
							st->psn->SetBound(0,0,0,0);
						}
						else
			
					new FieldInt(root, sParam, &iValue[iI]);
					
						iI++;
					} break;
					case ppZONE: {
						iValue[iI] = prj->iParam(ppv);
						if (csprj->fDataReadOnly()) {
							String s("%S: %li", sParam, iValue[iI]);
							st = new StaticText(root, s);
							st->psn->SetBound(0,0,0,0);
						}
						else {
							String sPrj = prj->sName();
							long iMaxZone = prj->iMaxZoneNr();
							fiZone = new FieldInt(root, sParam, &iValue[iI], ValueRange(1,iMaxZone));
							fiZone->SetCallBack((NotifyProc)&CoordSysProjectionView::ZoneCallBack);
							initRemark();
						}
						iI++;
					} break;
					case ppBOOL: {
						fValue[iF] = prj->iParam(ppv) != 0;
						if (csprj->fDataReadOnly()) {
							String s("%S: %s", sParam, fValue[iF]?"Yes":"No");
							st = new StaticText(root, s);
							st->psn->SetBound(0,0,0,0);
						}
						else
							(new CheckBox(root, sParam, &fValue[iF]))->SetIndependentPos();
						iF++;
					} break;
					case ppLAT:
						rValue[iR] = prj->rParam(ppv);
						if (csprj->fDataReadOnly()) {
							String s("%S: %S", sParam, LatLon::sLat(rValue[iR]));
							st = new StaticText(root, s);
							st->psn->SetBound(0,0,0,0);
						}
						else
							new FieldLat(root, sParam, &rValue[iR]);
						iR++;
						break;
					case ppLON:
						rValue[iR] = prj->rParam(ppv);
						if (csprj->fDataReadOnly()) {
							String s("%S: %S", sParam, LatLon::sLon(rValue[iR]));
							st = new StaticText(root, s);
							st->psn->SetBound(0,0,0,0);
						}
						else
							new FieldLon(root, sParam, &rValue[iR]);
						iR++;
						break;
					case ppREAL:
						rValue[iR] = prj->rParam(ppv);
						if (csprj->fDataReadOnly()) {
							String s("%S: %.3f", sParam, rValue[iR]);
							st = new StaticText(root, s);
							st->psn->SetBound(0,0,0,0);
						}
						else
							new FieldReal(root, sParam, &rValue[iR]);
						iR++;
						break;
					case ppSCALE:
						rValue[iR] = prj->rParam(ppv);
						if (csprj->fDataReadOnly()) {
							String s("%S: %.10f", sParam, rValue[iR]);
							st = new StaticText(root, s);
							st->psn->SetBound(0,0,0,0);
						}
						else
							new FieldReal(root, sParam, &rValue[iR], ValueRange(0.1,9.9,1e-10));
						iR++;
						break;
					case ppPOSREAL: 
						rValue[iR] = prj->rParam(ppv);
						if (csprj->fDataReadOnly()) {
							String s("%S: %.3f", sParam, rValue[iR]);
							st = new StaticText(root, s);
							st->psn->SetBound(0,0,0,0);
						}
						else
							new FieldReal(root, sParam, &rValue[iR], ValueRange(1e-9,1e20,0));
						iR++;
						break;
					case ppANGLE: 
						rValue[iR] = prj->rParam(ppv);
						if (csprj->fDataReadOnly()) {
							String s("%S: %.3f", sParam, rValue[iR]);
							st = new StaticText(root, s);
							st->psn->SetBound(0,0,0,0);
						}
						else
							new FieldReal(root, sParam, &rValue[iR], ValueRange(-90,90,0));
						iR++;
						break;
					case ppACUTANGLE: 
						rValue[iR] = prj->rParam(ppv);
						if (csprj->fDataReadOnly()) {
							String s("%S: %.3f", sParam, rValue[iR]);
							st = new StaticText(root, s);
							st->psn->SetBound(0,0,0,0);
						}
						else
							new FieldReal(root, sParam, &rValue[iR], ValueRange(0,90,0));
						iR++;
						break;  
				}
			}
		}
	}
	SetMenHelpTopic(htpCSEditProj);
}

void CoordSysProjectionView::ShowProjInfo()
{
	CoordSystemProjection *csprj = GetDocument()->CoordSys()->pcsProjection();
	ISTRUE(fINotEqual, csprj, (CoordSystemProjection*) NULL);


  FieldGroup* fg = new FieldGroup(root, true);
  Projection prj = csprj->prj;
  String sPrjName = prj->sName();
  String s = SCSInfProj;
  s &= sPrjName;
  StaticText* st = new StaticText(fg, s);

  int iSize = 100;
  char sBuf[100];
  String sPath = IlwWinApp()->Context()->sIlwDir();
  sPath &= "\\Resources\\Def\\projs.def";
  GetPrivateProfileString("Projections", sPrjName.scVal(), "", sBuf, iSize, sPath.scVal());
  if (*sBuf) 
	{
    st->psn->SetBound(0,0,0,0);
    new StaticText(fg, sBuf);
  }
}

int CoordSysProjectionView::ButtonProjection(Event*)
{
  class ChangePrjForm: public FormWithDest 
	{
  public:
    ChangePrjForm(CWnd* w, String* sPrj) 
    : FormWithDest(w, SCSTitleSelectProj)
    {
      new StaticText(root, SCSUiProj);
      new FieldProjection(root, sPrj);
      SetMenHelpTopic(htpChangePrj);
      create();
    }
  };
  //root->StoreData();

	CoordSystemProjection *csprj = GetDocument()->CoordSys()->pcsProjection();
	ISTRUE(fINotEqual, csprj, (CoordSystemProjection*) NULL);

	Projection prj = csprj->prj;
  String sPrj = SCSUnknownPrj;
  if (prj.fValid())
    sPrj = prj->sName();
  ChangePrjForm frm(this, &sPrj);
  if (frm.fOkClicked()) 
	{
		DataHasChanged(true);
		fbs |= fbsNOCANCELBUTTON; // remove cancel button
		exec(); // store prj and and other parameters!
		try {
      prj = Projection(sPrj);
		}
		catch (ErrorObject&) {
			prj = Projection(); //in case of "UNKNOWN" projection
		}											// an invalid prj is created 
		csprj->prj = prj;
		if (prj.fValid()) 
		{
			Datum *datum = prj->datumDefault();
			if (datum) 
			{
				if (csprj->datum)
					delete csprj->datum;
				csprj->datum = datum;
				if (csprj->datum)
					csprj->ell = csprj->datum->ell;
			}
			else
				datum = csprj->datum;
		}
		if (!prj.fValid() || !prj->fEllipsoid()) 
		{
			csprj->ell = Ellipsoid();
			if (csprj->datum) 
			{
				delete csprj->datum;
				csprj->datum = 0;
			}
		}
		csprj->Updated();
  	PostMessage(ILWM_RECALCULATEFORM);
		return 1;
  }
  return 0;
}

int CoordSysProjectionView::ZoneCallBack(Event*)
{
	fiZone->StoreData();
	Coord cMin = fldCrdMin->crdVal();
    Coord cMax = fldCrdMax->crdVal();
	bool fValidCrdBounds 
		= (cMin.x < cMax.x && cMin.y < cMax.y)||(cMin.fUndef() && cMax.fUndef());
	CoordSystemProjection *csprj = GetDocument()->CoordSys()->pcsProjection();
	Projection prj = csprj->prj;
	String sPrj = prj->sName();
	long iMaxZone = prj->iMaxZoneNr();
	iZoneNr = fiZone->iVal();
	if (iZoneNr <= 0 || iZoneNr > iMaxZone)
	{
		String sRem(SPRJErrWrongZoneNumber_i.sVal(), iMaxZone);
			//String sRem = prj->sInvalidZone();
		stRemark->SetVal(sRem);
		DisableOK();
	}
	else
	{
		DataHasChanged(true);
		stRemark->SetVal("");
		if (fValidCrdBounds)
			EnableOK();
	}
	return 0; 
}

int CoordSysProjectionView::exec()
{
	if ( !fDataHasChanged() ) return 0;

  CoordSysViaLatLonView::exec();

	CoordSystemProjection *csprj = GetDocument()->CoordSys()->pcsProjection();
	ISTRUE(fINotEqual, csprj, (CoordSystemProjection*) NULL);

  if (csprj->prj.fValid()) {
    Projection prj = csprj->prj;
    int iF = 0, iI = 0, iR = 0;
    try {
      for (int i = 0; i < pvLAST; ++i) {
        ProjectionParamValue ppv = ProjectionParamValue(i);
        ProjectionParamType pp = prj->ppParam(ppv);
        switch (pp) {
          case ppINT:
					case ppZONE:
            prj->Param(ppv, iValue[iI++]);
            break;
          case ppBOOL:
            prj->Param(ppv, (long)fValue[iF++]);
            break;
          case ppLAT:
          case ppLON:
          case ppREAL:
          case ppSCALE:
          case ppPOSREAL:
          case ppANGLE:
          case ppACUTANGLE:
            prj->Param(ppv, rValue[iR++]);
            break;
        }
      }
			csprj->Store();
      prj->Prepare();
    }
    catch (ErrorObject& err) {
      err.Show();
    }
  }
	if (!fDefineCrdMinMax)
	{
			csprj->cb = CoordBounds(crdUNDEF, crdUNDEF);
	}
  return 0;
}

void CoordSysProjectionView::initRemark()
{
  String sFill('x', 60);
  stRemark = new StaticText(root, sFill);
  stRemark->SetIndependentPos();
}

#ifdef _DEBUG
void CoordSysProjectionView::AssertValid() const
{
	CoordSysViaLatLonView::AssertValid();
}

void CoordSysProjectionView::Dump(CDumpContext& dc) const
{
	CoordSysViaLatLonView::Dump(dc);
}

#endif //_DEBUG

