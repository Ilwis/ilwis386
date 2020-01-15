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
#pragma warning( disable : 4786 )

#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\FldOneSelectTextOnly.h"
#include "Client\FormElements\fldfontn.h"
#include "Engine\Base\File\Directory.h"
#include "Client\Editors\Utils\SYMBOL.H"
#include "Client\Forms\FormGeneralPreferences.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\FormElements\selector.h"
#include "Client\FormElements\fldrpr.h"
#include "Client\FormElements\fldcolor.h"
#include "Engine\Base\File\Directory.h"
#include "Client\FormElements\FieldBrowseDir.h"
#include "Client\FormElements\FieldRprListView.h"
#include "Client\MainWindow\ACTION.H"
#include "Client\MainWindow\ACTPAIR.H"
#include "Engine\Base\System\Engine.h"
#include "Engine\Base\System\LOGGER.H"
#include "Client\Base\BaseCommandHandlerUI.h"
#include "Client\MainWindow\CommandHandlerUI.h"
#include "Client\ilwis.h"
#include "Client\FormElements\TreeSelector.h"
#include "Headers\Hs\Editor.hs"
#include "Headers\Hs\GeneralPrefForm.hs"
//#include <shfolder.h>

FieldPage::FieldPage(FormEntry *entry) :
	FieldGroup(entry)
{
	psn->iPosY = psn->iBndDown;
	SetIndependentPos();
}

void FieldPage::create()
{
	FieldGroup::create();
	Hide();
}


//------------------------------------------------------------
FormGeneralPreferences::FormGeneralPreferences() :
  FormWithDest(IlwWinApp()->GetMainWnd(), TR("Preferences")),
	fInitial(true),
	curPage(NULL)
{
	IlwisSettings settings("DefaultSettings");

	IlwisSettings settings2("DefaultSettings\\DataObjectPos");
	mapPos.resize(4);
	debugLog = false;
	for(int i=0; i<mapPos.size(); ++i)
	{
		mapPos[i].rcMin.Col = settings2.iValue(String("X%d", i), iUNDEF);
		mapPos[i].rcMin.Row = settings2.iValue(String("Y%d", i), iUNDEF);
		mapPos[i].rcMax.Col = settings2.iValue(String("XSize%d", i), iUNDEF);
		mapPos[i].rcMax.Row = settings2.iValue(String("YSize%d", i), iUNDEF);
	}

	tree = new TreeSelector(root);
	tree->SetCallBack((NotifyProc)&FormGeneralPreferences::Fill);
	pages.push_back(FormGeneralPreferences::Page(TR("Main Window"), new FieldPage(root)));
	pages.push_back(FormGeneralPreferences::Page(TR("Main Window#Position & Size"), new FieldPage(root)));
	pages.push_back(FormGeneralPreferences::Page(TR("Main Window#Menu"), new FieldPage(root)));
	pages.push_back(FormGeneralPreferences::Page(TR("Map Window"), new FieldPage(root)));
	pages.push_back(FormGeneralPreferences::Page(TR("Map Window#Display"), new FieldPage(root)));
	pages.push_back(FormGeneralPreferences::Page(TR("Map Window#Display#Raster Maps"), new FieldPage(root)));
	pages.push_back(FormGeneralPreferences::Page(TR("Map Window#Display#Segment Maps"), new FieldPage(root)));
	pages.push_back(FormGeneralPreferences::Page(TR("Map Window#Display#Polygon Maps"), new FieldPage(root)));
	pages.push_back(FormGeneralPreferences::Page(TR("Map Window#Display#Point Maps"), new FieldPage(root)));
	pages.push_back(FormGeneralPreferences::Page(TR("Map Window#Default Representations"), new FieldPage(root)));
	pages.push_back(FormGeneralPreferences::Page(TR("Map Window#Position & Size"), new FieldPage(root)));
	pages.push_back(FormGeneralPreferences::Page(TR("Map Window#Editors"), new FieldPage(root)));
	pages.push_back(FormGeneralPreferences::Page(TR("Map Window#Editors#Point Editor"), new FieldPage(root)));
	pages.push_back(FormGeneralPreferences::Page(TR("Map Window#Editors#Segment Editor"), new FieldPage(root)));
	pages.push_back(FormGeneralPreferences::Page(TR("Table Window"), new FieldPage(root)));
	pages.push_back(FormGeneralPreferences::Page(TR("Table Window#Graph Window"), new FieldPage(root)));
	pages.push_back(FormGeneralPreferences::Page(TR("Font"), new FieldPage(root)));
	pages.push_back(FormGeneralPreferences::Page(TR("Font#General Font"), new FieldPage(root)));
	pages.push_back(FormGeneralPreferences::Page(TR("Font#Dialog Font"), new FieldPage(root)));
	pages.push_back(FormGeneralPreferences::Page(TR("Font#Table Font"), new FieldPage(root)));
	pages.push_back(FormGeneralPreferences::Page(TR("Font#Graphs Font"), new FieldPage(root)));
	pages.push_back(FormGeneralPreferences::Page(TR("General"), new FieldPage(root)));
	pages.push_back(FormGeneralPreferences::Page(TR("Directories"), new FieldPage(root)));
	pages.push_back(FormGeneralPreferences::Page(TR("Advanced"), new FieldPage(root)));	
	for(PageIter cur=pages.begin(); cur != pages.end(); ++cur)
		(*cur).page->Align(tree, AL_AFTER);



	MainWindowPage(settings);

	MapWindowPage(settings);

	DirectoryPage(settings);

  FontPage(settings);

	TableWindowPage(settings);

   GeneralPage(settings);

	AdvancedPage(settings);

  SetMenHelpTopic("ilwismen\\main_window_preferences.htm");
	create();
}
FormGeneralPreferences::~FormGeneralPreferences()
{
}

void FormGeneralPreferences::GeneralPage(IlwisSettings& settings)
{
	FieldPage *page = GetPage(TR("General"));
	fOldTranquilizers = settings.fValue("OldTranquilizers", false);
	language = settings.sValue("LanguageExtension",".eng");

	cbOldTranquilizers = new CheckBox(page, TR("Old style progress indicators"), &fOldTranquilizers);
	new FieldString(page,TR("Language extension used"),&language);
}

void FormGeneralPreferences::FontPage(IlwisSettings& settings)
{
	FieldPage *page = GetPage(TR("Font"));
	new StaticText(page, TR("This item contains subentries, please expand. "));

	page = GetPage(TR("Font#General Font"));
	FieldGroup *grp = new FieldGroup(page);
	new StaticText(grp, TR("General Font"), true);
	StaticText* st = new StaticText(grp, TR("The font that is used in the Main Window, in Catalogs"));
	st->psn->SetBound(0,0,0,0);
	new StaticText(grp, TR("and in data windows"));
	grp->SetBevelStyle(FormEntry::bsLOWERED);

	new FieldBlank(page);

	CFont * fnt = IlwWinApp()->GetFont(IlwisWinApp::sfWindowMedium);
	fnt->GetLogFont(&lfGeneral);
	FieldLogFont *fl = new FieldLogFont(page, &lfGeneral);
	fl->SetIndependentPos();

	page = GetPage(TR("Font#Dialog Font"));
	grp = new FieldGroup(page);
	new StaticText(grp,TR("Dialog Font"), true);
	st = new StaticText(grp, TR("The font that is used in all dialog boxes"));
//	st->psn->SetBound(0,0,0,0);
//	new StaticText(grp, TR("<Not used>"));
	grp->SetBevelStyle(FormEntry::bsLOWERED);

	new FieldBlank(page);

	fnt = IlwWinApp()->GetFont(IlwisWinApp::sfFORM);
	fnt->GetLogFont(&lfForm);
  fl = new FieldLogFont(page, &lfForm, FieldLogFont::faVECTOR);
	fl->SetIndependentPos();

	page = GetPage(TR("Font#Table Font"));

	grp = new FieldGroup(page);
	new StaticText(grp, TR("Table Font"), true);
	new StaticText(grp, TR("The font that is used in all tables."));
	grp->SetBevelStyle(FormEntry::bsLOWERED);

	new FieldBlank(page);
	fnt = IlwWinApp()->GetFont(IlwisWinApp::sfTABLE);
	fnt->GetLogFont(&lfTable);
  fl = new FieldLogFont(page, &lfTable, FieldLogFont::faFIXED);
	fl->SetIndependentPos();

	page = GetPage(TR("Font#Graphs Font"));

	grp = new FieldGroup(page);
	new StaticText(grp, TR("Graph Font"), true);
	new StaticText(grp, TR("The font that is used in all graphs."));
	grp->SetBevelStyle(FormEntry::bsLOWERED);

	new FieldBlank(page);

	fnt = IlwWinApp()->GetFont(IlwisWinApp::sfGRAPH);
	fnt->GetLogFont(&lfGraph);
	fl = new FieldLogFont(page, &lfGraph, FieldLogFont::faTRUETYPE);
	fl->SetIndependentPos();
}

void FormGeneralPreferences::MainWindowPage(IlwisSettings& settings)
{
	FieldPage *page = GetPage(TR("Main Window"));
	StaticText* st = new StaticText(page, TR("Commandline"), true);
	st->SetIndependentPos();
	iHistoryEntries = settings.iValue("HistoryEntries", 8);
	new FieldInt(page, TR("&History entries"), &iHistoryEntries, ValueRange(0, 30), true);

	new FieldBlank(page);
	st = new StaticText(page, TR("General Options"), true);
	st->psn->SetBound(0,0,0,0);
	st->SetIndependentPos();

	fWarnNotUpToDate = settings.fValue("WarnNotUpToDate", false);
	fShowRasterDef = settings.fValue("ShowRasterDefForm", true);
	CheckBox* cb = new CheckBox(page, TR("Give a &Warning when an object is opened which is not up-to-date"), &fWarnNotUpToDate);
	cb->SetIndependentPos();
	cb = new CheckBox(page, TR("Show a &Raster map definition form when calculating"), &fShowRasterDef);
	cb->SetIndependentPos();

	new FieldBlank(page);
	st = new StaticText(page, TR("Default double-click actions"), true);
	st->psn->SetBound(0,0,0,0);
	st->SetIndependentPos();
	sMplAction = settings.sValue("MapListDblClkAction", "Open");
	st = new StaticText(page, TR("&Filter"));
	sFltAction = settings.sValue("FilterDblClkAction", "Open");
	ftoFlt = new FieldOneSelectTextOnly(page, &sFltAction);
	sStpAction = settings.sValue("StereoPairDblClkAction", "Stereoscope");
	
	ftoFlt->Align(st, AL_AFTER);
	st = new StaticText(page, TR("&Map List"));
	ftoMpl = new FieldOneSelectTextOnly(page, &sMplAction);
	ftoMpl->Align(st, AL_AFTER);
	st = new StaticText(page, TR("&Stereo Pair"));
	ftoStp = new FieldOneSelectTextOnly(page, &sStpAction);
	ftoStp->Align(st, AL_AFTER);

	page = GetPage(TR("Main Window#Position & Size"));
	st = new StaticText(page, TR("Default position and size of Main Window"), true);
	st->SetIndependentPos();
	MWX = new FieldInt(page, TR("&X Position"), &(mapPos[mpMAIN].rcMin.Col), ValueRange(1, 2000), false, true);
	MWY = new FieldInt(page, TR("&Y Position"), &(mapPos[mpMAIN].rcMin.Row), ValueRange(1, 2000), false, true);
	MWW = new FieldInt(page, TR("&Width"), &(mapPos[mpMAIN].rcMax.Col), ValueRange(100, 2000), false, true);
	MWH = new FieldInt(page, TR("&Height"), &(mapPos[mpMAIN].rcMax.Row), ValueRange(100, 2000), false, true);

	new PushButton(page, TR("&Reset"), (NotifyProc)&FormGeneralPreferences::ResetButtonMW);

	page = GetPage(TR("Main Window#Menu"));
	useAltLayout = settings.fValue("OperationsMenu",false);
	st = new StaticText(page,"Operations Menu structure", true);
	st->SetIndependentPos();
	new PushButton(page, "Save curent layout", (NotifyProc)&FormGeneralPreferences::SaveOperationsLayout);
	new CheckBox(page,"Use alternative layout",&useAltLayout);
}

int FormGeneralPreferences::SaveOperationsLayout(Event *){
	FileName fn(String("%S\\Resources\\Def\\OperationsLayout.def", getEngine()->getContext()->sIlwDir()));
	IlwWinApp()->getCommands()->saveOperationsDefinitions(fn);
	return 1;
}

int FormGeneralPreferences::ResetButtonMW(Event *)
{
	MWX->SetVal(iUNDEF);
	MWY->SetVal(iUNDEF);
	MWW->SetVal(iUNDEF);
	MWH->SetVal(iUNDEF);	

	return 1;
}

void FormGeneralPreferences::TableWindowPage(IlwisSettings& settings)
{
	FieldPage *page = GetPage(TR("Table Window"));

	fShowAsTableCL = settings.fValue("ShowAsTableCL", false);
	new StaticText(page, TR("Commandline"), true);
  new CheckBox(page, TR("&Show Command Line when showing non-tables"), &fShowAsTableCL);
	new FieldBlank(page);

	new StaticText(page, TR("Default position and size of Table Window"), true);
	FieldGroup* fg = new FieldGroup(page);
	TWX = new FieldInt(fg, TR("&X Position"), &(mapPos[mpTABLE].rcMin.Col), ValueRange(1, 2000), false, true);
	TWY = new FieldInt(fg, TR("&Y Position"), &(mapPos[mpTABLE].rcMin.Row), ValueRange(1, 2000), false, true);
	TWW = new FieldInt(fg, TR("&Width"), &(mapPos[mpTABLE].rcMax.Col), ValueRange(100, 2000), false, true);
	TWH = new FieldInt(fg, TR("&Height"), &(mapPos[mpTABLE].rcMax.Row), ValueRange(100, 2000), false, true);
	fg->SetIndependentPos();
	
	new PushButton(page, TR("&Reset"), (NotifyProc)&FormGeneralPreferences::ResetButtonTW);

	GraphWindowPage(settings);
}

int FormGeneralPreferences::ResetButtonTW(Event *)
{
	TWX->SetVal(iUNDEF);
	TWY->SetVal(iUNDEF);
	TWW->SetVal(iUNDEF);
	TWH->SetVal(iUNDEF);	

	return 1;
}

void FormGeneralPreferences::GraphWindowPage(IlwisSettings& settings)
{
	FieldPage *page = GetPage(TR("Table Window#Graph Window"));

	StaticText* st = new StaticText(page, TR("Default position and size of Graph Window"), true);
	st->SetIndependentPos();
	GWX = new FieldInt(page, TR("&X Position"), &(mapPos[mpGRAPH].rcMin.Col), ValueRange(1, 2000), false, true);
	GWY = new FieldInt(page, TR("&Y Position"), &(mapPos[mpGRAPH].rcMin.Row), ValueRange(1, 2000), false, true);
	GWW = new FieldInt(page, TR("&Width"), &(mapPos[mpGRAPH].rcMax.Col), ValueRange(100, 2000), false, true);
	GWH = new FieldInt(page, TR("&Height"), &(mapPos[mpGRAPH].rcMax.Row), ValueRange(100, 2000), false, true);

	new PushButton(page, TR("&Reset"), (NotifyProc)&FormGeneralPreferences::ResetButtonGW);	

}

int FormGeneralPreferences::ResetButtonGW(Event *)
{
	GWX->SetVal(iUNDEF);
	GWY->SetVal(iUNDEF);
	GWW->SetVal(iUNDEF);
	GWH->SetVal(iUNDEF);	

	return 1;
}

void FormGeneralPreferences::MapWindowPage(IlwisSettings& settings)
{
	FieldPage *pg = GetPage(TR("Map Window"));
	fPanWheel = settings.fValue("PanWheel", false);
	new CheckBox(pg, TR("Old style scrollwheel behavior"), &fPanWheel);
	new StaticText(pg, TR("(use scrollwheel for Panning instead of Zooming)"));

	FieldPage *page = GetPage(TR("Map Window#Position & Size"));

	StaticText* st = new StaticText(page, TR("Default position and size of Map Window"), true);
	st->SetIndependentPos();
	MpWX = new FieldInt(page, TR("&X Position") , &(mapPos[mpMAPS].rcMin.Col), ValueRange(1, 2000), false, true);
	MpWY = new FieldInt(page, TR("&Y Position"), &(mapPos[mpMAPS].rcMin.Row), ValueRange(1, 2000), false, true);
	MpWW = new FieldInt(page, TR("&Width"), &(mapPos[mpMAPS].rcMax.Col), ValueRange(100, 2000), false, true);
	MpWH = new FieldInt(page, TR("&Height"), &(mapPos[mpMAPS].rcMax.Row), ValueRange(100, 2000), false, true);

	new PushButton(page, TR("&Reset"), (NotifyProc)&FormGeneralPreferences::ResetButtonMpW);		

	DefaultRprPage(settings);

	pg = GetPage(TR("Map Window#Editors"));
	new StaticText(pg, TR("This item contains subentries, please expand. "));
	SegmentEdMapPage();

	PointEdMapPage();

	DisplayOptionsPages(settings);
}

int FormGeneralPreferences::ResetButtonMpW(Event *)
{
	MpWX->SetVal(iUNDEF);
	MpWY->SetVal(iUNDEF);
	MpWW->SetVal(iUNDEF);
	MpWH->SetVal(iUNDEF);	

	return 1;
}

void FormGeneralPreferences::PointEdMapPage()
{
	IlwisSettings settings("Map Window\\Point Editor");
	FieldPage *page = GetPage(TR("Map Window#Editors#Point Editor"));
	// to ensure that on show time the fill color field is only shoen when needed
  page->SetCallBack((NotifyProc)&FormGeneralPreferences::FieldSymbolCallBack);
	StaticText* st = new StaticText(page, TR("Point Editor"), true);
	st->SetIndependentPos();

  String fn = IlwWinApp()->Context()->fnUserINI().sFullName();
  char sBuf[80];
  String sVal = "yes";
  GetPrivateProfileString("Point Editor", "Show Text", sVal.sVal(), sBuf, 79, fn.sVal());
  fText = strcmp(sBuf, "no") ? true : false;
	fText = settings.fValue("Show Text", fText);
	colText = (Color)-1;
  sVal = String("%06lx", (long)colText);
  GetPrivateProfileString("Point Editor", "Text Color", sVal.sVal(), sBuf, 79, fn.sVal());
  sscanf(sBuf,"%lx",&colText);
	colText = settings.clrValue("Text Color", colText);
  colBackText = (Color)-2;
  sVal = String("%06lx", (long)colBackText);
  GetPrivateProfileString("Point Editor", "Background Color", sVal.sVal(), sBuf, 79, fn.sVal());
  sscanf(sBuf,"%lx",&colBackText);
	colBackText = settings.clrValue("Background Color", colBackText);
	iSmb = smbPlus;
  iSmb = (SymbolType)GetPrivateProfileInt("Point Editor", "Symbol Type", (int)iSmb, fn.sVal());
	iSmb = (SymbolType)settings.iValue("Symbol Type", (int)iSmb);
	iSmbSize = 3;
  iSmbSize = GetPrivateProfileInt("Point Editor", "Symbol Size", iSmbSize, fn.sVal());
	iSmbSize = settings.iValue("Symbol Size", iSmbSize);
	iSmbWidth = 1;
  iSmbWidth = GetPrivateProfileInt("Point Editor", "Pen Width", iSmbWidth, fn.sVal());
	iSmbWidth = settings.iValue("Pen Width", iSmbWidth);
	colBound = (Color)-1;
  sVal = String("%06lx", (long)colBound);
  GetPrivateProfileString("Point Editor", "Pen Color", sVal.sVal(), sBuf, 79, fn.sVal());
  sscanf(sBuf,"%lx",&colBound);
	colBound = settings.clrValue("Pen Color", colBound);
	colFill = (Color)-2; 
  sVal = String("%06lx", (long)colFill);
  GetPrivateProfileString("Point Editor", "Fill Color", sVal.sVal(), sBuf, 79, fn.sVal());
  sscanf(sBuf,"%lx",&colFill);
	colFill = settings.clrValue("Fill Color", colFill);

  CheckBox* cbText = new CheckBox (page, TR("&Show Text"), &fText);
  FieldGroup* fg = new FieldGroup(cbText);
  fg->Align(cbText,AL_UNDER);
  new FieldColor(fg, TR("Text &Color"), &colText);
  new FieldFillColor(fg, TR("&Background"), &colBackText);
  new FieldBlank(page);
  fsmb = new FieldSymbol(page, TR("Symbol &Type"), &iSmb, &hIcon);
  fsmb->SetCallBack((NotifyProc)&FormGeneralPreferences::FieldSymbolCallBack);
  new FieldInt(page, TR("Symbol &Size"), &iSmbSize, ValueRange(1L,100L), true);
  new FieldInt(page, TR("Pen &width"), &iSmbWidth, ValueRange(1L,100L), true);
  new FieldColor(page, TR("&Color"), &colBound);
  ffc = new FieldFillColor(page, TR("&Fill Color"), &colFill);
}

int FormGeneralPreferences::FieldSymbolCallBack(Event*)
{
  fsmb->StoreData();
  switch (SymbolType(iSmb)) 
	{
    case smbCircle:
    case smbSquare:
    case smbDiamond:
    case smbDeltaUp:
    case smbDeltaDown:
      ffc->Show();
      break;
    case smbPlus:
    case smbMinus:
    case smbCross:
      ffc->Hide();
      break;
  }
	return 1;
}

void FormGeneralPreferences::SegmentEdMapPage()
{
  colNorm = Color(0,255,0); // green
  colRetouch = Color(255,0,0); // red
  colDeleted = Color(0,255,255); // cyan
  iSnapPixels = 5;								 

	IlwisSettings settings("Map Window\\Segment Editor");
  String fn = IlwWinApp()->Context()->fnUserINI().sFullName();
  char sBuf[80];
  String sVal = "yes";
  GetPrivateProfileString("Segment Editor", "Auto Snap", sVal.sVal(), sBuf, 79, fn.sVal());
  fAutoSnap = strcmp(sBuf, "no") ? true : false;
	fAutoSnap = settings.fValue("Auto Snap", fAutoSnap);
  sVal = "yes";
  GetPrivateProfileString("Segment Editor", "Show Nodes", sVal.sVal(), sBuf, 79, fn.sVal());
  fShowNodes = strcmp(sBuf, "no") ? true : false;
	fShowNodes = settings.fValue("Show Nodes", fShowNodes);

  iSnapPixels = GetPrivateProfileInt("Segment Editor", "Snap Pixels", 5, fn.sVal());
	iSnapPixels = settings.iValue("Snap Pixels", iSnapPixels);
  if (iSnapPixels < 1)
    iSnapPixels = 1;
  sVal = String("%06lx", (long)colNorm);
  GetPrivateProfileString("Segment Editor", "Normal Color", sVal.sVal(), sBuf, 79, fn.sVal());
  sscanf(sBuf,"%lx",&colNorm);
	colNorm = settings.clrValue("Normal Color", colNorm);
  sVal = String("%06lx", (long)colRetouch);
  GetPrivateProfileString("Segment Editor", "Retouch Color", sVal.sVal(), sBuf, 79, fn.sVal());
  sscanf(sBuf,"%lx",&colRetouch);
	colRetouch = settings.clrValue("Retouch Color", colRetouch);
  sVal = String("%06lx", (long)colDeleted);
  GetPrivateProfileString("Segment Editor", "Deleted Color", sVal.sVal(), sBuf, 79, fn.sVal());
  sscanf(sBuf,"%lx",&colDeleted);
	colDeleted = settings.clrValue("Deleted Color", colDeleted);
  colFindUndef = Color(255,0,0); // red
	colFindUndef = settings.clrValue("Find Undef Color", colFindUndef);

	FieldPage *page = GetPage(TR("Map Window#Editors#Segment Editor"));
	StaticText* st = new StaticText(page, TR("Segment Editor"), true);
	st->SetIndependentPos();
  new FieldColor(page, TR("&Color Digitizer Cursor"), &colDig);
  new CheckBox(page, TR("&Auto Snap"), &fAutoSnap);
  new CheckBox(page, TR("Show &Nodes"),&fShowNodes);
	new FieldInt(page, TR("&Snap tolerance (pixels)"), &iSnapPixels, ValueRange(2,30), true);
	new FieldColor(page, TR("&Normal color"), &colNorm);
	new FieldColor(page, TR("&Retouch color"), &colRetouch);
	new FieldColor(page, TR("&Deleted color"), &colDeleted);
	new FieldColor(page, TR("Find &Undef color"), &colFindUndef);
}

void FormGeneralPreferences::GetSystemRpr(IlwisSettings& settings)
{
	String sSystemDir = IlwWinApp()->ilwapp->sStdDir();
	CFileFind finder;
	BOOL fFound  = finder.FindFile(String("%S\\*.dom", sSystemDir).c_str());
	fFound = finder.FindNextFile();
	while ( fFound )
	{
			if ( finder.IsDirectory() || finder.IsDots() )
				continue;	
			FileName fn(finder.GetFilePath());

			String sDefaultRpr = settings.sValue(fn.sFile, "");
			if ( sDefaultRpr == "")
			{
				String sDomValue;
				ObjectInfo::ReadElement("Domain", "Type", fn, sDomValue);
				if ( fCIStrEqual(sDomValue, "DomainValue") || fCIStrEqual(sDomValue, "DomainImage"))
				{
					ObjectInfo::ReadElement("Domain", "Representation", fn, sDefaultRpr);
					FileName fnRpr(sDefaultRpr);
					fnRpr.Dir(sSystemDir);
					defRpr[fn.sFile.toLower()] = fnRpr.sFullPath();
				}
			}
			else
				defRpr[fn.sFile.toLower()] = sDefaultRpr;
			fFound = finder.FindNextFile();
	}
}

void FormGeneralPreferences::DefaultRprPage(IlwisSettings& settings)
{
	FieldPage *page = GetPage(TR("Map Window#Default Representations"));
	GetSystemRpr(settings);
	rprList = new FieldRprListView(page, defRpr);
}

void FormGeneralPreferences::DirectoryPage(IlwisSettings& settings)
{
	FieldPage *page = GetPage(TR("Directories"));

	sToolDir = settings.sValue("ToolDir", "");
	sScriptDirs = settings.sValue("ScriptDirs", "");
	char sPath[MAX_PATH];
	LPITEMIDLIST pidl = NULL;
	SHGetSpecialFolderLocation(0, CSIDL_APPDATA, &pidl);
	SHGetPathFromIDList(pidl, sPath);
	
  LPMALLOC pMalloc = NULL;
  SHGetMalloc(&pMalloc);
  if(pidl)
		pMalloc->Free(pidl);
  pMalloc->Release();

	String sDef(sPath);
	sLogDir = settings.sValue("LogDir",sDef);
	sDefaultStartUpDir = settings.sValue("DefaultStartUpDir", sDef);

	FormEntry *fe = new FieldBrowseDir(page, TR("&Tool Directories"), TR("&Tool Directories"), &sToolDir);
	fe->SetWidth(100);
	fe = new FieldBrowseDir(page, TR("&Script Directories"), TR("&Script Directories"), &sScriptDirs);
	fe->SetWidth(100);
	fe = new FieldBrowseDir(page, TR("Directory for &Log file"), TR("Directory for &Log file"), &sLogDir);
	fe->SetWidth(100);
	fe = new FieldBrowseDir(page, TR("&Default Startup dir"), TR("&Default Startup dir"), &sDefaultStartUpDir);
	fe->SetWidth(100);
}


void FormGeneralPreferences::AdvancedPage(IlwisSettings& settings)
{
	FieldPage *page = GetPage(TR("Advanced"));

	fSoftwareRendering = settings.fValue("SoftwareRendering", false);

  FieldGroup *grp = new FieldGroup(page);
	new StaticText(grp, TR("Restore Defaults"), true);
	new StaticText(grp, TR("To remove user-specific registry keys upon exit"));
	new StaticText(grp, TR("i.e. all default settings will be restored upon exit."));
	grp->SetBevelStyle(FormEntry::bsLOWERED);	
	new FieldBlank(page, 0.25);	
	new PushButton(page, TR("Restore defaults"), (NotifyProc)&FormGeneralPreferences::RestoreDefaults);
	new CheckBox(page,TR("Use debug logging"),&debugLog);
	new CheckBox(page,TR("Use Software Rendering (Compatibility mode)"),&fSoftwareRendering);
}

int FormGeneralPreferences::RestoreDefaults(Event *)
{
	if ( IlwisSettings::fDeletePossible() )
		IlwWinApp()->fRemoveUserRegistry = true;
	else
		MessageBox(TR("Delete not possible, Administrator rights required").c_str(), TR("Error").c_str(), MB_OK | MB_ICONEXCLAMATION);
	return 1;
}


void FormGeneralPreferences::DisplayOptionsPages(IlwisSettings& settings)
{
	String sOptions = settings.sValue("ShowDisplayOptions");
	if ( sOptions != "")
	{
		Array<String> arVals;
		Split(sOptions, arVals, ",");
		for(vector<String>::iterator cur = arVals.begin(); cur !=arVals.end(); ++cur)
			dispOptions.push_back((*cur).iVal());
	}

	if ( dispOptions.size() < IlwisWinApp::dosEND)
	{
		dispOptions.resize(IlwisWinApp::dosEND);
		fill(dispOptions.begin(), dispOptions.end(), 1);
	}
	dispChecks.resize(dispOptions.size());

	FieldPage *page = GetPage(TR("Map Window#Display"));
	dispOptions[IlwisWinApp::dosALL] = false;
//	dispChecks[IlwisWinApp::dosALL] = new CheckBox(page, TR("&Show display options when opening a map"), &dispOptions[IlwisWinApp::dosALL]);

	new FieldBlank(page);
	new StaticText(page, TR("Default Colors for domain Bool"), true);
	FieldGroup* fg = new FieldGroup(page);
	clrYes = Color(0,176,20);
	clrYes = settings.clrGetRGBColor("YesColor", clrYes);
	new FieldColor(fg, TR("&True"), &clrYes);
	clrNo = Color(168,168,168);
	clrNo = settings.clrGetRGBColor("NoColor", clrNo);
	new FieldColor(fg, TR("&False"), &clrNo);
	fg->SetIndependentPos();

	new FieldBlank(page);
	new StaticText(page, TR("Default Color for Single colored maps"), true);
	fg = new FieldGroup(page);
	clrSinglePnt = clrSingleSeg = clrSinglePol = Color(0,176,20);
	clrSinglePnt = settings.clrGetRGBColor("SingleColorPoints", clrSinglePnt);
	clrSingleSeg = settings.clrGetRGBColor("SingleColorSegments", clrSingleSeg);
	clrSinglePol = settings.clrGetRGBColor("SingleColorPolygons", clrSinglePol);
	new FieldColor(fg, TR("&Point Maps"), &clrSinglePnt);
	new FieldColor(fg, TR("&Segment Maps"), &clrSingleSeg);
	new FieldColor(fg, TR("&Polygon Maps"), &clrSinglePol);
	fg->SetIndependentPos();

	page = GetPage(TR("Map Window#Display#Raster Maps"));
	new StaticText(page, TR("Show Display Options dialog box for:"), true);
	dispChecks[IlwisWinApp::dosRAS]			= new CheckBox(page, TR("&All Raster maps"), &dispOptions[IlwisWinApp::dosRAS]);
	dispChecks[IlwisWinApp::dosRASIMG]		= new CheckBox(page, TR("using domain &Image"), &dispOptions[IlwisWinApp::dosRASIMG]);
	dispChecks[IlwisWinApp::dosRASIMG]->psn->iBndLeft += 15;
	dispChecks[IlwisWinApp::dosRASVAL]		= new CheckBox(page, TR("using domain &Value"), &dispOptions[IlwisWinApp::dosRASVAL]);
	dispChecks[IlwisWinApp::dosRASSORT]	= new CheckBox(page, TR("using domain &Class/ID"), &dispOptions[IlwisWinApp::dosRASSORT]);
	dispChecks[IlwisWinApp::dosRASBOOL]	= new CheckBox(page, TR("using domain &Bool"), &dispOptions[IlwisWinApp::dosRASBOOL]);
	dispChecks[IlwisWinApp::dosRASOTHER] = new CheckBox(page, TR("using &other domains"), &dispOptions[IlwisWinApp::dosRASOTHER]);

	dispChecks[IlwisWinApp::dosRAS]->SetCallBack((NotifyProc)&FormGeneralPreferences::ShowMapDispOptionsChecks);
	for ( int i=IlwisWinApp::dosRASIMG; i<= IlwisWinApp::dosRASOTHER; ++i)
		dispChecks[i]->SetCallBack((NotifyProc)&FormGeneralPreferences::ShowDomainDispOptionsChecks);

	StaticText * stPyramidFiles = new StaticText(page, TR("Pyramid Layers"), true);
	stPyramidFiles->psn->iBndLeft -= 15;
	fPyrCreateFirstDisplay = settings.fValue("CreatePyrWhenFirstDisplayed", false);
	new CheckBox(page, TR("Create Pyramid Layers by default"), &fPyrCreateFirstDisplay);	

	page = GetPage(TR("Map Window#Display#Segment Maps"));
	new StaticText(page, TR("Show Display Options dialog box for:"), true);
	dispChecks[IlwisWinApp::dosSEG] = new CheckBox(page, TR("&All Segment maps"), &dispOptions[IlwisWinApp::dosSEG]);
	dispChecks[IlwisWinApp::dosSEGVAL]		= new CheckBox(page, TR("using domain &Value"), &dispOptions[IlwisWinApp::dosSEGVAL]);
	dispChecks[IlwisWinApp::dosSEGVAL]->psn->iBndLeft += 15;
	dispChecks[IlwisWinApp::dosSEGSORT]	= new CheckBox(page, TR("using domain &Class/ID"), &dispOptions[IlwisWinApp::dosSEGSORT]);
	dispChecks[IlwisWinApp::dosSEGBOOL]	= new CheckBox(page, TR("using domain &Bool"), &dispOptions[IlwisWinApp::dosSEGBOOL]);
	dispChecks[IlwisWinApp::dosSEGOTHER] = new CheckBox(page, TR("using &other domains"), &dispOptions[IlwisWinApp::dosSEGOTHER]);


	page = GetPage(TR("Map Window#Display#Polygon Maps"));
	new StaticText(page, TR("Show Display Options dialog box for:"), true);
	dispChecks[IlwisWinApp::dosPOL] = new CheckBox(page, TR("&All Polygon maps"), &dispOptions[IlwisWinApp::dosPOL]);
	dispChecks[IlwisWinApp::dosPOLVAL]		= new CheckBox(page, TR("using domain &Value"), &dispOptions[IlwisWinApp::dosPOLVAL]);
	dispChecks[IlwisWinApp::dosPOLVAL]->psn->iBndLeft += 15;
	dispChecks[IlwisWinApp::dosPOLSORT]	= new CheckBox(page, TR("using domain &Class/ID"), &dispOptions[IlwisWinApp::dosPOLSORT]);
	dispChecks[IlwisWinApp::dosPOLBOOL]	= new CheckBox(page, TR("using domain &Bool"), &dispOptions[IlwisWinApp::dosPOLBOOL]);
	dispChecks[IlwisWinApp::dosPOLOTHER] = new CheckBox(page, TR("using &other domains"), &dispOptions[IlwisWinApp::dosPOLOTHER]);

	page = GetPage(TR("Map Window#Display#Point Maps"));
	new StaticText(page, TR("Show Display Options dialog box for:"), true);
	dispChecks[IlwisWinApp::dosPNT] = new CheckBox(page, TR("&All Point maps"), &dispOptions[IlwisWinApp::dosPNT]);
	dispChecks[IlwisWinApp::dosPNTVAL]		= new CheckBox(page, TR("using domain &Value"), &dispOptions[IlwisWinApp::dosPNTVAL]);
	dispChecks[IlwisWinApp::dosPNTVAL]->psn->iBndLeft += 15;
	dispChecks[IlwisWinApp::dosPNTSORT]	= new CheckBox(page, TR("using domain &Class/ID"), &dispOptions[IlwisWinApp::dosPNTSORT]);
	dispChecks[IlwisWinApp::dosPNTBOOL]	= new CheckBox(page, TR("using domain &Bool"), &dispOptions[IlwisWinApp::dosPNTBOOL]);
	dispChecks[IlwisWinApp::dosPNTOTHER] = new CheckBox(page, TR("using &other domains"), &dispOptions[IlwisWinApp::dosPNTOTHER]);

	for (int i=IlwisWinApp::dosRAS; i < IlwisWinApp::dosEND; ++i)
		dispChecks[i]->SetCallBack((NotifyProc)&FormGeneralPreferences::ShowDomainDispOptionsChecks);

	dispChecks[IlwisWinApp::dosRAS]->SetCallBack((NotifyProc)&FormGeneralPreferences::ShowMapDispOptionsChecks);
	dispChecks[IlwisWinApp::dosSEG]->SetCallBack((NotifyProc)&FormGeneralPreferences::ShowMapDispOptionsChecks);
	dispChecks[IlwisWinApp::dosPOL]->SetCallBack((NotifyProc)&FormGeneralPreferences::ShowMapDispOptionsChecks);
	dispChecks[IlwisWinApp::dosPNT]->SetCallBack((NotifyProc)&FormGeneralPreferences::ShowMapDispOptionsChecks);

//	dispChecks[IlwisWinApp::dosALL]->SetCallBack((NotifyProc)&FormGeneralPreferences::ShowMapDispOptionsChecks);

}

void FormGeneralPreferences::SetMapCheck(int iStart, int iEnd, int iTarget)
{
	bool fVal;
	for (int i=iStart; i <= iEnd; ++i)
	{
		bool fCurVal = ((CheckBox *)(dispChecks[i]))->fVal();
		fVal = i==iStart ?  fCurVal : fVal && fCurVal  ;
	}
	dispChecks[iTarget]->SetVal(fVal);
}

int FormGeneralPreferences::ShowDomainDispOptionsChecks(Event*)
{
	if ( !fEndInitialization ) return 1;
	SetMapCheck(IlwisWinApp::dosRASIMG, IlwisWinApp::dosRASOTHER, IlwisWinApp::dosRAS);
	SetMapCheck(IlwisWinApp::dosSEGVAL, IlwisWinApp::dosSEGOTHER, IlwisWinApp::dosSEG);
	SetMapCheck(IlwisWinApp::dosPOLVAL, IlwisWinApp::dosPOLOTHER, IlwisWinApp::dosPOL);
	SetMapCheck(IlwisWinApp::dosPNTVAL, IlwisWinApp::dosPNTOTHER, IlwisWinApp::dosPNT);
	return 1;
}

void FormGeneralPreferences::SetDomainCheck(int iStart, int iEnd, int iType)
{
	if (dispChecks[iType]->fClicked)
	{
		bool fCheck = dispChecks[iType]->fVal();
		for ( int i=iStart; i<= iEnd; ++i)
		{
			dispOptions[i] = fCheck;
			dispChecks[i]->SetVal(fCheck);
		}
		dispChecks[iType]->fClicked = false;
	}
}

int FormGeneralPreferences::ShowMapDispOptionsChecks(Event*)
{
	if ( !fEndInitialization ) return 1;
	SetDomainCheck(IlwisWinApp::dosRASIMG, IlwisWinApp::dosRASOTHER, IlwisWinApp::dosRAS); 
	SetDomainCheck(IlwisWinApp::dosSEGVAL, IlwisWinApp::dosSEGOTHER, IlwisWinApp::dosSEG); 
	SetDomainCheck(IlwisWinApp::dosPOLVAL, IlwisWinApp::dosPOLOTHER, IlwisWinApp::dosPOL); 
	SetDomainCheck(IlwisWinApp::dosPNTVAL, IlwisWinApp::dosPNTOTHER, IlwisWinApp::dosPNT); 
//	if ( dispChecks[iType]->fClicked == true )
//	SetDomainCheck(IlwisWinApp::dosRAS, IlwisWinApp::dosEND - 1, IlwisWinApp::dosALL);

	return 1;
}

FieldPage *FormGeneralPreferences::GetPage(const String& sVal)
{
	for(PageIter cur=pages.begin(); cur != pages.end(); ++cur)
		if ( (*cur).sName == sVal)
			return (*cur).page;

	return NULL;
}
	
int FormGeneralPreferences::Fill(Event*)
{
	if (curPage)
		curPage->StoreData();
	if (tree->fValid() && fInitial)
	{
		for(PageIter cur=pages.begin(); cur != pages.end(); ++cur)
		{
			if ( (*cur).sName != "")
			{
				tree->Add((*cur).sName,0);
			}
		}
		ftoMpl->AddString("Open");
		ftoMpl->AddString("Colorcomp");
		ftoMpl->AddString("Slideshow");
		ftoFlt->AddString("Open");
		ftoFlt->AddString("Filter");
		ftoStp->AddString("Anaglyph");
		ftoStp->AddString("Stereoscope");
		ftoFlt->SelectItem(sFltAction);
		ftoMpl->SelectItem(sMplAction);
		ftoStp->SelectItem(sStpAction);

		fInitial = false;
		return 1;
  }
	for(PageIter cur3=pages.begin(); cur3 != pages.end(); ++cur3)
		(*cur3).page->Hide();

	String sVal = tree->sBranchValue();
	curPage = GetPage(sVal);
	if (curPage)
		curPage->Show();

	return 1;
}

void SetRegEntry(const pair<String, String> &entry)
{
	IlwisSettings settings("DefaultSettings");
	String sSystemDir = IlwWinApp()->Context()->sStdDir();
	FileName fn(entry.first, ".dom");
	fn.Dir(sSystemDir);
	String sDefaultRpr;
	ObjectInfo::ReadElement("Domain", "Representation", fn, sDefaultRpr);
	FileName fnDef(sDefaultRpr);
	fnDef.Dir(sSystemDir);
	FileName fnRpr = FileName(entry.second);
	if ( !(fCIStrEqual(fnRpr.sFile, fnDef.sFile)) && fnRpr.fExist())
		settings.SetValue(entry.first, entry.second);
	else
		settings.DeleteValue(entry.first);
}

int FormGeneralPreferences::exec()
{
	FormWithDest::exec();
	IlwisSettings settings("DefaultSettings");

	// font pages
	IlwWinApp()->StandardFonts[IlwisWinApp::sfFORM]->Detach();
	IlwWinApp()->StandardFonts[IlwisWinApp::sfFORM]->CreateFontIndirect(&lfForm);
	IlwWinApp()->StandardFonts[IlwisWinApp::sfWindowMedium]->Detach();
	IlwWinApp()->StandardFonts[IlwisWinApp::sfWindowMedium]->CreateFontIndirect(&lfGeneral);
	IlwWinApp()->StandardFonts[IlwisWinApp::sfTABLE]->Detach();
	IlwWinApp()->StandardFonts[IlwisWinApp::sfTABLE]->CreateFontIndirect(&lfTable);
	IlwWinApp()->StandardFonts[IlwisWinApp::sfGRAPH]->Detach();	
	IlwWinApp()->StandardFonts[IlwisWinApp::sfGRAPH]->CreateFontIndirect(&lfGraph);

	settings.SetValue("OperationsMenu",useAltLayout);

    //Tranquilizers
	settings.SetValue("OldTranquilizers", fOldTranquilizers);
	IlwWinApp()->SetTranquilizerStyle(fOldTranquilizers);

	// mouse wheel
	settings.SetValue("PanWheel", fPanWheel);
	
	//display
	String sOptions;
	for(vector<long>::iterator cur = dispOptions.begin(); cur != dispOptions.end(); ++cur)
		sOptions += ( sOptions == "" ? String("%d", (*cur)) :  String(",%d", (*cur)));

	// dirs
	settings.SetValue("ShowDisplayOptions", sOptions);
	settings.SetValue("CreatePyrWhenFirstDisplayed", fPyrCreateFirstDisplay);
	settings.SetValue("ToolDir", sToolDir);
	settings.SetValue("ScriptDirs", sScriptDirs);
	settings.SetValue("LogDir",sLogDir);
	settings.SetValue("DefaultStartUpDir", sDefaultStartUpDir);
	settings.SetValue("WarnNotUpToDate", fWarnNotUpToDate);
	settings.SetValue("ShowRasterDefForm", fShowRasterDef);
	settings.SetValue("HistoryEntries", iHistoryEntries);
	settings.SetValue("LanguageExtension",language);
	//Rpr
	for_each(defRpr.begin(), defRpr.end(), SetRegEntry);
	settings.SetValue("YesColor", clrYes);
	settings.SetValue("NoColor", clrNo);
	settings.SetValue("SingleColorPoints", clrSinglePnt);
	settings.SetValue("SingleColorSegments", clrSingleSeg);
	settings.SetValue("SingleColorPolygons", clrSinglePol);
	// double click action
	settings.SetValue("MapListDblClkAction", sMplAction);
	settings.SetValue("FilterDblClkAction", sFltAction);
	settings.SetValue("StereoPairDblClkAction", sStpAction);
	settings.SetValue("ShowAsTableCL", fShowAsTableCL);
	settings.SetValue("SoftwareRendering", fSoftwareRendering);
	const_cast<ActionPairList *>(IlwWinApp()->apl())->ChangeActionPair(".mpl", "", "", sMplAction, "");
	const_cast<ActionPairList *>(IlwWinApp()->apl())->ChangeActionPair(".fil", "", "", sFltAction, "");
	const_cast<ActionPairList *>(IlwWinApp()->apl())->ChangeActionPair(".stp", "", "", sStpAction, "");

	//sizes
	IlwisSettings settings2("DefaultSettings\\DataObjectPos");
	for(int i=0; i<mapPos.size(); ++i)
	{
		settings2.SetValue(String("X%d", i), mapPos[i].rcMin.Col);
		settings2.SetValue(String("Y%d", i), mapPos[i].rcMin.Row);
		settings2.SetValue(String("XSize%d", i), mapPos[i].rcMax.Col);
		settings2.SetValue(String("YSize%d", i), mapPos[i].rcMax.Row);
	}

	IlwisSettings settings3("Map Window\\Segment Editor");

	settings3.SetValue("Snap Pixels", iSnapPixels);
	settings3.SetValue("Normal Color", colNorm);
	settings3.SetValue("Retouch Color", colRetouch);
	settings3.SetValue("Deleted Color", colDeleted);
	settings3.SetValue("Find Undef Color", colFindUndef);

	settings.SetValue("Cursor Color", colDig);
	settings.SetValue("Auto Snap", fAutoSnap);
	settings.SetValue("Show Nodes", fShowNodes);

	IlwisSettings settings4("Map Window\\Point Editor");

	settings4.SetValue("Show Text", fText);
	settings4.SetValue("Text Color", colText);
	settings4.SetValue("Background Color", colBackText);
	settings4.SetValue("Symbol Type", iSmb);
	settings4.SetValue("Symbol Size", iSmbSize);
	settings4.SetValue("Pen Width", iSmbWidth);
	settings4.SetValue("Pen Color", colBound);
	settings4.SetValue("Fill Color", colFill);
	if ( debugLog) {
		getEngine()->setDebugMode(debugLog);
	}

	return 1;
}
