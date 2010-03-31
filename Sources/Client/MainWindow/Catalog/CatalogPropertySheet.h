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
#ifndef CATALOGPROPERTYSHEET_H
#define CATALOGPROPERTYSHEET_H



#if !defined(AFX_FORMBASEPROPERTYPAGE_H__326B98B4_6CDC_11D3_B7E1_00A0C9D5342F__INCLUDED_)
#include "Client\FormElements\FormBasePropertyPage.h"
#endif

#undef IMPEXP
#ifdef ILWISCLIENT
#define IMPEXP __declspec(dllexport)
#else
#define IMPEXP __declspec(dllimport)
#endif

class FieldOneSelectTextOnly;


class CatalogPropertySheet : public CPropertySheet
{
public:
	CatalogPropertySheet(Catalog *cat);

	BOOL OnInitDialog();

	Catalog *catalog;
};

class SimpleQueryPropPage : public FormBasePropertyPage
{
public:
	SimpleQueryPropPage(CatalogPropertySheet& sh, vector<NameExt>&);
	FormEntry* CheckData();

private:
	CatalogPropertySheet& sheet;
	int exec();
	bool fAll;
	bool fHideCollCont;
	String sExtensions;
};

class VisibleColumnsPropPage : public FormBasePropertyPage
{
public:
	VisibleColumnsPropPage(Catalog *c);
	FormEntry* CheckData();	

private:
	int exec();
	int SetDefault(Event *)	;
	int RestoreDefault(Event *);	
	
	vector<bool> fVisibleColumns;
	bool fShowGrid;
	Catalog *cat;
};

class IMPEXP QueryPropPage : public FormBasePropertyPage
{
public:
	QueryPropPage(CatalogPropertySheet& sh, map<String, String>& mpQueries);
	FormEntry* CheckData();	

private:
	
  class NameForm : public FormWithDest
	{
		public:
			NameForm(CWnd *wnd, String& sVal) :
					FormWithDest(wnd, SMSTitleCatQueryName)
			{
				new FieldString(root, SMSUiQueryName, &sVal);
				create();
      }
			
	};

	CatalogPropertySheet& sheet;
	String sExpr;
	String sChoice;
	map<String, String>& mpQueries;
	FieldOneSelectTextOnly * fld;
	FieldStringMulti *fsExpr;
	PushButton *pbSave;
	PushButton *pbSaveAs;
	PushButton *pbDelete;
	int FillAll(Event *);
	int exec();

	int Replace(Event*);
	int Add(Event*);
	int Delete(Event*);
	int ChangeQuery(Event*);
	BOOL OnApply();
};

#endif
