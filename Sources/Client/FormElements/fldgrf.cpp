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
: FormWithDest(wPar, SGRTitleCreateGrf), wParent(wPar),
  sGeoRef(sGrf), crdMin(cb.cMin), crdMax(cb.cMax),
  fgr(0), rg(0), fgCorn(0), stRemark(0), cbSubPixel(0)
{
	iImg = IlwWinApp()->iImage(".grf");

	sNewName = *sGeoRef;
	sCoordSys = cs->sName();
	if ("unknown" == sCoordSys)
		sCoordSys = "";
	fgr = new FieldDataTypeCreate(root, SGRUiGrfName, &sNewName, ".GRF", false);
	fgr->SetIndependentPos();
	fgr->SetCallBack((NotifyProc)&FormCreateGeoRef::NameChange);
	StaticText* st = new StaticText(root, SGRUiDescription);
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
	if (fOnlyCorners)
		fgCorn = new FieldGroup(root, true);
	else {
		rg = new RadioGroup(root, "", &iOption);
		rg->SetCallBack((NotifyProc)&FormCreateGeoRef::GeoRefTypeChange);
		rbCorners = new RadioButton(rg, SGRUiGrfCorners);
		rbCTP = new RadioButton(rg, SGRUiGrfTiepoints);
		rbDirLin = new RadioButton(rg, SGRUiGrfDirLin);
		rbOrthoPhoto = new RadioButton(rg, SGRUiGrfTiepointsPhoto);
		rbParallProj = new RadioButton(rg, SGRUiGrfParallProj);
		rb3D = new RadioButton(rg, SGRUiGrf3D);
		fgCorn = new FieldGroup(rbCorners, true);
		fgCorn->Align(rb3D, AL_UNDER);
	}
  
	fCoC = true;
	fcsc = new FieldCoordSystemC(fgCorn, SGRUiCoordSys, &sCoordSys);
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
	}
	ValueRange vrrPixSize(0.001, 1e6, 0.001);
	
	// GeoRefCorners in meters
	fgCsyMeters = new FieldGroup(fgCorn, true);
	fgCsyMeters->Align(fcsc, AL_UNDER);
	
	fldPixInMeters = new FieldReal(fgCsyMeters, SGRUiPixSize, &rPixSize, vrrPixSize);
	fldPixInMeters->SetCallBack((NotifyProc)&FormCreateGeoRef::CallBackCorners);
	
	fldCrdMin = new FieldCoord(fgCsyMeters, SGRUiMinXY, &crdMin);
	fldCrdMin->SetCallBack((NotifyProc)&FormCreateGeoRef::CallBackCorners);
	fldCrdMax = new FieldCoord(fgCsyMeters, SGRUiMaxXY, &crdMax);
	fldCrdMax->SetCallBack((NotifyProc)&FormCreateGeoRef::CallBackCorners);

	// GeoRefCorners in LatLon
	fgCsyLatLons = new FieldGroup(fgCorn, true);
	fgCsyLatLons->Align(fcsc, AL_UNDER);
	
	fldPixInDegMinSec = new FieldDMS(fgCsyLatLons, SGRUiPixSize, &rPixSizeDMS, 30.0, true);
	fldPixInDegMinSec->SetCallBack((NotifyProc)&FormCreateGeoRef::CallBackCorners);
	
	fldMinLat = new FieldLat(fgCsyLatLons, SMSUiMinLatLon, &llMin.Lat);
	fldMinLat->SetCallBack((NotifyProc)&FormCreateGeoRef::CallBackCorners);
	fldMinLat->Align(fldPixInDegMinSec, AL_UNDER);
	fldMinLon = new FieldLon(fgCsyLatLons, "", &llMin.Lon);
	fldMinLon->SetCallBack((NotifyProc)&FormCreateGeoRef::CallBackCorners);
	fldMinLon->Align(fldMinLat, AL_AFTER);
	fldMaxLat = new FieldLat(fgCsyLatLons, SMSUiMaxLatLon, &llMax.Lat);
	fldMaxLat->SetCallBack((NotifyProc)&FormCreateGeoRef::CallBackCorners);
	fldMaxLat->Align(fldMinLat, AL_UNDER);
	fldMaxLon = new FieldLon(fgCsyLatLons, "", &llMax.Lon);
	fldMaxLon->SetCallBack((NotifyProc)&FormCreateGeoRef::CallBackCorners);
	fldMaxLon->Align(fldMaxLat, AL_AFTER);
	
	cbCoC = new CheckBox(fgCorn, SGRUiCenterOfCorners, &fCoC);
	cbCoC->Align(fldMaxLat, AL_UNDER);
	cbCoC->SetIndependentPos();
	cbCoC->SetCallBack((NotifyProc)&FormCreateGeoRef::CallBackCorners);

	if (!fOnlyCorners) {
		FieldGroup* fgCTP = new FieldGroup(rbCTP,true);
		fgCTP->Align(rg, AL_UNDER);
		new FieldCoordSystemC(fgCTP, SGRUiCoordSys, &sCoordSys);
		new FieldDataType(fgCTP, SGRUiReferenceMap, &sRefMap, ".mpr.mpl", true);
		FieldBlank* fb = new FieldBlank(root,8);
		fb->Align(fb, AL_UNDER);
		cbSubPixel = new CheckBox(fb, "Sub-Pixel Precision", &fSubPixelPrecise);
		cbSubPixel->SetCallBack((NotifyProc)&FormCreateGeoRef::CallBackGrfSubPixelChange);

		FieldGroup* fgDirLin = new FieldGroup(rbDirLin, true);
		fgDirLin->Align(rg, AL_UNDER);
		new FieldDataType(fgDirLin, SGRUiReferenceMap, &sRefMap, ".mpr.mpl", true);
		new FieldDataType(fgDirLin, SGRUiDTM, &sDTM, new MapListerDomainType(dmVALUE), true);
		
		FieldGroup* fgOrthoPhoto = new FieldGroup(rbOrthoPhoto, true);
		fgOrthoPhoto->Align(rg, AL_UNDER);
		new FieldDataType(fgOrthoPhoto, SGRUiReferenceMap, &sRefMap, ".mpr.mpl", true);
		new FieldDataType(fgOrthoPhoto, SGRUiDTM, &sDTM, new MapListerDomainType(dmVALUE), true);

		FieldGroup* fgParallProj = new FieldGroup(rbParallProj, true);
		fgParallProj->Align(rg, AL_UNDER);
		new FieldDataType(fgParallProj, SGRUiReferenceMap, &sRefMap, ".mpr.mpl", true);
		new FieldDataType(fgParallProj, SGRUiDTM, &sDTM, new MapListerDomainType(dmVALUE), true);
		
		FieldGroup* fg3D = new FieldGroup(rb3D,true);
		fg3D->Align(rg, AL_UNDER);
		rcSize = RowCol(300L,400L);
		new FieldRowCol(fg3D, SGRUiRowsCols, &rcSize);
		new FieldDataType(fg3D, SGRUiDTM, &sDTM, new MapListerDomainType(dmVALUE), true);
	}
	
	String s('X', 50);
	stRemark = new StaticText(root, s);
	stRemark->SetIndependentPos();
	stRemark->Align(fgCorn, AL_UNDER);
	stRemark->SetVal(String());
	if (fOnlyCorners) 
		SetMenHelpTopic(htpGrfCreateOnlyCorners);
	else
		SetMenHelpTopic(htpGrfCreate);
	
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
		stRemark->SetVal(SGRRemNotValidGrfName);
	else if(File::fExist(fn))   
		stRemark->SetVal(SGRRemGrfExists);
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
					String s(SGRRemLinesCols_ii.scVal(), rcSize.Row, rcSize.Col);
					stRemark->SetVal(s);
					m_fBoundsOK = true;
				}
				else
				{
					stRemark->SetVal(SGRRemMinSmallerMax);
					m_fBoundsOK = false;
				}
			}
			else
				if (rPixSize != rUNDEF && rPixSize <= 0.001)
					stRemark->SetVal(SGRRemPixTooSmall);
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
: FormWithDest(wPar, SGRTitleCreateGrf),
  sGeoRef(sGrf), map(mp), rcSize(mp->rcSize()), fOnlyTiepoints(fOnlyTiep),
  crdMin(cb.cMin), crdMax(cb.cMax), fEditStart(fStartEdit),
  fgr(0), rg(0), stRemark(0), fgCorners(0), cbSubPixel(0)
{
	iImg = IlwWinApp()->iImage(".grf");

  sNewName = *sGeoRef;
  sCoordSys = cs->sName();
  fgr = new FieldDataTypeCreate(root, SGRUiGrfName, &sNewName, ".GRF", false);
	fgr->SetIndependentPos();
  fgr->SetCallBack((NotifyProc)&FormCreateGeoRefRC::CallBack);
  StaticText* st = new StaticText(root, SGRUiDescription);
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
  
  iType = 0;
	fCallBackGrfSubPixelCalled = false;
	fSubPixelPrecise = false;
  rg = new RadioGroup(root, "", &iType);
  rg->SetCallBack((NotifyProc)&FormCreateGeoRefRC::CallBack);
  RadioButton* rbCorners = 0;
  if (!fOnlyTiepoints)
    rbCorners = new RadioButton(rg, SGRUiGrfCorners);
  RadioButton* rbTiePoints = new RadioButton(rg, SGRUiGrfTiepoints);
  RadioButton* rbDirLin = new RadioButton(rg, SGRUiGrfDirLin);
  RadioButton* rbOrthoPhoto = new RadioButton(rg, SGRUiGrfTiepointsPhoto);
	RadioButton* rbParallProj = new RadioButton(rg, SGRUiGrfParallProj);

  if (rbCorners) {
    fCoC = true;
    fgCorners = new FieldGroup(rbCorners, true);
    fgCorners->Align(rg, AL_UNDER);
    new FieldCoordSystemC(fgCorners, SGRUiCoordSys, &sCoordSys);
    FieldCoord* fc1 = new FieldCoord(fgCorners, SGRUiMinXY, &crdMin);
    FieldCoord* fc2 = new FieldCoord(fgCorners, SGRUiMaxXY, &crdMax);
    cbCoC = new CheckBox(fgCorners, SGRUiCenterOfCorners, &fCoC);
    cbCoC->SetIndependentPos();
    fgCorners->SetCallBack((NotifyProc)&FormCreateGeoRefRC::CallBack);
    cbCoC->SetCallBack((NotifyProc)&FormCreateGeoRefRC::CallBack);
    fc1->SetCallBack((NotifyProc)&FormCreateGeoRefRC::CallBack);
    fc2->SetCallBack((NotifyProc)&FormCreateGeoRefRC::CallBack);
  }

  if (rbTiePoints) {
    FieldGroup* fg = new FieldGroup(rbTiePoints, true);
    fg->Align(rg, AL_UNDER);
    new FieldCoordSystemC(fg, SGRUiCoordSys, &sCoordSys);
  }

  if (rbDirLin) {
    FieldGroup* fgDirLin = new FieldGroup(rbDirLin, true);
    fgDirLin->Align(rg, AL_UNDER);
    new FieldDataType(fgDirLin, SGRUiDTM, &sDTM, new MapListerDomainType(dmVALUE), true);
  }
   
	FieldGroup* fgOrthoPhoto = 0;
  if (rbOrthoPhoto) {
    fgOrthoPhoto = new FieldGroup(rbOrthoPhoto, true);
    fgOrthoPhoto->Align(rg, AL_UNDER);
    new FieldDataType(fgOrthoPhoto, SGRUiDTM, &sDTM, new MapListerDomainType(dmVALUE), true);
  }

	if (rbParallProj) {
		FieldGroup* fgParallProj= new FieldGroup(rbParallProj, true);
		fgParallProj->Align(rg, AL_UNDER);
		new FieldDataType(fgParallProj, SGRUiDTM, &sDTM, new MapListerDomainType(dmVALUE), true);
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
    SetMenHelpTopic(htpGrfCreateOnlyTiepoints);
  else  
    SetMenHelpTopic(htpGrfCreateRC);
  create();
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
    stRemark->SetVal(SGRRemNotValidGrfName);
  else if(File::fExist(fn))   
    stRemark->SetVal(SGRRemGrfExists);
  else {
    rg->StoreData();
    if (iType == 0 && !fOnlyTiepoints) {
      fgCorners->StoreData();
      if (crdMin.x < crdMax.x && crdMin.y < crdMax.y) {
        double rX, rY;
        int iCorr = fCoC ? 1 : 0;
        rX = (crdMax.x - crdMin.x) / (rcSize.Col - iCorr);
        rY = (crdMax.y - crdMin.y) / (rcSize.Row - iCorr);
        String s(SGRRemPixSize_ff.scVal(), rX, rY);
        stRemark->SetVal(s);
        fOk = true;
      }  
      else 
        stRemark->SetVal(SGRRemMinSmallerMax);
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
  : FormWithDest(wPar, SGRTitleCreateGrf3D)
  {
    new FieldDataTypeCreate(root, SGRUiGrfName, sGrf, ".GRF", false);
    StaticText* st = new StaticText(root, SGRUiDescription);
    st->psn->SetBound(0,0,0,0);
    FieldString* fs = new FieldString(root, "", sDescr);
    fs->SetWidth(120);
    fs->SetIndependentPos();
    new FieldRowCol(root, SGRUiRowsCols, rcSize);
    new FieldMap(root, SGRUiDTM, sDTM);
    SetMenHelpTopic(htpGrfCreate3D);
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
