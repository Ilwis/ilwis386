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
 along with this program (see gnu-gpl v2.txt); if not, write to
 the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 Boston, MA 02111-1307, USA or visit the web page of the Free
 Software Foundation, http://www.fsf.org.

 Created on: 2014-05-02
 ***************************************************************/

#include "ProbabilityDensity\MapProbabilityDensity.h"
#include "Engine\Base\DataObjects\ObjectCollection.h"
#include "Engine\Applications\ObjectCollectionVirtual.h"
#include "ProbabilityDensity\ProbabilityDensityFromPoints.h"
#include "Client\Headers\AppFormsPCH.h"
#include "Client\FormElements\frmgenap.h"
#include "ProbabilityDensity\FormProbabilityDensity.h"
#include "Client\Base\BaseCommandHandlerUI.h"
#include "Client\MainWindow\CommandHandlerUI.h"

InfoVector* getCommandInfo() {
	InfoVector *infos = new InfoVector();
	infos->push_back(CommandMap::newCommandInfo(createMapProbabilityDensity,"MapProbabilityDensity"));
	infos->push_back(CommandMap::newCommandInfo(createProbabilityDensityFromPoints,"ProbabilityDensityFromPoints",metadataProbabilityDensityFromPoints));
	return infos;
}

InfoUIVector* getCommandInfoUI(ILWIS::Module *module) {
	InfoUIVector *infosui = new InfoUIVector();
	(*infosui).push_back(CommandHandlerUI::createCommandInfo("frmprobabilitydensity","",Cmdfrmprobabilitydensity,TR("R&asterize..&Probability Density"),TR("Probability Density"),".ioc",".mpp",0000,TR("Create a probability density raster map for each point in a Point Map")));
	return infosui;
}

extern "C" _export ILWIS::Module *getModuleInfo(const FileName& fnModule) {
	ILWIS::Module *module = new ILWIS::Module("ProbabilityDensity", fnModule,ILWIS::Module::mi38,"1.3");
	module->addMethod(ILWIS::Module::ifgetCommandInfo, (void *)getCommandInfo);
	module->addMethod(ILWIS::Module::ifgetCommandInfoUI, (void *)getCommandInfoUI);
	return module;
}



