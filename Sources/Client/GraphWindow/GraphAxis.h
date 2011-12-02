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
// GraphAxis.h: interface for the GraphAxis class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRAPHAXIS_H)
#define AFX_GRAPHAXIS_H

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
class CartesianGraphDrawer;

class IMPEXP GraphAxis
{
public:
	enum GraphAxisType { gatNORMAL=0, gatLOGARITHMIC, gatNORMALPROBABILITY };
	enum GraphAxisPos { gapX=0, gapYLeft, gapYRight, gapXRose, gapYRose };
	GraphAxis(GraphAxisPos gap, CartesianGraphDrawer* gd);
	virtual ~GraphAxis();
	virtual void draw(CDC* cdc);
	virtual void drawGrid(CDC* cdc);
  virtual bool fConfig();
	void Set(const DomainValueRangeStruct& dvrs);
	void SetMinMax(const RangeReal& rr); // and default step
	void ExpandMinMax(const RangeReal& rr);
	void SetGridStep(double r);
	double rMin() const;
	double rMax() const;
	double rGridStep() const;
	double rConv(double r) const; // conversion to internal 'coordinates' 0..1000
	double rConvInv(double r) const; // conversion from internal 'coordinates' to 'real values'
	double rFirstGridLine() const;
	double rNextGridLine(double rCurr) const;
	String sText(double rVal, bool fUseDvrsData=false) const;
	int iSize() const; // width for vert. axis; height for hor. axis
	virtual int iFloatPos() const;
	virtual double rShift() const;
  virtual void SaveSettings(const FileName& fn, const String& sSection);
  virtual void LoadSettings(const FileName& fn, const String& sSection);
	virtual int iCalcSize() const;
	void CalcSize();
	String sTitle;
	bool fShowGrid;
	bool fFloating;
	GraphAxisType gat;
	GraphAxisPos gap;
	int iDefTicks;
	DomainValueRangeStruct dvrs, dvrsData;
	DomainSort* ds;
	bool fDomainNone;
  bool fShowCodes; // for DomainSort
	ValueRange vr;
	bool fVisible;
	GraphView* gvw;
	CartesianGraphDrawer* grdrw;
	int iTextRot;
	int _iSize;
 	LOGFONT lfTitle;
  int iExtraSize;
private:
  double m_rGridStep;
};

#endif 
