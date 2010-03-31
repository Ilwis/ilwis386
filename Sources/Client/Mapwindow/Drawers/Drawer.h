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
// Drawer.h: interface for the Drawer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DRAWER_H__96068AF6_E5AE_11D2_B73F_00A0C9D5342F__INCLUDED_)
#define AFX_DRAWER_H__96068AF6_E5AE_11D2_B73F_00A0C9D5342F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef ILWMAPVIEW_H
#include "Engine\Map\Mapview.h"
#endif

#if !defined(AFX_BASEDRAWER_H__8A84267D_E359_11D2_B73E_00A0C9D5342F__INCLUDED_)
#include "Client\Mapwindow\Drawers\BaseDrawer.h"
#endif

enum DrawMethod
{ drmRPR, drmSINGLE, drmMULTIPLE, drmIMAGE,
  drmCOLOR, drmBOOL, drmBYATTRIBUTE, drmARROW, drmGRAPH };

class MapPaneView;
class MapCompositionDoc;

// commented away functions have still to be added!

class Drawer : public BaseDrawer  
{
	friend class MapCompositionDoc;
	friend class DrawerForm;
public:
	virtual ~Drawer();
//	CBitmap* bmMenu(CDC*); -- does not work properly!
	//{{AFX_VIRTUAL(Drawer)
  virtual int draw(CDC*, zRect, Positioner*, volatile bool* fDrawStop) = 0;
  virtual int Configure(bool fShow=true);
  virtual int Setup();
  virtual String sName();
  virtual String sInfo(const Coord&);
  virtual Ilwis::Record rec(const Coord&);
  virtual void Edit();
  virtual void Prop();
  virtual void WriteLayer(MapView&, const char* sSection);
  virtual FormBaseDialog* wEditRpr(CWnd* wPar, const Coord& crd);
  virtual FormBaseDialog* wEditRpr(CWnd* wPar, int iRaw);
  virtual void DrawLegendRect(CDC*, CRect, int iRaw) const;
  virtual void DrawValueLegendRect(CDC*, CRect, double rValue) const;	
  virtual bool fAttributes();
  virtual bool fRepresentation();
  virtual void EditRepresentation();
  virtual bool fEditable();
  virtual bool fProperty();
  virtual bool fEditDomain();
  virtual void EditDomain();
  virtual void ExecuteAction(const Coord&);
  virtual IlwisObject obj() const;
  virtual bool fDependsOn(const Drawer*) const;
	virtual bool fLegendUseful() const;
	//}}AFX_VIRTUAL
	//{{AFX_MSG(Drawer)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
public:
  unsigned fAct: 1;
  unsigned fNew: 1;
  unsigned fSelectPossible: 1;
  unsigned fDelete: 1;
  unsigned fDeletePossible: 1;
  bool fSelectable;
	bool fScaleLimits;
	RangeInt riScaleLimits;
  bool fTransparency;
  int iTransparence; // in percentage, 100% means transparent
	RangeInt riTranquilizer;
	int iTranquilizer;
  static void CalcBounds(const GeoRef& grf, const MinMax&, LatLon& llMin, LatLon& llMax);
  static void CalcBounds(const CoordSystem& cs, const LatLon& llMin, const LatLon& llMax, CoordBounds& cb);
protected:
  Drawer(MapCompositionDoc*);
  Drawer(MapCompositionDoc*, const MapView& view, const char* sSection);
	MapCompositionDoc* mcd;
private:
	DECLARE_MESSAGE_MAP()
};

class DrawerForm: public FormWithDest
{
public:
  DrawerForm(Drawer*, const String& sTitle);
protected:
	void AskScaleLimits();
	Drawer* drw;
};


#endif // !defined(AFX_DRAWER_H__96068AF6_E5AE_11D2_B73F_00A0C9D5342F__INCLUDED_)
