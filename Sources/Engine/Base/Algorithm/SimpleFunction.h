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
// SimpleFunction.h: interface for the SimpleFunction class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FUNCTION_H__CE61C846_E09A_4BD6_937F_889978879B56__INCLUDED_)
#define AFX_FUNCTION_H__CE61C846_E09A_4BD6_937F_889978879B56__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxwin.h> // prevent warning C4251: 'm_Anchors' 
#include <vector>
using namespace std;

class DoublePoint
{
public:
	DoublePoint()
		: X (0)
		, Y (0)
	{
	}
	DoublePoint(double _X, double _Y)
		: X (_X)
		, Y (_Y)
	{
	}
	bool operator == (DoublePoint & pOther)
	{
		return (X == pOther.X) && (Y == pOther.Y);
	}
	bool operator != (DoublePoint & pOther)
	{
		return (X != pOther.X) || (Y != pOther.Y);
	}
	double rDist2(DoublePoint b) const
	{
		double rX = X - b.X;
		double rY = Y - b.Y;
		return (rX * rX + rY * rY);
	}

	double X;
	double Y;
};

class __declspec(dllexport) DoubleRect
{
public:
	DoubleRect();
	DoubleRect(double l, double t, double r, double b);
	void SetRect(double l, double t, double r, double b);
	const double Width() const;
	const double Height() const;
	const bool fValid() const;
	void NormalizeRect(bool fULOrigin = false);

	double top;
	double bottom;
	double left;
	double right;
private:
	bool m_fValid;
};

class __declspec(dllexport) SimpleFunction  
{
public:
	SimpleFunction(int iNrAnchors, const DoubleRect & drDomain); // iSolutions: # of points needed to solve the function
	virtual ~SimpleFunction();
	virtual double rGetFx(double x) const = 0; // the most important function of this class that applies f to x and returns y
	DoubleRect Domain() const;
	void SetAnchors(vector <DoublePoint> vAnchors);
	virtual void SetAnchor(DoublePoint pAnchor);
	virtual void SetDefaultAnchors() = 0; // also call SolveParams in the implementation of this function
	vector <DoublePoint> vAnchors() const;
protected:
	virtual void SolveParams() = 0;
	vector <DoublePoint> m_Anchors;
	DoubleRect m_Domain;
private:
	void SeparateAnchorPoints();
	double rUnitDist2(DoublePoint & p1, DoublePoint & p2);
};

#endif // !defined(AFX_FUNCTION_H__CE61C846_E09A_4BD6_937F_889978879B56__INCLUDED_)
