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

(*infosui).push_back(CommandHandlerUI::createCommandInfo("attribpnt","",Cmdattribpnt,TR("P&oint Operations..&Attribute Map"),TR("Attribute Map of Point Map "),"ExePnt16IcoL",".mpp",4702,TR("Create an attribute point map")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("maskpnt","",Cmdmaskpnt,TR("P&oint Operations..&Mask Points"),TR("Mask Points "),"ExePnt16IcoL",".mpp",4782,TR("Selectively copy points from a point map into a new point map")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("subpnt","",Cmdsubpnt,TR("&Spatial Reference Operations.&Vector.&Sub Map"),TR("SubMap of Point Map "),"ExePnt16IcoL",".mpp",4752,TR("Copy part of a point map into a new map")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("gluepnt","",Cmdgluepnt,TR("P&oint Operations..&Glue Maps"),TR("Glue Point Maps "),"ExePnt16IcoL",".mpp",4762,TR("Glue multiple point maps into one point map")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("transfpnt","",Cmdtransfpnt,TR("&Spatial Reference Operations.&Vector.&Transform Points"),TR("Transform Points "),"ExePnt16IcoL",".mpp.csy",4742,TR("Transform points to a new coordinate system")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("raspnt","",Cmdraspnt,TR("V&ectorize..&Raster to Point"),TR("Raster to Point "),"ExePnt16IcoL",".mpr",4712,TR("Extract points from a raster map and return a point map")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("polpnt","",Cmdpolpnt,TR("V&ectorize..&Polygon to Point"),TR("Polygon to Point "),"ExePnt16IcoL",".mpa",4732,TR("Extract a point from each polygon")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("segpnt","",Cmdsegpnt,TR("V&ectorize..&Segment to Point"),TR("Segment to Point,"),"ExePnt16IcoL",".mps",4724,TR("Extract points from segments: distance interval, nodes or all coordinates")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("tblpnt","",Cmdtblpnt,TR("&Table Operations..Table to &PointMap"),TR("Table to Point "),"ExePnt16IcoL",".tbt",4792,TR("Create a point map from the X and Y columns of a table")));

(*infosui).push_back(CommandHandlerUI::createCommandInfo("intersectpnt","",Cmdpointmapintersect,TR("P&oint Operations..&Intersection"),TR("Intersection of Points"),"ExePnt16IcoL",".mpp.mps.mpa",0,TR("Create a new Point map by intersecting two Point maps")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("unionpnt","",Cmdpointmapunion,TR("P&oint Operations..&Union"),TR("Point Union"),"ExePnt16IcoL",".mpp.mps",0,TR("Create a new Point map by Union of the Points of two Point maps")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("diffpnt","",Cmdpointmapdifference,TR("P&oint Operations..&Difference"),TR("Difference of Points"),"ExePnt16IcoL",".mpp",0,TR("Create a new Point map by difference of the Points of two Point maps")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("symdiffpnt","",Cmdpointmapsymetricdifference,TR("P&oint Operations..&Symetric Difference"),TR("Symetric Difference of Points"),"ExePnt16IcoL",".mpp.mps",0,TR("Create a new Point map by the symetric difference of the Points of two Point maps")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("segmiddlepnt","",Cmdpointinsegment,TR("P&oint Operations..&Point in Segment"),TR("Find a point in a segment"),"ExePnt16IcoL",".mps",0,TR("Create a pointmap with points that are in the middle of segments")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("relatepnt","",Cmdpointmaprelate,TR("P&oint Operations..&Relate"),TR("Spatial Relation"),"ExePnt16IcoL",".mpp",0,TR("Create a point map out of spatial relation(s) with other feature maps")));

	return infosui;
}

extern "C" _export ILWIS::Module *getModuleInfo(const FileName& fnModule) {
	ILWIS::Module *module = new ILWIS::Module("Pointnmap applications UI", fnModule,ILWIS::Module::mi37,"1.0");
	module->addMethod(ILWIS::Module::ifgetCommandInfoUI, (void *)getCommandInfoUI);  

	return module;
}
