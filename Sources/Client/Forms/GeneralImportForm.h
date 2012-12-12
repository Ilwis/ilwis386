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

#pragma once;

class TreeSelector;

typedef void (*GetImportOptionForms)(CWnd *wnd,vector<ImportDriver>& drivers);

class FieldImportPage : public FieldGroup
{
	public:
		FieldImportPage(FormEntry *entry);
		void create();
		virtual void Fill(){};
};

class FormExtraImportOptions : public FormWithDest {
public:
	FormExtraImportOptions(CWnd* parent, const String& title) : FormWithDest(parent, title) {}
	virtual String sGetExtraOptionsPart(const String& currentExp) = 0;
};

class GeneralImportForm : public FormWithDest {
public:
	GeneralImportForm(CWnd* parent);

	struct Page
		{
			Page(const String& sN, FieldImportPage *p) : sName(sN), page(p) {}

			FieldImportPage *page;
			String    sName;
		};

private:

	TreeSelector *tree;
	FieldImportPage    *curPage;
	vector<Page> pages;
	vector<ImportDriver> drivers;
	ImportFormat currentFormat;
	ImportDriver currentDriver;
	bool fInitial;
	String sInput,sOutput;
	bool fUseAs;
	bool fMatch;
	Array<ImportItem> oldStyleImports;
	String extraOptions;
	bool isWfs;

	FieldString *fsInput;
	FieldString *fsOutput;
	FieldString *fsDriverDetails;
	CheckBox *cb;
	CheckBox *cbMatch;
	PushButton *pbMoreOptions;
	String driverMessage;

	int Fill(Event*);
	int moreOptions(Event *);
	FieldImportPage *GetPage(const String& sVal);
	FormEntry *CheckData();
	long getTypeMask(const String& type);
	void addModule(const FileName& fnModule,vector<GetImportOptionForms>& options);
	void addModules();
	//void addFolder(const String& dir,vector<GetImportOptionForms>& options);
	void AddOldStyleIlwisImports();
	void readImportDef();
	ImportFormat getFormat(const String& sVal);
	int ObjectSelection(Event *ev);
	int OutputSelection(Event *ev);
	int SetDefaultOutputName(Event *dv);
	int exec();
	FileName SetExtension(const FileName& fn);
	int similarNames(Event *ev);
	String layer;
};


typedef vector<GeneralImportForm::Page>::iterator ImportPageIter;

