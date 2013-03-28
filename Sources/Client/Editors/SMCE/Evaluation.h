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
// Evaluation.h: interface for the Evaluation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WEIGHTS_H__F0DBB941_2489_4AFE_96C8_6A102F9761FF__INCLUDED_)
#define AFX_WEIGHTS_H__F0DBB941_2489_4AFE_96C8_6A102F9761FF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <map>
#include <list>
using namespace std;
#include "Client\FormElements\selector.h"

class Evaluation  
{
public:
	enum eActionTP{iOK, iCANCEL, iOTHERMETHOD};
	enum eResultType{iSUMTOONE, iMAXISONE};

	Evaluation(eResultType _eResult, const String& sTitle);
	virtual ~Evaluation();
	virtual void Rename(String oldName, String newName) = 0; // handle renaming
	virtual eActionTP ShowForm(CWnd* wPar, list <String> *liNames, map <String, double> *mp) = 0;
	virtual void WriteElements(const char* sSection, const ElementContainer& en) = 0;
	virtual void ReadElements(const char* sSection, const ElementContainer& en) = 0;
	virtual void Recalculate(list <String> *liNames, map <String, double> *mp) = 0;
	virtual String sDescription() = 0;

	static Evaluation* create(CWnd* wPar, list <String> *liNames, map <String, double> *mp, Evaluation* ePrevious, eResultType _eResult, const String& sTitle);
	static Evaluation* CreateFromElementContainer(const char* sSection, const ElementContainer& en, eResultType _eResult, const String& sTitle);

protected:
	eResultType eResult;
	String m_sTitle;
};

class DirectEvaluation : public Evaluation
{
public:
	DirectEvaluation(eResultType _eResult, const String& sTitle);
	virtual ~DirectEvaluation();
	void Rename(String oldName, String newName);
	eActionTP ShowForm(CWnd* wPar, list <String> *liNames, map <String, double> *mp);
	static Evaluation* create(CWnd* wPar, list <String> *liNames, map <String, double> *mp, Evaluation* ePrevious, eResultType _eResult, const String& sTitle);
	void WriteElements(const char* sSection, const ElementContainer& en);
	void ReadElements(const char* sSection, const ElementContainer& en);
	void Recalculate(list <String> *liNames, map <String, double> *mp);
	String sDescription();

private:
	void Refresh(map <String, double> *mp); // makes sure the map is refreshed
	map <String, double> mapDirectEvaluation; // these are internal - not normalized
};

class PairwiseComparison : public Evaluation
{
public:
	PairwiseComparison(eResultType _eResult, const String& sTitle);
	virtual ~PairwiseComparison();
	void Rename(String oldName, String newName);
	eActionTP ShowForm(CWnd* wPar, list <String> *liNames, map <String, double> *mp);
	static Evaluation* create(CWnd* wPar, list <String> *liNames, map <String, double> *mp, Evaluation* ePrevious, eResultType _eResult, const String& sTitle);
	void WriteElements(const char* sSection, const ElementContainer& en);
	void ReadElements(const char* sSection, const ElementContainer& en);
	void Recalculate(list <String> *liNames, map <String, double> *mp);
	String sDescription();

private:
	void Refresh(map <String, double> *mp); // makes sure the map is refreshed
	map <pair <String, String>, double> mapPairwiseEvaluation; // store entire matrix
};

class FormBaseWizardPage; // forward declaration
class PairwiseComparisonWizard : public CPropertySheet
{
public:
  PairwiseComparisonWizard(CWnd* wPar, list <String> *liNames, map <pair <String, String>, double> *mapPairwiseEvaluation, map <String, double> * mp, bool* fChangeMethod, Evaluation::eResultType _eResult, String sLabel);
	virtual ~PairwiseComparisonWizard();
	void ActivatePage(int iPageNumber);
private:
	vector <FormBaseWizardPage*> pgVectorCompareTwo;
	vector <bool> vfComparisonDone;
	FormBaseWizardPage *pgShowResults;
	bool *m_fChangeMethod;
	Evaluation::eResultType m_eResult;
};

class RankOrdering : public Evaluation
{
public:

	enum eMethodTP {iEXPECTEDVALUE, iRANKSUM};

	RankOrdering(eResultType _eResult, const String& sTitle);
	virtual ~RankOrdering();
	void Rename(String oldName, String newName);
	eActionTP ShowForm(CWnd* wPar, list <String> *liNames, map <String, double> *mp);
	static Evaluation* create(CWnd* wPar, list <String> *liNames, map <String, double> *mp, Evaluation* ePrevious, eResultType _eResult, const String& sTitle);
	void WriteElements(const char* sSection, const ElementContainer& en);
	void ReadElements(const char* sSection, const ElementContainer& en);
	void Recalculate(list <String> *liNames, map <String, double> *mp);
	String sDescription();

private:
	eMethodTP eMethod;

	static String sMethodTypeToString(eMethodTP e)
	{
		switch(e)
		{
		case iEXPECTEDVALUE:
			return "ExpectedValue";
			break;
		case iRANKSUM:
			return "RankSum";
			break;
		default:
			return "";
		}
	};

	static eMethodTP eMethodTypeFromString(String s)
	{
		if ("ExpectedValue" == s)
			return iEXPECTEDVALUE;
		else if ("RankSum" == s)
			return iRANKSUM;
		else
			return iEXPECTEDVALUE; // wrong!!
	};

	void Refresh(map <String, double> *mp); // makes sure the map is refreshed
	map <String, int> mapRankOrdering; // internal
	double rRankSumPower;
};

class OwnerDrawListBoxWithToolTip : public OwnerDrawListBox
{
public:
	OwnerDrawListBoxWithToolTip(FormEntry *f, DWORD iStyle, const CRect& rct, CWnd *parent, int id);
	virtual int OnToolHitTest( CPoint point, TOOLINFO* pTI ) const;
};

class StringArraySelector : public BaseSelector
{
public:
	StringArraySelector(FormEntry *par, map <String, int> *mRanks, bool fSource);
	String sGetString(int iIndex);
	void Fill();
	void AddString(const String &s);
	void DeleteString(int iIndex);
	void InsertString(int iIndex, const String &s);
	CWnd* pWnd();
	void SetOther(StringArraySelector * sasOther);
	void SetSame();
	void SetLower();
	void SetBack();
	bool fSetSamePossible();
	bool fSetLowerPossible();
	bool fSetBackPossible();
	virtual void StoreData();

protected:
	void create();
	// int Selected(Event *);
	int mouseButtonDown(Event *);
	int mouseButtonUp(Event *);
	int mouseMove(Event *);
	virtual void DrawItem(Event* ev);
private:
	void MoveItem(int iOldIndex, int iNewIndex, bool fBelowBottomItem);
	void RankItem(int iLeftIndex, int iRightIndex, bool fBelowBottomItem);
	void UnrankItem(int iRightIndex);
	bool fReRankIfNeeded(int iDeletedRank);
	virtual String sName(int id);
	int m_iDragItemIndex;
	map <String, int> *m_mRanks;
	bool m_fSource;
	StringArraySelector * m_sasOther;
	zCursor m_curDrag, m_curDragProhibited, m_curNormal;
};

#endif // !defined(AFX_WEIGHTS_H__F0DBB941_2489_4AFE_96C8_6A102F9761FF__INCLUDED_)
