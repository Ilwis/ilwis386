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
#ifndef DataBaseCatalog_H
#define DataBaseCatalog_H

#undef IMPEXP
#ifdef ILWISCLIENT
#define IMPEXP __declspec(dllexport)
#else
#define IMPEXP __declspec(dllimport)
#endif

class ObjectCollectionDoc;
class ObjectLister;
class DataBaseCollectionDoc;

class IMPEXP DataBaseCatalog : public CollectionCatalog
{
public:
	~DataBaseCatalog();

	DataBaseCollectionDoc* GetDocument();

protected:
	DataBaseCatalog();           // protected constructor used by dynamic creation
	void OnAddObject();
//	void OnUpdateAddObject(CCmdUI* pCmdUI);
	void OnRemoveObject();
	void OnCatDel();
	void OnCopy();
	void OnPaste();
	void OnSetView() ;
	void OnInitialUpdate();
	BOOL OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
	DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) ;
	virtual bool fOK(const FileName& fn);
	virtual void DeleteItems();
	virtual String sAddObjectTitle();
	virtual String sMsgAddObject();
	virtual ObjectLister *olActiveLister(const String& sExt);
	void OnDelete();
	void OnUpdateDelete(CCmdUI* pCmdUI)	;
	void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	void OnContextMenu(CWnd* pWnd, CPoint point) ;
	void OnOpenQuery();
	void OnShow();
	void OnUpdatePaste(CCmdUI* pCmdUI);
	void OnUpdateCopy(CCmdUI* pCmdUI);	
	void OnUpdateAddObject(CCmdUI* pCmdUI);
	void CreateTable(const FileName& fn);

	String sQuery;
	String sViewName;

	DECLARE_DYNCREATE(DataBaseCatalog);

	DECLARE_MESSAGE_MAP()
};

#endif
