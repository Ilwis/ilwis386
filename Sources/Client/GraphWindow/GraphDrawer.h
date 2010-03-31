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
// GraphDrawer.h: interface for the GraphDrawer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRAPHDRAWER_H)
#define AFX_GRAPHDRAWER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#undef IMPEXP
#ifdef ILWISCLIENT
#define IMPEXP __declspec(dllexport)
#else
#define IMPEXP __declspec(dllimport)
#endif

class GraphView;
class GraphAxis;
class CartesianGraphOptionsForm;
class GraphLegend;

#include "Client\GraphWindow\GraphLayer.h"
 
class IMPEXP GraphDrawer
{
public:
  enum Area { aXAXIS, aYAXISLEFT, aYAXISRIGHT, aROSEX, aROSEY, aTITLE, aLEGEND, aGRAPH };
	GraphDrawer(GraphView*);
	virtual ~GraphDrawer();
	virtual bool fConfigure();
  bool fConfigureTitle();
	virtual void draw(CDC*);
	virtual int iSpaceNeededX() const;
	virtual int iSpaceNeededY() const;
  virtual String sText(CPoint p) const;
	virtual bool fAddGraph(bool fConfigLayer);
	virtual bool fAddFormulaGraph(bool fConfigLayer);
	virtual bool fAddLsfGraph(bool fConfigLayer);
	virtual bool fAddSmvGraph(bool fConfigLayer);
	virtual void CalcAxisSizes();
  void SaveSettings(const FileName& fn, const String& sSection);
  void LoadSettings(const FileName& fn, const String& sSection);
	int iLayers() const
		{ return agl.iSize(); }
	virtual void Init();
  virtual void SetView(GraphView* gv);
  virtual Area aPoint(CPoint pt) const;
  void CalcTitleSize();
	GraphView* gvw;
	Array<GraphLayer*> agl;
  GraphLegend* grleg;
  String sTitle;
 	LOGFONT lfTitle;
  int m_iTitleSize;
};


class IMPEXP CartesianGraphDrawer : public GraphDrawer
{
public:
	CartesianGraphDrawer(GraphView*);
	virtual ~CartesianGraphDrawer();
	virtual void draw(CDC*);
  void drawTitle(CDC*);
	virtual bool fConfigure();
	virtual bool fConfigure(CartesianGraphLayer*, bool fLastGraph=false);
	CPoint ptPos(double rRow, double rCol) const;
  void Pt2XY(const CPoint& p, double& rX, double &rY) const;
	virtual int iSpaceNeededX() const;
	virtual int iSpaceNeededY() const;
  virtual String sText(CPoint p) const;
	int iNrBarGraphs() const;
	int iBarGraph(CartesianGraphLayer*) const;
  void SaveSettings(const FileName& fn, const String& sSection);
  void LoadSettings(const FileName& fn, const String& sSection);
  void ClearSettings(const FileName& fn, const String& sSection);
	virtual void CalcAxisSizes();
  virtual void SetView(GraphView* gv);
  virtual Area aPoint(CPoint pt) const;
	GraphAxis *gaxX, *gaxYLeft, *gaxYRight;
	CartesianGraphLayer* cgl(int i) const { return static_cast<CartesianGraphLayer*>(agl[i]); }
};

#endif 
