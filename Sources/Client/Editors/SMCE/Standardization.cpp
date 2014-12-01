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
// Standardization.cpp: implementation of the Standardization class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Client\Editors\SMCE\Standardization.h"
#include "Client\Editors\SMCE\CriteriaTreeItem.h"
#include "Client\Editors\SMCE\CriteriaTreeDoc.h" // for the wndGetActiveView
#include "Client\Editors\SMCE\Evaluation.h"
#include "Client\Editors\SMCE\LinearFunctions.h"
#include "Client\Editors\SMCE\CurvyFunctions.h"
#include "Client\FormElements\fldtbl.h" // For FieldTable
#include "Client\FormElements\fldcol.h" // For FieldColumn
#include "Client\FormElements\FieldMultiObjectSelect.h" // For AttributeFileName
#include "Client\FormElements\FieldGraph.h" // For FieldGraph
#include "Engine\Domain\dmsort.h" // for DomainSort
#include "Engine\Table\tblview.h" // for TableView
#include "Client\ilwis.h" // for IlwWinApp
#include "Headers\messages.h" // for ILWM_EXECUTE
#include "Headers\Hs\Mainwind.hs" // for ErrAttrTable
#include "Headers\Hs\Table.hs" // for TR("Could not create column (not enough memory)")
#include "Headers\Hs\smce.hs"
#include "Headers\Htp\Ilwismen.htp"

//////////////////////////////////////////////////////////////////////
// class Standardization
// Usage:
// Start by defining an object of "StandardizationNone"
// Call SubForm to insert the forms in a partial form
// If OK is clicked, use "create" to create a new object,
// and put the new in the place of your previous std member (if any)
// After that, sStandardize() returns the values needed
//////////////////////////////////////////////////////////////////////

const ValueRange Standardization::vrStd = ValueRange(0, 1, 0.001);

Standardization::Standardization(Effect* peOwner)
: m_pOwnerEffect(peOwner)
, m_fStandardized(false)
, m_fFormAllowed(true)
{
}

Standardization::~Standardization()
{
}

Standardization* Standardization::create(Effect* peOwner)
{
	Standardization* stdReturn = 0;

	if (peOwner)
	{
		if (peOwner->fIsConstraint())
		{
			switch(peOwner->eOutputValue())
			{
				case Effect::iVALUE:
					stdReturn = StandardizationValueConstraint::create(peOwner);
					break;
				case Effect::iCLASS:
					stdReturn = StandardizationClass::create(peOwner);
					break;
				case Effect::iBOOL:
					stdReturn = StandardizationBoolConstraint::create(peOwner);
					break;
			}
		}
		else // factor
		{
			switch(peOwner->eOutputValue())
			{
				case Effect::iVALUE:
					stdReturn = StandardizationValue::create(peOwner);
					break;
				case Effect::iCLASS:
					stdReturn = StandardizationClass::create(peOwner);
					break;
				case Effect::iBOOL:
					stdReturn = StandardizationBool::create(peOwner);
					break;
			}
		}
	}

	return stdReturn;
}

bool Standardization::fStandardized()
{
	return m_fStandardized;
}

bool Standardization::fFormAllowed()
{
	return m_fFormAllowed;
}

void Standardization::WriteElements(const char* sSection, const ElementContainer& en)
{
	ObjectInfo::WriteElement(sSection, "Standardized", en, m_fStandardized);
	ObjectInfo::WriteElement(sSection, "FormAllowed", en, m_fFormAllowed);
}

void Standardization::ReadElements(const char* sSection, const ElementContainer& en)
{
	// some checking is important here, otherwise the user may not be able to standardize with a corrupt ODF
	
	if (0 == ObjectInfo::ReadElement(sSection, "Standardized", en, m_fStandardized))
		m_fStandardized = false;

	bool fAllowed = true; // just in case
	if (0 != ObjectInfo::ReadElement(sSection, "FormAllowed", en, fAllowed))
		m_fFormAllowed = fAllowed; // if not available in ODF, leave as it is
}

void Standardization::SetModifiedFlag(BOOL bModified)
{
	if (m_pOwnerEffect)
	{
		m_pOwnerEffect->SetModifiedFlag(bModified);
		m_pOwnerEffect->RecursivelyDeleteOutputMaps();
	}
}

//////////////////////////////////////////////////////////////////
// StdValueMethodToInt
//////////////////////////////////////////////////////////////////

void StdValueMethodToInt(StandardizationValue::eStdValueMethodTP eMethod, StandardizationValue::eCostBenefitTP eCostBenefit, int & iMethod, int & iCostBenefit)
{
	switch (eMethod)
	{
		case StandardizationValue::iMAXIMUM:
			iMethod = 0;
			iCostBenefit = (eCostBenefit==StandardizationValue::iBENEFIT)?0:1;
			break;
		case StandardizationValue::iINTERVAL:
			iMethod = 1;
			iCostBenefit = (eCostBenefit==StandardizationValue::iBENEFIT)?0:1;
			break;
		case StandardizationValue::iGOAL:
			iMethod = 2;
			iCostBenefit = (eCostBenefit==StandardizationValue::iBENEFIT)?0:1;
			break;
		case StandardizationValue::iCONVEX:
			iMethod = 3;
			iCostBenefit = (eCostBenefit==StandardizationValue::iBENEFIT)?0:1;
			break;
		case StandardizationValue::iCONCAVE:
			iMethod = 4;
			iCostBenefit = (eCostBenefit==StandardizationValue::iBENEFIT)?0:1;
			break;
		case StandardizationValue::iUSHAPE:
			iMethod = (eCostBenefit==StandardizationValue::iBENEFIT)?5:6;
			iCostBenefit = 2;
			break;
		case StandardizationValue::iGAUSSIAN:
			iMethod = (eCostBenefit==StandardizationValue::iBENEFIT)?7:8;
			iCostBenefit = 2;
			break;
		case StandardizationValue::iPIECEWISELINEAR:
			iMethod = 9;
			iCostBenefit = 2;
			break;
		case StandardizationValue::iPIECEWISELINEAR5:
			iMethod = 10;
			iCostBenefit = 2;
			break;
	}
}

//////////////////////////////////////////////////////////////////
// IntToStdValueMethod
//////////////////////////////////////////////////////////////////

void IntToStdValueMethod(int iMethod, int iCostBenefit, StandardizationValue::eStdValueMethodTP & eMethod, StandardizationValue::eCostBenefitTP & eCostBenefit)
{
	switch (iMethod)
	{
		case 0:
			eMethod = StandardizationValue::iMAXIMUM;
			eCostBenefit = (iCostBenefit==0)?StandardizationValue::iBENEFIT:StandardizationValue::iCOST;
			break;
		case 1:
			eMethod = StandardizationValue::iINTERVAL;
			eCostBenefit = (iCostBenefit==0)?StandardizationValue::iBENEFIT:StandardizationValue::iCOST;
			break;
		case 2:
			eMethod = StandardizationValue::iGOAL;
			eCostBenefit = (iCostBenefit==0)?StandardizationValue::iBENEFIT:StandardizationValue::iCOST;
			break;
		case 3:
			eMethod = StandardizationValue::iCONVEX;
			eCostBenefit = (iCostBenefit==0)?StandardizationValue::iBENEFIT:StandardizationValue::iCOST;
			break;
		case 4:
			eMethod = StandardizationValue::iCONCAVE;
			eCostBenefit = (iCostBenefit==0)?StandardizationValue::iBENEFIT:StandardizationValue::iCOST;
			break;
		case 5:
			eMethod = StandardizationValue::iUSHAPE;
			eCostBenefit = StandardizationValue::iBENEFIT;
			break;
		case 6:
			eMethod = StandardizationValue::iUSHAPE;
			eCostBenefit = StandardizationValue::iCOST;
			break;
		case 7:
			eMethod = StandardizationValue::iGAUSSIAN;
			eCostBenefit = StandardizationValue::iBENEFIT;
			break;
		case 8:
			eMethod = StandardizationValue::iGAUSSIAN;
			eCostBenefit = StandardizationValue::iCOST;
			break;
		case 9:
			eMethod = StandardizationValue::iPIECEWISELINEAR;
			eCostBenefit = StandardizationValue::iBENEFIT;
			break;
		case 10:
			eMethod = StandardizationValue::iPIECEWISELINEAR5;
			eCostBenefit = StandardizationValue::iBENEFIT;
			break;
	}
}

//////////////////////////////////////////////////////////////////
// pGetNewFunction
//////////////////////////////////////////////////////////////////

SmceFunction* pGetNewFunction(StandardizationValue::eStdValueMethodTP eStdValueMethod, DoubleRect drFunctionDomain, vector<DoublePoint> * pvAnchors, StandardizationValue::eCostBenefitTP eCostBenefit)
{
	SmceFunction * pNewFunction = 0;

	bool fBenefit = eCostBenefit != StandardizationValue::iCOST;
	
	switch (eStdValueMethod)
	{
		case StandardizationValue::iMAXIMUM:
			pNewFunction = new MaximumFunction(drFunctionDomain, fBenefit);
			break;
		case StandardizationValue::iINTERVAL:
			pNewFunction = new IntervalFunction(drFunctionDomain, fBenefit);
			break;
		case StandardizationValue::iGOAL:
			pNewFunction = new GoalFunction(drFunctionDomain, fBenefit);
			break;
		case StandardizationValue::iCONVEX:
			pNewFunction = new ConvexFunction(drFunctionDomain, fBenefit);
			break;
		case StandardizationValue::iCONCAVE:
			pNewFunction = new ConcaveFunction(drFunctionDomain, fBenefit);
			break;
		case StandardizationValue::iUSHAPE:
			pNewFunction = new QuadraticFunction(drFunctionDomain, fBenefit);
			break;
		case StandardizationValue::iGAUSSIAN:
			pNewFunction = new GaussianFunction(drFunctionDomain, fBenefit);
			break;
		case StandardizationValue::iPIECEWISELINEAR:
			pNewFunction = new PiecewiseLinearFunction(drFunctionDomain, fBenefit);
			break;
		case StandardizationValue::iPIECEWISELINEAR5:
			pNewFunction = new PiecewiseLinear5Function(drFunctionDomain, fBenefit);
			break;
	}

	if (pNewFunction)
	{
		if (pvAnchors)
		{
			if (pvAnchors->size() == pNewFunction->vAnchors().size())
				pNewFunction->SetAnchors(*pvAnchors);
			else
			{
				// preserve min/max
				pNewFunction->SetDefaultAnchors();
				vector<DoublePoint> vAnchors = pNewFunction->vAnchors();
				if (vAnchors.size() >= 2 && pvAnchors->size() >= 2)
				{
					vAnchors[0] = (*pvAnchors)[0];
					vAnchors[vAnchors.size() - 1] = (*pvAnchors)[pvAnchors->size() - 1];
					pNewFunction->SetAnchors(vAnchors);
				}
			}
		}
		else
			pNewFunction->SetDefaultAnchors();
	}

	return pNewFunction;
}

//////////////////////////////////////////////////////////////////
// StandardizationValue
//////////////////////////////////////////////////////////////////

StandardizationValue::StandardizationValue(Effect* peOwner)
: Standardization(peOwner)
, stdMethod(0)
, eStdValueMethod(iMAXIMUM)
, eCostBenefit(iBENEFIT)
{

}

StandardizationValue::~StandardizationValue()
{
	if (stdMethod)
		delete stdMethod;
}

Standardization* StandardizationValue::create(Effect* peOwner)
{
	return new StandardizationValue(peOwner);
}

Standardization* StandardizationValue::copy(Effect* peOwner)
{
	StandardizationValue* stdReturn = new StandardizationValue(peOwner);
	stdReturn->m_fFormAllowed = m_fFormAllowed;
	stdReturn->m_fStandardized = m_fStandardized;
	stdReturn->eStdValueMethod = eStdValueMethod;
	stdReturn->eCostBenefit = eCostBenefit;
	if (stdMethod)
		stdReturn->stdMethod = stdMethod->copy(peOwner);
	return stdReturn;
}

int StandardizationValue::iCostBenefit()
{
	int iMethod;
	int iCostBenefit;
	StdValueMethodToInt(eStdValueMethod, eCostBenefit, iMethod, iCostBenefit);
	return iCostBenefit;
}

#define ANCHORS_IN_FORM 6

class StandardizationValueForm: public FormWithDest
{
public:
	StandardizationValueForm(CWnd* wPar, StandardizationValue::eStdValueMethodTP* eMethod, StandardizationValue::eCostBenefitTP* eCostBenefit, RangeReal rrMinMax, int iNrAlternatives, SmceFunction ** ppFunction)
	: FormWithDest(wPar, TR("Standardize Value Input"))
	, vrPrecision(ValueRange(-LONG_MAX+1, LONG_MAX, 0.001))
	, m_rrMinMax(rrMinMax)
	, m_ppFunction(ppFunction)
	, m_iCurrentStdValueMethod(-1)
	, m_eStdValueMethod(eMethod)
	, m_eCostBenefit(eCostBenefit)
	, m_fInCallBackCostBenefitChanged(false)
	, m_fInCallBackStdMethodChanged(false)
	, m_fInCallBackAnchorChanged(false)
	{
		StdValueMethodToInt(*eMethod, *eCostBenefit, m_iMethod, m_iCostBenefit);
		FieldGroup* fgLeft = new FieldGroup(root);
		FieldGroup* fgRight = new FieldGroup(root);
		fgRight->Align(fgLeft, AL_AFTER);
		if (rrMinMax.fValid())
		{
			String sAlternativesRemark ("");
			if (iNrAlternatives > 1)
				sAlternativesRemark = TR("over all alternatives ");
			StaticText* stMin = new StaticText(fgLeft, String(TR("The minimum %Sis %lg").c_str(), sAlternativesRemark, rrMinMax.rLo()));
			StaticText* stMax = new StaticText(fgLeft, String(TR("The maximum %Sis %lg").c_str(), sAlternativesRemark, rrMinMax.rHi()));
			stMin->SetIndependentPos();
			stMax->SetIndependentPos();
		}
		else
		{
			StaticText* stOne = new StaticText(fgLeft, TR("Problem calculating min/max values."));
			StaticText* stTwo = new StaticText(fgLeft, TR("Maximum and Interval standardization not recommended."));
			stOne->SetIndependentPos();
			stTwo->SetIndependentPos();
		}

		// Cost/Benefit consideration
		rgCostBenefit = new RadioGroup(fgLeft, TR("Consider as:"), &m_iCostBenefit);
		rgCostBenefit->SetCallBack((NotifyProc)&StandardizationValueForm::CallBackCostBenefitChanged);
		// rgCostBenefit->Align(fs, AL_UNDER);
		new RadioButton(rgCostBenefit, TR("Benefit"));
		new RadioButton(rgCostBenefit, TR("Cost"));
		new RadioButton(rgCostBenefit, "Combination");

		StaticText * stMethodLabel = new StaticText(fgLeft, TR("Method"));
		rgStdValueMethod = new RadioGroup(fgLeft, "", &m_iMethod);
		rgStdValueMethod->SetCallBack((NotifyProc)&StandardizationValueForm::CallBackStdMethodChanged);
		rbMaximum = new RadioButton(rgStdValueMethod, TR("Maximum"));
		rbInterval = new RadioButton(rgStdValueMethod, TR("Interval"));
		rbGoal = new RadioButton(rgStdValueMethod, TR("Goal"));
		rbConvex = new RadioButton(rgStdValueMethod, "Convex");
		rbConcave = new RadioButton(rgStdValueMethod, "Concave");
		rbUShape_up = new RadioButton(rgStdValueMethod, "U-Shape, up");
		rbUShape_up->Align(stMethodLabel, AL_UNDER);
		rbUShape_down = new RadioButton(rgStdValueMethod, "U-Shape, down");
		rbGaussian_up = new RadioButton(rgStdValueMethod, "Gaussian, up");
		rbGaussian_down = new RadioButton(rgStdValueMethod, "Gaussian, down");
		rbPiecewiseLinear = new RadioButton(rgStdValueMethod, "Piecewise Linear");
		rbPiecewiseLinear5 = new RadioButton(rgStdValueMethod, "Piecewise Linear5");
		for (int i = 0; i < ANCHORS_IN_FORM; ++i)
		{
			frAnchorX[i] = new FieldReal(fgLeft, String("X%d", i+1), &m_Anchors[i].X, vrPrecision);
			frAnchorX[i]->SetCallBack((NotifyProc)&StandardizationValueForm::CallBackAnchorChangedInForm);
			
			if (i > 0)
				frAnchorX[i]->Align(frAnchorX[i-1], AL_UNDER);
			else
				frAnchorX[i]->Align(rbGoal, AL_AFTER);

			frAnchorY[i] = new FieldReal(fgLeft, String("Y%d", i+1), &m_Anchors[i].Y, vrPrecision);
			frAnchorY[i]->SetCallBack((NotifyProc)&StandardizationValueForm::CallBackAnchorChangedInForm);
			frAnchorY[i]->Align(frAnchorX[i], AL_AFTER);
		}
		fgFunctionGraph = new FieldGraph(fgRight);
		fgFunctionGraph->SetCallBack((NotifyProc)&StandardizationValueForm::CallBackAnchorChangedInGraph);
		fgFunctionGraph->SetWidth(200);
		fgFunctionGraph->SetHeight(300);
		fgFunctionGraph->SetIndependentPos();

		SetMenHelpTopic("ilwismen\\smce_window_standardize_value.htm");

		create();
	}
	int CallBackCostBenefitChanged(Event*)
	{
		if (m_fInCallBackCostBenefitChanged)
			return 0;
		m_fInCallBackCostBenefitChanged = true;

		switch (rgCostBenefit->iVal())
		{
		case 0:
		case 1:
			rbMaximum->Show();
			rbInterval->Show();
			rbGoal->Show();
			rbConvex->Show();
			rbConcave->Show();
			rbUShape_up->Hide();
			rbUShape_down->Hide();
			rbGaussian_up->Hide();
			rbGaussian_down->Hide();
			rbPiecewiseLinear->Hide();
			rbPiecewiseLinear5->Hide();
			
			if (rgStdValueMethod->iVal() > 4)
				rgStdValueMethod->SetVal(0);
			
			if (m_rrMinMax.rLo() < 0)
			{
				rbMaximum->Disable();
				if (rbMaximum->fVal())
					rgStdValueMethod->SetVal(rgStdValueMethod->iVal() + 1); // jump to interval
			}
			else
				rbMaximum->Enable();

			if (m_rrMinMax.rLo() == m_rrMinMax.rHi())
			{
				rbInterval->Disable();
				if (rbInterval->fVal())
					rgStdValueMethod->SetVal(rgStdValueMethod->iVal() + 1); // jump to goal
			}
			else
				rbInterval->Enable();

			break;
		case 2:
			rbMaximum->Hide();
			rbInterval->Hide();
			rbGoal->Hide();
			rbConvex->Hide();
			rbConcave->Hide();
			rbUShape_up->Show();
			rbUShape_down->Show();
			rbGaussian_up->Show();
			rbGaussian_down->Show();
			rbPiecewiseLinear->Show();
			rbPiecewiseLinear5->Show();
			if (rgStdValueMethod->iVal() < 5)
				rgStdValueMethod->SetVal(5);
			break;
		}


		if (rgCostBenefit->iVal() != m_iCostBenefit)
		{
			m_iCostBenefit = rgCostBenefit->iVal();
			RefreshFunction(false);
		}

		ShowHideAnchorEdits();
		EnableDisableOk();
		
		m_fInCallBackCostBenefitChanged = false;
		return 0;
	}
	int CallBackStdMethodChanged(Event*)
	{
		if (m_fInCallBackStdMethodChanged)
			return 0;
		m_fInCallBackStdMethodChanged = true;

		if (rgStdValueMethod->iVal() != m_iCurrentStdValueMethod)
		{
			bool fInit = (m_iCurrentStdValueMethod == -1);
			m_iCurrentStdValueMethod = rgStdValueMethod->iVal();
			RefreshFunction(fInit);
		}

		ShowHideAnchorEdits();
		EnableDisableOk();
		
		m_fInCallBackStdMethodChanged = false;
		return 0;
	}
	void RefreshFunction(bool fInit)
	{
		DoubleRect drFunctionDomain (m_rrMinMax.rLo(), 0, m_rrMinMax.rHi(), 1);
		StandardizationValue::eStdValueMethodTP eMethod;
		StandardizationValue::eCostBenefitTP eCostBenefit;
		IntToStdValueMethod(rgStdValueMethod->iVal(), rgCostBenefit->iVal(), eMethod, eCostBenefit);
		SmceFunction * pNewFunction = 0;
		if (fInit)
		{
			vector<DoublePoint> vAnchors;
			if (m_ppFunction && *m_ppFunction && ((*m_ppFunction)->vAnchors().size() >= 2))
			{
				vAnchors = (*m_ppFunction)->vAnchors();
				//vAnchors.push_back((*m_ppFunction)->vAnchors()[0]);
				//vAnchors.push_back((*m_ppFunction)->vAnchors()[(*m_ppFunction)->vAnchors().size() - 1]);
			}
			pNewFunction = pGetNewFunction(eMethod, drFunctionDomain, &vAnchors, eCostBenefit);
		}
		else
			pNewFunction = pGetNewFunction(eMethod, drFunctionDomain, 0, eCostBenefit);

		fgFunctionGraph->SetFunction(pNewFunction);

		// thread safety

		if (m_ppFunction)
		{
			SmceFunction * funcToDelete = 0;
			
			if (*m_ppFunction)
				funcToDelete = *m_ppFunction;
		
			*m_ppFunction = pNewFunction;

			if (funcToDelete)
				delete funcToDelete;
		}
		else if (pNewFunction) // something is wrong and this is unused .. clean it up here
			delete pNewFunction;
	}
	void ShowHideAnchorEdits()
	{
		m_fInCallBackAnchorChanged = true; // block callbacks of fr...

		int iActiveAnchors = 0;
		if (m_ppFunction && *m_ppFunction)
			iActiveAnchors = (*m_ppFunction)->vAnchors().size();
		int i = 0;
		for (; i < iActiveAnchors; ++i)
		{
			frAnchorX[i]->Show();
			frAnchorX[i]->SetVal((*m_ppFunction)->vAnchors()[i].X);
			frAnchorY[i]->Show();
			frAnchorY[i]->SetVal((*m_ppFunction)->vAnchors()[i].Y);
		}
		// hide the rest
		for (; i < ANCHORS_IN_FORM; ++i)
		{
			frAnchorX[i]->Hide();
			frAnchorY[i]->Hide();
		}
		
		m_fInCallBackAnchorChanged = false; // re-enable callbacks...
	}
	void EnableDisableOk()
	{
		/*
		bool fEnable = rbMaximum->fVal() ||
							 rbInterval->fVal() ||
							((!rbMaximum->fVal()) && (!rbInterval->fVal()) && (frMin->rVal()<frMax->rVal()));
		*/
		bool fEnable = true; // TODO
		if (fEnable)
			EnableOK();
		else    
			DisableOK();
	}
	int CallBackAnchorChangedInForm(Event*)
	{
		if (m_fInCallBackAnchorChanged)
			return 0;
		m_fInCallBackAnchorChanged = true;

		if (m_ppFunction && *m_ppFunction)
		{
			vector <DoublePoint> vAnchors;
			int iActiveAnchors = (*m_ppFunction)->vAnchors().size();
			for (int i = 0; i < iActiveAnchors; ++i)
			{
				DoublePoint dpAnchor (frAnchorX[i]->rVal(), frAnchorY[i]->rVal());
				vAnchors.push_back(dpAnchor);
			}
			if ((*m_ppFunction)->vAnchors().size() == vAnchors.size())
			{
				(*m_ppFunction)->SetAnchors(vAnchors);
				fgFunctionGraph->Replot();
			}
		}

		EnableDisableOk();			

		m_fInCallBackAnchorChanged = false;

		return 0;
	}
	int CallBackAnchorChangedInGraph(Event*)
	{
		if (m_fInCallBackAnchorChanged)
			return 0;
		m_fInCallBackAnchorChanged = true;

		if (m_ppFunction && *m_ppFunction)
		{
			vector <DoublePoint> vAnchors = (*m_ppFunction)->vAnchors();
			int iActiveAnchors = vAnchors.size();
			for (int i = 0; i < iActiveAnchors; ++i)
			{
				frAnchorX[i]->SetVal(vAnchors[i].X);
				frAnchorY[i]->SetVal(vAnchors[i].Y);
			}
		}

		EnableDisableOk();

		m_fInCallBackAnchorChanged = false;

		return 0;
	}
  afx_msg void OnOK()
	{
		FormWithDest::OnOK(); // calls StoreData
		IntToStdValueMethod(m_iMethod, m_iCostBenefit, *m_eStdValueMethod, *m_eCostBenefit);

		if (m_ppFunction && *m_ppFunction)
		{
			vector <DoublePoint> vAnchors;
			int iActiveAnchors = (*m_ppFunction)->vAnchors().size();
			for (int i = 0; i < iActiveAnchors; ++i)
				vAnchors.push_back(m_Anchors[i]);
			if ((*m_ppFunction)->vAnchors().size() == vAnchors.size())
				(*m_ppFunction)->SetAnchors(vAnchors);
		}
	}

private:
	RadioButton * rbMaximum;
	RadioButton * rbInterval;
	RadioButton * rbGoal;
	RadioButton * rbConvex;
	RadioButton * rbConcave;
	RadioButton * rbUShape_up;
	RadioButton * rbUShape_down;
	RadioButton * rbGaussian_up;
	RadioButton * rbGaussian_down;
	RadioButton * rbPiecewiseLinear;
	RadioButton * rbPiecewiseLinear5;
	RadioGroup* rgCostBenefit;
	RadioGroup* rgStdValueMethod;
	FieldGraph* fgFunctionGraph;

	bool m_fInCallBackCostBenefitChanged;
	bool m_fInCallBackStdMethodChanged;
	bool m_fInCallBackAnchorChanged;

	SmceFunction ** m_ppFunction;
	int m_iCurrentStdValueMethod;
	int m_iCostBenefit;
	int m_iMethod;
	StandardizationValue::eStdValueMethodTP * m_eStdValueMethod;
	StandardizationValue::eCostBenefitTP * m_eCostBenefit;

	RangeReal m_rrMinMax;
	ValueRange vrPrecision;
	
	// space for anchor points in form
	DoublePoint m_Anchors [ANCHORS_IN_FORM];
	FieldReal* frAnchorX [ANCHORS_IN_FORM];
	FieldReal* frAnchorY [ANCHORS_IN_FORM];
};

void StandardizationValue::ShowForm()
{
	CWnd* wnd = 0;
	RangeReal rrMinMax = RangeReal();
	int iNrAlternatives = 1;
	if (m_pOwnerEffect != 0)
	{
		iNrAlternatives = m_pOwnerEffect->iGetNrAlternatives();
		CWaitCursor cwait;
		for (int i=1; i<=iNrAlternatives; ++i)
		{
			if (1==i) // first time unconditional
				rrMinMax = m_pOwnerEffect->rrMinMax(i);
			else
				rrMinMax = RangeReal(min(rrMinMax.rLo(), m_pOwnerEffect->rrMinMax(i).rLo()),max(rrMinMax.rHi(), m_pOwnerEffect->rrMinMax(i).rHi()));
		}

		wnd = m_pOwnerEffect->GetDocument()->wndGetActiveView();
	}
	DoubleRect drFunctionDomain (rrMinMax.rLo(), 0, rrMinMax.rHi(), 1);
	vector<DoublePoint> vAnchors;
	if (stdMethod && stdMethod->ppFunction() && *(stdMethod->ppFunction()))
		vAnchors = (*(stdMethod->ppFunction()))->vAnchors();
	SmceFunction * pFunction = pGetNewFunction(eStdValueMethod, drFunctionDomain, &vAnchors, eCostBenefit);
	StandardizationValueForm frm(wnd, &eStdValueMethod, &eCostBenefit, rrMinMax, iNrAlternatives, &pFunction);
	if (frm.fOkClicked())
	{
		m_fStandardized = true;

		if (stdMethod)
			delete stdMethod; // this also deletes ppFunction
		stdMethod = 0;

		switch (eStdValueMethod)
		{
			case iMAXIMUM:
				stdMethod = StdValueMaximum::create(m_pOwnerEffect, eCostBenefit);
				delete pFunction;
				break;
			case iINTERVAL:
				stdMethod = StdValueInterval::create(m_pOwnerEffect, eCostBenefit);
				delete pFunction;
				break;
			default:
				stdMethod = StdValueGeneral::create(m_pOwnerEffect, eStdValueMethod, eCostBenefit, pFunction);
				break;
		}

		SetModifiedFlag();
	}
	else
		delete pFunction;
}

String StandardizationValue::sStandardize(String sData)
{
	if (stdMethod != 0)
		return stdMethod->sStandardize(sData);
	else
		return sData;
}

void StandardizationValue::WriteElements(const char* sSection, const ElementContainer& en)
{
	Standardization::WriteElements(sSection, en);

	ObjectInfo::WriteElement(sSection, "CostBenefit", en, sCostBenefitTypeToString(eCostBenefit));
	ObjectInfo::WriteElement(sSection, "StdValueMethod", en, sStdValueMethodToString(eStdValueMethod));

	if (stdMethod)
		stdMethod->WriteElements(sSection, en);
}

void StandardizationValue::ReadElements(const char* sSection, const ElementContainer& en)
{
	Standardization::ReadElements(sSection, en);

	String sType;
	ObjectInfo::ReadElement(sSection, "CostBenefit", en, sType);
	eCostBenefit = eCostBenefitTypeFromString(sType);
	String sStdValueMethod;
	ObjectInfo::ReadElement(sSection, "StdValueMethod", en, sStdValueMethod);
	eStdValueMethod = eStdValueMethodFromString(sStdValueMethod);

	if (stdMethod)
		delete stdMethod;
	stdMethod = 0;

	switch (eStdValueMethod)
	{
		case iMAXIMUM:
			stdMethod = StdValueMaximum::create(m_pOwnerEffect, eCostBenefit);
			break;
		case iINTERVAL:
			stdMethod = StdValueInterval::create(m_pOwnerEffect, eCostBenefit);
			break;
		default:
			stdMethod = StdValueGeneral::create(m_pOwnerEffect, eStdValueMethod, eCostBenefit, 0);
			break;
	}
	if (stdMethod)
		stdMethod->ReadElements(sSection, en);
}

String StandardizationValue::sDescription()
{
	if (stdMethod)
		return stdMethod->sDescription();
	else
		return "";
}

//////////////////////////////////////////////////////////////////
// StdValueMethod - parent
//////////////////////////////////////////////////////////////////

StdValueMethod::StdValueMethod(Effect* peOwner, SmceFunction * pFunction, StandardizationValue::eCostBenefitTP _eCostBenefit)
: m_pOwnerEffect(peOwner)
, m_pFunction(pFunction)
, eCostBenefit(_eCostBenefit)
{

}

StdValueMethod::~StdValueMethod()
{
	if (m_pFunction)
		delete m_pFunction;
}

String StdValueMethod::sMinMaxCorrector(vector <String> vsTerms, bool fMax)
{
	String sMinMax(fMax?"max":"min");
	if (vsTerms.size() == 1)
		return vsTerms[0];
	else if (vsTerms.size() == 2)
		return String("%S(%S,%S)", sMinMax, vsTerms[0], vsTerms[1]);
	else if (vsTerms.size() == 3)
		return String("%S(%S,%S,%S)", sMinMax, vsTerms[0], vsTerms[1], vsTerms[2]);
	else if (vsTerms.size() > 3)
	{
		unsigned int iOneThird = vsTerms.size() / 3;
		vector <String> vsTemp1;
		vector <String> vsTemp2;
		vector <String> vsTemp3;
		for (unsigned int i=0; i<vsTerms.size(); ++i)
			if (i<iOneThird)
				vsTemp1.push_back(vsTerms[i]);
			else if (i<(2*iOneThird))
				vsTemp2.push_back(vsTerms[i]);
			else
				vsTemp3.push_back(vsTerms[i]);
		return String("%S(%S,%S,%S)", sMinMax, sMinMaxCorrector(vsTemp1, fMax), sMinMaxCorrector(vsTemp2, fMax), sMinMaxCorrector(vsTemp3, fMax));
	}
	else return "1"; // probably something went wrong!
}

String StdValueMethod::sTotalMinMax(bool fMax)
{
	// Also takes care of maximum 3 items in ILWIS max statement
	// such a statement should become max(max(a,b,c),max(d,e,f),max(g,h,i))

	vector <String> vsAll;
	for (int i=1; i<=m_pOwnerEffect->iGetNrAlternatives(); ++i)
	{
		String sInputMinMax = m_pOwnerEffect->sInputMinMax(i, fMax);
		if ((sInputMinMax != "") && (vsAll.end() == find(vsAll.begin(), vsAll.end(), sInputMinMax))) // non-empty && unique
			vsAll.push_back(sInputMinMax);
	}

	return sMinMaxCorrector(vsAll, fMax);
}

SmceFunction ** StdValueMethod::ppFunction()
{
	return &m_pFunction;
}
//////////////////////////////////////////////////////////////////
// StdValueMaximum
//////////////////////////////////////////////////////////////////

StdValueMaximum::StdValueMaximum(Effect* peOwner, StandardizationValue::eCostBenefitTP _eCostBenefit)
: StdValueMethod(peOwner, 0, _eCostBenefit)
{

}

StdValueMaximum::~StdValueMaximum()
{

}

StdValueMethod* StdValueMaximum::create(Effect* peOwner, StandardizationValue::eCostBenefitTP _eCostBenefit)
{
	return new StdValueMaximum(peOwner, _eCostBenefit);
}

StdValueMethod* StdValueMaximum::copy(Effect* peOwner)
{
	StdValueMaximum* stdMethodReturn = new StdValueMaximum(peOwner, eCostBenefit);
	return stdMethodReturn;
}

String StdValueMaximum::sDescription()
{
	return TR("Maximum");
}

String StdValueMaximum::sStandardize(String sData)
{
	// Formulas:
	// For benefits:
	// result = value/TOTAL_MAX
	// For costs:
	// result = 1-value/TOTAL_MAX+TOTAL_MIN/TOTAL_MAX (shift up)

	String sReturn("");
	// value/TOTAL_MAX OR 1-value/TOTAL_MAX
	if (eCostBenefit == StandardizationValue::iBENEFIT)
		sReturn += String("%S/%S", sData, sTotalMinMax(true));
	else // iCOST
	{
		sReturn += String("1-%S/%S", sData, sTotalMinMax(true));
		// +TOTAL_MIN/TOTAL/MAX if needed
		if ((sTotalMinMax(false) != "0"))
			sReturn += String("+%S/%S", sTotalMinMax(false), sTotalMinMax(true));
	}

	return sReturn;
}

void StdValueMaximum::WriteElements(const char* sSection, const ElementContainer& en)
{
	// no members to save/restore in this class
}

void StdValueMaximum::ReadElements(const char* sSection, const ElementContainer& en)
{
	// no members to save/restore in this class
}

//////////////////////////////////////////////////////////////////
// StdValueInterval
//////////////////////////////////////////////////////////////////

StdValueInterval::StdValueInterval(Effect* peOwner, StandardizationValue::eCostBenefitTP _eCostBenefit)
: StdValueMethod(peOwner, 0, _eCostBenefit)
{

}

StdValueInterval::~StdValueInterval()
{

}

StdValueMethod* StdValueInterval::create(Effect* peOwner, StandardizationValue::eCostBenefitTP _eCostBenefit)
{
	return new StdValueInterval(peOwner, _eCostBenefit);
}

StdValueMethod* StdValueInterval::copy(Effect* peOwner)
{
	StdValueInterval* stdMethodReturn = new StdValueInterval(peOwner, eCostBenefit);
	return stdMethodReturn;
}

String StdValueInterval::sDescription()
{
	return TR("Interval");
}

String StdValueInterval::sStandardize(String sData)
{
	// Formulas:
	// For benefits:
	// result = (value-TOTAL_MIN)/(TOTAL_MAX-TOTAL_MIN)
	// For costs:
	// result = 1-(value-TOTAL_MIN)/(TOTAL_MAX-TOTAL_MIN)

	String sReturn("");
	// (value-TOTAL_MIN) OR 1-(value-TOTAL_MIN)
	if (eCostBenefit != StandardizationValue::iBENEFIT) // iCOST
		sReturn += "1-";
	if (sTotalMinMax(false) != "0")
		sReturn += String("(%S-%S)/(%S-%S)", sData, sTotalMinMax(false), sTotalMinMax(true), sTotalMinMax(false));
	else
		sReturn += String("%S/%S", sData, sTotalMinMax(true));

	return sReturn;
}

void StdValueInterval::WriteElements(const char* sSection, const ElementContainer& en)
{
	// no members to save/restore in this class
}

void StdValueInterval::ReadElements(const char* sSection, const ElementContainer& en)
{
	// no members to save/restore in this class
}

//////////////////////////////////////////////////////////////////
// StdValueGeneral
//////////////////////////////////////////////////////////////////

StdValueGeneral::StdValueGeneral(Effect* peOwner, StandardizationValue::eStdValueMethodTP _eStdValueMethod, StandardizationValue::eCostBenefitTP _eCostBenefit, SmceFunction * pFunction)
: StdValueMethod(peOwner, pFunction, _eCostBenefit)
, eStdValueMethod(_eStdValueMethod)
{

}

StdValueGeneral::~StdValueGeneral()
{
}

StdValueMethod* StdValueGeneral::create(Effect* peOwner, StandardizationValue::eStdValueMethodTP _eStdValueMethod, StandardizationValue::eCostBenefitTP _eCostBenefit, SmceFunction * pFunction)
{
	return new StdValueGeneral(peOwner, _eStdValueMethod, _eCostBenefit, pFunction);
}

StdValueMethod* StdValueGeneral::copy(Effect* peOwner)
{
	SmceFunction* pFunc = 0;
	if (m_pFunction)
		pFunc = pGetNewFunction(eStdValueMethod, m_pFunction->Domain(), &(m_pFunction->vAnchors()), eCostBenefit);

	StdValueGeneral* stdMethodReturn = new StdValueGeneral(peOwner, eStdValueMethod, eCostBenefit, pFunc);
	
	return stdMethodReturn;
}

String StdValueGeneral::sDescription()
{
	if (m_pFunction)
		return m_pFunction->sDescription();
	else
		return "";
}

String StdValueGeneral::sStandardize(String sData)
{
	String sReturn("min(1,max(0,");
	if (m_pFunction)
		sReturn += m_pFunction->sGetFx(sData);
	else
		sReturn += sData;
	sReturn += "))";

	return sReturn;
}

void StdValueGeneral::WriteElements(const char* sSection, const ElementContainer& en)
{
	if (m_pFunction)
	{
		if (m_pFunction->vAnchors().size() >= 2)
		{
			ObjectInfo::WriteElement(sSection, "Min", en, m_pFunction->vAnchors()[0].X); // these 4 lines are here for backwards compatibility
			ObjectInfo::WriteElement(sSection, "MinY", en, m_pFunction->vAnchors()[0].Y);
			ObjectInfo::WriteElement(sSection, "Max", en, m_pFunction->vAnchors()[m_pFunction->vAnchors().size() - 1].X);
			ObjectInfo::WriteElement(sSection, "MaxY", en, m_pFunction->vAnchors()[m_pFunction->vAnchors().size() - 1].Y);
			for (unsigned int i = 1; i < m_pFunction->vAnchors().size() - 1; ++i)
			{
				ObjectInfo::WriteElement(sSection, String("Anchor%dX", i).c_str(), en, m_pFunction->vAnchors()[i].X);
				ObjectInfo::WriteElement(sSection, String("Anchor%dY", i).c_str(), en, m_pFunction->vAnchors()[i].Y);
			}
		}
	}
}

void StdValueGeneral::ReadElements(const char* sSection, const ElementContainer& en)
{
	RangeReal rrMinMax = RangeReal();
	int iNrAlternatives = 1;
	if (m_pOwnerEffect)
	{
		iNrAlternatives = m_pOwnerEffect->iGetNrAlternatives();
		CWaitCursor cwait;
		for (int i=1; i<=iNrAlternatives; ++i)
		{
			if (1==i) // first time unconditional
				rrMinMax = m_pOwnerEffect->rrMinMax(i);
			else
				rrMinMax = RangeReal(min(rrMinMax.rLo(), m_pOwnerEffect->rrMinMax(i).rLo()),max(rrMinMax.rHi(), m_pOwnerEffect->rrMinMax(i).rHi()));
		}
	}

	DoubleRect drFunctionDomain (rrMinMax.rLo(), 0, rrMinMax.rHi(), 1);

	if (m_pFunction)
		delete m_pFunction;
	m_pFunction = pGetNewFunction(eStdValueMethod, drFunctionDomain, 0, eCostBenefit);
	if (m_pFunction)
	{
		if (m_pFunction->vAnchors().size() >= 2)
		{
			vector <DoublePoint> vAnchors;
			vAnchors.resize(m_pFunction->vAnchors().size());
			ObjectInfo::ReadElement(sSection, "Min", en, vAnchors[0].X);
			ObjectInfo::ReadElement(sSection, "MinY", en, vAnchors[0].Y);
			ObjectInfo::ReadElement(sSection, "Max", en, vAnchors[vAnchors.size() - 1].X);
			ObjectInfo::ReadElement(sSection, "MaxY", en, vAnchors[vAnchors.size() - 1].Y);
			for (unsigned int i = 1; i < m_pFunction->vAnchors().size() - 1; ++i)
			{
				ObjectInfo::ReadElement(sSection, String("Anchor%dX", i).c_str(), en, vAnchors[i].X);
				ObjectInfo::ReadElement(sSection, String("Anchor%dY", i).c_str(), en, vAnchors[i].Y);
			}
			m_pFunction->SetAnchors(vAnchors);
		}
	}
}

//////////////////////////////////////////////////////////////////
// StandardizationValueConstraint
//////////////////////////////////////////////////////////////////

StandardizationValueConstraint::StandardizationValueConstraint(Effect* peOwner)
: Standardization(peOwner)
, rMin(0), rMax(0), fMin(false), fMax(false)
{
}

StandardizationValueConstraint::~StandardizationValueConstraint()
{
}

Standardization* StandardizationValueConstraint::create(Effect* peOwner)
{
	return new StandardizationValueConstraint(peOwner);
}

Standardization* StandardizationValueConstraint::copy(Effect* peOwner)
{
	StandardizationValueConstraint* stdReturn = new StandardizationValueConstraint(peOwner);
	stdReturn->m_fFormAllowed = m_fFormAllowed;
	stdReturn->m_fStandardized = m_fStandardized;
	stdReturn->fMin = fMin;
	stdReturn->fMax =fMax;
	stdReturn->rMin = rMin;
	stdReturn->rMax = rMax;
	return stdReturn;
}

int StandardizationValueConstraint::iCostBenefit()
{
	return 2;
}

class StandardizationValueConstraintForm: public FormWithDest
{
public:
	StandardizationValueConstraintForm(CWnd* wPar, RangeReal rrMinMax, double* rMin, double* rMax, bool* fMin, bool* fMax, int iNrAlternatives)
	: FormWithDest(wPar, TR("Standardize Value Input"))
	, vrPrecision(ValueRange(-LONG_MAX+1, LONG_MAX, 0.001))
	{
		if (rrMinMax.fValid())
		{
			String sAlternativesRemark("");
			if (iNrAlternatives > 1)
				sAlternativesRemark = TR("over all alternatives ");
			StaticText* stMin = new StaticText(root, String(TR("The minimum %Sis %lg").c_str(), sAlternativesRemark, rrMinMax.rLo()));
			StaticText* stMax = new StaticText(root, String(TR("The maximum %Sis %lg").c_str(), sAlternativesRemark, rrMinMax.rHi()));
			stMin->SetIndependentPos();
			stMax->SetIndependentPos();
		}
		StaticText* stTitle = new StaticText(root, TR("Pass on the following condition(s):"));
		stTitle->SetIndependentPos();
		cbUseMinimum = new CheckBox(root, TR("Minimum"), fMin);
		cbUseMinimum->SetCallBack((NotifyProc)&StandardizationValueConstraintForm::CallBackChange);
		frMinimum = new FieldReal(cbUseMinimum, "", rMin, vrPrecision);
		frMinimum->Align(cbUseMinimum, AL_AFTER);
		frMinimum->SetCallBack((NotifyProc)&StandardizationValueConstraintForm::CallBackChange);
		cbUseMaximum = new CheckBox(root, TR("Maximum"), fMax);
		cbUseMaximum->Align(cbUseMinimum, AL_UNDER);
		cbUseMaximum->SetCallBack((NotifyProc)&StandardizationValueConstraintForm::CallBackChange);
		frMaximum = new FieldReal(cbUseMaximum, "", rMax, vrPrecision);
		frMaximum->Align(cbUseMaximum, AL_AFTER);
		frMaximum->SetCallBack((NotifyProc)&StandardizationValueConstraintForm::CallBackChange);
		frDescription = new FieldString(root, &sDummy, ES_READONLY);
		frDescription->Align(cbUseMaximum, AL_UNDER);
		frDescription->SetIndependentPos();
		frDescription->SetWidth(180);

		SetMenHelpTopic("ilwismen\\smce_window_standardize_value.htm");
		
		create();
	}
	int CallBackChange(Event*)
	{
		bool fMin = cbUseMinimum->fVal();
		bool fMax = cbUseMaximum->fVal();
		double rMin = frMinimum->rVal();
		double rMax = frMaximum->rVal();
		// copy from sDescription
		String sDescription;
		if (!(fMin || fMax))
			sDescription = "<>0";
		else if (fMin && !fMax)
			sDescription = String("%S=%lg", TR("Min"), rMin);
		else if (!fMin && fMax)
			sDescription = String("%S=%lg", TR("Max"), rMax);
		else // (fMin && fMax)
		{
			if (rMin <= rMax)
				sDescription = String("%S [%lg,%lg]", TR("Inside"), rMin, rMax);
			else // rMax < rMin
				sDescription = String("%S (%lg, %lg)", TR("Outside"), rMax, rMin);
		}
		frDescription->SetVal(String(TR("Resulting condition: %S").c_str(), sDescription));

		return 0;
	}
private:
	ValueRange vrPrecision;
	CheckBox* cbUseMinimum;
	CheckBox* cbUseMaximum;
	FieldReal* frMinimum;
	FieldReal* frMaximum;
	FieldString* frDescription;
	String sDummy;
};

void StandardizationValueConstraint::ShowForm()
{
	CWnd* wnd = 0;
	RangeReal rrMinMax = RangeReal();
	int iNrAlternatives = 1;
	MapEffect* me = dynamic_cast<MapEffect*>(m_pOwnerEffect);
	if (me)
	{
		CWaitCursor cwait;
		iNrAlternatives = me->iGetNrAlternatives();
		for (int i=1; i<=iNrAlternatives; ++i)
		{
			if (1==i) // first time unconditional
				rrMinMax = me->rrMinMax(i);
			else
				rrMinMax = RangeReal(min(rrMinMax.rLo(), me->rrMinMax(i).rLo()),max(rrMinMax.rHi(), me->rrMinMax(i).rHi()));
		}
	}
	if (!m_fStandardized && rrMinMax.fValid())
	{
		fMin = true;
		rMin = (rrMinMax.rLo() + rrMinMax.rHi()) / 2.0;
		rMax = rMin;
	}
	if (m_pOwnerEffect)
		wnd = m_pOwnerEffect->GetDocument()->wndGetActiveView();
	StandardizationValueConstraintForm frm(wnd, rrMinMax, &rMin, &rMax, &fMin, &fMax, iNrAlternatives);
	if (frm.fOkClicked())
	{
		m_fStandardized = true;
		SetModifiedFlag();
	}
}

String StandardizationValueConstraint::sStandardize(String sData)
{
	// cases:
	// !fMin && !fMax (no constraint?)
	// fMin && !fMax
	// !fMin && fMax
	// fMin && fMax && rMin <= rMax
	// fMin && fMax && rMin > rMax
	if (!(fMin || fMax))
		return sData;
	else if (fMin && !fMax)
		return String("%S>=%lg", sData, rMin);
	else if (!fMin && fMax)
		return String("%S<=%lg", sData, rMax);
	else // (fMin && fMax)
	{
		if (rMin <= rMax)
			return String("(%S>=%lg) and (%S<=%lg)", sData, rMin, sData, rMax);
		else // rMax < rMin
			return String("(%S<=%lg) or (%S>=%lg)", sData, rMax, sData, rMin);
	}
}

void StandardizationValueConstraint::WriteElements(const char* sSection, const ElementContainer& en)
{
	Standardization::WriteElements(sSection, en);
	ObjectInfo::WriteElement(sSection, "Min", en, rMin);
	ObjectInfo::WriteElement(sSection, "Max", en, rMax);
	ObjectInfo::WriteElement(sSection, "UseMin", en, fMin);
	ObjectInfo::WriteElement(sSection, "UseMax", en, fMax);
}

void StandardizationValueConstraint::ReadElements(const char* sSection, const ElementContainer& en)
{
	Standardization::ReadElements(sSection, en);
	ObjectInfo::ReadElement(sSection, "Min", en, rMin);
	ObjectInfo::ReadElement(sSection, "Max", en, rMax);
	ObjectInfo::ReadElement(sSection, "UseMin", en, fMin);
	ObjectInfo::ReadElement(sSection, "UseMax", en, fMax);
}

String StandardizationValueConstraint::sDescription()
{
	// cases:
	// !fMin && !fMax (no constraint?)
	// fMin && !fMax
	// !fMin && fMax
	// fMin && fMax && rMin <= rMax
	// fMin && fMax && rMin > rMax
	if (!(fMin || fMax))
		return "<>0";
	else if (fMin && !fMax)
		return String("%S=%lg", TR("Min"), rMin);
	else if (!fMin && fMax)
		return String("%S=%lg", TR("Max"), rMax);
	else // (fMin && fMax)
	{
		if (rMin <= rMax)
			return String("%S [%lg,%lg]", TR("Inside"), rMin, rMax);
		else // rMax < rMin
			return String("%S (%lg, %lg)", TR("Outside"), rMax, rMin);
	}
}

//////////////////////////////////////////////////////////////////
// StandardizationClass
//////////////////////////////////////////////////////////////////

StandardizationClass::StandardizationClass(Effect* peOwner)
: Standardization(peOwner)
, pWeights(0)
{

}

StandardizationClass::~StandardizationClass()
{
	if (pWeights)
		delete pWeights;
}

Standardization* StandardizationClass::create(Effect* peOwner)
{
	return new StandardizationClass(peOwner);
}

Standardization* StandardizationClass::copy(Effect* peOwner)
{
	StandardizationClass* stdReturn = new StandardizationClass(peOwner);
	stdReturn->m_fFormAllowed = m_fFormAllowed;
	stdReturn->m_fStandardized = m_fStandardized;
	stdReturn->sAttribCol = sAttribCol;
	return stdReturn;
}

int StandardizationClass::iCostBenefit()
{
	return 2;
}

String StandardizationClass::sDescription()
{
	return String(TR("Attr='%S'").c_str(), sAttribCol);
}

String StandardizationClass::sStandardize(String sData)
{
	FileName fnData(sData); // purpose: strip sCol .. gives mapcalc syntax error; should be solved
	fnData.sCol = "";       // better in future: user wants to std the domain of the col
	return String("%S.%S", fnData.sRelativeQuoted(), sAttribCol);
}

class FieldStdColumn: public FieldColumn
{
public:
	FieldStdColumn(FormEntry* fe, const String& sQuestion, const Table& tbl, String *psName, long types = 0)
	: FieldColumn(fe, sQuestion, tbl, psName, types)
	{
	}
	void FillWithStdColumns(TablePtr *t)
	{
		String sCol;
  
		tbl = t;
		if (0 == dynamic_cast<TableView*>(t))
			table.SetPointer(t);
		if (0 == fld->ose)
			return;
		fld->ose->ResetContent();
		if (tbl) {
			for (int i = 0; i < tbl->iCols(); ++i) {
				Column col = tbl->col(i);
				if (!col.fValid())
					continue;
				if (dmTypes) {
					Domain dm = col->dm();
					bool fAdd = false;
					if ((dmTypes & dmCLASS) && 0 != dm->pdc()) fAdd = true;
					else if ((dmTypes & dmIDENT) && 0 != dm->pdid()) fAdd = true;
					else if ((dmTypes & dmVALUE) && 0 != dm->pdv() && 0 == dm->pdbool()) fAdd = true;
					else if ((dmTypes & dmGROUP) && 0 != dm->pdgrp()) fAdd = true;
					else if ((dmTypes & dmIMAGE) && 0 != dm->pdi()) fAdd = true;
					else if ((dmTypes & dmPICT)  && 0 != dm->pdp()) fAdd = true;
					else if ((dmTypes & dmCOLOR) && 0 != dm->pdcol()) fAdd = true;
					else if ((dmTypes & dmBOOL)  && 0 != dm->pdbool()) fAdd = true;
					else if ((dmTypes & dmBIT)   && 0 != dm->pdbit()) fAdd = true;
					else if ((dmTypes & dmSTRING)&& 0 != dm->pds()) fAdd = true;
					else if ((dmTypes & dmCOORD) && 0 != dm->pdcrd()) fAdd = true;
					else if ((dmTypes & dmUNIQUEID)&& 0 != dm->pdUniqueID()) fAdd = true;				
					if (!fAdd)
						continue;
				}
				RangeReal rr (col->rrMinMax());
				if ((rr.fValid() && RangeReal(0,1).fContains(rr)) || (rr.rLo() == DBL_MAX && rr.rHi() == -DBL_MAX))
				{
					sCol = col->sName();
					sCol &= ".clm";
					fld->ose->AddString(sCol.sVal());
				}
			}
		}
		sCol = _sName.sUnQuote();
		if ( sCol != "")
		{
			sCol &= ".clm";
			fld->ose->SelectString(-1, sCol.c_str());
		}
		else
			fld->ose->SetCurSel(0);
	}
};

class StandardizationClassForm: public FormWithDest
{
public:
	StandardizationClassForm(CWnd* wPar, AttributeFileName afnMap, String *sAttrTable, String* sAttribCol, Evaluation** ppWeights, bool fIsConstraint)
	: FormWithDest(wPar, TR("Standardize Class Input"))
	, m_fInitDone(false)
	, m_sAttrTable(sAttrTable)
	, m_sAttribCol(sAttribCol)
	, m_ds(0)
	, m_dsMap(0)
	, m_ppWeights(ppWeights)
	, m_fIsConstraint(fIsConstraint)
	, fModified(false)
	, m_fWizardAvailable(false)
	, m_sMapCol(afnMap.sCol)
	{

		FileName fnMap = FileName(afnMap); // use FileName copy constructor to remove sCol
		BaseMap bm (fnMap);
		if (bm.fValid())
			if (bm->dm().fValid())
				m_dsMap = bm->dm()->pdsrt();

		m_ds = m_dsMap; // default

		if (m_sAttrTable->length() != 0)
		{
			// set tblAttribute
			try
			{
				FileName fnAttribTbl((*m_sAttrTable), ".tbt", false); // don't assume the extension is there; Table throws error if there's no extension
				tblAttribute = fnAttribTbl;
			}
			catch (ErrorObject&)
			{
				// -- just ignore it
			}
		}

		if ((m_sMapCol != "") && tblAttribute.fValid())
		{
			// user selected an attribute column .. set colMapAttr to it and m_ds to its domain
			colMapAttr = tblAttribute->col(m_sMapCol);
			if (colMapAttr.fValid())
				m_ds = colMapAttr->dm()->pdsrt();
		}

		new StaticText(root, TR("Attribute Table:"));
		FormEntry* feFirstElement;
		if (m_sAttrTable->length() == 0)
		{
			ftAttTable = new FieldTableC(root, "", m_sAttrTable, bm->dm());
			ftAttTable->SetCallBack((NotifyProc)&StandardizationClassForm::CallBackAttTableChange);
			feFirstElement = ftAttTable;
			m_fAttrTableLinkReadOnly = false;
		}
		else
		{
			feFirstElement = new StaticText(root, String("'%S'", FileName(*m_sAttrTable).sFileExt()));
			m_fAttrTableLinkReadOnly = true;
		}
		feFirstElement->SetIndependentPos();
		PushButton* pbOpenTbl = new PushButton(root, TR("Edit"), (NotifyProc)&StandardizationClassForm::CallBackOpenAttrib);
		pbOpenTbl->Align(feFirstElement, AL_AFTER);
		pbOpenTbl->SetIndependentPos();
		FieldBlank* fbDummy1 = new FieldBlank(root, 0.5);
		fbDummy1->Align(feFirstElement, AL_UNDER);
		PushButton* pbRefresh = new PushButton(root, TR("Refresh Attribute List"), (NotifyProc)&StandardizationClassForm::CallBackUpdateColumns);
		pbRefresh->Align(fbDummy1, AL_UNDER);
		FieldBlank* fbDummy2 = new FieldBlank(root, 0.3);
		fbDummy2->Align(pbRefresh, AL_UNDER);

		StaticText* stAttrLabel;
		if (fIsConstraint)
			stAttrLabel = new StaticText(root, TR("Boolean Attribute:"));
		else
			stAttrLabel = new StaticText(root, TR("Attribute between 0 and 1:"));
		stAttrLabel->Align(fbDummy2, AL_UNDER);

		if (fIsConstraint)
			fldCol = new FieldStdColumn(root, "", Table(), sAttribCol, dmBOOL);
		else
			fldCol = new FieldStdColumn(root, "", Table(), sAttribCol, dmVALUE|dmBOOL|dmBIT);
		fldCol->SetCallBack((NotifyProc)&StandardizationClassForm::ColCallBackFunc);
		fldCol->Align(stAttrLabel, AL_UNDER);
		fldCol->psn->SetBound(0,-1,0,0); // to "connect" to button
		OwnButtonSimple* pbCreateColumn = new OwnButtonSimple(root, "CreateBut", (NotifyProc)&StandardizationClassForm::CallBackAddColumn, false, false);
		pbCreateColumn->Align(fldCol, AL_AFTER);
		pbCreateColumn->SetIndependentPos();

		if (!fIsConstraint && m_ds && m_ds->iNettoSize() <= 15) // max. 15 classes .. could be improved but needs more time
		{
			m_fWizardAvailable = true;
			PushButton* pbApplyWizard = new PushButton(root, TR("Apply Weigh Wizard"), (NotifyProc)&StandardizationClassForm::CallBackApplyWizard);
			pbApplyWizard->Align(pbCreateColumn, AL_AFTER);
		}

		SetMenHelpTopic("ilwismen\\smce_window_standardize_class.htm");

		create();
	}
	int ColCallBackFunc(Event*)
	{
		if (!m_fInitDone)
		{
			FillWithSuitableColumns();
			m_fInitDone = true;
		}

		return 0;
	}
	int CallBackOpenAttrib(Event*)
	{
		if (m_sAttrTable->length() > 0)
		{
			FileName fnTbl(*m_sAttrTable, ".tbt", false);
			String sExec("show %S", fnTbl.sFullNameQuoted());
			char* str = sExec.sVal();
			IlwWinApp()->GetMainWnd()->SendMessage(ILWM_EXECUTE, 0, (LPARAM)str);
		}
		return 0;
	}
	int CallBackAddColumn(Event*)
	{
		class NewColumnForm: public FormWithDest
		{
		public:
			NewColumnForm(CWnd* parent, bool fIsConstraint)
			: FormWithDest(parent, TR("Add Attribute"))
			,	vr(0, 1, 0.01)
			{
				if (fIsConstraint)
				{
					sDomName = "bool.dom";
					sDescr = "constraining boolean values";
					vr = ValueRange(0, 1, 1); // Table->Add Column->Choose Bool
				}
				else
				{
					sDomName = "value.dom";
					sDescr = "standardized values";
				}
				new FieldString(root, TR("&Attribute Name"), &sColName, Domain(), false);
				create();
			}
			String sColName;
			String sDomName; // in case we wanna allow the user to change this someday
			String sDescr; // in case we wanna allow the user to change this someday
			ValueRange vr; // in case we wanna allow the user to change this someday
		};

		if (m_sAttrTable->length() == 0)
			return 0; // let user select attr table first
		if (!tblAttribute.fValid())
			return 0; // useless
		if (tblAttribute->fDataReadOnly()) 
			return 0; // useless
		try
		{
			NewColumnForm frm(wnd(), m_fIsConstraint);
			if (frm.fOkClicked())
			{
				if ("" == frm.sColName)
					return 0; // useless

				Domain dm(frm.sDomName);
				if (!dm.fValid())
					return 0; // strange if this happens, but useless

				DomainValueRangeStruct dvrs(dm, frm.vr);
				
				Column col;
				try
				{
					col = Column(tblAttribute, frm.sColName, dvrs);
				}
				catch (CMemoryException* err)
				{
					AfxMessageBox(TR("Could not create column (not enough memory)").c_str());
					err->Delete();
				}
				if (!col.fValid()) // failed to create .. also if it already exists
				{
					fldCol->SetVal(frm.sColName); // if exists, try to display anyway
					return 0;
				}
				col->SetDescription(frm.sDescr);  
				col->Fill();
				FillWithSuitableColumns();
				fldCol->SetVal(frm.sColName);
			}    
		}
		catch (ErrorObject& err)
		{
			err.Show();
		}

		if (!m_fWizardAvailable)
		{
			Sleep(500);
			CallBackOpenAttrib(0);
		}

		return 0;
	}
	int CallBackUpdateColumns(Event*)
	{
		FillWithSuitableColumns();
		return 0;
	}
	int CallBackApplyWizard(Event*)
	{
		list <String> liClasses;
		map <String, double> mpClassWeights;

		if (0 == m_ds)
			return 0; // algorithm would be useless anyway

		if (m_sAttrTable->length() == 0)
			return 0; // let user select attr table first

		if (!tblAttribute.fValid())
			return 0; // useless
		Column colStd (tblAttribute->col(fldCol->sName()));
		if (!colStd.fValid())
			return 0; // useless

		if ((m_sMapCol != "") && (!colMapAttr.fValid()))
				return 0; // useless

		// Definitions:
		// "colStd" is the column that contains the standardized real values
		// "colMapAttr" (if valid) is the column with items from the second domain m_ds

		// Copy values from domain(s) and table to liClasses and mpClassWeights
		// Steps:
		// 1) Loop domain m_ds and add all class names to liClasses
		// 2) <in the same loop>: for each class name find the corresponding weight from the table (if available) and put it in mpClassWeights
		// Notes:
		// The map being standardized has domain m_dsMap. It is a class/id map (otherwise program flow wouldn't get here).
		// If the user has selected an attribute column to be used as a criterion, this is a class column, with domain m_ds. The column is "colMapAttr"
		// If the user has selected the map itself to be used as a criterion, then m_ds == m_dsMap, and it is definitely a class map (not id).
		int i; // used again later, at "copy back"
		for (i = 1; i <= m_ds->iNettoSize(); ++i)
		{
			long iKey = m_ds->iKey(i);
			String s = m_ds->sValueByRaw(iKey,0);
			liClasses.push_back(s.c_str());
			long iKeyInMapDomain = iKey; // default
			if (m_ds != m_dsMap)
			{
				// find first occurrence of s in colMapAttr
				int j = 1;
				bool fFound = false;
				while (j <= m_dsMap->iNettoSize() && !fFound)
				{
					long jKey = m_dsMap->iKey(j);
					String s2 = colMapAttr->sValue(jKey, 0);
					if (s2 == s)
					{
						fFound = true;
						iKeyInMapDomain = jKey;
					}
					++j;
				}
			}
			mpClassWeights[s.c_str()]= colStd->rValue(iKeyInMapDomain);
		}

		// Call the "Evaluation wizard" - let the user alter mpClassWeights
		if (*m_ppWeights)
		{
			Evaluation::eActionTP action = (*m_ppWeights)->ShowForm(wnd(), &liClasses, &mpClassWeights);
			if (action == Evaluation::iOTHERMETHOD)
			{
				// user wants different method ..
				Evaluation* pNewWeights = Evaluation::create(wnd(), &liClasses, &mpClassWeights, *m_ppWeights, Evaluation::iMAXISONE, TR("Priority Values"));
				if (pNewWeights != *m_ppWeights)
				{
					delete *m_ppWeights;
					*m_ppWeights = pNewWeights;
				}
				fModified = true; // pessimistic .. better more often "Modified" than forgetting to save when modified
			}
			else if (action == Evaluation::iOK)
				fModified = true;
		}
		else
		{
			*m_ppWeights = Evaluation::create(wnd(), &liClasses, &mpClassWeights, 0, Evaluation::iMAXISONE, TR("Priority Values"));
			if (*m_ppWeights)
				fModified = true;
		}

		// Copy values back from mpClassWeights to table

		if (fModified) // skip if user cancelled
		{
			for (i = 1; i <= m_dsMap->iNettoSize(); ++i)
			{
				long iKey = m_dsMap->iKey(i);
				String s = m_dsMap->sValueByRaw(iKey,0);

				if (m_ds != m_dsMap)
					s = colMapAttr->sValue(iKey, 0); // do the "join": s should come from colMapAttr
				
				colStd->PutVal(iKey, mpClassWeights[s.c_str()]);
			}
			tblAttribute->Updated();
		}

		return 0;
	}
	void FillWithSuitableColumns()
	{
		try
		{
			if (tblAttribute.fValid())
				fldCol->FillWithStdColumns(tblAttribute.ptr());
			else
				fldCol->FillWithColumns("");
		}
		catch (ErrorObject&)
		{
			// -- just ignore it
		}
	}
	int CallBackAttTableChange(Event* e)
	{
		ftAttTable->StoreData();
		if (m_sAttrTable->length() == 0)
			return 0;

		// set tbl to the new table
		try
		{
			FileName fnAttribTbl((*m_sAttrTable), ".tbt", false); // don't assume the extension is there; Table throws error if there's no extension
			tblAttribute = fnAttribTbl;
		}
		catch (ErrorObject&)
		{
			// -- just ignore it
		}

		CallBackUpdateColumns(e);
			
		return 0;
	}
	int exec()
	{
		FormWithDest::exec();

		bool fAttrTableLinked = !m_fAttrTableLinkReadOnly && (m_sAttrTable->length() > 0);

		if ((0 == m_dsMap) || (!fAttrTableLinked))
			return 0;

		bool fAttrTableOk = tblAttribute.fValid();

		if (!fAttrTableOk)
		{
			if (m_dsMap->fTblAtt())  // Only set in case the domain DID have an attribute table
				m_dsMap->SetNoAttributeTable();
		}
		else
		{
			bool fHasAtt = m_dsMap->fTblAtt();
			bool fNeedChange = !fHasAtt;
			if (fHasAtt)  // Check if the new tblattr is equal to the one currently in use
			{
				try
				{
					Table tblAtt = m_dsMap->tblAtt();
					fNeedChange = (tblAttribute != tblAtt);
				}
				catch(ErrorObject& err)
				{
					err.Show();
				}
			}
			if (fNeedChange)  // Only set in case the domain did NOT have an attribute table or
											// another table has been specified
			{
				if (m_dsMap->fEqual(*tblAttribute->dm().ptr()))
					m_dsMap->SetAttributeTable(tblAttribute);
				else
					MessageBox(TR("Attribute Table should have same domain!").c_str(), TR("Error").c_str());
			}
		}  
		return 0;
	}
public:
	bool fModified;
private:
	FieldTableC *ftAttTable;
	FieldStdColumn *fldCol;
	String *m_sAttrTable;
	String *m_sAttribCol;
	DomainSort* m_ds;
	DomainSort* m_dsMap;
	bool m_fAttrTableLinkReadOnly;
	bool m_fInitDone;
	bool m_fIsConstraint;
	bool m_fWizardAvailable;
	Evaluation** m_ppWeights;
	String m_sMapCol;
	Column colMapAttr;
	Table tblAttribute;
};

void StandardizationClass::ShowForm()
{
	CWnd* wnd = 0;

	AttributeFileName afnMap = FileName();
	FileName fnMap = FileName();
	FileName fnAttribTbl = FileName();
	bool fIsConstraint = false;
	if (m_pOwnerEffect)
	{
		fIsConstraint = m_pOwnerEffect->fIsConstraint();
		
		wnd = m_pOwnerEffect->GetDocument()->wndGetActiveView();
		
		// actually here we should check the input maps for all alternatives! Still TODO!
		afnMap = m_pOwnerEffect->afnFindFirstInputMap();
		fnMap = FileName(afnMap); // sCol is lost (as designed in FileName) which is necessary
		// try if we can find the attribtable of the map
		if (fnMap.fValid() && (fnMap != FileName()))
			fnAttribTbl = ObjectInfo::fnAttributeTable(fnMap);
	}
	String sAttrTable = fnAttribTbl.sFullPath();
	bool fOkClicked;
	bool fModified;
	{
		// block .. force out-of-scope of Table in frm so that there's no interference with Table in OK block below
		StandardizationClassForm frm(wnd, afnMap, &sAttrTable, &sAttribCol, &pWeights, fIsConstraint);
		fOkClicked = frm.fOkClicked();
		fModified = frm.fModified;
	}
	if (fOkClicked)
	{
		m_fStandardized = false; // check a few conditions to set to true; we check if the user edited the column after selecting it
		try
		{
			// try again to find the attribtable of the map
			if (fnMap.fValid() && (fnMap != FileName()))
				fnAttribTbl = ObjectInfo::fnAttributeTable(fnMap);
			Table tbl (fnAttribTbl);
			if (tbl.fValid())
			{
				Column col (tbl->col(sAttribCol));
				if (col.fValid())
				{
					RangeReal rr (col->rrMinMax());
					if (rr.fValid() && RangeReal(0,1).fContains(rr))
						m_fStandardized = true;
				}
				else
					MessageBox(wnd->GetSafeHwnd(), String(TR("Attribute '%S' not found in attribute table '%S'.").c_str(), sAttribCol, fnAttribTbl.sFileExt()).c_str(), TR("Problem with selected attribute").c_str(), MB_OK);
			}
		}
		catch (ErrorObject& err)
		{
			err.Show();
		}
		if (!m_fStandardized) // no success
			MessageBox(wnd->GetSafeHwnd(), TR("Please select an attribute with values in the range [0,1].").c_str(), TR("Problem with selected attribute").c_str(), MB_OK);

		SetModifiedFlag();
	}
	else if (fModified) // if user changed std parameters
		SetModifiedFlag();
}

void StandardizationClass::WriteElements(const char* sSection, const ElementContainer& en)
{
	Standardization::WriteElements(sSection, en);

	ObjectInfo::WriteElement(sSection, "AttribCol", en, sAttribCol);
	if (pWeights)
		pWeights->WriteElements(sSection, en);
}

void StandardizationClass::ReadElements(const char* sSection, const ElementContainer& en)
{
	Standardization::ReadElements(sSection, en);

	ObjectInfo::ReadElement(sSection, "AttribCol", en, sAttribCol);
	pWeights = Evaluation::CreateFromElementContainer(sSection, en, Evaluation::iMAXISONE, TR("Priority Values"));
}

//////////////////////////////////////////////////////////////////
// StandardizationBool
//////////////////////////////////////////////////////////////////

StandardizationBool::StandardizationBool(Effect* peOwner)
: Standardization(peOwner)
, rTrue(rUNDEF)
, rFalse(rUNDEF)
{

}

StandardizationBool::~StandardizationBool()
{

}

Standardization* StandardizationBool::create(Effect* peOwner)
{
	return new StandardizationBool(peOwner);
}

Standardization* StandardizationBool::copy(Effect* peOwner)
{
	StandardizationBool* stdReturn = new StandardizationBool(peOwner);
	stdReturn->m_fFormAllowed = m_fFormAllowed;
	stdReturn->m_fStandardized = m_fStandardized;
	stdReturn->rTrue = rTrue;
	stdReturn->rFalse = rFalse;
	return stdReturn;
}

int StandardizationBool::iCostBenefit()
{
	return 2;
}

String StandardizationBool::sDescription()
{
	return String("T=%lg;F=%lg", rTrue, rFalse);
}

String StandardizationBool::sStandardize(String sData)
{
	return String("iff(%S,%lg,%lg)", sData, rTrue, rFalse);
}

class StandardizationBoolForm: public FormWithDest
{
public:
	StandardizationBoolForm(CWnd* wPar, ValueRange m_ValueRange, double* rTrue, double* rFalse)
	: FormWithDest(wPar, TR("Standardize Boolean Input"))
	{
		FieldReal* frTrue = new FieldReal(root, TR("Value for TRUE"), rTrue, m_ValueRange);
		FieldReal* frFalse = new FieldReal(root, TR("Value for FALSE"), rFalse, m_ValueRange);

		SetMenHelpTopic("ilwismen\\smce_window_standardize_boolean.htm");

		create();
	}
};

void StandardizationBool::ShowForm()
{
	double _rTrue = rTrue;
	double _rFalse = rFalse;
	if ((_rTrue == rUNDEF) && (_rFalse == rUNDEF))
	{
			// we can give initial defaults to the member vars
		_rTrue = 1;
		_rFalse = 0;
	}

	CWnd* wnd = 0;
	if (m_pOwnerEffect)
		wnd = m_pOwnerEffect->GetDocument()->wndGetActiveView();
	StandardizationBoolForm frm(wnd, vrStd, &_rTrue, &_rFalse);
	if (frm.fOkClicked())
	{
		rTrue = _rTrue;
		rFalse = _rFalse;
		m_fStandardized = true;
		SetModifiedFlag();
	}
}

void StandardizationBool::WriteElements(const char* sSection, const ElementContainer& en)
{
	Standardization::WriteElements(sSection, en);

	ObjectInfo::WriteElement(sSection, "False", en, rFalse);
	ObjectInfo::WriteElement(sSection, "True", en, rTrue);
}

void StandardizationBool::ReadElements(const char* sSection, const ElementContainer& en)
{
	Standardization::ReadElements(sSection, en);

	ObjectInfo::ReadElement(sSection, "False", en, rFalse);
	ObjectInfo::ReadElement(sSection, "True", en, rTrue);
}

//////////////////////////////////////////////////////////////////
// StandardizationBoolConstraint
//////////////////////////////////////////////////////////////////

StandardizationBoolConstraint::StandardizationBoolConstraint(Effect* peOwner)
: Standardization(peOwner)
, m_eCostBenefit(iBENEFIT)
{
}

StandardizationBoolConstraint::~StandardizationBoolConstraint()
{
}

Standardization* StandardizationBoolConstraint::create(Effect* peOwner)
{
	return new StandardizationBoolConstraint(peOwner);
}

Standardization* StandardizationBoolConstraint::copy(Effect* peOwner)
{
	StandardizationBoolConstraint* stdReturn = new StandardizationBoolConstraint(peOwner);
	stdReturn->m_fFormAllowed = m_fFormAllowed;
	stdReturn->m_fStandardized = m_fStandardized;
	stdReturn->m_eCostBenefit = m_eCostBenefit;
	return stdReturn;
}

int StandardizationBoolConstraint::iCostBenefit()
{
	switch (m_eCostBenefit)
	{
		case StandardizationValue::iBENEFIT:
			return 0;
			break;
		case StandardizationValue::iCOST:
			return 1;
			break;
		default:
			return 0;
			break;
	}
}

String StandardizationBoolConstraint::sDescription()
{
	if (m_eCostBenefit == StandardizationValue::iBENEFIT)
		return TR("True");
	else // cost
		return TR("False");
}

String StandardizationBoolConstraint::sStandardize(String sData)
{
	if (m_eCostBenefit == iBENEFIT)
		return sData;
	else // cost
		return String("not %S", sData);
}

class StandardizationBoolConstraintForm: public FormWithDest
{
public:
	StandardizationBoolConstraintForm(CWnd* wPar, int* iCostBenefit)
	: FormWithDest(wPar, TR("Standardize Boolean Input"))
	{
		RadioGroup* rgCostBenefit = new RadioGroup(root, "", iCostBenefit);
		RadioButton* rbBenefit = new RadioButton(rgCostBenefit, TR("TRUE passes, FALSE will be blocked"));
		RadioButton* rbCost = new RadioButton(rgCostBenefit, TR("FALSE passes, TRUE will be blocked"));
		
		SetMenHelpTopic("ilwismen\\smce_window_standardize_boolean.htm");
		
		create();
	}
};

void StandardizationBoolConstraint::ShowForm()
{
	CWnd* wnd = 0;
	if (m_pOwnerEffect)
		wnd = m_pOwnerEffect->GetDocument()->wndGetActiveView();
	int iCostBenefit = (m_eCostBenefit == iBENEFIT) ? 0 : 1; // see form: benefit = 0; cost = 1
	int iCostBenefitPrevious = iCostBenefit;
	StandardizationBoolConstraintForm frm(wnd, &iCostBenefit);
	if (frm.fOkClicked())
	{
		m_fStandardized = true;
		m_eCostBenefit = (iCostBenefit == 0) ? iBENEFIT : iCOST;
		SetModifiedFlag();
	}
}

void StandardizationBoolConstraint::WriteElements(const char* sSection, const ElementContainer& en)
{
	Standardization::WriteElements(sSection, en);
	
	ObjectInfo::WriteElement(sSection, "CostBenefit", en, sCostBenefitTypeToString(m_eCostBenefit));
}

void StandardizationBoolConstraint::ReadElements(const char* sSection, const ElementContainer& en)
{
	Standardization::ReadElements(sSection, en);

	String sType;
	ObjectInfo::ReadElement(sSection, "CostBenefit", en, sType);
	m_eCostBenefit = eCostBenefitTypeFromString(sType);
}
