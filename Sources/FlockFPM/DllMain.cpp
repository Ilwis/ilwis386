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
Bas Retsios, Martin Schouwenburg, Lichun Wang, Jelle Wind 

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
// DllMain.cpp : implementation file
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "client\formelements\fldlist.h"
#include "Client\FormElements\fldcol.h"
#include "Engine\Base\System\Module.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Drawers\SimpleDrawer.h" 

#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "Drawers\LayerDrawer.h"

#include "Drawers\FeatureLayerDrawer.h"

#include "FlockFPMTool.h"
#include "LCMFlockViz.h"
#include "FlockFPMDrawer.h"

using namespace ILWIS;

DrawerToolInfoVector *createDrawerTool() {
	DrawerToolInfoVector *infos = new DrawerToolInfoVector();
	infos->push_back(new DrawerToolInfo("FlockFPMTool",createFlockFPMTool));
	return infos;
}

DrawerInfoVector *createDrawer() {
	DrawerInfoVector *infos = new DrawerInfoVector();
	infos->push_back(new DrawerInfo("FlockFPMDrawer","FlockFPM",createFlockFPMDrawer));
	return infos;
}

extern "C" _export ILWIS::Module *getModuleInfo(const FileName& fnModule) {
	ILWIS::Module *module = new ILWIS::Module("FlockFPM", fnModule,ILWIS::Module::mi38,"1.3");
	module->addMethod(ILWIS::Module::ifDrawerTools, (void *)createDrawerTool);
	module->addMethod(ILWIS::Module::ifDrawers, (void *)createDrawer);  
	return module;
}
