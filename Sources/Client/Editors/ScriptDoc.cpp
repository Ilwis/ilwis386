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
//#include "Headers\stdafx.h"
#include "Client\Headers\formelementspch.h"
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Headers\constant.h"
#include "Client\Base\Res.h"
#include "Engine\Base\AssertD.h"
#include "Engine\Scripting\Script.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\Base\ButtonBar.h"
#include "Client\MainWindow\AcceleratorManager.h"
#include "Client\Editors\TextDoc.h"
#include "Client\Editors\ScriptDoc.h"
#include "Client\Base\BaseView2.h"
#include "Client\Editors\TextView.h"
#include "Client\ilwis.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ScriptDoc

IMPLEMENT_DYNCREATE(ScriptDoc, TextDocument)

BEGIN_MESSAGE_MAP(ScriptDoc, TextDocument)
	//{{AFX_MSG_MAP(ScriptDoc)
	ON_COMMAND(ID_FILE_RUN_SCRIPT, OnRunScript)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ScriptDoc construction/destruction

ScriptDoc::ScriptDoc()
{
}

ScriptDoc::~ScriptDoc()
{
}

BOOL ScriptDoc::OnNewDocument()
{
	if (!TextDocument::OnNewDocument())
		return FALSE;

	FileName fnTemp = FileName::fnUnique("Noname.isl");
	script = Script(fnTemp, ""); // create an empty script
	script->fErase = true;

	return TRUE;
}

void ScriptDoc::OnRunScript()
{
	if (IsModified())
		if (!DoFileSave())
			return;       // don't continue

	String sCmd("run %S", script->fnObj.sFullNameQuoted());
	IlwWinApp()->Execute(sCmd);
}

BOOL ScriptDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	if (!TextDocument::OnOpenDocument(lpszPathName))
		return FALSE;
	FileName fn(lpszPathName);

	script = Script(fn);
	if (!script.fValid())
		return FALSE;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// ScriptDoc diagnostics

#ifdef _DEBUG
void ScriptDoc::AssertValid()
{
	TextDocument::AssertValid();
}

void ScriptDoc::Dump(CDumpContext& dc)
{
	TextDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// ScriptDoc commands
IlwisObject ScriptDoc::obj() const
{
  return script;
}

Script ScriptDoc::scr() const
{
	return script;
}

zIcon ScriptDoc::icon() const
{
  return zIcon("ScriptIcon");
}

string ScriptDoc::sGetText()
{
	if (script.fValid())
	{
		return script->sScript();
	}
	return "";
}

void ScriptDoc::SetText(const string sTxt)
{
	if (script.fValid())
		script->SetScript(sTxt);
}

string ScriptDoc::sGetDescription()
{
	if (obj().fValid())
		return obj()->sDescription;
	else
		return "";
}

// Not realy necessary, but added to be able to control the FileSaveDialog (future)
BOOL ScriptDoc::DoSave(LPCTSTR lpszPathName, BOOL bReplace)
{
	return TextDocument::DoSave(lpszPathName, bReplace);
}

void ScriptDoc::SaveDocumentAs(LPCTSTR s)
{
	String sFile(s);
	FileName fn(sFile);
	POSITION pos = GetFirstViewPosition();
	TextView* tv = NULL;
	FormBaseView* fbv = NULL;
	for (CView* pView = GetNextView(pos); pView; pView = GetNextView(pos)) 
	{
		if (!tv)
			tv = dynamic_cast<TextView*>(pView);
		if ( !fbv )
			fbv = dynamic_cast<FormBaseView*>(pView);
	}
	if (fbv) 
		fbv->exec();
	if (tv)
	{
		Script scr(fn, tv->sText());
		for (int i=0; i < script->iParams(); ++i)
			scr->SetParam(i, script->ptParam(i), script->sParam(i), script->fParamIncludeExtension(i), script->sDefaultValue(i));
		scr->SetDescription(tv->sGetDescription());
		scr->Store();

		OnOpenDocument(scr->fnObj.sFullName().c_str()); // reopen the script with the new name
	}
	SetModifiedFlag(false);
}

BOOL ScriptDoc::OnSaveDocument(LPCTSTR s)
{
	FileName fnNew(s);
	if (script->fErase || fnNew != obj()->fnObj)  // temporary object is set to be deleted
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
		FormBaseView* fbv = dynamic_cast<FormBaseView*>(pView);
		if (fbv) {
			fbv->exec();
		}
	}
	obj()->Updated();
	SetModifiedFlag(false);

	return TRUE;
}
