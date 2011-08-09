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
// Editor.cpp: implementation of the Editor class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Engine\Map\Segment\Seg.h"
#include "Engine\Map\Polygon\POL.H"
#include "Client\ilwis.h"
#include "Client\Mapwindow\Positioner.h"
#include "Client\Editors\Editor.h"
#include "Headers\constant.h"
#include "Client\Editors\Map\EditField.h"
#include "Engine\Domain\Dmvalue.h"
#include "Engine\Domain\dmsort.h"
#include "Headers\Hs\Editor.hs"
#include "Client\FormElements\fldval.h"
#include "Client\Base\datawind.h"
#include "Client\Mapwindow\MapWindow.h"
#include "Headers\Htp\Ilwis.htp"
#include "Headers\htmlhelp.h"
#include "Client\Help\ChmFinder.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


BEGIN_MESSAGE_MAP(Editor, CCmdTarget)
	//{{AFX_MSG_MAP(Editor)
	ON_COMMAND(ID_EXIT, OnExit)
	ON_COMMAND(ID_EXITEDITOR, OnExit)
	ON_COMMAND(ID_HLPKEY, OnHelp)
	ON_COMMAND(ID_HELP_RELATED_TOPICS, OnRelatedTopics)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



Editor::Editor(MapPaneView* mappaneview)
: mpv(mappaneview)
, wEditField(0)
{
	fOk = true;
	hmenFile = 0;
	hmenEdit = 0;
	drw = 0;
	help = "ilwis\\map_window.htm";
	sHelpKeywords = "Map Window";
}

Editor::~Editor()
{
	delete wEditField; // to prevent that destructor of EditField accesses already deleted Editor later
	MapWindow* mw = mpv->mwParent();
	if (mw)
		mw->UpdateMenu();
	DestroyMenu(hmenFile);
	DestroyMenu(hmenEdit);
}

Domain Editor::dm() const
{
	return dvs.dm();
}

Representation Editor::rpr() const
{
	return _rpr;
}

DomainValueRangeStruct Editor::dvrs() const
{
	return dvs;
}


bool Editor::OnSetCursor() // called by MapPaneView::OnSetCursor
{
	bool fSetCursor = (HCURSOR)0 != curActive;
	if (fSetCursor)
		SetCursor(curActive);
	return fSetCursor;
}

void Editor::OnExit()
{
	mpv->PostMessage(WM_COMMAND, ID_NONEEDIT, 0);
}

void Editor::OnEdit()
{
  Coord c = crdUNDEF;
	Edit(c);
}
 
bool Editor::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	return false;
}

bool Editor::OnLButtonDown(UINT nFlags, CPoint point)
{
	return false;
}

bool Editor::OnLButtonUp(UINT nFlags, CPoint point)
{
	return false;
}

bool Editor::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	Coord crd = mpv->crdPnt(point);
	Edit(crd);
	return true;
}

bool Editor::OnMouseMove(UINT nFlags, CPoint point)
{
	return false;
}

void Editor::EditFieldStart(Coord crd, const String& s)
{
  Domain _dm = dvs.dm();
  if (!_dm.fValid())
    return;
  DomainClass* dc = _dm->pdc();
  DomainBit* dbit = _dm->pdbit();
  DomainBool* dbool = _dm->pdbool();
  if (dc) 
    wEditField = new EditFieldClass(this, crd, dc, s);
  else if (dbit) {
    long iRaw = dbit->iRaw(s);
    if (++iRaw > 1)
      iRaw = 0;
    String sRes = dbit->sValueByRaw(iRaw,0);
    EditFieldOK(crd, sRes);
  }
  else if (dbool) {
    long iRaw = dbool->iRaw(s);
    if (iUNDEF == iRaw)
      iRaw = 0;
    if (++iRaw > 2)
      iRaw = 0;
    String sRes = dbool->sValueByRaw(iRaw,0);
    EditFieldOK(crd, sRes);
  }
  else   
    wEditField = new EditField(this, crd, dvs, s);
}

void Editor::EditFieldOK(Coord, const String&)
{
  // empty
}

void Editor::StartBusy()
{
}

void Editor::EndBusy()
{
}  

int Editor::AskValue(const String& sRemark, unsigned int htp)
{
  class AskValueForm: public FormWithDest
  {
  public:
    AskValueForm(CWnd* parent, const String& sRemark, const String& sQuestion, 
                 DomainValueRangeStruct& dvrs, String* sVal, unsigned int htp)
    : FormWithDest(parent, TR("Edit"))
    {
      StaticText* st = new StaticText(root, sRemark);
      st->SetIndependentPos();
      FieldVal* fv = new FieldVal(root, sQuestion, dvrs, sVal, true);
      if (dvrs.dm()->iWidth() > 12)
        fv->SetWidth(75);
      if (htp)  
        SetMenHelpTopic(help);
      create();
    }
  };
  String sVal = sValue;
  StartBusy();
  DomainSort* ds = dm()->pdsrt();
	String sQuestion = TR("&Value");
	if (ds) {
		if (ds->pdc())
			sQuestion = TR("&Class Name");
		else
			sQuestion = TR("&ID");
	}

	int iNrItems;
	if (ds)					 
		iNrItems = ds->iNettoSize();
retry:  
  {
    AskValueForm frm(pane(), sRemark, sQuestion, dvrs(), &sVal, htp);
		if (ds && iNrItems < ds->iNettoSize()) {
			mpv->CView::GetDocument()->UpdateAllViews(mpv,5); // only update layer view
			iNrItems = ds->iNettoSize();
		}
    if (!frm.fOkClicked()) {
      EndBusy();
      return 0;
    }  
  }    
	if ("?" == sVal) {
		// do nothing, allow undef
	}
  else if (ds) {
    try {
      if (ds->iRaw(sVal) == iUNDEF) {
        ds->iAdd(sVal);
			}
			if (iNrItems < ds->iNettoSize())
				mpv->CView::GetDocument()->UpdateAllViews(mpv,5); // only update layer view
    }
    catch (ErrorObject& err) {
      err.Show();
      goto retry;
    }
  }
  else if (!dvrs().fValid(sVal)) {
    String sMsg(TR("'%S' is not a valid value").c_str(), sVal);
    int iRet = pane()->MessageBox(sMsg.sVal(), TR("Editor").c_str(), MB_ICONEXCLAMATION|MB_OKCANCEL);
    if (IDCANCEL == iRet) {
      EndBusy();
      return 0;
    }
    goto retry;
  }
  sValue = sVal;
  EndBusy();
  return 1;
}

int Editor::Edit(const Coord&)
{
  return 0;
}

void Editor::PreDraw()
{
	// empty
}

bool Editor::OnContextMenu(CWnd* pWnd, CPoint point)
{
	return false;
}

void Editor::UpdateMenu()
{
	MapWindow* mw = mpv->mwParent();
	if (0 == mw)
		return;
	mw->UpdateMenu(hmenFile, hmenEdit);
}

LRESULT Editor::OnUpdate(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

void Editor::OnHelp() 
{
	IlwWinApp()->showHelp(help);
}

void Editor::OnRelatedTopics()
{
//	HH_AKLINK link;
//	link.cbStruct =     sizeof(HH_AKLINK);
//	link.fReserved =    FALSE;
//	link.pszKeywords =  sHelpKeywords.c_str(); 
//	link.pszUrl =       NULL; 
//	link.pszMsgText =   NULL; 
//	link.pszMsgTitle =  NULL; 
//	link.pszWindow =    NULL;
//	link.fIndexOnFail = TRUE;
//
//	String sHelpFile (ChmFinder::sFindChmFile("ilwis.chm"));
//	HtmlHelp(GetDesktopWindow(), sHelpFile.sVal(), HH_KEYWORD_LOOKUP, (DWORD)&link);
}

void Editor::PreSaveState()
{
// do nothing
}
