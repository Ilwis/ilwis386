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
//
//////////////////////////////////////////////////////////////////////

//#include "AdditionalHeaders\Pch\AppFormsPCH.h"
//#include "ApplicationForms\FormMapFlowDirection.h"
//#include "AdditionalHeaders\Shared\messages.h"
//#include "Main/ilwis.h"
//#include "AdditionalHeaders\Shared\Constant.h"
#include "Client\Headers\AppFormsPCH.h"
#include "HydroFlowApplicationsUI\FormMapFlowDirection.h"
#include "Headers\messages.h"
#include "Client\ilwis.h"
#include "Headers\constant.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

LRESULT Cmdfillsinks(CWnd *wnd, const String& s)
{
	new FormMapFillSinks(wnd, s.scVal());
	return -1;
}

FormMapFillSinks::FormMapFillSinks(CWnd* mw, const char* sPar)
:FormMapCreate(mw, SAFTitleFlowFillSinks)
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
		new FieldMap(root, SAFUiInputDEM, &m_sMap, new MapListerDomainType(".mpr", dmVALUE, false));
		m_iMethod = 0;
		RadioGroup* rg = new RadioGroup(root, "Method", &m_iMethod, true);
		RadioButton* rbFill = new RadioButton(rg, "Fill sink");
		RadioButton* rbCut = new RadioButton(rg, "Cut terrain");
		rbCut->Align(rbFill, AL_UNDER);
		initMapOut(false, false);
		SetAppHelpTopic(htpFillSinks);
		create();
}


int FormMapFillSinks::exec() 
{
	FormMapCreate::exec();
	FileName fn(sOutMap);
	FileName fnMap(m_sMap); 
	m_sMap = fnMap.sRelativeQuoted(false,fn.sPath());

	String sMethod;
	if (m_iMethod == 0)
		sMethod = "fill";
	else
		sMethod = "cut";


	String sExpr = String("MapFillSinks(%S,%S)",m_sMap, sMethod);
	execMapOut(sExpr);  
	return 0;
}
LRESULT Cmdflowdirection(CWnd *wnd, const String& s)
{
	new FormMapFlowDirection(wnd, s.scVal());
	return -1;
}

FormMapFlowDirection::FormMapFlowDirection(CWnd* mw, const char* sPar)
:FormMapCreate(mw, SAFTitleFlowDirection)
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
						if (sMap == "")
							sMap = fn.sFullNameQuoted(true);
						else
							sOutMap = fn.sFullName(false);
			}
	  }
		   
		new FieldDataType(root, SAFUiInputDEM, &sMap, new MapListerDomainType(".mpr", dmVALUE, false),true);
    //*FieldBlank *fb1 = new FieldBlank(root, 0); // Used to force proper alignment
		m_iMethod = 0;
		RadioGroup* rg = new RadioGroup(root, SAFUiFlowMethod, &m_iMethod, true);
		RadioButton* rbSlope = new RadioButton(rg, SAFUiSlope);
    RadioButton* rbHeight = new RadioButton(rg, SAFUiHeight);
		rbHeight->Align(rbSlope, AL_UNDER);
	  m_fParallelDrainageCorrection = false;
    m_chkParallelDrainageCorrection = new CheckBox(root, "Parallel drainage correction algorithm", &m_fParallelDrainageCorrection);
    m_chkParallelDrainageCorrection->SetIndependentPos();
    //m_chkParallelDrainageCorrection->SetCallBack((NotifyProc)&FormMapFlowDirection::PDCOnSelect);
    FieldBlank *fb = new FieldBlank(root, 0); // Used to force proper alignment	
    fb->Align(m_chkParallelDrainageCorrection, AL_UNDER);
    initMapOut(false, false);
    SetAppHelpTopic(htpFlowDirection);
		create();
}

unsigned int GetMemorySize()
{
	MEMORYSTATUS memoryStatus;

	memset (&memoryStatus, sizeof (MEMORYSTATUS), 0);
	memoryStatus.dwLength = sizeof (MEMORYSTATUS);

	GlobalMemoryStatus (&memoryStatus);
	return memoryStatus.dwTotalPhys;

}

int FormMapFlowDirection::exec() 
{
  FormMapCreate::exec();
  FileName fn(sOutMap,".mpr", TRUE);
  FileName fnMap(sMap); 
  sMap = fnMap.sRelativeQuoted(false,fn.sPath());

	String sMethod;
	if (m_iMethod == 0)
		sMethod = "Slope";
	else
		sMethod = "Height";

	if (m_fParallelDrainageCorrection) {
		sMethod &= ",Parallel";
	}
    String sExpr = String("MapFlowDirection(%S,%S)",sMap, sMethod);
	execMapOut(sExpr);
  return 0;
}

int FormMapFlowDirection::PDCOnSelect(Event *)
{
    m_chkParallelDrainageCorrection->StoreData();
    if (m_fParallelDrainageCorrection){
      butDefine.EnableWindow(FALSE); 
      butHelp.EnableWindow(FALSE); 
    }
    else{
      butHelp.EnableWindow(TRUE); 
      butDefine.EnableWindow(TRUE); 
    }

		return 1;
}

LRESULT Cmdflowaccumulation(CWnd *wnd, const String& s)
{
	new FormMapFlowAccumulation(wnd, s.scVal());
	return -1;
}

FormMapFlowAccumulation::FormMapFlowAccumulation(CWnd* mw, const char* sPar)
:FormMapCreate(mw, SAFTitleFlowAccumulation)
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
						if (sMap == "")
							sMap = fn.sFullNameQuoted(true);
						else
							sOutMap = fn.sFullName(false);
			}
	  }
		new FieldDataType(root, SAFUiInputFlowDirection, &sMap, new MapListerDomainName(FileName("FlowDirection.dom")), true);
		
		initMapOut(false, false);
		SetAppHelpTopic(htpFlowAccumulation);
		create();
}


int FormMapFlowAccumulation::exec() 
{
  FormMapCreate::exec();
  FileName fn(sOutMap);
  FileName fnMap(sMap); 
  sMap = fnMap.sRelativeQuoted(false,fn.sPath());

	
  String sExpr = String("MapFlowAccumulation(%S)",sMap);

  execMapOut(sExpr);  
  return 0;
}

LRESULT Cmddemoptimization(CWnd *wnd, const String& s)
{
	new FormMapDemOptimization(wnd, s.scVal());
	return -1;
}

FormMapDemOptimization::FormMapDemOptimization(CWnd* mw, const char* sPar)
:FormMapCreate(mw, SAFTitleDemOptimization)
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
		m_DrainageMapType =  dmtSegment;
		
		m_fldmp = new FieldMap(root, SAFUiInputDEM, &m_sMap, new MapListerDomainType(".mpr", dmVALUE, false));
		
		m_fdtDrainage = new FieldDataType(root, SAFUiDrainageMap, &m_sDrainageMap, new MapListerDomainType(".mps.mpa",0,false),true);
		m_fdtDrainage->SetCallBack((NotifyProc)&FormMapDemOptimization::DrainageMap);
		
		m_fChkAttTbl = false;
		m_chkAttTbl = new CheckBox(root, SAFUiTable, &m_fChkAttTbl);
		m_chkAttTbl->SetCallBack((NotifyProc)&FormMapDemOptimization::TableFlagOnSelect);
		m_ftbl = new FieldTable(m_chkAttTbl, "", &m_sTbl);
		m_ftbl->Align(m_chkAttTbl,AL_AFTER);
		m_ftbl->SetCallBack((NotifyProc)&FormMapDemOptimization::TableOnSelect);

		m_stBufDist = new StaticText(root, SAFUiBufferDistance);
		m_stBufDist->Align(m_chkAttTbl,AL_UNDER);
		m_fldColBufDist = new FieldColumn(m_chkAttTbl, "", Table(),&m_sBufDist, dmVALUE);
		m_fldColBufDist->Align(m_stBufDist, AL_AFTER);
		
		m_stSmooth = new StaticText(root, SAFUiSmoothDrop);
		m_stSmooth->Align(m_stBufDist, AL_UNDER);
		m_fldColSmooth = new FieldColumn(m_chkAttTbl,"", Table(), &m_sSmooth, dmVALUE);
		m_fldColSmooth->Align(m_stSmooth, AL_AFTER);
		
		StaticText *stSharp = new StaticText(root, SAFUiSharpDrop);
		stSharp->Align(m_stSmooth, AL_UNDER);
		m_fldColSharp = new FieldColumn(m_chkAttTbl,"", Table(), &m_sSharp, dmVALUE);
		m_fldColSharp->Align(stSharp, AL_AFTER);
		
		m_rBufDist = rUNDEF;
		m_rSmooth = rUNDEF;
		m_rSharp = rUNDEF;
		ValueRange vr1(0, 1e300, 1e-6);
		m_fldBufDist = new FieldReal(root, "", &m_rBufDist, vr1);
		m_fldBufDist->Align(m_stBufDist, AL_AFTER);
		
		ValueRange vr2(-1e300, 1e300, 1e-6);
		m_fldSmooth = new FieldReal(root, "", &m_rSmooth, vr2);
		m_fldSmooth->Align(m_stSmooth, AL_AFTER);
		
		m_fldSharp = new FieldReal(root, "", &m_rSharp, vr2);
		m_fldSharp->Align(stSharp, AL_AFTER);

		FieldBlank *fb = new FieldBlank(root, 0); // Used to force proper alignment
		fb->Align(stSharp,AL_UNDER) ;
		
		initMapOut(false, false);
		SetAppHelpTopic(htpDemOptimization);
		create();
}

int FormMapDemOptimization::exec() 
{
  FormMapCreate::exec();
	String sExpr;
	Map mp(m_sMap);
	FileName fn(sOutMap);

  switch (m_DrainageMapType)
	{
		case dmtSegment:
			if (m_fChkAttTbl)
			{
							FileName fnTbl(m_sTbl);
							m_sTbl = fnTbl.sRelativeQuoted(false,fn.sPath());
							sExpr = String("MapDEMOptimization(%S,%S,%S,%S,%S,%S)", 
									mp->fnObj.sRelativeQuoted(), 
									m_sm->fnObj.sRelativeQuoted(),
									m_sTbl,
									m_sBufDist, 
									m_sSmooth, 
									m_sSharp);
			}
			else
							sExpr = String("MapDEMOptimization(%S,%S,%g,%g,%g)", 
									mp->fnObj.sRelativeQuoted(), 
									m_sm->fnObj.sRelativeQuoted(),
									m_rBufDist, 
									m_rSmooth, 
									m_rSharp);
			break;
		case dmtPolygon:
			PolygonMap pm = PolygonMap(m_sDrainageMap);
			sExpr = String("MapDEMOptimization(%S,%S,%g)", 
									mp->fnObj.sRelativeQuoted(), 
									pm->fnObj.sRelativeQuoted(),
									m_rSharp);

			break;
	}
  

  execMapOut(sExpr);  
  return 0;
}

int FormMapDemOptimization::DrainageMap(Event *)
{
	m_fdtDrainage->StoreData();
	m_chkAttTbl->StoreData();
	
	m_chkAttTbl->disable();
	m_chkAttTbl->SetVal(false);
	m_fChkAttTbl = false;
				
	FileName fn = FileName(m_sDrainageMap);
	if (fCIStrEqual(fn.sExt, ".mps"))
	{
			m_stBufDist->Show();
			m_stSmooth->Show();
			m_DrainageMapType =  dmtSegment;
			m_sm = SegmentMap(m_sDrainageMap);
			m_ftbl->SetDomain(m_sm->dm());
			if (m_sm->fTblAtt() || m_sm->dm()->dmt() == dmtCLASS  
													|| m_sm->dm()->dmt() == dmIDENT 
													|| m_sm->dm()->dmt() == dmUNIQUEID)
			{
				m_chkAttTbl->enable();
				if (m_sm->fTblAtt() || m_fChkAttTbl )
				{
					ShowCols();
					if (m_sm->fTblAtt())
					{
						Table tblAtt = m_sm->tblAtt();
						m_ftbl->SetVal(tblAtt->fnObj.sFullPathQuoted());
						FillCols();
					}				
				}
				else
					ShowVals(); //table flag is off
			}
			else
			{
				m_chkAttTbl->disable();
				ShowVals();
			}
	}
	else if (fCIStrEqual(fn.sExt, ".mpa"))
	{
		m_stBufDist->Hide();
		m_stSmooth->Hide();
		m_DrainageMapType =  dmtPolygon;
		m_chkAttTbl->disable(); 
		ShowVals();
		m_fldBufDist->Hide();
		m_fldSmooth->Hide();
	}
	else
	{
		m_stBufDist->Show();
		m_stSmooth->Show();
	}
			
	return 1;
}

int FormMapDemOptimization::TableFlagOnSelect(Event *)
{
		m_chkAttTbl->StoreData();
		m_ftbl->StoreData();
		if (!m_fChkAttTbl)
			ShowVals();
		else
			ShowCols();	
		return 1;
}

int FormMapDemOptimization::TableOnSelect(Event *)
{
		m_ftbl->StoreData();
	  m_chkAttTbl->StoreData();
		if (m_fChkAttTbl)
		{
			FillCols();
				return 0;
		}
		return 1;
}

int FormMapDemOptimization::SetDefDomain(Event *)
{
		//set the default domain and value range for the output map 
		m_fldmp->StoreData();

  	try {
      FileName fnMap(m_sMap);
      if (fnMap.sFile == "")
        return 0;
      Map mp(m_sMap);
			fdc->SetVal(mp->dm()->fnObj.sFullPath());
			RangeReal rrInput  = mp->rrMinMax();
			ValueRange vr =ValueRange(rrInput.rLo(), rrInput.rHi(), mp->dvrs().rStep()) ;
      SetDefaultValueRange(vr);
  	}
    catch (ErrorObject&) {}
    return 0;
  
}

void FormMapDemOptimization::ShowCols()
{
			m_chkAttTbl->SetVal(true);
			m_fldBufDist->Hide();
			m_fldSmooth->Hide();
			m_fldSharp->Hide();
			m_fldColBufDist->Show();
			m_fldColSmooth->Show();
			m_fldColSharp->Show();
			m_ftbl->Show();

}

void FormMapDemOptimization::ShowVals()
{
			m_chkAttTbl->SetVal(false);
			m_fldBufDist->Show();
			m_fldSmooth->Show();
			m_fldSharp->Show();
			m_fldColBufDist->Hide();
			m_fldColSmooth->Hide();
			m_fldColSharp->Hide();
			m_ftbl->Hide();
}

void FormMapDemOptimization::FillCols()
{
		if (m_sTbl == "")
			return ;

		Table tblAtt = Table(m_sTbl);
		long iCols = tblAtt->iCols();
		m_fldColBufDist->FillWithColumns(tblAtt.ptr());
		m_fldColSmooth->FillWithColumns(tblAtt.ptr());
		m_fldColSharp->FillWithColumns(tblAtt.ptr());

		for (long iCol = 0; iCol < iCols; iCol++)
		{
			
				Column Col = tblAtt->col(iCol);
				Domain dm = Col->dm();
				if (dm->dmt() == dmtVALUE)
				{
					String sColName = Col->sName();
					if (fCIStrEqual(sColName, "buffer_dist"))
							m_fldColBufDist->SetVal("buffer_dist");
					if (fCIStrEqual(sColName, "smooth_drop"))
							m_fldColSmooth->SetVal("smooth_drop");
					if (fCIStrEqual(sColName, "sharp_drop"))
							m_fldColSharp->SetVal("sharp_drop");
					
				}
		}
	
}