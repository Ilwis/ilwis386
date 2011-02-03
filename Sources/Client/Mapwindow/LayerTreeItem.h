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

#pragma once

using namespace ILWIS;

class _export LayerTreeItem  
{
public:
	LayerTreeItem(LayerTreeView*);
	virtual ~LayerTreeItem();
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnLButtonDblClk(UINT nFlags, CPoint point);
	virtual void SwitchCheckBox(bool fOn);
	virtual void OnContextMenu(CWnd* pWnd, CPoint pos);
	virtual void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	LayerTreeView *getTreeView() { return ltv; }
protected:
	LayerTreeView* ltv;
	HTREEITEM htiStart;
};

class DrawerLayerTreeItem: public LayerTreeItem
{
public:
	DrawerLayerTreeItem(LayerTreeView*, ILWIS::NewDrawer*);
	virtual ~DrawerLayerTreeItem();
	virtual void OnLButtonDblClk(UINT nFlags, CPoint point);
	virtual void SwitchCheckBox(bool fOn);
	virtual void OnContextMenu(CWnd* pWnd, CPoint pos);
	virtual void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult); 
	ILWIS::NewDrawer* drw() const { return dr; }
protected:
	ILWIS::NewDrawer* dr;		 
	enum { eNAME, eDESCR, eNAMEDESCR } eText;
};

class _export ObjectLayerTreeItem: public LayerTreeItem
{
public:
	ObjectLayerTreeItem(LayerTreeView*, IlwisObjectPtr*);
	virtual ~ObjectLayerTreeItem();
	virtual void OnLButtonDblClk(UINT nFlags, CPoint point);
	//void SwitchCheckBox(bool fOn);
	virtual void OnContextMenu(CWnd* pWnd, CPoint pos);
private:
	IlwisObjectPtr* ptr;
};

class _export LegendLayerTreeItem: public LayerTreeItem
{
public:
	LegendLayerTreeItem(LayerTreeView*, ILWIS::NewDrawer*);
	virtual ~LegendLayerTreeItem();
	virtual void OnLButtonDblClk(UINT nFlags, CPoint point);
	virtual void OnContextMenu(CWnd* pWnd, CPoint pos);
private:
	ILWIS::NewDrawer *dr;
	 
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
	ColumnLayerTreeItem(LayerTreeView*, ILWIS::AbstractMapDrawer*, HTREEITEM hti);
	virtual ~ColumnLayerTreeItem();
	virtual void SwitchCheckBox(bool fOn);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnContextMenu(CWnd* pWnd, CPoint pos);
	AbstractMapDrawer * mdr() { return dr;}
private:
	void ShowColumnField();
	ILWIS::AbstractMapDrawer *dr;
	void FinishColumnField();
	HTREEITEM hti;
	ChooseColumnComboBox* cccb;
};

class _export LegendClassLayerTreeItem: public LayerTreeItem
{
public:
	LegendClassLayerTreeItem(LayerTreeView*, ILWIS::NewDrawer*, Domain _dm, int iRaw);
	virtual ~LegendClassLayerTreeItem();
	virtual void OnLButtonDblClk(UINT nFlags, CPoint point);
	virtual void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult); 
	virtual void SwitchCheckBox(bool fOn);
private:
	Domain dm;
	int iRaw;

	ILWIS::NewDrawer *dr;

};

class _export LegendValueLayerTreeItem: public LayerTreeItem
{
public:
	LegendValueLayerTreeItem(LayerTreeView*, NewDrawer*, DomainValueRangeStruct _dvrs, double rValue);
	virtual ~LegendValueLayerTreeItem();
	virtual void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
private:
	DomainValueRangeStruct dvrs;
	double rVal;
	NewDrawer *dr;
};

class DisplayOptionTree: public LayerTreeItem
{
public:
	DisplayOptionTree(LayerTreeView*, HTREEITEM hti);
	virtual ~DisplayOptionTree();
	virtual void OnLButtonDblClk(UINT nFlags, CPoint point);
private:
	HTREEITEM htiStart;
};

class _export DisplayOptionTreeItem: public LayerTreeItem
{
public:
	DisplayOptionTreeItem(LayerTreeView*, HTREEITEM parent, ILWIS::NewDrawer *dr, DisplayOptionItemFunc f,HTREEITEM item=0, SetChecks *checks=0, SetCheckFunc cf=0);
	DisplayOptionTreeItem(LayerTreeView*, HTREEITEM parent, ILWIS::NewDrawer *dr, SetCheckFunc f,DisplayOptionItemFunc fun=0, ILWIS::NewDrawer *_altHandler=0);
	virtual ~DisplayOptionTreeItem();
	virtual void OnLButtonDblClk(UINT nFlags, CPoint point);
	void SwitchCheckBox(bool fOn);
	HTREEITEM getParent() { return parent;}
	void setTreeItem(HTREEITEM it) ;
	
private: 
	DisplayOptionItemFunc func;
	SetCheckFunc setCheckFunc;
	ILWIS::NewDrawer *drw;
	ILWIS::NewDrawer *altHandler;
	SetChecks *checks;
	HTREEITEM hti;
	HTREEITEM parent;
};

class _export DisplayOptionColorItem: public DisplayOptionTreeItem
{
public:
	DisplayOptionColorItem(const String& sText, LayerTreeView*, HTREEITEM parent, ILWIS::NewDrawer *dr, DisplayOptionItemFunc f,HTREEITEM item=0, SetChecks *checks=0, SetCheckFunc cf=0);
	//DisplayOptionColorItem(LayerTreeView*, ILWIS::NewDrawer *dr, SetCheckFunc f,DisplayOptionItemFunc fun=0, HTREEITEM item=0, ILWIS::NewDrawer *_altHandler=0);
	void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	void setColor(Color c) ;
private:
	Color color;
	String sText;

};



class _export SetChecks {
public:
	SetChecks(LayerTreeView*, NewDrawer *dr,SetCheckFunc _f);
	~SetChecks();
	void addItem(HTREEITEM hti);
	void checkItem(HTREEITEM hti,bool fOn);
private:
	vector<HTREEITEM> checkedItems;
	LayerTreeView *tv;
	NewDrawer *drw;
	SetCheckFunc fun;

};



