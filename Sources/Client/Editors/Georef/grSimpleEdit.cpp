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
/* GeoRefSimple editor
   by Willem Nieuwenhuis, 11 August 1999
   Copyright Ilwis System Development ITC
*/

#include "Client\Headers\formelementspch.h"
#include "Client\ilwis.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\Editors\Georef\GeoRefDoc.h"
#include "Client\FormElements\fldgrf.h"
#include "Client\FormElements\fldcs.h"
#include "Engine\SpatialReference\Gr.h"
#include "Engine\SpatialReference\Grsmpl.h"
#include "Headers\Htp\Ilwismen.htp"
#include "Client\Forms\generalformview.h"
#include "Client\Editors\Georef\grSimpleEdit.h"
#include "Headers\Hs\Georef.hs"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// GeoRefSimpleView

IMPLEMENT_DYNCREATE(GeoRefSimpleView, GeneralFormView)

BEGIN_MESSAGE_MAP(GeoRefSimpleView, GeneralFormView)
END_MESSAGE_MAP()

GeoRefSimpleView::GeoRefSimpleView()
{

}

GeoRefSimpleView::~GeoRefSimpleView()
{
}

GeoRefDoc* GeoRefSimpleView::GetDocument()
{
	return (GeoRefDoc*)m_pDocument;
}

void GeoRefSimpleView::CreateForm()
{
	create();
}

void GeoRefSimpleView::create()
{
	GeneralFormView::create();

	GeoRefSmpl* pgs = GetDocument()->gr()->pgsmpl();
	ISTRUE(fINotEqual, pgs, (GeoRefSmpl*)0);

	pgs->GetSmpl(a11, a12, a21, a22, b1, b2);

	rcSize  = pgs->rcSize();

	String sRemark(TR("%li lines and %li columns").c_str(), rcSize.Row, rcSize.Col);
	FormEntry* fe = new StaticText(root, sRemark);
	fe->SetIndependentPos();

	sCoordSystem = pgs->cs()->sName(true);

	if (GetDocument()->gr()->fReadOnly())
	{
		String s("%S: %S", TR("&Coordinate System"), sCoordSystem);
		StaticText* st = new StaticText(root, s);
		st->psn->SetBound(0, 0, 0, 0);

		s = String(TR("a11= %lg,   a12= %lg").c_str(), a11, a12);
		st = new StaticText(root, s);
		st->psn->SetBound(0, 0, 0, 0);

		s = String(TR("a21= %lg,   a22= %lg").c_str(), a21, a22);
		st = new StaticText(root, s);
		st->psn->SetBound(0, 0, 0, 0);

		s = String(TR("b1= %lg,   b2= %lg").c_str(), b1, b2);
		st = new StaticText(root, s);
		st->psn->SetBound(0, 0, 0, 0);

		s = String(' ', 50);
		stRemark = new StaticText(root, s);
	}
	else 
	{
		FieldGroup* fg = new FieldGroup(root);
		FieldReal* fr11 = new FieldReal(fg, "a11", &a11);
		FieldReal* fr12 = new FieldReal(fg, "a12", &a12);
		fr12->Align(fr11, AL_AFTER);
		FieldReal* fr21 = new FieldReal(fg, "a21", &a21);
		fr21->Align(fr11, AL_UNDER);
		FieldReal* fr22 = new FieldReal(fg, "a22", &a22);
		fr22->Align(fr21, AL_AFTER);
		FieldReal* fr1  = new FieldReal(fg, "b1",  &b1 );
		fr1->Align(fr21, AL_UNDER);
		FieldReal* fr2  = new FieldReal(fg, "b2",  &b2 );
		fr2->Align(fr1, AL_AFTER);
		String s(' ', 50);
		stRemark = new StaticText(fg, s);
		stRemark->SetIndependentPos();
		stRemark->Align(fr1, AL_UNDER);
	}

	SetCallBack((NotifyProc)&GeoRefSimpleView::CallBack);

	SetMenHelpTopic("");
}

int GeoRefSimpleView::exec()
{
	try
	{
		GeneralFormView::exec();

		GeoRefSmpl* pgs = GetDocument()->gr()->pgsmpl();
		ISTRUE(fINotEqual, pgs, (GeoRefSmpl*)0);

		pgs->SetSmpl(a11, a12, a21, a22, b1, b2);
		pgs->Updated();

		return 0;
	}
	catch (ErrorObject& err)
	{
		err.Show();
	}  
	return 1;
}

int GeoRefSimpleView::CallBack(Event*)
{
	root->StoreData();

	double rDet = abs(a11 * a22 - a12 * a21);
	if (rDet > 1e-13)
	{
		double rPixSize = 1 /sqrt(rDet);
		const CoordSystem & cs = GetDocument()->gr()->cs();
		bool fLatLon = false;
		if (cs.fValid())
			fLatLon = (0 != cs->pcsLatLon());
		if (fLatLon) {
			String sPix = LatLon::sDegree(rPixSize);
			String sPixSize(TR("Pixel Size = %S").c_str(), sPix);
			stRemark->SetVal(sPixSize);
		} else {
			String sPixSize(TR("Pixel Size = %.3f m").c_str(), rPixSize);
			stRemark->SetVal(sPixSize);
		}
		EnableOK(); 
	}
	else
	{
		stRemark->SetVal(TR("Singular Matrix"));
		DisableOK(); 
	}

	return 0;
}
