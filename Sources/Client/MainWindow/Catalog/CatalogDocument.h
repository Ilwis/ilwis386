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
#ifndef CATALOGDOCUMENT_H
#define CATALOGDOCUMENT_H

#if !defined(AFX_ILWISDOCUMENT_H__30AB4836_C0DB_11D2_B70E_00A0C9D5342F__INCLUDED_)
#include "Client\Base\IlwisDocument.h"
#endif

#undef IMPEXP
#ifdef ILWISCLIENT
#define IMPEXP __declspec(dllexport)
#else
#define IMPEXP __declspec(dllimport)
#endif

class Directory;
class CatalogDocument;

class IMPEXP CatalogDocument: public IlwisDocument
{
//	friend UINT NotifyInThread(LPVOID lp);

protected:
	CatalogDocument();
	DECLARE_DYNCREATE(CatalogDocument)

public:
	virtual void SaveSettings(IlwisSettings& settings, int iNr);
//	void StartNotifyThread();
	//{{AFX_VIRTUAL(CatalogDocument)
	public:
  virtual IlwisObject obj() const;
	virtual String sGetPathName() const;
	//}}AFX_VIRTUAL
public:
	virtual void GetFileNames(vector<FileName>&);
	virtual void GetFileNames(vector<FileName>& vfn, vector<FileName>& vfnDisplay);
	virtual ~CatalogDocument();
	virtual String sName() const;

	virtual void AddObject(const FileName& fn);
	virtual void RemoveObject(const FileName& fn);
	virtual String sAllowedTypes();
  
	void SetFilter(const String& sF);
	void SetRegistryName(const String& sN);
	String sRegName();

protected:
	//{{AFX_MSG(IlwisDocument)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

  String sRegID; 
	bool fContinueThread;
	HANDLE threadHandle;
  void Init();
};

#endif
