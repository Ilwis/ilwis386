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
#include "HydroFlowApplicationsUI\FormMapFlowDirection.h"
#include "HydroFlowApplicationsUI\frmCatchmentParameters.h"
#include "Client\ilwis.h"
#include "Headers\Hs\Applications.hs"

InfoUIVector* getCommandInfoUI(ILWIS::Module *module) {
	InfoUIVector *infosui = new InfoUIVector();
	String ilwDir = IlwWinApp()->Context()->sIlwDir();
	String hyrdoDemBase = ilwDir + "Scripts\\Hydro-DEM\\";

(*infosui).push_back(CommandHandlerUI::createCommandInfo("run '" + hyrdoDemBase + "dem_visualization'.isl","",Cmdrun,SMENUdem_visualization,SMENUSdem_visualization,"ExeMap16IcoL",".isl",8070,SMENUDdem_visualization));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("fillsinks","",Cmdfillsinks,SMENUfillsinks,SMENUSfillsinks,"ExeMap16IcoL",".mpr",8022,SMENUDfillsinks));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("flowdirection","",Cmdflowdirection,SMENUflowdirection,SMENUSflowdirection,"ExeMap16IcoL",".mpr",8012,SMENUDflowdirection));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("flowaccumulation","",Cmdflowaccumulation,SMENUflowaccumulation,SMENUSflowaccumulation,"ExeMap16IcoL",".mpr",8017,SMENUDflowaccumulation));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("demoptimization","",Cmddemoptimization,SMENUdemoptimization,SMENUSdemoptimization,"ExeMap16IcoL",".mpr",8027,SMENUDdemoptimization));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("topologicaloptimization","",Cmdtopologicaloptimization,SMENUtopologicaloptimization,SMENUStopologicaloptimization,"ExeMap16IcoL",".mpr",8062,SMENUDtopologicaloptimization));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("variablethresholdcomputation","",Cmdvariablethresholdcomputation,SMENUvariablethresholdcomputation,SMENUSvariablethresholdcomputation,"ExeMap16IcoL",".mpr",8067,SMENUDvariablethresholdcomputation));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("drainagenetworkextraction","",Cmddrainagenetworkextraction,SMENUdrainagenetworkextraction,SMENUSdrainagenetworkextraction,"ExeMap16IcoL",".mpr",8032,SMENUDdrainagenetworkextraction));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("drainagenetworkordering","",Cmddrainagenetworkordering,SMENUdrainagenetworkordering,SMENUSdrainagenetworkordering,"ExeMap16IcoL",".mpr",8037,SMENUDdrainagenetworkordering));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("catchmentextraction","",Cmdcatchmentextraction,SMENUcatchmentextraction,SMENUScatchmentextraction,"ExeMap16IcoL",".mpr",8047,SMENUDcatchmentextraction));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("catchmentmerge","",Cmdcatchmentmerge,SMENUcatchmentmerge,SMENUScatchmentmerge,"ExeMap16IcoL",".mpr",8052,SMENUDcatchmentmerge));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("overlandflowlength","",Cmdoverlandflowlength,SMENUoverlandflowlength,SMENUSoverlandflowlength,"ExeMap16IcoL",".mpr",8042,SMENUDoverlandflowlength));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("flowlengthtooutlet","",Cmdflowlengthtooutlet,SMENUflowlengthtooutlet,SMENUSflowlengthtooutlet,"ExeMap16IcoL",".mpr",8042,SMENUDflowlengthtooutlet));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("flowpathlongitudinalprofile","",Cmdflowpathlongitudinalprofile,SMENUflowpathlongitudinalprofile,SMENUSflowpathlongitudinalprofile,"ExeMap16IcoL",".mpr",8042,SMENUDflowpathlongitudinalprofile));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("run '" + hyrdoDemBase + "Compound_Parameter_Extraction'.isl","",Cmdrun,SMENUCompound_Parameter_Extraction,SMENUSCompound_Parameter_Extraction,"ExeMap16IcoL",".isl",8071,SMENUSCompound_Parameter_Extraction));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("hortonplots","",Cmdhortonplots,SMENUhortonplots,SMENUShortonplots,"ExeTbl16IcoL",".tbl",8057,SMENUDhortonplots));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("run '" + hyrdoDemBase + "aggregate_statistics'.isl","",Cmdrun ,SMENUaggregate_statistics,SMENUSaggregate_statistics,"ExeTbl16IcoL",".isl",8072,SMENUDaggregate_statistics));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("run '" + hyrdoDemBase + "cumulative_hypsometric_curve'.isl","",Cmdrun,SMENUcumulative_hypsometric_curve,SMENUScumulative_hypsometric_curve,"ExeTbl16IcoL",".isl",8073,SMENUDcumulative_hypsometric_curve));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("run '" + hyrdoDemBase + "class_coverage_statistics'.isl","",Cmdrun,SMENUclass_coverage_statistics,SMENUSclass_coverage_statistics,"ExeTbl16IcoL",".isl",8074,SMENUDclass_coverage_statistics));

	return infosui;
}

extern "C" _export ILWIS::Module *getModuleInfo(const FileName& fnModule) {
	ILWIS::Module *module = new ILWIS::Module("Hydrological flow applications UI", fnModule,ILWIS::Module::mi37,"1.0");
	module->addMethod(ILWIS::Module::ifgetCommandInfoUI, (void *)getCommandInfoUI);  

	return module;
}
