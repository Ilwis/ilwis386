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
#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\fldcol.h"
#include "Client\Editors\SMCE\FormAggregateValue.h"
#include "Client\FormElements\objlist.h"
#include "Client\FormElements\FieldMultiObjectSelect.h"
#include "Client\FormElements\FldOneSelectTextOnly.h"
#include "Engine\SpatialReference\Gr.h"
#include "Client\ilwis.h"
#include "Engine\Scripting\CalculateAggregateValue.h"
#include "Client\Editors\SMCE\FormAggregateValueMulti.h"
#include "Engine\Scripting\InstAggregateValue.h"
#include "Headers\Hs\Appforms.hs"

AggregateValueOperationStruct::AggregateValueOperationStruct() :
	rPower(rUNDEF)
{
}
	
SelectedFMOItem::SelectedFMOItem()
{
}

SelectedFMOItem::SelectedFMOItem(FMOSItem *item) :
	FMOSItem(*item)
{
	SelectedFMOItem *sfi = dynamic_cast<SelectedFMOItem *>(item);
	if ( sfi)
	{
		sAttributeColumn = sfi->sGetAttributeColumn();
		if ( sfi->rGetResult() != rUNDEF)
			sAggregateResult = String("%f", sfi->rGetResult());
	}
}

SelectedFMOItem::SelectedFMOItem(const AttributeFileName& fn, const String& sCol, const String& sRes) :
	FMOSItem(fn),
    sAggregateResult(sRes),		
	sAttributeColumn(sCol)
{
}		

bool SelectedFMOItem::fEqual(FMOSItem *item)
{
	SelectedFMOItem *it = dynamic_cast<SelectedFMOItem *>(item);
	if (it)
		return it->sGetAttributeColumn() == sAttributeColumn && fnItem() == it->fnItem();
	else
		return FMOSItem::fEqual(item);
}

FMOSItem* SelectedFMOItem::Clone()	
{
	return new SelectedFMOItem(fnItem(),sAttributeColumn, sAggregateResult); 
}

void SelectedFMOItem::SetAggregateResult(const String& sR)
{
	sAggregateResult = sR;
}

void SelectedFMOItem::SetAttributeColumn(const String& sCol)
{
	sAttributeColumn = sCol;
}

String SelectedFMOItem::sGetAttributeColumn() const
{
	return sAttributeColumn;
}

double SelectedFMOItem::rGetResult() const
{
	return sAggregateResult.rVal();
}

// return value for what is shown in the listbox. In this case a concattenation of
// file, attrib column, result
String SelectedFMOItem::sItem() const
{
	String sN;
	if ( sAttributeColumn == "")
		sN = fnItem().sRelative(false);
	else
	    sN = fnItem().sRelative(false) + "." + sAttributeColumn;
	
	if ( sN.size() > 22)
	{
		String sLeft = sN.substr(0, 10);
		String sRight = sN.substr(sN.size() - 10, 10);
		sN = sLeft + "..." + sRight;
	}
	String sR("%-*S %S.mpr", 24, sN, sAggregateResult);	
	return sR;
}


FormAggregateValueMulti::FormAggregateValueMulti(CWnd* parent, vector<AttributeFileName>* vfnInputMaps, 
						vector< AggregateValueStruct >* vrOutputV, const dmType fd, const String& operation) :
FormBaseDialog(parent, ((fd&dmCLASS)==dmCLASS)?SAFTitleAggrValueSliced:SAFTitleAggrValueS, true, false, true),		
	arrInput(0),
	vrOutputValues(vrOutputV),
	m_fd(fd),
	ts(0),
	fEnableCheckData(false)
{
	if ( vfnInputMaps)
	{
		arrInput = new vector<AttributeFileName>(vfnInputMaps->size())	;	
		copy(vfnInputMaps->begin(), vfnInputMaps->end(), arrInput->begin());
	}		
	create();
}

FormAggregateValueMulti::FormAggregateValueMulti(CWnd* parent)
: 	FormBaseDialog(parent, SAFTitleAggrValue, true, false, true),
  arrInput(0),
  vrOutputValues(0),
	m_fd((dmType)(dmVALUE|dmCLASS)),
	ts(0)
{
	create();
}

void FormAggregateValueMulti::create()
{
	ic = 0;
	int iImg = IlwWinApp()->iImage("CalculationMultiple");
	HICON hIco = IlwWinApp()->ilSmall.ExtractIcon(iImg);
	SetIcon(hIco,FALSE);
		
	if ( arrInput == 0)
		arrInput = & vfnDummy;
	
	fbs |= fbsNOCANCELBUTTON | fbsOKHASCLOSETEXT;
	
	FieldGroup *fgrp = new FieldGroup(root);
	FieldGroup *fgrp2 = new FieldGroup(fgrp);

	favf = new FieldAggregateValueFunc(fgrp2, SAFUiOperation, &sOperation, m_fd);
	favf->SetCallBack((NotifyProc)(&FormAggregateValueMulti::HandleOperationChanges));
	fgrp->SetCallBack((NotifyProc)(&FormAggregateValueMulti::Init));
	new FieldBlank(fgrp2, 0.2);

	ObjectExtensionLister *obj = new MapListerDomainType(dmVALUE | dmBOOL | dmIMAGE, true);
	fld = new FieldMultiAggregateValueSelect(fgrp2, SAFUiInputMaps, obj, *arrInput, &ts);
	fld->SetSelectionFunction(this, (NotifyProc)&FormAggregateValueMulti::Selection);

	FieldBlank *fb = new FieldBlank(fgrp2, 0.2);
	if ( arrInput->size() == 0)	
	{	
		HICON hIcon;
		hIcon = (HICON)LoadImage(IlwWinApp()->m_hInstance,"GrfIcon", IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);     
		ic = new StaticIcon(fgrp2, hIcon, false);
		ic->Align(fb, AL_UNDER);
		ic->Hide();
		ic->psn->iPosX += 10;

		fsGeoref = new FieldString(fgrp2, &sGeoref, ES_AUTOHSCROLL|WS_TABSTOP|WS_GROUP|ES_READONLY);
		fsGeoref->Align(ic, AL_AFTER);
		fsGeoref->Hide();		
		ic->Hide();
		ic->SetIndependentPos();		
	}

	if ( arrInput->size() == 0)
	{
		fldCol = new FieldColumnWithNone(fgrp2, SAFUiAttributeColumns, Table(), &sAttributeColumn, dmVALUE | dmBOOL);
		fldCol->Align(fld, AL_UNDER);
		fldCol->SetCallBack((NotifyProc)&FormAggregateValueMulti::HandleAttributeColumnChanges);
		SetAdditionalBlock(fgrp2, fldCol, true);		
	}
	else
		SetAdditionalBlock(fgrp2, fgrp2, true);		

	pbCalc = new PushButton(root, SAFUiCalculate, (NotifyProc) (&FormAggregateValueMulti::CalculateAggregateValues));
	fld->SetCallBack((NotifyProc)(&FormAggregateValueMulti::HandleChangesInMultiSelect));
	pbCalc->Align(rgBoolean, AL_UNDER);

	if ((m_fd & dmCLASS) == dmCLASS)
		SetAppHelpTopic("ilwismen\\smce_window_aggregate_values_sliced.htm");
	else
		SetAppHelpTopic("ilwismen\\smce_window_aggregate_values.htm");

	FormBaseDialog::create();
}

FormAggregateValueMulti::~FormAggregateValueMulti()
{
	if (ts)
	{
		if (ts->fRunning)
		{
			ts->fAbort = true;
			while (ts->fRunning)
			{
				Sleep(200);
				 MSG msg;
				 while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
					 ::DispatchMessage(&msg);
			}
		}

		delete ts;
		ts = 0;
	}
	
	if ( arrInput->size() != 0 )
		delete arrInput;
}

FormEntry* FormAggregateValueMulti::feDefaultFocus()
{
  return pbCalc;
}

void FormAggregateValueMulti::OnCancel()
{
	if ((ts == 0) || (!ts->fAbort))
		FormBaseDialog::OnCancel();
}

int FormAggregateValueMulti::HandleAttributeColumnChanges(Event *)
{
	if ( fldCol)
		fldCol->StoreData();
	return 1;
}

int FormAggregateValueMulti::Init(Event *)
{
	pbCalc->Disable();
	pbCalc->SetFocus();

	return 1;
}
// handles reaction to a selection a the left listbox. In this case it shows, if needed, a attrib column combobox
int FormAggregateValueMulti::Selection(Event *)
{
	// if more than one map is selected no valid map will be returend (and no combobox shown)
	if ( fldCol == 0)		
		return 1;
	
	Map mp = fld->mpGetSelectedMap();
	sAttributeColumn = "";
	if ( mp.fValid())
	{
		if ( mp->fTblAtt())
		{
			Table tbl = mp->tblAtt();
			fldCol->FillWithColumns(tbl->fnObj, InstAggregateValue::dmtValidDomainTypes(sOperation));
			fldCol->Show();
			fldCol->SelectNone();
			return 1;
		}
		else
			fldCol->Hide();
	}
	else
		fldCol->Hide();
	
	fldCol->FillWithColumns();
	return 1;
}

FormEntry *FormAggregateValueMulti::CheckData()
{
	if (fEnableCheckData)
		return FormBaseDialog::CheckData();
	else
		return 0;
}
int FormAggregateValueMulti::exec()
{
	// copy results to output vector
	if ( vrOutputValues)
	{
		fld->GetResults(vrOutputValues);
		int iN = vrOutputValues->size();
		for (int i=0; i < iN; ++i)
			(*vrOutputValues)[i].sOperation = sOperation;
	}		
	return 1;
}

// starts the sequence to perform the calculation .Data is collected and redirected to the correct control
int FormAggregateValueMulti::CalculateAggregateValues(Event *)
{
	fEnableCheckData = true;
	FormEntry *pfe = CheckData();
	fEnableCheckData = false;

	if (pfe) 
	{
		MessageBeep(MB_ICONEXCLAMATION);
		pfe->SetFocus();
		return 1;
	}

	AggregateValueOperationStruct avos;
	cbAdditional->StoreData();
	rgBoolean->StoreData();
	fmMapWeight->StoreData();
	fmMapBoolean->StoreData();
	fsExpression->StoreData();
	frPower->StoreData();
	avos.sOperation = sOperation;

	if ( fCIStrEqual(sOperation, "connectivityindex")) 	
		avos.rPower = rPower;
	
	if ( fAdditional )
	{
		if ( (iBooleanChoice == 0 || iBooleanChoice == 1) && sMapName == "")
			return 1;
		if ( iBooleanChoice  == 2 && sBooleanExpression == "" )
			return 1;

		if ( sMapName != "")
			avos.fnAdditional = AttributeFileName(sMapName);
	
		avos.sBooleanExpression = sBooleanExpression;	
	}	
	fld->CalculateAggregateValues(avos); 
	
	return 1;
}

//handles changes to the operation combobox. Some choices are only available for some operations
int FormAggregateValueMulti::HandleOperationChanges(Event *)
{
	favf->StoreData();
	cbAdditional->StoreData();	
	// hide parameter part and re-show them as conditions require
	frPower->Hide();
	rgBoolean->Hide();
	cbAdditional->Hide();

	GeoRef grf = fld->grfGetSelectedGeoref();	
	ObjectLister *ol = 0;	
	// additional parameters maybe show again, filter has to be reset
	long dmTypes = InstAggregateValue::dmtValidDomainTypes(sOperation);
	
	if ( grf.fValid())
		 ol = new MapListerDomainTypeAndGeoRef(grf->fnObj, dmTypes, true);		
	else
		ol = new MapListerDomainType(dmTypes, true);

	if ( (dmTypes & dmVALUE) | (dmTypes & dmBOOL) | (dmTypes & dmIMAGE))
	{
		cbAdditional->Show();
		if ( fAdditional)
		{
			rgBoolean->Show();		
		}			
	}		
	
	// connectivity index has a power parameter 	
	if ( fCIStrEqual(sOperation, "connectivityindex") )
	{
		frPower->Show();
	}
	fld->SetObjectLister(ol);	
	fld->CheckContent(sOperation);

	return 1;
}

// handles the actions that need to be taken if the move to button is used
int FormAggregateValueMulti::HandleChangesInMultiSelect(Event *)
{
	if ( fldCol && fldCol->fShow())
		fldCol->StoreData();
	// attribute column must be added to the file that is moving to the left listbox
	if ( sAttributeColumn != "")
	{
		fld->AddAttribColumnToSelectedFile(sAttributeColumn);
	}	
	//find the georef of the just selected map. If multiple maps are selected no valid georef is returned if they 
	// have differetn georefs
	GeoRef grf = fld->grfGetSelectedGeoref();

    // set the object listers of the relevant controls to the info found above		
	if ( grf.fValid() )
	{
		// find the domaintypes of the just selected maps. 		
		long dmt = InstAggregateValue::dmtValidDomainTypes(sOperation);
		ObjectLister *ol1 = new MapListerDomainTypeAndGeoRef(grf->fnObj, dmt, true);
		ObjectLister *ol2 = new MapListerDomainTypeAndGeoRef(grf->fnObj, dmVALUE, false);		
		ObjectLister *ol3 = new MapListerDomainTypeAndGeoRef(grf->fnObj, dmBOOL, false);		
		fld->SetObjectLister(ol1);
		if (fldCol)
			fldCol->Hide();
		fmMapWeight->SetObjLister(ol2);
		fmMapBoolean->SetObjLister(ol3);

		if ( arrInput->size() == 0)
		{
			ic->Show();			
			fsGeoref->SetVal(grf->fnObj.sRelative(false));
			fsGeoref->Show();
		}			
	}	
	else // if no valid grf is used reset al filters
	{
		ObjectExtensionLister *ol1 = new MapListerDomainType(dmVALUE | dmBOOL | dmIMAGE, true);		
		ObjectExtensionLister *ol2 = new MapListerDomainType(dmVALUE , false);		
		ObjectExtensionLister *ol3 = new MapListerDomainType(dmBOOL, false);		
		fld->SetObjectLister(ol1);
		fmMapWeight->SetObjLister(ol2);
		fmMapBoolean->SetObjLister(ol3);
		if ( fldCol)
    		fldCol->Hide();
		if ( arrInput->size() == 0)
			fsGeoref->SetVal("");
	}

	if ( fld->GetRightCount() > 0 )
	{
		pbCalc->Enable();
//		pbCalc->SetDefault(true);
		pbCalc->SetFocus();
	}
	else
	{
		pbCalc->Disable();
	}		
	
	return 1;
}
//--------------------------------
AggregateValueInputSelector::AggregateValueInputSelector(FormEntry* par, FormAggregateValueMulti *a, ObjectLister *lister) :
	InputObjectSelector(par, lister),
	ava(a)
{

}

void AggregateValueInputSelector::GetSelectedItems(vector<FMOSItem *>& vec )
{
  int iCnt = lb->GetSelCount(); 
  int* pi = new int[iCnt];
  iCnt = lb->GetSelItems(iCnt, pi);
  vec.resize(iCnt);
  for (int i = 0; i < iCnt; ++i)
  {
	SelectedFMOItem *item = new SelectedFMOItem(fiGetItem(pi[i])->Clone());	  
    item->SetAttributeColumn(ava->sGetAttributeColumn());
	vec[i] = item;
  }		
  delete [] pi;
	
}

String AggregateValueInputSelector::sGetText()
{
  if (!fShow() || !lb) return "";
  int iCnt = lb->GetCount();
  String sText;
  CString sLbText;
  for (int i = 0; i < iCnt; ++i) 
  {
    lb->GetText(i, sLbText);
    sText &= String(sLbText);
    sText &= "\r\n";
  }
  return sText;
}

//---------------------------------

AggregateValueResultSelector::AggregateValueResultSelector(FormEntry* par, vector<AttributeFileName>& selected) :
	SelectedObjectsSelector(par, selected)
{
	SetWidth(150);
	
	// Set the font .. same used as in tables, just a bit smaller so that underscores are visible
	LOGFONT logFont;
	IlwWinApp()->GetFont(IlwisWinApp::sfTABLE)->GetLogFont(&logFont);

	logFont.lfHeight = logFont.lfHeight * 0.95;
	fnt = new CFont();
	fnt->CreateFontIndirect(&logFont);

	SetFont(fnt);
}

AggregateValueResultSelector::~AggregateValueResultSelector()
{
	delete fnt;
}

// reset the content of the left listbox if maps are present that do not fit the
// operation selected (e.g. sort maps with an maximum operation)
void AggregateValueResultSelector::CheckContent(const String& sOperation)
{
	int iN = iGetCount();
	if (iN <= 0)
		return;
	SelectedFMOItem *item = (SelectedFMOItem *)fiGetItem(0);
	if ( item == 0 )
		return;
	
	AttributeFileName fn = item->fnItem();
	if ( !fn.fExist())
		return;
	Map mp(fn);
	String sAttrib = item->sGetAttributeColumn();
	Domain dm;
	if ( sAttrib == "")
		dm = mp->dm();
	else
	{
		if ( mp->fTblAtt())
			if (mp->tblAtt()->col(sAttrib).fValid() )		
				dm = mp->tblAtt()->col(sAttrib)->dm();
	}
	
	if ( !dm.fValid())
		return;

	if (fCIStrEqual(sOperation,"connectivityindex") | fCIStrEqual(sOperation,"shapeindex"))
	{
		if( !dm->pdsrt())
			ResetContent();
	}
	else
	{
		if (!dm->pdv())
			ResetContent();
	}		

}

// adds an attribute table to the correct index in the left listbox
void AggregateValueResultSelector::InsertAttributeColum(int iIndex, const String& sCol)
{
	SelectedFMOItem *item = (SelectedFMOItem*)fiGetItem(iIndex)->Clone();
	item->SetAttributeColumn(sCol);
	ChangeItem(iIndex, item);
}

void AggregateValueResultSelector::AddItem(FMOSItem* item)
{
	SelectedFMOItem *itemnew = new SelectedFMOItem(item);
	SelectedObjectsSelector::AddItem(itemnew);
}

String AggregateValueResultSelector::sGetText()
{
  if (!fShow() || !lb) return "";
  int iCnt = lb->GetCount();
  String sText;
  CString sLbText;
  for (int i = 0; i < iCnt; ++i) 
  {
    lb->GetText(i, sLbText);
    sText &= String(sLbText);
    sText &= "\r\n";
  }
  return sText;
}

//--------------------------------
FieldMultiAggregateValueSelect::FieldMultiAggregateValueSelect(FormEntry* par, const String& sQuestion, ObjectLister *lister, vector<AttributeFileName>& arrSelected, ThreadStruct** _pts) :
	FieldMultiObjectSelectBase(par, sQuestion, lister, arrSelected),
	pts(_pts)
{
  if ( arrSelected.size() == 0 )		
	mosInput = new AggregateValueInputSelector(fgLeft, (FormAggregateValueMulti*)frm(),lister);
 
  ossSelected = new AggregateValueResultSelector( fgRight ,arrSelected);
}

void FieldMultiAggregateValueSelect::SetSelectionFunction(CallBackHandler *cbh, NotifyProc func)
{
  if ( mosInput != 0)	
	mosInput->SetSelectionFunction(cbh,func);	
}	

// copies data from the listbox items to the output vector. Function is sued in the exec function of the form
void FieldMultiAggregateValueSelect::GetResults(vector< AggregateValueStruct >* vrOutputValues)
{
	int iN = ossSelected->iGetCount();
    vrOutputValues->resize(iN);	
	for (int i = 0; i < iN; ++i)
	{
		 SelectedFMOItem *item = (SelectedFMOItem*)(ossSelected->fiGetItem(i));
		 ((*vrOutputValues)[i]).fnMapUsed = item->fnItem();
 		 ((*vrOutputValues)[i]).sAttributeColumn = item->sGetAttributeColumn();
		 ((*vrOutputValues)[i]).rAggregateValue = item->rGetResult();
	}		
}

// returns the selected map its georef. If multiple maps are selected it will only return a valid georef
// if all georefs are the same
GeoRef FieldMultiAggregateValueSelect::grfGetSelectedGeoref()
{
	GeoRef grf;
	int iN = ossSelected->iGetCount();
	for (int i = 0; i < iN; ++i)
	{
		AttributeFileName fn = ossSelected->fiGetItem(i)->fnItem();
		Map mp(fn);
		GeoRef grfNew = mp->gr();
		if ( !grf.fValid())
			grf = grfNew;
		else if ( grf != grfNew )			
			return GeoRef();
	}
	return grf;
}

void FieldMultiAggregateValueSelect::AddAttribColumnToSelectedFile(const String& sAttributeColumn)
{
	FMOSItems vfn;
	if ( mosInput == 0)
		return;
	mosInput->GetSelectedItems(vfn);
	if ( vfn.size() != 1)
		return;

	AttributeFileName fnSelected = vfn[0]->fnItem();
	vfn.CleanUp();
	
	int iN = ossSelected->iGetCount();
	for ( int i=0; i<iN ;++i)
	{
		if ( fnSelected == ossSelected->fiGetItem(i)->fnItem())
		{
			AggregateValueResultSelector *avs = (AggregateValueResultSelector*)ossSelected;
			avs->InsertAttributeColum(i, sAttributeColumn);
			return;
		}			
	}		
	
}

// returns a or'ed combination of the dmTypes used in the selected maps
dmType FieldMultiAggregateValueSelect::dmtGetSelectedDomainType(const String& sAttribCol)
{
	long dmt = 0;
	int iN = ossSelected->iGetCount();
	for (int i = 0; i < iN; ++i)
	{
		AttributeFileName fn = ossSelected->fiGetItem(i)->fnItem();
		Map mp(fn);
		if ( sAttribCol != "") // if an attribute column
		{
			if ( mp->fTblAtt())
			{
				if (mp->tblAtt()->col(sAttribCol).fValid() )
					dmt = dmt | dmtype(mp->tblAtt()->col(sAttribCol)->dm()->dmt());
			}				
		}
		else
			dmt= dmt | dmtype(mp->dm()->dmt());
	}
	return (dmType)dmt;
}

// returns the first selected map. if multiple maps are selected it will return the first one
Map FieldMultiAggregateValueSelect::mpGetSelectedMap()
{
	Map mp;
	FMOSItems vfnMaps;
	if ( mosInput == 0)
		return Map();
	
	mosInput->GetSelectedItems(vfnMaps);
	if (vfnMaps.size() == 1 && vfnMaps[0])
		mp = Map(vfnMaps[0]->fnItem());
	
	vfnMaps.CleanUp();
	
	return mp;
}

int FieldMultiAggregateValueSelect::GetRightCount()
{
	return ossSelected->iGetCount();
}

void FieldMultiAggregateValueSelect::CheckContent(const String& sOperation)
{
	AggregateValueResultSelector *selector = reinterpret_cast<AggregateValueResultSelector*>(ossSelected);
	selector->CheckContent(sOperation);
}

// this thread does the actual calculation. It does them in order and not all at once
UINT CalcExpression(LPVOID pParms)
{
	ThreadStruct *ts = (ThreadStruct *)pParms;
	try
	{
		IlwWinApp()->Context()->InitThreadLocalVars();
		CalculateAggregateValue	cavCalcObject(ts->avos);	
		int iN = ts->selector->iGetCount();
		String sResult;
		ts->form->pbCalc->disable();

		int iMaxLength = 0;
		
		for(int i=0; i < iN; ++i)
		{
			if (ts->fAbort)
				break;
			SelectedFMOItem *avs = (SelectedFMOItem *)ts->selector->fiGetItem(i);
			AttributeFileName fnInputMap = avs->fnItem();
			String sCol = avs->sGetAttributeColumn();
			String sResult = cavCalcObject.sCalculateAggregateValue(fnInputMap, sCol);
			SelectedFMOItem *item = (SelectedFMOItem*)ts->selector->fiGetItem(i)->Clone();
			item->SetAggregateResult(sResult);
			ts->selector->ChangeItem(i, item);		
			
		}
		ts->form->pbCalc->enable();	
		ts->form->EnableOK();
		IlwWinApp()->Context()->RemoveThreadLocalVars();
		ts->fRunning = false;
	}
	catch(ErrorObject& err)
	{
		ts->form->pbCalc->enable();	
		ts->form->EnableOK();
		IlwWinApp()->Context()->RemoveThreadLocalVars();
		ts->fRunning = false;
	}
	catch (...)
	{
		try // our last attempt to enable the buttons
		{
			ts->form->pbCalc->enable();	
			ts->form->EnableOK();
			IlwWinApp()->Context()->RemoveThreadLocalVars();
			ts->fRunning = false;
		}
		catch (...)
		{
		}
	}
	return 1;
}

void FieldMultiAggregateValueSelect::CalculateAggregateValues(AggregateValueOperationStruct& avos)
{
	frm()->DisableOK();
	if ((*pts) == 0)
		(*pts) = new ThreadStruct;
	(*pts)->avos = avos;
	(*pts)->selector = (AggregateValueResultSelector *)(ossSelected);
	(*pts)->form = (FormAggregateValueMulti *)frm();
	(*pts)->fRunning = true;
	(*pts)->fAbort = false;
	AfxBeginThread(CalcExpression, (LPVOID)(*pts));	
}
