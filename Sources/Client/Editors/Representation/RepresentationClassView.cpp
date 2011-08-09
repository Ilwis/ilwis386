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
// RepresentationClassView.cpp : implementation of the RepresentationClassView class
//
#include "Client\Headers\formelementspch.h"
#include "Engine\Map\Segment\Seg.h"
#include "engine\map\polygon\POL.H"
#include "Headers\Hs\Represen.hs"
#include "Headers\Hs\Editor.hs"
#include "Headers\messages.h"
#include "Headers\constant.h"
#include "Headers\Hs\COLORS.hs"
#include "Engine\Base\Algorithm\Random.h"
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Client\Base\ZappToMFC.h"
#include "Client\Base\Res.h"
#include "Engine\Base\AssertD.h"
#include "Client\ilwis.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\Base\Framewin.h"
#include "Engine\Domain\dm.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\Base\BaseView2.h"
#include "Client\Editors\Utils\ColorCB.h"
#include "Client\Editors\Utils\colorScroll.h"
#include "Client\Editors\Utils\colorIntensity.h"
#include "Client\Editors\Utils\ColorGrid.h"
#include "Client\Editors\Utils\sizecbar.h"
#include "Client\Base\ButtonBar.h"
#include "Engine\Representation\Rprclass.h"
#include "Engine\Representation\Rprgrad.h"
#include "Engine\Domain\dmsort.h"
#include "Engine\Domain\dmpict.h"
#include "Client\Editors\Representation\RepresentationDoc.h"
#include "Client\Editors\Representation\RepresentationView.h"
#include "Client\Editors\Representation\RprClassLB.h"
#include "Client\Editors\Representation\ColorIntensityBar.h"
#include "Client\Editors\Representation\ColorGridBar.h"
#include "Client\Editors\Representation\RepresentationClassView.h"
#include "Client\FormElements\fldcolor.h"
#include "Client\FormElements\syscolor.h"
#include "Client\Editors\Utils\SYMBOL.H"
#include "Client\Editors\Utils\Smbext.h"
#include "Client\Editors\Utils\line.h"
#include "Client\Editors\Utils\Pattern.h"
#include "Client\FormElements\fldcol.h"
#include "Client\FormElements\fldrpr.h"
#include "Client\Mapwindow\Positioner.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Client\Editors\Representation\RprEditForms.h"
#include "Client\Base\Framewin.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//---[ RepresentationClassView ]---------------------------------------------------------
IMPLEMENT_DYNCREATE(RepresentationClassView, RepresentationView)

BEGIN_MESSAGE_MAP(RepresentationClassView, RepresentationView)
	//{{AFX_MSG_MAP(RepresentationClassView)
	ON_WM_SIZE()
	ON_COMMAND(ID_NO_OF_CELLS, OnNoOfCells)
	ON_COMMAND(ID_RPREDITITEMS, OnEditMultiple)
	ON_COMMAND(ID_EDIT_SELECT_ALL, OnEditSelectAll)
	ON_COMMAND(ID_EDIT_UNDO_COLOR, OnUndo)
	ON_COMMAND(ID_CONFIGURE, OnCustomize)
	ON_COMMAND(ID_RPRRAS, OnRaster)
	ON_COMMAND(ID_RPRSEG, OnSegment)
	ON_COMMAND(ID_RPRPOL, OnPolygon)
	ON_COMMAND(ID_RPRPNT, OnPoint)
	ON_COMMAND(ID_RPR_REFRESH, OnRefresh)
	ON_COMMAND(ID_COLORGRIDBAR, OnColorGridBar)
	ON_COMMAND(ID_COLORINTENSITYBAR, OnColorIntensityBar)
	ON_COMMAND(ID_RPRCLASSBUTTONBAR, OnRprClassBar)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO_COLOR, OnUpdateEditItem) 
	ON_UPDATE_COMMAND_UI(ID_COLORGRIDBAR, OnUpdateControlBar)
	ON_UPDATE_COMMAND_UI(ID_COLORINTENSITYBAR, OnUpdateControlBar)
	ON_UPDATE_COMMAND_UI(ID_RPRCLASSBUTTONBAR, OnUpdateControlBar)
	ON_UPDATE_COMMAND_UI(ID_RPRRAS, OnUpdateDrawMode)
	ON_UPDATE_COMMAND_UI(ID_RPRPOL, OnUpdateDrawMode)
	ON_UPDATE_COMMAND_UI(ID_RPRSEG, OnUpdateDrawMode)
	ON_UPDATE_COMMAND_UI(ID_RPRPNT, OnUpdateDrawMode)
	ON_UPDATE_COMMAND_UI(ID_RPREDITITEM, OnUpdateEditItem)
	ON_UPDATE_COMMAND_UI(ID_RPREDITITEMS, OnUpdateEditItem)
	ON_UPDATE_COMMAND_UI(ID_CONFIGURE, OnUpdateEditItem)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

RepresentationClassView::RepresentationClassView() 
{
}

RepresentationClassView::~RepresentationClassView()
{
}

int RepresentationClassView::iGetColumnWidth()
{
	if ( tabs.rprClassLB.GetSafeHwnd() != NULL)
	{
		CRect rct;
		tabs.rprClassLB.GetItemRect(0, &rct);
		int iColumnWidth = rct.Width();
		return iColumnWidth;
	}
	return iUNDEF;
}

void RepresentationClassView::OnUndo()
{
	tabs.rprClassLB.Undo();
}

void RepresentationClassView::OnSize( UINT nType, int cx, int cy )
{
	if (tabs.rprClassLB.GetSafeHwnd() != NULL)
		tabs.OnSize(nType, cx, cy);
}

void RepresentationClassView::OnInitialUpdate() 
{
	RepresentationView::OnInitialUpdate();
	RepresentationClassDoc *scrdoc = GetDocument();
	ISTRUE(fINotEqual, scrdoc, (RepresentationClassDoc *)NULL);

	DomainSort *ds = scrdoc->rpr()->dm()->pdsrt(); 
	DomainPicture *dp = scrdoc->rpr()->dm()->pdp();

	iColWidth = scrdoc->prc()->iReadElement("DomainClass", "ColumnWidth");
	if ( iColWidth <= 0 )
	{
		iColWidth = ds ? 100 : 50;
	}

	CFrameWnd *frm = GetParentFrame();

	tabs.Create(this);
	
	AddToFileMenu(ID_FILE_SAVE_COPY_AS);
	AddToFileMenu(ID_RPRDOM);
	AddToFileMenu(ID_PROP);
	AddToFileMenu(DONT_CARE, MF_SEPARATOR);
	AddToFileMenu(ID_FILE_CLOSE);

	AddToEditMenu(ID_EDIT_UNDO_COLOR);
	AddToEditMenu(ID_RPREDITITEMS);
	AddToEditMenu(ID_EDIT_SELECT_ALL);
	
	AddToViewMenu(ID_RPRRAS);
	AddToViewMenu(ID_RPRPOL);
	AddToViewMenu(ID_RPRSEG);
	AddToViewMenu(ID_RPRPNT);
	AddToViewMenu(DONT_CARE, MF_SEPARATOR);
	AddToViewMenu(ID_CONFIGURE);
	AddToViewMenu(ID_NO_OF_CELLS);
	AddToViewMenu(DONT_CARE, MF_SEPARATOR);
	AddToViewMenu(ID_RPR_REFRESH);
	AddToViewMenu(DONT_CARE, MF_SEPARATOR);
	AddToViewMenu(ID_DESCRIPTIONBAR);
	AddToViewMenu(ID_COLORGRIDBAR);
	AddToViewMenu(ID_COLORINTENSITYBAR);
	AddToViewMenu(ID_STATUSLINE);
	AddToViewMenu(ID_RPRCLASSBUTTONBAR);	
	
	AddToHelpMenu(ID_HLPKEY);			// F1: Context help
	AddToHelpMenu(ID_HLPCONTENTS);	// F11: Contents
	AddToHelpMenu(DONT_CARE, MF_SEPARATOR);
	AddToHelpMenu(ID_APP_ABOUT);  //, "&About ILWIS...");

	FrameWindow* frmw = static_cast<FrameWindow*>(frm);
	frmw->SetAcceleratorTable();
	frmw->SetWindowName("RepresentationClassEditor");

	long iRaw = ds ? ds->iKey(1) : 0;
	Color clr = Color(scrdoc->rpr()->clrRaw(iRaw));

	CRect rct;
	frm->GetWindowRect(&rct);
	frm->MoveWindow(rct.left, rct.top, 440, 440);
	frm->GetWindowRect(&rct);
	csBar.Create(this);
	ciBar.Create(this, clr);
	m_bbTools.Create(frm, "rprclass.but", "Representation editor", ID_RPRCLASSBUTTONBAR);	

	frm->EnableDocking(CBRS_ALIGN_ANY);
	frm->DockControlBar(&csBar, AFX_IDW_DOCKBAR_LEFT, &CRect(rct.left, rct.top , rct.left + iBARWIDTH + 20 ,rct.top + iBARWIDTH ));
	frm->DockControlBar(&ciBar, AFX_IDW_DOCKBAR_LEFT, &CRect(rct.left,rct.top + iBARWIDTH, rct.left + iBARWIDTH + 20, rct.top + 3 * iBARWIDTH)); //AFX_IDW_DOCKBAR_LEFT);
	frm->DockControlBar(&m_bbTools, AFX_IDW_DOCKBAR_TOP);

	frm->RecalcLayout();
	tabs.rprClassLB.InitUndo();
}

void RepresentationClassView::OnNoOfCells()
{
	csBar.colGrid.OnNoOfCells();
}


class  CustomizeForm: public FormWithDest
{
public:
	CustomizeForm(CWnd* wPar, const Domain& dom, int *iState, int* w);
	int Check(Event *);
	FormEntry* feDefaultFocus();

private:
	  
	FieldInt* fi;
	RadioButton *rb, *rb1, *rb2;
	const Domain& dm;
};

CustomizeForm::CustomizeForm(CWnd* wPar, const Domain& dom, int *iState, int* w)
: FormWithDest(wPar, TR("Customize")),
  dm(dom)
{
	new FieldBlank(root);
	RadioGroup* rgNames = new RadioGroup(root, TR("Display Name/Code"), iState, true);
	rb =  new RadioButton(rgNames, TR("Display Name"));
	rb->Align(rgNames, AL_UNDER);
	rb1 = new RadioButton(rgNames, TR("Display Name and Code"));
	rb2 = new RadioButton(rgNames, TR("Display Code"));
	new FieldBlank(root);
	fi = new FieldInt(root, TR("Column &Width"), w, ValueRange(20L,250L));
	fi->SetCallBack((NotifyProc)& CustomizeForm::Check);
	SetMenHelpTopic("ilwismen\\representation_class_editor_column_width.htm");
	create();
} 

int CustomizeForm::Check(Event *)
{
	fi->StoreData();
	if ( !dm->pdsrt()->fCodesAvailable() )
	{
			rb1->Disable();
			rb2->Disable();
	}
	if ( fi->iVal() < 20 || fi->iVal() >250)
		DisableOK();
	else
		EnableOK();
	return 0;
}

FormEntry* CustomizeForm::feDefaultFocus()
{ return fi; }

void RepresentationClassView::OnCustomize()
{

	int w = iColWidth;
	RepresentationClassDoc *scrdoc=GetDocument();
	int iNameState = scrdoc->prc()->iReadElement("DomainClass", "NameState");
	iNameState = max(0, iNameState);
	CustomizeForm frm(this, GetDocument()->rpr()->dm(), &iNameState, &w);
	if (frm.fOkClicked()) 
	{
    if (w >= 20 && w <= 250) 
	{
		tabs.rprClassLB.SetColumnWidth(w);
		tabs.rprClassLB.SetNameDisplayState((RepresentationClassLB::ndNameDisplay)iNameState);
		iColWidth = w;
		RepresentationClassDoc *scrdoc=GetDocument();
    	ISTRUE(fINotEqual, scrdoc, (RepresentationClassDoc *)NULL);
		scrdoc->prc()->WriteElement("DomainClass", "ColumnWidth", iColWidth);
		scrdoc->prc()->WriteElement("DomainClass", "NameState", (long)iNameState);
    }  
  }
}

void RepresentationClassView::OnDraw(CDC* pDC)
{
	RepresentationDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

void RepresentationClassView::OnRaster()
{
	tabs.rprClassLB.SetDrawMode(RepresentationClassLB::opRAS);
	tabs.SetCurSel(RepresentationClassLB::opRAS);
}

void RepresentationClassView::OnPolygon()
{
	tabs.rprClassLB.SetDrawMode(RepresentationClassLB::opPOL);
	tabs.SetCurSel(RepresentationClassLB::opPOL)	;
}

void RepresentationClassView::OnSegment()
{
	tabs.rprClassLB.SetDrawMode(RepresentationClassLB::opSEG);
	tabs.SetCurSel(RepresentationClassLB::opSEG);	
}

void RepresentationClassView::OnPoint()
{
	tabs.rprClassLB.SetDrawMode(RepresentationClassLB::opPNT);
	tabs.SetCurSel(RepresentationClassLB::opPNT);	
}

void RepresentationClassView::OnUpdateDrawMode(CCmdUI *cmd)
{
	cmd->SetCheck(0);
	if ( cmd->m_nID == ID_RPRRAS && tabs.rprClassLB.GetDrawMode() == RepresentationClassLB::opRAS )
		cmd->SetCheck();
	if ( cmd->m_nID == ID_RPRPOL && tabs.rprClassLB.GetDrawMode() == RepresentationClassLB::opPOL )
		cmd->SetCheck();
	if ( cmd->m_nID == ID_RPRSEG && tabs.rprClassLB.GetDrawMode() == RepresentationClassLB::opSEG )
		cmd->SetCheck();
	if ( cmd->m_nID == ID_RPRPNT && tabs.rprClassLB.GetDrawMode() == RepresentationClassLB::opPNT )
		cmd->SetCheck();
}

void RepresentationClassView::OnEditSelectAll()
{
	tabs.rprClassLB.SelItemRange(TRUE, 0, tabs.rprClassLB.GetCount());
}

void RepresentationClassView::OnEditSingleItem()
{
	if (GetDocument()->rpr()->fDataReadOnly()) 
	{
		MessageBeep(MB_ICONASTERISK);
		return;
	}

	int id = tabs.rprClassLB.GetCurSel();
	DomainSort *ds = GetDocument()->rpr()->dm()->pdsrt();
	long iRaw = ds ? ds->iKey(id + 1) : id;

	RepresentationClass *rpc = GetDocument()->prc();
	Color clrUndo = rpc->clrRaw(iRaw);  // Get current displayed color
	
	FormBaseDialog* frm = 0;
	switch (tabs.rprClassLB.GetDrawMode())
	{
		case RepresentationClassLB::opRAS:
			frm = new MapRprEditForm(this, rpc, iRaw, true);
			break;
		case RepresentationClassLB::opSEG:
			frm = new SegmentMapRprEditForm(this, rpc, iRaw, true);
			break;
		case RepresentationClassLB::opPOL:
			frm = new PolygonMapRprEditForm(this, rpc, iRaw, true);
			break;
		case RepresentationClassLB::opPNT:
			frm = new PointMapRprEditForm(this, rpc, iRaw, true);
			break;
	}
	if (frm->fOkClicked()) 
	{
		tabs.rprClassLB.Invalidate();
		tabs.rprClassLB.SetUndo(id, clrUndo);
		csBar.Invalidate();
		// ensure that new color is drawn
		tabs.rprClassLB.OnSelChange();
	}  
	tabs.rprClassLB.SetFocus();
	delete frm;
}

void RepresentationClassView::OnColorGridBar()
{
	CWnd * wnd = csBar.GetOwner();
	CWnd * wnd2 = csBar.GetParent();
	if ( csBar.GetSafeHwnd())
	{
		if (csBar.IsWindowVisible())
			GetParentFrame()->OnBarCheck(ID_COLORGRIDBAR);
		else
			GetParentFrame()->OnBarCheck(ID_COLORGRIDBAR);
	}
}

void RepresentationClassView::OnColorIntensityBar()
{

	if ( ciBar.GetSafeHwnd())
	{
		if (ciBar.IsWindowVisible())
			GetParentFrame()->OnBarCheck(ID_COLORINTENSITYBAR);
		else
			GetParentFrame()->OnBarCheck(ID_COLORINTENSITYBAR);
	}
}

void RepresentationClassView::OnRprClassBar()
{
	GetParentFrame()->OnBarCheck(ID_RPRCLASSBUTTONBAR);
}

void RepresentationClassView::OnUpdateControlBar(CCmdUI* pCmdUI)
{
	GetParentFrame()->OnUpdateControlBarMenu(pCmdUI);
}

// Form classes
class MultipleRprItemForm: public FormWithDest
{
public:
  MultipleRprItemForm(CWnd* wPar,  RepresentationClassLB::DrawMode mode, const Color& c1, const Color& c2, int *iSize)
  : FormWithDest(wPar, TR("Edit Multiple Items"))
  {
    iOption = 1;
    fVariation = false;
    iVariation = 50;
    col1 = c1;
    col2 = c2;
    RadioGroup* rg = new RadioGroup(root, "", &iOption);
    RadioButton* rbSingle = new RadioButton(rg, TR("&Single Color"));
    RadioButton* rbRange = new RadioButton(rg, TR("Color &Range"));

    FieldGroup* fgSingle = new FieldGroup(rbSingle);
    fgSingle->Align(rg, AL_UNDER);
    new FieldColor(fgSingle, TR("&Color"), &col2);

    FieldGroup* fgRange = new FieldGroup(rbRange);
    fgRange->Align(rg, AL_UNDER);
    new FieldColor(fgRange, TR("&From Color"), &col1);
    new FieldColor(fgRange, TR("&To Color"), &col2);

    CheckBox* cbVar = new CheckBox(root, TR("&Variation"), &fVariation);
    cbVar->Align(fgRange, AL_UNDER);
    new FieldInt(cbVar, "", &iVariation, ValueRange(1,255));

		if ( mode == RepresentationClassLB::opPNT) 
		{
			FieldInt *fi = new FieldInt(root, "Symbol size", iSize);
			fi->Align(cbVar, AL_UNDER);
		}

    SetMenHelpTopic("ilwismen\\representation_class_editor_edit_multiple_items.htm");
		seedrand(30);
    create();
  };
  Color colRange(int iNr, int iSel) {
    Color clr, c2;
    switch (iOption) {
      case 0:
        clr = (Color) col2;
        break;
      case 1:
        iSel -= 1;
        clr = (Color) col1;
        c2 = (Color) col2;
        clr.red() += (c2.red() - clr.red()) * iNr / iSel;
        clr.green() += (c2.green() - clr.green()) * iNr / iSel;
        clr.blue() += (c2.blue() - clr.blue()) * iNr / iSel;
        break;
    }
    if (fVariation)
      clr = colVar(clr);
    return clr;
  }
private:
  int iVar(int iVal) {
    int iRes;
		srand(time(NULL));
    do {
      int iRnd = (int)( 2 * iVariation * (double)rand()/RAND_MAX - iVariation);
      iRes = iRnd + iVal;
    } while (iRes < 0 || iRes > 255);
    return iRes;
  }
  Color colVar(Color col) {
    col.red() = iVar(col.red());
    col.green() = iVar(col.green());
    col.blue() = iVar(col.blue());
    return col;
  }
  int iOption, iVariation;
  bool fVariation;
  Color col1, col2;
};

void RepresentationClassView::OnEditMultiple()
{
	RepresentationClass *rpc = GetDocument()->prc();
  if (rpc->fDataReadOnly()) {
    MessageBeep(MB_ICONASTERISK);
    return ;
  }
  int iSel = tabs.rprClassLB.GetSelCount();
	if ( iSel == 1 )
	{
		OnEditSingleItem();
		return;
	}
  int* iArr = new int[iSel];
  iSel = tabs.rprClassLB.GetSelItems(iSel, iArr);

  int id = tabs.rprClassLB.GetCurSel();
  long iRaw1, iRaw2;
	DomainSort *ds = rpc->dm()->pdsrt();
  if (ds)
	{
    iRaw1 = ds->iKey(iArr[0] + 1);
    iRaw2 = ds->iKey(iArr[iSel - 1] + 1);		
	}		
  else 
	{
    iRaw1 = id; 
    iRaw2 = id + iSel;   
	}		
  Color col1, col2;
  if (tabs.rprClassLB.GetDrawMode() == RepresentationClassLB::opPNT)
	{
    col1 = rpc->clrSymbol(iRaw1);
    col2 = rpc->clrSymbol(iRaw2);		
	}		
  else
	{
    col1 = rpc->clrRaw(iRaw1);
    col2 = rpc->clrRaw(iRaw2);			
	}		

 	
	RepresentationClassLB::DrawMode mode = tabs.rprClassLB.GetDrawMode();
	int iItemSize = rpc->iSymbolSize(iRaw1);
  MultipleRprItemForm frm(this, mode, col1, col2, &iItemSize);
	long iRaw;
  if (frm.fOkClicked()) 
	{
    for (int i = 0; i < iSel; ++i) 
		{
      if (ds)
        iRaw = ds->iKey(iArr[i]+1);
      else
        iRaw = iArr[i];
      Color clr = frm.colRange(i, iSel);
      if (mode == RepresentationClassLB::opPNT)
			{
        rpc->PutSymbolColor(iRaw,clr);
				rpc->PutSymbolSize(iRaw, iItemSize);
			}				
      else
        rpc->PutColor(iRaw,clr);
    }
		csBar.Invalidate();
    tabs.rprClassLB.Invalidate();
  }
  delete [] iArr;
  return ;
}

void RepresentationClassView::OnUpdateEditItem(CCmdUI *cmd)
{
	cmd->Enable(TRUE);
	if (cmd->m_nID == ID_RPREDITITEM || cmd->m_nID == ID_RPREDITITEMS || cmd->m_nID == ID_RPRWIDTH)
  {
		if ( GetDocument()->fReadOnly())
			cmd->Enable(FALSE);
	}
	if ( cmd->m_nID == ID_RPREDITITEM )
	{
		if ( tabs.rprClassLB.GetSelCount() != 1 )
			cmd->Enable(FALSE);
	}
	if ( cmd->m_nID == ID_RPREDITITEMS )
		if ( tabs.rprClassLB.GetSelCount() < 1 )
			cmd->Enable(FALSE);
  if ( cmd->m_nID == ID_EDIT_UNDO_COLOR )
		if ( !tabs.rprClassLB.fCanUndo())
			cmd->Enable(FALSE);
}

void RepresentationClassView::LoadState(IlwisSettings& settings)
{
	String sBarKey = String("%S\\ToolBars\\Bars", settings.sIlwisSubKey());
	csBar.LoadSettings(sBarKey);
	//csBar.LoadState(sBarKey.c_str());
	//ciBar.LoadState(sBarKey.c_str());
}

void RepresentationClassView::SaveState(IlwisSettings& settings)
{
	String sBarKey = String("%S\\ToolBars\\Bars", settings.sIlwisSubKey());
	csBar.SaveSettings(sBarKey);
	//csBar.SaveState(sBarKey.c_str());
	//ciBar.SaveState(sBarKey.c_str());
}

void RepresentationClassView::OnRefresh()
{
	tabs.rprClassLB.Refresh();
}

//-------------------------------------------------------------------------------------------
#ifdef _DEBUG
void RepresentationClassView::AssertValid() const
{
	RepresentationView::AssertValid();
}

void RepresentationClassView::Dump(CDumpContext& dc) const
{
	RepresentationView::Dump(dc);
}

RepresentationClassDoc* RepresentationClassView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(RepresentationClassDoc)));
	return dynamic_cast<RepresentationClassDoc *>(m_pDocument);
}
#endif //_DEBUG

