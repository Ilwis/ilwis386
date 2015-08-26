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

(*infosui).push_back(CommandHandlerUI::createCommandInfo("run '" + hyrdoDemBase + "dem_visualization'.isl","",Cmdrun,TR("&DEM hydro-processing@01..DEM Visualization"),TR("DEM Visualization"),"ExeMap16IcoL",".isl",8070,TR("Run script to calculate colour shadow map")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("fillsinks","",Cmdfillsinks,TR("&DEM hydro-processing@02.&Flow Determination.Fill &Sinks"),TR("Fill Sinks"),"ExeMap16IcoL",".mpr",8022,TR("Remove local depressions from a DEM")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("flowdirection","",Cmdflowdirection,TR("&DEM hydro-processing@03.&Flow Determination.Flow &Direction"),TR("Flow Direction"),"ExeMap16IcoL",".mpr",8012,TR("Calculate a flow direction map from a Digital Elevation Model (DEM)")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("flowaccumulation","",Cmdflowaccumulation,TR("&DEM hydro-processing@04.&Flow Determination.Flow &Accumulation"),TR("Flow Accumulation"),"ExeMap16IcoL",".mpr",8017,TR("Calculate a flow accumulation map from a flow direction map")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("demoptimization","",Cmddemoptimization,TR("&DEM hydro-processing@05.&Flow Modification.DEM &Optimization"),TR("DEM Optimization"),"ExeMap16IcoL",".mpr",8027,TR("Enhance a DEM with drainage features")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("topologicaloptimization","",Cmdtopologicaloptimization,TR("&DEM hydro-processing@06.&Flow Modification.Topological Optimization"),TR("Topological Optimization"),"ExeMap16IcoL",".mpr",8062,TR("Add streams over undefined area to ensure continuous topology")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("variablethresholdcomputation","",Cmdvariablethresholdcomputation,TR("&DEM hydro-processing@07..Variable Threshold Computation"),TR("Variable Threshold Computation"),"ExeMap16IcoL",".mpr",8067,TR("Calculate a variable stream threshold map")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("drainagenetworkextraction","",Cmddrainagenetworkextraction,TR("&DEM hydro-processing@08.&Network and Catchment Extraction.Drainage Network Extraction"),TR("Drainage Network Extraction"),"ExeMap16IcoL",".mpr",8032,TR("Calculate a basic drainage raster map using thresholds")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("drainagenetworkordering","",Cmddrainagenetworkordering,TR("&DEM hydro-processing@09.&Network and Catchment Extraction.Drainage Network Ordering"),TR("Drainage Network Ordering"),"ExeMap16IcoL",".mpr",8037,TR("Fully define the drainage network, drainage links are ordered according to Strahler and Shreve system")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("catchmentextraction","",Cmdcatchmentextraction,TR("&DEM hydro-processing@10.&Network and Catchment Extraction.Catchment Extraction"),TR("Catchment Extraction"),"ExeMap16IcoL",".mpr",8047,TR("Delineate sub-catchments in the drainage network.")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("catchmentmerge","",Cmdcatchmentmerge,TR("&DEM hydro-processing@11.&Network and Catchment Extraction.Catchment Merge"),TR("Catchment Merge"),"ExeMap16IcoL",".mpr",8052,TR("Merge Sub-catchments.")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("overlandflowlength","",Cmdoverlandflowlength,TR("&DEM hydro-processing@12.&Compound Parameter Extraction.Overland Flow Length"),TR("Overland Flow Length"),"ExeMap16IcoL",".mpr",8042,TR("Calculate overland flow length")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("flowlengthtooutlet","",Cmdflowlengthtooutlet,TR("&DEM hydro-processing@13.&Compound Parameter Extraction.Flow Length to Outlet"),TR("Flow Length to Outlet"),"ExeMap16IcoL",".mpr",8042,TR("Calculate flow length to outlet")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("flowpathlongitudinalprofile","",Cmdflowpathlongitudinalprofile,TR("&DEM hydro-processing@14.&Compound Parameter Extraction.Flow Path Longitudinal Profile"),TR("Flow Path Longitudinal Profile"),"ExeMap16IcoL",".mpr",8042,TR("Calculate flow path longitudinal profile")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("run '" + hyrdoDemBase + "Compound_Parameter_Extraction'.isl","",Cmdrun,TR("&DEM hydro-processing@15.&Compound Parameter Extraction.Compound Index Calculation"),TR("Compound Index Calculation"),"ExeMap16IcoL",".isl",8071,TR("Compound Index Calculation")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("hortonplots","",Cmdhortonplots,TR("&DEM hydro-processing@16.&Statistical Parameter Extraction.Horton Statistics"),TR("Horton Statistics"),"ExeTbl16IcoL",".tbl",8057,TR("Calculate parameters for Horton plots")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("run '" + hyrdoDemBase + "aggregate_statistics'.isl","",Cmdrun ,TR("&DEM hydro-processing@17.&Statistical Parameter Extraction.Aggregate Statistics"),TR("Aggregate Statistics "),"ExeTbl16IcoL",".isl",8072,TR("Run script to calculate aggregate statistics")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("run '" + hyrdoDemBase + "cumulative_hypsometric_curve'.isl","",Cmdrun,TR("&DEM hydro-processing@18.&Statistical Parameter Extraction.Cumulative Hypsometric Curve"),TR("Cumulative Hypsometric Curve"),"ExeTbl16IcoL",".isl",8073,TR("Run script to construct cumulative hypsometric curve for a given catchment")));
(*infosui).push_back(CommandHandlerUI::createCommandInfo("run '" + hyrdoDemBase + "class_coverage_statistics'.isl","",Cmdrun,TR("&DEM hydro-processing@19.&Statistical Parameter Extraction.Class Coverage Statistics"),TR("Class Coverage Statistics"),"ExeTbl16IcoL",".isl",8074,TR("Run script to obtain the percentage coverage of each class on a selected catchment")));

	return infosui;
}

extern "C" _export ILWIS::Module *getModuleInfo(const FileName& fnModule) {
	ILWIS::Module *module = new ILWIS::Module("Hydrological flow applications UI", fnModule,ILWIS::Module::mi37,"1.1");
	module->addMethod(ILWIS::Module::ifgetCommandInfoUI, (void *)getCommandInfoUI);  

	return module;
}
