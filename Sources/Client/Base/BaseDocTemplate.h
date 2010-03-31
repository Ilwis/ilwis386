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
#ifndef BASEDOCTEMPLATE_H
#define BASEDOCTEMPLATE_H

/*#undef IMPEXP
#ifdef ILWISCLIENT
#define IMPEXP __declspec(dllexport)
#else
#define IMPEXP __declspec(dllimport)
#endif*/

class BaseDocTemplate : public CMultiDocTemplate
{
public:
	BaseDocTemplate( 
		CRuntimeClass* pDocClass, 
		CRuntimeClass* pFrameClass, 
		CRuntimeClass* pViewClass);
	DECLARE_DYNAMIC(BaseDocTemplate)

	virtual BOOL GetDocString(CString& sRes, enum DocStringIndex index) const; // get one of the info strings
	virtual CFrameWnd* CreateNewFrame(CDocument* pDoc, CFrameWnd* pOther);
	virtual CDocument* OpenDocumentFile(LPCTSTR lpszPathName, BOOL bMakeVisible = TRUE);
	CDocument* OpenDocumentFile(CDocument* pDoc, BOOL bMakeVisible = TRUE);
	IlwisDocument* OpenDocumentFile(LPCTSTR lpszPathName, IlwisDocument::OpenType);
	virtual IlwisDocument* OpenDocumentFile(LPCTSTR lpszPathName, ParmList& pm);
	virtual Confidence MatchDocType(LPCTSTR lpszPathName, CDocument*& rpDocMatch);
	virtual Confidence MatchDocType(const FileName& fn, CDocument*& rpDocMatch, const String& sMethod);	
	virtual void InitialUpdateFrame(CFrameWnd* pFrame, CDocument* pDocument, BOOL bMakeVisible=TRUE);
	void SetObjectType(const String& s);
	String sGetObjectType() const;

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	virtual ~BaseDocTemplate();
private:
  virtual bool fTypeOk(LPCTSTR lpszPathName);
  String sObjectType;
};

#endif
