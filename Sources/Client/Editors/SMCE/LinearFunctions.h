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

#if !defined(AFX_LINEFUNCTION_H__B90CBC59_5D00_439A_96A8_E53430122F30__INCLUDED_)
#define AFX_LINEFUNCTION_H__B90CBC59_5D00_439A_96A8_E53430122F30__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Client\Editors\SMCE\SmceFunction.h"

class MaximumFunction : public SmceFunction
{
public:
	MaximumFunction(const DoubleRect & drDomain, bool fBenefit);
	virtual ~MaximumFunction();
	virtual double rGetFx(double x) const;
	virtual void SetDefaultAnchors();
	virtual String sGetFx(String sData);
	virtual String sDescription();
protected:
	virtual void SolveParams();
};

class IntervalFunction : public SmceFunction
{
public:
	IntervalFunction(const DoubleRect & drDomain, bool fBenefit);
	virtual ~IntervalFunction();
	virtual double rGetFx(double x) const;
	virtual void SetDefaultAnchors();
	virtual String sGetFx(String sData);
	virtual String sDescription();
protected:
	virtual void SolveParams();
};

class GoalFunction : public SmceFunction
{
public:
	GoalFunction(const DoubleRect & drDomain, bool fBenefit);
	virtual ~GoalFunction();
	virtual double rGetFx(double x) const;
	virtual void SetDefaultAnchors();
	virtual String sGetFx(String sData);
	virtual String sDescription();
protected:
	virtual void SolveParams();
private:
	double a;
	double b; // y = ax + b
};

class PiecewiseLinearFunction : public SmceFunction
{
public:
	PiecewiseLinearFunction(const DoubleRect & drDomain, bool fBenefit);
	virtual ~PiecewiseLinearFunction();
	virtual double rGetFx(double x) const;
	virtual void SetDefaultAnchors();
	virtual String sGetFx(String sData);
	virtual String sDescription();
protected:
	virtual void SolveParams();
private:
	double a1;
	double b1; // y = a1x + b1 in first interval
	double a2;
	double b2; // y = a2x + b2 in second interval
	double a3;
	double b3; // y = a3x + b3 in third interval
};

class PiecewiseLinear5Function : public SmceFunction
{
public:
	PiecewiseLinear5Function(const DoubleRect & drDomain, bool fBenefit);
	virtual ~PiecewiseLinear5Function();
	virtual double rGetFx(double x) const;
	virtual void SetDefaultAnchors();
	virtual String sGetFx(String sData);
	virtual String sDescription();
protected:
	virtual void SolveParams();
private:
	double a1;
	double b1; // y = a1x + b1 in first interval
	double a2;
	double b2; // y = a2x + b2 in second interval
	double a3;
	double b3; // y = a3x + b3 in third interval
	double a4;
	double b4; // y = a4x + b4 in fourth interval
	double a5;
	double b5; // y = a5x + b5 in fifth interval
};

#endif // !defined(AFX_LINEFUNCTION_H__B90CBC59_5D00_439A_96A8_E53430122F30__INCLUDED_)
