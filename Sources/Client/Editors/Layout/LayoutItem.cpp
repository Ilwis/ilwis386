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
// LayoutItem.cpp: implementation of the LayoutItem class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Client\Editors\Layout\LayoutItem.h"
#include "Client\Editors\Layout\LayoutDoc.h"
#include "Headers\constant.h"
#include "Engine\Base\File\ElementMap.h"
#include "Client\Mapwindow\ZoomableView.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(LayoutItem, CCmdTarget)
	//{{AFX_MSG_MAP(LayoutItem)
	ON_COMMAND(ID_ITEM_EDIT, OnItemEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


LayoutItem::LayoutItem(LayoutDoc* ldoc)
: ld(ldoc)
, fInitialized(false)
{
	// default somewthere upper left
	mmPos = MinMax(RowCol(300,300),RowCol(500,500));
	ld->SetModifiedFlag();
}

LayoutItem::~LayoutItem()
{
}

CRect LayoutItem::rectPos() const // return position in MetaFile units
{
	return CRect(mmPos.MinCol(), mmPos.MinRow(),
	             mmPos.MaxCol(), mmPos.MaxRow());
}

void LayoutItem::OnDraw(CDC* cdc)
{
	fInitialized = true;
	// default indicator
	Color colBlack(0,0,0);
	CPen penBlack(PS_SOLID,1,colBlack); 	
	CPen* penOld = cdc->SelectObject(&penBlack);
	CRect rect = rectPos();
	cdc->Rectangle(&rect);
	cdc->MoveTo(rect.TopLeft());
	cdc->LineTo(rect.BottomRight());
	cdc->SelectObject(penOld);
}

bool LayoutItem::fIsotropic() const
{
	return false;
}

double LayoutItem::rHeightFact() const
{
	return rUNDEF;
}

bool LayoutItem::fOnChangedItemSize(LayoutItem* li)
{
	return li == this;
}

void LayoutItem::SetPosition(MinMax mm, int iHit) 
{ 
	if (fIsotropic()) {
		double rFact = rHeightFact();
		if (rFact > 0) {
			bool fAdjustWidth;
			int iWidth = mm.width();
			int iHeight = mm.height();
			switch (iHit) 
			{
				case CRectTracker::hitTop:
				case CRectTracker::hitBottom:
					fAdjustWidth = true;
					break;
				case CRectTracker::hitLeft:
				case CRectTracker::hitRight:
					fAdjustWidth = false;
					break;
				default: 
				{
					double rW = double(iWidth) / mmPos.width();
					if (rW < 1)
						rW = 1 / rW;
					double rH = double(iHeight) / mmPos.height();
					if (rH < 1)
						rH = 1 / rH;
					fAdjustWidth = rW > rH;
				}
			}
			if (fAdjustWidth) 
			{
				iWidth = rounding(iHeight / rFact);
				mm.MaxCol() = mm.MinCol() + iWidth;
			}
			else 
			{
				iHeight = rounding(iWidth * rFact);
				mm.MaxRow() = mm.MinRow() + iHeight;
			}
		}
	}
	if (mm != mmPos)
		ld->SetModifiedFlag();
	mmPos = mm; 
}

bool LayoutItem::fClip() const
{
	return false;
}

void LayoutItem::OnItemEdit()
{
	if (fConfigure()) {
		ld->SetModifiedFlag();
		ld->UpdateAllViews(0, LayoutDoc::hintITEM, this);
	}
}

bool LayoutItem::fConfigure()
{
	return false;
}

void LayoutItem::Serialize(CArchive& ar, const char* sSection)
{
	ElementContainer en;
	en.em = new ElementMap;
	if (ar.IsStoring()) {
		WriteElements(en, sSection);
		en.em->SerializeSection(ar, sSection);
	}
	else {
		en.em->SerializeSection(ar, sSection);
		ReadElements(en, sSection);
	}  
}

void LayoutItem::ReadElements(ElementContainer& en, const char* sSection)
{
	ObjectInfo::ReadElement(sSection, "Position", en, mmPos); 	
	mmPos.MinCol() /= 10;
	mmPos.MaxCol() /= 10;
	mmPos.MinRow() /= 10;
	mmPos.MaxRow() /= 10;
}

void LayoutItem::WriteElements(ElementContainer& en, const char* sSection)
{
	MinMax mm = mmPos;
	mm.MinCol() *= 10;
	mm.MaxCol() *= 10;
	mm.MinRow() *= 10;
	mm.MaxRow() *= 10;
	ObjectInfo::WriteElement(sSection, "Position", en, mm); 	
}

String LayoutItem::sType() const
{
	return "Unspecified";
}

void LayoutItem::SetID(int iNr)
{
	ID = iNr;
}

int LayoutItem::iID() const
{
	return ID; 
}

bool LayoutItem::fAddExtraClipboardItems()
{
  // do nothing
	return true;
}

String LayoutItem::sName() const
{
	return sType();
}

bool LayoutItem::fDependsOn(LayoutItem* li)
{
	return false;
}

#define sMen(ID) ILWSF("men",ID).scVal()
#define pmadd(ID) men.AppendMenu(MF_STRING, ID, sMen(ID)); 

void LayoutItem::OnContextMenu(CWnd* wnd, CPoint pt) 
{
	ZoomableView* zv = dynamic_cast<ZoomableView*>(wnd);
	CMenu men;
	men.CreatePopupMenu();
	if (zv) {
		pmadd(ID_NORMAL);
		pmadd(ID_ZOOMIN);
		pmadd(ID_ZOOMOUT);
		men.AppendMenu(MF_SEPARATOR);
	}
	pmadd(ID_ITEM_EDIT);
	pmadd(ID_EDIT_CLEAR);
	men.SetDefaultItem(ID_ITEM_EDIT);
  int iCmd = men.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON|TPM_NONOTIFY|TPM_RETURNCMD, pt.x, pt.y, wnd);
	switch (iCmd) 
	{
		case ID_ITEM_EDIT:
			OnItemEdit();
			return;
		default:
			wnd->SendMessage(WM_COMMAND, iCmd);
	}
}

double LayoutItem::rScale() const   // 1:....
{
	return rUNDEF;
}

void LayoutItem::SetScale(double rScale)
{
	// do nothing
}

void LayoutItem::SaveModified()
{
	// by default do nothing
}
