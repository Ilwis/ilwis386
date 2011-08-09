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
//Created Martin Schouwenburg 4-4-99
#include "Headers\toolspch.h"
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Client\Base\Res.h"
#include "Engine\Base\AssertD.h"
#include "Engine\Function\FUNCUSER.H"
#include "Client\Base\IlwisDocument.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\Base\BaseView2.h"
#include "Client\Base\ButtonBar.h"
#include "Client\Editors\TextDoc.h"
#include "Client\Editors\TextView.h"
#include "Client\Editors\FuncUserDoc.h"
#include "Headers\Hs\Script.hs"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const bool FULL_FUNCTION = false;
const bool EXPRESSION_ONLY = true;

/////////////////////////////////////////////////////////////////////////////
// FuncUserDoc

IMPLEMENT_DYNCREATE(FuncUserDoc, TextDocument)

BEGIN_MESSAGE_MAP(FuncUserDoc, TextDocument)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// FuncUserDoc construction/destruction

FuncUserDoc::FuncUserDoc()
{
}

FuncUserDoc::~FuncUserDoc()
{
}

BOOL FuncUserDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	((CEditView*)m_viewList.GetHead())->SetWindowText(NULL);

	FileName fnTemp = FileName::fnUnique("Noname.fun");
	function = FuncUser(fnTemp, ""); // create an empty function
	function->fErase = true;

	return TRUE;
}

BOOL FuncUserDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
  ISTRUE(fINotEqual, lpszPathName, (LPCTSTR)0);
  ISTRUE(fINotEqual, lpszPathName[0], (char)0);

	if (!TextDocument::OnOpenDocument(lpszPathName))
		return FALSE;

  FileName fn(lpszPathName);
  function = FuncUser(fn);
  if (!function.fValid())
    return FALSE;

  return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// FuncUserDoc diagnostics

#ifdef _DEBUG
void FuncUserDoc::AssertValid() 
{
	TextDocument::AssertValid();
	//ISTRUE(fINotEqual, function.fValid(), false);

}

void FuncUserDoc::Dump(CDumpContext& dc)
{
	TextDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// FuncUserDoc commands
IlwisObject FuncUserDoc::obj() const
{
  return function;
}

zIcon FuncUserDoc::icon() const
{
  return zIcon("FunctionIcon");
}

string FuncUserDoc::sGetText()
{
	return function->sFuncDef();
}

void FuncUserDoc::SetText(const string sTxt)
{
	function->SetFuncDef(sTxt);
}

string FuncUserDoc::sGetDescription()
{
	return function->sDescription;
}

void FuncUserDoc::SetDescription(const String& s)
{
	function->sDescription = s;
}

void FuncUserDoc::SaveDocumentAs(LPCTSTR s)
{
	String sFile(s);
	FileName fn(sFile);
  POSITION pos = GetFirstViewPosition();
	for (CView* pView = GetNextView(pos); pView; pView = GetNextView(pos)) 
	{
		TextView* tv = dynamic_cast<TextView*>(pView);
		if (tv) 
		{
			fn.sExt = ".fun";
			FuncUser fnc(fn, tv->sText(), FULL_FUNCTION); // do not add function template
			fnc->SetDescription(tv->sGetDescription());
			if (fnc->fFuncDefOk(tv->sText()))
				fnc->Store();
			else
				fnc->fErase = true;

			OnOpenDocument(fnc->fnObj.sFullName().c_str());
		}
	}
	SetModifiedFlag(false);
}

BOOL FuncUserDoc::OnSaveDocument(LPCTSTR s)
{
	FileName fnNew(s);
	if (function->fErase || fnNew != obj()->fnObj)  // temporary object is set to be deleted
	{
		SaveDocumentAs(s);
		return TRUE;
	}

	POSITION pos = GetFirstViewPosition();
	for (CView* pView = GetNextView(pos); pView; pView = GetNextView(pos)) 
	{
		TextView* tv = dynamic_cast<TextView*>(pView);
		if (tv) {
			obj()->SetDescription(tv->sGetDescription());
			SetText(tv->sText());
		}
	}
	obj()->Updated();
	SetModifiedFlag(false);

	return TRUE;
}
