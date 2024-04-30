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
// SimpleFunction.cpp: implementation of the SimpleFunction class.
//
//////////////////////////////////////////////////////////////////////

#include "Engine\Base\Algorithm\SimpleFunction.h"
#include <math.h> // fabs

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// DoubleRect

DoubleRect::DoubleRect()
: top(0)
, bottom(0)
, left(0)
, right(0)
, m_fValid(false)
{
}

DoubleRect::DoubleRect(double l, double t, double r, double b)
: top(t)
, bottom(b)
, left(l)
, right(r)
, m_fValid(true)
{
}

void DoubleRect::SetRect(double l, double t, double r, double b)
{
	top = t;
	bottom = b;
	left = l;
	right = r;
	m_fValid = true;
}

const double DoubleRect::Width() const
{
	return fabs(right - left);
}

const double DoubleRect::Height() const
{
	return fabs(bottom - top);
}

const bool DoubleRect::fValid() const
{
	return m_fValid;
}

void DoubleRect::NormalizeRect(bool fULOrigin)
{
	if (right < left)
	{
		double temp = right;
		right = left;
		left = temp;
	}
	if (fULOrigin) // RowCols and Windows screen coordinates have upper-left origin
	{
		if (bottom < top)
		{
			double temp = bottom;
			bottom = top;
			top = temp;
		}
	}
	else // Bottom-left origin .. bottom SHOULD be smaller than top
	{
		if (bottom > top)
		{
			double temp = bottom;
			bottom = top;
			top = temp;
		}
	}
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SimpleFunction::SimpleFunction(int iNrAnchors, const DoubleRect & drDomain)
{
	m_Anchors.resize(iNrAnchors);
	m_Domain = drDomain;
	m_Domain.NormalizeRect();
}

SimpleFunction::~SimpleFunction()
{

}

void SimpleFunction::SetAnchors(vector <DoublePoint> vAnchors)
{
	m_Anchors = vAnchors;
	SeparateAnchorPoints();
	SolveParams();
}

double SimpleFunction::rUnitDist2(DoublePoint & p1, DoublePoint & p2)
{
	double rX = p1.X - p2.X;
	double rY = p1.Y - p2.Y;
	double rNormX = rX / m_Domain.Width();
	double rNormY = rY / m_Domain.Height();
	return rNormX * rNormX + rNormY * rNormY;
}

void SimpleFunction::SetAnchor(DoublePoint pAnchor)
{
	if (m_Anchors.size() > 0)
	{
		int iPoint = 0;
		for (size_t i = 1; i < m_Anchors.size(); ++i)
		{
			if (rUnitDist2(pAnchor, m_Anchors[i]) < rUnitDist2(pAnchor, m_Anchors[iPoint]))
				iPoint = i;
		}
		m_Anchors[iPoint] = pAnchor;
		SeparateAnchorPoints();
	}
	SolveParams();
}

DoubleRect SimpleFunction::Domain() const
{
	return m_Domain;
}

vector <DoublePoint> SimpleFunction::vAnchors() const
{
	return m_Anchors;
}

bool bLessThan(DoublePoint &dp1, DoublePoint &dp2)
{
	return dp1.X < dp2.X;
}

void SimpleFunction::SeparateAnchorPoints()
{
	double rMinSeparation = m_Domain.Width() / 1000.0;
	for (size_t i = 1; i < m_Anchors.size(); ++i)
	{
		if (m_Anchors[i].X < (m_Anchors[i-1].X + rMinSeparation))
			m_Anchors[i].X = m_Anchors[i-1].X + rMinSeparation;
	}
}
