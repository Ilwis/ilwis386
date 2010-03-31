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
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\ilwis.h"
#include "Client\Base\datawind.h"
#include "Client\Mapwindow\MapWindow.h"
#include "Headers\constant.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\MainWindow\Catalog\CatalogDocument.h"
#include "Engine\Map\Mapview.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Engine\SpatialReference\GR3D.H"
#include "Client\Mapwindow\MiniMapPaneView.h"
#include "Client\Base\Framewin.h"
#include "Client\Mapwindow\Drawers\BaseDrawer.h"
#include "Client\Mapwindow\Drawers\Drawer.h"
#include "Client\Mapwindow\Drawers\BaseMapDrawer.h"
#include "Client\Editors\Editor.h"
#include "Client\Editors\Editor.h"
#include "Client\Editors\Georef\GeoRef3DEditor.h"
#include "Client\Editors\Georef\GeoRef3DDirectionPane.h"
#include "Client\Editors\Georef\GeoRef3DDirectionTool.h"
#include "Headers\Hs\Mapwind.hs"


GeoRef3DDirectionTool::GeoRef3DDirectionTool(GeoRef3DDirectionPane *pane, GeoRef3DEditorBar *edit) :
	MapPaneViewTool( (MapPaneView *)pane),
	mpvGr3D(pane),
	gr3DBar(edit),
	fChanged(false)
{
	pFrom = pTo = CPoint(iUNDEF, iUNDEF);
}

GeoRef3DDirectionTool::~GeoRef3DDirectionTool()	
{
}

void GeoRef3DDirectionTool::OnMouseMove(UINT nFlags, CPoint point)
{
	if (fDown && pFrom != CPoint(iUNDEF, iUNDEF) ) {
		if (pTo == point)
			return;
		mpvGr3D->SetDirectionPoints(pFrom, pTo);
		pTo = point;
		Coord crdTo = mpvGr3D->crdPnt(pTo);
		gr3DBar->SetValue(GeoRef3DEditorBar::ituFORM, GeoRef3DEditorBar::vtuFOCALPOINT, crdTo.x, crdTo.y);
		mpvGr3D->SetDirectionPoints(pFrom, pTo);
		fChanged = true;
	}	
}


void GeoRef3DDirectionTool::OnLButtonDown(UINT nFlags, CPoint point)
{
	mpvGr3D->Invalidate();
  pFrom = pTo = point;
  fDown = true;
	Coord crdFrom = mpvGr3D->crdPnt(pFrom);
	gr3DBar->SetValue(GeoRef3DEditorBar::ituFORM, GeoRef3DEditorBar::vtuVIEWPOINT, crdFrom.x, crdFrom.y);
	mpvGr3D->SetDirectionPoints(pFrom, pTo);
	
}

void GeoRef3DDirectionTool::OnLButtonUp(UINT nFlags, CPoint point)
{
  pTo = point;
  fDown = FALSE;
  if (pTo == pFrom) 
    return;
	Coord crdTo = mpvGr3D->crdPnt(pTo);
	gr3DBar->SetValue(GeoRef3DEditorBar::ituFORM, GeoRef3DEditorBar::vtuFOCALPOINT, crdTo.x, crdTo.y);	
	mpvGr3D->SetDirectionPoints(pFrom, pTo);	
	
	mpvGr3D->Invalidate();
	if ( fChanged )
	{
		CDocument *doc = gr3DBar->MainPane()->GetDocument();
		doc->UpdateAllViews(0);
	}			
	Stop();
}




