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
/* GeoRefFactor editor
   by Willem Nieuwenhuis, 11 August 1999
   Copyright Ilwis System Development ITC
*/

#include "Client\Headers\formelementspch.h"
#include "Client\ilwis.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\Editors\Georef\GeoRefDoc.h"
#include "Client\Editors\Georef\GeoRefGenerDoc.h"
#include "Client\FormElements\fldgrf.h"
#include "Engine\SpatialReference\Gr.h"
#include "Engine\SpatialReference\Grfactor.h"
#include "Headers\Htp\Ilwismen.htp"
#include "Client\Forms\generalformview.h"
#include "Client\Editors\Georef\GrGenerEdit.h"
#include "Client\Editors\Georef\GrFactorEdit.h"
#include "Headers\Hs\Georef.hs"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// GeoRefFactorView

IMPLEMENT_DYNCREATE(GeoRefFactorView, GeoRefGeneratedView)

BEGIN_MESSAGE_MAP(GeoRefFactorView, GeoRefGeneratedView)
END_MESSAGE_MAP()

GeoRefFactorView::GeoRefFactorView()
{

}

GeoRefFactorView::~GeoRefFactorView()
{
}

bool GeoRefFactorView::fRemarkNeeded()
{
	return true;
}

void GeoRefFactorView::FillDerivedFields(FieldGroup* fgGenerRoot)
{
	GeoRefFactor* pgf = GetDocument()->gr()->pgFac();
	ISTRUE(fINotEqual, pgf, (GeoRefFactor*)0);

	m_iSizeDirection = (pgf->rFactor() > 1) ? 1 : 0;
	m_rFactor = m_iSizeDirection == 1 ? pgf->rFactor() : 1 / pgf->rFactor();
	m_rcOffset = pgf->rcOffset();
	double rPix = m_rParentPixSize / pgf->rFactor();

	String sGRSize(TR("%li lines and %li columns").c_str(), m_rcSize.Row, m_rcSize.Col);
	SetSizeString(sGRSize);
	String sPixSize(TR("Pixel Size = %.3f m").c_str(), rPix);
	SetPixelSizeString(sPixSize);

	// Disabled editable Offset and Size fields for now; show R/O only
	if (1) // GetDocument()->gr()->fReadOnly())
	{
		StaticText* st;
		if (m_iSizeDirection == 0)   // aggregation, pixel enlargement
		{
			String s("%S %lg", TR("Increased pixel size, factor ="), m_rFactor);
			st = new StaticText(fgGenerRoot, s);
			st->psn->SetBound(0, 0, 0, 0);

			s = String("%S = (%ld, %ld)", TR("&Offset Row, Col"), m_rcOffset.Row, m_rcOffset.Col);
			st = new StaticText(fgGenerRoot, s);
			st->psn->SetBound(0, 0, 0, 0);
		}
		else   // densify, pixel shrinking
		{
			String s("%S %lg", TR("Decreased pixel size, factor ="), m_rFactor);
			st = new StaticText(fgGenerRoot, s);
			st->psn->SetBound(0, 0, 0, 0);
		}
	}
	else 
	{
		// FieldGroup fgGenerRoot has been created in parent class
		rgSizePixel = new RadioGroup(fgGenerRoot, TR("&Pixel size"), &m_iSizeDirection, true);
		rgSizePixel->SetCallBack((NotifyProc)&GeoRefFactorView::SizePixel);
		new RadioButton(rgSizePixel, TR("&Increase"));
		new RadioButton(rgSizePixel, TR("Dec&rease"));

		frFactor = new FieldReal(fgGenerRoot, TR("&Factor"), &m_rFactor, ValueRange(-1e300,1e300,1));
		frFactor->SetCallBack((NotifyProc)&GeoRefFactorView::CheckFactor);
		fgOffset = new FieldGroup(fgGenerRoot);
		fiRow = new FieldInt(fgOffset, TR("&Offset Row, Col"), &m_rcOffset.Row);
		fiCol = new FieldInt(fgOffset, "", &m_rcOffset.Col);
		fiRow->SetCallBack((NotifyProc)&GeoRefFactorView::CheckWithFactor);
		fiCol->SetCallBack((NotifyProc)&GeoRefFactorView::CheckWithFactor);
		fiCol->Align(fiRow, AL_AFTER);
	}

	SetMenHelpTopic("");
}

int GeoRefFactorView::exec()
{
	try
	{
		GeoRefGeneratedView::exec();

		GeoRefFactor* pgf = GetDocument()->gr()->pgFac();
		ISTRUE(fINotEqual, pgf, (GeoRefFactor*)0);

		pgf->SetFactor(m_iSizeDirection == 0 ? 1 / m_rFactor : m_rFactor);
		pgf->SetOffset(m_rcOffset);
		pgf->SetRowCol(m_rcSize);

		pgf->Updated();

		return 0;
	}
	catch (ErrorObject& err)
	{
		err.Show();
	}  
	return 1;
}

void GeoRefFactorView::CalcParentGeoRefSize()
{
	if (m_iSizeDirection == 0)
	{
		m_rcOrgSize.Row = longConv(m_rcSize.Row * m_rFactor);
		m_rcOrgSize.Row += m_rcOffset.Row;
		m_rcOrgSize.Col = longConv(m_rcSize.Col * m_rFactor);
		m_rcOrgSize.Col += m_rcOffset.Col;
	}
	else
	{
		m_rcOrgSize.Row = (long)(m_rcSize.Row / m_rFactor);
		m_rcOrgSize.Col = (long)(m_rcSize.Col / m_rFactor);
	}
}

void GeoRefFactorView::ReCompute()
{
	double rPix = m_rParentPixSize;
	if (m_iSizeDirection == 0)
	{
		m_rcSize.Row = m_rcOrgSize.Row - m_rcOffset.Row;
		m_rcSize.Row = (long)(m_rcSize.Row / m_rFactor);
		m_rcSize.Col = m_rcOrgSize.Col - m_rcOffset.Col;
		m_rcSize.Col = (long)(m_rcSize.Col / m_rFactor);
		rPix *= m_rFactor;
	}
	else
	{
		m_rcSize.Row = (long)(m_rcOrgSize.Row * m_rFactor);
		m_rcSize.Col = (long)(m_rcOrgSize.Col * m_rFactor);
		rPix /= m_rFactor;
	}

	String sGRSize(TR("%li lines and %li columns").c_str(), m_rcSize.Row, m_rcSize.Col);
	SetSizeString(sGRSize);
	String sPixSize(TR("Pixel Size = %.3f m").c_str(), rPix);
	SetPixelSizeString(sPixSize);
}

int GeoRefFactorView::SizePixel(Event* evt)
{
	rgSizePixel->StoreData();
	switch (m_iSizeDirection)
	{
	    case 0: fgOffset->Show();  // in case of aggregation
				break;

		case 1: fgOffset->Hide();  // in case of densify
				CheckFactor(evt);  // update state of the OK button
				String s = String(' ', 60);
				SetRemark(s);
			    break;
	}
	ReCompute();

	return 0;
}

int GeoRefFactorView::CheckWithFactor(Event*)
{
	root->StoreData();

	if ( (m_rcOffset.Row >= m_rFactor || m_rcOffset.Row < 0) ||
		 (m_rcOffset.Col >= m_rFactor || m_rcOffset.Col < 0) )
	{
		SetRemark(TR("Row and/or Col offset should be less than the Factor"));
		DisableOK();
	}
	else
	{
		String s(' ', 60);
		SetRemark(s);

		EnableOK();
	}

	return 0;
}

int GeoRefFactorView::CheckFactor(Event*)
{
	root->StoreData();

	if (m_rFactor < 1)
	{
		SetRemark(TR("Factor should be larger than 1"));
		DisableOK();
	}
	else
	{
		String s(' ', 60);
		SetRemark(s);

		ReCompute();
		EnableOK();
	}

	return 0;
}
