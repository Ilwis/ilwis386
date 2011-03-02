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
#include "Client\Mapwindow\Drawers\RootDrawer.h"
#include "Client\Mapwindow\Drawers\AbstractMapDrawer.h"
#include "Client\Mapwindow\PixelInfoDoc.h"
#include "Engine\Base\DataObjects\ObjectCollection.h"
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
#include "Client\Mapwindow\Drawers\DrawerContext.h"
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
#include "Client\Editors\Editor.h"
#include "Client\Mapwindow\Drawers\ZValueMaker.h"


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
	rootDrawer = new RootDrawer(this);
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
			mpv = MapView(en);
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
		AbstractMapDrawer *drw = dynamic_cast<AbstractMapDrawer *>(drawers.at(i));
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
		AbstractMapDrawer *drw = dynamic_cast<AbstractMapDrawer *>(drawers.at(i));
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
	if (!IlwisDocument::OnOpenDocument(fn.sRelative().scVal()))
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
			SetPathName(fn.sFullName().scVal());
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
				String sErr(SMWErrGeoRefReadOnly_S.scVal(), grf->sName());
				MessageBox(0, sErr.scVal(), SMWErrError.scVal(), MB_OK|MB_ICONSTOP|MB_TOPMOST); 
				return FALSE;
			}
			GeoRefCTP* gc = grf->pgCTP();
			if (0 == gc)
				return FALSE;
			FileName fn = gc->fnBackgroundMap;
			if (!fn.fValid()) {
				MessageBox(0, SMWErrNoBackgroundMap.scVal(), SMWErrError.scVal(), MB_OK|MB_ICONSTOP|MB_TOPMOST); 
				return FALSE;
			}
			if (".mpl" == fn.sExt) {
				MapList ml(fn);
				if (!ml.fValid())
					return FALSE;
				if (ml->gr() != grf) {
					MessageBox(0, SMWErrIncorrectBackgroundMap.scVal(), SMWErrError.scVal(), MB_OK|MB_ICONSTOP|MB_TOPMOST); 
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
					MessageBox(0, SMWErrIncorrectBackgroundMap.scVal(), SMWErrError.scVal(), MB_OK|MB_ICONSTOP|MB_TOPMOST); 
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
		else if (".ioc" == fn.sExt && ot == IlwisDocument::otANIMATION) {
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
			if (!OnOpenDocument(s.scVal()))
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
	OnSaveView();
	if (!mpv.fValid())
		return;
	String sViewName = mpv->sName(true);
	if ("" == sViewName)
	{
		FileName fn = GetPathName(); // Convert to Ilwis fn to get quoted fullpath
		sViewName = fn.sFullPathQuoted().scVal();
	}
	else // still sViewName can contain a LFN
	{
		FileName fn (sViewName);
		sViewName = fn.sRelativeQuoted().scVal();
	}
	String sCmd("layout %S", sViewName);
	IlwWinApp()->Execute(sCmd);
}

void MapCompositionDoc::OnSaveView()
{
	fUseSerialize = true;
	if (!mpv.fValid())
		OnSaveViewAs();
	else 
		OnFileSave();
}

void MapCompositionDoc::OnSaveViewAs()
{
	fUseSerialize = true;
	class SaveViewForm: public FormWithDest 
	{
	public:
		SaveViewForm(CWnd* parent, String* sName, String* sTitle)
			: FormWithDest(parent, SMWTitleSaveView)
		{
			iImg = IlwWinApp()->iImage(".mpv");

			new FieldBlank(root);
			new FieldViewCreate(root, SMWUiViewName, sName);
			FieldString *fs = new FieldString(root, SMWUiViewTitle, sTitle);
			fs->SetWidth(120);
			//      SetHelpTopic(htpSaveViewForms);
			SetMenHelpTopic(htpSaveView);
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
			String sErr(SAFMsgAlreadyExistsOverwrite_S.scVal(), fn.sFullPath(true));
			if (IDYES != MessageBox(0, sErr.scVal(), SAFMsgAlreadyExists.scVal(), MB_YESNO|MB_ICONEXCLAMATION))
				return;
		}
		mpv = MapView(sViewName, true);
		mpv->sDescription = sTitle;
		sViewName = mpv->fnObj.sFullName();
		DoSave(sViewName.scVal());
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
			double* rTop, double* rBottom, double* rLeft, double* rRight)
			: FormWithDest(wParent, SMWTitleExtWnd)
		{
			ValueRange vrr(-99, 1000, 1);
			frTop = new FieldReal(root, SMWUiPrcTop, rTop, vrr);
			new FieldReal(root, SMWUiPrcBot, rBottom, vrr);
			new FieldReal(root, SMWUiPrcLft, rLeft  , vrr);
			new FieldReal(root, SMWUiPrcRgt, rRight , vrr);
			SetMenHelpTopic(htpDspExtend);
			create();
		}
		FormEntry* feDefaultFocus()
		{ 
			return frTop;
		}
	private:  
		FieldReal* frTop;
	};
	//double rTop, rBottom, rLeft, rRight;
	//rTop = (mmMapBounds.MinRow() - mmSize.MinRow()) * 100.0 / mmMapBounds.height();
	//rTop = floor(100 * rTop) / 100;
	//rBottom = (mmSize.MaxRow() - mmMapBounds.MaxRow()) * 100.0 / mmMapBounds.height();
	//rBottom = floor(100 * rBottom) / 100;
	//rLeft = (mmMapBounds.MinCol() - mmSize.MinCol()) * 100.0 / mmMapBounds.width();
	//rLeft = floor(100 * rLeft) / 100;
	//rRight = (mmSize.MaxCol() - mmMapBounds.MaxCol()) * 100.0 / mmMapBounds.width();
	//rRight = floor(100 * rRight) / 100;
	//ExtendForm frm(0,&rTop,&rBottom,&rLeft,&rRight);
	//if (frm.fOkClicked()) 
	//{
	//	MinMax mm;
	//	mm.MinRow() = mmMapBounds.MinRow() - rounding(rTop / 100.0 * mmMapBounds.height());
	//	mm.MaxRow() = mmMapBounds.MaxRow() + rounding(rBottom / 100.0 * mmMapBounds.height());
	//	mm.MinCol() = mmMapBounds.MinCol() - rounding(rLeft / 100.0 * mmMapBounds.width());
	//	mm.MaxCol() = mmMapBounds.MaxCol() + rounding(rRight / 100.0 * mmMapBounds.width());
	//	SetBounds(mm);
	//}
}

void MapCompositionDoc::OnExtCoord()
{
	class BoundsForm: public FormWithDest
	{
	public:
		BoundsForm(CWnd* parent, Coord* cMin, Coord* cMax)
			: FormWithDest(parent, SMWTitleWndBnd)
		{
			fcMin = new FieldCoord(root, SMWUiMinXY, cMin);
			new FieldCoord(root, SMWUiMaxXY, cMax);
			SetMenHelpTopic(htpDspBounds);
			create();
		}
		FormEntry* feDefaultFocus()
		{ 
			return fcMin;
		}
	private:  
		FieldCoord* fcMin;
	};
	//MinMax mm = mmBounds();
	//CoordBounds cb;
	//cb += georef->cConv(mm.rcMin);
	//cb += georef->cConv(mm.rcMax);
	//cb += georef->cConv(RowCol(mm.MinRow(), mm.MaxCol()));
	//cb += georef->cConv(RowCol(mm.MaxRow(), mm.MinCol()));
	//BoundsForm frm(0,&cb.cMin,&cb.cMax);
	//if (frm.fOkClicked()) 
	//{
	//	SetBounds(cb);
	//}
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
		AbstractMapDrawer *mapdrawer = dynamic_cast<AbstractMapDrawer *>(rootDrawer->getDrawer(i));
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
		sprintf(s, "&%i %s", i+1, str.scVal());
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
		BaseMapEditor* edit = mpv->editGet();
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
			AbstractMapDrawer *mapdrawer = dynamic_cast<AbstractMapDrawer *>(rootDrawer->getDrawer(i));
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
	ILWIS::NewDrawer *drawer = IlwWinApp()->getDrawer(type, subtype, &parms);
	drawer->addDataSource((void *)&bmp);
	rootDrawer->setCoordinateSystem(bmp->cs());
	rootDrawer->addCoordBounds(bmp->cs(), bmp->cb(), false);
	ILWIS::PreparationParameters pp(RootDrawer::ptGEOMETRY | RootDrawer::ptRENDER,0);
	drawer->prepare(&pp);
	rootDrawer->addDrawer(drawer);
	addToPixelInfo(bmp, drawer);
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

	createBaseMapDrawer(mp, "RasterLayerDrawer", "Ilwis38");	

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

	if (ot & otANIMATION) {
		ILWIS::DrawerParameters parms(rootDrawer, rootDrawer);
		ComplexDrawer *drawer = (ComplexDrawer *)IlwWinApp()->getDrawer("AnimationDrawer", "Ilwis38", &parms);
		drawer->addDataSource((void *)&list);
		rootDrawer->setCoordinateSystem(bmp->cs());
		rootDrawer->addCoordBounds(bmp->cs(), bmp->cb(), false);
		drawer->getZMaker()->setSpatialSource(bmp, rootDrawer->getMapCoordBounds());
		drawer->getZMaker()->setDataSourceMap(bmp);
		ILWIS::PreparationParameters pp(RootDrawer::ptGEOMETRY | RootDrawer::ptRENDER,bmp->cs());
		drawer->prepare(&pp);
		rootDrawer->addDrawer(drawer);
	}
	else {
	//	eType = eColorComp;
	}

	
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
		ILWIS::NewDrawer *drawer = IlwWinApp()->getDrawer("AnimationDrawer", "Ilwis38", &parms);
		drawer->addDataSource((void *)&maplist);
		rootDrawer->setCoordinateSystem(mp->cs());
		rootDrawer->addCoordBounds(mp->cs(), mp->cb(), false);
		ILWIS::PreparationParameters pp(RootDrawer::ptGEOMETRY | RootDrawer::ptRENDER,0);
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

	//================================================ TEST!!!!!!!
	createBaseMapDrawer(sm,"FeatureLayerDrawer", "Ilwis38");
	//===============================================
	
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

	createBaseMapDrawer(pm,"FeatureLayerDrawer", "Ilwis38");
	
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

	createBaseMapDrawer(pm,"FeatureLayerDrawer", "Ilwis38");
	//===============================================

	if (ot & otEDIT) {
		::AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_EDITLAYER, 0);
	}
	return TRUE;
}

void MapCompositionDoc::addToPixelInfo(const BaseMap& bm, NewDrawer *drw) {
	if (!pixInfoDoc) 
		pixInfoDoc = new PixelInfoDoc();

	pixInfoDoc->OnOpenDocument(bm->fnObj.sFullPath().scVal(), drw);
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
		: FormWithDest(parent, SMWTitleAddLayer), asAnim(asAnimation)
	{
		new FieldBlank(root);
		fdtl = new FieldDataTypeLarge(root, sName, ".mpr.mpl.mps.mpa.mpp.atx");
		//    new FieldSegmentMap(root, SDUiSegMap, sName);
		if ( asAnimation != 0) {
			CheckBox *cb = new CheckBox(root,SMWUiAnimationLayer,asAnimation);
			cb->SetCallBack((NotifyProc)&AddLayerForm::changeFilter);
		}
		SetMenHelpTopic(htpAddLayer);
		create();
	}
private:
	bool * asAnim;
	FieldDataTypeLarge * fdtl;
	int changeFilter(Event *ev) {
		if ( *asAnim) {
			fdtl->SetExt(".mpl.mpr.mps.mpa.ioc.mpp");
		} else {
			fdtl->SetExt(".mpr.mpl.mps.mpa.mpp.atx");
		}
		return 1;
	}
};

class AddRasForm: public FormWithDest
{
public:
	AddRasForm(CWnd* parent, String* sName)
		: FormWithDest(parent, SMWTitleAddSegMap)
	{
		new FieldBlank(root);
		new FieldDataTypeLarge(root, sName, ".mpr");
		//    new FieldSegmentMap(root, SDUiSegMap, sName);
		SetMenHelpTopic(htpAddRasMap);
		create();
	}
};

class AddSegForm: public FormWithDest
{
public:
	AddSegForm(CWnd* parent, String* sName)
		: FormWithDest(parent, SMWTitleAddSegMap)
	{
		new FieldBlank(root);
		new FieldDataTypeLarge(root, sName, ".mps");
		//    new FieldSegmentMap(root, SDUiSegMap, sName);
		SetMenHelpTopic(htpAddSegMap);
		create();
	}
};

class AddPolForm: public FormWithDest
{
public:
	AddPolForm(CWnd* parent, String* sName)
		: FormWithDest(parent, SMWTitleAddPolMap)
	{
		new FieldBlank(root);
		new FieldDataTypeLarge(root, sName, ".mpa");
		//    new FieldSegmentMap(root, SDUiSegMap, sName);
		SetMenHelpTopic(htpAddPolMap);
		create();
	}
};

class AddPntForm: public FormWithDest
{
public:
	AddPntForm(CWnd* parent, String* sName)
		: FormWithDest(parent, SMWTitleAddPntMap)
	{
		new FieldBlank(root);
		new FieldDataTypeLarge(root, sName, ".mpp");
		//    new FieldSegmentMap(root, SDUiSegMap, sName);
		SetMenHelpTopic(htpAddPntMap);
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
		Map mp(fn);
		fOk = fGeoRefOk(mp);
	}
	else if (fn.sExt == ".mpl") 
	{
		MapList mpl(fn);
		fOk = fGeoRefOk(mpl[mpl->iLower()]);
	}
	else if (fn.sExt == ".grf" || fn.sExt == ".csy") {
		fOk = true; 
		// melding op status regel?
	}
	return fOk;
}

NewDrawer* MapCompositionDoc::drAppend(const FileName& fn, bool asAnimation)
{
	if (!fAppendable(fn))
	{
		String sErr(SMWErrCannotBeAdded_S.scVal(), fn.sShortName());
		AfxGetMainWnd()->MessageBox(sErr.scVal(), SMWUiAddDataLayer.sVal(), MB_OK|MB_ICONEXCLAMATION);
		return 0;

		return 0;
	}    
	NewDrawer* dr = 0;
	// add layer
	if (".mps" == fn.sExt || ".mpa" == fn.sExt || ".mpp" == fn.sExt) {
		BaseMap bm(fn);
		dr = drAppend(bm,asAnimation);
	}
	else if (".mpr" == fn.sExt) {
		Map mp(fn);
		dr = drAppend(mp,asAnimation);
	}
	else if (".mpl" == fn.sExt) {
		MapList ml(fn);
		dr = drAppend(ml,asAnimation);
	}
	else if (".csy" == fn.sExt) {
		CoordSystem csy(fn);
		SetCoordSystem(csy);
	}

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
		drAppend(fn, asAnimation);
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

NewDrawer* MapCompositionDoc::drAppend(const Map& rasmap, bool asAnimation)
{
	if (!fGeoRefOk(rasmap))
	{
		AfxGetMainWnd()->MessageBox(SMWErrNotSameGeoRef.sVal(), SMWUiAddDataLayer.sVal(), MB_OK|MB_ICONEXCLAMATION);
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
	NewDrawer *dr = createBaseMapDrawer(rasmap,"RasterLayerDrawer","Ilwis38");

	ChangeState();
	return dr;
}

NewDrawer* MapCompositionDoc::drAppend(const MapList& maplist, bool asAnimation)
{
	int iOption = 1;
	switch (iOption) {
	case 0:
		//dr = new MapListColorCompDrawer(this, maplist);
		break;
	case 1: 
		{
			ILWIS::DrawerParameters parms(rootDrawer, rootDrawer);
			ILWIS::NewDrawer *drawer = IlwWinApp()->getDrawer("AnimationDrawer", "Ilwis38", &parms);
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
		}
	}
	return 0;
}


NewDrawer* MapCompositionDoc::drAppend(const BaseMap& mp, bool asAnimation)
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
		if ( asAnimation) {
			drawer = IlwWinApp()->getDrawer("AnimationDrawer", "Ilwis38", &parms);
		}
		else {
			if ( IlwisObject::iotObjectType(mp->fnObj) !=  IlwisObject::iotRASMAP)
				drawer = createBaseMapDrawer(mp, "FeatureLayerDrawer", "Ilwis38");
			else
				drawer = createBaseMapDrawer(mp, "RasterLayerDrawer", "Ilwis38");
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
		: FormWithDest(parent, SMWTitleAddAnnText)
		//, mw(parent)
	{
		new FieldBlank(root);
		new FieldDataTypeLarge(root, sName, ".ATX");
		//    new PushButton(root, SMWUiCreate, (NotifyProc)&AddAnnTextForm::CreateAtx);
		SetMenHelpTopic(htpAddAnnText);
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
: FormWithDest(parent, SMWTitleAddBitmapOrPicture)
{
new FieldDataTypeLarge(root, sName, ".bmp.wmf.emf");
new CheckBox(root, SMWUiIsotropic, fIsotropic);
SetMenHelpTopic(htpAddBitmapPicture);
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
		String s(SMWErrCSUnusable);
		wndGetActiveView()->MessageBox(s.scVal(), SMWMsgRplCsy.scVal(), MB_OK|MB_ICONEXCLAMATION);
		return; 
	}
	rootDrawer->setCoordinateSystem(cs, true);
	PreparationParameters pp(NewDrawer::ptGEOMETRY);
	rootDrawer->prepare(&pp);
	//	String s(SMWErrCSUnusable);
	//	wndGetActiveView()->MessageBox(s.scVal(), SMWMsgRplCsy.scVal(), MB_OK|MB_ICONEXCLAMATION);
	//	return;  O
	//if (!cs.fValid() || fRaster) 
	//	return;
	//if (georef->cs() == cs) 
	//	return;		// nothing to be done
	//if (!cs->fConvertFrom(georef->cs())) {
	//	String s(SMWErrCSUnusable);
	//	wndGetActiveView()->MessageBox(s.scVal(), SMWMsgRplCsy.scVal(), MB_OK|MB_ICONEXCLAMATION);
	//	return;  O	//}  
	//LatLon llMin, llMax;
	//double rGrid;
	//Drawer::CalcBounds(georef, mmBounds(), llMin, llMax);
	//rGrid = llMax.Lat - llMin.Lat;
	//rGrid /= 10;
	//llMin.Lat -= rGrid;
	//llMax.Lat += rGrid;
	//rGrid = llMax.Lon - llMin.Lon;
	//rGrid /= 10;
	//llMin.Lon -= rGrid;
	//llMax.Lon += rGrid;
	//CoordBounds cb;
	//Drawer::CalcBounds(cs, llMin, llMax, cb);
	//rGrid = max(cb.width(), cb.height());
	//rGrid /= 10;
	//cb.MinX() -= rGrid;
	//cb.MaxX() += rGrid;
	//cb.MinY() -= rGrid;
	//cb.MaxY() += rGrid;
	//rGrid = max(cb.width(), cb.height());
	//long iSize = 0x4000; //32767L;
	//rGrid /= iSize;
	//long iXSize = (long)(cb.width()  / rGrid);
	//long iYSize = (long)(cb.height() / rGrid);
	//float a11,a12,a21,a22,b1,b2;
	//a11 = (float)(1.0 / rGrid);
	//a21 = 0;
	//a12 = 0;
	//a22 = (float)(-1.0/ rGrid);
	//b1  = (float)(- cb.MinX() / rGrid);
	//b2  = (float)(cb.MaxY() / rGrid);
	//georef = GeoRef(cs,RowCol(iYSize,iXSize),a11,a12,a21,a22,b1,b2);
	//MinMax mm;
	//mm.rcMin = RowCol(0L,0L);
	//mm.rcMax = georef->rcSize();
	//mmSize = mmMapBounds = mm;
	ChangeState();
	UpdateAllViews(0,3);
}

void MapCompositionDoc::OnChangeCoordSystem()
{
	class CCSForm: public FormWithDest
	{
	public:
		CCSForm(CWnd* parent, String* sName)
			: FormWithDest(parent, SMWTitleChangeCoordSystem)
		{
			new FieldDataTypeLarge(root, sName, ".csy");
			SetMenHelpTopic(htpDspChangeCS);
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
		AbstractMapDrawer* md = dynamic_cast<AbstractMapDrawer*>(dr);
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
			String sTitle(SMWTitleHistogramOf_S.scVal(), md->getName());
			gbHist->SetWindowText(sTitle.scVal());
			fw->FloatControlBar(gbHist,CPoint(100,100));
			fw->ShowControlBar(gbHist,TRUE,FALSE);
			return;
		}
		if (md->getType() == "RasterLayerDrawer" ) // && md->dm()->pdv()) 
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
			String sTitle(SMWTitleHistogramOf_S.scVal(), md->getName());
			gbHist->SetWindowText(sTitle.scVal());
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
			AbstractMapDrawer* md = dynamic_cast<AbstractMapDrawer*>(dr);
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

void MapCompositionDoc::OnOpen() 
{
	class ShowBaseMapForm: public FormWithDest
	{
	public:
		ShowBaseMapForm(CWnd* parent, String* sName)
			: FormWithDest(parent, SMSTitleShowMap)
		{
			new FieldDataObject(root, sName);
			SetMenHelpTopic(htpOpenBaseMap);
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
			OnOpenDocument(sMap.scVal());
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
	String sMsg(SMWMsgSaveChangesMapView_s.scVal(), (const char*)sTitle);
	UINT nType = MB_ICONEXCLAMATION|MB_TOPMOST;
	if (fAllowCancel)
		nType |= MB_YESNOCANCEL;
	else
		nType |= MB_YESNO;
	switch (wndGetActiveView()->MessageBox(sMsg.scVal(), SMWMsgSaveChanges.scVal(), nType))
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
		AbstractMapDrawer* md = dynamic_cast<AbstractMapDrawer*>(dr);
		sList &= " ";
		sList &= md->getBaseMap()->fnObj.sFullNameQuoted();
	}
	IlwWinApp()->OpenPixelInfo(sList.scVal());
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


