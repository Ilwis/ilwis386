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
// GraphView.h: interface for the GraphView class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRAPHFORMS_H)
#define AFX_GRAPHFORMS_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Client\FormElements\formbase.h"
#include "Client\FormElements\FormBasePropertyPage.h"
#include "Client\FormElements\selector.h"
#include <stack>

#undef IMPEXP
#ifdef ILWISCLIENT
#define IMPEXP __declspec(dllexport)
#else
#define IMPEXP __declspec(dllimport)
#endif


class GraphAxis;
class CartesianGraphDrawer;
class RoseDiagramDrawer;
class GraphView;
class GraphLayer;
class GraphLayerSelector;


class GraphPropertyPage : public FormBasePropertyPage
{
public:
	 GraphPropertyPage() : FormBasePropertyPage("") {}
	 GraphPropertyPage(const String& sTitle) : FormBasePropertyPage(sTitle) {}
	 BOOL OnApply();
};

class GraphLayerManPage : public GraphPropertyPage
{
public:
	GraphLayerManPage() {} 
	GraphLayerManPage(GraphDrawer* grdrw); 
	~GraphLayerManPage();
  int Select(Event*);  
  int OnKeyDown( Event*);
  int Prop(Event*);
	void Fill();
private:
  int Add(Event*);
  int Remove(Event*);
  int MakeUpToDate(Event*);
	void PageAdded();
	bool fLegend;
  GraphLayerSelector* gls;
	GraphDrawer* gd;
  PushButton *pbAdd, *pbProp, *pbRemove;
  int exec();
};

class _export GraphLayerSelector: public BaseSelector
{
public:
  void StoreData();
  GraphLayerSelector(FormEntry* par, GraphLayerManPage*, GraphDrawer*);
  ~GraphLayerSelector();
  int idSelect();
  void Fill();
protected:
  void create();
  long DrawItem(DRAWITEMSTRUCT* dis);
private:
  int mouseButtonDown(Event* Evt);
  int mouseButtonUp(Event* Evt);
  bool fDrag;
  int idDrag;
  zCursor *cur, curArrow;
  String sName(int id);
  GraphLayerManPage* gmp;
	GraphDrawer* gd;
};



class GraphAxisPage : public GraphPropertyPage
{
public:
	GraphAxisPage(GraphAxis* ga, const String& sTitle); 
	virtual void Init();
	int exec();
	GraphAxis* gaxis;
	RangeReal rrMinMax;
	double rGridStep;
};

class RoseDiagramAxis;

class RoseDiagramAxisPage : public GraphAxisPage
{
public:
	RoseDiagramAxisPage(RoseDiagramAxis* rda, const String& sTitle); 
	virtual void Init();
	int exec();
	RoseDiagramAxis* rdaxis;
};


class FieldFillColor;
class FieldSymbol;
class Symbol;
class CartesianGraphLayer;

class GraphLayerOptionsForm : public GraphPropertyPage
{
public:
	GraphLayerOptionsForm(GraphLayer* gl); 
	~GraphLayerOptionsForm(); 
	int exec();
  int FieldSymbolCallBack(Event*);
  int Remove(Event*);
	FieldFillColor* ffc;
  FieldSymbol* fsmb;
	Symbol* smb;
	int iColor, iYAxis;
	GraphDrawer* gd;
	GraphLayer* gl;
};

class FormulaGraphLayer;

class FormulaGraphLayerOptionsForm : public GraphLayerOptionsForm
{
public:
	FormulaGraphLayerOptionsForm(FormulaGraphLayer* gl); 
	~FormulaGraphLayerOptionsForm(); 
	int exec();
	String sExpr;
	FormulaGraphLayer* fgl;
};

class FieldRegressionFunc;
class LsfGraphLayer;

class LsfGraphLayerOptionsForm : public GraphLayerOptionsForm
{
public:
	LsfGraphLayerOptionsForm(LsfGraphLayer* gl); 
	~LsfGraphLayerOptionsForm(); 
	int exec();
	int FitFuncCallBack(Event*);
	int iTerms;
	String* sFitType;
	LsfGraphLayer* lsfgl;
	FieldRegressionFunc* frf;
	StaticText* stRegr, *stFormula;
	FieldInt* fiTerms;
};

class SmvGraphLayer;

class SmvGraphLayerOptionsForm : public GraphLayerOptionsForm
{
public:
	SmvGraphLayerOptionsForm(SmvGraphLayer* gl); 
	~SmvGraphLayerOptionsForm(); 
	int exec();
	SemiVariogram smv;
	SmvGraphLayer* smvgl;
};

class GraphOptionsForm : public CPropertySheet
{
public:
	_export GraphOptionsForm(GraphDrawer* grd);
	virtual _export ~GraphOptionsForm();
	GraphDrawer* gd;
  virtual void Refill(int iActivePage=0);
	GraphLayerManPage* glmp;
	int iGraphPropPages;
protected:
	void AddPage(CPropertyPage *pPage);
private:
	stack<CPropertyPage*> spp;
};

class CartesianGraphOptionsForm : public GraphOptionsForm
{
  public:
		_export CartesianGraphOptionsForm(CartesianGraphDrawer* grd, bool fAxisInfoOnly=false);
		virtual _export ~CartesianGraphOptionsForm();
};

class RoseDiagramOptionsForm : public GraphOptionsForm
{
  public:
		_export RoseDiagramOptionsForm(RoseDiagramDrawer* rdd, bool fAxisInfoOnly=false);
		virtual _export ~RoseDiagramOptionsForm();
};

#endif 
