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
#ifndef OBJECTCOLLECTIONDOC_H
#define OBJECTCOLLECTIONDOC_H

/////////////////////////////////////////////////////////////////////////////
// ObjectCollectionDoc document

#ifndef CATALOGDOCUMENT_H
#include "Client\MainWindow\Catalog\CatalogDocument.h"
#endif

#ifndef ILWOBJECTCOLLECTION_H
#include "Engine\Base\DataObjects\ObjectCollection.h"
#endif

#undef IMPEXP
#ifdef ILWISCLIENT
#define IMPEXP __declspec(dllexport)
#else
#define IMPEXP __declspec(dllimport)
#endif

class IMPEXP ObjectCollectionDoc: public CatalogDocument
{
public:
	virtual ~ObjectCollectionDoc();
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
  virtual IlwisObject obj() const;
	virtual void GetFileNames(vector<FileName>& vfn, vector<FileName>& vfnDisplay);
	virtual zIcon icon() const;
	String sName() const;
	virtual void SaveSettings(IlwisSettings& settings,int iNr);
	virtual String sGetPathName() const;
  virtual bool fEditable() const;

	virtual void AddObject(const FileName& fn);
	virtual void RemoveObject(const FileName& fn);
	bool fAlreadyInCollection(const FileName& fn) const;
	virtual String sAllowedTypes();
	void OnOpenFilm();

protected:
	ObjectCollectionDoc();           // protected constructor used by dynamic creation

  ObjectCollection oc;
	virtual BOOL SaveModified();

	DECLARE_MESSAGE_MAP()
	DECLARE_DYNCREATE(ObjectCollectionDoc)
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif 
