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
// PointMapDrawer.h: interface for the PointMapDrawer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PointMapDrawer)
#define AFX_PointMapDrawer

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

enum FontStyle { FS_ITALIC    = 0x0001, 
                 FS_UNDERLINE	= 0x0002,
                 FS_STRIKEOUT	= 0x0004 };
/*
#define FS_ITALIC	0x0001
#define FS_UNDERLINE	0x0002
#define FS_STRIKEOUT	0x0004
*/
class PointMapDrawer : public BaseMapDrawer  
{
	friend class PointMapDrawerForm;
  friend class PointMapDrawerForm; 
  friend class PointMapByAttributeForm;
  friend class PointMapArrowForm;
  friend class PointMapGraphForm;
	friend class LegendLayoutItem;
	friend class LegendPointArrowDrawer;
	friend class LegendPointGraphDrawer;
	friend class LegendPointValueDrawer;
	friend class LegendPointCircleDrawer;
	friend class LegendPointBarDrawer;
public:
  PointMapDrawer(MapCompositionDoc*, const PointMap&);
  PointMapDrawer(MapCompositionDoc*, const MapView&, const char* sSection);
  ~PointMapDrawer();
	//{{AFX_VIRTUAL(PointMapDrawer)
  virtual int draw(CDC*, zRect, Positioner*, volatile bool* fDrawStop);
  virtual String sInfo(const Coord&);
// virtual Record rec(const Coord&);
//  virtual void Edit();
  virtual int Configure(bool fShow=true);
  virtual int Setup();
//virtual bool fProperty();
//  virtual void Prop();
  virtual void WriteLayer(MapView&, const char* sSection);
  virtual FormBaseDialog* wEditRpr(CWnd* wPar, const Coord& crd);
  virtual FormBaseDialog* wEditRpr(CWnd* wPar, int iRaw);
  virtual IlwisObject obj() const;
	virtual zIcon icon() const;
  virtual void DrawLegendRect(CDC*, CRect, int iRaw) const;
  virtual void DrawValueLegendRect(CDC*, CRect, double rValue) const;
	virtual bool fLegendUseful() const;
	//}}AFX_VIRTUAL
	//{{AFX_MSG(PointMapDrawer)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
  bool fPntInMask(long iNr) const;
  double _export rStretchSize(double rVal, double rMin, double rMax) const;
  int _export iStretchSize(double rVal, double rMin, double rMax) const;
  ExtendedSymbol smb;
  int iMinSize, iSize;
  int iLinLog, iRadiusArea;
  bool fText, fSymbol;
  String sFaceName;
  short iFHeight;
  short iFWeight;
  short iFStyle;
  Color clrText;
  PointMap mp;
  bool fMask;
  Mask mask;
  Column colCls, colRot, colClr, colVal, colTxt, colJst, colWidth;
  ColumnAndColorArray cacarr;
  enum { stPIECHART, stBARGRAPH, stGRAPH, stCOMPBAR, stVOLCUBES } eST;
  int iWidth;
  Color clrLine;
};

#endif // !defined(AFX_PointMapDrawer)
