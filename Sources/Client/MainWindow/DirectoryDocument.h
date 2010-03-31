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
#ifndef DIRECTORYDOCUMENT_H
#define DIRECTORYDOCUMENT_H

#ifndef CATALOGDOCUMENT_H
#include "Client\MainWindow\Catalog\CatalogDocument.h"
#endif


#undef IMPEXP
#ifdef ILWISCLIENT
#define IMPEXP __declspec(dllexport)
#else
#define IMPEXP __declspec(dllimport)
#endif

class IMPEXP DirectoryDocument: public CatalogDocument
{
protected:
	DirectoryDocument();
	DECLARE_DYNCREATE(DirectoryDocument)
public:
	DirectoryDocument(LPCTSTR lpszPathName);
	//{{AFX_VIRTUAL(DirectoryDocument)
	public:
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	//}}AFX_VIRTUAL
	virtual void SetPathName(LPCTSTR lpszPathName, BOOL bAddToMRU=FALSE);
public:
	virtual void GetFileNames(vector<FileName>&);
	virtual ~DirectoryDocument();
	virtual String sName() const;
	zIcon icon() const;
protected:
	void GetFileNames(vector<FileName>& vfn, const String& sMask);
	//{{AFX_MSG(DirectoryDocument)
	afx_msg void OnDirUp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};












#endif
