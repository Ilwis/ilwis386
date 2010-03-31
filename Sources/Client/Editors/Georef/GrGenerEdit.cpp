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
/* GeoRefGenerated editor
   by Willem Nieuwenhuis, 11 August 1999
   Copyright Ilwis System Development ITC
*/

#include "Client\Headers\formelementspch.h"
#include "Client\ilwis.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\Editors\Georef\GeoRefDoc.h"
#include "Client\Editors\Georef\GeoRefGenerDoc.h"
#include "Engine\SpatialReference\Gr.h"
#include "Headers\Htp\Ilwismen.htp"
#include "Client\Forms\generalformview.h"
#include "Client\Editors\Georef\GrGenerEdit.h"
#include "Headers\Hs\Georef.hs"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// GeoRefGeneratedView

IMPLEMENT_DYNCREATE(GeoRefGeneratedView, GeneralFormView)

BEGIN_MESSAGE_MAP(GeoRefGeneratedView, GeneralFormView)
END_MESSAGE_MAP()

GeoRefGeneratedView::GeoRefGeneratedView()
{
}

GeoRefGeneratedView::~GeoRefGeneratedView()
{
}

GeoRefGeneratedDoc* GeoRefGeneratedView::GetDocument()
{
	return (GeoRefGeneratedDoc*)m_pDocument;
}

void GeoRefGeneratedView::CreateForm()
{
	create();
}

void GeoRefGeneratedView::FillDerivedFields(FieldGroup* fgGenerRoot)
{
}

bool GeoRefGeneratedView::fRemarkNeeded()
{
	return true;
}

void GeoRefGeneratedView::SetRemark(string sRem)
{
	if (fRemarkNeeded())
		stRemark->SetVal(sRem);
}

void GeoRefGeneratedView::SetSizeString(string sSize)
{
	stSize->SetVal(sSize);
}

void GeoRefGeneratedView::SetPixelSizeString(string sSize)
{
	if (stPixelSize)
		stPixelSize->SetVal(sSize);
}

void GeoRefGeneratedView::create()
{
	GeneralFormView::create();

	m_fnParent = GetDocument()->fnParent();
	m_fnRoot = GetDocument()->fnRoot();
	m_rcSize  = GetDocument()->gr()->rcSize();
	m_rParentPixSize = GetDocument()->rParentPixelSize();
	m_rcOrgSize = GetDocument()->rcParentSize();

	FieldGroup* fgMain = new FieldGroup(root);

	String s;

	FlatIconButton *fibLinked = 0;
	StaticText *st;
	int iImage = IlwWinApp()->iImage(".grf");
	if (iImage) 
	{
		s = String("%S: ", SGRInfParentGeoRef);
		st = new StaticText(fgMain, s);
		st->SetIndependentPos();
		st->psn->SetBound(0, 0, 0, 0);

		HICON icon = IlwWinApp()->ilSmall.ExtractIcon(iImage);
		fibLinked = new FlatIconButton(fgMain, icon, m_fnParent.sShortNameQuoted(FALSE), 
									   (NotifyProc)&GeoRefGeneratedView::OpenParentGeoRef, m_fnParent, false);
		fibLinked->SetIndependentPos();
		fibLinked->Align(st, AL_AFTER);
		st->psn->iHeight = fibLinked->psn->iHeight; // necessary to properly align fields with different heights
	}

	FlatIconButton *fibCoordSys = 0;
	StaticText *stCS;
	CoordSystem csLoc = GetDocument()->gr()->cs();
	iImage = IlwWinApp()->iImage(".csy");
	if (iImage)
	{
		s = String("%S: ", csLoc->sType());
		stCS = new StaticText(fgMain, s);
		if (st)
			stCS->Align(st, AL_UNDER);
		stCS->psn->SetBound(0, 0, 0, 0);

		HICON icon = IlwWinApp()->ilSmall.ExtractIcon(iImage);
		fibCoordSys = new FlatIconButton(fgMain, icon, csLoc->fnObj.sShortNameQuoted(FALSE), 
										 (NotifyProc)&GeoRefGeneratedView::OpenCoordSystem, csLoc->fnObj, false);
		fibCoordSys->SetIndependentPos();
		fibCoordSys->Align(stCS, AL_AFTER);
		stCS->psn->iHeight = fibCoordSys->psn->iHeight; // necessary to properly align fields with different heights
	}
	FieldBlank *fb = new FieldBlank(fgMain, 0.3);
	if (stCS)
		fb->Align(stCS, AL_UNDER);
	else if (st)
		fb->Align(st, AL_UNDER);

	FieldGroup* fgGenerRoot = new FieldGroup(fgMain);  // all fields for the derived forms will be placed in fgGenerRoot
	fgGenerRoot->Align(fb, AL_UNDER);
	fgGenerRoot->SetIndependentPos();

	FieldGroup* fgMessages = new FieldGroup(fgMain);
	fgMessages->Align(fgGenerRoot, AL_UNDER);
	fgMessages->SetIndependentPos();

	s = String('X', 50);
	stSize = new StaticText(fgMessages, s);
	stSize->SetIndependentPos();
	stSize->SetVal("");

	StaticText *stAlign = stSize; // used to align the remarks section under
	if (m_rParentPixSize != rUNDEF)
	{
		stPixelSize = new StaticText(fgMessages, s);
		stPixelSize->Align(stSize, AL_UNDER);
		stPixelSize->SetIndependentPos();
		stPixelSize->SetVal("");
		stAlign = stPixelSize;
	}
	else
		stPixelSize = 0;

	if (fRemarkNeeded())
	{
		stRemark = new StaticText(fgMessages, s);
		stRemark->Align(stAlign, AL_UNDER);
		stRemark->SetIndependentPos();
		stRemark->SetVal("");
	}

	FillDerivedFields(fgGenerRoot);  // Now fill all fields for the derived form
}

int GeoRefGeneratedView::OpenParentGeoRef(Event*)
{
	IlwWinApp()->OpenDocumentFile(m_fnParent.sFullName().scVal());
	return 0;
}

int GeoRefGeneratedView::OpenRootGeoRef(Event*)
{
	IlwWinApp()->OpenDocumentFile(m_fnRoot.sFullName().scVal());
	return 0;
}

int GeoRefGeneratedView::OpenCoordSystem(Event*)
{
	IlwWinApp()->OpenDocumentFile(GetDocument()->gr()->cs()->fnObj.sFullName().scVal());
	return 0;
}
