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
// DomainSortDoc.cpp: implementation of the DomainSortDoc class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Client\ilwis.h"
#include "Client\Editors\Domain\classItem.h"
#include "Engine\Domain\dmsort.h"
#include "Engine\Domain\dmgroup.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\Editors\Domain\DomDoc.h"
#include "Client\Editors\Domain\DomainSortDoc.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\Base\BaseView.h"
#include "Client\Base\ButtonBar.h"
#include "Client\Editors\Utils\OwnerHeaderCtrl.h"
#include "Client\Editors\Domain\DomainView.h"
#include "Engine\Base\AssertD.h"
#include "Engine\Base\DataObjects\ERR.H"
#include "Engine\Base\DataObjects\Tranq.h"
#include "Headers\Hs\DOMAIN.hs"
#include "Headers\constant.h"
#include "Engine\Base\File\COPIER.H"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DomainSortDoc

IMPLEMENT_DYNCREATE(DomainSortDoc, DomainDoc)

DomainSortDoc::DomainSortDoc()
{
}

DomainSortDoc::~DomainSortDoc()
{
}


BEGIN_MESSAGE_MAP(DomainSortDoc, DomainDoc)
	//{{AFX_MSG_MAP(DomainSortDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	ON_COMMAND(ID_DOMRPR,           OnRepresentation)
	ON_UPDATE_COMMAND_UI(ID_DOMRPR,	OnUpdateRepresentation)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// DomainSortDoc diagnostics

#ifdef _DEBUG
void DomainSortDoc::AssertValid() const
{
	DomainDoc::AssertValid();
}

void DomainSortDoc::Dump(CDumpContext& dc) const
{
	DomainDoc::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// DomainSortDoc commands

BOOL DomainSortDoc::OnNewDocument()
{
  // not allowed to create an empty DomainSortDoc
  return FALSE;
/*
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
*/
}

BOOL DomainSortDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	if (!DomainDoc::OnOpenDocument(lpszPathName))
		return FALSE;

    ISTRUE(fINotEqual, dom->pdsrt(), (DomainSort*)0);   // only DomainSort type domain allowed

    if (!dom.fValid())
        return FALSE;
//    SetTitle(dom->sDescription.sVal());
    return TRUE;
}

void DomainSortDoc::OnRepresentation()
{
	try
	{
		Representation rpr = dm()->rpr();
		IlwWinApp()->OpenDocumentAsRpr(rpr->fnObj.sFullNameQuoted().c_str());
	}
	catch (const ErrorObject& err)
	{
		err.Show();
	}
}

void DomainSortDoc::OnUpdateRepresentation(CCmdUI* pCmdUI)
{
	bool *fDoNotShowError = (bool *)(IlwWinApp()->Context()->pGetThreadLocalVar(IlwisAppContext::tlvDONOTSHOWFINDERROR));
	*fDoNotShowError = true;

	pCmdUI->Enable(!fIsID() && dm()->rpr().fValid());	// DomainID does not have representation or rpr may be invalid

	*fDoNotShowError = false;
}

void DomainSortDoc::StoreWidth(const string& sSection, int iWid)
{
	dm()->WriteElement(sSection.c_str(), "Width", (long)iWid);
}

int DomainSortDoc::RestoreWidth(const string& sSection)
{
	int iWid = iUNDEF;
	dm()->ReadElement(sSection.c_str(), "Width", (long&)iWid);
	if (iWid != iUNDEF)
		return iWid;
	else
		return 10;	// width is in characters
}

// Interface function to the DomainSort
String DomainSortDoc::sCode(long iKey)
{
    DomainSort* pds = dom->pdsrt();
    ISTRUE(fINotEqual, pds, (DomainSort*)0);

    return pds->sCodeByRaw(pds->iKey(iKey), 0);
}

String DomainSortDoc::sName(long iKey)
{
    DomainSort* pds = dom->pdsrt();
    ISTRUE(fINotEqual, pds, (DomainSort*)0);

    return pds->sNameByRaw(pds->iKey(iKey), 0);
}

String DomainSortDoc::sDescription(long iKey)
{
    DomainSort* pds = dom->pdsrt();
    ISTRUE(fINotEqual, pds, (DomainSort*)0);

	return pds->sDescriptionByRaw(pds->iKey(iKey));
}

String DomainSortDoc::sPrefix()
{
    DomainSort* pds = dom->pdsrt();
    ISTRUE(fINotEqual, pds, (DomainSort*)0);

	return pds->sGetPrefix();
}

void DomainSortDoc::SetName(long iKey, const String& sName)
{
    DomainSort* pds = dom->pdsrt();
    ISTRUE(fINotEqual, pds, (DomainSort*)0);

	pds->SetVal(pds->iKey(iKey), sName);

	UpdateAllViews(NULL, 0, NULL);
}

void DomainSortDoc::SetCode(long iKey, const String& sCode)
{
    DomainSort* pds = dom->pdsrt();
    ISTRUE(fINotEqual, pds, (DomainSort*)0);

	pds->SetCode(pds->iKey(iKey), sCode);

	UpdateAllViews(NULL, 0, NULL);
}

void DomainSortDoc::SetDescription(long iKey, const String& sDescr)
{
    DomainSort* pds = dom->pdsrt();
    ISTRUE(fINotEqual, pds, (DomainSort*)0);

	pds->SetDescription(pds->iKey(iKey), sDescr);

	UpdateAllViews(NULL, 0, NULL);
}

void DomainSortDoc::SetPrefix(const String& sNewPrefix)
{
	DomainSort* pds = dom->pdsrt();
	ISTRUE(fINotEqual, pds, (DomainSort*)0);

	Tranquilizer trq;
	trq.Start();

	// Temporarily set the sorting to manual to prevent
	// unnecessary sorting
	DomainSort::DomainSortingType dst = pds->dsType;
	pds->dsType = DomainSort::dsMANUAL;

	pds->SetPrefix(sNewPrefix, &trq);

	pds->dsType = dst;
	if (dst != DomainSort::dsMANUAL)
		Sort();

	UpdateAllViews(NULL, 0, NULL);

	trq.Stop();
}

long DomainSortDoc::iCount()
{
    DomainSort* pds = dom->pdsrt();
    ISTRUE(fINotEqual, pds, (DomainSort*)0);

    return pds->iNettoSize();
}

bool DomainSortDoc::fIsClass()
{
	return dom->pdc() != 0;
}

bool DomainSortDoc::fIsID()
{
	return dom->pdid() != 0;
}

bool DomainSortDoc::fIsGroup()
{
	return dom->pdgrp() != 0;
}

long DomainSortDoc::iMergeAddItem(const ClassItem& cli)
{
	if (cli.sName().length() ==0)
		return iUNDEF;

	DomainSort* pds = dom->pdsrt();
	ISTRUE(fINotEqual, pds, (DomainSort*)0);

	long id = pds->iMergeAdd(cli.sName(), cli.sCode());
	if (id == iUNDEF)
		return iUNDEF;

	pds->SetDescription(id, cli.sDescription());
	pds->Updated();
	id = pds->iOrd(cli.sName());
	UpdateAllViews(NULL, 0, NULL);

	return id;
}

long DomainSortDoc::iAddItem(const ClassItem& cli)
{
	if (cli.sName().length() ==0)
		return iUNDEF;

    DomainSort* pds = dom->pdsrt();
    ISTRUE(fINotEqual, pds, (DomainSort*)0);

	long id = pds->iAdd(cli.sName());
	if (cli.sCode().length() != 0)
		pds->SetCode(id, cli.sCode());
	pds->SetDescription(id, cli.sDescription());
	pds->Updated();
	UpdateAllViews(NULL, 0, NULL);

	return pds->iOrd(id);
}

void DomainSortDoc::MoveItem(long iKeyTo, long iKeyFrom)
{
    DomainSort* pds = dom->pdsrt();
    ISTRUE(fINotEqual, pds, (DomainSort*)0);

//	long iItemTo = pds->iKey(iKeyTo);
	long iItemFrom = pds->iKey(iKeyFrom);
	pds->SetOrd(iItemFrom, iKeyTo);

	UpdateAllViews(NULL, 0, NULL);
}

void DomainSortDoc::SetItem(long iKey, const ClassItem &cli)
{
    DomainSort* pds = dom->pdsrt();
    ISTRUE(fINotEqual, pds, (DomainSort*)0);

	long iItem = pds->iKey(iKey);
	pds->SetVal(iItem, cli.sName());
	pds->SetCode(iItem, cli.sCode());
	pds->SetDescription(iItem, cli.sDescription());

	UpdateAllViews(NULL, 0, NULL);
}

void DomainSortDoc::DeleteItem(const ClassItem &cli)
{
    DomainSort* pds = dom->pdsrt();
    ISTRUE(fINotEqual, pds, (DomainSort*)0);

	long iRaw = pds->iRaw(cli.sName());
	if (iRaw < 0)
		return;
	pds->Delete(iRaw);
	UpdateAllViews(NULL, 0, NULL);
}

void DomainSortDoc::GetItem(long iKey, ClassItem &cli)
{
    DomainSort* pds = dom->pdsrt();
    ISTRUE(fINotEqual, pds, (DomainSort*)0);

	long iItem = pds->iKey(iKey);
	cli.SetName(pds->sNameByRaw(iItem, 0));
	cli.SetCode(pds->sCodeByRaw(iItem, 0));
	cli.SetDescription(pds->sDescriptionByRaw(iItem));
}

void DomainSortDoc::AddFromDomain(const String &sDomain)
{
	DomainSort* pds = dom->pdsrt();
	ISTRUE(fINotEqual, pds, (DomainSort*)0);

	Domain dmMerge(sDomain);
	ISTRUE(fINotEqual, dmMerge->pdsrt(), (DomainSort*)0);

	if (dmMerge->fnObj == pds->fnObj)
		throw SelfMergeError(sDomain);
	else 
	{
		Tranquilizer trq;
		trq.Start();
		pds->Merge(dmMerge->pdsrt(), &trq);
		trq.Stop();
		UpdateAllViews(NULL, 0, NULL);
	}
}

bool DomainSortDoc::fAlphaSorted()
{
    DomainSort* pds = dom->pdsrt();
    ISTRUE(fINotEqual, pds, (DomainSort*)0);

	return pds->dsType == DomainSort::dsALPHA;
}

bool DomainSortDoc::fAlphaNumSorted()
{
    DomainSort* pds = dom->pdsrt();
    ISTRUE(fINotEqual, pds, (DomainSort*)0);

	return pds->dsType == DomainSort::dsALPHANUM;
}

bool DomainSortDoc::fCodeAlphabetic()
{
    DomainSort* pds = dom->pdsrt();
    ISTRUE(fINotEqual, pds, (DomainSort*)0);

	return pds->dsType == DomainSort::dsCODEALPHA;
}

bool DomainSortDoc::fCodeAlphaNumeric()
{
    DomainSort* pds = dom->pdsrt();
    ISTRUE(fINotEqual, pds, (DomainSort*)0);

	return pds->dsType == DomainSort::dsCODEALNUM;
}

bool DomainSortDoc::fManualSorted()
{
    DomainSort* pds = dom->pdsrt();
    ISTRUE(fINotEqual, pds, (DomainSort*)0);

	return pds->dsType == DomainSort::dsMANUAL;
}

long DomainSortDoc::iGetItemIndexOf(const ClassItem &cli)
{
    DomainSort* pds = dom->pdsrt();
    ISTRUE(fINotEqual, pds, (DomainSort*)0);

	long iRaw = pds->iRaw(cli.sName());
	if (iRaw != iUNDEF)
		return pds->iOrd(iRaw);
	else
		return 0;
}

void DomainSortDoc::SetSortAlphabetic()
{
    DomainSort* pds = dom->pdsrt();
    ISTRUE(fINotEqual, pds, (DomainSort*)0);

	pds->dsType = DomainSort::dsALPHA;
}

void DomainSortDoc::SetSortManual()
{
    DomainSort* pds = dom->pdsrt();
    ISTRUE(fINotEqual, pds, (DomainSort*)0);

	pds->dsType = DomainSort::dsMANUAL;
}

void DomainSortDoc::SetSortAlphaNum()
{
    DomainSort* pds = dom->pdsrt();
    ISTRUE(fINotEqual, pds, (DomainSort*)0);

	pds->dsType = DomainSort::dsALPHANUM;
}

void DomainSortDoc::SetSortCodeAlpha()
{
    DomainSort* pds = dom->pdsrt();
    ISTRUE(fINotEqual, pds, (DomainSort*)0);

	pds->dsType = DomainSort::dsCODEALPHA;
}

void DomainSortDoc::SetSortCodeAlphaNum()
{
    DomainSort* pds = dom->pdsrt();
    ISTRUE(fINotEqual, pds, (DomainSort*)0);

	pds->dsType = DomainSort::dsCODEALNUM;
}

void DomainSortDoc::Sort()
{
    DomainSort* pds = dom->pdsrt();
    ISTRUE(fINotEqual, pds, (DomainSort*)0);

	if (pds->dsType == DomainSort::dsALPHA)
		pds->SortAlphabetical();
	else if (pds->dsType == DomainSort::dsALPHANUM)
		pds->SortAlphaNumeric();
	else if (pds->dsType == DomainSort::dsCODEALPHA)
		pds->SortCodeAlphabetical();
	else if (pds->dsType == DomainSort::dsCODEALNUM)
		pds->SortCodeAlphaNumeric();

	UpdateAllViews(NULL, 0, NULL);
}

long DomainSortDoc::iFindName(const String& sName)
{
	DomainSort* pds = dom->pdsrt();
	ISTRUE(fINotEqual, pds, (DomainSort*)0);

	return pds->iOrd(pds->iFindNameRaw(sName));
}

long DomainSortDoc::iFindCode(const String& sCode)
{
	DomainSort* pds = dom->pdsrt();
	ISTRUE(fINotEqual, pds, (DomainSort*)0);

	return pds->iOrd(pds->iFindCodeRaw(sCode));
}

bool DomainSortDoc::fCanAdd(const ClassItem &cli)
{
    DomainSort* pds = dom->pdsrt();
    ISTRUE(fINotEqual, pds, (DomainSort*)0);

	if (pds->iOrd(cli.sName()) != iUNDEF)
		return false;
	if (cli.sCode().length() > 0 && pds->iOrd(cli.sCode()) != iUNDEF)
		return false;
	return true;
}

String DomainSortDoc::sMakeClipLine(const ClassItem &cli)
{
	return String(FORMAT_CLASSID, cli.sName(), cli.sCode(), cli.sDescription());
}

void DomainSortDoc::MakeItemPart(DomainSortFields dsfField, ClassItem &cli, const string &sVal)
{
	switch (dsfField) 
	{
		case dsfCLASSID:		cli.SetName(sVal);
								break;
		case dsfCODE:			cli.SetCode(sVal);
								break;
		case dsfDESCRIPTION:	cli.SetDescription(sVal);
								break;
	}
}

void DomainSortDoc::SetItemPart(long iKey, DomainSortFields dsfField, const string& sVal)
{
	switch (dsfField) 
	{
		case dsfCLASSID:		SetName(iKey, sVal);
								break;
		case dsfCODE:			SetCode(iKey, sVal);
								break;
		case dsfDESCRIPTION:	SetDescription(iKey, sVal);
								break;
	}
}

string DomainSortDoc::sGetItemPart(long iKey, DomainSortFields dsfField)
{
	switch (dsfField) 
	{
		case dsfCLASSID:		return sName(iKey);
								break;
		case dsfCODE:			return sCode(iKey);
								break;
		case dsfDESCRIPTION:	return sDescription(iKey);
								break;
	}
	return sUNDEF;	// Should not be possible!
}


/////////////////////////////////////////////////////////////////////////////
// DomainGroupDoc

IMPLEMENT_DYNCREATE(DomainGroupDoc, DomainSortDoc)

DomainGroupDoc::DomainGroupDoc()
{}

DomainGroupDoc::~DomainGroupDoc()
{}


BEGIN_MESSAGE_MAP(DomainGroupDoc, DomainSortDoc)
	//{{AFX_MSG_MAP(DomainGroupDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// DomainGroupDoc diagnostics

#ifdef _DEBUG
void DomainGroupDoc::AssertValid() const
{
	IlwisDocument::AssertValid();
}

void DomainGroupDoc::Dump(CDumpContext& dc) const
{
	IlwisDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// DomainGroupDoc commands

BOOL DomainGroupDoc::OnNewDocument()
{
  // not allowed to create an empty DomainGroupDoc
  return FALSE;
}

BOOL DomainGroupDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	if (!DomainSortDoc::OnOpenDocument(lpszPathName))
		return FALSE;

    return TRUE;
}

// Interface function to the DomainGroup
double DomainGroupDoc::rBound(long iKey)
{
    DomainGroup* pdg = dom->pdgrp();
    ISTRUE(fINotEqual, pdg, (DomainGroup*)0);

	return pdg->rUpper(pdg->iKey(iKey));
}

void DomainGroupDoc::SetBound(long iKey, double rBound)
{
    DomainGroup* pdg = dom->pdgrp();
    ISTRUE(fINotEqual, pdg, (DomainGroup*)0);

	pdg->SetUpperBound(pdg->iKey(iKey), rBound);

	UpdateAllViews(NULL, 0, NULL);
}

long DomainGroupDoc::iMergeAddItem(const ClassItem& cli)
{
	if (cli.sName().length() ==0)
		return iUNDEF;

	DomainGroup* pdg = dom->pdgrp();
	ISTRUE(fINotEqual, pdg, (DomainGroup*)0);

	const GroupItem& gli = dynamic_cast<const GroupItem&>(cli);
	double rBnd = pdg->rUpper(pdg->iClassify(gli.rBound()));

  // rBnd <> iUNDEF: found upper boundary >= new boundary
	if (rBnd != iUNDEF && abs(rBnd - gli.rBound()) < 1.0e-6)
		return iUNDEF;

	long id = pdg->iMergeAdd(gli.sName(), gli.sCode());
	if (id == iUNDEF)
		return iUNDEF;

	pdg->SetDescription(id, gli.sDescription());
	pdg->SetUpperBound(id, gli.rBound());
	pdg->Updated();
	id = pdg->iOrd(gli.sName());
	UpdateAllViews(NULL, 0, NULL);

	return id;
}

long DomainGroupDoc::iAddItem(const ClassItem& cli)
{
	if (cli.sName().length() ==0)
		return iUNDEF;

	DomainGroup* pdg = dom->pdgrp();
	ISTRUE(fINotEqual, pdg, (DomainGroup*)0);

	const GroupItem& gli = dynamic_cast<const GroupItem&>(cli);

	long id = pdg->iAdd(gli.sName());
	if (gli.sCode().length() != 0)
		pdg->SetCode(id, gli.sCode());
	pdg->SetDescription(id, gli.sDescription());
	pdg->SetUpperBound(id, gli.rBound());
	pdg->Updated();
	id = pdg->iOrd(gli.sName());
	UpdateAllViews(NULL, 0, NULL);

	return id;
}

void DomainGroupDoc::SetItem(long iKey, const ClassItem &cli)
{
    DomainGroup* pdg = dom->pdgrp();
    ISTRUE(fINotEqual, pdg, (DomainGroup*)0);

	long iItem = pdg->iKey(iKey);
	const GroupItem& gli = dynamic_cast<const GroupItem&>(cli);
	pdg->SetVal(iItem, gli.sName());
	pdg->SetCode(iItem, gli.sCode());
	pdg->SetDescription(iItem, gli.sDescription());
	pdg->SetUpperBound(iItem, gli.rBound());

	UpdateAllViews(NULL, 0, NULL);
}

void DomainGroupDoc::GetItem(long iKey, ClassItem &cli)
{
    DomainGroup* pdg = dom->pdgrp();
    ISTRUE(fINotEqual, pdg, (DomainGroup*)0);

	long iItem = pdg->iKey(iKey);
	GroupItem& gli = dynamic_cast<GroupItem&>(cli);
	gli.SetName(pdg->sNameByRaw(iItem, 0));
	gli.SetCode(pdg->sCodeByRaw(iItem, 0));
	gli.SetDescription(pdg->sDescriptionByRaw(iItem));
	gli.SetBound(pdg->rUpper(iItem));
}

bool DomainGroupDoc::fCanAdd(const ClassItem &cli)
{
    DomainGroup* pdg = dom->pdgrp();
    ISTRUE(fINotEqual, pdg, (DomainGroup*)0);

	if (pdg->iOrd(cli.sName()) != iUNDEF)
		return false;
	if (cli.sCode().length() > 0 && pdg->iOrd(cli.sCode()) != iUNDEF)
		return false;

	const GroupItem& gli = dynamic_cast<const GroupItem&>(cli);
	double rBnd = pdg->rUpper(pdg->iClassify(gli.rBound()));

  // rBnd <> iUNDEF: found upper boundary >= new boundary
	if (rBnd != iUNDEF && abs(rBnd - gli.rBound()) < 1.0e-6)
		return false;
	return true;
}

String DomainGroupDoc::sMakeClipLine(const ClassItem &cli)
{
	const GroupItem& gli = dynamic_cast<const GroupItem&>(cli);

	return String(FORMAT_GROUP, gli.sName(), gli.sCode(), gli.sDescription(), gli.rBound());
}

/*
	dsfField is determined by the software: the ordering is exactly according to the Class/GroupItems
*/
void DomainGroupDoc::MakeItemPart(DomainSortFields dsfField, ClassItem &cli, const string &sVal)
{
	switch (dsfField) 
	{
		case dsfUPPER: {	// handle the upper bound value
					GroupItem& gli = dynamic_cast<GroupItem&>(cli);
					double rBound = String(sVal).rVal();
					gli.SetBound(rBound);
					break;
				 }
		default:	DomainSortDoc::MakeItemPart(dsfField, cli, sVal);
					break;
	}
}

/*
	dsfField is determined by UI: it is the column number in the listview.
	These are ordered differently from the internal structure in the
	Class/GroupItems. Therefore a mapping has to be performed.
*/
void DomainGroupDoc::SetItemPart(long iKey, DomainSortFields dsfField, const string& sVal)
{
	// map the column index to the correct subitem
	int iField = dsfField;
	if (--iField < 0)
		iField = dsfUPPER;

	switch (iField) 
	{
		case dsfUPPER:	{
					double rBound = String(sVal).rVal();
					SetBound(iKey, rBound);
					break;
				}
		default:	DomainSortDoc::SetItemPart(iKey, (DomainSortFields)iField, sVal);
					break;
	}
}

/*
	dsfField is determined by UI: it is the column number in the listview.
	These are ordered differently from the internal structure in the
	Class/GroupItems. Therefore a mapping has to be performed.
*/
string DomainGroupDoc::sGetItemPart(long iKey, DomainSortFields dsfField)
{
	// map the column index to the correct subitem
	int iField = dsfField;
	if (--iField < 0)
		iField = dsfUPPER;

	switch (iField) 
	{
		case dsfUPPER:	{
					double rVal = rBound(iKey);
					String sField;
					if (rVal == rUNDEF)
						sField = "?";
					else
						sField = String("%g", rVal).sVal();
					return sField;
					break;
				}
		default:	return DomainSortDoc::sGetItemPart(iKey, (DomainSortFields)iField);
					break;
	}
}

