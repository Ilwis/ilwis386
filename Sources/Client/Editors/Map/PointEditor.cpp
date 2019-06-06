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
// PointEditor.cpp: implementation of the PointEditor class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Engine\Map\Segment\Seg.h"
#include "Engine\Map\Polygon\POL.H"
#include "Engine\Map\Point\PNT.H"
#include "Client\Mapwindow\Positioner.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Client\Editors\Editor.h"
#include "Client\Editors\Digitizer\DigiEditor.h"
#include "Client\Editors\Utils\SYMBOL.H"
#include "Client\Base\ButtonBar.h"
#include "Client\Editors\Map\PointEditor.h"
#include "Client\FormElements\syscolor.h"
#include "Client\FormElements\fldval.h"
#include "Client\Mapwindow\AreaSelector.h"
#include "Client\Mapwindow\IlwisClipboardFormat.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\MainWindow\Catalog\CatalogDocument.h"
#include "Engine\Map\Mapview.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Headers\constant.h"
#include "Engine\Domain\dmident.h"
#include "Client\FormElements\fldcolor.h"
#include "Client\ilwis.h"
#include "Headers\Htp\Ilwis.htp"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\Mapwindow\MapWindow.h"
#include "Engine\Domain\DomainUniqueID.h"
#include "Engine\Table\Rec.h"
#include "Engine\Drawers\TextDrawer.h"
#include "Engine\Drawers\OpenGLText.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(PointEditor, DigiEditor)
	//{{AFX_MSG_MAP(PointEditor)
	ON_COMMAND(ID_COPY, OnCopy)
	ON_UPDATE_COMMAND_UI(ID_COPY, OnUpdateCopy)
	ON_COMMAND(ID_PASTE, OnPaste)
	ON_UPDATE_COMMAND_UI(ID_PASTE, OnUpdatePaste)
	ON_COMMAND(ID_EDIT, OnEdit)
	ON_UPDATE_COMMAND_UI(ID_EDIT, OnUpdateEdit)
	ON_COMMAND(ID_CLEAR, OnClear)
	ON_UPDATE_COMMAND_UI(ID_CLEAR, OnUpdateCopy)
	ON_COMMAND(ID_CUT, OnCut)
	ON_UPDATE_COMMAND_UI(ID_CUT, OnUpdateCopy)
	ON_COMMAND(ID_SELECTMODE, OnSelectMode)
	ON_UPDATE_COMMAND_UI(ID_SELECTMODE, OnUpdateMode)
	ON_COMMAND(ID_MOVEMODE, OnMoveMode)
	ON_UPDATE_COMMAND_UI(ID_MOVEMODE, OnUpdateMode)
	ON_COMMAND(ID_INSERTMODE, OnInsertMode)
	ON_UPDATE_COMMAND_UI(ID_INSERTMODE, OnUpdateMode)
	ON_COMMAND(ID_FINDUNDEFS, OnFindUndefs)
	ON_UPDATE_COMMAND_UI(ID_FINDUNDEFS, OnUpdateMode)
	ON_COMMAND(ID_CONFIGURE, OnConfigure)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_UNDOALL, OnUndoAllChanges)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
	ON_UPDATE_COMMAND_UI(ID_UNDOALL, OnUpdateFileSave)
	ON_COMMAND(ID_ADDPOINT, OnAddPoint)
	ON_COMMAND(ID_SEGSETBOUNDS, OnSetBoundaries)
	ON_COMMAND(ID_SELALL, OnSelectAll)
	ON_COMMAND(ID_SELECTAREA, OnAreaSelected)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#define sMen(ID) ILWSF("men",ID).c_str()
#define addmen(ID) men.AppendMenu(MF_STRING, ID, sMen(ID)); 
#define addSub(ID) menSub.AppendMenu(MF_STRING, ID, sMen(ID)); 

PointEditor::PointEditor(MapPaneView* mpvw, PointMap m)
: DigiEditor(mpvw,m->cb()),
curEdit("EditCursor"),
curPntEdit("EditPntCursor"),
curPntMove("EditPntMoveCursor"), 
curPntMoving("EditPntMovingCursor"), 
mp(m)
{
	iFmtPnt = RegisterClipboardFormat("IlwisPoints");
	iFmtDom = RegisterClipboardFormat("IlwisDomain");

	if (mp->fDependent() || mp->fDataReadOnly() || !mp->dm()->fValidDomain()) {
		mpv->MessageBox(TR("Not an editable Point Map").c_str(), TR("Point Editor").c_str(), MB_OK|MB_ICONSTOP);
		fOk = false;
		return;
	}
	mp->KeepOpen(true);
	dvs = mp->dvrs();

	MapCompositionDoc* mcd = mpv->GetDocument();
	vector<NewDrawer *> allDrawers;
	mcd->rootDrawer->getDrawers(allDrawers);
	for(int i = 0; i < allDrawers.size(); ++i) {
		SpatialDataDrawer *dr = dynamic_cast<SpatialDataDrawer *>(allDrawers.at(i));
		if ( dr) {
			if ( dr->getBaseMap()->fnObj == mp->fnObj) {
				Editor::drw = dr->getDrawer(0);
				fDrawerActive = Editor::drw->isActive();
				Editor::drw->setActive(false);
				break;
			}
		}
	}

	if (0 == Editor::drw) {
		SpatialDataDrawer *dr = dynamic_cast<SpatialDataDrawer *>(mcd->drAppend(mp, IlwisDocument::otEDIT));
		if (dr) {
			Editor::drw = dr->getDrawer(0);
			fDrawerActive = Editor::drw->isActive();
			Editor::drw->setActive(false);
		}
		mcd->UpdateAllViews(mpv,2);
	}

	aSelect.Resize(mp->iFeatures());
	for (long i = 0; i < mp->iFeatures(); ++i)
		aSelect[i] = false;
	mode = modeSELECT;
	fFindUndefs = false;
	smb.smb = smbPlus;
	fText = true;
	colText = (Color)-1;
	colBack = (Color)-2;
	colFindUndef = Color(255,0,0);
	fnt.FromHandle((HFONT)GetStockObject(ANSI_VAR_FONT));
	curActive = curEdit;
	DigInit(Coord());

	IlwisSettings settings("Map Window\\Point Editor");

	String fn = IlwWinApp()->Context()->fnUserINI().sFullName();
	char sBuf[80];
	String sVal = "yes";
	GetPrivateProfileString("Point Editor", "Show Text", sVal.sVal(), sBuf, 79, fn.sVal());
	fText = strcmp(sBuf, "no") ? true : false;
	fText = settings.fValue("Show Text", fText);
	sVal = String("%06lx", (long)colText);
	GetPrivateProfileString("Point Editor", "Text Color", sVal.sVal(), sBuf, 79, fn.sVal());
	if (sBuf[0])
		sscanf(sBuf,"%lx",&colText);
	colText = settings.clrValue("Text Color", colText);
	sVal = String("%06lx", (long)colBack);
	GetPrivateProfileString("Point Editor", "Background Color", sVal.sVal(), sBuf, 79, fn.sVal());
	if (sBuf[0])
		sscanf(sBuf,"%lx",&colBack);
	colBack = settings.clrValue("Background Color", colBack);
	smb.smb = (SymbolType)GetPrivateProfileInt("Point Editor", "Symbol Type", (int)smb.smb, fn.sVal());
	smb.smb = (SymbolType)settings.iValue("Symbol Type", (int)smb.smb);
	smb.iSize = GetPrivateProfileInt("Point Editor", "Symbol Size", smb.iSize, fn.sVal());
	smb.iSize = settings.iValue("Symbol Size", smb.iSize);
	smb.iWidth = GetPrivateProfileInt("Point Editor", "Pen Width", smb.iWidth, fn.sVal());
	smb.iWidth = settings.iValue("Pen Width", smb.iWidth);
	sVal = String("%06lx", (long)smb.col);
	GetPrivateProfileString("Point Editor", "Pen Color", sVal.sVal(), sBuf, 79, fn.sVal());
	sscanf(sBuf,"%lx",&smb.col);
	smb.col = settings.clrValue("Pen Color", smb.col);
	sVal = String("%06lx", (long)smb.fillCol);
	GetPrivateProfileString("Point Editor", "Fill Color", sVal.sVal(), sBuf, 79, fn.sVal());
	sscanf(sBuf,"%lx",&smb.fillCol);
	smb.fillCol = settings.clrValue("Fill Color", smb.fillCol);
	colFindUndef = settings.clrValue("Find Undef Color", colFindUndef);

	CMenu men;
	CMenu menSub;
	men.CreateMenu();
	addmen(ID_UNDOALL);
	addmen(ID_FILE_SAVE);
	addmen(ID_SEGSETBOUNDS);
	men.AppendMenu(MF_SEPARATOR);
	addmen(ID_CONFIGURE);
	menSub.CreateMenu();
	addSub(ID_FILE_DIGREF);
	addSub(ID_DIGACTIVE);
	men.AppendMenu(MF_POPUP, (UINT)menSub.GetSafeHmenu(), sMen(ID_DIG)); 
	menSub.Detach();
	addmen(ID_EXITEDITOR);
	hmenFile = men.GetSafeHmenu();
	men.Detach();

	men.CreateMenu();
	addmen(ID_CUT  );
	addmen(ID_COPY );
	addmen(ID_PASTE);
	addmen(ID_CLEAR);
	men.AppendMenu(MF_SEPARATOR);
	addmen(ID_SELALL);
	addmen(ID_EDIT);
	addmen(ID_ADDPOINT);
	men.AppendMenu(MF_SEPARATOR);
	addmen(ID_SELECTMODE);
	addmen(ID_MOVEMODE);
	addmen(ID_INSERTMODE);
	men.AppendMenu(MF_SEPARATOR);
	addmen(ID_FINDUNDEFS);
	hmenEdit = men.GetSafeHmenu();
	men.Detach();
	UpdateMenu();

	DataWindow* dw = mpv->dwParent();
	if (dw) {
		dw->bbDataWindow.LoadButtons("pntedit.but");
		dw->RecalcLayout();
	}
	help = "ilwis\\point_editor_functionality.htm";
	sHelpKeywords = "Point editor";

	const SVGLoader *loader = NewDrawer::getSvgLoader();
	SVGLoader::const_iterator cur = loader->find("half-tone");
	if ( cur == loader->end() || (*cur).second->getType() == IVGElement::ivgPOINT) {
		hatch = 0;
		hatchInverse = 0;
	} else {
		hatch = (*cur).second->getHatch();
		hatchInverse = (*cur).second->getHatchInverse();
	}
}

PointEditor::~PointEditor()
{
	if (mp->fChanged) {
		mp->Updated();
		mp->Store();
	}  
	mp->KeepOpen(false);
	if (Editor::drw) {
		PreparationParameters pp(NewDrawer::ptRENDER | NewDrawer::ptGEOMETRY);
		Editor::drw->prepare(&pp);
		Editor::drw->setActive(fDrawerActive);
	}
}

bool PointEditor::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CMenu men;
	men.CreatePopupMenu();
	addmen(ID_NORMAL);
	addmen(ID_ZOOMIN);
	addmen(ID_ZOOMOUT);
	addmen(ID_PANAREA);
	men.AppendMenu(MF_SEPARATOR);
	addmen(ID_EDIT);
	men.EnableMenuItem(ID_EDIT, fEditOk() ? MF_ENABLED : MF_GRAYED);
	addmen(ID_EDIT_CUT);
	men.EnableMenuItem(ID_EDIT_CUT, fCopyOk() ? MF_ENABLED : MF_GRAYED);
	addmen(ID_EDIT_COPY);
	men.EnableMenuItem(ID_EDIT_COPY, fCopyOk() ? MF_ENABLED : MF_GRAYED);
	addmen(ID_EDIT_PASTE);
	men.EnableMenuItem(ID_EDIT_PASTE, fPasteOk() ? MF_ENABLED : MF_GRAYED);
	addmen(ID_CLEAR);	
	men.EnableMenuItem(ID_CLEAR, fCopyOk() ? MF_ENABLED : MF_GRAYED);
	men.AppendMenu(MF_SEPARATOR);
	addmen(ID_ADDPOINT);
	addmen(ID_FINDUNDEFS);
	if (fFindUndefs)
		men.CheckMenuItem(ID_FINDUNDEFS,MF_BYCOMMAND|MF_CHECKED);
	men.AppendMenu(MF_SEPARATOR);
	//  add(ID_SELECTMODE);
	//  add(ID_MOVEMODE);
	//  add(ID_INSERTMODE);
	//  men->addHbreak();
	addmen(ID_CONFIGURE);
	addmen(ID_EXITEDITOR);
	men.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, point.x, point.y, pWnd);
	return true;
}

IlwisObject PointEditor::obj() const
{
	return mp;
}

int PointEditor::draw(volatile bool* fDrawStop)
{
	MapCompositionDoc* mcd = mpv->GetDocument();
	RootDrawer * rootDrawer = mcd->rootDrawer;
	CoordBounds cbZoom = rootDrawer->getCoordBoundsZoom();
	double delta = smb.iSize * cbZoom.width() / 2000.0;

	Color cText, cBack, cFgBr, cBgBr;
	cFgBr = SysColor(COLOR_HIGHLIGHT);
	cBgBr = SysColor(COLOR_WINDOW);
	if (fText) {
		if ((long)colText == -1)
			cText = SysColor(COLOR_WINDOWTEXT);
		else {
			cText = colText;
		}
		cText.alpha() = 255;
		if ((long)colBack != -2) {
			cBack = colBack;
		}
	}
	ILWIS::DrawerParameters dpLayerDrawer (rootDrawer, 0);
	ILWIS::TextLayerDrawer *textLayerDrawer = (ILWIS::TextLayerDrawer *)NewDrawer::getDrawer("TextLayerDrawer", "ilwis38",&dpLayerDrawer);	
	ILWIS::DrawerParameters dpTextDrawer (rootDrawer, textLayerDrawer);
	ILWIS::TextDrawer *textDrawer = (ILWIS::TextDrawer *)NewDrawer::getDrawer("TextDrawer","ilwis38",&dpTextDrawer);
	OpenGLText * font = new OpenGLText (rootDrawer, "arial.ttf", 15, true, 1, -15);
	textLayerDrawer->setFont(font);

	if (smb.iWidth != 1) {
		if (smb.smb == smbCircle) {
			glEnable(GL_LINE_SMOOTH);
			glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
		}
		glLineWidth(smb.iWidth);
	}

	for (long r = 0; r < mp->iFeatures(); ++r) {
		Coord crd = mp->cValue(r);
		if (crd.fUndef())
			continue;
		crd = rootDrawer->glConv(mp->cs(), crd);
		if (!rootDrawer->is3D())
			crd.z = 0; // OpenGLText is unable to draw on 2D when crd.z != 0
		if (fText) {
			if (mode == modeMOVING)
			{
				if (r == iActNr) 
				{
					Color clr = cText.clrDraw(drcLIGHT).clrDraw(drcGREY);
					font->setColor(clr);
				}
				else
					font->setColor(cText);
			} else
				font->setColor(cText);
			zPoint pntText = smb.pntText(0,zPoint(0,0)); // does this compute a good offset for the text?
			String s = mp->sValue(r);
			textDrawer->setText(crd,s);
			if ((long)colBack != -2) {
				glColor4f(cBack.redP(), cBack.greenP(), cBack.blueP(), 1);
				CoordBounds cbText = textDrawer->getTextExtent();
				cbText.cMin += crd;
				cbText.cMax += crd;
				cbText.cMin.y -= textDrawer->getHeight();
				cbText.cMax.y -= textDrawer->getHeight();
				cbText *= 1.3;
				double heightIncrease = cbText.height() / 8.0;
				cbText.cMax.y += heightIncrease;
				cbText.cMin.y -= heightIncrease;
				glBegin(GL_QUADS);
				glVertex3f(cbText.cMin.x, cbText.cMin.y, 0);
				glVertex3f(cbText.cMax.x, cbText.cMin.y, 0);
				glVertex3f(cbText.cMax.x, cbText.cMax.y, 0);
				glVertex3f(cbText.cMin.x, cbText.cMax.y, 0);
				glEnd();
			}
			textDrawer->draw(ILWIS::NewDrawer::drl2D);
		}
		if (aSelect[r]) {
			glColor4d(cFgBr.redP(), cFgBr.greenP(), cFgBr.blueP(), 1);
			glEnable(GL_POLYGON_STIPPLE);
			glPolygonStipple(hatch);
			glBegin(GL_QUADS);
			glVertex3f(crd.x - delta, crd.y - delta, 0);
			glVertex3f(crd.x + delta, crd.y - delta, 0);
			glVertex3f(crd.x + delta, crd.y + delta, 0);
			glVertex3f(crd.x - delta, crd.y + delta, 0);
			glEnd();
			glColor4d(cBgBr.redP(), cBgBr.greenP(), cBgBr.blueP(), 1);
			glPolygonStipple(hatchInverse);
			glBegin(GL_QUADS);
			glVertex3f(crd.x - delta, crd.y - delta, 0);
			glVertex3f(crd.x + delta, crd.y - delta, 0);
			glVertex3f(crd.x + delta, crd.y + delta, 0);
			glVertex3f(crd.x - delta, crd.y + delta, 0);
			glEnd();
			glDisable(GL_POLYGON_STIPPLE);
		}
		Symbol sym = smb;
		if (fFindUndefs && iUNDEF == mp->iRaw(r)) {
			sym.col = colFindUndef;
			sym.fillCol = colFindUndef;
		}
		if (mode == modeMOVING && r == iActNr) 
		{
			Color clr = sym.col.clrDraw(drcLIGHT).clrDraw(drcGREY);
			sym.col = clr;
			sym.fillCol = clr;
		}
		drawSmb(sym, crd, delta);
	}

	if (smb.iWidth != 1) {
		if (smb.smb == smbCircle) {
			glDisable(GL_LINE_SMOOTH);
		}
		glLineWidth(1);
	}
	drawDigCursor();
	delete textLayerDrawer;
	return 0;
}

void PointEditor::drawSmb(const Symbol & sym, const Coord & crd, double delta) const
{
	glPushMatrix();
	glTranslated(crd.x, crd.y, 0);
	glScaled(delta, delta, 0);

	switch (sym.smb) 
	{
	case smbCircle:
		{
			int iSteps = 20;
			double fStart = 0;
			glColor4d(sym.fillCol.redP(), sym.fillCol.greenP(), sym.fillCol.blueP(), 1);
			glBegin(GL_TRIANGLE_FAN);
			for (int i = 0; i < iSteps; ++i) {
				double f = fStart + M_PI * i * 2.0 / iSteps;
				glVertex3f(cos(f), sin(f), 0);
			}
			glEnd();
			glColor4d(sym.col.redP(), sym.col.greenP(), sym.col.blueP(), 1);
			glBegin(GL_LINE_STRIP);
			for (int i = 0; i <= iSteps; ++i) {
				double f = fStart + M_PI * i * 2.0 / iSteps;
				glVertex3f(cos(f), sin(f), 0);
			}
			glEnd();
		}
		break;
	case smbSquare:
		{
			int iSteps = 4;
			double fStart = M_PI / 4.0;
			glColor4d(sym.fillCol.redP(), sym.fillCol.greenP(), sym.fillCol.blueP(), 1);
			glBegin(GL_TRIANGLE_FAN);
			for (int i = 0; i < iSteps; ++i) {
				double f = fStart + M_PI * i * 2.0 / iSteps;
				glVertex3f(cos(f), sin(f), 0);
			}
			glEnd();
			glColor4d(sym.col.redP(), sym.col.greenP(), sym.col.blueP(), 1);
			glBegin(GL_LINE_STRIP);
			for (int i = 0; i <= iSteps; ++i) {
				double f = fStart + M_PI * i * 2.0 / iSteps;
				glVertex3f(cos(f), sin(f), 0);
			}
			glEnd();
		}
		break;
	case smbDiamond:
		{
			int iSteps = 4;
			double fStart = 0;
			glColor4d(sym.fillCol.redP(), sym.fillCol.greenP(), sym.fillCol.blueP(), 1);
			glBegin(GL_TRIANGLE_FAN);
			for (int i = 0; i < iSteps; ++i) {
				double f = fStart + M_PI * i * 2.0 / iSteps;
				glVertex3f(cos(f), sin(f), 0);
			}
			glEnd();
			glColor4d(sym.col.redP(), sym.col.greenP(), sym.col.blueP(), 1);
			glBegin(GL_LINE_STRIP);
			for (int i = 0; i <= iSteps; ++i) {
				double f = fStart + M_PI * i * 2.0 / iSteps;
				glVertex3f(cos(f), sin(f), 0);
			}
			glEnd();
		}
		break;
	case smbDeltaUp:
		{
			int iSteps = 3;
			double fStart = M_PI / 2.0; // angles are anticlockwise, starting horizontal
			glColor4d(sym.fillCol.redP(), sym.fillCol.greenP(), sym.fillCol.blueP(), 1);
			glBegin(GL_TRIANGLE_FAN);
			for (int i = 0; i < iSteps; ++i) {
				double f = fStart + M_PI * i * 2.0 / iSteps;
				glVertex3f(cos(f), sin(f), 0);
			}
			glEnd();
			glColor4d(sym.col.redP(), sym.col.greenP(), sym.col.blueP(), 1);
			glBegin(GL_LINE_STRIP);
			for (int i = 0; i <= iSteps; ++i) {
				double f = fStart + M_PI * i * 2.0 / iSteps;
				glVertex3f(cos(f), sin(f), 0);
			}
			glEnd();
		}
		break;
	case smbDeltaDown:
		{
			int iSteps = 3;
			double fStart = - M_PI / 2.0;
			glColor4d(sym.fillCol.redP(), sym.fillCol.greenP(), sym.fillCol.blueP(), 1);
			glBegin(GL_TRIANGLE_FAN);
			for (int i = 0; i < iSteps; ++i) {
				double f = fStart + M_PI * i * 2.0 / iSteps;
				glVertex3f(cos(f), sin(f), 0);
			}
			glEnd();
			glColor4d(sym.col.redP(), sym.col.greenP(), sym.col.blueP(), 1);
			glBegin(GL_LINE_STRIP);
			for (int i = 0; i <= iSteps; ++i) {
				double f = fStart + M_PI * i * 2.0 / iSteps;
				glVertex3f(cos(f), sin(f), 0);
			}
			glEnd();
		}
		break;
	case smbPlus:
		{
			glColor4d(sym.col.redP(), sym.col.greenP(), sym.col.blueP(), 1);
			glBegin(GL_LINES);
			glVertex3f(0, -1, 0);
			glVertex3f(0, 1, 0);
			glEnd();
		}
		// fall through
	case smbMinus:
		{
			glColor4d(sym.col.redP(), sym.col.greenP(), sym.col.blueP(), 1);
			glBegin(GL_LINES);
			glVertex3f(-1, 0, 0);
			glVertex3f(1, 0, 0);
			glEnd();
		}
		break;
	case smbCross:
		{
			glColor4d(sym.col.redP(), sym.col.greenP(), sym.col.blueP(), 1);
			glBegin(GL_LINES);
			glVertex3f(-1, -1, 0);
			glVertex3f(1, 1, 0);
			glVertex3f(-1, 1, 0);
			glVertex3f(1, -1, 0);
			glEnd();
		} 
		break;
	}
	glPopMatrix();
}

int PointEditor::draw(CDC* cdc, zRect rect, Positioner* psn, volatile bool* fDrawStop)
{
	MapCompositionDoc* mcd = mpv->GetDocument();
	/*
	-- naar DigiEditor::PreDraw ?? remove cursor?

	zRect rct;
	mappane->getInterior(rct);
	if (rct == rect)
	crdDig = Coord();
	*/
	Color cText, cBack, cFgBr, cBgBr;

	Color c;
	if ((long)smb.col == -1)
		c = SysColor(COLOR_WINDOWTEXT);
	else 
		c = smb.col;
	CPen pen(PS_SOLID,smb.iWidth,c);
	CPen* penOld = cdc->SelectObject(&pen);
	CBrush br;
	if ((long)smb.fillCol == -2)
		br.CreateStockObject(HOLLOW_BRUSH);
	else {
		c = smb.fillCol;
		br.CreateSolidBrush(c);
	}
	CBrush* brOld = cdc->SelectObject(&br);
	CFont* fntOld = 0;
	cFgBr = SysColor(COLOR_HIGHLIGHT);
	cBgBr = SysColor(COLOR_WINDOW);
	if (fText) {
		//    int nCol;
		if ((long)colText == -1)
			cText = SysColor(COLOR_WINDOWTEXT);
		else {
			cText = colText;
		}
		if ((long)colBack != -2) {
			cBack = colBack;
		}
		fntOld = cdc->SelectObject(&fnt);
		cdc->SetTextAlign(TA_LEFT|TA_TOP);  // = default
		cdc->SetTextColor(cText);
		if ((long)colBack == -2)
			cdc->SetBkMode(TRANSPARENT);
		else {
			cdc->SetBkMode(OPAQUE);
			cdc->SetBkColor(cBack);
		}
	}
	CBitmap bm;
	short bits[8] = { 0x99, 0x66, 0x66, 0x99, 0x99, 0x66, 0x66, 0x99 };
	bm.CreateBitmap(8,8,1,1,bits);
	CBrush brSel(&bm);

	CPen penSel(PS_NULL,0,Color(0));
	MinMax mm = psn->mmSize();
	for (long r = 0; r < mp->iFeatures(); ++r) {
		//    if (mappane->fDrawCheck())
		//      break;
		Coord crd = mp->cValue(r);
		if (crd.fUndef())
			continue;
		//double rRow, rCol;
		//mpv->Coord2RowCol(crd, rRow, rCol);
		zPoint pnt = mpv->pntPos(crd);
		if (fText) {
			if (mode == modeMOVING)
			{
				if (r == iActNr) 
				{
					Color clr = cText.clrDraw(drcLIGHT).clrDraw(drcGREY);
					cdc->SetTextColor(clr);
				}
				else
					cdc->SetTextColor(cText);
			}
			zPoint pntText = smb.pntText(cdc,pnt);
			String s = mp->sValue(r);
			cdc->TextOut(pntText.x,pntText.y,s.sVal());
		}
		if (aSelect[r]) {
			cdc->SetBkMode(OPAQUE);
			cdc->SetTextColor(cFgBr);
			cdc->SetBkColor(cBgBr);
			CPen* pen0 = cdc->SelectObject(&penSel);
			CBrush* br0 = cdc->SelectObject(&brSel);
			zRect rect;
			rect.left() = pnt.x - smb.iSize/2 - 1;
			rect.top() = pnt.y - smb.iSize/2 - 1;
			rect.right() = rect.left() + smb.iSize + 3;
			rect.bottom() = rect.top() + smb.iSize + 3;
			cdc->Rectangle(rect);
			cdc->SelectObject(pen0);
			cdc->SelectObject(br0);
			if (fText) {
				cdc->SetTextColor(cText);
				if ((long)colBack == -2)
					cdc->SetBkMode(TRANSPARENT);
				else
					cdc->SetBkColor(cBack);
			}
		}
		Symbol sym = smb;
		if (fFindUndefs && iUNDEF == mp->iRaw(r)) {
			sym.col = colFindUndef;
			sym.fillCol = colFindUndef;
		}
		if (mode == modeMOVING && r == iActNr) 
		{
			Color clr = sym.col.clrDraw(drcLIGHT).clrDraw(drcGREY);
			sym.col = clr;
			sym.fillCol = clr;
		}
		sym.drawSmb(cdc,0,pnt);
	}
	//if (fText)
	//mappane->canvas()->popFont();
	if (0 != fntOld)
		cdc->SelectObject(fntOld);

	cdc->SelectObject(penOld);
	cdc->SelectObject(brOld);
	drawDigCursor();
	return 0;
}

bool PointEditor::OnLButtonDown(UINT nFlags, CPoint point)
{
	mpv->SetFocus();
	Coord crd = mpv->crdPnt(point);
	if (!cb.fUndef() && !cb.fContains(crd)) {
		MessageBeep(-1);
		return false;
	}  
	zPoint pnt = point;
	pnt.x += smb.iSize + 1;
	pnt.y += smb.iSize + 1;
	Coord c1 = mpv->crdPnt(pnt);
	double rPrx = rDist(crd,c1);
	switch (mode) {
	case modeMOVE: {
		long iNr = mp->iRec(crd,rPrx);
		if (iNr < 0) {
			MessageBeep(MB_ICONASTERISK);
			break;
		}  
		Mode(modeMOVING);
		GreyDigitizer(true);
		mode = modeMOVING;
		iActNr = iNr;
		mpv->Invalidate();
	} break;
	case modeADD: 
		iLastButton = -1;
		AddPoint(crd);
		break;
	case modeSELECT: {
		fCtrl = nFlags & MK_CONTROL ? true : false;
		fShft = nFlags & MK_SHIFT ? true : false;
		if (!fCtrl && !fShft)
			DeselectAll();
		if (mpv->tools.size())
			return true;
		if (DragDetect(*mpv, point)) {
			AreaSelector * as = new AreaSelector(mpv, this, (NotifyRectProc)&PointEditor::AreaSelected);
			as->OnLButtonDown(nFlags, point);
			mpv->addTool(as, ID_SELECTAREA);
		}
		else {
			long iNr = mp->iRec(crd,rPrx);
			if (iNr == iUNDEF)
				MessageBeep(MB_ICONASTERISK);
			else {
				if (fCtrl)
					if (fShft)
						aSelect[iNr] = 0;
					else
						aSelect[iNr] = 1;
				else
					aSelect[iNr] = aSelect[iNr] ? 0 : 1;
				mpv->Invalidate();
			}
		}
		break;
		} 
	}  
	return true;
}

class PointCoordForm: public FormWithDest
{
public:
	PointCoordForm(CWnd* parent, const char* sTitle, Coord* crd)
		: FormWithDest(parent, sTitle)
	{
		new FieldReal(root, "&X", &crd->x);
		new FieldReal(root, "&Y", &crd->y);
		SetHelpItem("ilwismen\\point_editor_add_point.htm");
		create();
	}
};

bool PointEditor::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	switch (mode) {
case modeMOVE: 
case modeMOVING:
	{
		Coord crd = mpv->crdPnt(point);
		int iNr = mp->iRec(crd);
		if (iUNDEF == iNr) 
			break;
		Coord crdValue = mp->cValue(iNr);
		PointCoordForm frm(mpv, TR("Edit Point").c_str(), &crdValue);
		if (frm.fOkClicked()) {
			mp->PutVal(iNr, crdValue);
			mpv->Invalidate();
		}
	} return true;
	}
	return DigiEditor::OnLButtonDblClk(nFlags, point);
}

bool PointEditor::OnLButtonUp(UINT nFlags, CPoint point)
{
	Coord crd = mpv->crdPnt(point);
	if (mode == modeMOVING) {
		if (!cb.fUndef() && !cb.fContains(crd)) {
			MessageBeep(-1);
			return 1;
		}  
		Mode(modeMOVE);
		mp->PutVal(iActNr, crd);
		mp->Updated();
		GreyDigitizer(false);
		mpv->Invalidate();
	}
	return true;
}

bool PointEditor::OnMouseMove(UINT nFlags, CPoint point)
{
	Coord crd = mpv->crdPnt(point);
	// put something on status line
	if (0 != nFlags) // something down
		return true; // do not let mappaneview do anything with the mousemove
	else
		return false;
}

int PointEditor::Edit(const Coord& crd)
{
	long iSel = 0;
	long iNr = -1;
	if (crd.fUndef()) {
		for (unsigned long i = 0; i < aSelect.iSize(); ++i)
			if (aSelect[i]) {
				iSel += 1;
				if (iNr == -1)
					iNr = i;  
			}  
			if (iSel == 0)
				return 1;  
	}
	else {
		iNr = mp->iRec(crd);
		if (iUNDEF == iNr) {
			MessageBeep(MB_ICONASTERISK);
			return 1;
		}
		iSel = 1;
	}

	sValue = mp->sValue(iNr,0);
	DomainUniqueID* duid = dm()->pdUniqueID();
	if (iSel == 1) {  // exactly one selected to edit
		crdValue = mp->cValue(iNr);
		if (crdValue.fUndef())
			return 1;
		if (duid) {
			int iRaw = mp->iRaw(iNr);
			EditAttrib(iRaw);
			return 1;
		}
		EditFieldStart(crdValue,sValue);
		return 1;
	}
	if (dm()->pdid()) {
		mpv->MessageBox(TR("Points only individually editable").c_str(), TR("Point Editor").c_str());
		return 1;
	}

	String sRemark(TR("%li points selected.").c_str(), iSel);
	crdValue = crdUNDEF;
	if (AskValue(sRemark, "ilwismen\\point_editor_edit_selection.htm")) {
		removeDigCursor();
		crdDig = Coord();
		for (unsigned long i = 0; i < aSelect.iSize(); ++i) {
			if (aSelect[i]) {
				mp->PutVal(i, sValue);
			}
		}
		mp->Updated();
		mpv->Invalidate();
	}
	return 1;
}

void PointEditor::EditFieldOK(Coord crd, const String& s)
{
	crdValue = crd;
	sValue = s;
	long iNr = mp->iRec(crdValue, 1e-20); // iNr is like iRec (0-offset)
	if (("?" != s) && dm()->pdid()) {
		long iRaw = dm()->iRaw(s);
		if (iUNDEF == iRaw)
			dm()->pdid()->iAdd(s);
		else {
			long iRec = mp->iRec(iRaw);
			if (iNr == iRec)
				iRec = iUNDEF;
			if (iRec >= 0) {
				int iRet = mpv->MessageBox(TR("Value already in use. Use anyway?").c_str(), TR("Point Editor").c_str(),
					MB_YESNO|MB_DEFBUTTON2|MB_ICONASTERISK);
				if (IDYES != iRet) {
					if (iNr != iUNDEF)
						mp->PutVal(iNr, "?");
					EditFieldStart(crd, sValue);
					return;
				}
			}
		}
	}
	if (iNr == iUNDEF)
		iNr = mp->iAddVal(crdValue,s) - 1; // iAddVal returns geometries.size(), iNr must become the index of the newly added item
	else
		mp->PutVal(iNr,s);
	mp->Updated();
	if (aSelect.iSize() < (iNr + 1))
		aSelect &= false;
	removeDigCursor();
	crdDig = Coord();

	if (dm()->pdid()) {
		int iRaw = mp->iRaw(iNr);
		EditAttrib(iRaw);
	}
	mpv->Invalidate();
}

bool PointEditor::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	bool fCtrl = GetKeyState(VK_CONTROL) & 0x8000 ? true : false;
	bool fShift = GetKeyState(VK_SHIFT) & 0x8000 ? true : false;
	switch (nChar) {
case VK_ESCAPE:
	switch (mode) {
case modeADD: 
	Mode(modeSELECT); 
	break;
case modeMOVE: 
	Mode(modeSELECT); 
	break;
case modeSELECT: 
	DeselectAll(); 
	break;
case modeMOVING: 
	Mode(modeMOVE); 
	mpv->Invalidate();
	break;
	}
	return true;
case VK_INSERT:
	if (!fShift && !fCtrl) {
		switch (mode) {
case modeADD: Mode(modeSELECT); break;
case modeSELECT: Mode(modeADD); break;
		}
		return true;
	}
	break;
case VK_DELETE:
	if (!fShift && !fCtrl) {
		OnClear();
		return true;
	}
	break;
case VK_RETURN:
	if (!fShift && !fCtrl) {
		Edit(Coord());
		return true;
	}
	break;
	}
	return false;
}

int PointEditor::DigInit(Coord)
{
	dc = dcCROSS;
	ChangeWindowBasis = (DigiFunc)&PointEditor::DigInit;
	SetDigiFunc(TR("Point Editor"), (DigiFunc)&DigiEditor::MoveCursor,
		(DigiFunc)&PointEditor::AddPoint, TR("Add Point"),
		(DigiFunc)&PointEditor::EditPoint, TR("Move Point"),
		(DigiFunc)&DigiEditor::ChangeWindow, TR("Change Window"),
		(DigiFunc)&PointEditor::EditPointValue, TR("Edit Point"));
	return 0;
}

bool PointEditor::fCopyOk()
{
	long iSel = 0;
	for (unsigned long i = 0; i < aSelect.iSize(); ++i)
		if (aSelect[i]) {
			iSel += 1;
		}
		return iSel > 0;
}

bool PointEditor::fEditOk()
{
	long iSel = 0;
	for (unsigned long i = 0; i < aSelect.iSize(); ++i)
		if (aSelect[i]) {
			iSel += 1;
		}
		if (dm()->pdid()) 
			return iSel == 1;
		else
			return iSel >= 1;
}

bool PointEditor::fPasteOk()
{
	return IsClipboardFormatAvailable(iFmtPnt) ? true : false;
}


void PointEditor::OnUpdateCopy(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(fCopyOk());
}

void PointEditor::OnUpdateEdit(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(fEditOk());
}

void PointEditor::OnUpdatePaste(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(fPasteOk());
}

void PointEditor::OnClear()
{
	unsigned int i;
	long iNr = -1;
	long iSel = 0;
	for (i = 0; i < aSelect.iSize(); ++i) {
		if (aSelect[i]) {
			iSel += 1;
			if (iNr == -1)
				iNr = i;  
		}
	}
	if (iSel == 0)
		return;  
	String s;  
	if (iSel == 1)
		s = TR("Delete Selected Point?");
	else
		s = String(TR("Delete the %li Selected Points?").c_str(), iSel);    
	int iRet = mpv->MessageBox(s.sVal(), TR("Point Editor").c_str(),
		MB_YESNO|MB_ICONQUESTION|MB_DEFBUTTON2);
	bool fDel = (IDYES == iRet) ? true : false;
	bool fChange = false;
	for (i = aSelect.iSize(); i > 0; --i) { // note: i is declared as unsigned int, thus for (i = aSelect.iSize() - 1; i >= 0; --i) doesn't work as expected
		if (aSelect[i-1]) {
			fChange = true;
			if (fDel)
				mp->Delete(i-1);
		}
	}
	if (fChange) {
		aSelect.Reset();
		aSelect.Resize(mp->iFeatures());
		for (i = 0; i < aSelect.iSize(); ++i)
			aSelect[i] = false;
	}
	mpv->Invalidate();
}

void PointEditor::Mode(enumMode Mode) 
{
	if (Mode != modeSELECT)
		DeselectAll();
	mpv->OnNoTool();
	switch (Mode) {
case modeSELECT:
	curActive = curEdit;
	break;
case modeMOVE:
	curActive = curPntMove;
	break;
case modeMOVING:
	curActive = curPntMoving;
	break;
case modeADD:
	curActive = curPntEdit;
	break;
	}
	mode = Mode;
	OnSetCursor();
}

void PointEditor::OnFindUndefs()
{
	if (mode != modeSELECT) 
		Mode(modeSELECT);
	fFindUndefs = !fFindUndefs;
	mpv->Invalidate();
}

void PointEditor::OnUpdateMode(CCmdUI* pCmdUI)
{
	BOOL fCheck;
	switch (pCmdUI->m_nID) {
case ID_SELECTMODE:
	fCheck = modeSELECT == mode;
	break;
case ID_MOVEMODE:
	fCheck = modeMOVE == mode;
	break;
case ID_INSERTMODE:
	fCheck = modeADD == mode;
	break;
case ID_FINDUNDEFS:
	pCmdUI->SetCheck(fFindUndefs);
	return;
	}
	if (0 != mpv->tools.size())
		fCheck = false;
	pCmdUI->SetRadio(fCheck);
}

void PointEditor::OnSelectMode()
{
	Mode(modeSELECT);
}

void PointEditor::OnMoveMode()
{
	Mode(modeMOVE);
}

void PointEditor::OnInsertMode()
{
	Mode(modeADD);
}

void PointEditor::AreaSelected(CRect rect)
{
	Coord c1 = mpv->crdPnt(rect.TopLeft());
	Coord c2 = mpv->crdPnt(rect.BottomRight());
	for (long i = 0; i < mp->iFeatures(); ++i) {
		Coord crd = mp->cValue(i);
		if (crd.fInside(c1,c2)) {
			if (fCtrl)
				if (fShft)
					aSelect[i] = 0;
				else
					aSelect[i] = 1;
			else
				aSelect[i] = aSelect[i] ? 0 : 1;
		}
	}
	mpv->PostMessage(WM_COMMAND, ID_SELECTAREA);
}

void PointEditor::OnAreaSelected()
{
	mpv->changeStateTool(ID_SELECTAREA, false);
	mpv->Invalidate();
}

bool PointEditor::DeselectAll()
{
	bool fChange = false;
	removeDigCursor();
	crdDig = Coord();
	for (unsigned long i = 0; i < aSelect.iSize(); ++i) {
		if (aSelect[i]) {
			fChange = true;
			aSelect[i] = false;
		}
	}
	if (fChange)
		mpv->Invalidate();
	return fChange;
}

int PointEditor::AddPoint(Coord crd)
{
	if (iLastButton == 1)
		return 0;
	DeselectAll();
	if (crd.fUndef())
		return 0;
	crdValue = crd;
	crdDig = Coord();
	if (dm()->pdid())
		sValue = "";
	DomainUniqueID* duid = dm()->pdUniqueID();
	DomainBit* dbit = dm()->pdbit();
	DomainBool* dbool = dm()->pdbool();
	if (duid) {
		int iRaw = duid->iAdd();
		int iNr = mp->iAddRaw(crd,iRaw) - 1;
		aSelect &= false;
		EditAttrib(iRaw);
		mpv->Invalidate();
	}
	else if (dbit || dbool)
		EditFieldOK(crd, "1");
	else    
		EditFieldStart(crd, sValue);
	return 0;
}

void PointEditor::EditAttrib(int iRaw)
{
	Table tbl = mp->tblAtt();
	if (tbl.fValid()) {
		tbl->CheckNrRecs();
		Ilwis::Record rec = tbl->rec(iRaw);
		mpv->ShowRecord(rec);
	}
}

int PointEditor::EditPoint(Coord crd)
{
	DeselectAll();
	if (crd.fUndef())
		return 0;
	zPoint pnt = mpv->pntPos(crd);
	pnt.x += smb.iSize + 1;
	pnt.y += smb.iSize + 1;
	Coord c1 = mpv->crdPnt(pnt);
	double rPrx = rDist(crd,c1);
	long iNr = mp->iRec(crd, rPrx);
	if (iNr == iUNDEF)
		MessageBeep(MB_ICONASTERISK);
	else {
		aSelect[iNr] = true;
		iActNr = iNr;
		EditPointInit(crd);
		mpv->Invalidate();
	}
	return 0;
}

int PointEditor::EditPointInit(Coord crd)
{
	dc = dcPLUS;
	ChangeWindowBasis = (DigiFunc)&PointEditor::EditPointInit;
	SetDigiFunc(TR("Move Point"), (DigiFunc)&DigiEditor::MoveCursor,
		(DigiFunc)&PointEditor::EditPointCrd, TR("New Position"),
		NULL, "",
		(DigiFunc)&DigiEditor::ChangeWindow, TR("Change Window"),
		(DigiFunc)&PointEditor::DigInit, TR("Return"));
	return 0;
}

int PointEditor::EditPointCrd(Coord crd)
{
	crdDig = Coord();
	mp->PutVal(iActNr, crd);
	mp->Updated();
	DigInit(crd);
	mpv->Invalidate();
	return 0;
}

int PointEditor::EditPointValue(Coord crd)
{
	if (iLastButton == 4)
		return 0;
	DeselectAll();
	if (crd.fUndef())
		return 0;
	zPoint pnt = mpv->pntPos(crd);
	pnt.x += smb.iSize + 1;
	pnt.y += smb.iSize + 1;
	Coord c1 = mpv->crdPnt(pnt);
	double rPrx = rDist(crd,c1);
	long iNr = mp->iRec(crd,rPrx);
	if (iNr == iUNDEF)
		MessageBeep(MB_ICONASTERISK);
	else 
	{
		aSelect[iNr] = true;
		iActNr = iNr;
		crd = mp->cValue(iActNr);
		crdDig = Coord();

		DomainUniqueID* duid = dm()->pdUniqueID();
		if (duid) {
			int iRaw = mp->iRaw(iNr);
			EditAttrib(iRaw);
			return 1;
		}
		sValue = mp->sValue(iActNr);
		EditFieldStart(crd, sValue);
		mpv->Invalidate();
	}
	return 0;
}

void PointEditor::OnCopy()
{
	if (!fCopyOk())
		return;
	CWaitCursor curWait;
	if (!mpv->OpenClipboard())
		return;
	EmptyClipboard();

	const int iSIZE = 1000000;
	char* sBuf = new char[iSIZE];
	char* s = sBuf;
	long i;
	int j;
	Coord crd;
	String str, sVal;

	long iSize = 1;
	for (i = 0; i < aSelect.iSize(); ++i)
		if (aSelect[i]) ++iSize;
	IlwisPoint* ip = new IlwisPoint[iSize];
	ip[0].x = rUNDEF;
	ip[0].y = rUNDEF;
	ip[0].iRaw = iSize-1;
	long iTotLen = 0;
	int iLen;
	for (i = 0, j = 1; i < mp->iFeatures(); ++i) { // keep binary structure "ip" compatible with older versions of ILWIS, to exchange points through the clipboard
		if (aSelect[i]) {
			crd = mp->cValue(i);
			ip[j].x = crd.x;
			ip[j].y = crd.y;
			ip[j].iRaw = mp->iRaw(i);
			++j;
			if (iTotLen > iSIZE) 
				continue;
			if (fText) {
				sVal = mp->sValue(i,0);
				str = String("%.3f\t%.3f\t%S\r\n", crd.x, crd.y, sVal);
			}
			else
				str = String("%g\t%g\r\n", crd.x, crd.y);
			iLen = str.length();
			iTotLen += iLen;
			if (iTotLen > iSIZE) 
				continue;
			strcpy(s, str.sVal());
			s += iLen;
		}
	}
	iLen = (1+iSize) * sizeof(IlwisPoint);
	HANDLE hnd = GlobalAlloc(GMEM_MOVEABLE,iLen);
	void* pv = GlobalLock(hnd);
	memcpy(pv, ip, iLen);
	GlobalUnlock(hnd);
	SetClipboardData(iFmtPnt, hnd);

	// Ilwis Domain Format
	IlwisDomain* id = new IlwisDomain(mp->dm(), mp->vr());
	iLen = sizeof(IlwisDomain);
	hnd = GlobalAlloc(GMEM_MOVEABLE,iLen);
	pv = GlobalLock(hnd);
	memcpy(pv, id, iLen);
	GlobalUnlock(hnd);
	SetClipboardData(iFmtDom, hnd);
	delete id;

	// Text Format
	*s = '\0';
	hnd = GlobalAlloc(GMEM_FIXED, strlen(sBuf)+2);
	char* pc = (char*)GlobalLock(hnd);
	strcpy(pc,sBuf);
	GlobalUnlock(hnd);
	SetClipboardData(CF_TEXT,hnd);

	delete [] ip;
	delete [] sBuf;

	CloseClipboard();
}

void PointEditor::OnCut()
{
	OnCopy();
	OnClear();
}

void PointEditor::OnPaste()
{
	if (!fPasteOk()) return;
	unsigned int iSize;
	long i;

	CWaitCursor curWait;
	if (!mpv->OpenClipboard())
		return;

	bool fConvert = false, fValues = false, fSort = false;
	Domain dmMap, dmCb;
	ValueRange vrCb;
	if (IsClipboardFormatAvailable(iFmtDom)) 
	{
		dmMap = mp->dm();

		HANDLE hnd = GetClipboardData(iFmtDom);
		iSize = (unsigned int)GlobalSize(hnd);
		IlwisDomain id;
		if (sizeof(id) < iSize)
			iSize = sizeof(id);
		memcpy(&id, (char*)GlobalLock(hnd),iSize);
		GlobalUnlock(hnd);
		dmCb = id.dm();
		if (dmMap->pdv()) {
			if (0 == dmCb->pdv()) {
				mpv->MessageBox(TR("Data in Clipboard does not have a Value Domain.\nPasting is not possible.").c_str(),TR("Point Editor").c_str(),MB_OK|MB_ICONSTOP);
				CloseClipboard();
				return;
			}
			ValueRange vrMap = mp->vr();
			vrCb = id.vr();
			fValues = true;
			if (vrMap != vrCb)
				fConvert = true;
		}
		else if (dmMap->pdc()) {
			if (0 == dmCb->pdc()) {
				mpv->MessageBox(TR("Data in Clipboard does not have a Class Domain.\nPasting is not possible.").c_str(),TR("Point Editor").c_str(),MB_OK|MB_ICONSTOP);
				CloseClipboard();
				return;
			}
			fSort = true;
			if (dmMap != dmCb)
				fConvert = true;
		}
		else {
			if (dmMap != dmCb)
				fConvert = true;
			if (dmMap->pdsrt())
				fSort = true;
		}
	}
	HANDLE hnd = GetClipboardData(iFmtPnt);
	iSize = (unsigned int)GlobalSize(hnd);
	char* cp = new char[iSize];
	memcpy(cp, (char*)GlobalLock(hnd),iSize);
	IlwisPoint* ip = (IlwisPoint*) cp;
	iSize /= sizeof(IlwisPoint);
	iSize = ip[0].iRaw;

	for (i = 0; i < aSelect.iSize(); ++i)
		aSelect[i] = false;
	for (unsigned int j = 0; j < iSize; ++j) {
		Coord crd (ip[1+j].x, ip[1+j].y, 0); // Paste sets z-coord to 0, just like when manually digitizing new points.
		if (!crd.fUndef()) {
			long iRaw = ip[1+j].iRaw;
			if (fConvert) {
				if (fValues) {
					double rVal;
					if (vrCb.fValid())
						rVal = vrCb->rValue(iRaw);
					else
						rVal = iRaw;
					mp->iAddVal(crd, rVal);
				}
				else {
					String sVal;
					if (vrCb.fValid())
						sVal = vrCb->sValueByRaw(dmCb,iRaw, 0);
					else
						sVal = dmCb->sValueByRaw(iRaw, 0);
					if (fSort) {
						if ("?" == sVal) {
							mp->iAddRaw(crd, iUNDEF);
							continue;
						}
						iRaw = dmMap->iRaw(sVal);
						if (iUNDEF == iRaw) {
							String sMsg(TR("'%S' is not in the domain %S\nAdd this item to the domain?").c_str(), sVal, dmMap->sName());
							int iRet = mpv->MessageBox(sMsg.sVal(),TR("Point Editor").c_str(),MB_YESNOCANCEL|MB_ICONASTERISK);
							if (IDYES == iRet)
								try {
									iRaw = dmMap->pdsrt()->iAdd(sVal);
							}
							catch (ErrorObject& err) {
								err.Show();
								iRaw = iUNDEF;
							}
							else if (IDCANCEL == iRet)
								break;
							else
								continue;
						}
						mp->iAddRaw(crd, iRaw);
					}
					else
						if (dmMap->fValid(sVal))
							mp->iAddVal(crd, sVal);
				}
			}
			else
				mp->iAddRaw(crd, iRaw);
		}
		aSelect &= true;
	}
	mpv->Invalidate();
	CloseClipboard();
}

class PointConfigForm: public FormWithDest
{
public:
	PointConfigForm(CWnd* parent, int* iSize, int* iWidth,
		Color* colDig,
		bool* fText, Color* colText, Color* colBackText,
		Color* col,
		long* Symbol, Color* fillCol,
		HICON* hIcon, Color* colFindUndef)
		: FormWithDest(parent, TR("Customize Point Editor")),
		iSmb(Symbol)
	{
		//      new FieldBlank(root);
		new FieldColor(root, TR("&Color Digitizer Cursor"), colDig);
		CheckBox* cbText = new CheckBox (root, TR("&Show Text"), fText);
		FieldGroup* fg = new FieldGroup(cbText);
		fg->Align(cbText,AL_UNDER);
		new FieldColor(fg, TR("Text &Color"), colText);
		new FieldFillColor(fg, TR("&Background"), colBackText);
		new FieldBlank(root);
		fsmb = new FieldSymbol(root, TR("Symbol &Type"), Symbol, hIcon);
		fsmb->SetCallBack((NotifyProc)&PointConfigForm::FieldSymbolCallBack);
		new FieldInt(root, TR("Symbol &Size"), iSize, ValueRange(1L,100L), true);
		new FieldInt(root, TR("Pen &width"), iWidth, ValueRange(1L,100L), true);
		new FieldColor(root, TR("&Color"), col);
		ffc = new FieldFillColor(root, TR("&Fill Color"), fillCol);
		new FieldColor(root, TR("Find &Undef color"), colFindUndef);
		SetHelpItem("ilwismen\\point_editor_customize.htm");
		create();
	}
private:
	long *iSmb;
	FieldSymbol* fsmb;
	FieldFillColor* ffc;
	int FieldSymbolCallBack(Event*)
	{
		fsmb->StoreData();
		switch (SymbolType(*iSmb)) {
case smbCircle:
case smbSquare:
case smbDiamond:
case smbDeltaUp:
case smbDeltaDown:
	ffc->Show();
	break;
case smbPlus:
case smbMinus:
case smbCross:
	ffc->Hide();
	break;
		}
		return 0;
	}
};

void PointEditor::OnConfigure()
{
	long iSmb = (long) smb.smb;
	PointConfigForm frm(mpv,&smb.iSize,&smb.iWidth,
		&colDig,
		&fText, &colText, &colBack,
		&smb.col, &iSmb, &smb.fillCol, &smb.hIcon, &colFindUndef);
	if (frm.fOkClicked()) {
		smb.smb = (SymbolType) iSmb;
		if (smb.smb == smbDiamond && smb.iWidth % 2 == 0)
			smb.iWidth += 1;
		MapCompositionDoc* mcd = mpv->GetDocument();

		IlwisSettings setSeg("Map Window\\Segment Editor");
		setSeg.SetValue("Cursor Color", colDig);
		IlwisSettings settings("Map Window\\Point Editor");
		settings.SetValue("Show Text", fText);
		settings.SetValue("Text Color", colText);
		settings.SetValue("Background Color", colBack);
		settings.SetValue("Symbol Type", iSmb);
		settings.SetValue("Symbol Size", smb.iSize);
		settings.SetValue("Pen Width", smb.iWidth);
		settings.SetValue("Pen Color", smb.col);
		settings.SetValue("Fill Color", smb.fillCol);
		settings.SetValue("Find Undef Color", colFindUndef);
		mpv->Invalidate();
	}
}

zIcon PointEditor::icon() const
{
	return zIcon("PntIcon");
}

String PointEditor::sTitle() const
{
	String s(TR("Point Editor: %S").c_str(), mp->sName());
	return s;
}

void PointEditor::OnUpdateFileSave(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(mp->fChanged);
}

void PointEditor::OnFileSave()
{
	mp->Store();
	MapCompositionDoc* mcd = mpv->GetDocument();
	mcd->UpdateAllViews(mpv,0);
}

class PointForm: public FormWithDest
{
public:
	PointForm(CWnd* parent, const char* sTitle, Coord* crd, const DomainValueRangeStruct& dvrs, String* s)
		: FormWithDest(parent, sTitle)
	{
		//      if (!crd->fUndef()) {
		new FieldReal(root, "&X", &crd->x);
		new FieldReal(root, "&Y", &crd->y);
		//      }
		//      else
		//	new FieldBlank(root);
		//      FieldString* fs = new FieldString(root, SDUiValue, s);
		FieldVal* fv = new FieldVal(root, TR("&Value"), dvrs, s, true);
		if (dvrs.iWidth() > 12)
			fv->SetWidth(75);
		//      SetHelpItem("ilwis\\point_editor_functionality.htm");
		SetHelpItem("ilwismen\\point_editor_add_point.htm");
		create();
	}
};

void PointEditor::OnAddPoint()
{
	if (dm()->pdid())
		sValue = "";
	PointForm frm(mpv, TR("Add Point").c_str(), &crdValue, dvrs(), &sValue);
	if (!frm.fOkClicked())
		return;
	if (!mp->cb().fContains(crdValue)) {
		mpv->MessageBox(TR("The coordinate lies not inside the map").c_str(), TR("Point Editor").c_str(),
			MB_OK|MB_ICONSTOP);
		return;
	}
	String s = sValue;
	if (frm.fOkClicked()) {
		if (dm()->pdid()) {
			if (mp->iRec(sValue) >= 0) {
				int iRet = mpv->MessageBox(TR("Value already in use. Use anyway?").c_str(), TR("Point Editor").c_str(),
					MB_YESNO|MB_DEFBUTTON2|MB_ICONASTERISK);
				if (IDNO == iRet)
					s = "?";
			}
			else if (iUNDEF == dm()->pdid()->iRaw(s))
				try {
					dm()->pdid()->iAdd(s);
			}
			catch (ErrorObject& err) {
				err.Show();
				return;
			}
		}    
		long iNr = mp->iAddVal(crdValue,s) - 1;
		aSelect &= false;
		mpv->Invalidate();
	}
}

void PointEditor::OnUndoAllChanges()
{
	int iRet = mpv->MessageBox(TR("Undo all changes in Point Map,\nContinue?").c_str(),
		TR("Point Editor").c_str(), MB_ICONQUESTION|MB_OKCANCEL|MB_DEFBUTTON2);
	if (IDOK == iRet) {
		DeselectAll();
		CWaitCursor curWait;
		mp->UndoAllChanges();
		aSelect.Resize(mp->iFeatures()); // make zero based
		for (unsigned int i = 0; i < aSelect.iSize(); ++i)
			aSelect[i] = false;
		Mode(modeSELECT);
		mpv->Invalidate();
	}
}

namespace {
	class EditBoundsForm: public FormWithDest
	{
	public:
		EditBoundsForm(CWnd* wPar, PointMap pntmap, CoordBounds* cb, bool* fAdaptWindow)
			: FormWithDest(wPar, TR("Boundaries"))
			, pm(pntmap)
			, cbRef(cb)
			, fDefaultCalculated(false)
		{
			iImg = IlwWinApp()->iImage(".mpp");

			fcMin = new FieldCoord(root, TR("&Min X, Y"), &cb->cMin);
			fcMax = new FieldCoord(root, TR("&Max X, Y"), &cb->cMax);
			new PushButton(root, TR("&Default"), (NotifyProc)&EditBoundsForm::DefaultButton);
			new CheckBox(root, TR("&Adapt Window"), fAdaptWindow);
			SetHelpItem("ilwismen\\point_editor_bounds_of_map.htm");
			create();
		}
	private:
		void CheckBounds(CoordBounds& cb)
		{
			for (long i = 0; i < pm->iFeatures(); ++i) 
				cb += pm->cValue(i);
		}
		int DefaultButton(Event*)
		{
			if (!fDefaultCalculated)
			{
				CoordBounds cb;
				CheckBounds(cb);
				cbDefault = cb;
				fDefaultCalculated = true;
			}
			fcMin->SetVal(cbDefault.cMin);    
			fcMax->SetVal(cbDefault.cMax);    
			return 0;    
		}
		int exec()
		{
			FormWithDest::exec();
			if (fDefaultCalculated)
				*cbRef += cbDefault;
			else
				CheckBounds(*cbRef);
			return 0;
		}
		PointMap pm;
		FieldCoord *fcMin, *fcMax;
		bool fDefaultCalculated;
		CoordBounds cbDefault, *cbRef;
	};
}

void PointEditor::OnSetBoundaries()
{
	CoordBounds cb = mp->cbGetCoordBounds();
	bool fAdaptWindow = false;
	EditBoundsForm frm(mpv, mp, &cb, &fAdaptWindow);
	if (frm.fOkClicked()) 
	{
		mp->SetCoordBounds(cb);
		if (fAdaptWindow) 
		{
			pane()->GetDocument()->rootDrawer->setCoordBoundsMap(cb); // TODO: cConv / cbConv
			pane()->OnEntireMap();
		}
	}
}

void PointEditor::OnSelectAll()
{
	bool fChange = false;
	removeDigCursor();
	crdDig = Coord();
	for (unsigned long i = 0; i < aSelect.iSize(); ++i) {
		if (!aSelect[i]) {
			fChange = true;
			aSelect[i] = true;
		}
	}
	if (fChange)
		mpv->Invalidate();
}
