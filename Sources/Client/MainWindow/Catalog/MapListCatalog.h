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
#ifndef MapListCatalog_H
#define MapListCatalog_H

#undef IMPEXP
#ifdef ILWISCLIENT
#define IMPEXP __declspec(dllexport)
#else
#define IMPEXP __declspec(dllimport)
#endif

class MapListDoc;

class IMPEXP MapListCatalog : public CollectionCatalog
{
public:
	~MapListCatalog();

	MapListDoc* GetDocument();

protected:
	MapListCatalog();           // protected constructor used by dynamic creation
	virtual bool fOK(const FileName& fn);
	virtual String sFieldValue(int iRow, int iCol);
	virtual bool fIsManualSortAvail();  // for change order by means of drag/drop, eg for maplist
	virtual String sAddObjectTitle();
	virtual String sMsgAddObject();
	virtual ObjectLister *olActiveLister(const String& sExt);
	void OnInitialUpdate();
	void SaveCatalogView(const String& sDocKey, int iNr, bool fAsTemplate = false);
	void LoadViewSettings(const String sDocKey, int iViewNr, bool fAsTemplate = false);
	void SortColumn(int i);

	//{{AFX_MSG(MapListCatalog)
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnUpdateDisable(CCmdUI* pCmdUI);
	afx_msg void OnSelButUI(CCmdUI* pCmdUI);
	//}}AFX_MSG
private:
	bool fInOrder();
	DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	BOOL OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point) ;

	DECLARE_DYNCREATE(MapListCatalog);
	DECLARE_MESSAGE_MAP()
};

#endif
