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
// PolygonMapDrawer.h: interface for the PolygonMapDrawer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PolygonMapDrawer)
#define AFX_PolygonMapDrawer

#include "Engine\Map\Polygon\POL.H"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class PolygonMapDrawer : public BaseMapDrawer  
{
	friend class PolygonMapDrawerForm;
public:
  PolygonMapDrawer(MapCompositionDoc*, const PolygonMap&);
  PolygonMapDrawer(MapCompositionDoc*, const MapView&, const char* sSection);
  ~PolygonMapDrawer();
	//{{AFX_VIRTUAL(PolygonMapDrawer)
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
	//{{AFX_MSG(PolygonMapDrawer)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
protected:
  bool fBoundariesOnly, fBoundaries, fHideSuperfluousBoundaries;
  bool fPolInMask(const ILWIS::Polygon *) const;
  bool drawSinglePolygon(CDC*, zRect, Positioner*, volatile bool* fDrawStop, ILWIS::Polygon *, const CoordBounds& cb, const CoordBounds& cbIntern, Color c);
  int iBoundaryWidth;
  Color clrBoundary;
  PolygonMap pm;
	int iWidth;
	bool fMask;
	Mask mask;
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_PolygonMapDrawer)
