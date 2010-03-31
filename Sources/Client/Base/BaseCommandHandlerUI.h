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
// TableCommandHandler.h: interface for the TableCommandHandler class.
//
//////////////////////////////////////////////////////////////////////

#pragma once


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

struct ApplicationInfoUI;
class Tranquilizer;

typedef void(*ModuleInitUI)();
typedef vector<ApplicationInfoUI *> InfoUIVector;
typedef InfoUIVector* (*AppInfoUI)();
typedef int (*TesterFunc)();
typedef map<String, ApplicationInfoUI *> CommandMapUI;
typedef CommandMapUI::iterator CommandIterUI;
typedef InfoUIVector::iterator InfoUIVIter;
typedef LRESULT (*UIHandlerFunction)(CWnd *wnd, const String&);
typedef pair<String, ApplicationInfoUI *> ApplicationPairUI;
typedef map<String, String> Aliases;
typedef Aliases::iterator AliasesIter;


_export LRESULT Cmdrun(CWnd *parent, const String& sCmd);

class BaseCommandHandlerUI 
{
public:
	static const long OPERATION_NOT_FOUND = iUNDEF + 1;
	BaseCommandHandlerUI(CFrameWnd *frm = NULL);
	~BaseCommandHandlerUI();
	virtual LRESULT Execute(const String& sCmd, CWnd *parent = NULL);
	void addModules();
	CommandMapUI getCommands() { return commands; }
	void setCommands(CommandMapUI& cmap) { commands = cmap; fCleanUp = false;}
	_export static ApplicationInfoUI* createApplicationInfo(String app, String names, UIHandlerFunction appHandler, 
		String menuStructure,String listName, String icon, String extension, int htopic, String description, bool fVis=true);
	void SetOwner(CFrameWnd * owner);
	void initModules();

protected:
	void add(InfoUIVector *apps);
	virtual void addExtraCommands() {};
	void addCommand(ApplicationInfoUI *ai);
	CFrameWnd *wndOwner;
	CommandMapUI commands;
	bool fCleanUp;
	vector<ModuleInitUI> moduleInits;

private:
	long doCommand(CWnd *parent, String sCom, String sParm);
	
	Aliases aliases;
};

struct ApplicationInfoUI 
{
public:
	String command;
	Array<String> names;
	UIHandlerFunction handler;
	String menuString;
	String operationName;
	String iconName;
	String description;
	String extension;
	int htopic;
	bool visible;
};
