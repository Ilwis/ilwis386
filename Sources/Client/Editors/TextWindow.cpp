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
#include "Engine\Base\DataObjects\Color.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\Base\datawind.h"
//#include "Headers\xercesc\util\Platforms\Win32\resource.h"
#include "Headers\constant.h"
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Client\Base\Res.h"
#include "Engine\Base\AssertD.h"
#include "Client\Base\BaseView2.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\Editors\TextDoc.h"
#include "Client\Base\ButtonBar.h"
#include "Client\Editors\TextView.h"
#include "Client\Editors\TextWindow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// TextWindow

IMPLEMENT_DYNCREATE(TextWindow, DataWindow)

BEGIN_MESSAGE_MAP(TextWindow, DataWindow)
	//{{AFX_MSG_MAP(TextWindow)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	ON_COMMAND(ID_DESCRIPTIONBAR,	OnDescriptionBar)
	ON_UPDATE_COMMAND_UI(ID_DESCRIPTIONBAR,		OnUpdateDescriptionBar)
	ON_WM_CREATE()
	ON_WM_GETMINMAXINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// TextWindow construction/destruction

TextWindow::TextWindow()
{
	// TODO: add member initialization code here
	
}

TextWindow::~TextWindow()
{
}

int TextWindow::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (DataWindow::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void TextWindow::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI)
{
	lpMMI->ptMinTrackSize.x = 260;
	lpMMI->ptMinTrackSize.y = 260;
}

BOOL TextWindow::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !DataWindow::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

void TextWindow::OnDescriptionBar()
{
	OnBarCheck(ID_DESCRIPTIONBAR);
}

void TextWindow::OnUpdateDescriptionBar(CCmdUI* pCmdUI)
{
	OnUpdateControlBarMenu(pCmdUI);
}

/////////////////////////////////////////////////////////////////////////////
// TextWindow diagnostics

#ifdef _DEBUG
void TextWindow::AssertValid() const
{
	DataWindow::AssertValid();
}

void TextWindow::Dump(CDumpContext& dc) const
{
	DataWindow::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// TextWindow message handlers

