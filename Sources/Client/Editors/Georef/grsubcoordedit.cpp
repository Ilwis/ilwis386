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
/* GeoRefSubMapCoordCoord editor
   by Willem Nieuwenhuis, 21 January 2000
   Copyright Ilwis System Development ITC
*/

#include "Client\Headers\formelementspch.h"
#include "Client\ilwis.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\Editors\Georef\GeoRefDoc.h"
#include "Client\Editors\Georef\GeoRefGenerDoc.h"
#include "Client\FormElements\fldgrf.h"
#include "Engine\SpatialReference\Gr.h"
#include "Engine\SpatialReference\Grsub.h"
#include "Headers\Htp\Ilwismen.htp"
#include "Client\Forms\generalformview.h"
#include "Client\Editors\Georef\GrGenerEdit.h"
#include "Client\Editors\Georef\grsubcoordedit.h"
#include "Headers\Hs\Georef.hs"
#include "Headers\Hs\Appforms.hs"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// GeoRefSubMapCoordCoordView

IMPLEMENT_DYNCREATE(GeoRefSubMapCoordView, GeoRefGeneratedView)

BEGIN_MESSAGE_MAP(GeoRefSubMapCoordView, GeoRefGeneratedView)
END_MESSAGE_MAP()

GeoRefSubMapCoordView::GeoRefSubMapCoordView()
{

}

GeoRefSubMapCoordView::~GeoRefSubMapCoordView()
{
}

bool GeoRefSubMapCoordView::fRemarkNeeded()
{
	return false;
}

void GeoRefSubMapCoordView::FillDerivedFields(FieldGroup* fgGenerRoot)
{
	GeoRefSubMapCoords* pgs = dynamic_cast<GeoRefSubMapCoords*>(GetDocument()->gr().ptr());
	ISTRUE(fINotEqual, pgs, (GeoRefSubMapCoords*)0);

	m_rcOffset = pgs->rcTopLeft();
	m_crdFirst = pgs->crd1();
	m_crdSecond = pgs->crd2();

	String sGRSize(TR("%li lines and %li columns").c_str(), m_rcSize.Row, m_rcSize.Col);
	SetSizeString(sGRSize);
	const CoordSystem & cs = GetDocument()->gr()->cs();
	bool fLatLon = false;
	if (cs.fValid())
		fLatLon = (0 != cs->pcsLatLon());
	if (fLatLon) {
		String sPix = LatLon::sDegree(m_rParentPixSize);
		String sPixSize(TR("Pixel Size = %S").c_str(), sPix);
		SetPixelSizeString(sPixSize);
	} else {
		String sPixSize(TR("Pixel Size = %.3f m").c_str(), m_rParentPixSize);
		SetPixelSizeString(sPixSize);
	}

	// Disabled editable Offset and Size fields for now; show R/O only
	if (1) // GetDocument()->gr()->fReadOnly())
	{
		sGRSize = String(TR("First Coordinate: (%.3lf, %.3lf)").c_str(), m_crdFirst.x, m_crdFirst.y);
		StaticText* stSize2 = new StaticText(fgGenerRoot, sGRSize);
		stSize2->SetIndependentPos();

		sGRSize = String(TR("Opposite Coordinate: (%.3lf, %.3lf)").c_str(), m_crdSecond.x, m_crdSecond.y);
		stSize2 = new StaticText(fgGenerRoot, sGRSize);
		stSize2->SetIndependentPos();
	}
	else 
	{
		FieldGroup* fgOffset = new FieldGroup(fgGenerRoot);
		fcFirst = new FieldCoord(fgOffset, TR("&First Coordinate"), &m_crdFirst);

		fcSecond = new FieldCoord(fgOffset, TR("O&pposite Coordinate"), &m_crdSecond);
		fcSecond->Align(fcFirst, AL_UNDER);
	}

	SetMenHelpTopic("");
}

int GeoRefSubMapCoordView::exec()
{
	try
	{
		GeoRefGeneratedView::exec();

		GeoRefSubMapCoords* pgs = dynamic_cast<GeoRefSubMapCoords*>(GetDocument()->gr().ptr());
		ISTRUE(fINotEqual, pgs, (GeoRefSubMapCoords*)0);

		pgs->SetCrd1(m_crdFirst);
		pgs->SetCrd2(m_crdSecond);

		pgs->Updated();

		return 0;
	}
	catch (ErrorObject& err)
	{
		err.Show();
	}  
	return 1;
}
