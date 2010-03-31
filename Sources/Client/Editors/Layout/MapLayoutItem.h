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
// MapLayoutItem.h: interface for the MapLayoutItem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAPLAYOUTITEM_H__E692EED3_F6A0_11D3_B874_00A0C9D5342F__INCLUDED_)
#define AFX_MAPLAYOUTITEM_H__E692EED3_F6A0_11D3_B874_00A0C9D5342F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_GeneralBar_H__A9C4CEB4_A34D_11D3_B821_00A0C9D5342F__INCLUDED_)
#include "Client\Editors\Utils\GeneralBar.h"
#endif

//#if !defined(AFX_LAYERTREEBAR_H__4E781484_676B_11D3_B7DC_00A0C9D5342F__INCLUDED_)
//#include "Client\Mapwindow\LayerTreeBar.h"
//#endif


class MapCompositionDoc;
class MapCompositionSrvItem;
class ZoomableView;

class MapLayoutItem: public LayoutItem  
{
public:
	enum Side { sideTOP=1, sideBOTTOM, sideLEFT, sideRIGHT, sideANY};
	MapLayoutItem(LayoutDoc* ld, MapCompositionDoc*);
	virtual ~MapLayoutItem();
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual void OnDraw(CDC* cdc);
	virtual bool fIsotropic() const;
	virtual bool fClip() const;
	virtual void SetPosition(MinMax mm, int iHit);
	virtual double rHeightFact() const;
	virtual bool fConfigure();
	virtual String sType() const;
	virtual String sName() const;
	virtual void ReadElements(ElementContainer&, const char* sSection);
	virtual void WriteElements(ElementContainer&, const char* sSection);
	virtual void OnContextMenu(CWnd* pWnd, CPoint point);
	double rScale() const;   // 1:....
	void InitScale();
	void SetScale(double rScale);
	virtual void SaveModified();
	double rAzimuth() const; // in radians
	CoordBounds cbBounds() const;
	Coord cConv(CPoint pt) const;	// pt in 0.1 mm units
	LatLon llConv(const Coord& crd) const; 
	LatLon llConv(CPoint pt) const;	// pt in 0.1 mm units
	CPoint ptBorderX(Side, double rX) const;
	CPoint ptBorderY(Side, double rY) const;
	CPoint ptBorderLat(Side, double rLat) const;
	CPoint ptBorderLon(Side, double rLon) const;
	MapCompositionDoc* mcd();
	const MapCompositionDoc* mcd() const;
	afx_msg void OnAddScaleText();
	afx_msg void OnAddScaleBar();
	afx_msg void OnAddNorthArrow();
	afx_msg void OnAddLegend();
	afx_msg void OnAddMapBorder();
	afx_msg void OnEntireMap();
	afx_msg void OnItemReplaceMapView();
	afx_msg void OnRedraw();
	afx_msg void OnSetScale();
	afx_msg void OnUpdateValidGeoRef(CCmdUI* pCmdUI);
	afx_msg void OnUpdateValidScale(CCmdUI* pCmdUI);
	afx_msg void OnKeepScale();
	afx_msg void OnUpdateKeepScale(CCmdUI* pCmdUI);
	void OnSelectArea(ZoomableView*);			
	void AreaSelected(CRect rect);
	void OnPanRect(ZoomableView*);			
	void PanMove(CPoint pt);
protected:
	void Init();
	CPoint ptConv(double rRow, double rCol) const;	// pt in 0.1 mm units
	void InitMetafile();
	FileName fnMapView;
	double m_rScale;
	ZoomableView* zv;
	MapCompositionSrvItem* mcsi;
	GeneralBar ltb;
	bool fCreatedLayerTreeBar;
	HMETAFILE hMF;
	DECLARE_MESSAGE_MAP()
	friend class MapLayoutItemForm;
	bool fBusyDrawing;
private:
	bool fKeepScale;
};			

#endif // !defined(AFX_MAPLAYOUTITEM_H__E692EED3_F6A0_11D3_B874_00A0C9D5342F__INCLUDED_)
