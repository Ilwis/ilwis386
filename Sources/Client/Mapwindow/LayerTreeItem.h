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
// LayerTreeItem.h: interface for the LayerTreeItem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LAYERTREEITEM_H__0340C284_8C41_11D3_B809_00A0C9D5342F__INCLUDED_)
#define AFX_LAYERTREEITEM_H__0340C284_8C41_11D3_B809_00A0C9D5342F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class LayerTreeItem  
{
public:
	LayerTreeItem(LayerTreeView*);
	virtual ~LayerTreeItem();
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnLButtonDblClk(UINT nFlags, CPoint point);
	virtual void SwitchCheckBox(bool fOn);
	virtual void OnContextMenu(CWnd* pWnd, CPoint pos);
	virtual void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult); 
protected:
	LayerTreeView* ltv;
};

class DrawerLayerTreeItem: public LayerTreeItem
{
public:
	DrawerLayerTreeItem(LayerTreeView*, Drawer*);
	virtual ~DrawerLayerTreeItem();
	virtual void OnLButtonDblClk(UINT nFlags, CPoint point);
	virtual void SwitchCheckBox(bool fOn);
	virtual void OnContextMenu(CWnd* pWnd, CPoint pos);
	virtual void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult); 
	Drawer* drw() const { return dr; }
protected:
	Drawer* dr;		 
	enum { eNAME, eDESCR, eNAMEDESCR } eText;
};

class ObjectLayerTreeItem: public LayerTreeItem
{
public:
	ObjectLayerTreeItem(LayerTreeView*, IlwisObjectPtr*);
	virtual ~ObjectLayerTreeItem();
	virtual void OnLButtonDblClk(UINT nFlags, CPoint point);
	virtual void OnContextMenu(CWnd* pWnd, CPoint pos);
private:
	IlwisObjectPtr* ptr;
};

class LegendLayerTreeItem: public LayerTreeItem
{
public:
	LegendLayerTreeItem(LayerTreeView*, Drawer*);
	virtual ~LegendLayerTreeItem();
	virtual void OnLButtonDblClk(UINT nFlags, CPoint point);
	virtual void OnContextMenu(CWnd* pWnd, CPoint pos);
private:
	Drawer* dr;		 
};

class PropertiesLayerTreeItem: public LayerTreeItem
{
public:
	PropertiesLayerTreeItem(LayerTreeView*, IlwisObjectPtr*);
	virtual ~PropertiesLayerTreeItem();
	virtual void OnLButtonDblClk(UINT nFlags, CPoint point);
private:
	IlwisObjectPtr* ptr;
};

class ChooseColumnComboBox;
class ColumnLayerTreeItem: public LayerTreeItem
{
	friend class ChooseColumnComboBox;
public:
	ColumnLayerTreeItem(LayerTreeView*, BaseMapDrawer*, HTREEITEM hti);
	virtual ~ColumnLayerTreeItem();
	virtual void SwitchCheckBox(bool fOn);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnContextMenu(CWnd* pWnd, CPoint pos);
private:
	void ShowColumnField();
	void FinishColumnField();
	BaseMapDrawer* bmd;		 
	HTREEITEM hti;
	ChooseColumnComboBox* cccb;
};

class LegendClassLayerTreeItem: public LayerTreeItem
{
public:
	LegendClassLayerTreeItem(LayerTreeView*, Drawer*, Domain _dm, int iRaw);
	virtual ~LegendClassLayerTreeItem();
	virtual void OnLButtonDblClk(UINT nFlags, CPoint point);
	virtual void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult); 
private:
	Drawer* dr;
	Domain dm;
	int iRaw;
};

class LegendValueLayerTreeItem: public LayerTreeItem
{
public:
	LegendValueLayerTreeItem(LayerTreeView*, Drawer*, DomainValueRangeStruct _dvrs, double rValue);
	virtual ~LegendValueLayerTreeItem();
	virtual void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult); 
private:
	Drawer* dr;
	DomainValueRangeStruct dvrs;
	double rVal;
};




#endif // !defined(AFX_LAYERTREEITEM_H__0340C284_8C41_11D3_B809_00A0C9D5342F__INCLUDED_)
