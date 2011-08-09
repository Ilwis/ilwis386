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
// IlwisDocTemplate.cpp : implementation file
//

// handling of parameters -showas and -edit
// still have to be implemented
// needed is a function similar to MatchDocType

#pragma warning( disable : 4786 )

#include "Client\Headers\formelementspch.h"
#include "Client\ilwis.h"
#include "Client\Base\BaseDocTemplate.h"
#include "Client\Base\IlwisDocTemplate.h"
#include "Engine\DataExchange\ForeignFormat.h"
#include "Client\Base\IlwisDocument.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\Base\Framewin.h"
#include "Headers\Hs\Mainwind.hs"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// defined in afximpl.h:
BOOL AFXAPI AfxComparePath(LPCTSTR lpszPath1, LPCTSTR lpszPath2);


/////////////////////////////////////////////////////////////////////////////
// IlwisDocTemplate

IMPLEMENT_DYNAMIC(IlwisDocTemplate, BaseDocTemplate)

IlwisDocTemplate::IlwisDocTemplate(
    const char* pcExt, 
    const char* pcType, 
	  const char* pcObjectName,
    CRuntimeClass* pDocClass,
		CRuntimeClass* pFrameClass, 
    CRuntimeClass* pViewClass)
:	BaseDocTemplate(pDocClass, pFrameClass, pViewClass)
{
  char *s0, *s;
  String str;
  // extensions
  s0 = 0;
  str = pcExt;
  for (s = str.sVal(); *s; ++s) 
    if ('.' == *s) {
      if (s0) {
        *s = 0;
        ssExt.insert(s0);
        *s = '.';
      }
      s0 = s;
    }
  if (s0)
    ssExt.insert(s0);
  // types
  str = pcType;
  s0 = str.sVal();
  for (s = str.sVal(); *s; ++s) 
    if (',' == *s) {
      if (s0) {
        *s = 0;
        ssType.insert(s0);
        ++s;
      }
      if (*s)
        s0 = s;
    }
  if (s0 && *s0)
    ssType.insert(s0);

  SetObjectType(pcObjectName);
}

IlwisDocTemplate::~IlwisDocTemplate()
{
}

/////////////////////////////////////////////////////////////////////////////
// IlwisDocTemplate diagnostics

#ifdef _DEBUG
void IlwisDocTemplate::AssertValid() const
{
	CMultiDocTemplate::AssertValid();
}

void IlwisDocTemplate::Dump(CDumpContext& dc) const
{
	CMultiDocTemplate::Dump(dc);
}
#endif //_DEBUG


void IlwisDocTemplate::InitialUpdateFrame(CFrameWnd* pFrame, 
          CDocument* pDocument, BOOL bMakeVisible)
{
	IlwisDocument* pid = dynamic_cast<IlwisDocument*>(pDocument);
	if (pid)
		pFrame->SetIcon(pid->icon(), TRUE);
	FrameWindow* pfrm = dynamic_cast<FrameWindow*>(pFrame);
	ASSERT_VALID(pfrm);
	if (pfrm)
		pfrm->InitialUpdate(pDocument, bMakeVisible);
}


CDocTemplate::Confidence IlwisDocTemplate::MatchDocType(LPCTSTR lpszPathName,
	CDocument*& rpDocMatch)
{
	ASSERT(lpszPathName != NULL);
	rpDocMatch = NULL;

	// go through all documents
	POSITION pos = GetFirstDocPosition();
	while (pos != NULL)
	{
		CDocument* pDoc = GetNextDoc(pos);

		HWND hWnd = NULL; // Try to get something from the pDoc to confirm that it is valid
		if (pDoc != NULL)
		{
			POSITION posView = pDoc->GetFirstViewPosition();
			if (posView != NULL)
			{
				CView* pView = pDoc->GetNextView(posView);
				hWnd = pView->GetSafeHwnd(); // If hWnd != NULL, pDoc was valid ( <==> not closing)
			}
		}

		if (AfxComparePath(pDoc->GetPathName(), FileName(lpszPathName).sFullPath().sVal()) && (hWnd != NULL))
		{
			FileName fn(lpszPathName);
			if (".mpr" == fn.sExt || ".mpp" == fn.sExt 
				|| ".mps" == fn.sExt || ".mpa" == fn.sExt
				|| ".mpv" == fn.sExt)
			{
				switch (MessageBox(hWnd, TR("Map is already shown in a MapWindow.\nDo you wish to open the map in a new map window again?").c_str(), "ILWIS", MB_YESNO|MB_ICONEXCLAMATION|MB_DEFBUTTON2|MB_TOPMOST))
				{
					case IDNO:
						rpDocMatch = pDoc;
						return yesAlreadyOpen;
					case IDYES:
						pos = 0;
						break;
				}
			}
			else {
				rpDocMatch = pDoc;
				return yesAlreadyOpen;
			}
		}
	}

  bool fExtOk = false;	 
	FileName fn(lpszPathName);
	if (fn.sExt == "")
	  return noAttempt;
  LPCTSTR lpszDot = fn.sExt.c_str();
  if (ssExt.find(lpszDot) != ssExt.end()) {
	bool ft = fTypeOk(lpszPathName);
	if ( ft && ( fn.sExt == ".mpl" || fn.sExt ==".ioc"))
		return yesAttemptNative;
    return ft ? maybeAttemptNative : noAttempt;
	//return ft ? yesAttemptNative : noAttempt;
  }
  return noAttempt;
}

CDocTemplate::Confidence IlwisDocTemplate::MatchDocType(const FileName& fnFO, CDocument*& rpDocMatch, const String& sMethod)
{
	if ( IlwisObject::iotObjectType(fnFO) != IlwisObject::iotANY )
		return MatchDocType(fnFO.sRelative().c_str(), rpDocMatch);

	ForeignFormat *ff = ForeignFormat::Create(sMethod);		
	if ( ff )
	{		
		String sObjectType = sGetObjectType();
		bool fCanUse = ff->fMatchType(fnFO.sFullPath(), sObjectType);
		delete ff;
		if ( fCanUse )
		{
			return yesAttemptForeign;			
		}			
	}
	return noAttempt;
}

bool IlwisDocTemplate::fTypeOk(LPCTSTR lpszPathName)
{
	if (ssType.size() == 0)
		return true;
  FileName fn(lpszPathName);
	if ( fn.fExist() )
	{
	  String sSection, sType, sParentType;
		for (sSection = "Ilwis";; sSection = sType)
		{
		 sType = "";
			if (!ObjectInfo::ReadElement(sSection.sVal(), "Type", fn, sType))
				return false;
			sParentType = sType;
			if (ssType.find(sType) != ssType.end())
				return true;
		}			
  }
	else // could be that this is a not yet existing file, based on only a display name, llok at the extension
	{
		FileName fnFO(lpszPathName);
		if ( ssExt.find(fnFO.sExt) != ssExt.end())
			return true;
	}
	return false;
}



BOOL IlwisDocTemplate::GetDocString(CString& sRes, enum DocStringIndex index) const // get one of the info strings
{
	if (BaseDocTemplate::GetDocString(sRes, index))
		return TRUE;
  switch (index) {
    case filterExt:          // user visible extension for FileOpen
		{
			sRes = "";
			for (set<String>::const_iterator iter = ssExt.begin(); iter != ssExt.end(); iter++)
				sRes += (*iter).c_str();
			return TRUE;
		}
    default:
      return FALSE;
		break;
  }
}

