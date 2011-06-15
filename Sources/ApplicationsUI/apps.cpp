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
#include "ApplicationsUI\frmmapap.h"
#include "ApplicationsUI\frmmapcl.h"
#include "ApplicationsUI\frmmapip.h"
#include "ApplicationsUI\frmmapip.h"
#include "ApplicationsUI\frmmatap.h"
#include "ApplicationsUI\frmtblap.h"
#include "ApplicationsUI\frmUniqueId.h"
#include "ApplicationsUI\frmappia.h"
#include "ApplicationsUI\frmstereopaircr.h"
#include "ApplicationsUI\FormMaplistGraph.h"
#include "ApplicationsUI\FormMapAggregateMapList.h"
#include "ApplicationsUI\FormMapListChangeDetection.h"
#include "Client\Base\IlwisDocument.h"
#include "Engine\Table\tblinfo.h"
#include "Client\TableWindow\TableDoc.h"
#include "ApplicationsUI\Tblforms.h"
#include "Engine\Base\System\LOGGER.H"
#include "Client\Base\BaseCommandHandlerUI.h"
#include "Client\MainWindow\CommandHandlerUI.h"
#include "ApplicationsUI\TableCreateTimeColumn.h"
#include "Client\ilwis.h"
#include "Headers\Hs\Applications.hs"

extern "C" _export InfoUIVector* getCommandInfoUI(ILWIS::Module *module) {
	InfoUIVector *infosui = new InfoUIVector();
	String ilwDir = IlwWinApp()->Context()->sIlwDir();
	String hyrdoDemBase = ilwDir + "Scripts\\Hydro-DEM\\";

(*infosui).push_back(CommandHandlerUI::createCommandInfo("changedetect","",Cmdchangedetection,TR("&Raster Operations..&Change detection"),TR("Change Detection"),".mpl",".mpl",0,TR("Detect changes in a maplist with baseline data")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("aggregatemaplist","",Cmdaggregatemaplist,TR("&Raster Operations..&Aggregate Maplist"),TR("Aggregate Maplist"),"ExeMap16IcoL",".mpl",0,TR("Aggregate a maplist into a single map")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("apply3d","",Cmdapply3d, SMENUApply3D,SMENUSApply3D,"ExeMap16IcoL","",4102,SMENUDApply3D));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("mapcalc","",Cmdmapcalc,SMENUMapCalc,SMENUSMapCalc,"ExeMap16IcoL","",4022,SMENUDMapCalc));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("maplistcalc","",Cmdmaplistcalc,SMENUMapListCalc,SMENUSMapListCalc,".mpl","",4022,SMENUDMapListCalc));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("attribras","",Cmdattribras,SMENUAttribMap,SMENUSAttribMap,"ExeMap16IcoL",".mpr",4002,SMENUDAttribMap));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("cross","",Cmdcross,SMENUCross,SMENUSCross,"ExeTbl16IcoL",".mpr",4202,SMENUDCross));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("aggregate","",Cmdaggregate,SMENUAggregate,SMENUSAggregate,"ExeMap16IcoL",".mpr.mpl",4192,SMENUDAggregate));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("distance","",Cmddistance,SMENUDistance,SMENUSDistance,"ExeMap16IcoL",".mpr",4052,SMENUDDistance));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("effectdistance","",Cmdeffectdistance,SMENUEffect,SMENUSEffect,"ExeMap16IcoL",".mpr",4052,SMENUDEffect));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("mapiter","",Cmdmapiter,SMENUItteration,SMENUSItteration,"ExeMap16IcoL",".mpr",4912,SMENUDItteration));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("areanumb","",Cmdareanumb,SMENUANumbering,SMENUSANumbering,"ExeMap16IcoL",".mpr",4012,SMENUDANumbering));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("subras","",Cmdsubras,SMENUSubMap,SMENUSSubMap,"ExeMap16IcoL",".mpr.mpl",4162,SMENUDSubMap));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("glueras","",Cmdglueras,SMENUGlueRas,SMENUSGlueRas,"ExeMap16IcoL",".mpr",4072,SMENUDGlueRas));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("mirror","",Cmdmirror,SMENUMirrorRotate,SMENUSMirrorRotate,"ExeMap16IcoL",".mpr.mpl",4082,SMENUDMirrorRotate));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("filter","",Cmdfilter,SMENUFilter,SMENUSFilter,"ExeMap16IcoL",".mpr.fil.mpl",4062,SMENUDFilter));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("stretch","",Cmdstretch,SMENUStretch,SMENUSStretch,"ExeMap16IcoL",".mpr.mpl",4152,SMENUDStretch));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("slicing","",Cmdslicing,SMENUSlicing,SMENUSSlicing,"ExeMap16IcoL",".mpr",4142,SMENUDSlicing));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("colorsep","",Cmdcolorsep,SMENUColorSep,SMENUSColorSep,"ExeMap16IcoL",".mpr",4182,SMENUDColorSep));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("mapcolorcomp","clrcmp",OpenMapListColorComp,"Image &Processing..Color Composite","Color Composite","ExeMap16IcoL","",4222,"Create a color composite of three image bands"));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("cluster","",Cmdcluster,SMENUCluster,SMENUSCluster,"ExeMap16IcoL",".mpr",4212,SMENUDCluster));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("classify","clsfy",Cmdclassify,SMENUClassify,SMENUSClassify,"ExeMap16IcoL",".sms.clf",4032,SMENUDClassify));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("resample","",Cmdresample,SMENUResample,SMENUSResample,"ExeMap16IcoL",".mpr.grf.mpl",4092,SMENUDResample));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("makestpfromdtm","",Cmdmakestpfromdtm,SMENUStereoPairDTM,SMENUSStereoPairDTM,".stp",".mpr",6102,SMENUDStereoPairDTM));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("histogram","hist",Cmdhistogram,SMENUHistogram,SMENUSHistogram,"ExeTbl16IcoL",".mpr.mpa.mps.mpp",5205,SMENUDHistogram));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("autocorr","",Cmdautocorr,SMENUAutoCorr,SMENUSAutoCorr,"ExeTbl16IcoL",".mpr",5002,SMENUDAutoCorr));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("variogramsurface","",Cmdvariogramsurface,SMENUVariogram,SMENUSVariogram,"ExeMap16IcoL",".mpr",4372,SMENUDVariogram));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("princcmp","",Cmdprinccmp,SMENUPrincmp,SMENUSPrincmp,"ExeMat16IcoL",".mpl",6002,SMENUDPrincmp));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("factanal","",Cmdfactanal,SMENUfactanal,SMENUSfactanal,"ExeMat16IcoL",".mpl",6012,SMENUDfactanal));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("matvarcov","",Cmdmatvarcov,SMENUVarCov,SMENUSVarCov,"ExeMat16IcoL",".mpl",6022,SMENUDVarCov));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("matcorr","",Cmdmatcorr,SMENUMatCor,SMENUSMatCor,"ExeMat16IcoL",".mpl",6032,SMENUDMatCor));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("mapliststatistics","",Cmdmapliststatistics,SMENUMplStat,SMENUSMplStat,"ExeMap16IcoL",".mpl",0000,SMENUDMplStat));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("histnbpol","",Cmdhistnbpol,SMENUNBPol,SMENUSNBPol,"ExeTbl16IcoL",".mpa",5022,SMENUDNBPol));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("histsegdir","",Cmdhistsegdir,SMENUHistSegDist,SMENUSHistSegDist,"ExeTbl16IcoL",".mps",5012,SMENUDHistSegDist));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("spatcorr","",Cmdspatcorr,SMENUSpatCorr,SMENUSSpatCorr,"ExeTbl16IcoL",".mpp",5042,SMENUDSpatCorr));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("variogramsurface","",Cmdvariogramsurface,SMENUvgramsufrace,SMENUSvgramsufrace,"ExeMap16IcoL",".mpp",4372,SMENUDvgramsufrace));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("crossvariogram","",Cmdcrossvariogram,SMENUCrossVario,SMENUSCrossVario,"ExeTbl16IcoL",".mpp",5072,SMENUDCrossVario));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("pattanal","",Cmdpattanal,SMENUPatAnal,SMENUSPatAnal,"ExeTbl16IcoL",".mpp",5032,SMENUDPatAnal));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("densras","",Cmddensras,SMENUDensRas,SMENUSDensRas,"ExeMap16IcoL",".mpr.mpl",4042,SMENUDDensRas));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("krigingras","",Cmdkrigingras,SMENUKrigRas,SMENUSKrigRas,"ExeMap16IcoL",".mpr",4112,SMENUSKrigRas));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("interpolseg","",Cmdinterpolseg,SMENUinterpolseg,SMENUSinterpolseg,"ExeMap16IcoL",".mps",4512,SMENUDinterpolseg));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("nearestpnt","",Cmdnearestpnt,SMENUnearestpnt,SMENUSnearestpnt,"ExeMap16IcoL",".mpp",4322,SMENUDnearestpnt));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("movaverage","",Cmdmovaverage,SMENUmovaverage,SMENUSmovaverage,"ExeMap16IcoL",".mpp",4332,SMENUDmovaverage));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("trendsurface","",Cmdtrendsurface,SMENUtrendsurface,SMENUStrendsurface,"ExeMap16IcoL",".mpp",4342,SMENUDtrendsurface));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("movsurface","",Cmdmovsurface,SMENUmovsurface,SMENUSmovsurface,"ExeMap16IcoL",".mpp",4352,SMENUDmovsurface));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("kriging","",Cmdkriging,SMENUkriging,SMENUSkriging,"ExeMap16IcoL",".mpp",4362,SMENUDkriging));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("anisotrkriging","",Cmdanisotrkriging,SMENUanisotrkriging,SMENUSanisotrkriging,"ExeMap16IcoL",".mpp",4365,SMENUDanisotrkriging));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("univkriging","",Cmdunivkriging,SMENUunivkriging,SMENUSunivkriging,"ExeMap16IcoL",".mpp",4366,SMENUDunivkriging));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("cokriging","",Cmdcokriging,SMENUcokriging,SMENUScokriging,"ExeMap16IcoL",".mpp",4382,SMENUDcokriging));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("uniqueid","",Cmduniqueid,SMENUuniqueid,SMENUSuniqueid,"ExeGenMap16Ico",".mpp.mps.mpa",5502,SMENUDuniqueid));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("transformheight","",Cmdtransformheight,SMENUtransformheight,SMENUStransformheight,".csy",".csy",4773,SMENUDtransformheight));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("transform","",Cmdtransform,SMENUtransform,SMENUStransform,".csy",".csy",4772,SMENUDtransform));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("transformtogeocentric","",Cmdtransformtogeocentric,SMENUtransformtogeocentric,SMENUStransformtogeocentric,".csy",".csy",4774,SMENUDtransformtogeocentric));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("transformfromgeocentric","",Cmdtransformfromgeocentric,SMENUtransformfromgeocentric,SMENUStransformfromgeocentric,".csy",".csy",4775,SMENUDtransformfromgeocentric));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("transformviageocentric","",Cmdtransformviageocentric,SMENUtransformviageocentric,SMENUStransformviageocentric,".csy",".csy",4776,SMENUDtransformviageocentric));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("polras","",Cmdpolras,SMENUpolras,SMENUSpolras,"ExeMap16IcoL",".mpa.grf",4402,SMENUDpolras));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("segras","",Cmdsegras,SMENUsegras,SMENUSsegras,"ExeMap16IcoL",".mps.grf",4502,SMENUDsegras));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("segdensity","",Cmdsegdensity,SMENUsegdensity,SMENUSsegdensity,"ExeMap16IcoL",".mps.grf",4522,SMENUDsegdensity));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("pntras","",Cmdpntras,SMENUpntras,SMENUSpntras,"ExeMap16IcoL",".mpp.grf",4302,SMENUDpntras));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("pntdensity","",Cmdpntdensity,SMENUpntdensity,SMENUSpntdensity,"ExeMap16IcoL",".mpp.grf",4312,SMENUDpntdensity));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("transpose","",Cmdtranspose,SMENUtranspose,SMENUStranspose,"ExeTbl16IcoL",".tbt",5052,SMENUDtranspose));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("tblchdom","",Cmdtblchdom,SMENUtblchdom,SMENUStblchdom,"ExeTbl16IcoL",".tbt",5062,SMENUDtblchdom));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("tblglue","",Cmdtblglue,SMENUtblglue,SMENUStblglue,"ExeTbl16IcoL",".tbt",5082,SMENUDtblglue));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("bursawolf","", Cmdbursawolf,SMENUcoordBursawolf,SMENUScoordBursawolf,".csy",".csy",-1,""));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("maplistgraph","",Cmdmaplistgraph,SMENUmaplistgraph,SMENUSmaplistgraph,".grh",".mpl",0000,SMENUDmaplistgraph));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("clmaggregate","",Cmdclmaggregate,"","",".grh",".mpl",0000,"Calculate the aggregate of a column", false));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("clmsemivariogram","",Cmdclmsemivar,"","",".grh",".mpl",0000,"Calculate the semivariogram of a column", false));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("clmstatistics","",Cmdclmstatistiscs,"","",".grh",".mpl",0000,"Calculate the statistics of a column", false));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("clmcumulative","",Cmdclmcumulative,"","",".grh",".mpl",0000,"", false));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("confmat","",Cmdclmconfusionmatrix,"","",".grh",".mpl",0000,"", false));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("clmleastsquares","",Cmdclmleastsquares,"","",".grh",".mpl",0000,"", false));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("clmsort","",Cmdclmsort,"","",".grh",".mpl",0000,"", false));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("clmslicing","",Cmdclmslicing,"","",".grh",".mpl",0000,"", false));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("clminvmoldensky","",Cmdclminvmoldensky,"","",".grh",".mpl",0000,"", false));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("clmfindmoldensky","",Cmdclmfindmoldensky,"","",".grh",".mpl",0000,"", false));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("clmfind7parameters","",Cmdclmfind7parameters,"","",".grh",".mpl",0000,"", false));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("clmmoledensky","",Cmdclmmoledensky,"","",".grh",".mpl",0000,"", false));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("clmhelmert7parms","",Cmdclmhelmert7parms,"","",".grh",".mpl",0000,"", false));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("clmfindazimuthdistance","",Cmdclmfindazimuthdistance,"","Find Azimuth Distance",".grh",".mpl",0000,"Find Azimuth Distance", false));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("timecolumn","",Cmdtimecolumn,TR("&Table Operations..&Create time column"),TR("Create time column"),"ExeTbl16IcoL",".tbl",0,TR("Create time column from maplist name information")));

(*infosui).push_back(CommandHandlerUI::createCommandInfo("updateallcolumns","",CmdUpdateAllColumns,"","",".grh",".mpl",0000,"", false));

return infosui;
}

extern "C" _export ILWIS::Module *getModuleInfo(const FileName& fnModule) {
	ILWIS::Module *module = new ILWIS::Module("Rastermap and table applications UI", fnModule,ILWIS::Module::mi37,"1.0");
	module->addMethod(ILWIS::Module::ifgetCommandInfoUI, (void *)getCommandInfoUI);  

	return module;
}
