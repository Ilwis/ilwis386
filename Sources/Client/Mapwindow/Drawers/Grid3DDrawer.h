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
#ifndef GRID3DDRAWER_H
#define GRID3DDRAWER_H

class _export Grid3DDrawer: public Drawer
{
public:
  Grid3DDrawer(MapCompositionDoc*);
  Grid3DDrawer(MapCompositionDoc*, const MapView& , const char* sSection);
  ~Grid3DDrawer();

  virtual int draw(CDC *dc, zRect, Positioner*, volatile bool *);
  virtual String sName();
  virtual int Configure(bool fShow=true);
  virtual void WriteLayer(MapView&, const char* sSection);
  virtual IlwisObject obj() const;
	GeoRef3D *grfGeoRef();
	bool fLegendUseful() const;
  virtual FormBaseDialog* wEditRpr(CWnd* wPar, const Coord& crd);
  virtual FormBaseDialog* wEditRpr(CWnd* wPar, int iRaw);
  virtual void DrawLegendRect(CDC*, CRect, int iRaw) const;
  virtual void DrawValueLegendRect(CDC*, CRect, double rValue) const;	
	RangeReal rrStretchRange() const;
  virtual Color clrRaw(long iRaw) const;
  virtual Color clrVal(double rVal) const;
	String sInfo(const Coord& crd);
	Map Drape();
	zIcon icon() const;
	
protected:
  struct PointStruct 
  {
    CPoint pnt;
    Color col;
  };
  void DrawFilledGrid(CDC *dc, CRect rect, Positioner *psn, volatile bool *fDrawStop);
  void DrawOpenGrid(CDC *dc, CRect rect, Positioner *psn, volatile bool *fDrawStop);
  PointStruct *MakePoints(Positioner *psn, long iRow, long iStartCol, long iEndCol, short& n, 
                          long iColDelta);
  CPoint *MakePoints(Positioner *psn, long iRow, long iStartCol, long iEndCol, short& n);
  void DrawPolygons(CDC *dc, CRect rect, PointStruct *topLine, PointStruct *bottomLine, short n);
  void DrawLines(CDC *dc, CRect rect, CPoint *topLine, CPoint *bottomLine, short n, double rColDelta);
  CPoint Coord2Pnt(Positioner* psn, Coord c);
  CoordBounds cbCalc(const MinMax& mm);

  void init();
  int Setup();

  double rDist;
  long iFillSize;
  Color col;
  int iWidth;
  Coord cStart;
  bool fFast, fFilled, fGrid;
  Map mp;

	bool fValue;
  bool fAttTable;
  Domain dm;
  Column colAtt;
  Representation rpr;
  bool fStretch;
  RangeInt riStretch;
  RangeReal rrStretch;
  RangeReal rrMinMax;
  RangeInt riRawStretch;
  DrawMethod drm;
//  int iNrColors;
//  int iPalOffset;
  Color clr;
  GeoRef3D *grf3d;
  GeoRef grf;
  RowCol rc[4];
  CoordBounds cb;
};

#endif
