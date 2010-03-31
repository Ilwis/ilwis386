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
along with this program (see gnu-gpl v2.txt)); if not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA or visit the web page of the Free
Software Foundation, http://www.fsf.org.
																				    
Created on: 2007-02-8
***************************************************************/
/* Forms for Interactive "Applications"
Copyright Ilwis System Development ITC
september 1996, by Wim Koolhoven
Last change:  WK   17 Sep 98    2:14 pm
*/


#include "Client\Headers\AppFormsPCH.h"					   
#include "PolygonApplicationsUI\frmpolap.h"
#include "Engine\Base\System\LOGGER.H"
#include "Client\Base\BaseCommandHandlerUI.h"
#include "Client\MainWindow\CommandHandlerUI.h"
#include "PolygonApplicationsUI\PolygonMapBuffer.h"
#include "PolygonApplicationsUI\PolygonMapConvexHull.h"
#include "Client\ilwis.h"
#include "PolygonApplicationsUI\PolygonMapIntersection.h"
#include "PolygonApplicationsUI\PolygonMapUnion.h"
#include "PolygonApplicationsUI\PolygonMapDifference.h"
#include "PolygonApplicationsUI\PolygonMapSymDifference.h"
#include "PolygonApplicationsUI\frmpolrelate.h"
#include "Headers\Hs\Applications.hs"

InfoUIVector* getApplicationInfoUI() {
	InfoUIVector *infosui = new InfoUIVector();
	String ilwDir = IlwWinApp()->Context()->sIlwDir();

(*infosui).push_back(CommandHandlerUI::createApplicationInfo("raspol","",Cmdraspol,SMENUraspol,SMENUSraspol,"ExePol16IcoL",".mpr",4652,SMENUDraspol));
(*infosui).push_back(CommandHandlerUI::createApplicationInfo("segpol","",Cmdsegpol,SMENUsegpol,SMENUSsegpol,"ExePol16IcoL",".mps",4612,SMENUDsegpol));
(*infosui).push_back(CommandHandlerUI::createApplicationInfo("attribpol","",Cmdattribpol,SMENUattribpol,SMENUSattribpol,"ExePol16IcoL",".mpa",4602,SMENUDattribpol));
(*infosui).push_back(CommandHandlerUI::createApplicationInfo("maskpol","",Cmdmaskpol,SMENUmaskpol,SMENUSmaskpol,"ExePol16IcoL",".mpa",4642,SMENUDmaskpol));
(*infosui).push_back(CommandHandlerUI::createApplicationInfo("labelpol","",Cmdlabelpol,SMENUlabelpol,SMENUSlabelpol,"ExePol16IcoL",".mpa",4622,SMENUDlabelpol));
(*infosui).push_back(CommandHandlerUI::createApplicationInfo("transfpol","",Cmdtransfpol,SMENUtransfpol,SMENUStransfpol,"ExePol16IcoL",".mpa.csy",4632,SMENUDtransfpol));
(*infosui).push_back(CommandHandlerUI::createApplicationInfo("idgrid","",Cmdidgrid,SMENUidgrid,SMENUSidgrid,"ExePol16IcoL",".csy",4672,SMENUDidgrid));
(*infosui).push_back(CommandHandlerUI::createApplicationInfo("bufpol","",Cmdpolygonmapbuffer,SMENUbufpol,SMENUSbufpol,"ExePol16IcoL",".mpa.mps.mpp",0,SMENUDbufpol));
(*infosui).push_back(CommandHandlerUI::createApplicationInfo("convexhull","",Cmdpolygonmapconvexhull,SMENUconvexhull,SMENUSconvexhull,"ExePol16IcoL",".mpa.mps.mpp",0,SMENUDconvexhull));
(*infosui).push_back(CommandHandlerUI::createApplicationInfo("intersectpol","",Cmdpolygonmapintersect,SMENUintersection,SMENUSintersection,"ExePol16IcoL",".mpa",0,SMENUDintersection));
(*infosui).push_back(CommandHandlerUI::createApplicationInfo("unionpol","",Cmdpolygonmapunion,SMENUunion,SMENUSunion,"ExePol16IcoL",".mpa",0,SMENUDunion));
(*infosui).push_back(CommandHandlerUI::createApplicationInfo("diffpol","",Cmdpolygonmapdifference,SMENUdifference,SMENUSdifference,"ExePol16IcoL",".mpa",0,SMENUDdifference));
(*infosui).push_back(CommandHandlerUI::createApplicationInfo("symdiffpol","",Cmdpolygonmapsymetricdifference,SMENUsymetricdifference,SMENUSsymetricdifference,"ExePol16IcoL",".mpa",0,SMENUDsymetricdifference));
(*infosui).push_back(CommandHandlerUI::createApplicationInfo("relatepol","",Cmdpolygonmaprelate,SMENUpolrelate,SMENUSpolrelate,"ExePol16IcoL",".mpa",0,SMENUDpolrelate));

	return infosui;
}

extern "C" _export ILWIS::Module *getModuleInfo() {
	ILWIS::Module *module = new ILWIS::Module("Polygonmap applications UI", "PolygonApplicationsUI.dll",ILWIS::Module::mi37,"1.0");
	module->addMethod(ILWIS::Module::ifGetAppInfoUI, (void *)getApplicationInfoUI);  

	return module;
}
