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
#if !defined(AFX_WINTHREAD_H__7A750BC3_F7BE_11D2_B757_00A0C9D5342F__INCLUDED_)
#define AFX_WINTHREAD_H__7A750BC3_F7BE_11D2_B757_00A0C9D5342F__INCLUDED_

#if !defined(AFX_ILWISDOCUMENT_H__30AB4836_C0DB_11D2_B70E_00A0C9D5342F__INCLUDED_)
#include "Client\Base\IlwisDocument.h"
#endif

#if !defined(AFX_ILWISDOCTEMPLATE_H__4DDF7EAC_B5DE_11D2_B6FD_00A0C9D5342F__INCLUDED_)
#include "Client\Base\IlwisDocTemplate.h"
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WinThread.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// WinThread thread

class _export WinThread : public CWinThread
{
public:
	WinThread(CDocManager*, LPCTSTR lpszFileName, IlwisDocument::OpenType ot=IlwisDocument::otNORMAL);           
	WinThread(CDocManager *pdm, const String& fn, const ParmList& pl);
	WinThread(IlwisDocTemplate*, LPCTSTR lpszFileName, IlwisDocument::OpenType ot=IlwisDocument::otNORMAL);           
	WinThread(CRuntimeClass*, const String& sCmd);           
	virtual ~WinThread();

	virtual BOOL InitInstance();
	virtual int ExitInstance();

  String sFileName() const;
protected:
	IlwisDocTemplate* m_pDocTemplate;
	CDocManager* m_pDocManager;
  CRuntimeClass* m_rtcType;
	IlwisDocument::OpenType ot;
	ParmList parms;
	String strFileName;
	String sDfltDir;
private:
	static CDocument* OpenDocumentFile(CDocManager* pdm, LPCTSTR lpszFileName, IlwisDocument::OpenType ot);
	static CDocument* OpenDocumentFile(CDocManager* pdm, LPCTSTR lpszFileName, ParmList& pm);
protected:
	// Generated message map functions
	//{{AFX_MSG(WinThread)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WINTHREAD_H__7A750BC3_F7BE_11D2_B757_00A0C9D5342F__INCLUDED_)
