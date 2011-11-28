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
/* Forms for Interactive "SegmentApplications"
Copyright Ilwis System Development ITC
september 1996, by Wim Koolhoven
Last change:  WK   17 Sep 98    2:14 pm
*/


#include "Client\Headers\AppFormsPCH.h"
#include "SegmentApplicationsUI\frmsegap.h"
#include "Client\ilwis.h"
#include "SegmentApplications\SEGATTRB.H"
#include "SegmentApplications\SEGTUNNL.H"
#include "SegmentApplications\SEGLABEL.H"
#include "SegmentApplications\SEGMASK.H"
#include "SegmentApplications\SEGPLBND.H"
#include "SegmentApplications\CNFRMRSV.H"
#include "SegmentApplications\SEGTRNSF.H"
#include "SegmentApplications\SEGSUBMP.H"
#include "SegmentApplications\SEGDENSF.H"
#include "SegmentApplications\SEGGLUE.H"
#include "SegmentApplications\SEGNUMB.H"
#include "SegmentApplications\SGFRMRSP.H"
#include "SegmentApplicationsUI\SegmentMapIntersection.h"		 
#include "SegmentApplicationsUI\SegmentMapUnion.h"
#include "SegmentApplicationsUI\SegmentMapDifference.h"
#include "SegmentApplicationsUI\SegmentMapSymDifference.h"
#include "SegmentApplicationsUI\SegmentVoronoi.h"
#include "SegmentApplicationsUI\frmSegrelate.h"
#include "Engine\Base\System\LOGGER.H"
#include "Client\Base\BaseCommandHandlerUI.h"
#include "Client\MainWindow\CommandHandlerUI.h"
#include "SegmentApplicationsUI\frmSegmentTIN.h"
#include "SegmentApplicationsUI\SegmentMapFromRasValueBndFrm.h"
#include "SegmentMapFromPointFrm.h"
#include "Headers\Hs\Applications.hs"

InfoUIVector* getCommandInfoUI(ILWIS::Module *module) {
	InfoUIVector *infosui = new InfoUIVector();
	String ilwDir = IlwWinApp()->Context()->sIlwDir();
	

(*infosui).push_back(CommandHandlerUI::createCommandInfo("attribseg","",Cmdattribseg,TR("&Segment Operations..&Attribute Map"),TR("Attribute Map of Segment Map "),"ExeSeg16IcoL",".mps",4822,TR("Create an attribute segment map")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("maskseg","",Cmdmaskseg,TR("&Segment Operations..&Mask Segments"),TR("Mask Segments "),"ExeSeg16IcoL",".mps",4812,TR("Selectively copy segments from a segment map into a new segment map")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("labelseg","",Cmdlabelseg,TR("&Segment Operations..Assign &Labels"),TR("Labels to Segments "),"ExeSeg16IcoL",".mps",4832,TR("Recode segments according to label points")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("subseg","",Cmdsubseg,TR("&Spatial Reference Operations.&Vector.&Sub Map"),TR("SubMap of Segment Map "),"ExeSeg16IcoL",".mps",4862,TR("Copy part of a segment map into a new map")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("glueseg","",Cmdglueseg,TR("&Segment Operations..&Glue Maps"),TR("Glue Segment Maps "),"ExeSeg16IcoL",".mps",4882,TR("Glue multiple segment maps into one segment map")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("densseg","",Cmddensseg,TR("&Segment Operations..&Densify Coordinates"),TR("Densify Segment Coordinates "),"ExeSeg16IcoL",".mps",4872,TR("Add more intermediate points in the segments of a segment map")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("transfseg","",Cmdtransfseg,TR("&Spatial Reference Operations.&Vector.&Transform Segments"),TR("Transform Segments "),"ExeSeg16IcoL",".mps.csy",4842,TR("Transform segments to a new coordinate system")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("tunnelseg","",Cmdtunnelseg,TR("&Segment Operations..T&unneling"),TR("Tunnel Segments "),"ExeSeg16IcoL",".mps",4852,TR("Reduce the number of intermediate points in the segments")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("cleanseg","",Cmdcleanseg,TR("&Segment Operations..&Clean Segments"),TR("Clean Segments"),"ExeSeg16IcoL",".mps",4802,TR("Clean segments: remove double segments/insert nodes")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("polseg","",Cmdpolseg,TR("V&ectorize..&Polygon to Segment"),TR("Polygon to Segment "),"ExeSeg16IcoL",".mpa",4892,TR("Extract boundaries from a polygon map")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("rasseg","",Cmdrasseg,TR("V&ectorize..&Raster to Segment"),TR("Raster to Segment "),"ExeSeg16IcoL",".mpr",4902,TR("Extract boundaries from a raster map and return a segment map")));

(*infosui).push_back(CommandHandlerUI::createCommandInfo("intersectseg","",Cmdsegmentmapintersect,TR("&Segment Operations..&Intersection"),TR("Intersection of Segments"),"ExeSeg16IcoL",".mpa.mps",0,TR("Create a new Segment map by intersecting two Segment maps")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("unionseg","",Cmdsegmentmapunion,TR("&Segment Operations..&Union"),TR("Segment Union"),"ExeSeg16IcoL",".mpa.mps",0,TR("Create a new Segment map by Union of the Segments of two Segment maps")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("diffseg","",Cmdsegmentmapdifference,TR("&Segment Operations..&Difference"),TR("Difference of Segments"),"ExeSeg16IcoL",".mpa.mps",0,TR("Create a new Segment map by difference of the Segments of two Segment maps")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("voronoi","",Cmdsegmentmapvoronoi,TR("&Segment Operations..&Voronoi map"),TR("Voronoi map of a point map"),"ExeSeg16IcoL",".mpp",0,TR("Create a Voronoi map from a pointmap")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("symdiffseg","",Cmdsegmentmapsymetricdifference,TR("&Segment Operations..&Symetric Difference"),TR("Symetric Difference of Segments"),"ExeSeg16IcoL",".mpa.mps",0,TR("Create a new Segment map by the symetric difference of the Segments of two Segment maps")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("relateseg","",Cmdsegmentmaprelate,TR("&Segment Operations..&Relate"),TR("Spatial Relation"),"ExeSeg16IcoL",".mps",0,TR("Create a segment map out of spatial relation(s) with other feature maps")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("tin","",Cmdsegmentmaptin,TR("&Segment Operations..&TIN"),TR("Triangulated Irregular Network(TIN)"),"ExeSeg16IcoL",".mps",0,TR("Create a Triangulated Irregular Network from a pointmap")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("isolines","",Cmdsegmentisoline,TR("&Segment Operations..&Iso Lines"),TR("Iso lines"),"ExeSeg16IcoL",".mpr",0,TR("Creates Iso lines from a value raster map")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("pointtosegment","",CmdSegmentMapFromPointFrm,TR("&Segment Operations..&Point to Segment"),TR("Point to segment"),"ExeSeg16IcoL",".mpp",0,TR("Creates a segment map from ordered points")));

return infosui;
}

extern "C" _export ILWIS::Module *getModuleInfo(const FileName& fnModule) {
	ILWIS::Module *module = new ILWIS::Module("Segmentmap applications UI", fnModule,ILWIS::Module::mi37,"1.2");
	module->addMethod(ILWIS::Module::ifgetCommandInfoUI, (void *)getCommandInfoUI);  

	return module;
}
