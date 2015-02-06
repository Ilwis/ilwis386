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
#include "GeonetcastToolboxUI\frmGeonetcast.h"
#include "GeonetcastToolboxUI\RSS.h"
#include "GeonetcastToolboxUI\frmExportGTiff.h"
#include "GeonetcastToolboxUI\frmImportJason2.h"
#include "GeonetcastToolboxUI\frmImportAVHRR3.h"
#include "frmMultiImageLayerHdf4.h"
#include "Engine\Base\DataObjects\XMLDocument.h"
#include "Engine\Base\System\LOGGER.H"
#include "Client\Base\BaseCommandHandlerUI.h"
#include "Client\MainWindow\CommandHandlerUI.h"
#include "Engine\Base\File\Directory.h"
#include "Engine\Base\System\commandhandler.h"
#include "Client\FormElements\FieldBrowseDir.h"
#include "GeonetCastToolboxUI\GeonetcCastFrm.h"
#include "AmesdSadc.h"
#include "Engine\Base\System\Engine.h"
#include "DDSFrm.h"
#include "Client\ilwis.h"
#include "iniFile.h"
#include <iostream>
#include <fstream>
#include "Engine\Base\File\COPIER.H"

#define MAXLENSEARCHCFG 1000

void parseUIInfo(const String& path, map<String, String>& keys) {
	FileName configf("config.xml", FileName(path) );
	ifstream configstr (configf.sFullPath().c_str(), std::ios::in);
	char buf[MAXLENSEARCHCFG];
	configstr.read(buf,MAXLENSEARCHCFG);
	String line(buf);
	configstr.close();
	int index = 0;
	if ( (index = line.find("UIInfo")) !=  string::npos) {
		index += 7;
		int index2 = line.find("\"/>",index);
		String info = line.substr(index, index2 - index);
		Array<String> parts;
		SplitOn(info,parts," ","\"");
		for(int i = 0; i < parts.size(); ++i) {
			if ( parts[i].size() > 0) {
				String p = parts[i].sTail("=");
				//p = p.substr(1,p.size() - 2);
				keys[parts[i].sHead("=")] = p;
			}
		}
	}
	String type = line.sHead(">");
	type = type.sTail("<");
	keys["type"] = type;
}

LRESULT CmdEoToolbox(CWnd *wnd, const String& s) {
	String path = getEngine()->getContext()->sIlwDir();
	path += "Extensions\\" + s + "-Toolbox\\config.xml";

	map<String, String> keys;

	parseUIInfo(path, keys);

	new GeonetCastFrm(keys["title"],keys["type"]);

	return 1;
}

void CmdEoToolbox2(const String& s) {
	String path = getEngine()->getContext()->sIlwDir();
	path += "Extensions\\" + s.sUnQuote() + "-Toolbox\\config.xml";

	map<String, String> keys;

	parseUIInfo(path, keys);

	new GeonetCastFrm(keys["title"],keys["type"]);
}

extern "C" _export void moduleInitUI(ILWIS::Module *module) {
	CFileFind finder;
	String path = module->getLocation().sPath();
	String pluginSystemFolder = path + "\\System\\" ;
	String pattern = pluginSystemFolder + "\\*.*";
	String dest =  getEngine()->getContext()->sStdDir() + "\\";
	BOOL fFound = finder.FindFile(pattern.c_str());
	while(fFound) {
		fFound = finder.FindNextFile();
		if (!finder.IsDirectory())
		{
			FileName fnOld (finder.GetFilePath());
			FileName fnNew(dest);
			fnNew.sFile = fnOld.sFile;
			fnNew.sExt = fnOld.sExt;
			CopyFile(fnOld.sFullPath().c_str(),fnNew.sFullPath().c_str(),FALSE); 
		}
	}
	getEngine()->getContext()->ComHandler()->AddCommand("eotoolbox",CmdEoToolbox2, 0);
}

extern "C" _export InfoUIVector* getApplicationInfoUI(ILWIS::Module *module) {
	InfoUIVector *infosui = new InfoUIVector();
	String path = module->getLocation().sPath();
	map<String, String> keys;

	parseUIInfo(path, keys);

	(*infosui).push_back(CommandHandlerUI::createCommandInfo("eotoolbox '" + keys["type"] + "'","",CmdEoToolbox ,keys["menu"],"Toolbox",keys["icon"],"",8072,"Imports"));

	return infosui;
}

extern "C" _export ILWIS::Module *getModuleInfo(const FileName& fnModule) {
	String path = fnModule.sPath();
	path.toLower();
	String dir = fnModule.sDir;
	Array<String> parts;
	Split(dir,parts,"\\");
	if ( parts.size() == 0)
		Split(dir,parts,"/");
	String lastPart = parts[parts.size() - 1];
	
	String suffix = "-" + lastPart.substr(0, lastPart.size() - String("-Toolbox").size()); 

	ILWIS::Module *module = new ILWIS::Module("EO-Toolbox" + suffix, fnModule,ILWIS::Module::mi38,"2.3");
	module->addMethod(ILWIS::Module::ifgetCommandInfoUI, (void *)getApplicationInfoUI);  
	module->addMethod(ILWIS::Module::ifInit, (void *)moduleInitUI);  

	return module;
}
