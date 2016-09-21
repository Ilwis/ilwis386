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
// DdeServerThread.cpp: implementation of the DdeServerThread class.
//
//////////////////////////////////////////////////////////////////////

#include "Headers\toolspch.h"
#include "Engine\Map\basemap.h"
#include "Engine\SpatialReference\Coordsys.h"
#include "Client\Base\DdeServerThread.h"
#include "Client\Base\Ddeserv.h"
#include "Headers\constant.h"
#include "Client\ilwis.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP( DdeServerThread, CWinThread )
	//{{AFX_MSG_MAP( DdeServerThread )
	ON_THREAD_MESSAGE(ILW_UPDATE, OnUpdate)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()


DdeServerThread::DdeServerThread()
	: ddeServer(0)
{
	sDfltDir = IlwWinApp()->sGetCurDir();
}

DdeServerThread::~DdeServerThread()
{
	IlwWinApp()->Context()->RemoveThreadLocalVars();
}

BOOL DdeServerThread::InitInstance()
{
	// calling base class is not needed, always returns FALSE!
	IlwWinApp()->Context()->InitThreadLocalVars();
	IlwWinApp()->SetCurDir(sDfltDir);
	ddeServer = new DdeServer;
	return TRUE;
}

int DdeServerThread::ExitInstance()
{
	delete ddeServer;
	ddeServer = 0;
	return CWinThread::ExitInstance();
}

void DdeServerThread::OnUpdate(WPARAM wParam, LPARAM lParam)
{
	//CoordMessage cm = (CoordMessage) wParam;
  CoordWithCoordSystem* cwcs = (CoordWithCoordSystem*)(void*) lParam;
  ddeServer->UpdateCoordinate(*cwcs);
}
