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
// SimpleGraphWindow.cpp : implementation file
//

#include "Client\FormElements\SimpleGraphWindow.h"

#include <math.h> // floor

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SimpleGraphWindow

SimpleGraphWindow::SimpleGraphWindow()
: m_fDragging(false)
, m_fBusyInPaintThread(false)
, m_fDirty(false)
, m_fRedraw(false)
, m_fAbortPaintThread(false)
, m_bmMemory(0)
, m_dcMemory(0)
, m_pFunc(0)
, m_fnt(0)
, m_iLeftBorderThickness(20)
, m_iTopBorderThickness(20)
, m_iRightBorderThickness(20)
, m_iBottomBorderThickness(20)
{

}

SimpleGraphWindow::~SimpleGraphWindow()
{
}


BEGIN_MESSAGE_MAP(SimpleGraphWindow, CWnd)
	//{{AFX_MSG_MAP(SimpleGraphWindow)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
	ON_WM_DESTROY()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// SimpleGraphWindow message handlers

void SimpleGraphWindow::SetAxisFont(CFont * fnt)
{
	m_fnt = fnt;
}

void SimpleGraphWindow::SetBorderThickness(int l, int t, int r, int b)
{
	m_iLeftBorderThickness = l;
	m_iTopBorderThickness = t;
	m_iRightBorderThickness = r;
	m_iBottomBorderThickness = b;
	SetDirty(true);
}

void SimpleGraphWindow::StartDrag(CPoint point)
{
	m_fDragging = true;
	
	if (m_pFunc)
	{
		DoublePoint dp (rScreenToX(point.x), rScreenToY(point.y));
		m_pFunc->SetAnchor(dp);
		SetDirty(true);
	}
}

void SimpleGraphWindow::Drag(CPoint point)
{
	if (m_fDragging)
	{
		if (m_pFunc)
		{
			DoublePoint dp (rScreenToX(point.x), rScreenToY(point.y));
			m_pFunc->SetAnchor(dp);
			SetDirty(true);
		}
	}
}

void SimpleGraphWindow::EndDrag(CPoint point)
{
	if (m_fDragging)
	{
		if (m_pFunc)
		{
			DoublePoint dp (rScreenToX(point.x), rScreenToY(point.y));
			m_pFunc->SetAnchor(dp);
		}
		SetDirty(true); // to renew the dcMemory with the latest graph
		m_fDragging = false;
	}
}

void SimpleGraphWindow::OnLButtonDown(UINT nFlags, CPoint point) 
{
	SetCapture();
	StartDrag(point);
	
	CWnd::OnLButtonDown(nFlags, point);
}

void SimpleGraphWindow::OnLButtonUp(UINT nFlags, CPoint point) 
{
	EndDrag(point);
	
	CWnd::OnLButtonUp(nFlags, point);
	ReleaseCapture();
}

void SimpleGraphWindow::OnMouseMove(UINT nFlags, CPoint point) 
{
	Drag(point);
	
	CWnd::OnMouseMove(nFlags, point);
}

void SimpleGraphWindow::OnPaint() 
{
	//CPaintDC dc(this); // device context for painting
	CWnd::OnPaint(); // there is flicker without this .. probably unnecessary paint messages of parent window

	SetDirty();
}

void SimpleGraphWindow::SetDirty(bool fRedraw)
{
	// fRedraw will replot the graph and the axis
	// everything should call SetDirty with 'true' except OnPaint
	// this gives a performance gain when the window only needs repainting,
	// which then comes down to a BitBlt operation instead of drawing all lines
	if (!m_fAbortPaintThread)
	{
		m_fDirty = true;
		m_fRedraw = m_fRedraw || fRedraw;
		if (!m_fBusyInPaintThread)
		{
			m_fBusyInPaintThread = true;
			AfxBeginThread(PaintInThread, this);
		}
	}
}

UINT SimpleGraphWindow::PaintInThread(LPVOID pParam)
{
	SimpleGraphWindow * pObject = (SimpleGraphWindow*)pParam;
	if (pObject == NULL)
		return 1;

	CDC* pDC = pObject->GetDC();

	CRect rectClient;
	pObject->GetClientRect(rectClient);
	if (!pObject->m_dcMemory)
	{
		pObject->m_bmMemory = new CBitmap();
		pObject->m_bmMemory->CreateCompatibleBitmap(pDC, rectClient.Width(),rectClient.Height());

		pObject->m_dcMemory = new CDC();
		pObject->m_dcMemory->CreateCompatibleDC(pDC);

		pObject->m_bmOldBitmap = (CBitmap*)(pObject->m_dcMemory)->SelectObject(pObject->m_bmMemory);

		CRgn bounds;
		bounds.CreateRectRgnIndirect(rectClient);

		pObject->m_dcMemory->SelectClipRgn(&bounds, RGN_AND);
	}

	while (!pObject->m_fAbortPaintThread && pObject->m_fDirty)
	{
		pObject->m_fDirty = false;

		while (!pObject->m_fAbortPaintThread && pObject->m_fRedraw)
		{
			pObject->m_fRedraw = false;
			pObject->m_dcMemory->FillSolidRect(rectClient, RGB(255, 255, 255)); // white background
			pObject->DrawAxes(pObject->m_dcMemory);
			pObject->DrawFunction(pObject->m_dcMemory, pObject->m_pFunc);
		}

		if (!pObject->m_fAbortPaintThread && !pObject->m_fDirty) // do not bother refreshing the window if the area is already invalid or if the window is closing
			pDC->BitBlt(rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height(), pObject->m_dcMemory, 0, 0, SRCCOPY);
	}

	pObject->ReleaseDC(pDC);

	pObject->m_fBusyInPaintThread = false;

	return 0;
}

void SimpleGraphWindow::DrawFunction(const SimpleFunction * pFunc)
{
	CDC* pDC = GetDC();
	CRect rectClient;
	GetClientRect(rectClient);

	CRgn bounds;
	bounds.CreateRectRgnIndirect(rectClient);

	pDC->SelectClipRgn(&bounds, RGN_AND);

	DrawFunction(pDC, pFunc);

	ReleaseDC(pDC);
}

void SimpleGraphWindow::SetFunction(SimpleFunction * funPtr)
{
	m_pFunc = funPtr;
	if (funPtr != 0)
	{
		m_XYFunctionDomain = funPtr->Domain();
		m_XYFunctionDomain.NormalizeRect();
	}
	SetDirty(true);
}

void SimpleGraphWindow::Replot()
{
	SetDirty(true);
}

void SimpleGraphWindow::DrawFunction(CDC* pDC, const SimpleFunction * pFunc)
{
	// create and select a thin, blue pen
	CPen penBlue;
	penBlue.CreatePen(PS_SOLID, 0, RGB(0, 0, 255));
	CPen* pOldPen = pDC->SelectObject(&penBlue);

	// Draw the function here
	if (pFunc)
	{
		CRect functionPlotRect (GetFunctionPlotRect());
		int iX = functionPlotRect.left;
		double rX = rScreenToX(iX);
		double rY = pFunc->rGetFx(rX);
		int iY = iYToScreen(rY);
		pDC->MoveTo(iX, iY);
		for (iX = functionPlotRect.left + 1; iX <= functionPlotRect.right; ++iX)
		{
			rX = rScreenToX(iX);
			rY = pFunc->rGetFx(rX);
			iY = iYToScreen(rY);
			pDC->LineTo(iX, iY);
		}
		pDC->LineTo(iX, iY); // include the last point in the plot

		// Also plot the anchor points
		CBrush brushGreen (RGB(0, 128, 0));
		for (int i = 0; i < pFunc->vAnchors().size(); ++i)
		{
			double rAnchorX = (pFunc->vAnchors().at(i)).X;
			double rAnchorY = (pFunc->vAnchors().at(i)).Y;
			int iAnchorX = iXToScreen(rAnchorX);
			int iAnchorY = iYToScreen(rAnchorY);
			CRect rect (iAnchorX, iAnchorY, iAnchorX, iAnchorY);
			rect.InflateRect(2, 2, 3, 3); // a 5x5 pixel square
			pDC->FrameRect(rect, &brushGreen);
		}
	}

	// put back the old objects

	pDC->SelectObject(pOldPen);
}

void SimpleGraphWindow::DrawAxes(CDC* pDC)
{
	// draw the axes with a thin black pen
	CPen penGray(PS_SOLID, 0, RGB(0, 0, 0));
	CPen* pOldPen = pDC->SelectObject(&penGray);

	if (!m_fRedraw && !m_fAbortPaintThread)
	{
		// draw the axes
		CRect functionPlotRect (GetFunctionPlotRect());
		pDC->MoveTo(functionPlotRect.left, functionPlotRect.top);
		pDC->LineTo(functionPlotRect.left, functionPlotRect.bottom);
		pDC->LineTo(functionPlotRect.right, functionPlotRect.bottom);

		// draw ticks on the axes		
		int iTicks = 5;
		int iTickThickness = 3; // nr of pixels for drawing a tick
		double rTickXSize = (double)(functionPlotRect.Width()) / (double)(iTicks - 1);
		double rTickYSize = (double)(functionPlotRect.Height()) / (double)(iTicks - 1);
		for (int i = 0; i < iTicks; ++i)
		{
			pDC->MoveTo(functionPlotRect.left, functionPlotRect.top + rTickYSize * i);
			pDC->LineTo(functionPlotRect.left - iTickThickness, functionPlotRect.top + rTickYSize * i);
			pDC->MoveTo(functionPlotRect.left + rTickXSize * i, functionPlotRect.bottom);
			pDC->LineTo(functionPlotRect.left + rTickXSize * i, functionPlotRect.bottom + iTickThickness);
		}

		// draw axis values
		// x-axis values
		CFont * oldFont = 0;
		if (m_fnt)
			oldFont = pDC->SelectObject(m_fnt);
		UINT iPreviousAlignment = pDC->SetTextAlign(TA_CENTER | TA_TOP);
		COLORREF clrPreviousColor = pDC->SetTextColor(RGB(0, 0, 0)); // black text
		CString sVal;
		sVal.Format("%g", m_XYFunctionDomain.left);
		pDC->TextOut(functionPlotRect.left, functionPlotRect.bottom + iTickThickness, sVal);
		sVal.Format("%g", m_XYFunctionDomain.right);
		pDC->TextOut(functionPlotRect.right, functionPlotRect.bottom + iTickThickness, sVal);
		// y-axis values
		pDC->SetTextAlign(TA_RIGHT | TA_BASELINE);
		sVal.Format("%g", m_XYFunctionDomain.top);
		pDC->TextOut(functionPlotRect.left - iTickThickness, functionPlotRect.top, sVal);
		sVal.Format("%g", m_XYFunctionDomain.bottom);
		pDC->TextOut(functionPlotRect.left - iTickThickness, functionPlotRect.bottom, sVal);

		pDC->SetTextColor(clrPreviousColor);
		pDC->SetTextAlign(iPreviousAlignment);
		if (m_fnt) // take care not to change this: m_fnt determines whether SelectObject should be called or not (in order to "release" m_fnt)
			pDC->SelectObject(oldFont);
	}
	
	// Put back the old objects.
	pDC->SelectObject(pOldPen);
}

CRect SimpleGraphWindow::GetFunctionPlotRect() const
{
	CRect rectClient;
	GetClientRect(rectClient);
	rectClient.DeflateRect(m_iLeftBorderThickness, m_iTopBorderThickness, m_iRightBorderThickness, m_iBottomBorderThickness);
	return rectClient;
}

const double SimpleGraphWindow::rYToRFactor() const
{
	if (m_XYFunctionDomain.fValid())
	{
		double rYFactor = GetFunctionPlotRect().Height() / m_XYFunctionDomain.Height();
		return rYFactor;
	}
	else
		return 1.0;
}

const double SimpleGraphWindow::rXToCFactor() const
{
	if (m_XYFunctionDomain.fValid())
	{
		double rXFactor = GetFunctionPlotRect().Width() / m_XYFunctionDomain.Width();
		return rXFactor;
	}
	else
		return 1.0;
}

const int SimpleGraphWindow::iXToScreen(const double rX) const
{
	double rXFactor = rXToCFactor();
	return floor((rX - m_XYFunctionDomain.left) * rXFactor + 0.5) + m_iLeftBorderThickness;
}

const int SimpleGraphWindow::iYToScreen(const double rY) const
{
	double rYFactor = rYToRFactor();
	return floor((m_XYFunctionDomain.top - rY) * rYFactor + 0.5) + m_iTopBorderThickness;
}

const double SimpleGraphWindow::rScreenToX(int iScreenX) const
{
	double rXFactor = rXToCFactor();
	return (iScreenX - m_iLeftBorderThickness) / rXFactor + m_XYFunctionDomain.left;
}

const double SimpleGraphWindow::rScreenToY(int iScreenY) const
{
	double rYFactor = rYToRFactor();
	return m_XYFunctionDomain.top - (iScreenY - m_iTopBorderThickness) / rYFactor;
}

void SimpleGraphWindow::ProcessMessages()
{
   MSG msg;
   while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		 ::DispatchMessage(&msg);
}

void SimpleGraphWindow::CloseThreads()
{
	m_fAbortPaintThread = true;
	int iLoopCount = 0;
	while (m_fBusyInPaintThread && iLoopCount < 50) // give it 5 sec
	{
		Sleep(100);
		ProcessMessages();
		++iLoopCount;
	}
}

void SimpleGraphWindow::OnDestroy() 
{
	CloseThreads();
	if (m_dcMemory)
	{
		// m_dcMemory->SelectObject(m_bmOldBitmap); // this crashes
		m_dcMemory->DeleteDC();
		delete m_dcMemory;
		delete m_bmMemory;
	}
	CWnd::OnDestroy();
}
