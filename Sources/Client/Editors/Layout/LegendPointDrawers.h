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
#include "Engine\Domain\Dmvalue.h"

class LegendPointClassDrawer: public LegendClassDrawer
{
public:
  LegendPointClassDrawer(LegendLayoutItem& item)
    : LegendClassDrawer(item)
  {
  }
  ~LegendPointClassDrawer()
  {}
  bool fValid() const
	{
		if (!LegendClassDrawer::fValid())
			return false;
		PointMapDrawer* pdr = dynamic_cast<PointMapDrawer*>(bmd());
		return 0 != pdr;
	}
protected:
  void WriteElements(ElementContainer& ec, const char* sSection)
	{
	  LegendClassDrawer::WriteElements(ec, sSection);
		ObjectInfo::WriteElement(sSection, "LegendType", ec, "PointClass");
	}
  void DrawItem(CDC* cdc, CPoint p, int i)
	{
		long iRaw = aiKeys[i];
		ExtendedSymbol sym(bmd()->rpr(), iRaw);
		p.x += iRowHeight / 2;
		p.y += iRowHeight / 3;
		sym.drawSmb(cdc, 0, p);
	}
};

class LegendPointArrowDrawer: public LegendClassDrawer
{
public:
  LegendPointArrowDrawer(LegendLayoutItem& item);
  ~LegendPointArrowDrawer();
  virtual bool fValid() const;
protected:
	virtual bool fConfigure();
  void WriteElements(ElementContainer& ec, const char* sSection);
  void DrawItem(CDC *cdc,CPoint p, int i);
};

class LegendPointGraphDrawer: public LegendItemDrawer
{
public:
  LegendPointGraphDrawer(LegendLayoutItem& item)
    : LegendItemDrawer(item),
    eText(eNAME)
  {
  }
  ~LegendPointGraphDrawer()
  {}
  bool fValid() const
  {
    if (!LegendItemDrawer::fValid())
      return false;
    PointMapDrawer* pmd = dynamic_cast<PointMapDrawer*>(bmd());
    if (0 == pmd)
      return false;
    if (drmGRAPH != pmd->drmMethod())
      return false;
    return true;
  }
protected:
  void WriteElements(ElementContainer& ec, const char* sSection)
	{
	  LegendItemDrawer::WriteElements(ec, sSection);
    String sText;
    switch (eText) {
      case eNAME: sText = "Name"; break;
      case eDESC: sText = "Description"; break;
    }
    ObjectInfo::WriteElement(sSection, "Text", ec, sText);
    ObjectInfo::WriteElement(sSection, "LegendType", ec, "PointGraph");
  }
  int iItems() {
    PointMapDrawer* pmd = dynamic_cast<PointMapDrawer*>(bmd());
    if (0 == pmd)
      return 0;
    return pmd->cacarr.iSize();
  }
  String sItem(int i) {
    PointMapDrawer* pmd = dynamic_cast<PointMapDrawer*>(bmd());
    if (0 == pmd)
      return sUNDEF;
    Column col = pmd->cacarr[i].col;
    switch (eText) {
      case eNAME:
        return col->sName();
      case eDESC:
        return col->sDescription;
    }
    return sUNDEF;
  }
  virtual void DrawItem(CDC* cdc, CPoint p, int i)
  {
    PointMapDrawer* pmd = dynamic_cast<PointMapDrawer*>(bmd());
    if (0 == pmd)
      return;
    Color clr = pmd->cacarr[i].clr;
		CBrush br(clr);
		CBrush* brOld = cdc->SelectObject(&br);
		p.y += iRowHeight / 10;
    int iH = 4 * iRowHeight / 5;
    int iW = 3 * iH / 2;
    zRect rct(p, zDimension(iW,iH));
    cdc->Rectangle(rct);
		cdc->SelectObject(brOld);
    return;
  }
  bool fConfigure();
  enum eTextType { eNAME, eDESC } eText;
};

class LegendPointValueDrawer: public LegendItemDrawer
{
public:
	LegendPointValueDrawer(LegendLayoutItem& item)
		: LegendItemDrawer(item),
		fUpward(false), iNrSymbols(2), fUnit(false)
	{
	}
	~LegendPointValueDrawer()
	{}
	bool fValid() const
	{
		if (!LegendItemDrawer::fValid())
			return false;
		PointMapDrawer* pmd = dynamic_cast<PointMapDrawer*>(bmd());
		if (0 == pmd)
			return false;
		if (!pmd->fStretch)
			return false;
		return true;
	}
protected:
	virtual void ReadElements(ElementContainer& ec, const char* sSection)
	{
		LegendItemDrawer::ReadElements(ec, sSection);
		ObjectInfo::ReadElement(sSection, "Upward", ec, fUpward);
		ObjectInfo::ReadElement(sSection, "Symbols", ec, iNrSymbols);
		ObjectInfo::ReadElement(sSection, "Unit", ec, fUnit);
	}
	void WriteElements(ElementContainer& ec, const char* sSection)
	{
		LegendItemDrawer::WriteElements(ec, sSection);
		ObjectInfo::WriteElement(sSection, "Upward", ec, fUpward);
		ObjectInfo::WriteElement(sSection, "Symbols", ec, iNrSymbols);
		ObjectInfo::WriteElement(sSection, "LegendType", ec, "PointValue");
		ObjectInfo::WriteElement(sSection, "Unit", ec, fUnit);		
	}
	int iItems() {
		return iNrSymbols;
	}
	double rValue(int i) {
		if (fUpward)
			i = iNrSymbols - i - 1;
		double rMin, rMax;
		PointMapDrawer* pdr = dynamic_cast<PointMapDrawer*>(bmd());
		rMin = pdr->rrStretch.rLo();
		rMax = pdr->rrStretch.rHi();
		if (0 == i)
			return rMin;
		if (iNrSymbols - 1 == i)
			return rMax;
		double rFact = (double) i / (iNrSymbols - 1);
		return rMin + rFact * (rMax - rMin);
	}
	virtual void DrawItem(CDC* cdc, CPoint p, int i)
	{
		double rVal = rValue(i);
		PointMapDrawer* pdr = dynamic_cast<PointMapDrawer*>(bmd());
		ExtendedSymbol sym = pdr->smb;
		double rMin, rMax;
		rMin = pdr->rrStretch.rLo();
		rMax = pdr->rrStretch.rHi();
		sym.iSize = pdr->iStretchSize(rVal, rMin, rMax);
		p.x += iRowHeight / 2;
		p.y += iRowHeight / 3;
		sym.drawSmb(cdc, 0, p);
	}
	String sItem(int i) {
		double rVal = rValue(i);
		DomainValue* pdv = bmd()->dm()->pdv();
		if (pdv)
		{
			String sUnit = pdv->sUnit();
			if ( fUnit && sUnit != "")
				return String("%S %S",pdv->sValue(rVal,0), sUnit);
			else 
				return pdv->sValue(rVal,0);
		}			
		return sUNDEF;
	}
	bool fConfigure();
	bool fUpward;
	int iNrSymbols;
	bool fUnit;
	LOGFONT lf;
};



