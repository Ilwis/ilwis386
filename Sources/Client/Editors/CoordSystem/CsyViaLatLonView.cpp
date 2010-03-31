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
// CoordSysViaLatLonView.cpp : implementation of the CoordSysViaLatLonView class
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
#include "Client\FormElements\fldprj.h"
#include "Headers\Hs\Coordsys.hs"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CoordSysViaLatLonView

IMPLEMENT_DYNCREATE(CoordSysViaLatLonView, CoordSysView)

BEGIN_MESSAGE_MAP(CoordSysViaLatLonView, CoordSysView)
	ON_MESSAGE(ILWM_RECALCULATEFORM, OnRecalcForm)	
END_MESSAGE_MAP()

const double rad2sec = 206264.80624709635515647335733078;//amount of arcseconds per radian

CoordSysViaLatLonView::CoordSysViaLatLonView()
{
	eAction = csvllOK;
  rRadius = 6371007.0;  // sphere with equal area as WGS 84
}

CoordSysViaLatLonView::~CoordSysViaLatLonView()
{
}

void CoordSysViaLatLonView::create()
{
	fShowCrdMinMax = true;
	CoordSysView::create();
}

void CoordSysViaLatLonView::ShowDatumEllInfo()
{
  StaticText* st = 0;
	CoordSystemViaLatLon* csvll = GetDocument()->CoordSys()->pcsViaLatLon();
	ISTRUE(fINotEqual, csvll, (CoordSystemViaLatLon*) NULL);
	short wid = csvll->iWidth();// undefined ellipsoid if wid == -1
		//it can be any sphre or ellipsoid (typical for \system\latlon.csy)
	if (wid == -1) return;

  if (csvll->datum) 
	{
			String s= SCSInfDatum;
			s &= csvll->datum->sName();
			st = new StaticText(root, s);
			st->SetIndependentPos();
			if (csvll->datum->sArea.length()) 
			{
				st->psn->SetBound(0,0,0,0);
				s = SCSInfDatumArea;
				s &= csvll->datum->sArea;
				st = new StaticText(root, s);
				st->SetIndependentPos();
			}
  }
  if (csvll->ell.fSpherical()) 
	{
    rRadius = csvll->ell.a;
    if (rRadius < 0.001){
				Ellipsoid* ell = new Ellipsoid("Sphere");// authalic sphere
				rRadius = ell->a;//was: 6371007, now: 6371007.1809185;// sphere with equal area as WGS 84
		}
    if (csvll->fDataReadOnly()) 
		{
      if (st)
        st->psn->SetBound(0,0,0,0);
      String s("%S %.7f", SCSUiSphereRadius, rRadius);
      st = new StaticText(root, s);
      st->SetIndependentPos();
    }
    else
      new FieldReal(root, SCSUiSphereRadius, &rRadius, ValueRange(0.001,1e40,0.0000001));
  }
  else 
	{
    if (st)
      st->psn->SetBound(0,0,0,0);
    String s = SCSInfEll;
    s &= csvll->ell.sName;
    st = new StaticText(root, s);
    st->SetIndependentPos();
	st->psn->SetBound(0,0,0,0);
	double a = csvll->ell.a;
	double f_inv = 1 / csvll->ell.f;
	s = String(SCSInfEllipsoid_a_and_1f_ff.scVal(), a, f_inv);
	st = new StaticText(root, s);
    st->SetIndependentPos();
  }
}

long CoordSysViaLatLonView::OnRecalcForm(WPARAM wParm, LPARAM lPar)
{
	RecalculateForm();
	return 1;
}

int CoordSysViaLatLonView::ButtonDatum(Event*)
{
  struct UserDefDatumInfo
  {
    UserDefDatumInfo()
    {
      iDatumType = 0;
      dx = dy = dz = 0;
      rotX = rotY = rotZ = 0;
      dS = 0;
      x0 = y0 = z0 = 0;
    }
    int iDatumType; // 0: Molodensky, 1: BursaWolf, 2: Badekas
    double dx, dy, dz;
  	double rotX, rotY, rotZ, dS;
    double x0, y0, z0;
  };
  class ChangeDatumForm: public FormWithDest 
	{
		public:
			ChangeDatumForm(CWnd* w, String* sDatum, String* sArea, UserDefDatumInfo* info) 
			: FormWithDest(w, SCSTitleSelectDatum)
      , datumInfo(info)
			{
				StaticText *st = new StaticText(root, SCSUiDatum);
		
        iType = fCIStrEqual("User Defined" , *sDatum) ? 1 : 0;
        RadioGroup* rg = new RadioGroup(root, "", &iType);
        RadioButton* rbPreDef = new RadioButton(rg, SCSUiPreDefined);
        RadioButton* rbUserDef = new RadioButton(rg, SCSUiUserDefined);
        
        FieldGroup* fgUserDef = new FieldGroup(rbUserDef);
        fgUserDef->Align(rbUserDef, AL_UNDER);
        new FieldBlank(fgUserDef);
        rgDatumType = new RadioGroup(fgUserDef, "", &datumInfo->iDatumType);
				rbUserDef->SetCallBack((NotifyProc)&ChangeDatumForm::DatumTypeChanged);
				rgDatumType->SetCallBack((NotifyProc)&ChangeDatumForm::DatumTypeChanged);
        new RadioButton(rgDatumType, "&Molodensky");
        new RadioButton(rgDatumType, "Bursa &Wolf");
        new RadioButton(rgDatumType, "&Badekas");

        frDX = new FieldReal(fgUserDef, "d&X  (m)", &datumInfo->dx, ValueRange(-2e3, 2e3, 0.001));
        frDX->Align(rbPreDef, AL_AFTER);
        frDY = new FieldReal(fgUserDef, "d&Y  (m)", &datumInfo->dy, ValueRange(-2e3, 2e3, 0.001));
        frDZ = new FieldReal(fgUserDef, "d&Z  (m)", &datumInfo->dz, ValueRange(-2e3, 2e3, 0.001));
        frRotX = new FieldReal(fgUserDef, "rot X (\")", &datumInfo->rotX, ValueRange(-2e3, 2e3, 0.00001));
        frRotY = new FieldReal(fgUserDef, "rot Y (\")", &datumInfo->rotY, ValueRange(-2e3, 2e3, 0.00001));
        frRotZ = new FieldReal(fgUserDef, "rot Z (\")", &datumInfo->rotZ, ValueRange(-2e3, 2e3, 0.00001));
        frDS = new FieldReal(fgUserDef, "dS (ppm)", &datumInfo->dS, ValueRange(-2e3, 2e3, 0.00001));
        frX0 = new FieldReal(fgUserDef, "Xo  (m)", &datumInfo->x0, ValueRange(-2e7, 2e7, 0.001));
        frY0 = new FieldReal(fgUserDef, "Yo  (m)", &datumInfo->y0, ValueRange(-2e7, 2e7, 0.001));
        frZ0 = new FieldReal(fgUserDef, "Zo  (m)", &datumInfo->z0, ValueRange(-2e7, 2e7, 0.001));

        FieldGroup* fgPreDef = new FieldGroup(rbPreDef);
        fgPreDef->Align(rbUserDef, AL_UNDER);
        fgPreDef->SetIndependentPos();
				fldDatum = new FieldDatum(fgPreDef, sDatum);
				fldDatum->SetCallBack((NotifyProc)&ChangeDatumForm::DatumChanged);
				stArea = new StaticText(fgPreDef, SCSUiArea);
				stArea->psn->SetBound(0,0,0,0);
				fldDatumArea = new FieldDatumArea(fgPreDef, sArea);
				fldDatumArea->SetCallBack((NotifyProc)&ChangeDatumForm::AreaChanged);
				String sFill('X', 50);
				stRemark1 = new StaticText(fgPreDef, sFill);
				stRemark1->psn->SetBound(0,0,0,0);
				stRemark2 = new StaticText(fgPreDef, sFill);
				stRemark2->psn->SetBound(0,0,-5,0);
				stRemark3 = new StaticText(fgPreDef, sFill);
				stRemark3->psn->SetBound(0,0,-5,0);
        
				SetMenHelpTopic(htpChangeDatum);
				create();
			}
      int iType;
		private:
			int DatumChanged(Event*) 
			{
        if (!IsWindowVisible()) 
          return 0;
				String sDatum = fldDatum->sValue();
				if (fldDatumArea->fInit(sDatum)) 
					stArea->Show();
				else 
					stArea->Hide();
				return AreaChanged(0);
			}
			void SetRemark(const String& strng) 
			{
				char *s0, *sTmp;
				char str[1000];
				strcpy(str, strng.scVal());
				s0 = sTmp = str;
				if (0 == str || strlen(str) < 70)
					s0 = 0;
				else {  
					while (sTmp && sTmp - str < 70) {
						s0 = sTmp++;
						sTmp = strchr(sTmp, ' ');
					}  
					*s0++ = 0;  
				}    
				String sTrimmed1(str);
				sTrimmed1 = sTrimmed1.sTrimSpaces();
				stRemark1->SetVal(sTrimmed1);
      
				strcpy(str ,s0 != 0 ? s0 : "");
				if (0 == str || strlen(str) < 70)
					s0 = 0;
				else {  
					s0 = sTmp;
					while (sTmp && sTmp - str < 70) {
						s0 = sTmp++;
						sTmp = strchr(sTmp, ' ');
					}  
					*s0++ = 0;  
				} 
				String sTrimmed2(str);
				sTrimmed2 = sTrimmed2.sTrimSpaces();
				stRemark2->SetVal(sTrimmed2);
				strcpy(str,s0 != 0 ? s0 : "");
				String sTrimmed3(str);
				sTrimmed3 = sTrimmed3.sTrimSpaces();
				stRemark3->SetVal(sTrimmed3);
			}
			int AreaChanged(Event*) 
			{
        if (!IsWindowVisible()) {
					stRemark1->SetVal("");
					stRemark2->SetVal("");
					stRemark3->SetVal("");
          return 0;
        }
				String sDatum = fldDatum->sValue();
				String sArea = fldDatumArea->sValue();
				if (sDatum != "" && sDatum != SCSDatumNotSpecified)
        {
					 MolodenskyDatum dat(sDatum, sArea);
  				 SetRemark(dat.sDescription);
				}
				else 
				{
					stRemark1->SetVal("");
					stRemark2->SetVal("");
					stRemark3->SetVal("");
				}  
				return 0;
			}
			int DatumTypeChanged(Event*) 
      {
        if (!rgDatumType->fShow())
          return 0;
        rgDatumType->StoreData();
        if (datumInfo->iDatumType < 1)
        {
          frRotX->Hide();
          frRotY->Hide();
          frRotZ->Hide();
          frDS->Hide();
        }
        else
        {
          frRotX->Show();
          frRotY->Show();
          frRotZ->Show();
          frDS->Show();
        }
        if (datumInfo->iDatumType < 2)
        {
          frX0->Hide();
          frY0->Hide();
          frZ0->Hide();
        }
        else
        {
          frX0->Show();
          frY0->Show();
          frZ0->Show();
        }
        return 0;
      }
      UserDefDatumInfo* datumInfo;
			FieldDatum* fldDatum;
			FieldDatumArea* fldDatumArea;  
			StaticText *stArea, *stRemark1, *stRemark2, *stRemark3;
      RadioGroup* rgDatumType;
      FieldReal *frDX, *frDY, *frDZ, *frRotX, *frRotY, *frRotZ, *frDS, *frX0, *frY0, *frZ0;
	};
  String sDatum, sArea;
	CoordSystemViaLatLon* csvll = GetDocument()->CoordSys()->pcsViaLatLon();
	ISTRUE(fINotEqual, csvll, (CoordSystemViaLatLon*) NULL);

  UserDefDatumInfo info;
  if (csvll->datum) 
	{
    sDatum = csvll->datum->sName();
    sArea = csvll->datum->sArea;
    Datum* molDat = csvll->datum;
    if (molDat)
    {
      info.iDatumType = 0;
      info.dx = molDat->dx;
      info.dy = molDat->dy;
      info.dz = molDat->dz;
      BursaWolfDatum* bwDat = dynamic_cast<BursaWolfDatum*>(molDat);
      if (bwDat)
      {
        info.iDatumType = 1;
        info.rotX = bwDat->m_rotX*rad2sec;// rotations displayed in arc-seconds
        info.rotY = bwDat->m_rotY*rad2sec;
        info.rotZ = bwDat->m_rotZ*rad2sec;
        info.dS = bwDat->m_dS*1000000;    // scale-change in ppm, parts per million
        BadekasDatum* baDat = dynamic_cast<BadekasDatum*>(bwDat);
        if (baDat)
        {
          info.iDatumType = 2;
          info.x0 = baDat->m_X0;
          info.y0 = baDat->m_Y0;
          info.z0 = baDat->m_Z0;
        }
      }

    }
  }
  else
    sDatum = SCSDatumNotSpecified;  
  ChangeDatumForm frm(this, &sDatum, &sArea, &info);
  if (frm.fOkClicked()) 
	{
		DataHasChanged(true);
		fbs |= fbsNOCANCELBUTTON; // remove cancel button
		exec(); // store prj and and other parameters!
    if (1 == frm.iType) 
    {
      switch (info.iDatumType)
      {
      case 0:
        csvll->datum = new MolodenskyDatum(csvll->ell, info.dx, info.dy, info.dz);
        break;
      case 1:
        csvll->datum = new BursaWolfDatum(csvll->ell, info.dx, info.dy, info.dz,
                                          info.rotX/rad2sec, info.rotY/rad2sec, 
																					info.rotZ/rad2sec, info.dS/1000000);
        break;
      case 2:
        csvll->datum = new BadekasDatum(csvll->ell, info.dx, info.dy, info.dz,
                                        info.rotX/rad2sec, info.rotY/rad2sec, 
																				info.rotZ/rad2sec, info.dS/1000000,
                                        info.x0, info.y0, info.z0);
        break;
      }
    }
    else if (sDatum == SCSDatumNotSpecified)
      csvll->datum = 0;
    else  
      csvll->datum = new MolodenskyDatum(sDatum, sArea);
		//if (csvll->datum)
    //  delete csvll->datum;
    //csvll->datum = molDatum;
    if (csvll->datum)
			csvll->ell = csvll->datum->ell;

		csvll->Updated();

		PostMessage(ILWM_RECALCULATEFORM);
		return true;
  }  
  return false;
}


int CoordSysViaLatLonView::ButtonEllipsoid(Event*)
{
  class ChangeEllForm: public FormWithDest 
	{
  public:
    ChangeEllForm(CWnd* w, String* sEll, double& a, double& f1) 
    : FormWithDest(w, SCSTitleSelectEll)
    {
      new StaticText(root, SCSUiEll);
      iType = fCIStrEqual("User Defined" , *sEll) ? 1 : 0;
      RadioGroup* rg = new RadioGroup(root, "", &iType);
      RadioButton* rbPreDef = new RadioButton(rg, SCSUiPreDefined);
      RadioButton* rbUserDef = new RadioButton(rg, SCSUiUserDefined);
      FieldGroup* fg = new FieldGroup(rbUserDef);
      fg->Align(rbUserDef, AL_UNDER);
      fg->SetIndependentPos();
      if (f1 < 200) {
        a = 6378137.0;
        f1 = 298.257223563;
      }
      new FieldReal(fg, "&a", &a, ValueRange(1, 166999999, 0.001)); // WGS84: 6378137.0
      new FieldReal(fg, "1/&f", &f1, ValueRange(200, 9999999999, 1e-9));         // WGS84: 298.257223563
      FieldEllipsoid* fe = new FieldEllipsoid(rbPreDef, sEll);
      fe->Align(rbUserDef, AL_UNDER);
      SetMenHelpTopic(htpChangeEll);
      create();
    }
    int iType;
  };
	CoordSystemViaLatLon* csvll = GetDocument()->CoordSys()->pcsViaLatLon();
	ISTRUE(fINotEqual, csvll, (CoordSystemViaLatLon*) NULL);
  String sEll = csvll->ell.sName;
  double a = 0, f1 = 0;
  if (!csvll->ell.fSpherical())
  {
    a = csvll->ell.a;
    f1 = 1/csvll->ell.f;
  }
  ChangeEllForm frm(this, &sEll, a, f1);
  if (frm.fOkClicked()) 
	{
		DataHasChanged(true);
		fbs |= fbsNOCANCELBUTTON; // remove cancel button
		exec(); // store prj and and other parameters!
    if (1 == frm.iType)
      csvll->ell = Ellipsoid(a, f1);
    else
      csvll->ell = Ellipsoid(sEll);

    if (csvll->datum)
		{
			if (fCIStrEqual("User Defined" , csvll->datum->sName().sSub(0,12))) 
				csvll->datum->ell = csvll->ell;
			else if (!fCIStrEqual(csvll->datum->ell.sName, sEll))
			{
				delete csvll->datum;
	      csvll->datum = 0;
	    }
		}

		csvll->Updated();

		PostMessage(ILWM_RECALCULATEFORM);
    return 1;
  }  
  return 0;
}

int CoordSysViaLatLonView::exec()
{
	if ( !fDataHasChanged() ) return 0;
  CoordSysView::exec();

	CoordSystemViaLatLon* csvll = GetDocument()->CoordSys()->pcsViaLatLon();
	ISTRUE(fINotEqual, csvll, (CoordSystemViaLatLon*) NULL);

  if (csvll->ell.fSpherical()) {
    csvll->ell = Ellipsoid(rRadius, 0);
  }
  return 0;
}

//----------------------
#ifdef _DEBUG
void CoordSysViaLatLonView::AssertValid() const
{
	CoordSysView::AssertValid();
}

void CoordSysViaLatLonView::Dump(CDumpContext& dc) const
{
	CoordSysView::Dump(dc);
}

#endif //_DEBUG

