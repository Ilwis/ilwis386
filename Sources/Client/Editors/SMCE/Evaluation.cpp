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
// Evaluation.cpp: implementation of the Evaluation class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Client\Editors\SMCE\Evaluation.h"
#include "Client\FormElements\FormBasePropertyPage.h"
#include "Client\FormElements\FieldIntSlider.h"
#include "Client\FormElements\FldOneSelectTextOnly.h"
#include "Headers\Hs\smce.hs"
#include "Headers\Htp\Ilwismen.htp"
#include <vector>

//////////////////////////////////////////////////////////////////////
// Evaluation
//////////////////////////////////////////////////////////////////////

Evaluation::Evaluation(eResultType _eResult, const String& sTitle)
: eResult(_eResult)
, m_sTitle(sTitle)
{

}

Evaluation::~Evaluation()
{

}

class EvaluationForm: public FormWithDest
{
public:
  EvaluationForm(CWnd* wPar, String sTitle, int* iType)
    : FormWithDest(wPar, sTitle)
  {
		RadioGroup* rgType = new RadioGroup(root, TR("Method"), iType);
		new RadioButton(rgType, TR("Direct"));
		new RadioButton(rgType, TR("Pairwise"));
		new RadioButton(rgType, TR("Rank Order"));

		create();
	}
};

Evaluation* Evaluation::create(CWnd* wPar, list <String> *liNames, map <String, double> *mp, Evaluation* ePrevious, eResultType _eResult, const String& sTitle)
{
	enum eEvaluationType{iDIRECT, iPAIRWISE, iRANKORDER} eET;
	if (mp->size() >0)
	{
		EvaluationForm frm (wPar, sTitle, (int*)&eET);
		if (frm.fOkClicked())
		{
			switch (eET)
			{
			case iDIRECT:
				if ((ePrevious) && (0 != dynamic_cast<DirectEvaluation*>(ePrevious)))
				{
					eActionTP action = ePrevious->ShowForm(wPar, liNames, mp);
					if (action == iOK || action == iCANCEL)
						return ePrevious;
					else
						return Evaluation::create(wPar, liNames, mp, ePrevious, _eResult, sTitle); // start over
				}
				else
					return DirectEvaluation::create(wPar, liNames, mp, ePrevious, _eResult, sTitle);
				break;
			case iPAIRWISE:
				if ((ePrevious) && (0 != dynamic_cast<PairwiseComparison*>(ePrevious)))
				{
					eActionTP action = ePrevious->ShowForm(wPar, liNames, mp);
					if (action == iOK || action == iCANCEL)
						return ePrevious;
					else
						return Evaluation::create(wPar, liNames, mp, ePrevious, _eResult, sTitle); // start over
				}
				else
					return PairwiseComparison::create(wPar, liNames, mp, ePrevious, _eResult, sTitle);
				break;
			case iRANKORDER:
				if ((ePrevious) && (0 != dynamic_cast<RankOrdering*>(ePrevious)))
				{
					eActionTP action = ePrevious->ShowForm(wPar, liNames, mp);
					if (action == iOK || action == iCANCEL)
						return ePrevious;
					else
						return Evaluation::create(wPar, liNames, mp, ePrevious, _eResult, sTitle); // start over
				}
				else
					return RankOrdering::create(wPar, liNames, mp, ePrevious, _eResult, sTitle);
				break;
			}
		}
	}
	return ePrevious;
}

Evaluation* Evaluation::CreateFromElementContainer(const char* sSection, const ElementContainer& en, eResultType _eResult, const String& sTitle)
{
	Evaluation * eReturn = 0;
	String sType;
	ObjectInfo::ReadElement(sSection, "StdType", en, sType);

	if ("Direct" == sType)
	{
		eReturn = new DirectEvaluation(_eResult, sTitle);
		eReturn->ReadElements(sSection, en);
	}
	else if ("Pairwise" == sType)
	{
		eReturn = new PairwiseComparison(_eResult, sTitle);
		eReturn->ReadElements(sSection, en);
	}
	else if ("RankOrder" == sType)
	{
		eReturn = new RankOrdering(_eResult, sTitle);
		eReturn->ReadElements(sSection, en);
	}

	return eReturn;
}

//////////////////////////////////////////////////////////////////////
// DirectEvaluation
//////////////////////////////////////////////////////////////////////

DirectEvaluation::DirectEvaluation(eResultType _eResult, const String& sTitle)
: Evaluation(_eResult, sTitle)
{

}

DirectEvaluation::~DirectEvaluation()
{

}

String DirectEvaluation::sDescription()
{
	return TR("Direct");
}

void DirectEvaluation::Refresh(map <String, double> *mp)
{
	map <String, double> mpTemp;
	for (map <String, double>::iterator it = mp->begin(); it != mp->end(); ++it)
		mpTemp[(*it).first]=mapDirectEvaluation[(*it).first];
	// this will cleanup
	mapDirectEvaluation = mpTemp;
}

class DirectEvaluationForm: public FormWithDest
{
public:
  DirectEvaluationForm(CWnd* wPar, list <String> *liNames, map <String, double> *mapEvaluation, map <String, double> * mp, bool* fChangeMethod, Evaluation::eResultType _eResult, String sLabel)
    : FormWithDest(wPar, TR("Direct Method"))
		, m_rSum(0)
		, m_rNormSum(0)
		, m_mapEvaluation(mapEvaluation)
		, m_mp(mp)
		, m_fChangeMethod(fChangeMethod)
		, m_eResult(_eResult)
		, iSize(mapEvaluation->size())
		, vrPrecision(ValueRange(0, LONG_MAX, 0.001))
  {
		vfrUserWeight.resize(iSize);
		vfrNormWeight.resize(iSize);

		FieldGroup * fgLeft = new FieldGroup(root);
		FieldGroup * fgRight = new FieldGroup(root);
		fgRight->Align(fgLeft, AL_AFTER);

		StaticText* stItemsTitle = new StaticText(fgLeft, TR("Items"));
		StaticText* stEvaluationTitle = new StaticText(fgLeft, sLabel);
		stEvaluationTitle->Align(stItemsTitle, AL_AFTER);
		StaticText* stNormalizedTitle = new StaticText(fgRight, TR("Normalized"));

		int i = 0;
		for (list <String>::iterator it = liNames->begin(); it != liNames->end(); ++it)
		{
			vfrUserWeight[i] = new FieldReal(fgLeft, (*it), &(*mapEvaluation)[(*it)], vrPrecision);
			vfrUserWeight[i]->SetCallBack((NotifyProc)&DirectEvaluationForm::CallBackFunc);
			if (i>0)
				vfrUserWeight[i]->Align(vfrUserWeight[i-1], AL_UNDER);
			else
				vfrUserWeight[i]->Align(stItemsTitle, AL_UNDER);
			vfrNormWeight[i] = new FieldReal(fgRight, "", &((*m_mp)[(*it)]), vrPrecision);
			if (i>0)
				vfrNormWeight[i]->Align(vfrNormWeight[i-1], AL_UNDER);
			else
				vfrNormWeight[i]->Align(stNormalizedTitle, AL_UNDER);
			++i;
		}
		frSum = new FieldReal(fgLeft, TR("Sum"), &m_rSum, vrPrecision);
		if (i>0)
			frSum->Align(vfrUserWeight[i-1], AL_UNDER);
		else
			frSum->Align(stItemsTitle, AL_UNDER);
		frNormSum = new FieldReal(fgRight, "", &m_rNormSum, vrPrecision);
		if (i>0)
			frNormSum->Align(vfrNormWeight[i-1], AL_UNDER);
		else
			frNormSum->Align(stNormalizedTitle, AL_UNDER);

		PushButton* pbChangeMethod = new PushButton(root, TR("Choose other method"), (NotifyProc)&DirectEvaluationForm::PushButtonCallBack);
		pbChangeMethod->Align(fgLeft, AL_UNDER);

		SetMenHelpTopic("ilwismen\\smce_window_weigh_direct.htm");

		create();
	}
	int PushButtonCallBack(Event*)
	{
		*m_fChangeMethod = true;
		EndDialog(IDCANCEL);
		return 0;
	}
	int CallBackFunc(Event*)
	{
		m_rSum = 0;
		m_rNormSum = 0;
		double rMax = 0;

		int i;
		for (i=0; i<iSize; ++i)
		{
			m_rSum += vfrUserWeight[i]->rVal();
			rMax = max(rMax, vfrUserWeight[i]->rVal());
		}

		frSum->SetVal(m_rSum);
		frSum->SetReadOnly(true);

		if (m_rSum == 0)
			DisableOK();
		else
		{
			EnableOK();

			for (i=0; i<iSize; ++i)
			{
				double rVal = 0;
				if (Evaluation::iSUMTOONE == m_eResult)
					rVal = vfrUserWeight[i]->rVal()/m_rSum;
				else if (Evaluation::iMAXISONE == m_eResult)
					rVal = vfrUserWeight[i]->rVal()/rMax;
				vfrNormWeight[i]->SetVal(rVal);
				vfrNormWeight[i]->SetReadOnly(true);
				m_rNormSum += rVal;
			}
		}

		for (i=0; i<iSize; ++i)
			vfrNormWeight[i]->SetReadOnly(true);

		frNormSum->SetVal(m_rNormSum);
		frNormSum->SetReadOnly(true);

		return 0;
	}
private:
	vector <FieldReal*> vfrUserWeight;
	map <String, double> *m_mapEvaluation;
	vector <FieldReal*> vfrNormWeight;
	map <String, double> *m_mp;
	FieldReal* frSum, *frNormSum;
	double m_rSum, m_rNormSum;
	bool* m_fChangeMethod;
	int iSize;
	Evaluation::eResultType m_eResult;
	ValueRange vrPrecision;
};

Evaluation::eActionTP DirectEvaluation::ShowForm(CWnd* wPar, list <String> *liNames, map <String, double> * mp)
{
	bool fChangeMethod = false;
	Refresh(mp); // in case there was some resizing since last time
	map <String, double> mapDirectEvaluationCopy = mapDirectEvaluation;
	map <String, double> mapUserEvaluationCopy = *mp;
	DirectEvaluationForm frm (wPar, liNames, &mapDirectEvaluationCopy, &mapUserEvaluationCopy, &fChangeMethod, eResult, m_sTitle);
	if (frm.fOkClicked())
	{
		mapDirectEvaluation = mapDirectEvaluationCopy;
		*mp = mapUserEvaluationCopy;
		return iOK;
	}
	return fChangeMethod?iOTHERMETHOD:iCANCEL;
}

void DirectEvaluation::Rename(String oldName, String newName)
{
	if (oldName != newName)
	{
		double rBackup = mapDirectEvaluation[oldName];
		mapDirectEvaluation.erase(oldName);
		mapDirectEvaluation[newName] = rBackup;
	}
}

Evaluation* DirectEvaluation::create(CWnd* wPar, list <String> *liNames, map <String, double> *mp, Evaluation* ePrevious, eResultType _eResult, const String& sTitle)
{
	DirectEvaluation* deNew = new DirectEvaluation(_eResult, sTitle);
	eActionTP action = deNew->ShowForm(wPar, liNames, mp);
	if (action == iOK)
		return deNew;
	else if (action == iCANCEL)
	{
		delete deNew; // user doesn't want it
		return ePrevious;
	}
	else
	{
		delete deNew; // user doesn't want it
		return Evaluation::create(wPar, liNames, mp, ePrevious, _eResult, sTitle); // try all over again!
	}
}

void DirectEvaluation::WriteElements(const char* sSection, const ElementContainer& en)
{
	ObjectInfo::WriteElement(sSection, "StdType", en, "Direct");
	int iSize = mapDirectEvaluation.size();
	ObjectInfo::WriteElement(sSection, "NrElements", en, iSize);
	int i=0;
	for (map <String, double>::iterator it = mapDirectEvaluation.begin(); it != mapDirectEvaluation.end(); ++it)
	{
		ObjectInfo::WriteElement(sSection, String("Element%d", i).c_str(), en, (*it).first);
		ObjectInfo::WriteElement(sSection, String("Value%d", i).c_str(), en, (*it).second);
		++i;
	}
}

void DirectEvaluation::ReadElements(const char* sSection, const ElementContainer& en)
{
	// type already known .. we're "new"-ed
	int iSize;
	if (!ObjectInfo::ReadElement(sSection, "NrElements", en, iSize))
		iSize = 0;
	mapDirectEvaluation.clear();
	for (int i=0; i<iSize; ++i)
	{
		String str;
		double d;
		ObjectInfo::ReadElement(sSection, String("Element%d", i).c_str(), en, str);
		ObjectInfo::ReadElement(sSection, String("Value%d", i).c_str(), en, d);
		mapDirectEvaluation[str]=d;
	}
}

void DirectEvaluation::Recalculate(list <String> *liNames, map <String, double> * mp)
{
	Refresh(mp); // in case there was some resizing since last time
	if (iSUMTOONE == eResult)
	{
		double rSum = 0;

		for (list <String>::iterator it = liNames->begin(); it != liNames->end(); ++it)
			rSum += mapDirectEvaluation[(*it)];

		if (rSum != 0)
		{
			for (list <String>::iterator it = liNames->begin(); it != liNames->end(); ++it)
				(*mp)[(*it)] = mapDirectEvaluation[(*it)]/rSum;
		}
		else
		{
			// sum = 0 .. make sure it becomes 1 !!
			list <String>::iterator it = liNames->begin();
			if (it != liNames->end())
				(*mp)[(*it)] = 1;
		}
	}
	else if (iMAXISONE == eResult)
	{
		double rMax = 0;

		for (list <String>::iterator it = liNames->begin(); it != liNames->end(); ++it)
			rMax = max(rMax, mapDirectEvaluation[(*it)]);

		if (rMax != 0)
		{
			for (list <String>::iterator it = liNames->begin(); it != liNames->end(); ++it)
				(*mp)[(*it)] = mapDirectEvaluation[(*it)]/rMax;
		}
		else
		{
			// max = 0 .. make sure it becomes 1 !!
			list <String>::iterator it = liNames->begin();
			if (it != liNames->end())
				(*mp)[(*it)] = 1;
		}
	}
}

//////////////////////////////////////////////////////////////////////
// PairwiseComparison
//////////////////////////////////////////////////////////////////////

PairwiseComparison::PairwiseComparison(eResultType _eResult, const String& sTitle)
: Evaluation(_eResult, sTitle)
{

}

PairwiseComparison::~PairwiseComparison()
{

}

String PairwiseComparison::sDescription()
{
	return TR("Pairwise");
}

void PairwiseComparison::Refresh(map <String, double> *mp)
{
	map <pair<String, String>, double> mpTemp;
	for (map <String, double>::iterator it = mp->begin(); it != mp->end(); ++it)
		for (map <String, double>::iterator it2 = mp->begin(); it2 != mp->end(); ++it2)
			mpTemp[pair<String,String>((*it).first,(*it2).first)]=mapPairwiseEvaluation[pair<String,String>((*it).first,(*it2).first)];
	// this will cleanup
	mapPairwiseEvaluation = mpTemp;
}

Evaluation* PairwiseComparison::create(CWnd* wPar, list <String> *liNames, map <String, double> *mp, Evaluation* ePrevious, eResultType _eResult, const String& sTitle)
{
	PairwiseComparison* pcNew = new PairwiseComparison(_eResult, sTitle);
	eActionTP action = pcNew->ShowForm(wPar, liNames, mp);
	if (action == iOK)
		return pcNew;
	else if (action == iCANCEL)
	{
		delete pcNew; // user doesn't want it
		return ePrevious;
	}
	else
	{
		delete pcNew; // user doesn't want it
		return Evaluation::create(wPar, liNames, mp, ePrevious, _eResult, sTitle); // try all over again!
	}
}

void PairwiseComparison::WriteElements(const char* sSection, const ElementContainer& en)
{
	ObjectInfo::WriteElement(sSection, "StdType", en, "Pairwise");
	int iSize = mapPairwiseEvaluation.size();
	ObjectInfo::WriteElement(sSection, "NrElements", en, iSize);
	int i=0;
	for (map <pair <String, String>, double>::iterator it = mapPairwiseEvaluation.begin(); it != mapPairwiseEvaluation.end(); ++it)
	{
		ObjectInfo::WriteElement(sSection, String("ElementX%d", i).c_str(), en, (*it).first.first);
		ObjectInfo::WriteElement(sSection, String("ElementY%d", i).c_str(), en, (*it).first.second);
		ObjectInfo::WriteElement(sSection, String("Value%d", i).c_str(), en, (*it).second);
		++i;
	}
}

void PairwiseComparison::ReadElements(const char* sSection, const ElementContainer& en)
{
	// type already known .. we're "new"-ed
	int iSize;
	if (!ObjectInfo::ReadElement(sSection, "NrElements", en, iSize))
		iSize = 0;
	mapPairwiseEvaluation.clear();
	for (int i=0; i<iSize; ++i)
	{
		String str1;
		String str2;
		double d;
		ObjectInfo::ReadElement(sSection, String("ElementX%d", i).c_str(), en, str1);
		ObjectInfo::ReadElement(sSection, String("ElementY%d", i).c_str(), en, str2);
		ObjectInfo::ReadElement(sSection, String("Value%d", i).c_str(), en, d);
		mapPairwiseEvaluation[pair<String,String>(str1,str2)]=d;
	}
}

void PairwiseComparison::Rename(String oldName, String newName)
{
	if (oldName != newName)
	{
		typedef pair<String, String> StringPair; // otherwise compiler gives syntax error!!
		list <StringPair> liToErase; // to collect the items to be deleted (deletion in for loop confuses the iterator and crashes)
		for (map <pair <String, String>, double>::iterator it = mapPairwiseEvaluation.begin(); it != mapPairwiseEvaluation.end(); ++it)
		{
			if ((*it).first.first == oldName)
			{
				if ((*it).first.second == oldName)
				{
					// special case: the diagonal
					double rBackup = (*it).second;
					liToErase.push_back(pair<String, String>(oldName, oldName));
					mapPairwiseEvaluation[pair<String, String>(newName, newName)] = rBackup;
				}
				else
				{
					double rBackup = (*it).second;
					String sSecond = (*it).first.second;
					liToErase.push_back(pair<String, String>(oldName, sSecond));
					mapPairwiseEvaluation[pair<String, String>(newName, sSecond)] = rBackup;
				}
			}
			else if ((*it).first.second == oldName)
			{
				double rBackup = (*it).second;
				String sFirst = (*it).first.first;
				liToErase.push_back(pair<String, String>(sFirst, oldName));
				mapPairwiseEvaluation[pair<String, String>(sFirst, newName)] = rBackup;
			}
		}
		for (list<StringPair>::iterator it2 = liToErase.begin(); it2 != liToErase.end(); ++it2)
			mapPairwiseEvaluation.erase(*it2);
	}
}

void CalculateSaatyWeights(list <String> *liNames, map <pair <String, String>, double> *mapPairwiseEvaluation, map <String, double> *mp, double *rConsistency, Evaluation::eResultType _eResult)
{
	// Calculate values according to Eigenvalue Method
	// Pascal source code from Definite translated to C, but the original source is
	// An Introduction to Numerical Methods with Pascal, L.V. Atkinson & P.J. Harley;
	// Addison-Wesley Publishers Limited, 1983

	map <String, double> v;
	map <String, double> y;
	for (list <String>::iterator it = liNames->begin(); it != liNames->end(); ++it)
		v[(*it)] = 1;
	int iCounter = 0;
	const int iMaxCounter = 100;
	const double epsilon = 0.001;
	const double rConsistencyDivider[7] = {0.609, 0.961, 1.182, 1.322, 1.394, 1.475, 1.493};
	double max;
	bool fOk;
	do
	{
		++iCounter;
		max = 0;

		for (list <String>::iterator itX = liNames->begin(); itX != liNames->end(); ++itX)
		{
			double m = 0;
			for (list <String>::iterator itY = liNames->begin(); itY != liNames->end(); ++itY)
				m += (*mapPairwiseEvaluation)[pair<String, String>((*itX),(*itY))] * v[(*itY)];
			y[(*itX)] = m;
			if (abs(m) > abs(max))
				max = m;
		}

		fOk = true;

		for (list <String>::iterator it = liNames->begin(); it != liNames->end(); ++it)
		{
			double newv = (max != 0) ? y[(*it)]/max : 0;
			if (abs(newv-v[(*it)]) > epsilon)
				fOk = false;
			v[(*it)] = newv;
		}
	} while (!fOk && (iCounter < iMaxCounter));

	if (!fOk)
	{
		// Eigenvalue method failed (?? undocumented why Definite does this) but under this
		// condition Definite uses the Least Squares Method:
		*rConsistency = -1;
		for (list <String>::iterator itX = liNames->begin(); itX != liNames->end(); ++itX)
		{
			(*mp)[(*itX)] = 1;
			for (list <String>::iterator itY = liNames->begin(); itY != liNames->end(); ++itY)
				(*mp)[(*itX)] = (*mp)[(*itX)] * (*mapPairwiseEvaluation)[pair<String, String>((*itX),(*itY))];
			if (0 != (*mp)[(*itX)])
				(*mp)[(*itX)] = exp(log((*mp)[(*itX)])/liNames->size());
		}
	}
	else
	{
		for (list <String>::iterator it = liNames->begin(); it != liNames->end(); ++it)
			(*mp)[(*it)] = v[(*it)];
		// consistency index calc, but only for 3 to 9 criteria
		if ((mp->size() >= 3) && (mp->size() <= 9))
		{
			*rConsistency = (max - mp->size())/(mp->size() - 1);
			*rConsistency /= rConsistencyDivider[mp->size() - 3];
		}
		else
			*rConsistency = -1;
	}

	if (Evaluation::iSUMTOONE == _eResult)
	{
		// make sure they sum to 1

		double rSum = 0;

		for (list <String>::iterator it = liNames->begin(); it != liNames->end(); ++it)
			rSum += (*mp)[(*it)];

		if (rSum != 0)
		{
			for (list <String>::iterator it = liNames->begin(); it != liNames->end(); ++it)
				(*mp)[(*it)] /= rSum;
		}
		else
		{
			// sum = 0 .. make sure it becomes 1 !!
			list <String>::iterator it = liNames->begin();
			if (it != liNames->end())
				(*mp)[(*it)] = 1;
		}

	}
	else if (Evaluation::iMAXISONE == _eResult)
	{
		// make sure max is 1

		double rMax = 0;

		for (list <String>::iterator it = liNames->begin(); it != liNames->end(); ++it)
			rMax = max (rMax, (*mp)[(*it)]);

		if (rMax != 0)
		{
			for (list <String>::iterator it = liNames->begin(); it != liNames->end(); ++it)
				(*mp)[(*it)] /= rMax;
		}
		else
		{
			// max = 0 .. make sure it becomes 1 !!
			list <String>::iterator it = liNames->begin();
			if (it != liNames->end())
				(*mp)[(*it)] = 1;
		}
	}
}

void PairwiseComparison::Recalculate(list <String> *liNames, map <String, double> * mp)
{
	Refresh(mp); // in case there was some resizing since last time
	double rConsistencyDummy;
	CalculateSaatyWeights(liNames, &mapPairwiseEvaluation, mp, &rConsistencyDummy, eResult);
}

class CompareTwoForm : public FormBaseWizardPage
{
public:
	CompareTwoForm(const String& sTitle, list <String> *liNames, map <pair <String, String>, double> *mapPairwiseEvaluation, vector <bool> * vfComparisonDone, String sX, String sY, int iCurrentComparisonIndex, int iTotalNrComparisons, bool* fChangeMethod)
	: FormBaseWizardPage(sTitle)
	, m_sX(sX)
	, m_sY(sY)
	, m_fChangeMethod(fChangeMethod)
	, m_mp(mapPairwiseEvaluation)
	, m_vfComparisonDone(vfComparisonDone)
	, m_iCurrentComparisonIndex(iCurrentComparisonIndex)
	, m_fUsingCheckBoxes(false)
	, fFirstTime(true)
	{
		iOption = iDoubleToOption((*m_mp)[pair<String, String>(sX, sY)]);
		vsSaatyOptions.clear();
    vsSaatyOptions.resize(9);
    vsSaatyOptions[0] = TR("is extremely more important than");
    vsSaatyOptions[1] = TR("is very strongly more important than");
    vsSaatyOptions[2] = TR("is strongly more important than");
		vsSaatyOptions[3] = TR("is moderately more important than");
		vsSaatyOptions[4] = TR("is equally important as");
		vsSaatyOptions[5] = TR("is moderately less important than");
		vsSaatyOptions[6] = TR("is strongly less important than");
		vsSaatyOptions[7] = TR("is very strongly less important than");
		vsSaatyOptions[8] = TR("is extremely less important than");
		sComboString = vsSaatyOptions[iOption];

		FieldGroup * fgLeft = new FieldGroup(root);
		FieldGroup * fgRight = new FieldGroup(root);
		fgRight->Align(fgLeft, AL_AFTER);

		StaticText * stLabel = new StaticText(fgLeft, TR("Current comparison:"));
		stLabel->SetIndependentPos();
		
		ValueRange vrRange(0, 8);
		fisRelation = new FieldIntSlider(fgLeft, &iOption, vrRange, TBS_VERT|TBS_AUTOTICKS|TBS_RIGHT);
		fisRelation->SetCallBack((NotifyProc)&CompareTwoForm::SliderCallBackFunc);
		fisRelation->SetHeight(90);
		fisRelation->SetIndependentPos();

		StaticText * stX = new StaticText(fgLeft, String("%S", sX));
		stX->Align(fisRelation, AL_AFTER);
		fosRelation = new FieldOneSelectTextOnly(fgLeft, &sComboString);
		fosRelation->SetWidth(100);
		fosRelation->SetCallBack((NotifyProc)&CompareTwoForm::ComboCallBackFunc);
		fosRelation->Align(stX, AL_UNDER, 10);
		StaticText * stY = new StaticText(fgLeft, String("%S", sY));
		stY->Align(fosRelation, AL_UNDER, 20);

		if (iTotalNrComparisons < 28)
		{
			m_fUsingCheckBoxes = true;
			m_rbComparison.resize(iTotalNrComparisons);
			m_cbComparison.resize(iTotalNrComparisons);
			int iCount = 0;
			RadioGroup* rgComparisons = new RadioGroup(fgRight, TR("Comparison Progress:"), &iRadioDummy);
			for (list <String>::iterator itX = liNames->begin(); itX != liNames->end(); ++itX)
				for (list <String>::iterator itY = itX; itY != liNames->end(); ++itY)
					if (itY != itX)
					{
						m_cbComparison[iCount] = new CheckBox(rgComparisons, "", (bool *)&((*m_vfComparisonDone)[iCount]));
						m_cbComparison[iCount]->SetStyle(true); // disabled
						m_cbComparison[iCount]->SetIndependentPos();
						m_rbComparison[iCount] = new RadioButton(rgComparisons, String("%S, %S", *itX, *itY));
						m_rbComparison[iCount]->SetCallBack((NotifyProc)&CompareTwoForm::RadioCallBackFunc);
						m_rbComparison[iCount]->Align(m_cbComparison[iCount], AL_AFTER);
						m_IdToIndex[m_rbComparison[iCount]->Id()] = iCount;
						m_cbComparison[iCount]->SetHeight(13);
						m_cbComparison[iCount]->SetWidth(8);
						m_rbComparison[iCount]->SetHeight(13);
						++iCount;
					}
		}
		else // we'd have too many comparisons (rows) to display
			new StaticText(fgRight, String(TR("Comparison %d of %d").c_str(), m_iCurrentComparisonIndex + 1, iTotalNrComparisons));

		PushButton* pbChangeMethod = new PushButton(root, TR("Choose other method"), (NotifyProc)&CompareTwoForm::PushButtonCallBack);
		pbChangeMethod->SetIndependentPos();
		pbChangeMethod->Align(fgLeft, AL_UNDER, 10);

		SetMenHelpTopic("ilwismen\\smce_window_weigh_pairwise.htm");

		create();
	}
	int PushButtonCallBack(Event*)
	{
		*m_fChangeMethod = true;
		EndDialog(IDCANCEL);
		return 0;
	}
	int ComboCallBackFunc(Event*)
	{
		if (fFirstTime)
		{
			fFirstTime = false;
			for (vector<string>::iterator it = vsSaatyOptions.begin(); it != vsSaatyOptions.end(); ++it)
				fosRelation->AddString(*it);
		}

		fisRelation->SetVal(fosRelation->ose->GetCurSel());
		return 0;
	}
	int SliderCallBackFunc(Event*)
	{
		fosRelation->ose->SetCurSel(fisRelation->iVal());
		return 0;
	}
	int RadioCallBackFunc(Event * ev)
	{
		if (ev)
		{
			PairwiseComparisonWizard* pcw = (PairwiseComparisonWizard*)GetParent();	
			pcw->ActivatePage(m_IdToIndex[ev->wParm]);
		}
		return 0;
	}
	BOOL OnSetActive()
	{
		if (m_fUsingCheckBoxes)
		{
			int i=0;
			for (vector <CheckBox*>::iterator cbIt = m_cbComparison.begin(); cbIt != m_cbComparison.end(); ++cbIt)
			{
				(*cbIt)->SetVal((*m_vfComparisonDone)[i]);
				++i;
			}
			i=0;
			for (vector <RadioButton*>::iterator rbIt = m_rbComparison.begin(); rbIt != m_rbComparison.end(); ++rbIt)
			{
				(*rbIt)->SetVal(m_iCurrentComparisonIndex == i);
				++i;
			}
		}

		return FormBaseWizardPage::OnSetActive();
	}
	LRESULT OnWizardNext()
	{
		if (m_fUsingCheckBoxes)
		{
			// implementation taken partly from FormBaseWizardPage::OnWizardNext()
			FormEntry *pfe = CheckData();
			if (pfe) 
			{
				MessageBeep(MB_ICONEXCLAMATION);
				pfe->SetFocus();
				return -1;
			}
			_fOkClicked = true;
			exec();
			
			CPropertySheet* ps = (CPropertySheet*)GetParent();
			int iP = ps->GetPageIndex(this);
			// first preference: unchecked pages towards the end
			// second preference: unchecked pages starting from first page till current - 1
			// last preference: jump to last page (the one with Results and <Finish>)

			// default: next = last page
			FormBaseWizardPage* pGoNext = dynamic_cast<FormBaseWizardPage*>(ps->GetPage(ps->GetPageCount() - 1));
			
			// try looking for unchecked page from beginning to current - 1
			for (int i=0; i < iP; ++i)
			{
				if (!(*m_vfComparisonDone)[i])
				{
					FormBaseWizardPage* pp = dynamic_cast<FormBaseWizardPage*>(ps->GetPage(i));
					if (pp->fPPEnabled())
					{
						pGoNext = pp;
						break; // stop for loop at first page found
					}
				}
			}

			// try looking for unchecked page towards end (careful: last page does not have a "check")
			for (int i=iP+1; i < ps->GetPageCount() - 1; ++i)
			{
				if (!(*m_vfComparisonDone)[i])
				{
					FormBaseWizardPage* pp = dynamic_cast<FormBaseWizardPage*>(ps->GetPage(i));
					if (pp->fPPEnabled())
					{
						pGoNext = pp;
						break; // stop for loop at first page found
					}
				}
			}

			ps->SetActivePage(pGoNext);

			return -1;
		}
		else // large number of comparisons .. display 1 of 26, 2 of 26 etc.
			return FormBaseWizardPage::OnWizardNext();
	}
	int exec()
	{
		fisRelation->StoreData(); // now iOption has the new value
		(*m_mp)[pair<String, String>((m_sX), (m_sY))] = rOptionToDouble(iOption);
		(*m_mp)[pair<String, String>((m_sY), (m_sX))] = 1.0/rOptionToDouble(iOption);
		(*m_vfComparisonDone)[m_iCurrentComparisonIndex] = true;
		return 0;
	}
	int iDoubleToOption(double d)
	{
		const double rEpsilon = 0.001;
		if (abs(d-1.0/9.0) < rEpsilon)
			return 8;
		else if (abs(d-1.0/7.0) < rEpsilon)
			return 7;
		else if (abs(d-1.0/5.0) < rEpsilon)
			return 6;
		else if (abs(d-1.0/3.0) < rEpsilon)
			return 5;
		else if (abs(d-1.0) < rEpsilon)
			return 4;
		else if (abs(d-3.0) < rEpsilon)
			return 3;
		else if (abs(d-5.0) < rEpsilon)
			return 2;
		else if (abs(d-7.0) < rEpsilon)
			return 1;
		else if (abs(d-9.0) < rEpsilon)
			return 0;
		else
			return 4; // default is "equal" (for d other than 9,7,5,3,1,1/3,1/5,1/7,1/9, thus also for d=0 which is the default that stl puts into the map when an element does not exist)
	}
	double rOptionToDouble(int iOpt)
	{
		switch (iOpt)
		{
		case 0:
			return 9.0;
			break;
		case 1:
			return 7.0;
			break;
		case 2:
			return 5.0;
			break;
		case 3:
			return 3.0;
			break;
		case 4:
			return 1.0;
			break;
		case 5:
			return 1.0/3.0;
			break;
		case 6:
			return 1.0/5.0;
			break;
		case 7:
			return 1.0/7.0;
			break;
		case 8:
			return 1.0/9.0;
			break;
		default:
			return 0.0; // error!!
			break;
		}
	}
private:
	int iOption;
	String sComboString;
	vector <bool> * m_vfComparisonDone;
	vector <CheckBox*> m_cbComparison;
	vector <RadioButton*> m_rbComparison;
	bool m_fUsingCheckBoxes;
	int m_iCurrentComparisonIndex;
	map <int, int> m_IdToIndex;
	bool fFirstTime;
	vector <string> vsSaatyOptions;
	FieldOneSelectTextOnly *fosRelation;
	FieldIntSlider * fisRelation;
	map <pair <String, String>, double> *m_mp;
	String m_sX;
	String m_sY;
	bool *m_fChangeMethod;
	int iRadioDummy;
};

class ShowPairwiseComparisonResultsForm : public FormBaseWizardPage
{
public:
	ShowPairwiseComparisonResultsForm(const String& sTitle, list <String> *liNames, map <pair <String, String>, double> *mapPairwiseEvaluation, map <String, double> * mp, bool* fChangeMethod, Evaluation::eResultType _eResult, String sLabel)
	: FormBaseWizardPage(sTitle)
	, m_liNames(liNames)
	, m_mapPairwiseEvaluation(mapPairwiseEvaluation)
	, m_mp(mp)
	, m_fChangeMethod(fChangeMethod)
	, m_eResult(_eResult)
	, vrPrecision(ValueRange(0, LONG_MAX, 0.001))
	{
		vfrNormWeight.resize(liNames->size());

		StaticText* stNormalizedTitle = new StaticText(root, String (TR("Resulting Normalized %S").c_str(), sLabel));

		int i = 0;
		for (list <String>::iterator it = liNames->begin(); it != liNames->end(); ++it)
		{
			vfrNormWeight[i] = new FieldReal(root, (*it), &((*m_mp)[(*it)]), vrPrecision);
			vfrNormWeight[i]->SetCallBack((NotifyProc)&ShowPairwiseComparisonResultsForm::CallBackFunc);
			if (i>0)
				vfrNormWeight[i]->Align(vfrNormWeight[i-1], AL_UNDER);
			else
				vfrNormWeight[i]->Align(stNormalizedTitle, AL_UNDER);
			++i;
		}
		frConsistency = new FieldReal(root, TR("Inconsistency ratio:"), &rConsistency);
		frConsistency->SetCallBack((NotifyProc)&ShowPairwiseComparisonResultsForm::CallBackFunc);
		stConsistency = new StaticText(root, TR("A value above 0.1 is an indication for inconsistencies in the pairwise comparison"));
		stConsistency->SetIndependentPos();

		PushButton* pbChangeMethod = new PushButton(root, TR("Choose other method"), (NotifyProc)&ShowPairwiseComparisonResultsForm::PushButtonCallBack);

		SetMenHelpTopic("ilwismen\\smce_window_weigh_pairwise.htm");

		create();
	}
	BOOL OnSetActive()
	{
		CalculateSaatyWeights(m_liNames, m_mapPairwiseEvaluation, m_mp, &rConsistency, m_eResult);
		int i = 0;
		for (list <String>::iterator it = m_liNames->begin(); it != m_liNames->end(); ++it)
		{
			vfrNormWeight[i]->SetVal((*m_mp)[(*it)]);
			++i;
		}
		if (rConsistency >= 0)
		{
			frConsistency->Show();
			frConsistency->SetVal(rConsistency);
			stConsistency->Show();
		}
		else
		{
			frConsistency->Hide();
			stConsistency->Hide();
		}

		return FormBaseWizardPage::OnSetActive();
	}
	int PushButtonCallBack(Event*)
	{
		*m_fChangeMethod = true;
		EndDialog(IDCANCEL);
		return 0;
	}
	int CallBackFunc(Event*)
	{
		for (unsigned int i=0; i<m_liNames->size(); ++i)
			vfrNormWeight[i]->SetReadOnly(true);

		frConsistency->SetReadOnly(true);

		return 0;
	}
	int exec()
	{
		return 0;
	}
private:
	list <String> *m_liNames;
	map <pair <String, String>, double> *m_mapPairwiseEvaluation;
	map <String, double> * m_mp;
	vector <FieldReal*> vfrNormWeight;
	ValueRange vrPrecision;
	double rConsistency;
	FieldReal* frConsistency;
	StaticText* stConsistency;
	bool *m_fChangeMethod;
	Evaluation::eResultType m_eResult;
};

PairwiseComparisonWizard::PairwiseComparisonWizard(CWnd* wPar, list <String> *liNames, map <pair <String, String>, double> *mapPairwiseEvaluation, map <String, double> * mp, bool* fChangeMethod, Evaluation::eResultType _eResult, String sLabel)
: CPropertySheet(TR("Pairwise Comparison").c_str(), wPar)
, m_fChangeMethod(fChangeMethod)
, m_eResult(_eResult)
{
	pgVectorCompareTwo.clear();
	SetWizardMode();

	// count the # of comparisons beforehand
	int iTotalNrComparisons = 0;
	for (list <String>::iterator itXa = liNames->begin(); itXa != liNames->end(); ++itXa)
		for (list <String>::iterator itYa = itXa; itYa != liNames->end(); ++itYa)
			if (itYa != itXa)
				++iTotalNrComparisons;
	vfComparisonDone.resize(iTotalNrComparisons);
	for (int i=0; i < iTotalNrComparisons; ++i)
		vfComparisonDone[i] = false;

	int iCurrentComparisonIndex = 0;
	for (list <String>::iterator itX = liNames->begin(); itX != liNames->end(); ++itX)
		for (list <String>::iterator itY = itX; itY != liNames->end(); ++itY)
			if (itY != itX)
			{
				FormBaseWizardPage *pg = new CompareTwoForm(TR("Pairwise Comparison"), liNames, mapPairwiseEvaluation, &vfComparisonDone, *itX, *itY, iCurrentComparisonIndex, iTotalNrComparisons, m_fChangeMethod);
				AddPage(pg);
				pgVectorCompareTwo.push_back(pg);
				++iCurrentComparisonIndex;
			}
			else
				(*mapPairwiseEvaluation)[pair<String, String>((*itX), (*itX))] = 1; // the matrix diagonal
	pgShowResults = new ShowPairwiseComparisonResultsForm(TR("Pairwise Comparison - Results"), liNames, mapPairwiseEvaluation, mp, m_fChangeMethod, m_eResult, sLabel);
	AddPage(pgShowResults);
}

PairwiseComparisonWizard::~PairwiseComparisonWizard()
{
	delete pgShowResults;
	for (vector<FormBaseWizardPage*>::iterator it = pgVectorCompareTwo.begin(); it != pgVectorCompareTwo.end(); ++it)
		if ((*it) != 0)
			delete (*it);
}

void PairwiseComparisonWizard::ActivatePage(int iPageNumber)
{
	SetActivePage(pgVectorCompareTwo[iPageNumber]);
}

Evaluation::eActionTP PairwiseComparison::ShowForm(CWnd* wPar, list <String> *liNames, map <String, double> * mp)
{
	bool fChangeMethod = false;
	Refresh(mp); // in case there was some resizing since last time
	map <pair <String, String>, double> mapPairwiseEvaluationCopy = mapPairwiseEvaluation;
	map <String, double> mapUserEvaluationCopy = *mp;
	
	PairwiseComparisonWizard *pcwiz = new PairwiseComparisonWizard(wPar, liNames, &mapPairwiseEvaluationCopy, &mapUserEvaluationCopy, &fChangeMethod, eResult, m_sTitle);
	int iRes = pcwiz->DoModal();
	delete pcwiz;
	
	if (ID_WIZFINISH == iRes)
	{
		mapPairwiseEvaluation = mapPairwiseEvaluationCopy;
		*mp = mapUserEvaluationCopy;
		return iOK;
	}
	return fChangeMethod?iOTHERMETHOD:iCANCEL;
}

OwnerDrawListBoxWithToolTip::OwnerDrawListBoxWithToolTip(FormEntry *f, DWORD iStyle, const CRect& rct, CWnd *parent, int id)
: OwnerDrawListBox(f, iStyle, rct, parent, id)
{
}

int OwnerDrawListBoxWithToolTip::OnToolHitTest( CPoint point, TOOLINFO* pTI ) const
{
    BOOL fOutside;
    UINT itemId = ItemFromPoint(point, fOutside);
    if ( fOutside ) 
        return -1;

    GetItemRect(itemId, &(pTI->rect));
	pTI->uFlags = 0;
    pTI->hwnd = m_hWnd;
    pTI->uId = itemId;
	CString tooltipTxt;
    GetText(itemId, tooltipTxt);

	pTI->lpszText = (LPSTR)malloc(tooltipTxt.GetLength() + 1);
	lstrcpy(pTI->lpszText, (LPCTSTR)tooltipTxt); //pTI->lpszText = LPSTR_TEXTCALLBACK; //Will trigger a TTN_NEEDTEXT message
    return itemId;
}

//////////////////////////////////////////////////////////////////////////
// Implementation of local StringArraySelector, needed for RankOrdering
//////////////////////////////////////////////////////////////////////////

StringArraySelector::StringArraySelector(FormEntry *par, map <String, int> *mRanks, bool fSource)
: BaseSelector(par)
, m_mRanks(mRanks)
, m_fSource(fSource)
, m_iDragItemIndex(-1)
, m_sasOther(0)
, m_curDrag("DragOk")
, m_curDragProhibited("DragProhibited")
, m_curNormal(Arrow)
{
	style |= LBS_HASSTRINGS;
	style &= ~(LBS_MULTIPLESEL|WS_VSCROLL);
}

void StringArraySelector::SetOther(StringArraySelector * sasOther)
{
	m_sasOther = sasOther;
}

void StringArraySelector::create()
{
	zPoint pntFld = zPoint(psn->iPosX,psn->iPosY);
	zDimension dimFld = zDimension(psn->iWidth,psn->iMinHeight);
	lb = new OwnerDrawListBoxWithToolTip(this, LBS_OWNERDRAWFIXED |  LBS_NOTIFY | style | LBS_DISABLENOSCROLL | WS_VSCROLL | WS_BORDER | WS_TABSTOP, CRect(pntFld, dimFld), frm()->wnd() , Id());
	lb->SetFont(frm()->fnt);
	CreateChildren();
  // lb->setNotify(this, (NotifyProc)&StringArraySelector::Selected, Notify(LBN_SELCHANGE));
	lb->setNotify(this,(NotifyProc)&StringArraySelector::mouseButtonDown, WM_LBUTTONDOWN);
	lb->setNotify(this,(NotifyProc)&StringArraySelector::mouseButtonUp, WM_LBUTTONUP);
	lb->setNotify(this,(NotifyProc)&StringArraySelector::mouseMove, WM_MOUSEMOVE);
	lb->EnableToolTips();

	Fill();
}
/*
int StringArraySelector::Selected(Event *)
{
	return 1;
}	
*/

int StringArraySelector::mouseButtonDown(Event * Evt)
{
  MouseLBDownEvent *ev=dynamic_cast<MouseLBDownEvent *>(Evt);
	if (0 == ev)
		return 0;
	
	BOOL fOutside;
  m_iDragItemIndex = lb->ItemFromPoint(ev->pos(), fOutside);

	if (fOutside)
		m_iDragItemIndex = -1;
	
	return 0;
}	

int StringArraySelector::mouseButtonUp(Event * Evt)
{
	if (m_iDragItemIndex == -1)
		return 0;

  MouseLBUpEvent *ev=dynamic_cast<MouseLBUpEvent *>(Evt);
	if (0 == ev)
		return 0;

	SetCursor(m_curNormal); // restore

	bool fInternalDrag = true; // assumption til proven otherwise
	int iDropItemIndex = -1;
	bool fOutside = true; // pessimistic .. til proven otherwise
	bool fBelow = false; // indication if user dragged after the end of the list

	CRect rectThis;
	lb->GetWindowRect(rectThis);
	CPoint pt = ev->pos();
	pt.Offset(rectThis.left, 0);
	if (pt.x >= rectThis.left && pt.x <= rectThis.right)
	{
		fOutside = false;
		if (lb->GetCount() > 0)
		{
			BOOL fVerticallyOutside;
			iDropItemIndex = lb->ItemFromPoint(ev->pos(), fVerticallyOutside);
			if (fVerticallyOutside && (pt.y > 0))
				fBelow = true;
		}
	}
	else
	{
		fInternalDrag = false;
		// one more attempt in other lb
		CRect rectOther;
		m_sasOther->lb->GetWindowRect(rectOther);
		if (pt.x >= rectOther.left && pt.x <= rectOther.right)
		{
			fOutside = false;
			if (m_sasOther->lb->GetCount() > 0)
			{
				BOOL fVerticallyOutside;
				pt.Offset(-rectOther.left, 0);
				iDropItemIndex = m_sasOther->lb->ItemFromPoint(pt, fVerticallyOutside);
				if (fVerticallyOutside && (pt.y > 0))
					fBelow = true;
			}
		}
	}

	if (fOutside || (!fBelow && fInternalDrag && (iDropItemIndex == m_iDragItemIndex)))
	{
		m_iDragItemIndex = -1;
		return 0;
	}

	// If we arrived here, the user actually dragged .. handle it!

	if (fInternalDrag) // internal drag
	{
		MoveItem(m_iDragItemIndex, iDropItemIndex, fBelow);
	}
	else // external drag (from left to right CListCtrl and vice versa)
	{
		// we're the receiver of the drag operation
		if (!m_fSource) // right to left
			UnrankItem(m_iDragItemIndex);
		else // left to right
			RankItem(m_iDragItemIndex, iDropItemIndex, fBelow);
	}

	m_iDragItemIndex = -1; // otherwise we're stuck on drag

	return 0;
}

int StringArraySelector::mouseMove(Event * Evt)
{
	if (m_iDragItemIndex == -1) // (hopefully) this is the same test as "LButtonDown?"
		return 0;

	// code below copied from mouseButtonUp code, so that the cursor indication that a drag is possible
	// corresponds to the action on mouse button release

  MouseMoveEvent *ev=dynamic_cast<MouseMoveEvent *>(Evt);
	if (0 == ev)
		return 0;

	// If we arrived here, the user is actually trying to drag

	bool fInternalDrag = true; // assumption til proven otherwise
	int iDropItemIndex = -1;
	bool fOutside = true; // pessimistic .. til proven otherwise
	bool fBelow = false; // indication if user dragged after the end of the list

	CRect rectThis;
	lb->GetWindowRect(rectThis);
	CPoint pt ((signed short)LOWORD(ev->lParm), (signed short)HIWORD(ev->lParm)); // function pos() is not available in MouseMoveEvent!!
	CPoint pos(pt); // copy ..
	pt.Offset(rectThis.left, 0);
	if (pt.x >= rectThis.left && pt.x <= rectThis.right)
	{
		fOutside = false;
		if (lb->GetCount() > 0)
		{
			BOOL fVerticallyOutside;
			iDropItemIndex = lb->ItemFromPoint(pos, fVerticallyOutside);
			if (fVerticallyOutside && (pt.y > 0))
				fBelow = true;
		}
	}
	else
	{
		fInternalDrag = false;
		// one more attempt in other lb
		CRect rectOther;
		m_sasOther->lb->GetWindowRect(rectOther);
		if (pt.x >= rectOther.left && pt.x <= rectOther.right)
		{
			fOutside = false;
			if (m_sasOther->lb->GetCount() > 0)
			{
				BOOL fVerticallyOutside;
				pt.Offset(-rectOther.left, 0);
				iDropItemIndex = m_sasOther->lb->ItemFromPoint(pt, fVerticallyOutside);
				if (fVerticallyOutside && (pt.y > 0))
					fBelow = true;
			}
		}
	}

	if (fOutside || (!fBelow && fInternalDrag && (iDropItemIndex == m_iDragItemIndex)))
		SetCursor(m_curDragProhibited);
	else
		SetCursor(m_curDrag);

	return 0;
}

bool StringArraySelector::fSetSamePossible()
{
	// call only in LEFT!!
	int iLeftIndex = lb->GetCurSel();
	int iRightIndex = m_sasOther->lb->GetCurSel();
	return ((iLeftIndex != -1) && ((iRightIndex != -1) || (m_sasOther->lb->GetCount() == 0)));
}

bool StringArraySelector::fSetLowerPossible()
{
	// call only in LEFT!!
	int iLeftIndex = lb->GetCurSel();
	int iRightIndex = m_sasOther->lb->GetCount() - 1;
	return ((iLeftIndex != -1) && ((iRightIndex != -1) || (m_sasOther->lb->GetCount() == 0)));
}

bool StringArraySelector::fSetBackPossible()
{
	// call only in RIGHT!!
	return (lb->GetCurSel() != -1);
}

void StringArraySelector::SetSame()
{
	int iLeftIndex = lb->GetCurSel();
	int iRightIndex = m_sasOther->lb->GetCurSel();
	if ((iLeftIndex != -1) && ((iRightIndex != -1) || (m_sasOther->lb->GetCount() == 0)))
		RankItem(iLeftIndex, iRightIndex, false);
}

void StringArraySelector::SetLower()
{
	int iLeftIndex = lb->GetCurSel();
	int iRightIndex = m_sasOther->lb->GetCount() - 1;
	if ((iLeftIndex != -1) && ((iRightIndex != -1) || (m_sasOther->lb->GetCount() == 0)))
		RankItem(iLeftIndex, iRightIndex, true);
}

void StringArraySelector::SetBack()
{
	int iRightIndex = lb->GetCurSel();
	if (iRightIndex != -1)
		UnrankItem(iRightIndex);
}

void StringArraySelector::MoveItem(int iOldIndex, int iNewIndex, bool fBelowBottomItem)
{
	if (!m_fSource) // only allowed in right-side CListCtrl
	{
		String sLabel = sGetString(iOldIndex);
		int iDeletedRank = (*m_mRanks)[sLabel];
		(*m_mRanks)[sLabel] = (fBelowBottomItem?1:0) + (*m_mRanks)[sGetString(iNewIndex)]; // the new rank
		if (fReRankIfNeeded(iDeletedRank))
			lb->Invalidate();
		DeleteString(iOldIndex);
		bool fAddOne = (iOldIndex > iNewIndex) || fBelowBottomItem; // to "correct" the position of the item
		InsertString((fAddOne?1:0) + iNewIndex, sLabel);
		lb->SelectString(-1, sLabel.c_str()); // the moved item .. we have to do it like this coz the formula for the index depends on the behavior of the CListBox
		DoCallBack();
	}
}

void StringArraySelector::RankItem(int iLeftIndex, int iRightIndex, bool fBelowBottomItem)
{
	// confirm orientation .. make sure we're called in LEFT control
	if (!m_fSource)
		m_sasOther->RankItem(iLeftIndex, iRightIndex, fBelowBottomItem);
	else
	{
		// now we're sure we're called in LEFT control
		String sLabel = sGetString(iLeftIndex);
		if (m_sasOther->lb->GetCount() > 0)
			(*m_mRanks)[sLabel] = (fBelowBottomItem?1:0) + (*m_mRanks)[m_sasOther->sGetString(iRightIndex)]; // the new rank
		else // we're the first item: rank = 1
			(*m_mRanks)[sLabel] = 1;
		DeleteString(iLeftIndex);
		m_sasOther->InsertString(1 + iRightIndex, sLabel);
		lb->SetCurSel(-1); // unselect any item .. it is more logical
		m_sasOther->lb->SetCurSel(1 + iRightIndex); // the newly added item
		DoCallBack();
	}
}

void StringArraySelector::UnrankItem(int iRightIndex)
{
	// confirm orientation .. make sure we're called in RIGHT control
	if (m_fSource)
		m_sasOther->UnrankItem(iRightIndex);
	else
	{
		// now we're sure we're called in RIGHT control
		String sLabel = sGetString(iRightIndex);
		int iDeletedRank = (*m_mRanks)[sLabel];
		(*m_mRanks)[sLabel] = iUNDEF; // not ranked
		if (fReRankIfNeeded(iDeletedRank))
			lb->Invalidate();
		DeleteString(iRightIndex);
		m_sasOther->AddString(sLabel);
		lb->SetCurSel(-1); // unselect any item .. it is more logical
		m_sasOther->lb->SetCurSel(m_sasOther->lb->GetCount() - 1); // the index of the item just added
		DoCallBack();
	}
}

bool StringArraySelector::fReRankIfNeeded(int iDeletedRank)
{
	bool fReRankNeeded = true; // prove otherwise with following 'for' loop
	for (map <String, int>::iterator it = m_mRanks->begin(); it != m_mRanks->end(); ++it)
	{
		if ((*it).second == iDeletedRank) // there is still an item using it
			fReRankNeeded = false;
	}

	if (fReRankNeeded)
	{
		// "shift" the ranks one position
		for (map <String, int>::iterator it = m_mRanks->begin(); it != m_mRanks->end(); ++it)
		{
			if ((*it).second > iDeletedRank)
				(*it).second = (*it).second - 1;
		}
	}

	return fReRankNeeded;
}

bool fLessThan(pair <String, int> &p1, pair <String, int> &p2)
{
	return ((p1.second < p2.second) || ((p1.second == p2.second) && (p1.first < p2.first)));
}

void StringArraySelector::Fill()
{
	lb->ResetContent();

	if (m_fSource)
	{
		for (map<String, int>::iterator it = m_mRanks->begin(); it != m_mRanks->end(); ++it)
			if ((*it).second == iUNDEF)
				AddString((*it).first);
	}
	else
	{
		typedef pair <String, int> RankTp;
		vector <RankTp> vRanks;
		// copy the map into a vector so that it can be sorted
		for (map<String, int>::iterator it = m_mRanks->begin(); it != m_mRanks->end(); ++it)
			if ((*it).second != iUNDEF)
				vRanks.push_back(*it);
		// then sort it
		sort(vRanks.begin(), vRanks.end(), fLessThan);
		// then add all vector elements in the listctrl in order
		for (unsigned int i=0; i<vRanks.size(); ++i)
			AddString(vRanks[i].first);
	}

	lb->SetCurSel(0);
}

String StringArraySelector::sName(int id)
{
  if (lb) 
  {
    CString s;
    lb->GetText(id, s);
		String sLabel (s);
		if (m_fSource)
			return sLabel;
		else // compose the string
			return String ("%d %S", (*m_mRanks)[sLabel], sLabel);
  }
  else
    return "";  
}

String StringArraySelector::sGetString(int iIndex)
{
  if (lb)
  {
    CString s;
    lb->GetText(iIndex, s);
    return String(s);
	}
	else
		return "";
}

void StringArraySelector::AddString(const String &s)
{
	lb->AddString(s.c_str());
}

void StringArraySelector::DeleteString(int iIndex)
{
	lb->DeleteString(iIndex);
}

void StringArraySelector::InsertString(int iIndex, const String &s)
{
	lb->InsertString(min(iIndex, lb->GetCount()), s.c_str());
}

CWnd* StringArraySelector::pWnd()
{
	return lb;
}

void StringArraySelector::StoreData()
{
}

void StringArraySelector::DrawItem(Event* ev)
{
  DrawItemEvent *dev=dynamic_cast<DrawItemEvent *>(ev);
  ISTRUE(fINotEqual, dev, (DrawItemEvent *)NULL);

  DRAWITEMSTRUCT *dis=reinterpret_cast<DRAWITEMSTRUCT *> (dev->lParm);

  int id = dis->itemID;
	if (id < 0)
		return;

	String s = sName(id);
	CDC dc;
	dc.Attach(dis->hDC);

	bool fSel = ((dis->itemAction | ODA_SELECT) && (dis->itemState & ODS_SELECTED));

  idw.DrawItem(dis, 2, s, fSel, true, false);

	dc.Detach();
}

//////////////////////////////////////////////////////////////////////
// RankOrdering
//////////////////////////////////////////////////////////////////////

RankOrdering::RankOrdering(eResultType _eResult, const String& sTitle)
: Evaluation(_eResult, sTitle)
, eMethod(iEXPECTEDVALUE)
, rRankSumPower(1.0)
{

}

RankOrdering::~RankOrdering()
{

}

String RankOrdering::sDescription()
{
	switch (eMethod)
	{
	case iEXPECTEDVALUE:
		return TR("ExpVal");
		break;
	case iRANKSUM:
		return TR("RankSum");
		break;
	default:
		return "";
	}
}

void RankOrdering::Refresh(map <String, double> *mp)
{
	map <String, int> mpTemp;
	for (map<String, double>::iterator it = mp->begin(); it != mp->end(); ++it)
	{
		if (mapRankOrdering.find((*it).first) != mapRankOrdering.end())
			mpTemp[(*it).first] = mapRankOrdering[(*it).first];
		else
			mpTemp[(*it).first] = iUNDEF; // if we don't do this, 0's will enter in the map silently for all not-yet-ranked elements
	}
	// this will cleanup
	mapRankOrdering = mpTemp;
	// now still cover the case of deleted items: "compact ranks"
	int iCheckedRank = 0;
	bool fDone = false;
	while (!fDone)
	{
		// iCheckedRank is the highest rank we already checked and confirmed compactness
		// Find the smallest rank that we didn't check yet. This will become iNextRank
		int iNextRank;
		fDone = true; // loop success will prove otherwise
		for (map<String, int>::iterator it = mapRankOrdering.begin(); it != mapRankOrdering.end(); ++it)
			if ((*it).second > iCheckedRank)
			{
				if (fDone) // first time !!
				{
					iNextRank = (*it).second;
					fDone = false;
				}
				else
					iNextRank = min((*it).second, iNextRank);
			}
		// now iNextRank points to the next rank after iCheckedRank
		// If there is a gap between that rank and the previously checked rank, move it down
		if (!fDone && (iNextRank != (iCheckedRank + 1)))
		{
			int iDiff = iNextRank - iCheckedRank - 1; // the amount to move down
			for (map<String, int>::iterator it = mapRankOrdering.begin(); it != mapRankOrdering.end(); ++it)
				if ((*it).second == iNextRank)
					(*it).second -= iDiff;
		}
		++iCheckedRank;
	}
}

Evaluation* RankOrdering::create(CWnd* wPar, list <String> *liNames, map <String, double> *mp, Evaluation* ePrevious, eResultType _eResult, const String& sTitle)
{
	RankOrdering * roNew = new RankOrdering(_eResult, sTitle);
	eActionTP action = roNew->ShowForm(wPar, liNames, mp);
	if (action == iOK)
		return roNew;
	else if (action == iCANCEL)
	{
		delete roNew; // user doesn't want it
		return ePrevious;
	}
	else
	{
		delete roNew; // user doesn't want it
		return Evaluation::create(wPar, liNames, mp, ePrevious, _eResult, sTitle); // try all over again!
	}
}

void RankOrdering::WriteElements(const char* sSection, const ElementContainer& en)
{
	ObjectInfo::WriteElement(sSection, "StdType", en, "RankOrder");
	int iSize = mapRankOrdering.size();
	ObjectInfo::WriteElement(sSection, "NrElements", en, iSize);
	int i=0;
	for (map <String, int>::iterator it = mapRankOrdering.begin(); it != mapRankOrdering.end(); ++it)
	{
		ObjectInfo::WriteElement(sSection, String("Element%d", i).c_str(), en, (*it).first);
		ObjectInfo::WriteElement(sSection, String("Value%d", i).c_str(), en, (*it).second);
		++i;
	}
	ObjectInfo::WriteElement(sSection, "Method", en, sMethodTypeToString(eMethod));
	ObjectInfo::WriteElement(sSection, "RankSumPower", en, rRankSumPower);
}

void RankOrdering::ReadElements(const char* sSection, const ElementContainer& en)
{
	// type already known .. we're "new"-ed
	int iSize;
	if (!ObjectInfo::ReadElement(sSection, "NrElements", en, iSize))
		iSize = 0;
	mapRankOrdering.clear();
	for (int i=0; i<iSize; ++i)
	{
		String str;
		int d;
		ObjectInfo::ReadElement(sSection, String("Element%d", i).c_str(), en, str);
		ObjectInfo::ReadElement(sSection, String("Value%d", i).c_str(), en, d);
		mapRankOrdering[str]=d;
	}
	String sType;
	ObjectInfo::ReadElement(sSection, "Method", en, sType);
	eMethod = eMethodTypeFromString(sType);
	double rRankSumPowerBackup = rRankSumPower;
	if (0 == ObjectInfo::ReadElement(sSection, "RankSumPower", en, rRankSumPower))
		rRankSumPower = rRankSumPowerBackup;
}

void RankOrdering::Rename(String oldName, String newName)
{
	if (oldName != newName)
	{
		int iBackup = mapRankOrdering[oldName];
		mapRankOrdering.erase(oldName);
		mapRankOrdering[newName] = iBackup;
	}
}

void CalculateRankOrderWeights(map <String, int> *mpRankOrdering, map <String, double> *mp, RankOrdering::eMethodTP eMethod, double rRankSumPower, Evaluation::eResultType _eResult)
{
	// the "formula's" generate a list of weights (summing to 1)
	// the list's length is the map's length
	// then each label gets the "average weight" for its rank
	vector <double> vWeights;
	int iTotal = 0;
	for (map <String, int>::iterator itMap = mpRankOrdering->begin(); itMap != mpRankOrdering->end(); ++itMap)
		if ((*itMap).second != iUNDEF)
			++iTotal;
	vWeights.resize(iTotal);

	// Now the selected ranking method comes into play

	if (iTotal >= 1)
	{
		if (eMethod == RankOrdering::iEXPECTEDVALUE)
		{
			vWeights[iTotal - 1] = 1.0/(iTotal*iTotal);
			for (int i = iTotal - 1; i > 0; --i)
				vWeights[i-1] = vWeights[i] + 1.0/(iTotal*i);
		}
		else if (eMethod == RankOrdering::iRANKSUM)
		{
			double rDenom = 0;
			for (int i = 1; i<=iTotal; ++i)
				rDenom += pow((iTotal + 1 - i), rRankSumPower);
			for (int k = 1; k <= iTotal; ++k)
				vWeights[k-1] = pow((iTotal + 1 - k), rRankSumPower) / rDenom;
		}
	}

	// Selected method is applied; result is in vWeights

	// Now vWeights has to be distributed over the ranked items.

	vector <double> vWeightsByRank;
	vWeightsByRank.resize(iTotal); // this is too big when two or more items have the same rank, but it doesn't matter
	int iPos = 0; // index to vWeights .. to "consume" the weights in the next loop
	for (int i=1; i<= iTotal; ++i) // loop the ranks
	{
		// how many of "i" ?
		// STL is so "beautifully" documented that I do not understand how to work with the count functions .. so I just loop the entire map!
		int iCount = 0;
		for (map <String, int>::iterator it = mpRankOrdering->begin(); it != mpRankOrdering->end(); ++it)
			if ((*it).second == i)
				++iCount;
		// rank "i" occurred iCount times.
		vWeightsByRank[i-1] = 0;
		if (iCount > 0)
		{
			for (int j=0; j < iCount; ++j)
				vWeightsByRank[i-1] += vWeights[iPos++];
			vWeightsByRank[i-1] /= iCount; // "average"!
		}
	}
	for (map <String, double>::iterator it = mp->begin(); it != mp->end(); ++it)
	{
		if ((*mpRankOrdering)[(*it).first] != iUNDEF)
			(*it).second = vWeightsByRank[(*mpRankOrdering)[(*it).first] - 1];
		else
			(*it).second = 0;
	}

	if (Evaluation::iMAXISONE == _eResult)
	{
		// a small "boost" so that the maximum is one
		double rMax = 0;
		for (map <String, double>::iterator it = mp->begin(); it != mp->end(); ++it)
			rMax = max(rMax, (*it).second);
		if (rMax > 0)
		{
			for (map <String, double>::iterator it = mp->begin(); it != mp->end(); ++it)
				(*it).second = (*it).second / rMax;
		}
		else
		{
			// max = 0 .. make sure it becomes 1 !!
			map <String, double>::iterator it = mp->begin();
			if (it != mp->end())
				(*it).second = 1;
		}
	}
	else // sum is one
	{
		double rSum = 0;
		for (map <String, double>::iterator it = mp->begin(); it != mp->end(); ++it)
			rSum += (*it).second;
		if (rSum == 0)
		{
			// sum = 0 .. make sure it becomes 1 !!
			map <String, double>::iterator it = mp->begin();
			if (it != mp->end())
				(*it).second = 1;
		}
	}
}

void RankOrdering::Recalculate(list <String> *liNames, map <String, double> *mp)
{
	Refresh(mp); // in case there was some resizing since last time
	CalculateRankOrderWeights(&mapRankOrdering, mp, eMethod, rRankSumPower, eResult);
}

class RankOrderingForm: public FormWithDest
{
public:
  RankOrderingForm(CWnd* wPar, list <String> *liNames, map <String, int> *mapEvaluation, map <String, double> * mp, RankOrdering::eMethodTP * eMethod, bool* fChangeMethod, double* rRankSumPower, Evaluation::eResultType _eResult, String sLabel)
    : FormWithDest(wPar, TR("Rank Ordering"))
		, m_mapEvaluation(mapEvaluation)
		, m_mp(mp)
		, m_fChangeMethod(fChangeMethod)
		, m_liNames(liNames)
		, m_eResult(_eResult)
		, iSize(mapEvaluation->size())
		, vrPrecision(ValueRange(0, LONG_MAX, 0.001))
  {
		vfrWeight.resize(iSize);

		rgMethod = new RadioGroup(root, TR("Ranking Method"), (int*)eMethod);
		new RadioButton(rgMethod, TR("Expected Value"));
		RadioButton* rbRankSum = new RadioButton(rgMethod, TR("Rank Sum"));
		frPower = new FieldReal(rbRankSum, TR("Rank Sum Power"), rRankSumPower);
		frPower->SetCallBack((NotifyProc)&RankOrderingForm::CallBackFunc);
		rgMethod->SetCallBack((NotifyProc)&RankOrderingForm::CallBackFunc);
		rgMethod->SetIndependentPos();

		sasLeft = new StringArraySelector(root, m_mapEvaluation, true);
		sasLeft->Align(rgMethod, AL_UNDER);
		sasLeft->SetWidth(80);
		FieldGroup* fgMiddle = new FieldGroup(root);
		sasRight = new StringArraySelector(root, m_mapEvaluation, false);
		fgMiddle->Align(sasLeft, AL_AFTER);
		sasRight->Align(fgMiddle, AL_AFTER);
		sasRight->SetWidth(80);
		sasLeft->SetOther(sasRight);
		sasRight->SetOther(sasLeft);
		if (iSize > 9)
		{
			int iNewHeight = iSize * sasLeft->psn->iHeight / 9;
			sasLeft->SetHeight(iNewHeight);
			sasRight->SetHeight(iNewHeight);
		}

		pbSame = new OwnButtonSimple(fgMiddle, "SMCERankSame", (NotifyProc)&RankOrderingForm::CallBackSame, false, true);
		pbLower = new OwnButtonSimple(fgMiddle, "SMCERankLower", (NotifyProc)&RankOrderingForm::CallBackLower, false, true);
		pbBack = new OwnButtonSimple(fgMiddle, "SMCERankBack", (NotifyProc)&RankOrderingForm::CallBackBack, false, true);

		sasLeft->SetCallBack((NotifyProc)&RankOrderingForm::CallBackFunc);
		sasRight->SetCallBack((NotifyProc)&RankOrderingForm::CallBackFunc);

		StaticText* stWeightsTitle = new StaticText(root, String(TR("Resulting %S").c_str(), sLabel));
		stWeightsTitle->Align(sasRight, AL_AFTER);
		stWeightsTitle->SetIndependentPos();

		FieldGroup * fgWeights = new FieldGroup(root);
		fgWeights->Align(stWeightsTitle, AL_UNDER);

		int i = 0;
		for (list <String>::iterator it = liNames->begin(); it != liNames->end(); ++it)
		{
			vfrWeight[i] = new FieldReal(fgWeights, (*it), &((*m_mp)[(*it)]), vrPrecision);
			if (i>0)
				vfrWeight[i]->Align(vfrWeight[i-1], AL_UNDER);
			++i;
		}

		PushButton* pbChangeMethod = new PushButton(root, TR("Choose other method"), (NotifyProc)&RankOrderingForm::PushButtonCallBack);
		pbChangeMethod->Align(sasLeft, AL_UNDER);
		pbChangeMethod->SetIndependentPos();

		SetMenHelpTopic("ilwismen\\smce_window_weigh_rank_order.htm");

		create();
	}
	int PushButtonCallBack(Event*)
	{
		*m_fChangeMethod = true;
		EndDialog(IDCANCEL);
		return 0;
	}
	int CallBackFunc(Event*)
	{
		CalculateRankOrderWeights(m_mapEvaluation, m_mp, (RankOrdering::eMethodTP)rgMethod->iVal(), frPower->rVal(), m_eResult);

		int i = 0;
		double rSum = 0;
		for (list <String>::iterator it = m_liNames->begin(); it != m_liNames->end(); ++it)
		{
			// assuming that the order didn't change
			vfrWeight[i]->SetVal((*m_mp)[(*it)]);
			vfrWeight[i]->SetReadOnly(true);
			rSum += (*m_mp)[(*it)];
			++i;
		}

		if (rSum != 0)
			EnableOK();
		else
			DisableOK();
		
		// disable/enable the buttons - FormEntry::Enable()/Disable() are not implemented, so ...
		CWnd* pSame = GetDlgItem(pbSame->Id());
		if (pSame)
			pSame->EnableWindow(sasLeft->fSetSamePossible());
		CWnd* pLower = GetDlgItem(pbLower->Id());
		if (pLower)
			pLower->EnableWindow(sasLeft->fSetLowerPossible());
		CWnd* pBack = GetDlgItem(pbBack->Id());
		if (pBack)
			pBack->EnableWindow(sasRight->fSetBackPossible());

		return 0;
	}
	int CallBackSame(Event*)
	{
		sasLeft->SetSame();
		return 0;
	}
	int CallBackLower(Event*)
	{
		sasLeft->SetLower();
		return 0;
	}
	int CallBackBack(Event*)
	{
		sasRight->SetBack();
		return 0;
	}
private:
	map <String, int> *m_mapEvaluation;
	map <String, double> *m_mp;
	list <String> *m_liNames;
	RadioGroup* rgMethod;
	FieldReal* frPower;
	StringArraySelector *sasLeft, *sasRight;
	OwnButtonSimple *pbSame, *pbLower, *pbBack;
	vector <FieldReal*> vfrWeight;
	ValueRange vrPrecision;
	int iSize;
	bool *m_fChangeMethod;
	Evaluation::eResultType m_eResult;
};

Evaluation::eActionTP RankOrdering::ShowForm(CWnd* wPar, list <String> *liNames, map <String, double> *mp)
{
	bool fChangeMethod = false;
	Refresh(mp); // in case there was some resizing since last time
	map <String, int> mapRankOrderingCopy = mapRankOrdering;
	map <String, double> mapUserEvaluationCopy = *mp;

	RankOrderingForm frm (wPar, liNames, &mapRankOrderingCopy, &mapUserEvaluationCopy, &eMethod, &fChangeMethod, &rRankSumPower, eResult, m_sTitle);
	if (frm.fOkClicked())
	{
		CalculateRankOrderWeights(&mapRankOrderingCopy, &mapUserEvaluationCopy, eMethod, rRankSumPower, eResult); // test
		mapRankOrdering = mapRankOrderingCopy;
		*mp = mapUserEvaluationCopy;
		return iOK;
	}
	return fChangeMethod?iOTHERMETHOD:iCANCEL;
}


