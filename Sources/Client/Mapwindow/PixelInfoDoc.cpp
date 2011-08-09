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
// PixelInfoDoc.cpp : implementation file
//

#include "Client\Headers\formelementspch.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\ilwis.h"
#include "Engine\Base\DataObjects\ObjectCollection.h"
#include "Client\Base\datawind.h"
#include "Engine\Drawers\drawer_n.h"
#include "Client\Mapwindow\PixelInfoWindow.h"
#include "Headers\constant.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\PixelInfoDoc.h"
#include "Headers\Hs\PIXINFO.hs"
#include "Client\FormElements\flddat.h"
#include "Client\Mapwindow\RECITSEL.H"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "Client\TableWindow\BaseTablePaneView.h"
#include "Engine\map\Feature.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// PixelInfoDoc

IMPLEMENT_DYNCREATE(PixelInfoDoc, IlwisDocument)

BEGIN_MESSAGE_MAP(PixelInfoDoc, IlwisDocument)
	//{{AFX_MSG_MAP(PixelInfoDoc)
	ON_COMMAND(ID_ADDMAPS, OnAddMaps)
	ON_COMMAND(ID_ADDCSYS, OnAddCsys)
	ON_COMMAND(ID_ADDGRF, OnAddGrf)
	ON_COMMAND(ID_PIXCONFIGURE, OnPixConfigure)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


PixelInfoDoc::PixelInfoDoc()
: riCoord(NULL,crdUNDEF)
, fMouse(true)
, fMouseCont(true)
, fDigitizer(true)
, fDigitizerCont(false),
editFeature(0)
{
	//  riArray.Resize(1,1);
	riArray.Resize(1); // is now zero based

	IlwisSettings settings("pixelinfo");

	fMouseCont = settings.fValue("MouseContineous", false);
	fDigitizerCont = settings.fValue("DigitizerContineous", false);	
	fMouse = settings.fValue("UseMouse", true);		
	fDigitizer = settings.fValue("UseDigitizer", false);
	mdoc = 0;
}

BOOL PixelInfoDoc::OnNewDocument()
{	
	// IlwisDocument does not accept OnNewDocument()
	if (!CDocument::OnNewDocument())
		return FALSE;
	SetTitle(TR("Pixel Information").c_str());
	riCoord.AllowEdit(true);
	Update();
	return TRUE;
}

BOOL PixelInfoDoc::OnOpenDocument(LPCTSTR lpszPathName, MapCompositionDoc *doc, ILWIS::ComplexDrawer *drw)
{
	// IlwisDocument does not accept OnNewDocument()
	if (!OnNewDocument())
		return FALSE;
	ParmList pm(lpszPathName);
	mdoc = doc;
	for (int i=0; i < pm.iFixed(); i++) {
		try {
			FileName fn(pm.sGet(i));
			if (fn.sExt == ".csy") {
				CoordSystem cs(fn);
				AddCoordSystem(cs);
			}
			if (fn.sExt == ".grf") {
				GeoRef grf(fn);
				AddGeoRef(grf);
			}
			else if (fn.sExt == ".mpl") {
				MapList mpl(fn);
				AddMapList(mpl, drw->getType() == "AnimationDrawer" ? RecItem::atANIMATION : RecItem::atNORMAL,drw);
			}
			else if (fn.sExt == ".mpr" ||fn.sExt == ".mpa" ||fn.sExt == ".mpp" ||fn.sExt == ".mps") {
				BaseMap map(fn);
				AddMap(map);
			} else if ( fn.sExt == ".ioc") {
				ObjectCollection oc(fn);
				String type = drw->getType();
				RecItem::AddType atype = RecItem::atNORMAL;
				if ( type == "AnimationDrawer")
					atype = RecItem::atANIMATION;
				if ( type == "CollectionDrawer")
					atype = RecItem::atCOLLECTIONLAYER;
				AddCollection(oc, atype ,drw);
			}
		}
		catch (const ErrorObject& err) {
			err.Show();
		}
	}


	Update();
	return TRUE;
}

PixelInfoDoc::~PixelInfoDoc()
{
	if ( mdoc)
		mdoc->pixInfoDoc = 0; //  remove the reference to this one
}


/////////////////////////////////////////////////////////////////////////////
// PixelInfoDoc diagnostics

#ifdef _DEBUG
void PixelInfoDoc::AssertValid() const
{
	IlwisDocument::AssertValid();
}

void PixelInfoDoc::Dump(CDumpContext& dc) const
{
	IlwisDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// PixelInfoDoc serialization

void PixelInfoDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// PixelInfoDoc commands

zIcon PixelInfoDoc::icon() const
{
	return zIcon("PixInfoIcon");
}

int PixelInfoDoc::iSize() const
{
	return riArray.iSize();
}

String PixelInfoDoc::sName(int iRow) const
{
	if (iRow > 0 && iRow <= iSize()) {
		String sRes = riArray[(int)iRow-1]->sName(); // is zero based
		return sRes;
	}
	else
		return "";
}

FileName PixelInfoDoc::fn(int iRow) const
{
	if (iRow > 0 && iRow <= iSize()) 
		return riArray[iRow-1]->fnObj(); // is zero based
	else
		return FileName();
}

String PixelInfoDoc::sValue(int iRow) const
{
	if (iRow > 0 && iRow <= iSize()) {
		String sRes = riArray[iRow-1]->sValue(); // is zero based
		return sRes;
	}
	else
		return "";
}

void PixelInfoDoc::Update()
{
	riArray.Reset();
	riCoord.AddSelfToArray(riArray);
	UpdateAllViews(0);
}

LRESULT PixelInfoDoc::OnUpdate(WPARAM wParam, LPARAM lParam)
{
	CoordMessage cm = (CoordMessage) wParam;

	CoordWithCoordSystem* c = (CoordWithCoordSystem*)(void*) lParam;
	bool fUpdate = false;
	if ( cm & cmMOUSECLICK ) {
		fUpdate = fMouse;
	}
	if ( cm & cmMOUSEMOVE && !(cm & cmZOOMIN)) {
		fUpdate = fMouse && fMouseCont;
	}
	if ( cm & cmDIGIMOVE) {
		fUpdate = fDigitizer && fDigitizerCont;
	}
	if ( cm & cmINSERT)
		fUpdate = true;

	if (fUpdate) {
		riCoord.SetValue(*c);
		UpdateAllViews(0, (LPARAM)BaseTablePaneView::uhNOBUTTONS);
	}
	return 0;
}

void PixelInfoDoc::setAssociatedDrawerTool(ILWIS::DrawerTool *drw, const String& targetName) {
	riCoord.setAssociatedDrawerTool(drw, targetName);
	if ( drw)
		mdoc = drw->getDocument();

}

void PixelInfoDoc::OnAddMaps()
{
	String s;
	DataObjectForm frm(wndGetActiveView(), TR("Pixel Info - Add Map"), &s, ".mpr.mpa.mps.mpp.mpl", "ilwismen\add_map_to_pixel_information_window.htm");
	if (frm.fOkClicked()) {
		FileName fn = s;
		if (fn.sExt == ".mpl") {
			MapList mpl(fn);
			AddMapList(mpl, RecItem::atNORMAL, 0);
		}
		else {
			BaseMap mp(s);
			AddMap(mp);
		}  
	}
}

void PixelInfoDoc::OnAddCsys()
{
	String s;
	DataObjectForm frm(wndGetActiveView(), TR("Pixel Info - Add Coordinate System"), &s, ".csy", "ilwismen\add_coordinate_system_to_pixel_info_window.htm");
	if (frm.fOkClicked()) {
		CoordSystem cs(s);
		AddCoordSystem(cs);
	}
}

void PixelInfoDoc::OnAddGrf()
{
	String s;
	DataObjectForm frm(wndGetActiveView(), TR("Pixel Info - Add GeoReference"), &s, ".grf", "");
	if (frm.fOkClicked()) {
		GeoRef grf(s);
		AddGeoRef(grf);
	}
}

void PixelInfoDoc::AddMap(const BaseMap& mp)
{
	if (IOTYPE(mp->fnObj) == IlwisObject::iotRASMAP) {
		Map rmap(mp->fnObj);
		riCoord.AddRasterMap(rmap);
	} else
		riCoord.AddMap(mp);
	Update();
}

void PixelInfoDoc::AddMapList(const MapList& mpl, RecItem::AddType tp, ILWIS::ComplexDrawer *drw)
{
	if (mpl.fValid()) {
		if ( tp == RecItem::atNORMAL || drw == 0) {
			for (int i = mpl->iLower(); i <= mpl->iUpper(); ++i)
				riCoord.AddMap(mpl[i]);
		} else if ( tp == RecItem::atANIMATION) {
			riCoord.AddAnimation(mpl, drw);
		}
		Update();
	}
}
void PixelInfoDoc::AddCollection(const ObjectCollection& col, RecItem::AddType tp, ILWIS::ComplexDrawer *drw)
{
	if ( col.fValid()) {
		if ( tp == RecItem::atNORMAL || drw == 0) {
			riCoord.AddCollection(col);
		} else if ( tp == RecItem::atANIMATION ) {
			riCoord.AddAnimation(col, drw);

		}  else if ( tp == RecItem::atCOLLECTIONLAYER ) {
			riCoord.AddCollectionLayer(col, drw);

		}
	Update();
	}
}

void PixelInfoDoc::AddCoordSystem(const CoordSystem& cs)
{
	riCoord.AddCoordSystem(cs);
	Update();
}

void PixelInfoDoc::AddGeoRef(const GeoRef& grf)
{
	riCoord.AddGeoRef(grf);
	Update();
}

class PixelInfoConfigureForm: public FormWithDest
{
public:
	PixelInfoConfigureForm(CWnd* w, PixelInfoDoc* pid)
		: FormWithDest(w, TR("Customize Pixel Info"))
	{
		StaticText* st = new StaticText(root, TR("Receive coordinate data from:"));
		st->SetIndependentPos();
		CheckBox* cbMouse = new CheckBox (root, TR("&Mouse"), &pid->fMouse);
		new CheckBox (cbMouse, TR("&Continuous"), &pid->fMouseCont);
		CheckBox* cbDig = new CheckBox (root, TR("&Digitizer"), &pid->fDigitizer);
		cbDig->Align(cbMouse, AL_UNDER);
		new CheckBox (cbDig, TR("&Continuous"), &pid->fDigitizerCont);
		st = new StaticText(root, TR("Show values of:"));
		st->Align(cbDig, AL_UNDER);
		st->SetIndependentPos();
		RecItemSelector* ris = new RecItemSelector(root, &pid->riCoord);
		ris->SetIndependentPos();
		SetMenHelpTopic("ilwismen\\customize_the_pixel_information_window.htm");
		create();
	}
};

void PixelInfoDoc::OnPixConfigure()
{
	PixelInfoConfigureForm frm(wndGetActiveView(), this);
	if (frm.fOkClicked()) 
		Update();
}

void PixelInfoDoc::OnCloseDocument()
{
	IlwisSettings settings("pixelinfo");

	settings.SetValue("MouseContineous", fMouseCont);
	settings.SetValue("DigitizerContineous", fDigitizerCont);	
	settings.SetValue("UseDigitizer", fDigitizer);		
	settings.SetValue("UseMouse", fMouse);		

	IlwisDocument::OnCloseDocument();
}

bool PixelInfoDoc::fRowEditable(int rowIndex) const {
	if (rowIndex > 0 && rowIndex <= iSize()) {
		return riArray[rowIndex - 1]->fAllowEdit();
	}
	return false;
}

RecItem * PixelInfoDoc::getItem(int rowIndex) {
	if (rowIndex > 0 && rowIndex <= iSize()) {
		return riArray[rowIndex - 1];
	}
	return 0;
}

void PixelInfoDoc::setEditFeature(Feature *f) {
	editFeature = f;
}

Feature *PixelInfoDoc::getEditFeature() {
	return editFeature;
}