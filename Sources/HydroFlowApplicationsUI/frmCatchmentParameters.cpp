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
// FormCatchmentParameters.cpp: implementation of the FormCatchmentParameters class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\AppFormsPCH.h"
#include "HydroFlowApplicationsUI\frmCatchmentParameters.h"
#include "Headers\Hs\Coordsys.hs"

using namespace std;

//////////////////////////////////////////////////////////////////////
// ***Drainage Extraction
//////////////////////////////////////////////////////////////////////
LRESULT Cmddrainagenetworkextraction(CWnd *wnd, const String& s)
{
	new FormDrainageNetworkExtraction(wnd, s.c_str());
	return -1;
}


FormDrainageNetworkExtraction::FormDrainageNetworkExtraction(CWnd* mw, const char* sPar)
:FormMapCreate(mw, TR("Drainage Network Extraction"))
{
		if (sPar) {
			TextInput inp(sPar);
			TokenizerBase tokenizer(&inp);
			String sVal;
				for (;;) {
					Token tok = tokenizer.tokGet();
					sVal = tok.sVal();
					if (sVal == "")
						break;
					FileName fn(sVal);
					if (fn.sExt == ".mpr" || fn.sExt == "") 
						if (m_sMap == "")
							m_sMap = fn.sFullNameQuoted(true);
						else
							sOutMap = fn.sFullName(false);
			}
	  }
		   
		new FieldDataType(root, TR("Flow &Accumulation Map"), &m_sMap, new MapListerDomainType(".mpr", dmVALUE, false),true);
		
		ValueRange vr1(1, LONG_MAX);
		StaticText *stThreshold = new StaticText(root, TR("Stream &Threshold (Nr. of Pixels)"));
		m_fldThreshold = new FieldInt(root, "", &m_iThreshold, vr1);
		m_fldThreshold->Align(stThreshold, AL_AFTER);


		m_fChkThresholdMap = false;
		m_chkThresholdMap = new CheckBox(root, TR("Use Stream Threshold &Map"), &m_fChkThresholdMap);
		m_chkThresholdMap->Align(stThreshold, AL_UNDER);
		m_chkThresholdMap->SetCallBack((NotifyProc)&FormDrainageNetworkExtraction::ThresholdMapOnSelect);
		m_fldThresholdMap = new FieldMap(root, "", &m_sThresholdMap, new MapListerDomainType(".mpr", dmVALUE, true));
		m_fldThresholdMap->Align(m_chkThresholdMap,AL_AFTER);

    FieldBlank *fb1 = new FieldBlank(root, 0); 
		fb1->Align(m_chkThresholdMap,AL_UNDER) ;
		m_fltFlowMap = new FieldDataType(root, TR("&Flow Direction Map"), &m_sFlowMap, new MapListerDomainName(FileName("FlowDirection.dom")), true);
		//m_fltFlowMap->Align(m_chkThresholdMap,AL_UNDER);
    m_fltFlowMap->Align(fb1,AL_UNDER);

		FieldBlank *fb2 = new FieldBlank(root, 0); // Used to force proper alignment
		fb2->Align(m_fltFlowMap,AL_UNDER) ;
		
		initMapOut(false, false);
		SetHelpItem("ilwisapp\\drainage_network_extraction_dialog_box.htm");
		create();
}

int FormDrainageNetworkExtraction::exec() 
{
  FormMapCreate::exec();
  FileName fn(sOutMap);
  FileName fnMap(m_sMap); 
  m_sMap = fnMap.sRelativeQuoted(false,fn.sPath());
	
	FileName fnFlowMap(m_sFlowMap); 
  m_sFlowMap = fnFlowMap.sRelativeQuoted(false,fn.sPath());

	FileName fnthMap(m_sThresholdMap); 
  m_sThresholdMap = fnthMap.sRelativeQuoted(false,fn.sPath());

	String sExpr;
	if (m_fChkThresholdMap)
	{
		
		sExpr = String("MapDrainageNetworkExtraction(%S,%S,%S )", 
									m_sMap, 
									m_sThresholdMap, m_sFlowMap);
	}
	else
	{
		sExpr = String("MapDrainageNetworkExtraction(%S,%li)", 
									m_sMap, 
									m_iThreshold);
	}	

	execMapOut(sExpr);  
  return 0;
}

int FormDrainageNetworkExtraction::ThresholdMapOnSelect(Event *)
{
		m_chkThresholdMap->StoreData();
		m_fldThresholdMap->StoreData();
		m_fltFlowMap->StoreData();
		if (!m_fChkThresholdMap)
		{
			m_fldThresholdMap->Hide();
			m_fldThreshold->Show();
      m_fltFlowMap->Hide();
		}
		else
		{
			m_fldThresholdMap->Show();
			m_fldThreshold->Hide();
      m_fltFlowMap->Show();
		}	
		return 1;
}

//////////////////////////////////////////////////////////////////////
// *** Drainage Network Ordering
//////////////////////////////////////////////////////////////////////
LRESULT Cmddrainagenetworkordering(CWnd *wnd, const String& s)
{
	new FormDrainageNetworkOrdering(wnd, s.c_str());
	return -1;
}

FormDrainageNetworkOrdering::FormDrainageNetworkOrdering(CWnd* mw, const char* sPar)
:FormMapCreate(mw, TR("Drainage Network Ordering"))
{
		if (sPar) {
			TextInput inp(sPar);
			TokenizerBase tokenizer(&inp);
			String sVal;
				for (;;) {
					Token tok = tokenizer.tokGet();
					sVal = tok.sVal();
					if (sVal == "")
						break;
					FileName fn(sVal);
					if (fn.sExt == ".mpr" || fn.sExt == "") 
						if (m_sMap == "")
							m_sMap = fn.sFullNameQuoted(true);
						else
							sOutMap = fn.sFullName(false);
			}
	  }
		new FieldDataType(root, TR("&Input DEM"), &m_sMap, new MapListerDomainType(".mpr", dmVALUE, false),true);
		new FieldDataType(root, TR("&Flow Direction Map"), &m_sFlowMap, new MapListerDomainName(FileName("FlowDirection.dom")), true);
		new FieldDataType(root, "Drainage Network Map", &m_sDrainageMap, new MapListerDomainType(".mpr", dmBOOL, false), true);
		
		m_rThreshold = rUNDEF;
		ValueRange vr1(0, 1e300, 1e-6);
		StaticText *stThreshold = new StaticText(root, TR("&Minimum Drainage Length"));
		FieldReal *fldThreshold = new FieldReal(root, "", &m_rThreshold, vr1);
		fldThreshold->Align(stThreshold, AL_AFTER);

		FieldBlank *fb = new FieldBlank(root, 0); // Used to force proper alignment
		fb->Align(stThreshold,AL_UNDER) ;
		initMapOut(false, false);

    StaticText *stSegmentMapRemark = new StaticText(root, TR("A segment vector map will be created with the same name as output raster"));
    stSegmentMapRemark->SetIndependentPos();
    SetHelpItem("ilwisapp\\drainage_network_ordering_dialog_box.htm");
		create();
}


int FormDrainageNetworkOrdering::exec() 
{
  FormMapCreate::exec();
  FileName fn(sOutMap);
  FileName fnMap(m_sMap); 
  m_sMap = fnMap.sRelativeQuoted(false,fn.sPath());

	FileName fnFlowMap(m_sFlowMap); 
  m_sFlowMap = fnFlowMap.sRelativeQuoted(false,fn.sPath());

	FileName fnDrainageMap(m_sDrainageMap); 
  m_sDrainageMap = fnDrainageMap.sRelativeQuoted(false,fn.sPath());

	String sExpr;
	sExpr = String("MapDrainageNetworkOrdering(%S,%S,%S, %g)", 
									m_sMap, 
									m_sFlowMap,
									m_sDrainageMap,
									m_rThreshold);

	execMapOut(sExpr);  
  return 0;
}

//////////////////////////////////////////////////////////////////////
// *** Catchment Extraction
//////////////////////////////////////////////////////////////////////

LRESULT Cmdcatchmentextraction(CWnd *wnd, const String& s)
{
	new FormCatchmentExtraction(wnd, s.c_str());
	return -1;
}

FormCatchmentExtraction::FormCatchmentExtraction(CWnd* mw, const char* sPar)
:FormMapCreate(mw, TR("Catchment Extraction"))
{
		if (sPar) {
			TextInput inp(sPar);
			TokenizerBase tokenizer(&inp);
			String sVal;
				for (;;) {
					Token tok = tokenizer.tokGet();
					sVal = tok.sVal();
					if (sVal == "")
						break;
					FileName fn(sVal);
					if (fn.sExt == ".mpr" || fn.sExt == "") 
						if (m_sDrainageMap == "")
							m_sDrainageMap = fn.sFullNameQuoted(true);
						else
							sOutMap = fn.sFullName(false);
			}
	  }

		new FieldDataType(root, "Drainage Network Ordering Map", &m_sDrainageMap, new MapListerDomainType(".mpr", dmIDENT, false), true);
		FieldDataType *fdt = new FieldDataType(root, TR("&Flow Direction Map"), &m_sFlowMap, new MapListerDomainName(FileName("FlowDirection.dom")), true);
		
		FieldBlank *fb = new FieldBlank(root, 0); // Used to force proper alignment
		fb->Align(fdt,AL_UNDER) ;
		initMapOut(false, false);
		SetHelpItem("ilwisapp\\catchment_extraction_dialog_box.htm");
		create();
}

int FormCatchmentExtraction::exec() 
{
  FormMapCreate::exec();
  FileName fn(sOutMap);
  FileName fnMap(m_sDrainageMap); 
  m_sDrainageMap = fnMap.sRelativeQuoted(false,fn.sPath());

	FileName fnFlowMap(m_sFlowMap); 
  m_sFlowMap = fnFlowMap.sRelativeQuoted(false,fn.sPath());

	String sExpr;
	sExpr = String("MapCatchmentExtraction(%S,%S)", 
									m_sDrainageMap, 
									m_sFlowMap);
									
	execMapOut(sExpr);  
  return 0;
}

//////////////////////////////////////////////////////////////////////
// *** Catchment Merge Form
//////////////////////////////////////////////////////////////////////

LRESULT Cmdcatchmentmerge(CWnd *wnd, const String& s)
{
	new FormCatchmentMerge(wnd, s.c_str());
	return -1;
}

FormCatchmentMerge::FormCatchmentMerge(CWnd* mw, const char* sPar)
:FormMapCreate(mw, TR("Catchment Merging"))
{
	if (sPar) {
		TextInput inp(sPar);
		TokenizerBase tokenizer(&inp);
		String sVal;
		for (;;) {
			Token tok = tokenizer.tokGet();
			sVal = tok.sVal();
			if (sVal == "")
				break;
			FileName fn(sVal);
			if (fn.sExt == ".mpr" || fn.sExt == "") 
				if (m_sDrainageMap == "")
					m_sDrainageMap = fn.sFullNameQuoted(true);
				else
					sOutMap = fn.sFullName(false);
		}
	}

	new FieldDataType(root, TR("&Drainage Network Ordering Map"), &m_sDrainageMap, new MapListerDomainType(".mpr", dmIDENT, false), true);
	FieldDataType *fdt = new FieldDataType(root, TR("&Flow Direction Map"), &m_sFlowMap, new MapListerDomainName(FileName("FlowDirection.dom")), true);
	new FieldDataType(root, TR("Flow &Accumulation Map"), &m_sFacc, new MapListerDomainType(".mpr", dmVALUE, true),true);
    new FieldDataType(root, String("DEM"), &m_sDEM, new MapListerDomainType(".mpr", dmVALUE, true),true);
	FieldBlank *fb1 = new FieldBlank(root, 0); // Used to force proper alignment

    m_iMergeOption = 0;
    m_iOrderSystem = 0;
    m_fExtractStreamNetwork = false;
    RadioGroup* rg = new RadioGroup(root, TR("&Merging Option"), &m_iMergeOption, true);
    
    RadioButton* rbOutletLocations = new RadioButton(rg, TR("&Use Outlet Locations"));
    rbOutletLocations->Align(rg, AL_UNDER);
    FieldPointMap *feMap = new FieldPointMap(rbOutletLocations, "", &m_sOutletMap, new MapListerDomainType(".mpp", 0, false));
    feMap->Align(rbOutletLocations, AL_AFTER);
	RadioButton* rbStreamOrders = new RadioButton(rg, TR("&Use Stream Orders"));
	rbStreamOrders->Align(rbOutletLocations, AL_UNDER);
		
    m_fUndefined = false;
    CheckBox* chkUndefind = new CheckBox(rbOutletLocations, "Include Undefined Pixels", &m_fUndefined);
    chkUndefind->Align(rbStreamOrders, AL_UNDER);
    
    ValueRange vr1(1, LONG_MAX);
	FieldInt *fldStreamOrders = new FieldInt(rbStreamOrders, "", &m_iStreamOrderd,vr1);
		
	FieldBlank *fb2 = new FieldBlank(root, 0); // Used to force proper alignment
	fb2->Align(rbStreamOrders, AL_UNDER);

	m_fExtractOriginalOrder = false;
	CheckBox* chkExtractOriginalOrder = new CheckBox(rbStreamOrders, "Extract Original Order", &m_fExtractOriginalOrder);
	chkExtractOriginalOrder->Align(rbStreamOrders, AL_UNDER);
    m_iOrderSystem = 0;
	RadioGroup* rgOrderSystem = new RadioGroup(rbStreamOrders, TR("&Ordering System"), &m_iOrderSystem, true);
	rgOrderSystem->Align(chkExtractOriginalOrder, AL_UNDER);
	RadioButton* rbStrahler = new RadioButton(rgOrderSystem, TR("&Strahler Order System"));
	rbStrahler->Align(rgOrderSystem, AL_UNDER);
	RadioButton* rbStreve = new RadioButton(rgOrderSystem, TR("&Shreve Order System"));
	rbStreve->Align(rgOrderSystem, AL_UNDER);
		
	FieldBlank *fb3 = new FieldBlank(root, 0); // Used to force proper alignment
	fb3->Align(rbStreve, AL_UNDER);
	initMapOut(false, false);

    m_fLongestSegmentMap = false;
    CheckBox* cbLSegmap = new CheckBox(root, TR("Longest Flow Path Segment Map"), &m_fLongestSegmentMap);
    FieldMapCreate* fmc = new FieldMapCreate(cbLSegmap, "", &m_sLongestFlowSegmentMap);
    
    m_fExtractStreamNetwork = false;
    m_chkSegExtractedMap = new CheckBox(rbOutletLocations, TR("&Extract Stream Segments and Attributes"), &m_fExtractStreamNetwork);
    m_chkSegExtractedMap->SetCallBack((NotifyProc)&FormCatchmentMerge::ExtractSegmentMapOnSelect);
		m_chkSegExtractedMap->Align(cbLSegmap, AL_UNDER);
    m_chkSegExtractedMap->SetIndependentPos();

    m_stSegmentMapRemark = new StaticText(rbOutletLocations, TR("Segment map and table obtain the same name as the output raster map"));
    m_stSegmentMapRemark->Align(m_chkSegExtractedMap, AL_UNDER);
    m_stSegmentMapRemark->SetIndependentPos();
    SetHelpItem("ilwisapp\\catchment_merge_dialog_box.htm");
	create();
}


int FormCatchmentMerge::exec() 
{
  FormMapCreate::exec();
  FileName fn(sOutMap);
  FileName fnDrainageMap(m_sDrainageMap); 
  m_sDrainageMap = fnDrainageMap.sRelativeQuoted(false,fn.sPath());

	FileName fnFlowMap(m_sFlowMap); 
  m_sFlowMap = fnFlowMap.sRelativeQuoted(false,fn.sPath());
  
  FileName fnFacc(m_sFacc); 
  m_sFacc = fnFacc.sRelativeQuoted(false,fn.sPath());

  if (m_fLongestSegmentMap) 
  {
    FileName fnLongestFlowSegmentMap(m_sLongestFlowSegmentMap);
    m_sLongestFlowSegmentMap = fnLongestFlowSegmentMap.sRelativeQuoted(false,fn.sPath());
  }
  else
    m_sLongestFlowSegmentMap = "";

	String sExpr;
  if (m_iMergeOption == 0)
  {
    if (m_sLongestFlowSegmentMap.length() == 0)
      sExpr = String("MapCatchmentMerge(%S,%S,%S,%S,%S,%li,%li)", 
									m_sDrainageMap, 
									m_sFlowMap,
                  m_sFacc,
                  m_sDEM,
                  m_sOutletMap,
                  m_fUndefined,
                  m_fExtractStreamNetwork);
    else 
      sExpr = String("MapCatchmentMerge(%S,%S,%S,%S,%S,%li,%li,%S)", 
									m_sDrainageMap, 
									m_sFlowMap,
                  m_sFacc,
                  m_sDEM,
                  m_sOutletMap,
                  m_fUndefined,
                  m_fExtractStreamNetwork,
                  m_sLongestFlowSegmentMap);
  }
  else
  {
    String sOrderSystem;
    if (m_iOrderSystem == 0)
      sOrderSystem = "Strahler";
    else
      sOrderSystem = "Shreve";

    if (m_sLongestFlowSegmentMap.length() == 0)
      sExpr = String("MapCatchmentMerge(%S,%S,%S,%S,%S,%li,,%li)", 
				  m_sDrainageMap, 
				  m_sFlowMap,
                  m_sFacc,
                  m_sDEM,
                  sOrderSystem,
                  m_iStreamOrderd,
				  m_fExtractOriginalOrder);
    else
      sExpr = String("MapCatchmentMerge(%S,%S,%S,%S,%S,%li,%li,%S)", 
				  m_sDrainageMap, 
				  m_sFlowMap,
                  m_sFacc,
                  m_sDEM,
                  sOrderSystem,
                  m_iStreamOrderd,
				  m_fExtractOriginalOrder,
                  m_sLongestFlowSegmentMap);
  }
									
  execMapOut(sExpr);  
  return 0;
}

int FormCatchmentMerge::ExtractSegmentMapOnSelect(Event *)
{
		m_chkSegExtractedMap->StoreData();
		if (!m_fExtractStreamNetwork || !m_chkSegExtractedMap->fShow() )
		{
			m_stSegmentMapRemark->Hide();
		}
		else
		{
			m_stSegmentMapRemark->Show();
		}	
		return 1;
}


//////////////////////////////////////////////////////////////////////
// *** Slope Lengths
//////////////////////////////////////////////////////////////////////

LRESULT Cmdoverlandflowlength(CWnd *wnd, const String& s)
{
	new FormSlopeLengths(wnd, s.c_str());
	return -1;
}

FormSlopeLengths::FormSlopeLengths(CWnd* mw, const char* sPar)
:FormMapCreate(mw, TR("Overland Flow Length"))
{
		if (sPar) {
			TextInput inp(sPar);
			TokenizerBase tokenizer(&inp);
			String sVal;
				for (;;) {
					Token tok = tokenizer.tokGet();
					sVal = tok.sVal();
					if (sVal == "")
						break;
					FileName fn(sVal);
					if (fn.sExt == ".mpr" || fn.sExt == "") 
						if (m_sDrainageMap == "")
							m_sDrainageMap = fn.sFullNameQuoted(true);
						else
							sOutMap = fn.sFullName(false);
			}
	  }

		new FieldDataType(root, TR("&Drainage Network Ordering Map"), &m_sDrainageMap, new MapListerDomainType(".mpr", dmIDENT, false), true);
		FieldDataType *fdt = new FieldDataType(root, TR("&Flow Direction Map"), &m_sFlowMap, new MapListerDomainName(FileName("FlowDirection.dom")), true);
		
		FieldBlank *fb = new FieldBlank(root, 0); // Used to force proper alignment
		fb->Align(fdt,AL_UNDER) ;
		initMapOut(false, false);
		SetHelpItem("ilwisapp\\overland_flow_length_dialog_box.htm");
		create();
}


int FormSlopeLengths::exec() 
{
  FormMapCreate::exec();
  FileName fn(sOutMap);
  FileName fnMap(m_sDrainageMap); 
  m_sDrainageMap = fnMap.sRelativeQuoted(false,fn.sPath());

	FileName fnFlowMap(m_sFlowMap); 
  m_sFlowMap = fnFlowMap.sRelativeQuoted(false,fn.sPath());

	String sExpr;
	sExpr = String("MapOverlandFlowLength(%S,%S)", 
									m_sDrainageMap, 
									m_sFlowMap);
									
	execMapOut(sExpr);  
  return 0;
}

//////////////////////////////////////////////////////////////////////
// *** Flow Length to Outlet
//////////////////////////////////////////////////////////////////////
LRESULT Cmdflowlengthtooutlet(CWnd *wnd, const String& s)
{
	new FormFlowLength2Outlet(wnd, s.c_str());
	return -1;
}

FormFlowLength2Outlet::FormFlowLength2Outlet(CWnd* mw, const char* sPar)
:FormMapCreate(mw, "Flow Length to Outlet")
{
		if (sPar) {
			TextInput inp(sPar);
			TokenizerBase tokenizer(&inp);
			String sVal;
				for (;;) {
					Token tok = tokenizer.tokGet();
					sVal = tok.sVal();
					if (sVal == "")
						break;
					FileName fn(sVal);
					if (fn.sExt == ".mpr" || fn.sExt == "") 
						if (m_sDrainageMap == "")
							m_sDrainageMap = fn.sFullNameQuoted(true);
						else
							sOutMap = fn.sFullName(false);
			}
	  }

		new FieldDataType(root, TR("&Drainage Network Ordering Map"), &m_sDrainageMap, new MapListerDomainType(".mpr", dmIDENT, false), true);
		FieldDataType *fdt = new FieldDataType(root, TR("&Flow Direction Map"), &m_sFlowMap, new MapListerDomainName(FileName("FlowDirection.dom")), true);
		
		FieldBlank *fb = new FieldBlank(root, 0); // Used to force proper alignment
		fb->Align(fdt,AL_UNDER) ;
		initMapOut(false, false);
		create();
}


int FormFlowLength2Outlet::exec() 
{
  FormMapCreate::exec();
  FileName fn(sOutMap);
  FileName fnMap(m_sDrainageMap); 
  m_sDrainageMap = fnMap.sRelativeQuoted(false,fn.sPath());

	FileName fnFlowMap(m_sFlowMap); 
  m_sFlowMap = fnFlowMap.sRelativeQuoted(false,fn.sPath());

	String sExpr;
	sExpr = String("MapFlowLength2Outlet(%S,%S)", 
									m_sDrainageMap, 
									m_sFlowMap);
									
	execMapOut(sExpr);  
  return 0;
}

//////////////////////////////////////////////////////////////////////
// *** Parameters for Horton Plots
//////////////////////////////////////////////////////////////////////

LRESULT Cmdhortonplots(CWnd *wnd, const String& s)
{
	new FormHortonPlots(wnd, s.c_str());
	return -1;
}

FormHortonPlots::FormHortonPlots(CWnd* mw, const char* sPar)
:FormTableCreate(mw, TR("Parameters for Horton Plots"))
{
		if (sPar) {
			TextInput inp(sPar);
			TokenizerBase tokenizer(&inp);
			String sVal;
				for (;;) {
					Token tok = tokenizer.tokGet();
					sVal = tok.sVal();
					if (sVal == "")
						break;
					FileName fn(sVal);
					if (fn.sExt == ".mpr" || fn.sExt == "") 
						if (m_sDrainageMap == "")
							m_sDrainageMap = fn.sFullNameQuoted(true);
						else
							sOutTable = fn.sFullName(false);
			}
	  }

		new FieldDataType(root, TR("&Catchment Map"), &m_sCatchmentMap, new MapListerDomainType(".mpr", dmIDENT, false), true);
    new FieldDataType(root, "Merged Catchment Map", &m_sMergedCatchmentMap, new MapListerDomainType(".mpr", dmVALUE, true), true);
		FieldDataType *fdt = new FieldDataType(root, TR("&Drainage Network Ordering Map"), &m_sDrainageMap, new MapListerDomainType(".mpr", dmVALUE, true), true);
		
		FieldBlank *fb = new FieldBlank(root, 0); // Used to force proper alignment
		fb->Align(fdt,AL_UNDER) ;
		initTableOut(false);
		SetHelpItem("ilwisapp\\horton_statistics_dialog_box.htm");
		create();
}


int FormHortonPlots::exec() 
{
  FormTableCreate::exec();
  FileName fn(sOutTable);
  FileName fnTable(m_sCatchmentMap); 
  m_sCatchmentMap = fnTable.sRelativeQuoted(false,fn.sPath());

  FileName fnMergedCatchmentMap(m_sMergedCatchmentMap); 
  m_sMergedCatchmentMap = fnMergedCatchmentMap.sRelativeQuoted(false,fn.sPath());

	FileName fnDrainageMap(m_sDrainageMap); 
  m_sDrainageMap = fnDrainageMap.sRelativeQuoted(false,fn.sPath());

	String sExpr;
	sExpr = String("TableHortonPlots(%S,%S,%S)", 
									m_sCatchmentMap, 
                  m_sMergedCatchmentMap,
									m_sDrainageMap);
									
	execTableOut(sExpr);  
  return 0;
}

//////////////////////////////////////////////////////////////////////
// *** Flow Path Longitudinal Profile
//////////////////////////////////////////////////////////////////////
LRESULT Cmdflowpathlongitudinalprofile(CWnd *wnd, const String& s)
{
	new FormFlowPathLongitudinalProfile(wnd, s.c_str());
	return -1;
}


FormFlowPathLongitudinalProfile::FormFlowPathLongitudinalProfile(CWnd* mw, const char* sPar)
:FormTableCreate(mw, "Flow Path Longitudinal Profile")
{
	if (sPar) {
		TextInput inp(sPar);
		TokenizerBase tokenizer(&inp);
		String sVal;
		for (;;) {
			Token tok = tokenizer.tokGet();
			sVal = tok.sVal();
			if (sVal == "")
				break;
			FileName fn(sVal);
			if (fn.sExt == ".mps" || fn.sExt == "") 
				if (m_sSegMap == "")
					m_sSegMap = fn.sFullNameQuoted(true);
				else
					sOutTable = fn.sFullName(false);
		}
	 }

	 new FieldDataType(root, "Flow Path Segment Map", &m_sSegMap, new MapListerDomainType(".mps", dmIDENT, false), true);
	 ValueRange vr1(1, LONG_MAX);
	 FieldInt *fldSegID = new FieldInt(root, "Segment ID", &m_iSegID,vr1);
	 ValueRange vr2(0, 1e300, 1e-6);
	 FieldReal *fldDist = new FieldReal(root, "Distance", &m_rDist, vr2);
	 new FieldDataType(root, "Attribute Map", &m_sAttMap, new MapListerDomainType(".mpr", dmVALUE, true), true);
	 FieldBlank *fb = new FieldBlank(root, 0); // Used to force proper alignment
	initTableOut(false);
		//SetHelpItem("ilwisapp\\horton_statistics_dialog_box.htm");
	create();
}

int FormFlowPathLongitudinalProfile::exec() 
{
  FormTableCreate::exec();
  FileName fn(sOutTable);
  FileName fnTable(m_sSegMap); 
  m_sSegMap = fnTable.sRelativeQuoted(false,fn.sPath());

  FileName fnAttMap(m_sAttMap); 
  m_sAttMap = fnAttMap.sRelativeQuoted(false,fn.sPath());

  String sExpr;
  sExpr = String("tblFlowPathLongitudinalProfile(%S,%li,%.2f,%S)", 
				  m_sSegMap, 
                  m_iSegID,
				  m_rDist,
				  m_sAttMap);
									
  execTableOut(sExpr);  
  return 0;
}

//////////////////////////////////////////////////////////////////////
// *** Topological Optimization
//////////////////////////////////////////////////////////////////////
LRESULT Cmdtopologicaloptimization(CWnd *wnd, const String& s)
{
	new FormTopologicalOptimization(wnd, s.c_str());
	return -1;
}

FormTopologicalOptimization::FormTopologicalOptimization(CWnd* mw, const char* sPar)
:FormMapCreate(mw, TR("Topological Optimization"))
{
		if (sPar) {
			TextInput inp(sPar);
			TokenizerBase tokenizer(&inp);
			String sVal;
				for (;;) {
					Token tok = tokenizer.tokGet();
					sVal = tok.sVal();
					if (sVal == "")
						break;
					FileName fn(sVal);
					if (fn.sExt == ".mpr" || fn.sExt == "") 
						if (m_sDrainageMap == "")
							m_sDrainageMap = fn.sFullNameQuoted(true);
						else
							sOutMap = fn.sFullName(false);
			}
	  }
    
    new FieldDataType(root, "Drainage Network Map", &m_sDrainageMap, new MapListerDomainType(".mpr", dmBOOL, false), true);
		new FieldDataType(root, TR("&Flow Direction Map"), &m_sFlowMap, new MapListerDomainName(FileName("FlowDirection.dom")), true);
		
    FieldDataType *fdt = new FieldDataType(root, TR("Stream Segment Map"), &m_sStreamSegMap, new MapListerDomainType(".mps",0,false),true);
		FieldBlank *fb = new FieldBlank(root, 0); // Used to force proper alignment
		fb->Align(fdt,AL_UNDER) ;
		initMapOut(false, false);

    FieldMapCreate* fmc = new FieldMapCreate(root, TR("Modified Flow Direction Map"), &m_sModifiedFlowMap);
		SetHelpItem("ilwisapp\\topological_optimization_dialog_box.htm");
		create();
}


int FormTopologicalOptimization::exec() 
{
  FormMapCreate::exec();
  FileName fn(sOutMap);
  FileName fnMap(m_sDrainageMap); 
  m_sDrainageMap = fnMap.sRelativeQuoted(false,fn.sPath());

	FileName fnFlowMap(m_sFlowMap); 
  m_sFlowMap = fnFlowMap.sRelativeQuoted(false,fn.sPath());

  FileName fnStreamMap(m_sStreamSegMap); 
  m_sStreamSegMap = fnStreamMap.sRelativeQuoted(false,fn.sPath());

  FileName fnModifiedFlowMap(m_sModifiedFlowMap); 
  m_sModifiedFlowMap = fnModifiedFlowMap.sRelativeQuoted(false,fn.sPath());


	String sExpr;
	sExpr = String("MapTopologicalOptimization(%S,%S,%S,%S)", 
									m_sDrainageMap, 
									m_sFlowMap,
                  m_sStreamSegMap,
                  m_sModifiedFlowMap);
									
	execMapOut(sExpr);  
  return 0;
}

//////////////////////////////////////////////////////////////////////
// *** Variable Threshold Computation
//////////////////////////////////////////////////////////////////////
LRESULT Cmdvariablethresholdcomputation(CWnd *wnd, const String& s)
{
	new FormVariableThresholdComputation(wnd, s.c_str());
	return -1;
}

FormVariableThresholdComputation::FormVariableThresholdComputation(CWnd* mw, const char* sPar)
:FormMapCreate(mw, "Variable Threshold Computation")
{
		if (sPar) {
			TextInput inp(sPar);
			TokenizerBase tokenizer(&inp);
			String sVal;
				for (;;) {
					Token tok = tokenizer.tokGet();
					sVal = tok.sVal();
					if (sVal == "")
						break;
					FileName fn(sVal);
					if (fn.sExt == ".mpr" || fn.sExt == "") 
						if (m_sMap == "")
							m_sMap = fn.sFullNameQuoted(true);
						else
							sOutMap = fn.sFullName(false);
			}
	  }
    m_AddString = false;
		new FieldDataType(root, TR("&Input DEM"), &m_sMap, new MapListerDomainType(".mpr", dmVALUE, false), true);
		ValueRange vr1(1, LONG_MAX);
    m_iMax = 5;
    m_iFilterSize = 3;
		FieldInt *fldFilterSize = new FieldInt(root, "Filter Size", &m_iFilterSize,vr1);
    ValueRange vr2(1, m_iMax);
    StaticText *stClasses = new StaticText(root,"Nr. of Classes");
    m_fldClasses = new FieldOneSelectTextOnly(root, &m_sClasses, false);
    m_fldClasses->Align(stClasses, AL_AFTER);
    m_fldClasses->SetCallBack((NotifyProc)&FormVariableThresholdComputation::NrofClassesOnChange);
    
    StaticText *stUpperboundthreshold = new StaticText(root, "Upper Bound Threshold");
    stUpperboundthreshold->Align(stClasses, AL_UNDER);
    FieldBlank *fb1 = new FieldBlank(root, 1);
    fb1->Align(stUpperboundthreshold, AL_AFTER);

    m_vFld.resize(m_iMax);
    m_vRowCol.resize(m_iMax);
    for (int i = 0; i < m_iMax; ++i)
    {
      m_vFld[i] = new FieldRowCol(root, "", &m_vRowCol[i]);
    }
    m_vFld[0]->Align(fb1, AL_UNDER);
    FieldBlank *fb = new FieldBlank(root, 0); // Used to force proper alignment
		fb->Align(root,AL_UNDER);
    initMapOut(false, false);
    m_fReliefMap = false;
    CheckBox *chkMap = new CheckBox(root, "Create internal relief map", &m_fReliefMap);
    StaticText *st = new StaticText(chkMap, "The map obtains unique name to the specified output map");
    st->Align(chkMap,AL_UNDER);
    st->SetIndependentPos();
    SetHelpItem("ilwisapp\\variable_threshold_computation_dialog_box.htm");
		create();
}


int FormVariableThresholdComputation::exec() 
{
  FormMapCreate::exec();
  FileName fn(sOutMap);
  FileName fnMap(m_sMap); 
  m_sMap = fnMap.sRelativeQuoted(false,fn.sPath());

	String sExpr;
  String sThreshold;
  for(int i=0; i<m_iClasses; i++)
  {
    sThreshold &= String("%li,%li",m_vRowCol[i].Row,m_vRowCol[i].Col);   
    sThreshold &= (i == m_iClasses-1) ? "" : ",";
  }
  
	sExpr = String("MapVariableThresholdComputation(%S,%li,%li,\"%S\",%li)", 
									m_sMap, 
									m_iFilterSize,
                  m_iClasses,
                  sThreshold,
                  m_fReliefMap); 
									
	execMapOut(sExpr);  
  return 0;
}

int FormVariableThresholdComputation::NrofClassesOnChange(Event *)
{
  m_fldClasses->StoreData();
  if(m_AddString == false)
  {
    for(int i=1; i<=m_iMax; ++i)
    {
      m_fldClasses->AddString(String("%li",i));
    }
    m_sClasses = String("%li",m_iMax);
    m_fldClasses->SelectItem(m_sClasses);
    m_AddString = true;
  }
  m_iClasses = m_sClasses.iVal();
	for(int i=0; i<m_iMax; ++i)
  {
      if (i < m_iClasses)
        m_vFld[i]->Show();
      else
        m_vFld[i]->Hide();
  }
	return 1;
}
