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
// LegendBlockDrawer.h: interface for the LegendBlockDrawer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LEGENDBLOCKDRAWER_H__D9D9FA03_6787_11D4_B8E7_00A0C9D5342F__INCLUDED_)
#define AFX_LEGENDBLOCKDRAWER_H__D9D9FA03_6787_11D4_B8E7_00A0C9D5342F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class LegendBlockDrawer: public LegendBaseDrawer
{
	friend class ConfForm;
protected:
  LegendBlockDrawer(LegendLayoutItem& item)
  : LegendBaseDrawer(item), iNrValues(3)
  {
  }
  ~LegendBlockDrawer()
  {}
	virtual void ReadElements(ElementContainer& ec, const char* sSection)
  {
    LegendBaseDrawer::ReadElements(ec, sSection);
		ObjectInfo::ReadElement(sSection, "Values", ec, iNrValues);
	}
  void WriteElements(ElementContainer& ec, const char* sSection)
  {
    LegendBaseDrawer::WriteElements(ec, sSection);
    ObjectInfo::WriteElement(sSection, "Values", ec, iNrValues);
  }
	virtual bool fConfigure();
  int iMaxWidth();
  double rValue(int iLim) {
		RangeReal rr = bmd()->rrStretchRange();
    double rLo = rr.rLo();
    double rHi = rr.rHi();
    double rFrac = double(iLim) / (iNrValues-1);
    return rLo + rFrac * (rHi - rLo);
  }
  virtual String sValue(double rVal) = 0;
	virtual void Setup() = 0;
  int iNrValues;
};

class LegendPointCircleDrawer: public LegendBlockDrawer
{
public:
  LegendPointCircleDrawer(LegendLayoutItem& item)
		: LegendBlockDrawer(item)
	{
	}
  ~LegendPointCircleDrawer()
	{}
  void WriteElements(ElementContainer& ec, const char* sSection)
	{
	  LegendBlockDrawer::WriteElements(ec, sSection);
    ObjectInfo::WriteElement(sSection, "LegendType", ec, "PointCircle");
	}
protected:
  bool fValid();
  void Setup();
  String sValue(double rVal);
	void OnDraw(CDC* cdc);
};

class LegendPointBarDrawer: public LegendBlockDrawer
{
public:
  LegendPointBarDrawer(LegendLayoutItem& item)
		: LegendBlockDrawer(item)
	{
	}
  ~LegendPointBarDrawer()
	{}
  void WriteElements(ElementContainer& ec, const char* sSection)
	{
	  LegendBlockDrawer::WriteElements(ec, sSection);
    ObjectInfo::WriteElement(sSection, "LegendType", ec, "PointBar");
	}
protected:
  bool fValid();
  void Setup();
  String sValue(double rVal);
	void OnDraw(CDC* cdc);
};




#endif // !defined(AFX_LEGENDBLOCKDRAWER_H__D9D9FA03_6787_11D4_B8E7_00A0C9D5342F__INCLUDED_)
