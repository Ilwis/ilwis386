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
#include "Headers\toolspch.h"
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Client\Base\Res.h"
#include "Engine\Base\AssertD.h"
#include "Client\Base\IlwisDocument.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\Base\BaseView2.h"
#include "Client\Base\ButtonBar.h"
#include "Client\Editors\TextDoc.h"
#include "Client\Editors\TextView.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ScriptDoc

IMPLEMENT_DYNCREATE(TextDocument, IlwisDocument)

BEGIN_MESSAGE_MAP(TextDocument, IlwisDocument)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateSave)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ScriptDoc construction/destruction

TextDocument::TextDocument()
{
	// TODO: add one-time construction code here
	DelaySaveDoc();
}

TextDocument::~TextDocument()
{
}


// following functions can not be pure virtual because of requirements of Doc/View Model
string TextDocument::sGetText() 
{
	throw NonImplementableFunction("XXTextDocument::sGetText() should have an implementation");
}

void TextDocument::SetText(const string sTxt)
{
	throw NonImplementableFunction("XXTextDocument::SettText() should have an implementation");
}

void TextDocument::SetDescription(const String& s)
{
	obj()->SetDescription(s);
}

string TextDocument::sGetDescription()
{
	throw NonImplementableFunction("XXTextDocument::sGetDescription() should have an implementation");
}

#ifdef _DEBUG
void TextDocument::AssertValid()
{
	throw NonImplementableFunction("XXTextDocument::AssertValid() should have an implementation");
}
#endif

BOOL TextDocument::OnSaveDocument(LPCTSTR s)
{
  POSITION pos = GetFirstViewPosition();
 	TextView* pView = (TextView *) GetNextView( pos );
	obj()->SetDescription(pView->sGetDescription());
	SetText(pView->sText());
	obj()->Updated();
	SetModifiedFlag(false);
  return TRUE;
}

void TextDocument::OnUpdateSave(CCmdUI *cmd)
{
	bool fSave = !fReadOnly() && IsModified();
	cmd->Enable(fSave);
}

void TextDocument::OnFileOpen()
{
	if (IsModified())
		OnFileSave();

	CString sExt, sType;
	GetDocTemplate()->GetDocString(sType, CDocTemplate::filterName);
	GetDocTemplate()->GetDocString(sExt, CDocTemplate::filterExt);

	CFileDialog OpenFile(TRUE, NULL, NULL, OFN_SHAREAWARE | OFN_SHOWHELP, sType);
	CString filter = sType;
	filter += (TCHAR)'\0';
	filter += (TCHAR)'*';
	filter += sExt;
	filter += (TCHAR)'\0';
	OpenFile.m_ofn.lpstrFilter = filter;
	OpenFile.m_ofn.nMaxCustFilter++;

	if ( OpenFile.DoModal() == IDCANCEL )
		return;

	FileName fn(OpenFile.GetPathName());

	OnOpenDocument(fn.sFullName().c_str());
  UpdateAllViews(NULL);
	SetModifiedFlag(false);

}


