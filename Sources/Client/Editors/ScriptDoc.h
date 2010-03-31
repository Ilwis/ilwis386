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
#ifndef SCRIPTDOC_H
#define SCRIPTDOC_H

#undef IMPEXP
#ifdef ILWISCLIENT
#define IMPEXP __declspec(dllexport)
#else
#define IMPEXP __declspec(dllimport)
#endif


class IMPEXP ScriptDoc : public TextDocument
{
public:
	virtual	~ScriptDoc();

	virtual BOOL OnNewDocument();
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual BOOL DoSave(LPCTSTR lpszPathName, BOOL bReplace);

	virtual IlwisObject obj() const;
	Script scr() const;
	virtual zIcon icon() const;
	//virtual void Serialize(CArchive& ar);

	virtual string	sGetText();
	virtual void    SetText(const string sTxt);
	virtual string	sGetDescription();
//	void OnUpdateClip(CCmdUI *cmd);

#ifdef _DEBUG
	virtual void AssertValid() ;
	virtual void Dump(CDumpContext& dc) ;
#endif

protected: // create from serialization only
	ScriptDoc();

	void SaveDocumentAs(LPCTSTR s);
	void OnRunScript();

	Script  script;

	DECLARE_DYNCREATE(ScriptDoc)
	DECLARE_MESSAGE_MAP()
};

#endif // SCRIPTDOC_H
