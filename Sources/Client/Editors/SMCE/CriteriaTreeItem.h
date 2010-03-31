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
// CriteriaTreeItem.h: interface for the CriteriaTreeItem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CRITERIATREEITEM_H__B7218813_FF2F_421C_B291_8BE958BC6701__INCLUDED_)
#define AFX_CRITERIATREEITEM_H__B7218813_FF2F_421C_B291_8BE958BC6701__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Headers\toolspch.h"

#include <afxwin.h>

#include <list>
#include <vector>
using namespace std;

class CriteriaTreeDoc;
class Evaluation;
class Standardization;
// class FileName;
class InPlaceEdit;
class NameEdit;
class FormWithDest;
class AttributeFileName;
struct AggregateValueOperationStruct;

class CriteriaTreeItem : public CCmdTarget
{
protected:
	enum eIconIndexTP{iGOAL, iGROUP, iMAPEFFECT, iMAPCONSTRAINT, iAGGREGATEDMAPEFFECT, iAGGREGATEDMAPCONSTRAINT, iSCOREEFFECT, iSCORECONSTRAINT}; // should correspond with icons loaded in view
public:
	enum eCtiTypeTP {iFACTOR, iCONSTRAINT};

	static String sCtiTypeToString(eCtiTypeTP e)
	{
		switch(e)
		{
		case iFACTOR:
			return "Factor";
			break;
		case iCONSTRAINT:
			return "Constraint";
			break;
		default:
			return "";
		}
	};

	static eCtiTypeTP eCtiTypeFromString(String s)
	{
		if ("Factor" == s)
			return iFACTOR;
		else if ("Constraint" == s)
			return iCONSTRAINT;
		else
			return iFACTOR; // wrong!!
	};

	CriteriaTreeItem(CriteriaTreeItem* pParent, CString sName, eCtiTypeTP eCT);
	CriteriaTreeItem(CriteriaTreeDoc* ctdDoc, CString sName, eCtiTypeTP eCT);
	CriteriaTreeItem(CriteriaTreeItem* ctiCopy);
	virtual ~CriteriaTreeItem();
	double rGetWeight();
	bool fHasWeight();
	bool fIsConstraint();
	CriteriaTreeItem* ctiPreviousItem(); // the previous item in the list
	virtual void SetNrAlternatives(int iNr) = 0;
	virtual bool fCalculationPossible(int iCol, bool fFirst) = 0;
	virtual bool fDone(int iCol, bool fRecursive = false) = 0; // indicates if the current element is ok
	virtual bool fEditAllowed(int iCol) = 0;
	int iGetNrAlternatives();
	String sName(); // the plain name
	void SetName(CString sName);
	virtual CString sDisplayText(int iCol) = 0; // iCol == 0 -> same as sDisplayText(), otherwise map/score text
	virtual int iIconIndex() = 0; // here we can play with the item's type and even indicate problems
	void EditTreeElement();
	virtual void DeleteTreeElement();
	CriteriaTreeDoc* GetDocument();
	CriteriaTreeItem* GetParent();
	void SetParent(CriteriaTreeItem* ctiNewParent);
	virtual AttributeFileName afnFindFirstInputMap();
	virtual void GetObjectStructure(ObjectStructure& os);

	virtual void Edit(int iCol, RECT rect, CWnd* wnd) = 0;
	// to change the name, standardization, aggregation etc
	// or the input data / output map names
	// rect is the coordinates of the item we're editing, in wnd coordinates
	// wnd is the window that we can use to position something if needed

	virtual void Delete(int iCol) = 0;
	// similar to Edit .. pass the command to the appropriate level

	virtual void SetData(int iCol, String sData) = 0;
	// to work together with Edit for iCol>0 :
	// Edit(...) will only start an inline editor for an element
	// when the user is done, SetData will be called to actually change the content of an element
	// communication based on String (between view and doc) is sufficient for now.
	// if in the future this choice seems to be inconvenient, change it to a pointer
	// that has to be cast depending on what we expect

	virtual String sTerm(int iCol) = 0;
	// returns the partial mapcalc result up to this cti for alternative iCol (1 based)

	virtual void WriteElements(const char* sSection, const ElementContainer& en);
	virtual void ReadElements(const char* sSection, const ElementContainer& en);
	// Write and Read the member data using ObjectInfo::WriteElement/ReadElement

	virtual void Show(int iCol, bool* pfInitRpr = 0);
	virtual void Properties(int iCol);
	virtual void Histogram(int iCol, bool* pfInitRpr = 0);
	virtual void AggregateValues();
	virtual void Slice();
	virtual void ShowSliced(int iCol, bool* pfInitRpr = 0);
	virtual void HistogramSliced(int iCol, bool* pfInitRpr = 0);
	virtual void AggregateValuesSliced();
	virtual void GenerateContourMaps();
	virtual void ShowContourMaps(int iCol, bool* pfInitRpr = 0);
	virtual void ShowStandardized(int iCol, bool* pfInitRpr = 0);
	virtual bool fDataExists(int iCol);
	virtual bool fSlicedExists(int iCol);
	virtual bool fContourMapExists(int iCol);
	virtual bool fShowStandardizedPossible(int iCol);
	virtual int iRecursivelyCalculateOutput(bool fCountOnly, bool fSpatialOnly, bool fShow);
	virtual void RecursivelyDeleteOutputMaps();
	virtual bool fSpatialItem() = 0;

	virtual void AddContextMenuOptions(CMenu& men) = 0;
	void SetModifiedFlag(BOOL bModified = TRUE);
	virtual String sStatusBarText() = 0;
	virtual COLORREF clrUserColor(int iCol);
	
	virtual void MoveHere(CriteriaTreeItem* ctiToMove) = 0;
	virtual bool fMoveHereAllowed(CriteriaTreeItem* ctiToMove) = 0;
	
	HTREEITEM hItem;
	eCtiTypeTP eCtiType;
	double rWeight;
protected:
	String sDisplayText(); // display text of cti itself .. includes eventual weight
private:
	CriteriaTreeItem* ctiParent;
	CString m_sName;
	CriteriaTreeDoc* pDocument; // the corresponding document
};

class EffectGroup : public CriteriaTreeItem
{
	DECLARE_MESSAGE_MAP()
public:
	EffectGroup(EffectGroup* egParent, CString sName);
	_export EffectGroup(CriteriaTreeDoc* ctdDoc, CString sName);
	// EffectGroup(EffectGroup* egCopy);
	EffectGroup(CriteriaTreeItem* ctiCopy);
	virtual _export ~EffectGroup();
	void OnInsertGroup();
	void OnInsertFactor();
	void OnInsertConstraint();
	void OnInsertScoreFactor();
	void OnInsertScoreConstraint();
	void OnLButtonDblClk();
	void DeleteChild(CriteriaTreeItem* ctiChild);
	void Replace(CriteriaTreeItem* ctiOld, CriteriaTreeItem* ctiNew);
	void DeleteAllChildren();
	list <CriteriaTreeItem*>::iterator itFirstChild();
	list <CriteriaTreeItem*>::iterator itLastChild();
	void SetNrAlternatives(int iNr);
	bool fCalculationPossible(int iCol, bool fFirst=true);
	bool fDone(int iCol, bool fRecursive);
	bool fEditAllowed(int iCol);
	COLORREF clrUserColor(int iCol);
	void GenerateItem(int iCol, bool fCalc, bool fShow);
	void GenerateOutput(int iCol);
	Evaluation* ptrWeights() const;
	CString sDisplayText(int iCol); // return output map name for iCol
	String sTerm(int iCol);
	String sOutputMap(int iCol); // returns for iCol the correct filename with relative path
	int iIconIndex();
	void DeleteTreeElement(); // pops up extra question
	void Delete(int iCol);
	AttributeFileName afnFindFirstInputMap();
	void Show(int iCol, bool* pfInitRpr = 0);
	void Properties(int iCol);
	void Histogram(int iCol, bool* pfInitRpr = 0);
	void AggregateValues();
	void Slice();
	void ShowSliced(int iCol, bool* pfInitRpr = 0);
	void HistogramSliced(int iCol, bool* pfInitRpr = 0);
	void AggregateValuesSliced();
	void GenerateContourMaps();
	void ShowContourMaps(int iCol, bool* pfInitRpr = 0);
	bool fDataExists(int iCol);
	bool fAllDataExists();
	bool fSlicedExists(int iCol);
	bool fContourMapExists(int iCol);
	void AddContextMenuOptions(CMenu& men);
	void WriteElements(const char* sSection, const ElementContainer& en);
	void _export ReadElements(const char* sSection, const ElementContainer& en);
	void Clear(); // for resetting the root in File/New or File/Open (we're not allowed to delete it)
	String sStatusBarText();
	void RecalcWeights();
	list <String> liChildLabels();
	bool fWeightsOk();
	String sWeighMethod();
	void _export GetObjectStructure(ObjectStructure& os);
	int iRecursivelyCalculateOutput(bool fCountOnly, bool fSpatialOnly, bool fShow = false);
	void RecursivelyDeleteOutputMaps();
	bool fSpatialItem();
	void MoveHere(CriteriaTreeItem* ctiToMove); // the virtual function
	void MoveHere(CriteriaTreeItem* ctiToMove, CriteriaTreeItem* ctiAfter); // a local extension of the virtual function
	bool fMoveHereAllowed(CriteriaTreeItem* ctiToMove);
	void InsertSorted(CriteriaTreeItem* ctiToInsert, CriteriaTreeItem* ctiAfter);

private:
	void Edit(int iCol, RECT rect, CWnd* wnd);
	void SetData(int iCol, String sData);
	list <CriteriaTreeItem*> liChildren;
	void OnWeigh();
	void OnUpdateWeigh(CCmdUI*);
	void OnUpdateInsert(CCmdUI*);
	void DeleteContourMaps();
	ValueRange vrGetValueRange(FileName fnSegmentMap);
	Evaluation* pWeights;
	vector <FileName> vOutputMaps;
	vector <String> vScores;
	InPlaceEdit* m_e;
};

class Effect : public CriteriaTreeItem
{
	DECLARE_MESSAGE_MAP()
public:
	enum eTypeTP {iMAP, iSCORE}; // only for create .. there is no member of this type

	enum eValueTP {iUNKNOWN, iVALUE, iCLASS, iBOOL};
	
	static String sInputValueTypeToString(eValueTP e)
	{
		switch(e)
		{
		case iVALUE:
			return "Value";
			break;
		case iCLASS:
			return "Class";
			break;
		case iBOOL:
			return "Bool";
			break;
		default:
			return "";
		}
	};

	static eValueTP eInputValueTypeFromString(String s)
	{
		if ("Value" == s)
			return iVALUE;
		else if ("Class" == s)
			return iCLASS;
		else if ("Bool" == s)
			return iBOOL;
		else
			return iUNKNOWN;
	};

	static Effect* create(EffectGroup* egParent, CString sName, eTypeTP eType, eValueTP eIV, String sDomain, eCtiTypeTP eCT);
	static Effect* create(Effect* eCopy, eTypeTP eType);
	Effect(EffectGroup* egParent, CString sName, eValueTP eIV, FileName fnDomain, eCtiTypeTP eCT);
	Effect(CriteriaTreeItem* ctiCopy);
	virtual ~Effect();
	void SetStandardization(Standardization* std);
	Standardization* ptrStandardization();
	virtual String sInputData(int iCol) = 0;
	long dmTypes();
	void SetInputValueTP(eValueTP eIV);
	eValueTP eInputValue();
	virtual eValueTP eOutputValue() = 0;
	void OnStandardize();
	int iCostBenefit(); // 0 == benefit, 1 == cost, 2 == combination
	bool fEditAllowed(int iCol);
	virtual void ClearData() = 0;
	FileName fnDomain();
	virtual String sInputMinMax(int iCol, bool fMax) = 0;
	void AddContextMenuOptions(CMenu& men);
	virtual RangeReal rrMinMax(int iCol) = 0;
	virtual void WriteElements(const char* sSection, const ElementContainer& en);
	virtual void ReadElements(const char* sSection, const ElementContainer& en);
	String sStatusBarText();
	String sStandardizationMethod();
	void DeleteTreeElement(); // pops up extra question
	void SetDomain(const FileName & fnDomain);
	void SetDomain(const String & sDomain); // for future: prevent FileName("") which might cause problems
	virtual void RefreshDomain();
	virtual void RefreshInputType();
	void MoveHere(CriteriaTreeItem* ctiToMove);
	bool fMoveHereAllowed(CriteriaTreeItem* ctiToMove);

private:
	void OnUpdateStandardize(CCmdUI*);
	Standardization* m_pStd;
	eValueTP m_eInputValue;
	FileName m_fnDomain;
};

class MapEffect : public Effect
{
public:
	MapEffect(EffectGroup* egParent, CString sName, eValueTP eIV, FileName fnDomain, eCtiTypeTP eCT);
	MapEffect(CriteriaTreeItem* ctiCopy);
	virtual ~MapEffect();
	void SetNrAlternatives(int iNr);
	CString sDisplayText(int iCol); // return map name for iCol
	String sTerm(int iCol);
	String sInputData(int iCol);
	bool fCalculationPossible(int iCol, bool fFirst);
	bool fDone(int iCol, bool fRecursive);
	int iIconIndex();
	void Delete(int iCol);
	AttributeFileName afnFindFirstInputMap();
	void ClearData();
	void Show(int iCol, bool* pfInitRpr = 0);
	void ShowStandardized(int iCol, bool* pfInitRpr = 0);
	void Properties(int iCol);
	bool fDataExists(int iCol);
	bool fShowStandardizedPossible(int iCol);
	void Histogram(int iCol, bool* pfInitRpr = 0);
	void AddContextMenuOptions(CMenu& men);
	String sInputMinMax(int iCol, bool fMax);
	RangeReal rrMinMax(int iCol);
	virtual void WriteElements(const char* sSection, const ElementContainer& en);
	virtual void ReadElements(const char* sSection, const ElementContainer& en);
	String sStatusBarText();
	void RefreshDomain();
	void RefreshInputType();
	void GetObjectStructure(ObjectStructure& os);
	void RecursivelyDeleteOutputMaps();
	bool fSpatialItem();
	void AggregateValues();
	void DoNotAggregateValues();
	eValueTP eOutputValue();

private:
	void Edit(int iCol, RECT rect, CWnd* wnd);
	void SetData(int iCol, String sData);
	void SetAVOS(AggregateValueOperationStruct* pavos);
	double rAggregateValue(int iCol); // manages vAggregateValues
	void ResizeAggregateValues(int iNr);
	NameEdit* m_ne;
	FormWithDest* m_frm; // a dummy form to use with the NameEdit that demands a FormBase
	vector <FileName> vMaps;
	vector <RangeReal> vrrMinMax;
	map <pair<String,String>, RangeReal> mprrMinMax; // for caching (attrtbl, sCol) pairs to RangeReal
	AggregateValueOperationStruct * m_pavos;
	vector <double> vAggregateValues; // for caching aggregate values (performance)
};

class ScoreEffect : public Effect
{
public:
	ScoreEffect(EffectGroup* egParent, CString sName, eValueTP eIV, FileName fnDomain, eCtiTypeTP eCT);
	ScoreEffect(CriteriaTreeItem* ctiCopy);
	virtual ~ScoreEffect();
	void SetNrAlternatives(int iNr);
	CString sDisplayText(int iCol); // return map name for iCol
	String sTerm(int iCol);
	String sInputData(int iCol);
	bool fCalculationPossible(int iCol, bool fFirst);
	void ShowStandardized(int iCol, bool* pfInitRpr = 0);
	bool fShowStandardizedPossible(int iCol);
	bool fDone(int iCol, bool fRecursive);
	int iIconIndex();
	void Delete(int iCol);
	void ClearData();
	String sInputMinMax(int iCol, bool fMax);
	RangeReal rrMinMax(int iCol);
	virtual void WriteElements(const char* sSection, const ElementContainer& en);
	virtual void ReadElements(const char* sSection, const ElementContainer& en);
	String sStatusBarText();
	void RefreshInputType();
	bool fSpatialItem();
	eValueTP eOutputValue();

private:
	void Edit(int iCol, RECT rect, CWnd* wnd);
	void SetData(int iCol, String sData);
	vector <double> vScores;
	InPlaceEdit* m_e;
};

#endif // !defined(AFX_CRITERIATREEITEM_H__B7218813_FF2F_421C_B291_8BE958BC6701__INCLUDED_)
