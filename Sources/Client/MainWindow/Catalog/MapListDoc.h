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
#if !defined(AFX_MAPLISTDOC_H__73850EC4_77E2_11D3_B7F1_00A0C9D5342F__INCLUDED_)
#define AFX_MAPLISTDOC_H__73850EC4_77E2_11D3_B7F1_00A0C9D5342F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MapListDoc.h : header file
//

#ifndef OBJECTCOLLECTIONDOC_H
#include "Client\MainWindow\Catalog\ObjectCollectionDoc.h"
#endif

/////////////////////////////////////////////////////////////////////////////
// MapListDoc document

#undef IMPEXP
#ifdef ILWISCLIENT
#define IMPEXP __declspec(dllexport)
#else
#define IMPEXP __declspec(dllimport)
#endif

class IMPEXP MapListDoc: public CatalogDocument

{
protected:
	MapListDoc();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(MapListDoc)
public:
	public:
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual IlwisObject obj() const;
	virtual void GetFileNames(vector<FileName>& vfn, vector<FileName>& vfnDisplay);
	virtual zIcon icon() const;
	String sName() const;
	virtual void SaveSettings(IlwisSettings& settings,int iNr);
	virtual void AddObject(const FileName& fn);
	virtual void RemoveObject(const FileName& fn);
  virtual bool fEditable() const;
	virtual String sGetPathName() const;
	Domain dm();
	GeoRef gr();
	RowCol rcSize();
	virtual ~MapListDoc();
	virtual String sAllowedTypes();
	int iFindIndex(const FileName& fn);
	void MoveMap(int iNewIndex, const FileName& fn);
  MapList maplist() const;
protected:
	MapList mpl;
	//{{AFX_MSG(MapListDoc)
	afx_msg void OnOpenColorComp();
	afx_msg void OnOpenFilm();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAPLISTDOC_H__73850EC4_77E2_11D3_B7F1_00A0C9D5342F__INCLUDED_)
