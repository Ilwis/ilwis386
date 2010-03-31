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
// GraphDoc.cpp: implementation of the GraphDoc class.
//
//////////////////////////////////////////////////////////////////////

#pragma warning( disable : 4786 )
#pragma warning( disable : 4503 )

#include "Client\Headers\formelementspch.h"
#include "Client\TableWindow\GraphDoc.h"
#include "Client\GraphWindow\GraphAxis.h"
#include "Client\GraphWindow\GraphDrawer.h"
#include "Engine\Table\GraphObject.h"
#include "Engine\Base\File\ElementMap.h"
#include "Client\ilwis.h"
#include "Headers\constant.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(GraphDoc, IlwisDocument)

BEGIN_MESSAGE_MAP(GraphDoc, IlwisDocument)
  ON_COMMAND(ID_GRPH_NEW, OnFileNew)
  ON_COMMAND(ID_GRPH_OPENTABLE, OnOpenTable)
END_MESSAGE_MAP()

GraphDoc::GraphDoc()
: grdr(0)
{
	fUseSerialize = true;
}

GraphDoc::~GraphDoc()
{
  delete grdr;
}

zIcon GraphDoc::icon() const
{
  return zIcon("GraphIcon");
}

void GraphDoc::DeleteContents()
{
  delete grdr;
  grdr = 0;
	UpdateAllViews(0);
}

int GraphDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	try {
		DeleteContents();

		FileName fn(lpszPathName);
		if (".grh" == fn.sExt || lpszPathName == 0) 
    {
  		if (0 != lpszPathName)
	  		SetPathName(fn.sFullName().scVal());
      return IlwisDocument::OnOpenDocument(lpszPathName);
    }      
    else
    {
      tbl = Table(fn);
      if (!tbl.fValid())
        return FALSE;
      return OnNewDocument();
    }
    return FALSE;
  }
	catch (ErrorObject& err) {
		err.Show();
	}
	return FALSE;
}

void GraphDoc::OnOpenTable()
{
  IlwWinApp()->OpenDocumentAsTable(tbl->sName(true).scVal());
}

int GraphDoc::iLayers() const 
{ 
  if (0 == grdr)
    return 0;
  return grdr->agl.iSize(); 
}

GraphLayer* GraphDoc::gl(int iNr) const 
{ 
  if (0 == grdr)
    return 0;
  return grdr->agl[iNr]; 
}

void GraphDoc::OnFileNew()
{
	if (!SaveModified())
		return;
	OnNewDocument();
 	UpdateAllViews(0);
}

