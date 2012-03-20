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
// CatalogDocument.cpp : implementation file
//

#include "Client\Headers\formelementspch.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Engine\Base\File\Directory.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\Editors\Utils\OwnerHeaderCtrl.h"
#include "Client\MainWindow\Catalog\Catalog.h"
#include "Client\MainWindow\Catalog\CatalogDocument.h"
#include "Client\ilwis.h"
#include "Headers\constant.h"
#include "Headers\Hs\Mainwind.hs"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CatalogDocument

IMPLEMENT_DYNCREATE(CatalogDocument, IlwisDocument)

/*
UINT NotifyInThread(LPVOID lp)
{
	DWORD dwWaitStatus; 
 
	CatalogDocument *doc = (CatalogDocument *) lp;
	CString sPath = doc->GetPathName();

	HANDLE dwChangeHandles[1];
	dwChangeHandles[0] = FindFirstChangeNotification( 
			sPath,                 // directory to watch 
			FALSE,                         // do not watch the subtree 
			FILE_NOTIFY_CHANGE_FILE_NAME); // watch file name changes 
 
	if (dwChangeHandles[0] != INVALID_HANDLE_VALUE) 
	{
		while (doc->fContinueThread) 
		{ 
			dwWaitStatus = WaitForMultipleObjects(1, dwChangeHandles, 
						FALSE, INFINITE);

			Directory *fn = new Directory((String)sPath); // will be deleted when handling the message
			IlwWinApp()->PostThreadMessage(ILW_READCATALOG, 0, (LPARAM)fn);
			if (!FindNextChangeNotification(dwChangeHandles[0])) 
				doc->fContinueThread = false;
		}
		FindCloseChangeNotification(dwChangeHandles[0]);
	}

	return 1;
} 
*/

CatalogDocument::CatalogDocument() :
	fContinueThread(false)
{
	Init();
}

CatalogDocument::~CatalogDocument() 
{
	// activation sequence will automatically select a new document
	// we must ensure here that the last document closing will leave the adminstration correct;
  fContinueThread = false;
 // ::WaitForSingleObject(threadHandle, 5000);
//	threadHandle = 0;
	if ( IlwWinApp()->dirDoc() == this)
		IlwWinApp()->SetCatalogDocument(NULL); 
}


BEGIN_MESSAGE_MAP(CatalogDocument, IlwisDocument)
	//{{AFX_MSG_MAP(CatalogDocument)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

IlwisObject CatalogDocument::obj() const
{
	return IlwisObject::objInvalid();
}

void CatalogDocument::GetFileNames(vector<FileName>& vfn)
{
	vfn.clear();
}

void CatalogDocument::GetFileNames(vector<FileName>& vfn, vector<FileName>& vfnDisplay)
{
	GetFileNames(vfn);	 
	vfnDisplay = vfn;
}

void CatalogDocument::SaveSettings(IlwisSettings& settings,int iNr)
{
	String sDocKey("%S\\Doc%d", settings.sIlwisSubKey(), iNr);
	POSITION pos = GetFirstViewPosition();
	int iNrView=0;
	while(pos)
	{
		IlwisSettings docsettings(sDocKey);
		docsettings.SetValue("DocName", sName());
		Catalog* cat = (Catalog *)GetNextView(pos) ;
		if ( cat)
			cat->SaveCatalogView(sDocKey, iNrView++);
	}
}

String CatalogDocument::sName() const
{
	return String("docdefault");
}

void CatalogDocument::SetFilter(const String& sF)
{
  	String sCatalogQuery = sF.sUnQuote().sTrimSpaces().toLower();
		Catalog* cat = dynamic_cast<Catalog *> (wndGetActiveView());
		if ( cat)
		{
			cat->SetCatalogQuery(sCatalogQuery);
			UpdateAllViews(0,1,0);
		}
}

String CatalogDocument::sRegName()
{
	return sRegID;
}

void CatalogDocument::SetRegistryName(const String& sN)
{
	sRegID = sN;
}

void CatalogDocument::Init()
{
}

/*
void CatalogDocument::StartNotifyThread()
{
	fContinueThread = true;
	CWinThread *thread = AfxBeginThread(NotifyInThread, this);
	threadHandle = thread->m_hThread;
}
*/

String CatalogDocument::sGetPathName() const
{
	String sName(GetPathName());
	if ( sName[sName.size() - 1] != '\\')
			 sName &= "\\";
	return FileName(sName).sPath();
}

String CatalogDocument::sAllowedTypes()
{
	// anything goes;
	return IlwisObject::sAllExtensions();
}

void CatalogDocument::AddObject(const FileName& fn)
{
  // empty default implementation
  ASSERT(0==1);
}

void CatalogDocument::RemoveObject(const FileName& fn)
{
  // empty default implementation
  ASSERT(0==1);
}

BOOL CatalogDocument::OnOpenDocument(LPCTSTR lpszPathName) {
	return IlwisDocument::OnOpenDocument(lpszPathName);
}