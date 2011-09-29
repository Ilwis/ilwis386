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
// GeoRefEditor.cpp: implementation of the GeoRefEditor class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Engine\Map\Segment\Seg.h"
#include "Engine\Map\Polygon\POL.H"
#include "Client\Mapwindow\Positioner.h"
#include "Client\Editors\Editor.h"
#include "Client\Editors\Utils\SYMBOL.H"
#include "Client\ilwis.h"
#include "Client\Base\datawind.h"
#include "Client\Editors\Utils\BaseBar.h"
#include "Client\MainWindow\CommandCombo.h"
#include "Headers\constant.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\TableWindow\TableDoc.h"
#include "Client\TableWindow\BaseTablePaneView.h"
#include "Client\TableWindow\TablePaneView.h"
#include "Engine\Map\Segment\Seg.h"
#include "Engine\Map\Polygon\POL.H"
#include "Client\Editors\Utils\SYMBOL.H"
#include "Client\Editors\Utils\sizecbar.h"
#include "Client\Editors\Georef\GeoRefEditorTableView.h"
#include "Client\Editors\Georef\GeoRefEditorTableBar.h"
#include "Client\Editors\Georef\TransformationComboBox.h"
#include "Client\Editors\Georef\TiePointEditor.h"
#include "Client\Editors\Georef\GeoRefEditor.h"
#include "Headers\Hs\Georef.hs"
#include "Engine\SpatialReference\grdirlin.h"
#include "Engine\SpatialReference\Grortho.h"
#include "Engine\SpatialReference\GrParallProj.h"
#include "Engine\SpatialReference\Grctppla.h"
#include "Headers\constant.h"
#include "Client\FormElements\fldcolor.h"
#include "Client\Mapwindow\MapWindow.h"
#include "Client\FormElements\fentdms.h"
#include "Headers\Htp\Ilwis.htp"
#include "Client\Base\IlwisDocument.h"
#include "Client\MainWindow\Catalog\CatalogDocument.h"
#include "Engine\Map\Mapview.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Editors\Georef\EditFiducialMarksForm.h"
#include "Client\Editors\Digitizer\DIGITIZR.H"
#include "Engine\Drawers\SimpleDrawer.h"
#include "Engine\Drawers\TextDrawer.h"
#include "Engine\Drawers\OpenGLText.h"


using namespace ILWIS;



BEGIN_MESSAGE_MAP(GeoRefEditor, TiePointEditor)
	//{{AFX_MSG_MAP(GeoRefEditor)
	ON_COMMAND(ID_EDITGRFADDPNT, OnAddPoint)
	ON_COMMAND(ID_EDITGRFDELPNT, OnDelPoint)
	ON_COMMAND(ID_EDITGRFTRANSF, OnTransformation)
	ON_COMMAND(ID_CONFIGURE, OnConfigure)
	ON_COMMAND(ID_EDITGRFFIDMARKS, OnEditFiducialMarks)
	ON_COMMAND(ID_EDITGRFSTOPFIDMARKS, OnStopFiducialMarks)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#define sMen(ID) ILWSF("men",ID).c_str()
#define addmen(ID) men.AppendMenu(MF_STRING, ID, sMen(ID)); 

GeoRefEditor::GeoRefEditor(MapPaneView* mpvw, GeoRef georef)
: TiePointEditor(mpvw), atpf(0), efmf(0), grf(georef), fInCalc(false), rSigma(0)
{
	grc = georef->pgCTP();
	if (0 == grc) {
		mpv->MessageBox(TR("GeoReference not editable").c_str(),TR("GeoReference Editor").c_str(),MB_OK|MB_ICONSTOP);
		fOk = false;
		return;
	}
	else
		fLatLon = ( 0 != grc->cs()->pcsLatLon());

	MapCompositionDoc* mcd = mpv->GetDocument();
	for(int i = 0; i < mcd->rootDrawer->getDrawerCount(); ++i)
	{
		SpatialDataDrawer* dr = CMAPDRW(mcd->rootDrawer, i);
		FileName fn = dr->getName();
		if (fn.sExt == ".mpr" || fn.sExt == ".mpl") {
			grc->fnBackgroundMap = fn;
			break;
		}  
	}

	SpatialDataDrawer *absndrw = CMAPDRW(mcd->rootDrawer, 0);;
	MapPtr *mptr = (MapPtr *)(absndrw->getBaseMap());
	mptr->SetGeoRef(GeoRef(FileName("none.grf")));
	mptr->DoNotStore(true);
	mcd->rootDrawer->setCoordinateSystem(CoordSystem());
	CoordBounds cbNone (Coord(0,0), Coord(mptr->rcSize().Col, -mptr->rcSize().Row)); // none.grf bounds
	mcd->rootDrawer->setCoordBoundsMap(cbNone);
	mcd->rootDrawer->setCoordBoundsView(cbNone, true);

	GeoRefDirectLinear* grdl = grc->pgDirectLinear();
	if (grdl) 
		mapDTM = grdl->mapDTM; 
	GeoRefOrthoPhoto* grop = grc->pgOrthoPhoto();
	if (grop)
		mapDTM = grop->mapDTM;
	GeoRefParallelProjective* grpp = grc->pgParallelProjective();
	if (grpp)
		mapDTM = grpp->mapDTM;

	if (mapDTM.fValid()) {
		DomainValueRangeStruct dvrsZ(-99999,99999,0.01);
		colZdem = grc->tbl()->col("Z_dtm");
		if (!colZdem.fValid())
			colZdem = grc->tbl()->colNew("Z_dtm", dvrsZ);
		colZdem->sDescription = "Terrain Elevation (DTM height)";
		colZdem->SetReadOnly();
		colZdem->SetOwnedByTable(true);
	}
	DomainValueRangeStruct dvrsRes(-9999,9999,0.01);
	colDRow = grc->tbl()->colNew("DRow", dvrsRes);
	colDCol = grc->tbl()->colNew("DCol", dvrsRes);
	colDRow->SetReadOnly();
	colDRow->SetOwnedByTable(true);
	colDCol->SetReadOnly();
	colDCol->SetOwnedByTable(true);
	if (grdl || grpp) {
		colZdiff = grc->tbl()->colNew("Z_diff", dvrsRes);
		colZdiff->SetReadOnly();
		colZdiff->SetOwnedByTable(true);
		colZdiff->sDescription = "Vertical Distance from Trend Plane";
	}
	CMenu men;
	men.CreateMenu();
	addmen(ID_FILE_PRINT);
	addmen(ID_CONFIGURE);
	men.AppendMenu(MF_SEPARATOR);
	addmen(ID_EXITEDITOR);
	hmenFile = men.GetSafeHmenu();
	men.Detach();

	men.CreateMenu();
	addmen(ID_CUT  );
	addmen(ID_COPY );
	addmen(ID_PASTE);
	addmen(ID_CLEAR);
	men.AppendMenu(MF_SEPARATOR);
	addmen(ID_EDITGRFADDPNT);
	addmen(ID_EDITGRFDELPNT);
	if (grc->pgCTPplanar())
		addmen(ID_EDITGRFTRANSF)
	else if (grc->pgOrthoPhoto())
	addmen(ID_EDITGRFFIDMARKS);
	hmenEdit = men.GetSafeHmenu();
	men.Detach();
	UpdateMenu();

	MapWindow* mw = mpv->mwParent();
	DataWindow* dw = mpv->dwParent();
	if (0 == dw) {
		fOk = false;
		return;
	}

	gretBar.Create(mpv->GetParentFrame(), this);
	grtd = new TableDoc;
	grtd->OnOpenDocument(grc->tbl());
	grtd->AddView(gretBar.grtv);
	gretBar.grtv->OnInitialUpdate();
	dw->DockControlBar(&gretBar,AFX_IDW_DOCKBAR_BOTTOM);

	CWindowDC dc(CWnd::GetDesktopWindow());
	TEXTMETRIC tm;
	dc.GetTextMetrics(&tm);
	int iHeight = tm.tmHeight * 8;
	int iWidth = tm.tmAveCharWidth * 20;
	if (grdl != 0 || grop != 0 || grpp != 0) //extra text for georef dirlin, orthoph and parall
		iWidth *= 5;
	CRect rect(0,0,iWidth,iHeight);

	ButtonBar& bb = dw->bbDataWindow;
	bb.LoadButtons("grfedit.but");
	CToolBarCtrl& tbc = bb.GetToolBarCtrl();
	TBBUTTONINFO bi;
	if (tbc.GetButtonInfo(ID_EDITGRFTRANSF, &bi)) 
	{
		int id = bb.CommandToIndex(ID_EDITGRFTRANSF); 
		if (grc->pgCTPplanar()) {
			TBBUTTON tb;
			tbc.GetButton(id,&tb);
			tb.iBitmap = iWidth;
			tb.idCommand = 0;
			tb.fsStyle = TBBS_SEPARATOR;
			id += 1;
			tbc.InsertButton(id, &tb);
			CRect rect;
			bb.GetItemRect(id, &rect);
			rect.top = -1;
			rect.bottom = rect.top + iHeight;
			rect.right += 2;
			cbTransf.Create(&bb,rect,this,grc->transf);
			cbTransf.SetFont(&fnt);
		}
		else {
			tbc.DeleteButton(id);
		}
	}

	DWORD dwCtrlStyle = TBSTYLE_FLAT | TBSTYLE_TRANSPARENT | TBSTYLE_TOOLTIPS;
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_TOOLTIPS | CBRS_FLYBY;
	CRect rectBB;
	rectBB.SetRect(2,1,2,1);
	bbTxt.CreateEx(mpv->GetParentFrame(), dwCtrlStyle, dwStyle, rectBB, 201);
	bbTxt.SetWindowText(TR("Quality Information").c_str());
	bbTxt.GetToolBarCtrl().SetImageList(&ilReBar);
	// without button the toolbar gets no height
	// the width of a button can not be changed
	// so we use both of them and correct the width of the separator
	UINT ai[2];
	ai[0] = ID_EDITGRFTRANSF;
	ai[1] = ID_SEPARATOR;
	bbTxt.SetButtons(ai,2);
	//	bbTxt.SetButtonInfo(0, 201,	TBBS_BUTTON, iWidth);
	iHeight = tm.tmHeight;
	bbTxt.GetItemRect(0, &rect);
	bbTxt.SetButtonInfo(1, 201,	TBBS_SEPARATOR, iWidth - rect.Width());
	rect.top = 2;
	rect.right = rect.left + iWidth;
	rect.bottom = rect.top + iHeight;
	//if (grdl != 0) // extra text space for georef dirlin
	//	rect.right += 3 * iWidth;
	//if (grop != 0) // extra text space for georef orthophoto
	//	rect.right += iWidth;
	edTxt.Create(WS_VISIBLE|WS_CHILD|ES_READONLY,rect,&bbTxt,201);
	edTxt.SetFont(&fnt);

	// hide scale bar
	if (mw)
		mw->ShowControlBar(&mw->barScale,FALSE,FALSE);

	dw->RecalcLayout();
	dw->bbDataWindow.GetWindowRect(&rect);
	bbTxt.EnableDocking(CBRS_ALIGN_TOP|CBRS_ALIGN_BOTTOM);
	bbTxt.SetBarStyle(bbTxt.GetBarStyle()|CBRS_GRIPPER|CBRS_BORDER_3D);
	rect.OffsetRect(1,0);
	dw->DockControlBar(&bbTxt,AFX_IDW_DOCKBAR_TOP,rect);

	dw->RecalcLayout();
	Calc();

	help = "ilwis\\georeference_tiepoints_editor.htm";
	sHelpKeywords = "Georeference tiepoints editor";

	if (grc->pgOrthoPhoto())
		dw->PostMessage(WM_COMMAND, ID_EDITGRFFIDMARKS, 0);
}

GeoRefEditor::~GeoRefEditor()
{
	grc->Store();
	MapCompositionDoc* mcd = mpv->GetDocument();
	SpatialDataDrawer *absndrw = CMAPDRW(mcd->rootDrawer, 0);;
	MapPtr *mptr = (MapPtr*)(absndrw->getBaseMap());
	mptr->SetGeoRef(grf);
	mptr->DoNotStore(false);
	mcd->rootDrawer->setCoordinateSystem(grf->cs());
	mcd->rootDrawer->setCoordBoundsMap(grf->cb());
	mcd->rootDrawer->setCoordBoundsView(grf->cb(), true);

	/*
	CReBar& rebar = mpv->mwParent()->rebar;
	CReBarCtrl& rbc = rebar.GetReBarCtrl();
	int iBand = rbc.IDToIndex(ID_EDITGRFTRANSF);
	rbc.DeleteBand(iBand);
	iBand = rbc.IDToIndex(1000);
	rbc.DeleteBand(iBand);
	*/
}

bool GeoRefEditor::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CMenu men, menSub;
	men.CreatePopupMenu();
	addmen(ID_NORMAL);
	addmen(ID_ZOOMIN);
	addmen(ID_ZOOMOUT);
	addmen(ID_PANAREA);
	men.AppendMenu(MF_SEPARATOR);
	addmen(ID_EDITGRFADDPNT);
	addmen(ID_EDITGRFDELPNT);
	if (grc->pgCTPplanar())
		addmen(ID_EDITGRFTRANSF)
	else if (grc->pgOrthoPhoto())
	addmen(ID_EDITGRFFIDMARKS);
	men.AppendMenu(MF_SEPARATOR);
	addmen(ID_CONFIGURE);
	addmen(ID_EXITEDITOR);
	menSub.CreateMenu();
	men.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, point.x, point.y, pWnd);
	return true;
}



IlwisObject GeoRefEditor::obj() const
{
	return grf;
}

zIcon GeoRefEditor::icon() const
{
	return zIcon("GrfIcon");
}

String GeoRefEditor::sTitle() const
{
	String s(TR("GeoReference Editor: %S").c_str(), grf->sName());
	return s;
}

int GeoRefEditor::draw(volatile bool* fDrawStop)
{
	RootDrawer * rootDrawer = mpv->GetDocument()->rootDrawer;
	ILWIS::DrawerParameters dpLayerDrawer (rootDrawer, 0);
	ILWIS::TextLayerDrawer *textLayerDrawer = (ILWIS::TextLayerDrawer *)NewDrawer::getDrawer("TextLayerDrawer", "ilwis38",&dpLayerDrawer);	
	ILWIS::DrawerParameters dpTextDrawer (rootDrawer, textLayerDrawer);
	ILWIS::TextDrawer *textDrawer = (ILWIS::TextDrawer *)NewDrawer::getDrawer("TextDrawer","ilwis38",&dpTextDrawer);
	OpenGLText * font = new OpenGLText (rootDrawer, "arial.ttf", 15, true, 1, -15);
	textLayerDrawer->setFont(font);
	for (long r = 1; r <= grc->iNr(); ++r) {
		Color clr;
		if (grc->fActive(r))
		{
			if (rSigma <= 0)
				clr = colActive;
			else {
				double rRow = colDRow->rValue(r);
				double rCol = colDCol->rValue(r);			
				double rErr = sqrt(rRow * rRow + rCol * rCol);
				if (rErr < 1.2 * rSigma)
					clr = colActGood;
				else if (rErr > 2 * rSigma)
					clr = colActBad;
				else
					clr = colActive;
			}
		}
		else
			clr = colPassive;
		zPoint pnt;
		if (!grc->fSubPixelPrecision)
		{
			RowCol rc = grc->rc(r);
			pnt = zPoint(rc.Col - 0.5, rc.Row - 0.5);
		}
		else
		{
			Coord crdRC = grc->crdRC(r);
			pnt = zPoint(crdRC.x-0.5,crdRC.y-0.5);
		}
		String s("%li", r);
		glColor4d(clr.redP(), clr.greenP(), clr.blueP(), 1);
		glPointSize(5.0);
		glBegin(GL_POINTS);
		glVertex3f(pnt.x, -pnt.y, 0);
		glEnd();
		textDrawer->addDataSource(&s);
		textDrawer->setCoord(Coordinate(pnt.x, -pnt.y, 0));
		font->setColor(clr);
		textDrawer->draw();
	}
	if (efmf) {
		efmf->draw();
		efmf->drawPrincPoint();
	}
	delete textLayerDrawer;

	return 0;
}

class AddTiePointForm: public FormWithDest
{
public: 
	AddTiePointForm(CWnd* wPar, GeoRefEditor* gre, RowCol rcDflt, Coord crdRCDflt, 
		Coord crdDflt, int iNr, bool fLatLong)  
		: FormWithDest(wPar, TR("Add Tie Point")),
		edit(gre), rc(rcDflt), crdRC(crdRCDflt), crd(crdDflt), frDTMheight(0)
	{
		String s(TR("Add Tiepoint number %i").c_str(), iNr);
		StaticText* st = new StaticText(root, s);
		st->SetIndependentPos();
		if (!gre->grc->fHasSubpixelPrecision())
			new FieldRowCol(root, TR("&Row, Col"), &rc);
		else
			new FieldCoord(root, TR("&Row, Col"), &crdRC);
		//edit->fLatLon = ( 0 != edit->grc->cs()->pcsLatLon());
		if (!fLatLong)
			fldCrd = new FieldCoord(root, TR("&X, Y"), &crd);
		else {
			ll = edit->grc->cs()->llConv(crd);
			fldLL = new FieldLatLon(root, TR("&Lat, Lon"), &ll);
		}
		fZ = false;
		rZ = rUNDEF;
		if (edit->mapDTM.fValid()) {
			CheckBox* cb = new CheckBox(root, TR("&Z"), &fZ);
			frDTMheight = new FieldReal(cb, "", &rZ);
		}
		SetMenHelpTopic("ilwismen\\georeference_tiepoints_editor_add_tiepoint.htm");
		edit->atpf = this;
		create();
	}
	~AddTiePointForm() {
		edit->atpf = 0;
	}
	void SetCoord(Coord c) {
		if (edit->fLatLon) {
			LatLon ll = edit->grc->cs()->llConv(c);
			fldLL->SetVal(ll);
		}
		else
			fldCrd->SetVal(c);
		if (frDTMheight) {
			double rZ = edit->mapDTM->rValue(c);
			frDTMheight->SetVal(rZ);
		}
	}  /*
	   void SetLatLon(const LatLon ll) {
	   fldLL->SetVal(ll);
	   Coord c = edit->grc->cs()->cConv(ll);
	   if (frDTMheight) {
	   double rZ = edit->mapDTM->rValue(c);
	   frDTMheight->SetVal(rZ);
	   }
	   }  */
	FormEntry* feDefaultFocus()
	{
		FormEntry* fe = FormWithDest::feDefaultFocus();
		if (fe)
			return fe;
		else {
			bool fLL = edit->fLatLon;
			if (!fLL)
				return fldCrd; 
			else
				return fldLL;
		}
	}
	int exec() {
		FormWithDest::exec();
		bool fLL = edit->fLatLon;
		if (fLL) {
			crd.x = ll.Lon;
			crd.y = ll.Lat;
		}
		if (!edit->grc->fHasSubpixelPrecision())
		{
			if (fZ)
				edit->EditFieldCoordOK(rc,crd,rZ);
			else
				edit->EditFieldCoordOK(rc,crd);
		}
		else
		{
			if (fZ)
				edit->EditFieldCoordOK(crdRC,crd,rZ);
			else
				edit->EditFieldCoordOK(crdRC,crd);
		}
		return 0;
	}
private:
	GeoRefEditor* edit;
	FieldCoord* fldCrd;
	FieldReal* frDTMheight;
	FieldLatLon* fldLL;
	RowCol rc;
	Coord crdRC;
	Coord crd;
	LatLon ll;
	bool fZ;
	double rZ;
};


void GeoRefEditor::OnAddPoint()
{
	RowCol rc; //(0L,0L);
	Coord crdRC;
	Coord crd; //(0,0);
	LatLon ll;
	int iNr = grc->iNr() + 1;
	//if (!grc->fSubPixelPrecision)
	//	AddTiePointForm frm(mpv, this, rc, crd, iNr, fLatLon);
	//else
	AddTiePointForm frm(mpv, this, rc, crdRC, crd, iNr, fLatLon);
}

void GeoRefEditor::EditFieldCoordOK(RowCol rc, Coord crd)
{
	int iPnt = grc->AddRec(rc, crd);
	drawPoint(iPnt);
	grtd->CheckNrRecs();
	Calc();
}

void GeoRefEditor::EditFieldCoordOK(RowCol rc, Coord crd, double rZ)
{
	int iPnt = grc->AddRec(rc, crd);
	Column colZ = grc->tbl()->col("Z");
	if (colZ.fValid())
		colZ->PutVal(iPnt, rZ);
	drawPoint(iPnt);
	grtd->CheckNrRecs();
	Calc();
}

void GeoRefEditor::EditFieldLatLonOK(RowCol rc, LatLon ll)
{
	int iPnt = grc->AddRec(rc, ll);
	drawPoint(iPnt);
	grtd->CheckNrRecs();
	Calc();
}

void GeoRefEditor::EditFieldLatLonOK(RowCol rc, LatLon ll, double rZ)
{
	int iPnt = grc->AddRec(rc, ll);
	Column colZ = grc->tbl()->col("Z");
	if (colZ.fValid())
		colZ->PutVal(iPnt, rZ);
	drawPoint(iPnt);
	grtd->CheckNrRecs();
	Calc();
}

void GeoRefEditor::EditFieldCoordOK(Coord crdRC, Coord crd)
{
	int iPnt = grc->AddRec(crdRC, crd);
	drawPoint(iPnt);
	grtd->CheckNrRecs();
	Calc();
}

void GeoRefEditor::EditFieldCoordOK(Coord crdRC, Coord crd, double rZ)
{
	int iPnt = grc->AddRec(crdRC, crd);
	Column colZ = grc->tbl()->col("Z");
	if (colZ.fValid())
		colZ->PutVal(iPnt, rZ);
	drawPoint(iPnt);
	grtd->CheckNrRecs();
	Calc();
}

void GeoRefEditor::EditFieldLatLonOK(Coord crdRC, LatLon ll)
{
	int iPnt = grc->AddRec(crdRC, ll);
	drawPoint(iPnt);
	grtd->CheckNrRecs();
	Calc();
}

void GeoRefEditor::EditFieldLatLonOK(Coord crdRC, LatLon ll, double rZ)
{
	int iPnt = grc->AddRec(crdRC, ll);
	Column colZ = grc->tbl()->col("Z");
	if (colZ.fValid())
		colZ->PutVal(iPnt, rZ);
	drawPoint(iPnt);
	grtd->CheckNrRecs();
	Calc();
}
zRect GeoRefEditor::rectPoint(long iNr)
{
	RowCol rc = grc->rc(iNr);
	Coord crdRC = grc->crdRC(iNr);
	zPoint pnt;
	if (!grc->fSubPixelPrecision)
		pnt = mpv->pntPos(Coord(rc.Row-0.5,rc.Col-0.5));
	else
		pnt = mpv->pntPos(Coord(crdRC.x-0.5,crdRC.y-0.5));
	zRect rect(pnt,pnt);
	rect.top()   -= smb.iSize / 2 + 1;
	rect.left()  -= smb.iSize / 2 + 1;
	rect.bottom()+= smb.iSize / 2 + 2;
	rect.right() += smb.iSize / 2 + 2;
	CClientDC cdc(mpv);
	zPoint pntText = smb.pntText(&cdc, pnt);
	CSize siz = cdc.GetTextExtent("123", 3);
	pntText.x += siz.cx + 1;
	pntText.y += siz.cy + 1;
	rect.bottom() = max(rect.bottom(), pntText.y);
	rect.right() = max(rect.right(), pntText.x);
	return rect;
}

int GeoRefEditor::drawPoint(long iNr)
{
	zRect rect = rectPoint(iNr);
	mpv->InvalidateRect(&rect);
	return 0;
}

void GeoRefEditor::Calc()
{
	if (0 == edTxt.m_hWnd)
		return;
	if (fInCalc)
		return;
	fInCalc = true;
	rSigma = 0;

	if (grc->tbl()->fChanged)
		grc->Updated();
	if (grc->fChanged)
		grc->Store();	 // to make Additional Info usable

	if (mapDTM.fValid())
		for (int i = 1; i <= grc->iNr(); ++i) {
			Coord crd = grc->crd(i);
			double rZ = mapDTM->rValue(crd);
			colZdem->PutVal(i, rZ);
		}
		int iRes = grc->Compute();
		if (iRes) {
			switch (iRes) {
				case -1:
				case -2:
					edTxt.SetWindowText(TR("Not enough points").c_str());
					break;
				case -3:
					edTxt.SetWindowText(TR("Singular Matrix").c_str());
					break;
				case -4:
					edTxt.SetWindowText("Incorrect Heights");
					break;
				case -5:
					edTxt.SetWindowText("No valid DTM");
					break;
				default:  
					edTxt.SetWindowText(TR("Error").c_str());
					break;
			}    
			for (int i = 1; i <= grc->iNr(); ++i) {
				colDRow->PutVal(i, rUNDEF);
				colDCol->PutVal(i, rUNDEF);
			}
		}  
		else {
			int iNr = 0;
			GeoRefDirectLinear* grdl = grc->pgDirectLinear();
			GeoRefOrthoPhoto* grop = grc->pgOrthoPhoto();
			GeoRefParallelProjective* grpp = grc->pgParallelProjective();

			for (int i = 1; i <= grc->iNr(); ++i) {
				double rRow, rCol;
				Coord crd = grc->crd(i);
				grc->Coord2RowCol(crd, rRow, rCol);
				if (!grc->fSubPixelPrecision)
				{
					RowCol rc = grc->rc(i);
					rRow -= rc.Row - 0.5;
					rCol -= rc.Col - 0.5;
				}
				else
				{
					Coord crdRC = grc->crdRC(i);
					rRow -= crdRC.x - 0.5;
					rCol -= crdRC.y - 0.5;
				}
				colDRow->PutVal(i, -rRow);
				colDCol->PutVal(i, -rCol);			
				if (grc->fActive(i)) {
					iNr += 1;
					rSigma += rRow * rRow + rCol * rCol;
					if (grdl != 0)
						colZdiff->PutVal(i, grdl->rVerticDist[i-1]);
					if (grpp != 0)
						colZdiff->PutVal(i, grpp->rVerticDist[i-1]);
				}
				else {
					if (grdl != 0 || grpp != 0)
						colZdiff->PutVal(i, rUNDEF);
				}
			}
			if (iNr >= grc->iMinNr()) {
				String s;
				if (iNr == grc->iMinNr()){
					rSigma = rUNDEF;
					s = "Sigma = ? ";
				}
				else {
					rSigma = sqrt(rSigma / (2 * (iNr - grc->iMinNr())));
					s = String(TR("Sigma = %6.3f pixels").c_str(), rSigma);
				}
				GeoRefCTPplanar* grcp = grc->pgCTPplanar();
				if (grcp != 0)
					grcp->SetSigma(rSigma);
				CVector cvPC;
				if (grdl != 0)
					cvPC = grdl->cvGetProjectionCenter();
				if (grop != 0)
					cvPC = grop->cvGetProjectionCenter();

				if (grdl != 0 || grop != 0) {
					double rNadX = cvPC(0);
					double rNadY = cvPC(1);
					Coord cNadir = Coord(rNadX, rNadY);
					double rPCz = cvPC(2);
					double rDTMz = mapDTM->rValue(cNadir);
					//double rFlyH = (double)(cvPC(2) - mapDTM->rValue(cNadir));
					double rFlyH = (double)(rPCz - rDTMz);
					s &= String("   Flying Height: %.0f m" , rFlyH);			
					s &= String("  Nadir: (%.0f, %.0f) " , rNadX, rNadY);
				}
				if (grdl != 0) {
					double rNonCopla = grdl->rGetNonCoplanarity();
					s &= String("    NC = %.1f m," , rNonCopla);
					double rZconf = grdl->rGetZConfidRange();
					s &= String("  Z-Conf = %.1f m" , rZconf);				
				}
				if (grpp != 0) {
					double rNonCopla = grpp->rGetNonCoplanarity();
					s &= String("    NC = %.1f m," , rNonCopla);
					double rZconf = grpp->rGetZConfidRange();
					s &= String("  Z-Conf = %.1f m" , rZconf);				
				}
				if (grop != 0) {				
					Coord cPP = grop->crdGetPrincipalPoint();
					s &= String("  Princ. Point: row %.1f, col %.1f " , cPP.x, cPP.y);
				}
				edTxt.SetWindowText(s.c_str());
			}
			else {
				edTxt.SetWindowText("");
				rSigma = 0;
			}
		}  
		for (int i = 1; i <= grc->iNr(); ++i) 
			drawPoint(i);
		grtd->UpdateAllViews(0);
		fInCalc = false;
}

void GeoRefEditor::OnTransfChanged()
{
	int iTransf = cbTransf.GetCurSel();
	grc->transf = static_cast<GeoRefCTP::Transf>(iTransf);
	Calc();
}

bool GeoRefEditor::OnLButtonDown(UINT nFlags, CPoint point)
{
	mpv->SetFocus();
	// RowCol rc = mpv->rcPos(point);
	Coord c = mpv->GetDocument()->rootDrawer->screenToWorld(RowCol(point.y,point.x));
	RowCol rc (-c.y, c.x);
	rc.Row += 1;
	rc.Col += 1;
	Coord crdRC;

	if (!grc->fSubPixelPrecision)
	{
		crdRC.x = rc.Row;
		crdRC.y = rc.Col;
	}
	else
	{
		// mpv->Pnt2RowCol(point, crdRC.x, crdRC.y);
		crdRC.x = -c.y;
		crdRC.y = c.x;

		crdRC.x += 0.5;
		crdRC.y += 0.5;
	}

	if (efmf)
	{
		efmf->OnLButtonDownRC(crdRC);
		return true;
	}
	if (iSelPnt) {
		drawPoint(iSelPnt);
		if (!grc->fSubPixelPrecision)
			grc->SetRowCol(iSelPnt, rc);
		else
			grc->SetcrdRC(iSelPnt, crdRC);
		drawPoint(iSelPnt);
		Calc();
		iSelPnt = 0;
		curActive = curAdd;
	}
	else {
		Coord crd;
		try {
			crd = grc->cConv(RowCol(crdRC.x,crdRC.y));
			crd = grc->cConv(rc);
			grc->RowCol2Coord(crdRC.x - 0.5, crdRC.y - 0.5, crd);
		}
		catch (...) {
		} 
		int i;
		if (!grc->fSubPixelPrecision)
			i = grc->iRec(rc);
		else
			i = grc->iRec(crdRC);
		if (i > 0)
			crd = grc->crd(i);
		else
			i = grc->iNr() + 1;
		AddTiePointForm frm(mpv, this, rc, crdRC, crd, i, fLatLon);
	}
	return true;
}

LRESULT GeoRefEditor::OnUpdate(WPARAM wParam, LPARAM lParam)
{
	if (0 == atpf)
		return 0;
	CoordMessage cm = (CoordMessage) wParam;
	if ( cm & cmMOUSECLICK || cm & cmDIGICLICK1)
	{
		CoordWithCoordSystem* c = (CoordWithCoordSystem*)(void*) lParam;
		Coord crd = grc->cs()->cConv((CoordSystem)*c, *c);
		atpf->SetCoord(crd);
		atpf->SetFocus();
	}
	return 0;
}

void GeoRefEditor::OnDelPoint()
{
	class DelTiePointForm: public FormWithDest
	{
	public:
		DelTiePointForm(CWnd* wPar, int* iNr, const RangeInt& ri)
			: FormWithDest(wPar, TR("Delete Tie Point"))
		{
			fi = new FieldInt(root, TR("&Tiepoint number"), iNr, ri);
			SetMenHelpTopic("ilwismen\\georeference_tiepoints_editor_delete_tiepoint.htm");
			create();
		}
	private:
		FormEntry* feDefaultFocus()
		{ return fi;
		}
		FieldInt* fi;  
	};  
	int iNr = grc->iNr();
	if (iNr <= 0)
		return;
	int iDel = iNr;  
	RangeInt ri(1, iNr);
	DelTiePointForm frm(mpv, &iDel, ri);
	if (frm.fOkClicked()) 
		DelPoints(iDel,1);
}

void GeoRefEditor::DelPoints(int iDel, int iRecs)
{
	CWaitCursor cur;
	if (iRecs <= 0)
		return;
	int iMax = grc->iNr();
	if (iMax <= 0)
		return;
	for (int i = iDel; i <= iMax; ++i)
		drawPoint(i);
	grc->tbl()->DeleteRec(iDel, iRecs);
	grtd->CheckNrRecs();
	Calc();
}

void GeoRefEditor::OnTransformation()
{
	class TransfForm: public FormWithDest
	{
	public:
		TransfForm(CWnd* wPar, int* transf)
			: FormWithDest(wPar, TR("Transformation"))
		{ // same sequence as in enum GeoRefCTP::Transf
			RadioGroup* rg = new RadioGroup(root, TR("Transformation:"), transf);
			new RadioButton(rg, TR("&Conformal"));
			new RadioButton(rg, TR("&Affine"));
			new RadioButton(rg, TR("&Second Order Bilinear"));
			new RadioButton(rg, TR("&Full Second Order"));
			new RadioButton(rg, TR("&Third Order"));
			new RadioButton(rg, TR("&Projective"));
			//    setHelpItem("ilwis\georeference_tiepoints_editor.htm");
			SetMenHelpTopic("ilwismen\\georeference_tiepoints_editor_transformation.htm");
			create();
		}
	};
	if (0 == grc->pgCTPplanar())
		return;
	int transf = grc->transf;  
	TransfForm frm(mpv, &transf);
	if (frm.fOkClicked()) {
		grc->transf = static_cast<GeoRefCTP::Transf>(transf); 
		grc->Updated();
		cbTransf.SetCurSel(grc->transf);
		Calc();
	}
}

class GreConfigForm: public FormWithDest
{
public:
	GreConfigForm(CWnd* parent, GeoRefEditor* gre)
		: FormWithDest(parent, TR("Customize GeoRefEditor"))
	{
		new FieldColor(root, TR("&Active Color"), &gre->colActGood);
		new FieldColor(root, TR("&Medium Error"), &gre->colActive);
		new FieldColor(root, TR("&Large Error"), &gre->colActBad);
		new FieldColor(root, TR("&Passive Color"), &gre->colPassive);
		if (0 != gre->efmf)
			new FieldColor(root, TR("&Fiducial Marks Color"), &gre->efmf->colFidMarks);
		new FieldInt(root, TR("Symbol &Size"), &gre->smb.iSize, ValueRange(1L,100L),true);
		SetMenHelpTopic("ilwismen\\georeference_tiepoints_editor_customize.htm");
		create();
	}
};  

void GeoRefEditor::OnConfigure()
{
	GreConfigForm frm(mpv, this);
	if (frm.fOkClicked()) {
		int iNr = grc->iNr();
		for (int i = 1; i <= iNr; ++i)
			drawPoint(i);

		IlwisSettings settings("Map Window\\TiePoint Editor");

		settings.SetValue("Active Color", colActive);
		settings.SetValue("Good Color", colActGood);
		settings.SetValue("Bad Color", colActBad);
		settings.SetValue("Passive Color", colPassive);
		settings.SetValue("Symbol Size", smb.iSize);
		if (efmf) {
			settings.SetValue("Passive Color", efmf->colFidMarks);
		}
	}  
}

void GeoRefEditor::OnEditFiducialMarks()
{
	if (0 == grc->pgOrthoPhoto())
		return;
	if (efmf) {
		efmf->SetFocus();  
		return;
	}  
	efmf = new EditFiducialMarksForm(mpv, grc->pgOrthoPhoto());
	efmf->create();
	DataWindow* dw = mpv->dwParent();
	dw->ShowControlBar(&gretBar,FALSE,FALSE);
}

void GeoRefEditor::OnStopFiducialMarks()
{
	efmf = 0;
	int iRes = grc->Compute();
	if (iRes < -10) { 
		OnExit();
		return;
	}
	DataWindow* dw = mpv->dwParent();
	dw->ShowControlBar(&gretBar,TRUE,FALSE);
	Calc();
}

