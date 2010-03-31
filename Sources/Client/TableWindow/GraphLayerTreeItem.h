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
// GraphLayerTreeItem.h: interface for the GraphLayerTreeItem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRAPHLAYERTREEITEM_H__E0A6D3A0_9BC9_4C43_B79C_B35E63F0A7D9__INCLUDED_)
#define AFX_GRAPHLAYERTREEITEM_H__E0A6D3A0_9BC9_4C43_B79C_B35E63F0A7D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class GraphLayerTreeView;
class GraphLayer;
class GraphAxis;
class GraphTreeField;
class GraphDrawer;

class GraphTreeItem  
{
public:
	GraphTreeItem(GraphLayerTreeView*);
	virtual ~GraphTreeItem();
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnLButtonDblClk(UINT nFlags, CPoint point);
	virtual void SwitchCheckBox(bool fOn);
  virtual void OnEdit();
	virtual bool OnContextMenu(CWnd* pWnd, CPoint pos);
	virtual void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult); 
  bool fCheckBox;
protected:
	GraphLayerTreeView* ltv;
};

class GraphLayerTreeItem : public GraphTreeItem
{
public:
	GraphLayerTreeItem(GraphLayerTreeView*, GraphLayer*);
	virtual ~GraphLayerTreeItem();
	virtual void SwitchCheckBox(bool fOn);
  virtual void OnEdit();
	virtual bool OnContextMenu(CWnd* pWnd, CPoint pos);
	virtual void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult); 
  void DisplayOptions();
  void Remove();
  GraphLayer* glGet() const { return gl; };
private:
  GraphLayer* gl;
};

class GraphAxisTreeItem : public GraphTreeItem
{
public:
	GraphAxisTreeItem(GraphLayerTreeView*, GraphAxis*);
	virtual ~GraphAxisTreeItem();
  virtual void OnEdit();
	virtual void SwitchCheckBox(bool fOn);
private:
  GraphAxis* ga;
};

class ObjectGraphTreeItem: public GraphTreeItem
{
public:
	ObjectGraphTreeItem(GraphLayerTreeView*, IlwisObjectPtr*);
	virtual ~ObjectGraphTreeItem();
  virtual void OnEdit();
	virtual bool OnContextMenu(CWnd* pWnd, CPoint pos);
private:
	IlwisObjectPtr* ptr;
};

class StringGraphTreeItem : public GraphTreeItem
{
public:
	StringGraphTreeItem(GraphLayerTreeView*, HTREEITEM, String*);
	virtual ~StringGraphTreeItem();
  virtual void OnEdit();
  void OnNotifyFieldFinished();
private:
  HTREEITEM hti;
  String* str;
  GraphTreeField* gtf;
};

class GraphAxisMinTreeItem : public GraphTreeItem
{
public:
	GraphAxisMinTreeItem(GraphLayerTreeView*, HTREEITEM, GraphAxis*);
	virtual ~GraphAxisMinTreeItem();
  virtual void OnEdit();
  void OnNotifyFieldFinished();
private:
  HTREEITEM hti;
  GraphAxis* ga;
  GraphTreeField* gtf;
};

class GraphAxisMaxTreeItem : public GraphTreeItem
{
public:
	GraphAxisMaxTreeItem(GraphLayerTreeView*, HTREEITEM, GraphAxis*);
	virtual ~GraphAxisMaxTreeItem();
  virtual void OnEdit();
  void OnNotifyFieldFinished();
private:
  HTREEITEM hti;
  GraphAxis* ga;
  GraphTreeField* gtf;
};

class BoolGraphTreeItem : public GraphTreeItem
{
public:
	BoolGraphTreeItem(GraphLayerTreeView*, bool*);
	virtual ~BoolGraphTreeItem();
	virtual void SwitchCheckBox(bool fOn);
private:
  bool* fFlag;
};

class GraphTitleTreeItem : public GraphTreeItem
{
public:
	GraphTitleTreeItem(GraphLayerTreeView*, GraphDrawer*);
	virtual ~GraphTitleTreeItem();
  virtual void OnEdit();
private:
  GraphDrawer* gd;
};





#endif // !defined(AFX_GRAPHLAYERTREEITEM_H__E0A6D3A0_9BC9_4C43_B79C_B35E63F0A7D9__INCLUDED_)
