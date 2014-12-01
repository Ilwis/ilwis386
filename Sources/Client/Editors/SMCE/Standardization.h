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
// Standardization.h: interface for the Standardization class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STANDARDIZATION_H__4BEFC004_B0F8_429A_9674_29DE323B896E__INCLUDED_)
#define AFX_STANDARDIZATION_H__4BEFC004_B0F8_429A_9674_29DE323B896E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Effect;
class Evaluation;

class Standardization
{
public:
	Standardization(Effect* peOwner);
	virtual ~Standardization();
	virtual String sStandardize(String sData) = 0; // this does the real work
	virtual void ShowForm() = 0; // pops a form
	bool fStandardized();
	bool fFormAllowed();
	static Standardization* create(Effect* peOwner);
	virtual Standardization* copy(Effect* peOwner) = 0;
	virtual int iCostBenefit() = 0; // 0 == benefir, 1 == cost, 2 == none
	virtual void WriteElements(const char* sSection, const ElementContainer& en);
	virtual void ReadElements(const char* sSection, const ElementContainer& en);
	void SetModifiedFlag(BOOL bModified = TRUE);
	virtual String sDescription() = 0;

	enum eCostBenefitTP {iBENEFIT, iCOST};

	static String sCostBenefitTypeToString(eCostBenefitTP e)
	{
		switch(e)
		{
		case iBENEFIT:
			return "Benefit";
			break;
		case iCOST:
			return "Cost";
			break;
		default:
			return "";
		}
	};

	static eCostBenefitTP eCostBenefitTypeFromString(String s)
	{
		if ("Benefit" == s)
			return iBENEFIT;
		else if ("Cost" == s)
			return iCOST;
		else
			return iBENEFIT; // wrong!!
	};

protected:
	Effect* m_pOwnerEffect;
	bool m_fStandardized;
	bool m_fFormAllowed;
	static const ValueRange vrStd;
};

class StdValueMethod;
class StandardizationValue : public Standardization
{
protected:
	StandardizationValue(Effect* peOwner);
	virtual ~StandardizationValue();
public:
	enum eStdValueMethodTP{iMAXIMUM, iINTERVAL, iGOAL, iCONVEX, iCONCAVE, iUSHAPE, iGAUSSIAN, iPIECEWISELINEAR, iPIECEWISELINEAR5}; // VALUE standardization

	void ShowForm();
	static Standardization* create(Effect* peOwner);
	Standardization* copy(Effect* peOwner);
	virtual int iCostBenefit();
	String sStandardize(String sData);
	void WriteElements(const char* sSection, const ElementContainer& en);
	void ReadElements(const char* sSection, const ElementContainer& en);
	String sDescription();

private:
	eStdValueMethodTP eStdValueMethod;
	eCostBenefitTP eCostBenefit;

	static String sStdValueMethodToString(eStdValueMethodTP e)
	{
		switch(e)
		{
		case iMAXIMUM:
			return "Maximum";
			break;
		case iINTERVAL:
			return "Interval";
			break;
		case iGOAL:
			return "Goal";
			break;
		case iCONVEX:
			return "Convex";
			break;
		case iCONCAVE:
			return "Concave";
			break;
		case iUSHAPE:
			return "UShape";
			break;
		case iGAUSSIAN:
			return "Gaussian";
			break;
		case iPIECEWISELINEAR:
			return "PiecewiseLinear";
			break;
		case iPIECEWISELINEAR5:
			return "PiecewiseLinear5";
			break;
		default:
			return "";
		}
	};

	static eStdValueMethodTP eStdValueMethodFromString(String s)
	{
		if ("Maximum" == s)
			return iMAXIMUM;
		else if ("Interval" == s)
			return iINTERVAL;
		else if ("Goal" == s)
			return iGOAL;
		else if ("Convex" == s)
			return iCONVEX;
		else if ("Concave" == s)
			return iCONCAVE;
		else if ("UShape" == s)
			return iUSHAPE;
		else if ("Gaussian" == s)
			return iGAUSSIAN;
		else if ("PiecewiseLinear" == s)
			return iPIECEWISELINEAR;
		else if ("PiecewiseLinear5" == s)
			return iPIECEWISELINEAR5;
		else
			return iMAXIMUM; // wrong!!
	};

	StdValueMethod* stdMethod;
};

class SmceFunction;

class StdValueMethod
{
public:
	StdValueMethod(Effect* peOwner, SmceFunction * pFunction, StandardizationValue::eCostBenefitTP _eCostBenefit);
	virtual ~StdValueMethod();
	virtual String sStandardize(String sData) = 0;
	virtual StdValueMethod* copy(Effect* peOwner) = 0;
	virtual void WriteElements(const char* sSection, const ElementContainer& en) = 0;
	virtual void ReadElements(const char* sSection, const ElementContainer& en) = 0;
	virtual String sDescription() = 0;
	SmceFunction ** ppFunction();

protected:
	Effect* m_pOwnerEffect;
	SmceFunction * m_pFunction;
	StandardizationValue::eCostBenefitTP eCostBenefit;
	String sTotalMinMax(bool fMax);
	String sMinMaxCorrector(vector <String> vsTerms, bool fMax);
};

class StdValueMaximum : public StdValueMethod
{
protected:
	StdValueMaximum(Effect* peOwner, StandardizationValue::eCostBenefitTP _eCostBenefit);
	virtual ~StdValueMaximum();
public:
	static StdValueMethod* create(Effect* peOwner, StandardizationValue::eCostBenefitTP _eCostBenefit);
	StdValueMethod* copy(Effect* peOwner);
	String sStandardize(String sData);
	void WriteElements(const char* sSection, const ElementContainer& en);
	void ReadElements(const char* sSection, const ElementContainer& en);
	String sDescription();
};

class StdValueInterval : public StdValueMethod
{
protected:
	StdValueInterval(Effect* peOwner, StandardizationValue::eCostBenefitTP _eCostBenefit);
	virtual ~StdValueInterval();
public:
	static StdValueMethod* create(Effect* peOwner, StandardizationValue::eCostBenefitTP _eCostBenefit);
	StdValueMethod* copy(Effect* peOwner);
	String sStandardize(String sData);
	void WriteElements(const char* sSection, const ElementContainer& en);
	void ReadElements(const char* sSection, const ElementContainer& en);
	String sDescription();
};

class StdValueGeneral : public StdValueMethod
{
protected:
	StdValueGeneral(Effect* peOwner, StandardizationValue::eStdValueMethodTP _eStdValueMethod, StandardizationValue::eCostBenefitTP _eCostBenefit, SmceFunction * pFunction);
	virtual ~StdValueGeneral();
public:
	static StdValueMethod* create(Effect* peOwner, StandardizationValue::eStdValueMethodTP _eStdValueMethod, StandardizationValue::eCostBenefitTP _eCostBenefit, SmceFunction * pFunction);
	StdValueMethod* copy(Effect* peOwner);
	String sStandardize(String sData);
	void WriteElements(const char* sSection, const ElementContainer& en);
	void ReadElements(const char* sSection, const ElementContainer& en);
	String sDescription();
private:
	StandardizationValue::eStdValueMethodTP eStdValueMethod;
};

class StandardizationValueConstraint : public Standardization
{
protected:
	StandardizationValueConstraint(Effect* peOwner);
	virtual ~StandardizationValueConstraint();
public:
	void ShowForm();
	static Standardization* create(Effect* peOwner);
	Standardization* copy(Effect* peOwner);
	virtual int iCostBenefit();
	String sStandardize(String sData);
	void WriteElements(const char* sSection, const ElementContainer& en);
	void ReadElements(const char* sSection, const ElementContainer& en);
	String sDescription();

private:
	double rMin, rMax;
	bool fMin, fMax;
};

class StandardizationClass : public Standardization
{
protected:
	StandardizationClass(Effect* peOwner);
	virtual ~StandardizationClass();
public:
	void ShowForm();
	static Standardization* create(Effect* peOwner);
	Standardization* copy(Effect* peOwner);
	virtual int iCostBenefit();
	String sStandardize(String sData);
	void WriteElements(const char* sSection, const ElementContainer& en);
	void ReadElements(const char* sSection, const ElementContainer& en);
	String sDescription();
private:
	String sAttribCol;
	Evaluation* pWeights;
};

class StandardizationBool : public Standardization
{
protected:
	StandardizationBool(Effect* peOwner);
	virtual ~StandardizationBool();
public:
	void ShowForm();
	static Standardization* create(Effect* peOwner);
	Standardization* copy(Effect* peOwner);
	virtual int iCostBenefit();
	String sStandardize(String sData);
	void WriteElements(const char* sSection, const ElementContainer& en);
	void ReadElements(const char* sSection, const ElementContainer& en);
	String sDescription();
private:
	double rTrue, rFalse; // sufficient for BOOL standardization
};

class StandardizationBoolConstraint : public Standardization
{
protected:
	StandardizationBoolConstraint(Effect* peOwner);
	virtual ~StandardizationBoolConstraint();
public:
	void ShowForm();
	static Standardization* create(Effect* peOwner);
	Standardization* copy(Effect* peOwner);
	virtual int iCostBenefit();
	String sStandardize(String sData);
	void WriteElements(const char* sSection, const ElementContainer& en);
	void ReadElements(const char* sSection, const ElementContainer& en);
	String sDescription();
private:
	eCostBenefitTP m_eCostBenefit;
};

#endif // !defined(AFX_STANDARDIZATION_H__4BEFC004_B0F8_429A_9674_29DE323B896E__INCLUDED_)
