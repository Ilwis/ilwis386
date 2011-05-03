#include "Headers\toolspch.h"
#include "Engine\Base\File\Directory.h"
#include "Engine\Base\System\LOGGER.H"
#include "Engine\Base\System\Engine.h"
#include "Engine\Applications\objvirt.h"
#include "Engine\Base\File\zlib.h"
#include "Engine\Base\File\unzip.h"
#include "engine\base\system\module.h"
#include "Engine\Map\Raster\MapList\maplist.h"
#include "Engine\Base\DataObjects\ObjectCollection.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Table\TBLHSTPL.H"
#include "Engine\Table\TBLHSTPL.H"
#include "Engine\Table\TBLHSTSG.H"
#include "Engine\Table\TBLHSTPT.H"
#include "Engine\Table\TBLHIST.H"
#include "Engine\Drawers\SVGElements.h"
#include "Engine\Drawers\SVGLoader.h"
#include "Engine\Drawers\Drawer_n.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Engine\Drawers\SelectionRectangle.h"
//#include "Client\Editors\Map\BaseMapEditor.h"
#include "Engine\Drawers\SimpleDrawer.h"
#include "Engine\Drawers\TextDrawer.h"
#include "Engine\Drawers\MouseClickInfoDrawer.h" 

using namespace ILWIS;

void ModuleMap::addModules() {
	CFileFind finder;

	String path = getEngine()->getContext()->sIlwDir() + "\\Extensions\\*.zip";
	BOOL fFound = finder.FindFile(path.scVal());
	while(fFound) {
		fFound = finder.FindNextFile();
		if (!finder.IsDirectory())
		{
			FileName fnNew (finder.GetFilePath());
			unzip(fnNew);
			_unlink(fnNew.sFullPath().scVal());
		}
	}   

	int depth = 0;
	path = getEngine()->getContext()->sIlwDir() + "\\Extensions";
	addFolder(path, depth + 1);
	applications.addExtraFunctions();

}

void ModuleMap::addFolder(const String& dir, int depth) {
	if ( depth > 2)
		return;
	CFileFind finder;
	String pattern = dir + "\\*.*";
	BOOL fFound = finder.FindFile(pattern.scVal());
	while(fFound) {
		fFound = finder.FindNextFile();
		if (!finder.IsDirectory())
		{
			FileName fnNew (finder.GetFilePath());
			if ( fnNew.sExt == ".dll" || fnNew.sExt == ".DLL")
				addModule(fnNew);
		} else {
			FileName fnNew (finder.GetFilePath());
			if ( fnNew.sFile != "." && fnNew.sFile != ".." && fnNew.sFile != "")
				addFolder(String(fnNew.sFullPath()), depth + 1);
		}
	}
	//modules that are dependent on not(yet) loaded modules may not load the first time. 
	// they get a second try as it can be assumed that the module they need is loaded then
	// of course if that module is dependent again it will not work but that situation is not
	// legal in the plugin system of ILWIS. Maybe a formal dependency definition should be given with each module(future).
	for(int i =0; i < retryList.size(); ++i) {
		addModule(retryList.at(i), true);
	}
}

void ModuleMap::addModule(const FileName& fnModule, bool retry) {
	try{
		String sName = fnModule.sFile + fnModule.sExt;
		sName = sName.toLower();
		if ( sName == "cygpng12.dll" || sName == "cygz.dll") // these two are problematic and will be skipped, not ILWIS anyway
			return;
		HMODULE hm = LoadLibrary(fnModule.sFullPath().scVal());
		if ( hm != NULL ) {
			//AppInfo f = (AppInfo)GetProcAddress(hm, "getCommandInfo");
			ModuleInfo m = (ModuleInfo)GetProcAddress(hm, "getModuleInfo");
			if ( m != NULL) {
				ILWIS::Module *mod = (*m)(fnModule);
				ILWIS::Module::ModuleInterface type = mod->getInterfaceVersion();
				getEngine()->getVersion()->fSupportsModuleInterfaceVersion(type, mod->getName());
				addModule(mod);
				AppInfo appFunc = (AppInfo)(mod->getMethod(ILWIS::Module::ifgetCommandInfo));
				if ( appFunc) {
					InfoVector *infos = (*appFunc)();
					if ( infos->size() > 0)
						applications.addApplications(*infos);
					delete infos;
				}
				ModuleInit initFunc = (ModuleInit)(mod->getMethod(ILWIS::Module::ifInit));
				if ( initFunc) {
					moduleInits[mod->getName()] = initFunc;
				}
				GetDrawers drawFuncs = (GetDrawers)(mod->getMethod(ILWIS::Module::ifDrawers));
				if ( drawFuncs) {
					DrawerInfoVector *infos = drawFuncs();
					for ( int i=0 ; i < infos->size(); ++i) {
						NewDrawer::addDrawer(infos->at(i)->name, infos->at(i)->subtype,infos->at(i)->createFunc);
						delete infos->at(i);
					}
					delete infos;
					NewDrawer::addDrawer("SelectionRectangle","ilwis38", createSelectionRectangle);
					NewDrawer::addDrawer("MouseClickInfoDrawer","ilwis38",createMouseClickInfoDrawer);
				}

			}
		} else {
			if ( retry == false) {
				vector<FileName>::iterator cur = std::find(retryList.begin(), retryList.end(),fnModule);
				if (cur == retryList.end())
					retryList.push_back(fnModule);
			}
		}
	}catch(ErrorObject& err){
		err.Show();
	}
}

void ModuleMap::initModules() {
	for(map<String, ModuleInit>::iterator cur =moduleInits.begin() ; cur != moduleInits.end(); ++cur) {
		ModuleInit moduleInit = (*cur).second;
		String name = (*cur).first;
		if ( find(name) != end()) {
			moduleInit((*this)[name]);	
		}
		
	}
}

void ModuleMap::getCommandInfo(const String& name, vector<CommandInfo *>& infos)	{ 
	CommandMap::iterator cur;
	int index=0;
	CommandInfo *inf = applications[name];
	if ( inf != 0)
		infos.push_back(inf);
	else if ( (index = name.find("*")) != string::npos) {
		if ( index == 0) {
			for(cur = applications.begin(); cur!= applications.end(); ++cur) {
				if ( name.size() == 1){
					infos.push_back((*cur).second);
					continue;
				}
				String key = (*cur).first;
				String namepart = name.substr(1);
				String keypart = key.substr(key.size() -  name.size() -1);
				if ( namepart == keypart)
					infos.push_back((*cur).second);
			}
		} else if ( index == name.size() - 1) {
			for(cur = applications.begin(); cur!= applications.end(); ++cur) {
				String key = (*cur).first;
				String namepart = name.substr(0,name.size() - 1);
				String keypart = key.substr(0,name.size() - 1);
				if ( namepart == keypart)
					infos.push_back((*cur).second);
			}
		} else 
			for(cur = applications.begin(); cur!= applications.end(); ++cur) {
				String key = (*cur).first;
				String namepart1 = name.sHead("*");
				String namepart2 = name.sTail("*");
				String keypart1 = key.substr(0,namepart1.size());
				String keypart2 = key.substr(key.size() -  namepart2.size());
				if ( namepart1 == keypart1 && namepart2 == keypart2)
					infos.push_back((*cur).second);
		}
	}
}

ModuleMap::~ModuleMap() {
	for(ModuleIter cur = this->begin(); cur != this->end(); ++cur) {
		Module *mi = (*cur).second;
		delete mi;
	}
}

void ModuleMap::addModule(ILWIS::Module *m) {
	if ( m->getName() == "")
		throw ErrorObject("Improper module definition : No name");
	if ( this->find(m->getName()) == this->end()) {
		(*this)[m->getName()] = m;
	}
}

void ModuleMap::addCommand(const String& sName, CommandFunc cf,MetaDataFunc mf) {
	applications.addCommand(sName, cf, mf);
}
//-------------------------------------------------------------------------------------------
CommandMap::~CommandMap() {
	for(AppIter cur = this->begin(); cur != this->end(); ++cur) {
		CommandInfo *ai = (*cur).second;
		delete ai;
	}
}

void CommandMap::addExtraFunctions() {
	InfoVector *infos = new InfoVector();
	(*infos).push_back(CommandMap::newCommandInfo(createTableHistogram,"TableHistogram"));
	(*infos).push_back(CommandMap::newCommandInfo(createTableHistogramPol,"TableHistogramPol"));
	(*infos).push_back(CommandMap::newCommandInfo(createTableHistogramSeg,"TableHistogramSeg"));
	(*infos).push_back(CommandMap::newCommandInfo(createTableHistogramPnt,"TableHistogramPnt"));

	addApplications(*infos);

	delete infos;
}

CommandInfo * CommandMap::newCommandInfo(CreateFunc appFunc, String appName) {
	CommandInfo *inf = new CommandInfo;
	inf->createFunction = appFunc;
	inf->name = appName.toLower();
	inf->metadata = NULL;

	return inf;
}

CommandInfo * CommandMap::newCommandInfo(CreateFunc appFunc, String appName, MetaDataFunc mdFunc) {
	CommandInfo *inf = new CommandInfo;
	inf->createFunction = appFunc;
	inf->name = appName.toLower();
	inf->metadata = mdFunc;

	return inf;
}

void CommandMap::addApplications(InfoVector apps) {
	for(InfoVIter cur = apps.begin(); cur != apps.end(); ++cur) {
		CommandInfo *ai = (*cur);
		InfoPair p(ai->name, ai);
		(*this).insert(p);
	}
}

CommandInfo * CommandMap::operator[](String n) {
	String name = n.toLower();
	AppIter iter = (*this).find(name);
	if ( iter != this->end() )
		return (*iter).second;
	for(AppIter cur = this->begin(); cur != this->end(); ++cur) {
		String entryName = (*cur).first;
		size_type index = entryName.find("___");
		if ( index == string::npos)
			continue;
		unsigned int i = 0;
		for(; i < index; ++i) {
			char c1 = name[i];
			char c2 = entryName[i];
			if ( c1 != c2 )
				break;

		}
		if ( i == index ) {
			return (*cur).second;
		}
	}
	return NULL;
}

void CommandMap::addCommand(const String& sName, CommandFunc cf,MetaDataFunc mf) {
	AppIter cur = find(sName);
	if ( cur == end()) {
		insert(pair<String, CommandInfo *>(sName, new CommandInfo(sName, cf, mf)));
	}
}
