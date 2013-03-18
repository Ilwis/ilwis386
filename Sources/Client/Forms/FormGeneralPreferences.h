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
#ifndef FORMGENERALPREFERENCES_H
#define FORMGENERALPREFERENCES_H

class TreeSelector;
class FieldOneSelectTextOnly;
class FieldSymbol;
class FieldColor;
class FieldFillColor;
class FieldRprListView;
class FormGeneralPreferences;

class FieldPage : public FieldGroup
{
	public:
		FieldPage(FormEntry *entry);
		void create();

};

class FormGeneralPreferences : public FormWithDest
{
  public:
		enum DataObject{mpMAPS, mpTABLE, mpGRAPH, mpMAIN};

    FormGeneralPreferences();
		~FormGeneralPreferences();

	public:
		struct Page
		{
			Page(const String& sN, FieldPage *p) : sName(sN), page(p) {}

			FieldPage *page;
			String    sName;
		};

		int                      exec();
		int						 SaveOperationsLayout(Event *);
		int						 Fill(Event*);
		int                      ShowMapDispOptionsChecks(Event*);
		int                      ShowDomainDispOptionsChecks(Event*);
		void										 SetDomainCheck(int iStart, int iEnd, int iType);
		void                     SetMapCheck(int iStart, int iEnd, int iType);
		void                     DisplayOptionsPages(IlwisSettings& settings);
		int                      BrowseForDir(Event *);
		void                     DirectoryPage(IlwisSettings& settings);
		void                     FontPage(IlwisSettings& settings);
		void                     MapWindowPage(IlwisSettings& settings);
		void                     MainWindowPage(IlwisSettings& settings);
		void                     DefaultRprPage(IlwisSettings& settings);
		void										 GraphWindowPage(IlwisSettings& settings);
		void                     GeneralPage(IlwisSettings& settings);
		void                     PointEdMapPage();
		void                     SegmentEdMapPage();
		void                     AdvancedPage(IlwisSettings& settings);
		void                     TableWindowPage(IlwisSettings& settings);
		FieldPage*               GetPage(const String& sName);
		void                     GetSystemRpr(IlwisSettings& settings);
		int                      FieldSymbolCallBack(Event*);
		int                      ResetButtonMW(Event *);
		int                      ResetButtonTW(Event *);
		int                      ResetButtonGW(Event *);
		int                      ResetButtonMpW(Event *);	
		int                      RestoreDefaults(Event *);
		TreeSelector			       *tree;
		vector<Page>             pages;
		bool							       fInitial;
		FieldPage                *curPage;
		FieldOneSelectTextOnly   *ftoMpl;
		FieldOneSelectTextOnly   *ftoFlt;
		FieldOneSelectTextOnly   *ftoStp;
		FieldSymbol              *fsmb;
		FieldFillColor           *ffc;
		FieldRprListView				 *rprList;
		FieldInt                 *MWX, *MWY, *MWW, *MWH;
		FieldInt                 *MpWX, *MpWY, *MpWW, *MpWH;
		FieldInt                 *TWX, *TWY, *TWW, *TWH;
		FieldInt                 *GWX, *GWY, *GWW, *GWH;
		CheckBox				 *cbOldTranquilizers;

// settings vars;
		LOGFONT                  lfForm, lfGeneral, lfTable, lfGraph;
		Array<long>              dispOptions;
		Array<CheckBox *>        dispChecks;
		String                   sToolDir;
		String                   sScriptDirs;
		String                   sLogDir;
		String                   sDefaultStartUpDir;
		bool                     fWarnNotUpToDate, fShowRasterDef;
		bool                     fPyrCreateFirstDisplay;
		vector< MinMax>          mapPos;
		Color                    clrYes, clrNo, clrUndef;
		int                      iHistoryEntries;
		String                   sMplAction;
		String                   sFltAction;
		String									 sStpAction;
		bool                     fAutoSnap, fShowNodes;
		int                      iSnapPixels;
		Color                    colNorm, colRetouch, colDeleted, colFindUndef;
		bool                     fText;
		Color                    colBackText, colFill, colBound, colText;
		Color                    colDig;
		HICON                    hIcon;
		long                     iSmbSize, iSmbWidth;
		long                     iSmb;
		map<String, String>      defRpr;
		bool                     fShowAsTableCL;
		bool                     fOldTranquilizers;
		String					 language;
		bool					 useAltLayout;
		bool					 debugLog;
		bool					 fSoftwareRendering;
};

typedef vector<FormGeneralPreferences::Page>::iterator PageIter;
typedef std::map<String, String>::iterator rprIter;

#endif
