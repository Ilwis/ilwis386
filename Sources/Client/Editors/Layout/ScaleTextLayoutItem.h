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
// ScaleTextLayoutItem.h: interface for the ScaleTextLayoutItem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCALETEXTLAYOUTITEM_H__D63A9FF5_F8B8_11D3_B875_00A0C9D5342F__INCLUDED_)
#define AFX_SCALETEXTLAYOUTITEM_H__D63A9FF5_F8B8_11D3_B875_00A0C9D5342F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_TEXTLAYOUTITEM_H__42F0FC94_F907_11D3_B875_00A0C9D5342F__INCLUDED_)
#include "Client\Editors\Layout\TextLayoutItem.h"
#endif

class MapLayoutItem;

class ScaleTextLayoutItem: public TextLayoutItem  
{
public:
	ScaleTextLayoutItem(LayoutDoc* ld, MapLayoutItem*);
	ScaleTextLayoutItem(LayoutDoc* ld);
	virtual ~ScaleTextLayoutItem();
	virtual void ReadElements(ElementContainer&, const char* sSection);
	virtual void WriteElements(ElementContainer&, const char* sSection);
	virtual void OnDraw(CDC* cdc);
	virtual bool fOnChangedItemSize(LayoutItem*);
	virtual bool fConfigure();
	virtual String sType() const;
	virtual String sName() const;
	virtual bool fDependsOn(LayoutItem*);
	virtual double rScale() const;   // 1:....
	virtual void SetScale(double rScale);
protected:
	MapLayoutItem* mli;
	DECLARE_MESSAGE_MAP()
	friend class ScaleTextLayoutItemForm;
};

#endif // !defined(AFX_SCALETEXTLAYOUTITEM_H__D63A9FF5_F8B8_11D3_B875_00A0C9D5342F__INCLUDED_)
