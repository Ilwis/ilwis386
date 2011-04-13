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
#include "Headers\Hs\Applications.hs"

InfoUIVector* getCommandInfoUI(ILWIS::Module *module) {
	InfoUIVector *infosui = new InfoUIVector();
	String ilwDir = IlwWinApp()->Context()->sIlwDir();
	

(*infosui).push_back(CommandHandlerUI::createCommandInfo("attribseg","",Cmdattribseg,SMENUattribseg,SMENUSattribseg,"ExeSeg16IcoL",".mps",4822,SMENUDattribseg));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("maskseg","",Cmdmaskseg,SMENUmaskseg,SMENUSmaskseg,"ExeSeg16IcoL",".mps",4812,SMENUDmaskseg));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("labelseg","",Cmdlabelseg,SMENUlabelseg,SMENUSlabelseg,"ExeSeg16IcoL",".mps",4832,SMENUDlabelseg));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("subseg","",Cmdsubseg,SMENUsubseg,SMENUSsubseg,"ExeSeg16IcoL",".mps",4862,SMENUDsubseg));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("glueseg","",Cmdglueseg,SMENUglueseg,SMENUSglueseg,"ExeSeg16IcoL",".mps",4882,SMENUDglueseg));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("densseg","",Cmddensseg,SMENUdensseg,SMENUSdensseg,"ExeSeg16IcoL",".mps",4872,SMENUDdensseg));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("transfseg","",Cmdtransfseg,SMENUtransfseg,SMENUStransfseg,"ExeSeg16IcoL",".mps.csy",4842,SMENUDtransfseg));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("tunnelseg","",Cmdtunnelseg,SMENUtunnelseg,SMENUStunnelseg,"ExeSeg16IcoL",".mps",4852,SMENUDtunnelseg));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("cleanseg","",Cmdcleanseg,SMENUcleanseg,SMENUScleanseg,"ExeSeg16IcoL",".mps",4802,SMENUDcleanseg));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("polseg","",Cmdpolseg,SMENUpolseg,SMENUSpolseg,"ExeSeg16IcoL",".mpa",4892,SMENUDpolseg));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("rasseg","",Cmdrasseg,SMENUrasseg,SMENUSrasseg,"ExeSeg16IcoL",".mpr",4902,SMENUDrasseg));

(*infosui).push_back(CommandHandlerUI::createCommandInfo("intersectseg","",Cmdsegmentmapintersect,SMENUsintersection,SMENUSsintersection,"ExeSeg16IcoL",".mpa.mps",0,SMENUDsintersection));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("unionseg","",Cmdsegmentmapunion,SMENUsunion,SMENUSsunion,"ExeSeg16IcoL",".mpa.mps",0,SMENUDsunion));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("diffseg","",Cmdsegmentmapdifference,SMENUsdifference,SMENUSsdifference,"ExeSeg16IcoL",".mpa.mps",0,SMENUDsdifference));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("voronoi","",Cmdsegmentmapvoronoi,SMENUvoronoi,SMENUSvoronoi,"ExeSeg16IcoL",".mpp",0,SMENUDvoronoi));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("symdiffseg","",Cmdsegmentmapsymetricdifference,SMENUssymetricdifference,SMENUSssymetricdifference,"ExeSeg16IcoL",".mpa.mps",0,SMENUDssymetricdifference));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("relateseg","",Cmdsegmentmaprelate,SMENUsegrelate,SMENUSsegrelate,"ExeSeg16IcoL",".mps",0,SMENUDsegrelate));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("tin","",Cmdsegmentmaptin,SMENUsegTIN,SMENUSsegTIN,"ExeSeg16IcoL",".mps",0,SMENUDsegTIN));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("isolines","",Cmdsegmentisoline,TR("&Segment Operations..&Iso Lines"),TR("Iso lines"),"ExeSeg16IcoL",".mps",0,TR("Creates Iso lines from a value raster map")));

return infosui;
}

extern "C" _export ILWIS::Module *getModuleInfo(const FileName& fnModule) {
	ILWIS::Module *module = new ILWIS::Module("Segmentmap applications UI", fnModule,ILWIS::Module::mi37,"1.0");
	module->addMethod(ILWIS::Module::ifgetCommandInfoUI, (void *)getCommandInfoUI);  

	return module;
}
