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
// MapBorderItem.h: interface for the MapBorderItem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAPBORDERITEM_H__04282E78_0617_11D4_B885_00A0C9D5342F__INCLUDED_)
#define AFX_MAPBORDERITEM_H__04282E78_0617_11D4_B885_00A0C9D5342F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_TEXTLAYOUTITEM_H__42F0FC94_F907_11D3_B875_00A0C9D5342F__INCLUDED_)
#include "Client\Editors\Layout\TextLayoutItem.h"
#endif

#ifndef LINE_H
#include "Client\Editors\Utils\line.h"
#endif

class MapLayoutItem;

class MapBorderItem: public TextLayoutItem  
{
public:
	MapBorderItem(LayoutDoc* ld, MapLayoutItem*);
	MapBorderItem(LayoutDoc* ld);
	virtual ~MapBorderItem();
	virtual void ReadElements(ElementContainer&, const char* sSection);
	virtual void WriteElements(ElementContainer&, const char* sSection);
	virtual void OnDraw(CDC* cdc);
	virtual bool fConfigure();
	virtual String sType() const;
	virtual String sName() const;
	virtual bool fDependsOn(LayoutItem*);
	virtual bool fOnChangedItemSize(LayoutItem*);
	virtual void SetPosition(MinMax mm, int iHit);
	virtual bool fIsotropic() const;
protected:
	enum CoordType { eFULLCOORDS, eSHORTENEDCOORDS };
	MinMax mmPosBasedOnMli() const;
	MinMax mmWishForMli(MinMax) const;
	void DrawHorizontalGraduation(CDC* cdc, vector<CPoint>&, int iWidth) const;
	void DrawVerticalGraduation(CDC* cdc, vector<CPoint>&, int iWidth) const;
	void DrawHorizontalTickMarks(CDC* cdc, vector<CPoint>&, int iWidth, Color clr) const;
	void DrawVerticalTickMarks(CDC* cdc, vector<CPoint>&, int iWidth, Color clr) const;
	String sTextMetric(double rVal, CoordType ct) const;
	String sTextDMS(double rVal, bool fLat, CoordType ct) const;
	void DrawHorizontalTexts(CDC* cdc, vector<CPoint>&, vector<double>&, int iWidth, bool fMetric, CoordType ct, MapLayoutItem::Side side = MapLayoutItem::sideANY, int iRotation=0) const;
	void DrawVerticalTexts(CDC* cdc, vector<CPoint>&, vector<double>&, int iWidth, bool fMetric, CoordType ct, MapLayoutItem::Side=MapLayoutItem::sideANY, int iRotation=0) const;
	MapLayoutItem* mli;
	bool fBorderLine, fNeatLineGraduation, fGridTicks, fGratTicks, fGridText, fGratText, fCornerCoords;
	enum { eMETRES, eDEGREES } eGraduationType;
	double rNeatLineInterval, rNeatLineDegreesInterval, rGridTickInterval, rGratTickInterval, rGridTextInterval, rGratTextInterval; // m or degrees
	double rTickLineWidth, rNeatLineWidth, rGridTickLength, rGratTickLength, rGridTextOffset, rGratTextOffset; // mm
	Color clrNeatLine, clrGridTick, clrGratTick, clrGridText, clrGratText, clrCornerCoords;
	CoordType eGridTextType, eGratTextType;
	LOGFONT lfGridText, lfGratText;
	double rBorderLineDist;	// in mm
	Line lnBorder;
private:
	bool fLatHor;
	bool arfEnabledTextLocations[5]; //base index = 1
	bool arfEnabledGratTextLocations[5]; //base index = 1	
	int iHorzBorderTextRot;
	int iVertBorderTextRot;	
	int iHorzBorderGratTextRot;
	int iVertBorderGratTextRot;	
	
	friend class MapBorderItemForm;

};

#endif // !defined(AFX_MAPBORDERITEM_H__04282E78_0617_11D4_B885_00A0C9D5342F__INCLUDED_)
