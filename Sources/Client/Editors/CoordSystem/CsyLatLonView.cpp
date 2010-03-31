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
// CoordSysLatLonView.cpp : implementation of the CoordSysLatLonView class
// Created by Martin Schouwenburg 15-6-99
#include "Client\Headers\formelementspch.h"
#include "Client\ilwis.h"
#include "Client\Base\IlwisDocument.h"
#include "Headers\Hs\Mainwind.hs"
#include "Client\Base\Res.h"
#include "Engine\Base\AssertD.h"
#include "Client\Forms\generalformview.h"
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Engine\SpatialReference\csviall.h"
#include "Engine\SpatialReference\Cslatlon.h"
#include "Engine\SpatialReference\DATUM.H"
#include "Client\Editors\CoordSystem\CSyDoc.h"
#include "Client\Editors\CoordSystem\CSyDoc.h"
#include "Client\Editors\CoordSystem\CsyView.h"
#include "Client\Editors\CoordSystem\CsyViaLatLonView.h"
#include "Client\Editors\CoordSystem\CsyLatLonView.h"
#include "Client\FormElements\fentdms.h"
#include "Headers\Hs\Coordsys.hs"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CoordSysLatLonView

IMPLEMENT_DYNCREATE(CoordSysLatLonView, CoordSysViaLatLonView)

BEGIN_MESSAGE_MAP(CoordSysLatLonView, CoordSysViaLatLonView)
END_MESSAGE_MAP()


CoordSysLatLonView::CoordSysLatLonView()
{
	fShowCrdMinMax=false;
}

CoordSysLatLonView::~CoordSysLatLonView()
{
}

void CoordSysLatLonView::CreateForm()
{
  CoordSysLatLonView::create();
}

void CoordSysLatLonView::create()
{
	CoordSysView::create();

	CoordSystemLatLon* csll = GetDocument()->CoordSys()->pcsLatLon();
	ISTRUE(fINotEqual, csll, (CoordSystemLatLon*) NULL);

	FieldGroup* fg = 0;
  StaticText* st = 0;
  Coord* cMin = &csll->cb.cMin;
  Coord* cMax = &csll->cb.cMax;
	m_fDefaultChanged = false;
  if (cMax->x < cMin->x) 
  {
    cMin->x = -180;
    cMax->x =  180;
	m_fDefaultChanged = true;
  }
  if (cMax->y < cMin->y) 
  {
    cMin->y = -90;
    cMax->y =  90;
	m_fDefaultChanged = true;
  }

  if (csll->fDataReadOnly()) 
  {
    fg = new FieldGroup(root);
    StaticText* stMin = new StaticText(fg, SMSUiMinLatLon);
    String s = csll->sValue(*cMin);
    StaticText* stCrd = new StaticText(fg, s);
    stCrd->Align(stMin, AL_AFTER);
    StaticText* stMax = new StaticText(fg, SMSUiMaxLatLon);
    stMax->Align(stMin, AL_UNDER);
    s = csll->sValue(*cMax);
    stCrd = new StaticText(fg, s);
    stCrd->Align(stMax, AL_AFTER);
    fg = new FieldGroup(root, true);
    fg->Align(stMax, AL_UNDER);
  }
  else 
  {
    fldLatMin = new FieldLat(root, SCSUiMinLat, &cMin->y);
	fldLatMin->SetCallBack((NotifyProc)&CoordSysLatLonView::CallBack);
    fldLonMin = new FieldLon(root, SCSUiMinLon, &cMin->x);
	fldLonMin->SetCallBack((NotifyProc)&CoordSysLatLonView::CallBack);
	fldLatMax = new FieldLat(root, SCSUiMaxLat, &cMax->y);
	fldLatMax->SetCallBack((NotifyProc)&CoordSysLatLonView::CallBack);
    fldLonMax = new FieldLon(root, SCSUiMaxLon, &cMax->x);
	fldLonMax->SetCallBack((NotifyProc)&CoordSysLatLonView::CallBack);
  }
  if (!csll->fDataReadOnly()) 
	{
    FieldGroup* fgButtons = new FieldGroup(root, true);
    PushButton *pbEll, *pbDat;
    pbEll = new PushButton(fgButtons, SCSUiEll, (NotifyProc)&CoordSysViaLatLonView::ButtonEllipsoid);
    pbDat = new PushButton(fgButtons, SCSUiDatum, (NotifyProc)&CoordSysViaLatLonView::ButtonDatum);
    pbDat->Align(pbEll, AL_AFTER);
    FieldBlank* fb = new FieldBlank(root, (float)0.2);
    fb->Align(fgButtons, AL_UNDER);
  }
  ShowDatumEllInfo();
  SetMenHelpTopic(htpCSEditLatLon);

}

int CoordSysLatLonView::CallBack(Event *)
{
  double llLatMin = fldLatMin->rVal();
  double llLatMax = fldLatMax->rVal();
  double llLonMin = fldLonMin->rVal();
  double llLonMax = fldLonMax->rVal();
  if (llLatMin < llLatMax && llLonMin < llLonMax )
	{
    EnableOK();
		DataHasChanged(true);
	}
  else
    DisableOK();  
  return 0;  
}

int CoordSysLatLonView::exec()
{
	if (!m_fDefaultChanged && !fDataHasChanged() ) return 0;
	CoordSysViaLatLonView::exec();

	CoordSystemLatLon* csll = GetDocument()->CoordSys()->pcsLatLon();
	ISTRUE(fINotEqual, csll, (CoordSystemLatLon*) NULL);

	csll->Store();

	return 1;

}

#ifdef _DEBUG
void CoordSysLatLonView::AssertValid() const
{
	CoordSysViaLatLonView::AssertValid();
}

void CoordSysLatLonView::Dump(CDumpContext& dc) const
{
	CoordSysViaLatLonView::Dump(dc);
}

#endif //_DEBUG

