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
// MapPaneViewTool.cpp : implementation file
//
#include "Client\Headers\formelementspch.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\ilwis.h"
#include "Client\Base\datawind.h"
#include "Client\Mapwindow\MapWindow.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\MainWindow\Catalog\CatalogDocument.h"
#include "Engine\Map\Mapview.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"


int MapPaneViewTool::idSeed=20000;


/////////////////////////////////////////////////////////////////////////////
// MapPaneViewTool

MapPaneViewTool::MapPaneViewTool(ZoomableView* mappaneview)
: mpv(mappaneview), stay(false), active(true),needsMouseFocus(false)
{
	id = idSeed++;
	if ( idSeed > 300000)
		idSeed = 200000;
}

MapPaneViewTool::~MapPaneViewTool()
{
	::ReleaseCapture();
	::SetCursor(LoadCursor(0,IDC_ARROW));
}

void MapPaneViewTool::OnMouseMove(UINT nFlags, CPoint point)
{
}

void MapPaneViewTool::OnLButtonDblClk(UINT nFlags, CPoint point)
{
}

void MapPaneViewTool::OnLButtonDown(UINT nFlags, CPoint point)
{
}

void MapPaneViewTool::OnLButtonUp(UINT nFlags, CPoint point)
{
}

void MapPaneViewTool::OnRButtonDblClk(UINT nFlags, CPoint point)
{
}

void MapPaneViewTool::OnRButtonDown(UINT nFlags, CPoint point)
{
	Stop();
}

void MapPaneViewTool::OnRButtonUp(UINT nFlags, CPoint point)
{
}

bool MapPaneViewTool::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags){
	return false;
}

bool MapPaneViewTool::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags){
	return false;
}

void MapPaneViewTool::OnEscape()
{
	Stop();
}

void MapPaneViewTool::Stop()
{
	//mpv->as = 0;
	//delete this;
}

bool MapPaneViewTool::OnSetCursor() // called by MapPaneView::OnSetCursor
{
	bool fSetCursor = (HCURSOR)0 != curActive;
	if (fSetCursor)
		::SetCursor(curActive);
	return fSetCursor;
}

void MapPaneViewTool::SetCursor(const zCursor& cur)
{
	curActive = cur;
	OnSetCursor();
}

int MapPaneViewTool::getId() { 
	return id;
}

bool MapPaneViewTool::isActive() const {
	return active;
}

void MapPaneViewTool::setActive(bool yesno) {
	if ( yesno) {
		for(map<int, MapPaneViewTool *>::iterator cur = mpv->tools.begin(); cur !=  mpv->tools.end(); ++cur) {
			MapPaneViewTool *tool = (*cur).second;
			// this will shut down all the (other)tools that need exclusive access to the mouse
			if ( tool->mouseFoucesNeeded() && tool->getId() != getId())
				tool->setActive(false);
		}
	}
	active = yesno;
}


