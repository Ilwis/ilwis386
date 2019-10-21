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
// MapCompositionDoc.cpp : implementation file
//

#pragma warning( disable : 4786 )
#pragma warning( disable : 4503 )

#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\FldOneSelectTextOnly.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Engine\Base\System\Engine.h"
#include "Client\Base\datawind.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\MainWindow\Catalog\CatalogDocument.h"
#include "Client\Mapwindow\MapWindow.h"
#include "Engine\Map\Mapview.h"
#include "Engine\Base\DataObjects\URL.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Client\Mapwindow\PixelInfoDoc.h"
#include "Engine\Base\DataObjects\ObjectCollection.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\FormElements\syscolor.h"
#include "Engine\Map\Segment\Seg.h"
#include "Engine\Map\Polygon\POL.H"
#include "Engine\Map\Point\PNT.H"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\Mapwindow\Drawers\DrawerTool.h"
#include "Client\Mapwindow\Positioner.h"
#include "Client\FormElements\fldcol.h"
#include "Client\FormElements\fldrpr.h"
#include "Client\FormElements\fldcolor.h"
#include "Client\Editors\Utils\SYMBOL.H"
#include "Client\Editors\Utils\Smbext.h"
#include "Client\Editors\Utils\line.h"
#include "Client\Editors\Utils\Pattern.h"
#include "Client\Editors\Utils\MULTICOL.H"
#include "Client\Mapwindow\ZoomableView.h"
#include "Engine\Drawers\DrawerContext.h"
#include "Client\Mapwindow\PixelInfoDoc.h"
#include "Engine\Stereoscopy\StereoPair.h"
#include "Headers\constant.h"
#include "Headers\Hs\Mapwind.hs"
#include "Headers\Hs\Coordsys.hs"
#include "Headers\Hs\Appforms.hs"
#include "Headers\Hs\Mainwind.hs"
#include "Headers\Hs\Editor.hs"
#include "Client\FormElements\flddat.h"
#include "Client\Mapwindow\MapPaneView.h"
//#include "AnnotationDrawer.h"
//#include "Client\Mapwindow\Drawers\NorthDrawer.h"
#include "Engine\SpatialReference\GRCTP.H"
#include "Engine\SpatialReference\csctp.h"
#include "Engine\Map\txtann.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Client\Editors\Utils\GeneralBar.h"
#include "Client\TableWindow\HistogramGraphView.h"
#include "Client\TableWindow\HistogramRGBGraphView.h"
#include "Client\TableWindow\HistogramGraphDoc.h"
#include "Engine\Base\File\ElementMap.h"
#include "Client\Mapwindow\MapCompositionSrvItem.h"
#include "Engine\DataExchange\WMSCollection.h"
#include "Client\Mapwindow\ScaleBarSrvItem.h"
#include "Client\Mapwindow\MapStatusBar.h"
#include "Engine\SampleSet\SAMPLSET.H"
#include "Client\Editors\Editor.h"
#include "Engine\Drawers\ZValueMaker.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// {8A842674-E359-11D2-B73E-00A0C9D5342F}
const CLSID MapCompositionDoc::clsid =
{ 0x8A842674, 0xE359, 0x11D2, { 0xB7, 0x3E, 0x00, 0xA0, 0xC9, 0xD5, 0x34, 0x2F } };

/////////////////////////////////////////////////////////////////////////////
// MapCompositionDoc

using namespace ILWIS;

IMPLEMENT_DYNCREATE(MapCompositionDoc, CatalogDocument)

BEGIN_MESSAGE_MAP(MapCompositionDoc, CatalogDocument)
	//{{AFX_MSG_MAP(MapCompositionDoc)
	ON_COMMAND(ID_EXTPERC, OnExtPerc)
	ON_COMMAND(ID_EXTCOORD, OnExtCoord)
	ON_UPDATE_COMMAND_UI(ID_EXTCOORD, OnUpdateExtCoord)
	ON_COMMAND(ID_ADDLAYER, OnAddLayer)
	ON_COMMAND(ID_ADDRASMAP, OnAddRasMap)
	ON_COMMAND(ID_ADDSEGMAP, OnAddSegMap)
	ON_COMMAND(ID_ADDPOLMAP, OnAddPolMap)
	ON_COMMAND(ID_ADDPOINTS, OnAddPntMap)

	ON_COMMAND(ID_SAVEVIEW, OnSaveView)
	ON_COMMAND(ID_SAVEVIEWAS, OnSaveViewAs)
	ON_COMMAND(ID_CREATE_LAYOUT, OnCreateLayout)
	ON_COMMAND(ID_FILE_OPEN, OnOpen)
	ON_COMMAND(ID_COPYSCALEBARLINK, OnCopyScaleBarLink)
	ON_COMMAND(ID_OPENPIXELINFO, OnOpenPixelInfo)
	//ON_COMMAND(ID_SHOWHISTOGRAM, OnShowHistogram)
	//ON_UPDATE_COMMAND_UI(ID_SHOWHISTOGRAM, OnUpdateShowHistogram)

	//ON_COMMAND_RANGE(ID_LAYFIRST, ID_LAYFIRST+900, OnDataLayer)
	ON_COMMAND_RANGE(ID_DOMLAYER, ID_DOMLAYER+900, OnDomainLayer)
	ON_COMMAND_RANGE(ID_RPRLAYER, ID_RPRLAYER+900, OnRprLayer)
	ON_COMMAND_RANGE(ID_EDITLAYER, ID_EDITLAYER+900, OnEditLayer)
	ON_COMMAND_RANGE(ID_PROPLAYER, ID_PROPLAYER+900, OnPropLayer)
	ON_COMMAND_RANGE(ID_HISTOLAYER, ID_HISTOLAYER+900, OnHistoLayer)
	ON_UPDATE_COMMAND_UI_RANGE(ID_LAYFIRST, ID_LAYFIRST+900, OnUpdateDataLayer)
	ON_UPDATE_COMMAND_UI_RANGE(ID_DOMLAYER, ID_DOMLAYER+900, OnUpdateDomainLayer)
	ON_UPDATE_COMMAND_UI_RANGE(ID_RPRLAYER, ID_RPRLAYER+900, OnUpdateRprLayer)
	ON_UPDATE_COMMAND_UI_RANGE(ID_EDITLAYER, ID_EDITLAYER+900, OnUpdateEditLayer)
	ON_UPDATE_COMMAND_UI_RANGE(ID_PROPLAYER, ID_PROPLAYER+900, OnUpdatePropLayer)
	ON_UPDATE_COMMAND_UI_RANGE(ID_HISTOLAYER, ID_HISTOLAYER+900, OnUpdateHistoLayer)
END_MESSAGE_MAP()


MapCompositionDoc::MapCompositionDoc()
: 
 m_fAbortSelectionThread(false)
, m_selectionThread(0)
{
	iListState = 0;
	fInCmdMsg = false;
	rDfltScale = rUNDEF;
	rootDrawer = new RootDrawer();
	fnView = FileName("mapview.mpv");
	selectedDrawer = 0;
	pixInfoDoc = 0;
	state = 0;
}

MapCompositionDoc::~MapCompositionDoc()
{
	if (m_selectionThread) {
		m_fAbortSelectionThread = true;
		m_selectionThread->ResumeThread();
		csSelectionThread.Lock(); // wait here til thread exits
		csSelectionThread.Unlock();
	}

	delete rootDrawer;
	if ( state & IlwisWinApp::osExitOnClose)
		AfxGetApp()->PostThreadMessage(WM_QUIT,0,0);
}

void MapCompositionDoc::DeleteContents()
{
	for(HistIter cur = gbHist.begin(); cur != gbHist.end(); ++cur)
		delete (*cur).second;

}

BOOL MapCompositionDoc::OnNewDocument()
{
	DeleteContents();
	if (!CatalogDocument::OnNewDocument())
		return FALSE;
	RegisterIfServerAttached(NULL, TRUE);
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// MapCompositionDoc serialization

void MapCompositionDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		if (!mpv.fValid())
		{
			ElementContainer en;
			mpv = MapView(en);
		}
		ElementContainer& en = const_cast<ElementContainer&>(mpv->fnObj);
		if (en.em != 0)
			delete en.em;  // clear the ODF info
		en.em = new ElementMap;

		StoreView();  // (re)write the ODF info
		en.em->Serialize(ar);
	}
	else
	{
		ElementContainer en;
		en.em = new ElementMap;
		en.em->Serialize(ar);
		try
		{
			FileName fn(ar.m_strFileName);
			mpv = MapView(fn);
			OnOpenMapView(mpv);
		}
		catch (CException* err) 
		{
			MessageBeep(MB_ICONHAND);
			err->ReportError(MB_OK|MB_ICONHAND|MB_TOPMOST);
			err->Delete();
		}
		en.em = 0;
	}
}

/////////////////////////////////////////////////////////////////////////////
// MapCompositionDoc commands

IlwisObject MapCompositionDoc::obj() const
{
	return mpv;
}

zIcon MapCompositionDoc::icon() const
{
	if (rootDrawer->getDrawerCount() != 1)
		return zIcon("MapViewIcon");
	else {
		if ( rootDrawer->getDrawerCount() > 0) {
			String name = rootDrawer->getDrawer(0)->iconName();
			int iImg = IlwWinApp()->iImage(name.c_str());
			HICON hIco = IlwWinApp()->ilLarge.ExtractIcon(iImg);
			return zIcon(hIco);
		}
	}
	return zIcon();
}

bool MapCompositionDoc::usesObject(const IlwisObject& ob) const {
	vector<NewDrawer *> drawers;
	rootDrawer->getDrawers(drawers);
	for(int i = 0; i < drawers.size(); ++i) {
		SpatialDataDrawer *drw = dynamic_cast<SpatialDataDrawer *>(drawers.at(i));
		if ( drw) {
			if ( drw->getBaseMap()->fnObj == ob->fnObj)
				return true;
		}
		
	}
	return false;
}

NewDrawer *MapCompositionDoc::getDrawerFor(const IlwisObject& ob, const Coord& crd) const{
	vector<NewDrawer *> drawers;
	rootDrawer->getDrawers(drawers);
	for(int i = 0; i < drawers.size(); ++i) {
		SpatialDataDrawer *drw = dynamic_cast<SpatialDataDrawer *>(drawers.at(i));
		if ( drw) {
			if ( drw->getBaseMap()->fnObj == ob->fnObj) {
				if ( crd.fUndef())
					return drw;
				else{
					if ( drw->inEditMode()) {
						BaseMapPtr *ptr = drw->getBaseMap();
						vector<Geometry *> geoms = ptr->getFeatures(crd);
						Feature *feature = CFEATURE(geoms[0]);
						vector<NewDrawer *> drawers;
						drw->getDrawerFor(feature, drawers);
						if ( drawers.size() > 0)
							return drawers[0];
					}
				}
			}
		}
		
	}
	return 0;
}

BOOL MapCompositionDoc::OnOpenDocument(LPCTSTR lpszPathName, int os) 
{
	return OnOpenDocument(lpszPathName, otNORMAL, os);
}

BOOL MapCompositionDoc::OnOpenDocument(LPCTSTR lpszPath, ParmList& pm, int os) 
{
	try {
		FileName fn(pm.sGet("output") != "" ? pm.sGet("output") : lpszPath);
		String sC = pm.sCmd();
		state = IlwisWinApp::osNormal;
		if ( fn.fExist() == false && pm.fExist("collection") ) // implicit object
		{
			if ( pm.sGet("method") == "") {
				ForeignCollection fc(pm.sGet("collection"));
				pm.Add(new Parm("method", fc->sGetMethod()));
			}
			ForeignCollection::CreateImplicitObject(fn, pm);
		}
		else if ( fn.fExist() && pm.fExist("exitOnClose")) {
			state |= IlwisWinApp::osExitOnClose;
		}
		if ( IlwisObject::iotObjectType(fn) > IlwisObject::iotRASMAP && IlwisObject::iotObjectType(fn) < IlwisObject::iotTABLE) 
		{
			int ot = pm.sGet("mode").iVal();
			if ( ot == iUNDEF) ot = 0;
			if ( IlwisObject::iotObjectType(fn) == IlwisObject::iotPOINTMAP)
				return OnOpenPointMap(PointMap(fn), (IlwisDocument::OpenType)ot, state);
			if ( IlwisObject::iotObjectType(fn) == IlwisObject::iotSEGMENTMAP)
				return OnOpenSegmentMap(SegmentMap(fn), (IlwisDocument::OpenType)ot, state);
			if ( IlwisObject::iotObjectType(fn) == IlwisObject::iotPOLYGONMAP)
				return OnOpenPolygonMap(PolygonMap(fn), (IlwisDocument::OpenType)ot, state);
		}
		if (!IlwisDocument::OnOpenDocument(fn.sRelative().c_str()))
			return FALSE;
	} catch (std::exception& err) {
		const char *txt = err.what();
		String mes("%s, probably invalid or corrupt data", txt);
		ErrorObject errObj(mes);
		errObj.Show();
	} catch (ErrorObject& err) {
		err.Show();
	}
	return OnOpenDocument(lpszPath, state);
}

BOOL MapCompositionDoc::OnOpenDocument(LPCTSTR lpszPathName, OpenType ot, int os)
{
	try {
		DeleteContents();
		//	RegisterIfServerAttached(NULL, TRUE);

		FileName fn(lpszPathName);
		if (".mpv" == fn.sExt || lpszPathName == 0) 
			fUseSerialize = true;
		if (0 != lpszPathName)
			SetPathName(fn.sFullName().c_str());
		if (!CatalogDocument::OnOpenDocument(lpszPathName))
			return FALSE;
		if (fUseSerialize)
			return TRUE;

		// check type on basis of extension
		// better would be based on file contents
		if (".mpr" == fn.sExt || (".mpl" == fn.sExt && fn.sSectionPostFix != "")) {
			Map mp(fn);
			if (!mp.fValid())
				return FALSE;
			return OnOpenRasterMap(mp,ot, os);
		}
		else if (".grf" == fn.sExt) {
			GeoRef grf(fn);
			if (!grf.fValid())
				return FALSE;
			if (grf->fReadOnly()) {
				String sErr(TR("GeoReference %S is read-only and thus cannot be edited").c_str(), grf->sName());
				MessageBox(0, sErr.c_str(), TR("Error").c_str(), MB_OK|MB_ICONSTOP|MB_TOPMOST); 
				return FALSE;
			}
			GeoRefCTP* gc = grf->pgCTP();
			if (0 == gc)
				return FALSE;
			FileName fn = gc->fnBackgroundMap;
			if (!fn.fValid()) {
				MessageBox(0, TR("No Background Map specified, editing not possible").c_str(), TR("Error").c_str(), MB_OK|MB_ICONSTOP|MB_TOPMOST); 
				return FALSE;
			}
			if (".mpl" == fn.sExt) {
				MapList ml(fn);
				if (!ml.fValid())
					return FALSE;
				if (ml->gr() != grf) {
					MessageBox(0, TR("Incorrect Background Map, editing not possible").c_str(), TR("Error").c_str(), MB_OK|MB_ICONSTOP|MB_TOPMOST); 
					return FALSE;
				}
				if (!OnOpenMapList(ml,otNORMAL))
					return FALSE;
			}
			else if (".mpr" == fn.sExt) {
				Map mp(fn);
				if (!mp.fValid())
					return FALSE;
				if (mp->gr() != grf) {
					MessageBox(0, TR("Incorrect Background Map, editing not possible").c_str(), TR("Error").c_str(), MB_OK|MB_ICONSTOP|MB_TOPMOST); 
					return FALSE;
				}
				if (!OnOpenRasterMap(mp,otNORMAL))
					return FALSE;
			}
			else
				return FALSE;
			::AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_EDITGRF, 0);
			return TRUE;
		}
		else if (".mpl" == fn.sExt) {
			MapList ml(fn);
			if (!ml.fValid())
				return FALSE;
			return OnOpenMapList(ml, ot, os);
		}
		else if (".ioc" == fn.sExt) {
			ObjectCollection oc(fn);
			if (!oc.fValid())
				return FALSE;
			return OnOpenObjectCollection(oc, ot, "ilwis38", os);
		}
		else if (".mps" == fn.sExt) {
			SegmentMap mp(fn);
			if (!mp.fValid())
				return FALSE;
			return OnOpenSegmentMap(mp, ot, os);
		}
		else if (".csy" == fn.sExt) {
			CoordSystem cs(fn);
			if (!cs.fValid())
				return FALSE;
			CoordSystemCTP* cstp = cs->pcsCTP();
			if (0 == cstp)
				return FALSE;
			if (!cstp->fnBackgroundMap.fValid())
				return FALSE;
			String s = cstp->fnBackgroundMap.sFullNameQuoted();
			if (!OnOpenDocument(s.c_str()))
				return FALSE;
			::AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_EDITCSY, 0);
			return TRUE;
		}
		else if (".mpa" == fn.sExt) {
			PolygonMap mp(fn);
			if (!mp.fValid())
				return FALSE;
			return OnOpenPolygonMap(mp, ot, os);
		}
		else if (".mpp" == fn.sExt) {
			PointMap mp(fn);
			if (!mp.fValid())
				return FALSE;
			return OnOpenPointMap(mp, ot, os);
		}
		else if (".mpv" == fn.sExt) {
			MapView mpv(fn);
			if (!mpv.fValid())
				return FALSE;
			return OnOpenMapView(mpv, os);
		}
		else if (".sms" == fn.sExt) {
			SampleSet sms(fn);
			if (!sms.fValid())
				return FALSE;
			MapList mpl = sms->mpl();
			if (!mpl.fValid())
				return FALSE;
			NewDrawer *drw;
			if (!( drw = drAppend(mpl,otCOLORCOMP)) != 0)
				return FALSE;
			/*ILWIS::DrawerTool *dt = DrawerTool::createTool("SampleSetEditor",mpvGetView(),ltvGetView(),drw);
			ltvGetView()->getRootTool()->addTool(dt);*/
			return TRUE;
		}
		else if (".stp" == fn.sExt) {
			StereoPair stp(fn);
			if (!stp.fValid())
				return FALSE;
			return OnOpenStereoPair(stp, ot);
		}
		//else if
	}
	catch (std::exception& err) {
		const char *txt = err.what();
		String mes("%s, probably invalid or corrupt data", txt);
		ErrorObject errObj(mes);
		errObj.Show();
	}
	catch (ErrorObject& err) {
		err.Show();
	}
	return FALSE;
}

void MapCompositionDoc::OnCreateLayout()
{
	//Printing printer;
	//printer.print(this);
}

void MapCompositionDoc::OnSaveView()
{
	fUseSerialize = true;
	if (!mpv.fValid())
		OnSaveViewAs();
	else {
		OnFileSave();
	}
}

void MapCompositionDoc::OnSaveViewAs()
{
	fUseSerialize = true;
	class SaveViewForm: public FormWithDest 
	{
	public:
		SaveViewForm(CWnd* parent, String* sName, String* sTitle)
			: FormWithDest(parent, TR("Save View As"))
		{
			iImg = IlwWinApp()->iImage(".mpv");

			new FieldBlank(root);
			new FieldViewCreate(root, TR("&Map View Name"), sName);
			FieldString *fs = new FieldString(root, TR("&Title"), sTitle);
			fs->SetWidth(120);
			//      setHelpItem("ilwismen\\save_view_as.htm"Forms);
			SetMenHelpTopic("ilwismen\\save_view_as.htm");
			create();
		}
	};
	String sViewName, sTitle;
	if (mpv.fValid()) {
		sViewName = mpv->sName();
		sTitle = mpv->sDescription;
	}
	SaveViewForm frm(wndGetActiveView(), &sViewName, &sTitle);
	if (frm.fOkClicked()) {
		FileName fn(sViewName, ".mpv", true);
		if (fn.fExist()) {
			String sErr(TR("File %S already exists.\nOverwrite?").c_str(), fn.sFullPath(true));
			if (IDYES != MessageBox(0, sErr.c_str(), TR("File already exists").c_str(), MB_YESNO|MB_ICONEXCLAMATION))
				return;
		}
		mpv = MapView(sViewName, true);
		mpv->sDescription = sTitle;
		sViewName = mpv->fnObj.sFullName();
		DoSave(sViewName.c_str());
		SetTitle(mpv);
	}
}

void MapCompositionDoc::StoreView()
{
	ObjectDependency objdep;
	mpv->Store();
	rootDrawer->store(mpv->fnObj,"Root");
	if (rootDrawer->getGeoReference().fValid())
		objdep.Add(rootDrawer->getGeoReference());
	if (rootDrawer->getCoordinateSystem().fValid())
		objdep.Add(rootDrawer->getCoordinateSystem());
	vector<NewDrawer *> allDrawers;
	rootDrawer->getDrawers(allDrawers);
	for(int i = 0; i < allDrawers.size(); ++i) {
		SpatialDataDrawer *dr = dynamic_cast<SpatialDataDrawer *>(allDrawers.at(i));
		if ( dr) {
			objdep.Add(dr->getBaseMap());
		}
	}
	objdep.Store(mpv.ptr());
}

void MapCompositionDoc::SetTitle(const IlwisObject& obj)
{
	//fnObj = obj->fnObj;
	String s = obj->sName();
	if ("" == s)
		return;
	if (obj->sDescription.length()) {
		s &= ": ";
		s &= obj->sDescription;
	}
	CDocument::SetTitle(s.sVal());
}

void MapCompositionDoc::OnExtPerc()
{
	class ExtendForm: public FormWithDest
	{
	public:
		ExtendForm(CWnd* wParent,
			double* rTop, double* rBottom, double* rLeft, double* rRight, MapCompositionDoc *_mdoc)
			: FormWithDest(wParent, TR("Extend Window by Percentage")), mdoc(_mdoc)
		{
			values.push_back(TR("User defined"));
			values.push_back(TR("Center"));
			values.push_back(TR("Right top"));
			values.push_back(TR("Left top"));
			values.push_back(TR("Right bottom"));
			values.push_back(TR("Left bottom"));
			choice = 0;
			ValueRange vrr(-99, 1000, 1);
			frTop = new FieldReal(root, TR("Percentage at &Top"), rTop, vrr);
			frBot = new FieldReal(root, TR("Percentage at &Bottom"), rBottom, vrr);
			frLeft = new FieldReal(root, TR("Percentage at &Left"), rLeft  , vrr);
			frRight = new FieldReal(root, TR("Percentage at &Right"), rRight , vrr);
			ftemplate = new FieldOneSelectString(root,TR("Templates"),&choice,values);
			ftemplate->SetCallBack((NotifyProc)&ExtendForm::setTemplate,this);
			SetMenHelpTopic("ilwismen\\extend_window_by_percentage.htm");
			create();
		}

		int ExtendForm::setTemplate(Event *ev) {
			ftemplate->StoreData();
			if ( choice > 0) {
				CoordBounds cb = mdoc->rootDrawer->getMapCoordBounds();
				bool portrait = cb.width() / cb.height() > 1;
				frRight->SetVal(0);
				frTop->SetVal(0);
				frLeft->SetVal(0);
				frBot->SetVal(0);
				int shiftMax = 40;
				int shiftMin = 20;
				if (choice == 1) {
					frLeft->SetVal(portrait ? shiftMin : shiftMax);
					frBot->SetVal(portrait ? shiftMax : shiftMin);
					frRight->SetVal(portrait ? shiftMax : shiftMin);
					frTop->SetVal(portrait ? shiftMin : shiftMax);
				}
				if (choice == 2) {
					frLeft->SetVal(portrait ? shiftMin : shiftMax);
					frBot->SetVal(portrait ? shiftMax : shiftMin);
	
				}
				if (choice == 3) {
					frBot->SetVal(portrait ? shiftMax : shiftMin);
					frLeft->SetVal(portrait ? shiftMin : shiftMax);
				}
				if (choice == 4) {
					frTop->SetVal(portrait ? shiftMax : shiftMin);
					frLeft->SetVal(portrait ? shiftMin : shiftMax);
				}
				if (choice == 5) {
					frTop->SetVal(portrait ? shiftMax : shiftMin);
					frRight->SetVal(portrait ? shiftMin : shiftMax);
				}
			}
			return 1;
		}
		FormEntry* feDefaultFocus()
		{ 
			if (ftemplate)
				ftemplate->SetVal(0);

			return frTop;
		}
	private:  
		FieldReal* frTop, *frBot, *frLeft, *frRight;
		FieldOneSelectString *ftemplate;
		long choice;
		vector<String> values;
		MapCompositionDoc *mdoc;
	};
	double rTop, rBottom, rLeft, rRight;
	rTop = rLeft = rBottom = rRight = 0;
	ExtendForm frm(0,&rTop,&rBottom,&rLeft,&rRight,this);
	if (frm.fOkClicked()) 
	{
		CoordBounds cb = rootDrawer->getMapCoordBounds();
		double ty,by;
		bool isNorthOriented = cb.MinY() > cb.MaxY();
		double dlx = cb.width() * rLeft / 100.0;
		double drx = cb.width() * rRight / 100.0;
		double dty, dby; 
		double lx = cb.MinX() - dlx;
		double rx = cb.MaxX() + drx;
		if ( isNorthOriented) {
			dby = cb.height() * rBottom / 100.0;
			dty = cb.height() * rTop / 100.0;
			ty = cb.MinY() - dty;
			by = cb.MaxY() + dby;
		} else {
			dby = cb.height() * rTop / 100.0;
			dty = cb.height() * rBottom / 100.0;
			ty = cb.MinY() - dty;
			by = cb.MaxY() + dby;
		}
		cb = CoordBounds(Coord(lx,ty), Coord(rx,by));
		CoordBounds cbZoom = rootDrawer->getCoordBoundsZoom(); // backup zoom
		rootDrawer->setCoordBoundsMap(cb); // set new bounds
		rootDrawer->setCoordBoundsView(cb, true); // set new view
		rootDrawer->setCoordBoundsZoom(cbZoom); // restore zoom
		ComplexDrawer *annotations = (ComplexDrawer *)(rootDrawer->getDrawer("AnnotationDrawers"));
		if (annotations) {
			PreparationParameters pp(NewDrawer::ptRENDER | NewDrawer::ptGEOMETRY);
			NewDrawer *borderDrw = annotations->getDrawer("AnnotationBorderDrawer");
			if ( borderDrw)
				borderDrw->prepare(&pp);
			NewDrawer *gridDrw = annotations->getDrawer("GridDrawer");
			if ( gridDrw)
				gridDrw->prepare(&pp);
			NewDrawer *graticuleDrw = annotations->getDrawer("GraticuleDrawer");
			if ( graticuleDrw)
				graticuleDrw->prepare(&pp);
		}
		MapPaneView * mpv = mpvGetView();
		if (mpv) {
			mpv->setScrollBars();
			mpv->Invalidate();
		}
	}
}

void MapCompositionDoc::OnExtCoord()
{
	class BoundsForm: public FormWithDest
	{
	public:
		BoundsForm(CWnd* parent, Coord* cMin, Coord* cMax)
			: FormWithDest(parent, TR("Extend Window by Coordinates"))
		{
			fcMin = new FieldCoord(root, TR("Minimum X, Y"), cMin);
			new FieldCoord(root, TR("Maximum X, Y"), cMax);
			SetMenHelpTopic("");
			create();
		}
		FormEntry* feDefaultFocus()
		{ 
			return fcMin;
		}
	private:  
		FieldCoord* fcMin;
	};
	CoordBounds cb = rootDrawer->getMapCoordBounds();
	BoundsForm frm(0,&cb.cMin,&cb.cMax);
	if (frm.fOkClicked()) 
	{
		CoordBounds cbZoom = rootDrawer->getCoordBoundsZoom(); // backup zoom
		rootDrawer->setCoordBoundsMap(cb); // set new bounds
		rootDrawer->setCoordBoundsView(cb, true); // set new view
		rootDrawer->setCoordBoundsZoom(cbZoom); // restore zoom
		ComplexDrawer *annotations = (ComplexDrawer *)(rootDrawer->getDrawer("AnnotationDrawers"));
		if (annotations) {
			PreparationParameters pp(NewDrawer::ptRENDER | NewDrawer::ptGEOMETRY);
			NewDrawer *borderDrw = annotations->getDrawer("AnnotationBorderDrawer");
			if ( borderDrw)
				borderDrw->prepare(&pp);
			NewDrawer *gridDrw = annotations->getDrawer("GridDrawer");
			if ( gridDrw)
				gridDrw->prepare(&pp);
			NewDrawer *graticuleDrw = annotations->getDrawer("GraticuleDrawer");
			if ( graticuleDrw)
				graticuleDrw->prepare(&pp);
		}
		MapPaneView * mpv = mpvGetView();
		if (mpv) {
			mpv->setScrollBars();
			mpv->Invalidate();
		}
	}
}

void MapCompositionDoc::OnUpdateExtCoord(CCmdUI* pCmdUI)
{
	//BOOL fEnable = georef->fNorthOriented();
	//pCmdUI->Enable(fEnable);
}


void MapCompositionDoc::menLayers(CMenu& men, int iBaseId)
{
	BOOL fOk = true;
	int id;
	for (id = iBaseId; fOk; ++id)
		fOk = men.DeleteMenu(id, MF_BYCOMMAND);

	CClientDC cdc(wndGetActiveView());

	//list<Drawer*>::iterator iter = dl.begin();
	//for (i = 1, id = iBaseId; 
	//	iter != dl.end(); 
	//	++iter, ++i, ++id)
	for(int i =0,id = iBaseId; i < rootDrawer->getDrawerCount(); ++i,++id)
	{  
		char s[512];
		SpatialDataDrawer *mapdrawer = dynamic_cast<SpatialDataDrawer *>(rootDrawer->getDrawer(i));
		if (!mapdrawer)
			continue;
		String str = mapdrawer->getBaseMap()->fnObj.sFullName();
		FileName fn = str;
		if (".mpr" == fn.sExt)
			str = String("map %S", fn.sFile);
		else if (".mps" == fn.sExt)
			str = String("seg %S", fn.sFile);
		else if (".mpa" == fn.sExt)
			str = String("pol %S", fn.sFile); 
		else if (".mpp" == fn.sExt)
			str = String("pnt %S", fn.sFile);
		else if (".atx" == fn.sExt)
			str = String("txt %S", fn.sFile);
		else
			str = fn.sFile;
		sprintf(s, "&%i %s", i+1, str.c_str());
		men.AppendMenu(MF_STRING, id, s);
		UINT iFlag;
		switch (iBaseId) {
	  //case ID_LAYFIRST:
		 // iFlag = mapdrawer->isActive() 
			//  ? MF_CHECKED : MF_UNCHECKED;
		 // men.CheckMenuItem(id, MF_BYCOMMAND | iFlag);
		 // break;
	  //case ID_ATTLAYER:  
		 // iFlag = !mapdrawer->isActive() || !mapdrawer->getAtttributeTable().fValid()
			//  ? MF_GRAYED : MF_ENABLED;
		 // men.EnableMenuItem(id, MF_BYCOMMAND | iFlag);
		 // break;
	  //case ID_RPRLAYER:  
		 // iFlag = !mapdrawer->isActive() || ! mapdrawer->getBaseMap()->dm()->rpr().fValid()
			//  ? MF_GRAYED : MF_ENABLED;
		 // men.EnableMenuItem(id, MF_BYCOMMAND | iFlag);
		 // break;
	  case ID_EDITLAYER:  
		  iFlag = !mapdrawer->isEditable()
			  ? MF_GRAYED : MF_ENABLED;
		  men.EnableMenuItem(id, MF_BYCOMMAND | iFlag);
		  break;
	  case ID_PROPLAYER:  
		  iFlag = MF_ENABLED;
		  men.EnableMenuItem(id, MF_BYCOMMAND | iFlag);
		  break;
	 // case ID_DOMLAYER:
		//  {  
		//	  BaseMap bm;
		//	  bm.SetPointer(mapdrawer->getBaseMap());
		//	  iFlag = !fDomainEditable(bm) ? MF_GRAYED : MF_ENABLED;
		//	  men.EnableMenuItem(id, MF_BYCOMMAND | iFlag);
		//  }
		//  break;
		}    
	}
}

bool MapCompositionDoc::fDomainEditable( const BaseMap& bmap) {
	if (bmap->dm()->fDataReadOnly())
		return false;
	if (0 != bmap->dm()->pdsrt())
		return true;
	if (0 != bmap->dm()->pdvi())
		return true;
	if (0 != bmap->dm()->pdvr())
		return true;
	return false;  
}

//void MapCompositionDoc::OnDataLayer(UINT nID)
//{
//	list<Drawer*>::iterator iter = dl.begin();
//	int i, id;
//	for (i = 1, id = ID_LAYFIRST;
//		iter != dl.end(); 
//		++iter, ++i, ++id)
//	{  
//		if (id == nID) {
//			if ((*iter)->Configure()) {
//				ChangeState();
//				UpdateAllViews(0,2);
//			}
//		}
//	}
//}

void MapCompositionDoc::OnUpdateDataLayer(CCmdUI* pCmdUI)	
{
	MapPaneView* mpv = mpvGetView();
	bool fEnable = true;
	if (0 != mpv) 
	{
		Editor* edit = mpv->editGet();
		if (0 != edit)
		{
			int i, id;
			for (i = 0, id = ID_LAYFIRST;
				i < rootDrawer->getDrawerCount(); 
				++i, ++id)
			{  
				if (id == pCmdUI->m_nID) {
					NewDrawer *drw = rootDrawer->getDrawer(i);
					//fEnable = edit->dr() != dr;
					break;
				}
			}
		}
	}
	pCmdUI->Enable(fEnable);
}

void MapCompositionDoc::OnEditLayer(UINT nID)	
{
	for(int i =0,id = ID_EDITLAYER; i < rootDrawer->getDrawerCount(); ++i,++id)
	{  
		if (id == nID) {
			SpatialDataDrawer *mapdrawer = dynamic_cast<SpatialDataDrawer *>(rootDrawer->getDrawer(i));
			if (mapdrawer->isEditable()) {
				MapPaneView* mpv = mpvGetView();
				if (0 != mpv)
					mpv->EditNamedLayer(mapdrawer->getBaseMap()->fnObj);
			}
			return;
		}
	}
}

void MapCompositionDoc::OnUpdateEditLayer(CCmdUI* pCmdUI)	
{
	for(int i =0,id = ID_EDITLAYER; i < rootDrawer->getDrawerCount(); ++i,++id)
	{  
		if (id == pCmdUI->m_nID) {
			NewDrawer *drw = rootDrawer->getDrawer(i);
			bool fEnable = drw->isEditable();
			MapPaneView* mpv = mpvGetView();
			if (0 == mpv)
				fEnable = false;
			pCmdUI->Enable(fEnable);
		}
	}
}

void MapCompositionDoc::OnDomainLayer(UINT nID)
{
	//list<Drawer*>::iterator iter = dl.begin();
	//int i, id;
	//for (i = 1, id = ID_DOMLAYER;
	//	iter != dl.end(); 
	//	++iter, ++i, ++id)
	//{  
	//	if (id == nID) 
	//		(*iter)->EditDomain();
	//}
}

void MapCompositionDoc::OnUpdateDomainLayer(CCmdUI* pCmdUI)
{
	//list<Drawer*>::iterator iter = dl.begin();
	//int i, id;
	//for (i = 1, id = ID_DOMLAYER;
	//	iter != dl.end(); 
	//	++iter, ++i, ++id)
	//{  
	//	if (id == pCmdUI->m_nID) {
	//		bool fEnable = (*iter)->fEditDomain();
	//		pCmdUI->Enable(fEnable);
	//	}
	//}
}

void MapCompositionDoc::OnRprLayer(UINT nID)
{
	//list<Drawer*>::iterator iter = dl.begin();
	//int i, id;
	//for (i = 1, id = ID_RPRLAYER;
	//	iter != dl.end(); 
	//	++iter, ++i, ++id)
	//{  
	//	if (id == nID) 
	//		(*iter)->EditRepresentation();
	//}
}

void MapCompositionDoc::OnUpdateRprLayer(CCmdUI* pCmdUI)
{
	//list<Drawer*>::iterator iter = dl.begin();
	//int i, id;
	//for (i = 1, id = ID_RPRLAYER;
	//	iter != dl.end(); 
	//	++iter, ++i, ++id)
	//{  
	//	if (id == pCmdUI->m_nID) {
	//		bool fEnable = (*iter)->fRepresentation();
	//		pCmdUI->Enable(fEnable);
	//	}
	//}
}

void MapCompositionDoc::OnPropLayer(UINT nID)	
{

	for(int i =0,id = ID_PROPLAYER; i < rootDrawer->getDrawerCount(); ++i,++id)
	{  
		if (id == nID) {
			SpatialDataDrawer *mapdrawer = dynamic_cast<SpatialDataDrawer *>(rootDrawer->getDrawer(i));
			if ( mapdrawer) {
				IlwWinApp()->Execute("prop " + mapdrawer->getBaseMap()->fnObj.sFullPathQuoted());
			}
			return;
		}
	}
}

void MapCompositionDoc::OnHistoLayer(UINT nID){
	for(int i =0,id = ID_HISTOLAYER; i < rootDrawer->getDrawerCount(); ++i,++id)
	{  
		if (id == nID) {
			SpatialDataDrawer *mapdrawer = dynamic_cast<SpatialDataDrawer *>(rootDrawer->getDrawer(i));
			if ( mapdrawer) {
				ShowHistogram(mapdrawer->getBaseMap()->fnObj);
			}
			return;
		}
	}
}

void MapCompositionDoc::OnUpdateHistoLayer(CCmdUI* pCmdUI)	{
}
void MapCompositionDoc::OnUpdatePropLayer(CCmdUI* pCmdUI)	
{
	//list<Drawer*>::iterator iter = dl.begin();
	//int i, id;
	//for (i = 1, id = ID_PROPLAYER;
	//	iter != dl.end(); 
	//	++iter, ++i, ++id)
	//{  
	//	if (id == pCmdUI->m_nID) {
	//		bool fEnable = (*iter)->fProperty();
	//		pCmdUI->Enable(fEnable);
	//	}
	//}
}

ILWIS::NewDrawer *MapCompositionDoc::createBaseMapDrawer(const BaseMap& bmp, const String& type, const String& subtype, OpenType ot, int os) {

	ILWIS::DrawerParameters parms(rootDrawer, rootDrawer);
	ILWIS::NewDrawer *drawer = NewDrawer::getDrawer(type, subtype, &parms);
	CoordBounds cbZoom = rootDrawer->getCoordBoundsZoom(); // backup cbZoom
	CoordBounds cbMapRootDrawer = rootDrawer->getMapCoordBounds(); // backup cbMap
	if (!drawer)
		return 0;

	bool fExtendBounds = !(ot & otKEEPBOUNDS);
	drawer->addDataSource((void *)&bmp, fExtendBounds ? NewDrawer::dsoEXTENDBOUNDS : NewDrawer::dsoNONE);
	MapPtr *mptr = 0;
	if (IOTYPE(bmp->fnObj) == IlwisObject::iotRASMAP)
		mptr = (MapPtr *)bmp.pointer();
	if (mptr)
		rootDrawer->setCoordinateSystem(mptr->gr()->cs());
	else
		rootDrawer->setCoordinateSystem(bmp->cs());
	CoordBounds cbMap = bmp->cb();
	if (mptr) { // Raster Map
		if (mptr->gr()->fGeoRefNone()) { // NONE.grf: rootDrawer in GeoRefNone mode, with rcSize
			rootDrawer->setGeoreference(GeoRef(mptr->rcSize()), fExtendBounds);
		} else if (!cbMap.fValid()) { // Georef CTP that can't compute: rootDrawer in GeoRef mode
			rootDrawer->setGeoreference(mptr->gr(), fExtendBounds);
		} else
			rootDrawer->addCoordBounds(bmp->cs(), cbMap, fExtendBounds);
	} else // Vector Map
		rootDrawer->addCoordBounds(bmp->cs(), cbMap, fExtendBounds);
	ILWIS::PreparationParameters pp(RootDrawer::ptGEOMETRY);
	pp.subType = subtype;
	drawer->prepare(&pp);
	pp.type = RootDrawer::ptRENDER;
	drawer->prepare(&pp);
	rootDrawer->addDrawer(drawer, fExtendBounds);
	addToPixelInfo(bmp, (ComplexDrawer *)drawer);
	MapPaneView * mpv = mpvGetView();
	if (mpv) {
		FrameWindow * frame = mpv->getFrameWindow();
		if ( frame) {
			MapStatusBar *sbar = dynamic_cast<MapStatusBar*>(frame->status);
			if ( sbar) {
				sbar->SetActiveDrawer(drawer);
			}
		}
	}
	String sysFile = bmp->fnObj.sFullName();
	sysFile.toLower();
	if ( fExtendBounds && cbZoom.fValid() && (sysFile.find("\\system\\basemaps") != string::npos)) // restore cbZoom if we added a system background map
		rootDrawer->setCoordBoundsZoom(cbZoom);

	return drawer;
}

BOOL MapCompositionDoc::OnOpenRasterMap(const Map& mp, OpenType ot, int os) 
{
	if (!mp.fValid())
		return FALSE;
	if (!mp->fCalculated()) {
		int iPrior = AfxGetThread()->GetThreadPriority();
		AfxGetThread()->SetThreadPriority(THREAD_PRIORITY_LOWEST);
		mp->Calc();
		AfxGetThread()->SetThreadPriority(iPrior);
	}
	if (!mp->fCalculated())
		return FALSE;


	SetTitle(mp);

	createBaseMapDrawer(mp, "RasterDataDrawer", "Ilwis38", ot, os);	

	if (ot & otEDIT) {
		::AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_EDITLAYER, 0);
	}

	return TRUE;
}

String MapCompositionDoc::getForeignType(const Map& mp) {
	String s;
	int n = ObjectInfo::ReadElement("ForeignFormat", "Expression", mp->fnObj, s);
	if ( n <= 0 )
		return "";
	return s.sHead("(");

}

BOOL MapCompositionDoc::OnOpenObjectCollection(const ObjectCollection& list, OpenType ot, const String& subtype, int os)
{
	BaseMap bmp(list->fnObject(0));
	if (!bmp.fValid())
		return FALSE;
	SetTitle(bmp);
	ComplexDrawer *drawer=0;
	if (ot & otANIMATION) {
		ILWIS::DrawerParameters parms(rootDrawer, rootDrawer);
		drawer = (ComplexDrawer *)NewDrawer::getDrawer("AnimationDrawer", "Ilwis38", &parms);

	}
	else if (ot & otCOLLECTION)  {
		ILWIS::DrawerParameters parms(rootDrawer, rootDrawer);
		drawer = (ComplexDrawer *)NewDrawer::getDrawer("CollectionDrawer", "Ilwis38", &parms);
	}
	if ( drawer == 0)
		throw ErrorObject(TR("No compatible drawer found"));
	drawer->addDataSource((void *)&list);
	rootDrawer->addDrawer(drawer);
	rootDrawer->setCoordinateSystem(bmp->cs());
	rootDrawer->addCoordBounds(bmp->cs(), list->cb(), false);
	drawer->getZMaker()->setSpatialSource(bmp, rootDrawer->getMapCoordBounds());
	//drawer->getZMaker()->setDataSourceMap(bmp);
	addToPixelInfo(list, drawer);
	ILWIS::PreparationParameters pp(RootDrawer::ptGEOMETRY | RootDrawer::ptRENDER,bmp->cs());
	drawer->prepare(&pp);

	return TRUE;
}

BOOL MapCompositionDoc::OnOpenMapList(const MapList& maplist, OpenType ot, int os)
{
	if (maplist->iSize() <= 0)
		return FALSE;
	Map mp = maplist[maplist->iLower()];
	if (!mp.fValid())
		return FALSE;
	SetTitle(maplist);

	ILWIS::DrawerParameters parms(rootDrawer, rootDrawer);
	ILWIS::NewDrawer *drawer;
	if (ot & otANIMATION)
		drawer = NewDrawer::getDrawer("AnimationDrawer", "Ilwis38", &parms);
	else // both otNORMAL and otCOLORCOMP
		drawer = NewDrawer::getDrawer("RasterDataDrawer", "Ilwis38", &parms);

	CoordBounds cbZoom = rootDrawer->getCoordBoundsZoom(); // backup cbZoom
	CoordBounds cbMapRootDrawer = rootDrawer->getMapCoordBounds(); // backup cbMap
	bool fExtendBounds = !(ot & otKEEPBOUNDS);
	drawer->addDataSource((void *)&maplist, fExtendBounds ? NewDrawer::dsoEXTENDBOUNDS : NewDrawer::dsoNONE);
	rootDrawer->setCoordinateSystem(maplist->gr()->cs());
	CoordBounds cbMap = mp->cb();

	if (mp->gr()->fGeoRefNone()) { // NONE.grf: rootDrawer in GeoRefNone mode, with rcSize
		rootDrawer->setGeoreference(GeoRef(mp->rcSize()), fExtendBounds);
	} else if (!cbMap.fValid()) { // Georef CTP that can't compute: rootDrawer in GeoRef mode
		rootDrawer->setGeoreference(mp->gr(), fExtendBounds);
	} else
		rootDrawer->addCoordBounds(mp->cs(), cbMap, fExtendBounds);
	ILWIS::PreparationParameters pp(RootDrawer::ptGEOMETRY | RootDrawer::ptRENDER,0);
	drawer->prepare(&pp);
	rootDrawer->addDrawer(drawer, fExtendBounds);
	addToPixelInfo(maplist, (ComplexDrawer *)drawer);

	return TRUE;
}

BOOL MapCompositionDoc::OnOpenSegmentMap(const SegmentMap& sm, OpenType ot, int os) 
{
	if (!sm.fValid())
		return FALSE;
	if (!sm->fCalculated()) {
		int iPrior = AfxGetThread()->GetThreadPriority();
		AfxGetThread()->SetThreadPriority(THREAD_PRIORITY_LOWEST);
		sm->Calc();
		AfxGetThread()->SetThreadPriority(iPrior);
	}
	if (!sm->fCalculated())
		return FALSE;

	SetTitle(sm);

	createBaseMapDrawer(sm,"FeatureDataDrawer", "Ilwis38", ot, os);
	
	if (ot & otEDIT) {
		::AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_EDITLAYER, 0);
	}

	return TRUE;
}

BOOL MapCompositionDoc::OnOpenPolygonMap(const PolygonMap& pm, OpenType ot, int os) 
{
	if (!pm.fValid())
		return FALSE;
	if (!pm->fCalculated()) {
		int iPrior = AfxGetThread()->GetThreadPriority();
		AfxGetThread()->SetThreadPriority(THREAD_PRIORITY_LOWEST);
		pm->Calc();
		AfxGetThread()->SetThreadPriority(iPrior);
	}
	if (!pm->fCalculated())
		return FALSE;

	SetTitle(pm);

	createBaseMapDrawer(pm,"FeatureDataDrawer", "Ilwis38", ot, os);
	
	if (ot & otEDIT) {
		::AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_EDITLAYER, 0);
	}

	return TRUE;
}

BOOL MapCompositionDoc::OnOpenPointMap(const PointMap& pm, OpenType ot,int os) 
{
	if (!pm.fValid())
		return FALSE;
	if (!pm->fCalculated()) {
		int iPrior = AfxGetThread()->GetThreadPriority();
		AfxGetThread()->SetThreadPriority(THREAD_PRIORITY_LOWEST);
		pm->Calc();
		AfxGetThread()->SetThreadPriority(iPrior);
	}	
	if (!pm->fCalculated())
		return FALSE;

	SetTitle(pm);

	createBaseMapDrawer(pm,"FeatureDataDrawer", "Ilwis38", ot, os);

	if (ot & otEDIT) {
		::AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_EDITLAYER, 0);
	}

	return TRUE;
}

void MapCompositionDoc::addToPixelInfo(const IlwisObject& obj, ComplexDrawer *drw) {
	if (!pixInfoDoc) 
		pixInfoDoc = new PixelInfoDoc();

	pixInfoDoc->OnOpenDocument(obj->fnObj.sFullPathQuoted().c_str(), this,drw);
	pixInfoDoc->UpdateAllViews(0,2);
}

BOOL MapCompositionDoc::OnOpenStereoPair(const StereoPair& stp, OpenType ot)
{
	if (!stp.fValid())
		return FALSE;
	if (!stp->fCalculated()) {
		int iPrior = AfxGetThread()->GetThreadPriority();
		AfxGetThread()->SetThreadPriority(THREAD_PRIORITY_LOWEST);
		stp->Calc();
		AfxGetThread()->SetThreadPriority(iPrior);
	}	
	if (!stp->fCalculated())
		return FALSE;


	SetTitle(stp);

	//AnaglyphDrawer* dw = new AnaglyphDrawer(this, stp);
	//bool fShowForm = !(ot & (otNOASK));
	//if (fShowForm)
	//	fShowForm = dw->fShowDisplayOptions();
	//if (!dw->Configure(fShowForm)) 
	//{
	//	delete dw;
	//	return FALSE;
	//}
	//if (!stp->mapLeft->fHasPyramidFile() || !stp->mapRight->fHasPyramidFile())
	//{
	//	IlwisSettings settings("DefaultSettings");
	//	if (settings.fValue("CreatePyrWhenFirstDisplayed", false))
	//	{
	//		stp->mapLeft->CreatePyramidLayer();				
	//		stp->mapRight->CreatePyramidLayer();				
	//	}
	//}			
	//dw->fNew = false;
	////dl.push_back(dw);

	//mmMapBounds.MinCol() = 0;
	//mmMapBounds.MaxCol() = stp->mapLeft->iCols();
	//mmMapBounds.MinRow() = 0;
	//mmMapBounds.MaxRow() = stp->mapLeft->iLines();
	//mmSize = mmMapBounds;

	return TRUE;
}

BOOL MapCompositionDoc::OnOpenMapView(const MapView& mapview, int os)
{
	try {
		mpv = mapview;
		if (!mpv.fValid())
			return FALSE;

		if (!mpv->fCalculated()) {
			int iPrior = AfxGetThread()->GetThreadPriority();
			AfxGetThread()->SetThreadPriority(THREAD_PRIORITY_LOWEST);
			mpv->Calc();
			AfxGetThread()->SetThreadPriority(iPrior);
		}
		//if (!mpv->fCalculated())
		//	return FALSE;
		// (allow flexibility - a mapview could always be opened .. )

		if (mpv->sDescription.length()) 
		{
			FileName fn = GetPathName();
			String s = fn.sFile;
			s &= ": ";
			s &= mpv->sDescription;
			CDocument::SetTitle(s.sVal());
		}
		FileName fn = GetPathName();
		const String section = "Root";
		String type;
		int iRes = ObjectInfo::ReadElement(section.c_str(),"Type",fn,type);
		if (iRes != 0 && type == "RootDrawer") {
			rootDrawer->load(fn,section);
			ILWIS::PreparationParameters pp(NewDrawer::ptRESTORE,0);
			rootDrawer->prepare(&pp);
			for(int i=0; i < rootDrawer->getDrawerCount(); ++i) {
				SpatialDataDrawer *spdrw = dynamic_cast<SpatialDataDrawer *>(rootDrawer->getDrawer(i));
				if ( spdrw) {
					BaseMapPtr *bmptr = spdrw->getBaseMap();
					BaseMap bmp(bmptr->fnObj);
					addToPixelInfo(bmp, spdrw);
				}
			}

			return TRUE;
		} else if (0 != ObjectInfo::ReadElement("RootDrawer","Type",fn,type)) {
			rootDrawer->setCoordBoundsMap(CoordBounds(0,0,1,1));
			MessageBox(0, TR("This MapView can only be opened by ILWIS 3.8 til 3.8.5 .\nPlease close the MapWindow and re-create the MapView.\nDo not make any changes or save this MapView with this version of ILWIS.").c_str(), TR("Can't open old MapView").c_str(), MB_OK | MB_ICONEXCLAMATION);
			return TRUE;
		} else {
			// Unfortunately we can't prevent the MapWindow from opening, at this stage. Regardless of the ErrorObject or return FALSE, the MapWindow will open. We have to make the best out of it.
			rootDrawer->setCoordBoundsMap(CoordBounds(0,0,1,1));
			MessageBox(0, TR("This MapView can only be opened by ILWIS 3.7.2 or older.\nPlease close the MapWindow and re-create the MapView.\nDo not make any changes or save this MapView with this version of ILWIS.").c_str(), TR("Can't open old MapView").c_str(), MB_OK | MB_ICONEXCLAMATION);
			//throw ErrorObject(TR("This MapView can only be opened by ILWIS 3.7.2 or older.\nPlease close the MapWindow and re-create the MapView."));
			return TRUE;
		}
	}
	catch (ErrorObject& err) 
	{
		err.Show();
		return FALSE;
	}
}

NewDrawer* MapCompositionDoc::drDrawer(const MapView& view, const char* sSection)
{
	//String sType;
	//view->ReadElement(sSection, "Type", sType);
	//if ("MapDrawer" == sType)
	//	return new MapDrawer(this, view, sSection);
	//else if ("MapListDrawer" == sType)
	//	return new MapListDrawer(this, view, sSection);
	//else if ("MapListColorCompDrawer" == sType)
	//	return new MapListColorCompDrawer(this, view, sSection);
	//else if ("SegmentMapDrawer" == sType)
	//	return new SegmentMapDrawer(this, view, sSection);
	//else if ("PolygonMapDrawer" == sType)
	//	return new PolygonMapDrawer(this, view, sSection);
	//else if ("GridDrawer" == sType)
	//	return new GridDrawer(this, view, sSection);
	//else if ("Grid3DDrawer" == sType) {
	//	fGrid3DDrawer = true; 
	//	return new Grid3DDrawer(this, view, sSection);
	//}
	//else if ("PointMapDrawer" == sType || "PointDrawer" == sType )
	//	return new PointMapDrawer(this, view, sSection);
	//else if ("AnnotationTextDrawer" == sType)
	//	return new AnnotationTextDrawer(this, view, sSection);
	//else if ("GraticuleDrawer"==sType )
	//	return new GraticuleDrawer(this, view, sSection);
	//else if ("AnaglyphDrawer" == sType)
	//	return new AnaglyphDrawer(this, view, sSection);
	return 0;
}

class AddLayerForm: public FormWithDest
{
public:
	AddLayerForm(CWnd* parent, String* sName, long *c, bool *e) 
		: FormWithDest(parent, TR("Add Data Layer")), choice(c), extendBounds(e)
	{
		types.push_back("all");
		types.push_back("Raster maps.mpr");
		types.push_back("Polygon maps.mpa");
		types.push_back("Segment maps.mps");
		types.push_back("Point maps.mpp");
		types.push_back("Animations.Animation");
		types.push_back("Color composite.ColorComposite");
		types.push_back("Base maps.AllLayers");
		new FieldBlank(root);
		fdtl = new FieldDataTypeLarge(root, sName, "ioc.mpr.mpl.mps.mpa.mpp");
	
		fos = new FieldOneSelectString(root,TR("Filter"),choice,types);
		fos->SetIndependentPos();
		fos->SetCallBack((NotifyProc)&AddLayerForm::filter);
		new CheckBox(root, TR("Extend Map Boundaries"), extendBounds);
		SetMenHelpTopic("ilwismen\\add_layer_to_map_window.htm");
		create();
	}

	int filter(Event *) { 
		fos->StoreData();
		String flt = ".ioc.mpr.mpl.mps.mpa.mpp";
		fdtl->useBaseMaps(false);
		if ( *choice == 1)
			flt = ".mpr";
		if ( *choice == 2)
			flt = ".mpa";
		if ( *choice == 3)
			flt = ".mps";
		if ( *choice == 4)
			flt = ".mpp";
		if ( *choice == 5)
			flt = ".mpl.ioc";
		if ( *choice == 6)
			flt = ".mpl";
		if ( *choice == 7) {
			fdtl->useBaseMaps(true);
		}
		
			fdtl->SetExt(flt);
		return 1;
	}
private:
	long *choice;
	bool *extendBounds;
	vector<String> types;
	FieldOneSelectString *fos;
	FieldDataTypeLarge * fdtl;
};

class AddRasForm: public FormWithDest
{
public:
	AddRasForm(CWnd* parent, String* sName)
		: FormWithDest(parent, TR("Add Segment Map"))
	{
		new FieldBlank(root);
		new FieldDataTypeLarge(root, sName, ".mpr");
		//    new FieldSegmentMap(root, SDUiSegMap, sName);
		SetMenHelpTopic("");
		create();
	}
};

class AddSegForm: public FormWithDest
{
public:
	AddSegForm(CWnd* parent, String* sName)
		: FormWithDest(parent, TR("Add Segment Map"))
	{
		new FieldBlank(root);
		new FieldDataTypeLarge(root, sName, ".mps");
		//    new FieldSegmentMap(root, SDUiSegMap, sName);
		SetMenHelpTopic("");
		create();
	}
};

class AddPolForm: public FormWithDest
{
public:
	AddPolForm(CWnd* parent, String* sName)
		: FormWithDest(parent, TR("Add Polygon Map"))
	{
		new FieldBlank(root);
		new FieldDataTypeLarge(root, sName, ".mpa");
		//    new FieldSegmentMap(root, SDUiSegMap, sName);
		SetMenHelpTopic("");
		create();
	}
};

class AddPntForm: public FormWithDest
{
public:
	AddPntForm(CWnd* parent, String* sName)
		: FormWithDest(parent, TR("Add Point Map"))
	{
		new FieldBlank(root);
		new FieldDataTypeLarge(root, sName, ".mpp");
		//    new FieldSegmentMap(root, SDUiSegMap, sName);
		SetMenHelpTopic("");
		create();
	}
};

bool MapCompositionDoc::fAppendable(const FileName& fn)
{
	bool fOk = false;
	if (fn.sExt == ".mps" || fn.sExt == ".mpa" || 
		fn.sExt == ".mpp" ||
		fn.sExt == ".atx") {
			fOk = true;
			// melding op status regel?
	}
	else if (fn.sExt == ".mpr")
	{
		fOk = true;
	}
	else if (fn.sExt == ".mpl") 
	{
		fOk = true;
	}
	else if (fn.sExt == ".grf" || fn.sExt == ".csy") {
		fOk = true; 
		// melding op status regel?
	} else if ( IOTYPE(fn) == IlwisObject::iotOBJECTCOLLECTION) {
		fOk = true;
	}
	return fOk;
}

NewDrawer* MapCompositionDoc::drAppend(const FileName& fn, IlwisDocument::OpenType op, int os, const String& subtype)
{
	try{
		if (!fAppendable(fn))
		{
			String sErr(TR("%S cannot be added as a layer").c_str(), fn.sShortName());
			AfxGetMainWnd()->MessageBox(sErr.c_str(), TR("Add data layer").c_str(), MB_OK|MB_ICONEXCLAMATION);
			return 0;
		}    
		NewDrawer* dr = 0;
		// add layer
		if (".mps" == fn.sExt || ".mpa" == fn.sExt || ".mpp" == fn.sExt) {
			BaseMap bm(fn);
			dr = drAppend(bm,op,os, subtype);
		}
		else if (".mpr" == fn.sExt) {
			Map mp(fn);
			dr = drAppend(mp, op, os, subtype);
		}
		else if (".mpl" == fn.sExt) {
			MapList ml(fn);
			dr = drAppend(ml,op, os, subtype);
		}
		else if (".ioc" == fn.sExt) {
			ObjectCollection oc(fn);
			dr = drAppend(oc,op, os, subtype);
		}
		else if (".csy" == fn.sExt) {
			CoordSystem csy(fn);
			SetCoordSystem(csy);
		}
		UpdateAllViews(0);
		return dr;
	}catch (ErrorObject& err) {
		err.Show();
	}
	return 0;
}

void MapCompositionDoc::OnAddLayer()
{
	String sName;
	long choice = 0;
	bool extendBounds = false;
	AddLayerForm frm(wndGetActiveView(), &sName, &choice, &extendBounds);
	bool fOk = frm.fOkClicked();
	if (fOk) {
		FileName fn(sName);
		IlwisDocument::OpenType ot = otUNKNOWN;
		if ( choice == 5)
			ot = otANIMATION;
		if ( choice == 6)
			ot = otCOLORCOMP;
		if (!extendBounds)
			ot = (IlwisDocument::OpenType)(ot | otKEEPBOUNDS);
		if ( IOTYPE(fn) == IlwisObject::iotOBJECTCOLLECTION)
			drAppend(fn, ot == otUNKNOWN ? IlwisDocument::otCOLLECTION : ot);
		else if ( IOTYPE(fn) == IlwisObject::iotMAPLIST) {
			drAppend(fn, ot);
		}
		else{
			IlwisDocument::OpenType ot = (IlwisDocument::OpenType)(IlwisDocument::otUNKNOWN | (extendBounds ? 0 : otKEEPBOUNDS));
			drAppend(fn, ot);
		}
		ChangeState();
		UpdateAllViews(0,2);
	}
}

void MapCompositionDoc::OnAddRasMap()
{
	String sName;
	AddRasForm frm(wndGetActiveView(), &sName);
	bool fOk = frm.fOkClicked();
	if (fOk) {
		FileName fn(sName);
		Map mp(fn);
		NewDrawer* dr = drAppend(mp);
		if (dr)	// could be null
			ChangeState();
			UpdateAllViews(0,2);
	}
}

void MapCompositionDoc::OnAddSegMap()
{
	String sName;
	AddSegForm frm(wndGetActiveView(), &sName);
	bool fOk = frm.fOkClicked();
	if (fOk) {
		FileName fn(sName);
		SegmentMap sm(fn);
		NewDrawer* dr = drAppend(sm);
		if ( dr) {
			ChangeState();
			UpdateAllViews(0,2);
		}
	}
}

void MapCompositionDoc::OnAddPolMap()
{
	String sName;
	AddPolForm frm(wndGetActiveView(), &sName);
	bool fOk = frm.fOkClicked();
	if (fOk) {
		FileName fn(sName);
		PolygonMap pm(fn);
		NewDrawer* dr = drAppend(pm);
		if ( dr ) {
			ChangeState();
			UpdateAllViews(0,2);
		}
	}
}

void MapCompositionDoc::OnAddPntMap()
{
	String sName;
	AddPntForm frm(wndGetActiveView(), &sName);
	bool fOk = frm.fOkClicked();
	if (fOk) {
		FileName fn(sName);
		PointMap pm(fn);
		NewDrawer* dr = drAppend(pm);
		if ( dr) {
			ChangeState();
			UpdateAllViews(0,2);
		}
	}
}

bool MapCompositionDoc::fCoordSystemOk(const BaseMap& bmap)
{
	if (!bmap.fValid()) 
		return false;
	return true;
}

void MapCompositionDoc::RemoveDrawer(ILWIS::NewDrawer* drw)
{
	ChangeState();
	rootDrawer->removeDrawersUsing(drw);
	SpatialDataDrawer *spdrw = dynamic_cast<SpatialDataDrawer *>(drw);
	if ( spdrw) {
		BaseMapPtr *bmptr = spdrw->getBaseMap();
		if ( bmptr) {
			FileName fnObj = bmptr->fnObj;
			pixInfoDoc->remove(fnObj);
		}
	}
	rootDrawer->removeDrawer(drw->getId());
	ChangeState();
}

NewDrawer* MapCompositionDoc::drAppend(const Map& rasmap, int os)
{
	if (!fGeoRefOk(rasmap))
	{
		AfxGetMainWnd()->MessageBox(TR("Raster Map to Add does not have same GeoReference").c_str(), TR("Add data layer").c_str(), MB_OK|MB_ICONEXCLAMATION);
		return 0;
	}
	if (!rasmap->fCalculated())
	{
		int iPrior = AfxGetThread()->GetThreadPriority();
		AfxGetThread()->SetThreadPriority(THREAD_PRIORITY_LOWEST);

		rasmap->Calc();

		AfxGetThread()->SetThreadPriority(iPrior);
	}
	if (!rasmap->fCalculated() && !rasmap->fDefOnlyPossible())
		return 0;
	TableHistogramInfo thi(rasmap);
	NewDrawer *dr = createBaseMapDrawer(rasmap,"RasterDataDrawer","Ilwis38", otUNKNOWN, os);

	ChangeState();
	return dr;
}

NewDrawer* MapCompositionDoc::drAppend(const ObjectCollection& oc, IlwisDocument::OpenType op, int os, const String& subtype)
{
	ILWIS::NewDrawer *drawer = 0;
	if ( op & IlwisDocument::otANIMATION) {
		ILWIS::DrawerParameters parms(rootDrawer, rootDrawer);
		drawer = NewDrawer::getDrawer("AnimationDrawer", subtype, &parms);
		drawer->addDataSource((void *)&oc);
		for(int i=0; i < oc->iNrObjects(); ++i) {
			FileName fn = oc->fnObject(i);
			if ( IOTYPEBASEMAP(fn)) {
				BaseMap bmp(fn);
				rootDrawer->setCoordinateSystem(bmp->cs());
				rootDrawer->addCoordBounds(bmp->cs(), bmp->cb(), false);
			}

		}
		ILWIS::PreparationParameters pp(RootDrawer::ptGEOMETRY | RootDrawer::ptRENDER,0);
		drawer->prepare(&pp);
		rootDrawer->addDrawer(drawer);
		ChangeState();
		UpdateAllViews(0,3);
		mpvGetView()->Invalidate();
	} else if( op & IlwisDocument::otCOLLECTION) {
		OnOpenObjectCollection(oc, op);

	} else {
		for(int i=0; i < oc->iNrObjects(); ++i) {
			FileName fn = oc->fnObject(i);
			if ( IOTYPEBASEMAP(fn)) {
				drAppend(fn);
			}

	/*		if ( obj.fValid() == false)
				continue;
			if ( IOTYPE(obj->fnObj) == IlwisObject::iotMAPLIST) {
				MapList mpl(obj->fnObj);
				drAppend(mpl, true);
			}
			if ( IOTYPE(obj->fnObj) == IlwisObject::iotRASMAP) {
				Map mp(obj->fnObj);
				drAppend(mp, true);
			}
			if ( IOTYPE(obj->fnObj) == IlwisObject::iotPOINTMAP) {
				PointMap pmp(obj->fnObj);
				drAppend(pmp, true);
			}
			if ( IOTYPE(obj->fnObj) == IlwisObject::iotPOLYGONMAP) {
				PolygonMap pmp(obj->fnObj);
				drAppend(pmp, true);
			}
			if ( IOTYPE(obj->fnObj) == IlwisObject::iotSEGMENTMAP) {
				SegmentMap smp(obj->fnObj);
				drAppend(smp, true);
			}*/
		}
	}
	ChangeState();
	UpdateAllViews(0,2);

	return drawer;
}

NewDrawer* MapCompositionDoc::drAppend(const MapList& maplist,IlwisDocument::OpenType op, int os, const String& subtype)
{
	if ( op & IlwisDocument::otANIMATION) {
		ILWIS::DrawerParameters parms(rootDrawer, rootDrawer);
		ILWIS::NewDrawer *drawer = NewDrawer::getDrawer("AnimationDrawer", subtype, &parms);
		CoordBounds cbZoom = rootDrawer->getCoordBoundsZoom(); // backup cbZoom
		CoordBounds cbMapRootDrawer = rootDrawer->getMapCoordBounds(); // backup cbMap
		bool fExtendBounds = !(op & otKEEPBOUNDS);
		drawer->addDataSource((void *)&maplist, fExtendBounds ? NewDrawer::dsoEXTENDBOUNDS : NewDrawer::dsoNONE);
		rootDrawer->setCoordinateSystem(maplist->gr()->cs());
		Map mp = maplist[maplist->iLower()];
		CoordBounds cbMap = mp->cb();
		if (mp->gr()->fGeoRefNone()) { // NONE.grf: rootDrawer in GeoRefNone mode, with rcSize
			rootDrawer->setGeoreference(GeoRef(mp->rcSize()), fExtendBounds);
		} else if (!cbMap.fValid()) { // Georef CTP that can't compute: rootDrawer in GeoRef mode
			rootDrawer->setGeoreference(mp->gr(), fExtendBounds);
		} else
			rootDrawer->addCoordBounds(mp->cs(), cbMap, fExtendBounds);
		ILWIS::PreparationParameters pp(RootDrawer::ptGEOMETRY | RootDrawer::ptRENDER,0);
		drawer->prepare(&pp);
		rootDrawer->addDrawer(drawer, fExtendBounds);
		addToPixelInfo(maplist, (ComplexDrawer *)drawer);
		ChangeState();
		UpdateAllViews(0,3);
		mpvGetView()->Invalidate();
		return drawer;
	} else if ( op & otCOLORCOMP ){
		ILWIS::DrawerParameters parms(rootDrawer, rootDrawer);
		ILWIS::NewDrawer *drawer = NewDrawer::getDrawer("RasterDataDrawer", subtype, &parms);
		CoordBounds cbZoom = rootDrawer->getCoordBoundsZoom(); // backup cbZoom
		CoordBounds cbMapRootDrawer = rootDrawer->getMapCoordBounds(); // backup cbMap
		bool fExtendBounds = !(op & otKEEPBOUNDS);
		drawer->addDataSource((void *)&maplist, fExtendBounds ? NewDrawer::dsoEXTENDBOUNDS : NewDrawer::dsoNONE);
		rootDrawer->setCoordinateSystem(maplist->gr()->cs());
		Map mp = maplist[maplist->iLower()];
		CoordBounds cbMap = mp->cb();
		if (mp->gr()->fGeoRefNone()) { // NONE.grf: rootDrawer in GeoRefNone mode, with rcSize
			rootDrawer->setGeoreference(GeoRef(mp->rcSize()), fExtendBounds);
		} else if (!cbMap.fValid()) { // Georef CTP that can't compute: rootDrawer in GeoRef mode
			rootDrawer->setGeoreference(mp->gr(), fExtendBounds);
		} else
			rootDrawer->addCoordBounds(mp->cs(), cbMap, fExtendBounds);
		ILWIS::PreparationParameters pp(RootDrawer::ptGEOMETRY | RootDrawer::ptRENDER,0);
		drawer->prepare(&pp);
		rootDrawer->addDrawer(drawer, fExtendBounds);
		addToPixelInfo(maplist, (ComplexDrawer *)drawer);
	} else {
		for(int i = 0; i < maplist->iSize(); ++i) {
			drAppend(maplist[i]->fnObj);
		}
	}
	return 0;
}

NewDrawer* MapCompositionDoc::drAppend(const BaseMap& mp,IlwisDocument::OpenType op, int os, const String& subtype)
{
	if (fCoordSystemOk(mp)) {
		if (!mp->fCalculated())
		{
			int iPrior = AfxGetThread()->GetThreadPriority();
			AfxGetThread()->SetThreadPriority(THREAD_PRIORITY_LOWEST);

			mp->Calc();
			AfxGetThread()->SetThreadPriority(iPrior);
		}
		if (!mp->fCalculated())
			return 0;

		ILWIS::DrawerParameters parms(rootDrawer, rootDrawer);
		ILWIS::NewDrawer *drawer;
		if ( op & IlwisDocument::otANIMATION) {
			drawer = NewDrawer::getDrawer("AnimationDrawer", subtype, &parms);
		}
		else {
			if ( IlwisObject::iotObjectType(mp->fnObj) !=  IlwisObject::iotRASMAP)
				drawer = createBaseMapDrawer(mp, "FeatureDataDrawer", subtype, op, os);
			else
				drawer = createBaseMapDrawer(mp, "RasterDataDrawer", subtype, op, os);
		}
		ChangeState();
		//UpdateAllViews(0,3);
		MapPaneView * mpv = mpvGetView();
		if (mpv)
			mpv->Invalidate();
		return drawer;
	}    
	return 0;
}


/*
void MapCompositionDoc::OnAddNorthArrow()
{
Drawer *dr = new NorthDrawer(this);
if (!dr->Configure()) {
dr->fAct = false;
delete dr;
}
else {
dl.push_back(dr);
ChangeState();
UpdateAllViews(0,2);
}
}
*/

class _export AddAnnTextForm: public FormWithDest
{
public:
	AddAnnTextForm(CWnd* parent, String* sName)
		: FormWithDest(parent, TR("Add Annotation Text"))
		//, mw(parent)
	{
		new FieldBlank(root);
		new FieldDataTypeLarge(root, sName, ".ATX");
		//    new PushButton(root, TR("&Create..."), (NotifyProc)&AddAnnTextForm::CreateAtx);
		SetMenHelpTopic("");
		create();
	}
private:
	/*
	int CreateAtx(zEvent*) {
	String sAtx;
	bool fOk;
	{
	zRect rect;
	mw->mppn->getInterior(rect);
	MinMax mm = mw->mppn->mps->mmRect(rect);
	zPrinterDisplay *pd = mw->mppn->prDisplay();
	PrinterPositioner psn(pd, mm, mw->mppn->georef);
	double rWorkScale = psn.rScale();

	FormCreateAnnotationText frm(this, &sAtx, rWorkScale);
	fOk = frm.fOkClicked();
	}
	if (fOk) {
	FileName fn(sAtx);
	fn.sExt = ".atx";
	mw->mppn->EditNamedLayer(fn);
	return endCancel(0);
	}
	return 0;
	}
	MapWindow* mw;
	*/
};




/*
void MapCompositionDoc::OnAddBitmap()
{
class BmForm: public FormWithDest
{
public:
BmForm(CWnd* parent, String* sName, bool* fIsotropic)
: FormWithDest(parent, TR("Add Bitmap or Picture"))
{
new FieldDataTypeLarge(root, sName, ".bmp.wmf.emf");
new CheckBox(root, TR("&Isotropic"), fIsotropic);
SetMenHelpTopic("ilwismen\\layout_editor_insert_bitmap_picture.htm");
create();
}
};
String sName;
bool fIsotropic;
BmForm form(wndGetActiveView(), &sName, &fIsotropic);
if (form.fOkClicked()) {
FileName fn(sName);
Drawer* dr = 0;
if (".bmp" == fn.sExt)
dr = new BitmapDrawer(this, fn, fIsotropic);
else if (".wmf" == fn.sExt || ".emf" == fn.sExt)  
dr = new MetaFileDrawer(this, fn, fIsotropic);
if (dr)	{
dl.push_back(dr);
ChangeState();
UpdateAllViews(0,2);
}
}
}

Drawer* MapCompositionDoc::drAppend(HENHMETAFILE hmf)
{
Drawer* dr = new MetaFileDrawer(this, hmf);
if (dr)	{
dl.push_back(dr);
ChangeState();
UpdateAllViews(0,2);
}
return dr;
}

Drawer* MapCompositionDoc::drAppend(HBITMAP hbm)
{
Drawer* dr = new BitmapDrawer(this, hbm);
if (dr)	{
dl.push_back(dr);
ChangeState();
UpdateAllViews(0,2);
}
return dr;
}
*/

void MapCompositionDoc::SetCoordSystem(const CoordSystem& cs)
{
	if (!cs->fConvertFrom(rootDrawer->getCoordinateSystem())) {
		String s(TR("Coordinate system not usable here"));
		wndGetActiveView()->MessageBox(s.c_str(), TR("Replace Coordinate System").c_str(), MB_OK|MB_ICONEXCLAMATION);
		return; 
	}
	rootDrawer->setCoordinateSystem(cs, true);
	PreparationParameters pp(NewDrawer::ptGEOMETRY | NewDrawer::ptRENDER | NewDrawer::ptNEWCSY);
	rootDrawer->prepare(&pp);

	mpvGetView()->Invalidate();
}


HistogramGraphView *MapCompositionDoc::getHistoView(const FileName& fn){
	HistIter iter = gbHist.find(fn.sPhysicalPath());
	if (iter != gbHist.end()) {
		return static_cast<HistogramGraphView *>((*iter).second->view);
	}
	return 0;

}

void MapCompositionDoc::replaceHistFile(const FileName& fnOld, const FileName& fnNew){
	HistIter iter = gbHist.find(fnOld.sPhysicalPath());
	if (iter != gbHist.end()) {
		GeneralBar *bar = static_cast<GeneralBar *>((*iter).second);
		gbHist.erase(iter);
		gbHist[fnNew.sPhysicalPath()] = bar;
	}	
}

void MapCompositionDoc::ShowHistogram(const FileName& fn, bool show, const RangeReal& rrX,const RangeReal& rrY, DrawerTool *tool, int index)
{
	HistIter iter = gbHist.find(fn.sPhysicalPath());
	//if ( iter == gbHist.end()) {
	//	TRACE("STOP");
	//}
	if (iter != gbHist.end() && show == false) {
		BOOL fShown = (*iter).second->IsWindowVisible();
		//(*iter).second->SendMessage(WM_CLOSE);
		HistogramGraphDoc* hgd = (HistogramGraphDoc*)(*iter).second->view->GetDocument();
		hgd->RemoveView((*iter).second->view);
		delete (*iter).second;
		gbHist.erase(iter);
		mpvGetView()->GetParentFrame()->RecalcLayout();
		if (fShown)
			return;


	}
	CWnd* wnd = wndGetActiveView();
	CFrameWnd* fw = wnd->GetTopLevelFrame();
	if (0 == fw)
		return;
	for (int i = 0; i < rootDrawer->getDrawerCount(); ++i) 
	{
		NewDrawer* dr = rootDrawer->getDrawer(i);
		SpatialDataDrawer* md = dynamic_cast<SpatialDataDrawer*>(dr);
		if (!md)
			continue;
		if (md->getType() == "ColorCompositeDrawer") 
		{
			throw ErrorObject("THis must be moved");  // HistogramRGBGraphView may not remain here
			HistogramRGBGraphView* hgv = new HistogramRGBGraphView(md);
			GeneralBar *gb = new GeneralBar;
			gb->view = hgv;
			gb->Create(fw, (dynamic_cast<FrameWindow*>(fw))->iNewBarID());
			hgv->Create(gb);
			AddView(hgv);
			hgv->OnInitialUpdate();
			String sTitle(TR("Histogram of %S").c_str(), md->getName());
			gb->SetWindowText(sTitle.c_str());
			fw->FloatControlBar(gb,CPoint(100,100));
			fw->ShowControlBar(gb,TRUE,FALSE);
			gbHist[fn.sPhysicalPath()] = gb;

			return;
		}
		if (md->getType() == "RasterDataDrawer" || md->getType() == "AnimationDrawer") // && md->dm()->pdv()) 
		{
			if ( md->getBaseMap()->fnObj != fn)
				continue;
			HistogramGraphDoc* hgd = new HistogramGraphDoc;
			Map mp(fn);
			TableHistogramInfo thi(mp);
			if ( rrX.fValid())
				hgd->setOverruleRangeX(rrX);
			if ( rrY.fValid())
				hgd->setOverruleRangeY(rrY);
			hgd->OnOpenDocument(thi.tbl());
			HistogramGraphView* hgv = new HistogramGraphView(md, this, tool);
			GeneralBar *gb = new GeneralBar;
			gb->view = hgv;
			gb->Create(fw, (dynamic_cast<FrameWindow*>(fw))->iNewBarID());
			hgv->Create(gb);
			hgd->AddView(hgv);
			hgv->OnInitialUpdate();
			IlwWinApp()->PostThreadMessage(ILW_ADDDATAWINDOW, (WPARAM)hgv->m_hWnd, 0);
			fw->DockControlBar(gb, AFX_IDW_DOCKBAR_TOP);
			String sTitle(TR("Histogram of %S").c_str(), mp->sName());
			gb->SetWindowText(sTitle.c_str());
			//fw->FloatControlBar(gb,CPoint(100,100));
			fw->ShowControlBar(gb,TRUE,FALSE);
			gbHist[fn.sPhysicalPath()] = gb;
			return;
		}
	}
}

void MapCompositionDoc::OnUpdateShowHistogram(CCmdUI* pCmdUI)	
{
	//BOOL fShown = false;
	//if (gbHist) 
	//	fShown = gbHist->IsWindowVisible();
	//bool fPossible = false;
	//CWnd* wnd = wndGetActiveView();
	//CFrameWnd* fw = wnd->GetTopLevelFrame();
	//if (fw) {
	//	for (int i = 0; i < rootDrawer->getDrawerCount(); ++i) 
	//	{
	//		NewDrawer* dr = rootDrawer->getDrawer(i);
	//		SpatialDataDrawer* md = dynamic_cast<SpatialDataDrawer*>(dr);
	//		if (md && (md->getBaseMap()->dm()->pdv() || md->getBaseMap()->dm()->pdsrt())) 
	//		{
	//			fPossible = true;
	//			break;
	//		}
	//	}
	//}
	//pCmdUI->SetCheck(fShown);
	//pCmdUI->Enable(fPossible||fShown);
}

BOOL MapCompositionDoc::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	if (fInCmdMsg)
		return FALSE;
	if (CatalogDocument::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;
	fInCmdMsg = true;
	BOOL fReturn = FALSE;
	//if (gbHist && IsWindow(gbHist->m_hWnd) && gbHist->IsWindowVisible())
	//	fReturn = gbHist->view->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
	fInCmdMsg = false;
	return fReturn;
}

COleServerItem* MapCompositionDoc::OnGetEmbeddedItem()
{
	return new MapCompositionSrvItem(this,TRUE);
}

void MapCompositionDoc::ChangeState() 
{ 
	iListState++; 
	SetModifiedFlag(); 
}

MapPaneView* MapCompositionDoc::mpvGetView() const
{
	POSITION pos = GetFirstViewPosition();
	while (0 != pos) {
		CView* vw = GetNextView(pos);
		MapPaneView* mpv = dynamic_cast<MapPaneView*>(vw);
		if (mpv)
			return mpv;
	}
	return 0;
}

LayerTreeView* MapCompositionDoc::ltvGetView() const
{
	POSITION pos = GetFirstViewPosition();
	while (0 != pos) {
		CView* vw = GetNextView(pos);
		LayerTreeView* ltv = dynamic_cast<LayerTreeView*>(vw);
		if (ltv)
			return ltv;
	}
	return 0;
}

void MapCompositionDoc::OnOpen() 
{
	class ShowBaseMapForm: public FormWithDest
	{
	public:
		ShowBaseMapForm(CWnd* parent, String* sName)
			: FormWithDest(parent, TR("Open Object"))
		{
			new FieldDataObject(root, sName);
			SetMenHelpTopic("ilwismen\\open_show_map_or_other_object.htm");
			create();
		}
	};
	if (!SaveModified())
		return;
	String sMap;
	ShowBaseMapForm frm(wndGetActiveView(), &sMap);
	if (frm.fOkClicked()) {
		OnNewDocument();
		UpdateAllViews(0,0);
		try {
			OnOpenDocument(sMap.c_str());
		}
		catch (ErrorObject& err) {
			err.Show();
			OnNewDocument();
		}
		ChangeState();
		UpdateAllViews(0,3);
	}
}

BOOL MapCompositionDoc::SaveModified()
{
	return fSaveModified(true);
}

BOOL MapCompositionDoc::fSaveModified(bool fAllowCancel)
{
	// largely copied from CDocument::SaveModified()
	if (!IsModified())
		return TRUE;        // ok to continue

	// not opened as MapView
	if (!mpv.fValid())
		return TRUE;

	CString sTitle = GetTitle();
	String sMsg(TR("Save the changes you made to Map View '%s'?").c_str(), (const char*)sTitle);
	UINT nType = MB_ICONEXCLAMATION|MB_TOPMOST;
	if (fAllowCancel)
		nType |= MB_YESNOCANCEL;
	else
		nType |= MB_YESNO;
	switch (wndGetActiveView()->MessageBox(sMsg.c_str(), TR("Save Changes").c_str(), nType))
	{
	case IDCANCEL:
		return FALSE;       // don't continue

	case IDYES:
		// If so, either Save or Update, as appropriate
		if (!DoFileSave())
			return FALSE;       // don't continue
		break;

	case IDNO:
		// If not saving changes, revert the document
		break;

	default:
		ASSERT(FALSE);
		break;
	}
	return TRUE;    // keep going
}

void MapCompositionDoc::OnDeactivate()
{
	SetModifiedFlag(); 
	m_bRemember = FALSE;
	OnSaveDocument(0);
	UpdateAllItems(0);
	COleServerDoc::OnDeactivate();
}

BOOL MapCompositionDoc::GetFileTypeString(CString& str)
{
	str = "ILWIS MapView";
	return TRUE;
}

void MapCompositionDoc::OnCopyScaleBarLink()
{
	//ScaleBarSrvItem* sbsi = new ScaleBarSrvItem(this, TRUE);
	//sbsi->CopyToClipboard(TRUE);
}

void MapCompositionDoc::OnOpenPixelInfo()
{
	String sList;
	for (int i = 0; i < rootDrawer->getDrawerCount(); ++i) 
	{
		NewDrawer* dr = rootDrawer->getDrawer(i);
		SpatialDataDrawer* md = dynamic_cast<SpatialDataDrawer*>(dr);
		sList &= " ";
		sList &= md->getBaseMap()->fnObj.sFullNameQuoted();
	}
	IlwWinApp()->OpenPixelInfo(sList.c_str());
}

bool MapCompositionDoc::fIsEmpty() const
{
	if ( !rootDrawer)
		return true;

	return rootDrawer->getDrawerCount() == 0;
}

void MapCompositionDoc::setViewName(const FileName& fn) {
	fnView = fn;
}

FileName MapCompositionDoc::getViewName() const{
	return fnView;
}

UINT MapCompositionDoc::selectFeaturesInThread(LPVOID pParam) {
	MapCompositionDoc * doc = (MapCompositionDoc*)pParam;
	if (doc == NULL)
		return 1;

	doc->csSelectionThread.Lock();

	while (!doc->m_fAbortSelectionThread)
	{
		while(doc->m_selectionQueue.size() > 0) {
			doc->csSelectionQueue.Lock();
			const RowSelectInfo * inf = doc->m_selectionQueue[doc->m_selectionQueue.size() - 1];
			doc->m_selectionQueue.pop_back();
			while(doc->m_selectionQueue.size() > 0) { // delete all older queued selection messages; only the last one counts
				const RowSelectInfo * inf = doc->m_selectionQueue[doc->m_selectionQueue.size() - 1];
				doc->m_selectionQueue.pop_back();
				delete inf;
			}		
			doc->csSelectionQueue.Unlock();

			if ( !doc->m_fAbortSelectionThread && inf->sender != (long)doc->mpvGetView()) {
				PreparationParameters pp(NewDrawer::ptRENDER);
				pp.rowSelect = *inf;
				doc->rootDrawer->prepare(&pp); // do we need a semaphore on "prepare"? can prepare be called simultaneously from another thread?
				if (IsWindow(doc->mpvGetView()->m_hWnd))
					doc->mpvGetView()->Invalidate();
			}
			delete inf;
		}
		if (!doc->m_fAbortSelectionThread)
			doc->m_selectionThread->SuspendThread(); // wait here, and dont consume CPU time either
	}
	
	doc->m_fAbortSelectionThread = false;
	doc->csSelectionThread.Unlock();
	return 0;
}

void MapCompositionDoc::selectFeatures(const RowSelectInfo * inf) {
	csSelectionQueue.Lock();
	m_selectionQueue.push_back(inf);
	csSelectionQueue.Unlock();
	if (!m_selectionThread)
		m_selectionThread = AfxBeginThread(selectFeaturesInThread, this);
	else
		m_selectionThread->ResumeThread();
}


