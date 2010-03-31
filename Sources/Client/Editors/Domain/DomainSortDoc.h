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
// DomainSortDoc.h: interface for the DomainSortDoc class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DOMAINSORTDOC_H__92A60E86_DDF2_11D2_9C9B_00A0C9D5320B__INCLUDED_)
#define AFX_DOMAINSORTDOC_H__92A60E86_DDF2_11D2_9C9B_00A0C9D5320B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#undef IMPEXP
#ifdef ILWISCLIENT
#define IMPEXP __declspec(dllexport)
#else
#define IMPEXP __declspec(dllimport)
#endif

#define errSelfMergeError 2411
#define FORMAT_CLASSID "%S\t%S\t%S\r\n"
#define FORMAT_GROUP   "%S\t%S\t%S\t%g\r\n"

//! SelfMergeError is generated when the user tries to merge with itself
class SelfMergeError : ErrorObject {
public:
	SelfMergeError(const String& sMsg) : ErrorObject(sMsg, errSelfMergeError) {}
};

//! DomainSortFields enumerates the possible DomainSort fields
enum DomainSortFields { dsfCLASSID, dsfCODE, dsfDESCRIPTION, dsfUPPER };

/////////////////////////////////////////////////////////////////////////////
//! DomainSortDoc document class
class IMPEXP DomainSortDoc : public DomainDoc  
{
public:
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);

	void MoveItem(long iKeyTo, long iKeyFrom);
	virtual String sMakeClipLine(const ClassItem& cli);
	virtual void MakeItemPart(DomainSortFields dsfField, ClassItem& cli, const string& sVal);
	virtual bool fCanAdd(const ClassItem& cli);
	void Sort();
	void SetSortManual();
	void SetSortAlphabetic();
	void SetSortAlphaNum();
	void SetSortCodeAlpha();
	void SetSortCodeAlphaNum();
	long iGetItemIndexOf(const ClassItem& cli);
	bool fManualSorted();	// manual ordering
	bool fAlphaSorted();	// sorted on class/ID name alphabetically
	bool fAlphaNumSorted();	// sorted on class/ID name alphanumeric
	bool fCodeAlphabetic();	// sorted on code alphabetically
	bool fCodeAlphaNumeric();	//  sorted on code alphanumeric
	void AddFromDomain(const String& sDomain);
	virtual void GetItem(long iKey, ClassItem& cli);
	virtual string sGetItemPart(long iKey, DomainSortFields dsfField);
	virtual void DeleteItem(const ClassItem& cli);
	virtual long iMergeAddItem(const ClassItem& cli);
	virtual long iAddItem(const ClassItem& cli);
	virtual void SetItem(long iKey, const ClassItem& cli);
	virtual void SetItemPart(long iKey, DomainSortFields dsfField, const string& sVal);

	bool fIsID();
	bool fIsClass();
	bool fIsGroup();
	long iCount();

	long iFindName(const String& sName);
	long iFindCode(const String& sCode);

	void SetDescription(long iKey, const String& sDescr);
	void SetCode(long iKey, const String& sCode);
	void SetName(long iKey, const String &sName);
	void SetPrefix(const String& sNewPrefix);

	String sName(long iKey);
	String sCode(long iKey);
	String sDescription(long iKey);
	String sPrefix();

	void StoreWidth(const string& sSection, int iWid);
	int RestoreWidth(const string& sSection);

	virtual ~DomainSortDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	DomainSortDoc();           // protected constructor used by dynamic creation
	virtual BOOL OnNewDocument();
	void OnRepresentation();
	void OnUpdateRepresentation(CCmdUI* pCmdUI);

	DECLARE_DYNCREATE(DomainSortDoc)
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//! DomainGroupDoc document class
class IMPEXP DomainGroupDoc : public DomainSortDoc  
{
public:
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);

	virtual String sMakeClipLine(const ClassItem& cli);
	virtual void MakeItemPart(DomainSortFields dsfField, ClassItem& cli, const string& sVal);
	virtual bool fCanAdd(const ClassItem& cli);
	long iGetItemIndexOf(const ClassItem& cli);
	virtual void GetItem(long iKey, ClassItem& cli);
	virtual string sGetItemPart(long iKey, DomainSortFields dsfField);
	double rBound(long iKey);
	virtual long iMergeAddItem(const ClassItem& cli);
	virtual long iAddItem(const ClassItem& cli);
	virtual void SetItem(long iKey, const ClassItem& cli);
	virtual void SetItemPart(long iKey, DomainSortFields dsfField, const string& sVal);
	void SetBound(long iKey, double rBound);
	virtual ~DomainGroupDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	DomainGroupDoc();           // protected constructor used by dynamic creation

	virtual BOOL OnNewDocument();

	DECLARE_DYNCREATE(DomainGroupDoc)
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DOMAINSORTDOC_H__92A60E86_DDF2_11D2_9C9B_00A0C9D5320B__INCLUDED_)
