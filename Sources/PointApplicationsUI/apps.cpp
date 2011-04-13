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
#include "Engine\Base\System\LOGGER.H"
#include "Client\Base\BaseCommandHandlerUI.h"
#include "Client\MainWindow\CommandHandlerUI.h"
#include "PointApplicationsUI\frmpntap.h"
#include "pointApplicationsUI\pointMapIntersection.h"
#include "pointApplicationsUI\pointMapUnion.h"
#include "pointApplicationsUI\pointMapDifference.h"
#include "pointApplicationsUI\pointMapSymDifference.h"
#include "PointApplicationsUI\frmPointInSegment.h"
#include "PointApplicationsUI\frmpointrelate.h"
#include "Client\ilwis.h"
#include "Headers\Hs\Applications.hs"

extern "C" _export InfoUIVector* getCommandInfoUI(ILWIS::Module *module) {
	InfoUIVector *infosui = new InfoUIVector();
	String ilwDir = IlwWinApp()->Context()->sIlwDir();
	String hyrdoDemBase = ilwDir + "Scripts\\Hydro-DEM\\";

(*infosui).push_back(CommandHandlerUI::createCommandInfo("attribpnt","",Cmdattribpnt,SMENUattribpnt,SMENUSattribpnt,"ExePnt16IcoL",".mpp",4702,SMENUDattribpnt));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("maskpnt","",Cmdmaskpnt,SMENUmaskpnt,SMENUSmaskpnt,"ExePnt16IcoL",".mpp",4782,SMENUDmaskpnt));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("subpnt","",Cmdsubpnt,SMENUsubpnt,SMENUSsubpnt,"ExePnt16IcoL",".mpp",4752,SMENUDsubpnt));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("gluepnt","",Cmdgluepnt,SMENUgluepnt,SMENUSgluepnt,"ExePnt16IcoL",".mpp",4762,SMENUDgluepnt));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("transfpnt","",Cmdtransfpnt,SMENUtransfpnt,SMENUStransfpnt,"ExePnt16IcoL",".mpp.csy",4742,SMENUDtransfpnt));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("raspnt","",Cmdraspnt,SMENUraspnt,SMENUSraspnt,"ExePnt16IcoL",".mpr",4712,SMENUDraspnt));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("polpnt","",Cmdpolpnt,SMENUpolpnt,SMENUSpolpnt,"ExePnt16IcoL",".mpa",4732,SMENUDpolpnt));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("segpnt","",Cmdsegpnt,SMENUsegpnt,SMENUSsegpnt,"ExePnt16IcoL",".mps",4724,SMENUDsegpnt));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("tblpnt","",Cmdtblpnt,SMENUtblpnt,SMENUStblpnt,"ExePnt16IcoL",".tbt",4792,SMENUDtblpnt));

(*infosui).push_back(CommandHandlerUI::createCommandInfo("intersectpnt","",Cmdpointmapintersect,SMENUpntintersection,SMENUSpntintersection,"ExePnt16IcoL",".mpp.mps.mpa",0,SMENUDpntintersection));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("unionpnt","",Cmdpointmapunion,SMENUpntunion,SMENUSpntunion,"ExePnt16IcoL",".mpp.mps",0,SMENUDpntunion));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("diffpnt","",Cmdpointmapdifference,SMENUpntdifference,SMENUSpntdifference,"ExePnt16IcoL",".mpp",0,SMENUDpntdifference));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("symdiffpnt","",Cmdpointmapsymetricdifference,SMENUpntsymetricdifference,SMENUSpntsymetricdifference,"ExePnt16IcoL",".mpp.mps",0,SMENUDpntsymetricdifference));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("segmiddlepnt","",Cmdpointinsegment,SMENUpointinsegment,SMENUSpointinsegment,"ExePnt16IcoL",".mps",0,SMENUDpointinsegment));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("relatepnt","",Cmdpointmaprelate,SMENUpntrelate,SMENUSpntrelate,"ExePnt16IcoL",".mpp",0,SMENUDpntrelate));

	return infosui;
}

extern "C" _export ILWIS::Module *getModuleInfo(const FileName& fnModule) {
	ILWIS::Module *module = new ILWIS::Module("Pointnmap applications UI", fnModule,ILWIS::Module::mi37,"1.0");
	module->addMethod(ILWIS::Module::ifgetCommandInfoUI, (void *)getCommandInfoUI);  

	return module;
}
