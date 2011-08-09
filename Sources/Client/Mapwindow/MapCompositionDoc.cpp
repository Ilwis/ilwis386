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
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\ilwis.h"
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
//#include "Client\Mapwindow\Drawers\BitmapDrawer.h"
//#include "Client\Mapwindow\Drawers\MetaFileDrawer.h"
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
#include "Client\Mapwindow\Printing\Printing.h"
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

	ON_COMMAND(ID_COORDSYSTEM, OnChangeCoordSystem)
	ON_UPDATE_COMMAND_UI(ID_COORDSYSTEM, OnUpdateChangeCoordSystem)
	ON_COMMAND(ID_SAVEVIEW, OnSaveView)
	ON_COMMAND(ID_SAVEVIEWAS, OnSaveViewAs)
	ON_COMMAND(ID_CREATE_LAYOUT, OnCreateLayout)
	ON_COMMAND(ID_FILE_OPEN, OnOpen)
	ON_COMMAND(ID_COPYSCALEBARLINK, OnCopyScaleBarLink)
	ON_COMMAND(ID_OPENPIXELINFO, OnOpenPixelInfo)
	ON_COMMAND(ID_SHOWHISTOGRAM, OnShowHistogram)
	ON_UPDATE_COMMAND_UI(ID_SHOWHISTOGRAM, OnUpdateShowHistogram)

	//ON_COMMAND_RANGE(ID_LAYFIRST, ID_LAYFIRST+900, OnDataLayer)
	ON_COMMAND_RANGE(ID_DOMLAYER, ID_DOMLAYER+900, OnDomainLayer)
	ON_COMMAND_RANGE(ID_RPRLAYER, ID_RPRLAYER+900, OnRprLayer)
	ON_COMMAND_RANGE(ID_EDITLAYER, ID_EDITLAYER+900, OnEditLayer)
	ON_COMMAND_RANGE(ID_PROPLAYER, ID_PROPLAYER+900, OnPropLayer)
	ON_UPDATE_COMMAND_UI_RANGE(ID_LAYFIRST, ID_LAYFIRST+900, OnUpdateDataLayer)
	ON_UPDATE_COMMAND_UI_RANGE(ID_DOMLAYER, ID_DOMLAYER+900, OnUpdateDomainLayer)
	ON_UPDATE_COMMAND_UI_RANGE(ID_RPRLAYER, ID_RPRLAYER+900, OnUpdateRprLayer)
	ON_UPDATE_COMMAND_UI_RANGE(ID_EDITLAYER, ID_EDITLAYER+900, OnUpdateEditLayer)
	ON_UPDATE_COMMAND_UI_RANGE(ID_PROPLAYER, ID_PROPLAYER+900, OnUpdatePropLayer)
END_MESSAGE_MAP()


MapCompositionDoc::MapCompositionDoc()
: gbHist(0)
{
	iListState = 0;
	fInCmdMsg = false;
	rDfltScale = rUNDEF;
	rootDrawer = new RootDrawer();
	fnView = FileName("mapview.mpv");
	selectedDrawer = 0;
	pixInfoDoc = 0;
}

MapCompositionDoc::~MapCompositionDoc()
{
	delete rootDrawer;
}

void MapCompositionDoc::DeleteContents()
{
	delete gbHist;
	gbHist = 0;
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
		String name = rootDrawer->getDrawer(0)->iconName();
		return zIcon();
	}
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

BOOL MapCompositionDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	return OnOpenDocument(lpszPathName, otNORMAL);
}

BOOL MapCompositionDoc::OnOpenDocument(LPCTSTR lpszPath, ParmList& pm) 
{
	FileName fn(pm.sGet("output") != "" ? pm.sGet("output") : lpszPath);
	String sC = pm.sCmd();
	if ( fn.fExist() == false && pm.fExist("collection") ) // implicit object
	{
		if ( pm.sGet("method") == "") {
			ForeignCollection fc(pm.sGet("collection"));
			pm.Add(new Parm("method", fc->sGetMethod()));
		}
		ForeignCollection::CreateImplicitObject(fn, pm);
	}
	if ( IlwisObject::iotObjectType(fn) > IlwisObject::iotRASMAP && IlwisObject::iotObjectType(fn) < IlwisObject::iotTABLE) 
	{
		int ot = pm.sGet("mode").iVal();
		if ( ot == iUNDEF) ot = 0;
		if ( IlwisObject::iotObjectType(fn) == IlwisObject::iotPOINTMAP)
			return OnOpenPointMap(PointMap(fn), (IlwisDocument::OpenType)ot);
		if ( IlwisObject::iotObjectType(fn) == IlwisObject::iotSEGMENTMAP)
			return OnOpenSegmentMap(SegmentMap(fn), (IlwisDocument::OpenType)ot);
		if ( IlwisObject::iotObjectType(fn) == IlwisObject::iotPOLYGONMAP)
			return OnOpenPolygonMap(PolygonMap(fn), (IlwisDocument::OpenType)ot);
	}	
	if (!IlwisDocument::OnOpenDocument(fn.sRelative().c_str()))
		return FALSE;
	Map map(fn);
	return OnOpenDocument(lpszPath);
}

BOOL MapCompositionDoc::OnOpenDocument(LPCTSTR lpszPathName, OpenType ot)
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
			return OnOpenRasterMap(mp,ot);
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
			return OnOpenMapList(ml, ot);
		}
		else if (".ioc" == fn.sExt) {
			ObjectCollection oc(fn);
			if (!oc.fValid())
				return FALSE;
			return OnOpenObjectCollection(oc, ot);
		}
		else if (".mps" == fn.sExt) {
			SegmentMap mp(fn);
			if (!mp.fValid())
				return FALSE;
			return OnOpenSegmentMap(mp, ot);
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
			return OnOpenPolygonMap(mp, ot);
		}
		else if (".mpp" == fn.sExt) {
			PointMap mp(fn);
			if (!mp.fValid())
				return FALSE;
			return OnOpenPointMap(mp, ot);
		}
		else if (".mpv" == fn.sExt) {
			MapView mpv(fn);
			if (!mpv.fValid())
				return FALSE;
			return OnOpenMapView(mpv);
		}
		else if (".sms" == fn.sExt) {
			SampleSet sms(fn);
			if (!sms.fValid())
				return FALSE;
			MapList mpl = sms->mpl();
			if (!mpl.fValid())
				return FALSE;
			if (!OnOpenMapList(mpl,otNORMAL))
				return FALSE;
			mpvGetView()->EditNamedLayer(fn);
			//		::AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_EDITSMS, 0);
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
	catch (ErrorObject& err) {
		err.Show();
	}
	return FALSE;
}

void MapCompositionDoc::OnCreateLayout()
{
	Printing printer;
	printer.print(this);
}

void MapCompositionDoc::OnSaveView()
{
	fUseSerialize = true;
	if (!mpv.fValid())
		OnSaveViewAs();
	else {
		rootDrawer->store(mpv->fnObj,"Root");
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
			//      setHelpItem("ilwismen\save_view_as.htm"Forms);
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
		mpv->Store();
		rootDrawer->store(mpv->fnObj,"Root");
		SetTitle(mpv);
	}
}

void MapCompositionDoc::StoreView()
{
	ObjectDependency objdep;
	//mpv->Store();
	//mpv->WriteElement("MapView", "Scale", rDfltScale);

	//MapPaneView* vw = mpvGetView();
	//if (vw) {
	//	CRect rect;
	//	vw->GetClientRect(&rect);
	//	mpv->WriteElement("MapView", "Width", rect.Width());
	//	mpv->WriteElement("MapView", "Height", rect.Height());
	//}

	//int iLayers = 0;
	//rootDrawer->store(mpv->fnObj,"");
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
		vector<string> values;
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
		double lx = cb.MinX() - cb.width() * rLeft / 100.0;
		double rx = cb.MaxX() + cb.width() * rRight / 100.0;
		if ( isNorthOriented) {
			ty = cb.MinY() - cb.height() * rTop / 100.0;
			by = cb.MaxY() + cb.height() * rBottom / 100.0;
		} else {
			ty = cb.MinY() - cb.height() * rBottom / 100.0;
			by = cb.MaxY() + cb.height() * rTop / 100.0;
		}
		cb = CoordBounds(Coord(lx,ty), Coord(rx,by));
		rootDrawer->addCoordBounds(rootDrawer->getCoordinateSystem(),cb);
		rootDrawer->getDrawerContext()->doDraw();
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
		rootDrawer->setCoordBoundsMap(cb);
		rootDrawer->getDrawerContext()->doDraw();
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
	  case ID_LAYFIRST:
		  iFlag = mapdrawer->isActive() 
			  ? MF_CHECKED : MF_UNCHECKED;
		  men.CheckMenuItem(id, MF_BYCOMMAND | iFlag);
		  break;
	  //case ID_ATTLAYER:  
		 // iFlag = !mapdrawer->isActive() || !mapdrawer->getAtttributeTable().fValid()
			//  ? MF_GRAYED : MF_ENABLED;
		 // men.EnableMenuItem(id, MF_BYCOMMAND | iFlag);
		 // break;
	  case ID_RPRLAYER:  
		  iFlag = !mapdrawer->isActive() || ! mapdrawer->getBaseMap()->dm()->rpr().fValid()
			  ? MF_GRAYED : MF_ENABLED;
		  men.EnableMenuItem(id, MF_BYCOMMAND | iFlag);
		  break;
	  case ID_EDITLAYER:  
		  iFlag = !mapdrawer->isEditable()
			  ? MF_GRAYED : MF_ENABLED;
		  men.EnableMenuItem(id, MF_BYCOMMAND | iFlag);
		  break;
	  case ID_PROPLAYER:  
		  iFlag = MF_ENABLED;
		  men.EnableMenuItem(id, MF_BYCOMMAND | iFlag);
		  break;
	  case ID_DOMLAYER:
		  {  
			  BaseMap bm;
			  bm.SetPointer(mapdrawer->getBaseMap());
			  iFlag = !fDomainEditable(bm) ? MF_GRAYED : MF_ENABLED;
			  men.EnableMenuItem(id, MF_BYCOMMAND | iFlag);
		  }
		  break;
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
	//list<Drawer*>::iterator iter = dl.begin();
	//int i, id;
	//for (i = 1, id = ID_PROPLAYER;
	//	iter != dl.end(); 
	//	++iter, ++i, ++id)
	//{  
	//	if (id == nID) {
	//		Drawer* dr = *iter;
	//		dr->Prop();
	//		return;
	//	}
	//}
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




ILWIS::NewDrawer *MapCompositionDoc::createBaseMapDrawer(const BaseMap& bmp, const String& type, const String& subtype) {

	ILWIS::DrawerParameters parms(rootDrawer, rootDrawer);
	ILWIS::NewDrawer *drawer = NewDrawer::getDrawer(type, subtype, &parms);
	drawer->addDataSource((void *)&bmp);
	rootDrawer->setCoordinateSystem(bmp->cs());
	rootDrawer->addCoordBounds(bmp->cs(), bmp->cb(), false);
	ILWIS::PreparationParameters pp(RootDrawer::ptGEOMETRY);
	drawer->prepare(&pp);
	pp.type = RootDrawer::ptRENDER;
	drawer->prepare(&pp);
	rootDrawer->addDrawer(drawer);
	addToPixelInfo(bmp, (ComplexDrawer *)drawer);
	FrameWindow * frame = mpvGetView()->getFrameWindow();
	if ( frame) {
		MapStatusBar *sbar = dynamic_cast<MapStatusBar*>(frame->status);
		if ( sbar) {
			sbar->SetActiveDrawer(drawer);
		}
	}

	return drawer;
}

BOOL MapCompositionDoc::OnOpenRasterMap(const Map& mp, OpenType ot) 
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

	createBaseMapDrawer(mp, "RasterDataDrawer", "Ilwis38");	

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

BOOL MapCompositionDoc::OnOpenObjectCollection(const ObjectCollection& list, OpenType ot)
{
	BaseMap bmp(list->fnObject(0));
	if (!bmp.fValid())
		return FALSE;
	SetTitle(bmp);
	ComplexDrawer *drawer=0;
	if (ot == otANIMATION) {
		ILWIS::DrawerParameters parms(rootDrawer, rootDrawer);
		drawer = (ComplexDrawer *)NewDrawer::getDrawer("AnimationDrawer", "Ilwis38", &parms);

	}
	else if (ot == otCOLLECTION)  {
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
	drawer->getZMaker()->setDataSourceMap(bmp);
	addToPixelInfo(list, drawer);
	ILWIS::PreparationParameters pp(RootDrawer::ptGEOMETRY | RootDrawer::ptRENDER,bmp->cs());
	drawer->prepare(&pp);

	return TRUE;
}

BOOL MapCompositionDoc::OnOpenMapList(const MapList& maplist, OpenType ot)
{
	Map mp = maplist[maplist->iLower()];
	if (!mp.fValid())
		return FALSE;
	SetTitle(maplist);

	if (ot & otANIMATION) {
	ILWIS::DrawerParameters parms(rootDrawer, rootDrawer);
		ILWIS::NewDrawer *drawer = NewDrawer::getDrawer("AnimationDrawer", "Ilwis38", &parms);
		drawer->addDataSource((void *)&maplist);
		rootDrawer->setCoordinateSystem(mp->cs());
		rootDrawer->addCoordBounds(mp->cs(), mp->cb(), false);
		ILWIS::PreparationParameters pp(RootDrawer::ptGEOMETRY | RootDrawer::ptRENDER,0);
		addToPixelInfo(maplist, (ComplexDrawer *)drawer);
		drawer->prepare(&pp);
		rootDrawer->addDrawer(drawer);
	}
	else {
	//	eType = eColorComp;
	}

	
	return TRUE;
}

BOOL MapCompositionDoc::OnOpenSegmentMap(const SegmentMap& sm, OpenType ot) 
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

	createBaseMapDrawer(sm,"FeatureDataDrawer", "Ilwis38");
	
	if (ot & otEDIT) {
		::AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_EDITLAYER, 0);
	}

	return TRUE;
}

BOOL MapCompositionDoc::OnOpenPolygonMap(const PolygonMap& pm, OpenType ot) 
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

	createBaseMapDrawer(pm,"FeatureDataDrawer", "Ilwis38");
	
	if (ot & otEDIT) {
		::AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_EDITLAYER, 0);
	}

	return TRUE;
}

BOOL MapCompositionDoc::OnOpenPointMap(const PointMap& pm, OpenType ot) 
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

	//================================================ TEST!!!!!!!

	createBaseMapDrawer(pm,"FeatureDataDrawer", "Ilwis38");
	//===============================================

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

BOOL MapCompositionDoc::OnOpenMapView(const MapView& mapview)
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
		rootDrawer->load(fn,"");
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
	AddLayerForm(CWnd* parent, String* sName, bool *asAnimation = 0)
		: FormWithDest(parent, TR("Add Data Layer")), asAnim(asAnimation), cb(0)
	{
		new FieldBlank(root);
		fdtl = new FieldDataTypeLarge(root, sName, ".mpr.mpl.mps.mpa.mpp.atx");
		//    new FieldSegmentMap(root, SDUiSegMap, sName);
		if ( asAnimation != 0) {
			cb = new CheckBox(root,TR("As animation layer"),asAnimation);
			cb->SetCallBack((NotifyProc)&AddLayerForm::changeFilter);
		}
		SetMenHelpTopic("ilwismen\\add_layer_to_map_window.htm");
		create();
	}
private:
	bool * asAnim;
	CheckBox *cb;
	FieldDataTypeLarge * fdtl;
	int changeFilter(Event *ev) {
		if ( cb)
			cb->StoreData();

		if ( *asAnim) {
			fdtl->SetExt(".mpl.ioc");
		} else {
			fdtl->SetExt(".ioc.mpr.mpl.mps.mpa.mpp.atx");
		}
		return 1;
	}
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

NewDrawer* MapCompositionDoc::drAppend(const FileName& fn, IlwisDocument::OpenType op)
{
	if (!fAppendable(fn))
	{
		String sErr(TR("%S cannot be added as a layer").c_str(), fn.sShortName());
		AfxGetMainWnd()->MessageBox(sErr.c_str(), TR("Add data layer").c_str(), MB_OK|MB_ICONEXCLAMATION);
		return 0;

		return 0;
	}    
	NewDrawer* dr = 0;
	// add layer
	if (".mps" == fn.sExt || ".mpa" == fn.sExt || ".mpp" == fn.sExt) {
		BaseMap bm(fn);
		dr = drAppend(bm);
	}
	else if (".mpr" == fn.sExt) {
		Map mp(fn);
		dr = drAppend(mp);
	}
	else if (".mpl" == fn.sExt) {
		MapList ml(fn);
		dr = drAppend(ml,op);
	}
	else if (".ioc" == fn.sExt) {
		ObjectCollection oc(fn);
		dr = drAppend(oc,op);
	}
	else if (".csy" == fn.sExt) {
		CoordSystem csy(fn);
		SetCoordSystem(csy);
	}
	UpdateAllViews(0);
	return dr;
}

void MapCompositionDoc::OnAddLayer()
{
	String sName;
	bool asAnimation = false;
	AddLayerForm frm(wndGetActiveView(), &sName, &asAnimation);
	bool fOk = frm.fOkClicked();
	if (fOk) {
		FileName fn(sName);
		if ( IOTYPE(fn) == IlwisObject::iotOBJECTCOLLECTION)
			drAppend(fn, asAnimation ? IlwisDocument::otANIMATION : IlwisDocument::otCOLLECTION);
		else if ( IOTYPE(fn) == IlwisObject::iotMAPLIST) {
			drAppend(fn, asAnimation ? IlwisDocument::otANIMATION : IlwisDocument::otUNKNOWN);
		}
		else{
			drAppend(fn,IlwisDocument::otUNKNOWN);
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
	rootDrawer->removeDrawer(drw->getId());
	ChangeState();
}

NewDrawer* MapCompositionDoc::drAppend(const Map& rasmap)
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
	NewDrawer *dr = createBaseMapDrawer(rasmap,"RasterDataDrawer","Ilwis38");

	ChangeState();
	return dr;
}

NewDrawer* MapCompositionDoc::drAppend(const ObjectCollection& oc, IlwisDocument::OpenType op)
{
	ILWIS::NewDrawer *drawer = 0;
	if ( op == IlwisDocument::otANIMATION) {
		ILWIS::DrawerParameters parms(rootDrawer, rootDrawer);
		drawer = NewDrawer::getDrawer("AnimationDrawer", "Ilwis38", &parms);
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
	} else if( op == IlwisDocument::otCOLLECTION) {
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

NewDrawer* MapCompositionDoc::drAppend(const MapList& maplist,IlwisDocument::OpenType op)
{
	if ( op == IlwisDocument::otANIMATION) {
		ILWIS::DrawerParameters parms(rootDrawer, rootDrawer);
		ILWIS::NewDrawer *drawer = NewDrawer::getDrawer("AnimationDrawer", "Ilwis38", &parms);
		drawer->addDataSource((void *)&maplist);
		Map mp = maplist[maplist->iLower()];
		rootDrawer->setCoordinateSystem(mp->cs());
		rootDrawer->addCoordBounds(mp->cs(), mp->cb(), false);
		ILWIS::PreparationParameters pp(RootDrawer::ptGEOMETRY | RootDrawer::ptRENDER,0);
		drawer->prepare(&pp);
		rootDrawer->addDrawer(drawer);
		ChangeState();
		UpdateAllViews(0,3);
		mpvGetView()->Invalidate();
		return drawer;
	} else {
		for(int i = 0; i < maplist->iSize(); ++i) {
			drAppend(maplist[i]->fnObj);
		}
	}
	return 0;
}


NewDrawer* MapCompositionDoc::drAppend(const BaseMap& mp,IlwisDocument::OpenType op)
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
		if ( op == IlwisDocument::otANIMATION) {
			drawer = NewDrawer::getDrawer("AnimationDrawer", "Ilwis38", &parms);
		}
		else {
			if ( IlwisObject::iotObjectType(mp->fnObj) !=  IlwisObject::iotRASMAP)
				drawer = createBaseMapDrawer(mp, "FeatureDataDrawer", "Ilwis38");
			else
				drawer = createBaseMapDrawer(mp, "RasterDataDrawer", "Ilwis38");
		}
		ChangeState();
		UpdateAllViews(0,3);
		mpvGetView()->Invalidate();
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
	PreparationParameters pp(NewDrawer::ptGEOMETRY);
	rootDrawer->prepare(&pp);

	ChangeState();
	UpdateAllViews(0,3);
}

void MapCompositionDoc::OnChangeCoordSystem()
{
	class CCSForm: public FormWithDest
	{
	public:
		CCSForm(CWnd* parent, String* sName)
			: FormWithDest(parent, TR("Change Coordinate System"))
		{
			new FieldDataTypeLarge(root, sName, ".csy");
			SetMenHelpTopic("ilwismen\\change_coordinate_system_of_a_map_window.htm");
			create();
		}
	};
	String sNam = rootDrawer->getCoordinateSystem()->fnObj.sShortName();
	CCSForm form(wndGetActiveView(), &sNam);
	if (form.fOkClicked()) {
		FileName fn(sNam);
		 if (fn.sExt == ".csy") {
			CoordSystem csy(fn);
			SetCoordSystem(csy);
		}
	}
}

void MapCompositionDoc::OnUpdateChangeCoordSystem(CCmdUI* pCmdUI)
{
	//pCmdUI->Enable(!fRaster);
}

void MapCompositionDoc::OnShowHistogram()
{
	if (gbHist) {
		BOOL fShown = gbHist->IsWindowVisible();
		delete gbHist;
		gbHist = 0;
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
		if (md->getType() == "ColorCompositeDrawer") 
		{
			throw ErrorObject("THis must be moved");  // HistogramRGBGraphView may not remain here
			HistogramRGBGraphView* hgv = new HistogramRGBGraphView(md);
			gbHist = new GeneralBar;
			gbHist->view = hgv;
			gbHist->Create(fw, (dynamic_cast<FrameWindow*>(fw))->iNewBarID());
			hgv->Create(gbHist);
			AddView(hgv);
			hgv->OnInitialUpdate();
			String sTitle(TR("Histogram of %S").c_str(), md->getName());
			gbHist->SetWindowText(sTitle.c_str());
			fw->FloatControlBar(gbHist,CPoint(100,100));
			fw->ShowControlBar(gbHist,TRUE,FALSE);
			return;
		}
		if (md->getType() == "RasterDataDrawer" ) // && md->dm()->pdv()) 
		{
			HistogramGraphDoc* hgd = new HistogramGraphDoc;
			Map mp;
			mp.SetPointer(md->getBaseMap());
			TableHistogramInfo thi(mp);
			hgd->OnOpenDocument(thi.tbl());
			HistogramGraphView* hgv = new HistogramGraphView;
			gbHist = new GeneralBar;
			gbHist->view = hgv;
			gbHist->Create(fw, (dynamic_cast<FrameWindow*>(fw))->iNewBarID());
			hgv->Create(gbHist);
			hgd->AddView(hgv);
			hgv->OnInitialUpdate();
			String sTitle(TR("Histogram of %S").c_str(), md->getName());
			gbHist->SetWindowText(sTitle.c_str());
			fw->FloatControlBar(gbHist,CPoint(100,100));
			fw->ShowControlBar(gbHist,TRUE,FALSE);
			return;
		}
	}
}

void MapCompositionDoc::OnUpdateShowHistogram(CCmdUI* pCmdUI)	
{
	BOOL fShown = false;
	if (gbHist) 
		fShown = gbHist->IsWindowVisible();
	bool fPossible = false;
	CWnd* wnd = wndGetActiveView();
	CFrameWnd* fw = wnd->GetTopLevelFrame();
	if (fw) {
		for (int i = 0; i < rootDrawer->getDrawerCount(); ++i) 
		{
			NewDrawer* dr = rootDrawer->getDrawer(i);
			SpatialDataDrawer* md = dynamic_cast<SpatialDataDrawer*>(dr);
			if (md && (md->getBaseMap()->dm()->pdv() || md->getBaseMap()->dm()->pdsrt())) 
			{
				fPossible = true;
				break;
			}
		}
	}
	pCmdUI->SetCheck(fShown);
	pCmdUI->Enable(fPossible||fShown);
}

BOOL MapCompositionDoc::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	if (fInCmdMsg)
		return FALSE;
	if (CatalogDocument::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;
	fInCmdMsg = true;
	BOOL fReturn = FALSE;
	if (gbHist && IsWindow(gbHist->m_hWnd) && gbHist->IsWindowVisible())
		fReturn = gbHist->view->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
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


