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
// CurvyFunctions.cpp: implementation of the CurvyFunctions class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Editors\SMCE\CurvyFunctions.h"

#include "Headers\Hs\smce.hs"

//////////////////////////////////////////////////////////////////////
// ConvexFunction
//////////////////////////////////////////////////////////////////////

ConvexFunction::ConvexFunction(const DoubleRect & drDomain, bool fBenefit)
: SmceFunction(3, drDomain, fBenefit)
{

}

ConvexFunction::~ConvexFunction()
{

}

double ConvexFunction::rGetFx(double x) const
{
	double rRet;

	if (fBenefit())
		rRet = a + b * exp(c * x);
	else
		rRet = m_Domain.top - (a + b * exp(c * x));

	if (rRet < m_Domain.bottom)
		rRet = m_Domain.bottom;
	else if (rRet > m_Domain.top)
		rRet = m_Domain.top;

	return rRet;
}

void ConvexFunction::SolveParams()
{
	if (m_Anchors[1].Y > m_Domain.top - m_Domain.Height() / 1000.0)
		m_Anchors[1].Y = m_Domain.top - m_Domain.Height() / 1000.0;
	else if (m_Anchors[1].Y < m_Domain.bottom + m_Domain.Height() / 1000.0)
		m_Anchors[1].Y = m_Domain.bottom + m_Domain.Height() / 1000.0;

	if (fBenefit())
	{
		// constraints
		m_Anchors[0].Y = m_Domain.bottom;
		m_Anchors[2].Y = m_Domain.top;

		a = m_Domain.bottom;
		c = log((m_Anchors[1].Y - a) / (m_Domain.top - a)) / (m_Anchors[1].X - m_Anchors[2].X);
		b = (m_Anchors[1].Y - a) / exp(c * m_Anchors[1].X);

		double rIntermediateSolution = a + b * exp(c * m_Anchors[0].X);
		double rStep = 10;
		int iIterations = 100;
		while (fabs(rIntermediateSolution - m_Domain.bottom) > 0.01 && (--iIterations > 0))
		{
			if (((rIntermediateSolution > m_Domain.bottom) && (rStep > 0)) || ((rIntermediateSolution < m_Domain.bottom) && (rStep < 0)))
				rStep /= -2.0;
			a += rStep;
			c = log((m_Anchors[1].Y - a) / (m_Domain.top - a)) / (m_Anchors[1].X - m_Anchors[2].X);
			b = (m_Anchors[1].Y - a) / exp(c * m_Anchors[1].X);

			rIntermediateSolution = a + b * exp(c * m_Anchors[0].X);
		}
	}
	else
	{
		// constraints
		m_Anchors[0].Y = m_Domain.top;
		m_Anchors[2].Y = m_Domain.bottom;

		a = -m_Domain.top;
		double rExp = (-a) / (m_Domain.top - a - m_Anchors[1].Y);
		if (rExp > 0)
			c = -fabs(log((-a) / (m_Domain.top - a - m_Anchors[1].Y)) / (m_Anchors[0].X - m_Anchors[1].X));
		else
			c = -fabs(log((m_Domain.top - a - m_Domain.bottom) / (m_Domain.top - a - m_Anchors[1].Y)) / (m_Anchors[2].X - m_Anchors[1].X));

		b = (m_Domain.top - a - m_Anchors[1].Y) / exp(c * m_Anchors[1].X);

		double rIntermediateSolution = m_Domain.top - (a + b * exp(c * m_Anchors[2].X));

		double rStep = 10;
		int iIterations = 100;
		while ((fabs(rIntermediateSolution - m_Domain.bottom) > 0.01) && (--iIterations > 0))
		{
			if (((rIntermediateSolution < m_Domain.bottom) && (rStep > 0)) || ((rIntermediateSolution > m_Domain.bottom) && (rStep < 0)))
				rStep /= -2.0;

			a += rStep;
			double rExp = (-a) / (m_Domain.top - a - m_Anchors[1].Y);
			if (rExp > 0)
				c = -fabs(log((-a) / (m_Domain.top - a - m_Anchors[1].Y)) / (m_Anchors[0].X - m_Anchors[1].X));
			else
				c = -fabs(log((m_Domain.top - a - m_Domain.bottom) / (m_Domain.top - a - m_Anchors[1].Y)) / (m_Anchors[2].X - m_Anchors[1].X));
				
			b = (m_Domain.top - a - m_Anchors[1].Y) / exp(c * m_Anchors[1].X);

			rIntermediateSolution = m_Domain.top - (a + b * exp(c * m_Anchors[2].X));
		}

	}
}

void ConvexFunction::SetDefaultAnchors()
{
	if (fBenefit())
	{
		m_Anchors[0].X = m_Domain.left;
		m_Anchors[0].Y = m_Domain.bottom;
		m_Anchors[1].X = m_Domain.left + (m_Domain.Width() / 2);
		m_Anchors[1].Y = m_Domain.bottom + (m_Domain.Height() / 4);
		m_Anchors[2].X = m_Domain.right;
		m_Anchors[2].Y = m_Domain.top;
	}
	else
	{
		m_Anchors[0].X = m_Domain.left;
		m_Anchors[0].Y = m_Domain.top;
		m_Anchors[1].X = m_Domain.left + (m_Domain.Width() / 2);
		m_Anchors[1].Y = m_Domain.bottom + (m_Domain.Height() / 4);
		m_Anchors[2].X = m_Domain.right;
		m_Anchors[2].Y = m_Domain.bottom;
	}
	SolveParams();
}

String ConvexFunction::sGetFx(String sData)
{
	SolveParams();
	//return a + b * exp(c * x) OR m_Domain.top - (a + b * exp(c * x))
	String sRet;
	if (fBenefit())
		sRet = String("%lg+%lg*exp(%lg*%S)", a, b, c, sData);
	else
		sRet = String("%lg-(%lg+%lg*exp(%lg*%S))", m_Domain.top, a, b, c, sData);

	return sRet;
}

String ConvexFunction::sDescription()
{
	return String("%S(%.3f,%.3f)", String("Convex"), m_Anchors[0].X, m_Anchors[2].X);
}

//////////////////////////////////////////////////////////////////////
// ConcaveFunction
//////////////////////////////////////////////////////////////////////

ConcaveFunction::ConcaveFunction(const DoubleRect & drDomain, bool fBenefit)
: SmceFunction(3, drDomain, fBenefit)
{

}

ConcaveFunction::~ConcaveFunction()
{

}

double ConcaveFunction::rGetFx(double x) const
{
	double rRet;

	if (fBenefit())
		rRet = a + b * exp(c * x);
	else
		rRet = m_Domain.top - (a + b * exp(c * x));

	if (rRet < m_Domain.bottom)
		rRet = m_Domain.bottom;
	else if (rRet > m_Domain.top)
		rRet = m_Domain.top;

	return rRet;
}

void ConcaveFunction::SolveParams()
{
	if (m_Anchors[1].Y > m_Domain.top - m_Domain.Height() / 1000.0)
		m_Anchors[1].Y = m_Domain.top - m_Domain.Height() / 1000.0;
	else if (m_Anchors[1].Y < m_Domain.bottom + m_Domain.Height() / 1000.0)
		m_Anchors[1].Y = m_Domain.bottom + m_Domain.Height() / 1000.0;

	if (fBenefit())
	{
		// constraints
		m_Anchors[0].Y = m_Domain.bottom;
		m_Anchors[2].Y = m_Domain.top;

		a = m_Domain.top;
		c = -fabs(log((m_Anchors[1].Y - a) / (m_Domain.bottom - a)) / (m_Anchors[1].X - m_Anchors[0].X));
		b = (m_Anchors[1].Y - a) / exp(c * m_Anchors[1].X);

		double rIntermediateSolution = a + b * exp(c * m_Anchors[2].X);
		double rStep = 10;
		int iIterations = 100;
		while (fabs(rIntermediateSolution - m_Domain.top) > 0.01 && (--iIterations > 0))
		{
			if (((rIntermediateSolution > m_Domain.top) && (rStep > 0)) || ((rIntermediateSolution < m_Domain.top) && (rStep < 0)))
				rStep /= -2.0;
			a += rStep;
			c = -fabs(log((m_Anchors[1].Y - a) / (m_Domain.bottom - a)) / (m_Anchors[1].X - m_Anchors[0].X));
			b = (m_Anchors[1].Y - a) / exp(c * m_Anchors[1].X);

			rIntermediateSolution = a + b * exp(c * m_Anchors[2].X);
		}
	}
	else
	{
		// constraints
		m_Anchors[0].Y = m_Domain.top;
		m_Anchors[2].Y = m_Domain.bottom;

		a = m_Domain.bottom;
		c = fabs(log((m_Domain.top - a - m_Domain.bottom) / (m_Domain.top - a - m_Anchors[1].Y)) / (m_Anchors[2].X - m_Anchors[1].X));
		b = (m_Domain.top - a - m_Anchors[1].Y) / exp(c * m_Anchors[1].X);

		double rIntermediateSolution = m_Domain.top - (a + b * exp(c * m_Anchors[0].X));

		double rStep = 10;
		int iIterations = 100;
		while (fabs(rIntermediateSolution - m_Domain.top) > 0.01 && (--iIterations > 0))
		{
			if (((rIntermediateSolution < m_Domain.top) && (rStep > 0)) || ((rIntermediateSolution > m_Domain.top) && (rStep < 0)))
				rStep /= -2.0;

			a += rStep;
			c = fabs(log((m_Domain.top - a - m_Domain.bottom) / (m_Domain.top - a - m_Anchors[1].Y)) / (m_Anchors[2].X - m_Anchors[1].X));
			b = (m_Domain.top - a - m_Anchors[1].Y) / exp(c * m_Anchors[1].X);

			rIntermediateSolution = m_Domain.top - (a + b * exp(c * m_Anchors[0].X));
		}
	}
}

void ConcaveFunction::SetDefaultAnchors()
{
	if (fBenefit())
	{
		m_Anchors[0].X = m_Domain.left;
		m_Anchors[0].Y = m_Domain.bottom;
		m_Anchors[1].X = m_Domain.left + (m_Domain.Width() / 2);
		m_Anchors[1].Y = m_Domain.bottom + (m_Domain.Height() / 1.5);
		m_Anchors[2].X = m_Domain.right;
		m_Anchors[2].Y = m_Domain.top;
	}
	else
	{
		m_Anchors[0].X = m_Domain.left;
		m_Anchors[0].Y = m_Domain.top;
		m_Anchors[1].X = m_Domain.left + (m_Domain.Width() / 2);
		m_Anchors[1].Y = m_Domain.bottom + (m_Domain.Height() / 1.5);
		m_Anchors[2].X = m_Domain.right;
		m_Anchors[2].Y = m_Domain.bottom;
	}
	SolveParams();
}

String ConcaveFunction::sGetFx(String sData)
{
	SolveParams();
	//return a + b * exp(c * x) OR m_Domain.top - (a + b * exp(c * x))
	String sRet;
	if (fBenefit())
		sRet = String("%lg+%lg*exp(%lg*%S)", a, b, c, sData);
	else
		sRet = String("%lg-(%lg+%lg*exp(%lg*%S))", m_Domain.top, a, b, c, sData);

	return sRet;
}

String ConcaveFunction::sDescription()
{
	return String("%S(%.3f,%.3f)", String("Concave"), m_Anchors[0].X, m_Anchors[2].X);
}

//////////////////////////////////////////////////////////////////////
// QuadraticFunction
//////////////////////////////////////////////////////////////////////

QuadraticFunction::QuadraticFunction(const DoubleRect & drDomain, bool fBenefit)
: SmceFunction(3, drDomain, fBenefit)
{

}

QuadraticFunction::~QuadraticFunction()
{

}

double QuadraticFunction::rGetFx(double x) const
{
	double rRet;

	rRet = a * x * x + b * x + c;

	if (rRet < m_Domain.bottom)
		rRet = m_Domain.bottom;
	else if (rRet > m_Domain.top)
		rRet = m_Domain.top;

	return rRet;
}

void QuadraticFunction::SolveParams()
{
	// constraints
	if (fBenefit())
	{
		m_Anchors[0].Y = m_Domain.bottom;
		m_Anchors[2].Y = m_Domain.bottom;
	}
	else
	{
		// constraints
		m_Anchors[0].Y = m_Domain.top;
		m_Anchors[2].Y = m_Domain.top;
	}
	double x0 = m_Anchors[0].X;
	double y0 = m_Anchors[0].Y;
	double x1 = m_Anchors[1].X;
	double y1 = m_Anchors[1].Y;
	double x2 = m_Anchors[2].X;
	double y2 = m_Anchors[2].Y;
	double teller = (y2-y0)*(x1-x0)-(y1-y0)*(x2-x0);
	double noemer = (x2*x2-x0*x0)*(x1-x0)-(x1*x1-x0*x0)*(x2-x0);
	a = teller / noemer;
	b = (y1-y0-a*(x1*x1-x0*x0))/(x1-x0);
	c = y0-a*x0*x0-b*x0;
}

void QuadraticFunction::SetDefaultAnchors()
{
	if (fBenefit())
	{
		m_Anchors[0].X = m_Domain.left;
		m_Anchors[0].Y = m_Domain.bottom;
		m_Anchors[1].X = m_Domain.left + (m_Domain.Width() / 2);
		m_Anchors[1].Y = m_Domain.top;
		m_Anchors[2].X = m_Domain.right;
		m_Anchors[2].Y = m_Domain.bottom;
	}
	else
	{
		m_Anchors[0].X = m_Domain.left;
		m_Anchors[0].Y = m_Domain.top;
		m_Anchors[1].X = m_Domain.left + (m_Domain.Width() / 2);
		m_Anchors[1].Y = m_Domain.bottom;
		m_Anchors[2].X = m_Domain.right;
		m_Anchors[2].Y = m_Domain.top;
	}
	SolveParams();
}

String QuadraticFunction::sGetFx(String sData)
{
	SolveParams();
	// return a * x * x + b * x + c
	// String sRet ("%lg*%S*%S+%lg*%S+%lg", a, sData, sData, b, sData, c);
	// return (a * x + b) * x + c (this is optimized so that sData is used twice instead of 3x)
	String sRet ("(%lg*%S+%lg)*%S+%lg", a, sData, b, sData, c);

	return sRet;
}

String QuadraticFunction::sDescription()
{
	return String("%S%s(%.3f,%.3f)", String("U-Shape"), fBenefit()?"+":"-", m_Anchors[0].X, m_Anchors[2].X);
}

//////////////////////////////////////////////////////////////////////
// GaussianFunction
//////////////////////////////////////////////////////////////////////

GaussianFunction::GaussianFunction(const DoubleRect & drDomain, bool fBenefit)
: SmceFunction(3, drDomain, fBenefit)
{

}

GaussianFunction::~GaussianFunction()
{

}

double GaussianFunction::rGetFx(double x) const
{
	double rRet;

	if (fBenefit())
		rRet = a * exp(b * pow(x - c, 2));
	else
		rRet = m_Domain.top - a * exp(b * pow(x - c, 2));

	if (rRet < m_Domain.bottom)
		rRet = m_Domain.bottom;
	else if (rRet > m_Domain.top)
		rRet = m_Domain.top;

	return rRet;
}

void GaussianFunction::SolveParams()
{
	if (fBenefit())
	{
		m_Anchors[1].X = (m_Anchors[0].X + m_Anchors[2].X) / 2;
		m_Anchors[0].Y = m_Domain.bottom + m_Domain.Height() / 1000;
		m_Anchors[2].Y = m_Domain.bottom + m_Domain.Height() / 1000;
		if (m_Anchors[1].Y < m_Domain.bottom + m_Domain.Height() / 500)
			m_Anchors[1].Y = m_Domain.bottom + m_Domain.Height() / 500;
	}
	else
	{
		m_Anchors[1].X = (m_Anchors[0].X + m_Anchors[2].X) / 2;
		m_Anchors[0].Y = m_Domain.top - m_Domain.Height() / 1000;
		m_Anchors[2].Y = m_Domain.top - m_Domain.Height() / 1000;
		if (m_Anchors[1].Y > m_Domain.top - m_Domain.Height() / 500)
			m_Anchors[1].Y = m_Domain.top - m_Domain.Height() / 500;
	}
	double x1 = m_Anchors[1].X;
	double y1 = m_Anchors[1].Y;
	double x2 = m_Anchors[2].X;
	double y2 = m_Anchors[2].Y;
	if (fBenefit())
	{
		a = y1;
		c = x1;
		b = log(y2 / y1) / pow(x2 - c, 2);
	}
	else
	{
		a = m_Domain.top - y1;
		c = x1;
		b = log((m_Domain.top - y2) / (m_Domain.top - y1)) / pow(x2 - c, 2);
	}
}

void GaussianFunction::SetDefaultAnchors()
{
	if (fBenefit())
	{
		m_Anchors[0].X = m_Domain.left;
		m_Anchors[1].X = m_Domain.left + (m_Domain.Width() / 2);
		m_Anchors[1].Y = m_Domain.top;
		m_Anchors[2].X = m_Domain.right;
	}
	else
	{
		m_Anchors[0].X = m_Domain.left;
		m_Anchors[1].X = m_Domain.left + (m_Domain.Width() / 2);
		m_Anchors[1].Y = m_Domain.bottom;
		m_Anchors[2].X = m_Domain.right;
	}
	SolveParams();
}

String GaussianFunction::sGetFx(String sData)
{
	SolveParams();
	// return a * exp(b * sq(x - c)) OR 1 - a * exp(b * sq(x - c))
	String sRet;
	if (fBenefit())
		sRet = String("%lg*exp(%lg*sq(%S-%lg))", a, b, sData, c);
	else
		sRet = String("%lg-%lg*exp(%lg*sq(%S-%lg))", m_Domain.top, a, b, sData, c);

	return sRet;
}

String GaussianFunction::sDescription()
{
	return String("%S%s(%.3f,%.3f)", String("Gaussian"), fBenefit()?"+":"-", m_Anchors[0].X, m_Anchors[2].X);
}
