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
// ScaleBarLayoutItem.cpp: implementation of the ScaleBarLayoutItem class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Client\Editors\Layout\LayoutItem.h"
#include "Client\Editors\Layout\TextLayoutItem.h"
#include "Client\Editors\Layout\ScaleBarLayoutItem.h"
#include "Client\Editors\Layout\MapLayoutItem.h"
#include "Headers\constant.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\Editors\Layout\LayoutDoc.h"
#include "Headers\Hs\Drwforms.hs"
#include "Headers\Hs\Editor.hs"
#include "Client\FormElements\fldcolor.h"
#include "Client\FormElements\fldfontn.h"
#include "Engine\Base\Round.h"
#include "Headers\Hs\Layout.hs"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(ScaleBarLayoutItem, TextLayoutItem)
	//{{AFX_MSG_MAP(ScaleBarLayoutItem)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


ScaleBarLayoutItem::ScaleBarLayoutItem(LayoutDoc* ld, MapLayoutItem* mapli)
	: TextLayoutItem(ld)
	, mli(mapli),
	rBarThickness(2)
{
	MinMax mm = mli->mmPosition();
	mm.MaxRow() = mm.MinRow();
	mm.MinRow() -= 150; // 1.5 cm above map.
	SetPosition(mm,-1);
	iSteps = 5;
	double rScale = mli->rScale(); // 1:...
	double rDX = 10000.0 / rScale; // step size in 0.1 mm units
	rDX *= iSteps;
	iStepLength = (long)(rRound(500/rDX));
	eType = eBLOCK;
	fSmallSteps = true;
	fIndicateUnits = false;
	fUseKm = false;
	eTickOrientation = toBOTTOM;
}

ScaleBarLayoutItem::ScaleBarLayoutItem(LayoutDoc* ld)
	: TextLayoutItem(ld), mli(0)
{
}

ScaleBarLayoutItem::~ScaleBarLayoutItem()
{
}

void ScaleBarLayoutItem::Setup()
{
	CWindowDC cdc(CWnd::GetDesktopWindow());
	InitFont init(this,&cdc);

	double rScale = mli->rScale(); // 1:...
	int iDX = (int)(iStepLength * 10000.0 / rScale); // step size in 0.1 mm units
	int iDY; 
	if( rBarThickness == iUNDEF)
		iDY = iDX / 5;
	else
		iDY = (int)(rBarThickness * 10);
	
	CRect rect = rectPos();
	
	CPoint pt = rect.TopLeft();
	CSize sz = cdc.GetTextExtent("0",1);

  int y1 = pt.y + iDY;
	int x2 = pt.x + iSteps * iDX + 1;
	int iText = iSteps * iStepLength;
	if (fIndicateUnits)
		if (fUseKm) 
			sText = String("%li km", iText/1000);
		else
			sText = String("%li m", iText);
	else
		sText = String("%li", iText);
	CString str = sText.scVal();
	sz = cdc.GetTextExtent(str);
	rect.right = x2 + sz.cx / 2 + 1;
	rect.bottom = y1 + sz.cy;
	sz = rect.Size();
	MinMax mm = mmPosition();
	mm.MaxCol() = mm.MinCol() + sz.cx;
	mm.MaxRow() = mm.MinRow() + sz.cy;
	LayoutItem::SetPosition(mm,-1);
}

void ScaleBarLayoutItem::OnDraw(CDC* cdc)
{
	InitFont init(this,cdc);
	cdc->SetTextAlign(TA_CENTER|TA_TOP);
	double rScale = mli->rScale(); // 1:...
	int iDX = (int)(iStepLength * 10000.0 / rScale); // step size in 0.1 mm units
	int iDY;
	if( rBarThickness == iUNDEF)
		iDY = iDX / 5;
	else
		iDY = (int)(rBarThickness * 10);
	CRect rect = rectPos();
	
	CPoint pt = rect.TopLeft();
	int x1, x2, y1, y2;
	cdc->SetTextJustification(0,0);
	CSize sz = cdc->GetTextExtent("0",1);
	pt.x += sz.cx / 2 + 1;
	CPen penClr(PS_SOLID,3,clr); // 0.3 mm pen 
	CPen penNull(PS_NULL,0,RGB(0,0,0));
	CBrush brWhite(RGB(255,255,255));
	CBrush brClr(clr);
	CPen* penOld = cdc->SelectObject(&penNull);
	CBrush* brOld = cdc->SelectObject(&brClr);

	x1 = x2 = pt.x;
	y1 = y2 = pt.y;
  y1 += iDY;
	int i;
	int iTextY = y1;
	switch (eType)
	{
		case eBLOCK: 
		{
			if (fSmallSteps) 
			{
				for (i = 0; i < 5; ++i) {
					x1 = x2;
					x2 = pt.x + (i+1) * iDX / 5 + 1;
					if (i % 2)
						cdc->SelectObject(&brWhite);
					else
						cdc->SelectObject(&brClr);
					cdc->Rectangle(x1,y1,x2,y2);
				}
			}
			for (i = 0; i < iSteps; ++i) 
			{
				if (fSmallSteps && 0 == i)
					continue;
				x1 = x2;
				x2 = pt.x + (i+1) * iDX + 1;
				if (i % 2)
					cdc->SelectObject(&brWhite);
				else
					cdc->SelectObject(&brClr);
				cdc->Rectangle(x1,y1,x2,y2);
			}
			cdc->SelectStockObject(NULL_BRUSH);
			cdc->SelectObject(&penClr);
			x1 = pt.x;
			cdc->Rectangle(x1,y1,x2,y2);
		} break;
		case eTICK:
		{
			cdc->SelectObject(&penClr);
			if ( eTickOrientation == toTOP )
			{
				iTextY = y1;
				swap(y1, y2);
			}				
			if (fSmallSteps) 
			{
				for (i = 0; i < 5; ++i) {
					x1 = x2;
					x2 = pt.x + (i+1) * iDX / 5 + 1;
					cdc->MoveTo(x2,y1);
					cdc->LineTo(x2,y2);
				}
			}
			for (i = 0; i < iSteps; ++i) 
			{
				if (fSmallSteps && 0 == i)
					continue;
				x1 = x2;
				x2 = pt.x + (i+1) * iDX + 1;
				cdc->MoveTo(x2,y1);
				cdc->LineTo(x2,y2);
			}
			x1 = pt.x;
			cdc->MoveTo(x1,y1);
			cdc->LineTo(x1,y2);
			cdc->LineTo(x2,y2);
		} break;
	}
	cdc->TextOut(x1,iTextY,"0");
	int iText = iSteps * iStepLength;
	if (fIndicateUnits)
		if (fUseKm)
		{
			int iRest = iText % 1000;
			if ( iRest > 0)
				sText = String("%.1f km", (double)iText/1000.0);
			else
				sText = String("%li km", iText/1000);
		}			
		else
			sText = String("%li m", iText);
	else
		sText = String("%li", iText);
	cdc->TextOut(x2,iTextY,sText.scVal());

	cdc->SelectObject(penOld);
	cdc->SelectObject(brOld);
	fInitialized = true;
}

bool ScaleBarLayoutItem::fOnChangedItemSize(LayoutItem* li)
{
	return li == this || li== mli;
}

class ScaleBarLayoutItemForm : public FormWithDest
{
public:
  ScaleBarLayoutItemForm(CWnd* wnd, ScaleBarLayoutItem* tli) 
		: FormWithDest(wnd, SLOTitleScaleBar)
  {
      FieldGroup *fg = new FieldGroup(root);

      RadioGroup *rg = new RadioGroup(fg, "", (int*)&tli->eType, true);
      RadioButton *rb1 = new RadioButton(rg, SDCUiBlockScale);
      RadioButton *rb = new RadioButton(rg, SDCUiTickScale);
			RadioGroup *rg2 = new RadioGroup(rb, SLOUiTickOrientation, (int*)&tli->eTickOrientation, true);
			rg2->Align(rb1, AL_UNDER);
			new RadioButton(rg2, SLOUiTickTop);			
			new RadioButton(rg2, SLOUiTickBottom);
			FieldReal *f = new FieldReal(rb1, SLOUiBarThicness, &tli->rBarThickness, ValueRange(0.1, 10.0, 0.1));
			f->Align(rb1, AL_UNDER);		
			

			FieldInt *fi = new FieldInt(fg, SDCUiNrSteps, &tli->iSteps, ValueRange(2,12), true);
			fi->SetIndependentPos();
	    new FieldInt(fg, SDCUiStepSizeScalebar, &tli->iStepLength);
			CheckBox* cbSS = new CheckBox(fg, SDCUiSmallSteps, &tli->fSmallSteps);
			cbSS->SetIndependentPos();
			CheckBox* cbUnits = new CheckBox(fg, SLOUiUnitIndication, &tli->fIndicateUnits);
			cbUnits->Align(cbSS, AL_UNDER);
			new CheckBox(cbUnits, SLOUiUseKm, &tli->fUseKm);
			cbUnits->SetIndependentPos();

      FieldColor *fc = new FieldColor(fg, SDCUiColor, &tli->clr);
			fc->Align(cbUnits, AL_UNDER);

	    FieldLogFont* flf = new FieldLogFont(fg, &tli->lf);
	
      SetMenHelpTopic(htpCnfAnnScale);
      create();
  }
};



bool ScaleBarLayoutItem::fConfigure()
{
	ScaleBarLayoutItemForm frm(ld->wndGetActiveView(), this);
	if (sText == " ")
		return false;
	fTransparent = true;
	if (frm.fOkClicked()) {
		Setup();
		return true;
	}
	return false;
}

String ScaleBarLayoutItem::sType() const
{
	return "ScaleBar";
}

void ScaleBarLayoutItem::ReadElements(ElementContainer& en, const char* sSection)
{
	TextLayoutItem::ReadElements(en, sSection);
	int iNr;
	ObjectInfo::ReadElement(sSection, "Map View", en, iNr);
	mli = dynamic_cast<MapLayoutItem*>(ld->liFindID(iNr));
	ObjectInfo::ReadElement(sSection, "Nr Steps", en, iSteps);
	ObjectInfo::ReadElement(sSection, "Step Length", en, iStepLength);
	String sType;
	eType = eBLOCK;
	ObjectInfo::ReadElement(sSection, "Type", en, sType);
	if ("Block" == sType)
		eType = eBLOCK;
	else if ("Tick" == sType)
		eType = eTICK;
	ObjectInfo::ReadElement(sSection, "Small Steps", en, fSmallSteps);
	ObjectInfo::ReadElement(sSection, "BarHeigh", en, rBarThickness);
	if ( rBarThickness == rUNDEF)
		rBarThickness = 2.0;
	int iTor;
	ObjectInfo::ReadElement(sSection, "Tick orientation", en, iTor);	
	eTickOrientation = (toTickOrientation)iTor;
	ObjectInfo::ReadElement(sSection, "Indicate Units", en, fIndicateUnits);
	ObjectInfo::ReadElement(sSection, "Use km", en, fUseKm);
}

void ScaleBarLayoutItem::WriteElements(ElementContainer& en, const char* sSection)
{
	TextLayoutItem::WriteElements(en, sSection);
	ObjectInfo::WriteElement(sSection, "Map View", en, mli->iID());
	ObjectInfo::WriteElement(sSection, "Nr Steps", en, iSteps);
	ObjectInfo::WriteElement(sSection, "Step Length", en, iStepLength);
	String sType;
	switch (eType) {
		case eBLOCK: sType = "Block"; break;
		case eTICK:  sType = "Tick";  break;
	}
	ObjectInfo::WriteElement(sSection, "Type", en, sType);
	ObjectInfo::WriteElement(sSection, "Small Steps", en, fSmallSteps);
	ObjectInfo::WriteElement(sSection, "BarHeight", en, rBarThickness);	
	ObjectInfo::WriteElement(sSection, "Tick orientation", en, (int)eTickOrientation);
	ObjectInfo::WriteElement(sSection, "Indicate Units", en, fIndicateUnits);
	ObjectInfo::WriteElement(sSection, "Use km", en, fUseKm);
}

void ScaleBarLayoutItem::SetPosition(MinMax mm, int iHit)
{
	switch (iHit) {
		case CRectTracker::hitTopLeft:
		case CRectTracker::hitTopRight:
		case CRectTracker::hitBottomLeft:
		case CRectTracker::hitBottomRight:
		case CRectTracker::hitLeft:
		case CRectTracker::hitRight:
		{
			MinMax mmOldPos = mmPosition();
			double rScale = mli->rScale(); // 1:...
			int iDX = (int)(iStepLength * 10000.0 / rScale); // step size in 0.1 mm units
			double rDiff = mm.width() - mmOldPos.width();
			rDiff /= iDX;
			int iDiff = rounding(rDiff);
			if (0 == iDiff)
				if (rDiff > 0.1)
					iDiff = 1;
				else if (rDiff < -0.1)
					iDiff = -1;
			iSteps += iDiff;
			if (iSteps < 2)
				iSteps = 2;
			Setup();
		} break;
	}
	LayoutItem::SetPosition(mm, iHit);
}

String ScaleBarLayoutItem::sName() const
{
	if (0 == mli)
		return SLONameScaleBar;
	else
		return String(SLONameScaleBar_S.scVal(), mli->sName());
}

bool ScaleBarLayoutItem::fDependsOn(LayoutItem* li)
{
	return li == mli;
}

double ScaleBarLayoutItem::rScale() const   // 1:....
{
	return mli->rScale();
}

void ScaleBarLayoutItem::SetScale(double rScale)
{
	mli->SetScale(rScale);
}

bool ScaleBarLayoutItem::fAddExtraClipboardItems()
{
  // do nothing
	return true;
}

