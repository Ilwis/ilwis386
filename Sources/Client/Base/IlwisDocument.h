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
#if !defined(AFX_ILWISDOCUMENT_H__30AB4836_C0DB_11D2_B70E_00A0C9D5342F__INCLUDED_)
#define AFX_ILWISDOCUMENT_H__30AB4836_C0DB_11D2_B70E_00A0C9D5342F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// IlwisDocument.h : header file
//
#undef IMPEXP
#ifdef ILWISCLIENT
#define IMPEXP __declspec(dllexport)
#else
#define IMPEXP __declspec(dllimport)
#endif

class GeneralBar;
class IlwisObject;
class zIcon;

/////////////////////////////////////////////////////////////////////////////
// IlwisDocument document

class IMPEXP IlwisDocument : public COleServerDoc
{
protected:
	IlwisDocument();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(IlwisDocument)
// Attributes
public:
	enum OpenType { otNORMAL=0, otEDIT=1, otNOASK=2, 
	                otCOLORCOMP, otSLIDESHOW,
	                otMPR, otMPA, otMPS, otMPP, otANIMATION,  otCOLLECTION };
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName, OpenType ot);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName, ParmList& pm);	
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual void OnIdle();
	virtual BOOL SaveModified();
	virtual BOOL OnNewDocument();
	virtual void OnOpenExpressionDoc(IlwisObject& ob);
  virtual bool fEditable() const;
	virtual CFile* GetFile(LPCTSTR lpszFileName, UINT nOpenFlags,	CFileException* pError);
	void ShowAddInfo(bool fDock);
protected:
	virtual COleServerItem* OnGetEmbeddedItem(); // should be overruled!

// Implementation
public:
	bool				fReadOnly() const;
	virtual	IlwisObject	obj() const;
	virtual bool usesObject(const IlwisObject& ob) const;
	void				SetObjectDescription(const String& s);
	string				sObjectDescription();
	virtual zIcon		icon() const;
	virtual				~IlwisDocument();
	CWnd* wndGetActiveView() const;
	void           DelaySaveDoc(bool fDelay=true);
	bool					fDelaySaveDoc();
	virtual bool  fIsEmpty() const; // used to determine if an existing document has a meaningfull contents
	                                // e.g. a layoutdoc without any items
#ifdef _DEBUG
	virtual void		AssertValid() const;
	virtual void		Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	DWORD m_dwCreationThreadID;		
	//! default false, set on true in OnOpenDocument() before calling the base class if serialization should be used
	bool fUseSerialize; 
	bool fDelaySave; // some editors save their content with every change (eg.g rpr) others need to have explicit save(e.g.script)

	//{{AFX_MSG(IlwisDocument)
		// NOTE - the ClassWizard will add and remove member functions here.
	afx_msg void OnProperties();
	afx_msg void OnShowAddInfo();
	afx_msg void OnUpdateShowAddInfo(CCmdUI* pCmdUI);
	afx_msg void OnUpdateProperties(CCmdUI* pCmdUI);
	afx_msg void OnOperationCmd(UINT nID);
	afx_msg void OnSaveCopyAs();
	afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);
	//}}AFX_MSG

private:
	IlwisObject *ilwisobj;
	GeneralBar* gbAddInfo;

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ILWISDOCUMENT_H__30AB4836_C0DB_11D2_B70E_00A0C9D5342F__INCLUDED_)
