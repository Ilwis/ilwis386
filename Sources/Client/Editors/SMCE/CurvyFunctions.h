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
// CurvyFunctions.h: interface for the CurvyFunctions class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CURVYFUNCTIONS_H__EB1B1AA2_72C4_4A35_AF08_AE1303A41DFE__INCLUDED_)
#define AFX_CURVYFUNCTIONS_H__EB1B1AA2_72C4_4A35_AF08_AE1303A41DFE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Client\Editors\SMCE\SmceFunction.h"

class ConvexFunction : public SmceFunction  
{
public:
	ConvexFunction(const DoubleRect & drDomain, bool fBenefit);
	virtual ~ConvexFunction();
	virtual double rGetFx(double x) const;
	virtual void SetDefaultAnchors();
	virtual String sGetFx(String sData);
	virtual String sDescription();
protected:
	virtual void SolveParams();
private:
	double a;
	double b;
	double c; // y = a + be^cx
};

class ConcaveFunction : public SmceFunction  
{
public:
	ConcaveFunction(const DoubleRect & drDomain, bool fBenefit);
	virtual ~ConcaveFunction();
	virtual double rGetFx(double x) const;
	virtual void SetDefaultAnchors();
	virtual String sGetFx(String sData);
	virtual String sDescription();
protected:
	virtual void SolveParams();
private:
	double a;
	double b;
	double c; // y = 1 - (a + be^cx)
};

class QuadraticFunction : public SmceFunction  
{
public:
	QuadraticFunction(const DoubleRect & drDomain, bool fBenefit);
	virtual ~QuadraticFunction();
	virtual double rGetFx(double x) const;
	virtual void SetDefaultAnchors();
	virtual String sGetFx(String sData);
	virtual String sDescription();
protected:
	virtual void SolveParams();
private:
	double a;
	double b;
	double c; // y = ax^2 + bx + c
};

class GaussianFunction : public SmceFunction  
{
public:
	GaussianFunction(const DoubleRect & drDomain, bool fBenefit);
	virtual ~GaussianFunction();
	virtual double rGetFx(double x) const;
	virtual void SetDefaultAnchors();
	virtual String sGetFx(String sData);
	virtual String sDescription();
protected:
	virtual void SolveParams();
private:
	double a;
	double b;
	double c; // y = a * e^(b(x-c))
};

#endif // !defined(AFX_CURVYFUNCTIONS_H__EB1B1AA2_72C4_4A35_AF08_AE1303A41DFE__INCLUDED_)
