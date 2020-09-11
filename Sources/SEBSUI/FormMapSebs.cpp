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

 Created on: 2007-06-9 by Lichun Wang
 ***************************************************************/
// FormMapSebs.cpp: implementation of the FormMapSebs class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\AppFormsPCH.h"
#include "Client\Forms\frmmapcr.h"
#include "Client\FormElements\FldOneSelectTextOnly.h"
#include "Client\FormElements\NonIlwisObjectLister.h"
#include "Client\FormElements\fldcol.h"
#include "Client\FormElements\fldtbl.h"
#include "SebsUI\FormMapSebs.h"
#include "Engine\Base\File\Directory.h"
#include "Client\FormElements\FieldBrowseDir.h"
#include "Headers\messages.h"
#include "Client\ilwis.h"
#include "Headers\constant.h"
#include "Client\FormElements\DatFileLister.h"
#include "Engine\Base\System\engine.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

LRESULT Cmdmapeto(CWnd *wnd, const String& s)
{
	new FormMapETo(wnd, s.c_str());
	return -1;
} 

FormMapETo::FormMapETo(CWnd* mw, const char* sPar)
:FormMapCreate(mw, TR("Reference ETo"))
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
					if (m_sU2 == "")
						m_sU2 = fn.sFullNameQuoted(true);
					else
						sOutMap = fn.sFullName(false);
		}
	}
	ValueRange vr(-1e300, 1e300, 1e-2);		   
	new FieldDataType(root, TR("Average daily wind speed map at 2m (m/s)"), &m_sU2, new MapListerDomainType(".mpr", dmVALUE, false),true);
	m_iEa_RH = 0;
	rg_Ea_RH = new RadioGroup(root, "", &m_iEa_RH, true);
	rg_Ea_RH->SetCallBack((NotifyProc)&FormMapETo::MapEaRHOnSelect);
	RadioButton* rbEa = new RadioButton(rg_Ea_RH, TR("Daily actual water vapor pressure (kPa)"));
	RadioButton* rbRH = new RadioButton(rg_Ea_RH, TR("Daily relative humidity (%)"));
	m_useEa = true;
	m_useEa_max_map = false;
	m_chkEa_max_map = new CheckBox(root, TR("Max. daily actual water vapor pressure map (kPa)"), &m_useEa_max_map);
	m_chkEa_max_map->SetCallBack((NotifyProc)&FormMapETo::MapEaMaxOnSelect);
	m_chkEa_max_map->Align(rbEa,AL_UNDER);
	m_rEa_max=rUNDEF;
	m_fdtEa_max_map = new FieldDataType(m_chkEa_max_map, "", &m_sEa_max, new MapListerDomainType(".mpr", dmVALUE, false),true);
	m_fldEa_max = new FieldReal(root, "", &m_rEa_max, vr);
	m_fldEa_max->Align(m_chkEa_max_map,AL_AFTER);
	
	m_useEa_min_map = false;
	m_chkEa_min_map = new CheckBox(root, TR("Min. daily actual water vapor pressure map (kPa)"), &m_useEa_min_map);
	m_chkEa_min_map->SetCallBack((NotifyProc)&FormMapETo::MapEaMinOnSelect);
	m_chkEa_min_map->Align(m_chkEa_max_map,AL_UNDER);
	m_fdtEa_min_map = new FieldDataType(m_chkEa_min_map, "", &m_sEa_min, new MapListerDomainType(".mpr", dmVALUE, false),true);
	m_rEa_min=rUNDEF;
	m_fldEa_min = new FieldReal(root, "", &m_rEa_min, vr);
	m_fldEa_min->Align(m_chkEa_min_map,AL_AFTER);

	m_useRH_max_min = true;
	m_chkRH_max_min = new CheckBox(root, TR("Max. and Min. relative humidity (%)"), &m_useRH_max_min);
	m_chkRH_max_min->SetCallBack((NotifyProc)&FormMapETo::MapRHMaxMinOnSelect);
	m_chkRH_max_min->Align(rg_Ea_RH,AL_UNDER);
	
	m_useRH_max_map = false;
	m_chkRH_max_map = new CheckBox(root, TR("Max. daily relative humidity map (%)"), &m_useRH_max_map);
	m_chkRH_max_map->SetCallBack((NotifyProc)&FormMapETo::MapRHMaxOnSelect);
	m_chkRH_max_map->Align(m_chkRH_max_min,AL_UNDER);
	m_fdtRH_max_map = new FieldDataType(m_chkRH_max_map, "", &m_sRH_max, new MapListerDomainType(".mpr", dmVALUE, false),true);
	m_rRH_max=rUNDEF;
	m_fldRH_max = new FieldReal(root, "", &m_rRH_max, vr);
	m_fldRH_max->Align(m_chkRH_max_map,AL_AFTER);

	m_useRH_min_map = false;
	m_chkRH_min_map = new CheckBox(root, TR("Min. daily relative humidity map (%)"), &m_useRH_min_map);
	m_chkRH_min_map->SetCallBack((NotifyProc)&FormMapETo::MapRHMinOnSelect);
	m_chkRH_min_map->Align(m_chkRH_max_map,AL_UNDER);
	m_fdtRH_min_map = new FieldDataType(m_chkRH_min_map, "", &m_sRH_min, new MapListerDomainType(".mpr", dmVALUE, false),true);
	m_rRH_min=rUNDEF;
	m_fldRH_min = new FieldReal(root, "", &m_rRH_min, vr);
	m_fldRH_min->Align(m_chkRH_min_map,AL_AFTER);

	FieldBlank *fb1 = new FieldBlank(root, 1); // Used to force proper alignment	
    fb1->Align(m_chkRH_min_map, AL_UNDER);

	m_useRH_avg_map = false;
	m_chkRH_avg_map = new CheckBox(root, TR("Average daily relative humidity map (%)"), &m_useRH_avg_map);
	m_chkRH_avg_map->SetCallBack((NotifyProc)&FormMapETo::MapRHAvgOnSelect);
	m_chkRH_avg_map->Align(m_chkRH_max_min,AL_UNDER);
	m_fdtRH_avg_map = new FieldDataType(m_chkRH_avg_map, "", &m_sRH_avg, new MapListerDomainType(".mpr", dmVALUE, false),true);
	m_rRH_avg=rUNDEF;
	m_fldRH_avg = new FieldReal(root, "", &m_rRH_avg, vr);
	m_fldRH_avg->Align(m_chkRH_avg_map,AL_AFTER);

	m_useT_max_map = false;
	m_chkT_max_map = new CheckBox(root, TR("Max daily air temperature map (Celsius)"), &m_useT_max_map);
	m_chkT_max_map->SetCallBack((NotifyProc)&FormMapETo::MapTMaxOnSelect);
	m_chkT_max_map->Align(fb1,AL_UNDER);
	m_fdtT_max_map = new FieldDataType(m_chkT_max_map, "", &m_sT_max, new MapListerDomainType(".mpr", dmVALUE, false),true);
	m_rT_max=rUNDEF;
	m_fldT_max = new FieldReal(root, "", &m_rT_max, vr);
	m_fldT_max->Align(m_chkT_max_map,AL_AFTER);
	
	m_useT_min_map = false;
	m_chkT_min_map = new CheckBox(root, TR("Min daily air temperature map (Celsius)"), &m_useT_min_map);
	m_chkT_min_map->SetCallBack((NotifyProc)&FormMapETo::MapTMinOnSelect);
	m_chkT_min_map->Align(m_chkT_max_map,AL_UNDER);
	m_fdtT_min_map = new FieldDataType(m_chkT_min_map, "", &m_sT_min, new MapListerDomainType(".mpr", dmVALUE, false),true);
	m_rT_min=rUNDEF;
	m_fldT_min = new FieldReal(root, "", &m_rT_min, vr);
	m_fldT_min->Align(m_chkT_min_map,AL_AFTER);
	
	
	m_useT_avg_map = false;
	m_chkT_avg_map = new CheckBox(root, TR("Average daily air temperature map (Celsius)"), &m_useT_avg_map);
	m_chkT_avg_map->SetCallBack((NotifyProc)&FormMapETo::MapTAvgOnSelect);
	m_chkT_avg_map->Align(m_chkT_min_map,AL_UNDER);
	m_fdtT_avg_map = new FieldDataType(m_chkT_avg_map, "", &m_sT_avg, new MapListerDomainType(".mpr", dmVALUE, false),true);
	m_rT_avg=rUNDEF;
	m_fldT_avg = new FieldReal(root, "", &m_rT_avg, vr);
	m_fldT_avg->Align(m_chkT_avg_map,AL_AFTER);

	FieldBlank *fb2 = new FieldBlank(root, 1); // Used to force proper alignment	
    fb2->Align(m_chkT_avg_map, AL_UNDER);
	m_iRn_E = 0;
	rg_Rn_E = new RadioGroup(root, "", &m_iRn_E, true);
	rg_Rn_E->Align(fb2,AL_UNDER);
	rg_Rn_E->SetCallBack((NotifyProc)&FormMapETo::MapRnEOnSelect);
	RadioButton* rbNa = new RadioButton(rg_Rn_E, TR("Solar net radiation at crop surface (MJ.m^-2/day)"));
	RadioButton* rbE = new RadioButton(rg_Rn_E, TR("Average daily shortwave transmissivity"));
	m_useRn_map=false;
	m_chkRn_map = new CheckBox(root, TR("Solar net radiation at crop surface map (MJ.m^-2/day)"), &m_useRn_map);
	m_chkRn_map->SetCallBack((NotifyProc)&FormMapETo::MapRnOnSelect);
	m_chkRn_map->Align(rg_Rn_E,AL_UNDER);
	m_fdtRn_map = new FieldDataType(m_chkRn_map, "", &m_sRn, new MapListerDomainType(".mpr", dmVALUE, false),true);
	m_rRn=rUNDEF;
	m_fldRn = new FieldReal(root, "", &m_rRn, vr);
	m_fldRn->Align(m_chkRn_map,AL_AFTER);
	
	m_useE_map=false;
	m_chkE_map = new CheckBox(root, TR("Average daily shortwave transmissivity map"), &m_useE_map);
	m_chkE_map->SetCallBack((NotifyProc)&FormMapETo::MapEOnSelect);
	m_chkE_map->Align(m_chkRn_map,AL_UNDER);
	m_fdtE_map = new FieldDataType(m_chkE_map, "", &m_sE, new MapListerDomainType(".mpr", dmVALUE, false),true);
	m_rE=rUNDEF;
	m_fldE = new FieldReal(root, "", &m_rE, vr);
	m_fldE->Align(m_chkE_map,AL_AFTER);
	
	m_useHeight_map = false;
	m_chkDem_map = new CheckBox(root, TR("DEM map"), &m_useHeight_map);
	m_chkDem_map->Align(m_chkE_map,AL_UNDER);
	m_chkDem_map->SetCallBack((NotifyProc)&FormMapETo::MapDemOnSelect);
	m_fdtDEM_map = new FieldDataType(m_chkDem_map, "", &m_sHeight, new MapListerDomainType(".mpr", dmVALUE, false),true);
	m_rHeight=rUNDEF;
	m_fldDem = new FieldReal(root, "", &m_rHeight, vr);
	m_fldDem->Align(m_chkDem_map,AL_AFTER);

	FieldInt* fldDaynumber = new FieldInt(root, TR("Julian day number"), &m_daynumber, ValueRange(1,365));
	fldDaynumber->Align(m_chkDem_map,AL_UNDER);
	FieldBlank *fb3 = new FieldBlank(root, 1); // Used to force proper alignment	
    fb3->Align(fldDaynumber, AL_UNDER);

   	initMapOut(false, false);
	m_fRaMap = true;
	CheckBox* chkRa = new CheckBox(root, TR("Output daily terrestrial SW incoming solar radiation map"), &m_fRaMap);
	FieldMapCreate* fmRa = new FieldMapCreate(chkRa, "", &m_sRa);
		
    //SetHelpItem("ilwisapp\\flow_direction.htm");
	create();
}
int FormMapETo::exec() 
{
  //FormMapETo::exec();
  FormMapCreate::exec();
  FileName fn(sOutMap,".mpr", TRUE);

  FileName fnMapU2(m_sU2); 
  m_sU2 = fnMapU2.sRelativeQuoted(false,fn.sPath());

  if (m_iEa_RH==0){
	  if(m_useEa_max_map){  
		FileName fnMapEa_max(m_sEa_max); 
		m_sEa_max = fnMapEa_max.sRelativeQuoted(false,fn.sPath());	
		m_rEa_max = 0;
	  }
	  else
		m_sEa_max = "nomap";
	  if(m_useEa_min_map){  
		FileName fnMapEa_min(m_sEa_min); 
		m_sEa_min = fnMapEa_min.sRelativeQuoted(false,fn.sPath());	
		m_rEa_min = 0;
	  }
	  else
		m_sEa_min = "nomap";
  }
  else{
	  if(m_useRH_max_min){
        m_useRH_max_min = true;
		m_sRH_avg = "nomap";
		m_rRH_avg = 0;
		if(m_useRH_max_map){
		  FileName fnMapRH_max(m_sRH_max); 
		  m_sRH_max = fnMapRH_max.sRelativeQuoted(false,fn.sPath());	
		  m_rRH_max = 0;
		}
		else
		  m_sRH_max = "nomap";
		if(m_useRH_min_map){
		  FileName fnMapRH_min(m_sRH_min); 
		  m_sRH_min = fnMapRH_min.sRelativeQuoted(false,fn.sPath());	
		  m_rRH_min = 0;
		}
		else
		  m_sRH_min = "nomap";	
	  }
	  else{
		  m_useRH_max_min = false;
		  m_sRH_max = "nomap";
		  m_sRH_min = "nomap";
		  m_rRH_min = 0;
		  m_rRH_max = 0;
		  if(m_useRH_avg_map){
			FileName fnMapRH_avg(m_sRH_avg); 
			m_sRH_avg = fnMapRH_avg.sRelativeQuoted(false,fn.sPath());	
			m_rRH_avg = 0;
		}
		else
			m_sRH_avg = "nomap";	

	  }
  }

  if(m_useT_max_map){
	  FileName fnMapT_max(m_sT_max); 
	  m_sT_max = fnMapT_max.sRelativeQuoted(false,fn.sPath());	
	  m_rT_max = 0;
  }
  else
	  m_sT_max = "nomap";	
  if(m_useT_min_map){
	  FileName fnMapT_min(m_sT_min); 
	  m_sT_min = fnMapT_min.sRelativeQuoted(false,fn.sPath());	
	  m_rT_min = 0;
  }
  else
	  m_sT_min = "nomap";	
  if(m_useT_avg_map){
	  FileName fnMapT_avg(m_sT_avg); 
	  m_sT_avg = fnMapT_avg.sRelativeQuoted(false,fn.sPath());	
	  m_rT_avg = 0;
  }
  else
	  m_sT_avg = "nomap";	
  if(m_iRn_E==0){
    m_useRn = true;
	if(m_useRn_map){
      FileName fnMapRn(m_sRn); 
	  m_sRn = fnMapRn.sRelativeQuoted(false,fn.sPath());	
	  m_rRn = 0;
	}
	else
	  m_sRn = "nomap";	
  }
  else{
    m_useRn = false;
	if(m_useE_map){
	  FileName fnMapE(m_sE); 
	  m_sRn = fnMapE.sRelativeQuoted(false,fn.sPath());	
	  m_rRn = 0;
	}
	else{
	  m_sRn = "nomap";	
	  m_rRn = m_rE;
	}
  }

  if(m_useHeight_map){
	  FileName fnMapDem(m_sHeight); 
	  m_sHeight = fnMapDem.sRelativeQuoted(false,fn.sPath());	
	  m_rHeight = 0;
  }
  else
	  m_sHeight = "nomap";	
  if (m_fRaMap) 
  {
    FileName fnRaMap(m_sRa);
    m_sRa = fnRaMap.sRelativeQuoted(false,fn.sPath());
  }
  else
    m_sRa = "nomap";

  String sExpr;
  if(m_iEa_RH==0)
	  sExpr=String("MapETo(%S,%li,%S,%g,%li,%S,%g,%li,%S,%g,%li,%S,%g,%li,%S,%g,%li,%li,%S,%g,%li,%S,%g,%li,%S)", 
										m_sU2,
										m_useEa_max_map,m_sEa_max,m_rEa_max,
										m_useEa_min_map,m_sEa_min,m_rEa_min,
										m_useT_max_map,m_sT_max,m_rT_max,
										m_useT_min_map,m_sT_min,m_rT_min,
										m_useT_avg_map,m_sT_avg,m_rT_avg,
										m_useRn,
										m_useRn_map,m_sRn,m_rRn,
										m_useHeight_map,m_sHeight,m_rHeight,
										m_daynumber,
										m_sRa);
										
  else
	  sExpr=String("MapETo(%S,%li,%li,%S,%g,%li,%S,%g,%li,%S,%g,%li,%S,%g,%li,%S,%g,%li,%S,%g,%li,%li,%S,%g,%li,%S,%g,%li,%S)", 
										m_sU2,
										m_useRH_max_min,
										m_useRH_max_map,m_sRH_max,m_rRH_max,
										m_useRH_min_map,m_sRH_min,m_rRH_min,
										m_useRH_avg_map,m_sRH_avg,m_rRH_avg,
										m_useT_max_map,m_sT_max,m_rT_max,
										m_useT_min_map,m_sT_min,m_rT_min,
										m_useT_avg_map,m_sT_avg,m_rT_avg,
										m_useRn,
										m_useRn_map,m_sRn,m_rRn,
										m_useHeight_map,m_sHeight,m_rHeight,
										m_daynumber,
										m_sRa);

  execMapOut(sExpr);  	
  return 0;
}
int FormMapETo::MapEaRHOnSelect(Event *)
{
    rg_Ea_RH->StoreData();
    if (m_iEa_RH==0){
	  m_chkEa_max_map->Show();
	  m_fdtEa_max_map->Show();
	  m_fldEa_max->Show();
	  m_chkEa_min_map->Show();
	  m_fdtEa_min_map->Show();
	  m_fldEa_min->Show();
		
      m_chkEa_max_map->Show();
	  if(m_useEa_max_map){
		m_fdtEa_max_map->Show();
		m_fldEa_max->Hide();
	  }
	  else{
		m_fdtEa_max_map->Hide();
		m_fldEa_max->Show();
	  }
	  m_chkEa_min_map->Show(); 
	  if(m_useEa_min_map){
		m_fdtEa_min_map->Show();
		m_fldEa_min->Hide();
	  }
	  else{
		m_fdtEa_min_map->Hide();
		m_fldEa_min->Show();
	  }
	  
	  m_chkRH_max_min->Hide();
	  m_chkRH_max_map->Hide();
	  m_chkRH_min_map->Hide();
	  m_chkRH_avg_map->Hide();
	  m_fdtRH_max_map->Hide();
	  m_fdtRH_min_map->Hide();
	  m_fdtRH_avg_map->Hide();
	  m_fldRH_max->Hide();	
	  m_fldRH_min->Hide();	
	  m_fldRH_avg->Hide();

	}
    else{
      m_chkEa_max_map->Hide();
	  m_fdtEa_max_map->Hide();
	  m_fldEa_max->Hide();
	  m_chkEa_min_map->Hide();
	  m_fdtEa_min_map->Hide();
	  m_fldEa_min->Hide();

	  m_chkRH_max_min->Show();
	  m_chkRH_max_map->Show();
	  m_chkRH_min_map->Show();
	  m_chkRH_avg_map->Show();
	  m_fdtRH_max_map->Show();
	  m_fdtRH_min_map->Show();
	  m_fdtRH_avg_map->Show();
	  m_fldRH_max->Show();	
	  m_fldRH_min->Show();	
	  m_fldRH_avg->Show();
	
	  m_chkRH_max_min->Show();
	  if(m_useRH_max_min){
		m_chkRH_max_map->Show();
		if(m_useRH_max_map){
			m_fdtRH_max_map->Show();
			m_fldRH_max->Hide();	
		}
		else{
			m_fdtRH_max_map->Hide();
			m_fldRH_max->Show();	
		}
		if(m_useRH_min_map){
			m_fdtRH_min_map->Show();
			m_fldRH_min->Hide();	
		}
		else{
			m_fdtRH_min_map->Hide();
			m_fldRH_min->Show();	
		}
		m_chkRH_avg_map->Hide();
		m_fldRH_avg->Hide();
		m_fdtRH_avg_map->Hide();
	  }
	  else{
		m_chkRH_max_map->Hide();
		m_chkRH_min_map->Hide();
		m_fldRH_max->Hide();	
		m_fldRH_min->Hide();
		m_chkRH_avg_map->Show();
		if(m_useRH_avg_map){
			m_fdtRH_avg_map->Show();
			m_fldRH_avg->Hide();	
		}
		else{
			m_fdtRH_avg_map->Hide();
			m_fldRH_avg->Show();	
		}

	  }
    }
	return 1;
}

int FormMapETo::MapRHMaxMinOnSelect(Event *)
{
	m_chkRH_max_min->StoreData();
	if(m_useRH_max_min){
		m_chkRH_max_map->Show();
		m_chkRH_min_map->Show();
		m_chkRH_avg_map->Hide();
		m_fldRH_max->Show();	
		m_fldRH_min->Show();	
		m_fldRH_avg->Hide();	
	}
	else{
		m_chkRH_max_map->Hide();
		m_chkRH_min_map->Hide();
		m_chkRH_avg_map->Show();
		m_fldRH_max->Hide();	
		m_fldRH_min->Hide();	
		m_fldRH_avg->Show();	
	}
	
	return 1;
}

int FormMapETo::MapEaMaxOnSelect(Event *)
{
	m_chkEa_max_map->StoreData();
	if(m_useEa_max_map){
		m_fdtEa_max_map->Show();
		m_fldEa_max->Hide();
	}
	else{
		m_fdtEa_max_map->Hide();
		m_fldEa_max->Show();
	}
	return 1;
}
int FormMapETo::MapEaMinOnSelect(Event *)
{
	m_chkEa_min_map->StoreData();
	if(m_useEa_min_map){
		m_fdtEa_min_map->Show();
		m_fldEa_min->Hide();
	}
	else{
		m_fdtEa_min_map->Hide();
		m_fldEa_min->Show();
	}
	return 1;
}
int FormMapETo::MapRHMaxOnSelect(Event *)
{
	m_chkRH_max_map->StoreData();
	if(m_useRH_max_map){
		m_fdtRH_max_map->Show();
		m_fldRH_max->Hide();
	}
	else{
		m_fdtRH_max_map->Hide();
		m_fldRH_max->Show();
	}
	return 1;
}
int FormMapETo::MapRHMinOnSelect(Event *)
{
	m_chkRH_min_map->StoreData();
	if(m_useRH_min_map){
		m_fdtRH_min_map->Show();
		m_fldRH_min->Hide();
	}
	else{
		m_fdtRH_min_map->Hide();
		m_fldRH_min->Show();
	}
	return 1;
}
int FormMapETo::MapRHAvgOnSelect(Event *)
{
	m_chkRH_avg_map->StoreData();
	if(m_useRH_avg_map){
		m_fdtRH_avg_map->Show();
		m_fldRH_avg->Hide();
	}
	else{
		m_fdtRH_avg_map->Hide();
		m_fldRH_avg->Show();
	}
	return 1;
}

int FormMapETo::MapTAvgOnSelect(Event *)
{
	m_chkT_avg_map->StoreData();
	if(m_useT_avg_map){
		m_fdtT_avg_map->Show();
		m_fldT_avg->Hide();
	}
	else{
		m_fdtT_avg_map->Hide();
		m_fldT_avg->Show();
	}
	return 1;
}
int FormMapETo::MapTMaxOnSelect(Event *)
{
	m_chkT_max_map->StoreData();
	if(m_useT_max_map){
		m_fdtT_max_map->Show();
		m_fldT_max->Hide();
	}
	else{
		m_fdtT_max_map->Hide();
		m_fldT_max->Show();
	}
	return 1;
}
int FormMapETo::MapTMinOnSelect(Event *)
{
	m_chkT_min_map->StoreData();
	if(m_useT_min_map){
		m_fdtT_min_map->Show();
		m_fldT_min->Hide();
	}
	else{
		m_fdtT_min_map->Hide();
		m_fldT_min->Show();
	}
	return 1;
}

int FormMapETo::MapRnEOnSelect(Event *)
{
	rg_Rn_E->StoreData();
	if(m_iRn_E==0){//m_useRn
	  m_chkRn_map->Show();	
	  m_fdtRn_map->Show();
	  m_fldRn->Show();	
	  m_chkE_map->Hide();	
	  m_fdtE_map->Hide();
	  m_fldE->Hide();	
	  if(m_useRn_map){
		m_fdtRn_map->Show();
		m_fldRn->Hide();
	  }
	  else{
		m_fdtRn_map->Hide();
		m_fldRn->Show();
	  }
	}
	else{
	  m_chkRn_map->Hide();		
	  m_fdtRn_map->Hide();
	  m_fldRn->Hide();
	  m_chkE_map->Show();	
	  m_fdtE_map->Show();
	  m_fldE->Show();	
	  if(m_useE_map){
		m_fdtE_map->Show();
		m_fldE->Hide();
	  }
	  else{
		m_fdtE_map->Hide();
		m_fldE->Show();
	  }
	}
	return 1;
}

int FormMapETo::MapRnOnSelect(Event *)
{
	m_chkRn_map->StoreData();
	if(m_useRn_map){
		m_fdtRn_map->Show();
		m_fldRn->Hide();
	}
	else{
		m_fdtRn_map->Hide();
		m_fldRn->Show();
	}
	return 1;
}

int FormMapETo::MapEOnSelect(Event *)
{
	m_chkE_map->StoreData();
	if(m_useE_map){
		m_fdtE_map->Show();
		m_fldE->Hide();
	}
	else{
		m_fdtE_map->Hide();
		m_fldE->Show();
	}
	return 1;
}

int FormMapETo::MapDemOnSelect(Event *)
{
	m_chkDem_map->StoreData();
	if(m_useHeight_map){
		m_fdtDEM_map->Show();
		m_fldDem->Hide();
	}
	else{
		m_fdtDEM_map->Hide();
		m_fldDem->Show();
	}
	return 1;
}


LRESULT Cmdmapsebs(CWnd *wnd, const String& s)
{
	new FormMapSebs(wnd, s.c_str());
	return -1;
}

FormMapSebs::FormMapSebs(CWnd* mw, const char* sPar)
:FormMapCreate(mw, TR("Surface Energy Balance System (SEBS)"))
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
					if (m_sMapLST == "")
						m_sMapLST = fn.sFullNameQuoted(true);
					else
						sOutMap = fn.sFullName(false);
		}
	}
	ValueRange vr(-1e300, 1e300, 1e-2);
	FieldGroup *fg3 = new FieldGroup(root);
	FieldGroup *fg1 = new FieldGroup(fg3);
	new FieldDataType(fg1, TR("Land Surface Temperature"), &m_sMapLST, new MapListerDomainType(".mpr", dmVALUE, false),true);
	new FieldDataType(fg1, TR("Emissivity") , &m_sMapEmis, new MapListerDomainType(".mpr", dmVALUE, false),true);
	new FieldDataType(fg1, TR("Land Surface Albedo"), &m_sMapAlbedo, new MapListerDomainType(".mpr", dmVALUE, false),true);
	FieldDataType *fldNDVI = new FieldDataType(fg1, TR("NDVI"), &m_sMapNDVI, new MapListerDomainType(".mpr", dmVALUE, false),true);
	

	m_fPv = false;
	CheckBox *chkPv = new CheckBox(fg1, "Vegetation Fraction (Fc)", &m_fPv);
	//chkPv->Align(fldNDVI, AL_UNDER);
	new FieldDataType(chkPv, "", &m_sMapPv, new MapListerDomainType(".mpr", dmVALUE, false),true);
	
	m_fLai = false;
	CheckBox *chkLai = new CheckBox(fg1, TR("Leaf Area Index"), &m_fLai);
	chkLai->Align(chkPv,AL_UNDER);
	new FieldDataType(chkLai, "", &m_sMapLai, new MapListerDomainType(".mpr", dmVALUE, false),true);
	
	m_fSza = false;
	m_chkSza = new CheckBox(fg1, TR("Sun Zenith Angle Map (degree)"), &m_fSza);
	m_chkSza->Align(chkLai, AL_UNDER);
	m_chkSza->SetCallBack((NotifyProc)&FormMapSebs::MapOnSelect);
	new FieldDataType(m_chkSza, "", &m_sMapSza, new MapListerDomainType(".mpr", dmVALUE, false), true);
	m_rSza = 60;
	m_fldSza = new FieldReal(fg1, "", &m_rSza, vr); //Avg. SZA value
	m_fldSza->Align(m_chkSza,AL_AFTER);

	m_fDem = false;
	m_chkDem = new CheckBox(fg1, TR("DEM map"), &m_fDem);
	m_chkDem->SetCallBack((NotifyProc)&FormMapSebs::DemMapOnSelect);
	m_chkDem->Align(m_chkSza,AL_UNDER);
	new FieldDataType(m_chkDem, "", &m_sMapDem, new MapListerDomainType(".mpr", dmVALUE, false), true);
	m_rDem = rUNDEF;
	m_fldDem = new FieldReal(fg1, "", &m_rDem, vr);//Avg. DEM value
	m_fldDem->Align(m_chkDem,AL_AFTER);
	m_fSdwnMap = false;
	m_chkSdwnMap = new CheckBox(fg1, TR("Inst. downward solar radiation map(Watts/m^2)"), &m_fSdwnMap);
	m_chkSdwnMap->Align(m_chkDem, AL_UNDER);
	m_chkSdwnMap->SetCallBack((NotifyProc)&FormMapSebs::SdwnMapOnSelect);
	m_fldSdwn = new FieldDataType(m_chkSdwnMap, "", &m_sSdwnMap, new MapListerDomainType(".mpr", dmVALUE, false),true);
	m_fS_dwn = true;
	m_chkSdwn = new CheckBox(fg1, TR("Inst. downward solar radiation value(Watts/m^2)"), &m_fS_dwn);
	m_S_dwn = 1025.0;
	m_chkSdwn->SetCallBack((NotifyProc)&FormMapSebs::CalcSdwnOnSelect);
	m_chkSdwn->Align(m_chkSdwnMap, AL_UNDER);
	FieldReal* flS_dwn = new FieldReal(m_chkSdwn, "", &m_S_dwn, vr);
	flS_dwn->Align(m_chkSdwn, AL_AFTER);

	m_Visi = 39.2;
	m_flVisi = new FieldReal(fg1, TR("Horizontal visibility (km)"), &m_Visi, vr);
	m_flVisi->Align(m_chkSdwn, AL_UNDER);
	
	FieldGroup *fg2 = new FieldGroup(fg3);
	StaticText *st = new StaticText(fg2, TR("Land use map with associated surface parameters"), true);
	
	m_fHc = false;
	CheckBox *chkHc = new CheckBox(fg2, TR("Canopy height map [m]"), &m_fHc);
	chkHc->Align(st, AL_UNDER);
	new FieldDataType(chkHc, "", &m_sHc, new MapListerDomainType(".mpr", dmVALUE, true), true);

	m_fD0 = false;
	CheckBox *chkD0 = new CheckBox(fg2, TR("Displacement height map [m]"), &m_fD0);
	chkD0->Align(chkHc, AL_UNDER);
	new FieldDataType(chkD0, "", &m_sD0, new MapListerDomainType(".mpr", dmVALUE, true), true);

	m_fLUM = false;
	CheckBox *chkLUM = new CheckBox(fg2, TR("Surface roughness map [m]"), &m_fLUM);
	chkLUM->Align(chkD0, AL_UNDER);
	new FieldDataType(chkLUM, "", &m_sLUM, new MapListerDomainType(".mpr", dmVALUE, true), true);
	
	m_fDaynumber = true;
	m_chkDaynumber = new CheckBox(fg2, TR("Julian day number"), &m_fDaynumber);
	m_chkDaynumber->Align(chkLUM, AL_UNDER);
	m_chkDaynumber->SetCallBack((NotifyProc)&FormMapSebs::DaynumberOnSelect);
	FieldInt* fldDaynumber = new FieldInt(m_chkDaynumber, "", &m_iDaynumber, ValueRange(1,365));
	m_iMonth = iUNDEF;
	m_fldMonth = new FieldInt(fg2, TR("Month"), &m_iMonth, ValueRange(1,12));
	m_fldMonth->SetIndependentPos();
	m_fldMonth->Align(m_chkDaynumber,AL_UNDER);
	//m_fldMonth->SetWidth(45);
	
	m_iDay = iUNDEF;
    m_fldDay = new FieldInt(fg2, TR("Day"), &m_iDay, ValueRange(1,31,1)); //Day
	m_fldDay->SetIndependentPos();
	m_fldDay->Align(m_fldMonth,AL_AFTER);
	m_iYear = iUNDEF;
	m_fldYear = new FieldInt(fg2, TR("Year"), &m_iYear, ValueRange(1,LONG_MAX)); //Year
	m_fldYear->SetIndependentPos();
	m_fldYear->Align(m_fldDay,AL_AFTER);
	
	m_Z_ref = 2.0;
	FieldReal* fldReferenceHeight = new FieldReal(fg2, TR("Reference Height (m)"), &m_Z_ref, vr);
	fldReferenceHeight->Align(m_fldMonth,AL_UNDER);
	m_hi = 1000.0;
	FieldReal* fldBPLHeight = new FieldReal(fg2, TR("PBL height (m)"), &m_hi, vr);
	
	m_fQ_ref = false;
	m_chkQ_ref = new CheckBox(fg2, TR("Specific humidity map (kg/kg)"), &m_fQ_ref);
	m_chkQ_ref->SetCallBack((NotifyProc)&FormMapSebs::MapOnSelect);
	new FieldDataType(m_chkQ_ref, "", &m_sQ_ref, new MapListerDomainType(".mpr", dmVALUE, false), true);
	
	m_Q_ref = 0.006;
	ValueRange vr1(-1e300, 1e300, 1e-3);		   
	m_fldQ_ref = new FieldReal(fg2, "", &m_Q_ref, vr1);
	m_fldQ_ref->Align(m_chkQ_ref,AL_AFTER);
	
	m_fU_ref = false;
	m_chkU_ref = new CheckBox(fg2, TR("Wind speed map (m/s)"), &m_fU_ref);
	m_chkU_ref->SetCallBack((NotifyProc)&FormMapSebs::MapOnSelect);
	m_chkU_ref->Align(m_chkQ_ref,AL_UNDER);
	new FieldDataType(m_chkU_ref, "", &m_sU_ref, new MapListerDomainType(".mpr", dmVALUE, false), true);
	m_U_ref = 2.0;
	m_fldU_ref = new FieldReal(fg2, "", &m_U_ref, vr);
	m_fldU_ref->Align(m_chkU_ref,AL_AFTER);
	
	m_fT_ref = false;
	m_chkT_ref = new CheckBox(fg2, TR("Air temperature map (Celsius)"), &m_fT_ref);
	m_chkT_ref->SetCallBack((NotifyProc)&FormMapSebs::MapOnSelect);
	m_chkT_ref->Align(m_chkU_ref,AL_UNDER);
	new FieldDataType(m_chkT_ref, "", &m_sT_ref, new MapListerDomainType(".mpr", dmVALUE, false), true);
	m_T_ref = 25.0;
	m_fldT_ref = new FieldReal(fg2, "", &m_T_ref, vr);
	m_fldT_ref->Align(m_chkT_ref,AL_AFTER);
	
	m_fP_ref = false;
	m_chkP_ref = new CheckBox(fg2, TR("Pressure at reference height map (Pa)"), &m_fP_ref);
	m_chkP_ref->SetCallBack((NotifyProc)&FormMapSebs::MapOnSelect);
	m_chkP_ref->Align(m_chkT_ref,AL_UNDER);
	new FieldDataType(m_chkP_ref, "", &m_sP_ref, new MapListerDomainType(".mpr", dmVALUE, false), true);
	m_P_ref = 100000.0;
	m_fldP_ref = new FieldReal(fg2, "", &m_P_ref, vr);
	m_fldP_ref->Align(m_chkP_ref, AL_AFTER);

	m_fP_sur = false;
	m_chkP_sur = new CheckBox(fg2, TR("Pressure at surface map (Pa)"), &m_fP_sur);
	m_chkP_sur->SetCallBack((NotifyProc)&FormMapSebs::MapOnSelect);
	m_chkP_sur->Align(m_chkP_ref,AL_UNDER);
	new FieldDataType(m_chkP_sur, "", &m_sP_sur, new MapListerDomainType(".mpr", dmVALUE, false), true);
	m_P_sur = 100100.0;
	m_fldP_sur = new FieldReal(fg2, "", &m_P_sur, vr);
	m_fldP_sur->Align(m_chkP_sur,AL_AFTER);

	FieldBlank *fb1 = new FieldBlank(fg2, 1); // Used to force proper alignment	
    fb1->Align(m_chkP_sur, AL_UNDER);
    

	m_fTa_avg_map = false;
	m_chkTa_avg_map = new CheckBox(fg2, TR("Mean daily air temperature map (Celsius)"), &m_fTa_avg_map);
	m_chkTa_avg_map->SetCallBack((NotifyProc)&FormMapSebs::MapOnSelect);
	m_chkTa_avg_map->Align(m_chkP_sur,AL_UNDER);
	new FieldDataType(m_chkTa_avg_map, "", &m_sTa_avg, new MapListerDomainType(".mpr", dmVALUE, false), true);
	m_Ta_avg = 25;
	m_fldTa_avg = new FieldReal(fg2, "", &m_Ta_avg);
	m_fldTa_avg->Align(m_chkTa_avg_map,AL_AFTER);

	m_fN_s_map = false;
	m_chkN_s_map = new CheckBox(fg2, TR("Sunshine hours per day"), &m_fN_s_map);
	m_chkN_s_map->SetCallBack((NotifyProc)&FormMapSebs::MapOnSelect);
	m_chkN_s_map->Align(m_chkTa_avg_map,AL_UNDER);
	new FieldDataType(m_chkN_s_map, "", &m_sN_s, new MapListerDomainType(".mpr", dmVALUE, false), true);
	m_N_s = 10;
	m_fldN_s = new FieldReal(fg2, "", &m_N_s);
	m_fldN_s->Align(m_chkN_s_map,AL_AFTER);

	//m_fKB=true: use kb as input , otherwise use sebs model for kb estimates 
	m_fKB = true;
	m_chkKB = new CheckBox(fg2, "Input kB^-1", &m_fKB);
	m_chkKB->SetCallBack((NotifyProc)&FormMapSebs::kbCalOnSelect);
	m_chkKB->Align(m_chkN_s_map,AL_UNDER);
    
	m_fKB_s_map = false;
	m_chkKB_s_map = new CheckBox(fg2, "kB^-1 Map", &m_fKB_s_map);
	m_chkKB_s_map->Align(m_chkKB,AL_UNDER);
	m_chkKB_s_map->SetCallBack((NotifyProc)&FormMapSebs::kbMapOnSelect);
	m_chkKB_s_map->Align(m_chkKB,AL_UNDER);
	m_fldKB_s_map = new FieldDataType(m_chkKB_s_map, "", &m_sKB_s_map, new MapListerDomainType(".mpr", dmVALUE, false), true);
	m_kb = 2.5;
	m_fldKB_s = new FieldReal(fg2, "", &m_kb);
	m_fldKB_s->Align(m_chkKB_s_map,AL_AFTER);
	fg2->Align(fg1, AL_AFTER);
	
	//FieldBlank *fb = new FieldBlank(root, 1); // Used to force proper alignment	
 //   fb->Align(fg3, AL_UNDER);
    	
    //initMapOut(false, false);
	FieldGroup *fg4 = new FieldGroup(root);

	FieldMapCreate *fmc = new FieldMapCreate(fg4, TR("&Output Raster Map"), &sOutMap);
	fmc->Align(fg3,AL_UNDER);
    FieldString* fs = new FieldString(fg4, TR("&Description:"), &sDescr);
    fs->Align(fmc, AL_AFTER);

    fs->SetWidth(120);
 // fs->SetIndependentPos();
	fg4->SetIndependentPos();
	
	create();
}

int FormMapSebs::exec() 
{
  FormMapCreate::exec();
  FileName fn(sOutMap,".mpr", TRUE);

  FileName fnMapLST(m_sMapLST); 
  m_sMapLST = fnMapLST.sRelativeQuoted(false,fn.sPath());

  FileName fnMapEmis(m_sMapEmis); 
  m_sMapEmis = fnMapEmis.sRelativeQuoted(false,fn.sPath());	

  FileName fnMapAlbedo(m_sMapAlbedo); 
  m_sMapAlbedo = fnMapAlbedo.sRelativeQuoted(false,fn.sPath());	
  

  FileName fnMapNdvi(m_sMapNDVI); 
  m_sMapNDVI = fnMapNdvi.sRelativeQuoted(false,fn.sPath());	
	
  
  if (m_fLai){
	FileName fnMapLai(m_sMapLai); 
	m_sMapLai = fnMapLai.sRelativeQuoted(false,fn.sPath());	
  }
  //m_fPv = true;
  if (m_fPv){
	FileName fnMapPv(m_sMapPv); 
	m_sMapPv = fnMapPv.sRelativeQuoted(false,fn.sPath());	
  }

  if (m_fSza){
	FileName fnMapSza(m_sMapSza); 
	m_sMapSza = fnMapSza.sRelativeQuoted(false,fn.sPath());	
	m_rSza = 0;
  }
  else
	m_sMapSza = "nomap";
  if (m_fDem){
	FileName fnMapDem(m_sMapDem); 
	m_sMapDem = fnMapDem.sRelativeQuoted(false,fn.sPath());	
	m_rDem = 0;
  }
  else
    m_sMapDem  = "nomap";
  if (m_fLUM){
	FileName fnMapLUM(m_sLUM); 
	m_sLUM = fnMapLUM.sRelativeQuoted(false,fn.sPath());	
  }
  else
	m_sLUM = "nomap";

  if (m_fHc){
	FileName fnMapHc(m_sHc); 
	m_sHc = fnMapHc.sRelativeQuoted(false,fn.sPath());	
  }
  else
	m_sHc = "nomap";
  
  if (m_fD0){
	FileName fnMapD0(m_sD0); 
	m_sD0 = fnMapD0.sRelativeQuoted(false,fn.sPath());	
  }
  else
	m_sD0 = "nomap";

  if (m_fSdwnMap){
	FileName fnMapSdwn(m_sSdwnMap); 
	m_sSdwnMap = fnMapSdwn.sRelativeQuoted(false,fn.sPath());	
  }
  else
	m_sSdwnMap = "nomap";

  if (m_fQ_ref){
	FileName fnMapQ_ref(m_sQ_ref); 
	m_sQ_ref = fnMapQ_ref.sRelativeQuoted(false,fn.sPath());	
  }
  
  if (m_fU_ref){
	FileName fnMapU_ref(m_sU_ref); 
	m_sU_ref = fnMapU_ref.sRelativeQuoted(false,fn.sPath());	
  }
  
  if (m_fT_ref){
	FileName fnMapT_ref(m_sT_ref); 
	m_sT_ref = fnMapT_ref.sRelativeQuoted(false,fn.sPath());	
  }

  if (m_fP_ref){
	FileName fnMapP_ref(m_sP_ref); 
	m_sP_ref = fnMapP_ref.sRelativeQuoted(false,fn.sPath());	
  }

  if (m_fP_sur){
	FileName fnMapP_sur(m_sP_sur); 
	m_sP_sur = fnMapP_sur.sRelativeQuoted(false,fn.sPath());	
  }
  
  if (m_fDaynumber){
	  m_iMonth = 0;
	  m_iDay = 0;	
	  m_iYear = 0;
  }
  else
	  m_iDaynumber = 0;

  if (m_fTa_avg_map){
	FileName fnMapTa_avg(m_sTa_avg); 
	m_sTa_avg = fnMapTa_avg.sRelativeQuoted(false,fn.sPath());	
  }
  if (m_fN_s_map){
	FileName fnMapN_s(m_sN_s); 
	m_sN_s = fnMapN_s.sRelativeQuoted(false,fn.sPath());	
  }

  if(m_fKB){
	  if(m_fKB_s_map){
	  	  FileName fnMapKB(m_sKB_s_map); 
	      m_sKB_s_map = fnMapKB.sRelativeQuoted(false,fn.sPath());	
      }
  }
  
  String sExpr("MapSEBS(%S,%S,%S,%S,%li,%S,%li,%S,%li,%S,%g,%li,%S,%g,%li,%li,%li,%li,%li,%g,%g,%li,%S,%g,%li,%S,%g,%li,%S,%g,%li,%S,%g,%li,%S,%g,%g,%li,%g,%li,%S,%li,%S,%li,%S,%li,%S,%li,%S,%g,%li,%S,%g,%li,%g,%S,%li)", 
										m_sMapLST, 
										m_sMapEmis,
										m_sMapAlbedo,
										m_sMapNDVI,
										m_fLai,
										m_sMapLai,
										m_fPv,
										m_sMapPv,
										m_fSza,
										m_sMapSza,
										m_rSza,
										m_fDem,
										m_sMapDem,
										m_rDem,
										m_fDaynumber,
										m_iDaynumber,
										m_iMonth,
										m_iDay,
										m_iYear,
										m_Z_ref,
										m_hi,
										m_fQ_ref,
										m_sQ_ref,
										m_Q_ref,
										m_fU_ref,
										m_sU_ref,
										m_U_ref,
										m_fT_ref,
										m_sT_ref,
										m_T_ref,
										m_fP_ref,
										m_sP_ref,
										m_P_ref,
										m_fP_sur,
										m_sP_sur,
										m_P_sur,
										m_Visi,
										m_fS_dwn,
										m_S_dwn,
										m_fLUM,
										m_sLUM,
										m_fHc,
										m_sHc,
										m_fD0,
										m_sD0,
										m_fSdwnMap,
										m_sSdwnMap,
										m_fTa_avg_map,m_sTa_avg,m_Ta_avg,
										m_fN_s_map,m_sN_s,m_N_s,
										m_fKB,m_kb, m_sKB_s_map, m_fKB_s_map);
  
  execMapOut(sExpr);  	
  return 0;
}

int FormMapSebs::kbMapOnSelect(Event *)
{
	m_chkKB_s_map->StoreData();
	if (m_fKB_s_map){
		m_fldKB_s->Hide();
		
	}
	else{
		m_fldKB_s->Show();
	}
	return 1;
}
int FormMapSebs::kbCalOnSelect(Event *)
{
	m_chkKB->StoreData();
	if (m_fKB != true){
		m_chkKB_s_map->Hide();
		m_fldKB_s_map->Hide();
		m_fldKB_s->Hide();
	}
	else{
		m_chkKB_s_map->Show();
		if (m_fKB_s_map){ 
		 m_fldKB_s_map->Show();
		 m_fldKB_s->Hide();
        }
		else{
         m_fldKB_s_map->Hide();   
		 m_fldKB_s->Show();
        }
	}
	return 1;
}

int FormMapSebs::MapOnSelect(Event *)
{
    m_chkSza->StoreData();
    if (m_fSza){
      m_fldSza->Hide(); 
    }
    else{
      m_fldSza->Show();
    }

	m_chkQ_ref->StoreData();
    if (m_fQ_ref){
      m_fldQ_ref->Hide(); 
    }
    else{
      m_fldQ_ref->Show();
    }

	m_chkU_ref->StoreData();
    if (m_fU_ref){
      m_fldU_ref->Hide(); 
    }
    else{
      m_fldU_ref->Show();
    }

	m_chkT_ref->StoreData();
    if (m_fT_ref){
      m_fldT_ref->Hide(); 
    }
    else{
      m_fldT_ref->Show();
    }

	m_chkP_ref->StoreData();
    if (m_fP_ref){
      m_fldP_ref->Hide(); 
    }
    else{
      m_fldP_ref->Show();
    }

	m_chkP_sur->StoreData();
    if (m_fP_sur){
      m_fldP_sur->Hide(); 
    }
    else{
      m_fldP_sur->Show();
    }

	m_chkTa_avg_map->StoreData();
	if(m_fTa_avg_map)
		m_fldTa_avg->Hide();
	else
		m_fldTa_avg->Show();
	m_chkN_s_map->StoreData();
	if(m_fN_s_map)
		m_fldN_s->Hide();
	else
		m_fldN_s->Show();

		return 1;
}

int FormMapSebs::SdwnMapOnSelect(Event *)
{
	m_chkSdwnMap->StoreData();
	if (m_fSdwnMap){
		m_fS_dwn = false;
		m_chkSdwn->SetVal(false);
	}
	if (m_fSdwnMap || m_fS_dwn)
		m_flVisi->Hide();
	else
		m_flVisi->Show();
	return 1;
}

int FormMapSebs::CalcSdwnOnSelect(Event *)
{
	m_chkSdwn->StoreData();
	if (m_fS_dwn){
		m_fSdwnMap = false;
		m_chkSdwnMap->SetVal(false);
		m_fldSdwn->Hide();
	}
	if (m_fSdwnMap || m_fS_dwn)
		m_flVisi->Hide();
	else
		m_flVisi->Show();
	return 1;
}

int FormMapSebs::DemMapOnSelect(Event *)
{
    m_chkDem->StoreData();
    if (m_fDem){
      m_fldDem->Hide(); 
    }
    else{
      m_fldDem->Show();
    }

		return 1;
}

int FormMapSebs::DaynumberOnSelect(Event *)
{
    m_chkDaynumber->StoreData();
    if (m_fDaynumber){
      m_fldMonth->Hide(); 
	  m_fldDay->Hide();
	  m_fldYear->Hide(); 
    }
    else{
      m_fldMonth->Show(); 
	  m_fldDay->Show();
	  m_fldYear->Show(); 
    }

		return 1;
}


LRESULT Cmdrawdata2radiance(CWnd *wnd, const String& s)
{
	new FormMapSI2Radiance(wnd, s.c_str());
	return -1;
}

FormMapSI2Radiance::FormMapSI2Radiance(CWnd* mw, const char* sPar)
:FormMapCreate(mw, TR("Raw to radiance/reflectance (MODIS)"))
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
						if (m_sMapRawdata == "")
							m_sMapRawdata = fn.sFullNameQuoted(true);
						else
							sOutMap = fn.sFullName(false);
			}
	  }
		new FieldMap(root, TR("Input Raw Data File"), &m_sMapRawdata, new MapListerDomainType(".mpr", dmVALUE, false));
		m_rScale = rUNDEF;
		ValueRange vr(-1e300, 1e300, 1e-10);
		FieldReal* fldRadianceScale = new FieldReal(root, TR("Radiance/Reflectance scale"), &m_rScale, vr);
	
		m_rOffset = rUNDEF;
		FieldReal* fldRadianceOffset = new FieldReal(root, TR("Radiance/Reflectance Offset"), &m_rOffset,vr);
		
		initMapOut(false, false);
		//SetHelpItem("ilwisapp\\fill_sinks.htm");
		create();
}

int FormMapSI2Radiance::exec() 
{
  FormMapCreate::exec();
  FileName fn(sOutMap);
  FileName fnMap(m_sMapRawdata); 
  m_sMapRawdata = fnMap.sRelativeQuoted(false,fn.sPath());

	
  String sExpr = String("MapSI2Radiance(%S,%.10f,%.10f)",m_sMapRawdata,m_rScale,m_rOffset);

  execMapOut(sExpr);  
  return 0;
}

LRESULT Cmdmapradiance2reflectance(CWnd *wnd, const String& s)
{
	new FormMapRadiance2Reflectance(wnd, s.c_str());
	return -1;
}

FormMapRadiance2Reflectance::FormMapRadiance2Reflectance(CWnd* mw, const char* sPar)
:FormMapCreate(mw, TR("Convert Radiance to Reflectance"))
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
						if (m_sMapRad == "")
							m_sMapRad = fn.sFullNameQuoted(true);
						else
							sOutMap = fn.sFullName(false);
			}
	  }
		new FieldMap(root, TR("Input Radiance Data File"), &m_sMapRad, new MapListerDomainType(".mpr", dmVALUE, false));
		FieldInt* fldDaynumber = new FieldInt(root, TR("Julian day number"), &m_iDaynumber, ValueRange(1,365));
		
		m_rSza= rUNDEF;
		m_AddSensorString = false;
		ValueRange vr(0, 90, 0.0001);
		m_fldSolarZenithAngle= new FieldReal(root, "Solar Zenith Angle (deg)", &m_rSza, vr); 
		m_fSensor= true;
		m_chkSensor= new CheckBox(root, TR("Sensor"), &m_fSensor);
		m_chkSensor->SetCallBack((NotifyProc)&FormMapRadiance2Reflectance::SensorOnSelect);
		m_fldSensor = new FieldOneSelectTextOnly(m_chkSensor, &m_sSensor, false);
		m_fldSensor->SetCallBack((NotifyProc)&FormMapRadiance2Reflectance::SensorOnChange);
		m_fldSensor->Align(m_chkSensor, AL_AFTER);
		fldBandnumber = new FieldInt(m_chkSensor, TR("Band Number"), &m_iBandnumber, ValueRange(1,9));
		fldBandnumber->Align(m_chkSensor, AL_UNDER);
		m_fldESUN= new FieldReal(root, "ESUN", &m_rEsun, ValueRange(0, 10000, 0.01)); 
		m_fldESUN->Align(m_chkSensor, AL_UNDER);
		initMapOut(false, false);
		//SetHelpItem("ilwisapp\\fill_sinks.htm");
		create();
}

int FormMapRadiance2Reflectance::exec() 
{
  FormMapCreate::exec();
  FileName fn(sOutMap);
  FileName fnMap(m_sMapRad); 
  m_sMapRad = fnMap.sRelativeQuoted(false,fn.sPath());

  String sExpr;
  if(m_fSensor)
	  sExpr = String("MapRadiance2Reflectance(%S,%li,%g,%S,%li)",m_sMapRad,m_iDaynumber,m_rSza,m_sSensor, m_iBandnumber);
  else
	  sExpr = String("MapRadiance2Reflectance(%S,%li,%g,%g)",m_sMapRad,m_iDaynumber,m_rSza,m_rEsun);	

  execMapOut(sExpr);  
  return 0;
}
int FormMapRadiance2Reflectance::SensorOnSelect(Event *)
{
    m_chkSensor->StoreData();
	if (m_fSensor){
		m_fldESUN->Hide();
	}
	else
		m_fldESUN->Show();
	return 1;
}

int FormMapRadiance2Reflectance::SensorOnChange(Event *)
{
    m_fldSensor->StoreData();
	if(m_AddSensorString == false)
	{
		m_fldSensor->AddString(String("ASTER"));
		m_sSensor = String("ASTER");
		m_fldSensor->SelectItem(m_sSensor);
		m_AddSensorString = true;
	}
	return 1;
}

LRESULT Cmdbrightnesstemperature(CWnd *wnd, const String& s)
{
	new FormMapBrightnessTemperature(wnd, s.c_str());
	return -1;
}

FormMapBrightnessTemperature::FormMapBrightnessTemperature(CWnd* mw, const char* sPar)
:FormMapCreate(mw, TR("Compute Brightness Temperature"))
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
						if (m_sMapRadiance == "")
							m_sMapRadiance = fn.sFullNameQuoted(true);
						else
							sOutMap = fn.sFullName(false);
			}
		}
		m_AddString = false;
		StaticText *stSensor = new StaticText(root,TR("Sensor"));
		m_fldSensor = new FieldOneSelectTextOnly(root, &m_sSensor, false);
		m_fldSensor->Align(stSensor, AL_AFTER);
		m_fldSensor->SetCallBack((NotifyProc)&FormMapBrightnessTemperature::SensorOnChange);
		m_fldSensor->SetWidth(90);
		FieldBlank *fb1 = new FieldBlank(root, 0.1); 
		fb1->Align(stSensor, AL_UNDER);
		m_fldBand1 = new FieldMap(root, TR("Input Radiance Band 31"), &m_sMapRadiance31, new MapListerDomainType(".mpr", dmVALUE, false));
		m_fldBand13 = new FieldMap(root, TR("Input TIR_Band 13"), &m_sMapRadiance13, new MapListerDomainType(".mpr", dmVALUE, false));
		m_fldBand13->Align(fb1, AL_UNDER);
		//m_fldBand2 = new FieldMap(root, "Input Radiance Band 32", &m_sMapRadiance32, new MapListerDomainType(".mpr", dmVALUE, false));
		m_fldBand = new FieldMap(root, TR("Input Radiance File"), &m_sMapRadiance, new MapListerDomainType(".mpr", dmVALUE, false));
		m_fldBand->Align(fb1, AL_UNDER);
		m_rWavelength = rUNDEF;
		ValueRange vr(0, 1e300, 1e-10);
		m_fldWavelength = new FieldReal(root, TR("Wavelength (um)"), &m_rWavelength, vr);
		initMapOut(false, false);
		FieldBlank *fb2 = new FieldBlank(root, 0.1); 
		m_fldBand2 = new FieldMap(root, TR("Input Radiance Band 32"), &m_sMapRadiance32, new MapListerDomainType(".mpr", dmVALUE, false));
		m_fldBand2->Align(fb2, AL_UNDER);
		m_fldBand14 = new FieldMap(root, TR("Input TIR_Band 14"), &m_sMapRadiance14, new MapListerDomainType(".mpr", dmVALUE, false));
		m_fldBand14->Align(fb2, AL_UNDER);
		m_fmcBT32 = new FieldMapCreate(root, TR("Output Map for Band 32"), &m_sBTMap32);
		m_fmcBT14 = new FieldMapCreate(root, TR("Output Map for TIR_Band 14"), &m_sBTMap14);
		m_fmcBT14->Align(m_fldBand14, AL_UNDER);
		//SetHelpItem("ilwisapp\\fill_sinks.htm");
		create();
}

int FormMapBrightnessTemperature::exec() 
{
  FormMapCreate::exec();
  FileName fn(sOutMap);
  String sExpr;
  if (fCIStrEqual(m_sSensor, "MODIS")){
	FileName fnMap31(m_sMapRadiance31); 
	m_sMapRadiance31 = fnMap31.sRelativeQuoted(false,fn.sPath());
	FileName fnMap32(m_sMapRadiance32); 
	m_sMapRadiance32 = fnMap32.sRelativeQuoted(false,fn.sPath());
	FileName fnBTMap32(m_sBTMap32); 
	m_sBTMap32 = fnBTMap32.sRelativeQuoted(false,fn.sPath());
	sExpr = String("MapBrightnessTemperature(%S,%S,%S,%S)",m_sSensor, m_sMapRadiance31,m_sMapRadiance32,m_sBTMap32);
  }
  else if (fCIStrEqual(m_sSensor, "ASTER")){
	FileName fnMap13(m_sMapRadiance13); 
	m_sMapRadiance13 = fnMap13.sRelativeQuoted(false,fn.sPath());
	FileName fnMap14(m_sMapRadiance14); 
	m_sMapRadiance14 = fnMap14.sRelativeQuoted(false,fn.sPath());
	FileName fnBTMap14(m_sBTMap14); 
	m_sBTMap14 = fnBTMap14.sRelativeQuoted(false,fn.sPath());
	sExpr = String("MapBrightnessTemperature(%S,%S,%S,%S)",m_sSensor, m_sMapRadiance13,m_sMapRadiance14,m_sBTMap14);
  }
  else{
	FileName fnMap(m_sMapRadiance); 
	m_sMapRadiance = fnMap.sRelativeQuoted(false,fn.sPath());
	String sSensor;
    sSensor = String("other");
	sExpr = String("MapBrightnessTemperature(%S,%S,%g)",sSensor,m_sMapRadiance,m_rWavelength);
  }
  execMapOut(sExpr);  
  return 0;
}
int FormMapBrightnessTemperature::SensorOnChange(Event *)
{
  m_fldSensor->StoreData();
  if(m_AddString == false)
  {
    m_fldSensor->AddString(String("MODIS"));
	m_fldSensor->AddString(String("ASTER"));
	m_fldSensor->AddString(String("Other Sensors"));
    m_sSensor = String("MODIS");
    m_fldSensor->SelectItem(m_sSensor);
    m_AddString = true;
  }
  if ((fCIStrEqual(m_sSensor, "MODIS"))){
	  m_fldBand1->Show();
	  m_fldBand2->Show();
	  m_fmcBT32->Show();
	  m_fldBand->Hide();
	  m_fldWavelength->Hide();

	  m_fldBand13->Hide();
	  m_fldBand14->Hide();
	  m_fmcBT14->Hide();
  }
  else if ((fCIStrEqual(m_sSensor, "ASTER"))){
	  m_fldBand1->Hide();
	  m_fldBand2->Hide();
	  m_fmcBT32->Hide();
	  m_fldBand->Hide();
	  m_fldWavelength->Hide();

	  m_fldBand13->Show();
	  m_fldBand14->Show();
	  m_fmcBT14->Show();
  }
  else{
	  m_fldBand1->Hide();
	  m_fldBand2->Hide();
	  m_fmcBT32->Hide();
	  m_fldBand->Show();
	  m_fldWavelength->Show();

	  m_fldBand13->Hide();
	  m_fldBand14->Hide();
	  m_fmcBT14->Hide();
  }

	return 1;
}

LRESULT Cmdemissivity(CWnd *wnd, const String& s)
{
	new FormMapEmissivity(wnd, s.c_str());
	return -1;
}

FormMapEmissivity::FormMapEmissivity(CWnd* mw, const char* sPar)
:FormMapCreate(mw, TR("Compute Surface Emissivity"))
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
						if (m_sMapRed == "")
							m_sMapRed = fn.sFullNameQuoted(true);
						else
							sOutMap = fn.sFullName(false);
			}
		}
		m_AddSensorString = false;
		StaticText *stSensor = new StaticText(root,TR("Sensor"));
		m_fldSensor = new FieldOneSelectTextOnly(root, &m_sSensor, false);
		m_fldSensor->Align(stSensor, AL_AFTER);
		m_fldSensor->SetCallBack((NotifyProc)&FormMapEmissivity::SensorOnChange);
		m_fldSensor->SetWidth(90);

		FieldBlank *fb = new FieldBlank(root, 0.1); // Used to force proper alignment
		fb->Align(stSensor, AL_UNDER);
		new FieldMap(root, TR("Input Red Map"), &m_sMapRed, new MapListerDomainType(".mpr", dmVALUE, false));
		new FieldMap(root, TR("Input Near Infrared Map"), &m_sMapNIR, new MapListerDomainType(".mpr", dmVALUE, false));
		m_fAlbedoMap = false;
		CheckBox* chkAlbedoMap = new CheckBox(root, TR("Input Albedo Map"), &m_fAlbedoMap);
		new FieldMap(chkAlbedoMap, "", &m_sMapAlbedo, new MapListerDomainType(".mpr", dmVALUE, false));
    
		FieldBlank *fb1 = new FieldBlank(root, 0); // Used to force proper alignment
		fb1->Align(chkAlbedoMap, AL_UNDER);
		initMapOut(false, false);
		m_fNdviMap = false;
		CheckBox* chkNdvi = new CheckBox(root, TR("Create NDVI Map"), &m_fNdviMap);
		FieldMapCreate* fmNdvi = new FieldMapCreate(chkNdvi, "", &m_sMapNdvi);
		m_fEmisDifMap = false;
		CheckBox* chkEmisDif = new CheckBox(root, TR("Create Emissivity Difference Map"), &m_fEmisDifMap);
		chkEmisDif->Align(chkNdvi, AL_UNDER);
		FieldMapCreate* fmEmisDif = new FieldMapCreate(chkEmisDif, "", &m_sMapEmisDif);
		m_fPvMap = false;
		CheckBox* chkPv = new CheckBox(root, TR("Create Vegetation Proportion Map"), &m_fPvMap);
		chkPv->Align(chkEmisDif, AL_UNDER);
		FieldMapCreate* fmPv = new FieldMapCreate(chkPv, "", &m_sMapPv);
    
		//SetHelpItem("ilwisapp\\fill_sinks.htm");
		create();
}

int FormMapEmissivity::exec() 
{
  FormMapCreate::exec();
  FileName fn(sOutMap);
  FileName fnMap(m_sMapRed);
  if (m_fAlbedoMap) 
  {
    FileName fnAlbedoMap(m_sMapAlbedo);
    m_sMapAlbedo = fnAlbedoMap.sRelativeQuoted(false,fn.sPath());
  }
  else
  m_sMapAlbedo = "";
  if (m_fNdviMap) 
  {
    FileName fnNdviMap(m_sMapNdvi);
    m_sMapNdvi = fnNdviMap.sRelativeQuoted(false,fn.sPath());
  }
  else
    m_sMapNdvi = "";
  if (m_fEmisDifMap) 
  {
    FileName fnEmisDifMap(m_sMapEmisDif);
    m_sMapEmisDif = fnEmisDifMap.sRelativeQuoted(false,fn.sPath());
  }
  else
    m_sMapEmisDif = "nomap";

  if (m_fPvMap) 
  {
    FileName fnPvMap(m_sMapPv);
    m_sMapPv = fnPvMap.sRelativeQuoted(false,fn.sPath());
  }
  else
    m_sMapPv = "nomap";
  	
  m_sMapRed = fnMap.sRelativeQuoted(false,fn.sPath());
  FileName fnNIRMap(m_sMapNIR);
  m_sMapNIR = fnNIRMap.sRelativeQuoted(false,fn.sPath());
  String sExpr = String("MapEmissivity(%S,%S,%S,%S,%S,%S,%S)",
		m_sMapRed,m_sMapNIR,m_sMapAlbedo,m_sSensor,m_sMapNdvi,m_sMapEmisDif,m_sMapPv);
  execMapOut(sExpr);  
  return 0;
}

int FormMapEmissivity::SensorOnChange(Event *)
{
  m_fldSensor->StoreData();
  if(m_AddSensorString == false)
  {
    m_fldSensor->AddString(String("MODIS"));
	m_fldSensor->AddString(String("ASTER"));
	m_fldSensor->AddString(String("AATSR"));
	
    m_sSensor = String("MODIS");
    m_fldSensor->SelectItem(m_sSensor);
    m_AddSensorString = true;
  }
	return 1;
}

LRESULT Cmdlandsurfacetemperature(CWnd *wnd, const String& s)
{
	new FormMapLandSurfaceTemperature(wnd, s.c_str());
	return -1;
}

FormMapLandSurfaceTemperature::FormMapLandSurfaceTemperature(CWnd* mw, const char* sPar)
:FormMapCreate(mw, TR("Compute Land Surface Temperature"))
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
						if (m_sMapBtemp12 == "")
							m_sMapBtemp12 = fn.sFullNameQuoted(true);
						else
							sOutMap = fn.sFullName(false);
			}
		}
		m_AddSensorString = false;
		StaticText *stSensor = new StaticText(root,TR("Sensor"));
		m_fldSensor = new FieldOneSelectTextOnly(root, &m_sSensor, false);
		m_fldSensor->Align(stSensor, AL_AFTER);
		m_fldSensor->SetCallBack((NotifyProc)&FormMapLandSurfaceTemperature::SensorOnChange);
		m_fldSensor->SetWidth(90);
		FieldBlank *fb = new FieldBlank(root, 0.1); // Used to force proper alignment
		fb->Align(stSensor, AL_UNDER);
		m_stModisBand31 = new StaticText(root,TR("Brightness Temperature Band31"));
		m_stAATSRBand1 = new StaticText(root,TR("Brightness Temperature Band1"));
		m_stAATSRBand1->Align(fb, AL_UNDER);
		m_stAsterBand13 = new StaticText(root,TR("Brightness Temperature Band13") );
		m_stAsterBand13->Align(fb, AL_UNDER);
		m_stModisBand32 = new StaticText(root,TR("Brightness Temperature Band32"));
		m_stModisBand32->Align(m_stModisBand31,AL_UNDER);
		FieldMap *fldBand1 = new FieldMap(root, "", &m_sMapBtemp12, new MapListerDomainType(".mpr", dmVALUE, false));
		fldBand1->Align(m_stAATSRBand1, AL_AFTER);
		m_stAATSRBand2 = new StaticText(root,TR("Brightness Temperature Band2"));
		m_stAATSRBand2->Align(m_stAATSRBand1,AL_UNDER);
		m_stAsterBand14 = new StaticText(root,TR("Brightness Temperature Band14"));
		m_stAsterBand14->Align(m_stAATSRBand1,AL_UNDER);
		FieldMap *fldBand2 = new FieldMap(root, "", &m_sMapBtemp11, new MapListerDomainType(".mpr", dmVALUE, false));
		fldBand2->Align(m_stAATSRBand2, AL_AFTER);
		FieldMap *fldEmis = new FieldMap(root, TR("Surface Emissivity"), &m_sMapEmissivity, new MapListerDomainType(".mpr", dmVALUE, false));
		fldEmis->Align(m_stAATSRBand2,AL_UNDER);
		new FieldMap(root, "Emissivity Difference", &m_sMapEmisDif, new MapListerDomainType(".mpr", dmVALUE, false));
		m_fInWaterVapor = false;
		CheckBox* chkInWaterVapor = new CheckBox(root, TR("Water vapor content map(g.cm^-2)"), &m_fInWaterVapor);
		new FieldMap(chkInWaterVapor, "", &m_sMapInWaterVapor, new MapListerDomainType(".mpr", dmVALUE, false));
    
		FieldBlank *fb1 = new FieldBlank(root, 0); // Used to force proper alignment
		fb1->Align(chkInWaterVapor, AL_UNDER);
		initMapOut(false, false);
		//SetHelpItem("ilwisapp\\fill_sinks.htm");
		create();
}


int FormMapLandSurfaceTemperature::exec() 
{
  FormMapCreate::exec();
  FileName fn(sOutMap);
  FileName fnMap(m_sMapBtemp12);
  if (m_fInWaterVapor) 
  {
    FileName fnInWaterVapor(m_sMapInWaterVapor);
    m_sMapInWaterVapor = fnInWaterVapor.sRelativeQuoted(false,fn.sPath());
  }
  else
    m_sMapInWaterVapor = "nomap";

  m_sMapBtemp12 = fnMap.sRelativeQuoted(false,fn.sPath());
  FileName fnBtemp11Map(m_sMapBtemp11);
  m_sMapBtemp11 = fnBtemp11Map.sRelativeQuoted(false,fn.sPath());
  FileName fnEmisMap(m_sMapEmissivity);
  m_sMapEmissivity = fnEmisMap.sRelativeQuoted(false,fn.sPath());
  FileName fnEmisDifMap(m_sMapEmisDif);
  m_sMapEmisDif = fnEmisDifMap.sRelativeQuoted(false,fn.sPath());
  String sExpr = String("MapLandSurfaceTemperature(%S,%S,%S,%S,%S,%S)",
						m_sSensor,
						m_sMapBtemp12,
						m_sMapBtemp11,
						m_sMapEmissivity,
						m_sMapEmisDif,
						m_sMapInWaterVapor
						);

  execMapOut(sExpr);  
  return 0;
}

int FormMapLandSurfaceTemperature::SensorOnChange(Event *)
{
  m_fldSensor->StoreData();
  if(m_AddSensorString == false)
  {
    m_fldSensor->AddString(String("MODIS"));
	m_fldSensor->AddString(String("ASTER"));
	m_fldSensor->AddString(String("AATSR"));
	
    m_sSensor = String("MODIS");
    m_fldSensor->SelectItem(m_sSensor);
    m_AddSensorString = true;
  }
  if ((fCIStrEqual(m_sSensor, "MODIS"))){
	  m_stModisBand31->Show();
	  m_stModisBand32->Show();
	  m_stAATSRBand1->Hide();
	  m_stAATSRBand2->Hide();
	  m_stAsterBand13->Hide();
	  m_stAsterBand14->Hide();	
  }
  else if ((fCIStrEqual(m_sSensor, "ASTER"))){
	  m_stAsterBand13->Show();
	  m_stAsterBand14->Show();	
	  m_stModisBand31->Hide();
	  m_stModisBand32->Hide();
	  m_stAATSRBand1->Hide();
	  m_stAATSRBand2->Hide();
  }
  else{
	  m_stModisBand31->Hide();
	  m_stModisBand32->Hide();
	  m_stAsterBand13->Hide();
	  m_stAsterBand14->Hide();	
	  m_stAATSRBand1->Show();
	  m_stAATSRBand2->Show();
  }
	return 1;
}

LRESULT Cmdalbedo(CWnd *wnd, const String& s)
{
	new FormMapAlbedo(wnd, s.c_str());
	return -1;
}

FormMapAlbedo::FormMapAlbedo(CWnd* mw, const char* sPar)
:FormMapCreate(mw, TR("Compute Land Surface Albedo"))
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
						if (m_sMapBand1 == "")
							m_sMapBand1 = fn.sFullNameQuoted(true);
						else
							sOutMap = fn.sFullName(false);
			}
		}
		m_AddString = false;
		StaticText *stMethod = new StaticText(root,TR("Sensor"));
		m_fldMethod = new FieldOneSelectTextOnly(root, &m_sMethod, false);
		m_fldMethod->Align(stMethod, AL_AFTER);
		m_fldMethod->SetCallBack((NotifyProc)&FormMapAlbedo::MethodOnChange);
		m_fldMethod->SetWidth(90);

		m_stBand1 = new StaticText(root,TR("Band1"));
		m_stBand1->Align(stMethod, AL_UNDER);
		FieldMap *fldRed = new FieldMap(root, "", &m_sMapBand1, new MapListerDomainType(".mpr", dmVALUE, false));
		fldRed->Align(m_stBand1, AL_AFTER);
		m_stBand2 = new StaticText(root,TR("Band2"));
		m_stBand2->Align(m_stBand1, AL_UNDER);
		m_stBandAster3 = new StaticText(root,TR("Band3"));
		m_stBandAster3->Align(m_stBand1, AL_UNDER);
		FieldMap *fldNir = new FieldMap(root, "", &m_sMapBand2, new MapListerDomainType(".mpr", dmVALUE, false));
		fldNir->Align(m_stBand2, AL_AFTER);
		m_stVis = new StaticText(root,TR("Visible Band"));
		m_stVis->Align(stMethod, AL_UNDER);
		m_stNIR = new StaticText(root,TR("Near IR Band"));
		m_stNIR->Align(m_stBand1, AL_UNDER);
		
		m_stBand3 = new StaticText(root,TR("Band3"));
		m_stBand3->Align(m_stBand2, AL_UNDER);
		m_fldBand3 = new FieldMap(root, "", &m_sBand3, new MapListerDomainType(".mpr", dmVALUE, false));
		m_fldBand3->Align(m_stBand3, AL_AFTER);
		m_stBand4 = new StaticText(root,TR("Band4"));
		m_stBand4->Align(m_stBand3, AL_UNDER);
		m_fldBand4 = new FieldMap(root, "", &m_sBand4, new MapListerDomainType(".mpr", dmVALUE, false));
		m_fldBand4->Align(m_stBand4, AL_AFTER);
		m_stBand5 = new StaticText(root,TR("Band5"));
		m_stBand5->Align(m_stBand4, AL_UNDER);
		m_fldBand5 = new FieldMap(root, "", &m_sBand5, new MapListerDomainType(".mpr", dmVALUE, false));
		m_fldBand5->Align(m_stBand5, AL_AFTER);
		m_stBand7 = new StaticText(root,TR("Band7"));
		m_stBand7->Align(m_stBand5, AL_UNDER);
		m_fldBand7 = new FieldMap(root, "", &m_sBand7, new MapListerDomainType(".mpr", dmVALUE, false));
		m_fldBand7->Align(m_stBand7, AL_AFTER);
		
		m_stBandAster5 = new StaticText(root,TR("Band5"));
		m_stBandAster5->Align(m_stBand2, AL_UNDER);
		m_stBandAster6 = new StaticText(root,TR("Band6"));
		m_stBandAster6->Align(m_stBandAster5, AL_UNDER);
		m_stBandAster8 = new StaticText(root,TR("Band8"));
		m_stBandAster8->Align(m_stBandAster6, AL_UNDER);
		m_stBandAster9 = new StaticText(root,TR("Band9"));
		m_stBandAster9->Align(m_stBandAster8, AL_UNDER);
				
		FieldBlank *fb1 = new FieldBlank(root, 0); // Used to force proper alignment
		fb1->Align(m_stBand7, AL_UNDER);
		initMapOut(false, false);
		//SetHelpItem("ilwisapp\\fill_sinks.htm");
		create();
}


int FormMapAlbedo::exec() 
{
  FormMapCreate::exec();
  FileName fn(sOutMap);
  FileName fnMap(m_sMapBand1);
  FileName fnMapNIR(m_sMapBand2);
  String sMethod;
  String sExpr;
  if ((fCIStrEqual(m_sMethod, "modis") || fCIStrEqual(m_sMethod, "aster"))){
    FileName fnBand3(m_sBand3);
    m_sBand3 = fnBand3.sRelativeQuoted(false,fn.sPath());
	FileName fnBand4(m_sBand4);
    m_sBand4 = fnBand4.sRelativeQuoted(false,fn.sPath());
	FileName fnBand5(m_sBand5);
    m_sBand5 = fnBand5.sRelativeQuoted(false,fn.sPath());
	FileName fnBand7(m_sBand7);
    m_sBand7 = fnBand7.sRelativeQuoted(false,fn.sPath());
	if (fCIStrEqual(m_sMethod, "modis"))
		sMethod = "modis";
	else
		sMethod = "aster";
	sExpr = String("MapAlbedo(%S,%S,%S,%S,%S,%S,%S)",
						m_sMapBand1,
						m_sMapBand2,
						sMethod,
						m_sBand3,
						m_sBand4,
						m_sBand5,
						m_sBand7);
  }
  else {
	sMethod = "using2bands";
	sExpr = String("MapAlbedo(%S,%S,%S)",
						m_sMapBand1,
						m_sMapBand2,
						sMethod);
		 
  }
  execMapOut(sExpr);  
  return 0;
}

int FormMapAlbedo::MethodOnChange(Event *)
{
  m_fldMethod->StoreData();
  if(m_AddString == false)
  {
    //m_fldMethod->AddString(String("using visible and near IR bands"));
	m_fldMethod->AddString(String("MODIS"));
	m_fldMethod->AddString(String("AATSR"));
	m_fldMethod->AddString(String("ASTER"));
	//m_fldMethod->AddString(String("Other Sensors"));
    m_sMethod = String("MODIS");
    m_fldMethod->SelectItem(m_sMethod);
    m_AddString = true;
  }
  //m_iClasses = m_sClasses.iVal();
  if ((fCIStrEqual(m_sMethod, "MODIS"))){
	  m_stBand3->Show();
	  m_stBand4->Show();
	  m_stBand5->Show();
	  m_stBand7->Show();
	  
	  m_fldBand3->Show();
	  m_fldBand4->Show();
	  m_fldBand5->Show();
	  m_fldBand7->Show();
	  m_stBand1->Show();
	  m_stBand2->Show();
	  m_stNIR->Hide();
	  m_stVis->Hide();
	  
	  m_stBandAster3->Hide();
	  m_stBandAster5->Hide();
	  m_stBandAster6->Hide();
	  m_stBandAster8->Hide();
	  m_stBandAster9->Hide();
	  
  }
  else if((fCIStrEqual(m_sMethod, "ASTER"))){
	  
	  m_fldBand3->Show();
	  m_fldBand4->Show();
	  m_fldBand5->Show();
	  m_fldBand7->Show();
	  m_stBand1->Show();
	  m_stBand2->Hide();
	  m_stBand3->Hide();
	  m_stBand4->Hide();
	  m_stBand5->Hide();
	  m_stBand7->Hide();
	  m_stBandAster3->Show();
	  m_stBandAster5->Show();
	  m_stBandAster6->Show();
	  m_stBandAster8->Show();
	  m_stBandAster9->Show();
	  m_stNIR->Hide();
	  m_stVis->Hide();	
  }
  else{
	  m_stBand1->Hide();
	  m_stBand2->Hide();
	  m_stNIR->Show();
	  m_stVis->Show();
	  m_stBand3->Hide();
	  m_stBand4->Hide();
	  m_stBand5->Hide();
	  m_stBand7->Hide();
	  m_fldBand3->Hide();
	  m_fldBand4->Hide();
	  m_fldBand5->Hide();
	  m_fldBand7->Hide();
	  m_stBandAster3->Hide();
	  m_stBandAster5->Hide();
	  m_stBandAster6->Hide();
	  m_stBandAster8->Hide();
	  m_stBandAster9->Hide();
	  
  }
	return 1;
}

LRESULT Cmdsmac(CWnd *wnd, const String& s)
{
	new FormMapSmac(wnd, s.c_str());
	return -1;
}

FormMapSmac::FormMapSmac(CWnd* mw, const char* sPar)
:FormMapCreate(mw, TR("Atmospheric Effect Correction (SMAC)"))
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
					if (m_sMapRef == "")
						m_sMapRef = fn.sFullNameQuoted(true);
					else
						sOutMap = fn.sFullName(false);
		}
	}

	new FieldDataType(root, TR("Top of atmosphere reflectance channel"), &m_sMapRef, new MapListerDomainType(".mpr", dmVALUE, false),true);
	new FieldDataType(root,TR("Coefficient file for sensor and channel"), &m_coef_file, ".*.dat", true, 0, FileName(getEngine()->getContext()->sStdDir() + "\\smac_coefs\\"), false);
	new StaticText(root, TR("Atmospheric correction data"));
	m_fMapOpticalDepth = false;
	m_chkMapOpticalDepth = new CheckBox(root, TR("Optical thickness map (at 550 nm)"), &m_fMapOpticalDepth);
	m_chkMapOpticalDepth ->SetCallBack((NotifyProc)&FormMapSmac::MapOnSelect);
	new FieldDataType(m_chkMapOpticalDepth , "", &m_sMapOpticalDepth, new MapListerDomainType(".mpr", dmVALUE, false), true);
	m_r_taup550 = rUNDEF;
	ValueRange vrOpticalDepth(0.05, 0.8, 0.01);		  
	m_fldOpticalDepth = new FieldReal(root, "", &m_r_taup550, ValueRange(0.05, 0.8, 0.0001)); 
	m_fldOpticalDepth->Align(m_chkMapOpticalDepth,AL_AFTER);

	m_fMapWaterVapor = false;
	m_chkMapWaterVapor = new CheckBox(root, TR("Water vapor content map (g/cm2)"), &m_fMapWaterVapor);
	m_chkMapWaterVapor->Align(m_chkMapOpticalDepth , AL_UNDER);
	m_chkMapWaterVapor->SetCallBack((NotifyProc)&FormMapSmac::MapOnSelect);
	new FieldDataType(m_chkMapWaterVapor, "", &m_sMapWaterVapor, new MapListerDomainType(".mpr", dmVALUE, false), true);
	m_r_uh2o = rUNDEF;
	m_fldWaterVapor = new FieldReal(root, "", &m_r_uh2o , ValueRange(0.0, 6.0, 0.0001)); 
	m_fldWaterVapor->Align(m_chkMapWaterVapor,AL_AFTER);

	m_fMapOzoneContent= false;
	m_chkMapOzoneContent= new CheckBox(root, TR("Ozone content map (atm-cm)"), &m_fMapOzoneContent);
	m_chkMapOzoneContent->Align(m_chkMapWaterVapor, AL_UNDER);
	m_chkMapOzoneContent->SetCallBack((NotifyProc)&FormMapSmac::MapOnSelect);
	new FieldDataType(m_chkMapOzoneContent, "", &m_sMapOzoneContent, new MapListerDomainType(".mpr", dmVALUE, false), true);
	m_r_uo3 = rUNDEF;
	m_fldOzoneContent= new FieldReal(root, "", &m_r_uo3, ValueRange(0.0, 0.7, 0.0001)); 
	m_fldOzoneContent->Align(m_chkMapOzoneContent,AL_AFTER);

	ValueRange vr(-1e300, 1e300, 1e-2);
	m_fMapSurfacePressure= false;
	m_chkMapSurfacePressure= new CheckBox(root, TR("Surface pressure map (hPa)"), &m_fMapSurfacePressure);
	m_chkMapSurfacePressure->Align(m_chkMapOzoneContent, AL_UNDER);
	m_chkMapSurfacePressure->SetCallBack((NotifyProc)&FormMapSmac::MapOnSelect);
	new FieldDataType(m_chkMapSurfacePressure, "", &m_sMapSurfacePressure, new MapListerDomainType(".mpr", dmVALUE, false), true);
	m_r_pression= rUNDEF;
	m_fldSurfacePressure= new FieldReal(root, "", &m_r_pression, vr); 
	m_fldSurfacePressure->Align(m_chkMapSurfacePressure,AL_AFTER);

	FieldBlank *fb2 = new FieldBlank(root, 0.2);
	fb2->Align(m_chkMapSurfacePressure, AL_UNDER);
	ValueRange vr2(-360, 360, 0.0001);
	StaticText* st2 = new StaticText(root, TR("Sun/satellite angle data"));
	st2->Align(fb2, AL_UNDER);
	m_fMapSolarZenithAngle= false;
	m_chkMapSolarZenithAngle= new CheckBox(root, TR("Solar zenith angle map (degrees)"), &m_fMapSolarZenithAngle);
	m_chkMapSolarZenithAngle->Align(st2, AL_UNDER);
	m_chkMapSolarZenithAngle->SetCallBack((NotifyProc)&FormMapSmac::MapOnSelect);
	new FieldDataType(m_chkMapSolarZenithAngle, "", &m_sMapSolarZenithAngle, new MapListerDomainType(".mpr", dmVALUE, false), true);
	m_r_tetas= rUNDEF;
	m_fldSolarZenithAngle= new FieldReal(root, "", &m_r_tetas, vr2); 
	m_fldSolarZenithAngle->Align(m_chkMapSolarZenithAngle,AL_AFTER);

	m_fMapSolarAzimutAngle= false;
	m_chkMapSolarAzimutAngle= new CheckBox(root, TR("Solar azimuth angle map (degrees)"), &m_fMapSolarAzimutAngle);
	m_chkMapSolarAzimutAngle->Align(m_chkMapSolarZenithAngle, AL_UNDER);
	m_chkMapSolarAzimutAngle->SetCallBack((NotifyProc)&FormMapSmac::MapOnSelect);
	new FieldDataType(m_chkMapSolarAzimutAngle, "", &m_sMapSolarAzimutAngle, new MapListerDomainType(".mpr", dmVALUE, false), true);
	m_r_phis= rUNDEF;
	m_fldSolarAzimutAngle= new FieldReal(root, "", &m_r_phis, vr2); 
	m_fldSolarAzimutAngle->Align(m_chkMapSolarAzimutAngle,AL_AFTER);

	m_fMapViewZenithAngle= false;
	m_chkMapViewZenithAngle= new CheckBox(root, TR("Sensor zenith angle map (degrees)"), &m_fMapViewZenithAngle);
	m_chkMapViewZenithAngle->Align(m_chkMapSolarAzimutAngle, AL_UNDER);
	m_chkMapViewZenithAngle->SetCallBack((NotifyProc)&FormMapSmac::MapOnSelect);
	new FieldDataType(m_chkMapViewZenithAngle, "", &m_sMapViewZenithAngle, new MapListerDomainType(".mpr", dmVALUE, false), true);
	m_r_tetav= rUNDEF;
	m_fldViewZenithAngle= new FieldReal(root, "", &m_r_tetav, vr2); 
	m_fldViewZenithAngle->Align(m_chkMapViewZenithAngle,AL_AFTER);

	m_fMapViewAzimutAngle= false;
	m_chkMapViewAzimutAngle= new CheckBox(root, TR("Sensor azimuth angle map (degrees)"), &m_fMapViewAzimutAngle);
	m_chkMapViewAzimutAngle->Align(m_chkMapViewZenithAngle, AL_UNDER);
	m_chkMapViewAzimutAngle->SetCallBack((NotifyProc)&FormMapSmac::MapOnSelect);
	new FieldDataType(m_chkMapViewAzimutAngle, "", &m_sMapViewAzimutAngle, new MapListerDomainType(".mpr", dmVALUE, false), true);
	m_r_phiv= rUNDEF;
	m_fldViewAzimutAngle= new FieldReal(root, "", &m_r_phiv, vr2); 
	m_fldViewAzimutAngle->Align(m_chkMapViewAzimutAngle,AL_AFTER);
	
	FieldBlank *fb3 = new FieldBlank(root, 0.5); // Used to force proper alignment	
    fb3->Align(m_chkMapViewAzimutAngle, AL_UNDER);
    	
    initMapOut(false, false);
    SetHelpItem("ilwisapp\\smac_dialog_box.htm");
	create();
}

int FormMapSmac::exec() 
{
  FormMapCreate::exec();
  FileName fn(sOutMap,".mpr", TRUE);

  FileName fnMapRef(m_sMapRef); 
  m_sMapRef = fnMapRef.sRelativeQuoted(false,fn.sPath());
  
  if (!m_fMapOpticalDepth)
	m_sMapOpticalDepth = String("%f", m_r_taup550);
  if (!m_fMapWaterVapor)
	m_sMapWaterVapor = String("%f", m_r_uh2o);
  if (!m_fMapOzoneContent)
	m_sMapOzoneContent = String("%f", m_r_uo3);
  if (!m_fMapSurfacePressure)
	m_sMapSurfacePressure = String("%f", m_r_pression);
  if (!m_fMapSolarZenithAngle)
	m_sMapSolarZenithAngle = String("%f", m_r_tetas);
  if (!m_fMapSolarAzimutAngle)
	m_sMapSolarAzimutAngle = String("%f", m_r_phis);
  if (!m_fMapViewZenithAngle)
	m_sMapViewZenithAngle = String("%f", m_r_tetav);
  if (!m_fMapViewAzimutAngle)
	m_sMapViewAzimutAngle = String("%f", m_r_phiv);
  	
  String sExpr("MapSMAC(%S,%S,%li,%S,%li,%S,%li,%S,%li,%S,%li,%S,%li,%S,%li,%S,%li,%S)", 
				m_sMapRef, 
				m_coef_file,
				m_fMapOpticalDepth, m_sMapOpticalDepth,
				m_fMapWaterVapor, m_sMapWaterVapor,
				m_fMapOzoneContent, m_sMapOzoneContent,
				m_fMapSurfacePressure, m_sMapSurfacePressure,
				m_fMapSolarZenithAngle, m_sMapSolarZenithAngle,
				m_fMapSolarAzimutAngle, m_sMapSolarAzimutAngle,
				m_fMapViewZenithAngle, m_sMapViewZenithAngle,
				m_fMapViewAzimutAngle, m_sMapViewAzimutAngle);
				
  execMapOut(sExpr);  	
  return 0;
}

int FormMapSmac::MapOnSelect(Event *)
{
    m_chkMapOpticalDepth->StoreData();
    if (m_fMapOpticalDepth){
      m_fldOpticalDepth->Hide(); 
    }
    else{
      m_fldOpticalDepth->Show();
    }

	m_chkMapWaterVapor->StoreData();
    if (m_fMapWaterVapor){
      m_fldWaterVapor->Hide(); 
    }
    else{
      m_fldWaterVapor->Show();
    }

	m_chkMapOzoneContent->StoreData();
    if (m_fMapOzoneContent){
      m_fldOzoneContent->Hide(); 
    }
    else{
      m_fldOzoneContent->Show();
    }

	m_chkMapSurfacePressure->StoreData();
    if (m_fMapSurfacePressure){
      m_fldSurfacePressure->Hide(); 
    }
    else{
      m_fldSurfacePressure->Show();
    }

	m_chkMapSolarZenithAngle->StoreData();
    if (m_fMapSolarZenithAngle){
      m_fldSolarZenithAngle->Hide(); 
    }
    else{
      m_fldSolarZenithAngle->Show();
    }

	m_chkMapSolarAzimutAngle->StoreData();
    if (m_fMapSolarAzimutAngle){
      m_fldSolarAzimutAngle->Hide(); 
    }
    else{
      m_fldSolarAzimutAngle->Show();
    }

	m_chkMapViewZenithAngle->StoreData();
    if (m_fMapViewZenithAngle){
      m_fldViewZenithAngle->Hide(); 
    }
    else{
      m_fldViewZenithAngle->Show();
    }

	m_chkMapViewAzimutAngle->StoreData();
    if (m_fMapViewAzimutAngle){
      m_fldViewAzimutAngle->Hide(); 
    }
    else{
      m_fldViewAzimutAngle->Show();
    }

	return 1;
}

LRESULT Cmdwatervapour(CWnd *wnd, const String& s)
{
	new FormMapWaterVapour(wnd, s.c_str());
	return -1;
}

FormMapWaterVapour::FormMapWaterVapour(CWnd* mw, const char* sPar)
:FormMapCreate(mw, "Compute Atmospheric Water Vapour")
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
						if (m_sMapBand1 == "")
							m_sMapBand1 = fn.sFullNameQuoted(true);
						else
							sOutMap = fn.sFullName(false);
			}
		}
		m_AddSensorString = false;
		StaticText *stSensor = new StaticText(root,"Sensor");
		m_fldSensor = new FieldOneSelectTextOnly(root, &m_sSensor, false);
		m_fldSensor->Align(stSensor, AL_AFTER);
		m_fldSensor->SetCallBack((NotifyProc)&FormMapWaterVapour::SensorOnChange);
		m_fldSensor->SetWidth(90);
		FieldBlank *fb = new FieldBlank(root, 0.1); // Used to force proper alignment
		fb->Align(stSensor, AL_UNDER);
		m_fldBand1 = new FieldMap(root, "Channel 1", &m_sMapBand1, new MapListerDomainType(".mpr", dmVALUE, false));
		m_stAATSRBand2 = new StaticText(root,"Channel 2");
		m_stBand2 = new StaticText(root,"Radiance channel 2");
		m_stBand2->Align(m_fldBand1, AL_UNDER);
		m_fldBand2 = new FieldMap(root, "", &m_sMapBand2, new MapListerDomainType(".mpr", dmVALUE, false));
		m_fldBand2->Align(m_stAATSRBand2,AL_AFTER);

		m_fldBand17 = new FieldMap(root, "Radiance channel 17", &m_sMapBand17, new MapListerDomainType(".mpr", dmVALUE, false));
		m_fldBand17->Align(m_stAATSRBand2, AL_UNDER);
		m_fldBand18 = new FieldMap(root, "Radiance channel 18", &m_sMapBand18, new MapListerDomainType(".mpr", dmVALUE, false));
		m_fldBand19 = new FieldMap(root, "Radiance channel 19", &m_sMapBand19, new MapListerDomainType(".mpr", dmVALUE, false));
		
		FieldBlank *fb1 = new FieldBlank(root, 0); // Used to force proper alignment
		fb1->Align(m_fldBand19, AL_UNDER);
		initMapOut(false, false);
		//SetHelpItem("ilwisapp\\fill_sinks.htm");
		create();
}

int FormMapWaterVapour::exec() 
{
  FormMapCreate::exec();
  FileName fn(sOutMap);
  String sExpr;
  if ((fCIStrEqual(m_sSensor, "MODIS"))){
	  FileName fnBand2(m_sMapBand2);
	  m_sMapBand2 = fnBand2.sRelativeQuoted(false,fn.sPath());
	  FileName fnBand17(m_sMapBand17);
	  m_sMapBand17 = fnBand17.sRelativeQuoted(false,fn.sPath());
	  FileName fnBand18(m_sMapBand18);
	  m_sMapBand18 = fnBand18.sRelativeQuoted(false,fn.sPath());
	  FileName fnBand19(m_sMapBand19);
	  m_sMapBand19 = fnBand19.sRelativeQuoted(false,fn.sPath());
	  sExpr = String("MapWaterVapour(%S,%S,%S,%S,%S)",
						m_sSensor,
						m_sMapBand2,
						m_sMapBand17,
						m_sMapBand18,
						m_sMapBand19
						);

  }
  else if ((fCIStrEqual(m_sSensor, "AATSR"))){
	  FileName fnBand1(m_sMapBand1);
	  m_sMapBand1 = fnBand1.sRelativeQuoted(false,fn.sPath());
	  FileName fnBand2(m_sMapBand2);
	  m_sMapBand2 = fnBand2.sRelativeQuoted(false,fn.sPath());
	  sExpr = String("MapWaterVapour(%S,%S,%S)",
						m_sSensor,
						m_sMapBand1,
						m_sMapBand2
						);
  }
  execMapOut(sExpr);  
  return 0;
}

int FormMapWaterVapour::SensorOnChange(Event *)
{
  m_fldSensor->StoreData();
  if(m_AddSensorString == false)
  {
    m_fldSensor->AddString(String("MODIS"));
	m_fldSensor->AddString(String("AATSR"));
	
    m_sSensor = String("MODIS");
    m_fldSensor->SelectItem(m_sSensor);
    m_AddSensorString = true;
  }
  if ((fCIStrEqual(m_sSensor, "MODIS"))){
	  m_fldBand1->Hide();
	  m_stAATSRBand2->Hide();
	  m_stBand2->Show();	
	  m_fldBand17->Show();
	  m_fldBand18->Show();
	  m_fldBand19->Show();
  }
  else if ((fCIStrEqual(m_sSensor, "AATSR"))){
	  m_fldBand1->Show();
	  m_stAATSRBand2->Show();
	  m_stBand2->Hide();	
	  m_fldBand17->Hide();
	  m_fldBand18->Hide();
	  m_fldBand19->Hide();
  }
	return 1;
}

LRESULT Cmdsoilmoisture(CWnd *wnd, const String& s)
{
	new FormMapSoilMoisture(wnd, s.c_str());
	return -1;
}

FormMapSoilMoisture::FormMapSoilMoisture(CWnd* mw, const char* sPar)
:FormMapCreate(mw, "Estimation of Soil Moisture")
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
						if (m_sMapSand == "")
							m_sMapSand = fn.sFullNameQuoted(true);
						else
							sOutMap = fn.sFullName(false);
			}
		}
		FieldMap *fldSand = new FieldMap(root, "Fraction of sand", &m_sMapSand, new MapListerDomainType(".mpr", dmVALUE, false));
		FieldMap *fldClay = new FieldMap(root, "Fraction of clay", &m_sMapClay, new MapListerDomainType(".mpr", dmVALUE, false));
		FieldMap *fldFc = new FieldMap(root, "Leaf area index", &m_sMapFc, new MapListerDomainType(".mpr", dmVALUE, false));
		FieldMap *fldLc = new FieldMap(root, "Land cover", &m_sMapLc, new MapListerDomainType(".mpr", dmIDENT, false));
		
		m_sAscatDir = IlwWinApp()->sGetCurDir();
		FormEntry* fb = new FieldBrowseDir(root, "Directory for Ascat data", "", &m_sAscatDir);
		fb->SetWidth(100);
		//FormEntry *fb = new FieldBrowseDir(fg, "", strCopyTo, &sDirectory);
		
		FieldBlank *fb1 = new FieldBlank(root, 0); // Used to force proper alignment
		fb1->Align(fb, AL_UNDER);
		initMapOut(false, false);
		//SetHelpItem("ilwisapp\\fill_sinks.htm");
		create();
}


int FormMapSoilMoisture::exec() 
{
  FormMapCreate::exec();
  FileName fn(sOutMap);
  FileName fnMapSand(m_sMapSand);
  m_sMapSand = fnMapSand.sRelativeQuoted(false,fn.sPath());
  FileName fnMapClay(m_sMapClay);
  m_sMapClay = fnMapClay.sRelativeQuoted(false,fn.sPath());
  FileName fnMapFc(m_sMapFc);
  m_sMapFc = fnMapFc.sRelativeQuoted(false,fn.sPath());
  FileName fnMapLc(m_sMapLc);
  m_sMapLc = fnMapLc.sRelativeQuoted(false,fn.sPath());
  String sExpr = String("MapSoilMoistureFromAscat(%S,%S,%S,%S,%S)",
						m_sMapSand,
						m_sMapClay,
						m_sMapFc,
						m_sMapLc,
						m_sAscatDir
						);

  execMapOut(sExpr);  
  return 0;
}

LRESULT Cmdsoilmoisture_ascat_l2(CWnd *wnd, const String& s)
{
	new FormMapSoilMoisture_l2(wnd, s.c_str());
	return -1;
}

FormMapSoilMoisture_l2::FormMapSoilMoisture_l2(CWnd* mw, const char* sPar)
:FormMapCreate(mw, "ASCAT Level 2 Soil Moisture Products")
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
						if (m_sMapLc == "")
							m_sMapLc = fn.sFullNameQuoted(true);
						else
							sOutMap = fn.sFullName(false);
			}
		}
		FieldMap *fldLc = new FieldMap(root, "Land Cover ", &m_sMapLc, new MapListerDomainType(".mpr", dmIDENT, false));
		m_sAscatDir = IlwWinApp()->sGetCurDir();
		FormEntry* fb = new FieldBrowseDir(root, "Directory for ASCAT L2 data", "", &m_sAscatDir);
		fb->SetWidth(100);
		
		FieldBlank *fb1 = new FieldBlank(root, 0); // Used to force proper alignment
		fb1->Align(fb, AL_UNDER);
		initMapOut(false, false);
		//SetAppHelpTopic(htpFillSinks);
		create();
}


int FormMapSoilMoisture_l2::exec() 
{
  FormMapCreate::exec();
  FileName fn(sOutMap);
  FileName fnMapLc(m_sMapLc);
  m_sMapLc = fnMapLc.sRelativeQuoted(false,fn.sPath());
  String sExpr = String("MapSoilMoistureFromAscat_l2(%S,%S)",
						m_sMapLc,
						m_sAscatDir
						);
  execMapOut(sExpr);  
  return 0;
}