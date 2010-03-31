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
#include "Client\Base\ilwis30.h"
#include "Engine\Map\Segment\Seg.h"
#include "Engine\Map\Polygon\POL.H"
#include "Client\FormElements\fldtbl.h"
#include "Client\FormElements\fldlist.h"
#include "Client\FormElements\fldgrf.h"
#include "Client\FormElements\FieldDependencyTree.h"
#include "Client\FormElements\FieldUsedByTree.h"
#include "Client\FormElements\FilenameLister.h"
#include "Client\FormElements\InfoText.h"
#include "Client\FormElements\FieldExtent.h"
#include "Headers\Hs\Mainwind.hs"
#include "Headers\Hs\Coordsys.hs"
#include "Headers\Hs\proj.hs"
#include "Headers\Hs\Sample.hs"
#include "Headers\Hs\Userint.hs"
#include "Headers\constant.h"
#include "Headers\messages.h"
#include "Headers\Htp\Ilwis.htp"
#include "Engine\Domain\dmsort.h"
#include "Engine\Domain\Dmvalue.h"
#include "Engine\Domain\dmpict.h"
#include "Engine\Table\tbl2dim.h"
#include "Engine\Base\DataObjects\ObjectStructure.h"
#include "Engine\Table\TBLHIST.H"
#include "Engine\Map\Mapview.h"
#include "Engine\Stereoscopy\StereoPair.h"
#include "Engine\SpatialReference\GRNONE.H"
#include "Engine\SpatialReference\GR3D.H"
#include "Engine\SpatialReference\Grortho.h"
#include "Engine\SpatialReference\grdirlin.h"
#include "Engine\SpatialReference\GrParallProj.h"
#include "Engine\SpatialReference\CoordSystemOrthoPhoto.h"
#include "Engine\SpatialReference\CoordSystemDirectLinear.h"
#include "Engine\SpatialReference\Csproj.h"
#include "Engine\SampleSet\SAMPLSET.H"
#include "Engine\Base\Algorithm\Random.h"
#include "Engine\Base\StlExtensions.h"
#include <afxdisp.h>        // MFC Automation classes
#include "Client\Help\helpctrl.h"
#include "Client\FormElements\FormBasePropertyPage.h"
#include "Client\FormElements\FieldObjShow.h"
#include "Client\Forms\PropertySheets.h"
#include "Client\Forms\PropForm.h"

class ValueRangeChangeForm: public FormWithDest
{
public:
	ValueRangeChangeForm(CWnd* wPar, const String& sTitle, const ValueRange& vr)
		: FormWithDest(wPar, sTitle), m_vr(vr)
	{
		m_fvr = new FieldValueRange(root, SMSUiRange, &m_vr, 0);

		m_rg = new RadioGroup(root, String(), &m_iOption);
		m_rg->SetIndependentPos();
		new RadioButton(m_rg, SMSUiConvertValues);   // So convert raw values
		new RadioButton(m_rg, SMSUiReinterpretValues);  // So leave raw values as-is

		create();
	}
	ValueRange vrNew()
	{
		return m_vr;
	}
	bool fConvertValues()
	{
		return m_iOption == 0;
	}
	int exec()
	{
		m_fvr->StoreData();
		m_rg->StoreData();

		return 0;
	}
private:
	FieldValueRange *m_fvr;
	RadioGroup      *m_rg;

	ValueRange      m_vr;
	int             m_iOption;
};

// Definition of all property sheets
//-------------------------------
// BasicPropertyFormPage member functions
BasicPropertyFormPage::BasicPropertyFormPage(const IlwisObject& object, const String& sPageTitle)
	:	FormBasePropertyPage(sPageTitle),
		m_obj(object)
{
	m_fReadOnly = object->fReadOnly();
}

void BasicPropertyFormPage::create()
{
	BuildPage();

	FormBasePropertyPage::create();
}

void BasicPropertyFormPage::BuildPage()
{
	int iImage = IlwWinApp()->iImage(m_obj->fnObj.sExt);
	HICON icon;
	if (iImage)
		icon = IlwWinApp()->ilLarge.ExtractIcon(iImage);

	String s = m_obj->sTypeName();
	s &= "  ";

	m_fib = new FlatIconButton(root, icon, s, (NotifyProc)&BasicPropertyFormPage::PressButton, m_obj->fnObj, false, FlatIconButton::fbsTITLE);
	m_fib->SetIndependentPos();

	m_fgPageRoot = new FieldGroup(root);
}

String BasicPropertyFormPage::sObjectDesc(const IlwisObject& obj)
{
	IlwisObject object(obj);
	if (!object.fValid())
		object = m_obj;

	int iImage = IlwWinApp()->iImage(object->fnObj.sExt);

	String s = object->sTypeName();
	if (object->sDescription.length() > 0 && object->sDescription != s)
	{
		if (iImage == 0 && object->sDescription.length() < 40)
		{
			s &= ".  ";
			s &= object->sDescription;
		}
		else
		{
			if (object->sDescription.length() < 80)
				s = object->sDescription;
			else
			{
				s = object->sDescription.sLeft(80);
				s &= "...";
			}
		}
	}

	return s;
}

void BasicPropertyFormPage::DisplayDefinition()
{
	FormEntry* fe;
	if (m_obj->fDependent())
	{
		String sExpr = m_obj->sExpression();
		StaticText *st = new InfoText(m_fgPageRoot, SMSRemExpression);
		st->psn->SetBound(0,0,0,0);
		fe = new FieldString(m_fgPageRoot, &sExpr, ES_AUTOHSCROLL|WS_TABSTOP|WS_GROUP|ES_READONLY);
		fe->SetWidth(200);
		fe->SetIndependentPos();
	}
}

int BasicPropertyFormPage::PressButton(Event*)
{
	String sCmd("open %S", m_obj->fnObj.sFullNameQuoted());
	char* str = sCmd.sVal();
	IlwWinApp()->GetMainWnd()->SendMessage(ILWM_EXECUTE, 0, (LPARAM)str);

	return 0;
}

// Inline function to build strings for value range and precision:
// Range:      "<lower> to <upper>"
// Precision:  "<step>"
void BuildVRStrings(const ValueRange& vr, String& sRange, String& sPrecision)
{
	if (vr->vri())
	{
		RangeInt ri = vr->riMinMax();
		String sLo = vr->sValue(ri.iLo());
		String sHi = vr->sValue(ri.iHi());
		sRange = String(SMSRemRange_SS.scVal(), sLo, sHi);
		sPrecision = "1";
	}
	else if (vr->vrr())
	{
		RangeReal rr = vr->rrMinMax();
		double rStep = vr->rStep();
		String sLo = vr->sValue(rr.rLo());
		String sHi = vr->sValue(rr.rHi());
		sPrecision = vr->sValue(rStep);
		sRange = String(SMSRemRange_SS.scVal(), sLo, sHi);
	}
	sRange = sRange.sTrimSpaces();
	sPrecision = sPrecision.sTrimSpaces();
}

// Build display Value range fields (R/O case), they are displayed as
// ValueRange:   <lo> to <high>
// Precision:    <step>
void SetRangePrecFields(const DomainValue* dv, const ValueRange& vr, FormEntry* root, StaticText *stRange, StaticText *stPrecision, StaticText *stUnit)
{
	String sRange, sPrecision;
	BuildVRStrings(vr, sRange, sPrecision);
	
	StaticText *st = new StaticText(root, SMSRemRange);
	stRange = new StaticText(root, sRange);
	stRange->Align(st, AL_AFTER);
	
	StaticText *stPrec = new StaticText(root, SMSRemPrecision);
	stPrec->Align(st, AL_UNDER);
	stPrec->psn->iBndUp = -5;
	stPrecision = new StaticText(root, sPrecision);
	stPrecision->Align(stPrec, AL_AFTER);

	FormEntry *feAlignTo = stPrec;
	
	if (dv->fUnit())
	{
		StaticText* stLeft = new StaticText(root, SMSRemUnitString);
		stLeft->Align(stPrec, AL_UNDER);
		stLeft->psn->iBndUp = -5;
		stUnit = new StaticText(root, dv->sUnit());
		stUnit->Align(stLeft, AL_AFTER);
		feAlignTo = stLeft;
	}

	FieldBlank *fb = new FieldBlank(root, 0);
	fb->Align(feAlignTo, AL_UNDER);
}

//-------------------------------
// GeneralPropPage member functions
GeneralPropPage::GeneralPropPage(const IlwisObject& object)
	: BasicPropertyFormPage(object, SMSPropIlwisObject)
{
}

void GeneralPropPage::BuildPage()
{
	BasicPropertyFormPage::BuildPage();

	String s;
	StaticText *st = 0;
	if (m_obj->fSystemObject())
		s = SMSRemSystemObject;
	else
	{
		String sLoc = m_obj->fnObj.sPath();
		if (sLoc[sLoc.length() - 1] == '\\')
			sLoc = sLoc.sLeft(sLoc.length() - 1);

		s = String(SMSRemLocation_S.scVal(), sLoc);
		st = new InfoText(m_fgPageRoot, s);
		s = m_obj->objtime.sDateTime();
	}
	st = new InfoText(m_fgPageRoot, s);
	st->psn->SetBound(0,0,0,0);

	// Get object size
	ObjectStructure osStruct;
	osStruct.GetAssociatedFiles(false);
	m_obj->GetObjectStructure(osStruct);
	list<String> lstFiles;
	osStruct.GetUsedFiles(lstFiles, false);
	CFileStatus status;
	long iSize = 0;
	for ( list<String>::iterator cur = lstFiles.begin(); cur != lstFiles.end(); ++cur)
	{
		FileName fn(*cur);
		
		if (CFile::GetStatus(fn.sFullPath().scVal(), status))
			iSize += status.m_size;
	}

	if (iSize > 0)
	{
		s = String(SMSRemObjectSize_i.scVal(), iSize);
		st = new InfoText(m_fgPageRoot, s);
		st->psn->SetBound(0,0,0,0);
	}

	if (!m_obj->fSystemObject())
	{
		CheckBox *cbRO = new CheckBox(m_fgPageRoot, SMSUiReadOnly, &m_fReadOnly);
		if (m_obj->fUseAs())
			cbRO->SetStyle(true); // disable the Checkbox
	}

	st = new InfoText(m_fgPageRoot, SMSUiDescription);
	st->psn->SetBound(0,0,0,0);
	if (m_fReadOnly) 
		m_fsDesc = new FieldString(m_fgPageRoot, &m_obj->sDescription, ES_AUTOHSCROLL|WS_TABSTOP|WS_GROUP|ES_READONLY);
	else 
		m_fsDesc = new FieldString(m_fgPageRoot, "", &m_obj->sDescription);

	m_fsDesc->SetWidth(200);
	m_fsDesc->SetIndependentPos();
}

int GeneralPropPage::exec()
{
	BasicPropertyFormPage::exec();

	bool fRO = m_obj->fReadOnly();
	if (!m_fReadOnly)
		m_obj->SetReadOnly(m_fReadOnly);
	try
	{
		bool fDescChanged = m_obj->sDescription != ObjectInfo::sDescr(m_obj->fnObj);
		if (m_obj->fChanged || fDescChanged)
			m_obj->Store();
		
		if (m_obj->fChanged || fDescChanged || (fRO != m_fReadOnly))
		{
			FileName* pfn = new FileName(m_obj->fnObj);
			AfxGetApp()->GetMainWnd()->PostMessage(ILW_READCATALOG, 0, (long)pfn);
		}
	}
	catch (const ErrorNotCreatedOpen&)
	{	// ignore read/only errors
	}
	catch (...)
	{
		throw; // rethrow other errors
	}

	if (m_fReadOnly)
		m_obj->SetReadOnly(m_fReadOnly);

	return 0;
}

//-------------------------------
// BaseMapPropPage member functions
BaseMapPropPage::BaseMapPropPage(const IlwisObject& obj, const String& sPageTitle)
	: BasicPropertyFormPage(obj, sPageTitle)
	, m_stCSDsc(0)
{
	m_bm = BaseMap(m_obj->fnObj);
	m_fgValues = 0;
	m_fFirst = true;
}

void BaseMapPropPage::BuildPage()
{
	BasicPropertyFormPage::BuildPage();

	SetCoordSystemField();
	SetDomainValueRangeField();
	SetAttribTableField();
	SetMapDetails();

	String s('X', 50);
	m_stRemark = new InfoText(m_fgPageRoot, s);
	m_stRemark->SetVal(String());   // clear remark string
}

// virtual functions
void BaseMapPropPage::SetCoordSystemField()
{
	CoordSystem cs = m_bm->cs();

	if (m_bm->fDependent() || m_fReadOnly)
		new FieldObjShow(m_fgPageRoot, cs);
	else
	{
		m_sNewCS = cs->sName();
		m_fcs = new FieldCoordSystemC(m_fgPageRoot, SMSUiCoordSys, &m_sNewCS);
		m_fcs->SetBounds(m_bm->cb());
		m_fcs->SetCallBack((NotifyProc)&BaseMapPropPage::CoordSysChange);

		String sDsc = sObjectDesc(cs);
		m_stCSDsc = new InfoText(m_fgPageRoot, sDsc);
		m_stCSDsc->psn->iBndDown = 0;
	}

	StaticText *stAbove = m_stCSDsc;
	// When valid display the boundaries of the map
	if (cs.fValid() && !(m_bm->cb().fUndef() && cs->fUnknown()))
	{
		String sMin = cs->sValue(m_bm->cb().cMin,0);
		String sMax = cs->sValue(m_bm->cb().cMax,0);
		String sTitleMin = SCSUiMinXY;
		String sTitleMax = SCSUiMaxXY;
		if (cs->pcsLatLon())
		{
			sTitleMin = SMSUiMinLatLon;
			sTitleMax = SMSUiMaxLatLon;
		}

		FieldGroup *fgCorners = new FieldGroup(m_fgPageRoot, true);
		FieldString *fs;
		StaticText *stTitle = new InfoText(fgCorners, SMSMsgMapBoundary);
		stTitle->psn->iBndDown = 0;
		StaticText *stMin = new InfoText(fgCorners, sTitleMin);
		int iMaxW = stMin->psn->iMinWidth + 15;
		stMin->psn->SetBound(0,0,0,0);  // set space around previous InfoText to a minimum.
		fs = new FieldString(fgCorners, &sMin, ES_AUTOHSCROLL|WS_TABSTOP|WS_GROUP|ES_READONLY);
		fs->psn->SetBound(0,0,0,0);
		fs->SetIndependentPos();
		fs->SetWidth(180);
		fs->Align(stMin, AL_AFTER);

		stAbove = stMin;
		StaticText *stMax = new InfoText(fgCorners, sTitleMax);
		iMaxW = max(iMaxW, stMax->psn->iMinWidth + 15);
		stMax->psn->SetBound(0,0,0,0);  // set space around previous InfoText to a minimum.
		stMax->Align(stAbove, AL_UNDER);
		fs = new FieldString(fgCorners, &sMax, ES_AUTOHSCROLL|WS_TABSTOP|WS_GROUP|ES_READONLY);
		fs->psn->SetBound(0,0,0,0);
		fs->SetIndependentPos();
		fs->SetWidth(180);
		fs->Align(stMax, AL_AFTER);

		stAbove = stMax;

		stMin->SetFieldWidth(iMaxW);
		stMax->SetFieldWidth(iMaxW);
	}
	// Use the zero height fieldblank, to align additional fields properly
	FieldBlank *fb = new FieldBlank(m_fgPageRoot, 0); // Used to force proper alignment
	fb->Align(stAbove, AL_UNDER);
}

// Helper routine:
// Select the valuerange to display in basemap property form:
// 1. Try value range of the map itself
//    If this is invalid or set to ridiculous values (-1e307, 1e307)
// 2. Try value range of domain of map
//    If this is invalid or set to ridiculous values (-1e307, 1e307)
// 3. Try MinMax of map itself
//    If this is invalid or set to ridiculous values (-1e307, 1e307)
// 4. Display -9999999.9, 9999999.9, 0.1
void CheckSetValidValueRange(const BaseMap& bm, ValueRange& vr)
{
	vr = bm->vr();
	// Only try it for DomainValue
	if (vr.fValid())
	{
		RangeReal rrVR = vr->rrMinMax();
		double rStep = vr->rStep();
		bool fValidVR = log(rrVR.rLo()) < 100 && log(rrVR.rHi()) < 100;
		if (!fValidVR)
		{
			DomainValue *pdv = bm->dm()->pdv();
			RangeReal rr;
			if (pdv)
				rr = pdv->rrMinMax();
			fValidVR = rr.fValid() && log(rr.rLo()) < 100 && log(rr.rHi()) < 100;
			if (fValidVR)
				vr = ValueRange(rr, rStep);
			else
			{
				rr = bm->rrMinMax();
				fValidVR = log(rr.rLo()) < 100 && log(rr.rHi()) < 100;
				if (fValidVR)
					vr = ValueRange(rr, rStep);
				else
					vr = ValueRange(-9999999.9, 9999999.9, 0.1); // make sure FieldValueRange will display minmax values
			}
		}
	}
}

void BaseMapPropPage::SetDomainValueRangeField()
{
	Domain dm = m_bm->dm();
	CheckSetValidValueRange(m_bm, m_vr);

	String s('X', 50);
	if (m_bm->fDependent() || m_fReadOnly)
	{
		if (dm.fValid())
		{
			StaticText *st = 0;
			if (dm->fValidDomain())
			{
				if (dm->fnObj == m_bm->fnObj)    // internal domain
					st = new InfoText(m_fgPageRoot, dm->sTypeName());
				else
					new FieldObjShow(m_fgPageRoot, dm);
			}
			else
				st = new InfoText(m_fgPageRoot, SMSRemMapWithInvalidDomain);

			if (dm->pdv() && !dm->pdi() && !dm->pdbool() && !dm->pdbit())
				SetRangePrecFields(dm->pdv(), m_vr, m_fgPageRoot, m_stValues, m_stPrecision, m_stUnit);
		}
	}
	else
	{
		m_sNewDM = dm->sName();
		m_fdm = new FieldDomainC(m_fgPageRoot, SMSUiDomain, &m_sNewDM, 0, true);
		m_fdm->SetValueRange(m_vr);
		m_fdm->SetCallBack((NotifyProc)&BaseMapPropPage::DomainChange);

		m_stDM = new InfoText(m_fgPageRoot, s);
		m_stDM->psn->SetBound(0,0,0,0);

		if (dm->pdv() && !dm->pdi() && !dm->pdbool() && !dm->pdbit())
		{
			m_fgValues = new FieldGroup(m_fgPageRoot);
			FieldBlank *fb = new FieldBlank(m_fgValues, 0);

			m_fvr = new FieldValueRange(m_fgValues, SMSUiRange, &m_vr, m_fdm);
			m_fvr->Align(fb, AL_UNDER);
			m_feAlignUnder = m_fvr;

			DomainValue *dv = dm->pdv();
			if (dv->fUnit())
			{
				StaticText* stLeft = new StaticText(m_fgPageRoot, SMSRemUnitString);
				StaticText* stUnit = new StaticText(m_fgPageRoot, dv->sUnit());
				stUnit->Align(stLeft, AL_AFTER);
				FieldBlank *fb = new FieldBlank(m_fgPageRoot, 0);
				fb->Align(stLeft, AL_UNDER);
				m_feAlignUnder = fb;
			}
		}
	}
}

void BaseMapPropPage::SetAttribTableField()
{
	if (m_bm->dm()->pdsrt())
	{
		fAttrTable = m_bm->fTblAttSelf();
		if (fAttrTable)
		{
			sAttrTable = m_bm->sName(false);
			sAttrTable &= ".tbt";
			Table tbl = m_bm->tblAtt();
			if (tbl.fValid())
				sAttrTable = tbl->sName(true);
			else
				fAttrTable = false;
		}  
		else
			sAttrTable = String();

		if (m_fReadOnly)
		{
			if (fAttrTable)
			{
				String s = SMSUiAttrTable;
				s &= " ";
				s &= sAttrTable;
				new InfoText(m_fgPageRoot, s);
			}
		} 
		else
		{
			CheckBox* cb = new CheckBox(m_fgPageRoot, SMSUiAttrTable, &fAttrTable);
			ftAttTable = new FieldTableC(cb, "", &sAttrTable, m_bm->dm());
			ftAttTable->SetCallBack((NotifyProc)&BaseMapPropPage::CallBackAttTableChange); 

			String s('X', 50);
			stAttTable = new InfoText(cb, s);
			stAttTable->SetVal(String());
			stAttTable->Align(cb, AL_UNDER);
		}  
	}  
}

// SetMapDetails can be used to add extra text at the end of the form
// This information is specific for each map type (such as #points in pointmap etc.)
void BaseMapPropPage::SetMapDetails()
{
	DisplayDefinition();
}

// Callback functions
int BaseMapPropPage::CoordSysChange(Event*)
{
	DataChanged(0);
	m_fcs->StoreData();

	String s;
	String sDsc;
	if (m_sNewCS.length() > 0)
	{
		FileName fnCS(m_sNewCS);
		CoordSystem cs;
		try
		{
			cs = CoordSystem(fnCS);
		}
		catch (ErrorObject& err)
		{
			err.Show(SMSErrCsyChange);
		}

		String sRem;
		if (cs.fValid())
			sDsc = sObjectDesc(cs);
		else
			sRem = SMSRemInvalidCsy;
		
		m_stRemark->SetVal(sRem);
	}

	if (m_stCSDsc)
		m_stCSDsc->SetVal(sDsc);

	return 0;
}

int BaseMapPropPage::DomainChange(Event*)
{
	DataChanged(0);
	m_fdm->StoreData();

	String s;
	String sRem;
	if (m_sNewDM.length() > 0)
	{
		FileName fnDM(m_sNewDM);
		Domain dm;
		try
		{
			dm = Domain(fnDM);
		}
		catch (ErrorObject& err)
		{
			err.Show(SMSErrDomChange);
		}

		if (dm.fValid())
		{
			s = sObjectDesc(dm);

			bool fDomEqual = dm == m_bm->dm();
			if (!(m_fFirst && fDomEqual))
			{
				DomainValue *pdv = dm->pdv();
				if (!pdv)
				{
					if (m_fgValues) m_fgValues->Hide();
				}
				else if (!dm->pdbool() && !dm->pdi() && !dm->pdbit())
				{
					m_vr = ValueRange(pdv->rrMinMax(), pdv->rStep());
					if (m_fgValues)
					{
						m_fvr->SetVal(m_vr);
						m_fgValues->Show();
					}
				}
				else
				{
					m_vr = ValueRange();
					if (m_fgValues) m_fgValues->Hide();
				}
			}
			m_fFirst = fDomEqual;
		}
		else
			sRem = SMSRemInvalidDomain;
	}
	
	m_stRemark->SetVal(sRem);
	m_stDM->SetVal(s);

	return 0;
}

int BaseMapPropPage::CallBackAttTableChange(Event*)
{
	if ( !fAttrTable ) return 1;
	DataChanged(0);
	
	ftAttTable->StoreData();
	if (sAttrTable.length() == 0)
	{
		stAttTable->SetVal("");
		return 0;
	}

	FileName fn(sAttrTable, ".tbt");
	String sRemark;
	Table tbl;
	try
	{
		tbl = Table(fn);
	}
	catch (ErrorObject& err)
	{
		err.Show(SMSErrAttrTableChange);
	}

	if (tbl.fValid()) 
		sRemark = tbl->sDescription;
	else  
		sRemark = "";

	stAttTable->SetVal(sRemark);
	return 0;
}

int BaseMapPropPage::exec()
{
	BasicPropertyFormPage::exec();

	BaseMapPtr* pmp = m_bm.ptr(); //   dynamic_cast<BaseMapPtr*>(m_obj.pointer());
	if (!pmp)
		return 0;

	// Change CoordSystem
	if (m_sNewCS.length() > 0)
	{
		FileName fnCS = IlwisObjectPtr::fnCheckPath(FileName(m_sNewCS, ".csy"));
		if (fnCS != pmp->cs()->fnObj)  // only change when necessary
		{
			CoordSystem cs(fnCS);
			if (cs.fValid())
				pmp->SetCoordSystem(cs);
		}
	}

	// Change Domain
	if (m_sNewDM.length() > 0)
	{
		FileName fnDM = IlwisObjectPtr::fnCheckPath(FileName(m_sNewDM, ".dom", false));
		DomainValueRangeStruct dvs(fnDM);
		bool fDomEqual = fnDM == pmp->dm()->fnObj;
		bool fVrEqual = dvs.vr() == m_vr;
		if (!fVrEqual)
			dvs.SetValueRange(m_vr);

		if (!fDomEqual || !fVrEqual)
		{
			if (dvs.dm().fValid())
				pmp->SetDomainValueRangeStruct(dvs);
		}
	}

	// Change attribute table
	if (0 == ftAttTable)
		return 0;

	Table tbl; 
	if (fAttrTable)
	{
		try
		{
			FileName fn = IlwisObjectPtr::fnCheckPath(FileName(sAttrTable, ".tbt"));
			if (fn.fValid()) 
				tbl = Table(fn);
		}
		catch (ErrorObject& err)
		{
			err.Show(SMSErrAttrTable);
		} 
		fAttrTable = tbl.fValid();
	}

	if (!fAttrTable)
	{
		if (pmp->fTblAttSelf() != fAttrTable)
			pmp->SetNoAttributeTable();
	}
	else
	{
		if (pmp->dm() == tbl->dm())
		{
			if (!pmp->fTblAtt() || (pmp->fTblAtt() && pmp->tblAtt()->fnObj != tbl->fnObj))
				pmp->SetAttributeTable(tbl);
		}
		else
			MessageBox(SMSErrAttrSameAsMap.scVal(), SMSErrError.scVal(), MB_OK | MB_ICONSTOP);  
	}  
	return 0;
}

//-------------------------------
// MapPropPage member functions
MapPropPage::MapPropPage(const IlwisObject& obj)
	: BaseMapPropPage(obj, SMSPropRasterMap)
{
	SetMenHelpTopic(htpPropInfoRasMap);
	m_fInGRCallBack = false;
	m_fvr = 0;
	m_feAlignUnder = 0;
	m_fgValues = 0;
	m_stPyramids = 0;
	pbPyramids = 0;
}

// virtual functions
void MapPropPage::SetCoordSystemField()
{
	MapPtr* pmp = dynamic_cast<MapPtr*>(m_bm.ptr()); // can not fail, current object is a Map for sure
	Map mp;
	mp.SetPointer(pmp);

	// Get Storage details
	int iBytes = 1;
	switch (mp->st())
	{
		case stINT: iBytes = 2; break;
		case stLONG: iBytes = 4; break;
		case stFLOAT: iBytes = 4; break;
		case stREAL: iBytes = 8; break;
		case stFOREIGNFORMAT: iBytes = 0; break; // unknown what storetype is used
	}

	if (fCIStrEqual(mp->fnObj.sExt, ".mpl"))  // is it a map internal to a maplist?
	{
		FileName fnData;
		// First check foreign format (some older maplists may have this section)
		ObjectInfo::ReadElement("ForeignFormat", "Filename", mp->fnObj, fnData);
		if (!fnData.fValid()) // no foreign format, check native
			ObjectInfo::ReadElement("Mapstore", "Data", mp->fnObj, fnData);
		String sData(SMSMsgMapUseExternal_S.scVal(), fnData.sRelative(true, mp->fnObj.sPath()));
		new InfoText(m_fgPageRoot, sData);
	}
	else if (iBytes == 0) // Foreign format
	{
		FileName fnData;
		ObjectInfo::ReadElement("ForeignFormat", "Filename", mp->fnObj, fnData);
		String sData(SMSMsgMapUseExternal_S.scVal(), fnData.sRelative(true, mp->fnObj.sPath()));
		new InfoText(m_fgPageRoot, sData);
	}


	StaticText *st = 0;
	// Display Georeference information	
	GeoRef gr = mp->gr();
	if (gr->fnObj == mp->fnObj)
		st = new InfoText(m_fgPageRoot, SMSRemInternalGeoRef);
	else
	{
		if (m_bm->fDependent() || m_fReadOnly || "" == gr->fnObj.sFile)
			new FieldObjShow(m_fgPageRoot, gr);
		else
		{
			m_sNewGR = gr->sName();
			CoordBounds cbLoc = gr->fGeoRefNone() ? CoordBounds(Coord(), Coord()) : m_bm->cb();

			m_fgr = new FieldGeoRefC(m_fgPageRoot, SMSUiGrf, &m_sNewGR, gr, mp, true);
			m_fgr->SetBounds(m_bm->cs(), cbLoc);
			m_fgr->SetCallBack((NotifyProc)&MapPropPage::GeoRefChange);
			
			String sDsc = sObjectDesc(gr);
			m_stGRDsc = new InfoText(m_fgPageRoot, sDsc);
			m_stGRDsc->psn->iBndDown = 0;
		}
	}

	// Fields to display CoordSystem information
	String s('X', 50);
	String sCS = gr->cs()->sTypeName();
	String sCsDsc = sObjectDesc(gr->cs());
	m_stCS = new InfoText(m_fgPageRoot, s);
	m_stCS->psn->iBndDown = 0;
	m_stCS->SetVal(sCS);
	if (!fCIStrEqual(sCS, sCsDsc))
	{
		m_stCSDsc = new InfoText(m_fgPageRoot, s);
		m_stCSDsc->SetVal(sCsDsc);
	}

	// Display geo-related map details
	s = String(SMSRemLinesCols_ii.scVal(), mp->iLines(), mp->iCols());
	double rPix = gr->rPixSize();
	if (rUNDEF != rPix)
	{
		s &= "  ";
		if ( 0 != gr->cs()->pcsLatLon())
		{ 
			String sDMS = LatLon::sDegree(rPix,15,false);//fShowMinus == false
			s &= String(SMSRemPixSizeDMS_S.scVal(),sDMS);		
		}
		else
			s &= String(SMSRemPixSize_f.scVal(), rPix);
	}  
	st = new InfoText(m_fgPageRoot, s);

	// Get corners of the corners of the map
	Coord crdTL, crdBL, crdTR, crdBR;
	gr->RowCol2Coord(0, 0, crdTL);
	gr->RowCol2Coord(0, mp->iCols(), crdTR);
	gr->RowCol2Coord(mp->iLines(), 0, crdBL);
	gr->RowCol2Coord(mp->iLines(), mp->iCols(), crdBR);
	String sTL = gr->cs()->sValue(crdTL, 0);
	String sTR = gr->cs()->sValue(crdTR, 0);
	String sBL = gr->cs()->sValue(crdBL, 0);
	String sBR = gr->cs()->sValue(crdBR, 0);

	// For LatLon undef looks like: "? N/S, ? E/W"
	// For others undef looks like: "(?,?)"
	// If any undef coordinate pair is detected the boundaries are not displayed
	FieldExtent *fex = 0;
	if ((sTL.length() > 1 && (sTL[0] != '?' && sTL[1] != '?')) &&
		(sTR.length() > 1 && (sTR[0] != '?' && sTR[1] != '?')) &&
		(sBL.length() > 1 && (sBL[0] != '?' && sBL[1] != '?')) &&
		(sBR.length() > 1 && (sBR[0] != '?' && sBR[1] != '?')))
	{
		fex = new FieldExtent(m_fgPageRoot, mp);
	}
	FieldBlank *fb = new FieldBlank(m_fgPageRoot, 0); // Used to force proper alignment
	if (fex == 0)
		fb->Align(st, AL_UNDER);
	else
		fb->Align(fex, AL_UNDER);
	
	// Display storage details
	if (iBytes > 0)
	{
		s = String(SMSMsgMapUses_i.scVal(), iBytes);
		if (1 == iBytes)
			s &= SMSMsgByte;
		else  
			s &= SMSMsgBytes;
		s &= SMSMsg_PerPixel;  
		st->psn->SetBound(0,0,0,0);  // set space around previous InfoText to a minimum..
		st = new InfoText(m_fgPageRoot, s); // .. and add the new InfoText
	}
}

int MapPropPage::CallBackPyramids(Event *)
{
	MapPtr* pmp = dynamic_cast<MapPtr*>(m_bm.ptr()); // can not fail, current object is a pmp for sure	
	if ( pmp->fHasPyramidFile() )
	{
		pmp->DeletePyramidFile();
		IlwWinApp()->SetCommandLine(String("DeletePyramidLayers %S", pmp->fnObj.sRelative()));
	}		
	else
	{
		pmp->CreatePyramidLayer();
		pmp->Store();
		IlwWinApp()->SetCommandLine(String("CreatePyramidLayers %S", pmp->fnObj.sRelative()));		
	}

	// Adjust the text of static text and the button to reflect the current pyramid status
	// It is not combined above, because if for some reason the removal/creation fails
	// we still display the correct information, by checking afterwards
	String sPyrText, sPyrButton;
	if (pmp->fHasPyramidFile())
	{
		sPyrText = SMSPropPyrAvailable;
		sPyrButton = SMSPropRemovePyr;
	}
	else
	{
		sPyrText = SMSPropPyrNotAvailable;
		sPyrButton = SMSPropCreatePyr;
	}
	if (pbPyramids)
		pbPyramids->SetText(sPyrButton);
	m_stPyramids->SetVal(sPyrText);

	return 1;	
}

void MapPropPage::SetDomainValueRangeField()
{
	BaseMapPropPage::SetDomainValueRangeField();

	if (!m_bm->fDependent() && !m_fReadOnly && m_fgValues)
	{
		String sBut = String("%S ...", SMSUiChangeValueRange);
		PushButton *pb = new PushButton(m_fgValues, sBut, (NotifyProc)&MapPropPage::DoChangeValueRange);
		pb->SetIndependentPos();
		pb->Align(m_fvr, AL_AFTER);
		
		// Re-assign the callbacks; change item details
		m_fvr->SetCallBack((NotifyProc)&MapPropPage::ValueRangeCallBack);  // currently only used to set to disabled state
	}
}

void MapPropPage::SetMapDetails()
{
	MapPtr* pmp = dynamic_cast<MapPtr*>(m_bm.ptr()); // can not fail, current object is a pmp for sure

	StaticText *st = 0;
	FormEntry  *feUnder = m_fvr;
	if (m_feAlignUnder)
		feUnder = m_feAlignUnder;
	String s;
	if (pmp->dm()->pdv())
	{
		// Display Min:Max if known
		if (pmp->vr().fValid() && pmp->vr()->vrr())
		{
			RangeReal rr = pmp->rrMinMax();
			if (rr.rHi() > rUNDEF && rr.rHi() >= rr.rLo())
			{
				if (st) st->psn->SetBound(0,0,0,0);
				ValueRange vr = pmp->vr();
				String sMin = vr->sValue(rr.rLo());
				String sMax = vr->sValue(rr.rHi());
				s = String(SMSRemMinMax_ss.scVal(), sMin, sMax);
				st = new InfoText(m_fgPageRoot, s);
			}
		}
		else
		{
			RangeInt ri = pmp->riMinMax();
			if (ri.iHi() > iUNDEF && ri.iHi() >= ri.iLo())
			{
				if (st) st->psn->SetBound(0,0,0,0);
				s = String(SMSRemMinMax_ii.scVal(), ri.iLo(), ri.iHi());
				st = new InfoText(m_fgPageRoot, s);
			}  
		}
		if (feUnder && st)
			st->Align(feUnder, AL_UNDER);

		if (st)
			feUnder = st;

		// Allow interpolation method selection
		if (m_fReadOnly)
		{
			switch (pmp->interpolMethod()) 
			{
				case imBILINEAR:
					if (st) st->psn->SetBound(0,0,0,0);
					st = new InfoText(m_fgPageRoot, SMSRemBiLinearInterpol);
					break;
				case imBICUBIC:
					if (st) st->psn->SetBound(0,0,0,0);
					st = new InfoText(m_fgPageRoot, SMSRemBiCubicInterpol);
					break;
			}
			feUnder = st;
		}
		else
		{
			switch (pmp->interpolMethod())
			{
				case imBILINEAR:
					m_fInterpol = true;
					m_iInterPolMethod = 0;
					break;
				case imBICUBIC:
					m_fInterpol = true;
					m_iInterPolMethod = 1;
					break;
				default:
					m_fInterpol = false;
					m_iInterPolMethod = 0;
			}
			CheckBox* cb = new CheckBox(m_fgPageRoot, SMSUiInterpol, &m_fInterpol);
			cb->SetIndependentPos();
			if (feUnder)
				cb->Align(feUnder, AL_UNDER);
			RadioGroup* rg = new RadioGroup(cb, "", &m_iInterPolMethod, true);
			new RadioButton(rg, SMSUiBiLinear);
			new RadioButton(rg, SMSUiBiCubic);
			FieldBlank* fb = new FieldBlank(m_fgPageRoot, 0);
			fb->Align(cb, AL_UNDER);
			feUnder = fb;
			st = 0;
		}
	}

	BaseMapPropPage::SetMapDetails();  // currently adds only the expression

	if (!m_fReadOnly && pmp->fExpressionChangeable())
		(new PushButton(m_fgPageRoot, SMSUiEditDefinition, (NotifyProc)&MapPropPage::EditDefinition))->SetIndependentPos();

	if (pmp->dm()->dmt() != dmtBIT)
	{
		if (!pmp->fDependent() || (pmp->fDependent() && pmp->fCalculated()))
		{
			String sPyrText, sPyrButton;
			if (pmp->fHasPyramidFile())
			{
				sPyrText = SMSPropPyrAvailable;
				sPyrButton = SMSPropRemovePyr;
			}
			else
			{
				sPyrText = SMSPropPyrNotAvailable;
				sPyrButton = SMSPropCreatePyr;
			}
			// Force the width of the statictext to the length of the longest possible string
			// And after that set the correct text
			m_stPyramids = new StaticText(m_fgPageRoot, SMSPropPyrNotAvailable);
			m_stPyramids->SetVal(sPyrText);
			if ( !pmp->fReadOnly())
			{
				pbPyramids = new PushButton(m_fgPageRoot, sPyrButton, (NotifyProc)&MapPropPage::CallBackPyramids);
				pbPyramids->Align(m_stPyramids, AL_AFTER);
			}
		}
	}			
}

// Callback functions
int MapPropPage::ValueRangeCallBack(Event*)
{
	DataChanged(0);
	m_fvr->Disable();

	return 0;
}

int MapPropPage::GeoRefChange(Event*)
{
	if (m_fInGRCallBack)
		return 0;

	DataChanged(0);
	m_fgr->StoreData();
	GeoRef gr;

	if (m_sNewGR.length() == 0)
		return 0;

	MapPtr* pmp = dynamic_cast<MapPtr*>(m_bm.pointer()); 
	if (!pmp)
		return 0;

	FileName fnOldGR = pmp->gr()->fnObj;
	try
	{
		FileName fnGR(m_sNewGR);
		gr = GeoRef(fnGR);
	}
	catch(const ErrorObject& err)
	{
		err.Show(SMSErrGrfChange);
	}

	m_stGRDsc->SetVal(sObjectDesc(gr));
	CoordSystem cs = gr->cs();

	m_stCS->SetVal(cs->sTypeName());
	if (m_stCSDsc)
		m_stCSDsc->SetVal(sObjectDesc(cs));

	// If the GeoRef is the same as it was no further checks are necessary
	if (gr->fnObj == fnOldGR)  // done
		return 0;

	// Set new GeoRef if:
	// - it is GeoRefNone  OR
	// - size of map equal to georef size
	GeoRefNone *grn = dynamic_cast<GeoRefNone*>(gr.ptr());
	if (grn == 0 && pmp->rcSize() != gr->rcSize())
	{
		Beep(440, 150);
		String s= SMSErrGrfIndDiffSize;
		s &= "\n";
		s &= SMSErrGrfNotChanged;
		MessageBox(s.scVal(), SMSErrError.scVal(), MB_ICONEXCLAMATION|MB_OK);
		m_fInGRCallBack = true;

		m_fgr->SetVal(fnOldGR.sFullName());

		m_fInGRCallBack = false;
	}                 

	CoordBounds cbLoc = gr->fGeoRefNone() ? CoordBounds(Coord(), Coord()) : m_bm->cb();
	m_fgr->SetBounds(gr->fGeoRefNone() ? m_bm->cs() : gr->cs(), cbLoc);

	return 0;
}

bool fCheckNoAmpers(char c)
{
	return c != '&';
}

// Act on the "value range change" button click
int MapPropPage::DoChangeValueRange(Event*)
{
	String sTitleIn = SMSUiChangeValueRange;
	String sTitle(' ', sTitleIn.length());
	copy_if(sTitleIn.begin(), sTitleIn.end(), sTitle.begin(), fCheckNoAmpers);
	
	ValueRangeChangeForm frm(GetParent(), sTitle, m_vr);
	if (frm.fOkClicked())
	{
		if (frm.vrNew() != m_vr)
		{
			FileName fnDM(m_sNewDM);
			Domain dm;
			try
			{
				dm = Domain(fnDM);
			}
			catch (ErrorObject& err)
			{
				err.Show(SMSErrDomChange);
			}

			m_vr = frm.vrNew();
			if (dm.fValid())
			{
				DomainValueRangeStruct dvs(dm, m_vr);
				if (frm.fConvertValues())
					m_bm->fConvertTo(dvs, Column());
				else
					m_bm->SetDomainValueRangeStruct(dvs);
				UpdateValueRangeFields();
			}
		}
	}

	return 0;
}

int MapPropPage::EditDefinition(Event*)
{
	String sCmd("editdef %S", m_obj->fnObj.sFullNameQuoted());
	GetParent()->SendMessage(WM_SYSCOMMAND, SC_CLOSE, 0);
	IlwWinApp()->Execute(sCmd);
	return 0;
}

// Other functions
void MapPropPage::StoreTypeInfo(StoreType st) const
{
	int iBytes = 1;
	switch (st)
	{
		case stINT:  iBytes = 2; break;
		case stLONG: iBytes = 4; break;
		case stREAL: iBytes = 8; break;
	}

//	String sRemark = String(SMSMsgMapUses_i, iBytes);
//	if (1 == iBytes)
//		sRemark &= SMSMsgByte;
//	else  
//		sRemark &= SMSMsgBytes;
//	sRemark &= SMSMsg_PerPixel;  
//	stStoreType->SetVal(sRemark);
}

// reload current ValueRange in edit controls
void MapPropPage::UpdateValueRangeFields()
{
	if (m_fvr && m_vr.fValid())
		m_fvr->SetVal(m_vr);
}

int MapPropPage::exec()
{
	BaseMapPropPage::exec();

	MapPtr* pmp = dynamic_cast<MapPtr*>(m_bm.pointer());
	if (!pmp)
		return 0;

	// Change Georeference
	if (m_sNewGR.length() > 0)
	{
		FileName fnGR = IlwisObjectPtr::fnCheckPath(FileName(m_sNewGR, ".grf"));
		if (fnGR != pmp->gr()->fnObj)
		{
			GeoRef gr(fnGR);
			if (gr.fValid())
				pmp->SetGeoRef(gr);
		}
	}

	// Change interpolation method (value maps only)
	if (pmp->dm()->pdv())
	{
		InterpolMethod im = imNEARNEIGHB;
		if (m_fInterpol)
			im = InterpolMethod(m_iInterPolMethod + 1);
		if (im != pmp->interpolMethod())
		{
			pmp->SetInterpolMethod(im);
			pmp->Updated();
		}
	}

	return 0;
}

//-------------------------------
// SegmentMapPropPage member functions
SegmentMapPropPage::SegmentMapPropPage(const IlwisObject& obj)
	: BaseMapPropPage(obj, SMSPropSegmentMap)
{
	SetMenHelpTopic(htpPropInfoSegMap);
}

void SegmentMapPropPage::SetMapDetails()
{
	SegmentMapPtr* psm = dynamic_cast<SegmentMapPtr*>(m_bm.ptr()); // can not fail, current object is a SegmentMap for sure

	BaseMapPropPage::SetMapDetails();

	StaticText* st;  
	String s(SMSRemSegments_i.scVal(), psm->iFeatures());
    if (psm->iSegDeleted() > 0)
      s &= String(SMSRemInclDelSeg_i.scVal(), psm->iSegDeleted());
	st = new InfoText(m_fgPageRoot, s);
}

//-------------------------------
// PointMapPropPage member functions
PointMapPropPage::PointMapPropPage(const IlwisObject& obj)
	: BaseMapPropPage(obj, SMSPropPointMap)
{
	SetMenHelpTopic(htpPropInfoPntMap);
}

void PointMapPropPage::SetMapDetails()
{
	PointMapPtr* ppm = dynamic_cast<PointMapPtr*>(m_bm.ptr()); // can not fail, current object is a PointMap for sure

	BaseMapPropPage::SetMapDetails();

	StaticText* st;  
	String s(SMSRemPoints_i.scVal(), ppm->iFeatures());
	st = new InfoText(m_fgPageRoot, s);
}

//-------------------------------
// PolygonMapPropPage member functions
PolygonMapPropPage::PolygonMapPropPage(const IlwisObject& obj)
	: BaseMapPropPage(obj, SMSPropPolygonMap)
{
	SetMenHelpTopic(htpPropInfoPolMap);
}

void PolygonMapPropPage::SetMapDetails()
{
	PolygonMapPtr* ppm = dynamic_cast<PolygonMapPtr*>(m_bm.ptr()); // can not fail, current object is a PolygonMap for sure

	BaseMapPropPage::SetMapDetails();

	StaticText* st;
	String s;
	if (ppm->fTopologicalMap())
		s = SMSRemTopologicalMap;
	else
		s = SMSRemNonTopologicalMap;
	st = new InfoText(m_fgPageRoot, s);

	s = String(SMSRemPolygons_i.scVal(), ppm->iFeatures());
	st = new InfoText(m_fgPageRoot, s);
}

//-------------------------------
// AdditionalInfoPage member functions
AdditionalInfoPage::AdditionalInfoPage(const IlwisObject& obj)
	: BasicPropertyFormPage(obj, SMSPropAdditionalInfo)
{
}

void AdditionalInfoPage::BuildPage()
{
	BasicPropertyFormPage::BuildPage();

	sInfo = m_obj->sAdditionalInfo();
	DWORD style = WS_GROUP | WS_TABSTOP | ES_MULTILINE | ES_WANTRETURN|
					ES_AUTOVSCROLL | WS_VSCROLL | WS_BORDER | ES_READONLY;        
	FieldStringMulti* fsm = new FieldStringMulti(m_fgPageRoot, &sInfo, style);
	fsm->SetFixedFont();
	fsm->SetWidth(240);
	fsm->SetHeight(200);
}

//-------------------------------
// TablePropPage member functions
TablePropPage::TablePropPage(const IlwisObject& obj)
	: BasicPropertyFormPage(obj, SMSPropTable)
{
	SetMenHelpTopic(htpPropInfoTable);
}

void TablePropPage::BuildPage()
{
	BasicPropertyFormPage::BuildPage();

	Table tbl(m_obj->fnObj);
	if (tbl->fUseAs()) // Foreign format
	{
		FileName fnData;
		ObjectInfo::ReadElement("ForeignFormat", "Filename", tbl->fnObj, fnData);
		if (fnData.fValid())
		{
			String sData(SMSMsgTableUseExternal_S.scVal(), fnData.sRelative(true, tbl->fnObj.sPath()));
			new InfoText(m_fgPageRoot, sData);
		}
	}
	
	if (fCIStrEqual(tbl->dm()->fnObj.sExt, ".dom"))
		new FieldObjShow(m_fgPageRoot, tbl->dm());
	else
		new InfoText(m_fgPageRoot, tbl->dm()->sTypeName());

	StaticText* st;
	String s(SMSRemRecsCols_ii.scVal(), tbl->iRecs(), tbl->iCols());
	st = new InfoText(m_fgPageRoot, s);
	DomainSort* ds = tbl->dm()->pdsrt();
	if (ds && ds->iNettoSize() != tbl->iRecs())
	{
		String s(SMSRemNettoRecs_i.scVal(), ds->iNettoSize());
		st->psn->SetBound(0,0,0,0);
		st = new InfoText(m_fgPageRoot, s);
	}
	if (tbl->fDependent())
		DisplayDefinition();
	if ( tbl->getVersionBinary() == ILWIS::Version::bvFORMATFOREIGN) {
		ObjectInfo::ReadElement("ForeignFormat","Query",tbl->fnObj,query);
		FieldStringMulti *fsQuery = new FieldStringMulti(m_fgPageRoot, "Query", &query);
		zDimension dim = fsQuery->Dim("gk");	
		fsQuery->SetWidth((short)(15 * dim.width()));
		fsQuery->SetHeight((short)(4 * dim.height()));
	}
}

int TablePropPage::exec()
{
	BasicPropertyFormPage::exec();
	ObjectInfo::WriteElement("ForeignFormat","Query",m_obj->fnObj,query);

	return 1;
}
//-------------------------------
// Table2DimPropPage member functions
Table2DimPropPage::Table2DimPropPage(const IlwisObject& obj)
	: BasicPropertyFormPage(obj, SMSProp2DimTable)
{
	SetMenHelpTopic(htpPropInfoTable2Dim);
}

void Table2DimPropPage::BuildPage()
{
	BasicPropertyFormPage::BuildPage();

	Table2Dim tbl(m_obj->fnObj);
	StaticText *st1, *st2;
	FieldObjShow* fos;

	FieldGroup *fgDomainPrim = new FieldGroup(m_fgPageRoot);
	st1 = new StaticText(fgDomainPrim, SMSRemPrimary);
	fos = new FieldObjShow(fgDomainPrim, tbl->dm1());  
	fos->Align(st1, AL_AFTER);

	FieldGroup *fgDomainSec = new FieldGroup(m_fgPageRoot);
	fgDomainSec->Align(fgDomainPrim, AL_UNDER);
	st2 = new StaticText(fgDomainSec, SMSRemSecondary);
	fos = new FieldObjShow(fgDomainSec, tbl->dm2());
	fos->Align(st2, AL_AFTER);

	if (tbl->iCols() == 1)
	{
		Column col = tbl->colValue();
		Domain dm = col->dm();
		fos = new FieldObjShow(m_fgPageRoot, dm);
		fos->Align(fgDomainSec, AL_UNDER);
		if (col->vr().fValid() && !dm->pdbool() && !dm->pdi() && !dm->pdbit())
		{
			StaticText *stValues = 0, *stPrecision = 0, *stUnit = 0;  // initialized in SetRangePrecFields
			SetRangePrecFields(dm->pdv(), col->vr(), m_fgPageRoot, stValues, stPrecision, stUnit);
		}  
	}
}

//-------------------------------
// HistogramPropPage member functions
HistogramPropPage::HistogramPropPage(const IlwisObject& obj)
	: BasicPropertyFormPage(obj, SMSPropHistogram)
{
	SetMenHelpTopic(htpPropInfoHistogram);
}

void HistogramPropPage::BuildPage()
{
	BasicPropertyFormPage::BuildPage();

	Table tbl(m_obj->fnObj);
	if (fCIStrEqual(tbl->dm()->fnObj.sExt, ".dom"))
		new FieldObjShow(m_fgPageRoot, tbl->dm());
	else
		new InfoText(m_fgPageRoot, tbl->dm()->sTypeName());
}

//-------------------------------
// GeoRefPropPage member functions
GeoRefPropPage::GeoRefPropPage(const IlwisObject& obj)
	: BasicPropertyFormPage(obj, SMSPropGeoReference)
{
	SetMenHelpTopic(htpPropInfoGeoRef);
	m_fm = 0;
}

void GeoRefPropPage::BuildPage()
{
	BasicPropertyFormPage::BuildPage();

	GeoRef grf(m_obj->fnObj);

	StaticText* st;
	double rPix = rUNDEF;
	String s;
	if (!grf->fGeoRefNone())
	{
		RowCol rc = grf->rcSize();
		s = String(SMSRemLinesCols_ii.scVal(), rc.Row, rc.Col);
		st = new InfoText(m_fgPageRoot, s);
		rPix = grf->rPixSize();
	}
	CoordSystem cs = grf->cs();
	if (rUNDEF != rPix)
	{
		st->psn->SetBound(0,0,0,0);
		if ( 0 != cs->pcsLatLon())
		{
			String sDMS = LatLon::sDegree(rPix,15,false);//fShowMinus == false
			s = String(SMSRemPixSizeDMS_S.scVal(),sDMS);		
		}
		else
			s = String(SMSRemPixSize_f.scVal(), rPix);
		st = new InfoText(m_fgPageRoot, s);
	}
	if (m_fReadOnly) 
		new FieldObjShow(m_fgPageRoot, cs);
	else
	{
		m_sNewCS = cs->sName();
		m_fcs = new FieldCoordSystemC(m_fgPageRoot, SMSUiCoordSys, &m_sNewCS);
		m_fcs->SetCallBack((NotifyProc)&GeoRefPropPage::CoordSysChange);
		
		String sDsc = sObjectDesc(cs);
		m_stCSDsc = new InfoText(m_fgPageRoot, sDsc);
		m_stCSDsc->psn->iBndDown = 0;
	}
	s = String('X', 60);
	m_stCS = new InfoText(m_fgPageRoot, s);
	m_stCS->SetVal(String());   // clear string, set again in callback: CoordSysChange

	// Display the boundaries of the GeoRef (as opposed to the boundaries of the CoordSys)
	RowCol rcSize = grf->rcSize();
	if (rcSize.Row != iUNDEF && rcSize.Col != iUNDEF)
	{
		FieldExtent *fex = new FieldExtent(m_fgPageRoot, grf);
		String sTitle = String("%S of \"%S\"", SMSUiCornersOfCorners, grf->sName());
		fex->SetTitle(sTitle);
		FieldBlank *fb = new FieldBlank(m_fgPageRoot, 0); // Used to force proper alignment
		fb->Align(fex, AL_UNDER);
	}

	gctp = grf->pgCTP();
	if (gctp)
	{
		sRefMap = gctp->fnBackgroundMap.sFullNameQuoted();
		if (m_fReadOnly)
		{
			String s = SMSUiReferenceMap;
			s &= " ";
			s &= sRefMap;
			new InfoText(m_fgPageRoot, s);
		}
		else
		{
			m_fm = new FieldDataType(m_fgPageRoot, SMSUiReferenceMap, &sRefMap, new MapListerGeoRef(grf->fnObj), true);
			m_fm->SetAllowEmpty(true);
			m_fm->SetCallBack((NotifyProc)&GeoRefPropPage::RefMapChange);
		}
	}
	Map mapDTM;
	GeoRef3D* g3d = grf->pg3d();
	GeoRefOrthoPhoto* gop = grf->pgOrthoPhoto();
	GeoRefDirectLinear* gdl = grf->pgDirectLinear();
	GeoRefParallelProjective* gpp = grf->pgParallelProjective();
	if (g3d)
		mapDTM = g3d->mapDTM;
	if (gop)
		mapDTM = gop->mapDTM;
	if (gdl)
		mapDTM = gdl->mapDTM;
	if (gpp)
		mapDTM = gpp->mapDTM;

	if (mapDTM.fValid())
	{
		FieldGroup* fgDTM = new FieldGroup(m_fgPageRoot);
		StaticText *stDTM = new InfoText(fgDTM, SMSUiDTM);
		FieldObjShow *fos = new FieldObjShow(fgDTM, mapDTM);
		fos->Align(stDTM, AL_AFTER);

		RowCol rcDTM = mapDTM->rcSize();
		String s = String(SMSRemLinesCols_ii.scVal(), rcDTM.Row, rcDTM.Col);

		st = new InfoText(m_fgPageRoot, s);
		st->psn->SetBound(0,0,0,0);
		st->Align(fgDTM, AL_UNDER);
		CoordBounds cbDTM = mapDTM->cb();
		CoordSystem csDTM = mapDTM->cs();
		if (csDTM.fValid() && !csDTM->fUnknown() && !csDTM->cb.fUndef())
			s = String(SMSMsgWithBoundaries_SS.scVal(),
								csDTM->sValue(cbDTM.cMin,0),
								csDTM->sValue(cbDTM.cMax,0));
		new InfoText(m_fgPageRoot, s);
	}
	
	s = String('X', 55);
	m_stRemark = new InfoText(m_fgPageRoot, s);
	m_stRemark->SetVal(String());   // clear remark string
}

// Callback functions
int GeoRefPropPage::RefMapChange(Event*)
{
	DataChanged(0);
	m_fm->StoreData();
	if (gctp)
		if (sRefMap.length() == 0)
			m_stRemark->SetVal(SMSRemBackMapRequired);
		else if (!File::fExist(FileName(sRefMap)))
		{
			if (fCIStrEqual(FileName(sRefMap).sExt, ".mpr"))
				m_stRemark->SetVal(SMSRemBackMapNotExist);
			else
				m_stRemark->SetVal(SMSRemBackMapListNotExist);
		}
		else
			m_stRemark->SetVal(String());
		
	return 0;
}

int GeoRefPropPage::CoordSysChange(Event*)
{
	DataChanged(0);
	m_fcs->StoreData();

	String s;
	String sDsc;
	if (m_sNewCS.length() > 0)
	{
		FileName fnCS(m_sNewCS);
		CoordSystem cs;
		try
		{
			cs = CoordSystem(fnCS);
		}
		catch (ErrorObject& err)
		{
			err.Show(SMSErrCsyChange);
		}

		String sRem;
		if (cs.fValid())
		{
			sDsc = sObjectDesc(cs);
			if (!cs->fUnknown() && !cs->cb.fUndef())
			{
				String sMin = cs->sValue(cs->cb.cMin,0);
				String sMax = cs->sValue(cs->cb.cMax,0);
				s = String(SMSMsgWithBoundaries_SS.scVal(), sMin, sMax);
			}

		}
		else
			sRem = SMSRemInvalidCsy;
		
		m_stRemark->SetVal(sRem);
	}

	m_stCS->SetVal(s);
	m_stCSDsc->SetVal(sDsc);

	return 0;
}

int GeoRefPropPage::exec()
{
	BasicPropertyFormPage::exec();

	GeoRef grf(m_obj->fnObj);
	GeoRefCTP *gctp = grf->pgCTP();
	if (gctp)
	{
		gctp->fnBackgroundMap = sRefMap;
		grf->Updated();
	}

	// Change CoordSystem
	if (m_sNewCS.length() > 0)
	{
		FileName fnCS = IlwisObjectPtr::fnCheckPath(FileName(m_sNewCS, ".csy"));
		if (fnCS != grf->cs()->fnObj)
		{
			CoordSystem cs(fnCS);
			if (cs.fValid())
				grf->SetCoordSystem(cs);
		}
	}

	return 0;
}

//-------------------------------
// CoordSystemPropPage member functions
CoordSystemPropPage::CoordSystemPropPage(const IlwisObject& obj)
	: BasicPropertyFormPage(obj, SMSPropCoordSystem)
{
	SetMenHelpTopic(htpPropInfoCoordSys);
}

void CoordSystemPropPage::BuildPage()
{
	BasicPropertyFormPage::BuildPage();

	CoordSystem cs(m_obj->fnObj);
	String s;
	if (cs.fValid() && !cs->fUnknown() && !cs->cb.fUndef())
		s = String(SMSMsgBoundaries_SS.scVal(),
					cs->sValue(cs->cb.cMin,0),
					cs->sValue(cs->cb.cMax,0));
	StaticText* st = new InfoText(m_fgPageRoot, s);

	CoordSystemDirect* csd = cs->pcsDirect();
	if (csd)
	{
		st->psn->SetBound(0,0,0,0);
		s = String(SMSMsgRefCoordSys_S.scVal(), csd->csOther->sName());
		st = new InfoText(m_fgPageRoot, s);
		CoordSystemCTP* csc = cs->pcsCTP();
		if (csc)
		{
			m_sRefMap = csc->fnBackgroundMap.sRelative();
			String s = SMSUiReferenceMap;
			s &= " ";
			s &= m_sRefMap;
			new InfoText(m_fgPageRoot, s);

			Map mapDTM;
			CoordSystemOrthoPhoto *cso = cs->pcsOrthoPhoto();
			CoordSystemDirectLinear *csd = cs->pcsDirectLinear();
			if (cso)
				mapDTM = cso->mapDTM;
			if (csd)
				mapDTM = csd->mapDTM;

			if (mapDTM.fValid())
			{
				RowCol rcDTM = mapDTM->rcSize();
				String s = String("%S \"%S\", %S", SMSUiDTM, mapDTM->sName(),
													String(SMSRemLinesCols_ii.scVal(), rcDTM.Row, rcDTM.Col));
				st = new InfoText(m_fgPageRoot, s);
				st->psn->SetBound(0,0,0,0);
				CoordBounds cbDTM = mapDTM->cb();
				CoordSystem csDTM = mapDTM->cs();
				if (csDTM.fValid() && !csDTM->fUnknown() && !csDTM->cb.fUndef())
					s = String(SMSMsgWithBoundaries_SS.scVal(),
										csDTM->sValue(cbDTM.cMin,0),
										csDTM->sValue(cbDTM.cMax,0));
				new InfoText(m_fgPageRoot, s);
			}
		}
	}
	CoordSystemProjection* csp = cs->pcsProjection();
	if (csp && csp->prj.fValid())
	{
		st->psn->SetBound(0,0,0,0);
		s = String(SMSMsgProjection_S.scVal(), csp->prj->sName());
		st = new InfoText(m_fgPageRoot, s);
		if (abs(csp->prj->iGetZoneNr()) <= csp->prj->iMaxZoneNr()) {
			st->psn->SetBound(0,0,0,0);
			s = String(SPRJInfoProjZoneNumber_i.scVal(),csp->prj->iGetZoneNr());
			st = new InfoText(m_fgPageRoot, s);
		}
		if (csp->prj->sName() == "utm")
		{
			if (csp->prj->fGetHemisphereN() == true) 
				s = String(SPRJInfoProjHemisphereN);
			if (csp->prj->fGetHemisphereN() == false) 
				s = String(SPRJInfoProjHemisphereS);
			if (s.length()) st = new InfoText(m_fgPageRoot, s);
		}
		if (abs(csp->prj->lam0) < M_PI) {
			st->psn->SetBound(0,0,0,0);
			s = String(SPRJInfoCentralMeridian_f.scVal(),csp->prj->lam0*180/M_PI);
			st = new InfoText(m_fgPageRoot, s);
		}
		if (abs(csp->prj->rGetCentralScaleFactor()) != 1){
			st->psn->SetBound(0,0,0,0);
			s = String(SPRJInfoCentralScaleF_f.scVal(),csp->prj->rGetCentralScaleFactor());
			st = new InfoText(m_fgPageRoot, s);
		}
	}
	CoordSystemViaLatLon* csvll = cs->pcsViaLatLon();
	if (csvll)
	{
		if (csvll->datum)
		{
			st->psn->SetBound(0,0,0,0);
			String s= SCSInfDatum;
			s &= csvll->datum->sName();
			st = new InfoText(m_fgPageRoot, s);
			if (csvll->datum->sArea.length())
			{
				st->psn->SetBound(0,0,0,0);
				s = SCSInfDatumArea;
				s &= csvll->datum->sArea;
				st = new InfoText(m_fgPageRoot, s);
			}
		}
		st->psn->SetBound(0,0,0,0);
		if (csvll->ell.fSpherical())
		{
			double rRadius = csvll->ell.a;
			if (rRadius < 0.001) {
				Ellipsoid* ell = new Ellipsoid("Sphere");// authalic sphere
				rRadius = ell->a;//was: 6371007, now: 6371007.1809185;//sphere with equal area as WGS 84
			}
			String s("%S %.7f", SCSUiSphereRadius, rRadius);
			st = new InfoText(m_fgPageRoot, s);
		}
		else
		{
			String s = SCSInfEll;
			s &= csvll->ell.sName;
			st = new InfoText(m_fgPageRoot, s);
		}
	}
}

//-------------------------------
// RepresentationPropPage member functions
RepresentationPropPage::RepresentationPropPage(const IlwisObject& obj)
	: BasicPropertyFormPage(obj, SMSPropRepresentation)
{
	SetMenHelpTopic(htpPropInfoRepresentation);
}

void RepresentationPropPage::BuildPage()
{
	BasicPropertyFormPage::BuildPage();

	Representation rpr(m_obj->fnObj);
	new FieldObjShow(m_fgPageRoot, rpr->dm());
}

//-------------------------------
// DomainPropPage member functions
DomainPropPage::DomainPropPage(const IlwisObject& obj)
	: BasicPropertyFormPage(obj, SMSPropDomain), m_frc(0), m_ftAttTable(0)
{
	SetMenHelpTopic(htpPropInfoDomain);
	m_fAttrTable = false;
}

String DomainPropPage::sDomainConvertCommand()
{
	return m_sCvtCommand;
}

void DomainPropPage::BuildPage()
{
	BasicPropertyFormPage::BuildPage();

	Domain dm(m_obj->fnObj);

	m_iWidth = dm->iWidth();
	if (m_fReadOnly)
	{
		String s("%i", m_iWidth);
		StaticText *stWidthCaption = new StaticText(m_fgPageRoot, SMSUiWidth);
		StaticText *stWidth = new StaticText(m_fgPageRoot, s);
		stWidth->Align(stWidthCaption, AL_AFTER);
		FieldBlank *fb = new FieldBlank(m_fgPageRoot, 0);
		fb->Align(stWidthCaption, AL_UNDER);
	}
	else
		new FieldInt(m_fgPageRoot, SMSUiWidth, &m_iWidth);

	DomainSort* ds = dm->pdsrt();
	if (ds)
	{
		m_fAttrTable = ds->fTblAtt();
		m_sAttrTable = ds->sName(false);
		m_sAttrTable &= ".tbt";
		if (m_fAttrTable)
		{
			Table tbl = ds->tblAtt();
			if (tbl.fValid())
				m_sAttrTable = tbl->sName(true);
		}
		else
			m_sAttrTable = String();

		if (m_fReadOnly)
		{
			if (m_fAttrTable)
			{
				String s = SMSUiAttrTable;
				s &= " ";
				s &= m_sAttrTable;
				new StaticText(m_fgPageRoot, s);
			}
		}
		else
		{
			CheckBox* cb = new CheckBox(m_fgPageRoot, SMSUiAttrTable, &m_fAttrTable);
			m_ftAttTable = new FieldTableC(cb, "", &m_sAttrTable, dm);
			m_ftAttTable->SetCallBack((NotifyProc)&DomainPropPage::CallBackAttTableChange); 
			String s('X', 50);
			m_stAttTable = new InfoText(cb, s);
			m_stAttTable->Align(cb, AL_UNDER);
			m_stAttTable->SetVal(String());   // clear string
		}

		String s(SMSRemDomItems_i.scVal(), ds->iNettoSize());
		new InfoText(m_fgPageRoot, s);
	}
	else if (dm->pdp())
	{
		String s(SMSRemDomColors_i.scVal(), dm->pdp()->iColors());
		new InfoText(m_fgPageRoot, s);
	}
	if (dm->pdvr() || dm->pdvi() || dm->pdi() || dm->pdp() || dm->pdc())
	{
		try
		{
			Representation rpr = dm->rpr();
			if (rpr.fValid())
				m_sRprName = rpr->sName(true);
		}
		catch (ErrorObject& err)
		{
			err.Show(SMSErrRprChange);
		}

		if (m_fReadOnly)
		{
			new FieldObjShow(m_fgPageRoot, dm->rpr());  

		}
		else
		{
			m_frc = new FieldRepresentationC(m_fgPageRoot, SMSUiRpr, &m_sRprName, dm);
			m_frc->SetCallBack((NotifyProc)&DomainPropPage::CallBackRprChange);

			String sFill('x', 55);
			m_stRpr = new InfoText(m_fgPageRoot, sFill);
			m_stRpr->SetVal(String());
		}
		DomainValue* dv = dm->pdv();
		if (0 != dv) 
		{
			ValueRange vr(dm);
			StaticText *stValues = 0, *stPrecision = 0, *stUnit = 0;  // Initialized in SetRangePrecFields
			SetRangePrecFields(dv, vr, m_fgPageRoot, stValues, stPrecision, stUnit);
			StaticText* st = 0;
			if (dv->fFixedPrecision() && dv->fFixedRange())
				st = new StaticText(m_fgPageRoot, SMSRemFixedRangePrecision);
			else if (dv->fFixedPrecision())  
				st = new StaticText(m_fgPageRoot, SMSRemFixedPrecision);
			else if (dv->fFixedRange())
				st = new StaticText(m_fgPageRoot, SMSRemFixedRange);
			if (st)
				st->SetIndependentPos();
		}
	}
	if (!m_fReadOnly)
	{
		if (dm->pdid())
			(new PushButton(m_fgPageRoot, SMSUiConvToClass, (NotifyProc)&DomainPropPage::ToClasses))->SetIndependentPos();
		if (dm->pdc())
			(new PushButton(m_fgPageRoot, SMSUiConvToID, (NotifyProc)&DomainPropPage::ToIds))->SetIndependentPos();
		if (dm->pdp())
			(new PushButton(m_fgPageRoot, SMSUiConvToClass, (NotifyProc)&DomainPropPage::PictureToClasses))->SetIndependentPos();
	}
}

// Callback functions
int DomainPropPage::CallBackAttTableChange(Event*)
{
	DataChanged(0);

	m_ftAttTable->StoreData();
	if (m_sAttrTable.length() == 0)
	{
		m_stAttTable->SetVal("");
		return 0;
	}

	FileName fn(m_sAttrTable, ".tbt");
	String sRemark;
	Table tbl;
	try
	{
		tbl = Table(fn);
	}
	catch (ErrorObject& err)
	{
		err.Show(SMSErrAttrTableChange);
	}

	if (tbl.fValid()) 
		sRemark = tbl->sDescription;
	else  
		sRemark = "";

	m_stAttTable->SetVal(sRemark);
	return 0;
}

int DomainPropPage::CallBackRprChange(Event*) 
{
	DataChanged(0);

	m_frc->StoreData();
	FileName fn(m_sRprName);
	String sRemark;
	Representation rpr;
	try
	{
		rpr = Representation(fn);
	}
	catch (ErrorObject& err)
	{
		err.Show(SMSErrRprChange);
	}  
	if (rpr.fValid())
		sRemark = rpr->sDescription;
	else  
		sRemark = SMSRemInvalidRpr;

	m_stRpr->SetVal(sRemark);

	return 1;  
}

// for the convert domain routines:
// The execute command is prepared here, but only executed after the property
// form are closed. This is necessary to get the iRef of the domain back to 1
// (so not used by any object)
int DomainPropPage::ToClasses(Event*)
{
	m_sCvtCommand = String("domidtoclass %S", m_obj->fnObj.sFullNameQuoted());
	GetParent()->SendMessage(WM_SYSCOMMAND, SC_CLOSE, 0);

	return 0;
}

int DomainPropPage::PictureToClasses(Event*)
{
	m_sCvtCommand = String("dompictoclass %S", m_obj->fnObj.sFullNameQuoted());
	GetParent()->SendMessage(WM_SYSCOMMAND, SC_CLOSE, 0);

	return 0;
}

int DomainPropPage::ToIds(Event*)
{
	m_sCvtCommand = String("domclasstoid %S", m_obj->fnObj.sFullNameQuoted());
	GetParent()->SendMessage(WM_SYSCOMMAND, SC_CLOSE, 0);

	return 0;
}

int DomainPropPage::exec()
{
	BasicPropertyFormPage::exec();

	Domain dm(m_obj->fnObj);

	if (dm->iWidth() != m_iWidth)
		dm->SetWidth(m_iWidth);

	// Set representation
	if (m_frc)
	{
		try
		{
			FileName fnRpr = IlwisObjectPtr::fnCheckPath(FileName(m_sRprName, ".rpr"));
			if (fnRpr != dm->rpr()->fnObj)
			{
				Representation rpr(fnRpr);
				if (dm->rpr() != rpr)
					dm->SetRepresentation(rpr);
			}
		}
		catch (ErrorObject& err)
		{
			err.Show();
		}
	}

	// Set attribute table
	DomainSort *ds = dm->pdsrt();
	if (0 == m_ftAttTable || !ds)
		return 0;
	
	Table tbl; 
	if (m_fAttrTable)
	{
		try
		{
			FileName fn(m_sAttrTable);
			if (fn.fValid()) 
				tbl = Table(fn);
		}
		catch (ErrorObject& err)
		{
			err.Show(SMSErrAttrTable);
		} 
		m_fAttrTable = tbl.fValid();
	}

	if (!m_fAttrTable)
	{
		if (ds->fTblAtt())  // Only set in case the domain DID have an attribute table
			ds->SetNoAttributeTable();
	}
	else
	{
		bool fHasAtt = ds->fTblAtt();
		bool fNeedChange = !fHasAtt;
		if (fHasAtt)  // Check if the new tblattr is equal to the one currently in use
		{
			Table tblAtt = ds->tblAtt();
			fNeedChange = (tbl != tblAtt);
		}
		if (fNeedChange)  // Only set in case the domain did NOT have an attribute table or
			              // another table has been specified
		{
			if (ds->fEqual(*tbl->dm().ptr()))
				ds->SetAttributeTable(tbl);
			else
				MessageBox(SMSErrAttrSameAsDomain.scVal(), SMSErrError.scVal());
		}
	}  

	return 0;
}

//-------------------------------
// SampleSetPropPage member functions
SampleSetPropPage::SampleSetPropPage(const IlwisObject& obj)
	: BasicPropertyFormPage(obj, SMSPropSampleSet)
{
	SetMenHelpTopic(htpPropInfoSampleSet);
}

void SampleSetPropPage::BuildPage()
{
	BasicPropertyFormPage::BuildPage();

	SampleSet sams(m_obj->fnObj);
	sms = sams.ptr();

	new FieldObjShow(m_fgPageRoot, sms->dm());  
	Map mp = sms->mapBack();
	if (mp.fValid())
		sBgMap = mp->fnObj.sFullNameQuoted();
	MapList mpl = sms->mpl();
	if (mpl.fValid())
		sMapList = mpl->fnObj.sFullNameQuoted();
	Map mpSmp = sms->map();
	if (mpSmp.fValid())
		sSmpMap = mpSmp->fnObj.sFullNameQuoted();

	CDC *cdc = CWnd::GetDesktopWindow()->GetWindowDC();
	fPalette = (cdc->GetDeviceCaps(RASTERCAPS) & RC_PALETTE) != 0;
	if (m_fReadOnly)
	{
		String s;
		s = SMSUiSampleMap;
		s &= " ";
		s &= sSmpMap;
		new StaticText(m_fgPageRoot, s);
		s = SMSUiMapList;
		s &= " ";
		s &= sMapList;
		new StaticText(m_fgPageRoot, s);
		if (fPalette)
		{
			s = SMSUiBackground;
			s &= " ";
			s &= sBgMap;
			new StaticText(m_fgPageRoot, s);
		}
	}
	else
	{
		FieldGroup* fg = new FieldGroup(m_fgPageRoot, true);

		MapListerDomainAndGeoRef *ol = new MapListerDomainAndGeoRef(mpl->gr()->fnObj, mpl->rcSize(), sms->dm()->fnObj);
		m_fldMap = new FieldMap(fg, SMSUiSampleMap, &sSmpMap, ol);
		m_fldMPL = new FieldMapList(fg, SMSUiMapList, &sMapList);
		m_fldMPL->SetCallBack((NotifyProc)&SampleSetPropPage::MapListChange);
		if (fPalette)
			new FieldMap(fg, SMSUiBackground, &sBgMap);
	}  
}

int SampleSetPropPage::MapListChange(Event*)
{
	FileName fnPrev(sMapList);  // current selected map list
	m_fldMap->StoreData();
	m_fldMPL->StoreData();
	FileName fnMPL(sMapList);   // new selected map list
	if (fnMPL == fnPrev)
		return 0;

	MapList mpl(fnMPL);
	SampleSet sms(m_obj->fnObj);
	MapListerDomainAndGeoRef *ol = new MapListerDomainAndGeoRef(mpl->gr()->fnObj, mpl->rcSize(), sms->dm()->fnObj);
	m_fldMap->SetObjLister(ol); // update the sample map constraints (domain and georef)

	if (!sSmpMap.empty())
	{
		Map mpSampleMap(sSmpMap);
		if (mpSampleMap->gr() != mpl->gr())
			sSmpMap = String();      // the previously selected sample map does not have the same GRF as the new maplist
	}
	m_fldMap->SetVal(sSmpMap);       // reselect the previously selected sample map (if any)

	return 0;
}

int SampleSetPropPage::exec()
{
	BasicPropertyFormPage::exec();

	Map mpSmp(sSmpMap);
	FileName fnMPL = FileName(sMapList);
	MapList mpl(fnMPL);
	if (mpl->gr() != mpSmp->gr())
	{
		MessageBox(SSSErrNotSameGeoRefSmp.scVal(), SMSErrError.scVal(), MB_OK|MB_ICONSTOP);
		return 0;
	}  
	if (fPalette)
	{
		Map mp(sBgMap);
		if (mpl->gr() != mp->gr())
		{
			MessageBox(SSSErrNotSameGeoRef.scVal(), SMSErrError.scVal(), MB_OK|MB_ICONSTOP);
			return 0;
		}
		sms->SetBackground(mp);
	}
	// Set the new maplist and samplemap; the maplist must be set first
	// because the samplemap is then checked against the maplist of the sample set
	sms->SetMapList(mpl);
	sms->SetSampleMap(mpSmp);

	return 0;
}

//-------------------------------
// MapListPropPage member functions
MapListPropPage::MapListPropPage(const IlwisObject& obj)
	: BasicPropertyFormPage(obj, SMSPropMapList)
{
	SetMenHelpTopic(htpPropInfoMapList);
	m_fInGRCallBack = false;
	m_stPyramids = 0;
	pbPyramids = 0;
}

void MapListPropPage::BuildPage()
{
	BasicPropertyFormPage::BuildPage();
	
	MapList mpl(m_obj->fnObj);
	
	if (mpl->fUseAs())
	{
		FileName fnData;
		// First check if foreign format
		ObjectInfo::ReadElement("ForeignFormat:1", "Filename", mpl->fnObj, fnData);
		if (!fnData.fValid()) // No foreign format, check native
			ObjectInfo::ReadElement("MapStore:1", "Data", mpl->fnObj, fnData);
		String sData(SMSMsgMapListUseExternal_S.scVal(), fnData.sRelative(true, mpl->fnObj.sPath()));
		new InfoText(m_fgPageRoot, sData);
	}
	
	int iMaps = mpl->iSize();
	if (iMaps > 0 || ! mpl->fDependent())
	{
		String sNrMaps(SMSRemRasterMaps_i.scVal(), iMaps);
		new InfoText(m_fgPageRoot, sNrMaps);
	}    
	
	if (iMaps > 0)
	{
		if (m_fReadOnly || mpl->fDependent())
			new FieldObjShow(m_fgPageRoot, mpl->gr());
		else
		{
			const Map& mpFirst = mpl[mpl->iLower()];
			CoordBounds cbLoc = mpFirst->gr()->fGeoRefNone() ? CoordBounds(Coord(), Coord()) : mpFirst->cb();
			m_sNewGR = mpl->gr()->sName();
			m_fgr = new FieldGeoRefC(m_fgPageRoot, SMSUiGrf, &m_sNewGR, grALL);
			m_fgr->SetBounds(mpFirst->cs(), cbLoc);
			m_fgr->SetCallBack((NotifyProc)&MapListPropPage::GeoRefChange);
			
			String sDsc = sObjectDesc(mpFirst->gr());
			StaticText *stGRDsc = new InfoText(m_fgPageRoot, sDsc);
		}
		new FieldObjShow(m_fgPageRoot, mpl->gr()->cs());
		
		new InfoText(m_fgPageRoot, SMSUiMapsDomain);
		Domain dom1 = mpl[mpl->iLower()]->dm();
		new FieldObjShow(m_fgPageRoot, dom1);
	}
	
	String s('X', 55);
	m_stRemark = new InfoText(m_fgPageRoot, s);
	m_stRemark->SetVal(String());   // clear remark string
	
	DisplayDefinition();
	
	if (iMaps > 0)
	{
		String sRem;
		if (!fCheckGeoRefConsistent(sRem))
			m_stRemark->SetVal(sRem);
		
		if (!mpl->fDependent() || (mpl->fDependent() && mpl->fCalculated()))
		{
			String sPyrText, sPyrButton;
			if (mpl->fHasPyramidFiles())
			{
				sPyrText = SMSPropMPLPyrAvailable;
				sPyrButton = SMSPropRemovePyr;
			}
			else
			{
				sPyrText = SMSPropMPLPyrNotAvailable;
				sPyrButton = SMSPropCreatePyr;
			}
			// Force the width of the statictext to the length of the longest possible string
			// And after that set the correct text
			m_stPyramids = new StaticText(m_fgPageRoot, SMSPropMPLPyrNotAvailable);
			m_stPyramids->SetVal(sPyrText);
			if ( !mpl->fReadOnly())
			{
				pbPyramids = new PushButton(m_fgPageRoot, sPyrButton, (NotifyProc)&MapListPropPage::CallBackPyramids);
				pbPyramids->Align(m_stPyramids, AL_AFTER);
			}
		}		
	}    
}

int MapListPropPage::CallBackPyramids(Event *)
{
	MapList mpl(m_obj->fnObj);	
	if ( mpl->fHasPyramidFiles() )
	{
		mpl->DeletePyramidFiles();
	}		
	else
	{
		mpl->CreatePyramidLayers();
		mpl->Store();
	}

	// Adjust the text of static text and the button to reflect the current pyramid status
	// It is not combined above, because if for some reason the removal/creation fails
	// we still display the correct information, by checking afterwards
	String sPyrText, sPyrButton;
	if (mpl->fHasPyramidFiles())
	{
		sPyrText = SMSPropMPLPyrAvailable;
		sPyrButton = SMSPropRemovePyr;
	}
	else
	{
		sPyrText = SMSPropMPLPyrNotAvailable;
		sPyrButton = SMSPropCreatePyr;
	}
	if (pbPyramids)
		pbPyramids->SetText(sPyrButton);
	m_stPyramids->SetVal(sPyrText);

	return 1;	
}

// Callback functions
int MapListPropPage::GeoRefChange(Event*)
{
	if (m_fInGRCallBack)
		return 0;

	DataChanged(0);
	m_fgr->StoreData();
	GeoRef gr;

	if (m_sNewGR.length() == 0)
		return 0;

	MapListPtr* pml = dynamic_cast<MapListPtr*>(m_obj.pointer()); 
	if (!pml)
		return 0;

	FileName fnOldGR = pml->gr()->fnObj;
	try
	{
		FileName fnGR(m_sNewGR);
		gr = GeoRef(fnGR);
	}
	catch(const ErrorObject& err)
	{
		err.Show(SMSErrGrfChange);
	}
	if (gr->fnObj == fnOldGR)  // no change
		return 0;

	if (dynamic_cast<GeoRefNone*>(gr.ptr()) == 0 &&
		pml->rcSize() != gr->rcSize())
	{
		Beep(440, 150);
		String s= SMSErrGrfIndDiffSize;
		s &= "\n";
		s &= SMSErrGrfNotChanged;
		MessageBox(s.scVal(), SMSErrError.scVal(), MB_ICONEXCLAMATION | MB_OK);
		m_fInGRCallBack = true;

		m_fgr->SetVal(fnOldGR.sFullName());

		m_fInGRCallBack = false;
	}
	else
	{
		String s = SMSMsgChangingGeoRefAccept;
		if (IDNO == MessageBox(s.scVal(), SMSMsgWarning.scVal(), MB_ICONEXCLAMATION | MB_YESNO))
		{
			m_fInGRCallBack = true;

			m_fgr->SetVal(fnOldGR.sFullName());

			m_fInGRCallBack = false;
		}
		else
			m_stRemark->SetVal(String());
	}

	const Map& mpFirst = pml->map(pml->iLower());
	CoordBounds cbLoc = gr->fGeoRefNone() ? CoordBounds(Coord(), Coord()) : mpFirst->cb();
	m_fgr->SetBounds(gr->fGeoRefNone() ? mpFirst->cs() : gr->cs(), cbLoc);

	String sRem;
	if (!fCheckGeoRefConsistent(sRem))
		m_stRemark->SetVal(sRem);

	return 0;
}

bool MapListPropPage::fCheckGeoRefConsistent(String& sRemark)
{
	MapListPtr* pml = dynamic_cast<MapListPtr*>(m_obj.pointer()); 
	if (!pml)
		return 0;  // actually pretty severe error!

	bool fOK = true;
	sRemark = String();
	try
	{
		pml->CheckGeoRefs();
	}
	catch (const ErrorIncompatibleMapSize& err)
	{
		sRemark = err.sWhat();
		fOK = false;
	}
	catch (const ErrorIncompatibleGeorefs& err)
	{
		sRemark = err.sWhat();
		fOK = false;
	}

	return fOK;
}

int MapListPropPage::exec()
{
	BasicPropertyFormPage::exec();

	MapListPtr* pml = dynamic_cast<MapListPtr*>(m_obj.pointer());
	if (!pml)
		return 0;

	// Change Georeference
	if (m_sNewGR.length() > 0)
	{
		FileName fnGR = IlwisObjectPtr::fnCheckPath(FileName(m_sNewGR, ".grf"));
		String sRem;
		if (fnGR != pml->gr()->fnObj || !fCheckGeoRefConsistent(sRem))
		{
			GeoRef gr(fnGR);
			if (gr.fValid())
				pml->SetGeoRef(gr);
		}
	}

	return 0;
}

//-------------------------------
// MapViewPropPage member functions
MapViewPropPage::MapViewPropPage(const IlwisObject& obj)
	: BasicPropertyFormPage(obj, SMSPropMapView)
{
	SetMenHelpTopic(htpPropInfoMapView);
}

void MapViewPropPage::BuildPage()
{
	BasicPropertyFormPage::BuildPage();

	MapView mpv(m_obj->fnObj);

	GeoRef georef;
	mpv->ReadElement("MapView", "GeoRef", georef);
	new FieldObjShow(m_fgPageRoot, georef);
	CoordSystem cs;
	mpv->ReadElement("MapView", "CoordSystem", cs);
	new FieldObjShow(m_fgPageRoot, cs);
}

//-------------------------------
// DependsOnPropPage member functions
DependsOnPropPage::DependsOnPropPage(const IlwisObject& obj)
	: BasicPropertyFormPage(obj, SMSPropDependency)
{
}

void DependsOnPropPage::BuildPage()
{
	BasicPropertyFormPage::BuildPage();

	DisplayDefinition();
	
	FormEntry* fe;
	bool fIsUpToDate = true;
	if (m_obj->fCalculated())
	{
		Time timNewest = 0;
		String sObjName;
		m_obj->GetNewestDependentObject(sObjName, timNewest);
		fIsUpToDate = timNewest == 0;
		switch (timNewest)
		{
			case 0:
				fe = new StaticText(m_fgPageRoot, SMSRemObjectIsUpToDate);
				break;
			case -1:
				fe = new StaticText(m_fgPageRoot, String("%S: %S", SMSRemObjectMissing, sObjName));
				break;
			default:
				fe = new StaticText(m_fgPageRoot, String("%S: %S (%S)", SMSRemObjectIsNotUpToDate, sObjName, timNewest.sDateTime()));
		}
	}
	else
		fe = new StaticText(m_fgPageRoot, SMSRemObjectNotCalculated);

	StaticText* st = new StaticText(m_fgPageRoot, SMSRemDependsOn);
	FieldDependencyTree *fdt = new FieldDependencyTree(m_fgPageRoot, m_obj);
	fdt->SetIndependentPos();

	FieldGroup *fgButtons = new FieldGroup(m_fgPageRoot, true);
	fgButtons->Align(fdt, AL_AFTER);
	PushButton *pb;
	if (m_obj->fCalculated())
	{
		if (!fIsUpToDate)
			pb = new PushButton(fgButtons, SMSUiMakeUpToDate, (NotifyProc)&DependsOnPropPage::CheckUpToDate);

		if (!m_fReadOnly)
			pb = new PushButton(fgButtons, SMSUiReleaseDiskSpace, (NotifyProc)&DependsOnPropPage::MakeSpace);
	}  
	else
		pb = new PushButton(fgButtons, SMSUiCalculate, (NotifyProc)&DependsOnPropPage::Calculate);

	if (!m_fReadOnly)
	{
		// For Break depenedency exclude all histogram types
		String sHisType = m_obj->sType().sLeft(9);
		if (!fCIStrEqual(sHisType, "Histogram"))
			pb = new PushButton(fgButtons, SMSUiBreakDepLink, (NotifyProc)&DependsOnPropPage::BreakLink);
	}
}

int DependsOnPropPage::CheckUpToDate(Event*)
{
	if (m_obj->fUpToDate())
	{
		String s(SMSMsgIsUpToDate_S.scVal(), m_obj->sTypeName());
		MessageBox(s.scVal(), SMSMsgCheckUpToDate.scVal(), MB_OK|MB_ICONINFORMATION);
	}
	else
	{
		String s1(SMSMsgIsNotUpToDate_S.scVal(), m_obj->sTypeName());
		String s("%S\n%S", s1, SMSMsgRecalcMakeUpToDate);
		if (IDYES == MessageBox(s.scVal(), SMSMsgCheckUpToDate.scVal(), MB_YESNO|MB_ICONINFORMATION))
		{
			String sCmd("makeuptodate %S", m_obj->fnObj.sFullNameQuoted());
			GetParent()->SendMessage(WM_SYSCOMMAND, SC_CLOSE, 0);
			IlwWinApp()->Execute(sCmd);
		}  
	}
	return 0;
}

int DependsOnPropPage::MakeSpace(Event*)
{
	String s = SMSMsgReleaseDiskSpace;
	s &= "\n";
	s &= SMSMsgSure;
	if (IDYES == MessageBox(s.scVal(), SMSMsgReleaseDiskSpace.scVal(), MB_YESNO|MB_ICONQUESTION))
	{
		String sCmd("reldiskspace %S", m_obj->fnObj.sFullNameQuoted());
		GetParent()->SendMessage(WM_SYSCOMMAND, SC_CLOSE, 0);
		IlwWinApp()->Execute(sCmd);
	}
	return 0;
}

int DependsOnPropPage::Calculate(Event*)
{
	String sCmd("calculate %S", m_obj->fnObj.sFullNameQuoted());
	GetParent()->SendMessage(WM_SYSCOMMAND, SC_CLOSE, 0);
	IlwWinApp()->Execute(sCmd);
	return 0;
}

int DependsOnPropPage::BreakLink(Event*)
{
	String s = SMSMsgBreakDepLink;
	s &= "\n";
	s &= SMSMsgSure;
	if (IDYES == MessageBox(s.scVal(), SMSMsgBreakDepLink.scVal(), MB_YESNO|MB_ICONQUESTION))
	{
		String sCmd("breakdep %S", m_obj->fnObj.sFullNameQuoted());
		GetParent()->SendMessage(WM_SYSCOMMAND, SC_CLOSE, 0);
		IlwWinApp()->Execute(sCmd);
	}  
	return 0;
}

//-------------------------------
// UsedByPropPage member functions
UsedByPropPage::UsedByPropPage(const IlwisObject& obj)
	: BasicPropertyFormPage(obj, SMSPropUsedBy)
{
}

void UsedByPropPage::BuildPage()
{
	BasicPropertyFormPage::BuildPage();

	StaticText* st = new StaticText(m_fgPageRoot, SMSRemUsedBy);
	new FieldUsedByTree(m_fgPageRoot, m_obj);
}

//-------------------------------
// ContainsPropPage member functions
ContainsPropPage::ContainsPropPage(const IlwisObject& obj)
	: BasicPropertyFormPage(obj, SMSPropContains)
{
}

void ContainsPropPage::BuildPage()
{
	BasicPropertyFormPage::BuildPage();

	CollectContainedFiles();

	StaticText* st = new StaticText(m_fgPageRoot, SMSUiObjectsContained);
	
	FilenameLister *sal = new FilenameLister(m_fgPageRoot, m_as, 0);

	sal->SetWidth(120);
	sal->SetHeight(200);
}

void ContainsPropPage::CollectContainedFiles()
{
	String sExt = m_obj->fnObj.sExt;

	if (sExt == ".mpl")
	{
		MapList mpl(m_obj->fnObj);

		// Maplist bands are added as "display_name|filename"
		// this is needed for internal bands, because the display name
		// is not the filename and the filename does not have enough info
		// to get the proper display name.
		for (int i = mpl->iLower(); i <= mpl->iUpper(); i++)
		{
			const Map& mp = mpl[i];
			String s = mpl->sDisplayName(mp->fnObj);
			if (mp->fnObj.sSectionPostFix != "")
				s &= ".mpr";  // also display the icon for internal maps
			s &= "|";
			s &= mp->fnObj.sRelativeQuoted();
			m_as.push_back(s);
		}
	}
	else if (sExt == ".mpv")
	{
		MapView mpv(m_obj->fnObj);
		Array<String> as;
		mpv->DependencyNames(as);
		for (int i = 0; i < as.iSize(); ++i)
		{
			FileName fn = as[i];
			if (fCIStrEqual(".grf", fn.sExt) || fCIStrEqual(".csy", fn.sExt))
				continue;

			m_as.push_back(fn.sRelative());
		}
	}
	else if (sExt == ".ilo")
	{
		int iNr=0;
		ObjectInfo::ReadElement("Layout", "Items", m_obj->fnObj, iNr);
		for (int i = 1; i <= iNr; ++i)
		{
			String sWhere("Item %d", i);
			String sKey;				
			ObjectInfo::ReadElement("Layout", sWhere.scVal(), m_obj->fnObj, sKey);
			if (!fCIStrEqual(sKey, "MapView"))  // Only MapViews wanted
				continue;

			FileName fnFile; // Get name of MapView
			ObjectInfo::ReadElement(sWhere.scVal(), sKey.scVal(), m_obj->fnObj, fnFile);
			if (IlwisObject::iotObjectType( fnFile) != IlwisObject::iotANY)
				m_as.push_back(fnFile.sRelative());
		}
	}
	else if (sExt == ".ioc")
	{
		int iNr = 0;
		ObjectInfo::ReadElement("ObjectCollection", "NrObjects", m_obj->fnObj, iNr);
		for (int i = 0; i < iNr; ++i)
		{
			String sWhere("Object%d", i);
			FileName fnFile; // Get name of Object
			ObjectInfo::ReadElement("ObjectCollection", sWhere.scVal(), m_obj->fnObj, fnFile);

			if (IlwisObject::iotObjectType( fnFile) != IlwisObject::iotANY)
				m_as.push_back(fnFile.sRelative());
		}
	}
	else if (sExt == ".grh")
	{
		Array<String> as;
		m_obj->DependencyNames(as);
		for (int i = 0; i < as.iSize(); ++i)
		{
			FileName fn = as[i];
			m_as.push_back(fn.sRelative());
		}
	}
	else if (sExt == ".stp")
	{
		//StereoPairPtr stpairPtr(m_obj->fnObj, false);
		StereoPair stpair(m_obj->fnObj);
		if (stpair.fValid())
		{
			if (stpair->mapLeft.fValid())
			{
				FileName fnL(stpair->mapLeft->fnObj);
				m_as.push_back(fnL.sRelative());
			}
			if (stpair->mapRight.fValid())
			{
				FileName fnR(stpair->mapRight->fnObj);
				m_as.push_back(fnR.sRelative());
			}
		}
	}
	else if (sExt == ".smc")
	{
		ObjectStructure osStruct;
		osStruct.GetAssociatedFiles(true); // to get grf, csy, etc... all that would be copied
		m_obj->GetObjectStructure(osStruct);

		list<String> lstFiles;
		osStruct.GetUsedFiles(lstFiles, false);
		for (list<String>::iterator it = lstFiles.begin(); it != lstFiles.end(); ++it)
		{
			FileName fnFile (*it);
			if (IlwisObject::iotObjectType(fnFile) != IlwisObject::iotANY && fnFile != m_obj->fnObj) // filter out .mp# etc. and ourselves!!
				m_as.push_back(fnFile.sRelative());
		}
	}
}

//-------------------------------
// ContainedByPropPage member functions
ContainedByPropPage::ContainedByPropPage(const IlwisObject& obj)
	: BasicPropertyFormPage(obj, SMSPropContainedBy)
{
}

void ContainedByPropPage::BuildPage()
{
	BasicPropertyFormPage::BuildPage();

	CollectOwners();

	StaticText* st = new StaticText(m_fgPageRoot, SMSUiContainedBy);
	
	FilenameLister *sal = new FilenameLister(m_fgPageRoot, m_as, 0);

	sal->SetWidth(120);
	sal->SetHeight(200);
}

void ContainedByPropPage::CollectOwners()
{
	int iNr = 0;
	ObjectInfo::ReadElement("Collection", "NrOfItems", m_obj->fnObj, iNr);
	if (iNr < 0)
		iNr = 0;

	for(int i = 0; i < iNr; ++i)
	{
		FileName fnItem;
		String sEntry("Item%d", i);
		ObjectInfo::ReadElement("Collection", sEntry.scVal(), m_obj->fnObj, fnItem);
		m_as.push_back(fnItem.sRelative());
	}

}

//-------------------------------
// HelpPropPage member functions
HelpPropPage::HelpPropPage(const FileName& fnObj)
	: FormBasePropertyPage(SMSPropHelp), m_hhc(0),
	m_sExt(fnObj.sExt)
{
}

HelpPropPage::~HelpPropPage()
{
	delete m_hhc;
}

BOOL HelpPropPage::OnSetActive()
{
	if (!FormBasePropertyPage::OnSetActive())
		return FALSE;

	if (0 == m_hhc)
	{
		m_iTopic = 0;
		CRect rct;
		GetClientRect(&rct);
		m_hhc = new HelpCtrl;
		m_hhc->Create(this, rct, 0, "main");
	}
	if (0 == m_iTopic)
	{
		if (!m_hhc->SetChmFile("ilwis.chm"))
			return FALSE;

		if (".mpr" == m_sExt)      m_iTopic = htpPopupMPR;
		else if (".mpa" == m_sExt) m_iTopic = htpPopupMPA;
		else if (".mps" == m_sExt) m_iTopic = htpPopupMPS;
		else if (".mpp" == m_sExt) m_iTopic = htpPopupMPP;
		else if (".mpl" == m_sExt) m_iTopic = htpPopupMPL;
		else if (".tbt" == m_sExt) m_iTopic = htpPopupTBT;
		else if (".ta2" == m_sExt) m_iTopic = htpPopupTA2;
		else if (".dom" == m_sExt) m_iTopic = htpPopupDOM;
		else if (".rpr" == m_sExt) m_iTopic = htpPopupRPR;
		else if (".grf" == m_sExt) m_iTopic = htpPopupGRF;
		else if (".csy" == m_sExt) m_iTopic = htpPopupCSY;
		else if (".mpv" == m_sExt) m_iTopic = htpPopupMPV;
		else if (".his" == m_sExt) m_iTopic = htpPopupHIS;
		else if (".hsa" == m_sExt) m_iTopic = htpPopupHSA;
		else if (".hss" == m_sExt) m_iTopic = htpPopupHSS;
		else if (".hsp" == m_sExt) m_iTopic = htpPopupHSP;
		else if (".sms" == m_sExt) m_iTopic = htpPopupSMS;
		else if (".mat" == m_sExt) m_iTopic = htpPopupMAT;
		else if (".grh" == m_sExt) m_iTopic = htpPopupGRH;
		else if (".fil" == m_sExt) m_iTopic = htpPopupFIL;
		else if (".fun" == m_sExt) m_iTopic = htpPopupFUN;
		else if (".isl" == m_sExt) m_iTopic = htpPopupISL;
		else if (".atx" == m_sExt) m_iTopic = htpPopupATX;
		else if (".ioc" == m_sExt) m_iTopic = htpPopupIOC;
		else if (".ilo" == m_sExt) m_iTopic = htpPopupILO;
		else if (".grh" == m_sExt) m_iTopic = htpPopupGRH;
		else if (".stp" == m_sExt) m_iTopic = htpPopupSTP;
		else if (".smc" == m_sExt) m_iTopic = htpPopupSMC;
		m_hhc->NavigateChm(m_iTopic);
	}

	return TRUE;
}

//-------------------------------
// Display the property form
UINT ShowPropForm(LPVOID lpObjectStruct)
{
	::OleInitialize(NULL);
	CWinThread* thr = AfxGetThread();
	thr->m_pMainWnd = 0;
	IlwWinApp()->Context()->InitThreadLocalVars();

	// ForeignFormat via GeoGateway do not need to be loaded for property forms
	bool *pfDoNotLoad = new bool(true);
	IlwWinApp()->Context()->SetThreadLocalVar(IlwisAppContext::tlvDONOTLOADGDB, pfDoNotLoad);

	ObjectStruct* os = (ObjectStruct*)lpObjectStruct;
	IlwisObject& obj = os->obj;
	FileName fn = obj->fnObj;
	IlwWinApp()->SetCurDir(fn.sPath());

	String sExt = obj->fnObj.sExt;

	FormBasePropertyPage *pgObject = 0;
	GeneralPropPage      *gpp = 0;
	DependsOnPropPage    *dop = 0;
	UsedByPropPage       *ubp = 0;
	ContainsPropPage     *cop = 0;
	ContainedByPropPage  *cbp = 0; 
	AdditionalInfoPage   *aip = 0;
	HelpPropPage         *hpp = 0;

	String sTitle(SMSPropPropertiesOf_S.scVal(), obj->sTypeName());

// The following lines try to activate an open property form, to avoid opening
// a second property form for the same object. However, although the proper window gets
// activated, the window from which this was tried reappears as the top window instead.
// Needs some research.
//	CWnd* wnd = CWnd::FindWindow(0, sTitle.scVal());
//	if (0 != wnd)    // property window already open, activate it
//	{
//		wnd->SetWindowPos(&CWnd::wndTop, 0, 0,0,0,SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOSIZE);
//
//		return 0;
//	}
//
	IlwisPropertySheet ps(sTitle);

	// Object type tab follows
	IlwisObject::iotIlwisObjectType iotObj = IlwisObject::iotObjectType(obj->fnObj);

	int htp;
	switch (iotObj)
	{
		case IlwisObject::iotRASMAP:
			pgObject = new MapPropPage(obj);
			break;
		case IlwisObject::iotPOLYGONMAP:
			pgObject = new PolygonMapPropPage(obj);
			break;
		case IlwisObject::iotSEGMENTMAP:
			pgObject = new SegmentMapPropPage(obj);
			break;
		case IlwisObject::iotPOINTMAP:
			pgObject = new PointMapPropPage(obj);
			break;
		case IlwisObject::iotTABLE:
			pgObject = new TablePropPage(obj);
			break;
		case IlwisObject::iotTABLE2DIM:
			pgObject = new Table2DimPropPage(obj);
			break;
		case IlwisObject::iotHISTRAS:
			pgObject = new HistogramPropPage(obj);
			break;
		case IlwisObject::iotHISTPOL:
			pgObject = new HistogramPropPage(obj);
			break;
		case IlwisObject::iotHISTSEG:
			pgObject = new HistogramPropPage(obj);
			break;
		case IlwisObject::iotHISTPNT:
			pgObject = new HistogramPropPage(obj);
			break;
		case IlwisObject::iotOBJECTCOLLECTION:
			htp = htpPropInfoObjectCollection;
			break;
		case IlwisObject::iotMAPLIST:
			pgObject = new MapListPropPage(obj);
			break;
		case IlwisObject::iotMAPVIEW:
			pgObject = new MapViewPropPage(obj);
			break;
		case IlwisObject::iotLAYOUT:
			htp = htpPropInfoLayout;
			break;
		case IlwisObject::iotCOORDSYSTEM:
			pgObject = new CoordSystemPropPage(obj);
			break;
		case IlwisObject::iotGEOREF:
			pgObject = new GeoRefPropPage(obj);
			break;
		case IlwisObject::iotDOMAIN:
			pgObject = new DomainPropPage(obj);
			break;
		case IlwisObject::iotREPRESENTATION:
			pgObject = new RepresentationPropPage(obj);
			break;
		case IlwisObject::iotFILTER:
			htp = htpPropInfoFilter;
			break;
		case IlwisObject::iotSCRIPT:
			htp = htpPropInfoScript;
			break;
		case IlwisObject::iotFUNCTION:
			htp = htpPropInfoFunction;
			break;
		case IlwisObject::iotMATRIX:
			htp = htpPropInfoMatrix;
			break;
		case IlwisObject::iotSAMPLESET:
			pgObject = new SampleSetPropPage(obj);
			break;
		case IlwisObject::iotANNOTATIONTEXT:
			htp = htpPropInfoAnnotationText;
			break;
    case IlwisObject::iotGRAPH:      
			htp = htpPropInfoGraph;
			break;
    case IlwisObject::iotSTEREOPAIR:      
			pgObject = new StereoPairPropPage(obj);
			break;
    case IlwisObject::iotCRITERIATREE:      
			htp = htpPropInfoSMCE;
			break;
		case IlwisObject::iotANY:
			break;
	}

	if (pgObject)
	{
		pgObject->create();
		ps.AddPage(pgObject);
		htp = pgObject->htp().iTopic;
	}

	// General tab page follows
	gpp = new GeneralPropPage(obj);
	gpp->SetMenHelpTopic(htp);
	gpp->create();
	ps.AddPage(gpp);

	// Dependency tab page follows
	if (obj->fDependent()) 
	{
		dop = new DependsOnPropPage(obj);
		dop->SetMenHelpTopic(htp);
		dop->create();
		ps.AddPage(dop);
	}

	// Contains page
	String sType;
	ObjectInfo::ReadElement("Ilwis", "Type", obj->fnObj, sType);
	// perform extra check for maplist necessary to prevent showing contains page for map
	if ((".mpl" == sExt && sType == "MapList") ||
		".ilo" == sExt ||
		".mpv" == sExt ||
		".ioc" == sExt ||
		".grh" == sExt ||
		".stp" == sExt ||
		".smc" == sExt)
	{
		cop = new ContainsPropPage(obj);
		cop->SetMenHelpTopic(htp);
		cop->create();
		ps.AddPage(cop);
	}

	int iNr = 0;
	ObjectInfo::ReadElement("Collection", "NrOfItems", obj->fnObj, iNr);
	if (iNr > 0)
	{
		cbp = new ContainedByPropPage(obj);
		cbp->SetMenHelpTopic(htp);
		cbp->create();
		ps.AddPage(cbp);
	}

	// Used By tab page follows
	if (
		".ilo" != sExt && 
		".isl" != sExt && 
		".his" != sExt && 
		".hsa" != sExt && 
		".hss" != sExt && 
		".hsp" != sExt &&  
		".fun" != sExt && 
		".grh" != sExt &&
		".smc" != sExt)
	{
		ubp = new UsedByPropPage(obj);
		ubp->SetMenHelpTopic(htp);
		ubp->create();
		ps.AddPage(ubp);
	}

	// AdditionalInfo tab page follows
	if (obj->fAdditionalInfo())
	{
		aip = new AdditionalInfoPage(obj);
		aip->SetMenHelpTopic(htp);
		aip->create();
		ps.AddPage(aip);
	}

	// Info tab page follows
	hpp = new HelpPropPage(obj->fnObj);
	hpp->SetMenHelpTopic(htp);
	hpp->create();
	ps.AddPage(hpp);

	switch (os->eRequest)
	{
		case ObjectStruct::ePROP:
			if (pgObject)
				ps.SetActivePage(pgObject);
			else
				ps.SetActivePage(gpp);
			break;
		case ObjectStruct::eHELP:
			ps.SetActivePage(hpp);
			break;
	}
	ps.DoModal();
	if (obj->fChanged)
		obj->Store();

	delete gpp;
	delete dop;
	delete ubp;
	delete aip;
	delete cop;
	delete cbp;
	delete hpp;

	String sCmd;
	if (iotObj == IlwisObject::iotDOMAIN)
	{
		// This is necessary to properly execute the domain convert functions (ID to Class etc)
		// These functions require that iRef is set to one (SetPointer requirement). This can only 
		// be achieved by first closing the property form and then executing the command.
		sCmd = ((DomainPropPage*)pgObject)->sDomainConvertCommand();
	}

	delete pgObject;
	delete os;

	// If a domain conversion was issued, start it here; All object
	// references from the property form have been removed.
	if (sCmd.length() > 0)
		IlwWinApp()->Execute(sCmd);

	::OleUninitialize();
	IlwWinApp()->Context()->RemoveThreadLocalVars();

	return 0;
}

//-------------------------------
// StereoPairPropPage member functions
StereoPairPropPage::StereoPairPropPage(const IlwisObject& obj)
	: BasicPropertyFormPage(obj, SMSPropStereoPair)
{
	SetMenHelpTopic(htpPropInfoStereoPair);
	m_stPyramids = 0;
	pbPyramids = 0;
}

void StereoPairPropPage::BuildPage()
{
	BasicPropertyFormPage::BuildPage();
	
	StereoPair stp(m_obj->fnObj);
	
	if (stp->fCalculated())
	{
		new FieldObjShow(m_fgPageRoot, stp->mapLeft->gr()->cs());
		
		new InfoText(m_fgPageRoot, SMSUiMapsDomain);
		Domain dom1 = stp->mapLeft->dm();
		new FieldObjShow(m_fgPageRoot, dom1);
	}
	
	
	String s('X', 55);
	m_stRemark = new InfoText(m_fgPageRoot, s);
	m_stRemark->SetVal(String());   // clear remark string
	
	iOption = stp->fUseGeorefLeft ? 0 : 1;
	rg = new RadioGroup(m_fgPageRoot, String(), &iOption, true);
	RadioButton* rbLeft = new RadioButton(rg, "Use Left Georef");    
	RadioButton* rbRight = new RadioButton(rg, "Use Right Georef in Anaglyph");  
	if (!stp->fDependent() || (stp->fDependent() && stp->fCalculated()))
	{
		String sPyrText, sPyrButton;
		if (stp->fHasPyramidFiles())
		{
			sPyrText = SMSPropMPLPyrAvailable;
			sPyrButton = SMSPropRemovePyr;
		}
		else
		{
			sPyrText = SMSPropMPLPyrNotAvailable;
			sPyrButton = SMSPropCreatePyr;
		}
		// Force the width of the statictext to the length of the longest possible string
		// And after that set the correct text
		m_stPyramids = new StaticText(m_fgPageRoot, SMSPropMPLPyrNotAvailable);
		//m_stPyramids->SetIndependentPos(); 
		m_stPyramids->Align(rg, AL_UNDER);
		m_stPyramids->SetVal(sPyrText);
		if ( !stp->fReadOnly())
		{
			pbPyramids = new PushButton(m_fgPageRoot, sPyrButton, (NotifyProc)&StereoPairPropPage::CallBackPyramids);
			pbPyramids->Align(m_stPyramids, AL_AFTER);
		}
	}		   
}
	
int StereoPairPropPage::CallBackPyramids(Event *)
{
	StereoPair stp(m_obj->fnObj);	
	if ( stp->fHasPyramidFiles() )
	{
		stp->DeletePyramidFiles();
	}		
	else
	{
		stp->CreatePyramidLayers();
		stp->Store();
	}

	// Adjust the text of static text and the button to reflect the current pyramid status
	// It is not combined above, because if for some reason the removal/creation fails
	// we still display the correct information, by checking afterwards
	String sPyrText, sPyrButton;
	if (stp->fHasPyramidFiles())
	{
		sPyrText = SMSPropMPLPyrAvailable;
		sPyrButton = SMSPropRemovePyr;
	}
	else
	{
		sPyrText = SMSPropMPLPyrNotAvailable;
		sPyrButton = SMSPropCreatePyr;
	}
	if (pbPyramids)
		pbPyramids->SetText(sPyrButton);
	m_stPyramids->SetVal(sPyrText);

	return 1;	
}
	
int StereoPairPropPage::exec()
{
	BasicPropertyFormPage::exec();
	//rg->StoreData();
	
	StereoPairPtr* stpp = dynamic_cast<StereoPairPtr*>(m_obj.pointer());
	if (iOption == 0)
		stpp->fUseGeorefLeft = true;
	else
		stpp->fUseGeorefLeft = false;
	stpp->Store();
	//if (!stpp)
	//	return 0;
	return 0;
}
