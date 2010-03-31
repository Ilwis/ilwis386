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
#if !defined(AFX_ILWISDOCTEMPLATE_H__4DDF7EAC_B5DE_11D2_B6FD_00A0C9D5342F__INCLUDED_)
#define AFX_ILWISDOCTEMPLATE_H__4DDF7EAC_B5DE_11D2_B6FD_00A0C9D5342F__INCLUDED_
#pragma warning(disable: 4786)
#include <set>

#if !defined(AFX_ILWISDOCUMENT_H__30AB4836_C0DB_11D2_B70E_00A0C9D5342F__INCLUDED_)
#include "Client\Base\IlwisDocument.h"
#endif

#include "Client\Base\BaseDocTemplate.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// IlwisDocTemplate.h : header file
//

#undef IMPEXP
#ifdef ILWISCLIENT
#define IMPEXP __declspec(dllexport)
#else
#define IMPEXP __declspec(dllimport)
#endif

/////////////////////////////////////////////////////////////////////////////
// IlwisDocTemplate document

class IMPEXP IlwisDocTemplate : public BaseDocTemplate
{
public:
	IlwisDocTemplate(const char* sExt, 
		const char* sType,
		const char* pcObjectName,
		CRuntimeClass* pDocClass, 
		CRuntimeClass* pFrameClass, 
		CRuntimeClass* pViewClass);
	DECLARE_DYNAMIC(IlwisDocTemplate)

	virtual BOOL GetDocString(CString& sRes, enum DocStringIndex index) const; // get one of the info strings
// Operations
public:
	virtual Confidence MatchDocType(LPCTSTR lpszPathName, CDocument*& rpDocMatch);
	virtual Confidence MatchDocType(const FileName& fnFO, CDocument*& rpDocMatch, const String& sMethod);
	virtual void InitialUpdateFrame(CFrameWnd* pFrame, CDocument* pDocument, BOOL bMakeVisible=TRUE);
  virtual bool fTypeOk(LPCTSTR lpszPathName);	

// Implementation
public:
	virtual ~IlwisDocTemplate();
protected:
  set<String> ssExt, ssType;
public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ILWISDOCTEMPLATE_H__4DDF7EAC_B5DE_11D2_B6FD_00A0C9D5342F__INCLUDED_)
