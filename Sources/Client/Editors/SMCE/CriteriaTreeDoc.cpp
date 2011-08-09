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
// CriteriaTreeDoc.cpp: implementation of the CriteriaTreeDoc class.
//
//////////////////////////////////////////////////////////////////////
#include "Client\Headers\formelementspch.h"
#include "Headers\constant.h"
#include "Client\Editors\SMCE\CriteriaTreeDoc.h"
#include "Client\Editors\SMCE\CriteriaTreeItem.h"
#include "Client\Editors\SMCE\SmceMapWindow.h"
#include "Engine\Base\DataObjects\CriteriaTreeObject.h"
#include "Engine\Base\File\ElementMap.h"
#include "Client\FormElements\frmgenap.h"
#include "Client\FormElements\flddat.h"
#include "Client\FormElements\FieldMultiObjectSelect.h" // For AttributeFileName
#include "Client\FormElements\objlist.h" // for extending ObjectExtLister
#include "Client\ilwis.h" // for IlwWinApp
#include "Headers\Hs\smce.hs"
#include "Headers\Htp\Ilwismen.htp"
#include "Headers\messages.h" // for ILWM_EXECUTE
#include "Client\Mapwindow\MapCompositionDoc.h" // for creating MapViews
#include "Client\Base\IlwisDocTemplate.h"
#include "Engine\Base\DataObjects\ObjectStructure.h"

#include "Client\Mapwindow\MapPaneView.h" // for MapPaneView

#include "Engine\Map\Segment\Seg.h"

/////////////////////////////////////////////////////////////////////////////
// CriteriaTreeDoc

IMPLEMENT_DYNCREATE(CriteriaTreeDoc, IlwisDocument)

BEGIN_MESSAGE_MAP(CriteriaTreeDoc, IlwisDocument)
	ON_COMMAND(ID_CT_FILE_NEW, OnFileNew)
	ON_COMMAND(ID_CT_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_CT_ALLMAPS, OnGenerateAllItems)
	ON_UPDATE_COMMAND_UI(ID_CT_ALLMAPS, OnUpdateGenerateAllMaps)
	ON_COMMAND(ID_CT_GENALLMAPS, OnGenerateAllItems)
	ON_UPDATE_COMMAND_UI(ID_CT_GENALLMAPS, OnUpdateGenerateAllMaps)
	ON_COMMAND(ID_CT_SETALTERNATIVES, OnSetAlternatives)
	ON_UPDATE_COMMAND_UI(ID_CT_SETALTERNATIVES, OnUpdateSetAlternatives)
	ON_COMMAND(ID_CT_EDITTREE, OnEditTree)
	ON_UPDATE_COMMAND_UI(ID_CT_EDITTREE, OnUpdateEditTree)
	ON_COMMAND(ID_CT_STDWEIGH, OnStdWeigh)
	ON_UPDATE_COMMAND_UI(ID_CT_STDWEIGH, OnUpdateStdWeigh)
	ON_COMMAND(ID_CT_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_CT_FILE_SAVE_AS, OnFileSaveAs)
	ON_COMMAND(ID_CT_OVERLAYMAPS, OnOverlayMaps)
	ON_UPDATE_COMMAND_UI(ID_CT_OVERLAYMAPS, OnUpdateOverlayMaps)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CriteriaTreeDoc construction/destruction

CriteriaTreeDoc::CriteriaTreeDoc()
: egCriteriaTreeRoot(0)
, eEditMode(eEDITTREE)
, m_grf(0)
, m_fFirstTime(true)
, m_iOverlayMapsOption(0)
{
	// TODO: add one-time construction code here
	fUseSerialize = true; // to keep IlwisDocument::OnSaveDocument happy
}

CriteriaTreeDoc::~CriteriaTreeDoc()
{
	if (egCriteriaTreeRoot)
		delete egCriteriaTreeRoot;
	if (m_grf)
		delete m_grf;
}

zIcon CriteriaTreeDoc::icon() const
{
	return zIcon("SMCEICON");
}

class NewCriteriaTreeDocumentForm: public FormWithDest
{
public:
  NewCriteriaTreeDocumentForm(CWnd* wPar, int* iChoice, bool fFirstTime)
  : FormWithDest(wPar, TR("Spatial Multicriteria Evaluation"))
	{
		fbs |= fbsNOCANCELBUTTON;
		new StaticText(root, TR("Create a new Criteria Tree for:"));
		rgChoice = new RadioGroup(root, "", iChoice);
		rgChoice->SetCallBack((NotifyProc)&NewCriteriaTreeDocumentForm::CallBackFunc);
		new RadioButton(rgChoice, TR("Problem Analysis"));
		new RadioButton(rgChoice, TR("Design of Alternatives"));
		RadioButton* rbChoice3 = new RadioButton(rgChoice, TR("Decision Making"));
		if (fFirstTime)
		{
			RadioButton* rbChoice4 = new RadioButton(rgChoice, TR("Open an existing Criteria Tree"));
			rbChoice4->SetHeight(50);
		}
		fsMessage = new FieldString(root, &m_sDummy, WS_BORDER|ES_READONLY|ES_MULTILINE|ES_AUTOVSCROLL|WS_TABSTOP|WS_GROUP);
		fsMessage->Align(rgChoice, AL_AFTER);
		fsMessage->SetWidth(150);
		fsMessage->SetHeight(100);
		
		SetMenHelpTopic("ilwismen\\smce_window_new.htm");
		
		create();
	}
	int CallBackFunc(Event*)
	{
		int i = rgChoice->iVal();
		switch (i)
		{
		case 0:
			fsMessage->SetVal(TR("Analyse a problem situation using one set of maps as evaluation criteria, e.g. environmental impact assessment. The criteria tree editor will start with an empty tree and a placeholder for one data set."));
			break;
		case 1:
			fsMessage->SetVal(TR("Perform analysis for designing alternatives/options using one set of maps as evaluation criteria, e.g. (un)suitability analysis. The criteria tree editor will start with an empty tree and a placeholder for one data set."));
			break;
		case 2:
			fsMessage->SetVal(TR("Decide between alternatives/options using one set of maps for each alternative as evaluation criteria. The criteria tree editor will start with an empty tree and placeholders for a number of data sets, each corresponding to an alternative."));
			break;
		default:
			fsMessage->SetVal(TR("You will be offered the possibility to find and re-open a previously saved criteria tree."));
			break;
		}
		return 0;
	}

private:
	String m_sDummy;
	FieldString* fsMessage;
	RadioGroup* rgChoice;
};

BOOL CriteriaTreeDoc::OnNewDocument()
{
	if (!IlwisDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)
	int iChoice = 0;
	NewCriteriaTreeDocumentForm frm (wndGetActiveView(), &iChoice, m_fFirstTime);
	if (frm.fOkClicked())
	{
		switch (iChoice)
		{
		case 0 : // nothing special
			break;
		case 1:  // nothing special
			break;
		case 2:
			m_vsAlternatives.clear();
			m_vsAlternatives.push_back("Alternative 1");
			m_vsAlternatives.push_back("Alternative 2");
			egCriteriaTreeRoot->SetNrAlternatives(2);
			wndGetActiveView()->PostMessage(WM_COMMAND, ID_CT_SETALTERNATIVES);
			break;
		case 3:
			wndGetActiveView()->PostMessage(WM_COMMAND, ID_CT_FILE_OPEN);
			break;
		}
	}
	else
		return FALSE;

	if (m_fFirstTime)
		m_fFirstTime = false;

	return TRUE;
}

void CriteriaTreeDoc::OnFileNew()
{
	if (!SaveModified())
		return;
	OnNewDocument();
}

void CriteriaTreeDoc::OnFileOpen()
{
	if (!SaveModified())
		return;
	class OpenForm: public FormWithDest
  {
  public:
    OpenForm(CWnd* parent, String* sName)
    : FormWithDest(parent, TR("Open Criteria Tree"))
    {
			new FieldDataTypeLarge(root, sName, ".smc");

			SetMenHelpTopic("ilwismen\\smce_window_open_criteria_tree.htm");

      create();
    }
  };
  String sCriteriaTree;
  OpenForm frm(wndGetActiveView(), &sCriteriaTree);
	if (frm.fOkClicked())
	{
		FileName fn (sCriteriaTree);
		IlwWinApp()->SetCurDir(fn.sPath()); // make sure we're relative to the opened file
		OnOpenDocument(sCriteriaTree.c_str());
		SetModifiedFlag(FALSE);
		UpdateAllViews(0); // is this the place ???
	}
}

void CriteriaTreeDoc::DeleteContents()
{
	NewTree("New Goal"); // calls UpdateAllViews but not the first time
	eEditMode = eEDITTREE;
}

void CriteriaTreeDoc::NewTree(CString sGoal)
{
	m_vsAlternatives.clear();
	m_vsAlternatives.push_back("");
	if (egCriteriaTreeRoot) // preserve it, just delete its children
	{
		egCriteriaTreeRoot->DeleteAllChildren();
		egCriteriaTreeRoot->Clear(); // reset members like weight, output maps
		egCriteriaTreeRoot->SetName(sGoal);
		SetGeoRef(0);
		UpdateAllViews(0);
	}
	else
	{
		// First call: avoid UpdateAllViews coz the CTreeCtrl isn't ready yet
		egCriteriaTreeRoot = new EffectGroup(this, sGoal);
	}
	egCriteriaTreeRoot->SetNrAlternatives(iGetNrAlternatives());
}

EffectGroup* CriteriaTreeDoc::egRoot()
{
	return egCriteriaTreeRoot;
}

int CriteriaTreeDoc::iGetNrAlternatives()
{
	return m_vsAlternatives.size();
}

class AllMapsForm: public FormWithDest
{
public:
  AllMapsForm(CWnd* wPar, int iNrMaps, bool* fShow)
    : FormWithDest(wPar, TR("Composite Index Map Calculation"))
	{
		new StaticText(root, String(TR("Generate the %d output map(s) in the criteria tree?").c_str(), iNrMaps));
		new StaticText(root, TR("Existing maps will be overwritten."));
		new CheckBox(root, TR("Show after calculation"), fShow);

		SetMenHelpTopic("ilwismen\\smce_window_all_composite_index_maps.htm");
		create();
	}
};

void CriteriaTreeDoc::OnGenerateAllItems()
{
	int iNrMapsToBeCalculated = egCriteriaTreeRoot->iRecursivelyCalculateOutput(true, true);
	if (iNrMapsToBeCalculated > 0)
	{
		bool fShow=true;
		AllMapsForm frm (0, iNrMapsToBeCalculated, &fShow);
		if (frm.fOkClicked())
		{
			CWaitCursor cwait;
			egCriteriaTreeRoot->iRecursivelyCalculateOutput(false, false, fShow); // all maps and scores!
		}
	}
	else
	{
		CWaitCursor cwait;
		egCriteriaTreeRoot->iRecursivelyCalculateOutput(false, false);
	}
}

void CriteriaTreeDoc::OnUpdateGenerateAllMaps(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(egCriteriaTreeRoot->iRecursivelyCalculateOutput(true, false) > 0);
}

class AskAlternativesForm: public FormWithDest
{
public:
  AskAlternativesForm(CWnd* wPar, vector<String> *vs)
    : FormWithDest(wPar, TR("Enter the number of alternatives"))
		, m_vs(vs)
		, m_iNr(vs->size())
		, iMAX(9)
  {
		m_fs.resize(iMAX);
		m_vs->resize(iMAX); // this is so that the thing "remembers" what the user typed
		// even reducing and then increasing the nr of alternatives
		for (int i=0; i<iMAX; ++i) // fill up all empties with defaults
		{
			if ((*m_vs)[i] == "")
				(*m_vs)[i] = String("Alternative %d", i+1);
		}
    fi = new FieldInt(root, TR("Number of alternatives:"), &m_iNr, ValueRange(1,9), true);
		fi->SetCallBack((NotifyProc)&AskAlternativesForm::CallBackFunc);
		for (int i=0; i<iMAX; ++i)
		{
			m_fs[i] = new FieldString(root, String(TR("Alternative %d").c_str(), i+1), &(*m_vs)[i]);
			m_fs[i]->SetCallBack((NotifyProc)&AskAlternativesForm::CallBackFunc);
			if ((*m_vs)[i] == "")
				m_fs[i]->SetVal(String("Alternative %d", i+1));
		}
		fsMessage = new FieldString(root, &m_sDummy, ES_READONLY|WS_TABSTOP|WS_GROUP);
		fsMessage->SetWidth(95);
		fsMessage->SetIndependentPos();

		SetMenHelpTopic("ilwismen\\smce_window_alternatives.htm");

		create();
	}
	int CallBackFunc(Event*)
	{
		fi->StoreData();

		if (m_iNr == 1)
		{
			fsMessage->SetVal(TR("Performing Problem Analysis / Design."));
			if (m_fs[0])
				m_fs[0]->Hide();
		}
		else
		{
			fsMessage->SetVal(TR("Performing Decision Making."));
			if (m_fs[0])
				m_fs[0]->Show();
		}

		bool fOK = (m_iNr > 0) && (m_iNr <= iMAX);
		if (fOK)
		{
			if (m_iNr != 1)
			{
				for (int i=0; i<m_iNr; ++i)
				{
					m_fs[i]->Show();
					fOK = fOK && (m_fs[i]->sVal()!="");
				}
			}
			for (int i=m_iNr; i<iMAX; ++i)
				m_fs[i]->Hide();
		}
		if (fOK)
			EnableOK();
		else    
			DisableOK();

		return 0;
	}
	bool fOkClicked()
	{
		m_vs->resize(m_iNr);
		if (m_iNr == 1)
			(*m_vs)[0] = "";
		return FormWithDest::fOkClicked();
	}
private:
	FieldInt* fi;
	int m_iNr;
	vector<String> *m_vs;
	const int iMAX;
	vector <FieldString*> m_fs;
	String m_sDummy;
	FieldString* fsMessage;
};

void CriteriaTreeDoc::OnSetAlternatives()
{
	vector <String> vTemp = m_vsAlternatives;
	AskAlternativesForm frm(0, &vTemp);
  if (frm.fOkClicked())
	{
		m_vsAlternatives = vTemp;
		egCriteriaTreeRoot->SetNrAlternatives(vTemp.size());
		SetModifiedFlag();
		UpdateAllViews(0, eALTNRCHANGED);
	}
}

void CriteriaTreeDoc::OnUpdateSetAlternatives(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(eEditMode == eEDITTREE);
}

CString CriteriaTreeDoc::sAlternative(int iCol)
{
	if (iCol > 0)
		return m_vsAlternatives[iCol-1].c_str();
	else
		return "";
}

void CriteriaTreeDoc::OnEditTree()
{
	eEditMode = eEDITTREE;
	UpdateAllViews(0, eEDITMODECHANGED);
}

void CriteriaTreeDoc::OnUpdateEditTree(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(eEditMode == eEDITTREE);
}

void CriteriaTreeDoc::OnStdWeigh()
{
	eEditMode = eSTDWEIGH;
	UpdateAllViews(0, eEDITMODECHANGED);
}

void CriteriaTreeDoc::OnUpdateStdWeigh(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(eEditMode == eSTDWEIGH);
	bool fTreeEditDone = egCriteriaTreeRoot->fDone(0, true);
	pCmdUI->Enable(fTreeEditDone || (eEditMode == eSTDWEIGH));
}

CriteriaTreeDoc::eEditModeTP CriteriaTreeDoc::GetMode()
{
	return eEditMode;
}

void CriteriaTreeDoc::SetGeoRef(GeoRef* grf)
{
	if (m_grf)
		delete m_grf;
	m_grf = grf;
}

GeoRef* CriteriaTreeDoc::ptrGrf()
{
	return m_grf;
}

void CriteriaTreeDoc::RefreshGeoref()
{
	if (egCriteriaTreeRoot)
	{
		FileName fn = FileName(egCriteriaTreeRoot->afnFindFirstInputMap());
		if ((fn.fValid()) && (fn!=FileName()) && (fn.fExist()))
		{

			bool *fDoNotShowError = (bool *)(IlwWinApp()->Context()->pGetThreadLocalVar(IlwisAppContext::tlvDONOTSHOWFINDERROR));
			bool fDoNotShowErrorPrevious = *fDoNotShowError;
			*fDoNotShowError = true; // don't allow Map to Show a "Find error: Georef.grf".
			try
			{
				Map mp (fn);
				if (mp.fValid())
				{
					GeoRef* gr = new GeoRef(mp->gr());
					if (gr->fValid())
						SetGeoRef(gr);
					else
						SetGeoRef(0);
				}
				else
					SetGeoRef(0);
			}
			catch (ErrorObject&)
			{
				SetGeoRef(0);
			}
			*fDoNotShowError = fDoNotShowErrorPrevious;
		}
		else
			SetGeoRef(0);
	}
}
/*
void CDocument::OnFileSave()
{
	DoFileSave();
}

void CDocument::OnFileSaveAs()
{
	if (!DoSave(NULL))
		TRACE0("Warning: File save-as failed.\n");
}
*/
void CriteriaTreeDoc::OnFileSave()
{
	// Something changes the current dir without setting it back, which is disasterous for a.o. the NameEdits
	String sOldDir = IlwWinApp()->sGetCurDir();
	IlwisDocument::OnFileSave();
	IlwWinApp()->SetCurDir(sOldDir);
}

void CriteriaTreeDoc::OnFileSaveAs()
{
	String sOldDir = IlwWinApp()->sGetCurDir();
	IlwisDocument::OnFileSaveAs();
	IlwWinApp()->SetCurDir(sOldDir);
}
/////////////////////////////////////////////////////////////////////////////
// CriteriaTreeDoc serialization

void CriteriaTreeDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		CriteriaTreeObject cto;
		ElementContainer& en = const_cast<ElementContainer&>(cto->fnObj);
		if (0 == en.em)
			en.em = new ElementMap;
		cto->Updated();
		cto->Store();
		// Done with ILWIS object
		// Now write out our own members into the ElementMap
		ObjectInfo::WriteElement("Display", "Mode", en, m_iOverlayMapsOption);
		ObjectInfo::WriteElement("Display", "Map", en, m_fnOverlayMap);
		ObjectInfo::WriteElement("Display", "MapView", en, m_fnOverlayMapViewTemplate);
		ObjectInfo::WriteElement("Alternatives", "Total", en, (int)m_vsAlternatives.size());
		for (unsigned int i=0; i<m_vsAlternatives.size(); ++i)
			ObjectInfo::WriteElement("Alternatives", String("Alternative%d", i).c_str(), en, m_vsAlternatives[i]);

		egCriteriaTreeRoot->WriteElements("Root", en); // the root is always there and we arrived here after a DeleteContents
		// Now write all at once
		en.em->Serialize(ar);

	}
	else
	{
		ElementContainer en;
		en.em = new ElementMap;
		// First read the entire ElementMap
		en.em->Serialize(ar);
		// Now read our own members from the Element Map
		if (!ObjectInfo::ReadElement("Display", "Mode", en, m_iOverlayMapsOption))
			m_iOverlayMapsOption = 0;
		if (!ObjectInfo::ReadElement("Display", "Map", en, m_fnOverlayMap))
			m_fnOverlayMap = FileName();
		else
		{
			if ((!m_fnOverlayMap.fExist()) && (m_iOverlayMapsOption == 1))
				m_iOverlayMapsOption = 0;
		}
		if (!ObjectInfo::ReadElement("Display", "MapView", en, m_fnOverlayMapViewTemplate))
			m_fnOverlayMapViewTemplate = FileName();
		else
		{
			if ((!m_fnOverlayMapViewTemplate.fExist()) && (m_iOverlayMapsOption == 2))
				m_iOverlayMapsOption = 0;
		}
		int iSize;
		if (!ObjectInfo::ReadElement("Alternatives", "Total", en, iSize))
			iSize = 1;
		m_vsAlternatives.resize(iSize);
		for (int i=0; i<iSize; ++i)
			ObjectInfo::ReadElement("Alternatives", String("Alternative%d", i).c_str(), en, m_vsAlternatives[i]);

		egCriteriaTreeRoot->ReadElements("Root", en); // the root is always there and we arrived here after a DeleteContents

		// Some extra initialisation
		RefreshGeoref();
		// Done!
	}
}

// Lister that will accept any vector map, and for raster maps anyone with a georef that "fits"

class BaseMapListerGeoRef: public ObjectExtLister
{
public:
	BaseMapListerGeoRef(const FileName& fnGeoRef, bool fAccMapList = false)
		: ObjectExtLister(".mpr.mps.mpa.mpp.mpl"), fnGrf(fnGeoRef)
		, fAcceptMapList(fAccMapList)
	{}
	virtual bool fOK(const FileName& fnMap, const String& sColName)
	{
		// Reject any object that is not a Map or MapList
		if (! (fCIStrEqual(fnMap.sExt , ".mpr") ||
					 fCIStrEqual(fnMap.sExt , ".mps") ||
					 fCIStrEqual(fnMap.sExt , ".mpa") ||
					 fCIStrEqual(fnMap.sExt , ".mpp") ||
					 fCIStrEqual(fnMap.sExt , ".mpl")) )  // allow MapList bands
			return false;

		bool fCheckObjectSelf = sCHECK_OBJECT_ONLY == sColName;
		// fCheckObjectSelf == true means the user clicked (selected) an item
		// fCheckObjectSelf == false means the tree is filling/expanding

		if (!fAcceptMapList && fCheckObjectSelf && fCIStrEqual(fnMap.sExt, ".mpl"))
			return false; // selection of maplist itself isn't allowed - force selection of one map

		FileName fnBand = fnMap;
		if (fCIStrEqual(fnMap.sExt , ".mpl") && fnBand.sSectionPostFix.length() == 0)
		{
			long iBands = 0;
			ObjectInfo::ReadElement("MapList", "Maps", fnMap, iBands);
			if (iBands == 0)
				return false;

			// Select the first band in the maplist as Map to check the domain
			long iOffset = 0;  // Initialize to get rid of warning
			ObjectInfo::ReadElement("MapList", "Offset", fnMap, iOffset);  // Offset is read properly
			String sKey = String("Map%li", iOffset);
			ObjectInfo::ReadElement("MapList", sKey.c_str(), fnMap, fnBand);
		}
		
		// fnBand contains the map to be checked

		FileName fnGeoRef;
		if (ObjectInfo::ReadElement("Map", "GeoRef", fnBand, fnGeoRef))
		{
			if (fnGeoRef.fExist())
				return (fnGrf == fnGeoRef);
			else
				return true; // Maps with only vector layers are always accepted
		}
		else
			return true; // Maps with only vector layers are always accepted
	}
private:
	const FileName fnGrf;
	bool fAcceptMapList;
};

// Lister that will accept any mapview with a georef that "fits"

class MapViewListerGeoRef: public ObjectExtLister
{
public:
	MapViewListerGeoRef(const FileName& fnGeoRef)
		: ObjectExtLister(".mpv"), fnGrf(fnGeoRef)
	{}
	virtual bool fOK(const FileName& fnMapView, const String&)
	{
		// Reject any object that is not a MapView
		if (! (fCIStrEqual(fnMapView.sExt , ".mpv")))
			return false;

		bool fRasterLayerFound = false;
		int iLayers;
		if (ObjectInfo::ReadElement("MapView", "Layers", fnMapView, iLayers))
		{
			int iCurrentLayer = 1;
			while (!fRasterLayerFound && (iCurrentLayer <= iLayers))
			{
				String sType;
				String sSection("Layer%i", iCurrentLayer);
				if (ObjectInfo::ReadElement(sSection.c_str(), "Type", fnMapView, sType))
					if ("MapDrawer" == sType)
						fRasterLayerFound = true;
				++iCurrentLayer;
			}

			if (fRasterLayerFound)
			{
				FileName fnGeoRef;
				if (ObjectInfo::ReadElement("MapView", "GeoRef", fnMapView, fnGeoRef))
				{
					if (fnGeoRef.fExist())
						return (fnGrf == fnGeoRef);
					else
						return true; // MapViews with only vector layers are always accepted
				}
				else
					return true; // MapViews with only vector layers are always accepted
			}
			else
				return true; // MapViews with only vector layers are always accepted
		}
		else
			return false; // invalid MapView (no layers defined)
	}
private:
	const FileName fnGrf;
};

class OverlayMapsForm: public FormWithDest
{
public:
  OverlayMapsForm(CWnd* wPar, int& iOption, String& sOverlayMap, String& sMapViewTemplate, FileName fnGrf)
    : FormWithDest(wPar, TR("Map Display Overlay Options"))
	{
		iImg = IlwWinApp()->iImage(".mpv");

		RadioGroup* rg = new RadioGroup(root, TR("When displaying maps, show:"), &iOption);
		RadioButton* rb1 = new RadioButton(rg, TR("Only requested maps"));
		RadioButton* rb2 = new RadioButton(rg, TR("Overlay with map:"));
		RadioButton* rb3 = new RadioButton(rg, TR("Overlay with MapView:"));

		if (!FileName(sOverlayMap).fExist())
			sOverlayMap = "";
		if (!FileName(sMapViewTemplate).fExist())
			sMapViewTemplate = "";
		new FieldDataType(rb2, "", &sOverlayMap, new BaseMapListerGeoRef(fnGrf), true);
		new FieldDataType(rb3, "", &sMapViewTemplate, new MapViewListerGeoRef(fnGrf), true);

		create();
	}
};

void CriteriaTreeDoc::OnOverlayMaps()
{
	if (0 != ptrGrf() && ptrGrf()->fValid())
	{
		String sOverlayMap = m_fnOverlayMap.sFullPathQuoted();
		String sOverlayMapViewTemplate = m_fnOverlayMapViewTemplate.sFullPathQuoted();
		OverlayMapsForm frm (wndGetActiveView(), m_iOverlayMapsOption, sOverlayMap, sOverlayMapViewTemplate, ptrGrf()->ptr()->fnObj);
		if (frm.fOkClicked())
		{
			m_fnOverlayMap = sOverlayMap;
			m_fnOverlayMapViewTemplate = sOverlayMapViewTemplate;
			SetModifiedFlag();
		}
	}
}

void CriteriaTreeDoc::OnUpdateOverlayMaps(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(0 != ptrGrf() && ptrGrf()->fValid());
}

// class MapCompositionDocEx extends MapCompositionDoc with one function to apply an external MapView as a template
// This extension was needed in order to read the drawers with their configuration from file calling drDrawer,
// otherwise function drDrawer would have to become "public"
// Current implementation: all layers in the supplied MapView will be added, except the first if it is a raster layer.

class MapCompositionDocEx: public MapCompositionDoc
{
public:
	void ApplyMapViewTemplate(const MapView& mapview)
	{
		//try {
		//	if (!mapview.fValid())
		//		return;
			throw ErrorObject("TO DO");
		//	int iLayers = mapview->iReadElement("MapView", "Layers");
		//	int iStartLayer = 1;
		//	String sType;
		//	// skip first layer if MapView has > 1 layer and the first is a raster map layer
		//	if (iLayers > 1)
		//	{
		//		mapview->ReadElement("Layer1", "Type", sType);
		//		if ("MapDrawer" == sType)
		//			++iStartLayer;
		//	}
		//	for (int i = iStartLayer; i <= iLayers; ++i) {
		//		String sSection("Layer%i", i);
		//		mapview->ReadElement(sSection.c_str(), "Type", sType);
		//		NewDrawer* dw = drDrawer(mapview, sSection.c_str());
		//		if (0 == dw) // protection against faulty .mpv files
		//			continue;
		//		if ("MapDrawer" == sType)
		//		{
		//			if (fRaster)
		//				dl.push_back(dw);
		//			else
		//				dl.push_front(dw);
		//			fRaster = true;
		//			FileName fnMap;
		//			mapview->ReadElement(sSection.c_str(), "Map", fnMap);
		//			Map rasmap(fnMap);
		//			if (rasmap->gr() != georef) 
		//				SetGeoRef(rasmap->gr());
		//		}
		//		else
		//			dl.push_back(dw);
		//	}
		//}
		//catch (ErrorObject& err) 
		//{
		//	err.Show();
		//}
	}
};

// MapViewThread is a so-called user-interface thread class (similar to WinThread)
// This class is dedicated to create a MapView on-the-fly and display it
// The member variables determine the layers that should be present

class MapViewThread: public CWinThread
{
public:
	MapViewThread()
	: m_fUseMapViewTemplate (false)
	, m_fSmceMapWindow (false)
	{
		dtSmceMapWindow = new IlwisDocTemplate(
			".mpr.mps.mpa.mpp.mpl.mpv", 
			"Map,SegmentMap,PolygonMap,PointMap,MapList,MapView",
			"ILWIS Map View",
			RUNTIME_CLASS(MapCompositionDoc),
			RUNTIME_CLASS(SmceMapWindow),
			RUNTIME_CLASS(MapPaneView));
	}

	~MapViewThread()
	{
		IlwWinApp()->Context()->RemoveThreadLocalVars();
		delete dtSmceMapWindow;
	}

	void AddLayer(FileName fn, bool fVisible = true, ValueRange vrCustom = ValueRange())
	{
		if (fVisible)
			m_vfnMaps.push_back(fn);
		else
			m_vfnInvisibleMaps.push_back(fn);

		if (vrCustom.fValid())
		{
			m_vstrvrCustomValueRanges[fn.sFileExt()] = vrCustom;
			m_fSmceMapWindow = true; // for now .. to save another member function to set this boolean
		}
	}

	void SetTemplateMapView(FileName fn)
	{
		if (fn.fExist())
		{
			m_fUseMapViewTemplate = true;
			m_fnMapViewTemplate = fn;
		}
	}

	void SetGeoRef(GeoRef& gr)
	{
		m_grf = gr;
	}

	virtual BOOL InitInstance()
	{
		struct OpeningDoc {
			OpeningDoc()
			{ IlwWinApp()->iOpeningDoc++; }
			~OpeningDoc()
			{ IlwWinApp()->iOpeningDoc--; }
		} od;
	
		IlwWinApp()->Context()->InitThreadLocalVars();
		
		::OleInitialize(NULL);
		
		MapCompositionDocEx* mcdex = new MapCompositionDocEx;
		if (mcdex->OnOpenDocument((m_vfnMaps[0].sFullPath().c_str()), IlwisDocument::otNOASK))
		{
			IlwWinApp()->docTemplMapWindow()->AddDocument(mcdex); // so that MFC "knows" that this document is already open
			
			for (unsigned int i = 1; i < m_vfnMaps.size(); ++i) // any other maps specified?
			{
				mcdex->drAppend(m_vfnMaps[i]);
			
			}

			if (m_fUseMapViewTemplate)
				mcdex->ApplyMapViewTemplate(m_fnMapViewTemplate);

			for (unsigned int i = 0; i < m_vfnInvisibleMaps.size(); ++i) // these maps should be added as layers, but switched off.
			{
				ILWIS::NewDrawer* dr = mcdex->drAppend(m_vfnInvisibleMaps[i]);
				if (dr)	// could be null
				{
					dr->setActive(false);
				}
			}
			throw ErrorObject("TO DO");
			//for (list<Drawer*>::iterator it = mcdex->dl.begin(); it != mcdex->dl.end(); ++it)
			//{
			//	SpatialDataDrawer* smd = dynamic_cast<SpatialDataDrawer*>(*it);
			//	if (smd)
			//	{
			//		ValueRange vr = m_vstrvrCustomValueRanges[smd->basemap()->fnObj.sFileExt()];
			//		if (vr.fValid()) // thicker lines, custom legend
			//		{
			//			smd->SetWidth(smd->iGetWidth() * 2);
			//			smd->SetCustomLegendValueRange(vr);
			//		}
			//	}
			//}

			CFrameWnd* pFrame;
			if (m_fSmceMapWindow)
				pFrame = dtSmceMapWindow->CreateNewFrame(mcdex, NULL);
			else
				pFrame = IlwWinApp()->docTemplMapWindow()->CreateNewFrame(mcdex, NULL);

			AfxGetThread()->m_pMainWnd = pFrame;
			IlwWinApp()->docTemplMapWindow()->InitialUpdateFrame(pFrame, mcdex, TRUE);

			if (m_grf.fValid())
			{
				SmceMapWindow* smw = dynamic_cast<SmceMapWindow*>(pFrame);
				if (smw)
					smw->SetGeoRef(m_grf);
			}

			return TRUE;
		}
		else
			delete mcdex;

		return FALSE;
	}
	
	virtual int ExitInstance()
	{
		::OleUninitialize();
		return CWinThread::ExitInstance();
	}

private:

	vector <FileName> m_vfnMaps;
	vector <FileName> m_vfnInvisibleMaps;
	map <String, ValueRange> m_vstrvrCustomValueRanges; // String: set to FileExt, without the path, so that comparison is easier
	bool m_fUseMapViewTemplate;
	bool m_fSmceMapWindow;
	FileName m_fnMapViewTemplate;
	GeoRef m_grf;
	IlwisDocTemplate *dtSmceMapWindow;
};

void CriteriaTreeDoc::ShowMap(FileName fnMap)
{
	switch (m_iOverlayMapsOption)
	{
		case 0:
		{
			String sExec("show %S -noask", fnMap.sFullPathQuoted());
			char* str = sExec.sVal();
			IlwWinApp()->GetMainWnd()->SendMessage(ILWM_EXECUTE, 0, (LPARAM)str);
			break;
		}
		case 1:
		{
			MapViewThread* mvt = new MapViewThread();
			if (mvt)
			{
				mvt->AddLayer(fnMap);
				mvt->AddLayer(m_fnOverlayMap);
				mvt->CreateThread(0, 0);
			}
			break;
		}
		case 2:
		{
			MapViewThread* mvt = new MapViewThread();
			if (mvt)
			{
				mvt->AddLayer(fnMap);
				mvt->SetTemplateMapView(m_fnOverlayMapViewTemplate);
				mvt->CreateThread(0, 0);
			}
			break;
		}
	}
}

void CriteriaTreeDoc::ShowContourMapCombination(vector<FileName> vfnContourMaps, vector<ValueRange> vvrCustomValueRanges, FileName fnInfoMap)
{
	switch (m_iOverlayMapsOption)
	{
		case 0:
		{
			MapViewThread* mvt = new MapViewThread();
			if (mvt)
			{
				mvt->AddLayer(vfnContourMaps[0], true, vvrCustomValueRanges[0]);
				for (unsigned int i = 1; i < vfnContourMaps.size(); ++i)
					mvt->AddLayer(vfnContourMaps[i], false, vvrCustomValueRanges[i]);
				mvt->AddLayer(fnInfoMap, false);
				if (ptrGrf())
					mvt->SetGeoRef(*ptrGrf());
				mvt->CreateThread(0, 0);
			}
			break;
		}
		case 1:
		{
			MapViewThread* mvt = new MapViewThread();
			if (mvt)
			{
				mvt->AddLayer(m_fnOverlayMap);
				mvt->AddLayer(vfnContourMaps[0], true, vvrCustomValueRanges[0]);
				for (unsigned int i = 1; i < vfnContourMaps.size(); ++i)
					mvt->AddLayer(vfnContourMaps[i], false, vvrCustomValueRanges[i]);
				mvt->AddLayer(fnInfoMap, false);
				if (ptrGrf())
					mvt->SetGeoRef(*ptrGrf());
				mvt->CreateThread(0, 0);
			}
			break;
		}
		case 2:
		{
			MapViewThread* mvt = new MapViewThread();
			if (mvt)
			{
				mvt->AddLayer(vfnContourMaps[0], true, vvrCustomValueRanges[0]);
				for (unsigned int i = 1; i < vfnContourMaps.size(); ++i)
					mvt->AddLayer(vfnContourMaps[i], false, vvrCustomValueRanges[i]);
				mvt->AddLayer(fnInfoMap, false);
				mvt->SetTemplateMapView(m_fnOverlayMapViewTemplate);
				if (ptrGrf())
					mvt->SetGeoRef(*ptrGrf());
				mvt->CreateThread(0, 0);
			}
			break;
		}
	}
}


void CriteriaTreeDoc::GetObjectStructure(const ElementContainer& en, ObjectStructure& os)
{
	int iOverlayMapsOption = 0;
	if (!ObjectInfo::ReadElement("Display", "Mode", en, iOverlayMapsOption))
		iOverlayMapsOption = 0;

	FileName fnToCopy;
	switch(iOverlayMapsOption)
	{
		case 0:
			break;
		case 1:
			ObjectInfo::ReadElement("Display", "Map", en, fnToCopy);
			break;
		case 2:
			ObjectInfo::ReadElement("Display", "MapView", en, fnToCopy);
			break;
	}

	if (fnToCopy.fValid() && fnToCopy.fExist())
	{
		os.AddFile(fnToCopy);
		// Retrieve the files belonging to the map (georef, tables, domains)
		if (IlwisObject::iotObjectType(fnToCopy) != IlwisObject::iotANY)
		{
			IlwisObject obj = IlwisObject::obj(fnToCopy);
			if ( obj.fValid())
				obj->GetObjectStructure(os);
		}					
	}
}
