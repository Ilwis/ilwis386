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
// DigiEditor.cpp: implementation of the DigiEditor class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Engine\Map\Segment\Seg.h"
#include "Engine\Map\Polygon\POL.H"
#include "Client\Mapwindow\Positioner.h"
#include "Client\Editors\Editor.h"
#include "Client\Editors\Digitizer\DigiEditor.h"
#include "Headers\Hs\Editor.hs"
#include "Client\Editors\Digitizer\DIGITIZR.H"
#include "Client\Editors\Digitizer\DIGINFO.H"
#include "Client\ilwis.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Headers\constant.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(DigiEditor, Editor)
	//{{AFX_MSG_MAP(DigiEditor)		
	ON_COMMAND(ID_DIGACTIVE, OnInfoWindow)
  ON_UPDATE_COMMAND_UI(ID_DIGACTIVE, OnUpdateInfoWindow)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


const int iSize = 11;


DigiEditor::DigiEditor(MapPaneView* mv, const CoordBounds& crdbnd)
: Editor(mv)
{
	cb = crdbnd;
  double rGrid = max(cb.width(), cb.height());
  rGrid /= 20;        // tolerate 5 % at all sides
  cb.MinX() -= rGrid;
  cb.MaxX() += rGrid;
  cb.MinY() -= rGrid;
  cb.MaxY() += rGrid;
  fActDigitizer = false;

	Digitizer* dig = IlwWinApp()->dig();
	if (dig)
	{
		dig->Enable();
    fActDigitizer = dig->fActive();
	}
  if (fActDigitizer)
    dig->Activate();

	fFocus = true;
  fGreyDigitizer = false;
  iLastButton = 0;

  info = new DigitizerInfoWindow();
	info->Create(mpv);
  info->ShowWindow(fActDigitizer ? SW_SHOW : SW_HIDE);

	CClientDC cdc(mpv);
	CBitmap bm;
	bm.CreateCompatibleBitmap(&cdc,iSize,iSize);
	dcBG.CreateCompatibleDC(&cdc);
	dcBG.SelectObject(&bm);
	bm.Detach();

  dc = dcCROSS;
  SetDigiFunc("", (DigiFunc)&DigiEditor::MoveCursor,
	      NULL, "", NULL, "", NULL, "", NULL, "");
	      
  String fn = IlwWinApp()->Context()->fnUserINI().sFullName();
  char sBuf[80];
  colDig = Color(0,0,0); // default black	      
  String sVal = String("%lx", (long)colDig);
  GetPrivateProfileString("DigitizerInfo", "Cursor Color", sVal.sVal(), sBuf, 79, fn.sVal());
  sscanf(sBuf,"%lx",&colDig);
	IlwisSettings settings("Map Window\\Segment Editor");
	colDig = settings.clrValue("Cursor Color", colDig);
}

DigiEditor::~DigiEditor()
{
	Digitizer* dig = IlwWinApp()->dig();
	if (dig)
		dig->Disable();
	if (IsWindow(info->m_hWnd))
		info->DestroyWindow();
}

void DigiEditor::drawDigCursor()
{
  if (!fActDigitizer || crdDig.fUndef() ||
      fGreyDigitizer || !fFocus)
    return;
  int iShft = iSize / 2;

  zPoint pnt = mpv->pntPos(crdDig);
  zRect rect;
  mpv->GetClientRect(&rect);
  if (!rect.PtInRect(pnt))
    return;

	CClientDC cdc(mpv);

	dcBG.BitBlt(0,0,iSize,iSize,&cdc,pnt.x-iShft,pnt.y-iShft,SRCCOPY);

	CPen pen(PS_SOLID,0,colDig);
	CPen* penOld = cdc.SelectObject(&pen);
	CGdiObject* brOld;
  switch (dc) {
    case dcCROSS:
      cdc.MoveTo(pnt.x-iShft,pnt.y-iShft);
      cdc.LineTo(pnt.x+iShft+1,pnt.y+iShft+1);
      cdc.MoveTo(pnt.x-iShft,pnt.y+iShft);
      cdc.LineTo(pnt.x+iShft+1,pnt.y-iShft-1);
      break;
    case dcPLUS:
      cdc.MoveTo(pnt.x-iShft,pnt.y);
      cdc.LineTo(pnt.x+iShft+1,pnt.y);
      cdc.MoveTo(pnt.x,pnt.y-iShft);
      cdc.LineTo(pnt.x,pnt.y+iShft+1);
      break;
    case dcBOX:
      iShft -= 2;
			brOld = cdc.SelectStockObject(HOLLOW_BRUSH);
      cdc.Rectangle(pnt.x-iShft,pnt.y-iShft,
		     pnt.x+iShft+1,pnt.y+iShft+1);
			cdc.SelectObject(brOld);
      cdc.MoveTo(pnt.x,pnt.y);
      cdc.LineTo(pnt.x,pnt.y+1);
      break;
  }
	cdc.SelectObject(penOld);
}

void DigiEditor::removeDigCursor()
{
  if (!fActDigitizer || crdDig.fUndef() || 
      fGreyDigitizer || !fFocus)
    return;

	CClientDC cdc(mpv);
  zPoint pnt = mpv->pntPos(crdDig);
  zRect rect;
  mpv->GetClientRect(&rect);
  if (!rect.PtInRect(pnt))
    return;

  int iShft = iSize / 2;
	cdc.BitBlt(pnt.x-iShft,pnt.y-iShft,iSize,iSize,&dcBG,0,0,SRCCOPY);
}

int DigiEditor::MoveCursor(Coord crd)
{
  if (!crd.fUndef() && !cb.fUndef() && !cb.fContains(crd)) {
    MessageBeep(-1);
    return 0;
  }  
  crdDig = crd;
  return 0;
}

int DigiEditor::SetDigiFunc(String sTitle, DigiFunc df0,
		DigiFunc df1, String sdf1,
		DigiFunc df2, String sdf2,
		DigiFunc df3, String sdf3,
		DigiFunc df4, String sdf4)
{
  info->SetWindowText(sTitle.c_str());
  df[0] = df0;
  df[1] = df1;
  df[2] = df2;
  df[3] = df3;
  df[4] = df4;

  info->button(1)->SetWindowText(sdf1.c_str());
  info->button(2)->SetWindowText(sdf2.c_str());
  info->button(3)->SetWindowText(sdf3.c_str());
  info->button(4)->SetWindowText(sdf4.c_str());

  return 0;
}

int DigiEditor::ChangeWindow(Coord c)
{
  if (iLastButton == 3)
    return 1;
  dc = dcCROSS;
  cLast = cPivot = c;
  return SetDigiFunc(TR("Change Window"),
		     (DigiFunc)&DigiEditor::ChangeWindowMove,
		     (DigiFunc)&DigiEditor::ChangeWindowEntireMap, TR("Entire Map"),
		     (DigiFunc)&DigiEditor::ChangeWindowFirstPoint, TR("1st Point"),
		     (DigiFunc)&DigiEditor::ChangeWindowExec, TR("2nd Point"),
		     (DigiFunc)&DigiEditor::ChangeWindowQuit, TR("Return"));
}

int DigiEditor::ChangeWindowFirstPoint(Coord c)
{
  ChangeWindowMove(cPivot);
  cLast = cPivot = c;
  return 0;
}

int DigiEditor::ChangeWindowEntireMap(Coord c)
{
  crdDig = Coord();
  ChangeWindowMove(cPivot);
  mpv->OnEntireMap();
  return ChangeWindowQuit(c);
}

int DigiEditor::ChangeWindowMove(Coord c)
{

  zPoint pLast = mpv->pntPos(cLast);
  zPoint p = mpv->pntPos(c);
  if (pLast != p) {	
		CClientDC dc(mpv);
		CGdiObject* brOld = dc.SelectStockObject(HOLLOW_BRUSH);
		dc.SetROP2(R2_NOT);
    zPoint pPivot = mpv->pntPos(cPivot);
    if (pPivot != pLast) {
			CRect rect(pPivot, pLast);
			dc.Rectangle(&rect);
		}
    pLast = p;
    cLast = c;
    if (pPivot != pLast) {
			CRect rect(pPivot, pLast);
			dc.Rectangle(&rect);
		}
		dc.SetROP2(R2_COPYPEN);
		dc.SelectObject(brOld);
  }

  return MoveCursor(c);
}

int DigiEditor::ChangeWindowQuit(Coord c)
{
  ChangeWindowMove(cPivot);
  return (this->*ChangeWindowBasis)(c);
  //return StartDig();
}

int DigiEditor::ChangeWindowExec(Coord c)
{
  if (iLastButton == 3)
    return ChangeWindowMove(c);

  ChangeWindowMove(cPivot);
  MoveCursor(Coord());
	CRect rect;
	rect.TopLeft() = mpv->pntPos(cPivot);
	rect.BottomRight() = mpv->pntPos(c);
	mpv->AreaSelected(rect);

  return ChangeWindowQuit(c);
}

void DigiEditor::GreyDigitizer(bool fGrey)
{
  removeDigCursor();
  fGreyDigitizer = fGrey;

	if (0 == info || !IsWindow(info->m_hWnd))
		return;
	info->title()->EnableWindow(!fGreyDigitizer);
  for (int i = 1; i <= 4; ++i)
    info->button(i)->EnableWindow(!fGreyDigitizer);
}

int DigiEditor::iDigiFunc(int iButton, Coord crd)
{
  if (!cb.fUndef() && !cb.fContains(crd)) {
    MessageBeep(-1);
    return 0;
  }  
  int iRet = 0;
  if (df[iButton]) {
    removeDigCursor();
    iRet = (this->*df[iButton])(crd);
    drawDigCursor();
  }
  return iRet;
};

/*
int DigiEditor::activate(zActivateEvt* Evt)
{
  bool fTmp = Evt->active();
  if (fTmp) {
    fFocus = true;
    drawDigCursor();
  }  
  else {
    removeDigCursor();  
    fFocus = false;
  }  
  info->show(fActDigitizer && fFocus ? SW_SHOW : SW_HIDE);
  return 0;
}
*/

LRESULT DigiEditor::OnUpdate(WPARAM wParam, LPARAM lParam)
{
	fActDigitizer = info->IsWindowVisible()?true:false;
  if (!fActDigitizer || fGreyDigitizer || !fFocus)
    return Editor::OnUpdate(wParam, lParam);

  int iButton = wParam - 100;
  if (iButton < -1 || iButton > 4)
    return Editor::OnUpdate(wParam, lParam);
  int iRet = 0;
  iButton += 1;
  if (iLastButton > 0 && iButton != iLastButton)
    info->button(iLastButton)->SetCheck(0);
  CoordWithCoordSystem* c = (CoordWithCoordSystem*) lParam;
  info->SetCoord(*c);
  if (iButton && iButton != iLastButton)
    info->button(iButton)->SetCheck(1);
  iRet = iDigiFunc(iButton, *c);
  iLastButton = iButton;
  return iRet;
}

void DigiEditor::StartBusy()
{
  GreyDigitizer(true);
}

void DigiEditor::EndBusy()
{
  GreyDigitizer(false);
}  

void DigiEditor::OnInfoWindow()
{
	if (info->IsWindowVisible())
		info->ShowWindow(SW_HIDE);
	else
		info->ShowWindow(SW_SHOW);
}

void DigiEditor::OnUpdateInfoWindow(CCmdUI* pCmdUI)
{
	Digitizer* dig = IlwWinApp()->dig();
	pCmdUI->Enable(0 != dig && dig->fActive());
	pCmdUI->SetCheck(info->IsWindowVisible()?1:0);
}
