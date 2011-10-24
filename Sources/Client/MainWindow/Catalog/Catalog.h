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
#if !defined(AFX_CATALOG_H__58D75853_D153_11D2_B729_00A0C9D5342F__INCLUDED_)
#define AFX_CATALOG_H__58D75853_D153_11D2_B729_00A0C9D5342F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Catalog.h : header file
//

#if !defined(ILWISHEADERCONTROL_H)
#include "Client\Editors\Utils\OwnerHeaderCtrl.h"
#endif

#include "Client\MainWindow\FileFilter.h"

class CatalogDocument;
class AssociationSelector;
class MapListCatalog;
/////////////////////////////////////////////////////////////////////////////
// Catalog view

#undef IMPEXP
#ifdef ILWISCLIENT
#define IMPEXP __declspec(dllexport)
#else
#define IMPEXP __declspec(dllimport)
#endif

struct LVColumnInfo;

class IMPEXP Catalog : public CListView
{
	friend class MDIFrameWindow;
	friend class MainWindow;
	friend class DirectoryDocument;
	friend class MapListCatalog;
public:
  enum ColumnTypes{ ctNAME, ctDEPENDENT, ctCALCULATED, ctUPTODATE, ctREADONLY, ctMODIFIED, 
	                  ctDOMAIN, ctDOMAINTYPE,
										ctCOORDSYSTEM,ctCOORDSYSTEMTYPE, ctGEOREF, ctGEOREFTYPE, ctGSIZE ,
										ctATTRIBUTETABLE, ctDESCRIPTION, ctEXPRESSION, ctTYPE, ctOBJECTSIZE,
										ctORDER, ctNONE, ctALL=ctNONE };  // ctALL is meant to be the last one
	enum CalcState{ csTRUE, csFALSE, csUNKNOWN };

	Catalog();
	CatalogDocument* GetDocument();
	void EraseCatalog();
	void FillCatalog();
	void SetFilterPane();
	void SetCatalogQuery(const String& sF);
	virtual void SaveCatalogView(const String& sDocKey, int iNr, bool fAsTemplate = false);
	virtual void LoadViewSettings(const String sDocKey, int iViewNr, bool fAsTemplate = false);
	void OnClose();
	void SetVisibleColumns(const vector<bool> &fColVis) ;
	void SetGridOnOf(bool fYesNo);
	bool fGrid() { return fShowGrid; }
	bool fColVisible(int iCol);
	bool fShowContainerContents();
	void ShowContainerContents(bool fYesNo);
	void IncludeSystemObjects( bool fYesNo);
	static void GetFromClipboard(vector<FileName>& arFiles);
	bool fIncludeSystemObjects();
	String sGetColumnName(int iCol);
	int iSortedColumn();
	FileFilter& GetFilter();
	void SetExternalFileExtensions(const String& sExt);
	String sGetExternalFileExtensions();
	void SetCurrentQuery(const String& str);

	//! will be filled by itself during OnContextMenu()	
	AssociationSelector* assSel; 
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Catalog)
	virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual void OnInitialUpdate();
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual bool fIsManualSortAvail();  // for change order by means of drag/drop, eg for maplist
	bool getUseBaseMaps() const { return useBasemaps; }
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~Catalog();

	// Generated message map functions
	//{{AFX_MSG(Catalog)
	afx_msg void OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCatList();
	afx_msg void OnCatDetails();
	afx_msg void OnUpdateCatList(CCmdUI*);
	afx_msg void OnUpdateCatDetails(CCmdUI*);
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu);
	afx_msg void OnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCatViewOptions();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnCopyTo();
	afx_msg void OnSelectAll();
	afx_msg void OnUpdateSelectAll(CCmdUI*);
	afx_msg void OnShowMapTables();
	afx_msg void OnShowAllData();
	afx_msg void OnShowAllObjects();
	afx_msg void OnShowBaseMaps();
	afx_msg void OnCreateObjectCollection();
	//}}AFX_MSG
	afx_msg void OnSelBut(UINT id);
	afx_msg void OnSelButUI(CCmdUI* pCmdUI);
	afx_msg void OnSortCol(UINT id);
	afx_msg void OnSortColUI(CCmdUI* pCmdUI);
	afx_msg	void OnUpdateCatalogQueries(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePyramids(CCmdUI* pCmdUI);

	void ShowContextViewMenu(CWnd* pWnd, CPoint point);
	bool CalcMenuProps(CWnd* pWns, CPoint point, FileName& fn, 	FileName& fnFileSub, 
		                   int& iNr, int&iSub, bool& fReport); // returns true when a menu should be shown

	COleDropTarget* odt;
	std::vector<FileName> vfn, vfnDisplay;
	std::vector<int> viIndex;
	String sCatalogQuery;
	int iSortColumn;
	bool fSortAscending;
	IlwisHeaderCtrl     m_HeaderCtrl;
	vector<LVColumnInfo> farColumns;
	vector< vector<String> > arsColumnStrings;
	map<String, bool> mapSeenFiles; // used to track circular dependencies. the bool signifies if a FileName is already seen
	FileFilter fil;
	bool fShowGrid;
	int iImgOvlSystem;
	int iImgOvlUseAs;
	bool fSystemObjects;
	String sExternalExtensions;
	String sTemp; // used by dispinfo, holds string while waiting for a draw;

	void QueryFileNames();
	virtual String sFieldValue(int iRow, int iCol);
	String sGetColumnString(int i, const FileName& fnObj, bool fExt=false);
	long iGetColumnString(int i, const FileName& fnObj);
	pair<long,long> piGetColumnString(int i, const FileName& fnObj);
	String sODFValue(const FileName& fn, const String& sSection, const String& sEntry, bool fIncludeType=false, bool fExt=false);
	map<String, String> UINames;
	CalcState csTraceUpToDate(const FileName& fnObj, int iTime=iUNDEF);
	FileName GetNextSelectedFile(POSITION& pos);

	virtual void SortColumn(int i);
	void OnHeaderClicked(NMHDR *pNMHDR, LRESULT *pResult);
	void OnHeaderSize(NMHDR* pNMHDR, LRESULT* pResult);
	void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	void OnCopy();
	void OnPaste();
	void OnUpdateEdit(CCmdUI* pCmdUI);
	void OnUpdatePaste(CCmdUI* pCmdUI);
	void OnOpen();
	void OnDelete();
	void OnOpenPixelInfo();
	void OnEditObject();
	void OnUpdateEditObject(CCmdUI* pCmdUI);
	void OnProp();
	BOOL OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point) ;
	void OnShow();
	void CreateColumns();
	void CopyObjects(vector<FileName>& arFiles, const FileName& fnWhere);
	void OnPropertiesUpdateUI(CCmdUI* pCmdUI);
	virtual void DeleteItems();
	void OnCatalogQueries(UINT iID)	;
	void OnPyramids();

	BOOL PreTranslateMessage(MSG* pMsg);
	void ShowStatusLineInfo(int id);
	void Execute(int id);
	long iObjectSize(const FileName& fn);
	long iGetObjectSize(const FileName& fn, const char* sSection = 0, const char *sKey = 0);
	bool fShowAll;

private:
	UINT FillColumnStringInThread();
	static UINT Catalog::FillColumnStringInThread(void *data);
	bool volatile fFilling;
	bool volatile fFillStop;
	String        sCurrentQuery;
	bool useBasemaps;

	int iFmtCopy;
	CCriticalSection csLockColumnStrings;

	DECLARE_DYNCREATE(Catalog)
	DECLARE_MESSAGE_MAP()
	friend class KeepCatalogSelection;
};

struct LVColumnInfo
{
	LVColumnInfo(bool fV=true, int iW=0, Catalog::ColumnTypes t=Catalog::ctNAME) 
		: fVisible(fV), iWidth(iW), type(t) 
	{}

	bool fVisible;
	int iWidth;
	Catalog::ColumnTypes type;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CATALOG_H__58D75853_D153_11D2_B729_00A0C9D5342F__INCLUDED_)
