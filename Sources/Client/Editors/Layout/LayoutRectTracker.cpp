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
// LayoutRectTracker.cpp: implementation of the LayoutRectTracker class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Client\Editors\Layout\LayoutRectTracker.h"
#include "Client\Editors\Layout\LayoutItem.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

LayoutRectTracker::LayoutRectTracker()
{
	li = 0;
}

LayoutRectTracker::~LayoutRectTracker()
{
}


void LayoutRectTracker::AdjustRect(int nHandle, LPRECT lpRect)
{
	if (0 == li)
		return;
/*
	if (!li->fIsotropic())
		return;
*/
	MinMax mm =	li->mmPosition();
	CRect& rect = (CRect&)*lpRect;
	double rFact;
	int iWidth = rect.Width();
	int iHeight = rect.Height();
	// initialize width and height
	switch (nHandle) 
	{
/*
		case hitTop:
		case hitBottom: 
			rFact = double(iHeight) / mm.height();
			iWidth = rFact * mm.width();
			break;
		case hitLeft:
		case hitRight:
			rFact = double(iWidth) / mm.width();
			iHeight = rFact * mm.height();
			break;
*/
		case hitTopLeft:
		case hitTopRight:
		case hitBottomRight:
		case hitBottomLeft:
			rFact = max(double(iHeight) / mm.height(), double(iWidth) / mm.width());
			iWidth = (int)(rFact * mm.width());
			iHeight = (int)(rFact * mm.height());
			break;
	}
	// left - right
	switch (nHandle) 
	{
/*
		case hitTop:
		case hitBottom: 
			rect.left = (rectOrig.left + rectOrig.right - iWidth)/2;
			rect.right = rect.left + iWidth;
			break;
*/
		case hitTopLeft:
		case hitBottomLeft:
			rect.right = rectOrig.right;
			rect.left = rect.right - iWidth;
			break;
		case hitTopRight:
		case hitBottomRight:
			rect.left = rectOrig.left;
			rect.right = rect.left + iWidth;
			break;
	}
	// top - bottom
	switch (nHandle) 
	{
/*
		case hitLeft:
		case hitRight:
			rect.top = (rectOrig.top + rectOrig.bottom - iHeight)/2;
			rect.bottom = rect.top + iHeight;
			break;
*/
		case hitTopLeft:
		case hitTopRight:
			rect.bottom = rectOrig.bottom;
			rect.top = rect.bottom - iHeight;
			break;
		case hitBottomRight:
		case hitBottomLeft:
			rect.top = rectOrig.top;
			rect.bottom = rect.top + iHeight;
			break;
	}
}

UINT LayoutRectTracker::GetHandleMask() const
{
	if (0 == li)
		return 0;
	if (!li->fIsotropic())
		return CRectTracker::GetHandleMask();
	else
		return 0x0F;
}

BOOL LayoutRectTracker::Track(CWnd* pWnd, CPoint point)
{
	rectOrig = m_rect;
	return CRectTracker::Track(pWnd, point);
}
