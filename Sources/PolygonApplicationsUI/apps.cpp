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

InfoUIVector* getCommandInfoUI(ILWIS::Module *module) {
	InfoUIVector *infosui = new InfoUIVector();
	String ilwDir = IlwWinApp()->Context()->sIlwDir();

(*infosui).push_back(CommandHandlerUI::createCommandInfo("raspol","",Cmdraspol,TR("V&ectorize..&Raster to Polygon"),TR("Raster to Polygon "),"ExePol16IcoL",".mpr",4652,TR("Extract polygons from a raster map and return a polygon map")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("segpol","",Cmdsegpol,TR("V&ectorize..&Segment to Polygon"),TR("Segment to Polygon "),"ExePol16IcoL",".mps",4612,TR("Polygonize a segment map")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("attribpol","",Cmdattribpol,TR("&Polygon Operations..&Attribute Map"),TR("Attribute Map of Polygon Map "),"ExePol16IcoL",".mpa",4602,TR("Create an attribute polygon map")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("maskpol","",Cmdmaskpol,TR("&Polygon Operations..&Mask Polygons"),TR("Mask Polygons "),"ExePol16IcoL",".mpa",4642,TR("Selectively copy polygons from a polygon map into a new polygon map")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("labelpol","",Cmdlabelpol,TR("&Polygon Operations..Assign &Labels"),TR("Labels to Polygons "),"ExePol16IcoL",".mpa",4622,TR("Recode polygons according to label points")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("transfpol","",Cmdtransfpol,TR("&Spatial Reference Operations.&Vector.&Transform Polygons"),TR("Transform Polygons "),"ExePol16IcoL",".mpa.csy",4632,TR("Transform polygons to a new coordinate system")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("idgrid","",Cmdidgrid,TR("&Polygon Operations..&ID Grid Map"),TR("ID Grid Map "),"ExePol16IcoL",".csy",4672,TR("Create polygon ID grid map")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("bufpol","",Cmdpolygonmapbuffer,TR("&Polygon Operations..&Buffer"),TR("Buffer "),"ExePol16IcoL",".mpa.mps.mpp",0,TR("Create a polygon map by buffering polygon,point or segment maps")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("convexhull","",Cmdpolygonmapconvexhull,TR("&Polygon Operations..&Convex Hull"),TR("Convex Hull"),"ExePol16IcoL",".mpa.mps.mpp",0,TR("Create a polygon map by making a convex hull for a point, segment or polygon map")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("intersectpol","",Cmdpolygonmapintersect,TR("&Polygon Operations..&Intersection"),TR("Intersection of Polygons"),"ExePol16IcoL",".mpa",0,TR("Create a new polygon map by intersecting two polygon maps")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("unionpol","",Cmdpolygonmapunion,TR("&Polygon Operations..&Union"),TR("Polygon Union"),"ExePol16IcoL",".mpa",0,TR("Create a new polygon map by Union of the polygons of two polygon maps")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("diffpol","",Cmdpolygonmapdifference,TR("&Polygon Operations..&Difference"),TR("Difference of Polygons"),"ExePol16IcoL",".mpa",0,TR("Create a new polygon map by difference of the polygons of two polygon maps")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("symdiffpol","",Cmdpolygonmapsymetricdifference,TR("&Polygon Operations..&Symetric Difference"),TR("Symetric Difference of Polygons"),"ExePol16IcoL",".mpa",0,TR("Create a new polygon map by the symetric difference of the polygons of two polygon maps")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("relatepol","",Cmdpolygonmaprelate,TR("&Polygon Operations..&Relate"),TR("Spatial Relation"),"ExePol16IcoL",".mpa",0,TR("Create a polygon map out of spatial relation(s) with other feature maps")));

	return infosui;
}

extern "C" _export ILWIS::Module *getModuleInfo(const FileName& fnModule) {
	ILWIS::Module *module = new ILWIS::Module("Polygonmap applications UI", fnModule,ILWIS::Module::mi37,"1.0");
	module->addMethod(ILWIS::Module::ifgetCommandInfoUI, (void *)getCommandInfoUI);  

	return module;
}
