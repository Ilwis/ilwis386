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
// CriteriaTreeDoc.h: interface for the CriteriaTreeDoc class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CRITERIATREEDOC_H__70252949_50FF_4D7A_967D_9CC51410FB03__INCLUDED_)
#define AFX_CRITERIATREEDOC_H__70252949_50FF_4D7A_967D_9CC51410FB03__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_ILWISDOCUMENT_H__30AB4836_C0DB_11D2_B70E_00A0C9D5342F__INCLUDED_)
#include "Client\Base\IlwisDocument.h"
#endif

#undef IMPEXP
#ifdef ILWISCLIENT
#define IMPEXP __declspec(dllexport)
#else
#define IMPEXP __declspec(dllimport)
#endif

#include <vector>
using namespace std;

class EffectGroup;
class String;

enum eHintType {eALTNRCHANGED = 10000, eITEMINSERTED, eITEMUPDATED, eNODEUPDATED, eNODEDELETED, eWEIGHTSCHANGED, eEDITMODECHANGED, eLABELCHANGED};

class IMPEXP CriteriaTreeDoc : public IlwisDocument  
{
	DECLARE_DYNCREATE(CriteriaTreeDoc)
	DECLARE_MESSAGE_MAP()

public:
	enum eEditModeTP {eEDITTREE, eSTDWEIGH};

	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual ~CriteriaTreeDoc();
	void NewTree(CString sGoal);
	EffectGroup* egRoot();
	void OnGenerateAllItems();
	void OnUpdateGenerateAllMaps(CCmdUI*);
	void OnEditTree();
	void OnUpdateEditTree(CCmdUI*);
	void OnStdWeigh();
	void OnUpdateStdWeigh(CCmdUI*);
	void OnSetAlternatives();
	void OnUpdateSetAlternatives(CCmdUI*);
	void OnOverlayMaps();
	void OnUpdateOverlayMaps(CCmdUI*);
	int iGetNrAlternatives();
	CString sAlternative(int iCol);
	eEditModeTP GetMode();
	void SetGeoRef(GeoRef* grf);
	GeoRef* ptrGrf();
	void RefreshGeoref(); // cleans up georef if there are no more maps in tree, or sets it to the first it encounters
	virtual zIcon icon() const;
	void ShowMap(FileName fnMap); // Takes care of overlaying the layers specified in "Overlay Maps"
	void ShowContourMapCombination(vector<FileName> vfnContourMaps, vector<ValueRange> vvrCustomValueRanges, FileName fnInfoMap); // Takes care of overlaying the layers specified in "Overlay Maps"

protected:
	CriteriaTreeDoc(); // create from serialization only

	afx_msg void OnFileNew();
	afx_msg void OnFileOpen();
	void OnFileSave();
	void OnFileSaveAs();
	virtual void DeleteContents();

private:
	EffectGroup * egCriteriaTreeRoot;
	vector <String> m_vsAlternatives;
	eEditModeTP eEditMode;
	GeoRef* m_grf;
	bool m_fFirstTime;
	int m_iOverlayMapsOption;
	FileName m_fnOverlayMap;
	FileName m_fnOverlayMapViewTemplate;
	bool m_fUseMapViewGeometry;
};

#endif // !defined(AFX_CRITERIATREEDOC_H__70252949_50FF_4D7A_967D_9CC51410FB03__INCLUDED_)
