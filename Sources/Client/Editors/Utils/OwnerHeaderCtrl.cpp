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
#include "Headers\toolspch.h"
#include "Client\Editors\Utils\OwnerHeaderCtrl.h"

IlwisHeaderCtrl::IlwisHeaderCtrl() 
{
	m_iSortCol = -1;
}

int IlwisHeaderCtrl::SetSortImage(int iCol, bool fAscending)
{
	int iPrevCol = m_iSortCol;

	if ( GetSafeHwnd() == NULL) return 0;
	
	m_iSortCol = iCol;
	m_fSortAsc = fAscending;

	// Change the item to owner drawn
	HDITEM hditem;

	hditem.mask = HDI_FORMAT;
	GetItem(iCol, &hditem);
	hditem.fmt |= HDF_OWNERDRAW;
	SetItem(iCol, &hditem);

	// Invalidate header control so that it gets redrawn
	Invalidate();
	return iPrevCol;
}

void IlwisHeaderCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC dc;

	dc.Attach(lpDrawItemStruct->hDC);

	// Get the column rect
	CRect rcLabel(lpDrawItemStruct->rcItem);

	// Save DC
	int nSavedDC = dc.SaveDC();

	// Set clipping region to limit drawing within column
	CRgn rgn;
	rgn.CreateRectRgnIndirect(&rcLabel);
	dc.SelectObject(&rgn);
	rgn.DeleteObject();

        // Draw the background
	dc.FillRect(rcLabel, &CBrush(::GetSysColor(COLOR_3DFACE)));
	
	// Labels are offset by a certain amount  
	// This offset is related to the width of a space character
	int offset = dc.GetTextExtent(_T(" "), 1).cx * 2;

	// Get the column text and format
	TCHAR buf[256];
	HD_ITEM hditem;
	
	hditem.mask = HDI_TEXT | HDI_FORMAT;
	hditem.pszText = buf;
	hditem.cchTextMax = 255;
	GetItem(lpDrawItemStruct->itemID, &hditem);

	// Determine format for drawing column label
	UINT uFormat = DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP 
					| DT_VCENTER | DT_END_ELLIPSIS ;

	if(hditem.fmt & HDF_CENTER)
		uFormat |= DT_CENTER;
	else if(hditem.fmt & HDF_RIGHT)
		uFormat |= DT_RIGHT;
	else
		uFormat |= DT_LEFT;

	// Adjust the rect if the mouse button is pressed on it
	if(lpDrawItemStruct->itemState == ODS_SELECTED)
	{
		rcLabel.left++;
		rcLabel.top += 2;
		rcLabel.right++;
	}

	// Adjust the rect further if Sort arrow is to be displayed
	if(lpDrawItemStruct->itemID == (UINT)m_iSortCol)
	{
		rcLabel.right -= 3 * offset;
	}

	rcLabel.left += offset;
	rcLabel.right -= offset;

	// Draw column label
	if(rcLabel.left < rcLabel.right)
		dc.DrawText(buf,-1,rcLabel, uFormat);

	// Draw the Sort arrow
	if(lpDrawItemStruct->itemID == (UINT)m_iSortCol)
	{
		CRect rcIcon(lpDrawItemStruct->rcItem);

		// Set up pens to use for drawing the triangle
		CPen penLight(PS_SOLID, 1, GetSysColor(COLOR_3DHILIGHT));
		CPen penShadow(PS_SOLID, 1, GetSysColor(COLOR_3DSHADOW));
		CPen *pOldPen = dc.SelectObject(&penLight);

		offset = (rcIcon.bottom - rcIcon.top) / 4;
		int rmargin = 4;
		if (m_fSortAsc) 
		{
		  // Draw triangle pointing downwards
		  dc.MoveTo(rcIcon.right - rmargin - offset-1, offset);
		  dc.LineTo(rcIcon.right - rmargin - 2*offset-1, rcIcon.bottom - offset - 1);
		  dc.MoveTo(rcIcon.right - rmargin - 2*offset-2, rcIcon.bottom - offset - 1);
      dc.SelectObject(pOldPen);
		  pOldPen = dc.SelectObject(&penShadow);
		  dc.LineTo(rcIcon.right - rmargin - 3*offset-1, offset);
		  dc.LineTo(rcIcon.right - rmargin - offset-1, offset);		
		}		
		else 
		{
		  // Draw triangle pointing upwards
		  dc.MoveTo(rcIcon.right - rmargin - 2*offset, offset);
		  dc.LineTo(rcIcon.right - rmargin - offset, rcIcon.bottom - offset-1);
		  dc.LineTo(rcIcon.right - rmargin - 3*offset-2, rcIcon.bottom - offset-1);
		  dc.MoveTo(rcIcon.right - rmargin - 3*offset-1, rcIcon.bottom - offset-1);
      dc.SelectObject(pOldPen);
		  pOldPen = dc.SelectObject(&penShadow);
		  dc.LineTo(rcIcon.right - rmargin - 2*offset, offset-1);		
		}		

		// Restore the pen
		dc.SelectObject(pOldPen);
	}

	// Restore dc
	dc.RestoreDC(nSavedDC);

	// Detach the dc before returning
	dc.Detach();
}
