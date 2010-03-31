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
#if !defined(AFX_DISTANCEMEASURER_H__BB72FBC9_4BCE_11D3_B7BC_00A0C9D5342F__INCLUDED_)
#define AFX_DISTANCEMEASURER_H__BB72FBC9_4BCE_11D3_B7BC_00A0C9D5342F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DistanceMeasurer.h : header file
//

#if !defined(AFX_MAPPANEVIEWTOOL_H__BB72FBCA_4BCE_11D3_B7BC_00A0C9D5342F__INCLUDED_)
#include "Client\Mapwindow\MapPaneViewTool.h"
#endif

/////////////////////////////////////////////////////////////////////////////
// DistanceMeasurer command target

class DistanceMeasurer : public MapPaneViewTool
{
public:
	DistanceMeasurer(SimpleMapPaneView*);           

	void OnMouseMove(UINT nFlags, CPoint point);
	void OnLButtonDown(UINT nFlags, CPoint point);
	void OnLButtonUp(UINT nFlags, CPoint point);
protected:
	virtual ~DistanceMeasurer();

private:
  void drawLine();
  void InfoReport();
  void Report();
  double rDistance();
  double rAzim();
	CoordSystem csy;
	bool fLatLonCoords();
	bool fProjectedCoords();
	bool fEllipsoidalCoords();
	double rSphericalDistance(const double rRadius);
	double rSphericalAzim(const double rRadius);
	double rSphericalMeridConv(const double rRadius);
	double rEllipsoidDistance(const CoordSystem& cs);
	double rEllipsoidAzimuth(const CoordSystem& cs);
	SimpleMapPaneView* mpv;
  bool fDown;
  zPoint pStart, pEnd;
  Coord cStart, cEnd;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DISTANCEMEASURER_H__BB72FBC9_4BCE_11D3_B7BC_00A0C9D5342F__INCLUDED_)
