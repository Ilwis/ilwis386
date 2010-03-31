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
// MapDrawer.h: interface for the MapDrawer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAPDRAWER_H__EEDE7CB5_E843_11D2_B744_00A0C9D5342F__INCLUDED_)
#define AFX_MAPDRAWER_H__EEDE7CB5_E843_11D2_B744_00A0C9D5342F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_DRAWER_H__96068AF6_E5AE_11D2_B73F_00A0C9D5342F__INCLUDED_)
#include "Client\Mapwindow\Drawers\BaseMapDrawer.h"
#endif

class MapDrawer : public BaseMapDrawer  
{
	friend class MapDrawerForm;
public:
  MapDrawer(MapCompositionDoc*, const Map&);
  MapDrawer(MapCompositionDoc*, const MapView&, const char* sSection);
  ~MapDrawer();
	//{{AFX_VIRTUAL(MapDrawer)
  virtual int draw(CDC*, zRect, Positioner*, volatile bool* fDrawStop);
  virtual String sInfo(const Coord&);
  virtual bool fEditable();
  virtual int Configure(bool fShow=true);
  virtual int Setup();
  virtual void WriteLayer(MapView&, const char* sSection);
  virtual FormBaseDialog* wEditRpr(CWnd* wPar, const Coord& crd);
  virtual FormBaseDialog* wEditRpr(CWnd* wPar, int iRaw);
  virtual IlwisObject obj() const;
	virtual zIcon icon() const;
  virtual void DrawLegendRect(CDC*, CRect, int iRaw) const;
  virtual void DrawValueLegendRect(CDC*, CRect, double rValue) const;
	//}}AFX_VIRTUAL
public:
  void ShowHistogram();
  const Map& mpGet() const { return mp; }
  void SetMap(const Map& map); 
	bool fUsesBandedDrawing();
	void UsesBandedDrawing(bool fYesNo);
	String getForeignType();

protected:
  Map mp;
  LPBITMAPINFO lpbi;
  void init();
  void PutLine(CDC*, zRect rect, int iLine, const LongBuf&);
  void ConvLine(LongBuf&);
  void ConvLine(const RealBuf&, LongBuf&);
  void calcColIndex();
  int GreyColor();
  int PrimaryColor(int iNr);
  int SingleColor();
  int DualColor();
public:  
  bool fImage;
  void Lut();
  void InitColors(ColorBuf&);
protected:  
  bool fText;
  Color clrText;
  RangeInt riRawStretch;
  Color* colIndex;
	bool fBands;
//  zBitmap* bmTmp;
private:
	void SetColIndex(Color* colIndexNew);
	//{{AFX_MSG(MapDrawer)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_MAPDRAWER_H__EEDE7CB5_E843_11D2_B744_00A0C9D5342F__INCLUDED_)
