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
/* FieldGeoRef
   by Wim Koolhoven, june 1995
   Copyright Ilwis System Development ITC
	Last change:  WK   28 Apr 98   11:59 am
*/
#include "Client\Headers\formelementspch.h"
#include "Client\ilwis.h"
#include "Client\FormElements\fldgrf.h"
#include "Client\FormElements\fldcs.h"
#include "Engine\SpatialReference\Gr.h"
#include "Engine\SpatialReference\GRNONE.H"
#include "Engine\SpatialReference\Grsmpl.h"
#include "Engine\SpatialReference\Grcornrs.h"
#include "Engine\SpatialReference\Grctppla.h"
#include "Engine\SpatialReference\Grortho.h"
#include "Engine\SpatialReference\grdirlin.h"
#include "Engine\SpatialReference\GrParallProj.h"
#include "Engine\SpatialReference\GR3D.H"
#include "Engine\SpatialReference\GRCTP.H"
#include "Engine\Base\Round.h"
#include "Headers\Htp\Ilwismen.htp"
#include "Client\FormElements\objlist.h"
#include "Headers\Hs\Georef.hs"
#include "Headers\Hs\Coordsys.hs"
#include "Headers\Hs\Mainwind.hs"



FieldGeoRefC::FieldGeoRefC(FormEntry* fe, const String& sQuestion, 
               String* sGeoRef, long types,
               CoordSystem csys, 
               CoordBounds cbnds)
: FieldDataTypeC(fe, sQuestion, sGeoRef, new GeoRefLister(types), true, 
   (NotifyProc)&FieldGeoRefC::CreateGeoRef),              
   cs(csys), cb(cbnds), fEditStart(false)
{ 
  sNewName = *sGeoRef; 
}

FieldGeoRefC::FieldGeoRefC(FormEntry* fe, const String& sQuestion, 
               String* sGeoRef, const GeoRef& grf)
: FieldDataTypeC(fe, sQuestion, sGeoRef, new GeoRefLister(grALL), true, 
   (NotifyProc)&FieldGeoRefC::CreateGeoRef),              
   cs(grf->cs()), cb(grf->cb()), fEditStart(false)
{ 
  sNewName = *sGeoRef; 
}

FieldGeoRefC::FieldGeoRefC(FormEntry* fe, const String& sQuestion, 
               String* sGeoRef, const GeoRef& grf, const Map& mp, bool fStartEdit)
  : FieldDataTypeC(fe, sQuestion, sGeoRef, new GeoRefLister(grALL), true, 
   (NotifyProc)&FieldGeoRefC::CreateGeoRef),              
   map(mp), cs(grf->cs()), cb(grf->cb()), fEditStart(fStartEdit)
{ 
  sNewName = *sGeoRef; 
}

void FieldGeoRefC::SetBounds(const CoordSystem& csys, const CoordBounds& cbnd)             
{ 
  cs = csys; 
  cb = cbnd; 
}

int FieldGeoRefC::CreateGeoRef(void*)
{
  FileName fn(sNewName);
  if (fn.sFile == "none")
    sNewName = "";
  bool fOk;
  if (!map.fValid()) {
    FormCreateGeoRef form(frm()->wnd(), &sNewName, cs, cb, true);
    fOk = form.fOkClicked();
  }
  else {
    FormCreateGeoRefRC form(frm()->wnd(), &sNewName, map, cs, cb, false, fEditStart);
    fOk = form.fOkClicked();
  }  
  if (fOk) {
    FillDir();
    FileName fn(sNewName);
    SetVal(fn.sFileExt());
    CallCallBacks();  
  }  
  return 0;
}

FormCreateGeoRef::FormCreateGeoRef(CWnd* wPar, String* sGrf,
                   CoordSystem cs, CoordBounds cb, bool fOnlyCorners)
: FormWithDest(wPar, TR("Create GeoReference")), wParent(wPar),
  sGeoRef(sGrf), crdMin(cb.cMin), crdMax(cb.cMax),
  fgr(0), rg(0), fgCorn(0), stRemark(0), cbSubPixel(0)
{
	iImg = IlwWinApp()->iImage(".grf");

	sNewName = *sGeoRef;
	sCoordSys = cs->sName();
	if ("unknown" == sCoordSys)
		sCoordSys = "";
	fgr = new FieldDataTypeCreate(root, TR("&GeoReference Name"), &sNewName, ".GRF", false);
	fgr->SetIndependentPos();
	fgr->SetCallBack((NotifyProc)&FormCreateGeoRef::NameChange);
	StaticText* st = new StaticText(root, TR("&Description:"));
	st->psn->SetBound(0,0,0,0);
	FieldString* fs = new FieldString(root, "", &sDescr);
	fs->SetWidth(120);
	fs->SetIndependentPos();
	
	fSubPixelPrecise = false;
	fCallBackGrfSubPixelCalled = false;
	iOption = 0;
	RadioButton* rbCorners = 0;
	RadioButton* rbCTP = 0;
	RadioButton* rbDirLin = 0;
	RadioButton* rbOrthoPhoto = 0;
	RadioButton* rbParallProj = 0;
	RadioButton* rb3D = 0;
	if (fOnlyCorners) {
		fgCorn = new FieldGroup(root, true);
		fCoC = false; // because the incoming cb is "corners"
	} else {
		rg = new RadioGroup(root, "", &iOption);
		rg->SetCallBack((NotifyProc)&FormCreateGeoRef::GeoRefTypeChange);
		rbCorners = new RadioButton(rg, TR("GeoRef &Corners"));
		rbCTP = new RadioButton(rg, TR("GeoRef &Tiepoints"));
		rbDirLin = new RadioButton(rg, TR("GeoRef &Direct Linear"));
		rbOrthoPhoto = new RadioButton(rg, TR("GeoRef &Ortho Photo"));
		rbParallProj = new RadioButton(rg, TR("GeoRef &Parallel Projective"));
		rb3D = new RadioButton(rg, TR("GeoRef &3-D display"));
		fgCorn = new FieldGroup(rbCorners, true);
		fgCorn->Align(rb3D, AL_UNDER);
		fCoC = true; // here there was no incoming "cb"; leave it "true" as this was the default for many years
	}
	
	fcsc = new FieldCoordSystemC(fgCorn, TR("&Coordinate System"), &sCoordSys);
	fcsc->SetCallBack((NotifyProc)&FormCreateGeoRef::CSysCallBack);
  
	bool fCrdUndef = false;
	if (crdMin.x >= crdMax.x) {
		fCrdUndef = true;
		crdMin.x = 0;
		crdMax.x = 0;
	}
	if (crdMin.y >= crdMax.y) {
		fCrdUndef = true;
		crdMin.y = 0;
		crdMax.y = 0;
	}
	fFromLatLon = ( 0 != cs->pcsLatLon());
	if (fCrdUndef) {
		rPixSize = 10;
		rPixSizeDMS = rRoundDMS(0.1);
	}
	else 
	{
		double r = max(crdMax.x - crdMin.x, crdMax.y - crdMin.y);
		r /= 800;
		rPixSize = rRound(r);
		rPixSizeDMS = rRoundDMS(r);
		if (fFromLatLon) {
			llMin = LatLon(crdMin.y, crdMin.x);
			llMax = LatLon(crdMax.y, crdMax.x);
		}
	}
	ValueRange vrrPixSize(0.001, 1e6, 0.001);
	
	// GeoRefCorners in meters
	fgCsyMeters = new FieldGroup(fgCorn, true);
	fgCsyMeters->Align(fcsc, AL_UNDER);
	
	fldPixInMeters = new FieldReal(fgCsyMeters, TR("&Pixel size"), &rPixSize, vrrPixSize);
	fldPixInMeters->SetCallBack((NotifyProc)&FormCreateGeoRef::CallBackCorners);
	
	fldCrdMin = new FieldCoord(fgCsyMeters, TR("&Min X, Y"), &crdMin);
	fldCrdMin->SetCallBack((NotifyProc)&FormCreateGeoRef::CallBackCorners);
	fldCrdMax = new FieldCoord(fgCsyMeters, TR("&Max X, Y"), &crdMax);
	fldCrdMax->SetCallBack((NotifyProc)&FormCreateGeoRef::CallBackCorners);

	// GeoRefCorners in LatLon
	fgCsyLatLons = new FieldGroup(fgCorn, true);
	fgCsyLatLons->Align(fcsc, AL_UNDER);
	
	fldPixInDegMinSec = new FieldDMS(fgCsyLatLons, TR("&Pixel size"), &rPixSizeDMS, 30.0, true);
	fldPixInDegMinSec->SetCallBack((NotifyProc)&FormCreateGeoRef::CallBackCorners);
	
	fldMinLat = new FieldLat(fgCsyLatLons, TR("&MinLatLon"), &llMin.Lat);
	fldMinLat->SetCallBack((NotifyProc)&FormCreateGeoRef::CallBackCorners);
	fldMinLat->Align(fldPixInDegMinSec, AL_UNDER);
	fldMinLon = new FieldLon(fgCsyLatLons, "", &llMin.Lon);
	fldMinLon->SetCallBack((NotifyProc)&FormCreateGeoRef::CallBackCorners);
	fldMinLon->Align(fldMinLat, AL_AFTER);
	fldMaxLat = new FieldLat(fgCsyLatLons, TR("&MaxLatLon"), &llMax.Lat);
	fldMaxLat->SetCallBack((NotifyProc)&FormCreateGeoRef::CallBackCorners);
	fldMaxLat->Align(fldMinLat, AL_UNDER);
	fldMaxLon = new FieldLon(fgCsyLatLons, "", &llMax.Lon);
	fldMaxLon->SetCallBack((NotifyProc)&FormCreateGeoRef::CallBackCorners);
	fldMaxLon->Align(fldMaxLat, AL_AFTER);
	
	cbCoC = new CheckBox(fgCorn, TR("&Center of Corner pixels"), &fCoC);
	cbCoC->Align(fldMaxLat, AL_UNDER);
	cbCoC->SetIndependentPos();
	cbCoC->SetCallBack((NotifyProc)&FormCreateGeoRef::CallBackCorners);

	if (!fOnlyCorners) {
		FieldGroup* fgCTP = new FieldGroup(rbCTP,true);
		fgCTP->Align(rg, AL_UNDER);
		new FieldCoordSystemC(fgCTP, TR("&Coordinate System"), &sCoordSys);
		new FieldDataType(fgCTP, TR("&Background Map"), &sRefMap, ".mpr.mpl", true);
		FieldBlank* fb = new FieldBlank(root,8);
		fb->Align(fb, AL_UNDER);
		cbSubPixel = new CheckBox(fb, "Sub-Pixel Precision", &fSubPixelPrecise);
		cbSubPixel->SetCallBack((NotifyProc)&FormCreateGeoRef::CallBackGrfSubPixelChange);

		FieldGroup* fgDirLin = new FieldGroup(rbDirLin, true);
		fgDirLin->Align(rg, AL_UNDER);
		new FieldDataType(fgDirLin, TR("&Background Map"), &sRefMap, ".mpr.mpl", true);
		new FieldDataType(fgDirLin, TR("&DTM"), &sDTM, new MapListerDomainType(dmVALUE), true);
		
		FieldGroup* fgOrthoPhoto = new FieldGroup(rbOrthoPhoto, true);
		fgOrthoPhoto->Align(rg, AL_UNDER);
		new FieldDataType(fgOrthoPhoto, TR("&Background Map"), &sRefMap, ".mpr.mpl", true);
		new FieldDataType(fgOrthoPhoto, TR("&DTM"), &sDTM, new MapListerDomainType(dmVALUE), true);

		FieldGroup* fgParallProj = new FieldGroup(rbParallProj, true);
		fgParallProj->Align(rg, AL_UNDER);
		new FieldDataType(fgParallProj, TR("&Background Map"), &sRefMap, ".mpr.mpl", true);
		new FieldDataType(fgParallProj, TR("&DTM"), &sDTM, new MapListerDomainType(dmVALUE), true);
		
		FieldGroup* fg3D = new FieldGroup(rb3D,true);
		fg3D->Align(rg, AL_UNDER);
		rcSize = RowCol(300L,400L);
		new FieldRowCol(fg3D, TR("&Rows, Columns"), &rcSize);
		new FieldDataType(fg3D, TR("&DTM"), &sDTM, new MapListerDomainType(dmVALUE), true);
	}
	
	String s('X', 50);
	stRemark = new StaticText(root, s);
	stRemark->SetIndependentPos();
	stRemark->Align(fgCorn, AL_UNDER);
	stRemark->SetVal(String());
	if (fOnlyCorners) 
		SetMenHelpTopic("ilwismen\\create_a_georeference.htm");
	else
		SetMenHelpTopic("ilwismen\\create_a_georeference.htm");
	
	m_fInShowHide = false;
	m_fInSetVal = false;
	
	create();
}

FormCreateGeoRef::~FormCreateGeoRef()
{
  if (wParent)
    ::SetActiveWindow(*wParent);
}

FormEntry* FormCreateGeoRef::feDefaultFocus()  
{
  fgr->SelectAll();
  return fgr;
}

// take care of the showing/hiding of the Geograph or Latlon coord fields
// fHideAll overrules and is used to hide both (for all non-georefcorners
void FormCreateGeoRef::ShowHide(bool fLatLon, bool fHideAll)
{
	if (m_fInShowHide)
		return;
	m_fInShowHide = true;
	fgCsyMeters->Hide();
	fgCsyLatLons->Hide();
	if (!fHideAll)
	{
		if (fLatLon)
			fgCsyLatLons->Show();
		else
			fgCsyMeters->Show();
	}	

	m_fInShowHide = false;
}

int FormCreateGeoRef::NameChange(Event*)
{
	if (fgr)
		fgr->StoreData();

	FileName fn(sNewName, ".grf");

	m_fNameOK = false;
	if (!fn.fValid())
		stRemark->SetVal(TR("Not a valid GeoReference name"));
	else if(File::fExist(fn))   
		stRemark->SetVal(TR("GeoReference already exists"));
	else
	{
		stRemark->SetVal(String());
		m_fNameOK = true;
	}
	
	SetOkButton();

	return m_fNameOK ? 0 : 1;  // != 0 means namechange detected an error
}

int FormCreateGeoRef::CSysCallBack(Event*) 
{
	if (m_fInShowHide)
		return 0;

	try 
	{
		fFromLatLon = false;
		FileName fnCS = FileName(sCoordSys, ".csy"); // remember previous selected CSY
		fcsc->StoreData();                           // get the new CSY

		if ("" == sCoordSys) {
			ShowHide(true, true); // hide all
			return 0;
		}
		CoordSystem cs(sCoordSys);
		if (cs.fValid())
		{
			fFromLatLon = (0 != cs->pcsLatLon());

			Coord cMin;
			Coord cMax;
			CoordBounds cbLoc;
			double rEps = 1; // geographic coordinates
			if (fFromLatLon)
			{
				cbLoc = CoordBounds(Coord(llMin.Lon, llMin.Lat), Coord(llMax.Lon, llMax.Lat));
				rEps = 1e-8;
			}
			else
				cbLoc = CoordBounds(crdMin, crdMax);

			cMin = cbLoc.cMin;
			cMax = cbLoc.cMax;
			if (cbLoc.fUndef() || rDist2(cMin, cMax) < rEps)
			{
				if (cs->cb.fUndef())
					cMin = cMax = Coord(0,0);
				else
				{
					cMin = cs->cb.cMin;
					cMax = cs->cb.cMax;
				}
			}
			
			double r = max(cMax.x - cMin.x, cMax.y - cMin.y);
			r /= 800;
			if (!fFromLatLon)
			{
				if (fnCS != cs->fnObj)  // only change bounds when CS really changes
				{
					m_fInSetVal = true;
					rPixSize = rRound(r);
					crdMin = cMin;
					crdMax = cMax;
					fldCrdMin->SetVal(cMin);
					fldCrdMax->SetVal(cMax);
					fldPixInMeters->SetVal(rPixSize);
					m_fInSetVal = false;
				}
			}
			else
			{
				if (fnCS != cs->fnObj)  // only change bounds when CS really changes
				{
					m_fInSetVal = true;
					rPixSizeDMS = rRoundDMS(r);
					llMin = LatLon(cMin.y, cMin.x);
					llMax = LatLon(cMax.y, cMax.x);
					fldMinLat->SetVal(cMin.y);
					fldMaxLat->SetVal(cMax.y);
					fldMinLon->SetVal(cMin.x);
					fldMaxLon->SetVal(cMax.x);
					fldPixInDegMinSec->SetVal(rPixSizeDMS);
					m_fInSetVal = false;
				}
			}
			CallBackCorners(0);
			ShowHide(fFromLatLon, false); // show relevant controls
		}
		else 
		{
			ShowHide(true, true); // hide all
		}
	}
	catch (ErrorObject&) 
	{
		ShowHide(true, true); // hide all
	}  
  return 0;  
}

int FormCreateGeoRef::CallBackCorners(Event*)
{
	if (m_fInShowHide || m_fInSetVal)
		return 0;

	m_fBoundsOK = false;
	if (fgCorn)
		fgCorn->StoreData();

	try {
		FileName fnCS(sCoordSys);
		if (!fnCS.fValid())
			return 0;
		
		CoordSystem csInput(fnCS);
		if (!csInput.fValid())
			return 0;
		
		fFromLatLon = (0 != csInput->pcsLatLon());

		if (iOption == 0)  // GeoRefCorners is selected
		{
			CoordBounds cbLoc;
			double rPSize;
			double rEps = 1; // geographic coordinates
			if (fFromLatLon)
			{
				m_fBoundsOK = rPixSizeDMS > 0.00000001;
				cbLoc = CoordBounds(Coord(llMin.Lon, llMin.Lat), Coord(llMax.Lon, llMax.Lat));
				rPSize = rPixSizeDMS;
				rEps = 1e-8;
			}
			else
			{
				m_fBoundsOK = rPixSize > 0.001;
				cbLoc = CoordBounds(crdMin, crdMax);
				rPSize = rPixSize;
			}

			Coord cMin = cbLoc.cMin;
			Coord cMax = cbLoc.cMax;


			if (m_fBoundsOK)
			{
				if (cMin.x < cMax.x && cMin.y < cMax.y)
				{
					rcSize.Col = ceil((cMax.x - cMin.x) / rPSize);
					rcSize.Row = ceil((cMax.y - cMin.y) / rPSize);
					double rDeltaX = (rcSize.Col * rPSize - (cMax.x - cMin.x)) / 2;
					double rDeltaY = (rcSize.Row * rPSize - (cMax.y - cMin.y)) / 2;
					cMin.x -= rDeltaX;
					cMin.y -= rDeltaY;
					cMax.x += rDeltaX;
					cMax.y += rDeltaY;
					if (fCoC)
					{
						rcSize.Col += 1;
						rcSize.Row += 1;
					} 
					String s(TR("%li lines and %li columns").c_str(), rcSize.Row, rcSize.Col);
					stRemark->SetVal(s);
					m_fBoundsOK = true;
				}
				else
				{
					stRemark->SetVal(TR("Minimum should be smaller than maximum"));
					m_fBoundsOK = false;
				}
			}
			else
				if (rPixSize != rUNDEF && rPixSize <= 0.001)
					stRemark->SetVal(TR("Pixel size too small"));
		}
	}
	catch (ErrorObject&) 
	{
		// do nothing
	}
	SetOkButton();

	return 0;
}

void FormCreateGeoRef::SetOkButton()
{
	if (m_fNameOK && m_fBoundsOK)
		EnableOK();
	else    
		DisableOK();
}

int FormCreateGeoRef::GeoRefTypeChange(Event*)
{
	if (m_fInShowHide)
		return 0;

	if (rg)  
		rg->StoreData();
	if ((0 != iOption) && (5 != iOption)) {
		m_fBoundsOK = true; // only relevant for GeoRefCorners
		SetOkButton();
		if (cbSubPixel)
			cbSubPixel->Show();
	}
	else if (5 == iOption)
	{
		m_fBoundsOK = true; // only relevant for GeoRefCorners
		SetOkButton();
		if (cbSubPixel)
			cbSubPixel->Hide();
	}
	else
	{
		if (cbSubPixel)
			cbSubPixel->Hide();
	}
	if ("" == sCoordSys) {
		ShowHide(true, true); // hide all
		return 0;
	}
	try {
		CoordSystem csInput(sCoordSys);
		fFromLatLon = (0 != csInput->pcsLatLon());
		ShowHide(fFromLatLon, iOption != 0); // show relevant controls
	}
	catch (ErrorObject&)
	{
		ShowHide(true, true); // hide all
	}  
	return 0;
}

int FormCreateGeoRef::CallBackGrfSubPixelChange(Event*)
{
	if (fCallBackGrfSubPixelCalled) 
		return 0;
	fCallBackGrfSubPixelCalled = true;
	cbSubPixel->StoreData();
	if (rg)
		rg->StoreData();
	cbSubPixel->SetVal(fSubPixelPrecise);
	fCallBackGrfSubPixelCalled = false;
	return 1;
}

int FormCreateGeoRef::exec()
{
	try {
		FormWithDest::exec();
		*sGeoRef = sNewName;
		FileName fn(*sGeoRef, ".grf");
		if ("" == sCoordSys)
			sCoordSys = "unknown"; // to prevent errors
		FileName fnCsy = sCoordSys;
		CoordSystem cs(fnCsy);
		Coord cMin, cMax;
		double rPSize;
		if (cs->pcsLatLon())
		{
			cMin = Coord(llMin.Lon, llMin.Lat);
			cMax = Coord(llMax.Lon, llMax.Lat);
			rPSize = rPixSizeDMS;
		}
		else
		{
			cMin = crdMin;
			cMax = crdMax;
			rPSize = rPixSize;
		}
		switch (iOption) {
		case 0: {
				RowCol rc = rcSize;
				if (fCoC)
				{
					rc.Col -= 1;
					rc.Row -= 1;
				} 
				double rDeltaX = (rc.Col * rPSize - (cMax.x - cMin.x)) / 2;
				double rDeltaY = (rc.Row * rPSize - (cMax.y - cMin.y)) / 2;
				cMin.x -= rDeltaX;
				cMin.y -= rDeltaY;
				cMax.x += rDeltaX;
				cMax.y += rDeltaY;
				GeoRefCorners grc(fn, cs, rcSize, !fCoC, cMin, cMax);
				if (sDescr != "")
					grc.sDescription = sDescr;
				else  
					grc.sDescription = grc.sTypeName();
				grc.Store();
			}
			break;
		case 1: {
				FileName fnRef(sRefMap);
				MapList ml;
				Map mp;
				FileName fnBackground;
				if (".mpl" == fnRef.sExt) {
					ml = MapList(fnRef);
					rcSize = ml->rcSize();
					fnBackground = ml->fnObj;
				}
				else {
					mp = Map(fnRef);
					rcSize = mp->rcSize();
					fnBackground = mp->fnObj;
				}
				GeoRef grf;
				GeoRefCTPplanar* pgrfPLA = new GeoRefCTPplanar(fn, cs, rcSize,fSubPixelPrecise);
				pgrfPLA->fnBackgroundMap = fnRef;
				grf.SetPointer(pgrfPLA);

				if (sDescr != "")
					grf->sDescription = sDescr;
				else  
					grf->SetDescription(grf->sTypeName());
				grf->Store();
				if (mp.fValid())
					mp->SetGeoRef(grf);
				else if (ml.fValid())
					ml->SetGeoRef(grf);
			}
			break;
		case 2: {
				FileName fnRef(sRefMap);
				MapList ml;
				Map mp;
				FileName fnBackground;
				if (".mpl" == fnRef.sExt) {
					ml = MapList(fnRef);
					rcSize = ml->rcSize();
					fnBackground = ml->fnObj;
				}
				else {
					mp = Map(fnRef);
					rcSize = mp->rcSize();
					fnBackground = mp->fnObj;
				}
				Map mpDTM(sDTM);
				GeoRef grf;
				GeoRefDirectLinear* pgrfDL = new GeoRefDirectLinear(fn, mpDTM, rcSize,fSubPixelPrecise);
				pgrfDL->fnBackgroundMap = fnBackground;
				grf.SetPointer(pgrfDL);
				if (sDescr != "")
					grf->sDescription = sDescr;
				else  
					grf->SetDescription(grf->sTypeName());
				grf->Store();
				if (mp.fValid())
					mp->SetGeoRef(grf);
				else if (ml.fValid())
					ml->SetGeoRef(grf);
			}
			break;
		case 3: {
				FileName fnRef(sRefMap);
				MapList ml;
				Map mp;
				FileName fnBackground;
				if (".mpl" == fnRef.sExt) {
					ml = MapList(fnRef);
					rcSize = ml->rcSize();
					fnBackground = ml->fnObj;
				}
				else {
					mp = Map(fnRef);
					rcSize = mp->rcSize();
					fnBackground = mp->fnObj;
				}
				Map mpDTM(sDTM);
				GeoRef grf;
				GeoRefOrthoPhoto* pgrfOrt = new GeoRefOrthoPhoto(fn, mpDTM, rcSize, fSubPixelPrecise);
				pgrfOrt->fnBackgroundMap = fnBackground;
				grf.SetPointer(pgrfOrt);
				if (sDescr != "")
					grf->sDescription = sDescr;
				else  
					grf->SetDescription(grf->sTypeName());
				grf->Store();
				if (mp.fValid())
					mp->SetGeoRef(grf);
				else if (ml.fValid())
					ml->SetGeoRef(grf);
			}
			break;
		case 4: {
				FileName fnRef(sRefMap);
				MapList ml;
				Map mp;
				FileName fnBackground;
				if (".mpl" == fnRef.sExt) {
					ml = MapList(fnRef);
					rcSize = ml->rcSize();
					fnBackground = ml->fnObj;
				}
				else {
					mp = Map(fnRef);
					rcSize = mp->rcSize();
					fnBackground = mp->fnObj;
				}
				Map mpDTM(sDTM);
				GeoRef grf;
				GeoRefParallelProjective* pgrfParall = new GeoRefParallelProjective(fn, mpDTM, rcSize, fSubPixelPrecise);
				pgrfParall->fnBackgroundMap = fnBackground;
				grf.SetPointer(pgrfParall);
				if (sDescr != "")
					grf->sDescription = sDescr;
				else  
					grf->SetDescription(grf->sTypeName());
				grf->Store();
				if (mp.fValid())
					mp->SetGeoRef(grf);
				else if (ml.fValid())
					ml->SetGeoRef(grf);
			}
			break;
		case 5: {
				Map mp(sDTM);
				GeoRef3D gr3D(fn, rcSize, mp);
				if (sDescr != "")
					gr3D.sDescription = sDescr;
				else  
					gr3D.sDescription = gr3D.sTypeName();
				gr3D.Store();
			}
			break;
		}
	}
	catch (ErrorObject& err) {
		err.Show();
	}
	return 1;
}

FormCreateGeoRefRC::FormCreateGeoRefRC(CWnd* wPar, String* sGrf, 
  const Map& mp, CoordSystem cs, CoordBounds cb, bool fOnlyTiep, bool fStartEdit)
: FormWithDest(wPar, TR("Create GeoReference")),
  sGeoRef(sGrf), map(mp), rcSize(mp->rcSize()), fOnlyTiepoints(fOnlyTiep),
  crdMin(cb.cMin), crdMax(cb.cMax), fEditStart(fStartEdit),
  fgr(0), rg(0), stRemark(0), fgCorners(0), cbSubPixel(0)
{
  iImg = IlwWinApp()->iImage(".grf");
  sNewName = *sGeoRef;
  sCoordSys = cs->sName();
  fgr = new FieldDataTypeCreate(root, TR("&GeoReference Name"), &sNewName, ".GRF", false);
  fgr->SetIndependentPos();
  fgr->SetCallBack((NotifyProc)&FormCreateGeoRefRC::CallBack);
  StaticText* st = new StaticText(root, TR("&Description:"));
  st->psn->SetBound(0,0,0,0);
  FieldString* fs = new FieldString(root, "", &sDescr);
  fs->SetWidth(120);
  fs->SetIndependentPos();

  if (crdMin.x >= crdMax.x) {
    crdMin.x = 0;
    crdMax.x = 0.1;
  }
  if (crdMin.y >= crdMax.y) {
    crdMin.y = 0;
    crdMax.y = 0.1;
  }
  fFromLatLon = ( 0 != cs->pcsLatLon());
  if (fFromLatLon) {
    llMin = LatLon(crdMin.y, crdMin.x);
    llMax = LatLon(crdMax.y, crdMax.x);
  }
  iType = 0;
  fCallBackGrfSubPixelCalled = false;
  fSubPixelPrecise = false;
  rg = new RadioGroup(root, "", &iType);
  rg->SetCallBack((NotifyProc)&FormCreateGeoRefRC::CallBack);
  RadioButton* rbCorners = 0;
  if (!fOnlyTiepoints)
    rbCorners = new RadioButton(rg, TR("GeoRef &Corners"));
  RadioButton* rbTiePoints = new RadioButton(rg, TR("GeoRef &Tiepoints"));
  RadioButton* rbDirLin = new RadioButton(rg, TR("GeoRef &Direct Linear"));
  RadioButton* rbOrthoPhoto = new RadioButton(rg, TR("GeoRef &Ortho Photo"));
  RadioButton* rbParallProj = new RadioButton(rg, TR("GeoRef &Parallel Projective"));

  if (rbCorners) {
    fCoC = false; // because the incoming cb is "corners"
    fgCorners = new FieldGroup(rbCorners, true);
    fgCorners->Align(rg, AL_UNDER);
    fcsc = new FieldCoordSystemC(fgCorners, TR("&Coordinate System"), &sCoordSys);
	fcsc->SetCallBack((NotifyProc)&FormCreateGeoRefRC::CSysCallBack);

	// GeoRefCorners in meters
	fgCsyMeters = new FieldGroup(fgCorners, true);
	fgCsyMeters->Align(fcsc, AL_UNDER);
    fldCrdMin = new FieldCoord(fgCsyMeters, TR("&Min X, Y"), &crdMin);
	fldCrdMin->SetCallBack((NotifyProc)&FormCreateGeoRefRC::CallBack);
    fldCrdMax = new FieldCoord(fgCsyMeters, TR("&Max X, Y"), &crdMax);
	fldCrdMax->SetCallBack((NotifyProc)&FormCreateGeoRefRC::CallBack);

	// GeoRefCorners in LatLon
	fgCsyLatLons = new FieldGroup(fgCorners, true);
	fgCsyLatLons->Align(fcsc, AL_UNDER);
	fldMinLat = new FieldLat(fgCsyLatLons, TR("&MinLatLon"), &llMin.Lat);
	fldMinLat->SetCallBack((NotifyProc)&FormCreateGeoRefRC::CallBack);
	fldMinLat->Align(fcsc, AL_UNDER);
	fldMinLon = new FieldLon(fgCsyLatLons, "", &llMin.Lon);
	fldMinLon->SetCallBack((NotifyProc)&FormCreateGeoRefRC::CallBack);
	fldMinLon->Align(fldMinLat, AL_AFTER);
	fldMaxLat = new FieldLat(fgCsyLatLons, TR("&MaxLatLon"), &llMax.Lat);
	fldMaxLat->SetCallBack((NotifyProc)&FormCreateGeoRefRC::CallBack);
	fldMaxLat->Align(fldMinLat, AL_UNDER);
	fldMaxLon = new FieldLon(fgCsyLatLons, "", &llMax.Lon);
	fldMaxLon->SetCallBack((NotifyProc)&FormCreateGeoRefRC::CallBack);
	fldMaxLon->Align(fldMaxLat, AL_AFTER);

    cbCoC = new CheckBox(fgCorners, TR("&Center of Corner pixels"), &fCoC);
	cbCoC->Align(fldMaxLat, AL_UNDER);
    cbCoC->SetIndependentPos();
    fgCorners->SetCallBack((NotifyProc)&FormCreateGeoRefRC::CallBack);
    cbCoC->SetCallBack((NotifyProc)&FormCreateGeoRefRC::CallBack);
  }

  if (rbTiePoints) {
    FieldGroup* fg = new FieldGroup(rbTiePoints, true);
    fg->Align(rg, AL_UNDER);
    new FieldCoordSystemC(fg, TR("&Coordinate System"), &sCoordSys);
  }

  if (rbDirLin) {
    FieldGroup* fgDirLin = new FieldGroup(rbDirLin, true);
    fgDirLin->Align(rg, AL_UNDER);
    new FieldDataType(fgDirLin, TR("&DTM"), &sDTM, new MapListerDomainType(dmVALUE), true);
  }
   
	FieldGroup* fgOrthoPhoto = 0;
  if (rbOrthoPhoto) {
    fgOrthoPhoto = new FieldGroup(rbOrthoPhoto, true);
    fgOrthoPhoto->Align(rg, AL_UNDER);
    new FieldDataType(fgOrthoPhoto, TR("&DTM"), &sDTM, new MapListerDomainType(dmVALUE), true);
  }

	if (rbParallProj) {
		FieldGroup* fgParallProj= new FieldGroup(rbParallProj, true);
		fgParallProj->Align(rg, AL_UNDER);
		new FieldDataType(fgParallProj, TR("&DTM"), &sDTM, new MapListerDomainType(dmVALUE), true);
  }

	if (rbTiePoints||rbDirLin||rbOrthoPhoto||rbParallProj)
	{
		cbSubPixel = new CheckBox(root, "Sub-Pixel Precision", &fSubPixelPrecise);
		if (fgOrthoPhoto)
			cbSubPixel->Align(fgOrthoPhoto, AL_UNDER);
		else
			cbSubPixel->Align(rg, AL_UNDER);
		cbSubPixel->SetCallBack((NotifyProc)&FormCreateGeoRefRC::CallBackGrfSubPixelChange);
	}

	String s('X', 50);
  stRemark = new StaticText(root, s);
	if (fgCorners)
    stRemark->Align(fgCorners, AL_UNDER);
  else if (cbSubPixel)
		stRemark->Align(cbSubPixel, AL_UNDER);
  else if (fgOrthoPhoto)
    stRemark->Align(fgOrthoPhoto, AL_UNDER);

  stRemark->SetIndependentPos();
  
	if (fOnlyTiepoints)
    SetMenHelpTopic("ilwismen\\create_a_georeference.htm");
  else  
    SetMenHelpTopic("ilwismen\\create_a_georeference_through_properties.htm");

  m_fInShowHide = false;
  m_fInSetVal = false;
  create();
}

// take care of the showing/hiding of the Geograph or Latlon coord fields
// fHideAll overrules and is used to hide both (for all non-georefcorners
void FormCreateGeoRefRC::ShowHide(bool fLatLon, bool fHideAll)
{
	if (m_fInShowHide)
		return;
	if (fOnlyTiepoints)
		return;
	m_fInShowHide = true;
	fgCsyMeters->Hide();
	fgCsyLatLons->Hide();
	if (!fHideAll)
	{
		if (fLatLon)
			fgCsyLatLons->Show();
		else
			fgCsyMeters->Show();
	}	

	m_fInShowHide = false;
}

FormEntry* FormCreateGeoRefRC::feDefaultFocus()  
{
  fgr->SelectAll();
  return fgr;
}

int FormCreateGeoRefRC::CallBackGrfSubPixelChange(Event*)
{
	if (fCallBackGrfSubPixelCalled) 
		return 0;
	fCallBackGrfSubPixelCalled = true;
	cbSubPixel->StoreData();
	if (rg)
		rg->StoreData();
	cbSubPixel->SetVal(fSubPixelPrecise);
	fCallBackGrfSubPixelCalled = false;
	return 1;
}

int FormCreateGeoRefRC::CSysCallBack(Event*) 
{
	if (m_fInShowHide)
		return 0;

	try 
	{
		fFromLatLon = false;
		FileName fnCS = FileName(sCoordSys, ".csy"); // remember previous selected CSY
		fcsc->StoreData();                           // get the new CSY

		if ("" == sCoordSys) {
			ShowHide(true, true); // hide all
			return 0;
		}
		CoordSystem cs(sCoordSys);
		if (cs.fValid())
		{
			fFromLatLon = (0 != cs->pcsLatLon());

			Coord cMin;
			Coord cMax;
			CoordBounds cbLoc;
			double rEps = 1; // geographic coordinates
			if (fFromLatLon)
			{
				cbLoc = CoordBounds(Coord(llMin.Lon, llMin.Lat), Coord(llMax.Lon, llMax.Lat));
				rEps = 1e-8;
			}
			else
				cbLoc = CoordBounds(crdMin, crdMax);

			cMin = cbLoc.cMin;
			cMax = cbLoc.cMax;
			if (cbLoc.fUndef() || rDist2(cMin, cMax) < rEps)
			{
				if (cs->cb.fUndef())
					cMin = cMax = Coord(0,0);
				else
				{
					cMin = cs->cb.cMin;
					cMax = cs->cb.cMax;
				}
			}
			
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
			CallBack(0);
			ShowHide(fFromLatLon, false); // show relevant controls
		}
		else 
		{
			ShowHide(true, true); // hide all
		}
	}
	catch (ErrorObject&) 
	{
		ShowHide(true, true); // hide all
	}  
  return 0;  
}

int FormCreateGeoRefRC::exec()
{
  try {
    FormWithDest::exec();
    if (fOnlyTiepoints)
      iType += 1;
    FileName fn = sCoordSys;
    CoordSystem cs(fn);
    *sGeoRef = sNewName;
    fn = FileName(*sGeoRef, ".grf");
    switch (iType) {
      case 0: {
        GeoRefCorners grc(fn, cs, rcSize, !fCoC, crdMin, crdMax);
        if (sDescr != "")
          grc.sDescription = sDescr;
        else  
          grc.sDescription = grc.sTypeName();
        grc.Store();
      } break;
      case 1: {
        GeoRef grf;
				GeoRefCTP* grctp = new GeoRefCTPplanar(fn, cs, rcSize, fSubPixelPrecise);
        grf.SetPointer(grctp);
				grctp->fnBackgroundMap = map->fnObj;
        if (sDescr != "")
          grf->sDescription = sDescr;
        grf->Store();
        map->SetGeoRef(grf);
        if (fEditStart) {
          String s = "edit ";
          s &= grf->sName(true);
					IlwWinApp()->Execute(s); 
        }
      } break;
      case 2: { 
        Map mpDTM(sDTM);
        GeoRef grf;
				GeoRefCTP* grctp = new GeoRefDirectLinear(fn, mpDTM, rcSize, fSubPixelPrecise );
        grf.SetPointer(grctp);
        if (sDescr != "")
          grf->sDescription = sDescr;
				grctp->fnBackgroundMap = map->fnObj;
        grf->Store();
        map->SetGeoRef(grf);
        if (fEditStart) {
          String s = "edit ";
          s &= grf->sName(true);
					IlwWinApp()->Execute(s);
        }
      } break;
      case 3: {
        Map mpDTM(sDTM);
        GeoRef grf;
				GeoRefCTP* grctp = new GeoRefOrthoPhoto(fn, mpDTM, rcSize, fSubPixelPrecise);
        grf.SetPointer(grctp);
        if (sDescr != "")
          grf->sDescription = sDescr;
				grctp->fnBackgroundMap = map->fnObj;
        grf->Store();
        map->SetGeoRef(grf);
        if (fEditStart) {
          String s = "edit ";
          s &= grf->sName(true);
					IlwWinApp()->Execute(s);
        }
      } break;
			case 4: {
        Map mpDTM(sDTM);
        GeoRef grf;
				GeoRefCTP* grctp = new GeoRefParallelProjective(fn, mpDTM, rcSize, fSubPixelPrecise);
        grf.SetPointer(grctp);
        if (sDescr != "")
          grf->sDescription = sDescr;
				grctp->fnBackgroundMap = map->fnObj;
        grf->Store();
        map->SetGeoRef(grf);
        if (fEditStart) {
          String s = "edit ";
          s &= grf->sName(true);
					IlwWinApp()->Execute(s); 
        }
      } break;
    }  
  }
  catch (ErrorObject& err) {
    err.Show();
  }  
  return 1;
}

int FormCreateGeoRefRC::CallBack(Event*)
{
  fgr->StoreData();
  bool fOk = false;
  FileName fn(sNewName, ".grf");
  if (!fn.fValid())
    stRemark->SetVal(TR("Not a valid GeoReference name"));
  else if(File::fExist(fn))   
    stRemark->SetVal(TR("GeoReference already exists"));
  else {
    rg->StoreData();
    if (iType == 0 && !fOnlyTiepoints) {
      fgCorners->StoreData();
      if (crdMin.x < crdMax.x && crdMin.y < crdMax.y) {
        double rX, rY;
        int iCorr = fCoC ? 1 : 0;
        rX = (crdMax.x - crdMin.x) / (rcSize.Col - iCorr);
        rY = (crdMax.y - crdMin.y) / (rcSize.Row - iCorr);
        String s(TR("Pixel Size = %.3f m, %.3f m").c_str(), rX, rY);
        stRemark->SetVal(s);
        fOk = true;
      }  
      else 
        stRemark->SetVal(TR("Minimum should be smaller than maximum"));
    }
    else {
      stRemark->SetVal("");
      fOk = true;
    }
  }
	if (cbSubPixel)
	{
		if (rg->iVal() == 0 && !fOnlyTiepoints)
			cbSubPixel->Hide();
		else
			cbSubPixel->Show();
	}
	try {
		CoordSystem csInput(sCoordSys);
		fFromLatLon = (0 != csInput->pcsLatLon());
		ShowHide(fFromLatLon, rg->iVal() != 0); // show relevant controls
	}
	catch (ErrorObject&)
	{
		ShowHide(true, true); // hide all
	}  
/*
  if (fOk)
    EnableOK();
  else    
    DisableOK();
*/
  return 0;
}

FieldGeoRef3DC::FieldGeoRef3DC(FormEntry* fe, const String& sQuestion, 
               String* sGeoRef)
: FieldDataTypeC(fe, sQuestion, sGeoRef, new GeoRefLister(gr3DONLY), true, 
     (NotifyProc)&FieldGeoRef3DC::CreateGeoRef)              
{ 
  sNewName = *sGeoRef; 
}

class FormCreateGeoRef3D: public FormWithDest
{
public:
  FormCreateGeoRef3D(CWnd* wPar, String* sGrf, String* sDTM, 
                     RowCol* rcSize, String* sDescr)
  : FormWithDest(wPar, TR("Create GeoReference 3D"))
  {
    new FieldDataTypeCreate(root, TR("&GeoReference Name"), sGrf, ".GRF", false);
    StaticText* st = new StaticText(root, TR("&Description:"));
    st->psn->SetBound(0,0,0,0);
    FieldString* fs = new FieldString(root, "", sDescr);
    fs->SetWidth(120);
    fs->SetIndependentPos();
    new FieldRowCol(root, TR("&Rows, Columns"), rcSize);
    new FieldMap(root, TR("&DTM"), sDTM);
    SetMenHelpTopic("ilwismen\\create_a_georeference_3d.htm");
    create();
  }                   
};

int FieldGeoRef3DC::CreateGeoRef(void*)
{
  String sGrf, sDTM, sDescr;
  RowCol rcSize(300L,400L);
  FormCreateGeoRef3D form(frm()->wnd(), &sGrf, &sDTM, &rcSize, &sDescr);
  try {
    if (form.fOkClicked()) {
      Map mp(sDTM);
      FileName fn(sGrf, ".grf");
      GeoRef3D gr3D(fn, rcSize, mp);
      if (sDescr != "")
        gr3D.sDescription = sDescr;
      gr3D.Store();
      FillDir();
      SetVal(fn.sFileExt());
    }
  }
  catch (ErrorObject& err) {
    err.Show();
  }  
  return 0;
}
