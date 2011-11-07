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
#if !defined(AFX_LAYERTREEVIEW_H__16CA23E5_5B8E_11D3_B7D6_00A0C9D5342F__INCLUDED_)
#define AFX_LAYERTREEVIEW_H__16CA23E5_5B8E_11D3_B7D6_00A0C9D5342F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class MapCompositionDoc;
class LayerTreeItem;

struct NodeInfo{
	NodeInfo(HTREEITEM it=0, bool ex = false) : hItem(it), expanded(ex) {}
	HTREEITEM hItem;
	bool expanded;
};

namespace ILWIS {
	class SpatialDataDrawer;
	class TextLayerDrawer;
	class DrawerTool;
}

struct TreeItem{
	TreeItem()  {
		item.pszText = new char[1024];
		item.cchTextMax = 1024;
	}
	TreeItem(TVITEM& _item) : item(_item) {
		item.pszText = new char[1024];
		item.cchTextMax = 1024;
	}
	operator TVITEM() { return item; }
	~TreeItem() { delete item.pszText; }

	TVITEM item;
};

class _export LayerTreeView : public CTreeView
{
public:
	LayerTreeView();           
	virtual ~LayerTreeView();
	MapCompositionDoc* GetDocument();
	void EditNamedLayer(const FileName& fn);

	virtual void OnInitialUpdate();
	virtual DROPEFFECT OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual void OnDragLeave();
	virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual BOOL OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg virtual void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBeginDrag(NMHDR* pNMHDR, LRESULT* pResult);
	void OnExpanding(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRemoveLayer();

	String getItemName(HTREEITEM item) const;
	HTREEITEM getItemHandle(const String& name) const;
	HTREEITEM getAncestor(HTREEITEM current, int depth);
	bool getItem(HTREEITEM hItem, UINT mask, TreeItem& item) const;
	void collectStructure(HTREEITEM parent=0, const String& name="");
	void DeleteAllItems(HTREEITEM hti, bool childerenOnly=false);
	ILWIS::DrawerTool *getRootTool();
	LayerTreeItem *getCurrent() const;
	//}}AFX_MSG
protected:
	void SwitchCheckBox(HTREEITEM hti);
	void DeleteAllItems();
	void OnDestroy();
//	void AddPropItems(HTREEITEM hti, int iImg, const IlwisObject& obj);
	CImageList ilStates;
private:
	HTREEITEM addMapItem(ILWIS::SpatialDataDrawer *mapDrawer, HTREEITEM after, int lastTool);
	void NextNode(HTREEITEM hItem, const String& name);
	void resetState();
	map<String, NodeInfo> nodes;
	COleDropTarget* odt;
	bool fDragging;
	HTREEITEM hDraggedItem;	
	CImageList *pDragImageList;
	ILWIS::DrawerTool *drwTool;
	LayerTreeItem *currentItem;
	DECLARE_MESSAGE_MAP()
public:
#ifdef _DEBUG
	virtual void AssertValid() const;
#endif //_DEBUG
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LAYERTREEVIEW_H__16CA23E5_5B8E_11D3_B7D6_00A0C9D5342F__INCLUDED_)
