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
#include "Client\Headers\formelementspch.h"
#include "Client\Mapwindow\MapListSplitterWindow.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\MainWindow\Catalog\MapListDoc.h"
#include "Client\Base\WinThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(MapListSplitterWindow, DataWindow)

BEGIN_MESSAGE_MAP(MapListSplitterWindow, DataWindow)
	ON_WM_CREATE()
END_MESSAGE_MAP()


MapListSplitterWindow::MapListSplitterWindow()
{
  WinThread* wt = dynamic_cast<WinThread*>(AfxGetThread());
  String str = wt->sFileName();
  FileName fn(str);
  mpl = MapList(fn);
  Create(NULL, "");
  InitialUpdate(0,TRUE);
}

MapListSplitterWindow::~MapListSplitterWindow()
{
}

BOOL MapListSplitterWindow::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!DataWindow::PreCreateWindow(cs))
		return FALSE;
//  cs.style |= WS_HSCROLL | WS_VSCROLL;
  return TRUE;  
}

BOOL MapListSplitterWindow::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
  iHor = 3;
  iVer = 3;
  int iH, iV;
  wndSplitter.CreateStatic(this,iVer,iHor,WS_CHILD|WS_VISIBLE|WS_HSCROLL|WS_VSCROLL);
 	CCreateContext contextT;
  pContext = &contextT;
  for (iH = 0; iH < iHor; ++iH)
    for (iV = 0; iV < iVer; ++iV) 
    {
      pContext->m_pCurrentDoc = new MapCompositionDoc;
    	wndSplitter.CreateView(iH, iV, RUNTIME_CLASS(MapPaneView), CSize(200, 200), pContext);
    }
  return TRUE;
}

int MapListSplitterWindow::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
 	if (DataWindow::OnCreate(lpCreateStruct) == -1)
		return -1;

  return 0;
}

void MapListSplitterWindow::InitialUpdate(CDocument* pDoc, BOOL bMakeVisible)
{
  int iMaps = mpl->iSize();
  
  int iH = 0;    
  int iV = 0;
  for (int i = mpl->iLower(); i <= mpl->iUpper(); ++i)
  {
    MapPaneView* vw = dynamic_cast<MapPaneView*>(wndSplitter.GetPane(iH,iV));
    MapCompositionDoc* mcd = vw->GetDocument();
    mcd->OnOpenDocument(mpl[i]->fnObj.sFullName().scVal(), IlwisDocument::otNOASK);
    if (++iH >= iHor) {
      iH = 0;
      ++iV;
    }
  }

  DataWindow::InitialUpdate(0, bMakeVisible);
}
