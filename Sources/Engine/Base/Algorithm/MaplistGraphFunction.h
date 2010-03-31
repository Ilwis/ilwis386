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
// MaplistGraphFunction.h: interface for the MaplistGraphFunction class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAPLISTGRAPHFUNCTION_H__EBFA9221_4BBA_43E6_85BA_D066DEE6C14C__INCLUDED_)
#define AFX_MAPLISTGRAPHFUNCTION_H__EBFA9221_4BBA_43E6_85BA_D066DEE6C14C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Engine\Base\Algorithm\SimpleFunction.h"

class _export MaplistGraphFunction : public SimpleFunction
{
public:
	MaplistGraphFunction();
	virtual ~MaplistGraphFunction();
	virtual double rGetFx(double x) const; // the most important function of this class that applies f to x and returns y
	virtual void SetDefaultAnchors(); // also call SolveParams in the implementation of this function
	virtual void SetAnchor(DoublePoint pAnchor);
	void SetMaplist(String sMapList);
	void ReadData();
	void SetStretch(bool fFixedStretch, double rMin, double rMax);
	void SetCoordAndCsy(CoordWithCoordSystem & cwcs);
	String sClipboardText();
	const CoordSystem & csyMpl() const;
	const GeoRef & grfMpl() const;
	const Coord & crdMpl() const;
	const DoublePoint & dpDragPoint() const;
	const int iDec() const;
protected:
	virtual void SolveParams();
private:
	MapList m_mpl;
	int m_iStartBand;
	int m_iEndBand;
	int m_iOffset;
	CoordSystem m_csy;
	Coord m_crd;
	GeoRef m_grfMpl; // for returning the rowcols back to the form, in "maplist coordinates"
	CoordSystem m_csyMpl; // for returning the latlons back to the form, in "maplist coordinates"
	Coord m_crdMpl; // for returning the projected coordinates back to the form, in "maplist coordinates"
	bool m_fFixedStretch;
	double m_rStretchMin;
	double m_rStretchMax;
	DoublePoint m_dpDragPoint;
	int m_iDec;
	vector <double> m_rData;
};

#endif // !defined(AFX_MAPLISTGRAPHFUNCTION_H__EBFA9221_4BBA_43E6_85BA_D066DEE6C14C__INCLUDED_)
