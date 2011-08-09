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
#include "Client\Headers\formelementspch.h"
#include "Headers\messages.h"
#include "Client\Editors\Utils\ColorCB.h"
#include "Client\Editors\Utils\ColorGrid.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Headers\constant.h"
#include "Headers\Hs\Represen.hs"

static const int iCCBWIDTH = 50;
static const int IDCB1=7346;
static const int IDCB2=7347;
static const int IDCB3=7348;
static const int IDCB4=7349;

//-- [ ColorGrid ]----------------------------------------------------------------------
BEGIN_MESSAGE_MAP(ColorGrid, CWnd)
	ON_WM_PAINT()
	ON_WM_LBUTTONUP()
	ON_WM_KEYUP()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_NO_OF_CELLS, OnNoOfCells)
	ON_CBN_SELCHANGE(IDCB1, OnCB1Changed)
	ON_CBN_SELCHANGE(IDCB2, OnCB2Changed)
	ON_CBN_SELCHANGE(IDCB3, OnCB3Changed)
	ON_CBN_SELCHANGE(IDCB4, OnCB4Changed)
END_MESSAGE_MAP()

BOOL ColorGrid::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, int iNC)
{
	CRect outerRct(rect);
	outerRct.InflateRect(2,2);
	BOOL iRet = CWnd::Create(NULL, "name", dwStyle, outerRct, pParentWnd, nID);
	iNCells = iNC;
	CDC *dc = GetWindowDC();
	int iCBHeight = dc->GetTextExtent("gk").cy + 8;
	ccb1.Create(WS_CHILD | WS_VISIBLE , CRect(0, 0,iCCBWIDTH,175), this,IDCB1);
	ccb2.Create(WS_CHILD | WS_VISIBLE , CRect(rect.right - iCCBWIDTH-2, 0 ,rect.right,175), this, IDCB2);
	ccb3.Create(WS_CHILD | WS_VISIBLE , CRect(rect.right - iCCBWIDTH,rect.bottom - iCBHeight ,
		                                        rect.right, rect.bottom + 175), this, IDCB3);
	ccb4.Create(WS_CHILD | WS_VISIBLE , CRect(0,rect.bottom - iCBHeight ,
		                                        iCCBWIDTH, rect.bottom + 175), this, IDCB4);

	GetClientRect(&gridRect);
	gridRect.DeflateRect(iCBHeight, iCBHeight);
	rCellW = (double)gridRect.Width() / iNCells;
	rCellH = (double)gridRect.Height() / iNCells ;
	iSelX = iSelY = iUNDEF;

	ccb1.SetCurSel(0); //black
	ccb2.SetCurSel(5); //red 
	ccb3.SetCurSel(6); //green
	ccb4.SetCurSel(7); //blue
	edgeColors.push_back( ccb1.GetColor());
	edgeColors.push_back( ccb2.GetColor());
	edgeColors.push_back( ccb3.GetColor());
	edgeColors.push_back( ccb4.GetColor());

	SetFocus();

	wndPostOffice = pParentWnd;

	return iRet;
}

void ColorGrid::OnContextMenu( CWnd* pWnd, CPoint point )
{
	CMenu menu;
	menu.CreatePopupMenu();
	String sText(ILWSF("men",ID_NO_OF_CELLS));
	menu.AppendMenu(MF_STRING, ID_NO_OF_CELLS, sText.c_str());
	menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y,
                      this);
	menu.Detach();
}

void ColorGrid::OnLButtonUp(UINT nFlags, CPoint point )
{

	RedrawSelectCells();

	if (!gridRect.PtInRect(point)) 
	{
		iSelX = iSelY = iUNDEF;
		return;
	}

	iSelX = ( point.x - gridRect.left) / rCellW;
	iSelY = ( point.y - gridRect.top) / rCellH;

	RedrawSelectCells();

	wndPostOffice->PostMessage(ILWM_SETCOLOR, SelectedColor(), TRUE);
	SetFocus();
}

void ColorGrid::RedrawSelectCells()
{
	CRect SelRect(0,0,rCellW, rCellH);
	SelRect.OffsetRect(gridRect.left + iSelX * rCellW, gridRect.top + iSelY * rCellH);
	SelRect.InflateRect(1,1);
	InvalidateRect(&SelRect);
}

int ColorGrid::iGetNoOfCells()
{
	return iNCells;
}

void ColorGrid::SetNoOfCells(int iN)
{
	iNCells = iN;
	rCellW = (double)gridRect.Width() / iNCells;
	rCellH = (double)gridRect.Height() / iNCells ;
	Invalidate();
}

void ColorGrid::OnNoOfCells()
{
 class NoOfCellsForm: public FormWithDest
 {
  public:
    NoOfCellsForm(CWnd* wPar, int* iX)
    : FormWithDest(wPar, TR("Resolution Color Selector"))
    {
      new FieldBlank(root);
      new FieldInt(root, TR("Nr. of colors along edge"), iX, ValueRange(2L,100L), true);
      SetMenHelpTopic("ilwismen\\representation_class_editor_column_width.htm");
      create();
    }  
  };
	int iX =   iGetNoOfCells();
  NoOfCellsForm frm(this, &iX);
  if (frm.fOkClicked()) 
	{
		iX = max(2, min(iX, 100));
		SetNoOfCells(iX);
  }
}

void ColorGrid::OnSize( UINT nType, int cx, int cy )
{
	if ( !GetSafeHwnd( ) ) return;

	CDC *dc = GetWindowDC();
	int iCBHeight = dc->GetTextExtent("gk").cy + 8;
	CRect rct = CRect(0,0, cx, cy);
	if ( !ccb1.GetSafeHwnd()) return;
  MoveWindow(rct); 
	ccb1.MoveWindow(CRect(0, 0,iCCBWIDTH,175));
	ccb2.MoveWindow(CRect(rct.right - iCCBWIDTH, 0 ,rct.right,175));
	ccb3.MoveWindow(CRect(rct.right - iCCBWIDTH,rct.bottom - iCBHeight ,
		                   rct.right, rct.bottom + 175));
	ccb4.MoveWindow(CRect(0,rct.bottom - iCBHeight ,
                      iCCBWIDTH, rct.bottom + 175));

	GetParent()->GetClientRect(&gridRect);
	gridRect.DeflateRect(iCBHeight, iCBHeight);
	rCellW = (double)gridRect.Width() / iNCells;
	rCellH = (double)gridRect.Height() / iNCells ;
	Invalidate();
}

void ColorGrid::SelectColor(COLORREF clr)
{
	RedrawSelectCells();
	iSelX = iSelY = iUNDEF;
	for(int iX = 0; iX < iNCells; ++iX )
	{
		for (int iY = 0; iY < iNCells; ++iY)
    {
			COLORREF curColor = CalcColor(iX, iY) ;
			if ( clr == curColor )
			{
				iSelX = iX;
				iSelY = iY;
				iX = iY = iNCells; // break loops
			}
		}
	}
	RedrawSelectCells();
}

void ColorGrid::OnKeyUp( UINT nChar, UINT nRepCnt, UINT nFlags )
{
	if (iSelY == iUNDEF || iSelX == iUNDEF) return;
	RedrawSelectCells();
	switch ( nChar)
	{
		case VK_DOWN:
			iSelY = min(iSelY + 1, iNCells-1);
			break;
		case VK_UP:
			iSelY = max(iSelY - 1, 0);
			break;
		case VK_LEFT:
			iSelX = max(iSelX - 1, 0);
			break;
		case VK_RIGHT:
			iSelX = min(iSelX + 1, iNCells-1);
			break;
		default:
			return;
	}
	wndPostOffice->PostMessage(ILWM_SETCOLOR, SelectedColor(), TRUE);
	RedrawSelectCells();
}

void ColorGrid::OnCB1Changed()
{
	if ( ccb1.GetCurSel() == ccb1.GetCount() -1 )
		ccb1.CustomColor();	
	COLORREF iCol = ccb1.GetColor();
	edgeColors[ecLEFTUP] = iCol;
	Invalidate();
	if ( iSelX != iUNDEF && iSelY != iUNDEF)
		wndPostOffice->PostMessage(ILWM_SETCOLOR, SelectedColor(), TRUE);
}

void ColorGrid::OnCB2Changed()
{
	if ( ccb2.GetCurSel() == ccb2.GetCount() -1 )
		ccb2.CustomColor();
	COLORREF iCol = ccb2.GetColor();
	edgeColors[ecRIGHTUP] = iCol;
	Invalidate();
	if ( iSelX != iUNDEF && iSelY != iUNDEF)
		wndPostOffice->PostMessage(ILWM_SETCOLOR, SelectedColor(), TRUE);
}

void ColorGrid::OnCB3Changed()
{
	if ( ccb3.GetCurSel() == ccb3.GetCount() -1 )
		ccb3.CustomColor();
	COLORREF iCol = ccb3.GetColor();
	edgeColors[ecRIGHTDOWN] = iCol;
	Invalidate();
	if ( iSelX != iUNDEF && iSelY != iUNDEF)
		wndPostOffice->PostMessage(ILWM_SETCOLOR, SelectedColor(), TRUE);
}

void ColorGrid::OnCB4Changed()
{
	if ( ccb4.GetCurSel() == ccb4.GetCount() -1 )
		ccb4.CustomColor();
	COLORREF iCol = ccb4.GetColor();
	edgeColors[ecLEFTDOWN] = iCol;
	Invalidate();
	if ( iSelX != iUNDEF && iSelY != iUNDEF)
		wndPostOffice->PostMessage(ILWM_SETCOLOR, SelectedColor(), TRUE);
}

void ColorGrid::OnPaint()
{
	CPaintDC dc(this);

	CRect rect;
	GetClientRect(rect);

  ccb1.ShowWindow(SW_SHOW);
  ccb2.ShowWindow(SW_SHOW);
  ccb3.ShowWindow(SW_SHOW);
  ccb4.ShowWindow(SW_SHOW);

	CBrush br(GetSysColor(COLOR_MENU));
  dc.FillRect(&rect, &br);

	CRect cell(0,0, rCellW, rCellH);
	cell.OffsetRect(gridRect.left, gridRect.top);

	for(int iX = 0; iX < iNCells; ++iX )
	{

		for (int iY = 0; iY < iNCells; ++iY)
		{
			COLORREF iColor = CalcColor(iX, iY);
			CBrush cellBrush(iColor);
			dc.FillRect(&cell, &cellBrush);
			cell.top = cell.bottom ;
      cell.bottom = gridRect.top + rCellH * ( iY + 2);
		}
		cell.left = cell.right;
		cell.right = gridRect.left + rCellW * ( iX + 2);
		cell.top = gridRect.top;
		cell.bottom = gridRect.top + rCellH;
	}

	CBrush brush(GetSysColor(COLOR_WINDOWTEXT));
	dc.FrameRect(&gridRect, &brush);

	if (iSelX != iUNDEF )
	{
		CRect selRect(0,0,rCellW, rCellH);
		selRect.OffsetRect(gridRect.left + iSelX * rCellW, gridRect.top + iSelY * rCellH);
		CPen pen(PS_DOT, 1, GetSysColor(COLOR_WINDOWTEXT));
		LOGBRUSH lb;
		lb.lbStyle = BS_NULL;
		CBrush brush;
		brush.CreateBrushIndirect(&lb);
		CBrush *obr = dc.SelectObject(&brush);
		CPen *opn = dc.SelectObject(&pen);
		dc.Rectangle(&selRect);
		dc.SelectObject(obr);
		dc.SelectObject(opn);
	}
}

COLORREF ColorGrid::SelectedColor()
{
	return CalcColor(iSelX, iSelY);
}

COLORREF ColorGrid::CalcColor(int iX, int iY)
{
	if (iX < 0 || iX >= iNCells) 
    return RGB(-1,-1,-1);

	double rD1 = min(iX, iY ) + abs(iX - iY);
	double rD2 = min(iNCells - iX - 1, iY ) + abs( iNCells - iX - 1 - iY);
	double rD3 = min(iNCells - iX - 1, iNCells - iY - 1 ) + abs( iY - iX );
	double rD4 = min(iNCells - iY - 1, iX ) + abs( iNCells - iY - 1 - iX);
	double rLeftUpFrac = (1.0 -  rD1 / ( iNCells - 1.0)); //; * ( 1.0 -  rD1 / ( iNCells - 1.0)); 
	double rRightUpFrac = ( 1.0 -  rD2 / ( iNCells - 1.0)) ;//) * ( 1.0 -  rD3 / ( iNCells - 1.0));
	double rRightDownFrac =  ( 1.0 -  rD3 / ( iNCells - 1.0)) ;//* ( 1.0 -  rD3 / ( iNCells - 1.0));
	double rLeftDownFrac = ( 1.0 -  rD4 / ( iNCells - 1.0)) ; //* ( 1.0 -  rD4 / ( iNCells - 1.0));

	int iC1 = GetBValue(edgeColors[ecLEFTUP]);
	int iC2 = GetBValue(edgeColors[ecRIGHTUP]);
	int iC3 = GetBValue(edgeColors[ecRIGHTDOWN]);
	int iC4 = GetBValue(edgeColors[ecLEFTDOWN]);
	int iBlue = iC1 * rLeftUpFrac + iC2 * rRightUpFrac + iC3 * rRightDownFrac + iC4 * rLeftDownFrac;

	iC1 = GetGValue(edgeColors[ecLEFTUP]);
	iC2 = GetGValue(edgeColors[ecRIGHTUP]);
	iC3 = GetGValue(edgeColors[ecRIGHTDOWN]);
	iC4 = GetGValue(edgeColors[ecLEFTDOWN]);
	int iGreen = iC1 * rLeftUpFrac + iC2 * rRightUpFrac + iC3 * rRightDownFrac + iC4 * rLeftDownFrac;

	iC1 = GetRValue( edgeColors[ecLEFTUP]);
	iC2 = GetRValue (edgeColors[ecRIGHTUP]);
	iC3 = GetRValue(edgeColors[ecRIGHTDOWN]);
	iC4 = GetRValue(edgeColors[ecLEFTDOWN]);
	int iRed = iC1 * rLeftUpFrac + iC2 * rRightUpFrac + iC3 * rRightDownFrac + iC4 * rLeftDownFrac;

	return RGB(min(255,iRed), min(255,iGreen), min(255,iBlue));
}

void ColorGrid::SaveSettings(const String& sKey)
{
	IlwisSettings settings(sKey);
	settings.SetValue("NrOfCells", iNCells);
}

void ColorGrid::LoadSettings(const String& sKey)
{
	IlwisSettings settings(sKey);
	SetNoOfCells(settings.iValue("NrOfCells", 15));
}

void ColorGrid::SetPostOffice(CWnd *wnd)
{
	wndPostOffice = wnd;
}
