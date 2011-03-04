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
// CoordSysEditor.cpp: implementation of the CoordSysEditor class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\AbstractMapDrawer.h"
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
#include "Client\Editors\CoordSystem\CoordSysEditor.h"
#include "Headers\Hs\Coordsys.hs"
#include "Headers\Hs\Georef.hs"
#include "Engine\SpatialReference\CoordSystemTiePoints.H"
#include "Engine\SpatialReference\CoordSystemOrthoPhoto.h"
#include "Engine\SpatialReference\CoordSystemDirectLinear.h"
#include "Headers\constant.h"
#include "Client\FormElements\fldcolor.h"
#include "Client\FormElements\fentdms.h"
#include "Headers\Htp\Ilwis.htp"
#include "Client\Mapwindow\MapWindow.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\MainWindow\Catalog\CatalogDocument.h"
#include "Engine\Map\Mapview.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Editors\Georef\EditFiducialMarksForm.h"
#include "Client\Editors\Digitizer\DIGITIZR.H"

using namespace ILWIS;


BEGIN_MESSAGE_MAP(CoordSystemEditor, TiePointEditor)
	//{{AFX_MSG_MAP(CoordSystemEditor)
	ON_COMMAND(ID_EDITGRFADDPNT, OnAddPoint)
	ON_COMMAND(ID_EDITGRFDELPNT, OnDelPoint)
	ON_COMMAND(ID_EDITGRFTRANSF, OnTransformation)
	ON_COMMAND(ID_CONFIGURE, OnConfigure)
	ON_COMMAND(ID_EDITGRFFIDMARKS, OnEditFiducialMarks)
	ON_COMMAND(ID_EDITGRFSTOPFIDMARKS, OnStopFiducialMarks)
	ON_COMMAND(ID_EDITCSYBOUNDS, OnBoundaries)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#define sMen(ID) ILWSF("men",ID).scVal()
#define addmen(ID) men.AppendMenu(MF_STRING, ID, sMen(ID)); 


CoordSystemEditor::CoordSystemEditor(MapPaneView* mpvw, CoordSystem csy)
: TiePointEditor(mpvw), atpf(0), efmf(0), cs(csy), fInCalc(false), rSigma(0)
{
	csctp = cs->pcsCTP();
	if (0 == csctp) {
		mpv->MessageBox(SCSErrCsyNotEditable.sVal(),SCSErrCsyEditor.sVal(),MB_OK|MB_ICONSTOP);
		fOk = false;
		return;
	}
	else
		fLatLon = ( 0 != csctp->csOther->pcsLatLon());

	MapCompositionDoc* mcd = mpv->GetDocument();
	for(int i = 0; i < mcd->rootDrawer->getDrawerCount(); ++i)
	{
		AbstractMapDrawer* dr = CMAPDRW(mcd->rootDrawer, i);
		FileName fn = dr->getName();
		if (fn.sExt != "") {
			csctp->fnBackgroundMap = fn;
			break;
		}  
	}

	CoordSystemDirectLinear* csdl = csctp->pcsDirectLinear();
	if (csdl) 
		mapDTM = csdl->mapDTM;

	CoordSystemOrthoPhoto* csop = csctp->pcsOrthoPhoto();
	if (csop)
		mapDTM = csop->mapDTM;

	if (mapDTM.fValid()) {
		DomainValueRangeStruct dvrsZ(-99999,99999,0.01);
		colZdem = csctp->tbl()->col("Z_dtm");
		if (!colZdem.fValid()) 
			colZdem = csctp->tbl()->colNew("Z_dtm", dvrsZ);
		colZdem->sDescription = "Terrain Elevation (DTM height)";
		colZdem->SetReadOnly();
		colZdem->SetOwnedByTable(true);
	}
	DomainValueRangeStruct dvrsRes(-9999,9999,0.01);
	colDX = csctp->tbl()->col("DX");
	if (!colDX.fValid())
		colDX = csctp->tbl()->colNew("DX", dvrsRes);
	colDY = csctp->tbl()->col("DY");
	if (!colDY.fValid())
		colDY = csctp->tbl()->colNew("DY", dvrsRes);
	colDX->SetReadOnly();
	colDY->SetReadOnly();
	if (csdl) {
		colZdiff = csdl->tbl()->colNew("Z_diff", dvrsRes);
		colZdiff->SetReadOnly();
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
	if (csctp->pcsTiePoints())
		addmen(ID_EDITGRFTRANSF)
	else if (csctp->pcsOrthoPhoto())
	addmen(ID_EDITGRFFIDMARKS);
	addmen(ID_EDITCSYBOUNDS);
	hmenEdit = men.GetSafeHmenu();
	men.Detach();
	UpdateMenu();

	MapWindow* mw = mpv->mwParent();
	DataWindow* dw = mpv->dwParent();
	if (0 == dw) {
		fOk = false;
		return;
	}
	gretBar.Create(dw, this);
	cstd = new TableDoc;
	cstd->OnOpenDocument(csctp->tbl());
	cstd->AddView(gretBar.grtv);
	gretBar.grtv->OnInitialUpdate();
	mpv->GetParentFrame()->DockControlBar(&gretBar,AFX_IDW_DOCKBAR_BOTTOM);

	CWindowDC dc(CWnd::GetDesktopWindow());
	TEXTMETRIC tm;
	dc.GetTextMetrics(&tm);
	int iHeight = tm.tmHeight * 8;
	int iWidth = tm.tmAveCharWidth * 20;
	if (csdl != 0 || csop != 0) //extra text for csy dirlin and orthoph
		iWidth *= 5;
	CRect rect(0,0,iWidth,iHeight);

	ButtonBar& bb = dw->bbDataWindow;
	bb.LoadButtons("csyedit.but");
	CToolBarCtrl& tbc = bb.GetToolBarCtrl();
	TBBUTTONINFO bi;
	if (tbc.GetButtonInfo(ID_EDITGRFTRANSF, &bi)) 
	{
		CoordSystemTiePoints *pcstiep = csctp->pcsTiePoints();
		int id = bb.CommandToIndex(ID_EDITGRFTRANSF); 
		if (pcstiep->pcsTiePoints()) {
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
			cbTransf.Create(&bb,rect,this,pcstiep->transf);
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
	bbTxt.SetWindowText(SCSTitleInfoBar.scVal());
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

	htpTopic = htpCoordSystemEditor;
	sHelpKeywords = "Coordsys tiepoints editor";
	if (csctp->pcsOrthoPhoto())
		dw->PostMessage(WM_COMMAND, ID_EDITGRFFIDMARKS, 0);
}

CoordSystemEditor::~CoordSystemEditor()
{
	csctp->Store();
}

bool CoordSystemEditor::OnContextMenu(CWnd* pWnd, CPoint point)
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
	if (csctp->pcsTiePoints())
		addmen(ID_EDITGRFTRANSF)
		men.AppendMenu(MF_SEPARATOR);
	addmen(ID_CONFIGURE);
	addmen(ID_EXITEDITOR);
	menSub.CreateMenu();
	men.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, point.x, point.y, pWnd);
	return true;
}

IlwisObject CoordSystemEditor::obj() const
{
	return cs;
}

zIcon CoordSystemEditor::icon() const
{
	return zIcon("CsyIcon");
}

String CoordSystemEditor::sTitle() const
{
	String s(SCSTitleCsyEditor_s.sVal(), cs->sName());
	return s;
}

int CoordSystemEditor::draw(CDC* cdc, zRect rect, Positioner* psn, volatile bool* fDrawStop)
{
	cdc->SetTextAlign(TA_LEFT|TA_TOP); //	default
	cdc->SetBkMode(TRANSPARENT);
	MinMax mm = psn->mmSize();
	for (long r = 1; r <= csctp->iNr(); ++r) {
		Color clr;
		if (csctp->fActive(r))
		{
			if (rSigma <= 0)
				clr = colActive;
			else {
				double rDX = colDX->rValue(r);
				double rDY = colDY->rValue(r);			
				double rErr = sqrt(rDX * rDX + rDY * rDY);
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
		cdc->SetTextColor(clr);
		smb.col = clr;
		Coord crd = csctp->crd(r);
		zPoint pnt = psn->pntPos(crd);
		zPoint pntText = smb.pntText(cdc, pnt);
		String s("%li", r);
		cdc->TextOut(pntText.x,pntText.y,s.sVal());
		smb.drawSmb(cdc, 0, pnt);
	}
	if (efmf) {
		efmf->draw(cdc, rect, psn);
		efmf->drawPrincPoint(cdc, rect, psn);
	}
	return 0;
}

void CoordSystemEditor::OnTransformation()
{
	class TransfForm: public FormWithDest
	{
	public:
		TransfForm(CWnd* wPar, int* transf)
			: FormWithDest(wPar, SCSTitleTransf)
		{ // same sequence as in enum CoordSystemCTP::Transf
			RadioGroup* rg = new RadioGroup(root, SGRUiTransf, transf);
			new RadioButton(rg, SGRUiConform);
			new RadioButton(rg, SGRUiAffine);
			new RadioButton(rg, SGRUiSecondOrder);
			new RadioButton(rg, SGRUiFullSecondOrder);
			new RadioButton(rg, SGRUiThirdOrder);
			new RadioButton(rg, SGRUiProjective);
			SetMenHelpTopic(htpCSEditTransf);
			create();
		}
	};
	CoordSystemTiePoints *pcstiep = csctp->pcsTiePoints();
	if (0 != pcstiep) {
		int transf = pcstiep->transf;
		TransfForm frm(mpv, &transf);
		if (frm.fOkClicked()) {
			pcstiep->transf = static_cast<CoordSystemTiePoints::Transf>(transf); // + 1);
			pcstiep->Updated();
			cbTransf.SetCurSel(pcstiep->transf);
			Calc();
		}
	}
}

class AddTiePointCsyForm: public FormWithDest
{
public: 
	AddTiePointCsyForm(CWnd* wPar, CoordSystemEditor* cse,
		const Coord& crdDflt, const Coord& crdRefDflt, int iNr, bool fLatLong)
		: FormWithDest(wPar, SCSTitleAddTiePoint), 
		edit(cse), crd(crdDflt), crdRef(crdRefDflt), frDTMheight(0)
	{
		String s(SCSRemAddTiepointNr_i.sVal(), iNr);
		StaticText* st = new StaticText(root, s);
		st->SetIndependentPos();
		new FieldCoord(root, SCSUiXY, &crd);
		if (!fLatLong)
			fldCrd = new FieldCoord(root, SCSUiRefXY, &crdRef);
		else {
			llRef = edit->csctp->csOther->llConv(crdRef);
			fldLL = new FieldLatLon(root, SGRUiLatLon, &llRef);
		}
		fZ = false;
		rZ = rUNDEF;
		if (edit->mapDTM.fValid()) {
			CheckBox* cb = new CheckBox(root, SGRUiZ, &fZ);
			frDTMheight = new FieldReal(cb, "", &rZ);
		}
		SetMenHelpTopic(htpCSEditAddPoint);
		edit->atpf = this;
		create();
	}
	~AddTiePointCsyForm() {
		edit->atpf = 0;
	}
	void SetCoord(Coord c) {
		fldCrd->SetVal(c);
	}
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
			crdRef.x = llRef.Lon;
			crdRef.y = llRef.Lat;
		}
		if (fZ)
			edit->EditFieldCoordOK(crd,crdRef,rZ);
		else
			edit->EditFieldCoordOK(crd,crdRef);
		return 0;
	}
private:
	CoordSystemEditor* edit;
	FieldCoord* fldCrd;
	FieldReal* frDTMheight;
	FieldLatLon* fldLL;
	Coord crd, crdRef;
	LatLon llRef;
	bool fZ;
	double rZ;
};


void CoordSystemEditor::OnAddPoint()
{
	Coord crd, crdRef;
	int iNr = csctp->iNr() + 1;
	AddTiePointCsyForm frm(mpv, this, crd, crdRef, iNr, fLatLon);
}

void CoordSystemEditor::EditFieldCoordOK(const Coord& crd, const Coord& crdRef)
{
	int iPnt = csctp->AddRec(crd, crdRef);
	drawPoint(iPnt);
	cstd->CheckNrRecs();
	Calc();
}

void CoordSystemEditor::EditFieldCoordOK(const Coord& crd, const Coord& crdRef, double rZ)
{
	int iPnt = csctp->AddRec(crd, crdRef);
	Column colZ = csctp->tbl()->col("Z");
	if (colZ.fValid())
		colZ->PutVal(iPnt, rZ);
	drawPoint(iPnt);
	cstd->CheckNrRecs();
	Calc();
}

void CoordSystemEditor::OnTransfChanged()
{ 
	CoordSystemTiePoints *pcstiep = csctp->pcsTiePoints();
	if (0 != pcstiep) {
		int iTransf = cbTransf.GetCurSel();
		pcstiep->transf = static_cast<CoordSystemTiePoints::Transf>(iTransf);
		Calc();
	}
}

void CoordSystemEditor::Calc()
{
	if (0 == edTxt.m_hWnd)
		return;
	if (fInCalc)
		return;
	fInCalc = true;

	rSigma = 0;
	if (csctp->tbl()->fChanged)
		csctp->Updated();
	if (csctp->fChanged)
		csctp->Store(); // to make Additional Info usable

	if (mapDTM.fValid())
		for (int i = 1; i <= csctp->iNr(); ++i) {
			Coord crd = csctp->crdRef(i);
			double rZ = mapDTM->rValue(crd);
			colZdem->PutVal(i, rZ);
		}
		int iRes = csctp->Compute();
		if (iRes) {
			switch (iRes) {
case -1:
case -2:
	edTxt.SetWindowText(SGRRemNotEnoughPoints.scVal());
	break;
case -3:
	edTxt.SetWindowText(SGRRemSingularMatrix.scVal());
	break;
case -4:
	edTxt.SetWindowText("Incorrect Heights");
	break;
case -5:
	edTxt.SetWindowText("No valid DTM");
	break;
default:
	edTxt.SetWindowText(SGRRemError.scVal());
	break;
			}    
			for (int i = 1; i <= csctp->iNr(); ++i) {
				colDX->PutVal(i, rUNDEF);
				colDY->PutVal(i, rUNDEF);
			}
		}  
		else {
			int iNr = 0;
			CoordSystemDirectLinear* csdl = csctp->pcsDirectLinear();
			CoordSystemOrthoPhoto* csop = csctp->pcsOrthoPhoto();

			for (int i = 1; i <= csctp->iNr(); ++i) {
				Coord crd = csctp->crd(i);
				Coord crdRef = csctp->crdRef(i);
				Coord crdCalc = csctp->cConvFromOther(crdRef);
				double rDX = crd.x - crdCalc.x;
				double rDY = crd.y - crdCalc.y;
				colDX->PutVal(i, rDX);
				colDY->PutVal(i, rDY);
				if (csctp->fActive(i)) {
					iNr += 1;
					rSigma += rDX * rDX + rDY * rDY;
					if (csdl != 0)
						colZdiff->PutVal(i, csdl->rVerticDist[i-1]);
				}
			}
			if (iNr >= csctp->iMinNr()) {
				String s;
				if (iNr == csctp->iMinNr() && (csdl == 0)) {
					rSigma = rUNDEF;
					s = "Sigma = ? ";
				}
				else {
					if (csdl != 0)
						rSigma = sqrt(rSigma / (2 * (iNr - csctp->iMinNr()) + 1));
					else
						rSigma = sqrt(rSigma / (2 * (iNr - csctp->iMinNr())));
					s = String(SCSRemSigma_f.scVal(), rSigma);
				}
				CVector cvPC;
				if (csdl != 0)
					cvPC = csdl->cvGetProjectionCenter();
				if (csop != 0)
					cvPC = csop->cvGetProjectionCenter();

				if (csdl != 0 || csop != 0) {
					double rNadX = cvPC(0);
					double rNadY = cvPC(1);
					Coord cNadir = Coord(rNadX, rNadY);
					double rFlyH = (double)(cvPC(2) - mapDTM->rValue(cNadir));
					s &= String("   Flying Height: %.0f m" , rFlyH );
					s &= String("  Nadir: (%.0f, %.0f) " , rNadX, rNadY);
				}
				if (csdl != 0) {
					double rNonCopla = csdl->rGetNonCoplanarity();
					s &= String("    NC = %.1f m," , rNonCopla);
					double rZconf = csdl->rGetZConfidRange();
					s &= String("  Z-Conf = %.1f m," , rZconf);				
				}
				if (csop != 0) {				
					Coord cPP = csop->crdGetPrincipalPoint();
					s &= String("  Princ. Point: X= %.1f, Y= %.1f " , cPP.x, cPP.y);
				}
				edTxt.SetWindowText(s.scVal());
			}
			else {
				edTxt.SetWindowText("");
				rSigma = 0;
			}
		}  
		cstd->UpdateAllViews(0);
		fInCalc = false;
}

zRect CoordSystemEditor::rectPoint(long iNr)
{
	Coord crd = csctp->crd(iNr);
	zPoint pnt = mpv->pntPos(crd);
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


int CoordSystemEditor::drawPoint(long iNr)
{
	zRect rect = rectPoint(iNr);
	mpv->InvalidateRect(&rect);
	return 0;
}

bool CoordSystemEditor::OnLButtonDown(UINT nFlags, CPoint point)
{
	mpv->SetFocus();
	Coord crd = mpv->crdPnt(point);
	if (efmf) {
		efmf->OnLButtonDown(crd);
		return true;
	}
	if (iSelPnt) {
		drawPoint(iSelPnt);
		csctp->SetCoord(iSelPnt, crd);
		drawPoint(iSelPnt);
		Calc();
		iSelPnt = 0;
		curActive = curAdd;
	}
	else {
		Coord crdRef;
		int i = csctp->iRec(crd);
		if (i > 0)
			crdRef = csctp->crdRef(i);
		else {
			i = csctp->iNr() + 1;
			crdRef = csctp->cConvToOther(crd);
		}
		AddTiePointCsyForm frm(mpv, this, crd, crdRef, i, fLatLon);
	}
	return true;
}

LRESULT CoordSystemEditor::OnUpdate(WPARAM wParam, LPARAM lParam)
{
	CoordMessage cm = (CoordMessage) wParam;
	if (0 != atpf) {
		if (cmMOUSECLICK & cm || cmDIGICLICK1 & cm) 
		{
			CoordWithCoordSystem* c = (CoordWithCoordSystem*)(void*) lParam;
			Coord crd = csctp->csOther->cConv((CoordSystem)*c, *c);
			if (0 != atpf) {
				atpf->SetCoord(crd);
				atpf->SetFocus();
			}
		}
	}
	else {
		if (cmDIGICLICK1 & cm) 
		{
			CoordWithCoordSystem* c = (CoordWithCoordSystem*)(void*) lParam;
			Coord crd = *c;
			if (efmf) {
				efmf->OnLButtonDown(crd);
				return 0;
			}
			if (iSelPnt) {
				drawPoint(iSelPnt);
				csctp->SetCoord(iSelPnt, crd);
				drawPoint(iSelPnt);
				Calc();
				iSelPnt = 0;
				curActive = curAdd;
			}
			else {
				Coord crdRef;
				int i = csctp->iRec(crd);
				if (i > 0)
					crdRef = csctp->crdRef(i);
				else {
					i = csctp->iNr() + 1;
					crdRef = csctp->cConvToOther(crd);
				}
				drawPoint(i);
				AddTiePointCsyForm frm(mpv, this, crd, crdRef, i, fLatLon);
			}
		}
	}
	return 0;
}

void CoordSystemEditor::OnDelPoint()
{
	class DelTiePointForm: public FormWithDest
	{
	public:
		DelTiePointForm(CWnd* wPar, int* iNr, const RangeInt& ri)
			: FormWithDest(wPar, SCSTitleDelTiePoint)
		{
			fi = new FieldInt(root, SGRUiTiePointNumber, iNr, ri);
			SetMenHelpTopic(htpCSEditDelPoint);
			create();
		}
	private:
		FormEntry* feDefaultFocus()
		{ return fi;
		}
		FieldInt* fi;  
	};  
	int iNr = csctp->iNr();
	if (iNr <= 0)
		return;
	int iDel = iNr;  
	RangeInt ri(1, iNr);
	DelTiePointForm frm(mpv, &iDel, ri);
	if (frm.fOkClicked()) {
		DelPoints(iDel,1);
	}
}

void CoordSystemEditor::DelPoints(int iDel, int iRecs)
{
	CWaitCursor cur;
	if (iRecs <= 0)
		return;
	int iMax = csctp->iNr();
	if (iMax <= 0)
		return;
	for (int i = iDel; i <= iMax; ++i)
		drawPoint(i);
	csctp->tbl()->DeleteRec(iDel, iRecs);
	cstd->CheckNrRecs();
	Calc();
}


class CseConfigForm: public FormWithDest
{
public:
	CseConfigForm(CWnd* parent, CoordSystemEditor* cse)
		: FormWithDest(parent, SCSTitleConfCsyEditor)
	{
		new FieldColor(root, SGRUiActGoodColor, &cse->colActGood);
		new FieldColor(root, SGRUiActMedColor, &cse->colActive);
		new FieldColor(root, SGRUiActBadColor, &cse->colActBad);
		new FieldColor(root, SGRUiPassiveColor, &cse->colPassive);
		new FieldInt(root, SGRUiSmbSize, &cse->smb.iSize, ValueRange(1L,100L), true);
		SetMenHelpTopic(htpCSEditCnf);
		create();
	}
};  

void CoordSystemEditor::OnConfigure()
{
	CseConfigForm frm(mpv, this);
	if (frm.fOkClicked()) {
		int iNr = csctp->iNr();
		for (int i = 1; i <= iNr; ++i)
			drawPoint(i);

		IlwisSettings settings("Map Window\\TiePoint Editor");

		settings.SetValue("Active Color", colActive);
		settings.SetValue("Good Color", colActGood);
		settings.SetValue("Bad Color", colActBad);
		settings.SetValue("Passive Color", colPassive);
		settings.SetValue("Symbol Size", smb.iSize);
	}
}

class EditBoundsForm: public FormWithDest
{
public:
	EditBoundsForm(CWnd* wPar, CoordSystemCTP* cs)
		: FormWithDest(wPar, SCSTitleBoundaries), csctp(cs)
	{
		iImg = IlwWinApp()->iImage(".csy");

		StaticText* st = new StaticText(root, SCSUiDescription);
		st->psn->SetBound(0,0,0,0);
		st->SetIndependentPos();
		FieldString* fsDesc = new FieldString(root, "", &csctp->sDescription);
		fsDesc->SetWidth(150);
		fsDesc->SetIndependentPos();

		fldCrdMin = new FieldCoord(root, SCSUiMinXY, &cs->cb.cMin);
		fldCrdMax = new FieldCoord(root, SCSUiMaxXY, &cs->cb.cMax);
		PushButton* pb = new PushButton(root, SCSUiDefaults,
			(NotifyProc)&EditBoundsForm::SetDefaults);
		SetMenHelpTopic(htpCSEditBounds);
		create();
	}
private:
	int SetDefaults(Event*) {
		try {
			CoordBounds cb = csctp->csOther->cb;
			cb = csctp->cbConv(csctp->csOther, cb);
			fldCrdMin->SetVal(cb.cMin);
			fldCrdMax->SetVal(cb.cMax);
		}
		catch (ErrorObject& err) {
			err.Show();
		}
		return 0;
	}
	CoordSystemCTP* csctp;
	FieldCoord* fldCrdMin;
	FieldCoord* fldCrdMax;
};


void CoordSystemEditor::OnBoundaries()
{
	EditBoundsForm frm(mpv, csctp);
}


void CoordSystemEditor::OnEditFiducialMarks()
{
	if (0 == csctp->pcsOrthoPhoto())
		return;
	if (efmf) {
		efmf->SetFocus();  
		return;
	}  
	efmf = new EditFiducialMarksForm(mpv, csctp->pcsOrthoPhoto());
	efmf->create();
	DataWindow* dw = mpv->dwParent();
	dw->ShowControlBar(&gretBar,FALSE,FALSE);
}

void CoordSystemEditor::OnStopFiducialMarks()
{
	efmf = 0;
	int iRes = csctp->Compute();
	if (iRes < -10) { 
		OnExit();
		return;
	}
	DataWindow* dw = mpv->dwParent();
	dw->ShowControlBar(&gretBar,TRUE,FALSE);
	Calc();
}

