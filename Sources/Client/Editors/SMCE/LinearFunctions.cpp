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
//////////////////////////////////////////////////////////////////////

#include "Client\Editors\SMCE\LinearFunctions.h"

#include "Headers\Hs\smce.hs"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// MaximumFunction
//////////////////////////////////////////////////////////////////////

MaximumFunction::MaximumFunction(const DoubleRect & drDomain, bool fBenefit)
: SmceFunction(0, drDomain, fBenefit)
{

}

MaximumFunction::~MaximumFunction()
{

}

double MaximumFunction::rGetFx(double x) const
{
	double a = fBenefit() ? ((m_Domain.top - m_Domain.bottom) / m_Domain.right) : ((m_Domain.bottom - m_Domain.top) / m_Domain.right);
	double b = fBenefit() ? m_Domain.bottom : (m_Domain.top - a * m_Domain.left);
	return a * x + b;
}

void MaximumFunction::SolveParams()
{
}

void MaximumFunction::SetDefaultAnchors()
{
}

String MaximumFunction::sGetFx(String sData)
{
	double a = fBenefit() ? ((m_Domain.top - m_Domain.bottom) / m_Domain.right) : ((m_Domain.bottom - m_Domain.top) / m_Domain.right);
	double b = fBenefit() ? m_Domain.bottom : (m_Domain.top - a * m_Domain.left);
	//return a * x + b;
	if (a != 0)
	{
		if (b != 0)
			return String ("%lg*%S+%lg", a, sData, b);
		else
			return String("%lg*%S", a, sData);
	}
	else
	{
		return String("%lg", b);
	}
}

String MaximumFunction::sDescription()
{
	return "Maximum";
}


//////////////////////////////////////////////////////////////////////
// IntervalFunction
//////////////////////////////////////////////////////////////////////

IntervalFunction::IntervalFunction(const DoubleRect & drDomain, bool fBenefit)
: SmceFunction(0, drDomain, fBenefit)
{

}

IntervalFunction::~IntervalFunction()
{

}

double IntervalFunction::rGetFx(double x) const
{
	double a = ((fBenefit()?m_Domain.top:m_Domain.bottom) - (fBenefit()?m_Domain.bottom:m_Domain.top)) / (m_Domain.right - m_Domain.left);
	double b = (fBenefit()?m_Domain.bottom:m_Domain.top) - a * m_Domain.left;
	return a * x + b;
}

void IntervalFunction::SolveParams()
{
}

void IntervalFunction::SetDefaultAnchors()
{
}

String IntervalFunction::sGetFx(String sData)
{
	double a = ((fBenefit()?m_Domain.top:m_Domain.bottom) - (fBenefit()?m_Domain.bottom:m_Domain.top)) / (m_Domain.right - m_Domain.left);
	double b = (fBenefit()?m_Domain.bottom:m_Domain.top) - a * m_Domain.left;
	//return a * x + b;
	if (a != 0)
	{
		if (b != 0)
			return String ("%lg*%S+%lg", a, sData, b);
		else
			return String("%lg*%S", a, sData);
	}
	else
	{
		return String("%lg", b);
	}
}

String IntervalFunction::sDescription()
{
	return "Interval";
}

//////////////////////////////////////////////////////////////////////
// GoalFunction
//////////////////////////////////////////////////////////////////////

GoalFunction::GoalFunction(const DoubleRect & drDomain, bool fBenefit)
: SmceFunction(2, drDomain, fBenefit)
{

}

GoalFunction::~GoalFunction()
{

}

double GoalFunction::rGetFx(double x) const
{
	if (x < m_Anchors[0].X)
		return fBenefit()?m_Domain.bottom:m_Domain.top;
	else if (x > m_Anchors[1].X)
		return fBenefit()?m_Domain.top:m_Domain.bottom;
	else
		return a * x + b;
}

void GoalFunction::SolveParams()
{
	m_Anchors[0].Y = fBenefit()?m_Domain.bottom:m_Domain.top;
	m_Anchors[1].Y = fBenefit()?m_Domain.top:m_Domain.bottom;

	a = (m_Anchors[1].Y - m_Anchors[0].Y) / (m_Anchors[1].X - m_Anchors[0].X);
	b = m_Anchors[0].Y - a * m_Anchors[0].X;
}

void GoalFunction::SetDefaultAnchors()
{
	m_Anchors[0].X = m_Domain.left;
	m_Anchors[1].X = m_Domain.right;
	SolveParams();
}

String GoalFunction::sGetFx(String sData)
{
	SolveParams();
	//return a * x + b;
	if (a != 0)
	{
		if (b != 0)
			return String ("%lg*%S+%lg", a, sData, b);
		else
			return String("%lg*%S", a, sData);
	}
	else
	{
		return String("%lg", b);
	}
}

String GoalFunction::sDescription()
{
	return String("%S(%.3f,%.3f)", TR("Goal"), m_Anchors[0].X, m_Anchors[1].X);
}

//////////////////////////////////////////////////////////////////////
// PiecewiseLinearFunction
//////////////////////////////////////////////////////////////////////

PiecewiseLinearFunction::PiecewiseLinearFunction(const DoubleRect & drDomain, bool fBenefit)
: SmceFunction(4, drDomain, fBenefit)
{

}

PiecewiseLinearFunction::~PiecewiseLinearFunction()
{

}

double PiecewiseLinearFunction::rGetFx(double x) const
{
	if (x < m_Anchors[1].X)
		return a1 * x + b1;
	else if (x < m_Anchors[2].X)
		return a2 * x + b2;
	else
		return a3 * x + b3;
}

void PiecewiseLinearFunction::SolveParams()
{
	m_Anchors[0].X = m_Domain.left;
	m_Anchors[3].X = m_Domain.right;
	for (int i = 0; i < 4; ++i)
	{
		if (m_Anchors[i].Y < m_Domain.bottom)
			m_Anchors[i].Y = m_Domain.bottom;
		else if (m_Anchors[i].Y > m_Domain.top)
			m_Anchors[i].Y = m_Domain.top;
	}

	a1 = (m_Anchors[1].Y - m_Anchors[0].Y) / (m_Anchors[1].X - m_Anchors[0].X);
	b1 = m_Anchors[0].Y - a1 * m_Anchors[0].X;

	a2 = (m_Anchors[2].Y - m_Anchors[1].Y) / (m_Anchors[2].X - m_Anchors[1].X);
	b2 = m_Anchors[1].Y - a2 * m_Anchors[1].X;
	
	a3 = (m_Anchors[3].Y - m_Anchors[2].Y) / (m_Anchors[3].X - m_Anchors[2].X);
	b3 = m_Anchors[2].Y - a3 * m_Anchors[2].X;
}

void PiecewiseLinearFunction::SetDefaultAnchors()
{
	m_Anchors[1].X = m_Domain.left + m_Domain.Width() / 3;
	m_Anchors[2].X = m_Domain.left + m_Domain.Width() * 2 / 3;
	if (fBenefit())
	{
		m_Anchors[0].Y = m_Domain.bottom;
		m_Anchors[1].Y = m_Domain.bottom + m_Domain.Height() / 3;
		m_Anchors[2].Y = m_Domain.bottom + m_Domain.Height() * 2 / 3;
		m_Anchors[3].Y = m_Domain.top;
	}
	else
	{
		m_Anchors[0].Y = m_Domain.top;
		m_Anchors[1].Y = m_Domain.bottom + m_Domain.Height() * 2 / 3;
		m_Anchors[2].Y = m_Domain.bottom + m_Domain.Height() / 3;
		m_Anchors[3].Y = m_Domain.bottom;
	}
	SolveParams();
}

String PiecewiseLinearFunction::sGetFx(String sData)
{
	SolveParams();
	//return a * x + b;
	return String ("iff(%S<%lg,%lg*%S+%lg,iff(%S<%lg,%lg*%S+%lg,%lg*%S+%lg))",sData, m_Anchors[1].X, a1, sData, b1, sData, m_Anchors[2].X, a2, sData, b2, a3, sData, b3);
}

String PiecewiseLinearFunction::sDescription()
{
	return String("%S(%.3f,%.3f)", String("PiecewiseLinear"), m_Anchors[0].X, m_Anchors[3].X);
}

//////////////////////////////////////////////////////////////////////
// PiecewiseLinear5Function
//////////////////////////////////////////////////////////////////////

PiecewiseLinear5Function::PiecewiseLinear5Function(const DoubleRect & drDomain, bool fBenefit)
: SmceFunction(6, drDomain, fBenefit)
{

}

PiecewiseLinear5Function::~PiecewiseLinear5Function()
{

}

double PiecewiseLinear5Function::rGetFx(double x) const
{
	if (x < m_Anchors[1].X)
		return a1 * x + b1;
	else if (x < m_Anchors[2].X)
		return a2 * x + b2;
	else if (x < m_Anchors[3].X)
		return a3 * x + b3;
	else if (x < m_Anchors[4].X)
		return a4 * x + b4;
	else
		return a5 * x + b5;
}

void PiecewiseLinear5Function::SolveParams()
{
	m_Anchors[0].X = m_Domain.left;
	m_Anchors[5].X = m_Domain.right;
	for (int i = 0; i < 6; ++i)
	{
		if (m_Anchors[i].Y < m_Domain.bottom)
			m_Anchors[i].Y = m_Domain.bottom;
		else if (m_Anchors[i].Y > m_Domain.top)
			m_Anchors[i].Y = m_Domain.top;
	}

	a1 = (m_Anchors[1].Y - m_Anchors[0].Y) / (m_Anchors[1].X - m_Anchors[0].X);
	b1 = m_Anchors[0].Y - a1 * m_Anchors[0].X;

	a2 = (m_Anchors[2].Y - m_Anchors[1].Y) / (m_Anchors[2].X - m_Anchors[1].X);
	b2 = m_Anchors[1].Y - a2 * m_Anchors[1].X;
	
	a3 = (m_Anchors[3].Y - m_Anchors[2].Y) / (m_Anchors[3].X - m_Anchors[2].X);
	b3 = m_Anchors[2].Y - a3 * m_Anchors[2].X;

	a4 = (m_Anchors[4].Y - m_Anchors[3].Y) / (m_Anchors[4].X - m_Anchors[3].X);
	b4 = m_Anchors[3].Y - a4 * m_Anchors[3].X;

	a5 = (m_Anchors[5].Y - m_Anchors[4].Y) / (m_Anchors[5].X - m_Anchors[4].X);
	b5 = m_Anchors[4].Y - a5 * m_Anchors[4].X;
}

void PiecewiseLinear5Function::SetDefaultAnchors()
{
	m_Anchors[1].X = m_Domain.left + m_Domain.Width() / 5;
	m_Anchors[2].X = m_Domain.left + m_Domain.Width() * 2 / 5;
	m_Anchors[3].X = m_Domain.left + m_Domain.Width() * 3 / 5;
	m_Anchors[4].X = m_Domain.left + m_Domain.Width() * 4 / 5;
	if (fBenefit())
	{
		m_Anchors[0].Y = m_Domain.bottom;
		m_Anchors[1].Y = m_Domain.bottom + m_Domain.Height() / 5;
		m_Anchors[2].Y = m_Domain.bottom + m_Domain.Height() * 2 / 5;
		m_Anchors[3].Y = m_Domain.bottom + m_Domain.Height() * 3 / 5;
		m_Anchors[4].Y = m_Domain.bottom + m_Domain.Height() * 4 / 5;
		m_Anchors[5].Y = m_Domain.top;
	}
	else
	{
		m_Anchors[0].Y = m_Domain.top;
		m_Anchors[1].Y = m_Domain.bottom + m_Domain.Height() * 4 / 5;
		m_Anchors[2].Y = m_Domain.bottom + m_Domain.Height() * 3 / 5;
		m_Anchors[3].Y = m_Domain.bottom + m_Domain.Height() * 2 / 5;
		m_Anchors[4].Y = m_Domain.bottom + m_Domain.Height() / 5;
		m_Anchors[5].Y = m_Domain.bottom;
	}
	SolveParams();
}

String PiecewiseLinear5Function::sGetFx(String sData)
{
	SolveParams();
	//return a * x + b;
	return String ("iff(%S<%lg,%lg*%S+%lg,iff(%S<%lg,%lg*%S+%lg,iff(%S<%lg,%lg*%S+%lg,iff(%S<%lg,%lg*%S+%lg,%lg*%S+%lg))))", sData, m_Anchors[1].X, a1, sData, b1, sData, m_Anchors[2].X, a2, sData, b2, sData, m_Anchors[3].X, a3, sData, b3, sData, m_Anchors[4].X, a4, sData, b4, a5, sData, b5);
}

String PiecewiseLinear5Function::sDescription()
{
	return String("%S(%.3f,%.3f)", String("PiecewiseLinear5"), m_Anchors[0].X, m_Anchors[5].X);
}
