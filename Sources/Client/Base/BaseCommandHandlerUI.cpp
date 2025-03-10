/***************************************************************
 ILWIS integrates image, vector and thematic data in one unique 
 and powerful package on the desktop. ILWIS delivers a wide 
 range of feautures including import/export, digitizing, editing, 
 analysis and display of data as well as production of 
 quality mapsinformation about the sensor mounting platform
 
 Exclusive rights of use by 52�North Initiative for Geospatial 
 Open Source Software GmbH 2007, Germany

 Copyright (C) 2007 by 52�North Initiative for Geospatial
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

#include "Client\Headers\formelementspch.h"
//#include "Engine\Drawers\drawer_n.h"
#include "Client\ilwis.h"
#include "Engine\Base\System\LOGGER.H"
#include "Engine\Base\DataObjects\Version.h"
#include "Client\Base\BaseCommandHandlerUI.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
//#include "Engine\Drawers\RootDrawer.h"
//#include "Engine\Drawers\SpatialDataDrawer.h"
//#include "Engine\Drawers\SelectionRectangle.h"
//#include "Client\Editors\Map\BaseMapEditor.h"
//#include "Engine\Drawers\SimpleDrawer.h"
//#include "Client\Mapwindow\Drawers\TextDrawer.h"
#include "Client\Mapwindow\Drawers\DrawerTool.h"
//#include "Client\Mapwindow\Drawers\CanvasBackgroundDrawer.h"
//#include "Client\Mapwindow\Drawers\MouseClickInfoDrawer.h" 
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
		CommandInfoUI *appInfo = cp.second;
		String sCmd = sParm.sTrimSpaces();
		UIHandlerFunction func = appInfo->handler;
		return (func)(parent == NULL ? wndOwner : parent, sParm);
	}
	return BaseCommandHandlerUI::OPERATION_NOT_FOUND;
}

LRESULT BaseCommandHandlerUI::Execute(const String& sCmd, CWnd *parent) {
	try{
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
	} catch (std::exception& err) {
		const char *txt = err.what();
		String mes("%s", txt);
		ErrorObject errObj(mes);
		errObj.Show();
	}catch (ErrorObject& err) {
		err.Show();
	}catch(CException* err) 
	{
		MessageBeep(MB_ICONHAND);
		err->ReportError(MB_OK|MB_ICONHAND|MB_TOPMOST);
		err->Delete();
	}
	return 0;

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
		AppInfoUI f = (AppInfoUI)(module->getMethod(ILWIS::Module::ifgetCommandInfoUI));
		if ( f ) {
			InfoUIVector *infos = (*f)(module);
			if ( infos->size() > 0)
				add(infos);
			delete infos;
		}
		ModuleInitUI initFunc = (ModuleInitUI)(module->getMethod(ILWIS::Module::ifInitUI));
		if ( initFunc) {
			moduleInits.push_back(initFunc);
		}
		GetDrawerTools toolFunc = (GetDrawerTools)(module->getMethod(ILWIS::Module::ifDrawerTools));
		if ( toolFunc) {
			DrawerToolInfoVector *infos = toolFunc();
			for ( int i=0 ; i < infos->size(); ++i) {
				DrawerTool::addCreateTool(infos->at(i)->name, infos->at(i)->createFunc);
				delete infos->at(i);
			}
			delete infos;
		}
		++index;

	}
	addExtraCommands();	 

}

void BaseCommandHandlerUI::addCommand(CommandInfoUI *ai)
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
		CommandInfoUI *ai = (*cur);
		addCommand(ai);
	}
}

CommandInfoUI *BaseCommandHandlerUI::createCommandInfo(String app, String names, UIHandlerFunction appHandler, String menuStructure,String listName, String icon, String extension, int htopic, String description, bool fVis) {
	CommandInfoUI *inf = new CommandInfoUI;
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

