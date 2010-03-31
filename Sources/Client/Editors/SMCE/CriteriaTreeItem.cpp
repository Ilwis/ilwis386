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
// CriteriaTreeItem.cpp: implementation of the CriteriaTreeItem class.
//
//////////////////////////////////////////////////////////////////////

#include "Headers\constant.h"
#include "Client\Headers\formelementspch.h"
#include "Client\Editors\SMCE\CriteriaTreeItem.h"
#include "Client\Editors\SMCE\CriteriaTreeDoc.h"
#include "Client\Editors\SMCE\Evaluation.h"
#include "Client\Editors\SMCE\Standardization.h"
#include "Client\Editors\SMCE\CriteriaTreeView.h"
#include "Client\Editors\Utils\InPlaceEdit.h"
#include "Client\Editors\Utils\InPlaceNameEdit.h"
#include "Client\FormElements\objlist.h"
#include "Client\FormElements\ObjectTreeCtrl.h"
#include "Client\ilwis.h"
#include "Client\FormElements\frmgenap.h"
#include "Client\FormElements\flddom.h"
#include "Client\FormElements\FieldMultiObjectSelect.h" // For AttributeFileName
#include "Headers\messages.h" // for ILWM_EXECUTE
#include "Engine\Domain\dmsort.h" // for DomainSort
#include "Headers\Hs\smce.hs"
#include "Headers\Htp\Ilwismen.htp"
#include "Client\Editors\SMCE\FormAggregateValue.h" // needed for next include
#include "Client\Editors\SMCE\FormAggregateValueMulti.h"
#include "Engine\Domain\dmgroup.h" // for DomainGroup
#include "Engine\Representation\Rpr.h" // for Representation
#include "Engine\Representation\Rprclass.h" // for RepresentationClass
#include "Engine\Base\DataObjects\ObjectStructure.h"
#include "Engine\Scripting\Calc.h" // for Calculator::sSimpleCalc
#include "Headers\Hs\Appforms.hs" // for AggregateValueSingle strings
#include "Engine\Map\Segment\Seg.h" // for contour lines

#define ID_CRITERIA_TREE_COMBO 378
#define ID_CRITERIA_TREE_EDIT 379

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

CriteriaTreeItem::CriteriaTreeItem(CriteriaTreeItem* pParent, CString sName, eCtiTypeTP eCT)
: ctiParent(pParent)
, m_sName(sName)
, rWeight(0)
, hItem(0)
, eCtiType(eCT)
{
	if (ctiParent)
		pDocument = ctiParent->pDocument;
}

CriteriaTreeItem::CriteriaTreeItem(CriteriaTreeDoc* ctdDoc, CString sName, eCtiTypeTP eCT)
: ctiParent(0)
, m_sName(sName)
, rWeight(0)
, hItem(0)
, pDocument(ctdDoc)
, eCtiType(eCT)
{
}

CriteriaTreeItem::CriteriaTreeItem(CriteriaTreeItem* ctiCopy)
: ctiParent(ctiCopy->ctiParent)
, hItem(ctiCopy->hItem)
, m_sName(ctiCopy->m_sName)
, pDocument(ctiCopy->pDocument)
, rWeight(ctiCopy->rWeight)
, eCtiType(ctiCopy->eCtiType)
{
}

CriteriaTreeItem::~CriteriaTreeItem()
{

}

String CriteriaTreeItem::sDisplayText()
{
	if (fHasWeight())
		return String("%.2f %s", rGetWeight(), m_sName);
	else
		return m_sName;
}

String CriteriaTreeItem::sName()
{
	return m_sName;
}

double CriteriaTreeItem::rGetWeight()
{
	return rWeight;
}

int CriteriaTreeItem::iGetNrAlternatives()
{
	if (GetDocument())
		return GetDocument()->iGetNrAlternatives();
	else
		return 0;
}

CriteriaTreeItem* CriteriaTreeItem::ctiPreviousItem()
{
	CriteriaTreeItem* ctiPrev = 0;
	EffectGroup* eg = (EffectGroup*)ctiParent;
	list <CriteriaTreeItem*>::iterator it = eg->itFirstChild();
	while ((it != eg->itLastChild()) && (*it != this))
	{
		ctiPrev = *it;
		++it;
	}
	return ctiPrev;
}

CriteriaTreeDoc* CriteriaTreeItem::GetDocument()
{
	return pDocument;
}

CriteriaTreeItem* CriteriaTreeItem::GetParent()
{
	return ctiParent;
}

void CriteriaTreeItem::SetParent(CriteriaTreeItem* ctiNewParent)
{
	ctiParent = ctiNewParent;
}

void CriteriaTreeItem::SetName(CString sName)
{
	m_sName = sName;
}

AttributeFileName CriteriaTreeItem::afnFindFirstInputMap()
{
	return FileName();
}

class FactorPropertyForm: public FormWithDest
{
public:
  FactorPropertyForm(CWnd* wPar, String* sName, int* iMapScore, list <String> *liReservedLabels)
    : FormWithDest(wPar, SSmcUiFactor)
		, m_liReservedLabels(liReservedLabels)
	{
		fs = new FieldString(root, SSmcUiName, sName);
		fs->SetWidth(60);
		fs->SetCallBack((NotifyProc)&FactorPropertyForm::CallBackFunc);

		SetMenHelpTopic(htpSMCEEditFactor);
		
		create();
	}
	int CallBackFunc(Event*)
	{
		// Now the OK conditions
		if ((fs->sVal().length()>0) && (m_liReservedLabels->end() == find(m_liReservedLabels->begin(), m_liReservedLabels->end(), fs->sVal())))
			EnableOK();
		else    
			DisableOK();
		return 0;
	}
private:
	FieldString* fs;
	list <String> *m_liReservedLabels;
};

class ConstraintPropertyForm : public FormWithDest
{
public:
  ConstraintPropertyForm(CWnd* wPar, String* sName, int* iMapScore, list <String> *liReservedLabels)
    : FormWithDest(wPar, SSmcUiConstraint)
		, vrStdValue(ValueRange(0, 1, 0.001))
		, m_liReservedLabels(liReservedLabels)
  {
		fs = new FieldString(root, SSmcUiName, sName);
		fs->SetWidth(60);
		fs->SetCallBack((NotifyProc)&ConstraintPropertyForm::CallBackFunc);

		/* // map/score determined via menu option
 		rgMapScore = new RadioGroup(root, "Input data", iMapScore, true);
		new RadioButton(rgMapScore, "Maps");
		new RadioButton(rgMapScore, "Scores");
		*/

		SetMenHelpTopic(htpSMCEEditConstraint);

		create();
	}
	int CallBackFunc(Event*)
	{
		if (fs->sVal().length()>0 && (m_liReservedLabels->end() == find(m_liReservedLabels->begin(), m_liReservedLabels->end(), fs->sVal())))
			EnableOK();
		else    
			DisableOK();
		return 0;
	}
private:
	FieldString* fs;

	ValueRange vrStdValue;
	list <String> *m_liReservedLabels;
};

class GroupPropertyForm: public FormWithDest
{
public:
  GroupPropertyForm(CWnd* wPar, String* sName, list <String> *liReservedLabels)
    : FormWithDest(wPar, SSmcUiGroup)
		, m_sName(sName)
		, m_liReservedLabels(liReservedLabels)
	{
		fs = new FieldString(root, SSmcUiName, m_sName);
		fs->SetWidth(60);
		fs->SetCallBack((NotifyProc)&GroupPropertyForm::CallBackFunc);
		
		SetMenHelpTopic(htpSMCEEditGroup);
		
		create();
	}
	int CallBackFunc(Event*)
	{
		// Now the OK conditions
		if ((fs->sVal().length()>0) && (m_liReservedLabels->end() == find(m_liReservedLabels->begin(), m_liReservedLabels->end(), fs->sVal())))
			EnableOK();
		else    
			DisableOK();
		return 0;
	}
private:
	FieldString* fs;
	String* m_sName;
	list <String> *m_liReservedLabels;
};

void CriteriaTreeItem::DeleteTreeElement()
{
	if (ctiParent) // otherwise we're the root that can't be deleted
		((EffectGroup*)ctiParent)->DeleteChild(this);
}

// no object type change, nor factor to constraint and v.versa via this edit
void CriteriaTreeItem::EditTreeElement()
{
	String sLabel (sName());
	Effect* e = dynamic_cast<Effect*>(this); // try what we are
	int iMapScore = 0;
	int iFactorConstraint = (int)eCtiType;
	if (e)
	{
		MapEffect* me = dynamic_cast<MapEffect*>(this); // try deeper
		if (!me)
			iMapScore = 1; // coz it was a score
	}
	int iOldMapScore = iMapScore;
	list <String> liReservedLabels;
	if (GetParent())
	{
		EffectGroup* egParent = dynamic_cast<EffectGroup*>(GetParent());
		if (egParent)
		{
			liReservedLabels = egParent->liChildLabels();
			liReservedLabels.remove(sLabel); // remove own label, otherwise user is forced to edit it!!
		}
	}
	FormWithDest* frm;
	if ((e!=0) && (eCtiType==iFACTOR))
		frm = new FactorPropertyForm(GetDocument()->wndGetActiveView(), &sLabel, &iMapScore, &liReservedLabels);
	else if ((e!=0) && (eCtiType==iCONSTRAINT))
		frm = new ConstraintPropertyForm(GetDocument()->wndGetActiveView(), &sLabel, &iMapScore, &liReservedLabels);
	else
		frm = new GroupPropertyForm(GetDocument()->wndGetActiveView(), &sLabel, &liReservedLabels);
	if (frm->fOkClicked())
	{
		SetModifiedFlag(); // if put at the end, it might crash because of the delete 'this'
		RecursivelyDeleteOutputMaps();
		// handle the parent's children-weight renaming
		if (ctiParent)
		{
			EffectGroup* egParent = dynamic_cast<EffectGroup*>(ctiParent);
			if (egParent && egParent->ptrWeights())
				egParent->ptrWeights()->Rename(sName(), sLabel);
		}
		// first the general changes received from the form
		SetName(sLabel.scVal());
		// then change type if needed
		if (iMapScore != iOldMapScore)
		{
			// type change requested
			CriteriaTreeItem* ctiNew = Effect::create(e, (Effect::eTypeTP)iMapScore); // copy all common settings
			((EffectGroup*)GetParent())->Replace(this, ctiNew); // eventually deletes 'this'
		}
		else
			GetDocument()->UpdateAllViews(0, eITEMUPDATED, this);
	}
	delete frm;
}

bool CriteriaTreeItem::fHasWeight()
{
	return (ctiParent != 0) && (eCtiType == iFACTOR);
}

bool CriteriaTreeItem::fIsConstraint()
{
	return eCtiType == iCONSTRAINT;
}

void CriteriaTreeItem::Show(int iCol, bool* pfInitRpr)
{
	// no input / output data known here ... handle in derivatives
}

void CriteriaTreeItem::Properties(int iCol)
{
	// no input / output data known here ... handle in derivatives
}

void CriteriaTreeItem::Histogram(int iCol, bool* pfInitRpr)
{
	// no input / output data known here ... handle in derivatives
}

void CriteriaTreeItem::AggregateValues()
{
	// no input / output data known here ... handle in derivatives
}

void CriteriaTreeItem::Slice()
{
	// no input / output data known here ... handle in derivatives
}

void CriteriaTreeItem::ShowSliced(int iCol, bool* pfInitRpr)
{
	// no input / output data known here ... handle in derivatives
}

void CriteriaTreeItem::HistogramSliced(int iCol, bool* pfInitRpr)
{
	// no input / output data known here ... handle in derivatives
}

void CriteriaTreeItem::AggregateValuesSliced()
{
	// no input / output data known here ... handle in derivatives
}

void CriteriaTreeItem::GenerateContourMaps()
{
	// no input / output data known here ... handle in derivatives
}

void CriteriaTreeItem::ShowContourMaps(int iCol, bool* pfInitRpr)
{
	// no input / output data known here ... handle in derivatives
}

void CriteriaTreeItem::ShowStandardized(int iCol, bool* pfInitRpr)
{
	// no input / output data known here ... handle in derivatives
}

bool CriteriaTreeItem::fDataExists(int iCol)
{
	// no data
	return false;
}

bool CriteriaTreeItem::fSlicedExists(int iCol)
{
	// no data
	return false;
}

bool CriteriaTreeItem::fContourMapExists(int iCol)
{
	// no data
	return false;
}

bool CriteriaTreeItem::fShowStandardizedPossible(int iCol)
{
	// no data
	return false;
}

int CriteriaTreeItem::iRecursivelyCalculateOutput(bool fCountOnly, bool fSpatialOnly, bool fShow)
{
	return 0;
}

void CriteriaTreeItem::WriteElements(const char* sSection, const ElementContainer& en)
{
	ObjectInfo::WriteElement(sSection, "CTIType", en, sCtiTypeToString(eCtiType));
	ObjectInfo::WriteElement(sSection, "Name", en, String(m_sName));
	ObjectInfo::WriteElement(sSection, "Weight", en, rWeight);
}

void CriteriaTreeItem::ReadElements(const char* sSection, const ElementContainer& en)
{
	String sType;
	ObjectInfo::ReadElement(sSection, "CTIType", en, sType);
	eCtiType = eCtiTypeFromString(sType);
	String sName;
	ObjectInfo::ReadElement(sSection, "Name", en, sName);
	m_sName = sName.scVal();
	if (!ObjectInfo::ReadElement(sSection, "Weight", en, rWeight))
		rWeight = 0;
}

void CriteriaTreeItem::SetModifiedFlag(BOOL bModified)
{
	if (GetDocument())
		GetDocument()->SetModifiedFlag(bModified);
}

void CriteriaTreeItem::GetObjectStructure(ObjectStructure& os)
{
  // handle in derivatives
}

void CriteriaTreeItem::RecursivelyDeleteOutputMaps()
{
	if (GetParent())
		GetParent()->RecursivelyDeleteOutputMaps();
	else if (!fSpatialItem())
		GetDocument()->UpdateAllViews(0, eLABELCHANGED, this);
}

COLORREF CriteriaTreeItem::clrUserColor(int iCol)
{
	if (!fDone(iCol))
	{
		COLORREF clrPink = RGB(255,211,211);
		return clrPink;
	}
	else
		return RGB(0, 0, 0); // the "exception" .. i.e. no coloring wanted
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(EffectGroup, CriteriaTreeItem)
	ON_COMMAND(ID_CT_INSERTGROUP, OnInsertGroup)
	ON_UPDATE_COMMAND_UI(ID_CT_INSERTGROUP, OnUpdateInsert)
	ON_COMMAND(ID_CT_INSERTFACTOR, OnInsertFactor)
	ON_UPDATE_COMMAND_UI(ID_CT_INSERTFACTOR, OnUpdateInsert)
	ON_COMMAND(ID_CT_INSERTCONSTRAINT, OnInsertConstraint)
	ON_UPDATE_COMMAND_UI(ID_CT_INSERTCONSTRAINT, OnUpdateInsert)
	ON_COMMAND(ID_CT_INSERTSCOREFACTOR, OnInsertScoreFactor)
	ON_UPDATE_COMMAND_UI(ID_CT_INSERTSCOREFACTOR, OnUpdateInsert)
	ON_COMMAND(ID_CT_INSERTSCORECONSTRAINT, OnInsertScoreConstraint)
	ON_UPDATE_COMMAND_UI(ID_CT_INSERTSCORECONSTRAINT, OnUpdateInsert)
	ON_COMMAND(ID_CT_GROUP, OnInsertGroup)
	ON_UPDATE_COMMAND_UI(ID_CT_GROUP, OnUpdateInsert)
	ON_COMMAND(ID_CT_FACTOR, OnInsertFactor)
	ON_UPDATE_COMMAND_UI(ID_CT_FACTOR, OnUpdateInsert)
	ON_COMMAND(ID_CT_CONSTRAINT, OnInsertConstraint)
	ON_UPDATE_COMMAND_UI(ID_CT_CONSTRAINT, OnUpdateInsert)
	ON_COMMAND(ID_CT_SCOREFACTOR, OnInsertScoreFactor)
	ON_UPDATE_COMMAND_UI(ID_CT_SCOREFACTOR, OnUpdateInsert)
	ON_COMMAND(ID_CT_SCORECONSTRAINT, OnInsertScoreConstraint)
	ON_UPDATE_COMMAND_UI(ID_CT_SCORECONSTRAINT, OnUpdateInsert)
	ON_COMMAND(ID_CT_WEIGH, OnWeigh)
	ON_UPDATE_COMMAND_UI(ID_CT_WEIGH, OnUpdateWeigh)
END_MESSAGE_MAP()

EffectGroup::EffectGroup(EffectGroup* egParent, CString sName)
: CriteriaTreeItem(egParent, sName, iFACTOR)
, pWeights(0)
{
}

EffectGroup::EffectGroup(CriteriaTreeDoc* ctdDoc, CString sName)
: CriteriaTreeItem(ctdDoc, sName, iFACTOR)
, pWeights(0)
{
}

EffectGroup::EffectGroup(CriteriaTreeItem* ctiCopy)
: CriteriaTreeItem(ctiCopy)
, pWeights(0)
{
	SetNrAlternatives(ctiCopy->iGetNrAlternatives());
}

void EffectGroup::Clear()
{
	// reset the root to a fresh one (called by OnFileNew/OnFileOpen)
	// root is never deleted - this would work inconvenient with UpdateAllViews(0) because we avoid deleting the root there
	eCtiType = iFACTOR;
	if (pWeights)
	{
		delete pWeights;
		pWeights = 0;
	}
	for (unsigned int i=0; i<vOutputMaps.size(); ++i)
		vOutputMaps[i] = "";
}

EffectGroup::~EffectGroup()
{
	list <CriteriaTreeItem*>::iterator iter = liChildren.begin();
	while (iter != liChildren.end())
	{
		if (*iter)
			delete *iter; // ???? is this right?
		++iter;
	}
	if (pWeights)
		delete pWeights;
}

list <CriteriaTreeItem*>::iterator EffectGroup::itFirstChild()
{
	return liChildren.begin();
}

list <CriteriaTreeItem*>::iterator EffectGroup::itLastChild()
{
	return liChildren.end();
}

AttributeFileName EffectGroup::afnFindFirstInputMap()
{
	FileName fn = FileName();
	list<CriteriaTreeItem*>::iterator it = itFirstChild();
	while ((it != itLastChild()) && (fn == FileName()))
	{
		fn = (*it)->afnFindFirstInputMap();
		++it;
	}
	return fn;
}

void EffectGroup::OnInsertGroup()
{
	CriteriaTreeItem* cti;
	String sLabel;
	list <String> liReservedLabels = liChildLabels();
	GroupPropertyForm frm (GetDocument()->wndGetActiveView(), &sLabel, &liReservedLabels);
	if (frm.fOkClicked())
	{
		// create the new object and set object-specific settings
		cti = new EffectGroup(this, sLabel.scVal());
		// now set cti settings
		cti->eCtiType = iFACTOR;
		cti->SetNrAlternatives(iGetNrAlternatives());
		liChildren.push_back(cti);
		RecalcWeights();
		GetDocument()->UpdateAllViews(0, eITEMINSERTED, cti);

		SetModifiedFlag();
		RecursivelyDeleteOutputMaps();
	}
}

void EffectGroup::OnInsertFactor()
{
	CriteriaTreeItem* cti;
	String sLabel;
	Effect::eTypeTP eType (Effect::iMAP);
	list <String> liReservedLabels = liChildLabels();

	FactorPropertyForm frm (GetDocument()->wndGetActiveView(), &sLabel, (int*)&eType, &liReservedLabels);
	if (frm.fOkClicked())
	{
		// create the new object and set object-specific settings
		cti = Effect::create(this, sLabel.scVal(), eType, Effect::iUNKNOWN, "", iFACTOR);
		// now set cti settings
		cti->eCtiType = iFACTOR;
		cti->SetNrAlternatives(iGetNrAlternatives());
		// insert the item before the first group in the sorted list
		list<CriteriaTreeItem*>::iterator it = itFirstChild();
		while ((it!=itLastChild()) && (0 == dynamic_cast<EffectGroup*>(*it)))
			++it;
		// now it points to the first group
		liChildren.insert(it, cti);
		RecalcWeights();
		GetDocument()->UpdateAllViews(0, eITEMINSERTED, cti);

		SetModifiedFlag();
		RecursivelyDeleteOutputMaps();
	}
}

void EffectGroup::OnInsertConstraint()
{
	if (GetDocument())
	{
		EffectGroup* egRoot = GetDocument()->egRoot();
		if (egRoot)
		{
			CriteriaTreeItem* cti;
			String sLabel;
			Effect::eTypeTP eType (Effect::iMAP);
			list <String> liReservedLabels = egRoot->liChildLabels();
			ConstraintPropertyForm frm (GetDocument()->wndGetActiveView(), &sLabel, (int*)&eType, &liReservedLabels);
			if (frm.fOkClicked())
			{
				// create the new object and set object-specific settings
				cti = Effect::create(egRoot, sLabel.scVal(), eType, Effect::iUNKNOWN, "", iCONSTRAINT);
				// now set cti settings
				cti->SetNrAlternatives(iGetNrAlternatives());
				list<CriteriaTreeItem*>::iterator it = egRoot->itFirstChild();
				while ((it!=egRoot->itLastChild()) && (iCONSTRAINT == (*it)->eCtiType))
					++it;
				// now it points to the first factor
				egRoot->liChildren.insert(it, cti);
				GetDocument()->UpdateAllViews(0, eITEMINSERTED, cti);

				SetModifiedFlag();
				RecursivelyDeleteOutputMaps();
			}
		}
	}
}

void EffectGroup::OnInsertScoreFactor()
{
	CriteriaTreeItem* cti;
	String sLabel;
	Effect::eTypeTP eType (Effect::iSCORE);
	list <String> liReservedLabels = liChildLabels();

	FactorPropertyForm frm (GetDocument()->wndGetActiveView(), &sLabel, (int*)&eType, &liReservedLabels);
	if (frm.fOkClicked())
	{
		// create the new object and set object-specific settings
		cti = Effect::create(this, sLabel.scVal(), eType, Effect::iUNKNOWN, "", iFACTOR);
		// now set cti settings
		cti->eCtiType = iFACTOR;
		cti->SetNrAlternatives(iGetNrAlternatives());
		// insert the item before the first group in the sorted list
		list<CriteriaTreeItem*>::iterator it = itFirstChild();
		while ((it!=itLastChild()) && (0 == dynamic_cast<EffectGroup*>(*it)))
			++it;
		// now it points to the first group
		liChildren.insert(it, cti);
		RecalcWeights();
		GetDocument()->UpdateAllViews(0, eITEMINSERTED, cti);

		SetModifiedFlag();
		RecursivelyDeleteOutputMaps();
	}
}

void EffectGroup::OnInsertScoreConstraint()
{
	if (GetDocument())
	{
		EffectGroup* egRoot = GetDocument()->egRoot();
		if (egRoot)
		{
			CriteriaTreeItem* cti;
			String sLabel;
			Effect::eTypeTP eType (Effect::iSCORE);
			list <String> liReservedLabels = egRoot->liChildLabels();
			ConstraintPropertyForm frm (GetDocument()->wndGetActiveView(), &sLabel, (int*)&eType, &liReservedLabels);
			if (frm.fOkClicked())
			{
				// create the new object and set object-specific settings
				cti = Effect::create(egRoot, sLabel.scVal(), eType, Effect::iUNKNOWN, "", iCONSTRAINT);
				// now set cti settings
				cti->SetNrAlternatives(iGetNrAlternatives());
				list<CriteriaTreeItem*>::iterator it = egRoot->itFirstChild();
				while ((it!=egRoot->itLastChild()) && (iCONSTRAINT == (*it)->eCtiType))
					++it;
				// now it points to the first factor
				egRoot->liChildren.insert(it, cti);
				GetDocument()->UpdateAllViews(0, eITEMINSERTED, cti);

				SetModifiedFlag();
				RecursivelyDeleteOutputMaps();
			}
		}
	}
}

void EffectGroup::DeleteTreeElement()
{
	bool fOKToDelete = true;
	if (liChildren.size() > 0)
		fOKToDelete = (IDYES == MessageBox(GetDocument()->wndGetActiveView()->GetSafeHwnd(), String(SSmcErrDeleteItem_S_D.scVal(), sName(), liChildren.size()).scVal(), SSmcErrWarning.scVal(), MB_YESNO|MB_ICONEXCLAMATION));
	else
		fOKToDelete = (IDYES == MessageBox(GetDocument()->wndGetActiveView()->GetSafeHwnd(), String(SSmcErrDeleteItem_S.scVal(), sName()).scVal(), SSmcErrWarning.scVal(), MB_YESNO|MB_ICONEXCLAMATION));

	if (fOKToDelete)
		CriteriaTreeItem::DeleteTreeElement();
}

void EffectGroup::DeleteChild(CriteriaTreeItem* ctiChild)
{
	liChildren.remove(ctiChild);
	if (liChildren.size()>0)
		RecalcWeights();
	GetDocument()->UpdateAllViews(0, eNODEDELETED, ctiChild);
	delete ctiChild;
	GetDocument()->RefreshGeoref();

	SetModifiedFlag();
	RecursivelyDeleteOutputMaps();
}

void EffectGroup::Replace(CriteriaTreeItem* ctiOld, CriteriaTreeItem* ctiNew)
{
	list <CriteriaTreeItem*>::iterator it = liChildren.begin();
	while(it != liChildren.end() && *it != ctiOld)
		++it;
	if (*it == ctiOld) // found; replace it
		*it = ctiNew;
	else	// not found (unlikely - maybe bug?) .. append it
		liChildren.push_back(ctiNew);
	GetDocument()->UpdateAllViews(0, eNODEUPDATED, this);
	if (ctiOld)
		delete ctiOld;

	SetModifiedFlag();
	RecursivelyDeleteOutputMaps();
}

void EffectGroup::MoveHere(CriteriaTreeItem* ctiToMove)
{
	MoveHere(ctiToMove, 0);
}

void EffectGroup::InsertSorted(CriteriaTreeItem* ctiToInsert, CriteriaTreeItem* ctiAfter)
{
	// insert the item at the wanted location (this may un-sort the list, but we solve this later)
	if (ctiAfter != 0)
	{
		list<CriteriaTreeItem*>::iterator it = itFirstChild();
		while ((it != itLastChild()) && ((*it) != ctiAfter))
			++it;
		if (it != itLastChild()) // to insert after the element found
			++it;
		liChildren.insert(it, ctiToInsert);
	}
	else
		liChildren.insert(itFirstChild(), ctiToInsert);
	
	// in order to re-sort the children list, split it into constraints, factors and groups
	list<CriteriaTreeItem*> liConstraints;
	list<CriteriaTreeItem*> liFactors;
	list<CriteriaTreeItem*> liGroups;
	for (list<CriteriaTreeItem*>::iterator it = itFirstChild(); it != itLastChild(); ++it)
	{
		if (0 == dynamic_cast<EffectGroup*>(*it))
		{
			// effect .. constraint or factor
			if ((*it)->fIsConstraint())
				liConstraints.push_back(*it);
			else
				liFactors.push_back(*it);
		}
		else
			liGroups.push_back(*it);
	}

	// reconstruct the list of children
	liChildren.clear();
	for (list<CriteriaTreeItem*>::iterator it = liConstraints.begin(); it != liConstraints.end(); ++it)
		liChildren.push_back(*it);
	for (list<CriteriaTreeItem*>::iterator it = liFactors.begin(); it != liFactors.end(); ++it)
		liChildren.push_back(*it);
	for (list<CriteriaTreeItem*>::iterator it = liGroups.begin(); it != liGroups.end(); ++it)
		liChildren.push_back(*it);
}

void EffectGroup::MoveHere(CriteriaTreeItem* ctiToMove, CriteriaTreeItem* ctiAfter)
{
	// valid cases:
	// 1. ctiToMove is an effect that is dragged to this EffectGroup
	// 2. ctiToMove is a constraint that is dragged to this EffectGroup
	// 3. ctiToMove is an effectgroup that is dragged to this EffectGroup
	// invalid cases:
	// 1. ctiToMove is an EffectGroup and this EffectGroup is a direct or indirect child of it

	// if ctiAfter != 0, append after ctiAfter, otherwise insert as early as possible in liChildren

	if (fMoveHereAllowed(ctiToMove))
	{
		if (ctiToMove->GetParent() == this) // re-arrangement in same group
		{
			// remove ctiToMove from children, then add it at requested position
			liChildren.remove(ctiToMove);
			InsertSorted(ctiToMove, ctiAfter);

			RecalcWeights();
			GetDocument()->UpdateAllViews(0, eNODEUPDATED, this);
		}
		else
		{
			// delete from source EffectGroup
			EffectGroup* egSource = (EffectGroup*) ctiToMove->GetParent();

			if (egSource != 0)
			{
				egSource->liChildren.remove(ctiToMove);
				if (egSource->liChildren.size() > 0)
					egSource->RecalcWeights();
				GetDocument()->UpdateAllViews(0, eNODEDELETED, ctiToMove); // delete from the source
				egSource->RecursivelyDeleteOutputMaps();
			}

			ctiToMove->SetParent(this); // a simple parent replacement
			
			InsertSorted(ctiToMove, ctiAfter);

			RecalcWeights();

			if (0 == dynamic_cast<EffectGroup*>(ctiToMove))
				GetDocument()->UpdateAllViews(0, eITEMINSERTED, ctiToMove);
			else
				GetDocument()->UpdateAllViews(0, eNODEUPDATED, this);				

			RecursivelyDeleteOutputMaps();
		}

		SetModifiedFlag();
	}
}

bool EffectGroup::fMoveHereAllowed(CriteriaTreeItem* ctiToMove)
{
	if (this != ctiToMove)
	{
		if (ctiToMove->GetParent() == this)
			return true; // re-arrangement in same group requested
		else
		{
			EffectGroup* egToMove = dynamic_cast<EffectGroup*>(ctiToMove);
			if (egToMove)
			{
				// confirm that we're not a child of ctiToMove
				CriteriaTreeItem * ctiTest = this;
				while ((ctiTest != 0) && (ctiTest != ctiToMove))
					ctiTest = ctiTest->GetParent();
				return (ctiTest != ctiToMove);
			}
			else
				return (!ctiToMove->fIsConstraint());
		}
	}
	else
		return false; // move to itself
}

void EffectGroup::DeleteAllChildren()
{
	// Meant for cleaning up the "root"
	if (GetParent() == 0)
	{
		list <CriteriaTreeItem*> liChildrenCopy = liChildren;
		liChildren.clear();
		GetDocument()->UpdateAllViews(0, eNODEUPDATED, this);
		for (list <CriteriaTreeItem*>::iterator it = liChildrenCopy.begin(); it != liChildrenCopy.end(); ++it)
			delete *it;
	}
}

void EffectGroup::SetNrAlternatives(int iNr)
{
	list <CriteriaTreeItem*>::iterator iter = liChildren.begin();
	while (iter != liChildren.end())
	{
		(*iter)->SetNrAlternatives(iNr);
		++iter;
	}
	vOutputMaps.resize(iNr);
	vScores.resize(iNr);
}

CString EffectGroup::sDisplayText(int iCol)
{
	if (iCol>0)
	{
		if (fSpatialItem())
		{
			String sRet = String(vOutputMaps[iCol-1].sFileExt());
			if ((sRet.length() == 0) && (GetParent() == 0))
				sRet = SSmcErrNoFilenameGiven;
			return sRet.scVal();
		}
		else if (fCalculationPossible(iCol))
			return vScores[iCol-1].scVal();
		else
			return "";
	}
	else
		return String("%S%S", CriteriaTreeItem::sDisplayText(), sWeighMethod()).scVal();
}

void EffectGroup::Edit(int iCol, RECT rect, CWnd* wnd)
{
	if (iCol > 0)
	{
		bool fEditNeeded = !fCalculationPossible(iCol);
		fEditNeeded = false;

		if (fSpatialItem() || fEditNeeded)
		{
			DWORD style = WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL;
			m_e = new InPlaceEdit((int)this, iCol, vOutputMaps[iCol-1].sRelative(false).scVal());
			m_e->Create(style, rect, wnd, ID_CRITERIA_TREE_EDIT);
		}
		// (int)this :: misuse of the m_iItem argument for passing a pointer
		// the pointer will be returned by InPlaceEdit::SendEndLabelNotify()
	}
	else
	{
		bool fModeIsEdit = false;
		if (GetDocument())
			fModeIsEdit = GetDocument()->GetMode() == CriteriaTreeDoc::eEDITTREE;
		if (fModeIsEdit)
			EditTreeElement();
		else
			EffectGroup::OnWeigh();
	}
}

void EffectGroup::Delete(int iCol)
{
	if (iCol>0)
	{
		vOutputMaps[iCol-1] = "";
		GetDocument()->UpdateAllViews(0, eLABELCHANGED, this);

		SetModifiedFlag();
	}
	else
	{
		bool fModeIsEdit = false;
		if (GetDocument())
			fModeIsEdit = GetDocument()->GetMode() == CriteriaTreeDoc::eEDITTREE;
		if (fModeIsEdit)
			DeleteTreeElement();
	}
}

void EffectGroup::SetData(int iCol, String sData)
{
	if (iCol>0 && sData != "")
	{
		if (vOutputMaps[iCol-1] != FileName(sData.sQuote(), ".mpr"))
		{
			vOutputMaps[iCol-1] = FileName(sData.sQuote(), ".mpr");
			SetModifiedFlag();
		}
	}
}

void EffectGroup::OnWeigh()
{
	list <String> liNames;
	map <String, double> mpChildWeights;
	// send
	for (list <CriteriaTreeItem*>::iterator it = itFirstChild(); it != itLastChild(); ++it)
	{
		if ((*it)->fHasWeight())
		{
			liNames.push_back((*it)->sName());
			mpChildWeights[(*it)->sName()]=(*it)->rWeight;
		}
	}

	if (pWeights)
	{
		Evaluation::eActionTP action = pWeights->ShowForm(GetDocument()->wndGetActiveView(), &liNames, &mpChildWeights);
		if (action == Evaluation::iOTHERMETHOD)
		{
			// user wants different method ..
			Evaluation* pNewWeights = Evaluation::create(GetDocument()->wndGetActiveView(), &liNames, &mpChildWeights, pWeights, Evaluation::iSUMTOONE, SSmcUiWeights);
			if (pNewWeights != pWeights)
			{
				delete pWeights;
				pWeights = pNewWeights;
			}
			SetModifiedFlag(); // pessimistic .. better more often "Save" than no save available when modified
			RecursivelyDeleteOutputMaps();
		}
		else if (action == Evaluation::iOK)
		{
			SetModifiedFlag();
			RecursivelyDeleteOutputMaps();
		}
	}
	else
	{
		pWeights = Evaluation::create(GetDocument()->wndGetActiveView(), &liNames, &mpChildWeights, 0, Evaluation::iSUMTOONE, SSmcUiWeights);
		if (pWeights)
		{
			SetModifiedFlag();
			RecursivelyDeleteOutputMaps();
		}
	}

	// receive
	for (list <CriteriaTreeItem*>::iterator it = itFirstChild(); it != itLastChild(); ++it)
	{
		if ((*it)->fHasWeight())
			(*it)->rWeight = mpChildWeights[(*it)->sName()];
	}

	GetDocument()->UpdateAllViews(0, eWEIGHTSCHANGED, this);
}

void EffectGroup::OnUpdateWeigh(CCmdUI* pCmdUI)
{
	bool fModeOK = false;
	if (GetDocument())
		fModeOK = GetDocument()->GetMode() == CriteriaTreeDoc::eSTDWEIGH;
	// condition: there must be at least one child with weight
	int iNrFactors = 0;
	for (list <CriteriaTreeItem*>::iterator it = itFirstChild(); it != itLastChild(); ++it)
		if ((*it)->fHasWeight())
			++iNrFactors;

	pCmdUI->Enable(fModeOK && (iNrFactors > 1));
}

void EffectGroup::RecalcWeights()
{
	if (pWeights)
	{
		list <String> liNames;
		map <String, double> mpChildWeights;
		// send
		for (list <CriteriaTreeItem*>::iterator it = itFirstChild(); it != itLastChild(); ++it)
		{
			if ((*it)->fHasWeight())
			{
				liNames.push_back((*it)->sName());
				mpChildWeights[(*it)->sName()]=(*it)->rWeight;
			}
		}

		// do
		pWeights->Recalculate(&liNames, &mpChildWeights);

		// receive
		for (list <CriteriaTreeItem*>::iterator it = itFirstChild(); it != itLastChild(); ++it)
		{
			if ((*it)->fHasWeight())
				(*it)->rWeight = mpChildWeights[(*it)->sName()];
		}
	}
	else if (fWeightsOk())
	{
		for (list <CriteriaTreeItem*>::iterator it = itFirstChild(); it != itLastChild(); ++it)
		{
			if ((*it)->fHasWeight())
				(*it)->rWeight = 1; // should find only one
		}
	}
	else // this node is not yet weighed - force user to do so
	{
		for (list <CriteriaTreeItem*>::iterator it = itFirstChild(); it != itLastChild(); ++it)
		{
			if ((*it)->fHasWeight())
				(*it)->rWeight = 0;
		}
	}

	// GetDocument()->UpdateAllViews(0, eWEIGHTSCHANGED, this);
}

COLORREF EffectGroup::clrUserColor(int iCol)
{
	COLORREF clrRet = CriteriaTreeItem::clrUserColor(iCol);
	if (clrRet != RGB(0,0,0))
		return clrRet;
	else if (iCol > 0)
	{
		if (fSpatialItem())
			clrRet = RGB(225,255,225); // green
		else
			clrRet = RGB(225,225,255); // blue
	}

	return clrRet;
}


bool EffectGroup::fEditAllowed(int iCol)
{
	// for iCol>0 always edit ok for maps
	// for iCol == 0 always edit ok in EditTree mode
	// for iCol == 0 and no EditTree, Edit means Weigh .. disable when appropriate
	CriteriaTreeDoc::eEditModeTP eMode = CriteriaTreeDoc::eEDITTREE;
	if (GetDocument())
		eMode = GetDocument()->GetMode();
	int iNrFactors = 0;
	for (list <CriteriaTreeItem*>::iterator it = itFirstChild(); it != itLastChild(); ++it)
		if ((*it)->fHasWeight())
			++iNrFactors;
	return ((iCol>0) && fSpatialItem()) || ((iCol==0) && (eMode == CriteriaTreeDoc::eEDITTREE)) || ((iCol==0) && (iNrFactors > 1));
}

void EffectGroup::OnUpdateInsert(CCmdUI* pCmdUI)
{
	bool fModeOK = false;
	if (GetDocument())
		fModeOK = GetDocument()->GetMode() == CriteriaTreeDoc::eEDITTREE;
	pCmdUI->Enable(fModeOK);
}

Evaluation* EffectGroup::ptrWeights() const
{
	return pWeights;
}

bool EffectGroup::fWeightsOk()
{

	int iNrFactors = 0;
	for (list <CriteriaTreeItem*>::iterator it = itFirstChild(); it != itLastChild(); ++it)
		if ((*it)->fHasWeight())
			++iNrFactors;

	return ((pWeights != 0) || (1 == iNrFactors));
}

bool EffectGroup::fDone(int iCol, bool fRecursive)
{
	// two cases: 1) we're editing the tree, 2) we're std/weighing
	// when we're editing, check the following:
	// check if the group has children
	// for iCol>0 check if the output map is !="" for the root
	// when we're std/weighing, check the following:
	// for iCol == 0 check if there are any children and all factors are weighed
	bool fOK;
	bool fModeIsEdit = false;
	if (GetDocument())
		fModeIsEdit = GetDocument()->GetMode() == CriteriaTreeDoc::eEDITTREE;
	if (fModeIsEdit)
	{
		if (iCol > 0)
			fOK = (GetParent() != 0) || (vOutputMaps[iCol-1].fValid()) || (!fSpatialItem());
		else
			fOK = (liChildren.size()>0);

		if (fRecursive) // means we're doing a final check to see if we can change the mode
		{
			if (GetParent() == 0) // we're the root
			{
				if (fSpatialItem())
				{
					unsigned int i=0;
					while (fOK && (i<vOutputMaps.size()))
					{
						fOK = fOK && vOutputMaps[i].fValid();
						++i;
					}
				}
			}
		}
	}
	else
	{
		if (iCol > 0)
		{
			fOK = true;
			if ((GetParent() == 0) && fSpatialItem()) // we're the root and the output is maps
				fOK = vOutputMaps[iCol-1].fValid();
		}
		else
		{
			fOK = (liChildren.size() > 0);
			bool fHasFactors = false;
			for (list <CriteriaTreeItem*>::iterator it = itFirstChild(); it!=itLastChild(); ++it)
			{
				if (!(*it)->fIsConstraint())
					fHasFactors = true;
			}
			if (fHasFactors)
				fOK = fOK && fWeightsOk();
		}
	}

	if (fRecursive)
		for (list <CriteriaTreeItem*>::iterator it = itFirstChild(); it!=itLastChild(); ++it)
			fOK = fOK && (*it)->fDone(iCol, true);

	return fOK;
}

bool EffectGroup::fCalculationPossible(int iCol, bool fFirst)
{
	// possible when children exist and all ok, and all factors weighed
	// if we're the output, output map should be !=""
	if (iCol > 0)
	{
		bool fOK = (liChildren.size() > 0);
		if (fFirst && fSpatialItem())
			fOK = fOK && vOutputMaps[iCol-1].fValid();
		bool fHasFactors = false;
		for (list <CriteriaTreeItem*>::iterator it = itFirstChild(); it!=itLastChild(); ++it)
		{
			fOK = fOK && (*it)->fCalculationPossible(iCol, false);
			if (!(*it)->fIsConstraint())
				fHasFactors = true;
		}
		if (fHasFactors)
			fOK = fOK && fWeightsOk();

		return fOK;
	}
	else // one is sufficient!
	{
		size_t iNrOutputMaps = vOutputMaps.size();
		for (unsigned int i=1; i<=iNrOutputMaps; ++i)
		{
			if (fCalculationPossible(i))
				return true;
		}
		return false;
	}
}

class FormGenerateOutput: public FormWithDest
{
public:
  FormGenerateOutput(CWnd* mwin, EffectGroup* eg, CriteriaTreeDoc* ctd, int iCol, bool* fShow)
	: FormWithDest(mwin, SSmcUiCIMapCalc)
	{
		String sAlternativeDescr ("");
		if (ctd->iGetNrAlternatives() == 1)
			iCol = 1; // set to simple form even when root is selected
		if (iCol > 0) // one map
		{
			if (eg->GetParent())
				new StaticText(root, SSmcUiGenIntermediateMap);
			else
				new StaticText(root, SSmcUiGenCIMap);
			if (eg->GetDocument()->iGetNrAlternatives() > 1)
				sAlternativeDescr = String(SSmcUiForAlternative_s.scVal(), eg->GetDocument()->sAlternative(iCol));
			if (eg->fCalculationPossible(iCol))
				new StaticText(root, String(SSmcUiMapName_S_S.scVal(), sAlternativeDescr, eg->sOutputMap(iCol)));
		}
		else // (iCol <= 0); all maps
		{
			if (eg->GetParent())
				new StaticText(root, SSmcUiGenIntermediateMaps);
			else
				new StaticText(root, SSmcUiGenCIMaps);
			for (int i=1; i<=ctd->iGetNrAlternatives(); ++i)
			{
				if (ctd->iGetNrAlternatives() > 1)
					sAlternativeDescr = String(SSmcUiForAlternative_s.scVal(), ctd->sAlternative(i));
				if (eg->fCalculationPossible(i))
					new StaticText(root, String(SSmcUiMap_S_S.scVal(), sAlternativeDescr, eg->sOutputMap(i)));
			}
		}

		CheckBox* cbShow = new CheckBox(root, SSmcUiShowAfterCalc, fShow);

		SetMenHelpTopic(htpSMCEGenerateSelectedMap);
		create();
	}
};

void EffectGroup::GenerateOutput(int iCol)
{
	if (fSpatialItem())
	{
		bool fShow=true;
		FormGenerateOutput frm (0, this, GetDocument(), iCol, &fShow);
		if (frm.fOkClicked())
		{
			CWaitCursor cwait;
			bool fExists = false;
			int iProceed = IDYES;
			if (iCol > 0) // one map
			{
				if (vOutputMaps[iCol-1].fExist())
					iProceed = MessageBox(0, SSmcErrOutputExists.scVal(), SSmcErrWarning.scVal(), MB_YESNO);
				if (iProceed == IDYES)
					GenerateItem(iCol, true, fShow);
			}
			else // (iCol <= 0); all maps
			{
				size_t iNrOutputMaps = vOutputMaps.size();
				unsigned int i;
				for (i=0; i<iNrOutputMaps; ++i)
				{
					if (vOutputMaps[i].fExist())
						fExists = true;
				}
				if (fExists)
					iProceed = MessageBox(0, SSmcErrMultiOutputExists.scVal(), SSmcErrWarning.scVal(), MB_YESNO);
				if (iProceed == IDYES)
				{
					Tranquilizer trq;
					trq.Start();
					trq.SetTitle(SSmcUiGeneratingMaps);

					for (i=1; i<=iNrOutputMaps; ++i)
					{
						trq.SetText(String(SSmcUiGenerating_s.scVal(), sDisplayText(i)));
						if (trq.fUpdate(i-1, iNrOutputMaps))
							break;
						GenerateItem(i, true, fShow);
					}
					trq.fUpdate(iNrOutputMaps, iNrOutputMaps);
					trq.Stop();
				}
			}
		}
	}
	else // non-spatial item
	{
		CWaitCursor cwait;
		if (iCol > 0) // one item
			GenerateItem(iCol, true, false);
		else // (iCol <= 0); all items
		{
			size_t iNrOutputMaps = vOutputMaps.size();
			for (int i=1; i<=iNrOutputMaps; ++i)
				GenerateItem(i, true, false);
		}
	}
}

void EffectGroup::GenerateItem(int iCol, bool fCalc, bool fShow)
{
	if ((iCol > 0) && fCalculationPossible(iCol))
	{
		try
		{
			if (fSpatialItem())
			{
				Map mp (vOutputMaps[iCol-1], sTerm(iCol));
				if (mp.fValid()) // prevent boom if after so much care we still generated an invalid mapcalc statement
				{
					Domain dm("nilto1.dom");
					if (dm.fValid()) // prevent boom if for some reason this is not found
						mp->SetDomainValueRangeStruct(dm);
					String sAlternativeDescr ("");
					if (GetDocument()->iGetNrAlternatives() > 1)
						sAlternativeDescr = String(SSmcUiOfAlternative_s.scVal(), GetDocument()->sAlternative(iCol));
					if (GetParent()) // we're intermediate
						mp->SetDescription(String(SSmcUiIntermediateMap_S_S.scVal(), sName(), sAlternativeDescr));
					else
						mp->SetDescription(String(SSmcUiCompositeIndexMap_S.scVal(), sAlternativeDescr));
					if (fCalc)
					{
						mp->Calc();
						if (fShow)
							Show(iCol);
					}
				}
			}
			else // non-spatial item
			{
				String sExpr = sTerm(iCol);
			  vScores[iCol-1] = Calculator::sSimpleCalc(sExpr);
				GetDocument()->UpdateAllViews(0, eLABELCHANGED, this);
			}
		}
	  catch (ErrorObject& err)
		{
			err.Show();
		}  
	}
}

int EffectGroup::iRecursivelyCalculateOutput(bool fCountOnly, bool fSpatialOnly, bool fShow)
{
	int iCount = 0;
	for (list<CriteriaTreeItem*>::iterator it = itFirstChild(); it != itLastChild(); ++it)
		iCount += (*it)->iRecursivelyCalculateOutput(fCountOnly, fSpatialOnly, fShow);
	
	if ((!fSpatialOnly) || fSpatialItem())
	{
		size_t iNrOutputMaps = vOutputMaps.size();
		for (int iCol=1; iCol<=iNrOutputMaps; ++iCol)
		{
			if (fCalculationPossible(iCol))
			{
				++iCount;
				if (!fCountOnly)
					GenerateItem(iCol, true, fShow); // Calc / Show
			}
		}
	}

	return iCount;
}

String EffectGroup::sTerm(int iCol)
{
	// make sure sReturn results in values between 0 and 1 (not class/bool)
	String sFactors ("");
	String sConstraints ("");
	String sReturn("");
	bool fFoundFactors = false;
	bool fFoundConstraints = false;
	// first loop and find the factors
	for (list <CriteriaTreeItem*>::iterator it = itFirstChild(); it != itLastChild(); ++it)
	{
		if (!(*it)->fIsConstraint())
		{
			if (fFoundFactors && (*it)->rGetWeight()!=0.0) // means it is not the first time
				sFactors += "+";
			if ((*it)->rGetWeight() == 1.0)
			{
				sFactors += (*it)->sTerm(iCol);
				fFoundFactors = true;
			}
			else if ((*it)->rGetWeight() != 0.0)
			{
				sFactors += String("%lg*(%S)", (*it)->rGetWeight(), (*it)->sTerm(iCol));
				fFoundFactors = true;
			}
			// fFoundFactors remains false if there's no factor or if all are multiplied by 0 (which is strange)
		}
	}
	// then loop once again to find the constraints
	for (list <CriteriaTreeItem*>::iterator it = itFirstChild(); it != itLastChild(); ++it)
	{
		if ((*it)->fIsConstraint())
		{
			if (fFoundConstraints) // means it is not the first time
				sConstraints += " and ";
			bool fBracketsNeeded = true;
			Effect* e = dynamic_cast<Effect*>(*it);
			if (e && (e->eInputValue() == Effect::iCLASS || e->eInputValue() == Effect::iBOOL))
				fBracketsNeeded = false;
			if (fBracketsNeeded)
				sConstraints += String("(%S)", (*it)->sTerm(iCol));
			else
				sConstraints += (*it)->sTerm(iCol);
			fFoundConstraints = true;
		}
	}
	// Four cases:
	// 1: there are no children
	// 2: there are only factors
	// 3: there are only constraints
	// 4: there are both factors and constraints
	if (!fFoundFactors && !fFoundConstraints)
		sReturn = "0"; // no contribution
	else if (fFoundFactors && !fFoundConstraints)
		sReturn = sFactors; // keep simple
	else if (!fFoundFactors && fFoundConstraints)
		sReturn = String("iff(%S,1,0)", sConstraints); // if constraints are satisfied, return 1, otherwise 0
	else // both true
		sReturn = String("iff(%S,%S,0)", sConstraints, sFactors); // if constraints are satisfied, return factor value, otherwise 0

	return sReturn;
}

String EffectGroup::sOutputMap(int iCol)
{
	if (iCol>0)
		return vOutputMaps[iCol-1].sRelativeQuoted();
	else
		return "";
}

int EffectGroup::iIconIndex()
{
	if (GetParent())
		return iGROUP;
	else
		return iGOAL;
}

void EffectGroup::Show(int iCol, bool* pfInitRpr)
{
	if (iCol>0)
	{
		if (vOutputMaps[iCol-1].fExist())
		{
			GetDocument()->ShowMap(vOutputMaps[iCol-1]);
			if (pfInitRpr && *pfInitRpr)
			{
				Sleep(500); // workaround threading GPF problem
				// This is actually a workaround for a threading problem of Representation
				// When the loop below is used to display multiple maps with additional RPR layers, the representation does not
				// get properly initialized: it seems to get disturbed by the other maps that open
				*pfInitRpr = false;
			}
		}
	}
	else
	{
		// all of them
		bool fInitRpr = true; // delay after first map is opened so that Rpr initializes properly
		int iNrOutputMaps = vOutputMaps.size();
		for (int i=1; i<=iNrOutputMaps; ++i)
			Show(i, &fInitRpr);
	}
}

void EffectGroup::Properties(int iCol)
{
	if (iCol>0 && vOutputMaps[iCol-1].fExist())
	{
		String sExec("prop %S", vOutputMaps[iCol-1].sFullPathQuoted());
		char* str = sExec.sVal();
		IlwWinApp()->GetMainWnd()->SendMessage(ILWM_EXECUTE, 0, (LPARAM)str);
	}
}

void EffectGroup::Histogram(int iCol, bool* pfInitRpr)
{
	if (iCol>0)
	{
		FileName fnHistogram (vOutputMaps[iCol-1], ".his", true);
		if (vOutputMaps[iCol-1].fExist())
		{
			if (!fnHistogram.fExist())
			{
				String sExec("%S=TableHistogram(%S)", fnHistogram.sFullPathQuoted(), vOutputMaps[iCol-1].sFullPathQuoted());
				char* str = sExec.sVal();
				IlwWinApp()->GetMainWnd()->SendMessage(ILWM_EXECUTE, 0, (LPARAM)str);
			}
			String sExec("show %S", fnHistogram.sFullPathQuoted());
			char* str = sExec.sVal();
			IlwWinApp()->GetMainWnd()->SendMessage(ILWM_EXECUTE, 0, (LPARAM)str);
			if (pfInitRpr && *pfInitRpr)
			{
				Sleep(500); // workaround "histogram crash" problem
				// This is actually a workaround for a threading problem
				// When the loop below is used to display multiple histograms, something does not
				// get properly initialized: it seems to get disturbed by the other histograms that open
				*pfInitRpr = false;
			}
		}
	}
	else
	{
		// all of them
		bool fInitRpr = true; // delay after first histogram is opened so that everything is initialized properly
		int iNrOutputMaps = vOutputMaps.size();
		for (int i=1; i<=iNrOutputMaps; ++i)
			Histogram(i, &fInitRpr);
	}
}

void EffectGroup::AggregateValues()
{
	vector<AttributeFileName> vafnMaps;
	int iNrOutputMaps = vOutputMaps.size();
	for (int i=0; i<iNrOutputMaps; ++i)
		if (vOutputMaps[i].fExist())
			vafnMaps.push_back(vOutputMaps[i]);
	new FormAggregateValueMulti(0, &vafnMaps, 0, dmVALUE);
}

class FormSlice: public FormWithDest
{
public:
  FormSlice(CWnd* mwin, int* iSlices, RangeReal* rrMinMax, bool* fEditClassNames)
	: FormWithDest(mwin, "Slice")
	, m_fShow(false)
	, vrStdValue(ValueRange(0, 1, 0.001))
	{
		fbs	|= fbsBUTTONSUNDER | fbsAPPLIC;
		ValueRange vri (2,99);
		new FieldInt(root, SSmcUiNrSlices, iSlices, vri, true);
		new FieldRangeReal(root, SSmcUiMinMax, rrMinMax, vrStdValue);
		cb = new CheckBox(root, SSmcUiEditOutputClasses, fEditClassNames);
		cb->SetCallBack((NotifyProc)&FormSlice::CBCallBackFunc);
		cb->SetIndependentPos();

		SetMenHelpTopic(htpSMCESlice);

		create();
	}
	void OnShow()
	{
		m_fShow = true;
		OnOK();
	}
	void OnDefine()
	{
		m_fShow = false;
		OnOK();
	}
	int CBCallBackFunc(Event*)
	{
		if (butShow.GetSafeHwnd())
			butShow.EnableWindow(!cb->fVal()); 
		return 0;
	}
	bool fShow()
	{
		return m_fShow;
	}
private:
	bool m_fShow;
	CheckBox* cb;
	ValueRange vrStdValue;
	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(FormSlice, FormWithDest)
	// ON_COMMAND(IDHELP, OnHelp)
	// ON_COMMAND(ID_HELP, OnHelp)
	ON_COMMAND(IDYES, OnShow)
	ON_COMMAND(IDNO, OnDefine)
END_MESSAGE_MAP()    

Color colRpr(int i, int iMax)
{
	// i is a value between 0 and iMax
	// this function returns a color where:
	// i between 0 and iMax/2 changes gradually from red (255,0,0) to yellow (255,255,0)
	// i between iMax/2 and iMax changes gradually from yellow (255,255,0) to green (56,216,48)
	// The indicated colors are taken from NILTO1.RPR

	if (i <= (iMax/2))
		return Color(255, 255*2*i/iMax, 0);
	else
		return Color(56+199*2*(iMax-i)/iMax, 216+39*2*(iMax-i)/iMax, 48*(2*i-iMax)/iMax);
}

void EffectGroup::Slice()
{
	RangeReal rrMinMax = RangeReal();
	// first "clean up" invalid maps
	vector<FileName> vfnMaps;
	vector<int> viCols;
	int iNrOutputMaps = vOutputMaps.size();
	for (int i=0; i<iNrOutputMaps; ++i)
	{
		if (vOutputMaps[i].fExist())
		{
			vfnMaps.push_back(vOutputMaps[i]);
			viCols.push_back(i+1); // i+1 is the iCol value that corresponds to the map
		}
	}

	if (vfnMaps.size() > 0)
	{
		// now calc MinMax of all output maps (normally [0,1] or something in that range)
		RangeReal rrMinMaxMap = RangeReal(0,1);
		iNrOutputMaps = vfnMaps.size();
		for (int i=0; i<iNrOutputMaps; ++i)
		{
			bool *fDoNotShowError = (bool *)(IlwWinApp()->Context()->pGetThreadLocalVar(IlwisAppContext::tlvDONOTSHOWFINDERROR));
			bool fDoNotShowErrorPrevious = *fDoNotShowError;
			*fDoNotShowError = true; // don't allow Map to Show a "Find error: Georef.grf".
			try
			{
				Map map(vfnMaps[i]);
				if (map.fValid())
				{
					//if (!map->fUpToDate())
					//	map->MakeUpToDate();
					rrMinMaxMap = map->rrMinMax(true); // calc if needed
				}
			}
			catch (ErrorObject&)
			{
			}
			*fDoNotShowError = fDoNotShowErrorPrevious;
			if (0==i) // first time unconditional
				rrMinMax = rrMinMaxMap;
			else
				rrMinMax = RangeReal(min(rrMinMax.rLo(), rrMinMaxMap.rLo()),max(rrMinMax.rHi(), rrMinMaxMap.rHi()));
		}
		// correction erroneous rrMinMax

		rrMinMax = RangeReal((rrMinMax.rLo() >= -1e300) ? rrMinMax.rLo() : 0,
			(rrMinMax.rHi() >= -1e300) ? rrMinMax.rHi() : 1);

		int iSlices = 5;
		bool fEditClassNames = false;

		rrMinMaxMap = rrMinMax; // rrMinMaxMap will contain the "original" values
		// (before the user has a chance to change them in the form)
		// We want to remember this because the minimum of the first class is:
		// min (all maps, user-defined minimum)

		// now show slice form, default # slices = 4, Min = rrMinMax.Lo(), Max = rrMinMax.Hi();
		FormSlice frm (GetDocument()->wndGetActiveView(), &iSlices, &rrMinMax, &fEditClassNames);
		if (frm.fOkClicked())
		{
			CWaitCursor cwait;
			int iMaxTrq = iSlices + 1 + iNrOutputMaps; // create domain, create representation, slice maps
			int iTrqStep = 0;
			Tranquilizer trq;
			trq.Start();
			trq.SetTitle(SSmcUiTrqSlicingMaps);
			
			trq.SetText(SSmcUiTrqCreatingDomain);
			trq.fUpdate(iTrqStep, iMaxTrq);

			// create a domain group
			String sDomainName ("%S_slices", vfnMaps[0].sShortName(false));
			FileName fnDomainName (sDomainName.sQuote(), ".dom");
			Domain dm (fnDomainName, 0, dmtGROUP);
			DomainGroup* pdg = dm->pdgrp();

			double rStep = rrMinMax.rWidth() / double(iSlices);
			for (int i=1; i <= iSlices; ++i)
			{
				trq.fUpdate(++iTrqStep, iMaxTrq);
				long id;
				if (i==1)
				{
					id = pdg->iAdd(String("suit. %.2f-%.2f", min(rrMinMaxMap.rLo(), rrMinMax.rLo()), rrMinMax.rLo() + i*rStep));
					pdg->SetDescription(id, String("suitability from %.3f to %.3f", min(rrMinMaxMap.rLo(), rrMinMax.rLo()), rrMinMax.rLo() + i*rStep));
				}
				else
				{
					id = pdg->iAdd(String("suit. %.2f-%.2f", rrMinMax.rLo() + (i-1)*rStep, rrMinMax.rLo() + i*rStep));
					pdg->SetDescription(id, String("suitability from %.3f to %.3f", rrMinMax.rLo() + (i-1)*rStep, rrMinMax.rLo() + i*rStep));
				}
				if (i < iSlices)
					pdg->SetUpperBound(id, rrMinMax.rLo() + i*rStep);
				else
					pdg->SetUpperBound(id, rrMinMax.rHi());
			}

			trq.fText(SSmcUiTrqCreatingRpr);
			trq.fUpdate(++iTrqStep, iMaxTrq);
			// fill the representation for this domain

			Representation rpr = pdg->rpr();
			RepresentationClass* prc = rpr->prc();
			if (prc != 0)
			{
				for (int i=1; i <= iSlices; ++i)
					prc->PutColor(i, colRpr(i-1, iSlices-1));
			}

			// slice the output maps in vfnMaps with the domain
			for (int i=0; i<iNrOutputMaps; ++i)
			{
				try
				{
					String sOutputMapName ("%S_sliced", vfnMaps[i].sShortName(false));
					trq.fText(String(SSmcUiTrqCreatingMap_S.scVal(), sOutputMapName));
					trq.fUpdate(++iTrqStep, iMaxTrq);
					FileName fnOutputMapName (sOutputMapName.sQuote(), ".mpr");
					String sExpression ("MapSlicing(%S,%S)", vfnMaps[i].sFullPathQuoted(), fnDomainName.sFullPathQuoted());
					Map mp (fnOutputMapName, sExpression);
					if (mp.fValid() && frm.fShow()) // prevent boom if we generated an invalid expression
					{
						if (!fEditClassNames)
						{
							mp->Calc();
							ShowSliced(viCols[i]);
						}
					}
				}
				catch (ErrorObject& err)
				{
					err.Show();
				}  
			}
			trq.fUpdate(iMaxTrq, iMaxTrq);
			trq.Stop();
			if (fEditClassNames)
			{
				String sExec("show %S -noask", fnDomainName.sFullPathQuoted());
				char* str = sExec.sVal();
				IlwWinApp()->GetMainWnd()->SendMessage(ILWM_EXECUTE, 0, (LPARAM)str);
			}
		}
	}
}

void EffectGroup::ShowSliced(int iCol, bool* pfInitRpr)
{
	if (iCol>0)
	{
		String sSlicedMapName ("%S_sliced", vOutputMaps[iCol-1].sShortName(false));
		FileName fnSlicedMapName (sSlicedMapName.sQuote(), ".mpr");
		if (fnSlicedMapName.fExist())
		{
			GetDocument()->ShowMap(fnSlicedMapName);
			if (pfInitRpr && *pfInitRpr)
			{
				Sleep(500); // workaround "black maps" problem
				// This is actually a workaround for a threading problem of Representation
				// When the loop below is used to display multiple maps, the representation does not
				// get properly initialized: it seems to get disturbed by the other maps that open
				*pfInitRpr = false;
			}
		}
	}
	else
	{
		// all of them
		bool fInitRpr = true; // delay after first map is opened so that Rpr initializes properly
		int iNrOutputMaps = vOutputMaps.size();
		for (int i=1; i<=iNrOutputMaps; ++i)
			ShowSliced(i, &fInitRpr);
	}
}

void EffectGroup::HistogramSliced(int iCol, bool* pfInitRpr)
{
	if (iCol>0)
	{
		String sSlicedMapName ("%S_sliced", vOutputMaps[iCol-1].sShortName(false));
		FileName fnSlicedMapName (sSlicedMapName.sQuote(), ".mpr");
		FileName fnHistogramSliced (fnSlicedMapName, ".his", true);
		if (fnSlicedMapName.fExist())
		{
			if (!fnHistogramSliced.fExist())
			{
				String sExec("%S=TableHistogram(%S)", fnHistogramSliced.sFullPathQuoted(), fnSlicedMapName.sFullPathQuoted());
				char* str = sExec.sVal();
				IlwWinApp()->GetMainWnd()->SendMessage(ILWM_EXECUTE, 0, (LPARAM)str);
			}
			String sExec("show %S", fnHistogramSliced.sFullPathQuoted());
			char* str = sExec.sVal();
			IlwWinApp()->GetMainWnd()->SendMessage(ILWM_EXECUTE, 0, (LPARAM)str);
			if (pfInitRpr && *pfInitRpr)
			{
				Sleep(500); // workaround "histogram crash" problem
				// This is actually a workaround for a threading problem
				// When the loop below is used to display multiple histograms, something does not
				// get properly initialized: it seems to get disturbed by the other histograms that open
				*pfInitRpr = false;
			}
		}
	}
	else
	{
		// all of them
		bool fInitRpr = true; // delay after first histogram is opened so that everything is initialized properly
		vector<FileName> vfnMaps;
		int iNrOutputMaps = vOutputMaps.size();
		for (int i=1; i<=iNrOutputMaps; ++i)
			HistogramSliced(i, &fInitRpr);
	}
}

void EffectGroup::AggregateValuesSliced()
{
	vector<AttributeFileName> vafnMaps;
	int iNrOutputMaps = vOutputMaps.size();
	for (int i=0; i<iNrOutputMaps; ++i)
	{
		String sSlicedMapName ("%S_sliced", vOutputMaps[i].sShortName(false));
		FileName fnSlicedMapName (sSlicedMapName.sQuote(), ".mpr");
		if (fnSlicedMapName.fExist())
			vafnMaps.push_back(fnSlicedMapName);
	}
	new FormAggregateValueMulti(0, &vafnMaps, 0, dmCLASS);
}

class FormContourMaps: public FormWithDest
{
public:
  FormContourMaps(CWnd* mwin, double* rContourLineInterval, bool* fFineContourLines, double* rFineContourLineInterval)
	: FormWithDest(mwin, "Generate Contour Maps")
	, m_fShow(false)
	{
		fbs	|= fbsBUTTONSUNDER | fbsAPPLIC;
		ValueRange vrr (0, 1, 0.01);
		new FieldReal(root, "Contour Line Interval", rContourLineInterval, vrr);
		CheckBox* cb = new CheckBox(root, "Create Fine Contour Maps", fFineContourLines);
		FieldReal* fr = new FieldReal(cb, "Fine Contour Line Interval", rFineContourLineInterval, vrr);
		fr->Align(cb, AL_UNDER);

		// SetMenHelpTopic(htpSMCESlice);

		create();
	}
	void OnShow()
	{
		m_fShow = true;
		OnOK();
	}
	void OnDefine()
	{
		m_fShow = false;
		OnOK();
	}
	bool fShow()
	{
		return m_fShow;
	}
private:
	bool m_fShow;
	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(FormContourMaps, FormWithDest)
	// ON_COMMAND(IDHELP, OnHelp)
	// ON_COMMAND(ID_HELP, OnHelp)
	ON_COMMAND(IDYES, OnShow)
	ON_COMMAND(IDNO, OnDefine)
END_MESSAGE_MAP()    


void EffectGroup::DeleteContourMaps()
{
	if (fSpatialItem())
	{
		int iNrOutputMaps = vOutputMaps.size();
		bool *fDoNotShowError = (bool *)(IlwWinApp()->Context()->pGetThreadLocalVar(IlwisAppContext::tlvDONOTSHOWFINDERROR));
		bool fDoNotShowErrorPrevious = *fDoNotShowError;
		*fDoNotShowError = true; // don't allow Map to Show a "Find error".
		for (int iCol=1; iCol<=iNrOutputMaps; ++iCol)
		{
			if (fContourMapExists(iCol))
			{
				String sContourMapName ("%S_contours", vOutputMaps[iCol-1].sShortName(false));
				FileName fnContourMapName (sContourMapName.sQuote(), ".mps");
				try
				{
					SegmentMap segmp (fnContourMapName);
					if (segmp.fValid())
						segmp->fErase = true;

					CFileFind finder;
					String strPattern(fnContourMapName.sFullPath(false) + "_*.mps");
					BOOL bWorking = finder.FindFile(strPattern.scVal());
					while (bWorking)
					{
						bWorking = finder.FindNextFile();
						FileName fnDetailedContourMapName(finder.GetFilePath());
						try
						{
							SegmentMap segmpdetailed (fnDetailedContourMapName);
							if (segmpdetailed.fValid())
								segmpdetailed->fErase = true;
						}
						catch (ErrorObject&)
						{
						}
					}

					finder.Close();
				}
				catch (ErrorObject&)
				{
				}
			}
		}
		*fDoNotShowError = fDoNotShowErrorPrevious;
	}
}

void EffectGroup::GenerateContourMaps()
{
	RangeReal rrMinMax = RangeReal();
	// first "clean up" invalid maps
	vector<FileName> vfnMaps;
	vector<int> viCols;
	int iNrOutputMaps = vOutputMaps.size();
	for (int i=0; i<iNrOutputMaps; ++i)
	{
		if (vOutputMaps[i].fExist())
		{
			vfnMaps.push_back(vOutputMaps[i]);
			viCols.push_back(i+1); // i+1 is the iCol value that corresponds to the map
		}
	}

	if (vfnMaps.size() > 0)
	{
		RangeReal rrMinMax = RangeReal(0,1);
		iNrOutputMaps = vfnMaps.size();

		double rIsoLineInterval = 0.2;
		double rFineIsoLineInterval = 0.05;
		bool fFineIsoLines = false;

		// show contour generation form
		FormContourMaps frm (GetDocument()->wndGetActiveView(), &rIsoLineInterval, &fFineIsoLines, &rFineIsoLineInterval);
		if (frm.fOkClicked())
		{
			CWaitCursor cwait;
			Tranquilizer trq;
			trq.Start();
			trq.SetTitle("Generating Contour Maps");

			// first delete old maps
			DeleteContourMaps();

			int iNrIsoLines = (int)(rrMinMax.rWidth() / rIsoLineInterval);

			// generate contour maps from the output maps in vfnMaps
			for (int i=0; i<iNrOutputMaps; ++i)
			{
				try
				{
					if (fFineIsoLines)
					{
						double rCurrentIsoLine = rrMinMax.rLo();
						for (int j = 0; j < iNrIsoLines; ++j)
						{
							String sOutputMapName ("%S_contours_%.2f_%.2f_%.2f", vfnMaps[i].sShortName(false), rCurrentIsoLine + rFineIsoLineInterval, rCurrentIsoLine + rIsoLineInterval - rFineIsoLineInterval, rFineIsoLineInterval);
							trq.fText(String(SSmcUiTrqCreatingMap_S.scVal(), sOutputMapName));
							trq.fUpdate(i * iNrIsoLines + j, iNrOutputMaps * (iNrIsoLines + 1));
							FileName fnOutputMapName (sOutputMapName.sQuote(), ".mps");
							String sExpression ("SegmentMapFromRasValueBnd(%S,%lg,%lg,%lg,8,NoSmooth)", vfnMaps[i].sFullPathQuoted(), rCurrentIsoLine + rFineIsoLineInterval, rCurrentIsoLine + rIsoLineInterval, rFineIsoLineInterval); // skip first iso line, which is already in the general contour map
							SegmentMap segmp (fnOutputMapName, sExpression);
							if (segmp.fValid() && frm.fShow()) // prevent boom if we generated an invalid expression
								segmp->Calc();
							rCurrentIsoLine += rIsoLineInterval;
						}
					}
					String sOutputMapName ("%S_contours", vfnMaps[i].sShortName(false));
					trq.fText(String(SSmcUiTrqCreatingMap_S.scVal(), sOutputMapName));
					if (fFineIsoLines)
						trq.fUpdate((i + 1) * iNrIsoLines, iNrOutputMaps * (iNrIsoLines + 1));
					else
						trq.fUpdate(i, iNrOutputMaps);
					FileName fnOutputMapName (sOutputMapName.sQuote(), ".mps");
					String sExpression ("SegmentMapFromRasValueBnd(%S,%lg,%lg,%lg,8,NoSmooth)", vfnMaps[i].sFullPathQuoted(), rrMinMax.rLo(), rrMinMax.rHi(), rIsoLineInterval);
					SegmentMap segmp (fnOutputMapName, sExpression);
					if (segmp.fValid() && frm.fShow()) // prevent boom if we generated an invalid expression
					{
						segmp->Calc();
						ShowContourMaps(viCols[i]);
					}
				}
				catch (ErrorObject& err)
				{
					err.Show();
				}  
			}
			if (fFineIsoLines)
				trq.fUpdate(iNrOutputMaps * (iNrIsoLines + 1), iNrOutputMaps * (iNrIsoLines + 1));
			else
				trq.fUpdate(iNrOutputMaps, iNrOutputMaps);
			trq.Stop();
		}
	}
}

ValueRange EffectGroup::vrGetValueRange(FileName fnSegmentMap)
{
	ValueRange vr = ValueRange();

	bool *fDoNotShowError = (bool *)(IlwWinApp()->Context()->pGetThreadLocalVar(IlwisAppContext::tlvDONOTSHOWFINDERROR));
	bool fDoNotShowErrorPrevious = *fDoNotShowError;
	*fDoNotShowError = true; // don't allow Map to Show a "Find error".
	if (fnSegmentMap.fExist())
	{
		try
		{
			SegmentMap segmp (fnSegmentMap);
			if (segmp.fValid())
			{
				String sExpression = segmp->sExpression();
				Array<String> as;
				int iParms = IlwisObjectPtr::iParseParm(sExpression, as);
				double rIsoStartVal = as[1].rVal();
				double rIsoEndVal = as[2].rVal();
				double rIsoStepVal = as[3].rVal();
				vr = ValueRange(rIsoStartVal, rIsoEndVal, rIsoStepVal);
			}
		}
		catch (ErrorObject&)
		{
		}
	}
	*fDoNotShowError = fDoNotShowErrorPrevious;

	return vr;
}

void EffectGroup::ShowContourMaps(int iCol, bool* pfInitRpr)
{
	if (iCol>0)
	{
		String sContourMapName ("%S_contours", vOutputMaps[iCol-1].sShortName(false));
		FileName fnContourMapName (sContourMapName.sQuote(), ".mps");
		if (fnContourMapName.fExist())
		{
			vector <FileName> vfnContourMaps;
			vector <ValueRange> vvrCustomValueRanges;
			
			vfnContourMaps.push_back(fnContourMapName);
			vvrCustomValueRanges.push_back(vrGetValueRange(fnContourMapName));

			CFileFind finder;
			String strPattern(fnContourMapName.sFullPath(false) + "_*.mps");
			BOOL bWorking = finder.FindFile(strPattern.scVal());
			while (bWorking)
			{
				bWorking = finder.FindNextFile();
				FileName fnDetailedContourMap(finder.GetFilePath());
				vfnContourMaps.push_back(fnDetailedContourMap);
				vvrCustomValueRanges.push_back(vrGetValueRange(fnDetailedContourMap));
			}

			finder.Close();

			GetDocument()->ShowContourMapCombination(vfnContourMaps, vvrCustomValueRanges, vOutputMaps[iCol-1]);
			if (pfInitRpr && *pfInitRpr)
			{
				Sleep(500); // workaround "black maps" problem
				// This is actually a workaround for a threading problem of Representation
				// When the loop below is used to display multiple maps, the representation does not
				// get properly initialized: it seems to get disturbed by the other maps that open
				*pfInitRpr = false;
			}
		}
	}
	else
	{
		// all of them
		bool fInitRpr = true; // delay after first map is opened so that Rpr initializes properly
		int iNrOutputMaps = vOutputMaps.size();
		for (int i=1; i<=iNrOutputMaps; ++i)
			ShowContourMaps(i, &fInitRpr);
	}
}

// #defines also used later at Effect::AddContextMenuOptions
#define sMen(ID) ILWSF("men",ID).scVal()
#define add(ID) men.AppendMenu(MF_STRING, ID, sMen(ID)); 
#define addBreak men.AppendMenu(MF_SEPARATOR);

void EffectGroup::AddContextMenuOptions(CMenu& men)
{
	if (GetDocument())
	{
		if (GetDocument()->GetMode() == CriteriaTreeDoc::eEDITTREE)
		{
			add(ID_CT_INSERTGROUP);
			addBreak;
			add(ID_CT_INSERTFACTOR);
			if (GetParent() == 0)
			{
				add(ID_CT_INSERTCONSTRAINT);
				addBreak;
			}
			add(ID_CT_INSERTSCOREFACTOR);
			if (GetParent() == 0)
				add(ID_CT_INSERTSCORECONSTRAINT);
		}
		else if (GetDocument()->GetMode() == CriteriaTreeDoc::eSTDWEIGH)
		{
			add(ID_CT_WEIGH);
			// condition: there must be at least two children with weight
			int iNrFactors = 0;
			for (list <CriteriaTreeItem*>::iterator it = itFirstChild(); it != itLastChild(); ++it)
				if ((*it)->fHasWeight())
					++iNrFactors;
			men.EnableMenuItem(ID_CT_WEIGH, (iNrFactors > 1) ? MF_ENABLED : MF_GRAYED);
		}
	}
}

bool EffectGroup::fDataExists(int iCol)
{
	if (fSpatialItem())
	{
		if (iCol > 0)
			return (vOutputMaps[iCol-1].fExist());
		else
		{
			int i=1;
			int iNrOutputMaps = vOutputMaps.size();
			while ((i <= iNrOutputMaps) && (!fDataExists(i)))
				++i;
			return (i <= iNrOutputMaps); // if true, at least one existing was found
		}
	}
	else
		return false;
}

bool EffectGroup::fAllDataExists()
{
	int i=1;
	int iNrOutputMaps = vOutputMaps.size();
	while ((i <= iNrOutputMaps) && (fDataExists(i)))
		++i;
	return (i > iNrOutputMaps); // all exist on disk
}

bool EffectGroup::fSlicedExists(int iCol)
{
	if (fSpatialItem())
	{
		if (iCol>0)
		{
			String sSlicedMapName ("%S_sliced", vOutputMaps[iCol-1].sShortName(false));
			FileName fnSlicedMapName (sSlicedMapName.sQuote(), ".mpr");
			return (fnSlicedMapName.fExist());
		}
		else
		{
			int i=1;
			int iNrOutputMaps = vOutputMaps.size();
			while ((i <= iNrOutputMaps) && (!fSlicedExists(i)))
				++i;
			return (i <= iNrOutputMaps); // if true, at least one existing was found
		}
	}
	else
		return false;
}

bool EffectGroup::fContourMapExists(int iCol)
{
	if (fSpatialItem())
	{
		if (iCol>0)
		{
			String sContourMapName ("%S_contours", vOutputMaps[iCol-1].sShortName(false));
			FileName fnContourMapName (sContourMapName.sQuote(), ".mps");
			return (fnContourMapName.fExist());
		}
		else
		{
			int i=1;
			int iNrOutputMaps = vOutputMaps.size();
			while ((i <= iNrOutputMaps) && (!fContourMapExists(i)))
				++i;
			return (i <= iNrOutputMaps); // if true, at least one existing was found
		}
	}
	else
		return false;
}

void EffectGroup::WriteElements(const char* sSection, const ElementContainer& en)
{
	CriteriaTreeItem::WriteElements(sSection, en);

	int iNrOutputMaps = vOutputMaps.size();
	ObjectInfo::WriteElement(sSection, "NrOutputMaps", en, iNrOutputMaps);
	for (int i=0; i<iNrOutputMaps; ++i)
		ObjectInfo::WriteElement(sSection, String("OutputMap%d", i).scVal(), en, vOutputMaps[i]);

	int iNrChildren = liChildren.size();
	ObjectInfo::WriteElement(sSection, "NrChildren", en, iNrChildren);
	int i=0;
	for (list<CriteriaTreeItem*>::iterator it = itFirstChild(); it!=itLastChild(); ++it)
	{
		CriteriaTreeItem* cti = (*it);
		String sType = "Group"; // it can either be Group, MapEffect or ScoreEffect .. if it isn't 2 out of 3, it is the 3rd!
		MapEffect* me = dynamic_cast<MapEffect*>(cti);
		if (me) // success .. it was a MapEffect
			sType = "Map"; // change the type
		else
		{
			ScoreEffect* se = dynamic_cast<ScoreEffect*>(cti);
			if (se) // success .. it was a ScoreEffect
				sType = "Score";
		} // cti sType done!!
		ObjectInfo::WriteElement(sSection, String("Child%dType", i).scVal(), en, sType);
		cti->WriteElements(String("%s_%S_%d", sSection, sName(), i).scVal(), en);
		++i;
	}
	if (pWeights)
		pWeights->WriteElements(sSection, en);
}

void EffectGroup::ReadElements(const char* sSection, const ElementContainer& en)
{
	CriteriaTreeItem::ReadElements(sSection, en);

	int iNrOutputMaps;
	if (!ObjectInfo::ReadElement(sSection, "NrOutputMaps", en, iNrOutputMaps))
		iNrOutputMaps = 1;
	vOutputMaps.resize(iNrOutputMaps);
	for (int i=0; i<iNrOutputMaps; ++i)
		ObjectInfo::ReadElement(sSection, String("OutputMap%d", i).scVal(), en, vOutputMaps[i]);

	vScores.resize(iNrOutputMaps);

	int iNrChildren;
	if (!ObjectInfo::ReadElement(sSection, "NrChildren", en, iNrChildren))
		iNrChildren = 0;
	liChildren.clear();
	for (int i=0; i<iNrChildren; ++i)
	{
		// first find out what we should "new" ...
		String sType;
		CriteriaTreeItem* cti = 0;
		ObjectInfo::ReadElement(sSection, String("Child%dType", i).scVal(), en, sType);
		// then "new" it!
		if ("Group" == sType)
			cti = new EffectGroup(this, ""); // default params .. will be filled by ReadElements
		else if ("Map" == sType)
			cti = new MapEffect(this, "", (Effect::eValueTP)0, "", (eCtiTypeTP)0); // default params .. will be filled by ReadElements
		else if ("Score" == sType)
			cti = new ScoreEffect(this, "", (Effect::eValueTP)0, "", (eCtiTypeTP)0); // default params .. will be filled by ReadElements
		if (cti) // prevent unknown type -> boom
		{
			cti->ReadElements(String("%s_%S_%d", sSection, sName(), i).scVal(), en);
			liChildren.push_back(cti);
		}
	}
	pWeights = Evaluation::CreateFromElementContainer(sSection, en, Evaluation::iSUMTOONE, SSmcUiWeights);
}

String EffectGroup::sStatusBarText()
{
	if (GetParent())
		return String(SSmcUiStatusTextGroup_S_L_D_s.scVal(), sName(), rWeight, liChildren.size(), (liChildren.size()==1)?"":SSmcUiChild_ren.scVal());
	else
		return String(SSmcUiStatusTextGroup_S_D_s.scVal(), sName(), liChildren.size(), (liChildren.size()==1)?"":SSmcUiChild_ren.scVal());
}

list <String> EffectGroup::liChildLabels()
{
	list <String> liReturn;
	for (list<CriteriaTreeItem*>::iterator it = itFirstChild(); it != itLastChild(); ++it)
		liReturn.push_back((*it)->sName());
	return liReturn;		
}

String EffectGroup::sWeighMethod()
{
	String sReturn ("");
	if (pWeights)
		sReturn = String(" -- %S", pWeights->sDescription());
	return sReturn;
}

void EffectGroup::GetObjectStructure(ObjectStructure& os)
{
	// First our own maps (output, sliced output and contour maps)
	int iNrOutputMaps = vOutputMaps.size();
	for (int iCol=1; iCol<=iNrOutputMaps; ++iCol)
	{
		if (fDataExists(iCol))
		{
			os.AddFile(vOutputMaps[iCol-1]);
			// Retrieve the files belonging to the map (georef, tables, domains)
			if (IlwisObject::iotObjectType(vOutputMaps[iCol-1]) != IlwisObject::iotANY)
			{
				IlwisObject obj = IlwisObject::obj(vOutputMaps[iCol-1]);
				if ( obj.fValid())
					obj->GetObjectStructure(os);
			}					
		}
	}
	for (int iCol=1; iCol<=iNrOutputMaps; ++iCol)
	{
		if (fSlicedExists(iCol))
		{
			String sSlicedMapName ("%S_sliced", vOutputMaps[iCol-1].sShortName(false));
			FileName fnSlicedMapName (sSlicedMapName.sQuote(), ".mpr");
			os.AddFile(fnSlicedMapName);
			// Retrieve the files belonging to the map (georef, tables, domains)
			if (IlwisObject::iotObjectType(fnSlicedMapName) != IlwisObject::iotANY)
			{
				IlwisObject obj = IlwisObject::obj(fnSlicedMapName);
				if ( obj.fValid())
					obj->GetObjectStructure(os);
			}					
		}
	}
	for (int iCol=1; iCol<=iNrOutputMaps; ++iCol)
	{
		if (fContourMapExists(iCol))
		{
			String sContourMapName ("%S_contours", vOutputMaps[iCol-1].sShortName(false));
			FileName fnContourMapName (sContourMapName.sQuote(), ".mps");
			os.AddFile(fnContourMapName);
			// Retrieve the files belonging to the map (tables, domains)
			if (IlwisObject::iotObjectType(fnContourMapName) != IlwisObject::iotANY)
			{
				IlwisObject obj = IlwisObject::obj(fnContourMapName);
				if ( obj.fValid())
					obj->GetObjectStructure(os);
			}

			CFileFind finder;
			String strPattern(fnContourMapName.sFullPath(false) + "_*.mps");
			BOOL bWorking = finder.FindFile(strPattern.scVal());
			while (bWorking)
			{
				bWorking = finder.FindNextFile();
				FileName fnDetailedContourMapName(finder.GetFilePath());
				os.AddFile(fnDetailedContourMapName);
				if (IlwisObject::iotObjectType(fnDetailedContourMapName) != IlwisObject::iotANY)
				{
					IlwisObject obj = IlwisObject::obj(fnDetailedContourMapName);
					if ( obj.fValid())
						obj->GetObjectStructure(os);
				}
			}

			finder.Close();
		}
	}
	// Then files of children
	for (list<CriteriaTreeItem*>::iterator it = itFirstChild(); it != itLastChild(); ++it)
		(*it)->GetObjectStructure(os);
}

void EffectGroup::RecursivelyDeleteOutputMaps()
{
	if (fSpatialItem())
	{
		int iNrOutputMaps = vOutputMaps.size();
		bool *fDoNotShowError = (bool *)(IlwWinApp()->Context()->pGetThreadLocalVar(IlwisAppContext::tlvDONOTSHOWFINDERROR));
		bool fDoNotShowErrorPrevious = *fDoNotShowError;
		*fDoNotShowError = true; // don't allow Map to Show a "Find error: Georef.grf".
		for (int iCol=1; iCol<=iNrOutputMaps; ++iCol)
		{
			if (fDataExists(iCol))
			{
				try
				{
					Map mp (vOutputMaps[iCol-1]);
					if (mp.fValid())
						mp->fErase = true;
				}
				catch (ErrorObject&)
				{
				}
			}
			if (fSlicedExists(iCol))
			{
				String sSlicedMapName ("%S_sliced", vOutputMaps[iCol-1].sShortName(false));
				FileName fnSlicedMapName (sSlicedMapName.sQuote(), ".mpr");
				try
				{
					Map mp (fnSlicedMapName);
					if (mp.fValid())
						mp->fErase = true;
				}
				catch (ErrorObject&)
				{
				}
			}
		}
		*fDoNotShowError = fDoNotShowErrorPrevious;
		DeleteContourMaps();
	}
	else
	{
		for (unsigned int i=0; i<vScores.size(); ++i)
			vScores[i] = "";
	}

	CriteriaTreeItem::RecursivelyDeleteOutputMaps();
}

bool EffectGroup::fSpatialItem()
{
	bool fRet = false;
	// return true if one of the following is valid:
	// 1. at least one child with fSpatialItem() == true has weight != 0.0, or
	// 2. if this group has no children (yet)
	// 3. in case of all weights == 0.0, at least one fSpatialItem() -> true, otherwise false

	if (liChildren.size() > 0)
	{
		bool fAllWeightsZero = true;
		bool fAtLeastOneSpatial = false;
		for (list<CriteriaTreeItem*>::iterator it = itFirstChild(); it != itLastChild(); ++it)
		{
			fRet = fRet || ((*it)->fSpatialItem() && ((*it)->rGetWeight() != 0.0));
			fAllWeightsZero = fAllWeightsZero && ((*it)->rGetWeight() == 0.0);
			fAtLeastOneSpatial = fAtLeastOneSpatial || (*it)->fSpatialItem();
		}
		if (fAllWeightsZero)
			fRet = fAtLeastOneSpatial;
	}
	else
		fRet = true;

	return fRet;
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(Effect, CriteriaTreeItem)
	ON_COMMAND(ID_CT_STANDARDIZE, OnStandardize)
	ON_UPDATE_COMMAND_UI(ID_CT_STANDARDIZE, OnUpdateStandardize)
END_MESSAGE_MAP()

Effect::Effect(EffectGroup* egParent, CString sName, eValueTP eIV, FileName fnDomain, eCtiTypeTP eCT)
: CriteriaTreeItem(egParent, sName, eCT)
, m_pStd(0)
, m_eInputValue(eIV)
, m_fnDomain(fnDomain)
{

}

Effect::Effect(CriteriaTreeItem* ctiCopy)
: CriteriaTreeItem(ctiCopy)
, m_pStd(0)
{
	Effect* e = dynamic_cast<Effect*>(ctiCopy);
	if (e)
	{
		m_eInputValue = e->m_eInputValue;
		m_fnDomain = e->m_fnDomain;
	}
}

Effect* Effect::create(EffectGroup* egParent, CString sName, eTypeTP eType, eValueTP eIV, String sDomain, eCtiTypeTP eCT)
{
	Effect* e = 0;
	switch (eType)
	{
		case iMAP:
			e = new MapEffect(egParent, sName, eIV, (sDomain != "") ? FileName(sDomain):FileName(), eCT);
			break;
		case iSCORE:
			e = new ScoreEffect(egParent, sName, eIV, (sDomain != "") ? FileName(sDomain):FileName(), eCT);
			break;
	}
	e->SetStandardization(Standardization::create(e));
	return e;
}

Effect* Effect::create(Effect* eCopy, eTypeTP eType)
{
	Effect* e = 0;
	switch (eType)
	{
		case iMAP:
			e = new MapEffect(eCopy);
			break;
		case iSCORE:
			e = new ScoreEffect(eCopy);
			break;
	}
	if ((e != 0) && eCopy->ptrStandardization())
		e->SetStandardization(eCopy->ptrStandardization()->copy(e));
	return e;
}

Effect::~Effect()
{
	if (m_pStd)
		delete m_pStd;
}

void Effect::SetStandardization(Standardization* std)
{
	if (m_pStd)
		delete m_pStd;
	m_pStd = std;
}

Standardization* Effect::ptrStandardization()
{
	return m_pStd;
}

void Effect::OnStandardize()
{
	if (m_pStd == 0)
		RefreshInputType(); // calls Standardization::create

	if (m_pStd != 0) // if still == 0, something wrong with input type .. prevent crash
	{
		if (m_pStd->fFormAllowed())
			m_pStd->ShowForm();

		GetDocument()->UpdateAllViews(0, eLABELCHANGED, this);
	}
}

void Effect::OnUpdateStandardize(CCmdUI* pCmdUI)
{
	bool fModeOK = false;
	if (GetDocument())
		fModeOK = GetDocument()->GetMode() == CriteriaTreeDoc::eSTDWEIGH;
	pCmdUI->Enable(fModeOK && ((m_pStd == 0) || m_pStd->fFormAllowed()));
}

bool Effect::fEditAllowed(int iCol)
{
	CriteriaTreeDoc::eEditModeTP eMode = CriteriaTreeDoc::eEDITTREE;
	if (GetDocument())
		eMode = GetDocument()->GetMode();
	return (eMode == CriteriaTreeDoc::eEDITTREE) || ((iCol == 0) && ((m_pStd == 0) || (m_pStd->fFormAllowed())));
}

long Effect::dmTypes()
{
	switch(m_eInputValue)
	{
		case iVALUE:
			return dmVALUE;
			break;
		case iCLASS:
			return dmCLASS;
			break;
		case iBOOL:
			return dmBOOL;
			break;
		default:
			return dmVALUE|dmCLASS|dmBOOL; // all types supported for smce
			break;
	}
	return 0;
}

void Effect::SetInputValueTP(eValueTP eIV)
{
	m_eInputValue = eIV;
}

Effect::eValueTP Effect::eInputValue()
{
	return m_eInputValue;
}

FileName Effect::fnDomain()
{
	return m_fnDomain;
}

int Effect::iCostBenefit()
{
	if (m_pStd)
		return m_pStd->iCostBenefit();
	else
		return 2;
}

void Effect::AddContextMenuOptions(CMenu& men)
{
	if (GetDocument())
	{
		add(ID_CT_STANDARDIZE);
		bool fModeOK = GetDocument()->GetMode() == CriteriaTreeDoc::eSTDWEIGH;
		men.EnableMenuItem(ID_CT_STANDARDIZE, (fModeOK && ((m_pStd == 0) || m_pStd->fFormAllowed())) ? MF_ENABLED : MF_GRAYED);
	}
}

void Effect::WriteElements(const char* sSection, const ElementContainer& en)
{
	CriteriaTreeItem::WriteElements(sSection, en);

	ObjectInfo::WriteElement(sSection, "InputValueType", en, sInputValueTypeToString(m_eInputValue));
	ObjectInfo::WriteElement(sSection, "Domain", en, m_fnDomain);

	if (m_pStd)
		m_pStd->WriteElements(sSection, en);
}

void Effect::ReadElements(const char* sSection, const ElementContainer& en)
{
	CriteriaTreeItem::ReadElements(sSection, en);

	String sType;
	ObjectInfo::ReadElement(sSection, "InputValueType", en, sType);
	m_eInputValue = eInputValueTypeFromString(sType);
	ObjectInfo::ReadElement(sSection, "Domain", en, m_fnDomain);

	// set the std after the output value type is known
	SetStandardization(Standardization::create(this));
	if (m_pStd)
		m_pStd->ReadElements(sSection, en);
}

String Effect::sStatusBarText()
{
	String sInputValue("");
	switch(m_eInputValue)
	{
	case iVALUE:
		sInputValue = SSmcTypeValue;
		break;
	case iCLASS:
		sInputValue = SSmcTypeClass;
		break;
	case iBOOL:
		sInputValue = SSmcTypeBool;
		break;
	}

	String sCostBenefit("");
	switch(iCostBenefit())
	{
		case 0:
			sCostBenefit = SSmcUiBenefit + "; ";
			break;
		case 1:
			sCostBenefit = SSmcUiCost + "; ";
			break;
	}
	if (eCtiType == iFACTOR)
		return String(SSmcUiStatusTextEffect_S_S_l.scVal(), sName(), sInputValue, sCostBenefit, rWeight);
	else // iCONSTRAINT
		return String(SSmcUiStatusTextEffect_S_S.scVal(), sName(), sInputValue);
}

String Effect::sStandardizationMethod()
{
	String sReturn ("");
	if (m_pStd && m_pStd->fStandardized())
		sReturn = String(" -- Std:%S", m_pStd->sDescription());
	return sReturn;
}

void Effect::DeleteTreeElement()
{
	bool fOKToDelete = (IDYES == MessageBox(GetDocument()->wndGetActiveView()->GetSafeHwnd(), String(SSmcErrDeleteItem_S.scVal(), sName()).scVal(), SSmcErrWarning.scVal(), MB_YESNO|MB_ICONEXCLAMATION));
	if (fOKToDelete)
		CriteriaTreeItem::DeleteTreeElement();
}

void Effect::SetDomain(const FileName & fnDomain)
{
	m_fnDomain = fnDomain;
}

void Effect::SetDomain(const String & sDomain)
{
	if (sDomain.length() > 0)
		m_fnDomain = FileName(sDomain);
	else
		m_fnDomain = FileName();
}

void Effect::RefreshDomain()
{
	// no implementation .. override if needed
}

void Effect::RefreshInputType()
{
	// no implementation .. override if needed
}

void Effect::MoveHere(CriteriaTreeItem* ctiToMove)
{
	if (GetParent())
		((EffectGroup*)GetParent())->MoveHere(ctiToMove, this);
}

bool Effect::fMoveHereAllowed(CriteriaTreeItem* ctiToMove)
{
	if ((this != ctiToMove) && GetParent())
		return GetParent()->fMoveHereAllowed(ctiToMove);
	else
		return false;
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

MapEffect::MapEffect(EffectGroup* egParent, CString sName, eValueTP eIV, FileName fnDomain, eCtiTypeTP eCT)
: Effect(egParent, sName, eIV, fnDomain, eCT)
, m_ne(0)
, m_frm(0)
, m_pavos(0)
{
}

MapEffect::MapEffect(CriteriaTreeItem* ctiCopy)
: Effect(ctiCopy)
, m_ne(0)
, m_frm(0)
, m_pavos(0)
{
	SetNrAlternatives(ctiCopy->iGetNrAlternatives());
}

MapEffect::~MapEffect()
{
	if (m_frm)
		delete m_frm;
	if (m_pavos)
		delete m_pavos;
}

void MapEffect::SetNrAlternatives(int iNr)
{
	vector <String> svCols;
	// it seems that 'resize' sometimes copies the vector
	// therefore also here we need to workaround the FileName bug (copy constructor forgets sCol)
	for (unsigned int i=0; i<vMaps.size(); ++i)
		svCols.push_back(vMaps[i].sCol); // backup columns !!
	vMaps.resize(iNr);
	for (unsigned int i=0; i<min(vMaps.size(),svCols.size()); ++i) // restore columns!!
		if (svCols[i] != "")
			vMaps[i].sCol=svCols[i];
	vrrMinMax.resize(iNr); // we suppose this initializes at rrUndef?
	ResizeAggregateValues(iNr); // This initializes at rUNDEF!
}

CString MapEffect::sDisplayText(int iCol)
{
	if (iCol>0)
	{
		if (vMaps[iCol-1].sCol != "")
			return String("%S:%S.clm", vMaps[iCol-1].sFile, vMaps[iCol-1].sCol).scVal();
		else
			return String(vMaps[iCol-1].sFileExt()).scVal();
	}
	else
	{
		String sCostBenefit;
		switch(iCostBenefit())
		{
		case 0:
			sCostBenefit = ":benefit";
			break;
		case 1:
			sCostBenefit = ":cost";
			break;
		case 2:
			sCostBenefit = ":none";
			break;
		default:
			sCostBenefit = ":none";
			break;
		}
		return String("%S%S%S", CriteriaTreeItem::sDisplayText(), sStandardizationMethod(), sCostBenefit).scVal();
	}
}

class DirectInPlaceNameEdit : public InPlaceNameEdit
{
	// InPlaceNameEdit was overridden for two reasons:
	// 1) Prevent crash when ComboBox visible and SMCEWindow closes
	// 2) Destroy ComboBox when ObjectTreeCtrl is hiding/destroying
public:
	DirectInPlaceNameEdit(CWnd* wndParent, FormBase* fb, int iItem, int iSubItem, ObjectLister* ol, CRect rcPos, int id)
		: InPlaceNameEdit(wndParent, fb, iItem, iSubItem, ol, rcPos, id)
		, fBusyInSetFocus(false), fBusyInKillFocus(false)
		{
			otc->iHideTime = 0; // in the otc implementation it is uninitialized, but we test on this!!
		}
protected:
	void OnKillFocus(CWnd* pNewWnd)
	{
		if (fBusyInKillFocus) // protect from re-entry and stack overflow
			return;

		// a crash would occur if we managed to keep the NameEdit active with
		// hidden ObjectTreeCtrl, and closed the SMCEWindow (pNewWnd will be 0)
		// (this can not occur with current implementation though)
		if (pNewWnd)
			InPlaceNameEdit::OnKillFocus(pNewWnd);
		else
		{
			fBusyInKillFocus = true; // not all needs protection .. only SendEndLabelNotify could trigger a popup (error message)
			SendEndLabelNotify();
			fBusyInKillFocus = false;
			DestroyWindow();
		}
	}
	void OnSetFocus(CWnd* pOldWnd)
	{
		if (fBusyInSetFocus) // protect from re-entry and stack overflow
			return;
		// when ObjectTreeCtrl closes, OnSetFocus is called
		// the code below destroys the NameEdit
		fBusyInSetFocus = true; // not all needs protection .. only SendEndLabelNotify could trigger a popup (error message)
		SendEndLabelNotify();
		fBusyInSetFocus = false;
		DestroyWindow();

	}
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
	{
		// When ObjectTreeCtrl hides, we only get a CB_GETCURSEL message .. catch it!
		if ((message == CB_GETCURSEL) || (message == CB_GETLBTEXTLEN) || (message == CB_GETLBTEXT))
			if ((!IsWindow(otc->GetSafeHwnd())) || ((otc->iHideTime>0) && (!otc->IsWindowVisible())))
				PostMessage(WM_SETFOCUS); // destroy ourselves! Use Post (Send is too fast and crashes us)
		
		return InPlaceNameEdit::OnWndMsg(message, wParam, lParam, pResult);
	}
private:
	bool fBusyInSetFocus, fBusyInKillFocus;

	DECLARE_MESSAGE_MAP()
};
BEGIN_MESSAGE_MAP(DirectInPlaceNameEdit, InPlaceNameEdit)
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()

class DirectMapListerDomainType: public MapListerDomainType
{
public:
	DirectMapListerDomainType(long types, bool fUseAttrib=false, bool fAllowMapLst=false)
		: MapListerDomainType(types, fUseAttrib, fAllowMapLst)
		, dmTypes(types)
	{}
	virtual bool fOK(const FileName& fnMap, const String& sColName = "")
	{
		String _sColName;
		if ((sCHECK_OBJECT_ONLY != sColName) && (sColName != ""))
			_sColName = sColName.sQuote(); // workaround .. column that needs quoting will be rejected from nameedit
		else
			_sColName = sColName;
		if (!MapListerDomainType::fOK(fnMap, _sColName))
			return false;
		if (sCHECK_OBJECT_ONLY != sColName)
			return true; // filling / expanding tree

		// If we're here, it means fOK is called because the user clicked on an item
		// Do a stricter check - if the user clicked on a map, accept it only if it is of the
		// requested dmType, otherwise refuse it
		// Hint: fOK is not called for attribute columns

		if (0 == dmTypes)
			return true;

		// The following is a simplified version of fAcceptDomain (many types are missing)

		FileName fnDom;
		ObjectInfo::ReadElement("BaseMap", "Domain", fnMap, fnDom);
		if (fnDom.sExt.length() == 0)
			fnDom.sExt = ".dom";
		if (!File::fExist(fnDom))
		{
			fnDom.Dir(IlwWinApp()->Context()->sStdDir());
			if (!File::fExist(fnDom)) 
				return false;
		}
		String sType;
		ObjectInfo::ReadElement("Domain", "Type", fnDom, sType);
	  if ((dmTypes & dmCLASS) && (fCIStrEqual("DomainClass" , sType))) return true;
		if (((dmTypes & dmCLASS) || (dmTypes & dmGROUP)) && (fCIStrEqual("DomainGroup" , sType))) return true;
		if ((dmTypes & dmVALUE) && (fCIStrEqual("DomainValue" , sType))) return true;
		if ((dmTypes & dmBOOL)  && (fCIStrEqual("DomainBool" , sType))) return true;
		return false;
	}
private:
	long dmTypes;
};

class _export DirectMapListerDomainTypeAndGeoRef: public MapListerDomainTypeAndGeoRef
{
public:
	DirectMapListerDomainTypeAndGeoRef(const FileName& fnGeoRef, long types, bool fUseAttrib=false, bool fAllowMapLst=false)
		: MapListerDomainTypeAndGeoRef(fnGeoRef, types, fUseAttrib, fAllowMapLst)
		, dmTypes(types)
	{}
	virtual bool fOK(const FileName& fnMap, const String& sColName="")
	{
		String _sColName;
		if ((sCHECK_OBJECT_ONLY != sColName) && (sColName != ""))
			_sColName = sColName.sQuote(); // workaround .. column that needs quoting will be rejected from nameedit
		else
			_sColName = sColName;
		if (!MapListerDomainTypeAndGeoRef::fOK(fnMap, _sColName))
			return false;
		if (sCHECK_OBJECT_ONLY != sColName)
			return true; // filling / expanding tree
		
		// If we're here, it means fOK is called because the user clicked on an item
		// Do a stricter check - if the user clicked on a map, accept it only if it is of the
		// requested dmType, otherwise refuse it
		// Hint: fOK is not called for attribute columns

		if (0 == dmTypes)
			return true;

		// The following is a simplified version of fAcceptDomain (many types are missing)

		FileName fnDom;
		ObjectInfo::ReadElement("BaseMap", "Domain", fnMap, fnDom);
		if (fnDom.sExt.length() == 0)
			fnDom.sExt = ".dom";
		if (!File::fExist(fnDom))
		{
			fnDom.Dir(IlwWinApp()->Context()->sStdDir());
			if (!File::fExist(fnDom)) 
				return false;
		}
		String sType;
		ObjectInfo::ReadElement("Domain", "Type", fnDom, sType);
	  if ((dmTypes & dmCLASS) && (fCIStrEqual("DomainClass" , sType))) return true;
	  if (((dmTypes & dmCLASS) || (dmTypes & dmGROUP)) && (fCIStrEqual("DomainGroup" , sType))) return true;
		if ((dmTypes & dmVALUE) && (fCIStrEqual("DomainValue" , sType))) return true;
		if ((dmTypes & dmBOOL)  && (fCIStrEqual("DomainBool" , sType))) return true;
		return false;
	}
private:
	long dmTypes;
};

void MapEffect::Edit(int iCol, RECT rect, CWnd* wnd)
{
	if (iCol > 0)
	{
		// InPlaceNameEdit takes care of Create and will also takes care of removing ObjectLister
		ObjectLister *ol;
		if (GetDocument()->ptrGrf() && GetDocument()->ptrGrf()->fValid())
		{
			if ((dmTypes() == dmCLASS) && (fnDomain() != FileName()) && (fnDomain().sFile.length() != 0))
				ol = new MapListerDomainAndGeoRef(GetDocument()->ptrGrf()->ptr()->fnObj, GetDocument()->ptrGrf()->ptr()->rcSize(), fnDomain(), true);
			else // value, bool, and also class of which we didn't decide the domain yet
				ol = new DirectMapListerDomainTypeAndGeoRef(GetDocument()->ptrGrf()->ptr()->fnObj, dmTypes(), true);
		}
		else
		{
			if ((dmTypes() == dmCLASS) && (fnDomain() != FileName()) && (fnDomain().sFile.length() != 0))
				ol = new BaseMapListerDomain(".mpr", fnDomain(), true);
			else
				ol = new DirectMapListerDomainType(dmTypes(), true);
		}

		if (!m_frm)
			m_frm = new FormWithDest(0, "",0); // initialize our dummy form

		m_ne = new DirectInPlaceNameEdit(wnd, m_frm, (int)this, iCol, ol, rect, ID_CRITERIA_TREE_COMBO);
		// (int)this :: misuse of the m_iItem argument for passing a pointer
		// the pointer will be returned by InPlaceNameEdit::SendEndLabelNotify()
		m_ne->SetMinTreeWidth(200); // min width in case user has small cols

		bool *fDoNotShowError = (bool *)(IlwWinApp()->Context()->pGetThreadLocalVar(IlwisAppContext::tlvDONOTSHOWFINDERROR));
		bool fDoNotShowErrorPrevious = *fDoNotShowError;
		*fDoNotShowError = true; // don't allow NameEdit->ObjectTreeCtrl->OnActivate->...->ObjectInfo to Show a "Find error: Georef.grf".

		if (vMaps[iCol-1].sFile != "")
		{
			if (vMaps[iCol-1].sCol != "")
			{
				// NameEdit (and ObjectTreeCtrl) has a function SelectAttribColumn that does what we
				// need here, under the condition that SetVal(map without column) and OpenTreeCtrl()
				// are called first
				FileName fnMap(vMaps[iCol-1]);

				// m_ne->SetVal(vMaps[iCol-1]);
				m_ne->SetVal(fnMap); // this makes sure OpenTreeCtrl gets expanded to almost the right folder
				m_ne->OpenTreeCtrl(); // immediately drop it!
				m_ne->SelectAttribColumn(vMaps[iCol-1]);
			}
			else
			{
				m_ne->SetVal(vMaps[iCol-1]);
				m_ne->OpenTreeCtrl(); // immediately drop it!
			}
		}
		else
			m_ne->OpenTreeCtrl(); // immediately drop it!

		*fDoNotShowError = fDoNotShowErrorPrevious;
	}
	else
	{
		bool fModeIsEdit = false;
		if (GetDocument())
			fModeIsEdit = GetDocument()->GetMode() == CriteriaTreeDoc::eEDITTREE;
		if (fModeIsEdit)
			EditTreeElement();
		else
			OnStandardize();
	}
}

void MapEffect::Delete(int iCol)
{
	if (iCol>0)
	{
		vMaps[iCol-1] = "";
		vrrMinMax[iCol-1] = RangeReal();
		vAggregateValues[iCol-1] = rUNDEF;
		GetDocument()->UpdateAllViews(0, eLABELCHANGED, this);
		GetDocument()->RefreshGeoref();
		RefreshDomain();
		RefreshInputType();

		SetModifiedFlag();
		RecursivelyDeleteOutputMaps();
	}
	else
	{
		bool fModeIsEdit = false;
		if (GetDocument())
			fModeIsEdit = GetDocument()->GetMode() == CriteriaTreeDoc::eEDITTREE;
		if (fModeIsEdit)
			DeleteTreeElement();
	}
}

void MapEffect::ClearData()
{
	for (unsigned int i=0; i<vMaps.size(); ++i)
	{
		vMaps[i] = "";
		vrrMinMax[i] = RangeReal();
		vAggregateValues[i] = rUNDEF;
	}
}

void MapEffect::SetData(int iCol, String sData)
{
	if (iCol>0 && sData != "")
	{
		if (FileName(sData, ".mpr") != vMaps[iCol-1])
		{
			vMaps[iCol-1] = FileName(sData, ".mpr");
			RefreshDomain();
			RefreshInputType();
			if (0 == GetDocument()->ptrGrf())
			{
				bool *fDoNotShowError = (bool *)(IlwWinApp()->Context()->pGetThreadLocalVar(IlwisAppContext::tlvDONOTSHOWFINDERROR));
				bool fDoNotShowErrorPrevious = *fDoNotShowError;
				*fDoNotShowError = true; // don't allow Map to Show a "Find error: Georef.grf".
				try
				{
					Map mp (vMaps[iCol-1]);
					if (mp.fValid())
					{
						GeoRef* gr = new GeoRef(mp->gr());
						if (gr->fValid())
							GetDocument()->SetGeoRef(gr);
					}
				}
				catch (ErrorObject& err)
				{
					err.Show();
				}
				*fDoNotShowError = fDoNotShowErrorPrevious;
			}
			vrrMinMax[iCol-1] = RangeReal(); // reset!
			vAggregateValues[iCol-1] = rUNDEF;
			SetModifiedFlag();
			RecursivelyDeleteOutputMaps();
		}
	}
}

void MapEffect::RefreshDomain()
{
	unsigned int i=0;
	while ((i<vMaps.size()) && (!vMaps[i].fValid()))
		++i; // if i<vMaps.size() we can use vMaps[i]

	if (i>=vMaps.size()) // clear the domain
		SetDomain(FileName());
	else if (fnDomain() == FileName())
	{
		FileName fnDom;

		if (vMaps[i].sCol != "")
		{
			FileName fnBand = vMaps[i]; // workaround for sCol bug: fnAttributeTable can't use it
			FileName fnAttrib = ObjectInfo::fnAttributeTable(fnBand);
			ObjectInfo::ReadElement(String("Col:%S", vMaps[i].sCol).scVal(), "Domain", fnAttrib, fnDom);
		}
		else
		{
			ObjectInfo::ReadElement("BaseMap", "Domain", vMaps[i], fnDom);
		}

		SetDomain(fnDom);
	} // "else": leave as it is
}

void MapEffect::RefreshInputType()
{
	unsigned int i=0;
	while ((i<vMaps.size()) && (!vMaps[i].fValid()))
		++i; // if i<vMaps.size() we can use vMaps[i]

	if (i>=vMaps.size()) // clear the input type and the corresponding std
	{
		SetInputValueTP(iUNKNOWN);
		SetStandardization(0);
		SetAVOS(0);
	}
	else if (eInputValue() == iUNKNOWN)
	{
		FileName fnDom;

		if (vMaps[i].sCol != "")
		{
			FileName fnBand = vMaps[i]; // workaround for sCol bug: fnAttributeTable can't use it
			FileName fnAttrib = ObjectInfo::fnAttributeTable(fnBand);
			ObjectInfo::ReadElement(String("Col:%S", vMaps[i].sCol).scVal(), "Domain", fnAttrib, fnDom);
		}
		else
		{
			ObjectInfo::ReadElement("BaseMap", "Domain", vMaps[i], fnDom);
		}

		if (fnDom.sExt.length() == 0)
			fnDom.sExt = ".dom";
		if (!File::fExist(fnDom))
			fnDom.Dir(IlwWinApp()->Context()->sStdDir());
		if (File::fExist(fnDom)) 
		{
			String sType;
			ObjectInfo::ReadElement("Domain", "Type", fnDom, sType);
			if (fCIStrEqual("DomainClass" , sType))
				SetInputValueTP(iCLASS);
			else if (fCIStrEqual("DomainGroup" , sType))
				SetInputValueTP(iCLASS);
			else if (fCIStrEqual("DomainValue" , sType))
				SetInputValueTP(iVALUE);
			else if (fCIStrEqual("DomainBool" , sType))
				SetInputValueTP(iBOOL);
			SetStandardization(Standardization::create(this));
			SetAVOS(0); // different map type may use different aggregation function
		}
	} // "else": leave as it is
}

String MapEffect::sTerm(int iCol)
{
	if ((iCol > 0) && ptrStandardization() && ptrStandardization()->fStandardized())
	{
		if (m_pavos == 0) // fSpatialItem()
			return ptrStandardization()->sStandardize(vMaps[iCol-1].sRelativeQuoted());
		else
		{
			// CalculateAggregateValue cav(*m_pavos);
			// remove col (if any) coz fExists tries to look for the wrong file
			// FileName fn (vMaps[iCol-1]); // copy forgets sCol
			// String sAggregatedMap = cav.sBuildExpression(fn, vMaps[iCol-1].sCol); // WISH!! for performance reasons we have to do the next line instead, i.e. hardcode the aggregated value
			// String sAggregatedMap = cav.sCalculateAggregateValue(fn, vMaps[iCol-1].sCol);
			double rVal = rAggregateValue(iCol);
			String sAggregatedMap ("%lg", rVal);
			return ptrStandardization()->sStandardize(sAggregatedMap);
		}
	}
	else
		return "";
}

void MapEffect::RecursivelyDeleteOutputMaps()
{
	if (fSpatialItem())
	{
		int iNrMaps = vMaps.size();
		bool *fDoNotShowError = (bool *)(IlwWinApp()->Context()->pGetThreadLocalVar(IlwisAppContext::tlvDONOTSHOWFINDERROR));
		bool fDoNotShowErrorPrevious = *fDoNotShowError;
		*fDoNotShowError = true; // don't allow Map to Show a "Find error: Georef.grf".
		for (int iCol=1; iCol<=iNrMaps; ++iCol)
		{
			String sStdMapName ("%S_standardized", vMaps[iCol-1].sShortName(false));
			FileName fnStdMap (sStdMapName.sQuote(), ".mpr");

			if (fnStdMap.fExist()) // delete it
			{
				try
				{
					Map mp (fnStdMap);
					if (mp.fValid())
						mp->fErase = true;
				}
				catch (ErrorObject&)
				{
				}
			}
		}
		*fDoNotShowError = fDoNotShowErrorPrevious;
	}

	CriteriaTreeItem::RecursivelyDeleteOutputMaps();
}

void MapEffect::ShowStandardized(int iCol, bool* pfInitRpr)
{
	if (fSpatialItem())
	{
		if ((iCol > 0) && fCalculationPossible(iCol, true))
		{
			try
			{
				String sStdMapName ("%S_standardized", vMaps[iCol-1].sShortName(false));
				FileName fnStdMap (sStdMapName.sQuote(), ".mpr");

				bool fSleep = false; // only sleep if first map exists, and rpr is not yet initialized

				if (!fnStdMap.fExist()) // calculate it
				{
					Map mp (fnStdMap, sTerm(iCol));
					if (mp.fValid()) // prevent boom if after so much care we still generated an invalid mapcalc statement
					{
						Domain dm("nilto1.dom");
						if (dm.fValid()) // prevent boom if for some reason this is not found
							mp->SetDomainValueRangeStruct(dm);
						if (iGetNrAlternatives() > 1)
							mp->SetDescription(String("Standardized map for criterion '%S' under alternative '%S'", sName(), GetDocument()->sAlternative(iCol)));
						else
							mp->SetDescription(String("Standardized map for criterion '%S'", sName()));
						mp->Calc();
					}
				}
				else
					fSleep = true;

				GetDocument()->ShowMap(fnStdMap);

				if (pfInitRpr && *pfInitRpr && fSleep)
				{
					Sleep(500); // workaround threading GPF problem
					// This is actually a workaround for a threading problem of Representation
					// When the loop below is used to display multiple maps with additional RPR layers, the representation does not
					// get properly initialized: it seems to get disturbed by the other maps that open
					*pfInitRpr = false;
				}
			}
			catch (ErrorObject& err)
			{
				err.Show();
			}  
		}
		else
		{
			// all of them
			bool fInitRpr = true; // delay after first map is opened so that Rpr initializes properly
			int iNrMaps = vMaps.size();
			for (int i=1; i<=iNrMaps; ++i)
				ShowStandardized(i, &fInitRpr);
		}
	}
	else // non-spatial item
	{
		int i = (iCol > 0) ? iCol : 1;
		int iNrMaps = vMaps.size();
		String sMessage;
		while (i <= iNrMaps)
		{
			if (fCalculationPossible(i, true))
			{
				try
				{
					String sExpr = sTerm(i);
					String sResult = Calculator::sSimpleCalc(sExpr);
					if (sMessage.length() > 0)
						sMessage += "\n";
					if (iGetNrAlternatives() > 1)
						sMessage += String("%s: %S    (expression = %S)", GetDocument()->sAlternative(i), sResult, sExpr);
					else
						sMessage += vMaps[i-1].sShortName() + ": " + sResult + "    (expression = " + sExpr + ")";
				}
				catch (ErrorObject& err)
				{
					err.Show();
				}
			}
			i += (iCol > 0) ? iNrMaps : 1; // jump out of "while loop" if the iCol had a value > 0
		}
		if (sMessage.length() > 0)
			MessageBox(0, sMessage.scVal(), String("Standardized value%s for aggregated criterion '%S'", (iGetNrAlternatives() > 1) ? "s" : "", sName()).scVal(), MB_OK);
	}
}

bool MapEffect::fShowStandardizedPossible(int iCol)
{
	if (iCol > 0)
		return fCalculationPossible(iCol, true);
	else // return true if it is at least possible to calculate one alternative
	{
		int i=1;
		int iNrInputMaps = vMaps.size();
		while ((i <= iNrInputMaps) && (!fCalculationPossible(i, true)))
			++i;
		return (i <= iNrInputMaps); // if true, at least one "possible to calculate" was found
	}
}

String MapEffect::sInputData(int iCol)
{
	if (iCol > 0)
		return vMaps[iCol-1].sRelativeQuoted();
	else
		return "";
}

AttributeFileName MapEffect::afnFindFirstInputMap()
{
	FileName fn = FileName();
	unsigned int i=0;
	while ((i<vMaps.size()) && (fn == FileName()))
	{
		if (vMaps[i].fValid())
			fn = vMaps[i];
		++i;
	}
	return fn;
}

bool MapEffect::fDone(int iCol, bool fRecursive)
{
	// for iCol>0 check if the selected input map exists, independent on the mode
	// if mode is std/weigh, check  for iCol==0 if the thing is standardized

	// NOTE: Decide on the fRecursive flag if we should check on the availability of all input data
	bool fOK = true;

	if (iCol > 0)
	{
		// FileName fExists bug workaround:
		// remove col (if any) coz fExists tries to look for the wrong file
		FileName fn (vMaps[iCol-1]); // copy forgets sCol
		fOK = fn.fExist(); // now check
	}
	else
	{
		// iCol == 0
		bool fModeIsEdit = false;
		if (GetDocument())
			fModeIsEdit = GetDocument()->GetMode() == CriteriaTreeDoc::eEDITTREE;
		if (fModeIsEdit)
		{
			if (fRecursive) // means we're doing a final check to see if we can change the mode
			{
				unsigned int i=1;
				while (fOK && (i<=vMaps.size()))
				{
					fOK = fOK && fDone(i, false); // not recursive!!
					++i;
				}
			}
		}
		else // fModeIsEdit == false
			fOK = (ptrStandardization() && ptrStandardization()->fStandardized());
	}
	return fOK;
}

bool MapEffect::fCalculationPossible(int iCol, bool fFirst)
{
	if (iCol > 0)
	{
		// Same as above - FileName fExists bug workaround:
		// remove col (if any) coz fExists tries to look for the wrong file
		FileName fn (vMaps[iCol-1]); // copy forgets sCol
		return (ptrStandardization() && ptrStandardization()->fStandardized() && fn.fExist());
	}
	else
		return false; // shouldn't happen
}

void MapEffect::Show(int iCol, bool* pfInitRpr)
{
	if (iCol>0)
	{
		// fExists workaround
		FileName fn (vMaps[iCol-1]); // copy forgets sCol
		if (fn.fExist())
		{
			GetDocument()->ShowMap(fn);
			if (pfInitRpr && *pfInitRpr)
			{
				Sleep(500); // workaround threading GPF problem
				// This is actually a workaround for a threading problem of Representation
				// When the loop below is used to display multiple maps with additional RPR layers, the representation does not
				// get properly initialized: it seems to get disturbed by the other maps that open
				*pfInitRpr = false;
			}
		}
	}
	else
	{
		// all of them
		bool fInitRpr = true; // delay after first map is opened so that Rpr initializes properly
		int iNrInputMaps = vMaps.size();
		for (int i=1; i<=iNrInputMaps; ++i)
			Show(i, &fInitRpr);
	}
}

void MapEffect::Properties(int iCol)
{
	if (iCol>0)
	{
		// fExists workaround
		FileName fn (vMaps[iCol-1]); // copy forgets sCol
		if (fn.fExist())
		{
			String sExec("prop %S", fn.sFullPathQuoted());
			char* str = sExec.sVal();
			IlwWinApp()->GetMainWnd()->SendMessage(ILWM_EXECUTE, 0, (LPARAM)str);
		}
	}
}

bool MapEffect::fDataExists(int iCol)
{
	if (iCol>0)
	{
		// fExists workaround
		FileName fn (vMaps[iCol-1]); // copy forgets sCol
		return (fn.fExist());
	}
	else
	{
		int i=1;
		int iNrInputMaps = vMaps.size();
		while ((i <= iNrInputMaps) && (!fDataExists(i)))
			++i;
		return (i <= iNrInputMaps); // if true, at least one existing was found
	}
}

void MapEffect::Histogram(int iCol, bool* pfInitRpr)
{
	if (iCol>0)
	{
		FileName fnHistogram (vMaps[iCol-1], ".his", true);
		if (vMaps[iCol-1].fExist())
		{
			if (!fnHistogram.fExist())
			{
				String sExec("%S=TableHistogram(%S)", fnHistogram.sFullPathQuoted(), vMaps[iCol-1].sFullPathQuoted());
				char* str = sExec.sVal();
				IlwWinApp()->GetMainWnd()->SendMessage(ILWM_EXECUTE, 0, (LPARAM)str);
			}
			String sExec("show %S", fnHistogram.sFullPathQuoted());
			char* str = sExec.sVal();
			IlwWinApp()->GetMainWnd()->SendMessage(ILWM_EXECUTE, 0, (LPARAM)str);
			if (pfInitRpr && *pfInitRpr)
			{
				Sleep(500); // workaround "histogram crash" problem
				// This is actually a workaround for a threading problem
				// When the loop below is used to display multiple histograms, something does not
				// get properly initialized: it seems to get disturbed by the other histograms that open
				*pfInitRpr = false;
			}
		}
	}
	else
	{
		// all of them
		bool fInitRpr = true; // delay after first histogram is opened so that everything is initialized properly
		int iNrMaps = vMaps.size();
		for (int i=1; i<=iNrMaps; ++i)
			Histogram(i, &fInitRpr);
	}
}

String MapEffect::sInputMinMax(int iCol, bool fMax)
{
	String sReturn = "";
	if ((iCol>0) && (vMaps[iCol-1].fValid()) && (vMaps[iCol-1]!=FileName()))
	{
		// 3 cases:
		// 1: we're having a class map with value cols
		// 2: we're having a true value map
		// discriminate between them checking sCol
		// 3: we're aggregating a map
		if (m_pavos == 0) // fSpatialInput()
		{
			if (vMaps[iCol-1].sCol != "")
			{
				// it is a class map .. mapmax/mapmin does not work on map.col
				// we can do nothing better than hardcode the max or min value in the string
				sReturn = String("%lg", fMax?rrMinMax(iCol).rHi():rrMinMax(iCol).rLo());
			}
			else
			{
				// good! use mapmax/mapmin
				String sMinMax(fMax?"max":"min");
				sReturn = String("map%S(%S)", sMinMax, vMaps[iCol-1].sRelativeQuoted());

				// The following is needed for mapmax/mapmin to return correct values
				bool *fDoNotShowError = (bool *)(IlwWinApp()->Context()->pGetThreadLocalVar(IlwisAppContext::tlvDONOTSHOWFINDERROR));
				bool fDoNotShowErrorPrevious = *fDoNotShowError;
				*fDoNotShowError = true; // don't allow Map to Show a "Find error: Georef.grf".
				try
				{
					Map map(vMaps[iCol-1]);
					if (map.fValid())
					{
						//if (!map->fUpToDate())
						//	map->MakeUpToDate();
						RangeReal rrDummy = map->rrMinMax(true); // calc if needed
					}
				}
				catch(ErrorObject&)
				{
				}
				*fDoNotShowError = fDoNotShowErrorPrevious;
			}
		}
		else
		{
			// CalculateAggregateValue cav(*m_pavos);
			// remove col (if any) coz fExists tries to look for the wrong file
			// FileName fn (vMaps[iCol-1]); // copy forgets sCol
			// sReturn = cav.sBuildExpression(fn, vMaps[iCol-1].sCol); // WISH!! for performance reasons we have to do the next instead, i.e. hardcode the aggregated value
			// sReturn = cav.sCalculateAggregateValue(fn, vMaps[iCol-1].sCol);
			double rVal = rAggregateValue(iCol);
			sReturn = String("%lg", rVal);
		}
	}

	return sReturn;
}

RangeReal MapEffect::rrMinMax(int iCol)
{
	RangeReal rrReturn = RangeReal();

	if (iCol>0)
	{
		if (vrrMinMax[iCol-1].fValid())
			rrReturn = vrrMinMax[iCol-1];
		else
		{
			// 3 cases:
			// 1: we're having a class map with value cols
			// 2: we're having a true value map
			// discriminate between them checking sCol
			// 3: we're aggregating a map

			if ((vMaps[iCol-1].fValid()) && (vMaps[iCol-1] != FileName()))
			{
				if (m_pavos == 0) // fSpatialInput()
				{
					if (vMaps[iCol-1].sCol != "") // class map
					{
						FileName fnMap(vMaps[iCol-1]); // strip-off sCol - fnAttribTbl can't handle it
						// our goal is to "catch" an attribute table
						// like the NameEdit, prefer the one of the map, having the one of the domain as a 2nd choice
						// the following function does this
						FileName fnAttribTbl = ObjectInfo::fnAttributeTable(fnMap);
						if (fnAttribTbl.fValid() && (fnAttribTbl != FileName()))
						{
							// get the minmax of the column we know
							// first check if we already have it in our cache
							rrReturn = mprrMinMax[pair<String, String>(fnAttribTbl.sRelativeQuoted(), vMaps[iCol-1].sCol)];
							if (!rrReturn.fValid()) // unfortunately we have to do the expensive calc
							{
								try
								{
									Table tbl (fnAttribTbl);
									if (tbl.fValid())
									{
										Column col (tbl->col(vMaps[iCol-1].sCol));
										if (col.fValid())
										{
											if (!col->fUpToDate())
												col->MakeUpToDate();
											rrReturn = col->rrMinMax();
										}
									}
								}
								catch (ErrorObject&)
								{
								}
    						if (rrReturn.fValid())
									mprrMinMax[make_pair(fnAttribTbl.sRelativeQuoted(), vMaps[iCol-1].sCol)] = rrReturn; // cache it!
							}
						}
					}
					else // values or bool, use BaseMapPtr::rrMinMax
					{
						bool *fDoNotShowError = (bool *)(IlwWinApp()->Context()->pGetThreadLocalVar(IlwisAppContext::tlvDONOTSHOWFINDERROR));
						bool fDoNotShowErrorPrevious = *fDoNotShowError;
						*fDoNotShowError = true; // don't allow Map to Show a "Find error: Georef.grf".
						try
						{
							Map map(vMaps[iCol-1]);
							if (map.fValid())
							{
								//if (!map->fUpToDate())
								//	map->MakeUpToDate();
								rrReturn = map->rrMinMax(true); // calc if needed
							}
						}
						catch(ErrorObject&)
						{
						}
						*fDoNotShowError = fDoNotShowErrorPrevious;
					}
				}
				else // we're aggregating a map
				{
					// CalculateAggregateValue cav(*m_pavos);
					// remove col (if any) coz fExists tries to look for the wrong file
					// FileName fn (vMaps[iCol-1]); // copy forgets sCol
					// String sResult = cav.sCalculateAggregateValue(fn, vMaps[iCol-1].sCol);
					double rResult = rAggregateValue(iCol);
					rrReturn = RangeReal(rResult, rResult);
				}
			}
			vrrMinMax[iCol-1] = rrReturn; // copy for eventual re-use
		}
	}

	return rrReturn;
}

void MapEffect::WriteElements(const char* sSection, const ElementContainer& en)
{
	Effect::WriteElements(sSection, en);

	int iSize = vMaps.size();
	ObjectInfo::WriteElement(sSection, "NrInputMaps", en, iSize);
	for (int i=0; i<iSize; ++i)
		ObjectInfo::WriteElement(sSection, String("InputMap%d", i).scVal(), en, vMaps[i].sRelativeQuoted());
	// although WriteElement does work with a filename, we also want to preserve sCol. Here sRelativeQuoted is used for consistency, but below we have no choice.
	if (m_pavos)
	{
		ObjectInfo::WriteElement(sSection, "AggregateValueOperation", en, m_pavos->sOperation);
		ObjectInfo::WriteElement(sSection, "AggregateValueAdditionalFilename", en, m_pavos->fnAdditional);
		ObjectInfo::WriteElement(sSection, "AggregateValuePower", en, m_pavos->rPower);
		ObjectInfo::WriteElement(sSection, "AggregateValueExpression", en, m_pavos->sBooleanExpression);
	}
}

void MapEffect::ReadElements(const char* sSection, const ElementContainer& en)
{
	// First read aggregation properties so that the output type is clear
	m_pavos = new AggregateValueOperationStruct;
	bool fAggregate =	(0 != ObjectInfo::ReadElement(sSection, "AggregateValueOperation", en, m_pavos->sOperation));
	if (fAggregate)
	{
		ObjectInfo::ReadElement(sSection, "AggregateValueAdditionalFilename", en, m_pavos->fnAdditional);
		ObjectInfo::ReadElement(sSection, "AggregateValuePower", en, m_pavos->rPower);
		ObjectInfo::ReadElement(sSection, "AggregateValueExpression", en, m_pavos->sBooleanExpression);
	}
	else
	{
		delete m_pavos;
		m_pavos = 0;
	}

	// Then continue with the rest of the member variables

	int iSize;
	if (!ObjectInfo::ReadElement(sSection, "NrInputMaps", en, iSize))
		iSize = 1;
	vMaps.resize(iSize);
	vrrMinMax.resize(iSize); // for consistency .. unless we decide to do SetNrAlternatives in advance from a higher level
	ResizeAggregateValues(iSize);
	for (int i=0; i<iSize; ++i)
	{
		String sMapFileName; // this is a workaround -- sCol is lost when reading a FileName
		ObjectInfo::ReadElement(sSection, String("InputMap%d", i).scVal(), en, sMapFileName);
		vMaps[i] = sMapFileName;
	}

	// And then perform the Effect::ReadElements that sets the std based on above properties

	Effect::ReadElements(sSection, en);
}

int MapEffect::iIconIndex()
{
	if (fSpatialItem())
	{
		if (eCtiType == iFACTOR)
			return iMAPEFFECT;
		else // constraint
			return iMAPCONSTRAINT;
	}
	else
	{
		if (eCtiType == iFACTOR)
			return iAGGREGATEDMAPEFFECT;
		else // constraint
			return iAGGREGATEDMAPCONSTRAINT;
	}
}

String MapEffect::sStatusBarText()
{
	return String("Map %S", Effect::sStatusBarText());
}

void MapEffect::GetObjectStructure(ObjectStructure& os)
{
	int iSize = vMaps.size();
	for (int i=0; i<iSize; ++i)
	{
		os.AddFile(vMaps[i]); // sCol is lost due to FileName design (we want this!)
		// Retrieve the files belonging to the map (georef, tables, domains)
		if (IlwisObject::iotObjectType(vMaps[i]) != IlwisObject::iotANY)
		{
			IlwisObject obj = IlwisObject::obj(vMaps[i]);
			if ( obj.fValid())
				obj->GetObjectStructure(os);
		}					
	}
	for (int i=1; i<=iSize; ++i)
	{
		String sStandardizedMapName ("%S_standardized", vMaps[i-1].sShortName(false));
		FileName fnStandardizedMapName (sStandardizedMapName.sQuote(), ".mpr");
		if (fnStandardizedMapName.fExist())
		{
			os.AddFile(fnStandardizedMapName);
			// Retrieve the files belonging to the map (georef, tables, domains)
			if (IlwisObject::iotObjectType(fnStandardizedMapName) != IlwisObject::iotANY)
			{
				IlwisObject obj = IlwisObject::obj(fnStandardizedMapName);
				if ( obj.fValid())
					obj->GetObjectStructure(os);
			}
		}
	}
}

bool MapEffect::fSpatialItem()
{
	return (m_pavos == 0);
}

class FormAggregateValueFunc; // forward

struct CalcThreadStruct
{
	CalcThreadStruct() {};
	AggregateValueOperationStruct	avos;
	FormAggregateValueFunc *form;
	bool fRunning; // when true, the thread is (still) busy
	bool fAbort; // when this is set on true, thread should quit (return) as soon as possible
};

class FormAggregateValueFunc : public FormWithDest, public AggregateValueAdditional
{
public:
	FormAggregateValueFunc(CWnd *wPar, AggregateValueOperationStruct * pavos, const dmType fd, vector <AttributeFileName> & arrSelected)
	 : FormWithDest(wPar, SAFTitleAggrValueS)
	, m_pavos(pavos)
	, m_pcts(0)
	{
		rPower = pavos->rPower;
		sMapName = pavos->fnAdditional.sRelativeQuoted();
		sBooleanExpression = pavos->sBooleanExpression;
		if (sMapName.length() > 0)
		{
			fAdditional = true;
			if (pavos->fnAdditional.fExist())
			{
				bool *fDoNotShowError = (bool *)(IlwWinApp()->Context()->pGetThreadLocalVar(IlwisAppContext::tlvDONOTSHOWFINDERROR));
				bool fDoNotShowErrorPrevious = *fDoNotShowError;
				*fDoNotShowError = true; // don't allow Map to Show a "Find error: Georef.grf".
				try
				{
					Map mp (pavos->fnAdditional);
					if (mp.fValid())
						iBooleanChoice = (mp->dm()->pdbool()) ? 0 : 1;
				}
				catch (ErrorObject&)
				{
				}
				*fDoNotShowError = fDoNotShowErrorPrevious;
			}
		}
		else if (sBooleanExpression.length() > 0)
		{
			fAdditional = true;
			iBooleanChoice = 2;
		}

		favf = new FieldAggregateValueFunc(root, SAFUiOperation, &(m_pavos->sOperation), fd, m_pavos->sOperation);
		favf->SetCallBack((NotifyProc)&FormAggregateValueFunc::HandleOperationChanges);
		SetAdditionalBlock(root, favf, false);
		
		pbCalc = new PushButton(root, SSmcUiAggregateValPreview, (NotifyProc) &FormAggregateValueFunc::CalculateAggregateValues);
		pbCalc->Align(rgBoolean, AL_UNDER);

		ossSelected = new AggregateValueResultSelector(root, arrSelected);
		ossSelected->SetIndependentPos();

		create();
	}

	~FormAggregateValueFunc()
	{
		if (m_pcts)
		{
			if (m_pcts->fRunning)
			{
				m_pcts->fAbort = true;
				while (m_pcts->fRunning)
				{
					Sleep(200);
					 MSG msg;
					 while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
						 ::DispatchMessage(&msg);
				}
			}

			delete m_pcts;
			m_pcts = 0;
		}
	}

	void OnCancel()
	{
		if (m_pcts)
		{
			if (m_pcts->fRunning)
			{
				m_pcts->fAbort = true;
				while (m_pcts->fRunning)
				{
					Sleep(200);
					 MSG msg;
					 while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
						 ::DispatchMessage(&msg);
				}
			}
		}
		FormWithDest::OnCancel();
	}

	int HandleOperationChanges(Event *)
	{
		favf->StoreData();
		//cbAdditional->StoreData();	
		frPower->Hide();
		// rgBoolean->Hide();
		cbAdditional->Hide();

		if ( fCIStrEqual(m_pavos->sOperation, "connectivityindex") )
			frPower->Show();
		else if (!fCIStrEqual(m_pavos->sOperation, "shapeindex"))
			cbAdditional->Show();		
		return 1;
	}

	int CalculateAggregateValues(Event *)
	{
		FormEntry *pfe = CheckData();
		if (pfe) 
		{
			MessageBeep(MB_ICONEXCLAMATION);
			pfe->SetFocus();
			return 1;
		}

		exec(); // to get a good m_pavos
		DisableOK();
		if (m_pcts == 0)
			m_pcts = new CalcThreadStruct;
		m_pcts->avos = *m_pavos;
		m_pcts->form = this; // (FormAggregateValueMulti *)frm();
		m_pcts->fRunning = true;
		m_pcts->fAbort = false;
		AfxBeginThread(CalcExpression, (LPVOID)m_pcts);
		return 1;
	}

	// this worker thread function does the actual calculation. It does them in order and not all at once
	static UINT CalcExpression(LPVOID pParms)
	{
		CalcThreadStruct *pcts = (CalcThreadStruct *)pParms;
		try
		{
			IlwWinApp()->Context()->InitThreadLocalVars();
			CalculateAggregateValue	cavCalcObject(pcts->avos);	
			int iN = pcts->form->ossSelected->iGetCount();
			String sResult;
			pcts->form->pbCalc->disable();

			int iMaxLength = 0;
			
			for(int i=0; i < iN; ++i)
			{
				if (pcts->fAbort)
					break;
				SelectedFMOItem *avs = (SelectedFMOItem *)pcts->form->ossSelected->fiGetItem(i);
				AttributeFileName fnInputMap = avs->fnItem();
				String sCol = avs->sGetAttributeColumn();
				String sResult = cavCalcObject.sCalculateAggregateValue(fnInputMap, sCol);
				SelectedFMOItem *item = (SelectedFMOItem*)pcts->form->ossSelected->fiGetItem(i)->Clone();
				item->SetAggregateResult(sResult);
				pcts->form->ossSelected->ChangeItem(i, item);		
				
			}
			pcts->form->pbCalc->enable();	
			pcts->form->EnableOK();
			IlwWinApp()->Context()->RemoveThreadLocalVars();
			pcts->fRunning = false;
		}
		catch(ErrorObject&)
		{
			pcts->form->pbCalc->enable();	
			pcts->form->EnableOK();
			IlwWinApp()->Context()->RemoveThreadLocalVars();
			pcts->fRunning = false;
		}
		catch (...)
		{
			try // our last attempt to enable the buttons
			{
				pcts->form->pbCalc->enable();	
				pcts->form->EnableOK();
				IlwWinApp()->Context()->RemoveThreadLocalVars();
				pcts->fRunning = false;
			}
			catch (...)
			{
			}
		}
		return 1;
	}

	int exec()
	{
		frPower->StoreData();
		cbAdditional->StoreData();
		rgBoolean->StoreData();
		fmMapWeight->StoreData();
		fmMapBoolean->StoreData();
		fsExpression->StoreData();

		// Reset these to prevent an avos that fails in CalculateAggregateValue::sBuildExpression
		m_pavos->fnAdditional = FileName();
		m_pavos->sBooleanExpression = "";
		
		if (fCIStrEqual(m_pavos->sOperation, "connectivityindex")) 	
			m_pavos->rPower = rPower;
		else if ( fAdditional )
		{
			switch (iBooleanChoice)
			{
			case 0:
			case 1:
				if (sMapName != "")
					m_pavos->fnAdditional = FileName(sMapName);
				break;
			case 2:
				m_pavos->sBooleanExpression = sBooleanExpression;
				break;
			}
		}
	
		return 1;
	}
private:
	AggregateValueOperationStruct * m_pavos;
	PushButton* pbCalc;
	AggregateValueResultSelector* ossSelected;
	CalcThreadStruct* m_pcts;
};

void MapEffect::AggregateValues()
{
	AggregateValueOperationStruct* pavos = new AggregateValueOperationStruct();
	if (m_pavos)
		*pavos = *m_pavos;

	vector <AttributeFileName> arrSelected;
	for (unsigned int i=0; i< vMaps.size(); ++i)
	{
		arrSelected.push_back(vMaps[i]);
	}
	FormAggregateValueFunc frm (GetDocument()->wndGetActiveView(), pavos, (dmType)dmTypes(), arrSelected);
	if (frm.fOkClicked())
	{
		for (unsigned int i=0; i<vMaps.size(); ++i)
		{
			vrrMinMax[i] = RangeReal();
			vAggregateValues[i] = rUNDEF;
		}

		RecursivelyDeleteOutputMaps(); // delete the output maps while we are still considered "spatial"
		SetAVOS(pavos);
		SetStandardization(Standardization::create(this));
		SetModifiedFlag();
		GetDocument()->UpdateAllViews(0, eITEMUPDATED, this); // also changes the icon
	}
	else
		delete pavos;
}

void MapEffect::DoNotAggregateValues()
{
	if (m_pavos != 0) // do not bother if the user does this operation for the second time
	{
		if (IDYES == MessageBox(GetDocument()->wndGetActiveView()->GetSafeHwnd(), SSmcUiConfirmCancelAggregation.scVal(), SSmcUiConfirmation.scVal(), MB_YESNO|MB_ICONQUESTION|MB_DEFBUTTON1))
		{
			for (unsigned int i=0; i<vMaps.size(); ++i)
			{
				vrrMinMax[i] = RangeReal();
				vAggregateValues[i] = rUNDEF;
			}

			SetAVOS(0);
			SetStandardization(Standardization::create(this));
			SetModifiedFlag();
			RecursivelyDeleteOutputMaps();
			GetDocument()->UpdateAllViews(0, eITEMUPDATED, this); // also changes the icon
		}
	}
}

void MapEffect::SetAVOS(AggregateValueOperationStruct* pavos)
{
	bool fSpatialChanged = ((m_pavos == 0) && (pavos != 0)) || ((m_pavos != 0) && (pavos == 0)); // XOR
	if (m_pavos)
		delete m_pavos;
	m_pavos = pavos;
	if (fSpatialChanged)
		GetDocument()->UpdateAllViews(0, eITEMUPDATED, this); // to change the icon
}

Effect::eValueTP MapEffect::eOutputValue()
{
	if (fSpatialItem())
		return eInputValue();
	else
		return iVALUE;
}

double MapEffect::rAggregateValue(int iCol)
{
	if (iCol > 0)
	{
		if (vAggregateValues[iCol-1] != rUNDEF)
			return vAggregateValues[iCol-1];
		else
		{
			String sResult = "";

			bool *fDoNotShowError = (bool *)(IlwWinApp()->Context()->pGetThreadLocalVar(IlwisAppContext::tlvDONOTSHOWFINDERROR));
			bool fDoNotShowErrorPrevious = *fDoNotShowError;
			*fDoNotShowError = true; // don't allow Map to Show a "Find error: Georef.grf".
			try
			{
				CalculateAggregateValue cav(*m_pavos);
				// remove col (if any) coz fExists tries to look for the wrong file
				FileName fn (vMaps[iCol-1]); // copy forgets sCol
				sResult = cav.sCalculateAggregateValue(fn, vMaps[iCol-1].sCol);
			}
			catch(ErrorObject&)
			{
			}
			*fDoNotShowError = fDoNotShowErrorPrevious;
			
			double rResult = rUNDEF;
			if (sResult.length() > 0)
			{
				rResult = sResult.rVal();
				vAggregateValues[iCol-1] = rResult; // store for next time
			}
			return rResult;
		}
	}
	else
		return 0;
}

void MapEffect::ResizeAggregateValues(int iNr)
{
	int iOldNr = vAggregateValues.size();
	vAggregateValues.resize(iNr);
	for (int i=iOldNr; i<iNr; ++i)
		vAggregateValues[i] = rUNDEF;
}

void MapEffect::AddContextMenuOptions(CMenu& men)
{
	if (GetDocument())
	{
		add(ID_CT_MAPAGGREGATION);
		add(ID_CT_NOMAPAGGREGATION);
		addBreak;
		bool fModeOK = GetDocument()->GetMode() == CriteriaTreeDoc::eSTDWEIGH;
		men.EnableMenuItem(ID_CT_MAPAGGREGATION, fModeOK ? MF_ENABLED : MF_GRAYED);
		men.EnableMenuItem(ID_CT_NOMAPAGGREGATION, fModeOK ? MF_ENABLED : MF_GRAYED);
		if (fSpatialItem())
			men.CheckMenuRadioItem(ID_CT_NOMAPAGGREGATION, ID_CT_NOMAPAGGREGATION, ID_CT_NOMAPAGGREGATION, MF_BYCOMMAND);
		else
			men.CheckMenuRadioItem(ID_CT_MAPAGGREGATION, ID_CT_MAPAGGREGATION, ID_CT_MAPAGGREGATION, MF_BYCOMMAND);
	}

	Effect::AddContextMenuOptions(men); // Standardize ...
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

ScoreEffect::ScoreEffect(EffectGroup* egParent, CString sName, eValueTP eIV, FileName fnDomain, eCtiTypeTP eCT)
: Effect(egParent, sName, eIV, fnDomain, eCT)
{
}

ScoreEffect::ScoreEffect(CriteriaTreeItem* ctiCopy)
: Effect(ctiCopy)
{
	SetNrAlternatives(ctiCopy->iGetNrAlternatives());
}

ScoreEffect::~ScoreEffect()
{

}

void ScoreEffect::SetNrAlternatives(int iNr)
{
	int iOldSize = vScores.size();
	vScores.resize(iNr);
	for (int i = iOldSize; i < iNr; ++i)
		vScores[i] = rUNDEF;
}

CString ScoreEffect::sDisplayText(int iCol)
{
	if (iCol>0)
	{
		if (vScores[iCol-1] != rUNDEF)
			return String("%lg", vScores[iCol-1]).scVal();
		else
			return "";
	}
	else
	{
		String sCostBenefit;
		switch(iCostBenefit())
		{
		case 0:
			sCostBenefit = ":benefit";
			break;
		case 1:
			sCostBenefit = ":cost";
			break;
		case 2:
			sCostBenefit = ":none";
			break;
		default:
			sCostBenefit = ":none";
			break;
		}

		return String("%S%S%S", CriteriaTreeItem::sDisplayText(), sStandardizationMethod(), sCostBenefit).scVal();
	}
}

void ScoreEffect::Edit(int iCol, RECT rect, CWnd* wnd)
{
	if (iCol > 0)
	{
		String sScore = "";
		if (vScores[iCol-1] != rUNDEF)
			sScore = String("%lg", vScores[iCol-1]);

		DWORD style = WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL;
		m_e = new InPlaceEdit((int)this, iCol, sScore.scVal());
		m_e->Create(style, rect, wnd, ID_CRITERIA_TREE_EDIT);
		// (int)this :: misuse of the m_iItem argument for passing a pointer
		// the pointer will be returned by InPlaceEdit::SendEndLabelNotify()
	}
	else
	{
		bool fModeIsEdit = false;
		if (GetDocument())
			fModeIsEdit = GetDocument()->GetMode() == CriteriaTreeDoc::eEDITTREE;
		if (fModeIsEdit)
			EditTreeElement();
		else
			OnStandardize();
	}
}

void ScoreEffect::Delete(int iCol)
{
	if (iCol>0)
	{
		vScores[iCol-1] = rUNDEF;
		GetDocument()->UpdateAllViews(0, eLABELCHANGED, this);
		SetModifiedFlag();
		RecursivelyDeleteOutputMaps();
	}
	else
	{
		bool fModeIsEdit = false;
		if (GetDocument())
			fModeIsEdit = GetDocument()->GetMode() == CriteriaTreeDoc::eEDITTREE;
		if (fModeIsEdit)
			DeleteTreeElement();
	}
}

void ScoreEffect::ClearData()
{
	for (unsigned int i=0; i<vScores.size(); ++i)
		vScores[i] = rUNDEF;
}

void ScoreEffect::SetData(int iCol, String sData)
{
	if (iCol>0 && sData != "")
	{
		if (sData.rVal() != vScores[iCol-1])
		{
			vScores[iCol-1] = sData.rVal();
			SetModifiedFlag();
			RecursivelyDeleteOutputMaps();
		}
	}
}

void ScoreEffect::RefreshInputType()
{
	if (eInputValue() == iUNKNOWN)
	{
		SetInputValueTP(iVALUE);
		SetStandardization(Standardization::create(this));
	} // "else": leave as it is
}

Effect::eValueTP ScoreEffect::eOutputValue()
{
	return eInputValue();
}

String ScoreEffect::sTerm(int iCol)
{
	if ((iCol > 0) && ptrStandardization() && ptrStandardization()->fStandardized())
		return ptrStandardization()->sStandardize(String("%lg", vScores[iCol-1]));
	else
		return "";
}

String ScoreEffect::sInputData(int iCol)
{
	return ""; // dummy implementation
}

bool ScoreEffect::fDone(int iCol, bool fRecursive)
{
	// only check the following if mode is std/weigh
	// for iCol>0 check if the selected input is entered
	// for iCol==0 check if the thing is standardized
	
	// NOTE: Decide on the fRecursive flag if we should check on the availability of all input data
	bool fOK = true;

	if (iCol > 0)
	{
		fOK = (vScores[iCol-1] != rUNDEF);
	}
	else // iCol == 0
	{
		bool fModeIsEdit = false;
		if (GetDocument())
			fModeIsEdit = GetDocument()->GetMode() == CriteriaTreeDoc::eEDITTREE;
		if (fModeIsEdit)
		{
			if (fRecursive) // means we're doing a final check to see if we can change the mode
			{
				unsigned int i=1;
				while (fOK && (i<=vScores.size()))
				{
					fOK = fOK && fDone(i, false); // not recursive!!
					++i;
				}
			}
		}
		else // fModeIsEdit == false
			fOK = (ptrStandardization() && ptrStandardization()->fStandardized());
	}
	return fOK;
}

bool ScoreEffect::fCalculationPossible(int iCol, bool fFirst)
{
	if (iCol > 0)
	{
		return (ptrStandardization() && ptrStandardization()->fStandardized() && (vScores[iCol-1] != rUNDEF));
	}
	else
		return false; // shouldn't happen
}

bool ScoreEffect::fShowStandardizedPossible(int iCol)
{
	if (iCol > 0)
		return fCalculationPossible(iCol, true);
	else // return true if it is at least possible to calculate one alternative
	{
		int i=1;
		int iNrInputScores = vScores.size();
		while ((i <= iNrInputScores) && (!fCalculationPossible(i, true)))
			++i;
		return (i <= iNrInputScores); // if true, at least one "possible to calculate" was found
	}
}

void ScoreEffect::ShowStandardized(int iCol, bool* pfInitRpr)
{
	int i = (iCol > 0) ? iCol : 1;
	int iNrScores = vScores.size();
	String sMessage;
	while (i <= iNrScores)
	{
		if (fCalculationPossible(i, true))
		{
			try
			{
				String sExpr = sTerm(i);
				String sResult = Calculator::sSimpleCalc(sExpr);
				if (sMessage.length() > 0)
					sMessage += "\n";
				if (iGetNrAlternatives() > 1)
					sMessage += String("%s: %S    (expression = %S)", GetDocument()->sAlternative(i), sResult, sExpr);
				else
					sMessage += "Result = " + sResult + "    (expression = " + sExpr + ")";
			}
			catch (ErrorObject& err)
			{
				err.Show();
			}
		}
		i += (iCol > 0) ? iNrScores : 1; // jump out of "while loop" if the iCol had a value > 0
	}
	if (sMessage.length() > 0)
		MessageBox(0, sMessage.scVal(), String("Standardized value%s for criterion '%S'", (iGetNrAlternatives() > 1) ? "s" : "", sName()).scVal(), MB_OK);
}

String ScoreEffect::sInputMinMax(int iCol, bool fMax)
{
	return String ("%lg", vScores[iCol-1]).scVal(); // ignoring max which is meant for maps
}

RangeReal ScoreEffect::rrMinMax(int iCol)
{
	RangeReal rrReturn = RangeReal();

	if (iCol>0)
		rrReturn = RangeReal(vScores[iCol-1], vScores[iCol-1]); // same value for min and max

	return rrReturn;
}

void ScoreEffect::WriteElements(const char* sSection, const ElementContainer& en)
{
	Effect::WriteElements(sSection, en);

	int iSize = vScores.size();
	ObjectInfo::WriteElement(sSection, "NrInputScores", en, iSize);
	for (int i=0; i<iSize; ++i)
		ObjectInfo::WriteElement(sSection, String("InputScore%d", i).scVal(), en, vScores[i]);
}

void ScoreEffect::ReadElements(const char* sSection, const ElementContainer& en)
{
	int iSize;
	if (!ObjectInfo::ReadElement(sSection, "NrInputScores", en, iSize))
		iSize = 1;
	vScores.resize(iSize);
	for (int i=0; i<iSize; ++i)
		ObjectInfo::ReadElement(sSection, String("InputScore%d", i).scVal(), en, vScores[i]);

	Effect::ReadElements(sSection, en);
}

int ScoreEffect::iIconIndex()
{
	if (eCtiType == iFACTOR)
		return iSCOREEFFECT;
	else // constraint
		return iSCORECONSTRAINT;
}

String ScoreEffect::sStatusBarText()
{
	return String("Score %S", Effect::sStatusBarText());
}

bool ScoreEffect::fSpatialItem()
{
	return false;
}
