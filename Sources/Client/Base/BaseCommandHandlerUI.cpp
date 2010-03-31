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

 Created on: 2007-02-8
 ***************************************************************/

#include "Headers\toolspch.h"
#include "Client\Mapwindow\Drawers\Drawer_n.h"
#include "Client\ilwis.h"
#include "Engine\Base\System\LOGGER.H"
#include "Engine\Base\DataObjects\Version.h"
#include "Client\Base\BaseCommandHandlerUI.h"
#include "Client\Mapwindow\Drawers\Drawer_n.h"
#include "Engine\Base\System\Engine.h"
//#include "Engine\Applications\ModuleMap.h"

BaseCommandHandlerUI::BaseCommandHandlerUI(CFrameWnd *frm) {
		wndOwner = frm;
		fCleanUp = true;
}

BaseCommandHandlerUI::~BaseCommandHandlerUI() {
	
	if ( fCleanUp) {
		for( CommandIterUI ci = commands.begin(); ci!= commands.end(); ++ci )
			delete (*ci).second;
	}
}

LRESULT BaseCommandHandlerUI::doCommand(CWnd *parent, String sCom, String sParm)
{
	CommandIterUI ci = commands.find(sCom);
	if (ci != commands.end()) // known command ?
	{
		const ApplicationPairUI &cp = *ci;
		ApplicationInfoUI *appInfo = cp.second;
		String sCmd = sParm.sTrimSpaces();
		UIHandlerFunction func = appInfo->handler;
		return (func)(parent == NULL ? wndOwner : parent, sParm);
	}
	return BaseCommandHandlerUI::OPERATION_NOT_FOUND;
}

LRESULT BaseCommandHandlerUI::Execute(const String& sCmd, CWnd *parent) {
	String sCom = sCmd.sHead(" ").toLower();
	size_t iSize = min(sCom.size() + 1, sCmd.size());
	String sParm = sCmd.substr(iSize); // iSize cannot be larger than sCmd.size()
	sParm = sParm.sTrimSpaces();
	LRESULT result = doCommand(parent, sCom, sParm);
	if(  result == OPERATION_NOT_FOUND) {
		AliasesIter iter = aliases.find(sCom);
		if ( iter != aliases.end() ) {
			sCom = (*iter).second;
			result = doCommand(parent, sCom, sParm);
		}
		else
			result = OPERATION_NOT_FOUND;
	}
	
	return result;

}

void BaseCommandHandlerUI::initModules() {
	for(vector<ModuleInitUI>::iterator cur = moduleInits.begin(); cur != moduleInits.end(); ++cur) {
		ModuleInitUI moduleInit = (*cur);
		moduleInit();
	}
}

void BaseCommandHandlerUI::addModules() {
	ILWIS::Module *module;
	int index=0;
	while( ( module=getEngine()->getModule(index)) != NULL ) {
		ILWIS::Module::ModuleInterface type = module->getInterfaceVersion();
		AppInfoUI f = (AppInfoUI)(module->getMethod(ILWIS::Module::ifGetAppInfoUI));
		if ( f ) {
			InfoUIVector *infos = (*f)();
			if ( infos->size() > 0)
				add(infos);
			delete infos;
		}
		ModuleInitUI initFunc = (ModuleInitUI)(module->getMethod(ILWIS::Module::ifInitUI));
		if ( initFunc) {
			moduleInits.push_back(initFunc);
		}
		GetDrawers drawFuncs = (GetDrawers)(module->getMethod(ILWIS::Module::ifDrawers));
		if ( drawFuncs) {
			DrawerInfoVector *infos = drawFuncs();
			for ( int i=0 ; i < infos->size(); ++i) {
				IlwWinApp()->addDrawer(infos->at(i)->name, infos->at(i)->createFunc);
			}
		}
		++index;

	}
	addExtraCommands();	 

}

void BaseCommandHandlerUI::addCommand(ApplicationInfoUI *ai)
{
	commands[ai->command] = ai;
	if(ai->names.size() > 0	) {
		for(unsigned int i = 0; i < ai->names.size(); ++i) {
			String name = ai->names[i];
			aliases[name] = ai->command;
		}
	}
}
void BaseCommandHandlerUI::add(InfoUIVector *apps) {
	for(InfoUIVIter cur = apps->begin(); cur != apps->end(); ++cur) {
		ApplicationInfoUI *ai = (*cur);
		addCommand(ai);
	}
}

ApplicationInfoUI *BaseCommandHandlerUI::createApplicationInfo(String app, String names, UIHandlerFunction appHandler, String menuStructure,String listName, String icon, String extension, int htopic, String description, bool fVis) {
	ApplicationInfoUI *inf = new ApplicationInfoUI;
	Split(names, inf->names, ",");
	inf->command = app;
	inf->handler = appHandler;
	inf->menuString = menuStructure;
	inf->iconName = icon;
	inf->extension = extension;
	inf->description = description;
	inf->htopic = htopic;
	inf->operationName = listName;
	inf->visible = fVis;
	return inf;
}

void BaseCommandHandlerUI::SetOwner(CFrameWnd * owner) {
	wndOwner = owner;
}

