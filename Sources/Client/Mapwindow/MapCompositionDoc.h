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
#if !defined(AFX_MAPCOMPOSITIONDOC_H__8A842674_E359_11D2_B73E_00A0C9D5342F__INCLUDED_)
#define AFX_MAPCOMPOSITIONDOC_H__8A842674_E359_11D2_B73E_00A0C9D5342F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MapCompositionDoc.h : header file
//

#ifndef CATALOGDOCUMENT_H
#include "Client\MainWindow\Catalog\CatalogDocument.h"
#endif

#ifndef ILWMAPVIEW_H
#include "Engine\Map\Mapview.h"
#endif

#include "Client/MapWindow/Drawers/Drawer_n.h"
#include "Client/MapWindow/Drawers/RootDrawer.h"

class MapCompositionSrvItem;


#undef IMPEXP
#ifdef ILWISCLIENT
#define IMPEXP __declspec(dllexport)
#else
#define IMPEXP __declspec(dllimport)
#endif

class Drawer;
class GeneralBar;
class Palette;
class MapPaneView;
class StereoPair;

/////////////////////////////////////////////////////////////////////////////
// MapCompositionDoc document

class IMPEXP MapCompositionDoc : public CatalogDocument
{
public:
	static const CLSID clsid;
	MapCompositionDoc();           // protected constructor used by dynamic creation
	virtual ~MapCompositionDoc();
  virtual IlwisObject obj() const;
  virtual zIcon icon() const;

	ILWIS::RootDrawer *rootDrawer;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(MapCompositionDoc)
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName, OpenType ot);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPath, ParmList& pm);
	virtual COleServerItem* OnGetEmbeddedItem();
	virtual BOOL OnNewDocument();
	virtual BOOL SaveModified();
	virtual void OnDeactivate();
	virtual BOOL GetFileTypeString(CString& rString);
	void OnExtPerc();	
	//}}AFX_VIRTUAL
	BOOL fSaveModified(bool fAllowCancel);

	MapCompositionSrvItem* GetEmbeddedItem()
		{ return (MapCompositionSrvItem*)CatalogDocument::GetEmbeddedItem(); }
	MapPaneView* mpvGetView() const;

	BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo); 
	void initBounds(MinMax mm);
  bool fCoordSystemOk(const BaseMap& bmap);
  bool fGeoRefOk(const Map& map);
	bool fAppendable(const FileName&);
	Drawer* drAppend(const FileName&);
	Drawer* drAppend(const Map&);
	Drawer* drAppend(const MapList&);
	Drawer* drAppend(const SegmentMap&);
	Drawer* drAppend(const PolygonMap&);
	Drawer* drAppend(const PointMap&);
	Drawer* drAppend(const AnnotationText&);
//	Drawer* drAppend(HENHMETAFILE);
//	Drawer* drAppend(HBITMAP);
	void RemoveDrawer(Drawer* dr);
	void SetCoordSystem(const CoordSystem&);
	void SetCoordSystem(const GeoRef&);
	void SetGeoRef(const GeoRef&); // no checks!
	double rPrefScale() const { return rDfltScale; }
	RowCol rcPrefOffset() const { return rcDfltOffset; }
	void SetScale(double rScale) { rDfltScale = rScale; }
	void SetOffset(RowCol rc) { rcDfltOffset = rc; }
	afx_msg void OnSaveView();
	afx_msg void OnSaveViewAs();
  Map mp;
  GeoRef georef;
  Color colBackground;
	MinMax mmBounds() const;
  void menLayers(CMenu& men, int iBaseId);
	int iState() const { return iListState; }
	void ChangeState();
  void SetBounds(const MinMax&);
  void SetBounds(const CoordBounds&);
	bool fIsEmpty() const;

    list<Drawer*> dl;
	bool fShowRowCol;
	bool fRaster, fGrid3DDrawer;
	CSize szPrefSize;

protected:
	MapView mpv;
	Drawer* drDrawer(const MapView&, const char* sSection);
private:
  void SetTitle(const IlwisObject& obj);
	void StoreView();
	String getForeignType(const Map& mp);
	BOOL OnOpenRasterMap(const Map&, OpenType ot);
	BOOL OnOpenMapList(const MapList&, OpenType ot);
	BOOL OnOpenSegmentMap(const SegmentMap&, OpenType ot);
	BOOL OnOpenPolygonMap(const PolygonMap&, OpenType ot);
	BOOL OnOpenPointMap(const PointMap&, OpenType ot);
	BOOL OnOpenMapView(const MapView&);
	BOOL OnOpenGeoRef3D(const GeoRef& grf, OpenType ot);
	BOOL OnOpenStereoPair(const StereoPair&, OpenType ot);
	virtual void DeleteContents();

private:
	afx_msg void OnExtCoord();
	afx_msg void OnUpdateExtCoord(CCmdUI* pCmdUI);
	afx_msg void OnAddLayer();
	afx_msg void OnAddRasMap();
	afx_msg void OnAddSegMap();
	afx_msg void OnAddPolMap();
	afx_msg void OnAddPntMap();
	afx_msg void OnAddGrid();
	afx_msg void OnAddGraticule();
	afx_msg void OnUpdateAddGrid(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAddGraticule(CCmdUI* pCmdUI);
	afx_msg void OnAddAnnText();
//	afx_msg void OnAddNorthArrow();
//	afx_msg void OnAddBitmap();
	afx_msg void OnChangeCoordSystem();
	afx_msg void OnUpdateChangeCoordSystem(CCmdUI* pCmdUI);
	afx_msg void OnOpen();
	afx_msg void OnCreateLayout();

	afx_msg void OnCopyScaleBarLink();
	afx_msg void OnOpenPixelInfo();
  afx_msg void OnDataLayer(UINT nID);	
  afx_msg void OnEditLayer(UINT nID);	
  afx_msg void OnDomainLayer(UINT nID);	
  afx_msg void OnRprLayer(UINT nID);	
  afx_msg void OnPropLayer(UINT nID);	
  afx_msg void OnUpdateDataLayer(CCmdUI* pCmdUI);	
  afx_msg void OnUpdateEditLayer(CCmdUI* pCmdUI);	
  afx_msg void OnUpdateDomainLayer(CCmdUI* pCmdUI);	
  afx_msg void OnUpdateRprLayer(CCmdUI* pCmdUI);	
  afx_msg void OnUpdatePropLayer(CCmdUI* pCmdUI);	

	afx_msg void OnBackgroundColor();
	afx_msg void OnShowHistogram();
  afx_msg void OnUpdateShowHistogram(CCmdUI* pCmdUI);	
private:
	MinMax mmInitGeoRef(const BaseMap& bm);	

	MinMax mmMapBounds, mmSize;
	int iListState; // with every change increases
	double rDfltScale;
	RowCol rcDfltOffset;
	GeneralBar* gbHist;
	bool fInCmdMsg;
	DECLARE_DYNCREATE(MapCompositionDoc)
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAPCOMPOSITIONDOC_H__8A842674_E359_11D2_B73E_00A0C9D5342F__INCLUDED_)
