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
#include "Client\Base\BaseCommandHandlerUI.h"
#include "Client\MainWindow\CommandHandlerUI.h"
#include "LandAllocation.h"
#include "FormLandAllocation.h"

extern "C" _export InfoUIVector* getCommandInfoUI(ILWIS::Module *module) {
	InfoUIVector *infosui = new InfoUIVector();
	(*infosui).push_back(CommandHandlerUI::createCommandInfo("FrmLandAllocation","",CmdLandAllocation,TR("P&oint Operations..&Land Allocation"),TR("Land Allocation of Point Map "),"ExePnt16IcoL",".mpp",4702,TR("Perform Land Allocation on a point map")));
	return infosui;
}

InfoVector* getCommandInfo() {
	InfoVector *infos = new InfoVector();
	infos->push_back(CommandMap::newCommandInfo(createLandAllocation,"PointMapLandAllocation"));
	return infos;
}

extern "C" _export ILWIS::Module *getModuleInfo(const FileName& fnModule) {
	ILWIS::Module *module = new ILWIS::Module("MOGA", fnModule,ILWIS::Module::mi38,"1.0");
	module->addMethod(ILWIS::Module::ifgetCommandInfoUI, (void *)getCommandInfoUI);
	module->addMethod(ILWIS::Module::ifgetCommandInfo, (void *)getCommandInfo);
	return module;
}