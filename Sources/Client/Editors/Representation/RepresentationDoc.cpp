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
#include "Headers\constant.h"
#include "Client\ilwis.h"
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Client\Base\Res.h"
#include "Engine\Base\AssertD.h"
#include "Engine\Scripting\Script.h"
#include "Client\Base\IlwisDocument.h"
#include "Engine\Representation\Rprgrad.h"
#include "Client\Editors\Representation\RepresentationDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
   
//-- [RepresentationDoc ]--------------------------------------------------------------------------------------

IMPLEMENT_DYNCREATE(RepresentationDoc, IlwisDocument)

BEGIN_MESSAGE_MAP(RepresentationDoc, IlwisDocument)
	ON_COMMAND(ID_RPRDOM, OnOpenDomain)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// RepresentationDoc construction/destruction

RepresentationDoc::RepresentationDoc()
{
	// TODO: add one-time construction code here

}

RepresentationDoc::~RepresentationDoc()
{
}

BOOL RepresentationDoc::OnNewDocument()
{
	// 20/4/01 Wim: cannot be done so return false
	return FALSE;

	if (!IlwisDocument::OnNewDocument())
		return FALSE;

	((CEditView*)m_viewList.GetHead())->SetWindowText(NULL);

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}

BOOL RepresentationDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	if (!IlwisDocument::OnOpenDocument(lpszPathName))
		return FALSE;

  FileName fn(lpszPathName);
  
  _rpr = Representation(fn);
  if (!_rpr.fValid())
    return FALSE;


  return TRUE;
}

BOOL RepresentationDoc::OnOpenDocument(LPCTSTR lpszPathName, OpenType ot)
{
	BOOL fRes = OnOpenDocument(lpszPathName);
	if (!fRes)
		return FALSE;
	switch (ot)
	{
		case otMPR:
			::AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_RPRRAS, 0);
			break;
		case otMPA:
			::AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_RPRPOL, 0);
			break;
		case otMPS:
			::AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_RPRSEG, 0);
			break;
		case otMPP:
			::AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_RPRPNT, 0);
			break;
	}
	return TRUE;
}


void RepresentationDoc::OnOpenDomain()
{
	String sCommand("open %S", dm()->fnObj.sFullNameQuoted());
	IlwWinApp()->Execute(sCommand);
}

/////////////////////////////////////////////////////////////////////////////
// RepresentationDoc commands
IlwisObject RepresentationDoc::obj() const
{
  return _rpr;
}

zIcon RepresentationDoc::icon() const
{
  return zIcon("RprIcon");
}

RepresentationGradual *RepresentationDoc::prg()
{
	return rpr()->prg();
}

RepresentationClass *RepresentationDoc::prc()
{
	return rpr()->prc();
}

Domain RepresentationDoc::dm()
{
	return rpr()->dm();
}

Representation RepresentationDoc::rpr()
{
	return _rpr;
}
