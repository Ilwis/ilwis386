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
/* GeoRefMirrorRotate editor
   by Willem Nieuwenhuis, 17 August 1999
   Copyright Ilwis System Development ITC
*/

#include "Client\Headers\formelementspch.h"
#include "Client\ilwis.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\Editors\Georef\GeoRefDoc.h"
#include "Client\Editors\Georef\GeoRefGenerDoc.h"
#include "Engine\SpatialReference\Gr.h"
#include "Engine\SpatialReference\GRMRROT.H"
#include "Headers\Htp\Ilwismen.htp"
#include "Client\Forms\generalformview.h"
#include "Client\Editors\Georef\GrGenerEdit.h"
#include "Client\Editors\Georef\GrMirRotEdit.h"
#include "Headers\Hs\Georef.hs"
#include "Client\FormElements\FldMirRotType.h"
#include "Headers\Hs\Appforms.hs"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// GeoRefMirrorRotateView

IMPLEMENT_DYNCREATE(GeoRefMirrorRotateView, GeoRefGeneratedView)

BEGIN_MESSAGE_MAP(GeoRefMirrorRotateView, GeoRefGeneratedView)
END_MESSAGE_MAP()

GeoRefMirrorRotateView::GeoRefMirrorRotateView()
	: iMethod(0)
{

}

GeoRefMirrorRotateView::~GeoRefMirrorRotateView()
{
}

bool GeoRefMirrorRotateView::fRemarkNeeded()
{
	return false;
}

void GeoRefMirrorRotateView::FillDerivedFields(FieldGroup* fgGenerRoot)
{
	GeoRefMirrorRotate* pgmr = GetDocument()->gr()->pgMirRot();
	ISTRUE(fINotEqual, pgmr, (GeoRefMirrorRotate*)0);

	m_mrtMethod = pgmr->mrtMethod();
	iMethod = (long)m_mrtMethod;

	String sGRSize(SGRRemLinesCols_ii.c_str(), m_rcSize.Row, m_rcSize.Col);
	SetSizeString(sGRSize);
	String sPixSize(SGRRemPixSize_f.c_str(), m_rParentPixSize);
	SetPixelSizeString(sPixSize);

	// Disabled editable Offset and Size fields for now; show R/O only
	if (1) // GetDocument()->gr()->fReadOnly())
	{
		String s = String("%S: %S", SAFUiResampleMethod, pgmr->sMirrorRotateType(m_mrtMethod));
		StaticText* st = new StaticText(fgGenerRoot, s);
		st->psn->SetBound(0, 0, 0, 0);
	}
	else 
	{
		StaticText *st = new StaticText(fgGenerRoot, SAFUIMirRotMethod);
		st->SetIndependentPos();
		fosMirRotType = new FieldMirrorRotateType(fgGenerRoot, &iMethod);
		fosMirRotType->SetCallBack((NotifyProc)&GeoRefMirrorRotateView::DispType);
		fosMirRotType->Align(st, AL_AFTER);
	}

	SetMenHelpTopic("");
}

int GeoRefMirrorRotateView::exec()
{
	try
	{
		GeoRefGeneratedView::exec();

		GeoRefMirrorRotate* pgmr = GetDocument()->gr()->pgMirRot();
		ISTRUE(fINotEqual, pgmr, (GeoRefMirrorRotate*)0);

		pgmr->SetMirrorRotateMethod(m_mrtMethod);

		pgmr->Updated();

		return 0;
	}
	catch (ErrorObject& err)
	{
		err.Show();
	}  
	return 1;
}

int GeoRefMirrorRotateView::DispType(Event*)
{
	root->StoreData();
	
    m_mrtMethod = (MirrorRotateType)iMethod;
	switch (m_mrtMethod)
	{
		case mrNORM:
		case mrHOR:
		case mrVERT:
		case mrR180:
			m_rcSize.Row = m_rcOrgSize.Row;
			m_rcSize.Col = m_rcOrgSize.Col;
			break;
		case mrR90:
		case mrR270:
		case mrTRANS:
		case mrDIAG:
			m_rcSize.Row = m_rcOrgSize.Col;
			m_rcSize.Col = m_rcOrgSize.Row;
			break;
	}
	String sGRSize(SGRRemLinesCols_ii.c_str(), m_rcSize.Row, m_rcSize.Col);
	SetSizeString(sGRSize);
	String sPixSize(SGRRemPixSize_f.c_str(), m_rParentPixSize);
	SetPixelSizeString(sPixSize);

	return 0;
}
