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
#include "Headers\messages.h"
#include "Headers\constant.h"
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Client\Base\Res.h"
#include "Engine\Base\AssertD.h"
#include "Engine\Domain\dm.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\Base\BaseView2.h"
#include "Engine\Representation\Rprgrad.h"
#include "Engine\Base\DataObjects\Pair.h"
#include "Client\Base\ZappToMFC.h"
#include "Client\Editors\Representation\RepresentationDoc.h"
#include "Client\Editors\Representation\RepresentationView.h"
#include "Client\Editors\Representation\RepresentationWindow.h"
#include "Headers\Htp\Ilwis.htp"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// RepresentationWindow

IMPLEMENT_DYNCREATE(RepresentationWindow, DataWindow)

BEGIN_MESSAGE_MAP(RepresentationWindow, DataWindow)
	//{{AFX_MSG_MAP(RepresentationWindow)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// RepresentationWindow construction/destruction

RepresentationWindow::RepresentationWindow()
{
  help = "ilwis\\representation_value_gradual_editor_functionality.htm";
	sHelpKeywords = "Representation Value/Gradual editor";
}

RepresentationWindow::~RepresentationWindow()
{
}

void RepresentationWindow::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI)
{
	RepresentationView *view = (RepresentationView *)GetActiveView();
	if ( view == NULL) return;
	lpMMI->ptMinTrackSize.x = 250;
	lpMMI->ptMinTrackSize.y = 250;
}

int RepresentationWindow::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (DataWindow::OnCreate(lpCreateStruct) == -1)
		return -1;
//	status.Create(this);
	return 0;
}

BOOL RepresentationWindow::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !DataWindow::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

void RepresentationWindow::LoadState(IlwisSettings& settings)
{

	DataWindow::LoadState(settings);
}

void RepresentationWindow::SaveState(IlwisSettings& settings)
{
	DataWindow::SaveState(settings);
}

/////////////////////////////////////////////////////////////////////////////
// RepresentationWindow diagnostics

#ifdef _DEBUG
void RepresentationWindow::AssertValid() const
{
	DataWindow::AssertValid();
}

void RepresentationWindow::Dump(CDumpContext& dc) const
{
	DataWindow::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// RepresentationWindow message handlers

