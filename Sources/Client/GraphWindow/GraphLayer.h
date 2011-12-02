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
// GraphLayer.h: interface for the GraphLayer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRAPHLAYER_H)
#define AFX_GRAPHLAYER_H

#include "Client\Editors\Utils\SYMBOL.H"
#include "Engine\Base\Algorithm\semivar.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef LINE_H
#include "Client\Editors\Utils\line.h"
#endif

#undef IMPEXP
#ifdef ILWISCLIENT
#define IMPEXP __declspec(dllexport)
#else
#define IMPEXP __declspec(dllimport)
#endif

class GraphView;
class GraphAxis;
class GraphDrawer;
class CartesianGraphDrawer;
class GraphLayerOptionsForm;


class IMPEXP GraphLayer
{
public:
	GraphLayer(GraphDrawer*);
  GraphLayer(GraphDrawer*, const FileName& fnObj, const String& sSection);
	virtual ~GraphLayer();
	virtual String sName();
	virtual void draw(CDC*);
	virtual bool fConfig();
	virtual int iNrPoints();
	virtual double rX(int iRec);
	virtual double rY(int iRec);
	virtual GraphLayerOptionsForm* frmOptions();
	GraphDrawer* gd;
  virtual void SaveSettings(const FileName& fn, const String& sSection);
	String sTitle;
  Line line;
  Color color;
  Symbol  smb;
  bool fRprColor;
	Representation rpr;
  bool fShow;
};

enum CartesianGraphType {cgtContineous=0, cgtStep, cgtBar, cgtNeedle, cgtPoint, cgtUnknownType };

class IMPEXP CartesianGraphLayer : public GraphLayer
{
public:
	CartesianGraphLayer(CartesianGraphDrawer*);
  CartesianGraphLayer(CartesianGraphDrawer*, const FileName& fnObj, const String& sSection);
	virtual ~CartesianGraphLayer();
	virtual String sName();
	virtual void draw(CDC*);
	virtual bool fConfig();
	virtual int iNrPoints();
	virtual double rX(int iRec);
	void SetType(String sGraphType);
	CartesianGraphDrawer* cgd;
  virtual void SaveSettings(const FileName& fn, const String& sSection);
	static CartesianGraphLayer* create(const FileName& fn, const String& sSection, CartesianGraphDrawer* gd);
	bool fYAxisLeft;
  CartesianGraphType cgt;
};

class IMPEXP ColumnGraphLayer : public CartesianGraphLayer
{
public:
	ColumnGraphLayer(CartesianGraphDrawer* cgd,const Table& tbl, const Column& colX, const Column& colY);
	ColumnGraphLayer(CartesianGraphDrawer* cgd, const FileName& fnObj, const String& sSection);
	virtual ~ColumnGraphLayer();
	virtual GraphLayerOptionsForm* frmOptions();
	virtual bool fConfig();
	virtual String sName();
	virtual void draw(CDC*);
  virtual void SaveSettings(const FileName& fn, const String& sSection);

// data:
	virtual int iNrPoints();
	virtual double rX(int iRec);
	virtual double rY(int iRec);
	Table tbl;
	Column colX, colY;
};

class Instructions;

class IMPEXP FormulaGraphLayer : public CartesianGraphLayer
{
public:
	FormulaGraphLayer(CartesianGraphDrawer*,const String& sExpr);
	FormulaGraphLayer(CartesianGraphDrawer*, const FileName& fnObj, const String& sSection);
	virtual ~FormulaGraphLayer();
	virtual String sName();
	virtual double rY(int iRec);
	virtual bool fConfig();
	virtual GraphLayerOptionsForm* frmOptions();
	void SetExpression(const String& sExpr);
  virtual void SaveSettings(const FileName& fn, const String& sSection);
	String sExpr;
	Instructions* inst;
};

class LeastSquaresFit;

class IMPEXP LsfGraphLayer : public ColumnGraphLayer
{
public:
	LsfGraphLayer(CartesianGraphDrawer*, const Table& t, const Column& colX, const Column& colY, const String& sFunc, int iTerms);
	LsfGraphLayer(CartesianGraphDrawer*, const FileName& fnObj, const String& sSection);
	virtual ~LsfGraphLayer();
	virtual String sName();
	virtual double rY(int iRec);
	void SetLsf(const String& sFitType, int iTerms);
	virtual bool fConfig();
	virtual GraphLayerOptionsForm* frmOptions();
  virtual void SaveSettings(const FileName& fn, const String& sSection);
  LeastSquaresFit* lsf;
	String sFitType;
	int iNrValidPnts, iTerms;
	String sGName;
};

class IMPEXP SmvGraphLayer : public CartesianGraphLayer
{
public:
	SmvGraphLayer(CartesianGraphDrawer*  cgd, const SemiVariogram& smv);
	SmvGraphLayer(CartesianGraphDrawer*  cgd, const FileName& fnObj, const String& sSection);
	virtual ~SmvGraphLayer();
	virtual String sName();
	virtual double rY(int iRec);
	virtual bool fConfig();
	virtual GraphLayerOptionsForm* frmOptions();
  virtual void SaveSettings(const FileName& fn, const String& sSection);
	SemiVariogram smv;
};

#endif 
