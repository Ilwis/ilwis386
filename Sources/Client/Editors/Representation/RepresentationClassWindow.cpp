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
#include "Client\Editors\Utils\ColorCB.h"
#include "Client\Editors\Utils\colorScroll.h"
#include "Client\Editors\Utils\colorIntensity.h"
#include "Client\Editors\Utils\ColorGrid.h"
#include "Client\Editors\Utils\sizecbar.h"
#include "Client\Base\ButtonBar.h"
#include "Client\Editors\Representation\ColorIntensityBar.h"
#include "Client\Editors\Representation\ColorGridBar.h"
#include "Client\Base\ButtonBar.h"
#include "Client\Editors\Representation\RprClassLB.h"
#include "Client\Editors\Representation\RepresentationClassView.h"
#include "Client\Editors\Representation\RepresentationWindow.h"
#include "Client\Editors\Representation\RepresentationClassWindow.h"
#include "Headers\Htp\Ilwis.htp"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// RepresentationClassWindow
/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(RepresentationClassWindow, RepresentationWindow)

BEGIN_MESSAGE_MAP(RepresentationClassWindow, RepresentationWindow)
	ON_WM_GETMINMAXINFO()
	ON_MESSAGE(ILWM_SETCOLOR, OnSetColor)
END_MESSAGE_MAP()

RepresentationClassWindow::RepresentationClassWindow()
{
  htpTopic = htpReprClassWindow;
	sHelpKeywords = "Representation Class editor";
}

RepresentationClassWindow::~RepresentationClassWindow()
{
}

void RepresentationClassWindow::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI)
{
	RepresentationClassView *view = (RepresentationClassView *)GetActiveView();
	if ( view == NULL) return;
	int iColumnWidth = view->iGetColumnWidth();
	lpMMI->ptMinTrackSize.x = iColumnWidth + 2*iBARWIDTH + 10;
}

LRESULT RepresentationClassWindow::OnSetColor(WPARAM wPar, LPARAM lPar)
{
	RepresentationClassView *view = dynamic_cast<RepresentationClassView *>(GetActiveView());
	if (view == NULL) return 0;
  view->ciBar.SetColor(wPar);
	view->csBar.SelectColor(wPar);
	view->tabs.rprClassLB.SetColor(wPar);
	return 0;
}

BOOL RepresentationClassWindow::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !RepresentationWindow::PreCreateWindow(cs) )
		return FALSE;

//	cs.style |= CBRS_TOOLTIPS;
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// RepresentationClassWindow diagnostics

#ifdef _DEBUG
void RepresentationClassWindow::AssertValid() const
{
	RepresentationWindow::AssertValid();
}

void RepresentationClassWindow::Dump(CDumpContext& dc) const
{
	RepresentationWindow::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// RepresentationClassWindow message handlers

