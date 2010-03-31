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
// CoordSysView.cpp : implementation of the CoordSysView class
// Created by Martin Schouwenburg 15-6-99
#include "Client\Headers\formelementspch.h"
#include "Client\ilwis.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\Forms\generalformview.h"
#include "Client\Base\Res.h"
#include "Engine\Base\AssertD.h"
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Engine\SpatialReference\Coordsys.h"
#include "Client\Editors\CoordSystem\CSyDoc.h"
#include "Client\Editors\CoordSystem\CsyView.h"
#include "Headers\Hs\Coordsys.hs"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CoordSysView

IMPLEMENT_DYNCREATE(CoordSysView, GeneralFormView)

BEGIN_MESSAGE_MAP(CoordSysView, GeneralFormView)
END_MESSAGE_MAP()


CoordSysView::CoordSysView() :
	fShowCrdMinMax(true),
	fDefineCrdMinMax(true)
{

}

CoordSysView::~CoordSysView()
{
}

void CoordSysView::CreateForm()
{
	CoordSysView::create();
}

void CoordSysView::create()
{
	GeneralFormView::create();

  FieldGroup* fg = 0;
	CoordSystem cs = GetDocument()->CoordSys();
	CoordSystemProjection* cspr = cs->pcsProjection();
  if (fShowCrdMinMax) 
	{
    Coord* cMin = &cs->cb.cMin;
    Coord* cMax = &cs->cb.cMax;
    if (cMax->x < cMin->x || cMax->y < cMin->y)
      *cMin = *cMax = crdUNDEF;//Coord(0,0);
    if (cs->fDataReadOnly()) 
		{
      StaticText* stMin = new StaticText(root, SCSUiMinXY);
      String s = cs->sValue(*cMin);
      StaticText* stCrd = new StaticText(root, s);
      stCrd->Align(stMin, AL_AFTER);
      StaticText* stMax = new StaticText(root, SCSUiMaxXY);
      stMax->Align(stMin, AL_UNDER);
      s = cs->sValue(*cMax);
      stCrd = new StaticText(root, s);
      stCrd->Align(stMax, AL_AFTER);
      fg = new FieldGroup(root, true);
      fg->Align(stMax, AL_UNDER);
    }
    else if (0 == cspr)
		{
      fldCrdMin = new FieldCoord(root, SCSUiMinXY, cMin);
      fldCrdMin->SetCallBack((NotifyProc)&CoordSysView::CallBack);
      fldCrdMax = new FieldCoord(root, SCSUiMaxXY, cMax);
      fldCrdMax->SetCallBack((NotifyProc)&CoordSysView::CallBack);
    }
  }
  SetMenHelpTopic(htpCSEdit);

}

int CoordSysView::CallBack(Event *)
{
  Coord cMin = fldCrdMin->crdVal();
  Coord cMax = fldCrdMax->crdVal();
	CoordSystem cs = GetDocument()->CoordSys();
	CoordSystemBoundsOnly *pcsb = cs->pcsBoundsOnly();
  if ((!cMin.fUndef() && !cMax.fUndef() && // require defined (in general)
		   cMin.x < cMax.x && cMin.y < cMax.y) // and well ordered bounds
			|| (!fDefineCrdMinMax)) // accept if user has chosen not to fill in coordbounds
	{
    EnableOK();
		CoordSystem cs = GetDocument()->CoordSys();
		CoordBounds cb(cMin, cMax);
		// two cases: fDefineCrdMinMax == true and fDefineCrdMinMax == false
		if ((fDefineCrdMinMax && !(cs->cb == cb)) ||(!fDefineCrdMinMax && !cs->cb.fUndef()))
			DataHasChanged(true);
	}
  else
    DisableOK();  
  return 0;  
}

int CoordSysView::CrdMinMaxCallBack(Event*)
{
	cbDefineCrdMinMax->StoreData();
	CallBack(0); // explicitly call! zapp sillyness: children's callbacks are called before own callback!
	// especially: call always because of the DataHasChanged that might happen in CallBack()
	if (!fDefineCrdMinMax)
	{
		CoordSystem cs = GetDocument()->CoordSys();
		Coord* cMin = &cs->cb.cMin;
		Coord* cMax = &cs->cb.cMax;
		*cMin = *cMax = crdUNDEF;
		EnableOK();
	}
	return 0; 
}

int CoordSysView::exec()
{
	GeneralFormView::exec();
	CoordSystem cs = GetDocument()->CoordSys();
	if ( cs->fReadOnly() )
		return 0;
	if ( fDataHasChanged())
		cs->Updated();

	return 0;
  
}

#ifdef _DEBUG
void CoordSysView::AssertValid() const
{
	GeneralFormView::AssertValid();
}

void CoordSysView::Dump(CDumpContext& dc) const
{
	GeneralFormView::Dump(dc);
}

#endif //_DEBUG

CoordSysDoc* CoordSysView::GetDocument() 
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CoordSysDoc)));
	return dynamic_cast<CoordSysDoc *>(m_pDocument);
}

