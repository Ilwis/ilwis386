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
  FormWithDest(IlwWinApp()->GetMainWnd(), "Preferences"),
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

  tree = new TreeSelector(root,1);
	tree->SetCallBack((NotifyProc)&FormGeneralPreferences::Fill);
	pages.push_back(FormGeneralPreferences::Page(SPFMainWindow, new FieldPage(root)));
	pages.push_back(FormGeneralPreferences::Page(SPFMainWindowSP, new FieldPage(root)));
	pages.push_back(FormGeneralPreferences::Page(SPFMainWindowMenu, new FieldPage(root)));
	pages.push_back(FormGeneralPreferences::Page(SPFMapWindow, new FieldPage(root)));
	pages.push_back(FormGeneralPreferences::Page(SPFMapWindowDisPlayOpt, new FieldPage(root)));
	pages.push_back(FormGeneralPreferences::Page(SPFMapWindowDisplayOptRM, new FieldPage(root)));
	pages.push_back(FormGeneralPreferences::Page(SPFMapWindowDisplayOptSM, new FieldPage(root)));
	pages.push_back(FormGeneralPreferences::Page(SPFMapWindowDisplayOptPM, new FieldPage(root)));
	pages.push_back(FormGeneralPreferences::Page(SPFMapWindowDisplayOptPoM, new FieldPage(root)));
	pages.push_back(FormGeneralPreferences::Page(SPFMapWindowDefRpr, new FieldPage(root)));
	pages.push_back(FormGeneralPreferences::Page(SPFMapWindowSizePos, new FieldPage(root)));
	pages.push_back(FormGeneralPreferences::Page(SPFMapWindowEdit, new FieldPage(root)));
	pages.push_back(FormGeneralPreferences::Page(SPFMapWindowEditPnt, new FieldPage(root)));
	pages.push_back(FormGeneralPreferences::Page(SPFMapWindowEditSeg, new FieldPage(root)));
	pages.push_back(FormGeneralPreferences::Page(SPFTableWindow, new FieldPage(root)));
	pages.push_back(FormGeneralPreferences::Page(SPFTableWindowGraph, new FieldPage(root)));
	pages.push_back(FormGeneralPreferences::Page(SPFFont, new FieldPage(root)));
	pages.push_back(FormGeneralPreferences::Page(SPFFontGeneral, new FieldPage(root)));
	pages.push_back(FormGeneralPreferences::Page(SPFFontDialog, new FieldPage(root)));
	pages.push_back(FormGeneralPreferences::Page(SPFFontTable, new FieldPage(root)));
	pages.push_back(FormGeneralPreferences::Page(SPFFontGraphs, new FieldPage(root)));
	pages.push_back(FormGeneralPreferences::Page(SPFGeneral, new FieldPage(root)));
	pages.push_back(FormGeneralPreferences::Page(SPFDirectories, new FieldPage(root)));
	pages.push_back(FormGeneralPreferences::Page(SPFAdvanced, new FieldPage(root)));	
	for(PageIter cur=pages.begin(); cur != pages.end(); ++cur)
		(*cur).page->Align(tree, AL_AFTER);



	MainWindowPage(settings);

	MapWindowPage(settings);

	DirectoryPage(settings);

  FontPage(settings);

	TableWindowPage(settings);

   GeneralPage(settings);

	AdvancedPage();

  SetMenHelpTopic(htpPreferences);
	create();
}
FormGeneralPreferences::~FormGeneralPreferences()
{
}

void FormGeneralPreferences::GeneralPage(IlwisSettings& settings)
{
	FieldPage *page = GetPage(SPFGeneral);
	fOldTranquilizers = settings.fValue("OldTranquilizers", false);
	language = settings.sValue("LanguageExtension",".eng");

	cbOldTranquilizers = new CheckBox(page, SPFCUseOldStyleTranquilizers, &fOldTranquilizers);
	new FieldString(page,SPFCLanguageExtension,&language);
}

void FormGeneralPreferences::FontPage(IlwisSettings& settings)
{
	FieldPage *page = GetPage(SPFFont);
	new StaticText(page, SPFCRemContSubEntries);

	page = GetPage(SPFFontGeneral);
	FieldGroup *grp = new FieldGroup(page);
	new StaticText(grp, SPFCGeneralFont, true);
	StaticText* st = new StaticText(grp, SPFCGenFntInfo1);
	st->psn->SetBound(0,0,0,0);
	new StaticText(grp, SPFCGenFntInfo2);
	grp->SetBevelStyle(FormEntry::bsLOWERED);

	new FieldBlank(page);

	CFont * fnt = IlwWinApp()->GetFont(IlwisWinApp::sfWindowMedium);
	fnt->GetLogFont(&lfGeneral);
	FieldLogFont *fl = new FieldLogFont(page, &lfGeneral);
	fl->SetIndependentPos();

	page = GetPage(SPFFontDialog);
	grp = new FieldGroup(page);
	new StaticText(grp,SPFCDialogFont, true);
	st = new StaticText(grp, SPFCFrmFntInfo1);
//	st->psn->SetBound(0,0,0,0);
//	new StaticText(grp, SPFCFrmFntInfo2);
	grp->SetBevelStyle(FormEntry::bsLOWERED);

	new FieldBlank(page);

	fnt = IlwWinApp()->GetFont(IlwisWinApp::sfFORM);
	fnt->GetLogFont(&lfForm);
  fl = new FieldLogFont(page, &lfForm, FieldLogFont::faVECTOR);
	fl->SetIndependentPos();

	page = GetPage(SPFFontTable);

	grp = new FieldGroup(page);
	new StaticText(grp, SPFCTableFont, true);
	new StaticText(grp, SPFCTblFntInfo);
	grp->SetBevelStyle(FormEntry::bsLOWERED);

	new FieldBlank(page);
	fnt = IlwWinApp()->GetFont(IlwisWinApp::sfTABLE);
	fnt->GetLogFont(&lfTable);
  fl = new FieldLogFont(page, &lfTable, FieldLogFont::faFIXED);
	fl->SetIndependentPos();

	page = GetPage(SPFFontGraphs);

	grp = new FieldGroup(page);
	new StaticText(grp, SPFCGraphFont, true);
	new StaticText(grp, SPFCGrpFntInfo);
	grp->SetBevelStyle(FormEntry::bsLOWERED);

	new FieldBlank(page);

	fnt = IlwWinApp()->GetFont(IlwisWinApp::sfGRAPH);
	fnt->GetLogFont(&lfGraph);
	fl = new FieldLogFont(page, &lfGraph, FieldLogFont::faTRUETYPE);
	fl->SetIndependentPos();
}

void FormGeneralPreferences::MainWindowPage(IlwisSettings& settings)
{
	FieldPage *page = GetPage(SPFMainWindow);
	StaticText* st = new StaticText(page, SPFCCommandLine, true);
	st->SetIndependentPos();
	iHistoryEntries = settings.iValue("HistoryEntries", 8);
	new FieldInt(page, SPFCHistoryEntries, &iHistoryEntries, ValueRange(0, 30), true);

	new FieldBlank(page);
	st = new StaticText(page, SPFCGeneral, true);
	st->psn->SetBound(0,0,0,0);
	st->SetIndependentPos();

	fWarnNotUpToDate = settings.fValue("WarnNotUpToDate", false);
	fShowRasterDef = settings.fValue("ShowRasterDefForm", true);
	CheckBox* cb = new CheckBox(page, SPFUiWarnNotUpToDate, &fWarnNotUpToDate);
	cb->SetIndependentPos();
	cb = new CheckBox(page, SPFCShowRasDefFrm, &fShowRasterDef);
	cb->SetIndependentPos();

	new FieldBlank(page);
	st = new StaticText(page, SPFCDefActions, true);
	st->psn->SetBound(0,0,0,0);
	st->SetIndependentPos();
	sMplAction = settings.sValue("MapListDblClkAction", "Open");
	st = new StaticText(page, SPFCFilter);
	sFltAction = settings.sValue("FilterDblClkAction", "Open");
	ftoFlt = new FieldOneSelectTextOnly(page, &sFltAction);
	sStpAction = settings.sValue("StereoPairDblClkAction", "Stereoscope");
	
	ftoFlt->Align(st, AL_AFTER);
	st = new StaticText(page, SPFCMapList);
	ftoMpl = new FieldOneSelectTextOnly(page, &sMplAction);
	ftoMpl->Align(st, AL_AFTER);
	st = new StaticText(page, SPFCStereoPair);
	ftoStp = new FieldOneSelectTextOnly(page, &sStpAction);
	ftoStp->Align(st, AL_AFTER);

	page = GetPage(SPFMainWindowSP);
	st = new StaticText(page, SPFCMainWindowSP, true);
	st->SetIndependentPos();
	MWX = new FieldInt(page, SPFCXPos, &(mapPos[mpMAIN].rcMin.Col), ValueRange(1, 2000), false, true);
	MWY = new FieldInt(page, SPFCYPos, &(mapPos[mpMAIN].rcMin.Row), ValueRange(1, 2000), false, true);
	MWW = new FieldInt(page, SPFCWidth, &(mapPos[mpMAIN].rcMax.Col), ValueRange(100, 2000), false, true);
	MWH = new FieldInt(page, SPFCHeight, &(mapPos[mpMAIN].rcMax.Row), ValueRange(100, 2000), false, true);

	new PushButton(page, SPFUiReset, (NotifyProc)&FormGeneralPreferences::ResetButtonMW);

	page = GetPage(SPFMainWindowMenu);
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
	FieldPage *page = GetPage(SPFTableWindow);

	fShowAsTableCL = settings.fValue("ShowAsTableCL", false);
	new StaticText(page, SPFCCommandLine, true);
  new CheckBox(page, SPFCShowAsTableComLine, &fShowAsTableCL);
	new FieldBlank(page);

	new StaticText(page, SPFCTableWndPos, true);
	FieldGroup* fg = new FieldGroup(page);
	TWX = new FieldInt(fg, SPFCXPos, &(mapPos[mpTABLE].rcMin.Col), ValueRange(1, 2000), false, true);
	TWY = new FieldInt(fg, SPFCYPos, &(mapPos[mpTABLE].rcMin.Row), ValueRange(1, 2000), false, true);
	TWW = new FieldInt(fg, SPFCWidth, &(mapPos[mpTABLE].rcMax.Col), ValueRange(100, 2000), false, true);
	TWH = new FieldInt(fg, SPFCHeight, &(mapPos[mpTABLE].rcMax.Row), ValueRange(100, 2000), false, true);
	fg->SetIndependentPos();
	
	new PushButton(page, SPFUiReset, (NotifyProc)&FormGeneralPreferences::ResetButtonTW);

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
	FieldPage *page = GetPage(SPFTableWindowGraph);

	StaticText* st = new StaticText(page, SPFCGraphWindowSizePos, true);
	st->SetIndependentPos();
	GWX = new FieldInt(page, SPFCXPos, &(mapPos[mpGRAPH].rcMin.Col), ValueRange(1, 2000), false, true);
	GWY = new FieldInt(page, SPFCYPos, &(mapPos[mpGRAPH].rcMin.Row), ValueRange(1, 2000), false, true);
	GWW = new FieldInt(page, SPFCWidth, &(mapPos[mpGRAPH].rcMax.Col), ValueRange(100, 2000), false, true);
	GWH = new FieldInt(page, SPFCHeight, &(mapPos[mpGRAPH].rcMax.Row), ValueRange(100, 2000), false, true);

	new PushButton(page, SPFUiReset, (NotifyProc)&FormGeneralPreferences::ResetButtonGW);	

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
	FieldPage *pg = GetPage(SPFMapWindow);
	new StaticText(pg, SPFCRemContSubEntries);

	FieldPage *page = GetPage(SPFMapWindowSizePos);

	StaticText* st = new StaticText(page, SPFCMapWindowSizePos, true);
	st->SetIndependentPos();
	MpWX = new FieldInt(page, SPFCXPos , &(mapPos[mpMAPS].rcMin.Col), ValueRange(1, 2000), false, true);
	MpWY = new FieldInt(page, SPFCYPos, &(mapPos[mpMAPS].rcMin.Row), ValueRange(1, 2000), false, true);
	MpWW = new FieldInt(page, SPFCWidth, &(mapPos[mpMAPS].rcMax.Col), ValueRange(100, 2000), false, true);
	MpWH = new FieldInt(page, SPFCHeight, &(mapPos[mpMAPS].rcMax.Row), ValueRange(100, 2000), false, true);

	new PushButton(page, SPFUiReset, (NotifyProc)&FormGeneralPreferences::ResetButtonMpW);		

	DefaultRprPage(settings);

	pg = GetPage(SPFMapWindowEdit);
	new StaticText(pg, SPFCRemContSubEntries);
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
	FieldPage *page = GetPage(SPFMapWindowEditPnt);
	// to ensure that on show time the fill color field is only shoen when needed
  page->SetCallBack((NotifyProc)&FormGeneralPreferences::FieldSymbolCallBack);
	StaticText* st = new StaticText(page, SPFCPointEditor, true);
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

  CheckBox* cbText = new CheckBox (page, SEDUiShowText, &fText);
  FieldGroup* fg = new FieldGroup(cbText);
  fg->Align(cbText,AL_UNDER);
  new FieldColor(fg, SEDUiTextColor, &colText);
  new FieldFillColor(fg, SEDUiBackground, &colBackText);
  new FieldBlank(page);
  fsmb = new FieldSymbol(page, SEDUiSmbType, &iSmb, &hIcon);
  fsmb->SetCallBack((NotifyProc)&FormGeneralPreferences::FieldSymbolCallBack);
  new FieldInt(page, SEDUiSmbSize, &iSmbSize, ValueRange(1L,100L), true);
  new FieldInt(page, SEDUiPenWidth, &iSmbWidth, ValueRange(1L,100L), true);
  new FieldColor(page, SEDUiColor, &colBound);
  ffc = new FieldFillColor(page, SEDUiFillColor, &colFill);
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

	FieldPage *page = GetPage(SPFMapWindowEditSeg);
	StaticText* st = new StaticText(page, SPFCSegmentEditor, true);
	st->SetIndependentPos();
  new FieldColor(page, SEDUiCursorColor, &colDig);
  new CheckBox(page, SEDUiAutoSnap, &fAutoSnap);
  new CheckBox(page, SEDUiShowNodes,&fShowNodes);
	new FieldInt(page, SEDUiSnapPixels, &iSnapPixels, ValueRange(2,30), true);
	new FieldColor(page, SEDUiNormalColor, &colNorm);
	new FieldColor(page, SEDUiRetoucheColor, &colRetouch);
	new FieldColor(page, SEDUiDeletedColor, &colDeleted);
	new FieldColor(page, SEDUiFindUndefColor, &colFindUndef);
}

void FormGeneralPreferences::GetSystemRpr(IlwisSettings& settings)
{
	String sSystemDir = IlwWinApp()->ilwapp->sStdDir();
	CFileFind finder;
	BOOL fFound  = finder.FindFile(String("%S\\*.dom", sSystemDir).scVal());
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
	FieldPage *page = GetPage(SPFMapWindowDefRpr);
	GetSystemRpr(settings);
	rprList = new FieldRprListView(page, defRpr);
}

void FormGeneralPreferences::DirectoryPage(IlwisSettings& settings)
{
	FieldPage *page = GetPage(SPFDirectories);

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

	FormEntry *fe = new FieldBrowseDir(page, SPFCToolDir, SPFCToolDir, &sToolDir);
	fe->SetWidth(100);
	fe = new FieldBrowseDir(page, SPFCScriptDir, SPFCScriptDir, &sScriptDirs);
	fe->SetWidth(100);
	fe = new FieldBrowseDir(page, SPFCLogDir, SPFCLogDir, &sLogDir);
	fe->SetWidth(100);
	fe = new FieldBrowseDir(page, SPFCStartDir, SPFCStartDir, &sDefaultStartUpDir);
	fe->SetWidth(100);
}


void FormGeneralPreferences::AdvancedPage()
{
	FieldPage *page = GetPage(SPFAdvanced);	

  FieldGroup *grp = new FieldGroup(page);
	new StaticText(grp, SPFCRestoreDefaults, true);
	new StaticText(grp, SPFCRemoveRegKeys1);
	new StaticText(grp, SPFCRemoveRegKeys2);
	grp->SetBevelStyle(FormEntry::bsLOWERED);	
	new FieldBlank(page, 0.25);	
	new PushButton(page, SPFUiRestoreDefaults, (NotifyProc)&FormGeneralPreferences::RestoreDefaults);
	new CheckBox(page,SPFUiDebugMode,&debugLog);
}

int FormGeneralPreferences::RestoreDefaults(Event *)
{
	if ( IlwisSettings::fDeletePossible() )
		IlwWinApp()->fRemoveUserRegistry = true;
	else
		MessageBox(SPFCNoDeletePossible.scVal(), SPFCError.scVal(), MB_OK | MB_ICONEXCLAMATION);
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

	FieldPage *page = GetPage(SPFMapWindowDisPlayOpt);
	dispOptions[IlwisWinApp::dosALL] = false;
//	dispChecks[IlwisWinApp::dosALL] = new CheckBox(page, SPFCDispOptAll, &dispOptions[IlwisWinApp::dosALL]);

	new FieldBlank(page);
	new StaticText(page, SPFCBoolColors, true);
	FieldGroup* fg = new FieldGroup(page);
	clrYes = Color(0,176,20);
	clrYes = settings.clrGetRGBColor("YesColor", clrYes);
	new FieldColor(fg, SPFCYes, &clrYes);
	clrNo = Color(168,168,168);
	clrNo = settings.clrGetRGBColor("NoColor", clrNo);
	new FieldColor(fg, SPFCNo, &clrNo);
	fg->SetIndependentPos();

	page = GetPage(SPFMapWindowDisplayOptRM);
	new StaticText(page, SPFCShowDisOp, true);
	dispChecks[IlwisWinApp::dosRAS]			= new CheckBox(page, SPFCAllRas, &dispOptions[IlwisWinApp::dosRAS]);
	dispChecks[IlwisWinApp::dosRASIMG]		= new CheckBox(page, SPFCDomImg, &dispOptions[IlwisWinApp::dosRASIMG]);
	dispChecks[IlwisWinApp::dosRASIMG]->psn->iBndLeft += 15;
	dispChecks[IlwisWinApp::dosRASVAL]		= new CheckBox(page, SPFCDomVal, &dispOptions[IlwisWinApp::dosRASVAL]);
	dispChecks[IlwisWinApp::dosRASSORT]	= new CheckBox(page, SPFCDomSort, &dispOptions[IlwisWinApp::dosRASSORT]);
	dispChecks[IlwisWinApp::dosRASBOOL]	= new CheckBox(page, SPFCDomBool, &dispOptions[IlwisWinApp::dosRASBOOL]);
	dispChecks[IlwisWinApp::dosRASOTHER] = new CheckBox(page, SPFCDomOther, &dispOptions[IlwisWinApp::dosRASOTHER]);

	dispChecks[IlwisWinApp::dosRAS]->SetCallBack((NotifyProc)&FormGeneralPreferences::ShowMapDispOptionsChecks);
	for ( int i=IlwisWinApp::dosRASIMG; i<= IlwisWinApp::dosRASOTHER; ++i)
		dispChecks[i]->SetCallBack((NotifyProc)&FormGeneralPreferences::ShowDomainDispOptionsChecks);

	StaticText * stPyramidFiles = new StaticText(page, SPFPyramidFiles, true);
	stPyramidFiles->psn->iBndLeft -= 15;
	fPyrCreateFirstDisplay = settings.fValue("CreatePyrWhenFirstDisplayed", false);
	new CheckBox(page, SPFPyrsWhenFirstDisp, &fPyrCreateFirstDisplay);	

	page = GetPage(SPFMapWindowDisplayOptSM);
	new StaticText(page, SPFCShowDisOp, true);
	dispChecks[IlwisWinApp::dosSEG] = new CheckBox(page, SPFCAllSeg, &dispOptions[IlwisWinApp::dosSEG]);
	dispChecks[IlwisWinApp::dosSEGVAL]		= new CheckBox(page, SPFCDomVal, &dispOptions[IlwisWinApp::dosSEGVAL]);
	dispChecks[IlwisWinApp::dosSEGVAL]->psn->iBndLeft += 15;
	dispChecks[IlwisWinApp::dosSEGSORT]	= new CheckBox(page, SPFCDomSort, &dispOptions[IlwisWinApp::dosSEGSORT]);
	dispChecks[IlwisWinApp::dosSEGBOOL]	= new CheckBox(page, SPFCDomBool, &dispOptions[IlwisWinApp::dosSEGBOOL]);
	dispChecks[IlwisWinApp::dosSEGOTHER] = new CheckBox(page, SPFCDomOther, &dispOptions[IlwisWinApp::dosSEGOTHER]);


	page = GetPage(SPFMapWindowDisplayOptPM);
	new StaticText(page, SPFCShowDisOp, true);
	dispChecks[IlwisWinApp::dosPOL] = new CheckBox(page, SPFCAllPol, &dispOptions[IlwisWinApp::dosPOL]);
	dispChecks[IlwisWinApp::dosPOLVAL]		= new CheckBox(page, SPFCDomVal, &dispOptions[IlwisWinApp::dosPOLVAL]);
	dispChecks[IlwisWinApp::dosPOLVAL]->psn->iBndLeft += 15;
	dispChecks[IlwisWinApp::dosPOLSORT]	= new CheckBox(page, SPFCDomSort, &dispOptions[IlwisWinApp::dosPOLSORT]);
	dispChecks[IlwisWinApp::dosPOLBOOL]	= new CheckBox(page, SPFCDomBool, &dispOptions[IlwisWinApp::dosPOLBOOL]);
	dispChecks[IlwisWinApp::dosPOLOTHER] = new CheckBox(page, SPFCDomOther, &dispOptions[IlwisWinApp::dosPOLOTHER]);

	page = GetPage(SPFMapWindowDisplayOptPoM);
	new StaticText(page, SPFCShowDisOp, true);
	dispChecks[IlwisWinApp::dosPNT] = new CheckBox(page, SPFCAllPoint, &dispOptions[IlwisWinApp::dosPNT]);
	dispChecks[IlwisWinApp::dosPNTVAL]		= new CheckBox(page, SPFCDomVal, &dispOptions[IlwisWinApp::dosPNTVAL]);
	dispChecks[IlwisWinApp::dosPNTVAL]->psn->iBndLeft += 15;
	dispChecks[IlwisWinApp::dosPNTSORT]	= new CheckBox(page, SPFCDomSort, &dispOptions[IlwisWinApp::dosPNTSORT]);
	dispChecks[IlwisWinApp::dosPNTBOOL]	= new CheckBox(page, SPFCDomBool, &dispOptions[IlwisWinApp::dosPNTBOOL]);
	dispChecks[IlwisWinApp::dosPNTOTHER] = new CheckBox(page, SPFCDomOther, &dispOptions[IlwisWinApp::dosPNTOTHER]);

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
	String sRpr = entry.second;
	if ( !(fCIStrEqual(sRpr, fnDef.sFullPath())) )
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
	// double click action
	settings.SetValue("MapListDblClkAction", sMplAction);
	settings.SetValue("FilterDblClkAction", sFltAction);
	settings.SetValue("StereoPairDblClkAction", sStpAction);
	settings.SetValue("ShowAsTableCL", fShowAsTableCL);
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
