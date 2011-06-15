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
/* GeoRefCorners editor
   by Willem Nieuwenhuis, 11 August 1999
   Copyright Ilwis System Development ITC
*/

#include "Client\Headers\formelementspch.h"
#include "Engine\Base\DataObjects\Dat2.h"
#include "Client\ilwis.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\Editors\Georef\GeoRefDoc.h"
#include "Client\FormElements\fldgrf.h"
#include "Client\FormElements\fldcs.h"
#include "Engine\SpatialReference\Gr.h"
#include "Engine\SpatialReference\Grcornrs.h"
#include "Headers\Htp\Ilwismen.htp"
#include "Client\Forms\generalformview.h"
#include "Client\Editors\Georef\GrCornersEdit.h"
#include "Headers\Hs\Georef.hs"
#include "Headers\Hs\Coordsys.hs"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// GeoRefCornersView

IMPLEMENT_DYNCREATE(GeoRefCornersView, GeneralFormView)

BEGIN_MESSAGE_MAP(GeoRefCornersView, GeneralFormView)
END_MESSAGE_MAP()

GeoRefCornersView::GeoRefCornersView()
{
}

GeoRefCornersView::~GeoRefCornersView()
{
}

GeoRefDoc* GeoRefCornersView::GetDocument()
{
	return (GeoRefDoc*)m_pDocument;
}

void GeoRefCornersView::CreateForm()
{
	create();
}

void GeoRefCornersView::create()
{
	GeneralFormView::create();

	GeoRefCorners* pgcr = GetDocument()->gr()->pgc();
	ISTRUE(fINotEqual, pgcr, (GeoRefCorners*)0);

	sCoordSystem = pgcr->cs()->sName(true);
	fFromLatLon = ( 0 != pgcr->cs()->pcsLatLon());

	crdMin  = pgcr->crdMin;
	crdMax  = pgcr->crdMax;
	if (fFromLatLon)
	{
		llMin = LatLon(crdMin.y, crdMin.x);
		llMax = LatLon(crdMax.y, crdMax.x);
	}
	fCenter = !pgcr->fCornersOfCorners;
	rcSize  = pgcr->rcSize();

	String sRemark(SGRRemLinesCols_ii.c_str(), rcSize.Row, rcSize.Col);
	FormEntry* fe = new StaticText(root, sRemark);
	fe->SetIndependentPos();

	if (GetDocument()->gr()->fReadOnly())
	{
		String s("%S: %S", SGRUiCoordSys, sCoordSystem);
		StaticText* st = new StaticText(root, s);
		st->psn->SetBound(0,0,0,0);
		s = String("%S: %s", SGRUiCenterOfCorners, fCenter ? "Yes" : "No");
		st = new StaticText(root, s);
		st->psn->SetBound(0,0,0,0);
		s = String("%S: %S", SGRUiMinXY, pgcr->cs()->sValue(crdMin));
		st = new StaticText(root, s);
		st->psn->SetBound(0,0,0,0);
		s = String("%S: %S", SGRUiMaxXY, pgcr->cs()->sValue(crdMax));
		st = new StaticText(root, s);
		s = String(' ', 50);
		stRemark = new StaticText(root, s);
	}
	else
	{
		fldCsyC = new FieldCoordSystemC(root, SGRUiCoordSys, &sCoordSystem);
		fldCsyC->SetCallBack((NotifyProc)&GeoRefCornersView::CoordSysCallBack);
		FieldGroup* fg = new FieldGroup(root);
		cbCoC = new CheckBox(fg, SGRUiCenterOfCorners, &fCenter);
		cbCoC->SetCallBack((NotifyProc)&GeoRefCornersView::CallBackCenterOfCorner);
		cbCoC->SetIndependentPos();

		fgCoord = new FieldGroup(root);
		fgCoord->Align(cbCoC, AL_UNDER);
		fldCrdMin = new FieldCoord(fgCoord, SGRUiMinXY, &crdMin);
		fldCrdMin->SetCallBack((NotifyProc)&GeoRefCornersView::CallBackMinMaxXY);
		fldCrdMax = new FieldCoord(fgCoord, SGRUiMaxXY, &crdMax);
		fldCrdMax->SetCallBack((NotifyProc)&GeoRefCornersView::CallBackMinMaxXY);

		fgLatLon = new FieldGroup(root);
		fgLatLon->Align(cbCoC, AL_UNDER);
		fldMinLat = new FieldLat(fgLatLon, SCSUiMinLat, &llMin.Lat);
		fldMinLat->SetCallBack((NotifyProc)&GeoRefCornersView::CallBackMinMaxLatLon);
		fldMinLon = new FieldLon(fgLatLon, SCSUiMinLon, &llMin.Lon);
		fldMinLon->SetCallBack((NotifyProc)&GeoRefCornersView::CallBackMinMaxLatLon);
		fldMaxLat = new FieldLat(fgLatLon, SCSUiMaxLat, &llMax.Lat);
		fldMaxLat->SetCallBack((NotifyProc)&GeoRefCornersView::CallBackMinMaxLatLon);
		fldMaxLon = new FieldLon(fgLatLon, SCSUiMaxLon, &llMax.Lon);
		fldMaxLon->SetCallBack((NotifyProc)&GeoRefCornersView::CallBackMinMaxLatLon);

		String s('X', 45);
		stRemark = new StaticText(root, s);
		stRemark->SetIndependentPos();
	}
	SetMenHelpTopic("ilwismen\\edit_georeference_corners.htm");
	m_fInSetVal = false;
}

int GeoRefCornersView::exec()
{
	try
	{
		GeneralFormView::exec();

		GeoRefCorners* pgcr = GetDocument()->gr()->pgc();
		ISTRUE(fINotEqual, pgcr, (GeoRefCorners*)0);

		Coord cMin = crdMin;
		Coord cMax = crdMax;
		CoordSystem cs(sCoordSystem);
		if (cs->pcsLatLon())
		{
			pgcr->crdMin = Coord(llMin.Lon, llMin.Lat);
			pgcr->crdMax = Coord(llMax.Lon, llMax.Lat);;
		}
		else
		{
			pgcr->crdMin = cMin;
			pgcr->crdMax = cMax;
		}
		pgcr->fCornersOfCorners = !fCenter;
		pgcr->SetCoordSystem(cs);

		pgcr->Compute();
		pgcr->Updated();

		return 0;
	}
	catch (ErrorObject& err)
	{
		err.Show();
	}  
	return 1;
}


int GeoRefCornersView::CoordSysCallBack(Event*)
{
	try {
		FileName fnCS = FileName(sCoordSystem, ".csy"); // remember previous selected CSY
		bool fCurrLatLon = fFromLatLon;
		fldCsyC->StoreData();
		CoordSystem cs(sCoordSystem);
		if (cs.fValid())
		{
			fFromLatLon = (0 != cs->pcsLatLon());

			Coord cMin = crdMin;
			Coord cMax = crdMax;
			double rEps = 1; // geographic coordinates
			if (!fFromLatLon)
			{
				fgLatLon->Hide();
				fgCoord->Show();
			}
			else
			{
				cMin = Coord(llMin.Lon, llMin.Lat);
				cMax = Coord(llMax.Lon, llMax.Lat);
				rEps = 1e-8;
				fgCoord->Hide();
				fgLatLon->Show();
			}

			CoordBounds cbLoc(cMin, cMax);
			if (fCurrLatLon != fFromLatLon)
			{
				cbLoc = CoordBounds();  // initialize to undef if different from previous CSY
				cMin = cbLoc.cMin;
				cMax = cbLoc.cMax;
			}
			
			double r = max(cMax.x - cMin.x, cMax.y - cMin.y);
			r /= 800;
			if (!fFromLatLon)
			{
				if (fnCS != cs->fnObj)  // only change bounds when CS really changes
				{
					m_fInSetVal = true;
					crdMin = cMin;
					crdMax = cMax;
					fldCrdMin->SetVal(cMin);
					fldCrdMax->SetVal(cMax);
					m_fInSetVal = false;
				}
			}
			else
			{
				if (fnCS != cs->fnObj)  // only change bounds when CS really changes
				{
					m_fInSetVal = true;
					llMin = LatLon(cMin.y, cMin.x);
					llMax = LatLon(cMax.y, cMax.x);
					fldMinLat->SetVal(cMin.y);
					fldMaxLat->SetVal(cMax.y);
					fldMinLon->SetVal(cMin.x);
					fldMaxLon->SetVal(cMax.x);
					m_fInSetVal = false;
				}
			}
		}
		else 
		{
			fgLatLon->Hide();
			fgCoord->Hide();
		}
	}
	catch (const ErrorObject&) {
		fgLatLon->Hide();
		fgCoord->Hide();
	}
	return 0;
}

int GeoRefCornersView::CallBackMinMaxXY(Event*)
{
	if (m_fInSetVal)
		return 0;

	root->StoreData();
	if (crdMin.x < crdMax.x && crdMin.y < crdMax.y)
	{
		long iRow, iCol;
		double rX, rY;
		iRow = rcSize.Row;
		iCol = rcSize.Col;
		if (fCenter)
		{
			iCol -= 1;
			iRow -= 1;
		} 
		rX = (crdMax.x - crdMin.x) / iCol;
		rY = (crdMax.y - crdMin.y) / iRow;

		String s(SGRRemPixSize_ff.c_str(), rX, rY);
		stRemark->SetVal(s);

		EnableOK(); 
	}
	else
	{
		stRemark->SetVal(SGRRemMinSmallerMax);
		DisableOK(); 
	}  
	return 0;
}

int GeoRefCornersView::CallBackMinMaxLatLon(Event*) 
{
	if (m_fInSetVal)
		return 0;

	root->StoreData();
	LatLon laloMin, laloMax;
	laloMin.Lat = fldMinLat->rVal();
	laloMin.Lon = fldMinLon->rVal();
	laloMax.Lat = fldMaxLat->rVal();
	laloMax.Lon = fldMaxLon->rVal();
	if (laloMin.Lat < laloMax.Lat && laloMin.Lon < laloMax.Lon)
	{
		long iRow, iCol;
		double rLat, rLon;
		iRow = rcSize.Row;
		iCol = rcSize.Col;
		if (fCenter)
		{
			iCol -= 1;
			iRow -= 1;
		} 
		rLon = (laloMax.Lon - laloMin.Lon) / iCol;
		rLat = (laloMax.Lat - laloMin.Lat) / iRow;

		String sLat = LatLon::sDegree(rLat); 
		String sLon = LatLon::sDegree(rLon); 
		String s(SGRRemPixSizeLatLon_SS.c_str(), sLon, sLat);
		stRemark->SetVal(s);
		EnableOK();
	}
	else {
		stRemark->SetVal(SGRRemMinSmallerMax);
		DisableOK();
	}
	return 0;  
}

int	GeoRefCornersView::CallBackCenterOfCorner(Event*)
{
	try {
		cbCoC->StoreData();
		CoordSystem cs(sCoordSystem);
		if (cs.fValid()) {
			fFromLatLon = ( 0 != cs->pcsLatLon());
			if (!fFromLatLon)
				CallBackMinMaxXY(0);
			else
				CallBackMinMaxLatLon(0);
		}
		else {
			fgLatLon->Hide();
			fgCoord->Hide();
		}
	}
	catch (const ErrorObject&) {
		fgLatLon->Hide();
		fgCoord->Hide();
	}
	return 0;
}

